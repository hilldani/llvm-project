// RUN: %clang_cc1 -emit-llvm -triple i386-linux-gnu %s -o - | FileCheck %s
// RUN: %clang_cc1 -x c++ -emit-pch -triple i386-linux-gnu -o %t %s
// RUN: %clang_cc1 -include-pch %t %s -triple i386-linux-gnu -emit-llvm -o - | FileCheck %s
// expected-no-diagnostics

#ifndef HEADER
#define HEADER

/// foo: declarations only

[[omp::assume("foo:before1")]] void foo();

[[omp::assume("foo:before2")]]
[[omp::assume("foo:before3")]] void
foo();

/// baz: static function declarations and a definition

[[omp::assume("baz:before1")]] static void baz();

[[omp::assume("baz:before2")]]
[[omp::assume("baz:before3")]] static void
baz();

// Definition
[[omp::assume("baz:def1,baz:def2")]] static void baz() { foo(); }

[[omp::assume("baz:after")]] static void baz();

/// bar: external function declarations and a definition

[[omp::assume("bar:before1")]] void bar();

[[omp::assume("bar:before2")]]
[[omp::assume("bar:before3")]] void
bar();

// Definition
[[omp::assume("bar:def1,bar:def2")]] void bar() { baz(); }

[[omp::assume("bar:after")]] void bar();

/// back to foo

[[omp::assume("foo:after")]] void foo();

/// class tests
class C {
  [[omp::assume("C:private_method")]] void private_method();
  [[omp::assume("C:private_static")]] static void private_static();

public:
  [[omp::assume("C:public_method1")]] void public_method();
  [[omp::assume("C:public_static1")]] static void public_static();
};

[[omp::assume("C:public_method2")]] void C::public_method() {
  private_method();
}

[[omp::assume("C:public_static2")]] void C::public_static() {
  private_static();
}

/// template tests
template <typename T>
[[omp::assume("template_func<T>")]] void template_func() {}

template <>
[[omp::assume("template_func<float>")]] void template_func<float>() {}

template <>
void template_func<int>() {}

template <typename T>
struct S {
  [[omp::assume("S<T>::method")]] void method();
};

template <>
[[omp::assume("S<float>::method")]] void S<float>::method() {}

template <>
void S<int>::method() {}

// CHECK:         define{{.*}} void @_Z3barv() #0
// CHECK:         define{{.*}} void @_ZL3bazv() #1
// CHECK:         define{{.*}} void @_ZN1C13public_methodEv({{.*}}) #2
// CHECK:         declare{{.*}} void @_ZN1C14private_methodEv({{.*}}) #3
// CHECK:         define{{.*}} void @_ZN1C13public_staticEv() #4
// CHECK:         declare{{.*}} void @_ZN1C14private_staticEv() #5
// CHECK:         define{{.*}} void @_Z13template_funcIfEvv() #6
// CHECK:         define{{.*}} void @_Z13template_funcIiEvv() #7
// CHECK:         define{{.*}} void @_ZN1SIfE6methodEv({{.*}}) #8
// CHECK:         define{{.*}} void @_ZN1SIiE6methodEv({{.*}}) #9
// CHECK:         declare{{.*}} void @_Z3foov() #10
// CHECK:         attributes #0
// CHECK-SAME:      "llvm.assume"="bar:before1,bar:before2,bar:before3,bar:def1,bar:def2"
// CHECK:         attributes #1
// CHECK-SAME:      "llvm.assume"="baz:before1,baz:before2,baz:before3,baz:def1,baz:def2,baz:after"
// CHECK:         attributes #2
// CHECK-SAME:      "llvm.assume"="C:public_method1,C:public_method2"
// CHECK:         attributes #3
// CHECK-SAME:      "llvm.assume"="C:private_method"
// CHECK:         attributes #4
// CHECK-SAME:      "llvm.assume"="C:public_static1,C:public_static2"
// CHECK:         attributes #5
// CHECK-SAME:      "llvm.assume"="C:private_static"
// CHECK:         attributes #6
// CHECK-SAME:      "llvm.assume"="template_func<T>,template_func<float>"
// CHECK:         attributes #7
// CHECK-SAME:      "llvm.assume"="template_func<T>"
// CHECK:         attributes #8
// CHECK-SAME:      "llvm.assume"="S<T>::method,S<float>::method"
// CHECK:         attributes #9
// CHECK-SAME:      "llvm.assume"="S<T>::method"
// CHECK:         attributes #10
// CHECK-SAME:      "llvm.assume"="foo:before1,foo:before2,foo:before3"

#endif
