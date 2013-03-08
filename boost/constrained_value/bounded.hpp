#ifndef BOOST_CONSTRAINED_VALUE_BOUNDED_HPP
#define BOOST_CONSTRAINED_VALUE_BOUNDED_HPP

/**
    @file    bounded.hpp
    @brief   Aliases of constrained to form %bounded object types together with related types and functions.
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
#include <boost/compressed_pair.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/swap.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/integral_c.hpp>

#include <boost/constrained_value/constrained.hpp>

namespace boost {
namespace constrained_value {



/**
    Class template for value-within-bounds predicates.

    Generated classes are predicates telling whether a value lays within the
    range defined by the given bounds. The bounds may be included or excluded
    from the range, as specified by the exclusion indicators.

    When the range specified by @c %within_bounds object is invalid (e.g., the
    upper bound is less than the lower bound), the object will report every value
    as not being within the allowed range. Note, however, that this class does not
    provide any mechanism of validating the range, since the comparison predicate
    is not required to be able to compare objects of the bounds' types with each
    other.

    @param LowerType Type of the lower bound.
    @param UpperType Type of the upper bound.
    @param LowerExclType Type of object @c Convertible to @c bool indicating
        whether the lower bound is excluded from the allowed range.
    @param UpperExclType Type of object @c Convertible to @c bool indicating
        whether the upper bound is excluded from the allowed range.
    @param CompareType Type of @c BinaryPredicate comparing the bounds with values
        passed as arguments of the member template functions. If it is a function
        objects class, it must have @c const invocation operator. The comparison
        must induce <a href="http://en.wikipedia.org/wiki/Strict_weak_ordering">
        strict weak ordering</a>.

    @remarks
    - Models @c UnaryPredicate.
    - Provides implicitly-declared special member functions apart from default
      constructor, but only when all the template actual parameters provide the
      corresponding functions too.
    - Actual template parameters are inherited (unpublicly) to make possible
      use of Empty Base Optimization.
*/
template <
    typename LowerType, typename UpperType,
    typename LowerExclType, typename UpperExclType,
    typename CompareType
