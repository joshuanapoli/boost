#ifndef BOOST_QUATERNION_IO_HPP
#define BOOST_QUATERNION_IO_HPP

#include <iosfwd>                                    // for the "<<" and ">>" operators
#include <sstream>                                    // for the "<<" operator
#ifndef    BOOST_NO_STD_LOCALE
    #include <locale>                                    // for the "<<" operator
#endif /* BOOST_NO_STD_LOCALE */

#include <boost/math/quaternion.hpp>

namespace boost
{
    namespace math
    {
        // Note:    we allow the following formats, whith a, b, c, and d reals
        //            a
        //            (a), (a,b), (a,b,c), (a,b,c,d)
        //            (a,(c)), (a,(c,d)), ((a)), ((a),c), ((a),(c)), ((a),(c,d)), ((a,b)), ((a,b),c), ((a,b),(c)), ((a,b),(c,d))
#if    BOOST_WORKAROUND(__GNUC__, < 3)
        template<typename T>
        std::istream &                            operator >> (    ::std::istream & is,
                                                                quaternion<T> & q)
#else
        template<typename T, typename charT, class traits>
        ::std::basic_istream<charT,traits> &    operator >> (    ::std::basic_istream<charT,traits> & is,
                                                                quaternion<T> & q)
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
        {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
            typedef char    charT;
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
            
#ifdef    BOOST_NO_STD_LOCALE
#else
            const ::std::ctype<charT> & ct = ::std::use_facet< ::std::ctype<charT> >(is.getloc());
#endif /* BOOST_NO_STD_LOCALE */
            
            T    a = T();
            T    b = T();
            T    c = T();
            T    d = T();
            
            ::std::complex<T>    u = ::std::complex<T>();
            ::std::complex<T>    v = ::std::complex<T>();
            
            charT    ch = charT();
            char    cc;
            
            is >> ch;                                        // get the first lexeme
            
            if    (!is.good())    goto finish;
            
#ifdef    BOOST_NO_STD_LOCALE
            cc = ch;
#else
            cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
            
            if    (cc == '(')                            // read "(", possible: (a), (a,b), (a,b,c), (a,b,c,d), (a,(c)), (a,(c,d)), ((a)), ((a),c), ((a),(c)), ((a),(c,d)), ((a,b)), ((a,b),c), ((a,b),(c)), ((a,b,),(c,d,))
            {
                is >> ch;                                    // get the second lexeme
                
                if    (!is.good())    goto finish;
                
#ifdef    BOOST_NO_STD_LOCALE
                cc = ch;
#else
                cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                
                if    (cc == '(')                        // read "((", possible: ((a)), ((a),c), ((a),(c)), ((a),(c,d)), ((a,b)), ((a,b),c), ((a,b),(c)), ((a,b,),(c,d,))
                {
                    is.putback(ch);
                    
                    is >> u;                                // we extract the first and second components
                    a = u.real();
                    b = u.imag();
                    
                    if    (!is.good())    goto finish;
                    
                    is >> ch;                                // get the next lexeme
                    
                    if    (!is.good())    goto finish;
                    
#ifdef    BOOST_NO_STD_LOCALE
                    cc = ch;
#else
                    cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                    
                    if        (cc == ')')                    // format: ((a)) or ((a,b))
                    {
                        q = quaternion<T>(a,b);
                    }
                    else if    (cc == ',')                // read "((a)," or "((a,b),", possible: ((a),c), ((a),(c)), ((a),(c,d)), ((a,b),c), ((a,b),(c)), ((a,b,),(c,d,))
                    {
                        is >> v;                            // we extract the third and fourth components
                        c = v.real();
                        d = v.imag();
                        
                        if    (!is.good())    goto finish;
                        
                        is >> ch;                                // get the last lexeme
                        
                        if    (!is.good())    goto finish;
                        
#ifdef    BOOST_NO_STD_LOCALE
                        cc = ch;
#else
                        cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                        
                        if    (cc == ')')                    // format: ((a),c), ((a),(c)), ((a),(c,d)), ((a,b),c), ((a,b),(c)) or ((a,b,),(c,d,))
                        {
                            q = quaternion<T>(a,b,c,d);
                        }
                        else                            // error
                        {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                            is.setstate(::std::ios::failbit);
#else
                            is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                        }
                    }
                    else                                // error
                    {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                        is.setstate(::std::ios::failbit);
#else
                        is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                    }
                }
                else                                // read "(a", possible: (a), (a,b), (a,b,c), (a,b,c,d), (a,(c)), (a,(c,d))
                {
                    is.putback(ch);
                    
                    is >> a;                                // we extract the first component
                    
                    if    (!is.good())    goto finish;
                    
                    is >> ch;                                // get the third lexeme
                    
                    if    (!is.good())    goto finish;
                    
#ifdef    BOOST_NO_STD_LOCALE
                    cc = ch;
#else
                    cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                    
                    if        (cc == ')')                    // format: (a)
                    {
                        q = quaternion<T>(a);
                    }
                    else if    (cc == ',')                // read "(a,", possible: (a,b), (a,b,c), (a,b,c,d), (a,(c)), (a,(c,d))
                    {
                        is >> ch;                            // get the fourth lexeme
                        
                        if    (!is.good())    goto finish;
                        
#ifdef    BOOST_NO_STD_LOCALE
                        cc = ch;
#else
                        cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                        
                        if    (cc == '(')                // read "(a,(", possible: (a,(c)), (a,(c,d))
                        {
                            is.putback(ch);
                            
                            is >> v;                        // we extract the third and fourth component
                            
                            c = v.real();
                            d = v.imag();
                            
                            if    (!is.good())    goto finish;
                            
                            is >> ch;                        // get the ninth lexeme
                            
                            if    (!is.good())    goto finish;
                            
#ifdef    BOOST_NO_STD_LOCALE
                            cc = ch;
#else
                            cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                            
                            if    (cc == ')')                // format: (a,(c)) or (a,(c,d))
                            {
                                q = quaternion<T>(a,b,c,d);
                            }
                            else                        // error
                            {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                                is.setstate(::std::ios::failbit);
#else
                                is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                            }
                        }
                        else                        // read "(a,b", possible: (a,b), (a,b,c), (a,b,c,d)
                        {
                            is.putback(ch);
                            
                            is >> b;                        // we extract the second component
                            
                            if    (!is.good())    goto finish;
                            
                            is >> ch;                        // get the fifth lexeme
                            
                            if    (!is.good())    goto finish;
                            
#ifdef    BOOST_NO_STD_LOCALE
                            cc = ch;
#else
                            cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                            
                            if    (cc == ')')                // format: (a,b)
                            {
                                q = quaternion<T>(a,b);
                            }
                            else if    (cc == ',')        // read "(a,b,", possible: (a,b,c), (a,b,c,d)
                            {
                                is >> c;                    // we extract the third component
                                
                                if    (!is.good())    goto finish;
                                
                                is >> ch;                    // get the seventh lexeme
                                
                                if    (!is.good())    goto finish;
                                
#ifdef    BOOST_NO_STD_LOCALE
                                cc = ch;
#else
                                cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                                
                                if        (cc == ')')        // format: (a,b,c)
                                {
                                    q = quaternion<T>(a,b,c);
                                }
                                else if    (cc == ',')    // read "(a,b,c,", possible: (a,b,c,d)
                                {
                                    is >> d;                // we extract the fourth component
                                    
                                    if    (!is.good())    goto finish;
                                    
                                    is >> ch;                // get the ninth lexeme
                                    
                                    if    (!is.good())    goto finish;
                                    
#ifdef    BOOST_NO_STD_LOCALE
                                    cc = ch;
#else
                                    cc = ct.narrow(ch, char());
#endif /* BOOST_NO_STD_LOCALE */
                                    
                                    if    (cc == ')')        // format: (a,b,c,d)
                                    {
                                        q = quaternion<T>(a,b,c,d);
                                    }
                                    else                // error
                                    {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                                        is.setstate(::std::ios::failbit);
#else
                                        is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                                    }
                                }
                                else                    // error
                                {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                                    is.setstate(::std::ios::failbit);
#else
                                    is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                                }
                            }
                            else                        // error
                            {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                                is.setstate(::std::ios::failbit);
#else
                                is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                            }
                        }
                    }
                    else                                // error
                    {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
                        is.setstate(::std::ios::failbit);
#else
                        is.setstate(::std::ios_base::failbit);
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
                    }
                }
            }
            else                                        // format:    a
            {
                is.putback(ch);
                
                is >> a;                                    // we extract the first component
                
                if    (!is.good())    goto finish;
                
                q = quaternion<T>(a);
            }
            
            finish:
            return(is);
        }
        
        
#if    BOOST_WORKAROUND(__GNUC__, < 3)
        template<typename T>
        ::std::ostream &                         operator << (    ::std::ostream & os,
                                                                quaternion<T> const & q)
#else
        template<typename T, typename charT, class traits>
        ::std::basic_ostream<charT,traits> &    operator << (    ::std::basic_ostream<charT,traits> & os,
                                                                quaternion<T> const & q)
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
        {
#if    BOOST_WORKAROUND(__GNUC__, < 3)
            ::std::ostringstream                        s;
#else
            ::std::basic_ostringstream<charT,traits>    s;
#endif /* BOOST_WORKAROUND(__GNUC__, < 3) */
            
            s.flags(os.flags());
#ifdef    BOOST_NO_STD_LOCALE
#else
            s.imbue(os.getloc());
#endif /* BOOST_NO_STD_LOCALE */
            s.precision(os.precision());
            
            s << '('    << q.R_component_1() << ','
                        << q.R_component_2() << ','
                        << q.R_component_3() << ','
                        << q.R_component_4() << ')';
            
            return os << s.str();
        }
    }
}
