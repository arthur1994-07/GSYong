#include "pch.h"

#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Json/json_spirit_reader.h"
#include "../SigmaCore/Json/MinJson.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/Msg/JsonMsgDefine.h"
#include "../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../RanLogic/Msg/GLContrlFactionMsg.h"
#include "./Land/Faction/FactionManagerClient.h"

#include "./Tutorial/GLBaseTutorial.h"
#include "./LandEvent/GLLandEventClient.h"
#include "./Char/GLCharClient.h"
#include "./ReferChar/ReferChar.h"
#include "./TexasHoldem/GLTexasHoldemClient.h"
#include "./Event/EventClient.h"
#include "./RanMobile/GLRanMobileClient.h"
#include "./GLGaeaClient.h"

// Lotto System
#include "./LottoSystem/GLLottoSystemManClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/**
    GLGaeaClient 의 멤버 함수뿐 아니라 타 클래스들의 함수도 등록이 가능 하게 한다.
    
    fun : 등록할 멤버 함수
    class : 멤버 함수를 가지는 클래스의 인스턴스
 */
#define CLASS_MEMBER_FUNCTION(fun, class) boost::bind(std::mem_fun(fun), class, _1)

void GLGaeaClient::initPacketFunc()
{
    m_GaeaClientMsgFunc[NET_MSG_JSON].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgJson, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_GETINFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsAuthInfoMsg, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_FB_AUTHED_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsAuthFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_FB_UNAUTHED_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsUnAuthFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_FB_GETUID_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsFriendAdd, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_FB_UPDATE_BR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsUpdateBrFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_TW_AUTHED_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsTwAuthedFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_TW_UNAUTHED_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsTwUnAuthedFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_TW_GETUID_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsTwGetUidFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SNS_TW_UPDATE_BR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSnsTwUpdateBr, this);

    // Note ].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::우편 시스템 관련
    m_GaeaClientMsgFunc[NET_MSG_POST_NOTIFY_RELAY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReceivePostNotify, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_GET_INFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostGetInfoFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_GET_DATA_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostGetDataFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_GET_STATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostGetStateFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_GET_SEND_STATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostGetSendStateFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_GET_RECV_STATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostGetRecvStateFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_REQ_CHANUM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostReqChaNumFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_SEND_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostSendFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_RETURN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostReturnFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_DELETE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostDeleteFb, this);
    m_GaeaClientMsgFunc[NET_MSG_POST_OPEN_TAKE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPostOpenTakeFb, this);

    ///////////////////////////
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PERIOD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPeriod, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_WEATHER].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgWeather, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_MAPWEATHER].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapWeather, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_WHIMSICAL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgWhimsical, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_MAPWHIMSICAL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapWhimsical, this);
    m_GaeaClientMsgFunc[NET_MSG_DROP_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropItem, this);
    m_GaeaClientMsgFunc[NET_MSG_DROP_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropMoney, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_PC_BASE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropPc_Base, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_PC_SKILLFACT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropPc_StateBlow_SkillFact, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_PC_PUT_ON_ITEMS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropPc_PutOnItems, this);
    //m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_PC_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropPc_End, this);
    m_GaeaClientMsgFunc[NET_MSG_UPDATE_DROP_CHAR_CLUB_NAME_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgUpdateDropCharClubNameAC, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_CROW].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropCrow, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_MATERIAL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropMaterial, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CHANGE_NAMEMAP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgChangeNameMap, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_NONCONFRONT_MODE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmNonConfrontMode, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_NONCONFRONT_MODE_VIEW_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmNonConfrontModeViewFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_WHISPER_STATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmWhisperStateFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_OUT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::DropOutCrow, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_GATEOUT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMoveActiveMap, this);
    //m_GaeaClientMsgFunc[NET_MSG_GCTRL_CREATE_TUTORIAL_MAP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ReqCreateTutorialMap, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ReqCreateInstantMap, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_NOTICE_MOVEMAP_RESULT_AFB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MoveMapResultAFB, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_MUST_LEAVE_MAP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ReLvMustLeaveFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_BUS_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ReqBusFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_TAXI_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ReqTaxiFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_TELEPORT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::TeleportFB, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_SYSTEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystem, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystemMsgPack, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_CONTENTS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystemContents, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystemContentsMsgPack, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystemProc, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_SHOW_TIME_UI_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystemProc, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INSTANCE_SHOW_ENTRANCE_STATE_UI_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstanceSystemProc, this);
	

    // resurrect
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_RESURRECT_FAFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgResurrect, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_RESURRECT_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgResurrectInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_RESURRECT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgResurrectFB, this);


    // faction System
    m_GaeaClientMsgFunc[NET_MSG_FACTION_SYSTEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFactionSystem, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPing, this);

    //mjeon
    //Ping msg process
    m_GaeaClientMsgFunc[NET_MSG_GM_PING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ProcessPingMsg, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_PING_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ProcessPingMsg, this);

    m_GaeaClientMsgFunc[NET_MSG_GM_PINGTRACE_ON_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ProcessPingTraceMsg, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_PINGTRACE_OFF_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ProcessPingTraceMsg, this);

    //	pc client에게.
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_MOVESTATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_JUMP_POS_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_ANIMOVE_START_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_CANCEL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_BLOCKING_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_AVOID_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_DAMAGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_HEAL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_RELEASE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_UPDATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPutonUpdateBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_CHANGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_LEVELUP_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_STATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_PASSIVE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_FLAGS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_GESTURE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_OPEN_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_CLOSE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_INFO_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_INFO_MARK_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_INFO_NICK_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_BRIGHT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LEARN_BIKE_BOOST_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_RENAME_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QITEMFACT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QITEMFACT_END_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_EVENTFACT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_EVENTFACT_END_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_IPEVENT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_GET_VEHICLE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UNGET_VEHICLE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_VEHICLE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_GOTO_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);	
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ITEMSHOPOPEN_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);	
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SKILL_CANCEL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_GATHERING_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_GATHERING_RESULT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_GATHERING_CANCEL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LANDEFFECT_UPDATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_MAPEFFECT_UPDATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_CDM_SAFETIME_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_SYNC].Msgfunc               = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgActivitySync, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_REWARD_POINT_SYNC].Msgfunc  = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgActivitySync, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_REWARD_BADGE_SYNC].Msgfunc  = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgActivitySync, this);
	m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_LOAD_COMPLETED_FF].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgActivitySync, this);

    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_POPUP_COMPLETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_TITLE_NOTIFY_CHANGE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_HIRE_SUMMON_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_UPDATE_CHARSLOT_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_EXP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAR_UPDATE_MONEY_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_SP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_LP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_SKP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_BRIGHT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_STATS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_CP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PICKUP_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PICKUP_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_HOLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_START].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_DATA].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_ITEM_SORT_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ITEM_SORT_FAIL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_INSERT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_DELETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_DEL_INSERT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ITEM_COOLTIME_ERROR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_RELEASE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_CHANGE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILLQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_ACTIONQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_BLOCKING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_AVOID].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_DAMAGE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_ATTACK_HEAL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_BEGIN_SKILL_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_END_SKILL_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_LEVELUP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_STATSUP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_STATSUP_EX_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_LEARNSKILL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILLUP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_RNSKILLUP_COMPLETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_DRUG_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUTON_DRUG_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SKILLCONSUME_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILLHOLD_RS_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SELECT_TARGET].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_QBOX_OPTION_MEMBER].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);


	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_INSERT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_DELETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_ITEM_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_DEL_INSERT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_DRUG_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_UPDATE_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_LOCK_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_AGREE_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_MONEY_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_COMPLETE_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRADE_CANCEL_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);


    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_ITEM_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_GRINDING_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_PERIODEXTEND_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_BOXOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_DISGUISE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_CLEANSER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_BOX_WRAPPING_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CHARGED_ITEM_GET_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CHARGED_ITEM_DEL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_RESET_SKST_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GET_CHARGED_ITEM_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_RENAME_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);	
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_POSTBOX_OPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REGEN_GATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_START2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_END2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONTPTY_START2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONTPTY_END2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONTCLB_START2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONTCLB_END2_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_RECOVE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CURE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CHARRESET_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_CHARCARD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_STORAGECARD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_REMODELOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_GARBAGE_RESULT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_PREMIUMSET_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAT_LOUDSPEAKER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAT_AREA_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAT_PARTY_RECRUIT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_CHAT_FACTION_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_INVENLINE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_DISJUNCTION_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PREMIUM_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STORAGE_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_RECOVERY_NPC_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_LOCKBOX_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_FIRECRACKER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_FIRECRACKER_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GM_MOVE2GATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_2_FRIEND_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_SHOWMETHEMONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GM_AUTO_LEVEL_LIST].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAutoLevelList, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_AUTO_LEVEL_RESULT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAutoLevelResult, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_AUTO_LEVEL_INFORMATION].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAutoLevelInformation, this);

	m_GaeaClientMsgFunc[NET_MSG_GM_MOB_LEVEL_LIST_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMobLevList, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_MAP_LAYER_NEW_NOTIFY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapLayerNewNotify, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_MAP_LAYER_DEL_NOTIFY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapLayerDelNotify, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_MAP_LAYER_ERROR_NOTIFY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapLayerErrorNotify, this);

	m_GaeaClientMsgFunc[NET_MSG_GM_LAND_INFORMATION].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgLand, this);

	m_GaeaClientMsgFunc[NET_MSG_GM_WHYSOSERIOUS_FB_FAC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGMWhySoSeriousFB, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_DOMINATE_MOVETO_CF_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_DOMINATE_MOVETO_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_GOTO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_NPC_ITEM_TRADE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_QUEST_START_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_STREAM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_DEL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_END_STREAM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_END_DEL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_MOBKILL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_ITEMUSE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_CONFTING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_QITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_REACHZONE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_TIMEOVER].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_TIME].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_NONDIE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_QUEST_COMPLETE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_TITLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_REGITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_DISITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_OPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_BUY_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_INFO_2CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEL_2CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDel, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubInfoDissolution, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_2CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMember, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_NEW_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubNewFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DISSOLUTION_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDissolutionFB, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_MEMBER_REQ_ASK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberReqAsk, this);
    //m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_MEMBER_REQ_FB_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ClubMemberReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_MEMBER_DEL_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberDel, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_MEMBER_DEL_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberDelFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberSecedeFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAuthorityReqAsk, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAuthorityReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_AUTHORITY_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAutority, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_MASTER_CHNAGE_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMasterChangeAC, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MARK_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMarkChangeFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMarkChange, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_RANK_CHANGE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubRank, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_RANK_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubRankFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberNickFB, this);
    //m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberState, this);
    //m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MEMBER_POS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberPos, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCD_CertifyFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_COMMISSION_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCommissionFB, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_RESET].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageReset, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_STORAGE_SEND_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageSendEnd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGetStorageItem, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_INSERT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageInsert, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_DELETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageDelete, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageDelIns, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageUpdateItem, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageUpdateMoney, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_NOTICE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubNoticeFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_NOTICE_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubNotice, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_SUBMASTER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubSubMasterFb, this);
    //m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_SUBMASTER_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_ALLIANCE_REQ_ASK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceReqAsk, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_ALLIANCE_ADD_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceAdd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceDel, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceDis, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceDelFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceSecFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceDisFB, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleReqAsk, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleBegin, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleBegin2, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleArmisticeReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleArmisticeReqAsk, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleOver, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleSubmissionReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattleKillUpdate, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubBattlePointUpdate, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceBattleReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceBattleReqAsk, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceBattleArmisticeReqFB, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceBattleArmisticeReqAsk, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceBattleSubmissionReqFB, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_RENAME_CLT].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgClubRename, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_RENAME_BRD].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::CharMsgProcess, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_RENAME_2ALLIANCE].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgClubRenameAlliance, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_RENAME_FB].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgClubRenameFB, this );

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PLAYERKILLING_ADD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PLAYERKILLING_DEL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberRename, this);

    m_GaeaClientMsgFunc[NET_MSG_REBUILD_RESULT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);	// ITEMREBUILD_MARK
    m_GaeaClientMsgFunc[NET_MSG_REBUILD_MOVE_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REBUILD_COST_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REBUILD_INPUT_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_LASTCALL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UPDATE_STARTCALL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    //m_GaeaClientMsgFunc[NET_MSG_SMS_PHONE_NUMBER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_SMS_SEND_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_CHINA_GAINTYPE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GM_LIMIT_EVENT_APPLY_START].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_LIMIT_EVENT_APPLY_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GM_LIMIT_EVENT_BEGIN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_LIMIT_EVENT_END_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

        /// 베트남 메시지
    m_GaeaClientMsgFunc[NET_MSG_VIETNAM_TIME_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VIETNAM_GAIN_EXP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VIETNAM_GAINTYPE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VIETNAM_GAIN_MONEY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VIETNAM_ALLINITTIME].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_EVENTFACT_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAR_ACTIVE_VEHICLE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_GET_VEHICLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_UNGET_VEHICLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_ACCESSORY_DELETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_VEHICLE_REQ_ITEM_INFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this); 
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_MOVE_MAP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LEARN_BIKE_BOOST_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_UPDATE_TRACING_CHARACTER].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_ATTENDLIST_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_ATTENDANCE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_ATTEND_REWARD_CLT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_NPC_RECALL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_INVEN_ITEM_MIX_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_GATHERING_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_REQ_GATHERING_RESULT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_NPCTALK_BASIC_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_NOTIFY_COMPLETED].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_NOTIFY_CLOSED].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_UPDATE_VALUE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_UPDATE_COMPLETED].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
//  m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_SYNC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
//  m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_REWARD_POINT_SYNC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
//  m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_REWARD_BADGE_SYNC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_ACTIVITY_CHANGE_BADGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GET_CHA_EXTRA_INFO_FH_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ACTION_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SKILLFACT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SKILLHOLD_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_STATEBLOW_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CURESTATEBLOW_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_END_SKILL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SKILLHOLD_RS_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SAVESKILLFACT_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PUSHPULL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::ActorMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CROW_MOVETO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CROW_ATTACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CROW_ATTACK_BLOCKING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CROW_AVOID].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CROW_DAMAGE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CROW_SKILL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_CROW_SKILL_DEALY_ACTION].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_CROW_END_SKILL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CROW_UPDATE_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::CrowMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ETNRY_FAILED].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgEtnryFailed, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DUPLICATE_CLOSEGAME].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDuplicateCloseGame, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LIMITTIME_OVER].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgLimitTimeOver, this);
    m_GaeaClientMsgFunc[NET_MSG_CYBERCAFECLASS_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCyberCafeClassUpdate, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_EVENT_EXP_END_CAF].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmEventExpEnd, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_EVENT_ITEM_GEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmEventItemGenFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_EVENT_GRADE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmEventGradeFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_CLASS_EVENT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmClassEvent, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_EVENT_ITEM_GEN_END_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmEventItemGenEndFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_EVENT_GRADE_END_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmEventGradeEndFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_BIGHEAD_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmBigHeadBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_BIGHAND_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmBigHandBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_FREEPK_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmFreePkBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_SHOP_INFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmShopInfoFb, this);

    // 대만에서 요청한 새로운 GM 명령어 ( /dsp player 의 확장판 )
    m_GaeaClientMsgFunc[NET_MSG_GM_VIEWALLPLAYER_FLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmViewAllPlayerFldFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_VIEWWORKEVENT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmViewWorkEventFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_MARK_INFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMarkInfoFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgclubDeathMatchStartBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDeathMatchEndBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDeathMatchRemainBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDeathMatchPointUpdate, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDeathMatchMyRankUpdate, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubDeathMatchRankingUpdate, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCdCertifyBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_CD_CERTIFY_ING_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCdCertyfingBrd, this);

    //선도전
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGuidBattleStartBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGuidBattleEndBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_GUID_BATTLE_REMAIN_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGuidBattleRemainBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SERVER_CLUB_GUID_BATTLE_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServerClubGuidBattleInfo, this);	

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LAND_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgLandInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SERVER_PKEVENT_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServerPkEventInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SERVER_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServerInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServerBrightEventInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServerClubDeathMatchInfo, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_COMMISSION_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCommissionBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCommissionRvBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_WHERE_NPC_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmWhereNpcFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_WHERE_NPC_ALL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmWhereNpcAllFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_PRINT_CROWLIST_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmPrintCrowListFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_WHERE_PC_MAP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmWherePcMapFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_WHERE_PC_POS_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmWherePcPosFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_MOVE2CHAR_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmMoveToCharFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_CHECKINSTANTNUM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmCheckInstantNumFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_CHAT_BLOCK_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmChatBlockFb, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_CHAT_BLOCK_NOTIFY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmChatBlockNotify, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_CHAR_INFO_AGT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmCharInfoAgtFb, this);
    m_GaeaClientMsgFunc[NET_MSG_USER_CHAR_INFO_AGT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgUserCharInfoAgtFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_CHAR_INFO_FLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmCharInfoFldFb, this);
    m_GaeaClientMsgFunc[NET_MSG_USER_CHAR_INFO_FLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgUserCharInfoFldFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_WARNING_MSG_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmWarningBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_INSTANT_MAP_REENTRY_CHECK_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInstantMapReEntryCheckFb, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_COUNTDOWN_MSG_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmCountDownBrd, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SERVERSTOP_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServerStopFb, this);
    m_GaeaClientMsgFunc[NET_MSG_SERVERTIME_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgServicerTimeBrd, this);

    //----------------------------------------------------------------------------------------------------------------
    // Message Processing about PET (START)
    //----------------------------------------------------------------------------------------------------------------

    // =======  Message Processing about MyPET (START)  ======= //

    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_USECARD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetReqUseCardFb, this);	
    m_GaeaClientMsgFunc[NET_MSG_PET_PETSKINPACKOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetSkinPackOpenFb, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_UNUSECARD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetReqUnUseCardFb, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_GIVEFOOD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetReqGiveFoodFb, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_PETCARDINFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetReqPetChardInfoFb, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_PETREVIVEINFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetReqPetRevivedInfoFb, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_REVIVE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetReqReviveFb, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_NOTENOUGHINVEN].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetNotEnoughInven, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_UPDATE_MOVE_STATE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_GETRIGHTOFITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_UPDATECLIENT_FULL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SKILLCHANGE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_RENAME_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_CHANGE_COLOR_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_CHANGE_STYLE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SLOT_EX_HOLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_HOLD_TO_SLOT_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SLOT_TO_HOLD_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_ADD_SKILLFACT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REMOVE_SKILLFACT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_LEARNSKILL_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REMOVE_SLOTITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_ATTACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_SAD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_ACCESSORY_DELETE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PET_JUMP_POS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyPetMsgProcess, this);

    // =======  Message Processing about MyPET (END)  ======= //
    //m_GaeaClientMsgFunc[NET_MSG_SUMMON_REQ_SLOT_EX_HOLD_FB:
    //m_GaeaClientMsgFunc[NET_MSG_SUMMON_REQ_HOLD_TO_SLOT_FB:
    //m_GaeaClientMsgFunc[NET_MSG_SUMMON_REMOVE_SLOTITEM_FB:

    // =======  Message Processing about AnyPET (START)  ======= //

    m_GaeaClientMsgFunc[NET_MSG_PET_DROPPET].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPetDropPet, this);
    m_GaeaClientMsgFunc[NET_MSG_CREATE_ANYPET].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCreateAnyPet, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_GOTO_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SKILL_CHANGE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_STOP_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_RENAME_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_CHANGE_COLOR_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_CHANGE_STYLE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_LEARNSKILL_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REQ_FUNNY_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_REMOVE_SLOTITEM_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_ATTACK_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_SAD_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_ACCESSORY_DELETE_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PET_PETSKINPACKOPEN_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PET_JUMP_POS_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::PetMsgProcess, this);
    // =======  Message Processing about AnyPet (END)  ======= //


    //----------------------------------------------------------------------------------------------------------------
    // Message Processing about PET (END)
    //----------------------------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------------------------
    // Message Processing about SUMMON (START)
    //----------------------------------------------------------------------------------------------------------------

    m_GaeaClientMsgFunc[NET_MSG_REQ_USE_SUMMON_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReqUseSummonFb, this);
	m_GaeaClientMsgFunc[NET_MSG_HIRE_BASICSUMMON_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReqHireSummonFC, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_GOTO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_ATTACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_ATTACK_BLOCKING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_ATTACK_AVOID].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_SKILL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_EFFECT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_UPDATE_CUR_SKILLPAGE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_UPDATE_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_LANDEFFECT_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_SUMMON_MAPEFFECT_UPDATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::SummonMsgProcess, this);

    m_GaeaClientMsgFunc[NET_MSG_GCTRL_DROP_SUMMON].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDropSummon, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_FAILED_DROP_SUMMON].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFailedDropSummon, this);

    //----------------------------------------------------------------------------------------------------------------
    // Message Processing about SUMMON (END)
    //----------------------------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------------------------
    // Message Processing about LandEvent (START)
    //----------------------------------------------------------------------------------------------------------------
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LANDEVENT_REMOVE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::LandEventMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::LandEventMsgProcess, this);

    // ------------------------------------------------------------------------
    // Point Shop
    m_GaeaClientMsgFunc[NET_MSG_PS_DATA_START_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPointShopDataStartAC, this);
    m_GaeaClientMsgFunc[NET_MSG_PS_DATA_AC].Msgfunc       = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPointShopDataAC, this);
    m_GaeaClientMsgFunc[NET_MSG_PS_DATA_END_AC].Msgfunc   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPointShopEndAC, this);

    m_GaeaClientMsgFunc[NET_MSG_CHAR_POINT_AC].Msgfunc    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPointShopCharPointAC, this);
    m_GaeaClientMsgFunc[NET_MSG_PS_BUY_ITEM_CF].Msgfunc   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPointShopBuyItemCF, this);

    m_GaeaClientMsgFunc[NET_MSG_CHAR_POINT_ADD_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPointShopAddPointFC, this);

    // -----------------------------------------------------------------------
    // Attendance
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_SYNC_FAC].Msgfunc           = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceSync, this);
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_RESET_FAC].Msgfunc          = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceReset, this);
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_UPGRADE_FAC].Msgfunc        = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceUpgrade, this);

    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_TASK_UPDATE_FC].Msgfunc    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceTaskUpdate, this);
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_TASK_RESET_FC].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceTaskReset, this);
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_TASK_COMPLETED_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceTaskCompleted, this);

    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_POPUP_COMPLETE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendancePopup, this);
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_TAKEREWARD_FB].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceTakeReward, this);
    m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_DONE_POINTITEM_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceDonePointshopItem, this);
	m_GaeaClientMsgFunc[NET_MSG_ATTENDANCE_LOAD_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAttendanceLoadEnd, this);
	
    // -----------------------------------------------------------------------
    // Captured the field

    m_GaeaClientMsgFunc[NET_MSG_CTF_READY_AC].Msgfunc                   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldReady, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_JOIN_RESULT_AC].Msgfunc             = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldJoinFB, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_WARNING4DROP_FC].Msgfunc            = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldWarning4Drop, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_UPDATE_QUEUE_AC].Msgfunc            = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldUpdateQueue, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_START_2ALL_AC].Msgfunc              = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldStartToAll, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_START_2PLAYERS_FC].Msgfunc          = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldStartToPlayers, this);
	m_GaeaClientMsgFunc[NET_MSG_CTF_DONE_AF_AC].Msgfunc					= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldDoneToAll, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_REWARD_FC].Msgfunc                  = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldReward, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_STOP_AC].Msgfunc                    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldStop, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_CROW_AUTHED].Msgfunc                = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldAuthed, this);
    m_GaeaClientMsgFunc[NET_MSG_CTF_SYNC_STATUS_AC].Msgfunc             = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldSyncStatus, this);
	m_GaeaClientMsgFunc[NET_MSG_CTF_UPDATE_PLAYERS_VECTOR_FC].Msgfunc   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldUpdatePlayersPosition, this);
	m_GaeaClientMsgFunc[NET_MSG_CTF_STATISTICS_FC].Msgfunc				= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldStatistics, this);
	m_GaeaClientMsgFunc[NET_MSG_CTF_RANKING_FAC].Msgfunc				= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldRankingToAll, this);
	m_GaeaClientMsgFunc[NET_MSG_CTF_REWARD_INFO].Msgfunc				= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCaptureTheFieldRewardToAll, this);
	m_GaeaClientMsgFunc[NET_MSG_CHA_VALIDATION_FA].Msgfunc				= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCharValidation, this);

    // ------------------------------------------------------------------------
    // 재매입
    m_GaeaClientMsgFunc[NET_MSG_ADD_REBUY_ITEM_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_BUY_REBUY_ITEM_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_CHECK_REBUY_SUPPLIES].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    // ------------------------------------------------------------------------
	m_GaeaClientMsgFunc[NET_MSG_REFER_CHAR_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReferChar, this);
    m_GaeaClientMsgFunc[NET_MSG_REFER_CHAR_PUTON].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReferChar, this);
    m_GaeaClientMsgFunc[NET_MSG_REFER_CHAR_SKILL].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReferChar, this);
	m_GaeaClientMsgFunc[NET_MSG_REFER_CHAR_INFO].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgReferChar, this);

    // 한국 셧다운 Shutdown
    m_GaeaClientMsgFunc[NET_MSG_KOR_SHUTDOWN_WARNING_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgKorShutdown, this);

    // ------------------------------------------------------------------------
    // World battle
    m_GaeaClientMsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_START_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCountryCountStartAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_AC].Msgfunc       = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCountryCountAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_COUNTRY_COUNT_END_AC].Msgfunc   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCountryCountEndAC, this);
    //m_GaeaClientMsgFunc[NET_MSG_CLUB_CHAR_LEVEL_CHANGE_AC].Msgfunc   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubChaLevelChangeAC, this);

    m_GaeaClientMsgFunc[NET_MSG_CLUB_AUTH_FC].Msgfunc   = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAuthFC, this);
    m_GaeaClientMsgFunc[NET_MSG_USER_CHANGE_COUNTRY_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgUserChangeCountryAC, this);

    m_GaeaClientMsgFunc[NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubCdCertifierChangeAC, this);

    m_GaeaClientMsgFunc[NET_MSG_PROTOCOL_INFO_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProtocolInfo, this);

	m_GaeaClientMsgFunc[NET_MSG_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgWbCbSetFlagFACBRD, this);


	// 맵 이름 검색 결과
	m_GaeaClientMsgFunc[NET_MSG_MAP_SEARCH_NAME_1_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapSearchNameAck1, this);
	m_GaeaClientMsgFunc[NET_MSG_MAP_SEARCH_NAME_2_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapSearchNameAck2, this);	
	m_GaeaClientMsgFunc[NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapSearchNameDetailInfo, this);
	m_GaeaClientMsgFunc[NET_MSG_MAP_SEARCH_NAME_POSITION_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapSearchNamePosition, this);

	m_GaeaClientMsgFunc[NET_MSG_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapPartyInfoListBoard_1, this);
	m_GaeaClientMsgFunc[NET_MSG_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapPartyInfoListBoard_2, this);

	m_GaeaClientMsgFunc[NET_MSG_MAP_PARTY_INFO_DETAIL_1_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapPartyInfoDetail_1, this);
	m_GaeaClientMsgFunc[NET_MSG_MAP_PARTY_INFO_DETAIL_2_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMapPartyInfoDetail_2, this);

    // Friend Group -----------------------------------------------------------
    m_GaeaClientMsgFunc[NET_MSG_FRIEND_GROUP_NEW_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendGroupNewAC, this);
    m_GaeaClientMsgFunc[NET_MSG_FRIEND_GROUP_DEL_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendGroupDelAC, this);
    m_GaeaClientMsgFunc[NET_MSG_FRIEND_SET_GROUP_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendSetGroupAC, this);
    m_GaeaClientMsgFunc[NET_MSG_FRIEND_GROUP_NAME_CHANGE_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendGroupNameChangeAC, this);
    m_GaeaClientMsgFunc[NET_MSG_FRIEND_GROUP_REQ_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendGroupReqAC, this);
    m_GaeaClientMsgFunc[NET_MSG_OTHER_CHAR_LEVEL_CHANGE_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgOtherCharLevelChangeAC, this);
    m_GaeaClientMsgFunc[NET_MSG_MAP_CHANGE_FRIEND_CLUB_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendMapChangeAC, this);

    // Notify -----------------------------------------------------------------
    m_GaeaClientMsgFunc[NET_MSG_NOTIFY_DATA_AC].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgNotifyDataAc, this);
    m_GaeaClientMsgFunc[NET_MSG_NOTIFY_DATA_AC_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgNotifyDataAcEnd, this);

	// MiniGame ------------------------------------------------------------------
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_MINIGAME].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMiniGame, this);

	// Event ------------------------------------------------------------------
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EVENT].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgEvent, this);

	// Party ------------------------------------------------------------------
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LURE_TAR].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LURE_FB].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LURE_FB_WITH_CHARNAME].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_FNEW].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_ADD].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_DEL].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_DISSOLVE].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_AUTHORITY].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_MOVEMAP].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_DETAIL].Msgfunc			= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_DETAIL_PARTY].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);	
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_LEVELUP].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_UPDATE].Msgfunc			= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_RENAME].Msgfunc			= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_MBR_PICKUP_LINK_BRD].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LINK_JOIN_FB].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LINK_JOIN_ANS].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LINK_LURE_FB].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_LINK_REFUSE_FB].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);	
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EXPEDITION_FNEW_FB].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);	
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_CHANGE_OPTION].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);		
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_ERROR].Msgfunc					= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EXPEDITION_CHECK_READY_REQUEST_CA].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EXPEDITION_CHECK_READY_ANSWER_CA].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);	
	m_GaeaClientMsgFunc[ NET_MSG_GCTRL_EXPEDITION_ERROR ].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgParty, this );	
	
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_ADD_TENDER_ITEM].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPartyTender, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_TENDER_ITEM].Msgfunc		= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPartyTender, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_DELETE_TENDER_ITEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPartyTender, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_TENDER_ITEM_FB].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPartyTender, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_JUNK_ITEM].Msgfunc			= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPartyTender, this);

    // Club -------------------------------------------------------------------
    m_GaeaClientMsgFunc[NET_MSG_CLUB_PULIC_FLAG_CHANGE_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubPublicFlagChangeAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_AUTO_KICK_AC].Msgfunc         = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAutoKickAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_NEWBIE_NOTICE_AC].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubNewbieNoticeAC, this);

    m_GaeaClientMsgFunc[NET_MSG_CLUB_LOG_AC].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubLogAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_LOG_AC_END].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubLogAc_End, this);

    m_GaeaClientMsgFunc[NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGradeNameChangeAC_Result, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_GRADE_NAME_CHANGE_AC].Msgfunc        = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGradeNameChangeAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_GRADE_FLAG_CHANGE_AC].Msgfunc        = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubGradeFlagChangeAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AC2].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubMemberGradeChangeAC2, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAR_ONLINE_AC].Msgfunc      = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCharOnlineAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAR_OFFLINE_AC].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCharOfflineAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CHAR_POSITION_AC].Msgfunc    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCharPositionAC, this);
    m_GaeaClientMsgFunc[NET_MSG_PARTY_INFO_REQ_AC].Msgfunc    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPartyInfoAC, this);
	m_GaeaClientMsgFunc[NET_MSG_CLUB_ID_2MASTERNAME_ANS_AC].Msgfunc    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubId2MasterNameAnsAC, this);

    // Product----------------------------------------------------------------------------------------
    m_GaeaClientMsgFunc[NET_MSG_REQ_LEARN_PRODUCTBOOK_FB].Msgfunc           = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PRODUCT_FB].Msgfunc                         = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PRODUCTBOOK].Msgfunc                        = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PRODUCT_TYPE_LIST_ACK].Msgfunc    = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PRODUCT_ISGETEXP_LIST_ACK].Msgfunc          = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PRODUCT_TYPE_ADD_ACK].Msgfunc               = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);
    m_GaeaClientMsgFunc[NET_MSG_PRODUCT_TYPE_DEL_ACK].Msgfunc               = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgProductProcess, this);

    // ------------------------------------------------------------------------
    m_GaeaClientMsgFunc[NET_MSG_CHAR_DB_NUM_REQ_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCharDbNumReqAC, this);
    m_GaeaClientMsgFunc[NET_MSG_FRIEND_STATE_REQ_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgFriendStateReqAC, this);

    m_GaeaClientMsgFunc[NET_MSG_GUIDANCE_MAP_INFO_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGuidanceMapInfoAC, this);
    m_GaeaClientMsgFunc[NET_MSG_GM_GUIDANCE_INFO_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmGuidanceInfoAC, this);
    m_GaeaClientMsgFunc[NET_MSG_OTHER_CHAR_PARTY_CHANGE_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgOtherCharPartyChangeAC, this);

    m_GaeaClientMsgFunc[NET_MSG_CLUB_JOIN_TIME_RESET_AC].Msgfunc     = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubJoinTimeResetAC, this);
    m_GaeaClientMsgFunc[NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubAllianceTimeResetAC, this);


	//개인상점 검색
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HAC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_CATCH_DELAY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCatchDelay, this);


	// ------------------------------------------------------------------------
	// 개인상점
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_NUM_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketItemNumFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketItemFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_END_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketItemEndFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_COMMON_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketCommonFB, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_LOG_NUM_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketLogNumFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_LOG_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketLogFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_LOG_END_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketLogEndFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_STATE_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketStateFC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_ITEM_NUM_TOTAL_HFC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPrivateMarketItemNumTotalFC, this);

    // 개인상점 검색 카드 사용
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

    // ------------------------------------------------------------------------
    //! 아이템 수리, 내구도
    m_GaeaClientMsgFunc[NET_MSG_ITEM_REPAIR_ACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgItemRepairAck, this);
    m_GaeaClientMsgFunc[NET_MSG_ITEM_REPAIR_CARD_ACK].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgItemRepairCardUse, this);

    // PVE
    m_GaeaClientMsgFunc[NET_MSG_PVE_MAPTABLE_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::AckPVEMapTable, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_ENTRANCE_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::AckPVEEntrance, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_ENTRANCE_CONFIRM_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::AckPVEEntranceConfirm, m_pPVEClient);
	m_GaeaClientMsgFunc[NET_MSG_PVE_CLEAROFBELONGING_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::AckPVEClearOfBelonging, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_ENTRANCE_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEEntranceState, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_MONSTERCLEAR_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEMonsterClearState, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_POSITIONINGCLEAR_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEPositioningClearState, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_STAGE_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEStageState, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_STAGE_RESULT_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEStageResult, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_PORTAL_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEPortalState, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_START_STAGE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEStartStage, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_MSG_START_STAGE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEMsgStartStage, m_pPVEClient);
    m_GaeaClientMsgFunc[NET_MSG_PVE_MSG_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVEMsg, m_pPVEClient);
	m_GaeaClientMsgFunc[NET_MSG_PVE_REMOVE_INDUN_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVERemoveIndun, m_pPVEClient);
	m_GaeaClientMsgFunc[NET_MSG_PVE_REBIRTH_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::MsgPVERebirth, m_pPVEClient);
	m_GaeaClientMsgFunc[NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::AckPVEAckReload, m_pPVEClient);
	m_GaeaClientMsgFunc[NET_MSG_PVE_RETRY_ACK_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLPVEClient::AckPVERetry, m_pPVEClient);
	
    // Char State
    m_GaeaClientMsgFunc[NET_MSG_CHAR_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCharState, this);

    m_GaeaClientMsgFunc[NET_MSG_GM_ACQUIRE_EXP_ANS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::AckAcquireEXP, this);
    m_GaeaClientMsgFunc[NET_MSG_GCTRL_ACQUIRE_EXP].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgAcquireEXP, this);

	//Rnattensdace
	m_GaeaClientMsgFunc[NET_MSG_RNATTENDANCE_QUEST_LIST_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgRnattendanceProcess, this);

	// Lotto System ---------------------------------------------------------------------------------------------------------------------------
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_GLOBAL_VALUE_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_STATUS_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NEXT_DRAWING_DATE_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_COUNT_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_INVENT_NUM_DSP_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_AC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_AB].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_GO_NEXT_TURN_AB].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LOTTO_SYSTEM_DRAWING_NUM_AB].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgLottoSystem, this );
	// ----------------------------------------------------------------------------------------------------------------------------------------

	// 프리미엄 맵 강퇴
	m_GaeaClientMsgFunc[NET_MSG_PREMIUM_MAP_EXIT].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgPremiumMapExit, this );

	//Ran TournamentSystem
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_MATCHING_SYSTEM].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgTournamentClient, this );
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgPackTournamentClient, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_MATCHING_UPDATE_JOIN_COUNT_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMatchingProc, this);

	// TexasHoldem
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_TEXAS_HOLD_EM].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgTexasHoldem, this );

    // 개인상점 포인트 거래
    m_GaeaClientMsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgConSignmentSaleListAck, this );
    m_GaeaClientMsgFunc[NET_MSG_CONSIGNMENT_ADD_ITEM_ACK].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgConSignmentSaleRegistAck, this );
    m_GaeaClientMsgFunc[NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgConSignmentSaleSendBackAck, this );
    m_GaeaClientMsgFunc[NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgConSignmentSalePurchaseAck, this );

    m_GaeaClientMsgFunc[NET_MSG_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgConSignmentSaleSlotExpirationDate, this );

	// FlyCameraControl
	m_GaeaClientMsgFunc[NET_MSG_GM_FLYCAMERACONTROL_FB].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgCameraControlFB, this );
	m_GaeaClientMsgFunc[NET_MSG_GM_FLYCAMERACONTROL_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgCameraControlFB, this );

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_APPLY_SKILLEFFECT_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgApplySkillEffectFC, this );

	//
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_SKILLDELAY].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_COUNTRECORDER_INFO_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgCountRecorderInfoFB, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgInvenItemDecomposeFb, this);

	m_GaeaClientMsgFunc[NET_MSG_GM_NOTIFY_MONEY_CAF].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgSetNotifyMoney, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_REQ_EXCHANGE_BUY_FROM_NPC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgNpcExchangeItem, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_USER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageUseFB, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgClubStorageMoneyUseFB, this);

	// Country;
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_VCOUNTRY_ADD_DAF].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgVCountryAddFAC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_VCOUNTRY_RESET_DAF].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgVCountryResetFAC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_SHOW_COUNTRY_MARK_FC].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgNameDisplayShowCountryMark, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_COSTUME_STATS_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCSStatsMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GM_CHANGE_WBJOIN_STATE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgChangeJoinWorldBattleState, this);	
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_ANS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgTransToWorldBattleAns, this);	
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_DB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgTransToWorldBattleDBAns, this);	

	m_GaeaClientMsgFunc[NET_MSG_DRUG_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgDrugFbProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_WITH_VEHICLE_TAR].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgWithVehicleTarget, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCSStatsReleaseMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_PARTY_FNEW_UPDATE].Msgfunc	= CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgParty, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_INVEN_TL_GRINDING].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EXP_COMPRESSOR_ANS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTRL_EXP_CAPSULE_ANS].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_LEVEL_LAYER_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgLevelLayerFC, this);
	// Macro
	m_GaeaClientMsgFunc[NET_MSG_MACROMODE_ONOFF_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMacroOnofFB, this);
	m_GaeaClientMsgFunc[NET_MSG_MACROMODE_STATE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMacroStateFC, this);
	m_GaeaClientMsgFunc[NET_MSG_MACROMODE_CLOSE_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMacroCloseFC, this);
	m_GaeaClientMsgFunc[NET_MSG_MACROMODE_NEARESTCROWINFO_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgMacroNearestCrowInfoFC, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_FORCERESET_SKILL_STAT_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgForceresetSkillStatFC, this);

	m_GaeaClientMsgFunc[NET_MSG_GCTRL_JACKPOT_NOTIFY].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgJackpotNotify, this );

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.15
     * modified : 
     */
	m_GaeaClientMsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmSetRandomboxChanceReqAC, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmGetRandomboxChanceListAC, this);
	m_GaeaClientMsgFunc[NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmDelRandomboxChanceAC, this);
	m_GaeaClientMsgFunc[NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgRandomboxWinnerNotifyAC, this);

	// 란모바일
	m_GaeaClientMsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmRanMobileCommandResponse, this);	
	m_GaeaClientMsgFunc[NET_MSG_GM_RESPONSE_USERCOUNT_LAND_FC].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MsgGmResponseUserCountLandFC, this);

	// 캐릭터 슬롯
	m_GaeaClientMsgFunc[NET_MSG_RESPONSE_LOCK_CHARSLOT_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgResponseLockCharSlotFC, this );
	m_GaeaClientMsgFunc[NET_MSG_RESPONSE_UNLOCK_CHARSLOT_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgResponseUnlockCharSlotFC, this );
	m_GaeaClientMsgFunc[NET_MSG_RESPONSE_CHANGE_CHARSLOT_FC].Msgfunc = CLASS_MEMBER_FUNCTION( &GLGaeaClient::MsgResponseChangeCharSlotFC, this );

	// 아이템 스킬 변경;
	m_GaeaClientMsgFunc[NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	// 아이템 기본 능력치 변경;
	m_GaeaClientMsgFunc[NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);

	// 아이템 가산 능력치 변경;
	m_GaeaClientMsgFunc[NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_FB].Msgfunc = CLASS_MEMBER_FUNCTION(&GLGaeaClient::MyCharMsgProcess, this);
}

