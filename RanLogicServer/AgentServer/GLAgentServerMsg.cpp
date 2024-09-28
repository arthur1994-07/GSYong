#include "../pch.h"
#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Item/GLITEMLMT.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/ReferCharMsg.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/GLContrlClubMsg.h"
#include "../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Event/GLIPEvent.h"

#include "../Club/GLClubServer.h"
#include "../Club/GLClubMan.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Util/GLItemLimit.h"
#include "../Database/DbAction/DbActionGame.h"
#include "../Database/DbAction/DbActionGameClub.h"
#include "../Database/DbAction/DbActionLog.h"
#include "../Guidance/GuidanceAgent.h"
#include "../Party/GLPartyAgent.h"
#include "../Party/GLPartyManagerAgent.h"
#include "../PK/GLFreePK.h"
#include "./GLAgentServer.h"
#include "./StateFieldServer/StateFieldServer.h"

#include "../CaptureTheField/CaptureTheFieldManager.h"

#include "../../RanLogic/Prison/LimitPrison.h"

#include "../Move/MoveManagerServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLAgentServer::initPacketFunc()
{
	m_MsgFunc[NET_FREEPK_GM_COMMAND_CA].Msgfunc					= &GLAgentServer::MsgFreePkGmCommand;
	m_MsgFunc[NET_MSG_CLUB_MASTER_MANDATE_CA].Msgfunc			= &GLAgentServer::MsgClubMasterMandate;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GATEOUT].Msgfunc				= &GLAgentServer::MsgReqGateOut;	
	m_MsgFunc[NET_MSG_GCTRL_REQ_GENITEM_AGT].Msgfunc			= &GLAgentServer::MsgReqGenItemAgt;
	m_MsgFunc[NET_MSG_FIELDSVR_CHARCHK_FB].Msgfunc				= &GLAgentServer::MsgReqFieldSvrCharChkFb;
	m_MsgFunc[NET_MSG_SERVER_CLOSE_FIELD].Msgfunc				= &GLAgentServer::MsgServerCloseField;
	m_MsgFunc[NET_MSG_SERVER_CLOSE_AGENT].Msgfunc				= &GLAgentServer::MsgServerCloseAgent;
	//m_MsgFunc[NET_MSG_SERVER_GENERALCHAT].Msgfunc				= &GLAgentServer::MsgServerGeneralChat;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT].Msgfunc	= &GLAgentServer::MsgConfrontPtyCheckMber;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT].Msgfunc	= &GLAgentServer::MsgConfrontClbCheckMber;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL].Msgfunc			= &GLAgentServer::MsgClubMemberDb2Del;
	m_MsgFunc[NET_MSG_CLUB_CERTIFIED_FA].Msgfunc			    = &GLAgentServer::MsgClubCertified;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT].Msgfunc		= &GLAgentServer::MsgClubIncomeMoney;
	//m_MsgFunc[NET_MSG_GCTRL_CLUB_INCOME_UP].Msgfunc			= &GLAgentServer::MsgClubIncomeMoneyUp;
	m_MsgFunc[NET_MSG_GCTRL_2_FRIEND_AG].Msgfunc				= &GLAgentServer::Msg2FriendAG;
	m_MsgFunc[NET_MSG_GM_MOVE2CHAR_AG].Msgfunc					= &GLAgentServer::MsgMove2CharAG;
	m_MsgFunc[NET_MSG_GM_CHAR_INFO_FLD_FB].Msgfunc				= &GLAgentServer::MsgGmCharInfoFldFB;
	m_MsgFunc[NET_MSG_USER_CHAR_INFO_FLD_FB].Msgfunc			= &GLAgentServer::MsgUserCharInfoFldFB;
	m_MsgFunc[NET_MSG_CLUB_CD_CERTIFY_ING_BRD].Msgfunc	        = &GLAgentServer::MsgClubCDCertifyIngBrd;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD].Msgfunc		= &GLAgentServer::MsgClubCDCertifyBrd;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT].Msgfunc = &GLAgentServer::MsgClubBattleKillUpdate;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT].Msgfunc = &GLAgentServer::MsgClubBattleLastKillUpdate;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB].Msgfunc		    = &GLAgentServer::MsgReqTaxiNpcPos;
	m_MsgFunc[NET_MSG_GCTRL_INSTANCE_SYSTEM].Msgfunc			= &GLAgentServer::MessageInstanceSystem;
    m_MsgFunc[NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK].Msgfunc	= &GLAgentServer::MessageInstanceSystemMsgPack;
    m_MsgFunc[NET_MSG_GCTRL_INSTANCE_CONTENTS].Msgfunc			= &GLAgentServer::MessageInstanceSystemContents;
    m_MsgFunc[NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK].Msgfunc	= &GLAgentServer::MessageInstanceSystemContentsMsgPack;

	m_MsgFunc[NET_MSG_GCTRL_INSTANCE_SET_RETURNMAP_INFORMATION_FA].Msgfunc	= &GLAgentServer::MessageInstanceSystemProc;

	m_MsgFunc[NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ].Msgfunc = &GLAgentServer::MsgReqCreateInstantMap;	
	m_MsgFunc[NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL].Msgfunc = &GLAgentServer::MsgReqDeleteInstantMap;
	m_MsgFunc[NET_MSG_GCTRL_NPC_COMMISSION].Msgfunc			= &GLAgentServer::MsgNpcCommission;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_END_AGT].Msgfunc= &GLAgentServer::MsgClubDeathMatchResult;
	m_MsgFunc[NET_MSG_POST_NOTIFY].Msgfunc					= &GLAgentServer::MsgPostNotify;
	m_MsgFunc[NET_MSG_POST_OPEN_TAKE_AF_FB].Msgfunc			= &GLAgentServer::MsgPostOpenAndTakeFA;
    /*
     * redmine  : #2462 특별활동 “서버 전체” 알림 기능 안되는 문제 확인 및 수정
     * modified : sckim, 2016.01.07, 사용하지 되지 않는 메세지 기능으로 판단하여 제거
     */
	//m_MsgFunc[NET_MSG_ACTIVITY_COMPLETED_FA].Msgfunc		= &GLAgentServer::MsgActivityCompletedFA;

	m_MsgFunc[NET_MSG_CTF_JOIN_FA].Msgfunc				= &GLAgentServer::MsgCTFJoinFA;
	//m_MsgFunc[NET_MSG_CTF_SYNC_REQ_CA].Msgfunc		= &GLAgentServer::MsgCTFSyncReqCA;
	m_MsgFunc[NET_MSG_CTF_QUIT_CFA].Msgfunc				= &GLAgentServer::MsgCTFQuitCA;
	m_MsgFunc[NET_MSG_CTF_MOVESTATE_BY_GM_CA].Msgfunc	= &GLAgentServer::MsgCTFMoveStateByGMCA;

	m_MsgFunc[NET_MSG_GCTRL_ACTSTATE].Msgfunc				= &GLAgentServer::MsgActState;
	m_MsgFunc[NET_MSG_REQ_FRIENDLIST].Msgfunc				= &GLAgentServer::MsgReqFriendList;
	m_MsgFunc[NET_MSG_REQ_FRIENDADD].Msgfunc				= &GLAgentServer::MsgReqFriendAdd;
	m_MsgFunc[NET_MSG_REQ_FRIENDADD_ANS].Msgfunc			= &GLAgentServer::MsgReqFriendAddAns;
	m_MsgFunc[NET_MSG_REQ_FRIENDDEL].Msgfunc				= &GLAgentServer::MsgReqFriendDel;
	m_MsgFunc[NET_MSG_REQ_FRIENDBLOCK].Msgfunc				= &GLAgentServer::MsgReqFriendBlock;


	
	m_MsgFunc[NET_MSG_CLUB_MEMBER_REQ_CA].Msgfunc	        = &GLAgentServer::MsgClubMemberReq;
	m_MsgFunc[NET_MSG_CLUB_MEMBER_REQ_ANS_CA].Msgfunc	    = &GLAgentServer::MsgClubMemberAns;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT].Msgfunc	= &GLAgentServer::MsgClubMemberReqAgt;
	m_MsgFunc[NET_MSG_CLUB_MEMBER_DEL_CA].Msgfunc           = &GLAgentServer::MsgClubMemberDel;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_SECEDE].Msgfunc		= &GLAgentServer::MsgClubMemberSecede;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_REQ].Msgfunc		= &GLAgentServer::MsgClubAuthorityReq;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS].Msgfunc	= &GLAgentServer::MsgClubAuthorityReqAns;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MARK_INFO].Msgfunc			= &GLAgentServer::MsgClubMarkInfo;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_MARK_CHANGE].Msgfunc		= &GLAgentServer::MsgClubMarkChange;
	m_MsgFunc[NET_MSG_CLUB_RANK_CHANGE_FA].Msgfunc			= &GLAgentServer::MsgClubRank2Agt;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_COMMISSION].Msgfunc		= &GLAgentServer::MsgClubGuidCommission;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_NOTICE_REQ].Msgfunc		= &GLAgentServer::MsgClubNoticeReq;
	//m_MsgFunc[NET_MSG_GCTRL_CLUB_SUBMASTER].Msgfunc		= &GLAgentServer::MsgClubSubMaster;
	m_MsgFunc[NET_MSG_CLUB_ALLIANCE_REQ_CA].Msgfunc		    = &GLAgentServer::MsgClubAllianceReq;
	m_MsgFunc[NET_MSG_CLUB_ALLIANCE_REQ_ANS_CA].Msgfunc	    = &GLAgentServer::MsgClubAllianceReqAns;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ].Msgfunc	= &GLAgentServer::MsgClubAllianceDelReq;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ].Msgfunc	= &GLAgentServer::MsgClubAllianceSecReq;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ].Msgfunc	= &GLAgentServer::MsgClubAllianceDisReq;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_REQ].Msgfunc		= &GLAgentServer::MsgClubBattleReq;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS].Msgfunc			= &GLAgentServer::MsgClubBattleReqAns;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ].Msgfunc		= &GLAgentServer::MsgClubBattleArmisticeReq;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS].Msgfunc	= &GLAgentServer::MsgClubBattleArmisticeReqAns;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ].Msgfunc		= &GLAgentServer::MsgClubBattleSubmissionReq;
	m_MsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ].Msgfunc				= &GLAgentServer::MsgAllianceBattleReq;
	m_MsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS].Msgfunc			= &GLAgentServer::MsgAllianceBattleReqAns;
	m_MsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ].Msgfunc		= &GLAgentServer::MsgAllianceBattleArmisticeReq;
	m_MsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS].Msgfunc	= &GLAgentServer::MsgAllianceBattleArmisticeReqAns;
	m_MsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ].Msgfunc		= &GLAgentServer::MsgAllianceBattleSubmissionReq;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_RENAME_AGTBRD].Msgfunc	= &GLAgentServer::MsgRenameBrd;
	//m_MsgFunc[NET_MSG_SMS_PHONE_NUMBER_AGTBRD].Msgfunc	= &GLAgentServer::MsgPhoneNumberBrd;
	m_MsgFunc[NET_MSG_GCTRL_UPDATE_STARTCALL].Msgfunc		= &GLAgentServer::MsgUpdateStartMap;	
	m_MsgFunc[NET_MSG_GCTRL_PUTON_CHANGE_AG].Msgfunc		= &GLAgentServer::MsgChangeArm;
	//m_MsgFunc[NET_MSG_REQ_FRIEND_CLUB_OPEN].Msgfunc		= &GLAgentServer::MsgFriendClubWindowOpen;
	m_MsgFunc[NET_QBOX_OPTION_REQ_AG].Msgfunc				= &GLAgentServer::MsgReqQboxOption;
	m_MsgFunc[NET_MSG_REQ_ATTENDLIST].Msgfunc				= &GLAgentServer::MsgReqAttendList;
	m_MsgFunc[NET_MSG_REQ_ATTENDANCE].Msgfunc				= &GLAgentServer::MsgReqAttendance;
	m_MsgFunc[NET_MSG_GM_WHISPER_STATE].Msgfunc				= &GLAgentServer::MsgWhisperState;
	m_MsgFunc[NET_MSG_CHAT_LINK_FB].Msgfunc					= &GLAgentServer::ChatMsgLinkProc;
	m_MsgFunc[NET_MSG_CHAT_LOUDSPEAKER_AGT].Msgfunc			= &GLAgentServer::ChatMsgLoudSpeakerAgt;
	m_MsgFunc[NET_MSG_GCTRL_RESURRECT_FAFC].Msgfunc			= &GLAgentServer::MsgReqReBirth;
    m_MsgFunc[NET_MSG_RESURRECT_MOVEMAP_FA].Msgfunc			= &GLAgentServer::MsgReqResurrectMoveMap;
    m_MsgFunc[NET_MSG_RESURRECT_MOVEMAP_FB].Msgfunc			= &GLAgentServer::MsgResurrectMoveMapFB;
	m_MsgFunc[NET_MSG_GCTRL_REGEN_GATE_FB].Msgfunc			= &GLAgentServer::MsgReqReGenGate;
	//m_MsgFunc[NET_MSG_GCTRL_FIELDSVR_OUT_FB].Msgfunc		= &GLAgentServer::MsgFieldSvrOutFb;
	m_MsgFunc[NET_MSG_GCTRL_REQ_MOVEMAP_FA].Msgfunc			= &GLAgentServer::MsgReCallAg;
	m_MsgFunc[NET_MSG_REQ_MUST_LEAVE_MAP_AG].Msgfunc		= &GLAgentServer::MsgLvUpMustLeaveMap;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TELEPORT_AG].Msgfunc		= &GLAgentServer::MsgTeleportAg;
	m_MsgFunc[NET_MSG_GCTRL_REQ_SERVER_CLUB_GUID_BATTLE_REMAIN_AG].Msgfunc		= &GLAgentServer::MsgClub_GuidBattle_RemainTime;
	m_MsgFunc[NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG].Msgfunc	= &GLAgentServer::MsgClubDeathMatchRemainTime;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_ON_OFF_CA].Msgfunc	= &GLAgentServer::MsgGmClubDeathMatchStartEndCA;
	    
    m_MsgFunc[NET_MSG_GCTRL_PARTY_LURE_WITH_CHARNUM].Msgfunc = &GLAgentServer::MsgPartyLureWithCharNum;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_LURE].Msgfunc					= &GLAgentServer::MsgPartyLure;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_LURE_TAR_ANS].Msgfunc	= &GLAgentServer::MsgPartyLureTarAns;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_SECEDE].Msgfunc				= &GLAgentServer::MsgPartySecede;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_AUTHORITY].Msgfunc			= &GLAgentServer::MsgPartyAuthority;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_DISSOLVE].Msgfunc			= &GLAgentServer::MsgPartyDissolve;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_LINK_JOIN].Msgfunc			= &GLAgentServer::MsgPartyLinkJoin;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_LINK_LURE].Msgfunc			= &GLAgentServer::MsgPartyLinkLure;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_LINK_REFUSE].Msgfunc		= &GLAgentServer::MsgPartyLinkRefuse;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_CHANGE_OPTION].Msgfunc	= &GLAgentServer::MsgPartyChangeOption;	    
	m_MsgFunc[NET_MSG_GCTRL_EXPEDITION_FNEW_CA].Msgfunc	= &GLAgentServer::MsgExpeditionNew;
	m_MsgFunc[NET_MSG_GCTRL_EXPEDITION_CHECK_READY_REQUEST_CA].Msgfunc	= &GLAgentServer::MsgExpeditionCheckReadyRequest;
	m_MsgFunc[NET_MSG_GCTRL_EXPEDITION_CHECK_READY_ANSWER_CA].Msgfunc	= &GLAgentServer::MsgExpeditionCheckReadyAnswer;	
	m_MsgFunc[NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT].Msgfunc		= &GLAgentServer::MsgExpeditionSwapSlot;
	m_MsgFunc[NET_MSG_PARTY_INFO_REQ_CA].Msgfunc						= &GLAgentServer::MsgPartyInfoReqCA;
	m_MsgFunc[NET_MSG_GCTRL_PARTY_MBR_UPDATE].Msgfunc			= &GLAgentServer::MsgMapPartyMemberUpdateFA;


	m_MsgFunc[NET_MSG_GCTRL_2_FRIEND_REQ].Msgfunc			= &GLAgentServer::Msg2FriendReq;
	m_MsgFunc[NET_MSG_GM_MOVE2CHAR].Msgfunc					= &GLAgentServer::MsgMove2Char;
	m_MsgFunc[NET_MSG_GCTRL_REQ_GATEOUT_FB].Msgfunc		= &GLAgentServer::MsgReqGateOutFB;	
	m_MsgFunc[NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB].Msgfunc	= &GLAgentServer::MsgReqCreateInstantMapFB;	
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT].Msgfunc				= &GLAgentServer::MsgReqConfront;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT_ANS].Msgfunc			= &GLAgentServer::MsgAnsConfront;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT_FB].Msgfunc			= &GLAgentServer::MsgAnsConfrontFB;
	m_MsgFunc[NET_MSG_GCTRL_CONFRONT_END2_AGT].Msgfunc	= &GLAgentServer::MsgConfrontEnd2Agt;
	m_MsgFunc[NET_MSG_GCTRL_CLUB_DISSOLUTION].Msgfunc	= &GLAgentServer::MsgClubDissolution;
	m_MsgFunc[NET_MSG_CHARPOS_FROMDB2AGT].Msgfunc			= &GLAgentServer::GameJoinToFieldSvr;
	m_MsgFunc[NET_MSG_SERVER_CTRL_WEATHER].Msgfunc		= &GLAgentServer::MsgServerCtrlWeather;
	m_MsgFunc[NET_MSG_SERVER_CTRL_WEATHER2].Msgfunc		= &GLAgentServer::MsgServerCtrlWeather;
	m_MsgFunc[NET_MSG_SERVER_CTRL_GENITEMHOLD].Msgfunc	= &GLAgentServer::MsgServerCtrlGenHold;
	m_MsgFunc[NET_MSG_SERVER_CTRL_TIME].Msgfunc				= &GLAgentServer::MsgServerCtrlTime;
	m_MsgFunc[NET_MSG_SERVER_CTRL_MONTH].Msgfunc			= &GLAgentServer::MsgServerCtrlMonth;
	m_MsgFunc[NET_MSG_BLOCK_DETECTED].Msgfunc					= &GLAgentServer::MsgBlockDetected;

	m_MsgFunc[NET_MSG_GM_KICK_USER].Msgfunc					= &GLAgentServer::MsgGmKickUser;
	m_MsgFunc[NET_MSG_GM_MOVE2GATE].Msgfunc                  = &GLAgentServer::MsgGMMove2Gate;
	m_MsgFunc[NET_MSG_GM_MOVE2MAPPOS].Msgfunc				= &GLAgentServer::MsgGMMove2Pos;	
	m_MsgFunc[NET_MSG_GM_EVENT_ITEM_GEN].Msgfunc			= &GLAgentServer::MsgGmEventItemGen;
	m_MsgFunc[NET_MSG_GM_EVENT_ITEM_GEN_END].Msgfunc	= &GLAgentServer::MsgGmEventItemGenEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_GRADE].Msgfunc				= &GLAgentServer::MsgGmEventGrade;
	m_MsgFunc[NET_MSG_GM_EVENT_GRADE_END].Msgfunc			= &GLAgentServer::MsgGmEventGradeEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_MONEY_GEN].Msgfunc		= &GLAgentServer::MsgGmEventMoneyGen;
	m_MsgFunc[NET_MSG_GM_EVENT_MONEY_GEN_END].Msgfunc	= &GLAgentServer::MsgGmEventMoneyGenEnd;
	m_MsgFunc[NET_MSG_GM_CLASS_EVENT].Msgfunc				= &GLAgentServer::MsgGmEventClassEvent;
	m_MsgFunc[NET_MSG_GM_EVENT_EX].Msgfunc						= &GLAgentServer::MsgGmEventEx;
	m_MsgFunc[NET_MSG_GM_EVENT_EX_END].Msgfunc				= &GLAgentServer::MsgGmEventExEnd;
	m_MsgFunc[NET_MSG_GM_EVENT_EXP_CAF].Msgfunc				= &GLAgentServer::MsgGmEventExp;
	m_MsgFunc[NET_MSG_GM_EVENT_EXP_END_CAF].Msgfunc		= &GLAgentServer::MsgGmEventExpEnd;
	m_MsgFunc[NET_MSG_GM_VIEWWORKEVENT].Msgfunc			= &GLAgentServer::MsgGmViewWorkEvent;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_BEGIN].Msgfunc		= &GLAgentServer::MsgGmLimitEventBegin;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_END].Msgfunc			= &GLAgentServer::MsgGmLimitEventEnd;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_RESTART].Msgfunc	= &GLAgentServer::MsgGmLimitEventRestart;
	m_MsgFunc[NET_MSG_GM_LIMIT_EVENT_TIME_REQ].Msgfunc	= &GLAgentServer::MsgGmLimitEventTimeReq;
	m_MsgFunc[NET_MSG_CYBERCAFECLASS_UPDATE].Msgfunc		= &GLAgentServer::MsgCyberCafeClassUpdate;
	m_MsgFunc[NET_MSG_MARKET_CHECKTIME].Msgfunc				= &GLAgentServer::MsgMarketOpenCheck;
	
    m_MsgFunc[NET_MSG_GM_BIGHEAD].Msgfunc                    = &GLAgentServer::MsgGmBigHead;
	m_MsgFunc[NET_MSG_GM_BIGHAND].Msgfunc                    = &GLAgentServer::MsgGmBigHand;
	m_MsgFunc[NET_MSG_GM_CHAT_BLOCK_UACCOUNT].Msgfunc        = &GLAgentServer::MsgGmChatBlockUA;
	m_MsgFunc[NET_MSG_GM_CHAT_BLOCK_CHARNAME].Msgfunc        = &GLAgentServer::MsgGmChatBlockCN;
	m_MsgFunc[NET_MSG_GM_CHAT_BLOCK_CHARID].Msgfunc          = &GLAgentServer::MsgGmChatBlockCI;
	m_MsgFunc[NET_MSG_USER_CHAR_INFO_4NAME].Msgfunc          = &GLAgentServer::MsgUserCharInfo4NAME;
	m_MsgFunc[NET_MSG_GM_CHAR_INFO_4NAME].Msgfunc            = &GLAgentServer::MsgGmCharInfo4NAME;
	m_MsgFunc[NET_MSG_GM_CHAR_INFO_4CHARID].Msgfunc          = &GLAgentServer::MsgGmCharInfo4CID;
	m_MsgFunc[NET_MSG_GM_FREEPK].Msgfunc                     = &GLAgentServer::MsgGmFreePK;
	m_MsgFunc[NET_MSG_GM_VIEWALLPLAYER].Msgfunc              = &GLAgentServer::MsgGmViewAllPlayer;
	m_MsgFunc[NET_MSG_GM_GENCHAR].Msgfunc                    = &GLAgentServer::MsgGmGenChar;
	m_MsgFunc[NET_MSG_GM_MOB_GEN].Msgfunc                    = &GLAgentServer::MsgGmGenMob;
	m_MsgFunc[NET_MSG_GM_MOB_GEN_EX].Msgfunc                 = &GLAgentServer::MsgGmGenMobEx;
	m_MsgFunc[NET_MSG_GM_MOB_DEL_EX].Msgfunc                 = &GLAgentServer::MsgGmDelMobEx;
	m_MsgFunc[NET_MSG_GM_MOB_DEL].Msgfunc                    = &GLAgentServer::MsgGmDelMob;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL].Msgfunc                  = &GLAgentServer::MsgGmMobLevel;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL_LAYER].Msgfunc            = &GLAgentServer::MsgGmMobLevel;
	m_MsgFunc[NET_MSG_GM_MOB_LEVEL_CLEAR].Msgfunc            = &GLAgentServer::MsgGmMobLevelClear;
	m_MsgFunc[NET_MSG_GM_WHERE_NPC].Msgfunc                  = &GLAgentServer::MsgGmWhereNPC;
	m_MsgFunc[NET_MSG_GM_WHERE_NPC_ALL].Msgfunc              = &GLAgentServer::MsgGmWhereNPCALL;
	m_MsgFunc[NET_MSG_GM_PRINT_CROWLIST].Msgfunc             = &GLAgentServer::MsgGmPrintCrowList;
	m_MsgFunc[NET_MSG_GM_WARNING_MSG].Msgfunc                = &GLAgentServer::MsgGmWarningMSG;
    m_MsgFunc[NET_MSG_GM_SHOWMETHEMONEY].Msgfunc             = &GLAgentServer::MsgGmShowMeTheMoney;
    m_MsgFunc[NET_MSG_GM_WHYSOSERIOUS].Msgfunc               = &GLAgentServer::MsgGmWhySoSerious;
	m_MsgFunc[NET_MSG_GM_WHYSOSERIOUS_FB_FAC].Msgfunc		= &GLAgentServer::MsgGmWhySoSeriousFB;
    m_MsgFunc[NET_MSG_GM_LOG_TO_DB_CAF].Msgfunc              = &GLAgentServer::MsgLogToDB;
	m_MsgFunc[NET_MSG_GM_GETWHISPERMSG].Msgfunc              = &GLAgentServer::MsgGetWhisperMsg;
	m_MsgFunc[NET_MSG_GM_NONCONFRONT_MODE].Msgfunc           = &GLAgentServer::MsgGmNonConfrontMode;
	m_MsgFunc[NET_MSG_GM_NONCONFRONT_MODE_VIEW_REQ].Msgfunc  = &GLAgentServer::MsgGmNonConfrontModeViewReq;
	m_MsgFunc[NET_MSG_GM_CHECKINSTANTNUM].Msgfunc            = &GLAgentServer::MsgGmInstantMapNum;
	m_MsgFunc[NET_MSG_GM_AUTO_LEVEL].Msgfunc					= &GLAgentServer::MsgGmAutoLevel;
	m_MsgFunc[NET_MSG_GM_MAP_LAYER].Msgfunc					= &GLAgentServer::MsgGmMapLayer;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TAXI].Msgfunc                = &GLAgentServer::MsgReqTaxi;
	//m_MsgFunc[NET_MSG_GM_TESTCREATE_INSTANT_MAP_REQ].Msgfunc = &GLAgentServer::MsgReqTestCreateInstantMap;
	m_MsgFunc[NET_MSG_INSTANT_MAP_REENTRY_CHECK_REQ].Msgfunc = &GLAgentServer::MsgReqReEntryRemainTime;
    
	m_MsgFunc[NET_MSG_GM_PING].Msgfunc          = &GLAgentServer::ProcessPingMsg;
	m_MsgFunc[NET_MSG_GM_PING_FB].Msgfunc       = &GLAgentServer::ProcessPingMsg;
	m_MsgFunc[NET_MSG_GM_PINGTRACE_ON].Msgfunc  = &GLAgentServer::ProcessPingTraceMsg;
	m_MsgFunc[NET_MSG_GM_PINGTRACE_OFF].Msgfunc = &GLAgentServer::ProcessPingTraceMsg;

    // Point Shop -------------------------------------------------------------
    m_MsgFunc[NET_MSG_PS_UPDATE_DATE_CA].Msgfunc = &GLAgentServer::MsgPointShopUpdateDateCA;

    // Rebuy Item -------------------------------------------------------------
    //m_MsgFunc[NET_MSG_ADD_REBUY_ITEM_CA].Msgfunc = &GLAgentServer::MsgAddRebuyItem;
    // Rebuy Item
    //m_MsgFunc[NET_MSG_ADD_REBUY_ITEM_FA].Msgfunc = &GLAgentServer::MsgAddRebuyItem;
    m_MsgFunc[NET_MSG_BUY_REBUY_ITEM_CA].Msgfunc = &GLAgentServer::MsgBuyRebuyItem;
    m_MsgFunc[NET_MSG_BUY_REBUY_ITEM_FA].Msgfunc = &GLAgentServer::MsgBuyRebuyItemFB;
    m_MsgFunc[NET_MSG_CHECK_REBUY_SUPPLIES].Msgfunc = &GLAgentServer::MsgCheckRebuySupplies;

	// ReferChar
	m_MsgFunc[NET_MSG_REFER_CHAR_REQ].Msgfunc = &GLAgentServer::MsgReferCharRequest;
    m_MsgFunc[NET_MSG_REFER_CHAR_FB].Msgfunc = &GLAgentServer::MsgReferCharRelay;
	m_MsgFunc[NET_MSG_REFER_CHAR_PUTON].Msgfunc = &GLAgentServer::MsgReferCharRelay;
	m_MsgFunc[NET_MSG_REFER_CHAR_SKILL].Msgfunc = &GLAgentServer::MsgReferCharRelay;
	m_MsgFunc[NET_MSG_REFER_CHAR_INFO].Msgfunc = &GLAgentServer::MsgReferCharRelay;

	
	//mjeon.instance
	m_MsgFunc[NET_MSG_MATCH_REQUEST_CA].Msgfunc = &GLAgentServer::MsgMatchRequestCA;
	m_MsgFunc[NET_MSG_MATCH_CANCEL_CAM].Msgfunc = &GLAgentServer::MsgMatchCancelCAM;

	
	m_MsgFunc[NET_MSG_TRANSFER_CHAR].Msgfunc = &GLAgentServer::MsgTransferChar;
	m_MsgFunc[NET_MSG_TRANSFER_CHAR_DONE_FA].Msgfunc = &GLAgentServer::MsgTransferCharDoneFA;

	m_MsgFunc[NET_MSG_GCHAT_INVITE_CAC].Msgfunc = &GLAgentServer::MsgChatInviteCAC;
	m_MsgFunc[NET_MSG_GCHAT_INVITE_CAC_FB].Msgfunc = &GLAgentServer::MsgChatInviteCAC_FB;
	m_MsgFunc[NET_MSG_GCHAT_QUIT_CA].Msgfunc = &GLAgentServer::MsgChatQuitCA;
	m_MsgFunc[NET_MSG_GCHAT_KICK_CA].Msgfunc = &GLAgentServer::MsgChatKickCA;
	m_MsgFunc[NET_MSG_GCHAT_MESSAGE].Msgfunc = &GLAgentServer::MsgChatMessageCAC;
	

	//m_MsgFunc[NET_MSG_MATCH_TEST_MAC].Msgfunc = &GLAgentServer::MsgMatchTestMAC;
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_REQ_CF].Msgfunc = &GLAgentServer::MsgMapSearchNameCF;
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_REQ_CA].Msgfunc = &GLAgentServer::MsgMapSearchName;

    m_MsgFunc[NET_MSG_CHAR_LEVEL_UP_FA].Msgfunc = &GLAgentServer::MsgLevelUpFA;

    // World battle -----------------------------------------------------------
    m_MsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_CA].Msgfunc = &GLAgentServer::MsgClubCountryCountCA;
    m_MsgFunc[NET_MSG_CLUB_COUNTRY_CA].Msgfunc       = &GLAgentServer::MsgClubCountryCA;

    m_MsgFunc[NET_MSG_CLUB_INCOME_RENEW_CA].Msgfunc = &GLAgentServer::MsgClubIncomeReNewCA;
    m_MsgFunc[NET_MSG_CLUB_GETSTORAGE_CA].Msgfunc = &GLAgentServer::MsgClubGetStorageCA;
    m_MsgFunc[NET_MSG_CLUB_STORAGE_TO_INVEN_CA].Msgfunc = &GLAgentServer::MsgClubStorageToInvenCA;
    
    //m_MsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_SPLIT].Msgfunc = &GLAgentServer::MsgClubStorageSplit;
    
    m_MsgFunc[NET_MSG_CLUB_STORAGE_DRAW_MONEY_CA].Msgfunc = &GLAgentServer::MsgClubStorageDrawMoneyCA;
    m_MsgFunc[NET_MSG_REQ_DROP_CHAR_CLUB_NAME_CA].Msgfunc = &GLAgentServer::MsgReqDropCharClubNameCA;

	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA].Msgfunc = &GLAgentServer::MsgMapSearchNameDetailInfoCA;
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA].Msgfunc = &GLAgentServer::MsgMapSearchNameDetailInfoFA;

	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_POSITION_REQ_CA].Msgfunc = &GLAgentServer::MsgMapSearchNamePositionCA;
	m_MsgFunc[NET_MSG_MAP_SEARCH_NAME_POSITION_ACK_FA].Msgfunc = &GLAgentServer::MsgMapSearchNamePositionFA;


	//////////////////////////////////////////////////////////////////////////
	m_MsgFunc[NET_MSG_MAP_CREATE_PARTY_BOARD_REQ_CA].Msgfunc = &GLAgentServer::MsgMapCreatePartyInfoCA;
	m_MsgFunc[NET_MSG_MAP_DELETE_PARTY_BOARD_REQ_CA].Msgfunc = &GLAgentServer::MsgMapDeletePartyInfoCA;
	m_MsgFunc[NET_MSG_MAP_REFRESH_PARTY_BOARD_REQ_CA].Msgfunc = &GLAgentServer::MsgMapRefreshPartyInfoCA;
	m_MsgFunc[NET_MSG_MAP_PARTY_INFO_DETAIL_REQ_CA].Msgfunc = &GLAgentServer::MsgMapPartyInfoDetailCA;

    // Friend -----------------------------------------------------------------
    m_MsgFunc[NET_MSG_FRIEND_GROUP_NEW_CA].Msgfunc = &GLAgentServer::MsgFriendGroupNewCA;
    m_MsgFunc[NET_MSG_FRIEND_GROUP_DEL_CA].Msgfunc = &GLAgentServer::MsgFriendGroupDelCA;
    m_MsgFunc[NET_MSG_FRIEND_SET_GROUP_CA].Msgfunc = &GLAgentServer::MsgFriendSetGroupCA;
    m_MsgFunc[NET_MSG_FRIEND_GROUP_NAME_CHANGE_CA].Msgfunc = &GLAgentServer::MsgFriendGroupNameChangeCA;
    m_MsgFunc[NET_MSG_FRIEND_GROUP_REQ_CA].Msgfunc = &GLAgentServer::MsgFriendGroupReqCA;

    // Notify -----------------------------------------------------------------
    m_MsgFunc[NET_MSG_NOTIFY_REQ_CA].Msgfunc = &GLAgentServer::MsgNotifyReqCA;

    // Club -------------------------------------------------------------------
    m_MsgFunc[NET_MSG_CLUB_GRADE_FLAG_CHANGE_CA].Msgfunc = &GLAgentServer::MsgClubGradeFlagChangeCA;
    m_MsgFunc[NET_MSG_CLUB_PULIC_FLAG_CHANGE_CA].Msgfunc = &GLAgentServer::MsgClubPublicFlagChangeCA;
    m_MsgFunc[NET_MSG_CLUB_AUTO_KICK_CA].Msgfunc         = &GLAgentServer::MsgClubAutoKickCA;
    m_MsgFunc[NET_MSG_CLUB_NEWBIE_NOTICE_CA].Msgfunc     = &GLAgentServer::MsgClubNewbieNoticeCA;    

    m_MsgFunc[NET_MSG_CLUB_ITEM_EXCHANGE_TO_INVEN_CA].Msgfunc = &GLAgentServer::MsgClubStorageExInvenCA;
    m_MsgFunc[NET_MSG_CLUB_STORAGE_EX_CLUB_STORAGE_CA].Msgfunc = &GLAgentServer::MsgClubStorageExClubStorageCA;
    m_MsgFunc[NET_MSG_CLUB_STORAGE_TO_CLUB_STORAGE_CA].Msgfunc = &GLAgentServer::MsgClubStorageToClubStorageCA;

    // ------------------------------------------------------------------------
    m_MsgFunc[NET_MSG_CHAR_DB_NUM_REQ_CA].Msgfunc = &GLAgentServer::MsgChaDbNumReqCA;
    m_MsgFunc[NET_MSG_FRIEND_STATE_REQ_CA].Msgfunc = &GLAgentServer::MsgFriendStateReqCA;

    m_MsgFunc[NET_MSG_GUIDANCE_MAP_INFO_CA].Msgfunc = &GLAgentServer::MsgGuidanceMapInfoCA;
    m_MsgFunc[NET_MSG_GM_GUIDANCE_INFO_CA].Msgfunc = &GLAgentServer::MsgGmGuidanceInfoCA;
    m_MsgFunc[NET_MSG_GM_GUIDANCE_CHANGE_CA].Msgfunc = &GLAgentServer::MsgGmGuidanceChangeCA;
	m_MsgFunc[NET_MSG_GM_GUIDANCE_ON_OFF_CA].Msgfunc = &GLAgentServer::MsgGmGuidanceBattleStartEndCA;

    m_MsgFunc[NET_MSG_CLUB_JOIN_TIME_RESET_CA].Msgfunc     = &GLAgentServer::MsgClubJoinTimeResetCA;
    m_MsgFunc[NET_MSG_CLUB_ALLIANCE_TIME_RESET_CA].Msgfunc = &GLAgentServer::MsgClubAllianceTimeResetCA;
	m_MsgFunc[NET_MSG_CLUB_ID_2MASTERNAME_REQ_CA].Msgfunc = &GLAgentServer::MsgClubId2MasterNameReqCA;

	// 개인상점 -------------------------------------------------------------------
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_OPTION_CA].Msgfunc = &GLAgentServer::MsgPrivateMarketOptionCA;

    // Product (제조) ------------------------------------------------------------------------
    //! 클라이언트 요청
    m_MsgFunc[NET_MSG_PRODUCT_REQ].Msgfunc                  = &GLAgentServer::MsgProductReqCA;
    m_MsgFunc[NET_MSG_REQ_LEARN_PRODUCTBOOK].Msgfunc        = &GLAgentServer::MsgProductLearnBookCA;
    m_MsgFunc[NET_MSG_REQ_PRODUCTBOOK].Msgfunc              = &GLAgentServer::MsgProductBookListCA;
    m_MsgFunc[NET_MSG_PRODUCT_TYPE_LIST_REQ].Msgfunc        = &GLAgentServer::MsgProductTypeListCA;
    m_MsgFunc[NET_MSG_PRODUCT_TYPE_ADD_REQ].Msgfunc         = &GLAgentServer::MsgProductTypeAddCA;
    m_MsgFunc[NET_MSG_PRODUCT_TYPE_DEL_REQ].Msgfunc         = &GLAgentServer::MsgProductTypeDelCA;
    m_MsgFunc[NET_MSG_PRODUCT_ISGETEXP_LIST_REQ].Msgfunc    = &GLAgentServer::MsgProductIsGetExpListCA;
    //! DB 응답
    m_MsgFunc[NET_MSG_PRODUCT_TYPE_LIST_ACK].Msgfunc        = &GLAgentServer::MsgProductTypeListDA;
    m_MsgFunc[NET_MSG_PRODUCTBOOK].Msgfunc                  = &GLAgentServer::MsgProductBookListDA;
    m_MsgFunc[NET_MSG_PRODUCT_ISGETEXP_LIST_ACK].Msgfunc    = &GLAgentServer::MsgProductIsGetExpListDA;

	//! Agent 요청;
	m_MsgFunc[NET_MSG_PRODUCT_EXCHANGE_LOG_FA].Msgfunc		= &GLAgentServer::MsgProductExchangeLogFA;

	m_MsgFunc[NET_MSG_GM_MSG_DELAY_MODIFY].Msgfunc = &GLAgentServer::MsgGmNetMsgDelayCA;

    // PVE
    m_MsgFunc[NET_MSG_PVE_MAPTABLE_REQ_CA].Msgfunc = &GLAgentServer::MsgPVEReqMapTableCA;
    m_MsgFunc[NET_MSG_PVE_ENTRANCE_REQ_CA].Msgfunc = &GLAgentServer::MsgPVEReqEntranceCA;
    m_MsgFunc[NET_MSG_PVE_ENTRANCE_CONFIRM_REQ].Msgfunc = &GLAgentServer::MsgPVEReqEntranceConfirmFA;
    m_MsgFunc[NET_MSG_PVE_CLEAROFBELONGING_REQ_CA].Msgfunc = &GLAgentServer::MsgPVEReqClearOfBelongingCA;
	m_MsgFunc[NET_MSG_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF].Msgfunc = &GLAgentServer::MsgPVEReqReloadTriggerCA;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FA].Msgfunc = &GLAgentServer::MsgPVEReqEnterNumCharge;

    // IP event.
    m_MsgFunc[NET_MSG_GM_SET_IPEVENT].Msgfunc = &GLAgentServer::MsgGmSetIPEvent;

    m_MsgFunc[NET_MSG_GM_ACQUIRE_EXP].Msgfunc = &GLAgentServer::MsgGmAcquireEXP;
	m_MsgFunc[NET_MSG_GCTRL_MATCHING_SYSTEM].Msgfunc = &GLAgentServer::MsgMsgTournamentAgent;
    m_MsgFunc[NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM].Msgfunc = &GLAgentServer::MsgMsgPackTournamentAgent;


	m_MsgFunc[NET_MSG_FIELD_STATE].Msgfunc = &GLAgentServer::MsgFieldServerStateFA;

	// Lotto System ---------------------------------------------------------------------------
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_AGENT_MYSELF].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;

	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_FA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;

	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_SET_INVENT_WIN_NUM_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_RESET_INVENT_WIN_NUM_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;

	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;
	m_MsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_CA].Msgfunc	= &GLAgentServer::LottoSystemMsgProc;

	m_MsgFunc[NET_MSG_GM_FLYCAMERACONTROL_CA].Msgfunc = &GLAgentServer::MsgGMFlyCameraControl;

	m_MsgFunc[NET_MSG_POST_GOT_DATA_CHANGE].Msgfunc	= &GLAgentServer::MsgPostGetDataChangeDB;
	m_MsgFunc[NET_MSG_POST_GOT_STATE_CHANGE].Msgfunc	= &GLAgentServer::MsgPostGetStateChangeDB;
	m_MsgFunc[NET_MSG_POST_GOT_SEND_STATE_CHANGE].Msgfunc	= &GLAgentServer::MsgPostGetSendStateChangeDB;
	m_MsgFunc[NET_MSG_POST_GOT_RECV_STATE_CHANGE].Msgfunc	= &GLAgentServer::MsgPostGetRecvStateChangeDB;

	m_MsgFunc[NET_MSG_GCTRL_EVENT].Msgfunc = &GLAgentServer::MsgEvent;
	// ----------------------------------------------------------------------------------------
	m_MsgFunc[NET_MSG_COUNTRECORDER_INFO_REQ].Msgfunc = &GLAgentServer::MsgCountRecorderInfoReq;
	m_MsgFunc[NET_MSG_COUNTRECORDER_INFO_FB].Msgfunc = &GLAgentServer::MsgCountRecorderInfoFB_FA;

	// User System Info
	//m_MsgFunc[NET_MSG_USER_SYSTEM_INFO_CA].Msgfunc			= &GLAgentServer::MsgUserSystemInfoCA;

	m_MsgFunc[NET_MSG_CHAR_MONEY_NOTIFY_FA].Msgfunc	= &GLAgentServer::MsgCharMoneyNotifyFA;
	m_MsgFunc[NET_MSG_GM_NOTIFY_MONEY_CAF].Msgfunc	= &GLAgentServer::MsgGmSetNotifyMoneyCA;

	/************************************************************************/
	/* Country System                                                       */
	/************************************************************************/
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_ADD_FAC].Msgfunc	= &GLAgentServer::MsgVCountryAddFAC;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_ADD_DAF].Msgfunc	= &GLAgentServer::MsgVCountryAddDA;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_ADD_AA].Msgfunc	= &GLAgentServer::MsgVCountryAddAA;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_RESET_FAC].Msgfunc	= &GLAgentServer::MsgVCountryResetFAC;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_RESET_DAF].Msgfunc	= &GLAgentServer::MsgVCountryResetDA;
	m_MsgFunc[NET_MSG_GCTRL_VCOUNTRY_RESET_AA].Msgfunc	= &GLAgentServer::MsgVCountryResetAA;

	m_MsgFunc[NET_MSG_GM_NETWORKPROFILE_REQ].Msgfunc = &GLAgentServer::MsgGmNetworkProfileReq;

	m_MsgFunc[NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE].Msgfunc = &GLAgentServer::MsgTransDB2WorldBattle;	
	m_MsgFunc[NET_MSG_GM_CHANGE_WBJOIN_STATE].Msgfunc = &GLAgentServer::MsgGmChangeJoinWorldBattle;

	m_MsgFunc[NET_MSG_GCTRL_PRISON_AG].Msgfunc			= &GLAgentServer::MsgPrisonAg;

	m_MsgFunc[NET_MSG_GCTRL_JACKPOT_NOTIFY].Msgfunc = &GLAgentServer::MsgJackpotNotify;
	m_MsgFunc[NET_MSG_GCTRL_JACKPOT_OPTION].Msgfunc = &GLAgentServer::MsgJackpotOption;

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14, 신규등록
     * modified : 
     */
    m_MsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA].Msgfunc = &GLAgentServer::MsgGmSetRandomboxChanceReqCA;

    m_MsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA].Msgfunc = &GLAgentServer::MsgGmGetRandomboxChanceListCA;

    m_MsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA].Msgfunc = &GLAgentServer::MsgGmDelRandomboxChanceCA;

	m_MsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_REQUEST].Msgfunc = &GLAgentServer::MsgGmRanMobileCommandRequest; //란모바일
	m_MsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE].Msgfunc = &GLAgentServer::MsgGmRanMobileCommandResponse; //란모바일

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    m_MsgFunc[NET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF].Msgfunc = &GLAgentServer::MsgGmRandomboxNotifyItemReloadReqCA;

	m_MsgFunc[NET_MSG_GCTRL_CLUB_RENAME_FA].Msgfunc = &GLAgentServer::MsgClubNameChangeFA;
}

