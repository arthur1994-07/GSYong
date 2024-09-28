#ifndef _DB_ACTION_LOG_H_
#define _DB_ACTION_LOG_H_

#pragma once

#include "../DbDefineLog.h"
#include "DbAction.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/PointShop/PointShopDefine.h"

#include "../../../RanLogic/Money/MoneyLogDefine.h"

namespace db
{

//! Session Server Database Action
class LogServerState : public DbAction
{
public:
	LogServerState(int nSGNum, int nSvrNum, int nUserNum, int nUserMax);
	virtual ~LogServerState() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nSGNum;
	int m_nSvrNum;
	int m_nUserNum;
	int m_nUserMax;
    __time64_t m_LogDate;
};

//!mjeon
//!DBAction for PING log
class PingDbAction : public DbAction
{
public:
	PingDbAction( int nUserNum, int nRTT, const char* szIP, __time64_t tTime );
	virtual ~PingDbAction() {};
	int Execute(NetServer* m_pServer);		

protected:	
	int m_nUserNum;
	int m_nRTT;
	const char *m_szIP;
	__time64_t m_tTime;
};


class LogPartyMatch : public DbAction
{
public:
	LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost);
	virtual ~LogPartyMatch() {};
	virtual int Execute (NetServer* pServer);

protected:
	int		m_nSGNum;
	int		m_nSvrNum;
	WORD	m_wWin;
	WORD	m_wLost;	
};


class LogItemExchange : public DbAction
{
public:
	//!CLogItemExchange ();
	LogItemExchange(const LOG_ITEM_EXCHANGE& sEXCHANGE);
	virtual ~LogItemExchange() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_ITEM_EXCHANGE m_sEXCHANGE;
};


class LogMoneyExchange : public DbAction
{
public:
	LogMoneyExchange ();
	LogMoneyExchange ( const LOG_MONEY_EXCHANGE &_sLOG );
	virtual ~LogMoneyExchange() {};
	virtual int Execute (NetServer* pServer);

protected:
	LOG_MONEY_EXCHANGE m_sLOG;
};

class LogPointShopExchange : public DbAction
{
public:
	LogPointShopExchange ( int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail );
	virtual ~LogPointShopExchange() {};
	virtual int Execute (NetServer* pServer);

protected:
	int m_nFromType;
	DWORD m_dwChaDbNum;
	int m_nExchangeFlag;
	int m_nItemMid;
	int m_nItemSid;
	int m_nPoint;
	int m_nExchangeFlagDetail;
	__time64_t m_LogDate; //! 로그시간
};

class LogItemConversion : public DbAction
{
public:
	LogItemConversion(
		const LOG_ITEM_CONVERSION &_sLOG );
	virtual ~LogItemConversion() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_ITEM_CONVERSION m_sLOG;
};

//! 아이템의 랜덤옵션 로그를 남긴다.
//! \param _sLOG 랜덤옵션 구조체

class LogRandomItem : public DbAction
{
	// 아이템이 바닥에 드랍되자마자 로그를 남긴다.
	// 때문에 쓸데없이 아이템의 로그가 많이 쌓이게 됨
	// 레어등급 이상만 남기도록 강제한다.
public:
	LogRandomItem(const LOG_RANDOM_OPTION& sLOG);
	virtual ~LogRandomItem() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_RANDOM_OPTION m_sLOG;
};

class LogAddonItem : public DbAction
{
public:
	LogAddonItem( const LOG_ADDON_OPTION& sLOG );
	virtual ~LogAddonItem() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_ADDON_OPTION m_sLOG;
};

class LogLinkSkillItem : public DbAction
{
public:
	LogLinkSkillItem( const LOG_LINKSKILL_OPTION& sLOG );
	virtual ~LogLinkSkillItem() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_LINKSKILL_OPTION m_sLOG;
};

class LogBasicStatItem : public DbAction
{
public:
	LogBasicStatItem( const LOG_BASICSTAT_OPTION& sLOG );
	virtual ~LogBasicStatItem() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_BASICSTAT_OPTION m_sLOG;
};

