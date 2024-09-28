#include "StdAfx.h"
#include "./AuthenticatorCTFDisplay.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CAuthenticatorCTFDisplay::s_fDefaultBlinkTime = 0.3f;

CAuthenticatorCTFDisplay::CAuthenticatorCTFDisplay(EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_emAuthSchool(SCHOOL_NONE)
, m_pImageAuth(NULL)
{
    ZeroMemory( m_pImage, sizeof(m_pImage) );
    ZeroMemory( m_pImage_Over, sizeof(m_pImage_Over) );
    ZeroMemory( m_pImage_Over_Base, sizeof(m_pImage_Over_Base) );
    ZeroMemory( m_pImage_Back, sizeof(m_pImage_Back) );
    ZeroMemory( m_pImage_FightSign, sizeof(m_pImage_FightSign) );
    ZeroMemory( m_pImage_Gage, sizeof(m_pImage_Gage) );
    ZeroMemory( m_fPercent, sizeof(m_fPercent) );
    ZeroMemory( m_fPercentBlinkTime, sizeof(m_fPercentBlinkTime) );
}

CAuthenticatorCTFDisplay::~CAuthenticatorCTFDisplay ()
{
    for ( int i=0; i<SCHOOL_NUM; ++i )
    {
        SAFE_DELETE( m_pImage_Over_Base[i] );
    }
}

void CAuthenticatorCTFDisplay::CreateSubControl ()
{
    {
        m_pImage_Back[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
        m_pImage_Back[SCHOOL_SM]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_SM_BACK" );
        RegisterControl(m_pImage_Back[SCHOOL_SM]);

        m_pImage_Back[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
        m_pImage_Back[SCHOOL_HA]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_HA_BACK" );
        RegisterControl(m_pImage_Back[SCHOOL_HA]);

        m_pImage_Back[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
        m_pImage_Back[SCHOOL_BH]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_BH_BACK" );
        RegisterControl(m_pImage_Back[SCHOOL_BH]);
    }

    {
        m_pImage[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
        m_pImage[SCHOOL_SM]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_SM" );
        RegisterControl(m_pImage[SCHOOL_SM]);

        m_pImage[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
        m_pImage[SCHOOL_HA]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_HA" );
        RegisterControl(m_pImage[SCHOOL_HA]);

        m_pImage[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
        m_pImage[SCHOOL_BH]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_BH" );
        RegisterControl(m_pImage[SCHOOL_BH]);

    }

    {
        m_pImage_Over[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
        m_pImage_Over[SCHOOL_SM]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_SM_OVER" );
        RegisterControl(m_pImage_Over[SCHOOL_SM]);

        m_pImage_Over[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
        m_pImage_Over[SCHOOL_HA]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_HA_OVER" );
        RegisterControl(m_pImage_Over[SCHOOL_HA]);

        m_pImage_Over[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
        m_pImage_Over[SCHOOL_BH]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_BH_OVER" );
        RegisterControl(m_pImage_Over[SCHOOL_BH]);

        //! 이미지의 Base, 메모리는 객체가 제거될때 삭제한다
        {
            m_pImage_Over_Base[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
            m_pImage_Over_Base[SCHOOL_SM]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_SM_OVER" );

            m_pImage_Over_Base[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
            m_pImage_Over_Base[SCHOOL_HA]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_HA_OVER" );

            m_pImage_Over_Base[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
            m_pImage_Over_Base[SCHOOL_BH]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_BH_OVER" );
        }
    }

    {
        m_pImage_FightSign[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
        m_pImage_FightSign[SCHOOL_SM]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_SM_FIGHTSIGN" );
        RegisterControl(m_pImage_FightSign[SCHOOL_SM]);

        m_pImage_FightSign[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
        m_pImage_FightSign[SCHOOL_HA]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_HA_FIGHTSIGN" );
        RegisterControl(m_pImage_FightSign[SCHOOL_HA]);

        m_pImage_FightSign[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
        m_pImage_FightSign[SCHOOL_BH]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_BH_FIGHTSIGN" );
        RegisterControl(m_pImage_FightSign[SCHOOL_BH]);

        m_pImage_Gage[SCHOOL_SM] = new CUIControl(m_pEngineDevice);
        m_pImage_Gage[SCHOOL_SM]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_GAGE" );
        RegisterControl(m_pImage_Gage[SCHOOL_SM]);

        m_pImage_Gage[SCHOOL_HA] = new CUIControl(m_pEngineDevice);
        m_pImage_Gage[SCHOOL_HA]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_GAGE" );
        RegisterControl(m_pImage_Gage[SCHOOL_HA]);

        m_pImage_Gage[SCHOOL_BH] = new CUIControl(m_pEngineDevice);
        m_pImage_Gage[SCHOOL_BH]->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_GAGE" );
        RegisterControl(m_pImage_Gage[SCHOOL_BH]);
    }

    m_pImageAuth = new CUIControl(m_pEngineDevice);
    m_pImageAuth->CreateSub( this, "AUTHENTICATOR_CTF_DISPLAY_IMAGE_AUTH" );
    RegisterControl(m_pImageAuth);
}

void CAuthenticatorCTFDisplay::Update ()
{
    m_pImageAuth->SetVisibleSingle( FALSE );

    for ( int i=0; i<SCHOOL_NUM; ++i )
    {
        m_pImage_FightSign[i]->SetVisibleSingle( FALSE );
        m_pImage_Gage[i]->SetVisibleSingle( FALSE );
    }

    switch ( m_emAuthSchool )
    {
    case SCHOOL_NONE:
        {
            m_pImageAuth->SetVisibleSingle( FALSE );
        }
        break;

    case SCHOOL_SM:
    case SCHOOL_HA:
    case SCHOOL_BH:
        {
            m_pImageAuth->SetVisibleSingle( TRUE );
            m_pImageAuth->SetGlobalPos(  m_pImage_FightSign[m_emAuthSchool]->GetGlobalPos() );
            m_pImageAuth->SetLocalPos( m_pImage_FightSign[m_emAuthSchool]->GetLocalPos() );
        }
        break;

    };

    for ( int i=0; i<SCHOOL_NUM; ++i )
    {
        if ( m_fPercentBlinkTime[i] <= 0.0f )
        {
            m_fPercentBlinkTime[i] = 0.0f;
            continue;
        }
        else
        {
            m_fPercentBlinkTime[i] = 0.0f;
        }

        m_pImage_FightSign[i]->SetVisibleSingle( TRUE );
    }

    SetImageCulling ( m_pImage_Over_Base[SCHOOL_SM], m_pImage_Over[SCHOOL_SM], m_fPercent[SCHOOL_SM] );
    SetImageCulling ( m_pImage_Over_Base[SCHOOL_HA], m_pImage_Over[SCHOOL_HA], m_fPercent[SCHOOL_HA] );
    SetImageCulling ( m_pImage_Over_Base[SCHOOL_BH], m_pImage_Over[SCHOOL_BH], m_fPercent[SCHOOL_BH] );

    //for ( int i=0; i<SCHOOL_NUM; ++i )
    //{
    //    if ( i == m_emAuthSchool )
    //    {
    //        continue;
    //    }

    //    UIRECT rcLocal  = m_pImage_Over[i]->GetLocalPos();
    //    UIRECT rcGlobal = m_pImage_Over[i]->GetGlobalPos();

    //    UIRECT rcGageLocal  = m_pImage_Gage[i]->GetLocalPos();
    //    UIRECT rcGageGlobal = m_pImage_Gage[i]->GetGlobalPos();

    //    m_pImage_Gage[i]->SetVisibleSingle( TRUE );
    //    m_pImage_Gage[i]->SetLocalPos( D3DXVECTOR2(rcLocal.left, rcLocal.top + rcLocal.sizeY - rcGageLocal.sizeY) );
    //    m_pImage_Gage[i]->SetGlobalPos( D3DXVECTOR2(rcGlobal.left, rcGlobal.top + rcGlobal.sizeY - rcGageLocal.sizeY) );
    //}
}

void CAuthenticatorCTFDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible () ) return ;

    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAuthenticatorCTFDisplay::SetImageCulling ( CUIControl* pBase, CUIControl* pImage, float fPercent )
{
	if ( fPercent < 0.0f )
	{
		fPercent = 0.0f;
	}

    UIRECT rcBaseLocal  = pBase->GetLocalPos();
    UIRECT rcBaseGlobal = pBase->GetGlobalPos();
    UIRECT rcTexturePos = pBase->GetTexturePos();

    UIRECT rcLocal  = pImage->GetLocalPos();
    UIRECT rcGlobal = pImage->GetGlobalPos();

    pImage->SetGlobalPos( UIRECT(rcGlobal.left, rcGlobal.top, rcBaseGlobal.sizeX, rcBaseGlobal.sizeY*fPercent) );
    pImage->SetLocalPos( UIRECT(rcLocal.left, rcLocal.top, rcBaseLocal.sizeX, rcBaseLocal.sizeY*fPercent) );
    pImage->SetTexturePos( UIRECT(rcTexturePos.left, rcTexturePos.top, rcTexturePos.sizeX, rcTexturePos.sizeY*fPercent) );
}

void CAuthenticatorCTFDisplay::SetPercentSM ( float fPercent ) 
{ 
    if ( m_fPercent[SCHOOL_SM] > fPercent )
    {
        m_fPercentBlinkTime[SCHOOL_SM] = s_fDefaultBlinkTime;
    }

    m_fPercent[SCHOOL_SM] = fPercent; 

}

void CAuthenticatorCTFDisplay::SetPercentHA ( float fPercent )
{ 
    if ( m_fPercent[SCHOOL_HA] > fPercent )
    {
        m_fPercentBlinkTime[SCHOOL_HA] = s_fDefaultBlinkTime;
    }

    m_fPercent[SCHOOL_HA] = fPercent; 
}

void CAuthenticatorCTFDisplay::SetPercentBH ( float fPercent )
{
    if ( m_fPercent[SCHOOL_BH] > fPercent )
    {
        m_fPercentBlinkTime[SCHOOL_BH] = s_fDefaultBlinkTime;
    }

    m_fPercent[SCHOOL_BH] = fPercent; 
}