
#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../InnerInterface.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../Util/UIWindowEx.h"

#include "FlyCameraControlUIDefine.h"
#include "FlyCameraControlPage.h"
#include "FlyCameraControlKeyframePage.h"
#include "FlyCameraControlWindow.h"

/*
GameWord - FLYCAMERACONTROL_WINDOW_TEXT
UiXmlList - Path
*/

FlyCameraControlWindow::FlyCameraControlWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pFlyCameraControlPage(NULL)
, m_pFlyCameraKeyframePage(NULL)
{
}

void FlyCameraControlWindow::SetVisibleSingle(BOOL bVisible)
{
	CUIWindowEx::SetVisibleSingle(bVisible);
}

void FlyCameraControlWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);	
}

void FlyCameraControlWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(FLYCAMERACONTROL_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray("FLYCAMERACONTROL_WINDOW"
		, (char*)ID2GAMEWORD("FLYCAMERACONTROL_WINDOW_TEXT", 0));
	
	// Page;
	CreateSubControl();

	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_RIGHT);
	m_pInterface->UiRegisterControl(this, true);
	m_pInterface->UiShowGroupFocus(FLYCAMERACONTROL_WINDOW);
}

void FlyCameraControlWindow::CreateSubControl(void)
{
	// Camera Control;
	m_pFlyCameraControlPage = new FlyCameraControlPage(m_pGaeaClient, m_pEngineDevice);
	m_pFlyCameraControlPage->CreateSub(this
		, "FLYCAMERACONTROL_PAGE"
		, UI_FLAG_DEFAULT
		, IDD_FLYCAMERACONTROL_WINDOW_MOVEROTATE_PAGE);
	m_pFlyCameraControlPage->CreateSubControl();
	RegisterControl(m_pFlyCameraControlPage);

	// Keyframe page;
	m_pFlyCameraKeyframePage = new FlyCameraControlKeyframePage(m_pGaeaClient, m_pEngineDevice);
	m_pFlyCameraKeyframePage->CreateSub(this
		, "FLYCAMERACONTROL_KEYFRAMEPAGE"
		, UI_FLAG_DEFAULT
		, IDD_FLYCAMERACONTROL_WINDOW_KEYFRAME_PAGE);
	m_pFlyCameraKeyframePage->CreateSubControl();
	RegisterControl(m_pFlyCameraKeyframePage);
}

void FlyCameraControlWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage(ControlID, dwMsg);
}

const D3DXVECTOR3& FlyCameraControlWindow::GetLastInputPos(void)
{
	return m_pFlyCameraKeyframePage->GetLastInputPos();
}