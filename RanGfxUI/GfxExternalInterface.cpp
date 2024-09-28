#include "stdafx.h"
#include "GfxExternalInterface.h"

#include "../SigmaCore/String/StringUtil.h"
#include "../enginelib/Common/CommonWeb.h"
#include "../enginelib/Common/FileSystem.h"
#include "../enginelib/DxTools/DxClubMan.h"
#include "../enginelib/DeviceMan.h"
#include "../enginelib/Common/DXInputString.h"

#include "../RanLogic/GLUseFeatures.h"

#include "./Core/GfxCore.h"
#include "./Core/CLIKDataBinding.h"
#include "./Slot/GfxSlot.h"
#include "./RenderTexture/GfxRenderTexture.h"
#include "./Display/GfxDisplay.h"
#include "./GfxInterface.h"
#include "./GfxWidgetInstanceMan.h"

//#include "../enginelib/GUInterface/GameTextControl.h"
#include "../RanLogicClient/StaticHoldItem.h"
#include "../RanLogicClient/Widget/GLWidgetsList.h"
#include "../RanLogicClient/Widget/GLWidgetScript.h"
#include "../RanLogicClient/Widget/GLWidgetMsgInterface.h"
#include "../RanLogicClient/Widget/GLWidgetTexture.h"
#include "../RanLogicClient/Input/GLInputManager.h"
#include "../RanLogicClient/Chat/GLChatMan.h"
#include "../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../RanLogicClient/Widget/GLWidgetSaveProp.h"
#include "../RanLogicClient/Minimap/GLMinimapClient.h"

#define GFX_ADD_CALLBACK( callbackName, fun, class ) \
	{\
	GfxCallbackFunc gfxCallFunc;\
	gfxCallFunc.CallbackFunc = boost::bind( std::mem_fun( fun ), class, _1 );\
	m_mapCallbackFunc[ callbackName ] = gfxCallFunc;\
	}\

//---------------------------------------------------------------------------------------------------------------------

// ������ ������ �Լ� ��
typedef void (*DataAdapterCallback)( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params );
struct DataAdapterFunc
{
	const char*			szName;
	DataAdapterCallback	func;
};

// ������ ������ �Լ� �� ����Ʈ ( DataAdapter.as ���� ���� )
// AS �Լ��� ���⼭ ����
namespace NSGFX_DATA_ADAPTER
{
	const DataAdapterFunc g_DataAdapterFuncList[] = 
	{
		// ����
		{ "notifyHoldStart", &GfxExternalInterface::OnNotifyHoldStart },
		{ "notifyHoldEnd", &GfxExternalInterface::OnNotifyHoldEnd },
		{ "notifyHoldEndForce", &GfxExternalInterface::OnNotifyHoldEndForce },
		{ "notifyHoldEndOutside", &GfxExternalInterface::OnNotifyHoldEndOutside },
		{ "notifyLeftMouseClick", &GfxExternalInterface::OnNotifySlotLeftMouseClick },
		{ "notifyRightMouseClick", &GfxExternalInterface::OnNotifySlotRightMouseClick },

		// ���� ����
		{ "getSlotTooltip", &GfxExternalInterface::OnGetSlotToolTip },
		// ���� ���� ����
		{ "getSlotSubTooltip", &GfxExternalInterface::OnGetSlotSubTooltip },
		// ���� �߰� ���� ����
		{ "getAddSlotCount", &GfxExternalInterface::OnGetAddSlotCount },
		// ���� �߰� ���� ���ε� ID
		{ "getAddSlotBinding", &GfxExternalInterface::OnGetAddSlotBinding },
		// ���� ��ȯ ���� ����
		{ "getTradeSlotCount", &GfxExternalInterface::OnGetTradeSlotCount },
		// ���� ��ȯ ���� ���ε� ID
		{ "getTradeSlotBinding", &GfxExternalInterface::OnGetTradeSlotBinding },
		// ���� ����-���Կ� ���� ��µ�
		{ "showTooltipSlot_bySlot", &GfxExternalInterface::OnShowTooltipSlot_bySlot },
		// ���� ����-���� ������ ��µ�;
		{ "showTooltipSlot_byID", GfxExternalInterface::OnShowTooltipSlot_byID },
		// ���� ���̵�
		{ "hideTooltipSlot", &GfxExternalInterface::OnHideTooltipSlot },
		// ���� Width �ִ밪
		{ "getMaxTooltipWidth", &GfxExternalInterface::OnGetMaxTooltipWidth },
		// ���� Width �ּҰ�
		{ "getMinTooltipWidth", &GfxExternalInterface::OnGetMinTooltipWidth },

		// ä�� �α�
		{ "registerMessageLogListener",     &GfxExternalInterface::OnRegisterMessageLogListener },
		{ "unregisterMessageLogListener",   &GfxExternalInterface::OnUnregisterMessageLogListener },

		// ���� �ؽ�ó
		{ "registerRenderTexture", &GfxExternalInterface::OnRegisterRenderTexture },
		{ "unregisterRenderTexture", &GfxExternalInterface::OnUnregisterRenderTexture },
		{ "clubIconRenderTexture", &GfxExternalInterface::OnClubIconRenderTexture },

		// HTML View Visible
		{ "visibleHtmlView", &GfxExternalInterface::OnHtmlViewSetVisible },

		// TextInput Conversion Mode
		{ "saveConversionMode", &GfxExternalInterface::OnSaveConversionMode },
		{ "setConversionMode", &GfxExternalInterface::OnSetConversionMode },

		// �����
		{ "isGameRun", &GfxExternalInterface::OnIsGameRun },

		// �ؽ�Ʈ��ǲ
		{ "GetCompositionString", &GfxExternalInterface::OnGetCompositionString },
		{ "SetFocusInTextInput", &GfxExternalInterface::OnSetFocusInTextInput },

		// ������Ƽ ����
		{ "saveProp", &GfxExternalInterface::OnSaveProp },

		// �������̽�
		{ "registerInterface", &GfxExternalInterface::OnRegisterInterface },
		{ "registerDisplayInterface", &GfxExternalInterface::OnRegisterDisplayInterface },

		{ "getLargeMapIconTooltip", &GfxExternalInterface::OnGetLargeMapIconTooltip}, 
		{ "chatLogRefresh", &GfxExternalInterface::OnChatLogRefresh },
		{ 0, 0 }
	};
}

