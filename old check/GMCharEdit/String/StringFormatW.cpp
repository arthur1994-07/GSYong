#include "StdAfx.h"
#include "./StringFormatW.h"

#include "../DebugInclude.h"

namespace sc {
    namespace string {

        boost::wformat wformatter(const std::wstring& f_string)
        {
            using namespace boost::io;
            boost::wformat fmter(f_string);
            fmter.exceptions(all_error_bits ^ (too_many_args_bit | too_few_args_bit));
            return fmter;
        }

    } // namespace string
} // namespace sc