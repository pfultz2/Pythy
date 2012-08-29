Pythy
=====

Having it all now: Pythy syntax for C++11. 

Overview
--------

In a [blog post](http://cpp-next.com/archive/2011/11/having-it-all-pythy-syntax/), David Abrahams discussed an improved function syntax based around polymorphic lambdas:
```c++
[]min(x, y)
{ return x < y ? x : y; }
```
The pythy library implements this in C++11 using a macro like this:
```c++
PYTHY(min, x, y)
( return x < y ? x : y; )
```
Internally, it uses references for each of the parameters. To make each parameter const, the const keyword can be written:
```c++
PYTHY(min, const x, const y)
( return x < y ? x : y; )
```
If mutablility needs to be forced, the `mutable` keyword can be used:
```c++
PYTHY(add, mutable x, y)
(
    x += y;
)
```
References can't be used, since a reference is already being used internally. 

This macro is much more powerful than using the simple `RETURNS` macro: 
```c++
#define RETURNS(...) -> decltype(__VA_ARGS__) { return (__VA_ARGS__); }
template<class T, class U>
auto min(T x, U y) RETURNS(x < y ? x : y)
```
A multi-statement function can be written:
```c++
PYTHY(first, r)
(
    if (r.empty()) throw "The range is empty";
    return *(begin(r));
)
```
Or a function that returns lambda can be written:
```c++
PYTHY(equal_to, x)
(
    return [=](decltype(x) y) { return x == y; }
)
```
Which can not be done using the `RETURNS` macro.

Implementation
--------------

This is implemented using a templated lambda. Lambdas can't be templated locally(and neither can classes), but we can make a lambda depend on a template parameter. So, if we define a lambda inside a templated class, like this:
```c++
template<class T0, class T1>
struct min_t
{
    constexpr static auto f = [](T0 x, T1 y){ return x < y ? x : y; }; 
};
```
This won't work, because a lambda closure(even if its non-capturing) is not a constexpr. However, using a trick(as suggested by Richard Smith from clang) we can initialize it as a null pointer:
```c++
template<typename T> 
typename std::remove_reference<T>::type *addr(T &&t) 
{ 
    return &t; 
}
template<class T0, class T1>
struct min_t
{
    constexpr static auto *f = false ? addr([](T0 x, T1 y){ return x < y ? x : y; }) : nullptr; 
};
```
Then when we want to call our function, we can do this:
```c++
template<class T0, class T1>
auto min(T0 x, T1 x) RETURNS((*min_t<T0, T1>::f(x, y)))
```
It appears that we are derefencing a null pointer. Remember in C++ when dereferencing a null pointer, undefined behavior occurs when there is an rvalue-to-lvalue conversion. However, since a non-capturing lambda closure is almost always implemented as an object with no members, undefined behavior never occurs, since it won't access any of its members. Its highly unlikely that a non-capturing lambda closure could be implemented another way since it must be convertible to a function pointer. But perhaps not? 

Requirements
------------

This requires a compiler that supports `auto`, `constexpr` and lambdas. It also relies on boost. For some compilers the `-DBOOST_PP_VARIADICS=1` must be passed into the compiler.