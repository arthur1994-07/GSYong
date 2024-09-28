#include "pch.h"
#include "GLWidgetScript.h"

#include <Imm.h>

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../enginelib/DeviceMan.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/GUInterface/UITextUtil.h"
#include "../../enginelib/DxSound/DxSoundMan.h"
#include "../../enginelib/DxSound/BgmSound.h"
#include "../../enginelib/DxSound/DxSoundLib.h"
#include "../../enginelib/DxCommon/d3dapp.h"
#include "../../enginelib/DxTools/DxViewPort.h"
#include "../../enginelib/DxTools/RENDERPARAM.h"
#include "../../RanGfxUI/GfxInterface.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../GLGaeaClient.h"
#include "../GfxInterfaceBase.h"
#include "../Stage/DxGlobalStage.h"
#include "../Tool/DxParamSet.h"

std::string GLWidgetScript::GetUITexturePath()
{
	return NSGFXFILE::g_strTexturePath;
}

std::string GLWidgetScript::GetMinimapTexturePath()
{
	std::string strPath = NSGFXFILE::g_strTexturePath;
	strPath.append( "\\mini\\" );

	return strPath;
}

LuaTable GLWidgetScript::GetResolutionList()
{
	LuaTable tbResolutionList( GLWidgetScript::GetInstance().GetLuaState() );

	CD3DApplication* const pD3DApp = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage()->GetD3DApp();
	if( NULL == pD3DApp )
		return tbResolutionList;

	D3DDeviceInfo* const pDeviceInfo = pD3DApp->GetCurDeviceInfo();
	if( NULL == pDeviceInfo )
		return tbResolutionList;

	for( DWORD i = 0; i < pDeviceInfo->dwNumModes; ++i )
	{
		LuaTable tbResolution( GLWidgetScript::GetInstance().GetLuaState() );

		tbResolution.set( "nWidth", pDeviceInfo->modes[ i ].Width );
		tbResolution.set( "nHeight", pDeviceInfo->modes[ i ].Height );

		DWORD dwFormat = 0;
		switch( pDeviceInfo->modes[ i ].Format )
		{
		case D3DFMT_R8G8B8 :
		case D3DFMT_A8R8G8B8 :
		case D3DFMT_X8R8G8B8 :
			dwFormat = 32;
			break;

		case D3DFMT_R5G6B5 :
		case D3DFMT_X1R5G5B5 :
		case D3DFMT_A1R5G5B5 :
			dwFormat = 16;
			break;
		}
		tbResolution.set( "nFormat", dwFormat );

		bool bRecommend = true;
		if( pDeviceInfo->modes[ i ].Width < RANPARAM::X_RESOLUTION ||
			pDeviceInfo->modes[ i ].Height < RANPARAM::Y_RESOLUTION )
			bRecommend = false;
		tbResolution.set( "bRecommend", bRecommend );

		tbResolutionList.set( i + 1, tbResolution );
	}

	return tbResolutionList;
}


int GLWidgetScript::GetScreenSizeWidth()
{
	NULL_INTERFACE_RETURN_VALUE( 0 );
	return int( GLWidgetScript::GetInstance().m_pInterface->GetPresentParams().BackBufferWidth );
}

int GLWidgetScript::GetScreenSizeHeight()
{
	NULL_INTERFACE_RETURN_VALUE( 0 );
	return int( GLWidgetScript::GetInstance().m_pInterface->GetPresentParams().BackBufferHeight );
}

LuaTable GLWidgetScript::GetMousePos()
{
	LuaTable tbMousePos( GLWidgetScript::GetInstance().m_pLuaState );

	NULL_INTERFACE_RETURN_VALUE( tbMousePos );

	int nMouseX = 0;
	int nMouseY = 0;
	GLWidgetScript::GetInstance().m_pInterface->GetMousePos( nMouseX, nMouseY );

	tbMousePos.set( "x", nMouseX );
	tbMousePos.set( "y", nMouseY );

	return tbMousePos;
}

void GLWidgetScript::ChangeScreenMode()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->ChangeWndMode();
}

bool GLWidgetScript::IsFullScreen()
{
	return TRUE == RANPARAM::bScrWindowed ? false : true;
}

