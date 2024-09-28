#include "../pch.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CacheServer::InsertMsg(int nClient, void* pMsg)
{
    if (pMsg)
    {
        NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) pMsg;
        m_pRecvMsgManager->MsgQueueInsert(nClient, pNmg, pNmg->Size());
    }
    return 0;
}

int CacheServer::InsertMsg( int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc/*=false*/ )
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
		InsertMsg(nClient, &MsgPack);

	return 0;
}


void CacheServer::RecvMessageProcess()
{
    // Flip Messge Queue
    m_pRecvMsgManager->MsgQueueFlip();
    // Get Messages and Processing message
    MSG_LIST* pMsg = m_pRecvMsgManager->GetMsg( NULL );
    while (pMsg)
    {
        // 메시지 처리
        MessageProcess(pMsg);
        pMsg = m_pRecvMsgManager->GetMsg( pMsg ); // 다음메시지 가져오기
    }
}

void CacheServer::MessageProcess(MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    DWORD dwClient = pMsg->dwClient;
    NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg->Buffer;
    
    if (dwClient<0 || dwClient >= (DWORD) m_nMaxClient)
    {
        WRITE_LOG_ERROR( _T("CacheServer::MsgProcess dwClient out of range") );
    }
    else
    {        
        EMNET_MSG emType = EMNET_MSG(pPacket->nType);
        if (emType < 0 || emType >= NET_MSG_GCTRL_MAX )
        {				
            sc::writeLogError(
                sc::string::format(
                    "MessageProcess Out of range or illegal message:%1%>=%2%",
                    emType,
                    NET_MSG_GCTRL_MAX));
        }
        else
        {
            if (m_MsgFunc[emType].Msgfunc)
            {                
                (this->*m_MsgFunc[emType].Msgfunc)(dwClient, pPacket);
            }
        }
    }
}

void CacheServer::MsgJson(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_JSON* pPacket = (NET_JSON*) pMsg;
    char JsonStr[NET_JSON::BUF_SIZE] = {0};
    StringCchCopy(JsonStr, NET_JSON::BUF_SIZE, pPacket->Buffer);
    MsgJsonParser(dwClient, std::string(JsonStr));
}

bool CacheServer::MsgJsonParser(DWORD ClientSlot, const std::string& JsonStr)
{
    json_spirit::mValue Value;
    if (!json_spirit::read(JsonStr, Value))
    {
        // Json read error
        sc::writeLogError(std::string("MsgJsonParser json read error"));
        return false;
    }

    const json_spirit::mObject& JsonObject = Value.get_obj();
    int MessageType = 0;
    if (!sc::Json::find_value(JsonObject, "M", MessageType))
    {
        sc::writeLogError(std::string("Json message type 'M' not exist"));
        return false;
    }

    sc::writeLogError( sc::string::format( "Unknown Json message %1%", MessageType) );
    return true;
}

