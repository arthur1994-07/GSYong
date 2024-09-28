#include "StdAfx.h"
#include "GfxInterface.h"

#include "../enginelib/Common/CommonWeb.h"
#include "../enginelib/DxTools/DxInputDevice.h"

#include "../enginelib/DxTools/DxViewPort.h"

#include "./Scene/GfxGameScene.h"
#include "./Scene/GfxLoadingScene.h"
#include "./Scene/GfxLoginScene.h"
#include "./Scene/GfxDebugScene.h"
#include "./Slot/GfxSlot.h"
#include "./Display/GfxDisplay.h"
#include "./GfxWidgetInstanceMan.h"

#include "../RanLogic/Market/GLConsignmentSaleDefine.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/RanMobile/GLRanMobile.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient//Club/GLClubClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"
#include "../RanLogicClient/Char/GLCharClient.h"
#include "../RanLogicClient/ReferChar/ReferChar.h"
#include "../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../RanLogicClient/PrivateMarketSearchBuy/ConsignmentSaleClient.h"
#include "../RanLogicClient/WishList/WishList.h"
#include "../RanLogicClient/Widget/GLWidgetScript.h"
#include "../RanLogicClient/Widget/GLWidgetsList.h"
#include "../RanLogicClient/Post/GLPostClient.h"
#include "../RanLogicClient/Party/GLPartyTenderManagerClient.h"
#include "../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../RanLogicClient/StaticHoldItem.h"

class SASInterface
{
public :
	SASInterface();
	~SASInterface();

	GFx::Value	valInterface;
	std::string strInterfaceName;

	void Clear();
	void SetName( GfxInterface::EMAS_Interface emInterface );
};

SASInterface::SASInterface()
{
}

SASInterface::~SASInterface()
{
	Clear();
}

void SASInterface::Clear()
{
	valInterface.SetUndefined();
}

void SASInterface::SetName( GfxInterface::EMAS_Interface emInterface )
{
	switch( emInterface )
	{
	case GfxInterface::AS_OpenWidget			: strInterfaceName = "root.CF_OpenWidget"; break;
	case GfxInterface::AS_CloseWidget			: strInterfaceName = "root.CF_CloseWidget"; break;
	case GfxInterface::AS_CloseAllWidget		: strInterfaceName = "root.CF_UnloadWidgetAll"; break;
	case GfxInterface::AS_UnloadWidget			: strInterfaceName = "root.CF_UnloadWidget"; break;
	case GfxInterface::AS_IsOpenWidget			: strInterfaceName = "root.CF_IsOpenWidget"; break;
	case GfxInterface::AS_IsLoadWidget			: strInterfaceName = "root.CF_IsLoadWidget"; break;
	case GfxInterface::AS_GetMultiWidgetCount	: strInterfaceName = "root.CF_GetMultiWidgetCount"; break;
	case GfxInterface::AS_EscWidgetClose		: strInterfaceName = "root.CF_EscClose"; break;
	case GfxInterface::AS_SetStageEvent			: strInterfaceName = "root.CF_SetStageEvent"; break;
	case GfxInterface::AS_SetProperty			: strInterfaceName = "root.CF_SetProperty"; break;
	case GfxInterface::AS_GetProperty			: strInterfaceName = "root.CF_GetProperty"; break;
	case GfxInterface::AS_SetEvent				: strInterfaceName = "root.CF_SetEvent"; break;
	case GfxInterface::AS_SetDataProvider		: strInterfaceName = "root.CF_SetDataProvider"; break;
	case GfxInterface::AS_RemoveDataProvider	: strInterfaceName = "root.CF_RemoveDataProvider"; break;
	case GfxInterface::AS_GetDataProvider		: strInterfaceName = "root.CF_GetDataProvider"; break;
	case GfxInterface::AS_SetDataProviderProp	: strInterfaceName = "root.CF_SetDataProviderProp"; break;
	case GfxInterface::AS_SetListItemPropArray	: strInterfaceName = "root.CF_SetListItemProperty_Array"; break;
	case GfxInterface::AS_SetListItemProp		: strInterfaceName = "root.CF_SetListItemProperty"; break;
	case GfxInterface::AS_CallListItemFunc		: strInterfaceName = "root.CF_CallListItemFunc"; break;
	case GfxInterface::AS_CallFunction			: strInterfaceName = "root.CF_CallFunction"; break;
	case GfxInterface::AS_SetFocus				: strInterfaceName = "root.CF_SetFocus"; break;
	case GfxInterface::AS_SetTextFormat			: strInterfaceName = "root.CF_SetTextFormat"; break;
	case GfxInterface::AS_SetTween				: strInterfaceName = "root.CF_SetTween"; break;
	case GfxInterface::AS_HitTestEnable			: strInterfaceName = "root.CF_HitTestEnable"; break;
	case GfxInterface::AS_GetSlotTooltipPosSize	: strInterfaceName = "root.CF_GetSlotTooltipPosSize"; break;
	case GfxInterface::AS_SlotDragStop			: strInterfaceName = "root.CF_StopDrag"; break;
	case GfxInterface::AS_GetDragSlotBinding	: strInterfaceName = "root.CF_GetSlotTooltipPosSize"; break;
	case GfxInterface::AS_GetWidgetInstance		: strInterfaceName = "root.CF_GetWidgetInstance"; break;
	case GfxInterface::AS_ResetWidgetPosition	: strInterfaceName = "root.CF_ResetWidgetPosition"; break;
	case GfxInterface::AS_IsOpenTooltip			: strInterfaceName = "root.CF_IsOpenTooltip"; break;
	case GfxInterface::AS_GetDataProviderArray	: strInterfaceName = "root.CF_GetDataProviderArray"; break;
	case GfxInterface::AS_SetSpeechBubble		: strInterfaceName = "root.CF_SetSpeechBubble"; break;
	case GfxInterface::AS_DelSpeechBubble		: strInterfaceName = "root.CF_DelSpeechBubble"; break;
	case GfxInterface::AS_SetFakeModal			: strInterfaceName = "root.CF_SetFakeModal"; break;
	case GfxInterface::AS_SetDataProvider_Terms	: strInterfaceName = "root.CF_SetDataProvider_Terms"; break;
	case GfxInterface::AS_SnapCheck				: strInterfaceName = "root.CF_SnapCheck"; break;
	case GfxInterface::AS_SetUIAlpha			: strInterfaceName = "root.CF_SetUIAlpha"; break;
	case GfxInterface::AS_GC					: strInterfaceName = "root.CF_DelSpeechBubble"; break;
	case GfxInterface::AS_RemoveTween					: strInterfaceName = "root.CF_RemoveTween"; break;
	case GfxInterface::AS_RegShowTooltipEnableState		: strInterfaceName = "root.CF_RegShowTooltipEnableState"; break;
	case GfxInterface::AS_UnregShowTooltipEnableState	: strInterfaceName = "root.CF_UnregShowTooltipEnableStatep"; break;
	case GfxInterface::AS_UpdateDraggingSlot			: strInterfaceName = "root.CF_UpdateDraggingSlot"; break;
	case GfxInterface::AS_SetFoward						: strInterfaceName = "root.CF_SetFoward"; break;
	}
}

//-----------------------------------------------------------------------------------------------------

const char* GfxInterface::SPLIT_STRING_INST	= ".";
const char* GfxInterface::SPLIT_STRING_ETC	= ";";

const float GfxInterface::fGC_CallTime = 60.0f;

GfxCore* GetCore()
{
	return GfxInterface::GetInstance()->GetCore();
}

GfxInterface::GfxInterface(void)
:	GfxInterfaceBase()
,	m_pDevice( NULL )
,	m_bReload( false )
,	m_pDebugScene( NULL )
,	m_bShowDebugScene( false )
,	m_fTimerGC( 0.0f )
,	m_bCallGC( false )
,	m_bUpdatedClubIconRT( false )
,	m_pOldInterface( NULL )
{
	m_pCore = new GfxCore;
	m_pGameScene = new GfxGameScene;
	//m_pLoadingScene = new GfxLoadingScene;
	m_pLoginScene = new GfxLoginScene;
	m_pASInterface = new SASInterface[ AS_Interface_Size ];
}

GfxInterface::GfxInterface( const GfxInterface& value )
{
	m_vecRegSlotID.clear();
	m_mapHtmlWebViewID.clear();
	
	while( false == m_queUpdateClubMark.empty() )
		m_queUpdateClubMark.pop();
}

GfxInterface::~GfxInterface(void)
{
	SAFE_DELETE_ARRAY( m_pASInterface );
	SAFE_DELETE( m_pDebugScene );
	SAFE_DELETE( m_pGameScene );
	//SAFE_DELETE( m_pLoadingScene );
	SAFE_DELETE( m_pLoginScene );
	SAFE_DELETE( m_pCore );

	m_pOldInterface->UiFinalCleanup();
	SAFE_DELETE( m_pOldInterface );
}

GfxInterface* GfxInterface::GetInstance()
{
	static GfxInterface Instance;
	return &Instance;
}

void GfxInterface::OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pDevice )
{
	m_hWnd = hWnd;
	m_pDevice = pDevice;

	GetCore()->OnCreateDevice( hWnd, pDevice );
}

void GfxInterface::OnResetDevice( LPDIRECT3DDEVICE9 pDevice )
{
	m_pDevice = pDevice;

	GetCore()->OnResetDevice( pDevice );
	m_pGameScene->OnResetDevice( pDevice );
	//m_pLoadingScene->OnResetDevice( pDevice );
	m_pLoginScene->OnResetDevice( pDevice );

	if( NULL != m_pDebugScene )
		m_pDebugScene->OnResetDevice( pDevice );

	GfxDisplay::GetInstance().OnResetDevice();

	m_pOldInterface->UiRestoreDeviceObjects( pDevice );
}

void GfxInterface::OnLostDevice()
{
	GetCore()->OnLostDevice();

	m_pOldInterface->UiInvalidateDeviceObjects();
}

void GfxInterface::OnDestroyDevice()
{
	std::for_each( m_vecRegSlotID.begin(), m_vecRegSlotID.end(), 
		boost::bind( std::mem_fun( &GfxInterface::UnregSlot ), this, _1 ) );
	m_vecRegSlotID.clear();
	
	GetCore()->OnDestroyDevice();
	m_pGameScene->OnDestroyDevice();
	//m_pLoadingScene->OnDestroyDevice();
	m_pLoginScene->OnDestroyDevice();

	if( NULL != m_pDebugScene )
		m_pDebugScene->OnDestroyDevice();

	m_pOldInterface->DeleteDeviceObjects();
}

void GfxInterface::OnFrameMove( float fElapsedTime )
{
	if( EMSTAGE_BG_LOGIN == GetStageBG() )
		m_pLoginScene->OnFrameMove( fElapsedTime );

	m_pGameScene->OnFrameMove( fElapsedTime );

	GfxDisplay::GetInstance().OnFrameMove( fElapsedTime );

	// 닫힌 UI 일정 시간 지나면 언로드
	CheckDeleteWidget( fElapsedTime );
	
	if( true == m_bReload )
	{
		GetCore()->GetExternalInterface()->ResetGameMovie();

		for( WORD i = 0; i < AS_Interface_Size; ++i )
			m_pASInterface[ i ].Clear();

		std::for_each( m_vecRegSlotID.begin(), m_vecRegSlotID.end(), 
			boost::bind( std::mem_fun( &GfxInterface::UnregSlot ), this, _1 ) );

		while( false == m_queUpdateClubMark.empty() )
			m_queUpdateClubMark.pop();

		GfxWidgetInstanceMan::GetInstance().Clear();
		GfxDisplay::GetInstance().Reset();

		m_pGameScene->LoadMovie( m_pDevice );
		//m_pLoadingScene->LoadMovie( m_pDevice );
		SetEdgeAAMode( RANPARAM::nUIAntiAliasing );

		m_bReload = false;
	}

	if( true == m_bUpdatedClubIconRT )
	{
		static const float fTime = 0.25f;
		static float f = 0.0f;

		f += fElapsedTime;
		if( fTime <= f )
		{
			m_bUpdatedClubIconRT = false;
			f = 0.0f;
		}
	}
	else
	{
		while( false == m_queUpdateClubMark.empty() )
		{
			const SUpdateClubMark& sMark = m_queUpdateClubMark.front();
			if( false == IsLoadWidget( sMark.nWidgetID, sMark.nSubID ) )
			{
				m_queUpdateClubMark.pop();
				continue;
			}

			CallFunction( sMark.nWidgetID, sMark.nSubID, "CreateClubIconTexture", 
				sMark.strParam.c_str(), sMark.strInst.c_str() );

			m_bUpdatedClubIconRT = true;

			m_queUpdateClubMark.pop();
			break;
		}
	}

	if( NULL != m_pDebugScene && true == m_bShowDebugScene )
		m_pDebugScene->OnFrameMove( fElapsedTime );
}

void GfxInterface::OnFrameRender( float fElapsedTime )
{
	//m_pOldInterface->UiRender( m_pDevice );

	LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
	if( D3D_OK != m_pDevice->CreateStateBlock( D3DSBT_ALL, &pStateBlock ) )
		return;

	pStateBlock->Capture();

	if( EMSTAGE_BG_LOGIN == GetStageBG() )
		m_pLoginScene->OnFrameRender( fElapsedTime );

	m_pGameScene->OnFrameRender( fElapsedTime );
	if( NULL != m_pDebugScene && true == m_bShowDebugScene )
		m_pDebugScene->OnFrameRender( fElapsedTime );

	pStateBlock->Apply();

	SAFE_RELEASE( pStateBlock );
}

void GfxInterface::InitOldInterface()
{
	m_pOldInterface = new OldInterface( m_pGaeaClient, m_pGaeaClient->GetGlobalStage()->GetEngineDevice() );
	m_pOldInterface->UiInitDeviceObjects( m_pDevice );
}

void GfxInterface::SetLoadingData( const SNATIVEID& sMapID, const std::string& strLoadingImgFile, bool bShowGrade )
{
	//m_pLoadingScene->SetData( sMapID, strLoadingImgFile, bShowGrade );
}

void GfxInterface::OnInitGame()
{
	m_pLoginScene->OnInit( m_pDevice );
	m_pGameScene->OnInit( m_pDevice );
}

void GfxInterface::OnInitLoading()
{
	//m_pLoadingScene->OnInit( m_pDevice );
}

void GfxInterface::OnBeginLoading( ELoadingSceneType eType )
{
	//m_pLoadingScene->OnBegin( eType );
}

void GfxInterface::OnEndLoading()
{
	//m_pLoadingScene->OnEnd();
}

void GfxInterface::InitGameStage(void)
{
	SetDisplayCountryMark(false);
}

void GfxInterface::ToggleDebugScene()
{
	m_bShowDebugScene = !m_bShowDebugScene;

	if( true == m_bShowDebugScene && NULL == m_pDebugScene )
	{
		m_pDebugScene = new GfxDebugScene();
		m_pDebugScene->OnInit( m_pDevice );
	}
}

void GfxInterface::StorePresentParameters( D3DPRESENT_PARAMETERS pp )
{
	GetCore()->StorePresentParameters( pp );
}

const D3DPRESENT_PARAMETERS& GfxInterface::GetPresentParams()
{
	return GetCore()->GetPresentParams();
}

void GfxInterface::GetMousePos( int& nMouseX, int& nMouseY )
{
	float fMouseX = 0.0f;
	float fMouseY = 0.0f;
	m_pGameScene->GetMousePos( fMouseX, fMouseY );

	nMouseX = (int) fMouseX;
	nMouseY = (int) fMouseY;
}

void GfxInterface::SetConversionModeEn()
{
	if( language::DEFAULT == RENDERPARAM::emLangSet ||
		language::KOREAN == RENDERPARAM::emLangSet )
	{
		GFx::IME::GFxIMEManagerWin32* pIME = GetCore()->GetIME();
		if( NULL == pIME )
			return;

		pIME->SetConversionMode( 0x01 );
	}
}

CString GfxInterface::GetConversionMode()
{
	if( language::DEFAULT == RENDERPARAM::emLangSet ||
		language::KOREAN == RENDERPARAM::emLangSet )
	{
		GFx::IME::GFxIMEManagerWin32* pIME = GetCore()->GetIME();
		if( NULL == pIME )
			return "";

		return pIME->GetConversionMode();
	}
	else
	{
		return "";
	}
}