void GLWidgetScript::TransInputEn()
{
	GfxInterfaceBase::GetInstance()->SetConversionModeEn();
}

void GLWidgetScript::CloseGame()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->CloseGame();
}

std::string GLWidgetScript::GetGameExternalText( const char* strKey, int nIndex )
{
	if( NULL == strKey || 0 > nIndex )
		return "";

	return std::string( ID2GAMEEXTEXT( strKey, nIndex ) );
}

std::string GLWidgetScript::GetGameInternalText( const char* strKey, int nIndex )
{
	if( NULL == strKey || 0 > nIndex )
		return "";

	return std::string( ID2GAMEINTEXT( strKey, nIndex ) );
}

std::string GLWidgetScript::GetGameWordText( const char* strKey, int nIndex )
{
	if( NULL == strKey || 0 > nIndex )
		return "";

	return std::string( ID2GAMEWORD( strKey, nIndex ) );
}

std::string GLWidgetScript::GetTipText_RandomIntex(void)
{
	return CGameTextMan::GetInstance().GetTipRandomText().GetString();
}

std::string GLWidgetScript::AnsiToUTF8( const char* szText )
{
	if( EMGFX_FONT_KOREAN == GfxInterfaceBase::GetInstance()->GetFontLanguage() )
		return std::string( szText );

	std::wstring strW = GLWidgetScript::GetInstance().m_pInterface->EncodeText( szText );
	return sc::string::unicodeToUtf8( strW );
}

std::string GLWidgetScript::UTF8ToAnsi( const char* szText )
{
	return sc::string::utf8_to_ansi( std::string( szText ) );
}

std::string GLWidgetScript::GetCharClassName( int nClass )
{
	return COMMENT::CHARCLASS[ CharClassToIndex( EMCHARCLASS( nClass ) ) ].c_str();
}

std::string GLWidgetScript::GetCharClassIndexName( int nIndex )
{
	if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		return std::string( CGameTextMan::GetInstance().GetCommentText( "CHARCLASS", nIndex ).GetString() );
	else
		return COMMENT::CHARCLASS[ EMCHARINDEX( nIndex ) ];
}

int GLWidgetScript::GetCharClassToClassIndex( int nClass )
{
	return CharClassToClassIndex(CharIndexToClass(EMCHARINDEX(nClass)));
}


int GLWidgetScript::GetCharClassFlagIndex( int nCharFlag )
{
	return CharClassToIndex( EMCHARCLASS( nCharFlag ) );
}

int GLWidgetScript::GetCharClassToSexFlag( int nCharSex )
{
	return CharClassToSex( EMCHARCLASS( nCharSex ) );
}   

int GLWidgetScript::GetPetType( const unsigned int nPetType )
{
	return COMMENT::GetPetDefineIndex( nPetType );
}

std::string GLWidgetScript::GetPetName( int nPetName )
{
	return COMMENT::szPetStyleCardText[ nPetName ];
}

std::string GLWidgetScript::GetMapName( DWORD dwID )
{
	return GLWidgetScript::GetInstance().m_pGaeaClient->GetMapName( SNATIVEID( dwID ) );
}

std::string GLWidgetScript::GetItemName( WORD wMID, WORD wSID )
{
	return GLogicData::GetInstance().GetItemName( wMID, wSID ).c_str();
}

std::string GLWidgetScript::GetSkillName( WORD wMID, WORD wSID )
{
	return GLogicData::GetInstance().GetSkillName( wMID, wSID );
}

LuaTable GLWidgetScript::GetDate()
{
	// 현재 시간
	CTime cTime( GLWidgetScript::GetInstance().m_pGaeaClient->GetCurrentTime() );

	// 서버 타임으로 변환( TimeZone 계산 )
	GLWidgetScript::GetInstance().m_pGaeaClient->GetConvertServerTime( cTime );

	LuaTable tbDate( GLWidgetScript::GetInstance().GetLuaState() );
	tbDate.set( "nYear", (int) cTime.GetYear() );
	tbDate.set( "nMonth", (int) cTime.GetMonth() );
	tbDate.set( "nDay", (int) cTime.GetDay() );
	tbDate.set( "nHour", (int) cTime.GetHour() );
	tbDate.set( "nMinute", (int) cTime.GetMinute() );

	return tbDate;
}

