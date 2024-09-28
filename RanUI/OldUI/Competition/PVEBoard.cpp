#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "PVEBoard.h"
#include "UIOrdinal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPVEBoard::CPVEBoard(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,   m_pOrdinal(0)
,	m_pDownBoard(0)
,   m_pResultImage(0)
,   m_pWaitAfterDeath(0)
,	m_fResultTimer(0.f)
{
	
}

CPVEBoard::~CPVEBoard()
{
}

bool CPVEBoard::CreateSubControl()
{
	if (m_pDownBoard == 0)
	{
		m_pDownBoard = CreateTextBox(18, _DEFAULT_FONT_SHADOW_FLAG, _T("COMPETITION_PVE_MESSAGE_TEXT"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

		if (m_pDownBoard == 0)
		{
			return false;
		}
	}

    if (m_pOrdinal == 0)
    {
        m_pOrdinal = new CUIOrdinal(m_pEngineDevice);

        if (m_pOrdinal == 0)
        {
            return false;
        }

        m_pOrdinal->CreateSub(this, _T("COMPETITION_PVE_STAGE_IMAGE"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
        m_pOrdinal->CreateSubControl();
        m_pOrdinal->SetVisibleSingle(FALSE);
        RegisterControl(m_pOrdinal);
    }

    if (m_pResultImage == 0)
    {
        m_pResultImage = new CSwapImage(m_pEngineDevice);

        if (m_pResultImage == 0)
        {
            return false;
        }

        m_pResultImage->CreateSub(this, _T("COMPETITION_PVE_RESULT_IMAGE"), UI_FLAG_DEFAULT);        
        m_pResultImage->SetVisibleSingle(FALSE);
        RegisterControl(m_pResultImage);
    }

    if (m_pWaitAfterDeath == 0)
    {
        m_pWaitAfterDeath = new CUIControl(m_pEngineDevice);

        if (m_pWaitAfterDeath == 0)
        {
            return false;
        }

        m_pWaitAfterDeath->CreateSub(this, _T("STAGE_WAITAFTERDEATH_IMAGE"), UI_FLAG_DEFAULT);
        m_pWaitAfterDeath->SetVisibleSingle(FALSE);
        RegisterControl(m_pWaitAfterDeath);
    }

	return true;
}

void CPVEBoard::SetOrdinalToUpBoard(const int nNum, bool bShow)
{
    if (m_pOrdinal)
    {
        m_pOrdinal->SetOrdinal(nNum, bShow);
        m_pOrdinal->SetVisibleSingle(bShow ? TRUE : FALSE);
    }
}

void CPVEBoard::SetTextToDownBoard(const TCHAR* pText, const D3DCOLOR& TextColor, bool bShow)
{
    if (m_pDownBoard && pText)
    {
        m_pDownBoard->SetText(pText, TextColor);
        m_pDownBoard->SetVisibleSingle(bShow ? TRUE : FALSE);
    }
}

CBasicTextBox* CPVEBoard::CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign)
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
    pText->SetVisibleSingle(FALSE);
	RegisterControl(pText);

	return pText;
}

void CPVEBoard::SetResult(ERESULT_PVE eResult, bool bShow)
{
    if (m_pResultImage == 0 || m_pWaitAfterDeath == 0)
    {
        return;
    }

    if (eResult == ERESULT_SUCCEED)
    {
        m_pResultImage->SetImage(_T("STAGE_SUCCEED_IMAGE"));
    }

    if (eResult == ERESULT_FAILED)
    {
        m_pResultImage->SetImage(_T("STAGE_FAILED_IMAGE"));
    }

	bool bResult = (eResult != ERESULT_WAITAFTERDEATH);
	bool bWaitAfterDeath = !bResult;

	bResult = bShow ? bResult : false;
	bWaitAfterDeath = bShow ? bWaitAfterDeath : false;	

    m_pResultImage->SetVisibleSingle(bResult);
    m_pWaitAfterDeath->SetVisibleSingle(bWaitAfterDeath);

	(bShow && bResult) ? m_fResultTimer = 3.f : m_fResultTimer = 0.f;	
}

void CPVEBoard::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	if (m_fResultTimer > 0.f)
	{
		m_fResultTimer -= fElapsedTime;

		if (m_fResultTimer <= 0.f)
		{
			m_fResultTimer = 0.f;
			SetResult(ERESULT_SUCCEED, false);
		}
	}
}