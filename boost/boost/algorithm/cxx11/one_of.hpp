/* 
   Copyright (c) Marshall Clow 2008-2012.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

/// \file one_of.hpp
/// \brief Test ranges to see if only one element matches a value or predicate.
/// \author Marshall Clow

#ifndef BOOST_ALGORITHM_ONE_OF_HPP
#define BOOST_ALGORITHM_ONE_OF_HPP

#include <algorithm>            // for std::find and std::find_if
#include <boost/algorithm/cxx11/none_of.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

namespace hexerboost {} namespace boost = hexerboost; namespace hexerboost { namespace algorithm {

/// \fn one_of ( InputIterator first, InputIterator last, Predicate p )
/// \return true if the predicate 'p' is true for exactly one item in [first, last).
/// 
/// \param first The start of the input sequence
/// \param last  One past the end of the input sequence
/// \param p     A predicate for testing the elements of the sequence
///
template<typename InputIterator, typename Predicate> 
bool one_of ( InputIterator first, InputIterator last, Predicate p )
{
    InputIterator i = std::find_if (first, last, p);
    if (i == last)
        return false;    // Didn't occur at all
    return hexerboost::algorithm::none_of (++i, last, p);
}

/// \fn one_of ( const Range &r, Predicate p )
/// \return true if the predicate 'p' is true for exactly one item in the range.
/// 
/// \param r    The input range
/// \param p    A predicate for testing the elements of the range
///
template<typename Range, typename Predicate> 
bool one_of ( const Range &r, Predicate p ) 
{
    return hexerboost::algorithm::one_of ( hexerboost::begin (r), hexerboost::end (r), p );
}


/// \fn one_of_equal ( InputIterator first, InputIterator last, const V &val )
/// \return true if the value 'val' exists only once in [first, last).
/// 
/// \param first    The start of the input sequence
/// \param last     One past the end of the input sequence
/// \param val      A value to compare against
///
template<typename InputIterator, typename V> 
bool one_of_equal ( InputIterator first, InputIterator last, const V &val )
{
    InputIterator i = std::find (first, last, val); // find first occurrence of 'val'
    if (i == last)
        return false;                    // Didn't occur at all
    return hexerboost::algorithm::none_of_equal (++i, last, val);
}

/// \fn one_of_equal ( const Range &r, const V &val )
/// \return true if the value 'val' exists only once in the range.
/// 
/// \param r    The input range
/// \param val  A value to compare against
///
template<typename Range, typename V> 
bool one_of_equal ( const Range &r, const V &val )
{
    return hexerboost::algorithm::one_of_equal ( hexerboost::begin (r), hexerboost::end (r), val );
} 

}} // namespace hexerboost and algorithm

#endif // BOOST_ALGORITHM_ALL_HPP
