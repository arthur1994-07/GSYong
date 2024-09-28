#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "../../StaticUIManager.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"

#include "./NotifyCTFSpurt.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

const float START_TIME = 5.0f;
const float END_TIME = 15.0f;
const float BLINK_VIEW_TIME = 2.0f;
const float BLINK_UNVIEW_TIME = 0.5f;

CNotifyCTFSpurt::CNotifyCTFSpurt( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pNotice(NULL)
, m_pWinSM(NULL)
, m_pWinHA(NULL)
, m_pWinBH(NULL)
, m_bEnableVisible(false)
, m_bVisible(false)
, m_bBlinkView(false)
{
}

void CNotifyCTFSpurt::CreateSubControl()
{
    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "NOTIFY_CTF_SPURT_NOTICE" );
        RegisterControl( pControl );
        pControl->SetVisibleSingle( FALSE );
        m_pNotice = pControl;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "NOTIFY_CTF_SPURT_WIN_SM" );
        RegisterControl( pControl );
        pControl->SetVisibleSingle( FALSE );
        m_pWinSM = pControl;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "NOTIFY_CTF_SPURT_WIN_HA" );
        RegisterControl( pControl );
        pControl->SetVisibleSingle( FALSE );
        m_pWinHA = pControl;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "NOTIFY_CTF_SPURT_WIN_BH" );
        RegisterControl( pControl );
        pControl->SetVisibleSingle( FALSE );
        m_pWinBH = pControl;
    }
}

void CNotifyCTFSpurt::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( m_bEnableVisible )
    {
        m_fStartTime -= fElapsedTime;

        if ( m_fStartTime <= 0.0f )
        {
            m_bEnableVisible = false;

            VisibleSpurt( m_emSchool );
        }
    }

    if ( m_bVisible )
    {
        m_fEndTime -= fElapsedTime;
        m_fBlinkTime -= fElapsedTime;

        if ( m_fBlinkTime <= 0.0f )
        {
            if ( m_bBlinkView )
            {
                m_bBlinkView = false;
                m_fBlinkTime = BLINK_UNVIEW_TIME;

                UnShowNotify();
            }
            else
            {
                m_bBlinkView = true;
                m_fBlinkTime = BLINK_VIEW_TIME;

                VisibleSpurt( m_emSchool );
            }
        }

        if ( m_fEndTime <= 0.0f )
        {
            m_bVisible = false;

            UnShowNotify();
        }
    }

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CNotifyCTFSpurt::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CNotifyCTFSpurt::SetVisibleSingle(BOOL visible)
{
    if ( visible == false )
    {
        m_bEnableVisible = false;
        m_bVisible = false;
        m_bBlinkView = false;
        UnShowNotify();
    }
    CUIGroup::SetVisibleSingle(visible);
}

void CNotifyCTFSpurt::CheckCTFSpurt()
{
    UnShowNotify();

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pGaeaClient->GetCaptureTheField()->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    int nAuthenticatorCount[ SCHOOL_NUM ];
    for ( int i = 0; i < SCHOOL_NUM; i++ )
        nAuthenticatorCount[ i ] = 0;

    GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO_ITER itr = sCTFState.vecAUTHENTICATOR.begin();
    GLCaptureTheFieldClient::VEC_AUTHENTICATORINFO_ITER itr_end = sCTFState.vecAUTHENTICATOR.end();

    for ( ; itr != itr_end; ++itr )
    {
        SAUTHENTICATORINFO& sInfo = *itr;
        ENUM_SCHOOL emSchool = sInfo.emAuthSchool;
        if ( emSchool < SCHOOL_NUM && emSchool != SCHOOL_NONE )
            nAuthenticatorCount[ emSchool ]++;
    }

    int nAuthenticatorMax = static_cast<int>( sCTFState.vecAUTHENTICATOR.size() );

	bool bReset = false;

    for ( int i = 0; i < SCHOOL_NUM; i++ )
    {
        if ( nAuthenticatorCount[ i ] == nAuthenticatorMax - 1 )
		{
            SetSpurtSchool( static_cast<ENUM_SCHOOL>( i ) );

			bReset = true;
		}
    }

	if( !bReset )
	{
		m_bEnableVisible = false;
		m_bVisible = false;
		m_bBlinkView = false;
		UnShowNotify();
	}
}

void CNotifyCTFSpurt::SetSpurtSchool( ENUM_SCHOOL emSchool )
{
    m_emSchool = emSchool;

    m_bEnableVisible = true;
    m_bVisible = false;
    m_bBlinkView = true;

    m_fStartTime = START_TIME;
    m_fEndTime = END_TIME;
    m_fBlinkTime = BLINK_VIEW_TIME;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD( lResolution );
    WORD Y_RES = LOWORD( lResolution );

    {
        const UIRECT& rcWindow = GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = ( X_RES - GetGlobalPos().sizeX ) * 0.5f;
        vPos.y = Y_RES * 0.2f;
        SetGlobalPos( vPos );
    }
}

void CNotifyCTFSpurt::VisibleSpurt( ENUM_SCHOOL emSchool )
{
    m_bVisible = true;

    WORD wCharSchool = m_pGaeaClient->GetCharacter()->m_wSchool;

    if ( wCharSchool == static_cast<WORD>( emSchool ) )
    {
        m_pNotice->SetVisibleSingle( TRUE );
    }
    else
    {
        switch ( emSchool )
        {
        case SCHOOL_SM:
            {
                m_pWinSM->SetVisibleSingle( TRUE );
            }
            break;

        case SCHOOL_HA:
            {
                m_pWinHA->SetVisibleSingle( TRUE );
            }
            break;

        case SCHOOL_BH:
            {
                m_pWinBH->SetVisibleSingle( TRUE );
            }
            break;
        };
    }
}

void CNotifyCTFSpurt::UnShowNotify()
{
    if( m_pNotice ) m_pNotice->SetVisibleSingle( FALSE );
    if( m_pWinSM ) m_pWinSM->SetVisibleSingle( FALSE );
    if( m_pWinHA ) m_pWinHA->SetVisibleSingle( FALSE );
    if( m_pWinBH ) m_pWinBH->SetVisibleSingle( FALSE );
}