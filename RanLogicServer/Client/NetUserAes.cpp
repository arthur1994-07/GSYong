#include "pch.h"
#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Encrypt/Aes.h"
#include "./NetUser.h"


#include "../../SigmaCore/DebugInclude.h"

/*
//! server 의 public key 를 가져온다.
//! 이 key 는 client 에 전송해야 한다.
//! \param pPubKey key 를 받을 버퍼 32 byte 이상
//! \param nKeyLength 공개키의 크기를 돌려준다.
void CNetUser::GetServerPubKey(BYTE* pPubKey, int& nKeyLength OUT)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_AesLock);
    if (pPubKey && m_pAes && IsOnline())
        m_pAes->getServerPubKey(pPubKey, nKeyLength);
}

void CNetUser::SetServerPubKey(BYTE* pPubKey)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_AesLock);
    SAFE_DELETE(m_pAes);
    if (pPubKey)
        m_pAes = new sc::Aes(pPubKey);
}

//! Client->Server Client PubKey
bool CNetUser::SetClientPubKey(BYTE* pPubKey, int nKeyLength)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_AesLock);
    if (pPubKey && m_pAes && IsOnline())
        return m_pAes->setClientPubKey(pPubKey);
    else
        return false;
}

void CNetUser::GetClientPubKey(BYTE* pPubKey, int& nKeyLength OUT)
{
    if (pPubKey && m_pAes)
        m_pAes->getClientPubKey(pPubKey, nKeyLength);
}

//! \param pBuffer 암호화할 버퍼
//! \nLength 암호화할 버퍼의 길이.
bool CNetUser::EncryptBuffer(BYTE* pBuffer, int nLength)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_AesLock);
    if (pBuffer && m_pAes)
        return m_pAes->encrypt(pBuffer, nLength);
    else
        return false;
}

//! \param pBuffer 복호화할 버퍼
//! \nLength 복호화할 버퍼의 길이.
bool CNetUser::DecryptBuffer(BYTE* pBuffer, int nLength)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_AesLock);
    if (pBuffer && m_pAes)
        return m_pAes->decrypt(pBuffer, nLength);
    else
        return false;
}

// ----------------------------------------------------------------------------

#ifdef _DEBUG
CryptoPP::SecByteBlock* CNetUser::AES_getSecretKey()
{
    if (m_pAes)
        return m_pAes->getSecretKey();
    else
        return NULL;
}

CryptoPP::DH* CNetUser::AES_getDH()
{
    if (m_pAes)
        return m_pAes->getDH();
    else
        return NULL;
}

byte* CNetUser::AES_getIV()
{
    if (m_pAes)
        return m_pAes->getIV();
    else
        return NULL;
}
// ----------------------------------------------------------------------------
#endif
*/