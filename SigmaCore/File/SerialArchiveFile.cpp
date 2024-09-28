#include "stdafx.h"
#include "SerialArchiveFile.h"
#include "FileFind.h"
#include "IsValidFileName.h"

#include "../String/StringUtil.h"

#include "../DebugInclude.h"

namespace sc
{

SerialArchiveFile::SerialArchiveFile(boost::function<void (const TCHAR*)> pArchive, const TCHAR* pFile)  :
    SerialFile()
,   m_pArchive(pArchive)
,   m_streaming(0)
{
    if (pFile)
    {
        OpenFile(FOT_READ, pFile);
    }
}

SerialArchiveFile::~SerialArchiveFile()
{
    
}

size_t SerialArchiveFile::read( void *pbuffer, DWORD dwSize )
{
    size_t rdcount = SerialFile::read(pbuffer, dwSize);

    if (pbuffer)
    {
        Tokenizer(static_cast<const TCHAR*>(pbuffer), static_cast<int>(dwSize));
    }

    return rdcount;
}

size_t SerialArchiveFile::write( const void* pbuffer, DWORD dwSize )
{
    if (pbuffer)
    {
        Tokenizer(static_cast<const TCHAR*>(pbuffer), static_cast<int>(dwSize));
    }

    m_streaming += dwSize;

    return dwSize;
}

void SerialArchiveFile::Tokenizer( const TCHAR* pStream, int nSize )
{
    while (nSize-- > 0)
    {
        MIN_ASSERT(nSize >= 0);

        if (IsValidFileName(*pStream++))
        {
            std::string strTemp;
            strTemp += *(pStream - 1);

            while(nSize-- > 0 && IsValidFileName(*pStream++))
            {
                MIN_ASSERT(nSize >= 0);
                strTemp += *(pStream - 1);
            }

            if (strTemp.empty() == false && IsValidFileName(strTemp.c_str()) == 0)
            {
                OnArchive(strTemp.c_str());
            }
        }
    }
}

void SerialArchiveFile::OnArchive( const TCHAR* pArchiveFile )
{
    std::string strTemp(pArchiveFile);
    boost::to_lower(strTemp);
    m_pArchive(strTemp.c_str());
}

BOOL SerialArchiveFile::OpenFile( const FOPENTYPE _Type, const char* _szFileName, bool bencode/*=false*/ )
{
    if (_szFileName && _tcsclen(_szFileName))
    {
        CString strTemp;
        sc::string::getFullFileName(_szFileName, strTemp);
        OnArchive(strTemp.GetBuffer());
    }

    if (_Type == FOT_READ)
    {
        return SerialFile::OpenFile(_Type, _szFileName, bencode);
    }

    m_OpenType = _Type;

    return TRUE;
}

BOOL SerialArchiveFile::BeginBlock()
{
    ++m_dwBlockMarkPOS;
    return TRUE;
}

BOOL SerialArchiveFile::EndBlock()
{
    --m_dwBlockMarkPOS;
    return TRUE;
}

} // namespace sc