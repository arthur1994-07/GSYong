#include "pch.h"
#include <boost/lexical_cast.hpp>
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringFormatW.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../GLogicDataMan.h"
#include "../GLLevelFile.h"
#include "./MapAxisInfoMan.h"
#include "./GLMapList.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


SMAPNODE::SMAPNODE(void)
    : m_bUse(false)
    , m_FieldServerID(0)
    , m_bBattleZone(false)
    , m_bPeaceZone(false)
    , m_bCommission(true)
    , m_bPKZone(false)
    , m_bFreePK(false)
    , m_bItemDrop(false)
    , m_bFriendMove(false)
    , m_bRestart(false)
    , m_bDecExp(false)
    , m_bPetActivity(false)
    , m_bVehicleActivity(false)
    , m_bClubBattleZone(false)
    , m_bCDMZone(false)
    , m_bCaptureTheField(false)
	, m_emInstanceType(SMAPNODE::EMINSTANCE_NORMAL)
    , m_bQBoxDrop(false)
    , m_bTutorialMap(false)
    , m_bLunchBoxForbid(false)
    , m_bCPReset(false)
    , m_bPKMap(false)
    , m_bUIMapSelect(false)
    , m_bUIMapInfo(false)
	, m_bPartyMap(false)
	, m_bExpeditionMap(false)
    , m_bClubPkRecord(false)
    , m_bPrivateMarketOpenable(false)
    , m_bSparring(false)
	, m_bCullByObjectMap(false)
	, m_bRebirthBlock(false)
	, m_bDropBlockItem(false)
	, m_bDisableSwitchMob(true)
	, m_fRateExpDrop(1.0f)
	, m_fRateItemDrop(1.0f)
	, m_bChangeSchool(true)
	, m_bMacro(true)
	, m_bPrison(false)
	, m_bChangeCharSlot(false)
{
    m_pLevelRequire = new SLEVEL_REQUIRE;
    m_pLevelEtcFunc = new SLEVEL_ETC_FUNC;
}

SMAPNODE::~SMAPNODE()
{
    delete m_pLevelRequire;
    delete m_pLevelEtcFunc;
}

SMAPNODE::SMAPNODE(const SMAPNODE& value)    
{ // 생성자 초기화 해주면 좋으나, 실수를 방지 하기 위해 =연산자 호출로 대체함;
    m_pLevelRequire = new SLEVEL_REQUIRE;
    m_pLevelEtcFunc = new SLEVEL_ETC_FUNC;
    operator = (value); 
}

SMAPNODE& SMAPNODE::operator = (const SMAPNODE& value)
{
    if (this == &value)
        return *this;

	m_bUse					= value.m_bUse;
    m_LevelFile				= value.m_LevelFile;
    m_NativeID				= value.m_NativeID;    
    m_FieldServerID		= value.m_FieldServerID;

	m_bBattleZone			= value.m_bBattleZone;
    m_bPeaceZone		= value.m_bPeaceZone;
    m_bCommission		= value.m_bCommission;
    
    m_bPKZone				= value.m_bPKZone;
    m_bFreePK				= value.m_bFreePK;
	m_bClubPkRecord		= value.m_bClubPkRecord;

    
    m_bItemDrop			= value.m_bItemDrop;
    m_bFriendMove		= value.m_bFriendMove;
    m_bRestart				= value.m_bRestart;
    m_bPetActivity		= value.m_bPetActivity;
    m_bDecExp				= value.m_bDecExp;
    m_bVehicleActivity	= value.m_bVehicleActivity;

    m_bClubBattleZone	= value.m_bClubBattleZone;
    m_bCDMZone			= value.m_bCDMZone;
	m_bCaptureTheField	= value.m_bCaptureTheField;

	m_MapID				= value.m_MapID;
	m_BgmName			= value.m_BgmName;
	m_LoadingImageName	= value.m_LoadingImageName;

	m_emInstanceType	= value.m_emInstanceType;
	m_bQBoxDrop			= value.m_bQBoxDrop;
    m_bLunchBoxForbid	= value.m_bLunchBoxForbid;
	m_bCPReset			= value.m_bCPReset;
	m_bPKMap				= value.m_bPKMap;
    
    m_bUIMapSelect		= value.m_bUIMapSelect;
    m_bUIMapInfo			= value.m_bUIMapInfo;
	m_bPartyMap			= value.m_bPartyMap;
	m_bExpeditionMap	= value.m_bExpeditionMap;

    m_bPrivateMarketOpenable		= value.m_bPrivateMarketOpenable;
    m_bSparring			= value.m_bSparring;
	m_bCullByObjectMap	= value.m_bCullByObjectMap;
    
    m_bTutorialMap = value.m_bTutorialMap;    

    *m_pLevelRequire = *value.m_pLevelRequire;
    *m_pLevelEtcFunc = *value.m_pLevelEtcFunc;

	m_prohibiteditems	= value.m_prohibiteditems;
	m_bRebirthBlock		= value.m_bRebirthBlock;

	m_bDropBlockItem = value.m_bDropBlockItem;

	m_bDisableSwitchMob = value.m_bDisableSwitchMob;

	m_fRateExpDrop = value.m_fRateExpDrop;
	m_fRateItemDrop = value.m_fRateItemDrop;

	m_bChangeSchool = value.m_bChangeSchool;
	m_bMacro = value.m_bMacro;

	m_bPrison = value.m_bPrison;

	m_bChangeCharSlot = value.m_bChangeCharSlot;

    return *this;
}

