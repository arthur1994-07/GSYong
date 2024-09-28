#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "UIProgressTimer.h"
#include "PVERetry.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPVERetry::CPVERetry(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CUIWindowEx(pInterface, pEngineDevice)
,	m_pGaeaClient(pGaeaClient)
,   m_pClock(0)
,	m_pTimer(0)
,	m_pText(0)
,	m_pOk(0)
,	m_pCancel(0)
{

}

CPVERetry::~CPVERetry()
{
}

bool CPVERetry::CreateSubControl()
{
    if (m_pClock == 0)
    {
        m_pClock = new CSwapImage(m_pEngineDevice);
    }

    if (m_pClock == 0)
    {
        return false;
    }

    m_pClock->CreateSub(this, _T("TIMER_IMAGE"));
    RegisterControl(m_pClock);

	if (m_pTimer == 0)
	{
		m_pTimer = new CUIProgressTimer(m_pEngineDevice, this);
        m_pTimer->SetWndID(CONTROL_ID_TIMER);
	}

	if (m_pTimer == 0)
	{
		return false;
	}

    CUIControl ktemp(m_pEngineDevice);
    ktemp.Create(0, "COMPETITION_PVE_RETRY_TIMER");
    UIRECT rt = ktemp.GetLocalPos();
    m_pTimer->Create(rt.left, rt.top);	
	RegisterControl(m_pTimer);

	if (m_pText == 0)
	{
		m_pText = new CBasicTextBox(m_pEngineDevice);
	}

	if (m_pText == 0)
	{
		return false;
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);

	if (pFont == 0)
	{
		return false;
	}

	m_pText->CreateSub(this, "COMPETITION_PVE_RETRY_TEXT");
	m_pText->SetFont(pFont);
	m_pText->SetTextAlign(TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);	
	RegisterControl(m_pText);

	if (m_pOk == 0)
	{
		m_pOk = CreateButton("COMPETITION_PVE_RETRY_OK", ID2GAMEWORD("COMPETITION_PVE", 4), CONTROL_ID_BUTTON_OK);
	}

	if (m_pOk == 0)
	{
		return false;
	}

	if (m_pCancel == 0)
	{
		m_pCancel = CreateButton("COMPETITION_PVE_RETRY_CANCEL", ID2GAMEWORD("COMPETITION_PVE", 5), CONTROL_ID_BUTTON_CANCEL);
	}

	if (m_pCancel == 0)
	{
		return false;
	}

	return true;
}

CBasicTextButton* CPVERetry::CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id)
{
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);

	if (pButton == 0)
	{
		return pButton;
	}

	pButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, id);
	pButton->CreateBaseButton(name, CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, text);
	RegisterControl(pButton);	

	return pButton;
}

void CPVERetry::Open(LONGLONG nMoney, int nTime)
{
	if (m_pText)
	{
		TCHAR temp[256] = {0};
		_stprintf_s(temp, 256, ID2GAMEINTEXT("COMPETITION_PVE_RETRY_TEXT", 0), nMoney);
		m_pText->SetText(temp, NS_UITEXTCOLOR::WHITE);
	}

    if (m_pTimer)
    {
        m_pTimer->Start(nTime);
    }

    if (m_pInterface)
    {
        m_pInterface->UiShowGroupFocus(COMPETITION_PVERETRY);
    }
}

void CPVERetry::SetNewInstance ( bool bNewInstance, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID )
{
	m_bNewInstance = bNewInstance;
	m_nInstanceID = nInstanceID;
	m_sKeyMapID = sKeyMapID;
}

void CPVERetry::SetVisibleSingle(BOOL bVisible)
{
    CUIWindowEx::SetVisibleSingle(bVisible);

	/*
    if (bVisible)
    {
        m_pInterface->UISetModalGroup(COMPETITION_PVERETRY, false);
    }
    else
    {
        m_pInterface->UISetModalGroup(COMPETITION_PVERETRY);
    }
	*/
}

void CPVERetry::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroup::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case CONTROL_ID_BUTTON_OK:
	case CONTROL_ID_BUTTON_CANCEL:
		{
			if (CHECK_MOUSE_IN(dwMsg))
			{
				if (CHECK_LB_UP_LIKE(dwMsg))
				{
					if (ControlID == CONTROL_ID_BUTTON_OK)
					{
						OnOk();
					}
					else
					{
						OnCancel();
					}
				}
			}
		}
		break;

    case CONTROL_ID_TIMER:
        {
            if (dwMsg == CUIProgressTimer::PT_TIMEOVER)
            {
                Close();
            }
        }
        break;
	}
}

void CPVERetry::OnOk()
{
    if (m_pGaeaClient)
    {
		if ( m_bNewInstance )
			m_pGaeaClient->ReqNewInstanceRetry( true, m_nInstanceID, m_sKeyMapID );
		else
			m_pGaeaClient->ReqPVERetry(true);
    }

    Close();
}

void CPVERetry::OnCancel()
{
    if (m_pGaeaClient)
    {
		if ( m_bNewInstance )
			m_pGaeaClient->ReqNewInstanceRetry( false, m_nInstanceID, m_sKeyMapID );
		else
			m_pGaeaClient->ReqPVERetry(false);
    }

    Close();
}

void CPVERetry::Close()
{
    if (m_pInterface)
    {
        m_pInterface->UiHideGroup(COMPETITION_PVERETRY);
    }
}
