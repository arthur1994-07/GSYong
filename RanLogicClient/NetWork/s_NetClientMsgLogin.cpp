#include "../pch.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Encrypt/RandomRC5Encrypt.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../../RanLogic/VtcGuard.h"
#include "../../RanLogic/GameVersion.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Util/china_md5.h"
#include "../../RanLogic/Util/s_CClientConsoleMsg.h"

#include "../Country/GLVictoriousCountryManClient.h"

#include "./s_NetClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//
//Taiwan, Hongkong, 내부
//
int CNetClient::SndLogin(const char* szUserID, const char* szUserPassword, const char* szRandomPassword, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
        nChannel = 0;

    int LauncherVersion = 0;
    int GameVersion = 0;
    gv::Version::GetVersion(LauncherVersion, GameVersion);

	// Initialize Victorious Country;
	Country::GLVictoriousCountryManClient* pVCountryMan =
		Country::GLVictoriousCountryManClient::Instance();
	if ( pVCountryMan )
		pVCountryMan->Initialize();

	NET_LOGIN_DATA nld(	nChannel,
						szRandomPassword,
						szUserPassword,
						szUserID,
						m_szEncryptKey,
						LauncherVersion,
						GameVersion);

	/// 암호방식 Tea 방식으로 변경됨. 
	/// 2005-11-16 Jgkim
	
	/*
	DWORD dwSize = nld.nmg.dwSize - sizeof(NET_MSG_GENERIC);

	char* pBuffer = (char*) &nld;
	m_Bit.buf_encode(pBuffer+sizeof(NET_MSG_GENERIC),
					 dwSize,
					 m_ck.nKeyDirection,
					 m_ck.nKey);
	*/	

	m_Tea.encrypt(nld.m_szUserid,         USR_ID_LENGTH+1);
	m_Tea.encrypt(nld.m_szPassword,       USR_PASS_LENGTH+1);
	m_Tea.encrypt(nld.m_szRandomPassword, USR_RAND_PASS_LENGTH+1);
	m_Tea.encrypt(nld.m_szEnCrypt,        ENCRYPT_KEY+1);
	
	return Send((char*) &nld);
}


//
//mjeon.indonesia
//
int CNetClient::IdnSndLogin(const char* userid, const char* userpass, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
        nChannel = 0;

	GASSERT(userid);
	GASSERT(userpass);
    /*
    char* IdUserId = (char*) userid;
    VtcGuard::GameLogIn(IdUserId);
    */

	std::string strPassEncrypted;
	strPassEncrypted = sc::string::getMd5A(userpass); //sc::string::getSHA256(std::string(userpass));	
	IDN_NET_LOGIN_DATA nld(nChannel, strPassEncrypted.c_str(), userid);
	return Send((char*)&nld);
}

int CNetClient::WorldBattleLogin(const char* szUserId, const char* szPassword, int Channel)
{
    if (Channel < 0 || Channel >= MAX_CHANNEL_NUMBER)
        Channel = 0;

	GASSERT(szUserId);
	GASSERT(szPassword);
    
	std::string strPassSha256;
	strPassSha256 = sc::string::getSha256(std::string(szPassword));

    // RC5 암호화
    sc::RandomRC5 EncRc5(std::string(""));

    std::string Key = EncRc5.GetKey();
    std::string EncUserId = EncRc5.GetEncrypt(std::string(szUserId));
    std::string EncPassword = EncRc5.GetEncrypt(std::string(strPassSha256));

    // Json message 구성
    NET_LOGIN_WB_CA SendData(Channel, Key, EncPassword, EncUserId);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    NET_MSG_PACK MsgCA;
    if (MsgCA.SetData(NET_MSG_LOGIN_WB_CA, SendBuffer, true, true))
    {
        return Send((char*) &MsgCA);
    }
    else
    {
        sc::ErrorMessageBox(std::string("WorldBattleLogin buffer overflow"), false);
        return NET_ERROR;
    }
}

//
//Daum Game(한국)
//
int CNetClient::DaumSndLogin(const TCHAR* szUUID, int nChannel)
{
	if (szUUID == NULL)
		return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	DAUM_NET_LOGIN_DATA dnld;

	dnld.nChannel = nChannel;
    ::StringCchCopy( dnld.szUUID, sc::string::UUID_LENGTH, szUUID );
	//::StringCchCopy(dnld.szParameter, DAUM_MAX_PARAM_LENGTH+1, szParameter);

	return Send((char *) &dnld);
}

int CNetClient::DaumSndPassCheck(const TCHAR* szDaumGID, const TCHAR* szUserPassword, int nCheckFlag)
{
	if( szDaumGID == NULL || szUserPassword == NULL )
		return NET_ERROR;

	DAUM_NET_PASSCHECK_DATA nld;

	::StringCchCopy( nld.szDaumGID, DAUM_MAX_GID_LENGTH+1, szDaumGID );
	::StringCchCopy( nld.szUserPass, DAUM_USERPASS+1, szUserPassword );
	nld.nCheckFlag = nCheckFlag;

	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}


