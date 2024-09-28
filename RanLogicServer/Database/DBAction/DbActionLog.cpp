#include "../../pch.h"
#include "../../../SigmaCore/Util/DateTime.h"
#include "DbActionLog.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"


#include "../../../RanLogic/Msg/ServerManagerMsg.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{

LogServerState::LogServerState(
    int nSGNum,
    int nSvrNum,
    int nUserNum,
    int nUserMax)
    : m_nSGNum(nSGNum)
    , m_nSvrNum(nSvrNum)
    , m_nUserNum(nUserNum)
    , m_nUserMax(nUserMax)
{
    m_LogDate = sc::time::GetCurrentTime();
}

int LogServerState::Execute(NetServer* pServer)
{
	return m_pDbManager->AddLogServerState(m_LogDate, m_nSGNum, m_nSvrNum, m_nUserNum, m_nUserMax);
}

//----------------- from DbActionLogicLog -----------------

//mjeon
//DBAction for PING log
PingDbAction::PingDbAction(int nUserNum, int nRTT, const char* szIP, __time64_t tTime)
    : m_nUserNum(nUserNum)
    , m_nRTT(nRTT)
    , m_szIP(szIP)
    , m_tTime(tTime)
{	
}

int PingDbAction::Execute(NetServer* m_pServer)
{
	//return m_pDbManager->AddLogPing(m_nUserNum, m_nRTT, m_szIP, m_tTime );
	return m_pDbManager->AddLogPing(m_nUserNum, m_nRTT, m_szIP, m_tTime );
}

LogPartyMatch::LogPartyMatch(
    int nSGNum,
    int nSvrNum,
    WORD wWin,
    WORD wLost)
    : m_nSGNum(nSGNum)
    , m_nSvrNum(nSvrNum)
    , m_wWin(wWin)
    , m_wLost(wLost)
{
}

int LogPartyMatch::Execute(NetServer* pServer)
{
	m_pDbManager->LogPartyMatch(m_nSGNum, m_nSvrNum, m_wWin, m_wLost);
	return NET_OK;
}

LogItemExchange::LogItemExchange(const LOG_ITEM_EXCHANGE &sEXCHANGE)
    : db::DbAction(db::LOG_DB)
{
	m_sEXCHANGE = sEXCHANGE;
    
    if(m_sEXCHANGE.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogItemExchange() - m_LogDate value is initial value!!"));

        m_sEXCHANGE.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }

	m_sEXCHANGE.Verify();
}

int LogItemExchange::Execute(NetServer* pServer)
{
	m_pDbManager->LogItemExchange(m_sEXCHANGE);
	return NET_OK;
}

LogMoneyExchange::LogMoneyExchange(const LOG_MONEY_EXCHANGE& _sLOG)
{
	m_sLOG = _sLOG;

    if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogMoneyExchange() - m_LogDate value is initial value!!"));

        m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }

}

int LogMoneyExchange::Execute(NetServer* pServer)
{
	m_pDbManager->LogMoneyExchange(m_sLOG);
	return NET_OK;
}


LogPointShopExchange::LogPointShopExchange(int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail)
{
	m_nFromType = nFromType;
	m_dwChaDbNum = ChaDbNum;
	m_nExchangeFlag = nExchangeFlag;
	m_nItemMid = ItemMid;
	m_nItemSid= ItemSid;
	m_nPoint = nPoint;
	m_nExchangeFlagDetail = nExchangeFlagDetail;

	m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
}

int LogPointShopExchange::Execute(NetServer* pServer)
{
	m_pDbManager->LogPointShopExchange(m_nFromType,m_dwChaDbNum, m_nExchangeFlag, m_nItemMid, m_nItemSid, m_nPoint, m_nExchangeFlagDetail );
	return NET_OK;
}



LogItemConversion::LogItemConversion(const LOG_ITEM_CONVERSION &_sLOG)
    : m_sLOG(_sLOG)
{
    if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogItemConversion() - m_LogDate value is initial value!!"));

        m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }
}

int LogItemConversion::Execute(NetServer* pServer)
{
	m_pDbManager->LogItemConversion(m_sLOG);
	return NET_OK;
}

