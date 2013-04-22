
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../helpers/prefix.hpp"
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "../helpers/postfix.hpp"

void foo(hexerboost::unordered_set<int>&,
        hexerboost::unordered_map<int, int>&,
        hexerboost::unordered_multiset<int>&,
        hexerboost::unordered_multimap<int, int>&);

int main()
{
    hexerboost::unordered_set<int> x1;
    hexerboost::unordered_map<int, int> x2;
    hexerboost::unordered_multiset<int> x3;
    hexerboost::unordered_multimap<int, int> x4;

    foo(x1, x2, x3, x4);
    
    return 0;
}