class GfxDataAdapterHandler : public GFx::FunctionHandler
{
public :
	GfxDataAdapterHandler( GfxExternalInterface* pthis )
		:	pThis(pthis)
	{
	}

	virtual void Call( const GFx::FunctionHandler::Params& params )
	{
		SetControlfp(true);

		( (DataAdapterCallback) params.pUserData )( pThis, params );

		SetControlfp(false);
	}

protected :
	GfxExternalInterface*  pThis;
};

//---------------------------------------------------------------------------------------------------------------------
GfxExternalInterface::GfxExternalInterface(void)
:	m_pGameDataBinder( NULL )
,	m_bCreateCompleteGameMovie( false )
{
}

GfxExternalInterface::~GfxExternalInterface(void)
{
	m_mapCallbackFunc.clear();
}

void GfxExternalInterface::Callback( Movie* pMovie, const char* strMethodName, const GFx::Value* args, unsigned argCount )
{
	MAP_CALLBACK_FUNC_ITER iter = m_mapCallbackFunc.find( strMethodName );
	if( m_mapCallbackFunc.end() == iter )
		return;

	GfxCallbackFuncValue value( args, argCount );
	iter->second.CallbackFunc( value );
}

void GfxExternalInterface::InitCallbackFunc()
{
	GFX_ADD_CALLBACK( "OnWidgetOpen", &GfxExternalInterface::OnWidgetOpen, this );
	GFX_ADD_CALLBACK( "OnWidgetClose", &GfxExternalInterface::OnWidgetClose, this );
	GFX_ADD_CALLBACK( "OnWidgetUnload", &GfxExternalInterface::OnWidgetUnload, this );
	GFX_ADD_CALLBACK( "OnDataAdapterInit", &GfxExternalInterface::OnDataAdapterInit, this );
	GFX_ADD_CALLBACK( "OnCreateCoreComplete", &GfxExternalInterface::OnCreateCoreComplete, this );
	GFX_ADD_CALLBACK( "OnCustomEvent", &GfxExternalInterface::OnCustomEvent, this );
	GFX_ADD_CALLBACK( "OnChatLinkClick", &GfxExternalInterface::OnChatLinkClick, this );
	GFX_ADD_CALLBACK( "OnChatReferName", &GfxExternalInterface::OnChatReferName, this );
	GFX_ADD_CALLBACK( "OnChatWhisper", &GfxExternalInterface::OnChatWhisper, this );
	GFX_ADD_CALLBACK( "OnPrintDebug", &GfxExternalInterface::OnPrintDebug, this );
	GFX_ADD_CALLBACK( "OnPrivateMarketClick", &GfxExternalInterface::OnPrivateMarketClick, this );
	GFX_ADD_CALLBACK( "OnItemNameClick", &GfxExternalInterface::OnItemNameClick, this );
	GFX_ADD_CALLBACK( "OnImageRelease", &GfxExternalInterface::OnImageRelease, this );
	GFX_ADD_CALLBACK( "OnChangeResolution", &GfxExternalInterface::OnChangeResolution, this );

	GFX_ADD_CALLBACK( "onDebugSceneComplete", &GfxExternalInterface::OnDebugSceneComplete, this );
	GFX_ADD_CALLBACK( "onToggleDebugScene", &GfxExternalInterface::OnToggleDebugScene, this );
	GFX_ADD_CALLBACK( "onTextCopy", &GfxExternalInterface::OnTextCopy, this );
}

// External Call ---------------------------------------------------------------
void GfxExternalInterface::OnWidgetOpen( const GfxCallbackFuncValue& value )
{
	if( 2 != value.argCount )
		return;

	WORD wWidgetID = (WORD) value.args[ 0 ].GetInt();
	WORD wSubID = (WORD) value.args[ 1 ].GetInt();

	const GLWidget* pWidget = GLWidgetsList::GetInstance().GetWidget( wWidgetID );
	if( NULL == pWidget )
		return;

	if( true == pWidget->m_bSaveProp )
		GLWidgetSaveProp::GetInstance().SetProperty( wWidgetID, wSubID );

	if( true == pWidget->m_bCloseToDelete )
		GfxWidgetInstanceMan::GetInstance().RemoveDelete( wWidgetID, wSubID );
}

void GfxExternalInterface::OnWidgetClose( const GfxCallbackFuncValue& value )
{
	if( 3 != value.argCount )
		return;

	WORD wWidgetID = (WORD) value.args[ 0 ].GetInt();
	WORD wSubID = (WORD) value.args[ 1 ].GetInt();
	bool bIsDelete = value.args[ 2 ].GetBool();

	const GLWidget* pWidget = GLWidgetsList::GetInstance().GetWidget( wWidgetID );
	if( NULL == pWidget )
		return;

	if( false == bIsDelete && true == pWidget->m_bCloseToDelete )
		GfxWidgetInstanceMan::GetInstance().InsertDelete( wWidgetID, wSubID );
}