BOOL GLAgentServer::ReActionMoveFieldSvr(DWORD dwClientID, DWORD dwGaeaID)
{
	// 클라이언트의 지속 스킬 리셋.
	GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;
	
	NetMsgSkillBrd.dwID		   = dwGaeaID;
	NetMsgSkillBrd.emCrow	   = CROW_PC;
	NetMsgSkillBrd.emResetType = EMSKILLHOLDRESET_MOVEFIELDSRV;
	for (int i=0; i<SKILLFACT_SIZE; ++i)
	{
		NetMsgSkillBrd.bRESET[i] = true;
	}
	SENDTOCLIENT(dwClientID, &NetMsgSkillBrd);

	GLMSG::SNETPC_CURESTATEBLOW_BRD NetMsgBlowBrd;
	NetMsgBlowBrd.dwID = dwGaeaID;
	NetMsgBlowBrd.emCrow = CROW_PC;
	NetMsgBlowBrd.dwCUREFLAG = DIS_ALL;
	SENDTOCLIENT(dwClientID, &NetMsgBlowBrd);

	GLMSG::SNETPC_QITEMFACT_END_BRD NetMsgQFactEndBrd;
	SENDTOCLIENT(dwClientID, &NetMsgQFactEndBrd);

	return TRUE;
}

// *****************************************************
// Desc: 필드서버 정보를 체크후 접속 정보 전송(Agent->Field)
// *****************************************************
BOOL GLAgentServer::MsgReqFieldSvrCharChkFb(NET_MSG_GENERIC* nmg, DWORD _dwClientID, DWORD _dwGaeaID)
{
	GLMSG::SNETPC_FIELDSVR_CHARCHK_FB* pNetMsg = (GLMSG::SNETPC_FIELDSVR_CHARCHK_FB *) nmg;
	GLCharAG* pChar = GetChar(pNetMsg->dwGaeaID);
	if (!pChar)
        return FALSE;

	// 국가세팅;
	CAgentServer* pAgentServer =
		static_cast< CAgentServer* >( m_pMsgServer );
	if ( pAgentServer )
		pChar->SetCountryInfo( pAgentServer->GetCountry( pChar->ChaDbNum() ) );

	// 체크된 횟수 LOG	
	if (pNetMsg->bExist)
    {
        // 캐릭터가 아직 존재한다. DB 작업중일 수 있다.
        //++pChar->m_dwFIELDCK_BEING;
        sc::writeLogInfo(
            sc::string::format(
                "Found same character. UserDbNum %1% ChaDbNum %2%",
                pChar->UserDbNum(),
                pChar->ChaDbNum()));
        pChar->FieldCheckNumReset();
        return TRUE;
    }

    pChar->FieldCheckNumIncrease();
    pChar->m_bFIELD_CK[pNetMsg->nChannel][pNetMsg->dwFIELDID] = true;

	// 필드 체크 완료 여부 판단. 만족 안하면 대기
	if (!pChar->IsFieldCheckComplete(&m_bFIELDSVR[0][0]))
        return TRUE;

	//if (pChar->m_dwFIELDCK_BEING > 0)
	//{
        //	필드서버로 접속이 실패함
		/*
		DEBUGMSG_WRITE( "Found same character in field server. %d removed exist character. ( name %s )",
			pChar->m_dwFIELDCK_BEING, pChar->m_szName );
		*/
	//}

	// 캐릭터 생성위치를 지정
	SNATIVEID sGenMapID = pChar->m_sStartMapID;
	DWORD dwGenGateID = pChar->m_dwStartGate;
	D3DXVECTOR3 vGenPos = pChar->m_vStartPos;

	//	저장된 위치가 있을 경우 설정함.
	if (pChar->m_sSaveMapID != NATIVEID_NULL())
	{
		if (GetFieldServer(sGenMapID) != FIELDSERVER_MAX)
		{
			sGenMapID = pChar->m_sSaveMapID;
			dwGenGateID = UINT_MAX;
			vGenPos = pChar->m_vSavePos;

			// MapList 에 Restart flag 가 설정되어 있으면
			// 강제적으로 시작지점으로 이동하게 된다.
			SMAPNODE* pGenMap = m_sMapList.FindMapNode(sGenMapID);
			if (!pGenMap)
			{
				sGenMapID		= GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
				dwGenGateID	= GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
				vGenPos			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
			else if (pGenMap->IsRestrt())
			{
				sGenMapID		= pChar->m_sStartMapID;
				dwGenGateID	= pChar->m_dwStartGate;
				vGenPos			= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
		}
	}

	GLAGLandMan* pLAND = GetLand(sGenMapID);

	//
	//mjeon.CaptureTheField
	//

	/*
	  이전에 CTF맵에서 종료했을 경우에 대해 처리해주자.
	  
	  1.PvE 맵에서 종료했을 경우
		-현재 CTF가 진행중이면, StartMap에서 시작하도록 한다.
		-현재 CTF가 진행중이 아니면, CTF PvE맵에서 시작하도록 한다.


	  2.PvP 맵에서 종료했을 경우
		-현재 CTF가 진행중이면, StartMap에서 시작하도록 한다.
		-현재 CTF가 진행중이 아니면, CTF PvE맵에서 시작하도록 한다.
	*/
	if (pLAND)
	{
		DWORD dwCTFPvE = m_pCTFMan->GetPvEMapID();
		DWORD dwCTFPvP = m_pCTFMan->GetPvPMapID();
		
		/*if (sGenMapID.dwID == dwCTFPvE)
		{
			if(m_pCTFMan->GetState() != CTF_STATE_STOP)	//CTF 진행중
			{
				sGenMapID	= pChar->m_sStartMapID;
				dwGenGateID = pChar->m_dwStartGate;
			}
			else
			{
				; //nothing to do
			}
		}*/
		if (sGenMapID.dwID == dwCTFPvP)
		{
			if(m_pCTFMan->GetState() != CTF_STATE_STOP)	//CTF 진행중
			{
				sGenMapID	= pChar->m_sStartMapID;
				dwGenGateID	= pChar->m_dwStartGate;
			}
			else
			{
				sGenMapID	= SNATIVEID(dwCTFPvE);
				dwGenGateID = m_pCTFMan->GetStartGate();
			}
		}

    	// 직전에 종료한 맵이 선도전 맵이면 선도클럽이 아닐경우 시작맵으로 지정	
		if (pLAND->IsClubBattleMap())
		{
			GLGuidance* pGUID = m_pGuidance->Find(pLAND->GetGuidance());
			if ( pGUID && pGUID->m_dwGuidanceClub != pChar->m_ClubDbNum )
			{

				GLAGLandMan* pLANDStart = GetLand ( pChar->m_sStartMapID );
				if (!pLANDStart || pLANDStart->IsClubDeathMatchMap())
				{
					sGenMapID = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
					dwGenGateID  = GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
				}
				else
				{
					sGenMapID = pChar->m_sStartMapID;
					dwGenGateID = pChar->m_dwStartGate;
				}
			}
		}

		if (pLAND->IsClubDeathMatchMap())
		{
			GLClubDeathMatch* pCDM = m_pClubDeathMatchMan->Find(pLAND->GetGuidance());

			if ( pCDM )
			{
				sGenMapID = pCDM->m_dwClubHallMap;
				dwGenGateID = pCDM->m_dwClubHallGate;
			}
		}
	}

	// 필드서버 정보를 가져와서 필드서버로 접속
	//
	bool bCANNOTFINDMAP(false);
	DWORD dwFieldServer = GetFieldServer ( sGenMapID );
	if ( dwFieldServer == FIELDSERVER_MAX )
	{
		bCANNOTFINDMAP = true;

        //	캐릭터가 생성될 필드서버를 찾지 못함. 그러나 진입할 수 있도록 초기화함 ( 캐릭터별 학교의 0번 게이트 )
        sc::writeLogError(
            sc::string::format(
                "Can't find field server associate with character Map %1%/%2% Cha name %3% ChaDbNum %4%",
			    sGenMapID.wMainID,
                sGenMapID.wSubID,
                pChar->m_szName,
                pChar->ChaDbNum()));

		// DBAction 작업 끝나면 바로 제거하고 테스트 해라
		// return FALSE;

		sGenMapID = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
		dwGenGateID = GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];

		GLAGLandMan* pLandMan = GetLand(sGenMapID);
		if (!pLandMan)
		{
            sc::writeLogError(
                sc::string::format(
                    "Can't find Start Map %1%/%2%",
                    sGenMapID.Mid(),
                    sGenMapID.Sid()));
			return FALSE;
		}

		DxLandGateMan* pGateMan = pLandMan->GetLandGateMan();
		if (!pGateMan)
		{
            sc::writeLogError(
                sc::string::format(
                    "Can't find GateMan Map %1%/%2% ChaDbNum %3%",
                    sGenMapID.Mid(),
                    sGenMapID.Sid(),
                    pChar->ChaDbNum()));
			return FALSE;

		}
		DxLandGate* pGate = pGateMan->FindLandGate(dwGenGateID);
		if (!pGate)
		{
            sc::writeLogError(
                sc::string::format(
                    "Can't find gate. Map %1%/%2% Gate %3% ChaDbNum %4%",
                    sGenMapID.Mid(),
                    sGenMapID.Sid(),
                    dwGenGateID,
                    pChar->ChaDbNum()));
			vGenPos = D3DXVECTOR3(0,0,0);
		}
		else
		{
			vGenPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
		}

		dwFieldServer = GetFieldServer ( sGenMapID );
		if ( dwFieldServer == FIELDSERVER_MAX )
		{
            sc::writeLogError(
                sc::string::format(
			        "Can't Find FieldServer associate with cha school. School %1% Map %2%/%3%",
                    pChar->m_wSchool,
                    sGenMapID.Mid(),
                    sGenMapID.Sid()));
			return FALSE;
		}

		pChar->m_sStartMapID = sGenMapID;
		pChar->m_dwStartGate = dwGenGateID;
		pChar->m_vStartPos   = vGenPos;
		pChar->m_sSaveMapID = sGenMapID;
		pChar->m_vSavePos   = vGenPos;

		// 이부분에서 변경된 스타트지점(MapID, GateID, 위치) 을 DB에 저장함
        AddGameAdoJob(
            db::DbActionPtr(
		        new db::CharacterPositionSet(
                    pChar->ClientSlot(), 
                    pChar->m_CharDbNum,
                    dwFieldServer,
                    pChar->m_sStartMapID.dwID,
                    pChar->m_dwStartGate,
                    pChar->m_vStartPos.x,
                    pChar->m_vStartPos.y,
                    pChar->m_vStartPos.z,
                    pChar->m_sSaveMapID.dwID,
                    pChar->m_vSavePos.x,
                    pChar->m_vSavePos.y,
                    pChar->m_vSavePos.z,
                    pChar->m_sSaveMapID.dwID,
                    pChar->m_vSavePos.x,
                    pChar->m_vSavePos.y,
                    pChar->m_vSavePos.z)));
	}

	// 만약 맵을 못 찾았다면 맵을 초기화 하고 다른 곳에서 캐릭터 조인정보를 전송한다
	if (bCANNOTFINDMAP)
        return true;

	//if (m_pMsgServer->ConnectFieldSvr(pChar->ClientSlot(), dwFieldServer, pChar->GaeaId(), pChar->m_nChannel) != NET_OK)
	if (m_pMsgServer->SetUpFieldInfo(pChar->ClientSlot(), dwFieldServer, pChar->GaeaId(), pChar->m_nChannel) != NET_OK)
	{
        //	캐릭터가 생성될 필드서버로 접속이 실패함
        sc::writeLogError(
            sc::string::format(
                "Can't connect field. FIELD %1% Cha name %2% ChaDbNum %3%",
			    dwFieldServer,
                pChar->m_szName,
                pChar->ChaDbNum()));
		return FALSE;
	}

	//	캐릭터 조인 정보 전송
	NET_GAME_JOIN_FIELDSVR NetJoinField(pChar->GetCountry());
	NetJoinField.emType				= EMJOINTYPE_FIRST;
	NetJoinField.dwSlotAgentClient	= pChar->ClientSlot();
	NetJoinField.SetUserId(pChar->m_szUserName);
	NetJoinField.nUserNum			= pChar->m_UserDbNum;
	NetJoinField.dwUserLvl			= pChar->m_dwUserLvl;
	NetJoinField.nChaNum			= pChar->m_CharDbNum;
	NetJoinField.dwGaeaID			= pChar->GaeaId();
	NetJoinField.PartyID				= pChar->GetPartyID();
    NetJoinField.tPREMIUM			= pChar->m_tPREMIUM;
	NetJoinField.tCHATBLOCK			= pChar->m_tCHATBLOCK;

	NetJoinField.sStartMap			= sGenMapID;
	NetJoinField.dwStartGate		= dwGenGateID;
	NetJoinField.vStartPos			= vGenPos;

	NetJoinField.dwActState			= pChar->m_dwActState;
	NetJoinField.bUseArmSub			= pChar->m_bUseArmSub;

	NetJoinField.emIPBonus			= pChar->m_emIPBonus;

	NetJoinField.sChinaTime			= pChar->m_sChinaTime;
	NetJoinField.sEventTime			= pChar->m_sEventTime;
	NetJoinField.PartyID				= pChar->GetPartyID();
	NetJoinField.sVietnamGainSystem = pChar->m_sVietnamSystem;

	NetJoinField.dwCountryID		= pChar->GetCountryInfo().dwID;

	if ( pChar->GetCryptKey().guid != GUID_NULL )
	{
		NetJoinField.ckey = pChar->GetCryptKey();
	}

	//** Add EventTime

	SENDTOFIELD ( pChar->ClientSlot(), &NetJoinField );

	// 케릭터의 현재 접속 필드서버 및 현재 맵 ID 지정.
	//
	SetCurrentField(pChar, dwFieldServer, sGenMapID);
	return TRUE;
}

BOOL GLAgentServer::MsgReqGenItemAgt ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_REQ_GENITEM_AGT *pNetMsg = (GLMSG::SNET_REQ_GENITEM_AGT *) nmg;

	int nCHANNEL = pNetMsg->nCHANNEL;

	const SNATIVEID &sNID = pNetMsg->sNID;
	
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( sNID );	
	if ( !pITEM )
		return FALSE;

	SMAPNODE* pMAPNODE = m_sMapList.FindMapNode ( pNetMsg->sMAPID );
	if ( !pMAPNODE )
		return FALSE;

	int nSvrNum = (int) pMAPNODE->GetFieldServerNum();

	if ( m_pItemLimit->DoCheckItemGen ( sNID ) )
	{
		sc::MGUID guid;
        sc::SeqUniqueGUID::Instance()->getGUID( guid );

		GLMSG::SNET_REQ_GENITEM_FLD_FB NetMsgFb;
		NetMsgFb.sNID = pNetMsg->sNID;
		NetMsgFb.sMAPID = pNetMsg->sMAPID;
		NetMsgFb.emGENTYPE = pNetMsg->emGENTYPE;
		NetMsgFb.guid = guid;
		NetMsgFb.emHoldGroup = pNetMsg->emHoldGroup;
		NetMsgFb.dwHoldGID = pNetMsg->dwHoldGID;
		NetMsgFb.vPos = pNetMsg->vPos;

		SENDTOFIELDSVR ( nCHANNEL, nSvrNum, &NetMsgFb );
	}

	return TRUE;
}

BOOL GLAgentServer::SendToGMMsgLink( GLCharAG* pChar, GLMSG::NET_CHAT_LINK_FB* pNetChatFB )
{
	UINT i;
	for( i = 0; i < pChar->m_vecGetWhisperList.size(); i++ )
	{
		GLCharAG* pSendGMChar = GetChar( pChar->m_vecGetWhisperList[i].dwGaeaID );
		if( pSendGMChar == NULL || 
			pSendGMChar->m_GmWhisperList.dwClientID != pChar->ClientSlot() ||
			pSendGMChar->m_GmWhisperList.dwGaeaID != pChar->GaeaId() ||
			pSendGMChar->m_bViewWhisperMSG == FALSE)
		{
			pChar->m_vecGetWhisperList.erase( pChar->m_vecGetWhisperList.begin() + i );
			i--;
			continue;
		}
		SENDTOCLIENT( pChar->m_vecGetWhisperList[i].dwClientID, pNetChatFB );
	}

	return TRUE;
}

//FA
BOOL GLAgentServer::MsgReqCreateInstantMap ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	GLMSG::SNET_CREATE_INSTANT_MAP_REQ *pNetMsg = (GLMSG::SNET_CREATE_INSTANT_MAP_REQ *) nmg;
	GLMSG::SNET_CREATE_INSTANT_MAP_FB NetMsgFB;	//AC

	//GLCharAG* pChar = GetChar(pNetMsg->dwGaeaID); //mjeon.AF.이 때 메시지에 포함된 GaeaID도 유효하나 ChaNum을 사용하도록 한다.
	GLCharAG *pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pChar)
		return FALSE;

	// 현재 맵 정보
	//
	GLAGLandMan* pCurLandMan = GetLand(pChar->GetCurrentMap().getGaeaMapID());
	if ( !pCurLandMan )
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_MAPID., Name[%1%], CurMapID[%2%/%3%], bCreate[%4%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pNetMsg->bCreate ) );
		*/		

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_MAPID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 현재 게이트
	DxLandGateMan* pCurGateMan = pCurLandMan->GetLandGateMan ();
	DxLandGate* pCurGate = pCurGateMan->FindLandGate ( pNetMsg->dwGateID );
	if ( !pCurGate )
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID., Name[%1%], CurMapID[%2%/%3%], bCreate[%4%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pNetMsg->bCreate ) );
		*/

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_GATEID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 목표 맵.
	//
	GLAGLandMan* pNextLandMan = GetLand ( pCurGate->GetToMapID() );
	if ( !pNextLandMan )
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID., Name[%1%], CurMapID[%2%/%3%], ToMapID[%4%/%5%], bCreate[%6%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pCurGate->GetToMapID().wMainID, pCurGate->GetToMapID().wSubID, 
			pNetMsg->bCreate ) );
		*/

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 목표 게이트.
	//
	DxLandGateMan* pNextGateMan = pNextLandMan->GetLandGateMan ();
	DxLandGate* pNextGate = pNextGateMan->FindLandGate ( pCurGate->GetToGateID() );
	if ( !pNextGate )
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID., Name[%1%], CurMapID[%2%/%3%], ToMapID[%4%/%5%], bCreate[%6%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pCurGate->GetToMapID().wMainID, pCurGate->GetToMapID().wSubID, 
			pNetMsg->bCreate ) );
		*/

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_TARMAPID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}
	const SMAPNODE* pMapNode = m_sMapList.FindMapNode (  pCurGate->GetToMapID() );
	
	if( pMapNode->GetInstanceType() != SMAPNODE::EMINSTANCE_OLD_INSTANCE )
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT., Name[%1%], CurMapID[%2%/%3%], ToMapID[%4%/%5%], bCreate[%6%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pCurGate->GetToMapID().wMainID, pCurGate->GetToMapID().wSubID, 
			pNetMsg->bCreate ) );
		*/

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}
	DWORD dwNextFieldSvr = GetFieldServer ( pCurGate->GetToMapID() );
	if( pChar->m_dwCurFieldSvr != dwNextFieldSvr )
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL., Name[%1%], CurMapID[%2%/%3%], ToMapID[%4%/%5%], bCreate[%6%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pCurGate->GetToMapID().wMainID, pCurGate->GetToMapID().wSubID, 
			pNetMsg->bCreate ) );
		*/

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	GLMSG::SNET_CREATE_INSTANT_MAP_FLD NetMsgFld;	//AF
	NetMsgFld.dwChaNum = pChar->ChaDbNum();	    

	// 이미 생성한 인던을 모두 검사한다.
	VEC_INSTANT_MAPID_ITER iter = m_vecInstantMapId.begin();
	BOOL bFindEnterMap = FALSE;

	for( ; iter != m_vecInstantMapId.end(); ++iter)
	{
        const DWORD currentInstanceMapID(*iter);
        GLAGLandManager* pLandManager(GetLandManager(currentInstanceMapID));

        if (!pLandManager)
            continue;

        if (pLandManager->getBaseMapID() != pCurGate->GetToMapID())
            continue;

        GLAGLandMan* pLand(pLandManager->getLand());
        if ( pLand == NULL )
            continue;

        // 주인이 같은 인던이 있는지 찾는다.
        const GLPartyID& PartyID = pChar->GetPartyID();
        if (PartyID.isValidParty() == true )
        {
            if ( (PartyID == pLand->GetInstantMapHostID()) && pLand->IsPartyInstantMap() )
            {
                bFindEnterMap = TRUE;
                pNetMsg->instanceMapID = pLand->GetGaeaMapID().Mid();
                break;
            }
        }
        else
        {
            if ( (pChar->GaeaId() == pLand->GetInstantMapHostID()) && !pLand->IsPartyInstantMap() )
            {
                bFindEnterMap = TRUE;
                pNetMsg->instanceMapID = pLand->GetGaeaMapID().Mid();
                break;
            }
        }
	}
	
	if( (pNetMsg->bCreate && bFindEnterMap) ||  // 생성 요청이 왔는데 이미 생성된 인던이 있는경우 메시지를 보내고 리턴한다.
		(!bFindEnterMap && !pNetMsg->bCreate) ) // 입장 요청이 왔는데 이미 생성된 인던이 없는경우 메시지를 보내고 리턴한다.
	{
		/*
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT_STATE., Name[%1%], CurMapID[%2%/%3%], ToMapID[%4%/%5%], bCreate[%6%]",
			pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
			pCurGate->GetToMapID().wMainID, pCurGate->GetToMapID().wSubID, 
			pNetMsg->bCreate ) );
		*/

		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_NOT_STATE;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}    
	
	GLAGLandManager* pInLandMan = NULL;
	if( pNetMsg->bCreate )
	{
		// 새로 입장했을 때만 진입 제한 시간 검사
		MAP_ENTRYLIMIT_ITER mapIter = m_mapEntryLimitInfo.find(pChar->m_CharDbNum);
		if( mapIter != m_mapEntryLimitInfo.end() )
		{
			LIST_ENTRYINFO_ITER listIter = mapIter->second.begin();
			for( ; listIter != mapIter->second.end(); ++listIter )
			{
				if( listIter->sMapID == pCurGate->GetToMapID() )
				{
					/*
					sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. EMCHAR_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT., Name[%1%], CurMapID[%2%/%3%], ToMapID[%4%/%5%], bCreate[%6%]",
						pChar->m_szName, pChar->m_sCurMapID.wMainID, pChar->m_sCurMapID.wSubID, 
						pCurGate->GetToMapID().wMainID, pCurGate->GetToMapID().wSubID, 
						pNetMsg->bCreate ) );
					*/

					NetMsgFB.mapID = pCurGate->GetToMapID();
					NetMsgFB.tEntryTime =  listIter->tEntryTime;
					NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT;

					SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
					return FALSE;
				}
			}
		}
		//DWORD StartTime = ::GetTickCount();

		const SNATIVEID& baseMapID(pCurGate->GetToMapID());
        const DWORD _instanceMapID(CreateInstantMap(baseMapID, pChar->m_nChannel, pChar->m_dwCurFieldSvr));
        if ( _instanceMapID >= MAX_LAND_MID )
		{
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
			SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );

			/*
			sc::writeLogDebug(sc::string::format( "Create Failed Instant Map!, Remain IDSize %1%, Map ID [%2%][%3%]", 
									MAX_CREATE_INSTANT_MAP_NUM - m_sInsertInstantMapId.useMapSubIDSet.size(), 
									sInstantMapID.wMainID, sInstantMapID.wSubID ) );
			*/

			// 신규인던 정보가 아니다, 그렇기 때문에 바로 삭제한다;
			DeleteInstantMap(_instanceMapID);
			return FALSE;
		}

		pInLandMan = GetLandManager(_instanceMapID);
		if (!pInLandMan)
		{
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
			SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);

			/*
			sc::writeLogDebug(sc::string::format( "Create Failed Search Failed Map!, Remain IDSize %d, Map ID [%2%][%3%]", 
									MAX_CREATE_INSTANT_MAP_NUM - m_sInsertInstantMapId.useMapSubIDSet.size(), 
									sInstantMapID.wMainID, sInstantMapID.wSubID ) );
			*/

			// 신규인던 정보가 아니다, 그렇기 때문에 바로 삭제한다;
			DeleteInstantMap(_instanceMapID);
			return FALSE;
		}

		pInLandMan->setInstantMapData(pChar->GaeaId(), pChar->GetPartyID());

		/*
		DWORD ElspTime = ::GetTickCount() - StartTime;

		HANDLE hProcess			= GetCurrentProcess(); 
		PROCESS_MEMORY_COUNTERS pmc;
		if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
		{
			sc::writeLogDebug(sc::string::format( "Create Instant Map, Name[%1%],Memory %2%, Time %3% msec, MapInfo: ID [%4%][%5%], PartyMap %6%, HostID %7%.", 
								pChar->m_szName, pmc.WorkingSetSize, ElspTime, sInstantMapID.wMainID, sInstantMapID.wSubID,
								pInLandMan->IsPartyInstantMap(), pInLandMan->GetInstantMapHostID() ) );
		}
		*/


		NetMsgFld.bCreate	  = TRUE;
		NetMsgFld.dwGaeaID	  = pNetMsg->dwGaeaID;
		NetMsgFld.dwGateID	  = pNetMsg->dwGateID;
		NetMsgFld.dwGateToIndex	= pNetMsg->dwGateToIndex;
		NetMsgFld.instanceMapID	= _instanceMapID;
		NetMsgFld.sBaseMapID  = baseMapID;

	}
	else
	{
		pInLandMan = GetLandManager(pNetMsg->instanceMapID);

		if (!pInLandMan)
		{
			NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_FAIL;
			SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}


		const DWORD _instanceMapID(pNetMsg->instanceMapID);
		NetMsgFld.bCreate		= FALSE;
		NetMsgFld.bCreateTest	= FALSE;
		NetMsgFld.dwGaeaID		= pNetMsg->dwGaeaID;
		NetMsgFld.dwGateID		= pNetMsg->dwGateID;
		NetMsgFld.dwGateToIndex	= pNetMsg->dwGateToIndex;
		NetMsgFld.instanceMapID	= _instanceMapID;
		NetMsgFld.sBaseMapID	= pCurGate->GetToMapID();
		
	}

	if( pInLandMan && pInLandMan->getLand()->GetUseLevelEtcFunc(EMETCFUNC_REENTRYLIMIT) )
	{
		SENTRYLIMITINFO sEntryInfo;
		CTime time = CTime::GetCurrentTime();
		sEntryInfo.SetData( time.GetTime(), pInLandMan->getLand()->GetLevelEtcFunc()->m_dwReEntryTime, pCurGate->GetToMapID() );

		bool bFind = FALSE;

		MAP_ENTRYLIMIT_ITER mapIter = m_mapEntryLimitInfo.find(pChar->m_CharDbNum);
		if( mapIter != m_mapEntryLimitInfo.end() )
		{
			LIST_ENTRYINFO_ITER listIter = mapIter->second.begin();
			
			for( ; listIter != mapIter->second.end(); ++listIter )
			{
				if( listIter->sMapID.wMainID == pCurGate->GetToMapID().wMainID )
				{					
					bFind = TRUE;
					break;
				}
			}
		}

		if( !bFind ) m_mapEntryLimitInfo[pChar->m_CharDbNum].push_back(sEntryInfo);
	}
	
	// 필드 서버 변경 필요 없음. 현재 필드서버로 인던 맵 생성을 요청
	SENDTOFIELDSVR ( pChar->m_nChannel, pChar->m_dwCurFieldSvr, &NetMsgFld );	

	return TRUE;
}

/*
BOOL GLAgentServer::MsgReqTestCreateInstantMap( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_TESTCREATE_INSTANT_MAP_REQ *pNetMsg = (GLMSG::SNET_TESTCREATE_INSTANT_MAP_REQ *) nmg;

	std::list<STestInstantMapCharInfo>::iterator iter = m_listTestCreateInstMapInfo.begin();
	for( ; iter != m_listTestCreateInstMapInfo.end(); ++iter )
	{
		STestInstantMapCharInfo oldCreateInfo = *iter;
		if( oldCreateInfo.dwGaeaID == dwGaeaID )
			return TRUE;
	}
	STestInstantMapCharInfo createInfo;
	createInfo.dwGaeaID     = dwGaeaID;
	createInfo.fCreateTime  = pNetMsg->fCreateTime;
	createInfo.sBaseMapID   = pNetMsg->sBaseMapID;
	createInfo.fElapsedTime = 0.0f;

	m_listTestCreateInstMapInfo.push_back(createInfo);

	return TRUE;	
}
*/

BOOL GLAgentServer::MsgReqReEntryRemainTime( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pChar = GetChar ( dwGaeaID );
	
	if ( !pChar )
		return FALSE;

	if ( pChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNET_INSTANT_MAP_REENTRY_CHECK_FB NetMsgFB;

	NetMsgFB.nLimitCount = 0;

	MAP_ENTRYLIMIT_ITER mapIter = m_mapEntryLimitInfo.find(pChar->m_CharDbNum);
	if( mapIter != m_mapEntryLimitInfo.end() )
	{
		LIST_ENTRYINFO_ITER listIter = mapIter->second.begin();
		for( ; listIter != mapIter->second.end(); ++listIter )
		{
			NetMsgFB.nRemainMinute[NetMsgFB.nLimitCount] = listIter->GetEntryLimitMinutes();
			NetMsgFB.sMapID[NetMsgFB.nLimitCount]		 = listIter->sMapID;

			NetMsgFB.nLimitCount++;

			if( NetMsgFB.nLimitCount >= MAX_INSTANT_TYPE )
				break;
		}
	}

	SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );

	return TRUE;
}

