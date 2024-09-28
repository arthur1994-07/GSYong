#include "stdafx.h"
#include "./RC5Encrypt.h"
#include "../String/StringUtil.h"
#include "./RandomRC5Encrypt.h"

#include "../DebugInclude.h"

namespace sc
{

RandomRC5::RandomRC5(const std::string& Key, size_t KeyLength)
    : m_pRc5(NULL)
{
    if (Key.empty())
    {
        CString RandomStr;
        sc::string::generateRandomString(RandomStr, static_cast<int> (KeyLength), 1, 1, 1);
        m_Key = RandomStr.GetString();
    }
    else
    {
        m_Key = Key;
    }

    m_pRc5 = new RC5EncryptA(m_Key);
}

RandomRC5::~RandomRC5()
{
    delete m_pRc5;
}

std::string RandomRC5::GetKey() const
{
    return m_Key;
}

std::string RandomRC5::GetEncrypt(const std::string& strSrc)
{
    return m_pRc5->getEncrypt(strSrc);
}

std::string RandomRC5::GetDecrypt(const std::string& strSrc)
{
    return m_pRc5->getDecrypt(strSrc);
}

} // namespace sc