void GfxInterface::SetEdgeAAMode( int nMode )
{
	GFx::Movie* pMovie = m_pGameScene->GetMovie();
	if( NULL == pMovie )
		return;

	Render::EdgeAAMode eMode = Render::EdgeAA_Inherit;
	switch( nMode )
	{
	case 0 :
		eMode = Render::EdgeAA_Disable;
		break;

	case 1 :
		eMode = Render::EdgeAA_Inherit;
		break;

	default :
		eMode = Render::EdgeAA_Disable;
		break;
	}

	pMovie->SetEdgeAAMode( eMode );
}

void GfxInterface::SetInteractDropItem( bool bEnable )
{
	GfxDisplay::GetInstance().SetEnableItemClick( bEnable );
}

std::string GfxInterface::GetIMECompositionString(void)
{
	if( language::DEFAULT == RENDERPARAM::emLangSet ||
		language::KOREAN == RENDERPARAM::emLangSet )
	{
		const wchar_t* szCompositionStr = GetCore()->GetIME()->GetCompositionString();
		if ( szCompositionStr == NULL )
			return std::string();

		return sc::string::unicodeToAnsi(szCompositionStr, wcslen(szCompositionStr));
	}
	else
	{
		DXInputString* pInputString = m_pGaeaClient->GetEngineDevice()->GetInputString();
		return NULL == pInputString->GetString() ? "" : pInputString->GetString();
	}
}

void GfxInterface::Reload()
{
	m_bReload = true;
}

void GfxInterface::CheckDeleteWidget( float fElapsedTime )
{
	GfxWidgetInstanceMan::MAP_DELETE_RESERVE& mapDelete = GfxWidgetInstanceMan::GetInstance().GetDeleteReserve();

	GfxWidgetInstanceMan::MAP_DELETE_RESERVE_ITER iter = mapDelete.begin();
	while( iter != mapDelete.end() )
	{
		float& fDeleteTimer = iter->second;
		fDeleteTimer += fElapsedTime;

		const SNATIVEID& sID = iter->first;
		bool bDelete = false;

		if( true == NSWIDGET_SCRIPT::g_bDef_WidgetCLoseToDeleteUseBatch )
		{
			if( NSWIDGET_SCRIPT::g_fDef_WidgetCloseToDeleteTime < fDeleteTimer )
				bDelete = true;
		}
		else
		{
			const GLWidget* pWidget = GLWidgetsList::GetInstance().GetWidget( sID.wMainID );
			if( NULL != pWidget )
			{
				if( (float) pWidget->m_wDeleteTime < fDeleteTimer )
					bDelete = true;
			}
		}

		if( true == bDelete )
		{
			m_bCallGC = true;

			GFx::Value args[ 2 ];
			args[ 0 ].SetInt( (int) sID.wMainID );
			args[ 1 ].SetInt( (int) sID.wSubID );
			Invoke( AS_UnloadWidget, args, 2 );

			iter = mapDelete.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( true == m_bCallGC )
	{
		m_fTimerGC += fElapsedTime;

		if( fGC_CallTime <= m_fTimerGC )
		{
			m_fTimerGC = 0.0f;
			m_bCallGC = false;

			GFx::Value result;
			Invoke( AS_GC, NULL, 0, &result );
		}
	}
}

GFx::Value* GfxInterface::FindInstance( WORD wWidgetID, WORD wSubID, std::string strInst )
{
	GFx::Value* pValue = GfxWidgetInstanceMan::GetInstance().GetWidgetInstance( wWidgetID, wSubID, strInst );
	if( NULL == pValue )
	{
		GFx::Value args[ 3 ];
		args[ 0 ].SetInt( wWidgetID );
		args[ 1 ].SetInt( wSubID );
		args[ 2 ].SetString( strInst.c_str() );

		GFx::Value valResult;
		Invoke( AS_GetWidgetInstance, args, 3, &valResult );

		if( false == valResult.IsNull() && false == valResult.IsUndefined() )
		{
			GfxWidgetInstanceMan::GetInstance().Insert( wWidgetID, wSubID, strInst, valResult );
			pValue = GfxWidgetInstanceMan::GetInstance().GetWidgetInstance( wWidgetID, wSubID, strInst );
		}
	}

	return pValue;
}

void GfxInterface::PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_pGameScene->PreProcessEvent( hWnd, uMsg, wParam, lParam );
}

bool GfxInterface::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != m_pDebugScene && true == m_bShowDebugScene )
		return m_pDebugScene->ProcessEvent( hWnd, uMsg, wParam, lParam, m_bMouseHitUI, m_bIsDragging );

	// HTML 뷰로 포커스가 넘어가면 이벤트가 다 씹혀 히트 테스트가 불가능하다.
	// 어차피 UI 위에서 뜨기 때문에 HTML 뷰 사용하는 UI 위에 마우스 올라갔으면 히트했다고 치자.
	if( NULL != GLWidgetScript::GetInstance().GetLuaState() )
	{
		int nMouseX = DxInputDevice::GetInstance().GetMouseLocateX();
		int nMouseY = DxInputDevice::GetInstance().GetMouseLocateY();

		LuaTable tbSize = GLWidgetScript::GetInstance().LuaCallFunc< LuaTable >(
			NSWIDGET_SCRIPT::g_strFunc_GetHTMLViewSize );

		for( int i = 1; i <= tbSize.TableLen(); ++i )
		{
			LuaTable tb = tbSize.get< LuaTable >( i );

			if( IS_LUATABLE( tb ) )
			{
				int nX = tb.get< int >( 1 );
				int nY = tb.get< int >( 2 );
				int nW = tb.get< int >( 3 );
				int nH = tb.get< int >( 4 );

				if( nX <= nMouseX && nX + nW >= nMouseX &&
					nY <= nMouseY && nY + nH >= nMouseY )
				{
					m_bMouseHitUI = true;
					break;
				}
			}
		}
	}

	return m_pGameScene->ProcessEvent( hWnd, uMsg, wParam, lParam, m_bMouseHitUI, m_bIsDragging );
}

void GfxInterface::SetDisplayCountryMark( bool bValue )
{
	GfxInterfaceBase::SetDisplayCountryMark( bValue );

	GfxDisplay::GetInstance().ForceUpdateNameDisplay();
}

void GfxInterface::SetFontLanguage( EMGFX_FONT_LANG emFontLang )
{
	m_emFontLang = emFontLang;
	GetCore()->SetFontLanguage( emFontLang );
}

bool GfxInterface::IsMouseInUI()
{
	GFx::Movie* pMovie = m_pGameScene->GetMovie();
	if( NULL == pMovie )
		return false;

	float fMouseX = 0.0f;
	float fMouseY = 0.0f;
	m_pGameScene->GetMousePos( fMouseX, fMouseY );

	return pMovie->HitTest( fMouseX, fMouseY, Movie::HitTest_ShapesNoInvisible );
}

bool GfxInterface::Invoke( EMAS_Interface emInterface, const GFx::Value* pArgs, const int nArgsCount, GFx::Value* pResult )
{
	if( false == GetCore()->GetExternalInterface()->IsCraeteCompleteGameMovie() )
		return false;

	GFx::Value result;
	if( NULL == pResult )
		pResult = &result;

	int nID = -1;
	bool bSuccess = false;

	if( NULL == pArgs )
	{
		bSuccess = m_pASInterface[ emInterface ].valInterface.InvokeSelf( pResult );
	}
	else
	{
		bSuccess = m_pASInterface[ emInterface ].valInterface.InvokeSelf( pResult, pArgs, nArgsCount );
		nID = pArgs[ 0 ].GetInt();
	}

	if( true == isProfiling() )
		ProfilingRecord( nID, m_pASInterface[ emInterface ].strInterfaceName );

	return bSuccess;
}

void GfxInterface::SplitString( VEC_STRING& vecSplitStr, const char* str, const std::string& strSeparator )
{
	if( true == strSeparator.empty() )
	{
		vecSplitStr.push_back( str );
		return;
	}

	std::string strPushInst = "";

	while( *str )
	{
		if( 0 == strncmp( str, NSWIDGET_SCRIPT::g_strPrepareHTML[ NSWIDGET_SCRIPT::HTML_SEMICOLON ][ 1 ], 3 ) )
		{
			strPushInst += NSWIDGET_SCRIPT::g_strPrepareHTML[ NSWIDGET_SCRIPT::HTML_SEMICOLON ][ 0 ];
			str += 3;

			continue;
		}
		else if( 0 == strncmp( str, strSeparator.c_str(), strSeparator.size() ) )
		{
			boost::replace_all( strPushInst, "\r\n", "\n" );
			vecSplitStr.push_back( strPushInst );
			strPushInst = "";

			++str;

			continue;
		}

		strPushInst += str[ 0 ];
		++str;
	}

	if( false == strPushInst.empty() )
	{
		boost::replace_all( strPushInst, "\r\n", "\n" );
		vecSplitStr.push_back( strPushInst );
	}
}

void GfxInterface::CreateStringArray( const char* str, Scaleform::GFx::Value* pValue, const std::string& strSeparator )
{
	VEC_STRING vecInst;
	SplitString( vecInst, str, strSeparator );

	m_pGameScene->GetMovie()->CreateArray( pValue );
	for( size_t i = 0; i < vecInst.size(); ++i )
	{
		GFx::Value val;
// 		if ( vecInst[ i ].empty() )
// 		{
// 			val.SetNull();
// 		}
// 		else
// 		{
			std::wstring strW = EncodeText( vecInst[ i ] );
			val.SetStringW( strW.c_str() );
//		}
		pValue->SetElement( i, val );
	}
}

void GfxInterface::PreSetProperty( GFx::Value* pArgs, int nWidgetID, int nSubID,
								   const char* strProp, const char* strInstance )
{
	pArgs[ 0 ].SetInt( nWidgetID );
	pArgs[ 1 ].SetInt( nSubID );
	pArgs[ 2 ].SetString( strProp );
	pArgs[ 4 ].SetString( strInstance );
}

EMGFX_OBJECT_TYPE GfxInterface::PreGetProperty( GFx::Value* pGfxValue, int nWidgetID, int nSubID,
												const char* strProp, const char* strInstance )
{
	GFx::Value args[ 4 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strProp );
	args[ 3 ].SetString( strInstance );

	GFx::Value result;
	if( false == Invoke( AS_GetProperty, args, 4, &result ) )
		return EMGFX_OBJ_TYPE_NULL;

	if( false == result.IsArray() || 2 != result.GetArraySize() )
		return EMGFX_OBJ_TYPE_NULL;

	GFx::Value gfxType;
	result.GetElement( 0, &gfxType );
	result.GetElement( 1, pGfxValue );

	return (EMGFX_OBJECT_TYPE) int( gfxType.GetInt() );
}

EMGFX_OBJECT_TYPE GfxInterface::PreGetDataProvider( Scaleform::GFx::Value* pGfxValue, int nWidgetID, int nSubID, 
												    int nIndex, const char* strName, const char* strInstance )
{
	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nIndex );
	args[ 3 ].SetString( strName );
	args[ 4 ].SetString( strInstance );

	GFx::Value result;
	if( false == Invoke( AS_GetDataProvider, args, 5, &result ) )
		return EMGFX_OBJ_TYPE_NULL;

	if( false == result.IsArray() || 2 != result.GetArraySize() )
		return EMGFX_OBJ_TYPE_NULL;

	GFx::Value gfxType;
	result.GetElement( 0, &gfxType );
	result.GetElement( 1, pGfxValue );

	return (EMGFX_OBJECT_TYPE) int( gfxType.GetInt() );
}

void GfxInterface::PreSetDataProviderProp( Scaleform::GFx::Value* pArgs, int nWidgetID, int nSubID, int nIndex,
										   const char* strName, const char* strInstance )
{
	pArgs[ 0 ].SetInt( nWidgetID );
	pArgs[ 1 ].SetInt( nSubID );
	pArgs[ 2 ].SetInt( nIndex );
	pArgs[ 3 ].SetString( strName );
	pArgs[ 5 ].SetString( strInstance );
}

GFx::Value* GfxInterface::PreSetListItemProp( GFx::Value* pArgs, int nWidgetID, int nSubID, int nIndex,
											 const char* strInstName, const char* strProp, const char* strInstance )
{
	pArgs[ 0 ].SetInt( nWidgetID );
	pArgs[ 1 ].SetInt( nSubID );
	pArgs[ 2 ].SetInt( nIndex );
	pArgs[ 3 ].SetString( strInstName );
	pArgs[ 4 ].SetString( strProp );
	pArgs[ 6 ].SetString( strInstance );

	return pArgs;
}

void GfxInterface::InsertTreeData( GFx::Value* pParentArray, int& nArrayIndex, 
								   const VEC_STRING& vecObjectID, LuaTable tbData )
{
	if( vecObjectID.size() != tbData.TableLen() )
		return;

	GFx::Value gfxObject;
	m_pGameScene->GetMovie()->CreateObject( &gfxObject );

	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		// LuaTable data = tbData.get< LuaTable >( i );
		// 테이블이 아닌 값을 테이블로 받으면 뻗는다.
		// 일단 ObjectID가 leaf일 때만 테이블로 받도록 하자.
		if( 0 != strcmp( "leaf", vecObjectID[ i - 1 ].c_str() ) )
		{
			const char* sz = tbData.get< const char* >( i );
			if( NULL != sz )
			{
				std::wstring strData = EncodeText( sz );
				gfxObject.SetMember( vecObjectID[ i - 1 ].c_str(), strData.c_str() );
			}
			// bool 값은 문자열로 안가져와진다.
			else
			{
				bool b = tbData.get< bool >( i );
				gfxObject.SetMember( vecObjectID[ i - 1 ].c_str(), b );
			}
		}
		else
		{
			LuaTable data = tbData.get< LuaTable >( i );
			if( false == lua_istable( data.m_obj->m_L, -1 ) )
				continue;

			GFx::Value gfxLeafArray;
			m_pGameScene->GetMovie()->CreateArray( &gfxLeafArray );

			int nLeafArrayIndex = 0;
			for( int j = 1; j <= data.TableLen(); ++j )
			{
				LuaTable tbLeafData = data.get< LuaTable >( j );
				if( false == lua_istable( tbLeafData.m_obj->m_L, -1 ) )
					continue;
				
				InsertTreeData( &gfxLeafArray, nLeafArrayIndex, vecObjectID, tbLeafData );
			}

			gfxObject.SetMember( vecObjectID[ i - 1 ].c_str(), gfxLeafArray );
		}
	}

	pParentArray->SetElement( nArrayIndex++, gfxObject );
}

//-------------------------------------------------------------------------------------------------

void GfxInterface::PreloadWidget()
{
	const MAP_WIDGET& vecWidget = GLWidgetsList::GetInstance().GetList();

	MAP_WIDGET_CITER iter = vecWidget.begin();
	for( ; iter != vecWidget.end(); ++iter )
	{
		const GLWidget& widget = iter->second;

		if( false == widget.m_bCloseToDelete )
			OpenWidget( widget.m_wWidgetID, 0, true );
	}
}

void GfxInterface::UnloadWidget( int nWidgetID, int nSubID )
{
	if( -1 == nWidgetID )
	{
		GFx::Value args;
		args.SetBoolean( true );
		Invoke( AS_CloseAllWidget, &args, 1 );
	}
	else
	{
		GFx::Value args[ 2 ];
		args[ 0 ].SetInt( nWidgetID );
		args[ 1 ].SetInt( nSubID );
		Invoke( AS_UnloadWidget, args, 2 );
	}
}