>
class within_bounds :
    // The order of parameters of compressed_pairs below may seem a bit odd, but:
    // - grouping them like <L, LE>, <U, UE> will cause error when L = U and
    //   LE = UE (the same type repeated in base classes list),
    // - grouping them like <L, U>, <LE, UE> will prohibit full space optimization
    //   when L = U or LE = UE due to the way compressed_pair is implemented.
    // Note, that the current solution would also cause repeated base class error
    // in the case when L = UE and U = LE, but such case seems to be very unlikely
    // and rather erroneous.
    private boost::compressed_pair<LowerType, LowerExclType>,
    private boost::compressed_pair<UpperExclType, UpperType>,
    private CompareType
{

    BOOST_STATIC_ASSERT(( boost::is_convertible<LowerExclType, bool>::value ));
    BOOST_STATIC_ASSERT(( boost::is_convertible<UpperExclType, bool>::value ));


// Shorthand typedefs:

    typedef
        boost::compressed_pair<LowerType, LowerExclType>
        _lower_holder_type;

    typedef
        boost::compressed_pair<UpperExclType, UpperType>
        _upper_holder_type;


public:


// Remember the template's actual parameters:

    /// The lower bound type.
    typedef LowerType lower_type;

    /// The upper bound type.
    typedef UpperType upper_type;

    /// The lower bound exclusion type.
    typedef LowerExclType lower_excl_type;

    /// The upper bound exclusion type.
    typedef UpperExclType upper_excl_type;

    /// The comparison predicate type.
    typedef CompareType compare_type;


// Access to the generators and compare object:

    /// Returns the lower bound object.
    lower_type & lower_bound()
    { return _lower_holder_type::first(); }

    /// Returns the lower bound object.
    BOOST_CONSTEXPR const lower_type & lower_bound() const
    { return _lower_holder_type::first(); }


    /// Returns the upper bound object.
    upper_type & upper_bound()
    { return _upper_holder_type::second(); }

    /// Returns the upper bound object.
    BOOST_CONSTEXPR const upper_type & upper_bound() const
    { return _upper_holder_type::second(); }


    /// Returns the lower bound exclusion indicator.
    lower_excl_type & lower_bound_excluded()
    { return _lower_holder_type::second(); }

    /// Returns the lower bound exclusion indicator.
    BOOST_CONSTEXPR const lower_excl_type & lower_bound_excluded() const
    { return _lower_holder_type::second(); }


    /// Returns the upper bound exclusion indicator.
    upper_excl_type & upper_bound_excluded()
    { return _upper_holder_type::first(); }

    /// Returns the upper bound exclusion indicator.
    BOOST_CONSTEXPR const upper_excl_type & upper_bound_excluded() const
    { return _upper_holder_type::first(); }


    /// Returns the comparison predicate object.
    compare_type & compare()
    { return *this; }

    /// Returns the comparison predicate object.
    BOOST_CONSTEXPR const compare_type & compare() const
    { return *this; }


// Special member functions and swap:

    /// The default constructor.
    /// @remarks
    /// - #lower_type, #upper_type, #lower_excl_type and #upper_excl_type
    ///   must be @c DefaultConstructible and @c CopyConstructible.
    /// - #compare_type must be @c DefaultConstructible.
    BOOST_CONSTEXPR within_bounds() :
        _lower_holder_type(lower_type(), lower_excl_type()),
        _upper_holder_type(upper_excl_type(), upper_type()),
        compare_type()
    { }

    BOOST_CONSTEXPR within_bounds(const within_bounds& other) :
        _lower_holder_type(other),
        _upper_holder_type(other),
        compare_type(other)
    { }

    // Using implicitly-declared other special member functions.

    /// The swap function.
    /// @remarks All the within_bounds template actual parameters must be @c Swappable.
    void swap(within_bounds & other)
    {
        _lower_holder_type::swap(other);
        _upper_holder_type::swap(other);
        boost::swap(compare(), other.compare());
    }


// Other constructors:

    /// Constructor initializing the compare predicate with the given object.
    /// @remarks
    /// - #lower_type, #upper_type, #lower_excl_type and #upper_excl_type
    ///   must be @c DefaultConstructible and @c CopyConstructible.
    /// - #compare_type must be @c CopyConstructible.
    BOOST_CONSTEXPR explicit within_bounds(const compare_type & c) :
        _lower_holder_type(lower_type(), lower_excl_type()),
        _upper_holder_type(upper_excl_type(), upper_type()),
        compare_type(c)
    { }

    /// Constructor initializing the bounds with the given objects.
    /// @remarks
    /// - #lower_type and #upper_type must be @c CopyConstructible.
    /// - #lower_excl_type and #upper_excl_type must be
    ///   @c DefaultConstructible and @c CopyConstructible.
    /// - #compare_type must be @c DefaultConstructible.
    BOOST_CONSTEXPR within_bounds(const lower_type & l, const upper_type & u) :
        _lower_holder_type(l, lower_excl_type()),
        _upper_holder_type(upper_excl_type(), u),
        compare_type()
    { }

    /// Constructor initializing the bounds exclusion indicators with the given objects.
    /// @remarks
    /// - #lower_type and #upper_type must be @c DefaultConstructible and
    ///   @c CopyConstructible.
    /// - #lower_excl_type and #upper_excl_type must be @c CopyConstructible.
    /// - #compare_type must be @c DefaultConstructible.
    BOOST_CONSTEXPR within_bounds(const lower_excl_type & le, const upper_excl_type & ue) :
        _lower_holder_type(lower_type(), le),
        _upper_holder_type(ue, upper_type()),
        compare_type()
    { }

    /// Constructor initializing the bounds and the compare predicate with the given objects.
    /// @remarks
    /// - #lower_type, #upper_type and #compare_type must be @c CopyConstructible.
    /// - #lower_excl_type and #upper_excl_type must be @c DefaultConstructible and
    ///   @c CopyConstructible.
    BOOST_CONSTEXPR within_bounds(const lower_type & l, const upper_type & u, const compare_type & c) :
        _lower_holder_type(l, lower_excl_type()),
        _upper_holder_type(upper_excl_type(), u),
        compare_type(c)
    { }

    /// Constructor initializing the bounds exclusion indicators
    /// and the compare predicate with the given objects.
    /// @remarks
    /// - #lower_type and #upper_type must be @c DefaultConstructible and @c CopyConstructible.
    /// - #lower_excl_type, #upper_excl_type and #compare_type must be @c CopyConstructible.
    BOOST_CONSTEXPR within_bounds(const lower_excl_type & le, const upper_excl_type & ue, const compare_type & c) :
        _lower_holder_type(lower_type(), le),
        _upper_holder_type(ue, upper_type()),
        compare_type(c)
    { }

    /// Constructor initializing the bounds and the bounds
    /// exclusion indicators with the given objects.
    /// @remarks
    /// - #lower_type, #upper_type, #lower_excl_type and #upper_excl_type must be
    ///   @c CopyConstructible.
    /// - #compare_type must be @c DefaultConstructible.
    BOOST_CONSTEXPR within_bounds(const lower_type & l, const upper_type & u,
                  const lower_excl_type & le, const upper_excl_type & ue) :
        _lower_holder_type(l, le),
        _upper_holder_type(ue, u),
        compare_type()
    { }

    /// Constructor initializing all the underlying objects with the given objects.
    /// @remarks All within_bounds template actual parameters must be @c CopyConstructible.
    BOOST_CONSTEXPR within_bounds(const lower_type & l, const upper_type & u,
                  const lower_excl_type & le, const upper_excl_type & ue, const compare_type & c) :
        _lower_holder_type(l, le),
        _upper_holder_type(ue, u),
        compare_type(c)
    { }


// Functions for checking whether a value lays within the bounds:

    /// Tells whether the @a value is below the range.
    /// @return Result of the compare() predicate invocation for the @a value
    /// and the lower_bound(), with respect to lower_bound_excluded().
    template <typename T>
    BOOST_CONSTEXPR bool is_below(const T & value) const
    {
        return !lower_bound_excluded()
          ? compare()(value, lower_bound())
          : !compare()(lower_bound(), value);
    }

    /// Tells whether the @a value is above the range.
    /// @return Result of the compare() predicate invocation for the @a value
    /// and the upper_bound(), with respect to upper_bound_excluded().
    template <typename T>
    BOOST_CONSTEXPR bool is_above(const T & value) const
    {
        return !upper_bound_excluded()
          ? compare()(upper_bound(), value)
          : !compare()(value, upper_bound());
    }

    /// Tells whether the @a value is within the range.
    /// @return Result of the compare() predicate invocation for the @a value
    /// and the bounds, with respect to bounds exclusion.
    template <typename T>
    BOOST_CONSTEXPR bool is_within(const T & value) const
    {
        return !is_below(value) && !is_above(value);
    }

    /// Tells whether the @a value is within the range.
    /// @return #is_within(@a value)
    template <typename T>
    BOOST_CONSTEXPR bool operator () (const T & value) const
    {
        return is_within(value);
    }

}; // class within_bounds<>

