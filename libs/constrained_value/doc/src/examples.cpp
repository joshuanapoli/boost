/**
@file    examples.cpp
@brief   Examples used in documentation for Boost Constrained Value library.
@version 2.3
@date    2008.11.30
@author  Robert Kawulak

This file is a part of the Boost Constrained Value library.
See http://www.boost.org/libs/constrained_value/ for library home page.

Copyright (C) 2004-2008 Robert Kawulak (Robert.Kawulak.at.gmail.com).
Distributed under the Boost Software License, Version 1.0 (see accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
*/

/*
Note: the code contains documentation examples only, but it should be
possible to compile and run it with no errors to ensure its correctness.
*/

#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <exception>
#include <string>
#include <vector>

#include <boost/lambda/lambda.hpp>
#include <boost/rational.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/constrained_value.hpp>
namespace cv = boost::constrained_value;



// these macro names are used to create links
// when the file is imported by QuickBook

#define LINK_bounded                      cv::bounded
#define LINK_bounded_int                  cv::bounded_int
#define LINK_broken_constraint            cv::broken_constraint
#define LINK_clip                         cv::clip
#define LINK_clipping                     cv::clipping
#define LINK_clipping_int                 cv::clipping_int
#define LINK_constrained                  cv::constrained
#define LINK_no_constraint                cv::no_constraint
#define LINK_throw_exception              cv::throw_exception
#define LINK_unconstrained                cv::unconstrained
#define LINK_within_bounds                cv::within_bounds
#define LINK_wrap                         cv::wrap
#define LINK_wrapping                     cv::wrapping
#define LINK_wrapping_int                 cv::wrapping_int

#define LINK_change_constraint            cv::change_constraint
#define LINK_change_bounds                cv::change_bounds
#define LINK_change_lower_bound           cv::change_lower_bound
#define LINK_change_upper_bound           cv::change_upper_bound
#define LINK_change_lower_bound_exclusion cv::change_lower_bound_exclusion
#define LINK_change_upper_bound_exclusion cv::change_upper_bound_exclusion

#define LINK_boost_function               boost::function
#define LINK_BOOST_THROW_EXCEPTION        BOOST_THROW_EXCEPTION
#define LINK_boost_mpl_true_              boost::mpl::true_
#define LINK_boost_mpl_false_             boost::mpl::false_
#define LINK_boost_mpl_integral_c         boost::mpl::integral_c
#define LINK_boost_lambda_1               boost::lambda::_1
#define LINK_boost_rational               boost::rational
#define LINK_BOOST_STATIC_ASSERT          BOOST_STATIC_ASSERT



// use this macro in places where exception is expected
// to ensure the code can be executed without errors

#define EAT_EXCEPTION( expr ) \
try {                         \
    expr                      \
    assert(false);            \
}                             \
catch(...) {                  \
}



// Motivation
//[ex_motivation

/*`
The Boost Constrained Value library contains class templates useful for creating [link constrained_object_def constrained objects]. A simple example is an object representing an hour of a day, for which only integers from the range \[0, 23] are valid values:
*/