bool GLWidgetScript::CheckString( const char* str )
{
	CString strTemp = str;
	strTemp = strTemp.Trim();

	if( TRUE == STRUTIL::CheckString( strTemp ) )
		return true;

	return false;
}

int GLWidgetScript::GetServiceProvider()
{
	return int( GLWidgetScript::GetInstance().m_pGaeaClient->GetServiceProvider() );
}

int GLWidgetScript::GetRenderServiceProvider()
{
	return (int) RENDERPARAM::emSERVICE_TYPE;
}

void GLWidgetScript::SetStageBG( int nStage )
{
	NULL_INTERFACE_RETURN;
	GLWidgetScript::GetInstance().m_pInterface->SetStageBG( nStage );
}

int GLWidgetScript::GetStageBG()
{
	NULL_INTERFACE_RETURN_VALUE( 0 );
	return GLWidgetScript::GetInstance().m_pInterface->GetStageBG();
}

void GLWidgetScript::GameToLobbyStage( bool bWorldBattle )
{
	CloseAllWidget( true );
	GfxInterfaceBase::GetInstance()->ClearDisplay();

	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	pGlobalStage->GameToLobbyStage( bWorldBattle );
}

void GLWidgetScript::ToCustomerCenter()
{
	HWND hWnd = NULL; 

	CWnd* pWnd = AfxGetMainWnd();
	if( NULL != pWnd )
		hWnd = pWnd->m_hWnd;

	if( NULL != hWnd )
		ShowWindow( hWnd, SW_MINIMIZE );

	ShellExecute( NULL, "open", RANPARAM::CustomerCenterAddress, NULL, NULL, SW_SHOWNORMAL );
}

void GLWidgetScript::OpenWebPage( const char* szURL )
{
	if( NULL == szURL )
		return;

	if( true == IsFullScreen() )
	{
		HWND hWnd = NULL; 

		CWnd* pWnd = AfxGetMainWnd();
		if( NULL != pWnd )
			hWnd = pWnd->m_hWnd;

		if( NULL != hWnd )
			ShowWindow( hWnd, SW_MINIMIZE );
	}

	ShellExecute( NULL, "open", szURL, NULL, NULL, SW_SHOWNORMAL );
}

void GLWidgetScript::SetSfxVolume( long lVolume )
{
	RANPARAM::nSndSfx = lVolume;
	DxSoundMan::GetInstance().SetSfxVolume( lVolume );
}

void GLWidgetScript::SetEnvVolume( long lVolume )
{
	RANPARAM::nSndMap = lVolume;
	DxSoundMan::GetInstance().SetMapVolume( lVolume );
}

void GLWidgetScript::SetBGMVolume( long lVolume )
{
	RANPARAM::nSndMusic = lVolume;
	DxBgmSound::GetInstance().SetVolume( lVolume );
}

long GLWidgetScript::GetSfxVolume( long lVolume )
{
	return RANPARAM::nSndSfx;
}

long GLWidgetScript::GetEnvVolume( long lVolume )
{
	return RANPARAM::nSndMap;
}

long GLWidgetScript::GetBGMVolume( long lVolume )
{
	return RANPARAM::nSndMusic;
}

void GLWidgetScript::SetMuteSfx( bool bMute )
{
	RANPARAM::bSndMuteSfx = true == bMute ? FALSE : TRUE;
	DxSoundMan::GetInstance().SetSfxMute( RANPARAM::bSndMuteSfx );
}

void GLWidgetScript::SetMuteEnv( bool bMute )
{
	RANPARAM::bSndMuteMap = true == bMute ? FALSE : TRUE;
	DxSoundMan::GetInstance().SetMapMute( RANPARAM::bSndMuteMap );
}

void GLWidgetScript::SetMuteBGM( bool bMute )
{
	RANPARAM::bSndMuteMusic = true == bMute ? FALSE : TRUE;
	DxBgmSound::GetInstance().SetMute( RANPARAM::bSndMuteMusic );
}