LogRandomItem::LogRandomItem(const LOG_RANDOM_OPTION& sLOG)
    : m_sLOG(sLOG)
{
    if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogRandomItem() - m_LogDate value is initial value!!"));

        m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }
}

int LogRandomItem::Execute(NetServer* pServer)
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem(SNATIVEID(m_sLOG.m_Mid,m_sLOG.m_Sid));
	if( pItem )
	{
		// 아이템이 바닥에 드랍되자마자 로그를 남긴다.
		// 때문에 쓸데없이 아이템의 로그가 많이 쌓이게 됨
		// 일반 등급 아이템은 로그를 남기지 않는다.
		if( pItem->sBasicOp.emLevel < EMITEM_LEVEL_HIGH )
			return NET_OK;
	}

	m_pDbManager->LogRandomItem(m_sLOG);

	return NET_OK;
}

LogCostumeStatItem::LogCostumeStatItem(const LOG_COSTUME_STATS& sLOG)
: m_sLOG(sLOG)
{
	if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
	{
		sc::writeLogError(std::string("LogCostumeStatItem() - m_LogDate value is initial value!!"));

		m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
	}
}

int LogCostumeStatItem::Execute(NetServer* pServer)
{	
	m_pDbManager->LogItemCostumeStat(m_sLOG);

	return NET_OK;
}

LogPetAction::LogPetAction(const LOG_PET_ACTION& sLOG)
    : m_sLOG(sLOG)
{
    if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogPetAction() - m_LogDate value is initial value!!"));

        m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }
}

int LogPetAction::Execute(NetServer* pServer)
{
	m_pDbManager->LogPetAction(m_sLOG);
	return NET_OK;
}

LogVehicleAction::LogVehicleAction(const LOG_VEHICLE_ACTION& sLOG)
    : m_sLOG(sLOG)
{
    if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogVehicleAction() - m_LogDate value is initial value!!"));

        m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }
}

int LogVehicleAction::Execute(NetServer* pServer)
{
	m_pDbManager->LogVehicleAction(m_sLOG);
	return NET_OK;
}

LogAppeal::LogAppeal(
    int nSGNum,
    int nChaNum,
    CString strMsg)
    : m_nSGNum(nSGNum)
    , m_nChaNum(nChaNum)
    , m_strMsg(strMsg)
{
}

int LogAppeal::Execute(NetServer* pServer)
{
	return m_pDbManager->LogAppeal(m_nSGNum, m_nChaNum, m_strMsg);
}

LogHackProgram::LogHackProgram(
    int nUserNum,
    int nChaNum,
    int nHackProgramNum,
    TCHAR* szINFO)
    : m_nUserNum(nUserNum)
    , m_nChaNum(nChaNum)
    , m_nHackProgramNum(nHackProgramNum)
{
	StringCchCopy( m_szINFO, LogHackProgram::MAX_INFO, szINFO );
}

int LogHackProgram::Execute(NetServer* pServer)
{
	return m_pDbManager->LogHackProgram(
		pServer->ServerGroup(),
		pServer->ServerNum(),
		m_nUserNum,
		m_nChaNum,
		m_nHackProgramNum,
		m_szINFO );
}

LogAction::LogAction(const LOG_ACTION &_sLOG)
    : m_sLOG(_sLOG)
{
    if(m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE)
    {
        sc::writeLogError(std::string("LogAction() - m_LogDate value is initial value!!"));

        m_sLOG.m_LogDate = sc::time::GetCurrentTime(); // 만일 시간값이 설정되어있지 않았다면, 현재시간으로 세팅한다
    }
}

int LogAction::Execute(NetServer* pServer)
{
	return m_pDbManager->LogAction(m_sLOG);
}

LogGuidance::LogGuidance(const LOG_GUIDANCE& sLog)
{
	m_sLog = sLog;
}

int LogGuidance::Execute(NetServer* pServer)
{
	m_pDbManager->LogGuidance(m_sLog);
	return NET_OK;
}


LogCDM::LogCDM(const LOG_CDM &sLog)
{
	m_sLog = sLog;
}

int LogCDM::Execute(NetServer* pServer)
{
	m_pDbManager->LogCDM(m_sLog);
	return NET_OK;
}


