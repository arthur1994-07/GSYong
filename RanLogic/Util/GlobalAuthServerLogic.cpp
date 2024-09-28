#include "pch.h"
#include "GlobalAuthManager.h"

namespace sc
{
// namespace sc START
CGlobalAuthServerLogic::CGlobalAuthServerLogic(void)
{
}

CGlobalAuthServerLogic::~CGlobalAuthServerLogic(void)
{
}

BOOL CGlobalAuthServerLogic::CertificateAuthData(std::string strEncryptedAuthData)
{
	m_authData.strEncryptedAuthData = strEncryptedAuthData;

	DecryptAuthData();

	ParsingAuthData( m_authData.strDecryptedAuthData );

	return TRUE;
}

std::string CGlobalAuthServerLogic::GetCertificatedAuthData()
{
	m_authData.strDecryptedAuthData = CombineCertificationData();

	EncryptAuthData();

	return m_authData.strEncryptedAuthData;
}
// namesapce sc END
}