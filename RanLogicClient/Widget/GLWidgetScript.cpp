#include "pch.h"
#include "GLWidgetScript.h"

#include "../../enginelib/Common/SUBPATH.h"
#include "../../enginelib/DxTools/PrivateResource.h"
#include "../../enginelib/DxTools/DxFontMan.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../RanGfxUI/GfxInterface.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Club/ClubAuthData.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"

#include "../GLGaeaClient.h"
#include "./GLWidgetsList.h"
#include "./GLWidgetMsgInterface.h"
#include "./GLWidgetSaveProp.h"

#include "../StaticHoldItem.h"
#include "../Tool/DxParamSet.h"
#include "../Login/GLLoginClient.h"
#include "../LobyCharacter/GLLobyCharClient.h"
#include "../LobyCharacter/GLLobyCharCreateClient.h"
#include "../Char/GLCharacter.h"
#include "../Party/GLPartyManagerClient.h"
#include "../Input/GLInputManager.h"
#include "../NPCTalk/GLNpcTalkClient.h"
#include "../PointShop/PointShopClient.h"
#include "../Product/GLProductClient.h"
#include "../Pet/GLPetClient.h"
#include "../PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../Pet/GLPetClient.h"
#include "../Friend/GLFriendClient.h"
#include "../GroupChat/GroupChatClient.h"
#include "../Tutorial\GLBaseTutorial.h"
#include "../RanLogicClient/Pet/GLPetClient.h"
#include "../Post/GLPostClient.h"
#include "../Club/GLClubClient.h"
#include "../Competition/Competition.h"
#include "../ReferChar/ReferChar.h"
#include "../MiniGame/GLMiniGameManager.h"
#include "../Attendance/GLAttendanceClient.h"
#include "../Activity/GLActivityClient.h"
#include "../DamageRecord/DamageRecord.h"
#include "../ItemPreview/GLItemPreviewClient.h"
#include "../InstanceSystem/InstanceSystemClient.h"
#include "../RebuildCard/RebuildCard.h"
#include "../Minimap/GLMinimapClient.h"
#include "../LottoSystem/GLLottoSystemManClient.h"
#include "../Transport/GLTaxiStationClient.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../FlyCameraControl/GLFlyCameraControl.h"
#include "../Transport/GLTaxiStationClient.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../Transport/GLBusStationClient.h"
#include "../PVE/GLPVEClient.h"
#include "../Country/GLCountryManClient.h"
#include "../Country/GLVictoriousCountryManClient.h"
#include "../CostumeStat/CotStatClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Macro/MacroManager.h"
#include "../RanMobile/GLRanMobileClient.h"

namespace NSWIDGET_SCRIPT_LIST
{
	const std::string g_strScriptList = "widgetscriptlist.lua";
	const std::string g_strWidgetScriptFile = "widgetscript.wsf";
}

GLWidgetScript::GLWidgetScript(void)
:	m_pGaeaClient( NULL )
,	m_pInterface( NULL )
,	m_pLuaState( NULL )
,	m_bReload( false )
,	m_bInitialize(false)
{
}

GLWidgetScript::GLWidgetScript( const GLWidgetScript& value )
{
}

GLWidgetScript::~GLWidgetScript(void)
{
	if( NULL != m_pLuaState )
	{
		lua_close( m_pLuaState );
		m_pLuaState = NULL;
	}
}

GLWidgetScript& GLWidgetScript::GetInstance()
{
	static GLWidgetScript Instance;
	return Instance;
}

void GLWidgetScript::Reload()
{
	m_bReload = true;
}

HRESULT GLWidgetScript::Init( GLGaeaClient* pClient, GfxInterfaceBase* pInterface, SUBPATH* pPath )
{
	m_bInitialize = false;

	m_pGaeaClient = pClient;
	m_pInterface = pInterface;
	m_pPath = pPath;

	if( NULL == m_pGaeaClient || NULL == pInterface )
		return E_FAIL;

	// 루아 스테이트 초기화
	m_pLuaState = lua_open();
	luaL_openlibs( m_pLuaState );

	// 전역 변수 초기화
	InitGlobal();
	// 함수 등록
	RegFunc();

	// 루아 파일 로드
	if( FAILED( LoadFile( pPath ) ) )
		return E_FAIL;

	// 루아 디파인 값 로드
	LoadDefine();

	m_bInitialize = true;

	return S_OK;
}

void GLWidgetScript::OnFrameMove( float fElapsedTime )
{
	GLWidgetMsgInterface::GetInstance().FrameMove( fElapsedTime );

	LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_FrameMove, "-f", fElapsedTime );

	if( true == m_bReload )
	{
		if( NULL != m_pLuaState )
		{
			lua_close( m_pLuaState );
			m_pLuaState = NULL;
		}

		Init( m_pGaeaClient, m_pInterface, m_pPath );

		m_bReload = false;
	}
}

HRESULT GLWidgetScript::LoadFile( const SUBPATH* pPath )
{
	if( NULL == m_pLuaState )
		return E_FAIL;

#if defined (_RELEASED) || defined (_DEBUG)
	// [shhan][2015.12.23] ReleaseD 와 Debug 는 무조건 각각의 파일들을 읽도록 한다.
	//
	//// ReleaseD 와 Debug 는 wsf 있을 경우 release 와 동일하게 동작하고
	//// 없다면 각각의 파일들을 읽도록 한다.
	////
	//std::string strPathWSF( sc::getAppPath() );
	//strPathWSF.append( pPath->WidgetScriptPath() );
	//strPathWSF.append( NSWIDGET_SCRIPT_LIST::g_strWidgetScriptFile );

	//FILE* pFile(NULL);
	//std::string strPathTemp( strPathWSF );
	//strPathTemp.append( _T(".temp") );
	//
	//CPrivateResource cResource( CPrivateResource::ETypeWidgetScript );
	//if ( pFile = cResource.LoadFile_Decrypt_NewFile( strPathWSF.c_str(), strPathTemp.c_str() ) )
	//{
	//	if ( !LoadMemory( pFile, strPathTemp ) )
	//	{
	//		return E_FAIL;
	//	}
	//}
	//else
	//{
		// 위젯 리스트 로드
		if( false == GLWidgetsList::GetInstance().LoadFile( m_pLuaState, pPath ) )
			return E_FAIL;

		std::string strScriptListPath( sc::getAppPath() );
		strScriptListPath.append( pPath->WidgetScriptPath() );
		m_strScriptPath = strScriptListPath;
		strScriptListPath.append( NSWIDGET_SCRIPT_LIST::g_strScriptList );

		// 스크립트 리스트 로드
		if( false == lua_tinker::dofile( m_pLuaState, strScriptListPath.c_str() ) )
			return E_FAIL;

		lua_tinker::table luaScriptList = lua_tinker::get< lua_tinker::table >( m_pLuaState, "WIDGET_SCRIPT_LIST" );
		for( int i = 1; i <= luaScriptList.TableLen(); ++i )
		{
			// 스크립트 리스트에서 위젯에 사용되는 스크립트를 가져온다.
			std::string strScriptPath = m_strScriptPath;
			strScriptPath.append( luaScriptList.get< const char* >( i ) );

			// 가져온 스크립트 로드
			lua_tinker::dofile( m_pLuaState, strScriptPath.c_str() );
		}
	//}

#else
	// Release 는 순수하게 wsf 만 읽도록 한다.
	//
	std::string strPathWSF( sc::getAppPath() );
	strPathWSF.append( pPath->WidgetScriptPath() );
	strPathWSF.append( NSWIDGET_SCRIPT_LIST::g_strWidgetScriptFile );
	
	FILE* pFile(NULL);
	std::string strPathTemp( strPathWSF );
	strPathTemp.append( _T(".temp") );

	CPrivateResource cResource( CPrivateResource::ETypeWidgetScript );
	if ( pFile = cResource.LoadFile_Decrypt_NewFile( strPathWSF.c_str(), strPathTemp.c_str() ) )
	{
		if ( !LoadMemory( pFile, strPathTemp ) )
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}
#endif

	return S_OK;
}

// wsf 파일을 기준으로 WidgetScript 들을 로딩한다.
// 넘어온 pZipFile 는 현 함수안에서 무조건 fclose()가 된다.
// pZipFile			: Zip 파일
// strPathWSF_ZIP	: Zip 파일의 Full Path.
BOOL GLWidgetScript::LoadMemory( FILE* pZipFile, const std::string& strPathWSF_ZIP )
{
	BOOL bReturn(TRUE);
	if ( ZipMan::GetInstance().LOAD_Memory( strPathWSF_ZIP, pZipFile ) )
	{
		bReturn = LoadFileInZip( strPathWSF_ZIP );
		ZipMan::GetInstance().DeleteMemory( strPathWSF_ZIP );
	}
	else
	{
		fclose( pZipFile );
	}

	return bReturn;
}