void GfxInterface::RegSlot( WORD wType, const char* strSlotID )
{
	Ptr< GfxSlotBase > pSlot = NULL;

	switch( EMGFX_SLOT_TYPE( wType ) )
	{
	case EMGFX_SLOT_ITEMBASE :
		pSlot = *new GfxItemBaseSlot;
		break;

	case EMGFX_SLOT_SKILLBASE :
		pSlot = *new GfxSkillBaseSlot;
		break;

	case EMGFX_SLOT_INVENTORY :
		pSlot = *new GfxInventorySlot;
		break;

	case EMGFX_SLOT_MINIGAME_DICE_REWARD:
		pSlot = *new GfxMiniGameDiceRewardSlot;
		break;

	case EMGFX_SLOT_TOOLTIP :
		pSlot = *new GfxTooltipSlot;
		break;

	case EMGFX_SLOT_CHARINFO :
		pSlot = *new GfxCharInfoSlot;
		break;

	case EMGFX_SLOT_INVEN_BANK :
		pSlot = *new GfxInvenBankSlot;
		break;

	case EMGFX_SLOT_INVEN_WISHLIST :
		pSlot = *new GfxInvenWishListSlot;
		break;

	case EMGFX_SLOT_INVEN_TRASH :
		pSlot = *new GfxInvenTrashSlot;
		break;

	case EMGFX_SLOT_SQS :
		pSlot = *new GfxSkillQuickSlot;
		break;

	case EMGFX_SLOT_IQS :
		pSlot = *new GfxItemQuickSlot;
		break;

	case EMGFX_SLOT_SKILL :
		pSlot = *new GfxSkillSlot;
		break;

	case EMGFX_SLOT_VEHICLE :
		pSlot = *new GfxVehicleSlot;
		break;

	case EMGFX_SLOT_NPCMARKET :
		pSlot = *new GfxNpcMarketSlot;
		break;

	case EMGFX_SLOT_NPCCART :
		pSlot = *new GfxNpcCartSlot;
		break;

	case EMGFX_SLOT_PRODUCT :
		pSlot = *new GfxProductSlot;
		break;
	
	case EMGFX_SLOT_POINTSHOP:
		pSlot = *new GfxPointShopSlot;
		break;

	case EMGFX_SLOT_POINTSHOP_CART:
		pSlot = *new GfxPointShopCartSlot;
		break;
	
	case EMGFX_SLOT_TRADE :
		pSlot = *new GfxTradeSlot;
		break;

	case EMGFX_SLOT_PMSPOINTTRADE :
		pSlot = *new GfxPMSPointTradeSlot;
		break;

	case EMGFX_SLOT_POSTBOX :
		pSlot = *new GfxPostBoxSlot;
		break;

	case EMGFX_SLOT_POSTBOX_SUB:
		pSlot = *new GfxPostBoxSubSlot;
		break;

	case EMGFX_SLOT_PET :
		pSlot = *new GfxPetSlot;
		break;

	case EMGFX_SLOT_REFERCHAR:
		pSlot = *new GfxReferCharSlot;
		break;

	case EMGFX_SLOT_CLUBSTORAGE:
		pSlot = *new GfxClubStorageSlot;
		break;

	case EMGFX_SLOT_ATTENDANCE_AWARD:
		pSlot = *new GfxAttendanceAwardSlot;
		break;

	case EMGFX_SLOT_ACTIVITY_SALES:
		pSlot = *new GfxActivitySalesSlot;
		break;

	case EMGFX_SLOT_REBUILD_CARD_ITEM:
		pSlot = *new GfxRebuildCardItemSlot;
		break;

	case EMGFX_SLOT_REBUILD_CARD_STEMP:
		pSlot = *new GfxRebuildCardStempSlot;
		break;

	case EMGFX_SLOT_ITEM_PREVIEW:
		pSlot = *new GfxItemPreviewSlot;
		break;

	case EMGFX_SLOT_PRIVATEMARKET:
		pSlot = *new GfxPrivateMarketSlot;
		break;

	case EMGFX_SLOT_LOCKER :
		pSlot = *new GfxLockerSlot;
		break;

	case EMGFX_SLOT_REFERCHAR_SKILL :
		pSlot = *new GfxReferCharSkillSlot;
		break;

	case EMGFX_SLOT_BUFF :
		pSlot = *new GfxBuffSlot;
		break;

	case EMGFX_SLOT_SETITEM :
		pSlot = *new GfxSetItemSlot;
		break;

	case EMGFX_SLOT_SUMMONSTATE:
		pSlot = *new GfxSummonStateSlot;
		break;

	case EMGFX_SLOT_PARTYDISTRIBUTION:
		pSlot = *new GfxPartyDistributionSlot;
		break;

	case EMGFX_SLOT_PRIVATEMARKETSEARCH :
		pSlot = *new GfxPrivateMarketSearchSlot;
		break;

	case EMGFX_SLOT_ITEMLINKINFO :
		pSlot = *new GfxItemLinkInfoSlot;
		break;

	default :
		return;
	}

	m_pGameScene->GetDataBinder()->RegisterBoundData( strSlotID, pSlot );
	m_vecRegSlotID.push_back( strSlotID );
}

void GfxInterface::UnregSlot( const std::string& strSlotID )
{
	m_pGameScene->GetDataBinder()->UnregisterBoundData( strSlotID.c_str() );
}

void GfxInterface::RegHtmlWebView( int nViewID )
{
	std::map< int, int >::iterator iter = m_mapHtmlWebViewID.find( nViewID );
	if( iter != m_mapHtmlWebViewID.end() )
		return;

	m_mapHtmlWebViewID.insert( std::make_pair( nViewID, COMMON_WEB::IssueNewID() ) );
}

int GfxInterface::GetHtmlWebID( int nViewID )
{
	std::map< int, int >::iterator iter = m_mapHtmlWebViewID.find( nViewID );
	if( iter == m_mapHtmlWebViewID.end() )
		return -1;

	return iter->second;
}

void GfxInterface::MoveHtmlWebView( int nViewID, bool bVisible, int nX, int nY, int nWidth, int nHeight )
{
	std::map< int, int >::iterator iter = m_mapHtmlWebViewID.find( nViewID );
	if( iter == m_mapHtmlWebViewID.end() )
		return;

	COMMON_WEB::MoveWindow( iter->second, nX, nY, nWidth, nHeight, true == bVisible ? TRUE : FALSE );
}

void GfxInterface::OpenWidget( int nWidgetID, int nSubID, bool bPreload )
{
	GFx::Value args[ 3 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( bPreload );
	Invoke( AS_OpenWidget, args, 3 );
}

void GfxInterface::CloseWidget( int nWidgetID, int nSubID )
{
	GFx::Value args[ 2 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	Invoke( AS_CloseWidget, args, 2 );
}

void GfxInterface::CloseAllWidget( bool bDelete )
{
	GfxWidgetInstanceMan::GetInstance().Clear();

	GFx::Value arg;
	arg.SetBoolean( bDelete );
	Invoke( AS_CloseAllWidget, &arg, 1 );
}

bool GfxInterface::IsOpenWidget( int nWidgetID, int nSubID )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, "" );
	if( NULL == pInstance )
		return false;

	GFx::Value::DisplayInfo info;
	pInstance->GetDisplayInfo( &info );

	return info.GetVisible();
}

bool GfxInterface::IsLoadWidget( int nWidgetID, int nSubID )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, "" );
	if( NULL == pInstance )
		return false;

	return true;
}

int GfxInterface::GetMultiWidgetCount( int nWidgetID )
{
	GFx::Value args;
	args.SetInt( nWidgetID );

	GFx::Value result;
	if( false == Invoke( AS_GetMultiWidgetCount, &args, 1, &result ) )
		return 0;

	return result.GetInt();
}

bool GfxInterface::EscWidgetClose()
{
	GFx::Value result;
	if( false == Invoke( AS_EscWidgetClose, NULL, 0, &result ) )
		return false;

	return result.GetBool();
}

bool GfxInterface::IsOpenTooltip()
{
	GFx::Value result;
	if( false == Invoke( AS_IsOpenTooltip, NULL, 0, &result ) )
		return false;

	return result.GetBool();
}

void GfxInterface::SetStageEvent( const char* strEvent, const char* strCallBack, const char* strRetParams )
{
	GFx::Value args[ 3 ];
	args[ 0 ].SetString( strEvent );
	args[ 1 ].SetString( strCallBack );
	CreateStringArray( strRetParams, &args[ 2 ], SPLIT_STRING_ETC );
	Invoke( AS_SetStageEvent, args, 3 );
}

void GfxInterface::SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int nValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		if( true == val.IsInt() )
			val.SetInt( nValue );
		else if( true == val.IsUInt() )
			val.SetUInt( (UInt32) nValue );
		else if( true == val.IsNumber() )
			val.SetNumber( (double) nValue );

		pInstance->SetMember( strProp, val );
	}
}

void GfxInterface::SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int nValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		if( true == val.IsInt() )
			val.SetInt( nValue );
		else if( true == val.IsUInt() )
			val.SetUInt( (UInt32) nValue );
		else if( true == val.IsNumber() )
			val.SetNumber( (double) nValue );

		pInstance->SetMember( strProp, val );
	}
}

void GfxInterface::SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double dValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		if( true == val.IsInt() )
			val.SetInt( (int) dValue );
		else if( true == val.IsUInt() )
			val.SetUInt( (UInt32) dValue );
		else if( true == val.IsNumber() )
			val.SetNumber( dValue );

		pInstance->SetMember( strProp, val );
	}
}

void GfxInterface::SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool bValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		/*if( false == val.IsBool() )
			return;*/

		val.SetBoolean( bValue );
		pInstance->SetMember( strProp, val );
	}
}

// void GfxInterface::SetProperty_Array( int nWidgetID, int nSubID, const char* strProp, const char* strInstanceArray, const char* strValueArray )
// {
// 	VEC_STRING vecProp, vecInst, vecValue;
// 	SplitString( vecProp, strProp, SPLIT_STRING_ETC );
// 	SplitString( vecInst, strInstanceArray, SPLIT_STRING_ETC );
// 	SplitString( vecValue, strValueArray, SPLIT_STRING_ETC );
// 	if( vecValue.size() != vecInst.size() )
// 		return;
// 	
// 	for( size_t i = 0; i < vecInst.size(); ++i )
// 	{
// 		SetProperty( nWidgetID, nSubID, vecProp[i].c_str(), vecInst[ i ].c_str(), 
// 			sc::string::ansi_to_utf8(vecValue[i]).c_str());
// 	}
// }

void GfxInterface::SetProperty_Array( int nWidgetID, int nSubID, LuaTable tbProp, LuaTable tbInstance, LuaTable tbValue, LuaTable tbValueType )
{
	const INT nSize(tbProp.TableLen());
	// if ( nSize != tbValue.TableLen() != tbValueType.TableLen() != tbInstance.TableLen() )
	if ((nSize != tbValue.TableLen())
		||(nSize != tbValueType.TableLen())
		||(nSize != tbInstance.TableLen()))
		return;

	for ( int i = 1; i < nSize+1; ++i )
	{
		const char* strProperty = tbProp.get<const char*>(i)
			,* strInstance = tbInstance.get<const char*>(i)
			,* strDataType = tbValueType.get<const char*>(i);
		if ( strDataType == NULL )
			continue;

		if ( 0 == strcmp(strDataType, "-n") )
		{
			INT nValue = tbValue.get<INT>(i);
			SetProperty(nWidgetID, nSubID, strProperty, strInstance, nValue);
		}
		else if ( 0 == strcmp(strDataType, "-dw") )
		{
			unsigned int uiValue = tbValue.get<unsigned int>(i);
			SetProperty(nWidgetID, nSubID, strProperty, strInstance, uiValue);
		}
		else if ( 0 == strcmp(strDataType, "-f") )
		{
			double dValue = tbValue.get<double>(i);
			SetProperty(nWidgetID, nSubID, strProperty, strInstance, dValue);
		}
		else if ( 0 == strcmp(strDataType, "-s") )
		{
			std::string strValue = tbValue.get<char*>(i);
			SetProperty(nWidgetID, nSubID, strProperty, strInstance, strValue.c_str());
		}
		else if ( 0 == strcmp(strDataType, "-b") )
		{
			bool bValue = tbValue.get<bool>(i);
			SetProperty(nWidgetID, nSubID, strProperty, strInstance, bValue);
		}
	}
}

void GfxInterface::SetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		std::string str = strValue;
		boost::replace_all( str, "\r\n", "\n" );

		// 기존
		std::wstring strW;
		if( EMGFX_FONT_KOREAN == m_emFontLang )
			strW = EncodeText( str );
		else
		{
			std::string strConv = GLWidgetScript::GetInstance().AnsiToUTF8(str.c_str());
			strW = sc::string::utf8ToUnicode( strConv );
		}

		val.SetStringW( strW.c_str() );
		pInstance->SetMember( strProp, val );
	}
}

void GfxInterface::SetProperty_WString( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, const char* strValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		std::string str = strValue;
		boost::replace_all( str, "\r\n", "\n" );

		// IME 입력값이 TextField에 정상 입력됨;
		std::wstring strW;
		if( EMGFX_FONT_KOREAN == m_emFontLang )
			strW = EncodeText( str );
		else
			strW = sc::string::utf8ToUnicode( str );

		val.SetStringW(strW.c_str());

		pInstance->SetMember( strProp, val );
	}
}

bool GfxInterface::GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, int& nValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return false;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		if( true == val.IsInt() )
			nValue = val.GetInt();
		else if( true == val.IsUInt() )
			nValue = (int) val.GetUInt();
		else if( true == val.IsNumber() )
			nValue = (int) val.GetNumber();
	}

	return true;
}

bool GfxInterface::GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, unsigned int& nValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return false;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		if( true == val.IsInt() )
			nValue = (int) val.GetInt();
		else if( true == val.IsUInt() )
			nValue = val.GetUInt();
		else if( true == val.IsNumber() )
			nValue = (unsigned int) val.GetNumber();
	}

	/*GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetProperty( &value, nWidgetID, nSubID, strProp, strInstance );	
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	nValue = (UInt32) value.GetNumber();*/

	return true;
}

bool GfxInterface::GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, double& dValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return false;

	GFx::Value val;
	if( true == pInstance->GetMember( strProp, &val ) )
	{
		if( true == val.IsInt() )
			dValue = (double) val.GetInt();
		else if( true == val.IsUInt() )
			dValue = (double) val.GetUInt();
		else if( true == val.IsNumber() )
			dValue = (double) val.GetNumber();
	}

	/*GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetProperty( &value, nWidgetID, nSubID, strProp, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	dValue = (double) value.GetNumber();*/

	return true;
}

bool GfxInterface::GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, bool& bValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return false;

	GFx::Value val;
	if( false == pInstance->GetMember( strProp, &val ) )
		return false;

	if( false == val.IsBool() )
		return false;

	bValue = val.GetBool();

	/*GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetProperty( &value, nWidgetID, nSubID, strProp, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	bValue = value.GetBool();*/

	return true;
}

bool GfxInterface::GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, std::string& strValue )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return false;

	GFx::Value val;
	if( false == pInstance->GetMember( strProp, &val ) )
		return false;

	if( false == val.IsString() )
		return false;

	// 기존
	if( EMGFX_FONT_KOREAN == m_emFontLang )
		strValue = sc::string::utf8_to_ansi( std::string( val.GetString() ) );
	else
		strValue = val.GetString();

	// 확인 필요
	//strValue = sc::string::utf8_to_ansi( std::string( val.GetString() ) );
	// 확인 필요2
	//strValue = val.GetString();


	/*GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetProperty( &value, nWidgetID, nSubID, strProp, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	strValue = value.GetString();*/

	return true;
}

bool GfxInterface::GetProperty( int nWidgetID, int nSubID, const char* strProp, const char* strInstance, LuaTable& tb )
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return false;

	GFx::Value val;
	if( false == pInstance->GetMember( strProp, &val ) )
		return false;

	if( false == val.IsArray() )
		return false;

	int nTableIndex = 1;
	for( int i = 0; true; ++i )
	{
		GFx::Value valEelement;
		if( false == val.GetElement( i, &valEelement ) )
			break;

		if( true == valEelement.IsUInt() )
		{
			unsigned int n = valEelement.GetUInt();
			tb.set( nTableIndex++, n );
			continue;
		}

		if( true == valEelement.IsInt() )
		{
			int n = valEelement.GetInt();
			tb.set( nTableIndex++, n );
			continue;
		}

		if( true == valEelement.IsNumber() )
		{
			double d = valEelement.GetNumber();
			tb.set( nTableIndex++, d );
			continue;
		}

		if( true == valEelement.IsBool() )
		{
			bool b = valEelement.GetBool();
			tb.set( nTableIndex++, b );
			continue;
		}

		if( true == valEelement.IsString() )
		{
			std::string str;
			if( EMGFX_FONT_KOREAN == m_emFontLang )
				str = sc::string::utf8_to_ansi( std::string( valEelement.GetString() ) );
			else
				str = valEelement.GetString();

			tb.set( nTableIndex++, str.c_str() );
			continue;
		}
	}
}