void GLGaeaClient::MsgProcess(NET_MSG_GENERIC* pMsg)
{    
    if (!m_pLandMClient)
    {
        TEMP_PACKET Packet;
        Packet.SetData(pMsg, pMsg->Size());
        m_TempPacket.push_back(Packet);
        return;
    }
    else
    {
        size_t TempSize = m_TempPacket.size();
        if (TempSize > 0)
        {
            for (size_t i=0; i<TempSize; ++i)
            {
                TEMP_PACKET& Packet = m_TempPacket[i];
                MsgProcess2((NET_MSG_GENERIC*) &Packet);
            }
            m_TempPacket.clear();
        }
        MsgProcess2(pMsg);
    }
}

void GLGaeaClient::MsgProcess2(NET_MSG_GENERIC* pMsg)
{
    EMNET_MSG emType = pMsg->Type();
    if (emType < 0 || emType >= NET_MSG_GCTRL_MAX)
        return;

    if (m_GaeaClientMsgFunc[emType].Msgfunc)        
        m_GaeaClientMsgFunc[emType].Msgfunc(pMsg);
    else        
        CDebugSet::ToListView("GLGaeaClient::MsgProcess() TYPE[%d] massage leak", emType);
}

void GLGaeaClient::MsgJson(NET_MSG_GENERIC* pMsg)
{
    NET_JSON* pPacket = (NET_JSON*) pMsg;
    char JsonStr[NET_JSON::BUF_SIZE] = {0};
    StringCchCopy(JsonStr, NET_JSON::BUF_SIZE, pPacket->Buffer);
    MsgJsonParser(std::string(JsonStr));
}

