
//  (C) Copyright Daniel James 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18. 

#if !defined(BOOST_FUNCTIONAL_DETAIL_HASH_FLOAT_HEADER)
#define BOOST_FUNCTIONAL_DETAIL_HASH_FLOAT_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/functional/detail/float_functions.hpp>
#include <errno.h>
#include <boost/limits.hpp>

namespace boost
{
    namespace hash_detail
    {
        inline void hash_float_combine(std::size_t& seed, std::size_t value)
        {
            seed ^= value + (seed<<6) + (seed>>2);
        }

        template <class T>
        inline std::size_t float_hash_value(T v)
        {
            int exp = 0;
            errno = 0;
            v = boost::hash_detail::call_frexp(v, &exp);
            if(errno) return 0;

            std::size_t seed = 0;

            std::size_t const length
                = (std::numeric_limits<T>::digits +
                        std::numeric_limits<int>::digits - 1)
                / std::numeric_limits<int>::digits;

            for(std::size_t i = 0; i < length; ++i)
            {
                v = boost::hash_detail::call_ldexp(v, std::numeric_limits<int>::digits);
                int const part = static_cast<int>(v);
                v -= part;
                hash_float_combine(seed, part);
            }

            hash_float_combine(seed, exp);

            return seed;
        }
    }
}

#endif