BOOL GLWidgetScript::LoadFileInZip( const std::string& strPathWSF_ZIP )
{
	// 위젯 리스트 로드
	if( false == GLWidgetsList::GetInstance().LoadBufferZIP( m_pLuaState, strPathWSF_ZIP ) )
		return FALSE;

	// 스크립트 리스트 로드
	BYTE* pByte(NULL);
	std::string strLucFile( sc::file::changeFileExt( NSWIDGET_SCRIPT_LIST::g_strScriptList, "luc" ) );
	std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	UINT size = ZipMan::GetInstance().UnzipToMemory(strPathWSF_ZIP, strLucFile, pByte);
	if ( size <= 0 || pByte == NULL )
	{
		return FALSE;
	}

	if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	{
		return FALSE;
	}

	// 스크립트 리스트에서 위젯에 사용되는 스크립트를 가져온다.
	lua_tinker::table luaScriptList = lua_tinker::get< lua_tinker::table >( m_pLuaState, "WIDGET_SCRIPT_LIST" );
	for( int i = 1; i <= luaScriptList.TableLen(); ++i )
	{
		strLucFile = sc::file::changeFileExt( std::string(luaScriptList.get< const char* >( i )), "luc" );
		std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
		size = ZipMan::GetInstance().UnzipToMemory(strPathWSF_ZIP, strLucFile, pByte);
		if ( size <= 0 || pByte == NULL )
		{
			return FALSE;
		}

		if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void GLWidgetScript::LoadDefine()
{
	// 저장할 UI 프로퍼티 테이블
	LuaTable tbSaveProp = GET_LUA_DEF( LuaTable, "WIDGET_SAVE_PROP" );
	GLWidgetSaveProp::GetInstance().Load( tbSaveProp );

	using namespace NSWIDGET_SCRIPT;

	//------------------------------------------------------------------------

	g_bDef_WidgetCLoseToDeleteUseBatch = GET_LUA_DEF( bool, "WIDGET_CLOSE_TO_DELETE_USEBATCH" );
	g_fDef_WidgetCloseToDeleteTime = GET_LUA_DEF( float, "WIDGET_CLOSE_TO_DELETE_TIME" );

	g_nDef_SlotTooltipMaxWidth = GET_LUA_DEF( int, "SLOT_TOOLTIP_MAX_WIDTH" );
	g_nDef_SlotTooltipMinWidth = GET_LUA_DEF( int, "SLOT_TOOLTIP_MIN_WIDTH" );

	g_dwDef_ItemSlotCountColor = GET_LUA_DEF( DWORD, "ITEMSLOT_COUNT_COLOR" );
	g_dwDef_ItemSlotMaxCountColor = GET_LUA_DEF( DWORD, "ITEMSLOT_COUNT_MAX_COLOR" );

	//------------------------------------------------------------------------
	// 디스플레이 정의값

	g_fDef_Display_ShowNameDistanceRatio = GET_LUA_DEF( float, "g_fDisplay_ShowNameDistanceRatio" );
	g_strDef_Display_NameMC = GET_LUA_DEF( const char*, "g_strDisplay_NameMC" );
	g_strDef_Display_NameMC_tfName = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_tfName" );
	g_strDef_Display_NameMC_mcNameBG = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_mcNameBG" );
	g_strDef_Display_NameMC_tfClub = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_tfClub" );
	g_strDef_Display_NameMC_mcClubBG = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_mcClubBG" );
	g_strDef_Display_NameMC_mcSchool = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_mcSchoolIcon" );
	g_strDef_Display_NameMC_mcCountry = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_mcCountryImg" );
	g_strDef_Display_NameMC_mcParty = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_mcPartyIcon" );
	g_strDef_Display_NameMC_rtClubIcon = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_rtClubIcon" );
	g_strDef_Display_NameMC_rtClubIconMC = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_rtClubIconMC" );
	g_strDef_Display_NameMC_rtClubIconLink = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_rtClubIconLink" );
	g_strDef_Display_NameMC_barHP = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_barHP" );
	g_nDef_Display_NameMC_bgOffsetX = GET_LUA_DEF( int, "g_nDisplay_NameMC_bgOffsetX" );
	g_nDef_Display_NameMC_bgOffsetY = GET_LUA_DEF( int, "g_nDisplay_NameMC_bgOffsetY" );
	g_strDef_Display_ItemNameMC = GET_LUA_DEF( const char*, "g_strDisplay_ItemNameMC" );

	g_strDef_Display_NameMC_NoSchool = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_NoSchool" );
	g_strDef_Display_NameMC_SMSchool = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_SMSchool" );
	g_strDef_Display_NameMC_HASchool = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_HASchool" );
	g_strDef_Display_NameMC_BHSchool = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_BHSchool" );

	g_strDef_Display_NameMC_NoParty = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_NoParty" );
	g_strDef_Display_NameMC_PartyMaster = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_PartyMaster" );
	g_strDef_Display_NameMC_PartyTarget = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_PartyTarget" );
	g_strDef_Display_NameMC_PartyGeneral = GET_LUA_DEF( const char*, "g_strDisplay_NameMC_PartyGeneral" );

	g_strDef_Display_HeadChatMC = GET_LUA_DEF( const char*, "g_strDisplay_HeadChatMC" );
	g_strDef_Display_HeadChat_tf = GET_LUA_DEF( const char*, "g_strDisplay_HeadChat_tf" );
	g_strDef_Display_HeadChat_bg = GET_LUA_DEF( const char*, "g_strDisplay_HeadChat_bg" );
	g_fDef_Display_HeadChatLifeTime = GET_LUA_DEF( float, "g_fDisplay_HeadChat_LifeTime" );

	g_strDef_Display_HpRedMC = GET_LUA_DEF( const char*, "g_strDisplay_HpRedMC" );
	g_strDef_Display_HpGreenMC = GET_LUA_DEF( const char*, "g_strDisplay_HpGreenMC" );
	g_strDef_Display_HpBlueMC = GET_LUA_DEF( const char*, "g_strDisplay_HpBlueMC" );
	g_strDef_Display_HpGreenSimpleMC = GET_LUA_DEF( const char*, "g_strDisplay_HpGreenSimpleMC" );

	g_strDef_Display_Hp_tf = GET_LUA_DEF( const char*, "g_strDisplay_Hp_tf" );
	g_strDef_Display_Hp_bar = GET_LUA_DEF( const char*, "g_strDisplay_Hp_bar" );

	g_strDef_Display_DmgMc = GET_LUA_DEF( const char*, "g_strDisplay_DmgMC" );
	g_fDef_Display_DmgLifeTime = GET_LUA_DEF( float, "g_strDisplay_DmgLifeTime" );
	g_fDef_Display_DmgRandOffsetRange = GET_LUA_DEF( float, "g_strDisplay_DmgRandOffsetRange" );
	g_fDef_Display_DmgHeadOffsetX = GET_LUA_DEF( float, "g_strDisplay_DmgHeadOffsetX" );
	g_fDef_Display_DmgHeadOffsetY = GET_LUA_DEF( float, "g_strDisplay_DmgHeadOffsetY" );

	g_strDef_Display_PrivateMarket = GET_LUA_DEF( const char*, "g_strDisplay_PrivateMarket" );
	g_strDef_Display_PrivateMarket_Button = GET_LUA_DEF( const char*, "g_strDisplay_PrivateMarket_Button" );
	g_strDef_Display_PrivateMarket_TextPM = GET_LUA_DEF( const char*, "g_strDisplay_PrivateMarket_TextPM" );
	g_strDef_Display_PrivateMarket_TextName = GET_LUA_DEF( const char*, "g_strDisplay_PrivateMarket_TextName" );
	g_strDef_Display_PrivateMarket_TextPMName = GET_LUA_DEF( const char*, "g_strDisplay_PrivateMarket_TextPMName" );

	g_nDisplayLimitCount = GET_LUA_DEF( int, "g_nDisplayLimitCount" );
}

void GLWidgetScript::LuaDoCommand( const char* strCommand )
{
	if( NULL == m_pLuaState )
		return;

	lua_tinker::dostring( m_pLuaState, strCommand );
}

//-----------------------------------------------------------------------------------------------------------------

void GLWidgetScript::InitGlobal()
{
	// 디버깅 여부
#if defined (_RELEASED) || defined (_DEBUG)
	SET_LUA_DEF( "g_bWidgetDebug", true );
#else
	SET_LUA_DEF( "g_bWidgetDebug", false );
#endif

	SET_LUA_DEF( "g_nSERVER_NORMAL", GLCONST_CHAR::wSERVER_NORMAL );
	SET_LUA_DEF( "g_nSERVER_CONGEST", GLCONST_CHAR::wSERVER_CONGEST );
	SET_LUA_DEF( "g_nSERVER_NOVACANCY", GLCONST_CHAR::wSERVER_NOVACANCY );
	SET_LUA_DEF( "g_bMONEY_DROP2FIELD", GLCONST_CHAR::bMONEY_DROP2FIELD );
	SET_LUA_DEF( "g_nInvenPageYCount", EInvenY );
	SET_LUA_DEF( "g_nINVEN_DEFAULT_LINE", GLCONST_CHAR::wInvenDefaultLine );
	SET_LUA_DEF( "g_nAUTO_REBIRTH_TIMER", GLCONST_CHAR::wAUTO_REBIRTH_TIMER );
	SET_LUA_DEF( "g_nRESURRECT_RECOMMEND_ITEM_MID", GLCONST_CHAR::nidREBIRTH_RECOMMEND_ITEM.Mid() );
	SET_LUA_DEF( "g_nRESURRECT_RECOMMEND_ITEM_SID", GLCONST_CHAR::nidREBIRTH_RECOMMEND_ITEM.Sid() );
	SET_LUA_DEF( "g_nRESURRECT_RECOMMEND_ITEM_SEARCHSTRING", GLCONST_CHAR::strREBIRTH_RECOMMEND_ITEM.c_str() );
	SET_LUA_DEF( "g_fMACRO_TERMSOFCLOSE_LIMITTIME", GLCONST_CHAR::fMacro_TermsofClose_LimitTime );
	SET_LUA_DEF( "g_nMACRO_TERMSOFCLOSE_POTION", GLCONST_CHAR::nMacro_TermsofClose_Potion );
	SET_LUA_DEF( "g_fMACRO_MAXFILLTIME", GLCONST_CHAR::fMacro_MaxFillTime );

	SET_LUA_DEF( "g_fREGIST_MAX_G", GLCONST_CHAR::fREGIST_MAX_G );
	SET_LUA_DEF( "g_fRESIST_MAX_CROW", GLCONST_CHAR::fRESIST_MAX_CROW );
	SET_LUA_DEF( "g_fRESIST_G", GLCONST_CHAR::fRESIST_G );
	SET_LUA_DEF( "g_wENABLE_NEW_CLASS", GLCONST_CHAR::wENABLE_NEW_CLASS );
	SET_LUA_DEF( "g_wLEVEL_FOR_EXTREME", GLCONST_CHAR::wLEVEL_FOR_EXTREME );
	SET_LUA_DEF( "g_wLEVEL_FOR_ACTOR", GLCONST_CHAR::wLEVEL_FOR_ACTOR );
	SET_LUA_DEF( "g_fEmergency_MobHPR", GLCONST_CHAR::fEmergency_MobHPR );
	SET_LUA_DEF( "g_fEmergency_BossHPR", GLCONST_CHAR::fEmergency_BossHPR );
	SET_LUA_DEF( "g_fEmergency_CharHPR", GLCONST_CHAR::fEmergency_CharHPR );

	if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SET_LUA_DEF( "g_strHELP_ADDRESS", RANPARAM::MultiHelpAddress[RANPARAM::emProvideLangFlag] );
	}
	else
	{
		SET_LUA_DEF( "g_strHELP_ADDRESS", RANPARAM::HelpAddress );
	}

	SET_LUA_DEF( "g_nSchoolNum", SCHOOL_NUM );
	SET_LUA_DEF( "g_nCOSTUMESTAT_MAXINVEST_NORMAL", COSS_GN_MAX );
	SET_LUA_DEF( "g_nCOSTUMESTAT_MAXINVEST_VAR", COSS_VAR_MAX );
	SET_LUA_DEF( "g_nCOSTUMESTAT_MAXINVEST_ADDED", COSS_ADDED_MAX );

	SET_LUA_DEF( "g_bIsEmulator", TRUE == m_pGaeaClient->GetGlobalStage()->IsEmulator() ? true : false );

	SET_LUA_DEF( "g_dwNameDisp_CharAlly", RANPARAM::ENameDisplay::CharAlly );
	SET_LUA_DEF( "g_dwNameDisp_CharEnemy", RANPARAM::ENameDisplay::CharEnemy );
	SET_LUA_DEF( "g_dwNameDisp_Mob", RANPARAM::ENameDisplay::Mob );
	SET_LUA_DEF( "g_dwNameDisp_NPC", RANPARAM::ENameDisplay::NPC );
	SET_LUA_DEF( "g_dwNameDisp_Summon", RANPARAM::ENameDisplay::Summon );
	SET_LUA_DEF( "g_dwNameDisp_Item", RANPARAM::ENameDisplay::Item );
}

void GLWidgetScript::RegFunc() 
{
	REG_LUA_FUNC( "RegSlot", GLWidgetScript::RegSlot );
	REG_LUA_FUNC( "RegHtmlWebView", GLWidgetScript::RegHtmlWebView );
	REG_LUA_FUNC( "MoveHtmlWebView", GLWidgetScript::MoveHtmlWebView );

	REG_LUA_FUNC( "OpenWidget", GLWidgetScript::OpenWidget );
	REG_LUA_FUNC( "CloseWidget", GLWidgetScript::CloseWidget );
	REG_LUA_FUNC( "CloseAllWidget", GLWidgetScript::CloseAllWidget );
	REG_LUA_FUNC( "IsOpenWidget", GLWidgetScript::IsOpenWidget );
	REG_LUA_FUNC( "IsLoadWidget", GLWidgetScript::IsLoadWidget );
	REG_LUA_FUNC( "GetMultiWidgetCount", GLWidgetScript::GetMultiWidgetCount );
	REG_LUA_FUNC( "EscWidgetClose", GLWidgetScript::EscWidgetClose );
	REG_LUA_FUNC( "IsOpenTooltip", GLWidgetScript::IsOpenTooltip );

	REG_LUA_FUNC( "SetStageEvent", GLWidgetScript::SetStageEvent );

	REG_LUA_FUNC( "Calculate_Plus", GLWidgetScript::Calculate_Plus );
	REG_LUA_FUNC( "Calculate_Minus", GLWidgetScript::Calculate_Minus );
	REG_LUA_FUNC( "Calculate_Multiply", GLWidgetScript::Calculate_Multiply );
	REG_LUA_FUNC( "Calculate_Divide", GLWidgetScript::Calculate_Divide );

	REG_LUA_FUNC( "SetInt", GLWidgetScript::SetInt );
	REG_LUA_FUNC( "SetUInt", GLWidgetScript::SetUInt );
	REG_LUA_FUNC( "SetDouble", GLWidgetScript::SetDouble );
	REG_LUA_FUNC( "SetBool", GLWidgetScript::SetBool );
	REG_LUA_FUNC( "SetString", GLWidgetScript::SetString );
	REG_LUA_FUNC( "SetWString", GLWidgetScript::SetWString );
	REG_LUA_FUNC( "SetArray",	GLWidgetScript::SetArray );

	REG_LUA_FUNC( "GetInt", GLWidgetScript::GetInt );
	REG_LUA_FUNC( "GetUInt", GLWidgetScript::GetUInt );
	REG_LUA_FUNC( "GetDouble", GLWidgetScript::GetDouble );
	REG_LUA_FUNC( "GetBool", GLWidgetScript::GetBool );
	REG_LUA_FUNC( "GetString", GLWidgetScript::GetString );
	REG_LUA_FUNC( "GetArray", GLWidgetScript::GetArray );

	REG_LUA_FUNC( "SetEvent", GLWidgetScript::SetEvent );

	REG_LUA_FUNC( "SetData", GLWidgetScript::SetData );
	REG_LUA_FUNC( "SetDataEx", GLWidgetScript::SetDataEx );
	REG_LUA_FUNC( "SetDataComplex", GLWidgetScript::SetDataComplex );
	REG_LUA_FUNC( "SetDataTerms", GLWidgetScript::SetDataTerms );
	REG_LUA_FUNC( "RemoveData", GLWidgetScript::RemoveData );

	REG_LUA_FUNC( "GetDataInt", GLWidgetScript::GetDataInt );
	REG_LUA_FUNC( "GetDataUInt", GLWidgetScript::GetDataUInt );
	REG_LUA_FUNC( "GetDataDouble", GLWidgetScript::GetDataDouble );
	REG_LUA_FUNC( "GetDataBool", GLWidgetScript::GetDataBool );
	REG_LUA_FUNC( "GetDataString", GLWidgetScript::GetDataString );
	REG_LUA_FUNC( "GetDataArray", GLWidgetScript::GetDataArray );
	
	REG_LUA_FUNC( "SetIndexDataInt", GLWidgetScript::SetIndexDataInt );
	REG_LUA_FUNC( "SetIndexDataUInt", GLWidgetScript::SetIndexDataUInt );
	REG_LUA_FUNC( "SetIndexDataDouble", GLWidgetScript::SetIndexDataDouble );
	REG_LUA_FUNC( "SetIndexDataBool", GLWidgetScript::SetIndexDataBool );
	REG_LUA_FUNC( "SetIndexDataString", GLWidgetScript::SetIndexDataString );

	REG_LUA_FUNC( "SetListItemInt", GLWidgetScript::SetListItemInt );
	REG_LUA_FUNC( "SetListItemUInt", GLWidgetScript::SetListItemUInt );
	REG_LUA_FUNC( "SetListItemDouble", GLWidgetScript::SetListItemDouble );
	REG_LUA_FUNC( "SetListItemBool", GLWidgetScript::SetListItemBool );
	REG_LUA_FUNC( "SetListItemString", GLWidgetScript::SetListItemString );
	REG_LUA_FUNC( "SetListItemArray", GLWidgetScript::SetListItemArray );
	REG_LUA_FUNC( "CallListItemFunc", GLWidgetScript::CallListItemFunc );

	REG_LUA_FUNC( "CallFunction", GLWidgetScript::CallFunction );
	REG_LUA_FUNC( "SetFocus", GLWidgetScript::SetFocus );
	REG_LUA_FUNC( "SetTextFormat", GLWidgetScript::SetTextFormat );
	REG_LUA_FUNC( "SetTextFormatSize", GLWidgetScript::SetTextFormatSize );
	REG_LUA_FUNC( "SetTween", GLWidgetScript::SetTween );
	REG_LUA_FUNC( "RemoveTween", GLWidgetScript::RemoveTween );
	REG_LUA_FUNC( "HitTestEnable", GLWidgetScript::HitTestEnable );
	REG_LUA_FUNC( "GetSlotTooltipPosSize", GLWidgetScript::GetSlotTooltipPosSize );
	REG_LUA_FUNC( "ResetWidgetPosition", GLWidgetScript::ResetWidgetPosition );
	REG_LUA_FUNC( "SetSpeechBubble", GLWidgetScript::SetSpeechBubble );
	REG_LUA_FUNC( "DelSpeechBubble", GLWidgetScript::DelSpeechBubble );
	REG_LUA_FUNC( "SetFakeModal", GLWidgetScript::SetFakeModal );
	REG_LUA_FUNC( "SnapCheck", GLWidgetScript::SnapCheck );
	REG_LUA_FUNC( "SetUIAlpha", GLWidgetScript::SetUIAlpha );
	REG_LUA_FUNC( "ReleaseSlot", GLWidgetScript::ReleaseSlot );
	REG_LUA_FUNC( "UpdateClubMark", GLWidgetScript::UpdateClubMark );
	REG_LUA_FUNC( "SetFoward", GLWidgetScript::SetFoward );
	REG_LUA_FUNC( "SetHeadData", GLWidgetScript::SetHeadData );
	REG_LUA_FUNC( "GetHeadData", GLWidgetScript::GetHeadData );
	REG_LUA_FUNC( "ChangeHeadData", GLWidgetScript::ChangeHeadData );
	REG_LUA_FUNC( "SetTreeData", GLWidgetScript::SetTreeData );
	REG_LUA_FUNC( "GetTreeData", GLWidgetScript::GetTreeData );
	REG_LUA_FUNC( "ChangeTreeData", GLWidgetScript::ChangeTreeData );

	// 유틸리티 ----------------------------------------------------------------------------
	REG_LUA_FUNC( "ToggleDebugScene", GLWidgetScript::ToggleDebugScene );
	REG_LUA_FUNC( "PrintDebug", GLWidgetScript::PrintDebug );
	REG_LUA_FUNC( "PrepareHTMLString", NSWIDGET_SCRIPT::PrepareHTMLString );
	REG_LUA_FUNC( "PrepareHTMLData", NSWIDGET_SCRIPT::PrepareHTMLData );
	REG_LUA_FUNC( "AnsiToUnicode", NSWIDGET_SCRIPT::AnsiToUnicode );
	REG_LUA_FUNC( "UnicodeToAnsi", NSWIDGET_SCRIPT::UnicodeToAnsi );
	REG_LUA_FUNC( "GetStringLen", NSWIDGET_SCRIPT::GetStringLength );
	REG_LUA_FUNC( "SplitString", NSWIDGET_SCRIPT::SplitString );
	REG_LUA_FUNC( "FlagAdd", NSWIDGET_SCRIPT::FlagAdd );
	REG_LUA_FUNC( "FlagSub", NSWIDGET_SCRIPT::FlagSub );
	REG_LUA_FUNC( "GetRandomColor", NSWIDGET_SCRIPT::GetRandomColor );
	REG_LUA_FUNC( "haveSymbolCha", NSWIDGET_SCRIPT::haveSymbolCha );
	REG_LUA_FUNC( "StringFindFirst", NSWIDGET_SCRIPT::StringFindFirst );
		
	REG_LUA_FUNC( "GenerateItemInfoHeader", NSWIDGET_SCRIPT::GenerateItemInfoHeader );
	REG_LUA_FUNC( "GenerateItemInfo", NSWIDGET_SCRIPT::GenerateItemInfo );
	REG_LUA_FUNC( "GenerateSkillInfoHeader", NSWIDGET_SCRIPT::GenerateSkillInfoHeader );
	REG_LUA_FUNC( "GenerateSkillInfo", NSWIDGET_SCRIPT::GenerateSkillInfo );
	REG_LUA_FUNC( "CurrencyFormat", GLWidgetScript::MakeMoney );
	REG_LUA_FUNC( "ColorDWORDtoWORD", GLWidgetScript::ColorDWORDtoWORD );
	REG_LUA_FUNC( "ColorWORDtoDWORD", GLWidgetScript::ColorWORDtoDWORD );
	REG_LUA_FUNC( "OpenForum", GLWidgetScript::OpenForum );

	// 클라이언트 --------------------------------------------------------------------------
	REG_LUA_FUNC( "GetScreenWidth", GLWidgetScript::GetScreenSizeWidth );
	REG_LUA_FUNC( "GetScreenHeight", GLWidgetScript::GetScreenSizeHeight );
	REG_LUA_FUNC( "GetMousePos", GLWidgetScript::GetMousePos );
	REG_LUA_FUNC( "IsFullScreen", GLWidgetScript::IsFullScreen );
	REG_LUA_FUNC( "TransInputEn", GLWidgetScript::TransInputEn );
	REG_LUA_FUNC( "GetGameExternalText", GLWidgetScript::GetGameExternalText );
	REG_LUA_FUNC( "GetGameInternalText", GLWidgetScript::GetGameInternalText );
	REG_LUA_FUNC( "GetGameWordText", GLWidgetScript::GetGameWordText );
	REG_LUA_FUNC( "GetTipText_RandomIntex", GLWidgetScript::GetTipText_RandomIntex );
	REG_LUA_FUNC( "AnsiToUTF8", GLWidgetScript::AnsiToUTF8 );
	REG_LUA_FUNC( "UTF8ToAnsi", GLWidgetScript::UTF8ToAnsi );
	REG_LUA_FUNC( "GetCharClassName", GLWidgetScript::GetCharClassName );
	REG_LUA_FUNC( "GetCharClassIndexName", GLWidgetScript::GetCharClassIndexName );
	REG_LUA_FUNC( "GetCharClassToClassIndex", GLWidgetScript::GetCharClassToClassIndex );
	REG_LUA_FUNC( "GetMapName", GLWidgetScript::GetMapName );
	REG_LUA_FUNC( "GetItemName", GLWidgetScript::GetItemName );
	REG_LUA_FUNC( "GetSkillName", GLWidgetScript::GetSkillName );
	REG_LUA_FUNC( "CloseGame", GLWidgetScript::CloseGame );
	REG_LUA_FUNC( "CheckString", GLWidgetScript::CheckString );
	REG_LUA_FUNC( "GetServiceProvider", GLWidgetScript::GetServiceProvider );
	REG_LUA_FUNC( "GetRenderServiceProvider", GLWidgetScript::GetRenderServiceProvider );
	REG_LUA_FUNC( "SetStageBG", GLWidgetScript::SetStageBG );
	REG_LUA_FUNC( "GetStageBG", GLWidgetScript::GetStageBG );
	REG_LUA_FUNC( "GetUITexturePath", GLWidgetScript::GetUITexturePath );
	REG_LUA_FUNC( "GetMinimapTexturePath", GLWidgetScript::GetMinimapTexturePath );
	REG_LUA_FUNC( "GetResolutionList", GLWidgetScript::GetResolutionList );
	REG_LUA_FUNC( "SetSfxVolume", GLWidgetScript::SetSfxVolume );
	REG_LUA_FUNC( "SetEnvVolume", GLWidgetScript::SetEnvVolume );
	REG_LUA_FUNC( "SetBGMVolume", GLWidgetScript::SetBGMVolume );
	REG_LUA_FUNC( "GetSfxVolume", GLWidgetScript::GetSfxVolume );
	REG_LUA_FUNC( "GetEnvVolume", GLWidgetScript::GetEnvVolume );
	REG_LUA_FUNC( "GetBGMVolume", GLWidgetScript::GetBGMVolume );
	REG_LUA_FUNC( "SetMuteSfx", GLWidgetScript::SetMuteSfx );
	REG_LUA_FUNC( "SetMuteEnv", GLWidgetScript::SetMuteEnv );
	REG_LUA_FUNC( "SetMuteBGM", GLWidgetScript::SetMuteBGM );
	REG_LUA_FUNC( "GetMuteSfx", GLWidgetScript::GetMuteSfx );
	REG_LUA_FUNC( "GetMuteEnv", GLWidgetScript::GetMuteEnv );
	REG_LUA_FUNC( "GetMuteBGM", GLWidgetScript::GetMuteBGM );
	REG_LUA_FUNC( "SetGameOption", GLWidgetScript::SetGameOption );
	REG_LUA_FUNC( "GetGameOption", GLWidgetScript::GetGameOption );
	REG_LUA_FUNC( "GetCharClassFlagIndex", GLWidgetScript::GetCharClassFlagIndex );
	REG_LUA_FUNC( "GetCharClassToSexFlag", GLWidgetScript::GetCharClassToSexFlag );
	REG_LUA_FUNC( "GetPetType", GLWidgetScript::GetPetType );
	REG_LUA_FUNC( "GetPetName", GLWidgetScript::GetPetName );
	REG_LUA_FUNC( "GetDate", GLWidgetScript::GetDate );
	REG_LUA_FUNC( "GameToLobbyStage", GLWidgetScript::GameToLobbyStage );
	REG_LUA_FUNC( "ToCustomerCenter", GLWidgetScript::ToCustomerCenter );
	REG_LUA_FUNC( "OpenWebPage", GLWidgetScript::OpenWebPage );
	REG_LUA_FUNC( "ChangeScreenMode", GLWidgetScript::ChangeScreenMode );
	REG_LUA_FUNC( "GetCameraPos", GLWidgetScript::GetCameraPos );
	REG_LUA_FUNC( "GetCameraDir", GLWidgetScript::GetCameraDir );
	REG_LUA_FUNC( "GetAutoResurrection", GLWidgetScript::GetAutoResurrection );
	REG_LUA_FUNC( "SetAutoResurrection", GLWidgetScript::SetAutoResurrection );
	REG_LUA_FUNC( "StopSound", GLWidgetScript::StopSound );
	REG_LUA_FUNC( "PlaySound", GLWidgetScript::PlaySound );
	REG_LUA_FUNC( "MakeNativeID", GLWidgetScript::MakeNativeID );
	REG_LUA_FUNC( "SplitNativeID", GLWidgetScript::SplitNativeID );
	REG_LUA_FUNC( "GetCompositionString", GLWidgetScript::GetCompositionString );
	REG_LUA_FUNC( "GetItem_BoxItemlist", GLWidgetScript::GetItem_BoxItemlist);
	REG_LUA_FUNC( "GetMoneyColor", GLWidgetScript::GetMoneyColor );


	// 커맨드 ----------------------------------------------------------------------------------
	REG_LUA_FUNC( "DoCommand", GLWidgetScript::DoCommand );

	// 아이콘 슬롯 -----------------------------------------------------------------------------
	REG_LUA_FUNC( "ResetSlot", GLWidgetScript::ResetSlot );
	REG_LUA_FUNC( "SetSlot_Item", GLWidgetScript::SetSlot_Item );
	REG_LUA_FUNC( "SetSlot_Item_NonStrengthen", GLWidgetScript::SetSlot_Item_NonStrengthen );
	REG_LUA_FUNC( "SetSlot_ItemBornT", GLWidgetScript::SetSlot_ItemBornT );
	REG_LUA_FUNC( "SetSlot_Item_DWORD", GLWidgetScript::SetSlot_Item_DWORD );
	REG_LUA_FUNC( "SetSlot_Skill", GLWidgetScript::SetSlot_Skill );
	REG_LUA_FUNC( "SetSlot_SkillLevel", GLWidgetScript::SetSlot_SkillLevel );
	REG_LUA_FUNC( "SetSlot_Inventory", GLWidgetScript::SetSlot_Inventory );
	REG_LUA_FUNC( "SetSlot_Rebuy", GLWidgetScript::SetSlot_Rebuy );
	REG_LUA_FUNC( "SetSlot_SummonState", GLWidgetScript::SetSlot_SummonState );
	REG_LUA_FUNC( "SetSlot_MiniGame_Dice", GLWidgetScript::SetSlot_MiniGame_Dice)
	REG_LUA_FUNC( "SetSlot_Bank", GLWidgetScript::SetSlot_Bank );
	REG_LUA_FUNC( "SetSlot_Wishlist", GLWidgetScript::SetSlot_Wishlist );
	REG_LUA_FUNC( "SetSlot_CharInfo", GLWidgetScript::SetSlot_CharInfo );
	REG_LUA_FUNC( "SetSlot_SQS", GLWidgetScript::SetSlot_SQS );
	REG_LUA_FUNC( "SetSlot_IQS", GLWidgetScript::SetSlot_IQS );
	REG_LUA_FUNC( "SetSlot_NpcMarket", GLWidgetScript::SetSlot_NpcMarket );	
	REG_LUA_FUNC( "SetSlot_NpcMarketCart", GLWidgetScript::SetSlot_NpcMarketCart );	
	REG_LUA_FUNC( "SetSlot_Trade", GLWidgetScript::SetSlot_Trade );
	REG_LUA_FUNC( "SetSlot_PointShop", GLWidgetScript::SetSlot_PointShop );	
	REG_LUA_FUNC( "SetSlot_PointShopCart", GLWidgetScript::SetSlot_PointShopCart );	
	REG_LUA_FUNC( "SetSlot_ReferChar", GLWidgetScript::SetSlot_ReferChar );
	REG_LUA_FUNC( "SetSlot_PMSPointTrade", GLWidgetScript::SetSlot_PMSPointTrade );
	REG_LUA_FUNC( "SetSlot_PMSPointTrade_Index", GLWidgetScript::SetSlot_PMSPointTrade_Index );
	REG_LUA_FUNC( "SetSlot_PrivateMarket", GLWidgetScript::SetSlot_PrivateMarket );
	REG_LUA_FUNC( "SetSlot_PrivateMarketSearch", GLWidgetScript::SetSlot_PrivateMarketSearch );
	REG_LUA_FUNC( "SetSlot_ReceivePost", GLWidgetScript::SetSlot_ReceivePost );
	REG_LUA_FUNC( "SetSlot_PostBoxSub_PreviewPageOpen", GLWidgetScript::SetSlot_PostBoxSub_PreviewPageOpen );
	REG_LUA_FUNC( "SetSlot_Pet", GLWidgetScript::SetSlot_Pet );	
	REG_LUA_FUNC( "SetSlot_ClubStorage", GLWidgetScript::SetSlot_ClubStorage );	
	REG_LUA_FUNC( "SetClubStorage_Item", GLWidgetScript::SetClubStorage_Item );
	REG_LUA_FUNC( "SetSlot_Locker", GLWidgetScript::SetSlot_Locker );
	REG_LUA_FUNC( "SetSlot_RebuildCard", GLWidgetScript::SetSlot_RebuildCard );
	REG_LUA_FUNC( "SetSlot_PartyDistribution", GLWidgetScript::SetSlot_PartyDistribution );
	REG_LUA_FUNC( "SetSlot_ItemRepair", GLWidgetScript::SetSlot_ItemRepair );
	REG_LUA_FUNC( "SetSlot_LottoWinItem", GLWidgetScript::SetSlot_LottoWinItem );
	REG_LUA_FUNC( "SetSlot_PreWinListItem", GLWidgetScript::SetSlot_PreWinListItem );
	REG_LUA_FUNC( "SetSlot_RanMobileReward", GLWidgetScript::SetSlot_RanMobileReward );
	
	REG_LUA_FUNC( "RegShowTooltipEnableState", GLWidgetScript::RegShowTooltipEnableState );
	REG_LUA_FUNC( "UnregShowTooltipEnableState", GLWidgetScript::UnregShowTooltipEnableState );
	
	// 채팅 --------------------------------------------------------------------------------------
	REG_LUA_FUNC( "AddChatMessage", GLWidgetScript::AddChatMessage );
	REG_LUA_FUNC( "AddLinkMessage", GLWidgetScript::AddLinkMessage );
	REG_LUA_FUNC( "AddStoredChat", GLWidgetScript::AddStoredChat );
	REG_LUA_FUNC( "AddStoredChatType", GLWidgetScript::AddStoredChatType );
	REG_LUA_FUNC( "ClearStoredChat", GLWidgetScript::ClearStoredChat );
	REG_LUA_FUNC( "AddHeadChat", GLWidgetScript::AddHeadChat );

	// 렌더 텍스처 ------------------------------------------------------------------------------
	REG_LUA_FUNC( "UpdateDiceRT", GLWidgetScript::UpdateDiceRT );
	REG_LUA_FUNC( "UpdateItemPreviewRT", GLWidgetScript::UpdateItemPreviewRT );

	// 데이터 관련 ------------------------------------------------------------------------------
	REG_LUA_FUNC( "GetPetStyleSize", GLCONST_PET::GetPetStyleSize );
	REG_LUA_FUNC( "GetPetStyleNum", GLCONST_PET::GetPetStyleNum );

	// CapsLock ----------------------------------------------------------------------------------
	REG_LUA_FUNC( "GetCapslockKeyState", GLWidgetScript::GetCapslockKeyState );
	REG_LUA_FUNC( "SetCapslockKeyState", GLWidgetScript::SetCapslockKeyState );

	// 서버 메시지 -------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "WidgetMsgInterface", GLWidgetMsgInterface );
	{
		typedef GLWidgetMsgInterface WMI;

		// 캐릭터 포스트아이템 초기화
		REG_LUA_CLASS_FUNC( WMI, "ResetCharPostItemData", &WMI::ResetCharPostItemData );
		
		REG_LUA_CLASS_FUNC( WMI, "ConnectLoginServer", &WMI::ConnectLoginServer );
		REG_LUA_CLASS_FUNC( WMI, "ConnectServerChannel", &WMI::ConnectServerChannel );
		REG_LUA_CLASS_FUNC( WMI, "Login", &WMI::Login );
		REG_LUA_CLASS_FUNC( WMI, "JoinGame", &WMI::JoinGame );
		REG_LUA_CLASS_FUNC( WMI, "JoinGame2ndPassword", &WMI::JoinGame2ndPassword );
		REG_LUA_CLASS_FUNC( WMI, "CreateChar", &WMI::CreateChar );
		REG_LUA_CLASS_FUNC( WMI, "ApplyStatPoint", &WMI::ApplyStatPoint );
		REG_LUA_CLASS_FUNC( WMI, "ChangeTitle", &WMI::ReqChangeTitle );
		REG_LUA_CLASS_FUNC( WMI, "InvenItemSort", &WMI::ReqInvenItemSort );
		REG_LUA_CLASS_FUNC( WMI, "TrashItem", &WMI::ReqGarbageResult );
		REG_LUA_CLASS_FUNC( WMI, "MoneyToField", &WMI::ReqMoneyToField );
		REG_LUA_CLASS_FUNC( WMI, "HoldItemToField", &WMI::ReqHoldToFieldForWidget );
		REG_LUA_CLASS_FUNC( WMI, "SendChatMsg", &WMI::SendChatMsg );
		REG_LUA_CLASS_FUNC( WMI, "SkillLevelUp", &WMI::SkillLevelUp );
		REG_LUA_CLASS_FUNC( WMI, "RequestConflict", &WMI::RequestConflict );
		REG_LUA_CLASS_FUNC( WMI, "AnswerConflict", &WMI::AnswerConflict );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenHairStyleChange", &WMI::ReqInvenHairStyleChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenFaceStyleChange", &WMI::ReqInvenFaceStyleChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenGenderChange", &WMI::ReqInvenGenderChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqBikePutOnWith", &WMI::ReqBikePutOnWith );
		REG_LUA_CLASS_FUNC( WMI, "MoveToMap", &WMI::MoveToMap );
		REG_LUA_CLASS_FUNC( WMI, "ReqToggleRun", &WMI::ReqToggleRun );
		REG_LUA_CLASS_FUNC( WMI, "CommitVehicleColor", &WMI::CommitVehicleColor );
		REG_LUA_CLASS_FUNC( WMI, "CommitCostumeColor", &WMI::CommitCostumeColor );
		REG_LUA_CLASS_FUNC( WMI, "RequestResetSkillStats", &WMI::RequestResetSkillStats );
		REG_LUA_CLASS_FUNC( WMI, "TryRequestBankItemInfo", &WMI::TryRequestBankItemInfo );
		REG_LUA_CLASS_FUNC( WMI, "ReqChargedItemTo", &WMI::ReqChargedItemTo );
		REG_LUA_CLASS_FUNC( WMI, "ResurrectCheckPoint", &WMI::ResurrectCheckPoint );
		REG_LUA_CLASS_FUNC( WMI, "ResurrectExp", &WMI::ResurrectExp );
		REG_LUA_CLASS_FUNC( WMI, "ResurrectCurPos", &WMI::ResurrectCurPos );
		REG_LUA_CLASS_FUNC( WMI, "ResurrectSkill", &WMI::ResurrectSkill );
		REG_LUA_CLASS_FUNC( WMI, "ReqPartyLinkJoin", &WMI::ReqPartyLinkJoin );
		REG_LUA_CLASS_FUNC( WMI, "ReqQuestComplete", &WMI::ReqQuestComplete );
		REG_LUA_CLASS_FUNC( WMI, "ReqQuestGiveUp", &WMI::ReqQuestGiveUp );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenLunchBox", &WMI::ReqInvenLunchBox );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenRename", &WMI::ReqInvenRename );
		REG_LUA_CLASS_FUNC( WMI, "ReqPMarketClose", &WMI::ReqPMarketClose );
		REG_LUA_CLASS_FUNC( WMI, "ReqStorageSaveMoney", &WMI::ReqStorageSaveMoneyForWidget );
		REG_LUA_CLASS_FUNC( WMI, "ReqStorageDrawMoney", &WMI::ReqStorageDrawMoneyForWidget );
		REG_LUA_CLASS_FUNC( WMI, "ReqStorageOpenCard", &WMI::ReqStorageOpenCard );
		REG_LUA_CLASS_FUNC( WMI, "ReqGetStorage", &WMI::ReqGetStorageForWidget );
		REG_LUA_CLASS_FUNC( WMI, "ReqStorageCard", &WMI::ReqStorageCard );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenDrug", &WMI::ReqInvenDrug );
		REG_LUA_CLASS_FUNC( WMI, "ReqStorageCloseCard", &WMI::ReqStorageCloseCard );
		REG_LUA_CLASS_FUNC( WMI, "ReqGateOut", &WMI::ReqGateOut );
		REG_LUA_CLASS_FUNC( WMI, "ReqGuidCommission", &WMI::ReqGuidCommissionForWidgt );
		REG_LUA_CLASS_FUNC( WMI, "ReqRecoveryExpNpc", &WMI::ReqRecoveryExpNpc );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubNew", &WMI::ReqClubNewForWidget );
		REG_LUA_CLASS_FUNC( WMI, "ReqSlotChange", &WMI::ReqSlotChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqStorageOpenFromInven", &WMI::ReqStorageOpenFromInven );
		REG_LUA_CLASS_FUNC( WMI, "ReqSkillHoldReset", &WMI::ReqSkillHoldResetForWidget );
		REG_LUA_CLASS_FUNC( WMI, "ReqSetVehicle", &WMI::ReqSetVehicle );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenSplit", &WMI::ReqInvenSplit );
		REG_LUA_CLASS_FUNC( WMI, "ReqPartyLinkLure", &WMI::ReqPartyLinkLure );
		REG_LUA_CLASS_FUNC( WMI, "ReqSkillRunSet", &WMI::ReqSkillRunSet );
		REG_LUA_CLASS_FUNC( WMI, "SendCustomMessage", &WMI::SendCustomMessage );
		REG_LUA_CLASS_FUNC( WMI, "ReqBeginMinigame", &WMI::ReqBeginMinigame );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectOddEvenMinigame", &WMI::ReqSelectOddEvenMinigame );
		REG_LUA_CLASS_FUNC( WMI, "ReqRollMinigame", &WMI::ReqRollMinigame );
		REG_LUA_CLASS_FUNC( WMI, "ReqResultMinigame", &WMI::ReqResultMinigame );
		REG_LUA_CLASS_FUNC( WMI, "ReqRewardMinigame", &WMI::ReqRewardMinigame );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectRewardMinigame", &WMI::ReqSelectRewardMinigame );
		REG_LUA_CLASS_FUNC( WMI, "ReqReceiveRewardMinigame", &WMI::ReqReceiveRewardMinigame );		
		REG_LUA_CLASS_FUNC( WMI, "ReqCloseMinigame", &WMI::ReqCloseMinigame );
		REG_LUA_CLASS_FUNC( WMI, "InstanceManRequestClose", &WMI::InstanceManRequestClose );
		REG_LUA_CLASS_FUNC( WMI, "SecedeParty", &WMI::SecedeParty );
		REG_LUA_CLASS_FUNC( WMI, "DissolveParty", &WMI::DissolveParty );
		REG_LUA_CLASS_FUNC( WMI, "AuthorityParty", &WMI::AuthorityParty );
		REG_LUA_CLASS_FUNC( WMI, "ChangeOptionParty", &WMI::ChangeOptionParty );
		REG_LUA_CLASS_FUNC( WMI, "ReqInviteParty", &WMI::ReqInviteParty );
		REG_LUA_CLASS_FUNC( WMI, "LureParty", &WMI::LureParty );
		REG_LUA_CLASS_FUNC( WMI, "LureNameParty", &WMI::LureNameParty );
		REG_LUA_CLASS_FUNC( WMI, "ReqPartyInviteAns", &WMI::ReqPartyInviteAns );
		REG_LUA_CLASS_FUNC( WMI, "SelectNpcTalk", &WMI::SelectNpcTalk );
		REG_LUA_CLASS_FUNC( WMI, "BuyItemNpcTalk", &WMI::BuyItemNpcTalk );
		REG_LUA_CLASS_FUNC( WMI, "SellItemNpcTalk", &WMI::SellItemNpcTalk );
		REG_LUA_CLASS_FUNC( WMI, "BuyCartItemNpcTalk", &WMI::BuyCartItemNpcTalk );
		REG_LUA_CLASS_FUNC( WMI, "RebuyNpcTalk", &WMI::RebuyNpcTalk );
		REG_LUA_CLASS_FUNC( WMI, "BuyCartListPointShop", &WMI::BuyCartListPointShop );
		REG_LUA_CLASS_FUNC( WMI, "ReqProductTypeAdd", &WMI::ReqProductTypeAdd );
		REG_LUA_CLASS_FUNC( WMI, "ReqProductTypeDelete", &WMI::ReqProductTypeDelete );
		REG_LUA_CLASS_FUNC( WMI, "ReqProduct", &WMI::ReqProduct );
		REG_LUA_CLASS_FUNC( WMI, "ReqProductCancel", &WMI::ReqProductCancel );
		REG_LUA_CLASS_FUNC( WMI, "ReqProductTypeList", &WMI::ReqProductTypeList );
		REG_LUA_CLASS_FUNC( WMI, "SetActivatedSkillPet", &WMI::SetActivatedSkillPet );
		REG_LUA_CLASS_FUNC( WMI, "ReqChangeStylePet", &WMI::ReqChangeStylePet );
		REG_LUA_CLASS_FUNC( WMI, "CommitColorPet", &WMI::CommitColorPet );
		REG_LUA_CLASS_FUNC( WMI, "ReqRenamePet", &WMI::ReqRenamePet );
		REG_LUA_CLASS_FUNC( WMI, "ReqTradeOpen", &WMI::ReqTradeOpen );
		REG_LUA_CLASS_FUNC( WMI, "ReqTradeMoney", &WMI::ReqTradeMoney );
		REG_LUA_CLASS_FUNC( WMI, "ReqTradeLock", &WMI::ReqTradeLock );
		REG_LUA_CLASS_FUNC( WMI, "ReqTradeAgree", &WMI::ReqTradeAgree );
		REG_LUA_CLASS_FUNC( WMI, "ReqTradeCancel", &WMI::ReqTradeCancel );
		REG_LUA_CLASS_FUNC( WMI, "ReqTradeAns", &WMI::ReqTradeAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendAdd", &WMI::ReqFriendAdd );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendGroupAdd", &WMI::ReqFriendGroupAdd );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendGroupRename", &WMI::ReqFriendGroupRename );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendGroupDelete", &WMI::ReqFriendGroupDelete );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendMoveGroup", &WMI::ReqFriendMoveGroup );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendBlock", &WMI::ReqFriendBlock );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendDel", &WMI::ReqFriendDel );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendInviteParty", &WMI::ReqFriendInviteParty );
		REG_LUA_CLASS_FUNC( WMI, "ReqMoveToFriend", &WMI::ReqMoveToFriend );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendLureAns", &WMI::ReqFriendLureAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqFriendLureAns_throughAlarm", &WMI::SfReqFriendLureAns_throughAlarm)
		REG_LUA_CLASS_FUNC( WMI, "ReqBuySearchItemPrivateMarket", &WMI::ReqBuySearchItemPrivateMarket );
		REG_LUA_CLASS_FUNC( WMI, "SearchItemPrivateMarket", &WMI::SearchItemPrivateMarket );
		REG_LUA_CLASS_FUNC( WMI, "RefreshLogPrivateMarket", &WMI::RefreshLogPrivateMarket );
		REG_LUA_CLASS_FUNC( WMI, "ReqPointTradeListPrivateMarket", &WMI::ReqPointTradeListPrivateMarket );
		REG_LUA_CLASS_FUNC( WMI, "ReqRegistItemPrivateMarket", &WMI::ReqRegistItemPrivateMarket );
		REG_LUA_CLASS_FUNC( WMI, "ReqUnregistItemPrivateMarket", &WMI::ReqUnregistItemPrivateMarket );
		REG_LUA_CLASS_FUNC( WMI, "ReqPrivateMarketRegItemOpen", &WMI::ReqPrivateMarketRegItemOpen );
		REG_LUA_CLASS_FUNC( WMI, "ReqPrivateMarketRegItem", &WMI::ReqPrivateMarketRegItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqPrivateMarketOpen", &WMI::ReqPrivateMarketOpen );
		REG_LUA_CLASS_FUNC( WMI, "ReqPrivateMarketClose", &WMI::ReqPrivateMarketClose );
		REG_LUA_CLASS_FUNC( WMI, "ReqPMarketBuyItem", &WMI::ReqPMarketBuyItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqGroupChat", &WMI::ReqGroupChat );
		REG_LUA_CLASS_FUNC( WMI, "ReqGroupChatFB", &WMI::ReqGroupChatFB );
		REG_LUA_CLASS_FUNC( WMI, "ReqQuitGroupChat", &WMI::ReqQuitGroupChat );
		REG_LUA_CLASS_FUNC( WMI, "SendChatGroupChat", &WMI::SendChatGroupChat );
		REG_LUA_CLASS_FUNC( WMI, "ReqKickGroupChat", &WMI::ReqKickGroupChat );
		REG_LUA_CLASS_FUNC( WMI, "ReqSearchCharacter", &WMI::ReqSearchCharacter );
		REG_LUA_CLASS_FUNC( WMI, "ReqSearchCharacterDetailInfo", &WMI::ReqSearchCharacterDetailInfo );
		REG_LUA_CLASS_FUNC( WMI, "ReqSearchCharacterPosition", &WMI::ReqSearchCharacterPosition );
		REG_LUA_CLASS_FUNC( WMI, "ReqSetClubMemberGrade", &WMI::ReqSetClubMemberGrade );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubNoticeChange", &WMI::ReqClubNoticeChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqGetClubStorage", &WMI::ReqGetClubStorage );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubStorageDrawMoney", &WMI::ReqClubStorageDrawMoney );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubStorageSaveMoney", &WMI::ReqClubStorageSaveMoney );
		REG_LUA_CLASS_FUNC( WMI, "ClubIncomeMoneyToStorageMoney", &WMI::ClubIncomeMoneyToStorageMoney );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubNickChange", &WMI::ReqClubNick );
		REG_LUA_CLASS_FUNC( WMI, "ReqChangeGradeName", &WMI::ReqChangeGradeName );
		REG_LUA_CLASS_FUNC( WMI, "ReqAuthorityChange", &WMI::ReqAuthorityChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubDissolution", &WMI::ReqClubDissolution );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubDissolutionCancel", &WMI::ReqClubDissolutionCancel );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubMarkChange", &WMI::ReqClubMarkChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubSecede", &WMI::ReqClubSecede );
		REG_LUA_CLASS_FUNC( WMI, "ClubAutoKickReq", &WMI::ClubAutoKickReq );
		REG_LUA_CLASS_FUNC( WMI, "DelClubMember", &WMI::DelClubMember );
		REG_LUA_CLASS_FUNC( WMI, "ClubMasterChange", &WMI::ClubMasterChange );
		REG_LUA_CLASS_FUNC( WMI, "ClubCDAuth", &WMI::ClubCDAuth );
		REG_LUA_CLASS_FUNC( WMI, "RequestClubJoin", &WMI::RequestClubJoin );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubJoinAns", &WMI::ReqClubJoinAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqPVERetry", &WMI::ReqPVERetry );
		REG_LUA_CLASS_FUNC( WMI, "ReqNewInstanceRetry", &WMI::ReqNewInstanceRetry );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubAuthorityAns", &WMI::ReqClubAuthorityAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubAllianceAns", &WMI::ReqClubAllianceAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqAllianceBattleArmistice", &WMI::ReqAllianceBattleArmistice );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubBattleArmistice", &WMI::ReqClubBattleArmistice );
		REG_LUA_CLASS_FUNC( WMI, "ReqAllianceBattleAns", &WMI::ReqAllianceBattleAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubBattleAns", &WMI::ReqClubBattleAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubAlliance", &WMI::ReqClubAlliance_PToPTarget );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubAllianceDel",	&WMI::ReqClubAllianceDel_Targetname );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubBattleArmistice_TargetName", &WMI::ReqClubBattleArmistice_TargetName );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubBattleSubmission", &WMI::ReqClubBattleSubmission_TargetName );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubJoin", &WMI::ReqClubJoin );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubJoinByCharname", &WMI::ReqClubJoinByCharname );
		REG_LUA_CLASS_FUNC( WMI, "ReqReferChar", &WMI::ReqReferChar );
		REG_LUA_CLASS_FUNC( WMI, "ReqReceivePageRefreshPost", &WMI::ReqReceivePageRefreshPost );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectPostOpen", &WMI::ReqSelectPostOpen );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectPostDelete", &WMI::ReqSelectPostDelete );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectPostTake", &WMI::ReqSelectPostTake );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectPostReturn", &WMI::ReqSelectPostReturn );
		REG_LUA_CLASS_FUNC( WMI, "ReqSelectPostTakeItem", &WMI::ReqSelectPostTakeItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqSendPostTable", &WMI::ReqSendPostTable );
		REG_LUA_CLASS_FUNC( WMI, "ReqReceiveHistoryPost", &WMI::ReqReceiveHistoryPost );
		REG_LUA_CLASS_FUNC( WMI, "ReqReceiveSendHistoryPost", &WMI::ReqReceiveSendHistoryPost );
		REG_LUA_CLASS_FUNC( WMI, "ReqReceiveRecvHistoryPost", &WMI::ReqReceiveRecvHistoryPost );
		REG_LUA_CLASS_FUNC( WMI, "ReqUseInvenPostBoxCardPost", &WMI::ReqUseInvenPostBoxCardPost );
		REG_LUA_CLASS_FUNC( WMI, "ReqCDMRankInfo", &WMI::ReqCDMRankInfo );
		REG_LUA_CLASS_FUNC( WMI, "ReqCTFJoin", &WMI::ReqCTFJoin );
		REG_LUA_CLASS_FUNC( WMI, "ReqCancelCompetition", &WMI::ReqCancelCompetition );
		REG_LUA_CLASS_FUNC( WMI, "ReqExitCompetition", &WMI::ReqExitCompetition );
		REG_LUA_CLASS_FUNC( WMI, "OnEnteranceCompetition", &WMI::OnEnteranceCompetition );
		REG_LUA_CLASS_FUNC( WMI, "OnEnteranceConfirmCompetition", &WMI::OnEnteranceConfirmCompetition );
		REG_LUA_CLASS_FUNC( WMI, "OnClearOfBelongingCompetition", &WMI::OnClearOfBelongingCompetition );
		REG_LUA_CLASS_FUNC( WMI, "OnEnteranceTournament", &WMI::OnEnteranceTournament );
		REG_LUA_CLASS_FUNC( WMI, "ReqGetBettingInfoCompetition", &WMI::ReqGetBettingInfoCompetition );
		REG_LUA_CLASS_FUNC( WMI, "ReqSetTournamentBettingMoney", &WMI::ReqSetTournamentBettingMoney );
		REG_LUA_CLASS_FUNC( WMI, "ReqCancelTournamentBettingMoney", &WMI::ReqCancelTournamentBettingMoney );
		REG_LUA_CLASS_FUNC( WMI, "ReqObserveTournament", &WMI::ReqObserveTournament );
		REG_LUA_CLASS_FUNC( WMI, "ReqReferCharCompetition", &WMI::ReqReferCharCompetition );
		REG_LUA_CLASS_FUNC( WMI, "ReqAttendanceTakeReward", &WMI::ReqAttendanceTakeReward );
		REG_LUA_CLASS_FUNC( WMI, "ReqRebuild_SelectCache", &WMI::ReqRebuild_SelectCache );
		REG_LUA_CLASS_FUNC( WMI, "ReqChangeRebuildType", &WMI::ReqChangeRebuildType );
		REG_LUA_CLASS_FUNC( WMI, "ReqRebuildItem_Fix", &WMI::ReqRebuildItem_Fix );
		REG_LUA_CLASS_FUNC( WMI, "ReqRebuildItem", &WMI::ReqRebuildItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqRebuildItem_Cancel", &WMI::ReqRebuildItem_Cancel );
		REG_LUA_CLASS_FUNC( WMI, "ReqRebuildItem_DirectFix", &WMI::ReqRebuildItem_DirectFix );
		REG_LUA_CLASS_FUNC( WMI, "ReqRebuildCloseWindow", &WMI::ReqRebuildCloseWindow );
		REG_LUA_CLASS_FUNC( WMI, "ReqPreWinNumLotto", &WMI::ReqPreWinNumLotto );
		REG_LUA_CLASS_FUNC( WMI, "ReqPreWinManListLotto", &WMI::ReqPreWinManListLotto );
		REG_LUA_CLASS_FUNC( WMI, "ReqPreAccumulateMoneyLotto", &WMI::ReqPreAccumulateMoneyLotto );
		REG_LUA_CLASS_FUNC( WMI, "ReqBuyLotto", &WMI::ReqBuyLotto );
		REG_LUA_CLASS_FUNC( WMI, "ReqBuyLottoList", &WMI::ReqBuyLottoList );
		REG_LUA_CLASS_FUNC( WMI, "ReqPreBuyListLotto", &WMI::ReqPreBuyListLotto );
		REG_LUA_CLASS_FUNC( WMI, "ReqConfirmBuyListLotto", &WMI::ReqConfirmBuyListLotto );
		REG_LUA_CLASS_FUNC( WMI, "GetLottoSystemState", &WMI::GetLottoSystemState );
		REG_LUA_CLASS_FUNC( WMI, "ReqTaxiStation", &WMI::ReqTaxiStation );
		REG_LUA_CLASS_FUNC( WMI, "ReqCTFieldQuit", &WMI::ReqCTFieldQuit );
		REG_LUA_CLASS_FUNC( WMI, "ReqKeyFrameInfoFlyCamCtrl", &WMI::ReqKeyFrameInfoFlyCamCtrl );
		REG_LUA_CLASS_FUNC( WMI, "ReqBusStation", &WMI::ReqBusStation );
		REG_LUA_CLASS_FUNC( WMI, "ReqPVEExit", &WMI::ReqPVEExit );
		REG_LUA_CLASS_FUNC( WMI, "ReqChangeCountry", &WMI::ReqChangeCountry );
		REG_LUA_CLASS_FUNC( WMI, "ReqJoiinWorldBattleServer", &WMI::ReqJoiinWorldBattleServer );
		REG_LUA_CLASS_FUNC( WMI, "ReqResetCountry", &WMI::ReqResetCountry );
		REG_LUA_CLASS_FUNC( WMI, "ReqUseExpCapsuleItem", &WMI::ReqUseExpCapsuleItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqUseExpCompressorItem", &WMI::ReqUseExpCompressorItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqUseUnlockCharacterSlotItem", &WMI::ReqUseUnlockCharacterSlotItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqCostumeStatCToS", &WMI::ReqCostumeStatCToS );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenItemDecompose", &WMI::ReqInvenItemDecompose );
		REG_LUA_CLASS_FUNC( WMI, "ReqTLGrinding", &WMI::ReqTLGrinding );
		REG_LUA_CLASS_FUNC( WMI, "ReqItemRepair", &WMI::ReqItemRepair );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenHairColorChange", &WMI::ReqInvenHairColorChange );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubBattle", &WMI::ReqClubBattle );
		REG_LUA_CLASS_FUNC( WMI, "ReqAllianceBattle", &WMI::ReqAllianceBattle );
		REG_LUA_CLASS_FUNC( WMI, "ReqGuidanceMapName", &WMI::ReqGuidanceMapName );
		REG_LUA_CLASS_FUNC( WMI, "ReqCostumeStatRelease", &WMI::ReqCostumeStatRelease );
		REG_LUA_CLASS_FUNC( WMI, "NotifyReferCharPublic", &WMI::NotifyReferCharPublic );
		REG_LUA_CLASS_FUNC( WMI, "ReqItemBankInfo", &WMI::ReqItemBankInfo );
		REG_LUA_CLASS_FUNC( WMI, "WaitKeyAndTryLogin", &WMI::WaitKeyAndTryLogin );
		REG_LUA_CLASS_FUNC( WMI, "ReqPetSkin", &WMI::ReqPetSkin );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileCreateMoney", &WMI::ReqRanMobileCreateMoney );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileCreateCash", &WMI::ReqRanMobileCreateCash );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileExtendTime", &WMI::ReqRanMobileExtendTime );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileInfo", &WMI::ReqRanMobileInfo );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileSendJewel", &WMI::ReqRanMobileSendJewel );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileGetDailyItem", &WMI::ReqRanMobileGetDailyItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileChangeItem", &WMI::ReqRanMobileChangeItem );
		REG_LUA_CLASS_FUNC( WMI, "ReqAllianceBattleArmisticeAns", &WMI::ReqAllianceBattleArmisticeAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqClubBattleArmisticeAns", &WMI::ReqClubBattleArmisticeAns );
		REG_LUA_CLASS_FUNC( WMI, "ReqInvenClubRename", &WMI::ReqInvenClubRename );
		REG_LUA_CLASS_FUNC( WMI, "ReqRanMobileInfoAll", &WMI::ReqRanMobileInfoAll );
	}
	REG_LUA_CLASS( "g_Msg", &GLWidgetMsgInterface::GetInstance() );

	// 클라이언트 --------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Client", GLGaeaClient );
	{
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetMemberInfoList", &GLGaeaClient::GetMemberInfoList );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubNoticeDate", &GLGaeaClient::GetClubNoticeDate );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubMasterDBNum", &GLGaeaClient::GetClubMasterDBNum );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubMasterName", &GLGaeaClient::GetClubMasterName );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubMasterGradeName", &GLGaeaClient::GetMyClubMasterGradeName );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubLogText", &GLGaeaClient::GetClubLogText );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubCreateDate", &GLGaeaClient::GetClubCreateDate );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubRank", &GLGaeaClient::GetClubRank );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetOhterCharClubName", &GLGaeaClient::GetOhterCharClubName );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubBattleRecords", &GLGaeaClient::GetClubBattleRecords );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetCDMRecord", &GLGaeaClient::GetCDMRecord );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetAllianceClubInfo", &GLGaeaClient::GetAllianceClubInfo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetHostileClubInfo", &GLGaeaClient::GetHostileClubInfo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubMarkInfo", &GLGaeaClient::GetClubMarkInfo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "ClubStorageMoney", &GLGaeaClient::ClubStorageMoney );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubNickName", &GLGaeaClient::GetClubNickName_char );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubAuthority", &GLGaeaClient::GetClubAuthority );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetClubGradeName", &GLGaeaClient::GetClubGradeName );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetDisolutiontime", &GLGaeaClient::GetDisolutiontime );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "ClubCDAuthCheck", &GLGaeaClient::ClubCDAuthCheck );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetCTFTime", &GLGaeaClient::GetCTFProgressTime );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "SetFlyCamCtrlMoveVelo", &GLGaeaClient::SetFlyCameraControl_MoveVelo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetFlyCamCtrlMoveVelo", &GLGaeaClient::GetFlyCameraControl_MoveVelo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "SetFlyCamCtrlRotVelo", &GLGaeaClient::SetFlyCameraControl_RotateVelo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetFlyCamCtrlRotVelo", &GLGaeaClient::GetFlyCameraControl_RotateVelo );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "VehicleWithAns", &GLGaeaClient::VehicleWithAns );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "Vehicle_GetoffwithVehicle", &GLGaeaClient::Vehicle_GetoffwithVehicle );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsSelectCountry", &GLGaeaClient::IsSelectCountry );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetSelectedCountryID", &GLGaeaClient::GetSelectedCountryID );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetSelectedCountryName", &GLGaeaClient::GetSelectedCountryName );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsCharPrison", &GLGaeaClient::IsCharPrison );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "TournamentTeamTextInfoRecall", &GLGaeaClient::SfTournamentTeamTextInfoRecall);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "SfReqPartyMemberGetSkillFactInfo", &GLGaeaClient::SfReqPartyMemberGetSkillFactInfo);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_GetItemInfo", &GLGaeaClient::SfPartyDistribution_GetItemInfo);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_AbadonItem", &GLGaeaClient::SfPartyDistribution_AbadonItem);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_SelectItem", &GLGaeaClient::SfPartyDistribution_SelectItem);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_Bindding", &GLGaeaClient::SfPartyDistribution_Bidding);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_ResetSelectByItemID", &GLGaeaClient::SfPartyDistribution_ResetSelectByItemID);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyResetSelectAll", &GLGaeaClient::SfResetSelectAll);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_SelectMember", &GLGaeaClient::SfPartyDistribution_SelectMember);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_GetMemberInfo", &GLGaeaClient::SfPartyDistribution_GetMemberInfo);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_TransferItem", &GLGaeaClient::SfPartyDistribution_TransferItem);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PartyDistribution_JunkItem", &GLGaeaClient::SfPartyDistribution_JunkItem);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetCurrentMapGuidClubInfo", &GLGaeaClient::SfGetCurrentMapGuidClubInfo);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "PeriodExtensionReconfirm", &GLGaeaClient::SfPeriodExtensionReconfirm);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "ReqPeriodExtension", &GLGaeaClient::SfReqPeriodExtension);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetGateOpen", &GLGaeaClient::GetGateOpen);
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsInstantMap", &GLGaeaClient::IsInstantMap );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsNewInstanceMap", &GLGaeaClient::IsNewInstanceMap );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsOldInstanceMap", &GLGaeaClient::IsOldInstanceMap );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsWorldBattleStage", &GLGaeaClient::SfIsWorldBattleStage );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "GetOpenCharacterSlotCount", &GLGaeaClient::GetOpenCharacterSlotCount );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsCTFMap_PVP", &GLGaeaClient::IsCTFMap_PVP );
		REG_LUA_CLASS_FUNC( GLGaeaClient, "IsCTFMap_PVE", &GLGaeaClient::IsCTFMap_PVE );
	}
	REG_LUA_CLASS( "g_Client", m_pGaeaClient );

	// 로그인 ------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Login", GLLoginClient );
	{
		REG_LUA_CLASS_FUNC( GLLoginClient, "IsOnline", &GLLoginClient::IsOnline );
		REG_LUA_CLASS_FUNC( GLLoginClient, "IsGameServerInfoEnd", &GLLoginClient::IsGameServerInfoEnd );
		REG_LUA_CLASS_FUNC( GLLoginClient, "GetGameServerList", &GLLoginClient::GetGameServerList );
		REG_LUA_CLASS_FUNC( GLLoginClient, "GetServerChannelList", &GLLoginClient::GetServerChannelList );
		REG_LUA_CLASS_FUNC( GLLoginClient, "SetServerNumber", &GLLoginClient::SetServerNumber );
		REG_LUA_CLASS_FUNC( GLLoginClient, "GetRandomPassNumber", &GLLoginClient::GetRandomPassNumber );
		REG_LUA_CLASS_FUNC( GLLoginClient, "GetCharRemain", &GLLoginClient::GetCharRemain );
		REG_LUA_CLASS_FUNC( GLLoginClient, "GetCharRemainEx", &GLLoginClient::GetCharRemainEx );
		REG_LUA_CLASS_FUNC( GLLoginClient, "GetCharRemainActor", &GLLoginClient::GetCharRemainActor );
		REG_LUA_CLASS_FUNC( GLLoginClient, "SaveUserID", &GLLoginClient::SaveUserID );
		REG_LUA_CLASS_FUNC( GLLoginClient, "SetSaveUserID", &GLLoginClient::SetSaveUserID );
		REG_LUA_CLASS_FUNC( GLLoginClient, "IsSaveUserID", &GLLoginClient::IsSaveUserID );

		REG_LUA_CLASS_VAL( GLLoginClient, "m_nSelectServerIndex", &GLLoginClient::m_nSelectServerIndex );
		REG_LUA_CLASS_VAL( GLLoginClient, "m_nServerGroup", &GLLoginClient::m_nServerGroup );
		REG_LUA_CLASS_VAL( GLLoginClient, "m_nServerChannel", &GLLoginClient::m_nServerChannel );
	}
	REG_LUA_CLASS( "g_Login", &GLLoginClient::GetInstance() );

	//클럽 Lua파일의 데이터얻는 용도------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "ClubDefine", club::ClubDefineData );
	{
		REG_LUA_CLASS_FUNC( club::ClubDefineData, "GetAuthDesc", &club::ClubDefineData::GetAuthDesc );
		REG_LUA_CLASS_FUNC( club::ClubDefineData, "GetAuthDescSize", &club::ClubDefineData::GetDescSize );
		REG_LUA_CLASS_FUNC( club::ClubDefineData, "GetGradeName", &club::ClubDefineData::GetGradeName );
		REG_LUA_CLASS_FUNC( club::ClubDefineData, "GetGradeSize", &club::ClubDefineData::GetGradeSize );
	}
	REG_LUA_CLASS( "g_ClubDefine", GLogicData::GetInstance().GetClubBase());

	// 로비 캐릭터 ------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "CharSelect", GLLobyCharClient );
	{
		REG_LUA_CLASS_FUNC( GLLobyCharClient, "GetLobyCharInfoTable", &GLLobyCharClient::GetLobyCharInfoTable );
		REG_LUA_CLASS_FUNC( GLLobyCharClient, "SelectLobyChar", &GLLobyCharClient::SelectLobyChar );
		REG_LUA_CLASS_FUNC( GLLobyCharClient, "DisconnectServer", &GLLobyCharClient::DisconnectServer );
		REG_LUA_CLASS_FUNC( GLLobyCharClient, "DeleteChar", &GLLobyCharClient::DeleteChar );
		REG_LUA_CLASS_FUNC( GLLobyCharClient, "Get2ndPassType", &GLLobyCharClient::Get2ndPassType );

	}
	REG_LUA_CLASS( "g_CharSelect", &GLLobyCharClient::GetInstance() );

	// 캐릭터 생성 -------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "CharCreate", GLLobyCharCreateClient );
	{
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "SelectCharSchool", &GLLobyCharCreateClient::SelectCharSchool );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "SelectCharClass", &GLLobyCharCreateClient::SelectCharClass );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "SelectCharFace", &GLLobyCharCreateClient::SelectCharFace );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "SelectCharHair", &GLLobyCharCreateClient::SelectCharHair );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "ShiftCharFace", &GLLobyCharCreateClient::ShiftCharFace );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "ShiftCharHair", &GLLobyCharCreateClient::ShiftCharHair );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "SetStyleStep", &GLLobyCharCreateClient::SetStyleStep );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "RotateChar", &GLLobyCharCreateClient::RotateChar );
		REG_LUA_CLASS_FUNC( GLLobyCharCreateClient, "GetCharRect", &GLLobyCharCreateClient::GetCharacterRect );
		
		REG_LUA_CLASS_VAL( GLLobyCharCreateClient, "m_nSelectSchool", &GLLobyCharCreateClient::m_nSelectSchool );
		REG_LUA_CLASS_VAL( GLLobyCharCreateClient, "m_nSelectSex", &GLLobyCharCreateClient::m_nSelectSex );
		REG_LUA_CLASS_VAL( GLLobyCharCreateClient, "m_nSelectClass", &GLLobyCharCreateClient::m_nSelectClass );
	}
	REG_LUA_CLASS( "g_CharCreate", &GLLobyCharCreateClient::GetInstance() );

	// 캐릭터 --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Character", GLCharacter );
	{
		REG_LUA_CLASS_FUNC( GLCharacter, "UserLevel", &GLCharacter::UserLevel );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetHP", &GLCharacter::GETHP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMaxHP", &GLCharacter::GETMAXHP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMP", &GLCharacter::GETMP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMaxMP", &GLCharacter::GETMAXMP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSP", &GLCharacter::GETSP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMaxSP", &GLCharacter::GETMAXSP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetEXP", &GLCharacter::GETEXP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMaxEXP", &GLCharacter::GETMAXEXP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCP", &GLCharacter::GETCP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMaxCP", &GLCharacter::GETMAXCP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetLevel", &GLCharacter::GetLevel );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetChaRebornNum", &GLCharacter::GetChaRebornNum );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetName", &GLCharacter::GetName );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetClubName", &GLCharacter::GetClubName );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetTitle", &GLCharacter::GetTitle );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetTitleSelectIndex", &GLCharacter::GetTitleSelectIndex );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSchool", &GLCharacter::GetCharSchool );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPow", &GLCharacter::GetPow );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetDex", &GLCharacter::GetDex );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSpi", &GLCharacter::GetSpi );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetStr", &GLCharacter::GetStr );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSta", &GLCharacter::GetSta );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPow_Skill", &GLCharacter::GetPow_Skill );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetDex_Skill", &GLCharacter::GetDex_Skill );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSpi_Skill", &GLCharacter::GetSpi_Skill );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetStr_Skill", &GLCharacter::GetStr_Skill );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSta_Skill", &GLCharacter::GetSta_Skill );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAddedPow", &GLCharacter::GetAddedPow );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAddedDex", &GLCharacter::GetAddedDex );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAddedSpi", &GLCharacter::GetAddedSpi );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAddedStr", &GLCharacter::GetAddedStr );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAddedSta", &GLCharacter::GetAddedSta );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetForce_High", &GLCharacter::GETFORCE_HIGH );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetForce_Low", &GLCharacter::GETFORCE_LOW );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetDefense", &GLCharacter::GETDEFENSE );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSumPowerAttribute", &GLCharacter::GetSumPowerAttribute );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetHit", &GLCharacter::GETHIT );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAvoid", &GLCharacter::GETAVOID );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetResistFire", &GLCharacter::GetResistFire );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetResistIce", &GLCharacter::GetResistIce );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetResistElectric", &GLCharacter::GetResistElectric );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetResistPoison", &GLCharacter::GetResistPoison );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetResistSpirit", &GLCharacter::GetResistSpirit );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetChangeUpAbillity", &GLCharacter::GetChangeUpAbillity );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPutOnItem", &GLCharacter::GetPutOnItem );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPutOnItems", &GLCharacter::GetPutOnItems );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPutOnItemCount", &GLCharacter::GetPutOnItemCount );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPutOnItemType", &GLCharacter::GetPutOnItemType );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetWeaponType", &GLCharacter::GetWeaponType );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetInvenMoney", &GLCharacter::GetInvenMoney );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPointShopPoint", &GLCharacter::PointShopPoint );
		REG_LUA_CLASS_FUNC( GLCharacter, "CheckTrashItem", &GLCharacter::CheckTrashItem );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetLastSkillID", &GLCharacter::GetSkillRunSlotID );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSkillQuick", &GLCharacter::GetSkillQuick );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetItemQuick", &GLCharacter::GetItemQuick );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSkillList", &GLCharacter::GetSkillList );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetLearnedSkillCost", &GLCharacter::GetLearnedSkillCost );// 스킬 배움 반환 금액
		REG_LUA_CLASS_FUNC( GLCharacter, "ResetSkillStatsBySkillUI", &GLCharacter::RequestResetSkillStatsBySkillUI );// 스텟, 스킬 초기화 
		REG_LUA_CLASS_FUNC( GLCharacter, "GetUsingSkillPoint", &GLCharacter::GetUsingSkillPoint );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSkillInvest", &GLCharacter::GetSkillInvest );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetLearnSkillMaxLevel", &GLCharacter::GetLearnSkillMaxLevel );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetLearnSkillMinLevel", &GLCharacter::GetLearnSkillMinLevel );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetNeedSkillPoint", &GLCharacter::GetNeedSkillPoint );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetClubID", &GLCharacter::GetClubID );
		REG_LUA_CLASS_FUNC( GLCharacter, "CheckIsValidClub", &GLCharacter::CheckIsValidClub );	
		//REG_LUA_CLASS_FUNC( GLCharacter, "IsClubValid", &GLCharacter::IsValidClub );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetDBNum", &GLCharacter::CharDbNum );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsRideAVehicleNow", &GLCharacter::IsRideAVehicleNow );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetHaveVehicleAll", &GLCharacter::GetHaveVehicleAll );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetConftMessage", &GLCharacter::GetConftMessage );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetHPRevoverNum", &GLCharacter::GetHPRevoverNum );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetHPRate_Mine", &GLCharacter::GetHPRate_Mine );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetHPRate_Target", &GLCharacter::GetHPRate_Target );
		REG_LUA_CLASS_FUNC( GLCharacter, "HaveClubJoinAutority", &GLCharacter::HaveClubJoinAutority );
		REG_LUA_CLASS_FUNC( GLCharacter, "HaveClubAllianceAuthority", &GLCharacter::HaveClubAllianceAuthority );
		REG_LUA_CLASS_FUNC( GLCharacter, "HaveClubBattleAuthority", &GLCharacter::HaveClubBattleAuthority );
		REG_LUA_CLASS_FUNC( GLCharacter, "HaveClubStorageAuthority", &GLCharacter::HaveClubStorageAuthority );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsAllianceMaster", &GLCharacter::IsAllianceMaster );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsClubMaster", &GLCharacter::IsClubMaster );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsHaveAllianceDisAuthority", &GLCharacter::IsHaveAllianceDisAuthority ); // 클럽 동맹해지 권한확인
		REG_LUA_CLASS_FUNC( GLCharacter, "HairStyleChange", &GLCharacter::HairStyleChange );
		REG_LUA_CLASS_FUNC( GLCharacter, "FaceStyleChange", &GLCharacter::FaceStyleChange );
		REG_LUA_CLASS_FUNC( GLCharacter, "UpdateSuit", &GLCharacter::UpdateSuit );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCriticalRate", &GLCharacter::GetCriticalRate );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCrushingBlow", &GLCharacter::GetCrushingBlow );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMoveSpeed", &GLCharacter::GetMoveSpeed );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetAtkSpeed", &GLCharacter::GetAtkSpeed );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetExpRate", &GLCharacter::GetExpRate );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetIncrHpTotal", &GLCharacter::GetIncrHpTotal );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetIncrMpTotal", &GLCharacter::GetIncrMpTotal );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetIncrSpTotal", &GLCharacter::GetIncrSpTotal );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetReceiveCP", &GLCharacter::GetReceiveCP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPotionHpInc", &GLCharacter::GetPotionHpInc );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPotionMpInc", &GLCharacter::GetPotionMpInc );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPotionSpInc", &GLCharacter::GetPotionSpInc );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCurrentMapID", &GLCharacter::GetCurrentMapID );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPosition", &GLCharacter::GetPositionForLua );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMapPosition", &GLCharacter::GetMapPosition );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetMapPositionOffset", &GLCharacter::GetMapPositionOffset );
		REG_LUA_CLASS_FUNC( GLCharacter, "SetVehicleColor", &GLCharacter::SetVehicleColor );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetVehicleColor", &GLCharacter::GetVehicleColor );
		REG_LUA_CLASS_FUNC( GLCharacter, "SetCostumeColor", &GLCharacter::SetCostumeColor );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCostumeColor", &GLCharacter::GetCostumeColor );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetOnInvenLine", &GLCharacter::GetOnINVENLINE );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetResurrectionCondition", &GLCharacter::GetResurrectionCondition );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetRsurrectionExpRecovery", &GLCharacter::GetRsurrectionExpRecovery );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetLevelUpExp", &GLCharacter::GET_LEVELUP_EXP );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetItemPosInInvenByTable", &GLCharacter::GetItemPosInInvenByTable );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetQuestListProc", &GLCharacter::GetQuestListProc );
		REG_LUA_CLASS_FUNC( GLCharacter, "GettbQuestProc", &GLCharacter::GettbQuestProc );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetQuestListEnd", &GLCharacter::GetQuestListEnd );
		REG_LUA_CLASS_FUNC( GLCharacter, "GettbQuestEnd", &GLCharacter::GettbQuestEnd );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetQuestComplete", &GLCharacter::GetQuestComplete );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetQuestStep", &GLCharacter::GetQuestStep );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetQuestCompleteCheck", &GLCharacter::GetQuestCompleteCheck );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetQuestGiveUp", &GLCharacter::GetQuestGiveUp );
		REG_LUA_CLASS_FUNC( GLCharacter, "GettbQuestProcInfo", &GLCharacter::GettbQuestProcInfo );
		REG_LUA_CLASS_FUNC( GLCharacter, "SetQuestHelpCheck", &GLCharacter::SetQuestHelpCheck );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSkillFactAge", &GLCharacter::GetSkillFactAge );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetStorageMoney", &GLCharacter::GetStorageMoney );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetStorageTime", &GLCharacter::GetStorageTimeForWidget );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsUseArmSub", &GLCharacter::IsUseArmSub );
		REG_LUA_CLASS_FUNC( GLCharacter, "VehicleActiveValue", &GLCharacter::VehicleActiveValue );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPutOnVehicleInfo", &GLCharacter::GetPutOnVehicleInfo );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetVehicleBoosterGauge", &GLCharacter::GetVehicleBoosterGauge );
		REG_LUA_CLASS_FUNC( GLCharacter, "VehicleFull", &GLCharacter::VehicleFull );
		REG_LUA_CLASS_FUNC( GLCharacter, "TempPutOnStatInvenItem", &GLCharacter::TempPutOnStatInvenItem );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetStartMapID", &GLCharacter::GetStartMapID );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetPremium", &GLCharacter::GetPremium );
		REG_LUA_CLASS_FUNC( GLCharacter, "FindInvenItemPos", &GLCharacter::FindInvenItemPos );		
		REG_LUA_CLASS_FUNC( GLCharacter, "PartyMemberSkillReaction", &GLCharacter::ReqPartyMemberSkillReaction );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetBuffList", &GLCharacter::GetBuffList );
		REG_LUA_CLASS_FUNC( GLCharacter, "CheckInvenItemDecompose", &GLCharacter::CheckInvenItemDecompose );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetEquipConsume", &GLCharacter::GetEquipConsume );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetRemainMacroTime", &GLCharacter::GetRemainMacroTime );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetRepairableItem", &GLCharacter::GetRepairableItem );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSummonHPInfo", &GLCharacter::GetSummonHPInfo );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSummonName", &GLCharacter::GetSummonName );
		REG_LUA_CLASS_FUNC( GLCharacter, "HairColorChange", &GLCharacter::HairColorChange );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetWeaponBoostInitData", &GLCharacter::GetWeaponBoostInitData );
		REG_LUA_CLASS_FUNC( GLCharacter, "ReqQBoxEnableState", &GLCharacter::ReqQBoxEnableState );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetInvenItemCount", &GLCharacter::GetInvenItemCount );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsExistReturnSchoolCard", &GLCharacter::IsExistReturnSchoolCard );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetInvenItemSuitType", &GLCharacter::GetInvenItemSuitType );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetInvenItemType", &GLCharacter::GetInvenItemType );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsEnableLunchBox", &GLCharacter::IsEnableLunchBox );
		REG_LUA_CLASS_FUNC( GLCharacter, "UseDisguiseInvenItem", &GLCharacter::UseDisguiseInvenItem );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsDisguiseInvenItem", &GLCharacter::IsDisguiseInvenItem );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetSlotTurnNumCountItem", &GLCharacter::GetSlotTurnNumCountItem );
		// 슬롯아이템의 수량을 받아온다. 다른 함수들은 그 아이템의 전체 수량을 받아옴.
		REG_LUA_CLASS_FUNC( GLCharacter, "ReqSelformBoxSelect", &GLCharacter::ReqSelformBoxSelect );
		REG_LUA_CLASS_FUNC( GLCharacter, "WearableCheck", &GLCharacter::WearableCheck );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCharInfoDetailAddonLimitInfo", &GLCharacter::GetCharInfoDetailAddonLimitInfo );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCharInfoDetailAddonInfo", &GLCharacter::GetCharInfoDetailAddonInfo );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCharTempPuonInfoDetailAddonInfo", &GLCharacter::GetCharTempPuonInfoDetailAddonInfo );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCharacterSlotLockState", &GLCharacter::GET_CHARACTER_SLOT_LOCKSTATE );
		REG_LUA_CLASS_FUNC( GLCharacter, "GetCurrentCharacterSlot", &GLCharacter::GET_CURRENT_CHARACTER_SLOT );
		REG_LUA_CLASS_FUNC( GLCharacter, "ReqChangeCharSlot", &GLCharacter::ReqChangeCharSlot );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsChangeCharSlot", &GLCharacter::IsChangeCharSlot );
		REG_LUA_CLASS_FUNC( GLCharacter, "IsIdle", &GLCharacter::IsIdleCharacter );

		REG_LUA_CLASS_VAL( GLCharacter, "nClassIndex", &GLCharacter::m_CHARINDEX );
		REG_LUA_CLASS_VAL( GLCharacter, "nAtb", &GLCharacter::m_nBright );
		REG_LUA_CLASS_VAL( GLCharacter, "nLivingPoint", &GLCharacter::m_nLiving );
		REG_LUA_CLASS_VAL( GLCharacter, "nCsb", &GLCharacter::m_nContributionPoint );
		REG_LUA_CLASS_VAL( GLCharacter, "nStatPoint", &GLCharacter::m_wStatsPoint );
		REG_LUA_CLASS_VAL( GLCharacter, "nSkillPoint", &GLCharacter::m_dwSkillPoint );
		REG_LUA_CLASS_VAL( GLCharacter, "m_emClass", &GLCharacter::m_emClass );
		REG_LUA_CLASS_VAL( GLCharacter, "m_wSex", &GLCharacter::m_wSex );
		REG_LUA_CLASS_VAL( GLCharacter, "m_wHair", &GLCharacter::m_wHair );
		REG_LUA_CLASS_VAL( GLCharacter, "m_wHairColor", &GLCharacter::m_wHairColor );
		REG_LUA_CLASS_VAL( GLCharacter, "m_wFace", &GLCharacter::m_wFace );
		REG_LUA_CLASS_VAL( GLCharacter, "dwServerID", &GLCharacter::m_dwServerID );
	}
	REG_LUA_CLASS( "g_MyChar", m_pGaeaClient->GetCharacter() );

	// 미니게임 ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "ManagerMiniGame", MiniGame::GLManagerClient );
	{
		typedef MiniGame::GLManagerClient MGM;

		REG_LUA_CLASS_FUNC( MGM, "getStageCurrent", &MGM::getStageCurrent );
		REG_LUA_CLASS_FUNC( MGM, "getStepCurrent", &MGM::getStepCurrent );
		REG_LUA_CLASS_FUNC( MGM, "getStepMaximum", &MGM::getStepMaximum );
		REG_LUA_CLASS_FUNC( MGM, "getMoneyCost", &MGM::getMoneyCost );
		REG_LUA_CLASS_FUNC( MGM, "getItemIDReward", &MGM::getItemIDReward );		
		REG_LUA_CLASS_FUNC( MGM, "isWin", &MGM::isWin );
		REG_LUA_CLASS_FUNC( MGM, "getTimeLimit", &MGM::getTimeLimit );
	}
	REG_LUA_CLASS( "g_ManagerMiniGame", m_pGaeaClient->GetMiniGameManager() );
	
	// 캐릭터 종류별 기본 상수 -------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLCONST_CHARCLASS", GLCONST_CHARCLASS );
	for( int i = 0; i < GLCI_NUM_ACTOR; ++i )
	{
		{
			REG_LUA_CLASS_VAL( GLCONST_CHARCLASS, "dwHAIRNUM", &GLCONST_CHARCLASS::dwHAIRNUM );
			REG_LUA_CLASS_VAL( GLCONST_CHARCLASS, "dwHEADNUM", &GLCONST_CHARCLASS::dwHEADNUM );
			REG_LUA_CLASS_VAL( GLCONST_CHARCLASS, "dwHAIRNUM_SELECT", &GLCONST_CHARCLASS::dwHAIRNUM_SELECT );
		}

		CString str;
		str.Format( "g_CONST_CHARCLASS%d", i );

		REG_LUA_CLASS( str.GetBuffer(), &GLCONST_CHAR::cCONSTCLASS[ i ] );
	}

	// 채팅 --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "ChatMan", GLChatMan );
	{
		REG_LUA_CLASS_FUNC( GLChatMan, "SetChatMacro", &GLChatMan::SetChatMacro );
		REG_LUA_CLASS_FUNC( GLChatMan, "GetChatMacro", &GLChatMan::GetChatMacro );
		REG_LUA_CLASS_FUNC( GLChatMan, "CreatePartyLink", &GLChatMan::CreatePartyLink );
		REG_LUA_CLASS_FUNC( GLChatMan, "GetChatBlock", &GLChatMan::GetChatBlock );
	}
	REG_LUA_CLASS( "g_ChatMan", &GLChatMan::GetInstance() );

	// 디바이스 설정 -----------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "DxParamSet", DXPARAMSET::STESTSET );
	{
		using namespace DXPARAMSET;

		REG_LUA_CLASS_FUNC( STESTSET, "VideoOK", &STESTSET::VideoOKFromWidget );
		REG_LUA_CLASS_FUNC( STESTSET, "AudioOK", &STESTSET::AudioOKFromWidget );
		REG_LUA_CLASS_FUNC( STESTSET, "GameOK", &STESTSET::GameOKFromWidget );

		REG_LUA_CLASS_VAL( STESTSET, "dwWidth", &STESTSET::m_dwScrWidth );
		REG_LUA_CLASS_VAL( STESTSET, "dwHeight", &STESTSET::m_dwScrHeight );
		REG_LUA_CLASS_VAL( STESTSET, "emFormat", &STESTSET::m_emScrFormat );
		REG_LUA_CLASS_VAL( STESTSET, "dwShadow", &STESTSET::m_dwShadowChar );
		REG_LUA_CLASS_VAL( STESTSET, "dwSight", &STESTSET::m_dwFogRange );
		REG_LUA_CLASS_VAL( STESTSET, "dwDetail", &STESTSET::m_dwSkinDetail );
		REG_LUA_CLASS_VAL( STESTSET, "dwTexFilter", &STESTSET::m_dwTexFilter );
		REG_LUA_CLASS_VAL( STESTSET, "dwTexQuality", &STESTSET::m_dwTexQuality );
		REG_LUA_CLASS_VAL( STESTSET, "dwPhysXCloth", &STESTSET::m_dwPhysXCloth );
		REG_LUA_CLASS_VAL( STESTSET, "dwDefault", &STESTSET::m_dwVideoLevel );
		REG_LUA_CLASS_VAL( STESTSET, "bBuff", &STESTSET::m_bBuff );
		REG_LUA_CLASS_VAL( STESTSET, "bRealReflect", &STESTSET::m_bRealReflect );
		REG_LUA_CLASS_VAL( STESTSET, "bRefract", &STESTSET::m_bRefract );
		REG_LUA_CLASS_VAL( STESTSET, "bGlow", &STESTSET::m_bGlow );
		REG_LUA_CLASS_VAL( STESTSET, "bPost", &STESTSET::m_bPost );
		REG_LUA_CLASS_VAL( STESTSET, "bFrameLimit", &STESTSET::m_bFrameLimit );
		REG_LUA_CLASS_VAL( STESTSET, "bTargetEffect", &STESTSET::m_bTargetEffect );
		REG_LUA_CLASS_VAL( STESTSET, "bClickEffect", &STESTSET::m_bClickEffect );
		REG_LUA_CLASS_VAL( STESTSET, "bMineEffect", &STESTSET::m_bMineEffect );
		REG_LUA_CLASS_VAL( STESTSET, "bPossibleNormalMap", &STESTSET::m_bPossibleNormalMap );
		REG_LUA_CLASS_VAL( STESTSET, "bUseNormalMap", &STESTSET::m_bUseNormalMap );
	}
	REG_LUA_CLASS( "g_DxParamSet", &DXPARAMSET::GetInstance() );

	// 파티 ---------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLPartyClient", GLPartyClient );
	{
		REG_LUA_CLASS_FUNC( GLPartyClient, "IsValid", &GLPartyClient::isValid );
		REG_LUA_CLASS_FUNC( GLPartyClient, "GetMember", &GLPartyClient::GetMemberForWidget );
		REG_LUA_CLASS_FUNC( GLPartyClient, "IsMyCharMaster", &GLPartyClient::IsMyCharMaster );
		REG_LUA_CLASS_FUNC( GLPartyClient, "GetItemOption", &GLPartyClient::GetItemOption );
		REG_LUA_CLASS_FUNC( GLPartyClient, "GetMoneyOption", &GLPartyClient::GetMoneyOption );
		REG_LUA_CLASS_FUNC( GLPartyClient, "GetItemFilterOption", &GLPartyClient::GetItemFilterOption );
		REG_LUA_CLASS_FUNC( GLPartyClient, "GetPartyID", &GLPartyClient::GetSimplePartyID );
	}
	REG_LUA_CLASS( "g_Party", m_pGaeaClient->GetMyPartyClient() );

	// 인풋 매니저 --------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLInputManager", GLInputManager );
	{
		typedef GLInputManager IM;

		REG_LUA_CLASS_FUNC( IM, "InitControlType", &IM::InitControlTypeForWidget );
		REG_LUA_CLASS_FUNC( IM, "GetCurrentControlType", &IM::GetCurrentControlType );
		REG_LUA_CLASS_FUNC( IM, "SetShortcutsDataFromString", &IM::SetShortcutsDataFromStringForWidget );
		REG_LUA_CLASS_FUNC( IM, "GetShortcutsToString", &IM::GetShortcutsToStringForWidget );
		REG_LUA_CLASS_FUNC( IM, "GetDefaultShortcutsToString", &IM::GetDefaultShortcutsToStringForWidget );
	}
	REG_LUA_CLASS( "g_InputMan", &GLInputManager::GetInstance() );

	// NPC 대화 ------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLNPCTalkClient", GLNpcTalkClient );
	{
		typedef GLNpcTalkClient NT;

		REG_LUA_CLASS_FUNC( NT, "GetBasicTalk", &NT::GetBasicTalk );
		REG_LUA_CLASS_FUNC( NT, "GetAnswerList", &NT::GetAnswerList );
		REG_LUA_CLASS_FUNC( NT, "GetMarketTab", &NT::GetMarketTab );
		REG_LUA_CLASS_FUNC( NT, "GetSaleItem", &NT::GetSaleItem );
		REG_LUA_CLASS_FUNC( NT, "GetRcommendSaleItem", &NT::GetRcommendSaleItem );
		REG_LUA_CLASS_FUNC( NT, "SetUsableItem", &NT::SetUsableItem );
		REG_LUA_CLASS_FUNC( NT, "GetUsableItem", &NT::GetUsableItem );
		REG_LUA_CLASS_FUNC( NT, "SetPurchaseItem", &NT::SetPurchaseItem );
		REG_LUA_CLASS_FUNC( NT, "GetPurchaseItem", &NT::GetPurchaseItem );
		REG_LUA_CLASS_FUNC( NT, "SetSellRMouse", &NT::SetSellRMouse );
		REG_LUA_CLASS_FUNC( NT, "GetSellRMouse", &NT::GetSellRMouse );
		REG_LUA_CLASS_FUNC( NT, "SetConfirmSellItem", &NT::SetConfirmSellItem );
		REG_LUA_CLASS_FUNC( NT, "GetConfirmSellItem", &NT::GetConfirmSellItem );
		REG_LUA_CLASS_FUNC( NT, "SetRebuySupplies", &NT::SetRebuySupplies );
		REG_LUA_CLASS_FUNC( NT, "GetRebuySupplies", &NT::GetRebuySupplies );
		REG_LUA_CLASS_FUNC( NT, "SetRebuyItemDeleteConfirm", &NT::SetRebuyItemDeleteConfirm );
		REG_LUA_CLASS_FUNC( NT, "GetRebuyItemDeleteConfirm", &NT::GetRebuyItemDeleteConfirm );
		REG_LUA_CLASS_FUNC( NT, "AddCart", &NT::AddCart );
		REG_LUA_CLASS_FUNC( NT, "RemoveCart", &NT::RemoveCart );
		REG_LUA_CLASS_FUNC( NT, "RemoveCartAll", &NT::RemoveCartAll );
		REG_LUA_CLASS_FUNC( NT, "GetCart", &NT::GetCart );
		REG_LUA_CLASS_FUNC( NT, "GetCartTotalPrice", &NT::GetCartTotalPrice );
		REG_LUA_CLASS_FUNC( NT, "GetRebuy", &NT::GetRebuy );
		REG_LUA_CLASS_FUNC( NT, "GetRebuyCount", &NT::GetRebuyCount );
		REG_LUA_CLASS_FUNC( NT, "GetAdItemID", &NT::GetAdItemID );
		REG_LUA_CLASS_FUNC( NT, "GetAdItemSearchString", &NT::GetAdItemSearchString );
		REG_LUA_CLASS_FUNC( NT, "IsUseable", &NT::IsUseable );
		REG_LUA_CLASS_FUNC( NT, "IsUseableFromID", &NT::IsUseableFromID );
	}
	REG_LUA_CLASS( "g_NpcTalk", &GLNpcTalkClient::GetInstance() );

	// PointShop --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "PointShopClient", pointshop::PointShopClient );
	{
		typedef pointshop::PointShopClient PS;

		REG_LUA_CLASS_FUNC( PS, "GetVehicleBoardList", &PS::GetVehicleBoardList );
		REG_LUA_CLASS_FUNC( PS, "GetVehicleBikeList", &PS::GetVehicleBikeList );
		REG_LUA_CLASS_FUNC( PS, "GetVehicleOilList", &PS::GetVehicleOilList );
		REG_LUA_CLASS_FUNC( PS, "GetVehiclePartsList", &PS::GetVehiclePartsList );
		
		REG_LUA_CLASS_FUNC( PS, "GetCategoryItem", &PS::GetCategoryItem );
		REG_LUA_CLASS_FUNC( PS, "GetItemList", &PS::GetItemList );
		REG_LUA_CLASS_FUNC( PS, "GetCategoryList", &PS::GetCategoryList );
		REG_LUA_CLASS_FUNC( PS, "GetCartList", &PS::GetCartList );
		REG_LUA_CLASS_FUNC( PS, "DelCartList", &PS::DelCartList );
		REG_LUA_CLASS_FUNC( PS, "AddCartList", &PS::AddCartListScaleform ); 
	}
	REG_LUA_CLASS( "g_PointShop", m_pGaeaClient->GetPointShopClient() );

	// Product -----------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLProductClient", GLProductClient );
	{
		typedef GLProductClient PD;

		REG_LUA_CLASS_FUNC( PD, "GetStudiedProducts", &PD::GetStudiedProducts );
		REG_LUA_CLASS_FUNC( PD, "GetProductList", &PD::GetProductList );
		REG_LUA_CLASS_FUNC( PD, "GetStudyState", &PD::GetStudyState );
		REG_LUA_CLASS_FUNC( PD, "GetProductSkill", &PD::GetProductSkill );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductData", &PD::GetSelectedProductData );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductRequirment", &PD::GetSelectedProductRequirment );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductName", &PD::GetSelectedProductName );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductResultData", &PD::GetSelectedProductResultData );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductKey", &PD::GetSelectedProductKey );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductCost", &PD::GetSelectedProductCost );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductDelay", &PD::GetSelectedProductDelay );
		REG_LUA_CLASS_FUNC( PD, "GetSelectedProductRate", &PD::GetSelectedProductRate );
		REG_LUA_CLASS_FUNC( PD, "GetProductTypeMaxStudyPoint", &PD::GetProductTypeMaxStudyPoint );
		REG_LUA_CLASS_FUNC( PD, "GetProductTypeStudyPoint", &PD::GetProductTypeStudyPoint );
		REG_LUA_CLASS_FUNC( PD, "IsUsableProductType", &PD::IsUsableProductType );
		REG_LUA_CLASS_FUNC( PD, "GetCountProduct", &PD::GetCountProduct );				// 제작 가능 개수 받아오기 
	}
	REG_LUA_CLASS( "g_Product", m_pGaeaClient->GetProductClient() );

	// Pet --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Pet", GLPetClient );
	{
		REG_LUA_CLASS_FUNC( GLPetClient, "IsVALID", &GLPetClient::IsVALID );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetName", &GLPetClient::GetName );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetType", &GLPetClient::GetType );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetTypeString", &GLPetClient::GetTypeString );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetPetCardID", &GLPetClient::GetPetCardID );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetPetItemList", &GLPetClient::GetPetItemList );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetSatiation", &GLPetClient::Satiation );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetPetSkill", &GLPetClient::GetPetSkillList );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetMaxSkillSlot", &GLPetClient::GetMaxSkillSlot );
		REG_LUA_CLASS_FUNC( GLPetClient, "StyleChange", &GLPetClient::StyleChange );
		REG_LUA_CLASS_FUNC( GLPetClient, "ColorChange", &GLPetClient::ColorChange );
		REG_LUA_CLASS_FUNC( GLPetClient, "UpdateSuit", &GLPetClient::UpdateSuit );
		REG_LUA_CLASS_FUNC( GLPetClient, "GetColor", &GLPetClient::GetColor );
		REG_LUA_CLASS_FUNC( GLPetClient, "SetColor", &GLPetClient::SetColor );
		
		REG_LUA_CLASS_VAL( GLPetClient, "m_emTYPE", &GLPetClient::m_emTYPE );
		REG_LUA_CLASS_VAL( GLPetClient, "m_wStyle", &GLPetClient::m_wStyle );
	}
	REG_LUA_CLASS( "g_Pet", m_pGaeaClient->GetPetClient() );
	
	// 친구 --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Friend", GLFriendClient );
	{
		REG_LUA_CLASS_FUNC( GLFriendClient, "IsFriend", &GLFriendClient::IsFriend );
		REG_LUA_CLASS_FUNC( GLFriendClient, "IsOnline", &GLFriendClient::IsOnline );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetFriendList", &GLFriendClient::GetFriendList );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetBlockList", &GLFriendClient::GetBlockList );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetGroupList", &GLFriendClient::GetGroupList );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetFriendListInGroup", &GLFriendClient::GetFriendListInGroup );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetMaxFriend", &GLFriendClient::GetMaxFriend );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetMaxBlock", &GLFriendClient::GetMaxBlock );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetMaxGroup", &GLFriendClient::GetMaxGroup );
		REG_LUA_CLASS_FUNC( GLFriendClient, "GetFriendCount", &GLFriendClient::GetFriendCount );
	}
	REG_LUA_CLASS( "g_Friend", m_pGaeaClient->GetFriendClient() );	
	
	// 개인상점검색 ---------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME("PrivateMarketSearch", PrivateMarketClient);
	{
		typedef PrivateMarketClient PM;
		
		REG_LUA_CLASS_FUNC( PM, "GetSearchItemList", &PM::GetSearchItemList );
		REG_LUA_CLASS_FUNC( PM, "GetSearchPageCount", &PM::GetSearchPageCount );
		REG_LUA_CLASS_FUNC( PM, "InsertWishItem", &PM::InsertWishItem );
		REG_LUA_CLASS_FUNC( PM, "DeleteWishItem", &PM::DeleteWishItem );
		REG_LUA_CLASS_FUNC( PM, "GetWishItemList", &PM::GetWishItemList );
		REG_LUA_CLASS_FUNC( PM, "GetLogList", &PM::GetLogList );
		REG_LUA_CLASS_FUNC( PM, "GetPointItemList", &PM::GetPointItemList );
		REG_LUA_CLASS_FUNC( PM, "GetSellingMoneyList", &PM::GetSellingMoneyList );
		REG_LUA_CLASS_FUNC( PM, "GetSellPeriodList", &PM::GetSellPeriodList );
		REG_LUA_CLASS_FUNC( PM, "GetSalesCompletionFee", &PM::GetSalesCompletionFee );
		REG_LUA_CLASS_FUNC( PM, "GetExpansionSlotInfo", &PM::GetExpansionSlotInfo );
		REG_LUA_CLASS_FUNC( PM, "CalcInsertionFee", &PM::CalcInsertionFee );
		REG_LUA_CLASS_FUNC( PM, "GetPointTradeMinPrice", &PM::GetPointTradeMinPrice );
		REG_LUA_CLASS_FUNC( PM, "GetSearchableItemNameList", &PM::GetSearchAbleItemNameList );
		REG_LUA_CLASS_FUNC( PM, "GetSearchPageCount", &PM::GetSearchPageCount );
	}
	REG_LUA_CLASS( "g_PrivateMarketSearch", m_pGaeaClient->GetPrivateMarketClient() );

	// 개인상점 ---------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "PrivateMarket", PrivateMarketClient );
	{
		REG_LUA_CLASS_FUNC( PrivateMarketClient, "IsOpener", &PrivateMarketClient::IsOpener );
		REG_LUA_CLASS_FUNC( PrivateMarketClient, "GetOpenerID", &PrivateMarketClient::GetOpenerID );
		REG_LUA_CLASS_FUNC( PrivateMarketClient, "IsRegInven", &PrivateMarketClient::IsRegInven );
		REG_LUA_CLASS_FUNC( PrivateMarketClient, "IsSold", &PrivateMarketClient::IsSold );
	}
	REG_LUA_CLASS( "g_PrivateMarket", m_pGaeaClient->GetPrivateMarketClient() );

	// 개인상점 ---------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLPrivateMarketDefine", private_market::GLPrivateMarketDefine );
	{
		typedef private_market::GLPrivateMarketDefine PMDef;

		REG_LUA_CLASS_FUNC( PMDef, "GetSearchDelayMillisecond", &PMDef::GetSearchDelayMillisecond );
		REG_LUA_CLASS_FUNC( PMDef, "GetLogDelayMillisecond", &PMDef::GetLogDelayMillisecond );
	}
	REG_LUA_CLASS( "g_PMDef", private_market::GLPrivateMarketDefine::Instance() );

	// 그룹채팅 -------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GroupChat", GLGroupChat );
	{
		REG_LUA_CLASS_FUNC( GLGroupChat, "GetMember", &GLGroupChat::GetMemberForWidget );
		REG_LUA_CLASS_FUNC( GLGroupChat, "GetLinkItemInfo", &GLGroupChat::GetLinkItemInfo );
		REG_LUA_CLASS_FUNC( GLGroupChat, "RemoveLinkItem", &GLGroupChat::RemoveLinkItem );
		REG_LUA_CLASS_FUNC( GLGroupChat, "GetMaxGroupChatTO", &GLGroupChat::GetMaxGroupChatTO );
	}
	REG_LUA_CLASS( "g_GroupChat", m_pGaeaClient->GetChatGroup() );

	// Tutorial --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "BaseTutorial", GLBaseTutorial );
	{
		REG_LUA_CLASS_FUNC( GLBaseTutorial, "IsTutorial", &GLBaseTutorial::IsTutorial );
	}
	REG_LUA_CLASS( "g_BaseTutorial", m_pGaeaClient->GetTutorial() );

	// 클럽--------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "ClubClient", GLClubClient);
	{
		REG_LUA_CLASS_FUNC( GLClubClient, "GetClubMemberList", &GLClubClient::GetClubMemberList);
		REG_LUA_CLASS_FUNC( GLClubClient, "GetNotice", &GLClubClient::GetNotice);	
		REG_LUA_CLASS_FUNC( GLClubClient, "GetNoticeChaName", &GLClubClient::GetNoticeChaName);
		REG_LUA_CLASS_FUNC( GLClubClient, "GetClubMemberOnOff", &GLClubClient::GetClubMemberOnOff);		
		REG_LUA_CLASS_FUNC( GLClubClient, "GetClubName", &GLClubClient::Name);
		REG_LUA_CLASS_FUNC( GLClubClient, "GetStorageMoney", &GLClubClient::GetStorageMoney);
		REG_LUA_CLASS_FUNC( GLClubClient, "GetIncomeMoney", &GLClubClient::GetIncomeMoney);
		REG_LUA_CLASS_FUNC( GLClubClient, "IsRegDissolution", &GLClubClient::IsRegDissolution);
		REG_LUA_CLASS_FUNC( GLClubClient, "GetAutoKickData", &GLClubClient::GetAutoKickData);
		REG_LUA_CLASS_FUNC( GLClubClient, "GetGudianceMapInfo", &GLClubClient::GetGudianceMapInfo );
	}
	REG_LUA_CLASS( "g_Club", m_pGaeaClient->GetMyClub() );

	// 정보보기창 --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME("CClient", ReferChar::CClient);
	{
		using namespace ReferChar;

		REG_LUA_CLASS_FUNC( CClient, "GetInfoData", &CClient::GetInfoData );
		REG_LUA_CLASS_FUNC( CClient, "GetCharInfoDetailAddonInfo", &CClient::GetCharInfoDetailAddonInfo );
		REG_LUA_CLASS_FUNC( CClient, "GetSkillData", &CClient::GetSkillData );
		REG_LUA_CLASS_FUNC( CClient, "GetSkillTabPoint", &CClient::GetSkillTabPoint );
		REG_LUA_CLASS_FUNC( CClient, "GetCharClass", &CClient::GetCharClass );
		REG_LUA_CLASS_FUNC( CClient, "GetPutOnItems", &CClient::GetPutOnItems );
		REG_LUA_CLASS_FUNC( CClient, "GetNameList", &CClient::GetNameListForWidget );
	}
	REG_LUA_CLASS( "g_Refer", m_pGaeaClient->GetReferChar() );

	// Post --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Post", GLPostClient );
	{
		REG_LUA_CLASS_FUNC( GLPostClient, "GetReceivePagePost", &GLPostClient::GetReceivePagePost );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetReceivePageSelectPost", &GLPostClient::GetReceivePageSelectPost );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetSendPageDesignType", &GLPostClient::GetSendPageDesignType );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetSendPageSlotItem", &GLPostClient::GetSendPageSlotItem );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetSendPageSlotItemTurnNum", &GLPostClient::GetSendPageSlotItemTurnNum );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetPreviewPagePost", &GLPostClient::GetPreviewPagePost );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetBaseFee", &GLPostClient::GetBaseFee );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetMaxAttachMoney", &GLPostClient::GetMaxAttachMoney );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetCommission", &GLPostClient::GetCommission );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetHistoryPageInfoSet", &GLPostClient::GetHistoryPageInfoSet );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetHistorySendPageInfoSet", &GLPostClient::GetHistorySendPageInfoSet );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetHistoryRecievePageInfoSet", &GLPostClient::GetHistoryRecievePageInfoSet );
		REG_LUA_CLASS_FUNC( GLPostClient, "HaveReceivePostAttachments", &GLPostClient::HaveReceivePostAttachments );
		
		
		REG_LUA_CLASS_FUNC( GLPostClient, "GetNewPost", &GLPostClient::GetNewPost );	
		REG_LUA_CLASS_FUNC( GLPostClient, "GetWaitingPost", &GLPostClient::GetWaitingPost );
		REG_LUA_CLASS_FUNC( GLPostClient, "GetReceivedNewPost", &GLPostClient::GetReceivedNewPost );
	}
	REG_LUA_CLASS( "g_Post", m_pGaeaClient->GetPostClient() );

	
	//전장 --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Competition", Competition );
	{
		REG_LUA_CLASS_FUNC( Competition, "GetMatchingInstanceList", &Competition::GetMatchingInstanceList );
		REG_LUA_CLASS_FUNC( Competition, "GetExceptionPvPInstanceList", &Competition::GetExceptionPvPInstanceList );
		REG_LUA_CLASS_FUNC( Competition, "GetPvEInstanceList", &Competition::GetPvEInstanceList );
		REG_LUA_CLASS_FUNC( Competition, "IsJoined", &Competition::IsJoined );
		REG_LUA_CLASS_FUNC( Competition, "GetSelectStatus", &Competition::GetSelectStatus );
		REG_LUA_CLASS_FUNC( Competition, "SelectInstanceDungeon", &Competition::SelectInstanceDungeon );
		REG_LUA_CLASS_FUNC( Competition, "GetCTFRewardFlag", &Competition::GetCTFRewardFlag );
		REG_LUA_CLASS_FUNC( Competition, "GetClientTimeInfo", &Competition::GetClientTimeInfo );
		REG_LUA_CLASS_FUNC( Competition, "GetCTFRankInfo", &Competition::GetCTFRankInfo );
		REG_LUA_CLASS_FUNC( Competition, "GetBettingGroupInfo", &Competition::GetBettingGroupInfo );
		REG_LUA_CLASS_FUNC( Competition, "GetCTFConsecutivelyCaptureSchool", &Competition::GetCTFConsecutivelyCaptureSchool );
		REG_LUA_CLASS_FUNC( Competition, "ConfirmJoin_OK", &Competition::ConfirmJoin_OK );
		REG_LUA_CLASS_FUNC( Competition, "ConfirmJoin_Cancel", &Competition::ConfirmJoin_Cancel );
	}
	REG_LUA_CLASS( "g_Competition", &Competition::GetInstance() );

	//Attendance --------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Attendance", attendance::GLAttendanceClient );
	{
		typedef attendance::GLAttendanceClient AT;

		REG_LUA_CLASS_FUNC( AT, "AttendanceUIGetStringCaption_1", &AT::AttendanceUIGetStringCaption_1 );
		REG_LUA_CLASS_FUNC( AT, "AttendanceUIGetStringCaption_2", &AT::AttendanceUIGetStringCaption_2 );
		REG_LUA_CLASS_FUNC( AT, "AttendanceUIGetStringProgress", &AT::AttendanceUIGetStringProgress );
		REG_LUA_CLASS_FUNC( AT, "GetPointTableInTask", &AT::GetPointTableInTask );
		REG_LUA_CLASS_FUNC( AT, "AttendanceUIGetState", &AT::GetState );
		REG_LUA_CLASS_FUNC( AT, "AttendanceUIGetAwardItem", &AT::AttendanceUIGetAwardItem );
		REG_LUA_CLASS_FUNC( AT, "AttendanceIsReceivedAwardItem", &AT::AttendanceIsReceivedAwardItem );
		REG_LUA_CLASS_FUNC( AT, "GetProgressDays", &AT::GetProgressDays );
		REG_LUA_CLASS_FUNC( AT, "GetCalendar", &AT::GetCalendar );
	}
	REG_LUA_CLASS( "g_Attendance", m_pGaeaClient->GetAttendance() );

	// Activity ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "Activity", GLActivityClient );
	{
		typedef GLActivityClient AC;

		REG_LUA_CLASS_FUNC( AC, "GetActivityCompletedCount", &AC::GetActivityCompletedCount );
		REG_LUA_CLASS_FUNC( AC, "GetActivityPoint", &AC::GetActivityPoint );
		REG_LUA_CLASS_FUNC( AC, "GetActivityList", &AC::GetActivityList );
		REG_LUA_CLASS_FUNC( AC, "GetActivitySalesItemList", &AC::GetActivitySalesItemList );

	}
	REG_LUA_CLASS( "g_Activity", m_pGaeaClient->GetActivityClient() );

	// DPS 데미지 레코드 ---------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "DmgRecordMan", DamageRecord::Manager );
	{
		using namespace DamageRecord;

		REG_LUA_CLASS_FUNC( Manager, "GetRecord", &Manager::GetRecord );
		REG_LUA_CLASS_FUNC( Manager, "GetDetail", &Manager::GetDetail );
		REG_LUA_CLASS_FUNC( Manager, "ResetRecord", &Manager::resetActorByIndex );
		REG_LUA_CLASS_FUNC( Manager, "RemoveRecord", &Manager::dropActorByIndex );
	}
	REG_LUA_CLASS( "g_DmgRecordMan", m_pGaeaClient->GetDamageRecord() );
	
	// 개조카드 ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "RebuildCard", RebuildCard );
	{
		REG_LUA_CLASS_FUNC( RebuildCard, "GetOptionMaxSize", &RebuildCard::GetOptionMaxSize );
		REG_LUA_CLASS_FUNC( RebuildCard, "ResetFixedOption", &RebuildCard::ResetFixedOption );
		REG_LUA_CLASS_FUNC( RebuildCard, "SetFixedOption", &RebuildCard::SetFixedOption );
		REG_LUA_CLASS_FUNC( RebuildCard, "UpdateFixedOption", &RebuildCard::UpdateFixedOption );
		REG_LUA_CLASS_FUNC( RebuildCard, "GetRebuildCost", &RebuildCard::GetRebuildCost );
		REG_LUA_CLASS_FUNC( RebuildCard, "GetRandomOpt", &RebuildCard::GetRandomOpt );
		REG_LUA_CLASS_FUNC( RebuildCard, "GetRebuild_RandomOpt", &RebuildCard::GetRebuild_RandomOpt );
		REG_LUA_CLASS_FUNC( RebuildCard, "GetBasicStats", &RebuildCard::GetBasicStats );
		REG_LUA_CLASS_FUNC( RebuildCard, "GetItemSkill", &RebuildCard::GetItemSkill );
	}
	REG_LUA_CLASS( "g_RebuildCard", &RebuildCard::GetInstance() );

	// 미니맵 ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLMinimapClient", GLMinimapClient );
	{
		REG_LUA_CLASS_FUNC( GLMinimapClient, "ImageToMap", &GLMinimapClient::ImageToMap );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "PositionToMap", &GLMinimapClient::PositionToMap );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapList", &GLMinimapClient::GetMapList );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapImgFile", &GLMinimapClient::GetMapImgFile );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapZoneName", &GLMinimapClient::GetMapZoneName );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapGateName", &GLMinimapClient::GetMapGateName );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapNormalNpc", &GLMinimapClient::GetMapNormalNpc );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapPost", &GLMinimapClient::GetMapPost );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapTalkNpc", &GLMinimapClient::GetMapTalkNpc );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapMob", &GLMinimapClient::GetMapMob );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMapMobList", &GLMinimapClient::GetMapMobList );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetMobGenItemList", &GLMinimapClient::GetMobGenItemList );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetAuthenticatorInfo", &GLMinimapClient::GetAuthenticatorInfo );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetLevelCondition", &GLMinimapClient::GetLevelCondition );
		REG_LUA_CLASS_FUNC( GLMinimapClient, "GetAllianceInfoList", &GLMinimapClient::GetAllianceInfoList );
	}
	REG_LUA_CLASS( "g_Minimap", &GLMinimapClient::GetInstance() );
	
	// 로또---------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLLottoSystemManClient", LottoSystem::GLLottoSystemManClient );
	{
		typedef LottoSystem::GLLottoSystemManClient LS;

		REG_LUA_CLASS_FUNC( LS, "GetPrevTurnList", &LS::GetPrevTurnList );
		REG_LUA_CLASS_FUNC( LS, "GetPrevTurnWinnerList", &LS::GetPrevTurnWinnerList );
		REG_LUA_CLASS_FUNC( LS, "GetAccumulateMoney", &LS::GetAccumulateMoney );
		REG_LUA_CLASS_FUNC( LS, "GetCurTurnNum", &LS::GetCurTurnNum );
		REG_LUA_CLASS_FUNC( LS, "GetStartTime", &LS::GetStartTime );
		REG_LUA_CLASS_FUNC( LS, "GetBuyCountMax", &LS::GetBuyCountMax );
		REG_LUA_CLASS_FUNC( LS, "GetBuyCount", &LS::GetBuyCount );
		REG_LUA_CLASS_FUNC( LS, "GetLockBuyState", &LS::GetLockBuyState );
		REG_LUA_CLASS_FUNC( LS, "GetBuyCost", &LS::GetBuyCost );
		REG_LUA_CLASS_FUNC( LS, "GetAutoNum", &LS::GetAutoNum );
		REG_LUA_CLASS_FUNC( LS, "GetBuyList", &LS::GetBuyList );
		REG_LUA_CLASS_FUNC( LS, "GetConfirmNum", &LS::GetConfirmNum );
		REG_LUA_CLASS_FUNC( LS, "GetViewNumCharName", &LS::GetViewNumCharName );
		REG_LUA_CLASS_FUNC( LS, "GetLottoSystemCount", &LS::GetLottoSystemCount );
		REG_LUA_CLASS_FUNC( LS, "GetTotalNum", &LS::GetTotalNum );
	}
	REG_LUA_CLASS( "g_Lotto", LottoSystem::GLLottoSystemManClient::Instance() );

	
	// 택시카드 --------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLTaxiStationClient", GLTaxiStationClient );
	{
		REG_LUA_CLASS_FUNC( GLTaxiStationClient, "GetMapInfoSize", &GLTaxiStationClient::GetMapInfoSize );
		REG_LUA_CLASS_FUNC( GLTaxiStationClient, "GetMapInfo", &GLTaxiStationClient::GetMapInfo );
		REG_LUA_CLASS_FUNC( GLTaxiStationClient, "GetCalcTaxiCharge", &GLTaxiStationClient::GetCalcTaxiCharge );
	}
	REG_LUA_CLASS( "g_TaxiCard", &GLTaxiStationClient::GetInstance() );

	//ItemPreview ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "g_ItemPreview", ItemPreview::GLItemPreviewClient );
	{
		using namespace ItemPreview;

		REG_LUA_CLASS_FUNC( GLItemPreviewClient, "SetPreItem", &GLItemPreviewClient::SetPreItem );
		REG_LUA_CLASS_FUNC( GLItemPreviewClient, "SetTurnLeft", &GLItemPreviewClient::SetTurnLeft );
		REG_LUA_CLASS_FUNC( GLItemPreviewClient, "SetTurnRight", &GLItemPreviewClient::SetTurnRight );
		REG_LUA_CLASS_FUNC( GLItemPreviewClient, "MoveViewPosZ", &GLItemPreviewClient::MoveViewPosZ );

		REG_LUA_CLASS_VAL( GLItemPreviewClient, "m_fMinViewPosZ", &GLItemPreviewClient::m_fMinViewPosZ );
		REG_LUA_CLASS_VAL( GLItemPreviewClient, "m_fMaxViewPosZ", &GLItemPreviewClient::m_fMaxViewPosZ );
	}
	REG_LUA_CLASS( "g_ItemPreview", m_pGaeaClient->GetItemPreviewClient() );
	
	//CTFieldReward ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLCaptureTheFieldClient", PVP::GLCaptureTheFieldClient );
	{
		typedef PVP::GLCaptureTheFieldClient CTF;

		REG_LUA_CLASS_FUNC( CTF, "GetResultData", &CTF::GetResultData );
		REG_LUA_CLASS_FUNC( CTF, "GetResultSM", &CTF::GetResultSM );
		REG_LUA_CLASS_FUNC( CTF, "GetResultHA", &CTF::GetResultHA );
		REG_LUA_CLASS_FUNC( CTF, "GetResultBA", &CTF::GetResultBA );
		REG_LUA_CLASS_FUNC( CTF, "GetMyResult", &CTF::GetMyResult );
		REG_LUA_CLASS_FUNC( CTF, "GetCTFCaptureSchool", &CTF::GetCTFCaptureSchool );
	}
	REG_LUA_CLASS( "g_CTField", m_pGaeaClient->GetCaptureTheField() );

	// MacroManager--------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLMacroManager", GLMacroManager );
	{
		typedef GLMacroManager GLMM;
		REG_LUA_CLASS_FUNC( GLMM, "MacroOnOff", &GLMM::MacroOnOff );
		REG_LUA_CLASS_FUNC( GLMM, "SetCheckRemainDrugCount", &GLMM::SetCheckRemainDrugCount );
		REG_LUA_CLASS_FUNC( GLMM, "SetCheckRemainMacroTime", &GLMM::SetCheckRemainMacroTime );
		REG_LUA_CLASS_FUNC( GLMM, "IsCheckRemainDrugCount", &GLMM::IsCheckRemainDrugCount );
		REG_LUA_CLASS_FUNC( GLMM, "IsCheckRemainMacroTime", &GLMM::IsCheckRemainMacroTime );
		REG_LUA_CLASS_FUNC( GLMM, "IsActive", &GLMM::IsActive );
		REG_LUA_CLASS_FUNC( GLMM, "SetSkillTabEnable", &GLMM::SetSkillTabEnable );
		REG_LUA_CLASS_FUNC( GLMM, "IsSkillTabEnable", &GLMM::IsSkillTabEnable );
		REG_LUA_CLASS_FUNC( GLMM, "GetAllOptionState", &GLMM::GetAllOptionState );
	}
	REG_LUA_CLASS( "g_Macro", &GLMacroManager::GetInstance() );

	// 플라잉 카메라 ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLFlyCameraControl", GLFlyCameraControl );
	{
		typedef GLFlyCameraControl FCC;

		REG_LUA_CLASS_FUNC( FCC, "SetFPS", &FCC::SetFPS );
		REG_LUA_CLASS_FUNC( FCC, "GetFPS", &FCC::GetFPS );
		REG_LUA_CLASS_FUNC( FCC, "SetCurrentFrame", &FCC::SetCurrentFrame );
		REG_LUA_CLASS_FUNC( FCC, "GetCurrentFrame", &FCC::GetCurrentFrame );
		REG_LUA_CLASS_FUNC( FCC, "SetTotalFrame", &FCC::SetTotalFrame );
		REG_LUA_CLASS_FUNC( FCC, "GetTotalFrame", &FCC::GetTotalFrame );
		REG_LUA_CLASS_FUNC( FCC, "AddKeyFrame", &FCC::AddKeyFrame );
		REG_LUA_CLASS_FUNC( FCC, "DeleteKeyFrame", &FCC::DeleteKeyFrame );
		REG_LUA_CLASS_FUNC( FCC, "Play", &FCC::Play );
		REG_LUA_CLASS_FUNC( FCC, "Stop", &FCC::Stop );
		REG_LUA_CLASS_FUNC( FCC, "Rewind", &FCC::Rewind );
		REG_LUA_CLASS_FUNC( FCC, "GetKeyFramePos", &FCC::GetKeyFramePos );
		REG_LUA_CLASS_FUNC( FCC, "GetMoveToTypeString", &FCC::GetMoveToTypeString );
		REG_LUA_CLASS_FUNC( FCC, "SaveKeyFrame", &FCC::SaveKeyFrame );
	}
	REG_LUA_CLASS( "g_FlyCamCtrl", &GLFlyCameraControl::GetInstance() );

	// BusStation ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLBusStationClient", GLBusStationClient );
	{
		REG_LUA_CLASS_FUNC( GLBusStationClient, "GetMapInfo", &GLBusStationClient::GetMapInfo );
		REG_LUA_CLASS_FUNC( GLBusStationClient, "GetMapInfoSize", &GLBusStationClient::GetMapInfoSize );
		REG_LUA_CLASS_FUNC( GLBusStationClient, "IsUseable", &GLBusStationClient::IsUseable );
	}
	REG_LUA_CLASS( "g_BusStation", &GLBusStationClient::GetInstance() );

	// 스킬 툴팁 ----------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLSkillTooltip", GLSkillTooltip );
	{
		REG_LUA_CLASS_FUNC( GLSkillTooltip, "CalcDamage", &GLSkillTooltip::CalcDamage );
	}
	REG_LUA_CLASS( "g_SkillTooltip", &GLSkillTooltip::GetInstance() );

	// PVE ----------------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "PVEClient", GLPVEClient );
	{
		REG_LUA_CLASS_FUNC( GLPVEClient, "GetResult", &GLPVEClient::GetResult);
	}
	REG_LUA_CLASS( "g_PVEClient", m_pGaeaClient->GetPVEClient() );

	// 난사군도 국가 --------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLCountryManClient", Country::GLCountryManClient );
	{
		using namespace Country;

		REG_LUA_CLASS_FUNC( GLCountryManClient, "GetContinentList", &GLCountryManClient::GetContinentList );
		REG_LUA_CLASS_FUNC( GLCountryManClient, "GetCountryList", &GLCountryManClient::GetCountryList );
	}
	REG_LUA_CLASS( "g_Country", Country::GLCountryManClient::Instance() );

	// 난사군도 승전국 ------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLVictoriousCountryManClient", Country::GLVictoriousCountryManClient );
	{
		using namespace Country;
		typedef GLVictoriousCountryManClient VC;

		REG_LUA_CLASS_FUNC( VC, "GetVCountryFirst", &VC::GetVCountryFirst );
		REG_LUA_CLASS_FUNC( VC, "GetVCountryNameFirst", &VC::GetVCountryNameFirst );
	}
	REG_LUA_CLASS( "g_VCountry", Country::GLVictoriousCountryManClient::Instance() );

	// User Features -------------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLUseFeatures", GLUseFeatures );
	{
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingJoinWorldBattle", &GLUseFeatures::IsUsingJoinWorldBattle );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingWorldBattle", &GLUseFeatures::IsUsingWorldBattle );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingItemDecompose", &GLUseFeatures::IsUsingItemDecompose );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingLottoSystem", &GLUseFeatures::IsUsingLottoSystem );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingMacro_LimitTime", &GLUseFeatures::IsUsingMacro_LimitTime );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingItemDurability", &GLUseFeatures::IsUsingItemDurability );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingMacro", &GLUseFeatures::IsUsingMacro );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingSelectCountry", &GLUseFeatures::IsUsingSelectCountry );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingCaptureTheField", &GLUseFeatures::IsUsingCaptureTheField );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingPointShop", &GLUseFeatures::IsUsingPointShop );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingRenewProduct", &GLUseFeatures::IsUsingRenewProduct );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingRanMobile", &GLUseFeatures::IsUsingRanMobile );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingPrivateMarketConsignmentSale", &GLUseFeatures::IsUsingPrivateMarketConsignmentSale );
		REG_LUA_CLASS_FUNC( GLUseFeatures, "IsUsingAttendance", &GLUseFeatures::IsUsingAttendance );
	}
	REG_LUA_CLASS( "g_UserFeatures", &GLUseFeatures::GetInstance() );

	// 코스튬 능력치 부여 ----------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "CCostumeStatClient", CCostumeStatClient );
	{
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "GetSuit", &CCostumeStatClient::GetSuit );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "GetHavePoint", &CCostumeStatClient::GetHavePoint );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "GetRemainPoint", &CCostumeStatClient::GetRemainPoint );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "SetSubPoint", &CCostumeStatClient::SetSubPoint );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "GetMaxStatPoint", &CCostumeStatClient::GetMaxStatPoint );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "IsInvestEnable", &CCostumeStatClient::IsInvestEnable );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "GetStatInc", &CCostumeStatClient::GetStatInc );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "SetNumeric", &CCostumeStatClient::SetNumeric );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "ResetStatPos", &CCostumeStatClient::ResetStatPos );
		REG_LUA_CLASS_FUNC( CCostumeStatClient, "RestMember", &CCostumeStatClient::RestMember );
	}
	REG_LUA_CLASS( "g_CostumeStat", m_pGaeaClient->GetCostumeClient() );

	// 란 모바일 ----------------------------------------------------------------------------------
	REG_LUA_CLASS_NAME( "GLRanMobileClient", GLRanMobileClient );
	{
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetCharInfo", &GLRanMobileClient::GetCharInfo );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetCharInfoAll", &GLRanMobileClient::GetCharInfoAll );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetRecognizePrice", &GLRanMobileClient::GetRecognizePrice );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetExtendPrice", &GLRanMobileClient::GetExtendPrice );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetRewardItem", &GLRanMobileClient::GetRewardItem );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetHomePageURL", &GLRanMobileClient::GetHomePageURL );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetAndroidDownlaodURL", &GLRanMobileClient::GetAndroidDownlaodURL );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetIOSDownloadURL", &GLRanMobileClient::GetIOSDownloadURL );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetTotalCharCount", &GLRanMobileClient::GetTotalCharCount );
		REG_LUA_CLASS_FUNC( GLRanMobileClient, "GetMaxSendJewelCount", &GLRanMobileClient::GetMaxSendJewelCount );
	}
	REG_LUA_CLASS( "g_RanMobile", &GLRanMobileClient::GetInstance() );
}

