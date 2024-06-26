; RUN: opt -S -passes=instcombine < %s | FileCheck %s
%S = type { i16, i32 }

define <2 x i16> @test1() {
entry:
  %c = icmp eq i16 extractelement (<2 x i16> bitcast (<1 x i32> <i32 1> to <2 x i16>), i32 0), 0
  %s = select i1 %c, %S zeroinitializer, %S { i16 0, i32 1 }
  %e = extractvalue %S %s, 0
  %b = insertelement <2 x i16> <i16 undef, i16 0>, i16 %e, i32 0
  ret <2 x i16> %b
}

; CHECK-LABEL: @test1(
; CHECK: ret <2 x i16> zeroinitializer
