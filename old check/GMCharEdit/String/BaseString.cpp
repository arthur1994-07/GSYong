#include "stdafx.h"
#include "../Encrypt/Rijndael.h"
#include "./BaseString.h"


#include "../DebugInclude.h"


namespace sc
{

CBaseString::CBaseString()
    : m_dwIndex(0)
    , m_dwLine(0)
    , m_bDecode(FALSE)
    , m_nVersion(-1)
{
    SecureZeroMemory(m_szFileName, sizeof(m_szFileName));
    m_pRijndael = new sc::CRijndael;
}

CBaseString::~CBaseString ()
{
    delete m_pRijndael;
    m_pRijndael = NULL;
}

} // namespace sc