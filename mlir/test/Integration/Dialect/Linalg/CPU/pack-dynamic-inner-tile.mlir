// DEFINE: %{compile} =  mlir-opt %s \
// DEFINE:  -transform-interpreter -test-transform-dialect-erase-schedule |\
// DEFINE: mlir-opt \
// DEFINE:  -test-lower-to-llvm -o %t
// DEFINE: %{entry_point} = main
// DEFINE: %{run} = mlir-runner %t -e %{entry_point} -entry-point-result=void \
// DEFINE:    -shared-libs=%mlir_runner_utils,%mlir_c_runner_utils

// RUN: rm -f %t && %{compile} && %{run} | FileCheck %s

/// End-to-end test for linalg.pack where one of the inner tile sizes is
/// dynamic.

func.func @main() {
  // Allocate and initialise the inputs
  %A_alloc = tensor.empty() : tensor<7x16xi32>

  %A = arith.constant dense<[
    [ 1,  8, 15, 22, 29, 36, 43, 50, 57, 64, 71, 78, 85, 92, 99 , 106],
    [ 2,  9, 16, 23, 30, 37, 44, 51, 58, 65, 72, 79, 86, 93, 100, 107],
    [ 3, 10, 17, 24, 31, 38, 45, 52, 59, 66, 73, 80, 87, 94, 101, 108],
    [ 4, 11, 18, 25, 32, 39, 46, 53, 60, 67, 74, 81, 88, 95, 102, 109],
    [ 5, 12, 19, 26, 33, 40, 47, 54, 61, 68, 75, 82, 89, 96, 103, 110],
    [ 6, 13, 20, 27, 34, 41, 48, 55, 62, 69, 76, 83, 90, 97, 104, 111],
    [ 7, 14, 21, 28, 35, 42, 49, 56, 63, 70, 77, 84, 91, 98, 105, 112]
  ]> : tensor<7x16xi32>

  func.call @pack(%A) : (tensor<7x16xi32>) -> ()

  return
}

func.func private @pack(%A: tensor<7x16xi32>) {
  %c1 = arith.constant 1 : index
  %pad_val = arith.constant 123 : i32

  // Dynamic tile size
  %tile_size = arith.constant 8 : index
  %A_pack_empty = tensor.empty(%c1, %tile_size) : tensor<?x16x?x1xi32>

  %A_pack = linalg.pack %A
    padding_value(%pad_val : i32)
    inner_dims_pos = [0, 1]
    inner_tiles = [%tile_size, 1]
    into %A_pack_empty : tensor<7x16xi32> -> tensor<?x16x?x1xi32>
  %A_cast = tensor.cast %A_pack : tensor<?x16x?x1xi32> to tensor<*xi32>

  // Print the results
  // CHECK: Unranked Memref base@ = 0x{{.*}} rank = 4 offset = 0 sizes = [1, 16, 8, 1] strides = [128, 8, 1, 1] data =
  // Tile 1: (8 x 1)
  // CHECK-NEXT:  1
  // CHECK-NEXT:  2
  // CHECK-NEXT:  3
  // CHECK-NEXT:  4
  // CHECK-NEXT:  5
  // CHECK-NEXT:  6
  // CHECK-NEXT:  7
  // Expect pad value after 7 elements
  // CHECK-NEXT:  123
  // Tile 2: (8 x 1)
  // CHECK-NEXT:  8
  // CHECK-NEXT:  9
  // CHECK-NEXT:  10
  // CHECK-NEXT:  11
  // CHECK-NEXT:  12
  // CHECK-NEXT:  13
  // CHECK-NEXT:  14
  // Expect pad value after further 7 elements
  // CHECK-NEXT:  123
  // Tile 3: (8 x 1)
  // CHECK-NEXT:  15
  // CHECK-NEXT:  16
  // ...
  call @printMemrefI32(%A_cast) : (tensor<*xi32>) -> ()

  return
}

module @transforms attributes { transform.with_named_sequence } {
  transform.named_sequence @__transform_main(%module: !transform.any_op {transform.consume}) {
    %pack = transform.structured.match ops{["linalg.pack"]} in %module : (!transform.any_op) -> !transform.any_op

    // 1. Tile so that we can decompose linalg.pack into tensor.pad and other
    // Ops (see step 2)
    %tiled_pack_op_p, %loops:2 = transform.structured.tile_using_for %pack tile_sizes [1, 1]
       : (!transform.any_op) -> (!transform.any_op, !transform.any_op, !transform.any_op)

    // 2. Decompose the tiled pack Op into (trimmed for brevity):
    //
    //  %padded = tensor.pad %slice_of_A (..) :
    //      tensor<?x?xi32> to tensor<8x1xi32>
    //  %inserted_slice = tensor.insert_slice %padded into %slice_of_A_pack (...) :
    //      tensor<8x1xi32> into tensor<1x1x?x1xi32>
    //
    // (NOTE: no tile is transposed, hence no linalg.transpose)
    //
    // This is followed by this decomposition of the pad Op:
    //
    //  %c123_i32 = arith.constant 123 : i32
    //  %slice_of_A = tensor.extract_slice %A[%3, %arg3] [%4, %5] [1, 1] :
    //    tensor<7x16xi32> to tensor<?x?xi32>
    //  %empty = tensor.empty() : tensor<8x1xi32>
    //  %fill = linalg.fill ins(%c123_i32 : i32) outs(%empty :
    //    tensor<8x1xi32>) -> tensor<8x1xi32>
    //  %inserted_slice = tensor.insert_slice %slice_of_A into %fill[0, 0] [%4, %5] [1, 1] :
    //    tensor<?x?xi32> into tensor<8x1xi32>
    //
    %func_op = transform.get_parent_op %tiled_pack_op_p {isolated_from_above} : (!transform.any_op) -> !transform.op<"func.func">
    transform.apply_patterns to %func_op {
      transform.apply_patterns.linalg.decompose_pack_unpack
      transform.apply_patterns.linalg.decompose_pad
    } : !transform.op<"func.func">

    // 3. Vectorize linalg.fill.
    // Vector sizes match the inner tiles in the payload IR.
    %fill = transform.structured.match ops{["linalg.fill"]} in %func_op : (!transform.op<"func.func">) -> !transform.any_op
    transform.structured.vectorize %fill vector_sizes [8, 1] : !transform.any_op

    transform.apply_patterns to %func_op {
      transform.apply_patterns.tensor.fold_tensor_subset_ops
      transform.apply_patterns.canonicalization
    } : !transform.op<"func.func">

    // 4. Bufferize before lowering to LLVM
    %bufferize = transform.bufferization.one_shot_bufferize %module
      {bufferize_function_boundaries=true} : (!transform.any_op) -> !transform.any_op

    // 5. Deallocate buffers.
    %func_op_bufferized = transform.structured.match ops{["func.func"]} in %bufferize : (!transform.any_op) -> !transform.op<"func.func">
    %func_op_deallocated = transform.apply_registered_pass "buffer-deallocation-pipeline" to %func_op_bufferized
      : (!transform.op<"func.func">) -> !transform.op<"func.func">

    // 6. Canonicalize
    transform.apply_patterns to %func_op_deallocated {
      transform.apply_patterns.canonicalization
    } : !transform.op<"func.func">

    transform.yield
  }
}

func.func private @printMemrefI32(%ptr : tensor<*xi32>)
