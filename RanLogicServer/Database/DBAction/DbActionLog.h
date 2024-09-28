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
	__time64_t m_LogDate; //! �α׽ð�
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

//! �������� �����ɼ� �α׸� �����.
//! \param _sLOG �����ɼ� ����ü

class LogRandomItem : public DbAction
{
	// �������� �ٴڿ� ������ڸ��� �α׸� �����.
	// ������ �������� �������� �αװ� ���� ���̰� ��
	// ������ �̻� ���⵵�� �����Ѵ�.
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
	// �������� �ٴڿ� ������ڸ��� �α׸� �����.
	// ������ �������� �������� �αװ� ���� ���̰� ��
	// ������ �̻� ���⵵�� �����Ѵ�.
public:
	LogCostumeStatItem(const LOG_COSTUME_STATS& sLOG);
	virtual ~LogCostumeStatItem() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_COSTUME_STATS m_sLOG;
};

//! ���� �׼� �α׸� �����.
//! \param _sLOG ��׼Ƿα� ����ü
class LogPetAction : public DbAction
{
public:
	LogPetAction(const LOG_PET_ACTION& sLOG);
	virtual ~LogPetAction() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_PET_ACTION m_sLOG;
};


//! Ż���� �׼� �α׸� �����.
//! \param _sLOG ��׼Ƿα� ����ü
class LogVehicleAction : public DbAction
{
public:
	LogVehicleAction(const LOG_VEHICLE_ACTION& sLOG);
	virtual ~LogVehicleAction() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	LOG_VEHICLE_ACTION m_sLOG;
};

//! ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.
class LogAppeal : public DbAction
{
public:
	
	//! ���ӳ����� �����̳� �Ű�� �Ű��� ������ DB�� ����Ѵ�.
	//! \param nSGNum �����׷�
	//! \param nChaNum �Ű��� ĳ���͹�ȣ
	//! \param strMsg �Ű��� (�ִ�ũ��� CHAT_MSG_SIZE)	
    LogAppeal(int nSGNum, int nChaNum, CString strMsg);
    virtual ~LogAppeal() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nSGNum;
    int m_nChaNum;
    CString m_strMsg;
};

//! �ҹ����α׷� ��볻���� ����Ѵ�.
class LogHackProgram : public DbAction
{
	enum { MAX_INFO = 512 };

public:	
	//! �ҹ����α׷� ��볻���� ����Ѵ�.
	//! \param nUserNum ������ȣ
	//! \param nChaNum ĳ���͹�ȣ
	//! \param nHackProgramNum ��ŷ���α׷� ��ȣ
	//! \param *szINFO �޸�Ʈ...���ڿ�
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

//! ĳ���� Ȱ�� �α׸� ����Ѵ�.
class LogAction : public DbAction
{
public:
	
	//! ĳ���� Ȱ�� �α׸� ����Ѵ�.
	//! \param &_sLOG �αױ���ü	
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

//! SMS ���ڸ޽����� �߼��Ѵ�.
class SendSMS : public DbAction
{
public:	
	//! SMS ���ڸ޽����� �߼��Ѵ�.
	//! \param nSendChaNum SMS �� �߼��ϴ� ĳ���� ��ȣ
	//! \param szSendUserID SMS �� �߼��ϴ� ����� ID
	//! \param nReceiveChaNum SMS �� ���� ĳ���� ��ȣ
	//! \param szReceivePhone SMS �� ���� ĳ������ ��ȭ��ȣ
	//! \param szSmsMsg SMS �޽��� ����
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

//! Ȧ¦���� ��� �α�
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
    __int64 m_BattingMoney; //! ���ñݾ�
    __int64 m_DividendMoney; //! ���ݾ�
    __int64 m_ChaMoney; //! ���� ĳ���� �����ݾ�
    BYTE  m_CurrentRound; //! ���� ����
    BYTE  m_WinLost; //! �¸� �й�
};

//! �Ⱓ�� ������ �ð� ����
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
 * redmine : #1161 Randombox Ȯ������ �ý��� ó��
 * created : sckim, 2015.12.17, Randombox Ȯ������ �׸� �Ϸ�α� ����
 * modified : 
 */
class LogRandomboxChanceComplete : public DbAction
{
public:	
	LogRandomboxChanceComplete(const SNATIVEID& BoxId,  /* RandomBox ID */
                               const SNATIVEID& ItemId, /* ��÷ Item ID */
                               INT32 ReqCount,          /* ��ǥ ���� ���� */
                               DWORD ChaNum,            /* ��ǥ ĳ���� ��ȣ */
                               DWORD GMNum,             /* ��� GM Number */
                               INT32 OpenCount,         /* ���� ���� ���� */
                               DWORD Winner             /* ��÷�� */
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

// �������
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