bool GLGaeaClient::MsgJsonParser(const std::string& JsonStr)
{
    json_spirit::mValue Value;
    if (!json_spirit::read(JsonStr, Value))
    {
        // Json read error
        return false;
    }

    const json_spirit::mObject& Object = Value.get_obj();
    int MessageType = 0;
    if (!sc::Json::find_value(Object, "M", MessageType))
    {
        PrintMsgText(NS_UITEXTCOLOR::RED, std::string("Json message type 'M' not exist"));
        return false;
    }

    bool bReturn = true;
    switch (MessageType)
    {
    case jm::MSG_WORLD_BATTLE_CLUB_INFO:
        MsgClubCountryDataAC(Object);
        break;

    case jm::MSG_WORLD_BATTLE_CLUB_INFO_END:
        MsgClubCountryDataEndAC(Object);
        break;
    
    default:		
        PrintMsgText(
            NS_UITEXTCOLOR::RED,
            sc::string::format(
                "Unknown Json message %1%", MessageType));
        bReturn = false;
        break;
    }
    return bReturn;
}

void GLGaeaClient::MsgActivitySync(NET_MSG_GENERIC* pMsg)
{
    EMNET_MSG emType = pMsg->Type();
    if (emType < 0 || emType >= NET_MSG_GCTRL_MAX)
        return;

    MyCharMsgProcess( pMsg );
}

