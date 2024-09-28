#ifndef _DB_ACTION_GAME_H_
#define _DB_ACTION_GAME_H_

#pragma once

#include "../../../RanLogic/Msg/GLContrlCharJoinMsg.h"
#include "../../../RanLogic/Character/CharNameCacheDefine.h"
#include "../../Msg/AgentMsg.h"

#include "DbAction.h"

namespace db
{

// ----------------------------------------------------------------------------
// Agent Server Database Action
// ----------------------------------------------------------------------------

//! ĳ���� ����
class CharacterCreateNew : public DbAction
{
public:
    CharacterCreateNew(
        int nIndex, // ĳ���� �ε���
        DWORD dwUserNum, // ����ڹ�ȣ
        DWORD dwSvrGrp, // �����׷��ȣ
        CString strChaName, // ĳ�����̸�
        WORD wSchool, // ĳ�����б�
        WORD wHair, // ĳ���� �Ӹ����
        WORD wFace, // ĳ���� �󱼸��
		WORD wHairColor, // ��� �÷�
		WORD wSex, // ����
        DWORD dwClient, 
        const std::string& UserIP, 
        USHORT uPort=0,
		BOOL	bJumping = false
		);
	virtual ~CharacterCreateNew() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
    int     m_nIndex; 
    DWORD   m_dwUserNum;
    DWORD   m_dwSvrGrp;
    CString m_strChaName;
    WORD    m_wSchool;
    WORD    m_wHair;
    WORD    m_wFace;
	WORD	m_wHairColor;
	WORD	m_wSex;
	BOOL	m_bJumping;
};

// ĳ���� ����
class CharacterDelete : public DbAction
{
public:
	CharacterDelete(
        int nUserNum, 
        int nChaNum, 
        const char* szPass2, 
        DWORD dwClient, 
        const std::string& UserIP, 
        USHORT uPort=0);
	virtual ~CharacterDelete() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
	CString m_strPass2;
};

//! DB���� ĳ���͸� �¶��� ���·� �����.
class CharacterOnline : public DbAction
{
public:
	CharacterOnline(int nChaNum);
	virtual ~CharacterOnline() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
};

//! ĳ���͸� �������� ���·� �����.
class CharacterOffline : public DbAction
{
public:
	CharacterOffline(int nChaNum);
	virtual ~CharacterOffline() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
};

//! DAUM:ĳ���� ����
class CharacterDeleteDaum : public DbAction
{
public:
	CharacterDeleteDaum(
        int nUserNum, int nChaNum, const char* szPass2, DWORD dwClient, const std::string& UserIP, USHORT uPort=0);
	virtual ~CharacterDeleteDaum() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
	CString m_strPass2;
};


class CharacterDeleteIdn : public DbAction
{
public:
	CharacterDeleteIdn(
		int nUserNum, int nChaNum, DWORD dwClient, std::string &strUserID, std::string &str2ndPass, const std::string& UserIP, USHORT uPort=0);
	virtual ~CharacterDeleteIdn() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
	std::string m_strId;
	std::string m_str2ndPass;
};



//! TERRA:ĳ���� ����
class CharacterDeleteTerra : public DbAction
{
public:
    CharacterDeleteTerra(
        int nUserNum, int nChaNum, DWORD dwClient, const std::string& UserIP, USHORT uPort=0);
	virtual ~CharacterDeleteTerra() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
};

//! GSP:ĳ���� ����
class CharacterDeleteGsp : public DbAction
{
public:
	CharacterDeleteGsp(
		int nUserNum,
		int nChaNum,
		DWORD dwClient,
        const std::string& UserIP,
		USHORT uPort=0 );
	virtual ~CharacterDeleteGsp() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
};

//! EXCITE:ĳ���� ����
class CharacterDeleteExciteJapan : public DbAction
{
public:
	CharacterDeleteExciteJapan(
        int nUserNum, int nChaNum, DWORD dwClient, const std::string& UserIP, USHORT uPort=0);
	virtual ~CharacterDeleteExciteJapan() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
};

//! JAPAN:ĳ���� ����
class CharacterDeleteJapan : public DbAction
{
public:
	CharacterDeleteJapan(
        int nUserNum, int nChaNum, DWORD dwClient, const std::string& UserIP, USHORT uPort=0);
	virtual ~CharacterDeleteJapan() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
};

class CharacterDeleteGs : public DbAction
{
public:
	CharacterDeleteGs(
        int nUserNum, 
        int nChaNum, 
        const char* szPass2, 
        DWORD dwClient, 
        const std::string& UserIP, 
        USHORT uPort=0);
	virtual ~CharacterDeleteGs() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
	CString m_strPass2;
};

class CharacterBasicInfoAgent : public DbAction
{
public:
	CharacterBasicInfoAgent(int nUserNum, int nSvrGrp, DWORD AgentClientSlot, DWORD CacheClientSlot, const char* szUserIP, USHORT uPort=0);
	virtual ~CharacterBasicInfoAgent() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_AgentClientSlot;
	int m_nUserNum;
	int m_nSvrGrp;
	//NET_CHA_BBA_INFO m_msg;
};

//! �ش� ������ȣ�� �������� ���� ��� ĳ���� ������ �����´�.
//! Lobby Display ��
class CharacterBasicInfoAgentAll : public DbAction
{
public:
    CharacterBasicInfoAgentAll(
        int UserDbNum,
        int nSvrGrp,
        DWORD AgentClientSlot,
        DWORD CacheClientSlot,
        const char* szUserIP,
        USHORT uPort=0);
    virtual ~CharacterBasicInfoAgentAll();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_AgentClientSlot;
    int m_UserDbNum;
    int m_nSvrGrp;    
};

class CharacterBasicInfo : public DbAction
{
public:
	CharacterBasicInfo(int nUserNum, int nChaNum, DWORD AgentSlot, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CharacterBasicInfo() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_AgentSlot;
	int m_UserDbNum;
	int m_ChaDbNum;
	//GLMSG::SNETLOBBY_CHARINFO m_msg;
};

class CharacterBasicInfoNew : public DbAction
{
public:
    CharacterBasicInfoNew(int nUserNum, int nChaNum, DWORD AgentSlot, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
    virtual ~CharacterBasicInfoNew() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_AgentSlot;
    int m_UserDbNum;
    int m_ChaDbNum;
};

class CharacterInfoAndJoinAgent : public DbAction
{
public:
    CharacterInfoAndJoinAgent(
        DWORD dwClientID,
        DWORD dwUserID,
        DWORD dwUserLvl,
        __time64_t tPREMIUM,
        __time64_t tCHATBLOCK,
        DWORD dwServerID,
        DWORD dwCharID,
		int nUse2ndPass,
		const char *sz2ndPass); // 2�����
    virtual ~CharacterInfoAndJoinAgent();

protected:
	DWORD			m_dwClientID;
	GLCHARAG_DATA*	m_pCharAgentData;
    __time64_t      m_tPremiumDate;
	__time64_t      m_tChatBlock;
	std::string		m_str2ndPassword; // 2�����
	int			m_nUse2ndPass; // 0 : ������, 1 : ���, 2: �ű� ����

public:	
	virtual int Execute(NetServer* pServer) override;
};


// ----------------------------------------------------------------------------
// Field Server Database Action
// ----------------------------------------------------------------------------

//mjeon.AF
class CharacterInfoAndJoinField : public DbAction
{
public:
    //CharacterInfoAndJoinField(wb::EM_COUNTRY Country);
	CharacterInfoAndJoinField( UINT nServerGroup, NET_GAME_JOIN_FIELDSVR *pMsg, SCHARDATA2* pCharData, float fMagicNum );

protected:
	GLMSG::NET_CHAR_JOIN_DF	m_msgDF;