void GfxInterface::SetEvent( int nWidgetID, int nSubID, const char* strEvent, const char* strCallBack,
							 const char* strRetProp, const char* strDelivery, const char* strInstance )
{
	GFx::Value args[ 7 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strEvent );
	args[ 3 ].SetString( strCallBack );
	CreateStringArray( strRetProp, &args[ 4 ], SPLIT_STRING_ETC );

	if( NULL == strDelivery )
		args[ 5 ].SetNull();
	else
		CreateStringArray( strDelivery, &args[ 5 ], SPLIT_STRING_ETC );

	args[ 6 ].SetString( strInstance );
	Invoke( AS_SetEvent, args, 7 );
}

void GfxInterface::SetDataProvider( int nWidgetID, int nSubID, const char* strDataProvider, const char* strInstance, bool bAdd )
{
	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	CreateStringArray( strDataProvider, &args[ 2 ], SPLIT_STRING_ETC );
	args[ 3 ].SetString( strInstance );
	args[ 4 ].SetBoolean( bAdd );
	Invoke( AS_SetDataProvider, args, 5 );
}

void GfxInterface::SetDataProviderEx( int nWidgetID, int nSubID, const char* strObjectID, 
									  const char* strDataProvider, const char* strInstance, bool bAdd )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, strObjectID, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return;

	VEC_STRING vecDataProvider;
	SplitString( vecDataProvider, strDataProvider, SPLIT_STRING_ETC );

	if( true == vecDataProvider.empty() )
		return;

	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	m_pGameScene->GetMovie()->CreateArray( &args[ 2 ] );
	args[ 3 ].SetString( strInstance );
	args[ 4 ].SetBoolean( bAdd );

	int nDataProviderElement = 0;
	int nDataProviderIndex = 0;
	while( true )
	{
		bool bPushObject = false;

		GFx::Value gfxObject;
		m_pGameScene->GetMovie()->CreateObject( &gfxObject );

		for( size_t i = 0; i < vecObjectID.size(); ++i )
		{
			if( nDataProviderIndex < static_cast< int >( vecDataProvider.size() ) )
			{
				bPushObject = true;

				std::wstring strW;
				if( EMGFX_FONT_KOREAN == m_emFontLang )
					strW = EncodeText( vecDataProvider[ nDataProviderIndex ] );
				else
				{
					std::string str = GLWidgetScript::GetInstance().AnsiToUTF8(vecDataProvider[ nDataProviderIndex ].c_str());
					strW = sc::string::utf8ToUnicode( str );
					//strW = sc::string::utf8ToUnicode( vecDataProvider[ nDataProviderIndex ] );
				}
				gfxObject.SetMember( vecObjectID[ i ].c_str(), strW.c_str() );
				++nDataProviderIndex;
			}
		}

		if( true == bPushObject )
		{
			args[ 2 ].SetElement( nDataProviderElement, gfxObject );
			++nDataProviderElement;
		}

		if( nDataProviderIndex >= static_cast< int >( vecDataProvider.size() ) )
			break;
	}

	Invoke( AS_SetDataProvider, args, 5 );
}

void GfxInterface::SetDataProvider_Terms(int nWidgetID, int nSubID
						   , const char* strInstName
						   , LuaTable tbWhereProvName, LuaTable tbWhereProvData, LuaTable tbWhereProvType
						   , LuaTable tbNewProvName, LuaTable tbNewProvData, LuaTable tbNewProvType)
{
	INT nWhereSize(tbWhereProvName.TableLen()), nNewSize(tbNewProvName.TableLen());
	if ( (nWhereSize != tbWhereProvData.TableLen()) ||
		 (nWhereSize != tbWhereProvType.TableLen()) ||
		 (nNewSize != tbNewProvData.TableLen() ) ||
		 (nNewSize != tbNewProvType.TableLen()))
		 return;

	GFx::Value args[9];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strInstName );
	m_pGameScene->GetMovie()->CreateArray( &args[ 3 ] );
	m_pGameScene->GetMovie()->CreateArray( &args[ 4 ] );
	m_pGameScene->GetMovie()->CreateArray( &args[ 5 ] );
	m_pGameScene->GetMovie()->CreateArray( &args[ 6 ] );
	m_pGameScene->GetMovie()->CreateArray( &args[ 7 ] );
	m_pGameScene->GetMovie()->CreateArray( &args[ 8 ] );
	// 조건 세팅;
 	for ( INT i = 1; i < nWhereSize+1; ++i )
 	{
 		GFx::Value gfxName, gfxValue, gfxType;
 		const char* strType = tbWhereProvType.get<const char*>(i);
 		if ( 0 == strcmp(strType, "-n") )
 		{
 			INT nValue = tbWhereProvData.get<INT>(i);
 			gfxValue.SetInt(nValue);
 		}
 		else if ( 0 == strcmp(strType, "-dw") )
 		{
 			unsigned int uiValue = tbWhereProvData.get<unsigned int>(i);
 			gfxValue.SetUInt(uiValue);
 		}
 		else if ( 0 == strcmp(strType, "-f") )
 		{
 			double dValue = tbWhereProvData.get<double>(i);
 			gfxValue.SetNumber(dValue);
 		}
 		else if ( 0 == strcmp(strType, "-s") )
 		{
 			std::wstring strValue = EncodeText( tbWhereProvData.get<char*>(i) );
 			gfxValue.SetStringW(strValue.c_str());
 		}
 		else if ( 0 == strcmp(strType, "-b") )
 		{
 			bool bValue = tbWhereProvData.get<bool>(i);
 			gfxValue.SetBoolean(bValue);
 		}

 		gfxName.SetString(tbWhereProvName.get<const char*>(i));
 		gfxType.SetString(strType);
 		args[3].PushBack(gfxName);
 		args[4].PushBack(gfxValue);
 		args[5].PushBack(gfxType);
 	}
 
 	// 변경시킬 값 세팅;
 	// 조건 세팅;
 	for ( INT i = 1; i < nNewSize+1; ++i )
 	{
 		GFx::Value gfxName, gfxValue, gfxType;
		//m_pGfx->GetMovie()->CreateObject(&gfxValue);
 		const char* strType = tbNewProvType.get<const char*>(i);
		const char* strName = tbNewProvName.get<const char*>(i);
		std::string strValue;
		gfxName.SetString(strName);
		gfxType.SetString(strType);
		args[6].PushBack(gfxName);
		args[8].PushBack(gfxType);

		// 깊은 복사 문제가 있어서 데이터 세팅시 if문 내부에서 하도록 한다;
 		if ( 0 == strcmp(strType, "-n") )
 		{
 			INT nValue = tbNewProvData.get<INT>(i);
 			gfxValue.SetInt(nValue);
			args[7].PushBack(gfxValue);
 		}
 		else if ( 0 == strcmp(strType, "-dw") )
 		{
 			unsigned int uiValue = tbNewProvData.get<unsigned int>(i);
 			gfxValue.SetUInt(uiValue);
			args[7].PushBack(gfxValue);
 		}
 		else if ( 0 == strcmp(strType, "-f") )
 		{
 			double dValue = tbNewProvData.get<double>(i);
 			gfxValue.SetNumber(dValue);
			args[7].PushBack(gfxValue);
 		}
 		else if ( 0 == strcmp(strType, "-s") )
 		{
			std::wstring strValue = EncodeText( tbNewProvData.get<const char*>(i) );
 			gfxValue.SetStringW(strValue.c_str());
			args[7].PushBack(gfxValue);
 		}
 		else if ( 0 == strcmp(strType, "-b") )
 		{
 			bool bValue = tbNewProvData.get<bool>(i);
 			gfxValue.SetBoolean(bValue);
			args[7].PushBack(gfxValue);
 		}
		else if ( 0 == strcmp(strType, "leaf"))
		{
			LuaTable tbLeafTable = tbNewProvData.get<LuaTable>(i);
			if( false == lua_istable( tbLeafTable.m_obj->m_L, -1 ) )
				continue;

			// 1 - objectid, 2 - data table;
			LuaTable tbObjectID = tbLeafTable.get<LuaTable>(1);
			LuaTable tbLeafData = tbLeafTable.get<LuaTable>(2);

			VEC_STRING vecObjectID;
			for( int k = 1; k <= tbObjectID.TableLen(); ++k )
			{
				vecObjectID.push_back(tbObjectID.get<const char*>(k));
			}

			int nArrayIndex = 0;
			m_pGameScene->GetMovie()->CreateArray(&gfxValue);
			for ( INT j = 1; j <= tbLeafData.TableLen(); ++j )
			{
				LuaTable data = tbLeafData.get< LuaTable >( j );
				if( false == lua_istable( data.m_obj->m_L, -1 ) )
				{
					CString strLog;
					strLog.Format( "%d index data is not a table.", i );
					GLWidgetScript::GetInstance().PrintDebug( strLog.GetBuffer() );
					continue;
				}
				InsertTreeData( &gfxValue, nArrayIndex, vecObjectID, data );
			}
			args[7].PushBack(gfxValue);
		}
	}

	Invoke( AS_SetDataProvider_Terms, args, 9 );
}

void GfxInterface::SetDataProviderComplex( int nWidgetID, int nSubID, const char* strObjectID, LuaTable tbData,
										   const char* strInstance, bool bAdd )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, strObjectID, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return;

	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	m_pGameScene->GetMovie()->CreateArray( &args[ 2 ] );
	args[ 3 ].SetString( strInstance );
	args[ 4 ].SetBoolean( bAdd );

	int nArrayIndex = 0;
	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		LuaTable data = tbData.get< LuaTable >( i );
		if( false == lua_istable( data.m_obj->m_L, -1 ) )
		{
			CString strLog;
			strLog.Format( "%d index data is not a table.", i );
			GLWidgetScript::GetInstance().PrintDebug( strLog.GetBuffer() );
			continue;
		}

		InsertTreeData( &args[ 2 ], nArrayIndex, vecObjectID, data );
	}

	Invoke( AS_SetDataProvider, args, 5 );
}

void GfxInterface::RemoveDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strInstance, bool bAll )
{
	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nIndex );
	args[ 3 ].SetString( strInstance );
	args[ 4 ].SetBoolean( bAll );
	Invoke( AS_RemoveDataProvider, args, 5 );
}

bool GfxInterface::GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int& nValue )
{
	GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetDataProvider( &value, nWidgetID, nSubID, nIndex, strName, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	switch( emType )
	{
	case EMGFX_OBJ_TYPE_INT :
		nValue = (int) value.GetInt();
		break;

	case EMGFX_OBJ_TYPE_UINT :
		nValue = (int) value.GetUInt();
		break;

	case EMGFX_OBJ_TYPE_NUMBER :
		nValue = (int) value.GetNumber();
		break;

	case EMGFX_OBJ_TYPE_BOOL :
		nValue = true == value.GetBool() ? 1 : 0;
		break;

	case EMGFX_OBJ_TYPE_STRING :
		nValue = atoi( value.GetString() );
		break;
	}

	return true;
}

bool GfxInterface::GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int& nValue )
{
	GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetDataProvider( &value, nWidgetID, nSubID, nIndex, strName, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	switch( emType )
	{
	case EMGFX_OBJ_TYPE_INT :
		nValue = (unsigned int) value.GetInt();
		break;

	case EMGFX_OBJ_TYPE_UINT :
		nValue = (unsigned int) value.GetUInt();
		break;

	case EMGFX_OBJ_TYPE_NUMBER :
		nValue = (unsigned int) value.GetNumber();
		break;

	case EMGFX_OBJ_TYPE_BOOL :
		nValue = true == value.GetBool() ? 1 : 0;
		break;

	case EMGFX_OBJ_TYPE_STRING :
		nValue = (unsigned int) atoi( value.GetString() );
		break;
	}

	return true;
}

bool GfxInterface::GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double& dValue )
{
	GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetDataProvider( &value, nWidgetID, nSubID, nIndex, strName, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	switch( emType )
	{
	case EMGFX_OBJ_TYPE_INT :
		dValue = (double) value.GetInt();
		break;

	case EMGFX_OBJ_TYPE_UINT :
		dValue = (double) value.GetUInt();
		break;

	case EMGFX_OBJ_TYPE_NUMBER :
		dValue = (double) value.GetNumber();
		break;

	case EMGFX_OBJ_TYPE_BOOL :
		dValue = true == value.GetBool() ? 1.0 : 0.0;
		break;

	case EMGFX_OBJ_TYPE_STRING :
		dValue = atof( value.GetString() );
		break;
	}

	return true;
}

bool GfxInterface::GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool& bValue )
{
	GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetDataProvider( &value, nWidgetID, nSubID, nIndex, strName, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	switch( emType )
	{
	case EMGFX_OBJ_TYPE_INT :
		bValue = 0 == value.GetInt() ? false : true;
		break;

	case EMGFX_OBJ_TYPE_UINT :
		bValue = 0 == value.GetUInt() ? false : true;
		break;

	case EMGFX_OBJ_TYPE_NUMBER :
		bValue = 0.0 == value.GetNumber() ? false : true;
		break;

	case EMGFX_OBJ_TYPE_BOOL :
		bValue = value.GetBool();
		break;

	case EMGFX_OBJ_TYPE_STRING :
		{
			const char* szBool = value.GetString();
			if( 0 == strcmp( szBool, "false" ) )
				bValue = false;
			else
				bValue = true;
		}
		break;
	}

	return true;
}

bool GfxInterface::GetDataProvider( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, std::string& strValue )
{
	GFx::Value value;

	EMGFX_OBJECT_TYPE emType = PreGetDataProvider( &value, nWidgetID, nSubID, nIndex, strName, strInstance );
	if( EMGFX_OBJ_TYPE_NULL == emType )
		return false;

	switch( emType )
	{
	case EMGFX_OBJ_TYPE_INT :
		{
			CString str;
			str.Format( "%d", (int) value.GetInt() );
			strValue = str.GetBuffer();
		}
		break;

	case EMGFX_OBJ_TYPE_UINT :
		{
			CString str;
			str.Format( "%d", (unsigned int) value.GetUInt() );
			strValue = str.GetBuffer();
		}
		break;

	case EMGFX_OBJ_TYPE_NUMBER :
		{
			CString str;
			str.Format( "%f", (double) value.GetNumber() );
			strValue = str.GetBuffer();
		}
		break;

	case EMGFX_OBJ_TYPE_BOOL :
		strValue = true == value.GetBool() ? "true" : "false";
		break;

	case EMGFX_OBJ_TYPE_STRING :
		strValue = value.GetString();
		break;
	}

	return true;
}

bool GfxInterface::GetDataProvider_Array( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, LuaTable& tbResult )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, strName, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return false;

	GFx::Value args[5];
	args[0].SetInt(nWidgetID);
	args[1].SetInt(nSubID);
	args[2].SetInt(nIndex);
	CreateStringArray( strName, &args[ 3 ], SPLIT_STRING_ETC );
	args[ 4 ].SetString( strInstance );

	GFx::Value gfxResult, gfxType, gfxValue;
	if( false == Invoke( AS_GetDataProviderArray, args, 5, &gfxResult ) )
		return false;

	switch ( gfxResult.GetType() )
	{
	case GFx::Value::VT_Undefined:
	case GFx::Value::VT_Null:
		return false;
	}

	{
		//LuaTable tbTable(GLWidgetScript::GetInstance().GetLuaState());
		//Scaleform::GFx::Value gfxType, gfxValue;
		int nItemCount = 1;
		for ( unsigned i = 0; i < gfxResult.GetArraySize(); i+=2 )
		{
			gfxResult.GetElement(i, &gfxType);
			gfxResult.GetElement(i+1, &gfxValue);
			switch( gfxType.GetInt() )
			{
			case EMGFX_OBJ_TYPE_INT :		tbResult.set(nItemCount, (int)gfxValue.GetInt() ); break;
			case EMGFX_OBJ_TYPE_UINT :		tbResult.set(nItemCount, (unsigned int)gfxValue.GetInt() ); break;
			case EMGFX_OBJ_TYPE_NUMBER :	tbResult.set(nItemCount, (double)gfxValue.GetInt() ); break;
			case EMGFX_OBJ_TYPE_BOOL :		tbResult.set(nItemCount, (bool)gfxValue.GetBool()); break;
			case EMGFX_OBJ_TYPE_STRING :	
				{
					std::string strValue(gfxValue.GetString());
					tbResult.set(nItemCount, sc::string::utf8_to_ansi(strValue).c_str() ); 
				}
				break;
			}
			nItemCount++;
		}
	}

	return true;
}