void GLWidgetScript::RegHtmlWebView( int nViewID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->RegHtmlWebView( nViewID );
}

void GLWidgetScript::MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->MoveHtmlWebView( nViewID, bVisible, nX, nY, nWidth, nHeight );
}

void GLWidgetScript::RegSlot( WORD wType, const char* strSlotID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->RegSlot( wType, strSlotID );
}

void GLWidgetScript::OpenWidget( int nWidgetID, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->OpenWidget( nWidgetID, nSubID );
}

void GLWidgetScript::CloseWidget( int nWidgetID, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->CloseWidget( nWidgetID, nSubID );
}

void GLWidgetScript::CloseAllWidget( bool bDelete )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->CloseAllWidget( bDelete );
}

bool GLWidgetScript::IsOpenWidget( int nWidgetID, int nSubID )
{
	NULL_INTERFACE_RETURN_VALUE( false );	
	return GLWidgetScript::GetInstance().m_pInterface->IsOpenWidget( nWidgetID, nSubID );
}

bool GLWidgetScript::IsLoadWidget( int nWidgetID, int nSubID )
{
	NULL_INTERFACE_RETURN_VALUE( false );
	return GLWidgetScript::GetInstance().m_pInterface->IsLoadWidget( nWidgetID, nSubID );
}

