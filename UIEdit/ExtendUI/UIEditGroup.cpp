#include "StdAfx.h"
#include "./UIEditGroup.h"

#include "../../EngineLib/GUInterface/BasicLineBox.h"
#include "../MainUI/InterfaceAdmin.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIEditGroup::CUIEditGroup (EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin)
    : CUIGroup(pEngineDevice)
    , m_pInterfaceAdmin(pInterfaceAdmin)
    , m_nSelected( CONTROL_NORMAL )
    , m_bFirstGap( FALSE )
    , m_vGap( D3DXVECTOR2( 0, 0 ) )
    , m_PosX( 0 )
    , m_PosY( 0 )
    , m_pSelLineBox( NULL )
    , m_bLineView( TRUE )
{
}

CUIEditGroup::~CUIEditGroup ()
{
}

void CUIEditGroup::CreateWindowControl (const TCHAR* szControl)
{
	m_pSelLineBox = new CBasicLineBox(m_pEngineDevice);
	m_pSelLineBox->SetCalcResizeWindow( TRUE );
	m_pSelLineBox->CreateSub( this, _T("BASIC_LINE_BOX_EDIT"), UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pSelLineBox->CreateBaseBoxOption ( "CHATMACRO_WINDOW_BACK_BOX" );
	m_pSelLineBox->SetDiffuse( D3DCOLOR_ARGB(255,0,0,0) );
	RegisterControl( m_pSelLineBox );
	ResizeControl(szControl);
}

void CUIEditGroup::ResizeControl(const TCHAR* szControl)
{	
	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szControl );
	UIRECT rcParentNewPos = TempControl.GetLocalPos();

	ResizeWindow(rcParentNewPos);
}

void CUIEditGroup::ResizeWindow(const UIRECT& rcNew)
{
	UIRECT rcChange;

	rcChange.sizeX	= rcNew.sizeX;
	rcChange.sizeY	= rcNew.sizeY;
	rcChange.left	= 0;
	rcChange.top	= 0;
	rcChange.right	= rcChange.left + rcChange.sizeX;
	rcChange.bottom	= rcChange.top + rcChange.sizeY;

	m_pSelLineBox->SetControlSize(rcChange.sizeX, rcChange.sizeY);
	m_pSelLineBox->ResizeWindow(rcChange);

}

void CUIEditGroup::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_PosX = x;
	m_PosY = y;

	CUIEditGroup* pControl= m_pInterfaceAdmin->GetSelectedControl();

	if( this == pControl && pControl )
	{
		SetSelected( CONTROL_SELECT );
	}
	else
	{
		SetSelected( CONTROL_NORMAL );

		for (int nIndex=0;nIndex<m_ChildID.size();nIndex++)
		{
			CUIEditGroup* pChildControl = m_pInterfaceAdmin->FindUIListCtrl( m_ChildID[nIndex] );
			if(pChildControl)
			{
				if( pChildControl == pControl )
				{
					SetSelected( CONTROL_PARENT );
					break;
				}
			}
		}
	}

	if(bFirstControl)
	{
		CheckMousestate();
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsExclusiveSelfControl() ) // 지금 현재 마우스 왼쪽 버튼으로 지정된 상태
	{
		if(bFirstControl)
		{
			int nPosX,nPosY;

			nPosX = x - m_vGap.x;
			nPosY = y - m_vGap.y;

			SetLocalPos ( D3DXVECTOR2 (  nPosX, nPosY ) ); // 윈도우를 움직인다.	
			RestoreDeviceObjects( DXUTGetD3DDevice() );
		}
	}
}



