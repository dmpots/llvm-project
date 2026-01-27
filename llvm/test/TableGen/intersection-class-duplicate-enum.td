// RUN: llvm-tblgen -gen-register-info -I %p/../../include %s -o - | FileCheck %s
//
// This test demonstrates a TableGen bug where duplicate enum definitions are
// generated for register class intersections when the name-flattening
// optimization produces identical names for two different register sets.
//
// XFAIL: *
//
// The bug causes C++ compilation errors due to duplicate enum values.
// When fixed, TableGen should generate distinct names that preserve the full
// path information to avoid collisions.

include "llvm/Target/Target.td"

def TestInstrInfo : InstrInfo;
def TestTarget : Target {
  let InstructionSet = TestInstrInfo;
}

//===----------------------------------------------------------------------===//
// Simple register definitions for 16 32-bit registers (R0-R16).
//===----------------------------------------------------------------------===//

// Define 16 simple 32-bit registers.
foreach i = 0...15 in {
  def R#i : Register<"r"#i>;
}

// Define sub-register indices for 96-bit and 128-bit tuples.
def sub0 : SubRegIndex<32>;
def sub1 : SubRegIndex<32, 32>;
def sub2 : SubRegIndex<32, 64>;
def sub3 : SubRegIndex<32, 96>;

//===----------------------------------------------------------------------===//
// Define 3x32b and 4x32b registers using the RegisterTuples helper.
//===----------------------------------------------------------------------===//

// Define consecutive 96-bit (3-register) tuples (e.g. R0_R1_R2).
def Tuples3 : RegisterTuples<[sub0, sub1, sub2],
                              [(add (sequence "R%u", 0, 13)),
                               (add (sequence "R%u", 1, 14)),
                               (add (sequence "R%u", 2, 15))]>;

// Define consecutive 128-bit (4-register) tuples (e.g. R0_R1_R2_R3).
def Tuples4 : RegisterTuples<[sub0, sub1, sub2, sub3],
                              [(add (sequence "R%u", 0, 12)),
                               (add (sequence "R%u", 1, 13)),
                               (add (sequence "R%u", 2, 14)),
                               (add (sequence "R%u", 3, 15))]>;

//===----------------------------------------------------------------------===//
// Define the register classes.
//===----------------------------------------------------------------------===//

// A 32-bit base class with all registers (0-15).
def R32_All : RegisterClass<"TestTarget", [i32], 32,
  (sequence "R%u", 0, 15)>;

// A restricted 32-bit class - excludes R0 to create intersection differences.
def R32_Restricted : RegisterClass<"TestTarget", [i32], 32,
  (sequence "R%u", 1, 15)>;

// A 96-bit class with spacing=4, positions (0, 4, 8, 12).
// Decimates all consecutive tuples by 4 to get every 4th starting position.
//
// IMPORTANT: The name "R96_WithAligned4" is critical for bug reproduction!
// Renaming to shorter "R96_Aligned4" causes TableGen to take a different code
// path that avoids the bug by generating compound "_and_" names instead of
// duplicate flattened names. This reveals the bug's sensitivity to class
// naming.
def R96_WithAligned4 : RegisterClass<"TestTarget", [i32], 32,
  (decimate Tuples3, 4)> {
  let Size = 96;
}

// A 96-bit class with spacing=8, positions (0, 8).
// This is a subset of R96_WithAligned4.
def R96_Aligned8 : RegisterClass<"TestTarget", [i32], 32,
  (decimate Tuples3, 8)> {
  let Size = 96;
}

// A 128-bit class with spacing=4, positions (0, 4, 8, 12).
// Decimates all tuples by 4 to get every 4th starting position.
// This is the key class that will create intersections via the sub0_sub1_sub2
// index.
def R128_Aligned4 : RegisterClass<"TestTarget", [i32], 32,
  (decimate Tuples4, 4)> {
  let Size = 128;
}

//===----------------------------------------------------------------------===//
// Bug Verification:
//
// The bug manifests as duplicate enum names for different register sets.
// We check that R128_Aligned4_with_sub0_in_R32_RestrictedRegClassID appears
// only once in the enum. When the bug is present, it appears twice.
//
// CHECK: // Register classes
// CHECK-LABEL: enum {
// CHECK:     R128_Aligned4_with_sub0_in_R32_RestrictedRegClassID = {{[0-9]+}},
// CHECK-NOT: R128_Aligned4_with_sub0_in_R32_RestrictedRegClassID =
// CHECK: };
//
// When the bug is present, two distinct register sets both get the same name:
//   - R128_Aligned4_with_sub0_sub1_sub2_in_R96_WithAligned4_with_sub0_in_R32_Restricted
//   - R128_Aligned4_with_sub0_sub1_sub2_in_R96_Aligned8_with_sub0_in_R32_Restricted
// Both incorrectly flatten to: R128_Aligned4_with_sub0_in_R32_Restricted
//===----------------------------------------------------------------------===//
