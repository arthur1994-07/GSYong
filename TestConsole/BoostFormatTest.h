#pragma once

#ifndef SC_STRING_FORMAT_H_
#define SC_STRING_FORMAT_H_

#pragma once

#include <string>
#pragma warning(disable:4996) // disable deprecated warning 
#include <boost/format.hpp>
#pragma warning(default:4996)

#define CATCH_START(TEXT) \
try { \
    using namespace boost::io; \
    boost::format fmter(strFmt); \
    fmter.exceptions( all_error_bits ^ ( too_many_args_bit | too_few_args_bit )  ); \
    TEXT; \
    return fmter.str(); \
 } \
catch(boost::io::format_error& ex) \
{ \
    std::string Err(strFmt); \
    Err.append(" ");  \
    Err.append(ex.what()); \
    return Err; \
}

namespace sc {
    namespace stringtest {
        //! boost format 에 맞추어서 문자열을 만들어 낸다.
        //! boost format 이 예외를 던지기 때문에 예외없는 버전이 필요해서 만듬	
        boost::format formatter( const std::string& f_string );        

        template<typename T1>
        std::string format( const std::string& strFmt, T1 arg1 )
        {
            CATCH_START(fmter % arg1);
        }

        template<typename T1, typename T2>
        std::string format( const std::string& strFmt, T1 arg1, T2 arg2 )
        {
           CATCH_START(fmter % arg1 % arg2);
        }

        template<typename T1, typename T2, typename T3>
        std::string format( const std::string& strFmt, T1 arg1, T2 arg2, T3 arg3 )
        {
            CATCH_START(fmter % arg1 % arg2 % arg3);
        }
    } // namespace string
} // namespace sc

#endif // _CORE_COMMON_STRING_FORMAT_H_