ResultLogCDM::ResultLogCDM(const LOG_CDM_RESULT &sLog)
{
	m_sLog = sLog;
}

int ResultLogCDM::Execute(NetServer* pServer)
{
	m_pDbManager->LogCDMReuslt(m_sLog);
	return NET_OK;
}

SendSMS::SendSMS(
    DWORD dwClientNum,
    DWORD dwSendChaNum,
    DWORD dwReceiveChaNum,
    TCHAR* szReceivePhone,
    TCHAR* szSendUserID,
    TCHAR* szSmsMsg,
    WORD wPosX,
    WORD wPosY)
{
	if (dwSendChaNum > 0 &&
		dwReceiveChaNum > 0 &&
		szReceivePhone != NULL &&
		szSendUserID != NULL &&
		szSmsMsg != NULL )
	{
		m_ClientSlot = dwClientNum;
		m_dwSendChaNum = dwSendChaNum;
		m_dwReceiveChaNum = dwReceiveChaNum;
		StringCchCopy( m_szReceivePhone, SMS_RECEIVER, szReceivePhone );
		StringCchPrintf(
			m_szSmsMsg,
			SMS_LENGTH,
			_T("RAN/%s/%s"),
			szSendUserID,
			szSmsMsg );
		m_wPosX = wPosX;
		m_wPosY = wPosY;

		m_bSMS = true;
	}
	else
	{
		m_bSMS = false;
	}
}

int SendSMS::Execute(NetServer* pServer)
{
	if (m_bSMS == true)
	{
		int nRESULT = m_pDbManager->LogSMS(	m_dwSendChaNum,
			m_dwReceiveChaNum,
			m_szReceivePhone,
			m_szSmsMsg );

		GLMSG::SNETPC_SEND_SMS_FROM_DB NetMsgDB2Fld;

		if( nRESULT == sc::db::DB_OK ) // 이름변경 성공
		{
			NetMsgDB2Fld.emFB = EMSMS_SEND_FROM_DB_OK;
			NetMsgDB2Fld.wPosX = m_wPosX;
			NetMsgDB2Fld.wPosY = m_wPosY;
		}
		else // 이름변경 실패
		{
			NetMsgDB2Fld.emFB = EMSMS_SEND_FROM_DB_FAIL;
		}

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(m_ClientSlot, (char*) &NetMsgDB2Fld );

		return nRESULT;
	}
	else
	{
		return sc::db::DB_ERROR;
	}
}

//! Insert PointShop log
LogPointShopInsert::LogPointShopInsert(DWORD ChaDbNum, const std::string& LogMessage)
    : DbAction(LOG_DB)
    , m_ChaDbNum(ChaDbNum)
    , m_LogMessage(LogMessage)
{
}

LogPointShopInsert::~LogPointShopInsert()
{
}

int LogPointShopInsert::Execute(NetServer* pServer)
{
    return m_pDbManager->LogPointShopInsert(m_ChaDbNum, m_LogMessage);
}

int LogPointShopInsertRenew::Execute( NetServer* pServer )
{
	/*
	DWORD m_ChaDbNum;
	EM_POINT_CHANGE_TYPE_RENEW m_LogType;
	CHARACTER_POINT m_BeforePoint;
	CHARACTER_POINT m_AfterPoint;
	SNATIVEID m_ItemID;
	*/
	__int64 nTemp1 = m_BeforePoint.Refundable();
	__int64 nTemp2 = m_BeforePoint.NotRefundable();
	__int64 nTemp3 = m_AfterPoint.Refundable();
	__int64 nTemp4 = m_AfterPoint.NotRefundable();


    return m_pDbManager->LogPointShopInsert(
		m_ChaDbNum, 
		static_cast<int>(m_LogType), 
		nTemp1, 
		nTemp2, 
		nTemp3, 
		nTemp4, 
		m_ItemID.Mid(), 
		m_ItemID.Sid() );
}



//
//mjeon.CaptureTheField
//
CTFStatLog::CTFStatLog(UINT *arrPlayerNum, UINT state)
    : m_state(state)
{
	GASSERT(arrPlayerNum);

	for (int i=0; i<SCHOOL_NUM; i++)
	{
		m_arrPlayerNum[i] = arrPlayerNum[i];
	}	
}

