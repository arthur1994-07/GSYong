#include "StdAfx.h"
#include "DoublePoint.h"
#include "DoublePointType.h"
#include "../../InnerInterface.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../EngineLib/DxEffect/EffProj/DxEffProj.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DoublePoint::DoublePoint(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient ( pGaeaClient )
, m_pInterface  ( pInterface )
{
}

DoublePoint::~DoublePoint ()
{
}

void DoublePoint::CreateSubControl ()
{
	CString strKeyword[MAXTYPE] = 
	{
		"RNCDM_DOUBLE_POINT",
	};

	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_pQI_TYPE[i] = new DoublePintType(m_pEngineDevice);
		m_pQI_TYPE[i]->CreateSub ( this, strKeyword[i].GetString(), UI_FLAG_DEFAULT, DOUBLE_POINT + i );
		m_pQI_TYPE[i]->CreateSubControl ( strKeyword[i] );
		m_pQI_TYPE[i]->SetVisibleSingle ( FALSE );
		m_pQI_TYPE[i]->SetUseRender ( FALSE );
		m_pQI_TYPE[i]->STOP ();
		m_pQI_TYPE[i]->RESET ();		
		RegisterControl ( m_pQI_TYPE[i] );

		m_QI_TYPE_KEEP[i] = new CUIControl(m_pEngineDevice);
		m_QI_TYPE_KEEP[i]->CreateSub ( this, strKeyword[i].GetString() );
		m_QI_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
		RegisterControl ( m_QI_TYPE_KEEP[i] );
	}

	m_pPositionControl = new CUIControl(m_pEngineDevice);
	m_pPositionControl->CreateSub ( this, "QUESTION_ITEM_POSITION" );
	m_pPositionControl->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pPositionControl );
}

bool	DoublePoint::START ( UIGUID cID )
{
	if ( cID < DOUBLE_POINT || MAXTYPE  < cID )
		return false;

	int nIndex = cID - DOUBLE_POINT;
	m_pQI_TYPE[nIndex]->SetVisibleSingle ( TRUE );
	m_pQI_TYPE[nIndex]->START ();

	return true;
}

bool	DoublePoint::RESET ( UIGUID cID )
{
	if ( cID < DOUBLE_POINT || MAXTYPE  < cID ) return false;

	int nIndex = cID - DOUBLE_POINT;	
	m_pQI_TYPE[nIndex]->RESET ();

	return true;
}

bool	DoublePoint::STOP ( UIGUID cID )
{
	if ( cID < DOUBLE_POINT || MAXTYPE  < cID ) return false;

	int nIndex = cID - DOUBLE_POINT;	
	m_pQI_TYPE[nIndex]->STOP ();
	m_pQI_TYPE[nIndex]->SetVisibleSingle ( FALSE );

	return true;
}

void DoublePoint::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	D3DXVECTOR3 vPos = m_pGaeaClient->GetCharacter()->GetPosBodyHeight();

	static D3DXVECTOR3 vScreenBack;
	D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	// 마우스 움직임에 흔들림을 보정한다.
	if( abs( vScreenBack.x - vScreen.x ) < 1.0f )
	{
		vScreen.x = vScreenBack.x;
	}

	bool bPLAYING( false );
	bool bKEEPING( false );

	for ( int i = 0; i < MAXTYPE; ++i )
	{
		const UIRECT& rcOriginPos = m_pQI_TYPE[i]->GetGlobalPos ();

		D3DXVECTOR2 vPos;
		vPos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
		vPos.y = m_pPositionControl->GetGlobalPos().top;

		if ( m_pQI_TYPE[i]->ISPLAYING () )
		{
			m_pQI_TYPE[i]->SetGlobalPos ( vPos );

			bPLAYING = true;
		}
		else
		{
			STOP ( DOUBLE_POINT + i );
		}

		if ( m_QI_TYPE_KEEP[i]->IsVisible () )
		{
			m_QI_TYPE_KEEP[i]->SetGlobalPos ( vPos );

			bKEEPING = true;
		}
	}

	vScreenBack = vScreen;

	if ( !bPLAYING && !bKEEPING ) m_pInterface->UiHideGroup ( GetWndID () );
}

void DoublePoint::ALLSTOP ()
{
	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_pQI_TYPE[i]->STOP ();
		m_pQI_TYPE[i]->RESET ();
		m_pQI_TYPE[i]->SetVisibleSingle ( FALSE );

		m_QI_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
	}

	m_pInterface->UiHideGroup ( GetWndID () );
}

bool DoublePoint::KEEP_START ( UIGUID cID )
{
	if ( cID < DOUBLE_POINT || MAXTYPE  < cID ) return false;

	int nIndex = cID - DOUBLE_POINT;	

	m_QI_TYPE_KEEP[nIndex]->SetVisibleSingle ( TRUE );
	m_QI_TYPE_KEEP[nIndex]->SetDiffuseAlpha ( 50 );

	return true;
}

void DoublePoint::KEEP_STOP ()
{
	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_QI_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
	}
}

void DoublePoint::SetVisibleSingle( BOOL bIsVisible )
{
	CUIGroup::SetVisibleSingle(bIsVisible);
	DxEffProjMan::GetInstance().EnableDoublePointEFF ( bIsVisible );
}
