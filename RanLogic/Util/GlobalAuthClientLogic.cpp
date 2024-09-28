#include "pch.h"
#include "GlobalAuthManager.h"


namespace sc
{
// namespace sc START
CGlobalAuthClientLogic::CGlobalAuthClientLogic(void)
{
}

CGlobalAuthClientLogic::~CGlobalAuthClientLogic(void)
{
}

std::string CGlobalAuthClientLogic::GetEncryptedAuthData()
{
	ReadUniqKey();

	m_authData.strDecryptedAuthData = CombineAuthData();

	EncryptAuthData();

	return m_authData.strEncryptedAuthData;
}

BOOL CGlobalAuthClientLogic::ConfirmAuthData(std::string strRecvEncryptedAuthData)
{
	m_authData.strEncryptedAuthData = strRecvEncryptedAuthData;

	DecryptAuthData();

	ParsingRecvAuthData(m_authData.strDecryptedAuthData);

	WriteUniqKey();

	return TRUE;
}

BOOL CGlobalAuthClientLogic::CertificationInternalAuth()
{
	return CertificationInternalUniqKey();
}

// namespace sc END
}