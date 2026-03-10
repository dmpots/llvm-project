; RUN: llc < %s -mtriple=x86_64 -stop-after=finalize-isel -o - | FileCheck %s

; Verify that the MIR printer correctly handles the inteldialect flag
; on INLINEASM instructions (ExtraInfo bit decoding).
; CHECK: INLINEASM {{.*}}inteldialect
; CHECK: INLINEASM {{.*}}attdialect
define void @test_inteldialect() {
  call void asm sideeffect inteldialect "nop", ""()
  call void asm sideeffect "nop", ""()
  ret void
}
