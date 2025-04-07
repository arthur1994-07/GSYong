#include "pch.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/MinJson.h"

#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"

#include "../Stage/DxGlobalStage.h"
#include "../Friend/GLFriendClient.h"
#include "../Land/GLLandManClient.h"
#include "../Emoticon/Emoticon.h"
#include "../GLGaeaClient.h"
#include "./dxincommand.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GameCommand::GameCommand(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
{
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/attendancemaintenance", boost::bind(&GameCommand::AttendanceMaintenance, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/bighead", boost::bind(&GameCommand::BigHead, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/bighand", boost::bind(&GameCommand::BigHand, this, _1)));
	
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/charinfo_cn", boost::bind(&GameCommand::CharInfoCn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/charinfo_ci", boost::bind(&GameCommand::CharInfoCi, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/chatblock_ua", boost::bind(&GameCommand::ChatBlockUa, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/chatblock_cn", boost::bind(&GameCommand::ChatBlockCn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/chatblock_ci", boost::bind(&GameCommand::ChatBlockCi, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/chatlog", boost::bind(&GameCommand::ChatLog, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/class_event_begin", boost::bind(&GameCommand::ClassEventBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/class_event_end",   boost::bind(&GameCommand::ClassEventEnd, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/clubcmd",   boost::bind(&GameCommand::Club, this, _1)));

#ifndef NDEBUG 
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/create_inst", boost::bind(&GameCommand::CreateInst, this, _1)));
#endif

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/crow_list", boost::bind(&GameCommand::CrowList, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/ctf",       boost::bind(&GameCommand::CTF, this, _1)));

#ifdef _RELEASED
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/match",		boost::bind(&GameCommand::MatchTest, this, _1)));
#endif

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/db",  boost::bind(&GameCommand::Db, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/dbunlock", boost::bind(&GameCommand::DbUnlock, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/dsp", boost::bind(&GameCommand::Dsplay, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/event_ex_begin", boost::bind(&GameCommand::EventExBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/event_ex_end",   boost::bind(&GameCommand::EventExEnd, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventexp_begin", boost::bind(&GameCommand::EventExpBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventexp_end",   boost::bind(&GameCommand::EventExpEnd, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventgrade_begin", boost::bind(&GameCommand::EventGradeBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventgrade_end",   boost::bind(&GameCommand::EventGradeEnd, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventitem_begin", boost::bind(&GameCommand::EventItemBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventitem_end",   boost::bind(&GameCommand::EventItemEnd, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventmoney_begin", boost::bind(&GameCommand::EventMoneyBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/eventmoney_end",   boost::bind(&GameCommand::EventMoneyEnd, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/fps_limit",  boost::bind(&GameCommand::FpsLimit, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/freepk_on",  boost::bind(&GameCommand::FreePkOn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/freepk_off", boost::bind(&GameCommand::FreePkOff, this, _1)));
	
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/genchar_cn", boost::bind(&GameCommand::GenCharCn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/genchar_ci", boost::bind(&GameCommand::GenCharCi, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/lcb", boost::bind(&GameCommand::GuidanceInfo, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/inst_num", boost::bind(&GameCommand::InstNum, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/itemhold", boost::bind(&GameCommand::ItemHold, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/itemlist_update", boost::bind(&GameCommand::ServerItemListUpdate, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/questlist_update", boost::bind(&GameCommand::ServerQuestListUpdate, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/skilllist_update", boost::bind(&GameCommand::ServerSkillListUpdate, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/kickuser_ci", boost::bind(&GameCommand::KickUserCi, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/kickuser_cn", boost::bind(&GameCommand::KickUserCn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/sfpf", boost::bind(&GameCommand::Scaleform_ProfilingOn, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/log2db", boost::bind(&GameCommand::Log2DB, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/login", boost::bind(&GameCommand::Login, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/logitem_reload", boost::bind(&GameCommand::LogItemReload, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/limit_event_begin", boost::bind(&GameCommand::LimitEventBegin, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/limit_event_end",   boost::bind(&GameCommand::LimitEventEnd, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/makeitem", boost::bind(&GameCommand::Whysoserious, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/instance", boost::bind(&GameCommand::Instance, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/minigame", boost::bind(&GameCommand::MiniGame, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/dps", boost::bind(&GameCommand::DPS, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/mob_gen",    boost::bind(&GameCommand::MobGen, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/mob_gen_ex", boost::bind(&GameCommand::MobGenEx, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/mob_del", boost::bind(&GameCommand::MobDel, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/mob_del_ex", boost::bind(&GameCommand::MobDelEx, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/month", boost::bind(&GameCommand::Month, this, _1)));	

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/move2gate", boost::bind(&GameCommand::MoveToGate, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/m2g",       boost::bind(&GameCommand::MoveToGate, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/move2pos", boost::bind(&GameCommand::MoveToPos, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/m2p",      boost::bind(&GameCommand::MoveToPos, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/move2char_cn", boost::bind(&GameCommand::MoveToCharCn, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/m2c_cn",       boost::bind(&GameCommand::MoveToCharCn, this, _1)));
	
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/move2char_ci", boost::bind(&GameCommand::MoveToCharCi, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/m2c_ci",       boost::bind(&GameCommand::MoveToCharCi, this, _1)));
	
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/msgview", boost::bind(&GameCommand::MsgView, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_on",   boost::bind(&GameCommand::NonConfrontOn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_off",  boost::bind(&GameCommand::NonConfrontOff, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_view", boost::bind(&GameCommand::NonConfrontView, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/ping",      boost::bind(&GameCommand::Ping, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/pingtrace", boost::bind(&GameCommand::PingTrace, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/pk_mode",   boost::bind(&GameCommand::PkMode, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/pointshop", boost::bind(&GameCommand::PointShop, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_on",   boost::bind(&GameCommand::NonConfrontOn, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_off",  boost::bind(&GameCommand::NonConfrontOff, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_view", boost::bind(&GameCommand::NonConfrontView, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/pingtrace", boost::bind(&GameCommand::PingTrace, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/pk_mode",   boost::bind(&GameCommand::PkMode, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/pointshop", boost::bind(&GameCommand::PointShop, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/protocol", boost::bind(&GameCommand::Protocol, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/cdm", boost::bind(&GameCommand::CDMOnOff, this, _1)));

#ifdef _RELEASED
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/posttake", boost::bind(&GameCommand::PostTake, this, _1)));
#endif

#ifdef _RELEASED
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/profile", boost::bind(&GameCommand::Profile, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/networkprofile", boost::bind(&GameCommand::NetworkProfile, this, _1)));
#endif

    //m_GmCommandData.insert(COMMAND_DATA_VALUE("/refer",            boost::bind(&GameCommand::ReferCharCommand, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/reload_interface", boost::bind(&GameCommand::UiReload, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/render",           boost::bind(&GameCommand::Render, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/script", boost::bind(&GameCommand::Script, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/shop_info", boost::bind(&GameCommand::ShopInfo, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/showcontentid", boost::bind(&GameCommand::ShowContentId, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/showmethemoney", boost::bind(&GameCommand::ShowMeTheMoney, this, _1)));

#ifndef NDEBUG 
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/summon", boost::bind(&GameCommand::Summon, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/summon_event_skill", boost::bind(&GameCommand::SummonEventSkill, this, _1)));
#endif

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/time",    boost::bind(&GameCommand::Time, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/visible", boost::bind(&GameCommand::Visible, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/warning_msg",   boost::bind(&GameCommand::WarningMessage, this, _1)));
    //m_GmCommandData.insert(COMMAND_DATA_VALUE("/wb",            boost::bind(&GameCommand::WorldBattle, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/weather",       boost::bind(&GameCommand::Weather, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/where_npc",     boost::bind(&GameCommand::WhereNpc, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/where_npc_all", boost::bind(&GameCommand::WhereNpcAll, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/getitem", boost::bind(&GameCommand::Whysoserious, this, _1)));
    
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/whisper", boost::bind(&GameCommand::Whisper, this, _1)));

    // ------------------------------------------------------------------------
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/hack", boost::bind(&GameCommand::Hack, this, _1)));

	// ------------------------------------------------------------------------
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/wb", boost::bind(&GameCommand::WorldBattle, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/clubmasterdelegation", boost::bind(&GameCommand::ClubMasterMandate, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/freepk", boost::bind(&GameCommand::FreePkGMCommand, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/net_msg_delay", boost::bind(&GameCommand::NetNsgDelay, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/pm", boost::bind(&GameCommand::PrivateMarket, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/reference_count", boost::bind(&GameCommand::ReferenceCount, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/load_ipevent", boost::bind(&GameCommand::LoadIPEvent, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/ipevent", boost::bind(&GameCommand::SetIPEvent, this, _1)));

    m_GmCommandData.insert(COMMAND_DATA_VALUE("/acquire_exp", boost::bind(&GameCommand::AcquireEXP, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/reload_trigger", boost::bind(&GameCommand::ReloadTrigger, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/go_stage", boost::bind(&GameCommand::GoStage, this, _1)));

	// Lotto System --------------------------------------------------------------------------------------------------------
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_system_id", boost::bind(&GameCommand::LottoSystemID, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_view_system_id", boost::bind(&GameCommand::LottoViewSystemID, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto", boost::bind(&GameCommand::LottoSystemChangeState, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num_dsp", boost::bind(&GameCommand::LottoSystemNumDSP, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num", boost::bind(&GameCommand::LottoSystemNum, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num_init", boost::bind(&GameCommand::LottoSystemNumInit, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num_add", boost::bind(&GameCommand::LottoSystemNumAdd, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num_del", boost::bind(&GameCommand::LottoSystemNumDel, this, _1 ) ) );

	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_type", boost::bind(&GameCommand::LottoSystemViewDrawingType, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num_set", boost::bind(&GameCommand::LottoSystemSetWinNum, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_num_reset", boost::bind(&GameCommand::LottoSystemResetWinNum, this, _1 ) ) );

	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_cant_buy", boost::bind(&GameCommand::LottoSystemCantBuy, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_can_buy", boost::bind(&GameCommand::LottoSystemCanBuy, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE("/lotto_turn", boost::bind(&GameCommand::LottoSystemTurnControl, this, _1 ) ) );

	// Lotto System Develop Mode -------------------------------------------------------------------------------------------
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/lotto_buy_ticket", boost::bind(&GameCommand::LottoBuyTicket, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/lotto_accumulate_money", boost::bind(&GameCommand::LottoAccumulateMoney, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/lotto_view_buy_list", boost::bind(&GameCommand::LottoViewBuyList, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/lotto_view_drawing_date", boost::bind(&GameCommand::LottoViewDrawingDate, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/lotto_view_win_num", boost::bind(&GameCommand::LottoWinNum, this, _1 ) ) );
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/lotto_view_win_man_list", boost::bind(&GameCommand::LottoViewWinManList, this, _1 ) ) );
	// ---------------------------------------------------------------------------------------------------------------------

	// Reload AI FSM -------------------------------------------------------------------------------------------------------
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/reload_ai_fsm", boost::bind(&GameCommand::ReloadAIFSM, this, _1)));
	// ---------------------------------------------------------------------------------------------------------------------

	// Change Basic Hire Summon --------------------------------------------------------------------------------------------
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/change_basic_hiresummon", boost::bind(&GameCommand::ChangeBasicHireSummon, this, _1)));
	// ---------------------------------------------------------------------------------------------------------------------

	// Show Faction Information Window -------------------------------------------------------------------------------------
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/show_faction_info", boost::bind(&GameCommand::ShowFactionInformation, this, _1)));
	// ---------------------------------------------------------------------------------------------------------------------

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/autolevel", boost::bind(&GameCommand::AutoLevel, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/maplayer", boost::bind(&GameCommand::MapLayer, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/matching", boost::bind(&GameCommand::Matching, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/mt", boost::bind(&GameCommand::Matching, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/land", boost::bind(&GameCommand::Land, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/texasholdem", boost::bind(&GameCommand::TexasHoldem, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/flycameracontrol", boost::bind(&GameCommand::FlyCameraControlOn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/camera", boost::bind(&GameCommand::FlyCameraControlCommand, this, _1)));

	// Post Renew
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/post", boost::bind(&GameCommand::PostRenew, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/countrecorder", boost::bind(&GameCommand::CountRecorder, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/gm",   boost::bind(&GameCommand::gm, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/whois",   boost::bind(&GameCommand::Whois, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/check",   boost::bind(&GameCommand::Check, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/safemode_test_on",   boost::bind(&GameCommand::SafeModeTestOn, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/for",   boost::bind(&GameCommand::For, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/refer",   boost::bind(&GameCommand::ReferCharCommand, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/repair",   boost::bind(&GameCommand::ItemRepairCommand, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/private_market",   boost::bind(&GameCommand::PrivateMarketCommand, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/event", boost::bind(&GameCommand::Event, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/set_log_money", boost::bind(&GameCommand::SetNotifyMoney, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/changeschool", boost::bind(&GameCommand::ChangeSchool, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/transdb", boost::bind(&GameCommand::TransDB2WorldBattle, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/jumping", boost::bind(&GameCommand::JumpingLevelUp, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/showlog", boost::bind(&GameCommand::ShowLog, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/joinworldbattle", boost::bind(&GameCommand::ChangeWBJoinState, this, _1)));

	m_GmCommandData.insert(COMMAND_DATA_VALUE("/queststart", boost::bind(&GameCommand::QuestSelfStart, this, _1)));

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14, 신규등록
     * modified : 
     */
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/setrandomchance", boost::bind(&GameCommand::SetRandomboxChance, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/getrandomchance", boost::bind(&GameCommand::GetRandomboxChance, this, _1)));
    m_GmCommandData.insert(COMMAND_DATA_VALUE("/delrandomchance", boost::bind(&GameCommand::DelRandomboxChance, this, _1)));

	// 란모바일
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/ranmobile", boost::bind(&GameCommand::RanMobileCommand, this, _1)));
	m_GmCommandData.insert(COMMAND_DATA_VALUE("/rm", boost::bind(&GameCommand::RanMobileCommandEx, this, _1)));

	// Information
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/info", boost::bind( &GameCommand::InformationCommand, this, _1 ) ) );

	// Character Slot
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/charslot", boost::bind( &GameCommand::CharSlotCommand, this, _1 ) ) );

	///////////////////////////////////////////////////////////////
	// User GM이 사용가능한 명령어

	// 유저 GM Master 등급이 나오기 전까지는 해당 기능을 막음
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/where_npc",     boost::bind(&GameCommand::WhereNpc, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/where_npc_all", boost::bind(&GameCommand::WhereNpcAll, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/visible", boost::bind(&GameCommand::Visible, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/countrecorder", boost::bind(&GameCommand::CountRecorder, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/msgview", boost::bind(&GameCommand::MsgView, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/crow_list", boost::bind(&GameCommand::CrowList, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/go_stage", boost::bind(&GameCommand::GoStage, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/autolevel", boost::bind(&GameCommand::AutoLevel, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/acquire_exp", boost::bind(&GameCommand::AcquireEXP, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/clubcmd",   boost::bind(&GameCommand::Club, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_on",   boost::bind(&GameCommand::NonConfrontOn, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_off",  boost::bind(&GameCommand::NonConfrontOff, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/nonconfront_view", boost::bind(&GameCommand::NonConfrontView, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/bighead", boost::bind(&GameCommand::BigHead, this, _1)));
	//m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/bighand", boost::bind(&GameCommand::BigHand, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/chatlog", boost::bind(&GameCommand::ChatLog, this, _1)));

	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/whisper", boost::bind(&GameCommand::Whisper, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/weather",       boost::bind(&GameCommand::Weather, this, _1)));
    m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/move2char_cn", boost::bind(&GameCommand::MoveToCharCn, this, _1)));
    m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/m2c_cn",       boost::bind(&GameCommand::MoveToCharCn, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/genchar_cn", boost::bind(&GameCommand::GenCharCn, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/chatblock_cn", boost::bind(&GameCommand::ChatBlockCn, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/ping",      boost::bind(&GameCommand::Ping, this, _1)));
    m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/kickuser_ci", boost::bind(&GameCommand::KickUserCi, this, _1)));
	m_UserGmCommandData.insert(COMMAND_DATA_VALUE("/kickuser_cn", boost::bind(&GameCommand::KickUserCn, this, _1)));

	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/jackpot", boost::bind( &GameCommand::Jackpot, this, _1 ) ) );

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
	m_GmCommandData.insert( COMMAND_DATA_VALUE( "/rc_reload", boost::bind( &GameCommand::RandomboxRCReload, this, _1 ) ) );
}

GameCommand::~GameCommand()
{
}

CString GameCommand::BOOL2STR(bool bOn)
{
	return bOn ? "ON" : "OFF";
}

/*
bool GameCommand::command(const char* szcommand)
{
	if (szcommand==NULL || szcommand[0]==NULL)
        return false;
	if (szcommand[0]!='/')
        return false;
    std::string strCommandOriginal(szcommand);
    return command(strCommandOriginal);
}
*/

bool GameCommand::command( const std::string& Command )
{
	if ( Command.empty() )
        return false;
    if ( Command.at(0) !='/' )
        return false;

    std::string strCommandOriginal( Command );
    splitVector SplitVec;
    // \t ' ' ',' 토큰분리
    boost::split( SplitVec, strCommandOriginal, boost::algorithm::is_any_of( "\t ," ), boost::algorithm::token_compress_on );
    if ( SplitVec.size() == 0 )
        return true;
	
	CDebugSet::ToListView( strCommandOriginal.c_str() );

    std::string strCommand = SplitVec[0];

	//	Note : 제스쳐.
	int nMOTION = CEmoticon::GetInstance().GetEmoticonOfMotion( strCommand );
	if ( nMOTION != UINT_MAX )
	{
		bool bACTION = m_pGaeaClient->GetCharacter()->ReqGESTURE( nMOTION, true );
		if ( bACTION )
            return true;
	}

	if ( strCommand == "/?" )
	{
		Help( SplitVec );
		return true;
	}

	// GM 명령.
	if ( m_pGaeaClient )
	{
		DWORD dwUSERLVL = m_pGaeaClient->GetCharacter()->UserLevel();
		
		if ( dwUSERLVL >= USER_GM3 )
		{
			sc::string::lowerString( strCommand );
			COMMAND_DATA_ITER iter = m_GmCommandData.find( strCommand );
			if ( iter == m_GmCommandData.end() )
			{
				m_pGaeaClient->PrintConsoleText( sc::string::format( "invalid command by GM:%1%", strCommand ) );
			}
			else
			{
				COMMAND_FUNC& CommandFunc = iter->second;
				bool Result = CommandFunc( SplitVec );
				if ( Result )
				{
					// gm command log save
					commandlog( SplitVec );
				}
			}
		}
		else if( dwUSERLVL >= USER_USER_GM && dwUSERLVL < USER_GM3 )
		{
			// 함수화 해야하는데 귀차니즘이란...
			sc::string::lowerString( strCommand );
			COMMAND_DATA_ITER iter = m_UserGmCommandData.find( strCommand );
			if ( iter == m_UserGmCommandData.end() )
			{
				m_pGaeaClient->PrintConsoleText( sc::string::format( "invalid command by User GM:%1%", strCommand ) );
			}
			else
			{
				COMMAND_FUNC& CommandFunc = iter->second;
				bool Result = CommandFunc( SplitVec );
				if ( Result )
				{
					// gm command log save
					commandlog( SplitVec );
				}
			}
		}
		else
		{
			return true;
		}
	}

	return true;
}

void GameCommand::commandlog( splitVector& command )
{
	if ( command.empty() )
	{
		return;
	}

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
	{
		return;
	}

	std::string strCommand( command[0] );
	bool bParam = false;

	json_spirit::Object message;
	message.push_back( json_spirit::Pair( "M", jm::MSG_GM_COMMAND_LOG_CA ) );
	message.push_back( json_spirit::Pair( "COMMAND", strCommand ) );

	size_t size = command.size();
	if ( size > 1 )
	{
		std::string strGmCommandParam( "" );
		size_t loof = 1;
		while ( loof < size )
		{
			strGmCommandParam += command[loof];
			loof++;
			if ( loof < size )
			{
				strGmCommandParam += " ";
			}
		}

		message.push_back( json_spirit::Pair( "PARAM", strGmCommandParam ) );
		bParam = true;
	}
	message.push_back( json_spirit::Pair( "HAVEPARAM", bParam ) );
	
	NET_JSON MsgCA;
	if ( MsgCA.SetData( json_spirit::write( message ) ) )
		m_pGaeaClient->NETSENDTOAGENT( &MsgCA );
}
