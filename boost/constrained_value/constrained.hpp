#ifndef BOOST_CONSTRAINED_VALUE_CONSTRAINED_HPP
#define BOOST_CONSTRAINED_VALUE_CONSTRAINED_HPP

/**
    @file    constrained.hpp
    @brief   The constrained class template together with related types and functions.
    @version 2.3
    @date    2008.11.30
    @author  Robert Kawulak

    This file is a part of the Boost Constrained Value library.
    See http://www.boost.org/libs/constrained_value/ for library home page.

    Copyright (C) 2004-2008 Robert Kawulak (Robert.Kawulak.at.gmail.com).
    Distributed under the Boost Software License, Version 1.0 (see accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
*/

#include <stdexcept>
#include <string>

#include <boost/config.hpp>

#ifndef BOOST_NO_IOSFWD // <boost/config.hpp> should be included before this
#    include <iosfwd>
#endif

#include <boost/throw_exception.hpp>
#include <boost/function.hpp>
#include <boost/compressed_pair.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <boost/type_traits/is_empty.hpp>
#include <boost/utility/swap.hpp>

namespace boost {
/// The Boost Constrained Value library's namespace.
namespace constrained_value {



/**
    Class of exceptions used by error policies for constrained.
*/
struct broken_constraint : public std::logic_error
{

    /// Constructs the exception.
    /// Passes @a what to the base class' constructor.
    explicit broken_constraint(const std::string & what =
        "Attempt to give constrained object a value that is not constraint-conforming.")
        : std::logic_error(what)
    { }

};

/**
    Error policy for constrained that throws an exception of @a ExceptionType.
    @remarks @a ExceptionType must be @c DefaultConstructible and derive publicly
    from @c std::exception.
*/
template <typename ExceptionType = broken_constraint>
struct throw_exception
{

    /// Type of exceptions thrown.
    typedef ExceptionType exception_type;

    /// Throws the exception.
    /// Uses <tt><a href="../../../../exception/doc/BOOST_THROW_EXCEPTION.html">
    /// BOOST_THROW_EXCEPTION</a></tt> macro to throw a default-constructed
    /// exception of #exception_type.
    template <typename V, typename C>
    void operator () (const V &, const V &, const C &) const
    {
        BOOST_THROW_EXCEPTION( exception_type() );
    }

};



/**
    Template of %constrained object classes.

    This class is a wrapper for #value_type. Objects of this class can be used
    just like objects of #value_type, with one exception: they can be given only
    a value which conforms to the specified constraint.

    @param ValueType The underlying value type.
    @param ConstraintPolicy Type of @c UnaryPredicate determining whether a value
        is constraint-conforming. If it is a function objects class, it must have
        @c const invocation operator.
    @param ErrorPolicy Type of ternary function called when an attempt is made
        to assign a non-constraint-conforming value to a constrained object.
        The arguments passed are (in order):
        <ul>
        <li>
            the underlying #value_type mutable object (its value may be
            non-constraint-conforming if the policy is called during
            initialization of a constrained object with such value),</li>
        <li>
            the new #value_type immutable object with non-constraint-conforming
            value,</li>
        <li>
            the underlying #constraint_type mutable object.</li>
        </ul>
        If the function returns, it should modify the underlying value or the
        constraint so that the value is constraint-conforming after the call.
        If it fails to do so, <tt><a href="../../../../utility/assert.html">
        BOOST_ASSERT</a></tt> is invoked.

    @invariant constraint()(value())

    @remarks
    - Provides implicitly-declared special member functions apart from default
      constructor, but only when all the template actual parameters provide the
      corresponding functions too.
    - Is implicitly @c Convertible to/from #value_type.
*/
template <
    typename ValueType,
    typename ConstraintPolicy = boost::function1<bool, const ValueType &>,
    typename ErrorPolicy = throw_exception<>
>
class constrained {


public:

// Remember the template's actual parameters:

    /// The underlying value type.
    typedef ValueType value_type;

    /// The constraint policy type.
    typedef ConstraintPolicy constraint_type;

    /// The error policy type.
    typedef ErrorPolicy error_handler_type;


// Access to the value and constraint:

    /// The underlying #value_type object.
    const value_type & value() const
    { return _value_and_policies.value; }