void GfxExternalInterface::OnWidgetUnload( const GfxCallbackFuncValue& value )
{
	if( 2 != value.argCount )
		return;

	GfxWidgetInstanceMan::GetInstance().Remove( 
		(WORD) value.args[ 0 ].GetUInt(), (WORD) value.args[ 1 ].GetInt() );
}

void GfxExternalInterface::OnDataAdapterInit( const GfxCallbackFuncValue& value )
{
	if( 1 != value.argCount )
		return;

	if( false == value.args[ 0 ].IsObject() )
		return;

	GFx::Movie* pMovie = GfxInterface::GetInstance()->GetGameScene()->GetMovie();

	GFx::Value classObj = value.args[ 0 ];

	Ptr< GfxDataAdapterHandler > handler = *SF_NEW GfxDataAdapterHandler( this );
	for( WORD i = 0; NSGFX_DATA_ADAPTER::g_DataAdapterFuncList[ i ].szName; ++i )
	{
		GFx::Value func;
		pMovie->CreateFunction( &func, handler, (void *) NSGFX_DATA_ADAPTER::g_DataAdapterFuncList[ i ].func );

		classObj.SetMember( NSGFX_DATA_ADAPTER::g_DataAdapterFuncList[ i ].szName, func );
	}
}

void GfxExternalInterface::OnCreateCoreComplete( const GfxCallbackFuncValue& value )
{
	GFx::Movie* pMovie = GfxInterface::GetInstance()->GetGameScene()->GetMovie();

	// ���÷��� �ʱ�ȭ
	GfxDisplay::GetInstance().Init();

	// AS�� ��ü widget ����Ʈ ���
	GFx::Value args[ 18 ], result;

	const MAP_WIDGET& vecWidget = GLWidgetsList::GetInstance().GetList();

	MAP_WIDGET_CITER iter = vecWidget.begin();
	for( ; iter != vecWidget.end(); ++iter )
	{
		const GLWidget& widget = iter->second;

		std::string strFileName = widget.m_strFileName;
		strFileName.append( NSGFXFILE::g_strExtension );

		std::string strWindowFile = widget.m_strWindowFile;
		strWindowFile.append( NSGFXFILE::g_strExtension );

		args[ 0 ].SetInt( widget.m_wWidgetID );
		args[ 1 ].SetString( strFileName.c_str() );
		args[ 2 ].SetString( strWindowFile.c_str() );
		args[ 3 ].SetStringW( widget.m_strTitleName.c_str() );
		args[ 4 ].SetString( widget.m_strLoadCompleteCall.c_str() );
		args[ 5 ].SetString( widget.m_strOpenCall.c_str() );
		args[ 6 ].SetBoolean( widget.m_bWindow );
		args[ 7 ].SetBoolean( widget.m_bModal );
		args[ 8 ].SetBoolean( widget.m_bEscCloseable );
		args[ 9 ].SetBoolean( widget.m_bCloseToDelete );
		args[ 10 ].SetInt( widget.m_wPosX );
		args[ 11 ].SetInt( widget.m_wPosY );
		args[ 12 ].SetInt( widget.m_wLayer );
		args[ 13 ].SetInt( widget.m_wAlignType );
		args[ 14 ].SetInt( widget.m_nOffsetX );
		args[ 15 ].SetInt( widget.m_nOffsetY );
		args[ 16 ].SetBoolean( widget.m_bSaveProp );
		args[ 17 ].SetBoolean( widget.m_bSnapping );
		pMovie->Invoke( "root.CF_RegUIList", &result, args, 18 );
	}

	m_bCreateCompleteGameMovie = true;

	//m_pUIMovie->Invoke( "root.CF_PreloadUI", "" );

	// ��ü UI ���İ�
	GfxInterface::GetInstance()->SetUIAlpha( GLWidgetSaveProp::GetInstance().fUIAlpha );

	// ��ƿ� �˸�
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CreateCoreComplete );
}

