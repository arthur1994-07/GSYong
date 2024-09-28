#include "stdafx.h"
#include "./AutoTxtFile.h"

#include "../DebugInclude.h"

namespace sc
{

AutoTxtFile::AutoTxtFile(
    const std::string& FileName,
    DWORD AutoNewFileSize,
    TxtFile::ENCODING Encoding)
    : m_WriteCount(0)        
    , m_AutoNewFileSize(AutoNewFileSize)
    , m_FileName(FileName)
    , m_Encoding(Encoding)
{   
    m_pTxtFile = new TxtFile(m_FileName, true, m_Encoding);
}

AutoTxtFile::~AutoTxtFile()
{
    if (m_pTxtFile)
        delete m_pTxtFile;
}        

void AutoTxtFile::write(const std::string& Msg)
{
    if (m_pTxtFile)
    {
        m_pTxtFile->write(Msg, true);
        ++m_WriteCount;
        if (m_WriteCount >= m_AutoNewFileSize)
        {
            delete m_pTxtFile;
            m_pTxtFile = new TxtFile(m_FileName, true, m_Encoding);
            m_WriteCount = 0;
        }
    }
}

void AutoTxtFile::write(const std::wstring& Msg)
{
    if (m_pTxtFile)
    {
        m_pTxtFile->write(Msg, true);
        ++m_WriteCount;
        if (m_WriteCount >= m_AutoNewFileSize)
        {
            delete m_pTxtFile;
            m_pTxtFile = new TxtFile(m_FileName, true, m_Encoding);
            m_WriteCount = 0;
        }
    }
}

void AutoTxtFile::ForceRenew()
{
    if (m_pTxtFile)
    {
        if(m_WriteCount > 0)
        {
            delete m_pTxtFile;
            m_pTxtFile = new TxtFile(m_FileName, true, m_Encoding);
            m_WriteCount = 0;
        }
    }
}

} // namespace sc