void GLGaeaClient::LandEventMsgProcess(NET_MSG_GENERIC* pMsg) 
{
    GLLandEventClient* pLandEvent = m_pLandMClient->GetLandEvent();
    if (pLandEvent)
        pLandEvent->MsgProcess(pMsg);
}


void GLGaeaClient::CharMsgProcess(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETPC_BROAD *pNetMsg = (GLMSG::SNETPC_BROAD *) pMsg;
    if (pNetMsg->dwGaeaID == m_pCharacter->GetGaeaID())
    {
        m_pCharacter->MsgProcess(pMsg);
    }
    else
    {
        std::tr1::shared_ptr<GLCharClient> pChar = m_pLandMClient->GetChar ( pNetMsg->dwGaeaID );
        if (pChar)
            pChar->MsgProcess (pMsg);
        else
            CDebugSet::ToListView("[GLCharClient* NULL] 수신 PC가 없는 메세지 발생. gaeaid %d", pNetMsg->dwGaeaID);
    }
}

void GLGaeaClient::MyCharMsgProcess(NET_MSG_GENERIC* pMsg) 
{
    m_pCharacter->MsgProcess (pMsg);
}

void GLGaeaClient::ActorMsgProcess(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETCROW_BROAD *pNetMsg = (GLMSG::SNETCROW_BROAD *) pMsg;
    ClientActor* pCopy = GetCopyActor ( pNetMsg->emCrow, pNetMsg->dwID );
    if (pCopy)
        pCopy->MsgProcess(pMsg);
}

void GLGaeaClient::CrowMsgProcess(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETCROW *pNetMsg = (GLMSG::SNETCROW *) pMsg;
    std::tr1::shared_ptr<GLCrowClient> spGLCrow = m_pLandMClient->GetCrow(pNetMsg->dwGlobID);	
	if (spGLCrow)
        spGLCrow->MsgProcess(pMsg);
}

void GLGaeaClient::MsgLandInfo(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNETPC_LAND_INFO *pNetMsg = (GLMSG::SNETPC_LAND_INFO *)pMsg;

    PLANDMANCLIENT pLand = GetActiveMap();
    if ( pLand && pLand->GetMapID() == pNetMsg->nidMAP )
    {
        pLand->m_bClubBattle = pNetMsg->bClubBattle;
        pLand->m_bClubBattleHall = pNetMsg->bClubBattleHall;
        pLand->m_bClubDeathMatch = pNetMsg->bClubDeathMatch;
        pLand->m_bClubDeathMatchHall = pNetMsg->bClubDeathMatchHall;
		pLand->m_bClubBattleInProgress = pNetMsg->bClubBattleInProgress;
        pLand->SetPKZone ( pNetMsg->bPK );
        pLand->m_fCommission = pNetMsg->fCommission;

		if ( pNetMsg->bClubBattle )
		{
			pLand->m_dwClubMapID = pNetMsg->dwClubMapID;
		}

        pLand->m_dwGuidClub = pNetMsg->dwGuidClub;
        pLand->m_dwGuidClubMarkVer = pNetMsg->dwGuidClubMarkVer;

        pLand->SetLunchBoxForbid( pNetMsg->bLunchBoxForbid );

        StringCchCopy ( pLand->m_szGuidClubName, CLUB_NAME_LENGTH, pNetMsg->szGuidClubName );

        if ( pLand->m_bClubBattle )
            PrintConsoleTextDlg ( ID2GAMEINTEXT("CLUB_BATTLE_LAND") );

        if ( pLand->m_bClubDeathMatch && m_vecCdmRank.empty() )
        {
            //	정보 요청
            ReqClubDeathMatchInfo();
            PrintConsoleTextDlg ( ID2GAMEINTEXT("CLUB_DEATHMATCH_LAND") );
        }
        else if ( !pLand->m_bClubDeathMatch )
        {
            m_vecCdmRank.clear();
            m_sMyCdmRank.Init();
            //	UI 닫기
            GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Close );
        }

        if (!m_pBaseTutorial->IsTutorial())
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("LAND_TRADE_COMMISSION"), pLand->m_fCommission);
            PrintConsoleTextDlg(strText);

            if ( m_bBRIGHTEVENT )
            {
                std::string strText = sc::string::format( "%1%", ID2GAMEINTEXT("BRIGHT_EVENT_MSG") );
                PrintConsoleTextDlg(strText);
            }

            if ( m_bSCHOOL_FREEPK )
            {
                std::string strText = sc::string::format( "%1%", ID2GAMEINTEXT("OTHER_SCHOOL_FREEPK_ON") );
                PrintConsoleTextDlg(strText);
            }

            // 적대관계 모두 삭제
            GetCharacter()->DEL_PLAYHOSTILE_ALL();
        }
        else
        {
        }

		CString strText(GetMapName ( m_pLandMClient->GetBaseMapID() ));
		const GLMapAxisInfo& sMapAxisInfo = pLand->GetMapAxisInfo();
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Minimap_MoveActiveMap,
			"-s, -s, -b, -dw, -dw, -s",
			strText.GetBuffer(),
			sMapAxisInfo.GetMinMapTex(),
			pLand->IsUIMapSelect() == false,
			GetCharacter()->m_dwServerID,
			pLand->m_dwGuidClub,
			pLand->m_szGuidClubName );
    }

	GLRanMobileClient::GetInstance().ReqInfo();
}