/*<-*/ void ex_motivation1() { /*->*/

LINK_bounded_int<int, 0, 23>::type hour;
hour = 20; // OK
/*<-*/ EAT_EXCEPTION( /*->*/hour = 26;/*<-*/ ) /*->*/ // exception!

/*`
Behavior in case of assignment of an invalid value can be customized. For instance, instead of throwing an exception as in the example above, the value may be adjusted to meet the constraint:
*/

LINK_wrapping_int<int, 0, 255>::type buffer_index;
buffer_index = 257; // OK: adjusts the value to fit in the range by wrapping it
assert( buffer_index == 1 );

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

/*`
The library doesn't focus only on [link bounded_object_def bounded objects] as in the examples above -- virtually any constraint can be imposed by using a predicate:
*/

// constraint (a predicate)
struct is_odd {
    bool operator () (int i) const
    { return (i % 2) != 0; }
};
/*<-*/ void ex_motivation2() { /*->*/
// and the usage is as simple as:
LINK_constrained<int, is_odd> odd_int = 1;
odd_int += 2; // OK
/*<-*/ EAT_EXCEPTION( /*->*/++odd_int;/*<-*/ ) /*->*/ // exception!

/*`
The library has a policy-based design to allow for flexibility in defining constraints and behavior in case of assignment of invalid values. Policies may be configured at compile-time for maximum efficiency or may be changeable at runtime if such dynamic functionality is needed.
*/

/*<-*/ } /*->*/

//]



// Simple constrained objects
//[ex_basic

/*`
The first step to use a [link constrained_object_def constrained object] is to define a constraint, which is simply a [@http://www.sgi.com/tech/stl/Predicate.html predicate]. For instance, if you want to have an object which always represents an even integer number, you can define the predicate like this:
*/

struct is_even {
    bool operator () (int i) const
    { return (i % 2) == 0; }
};

/*`
[note If the constraint is specified as a function object type like in this example, the invocation operator must be a `const` member function.]

Now you can create an object which is a wrapper for `int` and can have only an even value:
*/

typedef LINK_constrained<int, is_even> even_int;
even_int i;

/*`
Once a constrained object is created, it can be used in almost the same way as the underlying value type object. All the mutating (and stream extraction/insertion) operators are overloaded, so you can write:
*/

/*<-*/ void ex_basic() { /*->*/

i = 4;
i += 2;

/*`
However, the crucial difference between `even_int` and `int` is that the former will not let being assigned any odd value. In case of an attempt to assign such value it will use its error policy which, by default, throws an exception of `LINK_broken_constraint` class:
*/

/*<-*/ EAT_EXCEPTION( /*->*/i = 1;/*<-*/ ) /*->*/ // exception!

/*`
In the example above, the assignment is *not* performed and exception is thrown instead.

The same applies to initialization of constrained objects with invalid values:
*/

even_int a; // OK
even_int b(2); // OK
/*<-*/ EAT_EXCEPTION( /*->*/even_int c(3);/*<-*/ ) /*->*/ // exception!

/*`
In the first example, `a` is default-constructed which means the underlying value is value-initialized (so for `int` it will have the value of `0`).

[tip `LINK_constrained` objects are implicitly convertible to and from the underlying value type, making them easily interchangeable.]

For example, the statements below employ implicit conversion from `even_int` to `int`:
*/

int x = i; // initializes x with the underlying value of i
std::abs(i); // calls std::abs(int)

/*`
The implicit conversion is convenient, but there are some situations when it won't work. In such cases you can extract the value explicitly using the `value()` member function:
*/

// std::max(2, i); // won't compile: template parameter is ambiguous (int or even_int?)
std::max(2, i.value()); // OK: template parameter is int

/*`
Constrained objects can be read from and written into streams just like objects of the underlying value type.

[important Stream extraction/insertion operators for `LINK_constrained` read/write the underlying value only, not the whole constrained object with its constraint and error policy.]

Examples of using a constrained object with streams (assuming the "classic" locale is used) are shown below:
*/

std::stringstream stream;

i = 10;
stream << i;
assert( stream.str() == "10" );

stream.str("12 13");
stream >> i; // OK: 12 is even
assert( i == 12 );
/*<-*/ EAT_EXCEPTION( /*->*/stream >> i;/*<-*/ ) /*->*/ // exception: 13 is not even, i is not changed

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

//]



// Changing constraints at runtime
//[ex_runtime_constraint

/*`
Sometimes it may be useful to be able to change constraint of a constrained object at run-time. This is possible if a callback type (such as function pointer type or `LINK_boost_function`) is used as the constraint type. By default, `LINK_constrained` class template uses the latter if constraint type is not specified, so any type of constraint can be used:
*/

LINK_constrained<int> c(2, is_even());

/*`
In the line above, we define a constrained object `c` which initially has an object of previously defined `is_even` class as its constraint.

[important Note, that when a callback type is used as the type of constraint, the constraint must always be initialized properly when constructing a `LINK_constrained` object because it is invoked during the construction. Error policy may also be invoked if `LINK_constrained` object is initialized with an invalid value -- in such case it must be initialized properly too.]

Once we have a constrained object with dynamic constraint policy, we can change the constraint using `LINK_change_constraint()` function:
*/

struct is_positive
{
    bool operator () (int i) const
    { return i > 0; }
};
/*<-*/ void ex_runtime_constraint1() { /*->*/
LINK_change_constraint(c, is_positive());
/*<-*/ } /*->*/

/*`
From now on, `c` will accept only positive values (previous `is_even` constraint is not used anymore).

The constraint can be of any kind of callable object accepted by `LINK_boost_function`. For instance, it may be a function pointer, as in the example below:
*/

bool is_positive_fn(int i)
{
    return i > 0;
}
/*<-*/ void ex_runtime_constraint2() { /*->*/
LINK_change_constraint(c, &is_positive_fn);

/*`
LINK_Lambda_expressions can be used too, so any of the two examples above can be rewritten as:
*/

using LINK_boost_lambda_1;

LINK_change_constraint(c, _1 > 0);

/*`
Trying to change the constraint if the current value of a constrained object does not meet the new constraint will cause the error policy to be called:
*/

LINK_constrained<int> c(-2, is_even());
/*<-*/ EAT_EXCEPTION( /*->*/LINK_change_constraint(c, is_positive());/*<-*/ ) /*->*/ // exception: -2 is not positive!

/*`
[note Constraint of a constrained object cannot be accessed directly for modification, because the underlying value could become invalid according to the modified constraint. Therefore the constraint of a `LINK_constrained` object is immutable and `LINK_change_constraint()` function has to be used in order to modify the constraint. The function actually constructs a new object with the new constraint but the old value and error handler, and if this succeeds, swaps the old object with the new object.]
*/

/*<-*/ } /*->*/

//]



// Custom error policies
//[ex_error_policy

/*`
By default, constrained objects use `LINK_throw_exception<LINK_broken_constraint>` template class as their error policy. All it does is using `LINK_BOOST_THROW_EXCEPTION` macro with object of the specified type (here, `LINK_broken_constraint`) as the argument.

The default type of exceptions thrown can be changed, as in the example below:
*/

struct my_exception : public std::exception { }; // must derive from std::exception
/*<-*/ void ex_error_policy1() { /*->*/
LINK_constrained<int, is_even, LINK_throw_exception<my_exception> > c;
/*<-*/ EAT_EXCEPTION( /*->*/c = 1;/*<-*/ ) /*->*/ // throws my_exception()

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

/*`
You can also change the behavior of `LINK_constrained` objects by defining your own error policy. When an invalid value is assigned, `LINK_constrained` object does not actually assign it but calls its error policy instead. Error policy is a callable object with a signature like in the example below:
*/

struct ignore
{
    template <typename V, class C>
    void operator () (V & value, const V & new_value, C & constraint) const
    {
        if( !constraint(value) )
            throw LINK_broken_constraint();
    }
};

/*`
The policy defined above simply ignores any assignment of invalid value (given as the second argument), leaving the current underlying value (the first argument) unchanged.

Note, that error policy cannot be just an empty function -- it is obliged to either leave `LINK_constrained` object in a valid state if it returns (by properly modifying the underlying value or the constraint) or not to return at all (e.g., by throwing an exception). Therefore, when a `LINK_constrained` object is constructed with an invalid value, the policy cannot simply ignore the operation (this would allow for construction of invalid objects). This special case is handled in the body of the invocation operator: an exception is thrown, preventing construction of invalid `LINK_constrained` object.

The effects of using `ignore` policy are presented below:
*/

/*<-*/ void ex_error_policy2() { /*->*/

typedef LINK_constrained<int, is_even, ignore> ignoring_even_int;
ignoring_even_int i(2);

i = 4; // OK
++i; // error -- operation ignored (and no exception thrown)
assert( i == 4 );
i += 2; // OK
assert( i == 6 );

/*<-*/ EAT_EXCEPTION( /*->*/ignoring_even_int j(1);/*<-*/ ) /*->*/ // exception! (cannot ignore construction with invalid value)

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

//]



// Bounded objects
//[ex_bounded

/*<-*/ void ex_bounded() { /*->*/

/*`
A very common case of constrained objects are [link bounded_object_def bounded objects]. To simplify their creation, the library contains `LINK_within_bounds` predicate and the alias of `LINK_constrained` called `LINK_bounded`, which employs this predicate as the constraint. It can be used as follows:
*/

typedef LINK_bounded<int>::type bint;
bint i;

/*`
[note Note the `::type` after the template class name -- this library uses nested typedef technique for template aliases, since there are no true template aliases in C++ (yet).]

Bounded object `i` has default-initialized bounds. The bounds can be changed using suitable functions:
*/

LINK_change_lower_bound(i, -5);
LINK_change_upper_bound(i, 5);

/*`
Both the bounds can be changed at one time as well:
*/

LINK_change_bounds(i, -5, 5);


/*`
The abovementioned functions simply call `LINK_change_constraint()` with appropriate arguments.

The bounds can also be set during construction of a bounded object:
*/

/*<-*/ { /*->*/

bint i(bint::constraint_type(-5, 5));

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

/*`
In all cases the effect is the same: `i` can be assigned only a value that belongs to the \[-5, 5] interval:
*/

i = 5;
/*<-*/ EAT_EXCEPTION( /*->*/++i;/*<-*/ ) /*->*/ // exception!

/*`*/

i = -4;
/*<-*/ EAT_EXCEPTION( /*->*/i -= 2;/*<-*/ ) /*->*/ // exception!

/*`
Current bounds' values can be read in the following way:
*/

assert( i.constraint().lower_bound() == -5 );
assert( i.constraint().upper_bound() == 5 );

/*`
Trying to change the bounds so that the underlying value would be outside of the new bounds triggers the error policy:
*/

bint j(-5, bint::constraint_type(-10, 10));
/*<-*/ EAT_EXCEPTION( /*->*/LINK_change_lower_bound(j, 0);/*<-*/ ) /*->*/ // exception: -5 is not in [0, 10]

/*`
[note In order to use a type as the underlying type of a bounded object, a [@http://en.wikipedia.org/wiki/Strict_weak_ordering strict weak ordering] relation must be specified for it. By default, `operator < ()` for the type is used, but this behavior can be customized by providing a comparison predicate as the one of `LINK_bounded` template's parameters.]

[warning Built-in C++ floating point types should not be used to form bounded objects (see [link constrained_value.rationale.no_floats rationale] for explanation).]
*/

/*<-*/ } /*->*/

//]



// Bounded objects with open ranges
//[ex_open_range

/*<-*/ void ex_open_range() { /*->*/

/*`
By default, both the bounds are included in the allowed range and this trait cannot be changed at runtime. The behavior can be overridden by specifying explicitly the type of bounds exclusion.

To be able to change bounds exclusion at runtime, define a bounded object as shown below (some of the parameters of `LINK_bounded`, which were given default values in the previous example, need to be specified explicitly -- their meaning is explained in the comments):
*/

typedef LINK_boost_rational<int> rational; // helper typedef for brevity

typedef LINK_bounded<
    rational, // the underlying value type
    rational, // type of the lower bound (by default same as value type)
    rational, // type of the upper bound (by default same as lower bound type)
    LINK_throw_exception<>, // error policy (by default throw_exception<>)
    bool, // lower bound exclusion indicator type (by default boost::mpl::false_)
    bool  // upper bound exclusion indicator type (by default same as the lower one)
>::type bounded_rational;

/*`
When omitted, the bounds exclusion indicators default to `LINK_boost_mpl_false_`. Objects of this type are empty and do not occupy space within a bounded object, they are also implicitly convertible to `bool` yielding `false` (the value means that the bounds are not excluded from the allowed range). However, to be able to change bounds exclusion at runtime, `bool` type has to be used instead to allow for storing the state of bounds exclusion and change it.

You can use objects of `bounded_rational` type, defined above, in the following way:
*/

bounded_rational r(bounded_rational::constraint_type(0, 1, false, true));

/*`
Currently, `r` allows for values in the range \[0, 1):
*/

r = rational(99, 100);
/*<-*/ EAT_EXCEPTION( /*->*/r += rational(1, 100);/*<-*/ ) /*->*/ // exception!

/*`
You can query and change the status of bounds inclusion in the following way:
*/

assert( r.constraint().upper_bound_excluded() == true );
LINK_change_upper_bound_exclusion(r, false);

/*`
Now, both bounds are included in the allowed range, which is [0, 1]:
*/

r = rational(99, 100);
r += rational(1, 100); // OK

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

//]



// Compile-time fixed bounds
//[ex_fixed_bounds

/*`
Values of bounds are often associated with a concept represented by some bounded objects and don't change. For instance, variables representing percentage of completion of some task often lay in the range [0, 100] and that range is fixed. An alias of `LINK_bounded` called `LINK_bounded_int` can be used to express such types easily:
*/

/*<-*/ void ex_fixed_bounds1() { /*->*/

typedef LINK_bounded_int<int, 0, 100>::type percent;
percent p;

p = 50; // OK
/*<-*/ EAT_EXCEPTION( /*->*/p = 200;/*<-*/ ) /*->*/ // exception!

/*`
In this case, the bounds have to be of the same type as the underlying value type and values of this type must be allowed to be used as non-type template arguments (so only integral types and some pointer types are allowed).

Bounds are stored using `LINK_boost_mpl_integral_c`, so they do not occupy any space in a bounded object and can't be changed at runtime. The bounds are compile-time constants and can be read in two ways:
*/

assert( p.constraint().upper_bound() == 100 );
LINK_BOOST_STATIC_ASSERT( percent::constraint_type::upper_type::value == 100 );

/*`
[caution Using `LINK_bounded_int` (as any other type in this library) does not, by itself, protect from integer overflows (see [link constrained_value.rationale.overflows rationale section] for discussion on this topic).]
*/

/*<-*/ } /*->*/

/*`
As mentioned before, `LINK_bounded_int` can be used only for bounds that can be passed as template arguments. For other cases there is another method of creating bounded objects with bounds fixed at compile-time. The trick is to "convert" a value into a type, i.e. create a type that can be converted to the desired type yielding the value:
*/

typedef LINK_boost_rational<int> rational;

struct quarter2type // returns 1/4
{
    operator rational () const { return rational(1, 4); }
};

struct half2type // returns 1/2
{
    operator rational () const { return rational(1, 2); }
};

/*`
Using the types above, we can create a bounded object that always represents a rational number from the range \[1\/4, 1\/2]:
*/

/*<-*/ void ex_fixed_bounds2() { /*->*/

LINK_bounded<rational, quarter2type, half2type>::type my_rational(rational(1, 2));
my_rational = rational(1, 3); // OK
/*<-*/ EAT_EXCEPTION( /*->*/my_rational *= 2;/*<-*/ ) /*->*/ // exception: 2/3 > 1/2

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

//]



// Other error policies for bounded objects
//[ex_other_bounded

/*<-*/ void ex_other_bounded() { /*->*/

/*`
The library includes `LINK_wrap` error policy for bounded objects, which wraps out-of-bounds values over the bounds rather than throwing an exception. This allows for easy creation of objects that behave according to the modulo arithmetic. There are also two aliases of `LINK_bounded` defined: `LINK_wrapping` and `LINK_wrapping_int`, which use this error policy. An example is presented below:
*/

LINK_wrapping_int<int, 0, 359>::type angle = 180;

angle += 360;
assert( angle == 180 );

angle += 180;
assert( angle == 0 );

/*`
[important `LINK_wrap` error policy is suitable only for types representing integer numbers or random access iterators.]

Another error policy for bounded objects is `LINK_clip`, which clips values to the bounds. There are also two aliases of `LINK_bounded` defined for convenience: `LINK_clipping` and `LINK_clipping_int`. Below is an example of how the policy works:
*/

LINK_clipping_int<int, 0, 120>::type velocity;

// accelerating:
velocity += 50;
assert( velocity == 50 );

// accelerating more:
velocity += 100;
assert( velocity == 120 ); // cannot go faster than this

// decelerating:
velocity -= 100;
assert( velocity == 20 );

// decelerating more:
velocity -= 50;
assert( velocity == 0 ); // cannot go slower than this

/*`
[important When using `LINK_wrap` or `LINK_clip` error policy, the allowed range must always be valid (the upper bound cannot be less than the lower bound).]
*/

/*<-*/ } /*->*/

//]



// Using constrained objects in debug mode only
//[ex_unconstrained

/*`
Constrained objects can be used as a debugging tool to verify that a program operates only on an assumed subset of possible values. However, it is usually desirable to check the assumptions only in debug mode and avoid the checks in release code for performance reasons. The most straightforward approach to this problem would be to use a compile-time switch to turn the constraint-checking on in debug mode and off in release mode:
*/

#ifndef NDEBUG
typedef LINK_bounded_int<int, 0, 100>::type my_int;
#else
//<- avoid two different definitions of my_int when NDEBUG is defined
#ifndef NDEBUG
//->
typedef int my_int;
//<-
#endif
//->
#endif

/*`
However, the interface of `my_int` would be different in debug mode (when it would be a specialization of `LINK_constrained`) and in release mode (when it would be the built-in `int` type). This could lead to problems like in the example below:
*/

//<-
#ifndef NDEBUG
//->
my_int x = 4;
my_int y = std::max(x.value(), 2);
//<-
#endif
//->

/*`
Here, `my_int` couldn't be defined as `int`, because it doesn't have `value()` member function. On the other hand, this member function has to be called when `my_int` is `LINK_constrained`, because the implicit conversion will not work for arguments of template functions like `std::max()`.

To avoid this problem, the library defines an alias of `LINK_constrained` called `LINK_unconstrained`. It uses a dummy constraint (`LINK_no_constraint`) returning `true` for any value, and thus does not impose any constraint on the assigned values. Therefore, the definition of `my_int` may look like this:
*/

#ifndef NDEBUG
typedef LINK_bounded_int<int, 0, 100>::type my_int;
#else
typedef LINK_unconstrained<int>::type my_int;
#endif

/*`
Types created using `LINK_unconstrained` template provide the interface of `LINK_constrained`, but turn the constraint-checking off permanently. The implementation is optimized to allow a good compiler to generate code that is virtually as efficient as if using the bare underlying type.
*/

//]



// Bounded-length string
//[ex_bounded_length_string

/*`
The bounds of a bounded object don't actually have to be of the same type as the underlying value. In this section a more complex example of a bounded object is presented, which is a string with length laying only within specified bounds.

The first step is creation of a comparison predicate that can compare strings with the bounds (the bounds will represent string length in this case):
*/

struct compare_string_length {

    bool operator () (const std::string & s, std::string::size_type l) const
    { return s.length() < l; }

    bool operator () (std::string::size_type l, const std::string & s) const
    { return l < s.length(); }

};

/*`
This predicate returns `true` when length of a string passed as one of the arguments is less than length passed as another argument.

Having the comparison predicate, we can already define a type of bounded-length strings:
*/

/*<-*/ void ex_bounded_length_string() { /*->*/

typedef LINK_bounded<
    std::string, // the underlying value type
    std::string::size_type, // lower bound type
    std::string::size_type, // upper bound type
    LINK_throw_exception<>, // error policy
    LINK_boost_mpl_false_, // lower bound not excluded
    LINK_boost_mpl_false_, // upper bound not excluded
    compare_string_length // comparison of values and bounds
>::type bounded_length_string;

/*`
Since the comparison predicate used by `LINK_within_bounds` is the last parameter of `LINK_bounded`, we need to explicitly specify all the other parameters as they will not be given their default values.

Having defined the bounded-length string type, we can use it like this:
*/

bounded_length_string s; // s can store only empty strings as for now
LINK_change_upper_bound(s, 8); // s can now store strings with length of 0 to 8 characters
s = "bounded"; // OK: s is 7 characters long
/*<-*/ EAT_EXCEPTION( /*->*/s += "_length_string";/*<-*/ ) /*->*/ // exception: s would be too long!

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

//]



// Wrapping iterator
//[ex_iterator

/*<-*/ void ex_iterator() { /*->*/

/*`
Random access iterators may also be used as the underlying type of a bounded object -- for example, a type of const iterator wrapping over the bounds of a vector can be defined as below:
*/

typedef LINK_wrapping<std::vector<int>::const_iterator>::type wrapping_iter_t;

/*`
Now, having a vector populated with some sample values'''&#8230;'''
*/

std::vector<int> v;
for( int i = 0; i < 10; ++i )
    v.push_back(i);

/*`
'''&#8230;'''we can define a wrapping iterator for this vector object:
*/

wrapping_iter_t iter(v.begin(), wrapping_iter_t::constraint_type(v.begin(), v.end() - 1));

/*`
The first argument is the initial value, the second one defines the range over which the iterator will wrap (here, we define an iterator wrapping over the whole range of the vector).

[note For wrapping objects, the upper bound must always be included in the allowed range, so for a wrapping iterator it must point to the last element in the range (not the one-past-last as in the STL convention).]

This is how we may check whether the wrapping iterator actually works:
*/

for( int i = 0; i < 20; ++i ) // iterate over v twice in one loop
    assert( *((iter++).value()) == i % 10 );

/*`
[important Bounded iterators and their bounds may become invalid after some mutating operations performed on their container, just like normal iterators.]
*/

/*<-*/ } /*->*/

//]



// Object remembering its past extreme values
//[ex_with_memory

/*`
An interesting application of the library is creation of objects which remember extreme (the least and the greatest) values that has been assigned to them throughout their lifetime.

We can use bounded objects to store the current value together with the pair of extreme values (which will be represented by the bounds). In the beginning, both the bounds will be equal to the initial value. Once the object is created, assigning any value not fitting in the range will trigger the error policy, which will simply expand the bounds to make the new value fit within them. However, if the new value already fits within the current range, the bounds are not changed. This way, the bounds will store the least and the greatest values that has ever been assigned to the object.

The error policy described here is implemented below:
*/

// error policy expanding the bounds to fit the new value within them
struct expanding_policy {
    template <typename V, typename C>
    void operator () (V &, const V & new_val,
        LINK_within_bounds<V, V, boost::mpl::false_, boost::mpl::false_, C> & constraint)
    {
        if( constraint.is_below(new_val) )
            constraint.lower_bound() = new_val;
        else
            constraint.upper_bound() = new_val;
    }
};

/*`
Now we can define a type of integers with extreme values' memory in a very simple way:
*/

typedef cv::bounded<int, int, int, expanding_policy>::type minmax_int;

/*`
And we can see how it works:
*/

/*<-*/ void ex_with_memory() { /*->*/

minmax_int mm(4, minmax_int::constraint_type(4, 4)); // bounds need to be initialized with the same value

mm = -1;
mm = 10;
mm = 7;

assert( mm.constraint().lower_bound() == -1 ); // lower bound -- the minimal value
assert( mm.constraint().upper_bound() == 10 ); // upper bound -- the maximal value

/*`[/end the code block, please]*/

/*<-*/ } /*->*/

//]



int main()
{
    ex_motivation1();
    ex_motivation2();
    ex_basic();
    ex_runtime_constraint1();
    ex_runtime_constraint2();
    ex_error_policy1();
    ex_error_policy2();
    ex_bounded();
    ex_open_range();
    ex_fixed_bounds1();
    ex_fixed_bounds2();
    ex_other_bounded();
    ex_bounded_length_string();
    ex_iterator();
    ex_with_memory();
}
