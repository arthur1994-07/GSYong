#ifndef _SC_TXT_FILE_H_
#define _SC_TXT_FILE_H_

#pragma once

#include <string>

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(P) { if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }
#endif

namespace sc
{

class TxtFile
{
public:
    enum ENCODING
    {        
        ANSI, /// Ansi code page.        
        UTF8, /// UTF-8 encoding.        
        UTF16LE, /// UTF-16/UCS-2, little-endian encoding.        
        UTF16BE, /// UTF-16/UCS-2, big-endian encoding.        
    };

public:
    TxtFile(const std::string& FileName, bool AutoRename, ENCODING Encoding=UTF8);
    ~TxtFile();

protected:
    HANDLE m_hFile;
    std::string m_strFullFileName;

public:
    std::string FileName() const { return m_strFullFileName; }
    void write(const std::string& Msg, bool NewLine=true);
    void write(const std::wstring& Msg, bool NewLine=true);
};

} // namespace sc

#endif // _SC_TXT_FILE_H_