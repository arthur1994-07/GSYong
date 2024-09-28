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

//! 캐릭터 생성
class CharacterCreateNew : public DbAction
{
public:
    CharacterCreateNew(
        int nIndex, // 캐릭터 인덱스
        DWORD dwUserNum, // 사용자번호
        DWORD dwSvrGrp, // 서버그룹번호
        CString strChaName, // 캐릭터이름
        WORD wSchool, // 캐릭터학교
        WORD wHair, // 캐릭터 머리모양
        WORD wFace, // 캐릭터 얼굴모양
		WORD wHairColor, // 헤어 컬러
		WORD wSex, // 성별
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

// 캐릭터 삭제
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

//! DB에서 캐릭터를 온라인 상태로 만든다.
class CharacterOnline : public DbAction
{
public:
	CharacterOnline(int nChaNum);
	virtual ~CharacterOnline() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
};

//! 캐릭터를 오프라인 상태로 만든다.
class CharacterOffline : public DbAction
{
public:
	CharacterOffline(int nChaNum);
	virtual ~CharacterOffline() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nChaNum;
};

//! DAUM:캐릭터 삭제
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



//! TERRA:캐릭터 삭제
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

//! GSP:캐릭터 삭제
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

//! EXCITE:캐릭터 삭제
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

//! JAPAN:캐릭터 삭제
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

//! 해당 유저번호의 삭제되지 않은 모든 캐릭터 정보를 가져온다.
//! Lobby Display 용
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
		const char *sz2ndPass); // 2차비번
    virtual ~CharacterInfoAndJoinAgent();

protected:
	DWORD			m_dwClientID;
	GLCHARAG_DATA*	m_pCharAgentData;
    __time64_t      m_tPremiumDate;
	__time64_t      m_tChatBlock;
	std::string		m_str2ndPassword; // 2차비번
	int			m_nUse2ndPass; // 0 : 사용안함, 1 : 사용, 2: 신규 설정

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
	CharacterSave( SCHARDATA2* pCHARDATA, DWORD dwGaeaID );	// WorldBattle에서 캐릭터 저장용으로 사용
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

//! 캐릭터추가카드 아이템
//! 영구적으로 생성가능한 캐릭터 숫자를 1 증가시킨다.
class CharacterCreateNumIncrease : public DbAction
{
public:
    CharacterCreateNumIncrease(int nUserNum);
    virtual ~CharacterCreateNumIncrease() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nUserNum;
};

//! 해당 락커 만료시간을 정한다.
//! 1 번 5 번 락커는 정할 수 없음.
//! 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
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

//! 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
//! 주의 
//! 최초 0 에서 한줄 추가시 1 을 입력
//! 현재 한줄 상태에서 또 한줄 추가시 2 를 입력 (최종 줄수를 입력한다)
//! 현재 두줄 상태에서 한줄 추가시 3 을 입력
//! 최대 3까지만 입력가능.
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

//! 해당 사용자의 프리미엄 기간을 세팅한다.
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

//! 직전귀환 카드 사용을 위한 포지션을 저장한다.
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

//! 해당 캐릭터의 경험치를 세팅한다.
class CharacterExpSet : public DbAction
{
public:
	/**
	* 해당 캐릭터의 경험치를 세팅한다.
	* \n 주의
    * \n 캐릭터번호는 0 보다 커야한다.
    * \n 경험치는 0 보다 커야한다.
	* \param nChaNum 캐릭터번호
	* \param llExp 경험치
	* \return 
	*/
    CharacterExpSet(int nChaNum, LONGLONG llExp);
    virtual ~CharacterExpSet() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaNum;
    LONGLONG m_llExp;
};

//! 해당 캐릭터의 경험치를 가져온다.
class CharacterExpGet : public DbAction
{
public:

	/**
	* 해당 캐릭터의 경험치를 가져온다.
	* \n 주의 
    * \n 캐릭터번호는 0 보다 커야한다.
    * \n 경험치가 0 보다 작으면 에러이다.
	* \param nChaNum 캐릭터번호
	* \return 경험치
	*/
    CharacterExpGet(int nChaNum);
    virtual ~CharacterExpGet() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaNum;
};

//! 해당사용자의 채팅블록시간을 가져온다.
class UserChatBlockTimeGet : public DbAction
{
public:
	/**
	* 해당사용자의 채팅블록시간을 가져온다.
	* \param nUserNum 사용자번호
	* \return 
	*/
	UserChatBlockTimeGet(int nUserNum);
	virtual ~UserChatBlockTimeGet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
};

//! 해당사용자의 채팅블록시간을 설정한다.
class UserChatBlockTimeSet : public DbAction
{
public:
	/**
	* 해당사용자의 채팅블록시간을 설정한다.
	* \param nUserNum 사용자번호
	* \param tBlockTime 채팅 블록 만료시간
	* \return 
	*/
	UserChatBlockTimeSet(int nUserNum, __time64_t tBlockTime);
	virtual ~UserChatBlockTimeSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	__time64_t m_tBlockTime;
};

//! 캐릭터 이름을 변경한다.
//! m_dwChaNum 캐릭터번호
//! m_szCharName 변경할 캐릭터이름
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

//! 캐릭터의 머리색을 변경한다.
//! m_dwChaNum : 캐릭터번호
//! m_nHairColor : 머리색깔
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

//! 캐릭터의 헤어스타일을 변경한다.
//! m_dwChaNum : 캐릭터 번호
//! m_nHairStyle : 헤어스타일
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

//! 캐릭터의 얼굴스타일을 변경한다.
//! m_dwChaNum : 캐릭터 번호
//! m_nFaceStyle : 얼굴스타일
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

//! 캐릭터 성별을 변경한다.
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

// 캐릭터 학원을 변경한다
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

//! 캐릭터의 위치정보를 저장한다.
class CharacterPositionSet : public DbAction
{
public:
	//! 캐릭터의 위치정보를 저장한다.
	//! \param dwChaNum 캐릭터 번호
	//! \param dwStartmapID 시작하는 맵 번호
	//! \param dwStartGate 시작하는 게이트 번호
	//! \param fStartPosX 시작하는 X 좌표
	//! \param fStartPosY 시작하는 Y 좌표
	//! \param fStartPosZ 시작하는 Z 좌표
	//! \param dwSaveMapID 마지막으로 플레이한 맵 번호
	//! \param fSavePosX 마지막으로 플레이한 X 좌표
	//! \param fSavePosY 마지막으로 플레이한 Y 좌표
	//! \param fSavePosZ 마지막으로 플레이한 Z 좌표
	//! \param dwLastCallMapID 시작 귀환카드에 저장된 맵 번호
	//! \param fLastCallPosX 시작 귀환카드에 저장된 X 좌표
	//! \param fLastCallPosY 시작 귀환카드에 저장된 Y 좌표
	//! \param fLastCallPosZ 시작 귀환카드에 저장된 Z 좌표	
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
//! 캐릭터의 휴대폰 번호를 저장/업데이트한다.
class CharacterPhoneNumberSet : public DbAction
{
public:
	
	//! 캐릭터의 휴대폰 번호를 저장/업데이트한다.
	//! \param nChaNum 캐릭터 번호
	//! \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
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

//! Character 의 환불/환불 불가 포인트를 가져온다
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

//! 캐릭터 이름으로 DB 번호 요청
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
