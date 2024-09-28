#ifndef _SC_AUTO_TXT_FILE_H_
#define _SC_AUTO_TXT_FILE_H_

#pragma once

#include "./TxtFile.h"

namespace sc
{

class AutoTxtFile
{
public:
    AutoTxtFile(
        const std::string& FileName,
        DWORD AutoNewFileSize,
        TxtFile::ENCODING Encoding=TxtFile::UTF8);
    ~AutoTxtFile();

protected:
    TxtFile* m_pTxtFile;
    std::string m_FileName;
    DWORD m_WriteCount;
    DWORD m_AutoNewFileSize;
    TxtFile::ENCODING m_Encoding;

public:
    std::string GetFileName() const { return m_FileName; }
    DWORD GetWriteCount() const { return m_WriteCount; }
    DWORD GetAutoNewFileSize() const { return m_AutoNewFileSize; }
    void write(const std::string& Msg);
    void write(const std::wstring& Msg);
    void ForceRenew();
};

} // namespace sc

#endif // _SC_AUTO_TXT_FILE_H_