class LogCostumeStatItem : public DbAction
{
	// 아이템이 바닥에 드랍되자마자 로그를 남긴다.
	// 때문에 쓸데없이 아이템의 로그가 많이 쌓이게 됨
	// 레어등급 이상만 남기도록 강제한다.
public:
	LogCostumeStatItem(const LOG_COSTUME_STATS& sLOG);
	virtual ~LogCostumeStatItem() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_COSTUME_STATS m_sLOG;
};

//! 펫의 액션 로그를 남긴다.
//! \param _sLOG 펫액션로그 구조체
class LogPetAction : public DbAction
{
public:
	LogPetAction(const LOG_PET_ACTION& sLOG);
	virtual ~LogPetAction() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_PET_ACTION m_sLOG;
};


//! 탈것의 액션 로그를 남긴다.
//! \param _sLOG 펫액션로그 구조체
class LogVehicleAction : public DbAction
{
public:
	LogVehicleAction(const LOG_VEHICLE_ACTION& sLOG);
	virtual ~LogVehicleAction() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_VEHICLE_ACTION m_sLOG;
};

//! 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.
class LogAppeal : public DbAction
{
public:
	
	//! 게임내에서 진정이나 신고로 신고한 내용을 DB에 기록한다.
	//! \param nSGNum 서버그룹
	//! \param nChaNum 신고한 캐릭터번호
	//! \param strMsg 신고내용 (최대크기는 CHAT_MSG_SIZE)	
    LogAppeal(int nSGNum, int nChaNum, CString strMsg);
    virtual ~LogAppeal() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nSGNum;
    int m_nChaNum;
    CString m_strMsg;
};

//! 불법프로그램 사용내역을 기록한다.
class LogHackProgram : public DbAction
{
	enum { MAX_INFO = 512 };

public:	
	//! 불법프로그램 사용내역을 기록한다.
	//! \param nUserNum 유저번호
	//! \param nChaNum 캐릭터번호
	//! \param nHackProgramNum 해킹프로그램 번호
	//! \param *szINFO 콤멘트...문자열
	LogHackProgram(
		int nUserNum,
		int nChaNum,
		int nHackProgramNum,
		char* szINFO );
	virtual ~LogHackProgram() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nUserNum;
	int m_nChaNum;
	int m_nHackProgramNum;
	TCHAR m_szINFO[LogHackProgram::MAX_INFO];
};

//! 캐릭터 활동 로그를 기록한다.
class LogAction : public DbAction
{
public:
	
	//! 캐릭터 활동 로그를 기록한다.
	//! \param &_sLOG 로그구조체	
	LogAction(const LOG_ACTION& _sLOG);
	virtual ~LogAction() {};
    virtual int Execute(NetServer* pServer) override;

protected:
	LOG_ACTION m_sLOG;
};

class LogGuidance : public DbAction
{
public:
	LogGuidance ();
	LogGuidance ( const LOG_GUIDANCE &sLog );
	virtual ~LogGuidance() {};
	virtual int Execute (NetServer* pServer);

protected:
	LOG_GUIDANCE m_sLog;
};

class LogCDM : public DbAction
{
public:
	LogCDM ();
	LogCDM ( const LOG_CDM &sLog );
	virtual ~LogCDM() {};
	virtual int Execute (NetServer* pServer);

protected:
	LOG_CDM m_sLog;
};


class ResultLogCDM : public DbAction
{
public:
	ResultLogCDM ();
	ResultLogCDM ( const LOG_CDM_RESULT &sLog );
	virtual ~ResultLogCDM() {};
	virtual int Execute (NetServer* pServer);

protected:
	LOG_CDM_RESULT m_sLog;
};

