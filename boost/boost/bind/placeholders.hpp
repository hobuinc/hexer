#ifndef BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#define BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/placeholders.hpp - _N definitions
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <boost/bind/arg.hpp>
#include <boost/config.hpp>

namespace
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

static inline hexerboost::arg<1> _1() { return hexerboost::arg<1>(); }
static inline hexerboost::arg<2> _2() { return hexerboost::arg<2>(); }
static inline hexerboost::arg<3> _3() { return hexerboost::arg<3>(); }
static inline hexerboost::arg<4> _4() { return hexerboost::arg<4>(); }
static inline hexerboost::arg<5> _5() { return hexerboost::arg<5>(); }
static inline hexerboost::arg<6> _6() { return hexerboost::arg<6>(); }
static inline hexerboost::arg<7> _7() { return hexerboost::arg<7>(); }
static inline hexerboost::arg<8> _8() { return hexerboost::arg<8>(); }
static inline hexerboost::arg<9> _9() { return hexerboost::arg<9>(); }

#elif defined(BOOST_MSVC) || (defined(__DECCXX_VER) && __DECCXX_VER <= 60590031) || defined(__MWERKS__) || \
    defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 2)  

static hexerboost::arg<1> _1;
static hexerboost::arg<2> _2;
static hexerboost::arg<3> _3;
static hexerboost::arg<4> _4;
static hexerboost::arg<5> _5;
static hexerboost::arg<6> _6;
static hexerboost::arg<7> _7;
static hexerboost::arg<8> _8;
static hexerboost::arg<9> _9;

#else

hexerboost::arg<1> _1;
hexerboost::arg<2> _2;
hexerboost::arg<3> _3;
hexerboost::arg<4> _4;
hexerboost::arg<5> _5;
hexerboost::arg<6> _6;
hexerboost::arg<7> _7;
hexerboost::arg<8> _8;
hexerboost::arg<9> _9;

#endif

} // unnamed namespace

#endif // #ifndef BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
