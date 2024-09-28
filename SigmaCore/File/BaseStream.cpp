#include "stdafx.h"
#include "./BaseStream.h"

#include "../DebugInclude.h"

namespace sc
{
    BaseStream::BaseStream()
        : m_IsFileOpened(FALSE)
        , m_OpenType(FOT_READ)
        , m_FileID(0)
        , m_DefaultOffSet(0)
        , m_dwBlockMarkPOS(0)
        , m_bencode(false)
        , m_nVersion(-1)
    {
        SecureZeroMemory(m_szFileName, sizeof(m_szFileName));
        SecureZeroMemory(m_szFileType, sizeof(m_szFileType));
        SecureZeroMemory(m_nMarkPos, sizeof(m_nMarkPos));
        StringCchCopyA(m_szFileType, FILETYPESIZE, "default");
    }
    
    BaseStream::~BaseStream()
    {
    }


} // namespace sc