void GLGaeaClient::MsgGmClassEvent(NET_MSG_GENERIC* pMsg) 
{
    GLMSG::SNET_GM_CLASS_EVENT *pNetMsgFB = (GLMSG::SNET_GM_CLASS_EVENT *) pMsg;
    char szClassChar[32] = {0,};
    char szTypeChar[6] = {0,};			
    switch(pNetMsgFB->dwClassIndex)
    {
    case GLCI_FIGHTER_M: strcpy_s( szClassChar, 32, "FIGHT_M" ); break;
    case GLCI_ARMS_M:    strcpy_s( szClassChar, 32,  "ARMS_M" ); break;
    case GLCI_ARCHER_W:  strcpy_s( szClassChar, 32,  "ARCHER_M" ); break;
    case GLCI_SPIRIT_W:  strcpy_s( szClassChar, 32,  "SPIRIT_W" ); break;
    case GLCI_EXTREME_M: strcpy_s( szClassChar, 32,  "EXTREME_M" ); break;
    case GLCI_EXTREME_W: strcpy_s( szClassChar, 32,  "EXTREME_W" ); break;
    case GLCI_FIGHTER_W: strcpy_s( szClassChar, 32,  "FIGHTER_W" ); break;
    case GLCI_ARMS_W:    strcpy_s( szClassChar, 32,  "ARMS_W" ); break;
    case GLCI_ARCHER_M:  strcpy_s( szClassChar, 32,  "ARCHER_M" ); break;
    case GLCI_SPIRIT_M:  strcpy_s( szClassChar, 32,  "SPIRIT_M:" ); break;
    case GLCI_SCIENTIST_M:  strcpy_s( szClassChar, 32,  "SCIENTIST_M:" ); break;
    case GLCI_SCIENTIST_W:  strcpy_s( szClassChar, 32,  "SCIENTIST_W:" ); break;
    case GLCI_ASSASSIN_M:  strcpy_s( szClassChar, 32,  "ASSASSIN_M:" ); break;
    case GLCI_ASSASSIN_W:  strcpy_s( szClassChar, 32,  "ASSASSIN_W:" ); break;
	case GLCI_TRICKER_M:		strcpy_s( szClassChar, 32, "TRICKER_M:"); break;
	case GLCI_TRICKER_W:		strcpy_s( szClassChar, 32, "TRICKER_W:"); break;
    case GLCI_ETC_M:  strcpy_s( szClassChar, 32,  "GUITAR_M:" ); break;
    case GLCI_ETC_W:  strcpy_s( szClassChar, 32,  "GUITAR_W:" ); break;
	case GLCI_ACTOR_M:  strcpy_s( szClassChar, 32,  "ACTOR_M:" ); break;
	case GLCI_ACTOR_W:  strcpy_s( szClassChar, 32,  "ACTOR_W:" ); break;
    //case GLCI_NUM_ETC:  strcpy_s( szClassChar, 32,  "ALL:" ); break;
	case GLCI_NUM_ACTOR:  strcpy_s( szClassChar, 32,  "ALL:" ); break;
    }
    if (pNetMsgFB->dwEventType == 0)
        strcpy_s(szTypeChar, 6, "exp");
    else if (pNetMsgFB->dwEventType == 1)
        strcpy_s(szTypeChar, 6,  "item"); 
    else if (pNetMsgFB->dwEventType == 2)
        strcpy_s(szTypeChar, 6,  "money"); 

    if (pNetMsgFB->bStart)
    {				
        PrintConsoleText(
            sc::string::format(
            "class Event Begin!! : Type %s Class %s Rate : %.2f",
            szTypeChar,
            szClassChar,
            pNetMsgFB->fEventRate));
    }
    else
    {				
        PrintConsoleText(
            sc::string::format(
            "class Event End!! : Type %s Class %s Rate : %.2f",
            szTypeChar,
            szClassChar,
            pNetMsgFB->fEventRate));
    }
}

void GLGaeaClient::MsgReferChar( NET_MSG_GENERIC* nmg )
{
	switch( nmg->nType )
	{
	case NET_MSG_REFER_CHAR_FB:
		m_pReferChar->ReceiveMsgFb( nmg );
		break;

	case NET_MSG_REFER_CHAR_PUTON:
		m_pReferChar->ReceiveMsgPuton( nmg );
		break;

	case NET_MSG_REFER_CHAR_SKILL:
		m_pReferChar->ReceiveMsgSkill( nmg );
		break;

	case NET_MSG_REFER_CHAR_INFO:
		m_pReferChar->ReceiveMsgInfo( nmg );
		break;
	}
}

void GLGaeaClient::MsgPutonUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETPC_PUTON_UPDATE_BRD Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

    if (Data.m_GaeaID == m_pCharacter->GetGaeaID())
    {
        m_pCharacter->MsgProcess(pMsg);
    }
    else
    {
        std::tr1::shared_ptr<GLCharClient> pChar = m_pLandMClient->GetChar( Data.m_GaeaID );
        if (pChar)
            pChar->MsgProcess(pMsg);
        else
            CDebugSet::ToListView("[GLCharClient* NULL] 수신 PC가 없는 메세지 발생. gaeaid %d", Data.m_GaeaID);
    }    
}

void GLGaeaClient::MsgCatchDelay( NET_MSG_GENERIC* pMsg )
{
	//GLMSG::SNET_MSG_CATCH_DELAY* pPacket = ( GLMSG::SNET_MSG_CATCH_DELAY* ) pMsg;
	// 자꾸 메시지 보내지 마라.
	//PrintConsoleTextDlg ( "server is busy." );
}

void GLGaeaClient::AckAcquireEXP(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_GM_ACQUIRE_EXP_ANS* pNetMsg = (GLMSG::SNETPC_GM_ACQUIRE_EXP_ANS*)pMsg;

    std::string strEnable;
    if ( pNetMsg->bEnable )
        strEnable = "on";
    else
        strEnable = "off";

    PrintConsoleText( sc::string::format( "Acquire EXP %1%", strEnable ) );
}

void GLGaeaClient::MsgAcquireEXP(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ACQUIRE_EXP* pNetMsg = (GLMSG::SNETPC_ACQUIRE_EXP*)pMsg;

    PrintConsoleText(
        sc::string::format( ID2GAMEINTEXT( "ACQUIRE_EXP"), pNetMsg->EXP ) );
}

void GLGaeaClient::MsgInstanceSystem(NET_MSG_GENERIC* nmg)
{
    if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == true )
    {
        if ( m_pInstanceManager )
            m_pInstanceManager->_messageProcedure(nmg);
    }
}

void GLGaeaClient::MsgInstanceSystemMsgPack(NET_MSG_GENERIC* pMsg)
{
    if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == true )
    {
        if ( m_pInstanceManager )
            m_pInstanceManager->_messageProcedureMsgPack(pMsg);
    }
}

void GLGaeaClient::MsgInstanceSystemContents(NET_MSG_GENERIC* nmg)
{
    if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == true )
    {
        if ( m_pInstanceManager )
            m_pInstanceManager->_messageProcedure_Contents(nmg);
    }
}

void GLGaeaClient::MsgInstanceSystemContentsMsgPack(NET_MSG_GENERIC* nmg)
{
    if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == true )
    {
        if ( m_pInstanceManager )
            m_pInstanceManager->_messageProcedureMsgPack_Contents(nmg);
    }
}

void GLGaeaClient::MsgInstanceSystemProc ( NET_MSG_GENERIC* pMsg )
{
	if ( GLUseFeatures::GetInstance().IsUsingNewInstance() )
	{
		if ( m_pInstanceManager )
		{
			m_pInstanceManager->OnMessageProc(
				static_cast< const GLMSG::NET_INSTANCE_MESSAGE_HEADER* >( pMsg ) );
		}
	}
}

void GLGaeaClient::MsgFactionSystem(NET_MSG_GENERIC* nmg)
{
    const GLMSG::NET_FACTION_MESSAGE* const _pNetMsg = (const GLMSG::NET_FACTION_MESSAGE* const)nmg;
    if ( !m_pLandMClient )
        return;

    Faction::ManagerClient* const pFactionMan = m_pLandMClient->GetFactionManager();

    if ( pFactionMan == NULL )
        return;

    pFactionMan->messageProcedure(_pNetMsg);
}

void GLGaeaClient::MsgTournamentClient(NET_MSG_GENERIC* pMsg)
{
	if ( !m_pTournament )
		return;
	m_pTournament->_messageProcedure(pMsg);
}

void GLGaeaClient::MsgPing(NET_MSG_GENERIC* nmg)
{
    if ( m_pPingManager == NULL )
        return;

    m_pPingManager->_messageProcedure(nmg);
}


void GLGaeaClient::MsgPackTournamentClient(NET_MSG_GENERIC* pMsg)
{
	if ( !m_pTournament )
		return;
	m_pTournament->_messagePackProcedure(pMsg);
}

void GLGaeaClient::MsgMatchingProc ( NET_MSG_GENERIC* pMsg )
{
	if ( NULL == m_pTournament )
		return;

	m_pTournament->OnMsgProcedure( static_cast< const GLMSG::NET_MATCHING_MESSAGE_HEADER* >( pMsg ) );
}

void GLGaeaClient::MsgPremiumMapExit( NET_MSG_GENERIC* pMsg )
{
	const GLMSG::SNETPC_PREMIUM_MAP_EXIT* const pPacket = (const GLMSG::SNETPC_PREMIUM_MAP_EXIT* const) pMsg;

	if( 0 == pPacket->dwRemainTime )
	{
		PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "PREMIUM_MAP_FORCED_EXIT" ) );
	}
	else
	{
		std::string strNotify = sc::string::format( ID2GAMEINTEXT( "PREMIUM_MAP_FORCED_EXIT_TIME" ), pPacket->dwRemainTime );
		PrintMsgText( NS_UITEXTCOLOR::RED, strNotify );
	}
}

void GLGaeaClient::MsgTexasHoldem( NET_MSG_GENERIC* pMsg )
{
	if ( m_pTexasHoldemMgr )
	{
		m_pTexasHoldemMgr->MessageProcess( pMsg );
	}
}

void GLGaeaClient::MsgEvent(NET_MSG_GENERIC* _pMessage)
{
	if ( m_pEventManager == NULL )
		return;

	m_pEventManager->messageProcedure(_pMessage);
}