CTFStatLog::~CTFStatLog()
{
}

int CTFStatLog::Execute(NetServer* pServer)
{
	return m_pDbManager->CTFStatLog(m_arrPlayerNum, m_state);
}


CTFResultLog::CTFResultLog(WORD wSchoolAuthed, CTF_REWARD_BUFF *arrReward, UINT nHowMany)
    : m_wSchoolAuthed(wSchoolAuthed)
    , m_nHowMany(nHowMany)
{
	GASSERT(arrReward);

	for (UINT i=0; i<m_nHowMany; i++)
	{
		m_arrReward[i] = arrReward[i];
	}	
}

CTFResultLog::~CTFResultLog()
{
}

int CTFResultLog::Execute(NetServer* pServer)
{
	return m_pDbManager->CTFResultLog(m_wSchoolAuthed, m_arrReward, m_nHowMany);
}


CCTFPlayLog::CCTFPlayLog(DWORD dwChaNum, WORD wSchool, int nTotalPoint, int nRewardPoint)
    : m_nChaNum(dwChaNum)
    , m_wSchool(wSchool)
    , m_nTotalPoint(nTotalPoint)
    , m_nRewardPoint(nRewardPoint)
{
}

CCTFPlayLog::~CCTFPlayLog()
{
}

int CCTFPlayLog::Execute(NetServer* pServer)
{
	return m_pDbManager->CTFPlayLog(m_nChaNum, m_wSchool, m_nTotalPoint, m_nRewardPoint);
}

LogItemListInsertUpdate::LogItemListInsertUpdate(
    int ItemMid,
    int ItemSid,
    const std::string& ItemName)
    : m_ItemMid(ItemMid)
    , m_ItemSid(ItemSid)
    , m_ItemName(ItemName)
{
}

LogItemListInsertUpdate::~LogItemListInsertUpdate()
{
}

int LogItemListInsertUpdate::Execute(NetServer* pServer)
{
    return m_pDbManager->LogItemListInsertUpdate(m_ItemMid, m_ItemSid, m_ItemName);
}

//! 홀짝게임 결과 로그
LogMiniGameOddEvenInsert::LogMiniGameOddEvenInsert(
    DWORD UserDbNum,
    DWORD CharDbNum,        
    BYTE  CurrentRound,
    BYTE  WinLost,
    __int64 BattingMoney,
    __int64 DividendMoney,
    __int64 ChaMoney)
    : m_UserDbNum(UserDbNum)
    , m_CharDbNum(CharDbNum)
    , m_CurrentRound(CurrentRound)
    , m_WinLost(WinLost)
    , m_BattingMoney(BattingMoney)
    , m_DividendMoney(DividendMoney)
    , m_ChaMoney(ChaMoney)
{
}

LogMiniGameOddEvenInsert::~LogMiniGameOddEvenInsert()
{
}

int LogMiniGameOddEvenInsert::Execute(NetServer* pServer)
{
    return m_pDbManager->LogMiniGameOddEvenInsert(
        m_UserDbNum,
        m_CharDbNum,
        m_CurrentRound,
        m_WinLost,
        m_BattingMoney,
        m_DividendMoney,
        m_ChaMoney);    
}


int LogInspectionRewardPolicyInsert::Execute( NetServer* pServer )
{
	int nError = m_pSubDbManager->InspectionRewardPolicyInsert(tStart,tEnd,nRewardTime);

	GLMSG::NET_INSPECTION_REWARD_SET_ACK NetMsg;
	NetMsg.nResult = GLMSG::NET_INSPECTION_REWARD_SET_ACK::EM_ACK_DB_ERROR;

	if( nError == sc::db::DB_OK )
		NetMsg.nResult = GLMSG::NET_INSPECTION_REWARD_SET_ACK::EM_ACK_OK;

	// 서버에게 보낸다.
	CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
	if( pCacheServer == NULL )
		return sc::db::DB_ERROR;

	pCacheServer->SendToSession( &NetMsg);

	return nError;
}