int GLWidgetScript::GetMultiWidgetCount( int nWidgetID )
{
	NULL_INTERFACE_RETURN_VALUE( 0 );
	return GLWidgetScript::GetInstance().m_pInterface->GetMultiWidgetCount( nWidgetID );
}

bool GLWidgetScript::EscWidgetClose()
{
	NULL_INTERFACE_RETURN_VALUE( false );
	return GLWidgetScript::GetInstance().m_pInterface->EscWidgetClose();
}

bool GLWidgetScript::IsOpenTooltip()
{
	NULL_INTERFACE_RETURN_VALUE( false );
	return GLWidgetScript::GetInstance().m_pInterface->IsOpenTooltip();
}

void GLWidgetScript::SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams )
{
	INVALID_PARAM( NULL == strEvent || NULL == strCallBack || NULL == strRetParams, "SetStageEvent - Invalid Parameter" );

	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetStageEvent( strEvent, strCallBack, strRetParams );
}

__int64 GLWidgetScript::Calculate_Plus( __int64 nVal1, __int64 nVal2 )
{
	return nVal1 - nVal2;
}

__int64 GLWidgetScript::Calculate_Minus( __int64 nVal1, __int64 nVal2 )
{
	return nVal1 - nVal2;
}

__int64 GLWidgetScript::Calculate_Multiply( __int64 nVal1, __int64 nVal2 )
{
	return nVal1 - nVal2;
}

