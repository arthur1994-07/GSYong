#ifndef _LOGIC_SERVER_DB_ACTION_USER_H_
#define _LOGIC_SERVER_DB_ACTION_USER_H_

#pragma once

#include "DbAction.h"

//! Agent ���� : ĳ���� �������� 1 ����
class CAgentUserChaNumDecrease : public db::DbAction
{
public:
	CAgentUserChaNumDecrease(int nUserNum);
	virtual ~CAgentUserChaNumDecrease() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! TEST Session ���� : ĳ���� �������� 1 ����
class CUserTestChaNumDecrease : public db::DbAction
{
public:
	CUserTestChaNumDecrease(int nUserNum);
	virtual ~CUserTestChaNumDecrease() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! TEST Agent ���� : ĳ���� �������� 1 ����
class CAgentUserTestChaNumDecrease : public db::DbAction
{
public:
	CAgentUserTestChaNumDecrease(int nUserNum);
	virtual ~CAgentUserTestChaNumDecrease() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! Session : ������ �� �ִ� ĳ���� �� 1 ����
class CUserChaNumIncrease : public db::DbAction
{
public:
	CUserChaNumIncrease(int nUserNum);
	virtual ~CUserChaNumIncrease() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! Session ���� : ĳ���� �������� 1 ����
class CUserChaNumDecrease : public db::DbAction
{
public:
	CUserChaNumDecrease(int nUserNum);
	virtual ~CUserChaNumDecrease() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! Agent : ������ �� �ִ� ĳ���� �� 1 ����
class CAgentUserChaNumIncrease : public db::DbAction
{
public:
	CAgentUserChaNumIncrease(int nUserNum);
	virtual ~CAgentUserChaNumIncrease() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! TEST Session : ������ �� �ִ� ĳ���� �� 1 ����
class CUserTestChaNumIncrease : public db::DbAction
{
public:
	CUserTestChaNumIncrease(int nUserNum);
	virtual ~CUserTestChaNumIncrease() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! TEST Agent : ������ �� �ִ� ĳ���� �� 1 ����
class CAgentUserTestChaNumIncrease : public db::DbAction
{
public:
	CAgentUserTestChaNumIncrease(int nUserNum);
	virtual ~CAgentUserTestChaNumIncrease() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

class CUserUpdateCha : public db::DbAction
{
public:
	CUserUpdateCha(int nUserNum, const char* szCharName);
	virtual ~CUserUpdateCha();
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
    std::string m_strCharName;
};

//! �Ϻ� Agent : �α��� ó���� �� WhiteRock System�� ����ϴ� �κ� �߰�
class CAgentJapnaWRLogin : public db::DbAction
{
public:
	CAgentJapnaWRLogin(
		const char* szUserID,
		const char* szPassword,
        const std::string& strIP,
		int	nServerGroup,
		int nServerNum,
		DWORD dwClientNum 
		);
	virtual ~CAgentJapnaWRLogin();
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
	CString m_strPassword;
	CString m_strIP;
	int		m_nServerGroup;
	int		m_nServerNum;
	DWORD	m_dwClientNum;
};

//! �Ϻ� Agent : �α׾ƿ� ó���� �� WhiteRock System�� ����ϴ� �κ� �߰�
class CAgentJapnaWRLogout : public db::DbAction
{
public:
	CAgentJapnaWRLogout(const char* szUserUUID);
	virtual ~CAgentJapnaWRLogout() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserUUID;
};

class CUserLogoutSimple : public db::DbAction
{
public:
	CUserLogoutSimple(const CString& strUserID);
	virtual ~CUserLogoutSimple() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
};

class CUserLogoutSimple2 : public db::DbAction
{
public:
	CUserLogoutSimple2(int nUserNum);
	virtual ~CUserLogoutSimple2() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

class CUserLogout : public db::DbAction
{
public:
	CUserLogout(const char* szUserID, 
		int nUserNum, 
		int nGameTime,
		int nChaNum,
		int nSvrGrp,
		int nSvrNum,
		int nTotalGameTime,
		int nOfflineTime, 
		const char* szUserIP );
	virtual ~CUserLogout() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
	int m_nUserNum;
	int m_nGameTime;
	int m_nChaNum;
	int m_nSvrGrp;
	int m_nSvrNum;
	int m_nTotalGameTime;
	int m_nOfflineTime;
	CString m_strUserIP;
};

//! ����� üũ
//! Thailand (�±�)
class CAgentThaiUserCheck : public db::DbAction
{
public:
	CAgentThaiUserCheck(
        const char* szUserID, // ID 
        const char* szPasswd, // PWD
        const std::string& UserIP, // IP
        int nSvrGrp,          // �����׷�
        int nSvrNum,          // ������ȣ
        DWORD dwClient,
        DWORD dwClientNum);
	virtual ~CAgentThaiUserCheck () {} ;
	virtual int Execute (NetServer* pServer);

protected:
	CString m_strUserID;
	CString m_strPasswd;	
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum;
};


/**
 * \ingroup NetServerLib
 * ����� üũ
 * Taiwan / Hongkong
 */
class CAgentUserCheck : public db::DbAction
{
public:
	CAgentUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
        const std::string& UserIP, // IP
		const TCHAR* szRandomPasswd, // Random password
		int nRandomNum, // �����н����� ��ȣ
		int nSvrGrp, // �����׷�
		int nSvrNum, // ������ȣ
		DWORD dwClient,
		DWORD dwClientNum,
		DWORD dwServiceProvider );
	virtual ~CAgentUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
	CString m_strPasswd;
	CString m_strRandomPasswd;
	int m_nSvrGrp;
	int m_nSvrNum;
	int m_nRandomNum;
	DWORD m_dwClientNum;
	DWORD m_dwServiceProvider;
};


//
//mjeon.indonesia
//
class DIdnUserCheck : public db::DbAction
{
public:
	DIdnUserCheck(
        const std::string& strId,
        const std::string& strPass,
        const std::string& strIP,
        int nSvrGrp,
        int nSvrNum,
        DWORD dwClient,
        DWORD dwClientNum,
        DWORD dwServiceProvider);
	virtual ~DIdnUserCheck() {}
	virtual int Execute(NetServer* pServer) override;

protected:
	std::string	m_strId;
	std::string	m_strPass;