void GfxExternalInterface::OnCustomEvent( const GfxCallbackFuncValue& value )
{
	SetControlfp(true);

	if( 1 == value.argCount )
	{
		const char* szName = value.args[ 0 ].GetString();
		if( NULL == szName )
			return;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( szName );
	}
	else if( 3 == value.argCount )
	{
		const char* szName = value.args[ 0 ].GetString();
		if( NULL == szName )
			return;

		if( false == value.args[ 1 ].IsArray() ||
			false == value.args[ 2 ].IsArray() ||
			value.args[ 1 ].GetArraySize() != value.args[ 2 ].GetArraySize() )
			return;

		VEC_WIDGET_CALLFUNCVALUE vecCallFuncValue;

		int nSize = value.args[ 2 ].GetArraySize();
		for( int i = 0; i < nSize; ++i )
		{
			GFx::Value gfxType, gfxValue;
			value.args[ 1 ].GetElement( i, &gfxType );
			value.args[ 2 ].GetElement( i, &gfxValue );

			GLWidgetCallFuncValue callFuncValue;

			EMGFX_OBJECT_TYPE emType = (EMGFX_OBJECT_TYPE) int( gfxType.GetInt() );
			switch( emType )
			{
			case EMGFX_OBJ_TYPE_INT :
				{
					callFuncValue.emType = GLWidgetCallFuncValue::EMTYPE_INT;
					callFuncValue.nValue = (int) gfxValue.GetInt();
				}
				break;

			case EMGFX_OBJ_TYPE_UINT :
				{
					callFuncValue.emType = GLWidgetCallFuncValue::EMTYPE_UINT;
					callFuncValue.unValue = (unsigned int) gfxValue.GetUInt();
				}
				break;

			case EMGFX_OBJ_TYPE_NUMBER :
				{
					callFuncValue.emType = GLWidgetCallFuncValue::EMTYPE_DOUBLE;
					callFuncValue.dValue = (double) gfxValue.GetNumber();
				}
				break;

			case EMGFX_OBJ_TYPE_BOOL :
				{
					callFuncValue.emType = GLWidgetCallFuncValue::EMTYPE_BOOL;
					callFuncValue.bValue = gfxValue.GetBool();
				}
				break;

			case EMGFX_OBJ_TYPE_STRING :
				{
					callFuncValue.emType = GLWidgetCallFuncValue::EMTYPE_STRING;
					callFuncValue.strValue = gfxValue.GetString();
				}
				break;

			default :
				{
					callFuncValue.emType = GLWidgetCallFuncValue::EMTYPE_NULL;
				}
				break;
			}

			vecCallFuncValue.push_back( callFuncValue );
		}

		GLWidgetScript::GetInstance().LuaCallFunc< void >( szName, vecCallFuncValue );
	}

	SetControlfp(false);
}

void GfxExternalInterface::OnChatLinkClick( const GfxCallbackFuncValue& value )
{
	if( 1 != value.argCount )
		return;

	GLChatMan::GetInstance().OnNotifyClickLink( (int) value.args[ 0 ].GetNumber() );
}

void GfxExternalInterface::OnChatReferName( const GfxCallbackFuncValue& value )
{
	if( false == GLUseFeatures::GetInstance().IsUsingReferChar() )
		return;

	if( 1 != value.argCount )
		return;

	const char* szName = value.args[ 0 ].GetString();
	std::string strReq(sc::string::utf8_to_ansi( std::string(szName) ));
	GLWidgetScript::GetInstance().m_pGaeaClient->GetReferChar()->ReqReferChar( strReq.c_str() );
}

void GfxExternalInterface::OnChatWhisper( const GfxCallbackFuncValue& value )
{
	if( 1 != value.argCount )
		return;

	const char* szName = value.args[ 0 ].GetString();
	std::string strConv = GLWidgetScript::GetInstance().UTF8ToAnsi(szName);
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_BasicChat_SetWhisper,
		"-s", strConv.c_str() );
}

void GfxExternalInterface::OnPrintDebug( const GfxCallbackFuncValue& value )
{
	if( 2 != value.argCount )
		return;

	int nType = value.args[ 1 ].GetInt();

	WORD wColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	switch( nType )
	{
	case 0 :
		wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;

	case 1 :
		wColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	}

	GLWidgetScript::PrintDebug( value.args[ 0 ].GetString(), wColor );
}

void GfxExternalInterface::OnPrivateMarketClick( const GfxCallbackFuncValue& value )
{
	if( 1 != value.argCount )
		return;

	DWORD dwId = (DWORD) value.args[ 0 ].GetUInt();

	PrivateMarketClient* pPrivateMarket = GLWidgetScript::GetInstance().m_pGaeaClient->GetPrivateMarketClient();
	if( NULL != pPrivateMarket )
	{
		// ���λ��� �������̸� �ٸ� ��� ���λ��� �� �� ����.
		if( GLWidgetScript::GetInstance().LuaCallFunc<bool>(NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpener ) == true )
			return;

		// ���λ����� ���� �ִ� �����̸� �ٸ� ����� ���λ����� �� �� ����;
		if ( GLWidgetScript::GetInstance().LuaCallFunc<bool>(NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen ) == true )
			return;
		

		pPrivateMarket->ReqPMarketInfo( dwId );
		GfxDisplay::GetInstance().ToggleOffOtherPrivateMarket( dwId );

		GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
		if( pChar != NULL )
			pChar->InsertMarketClick(dwId);
	}
}

void GfxExternalInterface::OnItemNameClick( const GfxCallbackFuncValue& value )
{
	if( 2 != value.argCount )
		return;

	CROWREN dispName;
	DWORD dwId = (DWORD) value.args[ 0 ].GetUInt();
	EMCROW emCrow = (EMCROW) value.args[ 1 ].GetInt();

	if( false == GfxDisplay::GetInstance().GetItemNameDisplayName( dwId, emCrow, dispName ) )
		return;

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqPickupItem( &dispName );
}

void GfxExternalInterface::OnImageRelease( const GfxCallbackFuncValue& value )
{
	if( 1 != value.argCount )
		return;

	CString strPath = value.args[ 0 ].GetString();

	CString strMtf = strPath.Left( strPath.ReverseFind( '.' ) );
	strMtf += ".mtf";

	CFileSystem::getInstance().releaseMemory( strMtf.GetBuffer() );
}

void GfxExternalInterface::OnChangeResolution( const GfxCallbackFuncValue& value )
{
	const D3DPRESENT_PARAMETERS& pp = GfxInterface::GetInstance()->GetPresentParams();

	bool bResetUI = false;
	
	if( value.argCount != 0 )
		 bResetUI= value.args[ 0 ].GetBool();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ResetDevice,
		"-n, -n, -b",
		pp.BackBufferWidth, pp.BackBufferHeight, bResetUI );
}

