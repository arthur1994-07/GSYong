#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "UIOrdinal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIOrdinal::CUIOrdinal(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,   m_pDigitImg(0)
,   m_pOrdinalImg(0)
,   m_nUnitsDigit(0)
,   m_nTensDigit(0)
,   m_nOrdinal(ORDI_NONE)
{
	
}

CUIOrdinal::~CUIOrdinal()
{
}

bool CUIOrdinal::CreateSubControl()
{
    if (m_pDigitImg == 0)
    {
        m_pDigitImg = new CSwapImage(m_pEngineDevice);

        if (m_pDigitImg == 0)
        {
            return false;
        }

        m_pDigitImg->CreateSub(this, _T("ORDINAL_NUMBER_IMAGE0"), UI_FLAG_DEFAULT);
        RegisterControl(m_pDigitImg);
    }

    if (m_pOrdinalImg == 0)
    {
        m_pOrdinalImg =	new CSwapImage(m_pEngineDevice);

        if (m_pOrdinalImg == 0)
        {
            return false;
        }

        m_pOrdinalImg->CreateSub(this, _T("ORDINAL_IMAGE1"), UI_FLAG_DEFAULT);        
        RegisterControl(m_pOrdinalImg);
    }

	return true;
}

void CUIOrdinal::SetOrdinal(const int nNum, bool bShow)
{
    // th, st, nd, rd 순서
    int table[10] = { ORDI_TH, ORDI_ST, ORDI_ND, ORDI_RD, ORDI_TH, 
        ORDI_TH, ORDI_TH, ORDI_TH, ORDI_TH, ORDI_TH };
    int ordinal = (nNum % 10);

    // 예외 처리
    if (nNum == 11 || nNum == 12 || nNum == 13)
    {
        ordinal = 0;
    }

    m_nOrdinal = table[ordinal];

    // 자리수 추출
    m_nUnitsDigit = nNum % 10;
    m_nTensDigit = (nNum / 10) % 10;

    if (m_pDigitImg)
    {
        m_pDigitImg->SetVisibleSingle(bShow ? TRUE : FALSE);
    }

    if (m_pOrdinalImg)
    {
        m_pOrdinalImg->SetVisibleSingle(bShow ? TRUE : FALSE);
    }
}

const UIRECT CUIOrdinal::GetPivot()
{
    UIRECT entire = GetGlobalPos();
    UIRECT offset;

    if (m_pDigitImg)
    {
        UIRECT one = m_pDigitImg->GetLocalPos();

        int num = 1;

        if (m_nTensDigit > 0)
        {
            num = 2;
        }

        offset.sizeX += (num * one.sizeX);
    }

    if (m_pOrdinalImg)
    {
        UIRECT one = m_pOrdinalImg->GetLocalPos();
        offset.sizeX += one.sizeX;
    }

    entire.left += (entire.sizeX - offset.sizeX) * 0.5f;

    return entire;
}

HRESULT CUIOrdinal::Render(LPDIRECT3DDEVICEQ pd3dDevice)
{
    if( !IsVisible() ) return S_OK;

    HRESULT hr = S_OK;
    CUIControl::ChangeSizeWindow();
    hr = CUIControl::Render ( pd3dDevice );
    if( FAILED( hr ) ) return hr;

    UIRECT pivot = GetPivot();

    if (m_nTensDigit > 0)
    {
        const UIRECT rt = RenderNum(m_nTensDigit, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
        pivot.left += rt.sizeX;
    }

    const UIRECT rt = RenderNum(m_nUnitsDigit, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
    pivot.left += rt.sizeX;

    RenderOridinal(D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);

    return S_OK;
}

const UIRECT CUIOrdinal::RenderNum(const int nNum, const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (m_pDigitImg && m_pDigitImg->IsVisible())
    {
        TCHAR temp[50] = {0};
        _stprintf_s(temp, 50, _T("ORDINAL_NUMBER_IMAGE%d"), nNum);
        m_pDigitImg->SetImage(temp);        
        m_pDigitImg->SetGlobalPos(vPos);
        m_pDigitImg->Render(pd3dDevice);
        return m_pDigitImg->GetGlobalPos();
    }
 
    return UIRECT(0, 0, 0, 0);
}

const UIRECT CUIOrdinal::RenderOridinal(const D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (m_pOrdinalImg && m_nOrdinal != ORDI_NONE && m_pOrdinalImg->IsVisible())
    {
        TCHAR temp[50] = {0};
        _stprintf_s(temp, 50, _T("ORDINAL_IMAGE%d"), m_nOrdinal);
        m_pOrdinalImg->SetImage(temp);
        m_pOrdinalImg->SetGlobalPos(vPos);
        m_pOrdinalImg->Render(pd3dDevice);
        return m_pOrdinalImg->GetGlobalPos();
    }

    return UIRECT(0, 0, 0, 0);
}
