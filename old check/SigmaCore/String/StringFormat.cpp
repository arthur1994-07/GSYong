#include "StdAfx.h"
#include "./StringFormat.h"

#include "../DebugInclude.h"

namespace sc {
    namespace string {

        boost::format formatter(const std::string& f_string)
        {
            using namespace boost::io;
            boost::format fmter(f_string);
            fmter.exceptions(all_error_bits ^ (too_many_args_bit | too_few_args_bit));
            return fmter;
        }

        // boost::wformat formatter( const wchar_t* szString )
        // {
        // 	using namespace boost::io;
        // 	boost::wformat fmter(szString);
        // 	fmter.exceptions( all_error_bits ^ ( too_many_args_bit | too_few_args_bit )  );
        // 	return fmter;
        // }

    } // namespace sc
} // namespace string