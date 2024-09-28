#include "../pch.h"
#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Transport/GLBusStation.h"
#include "../../RanLogic/Transport/GLBusData.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../../RanLogic/GLChatLinkDef.h"
#include "../../RanLogic/Post/GLPostData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/ReferCharMsg.h"
#include "../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/Item/ItemDurability.h"
#include "../../RanLogic/Event/GLIPEvent.h"
#include "../../RanLogic/RanMobile/GLRanMobile.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubServer.h"
#include "../Club/GLClubMan.h"
#include "../Event/GLEventProc.h"
#include "../Event/EventField/EventField.h"
#include "../Land/Faction/FactionManagerField.h"
#include "../Database/DBAction/DBActionUser.h"
#include "../CountRecorder/CountRecorder.h"

#include "../Database/DBAction/DbActionGamePet.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbActionShop.h"
#include "../Database/DBAction/DbActionGameConsignmentSale.h"
#include "../Database/DbManager.h"
#include "../Guidance/GuidanceField.h"
#include "../PK/GLSchoolFreePK.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../Util/GLItemLimit.h"

#include "./GLGaeaServer.h"

#include "../../RanLogic/Msg/MapMsg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL	g_bAttendanceDailyMaintenance;


void GLGaeaServer::initPacketFunc()
{
	m_MsgFunc[NET_MSG_CHAT_LINK].Msgfunc = &GLGaeaServer::ChatMsgLinkProc;
	m_MsgFunc[NET_MSG_CHAT_LINK_FB].Msgfunc = &GLGaeaServer::ChatMsgLinkAFProc;
	m_MsgFunc[NET_MSG_GCTRL_NPC_RECALL].Msgfunc = &GLGaeaServer::RequestNpcRecall;
	m_MsgFunc[NET_MSG_GCTRL_NPC_COMMISSION_FB].Msgfunc = &GLGaeaServer::RequestNpcCommission;
	m_MsgFunc[NET_MSG_MARKET_CHECKTIME].Msgfunc = &GLGaeaServer::CheckMarketState;
	m_MsgFunc[NET_MSG_GCTRL_CHARGED_ITEM2_INVEN].Msgfunc = &GLGaeaServer::RequestChargedItem2Inven;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTSPTY_EXP_FLD].Msgfunc = &GLGaeaServer::RequestConftSPtyExp;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GATEOUT_REQ].Msgfunc = &GLGaeaServer::RequestGateOutReq;
	m_MsgFunc[NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ].Msgfunc = &GLGaeaServer::RequestCreateInstantMapReq;
	m_MsgFunc[NET_MSG_GCTRL_INSTANTMAP_MOVEOUT_REQ].Msgfunc = &GLGaeaServer::RequestInstantMapMoveOut;

	m_MsgFunc[NET_MSG_TRANSFER_REQUEST_AF].Msgfunc = &GLGaeaServer::TransferToInstance;
	m_MsgFunc[NET_MSG_TRANSFER_CHAR].Msgfunc = &GLGaeaServer::TransferChar;
	m_MsgFunc[NET_MSG_TRANSFERRED_CHAR_JOIN_AF].Msgfunc = &GLGaeaServer::TransferCharJoinAF;

	m_MsgFunc[NET_MSG_TRANSFER_BACK_MI].Msgfunc			= &GLGaeaServer::TransferFromInstance;


	m_MsgFunc[NET_MSG_GCTRL_NOTICE_MOVEMAP_RESULT_AFB].Msgfunc = &GLGaeaServer::RequestMoveMapStateFC;
	m_MsgFunc[NET_MSG_MOVEMAP_AF].Msgfunc = &GLGaeaServer::RequestFieldSvrOut;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GATEOUT].Msgfunc = &GLGaeaServer::RequestMoveMapPC;
	m_MsgFunc[NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD].Msgfunc = &GLGaeaServer::RequestCreateInstantMap;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_LANDIN].Msgfunc = &GLGaeaServer::RequestLandIn;
	m_MsgFunc[NET_MSG_GCTRL_TRADE].Msgfunc = &GLGaeaServer::RequestTrade;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_TAR_ANS].Msgfunc = &GLGaeaServer::RequestTradeTarAns;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_MONEY].Msgfunc = &GLGaeaServer::RequestTradeMoney;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_ITEM_REGIST].Msgfunc = &GLGaeaServer::RequestTradeItemResist;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_ITEM_REMOVE].Msgfunc = &GLGaeaServer::RequestTradeItemReMove;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_LOCK].Msgfunc = &GLGaeaServer::RequestTradeLock;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_AGREE].Msgfunc = &GLGaeaServer::RequestTradeAgree;
	m_MsgFunc[NET_MSG_GCTRL_TRADE_CANCEL].Msgfunc = &GLGaeaServer::RequestTradeCancel;
	m_MsgFunc[NET_MSG_FIELDSVR_CHARCHK].Msgfunc = &GLGaeaServer::RequestFieldSvrCharChk;
	m_MsgFunc[NET_MSG_REQ_MUST_LEAVE_MAP].Msgfunc = &GLGaeaServer::RequestMustLeaveMap;
	m_MsgFunc[NET_MSG_GCTRL_REQ_RECALL].Msgfunc = &GLGaeaServer::RequestInvenRecall;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TELEPORT].Msgfunc = &GLGaeaServer::RequestInvenTeleport;
	m_MsgFunc[NET_MSG_GCTRL_REQ_BUS].Msgfunc = &GLGaeaServer::RequestBus;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TAXI].Msgfunc = &GLGaeaServer::RequestTaxi;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TAXI_NPCPOS].Msgfunc = &GLGaeaServer::RequestTaxiNpcPos;
	m_MsgFunc[NET_MSG_GCTRL_2_FRIEND_CK].Msgfunc = &GLGaeaServer::Request2FriendCK;
	m_MsgFunc[NET_MSG_GCTRL_2_FRIEND_REQ].Msgfunc = &GLGaeaServer::Request2FriendREQ;
	m_MsgFunc[NET_MSG_GM_MOVE2CHAR_POS].Msgfunc = &GLGaeaServer::RequestMove2CharPos;
	m_MsgFunc[NET_MSG_GM_VIEWALLPLAYER_FLD_REQ].Msgfunc = &GLGaeaServer::RequestViewAllPlayer;
	m_MsgFunc[NET_MSG_GM_SHOP_INFO_REQ].Msgfunc = &GLGaeaServer::RequestShopInfo;
	m_MsgFunc[NET_MSG_GM_LAND].Msgfunc = &GLGaeaServer::MsgLand;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_ITEM].Msgfunc = &GLGaeaServer::ReqSearchShopItem;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ].Msgfunc = &GLGaeaServer::ReqSearchResultShopItem;

    m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_CF].Msgfunc = &GLGaeaServer::MsgReqInvenPMarketSearchOpenCF;
    m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_AF].Msgfunc = &GLGaeaServer::MsgReqInvenPMarketSearchOpenAF;

	m_MsgFunc[NET_MSG_GCTRL_MINIGAME].Msgfunc = &GLGaeaServer::MsgMiniGame;

	m_MsgFunc[NET_MSG_GCTRL_INSTANCE_SYSTEM].Msgfunc = &GLGaeaServer::MsgInstanceSystem;
    m_MsgFunc[NET_MSG_GCTRL_INSTANCE_CONTENTS].Msgfunc			= &GLGaeaServer::MsgInstanceSystemContents;
    m_MsgFunc[NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK].Msgfunc	= &GLGaeaServer::MsgInstanceSystemContentsMsgPack;
	m_MsgFunc[NET_MSG_FACTION_SYSTEM].Msgfunc = &GLGaeaServer::MsgFactionSystem;
	m_MsgFunc[NET_MSG_GCTRL_PING].Msgfunc = &GLGaeaServer::MsgPing;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_FNEW].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_ADD].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_DEL].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_DISSOLVE].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_AUTHORITY].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_MBR_MOVEMAP].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_MBR_RENAME].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_CHANGE_OPTION].Msgfunc = &GLGaeaServer::MsgParty;		
	m_MsgFunc[NET_MSG_GCTRL_PARTY_REFRESH_REQ].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_EXPEDITION_FNEW_FB].Msgfunc = &GLGaeaServer::MsgParty;
	m_MsgFunc[NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT].Msgfunc = &GLGaeaServer::MsgParty;

	m_MsgFunc[NET_MSG_GCTRL_PARTY_TENDER_ITEM].Msgfunc = &GLGaeaServer::MsgPartyTender;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_JUNK_ITEM].Msgfunc = &GLGaeaServer::MsgPartyTender;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_TRANSFER_ITEM].Msgfunc = &GLGaeaServer::MsgPartyTender;

	m_MsgFunc[NET_MSG_CLUB_RANK_CHANGE_AF].Msgfunc = &GLGaeaServer::RequestClubRank2Fld;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_INFO_2FLD].Msgfunc = &GLGaeaServer::RequestClubInfo;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD].Msgfunc = &GLGaeaServer::RequestClubDissolution2Fld;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEL_2FLD].Msgfunc = &GLGaeaServer::RequestClubDel2Fld;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD].Msgfunc = &GLGaeaServer::RequestClubAdd2Fld;
	m_MsgFunc[NET_MSG_CLUB_MEMBER_DEL_AF].Msgfunc = &GLGaeaServer::RequestClubDel;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD].Msgfunc = &GLGaeaServer::RequestClubMarkChange;
	//m_MsgFunc[NET_MSG_GCTRL_CLUB_SUBMASTER_FLD].Msgfunc = &GLGaeaServer::RequestClubSubMaster;
    m_MsgFunc[NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AF].Msgfunc = &GLGaeaServer::MsgClubCdCertifyChangeAF;
	m_MsgFunc[NET_MSG_CLUB_MASTER_CHANGE_AF].Msgfunc = &GLGaeaServer::RequestClubAuthority;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT_TAR].Msgfunc = &GLGaeaServer::RequestConfrontTarget;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_FLD].Msgfunc = &GLGaeaServer::RequestConfrontPartyChkMbr;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTPTY_START2_FLD].Msgfunc = &GLGaeaServer::RequestConfrontParty;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTPTY_END2_FLD].Msgfunc = &GLGaeaServer::RequestConfrontPartyEnd;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_FLD].Msgfunc = &GLGaeaServer::RequestConfrontClubChkMbr;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTCLB_START2_FLD].Msgfunc = &GLGaeaServer::RequestConfrontClub;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTCLB_END2_FLD].Msgfunc = &GLGaeaServer::RequestConfrontClubEnd;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD].Msgfunc = &GLGaeaServer::ServerClubDeathMatchReady;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD].Msgfunc = &GLGaeaServer::ServerClubDeathMatchStart;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD].Msgfunc = &GLGaeaServer::ServerClubDeathMatchEnd;
    
    //선도전
    m_MsgFunc[NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_FLD].Msgfunc = &GLGaeaServer::ServerClub_GuidBattleStart;
    m_MsgFunc[NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_FLD].Msgfunc = &GLGaeaServer::ServerClub_GuidBattleEnd;

	m_MsgFunc[NET_MSG_CLUB_CERTIFIED_AF].Msgfunc = &GLGaeaServer::ServerClubCertify;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_COMMISSION_FLD].Msgfunc = &GLGaeaServer::ServerClubGuidCommission;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_NOTICE_FLD].Msgfunc = &GLGaeaServer::ServerClubNotice;
	//m_MsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_GET_DB].Msgfunc = &GLGaeaServer::ServerClubStorageGetDB;
	//m_MsgFunc[NET_MSG_GCTRL_CLUB_INCOME_DN].Msgfunc = &GLGaeaServer::ServerClubInComeDn;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD].Msgfunc = &GLGaeaServer::ServerClubAllianceAddFld;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD].Msgfunc = &GLGaeaServer::ServerClubAllianceDel;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD].Msgfunc = &GLGaeaServer::ServerClubAllianceDis;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD].Msgfunc = &GLGaeaServer::ServerClubBattleBeginFld;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD].Msgfunc = &GLGaeaServer::ServerClubBattleOverFld;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD].Msgfunc = &GLGaeaServer::ServerClubBattleKillUpdate;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD].Msgfunc = &GLGaeaServer::ServerClubBattleLastKillUpdate;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD].Msgfunc = &GLGaeaServer::ReqClubMemberRename;
	m_MsgFunc[NET_MSG_GCTRL_LEVEL_EVENT_END_FLD].Msgfunc = &GLGaeaServer::ServerLevelEventEnd;
	m_MsgFunc[NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD].Msgfunc = &GLGaeaServer::ServerLevelEventWarning;
	m_MsgFunc[NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD].Msgfunc = &GLGaeaServer::ServerLevelEventCountdown;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GENITEM_FLD_FB].Msgfunc = &GLGaeaServer::RequestGenItemFieldFB;
	m_MsgFunc[NET_MSG_GCTRL_FIELDINFO_RESET].Msgfunc = &GLGaeaServer::ServerFieldInfoReset;
	m_MsgFunc[NET_MSG_GCTRL_SCHOOLFREEPK_FLD].Msgfunc = &GLGaeaServer::ServerSchoolFreePk;
	m_MsgFunc[NET_MSG_GCTRL_PERIOD].Msgfunc = &GLGaeaServer::ServerCtrlPeriod;
	m_MsgFunc[NET_MSG_SERVER_CTRL_WEATHER].Msgfunc = &GLGaeaServer::ServerCtrlWeather;
	m_MsgFunc[NET_MSG_SERVER_CTRL_WEATHER2].Msgfunc = &GLGaeaServer::ServerCtrlWeather2;
	m_MsgFunc[NET_MSG_SERVER_CTRL_TIME].Msgfunc = &GLGaeaServer::ServerCtrlTime;
	m_MsgFunc[NET_MSG_SERVER_CTRL_MONTH].Msgfunc = &GLGaeaServer::ServerCtrlMonth;
	m_MsgFunc[NET_MSG_SERVER_CTRL_GENITEMHOLD].Msgfunc = &GLGaeaServer::ServerCtrlGenItemHold;
	m_MsgFunc[NET_MSG_SERVER_PLAYERKILLING_MODE].Msgfunc = &GLGaeaServer::ServerCtrlPlayerKillingMode;
	m_MsgFunc[NET_MSG_CYBERCAFECLASS_UPDATE].Msgfunc = &GLGaeaServer::CyberCafeClassUpdate;
	m_MsgFunc[NET_MSG_GCTRL_DROP_OUT_FORCED].Msgfunc = &GLGaeaServer::ServerCtrlCharDropOutForced;	
	m_MsgFunc[NET_MSG_GM_PING].Msgfunc = &GLGaeaServer::ProcessPingMsg;
	m_MsgFunc[NET_MSG_GM_PING_FB].Msgfunc = &GLGaeaServer::ProcessPingMsg;
	m_MsgFunc[NET_MSG_POST_SEND].Msgfunc = &GLGaeaServer::MsgPostSend;
	m_MsgFunc[NET_MSG_POST_OPEN_TAKE_AF].Msgfunc = &GLGaeaServer::MsgPostOpenAndTakeAF;
	m_MsgFunc[NET_MSG_POST_TODO_AFTER_SEND_FF].Msgfunc = &GLGaeaServer::MsgPostToDoAfterSendFF;
	m_MsgFunc[NET_MSG_SNS_FB_UPDATE_AF].Msgfunc = &GLGaeaServer::MsgSNSFBUpdateAF;
	m_MsgFunc[NET_MSG_SNS_TW_UPDATE_AF].Msgfunc = &GLGaeaServer::MsgSNSTWUpdateAF;	
	m_MsgFunc[NET_MSG_ACTIVITY_DONE_ITEMTAKE].Msgfunc = &GLGaeaServer::MsgActivityDoneItemTake;
	m_MsgFunc[NET_MSG_ACTIVITY_CHANGE_BADGE].Msgfunc = &GLGaeaServer::MsgActivityChangeBadge;
	m_MsgFunc[NET_MSG_ACTIVITY_COMPLETED_FF].Msgfunc = &GLGaeaServer::MsgActivityCompletedFF;
	m_MsgFunc[NET_MSG_ACTIVITY_LOAD_COMPLETED_FF].Msgfunc = &GLGaeaServer::MsgActivityLoadCompletedFF;
	m_MsgFunc[NET_MSG_ATTENDANCE_SYNC_FF].Msgfunc = &GLGaeaServer::MsgAttendanceSyncFF;
	m_MsgFunc[NET_MSG_ATTENDANCE_RESET_FF].Msgfunc = &GLGaeaServer::MsgAttendanceResetFF;
	m_MsgFunc[NET_MSG_ATTENDANCE_UPGRADE_FF].Msgfunc = &GLGaeaServer::MsgAttendanceUpgradeFF;
	m_MsgFunc[NET_MSG_ATTENDANCE_DAILYMAINTENANCE_START_AF].Msgfunc = &GLGaeaServer::MsgAttendanceDailyMaintenanceStartAF;
	m_MsgFunc[NET_MSG_ATTENDANCE_DAILYMAINTENANCE_END_AF].Msgfunc = &GLGaeaServer::MsgAttendanceDailyMaintenanceEndAF;
	m_MsgFunc[NET_MSG_ATTENDANCE_TAKEREWARD_CF].Msgfunc = &GLGaeaServer::MsgAttendanceTakeRewardCF;
	m_MsgFunc[NET_MSG_ATTENDANCE_DONE_POINTITEM_CF].Msgfunc = &GLGaeaServer::MsgAttendanceDonePointItemCF;
	m_MsgFunc[NET_MSG_ATTENDANCE_WHISPER_DONE_AF].Msgfunc = &GLGaeaServer::MsgAttendanceWhisperDoneAF;	
	m_MsgFunc[NET_MSG_ATTENDANCE_LOAD_END].Msgfunc = &GLGaeaServer::MsgAttendanceLoadEnd;	


	m_MsgFunc[NET_MSG_CTF_READY_AF].Msgfunc					= &GLGaeaServer::MsgCTFReadyAF;
	m_MsgFunc[NET_MSG_CTF_PREPARE_AF].Msgfunc				= &GLGaeaServer::MsgCTFPrepareAF;
	m_MsgFunc[NET_MSG_CTF_START_AF].Msgfunc					= &GLGaeaServer::MsgCTFStartAF;
	m_MsgFunc[NET_MSG_CTF_JOIN_CF].Msgfunc					= &GLGaeaServer::MsgCTFJoinCF;
	m_MsgFunc[NET_MSG_CTF_RECALL_TO_AF].Msgfunc				= &GLGaeaServer::MsgCTFRecallToAF;	
	m_MsgFunc[NET_MSG_CTF_STOP_AF].Msgfunc					= &GLGaeaServer::MsgCTFStopAF;
	m_MsgFunc[NET_MSG_CTF_DONE_AF_AC].Msgfunc				= &GLGaeaServer::MsgCTFDoneAF;
    m_MsgFunc[NET_MSG_CTF_REWARD_INFO].Msgfunc				= &GLGaeaServer::MsgCTFRewardAF;
	m_MsgFunc[NET_MSG_CTF_SYNC_PLAYERNUM_AF].Msgfunc		= &GLGaeaServer::MsgCTFSyncPlayerNumAF;
	m_MsgFunc[NET_MSG_CTF_QUIT_CFA].Msgfunc					= &GLGaeaServer::MsgCTFQuitCF;
    m_MsgFunc[NET_MSG_CTF_QUIT_AF].Msgfunc					= &GLGaeaServer::MsgCTFQuitAF;
    //m_MsgFunc[NET_MSG_CTF_NEW_PLAYER_HAS_COME_AF].Msgfunc	= &GLGaeaServer::MsgCTFNewPlayerHasComeAF;
	m_MsgFunc[NET_MSG_CTF_NEXT_READY_TIME].Msgfunc			= &GLGaeaServer::MsgCTFNextReadyTimeAF;

	m_MsgFunc[NET_MSG_GM_PINGTRACE_ON].Msgfunc = &GLGaeaServer::ProcessPingTraceMsg;
	m_MsgFunc[NET_MSG_GM_PINGTRACE_OFF].Msgfunc = &GLGaeaServer::ProcessPingTraceMsg;
	m_MsgFunc[NET_MSG_GM_NONCONFRONT_MODE].Msgfunc = &GLGaeaServer::GMCtrlNonConfrontMode;
	m_MsgFunc[NET_MSG_GM_MOVE2MAPPOS_FLD].Msgfunc = &GLGaeaServer::GMCtrolMove2MapPos;
	m_MsgFunc[NET_MSG_GM_WHERE_NPC_FLD].Msgfunc = &GLGaeaServer::GMCtrlWhereNpc;
	m_MsgFunc[NET_MSG_GM_WHERE_NPC_ALL_FLD].Msgfunc = &GLGaeaServer::GMCtrlWhereNpcALL;
	m_MsgFunc[NET_MSG_GM_WARNING_MSG_FLD].Msgfunc = &GLGaeaServer::GMCtrlWarningMSG;
	m_MsgFunc[NET_MSG_GM_WHERE_PC_POS].Msgfunc = &GLGaeaServer::GMCtrolWherePcPos;
	m_MsgFunc[NET_MSG_GM_BIGHEAD].Msgfunc = &GLGaeaServer::GMCtrolBigHead;
	m_MsgFunc[NET_MSG_GM_BIGHAND].Msgfunc = &GLGaeaServer::GMCtrolBigHand;
	m_MsgFunc[NET_MSG_GM_MOB_GEN_FLD].Msgfunc = &GLGaeaServer::GMCtrolMobGen;
	m_MsgFunc[NET_MSG_GM_MOB_GEN_EX_FLD].Msgfunc = &GLGaeaServer::GMCtrolMobGenEx;
	m_MsgFunc[NET_MSG_GM_PRINT_CROWLIST_FLD].Msgfunc = &GLGaeaServer::GMCtrolMoPrintCrowList;
	m_MsgFunc[NET_MSG_GM_MOB_DEL_EX_FLD].Msgfunc = &GLGaeaServer::GMCtrolMobDelEx;
	m_MsgFunc[NET_MSG_GM_MOB_DEL_FLD].Msgfunc = &GLGaeaServer::GMCtrolMobDel;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL].Msgfunc = &GLGaeaServer::GMCtrolMobLevel;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL_LAYER].Msgfunc = &GLGaeaServer::GMCtrolMobLevelLayer;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL_CLEAR].Msgfunc = &GLGaeaServer::GMCtrolMobLevelClear;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL_LIST].Msgfunc = &GLGaeaServer::GMCtrolMobLevelList;
	m_MsgFunc[NET_MSG_GM_CHAT_BLOCK_FLD].Msgfunc = &GLGaeaServer::GMCtrolChatBlockFld;
	m_MsgFunc[NET_MSG_USER_CHAR_INFO_FLD].Msgfunc = &GLGaeaServer::UserCtrolCharInfoFld;
	m_MsgFunc[NET_MSG_GM_CHAR_INFO_FLD].Msgfunc = &GLGaeaServer::GMCtrolCharInfoFld;
	m_MsgFunc[NET_MSG_GM_FREEPK].Msgfunc = &GLGaeaServer::GMCtrolFreePK;
	m_MsgFunc[NET_MSG_GM_MOVE2GATE_FLD].Msgfunc = &GLGaeaServer::GMCtrolMove2Gate;
	m_MsgFunc[NET_MSG_GCTRL_SERVERSTOP].Msgfunc = &GLGaeaServer::ReserveServerStop;
	m_MsgFunc[NET_MSG_GM_KICK_USER_PROC_FLD].Msgfunc = &GLGaeaServer::GMKicUser;
    m_MsgFunc[NET_MSG_GM_SHOWMETHEMONEY_FLD].Msgfunc = &GLGaeaServer::GMCtrolShowMeTheMoney;
    m_MsgFunc[NET_MSG_GM_SHOWMETHEMONEY_EMULATOR_CF].Msgfunc = &GLGaeaServer::GmShowMoTheMoneyEmulatorCF;
    m_MsgFunc[NET_MSG_GM_WHYSOSERIOUS_FLD].Msgfunc = &GLGaeaServer::GMCtrolWhySoSerious;
    m_MsgFunc[NET_MSG_GM_WHYSOSERIOUS_EMULATOR_CF].Msgfunc = &GLGaeaServer::GmWhySoSeriousEmulatorCF;
    m_MsgFunc[NET_MSG_GM_LOG_TO_DB_CAF].Msgfunc = &GLGaeaServer::MsgLogToDB;

	m_MsgFunc[NET_MSG_VIETNAM_ALLINITTIME].Msgfunc = &GLGaeaServer::VietnamAllInitTime;

	m_MsgFunc[NET_MSG_GM_EVENT_EXP_CAF].Msgfunc = &GLGaeaServer::MsgEventExp;
	m_MsgFunc[NET_MSG_GM_EVENT_EXP_END_CAF].Msgfunc = &GLGaeaServer::MsgEventExpEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_ITEM_GEN].Msgfunc = &GLGaeaServer::MsgEventItemGen;
	m_MsgFunc[NET_MSG_GM_EVENT_ITEM_GEN_END].Msgfunc = &GLGaeaServer::MsgEventItemGenEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_GRADE].Msgfunc = &GLGaeaServer::MsgEventGrade;
	m_MsgFunc[NET_MSG_GM_EVENT_GRADE_END].Msgfunc = &GLGaeaServer::MsgEventGradeEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_MONEY_GEN].Msgfunc = &GLGaeaServer::MsgEventMoneyGen;
	m_MsgFunc[NET_MSG_GM_EVENT_MONEY_GEN_END].Msgfunc = &GLGaeaServer::MsgEventMoneyGenEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_EX].Msgfunc = &GLGaeaServer::MsgEventEx;
	m_MsgFunc[NET_MSG_GM_EVENT_EX_END].Msgfunc = &GLGaeaServer::MsgEventExEnd;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_BEGIN].Msgfunc = &GLGaeaServer::MsgLimitEventBegin;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_TIME_RESET].Msgfunc = &GLGaeaServer::MsgLimitEventTimeReset;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_END].Msgfunc = &GLGaeaServer::MsgLimitEventEnd;
	m_MsgFunc[NET_MSG_GM_CLASS_EVENT].Msgfunc = &GLGaeaServer::MsgClassEvent;
	m_MsgFunc[NET_MSG_CHAR_ACTIVE_VEHICLE_CF].Msgfunc = &GLGaeaServer::ReqActiveVehicle;	
	m_MsgFunc[NET_MSG_GCTRL_GET_VEHICLE].Msgfunc = &GLGaeaServer::ReqGetVehicle;
	m_MsgFunc[NET_MSG_VEHICLE_CREATE_FROMDB_FB].Msgfunc = &GLGaeaServer::CreateVehicleOnDB;
	m_MsgFunc[NET_MSG_VEHICLE_GET_FROMDB_FB].Msgfunc = &GLGaeaServer::GetVehicleInfoFromDB;
	m_MsgFunc[NET_MSG_VEHICLE_GET_FROMDB_ERROR].Msgfunc = &GLGaeaServer::GetVehicleInfoFromDBError;
	m_MsgFunc[NET_MSG_VEHICLE_REQ_ITEM_INFO].Msgfunc = &GLGaeaServer::GetVehicleItemInfo;
	m_MsgFunc[NET_MSG_GCTRL_WITH_VEHICLE].Msgfunc = &GLGaeaServer::WithVehicle;
	m_MsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS].Msgfunc = &GLGaeaServer::WithVehicleTarAns;
	m_MsgFunc[NET_QBOX_OPTION_REQ_FLD].Msgfunc = &GLGaeaServer::ReqQBoxEnable;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ].Msgfunc = &GLGaeaServer::ReqClubDeathMatchRanking;
	m_MsgFunc[NET_MSG_PET_REQ_USECARD].Msgfunc = &GLGaeaServer::RequestUsePETCARD;
	m_MsgFunc[NET_MSG_PET_REQ_UNUSECARD].Msgfunc = &GLGaeaServer::RequestUnUsePETCARD;
	m_MsgFunc[NET_MSG_PET_REQ_REVIVE].Msgfunc = &GLGaeaServer::RequestRevivePet;
	m_MsgFunc[NET_MSG_PET_REQ_REVIVE_FROMDB_FB].Msgfunc = &GLGaeaServer::RevivePet;
	m_MsgFunc[NET_MSG_CREATE_PET_FROMDB_FB].Msgfunc = &GLGaeaServer::CreatePETOnDB;
	m_MsgFunc[NET_MSG_GET_PET_FROMDB_FB].Msgfunc = &GLGaeaServer::GetPETInfoFromDB;
	m_MsgFunc[NET_MSG_GET_PET_FROMDB_ERROR].Msgfunc = &GLGaeaServer::GetPETInfoFromDBError;
	m_MsgFunc[NET_MSG_PET_REQ_PETCARDINFO].Msgfunc = &GLGaeaServer::MsgPETInfoCardReq;
	m_MsgFunc[NET_MSG_PET_REQ_GOTO].Msgfunc = &GLGaeaServer::MsgPetGoto;
	m_MsgFunc[NET_MSG_PET_REQ_STOP].Msgfunc = &GLGaeaServer::MsgPetStop;
	m_MsgFunc[NET_MSG_PET_REQ_UPDATE_MOVE_STATE].Msgfunc = &GLGaeaServer::MsgPetUpdateMoveState;
	m_MsgFunc[NET_MSG_PET_REQ_SKILL_CHANGE_CF].Msgfunc = &GLGaeaServer::MsgPetChangeActiveSkill;
	m_MsgFunc[NET_MSG_PET_REQ_RENAME].Msgfunc = &GLGaeaServer::MsgPetRename;
	m_MsgFunc[NET_MSG_PET_REQ_RENAME_FROMDB_FB].Msgfunc = &GLGaeaServer::PetRenameFeedBack;
	m_MsgFunc[NET_MSG_PET_REQ_CHANGE_COLOR].Msgfunc = &GLGaeaServer::MsgPetChangeColor;
	m_MsgFunc[NET_MSG_PET_REQ_CHANGE_STYLE].Msgfunc = &GLGaeaServer::MsgPetChangeStyle;
	m_MsgFunc[NET_MSG_PET_REQ_INVEN_EX_SLOT].Msgfunc = &GLGaeaServer::MsgPetAccInvenExSlot;
	m_MsgFunc[NET_MSG_PET_REQ_INVEN_TO_SLOT].Msgfunc = &GLGaeaServer::MsgPetAccInvenToSlot;
	m_MsgFunc[NET_MSG_PET_REQ_SLOT_TO_INVEN].Msgfunc = &GLGaeaServer::MsgPetAccSlotToInven;
	m_MsgFunc[NET_MSG_PET_REQ_LEARNSKILL].Msgfunc = &GLGaeaServer::MsgPetLearnSkill;
	m_MsgFunc[NET_MSG_PET_REQ_SKILLSLOT_EXPANSION].Msgfunc = &GLGaeaServer::MsgPetSkillSlotExpansion;
	m_MsgFunc[NET_MSG_PET_REQ_FUNNY].Msgfunc = &GLGaeaServer::MsgPetFunnyReq;
	m_MsgFunc[NET_MSG_PET_REMOVE_SLOTITEM].Msgfunc = &GLGaeaServer::MsgPetRemoveSlotItem;
	m_MsgFunc[NET_MSG_PET_PETSKINPACKOPEN].Msgfunc = &GLGaeaServer::MsgPetSkinPackItem;
	m_MsgFunc[NET_MSG_REQ_USE_SUMMON].Msgfunc = &GLGaeaServer::MsgReqSummonProc;	
	m_MsgFunc[NET_MSG_REQ_USE_SUMMON_EVENT_SKILL].Msgfunc = &GLGaeaServer::MsgReqSummonEventSkillProc;
	m_MsgFunc[NET_MSG_HIRE_BASICSUMMON_CF].Msgfunc = &GLGaeaServer::MsgHireBasicSummonProc;
	m_MsgFunc[NET_MSG_GCTRL_REQ_READY].Msgfunc = &GLGaeaServer::MsgReady;					
	m_MsgFunc[NET_MSG_GCTRL_ACTSTATE].Msgfunc = &GLGaeaServer::MsgActState;				
	m_MsgFunc[NET_MSG_GCTRL_MOVESTATE].Msgfunc = &GLGaeaServer::MsgMoveState;				
	m_MsgFunc[NET_MSG_GCTRL_GOTO].Msgfunc = &GLGaeaServer::MsgGoto;					
	m_MsgFunc[NET_MSG_GCTRL_DOMINATE_MOVETO_CF].Msgfunc = &GLGaeaServer::MsgDominateMoveTo;
	m_MsgFunc[NET_MSG_GCTRL_ANIMOVE_START_CF].Msgfunc = &GLGaeaServer::MsgAniMoveStartCF;
	m_MsgFunc[NET_MSG_GCTRL_ANIMOVE_END_CF].Msgfunc = &GLGaeaServer::MsgAniMoveEndCF;
	m_MsgFunc[NET_MSG_GCTRL_ATTACK].Msgfunc = &GLGaeaServer::MsgAttack;					
	m_MsgFunc[NET_MSG_GCTRL_ATTACK_CANCEL].Msgfunc = &GLGaeaServer::MsgAttackCancel;	
	m_MsgFunc[NET_MSG_GCTRL_SELECT_TARGET].Msgfunc = &GLGaeaServer::MsgSetTarget;
	m_MsgFunc[NET_MSG_GCTRL_REQ_FIELD_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqFieldToInven;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_FIELD_TO_HOLD].Msgfunc = &GLGaeaServer::MsgReqFieldToHold;
    m_MsgFunc[NET_MSG_GCTRL_REQ_HOLD_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqHoldToInven; 		
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_TO_HOLD].Msgfunc = &GLGaeaServer::MsgReqInvenToHold;
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_EX_INVEN].Msgfunc = &GLGaeaServer::MsgReqInvenExInven;
    m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGE_EX_INVEN].Msgfunc = &GLGaeaServer::MsgReqStorageExInven;
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_TO_SLOT].Msgfunc = &GLGaeaServer::MsgReqInvenToSlot;
    m_MsgFunc[NET_MSG_GCTRL_REQ_SLOT_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqSlotToInven;
	m_MsgFunc[NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD].Msgfunc = &GLGaeaServer::MsgReqVNGainToHold;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD].Msgfunc = &GLGaeaServer::MsgReqVNGainExHold;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN].Msgfunc = &GLGaeaServer::MsgReqHoldToVNGain;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqVNInvenToInven;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET].Msgfunc = &GLGaeaServer::MsgReqVNGainInvenReset;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_SLOT_TO_HOLD].Msgfunc = &GLGaeaServer::MsgReqSlotToHold;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT].Msgfunc = &GLGaeaServer::MsgReqItemSort;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_START].Msgfunc = &GLGaeaServer::MsgReqItemSortStart;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_DATA].Msgfunc = &GLGaeaServer::MsgReqItemSortData;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_END].Msgfunc = &GLGaeaServer::MsgReqItemSortEnd;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_SPLIT].Msgfunc = &GLGaeaServer::MsgReqInvenSplit;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_TO_FIELD].Msgfunc = &GLGaeaServer::MsgReqInvenToField;
    m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqHoldToInven;
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqInvenToInven;
    m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGE_TO_INVEN].Msgfunc = &GLGaeaServer::MsgReqStorageToInven;
	m_MsgFunc[NET_MSG_GCTRL_REQ_HOLD_TO_SLOT].Msgfunc = &GLGaeaServer::MsgReqHoldToSlot;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_SLOT_CHANGE].Msgfunc = &GLGaeaServer::MsgReqSlotChange;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_BUY_FROM_NPC].Msgfunc = &GLGaeaServer::MsgReqBuyFromNpc;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_SALE_TO_NPC].Msgfunc = &GLGaeaServer::MsgReqSaleToNpc;			
    m_MsgFunc[NET_MSG_GCTRL_REQ_RESURRECT].Msgfunc = &GLGaeaServer::MsgReqResurrect;
	m_MsgFunc[NET_MSG_GCTRL_RESURRECT_FAFC].Msgfunc = &GLGaeaServer::MsgReqReBirth;
	m_MsgFunc[NET_MSG_RESURRECT_MOVEMAP_AF].Msgfunc = &GLGaeaServer::MsgResurrectMoveMap;
	m_MsgFunc[NET_MSG_GCTRL_REQ_JUMPINGLEVELUP].Msgfunc = &GLGaeaServer::MsgReqJumpingLevelUp;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_CA].Msgfunc = &GLGaeaServer::MsgReqJumpingLevelUpCA;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_JUMPING_LEVLEUP_BYEXP].Msgfunc = &GLGaeaServer::MsgReqJumpingLevelUpByExp;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_LEVELUP].Msgfunc = &GLGaeaServer::MsgReqLevelUp;				
	m_MsgFunc[NET_MSG_GCTRL_REQ_STATSUP].Msgfunc = &GLGaeaServer::MsgReqStatsUp;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_STATSUP_EX].Msgfunc = &GLGaeaServer::MsgReqStatsUpEx;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_LEARNSKILL].Msgfunc = &GLGaeaServer::MsgReqLearnSkill;
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILLUP].Msgfunc = &GLGaeaServer::MsgReqSkillUp;				
    m_MsgFunc[NET_MSG_GCTRL_REQ_RNSKILLUP].Msgfunc = &GLGaeaServer::MsgReqRnSkillUp;				
	m_MsgFunc[NET_MSG_GCTRL_FIRECRACKER].Msgfunc = &GLGaeaServer::MsgReqFireCracker;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVENDRUG].Msgfunc = &GLGaeaServer::MsgReqInvenDrug;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX].Msgfunc = &GLGaeaServer::MsgReqInvenLunchBox;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_BOXOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenBoxOpen;		
	m_MsgFunc[NET_MSG_GET_CHARGED_ITEM_CF].Msgfunc = &GLGaeaServer::MsgReqInvenBoxInfo;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_DISGUISE].Msgfunc = &GLGaeaServer::MsgReqInvenDisguise;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_CLEANSER].Msgfunc = &GLGaeaServer::MsgReqInvenCleanser;		
	m_MsgFunc[NET_MSG_GCTRL_CHARGED_ITEM_GET].Msgfunc = &GLGaeaServer::MsgReqInvenChargedItem;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenRandomBoxOpen;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenSelformBoxSelectOpen;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_DISJUNCTION].Msgfunc = &GLGaeaServer::MsgReqInvenDisJunction;	
    m_MsgFunc[NET_MSG_GCTRL_INVEN_BOX_WRAPPING].Msgfunc = &GLGaeaServer::MsgReqInvenBoxWrapping;
    m_MsgFunc[NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING].Msgfunc = &GLGaeaServer::MsgReqInvenBoxUnwrapping;
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILL].Msgfunc = &GLGaeaServer::MsgReqSkill;				
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILLHOLD_RS].Msgfunc = &GLGaeaServer::MsgReqSkillHoldReset;		
	m_MsgFunc[NET_MSG_GCTRL_SKILL_CANCEL].Msgfunc = &GLGaeaServer::MsgReqSkillCancel;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE].Msgfunc = &GLGaeaServer::MsgReqSkillQSetActive;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILLQ_SET].Msgfunc = &GLGaeaServer::MsgReqSkillQSet;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILLQ_RESET].Msgfunc = &GLGaeaServer::MsgReqSkillQReSet;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_ACTIONQ_SET].Msgfunc = &GLGaeaServer::MsgReqActionQSet;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_ACTIONQ_RESET].Msgfunc = &GLGaeaServer::MsgReqActionQReSet;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM].Msgfunc = &GLGaeaServer::MsgReqGetStorageSpecificItem;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE].Msgfunc = &GLGaeaServer::MsgReqGetStorage;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGEDRUG].Msgfunc = &GLGaeaServer::MsgReqStorageDrug;			
	m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGESKILL].Msgfunc = &GLGaeaServer::MsgReqStorageSkill;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD].Msgfunc = &GLGaeaServer::MsgReqStorageToHold;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_EX_STORAGE].Msgfunc = &GLGaeaServer::MsgReqInvenExStorage;
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE].Msgfunc = &GLGaeaServer::MsgReqInvenToStorage;
    m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGE_TO_STORAGE].Msgfunc = &GLGaeaServer::MsgReqStorageToStorage;
	m_MsgFunc[NET_MSG_GCTRL_STORAGE_SAVE_MONEY].Msgfunc = &GLGaeaServer::MsgReqStorageSaveMoney;	
	m_MsgFunc[NET_MSG_GCTRL_STORAGE_DRAW_MONEY].Msgfunc = &GLGaeaServer::MsgReqStorageDrawMoney;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_STORAGE_SPLIT].Msgfunc = &GLGaeaServer::MsgReqStorageSplit;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_MONEY_TO_FIELD].Msgfunc = &GLGaeaServer::MsgReqMoneyToField;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_GRINDING].Msgfunc = &GLGaeaServer::MsgReqInvenGrinding;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_PERIODEXTEND].Msgfunc = &GLGaeaServer::MsgReqInvenPeriodExtend;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_LOCKBOX_UNLOCK].Msgfunc = &GLGaeaServer::MsgReqInvenLockBoxUnLock;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_LOCKBOXOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenLockBoxOpen;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_RESET_SKST].Msgfunc = &GLGaeaServer::MsgReqInvenResetSkSt;		
	m_MsgFunc[NET_MSG_GCTRL_REGEN_GATE].Msgfunc = &GLGaeaServer::MsgReqReGenGate;			
	m_MsgFunc[NET_MSG_GCTRL_CURE].Msgfunc = &GLGaeaServer::MsgReqCure;
	m_MsgFunc[NET_MSG_GCTRL_2_FRIEND_FLD].Msgfunc = &GLGaeaServer::MsgReqFriendFld;			
	m_MsgFunc[NET_MSG_GM_MOVE2CHAR_FLD].Msgfunc = &GLGaeaServer::MsgReqMove2CharFld;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_SKILL_REVIVE].Msgfunc = &GLGaeaServer::MsgReqSkillRevivel;
	m_MsgFunc[NET_MSG_GCTRL_GETEXP_RECOVERY_NPC].Msgfunc = &GLGaeaServer::MsgReqGetExpRecoveryNpc;	
	m_MsgFunc[NET_MSG_GCTRL_RECOVERY_NPC].Msgfunc = &GLGaeaServer::MsgReqRecoveryNpc;			
	m_MsgFunc[NET_MSG_GCTRL_CHARRESET].Msgfunc = &GLGaeaServer::MsgReqCharReset;			
	m_MsgFunc[NET_MSG_GCTRL_INVEN_CHARCARD].Msgfunc = &GLGaeaServer::MsgReqCharCard;			
	m_MsgFunc[NET_MSG_GCTRL_INVEN_STORAGECARD].Msgfunc = &GLGaeaServer::MsgReqStorageCard;			
	m_MsgFunc[NET_MSG_GCTRL_INVEN_INVENLINE].Msgfunc = &GLGaeaServer::MsgReqInvenLine;			
	m_MsgFunc[NET_MSG_GCTRL_INVEN_STORAGEOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenStorageOpen;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_REMODELOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenRemodelOpen;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_GARBAGEOPEN].Msgfunc = &GLGaeaServer::MsgReqInvenGarbageOpen;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_STORAGECLOSE].Msgfunc = &GLGaeaServer::MsgReqInvenStorageClose;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_PREMIUMSET].Msgfunc = &GLGaeaServer::MsgReqInvenPremiumSet;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_HAIR_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenHairChange;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_FACE_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenFaceChange;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenHairStyleChange;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenHairColorChange;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenFaceStyleChange;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_GENDER_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenGenderChange;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenSchoolChange;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_RENAME].Msgfunc = &GLGaeaServer::MsgReqInvenRename;			
	m_MsgFunc[NET_MSG_GCTRL_INVEN_RENAME_FROM_DB].Msgfunc = &GLGaeaServer::MsgInvenRename;
	m_MsgFunc[NET_MSG_GCTRL_INITIALIZE_COUNTRY_CF].Msgfunc = &GLGaeaServer::MsgInvenInitializeCountry;
	m_MsgFunc[NET_MSG_GCTRL_POSTBOX_OPEN].Msgfunc = &GLGaeaServer::MsgReqPostBoxOpen;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD].Msgfunc = &GLGaeaServer::MsgReqPostBoxOpenCard;	
	m_MsgFunc[NET_MSG_GCTRL_POSTBOX_CLOSE].Msgfunc = &GLGaeaServer::MsgReqPostBoxClose;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET].Msgfunc = &GLGaeaServer::MsgInvenVietnamGet;		
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT_START2_FLD].Msgfunc = &GLGaeaServer::MsgReqConFrontStart;		
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT_END2_FLD].Msgfunc = &GLGaeaServer::MsgReqConFrontEnd;			
	m_MsgFunc[NET_MSG_GCTRL_NPC_ITEM_TRADE].Msgfunc = &GLGaeaServer::MsgReqNpcItemTrade;
	m_MsgFunc[NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN].Msgfunc = &GLGaeaServer::MsgReqDialogueWindowOpen;
	m_MsgFunc[NET_MSG_GCTRL_REQ_QUEST_START].Msgfunc = &GLGaeaServer::MsgReqNpcQuestStart;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_QUEST_START_WITHOUT_NPC].Msgfunc = &GLGaeaServer::MsgReqNpcQuestStartWithoutNPC;		
	m_MsgFunc[NET_MSG_GCTRL_QUEST_PROG_NPCTALK].Msgfunc = &GLGaeaServer::MsgReqNpcQuestTalk;		
	m_MsgFunc[NET_MSG_GCTRL_QUEST_PROG_GIVEUP].Msgfunc = &GLGaeaServer::MsgReqQuestGiveUp;			
	m_MsgFunc[NET_MSG_GCTRL_QUEST_PROG_READ].Msgfunc = &GLGaeaServer::MsgReqQuestReadReset;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_QUEST_COMPLETE].Msgfunc = &GLGaeaServer::MsgReqQuestComplete;		
	m_MsgFunc[NET_MSG_GCTRL_REQ_GESTURE].Msgfunc = &GLGaeaServer::MsgReqGesture;			
	m_MsgFunc[NET_MSG_CHAT_LOUDSPEAKER].Msgfunc = &GLGaeaServer::MsgLoudSpeaker;			
	m_MsgFunc[NET_MSG_CHAT_PARTY_RECRUIT_FB_AF].Msgfunc = &GLGaeaServer::MsgChatPartyRecruitFB;		
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_TITLE].Msgfunc = &GLGaeaServer::MsgPMarketTitle;			
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_REGITEM].Msgfunc = &GLGaeaServer::MsgPMarketReqItem;			
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_DISITEM].Msgfunc = &GLGaeaServer::MsgPMarketDisItem;			
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_OPEN].Msgfunc = &GLGaeaServer::MsgPMarketOpen;			
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_CLOSE].Msgfunc = &GLGaeaServer::MsgPMarketClose;			
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_INFO].Msgfunc = &GLGaeaServer::MsgPMarketItemInfo;		
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_BUY].Msgfunc = &GLGaeaServer::MsgPMarketBuy;				
	m_MsgFunc[NET_MSG_GCTRL_CLUB_NEW].Msgfunc = &GLGaeaServer::MsgClubNew;				
	m_MsgFunc[NET_MSG_CLUB_RANK_UP_CF].Msgfunc = &GLGaeaServer::MsgClubRank;				
	m_MsgFunc[NET_MSG_GCTRL_CLUB_NEW_2FLD].Msgfunc = &GLGaeaServer::MsgClubNew2FLD;			
	//m_MsgFunc[NET_MSG_CLUB_MEMBER_REQ_CA].Msgfunc = &GLGaeaServer::MsgClubMemberReq;			
	//m_MsgFunc[NET_MSG_CLUB_MEMBER_REQ_ANS_CA].Msgfunc = &GLGaeaServer::MsgClubMemberAns;			
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_NICK].Msgfunc = &GLGaeaServer::MsgClubMemberNick;			
	m_MsgFunc[NET_MSG_GCTRL_CLUB_CD_CERTIFY].Msgfunc = &GLGaeaServer::MsgClubCDCertify;			
	//m_MsgFunc[NET_MSG_CLUB_GETSTORAGE_CA].Msgfunc = &GLGaeaServer::MsgReqClubGetStorage;		
	m_MsgFunc[NET_MSG_CLUB_STORAGE_TO_INVEN_AF].Msgfunc = &GLGaeaServer::MsgReqClubStorageToInven;	
	m_MsgFunc[NET_MSG_CLUB_STORAGE_EX_INVEN_AF].Msgfunc = &GLGaeaServer::MsgReqClubStorageExInven;	
	m_MsgFunc[NET_MSG_CLUB_INVEN_TO_STORAGE_CF].Msgfunc = &GLGaeaServer::MsgReqClubInvenToStorage;	
    m_MsgFunc[NET_MSG_CLUB_HOLD_TO_STORAGE_AF].Msgfunc = &GLGaeaServer::MsgReqClubHoldToStorageAF;	
	//m_MsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_SPLIT].Msgfunc = &GLGaeaServer::MsgReqClubStorageSplit;	
	m_MsgFunc[NET_MSG_CLUB_STORAGE_SAVE_MONEY_CF].Msgfunc = &GLGaeaServer::MsgReqClubStorageSaveMoney;
    m_MsgFunc[NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF].Msgfunc = &GLGaeaServer::MsgReqClubStorageSaveMoneyAF;
	m_MsgFunc[NET_MSG_CLUB_STORAGE_DRAW_MONEY_AF].Msgfunc = &GLGaeaServer::MsgReqClubStorageDrawMoney;
	//m_MsgFunc[NET_MSG_CLUB_INCOME_RENEW_CA].Msgfunc = &GLGaeaServer::MsgReqClubInComeReNew;		
	m_MsgFunc[NET_MSG_REBUILD_RESULT].Msgfunc = &GLGaeaServer::MsgReqRebuildResult;			// ITEMREBUILD_MARK
	m_MsgFunc[NET_MSG_REBUILD_MOVE_ITEM].Msgfunc = &GLGaeaServer::MsgReqRebuildMoveItem;		
	m_MsgFunc[NET_MSG_REBUILD_INPUT_MONEY].Msgfunc = &GLGaeaServer::MsgReqRebuildInputMoney;	
	m_MsgFunc[NET_MSG_GCTRL_GARBAGE_RESULT].Msgfunc = &GLGaeaServer::MsgReqGarbageResult;		
	//m_MsgFunc[NET_MSG_SMS_PHONE_NUMBER].Msgfunc = &GLGaeaServer::MsgReqPhoneNumber;			
	//m_MsgFunc[NET_MSG_SMS_PHONE_NUMBER_FROM_DB].Msgfunc = &GLGaeaServer::MsgPhoneNumber;			
	m_MsgFunc[NET_MSG_SMS_SEND].Msgfunc = &GLGaeaServer::MsgReqSendSMS;				
	m_MsgFunc[NET_MSG_SMS_SEND_FROM_DB].Msgfunc = &GLGaeaServer::MsgSendSMS;
	m_MsgFunc[NET_MSG_GCTRL_ITEMSHOPOPEN].Msgfunc = &GLGaeaServer::MsgItemShopOpen ;		
	m_MsgFunc[NET_MSG_GCTRL_INVEN_ITEM_MIX].Msgfunc = &GLGaeaServer::MsgReqItemMix;

	m_MsgFunc[NET_MSG_GCTRL_REQ_NPCTALK_BASIC].Msgfunc = &GLGaeaServer::MsgReqNpcTalkBasic;		
	
    // PET
	m_MsgFunc[NET_MSG_PET_GETRIGHTOFITEM].Msgfunc        = &GLGaeaServer::MsgReqGetRightOfItem ;	
	m_MsgFunc[NET_MSG_PET_REQ_GIVEFOOD].Msgfunc          = &GLGaeaServer::MsgGiveFood ;			
	m_MsgFunc[NET_MSG_PET_REQ_GETFULL_FROMDB_FB].Msgfunc = &GLGaeaServer::MsgGetPetFullFromDB ;    
	m_MsgFunc[NET_MSG_PET_REQ_PETREVIVEINFO].Msgfunc     = &GLGaeaServer::MsgPetReviveInfo ;       
	
    // 코스튬 염색. by luxes.
	m_MsgFunc[NET_MSG_GCTRL_COSTUM_COLOR_CHANGE].Msgfunc = &GLGaeaServer::MsgReqInvenCostumColorChange;	

	// vehicle
	m_MsgFunc[NET_MSG_VEHICLE_REQ_INVEN_EX_SLOT].Msgfunc          = &GLGaeaServer::MsgVehicleInvenExSlot ;	
	m_MsgFunc[NET_MSG_VEHICLE_REQ_INVEN_TO_SLOT].Msgfunc          = &GLGaeaServer::MsgVehicleInvenToSlot ;	
	m_MsgFunc[NET_MSG_VEHICLE_REQ_SLOT_TO_INVEN].Msgfunc          = &GLGaeaServer::MsgVehicleSlotToInven ;	
	m_MsgFunc[NET_MSG_VEHICLE_REMOVE_SLOTITEM].Msgfunc           = &GLGaeaServer::MsgVehicleRemoveSlot ;	
	m_MsgFunc[NET_MSG_VEHICLE_REQ_GIVE_BATTERY].Msgfunc          = &GLGaeaServer::MsgVehicleGiveBattery ;	
	m_MsgFunc[NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB].Msgfunc = &GLGaeaServer::MsgGetVehicleFullFromDB ;	
	m_MsgFunc[NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE].Msgfunc           = &GLGaeaServer::MsgReqInvenVehicleColorChange;			
	m_MsgFunc[NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE].Msgfunc        = &GLGaeaServer::MsgReqVehicleGetOff;	
	m_MsgFunc[NET_MSG_GCTRL_LEARN_BIKE_BOOST].Msgfunc            = &GLGaeaServer::MsgReqLearnBikeBoost;	
	m_MsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST].Msgfunc              = &GLGaeaServer::MsgReqUseBikeBoost;		
	
    // vietNam GainType System
	m_MsgFunc[NET_MSG_VIETNAM_TIME_REQ].Msgfunc = &GLGaeaServer::MsgReqVietnamGainType;

	m_MsgFunc[NET_MSG_REQ_GATHERING].Msgfunc               = &GLGaeaServer::MsgGathering;	
	m_MsgFunc[NET_MSG_REQ_GATHERING_CANCEL].Msgfunc        = &GLGaeaServer::MsgGatheringCancel;

    // PointShop
    m_MsgFunc[NET_MSG_GM_CHAR_POINT_ADD_CF].Msgfunc = &GLGaeaServer::MsgPointShopGmCharPointAddCF;
    m_MsgFunc[NET_MSG_PS_BUY_ITEM_CF].Msgfunc       = &GLGaeaServer::MsgPointShopBuyItemCF;
    m_MsgFunc[NET_MSG_PS_POINT_CARD_CF].Msgfunc     = &GLGaeaServer::MsgPointShopPointCardCF;

    // 재매입
    m_MsgFunc[NET_MSG_BUY_REBUY_ITEM_AF].Msgfunc = &GLGaeaServer::MsgBuyRebuyItem;

    m_MsgFunc[NET_MSG_CLUB_AUTH_AF].Msgfunc = &GLGaeaServer::MsgClubAuthAF;

	//! ReferChar
	m_MsgFunc[NET_MSG_REFER_CHAR_PUBLIC].Msgfunc = &GLGaeaServer::MsgReferCharPublic;
	m_MsgFunc[NET_MSG_REFER_CHAR_REQ].Msgfunc = &GLGaeaServer::MsgReferCharRequest;

	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF].Msgfunc = &GLGaeaServer::MsgMapSearchNameDetailInfo;
	
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_POSITION_REQ_AF].Msgfunc = &GLGaeaServer::MsgMapSearchNamePosition;
	
    m_MsgFunc[NET_MSG_GM_DB_UNLOCK_USER_AF].Msgfunc = &GLGaeaServer::GmDbUnlockUserAF;
	
    m_MsgFunc[NET_MSG_CLUB_GRADE_FLAG_CHANGE_AF].Msgfunc   = &GLGaeaServer::MsgClubGradeFlagChangeAF;
    m_MsgFunc[NET_MSG_CLUB_PULIC_FLAG_CHANGE_AF].Msgfunc   = &GLGaeaServer::MsgClubPublicFlagChangeAF;
    m_MsgFunc[NET_MSG_CLUB_AUTO_KICK_AF].Msgfunc           = &GLGaeaServer::MsgClubAutoKickAF;
    m_MsgFunc[NET_MSG_CLUB_NEWBIE_NOTICE_AF].Msgfunc       = &GLGaeaServer::MsgClubNewbieNoticeAF;
    m_MsgFunc[NET_MSG_CLUB_JOIN_TIME_RESET_AF].Msgfunc     = &GLGaeaServer::MsgClubJoinTimeResetAF;
    m_MsgFunc[NET_MSG_CLUB_ALLIANCE_TIME_RESET_AF].Msgfunc = &GLGaeaServer::MsgClubAllianceTimeResetAF;

	m_MsgFunc[NET_MSG_CHA_SAVE_DF].Msgfunc = &GLGaeaServer::MsgChaSaveDF;

	m_MsgFunc[NET_MSG_GM_MSG_DELAY_MODIFY].Msgfunc = &GLGaeaServer::MsgGMNetMsgDelay;

    //! Product ( 제조 )
    m_MsgFunc[NET_MSG_PRODUCT_REQ].Msgfunc = &GLGaeaServer::MsgProductReqAF;
    m_MsgFunc[NET_MSG_REQ_LEARN_PRODUCTBOOK].Msgfunc = &GLGaeaServer::MsgProductLearnBookAF;
    m_MsgFunc[NET_MSG_PRODUCT_TYPE_DEL_REQ].Msgfunc = &GLGaeaServer::MsgProductTypeDelAF;

	//! 개인상점
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_BUY_CF].Msgfunc = &GLGaeaServer::MsgPrivateMarketItemBuyCF;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_OPTION_AF].Msgfunc = &GLGaeaServer::MsgPrivateMarketOptionAF;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_CLOSE].Msgfunc = &GLGaeaServer::MsgPrivateMarketSearchClose;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_CF].Msgfunc = &GLGaeaServer::MsgPrivateMarketItemSearchBasicCF;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_CF].Msgfunc = &GLGaeaServer::MsgPrivateMarketItemSearchNameCF;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_LOG_CF].Msgfunc = &GLGaeaServer::MsgPrivateMarketLogCF;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_STATE_CF].Msgfunc = &GLGaeaServer::MsgPrivateMarketStateCF;
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEND_DATA_FF].Msgfunc = &GLGaeaServer::MsgPrivateMarketSendPrivateMarketData;

    //! 개인상점 위탁판매
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ].Msgfunc = &GLGaeaServer::MsgConsignmentSaleGetListReq;
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK].Msgfunc = &GLGaeaServer::MsgConsignmentSaleGetListReq;
    m_MsgFunc[NET_MSG_CONSIGNMENT_ADD_ITEM_REQ].Msgfunc = &GLGaeaServer::MsgConsignmentSaleRegistReq;
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ].Msgfunc = &GLGaeaServer::MsgConsignmentSaleSendBackReq;
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK].Msgfunc = &GLGaeaServer::MsgConsignmentSaleSendBackAck;
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB].Msgfunc = &GLGaeaServer::MsgConsignmentSaleSendBackAckDB;
    m_MsgFunc[NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB].Msgfunc = &GLGaeaServer::MsgConsignmentSalePurchaseAckDB;

    //! 아이템 수리
    m_MsgFunc[NET_MSG_ITEM_REPAIR_REQ].Msgfunc = &GLGaeaServer::MsgItemRepairReq;
    m_MsgFunc[NET_MSG_ITEM_REPAIR_CARD_REQ].Msgfunc = &GLGaeaServer::MsgItemRepairCardUseReq;

    // PVE	
    m_MsgFunc[NET_MSG_PVE_CREATE_INSTANT_MAP_REQ_AF].Msgfunc = &GLGaeaServer::MsgReqPVECreateInstantMapAF;
    m_MsgFunc[NET_MSG_PVE_ENTRANCE_INSTANT_MAP_REQ_AF].Msgfunc = &GLGaeaServer::MsgReqPVEEntranceInstantMapAF;
    m_MsgFunc[NET_MSG_PVE_PORTAL_REQ_CF].Msgfunc = &GLGaeaServer::MsgPVEReqPortalCF;    
    m_MsgFunc[NET_MSG_PVE_RETURN_REQ_AF].Msgfunc = &GLGaeaServer::MsgPVEReqReturnAF;
    m_MsgFunc[NET_MSG_PVE_RETRY_REQ_CF].Msgfunc = &GLGaeaServer::MsgPVEReqRetryCF;
    m_MsgFunc[NET_MSG_PVE_SAVEPOS_REQ_AF].Msgfunc = &GLGaeaServer::MsgPVEReqSavePosAF;
    m_MsgFunc[NET_MSG_PVE_STAGE_RESULT_FA_OR_AF].Msgfunc = &GLGaeaServer::MsgPVEStageResultAF;
	m_MsgFunc[NET_MSG_PVE_EXIT_REQ_CF].Msgfunc = &GLGaeaServer::MsgPVEReqExitCF;
	m_MsgFunc[NET_MSG_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF].Msgfunc = &GLGaeaServer::MsgPVEReqReloadTriggerAF;
	m_MsgFunc[NET_MSG_PVE_GM_GOSTAGE_REQ_CF].Msgfunc = &GLGaeaServer::MsgPVEReqGoStageCF;
    m_MsgFunc[NET_MSG_PVE_ENTRANCE_CONFIRM_REQ].Msgfunc = &GLGaeaServer::MsgPVEReqEntranceConfirmCF;
    

    // IP event
    m_MsgFunc[NET_MSG_GM_LOAD_IPEVENT].Msgfunc = &GLGaeaServer::MsgGmLoadIPEvent;
    m_MsgFunc[NET_MSG_GM_SET_IPEVENT].Msgfunc = &GLGaeaServer::MsgGmSetIPEvent;

    m_MsgFunc[NET_MSG_GM_ACQUIRE_EXP].Msgfunc = &GLGaeaServer::MsgGmAcquireEXP;

	//! Rnattendance
	m_MsgFunc[NET_MSG_GCTRL_REQ_REQ_RNATTENDANCE_QUEST_START].Msgfunc = &GLGaeaServer::MsgReqRnattendanceQuestStart;	

	// Lotto System;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_AF].Msgfunc = &GLGaeaServer::MsgLottoSystemProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_CF].Msgfunc = &GLGaeaServer::MsgLottoSystemProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_CF].Msgfunc = &GLGaeaServer::MsgLottoSystemProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_AF].Msgfunc = &GLGaeaServer::MsgLottoSystemProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC].Msgfunc = &GLGaeaServer::MsgLottoSystemProc;


	m_MsgFunc[NET_MSG_GCTRL_REQ_REQ_RNATTENDANCE_QUEST_START].Msgfunc = &GLGaeaServer::MsgReqRnattendanceQuestStart;

	// Reload AI FSM;
	m_MsgFunc[NET_MSG_GCTRL_RELOAD_AI_FSM_CF].Msgfunc = &GLGaeaServer::MsgReloadAIFSMProc;

	// Country System;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_ADD_DAF].Msgfunc = &GLGaeaServer::MsgVCountryAdd;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_RESET_DAF].Msgfunc = &GLGaeaServer::MsgVCountryReset;

	// 맵 이동
	m_MsgFunc[NET_MSG_REQ_MOVE_MAP].Msgfunc = &GLGaeaServer::MsgReqMoveMap;

	// New Instance System;
	m_MsgFunc[NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_CF].Msgfunc = &GLGaeaServer::MsgInstanceSystemProc;

	// PVE 무한의 재단 
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_CF].Msgfunc = &GLGaeaServer::MsgReqInvenStagePass;
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_AF].Msgfunc = &GLGaeaServer::MsgReqInvenStagePassOk;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_CF].Msgfunc = &GLGaeaServer::MsgReqInvenEnterNumCharge;	
	m_MsgFunc[NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_AF].Msgfunc = &GLGaeaServer::MsgReqInvenEnterNumChargeOk;

	
	// TexasHoldem
	m_MsgFunc[NET_MSG_GCTRL_TEXAS_HOLD_EM].Msgfunc = &GLGaeaServer::MsgTexasHoldem;


	m_MsgFunc[NET_MSG_GCTRL_MATCHING_SYSTEM].Msgfunc = &GLGaeaServer::MsgMsgTournamentAgent;


    m_MsgFunc[NET_MSG_GCTRL_INVEN_CONSIGNEMENT_SLOT_EXPIRATION_REQ].Msgfunc = &GLGaeaServer::MsgConsignmentSlotExpand;	

	// FlyCamera;
	m_MsgFunc[NET_MSG_GM_FLYCAMERACONTROL_CF].Msgfunc = &GLGaeaServer::GMFlyCameraControlCF;
	m_MsgFunc[NET_MSG_GM_FLYCAMERA_SETVIEWSIGHT_CF].Msgfunc = &GLGaeaServer::GMFlyCameraSetViewSight;
    m_MsgFunc[NET_MSG_GM_FLYCAMERACONTROL_MOVE_CF].Msgfunc = &GLGaeaServer::GMFlyCameraControlMoveCF;

	// Post Renew
	m_MsgFunc[NET_MSG_POST_RENEW_LIST_REQ].Msgfunc = &GLGaeaServer::MsgPostRenewListReq;
	m_MsgFunc[NET_MSG_POST_RENEW_SEND_REQ].Msgfunc = &GLGaeaServer::MsgPostRenewSendReq;
	m_MsgFunc[NET_MSG_POST_RENEW_DEL_REQ].Msgfunc = &GLGaeaServer::MsgPostRenewDelReq;
	m_MsgFunc[NET_MSG_POST_RENEW_ATTACH_REQ].Msgfunc = &GLGaeaServer::MsgPostRenewAttachReq;
	m_MsgFunc[NET_MSG_POST_RENEW_READ_REQ].Msgfunc = &GLGaeaServer::MsgPostRenewReadReq;
	m_MsgFunc[NET_MSG_POST_RENEW_RETURN_REQ].Msgfunc = &GLGaeaServer::MsgPostRenewReturnReq;

	// DB 처리 응답
	m_MsgFunc[NET_MSG_POST_RENEW_CHA_CHECK_ACK].Msgfunc = &GLGaeaServer::MsgPostRenewChaCheckDB;
	m_MsgFunc[NET_MSG_POST_RENEW_ATTACH_ACK].Msgfunc = &GLGaeaServer::MsgPostRenewAttachAckDB;

	m_MsgFunc[NET_MSG_GCTRL_EVENT].	Msgfunc = &GLGaeaServer::MsgEvent;
	m_MsgFunc[NET_MSG_COUNTRECORDER_INFO_REQ].Msgfunc = &GLGaeaServer::MsgCountRecorderInfoReq;
	m_MsgFunc[NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_CF].Msgfunc = &GLGaeaServer::MsgItemDecomposeReq;

	m_MsgFunc[NET_MSG_GM_NOTIFY_MONEY_CAF].Msgfunc = &GLGaeaServer::MsgGmSetNotifyMoneyAF;

	m_MsgFunc[NET_MSG_GCTRL_REQ_EXCHANGE_BUY_FROM_NPC].Msgfunc = &GLGaeaServer::MsgReqExchangeBuyFromNpc;

	m_MsgFunc[NET_MSG_GCTRL_INVEN_COSTUME_STATS].Msgfunc = &GLGaeaServer::MsgReqInvenCostumeStats;

	// 개인 검색 허용 여부를 필드에서 먼저 검색한뒤 에이젼트로 전송
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_REQ_CA].Msgfunc = &GLGaeaServer::MsgMapSearchNameCA;
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_REQ_CF].Msgfunc = &GLGaeaServer::MsgMapSearchNameCF;

	m_MsgFunc[NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE].Msgfunc = &GLGaeaServer::MsgTransDB2WorldBattle;	
	m_MsgFunc[NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_CHAR_SAVE].Msgfunc = &GLGaeaServer::MsgTransDB2WorldBattleCharSave;	
	m_MsgFunc[NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_DB].Msgfunc = &GLGaeaServer::MsgTransDB2WorldBattleAns;

	m_MsgFunc[NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE].Msgfunc = &GLGaeaServer::MsgReqInvenCostumeStats_Release;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_FNEW_UPDATE].Msgfunc = &GLGaeaServer::MsgParty;

	// 프로파일 명령어 사용시 주의
	m_MsgFunc[NET_MSG_GM_NETWORKPROFILE_REQ].Msgfunc = &GLGaeaServer::MsgGmNetworkProfileReq;

	m_MsgFunc[NET_MSG_GCTRL_EXP_COMPRESSOR_REQ].Msgfunc = &GLGaeaServer::MsgReqExpCompressor;	
	m_MsgFunc[NET_MSG_GCTRL_EXP_CAPSULE_REQ].Msgfunc = &GLGaeaServer::MsgReqExpCapsule;	

	m_MsgFunc[NET_MSG_SEND_SERVER_STOP_MSG].Msgfunc = &GLGaeaServer::SendServerStopMsg;

	m_MsgFunc[NET_MSG_GCTRL_INVEN_TL_GRINDING].Msgfunc = &GLGaeaServer::MsgReqInvenTLGrinding;		
	m_MsgFunc[NET_MSG_MACROMODE_ONOFF_CF].Msgfunc = &GLGaeaServer::MsgReqMacroModeOnoffCF;		
	m_MsgFunc[NET_MSG_MACROMODE_REQNEARESTCROWINFO_CF].Msgfunc = &GLGaeaServer::MsgReqMacroModeNearestCrowInfoCF;	

	m_MsgFunc[NET_MSG_CHA_SAVE_FA].Msgfunc = &GLGaeaServer::MsgChaSaveAF;
	m_MsgFunc[NET_MSG_CHA_SAVE_END_FA].Msgfunc = &GLGaeaServer::MsgChaSaveEndAF;

	m_MsgFunc[NET_MSG_GM_REQ_USERCOUNT_LAND_CF].Msgfunc = &GLGaeaServer::MsgGMReqUserCountLandCF;

	// 란모바일
	m_MsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_REQUEST].Msgfunc = &GLGaeaServer::MsgGmRanMobileCommandRequest; 
	m_MsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE].Msgfunc = &GLGaeaServer::MsgGmRanMobileCommandResponse; 

	m_MsgFunc[NET_MSG_GCTRL_JACKPOT_PUSH].Msgfunc = &GLGaeaServer::MsgJackpotPush;

	m_MsgFunc[NET_MSG_GM_RELEASE_CTF_BUFF].Msgfunc = &GLGaeaServer::GMCtrolReleaseCTFBuff;

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    m_MsgFunc[NET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF].Msgfunc = &GLGaeaServer::MsgGmRandomboxNotifyItemReloadReqAF;

	m_MsgFunc[NET_MSG_GCTRL_CLUB_RENAME_CF].Msgfunc = &GLGaeaServer::MsgClubNameChangeCF;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_RENAME_DF].Msgfunc = &GLGaeaServer::MsgClubNameChangeDF;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_RENAME_AF].Msgfunc = &GLGaeaServer::MsgClubNameChangeAF;

	// 캐릭터 슬롯;
	m_MsgFunc[NET_MSG_REQUEST_LOCK_CHARSLOT_CF].Msgfunc = &GLGaeaServer::MsgReqLockCharSlotCF;
	m_MsgFunc[NET_MSG_REQUEST_UNLOCK_CHARSLOT_CF].Msgfunc = &GLGaeaServer::MsgReqUnlockCharSlotCF;
	m_MsgFunc[NET_MSG_REQUEST_CHANGE_CHARSLOT_CF].Msgfunc = &GLGaeaServer::MsgReqChangeCharSlotCF;
	m_MsgFunc[NET_MSG_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF].Msgfunc = &GLGaeaServer::MsgReqChangeCharSlotUseItemCF;

	// 아이템 스킬 변경;
	m_MsgFunc[NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_CF].Msgfunc = &GLGaeaServer::MsgReqInvenChangeItemSkillCF;

	// 아이템 기본 능력치 변경;
	m_MsgFunc[NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_CF].Msgfunc = &GLGaeaServer::MsgReqInvenChangeBasicStatsCF;

	// 아이템 가산 능력치 변경;
	m_MsgFunc[NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_CF].Msgfunc = &GLGaeaServer::MsgReqInvenChangeAddStatsCF;

	m_MsgFunc[NET_MSG_SKILL_RESET_CF].Msgfunc = &GLGaeaServer::MsgReqResetSkillStatsBySkillUI;
}

/*
BOOL GLGaeaServer::TraceFilter(EMNET_MSG nType)	//mjeon.AF.추적용 필터 (임시)
{
	switch(nType)	//mjeon.AF.test
	{
		case NET_MSG_HEARTBEAT_SERVER_REQ:
		case NET_MSG_I_AM_AGENT:
		case MET_MSG_GAME_JOIN_FIELDSVR:
		case NET_MSG_FIELD_REQ_JOIN:
		case NET_MSG_GCTRL_PERIOD:					//특정 캐릭터가 아닌 GaeaServer의 날씨등의 동기화 메시지
		case NET_MSG_FIELDSVR_CHARCHK:				//메시지 내의 캐릭터 이름을 이용해서 GLChar를 생성
		case NET_MSG_ACTIVITY_LOAD_COMPLETED_FF:	//메시지 자체적으로 ChaNum을 갖고 있다.
		case NET_MSG_GCTRL_TRADE_CANCEL:
		case NET_MSG_GCTRL_REQ_LANDIN:				//NET_MSG_CHARACTER
		case NET_MSG_GCTRL_REQ_READY:				//NET_MSG_CHARACTER
		case NET_MSG_GCTRL_ACTSTATE:				//NET_MSG_CHARACTER
		case NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD:	//NET_MSG_CHARACTER
		case NET_MSG_GCTRL_REQ_GATEOUT:
		case NET_MSG_ATTENDANCE_SYNC_FF:
		case NET_MSG_ATTENDANCE_RESET_FF:
		case NET_MSG_ATTENDANCE_UPGRADE_FF:
		case NET_MSG_GM_MOVE2MAPPOS_FLD:
		case NET_MSG_GM_MOVE2GATE_FLD:
		case NET_MSG_ATTENDANCE_DAILYMAINTENANCE_END_AF:
		case NET_MSG_CTF_READY_AF:
		case NET_MSG_CTF_PREPARE_AF:
		case NET_MSG_CTF_START_AF:
		case NET_MSG_CTF_SYNC_PLAYERNUM_AF:
		case NET_MSG_CTF_DONE_AF_AC:
		case NET_MSG_CTF_REWARD_INFO:
		case NET_MSG_CTF_RECALL_TO_AF:
		case NET_MSG_CTF_QUIT_AF:
		case NET_MSG_CTF_STOP_AF:
		case NET_MSG_GCTRL_TRADE:
		case NET_MSG_GCTRL_TRADE_TAR_ANS:
		case NET_MSG_GCTRL_REGEN_GATE:		
		case NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE:
		case NET_MSG_GCTRL_WITH_VEHICLE:
		case NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS:
		case NET_MSG_FIELDSVR_OUT:
		case NET_MSG_TRANSFER_CHAR:
		case NET_MSG_TRANSFER_REQUEST_AF:
		case NET_MSG_TRANSFERRED_CHAR_JOIN_AF:
			return TRUE;

		default:
			return FALSE;
	}
}
*/

BOOL GLGaeaServer::MsgProcess(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (NULL == m_MsgFunc[nmg->nType].Msgfunc)
	{
		GLChar* pChar = GetChar(dwGaeaID);

		if( !m_bEmulator )
		{
			if (pChar)
			{
				sc::writeLogError(
					sc::string::format(
						"GLGaeaServer::MsgProcess illigal message(%d) Name(%s)",
						nmg->nType, pChar->m_szName ));
			}
			else
			{
				sc::writeLogError(
					sc::string::format(
						"GLGaeaServer::MsgProcess illigal message(%d)",
						nmg->nType));
			}
		}
		
		return FALSE;
	}
	else
	{
		if(dwClientID == CLIENTID_NULL)
		{
			sc::writeLogDebug(sc::string::format("(GLGaeaServer)ClientID is NULL for the msg(%1%)", nmg->nType));
			return FALSE;
		}
		
		/*
		if(dwGaeaID == GAEAID_NULL)
		{
			//mjeon.AF.test
			if (!TraceFilter(nmg->nType))
			{
				sc::writeLogDebug(sc::string::format("(GLGaeaServer)GaeaID is NULL for the msg(%1%)", nmg->nType));					
			}
		}
		*/

		{
			// 필리핀 락커 해킹 관련
			// 메시지 딜레이 추가
			GLChar* pChar = GetChar( dwGaeaID );
			if ( pChar && m_pNetMsgMgr )
			{
				if ( m_pNetMsgMgr->BlockNetMsg( nmg->nType, pChar->CharDbNum() ) )
				{
					// 메시지 출력
					sc::writeLogError(
						sc::string::format(
						"GLGaeaServer::MsgProcess msg delay catch message(%d) character(%d)",
						nmg->nType, pChar->CharDbNum() ));

					GLMSG::SNET_MSG_CATCH_DELAY NetNsgFC;
					SENDTOCLIENT( pChar->GETCLIENTID(), &NetNsgFC );
					return TRUE;	// 클라이언트 접속 종료가 되어서 TRUE 로 수정함, 메시지 쿨타임을 주려고한 것이기 때문에 접속 종료가 되면 안됨, FALSE 일 때 접속 종료되는 코드가 나중에 들어가서 여기에 걸리면 의도치 않게 접속 종료가 되었음
				}
			}
		}

		(this->*m_MsgFunc[nmg->nType].Msgfunc)(dwClientID, dwGaeaID, nmg);
		return TRUE;
	}
}


void GLGaeaServer::SENDTOALLCLIENT(NET_MSG_GENERIC* nmg)
{
    if (m_pMsgServer)
		m_pMsgServer->SendAllClient(nmg);
}

void GLGaeaServer::SENDTOALLCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    if (m_pMsgServer)
        m_pMsgServer->SendAllClient(MsgType, Buffer, UseCrc);
}

void GLGaeaServer::SENDTOALLCLIENT(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    BOOST_FOREACH( const std::tr1::shared_ptr<msgpack::sbuffer>& spBuffer, vecSpBuffer)
        if (m_pMsgServer)
            m_pMsgServer->SendAllClient(MsgType, *spBuffer, UseCrc);
}

void GLGaeaServer::SENDTOCLIENT(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GASSERT(dwClientID < m_dwMaxClient);
	if (dwClientID >= m_dwMaxClient*2)
        return;

	if (m_pMsgServer)
		m_pMsgServer->SendClient(dwClientID, nmg);
}

void GLGaeaServer::SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    GASSERT(dwClientID < m_dwMaxClient);
    if (dwClientID >= m_dwMaxClient*2)
        return;

    if (m_pMsgServer)
        m_pMsgServer->SendClient(dwClientID, MsgType, Buffer, UseCrc);
}

void GLGaeaServer::SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    BOOST_FOREACH( const std::tr1::shared_ptr<msgpack::sbuffer>& spBuffer, vecSpBuffer)
        SENDTOCLIENT( dwClientID, MsgType, *spBuffer, UseCrc);
}

void GLGaeaServer::SENDTOCLUBCLIENT(DWORD dwClubID, NET_MSG_GENERIC* nmg)
{
	m_pClubMan->SendClubClient(dwClubID, nmg, 0);
}

void GLGaeaServer::SENDTOCLUBCLIENT(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    m_pClubMan->SendClubClient(dwClubID, MsgType, Buffer, 0, UseCrc);
}

void GLGaeaServer::SENDTOCLIENT_ONMAP ( DWORD dwMapID, NET_MSG_GENERIC* nmg )
{
	GLLandMan* pLandMan = GetLand( dwMapID );
	if ( !pLandMan )
		return;

	if ( !m_pMsgServer )
		return;

	GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{		
		m_pMsgServer->SendClient ( pCharNode->Data->ClientSlot(), nmg );
	}
}

void GLGaeaServer::SENDTOCLIENT_ONMAP(DWORD dwMapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        SENDTOCLIENT_ONMAP(dwMapID, &MsgPack);
}

void GLGaeaServer::SENDTOCLIENT_ONMAP_INSTANT ( DWORD dwMapID, NET_MSG_GENERIC* nmg )
{
	GLLandMan* pLandMan = GetLand( dwMapID );
	
	if(!pLandMan)
		return;

	if(!m_pMsgServer)
		return;

	if(!pLandMan->IsInstantMap())
		return;

	WORD wMainID = pLandMan->GetBaseMapID().wMainID;

	SET_INSTANTMAP_ID_ITER iter		= m_setInstantMapIDs.begin();
	SET_INSTANTMAP_ID_ITER iterEnd	= m_setInstantMapIDs.end();

	for(; iter != iterEnd; ++iter)
	{
		const DWORD instanceMapID(*iter);

		if ( instanceMapID == dwMapID ) 
		{
			SENDTOCLIENT_ONMAP( instanceMapID, nmg );
		}
	}
}

void GLGaeaServer::SENDTOCLIENT_ONMAP_INSTANT(DWORD dwMapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{    
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        SENDTOCLIENT_ONMAP_INSTANT(dwMapID, &MsgPack);
}

void GLGaeaServer::SENDTOCLUBCLIENT_ONMAP ( DWORD dwMapID, DWORD dwClubID, NET_MSG_GENERIC* nmg )
{
	GLLandMan* pLandMan = GetLand( dwMapID );
	if ( !pLandMan )
		return;

	if ( !m_pMsgServer )
		return;

	GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{		
		if ( pCharNode->Data->m_ClubDbNum == dwClubID )
			m_pMsgServer->SendClient ( pCharNode->Data->ClientSlot(), nmg );
	}
}

void GLGaeaServer::SENDTOCLUBCLIENT_ONMAP(DWORD dwMapID, DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        SENDTOCLUBCLIENT_ONMAP(dwMapID, dwClubID, &MsgPack);        
}

void GLGaeaServer::SENDTOMYSELF(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, Buffer, UseCrc))
		SENDTOMYSELF(&MsgPack);        
}
void GLGaeaServer::SENDTOMYSELF(NET_MSG_GENERIC* nmg)
{
	if ( m_pMsgServer == NULL )
		return;

	m_pMsgServer->SendMySelf(0, nmg);
}

void GLGaeaServer::SENDTOCLIENT_PINGREQ( DWORD dwClientID )
{
	GLMSG::SNET_GM_PING MsgPing;
	MsgPing.nType = NET_MSG_GM_PING;
	MsgPing.dwSent = GetTickCount();
	MsgPing.bServerType = 1;	//0:AgentServer		1:FieldServer

	SENDTOCLIENT ( dwClientID, &MsgPing );
}


//
//mjeon
//Insert PingDbAction into job queue.
//
BOOL GLGaeaServer::InsertPingLog(DWORD dwClientID, DWORD dwGaeaID, DWORD dwRTT)
{
	GLChar* pPC = GetChar(dwGaeaID);
    if (!pPC)
        return FALSE;

	int nUserNum  = pPC->GetUserID();

	//mjeon
	//Even though CFeildServer is accessed via m_pMsgServer to get clientIP,
	//this kind of access is not good.
	//should change this call-flow ASAP.

	CFieldServer *pFieldServer = static_cast<CFieldServer*>(m_pMsgServer);
    std::string IpAddress = pFieldServer->GetClientIP(dwClientID);	
    if (IpAddress.empty())
    {
        if (m_pAdoMan)
        {
            m_pAdoMan->AddLogJob(
                db::DbActionPtr(
                    new db::PingDbAction(nUserNum, dwRTT, "0.0.0.0", CTime::GetCurrentTime().GetTime())));
        }
    }
    else
    {
        if (m_pAdoMan)
        {
            m_pAdoMan->AddLogJob(
                db::DbActionPtr(
	                new db::PingDbAction(nUserNum, dwRTT, IpAddress.c_str(), CTime::GetCurrentTime().GetTime())));
        }
    }
	return TRUE;
}

// *****************************************************
// Desc: 맵 진입 처리 ( Normal Map, Instant Map 공통 사용 )
// *****************************************************
BOOL GLGaeaServer::EnterLand( DWORD dwGaeaID, DWORD dwGateID, DWORD dwGateToIndex, SNATIVEID sMapID /*= NATIVEID_NULL()*/ )
{
	/*//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pPC = GetChar ( dwGaeaID );
	if ( !pPC )
		return FALSE;

	//	Note : 맵이동시 대련취소.
	//
	const SCONFTING& sCONFTING = pPC->GETCONFRONTING();
	
	if ( sCONFTING.IsCONFRONTING() )
	{
		switch ( sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				GLChar* pTAR = GetChar ( sCONFTING.dwTAR_ID );
				
				if ( pTAR )
					pTAR->ResetConfront ( EMCONFRONT_END_MOVEMAP );

				pPC->ResetConfront ( EMCONFRONT_END_MOVEMAP );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;

		case EMCONFT_PARTY:
			{
				pPC->ResetConfront ( EMCONFRONT_END_NOTWIN );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;

		case EMCONFT_GUILD:
			{
				pPC->ResetConfront ( EMCONFRONT_END_NOTWIN );
				pPC->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
			}
			break;
		};
	}


	DWORD			dwMoveFailed= 0;
	DxLandGateMan*	pInGateMan	= NULL;
	PDXLANDGATE		pInGate		= NULL;

	D3DXVECTOR3		vStartPos(0,0,0);

	//SUMMONID_ITER SummonIter      = pPC->m_dwSummonGUIDList.end();
	//SUMMONID_ITER SummonIterBegin = pPC->m_dwSummonGUIDList.begin();

	GLLandMan* const pLand(pPC->GetLandMan());
	if ( pLand == NULL )
	{
		GASSERT ( 0&&"!pPC->pLandMan" );
		return FALSE;
	}

	DxLandGateMan*	pOutGateMan(pLand->GetLandGateMan());

	PDXLANDGATE		pOutGate(pOutGateMan->FindLandGate ( dwGateID ));

	if ( !pOutGate )
	{
		dwMoveFailed = 1;
		sc::writeLogError(sc::string::format("EnterLand() ERROR : Error %1%, GaeaID %2%, GateID %3%, GateIdx %4%, MID %5%, SID %6%", dwMoveFailed, dwGaeaID, dwGateID, dwGateToIndex, sMapID.Mid(), sMapID.Sid() ));		
		return FALSE;
	}

	//	Memo :	이동할 맵의 ID
	//
	const SNATIVEID _gaeaMapIDTo(sMapID == SNATIVEID(false) ? pOutGate->GetToMapID(dwGateToIndex) : sMapID);

	//	Note : Gate에 연결된 맵 찾기.
	GLLandMan* pInLandMan(GetLand ( _gaeaMapIDTo ));

	if ( !pInLandMan )
	{
		dwMoveFailed = 2;
		sc::writeLogError(sc::string::format("EnterLand() ERROR : Error %1%, GaeaID %2%, GateID %3%, GateIdx %4%, MID %5%, SID %6%", dwMoveFailed, dwGaeaID, dwGateID, dwGateToIndex, sMapID.Mid(), sMapID.Sid() ));		
		return FALSE;		
	}

	if ( pInLandMan->GetGaeaMapID() == SNATIVEID(false) )
	{
		dwMoveFailed = 3;
		sc::writeLogError(sc::string::format("EnterLand() ERROR : Error %1%, GaeaID %2%, GateID %3%, GateIdx %4%, MID %5%, SID %6%", dwMoveFailed, dwGaeaID, dwGateID, dwGateToIndex, sMapID.Mid(), sMapID.Sid() ));		
		return FALSE;
	}


	//	Memo :	현재 맵의 ID
	const MapID& _mapIDCurrent(pLand->GetMapID());
	const SNATIVEID _baseMapIDCurrent(_mapIDCurrent.getBaseMapID());
	const SNATIVEID _gaeaMapIDCurrent(_mapIDCurrent.getGaeaMapID());

	//
	//mjeon.CaptureTheField
	//
	// 이제 점령전 진행중에도 사냥터로 이용할 수 있게 되었다;
	const SNATIVEID& _baseMapIDTo(pInLandMan->GetBaseMapID());

	//	Note : 목표 게이트 가져오기.
	//
	pInGateMan = pInLandMan->GetLandGateMan ();

	if ( !pInGateMan )
	{
		dwMoveFailed = 4;
		sc::writeLogError(sc::string::format("EnterLand() ERROR : Error %1%, GaeaID %2%, GateID %3%, GateIdx %4%, MID %5%, SID %6%", dwMoveFailed, dwGaeaID, dwGateID, dwGateToIndex, sMapID.Mid(), sMapID.Sid() ));		
		return FALSE;
	}

	pInGate = pInGateMan->FindLandGate ( pOutGate->GetToGateID(dwGateToIndex) );

	if ( !pInGate )
	{
		dwMoveFailed = 5;
		sc::writeLogError(sc::string::format("EnterLand() ERROR : Error %1%, GaeaID %2%, GateID %3%, GateIdx %4%, MID %5%, SID %6%", dwMoveFailed, dwGaeaID, dwGateID, dwGateToIndex, sMapID.Mid(), sMapID.Sid() ));		
		return FALSE;
	}

	vStartPos = pInGate->GetGenPos ( DxLandGate::GEN_RENDUM );

	if ( !pPC->m_pQuadNode )
	{
		//GASSERT ( 0&&"!pPC->m_pQuadNode" );		

		sc::writeLogError(sc::string::format(
			"EnterLand() : !pPC->m_pQuadNode NULL, dbNum:%d mapIDFrom:%d/%d(%d/%d) mapIDTo:%d/%d(%d/%d)",
			pPC->m_CharDbNum, 
			_baseMapIDCurrent.Mid(), _baseMapIDCurrent.Sid(), _gaeaMapIDCurrent.Mid(), _gaeaMapIDCurrent.Sid(),
			_baseMapIDTo.Mid(), _baseMapIDTo.Sid(), _gaeaMapIDTo.Mid(), _gaeaMapIDTo.Sid()));
	}

	//	Note : 자신이 본 주변 셀을 정리.
	pPC->ResetViewAround ();
	// 현재 활성화된 스킬 클리어;
	pPC->ActiveSkillClear();

	//	Note : 종전 맵에 있던 관리 노드 삭제.
	//
	GLLandMan* pCurrentLandMan(pPC->GetLandMan());
	const MapID& _charMapID(pPC->GetCurrentMap());
	const MapID& _landMapID(pCurrentLandMan == NULL ? MapID() : pCurrentLandMan->GetMapID());	
	if ( pCurrentLandMan != NULL )
	{
		if ( pCurrentLandMan->AddGlobPcDelList(pPC->m_pCharNodeInLand) == false )
		{
			sc::writeLogError(sc::string::format("EnterLand() : pGaeaPC->m_pCharNodeInLand is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
				_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	if (pPC->m_pQuadNode && pPC->m_pQuadNode->pData)
	{
		if ( pPC->m_pQuadNode->pData->m_PCList.DELNODE(pPC->m_pCellNode) == false )
		{
			sc::writeLogError(sc::string::format("EnterLand() : pGaeaPC->m_pQuadNode is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));		
		}
	}
	else
	{
		sc::writeLogError(sc::string::format("EnterLand() : pGaeaPC->m_pQuadNode is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
			_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
	}

	// Note : 종전 맵에있던 소환수 삭제
	//
	DropOutAllSummon ( pPC );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetNavi ( pInLandMan->GetNavi(), vStartPos );

	//	Note : 새로운 관리 ID 부여.
	//
	//pPC->m_sMapID		= sToMapID;
	//pPC->m_pLandMan	= pInLandMan;
	
	//
	//mjeon.tbb
	//
	pInLandMan->EnterLand(pPC);
	//pPC->m_pLandNode = pInLandMan->m_GlobPCList.ADDHEAD ( pPC );

	

	// 중국일 경우 GainType을 초기화 하여 메시지를 한 번 더 띄우게 한다.
#ifdef CH_PARAM_USEGAIN //chinatest%%%
	pPC->m_ChinaGainType = 255;
#endif


	//	Note : GLLandMan의 셀에 등록하는 작업.
	//			
	//	RegistChar(pPC); --> GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨.

	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pPC->m_dwPetGUID, false, true );

	// VEHICLE
	SetMapMoveVehicle( pPC->ClientSlot(), pPC->GetGaeaID(), dwGateToIndex);
	SetActiveVehicle( pPC->ClientSlot(), pPC->GetGaeaID(), false );


	// 자신의 버프와 상태이상 효과를 모두 사라지게 해준다.
	if( _gaeaMapIDCurrent != _gaeaMapIDTo )
	{	
		for ( int i=0; i<EMBLOW_MULTI; ++i )
			pPC->DISABLEBLOW ( i );

		for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			pPC->RESETSKEFF ( i );

		// CP 초기화 맵인지 확인
		if ( pInLandMan->IsCPReset() )
		{
			pPC->ResetCP();
		}
	}

	pPC->ResetAction();

	/// ------------------------------------

	// 인던인 경우 성공을 에이전트 서버에 알림
	if ( pPC && pInLandMan->isMapInstant() )
	{
		GLMSG::SNET_CREATE_INSTANT_MAP_FB NetMsgFB;	//FA
		NetMsgFB.dwChaNum	= pPC->CharDbNum();
		NetMsgFB.emFB			= EMCHAR_CREATE_INSTANT_MAP_OK;
		NetMsgFB.bCullByObjectMap = pInLandMan->IsCullByObjectMap();
        NetMsgFB.mapID			= pInLandMan->GetMapID();
		NetMsgFB.vPos			= pPC->GetPosition();

		SENDTOAGENT(&NetMsgFB);
	}*/

	return TRUE;
}

BOOL GLGaeaServer::EnterLand( const DWORD dwCharDbNum, const SNATIVEID toGaeaMapID, const DWORD dwToGateID, const DWORD dwGateToIndex, const D3DXVECTOR3& vPosition )
{
    /*GLChar* pPC= GetCharByDbNum(dwCharDbNum);

    if (pPC == 0)
    {
        return FALSE;
    }

	const SNATIVEID checkMapID(pPC->GetCurrentMap().getBaseMapID());

	// pLand->EnterLand()에서 값을 바꾸므로 레퍼런스로 받지 않도록 한다;
	// 한 함수내에서 값 참조 및 값 변경을 동시에 하는것은 실수의 여지가 많으므로 좋지 않다;
	const MapID _currentMapID(pPC->GetCurrentMap());    
	const SNATIVEID& currentBaseMapID(_currentMapID.getBaseMapID());
	const SNATIVEID& currentGaeaMapID(_currentMapID.getGaeaMapID());

    GLLandMan* pToLandMan(GetLand(toGaeaMapID));
    if (pToLandMan == 0)
    {
        return FALSE;
    }

	if ( pToLandMan->GetGaeaMapID() == SNATIVEID(false) )
		return FALSE;

	GLLandMan* pCurrentLandMan(pPC->GetLandMan());
	if ( pCurrentLandMan == NULL )
	{
		GASSERT ( 0&&"!pPC->m_pLandMan" );
		sc::writeLogError(sc::string::format("pPC->m_pLandMan NULL _ dbNum : %1% _ baseMap : %2%/%3% _ gaeaMap :%4%/%5%",
			pPC->m_CharDbNum, currentBaseMapID.Mid(), currentBaseMapID.Sid(), currentGaeaMapID.Mid(), currentGaeaMapID.Sid()));
		return FALSE;
	}

	D3DXVECTOR3 vStartPos(vPosition);
    DxLandGateMan* pGateMan(pToLandMan->GetLandGateMan());
	if ( pGateMan != 0 )
    {
		PDXLANDGATE pGate(pGateMan->FindLandGate(dwToGateID));
		if ( pGate != 0 )
			vStartPos = pGate->GetGenPos(DxLandGate::GEN_RENDUM);
		
    }

// 	if ( currentGaeaMapID == toGaeaMapID )
// 	{ // 시작 지역과 도착 지역이 동일 하다면 위치만 바꾸어 준다;
//         //	Note : 자신이 본 주변 셀을 정리.
//         pPC->ResetViewAround ();
//         // 현재 활성화된 스킬 클리어;
//         pPC->ActiveSkillClear();
//         //	Note : 새로운 맵 위치로 내비게이션 초기화.
// 		pPC->SetNavi(pToLandMan->GetNavi(), vStartPos);
// 		pPC->ResetAction();
// 		return TRUE;
// 	}

    if ( pPC->m_pQuadNode == NULL )
    { // pPC->m_pQuadNode 가 NULL이냐 아니냐는 클라이언트에 의해서 결정 된다;
		// 클라이언트에서 로딩 후 frameMove에 의해 적당한 시간이 지나면 패킷을 보내고,
		// 그때 서버의 GetViewAround()를 통해 pPC->m_pQuadNode를 설정 한다;
		// 의도되지 않은 시점에서의 요청의 경우 해당 요청이 조작된 경우라면
		// 이전과 같이 로그만 찍고 끝낼것이 아니라 로그 + 처리가 같이 들어가야 한다;
		// 더불어 조작 되지 않은 경우에는 깔끔하게 처리 할 수 있도록 해야지,
		// 이전처럼 덜렁 로그만 하나 덜렁 남기는것은 옳지 않다;

        sc::writeLogError(sc::string::format("!pPC->m_pQuadNode NULL, %d %d %d",
            pPC->m_CharDbNum, currentBaseMapID.dwID, toGaeaMapID.dwID ));
    }

    //	Note : 자신이 본 주변 셀을 정리.
    pPC->ResetViewAround ();
	// 현재 활성화된 스킬 클리어;
	pPC->ActiveSkillClear();

    //	Note : 종전 맵에 있던 관리 노드 삭제.
    //
	const MapID& _landMapID(pCurrentLandMan->GetMapID());	
	if ( pCurrentLandMan->AddGlobPcDelList(pPC->m_pCharNodeInLand) == false )
	{
		sc::writeLogError(sc::string::format("EnterLand() : AddGlobPcDelList() : pPC->m_pCharNodeInLand is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
			_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
	}


	if ( pPC->m_pQuadNode && pPC->m_pQuadNode->pData )
	{
		if ( pPC->m_pQuadNode->pData->m_PCList.DELNODE(pPC->m_pCellNode) == false )
		{
			sc::writeLogError(sc::string::format("EnterLand() : pPC->m_pCellNode is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}
	else
	{
		sc::writeLogError(sc::string::format("EnterLand() : pPC->m_pQuadNode is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
			_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid(),
			unsigned int(pPC->m_pQuadNode)));		
	}

    // Note : 종전 맵에있던 소환수 삭제
    //
    DropOutAllSummon ( pPC );

    //	Note : 새로운 맵 위치로 내비게이션 초기화.
    //
    pPC->SetNavi ( pToLandMan->GetNavi(), vStartPos );

    //
    //mjeon.tbb
    //
    pToLandMan->EnterLand(pPC);

    // 중국일 경우 GainType을 초기화 하여 메시지를 한 번 더 띄우게 한다.
#ifdef CH_PARAM_USEGAIN //chinatest%%%
    pPC->m_ChinaGainType = 255;
#endif

    pPC->m_dwCeID = 0;
    pPC->m_pQuadNode = NULL;
    pPC->m_pCellNode = NULL;

    // PET
    // 맵이동시 Pet 삭제
    DropOutPET ( pPC->m_dwPetGUID, false, true );

    // VEHICLE
    SetActiveVehicle( pPC->ClientSlot(), pPC->GetGaeaID(), false );
    

    // 자신의 버프와 상태이상 효과를 모두 사라지게 해준다.
    if( currentGaeaMapID != toGaeaMapID )
    {
        for ( int i=0; i<EMBLOW_MULTI; ++i )
            pPC->DISABLEBLOW ( i );

        for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
            pPC->RESETSKEFF ( i );

        // CP 초기화 맵인지 확인
        if ( pToLandMan->IsCPReset() )
        {
            pPC->ResetCP();
        }
    }

    pPC->ResetAction();

    // 인던인 경우 성공을 에이전트 서버에 알림
    if ( (pPC != NULL) && (pToLandMan->GetInstantType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE) )
    {
        GLMSG::SNET_CREATE_INSTANT_MAP_FB NetMsgFB;	//FA
        NetMsgFB.dwChaNum	= pPC->CharDbNum();
        NetMsgFB.emFB			= EMCHAR_CREATE_INSTANT_MAP_OK;
        NetMsgFB.bCullByObjectMap = pToLandMan->IsCullByObjectMap();
        NetMsgFB.mapID			= pToLandMan->GetMapID();
        NetMsgFB.vPos			= pPC->GetPosition();

        SENDTOAGENT(&NetMsgFB);
    }*/

    return TRUE;
}

// *****************************************************
// Desc: 맵이동 요청 처리
// *****************************************************

BOOL GLGaeaServer::RequestMoveMapPC ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{	
	GLMSG::SNETREQ_GATEOUT* pNetMsg = static_cast< GLMSG::SNETREQ_GATEOUT* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( pNetMsg->dwGaeaID );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	/// 출발 맵의 게이트 매니저를 구한다;
	GLLandMan* const pLand = pChar->GetLandMan();
	if ( NULL == pLand )
	{
		return FALSE;
	}

	DxLandGateMan* pOutGateMan = pLand->GetLandGateMan();
	if ( NULL == pOutGateMan )
	{
		return FALSE;
	}

	PDXLANDGATE pOutGate = pOutGateMan->FindLandGate( pNetMsg->dwGateID );
	if ( NULL == pOutGate )
	{
		sc::writeLogError( sc::string::format(
			"[ Move Map ] [ Invalid Gate ID ( Move Gate ) : %1% ]",
			pChar->CharDbNum()) );

		return FALSE;
	}

	/// 도착 맵이 올바른지 확인한다;
	SNATIVEID sBaseMapID = pOutGate->GetToMapID( pNetMsg->dwGateToIndex );
	GLLandMan* pArriveMapLand = GetLand( sBaseMapID );
	if ( NULL == pArriveMapLand )
	{
		sc::writeLogError( sc::string::format(
			"[ Move Map ] [ Invalid MapID ( Move Gate ) : %1% / %2% ]",
			pChar->CharDbNum(),
			sBaseMapID.Mid() ) );

		return FALSE;		
	}
	
	/// 이동 정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_GATE;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.dwGateID = pNetMsg->dwGateToIndex;
	sMoveInfo.sMapID = sBaseMapID;
	sMoveInfo.dwGateID = pOutGate->GetToGateID( pNetMsg->dwGateToIndex );

	/// 이동한다;
	GLMSG::SNETREQ_GATEOUT_FB sNetMsgGateOutFB;
	sNetMsgGateOutFB.dwChaNum = pChar->CharDbNum();
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
	{
		sNetMsgGateOutFB.emFB = EMCHAR_GATEOUT_FAIL;
		SENDTOAGENT( &sNetMsgGateOutFB );	//pChar->ClientSlot()

		return FALSE;
	}
	else
	{
		sNetMsgGateOutFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		sNetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
		sNetMsgGateOutFB.sMapID = pChar->GetCurrentMap();
		sNetMsgGateOutFB.vPos = pChar->GetPosition();
		SENDTOAGENT( &sNetMsgGateOutFB );
	}

	return TRUE;

	/*GLMSG::SNETREQ_GATEOUT_FB NetMsgGateOutFB;	
	
	BOOL bSuccess = EnterLand( pNetMsg->dwGaeaID, pNetMsg->dwGateID, pNetMsg->dwGateToIndex );

	GLChar* pPC = GetChar ( pNetMsg->dwGaeaID );

	if ( !pPC )
		return FALSE;	

	NetMsgGateOutFB.dwChaNum = pPC->CharDbNum();

    if( !bSuccess )
	{
		NetMsgGateOutFB.emFB = EMCHAR_GATEOUT_FAIL;
		SENDTOAGENT(&NetMsgGateOutFB);	//pChar->ClientSlot()
	}
	else
	{
		GLLandMan* pLand = pPC->GetLandMan();
		NetMsgGateOutFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		NetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
		NetMsgGateOutFB.sMapID = pPC->GetCurrentMap();
		NetMsgGateOutFB.vPos   = pPC->GetPosition();
		SENDTOAGENT(&NetMsgGateOutFB);	//pChar->ClientSlot()
	}

	return FALSE;*/
}

//AF
BOOL GLGaeaServer::RequestCreateInstantMap( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( (dwGaeaID != GAEAID_NULL) || (dwClientID != m_dwAgentSlot) )
		return FALSE;	

	GLMSG::SNET_CREATE_INSTANT_MAP_FLD *pNetMsg = (GLMSG::SNET_CREATE_INSTANT_MAP_FLD *)nmg;

	GLChar *pPC = GetCharByDbNum(pNetMsg->dwChaNum);

	if ( !pPC )
		return FALSE;

	SENTRYINSTMAPINFO instInfo;
	instInfo.bCreate			= pNetMsg->bCreate;
	instInfo.dwGaeaID		= pNetMsg->dwGaeaID;
	instInfo.PartyID			= pPC->GetPartyID();
	instInfo.sBaseMapID		= pNetMsg->sBaseMapID;
	instInfo.instanceMapID  = pNetMsg->instanceMapID;
	instInfo.dwChaNum		= pPC->CharDbNum();
	instInfo.dwGateID			= pNetMsg->dwGateID;
	instInfo.dwGateToID		= pNetMsg->dwGateToIndex;
	instInfo.bCreateTest		= pNetMsg->bCreateTest;
	instInfo.dwReqTime		= ::GetTickCount();

	//mjeon
	m_queInstantMapToCreate.push(instInfo);

	//InsertEntryInstMapInfo( instInfo, TRUE );

	return FALSE;
}


//
//Transfer: FieldSever -> InstanceServer
//
BOOL GLGaeaServer::TransferToInstance(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	return TRUE;

	/*GASSERT(IsField());

	GLMSG::NET_TRANSFER_REQUEST_AF *pmsg = (GLMSG::NET_TRANSFER_REQUEST_AF *)nmg;

	GLChar *pChar = GetChar64(pmsg->dwChaNum, pmsg->dwSvrGroupNum);

	if (!pChar)
	{
		sc::writeLogError(sc::string::format("TransferToInstance - Char(%1%, %2%) does not exist", pmsg->dwSvrGroupNum, pmsg->dwChaNum));
		return FALSE;
	}

	return TransferTo(pChar, pmsg->dwInstanceServerNum, SNATIVEID(pmsg->dwMapID), pmsg->dwGateID);*/
}

//
//Transfer: InstanceServer -> FieldSever
//
BOOL GLGaeaServer::TransferFromInstance( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	return TRUE;

	/*GASSERT(dwGaeaID == GAEAID_NULL);
	GASSERT(IsInstance());

	GLMSG::NET_TRANSFER_BACK_MI *pmsg = (GLMSG::NET_TRANSFER_BACK_MI *)nmg;

	GLChar *pChar = GetChar64(pmsg->dwChaNum, pmsg->dwSvrGroupNum);

	if (!pChar)
	{
		sc::writeLogError(sc::string::format("TransferFromInstance - Char(%1%, %2%) does not exist", pmsg->dwSvrGroupNum, pmsg->dwChaNum));
		return FALSE;
	}

	SNATIVEID idMap = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
	DWORD dwGateID = GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];

	return TransferTo(pChar, MAXNUM_OF_INSTANCESERVER_TO_MATCH, idMap, dwGateID);*/
}


//
//mjeon.instance.transfer
//  FAF or FAMI (M: Match, I: Instance) - Serialization Target Field
//
BOOL GLGaeaServer::TransferChar(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRANSFER_CHAR *pmsg = (GLMSG::SNET_TRANSFER_CHAR *)nmg;
	
	sc::writeLogDebug(sc::string::format("Received TransferChar(%1%/%2%)", pmsg->wSeq, pmsg->wLastSeq));	

	BOOL bDone = m_TransferMan.Assemble(UNIQUEID(pmsg->dwSvrGroupNum, pmsg->dwChaNum).Id(), pmsg->data, (DWORD)pmsg->wDataSize, pmsg->wSeq, pmsg->wLastSeq);

	if (bDone)
	{
		m_TransferMan.SetDestination(UNIQUEID(pmsg->dwSvrGroupNum, pmsg->dwChaNum).Id(), pmsg->dwMapID, pmsg->dwGateID);

		GASSERT((pmsg->wOnlye4DBSave && IsInstance()) == FALSE);

		if (pmsg->IsToInstance())	//FieldServer	->	InstanceServer
		{
			GASSERT(IsInstance());

			GLMSG::SNET_TRANSFER_CHAR_DONE_FA msg;
			msg.bToInstance		= pmsg->IsToInstance();
			msg.dwInstanceServerNum = pmsg->dwInstanceServerNum;
			msg.dwSvrGroupNum	= pmsg->dwSvrGroupNum;
			msg.dwChaNum		= pmsg->dwChaNum;
			msg.dwMapID			= pmsg->dwMapID;
			msg.dwGateID		= pmsg->dwGateID;
			
			m_pMsgServer->SendMatch(&msg);
		}
		else					//InstanceServer->	FieldServer
		{
			GASSERT(IsField());

			if(pmsg->wOnlye4DBSave)
			{
				//캐릭터 DB 저장
				GLChar *pChar = NEW_CHAR(wb::UNKNOWN_COUNTRY);

				if (!pChar)
				{
					sc::writeLogError(sc::string::format("GLGaeaServer::TransferChar() - NEW_CHAR(%1%, %2%) failed", pmsg->dwSvrGroupNum, pmsg->dwChaNum));
					return FALSE;
				}					

				DWORD dwSize = 0;
				DWORD64 dw64UniqueID= UNIQUEID(pmsg->dwSvrGroupNum, pmsg->dwChaNum).Id();

				BYTE *pBuffer = m_TransferMan.Take(dw64UniqueID, dwSize, pmsg->dwMapID, pmsg->dwGateID);

				int nLen = SerializeToChar(pBuffer, dwSize, pChar);

				if (nLen == 0)
				{
					RELEASE_CHAR(pChar);
					sc::writeLogError(sc::string::format("GLGaeaServer::TransferChar() - Serialization of the character(%1%, %2%) failed", pmsg->dwSvrGroupNum, pmsg->dwChaNum));
					return FALSE;
				}
				
				sc::writeLogDebug(sc::string::format("Char(%1%) transferred from InstanceServer is being saved into DB", pChar->m_szName));

                std::tr1::shared_ptr<db::DbAction> spNoAction((db::DbAction*) 0);
                SaveCharacterIntoDB(pChar, spNoAction);

				RELEASE_CHAR(pChar);
			}
			else
			{
				GLMSG::SNET_TRANSFER_CHAR_DONE_FA msg;
				msg.bToInstance		= pmsg->IsToInstance();
				msg.dwInstanceServerNum = pmsg->dwInstanceServerNum;
				msg.dwChaNum		= pmsg->dwChaNum;
				msg.dwSvrGroupNum	= m_pMsgServer->GetServerGroup();
				msg.dwMapID			= pmsg->dwMapID;
				msg.dwGateID		= pmsg->dwGateID;

				SENDTOAGENT(&msg);
			}
		}
	}

	return bDone;
}

BOOL GLGaeaServer::TransferCharJoinAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRANSFERRED_CHAR_JOIN_AF *pmsg = (GLMSG::SNET_TRANSFERRED_CHAR_JOIN_AF *)nmg;
	
	DWORD64 dw64UniqueID= UNIQUEID(pmsg->dwSvrGroupNum, pmsg->dwChaNum).Id();


	GLChar *pChar	= NEW_CHAR(pmsg->m_Country);
	DWORD dwSize	= 0;
	DWORD dwMapID	= pmsg->dwMapID;
	DWORD dwGateID	= 1;
	DWORD dwKey		= pmsg->dwKey;		//InstanceKey

	if (m_TransferMan.IsDone(dw64UniqueID) == FALSE)
	{
		RELEASE_CHAR(pChar);
		sc::writeLogError(sc::string::format("GLGaeaServer::TransferCharJoinAF - The Char(%1%, %2%) is not completely transferred yet.",pmsg->dwSvrGroupNum, pmsg->dwChaNum));
		return FALSE;
	}

	if (IsInstance())
	{
		if (dwKey == INVALID_INSTANCE_KEY)
		{
			RELEASE_CHAR(pChar);
			sc::writeLogError(sc::string::format("GLGaeaServer::TransferCharJoinAF - The Char(%1%, %2%)'s InstanceKey(%3%) is not valid.",pmsg->dwSvrGroupNum, pmsg->dwChaNum, dwKey));

			return FALSE;
		}
	}
	else	//FieldServer
	{
		GASSERT(dwKey == INVALID_INSTANCE_KEY);
	}
		
	BYTE *pBuffer = m_TransferMan.Take(dw64UniqueID, dwSize, dwMapID, dwGateID);
	
	int nLen = SerializeToChar(pBuffer, dwSize, pChar);

	if (!pChar || nLen == 0)
	{
		RELEASE_CHAR(pChar);
		sc::writeLogError(std::string("GLGaeaServer::TransferCharJoinAF() - Transfering character instance failed"));
		return FALSE;
	}

	GLChar *pCharInit = InitSerializedChar(pChar, dwKey, dwMapID, dwGateID);

	if (pCharInit)
	{
		if (IsInstance())
		{
			NET_MSG_JOIN_INSTANCESVR_INFO msgJoinFB;
			msgJoinFB.emType	= EMJOINTYPE_TRANSFER;
			msgJoinFB.dwSvrGroupNum = pmsg->dwSvrGroupNum;
			msgJoinFB.dwChaNum	= pmsg->dwChaNum;
			msgJoinFB.dwGaeaID	= pmsg->dwGaeaID;
			msgJoinFB.dwKey		= pmsg->dwKey;
			
			std::string strIP = m_pMsgServer->GetIP();
			GASSERT(!strIP.empty());

			msgJoinFB.SetIP(strIP.c_str());
			msgJoinFB.nServicePort = m_pMsgServer->GetPort();

			m_pMsgServer->SendMatch(&msgJoinFB);
		}
		else
		{
			//
			//Agent 에 케릭터의 접속 성공을 알림 (Agent 가 Client 에게 새로운 Field 서버에 접속하게 요청)	
			//
            SNATIVEID NewMapID(dwMapID);
//             GLLandMan* pLandMan = pCharInit->m_pLandMan;
//             if (pLandMan)
//                 NewMapID = pLandMan->GetMapID();

		    NET_GAME_JOIN_FIELDSVR_FB msgJoinFB(
                EMJOINTYPE_TRANSFER,
		        GetFieldSvrID(),
		        pChar->GetGaeaID(),
		        pChar->CharDbNum(),
                NewMapID,
                pCharInit->GetPosition());
		    SENDTOAGENT(&msgJoinFB);

			return TRUE;
		}		
	}
	
	return FALSE;
}

// *****************************************************
// Agent 의 이동로직 수행관련 성공 & 실패 여부;
// *****************************************************
// Agent -> Field;
BOOL GLGaeaServer::RequestMoveMapStateFC ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* pStateMsg =
		static_cast< GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* >( pMsg );
	if ( NULL == pStateMsg )
		return FALSE;

	MoveSystem::CMoveManagerField::Instance()->OnFailToMove( pStateMsg );

	return TRUE;
}

// *****************************************************
// Desc: 필드서버에서 떠나감을 알림;
// *****************************************************
// Agent -> Field;
BOOL GLGaeaServer::RequestFieldSvrOut ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_MOVEMAP_AF* pOutMsg =
		static_cast< GLMSG::SNETPC_MOVEMAP_AF* >( pMsg );
	if ( NULL == pOutMsg )
		return FALSE;

	if ( false == MoveSystem::CMoveManagerField::Instance()->OnMoveMap( pOutMsg ) )
		return FALSE;

	/*GLChar *pChar = GetChar64(pmsgOut->dwChaNum, pmsgOut->dwSvrGroupNum);
	if (!pChar)
		return FALSE;

//	pChar->ReportPositionToAgent();

	//대련취소
	const SCONFTING& sCONFTING = pChar->GETCONFRONTING();
	if (sCONFTING.IsCONFRONTING())
	{
		GASSERT(IsField());

		switch (sCONFTING.emTYPE)
		{
		case EMCONFT_ONE:
			{
				GLChar* pTAR = GetChar(sCONFTING.dwTAR_ID);
				if (pTAR)
					pTAR->ResetConfront(EMCONFRONT_END_MOVEMAP);

				pChar->ResetConfront(EMCONFRONT_END_MOVEMAP);
				pChar->ReceiveLivingPoint(GLCONST_CHAR::nCONFRONT_LOSS_LP);
			}
			break;
		
		case EMCONFT_PARTY:
			{
				pChar->ResetConfront(EMCONFRONT_END_NOTWIN);
				pChar->ReceiveLivingPoint(GLCONST_CHAR::nCONFRONT_LOSS_LP);
			}
			break;
		
		case EMCONFT_GUILD:
			{
				pChar->ResetConfront(EMCONFRONT_END_NOTWIN);
				pChar->ReceiveLivingPoint(GLCONST_CHAR::nCONFRONT_LOSS_LP);
			}
			break;
		};
	}

	GLMSG::SNETPC_FIELDSVR_OUT_FB msgOutFB;
	msgOutFB.dwSvrGroupNum	= pChar->SvrGroupNum();
	msgOutFB.dwChaNum		= pChar->CharDbNum();
	msgOutFB.positionPrevious	= pChar->GetPosition();
	msgOutFB.bTransfer		= pmsgOut->bTransfer;
	msgOutFB.bToInstance	= pmsgOut->bToInstance;
    msgOutFB.emType	        = pmsgOut->emType;	
    msgOutFB.bChangeField   = true;
	msgOutFB.sSummonable	= pChar->m_sSummonable;

	m_pMsgServer->ResetGaeaID(pChar->ClientSlot());

	ReserveDropOutPC(pChar->GetGaeaID(), db::DbActionPtr(new db::CharacterDbToAgentMessage(dwClientID, &msgOutFB)));
	
	GLLandMan* const pLand(pChar->GetLandMan());

	if ( pmsgOut->bSameFieldServer == FALSE )
	{ // 같은 필드 서버 내에서의 이동인 경우 진입할때 이벤트가 발생하므로 이곳에서는 취소 한다;
		const SNATIVEID& previousMapID = pLand == 0 ? SNATIVEID(false) : pLand->GetGaeaMapID();
		GLMSG::NET_INSTANCE_MOVE_MAP _moveMapMessage(pChar->CharDbNum(), previousMapID, pChar->GetPosition(), GetFieldSvrID(), pmsgOut->targetMapID);
		SENDTOMYSELF(&_moveMapMessage);
	}

	if( pChar->m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pChar->GetUserID();
		TracingMsg.SetAccount(pChar->m_szUID);

		CString strTemp;
		if ( pLand )
		{
			int nPosX(0);
			int nPosY(0);
			const D3DXVECTOR3& vPosition = pChar->GetPosition();
			pLand->GetMapAxisInfo().Convert2MapPos ( vPosition.x, vPosition.z, nPosX, nPosY );

			strTemp.Format(
				"FieldServer Out!!, [%s][%s], MAP:mid[%d]sid[%d], StartPos:[%d][%d], Money:[%I64d]",
				pChar->m_szUID,
				pChar->m_szName,
				pChar->GetCurrentMap().getGaeaMapID().Mid(),
				pChar->GetCurrentMap().getGaeaMapID().Sid(),
				nPosX,
				nPosY,
				pChar->GetInvenMoney());
		}
		else
		{
			strTemp.Format(
				"FieldServer Out!!, [%s][%s], MAP:mid[%d]sid[%d], Money:[%I64d]",
				pChar->m_szUID,
				pChar->m_szName,
				pChar->GetCurrentMap().getGaeaMapID().Mid(),
				pChar->GetCurrentMap().getGaeaMapID().Sid(),				
				pChar->GetInvenMoney());
		}
		

		TracingMsg.SetLog(strTemp.GetString());
		SENDTOAGENT(&TracingMsg);
	}
*/
	return TRUE;
}

// *****************************************************
// Desc: 레벨 업시 현재 맵에 머물러 있지 않아야 하는 경우
// *****************************************************
BOOL GLGaeaServer::RequestMustLeaveMap ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_REQ_MUST_LEAVE_MAP* pNetMsg = static_cast< GLMSG::SNETPC_REQ_MUST_LEAVE_MAP* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;
	
	GLChar* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar )
		return FALSE;
	
	if ( pChar->ClientSlot() != dwClientID )
		return FALSE;

	GLLandMan* pLand = pChar->GetLandMan();
	if ( NULL == pLand )
		return FALSE;

	const MapID& sMAPID = pNetMsg->sMAPID;
	DWORD dwGATEID = pNetMsg->dwGATEID;
	D3DXVECTOR3 vPOS( 0.f, 0.f, 0.f );

	/// 이동 가능한지 체크한다;
	GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_FB sNetMsgFB;

	if ( dwGATEID == UINT_MAX )				
	{
		SENDTOCLIENT( dwClientID, &sNetMsgFB );
		return FALSE;
	}

	const SLEVEL_REQUIRE* pLEVEL_REQUIRE = GetLevelRequire( pLand->GetBaseMapID() );
	if ( ( pLEVEL_REQUIRE != NULL ) && ( pNetMsg->emReason == EMREQ_MUST_LEAVE_REASON_LEVEL ) )
	{
		EMREQFAIL emReqFail = EMREQUIRE_COMPLETE;
		emReqFail = pLEVEL_REQUIRE->ISCOMPLETE( pChar );
		if ( emReqFail == EMREQUIRE_COMPLETE )
		{
			SENDTOCLIENT( dwClientID, &sNetMsgFB );
			return FALSE;
		}
	}
	else if ( pNetMsg->emReason == EMREQ_MUST_LEAVE_REASON_EXPEDITION )
	{
		if ( pChar->GetPartyID().isValidExpedition() )
		{
			SENDTOCLIENT( dwClientID, &sNetMsgFB );
			return FALSE;
		}
	}
	else
	{
		SENDTOCLIENT( dwClientID, &sNetMsgFB );
		return FALSE;
	}

	/// 이동 정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;
	sMoveInfo.vPosition = vPOS;

	/// 이동한다;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	return TRUE;

	/*// PET
	// 맵이동시 Pet 삭제
	DropOutPET ( pPC->m_dwPetGUID, true, true );

	//VEHICLE
	SaveVehicle( pPC->ClientSlot(), pPC->GetGaeaID(), true );

	// SUMMON
	DropOutAllSummon ( pPC );

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pPC->IsActState(EM_ACT_MACROMODE) )
	{
		pPC->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT ( pPC->ClientSlot(), &msg );
	}

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->GetFieldServerNum()!=m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_AG NetMsgAg;
		NetMsgAg.dwChaNum = pPC->CharDbNum();
		NetMsgAg.sMAPID = sMAPID;
		NetMsgAg.dwGATEID = dwGATEID;
		NetMsgAg.vPOS = vPOS;
		SENDTOAGENT(&NetMsgAg);	//pChar->ClientSlot()
	}
    else
    {
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다 .SNATIVEID sCurMapID = pChar->m_sMapID; 를 사용하여야 한다.;
        const MapID sCurMapID(pPC->GetCurrentMap());

		BOOL bOK = RequestInvenRecallThisSvr ( pPC, sMAPID, dwGATEID, vPOS );
		if ( !bOK )
		{
			SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
			return FALSE;
		}
		
		// 버프와 상태이상 제거
		if ( sCurMapID != sMAPID )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )
				pPC->DISABLEBLOW ( i );
			for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
				pPC->RESETSKEFF ( i );
			
			// CP 초기화 맵인지 확인
			if ( pMapNode->IsCombatPointReset() )
			{
				pPC->ResetCP();
			}
		}

		pPC->ResetAction();

		//	Note : 멥 이동 성공을 알림.
		//
		GLMSG::SNETPC_REQ_RECALL_FB NetRecallFB;
		NetRecallFB.dwChaNum = pPC->CharDbNum();
		NetRecallFB.emFB = EMREQ_RECALL_FB_OK;
		NetRecallFB.bCullByObjectMap = pMapNode->IsCullByObjectMap();
		NetRecallFB.sMAPID = sMAPID;
		NetRecallFB.vPOS = pPC->GetPosition();
		SENDTOAGENT(&NetRecallFB);	//pChar->ClientSlot()

	}

	return TRUE;*/
}


// *****************************************************
// Desc: 클라이언트 인던 맵 생성 요청 처리 
// *****************************************************
//CF
BOOL GLGaeaServer::RequestCreateInstantMapReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{	
	GLMSG::SNET_CREATE_INSTANT_MAP_REQ *pNetMsg = (GLMSG::SNET_CREATE_INSTANT_MAP_REQ *)nmg;

	GLMSG::SNET_CREATE_INSTANT_MAP_FB  NetMsgFB;	//FC
	GLMSG::SNET_CREATE_INSTANT_MAP_REQ NetMsg;		//FA

	SNATIVEID sTargetMapID;

	GLChar* pPC = GetChar ( dwGaeaID );	

	if(!pPC)
		return FALSE;

	NetMsg.dwChaNum = pPC->CharDbNum();
	
	const DWORD dwGateID = pNetMsg->dwGateID;
	
	if(dwGateID==UINT_MAX)
		goto _CreteInstantMapReqError;

	GLLandMan* const pLand(pPC->GetLandMan());
	if ( pLand == NULL )
		goto _CreteInstantMapReqError;    

	DxLandGateMan *pLandGateMan = pLand->GetLandGateMan();
	if(!pLandGateMan)
		goto _CreteInstantMapReqError;

	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate(dwGateID);
	
	if(!pLandGate)
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID;
		SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	sTargetMapID = pLandGate->GetToMapID();

	const SMAPNODE *pMapNode = FindMapNode(sTargetMapID);
	const GLLandMan *pDestLandMan = GetLand(sTargetMapID);
	
	if(!pMapNode || !pDestLandMan)
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID;
		SENDTOCLIENT(pPC->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	// 이전 맵의 스크립트에 게이트 이동에 관한 제어를 묻는다.;
	InstanceSystem::InstanceField* const pInstance(pLand->GetScriptInstance());
	if ( pInstance != NULL )
	{
		if ( pInstance->EventRequestMoveGate(pPC->GetCharID(), pLand->GetBaseMapID(), sTargetMapID, dwGateID, -1) == false )
			return FALSE;		
	}
	

	if ( pMapNode->GetInstanceType() != SMAPNODE::EMINSTANCE_OLD_INSTANCE )
	{
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT;
		SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 서버에서 검색된 게이트
	PDXLANDGATE pLandGateInServer = pLandGateMan->DetectGate(pPC->GetPosition());

	// 진입조건 검사
	if(pPC->UserLevel() < USER_USER_GM)
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
		emReqFail = pRequire->ISCOMPLETE ( pPC );

		if(emReqFail != EMREQUIRE_COMPLETE)
		{
			//	통과 권한이 없음.
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_CONDITION;
			SENDTOCLIENT(pPC->ClientSlot(), &NetMsgFB);
			return FALSE;
		}

		// 서버에서 검색한 게이트가 없을 경우 ( 대만 GS 툴에 대처 )
		if(!pLandGateInServer || !(pLandGateInServer->GetFlags()&DxLandGate::GATE_OUT))
			goto _CreteInstantMapReqError;

		// 파티맵이나 원정대맵일 경우 파티나 원정대에 가입되어 있는지 확인
		{
			GLMSG::SNETREQ_GATEOUT_FB NetMsgPartyMapFB;
			NetMsgPartyMapFB.sMapID = MapID(pMapNode->GetId(), pMapNode->GetId());
			NetMsgPartyMapFB.emFB = EMCHAR_GATEOUT_PARTYMAP;

			// 파티맵?
			if( true == pMapNode->IsPartyMap() && false == pMapNode->IsExpeditionMap() )
			{
				// 파티 가입 안되어 있거나 원정대라면
				if( false == pPC->isValidParty() || true == pPC->isValidExpedition() )
				{
					SENDTOCLIENT( pPC->ClientSlot(), &NetMsgPartyMapFB );
					return FALSE;
				}
			}
			// 원정대맵?
			else if( false == pMapNode->IsPartyMap() && true == pMapNode->IsExpeditionMap() )
			{
				// 원정대가 아니라면
				if( false == pPC->isValidExpedition() )
				{
					SENDTOCLIENT( pPC->ClientSlot(), &NetMsgPartyMapFB );
					return FALSE;
				}
			}
			// 둘다?
			else if( true == pMapNode->IsPartyMap() && true == pMapNode->IsExpeditionMap() )
			{
				// 파티 가입 안되어 있다면
				if( false == pPC->isValidParty() )
				{
					SENDTOCLIENT( pPC->ClientSlot(), &NetMsgPartyMapFB );
					return FALSE;
				}
			}
		}
	}


	// 이미 생성된 파티 중에 같은 인던을 생성한 사람이 있을경우의 처리
	if (pPC->isValidParty())
	{
		SET_INSTANTMAP_ID_ITER iter = m_setInstantMapIDs.begin();

		for(; iter != m_setInstantMapIDs.end(); ++iter)
		{	
			GLLandMan *pLandMan = GetLand(*iter);

			if(!pLandMan)
				continue;

			if(!pLandMan->IsInstantMap())
				continue;

			if(!pLandMan->IsPartyInstantMap())
				continue;

			if(pLandMan->GetBaseMapID() != sTargetMapID )
				continue;

			if ( pPC->GetPartyID() != pLandMan->GetInstantMapHostID() )
				continue;

			// 인던 삭제 대기타는 중
			if(pLandMan->Empty() || pLandMan->IsExcessiveLimitTime())
			{
				sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT_STATE., Name[%1%], CurMapID[%2%/%3%]",
									pPC->m_szName, pPC->GetCurrentMap().getGaeaMapID().Mid(), pPC->GetCurrentMap().getGaeaMapID().Sid() ) );

				NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT_STATE;
				SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );

				return FALSE;
			}

			NetMsg.instanceMapID = *iter;
			NetMsg.dwGaeaID	  = dwGaeaID;
			NetMsg.dwGateID	  = dwGateID;
			NetMsg.bCreate	  = FALSE;

			SENDTOAGENT(&NetMsg);
			return TRUE;
		}
	}	

    //mjeon.서버 상에 가용 물리 메모리가 100MB 이하인 경우에는 더 이상 인던을 생성하지 않는다.

    DWORD dwPhysicalMemAvailMB = (DWORD)(sc::getMemoryPhysicalAvail() / 1048576);	// 1048576 = 1024 * 1024

    if (dwPhysicalMemAvailMB <= 100)	//여유 물리 메모리가 100MB 이하
    {
        sc::writeLogError(sc::string::format("More memory is required for creating instance maps: only %1% (MB) is available !!", dwPhysicalMemAvailMB));

        NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_FIELDSVR;
        SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

	NetMsg.dwGateID = dwGateID;
	NetMsg.dwGaeaID = dwGaeaID;
	SENDTOAGENT(&NetMsg);

	return TRUE;

_CreteInstantMapReqError:
	NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
	SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );

	return FALSE;
}

// *****************************************************
// Desc: 클라이언트 인던 맵 생성 요청 처리 
// *****************************************************
BOOL GLGaeaServer::RequestInstantMapMoveOut( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETREQ_INSTANTMAP_MOVEOUT_REQ *pNetMsg = (GLMSG::SNETREQ_INSTANTMAP_MOVEOUT_REQ *)nmg;
	GLChar* pPC = GetChar ( dwGaeaID );	

	if ( !pPC )
		return FALSE;

	if ( pPC->ClientSlot()!=dwClientID )
		return FALSE;


	GLLandMan* pCurLandMan = GetLand ( pPC->GetCurrentMap().getGaeaMapID() );
	if( pCurLandMan )
	{
		if ( pCurLandMan->isMapInstant() == false )
			return FALSE;

		sc::writeLogDebug(sc::string::format( "Instant Map MoveOut Request. Name[%1%], CurMapID[%2%/%3%] PartyMap %4%, HostID %5%, UserCount %6%.", 
							pPC->m_szName, pPC->GetCurrentMap().getGaeaMapID().Mid(), pPC->GetCurrentMap().getGaeaMapID().Sid(), 
							pCurLandMan->IsPartyInstantMap(), pCurLandMan->GetInstantMapHostID(), pCurLandMan->m_GlobPCList.m_dwAmount ) );
	}
	else
	{
		sc::writeLogDebug(sc::string::format( "Instant Map MoveOut Request. Can't find the map. Name[%1%], CurMapID[%2%/%3%] ", 
							 pPC->m_szName, pPC->GetCurrentMap().getGaeaMapID().Mid(), pPC->GetCurrentMap().getGaeaMapID().Sid() ) );
		return FALSE;
	}

	pCurLandMan->MoveOutInstantMap(pPC);

	return TRUE;

}

// *****************************************************
// Desc: 클라이언트의 게이트 이동 요청 처리
// *****************************************************
BOOL GLGaeaServer::RequestGateOutReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETREQ_GATEOUT_REQ* pNetMsg = (GLMSG::SNETREQ_GATEOUT_REQ*)nmg;
	GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
	GLMSG::SNETREQ_GATEOUT NetMsg;	//FA	

	DWORD dwMoveFailed = 0;

	SNATIVEID mapIDTarget;

	GLChar* pPC = GetChar ( dwGaeaID );	

	if (!pPC)
		return FALSE;

	if ( pPC->IsDie() )
		return FALSE;

	if (pPC->ClientSlot() != dwClientID)
	{
		dwMoveFailed = 1;
		goto _GateOutError;
	}

	NetMsg.dwChaNum		= pPC->CharDbNum();
	NetMsgFB.dwChaNum	= pPC->CharDbNum();

	const DWORD dwGateID = pNetMsg->dwGateID;
	if (dwGateID==UINT_MAX)
	{
		dwMoveFailed = 2;
		goto _GateOutError;
	}

	GLLandMan* const pCurrentLand(pPC->GetLandMan());
	if (!pCurrentLand)
	{
		dwMoveFailed = 3;
		goto _GateOutError;
	}

	DxLandGateMan* const pLandGateMan(pCurrentLand->GetLandGateMan());
	if (!pLandGateMan)
	{
		dwMoveFailed = 4;
		goto _GateOutError;
	}

	PDXLANDGATE pLandGate(pLandGateMan->FindLandGate ( dwGateID ));
	if (!pLandGate)
	{
		dwMoveFailed = 5;
		goto _GateOutError;
	}
	
	mapIDTarget = pLandGate->GetToMapID(pNetMsg->dwGateToIndex);

	const SMAPNODE *pMapNode = FindMapNode ( mapIDTarget );
	if (!pMapNode)
	{
		dwMoveFailed = 6;
		goto _GateOutError;
	}

    // 이전 맵의 스크립트에 게이트 이동에 관한 제어를 묻는다.;
    //! 주의: Gate 0번으로 오는것이 꼭 실제 게이트 이동뿐만이 아니라 맵 입장조건이 안맞게 되어서 팅겨질때도 Gate0을 사용한다;
    InstanceSystem::InstanceField* const pInstance(pCurrentLand->GetScriptInstance());
    if ( pInstance != NULL )
	{
		const unsigned int _gateID(pLandGate->GetToGateID(pNetMsg->dwGateToIndex));
		if ( pInstance->EventRequestMoveGate(pPC->GetCharID(), pCurrentLand->GetBaseMapID(), mapIDTarget, dwGateID, _gateID) == false )
			return TRUE;

		const InstanceSystem::InformationPosition _reserveSetPosition(
			Faction::GLActorID(CROW_PC, pPC->GetCharID()),
			mapIDTarget, Faction::GLActorID(CROW_GATE, _gateID), 
			0.0f, 0.0f, InstanceSystem::EMMOVE_INSTANCE2INSTANCE);		
		m_pInstanceManager->requestMoveToPosition(pCurrentLand->GetGaeaMapID().Mid(), _reserveSetPosition);
		return TRUE;
	}

	// 서버에서 검색된 게이트
	PDXLANDGATE pLandGateInServer = pLandGateMan->DetectGate ( pPC->GetPosition() );

	// 진입조건 검사
	if ( pPC->UserLevel() < USER_USER_GM )
	{
		EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
		const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
		emReqFail = pRequire->ISCOMPLETE ( pPC ); 
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			//	통과 권한이 없음.
			NetMsgFB.emFB = EMCHAR_GATEOUT_CONDITION;
			SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
			return FALSE;
		}

		// 서버에서 검색한 게이트가 없을 경우 ( 대만 GS 툴에 대처 )
		if ( !pLandGateInServer || !(pLandGateInServer->GetFlags()&DxLandGate::GATE_OUT ) ) 
		{
			dwMoveFailed = 7; 
			goto _GateOutError;
		}

		// 파티맵이나 원정대맵일 경우 파티나 원정대에 가입되어 있는지 확인
		{
			GLMSG::SNETREQ_GATEOUT_FB NetMsgPartyMapFB;
			NetMsgPartyMapFB.sMapID = MapID(pMapNode->GetId(), pMapNode->GetId());
			NetMsgPartyMapFB.emFB = EMCHAR_GATEOUT_PARTYMAP;

			// 파티맵?
			if( true == pMapNode->IsPartyMap() && false == pMapNode->IsExpeditionMap() )
			{
				// 파티 가입 안되어 있거나 원정대라면
				if( false == pPC->isValidParty() || true == pPC->isValidExpedition() )
				{
					SENDTOCLIENT( pPC->ClientSlot(), &NetMsgPartyMapFB );
					return FALSE;
				}
			}
			// 원정대맵?
			else if( false == pMapNode->IsPartyMap() && true == pMapNode->IsExpeditionMap() )
			{
				// 원정대가 아니라면
				if( false == pPC->isValidExpedition() )
				{
					SENDTOCLIENT( pPC->ClientSlot(), &NetMsgPartyMapFB );
					return FALSE;
				}
			}
			// 둘다?
			else if( true == pMapNode->IsPartyMap() && true == pMapNode->IsExpeditionMap() )
			{
				// 파티 가입 안되어 있다면
				if( false == pPC->isValidParty() )
				{
					SENDTOCLIENT( pPC->ClientSlot(), &NetMsgPartyMapFB );
					return FALSE;
				}
			}
		}
	}

	// 선도전장이면
	GLLandMan* pLandMan = GetLand(mapIDTarget);
	if (pLandMan)
	{
		if (pMapNode->IsLeaderClubZone())
		{
			if ( GLCONST_CHAR::bPKLESS ) 
			{
				// 선도전 맵은 절대 못 들어간다...
				NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
				SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
				return FALSE;
			}			

			GLGuidance *pGuid = m_pGuidance->Find ( pLandMan->m_dwClubMapID );
			if ( pGuid )
			{

				// 선도전 일때 길트배틀 중이거나 종료시간 미달이면 출입금지
				std::tr1::shared_ptr<GLClubField> pClub = m_pClubMan->GetClub ( pPC->m_ClubDbNum );
				if ( pClub && pGuid->m_bBattle && 
					( pClub->GetAllBattleNum() > 0 ||
					 !pClub->IsGuidBattle() ) )
				{
					NetMsgFB.emFB = EMCHAR_GATEOUT_CLUBBATTLE;
					SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
					return FALSE;
				}


				if ( !pGuid->m_bBattle && !pGuid->IsGuidClub(pPC->m_ClubDbNum) )
				{
					std::tr1::shared_ptr<GLClubField> pGuidanceClub = m_pClubMan->GetClub ( pGuid->m_dwGuidanceClub );
					if ( pGuidanceClub )
					{
						// 선도클럽의 동맹이 아니면
						if ( !pClub || !pGuidanceClub->IsAllianceGuild_COMMON ( pClub->m_DbNum, pClub->m_dwAlliance ) )
						{
							NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
							SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
							return FALSE;
						}
					}
				}
			}
		}

		if (pMapNode->IsClubDeathMatchZone())
		{
			if (GLCONST_CHAR::bPKLESS) 
			{
				// 선도전 맵은 절대 못 들어간다...
				NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
				SENDTOCLIENT(pPC->ClientSlot(), &NetMsgFB);
				return FALSE;
			}			

			GLClubDeathMatch *pCDM = m_pDeathMatchMan->Find(pLandMan->m_dwClubMapID);
			if ( pCDM )
			{
				std::tr1::shared_ptr<GLClubField> pClub = m_pClubMan->GetClub ( pPC->m_ClubDbNum );
				if ( !pCDM->IsEnterMap( pPC->m_ClubDbNum ) || !pClub )
				{
					NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
					SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
					return FALSE;
				}	
								
				if ( pClub )
				{
					// 클럽배틀 진행중일때는 못들어간다.
                    if ( pClub->GetAllBattleNum() > 0 )
					{
						NetMsgFB.emFB = EMCHAR_GATEOUT_CLUBBATTLE2;
						SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
						return FALSE;
					}

					if ( !pClub->IsCDMEnter(pPC->m_CharDbNum) )
					{
						NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
						SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
						return FALSE;
					}
				}
			}
		}
	}
	
	if ( pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_NEW_INSTANCE )
	{
		const DWORD dbNum(pPC->GetCharID());
		if ( m_pInstanceManager->requestJoin(dbNum, mapIDTarget, dwGateID) == false )
			return FALSE;
	}

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pPC->IsActState(EM_ACT_MACROMODE) )
	{
		pPC->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT ( pPC->ClientSlot(), &msg );
	}

	// 에뮬레이터일 경우에는 Agent 가 없으므로 이곳에서 이동시킨다;
	if ( IsEmulatorMode() )
	{
		MoveSystem::SMOVE_INFO sMoveInfo;
		sMoveInfo.dwChaDBNum = pPC->CharDbNum();
		sMoveInfo.sMapID = mapIDTarget;
		sMoveInfo.dwGateID = pLandGate->GetToGateID( pNetMsg->dwGateToIndex );;

		MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );
	}
	
	NetMsg.dwGateToIndex = pNetMsg->dwGateToIndex;
	NetMsg.dwGateID = dwGateID;
	NetMsg.dwGaeaID = dwGaeaID;
	SENDTOAGENT(&NetMsg);

	return TRUE;

_GateOutError:
	if( pPC )
	{
		sc::writeLogDebug(sc::string::format( "Instant Map MoveOut Failed Name[%1%], CurrentMapID[%2%/%3%], GateID[%4%], MoveMapID[%5%/%6%], FailID[%7%]", 
							pPC->m_szName, pPC->GetCurrentMap().getGaeaMapID().Mid(), pPC->GetCurrentMap().getGaeaMapID().Sid(), pNetMsg->dwGateID, mapIDTarget.wMainID, mapIDTarget.wSubID, dwMoveFailed ) );
	}
	NetMsgFB.emFB = EMCHAR_GATEOUT_FAIL;
	SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );
	return FALSE;

}

BOOL GLGaeaServer::MsgReqReBirth(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	// Checking NULL Pointer;
	if( !nmg )
	{
		sc::writeLogError( "Function GLGaeaServer::MsgReqReBirth() : Packet is NULL!" );
		return FALSE;
	}

	GLMSG::NET_CHAR_RESURRECT_FAFC* pMsg =
		static_cast<GLMSG::NET_CHAR_RESURRECT_FAFC*>( nmg );

	GLChar* const pChar = GetCharByDbNum( pMsg->dwChaNum );
	if (!pChar)
	{
		sc::writeLogError( "Function GLAgentServer::MsgReqReBirth() : Char is NULL!" );
		return FALSE;
	}

	// PET
	// 맵 이동시 Pet 삭제;
	ReserveDropOutPet ( SDROPOUTPETINFO(pChar->m_dwPetGUID, true, true) );

	// VEHICLE
	SaveVehicle( pChar->GETCLIENTID(), pChar->GetGaeaID(), true );

	// SUMMON
	pChar->RemoveAllChildCrow();
	//pChar->SummonAllRemove();    

	SENDTOCLIENT( pChar->ClientSlot(), pMsg );
    return TRUE;
}

BOOL GLGaeaServer::MsgResurrectMoveMap(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    /*if ( dwGaeaID != GAEAID_NULL )
    	return FALSE;

    GLMSG::NET_CHAR_RESURRECT_MOVEMAP_AF* pNetMsg = (GLMSG::NET_CHAR_RESURRECT_MOVEMAP_AF*)nmg;
    GLChar* pPC = GetCharByDbNum(pNetMsg->dwChaNum);
    if ( !pPC )
        return FALSE;

    // PET
    // 맵이동시 Pet 삭제
    ReserveDropOutPet ( SDROPOUTPETINFO(pPC->m_dwPetGUID,true,true) );

    //VEHICLE
    SaveVehicle( pPC->GETCLIENTID(), pPC->GetGaeaID(), true );

    // SUMMON
    //pPC->SummonAllRemove();  
	pPC->RemoveAllChildCrow();

    // 현재 위치를 마지막 귀환 위치로 저장.;
    pPC->SaveLastCall ();
	
	// 마지막으로 살린후 이동 시킴;
	pPC->SetAlive(pNetMsg->fRestore);

    m_pMsgServer->ResetGaeaID(pPC->ClientSlot());

    // Agent으로 처리 완료 알림, actor에 예약함. ( new 된 객체는 db actor 에서 삭제된다. );
    GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FB NetMsg(pPC->CharDbNum(), pNetMsg->sMapID, pNetMsg->dwGateID);
    //	케릭터 삭제 요청.;
    ReserveDropOutPC(
        pPC->GetGaeaID(),
        db::DbActionPtr(
        new db::CharacterDbToAgentMessage(dwClientID, (NET_MSG_GENERIC*) &NetMsg)));*/

    return TRUE;
}

//CAF
BOOL GLGaeaServer::RequestLandIn(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg)
{
	// Agent 를 통해 릴레이된 메시지이므로, GaeaID 는 유효하지 않다;
	if ( ( dwGaeaID != GAEAID_NULL ) && ( m_bEmulator == false ) )
	{
		return FALSE;
	}

	if ( false == MoveSystem::CMoveManagerField::Instance()->OnCompleteMove(
		static_cast< GLMSG::SNETREQ_LANDIN* >( pMsg ) ) )
	{
		return FALSE;
	}

	/*GLChar* pPC = GetChar64(msg->dwChaNum, msg->dwSvrGroupNum);

	if(!pPC)
		return FALSE;

	if ( !pPC->IsActState(EM_GETVA_AFTER) )
	{
		//	Note : 캐릭터 주변의 정보를 인지.
		//
		pPC->GetViewAround ();

		//
		//mjeon.activity
		//
		//When a character moves into other FieldServer, it may take some time to load activity. 
		// At the time, this ActivityCheckMap() can be failed because ActivityLoad() did not completed yet. 
		// But in this case, CheckAfterLoad() will call CheckMap() inside it. Just ignore this failure.
		//
		pPC->ActivityCheckMap();

		//
		//mjeon.CaptureTheField
		//
		// Recall 처리가 완료되는 시점, 즉, 타겟 Field로의 이동이 완료되는 시점에 특별히 처리 해야할 사항이 있다면, 여기에서 하도록 하자.
		//
		pPC->ToDoRightAfterRecallDone();

        pPC->m_mapIDPrevious = pPC->GetCurrentMap();
		pPC->m_tLoginTime = msg->tLoginTime;

		// agent로 성공 메세지 송신;		
 		GLMSG::NET_INSTANCE_MOVE_DONE _messageMoveDone(pPC->GetCurrentMap(), pPC->CharDbNum());
 		SENDTOAGENT(&_messageMoveDone);
		
	}
	else
	{
        sc::writeLogError(
            sc::string::format(
			"RequestLandIn Call this function after receive view around message already. ChaName %1% ChaDbNum %2% DoActWait Prev %3%, Cur %4%",
                pPC->m_szName,
                pPC->CharDbNum(),
				msg->m_nDoActWaitTypePrev,
				msg->m_nDoActWaitType ));
	}*/

	return TRUE;
}

//CAF
BOOL GLGaeaServer::RequestTrade(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

	if ( m_dwAgentSlot != dwClientID )
	{
		return FALSE;
	}

	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		sc::writeLogError( "The WorldBattle Server don't trade." );

		return FALSE;
	}

	GLMSG::SNET_TRADE* pPacket = (GLMSG::SNET_TRADE*) nmg;
    if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, sizeof(GLMSG::SNET_TRADE)-sizeof(unsigned int)))
    {
        sc::writeLogError(
            std::string("RequestTrade GLMSG::SNET_TRADE CRC"));
        return FALSE;
    }

	//GLChar* pMyChar = GetChar(dwGaeaID);
	GLChar* pMyChar = GetCharByDbNum(pPacket->dwChaNum);
	if (!pMyChar)
        return FALSE;
	
	if ( GLHIDESET::CheckHideSet(EMHIDE_DEAL_REQUET_SEND) )		
	{
		pMyChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pMyChar->RestoreActStateByInvisible();
	}

	GLChar* pTarChar = GetChar(pPacket->dwTargetID);
	if (!pTarChar)
        return FALSE;

	// 거리 체크
	float fDistance = D3DXVec3Length ( &D3DXVECTOR3( pMyChar->GetPosition() - pTarChar->GetPosition() ) );
	float fTradeRange = static_cast< float >( pTarChar->GETBODYRADIUS() + pMyChar->GETBODYRADIUS() + 30 );
	float fTradeableDistance = fTradeRange + 15.0f;

	if ( fDistance > fTradeableDistance ) 
	{
		return FALSE;
	}

	//	Note : 이미 트래이드 중이라면 취소.
	if (pMyChar->m_sTrade.Valid())
        return FALSE;
	if (pTarChar->m_sTrade.Valid())
        return FALSE;

	if (pMyChar->VehicleState())
        return FALSE;
	if (pTarChar->VehicleState())
        return FALSE;

    // CTF 참여중이면 취소
    if (pMyChar->IsCTFPlayer())
        return FALSE;

	// 상대편에게 트래이드 시도 승낙 질의
	GLMSG::SNET_TRADE_TAR NetMsg;
	NetMsg.dwChaNum = pTarChar->CharDbNum();
	NetMsg.dwMasterID = pMyChar->GetGaeaID();//dwGaeaID;
	StringCchCopy( NetMsg.szMasterName, CHAR_SZNAME, pMyChar->GetName() );
    NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNET_TRADE_TAR)-sizeof(unsigned int));
	SENDTOAGENT(&NetMsg);	//pChar->ClientSlot()

	/*if( pMyChar->m_bTracingUser || pTarChar->m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pMyChar->GetUserID();
		TracingMsg.SetAccount(pMyChar->m_szUID);

		CString strTemp;
		strTemp.Format(
			"Trade Request, [%s][%s], Target Char:[%s][%s]", 
			pMyChar->m_szUID,
			pMyChar->m_szName,
			pTarChar->m_szUID,
			pTarChar->m_szName);

		TracingMsg.SetLog(strTemp.GetString());
		SENDTOAGENT(&TracingMsg);
	}*/

	return TRUE;
}

//CAF
BOOL GLGaeaServer::RequestTradeTarAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    if ( m_dwAgentSlot != dwClientID || dwGaeaID != GAEAID_NULL )
        return FALSE;

	GLMSG::SNET_TRADE_TAR_ANS* pPacket = (GLMSG::SNET_TRADE_TAR_ANS*) nmg;
    if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, sizeof(GLMSG::SNET_TRADE_TAR_ANS)-sizeof(unsigned int)))
    {
        sc::writeLogError(
            std::string("RequestTradeTarAns CRC"));
        return FALSE;
    }

	//GLChar* pMyChar = GetChar(dwGaeaID);
	GLChar* pMyChar = GetCharByDbNum(pPacket->dwChaNum);
	if (!pMyChar)
        return FALSE;

	GLChar* pMasterChar = GetChar ( pPacket->dwMasterID );
	if (!pMasterChar)
        return FALSE;

	// 거리 체크
	float fDistance = D3DXVec3Length ( &D3DXVECTOR3( pMyChar->GetPosition() - pMasterChar->GetPosition() ) );
	float fTradeRange = static_cast< float >( pMasterChar->GETBODYRADIUS() + pMyChar->GETBODYRADIUS() + 30 );
	float fTradeableDistance = fTradeRange + 15.0f;

	if ( fDistance > fTradeableDistance ) 
	{
		return FALSE;
	}

	//	Note : 이미 트래이드 중이라면 취소.
	if ( pMyChar->m_sTrade.Valid() )
        return FALSE;
	if ( pMasterChar->m_sTrade.Valid() )
        return FALSE;

	if ( pPacket->emAns == EMTRADE_CANCEL )
	{
		// 트래이드 시도 거부 MSG
		GLMSG::SNET_TRADE_FB NetMsgFB;
		NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
		NetMsgFB.emAns = EMTRADE_CANCEL;
		StringCchCopy( NetMsgFB.szMasterName, CHAR_SZNAME, pMyChar->GetName() );
        NetMsgFB.m_Crc32 = sc::string::getCrc32(&NetMsgFB, sizeof(GLMSG::SNET_TRADE_FB)-sizeof(unsigned int));
		NetMsgFB.dwChaNum = pMasterChar->CharDbNum();
		SENDTOAGENT(&NetMsgFB);	//pMasterChar->ClientSlot()
		return FALSE;
	}

	if ( GLHIDESET::CheckHideSet(EMHIDE_DEAL_ACCEPT_TARGET) )
	{
		pMyChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pMyChar->RestoreActStateByInvisible();
	}


	if ( GLHIDESET::CheckHideSet(EMHIDE_DEAL_ACCEPT_SELF) )
	{
		pMasterChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pMasterChar->RestoreActStateByInvisible();
	}

	// 트래이드 시작
	pMyChar->m_sTrade.Reset();
	pMyChar->m_sTrade.SetTarget ( pMasterChar->GetGaeaID() );

	pMasterChar->m_sTrade.Reset();
	pMasterChar->m_sTrade.SetTarget ( pMyChar->GetGaeaID() );

	// 트래이드 시작 MSG
	{
		GLMSG::SNET_TRADE_FB NetMsgFB;
		NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
		NetMsgFB.emAns = EMTRADE_OK;
		NetMsgFB.dwChaNum = pMasterChar->CharDbNum();
		StringCchCopy( NetMsgFB.szMasterName, CHAR_SZNAME, pMyChar->GetName() );
        NetMsgFB.m_Crc32 = sc::string::getCrc32(&NetMsgFB, sizeof(GLMSG::SNET_TRADE_FB)-sizeof(unsigned int));		
		SENDTOAGENT(&NetMsgFB);	//pMasterChar->ClientSlot()
	}

	{
		GLMSG::SNET_TRADE_FB NetMsgFB;
		NetMsgFB.dwTargetID = pPacket->dwMasterID;
		NetMsgFB.emAns = EMTRADE_OK;
		StringCchCopy( NetMsgFB.szMasterName, CHAR_SZNAME, pMasterChar->GetName() );
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.m_Crc32 = sc::string::getCrc32(&NetMsgFB, sizeof(GLMSG::SNET_TRADE_FB)-sizeof(unsigned int));		
		SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()
	}

	/*if( pMyChar->m_bTracingUser || pMasterChar->m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pMyChar->GetUserID();
		TracingMsg.SetAccount(pMyChar->m_szUID);

		CString strTemp;
		strTemp.Format(
			"Trade Answer OK, [%s][%s], Master Char: [%s][%s]", 
			pMyChar->m_szUID,
			pMyChar->m_szName,
			pMasterChar->m_szUID,
			pMasterChar->m_szName);
		TracingMsg.SetLog(strTemp.GetString());
		SENDTOAGENT(&TracingMsg);
	}*/

	return TRUE;
}


BOOL GLGaeaServer::RequestTradeMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

	GLMSG::SNET_TRADE_MONEY* pPacket = (GLMSG::SNET_TRADE_MONEY*) nmg;
    
	GLChar* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return FALSE;
	if (!pMyChar->m_sTrade.Valid())
        return FALSE;

    if (pPacket->m_MagicNum != GLMSG::SNET_TRADE_MONEY::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! RequestTradeMoney. CharDbNum %1% MagicNum %2%",
                pMyChar->CharDbNum(),
                pPacket->m_MagicNum));
        return FALSE;
    }

	// 거래창 잠금상태 확인: 잠겨있다면 거래창에 등록할 수 없다.
	if ( pMyChar->m_sTrade.IsLock() )
	{
		sc::writeLogError(
			sc::string::format(
			"HACKING! RequestTradeMoney. Trade is lock, but RequestTradeMoney. CharDbNum %1%",
			pMyChar->CharDbNum() ) );
		return FALSE;
	}

    if (pPacket->lnMoney < 0)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! SNET_TRADE_MONEY MONEY ERROR. CharDbNum %1%", pMyChar->CharDbNum()));
        return FALSE;
    }

    if (pPacket->lnMoney > pMyChar->GetInvenMoney())
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! SNET_TRADE_MONEY MONEY ERROR. CharDbNum %1% %2%>%3%",
                pMyChar->CharDbNum(),
                pPacket->lnMoney,
                pMyChar->GetInvenMoney()));
        return FALSE;
    }

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	GLChar* pTarChar = GetChar ( dwTargetID );
	if (!pTarChar || pTarChar->m_sTrade.GetTarget() != dwGaeaID)
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		SENDTOAGENT(&NetMsg);	// pMyChar->ClientSlot(), 
		return FALSE;
	}
	
//#if defined ( TW_PARAM ) || defined ( HK_PARAM ) 
	if (m_ServiceProvider == SP_HONGKONG)
	{
		if ( pTarChar->m_wLevel == 1 )
		{
			if( (pTarChar->GetInvenMoney() + pPacket->lnMoney) > 50000 )
			{
				sc::writeLogWarn(
                    sc::string::format("Target(Level1) too many money!! TarMoney %1% TarID %2% TarUserNum %3% MyUserNum %4% MyID %5% MyUserNum %6% TradeMoney %7%",
					    pTarChar->GetInvenMoney(),
                        pTarChar->m_szName,
                        pTarChar->GetUserID(), 
					    pMyChar->GetInvenMoney(),
                        pMyChar->m_szName,
                        pMyChar->GetUserID(),
                        pPacket->lnMoney));
				return FALSE;
			}
		}
//#endif
	}

	pMyChar->m_sTrade.SetMoney(pPacket->lnMoney);
	
	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	// 거래 잠금 해제.
	pMyChar->m_sTrade.SetLock( FALSE );
	pTarChar->m_sTrade.SetLock( FALSE );

	//	Note : 금액 변화 알림. MSG
	//
	GLMSG::SNET_TRADE_MONEY_TAR NetMsgMoneyTar(dwGaeaID, pPacket->lnMoney);
	SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgMoneyTar);
	SENDTOCLIENT(pTarChar->ClientSlot(), &NetMsgMoneyTar);
	return TRUE;
}

BOOL GLGaeaServer::RequestTradeItemResist(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

	GLMSG::SNET_TRADE_ITEM_REGIST *pNetMsg = (GLMSG::SNET_TRADE_ITEM_REGIST *)nmg;

	GLChar* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
        return FALSE;
	if ( !pMyChar->m_sTrade.Valid() )
        return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	GLChar* pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()

		return FALSE;
	}

	// 거래창 잠금상태 확인: 잠겨있다면 거래창에 등록할 수 없다.
	if ( pMyChar->m_sTrade.IsLock() )
	{
		sc::writeLogError(
			sc::string::format(
			"HACKING! RequestTradeItemResist. Trade is lock, but RequestTradeItemResist. CharDbNum %1%",
			pMyChar->CharDbNum() ) );
		return FALSE;
	}

	SINVENITEM *pInvenItem = pMyChar->InvenGetItem( pNetMsg->wInvenX, pNetMsg->wInvenY );
	if ( !pInvenItem )
        return FALSE;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if (!pItemData)
        return false;

	// 팻카드이면서 팻이 활성화 되어 있으면 거래할 수 없다.
	if ( pItemData->BasicType() == ITEM_PET_CARD )
	{
		const GLPetField* pMyPet = GetPET ( pMyChar->m_dwPetGUID );
		if (pMyPet && pMyPet->IsValid () && pInvenItem->CustomItem().PetDbNum() == pMyPet->DbNum())
            return S_FALSE;
	}

	// 탈것이면 거래할수 없다.
	if (pItemData->BasicType() == ITEM_VEHICLE && pInvenItem->VehicleDbNum() != 0)
	{
		return S_FALSE;
	}

	//	거래옵션
	//if ( !pItemData->CanExchange() )
	if ( !( pInvenItem->sItemCustom.IsExchange() != NULL ) )
        return FALSE;
	
	// GMCharEdit 로 넣은 아이템은 버리기 불가능하다. 이름이 대소문자만 달라서 일관성이 없고 검색에 문제가 있어 이름 같게 맞춤
	if ( pInvenItem->IsGM_GENITEM() )
        return FALSE;

	BOOL bOk = pMyChar->m_sTrade.InsertItem( pNetMsg->wPosX, pNetMsg->wPosY, pInvenItem );
	if (!bOk)
        return FALSE;

	SINVENITEM* pInven = pMyChar->m_sTrade.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if (!pInven)
        return FALSE;

	// #item 이건 뭐지?
	// 팻 생성여부 확인
	if ( pMyChar->m_CharDbNum <= 0 )
	{
		sc::writeLogError(
            sc::string::format(
                "Invalid Trade CharacterID PetID %d CharID %d dwClientID %d",
                pInvenItem->sItemCustom.m_PetDbNum,
                pMyChar->m_CharDbNum,
                pTarChar->ClientSlot()));
		return FALSE;
	}


	// 팻카드이면 팻카드 정보를 상대방에게 알려준다.
	if ( pItemData->BasicType() == ITEM_PET_CARD && pInvenItem->sItemCustom.m_PetDbNum != 0 )
	{
		GLPetField* pPetInfo = new GLPetField(this);

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
        AddGameAdoJob(
            db::DbActionPtr(
                new db::PetGet(
                    pPetInfo,
                    pInvenItem->sItemCustom.m_PetDbNum,
                    pTarChar->ClientSlot(),
                    pMyChar->m_CharDbNum, 
                    pNetMsg->wInvenX,
                    pNetMsg->wInvenY,
                    false,
                    false,
                    true)));
	}

	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	// 거래 잠금 해제.
	//pMyChar->m_sTrade.SetLock( FALSE );
	//pTarChar->m_sTrade.SetLock( FALSE );

	//	Note : 아이템 변화 알림. MSG.
	//
	GLMSG::SNET_TRADE_ITEM_REGIST_TAR NetRegistTar;
	NetRegistTar.dwGaeaID = dwGaeaID;
	NetRegistTar.sInsert = *pInven;

	NetRegistTar.dwChaNum = pMyChar->CharDbNum();
	SENDTOAGENT(&NetRegistTar);	//pMyChar->ClientSlot()

	NetRegistTar.dwChaNum = pTarChar->CharDbNum();
	SENDTOAGENT(&NetRegistTar);	//pTarChar->ClientSlot()

	return TRUE;
}


BOOL GLGaeaServer::RequestTradeItemReMove(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLMSG::SNET_TRADE_ITEM_REMOVE *pNetMsg = (GLMSG::SNET_TRADE_ITEM_REMOVE *)nmg;

	GLChar* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;

	if ( !pMyChar->m_sTrade.Valid() )
		return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	GLChar* pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()

		return FALSE;
	}

	// 거래창 잠금상태 확인: 잠겨있다면 거래창에 등록할 수 없다.
	if ( pMyChar->m_sTrade.IsLock() )
	{
		sc::writeLogError(
			sc::string::format(
			"HACKING! RequestTradeItemReMove. Trade is lock, but RequestTradeItemReMove. CharDbNum %1%",
			pMyChar->CharDbNum() ) );
		return FALSE;
	}

	SINVENITEM* pInven = pMyChar->m_sTrade.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInven )
		return FALSE;

	//	Note : 거래 목록에서 아이템 삭제.
	pMyChar->m_sTrade.DeleteItem( pNetMsg->wPosX, pNetMsg->wPosY );

	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	// 거래 잠금 해제.
	pMyChar->m_sTrade.SetLock( FALSE );
	pTarChar->m_sTrade.SetLock( FALSE );

	//	Note : 아이템 변화 알림. MSG.
	//
	GLMSG::SNET_TRADE_ITEM_REMOVE_TAR NetReMoveTar;
	NetReMoveTar.dwGaeaID = dwGaeaID;
	NetReMoveTar.wPosX = pNetMsg->wPosX;
	NetReMoveTar.wPosY = pNetMsg->wPosY;

	NetReMoveTar.dwChaNum = pMyChar->CharDbNum();
	SENDTOAGENT(&NetReMoveTar);	//pMyChar->ClientSlot()

	NetReMoveTar.dwChaNum = pTarChar->CharDbNum();
	SENDTOAGENT(&NetReMoveTar);	//pTarChar->ClientSlot()

	return TRUE;
}


BOOL GLGaeaServer::RequestTradeLock( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if (dwGaeaID == GAEAID_NULL)
        return FALSE;

	GLMSG::SNET_TRADE_LOCK* pPacket = (GLMSG::SNET_TRADE_LOCK*) nmg;
    if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, sizeof(GLMSG::SNET_TRADE_LOCK)-sizeof(unsigned int)))
    {
        sc::writeLogError(
            std::string("RequestTradeLock CRC"));
        return FALSE;
    }

	GLChar* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return FALSE;
	if (!pMyChar->m_sTrade.Valid())
        return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	GLChar* pTarChar = GetChar ( dwTargetID );

	if ( !pTarChar || pTarChar->m_sTrade.GetTarget()!=dwGaeaID )
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()

		return FALSE;
	}

	//	승인 블럭.
	pMyChar->m_sTrade.BlockAgree ();
	pTarChar->m_sTrade.BlockAgree ();

	// 거래창 잠금.
	pMyChar->m_sTrade.SetLock( TRUE );

	GLMSG::SNET_TRADE_LOCK_TAR pNetTradeLock;
	pNetTradeLock.dwGaeaID = pPacket->dwGaeaID;

	pNetTradeLock.dwChaNum = pMyChar->CharDbNum();
	SENDTOAGENT(&pNetTradeLock);	//pMyChar->ClientSlot()

	pNetTradeLock.dwChaNum = pTarChar->CharDbNum();
	SENDTOAGENT(&pNetTradeLock);	//pTarChar->ClientSlot()

	return TRUE;
}


BOOL GLGaeaServer::RequestTradeAgree(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if (dwGaeaID == GAEAID_NULL)
        return FALSE;

	GLMSG::SNET_TRADE_AGREE* pNetMsg = (GLMSG::SNET_TRADE_AGREE*) nmg;
    if (pNetMsg->m_Crc32 != sc::string::getCrc32(pNetMsg, sizeof(GLMSG::SNET_TRADE_AGREE)-sizeof(unsigned int)))
    {
        sc::writeLogError(
            std::string("RequestTradeAgree CRC"));
        return FALSE;
    }

	GLChar* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return FALSE;
	if (!pMyChar->m_sTrade.Valid())
        return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	GLChar* pTarChar = GetChar(dwTargetID);

	if (!pTarChar || pTarChar->m_sTrade.GetTarget() != dwGaeaID)
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()
		return FALSE;
	}

	// 거래창 잠금상태 확인: 잠겨있지 않다면 승인할 수 없다.
	if ( FALSE == pMyChar->m_sTrade.IsLock() )
	{
		sc::writeLogError(
			sc::string::format(
			"HACKING! RequestTradeAgree. Trade is lock, but RequestTradeAgree. CharDbNum %1%",
			pMyChar->CharDbNum() ) );
		return FALSE;
	}

    // Money Check
    LONGLONG TradeMoneyA = pMyChar->m_sTrade.GetMoney();
    LONGLONG TradeMoneyB = pTarChar->m_sTrade.GetMoney();
    if (TradeMoneyA < 0 || TradeMoneyB < 0)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! RequestTradeAgree Trade Money %1% %2% My CharDbNum %3% Tar CharDbNum %4%",
                TradeMoneyA,
                TradeMoneyB,
                pMyChar->CharDbNum(),
                pTarChar->CharDbNum()));
        GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
        SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()

		NetMsg.dwChaNum = pTarChar->CharDbNum();
        SENDTOAGENT(&NetMsg);	//pTarChar->ClientSlot()
        return FALSE;
    }

    if (pMyChar->GetInvenMoney() < TradeMoneyA)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! RequestTradeAgree Trade Money %1% < %2% CharDbNum %3%",
                pMyChar->GetInvenMoney(),
                TradeMoneyA,
                pMyChar->CharDbNum()));
        GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
        SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()

		NetMsg.dwChaNum = pTarChar->CharDbNum();
        SENDTOAGENT(&NetMsg);	//pTarChar->ClientSlot()
        return FALSE;
    }

    if (pTarChar->GetInvenMoney() < TradeMoneyB)
    {
        sc::writeLogError(
            sc::string::format(
            "HACKING! RequestTradeAgree Trade Money %1% < %2% CharDbNum %3%",
            pTarChar->GetInvenMoney(),
            TradeMoneyB,
            pTarChar->CharDbNum()));
        GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
        SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()

		NetMsg.dwChaNum = pTarChar->CharDbNum();
        SENDTOAGENT(&NetMsg);	//pTarChar->ClientSlot()
        return FALSE;
    }

	//	승인 시도.
	BOOL bOk = pMyChar->m_sTrade.SetAgree();
	if (!bOk)
	{
		//	승인 여부 결과 MSG.
		GLMSG::SNET_TRADE_AGREE_TAR NetMsgAgreeTar;
		NetMsgAgreeTar.dwGaeaID = dwGaeaID;
		NetMsgAgreeTar.emFB = EMTRADE_AGREE_TIME;
		NetMsgAgreeTar.dwChaNum = pMyChar->CharDbNum();
        NetMsgAgreeTar.m_Crc32 = sc::string::getCrc32(&NetMsgAgreeTar, sizeof(GLMSG::SNET_TRADE_AGREE_TAR)-sizeof(unsigned int));		
		SENDTOAGENT(&NetMsgAgreeTar);	//pMyChar->ClientSlot()
		return FALSE;
	}

	if ( pMyChar->m_sTrade.GetAgree() && pTarChar->m_sTrade.GetAgree() )
	{
		
		EMTRADE_CANCEL_TAR emCancelFB = EMTRADE_CANCEL_NORMAL;

		do
		{
			//	Note : 거래 목록이 유효한지 검사.
			//
			if ( !pMyChar->IsVaildTradeData() || !pTarChar->IsVaildTradeData() )
			{
				break;
			}

			//	Note : 물품 교환 공간이 인밴에 충분한지 검사.
			//
			BOOL bOk;
			bOk = pMyChar->IsVaildTradeInvenSpace( pTarChar->m_sTrade.GetTradeBox() );
			if ( !bOk )
			{
				emCancelFB = EMTRADE_CANCEL_MYINVEN;
				break;
			}

			bOk = pTarChar->IsVaildTradeInvenSpace( pMyChar->m_sTrade.GetTradeBox() );
			if ( !bOk )
			{
				emCancelFB = EMTRADE_CANCEL_TARINVEN;
				break;
			}

			//	Note : 거래 수행.
			//
			bOk = pMyChar->DoTrade( pTarChar->m_sTrade, pTarChar->m_CharDbNum );
			if ( !bOk )
            {
                emCancelFB = EMTRADE_CANCEL_NORMAL;

                //	Note : 거래 정보 리셋.
                //
                pMyChar->m_sTrade.Reset();
                pTarChar->m_sTrade.Reset();

                //	Note : 거래 취소됨을 알림. ( dwGaeaID 에게. )
                //
                {
                    GLMSG::SNET_TRADE_CANCEL_TAR NetMsgCancelTar;
                    NetMsgCancelTar.dwChaNum = pMyChar->CharDbNum();
                    NetMsgCancelTar.emCancel = emCancelFB;
                    SENDTOAGENT(&NetMsgCancelTar);	//pMyChar->ClientSlot()
                }

                //	Note : 거래 취소됨을 알림. ( dwTargetID 에게. )
                //
                {
                    if ( emCancelFB==EMTRADE_CANCEL_MYINVEN || emCancelFB==EMTRADE_CANCEL_TARINVEN )
                    {
                        emCancelFB = ( emCancelFB==EMTRADE_CANCEL_MYINVEN ? EMTRADE_CANCEL_TARINVEN : EMTRADE_CANCEL_MYINVEN );
                    }

                    GLMSG::SNET_TRADE_CANCEL_TAR NetMsgCancelTar;
                    NetMsgCancelTar.dwChaNum = pTarChar->CharDbNum();
                    NetMsgCancelTar.emCancel = emCancelFB;
                    SENDTOAGENT(&NetMsgCancelTar);	//pTarChar->ClientSlot()
                }

                return FALSE;
            }

			//	Note : 거래 정보 리셋.
			//
			pMyChar->m_sTrade.Reset();
			pTarChar->m_sTrade.Reset();

			//	Note : 거래 성공.
			//
			GLMSG::SNET_TRADE_COMPLETE_TAR NetMsgComplete;

			NetMsgComplete.dwChaNum = pMyChar->CharDbNum();
			SENDTOAGENT(&NetMsgComplete);	//pMyChar->ClientSlot()

			NetMsgComplete.dwChaNum = pTarChar->CharDbNum();
			SENDTOAGENT(&NetMsgComplete);	//pTarChar->ClientSlot()

			/*
			이제 필요없음
			if ( m_ServiceProvider == SP_OFFICE_TEST )
			{
				// 1.3에는 직접 merge가 되어 있고, 1.0은 테스트서버에만 적용
				// 게임머니 및 인벤토리 db에 저장한다. crm http://crm.mincoms.com/Ticket/TicketView/488 필리핀아이템 복사 대응
				// 1.3은 날쿼리문으로 거의 되어있어서 디비작업에 시간이 걸릴 수 있다.
				// 거래를 하고 바로 나가버리는 경우 약간 문제가 있을 수 있다. 빨리 1.4로 업데이트 하길
				AddGameAdoJob(
					db::DbActionPtr(
					new db::CharacterInventoryAndGameMoneySave( pMyChar ) ) );

				AddGameAdoJob(
					db::DbActionPtr(
					new db::CharacterInventoryAndGameMoneySave( pTarChar ) ) );	
			}
			*/

			return TRUE;
		}
		while( 0 );

		//	Note : 거래 정보 리셋.
		//
		pMyChar->m_sTrade.Reset();
		pTarChar->m_sTrade.Reset();

		//	Note : 거래 취소됨을 알림. ( dwGaeaID 에게. )
		//
		{
			GLMSG::SNET_TRADE_CANCEL_TAR NetMsgCancelTar;
			NetMsgCancelTar.dwChaNum = pMyChar->CharDbNum();
			NetMsgCancelTar.emCancel = emCancelFB;
			SENDTOAGENT(&NetMsgCancelTar);	//pMyChar->ClientSlot()
		}

		//	Note : 거래 취소됨을 알림. ( dwTargetID 에게. )
		//
		{
			if ( emCancelFB==EMTRADE_CANCEL_MYINVEN || emCancelFB==EMTRADE_CANCEL_TARINVEN )
			{
				emCancelFB = ( emCancelFB==EMTRADE_CANCEL_MYINVEN ? EMTRADE_CANCEL_TARINVEN : EMTRADE_CANCEL_MYINVEN );
			}

			GLMSG::SNET_TRADE_CANCEL_TAR NetMsgCancelTar;
			NetMsgCancelTar.dwChaNum = pTarChar->CharDbNum();
			NetMsgCancelTar.emCancel = emCancelFB;
			SENDTOAGENT(&NetMsgCancelTar);	//pTarChar->ClientSlot()
		}
	}
	else
	{
		//	승인 여부 결과 MSG.
		GLMSG::SNET_TRADE_AGREE_TAR NetMsgAgreeTar;
		NetMsgAgreeTar.dwGaeaID = dwGaeaID;
		NetMsgAgreeTar.emFB = EMTRADE_AGREE_OK;
		NetMsgAgreeTar.dwChaNum = pMyChar->CharDbNum();
		NetMsgAgreeTar.m_Crc32 = sc::string::getCrc32(&NetMsgAgreeTar, sizeof(GLMSG::SNET_TRADE_AGREE_TAR)-sizeof(unsigned int));		
		SENDTOAGENT(&NetMsgAgreeTar);	//pMyChar->ClientSlot()

		NetMsgAgreeTar.dwChaNum = pTarChar->CharDbNum();
		NetMsgAgreeTar.m_Crc32 = sc::string::getCrc32(&NetMsgAgreeTar, sizeof(GLMSG::SNET_TRADE_AGREE_TAR)-sizeof(unsigned int));
		SENDTOAGENT(&NetMsgAgreeTar);	//pTarChar->ClientSlot()
	}

	return TRUE;
}


//CAF
BOOL GLGaeaServer::RequestTradeCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());
	//if (dwGaeaID == GAEAID_NULL)
	//	return FALSE;

	GLMSG::SNET_TRADE_CANCEL *pNetMsg = (GLMSG::SNET_TRADE_CANCEL *)nmg;

	//GLChar* pMyChar = GetChar ( dwGaeaID );
	GLChar *pMyChar = GetCharByDbNum(pNetMsg->dwChaNum);

	if (!pMyChar)
        return FALSE;

	if (!pMyChar->m_sTrade.Valid())
        return FALSE;

	DWORD dwTargetID = pMyChar->m_sTrade.GetTarget();
	GLChar* pTarChar = GetChar(dwTargetID);
	if (!pTarChar || pTarChar->m_sTrade.GetTarget() != pMyChar->GetGaeaID())
	{
		//	Note : 트래이드 정보 리셋.
		pMyChar->m_sTrade.Reset ();

		//	Note : 트래이드 취소 클라이언트에 알림.
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()
		return FALSE;
	}

	//	거래 취소.
	pMyChar->m_sTrade.Reset();
	pTarChar->m_sTrade.Reset();

	{
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pMyChar->CharDbNum();
		NetMsg.emCancel = EMTRADE_CANCEL_NORMAL;
		SENDTOAGENT(&NetMsg);	//pMyChar->ClientSlot()
	}

	{
		GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
		NetMsg.dwChaNum = pTarChar->CharDbNum();
		NetMsg.emCancel = EMTRADE_CANCEL_OTHER;
		SENDTOAGENT(&NetMsg);	//pTarChar->ClientSlot()
	}

	return TRUE;
}


//AF
BOOL GLGaeaServer::RequestFieldSvrCharChk ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg  )
{
	if ( dwClientID != m_dwAgentSlot )
	{
		sc::writeLogError( sc::string::format( "RequestFieldSvrCharChk Wrong Try [IP:%s]", GetClientIP( dwClientID ) ) );
		return FALSE;
	}

	GLMSG::SNETPC_FIELDSVR_CHARCHK *pNetMsg = (GLMSG::SNETPC_FIELDSVR_CHARCHK *)nmg;
	bool bExist(false);

    // 혹시 user id 가 db 작업중인지 검사
    if (FindSaveDBUserID(pNetMsg->dwUserID) || FindCreatingCharUserID(pNetMsg->dwUserID))
    {
        // 에이젼트에 점검 FB
        GLMSG::SNETPC_FIELDSVR_CHARCHK_FB NetMsgFb;
        NetMsgFb.bExist = true;
        NetMsgFb.nChannel = m_nServerChannel;
        NetMsgFb.dwFIELDID = m_dwFieldSvrID;
        NetMsgFb.dwGaeaID = pNetMsg->dwGaeaID;
        StringCchCopy(NetMsgFb.szName, CHR_ID_LENGTH+1, pNetMsg->szName);
        SENDTOAGENT(&NetMsgFb);

        sc::writeLogError(
            sc::string::format(
            "Duplicate game join! RequestFieldSvrCharChk UserDbNum %1%", pNetMsg->dwUserID));

        return FALSE;
    }

	//	Note : 같은 이름의 캐릭터가 이미 접속되어 있는지 검사합니다.
	{
        GLChar* pChar = m_pCharMan->GetChaByName(pNetMsg->szName);
		if (pChar)
		{
			sc::writeLogError(
				sc::string::format(
				"RequestFieldSvrCharChk. GetChaByName. %1%, %2%", pNetMsg->dwUserID, pChar->CharDbNum()));

			bExist = true;

			//	종전 접속자에게 DropOut 메시지를 보냅니다.
			GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
			NetMsgForced.dwChaNum = pChar->CharDbNum();
			NetMsgForced.emForced = EMDROPOUT_ERROR;
			SENDTOAGENT(&NetMsgForced);	//pChar->ClientSlot()

			// 구인던일 경우 위치 백업 후 인던 밖으로 이동한다.
			/*const MapID _mapIDBackup(pChar->GetCurrentMap());
			const D3DXVECTOR3 _positionBackup(pChar->GetPosition());	
			const SMAPNODE* const pMapNode(FindMapNode(_mapIDBackup));
			const bool _bOldInstantMap((pMapNode != NULL) && (pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE));
			if ( _bOldInstantMap == true )
				SaveCheckInstantMap( pChar );

			//	Note : 현제 위치 저장.
			//
			pChar->SavePosition ();

			//	Note : 사망한 상태로 제거시 경험치 감소.
			//
			if ( pChar->IsActState(EM_ACT_DIE) )
			{
				//	Note : 부활시 경험치 감소.
				pChar->DoDeathPenaltyExp ();
			}

			if ( m_pDBMan )
			{
				// 케릭터 저장 요청.
				m_pCharMan->ReqSaveDbUserAdd( pChar->GetUserID() );

				GLMSG::SNETPC_MSG_CHA_SAVE_FA NetMsgFA( pChar->GetUserID() );
				SENDTOAGENT( &NetMsgFA );

                AddGameAdoJob(
                    db::DbActionPtr(
                        new db::CharacterSave( pChar ) ) );	//mjeon.ado
			}

			if ( _bOldInstantMap == true )
			{
				pChar->SetCurrentMap(_mapIDBackup);
				pChar->SetPosition(_positionBackup);	
			}*/	

			DWORD DropUserNum = pChar->GetUserID();

			//	Note : 케릭터가 지워진다.
			//
			DropOutPC(pChar);

			// 락해제가 되지않은 캐릭터 로그 확인 결과, 대부분 RequestFieldSvrCharChk 로그가 기록되어 있었다.
			// 지난번처럼 다시 유저락커 복사가 일어난다면 아래 코드 삭제한다. 
			// 이부분에서 락 풀어주도록 한다.
			// 유저락커 잠금 해제
			// [3/29/2017 khkim] 간혹 UserInvenOption이 1인 경우가 발생하는데, 이 부분에 발생하는 것으로 보인다.
			// 어떤 이유로 같은 gaeaid 가 있는 캐릭터가 발생하는지는 모르겠지만, 게임에 접속하려는 유저의 UserInvenOption 을 0으로 변경해주고 있다.
			// pChar 의 UserInvenOption 을 0으로 변경해야 한다.
			UpdateUserLockerOption( DropUserNum, 0 );/*UpdateUserLockerOption( pNetMsg->dwUserID, 0 );*/
		}
	}

	//	Note : 같은 Gaea id 가 접속중인지 검사.
	//
	if ( GetChar(pNetMsg->dwGaeaID) )
	{
		GLChar* pChar = GetChar(pNetMsg->dwGaeaID);

		if ( pChar )
		{
			sc::writeLogError(
				sc::string::format(
				"RequestFieldSvrCharChk. GetChar. %1%, %2%", pNetMsg->dwUserID, pChar->CharDbNum()));

			bExist = true;

			//	종전 접속자에게 DropOut 메시지를 보냅니다.
			GLMSG::SNET_DROP_OUT_FORCED NetMsgForced;
			NetMsgForced.dwChaNum = pChar->CharDbNum();
			NetMsgForced.emForced = EMDROPOUT_ERROR;
			SENDTOAGENT(&NetMsgForced);	//pChar->ClientSlot()

			// 구인던일 경우 위치 백업 후 인던 밖으로 이동한다.
			/*const MapID _mapIDBackup(pChar->GetCurrentMap());
			const D3DXVECTOR3 _positionBackup(pChar->GetPosition());	
			const SMAPNODE* const pMapNode(FindMapNode(_mapIDBackup));
			const bool _bOldInstantMap((pMapNode != NULL) && (pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE));
			if ( _bOldInstantMap == true )
				SaveCheckInstantMap( pChar );

			//	Note : 현제 위치 저장.
			//
			pChar->SavePosition ();

			//	Note : 사망한 상태로 제거시 경험치 감소.
			//
			if ( pChar->IsActState(EM_ACT_DIE) )
			{
				//	Note : 부활시 경험치 감소.
				pChar->DoDeathPenaltyExp ();
			}

			if ( m_pDBMan )
			{
				//	Note : 케릭터 정보 저장.
                m_pCharMan->ReqSaveDbUserAdd( pChar->GetUserID() );

				GLMSG::SNETPC_MSG_CHA_SAVE_FA NetMsgFA( pChar->GetUserID() );
				SENDTOAGENT( &NetMsgFA );

				AddGameAdoJob(
                    db::DbActionPtr(
                        new db::CharacterSave( pChar ) ) );	//mjeon.ado
			}

			if ( _bOldInstantMap == true )
			{
				pChar->SetCurrentMap(_mapIDBackup);
				pChar->SetPosition(_positionBackup);	
			}	*/

			DWORD DropUserNum = pChar->GetUserID();

			//	Note : 케릭터가 지워진다.
			//
			DropOutPC(pChar);

			// 락해제가 되지않은 캐릭터 로그 확인 결과, 대부분 RequestFieldSvrCharChk 로그가 기록되어 있었다.
			// 지난번처럼 다시 유저락커 복사가 일어난다면 아래 코드 삭제한다. 
			// 이부분에서 락 풀어주도록 한다.
			// 유저락커 잠금 해제
			// [3/29/2017 khkim] 간혹 UserInvenOption이 1인 경우가 발생하는데, 이 부분에 발생하는 것으로 보인다.
			// 어떤 이유로 같은 gaeaid 가 있는 캐릭터가 발생하는지는 모르겠지만, 게임에 접속하려는 유저의 UserInvenOption 을 0으로 변경해주고 있다.
			// pChar 의 UserInvenOption 을 0으로 변경해야 한다.
			UpdateUserLockerOption( DropUserNum, 0 );/*UpdateUserLockerOption( pNetMsg->dwUserID, 0 );*/
		}
	}

	//	Note : 혹시 user id 가 db 작업중인지 검사.
	if (m_pCharMan->ReqSaveDbUserFind(pNetMsg->dwUserID) || m_pCharMan->CreatingCharFind(pNetMsg->dwUserID))
	{
		bExist = true;
	}

	//	Note : 에이젼트에 점검 FB.
	//
	GLMSG::SNETPC_FIELDSVR_CHARCHK_FB NetMsgFb;
	NetMsgFb.bExist = bExist;
	NetMsgFb.nChannel = m_nServerChannel;
	NetMsgFb.dwFIELDID = m_dwFieldSvrID;
	NetMsgFb.dwGaeaID = pNetMsg->dwGaeaID;
	StringCchCopy ( NetMsgFb.szName, CHR_ID_LENGTH+1, pNetMsg->szName );

	SENDTOAGENT(&NetMsgFb);

	/*if ( bExist )
	{
		if ( m_pDBMan )
		{
			// DB 저장후에 메시지 처리하기 위해서 db action에 등록하여 메시지 전송.
			AddGameDbJob(
                db::DbActionPtr(
                    new db::CharacterDbToAgentMessage(dwClientID, (NET_MSG_GENERIC*) &NetMsgFb)));			
		}
	}
	else
	{
		//	Note : DB 작업이 없을 경우에는 즉시 전송.
		SENDTOAGENT(&NetMsgFb);	//dwClientID
	}*/

	return TRUE;
}


BOOL GLGaeaServer::ServerFieldInfoReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if( m_dwAgentSlot != dwClientID )
		return FALSE;

	GLMSG::SNET_FIELDINFO_RESET* pNetMsg = (GLMSG::SNET_FIELDINFO_RESET*)nmg;
	m_pPartyFieldMan->DelPartyAll();
	return TRUE;
}


BOOL GLGaeaServer::ServerSchoolFreePk(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (GLCONST_CHAR::bPKLESS)
        return FALSE;

	GLMSG::SNETPC_SCHOOLFREEPK_FLD *pNetMsg = (GLMSG::SNETPC_SCHOOLFREEPK_FLD *)nmg;
	SchoolFreePkSet(pNetMsg->bSCHOOL_FREEPK);
	
	GLMSG::SNETPC_SERVER_INFO NetMsgServerInfo;
	NetMsgServerInfo.bSCHOOL_FREEPK = SchoolFreePkOn();

	GLChar* pCHAR = NULL;
	
	//for ( DWORD i=0; i<m_dwMaxClient; ++i )
	UMAP_GLCHAR_ITER iter = m_mapCharacters.begin();
	for (; iter != m_mapCharacters.end(); ++iter)
	{
		pCHAR = iter->second;//GetChar(i);
		if (!pCHAR)
			continue;

		SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgServerInfo );
	}

	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlPeriod(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PERIOD *pNetMsg = (GLMSG::SNET_PERIOD *)nmg;
	GLPeriod::GetInstance().SetPeriod ( pNetMsg->sPRERIODTIME );

	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlWeather(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETSERVER_CTRL_WEATHER *pNetMsg = (GLMSG::SNETSERVER_CTRL_WEATHER *)nmg;
	GLPeriod::GetInstance().SetActiveWeather ( pNetMsg->bActive );
	GLPeriod::GetInstance().SetWeather ( pNetMsg->dwWeather );

	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlWeather2(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETSERVER_CTRL_WEATHER2 *pNetMsg = (GLMSG::SNETSERVER_CTRL_WEATHER2 *)nmg;
	SONEMAPWEATHER oneMapWeather;
	oneMapWeather.dwWeather   = pNetMsg->dwWeather;
	oneMapWeather.dwApplyTime = pNetMsg->dwApplyTime;
	oneMapWeather.map_mID     = pNetMsg->map_mID;
	oneMapWeather.map_sID     = pNetMsg->map_sID;

	if( pNetMsg->dwWeather == NULL )
	{
		GLPeriod::GetInstance().SetActiveWeather ( FALSE );
	}else{
		GLPeriod::GetInstance().SetActiveWeather ( TRUE );
	}
	GLPeriod::GetInstance().SetOneMapActiveWeather( oneMapWeather );

	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlTime(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETSERVER_CTRL_TIME *pNetMsg = (GLMSG::SNETSERVER_CTRL_TIME *)nmg;
	GLPeriod::GetInstance().SetHour ( pNetMsg->dwTime );
	
	GLChar* pChar = NULL;

	//for ( DWORD i=0; i<m_dwMaxClient; ++i )
	UMAP_GLCHAR_ITER iter = m_mapCharacters.begin();
	for (; iter != m_mapCharacters.end(); ++iter)
	{
		pChar = iter->second;//GetChar(i);
		if (pChar)
		    pChar->MsgSendPeriod();
	}

	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlMonth(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETSERVER_CTRL_MONTH *pNetMsg = (GLMSG::SNETSERVER_CTRL_MONTH *)nmg;
	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlGenItemHold(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETSERVER_CTRL_GENITEMHOLD *pNetMsg = (GLMSG::SNETSERVER_CTRL_GENITEMHOLD *)nmg;
	m_bGenItemHold = pNetMsg->bHold;

	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlPlayerKillingMode(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("ServerCtrlPlayerKillingMode Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNET_SERVER_PLAYERKILLING_MODE *pNetMsg = (GLMSG::SNET_SERVER_PLAYERKILLING_MODE *)nmg;
	GLCONST_CHAR::bPK_MODE = pNetMsg->bMODE;

	return TRUE;
}


BOOL GLGaeaServer::VietnamAllInitTime(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_VIETNAM_ALLINITTIME *pNetMsg = (GLMSG::SNETPC_VIETNAM_ALLINITTIME *)nmg;
	//GLMSG::SNETPC_VIETNAM_ALLINITTIME netMsg;

	sc::writeLogInfo(std::string("-=-=-=-= Init Vietnam Time =-=-=-=-"));
	
    GLCHARNODE* pCharNode = m_GaeaPCList.m_pHead;
    for (; pCharNode; pCharNode = pCharNode->pNext)
    {
        GLChar* pChar = pCharNode->Data;
        if (pChar)
            pChar->ReceiveVietnam(pNetMsg->initTime);
    }
	return TRUE;
}


BOOL GLGaeaServer::MsgVietTimeReset( DWORD dwClientID, DWORD dwGaeaID, GLMSG::SNETPC_VIETNAM_TIME_REQ_FB *pNetMsg ) 
{
//#if defined(VN_PARAM) //vietnamtest%%% && vietnamtest2
	if (m_ServiceProvider == SP_VIETNAM)
	{	
		GLChar* pChar = GetChar(dwGaeaID);
		if( !pChar )
			return FALSE;

		pChar->m_dwVietnamGainType = GAINTYPE_MAX;
		pChar->m_sVietnamSystem.Init();
		pChar->m_sVietnamSystem.loginTime = pNetMsg->loginTime;
		pChar->m_sVietnamSystem.gameTime  = 0;

		GLMSG::SNETPC_VIETNAM_ALLINITTIME NetMsg;
		NetMsg.initTime = pNetMsg->loginTime;
		SENDTOCLIENT(dwClientID,&NetMsg);
	}
//#endif
	return TRUE;
}


BOOL GLGaeaServer::ServerCtrlCharDropOutForced(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

	GLMSG::SNET_DROP_OUT_FORCED *pNetMsg = (GLMSG::SNET_DROP_OUT_FORCED *)nmg;
	GLChar* pPC = GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pPC)
        return FALSE;
	
	sc::writeLogInfo( sc::string::format(
		"[ Character ] [ Reserve Drop Out PC %1%, GLGaeaServer::ServerCtrlCharDropOutForced ]",
		pNetMsg->dwChaNum ) );

    ReserveDropOutPC(pPC->GetGaeaID(), db::DbActionNull::Null);

	// ReserveDropOutPC 이후에 void GLChar::Offline() 에서 처리된다.
    // ServerCtrlCharDropOutForced의 경우 중복접속에 의한 강제 종료이므로 Offline 이벤트 발생;
//     InstanceSystem::ManagerField* _pInstanceMan = GetInstanceManagerField();
//     if ( _pInstanceMan )
//         _pInstanceMan->offline(pPC->CharDbNum());

	return TRUE;
}


BOOL GLGaeaServer::CyberCafeClassUpdate( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CYBERCAFECLASS_UPDATE *pNetMsg = (GLMSG::SNET_CYBERCAFECLASS_UPDATE *)nmg;
	GLChar* pCHAR = GetChar ( dwGaeaID );
	if( !pCHAR )
		return FALSE;

	pCHAR->m_emIPBonus = pNetMsg->emIPBonus;

	return TRUE;
}


BOOL GLGaeaServer::GMCtrolMove2Gate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOVE2GATE_FLD *pNetMsg = (GLMSG::SNET_GM_MOVE2GATE_FLD *)nmg;
	GLChar* pPC = GetCharByDbNum(pNetMsg->dwChaNum);//GetChar ( pNetMsg->dwGaeaID );
	if ( !pPC )
		return FALSE;

	if ( m_dwAgentSlot != dwClientID )
		return FALSE;

	GLLandMan* const pLand(pPC->GetLandMan());
	if ( pLand == NULL )
		return FALSE;	

	GLLandMan* pInLandMan = NULL;
	DxLandGateMan* pInGateMan = NULL;
	PDXLANDGATE pInGate = NULL;
	D3DXVECTOR3 vStartPos(0,0,0);

	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	
	pGateMan = pLand->GetLandGateMan();
	pGate = pGateMan->FindLandGate ( pNetMsg->dwGATE );
	if ( !pGate )
		return FALSE;

	vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );

	//	Note : 새로운 맵 위치로 내비게이션 초기화.
	//
	pPC->SetPosition(vStartPos);

	//	Note : 변경된 결과 반환.
	//
	const D3DXVECTOR3& vPosition = pPC->GetPosition();
	GLMSG::SNET_GM_MOVE2GATE_FB NetMsgFB;
	NetMsgFB.vPOS = vPosition;
	SENDTOCLIENT ( pPC->ClientSlot(), &NetMsgFB );

	GLMSG::SNETPC_JUMP_POS_BRD NetMsgBrd(pPC->GetGaeaID(), vPosition, 0);
	pPC->SendMsgViewAround(&NetMsgBrd);
	return TRUE;
}

//
//mjeon.sns
//
BOOL GLGaeaServer::MsgSNSFBUpdateAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	if (!GLUseFeatures::GetInstance().IsUsingSNS())
	{
		sc::writeLogError(std::string("MsgSNSFBUpdateAF() - Wrong use of SNS-system while it is OFF!"));
		return FALSE;
	}

    GLMSG::SNET_SNS_FB_UPDATE_AF *msg = (GLMSG::SNET_SNS_FB_UPDATE_AF *) nmg;

    SFACEBOOK AUTHFB = msg->AUTHFB;	

    int nChaNum = AUTHFB.ChaNum;

    GLChar* pCha = GetCharByDbNum(nChaNum);

    if (pCha == NULL)
    {
        sc::writeLogError(std::string("SNSFBUpdate: Character does not exist."));

        //SNET_SNS_FB_UPDATE_AF: 단방향 업데이트용 메시지

        return FALSE;
    }

    //
    // Update the Character's FB-info.
    //
    pCha->m_AUTHFB = AUTHFB;

    GLMSG::SNET_SNS_FB_UPDATE_BR msgBR;
    msgBR.dwGaeaID = pCha->GetGaeaID();

    msgBR.byFB = (BYTE)(BOOL)( strlen(AUTHFB.SKey) > 0 && strlen(AUTHFB.UID) > 0 );	

    pCha->SendMsgViewAround(&msgBR);

    return TRUE;
}


BOOL GLGaeaServer::MsgSNSTWUpdateAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	if (!GLUseFeatures::GetInstance().IsUsingSNS())
	{
		sc::writeLogError(std::string("MsgSNSTWUpdateAF() - Wrong use of SNS-system while it is OFF!"));
		return FALSE;
	}

    GLMSG::SNET_SNS_TW_UPDATE_AF *msg = (GLMSG::SNET_SNS_TW_UPDATE_AF *) nmg;

    STWITTER AUTHTW = msg->AUTHTW;

    int nChaNum = AUTHTW.ChaNum;

    GLChar* pCha = GetCharByDbNum(nChaNum);

    if (pCha == NULL)
    {
        sc::writeLogError(std::string("SNSTWUpdate: Character does not exist."));

        //SNET_SNS_FB_UPDATE_AF: 단방향 업데이트용 메시지

        return FALSE;
    }

    //
    // Update the Character's FB-info.
    //
    pCha->m_AUTHTW = AUTHTW;

    GLMSG::SNET_SNS_TW_UPDATE_BR msgBR;
    msgBR.dwGaeaID = pCha->GetGaeaID();

    msgBR.byTW = (BYTE)(BOOL)( strlen(AUTHTW.AToken) > 0 && strlen(AUTHTW.ATokenS) > 0 && strlen(AUTHTW.UID) > 0 );

    pCha->SendMsgViewAround(&msgBR);

    return TRUE;
}


BOOL GLGaeaServer::MsgAttendanceSyncFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	GLMSG::SNET_ATTENDANCE_SYNC_FF *msg=(GLMSG::SNET_ATTENDANCE_SYNC_FF *)nmg;

	GLChar *pChar = GetCharByDbNum(msg->dwChaNum);

	if ( !pChar )
	{
		sc::writeLogError(
			sc::string::format( "Failed to load '%1%' character info. ( MsgAttendanceSyncFF )",
			msg->dwChaNum ) );

		return FALSE;
	}

	pChar->MsgAttendanceSyncFF( nmg );
	return TRUE;	
}

//DF
BOOL GLGaeaServer::MsgAttendanceResetFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	GLMSG::SNET_ATTENDANCE_RESET_FF *msg=(GLMSG::SNET_ATTENDANCE_RESET_FF *)nmg;

	GLChar *pChar = GetCharByDbNum(msg->dwChaNum);

	if ( !pChar )
		return FALSE;
	pChar->MsgAttendanceResetFF( nmg );
	return TRUE;	
}


BOOL GLGaeaServer::MsgAttendanceUpgradeFF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	GLMSG::SNET_ATTENDANCE_UPGRADE_FF *msg=(GLMSG::SNET_ATTENDANCE_UPGRADE_FF *)nmg;

	GLChar *pChar = GetCharByDbNum(msg->dwChaNum);

	if ( !pChar )
		return FALSE;
	pChar->MsgAttendanceUpgradeFF( nmg );
	return TRUE;	
}


BOOL GLGaeaServer::MsgAttendanceDailyMaintenanceStartAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	sc::writeLogInfo(std::string("MsgAttendanceDailyMaintenanceStartAF() starts saving attendance status of all characters into DB."));

	//
	// iterate all GLChars and calls Attendance::Save().
	//

	if (m_GaeaPCList.m_dwAmount <= 0)	//there's no character in the FieldS.
	{
		GLMSG::SNET_ATTENDANCE_DAILYMAINTENANCE_READY_FA msgReady;
		SENDTOAGENT(&msgReady);

		g_bAttendanceDailyMaintenance = TRUE;	//lock during attendance maintenance

		return TRUE;
	}

	if (g_bAttendanceDailyMaintenance)
	{
		g_bAttendanceDailyMaintenance = FALSE;	//unlock before attendance maintenance (여기서 unlock하지 않아도, 이 시점에서 FALSE여야하나, 예외상황이 존재하는지 체크해보자.)

		sc::writeLogError(std::string("GLGaeaServer::MsgAttendanceDailyMaintenanceStartAF() - g_bAttendanceDailyMaintenance is supposed to be unlocked at this time!"));
	}

	PGLCHAR pChar				= NULL;
	GLCHARNODE *pCharNode		= m_GaeaPCList.m_pHead;	

	for(; pCharNode; pCharNode = pCharNode->pNext)
	{
		pChar = pCharNode->Data;

		if (pChar)
		{
			pChar->AttendanceSetReady(FALSE);

			if(pCharNode == m_GaeaPCList.m_pTail || pCharNode->pNext == NULL)
			{
				pChar->AttendanceSave(TRUE);
			}
			else	// LastOne
			{
				pChar->AttendanceSave(FALSE);
			}
		}
	}

	g_bAttendanceDailyMaintenance = TRUE;	//lock during attendance maintenance

	return TRUE;
}


BOOL GLGaeaServer::MsgAttendanceDailyMaintenanceEndAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if (g_bAttendanceDailyMaintenance)
	{
		sc::writeLogInfo(std::string("MsgAttendanceDailyMaintenanceEndAF() starts loading attendance status of all characters from DB."));
		//
		// iterate all GLChars and calls Attendance::Load().
		//

		PGLCHAR pChar				= NULL;
		GLCHARNODE *pCharNode		= m_GaeaPCList.m_pHead;
		
		g_bAttendanceDailyMaintenance = FALSE;	//unlock after finishing attendance maintenance

		for(; pCharNode; pCharNode = pCharNode->pNext)
		{
			pChar = pCharNode->Data;

			if (pChar)
			{
				pChar->AttendanceLoad();
				pChar->AttendanceSetReady(TRUE);
			}
		}
	}	
	
	return TRUE;
}


BOOL GLGaeaServer::MsgAttendanceTakeRewardCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar(dwGaeaID);
	if (pChar)
    {		
	    pChar->MsgAttendanceTakeRewardCF(nmg);
	    return TRUE;	
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgAttendanceDonePointItemCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgAttendanceDonePointItemCF( nmg );
	return TRUE;	
}


BOOL GLGaeaServer::MsgAttendanceWhisperDoneAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	GLMSG::SNET_ATTENDANCE_WHISPER_DONE_AF *msg = (GLMSG::SNET_ATTENDANCE_WHISPER_DONE_AF *)nmg;
	
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgAttendanceWhisperDoneAF( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgAttendanceLoadEnd( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	// DBAction에서 보내는 메세지이고 별다른 처리를 하지 않으므로 검사를 하지 않고 클라이언트로 보냄
	GLMSG::SNET_ATTENDANCE_LOAD_END *msg = (GLMSG::SNET_ATTENDANCE_LOAD_END *)nmg;
	
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;

	pChar->SendToClient( nmg );

	return TRUE;	
}


//
//mjeon.CaptureTheField
//
BOOL GLGaeaServer::MsgCTFReadyAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFReadyAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_READY_AF *pmsg = (GLMSG::SNET_CTF_READY_AF *)nmg;

	if(!pmsg->bUpdate)
	{
		m_pCTFMan->Ready();
	}
	else
	{
		m_pCTFMan->SetRemainMinutes(pmsg->nRemainMinutes);
		m_pCTFMan->UpdateReady();
	}

	return TRUE;
}


BOOL GLGaeaServer::MsgCTFPrepareAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFPrepareAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}	

	m_pCTFMan->Prepare();

	return TRUE;
}


BOOL GLGaeaServer::MsgCTFStartAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFStartAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}
	
	m_pCTFMan->Start();

	return TRUE;
}


BOOL GLGaeaServer::MsgCTFJoinCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFJoinCF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	BOOL bRet = TRUE;

	GLMSG::SNET_CTF_JOIN_CF *pmsg = (GLMSG::SNET_CTF_JOIN_CF *)nmg;
	

	GLMSG::SNET_CTF_JOIN_FA msg;	
	msg.bJoin = pmsg->bJoin;


	GLChar* pChar = GetChar(dwGaeaID);

	//
	//1.Does the character exist?
	//
	if ( !pChar )
	{	
		sc::writeLogError(std::string("GLGaeaServer::MsgCTFJoinCF - Char does not exist"));

		bRet = FALSE;

		goto ctf_join_relay;
	}

    if ( pChar->IsDie() )
    {
		sc::writeLogError(sc::string::format("GLGaeaServer::MsgCTFJoinCF Hacking user %1%", pChar->CharDbNum()));		
        return FALSE;
    }

	msg.dwChaNum= pChar->CharDbNum();
	msg.wLevel	= pChar->GetLevel();
	
	
ctf_join_relay:

	SENDTOAGENT(&msg);	//dwClientID

	return bRet;
}


BOOL GLGaeaServer::MsgCTFRecallToAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFRecallToAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	//
	//This msg is sent by AgentServer using CharAG's client slot. So dwGaeaID and dwClientID are valid for the character.
	//
	GLMSG::SNET_CTF_RECALL_TO_AF *pmsg = (GLMSG::SNET_CTF_RECALL_TO_AF *)nmg;

	GLChar* pChar = GetCharByDbNum(pmsg->dwChaNum);

	//
	//1.Does the character exist?
	//
	if ( !pChar )
	{
		sc::writeLogError(std::string("GLGaeaServer::MsgCTFRecallToAF - Char does not exist"));
		return FALSE;
	}	


	//
	//In case of rebirth, a dead player will be also recalled by CTF system through this routine.
	//
	if (pmsg->idMap == GetCTFPvPMapID())
	{
		/*
		//
		//2-1.This is a recall for CTF players. (Nobody can play in CTFPvPMap not during CTF.)
		//
		if (pChar->m_sMapID != pmsg->idMap)
		{
			pChar->SetSTATE(EM_RECALLED_BY_CTF);
		}
		*/
		//  [6/2/2014 gbgim];
		// - 죽은 상태라면 절대로 맵이동할 수 없게 기획팀 원용씨와 협의하였습니다.;
		//   이곳이 아니더라도, 맵이동을 하는 경우 죽어있다면 이동안시켜야합니다;
		//   죽은 상태로 CTF강제 리콜시 오동작을 하는등 규칙이 정해져있지않아 Casebycase형식으로 부활 종류가;
		//   늘어나서 정한 규칙입니다; CRM#2482
		if ( !pChar->IsValidBody() )
		{
			GLMSG::SNET_CTF_JOIN_FA msg;
			msg.bJoin = FALSE;
			msg.emJoinResult = CTF_JOIN_NO_CHAR;
			msg.dwChaNum= pChar->CharDbNum();
			msg.wLevel	= pChar->GetLevel();
			SENDTOAGENT(&msg);

			return FALSE;
// 			GLMSG::SNETPC_REQ_TELEPORT_FB	msgfc;
// 			msgfc.emFB = EMREQ_TELEPORT_FB_JOINCON;
// 			SENDTOCLIENT(pChar->GETCLIENTID(), &msgfc);
// 			return FALSE;
		}
		//
		//2-2.Set or Unset flag to prevent using of potions. (Recall4Play OR Recall4Rebirth)
		//
		pChar->m_dwActionLimit |= ( EMACTION_LIMIT_PET | EMACTION_LIMIT_DRUG_HP | EMACTION_LIMIT_CARD );
		//pChar->m_sHP.TO_FULL();
		//pChar->m_sMP.TO_FULL();
		//pChar->m_sSP.TO_FULL();
	}
	else
	{
		pChar->m_dwActionLimit &= ~( EMACTION_LIMIT_PET | EMACTION_LIMIT_DRUG_HP | EMACTION_LIMIT_CARD );
	}

	//
	//3.Recall
	//
	return RecallTo(pChar, pmsg->idMap, pmsg->dwGateID);
}


BOOL GLGaeaServer::MsgCTFStopAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFStopAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	m_pCTFMan->Stop();

	return TRUE;
}

BOOL GLGaeaServer::MsgCTFNextReadyTimeAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT( IsField() );

	if ( ( dwClientID != m_dwAgentSlot ) || ( dwGaeaID != GAEAID_NULL ) )
	{
		GLChar* const pChar( GetChar( dwGaeaID ) );
		if ( pChar != NULL )
		{
			sc::writeLogError( sc::string::format( "MsgCTFNextReadyTimeAF() - HACKING!! dbNum : %1%", pChar->CharDbNum() ) );
		}
		return FALSE;
	}

	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		sc::writeLogError(std::string("MsgCTFNextReadyTimeAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_NEXT_READY_TIME *pmsg = ( GLMSG::SNET_CTF_NEXT_READY_TIME* )nmg;

	if ( m_pCTFMan )
	{
		m_pCTFMan->SetReadyTime( pmsg->ctReadyTime );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgCTFDoneAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFDoneAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	m_pCTFMan->Done();

	return TRUE;
}


BOOL GLGaeaServer::MsgCTFRewardAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

    if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
    {
        sc::writeLogError(std::string("MsgCTFRewardAF() - Wrong use of CTF while it is OFF!"));
        return FALSE;
    }
    GLMSG::SNET_CTF_REWARD_INFO *pmsg = (GLMSG::SNET_CTF_REWARD_INFO *)nmg;

    m_pCTFMan->Reward(pmsg->wSchoolAuthed, pmsg->nHowMany, pmsg->arrBuffs);
	
	
	SENDTOALLCLIENT(nmg);	//모든 클라이언트에게 보상정보 전달 (전장 통합UI 갱신)

    return TRUE;
}


BOOL GLGaeaServer::MsgCTFSyncPlayerNumAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFSyncPlayerNumAF() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_SYNC_PLAYERNUM_AF *pmsg = (GLMSG::SNET_CTF_SYNC_PLAYERNUM_AF *)nmg;

	for (int i=0; i<SCHOOL_NUM; i++)
	{
		m_pCTFMan->SetNumberOfPlayers(i, pmsg->arrPlayerNum[i]);
    }

	m_pCTFMan->PlayerNumHasUpdated();
	//m_pCTFMan->UpdatePlayersPosition();	//update players' position vector for minimap update

	return TRUE;
}


BOOL GLGaeaServer::MsgCTFQuitCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT( IsField() );

	// CTF 사용 체크;
	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		sc::writeLogError( 
			"MsgCTFQuitCF() - Wrong use of CTF while it is OFF!" );

		return FALSE;
	}

	// Message 체크;
	GLMSG::SNET_CTF_QUIT_CFA* pMsg =
		static_cast< GLMSG::SNET_CTF_QUIT_CFA*>( nmg );

	if ( !pMsg )
	{
		sc::writeLogError( 
			"MsgCTFQuitCF() - Message is NULL!" );

		return FALSE;
	}

	// 캐릭터 상태 체크;
	GLChar* const pChar( GetChar( dwGaeaID ) );
	if ( !pChar )
	{
		sc::writeLogError(
			"MsgCTFQuitCF() - Char is NULL." );
		return FALSE;
	}

	/*if ( pChar != NULL )
	{
		sc::writeLogError( sc::string::format(
			"MsgCTFQuitCF() - HACKING!! dbNum : %1%",
			pChar->CharDbNum() ) );
	}*/

	// 죽은 상태라면 현재 위치에서 부활;
	if ( pChar->IsDie() )
	{
		// 즉시 강제 부활;
		pChar->ResurrectImmediatelyForced();

		// CTF Quit 예약;
		pChar->ReserveAction( GLChar::EMRESERVED_ACTION_CTF_QUIT );
	}
	// 죽은 상태가 아니라면 바로 나가기 수행;
	else
	{
		pMsg->dwChaNum = pChar->CharDbNum();
		pChar->SendToAgent( pMsg );
	}

	return TRUE;
}


BOOL GLGaeaServer::MsgCTFQuitAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT(IsField());

	if ( (dwClientID != m_dwAgentSlot) || (dwGaeaID != GAEAID_NULL) )
	{
		GLChar* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFRecallToAF() - HACKING!! dbNum : %1%", pChar->CharDbNum()));
		}
		return FALSE;
	}

    if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
    {
        sc::writeLogError(std::string("MsgCTFQuitAF() - Wrong use of CTF while it is OFF!"));
        return FALSE;
    }

    GLMSG::SNET_CTF_QUIT_AF *pmsg = (GLMSG::SNET_CTF_QUIT_AF *)nmg;

    m_pCTFMan->Quit(pmsg->dwChaNum, (ENUM_CTF_STATE)pmsg->state);
    
    return TRUE;
}


//
//mjeon
//
//Process PingReq & Res
//
BOOL GLGaeaServer::ProcessPingTraceMsg( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	//mjeon
	//process the PingRequest from client
	//

	//Just send back the FB messages for acknowledge.
	switch( nmg->nType )
	{
	case NET_MSG_GM_PINGTRACE_ON:
		{
			m_bTracePing = true;

			sc::writeLogInfo(std::string("GM Command /PINGTRACE ON has input."));
			
			//GLMSG::SNET_GM_PINGTRACE_ON_FB MsgFB;			
			//SENDTOAGENT( dwClientID, &MsgFB );

			break;
		}
	case NET_MSG_GM_PINGTRACE_OFF:
		{
			m_bTracePing = false;

			sc::writeLogInfo(std::string("GM Command /PINGTRACE OFF has input."));

			//GLMSG::SNET_GM_PINGTRACE_OFF_FB MsgFB;			
			//SENDTOAGENT( dwClientID, &MsgFB );

			break;
		}
	}

	return TRUE;
}


BOOL GLGaeaServer::ProcessPingMsg( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	//mjeon
	//process the PingRequest from client
	//

	switch( nmg->nType )
	{
	case NET_MSG_GM_PING:
		{
			GLMSG::SNET_GM_PING *pMsgPing = (GLMSG::SNET_GM_PING *)nmg;
			GLMSG::SNET_GM_PING MsgPingFB;

			/*
			GLChar* pChar = GetChar ( pMsgPing->dwGaeaID );
			if ( !pChar )
				return FALSE;	
			*/

			MsgPingFB.nType = NET_MSG_GM_PING_FB;
			MsgPingFB.dwSent = pMsgPing->dwSent;
			MsgPingFB.dwRecvd = GetTickCount();
			MsgPingFB.bServerType = 1;	//FieldServer

			SENDTOCLIENT ( dwClientID, &MsgPingFB );

			break;
		}	

	case NET_MSG_GM_PING_FB:
		{
			GLMSG::SNET_GM_PING *pMsgPing = (GLMSG::SNET_GM_PING *)nmg;

			DWORD dwArrvd = GetTickCount();
			DWORD dwRTT =  dwArrvd - pMsgPing->dwSent;

			//DWORD dwPingTime = dwArrvd - pMsgPing->dwRecvd;	// 이 값은 필요할 경우만 사용하면 된다.
			//DWORD dwPongTime = m_dwRTT - dwPingTime;			// 이 값은 필요할 경우만 사용하면 된다.

			InsertPingLog( dwClientID, dwGaeaID, dwRTT);
			
			break;
		}


	}

	//mjeon.Test
	//TestCode for Server-Side Ping
	//InsertPingLog( dwClientID, dwGaeaID, 2515);


	
	//
	//mjeon
	//process the PingResponse from client - not used yet!
	//
	//else
	//{
	//	GLMSG::SNET_GM_PING *pMsgPing = (GLMSG::SNET_GM_PING *)nmg;
	//	DWORD dwArrvd = GetTickCount();
	//	m_dwRTT =  dwArrvd - pMsgPing->dwSent;
	//	DWORD dwPingTime = dwArrvd - pMsgPing->dwRecvd;		// 이 값은 필요할 경우만 사용하면 된다.
	//	DWORD dwPongTime = m_dwRTT - dwPingTime;			// 이 값은 필요할 경우만 사용하면 된다.

	//	break;
	//}	

	return TRUE;
}


BOOL GLGaeaServer::GMCtrlNonConfrontMode(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrlWhereNpcALL Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }

	
	GLMSG::SNET_GM_NONCONFRONT_MODE *pNetMsg = (GLMSG::SNET_GM_NONCONFRONT_MODE *)nmg;
	const SNATIVEID sMapID(pNetMsg->sMapID);
	GLLandMan* pLandMan = GetLand ( sMapID );
	if ( !pLandMan )
		return FALSE;

	if ( !pLandMan->RequestNonConfrontMode( pNetMsg ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMove2MapPos ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_GM_MOVE2MAPPOS_FLD* pNetMsg = static_cast< GLMSG::SNET_GM_MOVE2MAPPOS_FLD* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;
	
	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( NULL == pChar )
		return FALSE;
	
	/// GM 일 경우만 가능하다;
	if ( pChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	/// 맵이 인던맵이면 이동할 수 없다;
	const SMAPNODE* pMapNode = FindMapNode ( pNetMsg->nidMAP );
	if ( ( NULL == pMapNode ) ||
		( pMapNode->IsInstantMap() == true ) )
	{
		return FALSE;
	}

	/// 이동정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.sMapID = pNetMsg->nidMAP;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();

    std::tr1::shared_ptr< GLMapAxisInfo > spAxisInfo = GetAxisInfo( pMapNode->m_LevelFile );
    if ( spAxisInfo )
        spAxisInfo->MapPos2MiniPos( pNetMsg->dwPOSX, pNetMsg->dwPOSY, sMoveInfo.vPosition.x, sMoveInfo.vPosition.z );

	/// 이동한다;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	/*DropOutPET ( pChar->m_dwPetGUID, true, true );
	
	//VEHICLE
	SaveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), true );

	//SUMMON
	DropOutAllSummon ( pChar );

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT ( pChar->ClientSlot(), &msg );
	}

	//	Note : 다른 필드 서버일 경우.
	if ( pMapNode->GetFieldServerNum() != m_dwFieldSvrID )
	{
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = pChar->CharDbNum();
		NetMsgAg.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
		NetMsgAg.mapIDTo = nidMAP;
		NetMsgAg.dwGATEID = UINT_MAX;
		NetMsgAg.vPosition = vPOS;
		SENDTOAGENT(&NetMsgAg);	//pChar->ClientSlot()
	}
	//	Note : 같은 필드 서버일 경우.
	else
	{
		GLMSG::SNETPC_REQ_RECALL_FB NetMsgFB;
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(pChar->GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(pChar->GetCurrentMap());

		BOOL bOK = RequestInvenRecallThisSvr ( pChar, nidMAP, UINT_MAX, vPOS );
		if ( !bOK )
		{
			NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
			SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != MapID(nidMAP) )
		{
			for ( int i=0; i<EMBLOW_MULTI; ++i )
				pChar->DISABLEBLOW ( i );
			for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
				pChar->RESETSKEFF ( i );

			// CP 초기화 맵인지 확인
			if (pMapNode->IsCombatPointReset())
				pChar->ResetCP();
		}



		pChar->ResetAction();		

		//	Note : 멥 이동 성공을 알림.
		//
		NetMsgFB.dwChaNum = dwDbNum;
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.bCullByObjectMap = pMapNode->IsCullByObjectMap();
		NetMsgFB.sMAPID = nidMAP;
		NetMsgFB.vPOS = pChar->GetPosition();
		SENDTOAGENT(&NetMsgFB);	//pChar->ClientSlot()
	}*/

	return TRUE;
}

BOOL GLGaeaServer::GMCtrlWhereNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_WHERE_NPC_FLD *pNetMsg = (GLMSG::SNET_GM_WHERE_NPC_FLD *)nmg;
	if ( !m_bEmulator )
	{
		GASSERT(dwClientID == m_dwAgentSlot);
		if ( dwClientID != m_dwAgentSlot )
			return FALSE;
	}

	GLChar* pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	GLMSG::SNET_GM_WHERE_NPC_FB NetMsgFB;
	PGLCROW pCrow = NULL;
	int nPosX(INT_MAX), nPosY(INT_MAX);
	for ( DWORD i=0; i<MAXCROW; ++i )
	{
		pCrow = pLand->GetCrow(i);
		if ( pCrow && pCrow->m_pCrowData->sNativeID==pNetMsg->nidNPC )
		{				
			pLand->GetMapAxisInfo().Convert2MapPos ( pCrow->GetPosition().x, pCrow->GetPosition().z, nPosX, nPosY );

			NetMsgFB.nPosX = nPosX;
			NetMsgFB.nPosY = nPosY;
			SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgFB );
		}
	}
	
	return TRUE;
}



BOOL GLGaeaServer::GMCtrlWhereNpcALL(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_WHERE_NPC_ALL_FLD *pNetMsg = (GLMSG::SNET_GM_WHERE_NPC_ALL_FLD *)nmg;
	if ( !m_bEmulator )
	{
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrlWhereNpcALL Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
	}

	GLChar* pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	GLMSG::SNET_GM_WHERE_NPC_ALL_FB NetMsgFB;
	PGLCROW pCrow = NULL;
	int nPosX(INT_MAX), nPosY(INT_MAX);
	for ( DWORD i=0; i<MAXCROW; ++i )
	{
		pCrow = pLand->GetCrow(i);
		if ( pCrow && pCrow->GetCrow() == CROW_NPC )
		{				
			pLand->GetMapAxisInfo().Convert2MapPos ( pCrow->GetPosition().x, pCrow->GetPosition().z, nPosX, nPosY );

			NetMsgFB.nidNPC =  pCrow->m_pCrowData->sNativeID;
			NetMsgFB.nPosX = nPosX;
			NetMsgFB.nPosY = nPosY;

			SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgFB );
		}
	}

	return TRUE;
}



BOOL GLGaeaServer::GMCtrolWherePcPos(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_WHERE_PC_POS *pNetMsg = (GLMSG::SNET_GM_WHERE_PC_POS *)nmg;
	GLChar* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->UserLevel() < USER_GM3 )
		return FALSE;

	bool bFOUD(false);
	int nPosX(INT_MAX), nPosY(INT_MAX);

	pCHAR = GetCharByDbNum(pNetMsg->dwCHARID);

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pCHAR && pLand )
	{
		bFOUD = true;
		const D3DXVECTOR3 vPosition = pCHAR->GetPosition();
		pLand->GetMapAxisInfo().Convert2MapPos ( vPosition.x, vPosition.z, nPosX, nPosY );
	}

	GLMSG::SNET_GM_WHERE_PC_POS_FB NetMsgFB;
	NetMsgFB.bFOUND = bFOUD;
	NetMsgFB.nPosX = nPosX;
	NetMsgFB.nPosY = nPosY;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	return TRUE;
}

BOOL GLGaeaServer::GMCtrlWarningMSG(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_WARNING_MSG_FLD* pNetMsg = (GLMSG::SNET_GM_WARNING_MSG_FLD*)nmg;
	GLMSG::SNET_GM_WARNING_MSG_BRD NetMsgBrd;
	NetMsgBrd.bOn = pNetMsg->bOn;

    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrlWarningMSG Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLChar* pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	LANDQUADTREE* pLandTree = pLand->GetLandTree();
	if ( !pLandTree )
		return FALSE;

	// 맵의 크기를 얻어온다.
	D3DXVECTOR3 vMax, vMin;
	pLand->GetNaviMeshAABB ( vMax, vMin );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect( (int)vMax.x,(int)vMax.z,(int)vMin.x,(int)vMin.z );
	pLandTree->FindNodes ( bRect, pLandTree->GetRootNode(), &pQuadNode );

	GLLandNode* pLandNode = NULL;
	GLCHARNODE *pCharCur = NULL;
	GLChar* pGLChar = NULL;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		pLandNode = pQuadNode->pData;

		GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"Index over cell size");

		pCharCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCharCur; pCharCur = pCharCur->pNext )
		{
			pGLChar = pCharCur->Data;
			if( !pGLChar ) continue;

			SENDTOCLIENT ( pGLChar->ClientSlot(), &NetMsgBrd );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolBigHead(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrolBigHead Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNET_GM_BIGHEAD* pNetMsg = (GLMSG::SNET_GM_BIGHEAD*)nmg;
	m_bBigHead = pNetMsg->bBIGHEAD;
	m_fBigHead = pNetMsg->fSize;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolBigHand(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrolBigHand Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNET_GM_BIGHAND *pNetMsg = (GLMSG::SNET_GM_BIGHAND *)nmg;
	m_bBigHand = pNetMsg->bBIGHAND;
	m_fBigHand = pNetMsg->fSize;

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolFreePK(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrolFreePK Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNET_GM_FREEPK *pNetMsg = (GLMSG::SNET_GM_FREEPK *)nmg;
	if ( pNetMsg->dwPKTIME != 0 )
        m_bBrightEvent = true;
	else 
		m_bBrightEvent = false;

	return TRUE;
}

BOOL GLGaeaServer::GMKicUser( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMKicUser Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNET_GM_KICK_USER_PROC_FLD* pNetMsgFld = (GLMSG::SNET_GM_KICK_USER_PROC_FLD*)nmg;
	GLChar* pTARCHAR = GetCharByDbNum(pNetMsgFld->dwID);
	if (!pTARCHAR )
		return FALSE;

	sc::writeLogInfo( sc::string::format(
		"[ Character ] [ Reserve Drop Out PC %1%, GLGaeaServer::GMKicUser ]",
		pNetMsgFld->dwID ) );

    ReserveDropOutPC(pTARCHAR->GetGaeaID(), db::DbActionNull::Null);

	// ReserveDropOutPC 이후에 void GLChar::Offline() 에서 처리된다.
    // GMKicUser의 경우 명령어에 의한 강제 접속 종료이므로 Offline 이벤트 발생;
//     InstanceSystem::ManagerField* _pInstanceMan = GetInstanceManagerField();
//     if ( _pInstanceMan )
//         _pInstanceMan->offline(pTARCHAR->CharDbNum());


	return TRUE;
}

BOOL GLGaeaServer::GMCtrolShowMeTheMoney( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if(dwClientID != m_dwAgentSlot)
        sc::writeLogError( sc::string::format("GMCtrolShowMeTheMoney Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );

	GLMSG::SNET_GM_SHOWMETHEMONEY_FLD* pNetMsgFld = (GLMSG::SNET_GM_SHOWMETHEMONEY_FLD*)nmg;
	GLChar* pCHAR = GetChar(pNetMsgFld->dwGaeaID);
	if (!pCHAR)
        return FALSE;

    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrolShowMeTheMoney Hacking Try [CharName:%d] [CharDbNum:%d]", pCHAR->GetName(), pCHAR->CharDbNum() ) );
        return FALSE;
    }



    if (pCHAR->UserLevel() < USER_MASTER)
        return FALSE;

	pCHAR->CheckMoneyUpdate(pCHAR->GetInvenMoney(), pNetMsgFld->llMoney, TRUE, "Show Me The Money.");
	pCHAR->AddInvenMoneyServer(pNetMsgFld->llMoney, ADD_INVEN_MONEY_SHOWMETHEMONEY);

	GLMSG::SNET_GM_SHOWMETHEMONEY NetMsgFB(pCHAR->GetInvenMoney());
	SENDTOCLIENT(pCHAR->ClientSlot(), &NetMsgFB);
	return TRUE;
}

BOOL GLGaeaServer::GmShowMoTheMoneyEmulatorCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!IsEmulatorMode())
        return FALSE;
    GLMSG::NET_GM_SHOWMETHEMONEY_EMULATOR_CF* pPacket = (GLMSG::NET_GM_SHOWMETHEMONEY_EMULATOR_CF*) nmg;
    if (pPacket->MAGIC_NUM != GLMSG::NET_GM_SHOWMETHEMONEY_EMULATOR_CF::MAGIC_NUM)
        return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
        pChar->AddInvenMoneyServer(pPacket->llMoney, ADD_INVEN_MONEY_SHOWMETHEMONEY_EMUL);
        
        GLMSG::SNET_GM_SHOWMETHEMONEY NetMsgFB(pChar->GetInvenMoney());
        SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::GMCtrolWhySoSerious( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwClientID != m_dwAgentSlot )
	{
        sc::writeLogError( sc::string::format( "GMCtrolWhySoSerious Hacking Try [IP:%1%]", GetClientIP( dwClientID ) ) );		

		GLChar* pCracker( GetChar( dwGaeaID ) );
		if ( pCracker == NULL )
			return FALSE;

		sc::writeLogError( sc::string::format( "GMCtrolWhySoSerious Hacking Try [CharName:%1%] [CharDbNum:%1%]", pCracker->GetName(), pCracker->CharDbNum() ) );

		return FALSE;
	}

    GLMSG::SNET_GM_WHYSOSERIOUS_FLD* pPacket = ( GLMSG::SNET_GM_WHYSOSERIOUS_FLD* ) nmg;
    GLChar* pTargetChar( GetChar( pPacket->dwGaeaID ) );
    if ( pTargetChar == NULL )
        return FALSE;    	

	const SNATIVEID& _itemID( pPacket->sItemID );

	GLMSG::SNET_GM_WHYSOSERIOUS_FB_FAC messageFB( pPacket->dwMasterGaeaID, pTargetChar->GetName(), _itemID, pPacket->dwNums );

	if ( GmMakeItem( pTargetChar, _itemID, pPacket->dwNums ) == TRUE )
	{
		const std::string stringGMCmd( sc::string::format( "Character(%1%) Item(%2%/%3%) Add by GM-Command\0", pTargetChar->CharDbNum(), _itemID.Mid(), _itemID.Sid() ) );
		m_pAdoMan->AddUserJob( db::DbActionPtr( new UserWriteGMLog( pPacket->dwMasterUserNum, stringGMCmd, pPacket->szMasterIP ) ) );

		sc::writeLogInfo(sc::string::format(
			"GM:%1% insert item(%2%/%3%) into Player:%4%(%5%)", 
			pPacket->szMasterID, 
			_itemID.Mid(), _itemID.Sid(), 
			pTargetChar->GetName(), pTargetChar->CharDbNum() ) );

		messageFB.bFailed = false;
	}
	else
		messageFB.bFailed = true;

	SENDTOAGENT( &messageFB );

	return TRUE;
}

BOOL GLGaeaServer::MsgLogToDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
        sc::writeLogError( sc::string::format("MsgLogToDB Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );

    GLMSG::SNET_GM_LOG_TO_DB_CAF* pPacket = (GLMSG::SNET_GM_LOG_TO_DB_CAF*) nmg;

    GetItemLimit()->TxtLogToDBForce();

    return TRUE;
}

BOOL GLGaeaServer::GmWhySoSeriousEmulatorCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (!IsEmulatorMode())
        return FALSE;
    
    GLMSG::NET_GM_WHYSOSERIOUS_EMULATOR_CF* pPacket = (GLMSG::NET_GM_WHYSOSERIOUS_EMULATOR_CF*) nmg;
    if (pPacket->MAGIC_NUM != GLMSG::NET_GM_WHYSOSERIOUS_EMULATOR_CF::MAGIC_NUM)
        return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
		return FALSE;

	GmMakeItem(pChar, pPacket->sItemID, pPacket->dwNums);

	return TRUE;
}

BOOL GLGaeaServer::GMFlyCameraControlCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
// 	if (!IsEmulatorMode())
// 		return FALSE;

	GLMSG::SNET_GM_FLYCAMERACONTROL_CF* pMsg = (GLMSG::SNET_GM_FLYCAMERACONTROL_CF*)nmg;

	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar || pChar->m_dwUserLvl < USER_MASTER )
		return FALSE;
	
	pChar->m_bFlyCameraUse = pMsg->bOn;
	pChar->SetViewAround(300.0f, 300.0f);
	GLMSG::SNET_GM_FLYCAMERACONTROL_FC pMsgFC;
	pMsgFC.bOn = pMsg->bOn;
	SENDTOCLIENT( pChar->GETCLIENTID(), &pMsgFC );
	return TRUE;
}

BOOL GLGaeaServer::GMFlyCameraSetViewSight(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF* pMsg = (GLMSG::SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF*)nmg;

	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar || pChar->m_dwUserLvl < USER_MASTER )
		return FALSE;
	if ( pChar->m_bFlyCameraUse == false )
		return FALSE;

	pChar->SetViewAround(pMsg->fSight, 300.0f);
	return TRUE;
}

BOOL GLGaeaServer::GMFlyCameraControlMoveCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_FLYCAMERACONTROL_MOVE_CF* pMsg = (GLMSG::SNET_GM_FLYCAMERACONTROL_MOVE_CF*)nmg;
	GLChar* pChar = GetChar(dwGaeaID);
	if ( !pChar || pChar->m_dwUserLvl < USER_MASTER )
		return FALSE;

	pChar->SetFlyCameraPos(pMsg->vPosition);
	return TRUE;
}

BOOL GLGaeaServer::GMCtrolReleaseCTFBuff( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );
	if ( !m_pBuffMan || !pChar || pChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLLandMan* const pLand( pChar->GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	CHARDBNUM_LIST ChaList;

	pLand->CollectPC( ChaList );

	for ( CHARDBNUM_LIST_ITER it = ChaList.begin(); it != ChaList.end(); ++it )
	{
		GLChar* pTarget = GetCharByDbNum( *it );
		if ( pTarget )
		{
			if ( pTarget->m_sSKILLFACT[SKILLFACT_INDEX_CTF_NORMAL_REWARD0].sNATIVEID != SNATIVEID( false ) )
			{
				m_pBuffMan->ResetBuff( pTarget, m_pBuffMan->GetCTFBuffMID(), EMSKILLFACT_TYPE( EMSKILLFACT_TYPE_SYSTEM ) );
				sc::writeLogInfo( sc::string::format( "GMCtrolReleaseCTFBuff ctf buff reset chanum: %1%", pTarget->CharDbNum() ) );
			}
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::GmMakeItem( GLChar* pChar, const SNATIVEID& ItemID, DWORD ItemNum )
{
    if ( !pChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( std::string( "Can not find GetCharByDbNum on GmMakeItem" ) );
        return FALSE;
	}

    const SITEM* pItem = GLogicData::GetInstance().GetItem( ItemID );
    if ( !pItem )
        return FALSE;

    WORD wNums = ItemNum ? ( WORD )ItemNum : 1;

    // 인벤에 여유 공간이 있는지 검사
    BOOL bITEM_SPACE( FALSE );
    if ( pItem->ISPILE() )
    {
        //	겹침 아이템일 경우.
        WORD wPILENUM = pItem->sDrugOp.wPileNum;
        SNATIVEID sNID = pItem->sBasicOp.sNativeID;

        //	넣기 요청된 아이템수. ( 잔여량. )
        WORD wREQINSRTNUM = ( wNums * pItem->GETAPPLYNUM() );

        bITEM_SPACE = pChar->m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );

        //	인벤에 여유공간이 없을 경우 취소됨.
        if ( !bITEM_SPACE )
            return FALSE;

        //	아이템의 소유 이전 경로 기록.
        SITEMCUSTOM sDB_ITEM( sNID );
        sDB_ITEM.cGenType  = EMGEN_CHEAT;
        sDB_ITEM.wTurnNum  = wPILENUM;
        sDB_ITEM.tBORNTIME = CTime::GetCurrentTime().GetTime();

		// 통계 및 로그 수정
        pChar->LogItemExchange( 
            sDB_ITEM, 
            gamelog::ID_CHAR, 0, 
            gamelog::ID_CHAR, pChar->m_CharDbNum, 
            ITEM_ROUTE_CHAR, sDB_ITEM.wTurnNum, 
			( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_GM, 
			true );

        // 아이템을 인벤에 넣어준다.
        std::vector< UPDATED_ITEM > vecInsertedItems;
        pChar->InsertPileItem( sDB_ITEM, wREQINSRTNUM, vecInsertedItems, true );
    }
    else
    {
		for( WORD i = 0; i < wNums; ++i )
		{
			//	일반 아이템의 경우.
			WORD wInsertPosX( 0 ), wInsertPosY( 0 );
			bITEM_SPACE = pChar->m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

			//	인벤에 여유공간이 없을 경우 취소됨.
			if ( !bITEM_SPACE )
				return FALSE;

			// 새로운 아이템을 생성합니다
			SNATIVEID sNID = ItemID;

			SITEMCUSTOM sITEM_NEW( sNID );
			CTime cTIME = CTime::GetCurrentTime();
			sITEM_NEW.tBORNTIME = cTIME.GetTime();
			sITEM_NEW.wTurnNum = 1;
			sITEM_NEW.cGenType = EMGEN_CHEAT;
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			pChar->LogItemBasicStatOption( sITEM_NEW );
			pChar->LogItemLinkSkillOption( sITEM_NEW );
			pChar->LogItemAddonOption( sITEM_NEW );

			if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
				pChar->LogItemRandomOption( sITEM_NEW );

			// 인벤토리에 넣어줍니다
			if ( !pChar->InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, GM_MAKE_ITEM, true, false, true ) )
				return FALSE;

			// 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
			pChar->LogItemExchange(
				sITEM_NEW,
				gamelog::ID_CHAR, 0,
				gamelog::ID_CHAR,  pChar->m_CharDbNum,
				ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
				( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_GM, 
				true );
		}
    }

  /*  if ( pChar->m_bTracingUser )
    {
        GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
        TracingMsg.nUserNum  = pChar->GetUserID();
        TracingMsg.SetAccount( pChar->m_szUID );

        CString strTemp;
        strTemp.Format(
            "Cheating /makeitem, [%s][%s], Item:[%d,%d], Nums:[%d]",
            pChar->m_szUID, pChar->m_szName, ItemID.Mid(), ItemID.Sid(), wNums );

        TracingMsg.SetLog( strTemp.GetString() );
        pChar->SendToAgent( &TracingMsg );
    }*/

    return TRUE;
}

BOOL GLGaeaServer::GMCtrolMoPrintCrowList(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_PRINT_CROWLIST_FLD* pNetMsg = (GLMSG::SNET_GM_PRINT_CROWLIST_FLD*)nmg;
	
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMoPrintCrowList Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }

	GLChar* pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	if( pCHAR->m_dwUserLvl < USER_USER_GM )
	{
		return FALSE;
	}

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	//	Note : 가시 영역에 있는 모든 셀의 오브젝트들을 탐색한다.
	//
	int i;
	PGLCROW		pCrow;
	PCROWDATA	pCrowData;
	DWORD		dwID;

	EMCROW emCrow; 
	if ( pNetMsg->dwFindMob == 1 ) emCrow = CROW_MOB;
	else if ( pNetMsg->dwFindMob == 2 ) emCrow = CROW_NPC;

	std::map<DWORD, int> vecCrowDataList;
	for ( i = 0; i < MAXCROW; i++ )
	{
		pCrow = NULL;
		pCrowData = NULL;
		
		pCrow = pLand->GetCrow( i );
		if( pCrow == NULL )
			continue;

		pCrowData = pCrow->m_pCrowData;
		if( pCrowData == NULL )
			continue;

		if( pNetMsg->dwFindMob !=  0 )
		{
			if( pCrowData->m_emCrow != emCrow )
				continue;

			if( pCrowData->m_emCrow != emCrow )
				continue;
		}


		dwID = pCrow->m_sNativeID.dwID;
		
		if( vecCrowDataList.count( dwID ) == 0 )
		{
			vecCrowDataList.insert( std::make_pair( dwID, 1 ) );
		}
		else
		{
			vecCrowDataList[dwID] += 1;
		}
		
	}

	std::map<DWORD, int>::iterator iter = vecCrowDataList.begin();
	std::map<DWORD, int>::iterator iter_end = vecCrowDataList.end();

	GLMSG::SNET_GM_PRINT_CROWLIST_FB NetMsgFB;
	NetMsgFB.emCrow = emCrow;

	for ( ; iter != iter_end; ++iter )
	{
		SNATIVEID sID = iter->first;
		int nNum = iter->second;

		
		NetMsgFB.mID	= sID.wMainID;
		NetMsgFB.sID	= sID.wSubID;
		NetMsgFB.wNum = nNum;		

		SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgFB );
	}



	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobGen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_GEN_FLD *pNetMsg = (GLMSG::SNET_GM_MOB_GEN_FLD *)nmg;
	
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMobGen Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }

	SNATIVEID sID(pNetMsg->wMainID,pNetMsg->wSubID);
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sID );
	if ( !pCrowData )
		return FALSE;

	GLChar* pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		pLand->DropMaterial ( sID, pNetMsg->wPosX, pNetMsg->wPosY );
	else	
		pLand->DropCrow(sID, pNetMsg->wPosX, pNetMsg->wPosY);

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobGenEx ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_GEN_EX_FLD *pNetMsg = (GLMSG::SNET_GM_MOB_GEN_EX_FLD *)nmg;

    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMobGenEx Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }

	SNATIVEID sMobID(pNetMsg->wMobMID,pNetMsg->wMobSID);
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sMobID );
	if ( !pCrowData )
		return FALSE;

	const SNATIVEID sMapID(pNetMsg->wMapMID,pNetMsg->wMapSID);
	GLLandMan* pLandMan = GetLand ( sMapID );
	if ( !pLandMan )
		return FALSE;

	D3DXVECTOR3 vBasePos(0,0,0), vGenPos(0,0,0);
	const GLMapAxisInfo& MapAxisInfo = pLandMan->GetMapAxisInfo();
	MapAxisInfo.MapPos2MiniPos ( pNetMsg->wPosX, pNetMsg->wPosY, vBasePos.x, vBasePos.z );
	vBasePos.x -= (float)pNetMsg->wRange*50;
	vBasePos.z -= (float)pNetMsg->wRange*50;
	DWORD wRange = pNetMsg->wRange*100;

	if ( wRange < pNetMsg->wNum )
		return FALSE;

	std::vector<DWORD> vecRandPosX, vecRandPosZ;
	for ( DWORD i = 0; i < wRange; ++i ) vecRandPosX.push_back(i);
	std::random_shuffle ( vecRandPosX.begin(), vecRandPosX.end() );
	for ( DWORD i = 0; i < wRange; ++i ) vecRandPosZ.push_back(i);
	std::random_shuffle ( vecRandPosZ.begin(), vecRandPosZ.end() );

	for ( DWORD i = 0; i < pNetMsg->wNum; ++i )
	{
		vGenPos.x = (float)(vBasePos.x + vecRandPosX[i]);
		vGenPos.z = (float)(vBasePos.z + vecRandPosZ[i]);

		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		{
			DWORD dwGlobID = pLandMan->DropMaterial ( sMobID, vGenPos.x, vGenPos.z );
			pLandMan->m_listDelGlobIDExMat.push_back(dwGlobID);
		}
		else 
		{
			DWORD dwGlobID = pLandMan->DropCrow ( sMobID, vGenPos.x, vGenPos.z );
			pLandMan->m_listDelGlobIDEx.push_back(dwGlobID);
		}
	}

	return TRUE;
}


BOOL GLGaeaServer::GMCtrolMobDelEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_DEL_EX_FLD* pNetMsg = (GLMSG::SNET_GM_MOB_DEL_EX_FLD*)nmg;
	
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMobDelEx Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }
	SNATIVEID sMobID(pNetMsg->wMobMID,pNetMsg->wMobSID);
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sMobID );
	if ( !pCrowData )
		return FALSE;

	const SNATIVEID sMapID(pNetMsg->wMapMID,pNetMsg->wMapSID);
	GLLandMan* pLandMan = GetLand ( sMapID );
	if ( !pLandMan )
		return FALSE;

	if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
	{
		LISTDELMATERIALID_ITER iter = pLandMan->m_listDelGlobIDExMat.begin();
		LISTDELMATERIALID_ITER iter_end = pLandMan->m_listDelGlobIDExMat.end();
		for ( ;iter != iter_end; ) 
		{
			LISTDELMATERIALID_ITER iter_cur = iter++;
			PGLMATERIAL pMaterial = pLandMan->GetMaterial ( *iter_cur );
			if ( !pMaterial ) continue;

			if ( pMaterial->m_sNativeID == sMobID ) 
			{
				pLandMan->DropOutMaterial ( *iter_cur );
				pLandMan->m_listDelGlobIDExMat.erase(iter_cur);
			}
		}
	}
	else
	{
		LISTDELCROWID_ITER iter = pLandMan->m_listDelGlobIDEx.begin();
		LISTDELCROWID_ITER iter_end = pLandMan->m_listDelGlobIDEx.end();
		for ( ;iter != iter_end; ) 
		{
			LISTDELCROWID_ITER iter_cur = iter++;
			PGLCROW pCrow = pLandMan->GetCrow ( *iter_cur );
			if ( !pCrow ) continue;

			if ( pCrow->m_sNativeID == sMobID ) 
			{
				pLandMan->DropOutCrow ( *iter_cur );
				pLandMan->m_listDelGlobIDEx.erase(iter_cur);
			}
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobDel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_DEL_FLD *pNetMsg = (GLMSG::SNET_GM_MOB_DEL_FLD *)nmg;
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMobDel Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }

	GLChar* pCHAR = GetChar ( pNetMsg->dwGaeaID );
	if ( pCHAR == NULL )
		return FALSE;

	GLLandMan* pLandMan(GetLand ( pCHAR->GetCurrentMap().getGaeaMapID() ));
	if( pLandMan == NULL )
		return FALSE;

	if ( pNetMsg->sMobID == NATIVEID_NULL() ) 
	{
		if ( pNetMsg->bMaterial ) 
		{
			return pLandMan->DropOutMaterial();
		}
		else
		{
			return pLandMan->DropOutCrow();
		}
	}
	else
	{
		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pNetMsg->sMobID );
		if ( !pCrowData )
			return FALSE;

		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		{
			LISTDELMATERIALID_ITER iter = pLandMan->m_listDelGlobIDMat.begin();
			LISTDELMATERIALID_ITER iter_end = pLandMan->m_listDelGlobIDMat.end();			

			for ( ;iter != iter_end; ) 
			{
				LISTDELMATERIALID_ITER iter_cur = iter++;
				PGLMATERIAL pMaterial = pLandMan->GetMaterial ( *iter_cur );
				if ( !pMaterial ) continue;

				if ( pMaterial->m_sNativeID == pNetMsg->sMobID ) 
				{
					pLandMan->DropOutMaterial ( *iter_cur );
				}
			}
		}
		else
		{
			LISTDELCROWID_ITER iter = pLandMan->m_listDelGlobID.begin();
			LISTDELCROWID_ITER iter_end = pLandMan->m_listDelGlobID.end();

			for ( ;iter != iter_end; ) 
			{
				LISTDELCROWID_ITER iter_cur = iter++;
				PGLCROW pCrow = pLandMan->GetCrow ( *iter_cur );
				if ( !pCrow ) continue;

				if ( pCrow->m_sNativeID == pNetMsg->sMobID ) 
				{
					pLandMan->DropOutCrow ( *iter_cur );
				}
			}
		}

	}

	return TRUE;
}


BOOL GLGaeaServer::GMCtrolMobLevel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_LEV *pNetMsg = (GLMSG::SNET_GM_MOB_LEV *)nmg;
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMobLevel Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }
	
	SNATIVEID nidMAP = SNATIVEID(pNetMsg->wMAP_MID,pNetMsg->wMAP_SID);
	if ( nidMAP==SNATIVEID(false) )
		return FALSE;

	GLLandMan *pLandMan = GetLand ( nidMAP );
	if ( !pLandMan )
		return FALSE;

	pLandMan->SetMobSchManRegenLayer(pNetMsg->dwLayer);


	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobLevelLayer(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_LEV_LAYER *pNetMsg = (GLMSG::SNET_GM_MOB_LEV_LAYER *)nmg;
	if ( !m_bEmulator )
	{
		if(dwClientID != m_dwAgentSlot)
		{
			sc::writeLogError( sc::string::format("GMCtrolMobLevel Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
			return FALSE;
		}
	}

	SNATIVEID nidMAP = SNATIVEID(pNetMsg->wMAP_MID,pNetMsg->wMAP_SID);
	if ( nidMAP==SNATIVEID(false) )
		return FALSE;

	GLLandMan *pLandMan = GetLand ( nidMAP );
	if ( !pLandMan )
		return FALSE;

	pLandMan->SetMobSchManRegenLayer(pNetMsg->unLayer);

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolMobLevelClear(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_LEV_CLEAR *pNetMsg = (GLMSG::SNET_GM_MOB_LEV_CLEAR *)nmg;
    if ( !m_bEmulator )
    {
        if(dwClientID != m_dwAgentSlot)
        {
            sc::writeLogError( sc::string::format("GMCtrolMobLevelClear Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
            return FALSE;
        }
    }

	SNATIVEID nidMAP = SNATIVEID(pNetMsg->wMAP_MID,pNetMsg->wMAP_SID);
	if ( nidMAP==SNATIVEID(false) )
		return FALSE;

	GLLandMan *pLandMan = GetLand ( nidMAP );
	if ( !pLandMan )
		return FALSE;

	pLandMan->SetMobSchManRegenLayer(0);

	return TRUE;
}
BOOL GLGaeaServer::GMCtrolMobLevelList(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_MOB_LEV_LIST *pNetMsg = (GLMSG::SNET_GM_MOB_LEV_LIST *)nmg;
	if ( !m_bEmulator )
	{
		if(dwClientID != m_dwAgentSlot)
		{
			sc::writeLogError( sc::string::format("GMCtrolMobLevelList Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
			return FALSE;
		}
	}
	const DWORD _dwGaeaID(pNetMsg->dwGaeaID);
	GLChar* const _pGM(GetChar(_dwGaeaID));
	if ( _pGM == NULL )
		return FALSE;

	SNATIVEID nidMAP = SNATIVEID(pNetMsg->wMAP_MID,pNetMsg->wMAP_SID);
	if ( nidMAP==SNATIVEID(false) )
		return FALSE;

	GLLandMan *pLandMan = GetLand ( nidMAP );
	if ( !pLandMan )
		return FALSE;
	
	GLMSG::SNET_GM_MOB_LEV_LIST_FC NetMsg;
	NetMsg.wMAP_MID = pNetMsg->wMAP_MID;
	NetMsg.wMAP_SID = pNetMsg->wMAP_SID;
	NetMsg.nLayer = pLandMan->GetMobSchManCount();

	SENDTOCLIENT(_pGM->GETCLIENTID(), &NetMsg);
    return TRUE;
}

BOOL GLGaeaServer::GMCtrolChatBlockFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrolChatBlockFld Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNET_GM_CHAT_BLOCK_FLD *pNetMsg = (GLMSG::SNET_GM_CHAT_BLOCK_FLD *)nmg;
	GLChar* pCHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	if ( !pCHAR )
		return FALSE;

	if ( pNetMsg->dwBLOCK_MINUTE!=0 )
	{
		//	블럭시간 지정

		CTime cBLOCK = CTime::GetCurrentTime();
		CTimeSpan cSPAN(0,0,pNetMsg->dwBLOCK_MINUTE,0);
		cBLOCK += cSPAN;

		pCHAR->m_tCHATBLOCK = cBLOCK.GetTime();
	}
	else
	{
		//	블럭시간 해지.
		pCHAR->m_tCHATBLOCK = 0;

	}

	return TRUE;
}

BOOL GLGaeaServer::UserCtrolCharInfoFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_USER_CHAR_INFO_FLD *pNetMsg = (GLMSG::SNET_USER_CHAR_INFO_FLD *)nmg;

    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("UserCtrolCharInfoFld Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }


	
	GLChar* pCHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	if ( !pCHAR )
		return FALSE;

	int nPosX(0), nPosY(0);

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand != NULL )
	{
		const D3DXVECTOR3 vPosition = pCHAR->GetPosition();
		pLand->GetMapAxisInfo().Convert2MapPos ( vPosition.x, vPosition.z, nPosX, nPosY );
	}

	std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pCHAR->m_ClubDbNum );

	GLMSG::SNET_USER_CHAR_INFO_FLD_FB NetMsgFB;

	if ( pCLUB )
		StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, pCLUB->m_szName );
	else
		StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, "void" );

	NetMsgFB.dwChaNumRequest = pNetMsg->dwChaNumRequest;
	//NetMsgFB.dwCLIENTID_FB = pNetMsg->dwCLIENTID_FB;
	SENDTOAGENT(&NetMsgFB);

	return TRUE;
}

BOOL GLGaeaServer::GMCtrolCharInfoFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_CHAR_INFO_FLD *pNetMsg = (GLMSG::SNET_GM_CHAR_INFO_FLD *)nmg;
	
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("GMCtrolCharInfoFld Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }


	GLChar* pCHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	if ( !pCHAR )
		return FALSE;
	
	int nPosX(0), nPosY(0);
	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand != NULL )
	{
		const D3DXVECTOR3 vPosition = pCHAR->GetPosition();
		pLand->GetMapAxisInfo().Convert2MapPos(vPosition.x, vPosition.z, nPosX, nPosY);
	}

	std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pCHAR->m_ClubDbNum );

	GLMSG::SNET_GM_CHAR_INFO_FLD_FB NetMsgFB;
	NetMsgFB.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFB.nidMAP = pCHAR->GetCurrentMap().getGaeaMapID();
	NetMsgFB.wLEVEL = pCHAR->m_wLevel;
	NetMsgFB.wPOSX = nPosX;
	NetMsgFB.wPOSY = nPosY;
	NetMsgFB.sHP = pCHAR->m_sHP;
	NetMsgFB.sMP = pCHAR->m_sMP;
	NetMsgFB.sSP = pCHAR->m_sSP;
	NetMsgFB.sEXP = pCHAR->m_sExperience;

	if ( pCLUB )
		StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, pCLUB->m_szName );
	else
		StringCchCopy ( NetMsgFB.szCLUB, CHAR_SZNAME, "void" );

	//NetMsgFB.dwCLIENTID_FB = pNetMsg->dwCLIENTID_FB;
	NetMsgFB.dwChaNumRequest = pNetMsg->dwChaNumRequest;
	SENDTOAGENT(&NetMsgFB);
	
	return TRUE;
}

BOOL GLGaeaServer::RequestViewAllPlayer(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GM_VIEWALLPLAYER_FLD_REQ *pNetMsg = (GLMSG::SNET_GM_VIEWALLPLAYER_FLD_REQ *)nmg;
	GLChar* pCHAR = GetChar ( pNetMsg->dwID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->UserLevel() < USER_GM3 )
		return FALSE;

	GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB NetMsgFB;

	GLLandMan* const pLand(pCHAR->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	LANDQUADTREE* pLandTree = pLand->GetLandTree();
	if ( !pLandTree )
		return FALSE;

	// 맵의 크기를 얻어온다.
	D3DXVECTOR3 vMax, vMin;
	pLand->GetNaviMeshAABB ( vMax, vMin );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect( (int)vMax.x,(int)vMax.z,(int)vMin.x,(int)vMin.z );
	pLandTree->FindNodes ( bRect, pLandTree->GetRootNode(), &pQuadNode );

    DWORD dwPlayerNum = 0;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;
		if( !pLandNode ) continue;

		GASSERT(pLandTree->GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		//	Note : - Char -
		//
		GLCHARNODE *pCharCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCharCur; pCharCur = pCharCur->pNext )
		{
			GLChar* pGLChar = pCharCur->Data;
			if( !pGLChar ) continue;
			if ( pGLChar->ClientSlot() == pCHAR->ClientSlot() )	continue;

			// 여기서 패킷에 실어서 보내주면 된다
			GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB NetMsgFB;
			NetMsgFB.dwCHARID = pGLChar->m_CharDbNum;
			StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pGLChar->m_szName );

			SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgFB );
			dwPlayerNum++;
		}
	}
	NetMsgFB.dwPlayerNum = dwPlayerNum;
	SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsgFB );

	return TRUE;
}

BOOL GLGaeaServer::ServerLevelEventEnd ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_LEVEL_EVENT_END_FLD* pNetMsg = static_cast< GLMSG::SNET_LEVEL_EVENT_END_FLD* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLLandMan* pLandMan = GetLand( pNetMsg->dwMapID );
	if ( pLandMan )
	{
		SNATIVEID sMapID( pNetMsg->dwHallMapID );

		const SMAPNODE* pMapNode = FindMapNode( sMapID.dwID );
		if ( NULL == pMapNode )
			return FALSE;
       
        std::tr1::shared_ptr< GLMapAxisInfo > spAxisInfo = GetAxisInfo( pMapNode->m_LevelFile );
        
		MoveSystem::SMOVE_INFO sMoveInfo;
		sMoveInfo.sMapID = sMapID;
		
		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pChar = pCharNode->Data;
			if ( NULL == pChar )
				continue;

			/// 이동 위치를 구한다;
			int nRandNum = sc::Random::getInstance().GetValue() % MAX_EVENT_POS;
			GLLEV_POS levPos = pNetMsg->levPos[nRandNum];
			D3DXVECTOR3 vPOS(0.f , 0.f, 0.f ); 
            if ( spAxisInfo )
			    spAxisInfo->MapPos2MiniPos( levPos.m_dwPOS_X, levPos.m_dwPOS_Y, vPOS.x, vPOS.z );

			/// 이동정보를 세팅한다;
			sMoveInfo.dwChaDBNum = pChar->CharDbNum();
			sMoveInfo.vPosition = vPOS;

			/// 이동한다;
			MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );

			/*NetMsgAg.dwChaNum = pChar->CharDbNum();
			NetMsgAg.vPosition = vPOS;

			SENDTOAGENT(&NetMsgAg);	//pChar->ClientSlot()

			// 팻을 제거해준다
			DropOutPET ( pChar->m_dwPetGUID, true, true );

			// VEHICLE
			SaveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), true );

			// SUMMON
			DropOutAllSummon ( pChar );*/
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerLevelEventWarning( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_LEVEL_EVENT_WARNING_FLD * pNetMsg = (GLMSG::SNET_LEVEL_EVENT_WARNING_FLD *)nmg;
	SNATIVEID nidMAP( pNetMsg->dwMapID );
	GLLandMan *pLandMan = GetLand( nidMAP );
	if( pLandMan )
	{
		GLMSG::SNET_GM_WARNING_MSG_BRD NetMsgBrd;
		NetMsgBrd.bOn = pNetMsg->bOn;

		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pChar = pCharNode->Data;
			if( !pChar ) continue;

			SENDTOCLIENT( pChar->ClientSlot(), &NetMsgBrd );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::ServerLevelEventCountdown( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD * pNetMsg = (GLMSG::SNET_LEVEL_EVENT_COUNTDOWN_FLD *)nmg;
	SNATIVEID nidMAP( pNetMsg->dwMapID );
	GLLandMan *pLandMan = GetLand( nidMAP );
	if( pLandMan )
	{
		GLMSG::SNET_GM_COUNTDOWN_MSG_BRD NetMsgBrd;
		NetMsgBrd.nCount = pNetMsg->nCount;

		GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pChar = pCharNode->Data;
			if( !pChar ) continue;

			SENDTOCLIENT( pChar->ClientSlot(), &NetMsgBrd );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestChargedItem2Inven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// 구매한 아이템(아이템뱅크)에서 아이템 가져오는 메시지
	// 실제 dbaction PurchaseItem_Get 에서 InsertMsg 로 넣어주는 메시지인데, 클라이언트 슬롯은 요청한 클라이언트 슬롯으로 넣어준다.
	// 0으로 넣어주는게 아닌, 클라가 직접 이 메시지를 보내면? 그냥 아이템 넣어주도록 되어있다.
	// 랜덤옵션 생성은 일본의 경우만 하도록 되어있는데...
	// 구입한 목록에 있는지 없는지 확인하는 코드 삽입
	// 디비액션에서 클라이언트 0으로 하고 패킷안에 캐릭터번호 넣어서 하려고 했는데, GLGaeaServer 메시지처리에서 클라이언트아이디 0이면 처리하지않도록 되어있다.
	// 구매목록을 확인하는 코드넣으면 찾아가지 않은 것로 계속 변경할 수 있는 문제가 생길 수 있다. 롤백하는 코드를 같이 사용하면
	// 캐릭터에 없는 구매코드를 보내면 그 구매코드는 롤백되도록 할 수 있다.
	// 그래서 로직대로라면 이 메시지는 디비액션에서만 온다. 미리 구매코드 확인후에
	// 그래서 캐릭터에 없는 구매코드라면 로그 남기고 그냥 종료하도록 수정한다.
	// pPacket->szPurKey 체크 필요 
	GLMSG::SNET_CHARGED_ITEM2_INVEN* pPacket = ( GLMSG::SNET_CHARGED_ITEM2_INVEN* ) nmg;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
    {
        PurchaseItemRestoreDB( pPacket->szPurKey );
        return FALSE;
    }

	// 구매한 아이템이 맞는지 확인한다. 자세한 설명은 위에
	MAPSHOP_ITER posCharged = pChar->m_mapCharged.find( pPacket->szPurKey );
	if ( pChar->m_mapCharged.end() == posCharged )
	{
		// 로그
		sc::writeLogError( sc::string::format( "Hacking RequestChargedItem2Inven szPurKey error chanum: %1%", pChar->CharDbNum() ) );
		return FALSE;
	}

	if ( pPacket->dwUserID != pChar->GetUserID() )
    {
        PurchaseItemRestoreDB( pPacket->szPurKey );
        return FALSE;
    }

	SITEM* pITEM( NULL );
	pITEM = GLogicData::GetInstance().GetItem( pPacket->nidITEM );
	if ( !pITEM )
    {
        PurchaseItemRestoreDB( pPacket->szPurKey );
        return FALSE;
    }

	WORD wPosX( 0 ), wPosY( 0 );
	BOOL bOK = pChar->InvenFindInsertable( wPosX, wPosY );
	if ( !bOK )
    {
        PurchaseItemRestoreDB( pPacket->szPurKey );
        return FALSE;
    }

	// 아이템 발생.
    // 아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
	SITEMCUSTOM sITEM_NEW;
    NewItemMake( sITEM_NEW, pPacket->nidITEM, EMGEN_BILLING );

    // 아이템 에디트에서 입력한 개조 등급 적용
	sITEM_NEW.cDAMAGE = static_cast< BYTE >( pITEM->sBasicOp.wGradeAttack );
	sITEM_NEW.cDEFENSE = static_cast< BYTE >( pITEM->sBasicOp.wGradeDefense );
	sITEM_NEW.SetDbState( db::DB_INSERT );

	sITEM_NEW.GenerateBasicStat( false );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption();

	m_pItemLimit->LogItemBasicStatOption( sITEM_NEW );
	m_pItemLimit->LogItemLinkSkillOption( sITEM_NEW );
	m_pItemLimit->LogItemAddonOption( sITEM_NEW );

	// 일본에서만 유료 아이템에 랜덤 옵션 적용
	if ( m_ServiceProvider == SP_JAPAN )
	{
		if ( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
		{
			m_pItemLimit->LogItemRandomOption( sITEM_NEW );
		}
	}

	// 인벤토리에 넣고 뺄때 리턴값 체크 꼭 필요
	if ( !pChar->InvenInsertItem( sITEM_NEW, wPosX, wPosY, true, INSERT_INVEN_ITEM_CHARGED_ITEM, true, false, true ) )
	{
		PurchaseItemRestoreDB( pPacket->szPurKey );
		return FALSE;
	}

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	m_pItemLimit->LogItemExchange(
        sITEM_NEW,
        gamelog::ID_CHAR, 0,
        gamelog::ID_CHAR, pChar->m_CharDbNum,
        ITEM_ROUTE_CHAR,
        sITEM_NEW.wTurnNum,
		( pChar == NULL ) ? MapID( SNATIVEID( false ),SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_PURCHASE, 
        false );

	// 구입한 목록에서 제거
	MAPSHOP_ITER pos = pChar->m_mapCharged.find( pPacket->szPurKey );
	if ( pos != pChar->m_mapCharged.end() )
	{
        // 구매로그 전송
        const SHOPPURCHASE& PurchaseItem = pos->second;
        NET_CHAR_WEB_SHOP_LOG MsgFH(
            SNATIVEID( PurchaseItem.wItemMain, PurchaseItem.wItemSub ),
            PurchaseItem.m_Price,
            pChar->GetUserID(),
            pChar->CharDbNum(),        
            pChar->GetLevel(),
            m_pMsgServer->GetServerGroup() );
        SENDTOCACHE( &MsgFH );

		pChar->m_mapCharged.erase( pos );
	}

	// 클라이언트의 구입된 목록 리스트에서 제거.
	GLMSG::SNET_CHARGED_ITEM_DEL NetMsgDel;
	NetMsgDel.dwID = pPacket->dwInvenPos;
	SENDTOCLIENT( pChar->ClientSlot(), &NetMsgDel );
	return TRUE;
}

void GLGaeaServer::PurchaseItemRestoreDB(const char* szPurKey)
{
    if (szPurKey)
    {
        AddGameDbJob(
            db::DbActionPtr(
                new db::PurchaseItem_Restore(szPurKey)));
    }
}

BOOL GLGaeaServer::RequestConftSPtyExp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CONFT_SPTY_EXP_FLD *pNetMsg = (GLMSG::SNET_CONFT_SPTY_EXP_FLD *)nmg;
	GLCHARNODE *pCharNode = m_GaeaPCList.m_pHead;
	for ( ; pCharNode; pCharNode=pCharNode->pNext )
	{
		GLChar *pChar = pCharNode->Data;

		if ( pChar->m_wSchool != pNetMsg->wSCHOOL )	continue;

		int nExp = int ( pChar->m_sExperience.lnMax * pNetMsg->fEXP_RATE );
		pChar->ReceiveExp ( nExp, true );
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestGenItemFieldFB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwClientID != m_dwAgentSlot )
	{
		return FALSE;
	}

	GLMSG::SNET_REQ_GENITEM_FLD_FB* pNetMsg = ( GLMSG::SNET_REQ_GENITEM_FLD_FB* ) nmg;
	GLLandMan* pLandMan = GetLand( pNetMsg->sMAPID );
	if ( !pLandMan )
        return FALSE;
	pLandMan->RequestGenItemFieldFB( pNetMsg );
	return TRUE;
}

// *****************************************************
// Desc: 이동할 케릭터의 위치 반환
// *****************************************************
BOOL GLGaeaServer::RequestMove2CharPos(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("RequestMove2CharPos Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

	GLMSG::SNETPC_GM_MOVE2CHAR_POS *pNetMsg = (GLMSG::SNETPC_GM_MOVE2CHAR_POS *)nmg;

	// 에이전트로 회신할 메세지
	GLMSG::SNETPC_GM_MOVE2CHAR_AG NetMsgAG;
	NetMsgAG.dwCOMMAND_CHARID = pNetMsg->dwCOMMAND_CHARID;
	NetMsgAG.dwTO_CHARID = pNetMsg->dwTO_CHARID;

	// 이동할 케릭터 유효성
	GLChar* pTO_CHAR = GetCharByDbNum ( pNetMsg->dwTO_CHARID );
	if ( !pTO_CHAR )
	{
		NetMsgAG.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOAGENT(&NetMsgAG);
		return FALSE;
	}

	// 이동할 맵의 유효성
	GLLandMan* const pLand(pTO_CHAR->GetLandMan());
	if ( pLand == NULL )
	{
		NetMsgAG.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOAGENT(&NetMsgAG);
		return FALSE;
	}

	BOOL bCoB(FALSE);
	DWORD dwCoB(0);
	D3DXVECTOR3 vCoB(0,0,0);
	
	const D3DXVECTOR3& vPosition = pTO_CHAR->GetPosition();
	D3DXVECTOR3 vec2POS = vPosition;
	D3DXMATRIX matRotY;

	boost::shared_ptr<NavigationMesh> spNavi = pLand->GetNavi();
	if ( spNavi )
	{
		for ( float fAngle=0.0f; fAngle<D3DX_PI*2; fAngle += D3DX_PI/4.0f )
		{
			D3DXVECTOR3 vecDIR(1,0,0);
			D3DXMatrixRotationY ( &matRotY, fAngle );

			D3DXVec3TransformCoord ( &vecDIR, &vecDIR, &matRotY );
			D3DXVECTOR3 vPos = vPosition + vecDIR*10.0f;

			spNavi->IsCollision ( D3DXVECTOR3(vPos.x,vPos.y+20.0f,vPos.z), D3DXVECTOR3(vPos.x,vPos.y-30.0f,vPos.z), vCoB, dwCoB, bCoB );
			if ( bCoB )
			{
				vec2POS = vCoB;
				break;
			}
		}
	}

	//	Note : 위치 반환.
	NetMsgAG.emFB = EMGM_MOVE2CHAR_FB_OK;
	NetMsgAG.dwFieldServerIDTo = m_dwFieldSvrID;
	NetMsgAG.mapIDTo = pTO_CHAR->GetCurrentMap();
	NetMsgAG.vToPos = vec2POS;
	SENDTOAGENT(&NetMsgAG);

	return TRUE;
}

BOOL GLGaeaServer::CheckMarketState( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLMSG::SNETPC_MARKETCHECK *pNetMsg = (GLMSG::SNETPC_MARKETCHECK*)nmg;

	GLChar* pChar = GetChar ( dwGaeaID );

	if( pChar == NULL )
	{
		sc::writeLogError(sc::string::format("%%%% Could not found Character..%d %%%%", dwGaeaID ));
		return FALSE;
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestNpcRecall( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_NPC_RECALL* pNetMsg = ( GLMSG::SNET_INVEN_NPC_RECALL* )nmg;
		
	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	GLMSG::SNET_INVEN_NPC_RECALL_FB	NetMsgFB;

	//	인벤의 아이템 확인.
	SINVENITEM* pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return FALSE;

	//	아이템 정보 가져옴.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_NPC_RECALL )
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}
	
	// Note : 레벨 조건 확인
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return FALSE;
	}

	SNATIVEID sID( pItem->sBasicOp.sSubID );
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( sID );
	if ( !pCrowData )
	{		
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_NPC;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLLandMan* pLandMan( GetLand( pChar->GetCurrentMap().getGaeaMapID() ) );
	if ( !pLandMan )
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	const D3DXVECTOR3& vPosition = pChar->GetPosition();
	DWORD dwMobID = pLandMan->DropCrow( sID, vPosition.x, vPosition.z );

	if ( dwMobID == UINT_MAX ) 
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_POS;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	PGLCROW pCrow = pLandMan->GetCrow( dwMobID );
    if ( !pCrow ) 
	{
		NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_POS;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	pCrow->m_bNpcRecall = true;
	pCrow->m_dwCallCharID = pChar->m_CharDbNum;
	pCrow->m_dwCallUserID = pChar->GetUserID();
	pCrow->m_tGenTime = CTime::GetCurrentTime().GetTime();

	//	[자신에게] 인밴 아이탬 소모시킴.
	pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

	NetMsgFB.emFB = EMREQ_NPC_RECALL_FB_OK;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	return TRUE;
}


BOOL GLGaeaServer::RequestNpcCommission( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_NPC_COMMISSION_FB* pNetMsg = (GLMSG::SNET_INVEN_NPC_COMMISSION_FB*)nmg;
    if (m_dwAgentSlot != dwClientID)
    {        
        std::string HackMsg(
            sc::string::format(
                "Hacking! RequestNpcCommission not agent slot. ChaDbNum %1% Commission %2%",
                pNetMsg->dwCharID,
                pNetMsg->lnCommission));
        sc::writeLogError(HackMsg);
        return FALSE;
    }

    if (pNetMsg->m_MagicNum != GLMSG::SNET_INVEN_NPC_COMMISSION_FB::MAGIC_NUM)
    {
        std::string HackMsg(
            sc::string::format(
                "Hacking! RequestNpcCommission magic num mismatch. ChaDbNum %1% Commission %2% MagicNum %3%",
                pNetMsg->dwCharID,
                pNetMsg->lnCommission,
                pNetMsg->m_MagicNum));
        sc::writeLogError(HackMsg);
        return FALSE;
    }

	if ( pNetMsg->emFB == EMREQ_NPC_COMMISSION_CHAR )
	{
		GLChar* pChar = GetCharByDbNum( pNetMsg->dwCharID );
		if (!pChar)
			return FALSE;
		
		pChar->UpdateNpcCommission( pNetMsg->lnCommission );

	}
	else if ( pNetMsg->emFB == EMREQ_NPC_COMMISSION_DB )
	{
		//	DB에 창고머니 UPDATE
		SaveNpcCommissionDB( pNetMsg->dwCharID, pNetMsg->dwUserID, pNetMsg->lnCommission );	
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgEventExp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventExp(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventExpEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventExpEnd(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventItemGen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventItemGen(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventItemGenEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventItemGenEnd(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventGrade( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pEventProc->MsgEventGrade(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventGradeEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventGradeEnd(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventMoneyGen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventMoneyGen(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventMoneyGenEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventMoneyGenEnd(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventEx(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgEventExEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgEventExEnd(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgLimitEventBegin(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgLimitEventBegin(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgLimitEventTimeReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgLimitEventTimeReset(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgLimitEventEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgLimitEventEnd(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgClassEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	m_pEventProc->MsgClassEvent(nmg, dwClientID, dwGaeaID);
	return TRUE;
}

BOOL GLGaeaServer::MsgReady(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETREQ_READY *msg = (GLMSG::SNETREQ_READY *)nmg;

	if(!msg)
		return FALSE;

	DWORD dwChaNum = msg->dwChaNum;
	
	GLChar* pChar = GetChar64(msg->dwChaNum, msg->dwSvrGroupNum);

	if ( !pChar )
		return FALSE;

	pChar->MsgReady( nmg );
	return TRUE;	
}

//CAF or AF
BOOL GLGaeaServer::MsgActState(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_ACTSTATE *msg = (GLMSG::SNETPC_ACTSTATE *)nmg;

	if(!msg)
		return FALSE;

	DWORD dwChaNum		= msg->dwChaNum;
	DWORD dwSvrGroupNum = msg->dwSvrGroupNum;
	
	GLChar* pChar = GetChar64(dwChaNum, dwSvrGroupNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgActState(nmg);
	return TRUE;	
}

BOOL GLGaeaServer::MsgMoveState(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgMoveState( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgGoto(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_GOTO* pPacket = reinterpret_cast<GLMSG::SNETPC_GOTO*>( nmg );

	GLChar* pChar(GetChar(dwGaeaID == GAEAID_NULL ? pPacket->dwGaeaID : dwGaeaID));
	if ( pChar == NULL )
		return FALSE;

	pChar->MsgGoto( nmg );
	
	return TRUE;	
}

BOOL GLGaeaServer::MsgDominateMoveTo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgDominateMoveTo( nmg );
	
	return TRUE;
}

// AnimationMove 시 몸 회전을 다른 클라에 보낸다.
BOOL GLGaeaServer::MsgAniMoveStartCF( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgAniMoveStartCF( pMsg );

	return TRUE;
}

// AnimationMove로 이동된 자신의 마지막 위치를 서버로 전송한다.
BOOL GLGaeaServer::MsgAniMoveEndCF( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgAniMoveEndCF( pMsg );

	return TRUE;
}

BOOL GLGaeaServer::MsgAttack(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgAttack( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgAttackCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgAttackCancel( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgSetTarget(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgSetTarget( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqBuyFromNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqBuyFromNpc( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqExchangeBuyFromNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqExchangeBuyFromNpc( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqSaleToNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqSaleToNpc( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqResurrect(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_CHAR_REQ_RESURRECT_CF *msg = (GLMSG::NET_CHAR_REQ_RESURRECT_CF *)nmg;

    GLChar* const pChar = GetChar(dwGaeaID);
			
	if ( pChar == NULL )
	{
		sc::writeLogError("MsgReqResurrect() pChar == NULL");
        return FALSE;
	}

	/*if( pChar->VehicleActiveValue())
		pChar->ToggleVehicle(false);*/

    pChar->Resurrect((EMRESURRECTION_TYPE)msg->resurrectType);

    return TRUE;
}

BOOL GLGaeaServer::MsgReqLevelUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if (dwGaeaID == GAEAID_NULL)
		return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
		pChar->MsgReqLevelUp(nmg);
		return TRUE;
	}
	else
	{
		return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqJumpingLevelUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if (dwGaeaID == GAEAID_NULL)
		return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
		pChar->MsgReqJumpingLevelUp(nmg);
		return TRUE;
	}
	else
	{
		return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqJumpingLevelUpCA(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if (dwGaeaID == GAEAID_NULL)
		return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
		pChar->MsgReqJumpingLevelUpCA(nmg);
		return TRUE;
	}
	else
	{
		return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqJumpingLevelUpByExp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if (dwGaeaID == GAEAID_NULL)
		return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
		pChar->MsgReqJumpingLevelUpByExp(nmg);
		return TRUE;
	}
	else
	{
		return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqStatsUp(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if (pChar)
    {		
	    pChar->MsgReqStatsUp( nmg );
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqStatsUpEx(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar(dwGaeaID);
	if (pChar)
    {
	    pChar->MsgReqStatsUpEx( nmg );
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqActionQSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if (pChar)
    {		
	    pChar->MsgReqActionQSet( nmg );
	    return TRUE;	
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqActionQReSet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if (pChar)
    {	
	    pChar->MsgReqActionQReSet( nmg );
	    return TRUE;	
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqMoneyToField(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if (pChar)
    {		
	    pChar->MsgReqMoneyToField( nmg );
	    return TRUE;	
    }
    else
    {
        return FALSE;
    }
}

//CAF
BOOL GLGaeaServer::MsgReqReGenGate(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_REGEN_GATE *msg = (GLMSG::SNETPC_REQ_REGEN_GATE *)nmg;

	//GLChar* pChar = GetChar ( dwGaeaID );
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgReqReGenGate( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqCure(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if (pChar)
    {		
	    pChar->MsgReqCure( nmg );
	    return TRUE;	
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqMove2CharFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_GM_MOVE2CHAR_FLD *msg = (GLMSG::SNETPC_GM_MOVE2CHAR_FLD *)nmg;

	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgReqMove2CharFld( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqSkillRevivel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->Resurrect(EM_RESURRECTION_SKILL);
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqGetExpRecoveryNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqGetExpRecoveryNpc( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqRecoveryNpc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqRecoveryNpc( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqCharReset(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqCharReset( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqCharCard(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqCharCard( nmg );
	return TRUE;	
}



BOOL GLGaeaServer::MsgInvenVietnamGet(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgInvenVietnamGet( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqConFrontStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_CONFRONT_START2_FLD *msg = (GLMSG::SNETPC_CONFRONT_START2_FLD *)nmg;

	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgReqConFrontStart( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqConFrontEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_CONFRONT_END2_FLD *msg = (GLMSG::SNETPC_CONFRONT_END2_FLD *)nmg;
	
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return FALSE;
	pChar->MsgReqConFrontEnd( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqNpcItemTrade(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqNpcItemTrade( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqGesture(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqGesture( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgLoudSpeaker(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgLoudSpeaker( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqRebuildResult(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqRebuildResult( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqRebuildMoveItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqRebuildMoveItem( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqRebuildInputMoney(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqRebuildInputMoney( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqGarbageResult(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqGarbageResult( nmg );
	return TRUE;	
}
/*
BOOL GLGaeaServer::MsgReqPhoneNumber(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqPhoneNumber( nmg );
	return TRUE;	
}
*/
/*
BOOL GLGaeaServer::MsgPhoneNumber(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgPhoneNumber( nmg );
	return TRUE;	
}
*/
BOOL GLGaeaServer::MsgReqSendSMS(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqSendSMS( nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgSendSMS(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgSendSMS( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgItemShopOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgItemShopOpen( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqItemMix(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqItemMix( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqDialogueWindowOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqDialogueWindowOpen( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqNpcTalkBasic(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqNpcTalkBasic( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgReqGetRightOfItem (DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqGetRightOfItem( nmg );
	return TRUE;	
}


BOOL GLGaeaServer::MsgGathering(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgGathering( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgGatheringCancel(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;
	
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgGatheringCancel( nmg );
	return TRUE;	
}

BOOL GLGaeaServer::MsgBuyRebuyItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("MsgBuyRebuyItem Hacking Try [IP:%s]", GetClientIP(dwClientID) ) );
        return FALSE;
    }

    GLMSG::SNET_BUY_REBUY_ITEM_AF* pPacket = (GLMSG::SNET_BUY_REBUY_ITEM_AF*) nmg;
    if (pPacket->MagicNum != GLMSG::SNET_BUY_REBUY_ITEM_AF::MAGIC_NUMBER)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! MsgBuyRebuyItem %1% != %2%",
                pPacket->MagicNum,
                GLMSG::SNET_BUY_REBUY_ITEM_AF::MAGIC_NUMBER));
        return FALSE;
    }
    
    GLChar* pChar = GetCharByDbNum(pPacket->m_ChaDbNum);
    if (pChar)
    {
        pChar->MsgBuyRebuyItem(pPacket->sItemCustom, pPacket->Price);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReferCharPublic( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//GASSERT(dwGaeaID != GAEAID_NULL);
	if( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if( !pChar )
		return FALSE;

	ReferChar::SNET_MSG_REFER_CHAR_PUBLIC* pNetMsg = (ReferChar::SNET_MSG_REFER_CHAR_PUBLIC*)nmg;

	pChar->SetReferCharPublic( pNetMsg->m_bPublic );

	return TRUE;
}

BOOL GLGaeaServer::MsgReferCharRequest( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	ReferChar::SNET_MSG_REFER_CHAR_REQ* pNetMsg = (ReferChar::SNET_MSG_REFER_CHAR_REQ*)nmg;

	if( dwClientID != m_dwAgentSlot )
	{
		//GASSERT( dwGaeaID != GAEAID_NULL );
		if( dwGaeaID == GAEAID_NULL )
			return FALSE;

		GLChar* pChar = GetChar( dwGaeaID );
		if( !pChar )
			return FALSE;

		if( pChar->GetReferCharDelay() > 0.0f )
			return FALSE;

		pChar->SetReferCharDelay( GLCONST_CHAR::fReferCharDelay );

        GLChar* pTarget = m_pCharMan->GetChaByName(pNetMsg->m_szTargetName);
		if (pTarget)
		{
			// 상대 정보 보기는 유저 GM도 가능하도록 변경 (비즈니스팀 요청사항)
			if( pTarget->IsReferCharPublic() || pChar->m_dwUserLvl >= USER_USER_GM )
			{
				ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
				NetMsg.SetTargetName( pNetMsg->m_szTargetName );
				NetMsg.m_eResult = ReferChar::EReferSuccess;
				SENDTOCLIENT( pChar->ClientSlot(), &NetMsg );

				pTarget->SendMsgReferChar( true, pChar->ClientSlot(), 0 );
			}
			else
			{
				ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
				NetMsg.SetTargetName( pNetMsg->m_szTargetName );
				NetMsg.m_eResult = ReferChar::EReferPrivate;
				SENDTOCLIENT( pChar->ClientSlot(), &NetMsg );
			}
		}
		else
		{
			if( !m_bEmulator )
			{
				ReferChar::SNET_MSG_REFER_CHAR_REQ NetMsg;
				NetMsg.SetTargetName( pNetMsg->m_szTargetName );
				NetMsg.m_nRequestCharDbNum = pChar->CharDbNum();
				SENDTOAGENT( &NetMsg );
			}
			else
			{
				ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
				NetMsg.SetTargetName( pNetMsg->m_szTargetName );
				NetMsg.m_eResult = ReferChar::EReferLogout;
				SENDTOCLIENT( pChar->ClientSlot(), &NetMsg );
			}
		}
	}
	else
	{
		GLChar* pTarget = m_pCharMan->GetChaByName(pNetMsg->m_szTargetName);
		if (pTarget)
		{
			if (pTarget->IsReferCharPublic() || pNetMsg->m_bForce)
			{
				ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
				NetMsg.SetTargetName( pNetMsg->m_szTargetName );
				NetMsg.m_nRequestCharDbNum = pNetMsg->m_nRequestCharDbNum;
				NetMsg.m_eResult = ReferChar::EReferSuccess;
				SENDTOAGENT( &NetMsg );

				pTarget->SendMsgReferChar( false, 0, pNetMsg->m_nRequestCharDbNum );
			}
			else
			{
				ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
				NetMsg.SetTargetName( pNetMsg->m_szTargetName );
				NetMsg.m_nRequestCharDbNum = pNetMsg->m_nRequestCharDbNum;
				NetMsg.m_eResult = ReferChar::EReferPrivate;
				SENDTOAGENT( &NetMsg );
			}
		}
		else
		{
			ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
			NetMsg.SetTargetName( pNetMsg->m_szTargetName );
			NetMsg.m_nRequestCharDbNum = pNetMsg->m_nRequestCharDbNum;
			NetMsg.m_eResult = ReferChar::EReferLogout;
			SENDTOAGENT( &NetMsg );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgChaSaveDF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
// 	GLMSG::SNETPC_MSG_CHA_SAVE_DF *msg = ( GLMSG::SNETPC_MSG_CHA_SAVE_DF* )nmg;
// 	ResetSaveDBUserID( msg->dwUserNum );
// 
// 	GLMSG::SNETPC_MSG_CHA_SAVE_END_FA NetMsgFA( msg->dwUserNum );
// 	SENDTOAGENT( &NetMsgFA );

	return TRUE;
}

BOOL GLGaeaServer::MsgGMNetMsgDelay( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( NULL == m_pNetMsgMgr )
	{
		return FALSE;
	}

	GLMSG::SNET_GM_MSG_DELAY_MODIFY* msg = ( GLMSG::SNET_GM_MSG_DELAY_MODIFY* )nmg;
	switch ( msg->nMsgType )
	{
	case 1:
		{
			m_pNetMsgMgr->UpdateNetMsg( NET_MSG_GCTRL_STORAGE_SAVE_MONEY, static_cast< DWORD >( msg->fSecond * 1000 ) );
		}
		break;
	case 2:
		{
			m_pNetMsgMgr->UpdateNetMsg( NET_MSG_GCTRL_STORAGE_DRAW_MONEY, static_cast< DWORD >( msg->fSecond * 1000 ) );
		}
		break;
	default:
		{
			m_pNetMsgMgr->InsertNetMsg( msg->nMsgType, static_cast< DWORD >( msg->fSecond * 1000 ) );
		}
		break;
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgGmLoadIPEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwClientID != m_dwAgentSlot )
        return FALSE;

    if (dwGaeaID != GAEAID_NULL )
        return FALSE;

    ran_event::GLIPEvent::Instance()->Load( GLCONST_CHAR::strIPEventFileName, true );

    m_pEventProc->CheckIPEventCharacters();

    return TRUE;
}

BOOL GLGaeaServer::MsgGmSetIPEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwClientID != m_dwAgentSlot )
        return FALSE;

    if (dwGaeaID != GAEAID_NULL )
        return FALSE;

    m_pEventProc->MsgGmSetIPEvent(nmg, dwClientID, dwGaeaID);

    return TRUE;
}

BOOL GLGaeaServer::MsgGmAcquireEXP(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwClientID != m_dwAgentSlot )
        return FALSE;

    if (dwGaeaID != GAEAID_NULL )
        return FALSE;

    GLMSG::SNETPC_GM_ACQUIRE_EXP* pNetMsg = ( GLMSG::SNETPC_GM_ACQUIRE_EXP* )nmg;

    GLChar* pChar = GetChar( pNetMsg->dwID );
    if( !pChar )
        return FALSE;

    if(pNetMsg->bIsToggle)
    {
        pChar->m_bAcquireEXP = !pChar->m_bAcquireEXP;

        GLMSG::SNETPC_GM_ACQUIRE_EXP_ANS pNetAnsMsg;
        pNetAnsMsg.bEnable = pChar->m_bAcquireEXP;

        SENDTOCLIENT( pChar->GETCLIENTID(), &pNetAnsMsg );
    }
    else
    {
        pChar->m_bAcquireEXP = pNetMsg->bEnable;
    }

    return TRUE;
}

BOOL GLGaeaServer::MsgFactionSystem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    const GLMSG::NET_FACTION_MESSAGE* const pNetMsg = (const GLMSG::NET_FACTION_MESSAGE* const)nmg;
    GLChar* pChar = GetChar(dwGaeaID);
    if ( pChar == NULL )
        return FALSE;

	GLLandMan* const pLand(pChar->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

    Faction::ManagerField* const pFactionMan(pLand->GetFactionManager());
    if ( pFactionMan == NULL )
        return FALSE;

    return pFactionMan->messageProcedure(dwClientID, dwGaeaID, nmg);
}

BOOL GLGaeaServer::MsgReqMoveMap ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_REQ_MOVE_MAP* pNetMsg = static_cast< GLMSG::SNETPC_REQ_MOVE_MAP* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( NULL == pChar )
		return FALSE;

	/// 이동정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pNetMsg->dwChaNum;
	sMoveInfo.sMapID = pNetMsg->sMapID;
	sMoveInfo.dwGateID = pNetMsg->dwGata;
	sMoveInfo.vPosition = pNetMsg->vPosition;

	/// 이동한다;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	/*if( false == MoveMap( pChar, pNetMsg->sMapID, pNetMsg->dwGata, pNetMsg->vPosition ) )
		return FALSE;*/

	return TRUE;
}

BOOL GLGaeaServer::MsgTexasHoldem( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( NULL == m_pTexasHoldemMgr )
	{
		return FALSE;
	}

	m_pTexasHoldemMgr->MessageProcess( nmg, dwClientID, dwGaeaID );
	return TRUE;
}


BOOL GLGaeaServer::MsgMsgTournamentAgent( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( GLUseFeatures::GetInstance().IsUsingTournament() )
	{
		m_pMatchingCore->messageProcedureField(nmg, dwClientID, dwGaeaID);
	}
	return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSlotExpand( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_CONSIGNEMENT_SLOT_EXPIRATION* pNetMsg = ( GLMSG::SNETPC_REQ_CONSIGNEMENT_SLOT_EXPIRATION* ) nmg;

    GLChar* pChar = GetChar( dwGaeaID );

    if ( pChar )
    {
        // 아이템 확인
        SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
        if ( !pInvenItem )
            return false;

        const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if ( !pItem || pItem->sBasicOp.emItemType != ITEM_CONSIGMENT_SALE_EXTEND_CARD )
            return false;

		// Note : 레벨 조건 확인
		//
		if ( !pChar->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
		{
			return false;
		}

        if ( pInvenItem->TurnNum() <= 1 )
        {
            if ( pChar->InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 1, true ) == false )
                return false;
        }
        else
        {
            pInvenItem->sItemCustom.wTurnNum--;

            //	[자신에게] Inventory 아이템 소모됨.
            GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
            NetMsg_Inven_Update.dwChaNum = pChar->CharDbNum();
            NetMsg_Inven_Update.wPosX = pInvenItem->wPosX;
            NetMsg_Inven_Update.wPosY = pInvenItem->wPosY;
            NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsg_Inven_Update );
        }

        // 시간을 늘린다.
        CTime tSlotExpand( pChar->GetConsignmentSaleSlotExpansionDate() );
        CTime tCurTime = CTime::GetCurrentTime();
        CTimeSpan tLMT( pItem->sDrugOp.tTIME_LMT );

        if ( tSlotExpand < tCurTime )
            tSlotExpand = tCurTime + tLMT;
        else
            tSlotExpand = tSlotExpand + tLMT;
        
        AddGameAdoJob( db::DbActionPtr( 
            new db::ConsignmentSaleSlotExpansionDateUpdate( pChar->CharDbNum(), tSlotExpand.GetTime()) ));

        pChar->SetConsignmentSaleSlotExpansionDate(tSlotExpand.GetTime());

        
        GLMSG::NET_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK SendData;
        {
            SendData.ExpirationDate = pChar->GetConsignmentSaleSlotExpansionDate();
            SendData.bItemUsed = TRUE;
        }
        msgpack::sbuffer PackBuffer;
        msgpack::pack( PackBuffer, SendData );
        SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK, PackBuffer );

        return TRUE;
    }

    return FALSE;

}

BOOL GLGaeaServer::MsgLand(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLChar* pGM(GetChar(dwGaeaID));
	if ( !pGM || pGM->UserLevel() < USER_GM3 )
		return FALSE;

	const GLMSG::SNET_GM_LAND* const pLandMessage((const GLMSG::SNET_GM_LAND* const)nmg);
	const DWORD mainMapID(pLandMessage->nParameter0);
	const DWORD subMapID(pLandMessage->nParameter1);
	SNATIVEID mapID(mainMapID, subMapID);

	
	GLLandManager* pLandManager(GetLandManager(mapID));	
	if ( pLandManager == NULL )
	{
		GLLandMan* const pLand(pGM->GetLandMan());
		if ( pLand == NULL )
			mapID = pGM->GetCurrentMap().getGaeaMapID();
		else
			mapID = pLand->GetGaeaMapID();
		pLandManager = GetLandManager(mapID);
		if ( pLandManager == NULL )
			return FALSE;
	}	

	const DWORD nStage(pLandManager->getNStage());
	const DWORD nUsedMemory(pLandManager->getUsedMemory());

	GLMSG::SNET_GM_LAND_INFORMATION _landInformationMessage;
	_landInformationMessage.mapID = mapID;
	_landInformationMessage.nUsedMemory = nUsedMemory;	
	for ( DWORD _i(MAX_LAND_SID); _i--; )
	{
		GLLandMan* pLand(pLandManager->getLand(_i));
		if ( pLand == NULL )
			continue;

		const SNATIVEID& baseMapID(pLand->GetBaseMapID());
		const SNATIVEID& instanceMapID(pLand->GetGaeaMapID());
		_landInformationMessage.addInformationLand(baseMapID, instanceMapID);
	}	

	SENDTOCLIENT(dwClientID, &_landInformationMessage);
    return TRUE;
}

BOOL GLGaeaServer::MsgPing(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( m_pPingManager == NULL )
		return TRUE;

	return m_pPingManager->_messageProcedure(dwClientID, dwGaeaID, nmg) ? TRUE : FALSE;
}

BOOL GLGaeaServer::RequestConfrontTarget(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

	if ( m_dwAgentSlot != dwClientID )
	{
		return FALSE;
	}

	GLMSG::SNETPC_REQ_CONFRONT_TAR* pPacket =
		static_cast< GLMSG::SNETPC_REQ_CONFRONT_TAR* >( nmg );

	GLChar* pChar = GetChar( pPacket->dwID );
	if ( !pChar )
		return FALSE;

	GLChar* pTarChar = GetChar( pPacket->dwTargetID );
	if ( !pTarChar )
		return FALSE;

	pPacket->emVehicleState = pChar->VehicleState();
	SENDTOCLIENT( pTarChar->GETCLIENTID(), pPacket );
    return TRUE;
}

BOOL GLGaeaServer::MsgEvent(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE; // client 측으로의 메세지는 올 수 없다;

	if ( m_pEventManager == NULL )
		return FALSE;

	return m_pEventManager->messageProcedure(dwClientID, dwGaeaID, nmg) == true ? TRUE : FALSE;
}

BOOL GLGaeaServer::MsgCountRecorderInfoReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( !m_bEmulator )
	{
		if(dwClientID != m_dwAgentSlot)
		{
			sc::writeLogError( sc::string::format("MsgCountRecorderInfoReq Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
			return FALSE;
		}
	}

	GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ * pNetMsg = (GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ *)nmg;
	const SNATIVEID sMapID(pNetMsg->sMapID);
	GLLandMan* pLandMan = GetLand ( sMapID );
	// 다른 필드의 캐릭터가 보낸거 일 수 있다;
	// GLChar* pGM(GetChar(pNetMsg->dwGaeaID));
	if ( !pLandMan )//|| !pGM )
		return FALSE;

	GLCountRecorder* const pCountRecorder = pLandMan->GetCountRecorder();
	if ( !pCountRecorder )
		return FALSE;

	GLMSG::SNET_GM_COUNTRECORDER_INFO_FB	msgFB;
	msgFB.dwReqFlag = pNetMsg->dwFlag;
	msgFB.dwGaeaID = pNetMsg->dwGaeaID;
	switch ( pNetMsg->dwFlag )
	{
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPOUTCROW_CNT:
		{
			const GLCountRecorder::MAP_COUNTER& mapDropOutCrow= pCountRecorder->GetDropOutCrowMap();
			GLCountRecorder::MAP_COUNTER_CITR itr = mapDropOutCrow.begin()
				, itrEnd = mapDropOutCrow.end();
			for ( ; itr != itrEnd; ++itr )
			{
				msgFB.sTargetNID = itr->first;
				msgFB.lCount = itr->second;
				SENDTOAGENT(&msgFB);//SENDTOCLIENT(pGM->GETCLIENTID(), &msgFB);
			}	
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPITEM_CNT:
		{
			const GLCountRecorder::MAP_COUNTER& mapItemCount = pCountRecorder->GetDropItemMap();
			GLCountRecorder::MAP_COUNTER_CITR itr = mapItemCount.begin()
				, itrEnd = mapItemCount.end();
			for ( ; itr != itrEnd; ++itr )
			{
				msgFB.sTargetNID = itr->first;
				msgFB.lCount = itr->second;
				SENDTOAGENT(&msgFB);//SENDTOCLIENT(pGM->GETCLIENTID(), &msgFB);
			}	
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_MONEY_CNT:
		{
			const LONGLONG& lDropMoneyCnt = pCountRecorder->GetDropMoneyCount();
			msgFB.lCount = lDropMoneyCnt;
			SENDTOAGENT(&msgFB);//SENDTOCLIENT(pGM->GETCLIENTID(), &msgFB);
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ENTERPLAYER_CNT:
		{
			const LONGLONG& lEnterPlayerCnt = pCountRecorder->GetEnterPlayerCount();
			msgFB.lCount = lEnterPlayerCnt;
			SENDTOAGENT(&msgFB);//SENDTOCLIENT(pGM->GETCLIENTID(), &msgFB);
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ELAPSEDTIME:
		{
			const LONGLONG lElapsedTime(static_cast<LONGLONG>(pCountRecorder->GetElapsedTime()));
			msgFB.lCount = lElapsedTime;
			SENDTOAGENT(&msgFB);//SENDTOCLIENT(pGM->GETCLIENTID(), &msgFB);
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_RESET:
		{
			pCountRecorder->Clear();
			SENDTOAGENT(&msgFB);
		}
		break;
	}
	
	return TRUE;
}

BOOL GLGaeaServer::MsgGmSetNotifyMoneyAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwClientID != m_dwAgentSlot )
	{
		return FALSE;
	}

	GLMSG::NET_GM_NOTIFY_MONEY_CAF* pNetMsg = ( GLMSG::NET_GM_NOTIFY_MONEY_CAF *) nmg;
	if ( pNetMsg->money < 0 )
	{
		return FALSE;
	}

	GLCONST_CHAR::NotifyMoney = pNetMsg->money;
	return TRUE;
}

BOOL GLGaeaServer::MsgChaSaveAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
// 	GLMSG::SNETPC_MSG_CHA_SAVE_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_SAVE_FA *) nmg;
// 	m_pCharMan->ReqSaveDbUserAdd( pNetMsg->dwUserNum );
	return TRUE;
}

BOOL GLGaeaServer::MsgChaSaveEndAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
// 	GLMSG::SNETPC_MSG_CHA_SAVE_END_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_SAVE_END_FA *) nmg;
// 	m_pCharMan->ReqSaveDbUserDel( pNetMsg->dwUserNum );
	return TRUE;
}

BOOL GLGaeaServer::MsgMapSearchNameCA( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//GLLandMan* const pLand( GetLandMan() );
	//if ( pLand == NULL )
	//	return E_FAIL;

	GLMSG::NET_MAP_SEARCH_NAME_REQ_CA* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_REQ_CA*)nmg;

	//GLChar* pChar = GetChar( dwGaeaID );

	//if( pChar == NULL )
	//	return FALSE;


	GLChar* pTarChar = GetCharByDbNum( GetChaDbNumByChaName( pNetMsg->m_szChaName) );
	if ( !pTarChar )
	{		
		// 사용자가 잘못된 아이디를 입력할 수 있으므로 일반적인 오류이므로 별도로 로그를 남기지 않음
		return FALSE;
	}

	if( pTarChar->IsReferCharPublic() == true || pNetMsg->m_bGM == TRUE )
	{
		SENDTOAGENT(pNetMsg);
	}

	return TRUE;
}


BOOL GLGaeaServer::MsgMapSearchNameCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//GLLandMan* const pLand( GetLandMan() );
	//if ( pLand == NULL )
	//	return E_FAIL;

	GLMSG::NET_MAP_SEARCH_NAME_REQ_CF* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_REQ_CF*)nmg;

	GLChar* pChar = GetChar( dwGaeaID );

	if( pChar == NULL )
		return FALSE;

	//GLChar* pTarChar = GetCharByDbNum( GetChaDbNumByChaName( pNetMsg->m_szChaName) );
	//if ( !pTarChar )
	//{		
	//	// 사용자가 잘못된 아이디를 입력할 수 있으므로 일반적인 오류이므로 별도로 로그를 남기지 않음
	//	return FALSE;
	//}

	//if( pTarChar->IsReferCharPublic() == true || pChar->UserLevel() >= USER_USER_GM )
	//if( pChar->UserLevel() >= USER_USER_GM )
	//{
		pNetMsg->m_bGM = ( pChar->UserLevel() >= USER_USER_GM ) ? TRUE : FALSE;
		pNetMsg->m_dwGaeaID = dwGaeaID;
		pNetMsg->m_SenderChaDBNum = pChar->GETCHARDATA().CharDbNum();

		SENDTOAGENT(pNetMsg);
	//}

	return TRUE;
}


BOOL GLGaeaServer::MsgTransDB2WorldBattle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( dwClientID != m_dwAgentSlot )
	{
		return FALSE;
	}

	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE *pNetMsg = (GLMSG::SNETPC_TRANS_TO_WORLDBATTLE *) nmg;
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetCharByDbNum ( pNetMsg->dwChaDBNum );
	if ( NULL == pChar )
	{
		sc::writeLogError( "[ Battle Server ] [ A character is NULL. ]" );

		return FALSE;
	}

	GLLandMan* pLand = pChar->GetLandMan();
	if ( NULL == pLand )
	{
		sc::writeLogError( sc::string::format(
			"[ Battle Server ] [ A Land is NULL. < UserDBNum : %1%, ChaDBNum : %2% > ]",
			pChar->GetUserID(),
			pChar->CharDbNum() ) );

		return FALSE;
	}

	if ( pChar->IsPrisonMap() ||  pChar->IsCTFMap() || 
		pLand->IsNewInstantMap() ||
		pLand->IsOldInstantMap() )
	{
		sc::writeLogInfo( sc::string::format(
			"[ Battle Server ] [ A character is invalid position. < UserDBNum : %1%, ChaDBNum : %2% > ]",
			pChar->GetUserID(),
			pChar->CharDbNum() ) );

		GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_ANS NetMsgAns;
		NetMsgAns.InvalidPosition();
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgAns );

		return TRUE;
	}

	if ( pChar->UserLevel() < USER_GM3 )
	{
		// Use Features 확인;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
		{
			sc::writeLogError ( "[ WorldBattle Log ] [ Current Server is not the World Battle Server." );
			return TRUE;
		}

		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
		{
			sc::writeLogError ( "[ WorldBattle Log ] [ Current Server is not the Join World Battle Server." );
			return TRUE;
		}

		// 캐릭터 아이템 유효성 확인;
		if ( false == pChar->ConfirmItemValidation() )
		{
			sc::writeLogInfo( sc::string::format(
				"[ Battle Server ] [ A character has invalid items. < UserDBNum : %1%, ChaDBNum : %2% > ]",
				pChar->GetUserID(),
				pChar->CharDbNum() ) );

			GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_ANS NetMsgAns;
			NetMsgAns.InvalidCharacterItem();
			SENDTOCLIENT( pChar->ClientSlot(), &NetMsgAns );

			return TRUE;
		}
	}

	//  현재 정보를 DB에 저장한다
	PreSaveCharDB( pChar->CharDbNum() );
	SaveCharDB( pChar->GetGaeaID(), true );
	// WordlBattle 서버로 이전은 DB 저장이 완료된후 수행해야함. DB에 저장된 최신 데이터를 이용해서 데이터 복사를 수행하므로
    //pChar->MsgTransDB2WorldBattle( nmg );
    return TRUE;    
}

// 해당 메세지는 유저가 아니라 캐릭터 정보 저장후 내부메세지로 발생되는 것임
BOOL GLGaeaServer::MsgTransDB2WorldBattleCharSave(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE *pNetMsg = (GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE *) nmg;

	if( pNetMsg->dwGaeaID == GAEAID_NULL )
		return FALSE;

    GLChar* pChar = GetChar ( pNetMsg->dwGaeaID );
    if ( !pChar )
        return FALSE;

	pChar->MsgTransDB2WorldBattle( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgTransDB2WorldBattleAns(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB *msg = (GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB *)nmg;

    GLChar* pChar = GetCharByDbNum(msg->dwChaDBNum);
    if ( !pChar )
        return FALSE;
    pChar->MsgTransDB2WorldBattleAns( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgGmNetworkProfileReq(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_NETWORKPROFILE_REQ* pPacket = (GLMSG::SNET_NETWORKPROFILE_REQ*) nmg;

	if( pPacket->m_nProfileType == 1 )
	{
        // PROFILE 사용.
		g_bProfileEx = TRUE;
		sc::writeLogInfo(std::string("PROFILE ON"));
	}
	else if( pPacket->m_nProfileType == 0 )
	{
        // PROFILE 사용안함.
		g_bProfileEx = FALSE;
		sc::writeLogInfo(std::string("PROFILE OFF"));
	}
	//else
	//{
	//	int nTemp = 0;
	//	while(true)
	//	{
	//		nTemp = 0;
	//	}
	//}


	return TRUE;    
}

BOOL GLGaeaServer::MsgReqExpCompressor( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLMSG::SNETPC_EXP_COMPRESSOR_ANS MsgFB;
    GLChar* pChar = GetChar( dwGaeaID );

    if ( pChar )
    {
		int nExPercent = 0;
		int nCapsuleType = 0;
		SNATIVEID sCapsuleItem;

		// 압축 아이템 삭제 및 경험치 감소
		nCapsuleType = pChar->MsgReqExpCompressor( nmg, nExPercent, sCapsuleItem );
		if( nCapsuleType == 0 )
			return FALSE;

		SITEMCUSTOM sExpCapsule;

		const SNATIVEID sItemID( sCapsuleItem.Mid(), sCapsuleItem.Sid() );

		if ( !NewItemMake( sExpCapsule, sItemID, EMGEN_SYSTEM ) )
		{
			// 아이템 생성 불가0
			MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_FAIL;
			SENDTOCLIENT( dwClientID, &MsgFB );
			return FALSE;
		}

		sExpCapsule.GenerateBasicStat( false );
		sExpCapsule.GenerateLinkSkill();
		sExpCapsule.GenerateAddOption();

		//// 캡슐 아이템 경험치 설정 ( ex: 1억 * nExPercent )
		sExpCapsule.SetDurability( nExPercent );

		if ( !pChar->InvenInsertItem( sExpCapsule, true, 1, true, false, true ) )
		{
			// 인벤토리에 넣기 실패
			MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_FAIL;
			SENDTOCLIENT( dwClientID, &MsgFB );
			return FALSE;
		}
			
		MsgFB.emFB = EMINVEN_EXP_COMPRESSOR_OK;
		SENDTOCLIENT( dwClientID, &MsgFB );    

		return TRUE;
	}
	else
	{
		return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqExpCapsule(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (dwGaeaID == GAEAID_NULL)
		return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (pChar)
    {
		pChar->MsgReqExpCapsule(nmg);
		return TRUE;
	}
	else
	{
		return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqMacroModeOnoffCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLChar* pChar = GetChar(dwGaeaID);
	if (dwGaeaID == GAEAID_NULL || pChar == NULL )
		return FALSE;

	if ( GLUseFeatures::GetInstance().IsUsingMacro() == FALSE )
		return FALSE;

	const SMAPNODE* pMapNode(m_pMapList->FindMapNode(pChar->GetCurrentMap().getBaseMapID()));
	if ( pMapNode == NULL || pMapNode->IsMacroUsable() == FALSE )
		return FALSE;

	return pChar->MsgReqMacroModeOnoffCF(nmg);
}

BOOL GLGaeaServer::MsgReqMacroModeNearestCrowInfoCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLChar* pChar = GetChar(dwGaeaID);
	if (dwGaeaID == GAEAID_NULL || pChar == NULL )
		return FALSE;

	const DWORD dwTargetGaeaID(pChar->GetNearestEnemy());
	// land->GetCrow
	const MapID& mapID(pChar->GetCurrentMap());
	const SNATIVEID baseMapID(mapID.getBaseMapID());
	const SNATIVEID gaeaMapID(mapID.getGaeaMapID());
	GLLandMan* const pLand(this->GetLand(gaeaMapID));
	GLCrow* pCrow = pLand->GetCrow(dwTargetGaeaID);
	
	GLMSG::SNET_MACROMODE_NEARESTCROWINFO_FC msg;
	msg.bFind = false;
	if ( pCrow )
	{
		msg.bFind = true;
		msg.vPosition = pCrow->GetPosition();
	}
	SENDTOCLIENT(pChar->ClientSlot(), &msg);
	return TRUE;
}

BOOL GLGaeaServer::MsgGMReqUserCountLandCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pMsg )
{
	GLChar* pChar = GetChar( _dwGaeaID );
	if ( NULL == pChar )
		return FALSE;

	GLMSG::SNETPC_REQ_USERCOUNT_LAND_CF* pRecvMsg =
		static_cast< GLMSG::SNETPC_REQ_USERCOUNT_LAND_CF* >( _pMsg );
	if ( NULL == pRecvMsg )
		return FALSE;

	// Instance Map ID 를 이용해 Land 를 불러온다;
	DWORD dwInstanceMapID = static_cast< DWORD >( pRecvMsg->nInstanceMapID );
	GLLandManager* pLandMan = GetLandManager( dwInstanceMapID );
	if ( NULL == pLandMan )
	{
		GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, 0 );
		sResponseMsg.emFlag = EMUSERCOUNT_LAND_FAIL;
		SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );

		return FALSE;
	}

	Faction::EMFACTION_TYPE emFactionType = pRecvMsg->emType;
	switch ( emFactionType )
	{
	case Faction::EMFACTION_PARTY:
		{
			GLCountRecorder::MAP_COUNTER mapCounter;
			pLandMan->GetCurPartyPlayerCount( mapCounter );

			if ( mapCounter.empty() )
			{
				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, 0 );
				sResponseMsg.emFlag = EMUSERCOUNT_LAND_NOT_EXIST;
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );

				break;
			}

			GLCountRecorder::MAP_COUNTER_ITR iter = mapCounter.begin();
			GLCountRecorder::MAP_COUNTER_ITR eiter = mapCounter.end();
			for ( ; iter != eiter; ++iter )
			{
				DWORD dwID = iter->first;
				LONGLONG llCount = iter->second;

				if ( 0 == llCount )
					continue;

				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( dwID, llCount );
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );
			}
		}
		break;

	case Faction::EMFACTION_CLUB:
		{
			GLCountRecorder::MAP_COUNTER mapCounter;
			pLandMan->GetCurClubPlayerCount( mapCounter );

			if ( mapCounter.empty() )
			{
				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, 0 );
				sResponseMsg.emFlag = EMUSERCOUNT_LAND_NOT_EXIST;
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );

				break;
			}

			GLCountRecorder::MAP_COUNTER_ITR iter = mapCounter.begin();
			GLCountRecorder::MAP_COUNTER_ITR eiter = mapCounter.end();
			for ( ; iter != eiter; ++iter )
			{
				DWORD dwID = iter->first;
				LONGLONG llCount = iter->second;

				if ( 0 == llCount )
					continue;

				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( dwID, llCount );
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );
			}
		}
		break;

	case Faction::EMFACTION_COUNTRY:
		{
			GLCountRecorder::MAP_COUNTER mapCounter;
			pLandMan->GetCurCountryPlayerCount( mapCounter );

			if ( mapCounter.empty() )
			{
				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, 0 );
				sResponseMsg.emFlag = EMUSERCOUNT_LAND_NOT_EXIST;
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );

				break;
			}

			GLCountRecorder::MAP_COUNTER_ITR iter = mapCounter.begin();
			GLCountRecorder::MAP_COUNTER_ITR eiter = mapCounter.end();
			for ( ; iter != eiter; ++iter )
			{
				DWORD dwID = iter->first;
				LONGLONG llCount = iter->second;

				if ( 0 == llCount )
					continue;

				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( dwID, llCount );
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );
			}
		}
		break;

	case Faction::EMFACTION_SCHOOL:
		{
			GLCountRecorder::MAP_COUNTER mapCounter;
			pLandMan->GetCurSchoolPlayerCount( mapCounter );

			if ( mapCounter.empty() )
			{
				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, 0 );
				sResponseMsg.emFlag = EMUSERCOUNT_LAND_NOT_EXIST;
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );

				break;
			}

			GLCountRecorder::MAP_COUNTER_ITR iter = mapCounter.begin();
			GLCountRecorder::MAP_COUNTER_ITR eiter = mapCounter.end();
			for ( ; iter != eiter; ++iter )
			{
				DWORD dwID = iter->first;
				LONGLONG llCount = iter->second;

				if ( 0 == llCount )
					continue;

				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( dwID, llCount );
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );
			}
		}
		break;

	case Faction::EMFACTION_TEAM:
		{
			GLCountRecorder::MAP_FACTION_COUNTER mapCounter;
			pLandMan->GetCurFactionPlayerCount( mapCounter );

			if ( mapCounter.empty() )
			{
				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, 0 );
				sResponseMsg.emFlag = EMUSERCOUNT_LAND_NOT_EXIST;
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );

				break;
			}

			GLCountRecorder::MAP_FACTION_COUNTER_ITR iter = mapCounter.begin();
			GLCountRecorder::MAP_FACTION_COUNTER_ITR eiter = mapCounter.end();
			for ( ; iter != eiter; ++iter )
			{
				Faction::FactionID sFactionID = iter->first;
				LONGLONG llCount = iter->second;

				if ( 0 == llCount )
					continue;

				GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( sFactionID.factionID_Num, llCount );
				sResponseMsg.dwType = static_cast< DWORD >( sFactionID.factionType );
				SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );
			}
		}
		break;

	case Faction::EMFACTION_NONE:
		{
			LONGLONG llCount = pLandMan->GetCurPlayerCount();

			GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC sResponseMsg( 0, llCount );
			SENDTOCLIENT( pChar->ClientSlot(), &sResponseMsg );
		}
		break;
	}

	return TRUE;
}

// 란모바일
BOOL GLGaeaServer::MsgGmRanMobileCommandRequest(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
	{
		sc::writeLogError(std::string("RAN MOBILE option is closed.(MsgGmRanMobileCommandRequest)"));
		return FALSE;
	}

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST* pNetMsg = ( GLMSG::SNET_RANMOBILE_COMMAND_REQUEST *) nmg;

	GLChar* pChar = GetChar ( dwGaeaID );
	if( !pChar )
		return FALSE;

	CFieldServer *pFieldServer = static_cast<CFieldServer*>(m_pMsgServer);
	if ( !pFieldServer )
		return FALSE;

	std::string strName = pChar->GetName();

	switch( pNetMsg->nCommand )
	{
	case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_GM :
		{
			pNetMsg->nValue = NSRanMobile::GLRanMobile::GetInstance().GetFirstTime();
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_MONEY:
		{
			if( pChar->GetInvenMoney() < NSRanMobile::GLRanMobile::GetInstance().GetRecognizePrice() )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_MONEY;
				sErrorMsg.nResult = EResult_CreateChar_NoMoney;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			//sc::writeLogError(std::string("[RAN MOBILE] Create First Connection"));

			pNetMsg->nValue = NSRanMobile::GLRanMobile::GetInstance().GetFirstTime();
			strName = pNetMsg->szName;
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_CACHE:
		{
			SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( ITEM_RANMOBILE_CONNECTION );
			if( NULL == pInvenItem )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_CACHE;
				sErrorMsg.nResult = EResult_CreateChar_NoItem;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			pNetMsg->nValue = NSRanMobile::GLRanMobile::GetInstance().GetFirstTime();
			strName = pNetMsg->szName;

			//sc::writeLogError(std::string("[RAN MOBILE] Create Connection"));
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_EXTEND_TIME:
		{
			if( pChar->GetInvenMoney() < NSRanMobile::GLRanMobile::GetInstance().GetExtendPrice() )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_EXTEND_TIME;
				sErrorMsg.nResult = EResult_Extend_NoMoney;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			//sc::writeLogError(std::string("[RAN MOBILE] Extend Time"));

			pNetMsg->nValue = NSRanMobile::GLRanMobile::GetInstance().GetExtendTime();
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_DAILY_ITEM:
		{
			// 란 모바일 연동 여부 확인
			if( false == pChar->GetRanMobileRecognized() )
				return TRUE;

			// 일일 보상 조건 체크
			// 0일 경우는 일일 보상 한번도 받은 적 없는 상태다. 체크할 필요 없다.
			if( 0 != pChar->GetRanMobileRewardDate() )
			{
				CTime cRewardTime( pChar->GetRanMobileRewardDate() );
				CTime cCurTime = CTime::GetCurrentTime();

				if( cRewardTime.GetYear() == cCurTime.GetYear() )
				{
					if( cRewardTime.GetMonth() == cCurTime.GetMonth() )
					{
						if( cRewardTime.GetDay() >= cCurTime.GetDay() )
						{
							GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
							sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_GET_DAILY_ITEM;
							sErrorMsg.nResult = EResult_DailyReward_Already;
							SENDTOCLIENT( dwClientID, &sErrorMsg );

							return TRUE;
						}
					}
				}
			}

			//sc::writeLogError(std::string("[RAN MOBILE] Get Daily Item"));
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL:
		{
			// 란 모바일 연동 여부 확인
			if( false == pChar->GetRanMobileRecognized() )
				return TRUE;

			// 보석 개수 체크
			if( pNetMsg->nValue > pChar->m_cInventory.CountTurnItem( ITEM_RANMOBILE_JEWEL ) )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL;
				sErrorMsg.nResult = EResult_SendJewel_Count;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( ITEM_RANMOBILE_JEWEL );
			if( NULL == pInvenItem )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL;
				sErrorMsg.nResult = EResult_SendJewel_NoItem;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			//sc::writeLogError(std::string("[RAN MOBILE] Send Jewel"));
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_POINT:
		{
			//sc::writeLogError(std::string("[RAN MOBILE] Get MPoint"));
		}
		break;

	case EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM:
		{
			// 란 모바일 연동 여부 확인
			if( false == pChar->GetRanMobileRecognized() )
				return TRUE;

			const NSRanMobile::VEC_REWARD& vecReward = NSRanMobile::GLRanMobile::GetInstance().GetReward();
			if( vecReward.size() <= pNetMsg->nValue )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM;
				sErrorMsg.nResult = EResult_Change_NoData;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			const NSRanMobile::SRewardItem& sReward = vecReward[ pNetMsg->nValue ];

			if( sReward.dwNecessaryMPoint > pChar->GetRanMobilePoint() )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM;
				sErrorMsg.nResult = EResult_Change_MPoint;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			// 인벤토리 여유 공간 체크
			const SITEM* pItem = GLogicData::GetInstance().GetItem( sReward.sID );
			if( NULL == pItem )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM;
				sErrorMsg.nResult = EResult_Change_NoData;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			bool bItemSpace = false;
			if( true == pItem->ISPILE() )
			{
				//	겹침 아이템일 경우.
				WORD wPILENUM = pItem->sDrugOp.wPileNum;
				SNATIVEID sNID = pItem->sBasicOp.sNativeID;

				//	넣기 요청된 아이템수. ( 잔여량. )
				WORD wREQINSRTNUM = ( sReward.wCount * pItem->GETAPPLYNUM() );

				bItemSpace = pChar->m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );
			}
			else
			{
				for( WORD i = 0; i < sReward.wCount; ++i )
				{
					//	일반 아이템의 경우.
					WORD wInsertPosX( 0 ), wInsertPosY( 0 );
					bItemSpace = pChar->m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

					//	인벤에 여유공간이 없을 경우 취소됨.
					if( false == bItemSpace )
						break;
				}
			}

			if( false == bItemSpace )
			{
				GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE sErrorMsg;
				sErrorMsg.nCommand = EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM;
				sErrorMsg.nResult = EResult_Change_FullInventory;
				SENDTOCLIENT( dwClientID, &sErrorMsg );

				return TRUE;
			}

			int nMPoint = pChar->GetRanMobilePoint();
			if( sReward.dwNecessaryMPoint < nMPoint )
			{
				nMPoint -= sReward.dwNecessaryMPoint;
				pChar->SetRanMobilePoint( nMPoint );

				RanMobileBuyItem( pChar, sReward.sID, sReward.wCount, sReward.fExpCompressRatio );
			}

			//sc::writeLogError(std::string("[RAN MOBILE] Exchange MPoint Item"));

			pNetMsg->nValue = static_cast< int >( sReward.dwNecessaryMPoint ) * -1;
		}
		break;

	default:
		//sc::writeLogError(sc::string::format("[RAN MOBILE] Command ERROR : %1%", pNetMsg->nCommand));
		//return FALSE;
		break;
	}

	//	DB에 저장.
	AddGameAdoJob( db::DbActionPtr( 
		new db::RanMobileRequest(
		pChar->GetUserID(),
		pChar->m_CharDbNum,
		pFieldServer->GetServerGroup(),
		pNetMsg->nCommand,
		pNetMsg->nValue,
		strName,
		(int)pChar->m_emClass ) ) );

	return TRUE;
}


// 란모바일
BOOL GLGaeaServer::MsgGmRanMobileCommandResponse(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
	{
		sc::writeLogError(std::string("RAN MOBILE option is closed.(MsgGmRanMobileCommandRequest)"));
		return TRUE;
	}

	GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE* pNetMsg = (GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE*) nmg;

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaDBNum );
	if( pChar == NULL )
		return FALSE;

	if( ERanMobileResult_OK == pNetMsg->nResult )
	{
		switch( pNetMsg->nCommand )
		{
		case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO :
		case EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_GM :
			{
				// 란 모바일 연동 여부 설정
				pChar->SetRanMobileRecognized( true );
				// 일일 보상 받은 날짜 저장
				pChar->SetRanMobileRewardDate( pNetMsg->sMobileInfo.tRewardDate );
				// 모바일 포인트 저장
				pChar->SetRanMobilePoint( pNetMsg->sMobileInfo.nRMPoint );
			}
			break;

		case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_MONEY:
			{
				pChar->RanMobileCharacterConnectionFee( NSRanMobile::GLRanMobile::GetInstance().GetRecognizePrice(), false );
			}
			break;

		case EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_CACHE:
			{
				SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( ITEM_RANMOBILE_CONNECTION );
				if( NULL == pInvenItem )
					break;

				pChar->RanMobileDeleteInvenItem( 2, pInvenItem->GetNativeID(), 1 );
			}
			break;

		case EM_RAN_MOBILE::EM_RAN_MOBILE_EXTEND_TIME:
			{
				pChar->RanMobileCharacterConnectionFee( NSRanMobile::GLRanMobile::GetInstance().GetExtendPrice(), true );
			}
			break;

		case EM_RAN_MOBILE::EM_RAN_MOBILE_GET_DAILY_ITEM:
			{
				// 보상 시간 갱신
				pChar->SetRanMobileRewardDate( pNetMsg->sMobileInfo.tRewardDate );

				// 보상 아이템은 우편으로 쏴준다.
				PostSendSystemMail(
					pChar->CharDbNum(),
					std::string( pChar->GetName() ),
					NSRanMobile::GLRanMobile::GetInstance().GetDailyItemID().dwID,
					0,
					ATTENDANCE_REWARD_POST_DESIGN,
					ATTENDANCE_REWARD_POST_SENDER,
					std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ) ),
					std::string( ID2SERVERTEXT( "TEXT_RANMOBILE_DAILYREWARD_TITLE", 0 ) ),
					std::string( ID2SERVERTEXT( "TEXT_RANMOBILE_DAILYREWARD_CONTENTS", 0 ) ) );
			}
			break;

		case EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL:
			{
				SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( ITEM_RANMOBILE_JEWEL );
				if( NULL == pInvenItem )
					break;

				pChar->RanMobileDeleteInvenItem( 1,  pInvenItem->GetNativeID(), pNetMsg->nValue );
			}
			break;
		}
	}

	pNetMsg->ClearUserInfo();
	SENDTOCLIENT( pChar->ClientSlot(), pNetMsg );
	
	return TRUE;
}

BOOL GLGaeaServer::RanMobileBuyItem( GLChar* pChar, const SNATIVEID& ItemID, DWORD ItemNum, float fExpRaio )
{
	if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
	{
		sc::writeLogError(std::string("RAN MOBILE option is closed.(RanMobileBuyItem)"));
		return TRUE;
	}

	if ( !pChar )
	{
		sc::writeLogError( std::string( "Can not find GetCharByDbNum on RanMobileBuyItem" ) );
		return FALSE;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( ItemID );
	if ( !pItem )
		return FALSE;

	WORD wNums = ItemNum ? ( WORD )ItemNum : 1;

	// 인벤에 여유 공간이 있는지 검사
	BOOL bITEM_SPACE( FALSE );
	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( wNums * pItem->GETAPPLYNUM() );

		bITEM_SPACE = pChar->m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM );

		//	인벤에 여유공간이 없을 경우 취소됨.
		if ( !bITEM_SPACE )
			return FALSE;

		//	아이템의 소유 이전 경로 기록.
		SITEMCUSTOM sDB_ITEM( sNID );
		sDB_ITEM.cGenType  = EMGEN_CHEAT;
		sDB_ITEM.wTurnNum  = wPILENUM;
		sDB_ITEM.tBORNTIME = CTime::GetCurrentTime().GetTime();

		if( 0.0f < fExpRaio )
			sDB_ITEM.SetDurability( fExpRaio );

		// 통계 및 로그 수정
		pChar->LogItemExchange( 
			sDB_ITEM, 
			gamelog::ID_CHAR, 0, 
			gamelog::ID_CHAR, pChar->m_CharDbNum, 
			ITEM_ROUTE_CHAR, sDB_ITEM.wTurnNum, 
			( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_RANMOBILE_BUY, 
			true );

		// 아이템을 인벤에 넣어준다.
		std::vector< UPDATED_ITEM > vecInsertedItems;
		pChar->InsertPileItem( sDB_ITEM, wREQINSRTNUM, vecInsertedItems, true );
	}
	else
	{
		for( WORD i = 0; i < wNums; ++i )
		{
			//	일반 아이템의 경우.
			WORD wInsertPosX( 0 ), wInsertPosY( 0 );
			bITEM_SPACE = pChar->m_cInventory.FindInsertable( wInsertPosX, wInsertPosY );

			//	인벤에 여유공간이 없을 경우 취소됨.
			if ( !bITEM_SPACE )
				return FALSE;

			// 새로운 아이템을 생성합니다
			SNATIVEID sNID = ItemID;

			SITEMCUSTOM sITEM_NEW( sNID );
			CTime cTIME = CTime::GetCurrentTime();
			sITEM_NEW.tBORNTIME = cTIME.GetTime();
			sITEM_NEW.wTurnNum = 1;
			sITEM_NEW.cGenType = EMGEN_CHEAT;
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			if( 0.0f < fExpRaio )
				sITEM_NEW.SetDurability( fExpRaio );

			// 인벤토리에 넣어줍니다
			if ( !pChar->InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, GM_MAKE_ITEM, true, false, true ) )
				return FALSE;

			// 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
			pChar->LogItemExchange(
				sITEM_NEW,
				gamelog::ID_CHAR, 0,
				gamelog::ID_CHAR,  pChar->m_CharDbNum,
				ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
				( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_RANMOBILE_BUY, 
				true );
		}
	}

    return TRUE;
}

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
BOOL GLGaeaServer::MsgGmRandomboxNotifyItemReloadReqAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if(dwClientID != m_dwAgentSlot)
    {
        sc::writeLogError( sc::string::format("MsgGmRandomboxNotifyItemReloadReqAF Wrong Try [IP:%s]", GetClientIP(dwClientID) ) );
    }

    GLMSG::SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF* pPacket = (GLMSG::SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF*) nmg;

    if ( !RandomChanceLoadScript() )
    {
        sc::writeLogWarn(std::string("--- Random chance lua script reload fail!!! ---"));
    }

    return TRUE;
}


BOOL GLGaeaServer::MsgReqLockCharSlotCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_REQUEST_LOCK_CHARSLOT_CF* pNetMsg =
		static_cast< GLMSG::SNETPC_REQUEST_LOCK_CHARSLOT_CF* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( NULL == pChar )
		return FALSE;

	pChar->LockCharSlot( pNetMsg->wCharSlotNum );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqUnlockCharSlotCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_CF* pNetMsg =
		static_cast< GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_CF* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( NULL == pChar )
		return FALSE;

	pChar->UnlockCharSlot( pNetMsg->wCharSlotNum );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqChangeCharSlotCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_REQUEST_CHANGE_CHARSLOT_CF* pNetMsg =
		static_cast< GLMSG::SNETPC_REQUEST_CHANGE_CHARSLOT_CF* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( NULL == pChar )
		return FALSE;

	pChar->ChangeCharSlot( pNetMsg->wCharSlotNum );

	return TRUE;
}