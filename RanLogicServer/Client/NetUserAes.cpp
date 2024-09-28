#include "pch.h"
#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/Encrypt/Aes.h"
#include "./NetUser.h"


#include "../../SigmaCore/DebugInclude.h"

/*
//! server �� public key �� �����´�.
//! �� key �� client �� �����ؾ� �Ѵ�.
//! \param pPubKey key �� ���� ���� 32 byte �̻�
//! \param nKeyLength ����Ű�� ũ�⸦ �����ش�.
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

//! \param pBuffer ��ȣȭ�� ����
//! \nLength ��ȣȭ�� ������ ����.
bool CNetUser::EncryptBuffer(BYTE* pBuffer, int nLength)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_AesLock);
    if (pBuffer && m_pAes)
        return m_pAes->encrypt(pBuffer, nLength);
    else
        return false;
}

//! \param pBuffer ��ȣȭ�� ����
//! \nLength ��ȣȭ�� ������ ����.
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