BOOL GLAgentServer::TestCreateInstantMap( float fElpasedTime )
{
	if( m_listTestCreateInstMapInfo.size() == 0 )
		return FALSE;

	std::list<STestInstantMapCharInfo>::iterator iter = m_listTestCreateInstMapInfo.begin();
	for( ; iter != m_listTestCreateInstMapInfo.end(); ++iter )
	{
		iter->fElapsedTime += fElpasedTime;
		if( iter->fElapsedTime < iter->fCreateTime )
			continue;

		iter->fElapsedTime = 0.0f;

		STestInstantMapCharInfo createInfo(*iter);
		if( createInfo.sBaseMapID == NATIVEID_NULL() )
			continue;

		GLCharAG* pChar = GetChar ( createInfo.dwGaeaID );
		if ( !pChar )
		{
			createInfo.dwGaeaID	  = 0;
			createInfo.fElapsedTime = 0.0f;
			createInfo.sBaseMapID   = NATIVEID_NULL();
			m_listTestCreateInstMapInfo.erase( iter-- );
			continue;
		}

		SNATIVEID& sBaseMapID = createInfo.sBaseMapID;
		SMAPNODE* const pMapNode(m_sMapList.FindMapNode(sBaseMapID));
		if ( pMapNode == NULL )
			return FALSE;		

		//DWORD StartTime = ::GetTickCount();

        const DWORD _instanceMapID(CreateInstantMap(sBaseMapID, pChar->m_nChannel, pMapNode->GetFieldServerNum()));
        if ( _instanceMapID == GAEAID_NULL )
		{
			continue;
		}

		GLAGLandMan* pInLandMan = NULL;
        const SNATIVEID _mapID(_instanceMapID, 0);
        pInLandMan = GetLand(_mapID);
        if ( pInLandMan == NULL )
			continue;

		/*
		DWORD ElspTime = ::GetTickCount() - StartTime;

		HANDLE hProcess			= GetCurrentProcess(); 
		PROCESS_MEMORY_COUNTERS pmc;
		if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
		{
			sc::writeLogDebug(sc::string::format( "Create Instant Map, Memory %1%, Time %2% msec, MapInfo: ID [%3%][%4%], PartyMap %5%, HostID %6%.", 
								pmc.WorkingSetSize, ElspTime, sInstantMapID.wMainID, sInstantMapID.wSubID,
								pInLandMan->IsPartyInstantMap(), pInLandMan->GetInstantMapHostID() ) );
		}
		*/

		GLMSG::SNET_CREATE_INSTANT_MAP_FLD NetMsgFld;	//AF
		NetMsgFld.dwChaNum		= pChar->ChaDbNum();
		NetMsgFld.dwGaeaID	    = createInfo.dwGaeaID;	
		NetMsgFld.instanceMapID = _instanceMapID;
		NetMsgFld.sBaseMapID	= sBaseMapID;
		NetMsgFld.bCreate		= TRUE;
		NetMsgFld.bCreateTest	= TRUE;


		SENDTOFIELDSVR ( pChar->m_nChannel, pChar->m_dwCurFieldSvr, &NetMsgFld );
	}

	return TRUE;
}

BOOL GLAgentServer::MsgReqDeleteInstantMap( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( dwGaeaID != GAEAID_NULL )
	{
		sc::writeLogError(std::string("GLAgentServer::MsgReqDeleteInstantMap(). dwGaeaID != GAEAID_NULL"));
		return FALSE;
	}

	// Field -> Agent
	GLMSG::SNET_CREATE_INSTANT_MAP_DEL *pNetMsg = (GLMSG::SNET_CREATE_INSTANT_MAP_DEL *) nmg;

	// 신규인던 정보가 아니다, 그렇기 때문에 바로 삭제한다;
	DeleteInstantMap( pNetMsg->instanceMapID );

	return TRUE;	
}


// *****************************************************
// Desc: 이전게이트로 이동 요청 (Field->Agent)
// *****************************************************
BOOL GLAgentServer::MsgReqGateOut ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETREQ_GATEOUT *pNetMsg = (GLMSG::SNETREQ_GATEOUT *) nmg;
	//if ( dwGaeaID!=GAEAID_NULL )			return FALSE;

	//GLCharAG* pChar = GetChar ( pNetMsg->dwGaeaID );
	GLCharAG *pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if ( !pChar )
		return FALSE;

	// 현재 맵 정보.
	//
	GLAGLandMan* pCurLandMan = GetLand(pChar->GetCurrentMap().getGaeaMapID());
	if ( !pCurLandMan )
	{
        sc::writeLogError(
            sc::string::format(
                "EMCHAR_GATEOUT_MAPID %1% %2%", pChar->m_CharDbNum, pChar->GetCurrentMap().getGaeaMapID().Id()));

		GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
		NetMsgFB.emFB = EMCHAR_GATEOUT_MAPID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}
	
	// 현재 게이트
	DxLandGateMan* pCurGateMan = pCurLandMan->GetLandGateMan ();
	DxLandGate* pCurGate = pCurGateMan->FindLandGate ( pNetMsg->dwGateID );
	if ( !pCurGate )
	{
        sc::writeLogError(
            sc::string::format(
                "EMCHAR_GATEOUT_GATEID %1% %2% %3%",
                pChar->m_CharDbNum, 
			    pChar->GetCurrentMap().getGaeaMapID().Id(),
                pNetMsg->dwGateID));

		GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
		NetMsgFB.emFB = EMCHAR_GATEOUT_GATEID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}	

	const SNATIVEID& targetMapID = pCurGate->GetToMapID(pNetMsg->dwGateToIndex);
	// 목표 맵
	GLAGLandMan* pNextLandMan = GetLand ( targetMapID );
	if (!pNextLandMan)
	{
        sc::writeLogError(
            sc::string::format(
                "EMCHAR_GATEOUT_TARMAPID %1% %2% %3% %4%", 
			    pChar->m_CharDbNum,
                pChar->GetCurrentMap().getGaeaMapID().Id(),
                pCurGate->GetGateID(),
                targetMapID.dwID));

		GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
		NetMsgFB.emFB = EMCHAR_GATEOUT_TARMAPID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	/*
		mjeon.CaptureTheField

		IsCTFMap ?
		
		1.CTF PVP맵으로는 이동할 수 없다. (오직, CTF 시스템에 의해서만 Recall될 수 있다.)
		
		2.CTF START 상태에서 Gate를 타고 들어올 수 없다.
	*/

	if(pCurLandMan->GetBaseMapID().dwID != m_pCTFMan->GetPvPMapID() &&	//PvP맵 -> PvP맵으로의 게이트 이동은 허용
		pNextLandMan->GetBaseMapID().dwID == m_pCTFMan->GetPvPMapID())
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgReqGateOut() - %1% is tring to enter CTF-PVP map!", pChar->m_CharDbNum));
		
		GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
		NetMsgFB.emFB = EMCHAR_GATEOUT_CTF;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		
		return FALSE;
	}


	// 이제 점령전 진행중에도 사냥터로 이용할 수 있게 되었다;
	/*if(pNextLandMan->GetBaseMapID().dwID == m_pCTFMan->GetPvEMapID() && m_pCTFMan->IsStarted())
	{		
		sc::writeLogError(std::string("GLAgentServer::MsgReqGateOut() - Cannot enter to CTF map during started CTF."));
		
		GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
		NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		
		return FALSE;
	}*/	


	// 목표맵이 선두클럽 전투존인데 선도전이 끝났고 선도클럽의 길원이 아니면
	if (pNextLandMan->IsClubBattleMap())
	{
		if ( GLCONST_CHAR::bPKLESS ) 
		{
			GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
			NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
			SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}

		GLGuidance *pGuid = m_pGuidance->Find ( pNextLandMan->GetGuidance() );
		if ( pGuid )
		{

			// 선도전 일때 길트배틀 중이거나 종료시간 미달이면 출입금지
			std::tr1::shared_ptr<GLClubAgent> pClub = m_pClubMan->GetClub ( pChar->m_ClubDbNum );
			if ( pClub && pGuid->m_bBattle && ( pClub->GetAllBattleNum() > 0 || !pClub->IsGuidBattle()) )
			{
				GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
				NetMsgFB.emFB = EMCHAR_GATEOUT_CLUBBATTLE;
				SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
				return FALSE;
			}

			std::tr1::shared_ptr<GLClubAgent> pGuidanceClub = m_pClubMan->GetClub ( pGuid->m_dwGuidanceClub );
			if ( !pGuid->m_bBattle && !pGuid->IsGuidClub(pChar->m_ClubDbNum) )
			{
				if ( pGuidanceClub )
				{
					// 선도클럽의 동맹이 아니면
					if (pClub == NULL || !pGuidanceClub->IsAllianceGuild_COMMON(pClub->m_DbNum, pClub->m_dwAlliance))
					{
						GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
						NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
						SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
						return FALSE;
					}
				}
			}
		}
	}

	// 목표맵이 선두클럽 CDM존인데 CDM존이 끝났거나 진입 불가 상태일때
	if (pNextLandMan->IsClubDeathMatchMap())
	{
		if ( GLCONST_CHAR::bPKLESS || pChar->m_nChannel != 0) 
		{
			GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
			NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
			SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}

		GLClubDeathMatch *pCDM = m_pClubDeathMatchMan->Find(pNextLandMan->GetGuidance());
		if ( pCDM )
		{
			std::tr1::shared_ptr<GLClubAgent> pClub = m_pClubMan->GetClub( pChar->m_ClubDbNum );

			if ( !pCDM->IsEnterMap( pChar->m_ClubDbNum ) || !pClub )
			{
				GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
				NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
				SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
				return FALSE;
			}

            // CDM 인원 체크.
            if ( !pCurLandMan->IsClubDeathMatchMap() )
            {
                if ( !pNextLandMan->CheckCDMMembersLimited( pChar->m_ClubDbNum ) )
                {
                    GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
                    NetMsgFB.emFB = EMCHAR_GATEOUT_CDM_MEMS_LIMITED;
                    SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );

                    return FALSE;
                }
            }
			
			if ( pClub )
			{
				// 클럽배틀 진행중일때는 못들어간다.
                if ( pClub->GetAllBattleNum() > 0 )
				{
					GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
					NetMsgFB.emFB = EMCHAR_GATEOUT_CLUBBATTLE2;
					SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
					return FALSE;
				}

				if ( !pClub->IsCDMEnter(pChar->m_CharDbNum) )
				{
					GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
					NetMsgFB.emFB = EMCHAR_GATEOUT_HOLD;
					SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
					return FALSE;
				}
			}			
		}
	}

	// 목표 게이트.
	//
	DxLandGateMan* pNextGateMan = pNextLandMan->GetLandGateMan ();
	const DWORD dwToGateID = pCurGate->GetToGateID(pNetMsg->dwGateToIndex);
	DxLandGate* pNextGate = pNextGateMan->FindLandGate ( dwToGateID );
	if (!pNextGate)
	{
        sc::writeLogError(
            sc::string::format(
                "EMCHAR_GATEOUT_TARGATEID %1% %2% %3% %4% %5%", 
			    pChar->m_CharDbNum,
                pChar->GetCurrentMap().getGaeaMapID().Id(),
                pNetMsg->dwGateID,
                targetMapID.dwID, 
			    dwToGateID));

		GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
		NetMsgFB.emFB = EMCHAR_GATEOUT_TARGATEID;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

    // 신규 인던시스템일 경우에 스크립트쪽의 처리를 할 수 있도록 한다;
    SMAPNODE* pMapNode = m_sMapList.FindMapNode ( targetMapID );
    if ( pMapNode )
    {
        if ( pMapNode->IsNewInstanceMap() )
        {
            const DWORD dbNum(pChar->ChaDbNum());
            if ( m_pInstanceManager->doScript(dbNum, targetMapID, pNetMsg->dwGateID, dwToGateID) == true )
            { // 스크립트가 존재 하면 스크립트 쪽에서 처리하도록 한다;
                return TRUE;
            }
			else
			{
				sc::writeLogInfo( "[ Instance Log ] [ Failed Gate Move. ]" );

				//GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
				//NetMsgFB.emFB = EMCHAR_GATEOUT_FAIL;
				//SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );

				return FALSE;
			}
        }
    }

	// 필드 서버가 같을 경우;
	DWORD dwNextFieldSvr = GetFieldServer ( targetMapID );
	if ( dwNextFieldSvr == pChar->m_dwCurFieldSvr )
	{
		// Agent 에 있는 캐릭터 맵정보 변경;
		SetCurrentField(pChar, pChar->m_dwCurFieldSvr, targetMapID);

		// 필드 서버 변경 필요 없음. 현재 필드서버로 맵이동을 요청;
		SENDTOFIELDSVR ( pChar->m_nChannel, pChar->m_dwCurFieldSvr, nmg );
	}
	// 필드 서버가 다를 경우;
	else
	{
		MoveSystem::SMOVE_INFO_AGENT sTempMoveInfo;
		sTempMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_GATE;
		sTempMoveInfo.dwChaDBNum = pChar->ChaDbNum();
		sTempMoveInfo.sMapID = targetMapID;
		sTempMoveInfo.dwGateID = dwToGateID;
		sTempMoveInfo.vPosition = D3DXVECTOR3( FLT_MAX,FLT_MAX,FLT_MAX );
		sTempMoveInfo.dwFieldNum = dwNextFieldSvr;
		sTempMoveInfo.dwChannelNum = pChar->GetChannel();

		if ( false == MoveSystem::CMoveManagerServer::Instance()->MoveMap( sTempMoveInfo ) )
		{
			return FALSE;
		}
	}

		/*// 필드 서버가 정상인지 검사.
		//
		//BOOL bSVR_OK = m_pMsgServer->IsSlotOnline ( dwNextFieldSvr );
		bool bSVR_OK = IsFieldOnline(pChar->m_nChannel, dwNextFieldSvr);
		if (!bSVR_OK)
		{
			GLMSG::SNETREQ_GATEOUT_FB NetMsgFB;
			NetMsgFB.emFB = EMCHAR_GATEOUT_FIELDSVR;
			SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}

		// 필드 서버 변경시 클라이언트 정보 변경 사항 반영.
		//
		ReActionMoveFieldSvr ( pChar->ClientSlot(), pChar->GaeaId() );

		//	현재 필드 서버에 자신이 떠나갈 예정임을 통보.
		GLMSG::SNETPC_FIELDSVR_OUT NetMsg;
		NetMsg.dwChaNum = pChar->ChaDbNum();
		NetMsg.dwSvrGroupNum = m_pMsgServer->GetServerGroup();
		NetMsg.targetMapID = targetMapID;
		SENDTOFIELD ( pChar->ClientSlot(), &NetMsg );

		//	이동할 맵 ID 저장.
		pChar->SetNextFieldSvr ( targetMapID, dwToGateID, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX), dwNextFieldSvr );*/


		
	//}

	return TRUE;
}

BOOL GLAgentServer::CheckStartMap( DWORD dwGaeaID )
{
	GLCharAG* pChar = GetChar ( dwGaeaID );
	
	if ( !pChar )
		return FALSE;

	GLAGLandMan *pLandMan=NULL;
	DWORD dwGenGate = 0;

	pLandMan = GetLand ( pChar->m_sStartMapID );

	if (pLandMan && pLandMan->IsClubDeathMatchMap())
	{
		if ( GLCONST_CHAR::bPKLESS )
			return FALSE;

		if ( pChar->m_nChannel != 0 )
			return FALSE;

        GLClubDeathMatch *pCDM = m_pClubDeathMatchMan->Find(pLandMan->GetGuidance());
		if (pCDM && !pCDM->IsEnterMap(pChar->m_ClubDbNum))
            return FALSE;

		std::tr1::shared_ptr<GLClubAgent> pClub = m_pClubMan->GetClub ( pChar->m_ClubDbNum );		
		if ( !pClub )
			return FALSE;		

		// 클럽배틀 진행중일때는 못들어간다.
		if ( pClub->GetAllBattleNum() > 0 )
			return FALSE;

		if ( !pClub->IsCDMEnter(pChar->m_CharDbNum) )
			return FALSE;
	}

	if ( !pLandMan ) 		
	{
		return FALSE;
	}
	else if ( !pLandMan->GetLandGateMan()->FindLandGate(pChar->m_dwStartGate) )
	{
		DxLandGate* pLandGate = pLandMan->GetLandGateMan()->GetListLandGate();
		if ( !pLandGate )
			return FALSE;	//	Gate가 하나도 없는 곳엔 부활할 수 없음.
	}

	return TRUE;
}

/*BOOL GLAgentServer::MoveToMap( GLCharAG* pChar, SNATIVEID& sMapID, DWORD dwGenGate )
{
	if( NULL == pChar )
		return FALSE;

	DWORD dwNextFieldSvr = GetFieldServer( sMapID );

	if( dwNextFieldSvr == pChar->m_dwCurFieldSvr )
	{
		SetCurrentField( pChar, pChar->m_dwCurFieldSvr, sMapID );

		return FALSE;
	}
	else	// 필드 서버의 변경이 필요할 경우
	{
		// 필드 서버 변경시 클라이언트 정보 변경 사항 반영
		ReActionMoveFieldSvr( pChar->ClientSlot(), pChar->m_dwGaeaID );

		//	이동할 맵 ID 저장.
		pChar->SetNextFieldSvr( sMapID, dwGenGate, D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ), dwNextFieldSvr, pChar->m_nChannel );

		return TRUE;
	}

	return FALSE;
}*/

// *****************************************************
// Desc: 부활요청 처리 ( 같은 필드 )
// *****************************************************

BOOL GLAgentServer::MsgReqReBirth(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	// Checking NULL Pointer;
	if( !nmg )
	{
		sc::writeLogError( "Function GLAgentServer::MsgReqReBirth() : Packet is NULL!" );
		return FALSE;
	}

	GLMSG::NET_CHAR_RESURRECT_FAFC* pMsg =
		static_cast<GLMSG::NET_CHAR_RESURRECT_FAFC*>( nmg );

	GLCharAG* const pChar = GetCharByDbNum( pMsg->dwChaNum );
	if (!pChar)
	{
		sc::writeLogError( "Function GLAgentServer::MsgReqReBirth() : Char is NULL!" );
		return FALSE;
	}

	const MapID& sChangeMapID(pMsg->sMapID);
	if(  sChangeMapID.isValid() == false )
	{
		sc::writeLogError( "Function GLAgentServer::MsgReqReBirth() : ChangeMapID is NULL!" );
		return FALSE;
	}

	// Setting Map ID to character;
	pChar->SetCurrentMap( sChangeMapID );

	// Send To Client;
	SENDTOFIELD( pChar->ClientSlot(), pMsg );
	//SENDTOCLIENT( pMsg, pMsg->dwChaNum );

	return TRUE;
}

// *****************************************************
// Desc: 부활요청 처리
// *****************************************************

BOOL GLAgentServer::MsgReqResurrectMoveMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    /*if (dwGaeaID != GAEAID_NULL)
        return FALSE;

    GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FA* pNetMsg = (GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FA*)nmg;
    GLCharAG* const pChar = GetCharByDbNum(pNetMsg->dwChaNum);
    if (!pChar)
        return FALSE;

    GLAGLandMan* const pLandMan = GetLand(pNetMsg->sMapID);
    SNATIVEID sBirthMapID = pLandMan->GetBaseMapID();
    DWORD dwGenGate = pNetMsg->dwGateID;

    // 랜드가 없는 상황 (일어날수 없는 상황);
    if ( pLandMan == NULL )
    {   
        sc::writeLogError(
            sc::string::format(
            "MsgReqReBirth gate. Map %1%/%2% ChaDbNum %3%",
            pNetMsg->sMapID.Mid(),
            pNetMsg->sMapID.Sid(),
            pChar->ChaDbNum()));
        return FALSE;
    }

    // 게이트가 없는 상황;
    if ( !pLandMan->GetLandGateMan()->FindLandGate(pNetMsg->dwGateID) ) 
    {
        DxLandGate* pLandGate = pLandMan->GetLandGateMan()->GetListLandGate();
        dwGenGate = pLandGate->GetGateID();		//	첫번째 Gate 에서 생김.
    }

    // 해당 맵의 소속 필드번호를 얻어옴;
    DWORD dwNextFieldSvr = GetFieldServer(sBirthMapID);

    // 필드 서버 변경시 클라이언트 정보 변경 사항 반영;
    ReActionMoveFieldSvr ( pChar->ClientSlot(), pChar->GaeaId() );

    // 현재 필드 서버에 자신이 다른 곳에서 부활할 예정임을 통보.;
    GLMSG::NET_CHAR_RESURRECT_MOVEMAP_AF NetMsg(pChar->ChaDbNum(), sBirthMapID, dwGenGate, pNetMsg->fRestore);
    SENDTOFIELD ( pChar->ClientSlot(), &NetMsg );

    //	이동할 맵 ID 저장.;
    pChar->SetNextFieldSvr ( sBirthMapID, dwGenGate, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX), dwNextFieldSvr, pChar->m_nChannel );*/

    return TRUE;
}

BOOL GLAgentServer::MsgReqReGenGate ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_REGEN_GATE_FB *pNetMsg = (GLMSG::SNETPC_REQ_REGEN_GATE_FB *)nmg;

	//GLCharAG* pChar = GetChar ( dwGaeaID );
	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if ( !pChar )
		return FALSE;

	// 변경된 스타트 멥, 스타트 게이트의 정보를 적용.
	//
	pChar->m_sStartMapID = pNetMsg->sMapID;
	pChar->m_dwStartGate = pNetMsg->dwGateID;

	SENDTOCLIENT ( pChar->ClientSlot(), nmg );

	return TRUE;
}

/*
// *****************************************************
// Desc: 필드서버에 접속정보 전송(필드서버가 바뀔경우)
// *****************************************************
//FA
BOOL GLAgentServer::MsgFieldSvrOutFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_FIELDSVR_OUT_FB *msg = (GLMSG::SNETPC_FIELDSVR_OUT_FB *)nmg;

	GLCharAG* pChar = GetCharByDbNum(msg->dwChaNum);
	if (!pChar)
        return FALSE;

	if (pChar->m_sNextMapID == SNATIVEID(MAXLANDMID,MAXLANDSID))
        return FALSE;

	// 현재 필드 서버 및 맵 ID 지정.
	SetCurrentField(pChar, pChar->m_dwNextFieldSvr, pChar->m_sNextMapID);

	// 맵에 새로 진입하기 위해서 Field서버에 메시지 송신.
	NET_GAME_JOIN_FIELDSVR NetJoinField(pChar->GetCountry());
	NetJoinField.emType = EMJOINTYPE_MOVEMAP;
	
	NetJoinField.dwSlotAgentClient = pChar->ClientSlot();
	NetJoinField.SetUserId(pChar->m_szUserName);
	NetJoinField.nUserNum = pChar->m_UserDbNum;
	NetJoinField.dwUserLvl = pChar->m_dwUserLvl;
	NetJoinField.nChaNum = pChar->m_CharDbNum;
	NetJoinField.dwGaeaID = pChar->GaeaId();
	NetJoinField.tPREMIUM = pChar->m_tPREMIUM;
	NetJoinField.tCHATBLOCK = pChar->m_tCHATBLOCK;

	NetJoinField.sStartMap = pChar->m_sNextMapID;
	NetJoinField.dwStartGate = pChar->m_dwNextGateID;
	NetJoinField.vStartPos = pChar->m_vNextPos;

	NetJoinField.dwActState = pChar->m_dwActState;
	NetJoinField.bUseArmSub	= pChar->m_bUseArmSub;

	NetJoinField.emIPBonus	= pChar->m_emIPBonus;

	NetJoinField.sChinaTime			= pChar->m_sChinaTime;
	NetJoinField.sEventTime			= pChar->m_sEventTime;
	NetJoinField.sVietnamGainSystem = pChar->m_sVietnamSystem;

	pChar->ResetNextFieldSvr ();

	SENDTOFIELD (pChar->ClientSlot(), &NetJoinField);

	return TRUE;
}
*/

BOOL GLAgentServer::MsgResurrectMoveMapFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    /*if ( dwGaeaID != GAEAID_NULL )
        return FALSE;

    GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FB *pNetMsg = (GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FB*)nmg;

    GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
    if ( !pChar )
        return FALSE;

	const MapID& _mapIDNext(pChar->GetNextMap());
    if ( _mapIDNext.isValid() == false )
        return FALSE;

    // 현재 필드 서버 및 맵 ID 지정.;
    SetCurrentField(pChar, pChar->m_dwNextFieldSvr, _mapIDNext);

    // 맵에 새로 진입하기 위해서 Field서버에 메시지 송신;
    NET_GAME_JOIN_FIELDSVR NetJoinField(pChar->GetCountry());
    NetJoinField.emType = EMJOINTYPE_REBIRTH;

    NetJoinField.dwSlotAgentClient = pChar->ClientSlot();
    NetJoinField.nUserNum = pChar->m_UserDbNum;
    NetJoinField.SetUserId(pChar->m_szUserName);
    NetJoinField.dwUserLvl = pChar->m_dwUserLvl;
    NetJoinField.nChaNum = pChar->m_CharDbNum;
    NetJoinField.dwGaeaID = pChar->GaeaId();
    NetJoinField.PartyID = pChar->GetPartyID();
    NetJoinField.tPREMIUM = pChar->m_tPREMIUM;
    NetJoinField.tCHATBLOCK = pChar->m_tCHATBLOCK;

    NetJoinField.sStartMap = _mapIDNext;
    NetJoinField.dwStartGate = pChar->m_dwNextGateID;
    NetJoinField.vStartPos = D3DXVECTOR3(0,0,0);

    NetJoinField.dwActState = pChar->m_dwActState;
    NetJoinField.bUseArmSub	= pChar->m_bUseArmSub;

    NetJoinField.emIPBonus	= pChar->m_emIPBonus;

    NetJoinField.sChinaTime			= pChar->m_sChinaTime;
    NetJoinField.sEventTime			= pChar->m_sEventTime;	
    NetJoinField.sVietnamGainSystem = pChar->m_sVietnamSystem;

	NetJoinField.dwCountryID		= pChar->GetCountryInfo().dwID;

    NetJoinField.bChangeField = true; // MsgResurrectMoveMapFB은 다른 필드에서의 부활이므로, 이 값은 항상 true;

	if ( pChar->GetCryptKey().guid != GUID_NULL )
	{
		NetJoinField.ckey = pChar->GetCryptKey();
	}

	if ( pChar->GetCryptKey().guid != GUID_NULL )
	{
		NetJoinField.ckey = pChar->GetCryptKey();
	}

	pChar->ResetNextFieldSvr ();

    SENDTOFIELD(pChar->ClientSlot(), &NetJoinField);*/

    return TRUE;
}

// *****************************************************
// Desc: 레벨 업시 현재 맵에 머물러 있지 않아야 하는 경우
// *****************************************************
//FA
BOOL GLAgentServer::MsgLvUpMustLeaveMap( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_AG *pNetMsg = (GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_AG *)nmg;

	//GLCharAG* pChar = GetChar ( dwGaeaID );
	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if ( !pChar )
		return FALSE;

	const MapID& _mapID(pNetMsg->sMAPID);
	const SNATIVEID& _baseMapID(_mapID.getBaseMapID());
	const SNATIVEID& _gaeaMapID(_mapID.getGaeaMapID());
	// 목표 맵.
	//
	GLAGLandMan* pNextLandMan(GetLand(_gaeaMapID));
	if ( !pNextLandMan )
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_FB NetMsgFB;
		NetMsgFB.emFB = EMREQ_MUST_LEAVE_MAP_FB_FAIL;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 이동 위치 정보 유효성 검사.
	//
	if ( pNetMsg->dwGATEID==UINT_MAX && pNetMsg->vPOS==D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) )
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_FB NetMsgFB;
		NetMsgFB.emFB = EMREQ_MUST_LEAVE_MAP_FB_FAIL;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	// 맵이 있는지
	SMAPNODE* pMapNode = m_sMapList.FindMapNode ( _baseMapID );
	if ( !pMapNode )
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_FB NetMsgFB;
		NetMsgFB.emFB = EMREQ_MUST_LEAVE_MAP_FB_FAIL;
		SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;
	}

	DWORD dwNextFieldSvr = pMapNode->GetFieldServerNum();

	MoveSystem::SMOVE_INFO_AGENT sTempMoveInfo;
	sTempMoveInfo.dwChaDBNum = pChar->ChaDbNum();
	sTempMoveInfo.sMapID = _mapID;
	sTempMoveInfo.dwGateID = pNetMsg->dwGATEID;
	sTempMoveInfo.vPosition = pNetMsg->vPOS;
	sTempMoveInfo.dwFieldNum = dwNextFieldSvr;
	sTempMoveInfo.dwChannelNum = pChar->GetChannel();

	if ( false == MoveSystem::CMoveManagerServer::Instance()->MoveMap( sTempMoveInfo ) )
	{
		return FALSE;
	}

	return TRUE;

	/*// 필드서버를 옮겨야 할 경우
	{
		// 필드서버가 정상인지 검사.
		//
		//BOOL bSVR_OK = m_pMsgServer->IsSlotOnline ( dwNextFieldSvr );
		bool bSVR_OK = IsFieldOnline(pChar->m_nChannel, dwNextFieldSvr);
		if (!bSVR_OK)
		{
			GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_FB NetMsgFB;
			NetMsgFB.emFB = EMREQ_MUST_LEAVE_MAP_FB_FAIL;
			SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgFB );
			return FALSE;
		}

		// 필드 서버 변경시 클라이언트 정보 변경 사항 반영.
		//
		ReActionMoveFieldSvr ( pChar->ClientSlot(), dwGaeaID );

		//	현재 필드 서버에 자신이 떠나갈 예정임을 통보.
		GLMSG::SNETPC_FIELDSVR_OUT NetMsg;
		NetMsg.dwChaNum = pChar->ChaDbNum();
		NetMsg.dwSvrGroupNum = m_pMsgServer->GetServerGroup();
		NetMsg.targetMapID = _mapID;
		SENDTOFIELD ( pChar->ClientSlot(), &NetMsg );

		//	이동할 맵 ID 저장.
		pChar->SetNextFieldSvr ( _mapID, pNetMsg->dwGATEID, pNetMsg->vPOS, dwNextFieldSvr );

	}

	return TRUE;*/
}


// *****************************************************
// Desc: 소환요청 처리 (다른 필드서버일 경우)
// *****************************************************
//FA
BOOL GLAgentServer::MsgReCallAg ( NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( false == MoveSystem::CMoveManagerServer::Instance()->OnOutServer(
		static_cast< GLMSG::SNETPC_REQ_MOVEMAP_FA* >( pMsg ),
		dwGaeaID ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GLAgentServer::MsgTeleportAg ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	return TRUE;
}

//FAC
BOOL GLAgentServer::MsgReqGateOutFB ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETREQ_GATEOUT_FB *pNetMsg = (GLMSG::SNETREQ_GATEOUT_FB *) nmg;

	if (!pNetMsg)
		return FALSE;

	// 클라이언트에 전송한다.
	SENDTOCLIENT(nmg, pNetMsg->dwChaNum);

	return TRUE;
}

//FA
BOOL GLAgentServer::MsgReqCreateInstantMapFB ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( dwGaeaID != GAEAID_NULL )
	{
		sc::writeLogError(std::string("GLAgentServer::MsgReqCreateInstantMapFB(). dwGaeaID != GAEAID_NULL"));
		return FALSE;
	}

	GLMSG::SNET_CREATE_INSTANT_MAP_FB *pNetMsg = (GLMSG::SNET_CREATE_INSTANT_MAP_FB *) nmg;
	
	const MapID& _mapID(pNetMsg->mapID);
	const SNATIVEID& _gaeaMapID(_mapID.getGaeaMapID());
	const SNATIVEID& _baseMapID(_mapID.getBaseMapID());
    if (pNetMsg->emFB == EMCHAR_CREATE_INSTANT_MAP_OK )
    {
        // Agent 서버에서 갖고 있는 케릭터 맵정보 변경
        GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
		if ( pChar )
		{
			SetCurrentField(pChar, pChar->m_dwCurFieldSvr, pNetMsg->mapID);
		}
    }

	// 클라이언트에 전송한다.
	SENDTOCLIENT(nmg, pNetMsg->dwChaNum);
	//SENDTOCLIENT ( dwClientID, nmg );

	// 실패할 경우 인던을 삭제한다.
	if( pNetMsg->emFB == EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATE || pNetMsg->emFB == EMCHAR_CREATE_INSTANT_MAP_FAIL_CREATEMAX )
	{
		sc::writeLogDebug(sc::string::format( "Create Instant Map Failed. MapInfo: gaeaID[%1%/%2%], baseID[%3%/%4%];", _gaeaMapID.Mid(), _gaeaMapID.Sid(), _baseMapID.Mid(), _baseMapID.Sid()) );

		// 신규인던 정보가 아니다, 그렇기 때문에 바로 삭제한다;
        DeleteInstantMap(_gaeaMapID.Mid()); 
	}

	return TRUE;
}