void GfxInterface::SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, int nValue )
{
	GFx::Value args[ 6 ];

	PreSetDataProviderProp( args, nWidgetID, nSubID, nIndex, strName, strInstance );
	args[ 4 ].SetInt( nValue );
	Invoke( AS_SetDataProviderProp, args, 6 );
}

void GfxInterface::SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, unsigned int nValue )
{
	GFx::Value args[ 6 ];

	PreSetDataProviderProp( args, nWidgetID, nSubID, nIndex, strName, strInstance );
	args[ 4 ].SetUInt( nValue );
	Invoke( AS_SetDataProviderProp, args, 6 );
}

void GfxInterface::SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, double dValue )
{
	GFx::Value args[ 6 ];

	PreSetDataProviderProp( args, nWidgetID, nSubID, nIndex, strName, strInstance );
	args[ 4 ].SetNumber( dValue );
	Invoke( AS_SetDataProviderProp, args, 6 );
}

void GfxInterface::SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, bool bValue )
{
	GFx::Value args[ 6 ];

	PreSetDataProviderProp( args, nWidgetID, nSubID, nIndex, strName, strInstance );
	args[ 4 ].SetBoolean( bValue );
	Invoke( AS_SetDataProviderProp, args, 6 );
}

void GfxInterface::SetDataProviderProp( int nWidgetID, int nSubID, int nIndex, const char* strName, const char* strInstance, const char* strValue )
{
	GFx::Value args[ 6 ];

	PreSetDataProviderProp( args, nWidgetID, nSubID, nIndex, strName, strInstance );

	std::wstring strW = EncodeText( strValue );
	args[ 4 ].SetStringW( strW.c_str() );

	Invoke( AS_SetDataProviderProp, args, 6 );
}

void GfxInterface::SetListItemProp_Array( int nWidgetID
										 , int nSubID
										 , char* strIndexArr
										 , const char* strInstNameArr
										 , const char* strPropArr
										 , const char* strValueArr
										 , const char* strTypeArr
										 , const char* strInstance )
{
	VEC_STRING vecIndex, vecInstName, vecProp, vecValue, vecType;
	SplitString( vecIndex,		strIndexArr,		SPLIT_STRING_ETC );
	SplitString( vecInstName,	strInstNameArr,		SPLIT_STRING_ETC );
	SplitString( vecProp,		strPropArr,			SPLIT_STRING_ETC );
	SplitString( vecValue,		strValueArr,		SPLIT_STRING_ETC );
	SplitString( vecType,		strTypeArr,			SPLIT_STRING_ETC );

	//resut(pInstance) 
	// 위처럼해보자
	// 아래 처럼 생성하면 pObjectInterface == null 이라서 pushback이안된다;

	

	const int nSize(static_cast<int>(vecIndex.size()));
	GFx::Value result, arrValue;
	m_pGameScene->GetMovie()->CreateArray(&result);
	m_pGameScene->GetMovie()->CreateArray(&arrValue);
	GFx::Value data[2];
	data[0].SetInt(nWidgetID);
	data[1].SetInt(nSubID);
	
	result.PushBack(data[0]);
	result.PushBack(data[1]);

	
	for ( int i = 0; i < nSize; ++i )
	{
		//GFx::Value args[ 7 ];
		//PreSetListItemProp(args, nWidgetID, nSubID
		//	, atoi(vecInstName[i].c_str())
		//	, vecInstName[i].c_str()
		//	, vecProp[i].c_str()
		//	, strInstance);

		GFx::Value gfxElement;
		m_pGameScene->GetMovie()->CreateArray(&gfxElement);
		GFx::Value gfxValue;
		gfxValue.SetInt(atoi(vecIndex[i].c_str()));
		gfxElement.PushBack(gfxValue);
		gfxValue.SetString(vecInstName[i].c_str());
		gfxElement.PushBack(gfxValue);
		gfxValue.SetString(vecProp[i].c_str());
		gfxElement.PushBack(gfxValue);

		{
			if( 0 == strncmp( vecType[i].c_str(), "-n", 2 ) )
			{
				gfxValue.SetInt(atoi(vecValue[i].c_str()));//args[5].SetInt(atoi(vecValue[i].c_str()));
			}
			else if( 0 == strncmp( vecType[i].c_str(), "-dw", 3 ) )
			{
				gfxValue.SetUInt(atoi(vecValue[i].c_str()));//args[5].SetUInt(atoi(vecValue[i].c_str()));
			}
			else if( 0 == strncmp( vecType[i].c_str(), "-f", 2 ) )
			{
				gfxValue.SetNumber(atof(vecValue[i].c_str()));//args[5].SetNumber(atof(vecValue[i].c_str()));
			}
			else if( 0 == strncmp( vecType[i].c_str(), "-s", 2 ) )
			{
				std::wstring strW = EncodeText( vecValue[ i ] );
				gfxValue.SetStringW( strW.c_str() );
			}
			else if( 0 == strncmp( vecType[i].c_str(), "-b", 2 ) )
			{
				gfxValue.SetBoolean(atoi(vecValue[i].c_str()));//args[5].SetBoolean(atoi(vecValue[i].c_str()));
			}
			else
			{
			}
			gfxElement.PushBack(gfxValue);
		}
		gfxValue.SetString( strInstance );
		gfxElement.PushBack(gfxValue);

		arrValue.PushBack(gfxElement);
	}
	
	result.PushBack(arrValue);
	Invoke( AS_SetListItemPropArray, &result, 1 );
}

void GfxInterface::SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
								    int nValue, const char* strInstance )
{
	GFx::Value args[ 7 ];
	PreSetListItemProp( args, nWidgetID, nSubID, nIndex, strInstName, strProp, strInstance );
	args[ 5 ].SetInt( nValue );
	Invoke( AS_SetListItemProp, args, 7 );
}

void GfxInterface::SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp,
									unsigned int nValue, const char* strInstance )
{
	GFx::Value args[ 7 ];
	PreSetListItemProp( args, nWidgetID, nSubID, nIndex, strInstName, strProp, strInstance );
	args[ 5 ].SetUInt( nValue );
	Invoke( AS_SetListItemProp, args, 7 );
}

void GfxInterface::SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
								    double dValue, const char* strInstance )
{
	GFx::Value args[ 7 ];
	PreSetListItemProp( args, nWidgetID, nSubID, nIndex, strInstName, strProp, strInstance );
	args[ 5 ].SetNumber( dValue );
	Invoke( AS_SetListItemProp, args, 7 );
}

void GfxInterface::SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
								    bool bValue, const char* strInstance )
{
	GFx::Value args[ 7 ];
	PreSetListItemProp( args, nWidgetID, nSubID, nIndex, strInstName, strProp, strInstance );
	args[ 5 ].SetBoolean( bValue );
	Invoke( AS_SetListItemProp, args, 7 );
}

void GfxInterface::SetListItemProp( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strProp, 
								    const char* strValue, const char* strInstance )
{
	GFx::Value args[ 7 ];
	PreSetListItemProp( args, nWidgetID, nSubID, nIndex, strInstName, strProp, strInstance );

	std::wstring strW = EncodeText( strValue );
	args[ 5 ].SetStringW( strW.c_str() );
	Invoke( AS_SetListItemProp, args, 7 );
}

void GfxInterface::CallListItemFunc( int nWidgetID, int nSubID, int nIndex, const char* strInstName, const char* strFunc,
									 const char* strParams, const char* strInstance )
{
	GFx::Value args[ 7 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nIndex );
	args[ 3 ].SetString( strInstName );
	args[ 4 ].SetString( strFunc );
	CreateStringArray( strParams, &args[ 5 ], SPLIT_STRING_ETC );
	args[ 6 ].SetString( strInstance );
	Invoke( AS_CallListItemFunc, args, 7 );
}

void GfxInterface::CallFunction( int nWidgetID, int nSubID, const char* strName, const char* strParams, const char* strInstance )
{
	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strName );
	CreateStringArray( strParams, &args[ 3 ], SPLIT_STRING_ETC );
	args[ 4 ].SetString( strInstance );
	Invoke( AS_CallFunction, args, 5 );
}

void GfxInterface::SetFocus( int nWidgetID, int nSubID, const char* strInstance )
{
	GFx::Value args[ 3 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strInstance );
	Invoke( AS_SetFocus, args, 3 );
}

void GfxInterface::SetTextFormat( int nWidgetID, int nSubID, DWORD dwColor, bool bUnderline,
								  int nBeginIndex, int nEndIndex, const char* strInstance )
{
	GFx::Value args[ 7 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetUInt( dwColor );
	//pArgs[ 2 ].SetBoolean( bBold );
	//pArgs[ 3 ].SetBoolean( bItalic );
	args[ 3 ].SetBoolean( bUnderline );
	args[ 4 ].SetInt( nBeginIndex );
	args[ 5 ].SetInt( nEndIndex );
	args[ 6 ].SetString( strInstance );
	Invoke( AS_SetTextFormat, args, 7 );
}

void GfxInterface::SetTextFormatSize( int nWidgetID, int nSubID, DWORD dwSize, const char* strInstance )
{
	GFx::Value args[ 4 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetUInt( dwSize );
	args[ 3 ].SetString( strInstance );
	Invoke( AS_SetTextFormatSize, args, 4 );
}

void GfxInterface::SetTween( int nWidgetID, int nSubID, int nTime, int nDelay, int nEase, bool bLoop, const char* strComplete,
							 const char* strProp, const char* strValue, const char* strInstance )
{
	VEC_STRING vecProp, vecValue;
	SplitString( vecProp, strProp, SPLIT_STRING_ETC );
	SplitString( vecValue, strValue, SPLIT_STRING_ETC );
	
	if( vecProp.size() != vecValue.size() )
	{
		GLWidgetScript::GetInstance().PrintDebug( "SetTween - Mismatch Prop & Value" );
		return;
	}

	GFx::Value args[ 9 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nTime );
	args[ 3 ].SetInt( nDelay );
	args[ 4 ].SetInt( nEase );
	args[ 5 ].SetBoolean( bLoop );
	args[ 6 ].SetString( strComplete );
	m_pGameScene->GetMovie()->CreateObject( &args[ 7 ] );
	args[ 8 ].SetString( strInstance );

	for( WORD i = 0; i < vecProp.size(); ++i )
	{
		std::wstring strW = EncodeText( vecValue[ i ] );

		GFx::Value value;
		value.SetStringW( strW.c_str() );

		args[ 7 ].SetMember( vecProp[ i ].c_str(), value );
	}

	Invoke( AS_SetTween, args, 9 );
}

void GfxInterface::RemoveTween( int nWidgetID, int nSubID, const char* strInstance )
{
	GFx::Value args[ 3 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strInstance );
	Invoke( AS_RemoveTween, args, 3 );
}

void GfxInterface::HitTestEnable( int nWidgetID, int nSubID, bool bEnable, const char* strInstance )
{
	GFx::Value args[ 4 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetBoolean( !bEnable );
	args[ 3 ].SetString( strInstance );

	Invoke( AS_HitTestEnable, args, 4 );
}

void GfxInterface::GetSlotTooltipPosSize( int& nX, int& nY, int& nWidth, int& nHeight )
{
	GFx::Value result;
	Invoke( AS_GetSlotTooltipPosSize, NULL, 0, &result );

	if( false == result.IsArray() )
		return;

	GFx::Value args[ 4 ];
	for( int i = 0; i < 4; ++i )
		result.GetElement( i, &args[ i ] );

	nX = (int) args[ 0 ].GetNumber();
	nY = (int) args[ 1 ].GetNumber();
	nWidth = (int) args[ 2 ].GetNumber();
	nHeight = (int) args[ 3 ].GetNumber();
}

void GfxInterface::ResetWidgetPosition()
{
	Invoke( AS_ResetWidgetPosition );
}

void GfxInterface::SetSpeechBubble(int nWidgetID, int nSubID, const char* strInstance, const char* strText)
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value args[ 4 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strInstance );
	args[ 3 ].SetString( strText );
	Invoke( AS_SetSpeechBubble, args, 4 );
}

void GfxInterface::DelSpeechBubble(int nWidgetID, int nSubID, const char* strInstance)
{
	GFx::Value* pInstance = FindInstance( nWidgetID, nSubID, strInstance );
	if( NULL == pInstance )
		return;

	GFx::Value args[ 3 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( strInstance );
	Invoke( AS_DelSpeechBubble, args, 3 );
}

void GfxInterface::SetFakeModal( bool bValue )
{
	GFx::Value arg;
	arg.SetBoolean( bValue );
	Invoke( AS_SetFakeModal, &arg, 1 );
}

void GfxInterface::SnapCheck( int nWidgetID, int nSubID, int nX, int nY, int nWidth, int nHeight, int& nRetX, int& nRetY )
{
	GFx::Value args[ 6 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nX );
	args[ 3 ].SetInt( nY );
	args[ 4 ].SetInt( nWidth );
	args[ 5 ].SetInt( nHeight );

	GFx::Value result;
	if( true == Invoke( AS_SnapCheck, args, 6, &result ) &&
		true == result.IsArray() )
	{
		GFx::Value arr[ 2 ];
		result.GetElement( 0, &arr[ 0 ] );
		result.GetElement( 1, &arr[ 1 ] );

		if( true == arr[ 0 ].IsInt() )
			nRetX = arr[ 0 ].GetInt();
		else if( true == arr[ 0 ].IsUInt() )
			nRetX = (int) arr[ 0 ].GetUInt();
		else if( true == arr[ 0 ].IsNumber() )
			nRetX = (int) arr[ 0 ].GetNumber();

		if( true == arr[ 1 ].IsInt() )
			nRetY = arr[ 1 ].GetInt();
		else if( true == arr[ 1 ].IsUInt() )
			nRetY = (int) arr[ 1 ].GetUInt();
		else if( true == arr[ 1 ].IsNumber() )
			nRetY = (int) arr[ 1 ].GetNumber();
	}
}

void GfxInterface::SetUIAlpha( float fAlpha )
{
	GFx::Value arg, result;
	arg.SetNumber( fAlpha );

	Invoke( AS_SetUIAlpha, &arg, 1, &result );
}

void GfxInterface::UpdateClubMark( int nWidgetID, int nSubID, const char* szParam, const char* szInst )
{
	if( NULL == szParam || NULL == szInst )
		return;

	SUpdateClubMark sMark;
	sMark.nWidgetID = nWidgetID;
	sMark.nSubID = nSubID;
	sMark.strParam = szParam;
	sMark.strInst = szInst;
	m_queUpdateClubMark.push( sMark );
}

void GfxInterface::UpdateHoldSlot()
{
	holditem::HOLD_PLACE_TYPE eHoldType = holditem::WhenHoldItem();
	if( holditem::INVENTORY != eHoldType )
		return;

	SINVENITEM* pInvenItem = m_pGaeaClient->GetCharacter()->m_cInventory.GetItem( holditem::GetHoldPosX(), holditem::GetHoldPosY() );
	if( NULL == pInvenItem )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		m_pCore->GetExternalInterface()->GetGameDataBinder()->GetDataWrapper( holditem::GetHoldSlotBinding().c_str() ) );
	if( NULL == pSlot )
		return;

	GFx::Value valData;
	switch( pSlot->GetBaseType() )
	{
	case EMGFX_SLOT_ITEMBASE :
		{
			NSGFX_SLOT::CreateSlot_Item( m_pGameScene->GetMovie(), &valData, pInvenItem->CustomItem() );
		}
		break;

	default :
		return;
	}

	GFx::Value result;
	Invoke( AS_UpdateDraggingSlot, &valData, 1, &result );
}

void GfxInterface::SetFoward( int nWidgetID, int nSubID )
{
	GFx::Value args[ 2 ], result;
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	Invoke( AS_SetFoward, args, 2, &result );
}

void GfxInterface::SetHeadData( int nWidgetID, int nSubID, const char* strObjectID,
							    LuaTable tbData, const char* strInstance )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, strObjectID, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return;

	GFx::Value args[ 4 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	m_pGameScene->GetMovie()->CreateArray( &args[ 2 ] );
	args[ 3 ].SetString( strInstance );

	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		LuaTable tbData2 = tbData.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbData2 ) )
			return;

		GFx::Value gfxObject;
		m_pGameScene->GetMovie()->CreateObject( &gfxObject );

		for( int j = 1; j <= tbData2.TableLen(); ++j )
		{
			const char* sz = tbData2.get< const char* >( j );
			if( NULL == sz )
				return;

			std::wstring strW = EncodeText( sz );
			gfxObject.SetMember( vecObjectID[ j - 1 ].c_str(), strW.c_str() );
		}

		args[ 2 ].SetElement( i - 1, gfxObject );
	}

	Invoke( AS_SetHeadData, args, 4 );
}