void GfxExternalInterface::OnDebugSceneComplete( const GfxCallbackFuncValue& value )
{
	GFx::Movie* pMovie = GfxInterface::GetInstance()->GetDebugScene()->GetMovie();

	GFx::Value args[ 2 ], result;
	const MAP_WIDGET& vecWidget = GLWidgetsList::GetInstance().GetList();

	MAP_WIDGET_CITER iter = vecWidget.begin();
	for( ; iter != vecWidget.end(); ++iter )
	{
		const GLWidget& widget = iter->second;

		std::string strFileName = widget.m_strFileName;
		strFileName.append( NSGFXFILE::g_strExtension );

		args[ 0 ].SetString( widget.m_strFileName.c_str() );
		args[ 1 ].SetString( strFileName.c_str() );
		pMovie->Invoke( "root.CF_RegUI", &result, args, 2 );
	}

	pMovie->Invoke( "root.CF_EndRegUI", "" );
}

void GfxExternalInterface::OnToggleDebugScene( const GfxCallbackFuncValue& value )
{
	GfxInterface::GetInstance()->ToggleDebugScene();
}

void GfxExternalInterface::OnTextCopy( const GfxCallbackFuncValue& value )
{
	if( 1 != value.argCount )
		return;

	std::wstring str = value.args[ 0 ].GetStringW();
	sc::string::stringToClipboard( str );
}

// Data Adapter Call ---------------------------------------------------------------
void GfxExternalInterface::OnNotifyHoldStart( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 2 != params.ArgCount )
		return;

	if( false == params.pArgs[ 0 ].IsString() )
		return;

	const char* szSrcID = params.pArgs[ 0 ].GetString();

	GfxSlotBase* pSrcSlot = dynamic_cast< GfxSlotBase* >( pThis->GetGameDataBinder()->GetDataWrapper( szSrcID ) );
	if( NULL == pSrcSlot )
		return;

	holditem::SetHoldSlotBinding( szSrcID );

	bool bLock = params.pArgs[ 1 ].GetBool();

	pSrcSlot->HoldStart( bLock );
}

void GfxExternalInterface::OnNotifyHoldEnd( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 5 != params.ArgCount )
		return;

	if( false == params.pArgs[ 0 ].IsString() )
		return;

	const char* szHoldSlotID = params.pArgs[ 0 ].GetString();
	const char* szTargetSlotID = params.pArgs[ 2 ].IsString() ? params.pArgs[ 2 ].GetString() : NULL;
	bool bHoldSlotLock = params.pArgs[ 1 ].GetBool();
	bool bTargetSlotLock = params.pArgs[ 3 ].GetBool();
	bool bHold = params.pArgs[ 4 ].GetBool();

	GfxSlotBase* pHoldSlot = dynamic_cast< GfxSlotBase* >( pThis->GetGameDataBinder()->GetDataWrapper( szHoldSlotID ) );
	if( NULL == pHoldSlot )
		return;

	GfxSlotBase* pTargetSlot = dynamic_cast< GfxSlotBase* >( pThis->GetGameDataBinder()->GetDataWrapper( szTargetSlotID ) );

	// ���콺 Ŭ������ ���� ���ø�.
	if( true == bHold )
	{
		// ��� ��ó��
		if( true == pHoldSlot->PreDropSlotProc( pTargetSlot, bHoldSlotLock, bTargetSlotLock ) &&
			NULL != pTargetSlot )
		{
			// ���� ���� ��Ȯ�� ���Ҵٸ� ��� ��ó��
			if( false == pTargetSlot->PostDropSlotProc( pHoldSlot, bHoldSlotLock, bTargetSlotLock ) )
				holditem::ReleaseHoldItem();
		}
		else
		{
			// �ٸ� ���� �ʿ䰡 ������ �; �ϴ� ���⸸�ص�;
			if ( holditem::IsReserveDelete() == false )
				holditem::ReleaseHoldItem();
		}
	}
	// ���콺 �巡�׷� ���� ���ø�.
	else
	{
		// ���� ���� ��Ȯ�� �巡�� �ž���.
		if( NULL != pTargetSlot )
			pHoldSlot->DragSlotProc( pTargetSlot );

		holditem::ReleaseHoldItem();
	}

	pThis->GetGameDataBinder()->NotifyDataChange( szHoldSlotID );
	if( pHoldSlot != pTargetSlot )
		pThis->GetGameDataBinder()->NotifyDataChange( szTargetSlotID );
}

void GfxExternalInterface::OnNotifyHoldEndForce( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	const char* szHoldSlotID = params.pArgs[ 0 ].GetString();
	pThis->GetGameDataBinder()->NotifyDataChange( szHoldSlotID );
}

void GfxExternalInterface::OnNotifyHoldEndOutside( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 2 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	GLInputManager::GetInstance().DropHoldItem();
}

void GfxExternalInterface::OnNotifySlotLeftMouseClick( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 5 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	bool bLock = params.pArgs[ 1 ].GetBool();
	bool bCtrl = params.pArgs[ 2 ].GetBool();
	bool bAlt = params.pArgs[ 3 ].GetBool();
	bool bShift = params.pArgs[ 4 ].GetBool();

	pSlot->LeftMouseClick( bLock, bCtrl, bAlt, bShift );
}