BOOL GLAgentServer::MsgReqConfront ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_CONFRONT *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT *) nmg;
    GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, GAEAID_NULL, EMCONFRONT_FAIL, 0.0f);

	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;

	GLAGLandMan* pLandMan(GetLand(pMyChar->GetCurrentMap().getGaeaMapID()));
	if ( !pLandMan )
	{
		// 일반 오류.
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if (pLandMan->IsClubBattleMap())
	{
		// 일반 오류.
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if (pLandMan->IsClubDeathMatchMap())
	{
		// 일반 오류.
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	const DWORD dwTargetGaeaID = pNetMsg->dwID;
	GLCharAG* pTarChar = GetChar ( dwTargetGaeaID );
	if ( !pTarChar )
	{
		// 일반 오류.
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( !pNetMsg->sOption.VALID_OPT() )
	{
		// 일반 오류.
		NetMsgFB.emFB = EMCONFRONT_OPTION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pLandMan->IsPeaceZone() )
	{
		// 평화 지역에서는 대련 불가.
		NetMsgFB.emFB = EMCONFRONT_PEACE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

    if ( pTarChar->m_bCTFPlayer )
    {
        //! CTF 참여중이면 취소
        NetMsgFB.emFB = EMCONFRONT_FAIL;
        SENDTOCLIENT ( dwClientID, &NetMsgFB );
        return FALSE;
    }

    const MapID& _mapID(pMyChar->GetCurrentMap());
    const SNATIVEID& _baseMapID(_mapID.getBaseMapID());
    const SNATIVEID& _gaeaMapID(_mapID.getGaeaMapID());
	GLAGLandManager* const pLandManager(GetLandManager(_gaeaMapID));
	if ( pLandManager == NULL )
	{
		// 존재 하지 않는 지역;
		NetMsgFB.emFB = EMCONFRONT_NONCONFRONTMODE;
		SENDTOCLIENT( dwClientID, &NetMsgFB );
		return FALSE;
	}
	
	const SMAPNODE *pMapNode(m_sMapList.FindMapNode( _baseMapID ));
	if ( pMapNode && !pMapNode->IsSparring() )	
	{
		// 대련 불가능 맵
		NetMsgFB.emFB = EMCONFRONT_NONCONFRONTMODE;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}


	SNONCONFRONTLIST* pNonConfront = m_cAGPeriod.GetNonConfrontList();
	if ( pNonConfront )
	{
		if ( pNonConfront->IsNonConfront(pMyChar->GetCurrentMap().getGaeaMapID(), pMyChar->m_nChannel))
		{
			// 대련 불가능 모드가 켜져있음
			NetMsgFB.emFB = EMCONFRONT_NONCONFRONTMODE;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

	const GLPartyID& MyPartyID = pMyChar->GetPartyID();
	if ( MyPartyID.isValidParty() )
	{
		const GLPartyAgent* const pPARTY = GetParty(MyPartyID);
		if ( pPARTY && pPARTY->isPartyMember(dwTargetGaeaID) == true )
		{
			// 같은 파티 멤버 끼리는 대련 불가.
			NetMsgFB.emFB = EMCONFRONT_PARTY;
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

	if ( MyPartyID.isValidExpedition() || pTarChar->GetPartyID().isValidExpedition() )
	{
		NetMsgFB.emFB = EMCONFRONT_EXPEDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	const SCONFTING &sMYCONFTING = pMyChar->GETCONFRONTING();
	if ( sMYCONFTING.IsCONFRONTING() )
	{
		// (자신이) 이미 대련 중입니다.
		NetMsgFB.emFB = EMCONFRONT_ALREADY_ME;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	const SCONFTING &sTARCONFTING = pTarChar->GETCONFRONTING ();
	if ( sTARCONFTING.IsCONFRONTING() )
	{
		// (상대방이) 이미 대련 중입니다.
		NetMsgFB.emFB = EMCONFRONT_ALREADY_TAR;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}
	
	switch ( pNetMsg->emTYPE )
	{
	case EMCONFT_ONE:
		{
			// 대련 정보는 "대련" 종료시 양자 모두 로그를 남기므로 GaeaID 하나만 확인하면 중복 대련 체크가 가능.
			//
			DWORD dwCHARID = pMyChar->m_CharDbNum;
			const SCONFT_LOG* pCONFT = pTarChar->GETCONFRONT_LOG ( dwCHARID );
			if ( pCONFT )
			{
				// 일정 시간안에 지정횟수 이상 대련 방지.
				if ( pCONFT->dwCOUNT >= GLCONST_CHAR::dwCONFRONT_MAX )
				{
					NetMsgFB.emFB = EMCONFRONT_MAX;
					NetMsgFB.fTIME = GLCONST_CHAR::fCONFRONT_RESET - pCONFT->fTIMER;	//	대련 가능에 남은시간 : <연속대련방지>리셋시간 - 경과시간.
					SENDTOCLIENT ( dwClientID, &NetMsgFB );
					return FALSE;
				}

				// 종전 대련후 일정시간 경과 이전에 대련 방지.
				if ( pCONFT->fTIMER < GLCONST_CHAR::fCONFRONT_ELAP )
				{
					NetMsgFB.emFB = EMCONFRONT_ELAP;
					NetMsgFB.fTIME = GLCONST_CHAR::fCONFRONT_ELAP - pCONFT->fTIMER;	//	대련 가능에 남은시간 : <연속대련방지>리셋시간 - 경과시간.
					SENDTOCLIENT ( dwClientID, &NetMsgFB );
					return FALSE;
				}
			}

			// 대련 오청한 정보 저장.
			//
			pMyChar->m_sREQCONFTING.RESET();
			pMyChar->m_sREQCONFTING.emTYPE = EMCONFT_ONE;
			pMyChar->m_sREQCONFTING.dwTAR_ID = pTarChar->GaeaId();
			pMyChar->m_sREQCONFTING.sOption = pNetMsg->sOption;

			// 대련 (상대방에게) 요청.
			//
			GLMSG::SNETPC_REQ_CONFRONT_TAR NetMsgTAR(EMCONFT_ONE, dwGaeaID, pTarChar->GaeaId(), pMyChar->GetChaName(), pNetMsg->sOption);
			SENDTOFIELD ( pTarChar->m_ClientSlot, &NetMsgTAR );
			//SENDTOCLIENT ( pTarChar->m_ClientSlot, &NetMsgTAR );


			/*if( pMyChar->m_bTracingUser )
			{
				CDebugSet::ToTracingFile( pMyChar->m_szUserName, "Confront1 Request, [%s][%s], Target Char: [%s][%s], in Land(%d/%d, %d/%d);",
					pMyChar->m_szUserName, pMyChar->m_szName, pTarChar->m_szUserName, pTarChar->m_szName, _baseMapID.Mid(), _baseMapID.Sid(), _gaeaMapID.Mid(), _gaeaMapID.Sid() );
			}*/
		}
		break;

	case EMCONFT_PARTY:
		{
			const GLPartyID& MyPartyID = pMyChar->GetPartyID();
			const GLPartyID& TarPartyID = pTarChar->GetPartyID();
		
			if ( MyPartyID.isValidParty() == false )
			{
				// (자신이) 파티를 결성하지 않았습니다.
				NetMsgFB.emFB = EMCONFRONT_MENOTPARTY;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			const GLPartyAgent* const pMyParty = GetParty(MyPartyID);
			if ( NULL==pMyParty )
			{
				// (자신이) 파티를 결성하지 않았습니다.
				NetMsgFB.emFB = EMCONFRONT_MENOTPARTY;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			if ( false==pMyParty->isMaster(pMyChar->GaeaId()) )
			{
				// (자신이) 파티 마스터가 아닙니다.
				NetMsgFB.emFB = EMCONFRONT_MENOTMASTER;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			if ( TarPartyID.isValidParty() == false )
			{
				// (상대방이) 파티를 결성하지 않았습니다.
				NetMsgFB.emFB = EMCONFRONT_TARMASTER;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			const GLPartyAgent* const pTarParty = GetParty(TarPartyID);
			if ( NULL==pTarParty )
			{
				// (상대방이) 파티를 결성하지 않았습니다.
				NetMsgFB.emFB = EMCONFRONT_TARMASTER;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			if ( false==pTarParty->isMaster(pTarChar->GaeaId()) )
			{
				// (상대방이) 파티 마스터가 아닙니다.
				NetMsgFB.emFB = EMCONFRONT_TARMASTER;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			// 대련 오청한 정보 저장.
			//
			pMyChar->m_sREQCONFTING.RESET();
			pMyChar->m_sREQCONFTING.emTYPE = EMCONFT_PARTY;
			pMyChar->m_sREQCONFTING.dwTAR_ID = pTarChar->GaeaId();
			pMyChar->m_sREQCONFTING.sOption = pNetMsg->sOption;

			// 대련 (상대방에게) 요청.
			//
			GLMSG::SNETPC_REQ_CONFRONT_TAR NetMsgTAR(EMCONFT_PARTY, dwGaeaID, pTarChar->GaeaId(), pMyChar->GetChaName(), pNetMsg->sOption);
			SENDTOFIELD ( pTarChar->m_ClientSlot, &NetMsgTAR );
			//SENDTOCLIENT ( pTarChar->m_ClientSlot, &NetMsgTAR );

//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
			// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
			//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
			//{
				/*if( pMyChar->m_bTracingUser )
				{
					CDebugSet::ToTracingFile( pMyChar->m_szUserName, "Confront2 Request, [%s][%s], Target Char: [%s][%s], in Land(%d/%d, %d/%d);",
											  pMyChar->m_szUserName, pMyChar->m_szName, pTarChar->m_szUserName, pTarChar->m_szName, _baseMapID.Mid(), _baseMapID.Sid(), _gaeaMapID.Mid(), _gaeaMapID.Sid() );
				}*/
//#endif
			//}
		}
		break;

	case EMCONFT_GUILD:
		{
			DWORD dwMyClubID = pMyChar->m_ClubDbNum;
			DWORD dwTarClubID = pTarChar->m_ClubDbNum;

			if ( dwTarClubID==CLUB_NULL )
			{
				// 클럽 마스터가 아닙니다.
				NetMsgFB.emFB = EMCONFRONT_FAIL;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			std::tr1::shared_ptr<GLClubAgent> pTarClub = m_pClubMan->GetClub ( dwTarClubID );
			if ( !pTarClub || pTarClub->MasterCharDbNum()!=pTarChar->m_CharDbNum )
			{
				// 클럽 마스터가 아닙니다.
				NetMsgFB.emFB = EMCONFRONT_FAIL;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			if ( dwMyClubID==CLUB_NULL )
			{
				// 클럽 마스터가 아닙니다.
				NetMsgFB.emFB = EMCONFRONT_FAIL;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			std::tr1::shared_ptr<GLClubAgent> pMyClub = m_pClubMan->GetClub ( dwMyClubID );
			if ( !pMyClub || pMyClub->MasterCharDbNum()!=pMyChar->m_CharDbNum )
			{
				// 클럽 마스터가 아닙니다.
				NetMsgFB.emFB = EMCONFRONT_FAIL;
				SENDTOCLIENT ( dwClientID, &NetMsgFB );
				return FALSE;
			}

			// 대련 오청한 정보 저장.
			//
			pMyChar->m_sREQCONFTING.RESET();
			pMyChar->m_sREQCONFTING.emTYPE = EMCONFT_GUILD;
			pMyChar->m_sREQCONFTING.dwTAR_ID = pTarChar->GaeaId();
			pMyChar->m_sREQCONFTING.sOption = pNetMsg->sOption;

			// 대련 (상대방에게) 요청.
			//
			GLMSG::SNETPC_REQ_CONFRONT_TAR NetMsgTAR(EMCONFT_GUILD, dwGaeaID, pTarChar->GaeaId(), pMyChar->GetChaName(), pNetMsg->sOption);
			SENDTOFIELD ( pTarChar->m_ClientSlot, &NetMsgTAR );
			//SENDTOCLIENT ( pTarChar->m_ClientSlot, &NetMsgTAR );

//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
			// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
			//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
			//{
				/*if( pMyChar->m_bTracingUser )
				{
					CDebugSet::ToTracingFile( pMyChar->m_szUserName, "Confront3 Request, [%s][%s], Target Char: [%s][%s], in Land(%d/%d, %d/%d);",
											  pMyChar->m_szUserName, pMyChar->m_szName, pTarChar->m_szUserName, pTarChar->m_szName, _baseMapID.Mid(), _baseMapID.Sid(), _gaeaMapID.Mid(), _gaeaMapID.Sid() );
				}*/
//#endif
			//}
		}
		break;
	};

	return TRUE;
}

BOOL GLAgentServer::MsgAnsConfront ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_CONFRONT_ANS *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_ANS *) nmg;

	// Absolute Non-PK State Check. 대련 안됨.
	if ( GLCONST_CHAR::bPKLESS )
		return FALSE;

	if ( pNetMsg->emFB == EMCONFRONT_REFUSE )
	{
		GLCharAG* pReqChar = GetChar ( pNetMsg->dwID );
		if ( !pReqChar )
			return FALSE;

		GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(pNetMsg->emTYPE, dwGaeaID, EMCONFRONT_REFUSE, 0.0f);
		SENDTOCLIENT ( pReqChar->m_ClientSlot, &NetMsgFB );
	}
	else if ( pNetMsg->emFB == EMCONFRONT_AGREE )
	{
		//	대련 기본 정보.
		const EMCONFT_TYPE emTYPE = pNetMsg->emTYPE;
		const DWORD dwMY_ID = dwGaeaID;
		const DWORD dwREQ_ID = pNetMsg->dwID;
		const SCONFT_OPTION _sOption = pNetMsg->sOption;

		//	대련 상대방 ID 들.
		const DWORD dwMY_TAR = dwREQ_ID;
		const DWORD dwREQ_TAR = dwMY_ID;

		GLCharAG* pMyChar = GetChar ( dwMY_ID );
		if ( !pMyChar )
			return FALSE;

		const SNATIVEID& _gaeaMapID(pMyChar->GetCurrentMap().getGaeaMapID());
		GLAGLandMan* pLandMan = GetLand(_gaeaMapID);
		if ( !pLandMan )
			return FALSE;

		SNONCONFRONTLIST* pNonConfront = m_cAGPeriod.GetNonConfrontList();
		if ( pNonConfront )
		{
			if ( pNonConfront->IsNonConfront(_gaeaMapID, pMyChar->m_nChannel))
			{
				// 대련 불가능 모드가 켜져있음
				GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(EMCONFT_ONE, GAEAID_NULL, EMCONFRONT_NONCONFRONTMODE, 0.0f);;
				SENDTOCLIENT ( pMyChar->m_ClientSlot, &NetMsgFB );
				return FALSE;
			}
		}

		GLCharAG* pReqChar = GetChar ( dwREQ_ID );
		if ( !pReqChar )
			return FALSE;

		if ( pMyChar->GetPartyID().isValidExpedition() || pReqChar->GetPartyID().isValidExpedition() )
		{
			// 원정대 중엔 대련 불가능;
			//
			GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(emTYPE, dwGaeaID, EMCONFRONT_EXPEDITION, 0.0f);

			SENDTOCLIENT ( pMyChar->m_ClientSlot, &NetMsgFB );
			SENDTOCLIENT ( pReqChar->m_ClientSlot, &NetMsgFB );
		}

		const SCONFTING& sMY_CONFTING = pMyChar->GETCONFRONTING();
		const SCONFTING& sREQ_CONFTING = pReqChar->GETCONFRONTING();

		if ( sMY_CONFTING.IsCONFRONTING() || sREQ_CONFTING.IsCONFRONTING() )
			return FALSE;

		// 대련 조건 무결성 검사.
		//
		if ( emTYPE != pReqChar->m_sREQCONFTING.emTYPE )
			return FALSE;
		if ( dwREQ_TAR != pReqChar->m_sREQCONFTING.dwTAR_ID )
			return FALSE;
		if ( _sOption != pReqChar->m_sREQCONFTING.sOption )
			return FALSE;

		/*if( pMyChar->m_bTracingUser || pReqChar->m_bTracingUser )
		{
			CDebugSet::ToTracingFile( pMyChar->m_szUserName, "Confront Answer OK, [%s][%s], Request Char: [%s][%s], in Land(%d/%d);",
				pMyChar->m_szUserName, pMyChar->m_szName, pReqChar->m_szUserName, pReqChar->m_szName, _gaeaMapID.Mid(), _gaeaMapID.Sid());
		}*/

		// 대련 시작.
		//
		switch ( emTYPE )
		{
		case EMCONFT_ONE:
			{
				SCONFT_OPTION sREQOption = _sOption;		//	A ( 요청자 )
				SCONFT_OPTION sMYOption = _sOption;			//	B ( 수락자 )
				sMYOption.fHP_RATE = _sOption.fTAR_HP_RATE;	//	( 수락자 ) 는 체력 증가율 다르개 가짐.

				pMyChar->StartConfront ( emTYPE, dwMY_TAR, sMYOption );
				pReqChar->StartConfront ( emTYPE, dwREQ_TAR, sREQOption );
			}
			break;

		case EMCONFT_PARTY:
			{
				bool bmyparty(false), breqparty(false);
				const GLPartyID& MyPartyID = pMyChar->GetPartyID();
				const GLPartyID& ReqPartyID = pReqChar->GetPartyID();
			
				if ( MyPartyID.isValidParty() )
				{
					const GLPartyAgent* const pMyParty = GetParty(MyPartyID);
					if ( pMyParty && pMyParty->isMaster(pMyChar->GaeaId()) )
					{
						bmyparty = true;
					}
				}
				
				if ( ReqPartyID.isValidParty() )
				{
					const GLPartyAgent* const pReqParty = GetParty(ReqPartyID);
					if ( pReqParty && pReqParty->isMaster(pReqChar->GaeaId()) )
					{
						breqparty = true;
					}
				}				

				// 파티 유효할때 대련 시작됨;
				//
				if ( bmyparty && breqparty )
				{
					// 대련시작전 참여 멤버 체크;
					//
					GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_FLD NetMsgCheck(pReqChar->GetCurrentMap().getGaeaMapID(), ReqPartyID, MyPartyID, _sOption);
					SENDTOFIELDSVR ( pMyChar->m_nChannel, pMyChar->m_dwCurFieldSvr, &NetMsgCheck );
				}
				else
				{
					// 파티가 아니여서 대련 불가능.
					//
					GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(emTYPE, dwGaeaID, EMCONFRONT_FAIL, 0.0f);
					
					SENDTOCLIENT ( pMyChar->m_ClientSlot, &NetMsgFB );
					SENDTOCLIENT ( pReqChar->m_ClientSlot, &NetMsgFB );
				}
			}
			break;

		case EMCONFT_GUILD:
			{
				bool bmyclub(false), breqclub(false);
				DWORD dwMyClubID = pMyChar->m_ClubDbNum;
				DWORD dwReqClubID = pReqChar->m_ClubDbNum;
			
				if ( dwMyClubID!=CLUB_NULL )
				{
					std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( dwMyClubID );
					if ( pCLUB && pCLUB->MasterCharDbNum()==pMyChar->m_CharDbNum )
					{
						bmyclub = true;
					}
				}
				
				if ( dwReqClubID!=GAEAID_NULL )
				{
					std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( dwReqClubID );
					if ( pCLUB && pCLUB->MasterCharDbNum()==pReqChar->m_CharDbNum )
					{
						breqclub = true;
					}
				}

				// 클럽 유효할때 대련 시작됨.
				//
				if ( bmyclub && breqclub )
				{
					// 대련시작전 참여 멤버 체크.
					//
					GLMSG::SNET_CONFRONTCLB_CHECKMBR_FLD NetMsgCheck(pReqChar->GetCurrentMap().getGaeaMapID(), dwReqClubID, dwMyClubID, _sOption);
					SENDTOFIELDSVR ( pMyChar->m_nChannel, pMyChar->m_dwCurFieldSvr, &NetMsgCheck );
				}
				else
				{
					// 클럽이 아니여서 대련 불가능.
					//
					GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB(emTYPE, dwGaeaID, EMCONFRONT_FAIL, 0.0f);
					
					SENDTOCLIENT ( pMyChar->m_ClientSlot, &NetMsgFB );
					SENDTOCLIENT ( pReqChar->m_ClientSlot, &NetMsgFB );
				}
			}
			break;
		};
	}

	return TRUE;
}

BOOL GLAgentServer::MsgConfrontPtyCheckMber ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_AGT *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_CHECKMBR2_AGT *) nmg;
	
	StartPartyConfront ( pNetMsg->PartyID_A, pNetMsg->PartyID_B, pNetMsg->sMapID, pNetMsg->sOption,
		pNetMsg->dwPARTY_A_MEMBER, pNetMsg->dwPARTY_B_MEMBER, pNetMsg->vPosition );

	return TRUE;
}

BOOL GLAgentServer::MsgConfrontClbCheckMber ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CONFRONTCLB_CHECKMBR_AGT *pNetMsg = (GLMSG::SNET_CONFRONTCLB_CHECKMBR_AGT *) nmg;
	
	std::tr1::shared_ptr<GLClubAgent> pClubA = m_pClubMan->GetClub(pNetMsg->dwCLUB_A);
	std::tr1::shared_ptr<GLClubAgent> pClubB = m_pClubMan->GetClub(pNetMsg->dwCLUB_B);
	if ( !pClubA || !pClubB )
        return FALSE;

	CLUBCONFT_ITER pos, end;

	SCONFT_OPTION sOPT_A = pNetMsg->sOption;
	SCONFT_OPTION sOPT_B = pNetMsg->sOption;

	DWORD i(0);

	// A 팀 '클럽대련' 초기화.
	//
	pClubA->CLEARCONFT();

	for ( i=0; i<pNetMsg->dwCLUB_A_NUM; ++i )
	{
		DWORD dwMBR = pNetMsg->dwCLUB_A_MEMBER[i];

		GLCharAG* pChar = GetCharByDbNum ( dwMBR );
		if (!pChar)
            continue;
		if (pChar->GetCurrentMap() != pNetMsg->sMapID)
            continue;

		//	Note :클럽원의 대련정보 설정.
		//
		pChar->m_sCONFTING.RESET();
		pChar->m_sCONFTING.emTYPE = EMCONFT_GUILD;
		pChar->m_sCONFTING.dwTAR_ID = pNetMsg->dwCLUB_B;
		pChar->m_sCONFTING.sOption = sOPT_A;

		pClubA->ADDCONFT_MEMBER ( dwMBR );
	}

	if ( pClubA->GETCONFT_NUM()==0 )
		return FALSE;
	
	// B 팀 '클럽대련' 초기화.
	//
	pClubB->CLEARCONFT();

	for ( i=0; i<pNetMsg->dwCLUB_B_NUM; ++i )
	{
		DWORD dwMBR = pNetMsg->dwCLUB_B_MEMBER[i];

		GLCharAG* pChar = GetCharByDbNum ( dwMBR );
		if (!pChar)
            continue;
		if (pChar->GetCurrentMap() != pNetMsg->sMapID)
            continue;

		// 클럽원의 대련정보 설정.
		//
		pChar->m_sCONFTING.RESET();
		pChar->m_sCONFTING.emTYPE = EMCONFT_GUILD;
		pChar->m_sCONFTING.dwTAR_ID = pNetMsg->dwCLUB_A;
		pChar->m_sCONFTING.sOption = sOPT_B;

		pClubB->ADDCONFT_MEMBER ( dwMBR );
	}

	if ( pClubB->GETCONFT_NUM()==0 )
		return FALSE;

	// 클럽대련 정보 초기화.
	//
	pClubA->m_conftOPT = sOPT_A;
	pClubA->m_dwconftCLUB = pNetMsg->dwCLUB_B;
	pClubA->m_fconftTIMER = 0.0f;
	pClubA->SETCONFT_MEMBER();

	pClubB->m_conftOPT = sOPT_B;
	pClubB->m_dwconftCLUB = pNetMsg->dwCLUB_A;
	pClubB->m_fconftTIMER = 0.0f;
	pClubB->SETCONFT_MEMBER();

	// 필드에 대련 초기화 메시지.
	//		메시지에 전송된 my, tar school 은 사용되지 않는다.
	GLMSG::SNETPC_CONFRONTCLB_START2_FLD NetMsgFld( pNetMsg->sMapID, pNetMsg->dwCLUB_A, pNetMsg->dwCLUB_B, pNetMsg->vPosition, sOPT_A );

	pos = pClubA->m_setConftCURRENT.begin();
	end = pClubA->m_setConftCURRENT.end();
	for ( ; pos!=end; ++pos )
	{
		NetMsgFld.dwCLUB_A_MEMBER[NetMsgFld.dwCLUB_A_NUM++] = (*pos);
	}

	pos = pClubB->m_setConftCURRENT.begin();
	end = pClubB->m_setConftCURRENT.end();
	for ( ; pos!=end; ++pos )
	{
		NetMsgFld.dwCLUB_B_MEMBER[NetMsgFld.dwCLUB_B_NUM++] = (*pos);
	}

	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgAnsConfrontFB ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_CONFRONT_FB *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_FB *)nmg;

	switch ( pNetMsg->emTYPE )
	{
	case EMCONFT_ONE:
		{
			GLCharAG* pCHAR = GetChar(pNetMsg->dwID);
            if( pCHAR )
            {
                if ( pCHAR->m_sCONFTING.IsCONFRONTING() )
                {
                    GLCharAG* pTAR = GetChar(pCHAR->m_sCONFTING.dwTAR_ID);
                    if ( pTAR )
                    {
                        pTAR->m_sCONFTING.RESET();

                        // Note : 클라이언트에 대련 오류 송신.
                        if( pNetMsg->emFB == EMCONFRONT_FAIL )
                            SENDTOCLIENT(pTAR->m_ClientSlot,pNetMsg);
                    }
                }

                pCHAR->m_sCONFTING.RESET();

                // 클라이언트에 대련 오류 송신.
                SENDTOCLIENT(pCHAR->m_ClientSlot,pNetMsg);
            }
		}
		break;

	case EMCONFT_PARTY:
		{
			m_pPartyManager->ConfrontFB ( (NET_MSG_GENERIC*) pNetMsg );
		}
		break;

	case EMCONFT_GUILD:
		{
			std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( pNetMsg->dwID );
			if ( pCLUB )
			{
				pCLUB->CLEARCONFT();

				CLUBMEMBERS& Members = pCLUB->GetMembers();
                CLUBMEMBERS_ITER pos = Members.begin();
				CLUBMEMBERS_ITER end = Members.end();
				for ( ; pos!=end; ++pos)
				{
					GLCharAG* pCHAR = GetCharByDbNum((*pos).first);
					if (pCHAR)
                    {
					    pCHAR->m_sCONFTING.RESET();
					    // 클라이언트에 대련 오류 송신
					    GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB( EMCONFT_ONE, GAEAID_NULL, pNetMsg->emFB, 0.0f);
					    SENDTOCLIENT(pCHAR->m_ClientSlot,&NetMsgFB);
                    }
				}
			}
		}
		break;
	};

	return TRUE;
}

BOOL GLAgentServer::MsgConfrontEnd2Agt ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_CONFRONT_END2_AGT *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_AGT *)nmg;
	
	//GLCharAG* pMyChar = GetChar ( dwGaeaID );
	GLCharAG* pMyChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pMyChar)
		return FALSE;

	const SCONFTING &sCONFT = pMyChar->GETCONFRONTING();

	switch ( sCONFT.emTYPE )
	{
	case EMCONFT_ONE:
		{
			DWORD dwTAR_ID = sCONFT.dwTAR_ID;
			pMyChar->ResetConfrontOk ();

			GLCharAG* pTarChar = GetChar ( dwTAR_ID );
			if ( !pTarChar )
				return TRUE;
			pTarChar->ResetConfrontOk ();
		}
		break;

	case EMCONFT_PARTY:
		{
			pMyChar->ResetConfrontOk();

			if ( m_pPartyManager->DeleteConfrontMember(pMyChar->GetPartyID(), pMyChar->GaeaId()) == false )
				return FALSE;
		}
		break;

	case EMCONFT_GUILD:
		{
			pMyChar->ResetConfrontOk();

			std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub ( pMyChar->m_ClubDbNum );
			if ( !pCLUB )
                return FALSE;

			pCLUB->DELCONFT_MEMBER ( pMyChar->m_CharDbNum );
		}
		break;
	};

	return TRUE;
}

BOOL GLAgentServer::MsgServerCtrlWeather ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	SENDTOALLCHANNEL ( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgServerCtrlGenHold ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	SENDTOALLCHANNEL ( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgServerCtrlTime ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNETSERVER_CTRL_TIME *pNetMsg = (GLMSG::SNETSERVER_CTRL_TIME *)nmg;
	GLPeriod::GetInstance().SetHour ( pNetMsg->dwTime );

	SENDTOALLCHANNEL ( pNetMsg );

	return TRUE;
}

BOOL GLAgentServer::MsgServerCtrlMonth ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNETSERVER_CTRL_MONTH *pNetMsg = (GLMSG::SNETSERVER_CTRL_MONTH *)nmg;

	//GLPeriod::GetInstance();

	return TRUE;
}

BOOL GLAgentServer::MsgBlockDetected(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return FALSE;
	
	GLMSG::SNET_BLOCK_DETECTED* pPacket = (GLMSG::SNET_BLOCK_DETECTED*) nmg;
	if (pMyChar->m_CharDbNum == pPacket->dwCHARID)
	{
		CDebugSet::ToBlockDetectFile(
            "userid= %s (%d), charid= %s (%d), detect=%d, %s",
			pMyChar->m_szUserName,
			pMyChar->m_UserDbNum,
            pMyChar->m_szName,
            pMyChar->m_CharDbNum,
            pPacket->dwDETECTED,
            pPacket->szINFO);

		AddGameAdoJob(
            db::DbActionPtr(
                new db::LogHackProgram(
                    pMyChar->m_UserDbNum,
                    pMyChar->m_CharDbNum,
                    (int) pPacket->dwDETECTED,
                    pPacket->szINFO)));
	}

	sc::writeLogError(sc::string::format("userid= %1% (%2%), charid= %3% (%4%), detect=%5%, %6%", pMyChar->m_szUserName, pMyChar->m_UserDbNum, pMyChar->m_szName, pMyChar->m_CharDbNum, pPacket->dwDETECTED, pPacket->szINFO ));
	return TRUE;
}

// *****************************************************
// Desc: 케릭터 강제 접속 종료 요청 처리
// *****************************************************
BOOL GLAgentServer::MsgGmKickUser ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_KICK_USER *pNetMsg = (GLMSG::SNET_GM_KICK_USER*) nmg;

	GLCharAG* pTarChar = NULL;

	if( pNetMsg->dwID != 0 )
		pTarChar = GetCharByDbNum ( pNetMsg->dwID );
	else
		pTarChar = GetChar ( pNetMsg->szCHARNAME );

	if ( !pTarChar )
		return FALSE;

	GLMSG::SNET_GM_KICK_USER_PROC_FLD NetMsgFld;
	NetMsgFld.dwID = pNetMsg->dwID;
	SENDTOALLCHANNEL ( &NetMsgFld );

	GLMSG::SNET_GM_KICK_USER_PROC NetMsgProc;
	SENDTOCLIENT ( pTarChar->m_ClientSlot, &NetMsgProc );

	ReserveDropOutPC ( pTarChar->GaeaId() );

	return TRUE;
}

BOOL GLAgentServer::MsgGMMove2Gate ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNET_GM_MOVE2GATE* pNetMsg = (GLMSG::SNET_GM_MOVE2GATE*) nmg;

	GLMSG::SNET_GM_MOVE2GATE_FLD NetMsgFld;
	NetMsgFld.dwChaNum = pCHAR->ChaDbNum();
	NetMsgFld.dwGATE   = pNetMsg->dwGATE;
	
	SENDTOALLCHANNEL(&NetMsgFld);

	return TRUE;
}

// *****************************************************
// Desc: 특정맵의 특정위치에 이동 요청 처리
// *****************************************************
BOOL GLAgentServer::MsgGMMove2Pos ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_MOVE2MAPPOS* pNetMsg = (GLMSG::SNET_GM_MOVE2MAPPOS*) nmg;

	GLMSG::SNET_GM_MOVE2MAPPOS_FLD NetMsgFld;
	NetMsgFld.nidMAP   = pNetMsg->nidMAP;
	NetMsgFld.dwGaeaID = dwGaeaID;
	NetMsgFld.dwPOSX   = pNetMsg->dwPOSX;
	NetMsgFld.dwPOSY   = pNetMsg->dwPOSY;
	NetMsgFld.dwChaNum = pCHAR->ChaDbNum();
	
	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgServerGeneralChat(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    if (dwGaeaID == GAEAID_NULL)
    {
	    SENDTOALLCLIENT(nmg);
        return TRUE;
    }
    else
    {
        sc::writeLogError("HACK! Check GLAgentServer::MsgServerGeneralChat GaeaId not null");
        return FALSE;
    }
}

BOOL GLAgentServer::ChatMsgLoudSpeakerAgt ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_CHAT_LOUDSPEAKER_AGT *pNetMsg = (GLMSG::SNETPC_CHAT_LOUDSPEAKER_AGT *) nmg;
	
	//GLCharAG* pChar = GetChar ( dwGaeaID );
	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pChar)
		return FALSE;

	if (pChar->IsCHATBLOCK())
		return FALSE;

	GLMSG::NET_CHAT_LINK_FB NetMsgFB;
	EMNET_MSG_CHAT emType = pChar->m_dwUserLvl >= USER_USER_GM ? CHAT_TYPE_LOUDSPEAKER_GM:CHAT_TYPE_LOUDSPEAKER;
	NetMsgFB.SetData(emType, pChar->GetChaName(), pNetMsg->szMSG);	
	
	for (int i = 0; i < CHAT_LINK_MAX; ++i)
		NetMsgFB.sLinkDataRecv[i] = pNetMsg->sLinkDataRecv[i];

	NetMsgFB.ADD(pNetMsg->LinkData, pNetMsg->dwLinkDataSize);

	if (pNetMsg->bchannel_all)
        SENDTOALLCLIENT(&NetMsgFB);
	else
        SENDTOALLCLIENT(&NetMsgFB, pChar->m_nChannel);
	return TRUE;
}

//CA
BOOL GLAgentServer::MsgGmViewWorkEvent ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if (!pMyChar)
        return FALSE;
	if (pMyChar->m_dwUserLvl < USER_GM3)
        return FALSE;

	GLMSG::SNET_GM_VIEWWORKEVENT_FB NetMsgFB;
	if (m_WorkEvent.bWorkEvent[EVENT_EXP])  // 경험치 
	{
		StringCchPrintf(
            NetMsgFB.szWorkEventInfo,
            GLMSG::SNET_GM_VIEWWORKEVENT_FB::INFO_SIZE,
            "Event Type : Exp, Event Rate : %.2f",
            m_WorkEvent.exp_Event.m_Scale);
		SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

	if (m_WorkEvent.bWorkEvent[EVENT_ITEM])  // 아이템
	{
		StringCchPrintf(
            NetMsgFB.szWorkEventInfo,
            GLMSG::SNET_GM_VIEWWORKEVENT_FB::INFO_SIZE,
            "Event Type : Item, Event Rate : %.2f",
            m_WorkEvent.item_Event.fRATE);
		SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

	if (m_WorkEvent.bWorkEvent[EVENT_MONEY])  // 돈 드랍율
	{
		StringCchPrintf(
            NetMsgFB.szWorkEventInfo,
            GLMSG::SNET_GM_VIEWWORKEVENT_FB::INFO_SIZE,
            "Event Type : Money, Event Rate : %.2f",
            m_WorkEvent.money_Event.fRATE);
		SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

	if (m_WorkEvent.bWorkEvent[EVENT_GRADE])  // 아이템
	{
		StringCchPrintf(
            NetMsgFB.szWorkEventInfo,
            GLMSG::SNET_GM_VIEWWORKEVENT_FB::INFO_SIZE,
            "Event Type : Grade, Event Rate : %.2f",
            m_WorkEvent.grade_Event.fRATE);
		SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

	if (m_WorkEvent.bWorkEvent[EVENT_EXTYPE])  // EX 이벤트
	{
		char szTempChar[32] = {0,};
		if (m_WorkEvent.ex_Event.emType & EMGM_EVENT_SPEED)
			StringCchCat(szTempChar, 32, "SPEED,");
		if (m_WorkEvent.ex_Event.emType & EMGM_EVENT_ASPEED)
			StringCchCat(szTempChar, 32, "ATTACK SPEED,");
		if (m_WorkEvent.ex_Event.emType & EMGM_EVENT_ATTACK)
			StringCchCat(szTempChar, 32, "ATTACK RATE,");
		
		szTempChar[strlen(szTempChar)-1] = '\0';
		StringCchPrintf(
            NetMsgFB.szWorkEventInfo,
            GLMSG::SNET_GM_VIEWWORKEVENT_FB::INFO_SIZE,
            "Event Type : Ex Event[%s] Event Rate : %d",
            szTempChar,
            m_WorkEvent.ex_Event.wValue);
		SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

	if( m_WorkEvent.bWorkEvent[EVENT_LIMIT] )  // 리미트 이벤트 
	{
        std::string Msg(
            sc::string::format(
                "Event Type : LimitEvent, Start_LV %d, End_LV %d, Play_Time %d, Buster_Time %d, Item Rate : %.2f Exp Rate : %.2f, EventMinute %d", 
				m_WorkEvent.limit_Event.start_Lv,
                m_WorkEvent.limit_Event.end_Lv,
                m_WorkEvent.limit_Event.play_Time,
                m_WorkEvent.limit_Event.buster_Time, 
                m_WorkEvent.limit_Event.itemGain_Rate,
				m_WorkEvent.limit_Event.expGain_Rate,                
                m_WorkEvent.limit_Event.dwEventMinute));
		NetMsgFB.SetData(Msg);
        SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

	for (int i = 0; i < 3; i++)
	{
		if (!m_WorkEvent.bWorkEvent[EVENT_CLASS_EXP+i])
            continue;// 클래스별 경험치 이벤트

        std::string TempChar;
        std::string TempChar2;
		if( m_WorkEvent.class_Event[i].dwClassIndex == GLCC_ALL_ACTOR )
		{
			TempChar.append("ALL,");
		}
        else
        {
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_FIGHTER_M )
			{
				TempChar.append("FIGHT_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ARMS_M )	
			{
				TempChar.append("ARMS_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ARCHER_W )	
			{
				TempChar.append("ARCHER_W,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_SPIRIT_W )	
			{
				TempChar.append("SPIRIT_W,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_EXTREME_M )	
			{
				TempChar.append("EXTREME_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_EXTREME_W )	
			{
				TempChar.append("EXTREME_W,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_FIGHTER_W )
			{
				TempChar.append("FIGHTER_W,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ARMS_W )
			{
				TempChar.append("ARMS_W,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ARCHER_M )
			{
				TempChar.append("ARCHER_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_SPIRIT_M )
			{
				TempChar.append("SPIRIT_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_SCIENTIST_M )
			{
				TempChar.append("SCIENTIST_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_SCIENTIST_W )
			{
				TempChar.append("SCIENTIST_W,");
			}
            if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ASSASSIN_M )
            {
                TempChar.append("ASSASSIN_M,");
            }
            if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ASSASSIN_W )
            {
                TempChar.append("ASSASSIN_W,");
            }
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_TRICKER_M )
			{
				TempChar.append("TRICKER_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_TRICKER_W )
			{
				TempChar.append("TRICKER_W,");
			}
            if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ETC_M )
            {
                TempChar.append("ETC_M,");
            }
            if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ETC_W )
            {
                TempChar.append("ETC_W,");
            }
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ACTOR_M )
			{
				TempChar.append("ACTOR_M,");
			}
			if( m_WorkEvent.class_Event[i].dwClassIndex&GLCC_ACTOR_W )
			{
				TempChar.append("ACTOR_W,");
			}
		}        

		if( i == 0 )
			TempChar2 = "Exp";
		else if( i == 1 )
			TempChar2 = "Item";
		else if( i == 2 )
			TempChar2 = "Money";
		
        std::string Msg(
            sc::string::format(
                "Event Type : Class %s Event[%s], Event Rate : %.2f, start_LV %d, end_LV %d",
                TempChar2.c_str(),
                TempChar.c_str(), 
				m_WorkEvent.class_Event[i].fEventRate,
                m_WorkEvent.class_Event[i].start_Lv,
                m_WorkEvent.class_Event[i].end_Lv));
        NetMsgFB.SetData(Msg);
		SENDTOCLIENT(dwClientID, &NetMsgFB);
	}

    if( m_WorkEvent.bWorkEvent[EVENT_IP] ) // IP 이벤트.
    {
        DWORD AttackValue = ran_event::GLIPEvent::Instance()->GetAttackValue();
        DWORD AttackSpeed = ran_event::GLIPEvent::Instance()->GetAttackSpeed();
        DWORD MoveSpeed = ran_event::GLIPEvent::Instance()->GetMoveSpeed();
        DWORD ItemDrop = ran_event::GLIPEvent::Instance()->GetItempDrop();
        DWORD MoneyDrop = ran_event::GLIPEvent::Instance()->GetMoneyDrop();
        DWORD Exp = ran_event::GLIPEvent::Instance()->GetExp();
        DWORD HPRecovery = ran_event::GLIPEvent::Instance()->GetHPRecovery();
        DWORD MPRecovery = ran_event::GLIPEvent::Instance()->GetMPRecovery();
        DWORD SPRecovery = ran_event::GLIPEvent::Instance()->GetSPRecovery();
        DWORD AllRecovery = ran_event::GLIPEvent::Instance()->GetAllRecovery();
        DWORD CriticalRate = ran_event::GLIPEvent::Instance()->GetCriticalRate();

        std::string Msg(
            sc::string::format(
            "Attack: %1%, AttackSpeed: %2%, MoveSpeed: %3%, ItemDrop:%4%, MoneyDrop: %5%, EXP: %6%, HP_rcv: %7%, MP_rcv: %8%, SP_rcv: %9%, All_rcv: %10%, Critical: %11%"
            , AttackValue
            , AttackSpeed
            , MoveSpeed
            , ItemDrop
            , MoneyDrop
            , Exp
            , HPRecovery
            , MPRecovery
            , SPRecovery
            , AllRecovery
            , CriticalRate ));
        NetMsgFB.SetData(Msg);
        SENDTOCLIENT(dwClientID, &NetMsgFB);
    }

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventEx ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_EVENT_EX *pNetMsg = (GLMSG::SNET_GM_EVENT_EX *)nmg;

#ifndef _RELEASED
	if( pNetMsg->wValue < 1 )		pNetMsg->wValue = 1;
	else if( pNetMsg->wValue > 50 )	pNetMsg->wValue = 50;
#endif

	SENDTOALLCHANNEL ( pNetMsg );

	m_WorkEvent.bWorkEvent[EVENT_EXTYPE] = TRUE;
	m_WorkEvent.ex_Event	  = *pNetMsg;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventExEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	SENDTOALLCHANNEL ( nmg );

	m_WorkEvent.bWorkEvent[EVENT_EXTYPE] = FALSE;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventExp( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	
	if (!pMyChar)
		return FALSE;
	
	if (pMyChar->m_dwUserLvl < USER_GM1)
		return FALSE;
	
	GLMSG::NET_GM_EVENT_EXP_CAF *pPacket = (GLMSG::NET_GM_EVENT_EXP_CAF *)nmg;
	DWORD ChaDbNum = pMyChar->ChaDbNum();
    float Scale = pPacket->m_Scale;
	
    EventExp(ChaDbNum, Scale);

	return TRUE;
}

void GLAgentServer::EventExpSA(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_EVENT_EXP_SA* pPacket = (GLMSG::NET_EVENT_EXP_SA*) pMsg;
    sc::writeLogInfo(
        sc::string::format(
            "EventExpSA %1%", pPacket->m_Scale));
    EventExp(0, pPacket->m_Scale);
}

void GLAgentServer::EventExp(DWORD ChaDbNum, float Scale)
{
    GLMSG::NET_GM_EVENT_EXP_CAF MsgAF(ChaDbNum, Scale);

    if (MsgAF.m_Scale < GM_EVENT_EXP_MIN_RATE)
        MsgAF.m_Scale = GM_EVENT_EXP_MIN_RATE;

    if (MsgAF.m_Scale > GM_EVENT_EXP_MAX_RATE)
        MsgAF.m_Scale = GM_EVENT_EXP_MAX_RATE;

    SENDTOALLCHANNEL(&MsgAF);

    m_WorkEvent.bWorkEvent[EVENT_EXP] = TRUE;
    m_WorkEvent.exp_Event = MsgAF;
}

BOOL GLAgentServer::MsgGmEventExpEnd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
		return FALSE;
	if (pMyChar->m_dwUserLvl < USER_GM1)
		return FALSE;

    EventExpEnd();

	return TRUE;
}

void GLAgentServer::EventExpEndSA(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_EVENT_EXP_END_SA* pPacket = (GLMSG::NET_EVENT_EXP_END_SA*) pMsg;
    sc::writeLogInfo("EventExpEndSA");

    EventExpEnd();
}

void GLAgentServer::EventExpEnd()
{
    GLMSG::NET_GM_EVENT_EXP_END_CAF MsgAF;
    SENDTOALLCHANNEL(&MsgAF);

    m_WorkEvent.bWorkEvent[EVENT_EXP] = FALSE;
}

// 이벤트 시작!!
BOOL GLAgentServer::MsgGmLimitEventBegin( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );

	if ( !pMyChar )
	{
		sc::writeLogWarn(std::string("/limit_event_begin - The GM character does not exist."));
		return FALSE;
	}

	if ( pMyChar->m_dwUserLvl < USER_GM1 )
	{
		sc::writeLogWarn(std::string("/limit_event_begin - The GM character level is lower than GM level 1."));
		return FALSE;
	}

	//** Add EventTime
	GLMSG::SNET_GM_LIMIT_EVENT_BEGIN *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_BEGIN *)nmg;

	m_sEventState.bEventStart = TRUE;
	m_sEventState.EventBusterTime  = pNetMsg->buster_Time * 60;
	m_sEventState.EventPlayTime   = pNetMsg->play_Time * 60;

	m_sEventState.fExpGainRate     = pNetMsg->expGain_Rate;
	m_sEventState.fItemGainRate    = pNetMsg->itemGain_Rate;
	m_sEventState.MinEventLevel	   = pNetMsg->start_Lv;
	m_sEventState.MaxEventLevel    = pNetMsg->end_Lv;
	m_sEventState.dwEventEndMinute = pNetMsg->dwEventMinute;
	
	m_sEventState.EventStartTime  = CTime::GetCurrentTime();

	CTime crtTime = CTime::GetCurrentTime();
	//for ( DWORD i=0; i<m_dwMaxClient; ++i )
	//{
	//	GLCharAG* pChar = m_PCArray[i];
	//	if ( pChar )
	//	{
	//		pChar->m_sEventTime.loginTime = crtTime.GetTime();
	//	}
	//}
    for (AGENT_CHAR_DATA_ITER iter=m_CharData.begin(); iter!=m_CharData.end(); ++iter)
    {
        GLCharAG* pChar = iter->second;
        if (pChar)
		{
            pChar->m_sEventTime.loginTime = crtTime.GetTime();
			pChar->m_bReciveEventTime = TRUE;
		}
    }    

	sc::writeLogInfo(std::string("/limit_event_begin - Processed at Agent-side and the command is transfered to FieldServers."));

	SENDTOALLCHANNEL ( pNetMsg );

	m_WorkEvent.bWorkEvent[EVENT_LIMIT] = TRUE;
	m_WorkEvent.limit_Event	  = *pNetMsg;

	return TRUE;
}

// 이벤트 종료!!
BOOL GLAgentServer::MsgGmLimitEventEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	m_sEventState.bEventStart = false;

	GLMSG::SNET_GM_LIMIT_EVENT_END *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_END *)nmg;

	SENDTOALLCHANNEL ( nmg );

	m_WorkEvent.bWorkEvent[EVENT_LIMIT] = FALSE;

	return TRUE;
}


BOOL GLAgentServer::MsgGmLimitEventRestart ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_LIMIT_EVENT_RESTART *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_RESTART *)nmg;

	//GLCharAG* pMyChar = GetChar ( dwGaeaID );
	GLCharAG* pMyChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pMyChar)
		return FALSE;
	
	pMyChar->m_sEventTime.loginTime = pNetMsg->restartTime;

	return TRUE;
}

BOOL GLAgentServer::MsgGmLimitEventTimeReq( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
//	GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ *)nmg;;

	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ_FB NetSendMsg;

	if( m_sEventState.bEventStart == FALSE )
	{
		NetSendMsg.bEventStart = FALSE;
	}else{
		NetSendMsg.bEventStart = TRUE;

		NetSendMsg.buster_Time	= m_sEventState.EventBusterTime;
		NetSendMsg.play_Time	= m_sEventState.EventPlayTime;
		NetSendMsg.start_Lv		= m_sEventState.MinEventLevel;
		NetSendMsg.end_Lv		= m_sEventState.MaxEventLevel;
		NetSendMsg.expGain_Rate	= m_sEventState.fExpGainRate;
		NetSendMsg.itemGain_Rate	= m_sEventState.fItemGainRate;

		// 만약 처음 게임을 시작한다면 로그인 시간을 RESET한다.
		if( pCHAR->m_bReciveEventTime == FALSE )
		{
			CTime	  crtTime      = CTime::GetCurrentTime();

			pCHAR->m_sEventTime.loginTime = crtTime.GetTime();
			NetSendMsg.loginTime   = pCHAR->m_sEventTime.loginTime;

			GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET NetResetMsg;
			NetResetMsg.dwGaeaID  = pCHAR->GaeaId();
			NetResetMsg.loginTime = crtTime.GetTime();
			SENDTOFIELD ( pCHAR->m_ClientSlot, &NetResetMsg );


			pCHAR->m_bReciveEventTime = TRUE;

		}else{
			NetSendMsg.loginTime   = pCHAR->m_sEventTime.loginTime;
		}
	}

	SENDTOCLIENT ( pCHAR->m_ClientSlot, &NetSendMsg );

	return TRUE;
}


BOOL GLAgentServer::MsgCyberCafeClassUpdate( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;

	GLMSG::SNET_CYBERCAFECLASS_UPDATE *pNetMsg = (GLMSG::SNET_CYBERCAFECLASS_UPDATE *)nmg;
	pCHAR->m_emIPBonus = pNetMsg->emIPBonus;

	SENDTOFIELD ( pCHAR->m_ClientSlot, pNetMsg );
	SENDTOCLIENT ( pCHAR->m_ClientSlot, pNetMsg );

	return TRUE;
}


BOOL GLAgentServer::MsgGmEventItemGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_EVEN_ITEM_GEN *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN *)nmg;
	pNetMsg->dwChaNum = pMyChar->ChaDbNum();
	if ( pNetMsg->fRATE > 10.0f )
		pNetMsg->fRATE = 10.0f;

	SENDTOALLCHANNEL ( pNetMsg );

	m_WorkEvent.bWorkEvent[EVENT_ITEM] = TRUE;
	m_WorkEvent.item_Event	  = *pNetMsg;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventItemGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_EVEN_ITEM_GEN_END *msg = (GLMSG::SNET_GM_EVEN_ITEM_GEN_END *)nmg;
	msg->dwChaNum = pMyChar->ChaDbNum();

	SENDTOALLCHANNEL ( nmg );

	m_WorkEvent.bWorkEvent[EVENT_ITEM] = FALSE;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventGrade( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_EVENT_GRADE *pNetMsg = (GLMSG::SNET_GM_EVENT_GRADE *)nmg;
	pNetMsg->dwChaNum = pMyChar->ChaDbNum();

	if ( pNetMsg->fRATE > 90.0f )
		pNetMsg->fRATE = 90.0f;

	if ( pNetMsg->fRATE < 0.0f )
		pNetMsg->fRATE = 0.0f;

	SENDTOALLCHANNEL ( pNetMsg );

	m_WorkEvent.bWorkEvent[EVENT_GRADE] = TRUE;
	m_WorkEvent.grade_Event	  = *pNetMsg;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventGradeEnd( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_EVENT_GRADE_END *msg = (GLMSG::SNET_GM_EVENT_GRADE_END *)nmg;
	msg->dwChaNum = pMyChar->ChaDbNum();

	SENDTOALLCHANNEL(nmg);

	m_WorkEvent.bWorkEvent[EVENT_GRADE] = FALSE;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventClassEvent ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	
	if ( !pMyChar )
		return FALSE;
	
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_CLASS_EVENT *pNetMsg = (GLMSG::SNET_GM_CLASS_EVENT *)nmg;
	pNetMsg->dwChaNum = pMyChar->ChaDbNum();

	if ( pNetMsg->fEventRate > 10.0f )
		pNetMsg->fEventRate = 10.0f;

	SENDTOALLCHANNEL ( pNetMsg );

	m_WorkEvent.bWorkEvent[EVENT_CLASS_EXP+pNetMsg->dwEventType]	= pNetMsg->bStart;
	m_WorkEvent.class_Event[pNetMsg->dwEventType]	= *pNetMsg;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventMoneyGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_EVEN_MONEY_GEN *pNetMsg = (GLMSG::SNET_GM_EVEN_MONEY_GEN *)nmg;
	if ( pNetMsg->fRATE > 10.0f )	pNetMsg->fRATE = 10.0f;

	SENDTOALLCHANNEL ( pNetMsg );

	m_WorkEvent.bWorkEvent[EVENT_MONEY] = TRUE;
	m_WorkEvent.money_Event	  = *pNetMsg;

	return TRUE;
}

BOOL GLAgentServer::MsgGmEventMoneyGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	SENDTOALLCHANNEL ( nmg );

	m_WorkEvent.bWorkEvent[EVENT_MONEY] = FALSE;

	return TRUE;
}

BOOL GLAgentServer::MsgGmBigHead ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_BIGHEAD* pNetMsg = (GLMSG::SNET_GM_BIGHEAD*) nmg;

	// 모든 필드 서버에 송신.
	//
	SENDTOALLCHANNEL ( nmg );

	// 모든 클라이언트에 송신.
	//
	GLMSG::SNET_GM_BIGHEAD_BRD NetMsgBrd;
	NetMsgBrd.bBIGHEAD = pNetMsg->bBIGHEAD;
	NetMsgBrd.fSize = pNetMsg->fSize;
	SENDTOALLCLIENT ( &NetMsgBrd );

	return TRUE;
}

BOOL GLAgentServer::MsgGmBigHand ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_BIGHAND* pNetMsg = (GLMSG::SNET_GM_BIGHAND*) nmg;

	// 모든 필드 서버에 송신.
	//
	SENDTOALLCHANNEL ( nmg );

	// 모든 클라이언트에 송신.
	//
	GLMSG::SNET_GM_BIGHAND_BRD NetMsgBrd;
	NetMsgBrd.bBIGHAND = pNetMsg->bBIGHAND;
	NetMsgBrd.fSize = pNetMsg->fSize;
	SENDTOALLCLIENT ( &NetMsgBrd );

	return TRUE;
}

BOOL GLAgentServer::MsgGmFreePK(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return FALSE;
	if (pMyChar->m_dwUserLvl < USER_GM2)
        return FALSE;

	GLMSG::SNET_GM_FREEPK* pNetMsg = (GLMSG::SNET_GM_FREEPK*) nmg;

	m_pFreePK->StartFREEPKMODE(pNetMsg->dwPKTIME);
	SENDTOALLCHANNEL(nmg);
	
	GLMSG::SNET_GM_FREEPK_BRD NetMsgBrd;
	NetMsgBrd.dwPKTIME = pNetMsg->dwPKTIME;
	SENDTOALLCLIENT(&NetMsgBrd);
	return TRUE;
}

BOOL GLAgentServer::MsgFreePkGmCommand(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_FREEPK_GM_COMMAND_CA* pNetMsg = (GLMSG::SNET_FREEPK_GM_COMMAND_CA*)nmg;
	SetFreePkGMCommand( pNetMsg->bBool );
	SetFreePkMode(pNetMsg->bBool);
	return TRUE;
}

BOOL GLAgentServer::MsgGmViewAllPlayer ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNET_GM_VIEWALLPLAYER_FLD_REQ NetMsgFld;
	NetMsgFld.dwID = dwGaeaID;

	// 필드서버에 요청
	SENDTOFIELD ( pMyChar->m_ClientSlot, &NetMsgFld );

	return TRUE;
}

// *****************************************************
// Desc: 지정된 케릭터를 자기 옆으로 소환
// *****************************************************
BOOL GLAgentServer::MsgGmGenChar ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNETPC_GM_GENCHAR* pNetMsg = (GLMSG::SNETPC_GM_GENCHAR*) nmg;
	GLMSG::SNETPC_GM_GENCHAR_FB NetMsgFB;

	GLCharAG* pGenCHAR;

	if ( pNetMsg->dwCHARID != 0 )
	{
		pGenCHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	}
	else
	{
		pGenCHAR = GetChar ( pNetMsg->szNAME );
	}

	if ( !pGenCHAR )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pGenCHAR->m_dwCurFieldSvr==FIELDSERVER_MAX )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pGenCHAR->m_nChannel!=pMyChar->m_nChannel )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_CHANNEL;
		NetMsgFB.nChannel = pGenCHAR->m_nChannel;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// GM의 위치를 확인한다.
	GLMSG::SNETPC_GM_MOVE2CHAR_POS NetMsgCK;
	NetMsgCK.dwCOMMAND_CHARID = pGenCHAR->m_CharDbNum;
	NetMsgCK.dwTO_CHARID = pMyChar->m_CharDbNum;
	SENDTOFIELDSVR ( pMyChar->m_nChannel, pMyChar->m_dwCurFieldSvr, &NetMsgCK );

	NetMsgFB.emFB       = EMGM_MOVE2CHAR_FB_OK;
	
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	return TRUE;
}


BOOL GLAgentServer::MsgGmPrintCrowList ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNET_GM_PRINT_CROWLIST* pNetMsg = (GLMSG::SNET_GM_PRINT_CROWLIST*) nmg;

	GLMSG::SNET_GM_PRINT_CROWLIST_FLD NetMsgFld;
	NetMsgFld.dwFindMob = pNetMsg->dwFindMob;
	NetMsgFld.dwGaeaID  = dwGaeaID;
	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmGenMob ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_MOB_GEN* pNetMsg = (GLMSG::SNET_GM_MOB_GEN*) nmg;

	GLMSG::SNET_GM_MOB_GEN_FLD NetMsgFld;

	NetMsgFld.dwGaeaID = dwGaeaID;
	NetMsgFld.wMainID = pNetMsg->wMainID;
	NetMsgFld.wSubID = pNetMsg->wSubID;
	NetMsgFld.wPosX = pNetMsg->wPosX;
	NetMsgFld.wPosY = pNetMsg->wPosY;
	
	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmGenMobEx ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_MOB_GEN_EX* pNetMsg = (GLMSG::SNET_GM_MOB_GEN_EX*) nmg;

	SNATIVEID sMapID(pNetMsg->wMapMID,pNetMsg->wMapSID);
	if(! m_sMapList.FindMapNode( sMapID.dwID ) )
		return FALSE;

	GLMSG::SNET_GM_MOB_GEN_EX_FLD NetMsgFld;
	NetMsgFld.wMobMID  = pNetMsg->wMobMID;
	NetMsgFld.wMobSID  = pNetMsg->wMobSID;
	NetMsgFld.wMapMID  = pNetMsg->wMapMID;
	NetMsgFld.wMapSID  = pNetMsg->wMapSID;
	NetMsgFld.wPosX	   = pNetMsg->wPosX;
	NetMsgFld.wPosY    = pNetMsg->wPosY;
	NetMsgFld.wRange   = pNetMsg->wRange;
	NetMsgFld.wNum     = pNetMsg->wNum;
	NetMsgFld.dwGaeaID = dwGaeaID;
	
	if ( pNetMsg->bThisChannel ) SENDTOCHANNEL ( &NetMsgFld, pMyChar->m_nChannel );
	else						 SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmDelMobEx ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_MOB_DEL_EX* pNetMsg = (GLMSG::SNET_GM_MOB_DEL_EX*) nmg;

	SNATIVEID sMapID(pNetMsg->wMapMID,pNetMsg->wMapSID);
	if(! m_sMapList.FindMapNode( sMapID.dwID ) )
		return FALSE;

	GLMSG::SNET_GM_MOB_DEL_EX_FLD NetMsgFld;
	NetMsgFld.wMobMID  = pNetMsg->wMobMID;
	NetMsgFld.wMobSID  = pNetMsg->wMobSID;
	NetMsgFld.wMapMID  = pNetMsg->wMapMID;
	NetMsgFld.wMapSID  = pNetMsg->wMapSID;
	
	if ( pNetMsg->bThisChannel ) SENDTOCHANNEL ( &NetMsgFld, pMyChar->m_nChannel );
	else						 SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmDelMob( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_MOB_DEL* pNetMsg = (GLMSG::SNET_GM_MOB_DEL*) nmg;

	GLMSG::SNET_GM_MOB_DEL_FLD NetMsgFld;
	NetMsgFld.dwGaeaID	= dwGaeaID;
	NetMsgFld.sMobID	= pNetMsg->sMobID;
	NetMsgFld.bMaterial	= pNetMsg->bMaterial;

	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmMobLevel ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_GM1 )
		return FALSE;

	SENDTOALLCHANNEL ( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgGmMobLevelClear ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_GM1 )
		return FALSE;

	SENDTOALLCHANNEL ( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgGmWhereNPC ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_WHERE_NPC* pNetMsg = (GLMSG::SNET_GM_WHERE_NPC*) nmg;

	GLMSG::SNET_GM_WHERE_NPC_FLD NetMsgFld;

	NetMsgFld.nidNPC   = pNetMsg->nidNPC;
	NetMsgFld.dwGaeaID = dwGaeaID;
	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmWhereNPCALL ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pCHAR = GetChar ( dwGaeaID );
	if ( !pCHAR )
		return FALSE;
	if ( pCHAR->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_WHERE_NPC_ALL* pNetMsg = (GLMSG::SNET_GM_WHERE_NPC_ALL*) nmg;

	GLMSG::SNET_GM_WHERE_NPC_ALL_FLD NetMsgFld;

	NetMsgFld.dwGaeaID = dwGaeaID;
	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}


// *****************************************************
// Desc: 친구위치 확인후 필드서버에 이동 요청 (field->Agent)
// *****************************************************
BOOL GLAgentServer::Msg2FriendAG ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_2_FRIEND_AG *pNetMsg = (GLMSG::SNETPC_2_FRIEND_AG *) nmg;
	GLMSG::SNETPC_2_FRIEND_FB NetMsgFB;

	GLCharAG* pCHAR = GetCharByDbNum ( pNetMsg->dwCOMMAND_CHARID );
	if ( !pCHAR )
		return FALSE;

	if ( pNetMsg->emFB==EM2FRIEND_FB_OK )
	{
		GLMSG::SNETPC_2_FRIEND_FLD NetMsgFld;
        NetMsgFld.dwChaNum = pCHAR->ChaDbNum();
		NetMsgFld.sFriendMapID = pNetMsg->sFriendMapID;
		NetMsgFld.vFriendPos = pNetMsg->vFriendPos;
		NetMsgFld.wItemPosX = pNetMsg->wItemPosX;
		NetMsgFld.wItemPosY = pNetMsg->wItemPosY;
		SENDTOFIELD ( pCHAR->m_ClientSlot, &NetMsgFld );
	}
	else
	{
		// 친구에게 이동 실패 반환.
		NetMsgFB.emFB = pNetMsg->emFB;
		SENDTOCLIENT ( pCHAR->m_ClientSlot, &NetMsgFB );
	}

	return TRUE;
}

// *****************************************************
// Desc: 지정한 캐릭터의 귓속말을 엿듣는다.
// *****************************************************
BOOL GLAgentServer::MsgGetWhisperMsg( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_GM_GETWHISPERMSG* pNetMsg = (GLMSG::SNETPC_GM_GETWHISPERMSG*) nmg;

	GLCharAG* pMyChar = GetChar( dwGaeaID );
	if( !pMyChar )
		return FALSE;

	if( pMyChar->m_dwUserLvl < USER_USER_GM )
	{
		return FALSE;
	}

	if( strlen( pNetMsg->szNAME ) != 0 )
	{
		GLCharAG* pTo_CHAR;
		pTo_CHAR = GetChar( pNetMsg->szNAME );
		if( pTo_CHAR )
		{		
			UINT i; 
			bool bAdd = TRUE;
			for( i = 0; i < pTo_CHAR->m_vecGetWhisperList.size(); i++ )
			{
				if( dwClientID == pTo_CHAR->m_vecGetWhisperList[i].dwClientID &&
					dwGaeaID == pTo_CHAR->m_vecGetWhisperList[i].dwGaeaID )
				{
					bAdd = FALSE; break;
				}
			}
			if( bAdd )
			{
				SGETWHISPERLIST WhisperList;
				WhisperList.dwClientID = dwClientID;
				WhisperList.dwGaeaID   = dwGaeaID;
				pTo_CHAR->m_vecGetWhisperList.push_back( WhisperList );

				pMyChar->m_GmWhisperList.dwGaeaID   = pTo_CHAR->GaeaId();
				pMyChar->m_GmWhisperList.dwClientID = pTo_CHAR->m_ClientSlot;
				pMyChar->m_bViewWhisperMSG			= TRUE;
			}
		}
	}else{
		pMyChar->m_bViewWhisperMSG = FALSE;
	}
	return TRUE;
}

// *****************************************************
// Desc: 지정된 케릭터 위치로 이동
// *****************************************************
BOOL GLAgentServer::MsgMove2Char ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_GM_MOVE2CHAR* pNetMsg = (GLMSG::SNETPC_GM_MOVE2CHAR*) nmg;
	GLMSG::SNETPC_GM_MOVE2CHAR_FB NetMsgFB;

	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;

	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLCharAG* pTO_CHAR;;

	if ( pNetMsg->dwCHARID != 0 )
	{
		pTO_CHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	}
	else
	{
		pTO_CHAR = GetChar ( pNetMsg->szNAME );
	}

	if ( !pTO_CHAR )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_FAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pTO_CHAR->m_dwCurFieldSvr==FIELDSERVER_MAX )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pTO_CHAR->m_nChannel!=pMyChar->m_nChannel )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_CHANNEL;
		NetMsgFB.nChannel = pTO_CHAR->m_nChannel;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 친구의 위치 점검 요청.
	GLMSG::SNETPC_GM_MOVE2CHAR_POS NetMsgCK;
	NetMsgCK.dwCOMMAND_CHARID = pMyChar->m_CharDbNum;
	NetMsgCK.dwTO_CHARID = pTO_CHAR->m_CharDbNum;
	SENDTOFIELDSVR ( pTO_CHAR->m_nChannel, pTO_CHAR->m_dwCurFieldSvr, &NetMsgCK );

	return TRUE;
}

// *****************************************************
// Desc: 유저 채팅 블럭 처리 (User Account)
// *****************************************************
BOOL GLAgentServer::MsgGmChatBlockUA ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_CHAT_BLOCK_UACCOUNT *pNetMsg = (GLMSG::SNET_GM_CHAT_BLOCK_UACCOUNT *) nmg;
	GLMSG::SNET_GM_CHAT_BLOCK_FB NetMsgFB;

    GLCharAG* pCHAR = GetCharByUserAccount(std::string(pNetMsg->szUACCOUNT));
	// 접속중이 아닐경우
	if ( !pCHAR )
	{
		NetMsgFB.bBLOCK = false;
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pNetMsg->szUACCOUNT );
		SENDTOCLIENT ( dwClientID, &NetMsgFB );

		// 채팅블럭 예약자 DB에 저장한다.

		return FALSE;
	}

	if ( pNetMsg->dwBLOCK_MINUTE!=0 )
	{
		//	블럭시간 지정.
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

	//	DB에 저장.
    AddGameDbJob(
        db::DbActionPtr(
            new db::UserChatBlockTimeSet(pCHAR->m_UserDbNum,pCHAR->m_tCHATBLOCK)));

	//	FLD에 알림.
	GLMSG::SNET_GM_CHAT_BLOCK_FLD NetMsgFLD;
	NetMsgFLD.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFLD.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	SENDTOALLCHANNEL(&NetMsgFLD);

	//	FB.
	NetMsgFB.bBLOCK = true;
	NetMsgFB.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	// 블럭유저에게 블럭 유무 알림
	GLMSG::SNET_GM_CHAT_BLOCK_NOTIFY NetChatBlockNotify;
	NetChatBlockNotify.tChatBlock = pCHAR->m_tCHATBLOCK;
	NetChatBlockNotify.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	SENDTOCLIENT ( pCHAR->ClientSlot(), &NetChatBlockNotify );

	return TRUE;
}

// *****************************************************
// Desc: 유저 채팅 블럭 처리 (캐릭터명)
// *****************************************************
BOOL GLAgentServer::MsgGmChatBlockCN ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_CHAT_BLOCK_CHARNAME *pNetMsg = (GLMSG::SNET_GM_CHAT_BLOCK_CHARNAME *) nmg;
	GLMSG::SNET_GM_CHAT_BLOCK_FB NetMsgFB;

	GLCharAG* pCHAR = GetChar ( pNetMsg->szCHARNAME );
	// 접속중이 아닐경우
	if ( !pCHAR )
	{
		NetMsgFB.bBLOCK = false;
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pNetMsg->szCHARNAME );
		SENDTOCLIENT ( dwClientID, &NetMsgFB );

		// 채팅블럭 예약자 DB에 저장한다.
		

		return FALSE;
	}

	if ( pNetMsg->dwBLOCK_MINUTE!=0 )
	{
		//	블럭시간 지정.
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

	//	DB에 저장.	
	AddGameDbJob(
        db::DbActionPtr(
            new db::UserChatBlockTimeSet(pCHAR->m_UserDbNum,pCHAR->m_tCHATBLOCK)));

	//	FLD에 알림.
	GLMSG::SNET_GM_CHAT_BLOCK_FLD NetMsgFLD;
	NetMsgFLD.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFLD.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	SENDTOALLCHANNEL(&NetMsgFLD);

	//	FB.
	NetMsgFB.bBLOCK = true;
	NetMsgFB.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	// 블럭유저에게 블럭 유무 알림
	GLMSG::SNET_GM_CHAT_BLOCK_NOTIFY NetChatBlockNotify;
	NetChatBlockNotify.tChatBlock = pCHAR->m_tCHATBLOCK;
	NetChatBlockNotify.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	SENDTOCLIENT ( pCHAR->ClientSlot(), &NetChatBlockNotify );

	return TRUE;
}

BOOL GLAgentServer::MsgGmChatBlockCI ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNET_GM_CHAT_BLOCK_CHARID *pNetMsg = (GLMSG::SNET_GM_CHAT_BLOCK_CHARID *) nmg;
	GLMSG::SNET_GM_CHAT_BLOCK_FB NetMsgFB;

	GLCharAG* pCHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	if ( !pCHAR )
	{
		NetMsgFB.bBLOCK = false;
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, "notfound" );
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	if ( pNetMsg->dwBLOCK_MINUTE!=0 )
	{
		// 블럭시간 지정
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

	//	DB에 저장.
    AddGameDbJob(
        db::DbActionPtr(
            new db::UserChatBlockTimeSet(pCHAR->m_UserDbNum, pCHAR->m_tCHATBLOCK)));

	//	FLD에 알림.
	GLMSG::SNET_GM_CHAT_BLOCK_FLD NetMsgFLD;
	NetMsgFLD.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFLD.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	SENDTOALLCHANNEL(&NetMsgFLD);

	//	FB.
	NetMsgFB.bBLOCK = true;
	NetMsgFB.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	// 블럭유저에게 블럭 유무 알림
	GLMSG::SNET_GM_CHAT_BLOCK_NOTIFY NetChatBlockNotify;
	NetChatBlockNotify.tChatBlock = pCHAR->m_tCHATBLOCK;
	NetChatBlockNotify.dwBLOCK_MINUTE = pNetMsg->dwBLOCK_MINUTE;
	SENDTOCLIENT ( pCHAR->ClientSlot(), &NetChatBlockNotify );

	return TRUE;
}

BOOL GLAgentServer::MsgGmWarningMSG ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM1 )
		return FALSE;

	GLMSG::SNET_GM_WARNING_MSG *pNetMsg = (GLMSG::SNET_GM_WARNING_MSG*) nmg;
	GLMSG::SNET_GM_WARNING_MSG_FLD NetMsgBrd;

	NetMsgBrd.dwGaeaID = dwGaeaID;
	NetMsgBrd.bOn = pNetMsg->bOn;
	SENDTOALLCHANNEL ( &NetMsgBrd );

	return TRUE;
}

BOOL GLAgentServer::MsgGmShowMeTheMoney ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
		return FALSE;
	if (pMyChar->m_dwUserLvl < USER_MASTER)
		return FALSE;

	GLMSG::SNET_GM_SHOWMETHEMONEY* pNetMsg = (GLMSG::SNET_GM_SHOWMETHEMONEY*) nmg;
	
    GLMSG::SNET_GM_SHOWMETHEMONEY_FLD NetMsgBrd;
	NetMsgBrd.dwGaeaID = dwGaeaID;
	NetMsgBrd.llMoney = pNetMsg->llMoney;
	SENDTOALLCHANNEL(&NetMsgBrd);

	return TRUE;
}

BOOL GLAgentServer::MsgGmWhySoSerious ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    GLMSG::SNET_GM_WHYSOSERIOUS* pNetMsg = (GLMSG::SNET_GM_WHYSOSERIOUS*) nmg;

	GLCharAG* pGM(GetChar(dwGaeaID));
	GLCharAG* pTargetPlayer(GetChar(pNetMsg->szTargetName));
	if ( (pGM == NULL) || (pTargetPlayer == NULL) )
		return FALSE;
	if (pGM->m_dwUserLvl < USER_MASTER)
		return FALSE;
    
    GLMSG::SNET_GM_WHYSOSERIOUS_FLD NetMsgBrd;
	NetMsgBrd.dwMasterUserNum = pGM->m_UserDbNum;
	NetMsgBrd.dwMasterGaeaID = dwGaeaID;
    NetMsgBrd.dwGaeaID = pTargetPlayer->GaeaId();
    NetMsgBrd.sItemID  = pNetMsg->sItemID;
    NetMsgBrd.dwNums   = pNetMsg->dwNums;
	::StringCchCopy(NetMsgBrd.szMasterID, USR_ID_LENGTH, pGM->m_szUserName);
	::StringCchCopy(NetMsgBrd.szMasterIP, MAX_IP_LENGTH, pGM->GetIp());
	
    SENDTOALLCHANNEL(&NetMsgBrd);

    return TRUE;
}
BOOL GLAgentServer::MsgGmWhySoSeriousFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	GLMSG::SNET_GM_WHYSOSERIOUS_FB_FAC* pMessageFB((GLMSG::SNET_GM_WHYSOSERIOUS_FB_FAC*)nmg);
	GLCharAG* const pGM(GetChar(pMessageFB->dwGaeaIDGM));
	if ( pGM == NULL )
		return FALSE;

	SENDTOCLIENT(pGM->ClientSlot(), nmg);
    return TRUE;
}

BOOL GLAgentServer::MsgLogToDB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pMyChar = GetChar(dwGaeaID);
    if (!pMyChar)
        return FALSE;
    if (pMyChar->m_dwUserLvl < USER_GM3)
        return FALSE;

    GetItemLimit()->TxtLogToDBForce();

    GLMSG::SNET_GM_LOG_TO_DB_CAF* pNetMsg = (GLMSG::SNET_GM_LOG_TO_DB_CAF*) nmg;
    SENDTOALLCHANNEL(pNetMsg);

    return TRUE;
}

BOOL GLAgentServer::MsgGmInstantMapNum ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNETPC_GM_CHECKINSTANTNUM_FB NetMsgFB;
	NetMsgFB.dwInstantmapCnt = (DWORD)m_vecInstantMapId.size()/*m_sInsertInstantMapId.useMapSubIDSet.size()*/;	//생성된 인던의 갯수

	SENDTOCLIENT ( dwClientID, &NetMsgFB );


	return TRUE;
}

BOOL GLAgentServer::MsgGmAutoLevel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( m_pAutoLevel == NULL )
		return FALSE;

	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	return m_pAutoLevel->messageProcedure(nmg, dwGaeaID);	
}

BOOL GLAgentServer::MsgGmMapLayer(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( m_pMapLayer == NULL )
		return FALSE;

	return m_pMapLayer->messageProcedure(nmg, dwGaeaID);
}

BOOL GLAgentServer::MsgUserCharInfo4NAME ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;

	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_USER_CHAR_INFO_4NAME *pNetMsg = (GLMSG::SNET_USER_CHAR_INFO_4NAME *) nmg;
	GLMSG::SNET_USER_CHAR_INFO_AGT_FB NetMsgFB;

	GLCharAG* pCHAR = GetChar ( pNetMsg->szCHARNAME );
	if ( !pCHAR )
	{
		StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, "not found" );
		SENDTOCLIENT ( dwClientID, &NetMsgFB );

		return FALSE;
	}

	//	필드에 상세 정보 요청.
	GLMSG::SNET_USER_CHAR_INFO_FLD NetMsgFld;
	NetMsgFld.dwChaNumRequest = pMyChar->ChaDbNum();
	NetMsgFld.dwCHARID = pCHAR->m_CharDbNum;
	//NetMsgFld.dwCLIENTID_FB = dwClientID;
	SENDTOALLCHANNEL( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmCharInfo4NAME ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNET_GM_CHAR_INFO_4NAME *pNetMsg = (GLMSG::SNET_GM_CHAR_INFO_4NAME *) nmg;
	GLMSG::SNET_GM_CHAR_INFO_AGT_FB NetMsgFB;

	GLCharAG* pCHAR = GetChar ( pNetMsg->szCHARNAME );
	if ( !pCHAR )
	{
		StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, "notfound" );
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, "notfound" );
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	클라이언트에 agt fb
	StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	NetMsgFB.dwSERVER = m_dwSvrGroupNum;
	NetMsgFB.dwCHANNEL = (DWORD) pCHAR->m_nChannel;
	NetMsgFB.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFB.dwGAEAID = pCHAR->GaeaId();
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	//	필드에 상세 정보 요청.
	GLMSG::SNET_GM_CHAR_INFO_FLD NetMsgFld;
	NetMsgFld.dwCHARID = pCHAR->m_CharDbNum;
	//NetMsgFld.dwCLIENTID_FB = dwClientID;
	NetMsgFld.dwChaNumRequest = pMyChar->ChaDbNum();
	SENDTOALLCHANNEL(&NetMsgFld);

	return TRUE;
}

