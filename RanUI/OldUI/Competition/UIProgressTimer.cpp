#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "UIProgressTimer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIProgressTimer::CUIProgressTimer(EngineDeviceMan* pEngineDevice, CUIControl* pParent) :
	CUIGroup(pEngineDevice)
,	m_pGauge(0)
,   m_pBlank(0)
,	m_pText(0)
,	m_nTimer(0)
,	m_fCur(0.f)
{
	SetParent(pParent);
}

CUIProgressTimer::~CUIProgressTimer()
{
}

bool CUIProgressTimer::Create(float x, float y)
{
    if (m_pBlank == 0)
    {
        m_pBlank = new CBasicLineBoxSmart(m_pEngineDevice);
    }

    if (m_pBlank == 0)
    {
        return false;
    }

    m_pBlank->CreateSub(this, _T("TIMER_BAR"), UI_FLAG_XSIZE | UI_FLAG_YSIZE);
    m_pBlank->CreateSubControl(_T("TIMER_BAR_BLANK_IMAGE"));    
    RegisterControl(m_pBlank);

	if (m_pGauge == 0)
	{
		m_pGauge = new CBasicLineBoxSmart(m_pEngineDevice);
	}

	if (m_pGauge == 0)
	{
		return false;
	}

	m_pGauge->CreateSub(this, _T("TIMER_BAR"), UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	m_pGauge->CreateSubControl(_T("TIMER_BAR_IMAGE"));
	RegisterControl(m_pGauge);

	SetLocalPos(D3DXVECTOR2(x, y));

	return true;
}

void CUIProgressTimer::Start(int nTimer)
{
	MIN_ASSERT(nTimer > 0 && _T("타이머는 0 이하는 안됩니다!"));
	m_nTimer = nTimer;
	m_fCur = static_cast<float>(nTimer);

    if (m_pGauge)
    {
        m_pGauge->ResetControl(GetGlobalPos(), "TIMER_BAR");
    }
}

void CUIProgressTimer::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	if (m_fCur > 0)
	{
		m_fCur -= fElapsedTime;

		if (m_fCur < 0)
		{
			m_fCur = 0;
			
            CUIControl* pParent = GetParent();

            if (pParent)
            {
                pParent->TranslateUIMessage(GetWndID(), PT_TIMEOVER);
            }
		}

		if (m_pGauge)
		{
            float fPercent = (m_fCur / (float)m_nTimer);

            UIRECT rcGaugePos = m_pGauge->GetGlobalPos();
            UIRECT rcBlankPos = m_pBlank->GetGlobalPos();

            UIRECT rcNewPos = rcBlankPos;
            rcNewPos.sizeX = floor(rcNewPos.sizeX * fPercent);
            rcNewPos.right = rcNewPos.left + rcNewPos.sizeX;

			m_pGauge->SetLocalPos( D3DXVECTOR2( 0.0f, 0.0f ) );
            m_pGauge->ReSizeControl(rcGaugePos, rcNewPos);

			D3DXVECTOR2 vPos;
			vPos.x = m_pBlank->GetLocalPos().left;
			vPos.y = m_pBlank->GetLocalPos().top;

			m_pGauge->SetLocalPos( vPos );
		}
	}

	if (m_pText)
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), int(m_fCur));
		m_pText->SetText(temp);
	}
}