void GLGaeaClient::MsgAutoLevelResult(NET_MSG_GENERIC* _pMsg)
{
	const GLMSG::SNET_GM_AUTO_LEVEL_RESULT* const _pResultMessage((const GLMSG::SNET_GM_AUTO_LEVEL_RESULT* const)_pMsg);

	switch ( _pResultMessage->nResult )
	{
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_SUCCESS_BEGIN:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("Success To begin;"));
		break;
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_SUCCESS_END:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("Success To end;"));
		break;
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_SUCCESS_PROCESS:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("Success To process;"));
		break;		
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_WRONG_INDEX:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("wrong Level-Index;"));
		break;
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_NOT_USING:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("can't use that Level;"));
		break;
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_NOT_RUNNING:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("it's not running now;"));
		break;
	case GLMSG::SNET_GM_AUTO_LEVEL_RESULT::EMRESULT_ALREADY_RUNNING:
		PrintMsgText(NS_UITEXTCOLOR::TOMATO, std::string("it's already begun;"));
		break;
	}
}
void GLGaeaClient::MsgAutoLevelList(NET_MSG_GENERIC* _pMsg)
{
	const GLMSG::SNET_GM_AUTO_LEVEL_LIST* const _pLevelListMessage((const GLMSG::SNET_GM_AUTO_LEVEL_LIST* const)_pMsg);

	const unsigned int _nLevelSize(_pLevelListMessage->nLevelSize);

	const std::string _title(sc::string::format("Auto-Level List(total : %1%);", _nLevelSize));
	PrintMsgText(NS_UITEXTCOLOR::TOMATO, _title);

	for ( unsigned int _i(0); _i < _nLevelSize; ++_i )
	{
		const bool _bUse(_pLevelListMessage->levelName[_i].bUse);
		const unsigned int _nIndex(_pLevelListMessage->levelName[_i].nIndex);
		const std::string _levelName(_pLevelListMessage->levelName[_i].levelName);
		const std::string _running(_pLevelListMessage->levelName[_i].bRunning == true ? ", current Running;" : ";");
		const std::string _use(_bUse == true ? " is usable" : " is not usable");	

		const std::string _level(sc::string::format("- ID : %1% [%2%]%3%%4%", _nIndex, _levelName, _use, _running));		

		PrintMsgText(_bUse == true ? NS_UITEXTCOLOR::YELLOW : NS_UITEXTCOLOR::IVORY, _level);
	}	
}

void GLGaeaClient::MsgAutoLevelInformation(NET_MSG_GENERIC* _pMsg)
{
	const GLMSG::SNET_GM_AUTO_LEVEL_INFORMATION* const _pInformationMessage((const GLMSG::SNET_GM_AUTO_LEVEL_INFORMATION* const)_pMsg);

	const DWORD _indexLevel(_pInformationMessage->indexLevel);
	const SNATIVEID& _hallMapID(_pInformationMessage->hallMapID);
	const SNATIVEID& _callMapID(_pInformationMessage->callMapID);
	const GLLEV_TIME& _beginTime(_pInformationMessage->beginTime);
	const GLLEV_TIME& _endTime(_pInformationMessage->endTime);
	const bool* _eventDay(_pInformationMessage->eventDay);

	const std::string _levelNameString(sc::string::format("- ID : %1% [%2%]", _indexLevel, _pInformationMessage->levelName));
	const std::string _hallMapString(sc::string::format(" - start Map : %1%/%2%", _hallMapID.Mid(), _hallMapID.Sid()));
	const std::string _callMapString(sc::string::format(" - event Map : %1%/%2%", _callMapID.Mid(), _callMapID.Sid()));
	const std::string _beginTimeString(sc::string::format(" - begin Time : %1% : %2%", _beginTime.nHour, _beginTime.nMinute));
	const std::string _endTimeString(sc::string::format(" - end Time : %1% : %2%", _endTime.nHour, _endTime.nMinute));

	const std::string _dayWord[7][2] = 
	{
		"Mon, ", "[Mon], ",
		"Tue, ", "[Tue], ",
		"Wed, ", "[Wed], ",
		"Thu, ", "[Thu], ",
		"Fri, ", "[Fri], ",
		"Sat, ", "[Sat], ",
		"Sun", "[Sun]"
	};
	std::string _dayString(" - ");
	for ( unsigned int _index(0); _index < 7; ++_index )
	{
		_dayString += _dayWord[_index][_eventDay[_index] ? 1 : 0];
	}

	PrintMsgText(NS_UITEXTCOLOR::IVORY, "============================================");
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _levelNameString);
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _hallMapString);
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _callMapString);
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _beginTimeString);
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _endTimeString);
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _dayString);
	PrintMsgText(NS_UITEXTCOLOR::IVORY, "============================================");
}

void GLGaeaClient::MsgMobLevList(NET_MSG_GENERIC* pMsg)
{
	const GLMSG::SNET_GM_MOB_LEV_LIST_FC* const _pLayerListMessage((const GLMSG::SNET_GM_MOB_LEV_LIST_FC* const)pMsg);
	const WORD _wMainMapID(_pLayerListMessage->wMAP_MID);
	const WORD _wSubMapID(_pLayerListMessage->wMAP_SID);
	const DWORD _nLayer(_pLayerListMessage->nLayer);

	const std::string _layerInformationString(sc::string::format("Map(%1%/%2%) has %3%-Layer", _wMainMapID, _wSubMapID, _nLayer));	
	PrintMsgText(NS_UITEXTCOLOR::IVORY, "============================================");
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _layerInformationString);
}

void GLGaeaClient::MsgMapLayerNewNotify(NET_MSG_GENERIC* pMsg)
{
	const GLMSG::SNET_GM_MAP_LAYER_NEW_NOTIFY* const _pNewLayerMessage((const GLMSG::SNET_GM_MAP_LAYER_NEW_NOTIFY* const)pMsg);

	const WORD _wMainMapID(_pNewLayerMessage->mapID.Mid());
	const WORD _wSubMapID(_pNewLayerMessage->mapID.Sid());
	const DWORD _nLayerTo(_pNewLayerMessage->nLayerTo);

	const std::string _layerNewString(sc::string::format("Map(%1%/%2%) has been changed to Layer-%3%", _wMainMapID, _wSubMapID, _nLayerTo));	

	PrintMsgText(NS_UITEXTCOLOR::IVORY, "============================================");
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _layerNewString);
}
void GLGaeaClient::MsgMapLayerDelNotify(NET_MSG_GENERIC* pMsg)
{
	const GLMSG::SNET_GM_MAP_LAYER_DEL_NOTIFY* const _pDelLayerMessage((const GLMSG::SNET_GM_MAP_LAYER_DEL_NOTIFY* const)pMsg);

	const WORD _wMainMapID(_pDelLayerMessage->mapID.Mid());
	const WORD _wSubMapID(_pDelLayerMessage->mapID.Sid());

	const std::string _layerDelString(sc::string::format("Map(%1%/%2%) has been changed to default Layer-0", _wMainMapID, _wSubMapID));	
	PrintMsgText(NS_UITEXTCOLOR::IVORY, "============================================");
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _layerDelString);
}
void GLGaeaClient::MsgMapLayerErrorNotify(NET_MSG_GENERIC* pMsg)
{
	const GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY* const _pErrorLayerMessage((const GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY* const)pMsg);

	const WORD _wMainMapID(_pErrorLayerMessage->mapID.Mid());
	const WORD _wSubMapID(_pErrorLayerMessage->mapID.Sid());
	const DWORD _emErrorCode(_pErrorLayerMessage->emErrorCode);

	switch ( _emErrorCode )
	{
	case GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY::EMERROR_WRONG_MAP_ID:
		{
			const std::string _wrongMapIDString(sc::string::format("MapID[%1%/%2%] is INVALID!!", _wMainMapID, _wSubMapID));	
			PrintMsgText(NS_UITEXTCOLOR::RED, _wrongMapIDString);
		}
		break;
	case GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY::EMERROR_DIFFERENT_FIELD_SERVER:
		{
			const std::string _differentFieldServerString0("different Field-Server-Group;");	
			const std::string _differentFieldServerString1("move to another Map in same Field-Server-Group,");
			const std::string _differentFieldServerString2("and try again please;");
			PrintMsgText(NS_UITEXTCOLOR::RED, _differentFieldServerString0);
			PrintMsgText(NS_UITEXTCOLOR::RED, _differentFieldServerString1);
			PrintMsgText(NS_UITEXTCOLOR::RED, _differentFieldServerString2);
		}
		break;
	}
}

void GLGaeaClient::MsgLand(NET_MSG_GENERIC* pMsg)
{
	const GLMSG::SNET_GM_LAND_INFORMATION* const _pInformationLandMessage((const GLMSG::SNET_GM_LAND_INFORMATION* const)pMsg);	
	const unsigned int _mainMapID(_pInformationLandMessage->mapID.Mid());
	const unsigned int _subMapID(_pInformationLandMessage->mapID.Sid());
	const unsigned int _nStage(_pInformationLandMessage->nStage);
	const unsigned int _nUsedMemory(_pInformationLandMessage->nUsedMemory);

	PrintMsgText(NS_UITEXTCOLOR::IVORY, "============================================");
	if ( _nUsedMemory >= 1000 )
	{
		const std::string _informationLandString0(sc::string::format("MapID : %1%/%2%, used %3%,%4%KB;", _mainMapID, _subMapID, _nUsedMemory / 1000, _nUsedMemory % 1000));
		PrintMsgText(NS_UITEXTCOLOR::YELLOW, _informationLandString0);
	}
	else
	{
		const std::string _informationLandString0(sc::string::format("MapID : %1%/%2%, used %3%KB;", _mainMapID, _subMapID, _nUsedMemory));
		PrintMsgText(NS_UITEXTCOLOR::YELLOW, _informationLandString0);
	}
	
	const std::string _informationLandString1(sc::string::format("it has %1%-Stage;", _nStage));	
	
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _informationLandString1);

	for ( unsigned int _index(0); _index < _nStage; ++_index )
	{
		const SNATIVEID& _baseMapID(_pInformationLandMessage->mapIDLand[_index][0]);
		const SNATIVEID& _instanceMapID(_pInformationLandMessage->mapIDLand[_index][1]);

		const std::string _informationLandString(sc::string::format("%1%. baseMapID : %2%/%3%, instanceMapID : %4%/%5%;", _index + 1, _baseMapID.Mid(), _baseMapID.Sid(), _instanceMapID.Mid(), _instanceMapID.Sid()));
		PrintMsgText(NS_UITEXTCOLOR::YELLOW, _informationLandString);
	}
}

void GLGaeaClient::MsgGMWhySoSeriousFB(NET_MSG_GENERIC* pMsg)
{
	const GLMSG::SNET_GM_WHYSOSERIOUS_FB_FAC* const pMessageFB((const GLMSG::SNET_GM_WHYSOSERIOUS_FB_FAC* const)pMsg);	

	const SNATIVEID& _itemID(pMessageFB->sItemID);
	const DWORD _dwNums(pMessageFB->dwNums);
	const std::string _targetName(pMessageFB->szTargetName);

	const std::string& _itemName(GLItemMan::GetInstance().GetItemName(_itemID));

	std::string _resultMessage;
	if ( pMessageFB->bFailed == true )
		_resultMessage = sc::string::format("failed to insert item[%1%] x %2% to [%3%];", _itemName, _dwNums, _targetName);
	else
		_resultMessage = sc::string::format("succes to insert item[%1%] x %2% to [%3%];", _itemName, _dwNums, _targetName);
		
	PrintMsgText(NS_UITEXTCOLOR::YELLOW, _resultMessage);
}

// FlyCameraControl;
void GLGaeaClient::MsgCameraControlFB(NET_MSG_GENERIC* pMsg)
{
	// pMsg.nType == NET_MSG_GM_FLYCAMERACONTROL_EMULATOR_FC
	// pMsg.nType == NET_MSG_GM_FLYCAMERACONTROL_FB
	// GLMSG::SNET_GM_FLYCAMERACONTROL_FB *pNetMsg = (GLMSG::SNET_GM_FLYCAMERACONTROL_FB *) pMsg;
	switch ( pMsg->nType )
	{
	case NET_MSG_GM_FLYCAMERACONTROL_FC:			// Emulator
	case NET_MSG_GM_FLYCAMERACONTROL_FB:
		{
			GLMSG::SNET_GM_FLYCAMERACONTROL_FC* Msg = (GLMSG::SNET_GM_FLYCAMERACONTROL_FC*)pMsg;
			FlyCameraControlOnOff(Msg->bOn);
		}
		break;
	}	
}

void GLGaeaClient::FlyCameraControlOnOff( const bool bFlyCameraOn )
{
	if ( bFlyCameraOn == true )
	{
		GLInputManager::GetInstance().InitControlType( EM_CONTROLTYPE_FLYCAMERA );
		SetChaseCam(STARGETID(CROW_NULL, EMTARGET_NULL), false);
		PrintConsoleText("Fly Camera On!");
	}
	else
	{
		// AType으로 돌림; A,B결정여부는 나중에 기능 추가하던가하자;
		GLInputManager::GetInstance().InitControlType( EM_CONTROLTYPE_A );
		SetChaseCam(STARGETID(CROW_PC, m_pCharacter->GetGaeaID()), true);
		PrintConsoleText("Fly Camera Off!");
	}
}