BOOL GLAgentServer::MsgGmCharInfo4CID ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar ( dwGaeaID );
	if ( !pMyChar )
		return FALSE;
	if ( pMyChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLMSG::SNET_GM_CHAR_INFO_4CHARID *pNetMsg = (GLMSG::SNET_GM_CHAR_INFO_4CHARID *) nmg;
	GLMSG::SNET_GM_CHAR_INFO_AGT_FB NetMsgFB;

	GLCharAG* pCHAR = GetCharByDbNum ( pNetMsg->dwCHARID );
	if ( !pCHAR )
	{
		StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, "notfound" );
		StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, "notfound" );
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	//	클라이언트에 agt fb
	StringCchCopy ( NetMsgFB.szCHARNAME, CHAR_SZNAME, pCHAR->m_szName );
	StringCchCopy ( NetMsgFB.szUACCOUNT, USR_ID_LENGTH+1, pCHAR->m_szUserName );
	NetMsgFB.dwSERVER = m_dwSvrGroupNum;
	NetMsgFB.dwCHANNEL = (DWORD) pCHAR->m_nChannel;
	NetMsgFB.dwCHARID = pCHAR->m_CharDbNum;
	NetMsgFB.dwGAEAID = pCHAR->GaeaId();
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	//	필드에 상세 정보 요청.
	GLMSG::SNET_GM_CHAR_INFO_FLD NetMsgFld;
	NetMsgFld.dwCHARID = pCHAR->m_CharDbNum;
	//NetMsgFld.dwCLIENTID_FB = dwClientID;
	NetMsgFld.dwChaNumRequest = pMyChar->ChaDbNum();
	SENDTOALLCHANNEL ( &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgGmCharInfoFldFB ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_CHAR_INFO_FLD_FB *pNetMsg = (GLMSG::SNET_GM_CHAR_INFO_FLD_FB *) nmg;

	SENDTOCLIENT(pNetMsg, pNetMsg->dwChaNumRequest);

	return TRUE;
}