void GfxInterface::GetHeadData( int nWidgetID, int nIndex, const char* szInst, int nSubID, LuaTable& tb )
{
	GFx::Value args[ 4 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nIndex );
	args[ 3 ].SetString( szInst );

	GFx::Value result;
	if( false == Invoke( AS_GetHeadData, args, 4, &result ) )
		return;

	if( true == result.IsNull() )
		return;

	//var sendData:Object = new Object();
	//sendData.type = Util.GetObjectType( data[ name ] );
	//sendData.name = name as String;
	//sendData.value = data[ name ];
	//ret.push( sendData );

	for( int i = 0; true; ++i )
	{
		GFx::Value valData;
		if( false == result.GetElement( i, &valData ) )
			break;

		if( false == valData.IsObject() )
			continue;

		GFx::Value val[ 3 ];
		valData.GetMember( "type", &val[ 0 ] );
		valData.GetMember( "name", &val[ 1 ] );
		valData.GetMember( "value", &val[ 2 ] );

		switch( val[ 0 ].GetInt() )
		{
		case EMGFX_OBJ_TYPE_INT :
			{
				int n = val[ 2 ].GetInt();
				tb.set( val[ 1 ].GetString(), n );
			}
			break;

		case EMGFX_OBJ_TYPE_UINT :
			{
				unsigned int n = val[ 2 ].GetUInt();
				tb.set( val[ 1 ].GetString(), n );
			}
			break;

		case EMGFX_OBJ_TYPE_NUMBER :
			{
				double d = val[ 2 ].GetNumber();
				tb.set( val[ 1 ].GetString(), d );
			}
			break;

		case EMGFX_OBJ_TYPE_BOOL :
			{
				bool b = val[ 2 ].GetBool();
				tb.set( val[ 1 ].GetString(), b );
			}
			break;

		case EMGFX_OBJ_TYPE_STRING :
			{
				std::string str = val[ 2 ].GetString();
				if( EMGFX_FONT_KOREAN == m_emFontLang )
					str = sc::string::utf8_to_ansi( str );

				tb.set( val[ 1 ].GetString(), str.c_str() );
			}
			break;
		}
	}
}

void GfxInterface::ChangeHeadData( int nWidgetID, int nIndex, const char* szObjectID, 
								   LuaTable tbData, const char* szInst, int nSubID )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, szObjectID, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return;

	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetInt( nIndex );
	m_pGameScene->GetMovie()->CreateObject( &args[ 3 ] );
	args[ 4 ].SetString( szInst );

	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		const char* sz = tbData.get< const char* >( i );
		if( NULL == sz )
			return;

		std::wstring strW = EncodeText( sz );
		args[ 3 ].SetMember( vecObjectID[ i - 1 ].c_str(), strW.c_str() );
	}

	Invoke( AS_ChangeHeadData, args, 5 );
}

void GfxInterface::SetTreeData( int nWidgetID, int nSubID, LuaTable tbKey, const char* strObjectID, 
							    LuaTable tbData, const char* strInstance )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, strObjectID, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return;

	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	m_pGameScene->GetMovie()->CreateArray( &args[ 2 ] );
	m_pGameScene->GetMovie()->CreateArray( &args[ 3 ] );
	args[ 4 ].SetString( strInstance );

	for( int i = 1; i <= tbKey.TableLen(); ++i )
	{
		const char* sz = tbKey.get< const char* >( i );
		if( NULL == sz )
			return;

		GFx::Value val;
		val.SetString( sz );

		args[ 2 ].SetElement( i - 1, val );
	}

	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		LuaTable tbData2 = tbData.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbData2 ) )
			return;

		GFx::Value gfxArray;
		m_pGameScene->GetMovie()->CreateArray( &gfxArray );
		
		for( int j = 1; j <= tbData2.TableLen(); ++j )
		{
			LuaTable tbData3 = tbData2.get< LuaTable >( j );
			if( false == IS_LUATABLE( tbData3 ) )
				return;

			GFx::Value gfxObject;
			m_pGameScene->GetMovie()->CreateObject( &gfxObject );

			for( int k = 1; k <= tbData3.TableLen(); ++k )
			{
				const char* sz = tbData3.get< const char* >( k );
				if( NULL == sz )
					return;

				std::wstring strW = EncodeText( sz );
				gfxObject.SetMember( vecObjectID[ k - 1 ].c_str(), strW.c_str() );
			}

			gfxArray.SetElement( j - 1, gfxObject );
		}

		args[ 3 ].SetElement( i - 1, gfxArray );
	}

	Invoke( AS_SetTreeData, args, 5 );
}

void GfxInterface::GetTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szInst, int nSubID, LuaTable& tb )
{
	GFx::Value args[ 5 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( szKey );
	args[ 3 ].SetInt( nIndex );
	args[ 4 ].SetString( szInst );

	GFx::Value result;
	if( false == Invoke( AS_GetTreeData, args, 5, &result ) )
		return;

	if( true == result.IsNull() )
		return;

	//var sendData:Object = new Object();
	//sendData.type = Util.GetObjectType( data[ name ] );
	//sendData.name = name;
	//sendData.value = data[ name ];
	//ret.push( sendData );

	for( int i = 0; true; ++i )
	{
		GFx::Value valData;
		if( false == result.GetElement( i, &valData ) )
			break;

		if( false == valData.IsObject() )
			continue;

		GFx::Value val[ 3 ];
		valData.GetMember( "type", &val[ 0 ] );
		valData.GetMember( "name", &val[ 1 ] );
		valData.GetMember( "value", &val[ 2 ] );

		switch( val[ 0 ].GetInt() )
		{
		case EMGFX_OBJ_TYPE_INT :
			{
				int n = val[ 2 ].GetInt();
				tb.set( val[ 1 ].GetString(), n );
			}
			break;

		case EMGFX_OBJ_TYPE_UINT :
			{
				unsigned int n = val[ 2 ].GetUInt();
				tb.set( val[ 1 ].GetString(), n );
			}
			break;

		case EMGFX_OBJ_TYPE_NUMBER :
			{
				double d = val[ 2 ].GetNumber();
				tb.set( val[ 1 ].GetString(), d );
			}
			break;

		case EMGFX_OBJ_TYPE_BOOL :
			{
				bool b = val[ 2 ].GetBool();
				tb.set( val[ 1 ].GetString(), b );
			}
			break;

		case EMGFX_OBJ_TYPE_STRING :
			{
				std::string str = val[ 2 ].GetString();
				if( EMGFX_FONT_KOREAN == m_emFontLang )
					str = sc::string::utf8_to_ansi( str );

				tb.set( val[ 1 ].GetString(), str.c_str() );
			}
			break;
		}
	}
}

void GfxInterface::ChangeTreeData( int nWidgetID, const char* szKey, int nIndex, const char* szObjectID, 
								   LuaTable tbData, const char* szInst, int nSubID )
{
	VEC_STRING vecObjectID;
	SplitString( vecObjectID, szObjectID, SPLIT_STRING_ETC );

	if( true == vecObjectID.empty() )
		return;

	GFx::Value args[ 6 ];
	args[ 0 ].SetInt( nWidgetID );
	args[ 1 ].SetInt( nSubID );
	args[ 2 ].SetString( szKey );
	args[ 3 ].SetInt( nIndex );
	m_pGameScene->GetMovie()->CreateObject( &args[ 4 ] );
	args[ 5 ].SetString( szInst );

	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		const char* sz = tbData.get< const char* >( i );
		if( NULL == sz )
			return;

		std::wstring strW = EncodeText( sz );
		args[ 4 ].SetMember( vecObjectID[ i - 1 ].c_str(), strW.c_str() );
	}

	Invoke( AS_ChangeTreeData, args, 6 );
}

void GfxInterface::AddChatMessage( int nLogID, const std::string& strMsg, DWORD dwColor, 
								   const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
								   const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName )
{
	std::string strMessage = strMsg;
	boost::replace_all( strMessage, "\r\n", "\n" );
	

	std::wstring strW, strName;
	if( EMGFX_FONT_KOREAN == m_emFontLang )
	{
		strW = EncodeText( strMessage );
		strName = EncodeText( sReferName.strName );
	}
	else
	{
		strW = sc::string::utf8ToUnicode( strMessage );
		strName = sc::string::utf8ToUnicode( sReferName.strName );
	}

	GFx::Value args[ 6 ];
	args[ 0 ].SetInt( nLogID );
	args[ 1 ].SetStringW( strW.c_str() );
	args[ 2 ].SetUInt( dwColor );

	// 커스텀 컬러 배열
	m_pGameScene->GetMovie()->CreateArray( &args[ 3 ] );
	// 아이템 링크 배열
	m_pGameScene->GetMovie()->CreateArray( &args[ 4 ] );

	// 커스텀 컬러 설정
	GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR_CITER iter = vecCustomColor.begin();
	for( int i = 0; iter != vecCustomColor.end(); ++iter, ++i )
	{
		GFx::Value gfxObject;
		m_pGameScene->GetMovie()->CreateObject( &gfxObject );

		gfxObject.SetMember( "color", iter->dwColor );
		gfxObject.SetMember( "begin", (UInt32) iter->nBegin );
		gfxObject.SetMember( "end", (UInt32) iter->nEnd );

		args[ 3 ].SetElement( i, gfxObject );
	}

	// 이름 정보보기 데이터 설정
	m_pGameScene->GetMovie()->CreateObject( &args[ 5 ] );
	args[ 5 ].SetMember( "name", strName.c_str() );
	args[ 5 ].SetMember( "begin", (UInt32) sReferName.nBegin );
	args[ 5 ].SetMember( "end", (UInt32) sReferName.nEnd );

	std::vector< GFx::Value >& chatLogHandler = GetCore()->GetExternalInterface()->GetChatLogHandler();
	for( size_t i = 0; i < chatLogHandler.size(); ++i )
		chatLogHandler[ i ].InvokeSelf( NULL, args, 6 );
}

void GfxInterface::AddLinkMessage( int nLogID, const std::string& strMsg, DWORD dwColor,
								   const GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR& vecCustomColor,
								   VEC_LINK_TEXT_DATA& vecLinkTextData,
								   const GLCHAT_DEF::SCHAT_REFERCHAR_NAME& sReferName )
{
	std::string strMessage = strMsg;
	boost::replace_all( strMessage, "\r\n", "\n" );

	// 기존
	std::wstring strW, strName;
	INT nNameBegin, nNameEnd;
	if( EMGFX_FONT_KOREAN == m_emFontLang )
	{
		strW = EncodeText( strMessage );
		strName = EncodeText( sReferName.strName );

		nNameBegin = sReferName.nBegin;
		nNameEnd = sReferName.nEnd;
	}
	else
	{
		strW = sc::string::utf8ToUnicode( strMessage );
		strName = sc::string::utf8ToUnicode( sReferName.strName );

		nNameBegin = strW.find(strName);
		nNameEnd = nNameBegin + strName.length();
	}
	GFx::Value args[ 6 ];
	args[ 0 ].SetInt( nLogID );
	args[ 1 ].SetStringW( strW.c_str() );
	args[ 2 ].SetUInt( dwColor );

	// 커스텀 컬러 배열
	m_pGameScene->GetMovie()->CreateArray( &args[ 3 ] );
	// 아이템 링크 배열
	m_pGameScene->GetMovie()->CreateArray( &args[ 4 ] );

	// 커스텀 컬러 설정
	GLCHAT_DEF::VEC_CHAT_CUSTOM_COLOR_CITER iterColor = vecCustomColor.begin();
	for( int i = 0; iterColor != vecCustomColor.end(); ++iterColor, ++i )
	{
		GFx::Value gfxObject;
		m_pGameScene->GetMovie()->CreateObject( &gfxObject );

		gfxObject.SetMember( "color", iterColor->dwColor );
		gfxObject.SetMember( "begin", (UInt32) iterColor->nBegin );
		gfxObject.SetMember( "end", (UInt32) iterColor->nEnd );

		args[ 3 ].SetElement( i, gfxObject );
	}

	// 아이템 링크 설정
	VEC_LINK_TEXT_DATA_ITER iterLink = vecLinkTextData.begin();
	for( int i = 0; iterLink != vecLinkTextData.end(); ++iterLink, ++i )
	{
		GFx::Value gfxObject;
		m_pGameScene->GetMovie()->CreateObject( &gfxObject );

		gfxObject.SetMember( "startPos", (float) iterLink->sLinkPos.nStart );
		gfxObject.SetMember( "endPos", (float) iterLink->sLinkPos.nEnd );
		gfxObject.SetMember( "textColor", iterLink->dwTextColor );
		gfxObject.SetMember( "linkIndex", (float) iterLink->nLinkTypeIndex );

		args[ 4 ].SetElement( i, gfxObject );
	}

	// 이름 정보보기 데이터 설정
	m_pGameScene->GetMovie()->CreateObject( &args[ 5 ] );
	args[ 5 ].SetMember( "name", strName.c_str() );
	//args[ 5 ].SetMember( "begin", (UInt32) sReferName.nBegin );
	//args[ 5 ].SetMember( "end", (UInt32) sReferName.nEnd );
	args[ 5 ].SetMember( "begin", (UInt32)nNameBegin );
	args[ 5 ].SetMember( "end", (UInt32)nNameEnd );

	std::vector< GFx::Value >& chatLogHandler = GetCore()->GetExternalInterface()->GetChatLogHandler();
	for( size_t i = 0; i < chatLogHandler.size(); ++i )
		chatLogHandler[ i ].InvokeSelf( NULL, args, 6 );
}

void GfxInterface::SlotDragStop()
{
	Invoke( AS_SlotDragStop, NULL, 0 );
}

bool GfxInterface::GetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem )
{
	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( szSlotID ) );
	if( NULL == pSlot || EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return false;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	sItem = pItemSlot->GetItemCustom();

	return true;
}

