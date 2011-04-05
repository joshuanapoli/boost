# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.
#  *     (C) Copyright Edward Diener 2011.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
# define BOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
#
# include <boost/preprocessor/config/variadics.hpp>
#
#if BOOST_PP_VARIADICS
#
# include <boost/preprocessor/config/config.hpp>
#
# /* BOOST_PP_OVERLOAD detail macros */
#
#define BOOST_PP_OV_DETAIL_APPLY_VAR_I(macro, args) \
  macro args \
/**/
#define BOOST_PP_OV_DETAIL_APPLY_VAR(macro, args) \
  BOOST_PP_OV_DETAIL_APPLY_VAR_I(macro, args) \
/**/
#
#define BOOST_PP_OV_DETAIL_ARG_N( \
  A1,A2,A3,A4,A5,A6,A7,A8,A9,A10, \
  A11,A12,A13,A14,A15,A16,A17,A18,A19,A20, \
  A21,A22,A23,A24,N,...) N \
/**/
#define BOOST_PP_OV_DETAIL_RSEQ_N() \
  24,23,22,21,20, \
  19,18,17,16,15,14,13,12,11,10, \
  9,8,7,6,5,4,3,2,1,0 \
/**/
#
# if BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MSVC()
#    define BOOST_PP_OV_DETAIL_CAT(a, b) BOOST_PP_OV_DETAIL_CAT_I(a, b)
#    define BOOST_PP_OV_DETAIL_CAT_I(a, b) BOOST_PP_OV_DETAIL_CAT_II(a ## b)
#    define BOOST_PP_OV_DETAIL_CAT_II(res) res
# include <boost/preprocessor/facilities/empty.hpp>
#define BOOST_PP_OV_DETAIL_SIZE(...) \
  BOOST_PP_OV_DETAIL_CAT(BOOST_PP_OV_DETAIL_APPLY_VAR(BOOST_PP_OV_DETAIL_ARG_N, (__VA_ARGS__, BOOST_PP_OV_DETAIL_RSEQ_N())),BOOST_PP_EMPTY()) \
/**/
#else
#define BOOST_PP_OV_DETAIL_SIZE(...) \
  BOOST_PP_OV_DETAIL_APPLY_VAR(BOOST_PP_OV_DETAIL_ARG_N, (__VA_ARGS__, BOOST_PP_OV_DETAIL_RSEQ_N())) \
/**/
#endif
#
#define BOOST_PP_OV_SIZE(...) \
  BOOST_PP_OV_DETAIL_SIZE(__VA_ARGS__) \
/**/
#
# /* BOOST_PP_OVERLOAD */
#
#define BOOST_PP_OVERLOAD(prefix, ...) \
  BOOST_PP_OV_CAT(prefix, BOOST_PP_OV_SIZE(__VA_ARGS__)) \
/**/
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MWCC()
#    define BOOST_PP_OV_CAT(a, b) BOOST_PP_OV_CAT_I(a, b)
# else
#    define BOOST_PP_OV_CAT(a, b) BOOST_PP_OV_CAT_OO((a, b))
#    define BOOST_PP_OV_CAT_OO(par) BOOST_PP_OV_CAT_I ## par
# endif
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MSVC()
#    define BOOST_PP_OV_CAT_I(a, b) a ## b
# else
#    define BOOST_PP_OV_CAT_I(a, b) BOOST_PP_OV_CAT_II(a ## b)
#    define BOOST_PP_OV_CAT_II(res) res
# endif
#
#endif // BOOST_PP_VARIADICS
#endif // BOOST_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