BOOL GLAgentServer::MsgUserCharInfoFldFB ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_USER_CHAR_INFO_FLD_FB *pNetMsg = (GLMSG::SNET_USER_CHAR_INFO_FLD_FB *)nmg;

	SENDTOCLIENT(pNetMsg, pNetMsg->dwChaNumRequest);

	return TRUE;
}

// *****************************************************
// Desc: 케릭터 이동을 필드서버에 요청
// *****************************************************
BOOL GLAgentServer::MsgMove2CharAG ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_GM_MOVE2CHAR_AG *pNetMsg = (GLMSG::SNETPC_GM_MOVE2CHAR_AG *) nmg;
	GLMSG::SNETPC_GM_MOVE2CHAR_FB NetMsgFB;

	GLCharAG* pCHAR = GetCharByDbNum ( pNetMsg->dwCOMMAND_CHARID );
	if ( !pCHAR )
		return FALSE;

	if ( pNetMsg->emFB != EMGM_MOVE2CHAR_FB_OK )
	{ // 친구에게 이동 실패 반환.
		NetMsgFB.emFB = pNetMsg->emFB;
		SENDTOCLIENT ( pCHAR->m_ClientSlot, &NetMsgFB );
		return FALSE;
	}


	const MapID& _mapIDTo(pNetMsg->mapIDTo);
	const SNATIVEID _baseMapIDTo(_mapIDTo.getBaseMapID());
	const SNATIVEID _gaeaMapIDTo(_mapIDTo.getGaeaMapID());
	if ( _gaeaMapIDTo == SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMGM_MOVE2CHAR_FB_TO_CONDITION;
		SENDTOCLIENT ( pCHAR->m_ClientSlot, &NetMsgFB );
		return FALSE;
	}	

	GLMSG::SNETPC_GM_MOVE2CHAR_FLD NetMsgFld;
	NetMsgFld.dwChaNum = pCHAR->ChaDbNum();
	NetMsgFld.dwFieldServerIDTo = pNetMsg->dwFieldServerIDTo;
	NetMsgFld.mapIDTo = _mapIDTo;
	NetMsgFld.vToPos = pNetMsg->vToPos;
	SENDTOFIELD ( pCHAR->m_ClientSlot, &NetMsgFld );

	return TRUE;
}

BOOL GLAgentServer::MsgClub_GuidBattle_RemainTime ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_REMAIN_AG* pNetMsg = (GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_REMAIN_AG*) nmg;

	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if ( pChar )
	{
        m_pGuidance->SendGuidBattle_RemainTime( pChar->ClientSlot() );
	}
	return TRUE;
}

BOOL GLAgentServer::MsgClubDeathMatchRemainTime ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG* pNetMsg = (GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG*) nmg;
	GLCharAG* pChar = GetChar(pNetMsg->dwGaeaID);
	if (pChar)
	{
        m_pClubDeathMatchMan->SendClubDeathMatch_RemainTime( pChar->ClientSlot() );
	}
	return TRUE;
}


BOOL GLAgentServer::MsgGmClubDeathMatchStartEndCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
	GLMSG::SNET_GM_DEATHMATCH_ON_OFF_CA* pPacket = (GLMSG::SNET_GM_DEATHMATCH_ON_OFF_CA*) pMsg;
	GLCharAG* pChar = GetCharByGaeaID(GaeaID);
	if (!pChar)
		return FALSE;

	if (pChar->UserType() < USER_GM3)
		return FALSE;

	if (pPacket->m_nFlag == GLMSG::SNET_GM_DEATHMATCH_ON_OFF_CA::ONE)
	{
		GLClubDeathMatch* pClubDeathMatch = m_pClubDeathMatchMan->Find(pPacket->m_dwCDMID);
		if (!pClubDeathMatch)
			return FALSE;

		switch(pClubDeathMatch->m_emBattleState)
		{
		case GLClubDeathMatch::BATTLE_NOSTART://	시작하지 않음
			{
				pClubDeathMatch->m_bGMReady = true;
			}
			return TRUE;

		case GLClubDeathMatch::BATTLE_READY://	배틀 대기중
			{
				pClubDeathMatch->m_fReadyTimer = 0;
				pClubDeathMatch->m_bGMReady = false;
				pClubDeathMatch->DoBattleStart(this,0,-1);

				GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubBattle;
				NetMsgClubBattle.bClubDeathMatch = true;
				SENDTOALLCLIENT( &NetMsgClubBattle );

				m_pClubDeathMatchMan->SendClubDeathMatch_RemainTime_BRD();


			}
			return TRUE;;

		case GLClubDeathMatch::BATTLE_START://	배틀 중
			{
				pClubDeathMatch->m_fBattleTimer = static_cast<float>(pClubDeathMatch->m_dwBattleTime);
				m_pClubDeathMatchMan->SendClubDeathMatch_RemainTime_BRD();
				pClubDeathMatch->DoBattleEnd(this);

				GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubBattle;
				NetMsgClubBattle.bClubDeathMatch = false;
				SENDTOALLCLIENT( &NetMsgClubBattle );
			}
			return TRUE;
		}
	}

	if (pPacket->m_nFlag == GLMSG::SNET_GM_DEATHMATCH_ON_OFF_CA::ALL)
	{
		std::vector<GLClubDeathMatch>& vecClubDeathMatch = m_pClubDeathMatchMan->GetVecClubDeathMatch();
		for (size_t i=0; i<vecClubDeathMatch.size(); ++i)
		{
			switch(vecClubDeathMatch[i].m_emBattleState)
			{
			case GLClubDeathMatch::BATTLE_NOSTART://	시작하지 않음
				{
					vecClubDeathMatch[i].m_bGMReady = true;
				}
				return TRUE;

			case GLClubDeathMatch::BATTLE_READY://	배틀 대기중
				{
					vecClubDeathMatch[i].m_fReadyTimer = 0;
					vecClubDeathMatch[i].m_bGMReady = false;
					vecClubDeathMatch[i].DoBattleStart(this,0,-1);
					m_pClubDeathMatchMan->SendClubDeathMatch_RemainTime_BRD();

					GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubBattle;
					NetMsgClubBattle.bClubDeathMatch = true;
					SENDTOALLCLIENT( &NetMsgClubBattle );
				}
				return TRUE;

			case GLClubDeathMatch::BATTLE_START://	배틀 중
				{
					vecClubDeathMatch[i].m_fBattleTimer = static_cast<float>(vecClubDeathMatch[i].m_dwBattleTime);
					m_pClubDeathMatchMan->SendClubDeathMatch_RemainTime_BRD();
					vecClubDeathMatch[i].DoBattleEnd(this);

					GLMSG::SNETPC_SERVER_CLUB_DEATHMATCH_INFO NetMsgClubBattle;
					NetMsgClubBattle.bClubDeathMatch = false;
					SENDTOALLCLIENT( &NetMsgClubBattle );
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL GLAgentServer::MsgClubDeathMatchResult( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
    // 구버전 CDM임 신규CDM으로 교체시 코드 삭제 필요;

	GLMSG::SNET_CLUB_DEATHMATCH_END_AGT* pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_END_AGT*) nmg;

	GLMSG::SNET_CLUB_DEATHMATCH_END_BRD NetMsgBrd;
	GLMSG::SNET_SERVER_GENERALCHAT SendData;
	
	GLClubDeathMatch* pClubDM = m_pClubDeathMatchMan->Find(pNetMsg->dwCDMID);
	if (!pClubDM)
        return FALSE;
	if (!pClubDM->IsBattleEnd())
        return false;

	pClubDM->SetBattle(GLClubDeathMatch::BATTLE_NOSTART);
	
	NetMsgBrd.SetText(pClubDM->m_strName);
	SENDTOALLCLIENT(&NetMsgBrd);

	
	//	CDM랭킹을 로그로 남긴다. ( 추후에 db로 변경 예정 ) 
//	CTime cTime = CTime::GetCurrentTime();	
//	CDebugSet::ToCDMLogFile ( "CDM_ID[%d], Time[%04d-%02d-%02d]",
//		pClubDM->m_dwID, cTime.GetYear(), cTime.GetMonth(), cTime.GetDay() );

	for (int i = 0; i < RANKING_NUM; ++i)
	{
		if (pNetMsg->dwClubID[i] == CLUB_NULL)
            break;
		
		std::tr1::shared_ptr<GLClubAgent> pClub = m_pClubMan->GetClub(pNetMsg->dwClubID[i]);
		if (!pClub)
            break;

        std::string strText(
            sc::string::format(
                ID2SERVERTEXT("CLUB_DEATHMATCH_END_RANKING"), 
			    pNetMsg->wClubRanking[i],
                pClub->Name(),
                pNetMsg->wKillNum[i],
                pNetMsg->wDeathNum[i]));
		if (SendData.SETTEXT(strText))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
		    SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }

        // Club log
        std::string LogMsg(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_CDM"),
                pNetMsg->wKillNum[i],
                pNetMsg->wDeathNum[i],
                pNetMsg->wClubRanking[i]));
        SENDCLUBLOG(pNetMsg->dwClubID[i], club::LOG_CDM, LogMsg);

		//	CDM랭킹을 로그로 남긴다. ( 추후에 db로 변경 예정 ) 
//		CDebugSet::ToCDMLogFile ( "RANKING %02d CLUBID[%d]	Score[%04d/%04d] CLUB_NAME[%s]",
//			pNetMsg->wClubRanking[i], pNetMsg->dwClubID[i], pNetMsg->wKillNum[i], pNetMsg->wDeathNum[i], pClub->m_szName );
	}

	return TRUE;
}



BOOL GLAgentServer::MsgClubCDCertifyIngBrd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	SENDTOALLCLIENT ( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgClubCDCertifyBrd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	SENDTOALLCLIENT ( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgMarketOpenCheck ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);

	if( pMyChar == NULL )
	{
		sc::writeLogError(sc::string::format("%%%% Could not found Character..%d %%%%", dwGaeaID ));
	}

	return TRUE;
}

BOOL GLAgentServer::MsgClubBattleKillUpdate ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_AGT* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_AGT*) nmg;

	std::tr1::shared_ptr<GLClubAgent> pClub = m_pClubMan->GetClub ( pNetMsg->dwClubID );
	if ( !pClub )
        return FALSE;

	pClub->KillBattleClubMem( pNetMsg->dwBattleClubID, pNetMsg->wKillPoint );
	pClub->DeathBattleClubMem( pNetMsg->dwBattleClubID, pNetMsg->wDeathPoint );

	return TRUE;
}

BOOL GLAgentServer::MsgClubBattleLastKillUpdate( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT* pNetMsg = (GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT*) nmg;

	std::tr1::shared_ptr<GLClubAgent> pClub = m_pClubMan->GetClub ( pNetMsg->dwClubID_A );
	if ( !pClub )
        return FALSE;

	pClub->SetLastBattlePoint( pNetMsg->dwClubID_B, pNetMsg->wKillPoint, pNetMsg->wDeathPoint );

	return TRUE;
}

BOOL GLAgentServer::MsgReqTaxi ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_TAXI* pNetMsg = (GLMSG::SNETPC_REQ_TAXI*)nmg;

	GLMSG::SNETPC_REQ_TAXI_FB NetMsgFB;

	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
		return FALSE;

	// 정류장 검사
	STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
	if ( !pTaxiMap )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 정류장 검사
	STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
	if ( !pStation )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_STATIONFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}
	
	SNATIVEID sMAPID(pStation->dwMAPID);

	if ( sMAPID==SNATIVEID(false) )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	DWORD dwFieldServer = GetFieldServer( sMAPID );
	if ( dwFieldServer == FIELDSERVER_MAX )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

	// 같은 필드이면 그냥 보내고
	// 다른 필드이면 좌표를 구해서 보내준다.
	if ( dwFieldServer == pMyChar->m_dwCurFieldSvr )
	{
		pNetMsg->dwChaNum = pMyChar->ChaDbNum();
		SENDTOFIELD( dwClientID, pNetMsg );
	}
	else
	{
		GLMSG::SNETPC_REQ_TAXI_NPCPOS NetMsgPos;
		NetMsgPos.dwSelectMap = pNetMsg->dwSelectMap;
		NetMsgPos.dwSelectStop = pNetMsg->dwSelectStop;
		NetMsgPos.dwGaeaID = pNetMsg->dwGaeaID;
		NetMsgPos.wPosX = pNetMsg->wPosX;
		NetMsgPos.wPosY = pNetMsg->wPosY;

		SENDTOFIELDSVR( pMyChar->m_nChannel, dwFieldServer, &NetMsgPos );
	}

	return TRUE;
}

BOOL GLAgentServer::MsgReqTaxiNpcPos ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_REQ_TAXI_NPCPOS_FB* pNetMsg = (GLMSG::SNETPC_REQ_TAXI_NPCPOS_FB*)nmg;
	GLMSG::SNETPC_REQ_TAXI_FB NetMsgFB;

	GLCharAG* pMyChar = GetChar(pNetMsg->dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}

	if ( pNetMsg->vPos == D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_NPCFAIL;
		SENDTOCLIENT ( pMyChar->m_ClientSlot, &NetMsgFB );
		return FALSE;
	}
	


	GLMSG::SNETPC_REQ_TAXI NetMsg;	
	NetMsg.dwSelectMap = pNetMsg->dwSelectMap;
	NetMsg.dwSelectStop = pNetMsg->dwSelectStop;
	NetMsg.wPosX = pNetMsg->wPosX;
	NetMsg.wPosY = pNetMsg->wPosY;
	NetMsg.dwGaeaID = pNetMsg->dwGaeaID;
	NetMsg.vPos = pNetMsg->vPos;
	NetMsg.dwChaNum = pMyChar->ChaDbNum();
	
	SENDTOFIELD( pMyChar->m_ClientSlot, &NetMsg );

	return TRUE;
}

BOOL GLAgentServer::MsgNpcCommission( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_INVEN_NPC_COMMISSION* pNetMsg = (GLMSG::SNET_INVEN_NPC_COMMISSION*) nmg;
    if (pNetMsg->m_MagicNum != GLMSG::SNET_INVEN_NPC_COMMISSION::MAGIC_NUM)
    {
        std::string HackLog(
            sc::string::format(
                "Hacking! MsgNpcCommission. ChaDbNum %1% UserDbNum %2% Commission %3% MagicNum %4%",
                pNetMsg->dwCharID,
                pNetMsg->dwUserID,
                pNetMsg->lnCommission,
                pNetMsg->m_MagicNum));
        return FALSE;
    }

	GLMSG::SNET_INVEN_NPC_COMMISSION_FB NetMsgFB;

	NetMsgFB.dwCharID = pNetMsg->dwCharID;
	NetMsgFB.dwUserID = pNetMsg->dwUserID;
	NetMsgFB.lnCommission = pNetMsg->lnCommission;

	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwCharID);	

	//	캐릭터가 접속중일때 ( 타필드에 있는 상황 ) 
	//	비접속중일때 필드에 저장을 요청한다.
	if ( pChar )
	{
		NetMsgFB.emFB = EMREQ_NPC_COMMISSION_CHAR;		
		SENDTOFIELDSVR( pChar->m_nChannel, pChar->m_dwCurFieldSvr, &NetMsgFB );
	}
	else
	{
		NetMsgFB.emFB = EMREQ_NPC_COMMISSION_DB;			
		SENDTOFIELDSVR(  pNetMsg->nCHANNEL, pNetMsg->dwFieldID, &NetMsgFB );
	}
	
	return TRUE;
}


//
//mjeon.Post
//
//FA
BOOL GLAgentServer::MsgPostOpenAndTakeFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostOpenAndTakeFA() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}

	//msg from Field -> Agent
	GLMSG::SNET_POST_OPEN_TAKE_AF_FB *pmsg = (GLMSG::SNET_POST_OPEN_TAKE_AF_FB *) nmg;

	int iRecver				= pmsg->iRecver;		// Recver's charID	
	LONGLONG llPostID		= pmsg->llPostID;

	BOOL bMoneyAccept		= pmsg->byMoneyAccept > 0;		// Byte -> BOOL
	BOOL bAttach01Accept	= pmsg->byAttach01Accept > 0;	// Byte -> BOOL

	GLCharAG* pChar = GetCharByDbNum(iRecver);

	if ( pChar == NULL )
		return FALSE;
	
	DWORD dwClient = pChar->ClientSlot();

	// Reflect the result to the Agent's post data.

	BOOL bFound = FALSE;

	if ( !pChar )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_RECVER_NOTEXIST;

		SENDTOCLIENT(dwClient, &msgFB);

		return FALSE;
	}


	if ( pChar->m_pvPost->empty() )
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_POST_NOT_EXIST;

		SENDTOCLIENT(dwClient, &msgFB);

		return FALSE;
	}

	typedef std::vector<SAPOST>	VPOST;
	typedef VPOST::iterator		VPOST_ITER;

	VPOST &vp = *(pChar->m_pvPost);	
	VPOST_ITER i;

	for (i = vp.begin(); i != vp.end(); ++i)
	{
		if ( llPostID == i->m_llPostID )
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		GLMSG::SNET_POST_OPEN_TAKE_FB msgFB;

		msgFB.byAttach01Accept	= (BYTE)FALSE;
		msgFB.byMoneyAccept		= (BYTE)FALSE;
		msgFB.result			= EMPOST_POST_NOT_EXIST;

		SENDTOCLIENT(dwClient, &msgFB);

		return FALSE;
	}

/*
#ifdef _RELEASED
	sc::writeLogInfo(sc::string::format("(TRACE#7)PostAttach - bMoney:%1%, bAttach01:%2%, i->bMoney:%3%, i->bAttach01:%4%", bMoneyAccept, bAttach01Accept, (BOOL)i->byMoneyAccept, (BOOL)i->byAttach01Accept));
#endif
*/

	
	if (bMoneyAccept)	//mjeon.인도네시아 아이템 복사 해킹때문에 주석 다시 해제 -> 그러므로 시스템 요류로 인해 만일 첨부아이템을 정상적으로 받지 못했을 경우에는 재접속을 해야한다.
	{
		// update current post's status
		i->m_byMoneyAccept = (BYTE)TRUE;//bMoneyAccept;
	}


	if (bAttach01Accept)//mjeon.인도네시아 아이템 복사 해킹때문에 주석 다시 해제 -> 그러므로 시스템 요류로 인해 만일 첨부아이템을 정상적으로 받지 못했을 경우에는 재접속을 해야한다.
	{
		// update current post's status
		i->m_byAttach01Accept = (BYTE)TRUE;//bAttach01Accept;
	}

/*
#ifdef _RELEASED
	sc::writeLogInfo(sc::string::format("(TRACE#8)PostAttach - bMoney:%1%, bAttach01:%2%, i->bMoney:%3%, i->bAttach01:%4%", bMoneyAccept, bAttach01Accept, (BOOL)i->byMoneyAccept, (BOOL)i->byAttach01Accept));
#endif
*/

	// Just Relay the msg to the client.

	GLMSG::SNET_POST_OPEN_TAKE_FB msgAC;	// Agent -> Client

	msgAC.llPostID			= pmsg->llPostID;
	msgAC.byAttach01Accept	= pmsg->byAttach01Accept;
	msgAC.byMoneyAccept		= pmsg->byMoneyAccept;
	msgAC.result			= pmsg->result;

    SENDTOCLIENT(dwClient, &msgAC);

	return TRUE;
}


//
//mjeon.Post.Notify
//
// FieldServer -> AgentServer
//
BOOL GLAgentServer::MsgPostNotify(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostNotify() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}
    /*
    if (dwClientID >= net::RESERVED_SLOT) // NOT from FieldServer.
	{
		//Nothing to do.		
		//Client cannot send POST_NOTIFY to AgentServer.
		return FALSE;
	}
    */
    if (dwGaeaID != GAEAID_NULL)
    {
        // NOT from FieldServer.
        sc::writeLogError(std::string("MsgPostNotify dwGaeaID != GAEAID_NULL"));
        return FALSE;
    }
    
	GLMSG::SNET_POST_NOTIFY* pPacket = (GLMSG::SNET_POST_NOTIFY*) nmg;

	LONGLONG llPostID = pPacket->llPostID;
	int iRecver = pPacket->iRecver;

	if (iRecver < 0)
	{
		sc::writeLogError(std::string("[ERROR] PostNotify has wrong iRecver."));

		return FALSE;
	}

	if (llPostID < 0)
	{
		sc::writeLogError(std::string("[ERROR] PostNotify has wrong PostID."));

		return FALSE;
	}

	GLMSG::SNET_POST_NOTIFY_RELAY msgRelay;

	msgRelay.iRecver = pPacket->iRecver;
	msgRelay.llPostID = pPacket->llPostID;


	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(iRecver);

	if (pCharAG) // Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, &msgRelay);
	}
	//else					// Offline	
	//
	// In case of Offline, Nothing to do.
    // The recver will take the notification of new post at the next login.
	//	

	return TRUE;
}


//
//mjeon.activity
//
BOOL GLAgentServer::MsgActivitySyncFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//just relay to the client
	//
	GLMSG::SNET_ACTIVITY_SYNC *msgRelay = (GLMSG::SNET_ACTIVITY_SYNC *)nmg;

	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(msgRelay->nChaNum);

	if (pCharAG != NULL)	// Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, msgRelay);
	}
	else
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgActivitySyncFA() - CharAG(%1%) does not exist!", msgRelay->nChaNum));
		return FALSE;
	}

	return TRUE;
}


BOOL GLAgentServer::MsgActivityPointSyncFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//just relay to the client
	//
	GLMSG::SNET_ACTIVITY_REWARD_POINT_SYNC *msgRelay = (GLMSG::SNET_ACTIVITY_REWARD_POINT_SYNC *)nmg;

	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(msgRelay->nChaNum);

	if (pCharAG != NULL)	// Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, msgRelay);
	}
	else
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgActivityPointSyncFA() - CharAG(%1%) does not exist!", msgRelay->nChaNum));
		return FALSE;
	}

	return TRUE;
}


BOOL GLAgentServer::MsgActivityBadgeSyncFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//just relay to the client
	//
	GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC *msgRelay = (GLMSG::SNET_ACTIVITY_REWARD_BADGE_SYNC *)nmg;

	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(msgRelay->nChaNum);

	if (pCharAG != NULL)	// Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, msgRelay);
	}
	else
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgActivityBadgeSyncFA() - CharAG(%1%) does not exist!", msgRelay->nChaNum));
		return FALSE;
	}

	return TRUE;
}



//Handling CLUB-notify (Broadcast is handled in CAgentServer::MsgActivityxxxFA)
// FieldServer -> AgentServer --(relay)--> GLAgentServer
//
BOOL GLAgentServer::MsgActivityCompletedFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{	
	GLMSG::SNET_ACTIVITY_COMPLETED_FA *pmsg = (GLMSG::SNET_ACTIVITY_COMPLETED_FA *)nmg;

// sckim : Activity
//sc::writeLogError(sc::string::format("GLAgentServer::MsgActivityCompletedFA, pmsg->dwChaNumCloser : %1%", pmsg->dwChaNumCloser));

	GLCharAG* pCharAG = GetCharByDbNum(pmsg->dwChaNumCloser);

	if (!pCharAG)
	{
		// not an error. just ignore this notify.
		sc::writeLogWarn(std::string("MsgActivityCompletedFA: Character does not exist."));

		return FALSE;
	}

	GLMSG::SNET_ACTIVITY_NOTIFY_COMPLETED msg;	//to Clients
	msg.dwActivityID  = pmsg->dwActivityID;
	StringCchCopy(msg.szChaName, CHR_ID_LENGTH+1, pCharAG->m_szName);

	SENDTOCLUBCLIENT(pCharAG->m_ClubDbNum, &msg);

	return TRUE;
}



//
//mjeon.attendance
//
BOOL GLAgentServer::MsgAttendanceSyncFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//just relay to the client
	//
	GLMSG::SNET_ATTENDANCE_SYNC_FAC *msgRelay = (GLMSG::SNET_ATTENDANCE_SYNC_FAC *)nmg;

	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(msgRelay->nChaNum);

	if (pCharAG != NULL)	// Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, msgRelay);
	}
	else
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgAttendanceSyncFAC() - CharAG(%1%) does not exist!", msgRelay->nChaNum));
		return FALSE;
	}

	return TRUE;
}


BOOL GLAgentServer::MsgAttendanceResetFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//just relay to the client
	//
	GLMSG::SNET_ATTENDANCE_RESET_FAC *msgRelay = (GLMSG::SNET_ATTENDANCE_RESET_FAC *)nmg;

	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(msgRelay->nChaNum);

	if (pCharAG != NULL)	// Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, msgRelay);
	}
	else
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgAttendanceResetFAC() - CharAG(%1%) does not exist!", msgRelay->nChaNum));
		return FALSE;
	}

	return TRUE;
}


BOOL GLAgentServer::MsgAttendanceUpgradeFAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//just relay to the client
	//
	GLMSG::SNET_ATTENDANCE_UPGRADE_FAC *msgRelay = (GLMSG::SNET_ATTENDANCE_UPGRADE_FAC *)nmg;

	GLCharAG* pCharAG = NULL;
	pCharAG = GetCharByDbNum(msgRelay->nChaNum);

	if (pCharAG != NULL)	// Online
	{
		SENDTOCLIENT(pCharAG->m_ClientSlot, msgRelay);
	}
	else
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgAttendanceUpgradeFAC() - CharAG(%1%) does not exist!", msgRelay->nChaNum));
		return FALSE;
	}

	return TRUE;
}


BOOL GLAgentServer::ProcessPingTraceMsg( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	//mjeon
	//process the PingRequest from client
	//

	//Just send back the FB messages for acknowledge.
	switch( nmg->nType )
	{
	case NET_MSG_GM_PINGTRACE_ON:
		{
			sc::writeLogInfo(std::string("[INFO]GM Command /PINGTRACE ON has input."));

			//relay the message to all fieldservers.
			SENDTOALLCHANNEL(nmg);	//Broadcast to fieldservers			

			GLMSG::SNET_GM_PINGTRACE_ON_FB MsgFB;			

			//reply to the client
            SENDTOCLIENT ( dwClientID, &MsgFB );

			break;
		}
	case NET_MSG_GM_PINGTRACE_OFF:
		{
			sc::writeLogInfo(std::string("[INFO]GM Command /PINGTRACE OFF has input."));

			//relay the message to all fieldservers.
			SENDTOALLCHANNEL(nmg);	//Broadcast to fieldservers

			GLMSG::SNET_GM_PINGTRACE_OFF_FB MsgFB;

			//reply to the client
			SENDTOCLIENT ( dwClientID, &MsgFB );

			break;
		}
	}

	return TRUE;
}


//
//mjeon.CaptureTheField
//
//FA
BOOL GLAgentServer::MsgCTFJoinFA( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFJoinFA() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	if ( dwGaeaID != GAEAID_NULL )
	{
		GLCharAG* const pChar(GetChar(dwGaeaID));
		if ( pChar != NULL )
		{
			sc::writeLogError(sc::string::format("MsgCTFJoinFA() - HACKING!! dbNum : %1%", pChar->ChaDbNum()));
		}
		return FALSE;
	}

	GLMSG::SNET_CTF_JOIN_FA *pmsg = (GLMSG::SNET_CTF_JOIN_FA *)nmg;

	GLCharAG* pChaAG = GetCharByDbNum(pmsg->dwChaNum);

	if (!pChaAG)
	{
		sc::writeLogError(std::string("MsgCTFJoinFA() - The character does not exist!"));

		return FALSE;
	}

	if(!m_pCTFMan)
		return FALSE;

	

	if(m_pCTFMan->IsConsecutively(pChaAG->GetSchool()) == FALSE)
		return FALSE; // than OUT

	const SMAPNODE* pMapNode = FindMapNode( pChaAG->GetCurrentMap().getGaeaMapID());

	if( PRISONLIMIT.IsLimitStatus(pMapNode, pChaAG->m_dwUserLvl, PML_WAR) )
		return FALSE;
	
	// 점령전 입장
	m_pCTFMan->Join(pChaAG->ClientSlot(), pChaAG, pmsg->bJoin, pmsg->wLevel, pmsg->emJoinResult);
	
	return TRUE;
}


BOOL GLAgentServer::MsgCTFPrepareAF_FB( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFPrepareAF_FB() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_PREPARE_AF_FB *pmsg = (GLMSG::SNET_CTF_PREPARE_AF_FB *)nmg;

	if (pmsg->bHere)	
	{
		m_pCTFMan->Start(pmsg->nFieldSvr);
	}

	return TRUE;
}


BOOL GLAgentServer::MsgGetChaExtraInfoFA( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *msg = (GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB *)nmg;

	GLCharAG *pChaAG = GetCharByDbNum(msg->dwChaNum);

	if (!pChaAG)
	{
		sc::writeLogError(std::string("GLAgentServer::MsgGetChaExtraInfoFA() - The character does not exist."));

		return FALSE;
	}
/*
	// mjeon.capturethefield.tracing
	sc::writeLogInfo(sc::string::format("(TRACE) Sync ContributionPoint(%1%) for Char(%2%).", msg->nContributionPoint, msg->dwChaNum));
*/

	SENDTOCLIENT(pChaAG->ClientSlot(), nmg);

	return TRUE;	
}


/*
BOOL GLAgentServer::MsgCTFSyncReqCA( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFSyncReqCA() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLCharAG* pChaAG = GetChar(dwGaeaID);
	
	if (!pChaAG)
	{
		sc::writeLogError(std::string("MsgCTFSyncReqCA() - The character does not exist!"));
		return FALSE;
	}

	m_pCTFMan->Sync(dwClientID);

	return TRUE;
}
*/


BOOL GLAgentServer::MsgCTFQuitCA(NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFQuitCA() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	// 메세지를 단순히 알림 목적만으로 사용한다 치더라도 아래와 같이 캐스팅은 반드시 하자;
	// 다음 사람이 찾아 보기 매우 힘들다;
	const GLMSG::SNET_CTF_QUIT_CFA* const pNetMsg((const GLMSG::SNET_CTF_QUIT_CFA* const)nmg);

	GLCharAG* pChaAG = GetCharByDbNum( pNetMsg->dwChaNum );
	
	if (!pChaAG)
	{
		sc::writeLogError(std::string("MsgCTFQuitCA() - The character does not exist!"));
		return FALSE;
	}


	m_pCTFMan->Quit(pChaAG->ChaDbNum(), pChaAG->m_wSchool);
		
	return TRUE;
}


//
// 하나의 학원이 모든(3개의) 인증기를 인증했을 경우, FieldS -> AgentS
//

BOOL GLAgentServer::MsgCTFEndFA(NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFEndFA() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_END_FA *pmsg = (GLMSG::SNET_CTF_END_FA *)nmg;

	GLMSG::SNET_CTF_DONE_AF_AC msg;
	msg.wSchoolAuthed = pmsg->wSchoolAuthed;
	msg.bConsecutively = m_pCTFMan->GetConsecutively();

	SENDTOALLCLIENT(&msg);

	m_pCTFMan->AuthedBy(pmsg->wSchoolAuthed);
    m_pCTFMan->DoReward();
		
	return TRUE;
}


//
// CTF 시간이 종료되었을 경우, 2개 이상의 인증기를 인증한 학원이 승리학원이 된다. 이 때, 해당 학원 정보를 FieldS로부터 획득.
//
BOOL GLAgentServer::MsgCTFDoneFA_FB(NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFDoneFA_FB() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_DONE_AF_FB *pmsg = (GLMSG::SNET_CTF_DONE_AF_FB *)nmg;

	GLMSG::SNET_CTF_DONE_AF_AC msg;
	msg.wSchoolAuthed = pmsg->wSchoolAuthed;
	msg.bConsecutively = m_pCTFMan->GetConsecutively();
	SENDTOALLCLIENT(&msg);

	m_pCTFMan->AuthedBy(pmsg->wSchoolAuthed);
    m_pCTFMan->DoReward();
		
	return TRUE;
}


BOOL GLAgentServer::MsgCTFRankingFAC(NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFRankingFAC() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_CTF_RANKING_FAC *pmsg = (GLMSG::SNET_CTF_RANKING_FAC *)nmg;		
	SENDTOALLCLIENT(nmg);	//broadcast
	
	m_pCTFMan->SetRankers(pmsg->nHowMany, pmsg->arrRanking);
		
	return TRUE;
}


BOOL GLAgentServer::MsgCTFMoveStateByGMCA(NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
	{
		sc::writeLogError(std::string("MsgCTFMoveStateByGMCA() - Wrong use of CTF while it is OFF!"));
		return FALSE;
	}

	// 사용하지 않는 메세지라 하더라도 반드시 캐스팅은 해주자,
	// 검색이 되질 않아 뒷사람이 피곤하다;
	const GLMSG::SNET_CTF_MOVESTATE_BY_GM_CA* const pNetMsg((const GLMSG::SNET_CTF_MOVESTATE_BY_GM_CA* const)nmg);

	GLMSG::SNET_CTF_MOVESTATE_BY_GM_CA_FB msg;
	msg.stateBefore = m_pCTFMan->GetState();

	msg.stateAfter	= m_pCTFMan->MoveStateByGM();

	SENDTOCLIENT(dwClientID, &msg);
	
	return TRUE;
}

BOOL GLAgentServer::MsgMatchRequestCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MATCH_REQUEST_CA *msg = (GLMSG::NET_MATCH_REQUEST_CA *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	SNATIVEID	idMap(msg->dwMapID);
	DWORD		dwGateID = 1;

	GLMSG::NET_MATCH_REQUEST_CA_FB msgFB;
	msgFB.dwMapID	= msg->dwMapID;
	msgFB.dwGateID	= msg->dwGateID;
	msgFB.bDone		= FALSE;

	GLMSG::NET_MATCH_REQUEST_AM msgMatch;
	msgMatch.dwChaNum = pChar->ChaDbNum();
	msgMatch.bUnmatch = msg->bUnmatch;
	msgMatch.dwPartySize = 1;

	//InstaceServer -> FieldServer로 전송 요청인 경우
	if (msg->bUnmatch)
	{
		idMap = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
		dwGateID = GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
	}

	GLAGLandMan* pLandManAG = GetLand(idMap);
	if (!pLandManAG)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - LandMan(%1%,%2%) does not exist.", idMap.Mid(), idMap.Sid()));

		goto feedback_match_request;
	}

	if (dwGateID == MAXDWORD)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Map(%1%,%2%)'s Gate(%3%) does not exist.", idMap.Mid(), idMap.Sid(), dwGateID));

		goto feedback_match_request;
	}

	const SMAPNODE* pMapNode = m_sMapList.FindMapNode(idMap);
	if (!pMapNode)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - MapNode(%1%,%2%) does not exist.", idMap.Mid(), idMap.Sid()));

		goto feedback_match_request;
	}

	if (!msg->bUnmatch)	//FieldServer -> InstanceServer
	{
		if (!pMapNode->IsInstantMap())
		{
			sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Map(%1%,%2%) is not an InstanceMap.", idMap.Mid(), idMap.Sid()));

			goto feedback_match_request;
		}

		if (msg->bParty)
		{
			const GLPartyID& PartyID = pChar->GetPartyID();
			if ( PartyID.isValidParty() == false )
			{
				sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Char(%1%) cannot match with party(NULL)!.", pChar->ChaDbNum()));
				
				goto feedback_match_request;
			}

			GLPartyAgent *pParty = m_pPartyManager->GetParty(PartyID);
			if (!pParty)
			{
				sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Char(%1%) cannot match. It's partyMan is NULL!.", pChar->ChaDbNum()));
				
				goto feedback_match_request;
			}

			if (pParty->m_Member.size() <= 1)
			{
				sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Char(%1%)'s party-size is wrong(%2%).", pChar->ChaDbNum(), pParty->m_Member.size()));
				
				goto feedback_match_request;
			}

			GLCharAG *pMaster = GetChar(pParty->m_Member[MASTER_PARTY]);
			if (pChar != pMaster)
			{
				sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Char(%1%)'s request: Only party master can request for a match.", pChar->ChaDbNum()));

				goto feedback_match_request;
			}

			msgMatch.dwPartySize = pParty->getNPartyMember();

            DWORD j=1;
			for (DWORD i=1 ; i<msgMatch.dwPartySize; i++)
			{
				GLCharAG *pCharMember = GetChar(pParty->m_Member[i]);

				if (!pCharMember)
				{
					if (i == MASTER_PARTY)
					{
						sc::writeLogError(sc::string::format("GLAgentServer::MsgMatchTestCA - Char(%1%)'s request: Party master does not exist!.", pChar->ChaDbNum()));

						goto feedback_match_request;
					}
					else
						continue;
				}

				msgMatch.arrParty[j].dwChaNum	= pCharMember->ChaDbNum();
				msgMatch.arrParty[j].dwClass	= (DWORD)pCharMember->m_emClass;
				msgMatch.arrParty[j++].dwLevel	= (DWORD)pCharMember->m_wLevel;
			}

			GASSERT(j == msgMatch.dwPartySize);

			msgMatch.dwPartySize = j;
		}
	}

	msgMatch.arrParty[0].dwChaNum	= pChar->ChaDbNum();
	msgMatch.arrParty[0].dwClass	= (DWORD)pChar->m_emClass;
	msgMatch.arrParty[0].dwLevel	= (DWORD)pChar->m_wLevel;

	msgMatch.dwMapID = msg->dwMapID;
	msgMatch.dwSvrGroupNum = (DWORD)m_pMsgServer->GetServerGroup();

	SENDTOMATCH(&msgMatch);

	msgFB.bDone = TRUE;
	