//
//GS
//
int CNetClient::GsSndWBLogin(const char* szUserID, const char* szUserPassword, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	GS_NET_WB_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;
	::StringCchCopy(nld.szUserid,	GS_USER_ID+1,	szUserID);

	return Send((char *) &nld);
}

int CNetClient::GsSndLogin(const char* szUserID, const char* szUserPassword, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	// Initialize Victorious Country;
	Country::GLVictoriousCountryManClient* pVCountryMan =
		Country::GLVictoriousCountryManClient::Instance();
	if ( pVCountryMan )
		pVCountryMan->Initialize();

	GS_NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;
	
	::StringCchCopy(nld.szUserid,	GS_USER_ID+1,	szUserID);
	::StringCchCopy(nld.szPassword,	GS_USER_PASS+1,	szUserPassword);
	::StringCchCopy(nld.szEnCrypt,	ENCRYPT_KEY+1,	m_szEncryptKey);

	m_Tea.encrypt (nld.szUserid, GS_USER_ID+1);
	m_Tea.encrypt (nld.szPassword, GS_USER_PASS+1);
	m_Tea.encrypt (nld.szEnCrypt, ENCRYPT_KEY+1);

	return Send((char *) &nld);
}

//
//EU (Games-Masters)
//
int CNetClient::GmSndLogin( const char* userid, const char* userpass, int nChannel )
{
	if ( nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER )
		nChannel = 0;

	GM_NET_LOGIN_DATA nld;

	nld.nChannel = nChannel;

	::StringCchCopy( nld.szUserid, GM_USER_ID+1, userid );
	::StringCchCopy( nld.szPassword, GM_USER_PASS+1, userpass );
	::StringCchCopy( nld.szEnCrypt, ENCRYPT_KEY+1, m_szEncryptKey );

	m_Tea.encrypt( nld.szUserid, GM_USER_ID+1 );
	m_Tea.encrypt( nld.szPassword, GM_USER_PASS+1 );
	m_Tea.encrypt( nld.szEnCrypt, ENCRYPT_KEY+1 );

	return Send( ( char* )&nld );
}

//
//US (GameSamba)
//
int CNetClient::UsSndLogin( const char* userid, const char* userpass, int nChannel )
{
	if ( nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER )
		nChannel = 0;

	US_NET_LOGIN_DATA nld;

	nld.nChannel = nChannel;

	::StringCchCopy( nld.szUserid, GS_USER_ID+1, userid );
	::StringCchCopy( nld.szPassword, GS_USER_PASS+1, userpass );
	::StringCchCopy( nld.szEnCrypt, ENCRYPT_KEY+1, m_szEncryptKey );

	m_Tea.encrypt( nld.szUserid, GS_USER_ID+1 );
	m_Tea.encrypt( nld.szPassword, GS_USER_PASS+1 );
	m_Tea.encrypt( nld.szEnCrypt, ENCRYPT_KEY+1 );

	return Send( ( char* )&nld );
}

//
//Thailand (태국)
//
int CNetClient::ThaiSndLogin(const char* userid, const char* userpass, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	THAI_NET_LOGIN_DATA tnld;
		
	tnld.nChannel  = nChannel;
	
	::StringCchCopy(tnld.szUserid,   USR_ID_LENGTH+1,   userid);
	::StringCchCopy(tnld.szPassword, USR_PASS_LENGTH+1, userpass);

	/// 암호방식 Tea 방식으로 변경됨. 
	/// 2005-11-16 Jgkim
	
	/*
	DWORD dwSize = tnld.nmg.dwSize - sizeof(NET_MSG_GENERIC);

	char* pBuffer = (char*) &tnld;
	m_Bit.buf_encode(pBuffer+sizeof(NET_MSG_GENERIC),
					 dwSize,
					 m_ck.nKeyDirection,
					 m_ck.nKey);
	*/
	m_Tea.encrypt( tnld.szUserid, USR_ID_LENGTH+1 );
	m_Tea.encrypt( tnld.szPassword, USR_PASS_LENGTH+1 );
	
	return Send((char *) &tnld);
}


//
//Terra (Malaysia, 필리핀, 베트남)
//
int CNetClient::TerraSndLogin(const char* szTID, int nChannel)
{
	if (szTID == NULL)
		return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	TERRA_NET_LOGIN_DATA tnld;
	
	tnld.nChannel = nChannel;
	::StringCchCopy(tnld.szTID, TERRA_TID_ENCODE+1, szTID);

	return Send((char *) &tnld);
}

int CNetClient::TerraSndPassCheck(const TCHAR* szUserid, const TCHAR* szUserPassword, int nCheckFlag)
{
	if( szUserid == NULL || szUserPassword == NULL )
		return NET_ERROR;

	TERRA_NET_PASSCHECK_DATA nld;

	::StringCchCopy( nld.szTLoginName, TERRA_TLOGIN_NAME+1, szUserid );
	::StringCchCopy( nld.szUserPass, TERRA_USERPASS+1, szUserPassword );
	nld.nCheckFlag = nCheckFlag;
	
	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}


