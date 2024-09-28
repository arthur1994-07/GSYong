#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"
#include "./DPSPage/DPSPage.h"
#include "./DetailPage/DetailPage.h"
#include "DPSWindow.h"

#define fDPS_AUTO_SAVE_TIME 300.0f

DPSWindow::DPSWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_fElapsedTime(0.0f)
{
}

void DPSWindow::SetVisibleSingle(BOOL bVisible)
{
	if ( bVisible == FALSE )
	{
		if ( m_pDPSPage->isSelected() == true )
		{
			m_pDPSPage->ResetSelectIndex();
			return;
		}
		m_pGaeaClient->saveCSVActorDamageRecord();
	}
	else
		m_fElapsedTime = 0.0f;

	CUIWindowEx::SetVisibleSingle(bVisible);
}

void DPSWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

// 	m_fElapsedTime += fElapsedTime; // 5분에 한번씩 자동 저장;
// 	if ( m_fElapsedTime > fDPS_AUTO_SAVE_TIME )
// 	{
// 		m_pGaeaClient->saveCSVActorDamageRecord();
// 		m_fElapsedTime = 0.0f;
// 	}
		
	const UIRECT& OldPosition = GetLocalPos();	
	if ( m_pDPSPage->isSelected() == false )
		m_pDetailPage->SetVisibleSingle(FALSE);
	else
	{		
		const DWORD nSelectActorIndex= m_pDPSPage->GetSelectIndex();
		m_pDetailPage->SetActorIndex(nSelectActorIndex);
		m_pDetailPage->SetVisibleSingle(TRUE);		
	}
}

void DPSWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create(DPS_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateBaseWindowLightGray("DPS_WINDOW", (char*)ID2GAMEWORD("DPS_WINDOW_TEXT", 0));
	CreateSubControl();
	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_RIGHT);
	m_pInterface->UiRegisterControl(this, true);
	m_pInterface->UiShowGroupFocus(DPS_WINDOW);
}

void DPSWindow::CreateSubControl(void)
{
	m_pDPSPage = new DPSPage(m_pEngineDevice, m_pGaeaClient);
	m_pDPSPage->CreateSub(this, "DPS_WINDOW_DPS_PAGE", UI_FLAG_DEFAULT);	
	m_pDPSPage->CreateSubControl();	
	RegisterControl(m_pDPSPage);
	
	m_pDetailPage = new DetailPage(m_pEngineDevice, m_pGaeaClient);
	m_pDetailPage->CreateSub(this, "DPS_WINDOW_DETAIL_PAGE", UI_FLAG_DEFAULT);	
	m_pDetailPage->CreateSubControl();	
	RegisterControl(m_pDetailPage);
}

