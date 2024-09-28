#include "../pch.h"
#include "../Friend/GLFriendClient.h"
#include "../GroupChat/GroupChatClient.h"
#include "../GLGaeaClient.h"
#include "./DxGlobalStage.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DxGlobalStage::MsgProcessFrame(NET_MSG_GENERIC* pMsg)
{
    EMNET_MSG emType = pMsg->Type();   	
    if (emType < 0 || emType >= NET_MSG_GCTRL_MAX)
        return;

    if (m_GlobalStageMsgFunc[emType].Msgfunc)
        (this->*m_GlobalStageMsgFunc[emType].Msgfunc) (pMsg);
    else
        GaeaClientMsgProcessFrame(pMsg);
}

void DxGlobalStage::GaeaClientMsgFriend(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->GetFriendClient()->MsgProcess(pMsg);
}

void DxGlobalStage::LobyMsgProcessFrame(NET_MSG_GENERIC* pMsg)
{
    m_pLobyStage->MsgProcess(pMsg);
}

void DxGlobalStage::GameMsgProcessFrame(NET_MSG_GENERIC* pMsg)
{
    m_pGameStage->MsgProcess(pMsg);
}

void DxGlobalStage::GaeaClientMsgProcessFrame(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->MsgProcess(pMsg);
}

void DxGlobalStage::GaeaClientMsgProcessDirect(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->MsgProcess2(pMsg);
}

void DxGlobalStage::GaeaClientPingTrace(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->ProcessPingTraceMsg(pMsg);
}

void DxGlobalStage::GaeaClientAttendanceMaintenanceFB(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->ProcessAttendanceMaintenanceFB(pMsg);
}

void DxGlobalStage::GaeaClientCTFMoveStateByGMFB(NET_MSG_GENERIC* pMsg)
{
    m_pGaeaClient->ProcessCTFMoveStateByGMFB(pMsg);
}

void DxGlobalStage::GaeaClientMsgChatGroup(NET_MSG_GENERIC* pMsg)
{
	m_pGaeaClient->GetChatGroup()->MsgProcess(pMsg);
}