feedback_match_request:
	SENDTOCLIENT(dwClientID, &msgFB);
	
	return msgFB.bDone;
}


BOOL GLAgentServer::MsgMatchCancelCAM(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MATCH_CANCEL_CAM *msg = (GLMSG::NET_MATCH_CANCEL_CAM *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);
	if (!pChar)
		return FALSE;
	
	msg->dwSvrGroupNum = GetSvrGroupNum();
	msg->dwChaNum = pChar->ChaDbNum();

	SENDTOMATCH(nmg);
    return TRUE;
}


BOOL GLAgentServer::MsgMatchCancelCAM_FB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MATCH_CANCEL_CAM_FB *msg = (GLMSG::NET_MATCH_CANCEL_CAM_FB *)nmg;

	GASSERT(msg->dwSvrGroupNum == m_pMsgServer->GetServerGroup());
	if (msg->dwSvrGroupNum != m_pMsgServer->GetServerGroup())
	{
		sc::writeLogError(sc::string::format("(CRITICAL) MsgMatchCancelCAM_FB - SvrGroupNum is different!(%1%/%2%)", msg->dwSvrGroupNum, m_pMsgServer->GetServerGroup()));
		return FALSE;
	}

	GLCharAG *pChar = GetCharByDbNum(msg->dwChaNum);
	if (!pChar)
		return FALSE;

	SENDTOCLIENT(pChar->ClientSlot(), nmg);
    return TRUE;	
}


BOOL GLAgentServer::MsgMatchResultMAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MATCH_REQUEST_RESULT_MAC *msg = (GLMSG::NET_MATCH_REQUEST_RESULT_MAC *)nmg;

	GASSERT(msg->dwSvrGroupNum == m_pMsgServer->GetServerGroup());
	if (msg->dwSvrGroupNum != m_pMsgServer->GetServerGroup())
	{
		sc::writeLogError(sc::string::format("(CRITICAL) MsgMatchResultMAC - SvrGroupNum is different!(%1%/%2%)", msg->dwSvrGroupNum, m_pMsgServer->GetServerGroup()));
		return FALSE;
	}

	GLCharAG *pChar = GetCharByDbNum(msg->dwChaNum);
	if (!pChar)
		return FALSE;

	SENDTOCLIENT(pChar->ClientSlot(), nmg);
    return TRUE;
}


BOOL GLAgentServer::MsgTransferStartMA(NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_TRANSFER_START_MA *msg = (GLMSG::NET_TRANSFER_START_MA *)nmg;

	GASSERT(msg->dwSvrGroupNum == m_pMsgServer->GetServerGroup());
	if (msg->dwSvrGroupNum != m_pMsgServer->GetServerGroup())
	{
		sc::writeLogError(sc::string::format("(CRITICAL) MsgTransferStartMA - SvrGroupNum is different!(%1%/%2%)", msg->dwSvrGroupNum, m_pMsgServer->GetServerGroup()));
		return FALSE;
	}

	GLCharAG *pChar = GetCharByDbNum(msg->dwChaNum);

	if (!pChar)
		return FALSE;

	if(msg->dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(sc::string::format("MsgTransferStartMA - InstanceKey is invalid for the char(%1%)", msg->dwChaNum));
		return FALSE;
	}

	pChar->SetInstanceKey(msg->dwKey);

	//필드서버를 떠나기 전에 캐리터 정보를 이전한다
	GLMSG::NET_TRANSFER_REQUEST_AF msgTransfer;
	msgTransfer.dwInstanceServerNum = msg->dwInstanceServerNum;
	msgTransfer.dwChaNum		= pChar->ChaDbNum();
	msgTransfer.dwSvrGroupNum	= m_pMsgServer->GetServerGroup();
	msgTransfer.dwGaeaID		= pChar->GaeaId();
	msgTransfer.dwMapID			= msg->dwMapID;
	msgTransfer.dwGateID		= msg->dwGateID;

	SENDTOFIELD(pChar->ClientSlot(), &msgTransfer);

	return TRUE;
}


/*
//mjeon.instance.transfer
BOOL GLAgentServer::MsgTransferTestCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//
	//클라이언트는 Instance의 Key값을 전달한다.
	//
	GLMSG::NET_TRANSFER_TEST_CA *msg = (GLMSG::NET_TRANSFER_TEST_CA *)nmg;

	SNATIVEID idMap = msg->idMap;
	DWORD dwGateID = msg->dwGateID;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	GLAGLandMan* pLandManAG = GetLand(idMap);
	if (!pLandManAG)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferTestCA - LandMan(%1%,%2%) does not exist.", idMap.Mid(), idMap.Sid()));

		//여기에서 클라이언트에게 오류 통보해줄 것		
		return FALSE;
	}

	if (dwGateID == MAXDWORD)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferTestCA - Map(%1%,%2%)'s Gate(%3%) does not exist.", idMap.Mid(), idMap.Sid(), dwGateID));

		//여기에서 클라이언트에게 오류 통보해줄 것
		return FALSE;
	}

	SMAPNODE* pMapNode = m_sMapList.FindMapNode(idMap);
	if (!pMapNode)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferTestCA - MapNode(%1%,%2%) does not exist.", idMap.Mid(), idMap.Sid()));

		//여기에서 클라이언트에게 오류 통보해줄 것
		return FALSE;
	}

	DWORD dwNextFieldSvr = pMapNode->dwFieldSID;

	bool bOK = IsFieldOnline(pChar->m_nChannel, dwNextFieldSvr);
	if (!bOK)
	{
		sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferTestCA failure - Target FieldServer(%1%, %2%) is offline.", pChar->m_nChannel, dwNextFieldSvr));

		//여기에서 클라이언트에게 오류 통보해줄 것
		return FALSE;
	}

	//필드를 떠나기 전에 캐리터 정보를 미리 이전한다.
	GLMSG::NET_TRANSFER_REQUEST_AF msgTransfer;
	msgTransfer.dwChaNum= pChar->ChaDbNum();
	msgTransfer.dwMapID	= idMap.dwID;
	msgTransfer.dwGateID= dwGateID;

	SENDTOFIELD(pChar->ClientSlot(), &msgTransfer);

	return TRUE;
}
*/

//mjeon.instance.transfer
BOOL GLAgentServer::MsgTransferCharDoneFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	/*GLMSG::SNET_TRANSFER_CHAR_DONE_FA *pmsg = (GLMSG::SNET_TRANSFER_CHAR_DONE_FA *)nmg;

	GLCharAG *pChar = GetCharByDbNum(pmsg->dwChaNum);

	if (!pChar)
		return FALSE;

	ReActionMoveFieldSvr(pChar->ClientSlot(), dwGaeaID);

	SMAPNODE	*pMAPNODE	= NULL;
	SNATIVEID	idMapDest	= SNATIVEID(pmsg->dwMapID);
	DWORD		dwFieldSvrID= FIELDSERVER_MAX;

	//현재 필드 서버에 자신이 떠나갈 예정임을 통보
	GLMSG::SNETPC_FIELDSVR_OUT msgOut;
	msgOut.dwChaNum	= pChar->ChaDbNum();
	msgOut.dwSvrGroupNum = m_pMsgServer->GetServerGroup();
	msgOut.bTransfer = TRUE;
	msgOut.bToInstance = pmsg->bToInstance;
	msgOut.targetMapID = idMapDest;
	

	//Transfer의 방향이 FieldServer -> InstanceServer인 경우는 떠나갈 예정임을 FieldServer(Source)에 알린다.
	if (pmsg->bToInstance)
	{
		SENDTOFIELD(pChar->ClientSlot(), &msgOut);

		dwFieldSvrID	= pmsg->dwInstanceServerNum;
	}
	else
	{
		SENDTOMATCH(&msgOut);

		pMAPNODE = m_sMapList.FindMapNode(idMapDest);
		if (!pMAPNODE)
		{
			sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferCharDoneFA - Map to return(%1%,%2%) does not exist.", idMapDest.Mid(), idMapDest.Sid()));
			return FALSE;
		}

		GLAGLandMan* pLAND = GetLand(idMapDest);
		if (!pLAND)
		{
			sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferCharDoneFA - Land to return(%1%,%2%) does not exist.", idMapDest.Mid(), idMapDest.Sid()));
			return FALSE;
		}

		DWORD nChannel = pChar->GetChannel();

		if (nChannel != -1)
		{
			if (!IS_FIELDSERVER_ONLINE(nChannel, (UINT32)pMAPNODE->GetFieldServerNum()))
			{
				sc::writeLogError(
                    sc::string::format(
                        "GLAgentServer::MsgTransferCharDoneFA - FieldServer to return(%1%,%2%) if Offline.",
                        nChannel,
                        pMAPNODE->GetFieldServerNum()));
				return FALSE;
			}
		}

		dwFieldSvrID	= pMAPNODE->GetFieldServerNum();
	}

	//이동할 맵 ID 저장
	pChar->SetNextFieldSvr(idMapDest, pmsg->dwGateID, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX), dwFieldSvrID, pmsg->bToInstance);*/
	
	return TRUE;
}

//mjeon.instance.transfer
BOOL GLAgentServer::MsgTransferChar(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_TRANSFER_CHAR *pmsg = (GLMSG::SNET_TRANSFER_CHAR *)nmg;

	sc::writeLogDebug(sc::string::format("Received TransferChar(%1%/%2%)", pmsg->wSeq, pmsg->wLastSeq));

	GLCharAG *pCharAG = NULL;

	if (pmsg->wOnlye4DBSave)
	{
		//InstaceServer에서 DropOut하면서 발생한 DB저장용 transfer일 경우, 여기에서 캐릭터가 존재하지 않을 수 있다.
		//그러므로 무조건 0번 채널로 보내서 DB save 시키자.

		GASSERT(pmsg->IsToInstance() == FALSE);
	}
	else
	{
		pCharAG = GetCharByDbNum(pmsg->dwChaNum);

		if (!pCharAG)
			return FALSE;
	}

	if (pmsg->IsToInstance())
	{
		SENDTOMATCH(pmsg);
	}
	else
	{
		//
		// 복귀할 mapID
		//
		SNATIVEID idMapDest	= SNATIVEID(pmsg->dwMapID);

		SMAPNODE *pMAPNODE = m_sMapList.FindMapNode(idMapDest);
		if (!pMAPNODE)
		{
			sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferChar - Map to return(%1%,%2%) does not exist.", idMapDest.Mid(), idMapDest.Sid()));
			return FALSE;
		}

		GLAGLandMan* pLAND = GetLand(idMapDest);
		if (!pLAND)
		{
			sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferChar - Land to return(%1%,%2%) does not exist.", idMapDest.Mid(), idMapDest.Sid()));
			return FALSE;
		}

		DWORD nChannel = pmsg->wOnlye4DBSave ? 0 : pCharAG->GetChannel();

		if (nChannel != -1)
		{
			if (!IS_FIELDSERVER_ONLINE(nChannel, (UINT32)pMAPNODE->GetFieldServerNum()))
			{
				sc::writeLogError(sc::string::format("GLAgentServer::MsgTransferChar - FieldServer to return(%1%,%2%) if Offline.", nChannel, pMAPNODE->GetFieldServerNum()));
				return FALSE;
			}
		}

		SENDTOFIELD(nChannel, pMAPNODE->GetFieldServerNum(), pmsg);
	}
	
	return TRUE;
}


//
//mjeon.groupchat
//
BOOL GLAgentServer::MsgChatInviteCAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_GCHAT_INVITE_CAC *msg = (GLMSG::SNET_GCHAT_INVITE_CAC *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	msg->dwKey = pChar->ChaDbNum();
	StringCchCopy(msg->szChaNameOwner, sizeof(msg->szChaNameOwner), pChar->m_szName);

	GLCharAG *pCharWho = GetCharByDbNum(msg->dwChaNumWhom);

	if (!pCharWho)
	{
		return FALSE;
	}

	const SMAPNODE* pMapNode = FindMapNode( pChar->GetCurrentMap().getBaseMapID());

	if( pMapNode && PRISONLIMIT.IsLimitStatus(pMapNode, pChar->m_dwUserLvl, PML_CHATTING ) ) {
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = pChar->ChaDbNum();
		msg.code  = CHAT_CODE_PRISON;
		SENDTOCLIENT(pChar->ClientSlot(), &msg);

		return INVALID_CHAT_KEY;
	}

	pMapNode = FindMapNode( pCharWho->GetCurrentMap().getBaseMapID());

	if( pMapNode && PRISONLIMIT.IsLimitStatus(pMapNode, pCharWho->m_dwUserLvl, PML_CHATTING ) ) {
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = pChar->ChaDbNum();
		msg.code  = CHAT_CODE_OTHER_PRISON;
		SENDTOCLIENT(pChar->ClientSlot(), &msg);

		return INVALID_CHAT_KEY;
	}

	if (m_pGroupChatMan->CheckLevel(pChar->GetChaLevel()) == FALSE)
	{
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = pChar->ChaDbNum();
		msg.code  = CHAT_CODE_OWNER_LOW_LEVEL;
		SENDTOCLIENT(pChar->ClientSlot(), &msg);

		return INVALID_CHAT_KEY;
	}

	if (m_pGroupChatMan->CheckLevel(pCharWho->GetChaLevel()) == FALSE)
	{
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = pChar->ChaDbNum();
		msg.code  = CHAT_CODE_MEMBER_LOW_LEVEL;
		SENDTOCLIENT(pChar->ClientSlot(), &msg);	//초대한 주체에게 에러를 발송한다.
		
		return INVALID_CHAT_KEY;
	}

	BOOL bRet = m_pGroupChatMan->Invite(pChar->ChaDbNum(), pCharWho->ChaDbNum());

	if (bRet)
	{
		SENDTOCLIENT(pCharWho->ClientSlot(), nmg);	//relay to the target client
	}

    return TRUE;
}

BOOL GLAgentServer::MsgChatInviteCAC_FB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_GCHAT_INVITE_CAC_FB *msg = (GLMSG::SNET_GCHAT_INVITE_CAC_FB *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	if (msg->dwKey == INVALID_CHAT_KEY)
	{
		sc::writeLogError(stringformat("GLAgentServer::MsgChatInviteCAC_FB - Char(%1%) responsed with invalid key", pChar->ChaDbNum()));
		return FALSE;
	}

	msg->dwChaWhom = pChar->ChaDbNum();

	GLCharAG *pCharOwner = GetCharByDbNum(msg->dwKey);

	if (!pCharOwner)
	{
		sc::writeLogWarn(stringformat("GLAgentServer::MsgChatInviteCAC_FB - Chat owner char(%1%) does not exist", msg->dwKey));
		return FALSE;
	}

	if (msg->bAccept)
	{
		DWORD dwKey = m_pGroupChatMan->Accept(pChar->ChaDbNum(), msg->dwKey);

		msg->bAcceptDone = (dwKey != INVALID_CHAT_KEY);
	}

	SENDTOCLIENT(pCharOwner->ClientSlot(), nmg);	//relay to the chat owner

    return TRUE;
}

BOOL GLAgentServer::MsgChatQuitCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_GCHAT_QUIT_CA *msg = (GLMSG::SNET_GCHAT_QUIT_CA *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	if (msg->dwKey == INVALID_CHAT_KEY)
	{
		sc::writeLogError(stringformat("GLAgentServer::MsgChatQuitCA - Char(%1%) requested with invalid key", pChar->ChaDbNum()));
		return FALSE;
	}

	m_pGroupChatMan->Quit(pChar->ChaDbNum(), msg->dwKey);

    return TRUE;
}

BOOL GLAgentServer::MsgChatKickCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_GCHAT_KICK_CA *msg = (GLMSG::SNET_GCHAT_KICK_CA *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	if (msg->dwChaNum == INVALID_CHANUM)
	{
		sc::writeLogError(std::string("GLAgentServer::MsgChatKickCA - ChaNum is invalid"));
		return FALSE;
	}

	if (pChar->ChaDbNum() == msg->dwChaNum)
	{
		sc::writeLogError(std::string("GLAgentServer::MsgChatKickCA - Cannot kickout owner self."));
		return FALSE;
	}

	m_pGroupChatMan->KickOut(msg->dwChaNum, pChar->ChaDbNum());

    return TRUE;
}

BOOL GLAgentServer::MsgChatMessageCAC(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_GCHAT_MESSAGE_CAC *msg = (GLMSG::SNET_GCHAT_MESSAGE_CAC *)nmg;

	GLCharAG *pChar = GetChar(dwGaeaID);

	if (!pChar)
		return FALSE;

	msg->dwChaSay = pChar->ChaDbNum();

	m_pGroupChatMan->SendToRoom(pChar->ChaDbNum(), msg->dwKey, nmg);
    return TRUE;
}


//mjeon
//
//Process PingReq & Res
//
BOOL GLAgentServer::ProcessPingMsg( NET_MSG_GENERIC *nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_PING *pMsgPing = (GLMSG::SNET_GM_PING *)nmg;		
	GLMSG::SNET_GM_PING MsgPingFB;

	if ( pMsgPing->nType == NET_MSG_GM_PING )
	{
		MsgPingFB.nType = NET_MSG_GM_PING_FB;
		MsgPingFB.dwSent = pMsgPing->dwSent;
		MsgPingFB.dwRecvd = GetTickCount();
		MsgPingFB.bServerType = 0;	//AgentServer

		SENDTOCLIENT ( dwClientID, &MsgPingFB );
	}

	return TRUE;
}

// *****************************************************
// Desc: 대련 불가능한 지역 여부 / GM 명령어
// *****************************************************
BOOL GLAgentServer::MsgGmNonConfrontMode ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_NONCONFRONT_MODE *pNetMsg = (GLMSG::SNET_GM_NONCONFRONT_MODE *) nmg;

	GLCharAG* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	if ( pChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	SNONCONFRONTLIST* pConfrontList = m_cAGPeriod.GetNonConfrontList();
	if ( !pConfrontList )
		return FALSE;

	INT32 nChannel = pNetMsg->nChannel;

	if ( pNetMsg->bThisChannel )
	{
		nChannel = pChar->m_nChannel;
	}

	if ( pNetMsg->bMode )
	{
		if ( pNetMsg->sMapID == NATIVEID_NULL() )
		{
			pNetMsg->sMapID = pChar->GetCurrentMap().getGaeaMapID();
		}

		SMAPNODE* pMAPNODE = m_sMapList.FindMapNode ( pNetMsg->sMapID );
		if ( !pMAPNODE )
			return FALSE;

		GLAGLandMan* pLAND = GetLand ( pNetMsg->sMapID );
		if (!pLAND)
            return FALSE;

		if (nChannel != -1)
		{
			if (!IS_FIELDSERVER_ONLINE(nChannel, (UINT32) pMAPNODE->GetFieldServerNum()))
			{
				return FALSE;
			}
		}

		pConfrontList->Insert(this, pNetMsg->sMapID, nChannel, pNetMsg->dwPeriod);
	
		if ( nChannel != -1 )
		{
			SENDTOCHANNEL( pNetMsg, nChannel );
		}
		else
		{
			SENDTOALLCHANNEL( pNetMsg );
		}
	}
	else
	{
		if ( pNetMsg->sMapID == NATIVEID_NULL() )
		{
			if ( pConfrontList->sPeriodList.size() == 0 )
				return FALSE;

			SNONCONFRONTLIST::SNONCONFRONT& sNonConfront = pConfrontList->sPeriodList.back();
			pNetMsg->sMapID = sNonConfront.sMAPID;

			pConfrontList->Delete_Back();
		}
		else
		{
			SMAPNODE* pMAPNODE = m_sMapList.FindMapNode ( pNetMsg->sMapID );
			if ( !pMAPNODE )
				return FALSE;

			GLAGLandMan* pLAND = GetLand ( pNetMsg->sMapID );
			if ( !pLAND )
				return FALSE;

			pConfrontList->Delete( pNetMsg->sMapID, nChannel );
		}

		if ( nChannel != -1 )
		{
			SENDTOCHANNEL( pNetMsg, nChannel );
		}
		else
		{
			SENDTOALLCHANNEL( pNetMsg );
		}
	}

	return TRUE;
}

// *****************************************************
// Desc: 대련 불가능한 지역 리스트 요청 / GM 명령어
// *****************************************************
BOOL GLAgentServer::MsgGmNonConfrontModeViewReq ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_NONCONFRONT_MODE_VIEW_REQ *pNetMsg = (GLMSG::SNET_GM_NONCONFRONT_MODE_VIEW_REQ *) nmg;

	GLCharAG* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	if ( pChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	SNONCONFRONTLIST* pConfrontList = m_cAGPeriod.GetNonConfrontList();
	if ( !pConfrontList )
		return FALSE;

	GLMSG::SNET_GM_NONCONFRONT_MODE_VIEW_FB NetMsgFB;

	SNONCONFRONTLIST::LIST_ITER iter    = pConfrontList->sPeriodList.begin();
	SNONCONFRONTLIST::LIST_ITER iterEnd = pConfrontList->sPeriodList.end();
	
	WORD wIndex = 0;
	for ( ; iter!=iterEnd; ++iter )
	{
		SNONCONFRONTLIST::SNONCONFRONT& sNonConfront = (*iter);

		NetMsgFB.sInfo[wIndex].sMAPID   = sNonConfront.sMAPID;
		NetMsgFB.sInfo[wIndex].nCHANNEL = sNonConfront.nCHANNEL;

		if ( sNonConfront.bUNLIMITED )
		{
			NetMsgFB.sInfo[wIndex].dwAPPLYTIME    = -1;
			NetMsgFB.sInfo[wIndex].dwREMAINEDTIME = -1;
		}
		else
		{
			NetMsgFB.sInfo[wIndex].dwAPPLYTIME    = sNonConfront.dwENDTIME - sNonConfront.dwBEGINTIME;
			NetMsgFB.sInfo[wIndex].dwREMAINEDTIME = sNonConfront.dwENDTIME - sNonConfront.dwCURTIME;
		}

		++wIndex;

		if ( GM_NONCONFRONT_MODE_VIEW_MAX <= wIndex )
		{
			break;
		}
	}

	NetMsgFB.wSize = wIndex;
	SENDTOCLIENT ( dwClientID, &NetMsgFB );

	return TRUE;
}

/*
BOOL GLAgentServer::TraceFilter(EMNET_MSG nType)
{
	switch(nType)	//mjeon.AF.test
	{
		case NET_MSG_LOBBY_CHAR_JOIN:
		case NET_MSG_LOBBY_CHAR_JOIN_FB:
		case NET_MSG_LOBBY_CHAR_ITEM:
		case NET_MSG_LOBBY_CHAR_SKILL:
		case NET_MSG_LOBBY_QUEST_END:
		case NET_MSG_LOBBY_QUEST_PROG:
		case NET_MSG_LOBBY_CLUB_INFO:
		case NET_MSG_LOBBY_CLUB_ALLIANCE:
		case NET_MSG_LOBBY_CHAR_PUTON_EX:
		case NET_MSG_LOBBY_ITEM_COOLTIME:
		case NET_MSG_LOBBY_CLUB_MEMBER:
		case NET_MSG_LOBBY_CLUB_BATTLE:
		case NET_MSG_LOBBY_PRODUCTBOOK:
		case NET_MSG_GCTRL_DROP_OUT_FORCED:
		case MET_MSG_GAME_JOIN_FIELDSVR_FB:		
		case NET_MSG_FIELDSVR_CHARCHK_FB:
		case NET_MSG_FIELDSVR_OUT_FB:
		case NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ:
		case NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB:
		case NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL:
		case NET_MSG_GCTRL_REQ_GATEOUT:
		case NET_MSG_GCTRL_REQ_GATEOUT_FB:
		case NET_MSG_ATTENDANCE_RESET_FAC:
		case NET_MSG_ATTENDANCE_UPGRADE_FAC:
		case NET_MSG_ATTENDANCE_SYNC_FAC:
		case NET_MSG_CTF_PREPARE_AF_FB:
		case NET_MSG_CTF_DONE_AF_FB:
		case NET_MSG_CTF_END_FA:
		case NET_MSG_CTF_RANKING_FAC:
		case NET_MSG_LOG_UPDATE_TRACING_CHARACTER:
		case NET_MSG_GCTRL_REQ_REBIRTH_FB:
		case NET_MSG_GCTRL_CONFRONT_END2_AGT:
		case NET_MSG_GM_LIMIT_EVENT_RESTART:
		case NET_MSG_GCTRL_ACTSTATE:
		case NET_MSG_GCTRL_CLUB_NEW_2AGT:
		case NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT:
		case NET_MSG_GCTRL_CLUB_RANK_2AGT:
		//case NET_MSG_GCTRL_CLUB_INCOME_UP:
		case NET_MSG_GCTRL_CLUB_INFO_2CLT:
		case NET_MSG_GCTRL_CLUB_MEMBER_2CLT:
		case NET_MSG_GCTRL_REQ_SERVER_CLUB_BATTLE_REMAIN_AG:
		case NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG:
		case NET_MSG_GCTRL_INVEN_DELETE:
		case NET_MSG_GCTRL_PUTON_CHANGE_AG:
		case NET_MSG_BUY_REBUY_ITEM_FA:
        case NET_MSG_ADD_REBUY_ITEM_FA:
		case NET_MSG_GCTRL_INVEN_PREMIUMSET_FB:
		case NET_MSG_GCTRL_INVEN_RENAME_AGTBRD:
		case NET_MSG_GCTRL_UPDATE_STARTCALL:		
		case NET_MSG_GCTRL_REQ_RECALL_AG:
		case NET_MSG_GCTRL_REQ_RECALL_FB:
		case NET_MSG_GCTRL_REGEN_GATE_FB:
		case NET_MSG_GCTRL_CONFRONT_FB:
		//case NET_MSG_SMS_PHONE_NUMBER_AGTBRD:
		case NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT:
		case NET_MSG_CHAT_LOUDSPEAKER_AGT:		
		case NET_MSG_GET_CHA_EXTRA_INFO_FH_FB:
		case NET_MSG_CHAR_POINT_FA:
		case NET_MSG_GCTRL_QUEST_PROG_MOBKILL:
		case NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT:
		case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT:
		case NET_MSG_GCTRL_CLUB_DEATHMATCH_END_AGT:
		case NET_MSG_SERVER_GENERALCHAT:
		case NET_MSG_ACTIVITY_SYNC:
		case NET_MSG_ACTIVITY_REWARD_POINT_SYNC:
		case NET_MSG_ACTIVITY_REWARD_BADGE_SYNC:
		case NET_MSG_ACTIVITY_COMPLETED_FA:
		case NET_MSG_ACTIVITY_CLOSED_FA:
		case NET_MSG_ATTENDANCE_DAILYMAINTENANCE_READY_FA:
		case NET_MSG_POST_OPEN_TAKE_AF_FB:
		case NET_MSG_POST_NOTIFY:
		case NET_MSG_GCTRL_NPC_COMMISSION:
		case NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT:
		case NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT:
		case NET_MSG_GCTRL_2_FRIEND_AG:
		case NET_MSG_GCTRL_REQ_TELEPORT_AG:
		case NET_MSG_GCTRL_REQ_TELEPORT_FB:
		case NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB:
		case NET_MSG_CHAT_LINK_FB:		
		case NET_MSG_REQ_MUST_LEAVE_MAP_AG:
		case NET_MSG_GCTRL_TRADE_TAR:
		case NET_MSG_GCTRL_TRADE_FB:
		case NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR:
		case NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR:
		case NET_MSG_GCTRL_TRADE_LOCK_TAR:
		case NET_MSG_GCTRL_TRADE_AGREE_TAR:
		case NET_MSG_GCTRL_TRADE_COMPLETE_TAR:		
		case NET_MSG_CTF_JOIN_FA:
		case NET_MSG_USER_CHAR_INFO_FLD_FB:
		case NET_MSG_GM_CHAR_INFO_FLD_FB:
		case NET_MSG_GM_MOVE2CHAR_AG:
		case NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT:
		case NET_MSG_CLUB_CD_CERTIFY_ING_BRD:
		case NET_MSG_CLUB_CERTIFIED_FA:
		case NET_MSG_GCTRL_REQ_GENITEM_AGT:
		case NET_MSG_GCTRL_INVEN_RENAME_FROM_DB:
		case NET_MSG_CHARPOS_FROMDB2AGT:
		case NET_MSG_GCTRL_TRADE:
		case NET_MSG_GCTRL_TRADE_TAR_ANS:
		case NET_MSG_GCTRL_REGEN_GATE:
		case NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE:
		case NET_MSG_GCTRL_WITH_VEHICLE:
		case NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS:
        case NET_MSG_CLUB_COUNTRY_COUNT_CA:
		case NET_MSG_TRANSFER_CHAR:
		case NET_MSG_TRANSFER_CHAR_DONE_FA:
			return TRUE;

		default:
			return FALSE;
	}
}
*/

BOOL GLAgentServer::MsgProcess(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( NULL == m_MsgFunc[nmg->nType].Msgfunc )
	{
		return FALSE;
	}
	else
	{
		if(dwClientID == CLIENTID_NULL)
		{
			sc::writeLogError(sc::string::format("(GLAgentServer)ClientID is NULL for the msg(%1%)", nmg->nType));
		}

		/*
		if(dwGaeaID == GAEAID_NULL)
		{
			//mjeon.AF.test
			if (!TraceFilter(nmg->nType))
			{
				sc::writeLogError(sc::string::format("(GLAgentServer)GaeaID is NULL for the msg(%1%)", nmg->nType));
			}
		}
		*/

		(this->*m_MsgFunc[nmg->nType].Msgfunc)(nmg, dwClientID, dwGaeaID);
		return TRUE;
	}
}

