#ifndef BOOST_CONSTRAINED_VALUE_CLIPPING_HPP
#define BOOST_CONSTRAINED_VALUE_CLIPPING_HPP

/**
    @file    clipping.hpp
    @brief   Aliases of bounded for %bounded objects that clip the value to the bounds.
    @version 2.3
    @date    2008.11.30
    @author  Robert Kawulak

    This file is a part of the Boost Constrained Value library.
    See http://www.boost.org/libs/constrained_value/ for library home page.

    Copyright (C) 2004-2008 Robert Kawulak (Robert.Kawulak.at.gmail.com).
    Distributed under the Boost Software License, Version 1.0 (see accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
*/

#include <functional>

#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/integral_c.hpp>

#include <boost/constrained_value/bounded.hpp>

namespace boost {
namespace constrained_value {



/// Error policy for constrained that clips the new value to the bounds.
struct clip
{

    /// The policy invocation operator.
    ///
    /// If @a nv is below the range, assigns @c c.lower_bound() to @a ov.
    /// If @a nv is above the range, assigns @c c.upper_bound() to @a ov.
    ///
    /// @remarks
    /// - @a V must be @c Assignable from @a L and @a U.
    /// - @a c must represent a valid range (the upper bound cannot be less
    ///   than the lower bound with regard to the comparison predicate).
    template <typename V, typename L, typename U, typename C>
    void operator () (V & ov, const V & nv, const within_bounds <
        L, U, boost::mpl::false_, boost::mpl::false_, C> & c) const
    {
        BOOST_ASSERT( !c.compare()(c.upper_bound(), c.lower_bound()) );

        if( c.is_below(nv) )
            ov = c.lower_bound();
        else if( c.is_above(nv) )
            ov = c.upper_bound();
        else
            BOOST_ASSERT(false);
    }

};



/// An alias of bounded for %bounded objects that clip the value to the bounds.
template <
    typename ValueType,
    typename LowerType = ValueType,
    typename UpperType = LowerType,
    typename CompareType = std::less<ValueType>
>
struct clipping
{

    // To be able to clip to the bounds, both the bounds have to be included in the range.

    /// The nested-typedef alias.
    typedef BOOST_DEDUCED_TYPENAME
        bounded<
            ValueType,
            LowerType,
            UpperType,
            clip,
            boost::mpl::false_,
            boost::mpl::false_,
            CompareType
        >::type type;

};

/// An alias of clipping with integral compile-time-fixed bounds.
template <
    typename ValueType,
    ValueType LowerBound,
    ValueType UpperBound,
    typename CompareType = std::less<ValueType>
>
struct clipping_int
{

    /// The nested-typedef alias.
    typedef BOOST_DEDUCED_TYPENAME
        clipping<
            ValueType,
            boost::mpl::integral_c<ValueType, LowerBound>,
            boost::mpl::integral_c<ValueType, UpperBound>,
            CompareType
        >::type type;

};



} // namespace constrained_value
} // namespace boost

#endif // BOOST_CONSTRAINED_VALUE_CLIPPING_HPP