	virtual int Execute(NetServer* pServer) override;
};

class CharacterSave : public DbAction
{
public:
    CharacterSave();
	CharacterSave( SCHARDATA2* pCHARDATA, bool bAutoSave = false );
	CharacterSave( SCHARDATA2* pCHARDATA, DWORD dwGaeaID );	// WorldBattle���� ĳ���� ��������� ���
	virtual ~CharacterSave();
	virtual int Execute( NetServer* pServer ) override;

protected:
    void SetInfo( SCHARDATA2* pCHARDATA );

protected:
	SCHARDATA2	m_sCHARDATA;
	DWORD m_dwGaeaID;
	bool m_bAutoSave;
};

class UpdateUserLockerOption : public DbAction
{
public:
	UpdateUserLockerOption( DWORD UserNum, int Option );
	virtual ~UpdateUserLockerOption();
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_UserNum;
	int m_Option;
};

class CharacterInventoryAndGameMoneySave : public DbAction
{
public:
	CharacterInventoryAndGameMoneySave( SCHARDATA2* pCHARDATA );
	virtual ~CharacterInventoryAndGameMoneySave();
	virtual int Execute( NetServer* pServer );

protected:
	SCHARDATA2	m_sCHARDATA;

};

class CharacterDbToAgentMessage : public DbAction
{
public:
	CharacterDbToAgentMessage();
    CharacterDbToAgentMessage(DWORD dwClient, NET_MSG_GENERIC* nmg);
    CharacterDbToAgentMessage(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	virtual ~CharacterDbToAgentMessage();
	
	bool	SetMsg(DWORD dwClient, NET_MSG_GENERIC* nmg);
	BYTE*	GetMsg() {return m_aryMsg;}

	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwClientID;
	BYTE	m_aryMsg[NET_DATA_BUFSIZE];
};

//! ĳ�����߰�ī�� ������
//! ���������� ���������� ĳ���� ���ڸ� 1 ������Ų��.
class CharacterCreateNumIncrease : public DbAction
{
public:
    CharacterCreateNumIncrease(int nUserNum);
    virtual ~CharacterCreateNumIncrease() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nUserNum;
};

//! �ش� ��Ŀ ����ð��� ���Ѵ�.
//! 1 �� 5 �� ��Ŀ�� ���� �� ����.
//! 2, 3, 4 �� ��Ŀ�� ����ð��� ���� �� �ִ�.
class StorageDateSet : public DbAction
{
public:
    StorageDateSet(int nUserNum, int nStorageNum, __time64_t tTime);
    virtual ~StorageDateSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nUserNum;
    int m_nStorageNum;
    __time64_t m_tTime;
};

//! �ش� ĳ������ �κ��丮 �߰� �ټ��� �����Ѵ�.
//! ���� 
//! ���� 0 ���� ���� �߰��� 1 �� �Է�
//! ���� ���� ���¿��� �� ���� �߰��� 2 �� �Է� (���� �ټ��� �Է��Ѵ�)
//! ���� ���� ���¿��� ���� �߰��� 3 �� �Է�
//! �ִ� 3������ �Է°���.
class InvenAddNumSet : public DbAction
{
public :
    InvenAddNumSet(int nChaNum, WORD wInvenLine);
    virtual ~InvenAddNumSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaNum;
    WORD m_wInvenLine;
};

//! �ش� ������� �����̾� �Ⱓ�� �����Ѵ�.
class PremiumTimeSet : public DbAction
{
public:
    PremiumTimeSet(int nUserNum, __time64_t tPremiumTime);
    virtual ~PremiumTimeSet() {} ;
    virtual int Execute(NetServer* pServer) override;
protected:
    int m_nUserNum;
    __time64_t m_tPremiumTime;
};

//! ������ȯ ī�� ����� ���� �������� �����Ѵ�.
class CharacterLastCallPosSet : public DbAction
{
public:
    CharacterLastCallPosSet(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos);
    virtual ~CharacterLastCallPosSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaNum;
    DWORD m_dwMapID;
    D3DXVECTOR3 m_vPos;
};

//! �ش� ĳ������ ����ġ�� �����Ѵ�.
class CharacterExpSet : public DbAction
{
public:
	/**
	* �ش� ĳ������ ����ġ�� �����Ѵ�.
	* \n ����
    * \n ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
    * \n ����ġ�� 0 ���� Ŀ���Ѵ�.
	* \param nChaNum ĳ���͹�ȣ
	* \param llExp ����ġ
	* \return 
	*/
    CharacterExpSet(int nChaNum, LONGLONG llExp);
    virtual ~CharacterExpSet() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaNum;
    LONGLONG m_llExp;
};

//! �ش� ĳ������ ����ġ�� �����´�.
class CharacterExpGet : public DbAction
{
public:

