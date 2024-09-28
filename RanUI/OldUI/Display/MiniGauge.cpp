#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"

#include "../../InnerInterface.h"
#include "MiniGauge.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMiniGauge::CMiniGauge(GLGaeaClient* pGaeaClient, EngineDeviceMan* pRenderDevice)
    : CUIGroup(pRenderDevice)
	, m_pGaeaClient ( pGaeaClient )
    , m_pBack(NULL)
    , m_pRed(NULL)
    , m_pYellow(NULL)
    , m_pGreen(NULL)
    , m_CurrentValue(0.0f)
    , m_fTIME(1.0f)
    , m_nType(0)
    , m_bAutoUpdate(false)
{
}

CMiniGauge::~CMiniGauge(void)
{
}

void CMiniGauge::CreateSubControl ()
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "MINIBAR_GAUGE_BACK" );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pBack = pControl;

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "MINIBAR_GAUGE_RED" );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pRed = pControl;

	m_pRed->SetFlowBar( m_pRed->GetGlobalPos(), m_pRed->GetTexturePos() );
	m_pRed->SetFlowBarType( CUIControl::FLOW_BAR_UP );

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "MINIBAR_GAUGE_GREEN" );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pGreen = pControl;

	m_pGreen->SetFlowBar( m_pGreen->GetGlobalPos(), m_pGreen->GetTexturePos() );
	m_pGreen->SetFlowBarType( CUIControl::FLOW_BAR_UP );

	pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, "MINIBAR_GAUGE_YELLOW" );
	pControl->SetVisibleSingle ( FALSE );
	RegisterControl ( pControl );
	m_pYellow = pControl;

	m_pYellow->SetFlowBar( m_pYellow->GetGlobalPos(), m_pYellow->GetTexturePos() );
	m_pYellow->SetFlowBarType( CUIControl::FLOW_BAR_UP );
}

void CMiniGauge::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () )
        return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( !m_bAutoUpdate )
        return;

	m_CurrentValue -= fElapsedTime;

	if ( m_CurrentValue <= 0.0f )
		m_CurrentValue = 0.0f;

	if ( m_fTIME != 0.0f )
	{
        GaugeUpdate( m_CurrentValue );
	}
}


void CMiniGauge::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MINI_GAUGE_BACK:
	case MINI_GAUGE_BACK+1:
	case MINI_GAUGE_BACK+2:
	case MINI_GAUGE_BACK+3:
		{
		}
		break;
	}
}

void CMiniGauge::SetRedBar( float MaxPercent, bool AutoUpdate )
{
	m_pBack->SetVisibleSingle( TRUE );

	m_pRed->SetVisibleSingle( TRUE );
	m_pYellow->SetVisibleSingle( FALSE );
	m_pGreen->SetVisibleSingle( FALSE );

	m_nType = MINI_GAUGE_RED;
	m_fTIME = MaxPercent;
	m_CurrentValue = m_fTIME;
    m_bAutoUpdate = AutoUpdate;

	m_pRed->SetFlowBar( m_pRed->GetGlobalPos(), m_pRed->GetTexturePos() );
	m_pRed->FlowBarUpdate( m_CurrentValue / m_fTIME );
}

void CMiniGauge::SetYellowBar( float MaxPercent, bool AutoUpdate )
{
	m_pBack->SetVisibleSingle( TRUE );

	m_pRed->SetVisibleSingle( FALSE );
	m_pYellow->SetVisibleSingle( TRUE );
	m_pGreen->SetVisibleSingle( FALSE );

	m_nType = MINI_GAUGE_YELLOW;
	m_fTIME = MaxPercent;
	m_CurrentValue = m_fTIME;
    m_bAutoUpdate = AutoUpdate;

	m_pYellow->SetFlowBar( m_pYellow->GetGlobalPos(), m_pYellow->GetTexturePos() );
	m_pYellow->FlowBarUpdate( m_CurrentValue / m_fTIME );
}

void CMiniGauge::SetGreenBar( float MaxPercent, bool AutoUpdate )
{
	m_pBack->SetVisibleSingle( TRUE );

	m_pRed->SetVisibleSingle( FALSE );
	m_pYellow->SetVisibleSingle( FALSE );
	m_pGreen->SetVisibleSingle( TRUE );

	m_nType = MINI_GAUGE_GREEN;
	m_fTIME = MaxPercent;
	m_CurrentValue = m_fTIME;
    m_bAutoUpdate = AutoUpdate;

	m_pGreen->SetFlowBar( m_pGreen->GetGlobalPos(), m_pGreen->GetTexturePos() );
	m_pGreen->FlowBarUpdate( m_CurrentValue / m_fTIME );
}

void CMiniGauge::SetAutoBar( float MaxPercent, bool AutoUpdate )
{
	m_pBack->SetVisibleSingle( TRUE );

	m_pRed->SetVisibleSingle( FALSE );
	m_pYellow->SetVisibleSingle( FALSE );
	m_pGreen->SetVisibleSingle( TRUE );

	m_nType = MINI_GAUGE_AUTO;
	m_fTIME = MaxPercent;
	m_CurrentValue = m_fTIME;
    m_bAutoUpdate = AutoUpdate;

	m_pRed->SetFlowBar( m_pRed->GetGlobalPos(), m_pRed->GetTexturePos() );
	m_pYellow->SetFlowBar( m_pYellow->GetGlobalPos(), m_pYellow->GetTexturePos() );
	m_pGreen->SetFlowBar( m_pGreen->GetGlobalPos(), m_pGreen->GetTexturePos() );
	m_pGreen->FlowBarUpdate( m_CurrentValue / m_fTIME );
}

void CMiniGauge::GaugeUpdate( float CurrentValue )
{
    if ( CurrentValue > m_fTIME )
        CurrentValue = m_fTIME;
    else if ( CurrentValue < 0.0f )
        CurrentValue = 0.0f;

    m_CurrentValue = CurrentValue;
    float Percent = m_CurrentValue / m_fTIME;

    switch ( m_nType )
    {
    case MINI_GAUGE_RED:
        m_pRed->FlowBarUpdate( Percent );
        break;

    case MINI_GAUGE_GREEN:
        m_pGreen->FlowBarUpdate( Percent );
        break;

    case MINI_GAUGE_YELLOW:
        m_pYellow->FlowBarUpdate( Percent );
        break;

    case MINI_GAUGE_AUTO:
        {
            if ( Percent > 0.5f )
            {
                m_pGreen->FlowBarUpdate( Percent );

                m_pGreen->SetVisibleSingle( TRUE );
                m_pYellow->SetVisibleSingle( FALSE );
                m_pRed->SetVisibleSingle( FALSE );
            }
            else if ( Percent > 0.1f )
            {
                m_pYellow->FlowBarUpdate( Percent );

                m_pGreen->SetVisibleSingle( FALSE );
                m_pYellow->SetVisibleSingle( TRUE );
                m_pRed->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pRed->FlowBarUpdate( Percent );

                m_pGreen->SetVisibleSingle( FALSE );
                m_pYellow->SetVisibleSingle( FALSE );
                m_pRed->SetVisibleSingle( TRUE );
            }
        }
        break;
    };
}
