; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=aarch64-- | FileCheck %s

define i64 @test_clear_mask_i64_i32(i64 %x) nounwind {
; CHECK-LABEL: test_clear_mask_i64_i32:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    cmn w0, #1
; CHECK-NEXT:    csel x0, x8, x0, gt
; CHECK-NEXT:    ret
entry:
  %a = and i64 %x, 2147483648
  %r = icmp eq i64 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i64 [ %x, %entry], [ 42, %t]
  ret i64 %ret
}

define i64 @test_set_mask_i64_i32(i64 %x) nounwind {
; CHECK-LABEL: test_set_mask_i64_i32:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst x0, #0x80000000
; CHECK-NEXT:    csel x0, x8, x0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i64 %x, 2147483648
  %r = icmp ne i64 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i64 [ %x, %entry], [ 42, %t]
  ret i64 %ret
}

define i64 @test_clear_mask_i64_i16(i64 %x) nounwind {
; CHECK-LABEL: test_clear_mask_i64_i16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst x0, #0x8000
; CHECK-NEXT:    csel x0, x8, x0, eq
; CHECK-NEXT:    ret
entry:
  %a = and i64 %x, 32768
  %r = icmp eq i64 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i64 [ %x, %entry], [ 42, %t]
  ret i64 %ret
}

define i64 @test_set_mask_i64_i16(i64 %x) nounwind {
; CHECK-LABEL: test_set_mask_i64_i16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst x0, #0x8000
; CHECK-NEXT:    csel x0, x8, x0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i64 %x, 32768
  %r = icmp ne i64 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i64 [ %x, %entry], [ 42, %t]
  ret i64 %ret
}

define i64 @test_clear_mask_i64_i8(i64 %x) nounwind {
; CHECK-LABEL: test_clear_mask_i64_i8:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst x0, #0x80
; CHECK-NEXT:    csel x0, x8, x0, eq
; CHECK-NEXT:    ret
entry:
  %a = and i64 %x, 128
  %r = icmp eq i64 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i64 [ %x, %entry], [ 42, %t]
  ret i64 %ret
}

define i64 @test_set_mask_i64_i8(i64 %x) nounwind {
; CHECK-LABEL: test_set_mask_i64_i8:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst x0, #0x80
; CHECK-NEXT:    csel x0, x8, x0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i64 %x, 128
  %r = icmp ne i64 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i64 [ %x, %entry], [ 42, %t]
  ret i64 %ret
}

define i32 @test_clear_mask_i32_i16(i32 %x) nounwind {
; CHECK-LABEL: test_clear_mask_i32_i16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x8000
; CHECK-NEXT:    csel w0, w8, w0, eq
; CHECK-NEXT:    ret
entry:
  %a = and i32 %x, 32768
  %r = icmp eq i32 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i32 [ %x, %entry], [ 42, %t]
  ret i32 %ret
}

define i32 @test_set_mask_i32_i16(i32 %x) nounwind {
; CHECK-LABEL: test_set_mask_i32_i16:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x8000
; CHECK-NEXT:    csel w0, w8, w0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i32 %x, 32768
  %r = icmp ne i32 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i32 [ %x, %entry], [ 42, %t]
  ret i32 %ret
}

define i32 @test_clear_mask_i32_i8(i32 %x) nounwind {
; CHECK-LABEL: test_clear_mask_i32_i8:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x80
; CHECK-NEXT:    csel w0, w8, w0, eq
; CHECK-NEXT:    ret
entry:
  %a = and i32 %x, 128
  %r = icmp eq i32 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i32 [ %x, %entry], [ 42, %t]
  ret i32 %ret
}

define i32 @test_set_mask_i32_i8(i32 %x) nounwind {
; CHECK-LABEL: test_set_mask_i32_i8:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x80
; CHECK-NEXT:    csel w0, w8, w0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i32 %x, 128
  %r = icmp ne i32 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i32 [ %x, %entry], [ 42, %t]
  ret i32 %ret
}

define i16 @test_clear_mask_i16_i8(i16 %x) nounwind {
; CHECK-LABEL: test_clear_mask_i16_i8:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x80
; CHECK-NEXT:    csel w0, w8, w0, eq
; CHECK-NEXT:    ret
entry:
  %a = and i16 %x, 128
  %r = icmp eq i16 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i16 [ %x, %entry], [ 42, %t]
  ret i16 %ret
}

define i16 @test_set_mask_i16_i8(i16 %x) nounwind {
; CHECK-LABEL: test_set_mask_i16_i8:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x80
; CHECK-NEXT:    csel w0, w8, w0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i16 %x, 128
  %r = icmp ne i16 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i16 [ %x, %entry], [ 42, %t]
  ret i16 %ret
}

define i16 @test_set_mask_i16_i7(i16 %x) nounwind {
; CHECK-LABEL: test_set_mask_i16_i7:
; CHECK:       // %bb.0: // %entry
; CHECK-NEXT:    mov w8, #42 // =0x2a
; CHECK-NEXT:    tst w0, #0x40
; CHECK-NEXT:    csel w0, w8, w0, ne
; CHECK-NEXT:    ret
entry:
  %a = and i16 %x, 64
  %r = icmp ne i16 %a, 0
  br i1 %r, label %t, label %f
t:
  br label %f
f:
  %ret = phi i16 [ %x, %entry], [ 42, %t]
  ret i16 %ret
}