void CUIEditGroup::CheckMousestate()
{
	const DWORD dwMsg = GetMessageEx();

	if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
	{
		if ( CHECK_LB_DOWN_LIKE(dwMsg) )	//	마우스를 좌클릭으로 눌렀을 때,
		{			
			SetExclusiveControl();	//	단독 컨트롤로 등록하고,
			if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
			{
				UIRECT rcPos = GetLocalPos ();
				m_vGap.x = m_PosX - rcPos.left;
				m_vGap.y = m_PosY - rcPos.top;
				m_bFirstGap = TRUE;
			}
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
		{
			ResetExclusiveControl();	//	단독 컨트롤을 해제하고
			m_bFirstGap = FALSE;
		}
	}
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
	{											//	발생하더라도
		ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
		m_bFirstGap = FALSE;
	}
}

void CUIEditGroup::SetSelected(int nSel)
{
	m_nSelected = nSel;

	m_pSelLineBox->SetVisibleSingle( GetLineView() );

	if( m_pSelLineBox )
	{	
		switch( m_nSelected )
		{
		case CONTROL_NORMAL:
			{
				m_pSelLineBox->SetDiffuse( D3DCOLOR_ARGB(255,0,0,0) );
			}
			break;
		case CONTROL_SELECT:
			{
				m_pSelLineBox->SetDiffuse( D3DCOLOR_ARGB(255,255,255,255) );
			}
			break;
		case CONTROL_PARENT:
			{
				m_pSelLineBox->SetDiffuse( D3DCOLOR_ARGB(255,228,247,68) );
			}
			break;
		}
	}
}

int CUIEditGroup::FindChild(UIGUID ControlID)
{
	for (int nIndex=0;nIndex<m_ChildID.size();nIndex++)
	{
		if( m_ChildID[nIndex] == ControlID )
			return nIndex;
	}
	return -1;
}

void CUIEditGroup::AddChild(UIGUID ControlID)
{
	if( FindChild(ControlID) == -1 ) m_ChildID.push_back(ControlID);
}

void CUIEditGroup::DelChild(UIGUID ControlID)
{
	std::vector<UIGUID>::iterator iter;

	iter = m_ChildID.begin() + FindChild(ControlID);
	m_ChildID.erase(iter);
}

void CUIEditGroup::SetGlobalPos(const UIRECT& Pos)
{
	const UIRECT rcOriginPos = GetGlobalPos ();	

	CUIControl::SetGlobalPos ( Pos );
	if( GetParent() )
	{
		UIRECT rcParent = GetParent()->GetGlobalPos();
		CUIControl::SetLocalPos ( UIRECT(Pos.left - rcParent.left, Pos.top - rcParent.top, Pos.sizeX, Pos.sizeY) );

		AlignSubControlEX( rcParent, rcParent );
	}
	else 
	{
		CUIControl::SetLocalPos ( Pos );

		UIRECT rcNewWindow( 0, 0, m_pInterfaceAdmin->m_nViewWidth, m_pInterfaceAdmin->m_nViewHeight );
		AlignMainControlEX( rcNewWindow, rcNewWindow );
	}

	const UIRECT rcNewPos = GetGlobalPos ();	

	for (int nIndex=0;nIndex<m_ChildID.size();nIndex++)
	{
		CUIEditGroup* pControl = m_pInterfaceAdmin->FindUIListCtrl( m_ChildID[nIndex] );
		if(pControl)
		{
			//const UIRECT rcNewChildPos( rcOriginPos.left + rcChildPos.left, rcOriginPos.top + rcChildPos.top,
			//	rcChildPos.sizeX, rcChildPos.sizeY );
			pControl->AlignSubControlEX( rcOriginPos, rcNewPos );

			const UIRECT& rcChildPos = pControl->GetGlobalPos ();
			pControl->SetGlobalPos( rcChildPos  );
		}
	}
}

void CUIEditGroup::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	//CUIControl::SetGlobalPos ( vPos );

	const UIRECT& rcOriginPos = GetGlobalPos ();	
	const UIRECT rcNewChildPos( vPos.x, vPos.y,	rcOriginPos.sizeX, rcOriginPos.sizeY );
	SetGlobalPos (rcNewChildPos);
}

void CUIEditGroup::SetLocalPos(const UIRECT& Pos)
{
	UIRECT rcMovePos = Pos;

	if( GetParent() )
	{
		UIRECT rcParentPos = GetParent()->GetLocalPos();
		rcMovePos.sizeX = min( rcMovePos.sizeX, rcParentPos.sizeX );
		rcMovePos.sizeY = min( rcMovePos.sizeY, rcParentPos.sizeY );
	}
	else
	{
		rcMovePos.sizeX = min( rcMovePos.sizeX, m_pInterfaceAdmin->m_nViewWidth );
		rcMovePos.sizeY = min( rcMovePos.sizeY, m_pInterfaceAdmin->m_nViewHeight );
	}

	rcMovePos.left = max(0,rcMovePos.left);
	rcMovePos.top  = max(0,rcMovePos.top);
	rcMovePos.right  = rcMovePos.left + rcMovePos.sizeX;
	rcMovePos.bottom = rcMovePos.top  + rcMovePos.sizeY;

	if( rcMovePos.right >= m_pInterfaceAdmin->m_nViewWidth )
	{
		rcMovePos.right = m_pInterfaceAdmin->m_nViewWidth;
		rcMovePos.left = rcMovePos.right - rcMovePos.sizeX;
	}

	if( rcMovePos.bottom >= m_pInterfaceAdmin->m_nViewHeight )
	{
		rcMovePos.bottom = m_pInterfaceAdmin->m_nViewHeight;
		rcMovePos.top = rcMovePos.bottom - rcMovePos.sizeY;
	}

	CUIControl::SetLocalPos ( rcMovePos );

	if(GetParent())	
	{
		SetGlobalPos(UIRECT(GetParent()->GetGlobalPos().left + GetLocalPos().left,GetParent()->GetGlobalPos().top + GetLocalPos().top,GetLocalPos().sizeX ,GetLocalPos().sizeY ));
	}
	else SetGlobalPos(rcMovePos);
}