/** @relatesalso within_bounds
    Namespace-level swap for within_bounds.
    @remarks All the within_bounds template actual parameters must be @c Swappable.
*/
template <typename L, typename U, typename LE, typename UE, typename C>
void swap(within_bounds<L, U, LE, UE, C> & lhs, within_bounds<L, U, LE, UE, C> & rhs)
{
    lhs.swap(rhs);
}

// To be a literal type, bounded requires a constexpr comparison, but std::less does not necessarily provide one.
template<typename ValueType>
struct less
{
  typedef ValueType first_argument_type;
  typedef ValueType second_argument_type;
  typedef bool result_type;

  BOOST_CONSTEXPR less() {}
  BOOST_CONSTEXPR less(const less&) {}

  BOOST_CONSTEXPR bool operator() (const ValueType& x, const ValueType& y) const
  {
    return x < y;
  }
};

/// An alias of constrained for %bounded object types (using within_bounds constraint policy).
template <
    typename ValueType,
    typename LowerType = ValueType,
    typename UpperType = LowerType,
    typename ErrorPolicy = throw_exception<>,
    typename LowerExclType = boost::mpl::false_,
    typename UpperExclType = LowerExclType,
    typename CompareType = less<ValueType>
>
struct bounded
{

    /// The nested-typedef alias.
    typedef
        constrained<
            ValueType,
            within_bounds<
                LowerType,
                UpperType,
                LowerExclType,
                UpperExclType,
                CompareType
            >,
            ErrorPolicy
        > type;

};

/// An alias of bounded with integral compile-time-fixed bounds.
template <
    typename ValueType,
    ValueType LowerBound,
    ValueType UpperBound,
    typename ErrorPolicy = throw_exception<>,
    bool LowerExcl = false,
    bool UpperExcl = LowerExcl,
    typename CompareType = less<ValueType>
>
struct bounded_int
{

    /// The nested-typedef alias.
    typedef BOOST_DEDUCED_TYPENAME
        bounded<
            ValueType,
            boost::mpl::integral_c<ValueType, LowerBound>,
            boost::mpl::integral_c<ValueType, UpperBound>,
            ErrorPolicy,
            boost::mpl::bool_<LowerExcl>,
            boost::mpl::bool_<UpperExcl>,
            CompareType
        >::type type;

};