    /// Operator of implicit conversion to #value_type.
    /// @return value()
    operator const value_type & () const
    { return value(); }

    /// The underlying #constraint_type object.
    const constraint_type & constraint() const
    { return _value_and_policies.first(); }

    /// The underlying #error_handler_type object.
    const error_handler_type & error_handler() const
    { return _value_and_policies.second(); }

    /// Non-const access to the underlying #error_handler_type object.
    error_handler_type & error_handler()
    { return _value_and_policies.second(); }


// Special member functions and swap:

    /// The default constructor.
    ///
    /// If the value-initialized #value_type object is not constraint-conforming according
    /// to the default-initialized #constraint_type object, the default-initialized
    /// #error_handler_type object is called.
    ///
    /// @remarks
    /// - All the constrained template actual parameters must be @c DefaultConstructible.
    /// - Neither #constraint_type nor #error_handler_type can be a non-empty POD type.
    constrained()
        : _value_and_policies()
    { _initialize(); }

    // Using implicitly-declared other special member functions.

    /// The swap function.
    /// @remarks All the constrained template actual parameters must be @c Swappable.
    void swap(constrained & other)
    { _value_and_policies.swap(other._value_and_policies); }


 // Other constructors:

    /// Constructor initializing all the underlying objects with the corresponding
    /// objects of @a other.
    ///
    /// Initializes all the underlying objects with the corresponding objects of @a other.
    /// Then, if the #value_type object is not constraint-conforming according to the
    /// #constraint_type object, the #error_handler_type object is called.
    ///
    /// This constructor helps to avoid confusing situations when object of different
    /// constrained template class would be used as initializer, but only the value
    /// would be copied (since constrained is implicitly convertible to #value_type).
    ///
    /// @remarks
    /// - All the actual parameters of the constructor template must be @c Convertible
    ///   to the corresponding actual parameters of the containing class template.
    /// - All the containing class template actual parameters must be @c CopyConstructible.
    template <typename V, typename C, typename E>
    explicit constrained(const constrained<V, C, E> & other)
        : _value_and_policies(other.value(), other.constraint(), other.error_handler())
    { _initialize(); }

    /// Constructor converting implicitly from #value_type.
    ///
    /// If the copy of @a v is not constraint-conforming according to the default-initialized
    /// #constraint_type object, the default-initialized #error_handler_type object is called.
    ///
    /// @remarks
    /// - #value_type must be @c CopyConstructible.
    /// - #constraint_type and #error_handler_type must be @c DefaultConstructible.
    /// - Neither #constraint_type nor #error_handler_type can be a non-empty POD type.
    constrained(const value_type & v)
        : _value_and_policies(v)
    { _initialize(); }

    /// Constructor initializing the constraint with the given object.
    ///
    /// If the value-initialized #value_type object is not constraint-conforming according
    /// to the copy of @a c, the default-initialized #error_handler_type object is called.
    ///
    /// @remarks
    /// - #constraint_type must be @c CopyConstructible.
    /// - #value_type and #error_handler_type must be @c DefaultConstructible.
    /// - #error_handler_type cannot be a non-empty POD type.
    explicit constrained(const constraint_type & c)
        : _value_and_policies(c)
    { _initialize(); }

    /// Constructor initializing the error handler with the given object.
    ///
    /// If the value-initialized #value_type object is not constraint-conforming according
    /// to the default-initialized #constraint_type object, the copy of @a e is called.
    ///
    /// @remarks
    /// - #error_handler_type must be @c CopyConstructible.
    /// - #value_type and #constraint_type must be @c DefaultConstructible.
    /// - #constraint_type cannot be a non-empty POD type.
    explicit constrained(const error_handler_type & e)
        : _value_and_policies(e)
    { _initialize(); }

    /// Constructor initializing the value and the constraint with the given objects.
    ///
    /// If the copy of @a v is not constraint-conforming according to the copy of
    /// @a c, the default-initialized #error_handler_type object is called.
    ///
    /// @remarks
    /// - #value_type and #constraint_type must be @c CopyConstructible.
    /// - #error_handler_type must be @c DefaultConstructible.
    /// - #error_handler_type cannot be a non-empty POD type.
    constrained(const value_type & v, const constraint_type & c)
        : _value_and_policies(v, c)
    { _initialize(); }

