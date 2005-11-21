#ifndef BOOST_ARCHIVE_ARRAY_BINARY_OARCHIVE_HPP
#define BOOST_ARCHIVE_ARRAY_BINARY_OARCHIVE_HPP

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// (C) Copyright 2005 Matthias Troyer. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/array/optimized.hpp>
#include <boost/type_traits/is_fundamental.hpp>

namespace boost { namespace archive { namespace array {

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from binary_oarchive_impl instead.  This will
// preserve correct static polymorphism.

class binary_oarchive : 
    public binary_oarchive_impl<binary_oarchive>, 
    public optimized<binary_oarchive>
{
public:
    binary_oarchive(std::ostream & os, unsigned int flags = 0) :
        binary_oarchive_impl<binary_oarchive>(os, flags)
    {}

    // we provide an optimized save for all fundamental types
    
    template <class ValueType>
    struct use_array_optimization 
      : public is_fundamental<ValueType> 
    {};

    // the optimized save_array dispatches to save_binary 
    
    template <class ValueType>
    void save_array(ValueType const* p, std::size_t n, unsigned int)
    {
      save_binary(p,n*sizeof(ValueType));
    }
};


} } }  // end namespace boost::archive::array

// required by smart_cast for compilers not implementing 
// partial template specialization
BOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(boost::archive::array::binary_oarchive)

#endif // BOOST_ARCHIVE_ARRAY_BINARY_OARCHIVE_HPP
