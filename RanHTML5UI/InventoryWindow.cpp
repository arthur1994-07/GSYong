#include "StdAfx.h"
#include "InventoryWindow.h"

#include <GUIBase/UIManager.h>
#include <GUIBase/UIEventManager.h>
#include <GUIBase/UIWindowManager.h>

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/DeviceMan.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanUI/OldUI/Interface/SkillFunc.h"

namespace HTML5UI
{

static float InvokeTime = 0.0f;

// 숫자를 문자로;
static std::string StringToFloat( float value )
{
	CString str;
	str.Format ( "%f", value );
	return str.GetString();
};

static std::string StringToInt( int value )
{
	CString str;
	str.Format ( "%d", value );
	return str.GetString();
};

CInventoryWindow::CInventoryWindow(  GUIBase::UIID uiID
						 , const std::string& uiStrID
						 , GUIBase::UIRECT rcParentGlobalPos
						 , GUIBase::WINDOW_CFG windowCFG
						 , GUIBase::IWebView* webView
						 , GLGaeaClient* pGaeaClient
						 , EngineDeviceMan* pEngineDevice )
						 : GUIBase::IWindow( uiID, uiStrID, rcParentGlobalPos, windowCFG, webView )
						 , m_pGaeaClient( pGaeaClient )
						 , m_pEngineDevice( pEngineDevice )
{
	m_eachPageInfo.tabList = INVENTAB;
	m_eachPageInfo.nPage = 0;
}

CInventoryWindow::~CInventoryWindow() { }

/// 초기화 & 파괴;
void CInventoryWindow::Initialize() { ShowComponent(); }

void CInventoryWindow::Destroy() { }

bool CInventoryWindow::OnMouseEvent( std::tr1::shared_ptr<GUIBase::CUIMouseEvent> mouseEvent )
{
	if( !GUIBase::IWindow::OnMouseEvent( mouseEvent ) )
		return false;

	InvokeTime = timeGetTime()*0.001f;

	return true;
}

/// Loop
bool CInventoryWindow::FrameMove( float timeDelta )
{
	// 캐릭터 정보 가져오기;
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return false;

	Json::Value sendRoot;

	/// Point가 갱신될 때만 Invoke;
	static __int64 point = 0;
	if( point != pChar->PointShopPoint() )
	{
		InvokeTime = timeGetTime()*0.001f;
		point = pChar->PointShopPoint();
		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			point, 3, "," );

		sendRoot["message"] = "UPDATE_POINT";
		sendRoot["point"] = strText.GetString();

		// Update Point
		FireInvokeEvent( sendRoot );
	}

	/// 돈이 갱신될 때만 Invoke;
	static LONGLONG money = 0;
	if( money != pChar->GetInvenMoney() )
	{
		InvokeTime = timeGetTime()*0.001f;
		money = pChar->GetInvenMoney();
		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			money, 3, "," );

		sendRoot["message"] = "UPDATE_MONEY";
		sendRoot["money"] = strText.GetString();

		// Update Money
		FireInvokeEvent( sendRoot );
	}

	return true;
}

static void GetInvenItem(  GUIBase::UIID uiID
						 , Json::Value& sendRoot
						 , GLInventory& cInventory
						 , int x, int y )
{
	SINVENITEM* item = cInventory.GetItem( x, y );
	if( item )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem(
			item->GetNativeID() );
		if( !pItem )
			return;

		Json::Value itemSlot;

		// Invoke Message ( Insert Item )
		SNATIVEID sICONINDEX = pItem->sBasicOp.sICONID;
		const char* szTexture = pItem->GetInventoryFile();
		SNATIVEID sNativeID = pItem->sBasicOp.sNativeID ;
		std::string strTexture = szTexture;
		strTexture.resize( strTexture.size()-4 );
		strTexture.insert( strTexture.size(), ".png" );
		UIRECT uiRect = NS_SKILL::GetIconTexurePos ( sICONINDEX );

		sendRoot["message"] = "UPDATE_INVENTORY_ITEMSLOT";
		sendRoot["x"] = x;
		sendRoot["y"] = y;
		sendRoot["name"] = strTexture;
		sendRoot["left"] = uiRect.left;
		sendRoot["top"] = uiRect.top;
		sendRoot["width"] = uiRect.sizeX;
		sendRoot["height"] = uiRect.sizeY;

		// Initialize Inventory
		GUIBase::IWindow* window = GUIBase::CUIWindowManager::GetInstance()
			.GetWindow( uiID );
		if( window )
			window->FireInvokeEvent( sendRoot );
	}
}

