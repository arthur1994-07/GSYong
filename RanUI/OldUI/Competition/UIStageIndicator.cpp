#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "UIStageIndicator.h"
#include "UIStageNumber.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define DEF_OFFSET_X        6
#define DEF_BG_OFFSET_FRONT 4
#define DEF_BG_OFFSET_BACK  4

CUIStageIndicator::CUIStageIndicator(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,	m_pBg(0)
,	m_pPreActive(0)
,	m_pActive(0)
,	m_pPostActive(0)
,	m_pClear(0)
,   m_pNubmer(0)
,	m_nStart(0)
,	m_nEnd(0)
,	m_nRange(0)
,	m_nCur(0)
{
	
}

CUIStageIndicator::~CUIStageIndicator()
{
}

bool CUIStageIndicator::CreateSubControl()
{
	if (m_pBg == 0)
	{
		m_pBg = CreateControl(_T("STAGE_INDICATOR_BG_IMAGE"));
		GASSERT(m_pBg != NULL);
	}

    if (m_pNubmer == 0)
    {
        m_pNubmer = new CUIStageNumber(m_pEngineDevice);
        GASSERT(m_pNubmer != NULL);

		m_pNubmer->CreateSub(this, _T("STAGE_INDICATOR_BG_IMAGE"));
        m_pNubmer->CreateSubControl();
        RegisterControl(m_pNubmer);
    }

	if (m_pClear == 0)
	{
		m_pClear = CreateControl(_T("STAGE_INDICATOR_X_IMAGE"));

		if (m_pClear == 0)
		{
			return false;
		}
	}

    if (m_pPreActive == 0)
    {
        m_pPreActive = CreateControl(_T("STAGE_INDICATOR_PREACTIVE_IMAGE"));

        if (m_pPreActive == 0)
        {
            return false;
        }
    }

    if (m_pActive == 0)
    {
        m_pActive = CreateControl(_T("STAGE_INDICATOR_ACTIVE_IMAGE"));

        if (m_pActive == 0)
        {
            return false;
        }
    }

    if (m_pPostActive == 0)
    {
        m_pPostActive = CreateControl(_T("STAGE_INDICATOR_POSTACTIVE_IMAGE"));

        if (m_pPostActive == 0)
        {
            return false;
        }
    }

	return true;
}

CUIControl* CUIStageIndicator::CreateControl(const TCHAR* pControl)
{
	CUIControl* pUIControl = new CUIControl(m_pEngineDevice);

	if (pUIControl == 0)
	{
		return pUIControl;
	}

	pUIControl->CreateSub(this, pControl, UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
	RegisterControl(pUIControl);

	return pUIControl;
}

const UIRECT CUIStageIndicator::GetPivot()
{
	UIRECT entire = GetGlobalPos();
	UIRECT one = m_pActive->GetLocalPos();
    float fSizeX = (m_nRange + 1) * (one.sizeX + DEF_OFFSET_X);
    entire.left += (entire.sizeX - fSizeX) * 0.5f;
    entire.sizeX = fSizeX;
	return entire;
}

HRESULT CUIStageIndicator::Render(LPDIRECT3DDEVICEQ pd3dDevice)
{
	if( !IsVisible() ) return S_OK;

	HRESULT hr = S_OK;
	CUIControl::ChangeSizeWindow();
	hr = CUIControl::Render ( pd3dDevice );
	if( FAILED( hr ) ) return hr;

	// 현재 스테이지 예외 처리
	if (m_nStart > m_nCur && m_nEnd < m_nCur)
	{
		return hr;
	}

	// 범위 예외 처리
	if (m_nEnd - m_nStart < m_nRange)
	{
		m_nRange = m_nEnd - m_nStart + 1;
	}

	// 피벗 추출
	UIRECT pivot = GetPivot();

	// 배경 렌더링
 	RenderBG(pivot, pd3dDevice);

	// 렌더링 할 스테이지 범위 추출
	int nStart = m_nStart;
	int nEnd = nStart + m_nRange - 1;
 
 	// 클리어 된 스테이지 렌더링
	if (nStart < m_nCur)
	{
		for (int i = nStart; i < m_nCur; ++i)
		{
			const UIRECT rt = RenderPreStage(i, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice, true);
			pivot.left += rt.sizeX + DEF_OFFSET_X;
		}
	}

	// 현재 스테이지 렌더링
	const UIRECT rt = RenderCurStage(m_nCur, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
	pivot.left += rt.sizeX + DEF_OFFSET_X;

	// 미 클리어 스테이지 렌더링
	if (nEnd > m_nCur)
	{
		for (int i = m_nCur + 1; i <= nEnd; ++i)
		{
			const UIRECT rt = RenderPreStage(i, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice, false);
			pivot.left += rt.sizeX + DEF_OFFSET_X;
		}
	}

	// 라스트 스테이지 렌더링
	RenderPostStage(m_nEnd, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);

	return S_OK;
}

const UIRECT CUIStageIndicator::RenderBG(const UIRECT sRt, LPDIRECT3DDEVICEQ pd3dDevice)
{
	if (m_pBg)
	{
        float fSizeY = m_pBg->GetLocalPos().sizeY;
        float fCenterY = sRt.top + (sRt.sizeY - fSizeY) * 0.5f;
		m_pBg->SetGlobalPos(UIRECT(sRt.left + DEF_BG_OFFSET_FRONT, fCenterY, sRt.sizeX - DEF_BG_OFFSET_FRONT - DEF_BG_OFFSET_BACK, fSizeY));
		m_pBg->Render(pd3dDevice);
		return m_pBg->GetGlobalPos();
	}

	return UIRECT(0, 0, 0, 0);
}

const UIRECT CUIStageIndicator::RenderPreStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice, bool bClear)
{
	if (m_pPreActive)
	{
		m_pPreActive->SetGlobalPos(vPos);
		m_pPreActive->Render(pd3dDevice);

        RenderNumber(nNum, true, UIRECT(vPos.x, vPos.y, m_pPreActive->GetLocalPos().sizeX, m_pPreActive->GetLocalPos().sizeY), pd3dDevice);

        if (bClear)
        {
		    RenderClear(vPos, pd3dDevice);
        }

		return m_pPreActive->GetGlobalPos();
	}

	return UIRECT(0, 0, 0, 0);
}

const UIRECT CUIStageIndicator::RenderCurStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
	if (m_pActive)
	{
		m_pActive->SetGlobalPos(vPos);
		m_pActive->Render(pd3dDevice);

        RenderNumber(nNum, false, UIRECT(vPos.x, vPos.y, m_pActive->GetLocalPos().sizeX, m_pActive->GetLocalPos().sizeY), pd3dDevice);

		return m_pActive->GetGlobalPos();
	}

	return UIRECT(0, 0, 0, 0);
}

const UIRECT CUIStageIndicator::RenderPostStage(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (m_pPostActive)
    {
        m_pPostActive->SetGlobalPos(vPos);
        m_pPostActive->Render(pd3dDevice);

        RenderNumber(nNum, true, UIRECT(vPos.x, vPos.y, m_pPostActive->GetLocalPos().sizeX, m_pPostActive->GetLocalPos().sizeY), pd3dDevice);

        return m_pPostActive->GetGlobalPos();
    }

    return UIRECT(0, 0, 0, 0);
}

const UIRECT CUIStageIndicator::RenderClear(const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
	if (m_pClear)
	{
		m_pClear->SetGlobalPos(vPos);
		m_pClear->Render(pd3dDevice);
		return m_pClear->GetGlobalPos();
	}

	return UIRECT(0, 0, 0, 0);
}

void CUIStageIndicator::RenderNumber(const int nNum, bool bWhiteNBlack, const UIRECT sRt, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (m_pNubmer)
    {
        m_pNubmer->SetNumber(nNum, bWhiteNBlack);
        m_pNubmer->SetGlobalPos(sRt);
        m_pNubmer->Render(pd3dDevice);
    }
}