//! SMS 문자메시지를 발송한다.
class SendSMS : public DbAction
{
public:	
	//! SMS 문자메시지를 발송한다.
	//! \param nSendChaNum SMS 를 발송하는 캐릭터 번호
	//! \param szSendUserID SMS 를 발송하는 사용자 ID
	//! \param nReceiveChaNum SMS 를 받을 캐릭터 번호
	//! \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
	//! \param szSmsMsg SMS 메시지 내용
	SendSMS(
		DWORD dwClientNum,
		DWORD dwSendChaNum,
		DWORD dwReceiveChaNum,
		TCHAR* szReceivePhone,
		TCHAR* szSendUserID,
		TCHAR* szSmsMsg,
		WORD wPosX,
		WORD wPosY );
	virtual ~SendSMS() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwSendChaNum;
	DWORD m_dwReceiveChaNum;
	TCHAR m_szReceivePhone[SMS_RECEIVER];	
	TCHAR m_szSmsMsg[SMS_LENGTH];
	bool m_bSMS;
	WORD m_wPosX;
	WORD m_wPosY;
};

//! Insert PointShop log
class LogPointShopInsert : public DbAction
{
public:
    LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage);
    virtual ~LogPointShopInsert();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ChaDbNum;
    std::string m_LogMessage;
};

//! Insert PointShop log
class LogPointShopInsertRenew : public DbAction
{
public:
	LogPointShopInsertRenew(DWORD ChaDbNum, pointshop::EM_POINT_CHANGE_TYPE LogType, CHARACTER_POINT BeforePoint, CHARACTER_POINT AfterPoint, SNATIVEID ItemID )
	: m_ChaDbNum(ChaDbNum), m_LogType(LogType), m_BeforePoint(BeforePoint), m_AfterPoint(AfterPoint), m_ItemID(ItemID) {}
	virtual ~LogPointShopInsertRenew() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_ChaDbNum;
	pointshop::EM_POINT_CHANGE_TYPE m_LogType;
	CHARACTER_POINT m_BeforePoint;
	CHARACTER_POINT m_AfterPoint;
	SNATIVEID m_ItemID;
};


//
//mjeon.CaptureTheField
//
class CTFStatLog : public DbAction
{
public:
	CTFStatLog(UINT *arrPlayerNum, UINT state);
	virtual ~CTFStatLog();
	virtual int Execute(NetServer* pServer) override;

protected:
	UINT	m_arrPlayerNum[SCHOOL_NUM];
	UINT	m_state;
};


class CTFResultLog : public DbAction
{
public:
	CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany);
	virtual ~CTFResultLog();
	virtual int Execute(NetServer* pServer) override;

protected:
	CTF_REWARD_BUFF	m_arrReward[CTF_MAXSIZE_OF_BUFF_ARRAY_4_GLMSG];
	UINT			m_nHowMany;
	WORD			m_wSchoolAuthed;	
};

class CCTFPlayLog : public DbAction
{
public:
	CCTFPlayLog(DWORD dwChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint);
	virtual ~CCTFPlayLog();
	virtual int Execute(NetServer* pServer) override;

protected:
	UINT	m_nChaNum;
	WORD	m_wSchool;
	int		m_nTotalPoint;
	int		m_nRewardPoint;
};

class LogItemListInsertUpdate : public DbAction
{
public:
    LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName);
    virtual ~LogItemListInsertUpdate();
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_ItemMid;
    int m_ItemSid;
    std::string m_ItemName;
};

class LogQuestListInsertUpdate : public DbAction
{
public:
	LogQuestListInsertUpdate(int Mid, int Sid, const std::string& Name);
	virtual ~LogQuestListInsertUpdate();
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_Mid;
	int m_Sid;
	std::string m_Name;
};

class LogSkillListInsertUpdate : public DbAction
{
public:
	LogSkillListInsertUpdate(int Mid, int Sid, const std::string& Name, int Grade);
	virtual ~LogSkillListInsertUpdate();
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_Mid;
	int m_Sid;
	std::string m_Name;
	int m_Grade;
};

