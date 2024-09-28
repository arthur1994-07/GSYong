#ifndef SC_STRING_FORMATW_H_
#define SC_STRING_FORMATW_H_

#pragma once

#include <iostream>
#include <string>
#pragma warning(disable:4996) // disable deprecated warning 
#include <boost/format.hpp>
#pragma warning(default:4996)

#define BOOST_WFORMAT_CATCH(TEXT) \
try \
{ \
    using namespace boost::io; \
    boost::wformat fmter(strFmt); \
    fmter.exceptions( all_error_bits ^ ( too_many_args_bit | too_few_args_bit )  ); \
    TEXT; \
    return fmter.str(); \
} \
catch(boost::io::format_error& ex) \
{ \
    std::wstring Err(strFmt); \
    std::cerr << ex.what(); \
    return Err; \
}

namespace sc {
    namespace string {
        /**
        * \ingroup sc
        * \version 1.0
        * first version
        * \date 2008-09-18
        * \author Jgkim
        * \par license
        * Copyright (c) Min Communications. All rights reserved.
        */

        //! boost format 에 맞추어서 문자열을 만들어 낸다.
        //! boost format 이 예외를 던지기 때문에 예외없는 버전이 필요해서 만듬	
        boost::wformat wformatter( const std::wstring& f_string );        

        template<typename T1>
        std::wstring wformat( const std::wstring& strFmt, T1 arg1 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1);
        }

        template<typename T1, typename T2>
        std::wstring wformat( const std::wstring& strFmt, T1 arg1, T2 arg2 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2);
        }

        template<typename T1, typename T2, typename T3>
        std::wstring wformat( const std::wstring& strFmt, T1 arg1, T2 arg2, T3 arg3 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3);
        }

        template<typename T1, typename T2, typename T3, typename T4>
        std::wstring wformat( const std::wstring& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4);
        }

        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        std::wstring wformat( const std::wstring& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12 >
            std::wstring wformat(
            const std::wstring& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13 >
            std::wstring wformat(
            const std::wstring& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14 >
            std::wstring wformat(
            const std::wstring& strFmt, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20,
            typename T21 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
            T21 arg21 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20,
            typename T21, typename T22 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
            T21 arg21, T22 arg22 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20,
            typename T21, typename T22, typename T23 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
            T21 arg21, T22 arg22, T23 arg23 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20,
            typename T21, typename T22, typename T23, typename T24 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
            T21 arg21, T22 arg22, T23 arg23, T24 arg24 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20,
            typename T21, typename T22, typename T23, typename T24, typename T25 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25);
        }

        template<
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8, typename T9, typename T10,
            typename T11, typename T12, typename T13, typename T14, typename T15,
            typename T16, typename T17, typename T18, typename T19, typename T20,
            typename T21, typename T22, typename T23, typename T24, typename T25, typename T26 >
            std::wstring wformat(
            const std::wstring& strFmt,
            T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5,
            T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10,
            T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15,
            T16 arg16, T17 arg17, T18 arg18, T19 arg19, T20 arg20,
            T21 arg21, T22 arg22, T23 arg23, T24 arg24, T25 arg25,
            T26 arg26 )
        {
            BOOST_WFORMAT_CATCH(fmter % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7 % arg8 % arg9 % arg10 % arg11 % arg12 % arg13 % arg14 % arg15 % arg16 % arg17 % arg18 % arg19 % arg20 % arg21 % arg22 % arg23 % arg24 % arg25 % arg26);
        }

    } // namespace string
} // namespace sc

#endif // _CORE_COMMON_STRING_FORMATW_H_