void CacheServer::InitPacketFunc()
{
    m_MsgFunc[NET_MSG_JSON].Msgfunc  = &CacheServer::MsgJson;
    m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_ANS].Msgfunc  = &CacheServer::MsgHeartbeatAns;
    m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_REQ].Msgfunc  = &CacheServer::MsgHeartbeatReq;
    m_MsgFunc[NET_MSG_LOG_ITEM_EXCHANGE].Msgfunc     = &CacheServer::MsgLogItemExchange;    
    m_MsgFunc[NET_MSG_LOG_MONEY_EXCHANGE].Msgfunc    = &CacheServer::MsgLogMoneyExchange;
    m_MsgFunc[NET_MSG_LOG_ITEM_CONVERSION].Msgfunc   = &CacheServer::MsgLogItemConversion;
    m_MsgFunc[NET_MSG_LOG_ACTION].Msgfunc            = &CacheServer::MsgLogAction;
    m_MsgFunc[NET_MSG_LOG_RANDOM_OPTION].Msgfunc     = &CacheServer::MsgLogItemRandomOption;
    m_MsgFunc[NET_MSG_LOG_PET_ACTION].Msgfunc        = &CacheServer::MsgLogPetAction;
    m_MsgFunc[NET_MSG_LOG_VEHICLE_ACTION].Msgfunc    = &CacheServer::MsgLogVehicleAction;
	m_MsgFunc[NET_MSG_LOG_INVEN_COSTUME_STATS].Msgfunc     = &CacheServer::MsgLogCostumeStat;

	m_MsgFunc[NET_MSG_LOG_ADDON_OPTION].Msgfunc     = &CacheServer::MsgLogItemAddonOption;
	m_MsgFunc[NET_MSG_LOG_LINKSKILL_OPTION].Msgfunc     = &CacheServer::MsgLogItemLinkSkillOption;
	m_MsgFunc[NET_MSG_LOG_BASICSTAT_OPTION].Msgfunc     = &CacheServer::MsgLogItemBasicStatOption;

	m_MsgFunc[NET_MSG_CTF_STAT_LOG_AH].Msgfunc		 = &CacheServer::MsgCTFStatLogAH;
	m_MsgFunc[NET_MSG_CTF_REWARD_INFO].Msgfunc		 = &CacheServer::MsgCTFResultLogAH;

	m_MsgFunc[NET_MSG_GET_CHA_EXTRA_INFO_FH].Msgfunc = &CacheServer::MsgGetChaExtraInfoFH;
	m_MsgFunc[NET_MSG_GET_CHA_EXTRA_INFO_DH].Msgfunc = &CacheServer::MsgGetChaExtraInfoDH;
	m_MsgFunc[NET_MSG_SET_CHA_EXTRA_INFO_FH].Msgfunc = &CacheServer::MsgSetChaExtraInfoFH;


    m_MsgFunc[NET_MSG_GAME_JOIN_AH].Msgfunc    = &CacheServer::MsgGameJoinAH;
    m_MsgFunc[NET_MSG_CHAR_CLOSE_AH].Msgfunc   = &CacheServer::MsgGameCloseAH;
    m_MsgFunc[NET_MSG_CHA_LIST_AH].Msgfunc     = &CacheServer::MsgChaListAH;

    m_MsgFunc[NET_MSG_PS_DATA_START_DS].Msgfunc = &CacheServer::MsgPsDataStartDS;
    m_MsgFunc[NET_MSG_PS_DATA_END_DS].Msgfunc   = &CacheServer::MsgPsDataEndDS;
    m_MsgFunc[NET_MSG_PS_DATA_DS].Msgfunc       = &CacheServer::MsgPsDataDS;

    m_MsgFunc[NET_MSG_PS_REQ_DATA_SH].Msgfunc     = &CacheServer::MsgPsReqDataSH;
    m_MsgFunc[NET_MSG_PS_RELOAD_SS].Msgfunc       = &CacheServer::MsgPsReloadSS;

    m_MsgFunc[NET_MSG_CHAR_POINT_DH].Msgfunc     = &CacheServer::MsgCharPointDH;
    m_MsgFunc[NET_MSG_CHAR_REQ_POINT_FH].Msgfunc = &CacheServer::MsgCharReqPointFH;
    
	m_MsgFunc[NET_MSG_CHAR_CHANGE_POINT_CACHE_FH].Msgfunc = &CacheServer::MsgCharChangePointFH;
    m_MsgFunc[NET_MSG_CHAR_PS_POINT_CHANGE_FH].Msgfunc = &CacheServer::MsgCharPsPointChangeFH;
    m_MsgFunc[NET_MSG_CHA_BAINFO_DH].Msgfunc           = &CacheServer::MsgChaBaInfoDH;
    m_MsgFunc[NET_MSG_LOBBY_CHAR_INFO_AH].Msgfunc      = &CacheServer::MsgChaLobbyInfoAH;
	m_MsgFunc[NET_MSG_LOBBY_CHAR_INFO_AH_END].Msgfunc      = &CacheServer::MsgChaLobbyInfoAHEnd;
    m_MsgFunc[NET_MSG_LOBBY_CHAR_INFO_DH].Msgfunc      = &CacheServer::MsgChaLobbyInfoDH;
    m_MsgFunc[NET_MSG_LOBBY_NEW_CHAR_INFO_DH].Msgfunc  = &CacheServer::MsgChaLobbyNewInfoDH;

    m_MsgFunc[NET_MSG_LOBBY_USER_INFO_DH].Msgfunc     = &CacheServer::MsgLobbyUserInfoDH;
    m_MsgFunc[NET_MSG_LOBBY_CHAR_INFO_DH2].Msgfunc    = &CacheServer::MsgLobbyCharInfoDH2;
    m_MsgFunc[NET_MSG_LOBBY_USER_INFO_END_DH].Msgfunc = &CacheServer::MsgLobbyUserInfoEndDH;

    m_MsgFunc[NET_MSG_CHAR_WEB_SHOP_LOG_FH].Msgfunc = &CacheServer::MsgCharWebShopLogFH;

    m_MsgFunc[NET_MSG_VERSION_SS].Msgfunc = &CacheServer::SessionMsgVersionSS;
    m_MsgFunc[NET_MSG_GM_LOG_ITEM_RELOAD_SS].Msgfunc = &CacheServer::DoNotiong;
    m_MsgFunc[NET_MSG_GM_GAME_DATA_UPDATE_SS].Msgfunc = &CacheServer::DoNotiong;

    m_MsgFunc[NET_MSG_ODD_EVEN_LOG_FH].Msgfunc = &CacheServer::MsgOddEvenLogFH;
    m_MsgFunc[NET_MSG_CLUB_LOG_AH].Msgfunc     = &CacheServer::MsgClubLogAH;
    m_MsgFunc[NET_MSG_CLUB_LOG].Msgfunc        = &CacheServer::MsgClubLog;

    m_MsgFunc[NET_MSG_CLUB_LOG_DH].Msgfunc     = &CacheServer::MsgClubLogDh;
    m_MsgFunc[NET_MSG_CLUB_LOG_DH_END].Msgfunc = &CacheServer::MsgClubLogDhEnd;

    m_MsgFunc[NET_LOG_ATTENDANCE_SH].Msgfunc      = &CacheServer::MsgLogAttendanceSH;
    m_MsgFunc[NET_LOG_ATTENDANCE_TASK_SH].Msgfunc = &CacheServer::MsgLogAttendanceTaskSH;

	//개인상점 검색 시스템 TEST
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_REGITEM_FH].Msgfunc = &CacheServer::MsgPmarketRegitemFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_REQ_CAH].Msgfunc = &CacheServer::MsgPmarketSearchReqAH;

	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SELLER_REGISTER_FH].Msgfunc = &CacheServer::MsgPrivateMarketSellerRegisterFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_REGISTER_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemRegisterFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemSearchBasicFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_BUY_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemBuyFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemPayMoneyFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_BUY_ROLLBACK_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemBuyRollbackFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_BUY_RESULT_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemBuyResultFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_FH].Msgfunc = &CacheServer::MsgPrivateMarketItemIsHoldFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_LOG_FH].Msgfunc = &CacheServer::MsgPrivateMarketLogFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_STATE_FH].Msgfunc = &CacheServer::MsgPrivateMarketStateFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_AH].Msgfunc = &CacheServer::MsgPrivateMarketItemSearchNameAH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_OPTION_FH].Msgfunc = &CacheServer::MsgPrivateMarketOptionFH;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_CLOSE_FH].Msgfunc = &CacheServer::MsgPrivateMarketCloseFH;
	// for global auth
	m_MsgFunc[NET_MSG_AUTH_CERTIFICATION_ANS].Msgfunc = &CacheServer::MsgAuthCertificationResponse;

    //m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK].Msgfunc = &CacheServer::MsgConsignmentSaleGetListAck;
    m_MsgFunc[NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ].Msgfunc = &CacheServer::MsgConsignmentSaleRegisterReq;
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ].Msgfunc = &CacheServer::MsgConsignmentSaleSendbackReq;
	m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK].Msgfunc = &CacheServer::MsgConsignmentSaleSendbackReq;
	m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_ACK].Msgfunc = &CacheServer::MsgConsignmentSalePayMoneyAck;
	m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK].Msgfunc = &CacheServer::MsgConsignmentSaleUpdateItemAck;

	// 기간제 아이템 시간 연장
	m_MsgFunc[NET_MSG_INSPECTION_REWARD_SET_REQ].Msgfunc	= &CacheServer::MsgInspectionRewardSet;

	// 우편 코드 개선
	m_MsgFunc[NET_MSG_POST_RENEW_LIST_REQ].Msgfunc		= &CacheServer::MsgPostRenewListReq;
	m_MsgFunc[NET_MSG_POST_RENEW_LIST_ACK].Msgfunc		= &CacheServer::MsgPostRenewListDBAck;
	m_MsgFunc[NET_MSG_POST_RENEW_SEND_REQ].Msgfunc		= &CacheServer::MsgPostRenewSendReq;
	m_MsgFunc[NET_MSG_POST_RENEW_SEND_ACK].Msgfunc		= &CacheServer::MsgPostRenewSendDBAck;
	m_MsgFunc[NET_MSG_POST_RENEW_DEL_REQ].Msgfunc		= &CacheServer::MsgPostRenewDelReq;
	m_MsgFunc[NET_MSG_POST_RENEW_ATTACH_REQ].Msgfunc	= &CacheServer::MsgPostRenewAttachReq;
	//m_MsgFunc[NET_MSG_POST_RENEW_ATTACH_ACK].Msgfunc	= &CacheServer::MsgPostRenewAttachDBAck;
	m_MsgFunc[NET_MSG_POST_RENEW_READ_REQ].Msgfunc		= &CacheServer::MsgPostRenewReadReq;
	m_MsgFunc[NET_MSG_POST_RENEW_RETURN_REQ].Msgfunc	= &CacheServer::MsgPostRenewReturnReq;
	m_MsgFunc[NET_MSG_POST_RENEW_RETURN_ACK].Msgfunc		= &CacheServer::MsgPostRenewReturnDBAck;
	m_MsgFunc[NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ].Msgfunc = &CacheServer::MsgPostRenewAttachRollbackReq;

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14, 신규등록
     * modified : 
     */
    m_MsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH].Msgfunc = &CacheServer::MsgGmSetRandomboxChanceReqAH;
    m_MsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH].Msgfunc = &CacheServer::MsgGmGetRandomboxChanceListAH;
    m_MsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH].Msgfunc = &CacheServer::MsgGmDelRandomboxChanceAH;

    m_MsgFunc[NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH].Msgfunc = &CacheServer::MsgRandomboxOpenChanceReqFH;
    m_MsgFunc[NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH].Msgfunc = &CacheServer::MsgRandomboxOpenChanceEndFH;

	m_MsgFunc[NET_MSG_GCTRL_LOG_CHANGEMONEY_FH].Msgfunc = &CacheServer::MsgLogMoneyFH;
}

void CacheServer::SessionMsgVersionSS(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_VERSION_SS* pPacket = (NET_VERSION_SS*) pMsg;
    m_LauncherVersion = pPacket->m_LauncherVersion;
    m_PatchVersion = pPacket->m_PatchVersion;

    //sc::writeLogInfo(sc::string::format("Launcher version %1%", pPacket->m_LauncherVersion));
    //sc::writeLogInfo(sc::string::format("Patch version %1%", pPacket->m_PatchVersion));
}

void CacheServer::DoNotiong(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    sc::writeLogInfo(
        sc::string::format(
            "Received message but do nothing. %1%", pMsg->Type()));
}
