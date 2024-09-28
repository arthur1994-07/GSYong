#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "PVEEntranceState.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPVEEntranceState::CPVEEntranceState(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,	m_pState(0)
,	m_pTime(0)
,	m_nTimer(0)
,	m_fCur(0.f)
,	m_nAll(0)
{
	
}

CPVEEntranceState::~CPVEEntranceState()
{
}

bool CPVEEntranceState::CreateSubControl()
{
	if (m_pState == 0)
	{
		m_pState = CreateTextBox(12, _DEFAULT_FONT_SHADOW_FLAG, _T("COMPETITION_PVE_ENTRANCE_STATE_TEXT"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

		if (m_pState == 0)
		{
			return false;
		}
	}

	if (m_pTime == 0)
	{
		m_pTime = CreateTextBox(10, _DEFAULT_FONT_SHADOW_FLAG, _T("COMPETITION_PVE_ENTRANCE_TIME_TEXT"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

		if (m_pTime == 0)
		{
			return false;
		}
	}

	return true;
}

CBasicTextBox* CPVEEntranceState::CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign)
{
	CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);

	if (pText == 0)
	{
		return pText;
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, dwFontSize, dwFlags);

	if (pFont == 0)
	{
		return pText;
	}

	pText->CreateSub(this, pControl, wUIAlign);
	pText->SetFont(pFont);
	pText->SetTextAlign(nTextAlign);
	RegisterControl(pText);

	return pText;
}

void CPVEEntranceState::SetEntranceState(int nNum, int nAll, int nSec)
{
	SetPerson(nNum, nAll);
    SetTime(nSec);
}

void CPVEEntranceState::SetTime(int nSec)
{
	if (m_pTime)
	{
		TCHAR temp[256] = {0};
		const TCHAR* pStr = ID2GAMEWORD(_T("COMPETITION_PVE"), 7);
		_stprintf_s(temp, 256, pStr, nSec / 60, nSec % 60);
		m_pTime->SetText(temp, NS_UITEXTCOLOR::PALEGREEN);
	}
}

void CPVEEntranceState::SetPerson(int nNum, int nAll)
{
	if (m_pState)
	{
		TCHAR temp[256] = {0};
		const TCHAR* pStr = ID2GAMEWORD(_T("COMPETITION_PVE"), 6);
		_stprintf_s(temp, 256, pStr, nNum, nAll);
		m_pState->SetText(temp, NS_UITEXTCOLOR::ORANGE);
	}
}

void CPVEEntranceState::OnBeCompleted()
{
	SetVisibleSingle(FALSE);
}
