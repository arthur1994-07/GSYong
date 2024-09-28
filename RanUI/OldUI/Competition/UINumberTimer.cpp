#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "UINumberTimer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define DEF_OFFSET_CLOCK 4

CUINumberTimer::CUINumberTimer(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,	m_pClock(0)
,   m_pDigitImg(0)
,   m_pColonImg(0)
,	m_pMinusImg(0)
,   m_nUnitsDigitMin(0)
,   m_nTensDigitMin(0)
,   m_nUnitsDigitSec(0)
,   m_nTensDigitSec(0)
,	m_bBlank(false)
,	m_dwDuringTick(0)
,	m_dwPreTick(0)
,	m_SavedTime(0)
,	m_isTimerImage(true)
{
	
}

CUINumberTimer::~CUINumberTimer()
{
}

bool CUINumberTimer::CreateSubControl()
{
	m_pClock = CreateControl(_T("STAGE_TIMER_IMAGE"), this);

	if (m_pClock == 0)
	{
		return false;
	}	

    if (m_pDigitImg == 0)
    {
        m_pDigitImg = new CSwapImage(m_pEngineDevice);

        if (m_pDigitImg == 0)
        {
            return false;
        }

        m_pDigitImg->CreateSub(this, _T("STAGE_TIMER_NUMBER_IMAGE0"), UI_FLAG_DEFAULT);        
        RegisterControl(m_pDigitImg);
    }

    if (m_pColonImg == 0)
    {
        m_pColonImg = new CSwapImage(m_pEngineDevice);

        if (m_pColonImg == 0)
        {
            return false;
        }

        m_pColonImg->CreateSub(this, _T("STAGE_TIMER_CONLON_IMAGE"), UI_FLAG_DEFAULT);
        RegisterControl(m_pColonImg);
    }

	if (m_pMinusImg == 0)
	{
		m_pMinusImg = new CSwapImage(m_pEngineDevice);

		if (m_pMinusImg == 0)
		{
			return false;
		}

		m_pMinusImg->CreateSub(this, _T("STAGE_TIMER_MINUS_IMAGE"), UI_FLAG_DEFAULT);
		RegisterControl(m_pMinusImg);
	}

	return true;
}

CUIControl* CUINumberTimer::CreateControl(const TCHAR* pControl, CUIControl* pParentControl)
{
	CUIControl* pUIControl = new CUIControl(m_pEngineDevice);

	if (pUIControl)
	{
		pUIControl->CreateSub(pParentControl, pControl, UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);

		if (pParentControl)
		{
			const UIRECT& rcParentOldPos = pParentControl->GetLocalPos();
			pUIControl->AlignSubControl(rcParentOldPos, rcParentOldPos);
		}

		RegisterControl(pUIControl);
	}

	return pUIControl;
}

void CUINumberTimer::SetTime(int nTime)
{
	m_SavedTime = nTime;
    int nMin = nTime / 60;
    int nSec = nTime % 60;
    m_nUnitsDigitMin = nMin % 10;
    m_nTensDigitMin = (nMin / 10) % 10;
    m_nUnitsDigitSec = nSec % 10;
    m_nTensDigitSec = (nSec / 10) % 10;
}
int	CUINumberTimer::GetTime()
{
	return m_SavedTime;
}

void CUINumberTimer::OnCompleted()
{
	SetVisibleSingle(FALSE);
}

const UIRECT CUINumberTimer::GetPivot()
{
    UIRECT entire = GetGlobalPos();
    UIRECT offset;

    if (m_pClock)
    {
        UIRECT one = m_pClock->GetLocalPos();
        offset.sizeX += one.sizeX + DEF_OFFSET_CLOCK * 2;
    }

    if (m_pDigitImg)
    {
        UIRECT one = m_pDigitImg->GetLocalPos();
        offset.sizeX += (4 * one.sizeX);
    }

    if (m_pColonImg)
    {
        UIRECT one = m_pColonImg->GetLocalPos();
        offset.sizeX += one.sizeX;
    }

    entire.left += (entire.sizeX - offset.sizeX) * 0.5f;

    return entire;
}

HRESULT CUINumberTimer::Render(LPDIRECT3DDEVICEQ pd3dDevice)
{
    if( !IsVisible() ) return S_OK;

    HRESULT hr = S_OK;
    CUIControl::ChangeSizeWindow();
    hr = CUIControl::Render ( pd3dDevice );
    if( FAILED( hr ) ) return hr;

    UIRECT pivot = GetPivot();
    UIRECT rt;

    rt = RenderClock(D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
    pivot.left += rt.sizeX + DEF_OFFSET_CLOCK;

	if (IsBlank())
	{
		return S_OK;
	}

	bool bMinus = false;

	if (m_nTensDigitMin == 0 && m_nUnitsDigitMin == 0 &&
		m_nTensDigitSec == 0 && m_nUnitsDigitSec == 0)
	{
		bMinus = true;
	}

	if (bMinus)
	{
		rt = RenderMinus(D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
		pivot.left += rt.sizeX;
	}
	else
	{
		rt = RenderNum(m_nTensDigitMin, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
		pivot.left += rt.sizeX;
		rt = RenderNum(m_nUnitsDigitMin, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
		pivot.left += rt.sizeX;		
	}

    rt = RenderColon(D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
    pivot.left += rt.sizeX;

	if (bMinus)
	{
		rt = RenderMinus(D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
		pivot.left += rt.sizeX;
	}
	else
	{
		rt = RenderNum(m_nTensDigitSec, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
		pivot.left += rt.sizeX;
		rt = RenderNum(m_nUnitsDigitSec, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
		pivot.left += rt.sizeX;
	}

    return S_OK;
}

const UIRECT CUINumberTimer::RenderClock(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
	if(m_isTimerImage == false)
		return UIRECT(0, 0, 0, 0);

    if (m_pClock)
    {
        m_pClock->SetGlobalPos(vPos);
        m_pClock->Render(pd3dDevice);
        return m_pClock->GetGlobalPos();
    }

    return UIRECT(0, 0, 0, 0);
}

const UIRECT CUINumberTimer::RenderNum(int nNum, D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if ( nNum < 0 )
        return UIRECT(0, 0, 0, 0);

    if (m_pDigitImg)
    {   
        TCHAR temp[50] = {0};
        _stprintf_s(temp, 50, _T("STAGE_TIMER_NUMBER_IMAGE%d"), nNum);
        m_pDigitImg->SetImage(temp);
        m_pDigitImg->SetGlobalPos(vPos);
        m_pDigitImg->Render(pd3dDevice);
        return m_pDigitImg->GetGlobalPos();
    }

    return UIRECT(0, 0, 0, 0);
}

const UIRECT CUINumberTimer::RenderMinus(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
	if (m_pMinusImg)
	{
		const float OFFSET_X = 5; 
		vPos.x += OFFSET_X;
		m_pMinusImg->SetGlobalPos(vPos);
		m_pMinusImg->Render(pd3dDevice);
		UIRECT rt = m_pMinusImg->GetGlobalPos();
		rt.sizeX += OFFSET_X;
		return rt;
	}

	return UIRECT(0, 0, 0, 0);
}

const UIRECT CUINumberTimer::RenderColon(D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (m_pColonImg)
    {
        m_pColonImg->SetGlobalPos(vPos);
        m_pColonImg->Render(pd3dDevice);
        return m_pColonImg->GetGlobalPos();
    }

    return UIRECT(0, 0, 0, 0);
}

void CUINumberTimer::SetBlank(bool bBlank, DWORD dwDuringTick)
{
	if (m_bBlank != bBlank)
	{
		m_bBlank = bBlank;
		m_dwDuringTick = dwDuringTick;
		m_dwPreTick = GetTickCount();
	}
}

bool CUINumberTimer::IsBlank()
{
	if (m_bBlank)
	{
		DWORD dwCur = GetTickCount();
		DWORD dwBlankTime = m_dwPreTick + m_dwDuringTick;
		DWORD dwUnBlankTime = m_dwPreTick + m_dwDuringTick + m_dwDuringTick;

		if (dwCur < dwBlankTime)
		{
			return true;
		}
		else if (dwCur < dwUnBlankTime)
		{
			return false;
		}

		m_dwPreTick = dwCur;
	}

	return false;
}

void CUINumberTimer::SetTimerImage(bool isVisible)
{
	m_isTimerImage = isVisible;
}
