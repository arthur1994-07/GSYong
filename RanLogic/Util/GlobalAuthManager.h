#pragma once
#include "../../SigmaCore/Encrypt/RC5Encrypt.h"

//using namespace sc; // sc를 참조하는 다른 부분과 엮여있어서 모두 수정해야 하므로 인증모듈을 sc에 포함시킴
namespace sc
{
// namespace sc START
struct SAuthData
{
	std::string strEncryptedAuthData;
	std::string strDecryptedAuthData;
	int nServiceProvider;
	int nServerType;
	std::string strIP;
	int nPort;
	std::string strUniqKey;
	std::string strNewUniqKey;
	int nCertification;

	SAuthData()
	{
		strEncryptedAuthData.empty();
		strDecryptedAuthData.empty();
		nServiceProvider = 0;
		nServerType = 0;
		strIP.empty();
		nPort = 0;
		strUniqKey.empty();
		strNewUniqKey.empty();
		nCertification = 0;
	}

	SAuthData& operator=(const SAuthData& rhs)
	{
		strEncryptedAuthData = rhs.strEncryptedAuthData;
		strDecryptedAuthData = rhs.strDecryptedAuthData;
		nServiceProvider = rhs.nServiceProvider;
		nServerType = rhs.nServerType;
		nPort = rhs.nPort;
		strUniqKey = rhs.strUniqKey;
		strNewUniqKey = rhs.strNewUniqKey;
		nCertification = rhs.nCertification;

		return *this;
	}

	//SAuthData& operator=(const SAuthData* rhs)
	//{
	//	strEncryptedAuthData = (*rhs).strEncryptedAuthData;
	//	strDecryptedAuthData = (*rhs).strDecryptedAuthData;
	//	nServiceProvider = (*rhs).nServiceProvider;
	//	nServerType = (*rhs).nServerType;
	//	nPort = (*rhs).nPort;
	//	strUniqKey = (*rhs).strUniqKey;
	//	strNewUniqKey = (*rhs).strNewUniqKey;
	//	nCertification = (*rhs).nCertification;

	//	return *this;
	//}
};

class CGlobalAuthManager : public RC5EncryptA
{
public:
	CGlobalAuthManager(void);
	~CGlobalAuthManager(void);

protected:
	SAuthData m_authData;
	std::string m_FileName;
	std::string m_InternalFileName;

public:
	BOOL IsExistUniqKey();
	BOOL CreateDefaultUniqKey();
	BOOL ReadUniqKey();
	std::string CombineAuthData();
	std::string CombineCertificationData();
	std::string DecryptAuthData(std::string strEncryptedAuthData);
	std::string EncryptAuthData(std::string strDecryptedAuthData);
	BOOL DecryptAuthData();
	BOOL EncryptAuthData();
	BOOL WriteUniqKey();
	BOOL ParsingAuthData(std::string authData);
	BOOL ParsingRecvAuthData(std::string authData);
	BOOL ParsingRecvAuthData();
	SAuthData* GetAuthData() { return &m_authData; }
	
	BOOL IsExistInternalKey();
	BOOL CreateInternalKey();
	BOOL CertificationInternalUniqKey();

	void SetEncryptedAuthData(std::string strDecryptedData) { m_authData.strEncryptedAuthData = strDecryptedData; }
	void SetServiceProvider( int nServiceProvider ) { m_authData.nServiceProvider = nServiceProvider; }
	void SetServerType( int nServerType ) { m_authData.nServerType = nServerType; }
	void SetServerIP( std::string strIP ) { m_authData.strIP = strIP; }
	void SetServerPort( int nPort ) { m_authData.nPort = nPort; }
	void SetCertification( int nCertification ) { m_authData.nCertification = nCertification; }
	void SetUniqKey( std::string strKey ) { m_authData.strUniqKey = strKey; }
	void SetNewUniqKey( std::string strKey ) { m_authData.strNewUniqKey = strKey; }
};

class CGlobalAuthClientLogic : public CGlobalAuthManager
{
public:
	CGlobalAuthClientLogic(void);
	~CGlobalAuthClientLogic(void);

public:
	std::string GetEncryptedAuthData();
	BOOL ConfirmAuthData(std::string strRecvEncryptedAuthData);
	BOOL CertificationInternalAuth();
};

class CGlobalAuthServerLogic : public CGlobalAuthManager
{
public:
	CGlobalAuthServerLogic(void);
	~CGlobalAuthServerLogic(void);

public:
	BOOL CertificateAuthData(std::string strEncryptedAuthData);
	std::string GetCertificatedAuthData();
};
// namespace sc END
}