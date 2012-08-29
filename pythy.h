/*=============================================================================
    Copyright (c) 2012 Paul Fultz II
    pythy.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/


#include <boost/preprocessor.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>
#include <type_traits>


#ifndef PYTHY_GUARD_H
#define PYTHY_GUARD_H

namespace pythy
{
    struct addr
    {
        template<typename T> 
        typename std::remove_reference<T>::type *operator+(T &&t) 
        { 
            return &t; 
        }
    };
}

#define PYTHY_RETURNS(...) -> decltype(__VA_ARGS__) { return (__VA_ARGS__); }

//
// PYTHY_IS_PAREN is used to detect if the first token is a parenthesis.
// It expands to 1 if it is, otherwise it expands to 0.
//
#define PYTHY_IS_PAREN(x) PYTHY_IS_PAREN_CHECK(PYTHY_IS_PAREN_PROBE x)
#define PYTHY_IS_PAREN_CHECK(...) PYTHY_IS_PAREN_CHECK_N(__VA_ARGS__,0)
#define PYTHY_IS_PAREN_PROBE(...) ~, 1,
#ifndef _MSC_VER
#define PYTHY_IS_PAREN_CHECK_N(x, n, ...) n
#else
// MSVC workarounds
#define PYTHY_IS_PAREN_CHECK_RES(x) x
#define PYTHY_IS_PAREN_CHECK_II(x, n, ...) n
#define PYTHY_IS_PAREN_CHECK_I(x) PYTHY_IS_PAREN_CHECK_RES(PYTHY_IS_PAREN_CHECK_II x)
#define PYTHY_IS_PAREN_CHECK_N(...) PYTHY_IS_PAREN_CHECK_I((__VA_ARGS__))
#endif

#define PYTHY_EXPAND(...) __VA_ARGS__
#define PYTHY_EAT(...)
#define PYTHY_REM(...) __VA_ARGS__
#define PYTHY_TAIL(x) PYTHY_EAT x
#define PYTHY_PAIR(x) PYTHY_REM x

#define PYTHY_KEYWORD(x) BOOST_PP_CAT(PYTHY_KEYWORD_, x)
#define PYTHY_KEYWORD_const (const&)
#define PYTHY_KEYWORD_mutable (&)

#define PYTHY_IS_KEYWORD(x) PYTHY_IS_PAREN(PYTHY_KEYWORD(x))

#define PYTHY_REMOVE_KEYWORD(x) PYTHY_REMOVE_KEYWORD_I(PYTHY_KEYWORD(x), x)
#define PYTHY_REMOVE_KEYWORD_I(keyed, orig) BOOST_PP_IIF(PYTHY_IS_PAREN(keyed), PYTHY_REMOVE_KEYWORD_KEY, PYTHY_REMOVE_KEYWORD_ORIG)(keyed, orig)
#define PYTHY_REMOVE_KEYWORD_KEY(keyed, orig) PYTHY_TAIL(keyed)
#define PYTHY_REMOVE_KEYWORD_ORIG(keyed, orig) orig

#define PYTHY_TRANSFORM_KEYWORD(x) PYTHY_TRANSFORM_KEYWORD_I(PYTHY_KEYWORD(x), x)
#define PYTHY_TRANSFORM_KEYWORD_I(keyed, orig) BOOST_PP_IIF(PYTHY_IS_PAREN(keyed), PYTHY_TRANSFORM_KEYWORD_KEY, PYTHY_TRANSFORM_KEYWORD_ORIG)(keyed, orig)
#define PYTHY_TRANSFORM_KEYWORD_KEY(keyed, orig) PYTHY_PAIR(keyed)
#define PYTHY_TRANSFORM_KEYWORD_ORIG(keyed, orig) && orig


#define PYHTY_TYPE_FROM_VAR(x) BOOST_PP_CAT(Pythy_, x)

#define PYTHY_TEMPLATE_ARGS_P(s, data, x) BOOST_PP_NOT(PYTHY_IS_PAREN(x)) 
#define PYTHY_TEMPLATE_ARGS_OP(d, data, x) class PYHTY_TYPE_FROM_VAR(PYTHY_REMOVE_KEYWORD(x))
#define PYTHY_TEMPLATE_ARGS(seq) \
BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(PYTHY_TEMPLATE_ARGS_OP, ~, BOOST_PP_SEQ_FILTER(PYTHY_TEMPLATE_ARGS_P, ~, seq)))

#define PYTHY_FUNCTION_ARGS_OP(r, data, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_IIF(PYTHY_IS_PAREN(x), PYTHY_PAIR, PYTHY_FUNCTION_ARGS_OP_PARAM)(x)
#define PYTHY_FUNCTION_ARGS_OP_PARAM(x)  PYHTY_TYPE_FROM_VAR(PYTHY_REMOVE_KEYWORD(x)) PYTHY_TRANSFORM_KEYWORD(x)
#define PYTHY_FUNCTION_ARGS(seq) BOOST_PP_SEQ_FOR_EACH_I(PYTHY_FUNCTION_ARGS_OP, ~, seq)

#define PYTHY_FORWARD_ENUM(z, n, data) std::forward<T ## n>(x ## n)

#define PYTHY_FORWARD(name, n) \
template<BOOST_PP_ENUM_PARAMS(n, class T)> \
auto name(BOOST_PP_ENUM_BINARY_PARAMS(n, T, && x)) \
PYTHY_RETURNS \
( \
(*BOOST_PP_CAT(pythy_, name)<BOOST_PP_ENUM_PARAMS(n, T)>::p) \
(BOOST_PP_ENUM(n, PYTHY_FORWARD_ENUM, ~)) \
)


#define PYTHY_HEADER(name, args) \
template<PYTHY_TEMPLATE_ARGS(args)> \
struct BOOST_PP_CAT(pythy_, name); \
PYTHY_FORWARD(name, BOOST_PP_SEQ_SIZE(args)) \
\
template<PYTHY_TEMPLATE_ARGS(args)> \
struct BOOST_PP_CAT(pythy_, name) \
{ \
    constexpr static auto *p = false ? pythy::addr() + [] (PYTHY_FUNCTION_ARGS(args))

#define PYTHY_BODY(...) { __VA_ARGS__ } : nullptr; };


#define PYTHY(name, ...) \
    PYTHY_HEADER(name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    PYTHY_BODY


#endif