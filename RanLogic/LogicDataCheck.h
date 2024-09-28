#ifndef _LOGIC_DATA_CHECK_H_
#define _LOGIC_DATA_CHECK_H_

#pragma once

#include <string>
#include <boost/function.hpp>

namespace LOGIC_DATA_CHECK
{
    void NtkCheck(const std::string& NtkFile,
        int& ErrorCount,
        boost::function<void (std::string&)> ErrorLogFunc=NULL);

    void ErrorLog(std::string& ErrMsg,
        boost::function<void (std::string&)> ErrorLogFunc=NULL);
}

#endif // _LOGIC_DATA_CHECK_H_