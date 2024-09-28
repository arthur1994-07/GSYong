#include "../pch.h"
#include <stdlib.h>
#include "./s_NetClient.h"
#include "../../RanLogic/Util/s_CClientConsoleMsg.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Util/china_md5.h"

#include "../../SigmaCore/DebugInclude.h"

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::SndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (!szPasswd)
		return NET_ERROR;

	NET_CHA_DEL ncd(nChaNum, szPasswd); // 패스워드 복사	
	m_Tea.encrypt(ncd.m_szPass2, USR_PASS_LENGTH+1); // 패스워드를 암호화 한다.	
	// Agent 서버로 전송
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

	// Agent 서버로 전송
	return Send( (char *) &ncd );

	/*
	IDN_NET_CHA_DEL ncd;	
	ncd.nChaNum	= nChaNum;
	
	// Agent 서버로 전송
	return Send((char *)&ncd );
	*/
}


/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::DaumSndDelCharacter(int nChaNum, const TCHAR* szPasswd)
{
	if (!szPasswd)
		return NET_ERROR;

	DAUM_NET_CHA_DEL ncd(nChaNum, szPasswd);
	m_Tea.encrypt(ncd.m_szPass2, USR_PASS_LENGTH+1); // 패스워드 암호화	

	return Send((char *) &ncd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::TerraSndDelCharacter(int nChaNum)
{
	TERRA_NET_CHA_DEL ncd;	
	ncd.nChaNum	= nChaNum;

	return Send((char *) &ncd);
}

/**
* 캐릭터 삭제 요청 GSP
* \param nChaNum 캐릭터번호
* \return 
*/
int CNetClient::GspSndDelCharacter( int nChaNum )
{
	GSP_NET_CHA_DEL ncd;	
	ncd.nChaNum		= nChaNum;

	return Send((char *) &ncd);
}


/**
* 캐릭터 삭제 요청
* \param nChaNum 삭제할 캐릭터번호
* \param strPasswd 비밀번호
* \return 
*/
int	CNetClient::ChinaSndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (szPasswd == NULL) return NET_ERROR;

	CHINA_MD5::MD5 md5; // md5 해쉬용 클래스
	// 비밀번호의 문자열 길이를 구한다.
	size_t nLength = 0;
	HRESULT sRet = StringCchLength( szPasswd, USR_PASS_LENGTH, &nLength );

	if (sRet == STRSAFE_E_INVALID_PARAMETER)
		return NET_ERROR;

	// 해쉬값을 얻기 위해서 패스워드 문자열을 넣는다.
	// 해쉬값을 얻기 위해 업데이트 해 준다.
	md5.update( reinterpret_cast<const unsigned char*> (szPasswd), nLength );
	// 얻어진 해쉬값을 strPass 에 넣는다.
	std::string strPass = md5.asString();
	
	NET_CHA_DEL ncd(nChaNum, strPass.c_str());
	m_Tea.encrypt(ncd.m_szPass2, USR_PASS_LENGTH); // 패스워드를 암호화 한다.
	// Agent 서버로 전송
	return Send((char*) &ncd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::ExciteSndDelCharacter(int nChaNum)
{
	EXCITE_NET_CHA_DEL encd;
	//encd.nmg.nType	= EXCITE_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	encd.nChaNum	= nChaNum;
	return Send((char *) &encd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::JapanSndDelCharacter(int nChaNum)
{
	JAPAN_NET_CHA_DEL encd;
	//encd.nmg.nType	= JAPAN_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	encd.nChaNum	= nChaNum;
	return Send((char *) &encd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::GsSndDelCharacter(int nChaNum, const TCHAR* szPasswd)
{
	if (!szPasswd)
		return NET_ERROR;

	GS_NET_CHA_DEL ncd(nChaNum, szPasswd);
	m_Tea.encrypt(ncd.m_szPass2, GS_USER_PASS+1); // 패스워드 암호화	

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
