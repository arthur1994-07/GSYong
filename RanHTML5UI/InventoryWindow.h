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
		EInvenPage			= 5, // ���� �κ� ������ 3������ 5���� ���� 2015.01.26
	};

	enum INVENTORY_TAB_LIST
	{
		INVENTAB,
		BANKTAB,
		WISHLISTTAB
	};

	struct INVEN_PAGE
	{
		INVENTORY_TAB_LIST tabList;		// ���� ��;
		int nPage;						// ���� ���� ������;
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
	/// �ʱ�ȭ & �ı�;
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