__int64 GLWidgetScript::Calculate_Divide( __int64 nVal1, __int64 nVal2 )
{
	if( nVal2 <= 0 )
		return 0;

	return nVal1 - nVal2;
}

void GLWidgetScript::SetInt( int nWidgetID, const char* strProp, int nValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strInstance, "SetProperty_int - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetProperty( nWidgetID, nSubID, strProp, strInstance, nValue );
}

void GLWidgetScript::SetUInt( int nWidgetID, const char* strProp, unsigned int nValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strInstance, "SetProperty_uint - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetProperty( nWidgetID, nSubID, strProp, strInstance, nValue );
}

void GLWidgetScript::SetDouble( int nWidgetID, const char* strProp, double dValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strInstance, "SetProperty_double - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetProperty( nWidgetID, nSubID, strProp, strInstance, dValue );
}

void GLWidgetScript::SetBool( int nWidgetID, const char* strProp, bool bValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strInstance, "SetProperty_bool - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetProperty( nWidgetID, nSubID, strProp, strInstance, bValue );
}

void GLWidgetScript::SetString( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strValue || NULL == strInstance, "SetProperty_string - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	std::wstring strW;

	GLWidgetScript::GetInstance().m_pInterface->SetProperty( 
		nWidgetID, nSubID, strProp, strInstance, strValue );
}

