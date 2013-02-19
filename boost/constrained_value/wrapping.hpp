#ifndef BOOST_CONSTRAINED_VALUE_WRAPPING_HPP
#define BOOST_CONSTRAINED_VALUE_WRAPPING_HPP

/**
    @file    wrapping.hpp
    @brief   Aliases of bounded for %bounded objects that wrap the value over the bounds.
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



/// Error policy for constrained that wraps the new value over the bounds.
struct wrap
{

    /// The policy invocation operator.
    ///
    /// If @a nv is below the range, assigns the result of the following expression to @a ov:
    /// @code c.upper_bound() - (c.lower_bound() - nv - 1) % (c.upper_bound() - c.lower_bound() + 1) @endcode
    /// If @a nv is above the range, assigns the result of the following expression to @a ov:
    /// @code c.lower_bound() + (nv - c.upper_bound() - 1) % (c.upper_bound() - c.lower_bound() + 1) @endcode
    ///
    /// Note, that the expressions are not always fully overflow-safe. The types
    /// and bounds values should be selected appropriately to avoid overflows.
    ///
    /// @remarks
    /// - @a V, @a L and @a U must be types representing integer numbers
    ///   or random access iterators, allowing for the arithmetic computations
    ///   described above and for assignment of the result to @a V.
    /// - @a c must represent a valid range (the upper bound cannot be less
    ///   than the lower bound).
    template <typename V, typename L, typename U>
    void operator () (V & ov, const V & nv, const within_bounds<
        L, U, boost::mpl::false_, boost::mpl::false_, std::less<V> > & c) const
    {
        BOOST_ASSERT( !c.compare()(c.upper_bound(), c.lower_bound()) );

        if( c.is_below(nv) )
            ov = c.upper_bound() - (c.lower_bound() - nv - 1) % (c.upper_bound() - c.lower_bound() + 1);
        else if( c.is_above(nv) )
            ov = c.lower_bound() + (nv - c.upper_bound() - 1) % (c.upper_bound() - c.lower_bound() + 1);
        else
            BOOST_ASSERT(false);
    }

};



/// An alias of bounded for %bounded objects that wrap the value over the bounds.
template <
    typename ValueType,
    typename LowerType = ValueType,
    typename UpperType = LowerType
>
struct wrapping
{

    // To be able to wrap over the bounds, both the bounds have to be included
    // in the range and comparison must be performed using the less than operator.

    /// The nested-typedef alias.
    typedef BOOST_DEDUCED_TYPENAME
        bounded<
            ValueType,
            LowerType,
            UpperType,
            wrap,
            boost::mpl::false_,
            boost::mpl::false_,
            std::less<ValueType>
        >::type type;

};

/// An alias of wrapping with integral compile-time-fixed bounds.
template <
    typename ValueType,
    ValueType LowerBound,
    ValueType UpperBound
>
struct wrapping_int
{

    /// The nested-typedef alias.
    typedef BOOST_DEDUCED_TYPENAME
        wrapping<
            ValueType,
            boost::mpl::integral_c<ValueType, LowerBound>,
            boost::mpl::integral_c<ValueType, UpperBound>
        >::type type;

};



} // namespace constrained_value
} // namespace boost

#endif // BOOST_CONSTRAINED_VALUE_WRAPPING_HPP