void CUIEditGroup::SetLocalPos(const D3DXVECTOR2& vPos)
{
	D3DXVECTOR2 vMovePos = vPos;

	vMovePos.x = max(0,vMovePos.x);
	vMovePos.y = max(0,vMovePos.y);

	const UIRECT& rcOriginPos = GetLocalPos ();	
	const UIRECT rcNewChildPos( vMovePos.x, vMovePos.y,	rcOriginPos.sizeX, rcOriginPos.sizeY );

	SetLocalPos ( rcNewChildPos );
}

void CUIEditGroup::SetVisibleSingle ( BOOL bVisible )
{
	CUIControl::SetVisibleSingle( bVisible );

	for (int nIndex=0;nIndex<m_ChildID.size();nIndex++)
	{
		CUIEditGroup* pControl = m_pInterfaceAdmin->FindUIListCtrl( m_ChildID[nIndex] );
		if(pControl)
		{
			pControl->SetVisibleSingle( bVisible );
		}
	}
}

HRESULT CUIEditGroup::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//MIN_ASSERT( pd3dDevice );

	HRESULT hr = S_OK;

	UIRECT rcOriginPos = GetGlobalPos();
	SetGlobalPos( rcOriginPos );
	//UIRECT rcOriginPos = GetLocalPos ();
	//	
	//if( GetParent() )
	//{
	//	UIRECT rcParent = GetParent()->GetGlobalPos();
	//	AlignSubControlEX( rcParent , rcParent );
	//}
	//else 
	//{
	//	UIRECT rcNewWindow( 0, 0, m_pInterfaceAdmin->m_nViewWidth, m_pInterfaceAdmin->m_nViewHeight );
	//	UIRECT rcLastWindow( 0, 0, m_pInterfaceAdmin->m_nLastViewWidth, m_pInterfaceAdmin->m_nLastViewHeight );

	//	AlignMainControlEX( rcLastWindow, rcNewWindow );
	//}

	CUIGroup::RestoreDeviceObjects ( DXUTGetD3DDevice() );

	return S_OK;
}

void CUIEditGroup::AlignMainControlEX ( const UIRECT& rcLastParentPos, const UIRECT& rcNewParentPos )
{
	WORD wAlignFlag = GetAlignFlag();

	SetVisibleSingle(TRUE);

	const UIRECT& rcOriginPos = GetLocalPos ();

	CUIControl::AlignMainControlEX( rcLastParentPos, rcNewParentPos );
	ResizeWindow(GetGlobalPos());
	CUIGroup::RestoreDeviceObjects ( DXUTGetD3DDevice() );

	const UIRECT& rcNewPos = GetLocalPos ();
}

void CUIEditGroup::AlignSubControlEX( const UIRECT& rcLastParentPos, const UIRECT& rcNewParentPos )
{
	const UIRECT& rcOriginPos = GetLocalPos();

	CUIControl::AlignSubControlEX(rcLastParentPos, rcNewParentPos);
	ResizeWindow(GetGlobalPos());
	CUIGroup::RestoreDeviceObjects ( DXUTGetD3DDevice() );

	const UIRECT& rcNewPos = GetLocalPos ();

	//for (int nIndex=0;nIndex<m_ChildID.size();nIndex++)
	//{
	//	CUIEditGroup* pControl = m_pInterfaceAdmin->FindUIListCtrl( m_ChildID[nIndex] );
	//	if(pControl)
	//	{
	//		pControl->AlignSubControlEX( rcOriginPos, rcNewPos );
	//	}
	//}
}