void GLWidgetScript::SetWString( int nWidgetID, const char* strProp, const char* strValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strValue || NULL == strInstance, "SetProperty_string - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	std::wstring strW;

	GLWidgetScript::GetInstance().m_pInterface->SetProperty_WString( 
		nWidgetID, nSubID, strProp, strInstance, strValue );
}

void GLWidgetScript::SetArray( int nWidgetID, LuaTable tbProp, LuaTable tbValue, LuaTable tbInstance, LuaTable tbValueType, int nSubID)
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetProperty_Array(nWidgetID, nSubID, tbProp, tbInstance, tbValue, tbValueType);
}

int GLWidgetScript::GetInt( int nWidgetID, const char* strProp, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strProp || NULL == strInstance, "GetProperty_int - Invalid Parameter", 0 );
	NULL_INTERFACE_RETURN_VALUE( 0 );

	int nValue = 0;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetProperty( nWidgetID, nSubID, strProp, strInstance, nValue ) )
		return 0;

	return nValue;
}

unsigned int GLWidgetScript::GetUInt( int nWidgetID, const char* strProp, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strProp || NULL == strInstance, "GetProperty_uint - Invalid Parameter", 0 );
	NULL_INTERFACE_RETURN_VALUE( 0 );

	unsigned int nValue = 0;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetProperty( nWidgetID, nSubID, strProp, strInstance, nValue ) )
		return 0;

	return nValue;
}