bool GLWidgetScript::GetMuteSfx()
{
	return TRUE == RANPARAM::bSndMuteSfx ? false : true;
}

bool GLWidgetScript::GetMuteEnv()
{
	return TRUE == RANPARAM::bSndMuteMap ? false : true;
}

bool GLWidgetScript::GetMuteBGM()
{
	return TRUE == RANPARAM::bSndMuteMusic ? false : true;
}

void GLWidgetScript::SetGameOption( LuaTable tbOption )
{
	DXPARAMSET::GetInstance().m_bDIS_CONFT = tbOption.get< bool >( "bDuel" );
	DXPARAMSET::GetInstance().m_bDIS_TRADE = tbOption.get< bool >( "bTrade" );
	DXPARAMSET::GetInstance().m_bDIS_PARTY = tbOption.get< bool >( "bParty" );
	DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP = tbOption.get< bool >( "bMiniHP" );
	DXPARAMSET::GetInstance().m_bAUTO_CROW_HP = tbOption.get< bool >( "bMobHP" );
	DXPARAMSET::GetInstance().m_bSIMPLE_SUM_NAME = tbOption.get< bool >( "bSummon" );

	RANPARAM::bDIS_FRIEND = tbOption.get< bool >( "bFriend" );
	RANPARAM::bMOVABLE_ON_CHAT = tbOption.get< bool >( "bChat" );
	RANPARAM::bFORCED_ATTACK = tbOption.get< bool >( "bAttack" );
	RANPARAM::bSHOW_TIP = tbOption.get< bool >( "bTip" );
	RANPARAM::bNAME_DISPLAY = tbOption.get< bool >( "bName" );
	RANPARAM::emAroundDetail = EM_AROUND_DETAILTYPE( tbOption.get< int >( "nCharSimple" ) );
	RANPARAM::emSkillTypeUI = EM_SKILLTYPE_UI( tbOption.get< int >( "nSkill" ) );
	RANPARAM::bDIS_VEHICLE = tbOption.get< bool >( "bRide" );
	RANPARAM::bCharInfoPrivate = tbOption.get< bool >( "bMyInfo" );
	RANPARAM::bGroupChatInvite = tbOption.get< bool >( "bGroupChat" );
	RANPARAM::bCharInfoAddition = tbOption.get< bool >( "bAdditionalOp" );
	RANPARAM::dwNameDisplay = tbOption.get< DWORD >( "dwNameDisplay" );
	RANPARAM::bCheckAutoRebirth = tbOption.get< bool >( "bCheckAutoRebirth" );
	RANPARAM::bShowOtherDamage = tbOption.get< bool >( "bShowOtherDamage" );
	RANPARAM::nUIAntiAliasing = tbOption.get< int >( "nUIAntiAliasing" );
	RANPARAM::bDisplayLimit = tbOption.get< bool >( "bDisplayLimit" );

	GLWidgetSaveProp::GetInstance().fUIAlpha = tbOption.get< float >( "fUIAlpha" );
	GLWidgetSaveProp::GetInstance().m_nBasicChat_Size = tbOption.get< int >( "nBasicChat_TextSize" );

	GLWidgetScript::GetInstance().m_pGaeaClient->AllCharClientUpdateSuit();
	GLWidgetScript::GetInstance().m_pInterface->SetEdgeAAMode( RANPARAM::nUIAntiAliasing );

	DXPARAMSET::GetInstance().GameOKFromWidget();

	GLWidgetScript::GetInstance().LuaCallFunc<void>(
		NSWIDGET_SCRIPT::g_strFunc_BasicChat_MovaleOnChatLog
		, "-b", RANPARAM::bMOVABLE_ON_CHAT);

	GLWidgetScript::GetInstance().LuaCallFunc<void>(
		NSWIDGET_SCRIPT::g_strFunc_ShowTip
		, "-b", RANPARAM::bSHOW_TIP);
}

