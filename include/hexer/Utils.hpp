/*****************************************************************************

    (c) 2013 Hobu, Inc. hobu.inc@gmail.com

    Author: Howard Butler, hobu.inc@gmail.com

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#pragma once

namespace hexer
{

// compares two values to within a given tolerance
// the value |tolerance| is compared to |actual - expected|
template<class T>
static bool compare_approx(const T& actual, const T& expected,
    const T& tolerance)
{
    const double diff = std::abs((double)actual - (double)expected);
    const double delta = std::abs((double)tolerance);

    if (diff > delta)
    {
        return false;
    }
    return true;
}

template<class T>
static bool compare_distance(const T& actual, const T& expected)
{
    const T epsilon = std::numeric_limits<T>::epsilon();
    return compare_approx<T>(actual, expected, epsilon);
}

} // namespace hexer