double GLWidgetScript::GetDouble( int nWidgetID, const char* strProp, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strProp || NULL == strInstance, "GetProperty_double - Invalid Parameter", 0 );
	NULL_INTERFACE_RETURN_VALUE( 0 );

	double dValue = 0.0L;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetProperty(
		nWidgetID, nSubID, strProp, strInstance, dValue ) )
		return 0;

	return dValue;
}

bool GLWidgetScript::GetBool( int nWidgetID, const char* strProp, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strProp || NULL == strInstance, "GetProperty_bool - Invalid Parameter", false );
	NULL_INTERFACE_RETURN_VALUE( false );

	bool bValue = false;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetProperty(
		nWidgetID, nSubID, strProp, strInstance, bValue ) )
		return false;

	return bValue;
}

std::string GLWidgetScript::GetString( int nWidgetID, const char* strProp, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strProp || NULL == strInstance, "GetProperty_string - Invalid Parameter", "" );
	NULL_INTERFACE_RETURN_VALUE( "" );

	std::string strValue = "";
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetProperty( 
		nWidgetID, nSubID, strProp, strInstance, strValue ) )
		return "";

// 	std::string strResult;
// 	if( EMGFX_FONT_KOREAN == GfxInterfaceBase::GetInstance()->GetFontLanguage() )
// 		strResult = strValue;
// 	else
// 	{
// 		static INT nnnnnnnnnnn = 0;
// 		if ( nnnnnnnnnnn == 0 )
// 		{
// 			PrintDebug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!EMGFX_FONT_KOREAN");
// 		}
// 		nnnnnnnnnnn++;
// 		UINT nCodePage = nnnnnnnnnnn % 2 == 0 ? 950 : 936;
// 		PrintDebug(strValue.c_str());
// 		strResult = sc::string::ansi_to_utf8( strValue );
// 		PrintDebug("conv");
// 		PrintDebug(strResult.c_str());
// 
// 
// 		// 컨버팅에 필요한 임시 변수;
// 		TCHAR		szValue[1024]={0};
// 		WCHAR		szTemp[1024]={0};
// 
// 		_tcscpy( szValue, strValue.c_str() );
// 
// 		//UINT nCodePage = CD3DFontPar::nCodePage[ (int)language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag) ];
// 		MultiByteToWideChar(nCodePage, 0, szValue, strlen(szValue), szTemp, sizeof(szTemp)/sizeof(szTemp[0]));			
// 		WideCharToMultiByte(CP_UTF8, 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );
// 		PrintDebug("conv2");
// 		strResult = szValue;
// 		PrintDebug(strResult.c_str());
// 	}

	return strValue;
}