CharMoneyNotify::CharMoneyNotify( DWORD dwChaDbNum, __time64_t tDate, LONGLONG llOldMoney, LONGLONG llNewMoney, int nCause, int nChannel, int nField )
: m_dwChaDbNum( dwChaDbNum )
, m_tDate( tDate )
, m_llOldMoney( llOldMoney )
, m_llNewMoney( llNewMoney )
, m_nCause( nCause )
, m_nChannel( nChannel )
, m_nField( nField )
{
}

int CharMoneyNotify::Execute( NetServer* pServer )
{
	return m_pDbManager->MoneyNotifyInsert( m_dwChaDbNum, m_tDate, m_llOldMoney, m_llNewMoney, m_nCause, m_nChannel, m_nField );
}


LogQuestListInsertUpdate::LogQuestListInsertUpdate( int Mid, int Sid, const std::string& Name )
: m_Mid( Mid )
, m_Sid( Sid )
, m_Name( Name )
{
}

LogQuestListInsertUpdate::~LogQuestListInsertUpdate()
{
}

int LogQuestListInsertUpdate::Execute( NetServer* pServer )
{
	return m_pDbManager->LogQuestListInsertUpdate( m_Mid, m_Sid, m_Name );
}


LogSkillListInsertUpdate::LogSkillListInsertUpdate( int Mid, int Sid, const std::string& Name, int Grade )
: m_Mid( Mid )
, m_Sid( Sid )
, m_Name( Name )
, m_Grade( Grade )
{
}

LogSkillListInsertUpdate::~LogSkillListInsertUpdate()
{
}

int LogSkillListInsertUpdate::Execute( NetServer* pServer )
{
	return m_pDbManager->LogSkillListInsertUpdate( m_Mid, m_Sid, m_Name, m_Grade );
}


LogGmCommandInsert::LogGmCommandInsert( int nChaNum, const std::string& Command, const std::string& Param )
: m_nChaNum( nChaNum )
, m_Command( Command )
, m_Param( Param )
{
}

int LogGmCommandInsert::Execute( NetServer* pServer )
{
	return m_pDbManager->LogGmCommandInsert( m_nChaNum, m_Command, m_Param );
}

LogGMChat::LogGMChat( int nChatType, int nGMType, const std::string& SendName, const std::string& RecvName, const std::string& ChatMsg )
: m_nChatType( nChatType )
, m_nGMType( nGMType )
, m_SendName( SendName )
, m_RecvName( RecvName )
, m_ChatMsg( ChatMsg )
{
}

int LogGMChat::Execute( NetServer* pServer )
{
	return m_pDbManager->LogGMChat( m_nChatType, m_nGMType, m_SendName, m_RecvName, m_ChatMsg );
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, Randombox 확률제어 항목 완료로그 저장
 * modified : 
 */
LogRandomboxChanceComplete::LogRandomboxChanceComplete(const SNATIVEID& BoxId,
                                                       const SNATIVEID& ItemId,
                                                       INT32 ReqCount,
                                                       DWORD ChaNum,
                                                       DWORD GMNum,
                                                       INT32 OpenCount,
                                                       DWORD Winner)
    : m_sBoxId(BoxId)
    , m_sItemId(ItemId)
    , m_nReqCount(ReqCount)
    , m_dwChaNum(ChaNum)
    , m_dwGMNum(GMNum)
    , m_nOpenCount(OpenCount)
    , m_dwWinner(Winner)
{
}

int LogRandomboxChanceComplete::Execute( NetServer* pServer )
{
	return m_pDbManager->LogRandomboxChanceComplete(m_sBoxId, m_sItemId, m_nReqCount, m_dwChaNum, m_dwGMNum, m_nOpenCount, m_dwWinner);
}


RanMobileRequest::RanMobileRequest( DWORD dwUserNum, DWORD dwChaNum, int nServerNum, int nCommand, int nValue, const std::string& strChaName, int nChaClass )
: m_dwUserNum( dwUserNum )
, m_dwChaNum( dwChaNum )
, m_SnerverNum( nServerNum )
, m_nCommand( nCommand )
, m_nValue( nValue )
, m_strChaName( strChaName )
, m_nChaClass( nChaClass )
{
}

int RanMobileRequest::Execute( NetServer* pServer )
{
	return m_pDbManager->RanMobileRequest( m_dwUserNum, m_dwChaNum, m_SnerverNum, m_nCommand, m_nValue, m_strChaName, m_nChaClass );
}




CMoneyLogInsertAction::CMoneyLogInsertAction (
	int _nMainType,
	int _nSubType,
	LONGLONG _llMoney )
	: m_nMainType ( _nMainType )
	, m_nSubType ( _nSubType )
	, m_llMoney ( _llMoney )
{
}

int CMoneyLogInsertAction::Execute( NetServer* _pServer )
{
	if ( NULL == _pServer )
		return sc::db::DB_ERROR;

	if ( NULL == m_pDbManager )
		return sc::db::DB_ERROR;

	if( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CMoneyLogInsertAction::Execute. Use ADO" );
		return sc::db::DB_ERROR;
	}

	int nReturn = sc::db::DB_ERROR;
	nReturn = m_pDbManager->MoneyLogInsert( m_nMainType, m_nSubType, m_llMoney );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError( "DbAction(CMoneyLogInsertAction) failed to call CMoneyLogInsertAction()." );
	}

	return nReturn;
}



