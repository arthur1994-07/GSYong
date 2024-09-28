#include "../pch.h"
#include <stdlib.h>
#include "./s_NetClient.h"
#include "../../RanLogic/Util/s_CClientConsoleMsg.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Util/china_md5.h"

#include "../../SigmaCore/DebugInclude.h"

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::SndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (!szPasswd)
		return NET_ERROR;

	NET_CHA_DEL ncd(nChaNum, szPasswd); // �н����� ����	
	m_Tea.encrypt(ncd.m_szPass2, USR_PASS_LENGTH+1); // �н����带 ��ȣȭ �Ѵ�.	
	// Agent ������ ����
	return Send((char*) &ncd);
}

//
//mjeon.indonesia
//
int CNetClient::IdnSndDelCharacter(int nChaNum, std::string str2ndPass)
{
	if (str2ndPass.empty())
		return NET_ERROR;


	std::string strPassEncrypted;
	strPassEncrypted = sc::string::getMd5A(str2ndPass);

	IDN_NET_CHA_DEL ncd(nChaNum);

	StringCchCopy(ncd.sz2ndPass, sizeof(ncd.sz2ndPass), strPassEncrypted.c_str());

	// Agent ������ ����
	return Send( (char *) &ncd );

	/*
	IDN_NET_CHA_DEL ncd;	
	ncd.nChaNum	= nChaNum;
	
	// Agent ������ ����
	return Send((char *)&ncd );
	*/
}


/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::DaumSndDelCharacter(int nChaNum, const TCHAR* szPasswd)
{
	if (!szPasswd)
		return NET_ERROR;

	DAUM_NET_CHA_DEL ncd(nChaNum, szPasswd);
	m_Tea.encrypt(ncd.m_szPass2, USR_PASS_LENGTH+1); // �н����� ��ȣȭ	

	return Send((char *) &ncd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::TerraSndDelCharacter(int nChaNum)
{
	TERRA_NET_CHA_DEL ncd;	
	ncd.nChaNum	= nChaNum;

	return Send((char *) &ncd);
}

/**
* ĳ���� ���� ��û GSP
* \param nChaNum ĳ���͹�ȣ
* \return 
*/
int CNetClient::GspSndDelCharacter( int nChaNum )
{
	GSP_NET_CHA_DEL ncd;	
	ncd.nChaNum		= nChaNum;

	return Send((char *) &ncd);
}


/**
* ĳ���� ���� ��û
* \param nChaNum ������ ĳ���͹�ȣ
* \param strPasswd ��й�ȣ
* \return 
*/
int	CNetClient::ChinaSndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (szPasswd == NULL) return NET_ERROR;

	CHINA_MD5::MD5 md5; // md5 �ؽ��� Ŭ����
	// ��й�ȣ�� ���ڿ� ���̸� ���Ѵ�.
	size_t nLength = 0;
	HRESULT sRet = StringCchLength( szPasswd, USR_PASS_LENGTH, &nLength );

	if (sRet == STRSAFE_E_INVALID_PARAMETER)
		return NET_ERROR;

	// �ؽ����� ��� ���ؼ� �н����� ���ڿ��� �ִ´�.
	// �ؽ����� ��� ���� ������Ʈ �� �ش�.
	md5.update( reinterpret_cast<const unsigned char*> (szPasswd), nLength );
	// ����� �ؽ����� strPass �� �ִ´�.
	std::string strPass = md5.asString();
	
	NET_CHA_DEL ncd(nChaNum, strPass.c_str());
	m_Tea.encrypt(ncd.m_szPass2, USR_PASS_LENGTH); // �н����带 ��ȣȭ �Ѵ�.
	// Agent ������ ����
	return Send((char*) &ncd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::ExciteSndDelCharacter(int nChaNum)
{
	EXCITE_NET_CHA_DEL encd;
	//encd.nmg.nType	= EXCITE_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	encd.nChaNum	= nChaNum;
	return Send((char *) &encd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::JapanSndDelCharacter(int nChaNum)
{
	JAPAN_NET_CHA_DEL encd;
	//encd.nmg.nType	= JAPAN_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	encd.nChaNum	= nChaNum;
	return Send((char *) &encd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::GsSndDelCharacter(int nChaNum, const TCHAR* szPasswd)
{
	if (!szPasswd)
		return NET_ERROR;

	GS_NET_CHA_DEL ncd(nChaNum, szPasswd);
	m_Tea.encrypt(ncd.m_szPass2, GS_USER_PASS+1); // �н����� ��ȣȭ	

	return Send((char *) &ncd);
}

int	CNetClient::GmSndDelCharacter( int nChaNum )
{
	GS_NET_CHA_DEL ncd( nChaNum, NULL );
	return Send( ( char* )&ncd );
}

int	CNetClient::UsSndDelCharacter( int nChaNum )
{
	GS_NET_CHA_DEL ncd( nChaNum, NULL );
	return Send( ( char* )&ncd );
}