void GfxInterface::ResetSlot( const char* strSlotID )
{
	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	
	pSlot->Reset();
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetSlotItem( const char* szSlotID, SITEMCUSTOM& sItem )
{
	ResetSlot( szSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( szSlotID ) );
	if( NULL == pSlot || EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	GfxItemBaseSlot* pItemSlot = dynamic_cast<GfxItemBaseSlot*>(pSlot);
	
	pItemSlot->SetItemCustom(sItem);
	m_pGameScene->GetDataBinder()->NotifyDataChange( szSlotID );
}

void GfxInterface::SetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, WORD wRealMID, WORD wRealSID )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	sItem.TurnNumSet( wCount );

	if ( sItem.IsBoxWrapped() )
	{
		sItem.PetDbNumSet( SNATIVEID( wRealMID, wRealSID ).Id() );
	}

	pItemSlot->SetItemCustom( sItem );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetItemSlotData_NonStrengthen (
	const char* strSlotID,
	WORD wMID,
	WORD wSID,
	WORD wCount,
	WORD wRealMID,
	WORD wRealSID )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	sItem.TurnNumSet( wCount );
	sItem.cDAMAGE = 0;
	sItem.cDEFENSE = 0;

	if ( sItem.IsBoxWrapped() )
	{
		sItem.PetDbNumSet( SNATIVEID( wRealMID, wRealSID ).Id() );
	}

	pItemSlot->SetItemCustom( sItem );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetItemSlotData_BORNTIME( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, DWORD dwBornT )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	sItem.tBORNTIME = dwBornT;
	sItem.TurnNumSet( wCount );

	pItemSlot->SetItemCustom( sItem );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetSkillSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nLevel )
{
	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_SKILLBASE != pSlot->GetBaseType() )
		return;

	GfxSkillBaseSlot* pSkillSlot = (GfxSkillBaseSlot*) pSlot;

	SNATIVEID sSkillID( wMID, wSID );

	pSkillSlot->SetNativeID( sSkillID );
	pSkillSlot->SetLevel( nLevel );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetInventorySlotData( const char* strSlotID, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_INVENTORY != pSlot->GetType() )
		return;

	GfxInventorySlot* pInvenSlot = (GfxInventorySlot*) pSlot;

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	const SINVENITEM* pItem = pChar->m_cInventory.GetItem( nPosX, nPosY );
	if( NULL != pItem )
	{
		SITEMCUSTOM sItem = pItem->CustomItem();
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );

		pInvenSlot->SetItemCustom( sItem );
		pInvenSlot->SetNew( pItem->bNew );
	}
	else
	{
		pInvenSlot->Reset();
	}

	pInvenSlot->SetPosition( nPosX, nPosY );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetRebuySlotData( const char* strSlotID, int nRebuyID, WORD wMID, WORD wSID )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	const RebuyItem::REBUY_VEC& vecRebuy = pChar->RebuyList();
	if( vecRebuy.size() <= nRebuyID - 1 )
		return ;

	RebuyItem::SREBUY_ITEM sRebuyItem = vecRebuy[nRebuyID - 1];
	if( sRebuyItem.sItemCustom.GetNativeID().wMainID != wMID )
		return ;

	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	pItemSlot->SetItemCustom(sRebuyItem.sItemCustom);
	
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetSummonStateSlotData( const char* strSlotID, const char* strIconName, INT nIconIdxX, INT nIconIdxY )
{
	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if ( pSlot == NULL )
		return;
	if ( pSlot->GetType() != EMGFX_SLOT_SUMMONSTATE )
		return;

	GfxSummonStateSlot* pSummonStateSlot = (GfxSummonStateSlot*)pSlot;
	pSummonStateSlot->SetIconData(std::string(strIconName), nIconIdxX, nIconIdxY);
	m_pGameScene->GetDataBinder()->NotifyDataChange(strSlotID);
}

void GfxInterface::SetBankSlotData( const char* strSlotID, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_INVEN_BANK != pSlot->GetType() )
		return;

	GfxInvenBankSlot* pBankSlot = (GfxInvenBankSlot*) pSlot;

	GLCharacter* pChar  = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	const SINVENITEM* pItem = pChar->m_cInvenCharged.GetItem( nPosX, nPosY );
	if( NULL != pItem )
	{
		SITEMCUSTOM sItem = pItem->CustomItem();
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );
		pBankSlot->SetItemCustom( sItem );
	}
	else
	{
		pBankSlot->Reset();
	}

	pBankSlot->SetPosition( nPosX, nPosY );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetWishlistSlotData( const char* strSlotID, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_INVEN_WISHLIST != pSlot->GetType() )
		return;

	GfxInvenWishListSlot* pWishSlot = (GfxInvenWishListSlot*) pSlot;

	WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
	if( NULL == pWishList )
		return;

	WishList::ItemSPtr spItem = pWishList->GetItem( (DWORD) MAKELONG( (WORD) nPosX, (WORD) nPosY ) );
	if( NULL != spItem )
	{
		SITEMCUSTOM sItem = spItem->m_sItem;
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );

		pWishSlot->SetItemCustom( sItem );
	}
	else
	{
		pWishSlot->Reset();
	}

	pWishSlot->SetPosition( nPosX, nPosY );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetCharInfoSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, int nSlot )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_CHARINFO != pSlot->GetType() )
		return;

	GfxCharInfoSlot* pCharInfoSlot = (GfxCharInfoSlot*) pSlot;

	SITEMCUSTOM sItemTemp = sItem;
	if( 0 == sItemTemp.TurnNum() )
		sItemTemp.TurnNumSet( 1 );

	pCharInfoSlot->SetItemCustom( sItemTemp );
	pCharInfoSlot->SetSlot( nSlot );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetSkillQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex )
{
	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_SQS != pSlot->GetType() )
		return;

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	GfxSkillQuickSlot* pSkillSlot = (GfxSkillQuickSlot*) pSlot;

	pSkillSlot->SetNativeID( SNATIVEID( dwID ) );
	pSkillSlot->SetIndex( nIndex );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetItemQuickSlotData( const char* strSlotID, DWORD dwID, int nIndex )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_IQS != pSlot->GetType() )
		return;

	GfxItemQuickSlot* pItemSlot = (GfxItemQuickSlot*) pSlot;

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	const SNATIVEID sID( dwID );

	SINVENITEM* pItem = pChar->m_cInventory.FindItem( sID );
	if( NULL != pItem )
	{
		SITEMCUSTOM sItem = pItem->CustomItem();
		sItem.TurnNumSet( static_cast<WORD>(pChar->m_cInventory.CountPileItem( sID ) ) );
		pItemSlot->SetItemCustom( sItem );
	}
	else
	{
		SITEMCUSTOM sItem( sID );
		sItem.TurnNumSet( 0 );
		pItemSlot->SetItemCustom( sItem );
	}

	pItemSlot->SetIndex( nIndex );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}
void GfxInterface::SetItemMiniGameDiceRewardSlotData( const char* strSlotID, DWORD dwID, int nIndex )
{
	ResetSlot( strSlotID );

	GfxSlotBase* _pSlot(dynamic_cast< GfxSlotBase*>(m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID )));
	if ( NULL == _pSlot )
		return;
	if ( EMGFX_SLOT_MINIGAME_DICE_REWARD != _pSlot->GetType() )
		return;

	GfxMiniGameDiceRewardSlot* _pRewardSlot((GfxMiniGameDiceRewardSlot*)_pSlot);

	const SNATIVEID _ItemID(dwID);
	const SITEMCUSTOM _itemCustom(_ItemID);
	_pRewardSlot->SetItemCustom( _itemCustom );	
	_pRewardSlot->SetIndex( nIndex );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetNpcMarketSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_NPCMARKET != pSlot->GetType() )
		return;

	GfxNpcMarketSlot* pMarketSlot = (GfxNpcMarketSlot*) pSlot;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	sItem.TurnNumSet( wCount );

	pMarketSlot->SetItemCustom( sItem );
	pMarketSlot->SetPosition( nPosX, nPosY );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetTradeSlotData(const char* strSlotID, int nPosX, int nPosY, bool bMySlot)
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_TRADE != pSlot->GetType() )
		return;

	GfxTradeSlot* pTradeSlot = (GfxTradeSlot*) pSlot;
	GLTradeClient* pTradeClient = GfxInterface::GetInstance()->GetGaeaClient()->GetTradeClient();

	GLTrade* pTrade = NULL;
	if( true == bMySlot )
		pTrade = &pTradeClient->GetMyTrade();
	else
		pTrade = &pTradeClient->GetTarTrade();

	SINVENITEM* pInvenItem = pTrade->GetItem( nPosX, nPosY );
	if( NULL == pInvenItem )
		pTradeSlot->Reset();
	else
		pTradeSlot->SetItemCustom( pInvenItem->CustomItem() );

	pTradeSlot->SetPosition( nPosX, nPosY );
	pTradeSlot->SetMySlot( bMySlot );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetNpcMarketCartSlotData( const char* strSlotID, const SITEMCUSTOM& sItem, WORD wIndex )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_NPCCART != pSlot->GetType() )
		return;

	GfxNpcCartSlot* pCartSlot = (GfxNpcCartSlot*) pSlot;

	pCartSlot->SetItemCustom( sItem );
	pCartSlot->SetIndex( wIndex );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}