/// MsgProc
bool CInventoryWindow::MsgProc( const GUIBase::CALLBACK_MESSAGE msg )
{
	// 캐릭터 정보 가져오기;
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if( !pChar )
		return false;
	GLInventory& cInventory = pChar->m_cInventory;

	if( msg.message == "LOG" )
	{
		m_pGaeaClient->PrintConsoleText( msg.text );
		m_pGaeaClient->PrintMsgDlg(0xffff0000, StringToFloat( timeGetTime()*0.001f - InvokeTime ) );
	}
	else if( msg.message == "INITIALIZE" )
	{
		InvokeTime = timeGetTime()*0.001f;
		Json::Value sendRoot;

		// 초기화;
		sendRoot["message"] = "INITIALIZE";
		sendRoot["title"] = ID2GAMEWORD( "INVENTORY_NAME_STATIC" );
		sendRoot["tabNames"].append( ID2GAMEWORD( "INVENTORY_TAP_NAME", 0 ) );
		sendRoot["tabNames"].append( ID2GAMEWORD( "INVENTORY_TAP_NAME", 1 ) );
		sendRoot["tabNames"].append( ID2GAMEWORD( "INVENTORY_TAP_NAME", 2 ) );
		FireInvokeEvent( sendRoot );

		// Update Point
		CString strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			pChar->PointShopPoint(), 3, "," );

		sendRoot["message"] = "UPDATE_POINT";
		sendRoot["point"] = strText.GetString();

		FireInvokeEvent( sendRoot );

		// Update Money
		strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(
			pChar->GetInvenMoney(), 3, "," );

		sendRoot["message"] = "UPDATE_MONEY";
		sendRoot["money"] = strText.GetString();

		FireInvokeEvent( sendRoot );

		// Update ItemSlot
		for( int x=0; x<ESlotX; ++x )
			for( int y=0; y<ESlotY*EInvenPage; ++y )
				GetInvenItem( m_uiID, sendRoot, cInventory, x, y );
	}
	else if( msg.message == "UPDATE_ITEMPICK" )
	{
		int x = msg.x;
		int y = msg.y;
		int nNum = msg.x + msg.y * ESlotX;
		nNum += m_eachPageInfo.nPage * ESlotX * ESlotY;

		x = nNum % EInvenX;
		y = nNum / EInvenX;
		bool bSplitable = pChar->IsInvenSplitItem(
			WORD( x ), WORD( y ) );
		pChar->ReqInvenTo( WORD( x ), WORD( y ) );
	}
	else if( msg.message == "UPDATE_ITEMDROP" )
	{
		int x = msg.x;
		int y = msg.y;

		int nNum = x + y * ESlotX;
		nNum += m_eachPageInfo.nPage * ESlotX * ESlotY;

		x = nNum % EInvenX;
		y = nNum / EInvenX;
		bool bSplitable = pChar->IsInvenSplitItem(
			WORD( x ), WORD( y ) );
		pChar->ReqInvenTo( WORD( x ), WORD( y ) );
	}
	else if( msg.message == "CHANGE_INVENTORY_TAP" )
	{
		m_eachPageInfo.tabList = INVENTAB;
		m_eachPageInfo.nPage = msg.value;
	}
	else if( msg.message == "CHANGE_BANK_TAP" )
	{
		m_eachPageInfo.tabList = BANKTAB;
		m_eachPageInfo.nPage = msg.value;
		m_pGaeaClient->GetCharacter()->ReqItemBankInfo(false);
	}
	else if( msg.message == "CHANGE_WISHLIST_TAP" )
	{
		m_eachPageInfo.tabList = WISHLISTTAB;
		m_eachPageInfo.nPage = msg.value;
	}
	else if( msg.message == "CHANGE_RADIOBUTTON" )
	{
		m_eachPageInfo.nPage = msg.value;
	}
	else if( msg.message == "CLICK_SORTBUTTON" )
	{
		InvokeTime = timeGetTime()*0.001f;
		if( pChar->IsEnalbeItemSort() )
		{
			pChar->ReqInvenItemSort();
		}
	}

	return true;
}

}