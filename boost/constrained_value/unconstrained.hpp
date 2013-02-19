#ifndef BOOST_CONSTRAINED_VALUE_UNCONSTRAINED_HPP
#define BOOST_CONSTRAINED_VALUE_UNCONSTRAINED_HPP

/**
    @file    unconstrained.hpp
    @brief   Alias for constrained with no constraint.
    @version 2.3
    @date    2008.11.30
    @author  Robert Kawulak

    This file is a part of the Boost Constrained Value library.
    See http://www.boost.org/libs/constrained_value/ for library home page.

    Copyright (C) 2004-2008 Robert Kawulak (Robert.Kawulak.at.gmail.com).
    Distributed under the Boost Software License, Version 1.0 (see accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
*/

#include <boost/config.hpp>

#include <boost/constrained_value/constrained.hpp>

namespace boost {
namespace constrained_value {



/**
    A dummy constraint policy allowing for any value (and thus imposing no constraint).
*/
struct no_constraint {

    /// The policy invocation operator.
    /// @return @c true
    template <typename T>
    bool operator () (T) const
    { return true; }

};



/**
    Alias for constrained with no_constraint used as the constraint policy.

    This alias can be used in release mode to replace constrained objects used
    in debug mode for verification/debugging purposes. The implementation is
    optimized to allow a good compiler to generate code that is virtually as
    efficient as if using the bare underlying type.

    @param ValueType The underlying value type.
*/
template <typename ValueType>
class unconstrained
{

    /**
        A dummy error policy ignoring any error.

        Note: when using no_constraint policy for constrained, the error policy
        will actually never be called. However, the default error policy contains
        code related to exceptions, which may have a negative impact on efficiency
        of the resulting compiler output, therefore this class is used instead. In
        general case it is not conforming to the error policy specification, because
        it ignores construction of constrained object with invalid value. However,
        as mentioned above, the invocation operator will actually never get called
        so this policy will never break the invariant of the constrained object if
        used together with no_constraint policy.
    */
    struct _no_error_policy {

        /// The policy invocation operator.
        /// This function has empty body.
        template <typename V, typename C>
        void operator () (V, V, C)
        { }

    };


public:

    /// The nested-typedef alias.
    typedef
        constrained<
            ValueType,
            no_constraint,
            _no_error_policy
        > type;

};



} // namespace constrained_value
} // namespace boost

#endif // BOOST_CONSTRAINED_VALUE_UNCONSTRAINED_HPP
