#include "pch.h"
#include "BasicProgressBar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CBasicProgressBar::fZERO_PERCENT = 0.0f;
const float CBasicProgressBar::fHUNDRED_PERCENT = 1.0f;

CBasicProgressBar::CBasicProgressBar(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_fPercent ( fZERO_PERCENT )
    , m_pOverImage ( NULL )
    , m_nType ( HORIZONTAL )
{
	memset(m_pGrid, 0, sizeof(m_pGrid));
	memset(m_pGrid_OutLine, 0, sizeof(m_pGrid_OutLine));
}

CBasicProgressBar::~CBasicProgressBar ()
{
}

void CBasicProgressBar::SetPercent ( float fPercent )
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	if ( fPercent < fZERO_PERCENT )			fPercent = fZERO_PERCENT;
	else if ( fHUNDRED_PERCENT < fPercent )	fPercent = fHUNDRED_PERCENT;

	m_fPercent = fPercent;
}

float CBasicProgressBar::GetPercent ( void )
{
	return m_fPercent;
}

void CBasicProgressBar::UpdateProgress ()
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	UIRECT rcPos = m_pOverImageDummy->GetGlobalPos();
	UIRECT rcTexPos = m_pOverImageDummy->GetTexturePos ();
	switch ( m_nType )
	{
	case HORIZONTAL_REVERS:
		{
			rcPos.sizeX = rcPos.sizeX ;
			rcPos.right = rcPos.left + rcPos.sizeX * m_fPercent;	
			m_pOverImage->SetGlobalPos ( rcPos );

			rcTexPos.sizeX = rcTexPos.sizeX;
			rcTexPos.right = rcTexPos.left + rcTexPos.sizeX * m_fPercent;
			m_pOverImage->SetTexturePos ( rcTexPos );
		}
		break;
	case HORIZONTAL:
		{
			rcPos.sizeX = rcPos.sizeX * m_fPercent;
			rcPos.right = rcPos.left + rcPos.sizeX;	
			m_pOverImage->SetGlobalPos ( rcPos );
			
			rcTexPos.sizeX = rcTexPos.sizeX * m_fPercent;
			rcTexPos.right = rcTexPos.left + rcTexPos.sizeX;
			m_pOverImage->SetTexturePos ( rcTexPos );
		}
		break;

	case VERTICAL:
		{
			rcPos.sizeY = rcPos.sizeY * m_fPercent;
			rcPos.top = rcPos.bottom - rcPos.sizeY;
			m_pOverImage->SetGlobalPos ( rcPos );
			
			rcTexPos.sizeY = rcTexPos.sizeY * m_fPercent;
			rcTexPos.top = rcTexPos.bottom - rcTexPos.sizeY;
			m_pOverImage->SetTexturePos ( rcTexPos );
		}
		break;
	}
}

void CBasicProgressBar::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	UpdateProgress ();
}

void CBasicProgressBar::CreateOverImage ( const char* szOverImage )
{
	CUIControl* pOverImage = new CUIControl(m_pEngineDevice);
	pOverImage->CreateSub ( this, szOverImage );
	RegisterControl ( pOverImage );
	m_pOverImage = pOverImage;
	
	CUIControl* pOverImageDummy = new CUIControl(m_pEngineDevice);
	pOverImageDummy->CreateSub ( this, szOverImage );
	pOverImageDummy->SetVisibleSingle ( FALSE );
	RegisterControl ( pOverImageDummy );
	m_pOverImageDummy = pOverImageDummy;
}

void CBasicProgressBar::CreateGrid( char* szGrid )
{
	std::string strBaseName = GetControlNameEx();

	CUIControl BaseControl(m_pEngineDevice);
	BaseControl.CreateSub( this, strBaseName.c_str() );
	UIRECT rcBase = BaseControl.GetLocalPos();

	for( UINT i=0; i<MAX_GRID; ++i )
	{
		CUIControl* pGrid = new CUIControl(m_pEngineDevice);
		pGrid->CreateSub ( this, szGrid );
		pGrid->SetUseRender( TRUE );
		pGrid->SetDiffuse( 0xFF000000 );
		RegisterControl ( pGrid );
		m_pGrid[i] = pGrid;
		const UIRECT& rcGrid = pGrid->GetLocalPos();

		switch( GetType() )
		{
		case VERTICAL:
			{
				pGrid->SetAlignFlag( UI_FLAG_XSIZE );
				pGrid->SetLocalPos( UIRECT( 0.0f, static_cast<float>(i+1) * rcBase.sizeY/(MAX_GRID+1), rcBase.sizeX, rcGrid.sizeY ) );
			}
			break;

		case HORIZONTAL:
			{
				pGrid->SetAlignFlag( UI_FLAG_YSIZE );
				pGrid->SetLocalPos( UIRECT( static_cast<float>(i+1) * rcBase.sizeX/(MAX_GRID+1), 0.0f, rcGrid.sizeX, rcBase.sizeY ) );
			}
			break;
		};
	}

	CUIControl* pGrid = new CUIControl(m_pEngineDevice);
	pGrid->CreateSub ( this, szGrid, UI_FLAG_YSIZE );
	pGrid->SetUseRender( TRUE );
	pGrid->SetLocalPos( UIRECT( 0.0f, 0.0f, 1.0f, rcBase.sizeY ) );
	pGrid->SetDiffuse( 0xFF000000 );
	RegisterControl ( pGrid );
	m_pGrid_OutLine[0] = pGrid;

	pGrid = new CUIControl(m_pEngineDevice);
	pGrid->CreateSub ( this, szGrid, UI_FLAG_XSIZE );
	pGrid->SetUseRender( TRUE );
	pGrid->SetLocalPos( UIRECT( 0.0f, 0.0f, rcBase.sizeX, 1.0f ) );
	pGrid->SetDiffuse( 0xFF000000 );
	RegisterControl ( pGrid );
	m_pGrid_OutLine[1] = pGrid;

	pGrid = new CUIControl(m_pEngineDevice);
	pGrid->CreateSub ( this, szGrid, UI_FLAG_YSIZE );
	pGrid->SetUseRender( TRUE );
	pGrid->SetLocalPos( UIRECT( rcBase.sizeX-1.0f, 0.0f, 1.0f, rcBase.sizeY ) );
	pGrid->SetDiffuse( 0xFF000000 );
	RegisterControl ( pGrid );
	m_pGrid_OutLine[2] = pGrid;

	pGrid = new CUIControl(m_pEngineDevice);
	pGrid->CreateSub ( this, szGrid, UI_FLAG_XSIZE );
	pGrid->SetUseRender( TRUE );
	pGrid->SetLocalPos( UIRECT( 0.0f, rcBase.sizeY-1.0f, rcBase.sizeX, 1.0f ) );
	pGrid->SetDiffuse( 0xFF000000 );
	RegisterControl ( pGrid );
	m_pGrid_OutLine[3] = pGrid;
}

void CBasicProgressBar::SetOverImageDiffuse ( DWORD dwDiffuse )
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	m_pOverImage->SetDiffuse ( dwDiffuse );
}

DWORD CBasicProgressBar::GetOverImageDiffuse ()
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return 0;
	}

	return m_pOverImage->GetDiffuse ();
}

void CBasicProgressBar::SetOverImageUseRender ( BOOL bUseRender )
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return ;
	}

	m_pOverImage->SetUseRender ( bUseRender );
}

BOOL CBasicProgressBar::IsOverImageUseRender ()
{
	if ( !m_pOverImage )
	{
		GASSERT ( 0 && "CreateOverImage () 호출뒤에 사용하십시오." );
		return FALSE;
	}

	return m_pOverImage->IsUseRender ();
}

void CBasicProgressBar::SetAlignFlag ( WORD wFlag )
{
	CUIGroup::SetAlignFlag ( wFlag );

	m_pOverImage->SetAlignFlag ( wFlag );
	m_pOverImageDummy->SetAlignFlag ( wFlag );
}

void CBasicProgressBar::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	CUIGroup::AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetAlignGrid ();
	SetPercent ( GetPercent () );
	UpdateProgress ();
}

void CBasicProgressBar::SetAlignGrid ()
{
	const UIRECT& rcBase = GetLocalPos();

	for( UINT i=0; i<MAX_GRID; ++i )
	{
		CUIControl* pGrid = m_pGrid[i];
		
		if ( !pGrid )
		{
			return;
		}
		const UIRECT& rcGrid = pGrid->GetLocalPos();

		switch( GetType() )
		{
		case VERTICAL:
			{
				pGrid->SetAlignFlag( UI_FLAG_XSIZE );
				pGrid->SetLocalPos( UIRECT( 0.0f, static_cast<float>(i+1) * rcBase.sizeY/(MAX_GRID+1), rcBase.sizeX, rcGrid.sizeY ) );
			}
			break;

		case HORIZONTAL:
			{
				pGrid->SetAlignFlag( UI_FLAG_YSIZE );
				pGrid->SetLocalPos( UIRECT( static_cast<float>(i+1) * rcBase.sizeX/(MAX_GRID+1), 0.0f, rcGrid.sizeX, rcBase.sizeY ) );
			}
			break;
		};
	}

	m_pGrid_OutLine[0]->SetLocalPos( UIRECT( 0.0f, 0.0f, 1.0f, rcBase.sizeY ) );
	m_pGrid_OutLine[1]->SetLocalPos( UIRECT( 0.0f, 0.0f, rcBase.sizeX, 1.0f ) );
	m_pGrid_OutLine[2]->SetLocalPos( UIRECT( rcBase.sizeX-1.0f, 0.0f, 1.0f, rcBase.sizeY ) );
	m_pGrid_OutLine[3]->SetLocalPos( UIRECT( 0.0f, rcBase.sizeY-1.0f, rcBase.sizeX, 1.0f ) );
}

void  CBasicProgressBar::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );
	
	//	혹시나.
	SetPercent ( GetPercent () );
	UpdateProgress ();
}

void CBasicProgressBar::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	SetPercent ( GetPercent () );
	UpdateProgress ();
}