void GfxExternalInterface::OnNotifySlotRightMouseClick( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 5 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	bool bLock = params.pArgs[ 1 ].GetBool();
	bool bCtrl = params.pArgs[ 2 ].GetBool();
	bool bAlt = params.pArgs[ 3 ].GetBool();
	bool bShift = params.pArgs[ 4 ].GetBool();

	pSlot->RightMouseClick( bLock, bCtrl, bAlt, bShift );
}

void GfxExternalInterface::OnGetSlotToolTip( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	GFx::Value valTooltip;
	params.pMovie->CreateObject( &valTooltip );

	if( true == pSlot->GetTooltip( &valTooltip ) )
		*(params.pRetVal) = valTooltip;
	else
		params.pRetVal->SetNull();
}

// ��ġ �ű���;
void GfxExternalInterface::OnGetLargeMapIconTooltip(GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params)
{
	if( 4 != params.ArgCount )
		return;

	//////////////////////////////////////////////////////////////////////////
	// param -CrowType/-NpcType/-ID/-MapID

	const INT nCrowType(atoi(params.pArgs[0].GetString()))
		, nCrowSubType(atoi(params.pArgs[1].GetString()))
		, nID(atoi(params.pArgs[2].GetString()))
		, nMapID(atoi(params.pArgs[3].GetString()));

	std::string strTooltip = GLMinimapClient::GetInstance().SfLargemap_MakeTooltip((EMCROW)nCrowType, nCrowSubType, nID, nMapID);
	std::wstring strW = GfxInterface::GetInstance()->EncodeText( strTooltip );

	params.pMovie->CreateObject(params.pRetVal);
	params.pRetVal->SetMember("tooltip", strW.c_str());
	// params.pRetVal->SetTextHTML();
}

void GfxExternalInterface::OnChatLogRefresh(GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params)
{
	const INT nLogID( GLWidgetScript::GetInstance().LuaCallFunc< INT >( NSWIDGET_SCRIPT::g_strFunc_BasicGetCurrentLogID ) )
		, nType( GLWidgetScript::GetInstance().LuaCallFunc< INT >( NSWIDGET_SCRIPT::g_strFunc_BasicGetCurrentChatType ) );
	GLWidgetScript::GetInstance().AddStoredChatType(nLogID, nType);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFuc_BasicChat_Refresh );
}

void GfxExternalInterface::OnGetSlotSubTooltip( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount || false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	params.pMovie->CreateObject( params.pRetVal );
	pSlot->GetSubTooltip( params.pRetVal );
}

void GfxExternalInterface::OnGetAddSlotCount( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	int nCount = 0;
	if( true == pSlot->GetAddSlotCount( nCount ) )
		params.pRetVal->SetInt( nCount );
	else
		params.pRetVal->SetInt( 0 );
}

void GfxExternalInterface::OnGetAddSlotBinding( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	std::string strBinding = "";
	if( true == pSlot->GetAddSlotBinding( strBinding ) )
		params.pMovie->CreateString( params.pRetVal, strBinding.c_str() );
	else
		params.pMovie->CreateString( params.pRetVal, "" );
}

void GfxExternalInterface::OnGetTradeSlotCount( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	int nCount = 0;
	if( true == pSlot->GetTradeSlotCount( nCount ) )
		params.pRetVal->SetInt( nCount );
	else
		params.pRetVal->SetInt( 0 );
}

void GfxExternalInterface::OnGetTradeSlotBinding( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() )
		return;

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( params.pArgs[ 0 ].GetString() ) );
	if( NULL == pSlot )
		return;

	std::string strBinding = "";
	if( true == pSlot->GetTradeSlotBinding( strBinding ) )
		params.pMovie->CreateString( params.pRetVal, strBinding.c_str() );
	else
		params.pMovie->CreateString( params.pRetVal, "" );
}

void GfxExternalInterface::OnShowTooltipSlot_bySlot( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 2 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsString() || false == params.pArgs[ 1 ].IsString() )
		return;

	std::string strSlotID = params.pArgs[ 0 ].GetString();
	std::string strTooltipSlotID = params.pArgs[ 1 ].GetString();

	GfxSlotBase* pSlot = dynamic_cast< GfxSlotBase* >( pThis->GetGameDataBinder()->GetDataWrapper( strSlotID.c_str() ) );
	if( NULL == pSlot )
		return;

	GfxTooltipSlot* pTooltipSlot = dynamic_cast< GfxTooltipSlot* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( strTooltipSlotID.c_str() ) );
	if( NULL == pTooltipSlot )
		return;

	pTooltipSlot->SetSlot( pSlot );
	pThis->GetGameDataBinder()->NotifyDataChange( strTooltipSlotID.c_str() );
}

void GfxExternalInterface::OnShowTooltipSlot_byID( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 3 != params.ArgCount )
		return;
	// slotID, BindSlotString, TooltipType;
	if( false == params.pArgs[ 0 ].IsUInt() || false == params.pArgs[ 1 ].IsString() || false == params.pArgs[ 2 ].IsInt() )
		return;

	DWORD dwSlotID = (DWORD)params.pArgs[0].GetInt();
	std::string strTooltipSlotID = params.pArgs[1].GetString();
	DWORD dwTooltipType = (DWORD)params.pArgs[2].GetInt();
	GfxTooltipSlot* pTooltipSlot = dynamic_cast< GfxTooltipSlot* >( 
		pThis->GetGameDataBinder()->GetDataWrapper( strTooltipSlotID.c_str() ) );
	if( NULL == pTooltipSlot )
		return;

	pTooltipSlot->SetTooltipType((EMGFX_TOOLTIP_TYPE)dwTooltipType);
	pTooltipSlot->SetNID(SNATIVEID(dwSlotID));
	pThis->GetGameDataBinder()->NotifyDataChange( strTooltipSlotID.c_str() );
}