/**
    Changes the bounds and the bounds exclusion for the given %bounded object.

    Calls change_constraint() with a new within_bounds object contaning the
    given values and the comparison object of the old within_bounds object.

    @param b The %bounded object to modify.
    @param l The new lower bound object.
    @param u The new upper bound object.
    @param le The new lower bound exclusion indicating object.
    @param ue The new upper bound exclusion indicating object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TL, @a TU, @a TLE and @a TUE must be @c Convertible to, respectively, @a L, @a U, @a LE and @a UE.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TL, typename TU, typename TLE, typename TUE>
void change_bounds(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TL & l, const TU & u, const TLE & le, const TUE & ue)
{
    change_constraint(b, within_bounds<L, U, LE, UE, C>(l, u, le, ue, b.constraint().compare()));
}

/**
    Changes the bounds of the given %bounded object.

    Calls change_bounds() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param l The new lower bound object.
    @param u The new upper bound object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TL and @a TU must be @c Convertible to, respectively, @a L and @a U.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TL, typename TU>
void change_bounds(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TL & l, const TU & u)
{
    change_bounds(b, l, u, b.constraint().lower_bound_excluded(), b.constraint().upper_bound_excluded());
}

/**
    Changes the bounds exclusion for the given %bounded object.

    Calls change_bounds() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param le The new lower bound exclusion indicating object.
    @param ue The new upper bound exclusion indicating object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TLE and @a TUE must be @c Convertible to, respectively, @a LE and @a UE.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TLE, typename TUE>
void change_bounds_exclusion(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TLE & le, const TUE & ue)
{
    change_bounds(b, b.constraint().lower_bound(), b.constraint().upper_bound(), le, ue);
}

/**
    Changes the lower bound and its exclusion for the given %bounded object.

    Calls change_bounds() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param l The new lower bound object.
    @param le The new lower bound exclusion indicating object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TL and @a TLE must be @c Convertible to, respectively, @a L and @a LE.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TL, typename TLE>
void change_lower_bound(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TL & l, const TLE & le)
{
    change_bounds(b, l, b.constraint().upper_bound(), le, b.constraint().upper_bound_excluded());
}

/**
    Changes the lower bound of the given %bounded object.

    Calls change_lower_bound() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param l The new lower bound object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TL must be @c Convertible to @a L.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TL>
void change_lower_bound(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TL & l)
{
    change_lower_bound(b, l, b.constraint().lower_bound_excluded());
}

/**
    Changes the lower bound exclusion for the given %bounded object.

    Calls change_lower_bound() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param le The new lower bound exclusion indicating object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TLE must be @c Convertible to @a LE.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TLE>
void change_lower_bound_exclusion(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TLE & le)
{
    change_lower_bound(b, b.constraint().lower_bound(), le);
}

/**
    Changes the upper bound and its exclusion for the given %bounded object.

    Calls change_bounds() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param u The new upper bound object.
    @param ue The new upper bound exclusion indicating object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TU and @a TUE must be @c Convertible to, respectively, @a U and @a UE.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TU, typename TUE>
void change_upper_bound(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TU & u, const TUE & ue)
{
    change_bounds(b, b.constraint().lower_bound(), u, b.constraint().lower_bound_excluded(), ue);
}

/**
    Changes the upper bound of the given %bounded object.

    Calls change_upper_bound() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param u The new upper bound object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TU must be @c Convertible to @a U.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TU>
void change_upper_bound(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TU & u)
{
    change_upper_bound(b, u, b.constraint().upper_bound_excluded());
}

/**
    Changes the upper bound exclusion for the given %bounded object.

    Calls change_upper_bound() with a combination of the specified
    and existing policies of the previous within_bounds object.

    @param b The %bounded object to modify.
    @param ue The new upper bound exclusion indicating object.

    @remarks
    - @a V, @a L, @a U, @a LE, @a UE, @a C and @a E must be @c CopyConstructible and @c Swappable.
    - @a TUE must be @c Convertible to @a UE.
*/
template <typename V, typename L, typename U, typename LE, typename UE, typename C, typename E,
    typename TUE>
void change_upper_bound_exclusion(constrained<V, within_bounds<L, U, LE, UE, C>, E> & b,
    const TUE & ue)
{
    change_upper_bound(b, b.constraint().upper_bound(), ue);
}



} // namespace constrained_value
} // namespace boost

#endif // BOOST_CONSTRAINED_VALUE_BOUNDED_HPP