    /// Constructor initializing the value and the error handler with the given objects.
    ///
    /// If the copy of @a v is not constraint-conforming according to the default-initialized
    /// #constraint_type object, the copy of @a e is called.
    ///
    /// @remarks
    /// - #value_type and #error_handler_type must be @c CopyConstructible.
    /// - #constraint_type must be @c DefaultConstructible.
    /// - #constraint_type cannot be a non-empty POD type.
    constrained(const value_type & v, const error_handler_type & e)
        : _value_and_policies(v, e)
    { _initialize(); }

    /// Constructor initializing the constraint and the error handler with the given objects.
    ///
    /// If the value-initialized #value_type object is not constraint-conforming according
    /// to the copy of @a c, the copy of @a e is called.
    ///
    /// @remarks
    /// - #constraint_type and #error_handler_type must be @c CopyConstructible.
    /// - #value_type must be @c DefaultConstructible.
    constrained(const constraint_type & c, const error_handler_type & e)
        : _value_and_policies(c, e)
    { _initialize(); }

    /// Constructor initializing all the underlying objects with the given objects.
    ///
    /// If the copy of @a v is not constraint-conforming according to the copy of
    /// @a c, the copy of @a e is called.
    ///
    /// @remarks All constrained template actual parameters must be @c CopyConstructible.
    constrained(const value_type & v, const constraint_type & c, const error_handler_type & e)
        : _value_and_policies(v, c, e)
    { _initialize(); }


// Assignment operators:

    /// Assignment operator for objects of different constrained template classes.
    ///
    /// Constructs a temporary object of the containing class initializing
    /// it with @a other, and then swaps the object with @c *this.
    ///
    /// This operator helps to avoid confusing situations when object of different
    /// constrained template class would be assigned, but only the value would
    /// be copied (since constrained is implicitly convertible to #value_type).
    ///
    /// @return @c *this
    ///
    /// @remarks
    /// - All the actual parameters of the constructor template must be @c Convertible
    ///   to the corresponding actual parameters of the containing class template.
    /// - All the containing class template actual parameters must be
    ///   @c CopyConstructible and @c Swappable.
    template <typename V, typename C, typename E>
    constrained & operator = (const constrained<V, C, E> & other)
    {
        constrained tmp(other);
        swap(tmp);
        return *this;
    }

    /// Assignment operator for #value_type objects.
    ///
    /// If @a v is constraint-conforming, it is assigned to the underlying
    /// #value_type object. Otherwise the error handler is called.
    ///
    /// @return @c *this
    ///
    /// @remarks #value_type must be @c Assignable.
    constrained & operator = (const value_type & v)
    {
        if( constraint()(v) )
            _value() = v;
        else
            error_handler()(_value(), v, _constraint());

        BOOST_ASSERT( constraint()(value()) );

        return *this;
    }


private:

// Private helper functions:

    /// Performs initialization check of the underlying value.
    ///
    /// If the underlying value is not constraint-conforming,
    /// the error handler is called.
    void _initialize()
    {
        if( !constraint()(value()) )
        {
            error_handler()(_value(), value(), _constraint());
            BOOST_ASSERT( constraint()(value()) );
        }
    }

    /// Non-const access to the underlying #value_type object.
    value_type & _value()
    { return _value_and_policies.value; }

    /// Non-const access to the underlying #constraint_type object.
    constraint_type & _constraint()
    { return _value_and_policies.first(); }


// The value and policies holder:

    /**
        Helper struct that holds the members of a constrained object.

        This struct is used to take advantage of Empty Base Optimization in cases
        when one or both of the policies are empty classes. They could be base
        classes of constrained instead, but this would introduce issues with their
        members (especially operators) interacting with the interface of constrained.

        <tt>boost::compressed_pair</tt> is used to allow for cases when both the
        policies are of the same type or they are of function pointer/reference type.
        This, however, has the disadvantage that if they are PODs, they won't be
        initialized by some constructors. Therefore the constructors are banned for
        non-empty POD types to avoid bugs when uninitialized policy would be used.
    */
    struct _value_and_policies_holder :
        public boost::compressed_pair<constraint_type, error_handler_type>
    {

        typedef boost::compressed_pair<constraint_type, error_handler_type> base;

        value_type value;

    // Construction:

        _value_and_policies_holder()
            : base(), value()
        {
            BOOST_STATIC_ASSERT(( !boost::is_pod<constraint_type>::value
                || boost::is_empty<constraint_type>::value ));
            BOOST_STATIC_ASSERT(( !boost::is_pod<error_handler_type>::value
                || boost::is_empty<error_handler_type>::value ));
        }

        _value_and_policies_holder(const value_type & v)
            : base(), value(v)
        {
            BOOST_STATIC_ASSERT(( !boost::is_pod<constraint_type>::value
                || boost::is_empty<constraint_type>::value ));
            BOOST_STATIC_ASSERT(( !boost::is_pod<error_handler_type>::value
                || boost::is_empty<error_handler_type>::value ));
        }

        _value_and_policies_holder(const constraint_type & c)
            : base(c), value()
        {
            BOOST_STATIC_ASSERT(( !boost::is_pod<error_handler_type>::value
                || boost::is_empty<error_handler_type>::value ));
        }

        _value_and_policies_holder(const error_handler_type & e)
            : base(e), value()
        {
            BOOST_STATIC_ASSERT(( !boost::is_pod<constraint_type>::value
                || boost::is_empty<constraint_type>::value ));
        }

        _value_and_policies_holder(const value_type & v, const constraint_type & c)
            : base(c), value(v)
        {
            BOOST_STATIC_ASSERT(( !boost::is_pod<error_handler_type>::value
                || boost::is_empty<error_handler_type>::value ));
        }

        _value_and_policies_holder(const value_type & v, const error_handler_type & e)
            : base(e), value(v)
        {
            BOOST_STATIC_ASSERT(( !boost::is_pod<constraint_type>::value
                || boost::is_empty<constraint_type>::value ));
        }

        _value_and_policies_holder(const constraint_type & c, const error_handler_type & e)
            : base(c, e), value()
        { }

        _value_and_policies_holder(
            const value_type & v, const constraint_type & c, const error_handler_type & e)
            : base(c, e), value(v)
        { }

    // Swap:

        void swap(_value_and_policies_holder & other)
        {
            base::swap(other);
            boost::swap(value, other.value);
        }

    }; // struct _value_and_policies_holder