void GfxExternalInterface::OnHideTooltipSlot( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HideTooltipSlot );
}

void GfxExternalInterface::OnGetMaxTooltipWidth( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	params.pRetVal->SetInt( NSWIDGET_SCRIPT::g_nDef_SlotTooltipMaxWidth );
}

void GfxExternalInterface::OnGetMinTooltipWidth( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	params.pRetVal->SetInt( NSWIDGET_SCRIPT::g_nDef_SlotTooltipMinWidth );
}

void GfxExternalInterface::OnRegisterMessageLogListener( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsClosure() )
		return;

	GFx::Value& handler = params.pArgs[ 0 ];
	pThis->m_vecChatLogHandlers.push_back( handler );
}

void GfxExternalInterface::OnUnregisterMessageLogListener( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;
	if( false == params.pArgs[ 0 ].IsClosure() )
		return;

	GFx::Value& handler = params.pArgs[ 0 ];

	VEC_GFXVALUE_ITER iter = pThis->m_vecChatLogHandlers.begin();
	for( ; iter != pThis->m_vecChatLogHandlers.end(); ++iter )
	{
		const GFx::Value& val = *iter;
		if( val == handler )
		{
			pThis->m_vecChatLogHandlers.erase( iter );
			break;
		}
	}
}

void GfxExternalInterface::OnRegisterRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 4 != params.ArgCount )
		return;

	if( false == params.pArgs[ 0 ].IsInt() 
		|| false == params.pArgs[ 1 ].IsString()
		|| false == params.pArgs[ 2 ].IsNumber()
		|| false == params.pArgs[ 3 ].IsNumber())
		return;

	int nTextureType = params.pArgs[ 0 ].GetInt()
		, nWidth = (INT)params.pArgs[ 2 ].GetNumber()
		, nHeight = (INT)params.pArgs[ 3 ].GetNumber();
	if( EMWIDGET_TEXTURE_NONE >= nTextureType || EMWIDGET_TEXTURE_SIZE <= nTextureType )
		return;

	std::string strLinkID = params.pArgs[ 1 ].GetString();
	if( strLinkID.empty() )
		return;

	// ���� �ؽ�ó ����
	GfxRenderTexture* pRenderTexture = 
		GLWidgetTextureMan::GetInstance().CreateTexture( EMWIDGET_TEXTURE( nTextureType ) );
	if( NULL == pRenderTexture )
		return;

	// �̹����� �ؽ�ó�� ��ü
	pRenderTexture->ReplaceGfxTexture( strLinkID, nWidth, nHeight );
}

void GfxExternalInterface::OnUnregisterRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;

	if( false == params.pArgs[ 0 ].IsInt() )
		return;

	int nTextureType = params.pArgs[ 0 ].GetInt();
	if( EMWIDGET_TEXTURE_NONE >= nTextureType || EMWIDGET_TEXTURE_SIZE <= nTextureType )
		return;

	GfxRenderTexture* pRenderTexture = 
		GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE( nTextureType ) );
	if( NULL == pRenderTexture )
		return;

	// ���۷��� ī��Ʈ ����
	if( 0 < pRenderTexture->GetRefCount() )
		pRenderTexture->ReduceRefCount();
}

void GfxExternalInterface::OnClubIconRenderTexture( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	params.pRetVal->SetBoolean( false );

	if( 3 != params.ArgCount )
		return;

	// 0 : LinkID
	// 1 : ServerID
	// 2 : ClubID

	if( false == params.pArgs[ 0 ].IsString() ||
		false == params.pArgs[ 1 ].IsInt() ||
		false == params.pArgs[ 2 ].IsInt() )
		return;

	GFx::MovieImpl* pMovie = (GFx::MovieImpl*) GfxInterface::GetInstance()->GetGameScene()->GetMovie();
	if( NULL == pMovie )
		return;

	// main.swf���� ���ҽ� ������.
	GFx::Resource* pRes = pMovie->GetMovieDef()->GetResource( params.pArgs[ 0 ].GetString() );
	if( NULL == pRes )
		return;

	// ������ ���ҽ��� �̹������� Ȯ��.
	ImageResource* pImageRes = NULL;
	if( GFx::Resource::RT_Image == pRes->GetResourceType() )
		pImageRes = (ImageResource*) pRes;

	if( NULL == pImageRes )
		return;

	// ������ �ؽ�ó ����
	const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData(
		GfxInterface::GetInstance()->GetDevice(),
		params.pArgs[ 1 ].GetInt(),
		params.pArgs[ 2 ].GetInt() );

	D3D9::HAL* pHAL = GetCore()->GetRenderHAL();
	D3D9::TextureManager* pTexMan = (D3D9::TextureManager*) pHAL->GetTextureManager();

	// GFX �ؽ��� ����
	Ptr< Scaleform::Render::Texture > pTexture = pTexMan->CreateTexture( sMarkData.pddsTexture, 
		ImageSize( NSGFX_DISPLAY::CLUB_MARK_SIZE_WIDTH, NSGFX_DISPLAY::CLUB_MARK_SIZE_HEIGHT ) );
	if( NULL != pTexture )
	{
		Ptr< TextureImage > pTextureImg = *new TextureImage( 
			Scaleform::Render::Image_R8G8B8A8, pTexture->GetSize(), 0, pTexture );

		// ���ε� �ؽ�ó �̹����� ����
		pImageRes->SetImage( pTextureImg );
		// ����
		pMovie->ForceUpdateImages();
		pHAL->ForceUpdateImages( &pMovie->GetRenderContext() );

		params.pRetVal->SetBoolean( true );
	}
}