//CA
BOOL GLAgentServer::MsgActState(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNETPC_ACTSTATE *msg = (GLMSG::SNETPC_ACTSTATE *)nmg;
	
	if(!msg)
		return FALSE;

	GLCharAG *pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
	{
		return FALSE;
	}
	pMyChar->MsgActState(nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgReqFriendList(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (pMyChar)
	{
        pMyChar->MsgReqFriendList(nmg);
        return TRUE;		
	}
    else
    {
        return FALSE;
    }	
}

BOOL GLAgentServer::MsgReqFriendAdd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqFriendAdd(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgReqFriendAddAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqFriendAddAns(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgReqFriendDel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqFriendDel(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgReqFriendBlock(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqFriendBlock(nmg);
	return TRUE;
}


BOOL GLAgentServer::MsgClubMemberReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		sc::writeLogError( "The WorldBattle Server don't request to join club." );

		return S_FALSE;
	}

	GLMSG::NET_CLUB_MEMBER_REQ_CA *pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_CA *) nmg;
	GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);

	GLCharAG* pMyChar = GetChar(dwGaeaID);
	GLCharAG* pTarChar = GetChar(pNetMsg->m_TarGaeaID);
	// 같은 맵에서 클럽 초대시 GaeaID로 찾을 수 있으나;
	// 다른 맵인 경우 이름으로 찾는다;
	if ( !pTarChar )
	{
		pTarChar = GetChar(pNetMsg->m_szChaName);
	}

	if ( !pMyChar || !pTarChar )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	NetMsgFB.SetTarChaName(pTarChar->m_szName);

	//! CTF 참여중이면 취소
	if ( pTarChar->m_bCTFPlayer )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	// 다른 학원생과 클럽 결성 제한 옵션이 있을 경우 제한함.
	if ( !GLCONST_CHAR::bCLUB_2OTHERSCHOOL )
	{
		if ( pTarChar->m_wSchool!= pMyChar->m_wSchool )
		{
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL;
			SENDTOCLIENT(dwClientID,&NetMsgFB);
			return S_FALSE;
		}
	}

	// 국가체크
	if ( pMyChar->GetCountry() != pTarChar->GetCountry())
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

//	std::tr1::shared_ptr<GLClubField> pClub = GetMyClub();
	std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(pMyChar->GetClubDbNum());

	if (!pClub)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( pClub->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( !pClub->HaveMemberJoinAuthority( pMyChar->m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	bool bFOUND = pClub->IsMember(pTarChar->m_CharDbNum);
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pClub->m_dwRank);
	if ( pClub->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}	
	if ( pTarChar->m_ClubDbNum!=CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}


	GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC NetMsgAsk;
	NetMsgAsk.SetClubName(pClub->Name());
	NetMsgAsk.dwMaster = pMyChar->ChaDbNum();
	NetMsgAsk.SetMasterName(pClub->GetMasterName());

	SENDTOCLIENT(pTarChar->ClientSlot(), &NetMsgAsk);

	return TRUE;
}

BOOL GLAgentServer::MsgClubMemberAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_CLUB_MEMBER_REQ_ANS_CA *pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_ANS_CA *) nmg;

	GLCharAG* pMyChar = GetChar(dwGaeaID);
	GLCharAG* pMaster = GetCharByDbNum(pNetMsg->dwMaster);
//	if ( !pMyChar || !pMaster)
//		return S_FALSE;

	//GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);
	//NetMsgFB.SetTarChaName(pMyChar->m_szName);

	//  [1/3/2013 gbgim]
	if ( !pMyChar )
		return S_FALSE;

	GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);
	NetMsgFB.SetTarChaName(pMyChar->m_szName);

	if( !pMaster )
	{
		if( pNetMsg->bOK )
		{
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_NONEXISTENT;
			SENDTOCLIENT(pMyChar->m_ClientSlot,&NetMsgFB);
		}
		return S_FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	if (pMyChar->m_ClubDbNum != CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(pMaster->m_ClientSlot,&NetMsgFB);
		return S_FALSE;
	}

	if ( !pNetMsg->bOK )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_REFUSE;
		SENDTOCLIENT(pMaster->m_ClientSlot,&NetMsgFB);
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub(pMaster->m_ClubDbNum);
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(pMaster->m_ClientSlot,&NetMsgFB);
		return S_FALSE;
	}

	if ( pCLUB->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		SENDTOCLIENT(dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( !pCLUB->HaveMemberJoinAuthority(pMaster->m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SENDTOCLIENT(pMaster->m_ClientSlot,&NetMsgFB);
		return S_FALSE;
	}

	bool bFOUND = pCLUB->IsMember(pMyChar->m_CharDbNum);
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		SENDTOCLIENT(pMaster->m_ClientSlot,&NetMsgFB);
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pCLUB->m_dwRank);
	if ( pCLUB->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		SENDTOCLIENT(pMaster->m_ClientSlot,&NetMsgFB);
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_MEMBER_REQ_2AGT NetMsgAgt;
	NetMsgAgt.dwChaNum = pMaster->ChaDbNum();
	NetMsgAgt.dwReqGID = pMyChar->m_dwGaeaID;
//	SendToAgent(&NetMsgAgt);
	return MsgClubMemberReqAgt(&NetMsgAgt,dwClientID,dwGaeaID);
}

BOOL GLAgentServer::MsgClubMemberReqAgt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_CLUB_MEMBER_REQ_2AGT* pPacket = (GLMSG::SNET_CLUB_MEMBER_REQ_2AGT *)nmg;

	//GLCharAG* pMyChar = GetChar(dwGaeaID);
	GLCharAG* pMyChar = GetCharByDbNum(pPacket->dwChaNum);
	if (!pMyChar)
        return FALSE;

    DWORD MyClientSlot = pMyChar->ClientSlot();

    GLCharAG* pTarChar = GetChar(pPacket->dwReqGID);
    if (!pTarChar)
    {
        GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SENDTOCLIENT(MyClientSlot, &NetMsgFB);
        return FALSE;
    }

    WORD TarChaLevel = pTarChar->GetChaLevel();
    DWORD TarChaDbNum = pTarChar->ChaDbNum();
    std::string TargetChaName(pTarChar->m_szName);
    
    // 탈퇴후 유보시간 점검.
    if (pTarChar->m_tSECEDE != 0)
    {
        CTime cSECEDE(pTarChar->m_tSECEDE);
        CTimeSpan sSpan(GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY,0,10,0);

        cSECEDE += sSpan;

        CTime cCurTime = CTime::GetCurrentTime();
        if (cCurTime < cSECEDE)
        {
            GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);
            NetMsgFB.SetTarChaName(TargetChaName.c_str());
            NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_SECEDED;
            SENDTOCLIENT(MyClientSlot, &NetMsgFB);
            return FALSE;
        }
    }

    DWORD ClubDbNum = pMyChar->GetClubDbNum();
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(ClubDbNum);
    if (!spClub)
    {
        GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);
        NetMsgFB.SetTarChaName(TargetChaName.c_str());
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SENDTOCLIENT(MyClientSlot, &NetMsgFB);
        return FALSE;
    }

    if (!spClub->HaveMemberJoinAuthority(pMyChar->ChaDbNum()))
    {
        GLMSG::NET_CLUB_MEMBER_REQ_FB_AC NetMsgFB(false);
        NetMsgFB.SetTarChaName(TargetChaName.c_str());
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SENDTOCLIENT(MyClientSlot, &NetMsgFB);
        return FALSE;
    }

    bool bResult = ClubMemberAdd(
        std::string(pMyChar->GetChaName()),
        spClub,
        MyClientSlot,
        TargetChaName,
        TarChaDbNum,
        TarChaLevel,
        false,
        sc::time::GetCurrentTime(),
        pTarChar->m_emClass);
    if (bResult)
    {        
        pTarChar->SetClubDbNum(ClubDbNum);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgClubMemberDel(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (pMyChar)
	{
        pMyChar->MsgClubMemberDel(nmg);
        return TRUE;		
	}
    else
    {
        return FALSE;
    }	
}

BOOL GLAgentServer::MsgClubMemberSecede(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (pMyChar)
	{
        pMyChar->MsgClubMemberSecede(nmg);
        return TRUE;		
	}
    else
    {
        return FALSE;
    }	
}

BOOL GLAgentServer::MsgClubAuthorityReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubAuthorityReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubAuthorityReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)	
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubAuthorityReqAns(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubMarkInfo(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubMarkInfo(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubMarkChange(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubMarkChange(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubRank2Agt(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_CLUB_RANK_CHANGE_FA* msg = (GLMSG::NET_CLUB_RANK_CHANGE_FA*) nmg;

	//GLCharAG* pMyChar = GetChar(dwGaeaID);
	GLCharAG* pMyChar = GetCharByDbNum(msg->dwChaNum);
	if (pMyChar)
	{
	    pMyChar->MsgClubRank2Agt(nmg);
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgClubGuidCommission(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (pMyChar)
    {
        pMyChar->MsgClubGuidCommission(nmg);
        return TRUE;
    }
    else
    {
		return FALSE;
    }
}

BOOL GLAgentServer::MsgClubNoticeReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::SNET_CLUB_NOTICE_REQ* pPacket = (GLMSG::SNET_CLUB_NOTICE_REQ*) nmg;
	GLCharAG* pChar = GetChar(dwGaeaID);
	if (pChar)
	{
        //pMyChar->MsgClubNoticeReq(nmg);
        ClubNoticeChange(
            pChar->GetClubDbNum(),
            pChar->ChaDbNum(),
            pChar->ClientSlot(),
            pChar->GetChaName(),
            std::string(pPacket->szNotice));
        return TRUE;
	}
    else
    {
	    return FALSE;
    }
}
/*
BOOL GLAgentServer::MsgClubSubMaster(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (pMyChar)
	{
	    pMyChar->MsgClubSubMaster(nmg);
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}
*/

BOOL GLAgentServer::MsgClubAllianceDelReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubAllianceDelReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubAllianceSecReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubAllianceSecReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubAllianceDisReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (pMyChar)
	{
	    pMyChar->MsgClubAllianceDisReq(nmg);
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}
BOOL GLAgentServer::MsgClubBattleReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubBattleReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubBattleReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubBattleReqAns(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubBattleReqAnsAlliance(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	//pMyChar->MsgClubBattleReqAnsAlliance(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubBattleArmisticeReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubBattleArmisticeReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubBattleArmisticeReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubBattleArmisticeReqAns(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgClubBattleSubmissionReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubBattleSubmissionReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgAllianceBattleReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgAllianceBattleReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgAllianceBattleReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgAllianceBattleReqAns(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgAllianceBattleArmisticeReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgAllianceBattleArmisticeReq(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgAllianceBattleArmisticeReqAns(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgAllianceBattleArmisticeReqAns(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgAllianceBattleSubmissionReq(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgAllianceBattleSubmissionReq(nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgRenameBrd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNETPC_INVEN_RENAME_AGTBRD* pPacket = (GLMSG::SNETPC_INVEN_RENAME_AGTBRD *)nmg;

	GLCharAG* pMyChar = GetCharByDbNum(pPacket->dwChaNum);	
	if (pMyChar)
	{
	    pMyChar->MsgRenameBrd(nmg);
	    return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*
BOOL GLAgentServer::MsgPhoneNumberBrd(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNETPC_PHONE_NUMBER_AGTBRD *msg = (GLMSG::SNETPC_PHONE_NUMBER_AGTBRD *)nmg;
	GLCharAG* pMyChar = GetCharByDbNum(msg->dwChaNum);
	if (pMyChar)
	{
        pMyChar->MsgPhoneNumberBrd(nmg);
        return TRUE;
	}
    else
    {
        return FALSE;
    }	
}
*/

BOOL GLAgentServer::MsgUpdateStartMap(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNETPC_UPDATE_STARTCALL *msg = (GLMSG::SNETPC_UPDATE_STARTCALL *)nmg;

	GLCharAG* pMyChar = GetCharByDbNum(msg->dwChaNum);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgUpdateStartMap(nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgChangeArm( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_PUTON_CHANGE_AG *msg = (GLMSG::SNETPC_PUTON_CHANGE_AG *)nmg;

	GLCharAG* pMyChar = GetCharByDbNum(msg->dwChaNum);
	if (pMyChar)
	{
        pMyChar->MsgChangeArm(nmg);
        return TRUE;		
	}
    else
    {
        return FALSE;
    }	
}
/*
BOOL GLAgentServer::MsgFriendClubWindowOpen( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgFriendClubWindowOpen(nmg);
	return TRUE;
}
*/

BOOL GLAgentServer::MsgReqQboxOption( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqQboxOption(nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgReqAttendList( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqAttendList(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgReqAttendance( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgReqAttendance(nmg);
	return TRUE;
}
BOOL GLAgentServer::MsgWhisperState( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgWhisperState(nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgAddRebuyItem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    if ( dwGaeaID != GAEAID_NULL )
        return FALSE;

    GLMSG::SNET_ADD_REBUY_ITEM_FA* msg = (GLMSG::SNET_ADD_REBUY_ITEM_FA*) nmg;
    GLCharAG* pMyChar = GetCharByDbNum(msg->dwChaNum);
    if (pMyChar)
    {
        pMyChar->MsgAddRebuyItem(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgCharPosFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    if (dwGaeaID != GAEAID_NULL)
        return FALSE;
    NET_CHAR_POS_FA* pPacket = (NET_CHAR_POS_FA*) nmg;
    GLCharAG* pChar = GetCharByDbNum(pPacket->m_ChaDbNum);
    if (pChar)
    {
        std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(pChar->GetClubDbNum());
		const D3DXVECTOR3 vPosition(pPacket->m_fPosX, pPacket->m_fPosY, pPacket->m_fPosZ);
        if (spClub)
        {
            spClub->SetMemberPos(pChar->ChaDbNum(), vPosition);
        }
        pChar->SetMapPosition(vPosition, true);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgBuyRebuyItem(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pMyChar = GetChar(dwGaeaID);
    if (pMyChar)
    {
        pMyChar->MsgBuyRebuyItem(nmg);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLAgentServer::MsgBuyRebuyItemFB(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_BUY_REBUY_ITEM_FA *msg = (GLMSG::SNET_BUY_REBUY_ITEM_FA *)nmg;

	GLCharAG* pMyChar = GetCharByDbNum(msg->dwChaNum);
    //GLCharAG* pMyChar = GetChar(dwGaeaID);
    if ( !pMyChar )
    {
        return FALSE;
    }

    pMyChar->MsgBuyRebuyItemFB( nmg );

    return TRUE;
}

BOOL GLAgentServer::MsgCheckRebuySupplies(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::SNET_CHECK_REBUY_SUPPLIES *msg = (GLMSG::SNET_CHECK_REBUY_SUPPLIES *)nmg;

    GLCharAG* pMyChar = GetChar(dwGaeaID);
    if ( !pMyChar )
    {
        return FALSE;
    }

    pMyChar->MsgCheckRebuySupplies( nmg );

    return TRUE;
}

BOOL GLAgentServer::MsgClubId2MasterNameReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if ( !pMyChar )
	{
		return FALSE;
	}
	pMyChar->MsgClubId2MasterNameReqCA(nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgReferCharRequest( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	ReferChar::SNET_MSG_REFER_CHAR_REQ* pNetMsg = (ReferChar::SNET_MSG_REFER_CHAR_REQ*)nmg;

	GLCharAG* pChar = GetCharByDbNum( pNetMsg->m_nRequestCharDbNum );
	if( !pChar )
		return FALSE;
	
	GLCharAG* pTarget = GetChar( pNetMsg->m_szTargetName );
	if( pTarget )
	{
		// 상대 정보 보기는 유저 GM도 가능하도록 변경 (비즈니스팀 요청사항)
        if( pChar->m_dwUserLvl >= USER_USER_GM )
            pNetMsg->m_bForce = true;
        else
            pNetMsg->m_bForce = false;
        
		SENDTOFIELDSVR( pTarget->m_nChannel, pTarget->m_dwCurFieldSvr, pNetMsg );
	}
	else
	{
		ReferChar::SNET_MSG_REFER_CHAR_FB NetMsg;
		NetMsg.SetTargetName( pNetMsg->m_szTargetName );
		NetMsg.m_eResult = ReferChar::EReferLogout;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsg );
	}

	return TRUE;
}

BOOL GLAgentServer::MsgReferCharRelay( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	ReferChar::SNET_MSG_REFER_CHAR_BASE* pNetMsg = (ReferChar::SNET_MSG_REFER_CHAR_BASE*)nmg;

	GLCharAG* pChar = GetCharByDbNum( pNetMsg->m_nRequestCharDbNum );
	if( !pChar )
		return FALSE;

	SENDTOCLIENT( pChar->ClientSlot(), nmg );

	return TRUE;
}


BOOL GLAgentServer::MsgClubMasterMandate(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::SNET_CLUB_MASTER_MANDATE_CA* pNetMsg = (GLMSG::SNET_CLUB_MASTER_MANDATE_CA*)nmg;
	SetClubMasterMandate( pNetMsg->m_bBool );
	return TRUE;
}

BOOL GLAgentServer::MsgGmNetMsgDelayCA( NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID )
{
	GLCharAG* pChar = GetCharByGaeaID( GaeaID );
	if (!pChar)
		return FALSE;

	if (pChar->UserType() < USER_GM3)
		return FALSE;

	SENDTOALLCHANNEL( nmg );
	return TRUE;
}

BOOL GLAgentServer::MsgChaSaveFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	// AgentServerMsg.cpp case NET_MSG_CHA_SAVE_FA: 에서 처리됨
// 	GLMSG::SNETPC_MSG_CHA_SAVE_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_SAVE_FA* ) nmg;
// 	if ( m_pCharMan )
// 	{
// 		m_pCharMan->ChaSaveUserNumAdd( pNetMsg->dwUserNum );
// 		SENDTOALLCHANNEL( nmg );
// 	}

	return TRUE;
}

BOOL GLAgentServer::MsgChaSaveEndFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	// AgentServerMsg.cpp case NET_MSG_CHA_SAVE_END_FA: 에서 처리됨
// 	GLMSG::SNETPC_MSG_CHA_SAVE_END_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_SAVE_END_FA* ) nmg;
// 	if ( m_pCharMan )
// 	{
// 		m_pCharMan->ChaSaveUserNumDel( pNetMsg->dwUserNum );
// 		SENDTOALLCHANNEL( nmg );
// 	}

	return TRUE;
}

bool GLAgentServer::IsChaSaving( DWORD dwUserNum )
{
	if ( m_pCharMan )
	{
		return m_pCharMan->ChaSaveUserNumExist( dwUserNum );
	}
	return false;
}

BOOL GLAgentServer::MsgChaValidataionFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_MSG_CHA_VALIDATION_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_VALIDATION_FA* ) nmg;

	GLCharAG* pChar = GetCharByUserDbNum( pNetMsg->dwUserNum );

    if ( !pChar )
        return FALSE;

	SENDTOCLIENT( pChar->ClientSlot(), nmg );

	return TRUE;
}


BOOL GLAgentServer::MsgGmSetIPEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pMyChar = GetChar( dwGaeaID );
    if ( !pMyChar )
        return FALSE;

    if ( pMyChar->m_dwUserLvl < USER_GM1 )
        return FALSE;

    GLMSG::SNETPC_GM_SET_IPEVENT* pNetMsg = (GLMSG::SNETPC_GM_SET_IPEVENT *)nmg;

    SENDTOALLCHANNEL( pNetMsg );

    if ( pNetMsg->bEnable )
    {
        m_WorkEvent.bWorkEvent[EVENT_IP] = TRUE;
        m_WorkEvent.ip_Event = *pNetMsg;
    }
    else
        m_WorkEvent.bWorkEvent[EVENT_IP] = FALSE;

    return TRUE;
}

BOOL GLAgentServer::MsgGmAcquireEXP(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pMyChar = GetChar( dwGaeaID );
    if ( !pMyChar )
        return FALSE;

    if ( pMyChar->m_dwUserLvl < USER_USER_GM )
        return FALSE;

    GLMSG::SNETPC_GM_ACQUIRE_EXP* pNetMsg = (GLMSG::SNETPC_GM_ACQUIRE_EXP*)nmg;
    pNetMsg->dwID = dwGaeaID;

    SENDTOFIELDSVR( pMyChar->m_nChannel, pMyChar->m_dwCurFieldSvr, pNetMsg );

    return TRUE;
}

BOOL GLAgentServer::MsgFieldServerStateFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	return m_pStateFieldServer->messageProcedure(nmg) == true ? TRUE : FALSE;
}


BOOL GLAgentServer::MsgMsgTournamentAgent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if(!m_pMatchingCore)	
		return FALSE;
	m_pMatchingCore->messageProcedureAgent(nmg, dwClientID, dwGaeaID);

    return TRUE;
}

BOOL GLAgentServer::MsgMsgPackTournamentAgent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    if(!m_pMatchingCore)	
        return FALSE;
    m_pMatchingCore->messageProcedureMsgPack(nmg, dwClientID, dwGaeaID);

    return TRUE;
}

BOOL GLAgentServer::MsgTexasHolem( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD dwGaeaID )
{
	if ( NULL == m_pTexasHoldemMgr )
	{
		return FALSE;
	}

	m_pTexasHoldemMgr->MessageProcess( pMsg, ClientSlot, dwGaeaID );
	return TRUE;
}

// User System Info
BOOL GLAgentServer::MsgUserSystemInfoCA( NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID )
{
	// CAgentServer 에서 처리함
	return TRUE;
}

//! FlyCameraControl
BOOL GLAgentServer::MsgGMFlyCameraControl(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	// 단순 GM인지만 체크한다;
	GLCharAG* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
		return FALSE;
	if (pMyChar->m_dwUserLvl < USER_MASTER)
		return FALSE;

	GLMSG::SNET_GM_FLYCAMERACONTROL_CA* msg = (GLMSG::SNET_GM_FLYCAMERACONTROL_CA*)nmg;
	GLMSG::SNET_GM_FLYCAMERACONTROL_CF MsgAF;
	MsgAF.bOn = msg->bOn;
	SENDTOFIELD(pMyChar->ClientSlot(), &MsgAF);

	//GLMSG::SNET_GM_FLYCAMERACONTROL_FB NetMsgFB;
	//SENDTOCLIENT(dwClientID, &NetMsgFB);
	return TRUE;
}


BOOL GLAgentServer::MsgServerCloseAgent ( NET_MSG_GENERIC* pMsg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	GLMSG::SNET_SERVER_CLOSE_AGENT* pCloseMessage =
		static_cast< GLMSG::SNET_SERVER_CLOSE_AGENT* >( pMsg );

	if ( pCloseMessage )
	{
		sc::writeLogInfo( sc::string::format(
			"Close AgentServer ... %1%",
			pCloseMessage->dwAgentID ) );
	}

	return TRUE;
}


BOOL GLAgentServer::MsgServerCloseField(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	const GLMSG::SNET_SERVER_CLOSE_FIELD* const pCloseMessage((const GLMSG::SNET_SERVER_CLOSE_FIELD* const)nmg);

	// channel/ID 모두 net::INVALID_SLOT 일수 있다는 걸 염두해두고 처리해야 한다;
	const DWORD dwFieldChannel(pCloseMessage->dwFieldChannel);
	const DWORD dwFieldID(pCloseMessage->dwFieldID);

	// 다운된 필드서버가 점령전 필드일 경우 점령전을 중지한다;
	if ( m_pCTFMan &&
		m_pCTFMan->GetChannel() == dwFieldChannel &&
		m_pCTFMan->GetFieldNum() == static_cast< UINT >( dwFieldID ) )
	{
		m_pCTFMan->DoStop();
	}
	
	// 해당 필드 서버에 생성 되어 있던 신규 인던 모두 삭제;
	GLMSG::NET_INSTANCE_DESTROY _destroyInstantMessage(GAEAID_NULL, dwFieldChannel, dwFieldID);
	SENDTOMYSELF(&_destroyInstantMessage);

	typedef std::vector<unsigned int> IntegerVector;	
	{ // 구인던 및 무한의 재단도 삭제 처리 해야 한다;
		IntegerVector vectorInstanceID;
		for ( VEC_INSTANT_MAPID_ITER iter(m_vecInstantMapId.begin()); iter != m_vecInstantMapId.end(); ++iter )
		{
			const DWORD _instanceMapID(*iter);
			GLAGLandManager* _pLandManager(GetLandManager(_instanceMapID));
			if ( _pLandManager == NULL )	
				continue;

			const SMAPNODE* const pMapNode(FindMapNode(_pLandManager->getBaseMapID()));
			if ( pMapNode == NULL )
				continue;

			if ( pMapNode->GetInstanceType() != SMAPNODE::EMINSTANCE_OLD_INSTANCE )
				continue;

			const DWORD _dwInstantFieldID(pMapNode->GetFieldServerNum());
			const DWORD _dwInstantFieldChannel(_pLandManager->getFieldChannel());

			const bool _bValidChannel((dwFieldChannel == net::INVALID_SLOT) || (dwFieldChannel == _dwInstantFieldChannel));
			const bool _bValidID((dwFieldID == net::INVALID_SLOT) || (dwFieldID == _dwInstantFieldID));
			if ( (_bValidChannel == false) || (_bValidID == false) )
				continue;

			vectorInstanceID.push_back(_instanceMapID);			
		}

		const unsigned int _sizeInstanceMapID(vectorInstanceID.size());
		for ( unsigned int _index(_sizeInstanceMapID); _index--; )
		{
			const unsigned int _instanceMapID(vectorInstanceID[_index]);
			DeleteInstantMap(_instanceMapID);
		}
	}

	{		
		IntegerVector vectorFieldID;
		IntegerVector vectorFieldChannel;
		vectorFieldID.reserve(FIELDSERVER_MAX);
		vectorFieldChannel.reserve(MAX_CHANNEL_NUMBER);

		if ( dwFieldChannel == net::INVALID_SLOT )
		{
			for ( unsigned int _channel(MAX_CHANNEL_NUMBER); _channel--; )
			{
				if ( GetFieldNum(_channel) == 0 )
					continue;
				vectorFieldChannel.push_back(_channel);
			}
		}
		else
			vectorFieldChannel.push_back(dwFieldChannel);

		if ( dwFieldID == net::INVALID_SLOT )
		{
			for ( unsigned int _fieldID(FIELDSERVER_MAX); _fieldID--; )
				vectorFieldID.push_back(_fieldID);
		}
		else
			vectorFieldID.push_back(dwFieldID);


		for ( unsigned int _i(vectorFieldChannel.size()); _i--; )
		{
			const unsigned int _channel(vectorFieldChannel[_i]);
			for ( unsigned int _j(vectorFieldID.size()); _j--; )
			{
				const unsigned int _fieldID(vectorFieldID[_j]);
				GLMSG::NET_FIELD_STATE_MEMORY fieldStateMessage(_fieldID, _channel);
				fieldStateMessage.nUsedMemory = -1024 * 1024 * 32;
				SENDTOMYSELF(&fieldStateMessage);
			}
		}
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPostGetDataChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	// db action 에서 캐릭터 접근해서 처리하던 로직 메시지 처리기로 이동함
	if ( dwClientID != DBACTION_CLIENT_ID )
		return FALSE;

	GLMSG::SNET_POST_GET_DATA_CHANGE* pPacket = ( GLMSG::SNET_POST_GET_DATA_CHANGE* ) nmg;
	GLCharAG* pCha = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( pCha )
	{
		pCha->m_bPostGotData = pPacket->bPostGotData;
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPostGetStateChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	// db action 에서 캐릭터 접근해서 처리하던 로직 메시지 처리기로 이동함
	if ( dwClientID != DBACTION_CLIENT_ID )
		return FALSE;

	GLMSG::SNET_POST_GET_STATE_CHANGE* pPacket = ( GLMSG::SNET_POST_GET_STATE_CHANGE* ) nmg;
	GLCharAG* pCha = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( pCha )
	{
		pCha->m_bPostGotState = pPacket->bPostGotState;
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPostGetSendStateChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	// db action 에서 캐릭터 접근해서 처리하던 로직 메시지 처리기로 이동함
	if ( dwClientID != DBACTION_CLIENT_ID )
		return FALSE;

	GLMSG::SNET_POST_GET_SEND_STATE_CHANGE* pPacket = ( GLMSG::SNET_POST_GET_SEND_STATE_CHANGE* ) nmg;
	GLCharAG* pCha = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( pCha )
	{
		pCha->m_bPostGotSendState = pPacket->bPostGotSendState;
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPostGetRecvStateChangeDB( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	// db action 에서 캐릭터 접근해서 처리하던 로직 메시지 처리기로 이동함
	if ( dwClientID != DBACTION_CLIENT_ID )
		return FALSE;

	GLMSG::SNET_POST_GET_RECV_STATE_CHANGE* pPacket = ( GLMSG::SNET_POST_GET_RECV_STATE_CHANGE* ) nmg;
	GLCharAG* pCha = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( pCha )
	{
		pCha->m_bPostGotRecvState = pPacket->bPostGotRecvState;
	}

	return TRUE;
}

BOOL GLAgentServer::MsgCharMoneyNotifyFA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	NET_CHAR_MONEY_NOTIFY_FA* pPacket = ( NET_CHAR_MONEY_NOTIFY_FA* ) nmg;
	AddLogAdoJob(
		db::DbActionPtr( 
		new db::CharMoneyNotify( pPacket->dwChaDbNum, pPacket->tDate, pPacket->llOldMoney, pPacket->llNewMoney, pPacket->nCause, pPacket->nChannel, pPacket->nField ) ) );
	return TRUE;
}

BOOL GLAgentServer::MsgEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_EVENT_MESSAGE* const _pMessageEvent((GLMSG::NET_EVENT_MESSAGE* const)nmg);
	
	switch ( m_ServiceProvider )
	{
	case SP_OFFICE_TEST:
	case SP_KOREA:
	case SP_KOREA_TEST:
	case SP_GLOBAL:
	case SP_GS:
		break;
	default:
		return FALSE;
	}

	GLCharAG* const _pPlayerGM(GetChar(dwGaeaID));
	if ( _pPlayerGM == NULL )
		return FALSE;

	if ( _pPlayerGM->m_dwUserLvl < USER_GM1 )
	{
		sc::writeLogError(std::string("/event - The GM character level is lower than GM level 1."));
		return FALSE;
	}

	if ( m_pEventManager->messageProcedure(dwClientID, dwGaeaID, nmg) == true )
		return TRUE;

	SENDTOALLCHANNEL(nmg);

	return TRUE;
}

BOOL GLAgentServer::MsgCountRecorderInfoReq( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ * pNetMsg = (GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ *)nmg;
	GLCharAG* pMyChar = GetChar ( pNetMsg->dwGaeaID );
	if ( !pMyChar || pMyChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	const DWORD dwFieldServerNumber = GetFieldServer(pNetMsg->sMapID);
	SENDTOFIELDSVR(pMyChar->m_nChannel, dwFieldServerNumber, pNetMsg);
	return TRUE;
}

BOOL GLAgentServer::MsgCountRecorderInfoFB_FA( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	GLMSG::SNET_GM_COUNTRECORDER_INFO_FB* pNetMsg = (GLMSG::SNET_GM_COUNTRECORDER_INFO_FB *)nmg;
	const GLCharAG* const pCharAG = GetChar(pNetMsg->dwGaeaID);
	if ( pCharAG == NULL )
		return FALSE;

	SENDTOCLIENT(pCharAG->ClientSlot(), nmg);
	return TRUE;
}

BOOL GLAgentServer::MsgGmSetNotifyMoneyCA( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	GLMSG::NET_GM_NOTIFY_MONEY_CAF* pNetMsg = ( GLMSG::NET_GM_NOTIFY_MONEY_CAF *) pMsg;

	GLCharAG* pChar = GetChar ( GaeaID );

	if ( !pChar )
		return FALSE;

	if ( pChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	if ( pNetMsg->money < 0 )
	{
		return FALSE;
	}

	GLCONST_CHAR::NotifyMoney = pNetMsg->money;
	SENDTOALLCHANNEL( pMsg );
	SENDTOCLIENT( ClientSlot, pMsg );
	return TRUE;
}

BOOL GLAgentServer::MsgGmNetworkProfileReq( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	GLMSG::SNET_NETWORKPROFILE_REQ* pNetMsg = ( GLMSG::SNET_NETWORKPROFILE_REQ *) pMsg;

	GLCharAG* pChar = GetChar ( GaeaID );

	if ( !pChar )
		return FALSE;

	if ( pChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	SENDTOALLCHANNEL( pMsg );

	return TRUE;
}

BOOL GLAgentServer::MsgGmChangeJoinWorldBattle( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	GLMSG::SNET_CHANGE_WBJOIN_STATE* pNetMsg = ( GLMSG::SNET_CHANGE_WBJOIN_STATE *) pMsg;

	GLCharAG* pChar = GetChar ( GaeaID );

	if ( !pChar )
		return FALSE;

	if ( pChar->m_dwUserLvl < USER_GM3 )
		return FALSE;

	GLUseFeatures::GetInstance().SetUseFeature(GLUseFeatures::EMUSE_FEATURE_JOINWORLDBATTLE, pNetMsg->bJoinWorldBattle);

	SENDTOALLCLIENT( pNetMsg );

	return TRUE;
}

BOOL GLAgentServer::MsgTransDB2WorldBattle( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE* pNetMsg = ( GLMSG::SNETPC_TRANS_TO_WORLDBATTLE *) pMsg;

	GLCharAG* pChar = GetChar ( GaeaID );

	if ( NULL == pChar )
	{
		sc::writeLogError( "[ Battle Server ] [ A character is NULL. ]" );

		return FALSE;
	}

	bool bRet = false;
	// 현재 에이젼트가 전장서버 에이전트에 연결되어있는지 확인
	for( int i = 0; i < AGENTSERVER_MAX; i++ )
	{
		if( IsAgentOnline(i) == true ) bRet = true;
	}

	if( bRet == true )
	{
		sc::writeLogInfo( sc::string::format(
			"[ Battle Server ] [ Go to the Battle Server. < FieldServer : %1%/%2%, UserDBNum : %3%, ChaDBNum : %4% > ]",
			pChar->GetChannel(),
			pChar->GetCurrentFieldServer(),
			pChar->UserDbNum(),
			pChar->ChaDbNum() ) );

		pNetMsg->dwChaDBNum = pChar->ChaDbNum();
		SENDTOFIELD(pChar->ClientSlot(), pNetMsg);
	}
	else
	{
		sc::writeLogInfo( sc::string::format(
			"[ Battle Server ] [ The Battle Server is offline. < FieldServer : %1%/%2%, UserDBNum : %3%, ChaDBNum : %4% > ]",
			pChar->GetChannel(),
			pChar->GetCurrentFieldServer(),
			pChar->UserDbNum(),
			pChar->ChaDbNum() ) );

		GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_ANS NetMsgAns; // false
		NetMsgAns.OfflineWorldBattleServer();
		SENDTOCLIENT( ClientSlot, &NetMsgAns );
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPrisonAg( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNETPC_PRISON_AG *pNetMsg = (GLMSG::SNETPC_PRISON_AG *) nmg;

	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pChar)
		return FALSE;

	m_pCTFMan->ForceOut(pChar);
	m_pMatchingCore->ForcCancel(pNetMsg->dwChaNum);

	QuitParty(pChar, false );

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.14, 신규등록
 * modified : 
 */
BOOL GLAgentServer::MsgGmSetRandomboxChanceReqCA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC NetMsgFB;

    // 요청자에 대한 정보가 없으면 오류
	GLCharAG* pMyChar = GetChar(GaeaID);
	if (!pMyChar)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REQ_FLAG_GMGRADE_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

    // 요청자가 GM 1등급이 아니면 오류
	if (pMyChar->m_dwUserLvl < USER_GM1)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REQ_FLAG_GMGRADE_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

	GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA* pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA*)nmg;

    // Randombox의 MID/SID가 유효한지 체크
	const SITEM* pBoxItem = GLogicData::GetInstance().GetItem(pNetMsg->sBoxID);
	if ((!pBoxItem) || (pBoxItem->sBasicOp.emItemType != ITEM_RANDOMITEM))
	{
		NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_BOXID_ERR;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
	}

	if (!pBoxItem->sRandomBox.VALID())
	{
		NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_BOXID_ERR;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
	}

    // Item MID/SID가 유효한지 체크
    BOOL bFindItem = false;
	for (DWORD i = 0; i < pBoxItem->sRandomBox.vecBOX.size(); ++i)
	{
		ITEM::SRANDOMITEM sITEM = pBoxItem->sRandomBox.vecBOX[i];

        if (sITEM.nidITEM == pNetMsg->sItemID)
		{
			bFindItem = true;
			break;
		}
	}
    if (!bFindItem)
    {
		NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_ITEM_ERR;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

	const SITEM* pITEM_DATA = GLogicData::GetInstance().GetItem(pNetMsg->sItemID);
	if (!pITEM_DATA)
	{
		NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_ITEM_ERR;
		SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
	}

    // Count가 유효한지 체크
    if ((pNetMsg->nReqCount < 1) || (pNetMsg->nReqCount > 99999))
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_COUNT_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

	    return FALSE;
    }

    // ChaNum이 유효한지 체크
    /* 유효체크를 위하여 DB를 한번더 Select해야 하기 때문에 체크를 뺀다.
    if (pNetMsg->dwChaNum > 0)
    {
        GLCharAG* pReqChar = GetCharByDbNum(pNetMsg->dwChaNum);
        if (!pReqChar)
        {
            NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_CHANUM_ERR;
            SENDTOCLIENT(ClientSlot, &NetMsgFB);

		    return FALSE;
        }
    }
    */

    // CacheServer에 보내는 메시지를 생성하여 보낸다.
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH NetMsgAH;

    NetMsgAH.dwClientSlot = ClientSlot;
    NetMsgAH.sBoxID = pNetMsg->sBoxID;
    NetMsgAH.sItemID = pNetMsg->sItemID;
    NetMsgAH.nReqCount = pNetMsg->nReqCount;
    NetMsgAH.dwChaNum = pNetMsg->dwChaNum;
	NetMsgAH.dwGMNum = pMyChar->ChaDbNum();

	SENDTOCACHE(&NetMsgAH);

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.14, 신규등록
 * modified : 
 */
BOOL GLAgentServer::MsgGmSetRandomboxChanceReqHA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC NetMsgFB;

	GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA* pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA*)nmg;

    DWORD dwClientSlot = pNetMsg->dwClientSlot;

    // CacheServer로 부터 오류를 전송받아을때 
	if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA::REP_FLAG_FAIL)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_FAIL;
        SENDTOCLIENT(dwClientSlot, &NetMsgFB);

		return FALSE;
    }

	if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA::REP_FLAG_DUPLICATE_ERR)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_DUPLICATE_ERR;
        SENDTOCLIENT(dwClientSlot, &NetMsgFB);

		return FALSE;
    }

    NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_SUCESS;
    SENDTOCLIENT(dwClientSlot, &NetMsgFB);

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 조회
 * modified : 
 */
BOOL GLAgentServer::MsgGmGetRandomboxChanceListCA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC NetMsgFB;

    // 요청자에 대한 정보가 없으면 오류
	GLCharAG* pMyChar = GetChar(GaeaID);
	if (!pMyChar)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REQ_FLAG_GMGRADE_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

    // 요청자가 GM 1등급이 아니면 오류
	if (pMyChar->m_dwUserLvl < USER_GM1)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REQ_FLAG_GMGRADE_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

    // CacheServer에 보내는 메시지를 생성하여 보낸다.
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH NetMsgAH;

    NetMsgAH.dwClientSlot = ClientSlot;

	SENDTOCACHE(&NetMsgAH);

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 조회
 * modified : 
 */
BOOL GLAgentServer::MsgGmGetRandomboxChanceListHA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC NetMsgFB;

	GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA* pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA*)nmg;

    DWORD dwClientSlot = pNetMsg->dwClientSlot;

    // CacheServer로 부터 오류를 전송받아을때 
	if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA::REP_FLAG_FAIL)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REP_FLAG_FAIL;
        SENDTOCLIENT(dwClientSlot, &NetMsgFB);

		return FALSE;
    }

	if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA::REP_FLAG_NOTFOUND_ERR)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REP_FLAG_NOTFOUND_ERR;
        SENDTOCLIENT(dwClientSlot, &NetMsgFB);

		return FALSE;
    }

    NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REP_FLAG_SUCESS;
    NetMsgFB.nIndex = pNetMsg->nIndex;
    NetMsgFB.sBoxID = pNetMsg->sBoxID;
    NetMsgFB.sItemID = pNetMsg->sItemID;
    NetMsgFB.nReqCount = pNetMsg->nReqCount;
    NetMsgFB.dwTargetChaNum = pNetMsg->dwTargetChaNum;
    NetMsgFB.dwGMNum = pNetMsg->dwGMNum;
    NetMsgFB.nOpenCount = pNetMsg->nOpenCount;

    SENDTOCLIENT(dwClientSlot, &NetMsgFB);

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 삭제
 * modified : 
 */
BOOL GLAgentServer::MsgGmDelRandomboxChanceCA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC NetMsgFB;

	GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA* pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA*)nmg;

    // 요청자에 대한 정보가 없으면 오류
	GLCharAG* pMyChar = GetChar(GaeaID);
	if (!pMyChar)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REQ_FLAG_GMGRADE_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

    // 요청자가 GM 1등급이 아니면 오류
	if (pMyChar->m_dwUserLvl < USER_GM1)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REQ_FLAG_GMGRADE_ERR;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);

		return FALSE;
    }

    // CacheServer에 보내는 메시지를 생성하여 보낸다.
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH NetMsgAH;

    NetMsgAH.dwClientSlot = ClientSlot;
    NetMsgAH.nIndex = pNetMsg->nIndex;

	SENDTOCACHE(&NetMsgAH);

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 삭제
 * modified : 
 */
BOOL GLAgentServer::MsgGmDelRandomboxChanceHA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC NetMsgFB;

	GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA* pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA*)nmg;

    DWORD dwClientSlot = pNetMsg->dwClientSlot;

    // CacheServer로 부터 오류를 전송받아을때 
	if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA::REP_FLAG_FAIL)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REP_FLAG_FAIL;
        SENDTOCLIENT(dwClientSlot, &NetMsgFB);

		return FALSE;
    }

	if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA::REP_FLAG_NOTFOUND_ERR)
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REP_FLAG_NOTFOUND_ERR;
        SENDTOCLIENT(dwClientSlot, &NetMsgFB);

		return FALSE;
    }

    NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REP_FLAG_SUCESS;

    SENDTOCLIENT(dwClientSlot, &NetMsgFB);

	return TRUE;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.23, 삭제
 * modified : 
 */
BOOL GLAgentServer::MsgRandomboxWinnerNotifyFA(NET_MSG_GENERIC* nmg, DWORD ClientSlot, DWORD GaeaID)
{
	GLMSG::SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA* pNetMsg = (GLMSG::SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA*)nmg;

    const std::string szChaName = GetChaNameByDbNum(pNetMsg->dwTargetChanum);

    GLMSG::SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC NetMsgNotify(szChaName, pNetMsg->sRandomboxID, pNetMsg->sItemID);

    SENDTOALLCLIENT(&NetMsgNotify);

 #ifndef _RELEASE
    sc::writeLogInfo(sc::string::format("MsgRandomboxWinnerNotifyFA :[%1%]", szChaName));
 #endif

	return TRUE;
}

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
BOOL GLAgentServer::MsgGmRandomboxNotifyItemReloadReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pMyChar = GetChar(dwGaeaID);

    if (!pMyChar)
        return FALSE;

    if (pMyChar->m_dwUserLvl < USER_GM3)
        return FALSE;

    GLMSG::SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF* pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF*) nmg;
    SENDTOALLCHANNEL(pNetMsg);

    return TRUE;
}

void GLAgentServer::ChaSaveUserNumAdd( DWORD dwUserNum )
{
	if ( m_pCharMan )
	{
		m_pCharMan->ChaSaveUserNumAdd( dwUserNum );
	}
}

void GLAgentServer::ChaSaveUserNumDel( DWORD dwUserNum )
{
	if ( m_pCharMan )
	{
		m_pCharMan->ChaSaveUserNumDel( dwUserNum );
	}
}

BOOL GLAgentServer::MsgGmRanMobileCommandRequest( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
	{
//		sc::writeLogError(std::string("RAN MOBILE option is closed.(MsgGmRanMobileCommandRequest)"));	//
		return TRUE;
	}

	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST* pNetMsg = ( GLMSG::SNET_RANMOBILE_COMMAND_REQUEST *) pMsg;

	GLCharAG* pChar = GetChar ( GaeaID );

	if ( !pChar )
		return FALSE;

	//if ( pChar->m_dwUserLvl < USER_GM3 )
	//	return FALSE;

	if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
		return FALSE;

	CAgentServer* pAgentServer = static_cast< CAgentServer* >( m_pMsgServer );
	if ( !pAgentServer )
		return FALSE;

	//	DB에 저장.
	AddGameAdoJob( db::DbActionPtr( new db::RanMobileRequest(pChar->m_UserDbNum, pChar->m_CharDbNum, pAgentServer->GetServerGroup(), pNetMsg->nCommand, pNetMsg->nValue, std::string(pChar->GetChaName()), (int)pChar->m_emClass ) ));

	return TRUE;
}

BOOL GLAgentServer::MsgGmRanMobileCommandResponse( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
	{
//		sc::writeLogError(std::string("RAN MOBILE option is closed.(MsgGmRanMobileCommandRequest)")); //
		return TRUE;
	}

	GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE* pNetMsg = ( GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE *) pMsg;

	GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaDBNum);

	if( pChar == NULL ) return FALSE;

	int dwMyClientSlot =  pChar->ClientSlot();

	pNetMsg->ClearUserInfo();

	SENDTOCLIENT(dwMyClientSlot, pNetMsg);

	return TRUE;
}