LuaTable GLWidgetScript::GetGameOption()
{
	LuaTable tbOption( GLWidgetScript::GetInstance().GetLuaState() );

	tbOption.set( "bDuel", DXPARAMSET::GetInstance().m_bDIS_CONFT );			// 대련 거부
	tbOption.set( "bTrade", DXPARAMSET::GetInstance().m_bDIS_TRADE );			// 거래 거부
	tbOption.set( "bParty", DXPARAMSET::GetInstance().m_bDIS_PARTY );			// 파티 거부
	tbOption.set( "bMiniHP", DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP );		// 미니 HP 표시
	tbOption.set( "bMobHP", DXPARAMSET::GetInstance().m_bAUTO_CROW_HP );		// 몬스터 HP바 표시
	tbOption.set( "bSummon", DXPARAMSET::GetInstance().m_bSIMPLE_SUM_NAME );	// 소환수 이름 단순화
	tbOption.set( "bFriend", TRUE == RANPARAM::bDIS_FRIEND ? true : false );				// 친구 거부
	tbOption.set( "bChat", TRUE == RANPARAM::bMOVABLE_ON_CHAT ? true : false );				// 채팅창 뚫기
	tbOption.set( "bAttack", TRUE == RANPARAM::bFORCED_ATTACK ? true : false );				// 강제 공격
	tbOption.set( "bTip", TRUE == RANPARAM::bSHOW_TIP ? true : false );						// 팁 보기
	tbOption.set( "bName", TRUE == RANPARAM::bNAME_DISPLAY ? true : false );				// 이름 표시
	tbOption.set( "nCharSimple", (int) RANPARAM::emAroundDetail );							// 캐릭터 단순화 등급
	tbOption.set( "nSkill", (int) RANPARAM::emSkillTypeUI );								// 스킬 타입 표시
	tbOption.set( "bRide", TRUE == RANPARAM::bDIS_VEHICLE ? true : false );					// 탑승 요청 거부	
	tbOption.set( "bMyInfo", TRUE == RANPARAM::bCharInfoPrivate ? true : false );			// 내 정보 비공개
	tbOption.set( "bGroupChat", TRUE == RANPARAM::bGroupChatInvite ? true : false );		// 그룹채팅 거부
	tbOption.set( "bAdditionalOp", TRUE == RANPARAM::bCharInfoAddition ? true : false );	// 부가 능력 표시	
	tbOption.set( "fUIAlpha", GLWidgetSaveProp::GetInstance().fUIAlpha );					// 전체 UI 알파값
	tbOption.set( "dwNameDisplay", RANPARAM::dwNameDisplay );								// 이름 보기 설정
	tbOption.set( "bCheckAutoRebirth", RANPARAM::bCheckAutoRebirth );						// 자동 부활하기
	tbOption.set( "bShowOtherDamage", RANPARAM::bShowOtherDamage );							// 다른 유저 데미지 보기
	tbOption.set( "nUIAntiAliasing", RANPARAM::nUIAntiAliasing );							// UI 안티 앨리어싱
	tbOption.set( "bDisplayLimit", RANPARAM::bDisplayLimit );								// 이름/데미지 디스플레이 개수 제한
	tbOption.set( "nBasicChat_TextSize", GLWidgetSaveProp::GetInstance().m_nBasicChat_Size );					// 채팅 폰트 크기

	return tbOption;
}

LuaTable GLWidgetScript::GetCameraPos()
{
	const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetFromPt();

	LuaTable tbPos( GLWidgetScript::GetInstance().GetLuaState() );
	tbPos.set( "x", vPos.x );
	tbPos.set( "y", vPos.y );
	tbPos.set( "z", vPos.z );

	return tbPos;
}

LuaTable GLWidgetScript::GetCameraDir()
{
	D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vCameraDir = vLookatPt - vFromPt;

	LuaTable tbDir( GLWidgetScript::GetInstance().GetLuaState() );
	tbDir.set( "x", vCameraDir.x );
	tbDir.set( "y", vCameraDir.y );
	tbDir.set( "z", vCameraDir.z );

	return tbDir;
}

bool GLWidgetScript::GetAutoResurrection()
{
	return RANPARAM::bCheckAutoRebirth;
}

void GLWidgetScript::SetAutoResurrection( bool bValue )
{
	RANPARAM::bCheckAutoRebirth = bValue;
}