// Apply Skill Effect
void GLGaeaClient::MsgApplySkillEffectFC ( NET_MSG_GENERIC* pMsg )
{
	if ( !m_pLandMClient )
		return;

	GLMSG::SNET_APPLY_SKILLEFFECT_FC* pNetMsg =
		static_cast< GLMSG::SNET_APPLY_SKILLEFFECT_FC* >( pMsg );
	if ( !pNetMsg )
		return;

	Faction::GLActorID sActorID = pNetMsg->sActorID;
	switch ( sActorID.actorType )
	{
	case CROW_PC:
		{
			std::tr1::shared_ptr< GLCharClient > spChar =
				m_pLandMClient->GetChar( sActorID.actorID_Num );
			if ( spChar )
			{
				spChar->MsgApplySkillEffect( pMsg );
			}
		}
		break;

	case CROW_MOB:
		{
			std::tr1::shared_ptr< GLCrowClient > spCrow =
				m_pLandMClient->GetCrow ( sActorID.actorID_Num );
			if ( spCrow )
			{
				spCrow->MsgApplySkillEffect( pMsg );
			}
		}
		break;

	case CROW_SUMMON:
		{
			std::tr1::shared_ptr< GLSummonClient > spSummon =
				m_pLandMClient->GetSummon ( sActorID.actorID_Num );
			if ( spSummon )
			{
				spSummon->MsgApplySkillEffect( pMsg );
			}
		}
		break;
	};
}

void GLGaeaClient::MsgCountRecorderInfoFB(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_GM_COUNTRECORDER_INFO_FB* pNetMsg = (GLMSG::SNET_GM_COUNTRECORDER_INFO_FB*)pMsg;
	switch ( pNetMsg->dwReqFlag )
	{
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPOUTCROW_CNT:
		{
			const std::string strCrowName(GLCrowDataMan::GetInstance().GetCrowName(pNetMsg->sTargetNID.dwID));
			CString strNativeId;
			strNativeId.Format("CN_%03d_%03d", pNetMsg->sTargetNID.Mid(), pNetMsg->sTargetNID.Sid());
			const std::string strSentense(
				sc::string::format("DropOut Mob name: %s(%s)     count: %d", strCrowName.c_str(), strNativeId.GetString(), pNetMsg->lCount) );
			this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), strSentense.c_str());
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPITEM_CNT:
		{
			const std::string strItemName(GLItemMan::GetInstance().GetItemName(pNetMsg->sTargetNID));
			CString strNativeId;
			strNativeId.Format("IN_%03d_%03d", pNetMsg->sTargetNID.Mid(), pNetMsg->sTargetNID.Sid());
			const std::string strSentense(
				sc::string::format("Drop Item name: %s(%s)     count: %d", strItemName.c_str(), strNativeId.GetString(), pNetMsg->lCount) );
			this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), strSentense.c_str());
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_MONEY_CNT:
		{
			const std::string strSentense(
				sc::string::format("Drop money: %d", pNetMsg->lCount) );
			this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), strSentense.c_str());
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ENTERPLAYER_CNT:
		{
			const std::string strSentense(
				sc::string::format("Enterplayer count: %d", pNetMsg->lCount) );
			this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), strSentense.c_str());
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ELAPSEDTIME:
		{
			const std::string strSentense(
				sc::string::format("Update elapsed time: %d", pNetMsg->lCount) );
			this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), strSentense.c_str());
		}
		break;
	case GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_RESET:
		{
			this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), "countRecorder reset!");
		}
		break;
	}
}

void GLGaeaClient::MsgCharValidation(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_MSG_CHA_VALIDATION_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_VALIDATION_FA*) pMsg;

	m_pCharacter->SetChaStats( pNetMsg->sStats );
	m_pCharacter->SetChaAttack( pNetMsg->dwAttack );
	m_pCharacter->SetChaDefence( pNetMsg->dwDefence );

	this->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), "CHAR VALIDATION ERRO");
}

void GLGaeaClient::MsgInvenItemDecomposeFb(NET_MSG_GENERIC* pMsg)
{
	m_pCharacter->MsgProcess(pMsg);
}

void GLGaeaClient::MsgNpcExchangeItem(NET_MSG_GENERIC* pMsg)
{
	m_pCharacter->MsgProcess(pMsg);
}

void GLGaeaClient::MsgSetNotifyMoney( NET_MSG_GENERIC* pMsg )
{
	GLMSG::NET_GM_NOTIFY_MONEY_CAF* pNetMsg = ( GLMSG::NET_GM_NOTIFY_MONEY_CAF*) pMsg;

	std::string strText = sc::string::format( "Current GLCONST_CHAR::NotifyMoney is %1%", pNetMsg->money );
	PrintConsoleTextDlg(strText);
}

void GLGaeaClient::MyCSStatsMsgProcess(NET_MSG_GENERIC* pMsg) 
{
	m_pCharacter->MsgProcess (pMsg);
}

void GLGaeaClient::MyCSStatsReleaseMsgProcess(NET_MSG_GENERIC* pMsg) 
{
	m_pCharacter->MsgProcess (pMsg);
}

void GLGaeaClient::MsgDrugFbProcess(NET_MSG_GENERIC* pMsg) 
{
	GLMSG::SNETCROW_BROAD *pNetMsg = (GLMSG::SNETCROW_BROAD *) pMsg;
	ClientActor* pCopy = GetCopyActor ( pNetMsg->emCrow, pNetMsg->dwID );
	if (pCopy)
		pCopy->MsgProcess(pMsg);
	//m_pCharacter->MsgProcess (pMsg);
}

void GLGaeaClient::MsgTransToWorldBattleAns(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_ANS* pNetMsg =
		static_cast< GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_ANS* >( nmg );
	if ( pNetMsg )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_BattleServerEntranceFail,
			"-n", (int) pNetMsg->emFlag );
	}
}

void GLGaeaClient::MsgTransToWorldBattleDBAns(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB* pNetMsg = ( GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB*) nmg;
	//m_pInterface->DoModal( ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE" ), UI::MODAL_INFOMATION, UI::OK, UI::MODAL_SERVER_STAGE );
	//m_pInterface->WAITSERVER_DIALOGUE_OPEN ( ID2GAMEINTEXT("WAITSERVER_MESSAGE"), WAITSERVER_TOLOBY );

	if( pNetMsg->nRet == 0 )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_BattleServerEntranceFail,
			"-n", (int) pNetMsg->nRet );
	}
	else
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTI_BattleServerEnterWait );
	}

}

void GLGaeaClient::MsgChangeJoinWorldBattleState(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CHANGE_WBJOIN_STATE* pNetMsg = ( GLMSG::SNET_CHANGE_WBJOIN_STATE*) nmg;

	ToggleJoinWorldBattleButton(pNetMsg->bJoinWorldBattle);
}

void GLGaeaClient::MsgWithVehicleTarget(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_WITH_VEHICLE_TAR *pNetMsg = (GLMSG::SNET_WITH_VEHICLE_TAR *) nmg;
	GLLandManClient* pLand = GetActiveMap();
	if( !pLand )
		return;

	std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar( pNetMsg->dwMasterID );
	if( !pChar )
		return;

	std::string strName( pChar->GetName() );
	// 토너먼트중 파티를 할수가 없다.
	bool bTOURNAMNET = GetTournamentClient()->GetIsTournamentView();

	if ( RANPARAM::bDIS_VEHICLE || bTOURNAMNET ) 
	{
		GLMSG::SNET_WITH_VEHICLE_TAR_ANS NetMsg;
		NetMsg.dwMasterID = pNetMsg->dwMasterID;
		NetMsg.emAns = EMWITH_VEHICLE_CANCEL;
		NETSENDTOAGENT(&NetMsg);

		std::string strText(
			sc::string::format(
			ID2GAMEINTEXT("VEHICLE_FB_CANCEL_AUTO"), strName.c_str()));
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText.c_str());
		return;
	}
	m_dwVehicleRidingRequestTarget = pNetMsg->dwMasterID;
//OUTER_MODAL_RIDE_VEHICLETARGET
	GLWidgetScript::GetInstance().LuaCallFunc<void>(
		NSWIDGET_SCRIPT::g_strFunc_Modal_VehicleWith, "-s", strName.c_str());
}

void GLGaeaClient::MsgMacroOnofFB(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_MACROMODE_ONOFF_FB* pMacroMsg = static_cast<GLMSG::SNET_MACROMODE_ONOFF_FB*>(pMsg);
	
	switch (pMacroMsg->dwMsg)
	{
	case GLMSG::SNET_MACROMODE_ONOFF_FB::MACROON_OK:
		GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_ONOFF, true);
		break;
	case GLMSG::SNET_MACROMODE_ONOFF_FB::MACROOFF_OK:
		GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_ONOFF, false);
		break;
	case GLMSG::SNET_MACROMODE_ONOFF_FB::MACROON_ERROR_LACKTIME:	// 시간 부족;
		PrintConsoleText(CGameTextMan::GetInstance().GetGameWord("MACRO_ACTIVETEXT",0).GetString());
		break;
	case GLMSG::SNET_MACROMODE_ONOFF_FB::MACROON_ERROR_NONE:
	case GLMSG::SNET_MACROMODE_ONOFF_FB::MACROOFF_ERROR_NONE:
		break;
	}
}

void GLGaeaClient::MsgMacroCloseFC(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_MACROMODE_CLOSE_FC* pMacroMsg = static_cast<GLMSG::SNET_MACROMODE_CLOSE_FC*>(pMsg);
	switch ( pMacroMsg->dwMsg )
	{
	case GLMSG::SNET_MACROMODE_CLOSE_FC::MACROCLOSE_TIMEOVER:
		PrintConsoleText( CGameTextMan::GetInstance().GetGameWord("MACRO_STATETEXT", 1).GetString() );
		break;
	}
	GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_SERVEMSG_CLOSE);
}

void GLGaeaClient::MsgMacroStateFC(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC* pMacroMsg = static_cast<GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC*>(pMsg);
	switch ( pMacroMsg->dwMsg )
	{
	case GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC::MACROSTATE_TIMEFILL_NOTIFY:
		{
			PrintConsoleText( sc::string::format( CGameTextMan::GetInstance().GetGameWord("MACRO_STATETEXT", 0).GetString(), pMacroMsg->dwData ) );
			m_pCharacter->FillMacroTime(pMacroMsg->dwData);
		}
		break;
	case GLMSG::SNET_MACROMODE_STATE_NOTIFY_FC::MACROSTATE_TIMEFILL_FAILD_MAXTIME:
		{
			PrintConsoleText(CGameTextMan::GetInstance().GetGameWord("MACRO_STATETEXT", 2).GetString());
		}
		break;
	}
}

void GLGaeaClient::MsgMacroNearestCrowInfoFC(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_MACROMODE_NEARESTCROWINFO_FC* pMacroMsg = static_cast<GLMSG::SNET_MACROMODE_NEARESTCROWINFO_FC*>(pMsg);
	m_vMacroTracePos = pMacroMsg->vPosition;
	GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_TRACETARGETREQ_FEEDBACK, pMacroMsg->bFind);
}

void GLGaeaClient::MsgForceresetSkillStatFC ( NET_MSG_GENERIC* pMsg )
{
	m_pCharacter->MsgProcess( pMsg );
}

void GLGaeaClient::MsgGame( NET_MSG_GENERIC* pMsg )
{
	if( NULL == pMsg )
		return;

	switch( pMsg->nType )
	{
	case NET_MSG_GCTRL_REQ_TAKE_FB :
		MsgReqTakeFB( pMsg );
		break;

	case NET_MSG_SERVER_GENERALCHAT:
		MsgServerGeneralChat( pMsg );
		break ;
	}
}

void GLGaeaClient::MsgServerGeneralChat( NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
	GLMSG::SNET_SERVER_GENERALCHAT ChatData;

	if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), ChatData))
		PrintMsgText( NS_UITEXTCOLOR::CHAT_SYSTEM, ChatData.m_ChatMsg.c_str() );
}