void GfxExternalInterface::OnHtmlViewSetVisible( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 3 != params.ArgCount )
		return;

	if( false == params.pArgs[ 0 ].IsInt() ||
		false == params.pArgs[ 1 ].IsBool() ||
		false == params.pArgs[ 2 ].IsString() )
		return;

	int nWebID = GfxInterface::GetInstance()->GetHtmlWebID( (int) params.pArgs[ 0 ].GetInt() );
	if( -1 == nWebID )
		return;

	BOOL bVisible = true == params.pArgs[ 1 ].GetBool() ? TRUE : FALSE;
	const char* szAddress = params.pArgs[ 2 ].GetString();

	COMMON_WEB::SetVisible( nWebID, bVisible );
	COMMON_WEB::Navigate( nWebID, szAddress, bVisible, NULL );
	COMMON_WEB::SetFocusParent();
}

void GfxExternalInterface::OnSaveConversionMode( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	// ������ �Է� ��� ����
	pThis->m_strConversionMode = GfxInterface::GetInstance()->GetConversionMode();
}

void GfxExternalInterface::OnSetConversionMode( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	// TextInput�� ��Ŀ���� ���� ����Ƽ�� ���� �ڵ� ����ȴ�.
	// ������ �Է� �� ������ ��츸 ���� �����Ѵ�.
	if( pThis->m_strConversionMode == "ALPHANUMERIC_HALF" ||
		pThis->m_strConversionMode == "ALPHANUMERIC_FULL" )
	{
		GfxInterface::GetInstance()->SetConversionModeEn();
	}
}

void GfxExternalInterface::OnIsGameRun( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	params.pRetVal->SetBoolean( true );
}

void GfxExternalInterface::OnGetCompositionString( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( language::DEFAULT == RENDERPARAM::emLangSet ||
		language::KOREAN == RENDERPARAM::emLangSet )
	{
		const wchar_t* szCompositionStr = GetCore()->GetIME()->GetCompositionString();
		char szAnsi[4096] = {0};
		sc::string::unicodeToAnsi(szCompositionStr, szAnsi, 4096);
		params.pRetVal->SetStringW(szCompositionStr);
	}
	else
	{
		DXInputString* pInputString = GfxInterface::GetInstance()->GetGaeaClient()->GetEngineDevice()->GetInputString();
		params.pRetVal->SetString( pInputString->GetString() );
	}
}

void GfxExternalInterface::OnSetFocusInTextInput( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 1 != params.ArgCount )
		return;

	bool bFocus = params.pArgs[ 0 ].GetBool();
	GfxInterface::GetInstance()->SetFocusInTextInput( bFocus );

	if( language::DEFAULT != RENDERPARAM::emLangSet &&
		language::KOREAN != RENDERPARAM::emLangSet )
	{
		DXInputString* pInputString = GfxInterface::GetInstance()->GetGaeaClient()->GetEngineDevice()->GetInputString();
		if( true == bFocus )
		{
			pInputString->OnInput();
		}
		else
		{
			pInputString->OffInput();
		}
	}
}

void GfxExternalInterface::OnSaveProp( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 2 != params.ArgCount )
		return;

	WORD wWidgetID = (WORD) params.pArgs[ 0 ].GetInt();
	WORD wSubID = (WORD) params.pArgs[ 1 ].GetInt();

	GLWidgetSaveProp::GetInstance().SaveProperty( wWidgetID, wSubID );
}

void GfxExternalInterface::OnRegisterInterface( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 2 != params.ArgCount )
		return;

	WORD wIndex = (WORD) params.pArgs[ 0 ].GetInt();
	if( 0 > wIndex || GfxInterface::AS_Interface_Size <= wIndex )
		return;

	GFx::Value& func = params.pArgs[ 1 ];
	if( false == func.IsClosure() )
		return;

	GfxInterface::GetInstance()->m_pASInterface[ wIndex ].valInterface = func;
	GfxInterface::GetInstance()->m_pASInterface[ wIndex ].SetName( GfxInterface::EMAS_Interface( wIndex ) );
}

void GfxExternalInterface::OnRegisterDisplayInterface( GfxExternalInterface* pThis, const GFx::FunctionHandler::Params& params )
{
	if( 4 != params.ArgCount )
		return;

	GFx::Value& funcAddDisplay = params.pArgs[ 0 ];
	GFx::Value& funcChangeLayer = params.pArgs[ 1 ];
	GFx::Value& funcSetDamage = params.pArgs[ 2 ];
	GFx::Value& funcSortItemLayer = params.pArgs[ 3 ];

	if( false == funcAddDisplay.IsClosure() ||
		false == funcChangeLayer.IsClosure() ||
		false == funcSetDamage.IsClosure() ||
		false == funcSortItemLayer.IsClosure() )
		return;

	GfxDisplay::GetInstance().m_funcAddDisplay = funcAddDisplay;
	GfxDisplay::GetInstance().m_funcChangeLayer = funcChangeLayer;
	GfxDisplay::GetInstance().m_funcSetDamage = funcSetDamage;
	GfxDisplay::GetInstance().m_funcSortItemLayer = funcSortItemLayer;
}