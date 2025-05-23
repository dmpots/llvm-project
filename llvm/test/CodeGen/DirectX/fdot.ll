; RUN: opt -S  -dxil-intrinsic-expansion -mtriple=dxil-pc-shadermodel6.3-library %s | FileCheck %s --check-prefixes=CHECK,EXPCHECK
; RUN: opt -S  -dxil-intrinsic-expansion -dxil-op-lower -mtriple=dxil-pc-shadermodel6.3-library %s | FileCheck %s --check-prefixes=CHECK,DOPCHECK

; Make sure dxil operation function calls for dot are generated for float type vectors.

; CHECK-LABEL: dot_half2
define noundef half @dot_half2(<2 x half> noundef %a, <2 x half> noundef %b) {
entry:
; CHECK: [[A1:%.*]] = extractelement <2 x half> %a, i32 0
; CHECK: [[A2:%.*]] = extractelement <2 x half> %a, i32 1
; CHECK: [[B1:%.*]] = extractelement <2 x half> %b, i32 0
; CHECK: [[B2:%.*]] = extractelement <2 x half> %b, i32 1
; DOPCHECK: call half @dx.op.dot2.f16(i32 54, half [[A1]], half [[A2]], half [[B1]], half [[B2]]) #[[#ATTR:]]
; EXPCHECK: call half @llvm.dx.dot2.f16(half [[A1]], half [[A2]], half [[B1]], half [[B2]])
  %dx.dot = call half @llvm.dx.fdot.v2f16(<2 x half> %a, <2 x half> %b)
  ret half %dx.dot
}

; CHECK-LABEL: dot_half3
define noundef half @dot_half3(<3 x half> noundef %a, <3 x half> noundef %b) {
entry:
; CHECK: extractelement <3 x half> %a, i32 0
; CHECK: extractelement <3 x half> %a, i32 1
; CHECK: extractelement <3 x half> %a, i32 2
; CHECK: extractelement <3 x half> %b, i32 0
; CHECK: extractelement <3 x half> %b, i32 1
; CHECK: extractelement <3 x half> %b, i32 2
; DOPCHECK: call half @dx.op.dot3.f16(i32 55, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}) #[[#ATTR]]
; EXPCHECK: call half @llvm.dx.dot3.f16(half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}})
  %dx.dot = call half @llvm.dx.fdot.v3f16(<3 x half> %a, <3 x half> %b)
  ret half %dx.dot
}

; CHECK-LABEL: dot_half4
define noundef half @dot_half4(<4 x half> noundef %a, <4 x half> noundef %b) {
entry:
; CHECK: extractelement <4 x half> %a, i32 0
; CHECK: extractelement <4 x half> %a, i32 1
; CHECK: extractelement <4 x half> %a, i32 2
; CHECK: extractelement <4 x half> %a, i32 3
; CHECK: extractelement <4 x half> %b, i32 0
; CHECK: extractelement <4 x half> %b, i32 1
; CHECK: extractelement <4 x half> %b, i32 2
; CHECK: extractelement <4 x half> %b, i32 3
; DOPCHECK: call half @dx.op.dot4.f16(i32 56, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}) #[[#ATTR]]
; EXPCHECK: call half @llvm.dx.dot4.f16(half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}}, half %{{.*}})
  %dx.dot = call half @llvm.dx.fdot.v4f16(<4 x half> %a, <4 x half> %b)
  ret half %dx.dot
}

; CHECK-LABEL: dot_float2
define noundef float @dot_float2(<2 x float> noundef %a, <2 x float> noundef %b) {
entry:
; CHECK: extractelement <2 x float> %a, i32 0
; CHECK: extractelement <2 x float> %a, i32 1
; CHECK: extractelement <2 x float> %b, i32 0
; CHECK: extractelement <2 x float> %b, i32 1
; DOPCHECK: call float @dx.op.dot2.f32(i32 54, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}) #[[#ATTR]]
; EXPCHECK: call float @llvm.dx.dot2.f32(float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}})
  %dx.dot = call float @llvm.dx.fdot.v2f32(<2 x float> %a, <2 x float> %b)
  ret float %dx.dot
}

; CHECK-LABEL: dot_float3
define noundef float @dot_float3(<3 x float> noundef %a, <3 x float> noundef %b) {
entry:
; CHECK: extractelement <3 x float> %a, i32 0
; CHECK: extractelement <3 x float> %a, i32 1
; CHECK: extractelement <3 x float> %a, i32 2
; CHECK: extractelement <3 x float> %b, i32 0
; CHECK: extractelement <3 x float> %b, i32 1
; CHECK: extractelement <3 x float> %b, i32 2
; DOPCHECK: call float @dx.op.dot3.f32(i32 55, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}) #[[#ATTR]]
; EXPCHECK: call float @llvm.dx.dot3.f32(float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}})
  %dx.dot = call float @llvm.dx.fdot.v3f32(<3 x float> %a, <3 x float> %b)
  ret float %dx.dot
}

; CHECK-LABEL: dot_float4
define noundef float @dot_float4(<4 x float> noundef %a, <4 x float> noundef %b) {
entry:
; CHECK: extractelement <4 x float> %a, i32 0
; CHECK: extractelement <4 x float> %a, i32 1
; CHECK: extractelement <4 x float> %a, i32 2
; CHECK: extractelement <4 x float> %a, i32 3
; CHECK: extractelement <4 x float> %b, i32 0
; CHECK: extractelement <4 x float> %b, i32 1
; CHECK: extractelement <4 x float> %b, i32 2
; CHECK: extractelement <4 x float> %b, i32 3
; DOPCHECK: call float @dx.op.dot4.f32(i32 56, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}) #[[#ATTR]]
; EXPCHECK: call float @llvm.dx.dot4.f32(float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}}, float %{{.*}})
  %dx.dot = call float @llvm.dx.fdot.v4f32(<4 x float> %a, <4 x float> %b)
  ret float %dx.dot
}

; DOPCHECK: attributes #[[#ATTR]] = {{{.*}} memory(none) {{.*}}}

declare half  @llvm.dx.fdot.v2f16(<2 x half> , <2 x half> )
declare half  @llvm.dx.fdot.v3f16(<3 x half> , <3 x half> )
declare half  @llvm.dx.fdot.v4f16(<4 x half> , <4 x half> )
declare float @llvm.dx.fdot.v2f32(<2 x float>, <2 x float>)
declare float @llvm.dx.fdot.v3f32(<3 x float>, <3 x float>)
declare float @llvm.dx.fdot.v4f32(<4 x float>, <4 x float>)
