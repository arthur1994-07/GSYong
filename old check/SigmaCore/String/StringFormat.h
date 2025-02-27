#ifndef SC_STRING_FORMAT_H_
#define SC_STRING_FORMAT_H_

#pragma once

#include <string>
#pragma warning(disable:4996) // disable deprecated warning 
#include <boost/format.hpp>
#pragma warning(default:4996)
#include <boost/preprocessor.hpp>
#include <boost/algorithm/string.hpp>

namespace sc
{
    namespace string
    {
        boost::format formatter( const std::string& f_string );

#define X_GENERATE_FORMAT(z,n,x) %arg##n

#define X_GENERATE_FORMAT_FUNCTION(z,n,d)                                                  \
    template< BOOST_PP_ENUM_PARAMS(n,typename T)>                                          \
    std::string format( const std::string& strFmt, BOOST_PP_ENUM_BINARY_PARAMS(n, T, arg) )\
    try                                                                                    \
        {                                                                                  \
        using namespace boost::io;                                                         \
        boost::format f;                                                                   \
        f.exceptions( all_error_bits ^ ( too_many_args_bit | too_few_args_bit )  );        \
        f.parse(strFmt);                                                                   \
        f BOOST_PP_REPEAT(n,X_GENERATE_FORMAT,_);                                          \
        return f.str();                                                                    \
        }                                                                                  \
        catch(boost::io::format_error& ex)                                                 \
        {                                                                                  \
        std::string Err(strFmt);                                                           \
        Err.append(" ");                                                                   \
        Err.append(ex.what());                                                             \
        return Err;                                                                        \
        }

        BOOST_PP_REPEAT_FROM_TO( 1, 50, X_GENERATE_FORMAT_FUNCTION, _)

#undef X_GENERATE_FORMAT_FUNCTION
#undef X_GENERATE_FORMAT
    }
}

#define stringformat sc::string::format

//
//#define BOOST_FORMAT_CATCH(TEXT) \
//try \
//{ \
//    using namespace boost::io; \
//    boost::format fmter(strFmt); \
//    fmter.exceptions( all_error_bits ^ ( too_many_args_bit | too_few_args_bit )  ); \
//    TEXT; \
//    return fmter.str(); \
//} \
//catch(boost::io::format_error& ex) \
//{ \
//    std::string Err(strFmt); \
//    Err.append(" ");  \
//    Err.append(ex.what()); \
//    return Err; \
//}
//
//namespace sc {
//    namespace string {
//        /**
//        * \ingroup sc
//        * \version 1.0
//        * first version
//        * \date 2008-09-18
//        * \author Jgkim
//        * \history
//        * \par license
//        * Copyright (c) Min Communications. All rights reserved.
//        */
//
//        //! boost format 에 맞추어서 문자열을 만들어 낸다.
//        //! boost format 이 예외를 던지기 때문에 예외없는 버전이 필요해서 만듬	
//        boost::format formatter( const std::string& f_string );        
//
//        template<typename T1>
//        std::string format( const std::string& strFmt, T1 arg1 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1);
//        }
//
//        template<typename T1, typename T2>
//        std::string format( const std::string& strFmt, T1 arg1, T2 arg2 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2);
//        }
//
//        template<typename T1, typename T2, typename T3>
//        std::string format( const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3);
//        }
//
//        template<typename T1, typename T2, typename T3, typename T4>
//        std::string format( const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4);
//        }
//
//        template<typename T1, typename T2, typename T3, typename T4, typename T5>
//        std::string format( const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12 >
//            std::string format(
//            const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13 >
//            std::string format(
//            const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14 >
//            std::string format(
//            const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5,
//            typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15,
//            typename T16 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
//            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
//            T16 arg16 )
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29 >
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T28 arg29, T30 arg30)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T28 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39, typename T40>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39, T40 arg40)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39 % arg40);
//        }
//
//        template<
//            typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39, typename T40,
//            typename T41>
//            std::string format(
//            const std::string& strFmt,
//            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39, T40 arg40,
//            T41 arg41)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39 % arg40 % arg41);
//        }
//
//        template<
//            typename  T1, typename  T2, typename  T3, typename  T4, typename  T5, typename  T6, typename  T7, typename  T8, typename  T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39, typename T40,
//            typename T41, typename T42>
//            std::string format(
//            const std::string& strFmt,
//            T1   arg1, T2   arg2, T3   arg3, T4   arg4, T5   arg5, T6   arg6, T7   arg7, T8   arg8, T9   arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39, T40 arg40,
//            T41 arg41, T42 arg42)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39 % arg40 % arg41 % arg42);
//        }
//
//        template<
//            typename  T1, typename  T2, typename  T3, typename  T4, typename  T5, typename  T6, typename  T7, typename  T8, typename  T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39, typename T40,
//            typename T41, typename T42, typename T43>
//            std::string format(
//            const std::string& strFmt,
//            T1   arg1, T2   arg2, T3   arg3, T4   arg4, T5   arg5, T6   arg6, T7   arg7, T8   arg8, T9   arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39, T40 arg40,
//            T41 arg41, T42 arg42, T43 arg43)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39 % arg40 % arg41 % arg42 % arg43);
//        }
//
//        template<
//            typename  T1, typename  T2, typename  T3, typename  T4, typename  T5, typename  T6, typename  T7, typename  T8, typename  T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39, typename T40,
//            typename T41, typename T42, typename T43, typename T44>
//            std::string format(
//            const std::string& strFmt,
//            T1   arg1, T2   arg2, T3   arg3, T4   arg4, T5   arg5, T6   arg6, T7   arg7, T8   arg8, T9   arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39, T40 arg40,
//            T41 arg41, T42 arg42, T43 arg43, T44 arg44)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39 % arg40 % arg41 % arg42 % arg43 % arg44);
//        }
//
//        template<
//            typename  T1, typename  T2, typename  T3, typename  T4, typename  T5, typename  T6, typename  T7, typename  T8, typename  T9, typename T10,
//            typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18, typename T19, typename T20,
//            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30,
//            typename T31, typename T32, typename T33, typename T34, typename T35, typename T36, typename T37, typename T38, typename T39, typename T40,
//            typename T41, typename T42, typename T43, typename T44, typename T45>
//            std::string format(
//            const std::string& strFmt,
//            T1   arg1, T2   arg2,  T3  arg3,  T4  arg4, T5   arg5, T6   arg6, T7   arg7,  T8  arg8,  T9  arg9, T10 arg10,
//            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
//            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25, T26 arg26, T27 arg27, T28 arg28, T29 arg29, T30 arg30,
//            T31 arg31, T32 arg32, T33 arg33, T34 arg34, T35 arg35, T36 arg36, T37 arg37, T38 arg38, T39 arg39, T40 arg40,
//            T41 arg41, T42 arg42, T43 arg43, T44 arg44, T45 arg45)
//        {
//            BOOST_FORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26 % arg27 % arg28 % arg29 % arg30 % arg31 % arg32 % arg33 % arg34 % arg35 % arg36 % arg37 % arg38 % arg39 % arg40 % arg41 % arg42 % arg43 % arg44 % arg45);
//        }
//
//    } // namespace string
//} // namespace sc

#endif // _CORE_COMMON_STRING_FORMAT_H_