//
//Japan
//
int CNetClient::JapanSndLogin(const char* szUserID, const char* szUserPassword, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	JAPAN_NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;
	
	::StringCchCopy(nld.szUserid,	JAPAN_USER_ID+1,	szUserID);
	::StringCchCopy(nld.szPassword,	JAPAN_USER_PASS+1,	szUserPassword);
	::StringCchCopy(nld.szEnCrypt,	ENCRYPT_KEY+1,		m_szEncryptKey);


	/// 암호방식 Tea 방식으로 변경됨. 
	/// 2005-11-16 Jgkim
	
	/*
	DWORD dwSize = nld.nmg.dwSize - sizeof(NET_MSG_GENERIC);

	char* pBuffer = (char*) &nld;
	m_Bit.buf_encode(pBuffer+sizeof(NET_MSG_GENERIC),
					 dwSize,
					 m_ck.nKeyDirection,
					 m_ck.nKey);
	*/	

	m_Tea.encrypt (nld.szUserid, JAPAN_USER_ID+1);
	m_Tea.encrypt (nld.szPassword, JAPAN_USER_PASS+1);
	m_Tea.encrypt (nld.szEnCrypt, ENCRYPT_KEY+1);
	
	return Send((char *) &nld);
}


//
//Excite Japan 로그인 데이터 전송
//
/*
	szU			UserID		char 20
	szT			유효기간	char 14
	szMD5		MD5 char	32
	nChannel	채널번호
*/
int CNetClient::ExciteSndLogin(const char* szU, const char* szT, const char* szMD5, int nChannel)
{
	if (szU == NULL || szT == NULL || szMD5 == NULL)
		return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	EXCITE_NET_LOGIN_DATA enld;

	enld.nChannel = nChannel;
	::StringCchCopy(enld.szUID,   EXCITE_USER_ID+1, szU);
	::StringCchCopy(enld.szTDATE, EXCITE_TDATE+1,   szT);
	::StringCchCopy(enld.szMD5,   EXCITE_MD5+1,     szMD5);

	return Send((char *) &enld);
}

int CNetClient::ExciteSndPassCheck(const TCHAR* szUserid, const TCHAR* szUserPassword, int nCheckFlag)
{
	if( szUserid == NULL || szUserPassword == NULL )
		return NET_ERROR;

	EXCITE_NET_PASSCHECK_DATA nld;

	::StringCchCopy( nld.szExciteUserID, EXCITE_SEC_ID+1, szUserid );
	::StringCchCopy( nld.szUserPass, EXCITE_SEC_PASS+1, szUserPassword );
	nld.nCheckFlag = nCheckFlag;

	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}


//
//China
//
int CNetClient::ChinaSndLogin(const TCHAR* szUserID,const TCHAR* szUserPassword, const TCHAR* szRandomPassword, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
		nChannel = 0;

	CHINA_NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;

	CHINA_MD5::MD5 md5; // md5 해쉬용 클래스
	// 비밀번호의 문자열 길이를 구한다.
	size_t nLength = 0;
	HRESULT sRet = StringCchLength( szUserPassword, USR_PASS_LENGTH, &nLength );
	
	if (sRet == STRSAFE_E_INVALID_PARAMETER)
		return NET_ERROR;

	// 해쉬값을 얻기 위해서 패스워드 문자열을 넣는다.
	// 해쉬값을 얻기 위해 업데이트 해 준다.
	md5.update( reinterpret_cast<const unsigned char*> (szUserPassword), nLength );	
	// 얻어진 해쉬값을 strPass 에 넣는다.
	std::string strPass = md5.asString();
	// 얻어진 해쉬값을 복사해 넣는다.
	StringCchCopy( nld.szUserid,   USR_ID_LENGTH+1, szUserID ); // ID 를 복사해 넣는다.
	StringCchCopy( nld.szPassword, USR_PASS_LENGTH, strPass.c_str() );
	StringCchCopy( nld.szRandomPassword, USR_RAND_PASS_LENGTH+1, szRandomPassword);
	
	// 넣어진 모든 값들을 암호화 한다.
	m_Tea.encrypt( nld.szUserid, USR_ID_LENGTH+1 );
	m_Tea.encrypt( nld.szPassword, USR_PASS_LENGTH );
	m_Tea.encrypt( nld.szRandomPassword, USR_RAND_PASS_LENGTH+1 );

	//EncryptLoginDataWithApexRSA(	(unsigned char*)nld.szUserid, 
	//								(unsigned char*)nld.szPassword, 
	//								(unsigned char*)nld.szRandomPassword );

	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}

//
//GSP
//
int CNetClient::GspSndLogin(const TCHAR* szUUID, int nChannel)
{
	if (szUUID == NULL)
		return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	GSP_NET_LOGIN_DATA msg;
	
	msg.nChannel = nChannel;
	::StringCchCopy( msg.szUUID, GSP_USERID, szUUID );
	// m_Tea.encrypt( msg.szUUID, GSP_USERID );

	return Send( (char *) &msg );
}