void GLGaeaClient::MsgReqTakeFB( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_REQ_TAKE_FB* pNetMsg = (GLMSG::SNETPC_REQ_TAKE_FB*) pMsg;

	switch( pNetMsg->emTakeFB )
	{
	case EMTAKE_FB_OFF:
		//pNetMsg->emCrow 사라짐. (GUI)
		switch ( pNetMsg->emCrow)
		{
		case CROW_ITEM:
			{
				//PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_10") );
			}
			break;
		case CROW_MONEY:
			{
				//PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_11") );
			}
			break;
		}
		break;
	case EMTAKE_FB_RESERVED:
		{
			PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_12") );
		}
		break;

	case EMTAKE_FB_INVEN_ERROR:
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_13") );
		}
		break;
	case EMTAKE_FB_TOO_EVENT_ITEM:
		{
			PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_14") );
		}
		break;
	case EMTAKE_FB_DISTANCE:
		{
			//PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAKE_FB_DISTANCE") );
		}
		break;
	case EMTAKE_FB_TENDERING:
		{
			PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_33") );
		}
		break;
	};
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.23, Randombox Open시 당첨확률이 5%미만의 경우 공지 메시지를 전달
 * modified : sckim, 2016.09.20, Redmine #4544 확률/아이템 리스트 제어 기능 적용
 *            sckim, 2016.10.05, Redmine #4766 [확률제어시스템] 시스템 메시지 랜덤박스 색상 수정 요청 대응
 */
void GLGaeaClient::MsgRandomboxWinnerNotifyAC(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC* pNetMsg = ( GLMSG::SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC*) nmg;

    GLMSG::NET_CHAT_LINK_FB	NetChatFB;
    {
        std::string strChaName;
        std::string strChatMsg;
        std::string strRandomBox;
        std::string strItemInRandomBox;

        // 링크 정보 설정
        strChaName = std::string(pNetMsg->szTargetName);
	    strRandomBox = sc::string::format( "%s%s%s", strLINK_LEFT_SIMBOL, pNetMsg->sRandomboxID.GETNAME().c_str(), strLINK_RIGHT_SIMBOL );
	    strItemInRandomBox = sc::string::format( "%s%s%s", strLINK_LEFT_SIMBOL, pNetMsg->sItemID.GETNAME().c_str(), strLINK_RIGHT_SIMBOL );
        strChatMsg = sc::string::format( ID2GAMEINTEXT("RANDOMBOX_CHANCE_WINNER"), strChaName, strRandomBox, strItemInRandomBox );

	    const char* pCut0 = strstr( strChatMsg.c_str(), strRandomBox.c_str());
		if ( pCut0 )
		{
			SLINK_TYPE sLinkType0;
			sLinkType0.wMType = ITEM_LINK;
			//sLinkType0.wSType = ITEM_LINK_NORMAL;
			sLinkType0.wSType = ITEM_LINK_INVEN;

			int pCutLen0 = sc::string::ansiToUnicode(pCut0).length();

			int nStart0 = sc::string::ansiToUnicode(strChatMsg).length() -  pCutLen0;
			int nEnd0 = nStart0 + sc::string::ansiToUnicode(strRandomBox).length(); 

			NetChatFB.ADD( &pNetMsg->sRandomboxID, sizeof(pNetMsg->sRandomboxID));
			NetChatFB.sLinkDataRecv[0].sLinkType = sLinkType0;
			NetChatFB.sLinkDataRecv[0].sLinkPos.nStart = nStart0;
			NetChatFB.sLinkDataRecv[0].sLinkPos.nEnd = nEnd0;
			NetChatFB.sLinkDataRecv[0].dwSize = sizeof(pNetMsg->sRandomboxID);	
		}

	    const char* pCut1 = strstr( strChatMsg.c_str(), strItemInRandomBox.c_str());
		if ( pCut1 )
		{
			SLINK_TYPE sLinkType1;
			sLinkType1.wMType = ITEM_LINK;
			sLinkType1.wSType = ITEM_LINK_INVEN;

			int pCutLen1 = sc::string::ansiToUnicode(pCut1).length();

			int nStart1 = sc::string::ansiToUnicode(strChatMsg).length() -  pCutLen1;
			int nEnd1 = nStart1 + sc::string::ansiToUnicode(strItemInRandomBox).length(); 

			NetChatFB.ADD( &pNetMsg->sItemID, sizeof(pNetMsg->sItemID));
			NetChatFB.sLinkDataRecv[1].sLinkType = sLinkType1;
			NetChatFB.sLinkDataRecv[1].sLinkPos.nStart = nStart1;
			NetChatFB.sLinkDataRecv[1].sLinkPos.nEnd = nEnd1;
			NetChatFB.sLinkDataRecv[1].dwSize = sizeof(pNetMsg->sItemID);
		}	

		NetChatFB.emType = CHAT_TYPE_CTRL_GLOBAL;
		NetChatFB.SetName(pNetMsg->szTargetName);
		NetChatFB.SetChatMsg(strChatMsg.c_str());

		//GLChatMan::GetInstance().MsgSystemItemLinkMessage( &NetChatFB );
        GLChatMan::GetInstance().MsgSystemItemAndReferLinkMessage( &NetChatFB );
    }
    /*
    const std::string szBoxName(GLItemMan::GetInstance().GetItemName(pNetMsg->sRandomboxID));
    const std::string szItemName(GLItemMan::GetInstance().GetItemName(pNetMsg->sItemID));

	//PrintConsoleText(sc::string::format("ID2GAMEINTEXT:RANDOMBOX_CHANCE_WINNER:[%1%],[%2%],[%3%]", pNetMsg->szTargetName, szBoxName, szItemName));
    PrintConsoleText(sc::string::format(ID2GAMEINTEXT("RANDOMBOX_CHANCE_WINNER"), pNetMsg->szTargetName, szBoxName, szItemName));
    */
}


void GLGaeaClient::MsgGmRanMobileCommandResponse(NET_MSG_GENERIC* nmg)
{
	GLRanMobileClient::GetInstance().MsgCommandResponse( nmg );
}


void GLGaeaClient::MsgGmResponseUserCountLandFC ( NET_MSG_GENERIC* _pMsg )
{
	GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC* pResponseMsg =
		static_cast< GLMSG::SNETPC_RESPONSE_USERCOUNT_LAND_FC* >( _pMsg );
	if ( NULL == pResponseMsg )
		return;

	DWORD dwType = pResponseMsg->dwType;
	DWORD dwID = pResponseMsg->dwID;
	DWORD dwCount = pResponseMsg->dwCount;

	EMUSERCOUNT_LAND emFlag = pResponseMsg->emFlag;
	if ( emFlag == EMUSERCOUNT_LAND_SUCCESS )
	{
		PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, sc::string::format(
			"Land UserCount, Type : %1%, ID : %2%, Count : %3%",
			dwType,
			dwID,
			dwCount ) );
	}
	else if ( emFlag == EMUSERCOUNT_LAND_FAIL )
	{
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, "Not the same field!! Please go to the same field." );
	}
	else if ( emFlag == EMUSERCOUNT_LAND_NOT_EXIST )
	{
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, "None." );
	}
}

void GLGaeaClient::MsgResponseLockCharSlotFC ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_RESPONSE_LOCK_CHARSLOT_FC* pNetMsg =
		static_cast< GLMSG::SNETPC_RESPONSE_LOCK_CHARSLOT_FC* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	GLCharacter* pCharacter = GetCharacter();
	if ( NULL == pCharacter )
		return;

	EMCHARSLOT_LOCK_FLAG emFlag = pNetMsg->emFlag;

	switch( emFlag )
	{
	case EMCHARSLOT_LOCK_FAIL:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_LOCK_FAIL" ) );
		break;

	case EMCHARSLOT_LOCK_SUCCESS:
		pCharacter->m_arrLockSlotState[ pNetMsg->wCharSlotNum ] = true;

		PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, ID2GAMEINTEXT( "MSG_CHARSLOT_LOCK_SUCCESS" ) );
		break;

	case EMCHARSLOT_LOCK_ALREADY:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_LOCK_ALREADY" ) );
		break;
	}

	// Update UI;
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharInfo_Update );
}

void GLGaeaClient::MsgResponseUnlockCharSlotFC ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC* pNetMsg =
		static_cast< GLMSG::SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	GLCharacter* pCharacter = GetCharacter();
	if ( NULL == pCharacter )
		return;

	EMCHARSLOT_UNLOCK_FLAG emFlag = pNetMsg->emFlag;

	switch( emFlag )
	{
	case EMCHARSLOT_UNLOCK_FAIL:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_UNLOCK_FAIL" ) );
		break;

	case EMCHARSLOT_UNLOCK_SUCCESS:
		pCharacter->m_arrLockSlotState[ pNetMsg->wCharSlotNum ] = false;

		PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, ID2GAMEINTEXT( "MSG_CHARSLOT_UNLOCK_SUCCESS" ) );
		break;

	case EMCHARSLOT_UNLOCK_ALREADY:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_UNLOCK_ALREADY" ) );
		break;
	}

	// Update UI;
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharInfo_Update );
}

void GLGaeaClient::MsgResponseChangeCharSlotFC ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_RESPONSE_CHANGE_CHARSLOT_FC* pNetMsg =
		static_cast< GLMSG::SNETPC_RESPONSE_CHANGE_CHARSLOT_FC* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	GLCharacter* pCharacter = GetCharacter();
	if ( NULL == pCharacter )
		return;

	EMCHARSLOT_CHANGE_FLAG emFlag = pNetMsg->emFlag;

	switch( emFlag )
	{
	case EMCHARSLOT_CHANGE_FAIL:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_FAIL" ) );
		break;

	case EMCHARSLOT_CHANGE_SUCCESS:
		pCharacter->RESET_STATS_SKILL( NULL );

		// Change Character Slot;
		pCharacter->m_wSlotIndex = pNetMsg->wCharSlotNum;

		// Stats;
		pCharacter->m_sStats = pNetMsg->sStats;
		pCharacter->m_wStatsPoint = pNetMsg->wStatPoint;

		// Skill Quick Slot;
		pCharacter->m_wSKILLQUICK_ACT = pNetMsg->wSKILLQUICK_ACT;
		for ( int i=0; i<EMSKILLQUICK_SIZE; ++i )
			pCharacter->m_sSKILLQUICK[ i ] = pNetMsg->sSKILLQUICK[ i ];

		for ( WORD i=0; i<pNetMsg->dwSkillNum; ++i )
		{
			pCharacter->m_ExpSkills.insert ( std::make_pair ( pNetMsg->sSKILL[i].sNativeID.dwID, pNetMsg->sSKILL[i] ) );

			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->sSKILL[i].sNativeID.dwID );
			if ( pSkill && pSkill->m_sEXT_DATA.idDisguiseSkill != NATIVEID_NULL() )
			{
				PGLSKILL pDisguiseSkill = GLSkillMan::GetInstance().GetData ( pSkill->m_sEXT_DATA.idDisguiseSkill );
				if ( pDisguiseSkill )
				{
					std::pair<DWORD,SCHARSKILL> pairMap = 
						std::make_pair( pNetMsg->sSKILL[i].sNativeID.dwID, SCHARSKILL(pDisguiseSkill->m_sBASIC.sNATIVEID, pNetMsg->sSKILL[i].wLevel) );
					pCharacter->m_ExpChangeSkills.insert( pairMap );
				}
			}
		}

		pCharacter->m_dwSkillPoint = pNetMsg->dwSkillPoint;

		// 모든 수치를 다시 초기화 한다;
		pCharacter->INIT_DATA( FALSE, FALSE );

		// 유효한 스킬을 검사한다;
		pCharacter->SET_VALIDATION_EXPSKILL_MAP();

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_ApplyStatPoint );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_IQS_UpdateSlot );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlot );

		PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_SUCCESS" ) );
		break;

	case EMCHARSLOT_CHANGE_LOCK:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_LOCK" ) );
		break;

	case EMCHARSLOT_CHANGE_COOLTIME:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_COOLTIME" ) );
		break;

	case EMCHARSLOT_CHANGE_DIE:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_DIE" ) );
		break;

	case EMCHARSLOT_CHANGE_ACTION:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_ACTION" ) );
		break;

	case EMCHARSLOT_CHANGE_SAMESLOT:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_SAMESLOT" ) );
		break;

	case EMCHARSLOT_CHANGE_LIMITMAP:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_LIMITMAP" ) );
		break;

	case EMCHARSLOT_CHANGE_INVENLACK:
		PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "MSG_CHARSLOT_CHANGE_INVENLACK" ) );
		break;
	}

	// Update UI;
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharInfo_Update );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlot );
}