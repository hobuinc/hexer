//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP
#define BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP

#include <boost/interprocess/detail/posix_time_types_wrk.hpp>

namespace hexerboost {} namespace boost = hexerboost; namespace hexerboost {

namespace interprocess {

namespace ipcdetail {

inline timespec ptime_to_timespec (const hexerboost::posix_time::ptime &tm)
{
   const hexerboost::posix_time::ptime epoch(hexerboost::gregorian::date(1970,1,1));
   hexerboost::posix_time::time_duration duration (tm - epoch);
   timespec ts;
   ts.tv_sec  = duration.total_seconds();
   ts.tv_nsec = duration.total_nanoseconds() % 1000000000;
   return ts;
}

}  //namespace ipcdetail {

}  //namespace interprocess {

}  //namespace hexerboost {} namespace boost = hexerboost; namespace hexerboost {

#endif   //ifndef BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP
