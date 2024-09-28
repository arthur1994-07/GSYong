#include "StdAfx.h"
#include "./RnProgressBar.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

RnProgressBar::RnProgressBar( EngineDeviceMan* pEngineDevice )
	: CUIGroup( pEngineDevice )

	, m_eType( HORIZONTAL )

	, m_nNow( 0 )
	, m_nMax( 100 )
	, m_fRate( 0.0f )

	, m_pDefault( NULL )
    , m_pOverPos( NULL )
	, m_pOver( NULL )
{
}

RnProgressBar::~RnProgressBar()
{
}

void RnProgressBar::CreateSubControl( const CreateArg& arg )
{
    GASSERT( !arg.controlName.empty() );
    GASSERT( !arg.backgroundTextureName.empty() );
    GASSERT( !arg.overTextureName.empty() );

    // 배경
    CBasicLineBoxSmart* m_pDefault = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pDefault->CreateSub( this, arg.controlName.c_str() );
    m_pDefault->CreateSubControl( arg.backgroundTextureName.c_str() );
    RegisterControl( m_pDefault );

    UIRECT rcBorder = m_pDefault->GetBorder();
    UIRECT rcDefaultLocal = m_pDefault->GetLocalPos();
    {
        rcDefaultLocal.left     += rcBorder.left;
        rcDefaultLocal.top      += rcBorder.top;
        rcDefaultLocal.right    -= rcBorder.sizeX;
        rcDefaultLocal.bottom   -= rcBorder.sizeY;
        rcDefaultLocal.sizeX    -= rcBorder.right;
        rcDefaultLocal.sizeY    -= rcBorder.bottom;

    } 

    // 오버
    m_pOver = new CBasicLineBoxSmart( m_pEngineDevice );
    m_pOver->CreateSub( this, arg.controlName.c_str() );
    m_pOver->CreateSubControl( arg.overTextureName.c_str() );
    RegisterControl( m_pOver );

    // 위치 저장
    m_pOverPos = new CUIControl( m_pEngineDevice );
    m_pOverPos->CreateSub( this, arg.controlName.c_str() );
    m_pOverPos->SetLocalPos( rcDefaultLocal );
    RegisterControl( m_pOverPos );


}

void RnProgressBar::CreateSubControl( std::string controlName )
{
    RnProgressBar::CreateArg arg;

    arg.controlName = controlName;
    arg.backgroundTextureName = "PRODUCT_PROGRESS_BG_TEXTURE";
    arg.overTextureName = "PRODUCT_PROGRESS_OVER_TEXTURE";

    this->CreateSubControl( arg );
}

void RnProgressBar::SetAlignFlag( WORD wFlag )
{
    CUIGroup::SetAlignFlag( wFlag );

    m_pDefault->SetAlignFlag( wFlag );
    m_pOver->SetAlignFlag( wFlag );
}

void RnProgressBar::SetGlobalPos( const UIRECT& rcPos )
{
    CUIGroup::SetGlobalPos( rcPos );

    UpdateProgress();
}

void RnProgressBar::SetGlobalPos( const D3DXVECTOR2& vPos )
{
    CUIGroup::SetGlobalPos( vPos );

    UpdateProgress();
}

void RnProgressBar::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    UpdateProgress();
}

void RnProgressBar::UpdateProgress()
{
    if( m_pOverPos == NULL || m_pOver == NULL )
        return;

    UIRECT rcLocalPos = m_pOverPos->GetLocalPos();
	UIRECT rcPos = m_pOverPos->GetGlobalPos();

	switch( m_eType )
    {
    case HORIZONTAL:
        {
            rcLocalPos.sizeX = rcLocalPos.sizeX * m_fRate;
            rcLocalPos.right = rcLocalPos.left + rcLocalPos.sizeX;	
            m_pOver->SetLocalPos( rcLocalPos );

            rcPos.sizeX = rcPos.sizeX * m_fRate;
            rcPos.right = rcPos.left + rcPos.sizeX;	
            m_pOver->SetGlobalPos( rcPos );
        }
        break;

    case VERTICAL:
        {
            rcLocalPos.sizeY = rcLocalPos.sizeY * m_fRate;
            rcLocalPos.top = rcLocalPos.bottom - rcLocalPos.sizeY;
            m_pOver->SetLocalPos( rcLocalPos );

            rcPos.sizeY = rcPos.sizeY * m_fRate;
            rcPos.top = rcPos.bottom - rcPos.sizeY;
            m_pOver->SetGlobalPos( rcPos );
        }
        break;
    }
}

void RnProgressBar::CalculateRate()
{
	if( m_nNow <= 0 || m_nMax <= 0 )
	{
		m_fRate = 0.0f;
	}
	else if( m_nNow >= m_nMax )
	{
		m_fRate = 1.0f;
	}
	else
	{
		m_fRate = float( m_nNow ) / float( m_nMax );
	}
}

void RnProgressBar::CalculateValue()
{
	if ( m_fRate < 0.0f )
		m_fRate = 0.0f;
	else if ( m_fRate > 1.0f)
		m_fRate = 1.0f;

	m_nNow = static_cast<DWORD>( m_fRate * m_nMax );
}

bool RnProgressBar::IsEndProgress()
{
	if( m_fRate >= 1.0f )
		return true;

	if( m_nNow >= m_nMax )
		return true;

	return false;
}