    /// The underlying value and policies.
    _value_and_policies_holder _value_and_policies;


}; // class constrained<>

/** @relatesalso constrained
    Namespace-level swap for constrained.
    @remarks All the actual template parameters must be @c Swappable.
*/
template <typename V, typename C, typename E>
void swap(constrained<V, C, E> & lhs, constrained<V, C, E> & rhs)
{
    lhs.swap(rhs);
}



/** @relatesalso constrained
    Changes the constraint of the given constrained object.

    Creates a new object with the value and error handler of @a c, but with
    @a new_constraint as its constraint, and swaps the object with @a c.

    Note, that using this function is reasonable only when @a C objects have
    some internal state.

    @remarks
    - @a T must be @c Convertible to @a C.
    - @a V, @a C and @a E must be @c CopyConstructible and @c Swappable.
*/
template <typename V, typename C, typename E, typename T>
void change_constraint(constrained<V, C, E> & c, const T & new_constraint)
{
    constrained<V, C, E> tmp(c.value(), new_constraint, c.error_handler());
    c.swap(tmp);
}



// Increment/decrement operators:

/** @relatesalso constrained
    Pre-increment operator for constrained.
    Creates a copy of the underlying value of @a c, pre-increments it and assigns it back to @a c.
    @return @a c
    @remarks @a V must be @c CopyConstructible, @c Assignable and @c PreIncrementable.
*/
template <typename V, typename C, typename E>
constrained<V, C, E> & operator ++ (constrained<V, C, E> & c)
{
    V tmp(c.value());
    ++tmp;
    c = tmp;
    return c;
}

/** @relatesalso constrained
    Pre-decrement operator for constrained.
    Creates a copy of the underlying value of @a c, pre-decrements it and assigns it back to @a c.
    @return @a c
    @remarks @a V must be @c CopyConstructible, @c Assignable and @c PreDecrementable.
*/
template <typename V, typename C, typename E>
constrained<V, C, E> & operator -- (constrained<V, C, E> & c)
{
    V tmp(c.value());
    --tmp;
    c = tmp;
    return c;
}

/** @relatesalso constrained
    Post-increment operator for constrained.
    Creates a copy of @a c and then pre-increments @a c.
    @return The copy of @a c before the increment.
    @remarks constrained<@a V, @a C, @a E> must be @c CopyConstructible and @c PreIncrementable.
*/
template <typename V, typename C, typename E>
const constrained<V, C, E> operator ++ (constrained<V, C, E> & c, int)
{
    const constrained<V, C, E> tmp(c);
    ++c;
    return tmp;
}

/** @relatesalso constrained
    Post-decrement operator for constrained.
    Creates a copy of @a c and then pre-decrements @a c.
    @return The copy of @a c before the decrement.
    @remarks constrained<@a V, @a C, @a E> must be @c CopyConstructible and @c PreDecrementable.
*/
template <typename V, typename C, typename E>
const constrained<V, C, E> operator -- (constrained<V, C, E> & c, int)
{
    const constrained<V, C, E> tmp(c);
    --c;
    return tmp;
}



// Overloaded arithmetic and bitwise assignment operators:

#ifdef BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR
#    error "BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR already defined."
#else
#    define BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(_op_, _op_name_)            \
                                                                                     \
/** @relatesalso constrained                                                      */ \
/** _op_name_ operator for constrained and @a T.                                  */ \
/** Creates a copy of the underlying value of @a c, applies the operator to       */ \
/** the copy using the value of @a v and assigns the new value back to @a c.      */ \
/** @return @a c @n                                                               */ \
/** @remarks                                                                      */ \
/** @li @a V must be @c CopyConstructible and @c Assignable.                      */ \
/** @li _op_name_ operator must be available for arguments of type @a V and @a T. */ \
template <typename V, typename C, typename E, typename T>                            \
constrained<V, C, E> & operator _op_##= (constrained<V, C, E> & c, const T & v)      \
{                                                                                    \
    V tmp(c.value());                                                                \
    tmp _op_##= v;                                                                   \
    c = tmp;                                                                         \
    return c;                                                                        \
}

BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(+, +=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(-, -=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(*, *=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(/, /=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(%, \%=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(^, ^=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(&, &=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(|, |=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(>>, >>=)
BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR(<<, <<=)

#    undef BOOST_DEFINE_CONSTRAINED_ASSIGNMENT_OPERATOR
#endif



// Stream input/output operators:

#ifndef BOOST_NO_IOSFWD

/** @relatesalso constrained
    Stream extraction operator for constrained.

    Creates a variable that is a copy of the value of @a c and
    calls stream extraction operator for the variable using @a is.
    If this succeeds, assigns the variable to @a c.

    Note, that this operator reads only the underlying value
    from the stream, but not the constraint nor the error policy.

    @return @a is
    @remarks
    - @a V must be @c InputStreamable and @c CopyConstructible.
    - This function template is not defined if @c BOOST_NO_IOSFWD macro is defined.
*/
template <typename Ch, class Tr, typename V, typename C, typename E>
std::basic_istream<Ch, Tr> &
operator >> (std::basic_istream<Ch, Tr> & is, constrained<V, C, E> & c)
{
    V tmp(c.value());
    is >> tmp;
    if( is )
        c = tmp;
    return is;
}

/** @relatesalso constrained
    Stream insertion operator for constrained.

    Outputs the value of @a c to @a os.

    Note, that this operator writes only the underlying value
    to the stream, but not the constraint nor the error policy.

    @return @a os
    @remarks
    - @a V must be @c OutputStreamable.
    - This function template is not defined if @c BOOST_NO_IOSFWD macro is defined.
*/
template <typename Ch, class Tr, typename V, typename C, typename E>
std::basic_ostream<Ch, Tr> &
operator << (std::basic_ostream<Ch, Tr> & os, const constrained<V, C, E> & c)
{
    os << c.value();
    return os;
}

#endif // BOOST_NO_IOSFWD



} // namespace constrained_value
} // namespace boost

#endif // BOOST_CONSTRAINED_VALUE_CONSTRAINED_HPP