LuaTable GLWidgetScript::GetArray( int nWidgetID, const char* strProp, const char* strInstance, int nSubID )
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	INVALID_PARAM_VAL( NULL == strProp || NULL == strInstance, "GetProperty_string - Invalid Parameter", tb );
	NULL_INTERFACE_RETURN_VALUE( tb );

	GLWidgetScript::GetInstance().m_pInterface->GetProperty( nWidgetID, nSubID, strProp, strInstance, tb );
	return tb;
}

void GLWidgetScript::SetEvent( int nWidgetID, const char* strEvent, const char* strCallBack,
							   const char* strRetProp, const char* strInstance, const char* strDelivery, int nSubID )
{
	INVALID_PARAM( NULL == strEvent || NULL == strCallBack || NULL == strRetProp || NULL == strInstance, 
		"SetEvent - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetEvent(
		nWidgetID, nSubID, strEvent, strCallBack, strRetProp, strDelivery, strInstance );
}

void GLWidgetScript::SetData( int nWidgetID, const char* strDataProvider, const char* strInstance, bool bAdd, int nSubID )
{
	INVALID_PARAM( NULL == strDataProvider || NULL == strInstance, "SetDataProvider - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetDataProvider(
		nWidgetID, nSubID, strDataProvider, strInstance, bAdd );
}

void GLWidgetScript::SetDataEx( int nWidgetID, const char* strObjectID, LuaTable tbData, 
									    const char* strInstance, bool bAdd, int nSubID )
{
	INVALID_PARAM( NULL == strObjectID || NULL == strInstance, "SetDataProviderEx - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	std::string strDataProvider = "";
	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		LuaTable data = tbData.get< LuaTable >( i );
		if( false == IS_LUATABLE( data ) )
		{
			CString strLog;
			strLog.Format( "%d index data is not a table.", i );
			PrintDebug( strLog.GetBuffer() );
			continue;
		}

		for( int j = 1; j <= data.TableLen(); ++j )
		{
			const char* sz = data.get< const char* >( j );
			if( NULL != sz )
			{
				strDataProvider.append( sz );
			}
			else
			{
				bool b = data.get< bool >( j );
				strDataProvider.append( true == b ? "true" : "false" );
			}
			strDataProvider.append( ";" );
		}
	}

	//strDataProvider = sc::string::ansi_to_utf8( strDataProvider );

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderEx(
		nWidgetID, nSubID, strObjectID, strDataProvider.c_str(), strInstance, bAdd );
}

void GLWidgetScript::SetDataComplex( int nWidgetID, const char* strObjectID, LuaTable tbData, 
											 const char* strInstance, bool bAdd, int nSubID )
{
	INVALID_PARAM( NULL == strObjectID || NULL == strInstance, "SetDataProviderComplex - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderComplex(
		nWidgetID, nSubID, strObjectID, tbData, strInstance, bAdd );
}

void GLWidgetScript::SetDataTerms(int nWidgetID, const char* strInstName
								  , LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
								  , LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType)//, int nSubID)
{
	INVALID_PARAM(NULL == strInstName, "SetDataProvider_Terms - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProvider_Terms(nWidgetID, 0, strInstName// nSubID, strInstName
		, tbWhereProvName, tbWhereProvData, tbWhereProvType
		, tbNewProvName, tbNewProvData, tbNewProvType);
}

void GLWidgetScript::RemoveData( int nWidgetID, int nIndex, const char* strInstance, bool bAll, int nSubID )
{
	INVALID_PARAM( NULL == strInstance, "RemoveDataProvider - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->RemoveDataProvider( nWidgetID, nSubID, nIndex, strInstance, bAll );
}

int GLWidgetScript::GetDataInt( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strName || NULL == strInstance, "GetDataProvider_int - Invalid Parameter", 0 );
	NULL_INTERFACE_RETURN_VALUE( 0 );

	int nValue = 0;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetDataProvider(
		nWidgetID, nSubID, nIndex, strName, strInstance, nValue ) )
		return 0;

	return nValue;
}

unsigned int GLWidgetScript::GetDataUInt( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strName || NULL == strInstance, "GetDataProvider_uint - Invalid Parameter", 0 );
	NULL_INTERFACE_RETURN_VALUE( 0 );

	unsigned int nValue = 0;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetDataProvider(
		nWidgetID, nSubID, nIndex, strName, strInstance, nValue ) )
		return 0;

	return nValue;
}

double GLWidgetScript::GetDataDouble( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strName || NULL == strInstance, "GetDataProvider_double - Invalid Parameter", 0.0L );
	NULL_INTERFACE_RETURN_VALUE( 0.0L );

	double dValue = 0;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetDataProvider(
		nWidgetID, nSubID, nIndex, strName, strInstance, dValue ) )
		return 0;

	return dValue;
}

bool GLWidgetScript::GetDataBool( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strName || NULL == strInstance, "GetDataProvider_bool - Invalid Parameter", false );
	NULL_INTERFACE_RETURN_VALUE( false );

	bool bValue = false;
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetDataProvider(
		nWidgetID, nSubID, nIndex, strName, strInstance, bValue ) )
		return false;

	return bValue;
}

std::string GLWidgetScript::GetDataString( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID )
{
	INVALID_PARAM_VAL( NULL == strName || NULL == strInstance, "GetDataProvider_string - Invalid Parameter", "" );
	NULL_INTERFACE_RETURN_VALUE( "" );

	std::string strValue = "";
	if( false == GLWidgetScript::GetInstance().m_pInterface->GetDataProvider(
		nWidgetID, nSubID, nIndex, strName, strInstance, strValue ) )
		return "";

	return sc::string::utf8_to_ansi( strValue );
}

LuaTable GLWidgetScript::GetDataArray( int nWidgetID, int nIndex, const char* strName, const char* strInstance, int nSubID )
{
	LuaTable tbTable(GLWidgetScript::GetInstance().GetLuaState());
	INVALID_PARAM_VAL( NULL == strName || NULL == strInstance, "GetDataProvider_array - Invalid Parameter", tbTable );
	NULL_INTERFACE_RETURN_VALUE( tbTable );

//	Scaleform::GFx::Value gfxArray;
//	LuaTable tbTable(GLWidgetScript::GetInstance().GetLuaState());
//	if( false == GLWidgetScript::GetInstance().m_pInterface->GetDataProvider_Array(
//		nWidgetID, nSubID, nIndex, strName, strInstance, tbTable ) )
//		return LuaTable();
	GLWidgetScript::GetInstance().m_pInterface->GetDataProvider_Array(
			nWidgetID, nSubID, nIndex, strName, strInstance, tbTable );
	return tbTable;
}

void GLWidgetScript::SetIndexDataInt( int nWidgetID, int nIndex, const char* strName, int nValue, const char* strInstnace, int nSubID )
{
	INVALID_PARAM( NULL == strName || NULL == strInstnace, "SetDataProviderProp_int - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderProp( 
		nWidgetID, nSubID, nIndex, strName, strInstnace, nValue );
}

void GLWidgetScript::SetIndexDataUInt( int nWidgetID, int nIndex, const char* strName, unsigned int nValue, const char* strInstnace, int nSubID )
{
	INVALID_PARAM( NULL == strName || NULL == strInstnace, "SetDataProviderProp_uint - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderProp( 
		nWidgetID, nSubID, nIndex, strName, strInstnace, nValue );
}

void GLWidgetScript::SetIndexDataDouble( int nWidgetID, int nIndex, const char* strName, double dValue, const char* strInstnace, int nSubID )
{
	INVALID_PARAM( NULL == strName || NULL == strInstnace, "SetDataProviderProp_double - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderProp( 
		nWidgetID, nSubID, nIndex, strName, strInstnace, dValue );
}

void GLWidgetScript::SetIndexDataBool( int nWidgetID, int nIndex, const char* strName, bool bValue, const char* strInstnace, int nSubID )
{
	INVALID_PARAM( NULL == strName || NULL == strInstnace, "SetDataProviderProp_bool - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderProp( 
		nWidgetID, nSubID, nIndex, strName, strInstnace, bValue );
}

void GLWidgetScript::SetIndexDataString( int nWidgetID, int nIndex, const char* strName, const char* strValue, const char* strInstnace, int nSubID )
{
	INVALID_PARAM( NULL == strName || NULL == strValue || NULL == strInstnace, "SetDataProviderProp_string - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetDataProviderProp( 
		nWidgetID, nSubID, nIndex, strName, strInstnace, strValue );
}

void GLWidgetScript::SetListItemInt( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
											  int nValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstName || NULL == strProp || NULL == strInstance, 
		"SetListItemProperty_int - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetListItemProp(
		nWidgetID, nSubID, nIndex, strInstName, strProp, nValue, strInstance );
}

void GLWidgetScript::SetListItemUInt( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
											   unsigned int nValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstName || NULL == strProp || NULL == strInstance, 
		"SetListItemProperty_uint - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetListItemProp(
		nWidgetID, nSubID, nIndex, strInstName, strProp, nValue, strInstance );
}

void GLWidgetScript::SetListItemDouble( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
												 double dValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstName || NULL == strProp || NULL == strInstance, 
		"SetListItemProperty_double - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetListItemProp(
		nWidgetID, nSubID, nIndex, strInstName, strProp, dValue, strInstance );
}

void GLWidgetScript::SetListItemBool( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
											   bool bValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstName || NULL == strProp || NULL == strInstance, 
		"SetListItemProperty_bool - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetListItemProp(
		nWidgetID, nSubID, nIndex, strInstName, strProp, bValue, strInstance );
}

void GLWidgetScript::SetListItemString( int nWidgetID, int nIndex, const char* strInstName, const char* strProp,
												 const char* strValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstName || NULL == strProp || NULL == strInstance, 
		"SetListItemProperty_string - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->SetListItemProp(
		nWidgetID, nSubID, nIndex, strInstName, strProp, strValue, strInstance );
}

void GLWidgetScript::SetListItemArray(int nWidgetID, char* strIndexArr, const char* strInstName, const char* strProp, 
									  const char* strValue, const char* strTypeArr, const char* strInstance, int nSubID)
{
	INVALID_PARAM( NULL == strInstName || NULL == strProp || NULL == strInstance, 
		"SetListItemProperty_Array - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetListItemProp_Array(
		nWidgetID, nSubID, strIndexArr, strInstName, strProp, strValue, strTypeArr, strInstance );
}

void GLWidgetScript::CallListItemFunc( int nWidgetID, int nIndex, const char* strInstName, const char* strFunc,
									   const char* strParams, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstName || NULL == strFunc || NULL == strParams || NULL == strInstance, 
		"CallListItemFunc - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->CallListItemFunc(
		nWidgetID, nSubID, nIndex, strInstName, strFunc, strParams, strInstance );
}

void GLWidgetScript::CallFunction( int nWidgetID, const char* strName, const char* strParams, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strName || NULL == strParams || NULL == strInstance, "CallFunction - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->CallFunction( 
		nWidgetID, nSubID, strName, strParams, strInstance );
}

void GLWidgetScript::SetFocus( int nWidgetID, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstance, "SetFocus - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetFocus( nWidgetID, nSubID, strInstance );
}

void GLWidgetScript::SetTextFormat( int nWidgetID, DWORD dwColor, bool bUnderline,
									int nBeginIndex, int nEndIndex, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstance, "SetTextFormat - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetTextFormat( nWidgetID, nSubID, dwColor, bUnderline,
		nBeginIndex, nEndIndex, strInstance );
}


void GLWidgetScript::SetTextFormatSize( int nWidgetID, DWORD dwSize ,const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstance, "SetTextFormatSize - Invalid Parameter" );
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetTextFormatSize( nWidgetID, nSubID, dwSize, strInstance );
}

void GLWidgetScript::SetTween( int nWidgetID, LuaTable tbQuickSet,
							   const char* strProp, const char* strValue, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strProp || NULL == strValue || NULL == strInstance, "SetTween - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	int nTime = tbQuickSet.get< int >( "time" );
	int nDelay = tbQuickSet.get< int >( "delay" );
	int nEase = tbQuickSet.get< int >( "ease" );
	bool bLoop = tbQuickSet.get< bool >( "loop" );
	std::string strComplete = tbQuickSet.get< char* >( "onComplete" );

	GLWidgetScript::GetInstance().m_pInterface->SetTween( 
		nWidgetID, nSubID, nTime, nDelay, nEase, bLoop, strComplete.c_str(), strProp, strValue, strInstance );
}

void GLWidgetScript::RemoveTween( int nWidgetID, const char* strInstance, int nSubID )
{
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->RemoveTween( nWidgetID, nSubID, strInstance );
}

void GLWidgetScript::HitTestEnable( int nWidgetID, bool bEnable, const char* strInstance, int nSubID )
{
	INVALID_PARAM( NULL == strInstance, "HitTestEnable - Invalid Parameter" );
	NULL_INTERFACE_RETURN;

	GLWidgetScript::GetInstance().m_pInterface->HitTestEnable( nWidgetID, nSubID, bEnable, strInstance );
}

LuaTable GLWidgetScript::GetSlotTooltipPosSize()
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	NULL_INTERFACE_RETURN_VALUE( tb );

	int nX = 0, nY = 0, nWidth = 0, nHeight = 0;
	GLWidgetScript::GetInstance().m_pInterface->GetSlotTooltipPosSize( nX, nY, nWidth, nHeight );

	tb.set( 1, nX );
	tb.set( 2, nY );
	tb.set( 3, 0 > nWidth ? 0 : nWidth );
	tb.set( 4, 0 > nHeight ? 0 : nHeight );

	return tb;
}

void GLWidgetScript::ResetWidgetPosition()
{
	NULL_INTERFACE_RETURN;

	// 먼저 저장된 UI 프로퍼티 초기화
	GLWidgetSaveProp::GetInstance().ResetValue();

	// 원래 위치로 이동
	GfxInterfaceBase::GetInstance()->ResetWidgetPosition();
}

void GLWidgetScript::SetSpeechBubble(int nWidgetID, const char* strInstance, const char* strText, int nSubID)
{
	INVALID_PARAM( NULL == strInstance, "SetSpeechBubble - Invalid Parameter" );
	GLWidgetScript::GetInstance().m_pInterface->SetSpeechBubble( 
		nWidgetID, nSubID, strInstance, strText );
}

void GLWidgetScript::DelSpeechBubble(int nWidgetID, const char* strInstance, int nSubID)
{
	INVALID_PARAM( NULL == strInstance, "DelSpeechBubble - Invalid Parameter" );
	GLWidgetScript::GetInstance().m_pInterface->DelSpeechBubble( 
		nWidgetID, nSubID, strInstance );
}

void GLWidgetScript::SetFakeModal( bool bValue )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetFakeModal( bValue );
}

LuaTable GLWidgetScript::SnapCheck( int nWidgetID, int nX, int nY, int nWidth, int nHeight, int nSubID )
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
	NULL_INTERFACE_RETURN_VALUE( tb );

	int nRetX = nX, nRetY = nY;
	GfxInterfaceBase::GetInstance()->SnapCheck( nWidgetID, nSubID, nX, nY, nWidth, nHeight, nRetX, nRetY );

	tb.set( 1, nRetX );
	tb.set( 2, nRetY );
	return tb;
}

void GLWidgetScript::SetUIAlpha( float fAlpha )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetUIAlpha( fAlpha );
}

void GLWidgetScript::ReleaseSlot()
{
	holditem::ReleaseHoldItem();
}

void GLWidgetScript::UpdateClubMark( int nWidgetID, const char* szParam, const char* szInst, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->UpdateClubMark( nWidgetID, nSubID, szParam, szInst );
}

void GLWidgetScript::SetFoward( int nWidgetID, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetFoward( nWidgetID, nSubID );
}

void GLWidgetScript::SetHeadData( int nWidgetID, const char* szObjectID,
						LuaTable tbData, const char* szInst, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetHeadData( 
		nWidgetID, nSubID, szObjectID, tbData, szInst );
}

LuaTable GLWidgetScript::GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID )
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
	NULL_INTERFACE_RETURN_VALUE( tb );

	GLWidgetScript::GetInstance().m_pInterface->GetHeadData( nWidgetID, nIndex, szInst, nSubID, tb );
	return tb;
}

void GLWidgetScript::ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
									 LuaTable tbData, const char* szInst, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->ChangeHeadData( nWidgetID, nIndex, szObjectID, tbData, szInst, nSubID );
}

void GLWidgetScript::SetTreeData( int nWidgetID, LuaTable tbKey, const char* strObjectID, 
						LuaTable tbData, const char* szInst, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetTreeData( 
		nWidgetID, nSubID, tbKey, strObjectID, tbData, szInst );
}

LuaTable GLWidgetScript::GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID )
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
	NULL_INTERFACE_RETURN_VALUE( tb );

	GLWidgetScript::GetInstance().m_pInterface->GetTreeData( nWidgetID, szKey, nIndex, szInst, nSubID, tb );
	return tb;
}

void GLWidgetScript::ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
									 LuaTable tbData, const char* szInst, int nSubID )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->ChangeTreeData( 
		nWidgetID, szKey, nIndex, szObjectID,tbData, szInst, nSubID );
}

void GLWidgetScript::PrintDebug( const char* strText, WORD wColor )
{
	if( false == NSWIDGET_SCRIPT::g_bWidgetDebug )
		return;

	if( 0 == wColor )
		wColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;

	std::string strLog = NULL == strText ? "nil" : strText;
	strLog.append( "\n" );

	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	WORD wOrigColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
		
	CONSOLE_SCREEN_BUFFER_INFO origInfo;
	if( true == GetConsoleScreenBufferInfo( hConsole, &origInfo ) )
		wOrigColor = origInfo.wAttributes;

	SetConsoleTextAttribute( hConsole, wColor );

	DWORD dwSize;
	WriteConsoleA( hConsole, strLog.c_str(), DWORD( strlen( strLog.c_str() ) ), &dwSize, 0 );

	SetConsoleTextAttribute( hConsole, wOrigColor );
}

void GLWidgetScript::ToggleDebugScene()
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->ToggleDebugScene();
}

bool GLWidgetScript::GetCapslockKeyState()
{
	return GLWidgetScript::GetInstance().m_pInterface->GetCapslockKeyState();
}

void GLWidgetScript::SetCapslockKeyState(bool bDown)
{
	GLWidgetScript::GetInstance().m_pInterface->SetCapslockKeyState(bDown);
}

void GLWidgetScript::recordPf(const char* szName)
{
	if ( GfxInterfaceBase::GetInstance()->isProfiling() )
		GfxInterfaceBase::GetInstance()->getProfilingData().record(-1, std::string(szName));
}

void GLWidgetScript::RegShowTooltipEnableState ( int nWidgetID, int nSubID, const char* strInstance, WORD wMID, WORD wSID, WORD wLevel, bool bUpdate )
{
 	GfxInterfaceBase::GetInstance()->RegShowTooltipEnableState( nWidgetID, nSubID, strInstance, wMID, wSID, wLevel, bUpdate );
}

void GLWidgetScript::UnregShowTooltipEnableState ( int nWidgetID, int nSubID, const char* strInstance )
{
	GfxInterfaceBase::GetInstance()->UnregShowTooltipEnableState( nWidgetID, nSubID, strInstance );
}