void GLWidgetScript::StopSound( const char* str )
{
	DxSoundLib::GetInstance()->StopSound( str );
}

void GLWidgetScript::PlaySound( const char* str )
{
	DxSoundLib::GetInstance()->PlaySound( str );
}

DWORD GLWidgetScript::MakeNativeID( WORD wMID, WORD wSID )
{
	SNATIVEID sID( wMID, wSID );
	return sID.Id();
}

LuaTable GLWidgetScript::SplitNativeID( DWORD dwID )
{
	SNATIVEID sID( dwID );

	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );
	tb.set( 1, sID.Mid() );
	tb.set( 2, sID.Sid() );

	return tb;
}

std::string GLWidgetScript::MakeMoney( LONGLONG llMoney )
{
	CString str = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llMoney, 3, "," );
	return str.GetBuffer();
}

WORD GLWidgetScript::ColorDWORDtoWORD( DWORD dwColor )
{
	// |  X |       RED      |   GREEN   |    BLUE   |
	// | 15 | 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0 | 

	const float fRGBratio = 8.225806f;

	WORD wR = ((dwColor & 0xFF0000) >> 16) / fRGBratio;
	WORD wG = ((dwColor & 0xFF00) >> 8) / fRGBratio;
	WORD wB = ((dwColor & 0xFF)) / fRGBratio;

	return wR << 10 | wG << 5 | wB;
}

DWORD GLWidgetScript::ColorWORDtoDWORD( WORD wColor )
{
	// |  X |       RED      |   GREEN   |    BLUE   |
	// | 15 | 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0 | 

	const float fRGBratio = 8.225806f;

	DWORD dwR = ((wColor & 0x7c00) >> 10) * fRGBratio;
	DWORD dwG = ((wColor & 0x3e0) >> 5) * fRGBratio;
	DWORD dwB = ((wColor & 0x1f)) * fRGBratio;

	return D3DCOLOR_XRGB( dwR, dwG, dwB );
}

void GLWidgetScript::OpenForum()
{
	CWnd* pWnd = ::AfxGetMainWnd();
	HWND hWnd = NULL;
	if ( pWnd)
		hWnd = pWnd->m_hWnd;
	if ( hWnd )
		::ShowWindow( hWnd, SW_MINIMIZE );


	EMSERVICE_PROVIDER  ServiceProvider =  GLogicData::GetInstance().GetServiceProvider();
	if (ServiceProvider == SP_KOREA_TEST)
		::ShellExecute( NULL, "open", RANPARAM::strTWForumURL, NULL, NULL, SW_SHOW );
	else
		::ShellExecute( NULL, "open", RANPARAM::strForumURL, NULL, NULL, SW_SHOW );
}

std::string GLWidgetScript::GetCompositionString()
{
	//const wchar_t* szCompositionStr = GfxInterfaceBase::GetInstance()->GetIMECompositionString();
	// const wchar_t* szCompositionStr = GetCore()->GetIME()->GetCompositionString();
	//return sc::string::unicodeToAnsi(szCompositionStr, wcslen(szCompositionStr));

	return GfxInterfaceBase::GetInstance()->GetIMECompositionString();;
}

LuaTable GLWidgetScript::GetItem_BoxItemlist( WORD wMID, WORD wSID )
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );

	SITEM* pItem = GLogicData::GetInstance().GetItem ( SNATIVEID(wMID, wSID) );
	if ( !pItem )
		return tbItemList;
	
	INT nIndex, nItemCount(0), nSize((INT)pItem->sBox.vecItems.size());
	for ( nIndex = 0; nIndex < nSize; ++nIndex )
	{
		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, pItem->sBox.vecItems[nIndex].nidITEM.Mid() );
		tbItem.set( 2, pItem->sBox.vecItems[nIndex].nidITEM.Sid() );
		tbItem.set( 3, pItem->sBox.vecItems[nIndex].dwAMOUNT );
		tbItemList.set( ++nItemCount, tbItem );
	}
	return tbItemList;
}

DWORD GLWidgetScript::GetMoneyColor( LONGLONG lMoney )
{
	return MONEYCOLOR::GetMoneyColor(lMoney);
}