//! 홀짝게임 결과 로그
class LogMiniGameOddEvenInsert : public DbAction
{
public:
    LogMiniGameOddEvenInsert(
        DWORD UserDbNum,
        DWORD CharDbNum,        
        BYTE  CurrentRound,
        BYTE  WinLost,
        __int64 BattingMoney,
        __int64 DividendMoney,
        __int64 ChaMoney);
    virtual ~LogMiniGameOddEvenInsert();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_UserDbNum;
    DWORD m_CharDbNum;        
    __int64 m_BattingMoney; //! 배팅금액
    __int64 m_DividendMoney; //! 배당금액
    __int64 m_ChaMoney; //! 최종 캐릭터 소지금액
    BYTE  m_CurrentRound; //! 현재 라운드
    BYTE  m_WinLost; //! 승리 패배
};

//! 기간제 아이템 시간 연장
class LogInspectionRewardPolicyInsert : public DbAction
{
public:
	LogInspectionRewardPolicyInsert( __time64_t tStart_, __time64_t tEnd_, int nRewardTime_ )
		: tStart(tStart_)
		, tEnd(tEnd_)
		, nRewardTime(nRewardTime_){};
	virtual ~LogInspectionRewardPolicyInsert(){};
	virtual int Execute(NetServer* pServer) override;

protected:
	__time64_t tStart;
	__time64_t tEnd;
	int nRewardTime;
};

class CharMoneyNotify : public DbAction
{
public:	
	CharMoneyNotify(
		DWORD dwChaDbNum,
		__time64_t tDate,
		LONGLONG llOldMoney,
		LONGLONG llNewMoney, 
		int nCause,
		int nChannel,
		int nField );
	virtual ~CharMoneyNotify() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_dwChaDbNum;
	__time64_t m_tDate;
	LONGLONG m_llOldMoney;
	LONGLONG m_llNewMoney;
	int m_nCause;
	int m_nChannel;
	int m_nField;
};

class LogGmCommandInsert : public DbAction
{
public:	
	LogGmCommandInsert( int nChaNum, const std::string& Command, const std::string& Param );
	virtual ~LogGmCommandInsert() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
	int m_nChaNum;
	std::string m_Command;
	std::string m_Param;
};

class LogGMChat : public DbAction
{
public:	
	LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg );
	virtual ~LogGMChat() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
	int m_nChatType;
	int m_nGMType;
	std::string m_SendName;
	std::string m_RecvName;
	std::string m_ChatMsg;
};

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, Randombox 확률제어 항목 완료로그 저장
 * modified : 
 */
class LogRandomboxChanceComplete : public DbAction
{
public:	
	LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                               const SNATIVEID& ItemId, /* 당첨 Item ID */
                               INT32 ReqCount,          /* 목표 오픈 수량 */
                               DWORD ChaNum,            /* 목표 캐릭터 번호 */
                               DWORD GMNum,             /* 등록 GM Number */
                               INT32 OpenCount,         /* 실제 오픈 수량 */
                               DWORD Winner             /* 당첨자 */
                              );
	virtual ~LogRandomboxChanceComplete() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
    SNATIVEID m_sBoxId;
    SNATIVEID m_sItemId;
    INT32 m_nReqCount;
    DWORD m_dwChaNum;
    DWORD m_dwGMNum;
    INT32 m_nOpenCount;
    DWORD m_dwWinner;
};

// 란모바일
class RanMobileRequest : public DbAction
{
public:	
	RanMobileRequest(
		DWORD dwUserNum,
		DWORD dwChaNum,
		int nServerNum,
		int nCommand,
		int nValue,
		const std::string& strChaName, 
		int nChaClass );
	virtual ~RanMobileRequest() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_dwUserNum;
	DWORD m_dwChaNum;
	int m_SnerverNum;
	int m_nCommand;
	int m_nValue;
	std::string m_strChaName;
	int m_nChaClass;
};


// Money Log;
class CMoneyLogInsertAction : public DbAction
{
public:	
	CMoneyLogInsertAction(
		int _nMainType,
		int _nSubType,
		LONGLONG _llMoney );
	virtual ~CMoneyLogInsertAction ( ) { };

public:
	virtual int Execute( NetServer* pServer ) override;

private:
	int m_nMainType;
	int m_nSubType;
	LONGLONG m_llMoney;
};

} // namespace db
#endif //! _DB_ACTION_LOG_H_