	/**
	* �ش� ĳ������ ����ġ�� �����´�.
	* \n ���� 
    * \n ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
    * \n ����ġ�� 0 ���� ������ �����̴�.
	* \param nChaNum ĳ���͹�ȣ
	* \return ����ġ
	*/
    CharacterExpGet(int nChaNum);
    virtual ~CharacterExpGet() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaNum;
};

//! �ش������� ä�ú�Ͻð��� �����´�.
class UserChatBlockTimeGet : public DbAction
{
public:
	/**
	* �ش������� ä�ú�Ͻð��� �����´�.
	* \param nUserNum ����ڹ�ȣ
	* \return 
	*/
	UserChatBlockTimeGet(int nUserNum);
	virtual ~UserChatBlockTimeGet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! �ش������� ä�ú�Ͻð��� �����Ѵ�.
class UserChatBlockTimeSet : public DbAction
{
public:
	/**
	* �ش������� ä�ú�Ͻð��� �����Ѵ�.
	* \param nUserNum ����ڹ�ȣ
	* \param tBlockTime ä�� ��� ����ð�
	* \return 
	*/
	UserChatBlockTimeSet(int nUserNum, __time64_t tBlockTime);
	virtual ~UserChatBlockTimeSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	__time64_t m_tBlockTime;
};

//! ĳ���� �̸��� �����Ѵ�.
//! m_dwChaNum ĳ���͹�ȣ
//! m_szCharName ������ ĳ�����̸�
class CharacterRename : public DbAction
{
public:
	CharacterRename(
        DWORD dwClientID,
        DWORD dwChaNum, 
        const char* szCharName,
        WORD wPosX,
        WORD wPosY);
	virtual ~CharacterRename() {};
	virtual int Execute (NetServer* pServer);	

protected:
	DWORD m_dwChaNum;	
	char  m_szCharName[CHR_ID_LENGTH];
	WORD  m_wPosX;
	WORD  m_wPosY;
};

//! ĳ������ �Ӹ����� �����Ѵ�.
//! m_dwChaNum : ĳ���͹�ȣ
//! m_nHairColor : �Ӹ�����
class CharacterHairColorSet : public DbAction
{
public:
	CharacterHairColorSet(DWORD dwChaNum, 
		                   int nHairColor);
	virtual ~CharacterHairColorSet() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	int   m_nHairColor;
};

//! ĳ������ ��Ÿ���� �����Ѵ�.
//! m_dwChaNum : ĳ���� ��ȣ
//! m_nHairStyle : ��Ÿ��
class CharacterHairStyleSet : public DbAction
{
public:
	CharacterHairStyleSet(DWORD dwChaNum,
		                   int nHairStyle);
	virtual ~CharacterHairStyleSet() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	int m_nHairStyle;
};

//! ĳ������ �󱼽�Ÿ���� �����Ѵ�.
//! m_dwChaNum : ĳ���� ��ȣ
//! m_nFaceStyle : �󱼽�Ÿ��
class CharacterFaceStyleSet : public DbAction
{
public:
	CharacterFaceStyleSet(DWORD dwChaNum,
		                   int nFaceStyle);
	virtual ~CharacterFaceStyleSet() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	int m_nFaceStyle;
};

//! ĳ���� ������ �����Ѵ�.
class CharacterGenderChangeSet : public DbAction
{
public:
	CharacterGenderChangeSet( DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor );
	virtual ~CharacterGenderChangeSet() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	int m_nClass;
	int	m_nSex;
	int m_nFace;
	int m_nHair;
	int m_nHairColor;
};

// ĳ���� �п��� �����Ѵ�
class CharacterSchoolChangeSet : public DbAction
{
public:
	CharacterSchoolChangeSet( DWORD dwChaNum, int nSchool );
	virtual ~CharacterSchoolChangeSet() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	int m_nSchool;
};

//! ĳ������ ��ġ������ �����Ѵ�.
class CharacterPositionSet : public DbAction
{
public:
	//! ĳ������ ��ġ������ �����Ѵ�.
	//! \param dwChaNum ĳ���� ��ȣ
	//! \param dwStartmapID �����ϴ� �� ��ȣ
	//! \param dwStartGate �����ϴ� ����Ʈ ��ȣ
	//! \param fStartPosX �����ϴ� X ��ǥ
	//! \param fStartPosY �����ϴ� Y ��ǥ
	//! \param fStartPosZ �����ϴ� Z ��ǥ
	//! \param dwSaveMapID ���������� �÷����� �� ��ȣ
	//! \param fSavePosX ���������� �÷����� X ��ǥ
	//! \param fSavePosY ���������� �÷����� Y ��ǥ
	//! \param fSavePosZ ���������� �÷����� Z ��ǥ
	//! \param dwLastCallMapID ���� ��ȯī�忡 ����� �� ��ȣ
	//! \param fLastCallPosX ���� ��ȯī�忡 ����� X ��ǥ
	//! \param fLastCallPosY ���� ��ȯī�忡 ����� Y ��ǥ
	//! \param fLastCallPosZ ���� ��ȯī�忡 ����� Z ��ǥ	
	CharacterPositionSet(DWORD dwClientID,
				DWORD dwChaNum,
				DWORD dwFieldServer,
				DWORD dwStartMapID,
				DWORD dwStartGate,
				FLOAT fStartPosX,
				FLOAT fStartPosY,
				FLOAT fStartPosZ,
				DWORD dwSaveMapID,
				FLOAT fSavePosX,
				FLOAT fSavePosY,
				FLOAT fSavePosZ,
				DWORD dwLastCallMapID,
				FLOAT fLastCallPosX,
				FLOAT fLastCallPosY,
				FLOAT fLastCallPosZ);
	virtual ~CharacterPositionSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwChaNum;
	DWORD m_dwStartMapID;
	DWORD m_dwFieldServer;
	DWORD m_dwStartGate;
	FLOAT m_fStartPosX;
	FLOAT m_fStartPosY;
	FLOAT m_fStartPosZ;
	DWORD m_dwSaveMapID;
	FLOAT m_fSavePosX;
	FLOAT m_fSavePosY;
	FLOAT m_fSavePosZ;
	DWORD m_dwLastCallMapID;
	FLOAT m_fLastCallPosX;
	FLOAT m_fLastCallPosY;
	FLOAT m_fLastCallPosZ;
};
/*
//! ĳ������ �޴��� ��ȣ�� ����/������Ʈ�Ѵ�.
class CharacterPhoneNumberSet : public DbAction
{
public:
	
	//! ĳ������ �޴��� ��ȣ�� ����/������Ʈ�Ѵ�.
	//! \param nChaNum ĳ���� ��ȣ
	//! \param szPhoneNumber ��ȭ��ȣ NULL ���� ���� 14�� SMS_RECEIVER
    CharacterPhoneNumberSet(
		DWORD dwClientNum,
		DWORD dwChaNum,
		const TCHAR* szPhoneNumber );
	virtual ~CharacterPhoneNumberSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwChaNum;
	TCHAR m_szPhoneNumber[SMS_RECEIVER];
	bool m_bSMS;
};
*/
class UserAttendInsert : public DbAction
{
public:
	UserAttendInsert( DWORD dwUserID, CString strUserName, __time64_t tAttendTime, int nComboAttend, int nAttendReward, SNATIVEID sItemID );
	virtual ~UserAttendInsert() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwUserID;
	CString m_strUserName;
	int	  m_nComboAttend;
	int	  m_nAttendReward;
	__time64_t m_tAttendTime;
	SNATIVEID m_sItemID;
};

class ShopItemMapInsert : public DbAction
{
public:
	ShopItemMapInsert( int nItemMID, int nItemSID );
	virtual ~ShopItemMapInsert() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int	  m_nItemMID;
	int	  m_nItemSID;
};

class ThaiCafeClassCheck : public DbAction
{
public:
	ThaiCafeClassCheck ( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP );
	virtual ~ThaiCafeClassCheck() {};
	virtual int Execute(NetServer* pServer) override;
protected:
	DWORD m_dwClientID;
	DWORD m_dwCClassType;
	TCHAR m_szIp[MAX_IP_LENGTH+1];
};

class MyCafeClassCheck : public DbAction
{
public:
	MyCafeClassCheck ( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP );
	virtual ~MyCafeClassCheck() {};
	virtual int Execute(NetServer* pServer) override;
protected:
	DWORD m_dwClientID;
	DWORD m_dwCClassType;
	TCHAR m_szIp[MAX_IP_LENGTH+1];
};

class PhCafeClassCheck : public DbAction
{
public:
	PhCafeClassCheck ( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP );
	virtual ~PhCafeClassCheck() {};
	virtual int Execute(NetServer* pServer) override;
protected:
	DWORD m_dwClientID;
	DWORD m_dwCClassType;
	TCHAR m_szIp[MAX_IP_LENGTH+1];
};

class UserMoneyAddUpdate : public DbAction
{
public:
	UserMoneyAddUpdate ( const DWORD dwUserID, const LONGLONG lnUserMoney );
	virtual ~UserMoneyAddUpdate() {};
	virtual int Execute(NetServer* pServer) override;
protected:
	DWORD m_dwUserID;
	LONGLONG m_lnUserMoney;
};

class UserLastInfoAddUpdate : public DbAction
{
public:
	UserLastInfoAddUpdate ( const DWORD dwUserID, const LONGLONG lnUserMoney );
	virtual ~UserLastInfoAddUpdate() {};
	virtual int Execute(NetServer* pServer) override;
protected:
	DWORD m_dwUserID;
	LONGLONG m_lnUserMoney;
};

//! Character �� ȯ��/ȯ�� �Ұ� ����Ʈ�� �����´�
class CharacterPointGet : public DbAction
{
public:
    CharacterPointGet(int ChaDbNum);
    virtual ~CharacterPointGet();
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_ChaDbNum;
};


//
//mjeon.CaptureTheField
//
class GetChaExtraInfo : public DbAction
{
public:
	GetChaExtraInfo(DWORD dwClientSlot/*FieldServer's slot in this case*/, UINT nChaNum);
	virtual ~GetChaExtraInfo();
	virtual int Execute(NetServer* pServer) override;

protected:
	UINT			m_nChaNum;
};


class SetChaExtraInfo : public DbAction
{
public:
	SetChaExtraInfo(UINT nChaNum, int nContributionPoint);
	virtual ~SetChaExtraInfo();
	virtual int Execute(NetServer* pServer) override;

protected:
	UINT			m_nChaNum;
	int				m_nContributionPoint;
};

//! ĳ���� �̸����� DB ��ȣ ��û
class GetChaDbNum : public DbAction
{
public:
    GetChaDbNum(DWORD ClientSlot, DWORD ReqSeq, const std::string& ChaName, charnamechache::REQUEST_TYPE ReqType, const char* ReqCharName = NULL);
    virtual ~GetChaDbNum();
    virtual int Execute(NetServer* pServer) override;

protected:
    std::string m_ChaName;
    DWORD m_ReqSeq;
    charnamechache::REQUEST_TYPE m_ReqType;
    std::string m_ReqCharName;

};

class GetinfofromChaDb : public DbAction
{
public:
	GetinfofromChaDb(DWORD ClientSlot, DWORD ReqSeq, const std::string& ChaName, charnamechache::REQUEST_TYPE ReqType, const char* ReqCharName = NULL);
	virtual ~GetinfofromChaDb();
	virtual int Execute(NetServer* pServer) override;

protected:
	std::string m_ChaName;
	DWORD m_ReqSeq;
	charnamechache::REQUEST_TYPE m_ReqType;
	std::string m_ReqCharName;

};

class TransDBDataToWorldBattle : public DbAction
{
public:
	TransDBDataToWorldBattle( SCHARDATA2 *pChaData2, DWORD dwChaNum );
	virtual ~TransDBDataToWorldBattle() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	SCHARDATA2 m_sCharData2;
};

class ReserveServerStop : public DbAction
{
public:
	ReserveServerStop();
	virtual ~ReserveServerStop() {};
	virtual int Execute( NetServer* pServer ) override;	

};

class SetCharacterOption : public DbAction
{
public:
	SetCharacterOption( DWORD ChaNum, int CharacterOption );
	virtual ~SetCharacterOption() {};
	virtual int Execute( NetServer* pServer ) override;	

protected:
	DWORD m_ChaNum;
	int   m_CharacterOption;
};

class CalculateExpCompressor : public DbAction
{
public:
	CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp );
	virtual ~CalculateExpCompressor() {};
	virtual int Execute(NetServer* pServer) override;	

protected:
	DWORD m_dwChaNum;
	__int64 m_llCurExp;
	__int64 m_llCompressedExp;
};

} // namespace db
#endif // _DB_ACTION_GAME_H_
