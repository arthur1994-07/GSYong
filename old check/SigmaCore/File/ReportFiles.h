#ifndef _SC_REPORT_FILES_H_
#define  _SC_REPORT_FILES_H_

#pragma once

#include <string>
#include <vector>

namespace REPORTFILES
{
    void MakeFileList(const char* szPath, const std::vector<std::string>& BanFiles);
    void PathRecurse(const std::string strRootPath, const std::string strCurPath, const std::vector<std::string>& BanFiles);
}

#endif // _SC_REPORT_FILES_H_