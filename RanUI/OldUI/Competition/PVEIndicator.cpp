#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "UINumberTimer.h"
#include "UIStageIndicator.h"
#include "PVEIndicator.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPVEIndicator::CPVEIndicator(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,   m_pTimerIndicator(0)
,   m_pStageIndicator(0)
,   m_pMobIndicator(0)
,   m_pMoveIndicator(0)
{
	
}

CPVEIndicator::~CPVEIndicator()
{

}

bool CPVEIndicator::CreateSubControl()
{
	if (m_pTimerIndicator == 0)
	{
		m_pTimerIndicator = new CUINumberTimer(m_pEngineDevice);

		if (m_pTimerIndicator == 0)
		{
			return false;
		}

		m_pTimerIndicator->CreateSub(this, _T("CONTROL_INDICATOR_TIMER"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
		m_pTimerIndicator->CreateSubControl();
		RegisterControl(m_pTimerIndicator);
		m_pTimerIndicator->SetVisibleSingle(FALSE);
	}

	if (m_pStageIndicator == 0)
	{
		m_pStageIndicator = new CUIStageIndicator(m_pEngineDevice);

		if (m_pStageIndicator == 0)
		{
			return false;
		}
		
		m_pStageIndicator->CreateSub(this, _T("CONTROL_INDICATOR_STAGE"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
		m_pStageIndicator->CreateSubControl();
		RegisterControl(m_pStageIndicator);
		m_pStageIndicator->SetVisibleSingle(FALSE);
	}

	if (m_pMobIndicator == 0)
	{
		m_pMobIndicator = CreateTextBox(15, _DEFAULT_FONT_SHADOW_FLAG, _T("COMPETITION_PVE_MOB_TEXT"), UI_FLAG_CENTER_X, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

		if (m_pMobIndicator == 0)
		{
			return false;
		}
	}

    if (m_pMoveIndicator == 0)
    {
        m_pMoveIndicator = CreateTextBox(15, _DEFAULT_FONT_SHADOW_FLAG, _T("COMPETITION_PVE_MOB_TEXT"), UI_FLAG_CENTER_X, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

        if (m_pMoveIndicator == 0)
        {
            return false;
        }
    }

	return true;
}

CBasicTextBox* CPVEIndicator::CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign)
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

void CPVEIndicator::SetTime(int nSec, bool bBlank, bool bShow)
{
 	if (m_pTimerIndicator)
 	{
		m_pTimerIndicator->SetBlank(bBlank);
 		m_pTimerIndicator->SetTime(nSec);
 		m_pTimerIndicator->SetVisibleSingle(bShow ? TRUE : FALSE);
 	}
}

void CPVEIndicator::SetStage(int nStage, int nEnd, int nRange, bool bShow)
{
	if (m_pStageIndicator)
	{
		m_pStageIndicator->SetStage(nStage);

        int nStart = 0;

        if (nRange > 0)
        {
            nStart = ((nStage - 1) / nRange) * nRange + 1;
        }

		m_pStageIndicator->SetRange(nStart, nEnd, nRange);
		m_pStageIndicator->SetVisibleSingle(bShow ? TRUE : FALSE);
	}
}

void CPVEIndicator::SetMonster(int nRem, int nAll, bool bShow)
{
	if (m_pMobIndicator)
	{
		TCHAR temp[256] = {0};
		_stprintf_s(temp, 256, ID2GAMEWORD(_T("COMPETITION_PVE"), 15), nRem, nAll);
		m_pMobIndicator->SetTextNoSplit(temp, NS_UITEXTCOLOR::YELLOW);
        m_pMobIndicator->SetVisibleSingle(bShow ? TRUE : FALSE);
	}

	Align();
}

void CPVEIndicator::SetPositioning(int nX, int nY, int nNow, int nNeed, bool bShow)
{
    if (m_pMoveIndicator)
    {
        TCHAR temp[256] = {0};
        //_stprintf_s(temp, 256, "위치:%d/%d  인원:%d/%d ", nX, nY, nNow, nNeed);
        //_stprintf_s(temp, 256, ID2GAMEWORD(_T("COMPETITION_PVE"), 25), nX, nY);
        std::string strTemp = sc::string::format( ID2GAMEWORD(_T("COMPETITION_PVE"), 25), nX, nY, nNow, nNeed );
        m_pMoveIndicator->SetTextNoSplit(strTemp.c_str(), NS_UITEXTCOLOR::YELLOW);
        m_pMoveIndicator->SetVisibleSingle(bShow ? TRUE : FALSE);
    }

	Align();
}

void CPVEIndicator::Align()
{
	bool bVisibleMob = false;
	bool bVisibleMove = false;
	UIRECT rt;

	if (m_pMobIndicator)
	{		
		rt = m_pMobIndicator->GetGlobalPos();

		bVisibleMob = m_pMobIndicator->IsVisible() == TRUE;
	}

	if (m_pMoveIndicator)
	{
		bVisibleMove = m_pMoveIndicator->IsVisible() == TRUE;
	}

	/**
		몹 상태를 보여 주고 있다면
		이동 포인터를 그 아래 보여 주고 그렇지 않으면
		몹 상태에 이동 포인터를 출력해 준다.

	 */
	if (bVisibleMove)
	{
		if (bVisibleMob)
		{
			m_pMoveIndicator->SetGlobalPos(UIRECT(rt.left, rt.top + rt.sizeY, rt.sizeX, rt.sizeY));
		}
		else
		{
			m_pMoveIndicator->SetGlobalPos(rt);
		}
	}
}