void DxGlobalStage::initPacketFunc()
{
    m_GlobalStageMsgFunc[NET_MSG_LOGIN_FB].Msgfunc           = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_PASSCHECK_FB].Msgfunc       = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHA_BAINFO_AC].Msgfunc      = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHARINFO_AC].Msgfunc  = &DxGlobalStage::LobyMsgProcessFrame;
	m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHARINFO_AC_END].Msgfunc  = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHA_DEL_FB_OK].Msgfunc      = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHA_DEL_FB_CLUB].Msgfunc    = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHA_DEL_FB_ERROR].Msgfunc   = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHA_NEW_FB].Msgfunc         = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[JAPAN_NET_MSG_UUID].Msgfunc         = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_JOIN_FB].Msgfunc = &DxGlobalStage::LobyMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHINA_ERROR].Msgfunc  = &DxGlobalStage::LobyMsgProcessFrame;
	m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHARJOIN_2NDPASS].Msgfunc  = &DxGlobalStage::LobyMsgProcessFrame; // 2차비번
	m_GlobalStageMsgFunc[NET_MSG_DEFAULT_GAME_OPTION].Msgfunc      = &DxGlobalStage::LobyMsgProcessFrame;
	m_GlobalStageMsgFunc[GS_NET_MSG_WB_LOGIN_FB].Msgfunc           = &DxGlobalStage::LobyMsgProcessFrame; // WB 접속여부 확인

    m_GlobalStageMsgFunc[NET_MSG_CHAT_CTRL_FB].Msgfunc            = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHAT_LINK_FB].Msgfunc            = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_SERVER_GENERALCHAT].Msgfunc      = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHAT_PRIVATE_FAIL].Msgfunc       = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_CHAT_BLOCK].Msgfunc              = &DxGlobalStage::GameMsgProcessFrame; 
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_JOIN].Msgfunc         = &DxGlobalStage::GameMsgProcessFrame;
    //m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_PUTON].Msgfunc        = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_PUTON_EX].Msgfunc     = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_ITEM].Msgfunc         = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_SKILL].Msgfunc        = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_QUEST_END].Msgfunc         = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_QUEST_PROG].Msgfunc        = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHARGE_ITEM].Msgfunc       = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_ITEM_COOLTIME].Msgfunc     = &DxGlobalStage::GameMsgProcessFrame;
	m_GlobalStageMsgFunc[NET_MSG_LOBBY_INSTANCE_LIST].Msgfunc		= &DxGlobalStage::GameMsgProcessFrame;
	m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_DEFAULT].Msgfunc     = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_INFO].Msgfunc         = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_INFO_EX].Msgfunc      = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_INFO_END].Msgfunc     = &DxGlobalStage::GameMsgProcessFrame;
    //m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_MEMBER].Msgfunc       = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK].Msgfunc = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK_END].Msgfunc = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_ALLIANCE].Msgfunc     = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CLUB_BATTLE].Msgfunc       = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_LOBBY_CHAR_VIETNAM_ITEM].Msgfunc = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_DROP_OUT_FORCED].Msgfunc   = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_REQ_TAKE_FB].Msgfunc       = &DxGlobalStage::GameMsgProcessFrame;       
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_TRADE_TAR].Msgfunc         = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_CONFRONT_TAR].Msgfunc      = &DxGlobalStage::GameMsgProcessFrame;
    //m_GlobalStageMsgFunc[NET_MSG_EVENT_LOTTERY_FB].Msgfunc        = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_APEX_ANSWER].Msgfunc             = &DxGlobalStage::GameMsgProcessFrame;
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_TAR].Msgfunc  = &DxGlobalStage::GaeaClientMsgProcessFrame;
//     m_GlobalStageMsgFunc[NET_MSG_ACTIVITY_SYNC].Msgfunc           = &DxGlobalStage::GameMsgProcessFrame;
//     m_GlobalStageMsgFunc[NET_MSG_ACTIVITY_REWARD_POINT_SYNC].Msgfunc  = &DxGlobalStage::GameMsgProcessFrame;
//     m_GlobalStageMsgFunc[NET_MSG_ACTIVITY_REWARD_BADGE_SYNC].Msgfunc  = &DxGlobalStage::GameMsgProcessFrame;

	m_GlobalStageMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT2].Msgfunc = &DxGlobalStage::GameMsgProcessFrame;
	    
    m_GlobalStageMsgFunc[NET_MSG_GM_PINGTRACE_ON_FB].Msgfunc  = &DxGlobalStage::GaeaClientPingTrace;
    m_GlobalStageMsgFunc[NET_MSG_GM_PINGTRACE_OFF_FB].Msgfunc = &DxGlobalStage::GaeaClientPingTrace;

	m_GlobalStageMsgFunc[NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB].Msgfunc = &DxGlobalStage::GaeaClientAttendanceMaintenanceFB;
	m_GlobalStageMsgFunc[NET_MSG_CTF_MOVESTATE_BY_GM_CA_FB].Msgfunc					= &DxGlobalStage::GaeaClientCTFMoveStateByGMFB;	

    m_GlobalStageMsgFunc[NET_MSG_REQ_FRIENDADD_FB].Msgfunc             = &DxGlobalStage::GaeaClientMsgFriend;
    m_GlobalStageMsgFunc[NET_MSG_REQ_FRIENDADD_LURE].Msgfunc           = &DxGlobalStage::GaeaClientMsgFriend;
    m_GlobalStageMsgFunc[NET_MSG_REQ_FRIENDDEL_FB].Msgfunc             = &DxGlobalStage::GaeaClientMsgFriend;
    m_GlobalStageMsgFunc[NET_MSG_REQ_FRIENDBLOCK_FB].Msgfunc           = &DxGlobalStage::GaeaClientMsgFriend;
    m_GlobalStageMsgFunc[NET_MSG_FRIENDINFO].Msgfunc                   = &DxGlobalStage::GaeaClientMsgFriend;
    m_GlobalStageMsgFunc[NET_MSG_FRIENDSTATE].Msgfunc                  = &DxGlobalStage::GaeaClientMsgFriend;
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_FRIEND_RENAME_CLT].Msgfunc      = &DxGlobalStage::GaeaClientMsgFriend;
    //m_GlobalStageMsgFunc[NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT].Msgfunc = &DxGlobalStage::GaeaClientMsgFriend;

    m_GlobalStageMsgFunc[NET_MSG_GM_KICK_USER_PROC].Msgfunc = &DxGlobalStage::MsgGmKickUserProc;

    m_GlobalStageMsgFunc[NET_MSG_GAMEGUARD_AUTH].Msgfunc   = &DxGlobalStage::MsgGameGuardAuth;
    m_GlobalStageMsgFunc[NET_MSG_GAMEGUARD_AUTH_1].Msgfunc = &DxGlobalStage::MsgGameGuardAuth1;
    m_GlobalStageMsgFunc[NET_MSG_GAMEGUARD_AUTH_2].Msgfunc = &DxGlobalStage::MsgGameGuardAuth2;

    m_GlobalStageMsgFunc[NET_MSG_APEX_DATA].Msgfunc = &DxGlobalStage::MsgApexData;

    m_GlobalStageMsgFunc[NET_MSG_USER_CHANGE_COUNTRY_AC].Msgfunc         = &DxGlobalStage::GaeaClientMsgProcessDirect;
    m_GlobalStageMsgFunc[NET_MSG_GCTRL_CLUB_NEW_FB].Msgfunc              = &DxGlobalStage::GaeaClientMsgProcessDirect;
    m_GlobalStageMsgFunc[NET_MSG_JSON].Msgfunc                           = &DxGlobalStage::GaeaClientMsgProcessDirect;
    m_GlobalStageMsgFunc[NET_MSG_CLUB_MEMBER_REQ_FB_AC].Msgfunc       = &DxGlobalStage::GaeaClientMsgProcessDirect;
    m_GlobalStageMsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_START_AC].Msgfunc    = &DxGlobalStage::GaeaClientMsgProcessDirect;
    m_GlobalStageMsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_AC].Msgfunc          = &DxGlobalStage::GaeaClientMsgProcessDirect;
    m_GlobalStageMsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_END_AC].Msgfunc      = &DxGlobalStage::GaeaClientMsgProcessDirect;

	m_GlobalStageMsgFunc[NET_MSG_GCHAT_INVITE_CAC].Msgfunc				 = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_INVITE_CAC_FB].Msgfunc			 = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_ERRORCODE].Msgfunc				 = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_QUIT_CA].Msgfunc					 = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_MEMBER_UPDATE_AC].Msgfunc		     = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_MEMBER_SYNC_AC].Msgfunc		     = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_KICK_CA].Msgfunc					 = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_MESSAGE].Msgfunc				     = &DxGlobalStage::GaeaClientMsgChatGroup;
	m_GlobalStageMsgFunc[NET_MSG_GCHAT_CONFIG_AC].Msgfunc				 = &DxGlobalStage::GaeaClientMsgChatGroup;

	m_GlobalStageMsgFunc[NET_MSG_POST_RENEW_LIST_ACK].Msgfunc				 = &DxGlobalStage::MsgPostRenew;
	m_GlobalStageMsgFunc[NET_MSG_POST_RENEW_SEND_ACK].Msgfunc				 = &DxGlobalStage::MsgPostRenew;
	m_GlobalStageMsgFunc[NET_MSG_POST_RENEW_DEL_ACK].Msgfunc				 = &DxGlobalStage::MsgPostRenew;
	m_GlobalStageMsgFunc[NET_MSG_POST_RENEW_ATTACH_ACK].Msgfunc				 = &DxGlobalStage::MsgPostRenew;	
	m_GlobalStageMsgFunc[NET_MSG_POST_RENEW_RETURN_ACK].Msgfunc				 = &DxGlobalStage::MsgPostRenew;	
	m_GlobalStageMsgFunc[NET_MSG_POST_RENEW_NEW_POST].Msgfunc				 = &DxGlobalStage::MsgPostRenew;	

	m_GlobalStageMsgFunc[NET_MSG_GCTRL_COUNTRY_CHANGE_AFC].Msgfunc			= &DxGlobalStage::MsgUpdateUserCountryAC;
	m_GlobalStageMsgFunc[NET_MSG_GCTRL_VCOUNTRY_SYNC_AC].Msgfunc			= &DxGlobalStage::MsgSyncVictoriousCountry;

	m_GlobalStageMsgFunc[NET_MSG_GCTRL_SYNC_USE_FEATURES_AC].Msgfunc		= &DxGlobalStage::MsgSyncUseFeatures;
	m_GlobalStageMsgFunc[NET_MSG_GCTRL_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC].Msgfunc		= &DxGlobalStage::MsgSyncUseFeaturesJoinWorldBattle;

	m_GlobalStageMsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE].Msgfunc		= &DxGlobalStage::MsgRanMobileCommandResponse;
}