void GfxInterface::SetPointShopSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD wCount, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_POINTSHOP != pSlot->GetType() )
		return;

	GfxPointShopSlot* pPointShopSlot = (GfxPointShopSlot*) pSlot;

	if( 0 == wCount )
		wCount = 1;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	sItem.TurnNumSet( wCount );

	pPointShopSlot->SetItemCustom( sItem );
	pPointShopSlot->SetPosition( nPosX, nPosY );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetPointShopCartSlotData( const char* strSlotID, WORD wMID, WORD wSID, int nIndex )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_POINTSHOP_CART != pSlot->GetType() )
		return;

	GfxPointShopCartSlot* pPointShopCartSlot = (GfxPointShopCartSlot*) pSlot;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	if( 0 == sItem.wTurnNum )
		sItem.TurnNumSet( 1 );

	pPointShopCartSlot->SetItemCustom( sItem );
	pPointShopCartSlot->SetIndex(nIndex);
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetReferSlotData( const char* strSlotID, int nSlot )
{
	ResetSlot( strSlotID );

	if( SLOT_VEHICLE == nSlot )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_REFERCHAR != pSlot->GetType() )
		return;

	GLCHARLOGIC* pCharLogic = m_pGaeaClient->GetReferChar()->GetCompleteData();
	if( NULL == pCharLogic )
		return;

	GfxReferCharSlot* pCharInfoSlot = (GfxReferCharSlot*) pSlot;

	const SITEMCUSTOM& sItem = pCharLogic->m_PutOnItems[ EMSLOT( nSlot ) ];
	if( true == sItem.GetNativeID().IsValidNativeID() )
	{
		SITEMCUSTOM sItemTemp = sItem;
		if( 0 == sItemTemp.TurnNum() )
			sItemTemp.TurnNumSet( 1 );

		pCharInfoSlot->SetItemCustom( sItemTemp );
	}
	else
	{
		pCharInfoSlot->Reset();
	}

	pCharInfoSlot->SetSlot( nSlot );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetPMSPointTradeSlotData( const char* strSlotID, WORD wPosX, WORD wPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	if( EMGFX_SLOT_PMSPOINTTRADE != pSlot->GetType() )
		return;

	GfxPMSPointTradeSlot* pPrivateMarketSearchSlot = (GfxPMSPointTradeSlot*) pSlot;

	const SINVENITEM* pInvenItem = m_pGaeaClient->GetCharacter()->m_cInventory.FindPosItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return;

	pPrivateMarketSearchSlot->SetItemCustom( pInvenItem->CustomItem() );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetPMSPointTradeSlotData_Index( const char* strSlotID, DWORD dwIndex, DWORD dwCount )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	if( EMGFX_SLOT_ITEMBASE != pSlot->GetType() )
		return;

	const private_market::ConsignmentSaleItem* pConsignmentSaleItem = m_pGaeaClient->GetConsignmentSale()->GetConsignmentSaleItem( dwIndex );
	if ( pConsignmentSaleItem == NULL )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	pItemSlot->SetItemCustom( pConsignmentSaleItem->sItemcustom );
	pItemSlot->GetItemCustom_NoneConst().TurnNumSet( (WORD) dwCount );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

// 개인상점 아이템 등록 슬롯
void GfxInterface::SetPrivateMarketSlotData( const char* strSlotID, int nPosX, int nPosY, bool bSelf )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_PRIVATEMARKET != pSlot->GetType() )
		return;

	SSALEITEM* pSaleItem = NULL;

	if( false == bSelf )
	{
		PrivateMarketClient* pMarket = m_pGaeaClient->GetPrivateMarketClient();

		std::tr1::shared_ptr< GLCharClient > pCharClient = m_pGaeaClient->GetChar( pMarket->GetOpenerID() );
		if( NULL == pCharClient )
			return;

		pSaleItem = pCharClient->m_sPMarket.GetItem( SNATIVEID( (WORD) nPosX, (WORD) nPosY ) );
	}
	else
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( NULL == pChar )
			return;

		pSaleItem = pChar->m_sPMarket.GetItem( SNATIVEID( (WORD) nPosX, (WORD) nPosY ) );
	}

	if( NULL == pSaleItem )
		return;

	SITEMCUSTOM sItem = pSaleItem->sITEMCUSTOM;
	if( true == pSaleItem->bSOLD )
		sItem.TurnNumSet( 0 );
	else
		sItem.TurnNumSet( (WORD) pSaleItem->dwNUMBER );

	GfxPrivateMarketSlot* pPrivateMarketSlot = (GfxPrivateMarketSlot*) pSlot;
	pPrivateMarketSlot->SetItemCustom( sItem );
	pPrivateMarketSlot->SetPosition( nPosX, nPosY );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetPrivateMarketSearchSlotData( const char* strSlotID, DWORD dwIndex, WORD wCount, bool bSearchPage )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	const private_market::sSaleItem* pSaleItem = NULL;
	if( true == bSearchPage )
		pSaleItem = m_pGaeaClient->GetPrivateMarketClient()->GetSearchPageData_sSaleItem( dwIndex );
	else
		pSaleItem = m_pGaeaClient->GetPrivateMarketClient()->GetWishPageData_sSaleItem( dwIndex );

	if( pSaleItem == NULL )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	pItemSlot->SetItemCustom( pSaleItem->sItemcustom );
	pItemSlot->GetItemCustom_NoneConst().TurnNumSet( wCount );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetReceivePostSlotData( const char* strSlotID, DWORD dwIndex, DWORD dwCount )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	const SAPOSTCLIENT* pAPostClient = m_pGaeaClient->GetPostClient()->GetSAPOSTCLIENT_ReceiveVec( dwIndex );
	if ( pAPostClient == NULL )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	// 사용편의상 우편데이터는 map과 vector로 관리한다;
	// 여기서 mep->second와 vector->data는 레퍼런스로 같은값을 참조;
	// 다른 모든데이터는 레퍼런스로 초기화가되나 SITEMCUSTOM은 다시한번 자신의 복사생성자가 콜되면서 복사를 해버린다;
	// 즉 동기화가 안맞아버림;
	if ( pAPostClient->m_byAttach01Accept == TRUE )	// 아이템 받았더라면;
		pItemSlot->SetItemCustom( SITEMCUSTOM(NATIVEID_NULL()) );
	else
		pItemSlot->SetItemCustom( pAPostClient->m_sAttach01 );

	pItemSlot->GetItemCustom_NoneConst().TurnNumSet( (WORD) dwCount );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetSlot_PostBoxSub_PreviewPageOpen( const char* strSlotID )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
		return;

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	pItemSlot->SetItemCustom( pCharacter->GetPostItem() );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetPetItemSlotData( const char* strSlotID, WORD wMID, WORD wSID, WORD nSlot )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_PET != pSlot->GetType() )
		return;

	GfxPetSlot* pPetSlot = (GfxPetSlot*) pSlot;

	SITEMCUSTOM sItem( SNATIVEID( wMID, wSID ) );
	pPetSlot->SetSlot(nSlot);
	pPetSlot->SetItemCustom( sItem );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetClubStorageSlotData( const char* strSlotID, WORD wChannel, WORD nPosX, WORD nPosY)
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_CLUBSTORAGE != pSlot->GetType() )
		return;

	GfxClubStorageSlot* pClubSlot = (GfxClubStorageSlot*) pSlot;

	SINVENITEM* pClubItem = m_pGaeaClient->GetMyClub()->FindPosItem( wChannel, nPosX, nPosY );
	if( NULL != pClubItem )
	{
		SITEMCUSTOM sItem = pClubItem->CustomItem();
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );

		pClubSlot->SetItem( sItem );
	}
	else
	{
		pClubSlot->Reset();
	}

	pClubSlot->SetPosition( nPosX, nPosY );
	pClubSlot->SetChannel( wChannel );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetClubStorageItem(const char* strSlotID, WORD Channel, WORD nPosX, WORD nPosY)
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_CLUBSTORAGE != pSlot->GetType() )
		return;

	GfxClubStorageSlot* pClubStrgSlot = (GfxClubStorageSlot*) pSlot;

	SINVENITEM* pClubItem = GfxInterface::GetInstance()->GetGaeaClient()->GetMyClub()->FindPosItem(Channel, nPosX, nPosY);
	if( NULL == pClubItem )
	{
		pClubStrgSlot->Reset();
	}
	else
	{
		SITEMCUSTOM sItem = pClubItem->CustomItem();
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );

		pClubStrgSlot->SetItem( sItem );
	}

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetLockerSlot( const char* strSlotID, int nIndex, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	if( 0 > nIndex || EMSTORAGE_CHANNEL <= nIndex )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_LOCKER != pSlot->GetType() )
		return;

	GfxLockerSlot* pLockerSlot = (GfxLockerSlot*) pSlot;
	GLCharacter* pChar = GetGaeaClient()->GetCharacter();

	const SINVENITEM* pItem = pChar->m_cStorage[ nIndex ].GetItem( nPosX, nPosY );
	if( NULL != pItem )
	{
		SITEMCUSTOM sItem = pItem->CustomItem();
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );

		pLockerSlot->SetItemCustom( sItem );
	}
	else
	{
		pLockerSlot->Reset();
	}

	pLockerSlot->SetIndex( nIndex );
	pLockerSlot->SetPosition( nPosX, nPosY );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetRebuildCardSlot( const char* strSlotID, int nPosX, int nPosY )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot || EMGFX_SLOT_REBUILD_CARD_ITEM != pSlot->GetType() )
		return;

	GfxRebuildCardItemSlot* pItemSlot = (GfxRebuildCardItemSlot*) pSlot;

	const SINVENITEM* pInvenItem = m_pGaeaClient->GetCharacter()->m_cInventory.GetItem( nPosX, nPosY );
	if( NULL != pInvenItem )
	{
		SITEMCUSTOM sItem = pInvenItem->CustomItem();
		if( 0 == sItem.TurnNum() )
			sItem.TurnNumSet( 1 );

		pItemSlot->SetItemCustom( sItem );
	}
	else
	{
		pItemSlot->Reset();
	}

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetItemSlotData_PartyDistribution( const char* strSlotID, DWORD dwItemID, WORD wSlotIndex)
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	GfxPartyDistributionSlot* pItemSlot = (GfxPartyDistributionSlot*)pSlot;
	pItemSlot->m_nSlotIndex = wSlotIndex;

	GLTenderItemCustomClient* pTenderItem = m_pGaeaClient->GetPartyTenderManager()->GetTenderItemCustom(wSlotIndex);
	if ( pTenderItem == NULL )
		return;

	pItemSlot->SetItemCustom(pTenderItem->m_ItemCustom);
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetItemRepairSlotData( const char* strSlotID, int nSlot )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;
	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	if( nSlot < 0 || nSlot >= SLOT_TSIZE )
		return;

	GLCharacter* pChar = GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	SITEMCUSTOM sItemCustom = pChar->m_PutOnItems[ nSlot ];
	sItemCustom.TurnNumSet( 1 );

	GfxItemBaseSlot* pItemSlot = (GfxItemBaseSlot*) pSlot;
	pItemSlot->SetItemCustom( sItemCustom );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetLottoWinItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >(
		m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if ( NULL == pSlot )
		return;

	if ( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystem( dwLottoSystemID );
	if ( NULL == pLottoSystem )
		return;

	LottoSystem::LOTTO_WIN_TYPE sLottoWinType1st = pLottoSystem->GetLottoWinType1st();
	SITEMCUSTOM sItemCustom = SITEMCUSTOM( sLottoWinType1st.sRewardItemID );
	sItemCustom.TurnNumSet( sLottoWinType1st.dwRewardItemCount );

	GfxItemBaseSlot* pItemSlot = static_cast< GfxItemBaseSlot* >( pSlot );
	if ( pItemSlot )
		pItemSlot->SetItemCustom( sItemCustom );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetLottoPreWinListItemSlotData ( const char* strSlotID, DWORD dwLottoSystemID, int nRanking )
{
	ResetSlot( strSlotID );

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >(
		m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if ( NULL == pSlot )
		return;

	if ( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	LottoSystem::CLottoSystemClient* pLottoSystem =
		LottoSystem::GLLottoSystemManClient::Instance()->GetLottoSystem( dwLottoSystemID );
	if ( NULL == pLottoSystem )
		return;

	LottoSystem::LOTTO_SYSTEM_WINNER_MAN sWinnerMan;
	LottoSystem::LOTTO_SYSTEM_WINNER_MAN_LIST sWinnerManList = pLottoSystem->GetWinnerManList();
	for ( int i=0; i<sWinnerManList.winnerManList.size(); ++i )
	{
		LottoSystem::LOTTO_SYSTEM_WINNER_MAN sTempWinnerMan = sWinnerManList.winnerManList.at( i );
		if ( sTempWinnerMan.ranking == nRanking )
		{
			sWinnerMan = sTempWinnerMan;
			break;
		}
	}

	SITEMCUSTOM sItemCustom = SITEMCUSTOM( sWinnerMan.sRewardItem );
	sItemCustom.TurnNumSet( sWinnerMan.nRewardItemCount );

	GfxItemBaseSlot* pItemSlot = static_cast< GfxItemBaseSlot* >( pSlot );
	if ( pItemSlot )
		pItemSlot->SetItemCustom( sItemCustom );

	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

void GfxInterface::SetRanMobileRewardItemSlotData( const char* strSlotID, int nIndex )
{
	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( strSlotID ) );
	if( NULL == pSlot )
		return;

	if( EMGFX_SLOT_ITEMBASE != pSlot->GetBaseType() )
		return;

	ResetSlot( strSlotID );

	const NSRanMobile::VEC_REWARD& vecReward = NSRanMobile::GLRanMobile::GetInstance().GetReward();
	if( static_cast< size_t >( nIndex ) >= vecReward.size() )
		return;

	GfxItemBaseSlot* pItemSlot = static_cast< GfxItemBaseSlot* >( pSlot );
	if( NULL == pItemSlot )
		return;

	const NSRanMobile::SRewardItem& sReward = vecReward[ nIndex ];

	SITEMCUSTOM sItemCustom = SITEMCUSTOM( sReward.sID );
	sItemCustom.TurnNumSet( sReward.wCount );
	sItemCustom.SetDurability( sReward.fExpCompressRatio );
	
	pItemSlot->SetItemCustom( sItemCustom );
	m_pGameScene->GetDataBinder()->NotifyDataChange( strSlotID );
}

SNATIVEID GfxInterface::GetDragSlotSkillID()
{
	GFx::Value result;
	if( false == Invoke( AS_GetDragSlotBinding, NULL, 0, &result ) )
		return SNATIVEID( false );

	const char* szBinding = result.GetString();

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( m_pGameScene->GetDataBinder()->GetDataWrapper( szBinding ) );
	if( NULL == pSlot || EMGFX_SLOT_SKILLBASE != pSlot->GetBaseType() )
		return SNATIVEID( false );

	GfxSkillBaseSlot* pSkillSlot = (GfxSkillBaseSlot*) pSlot;
	return pSkillSlot->GetNativeID();
}

void GfxInterface::SetSkillSlotCoolDown( const SNATIVEID& sID )
{
	float fCoolDownTime = 0.0f;
	float fCoolDownRatio = 0.0f;
	m_pGaeaClient->GetCharacter()->GetSkillCoolDownTime( sID, fCoolDownTime, fCoolDownRatio );

	GFx::Value args[ 2 ], result;
	args[ 0 ].SetNumber( fCoolDownTime );
	args[ 1 ].SetNumber( fCoolDownRatio );

	VEC_BIND_SLOT vecBindSlot;
	m_pGameScene->GetDataBinder()->GetDataWrapperType( EMGFX_SLOT_SKILLBASE, vecBindSlot );

	VEC_BIND_SLOT_ITER iter = vecBindSlot.begin();
	for( ; iter != vecBindSlot.end(); ++iter )
	{
		GfxSkillBaseSlot* pSlot = (GfxSkillBaseSlot*) iter->pSlot;

		if( sID != pSlot->GetNativeID() )
			continue;

		std::vector< GFx::Value* >::iterator iterBind = iter->vecBind.begin();
		for( ; iterBind != iter->vecBind.end(); ++iterBind )
		{
			GFx::Value* pBind = (*iterBind);

			GFx::Value valIsUse;
			if( false == pBind->GetMember( "useCoolDown", &valIsUse ) )
				continue;

			if( false == valIsUse.GetBool() )
				continue;

			pBind->Invoke( "SetCoolDownTween", &result, args, 2 );
		}
	}
}

void GfxInterface::SetItemSlotCoolDown( const SNATIVEID& sID )
{
	float fCoolDownTime = 0.0f;
	float fCoolDownRatio = 0.0f;
	m_pGaeaClient->GetCharacter()->GetItemCoolDownTime( sID, fCoolDownTime, fCoolDownRatio );

	GFx::Value args[ 2 ], result;
	args[ 0 ].SetNumber( fCoolDownTime );
	args[ 1 ].SetNumber( fCoolDownRatio );

	VEC_BIND_SLOT vecBindSlot;
	m_pGameScene->GetDataBinder()->GetDataWrapperType( EMGFX_SLOT_ITEMBASE, vecBindSlot );

	VEC_BIND_SLOT_ITER iter = vecBindSlot.begin();
	for( ; iter != vecBindSlot.end(); ++iter )
	{
		GfxSkillBaseSlot* pSlot = (GfxSkillBaseSlot*) iter->pSlot;

		if( sID != pSlot->GetNativeID() )
			continue;

		std::vector< GFx::Value* >::iterator iterBind = iter->vecBind.begin();
		for( ; iterBind != iter->vecBind.end(); ++iterBind )
		{
			GFx::Value* pBind = (*iterBind);
			pBind->Invoke( "SetCoolDownTween", &result, args, 2 );
		}
	}
}

void GfxInterface::AddNameDisplay( CROWREN& sDispName, const char* szOwnerName )
{
	//if( true == m_bDisplayOn )
	//	GfxDisplay::GetInstance().AddName( sDispName, szOwnerName );

	if( true == m_bDisplayOn )
		m_pOldInterface->NameDisplayAddName( sDispName, szOwnerName );
}

void GfxInterface::ClearDisplay()
{
	GfxDisplay::GetInstance().Clear();
}

void GfxInterface::DeletePrivateMarketDisplay( DWORD dwID )
{
	GfxDisplay::GetInstance().DelPrivateMarket( dwID );
}

void GfxInterface::NameDisplaySetCount( DWORD Count )
{
	m_pOldInterface->NameDisplaySetCount( Count	);
}

void GfxInterface::AddHeadChatDisplay( const std::string& strName, DWORD dwNameColor,
						const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos )
{
	if( true == m_bDisplayOn && TRUE == m_pGaeaClient->GetGlobalStage()->GetGameStage()->m_bUseInterface )
		GfxDisplay::GetInstance().AddHeadChat( strName, dwNameColor, strChat, dwColor, vPos );

	//if( true == m_bDisplayOn && TRUE == m_pGaeaClient->GetGlobalStage()->GetGameStage()->m_bUseInterface )
	//	GfxInterfaceBase::GetInstance()->AddChatMsg( CString(strName.c_str()),dwNameColor,
	//	CString(strChat.c_str()),dwColor,vPos);
}

void GfxInterface::AddDamageDisplay( const D3DXVECTOR3& vPos, int nDamage, DWORD dwDamageFlag,
									 BOOL bAttack, const STARGETID& sCauser, const STARGETID& sTarget )
{
	if( false == RANPARAM::bShowOtherDamage )
	{
		DWORD dwGaeaID = m_pGaeaClient->GetCharacter()->GetGaeaID();

		// 데미지 유발자나 대상이 소환수일 경우 소환수 소유자가 내 캐릭터인지 체크
		if( CROW_SUMMON == sCauser.emCrow )
		{
			std::tr1::shared_ptr< GLSummonClient > pSummon = m_pGaeaClient->GetSummon( sCauser.GaeaId );
			if( NULL != pSummon )
			{
				if( dwGaeaID != pSummon->m_dwOwnerGaeaID )
					return;
			}
		}
		else if( CROW_SUMMON == sTarget.emCrow )
		{
			std::tr1::shared_ptr< GLSummonClient > pSummon = m_pGaeaClient->GetSummon( sTarget.GaeaId );
			if( NULL != pSummon )
			{
				if( dwGaeaID != pSummon->m_dwOwnerGaeaID )
					return;
			}
		}
		// 데미지 유발자가 내 캐릭터가 아닌지 확인
		else if( CROW_PC != sCauser.emCrow || dwGaeaID != sCauser.GaeaId )
		{
			// 데미지 타겟이 내 캐릭터가 아닌지 확인
			if( CROW_PC != sTarget.emCrow || dwGaeaID != sTarget.GaeaId )
				return;
		}
	}

	//if( true == m_bDisplayOn && TRUE == m_pGaeaClient->GetGlobalStage()->GetGameStage()->m_bUseInterface )
	//	GfxDisplay::GetInstance().AddDamage( vPos, nDamage, dwDamageFlag, TRUE == bAttack ? true : false );

	if( true == m_bDisplayOn && TRUE == m_pGaeaClient->GetGlobalStage()->GetGameStage()->m_bUseInterface )
		m_pOldInterface->SetDamage( vPos, nDamage, dwDamageFlag, bAttack );
}

void GfxInterface::AddPrivateMarketDisplay( const CROWREN& sDispName )
{
	if( true == m_bDisplayOn && TRUE == m_pGaeaClient->GetGlobalStage()->GetGameStage()->m_bUseInterface )
		GfxDisplay::GetInstance().AddPrivateMarket( sDispName );

	//if( true == m_bDisplayOn && TRUE == m_pGaeaClient->GetGlobalStage()->GetGameStage()->m_bUseInterface )
	//	m_pOldInterface->ADD_MARKET(sDispName);
}

void GfxInterface::ToggleOffPrivateMarketDisplay( DWORD dwID )
{
	GfxDisplay::GetInstance().ToggleOffPrivateMarket( dwID );
}

void GfxInterface::ChangeClubIcon( DWORD dwClubID )
{
	GfxDisplay::GetInstance().ChangeClubIcon( dwClubID );
}

bool GfxInterface::GetCapslockKeyState()
{
	return m_pGameScene->GetCapslockKeyState();
}

void GfxInterface::SetCapslockKeyState(bool bDown)
{
	m_pGameScene->SetCapslockKeyState(bDown);
}

bool GfxInterface::RegShowTooltipEnableState( int nWidgetID, int nSubID, const char* strInstance, WORD wMID, WORD wSID, WORD wLevel, bool bUpdate )
{
	GfxSkillSlot gfxSlot;
	gfxSlot.SetNativeID(SNATIVEID(wMID, wSID));
	gfxSlot.SetLevel(wLevel);

	std::wstring strW = EncodeText( NSWIDGET_SCRIPT::GetTypeText_SkillNextLevel() );

	GFx::Value args[6];
	args[0].SetInt( nWidgetID );
	args[1].SetInt(nSubID );
	args[2].SetString(strInstance);
	m_pGameScene->GetMovie()->CreateObject(&args[3]);
	args[3].SetMember("typetext", strW.c_str() );
	gfxSlot.GetTooltip(&args[3]);
	
	m_pGameScene->GetMovie()->CreateObject(&args[4]);
	args[4].SetMember("typetext", strW.c_str());
	gfxSlot.GetSubTooltip(&args[4]);
	args[5].SetBoolean(bUpdate);
	Invoke( AS_RegShowTooltipEnableState, args, 6 );
	return true;
}

bool GfxInterface::UnregShowTooltipEnableState( int nWidgetID, int nSubID, const char* strInstance )
{
	GFx::Value args[3];
	args[0].SetInt( nWidgetID );
	args[1].SetInt( nSubID );
	args[2].SetString( strInstance );
	Invoke( AS_UnregShowTooltipEnableState, args, 3 );
	return true;
}



////////////////////////// OLD UI


void GfxInterface::SetTargetInfoSelect( STARGETID sTargetID	)
{
	m_pOldInterface->SetTargetInfoSelect(sTargetID);
}

void GfxInterface::SetTargetInfoMouseOver( STARGETID sTargetID	)
{
	m_pOldInterface->SetTargetInfoMouseOver(sTargetID);
}

void GfxInterface::ResetTargetInfoSelect()
{
	if( m_pOldInterface == NULL	)
		return ;

	m_pOldInterface->ResetTargetInfoSelect();
}

void GfxInterface::AddChatMsg( const CString& strName, const D3DCOLOR& dwIDColor, 
						const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos ) 
{
	//m_pOldInterface->AddChatMsg(strName,dwIDColor,strChat,dwChatColor,vPos);
}

void GfxInterface::UiFrameMove(LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime)
{
	m_pOldInterface->UiFrameMove(pd3dDevice, fElapsedTime);
}

void GfxInterface::UiRender(LPDIRECT3DDEVICEQ pd3dDevice)
{
	m_pOldInterface->UiRender(pd3dDevice);
}


void GfxInterface::ADD_MARKET( const CROWREN &sDISP ){ m_pOldInterface->ADD_MARKET(sDISP);}