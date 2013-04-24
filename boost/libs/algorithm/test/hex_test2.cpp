/* 
   Copyright (c) Marshall Clow 2011-2012.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    For more information, see http://www.boost.org

Test non-string cases; vector and list
*/

#include <boost/config.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/test/included/test_exec_monitor.hpp>

#include <string>
#include <iostream>
#include <deque>
#include <list>


const char *tohex [] = {
    "",
    "a",
    "\001",
    "12",
    "asdfadsfsad",
    "01234567890ABCDEF",
    NULL        // End of the list
    };

void test_to_hex () {
    for ( const char **p = tohex; *p; p++ ) {
        std::deque<char> arg, argh;
        std::list<char> one, two, three;
        arg.assign ( *p, *p + strlen (*p));
        hexerboost::algorithm::hex ( *p,                       std::back_inserter ( one ));
        hexerboost::algorithm::hex ( arg,                      std::back_inserter ( two ));
        hexerboost::algorithm::hex ( arg.begin (), arg.end (), std::back_inserter ( three ));
        BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));

        std::copy ( one.begin (), one.end (), std::back_inserter ( argh ));
        one.clear (); two.clear (); three.clear ();

//      hexerboost::algorithm::unhex ( argh.c_str (),              std::back_inserter ( one ));
        hexerboost::algorithm::unhex ( argh,                       std::back_inserter ( two ));
        hexerboost::algorithm::unhex ( argh.begin (), argh.end (), std::back_inserter ( three ));
//      BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), arg.begin ()));
        }

//  Again, with a front_inserter
    for ( const char **p = tohex; *p; p++ ) {
        std::deque<char> arg, argh;
        std::list<char> one, two, three;
        arg.assign ( *p, *p + strlen (*p));
        hexerboost::algorithm::hex ( *p,                       std::front_inserter ( one ));
        hexerboost::algorithm::hex ( arg,                      std::front_inserter ( two ));
        hexerboost::algorithm::hex ( arg.begin (), arg.end (), std::front_inserter ( three ));
        BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));

    //  Copy, reversing
        std::copy ( one.begin (), one.end (), std::front_inserter ( argh ));
        one.clear (); two.clear (); three.clear ();

//      hexerboost::algorithm::unhex ( argh.c_str (),              std::front_inserter ( one ));
        hexerboost::algorithm::unhex ( argh,                       std::front_inserter ( two ));
        hexerboost::algorithm::unhex ( argh.begin (), argh.end (), std::front_inserter ( three ));
//      BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), arg.rbegin ()));   // reverse
        }
    }

const char *fromhex [] = {
    "20",
    "2122234556FF",
    NULL        // End of the list
    };


void test_from_hex_success () {
    for ( const char **p = fromhex; *p; p++ ) {
        std::deque<char> arg, argh;
        std::list<char> one, two, three;
        arg.assign ( *p, *p + strlen (*p));
        hexerboost::algorithm::unhex ( *p,                       std::back_inserter ( one ));
        hexerboost::algorithm::unhex ( arg,                      std::back_inserter ( two ));
        hexerboost::algorithm::unhex ( arg.begin (), arg.end (), std::back_inserter ( three ));
        BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));

        std::copy ( one.begin (), one.end (), std::back_inserter ( argh ));
        one.clear (); two.clear (); three.clear ();

//      hexerboost::algorithm::hex ( argh.c_str (),              std::back_inserter ( one ));
        hexerboost::algorithm::hex ( argh,                       std::back_inserter ( two ));
        hexerboost::algorithm::hex ( argh.begin (), argh.end (), std::back_inserter ( three ));
//      BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), arg.begin ()));
        }

//  Again, with a front_inserter
    for ( const char **p = fromhex; *p; p++ ) {
        std::deque<char> arg, argh;
        std::list<char> one, two, three;
        arg.assign ( *p, *p + strlen (*p));
        hexerboost::algorithm::unhex ( *p,                       std::front_inserter ( one ));
        hexerboost::algorithm::unhex ( arg,                      std::front_inserter ( two ));
        hexerboost::algorithm::unhex ( arg.begin (), arg.end (), std::front_inserter ( three ));
        BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));

    //  Copy, reversing
        std::copy ( one.begin (), one.end (), std::front_inserter ( argh ));
        one.clear (); two.clear (); three.clear ();

//      hexerboost::algorithm::hex ( argh.c_str (),              std::front_inserter ( one ));
        hexerboost::algorithm::hex ( argh,                       std::front_inserter ( two ));
        hexerboost::algorithm::hex ( argh.begin (), argh.end (), std::front_inserter ( three ));
//      BOOST_CHECK ( std::equal ( one.begin (), one.end (), two.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), three.begin ()));
        BOOST_CHECK ( std::equal ( two.begin (), two.end (), arg.rbegin ()));   // reversed
        }
    }


int test_main( int , char* [] )
{
  test_to_hex ();
  test_from_hex_success ();
  return 0;
}
