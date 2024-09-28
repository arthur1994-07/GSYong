#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "UIStageNumber.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIStageNumber::CUIStageNumber(EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,   m_pWhiteImg(0)
,   m_pBlackImg(0)
,   m_nUnitsDigit(0)
,   m_nTensDigit(0)
{
	
}

CUIStageNumber::~CUIStageNumber()
{
}

bool CUIStageNumber::CreateSubControl()
{
    if (m_pWhiteImg == 0)
    {
        m_pWhiteImg = new CSwapImage(m_pEngineDevice);

        if (m_pWhiteImg == 0)
        {
            return false;
        }

        m_pWhiteImg->CreateSub(this, _T("STAGE_INDICATOR_WHITE_NUMBER_IMAGE0"), UI_FLAG_DEFAULT);        
        RegisterControl(m_pWhiteImg);
    }

    if (m_pBlackImg == 0)
    {
        m_pBlackImg = new CSwapImage(m_pEngineDevice);

        if (m_pBlackImg == 0)
        {
            return false;
        }

        m_pBlackImg->CreateSub(this, _T("STAGE_INDICATOR_BLACK_NUMBER_IMAGE0"), UI_FLAG_DEFAULT);        
        RegisterControl(m_pBlackImg);
    }

	return true;
}

void CUIStageNumber::SetNumber(int nNum, bool bWhiteNBlack)
{
    m_bWhiteNBlack = bWhiteNBlack;
    m_nUnitsDigit = nNum % 10;
    m_nTensDigit = nNum / 10;
}

const UIRECT CUIStageNumber::GetPivot()
{
    UIRECT entire = GetGlobalPos();
    UIRECT offset;

    if (m_pWhiteImg)
    {
        UIRECT one = m_pWhiteImg->GetLocalPos();

        if (m_nTensDigit > 0)
        {
            offset.sizeX += (2 * one.sizeX);            
        }
        else
        {
            offset.sizeX += (one.sizeX);
        }

        offset.sizeY += one.sizeY;
    }

    entire.left += (entire.sizeX - offset.sizeX) * 0.5f;
    entire.top += (entire.sizeY - offset.sizeY) * 0.5f;

    return entire;
}

HRESULT CUIStageNumber::Render(LPDIRECT3DDEVICEQ pd3dDevice)
{
    if( !IsVisible() ) return S_OK;

    UIRECT pivot = GetPivot();
    UIRECT rt;

    if (m_nTensDigit > 0)
    {
        rt = RenderNum(m_nTensDigit, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
        pivot.left += rt.sizeX;        
    }

    rt = RenderNum(m_nUnitsDigit, D3DXVECTOR2(pivot.left, pivot.top), pd3dDevice);
    pivot.left += rt.sizeX;

    return S_OK;
}

const UIRECT CUIStageNumber::RenderNum(int nNum, D3DXVECTOR2 vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (m_bWhiteNBlack && m_pWhiteImg)
    {
        TCHAR temp[50] = {0};
        _stprintf_s(temp, 50, _T("STAGE_INDICATOR_WHITE_NUMBER_IMAGE%d"), nNum);
        m_pWhiteImg->SetImage(temp);
        m_pWhiteImg->SetGlobalPos(vPos);
        m_pWhiteImg->Render(pd3dDevice);
        return m_pWhiteImg->GetGlobalPos();
    }
    else if (m_pBlackImg)
    {
        TCHAR temp[50] = {0};
        _stprintf_s(temp, 50, _T("STAGE_INDICATOR_BLACK_NUMBER_IMAGE%d"), nNum);
        m_pBlackImg->SetImage(temp);
        m_pBlackImg->SetGlobalPos(vPos);
        m_pBlackImg->Render(pd3dDevice);
        return m_pBlackImg->GetGlobalPos();
    }

    return UIRECT(0, 0, 0, 0);
}