LogAddonItem::LogAddonItem( const LOG_ADDON_OPTION& sLOG )
: m_sLOG( sLOG )
{
	if ( m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE )
	{
		sc::writeLogError( std::string( "LogAddonItem() - m_LogDate value is initial value!!" ) );
		m_sLOG.m_LogDate = sc::time::GetCurrentTime();
	}
}

int LogAddonItem::Execute( NetServer* pServer )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( SNATIVEID( m_sLOG.m_Mid, m_sLOG.m_Sid ) );
	if ( pItem )
	{
		// 아이템이 바닥에 드랍되자마자 로그를 남긴다.
		// 때문에 쓸데없이 아이템의 로그가 많이 쌓이게 됨
		// 일반 등급 아이템은 로그를 남기지 않는다.
		if ( pItem->sBasicOp.emLevel < EMITEM_LEVEL_HIGH )
			return NET_OK;
	}

	m_pDbManager->LogAddonItem( m_sLOG );
	return NET_OK;
}

LogLinkSkillItem::LogLinkSkillItem( const LOG_LINKSKILL_OPTION& sLOG )
: m_sLOG( sLOG )
{
	if ( m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE )
	{
		sc::writeLogError( std::string( "LogLinkSkillItem() - m_LogDate value is initial value!!" ) );
		m_sLOG.m_LogDate = sc::time::GetCurrentTime();
	}
}

int LogLinkSkillItem::Execute( NetServer* pServer )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( SNATIVEID( m_sLOG.m_Mid, m_sLOG.m_Sid ) );
	if ( pItem )
	{
		// 아이템이 바닥에 드랍되자마자 로그를 남긴다.
		// 때문에 쓸데없이 아이템의 로그가 많이 쌓이게 됨
		// 일반 등급 아이템은 로그를 남기지 않는다.
		if ( pItem->sBasicOp.emLevel < EMITEM_LEVEL_HIGH )
			return NET_OK;
	}

	m_pDbManager->LogLinkSkillItem( m_sLOG );
	return NET_OK;
}

LogBasicStatItem::LogBasicStatItem( const LOG_BASICSTAT_OPTION& sLOG )
: m_sLOG( sLOG )
{
	if ( m_sLOG.m_LogDate == DEFAULT_LOGDATE_VALUE )
	{
		sc::writeLogError( std::string( "LogBasicStatItem() - m_LogDate value is initial value!!" ) );
		m_sLOG.m_LogDate = sc::time::GetCurrentTime();
	}
}

int LogBasicStatItem::Execute( NetServer* pServer )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( SNATIVEID( m_sLOG.m_Mid, m_sLOG.m_Sid ) );
	if ( pItem )
	{
		// 아이템이 바닥에 드랍되자마자 로그를 남긴다.
		// 때문에 쓸데없이 아이템의 로그가 많이 쌓이게 됨
		// 일반 등급 아이템은 로그를 남기지 않는다.
		if ( pItem->sBasicOp.emLevel < EMITEM_LEVEL_HIGH )
			return NET_OK;
	}

	m_pDbManager->LogBasicStatItem( m_sLOG );
	return NET_OK;
}

} // namespace db
