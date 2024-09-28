#pragma once

#include <GUIBase/IWindow.h>
#include <GUIBase/UIEvent.h>
#include "UIWebView.h"

class GLGaeaClient;
class EngineDeviceMan;

namespace HTML5UI
{

class CInventoryWindow : public GUIBase::IWindow
{
public:
	enum
	{
		ESlotX				= 6,
		ESlotY				= 10,
		EPageMax			= 5, 

		EInvenX				= 6,
		EInvenY				= 10,
		EInvenPage			= 5, // 기존 인벤 페이지 3개에서 5래로 변경 2015.01.26
	};

	enum INVENTORY_TAB_LIST
	{
		INVENTAB,
		BANKTAB,
		WISHLISTTAB
	};

	struct INVEN_PAGE
	{
		INVENTORY_TAB_LIST tabList;		// 현재 탭;
		int nPage;						// 현재 탭의 페이지;
	};

	CInventoryWindow( GUIBase::UIID uiID
		, const std::string& uiStrID
		, GUIBase::UIRECT rcParentGlobalPos
		, GUIBase::WINDOW_CFG windowCFG
		, GUIBase::IWebView* webView
		, GLGaeaClient* pGaeaClient
		, EngineDeviceMan* pEngineDevice );
	~CInventoryWindow();

public:
	/// 초기화 & 파괴;
	virtual void Initialize();
	virtual void Destroy();

	virtual bool OnMouseEvent( std::tr1::shared_ptr<GUIBase::CUIMouseEvent> mouseEvent );

	/// Loop
	virtual bool FrameMove( float timeDelta );

	/// MsgProc
	virtual bool MsgProc( const GUIBase::CALLBACK_MESSAGE msg );

private:
	GLGaeaClient* m_pGaeaClient;
	EngineDeviceMan* m_pEngineDevice;

	INVEN_PAGE m_eachPageInfo;
};

}