	int			m_nSvrGrp;
	int			m_nSvrNum;	
	
	DWORD		m_dwClientNum;
	DWORD		m_dwServiceProvider;
};

class WorldBattleUserCheck : public db::DbAction
{
public:
    WorldBattleUserCheck(
        const std::string& strId,
        const std::string& strPass,
        const std::string& strIP,
        int nSvrGrp,
        int nSvrNum,
        //DWORD dwClient,
        DWORD ClientSlot,
        DWORD dwServiceProvider);
    virtual ~WorldBattleUserCheck();
    virtual int Execute(NetServer* pServer) override;

protected:
    std::string	m_strId;
    std::string	m_strPass;

    int			m_nSvrGrp;
    int			m_nSvrNum;	

    //DWORD		m_dwClientNum;
    DWORD		m_dwServiceProvider;
};


/**
* Daum ����� üũ
*/

//class CAgentDaumUserCheck : public CDbAction
//{
//public:
//	/**
//	* Daum ����� üũ
//	* \param szDaumGID 
//	* \param szDaumUID 
//	* \param szDaumSSNHEAD 
//	* \param szDaumSEX 
//	* \param szUserIP 
//	* \param nSvrGrp 
//	* \param nSvrNum 
//	* \param dwClient 
//	* \param dwClientNum 
//	* \return 
//	*/
//	CAgentDaumUserCheck(const char* szDaumGID, 
//		                const char* szDaumUID,
//				        const char* szDaumSSNHEAD,
//				        const char* szDaumSEX,
//				        const char* szUserIP, 
//				        int nSvrGrp, 
//				        int nSvrNum,				   
//				        DWORD dwClient,
//				        DWORD dwClientNum);
//	virtual ~CAgentDaumUserCheck() {} ;
//	virtual int Execute(CServer* pServer);
//
//protected:
//	CString m_strDaumGID;
//	CString m_strDaumUID;
//	CString m_strDaumSSNHEAD;
//	CString m_strDaumSEX;
//	CString m_strUserIP;
//	int m_nSvrGrp;
//	int m_nSvrNum;
//	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
//};

/**
 * Daum ����� üũ
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentDaumUserCheck : public db::DbAction
{
public:
	CAgentDaumUserCheck(
		const TCHAR* szUUID,
        const std::string& UserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentDaumUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUUID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};


/**
 * GSP : ����� �α���
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentGspUserCheck : public db::DbAction
{
public:
	CAgentGspUserCheck(
		const TCHAR* szUUID,
        const std::string& UserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentGspUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUUID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



/**
 * TERRA : ����� �α���
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentTerraUserCheck : public db::DbAction
{
public:
	CAgentTerraUserCheck(
		const TCHAR* szDecodedTID,
        const std::string& UserIP, 
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentTerraUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strDecodedTID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



/**
 * \ingroup NetServerLib
 * ����� üũ
 * Taiwan / Hongkong
 */
class CAgentGsUserCheck : public db::DbAction
{
public:
	CAgentGsUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
        const std::string& UserIP, // IP
		int nSvrGrp, // �����׷�
		int nSvrNum, // ������ȣ
		DWORD dwClient,
		DWORD dwClientNum,
		DWORD dwServiceProvider );
	virtual ~CAgentGsUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
	CString m_strPasswd;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum;
	DWORD m_dwServiceProvider;
};

/**
 * \ingroup NetServerLib
 * ����� üũ
 * EU
 */
class CAgentGmUserCheck : public db::DbAction
{
public:
	CAgentGmUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
        const std::string& UserIP, // IP
		int nSvrGrp, // �����׷�
		int nSvrNum, // ������ȣ
		DWORD dwClient,
		DWORD dwClientNum,
		DWORD dwServiceProvider );
	virtual ~CAgentGmUserCheck() {} ;
	virtual int Execute( NetServer* pServer ) override;

protected:
	CString m_strUserID;
	CString m_strPasswd;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum;
	DWORD m_dwServiceProvider;
};

/**
 * \ingroup NetServerLib
 * ����� üũ
 * US
 */
class CAgentUsUserCheck : public db::DbAction
{
public:
	CAgentUsUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
        const std::string& UserIP, // IP
		int nSvrGrp, // �����׷�
		int nSvrNum, // ������ȣ
		DWORD dwClient,
		DWORD dwClientNum,
		DWORD dwServiceProvider );
	virtual ~CAgentUsUserCheck() {} ;
	virtual int Execute( NetServer* pServer ) override;

protected:
	CString m_strUserID;
	CString m_strPasswd;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum;
	DWORD m_dwServiceProvider;
};

/**
 * Daum : ����� Password Check
 * \param szDaumGID
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentDaumPassCheck : public db::DbAction
{
public:
	CAgentDaumPassCheck(
		const TCHAR* szDaumGID,
		const TCHAR* szUserPass,
		int nCheckFlag,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentDaumPassCheck() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strDaumGID;
	CString m_strUserPass;
	int		m_nCheckFlag;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



/**
 * TERRA : ����� Password Check
 * \param szTLoginName
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentTerraPassCheck : public db::DbAction
{
public:
	CAgentTerraPassCheck(
		const TCHAR* szTLoginName,
		const TCHAR* szUserPass,
		int nCheckFlag,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentTerraPassCheck() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strTLoginName;
	CString m_strUserPass;
	int		m_nCheckFlag;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



/**
* Excite ����� üũ
*/
class CAgentExciteUserCheck : public db::DbAction
{
public:	
	CAgentExciteUserCheck(
        const char* szUserID,
        const std::string& UserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum);
	virtual ~CAgentExciteUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



/**
* �Ϻ� Gonzo ����� üũ
*/
class CAgentJapanUserCheck : public db::DbAction
{
public:	
	CAgentJapanUserCheck(
        const char* szUserID,
        const std::string& UserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum,
		int nUserNum);
	virtual ~CAgentJapanUserCheck() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strUserID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	int m_nUserNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



/**
 * EXCITE : ����� Password Check
 * \param szExciteUserID
 * \param szUserPass
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentExcitePassCheck : public db::DbAction
{
public:
	CAgentExcitePassCheck(
		const TCHAR* szExciteUserID,
		const TCHAR* szExciteUserID2,
		const TCHAR* szUserPass,
		int nCheckFlag,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentExcitePassCheck() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	CString m_strExciteUserID;
	CString m_strExciteUserID2;
	CString m_strUserPass;
	int		m_nCheckFlag;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};



///////////////////////////////////////////////////////////////////////////////
// �Ϲ� : ����� �α׾ƿ�
class CUserLogoutSvr : public db::DbAction
{
public:
	CUserLogoutSvr(int nSvrGrp, int nSvrNum);
	virtual ~CUserLogoutSvr() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nSvrGrp;
	int m_nSvrNum;
};

//! ����ڸ� �����ð� �� ��Ų��.
//! nUserNum : ����ڹ�ȣ
//! nDay : �� ��ų �Ⱓ(��)
class CUserBlock : public db::DbAction
{
public:
	CUserBlock(int nUserNum, int nDay);
	virtual ~CUserBlock() {};	
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nDay;
};

class CUserBlockDate : public db::DbAction
{
public:
	CUserBlockDate(int nUserNum, __time64_t tBlockTime);
	virtual ~CUserBlockDate() {};
	virtual int Execute(NetServer* pServer) override;
protected:
	int m_nUserNum;
	__time64_t m_Date;
};

class UserDbLogItemDataInsertUpdate : public db::DbAction
{
public:
    UserDbLogItemDataInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName);
    virtual ~UserDbLogItemDataInsertUpdate();
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_ItemMid;
    int m_ItemSid;
    std::string m_ItemName;
};

class UserCountryUpdate : public db::DbAction
{
public:
    UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country);
    virtual ~UserCountryUpdate();
    virtual int Execute(NetServer* m_pServer) override;

protected:
    int m_UserDbNum;
    wb::EM_COUNTRY m_Country;
};

class UserSystemInfoInsert : public db::DbAction
{
public:
	UserSystemInfoInsert( 
		DWORD dwUserNum, 
		DWORD dwCpuCoreNumber, 
		DWORD dwCpuClock, 
		DWORDLONG dwMemorySize,
		DWORD dwResolutionX,
		DWORD dwResolutionY,
		DWORD dwGpuVendor,
		DWORD dwGpuModel,
		DWORD dwShaderModel, 
		int languageCode );
	virtual ~UserSystemInfoInsert() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_dwUserNum;
	DWORD m_dwCpuCoreNumber;
	DWORD m_dwCpuClock;
	DWORDLONG m_dwMemorySize;
	DWORD m_dwResolutionX;
	DWORD m_dwResolutionY;
	DWORD m_dwGpuVendor;		// 0 - Etc, 1 - Intel, 2 - Nvidia, 3 - AMD
	DWORD m_dwGpuModel;
	DWORD m_dwShaderModel;
	int m_languageCode;

};

// �� �׷����� �𸣰����� UserDB�� Log�� �ִ�.
class UserWriteGMLog : public db::DbAction
{
public:
    UserWriteGMLog(int nGmNum_, const std::string& strCmd_, const std::string& strUserIP_)
        : nGmNum(nGmNum_)
        , strCmd(strCmd_)
        , strUserIP(strUserIP_) {}
    virtual ~UserWriteGMLog() {};
    virtual int Execute(NetServer* pServer) override;
protected:
    int nGmNum;
    std::string strCmd;
    std::string strUserIP;
};


#endif // _LOGIC_SERVER_DB_ACTION_USER_H_




