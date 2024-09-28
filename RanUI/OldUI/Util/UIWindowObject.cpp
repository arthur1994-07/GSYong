
// bjju.sns

#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIWindowTitle.h"
#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../InnerInterface.h"

#include "./UIWindowObjectController.h"
#include "./UIWindowObject.h"
#include "./UIPage.h"
#include "./UIPageFrame.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIWindowObject::CUIWindowObject (CInnerInterface* pInteface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx           (pInteface, pEngineDevice)
    , m_pFrame				( NULL )
    , m_pParentController	( NULL )
    , m_pd3dDevice			( NULL )
    , m_dwSlot				( CUIWindowObjectController::SLOT_NULL )
    , m_dwFlag              ( 0 )
    , m_dwAniType			( WINDOW_ANIMATION_NONE )
    , m_fAniTime			( 0.0f )
    , m_fAniSpeedPerSec		( 1.0f )
    , m_pAniDest			( NULL )
    , m_bAniPlay			( FALSE )
{
	// Blank
}

CUIWindowObject::~CUIWindowObject ()
{
	m_pd3dDevice = NULL;
	m_pFrame	 = NULL;
}

void CUIWindowObject::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	CreateSubControlEx();
}

void CUIWindowObject::CreatePageFrame ( const char* szFrameKeyword, const bool bDefaultPos )
{
	m_pFrame = new CUIPageFrame(m_pEngineDevice);
	m_pFrame->CreateSub	( this, szFrameKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE, WINDOW_OBJECT_FRAME );
	m_pFrame->SetAlignFlag ( UI_FLAG_LEFT | UI_FLAG_TOP );
	m_pFrame->SetVisibleSingle ( TRUE );
	RegisterControl ( m_pFrame );

    if ( bDefaultPos )
    {
        SetFramePos ( m_pFrame );
    }
}

void CUIWindowObject::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_WINDOW_OBJECT_CLOSE );
			}
		}
		break;

	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				DWORD dwButtonMsg = UIMSG_NOMESSAGE;
				if ( m_pClose )	dwButtonMsg = m_pClose->GetMessageEx ();
				if ( (dwMsg & UIMSG_LB_DOWN) && !CHECK_MOUSE_IN ( dwButtonMsg ) )	//	마우스를 좌클릭으로 눌렀을 때,
				{	
					SetParentExclusiveControl();	//	단독 컨트롤로 등록하고,
					SetExclusiveControl();

					if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
					{
						UIRECT rcPos = GetTopControl()->GetGlobalPos ();

						m_vGap.x = m_PosX - rcPos.left;
						m_vGap.y = m_PosY - rcPos.top;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					ResetParentExclusiveControl();	//	단독 컨트롤을 해제하고
					ResetExclusiveControl();
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				ResetParentExclusiveControl();		//	단독 컨트롤을 해제하고,
				ResetExclusiveControl();
			}
		}
		break;

	case WINDOW_OBJECT_FRAME:
		AddMessageEx( dwMsg );
		break;

	};

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CUIWindowObject::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	UpdateAnimation	( fElapsedTime );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CUIControl* pParent = GetParent();

	if ( !pParent )
	{
		return;
	}

	if( m_PosX != x || m_PosY != y )
	{
		m_PosX = x;
		m_PosY = y;

		// 지금 현재 마우스 왼쪽 버튼으로 지정된 상태
		if ( IsExclusiveSelfControl() )
		{		
			// 최상단 윈도우를 움직인다.
			UIRECT rcParent = pParent->GetGlobalPos();
			GetTopControl()->SetGlobalPos ( D3DXVECTOR2 ( x - m_vGap.x, y - m_vGap.y ) );
		}
	}
}

void CUIWindowObject::UpdateAnimation ( float fElapsedTime )
{
	if ( !m_bAniPlay )
	{
		return;
	}

	FLOAT fSpeed = m_fAniSpeedPerSec*fElapsedTime;
	m_fAniTime += fSpeed;

	if ( m_fAniTime >= 1.0f )
	{
		m_fAniTime = 1.0f;
		m_bAniPlay = FALSE;
	}

	switch ( m_dwAniType )
	{
	default:
	case WINDOW_ANIMATION_NONE:
		{
			m_bAniPlay = FALSE;

			SetResize ( m_rcAniRectDest );
		}
		break;

	case WINDOW_ANIMATION_LINEAR:
		{
			UIRECT rcAni;

			rcAni.left   = m_rcAniRectSrc.left    *(1.0f-m_fAniTime) + m_rcAniRectDest.left    *m_fAniTime;
			rcAni.right  = m_rcAniRectSrc.right   *(1.0f-m_fAniTime) + m_rcAniRectDest.right   *m_fAniTime;
			rcAni.top    = m_rcAniRectSrc.top	  *(1.0f-m_fAniTime) + m_rcAniRectDest.top     *m_fAniTime;
			rcAni.bottom = m_rcAniRectSrc.bottom  *(1.0f-m_fAniTime) + m_rcAniRectDest.bottom  *m_fAniTime;
			rcAni.sizeX  = m_rcAniRectSrc.sizeX	  *(1.0f-m_fAniTime) + m_rcAniRectDest.sizeX   *m_fAniTime;
			rcAni.sizeY  = m_rcAniRectSrc.sizeY   *(1.0f-m_fAniTime) + m_rcAniRectDest.sizeY   *m_fAniTime;

			SetResize ( rcAni );
		}
		break;
	};
}

void CUIWindowObject::PageOpen  ( CUIPage* pPage )
{
	BeginOpenAnimation ( pPage );
	
	if ( m_pFrame )
	{
		BOOL bAni = m_dwFlag&FLAG_PAGE_CHANGE_ANIMATION;
		m_pFrame->PageOpen ( this, pPage, bAni );

        // Note : 타이틀
        BOOL bTitleNonChange = m_dwFlag&FLAG_TITLE_NAME_NONCHANGE;
		if ( (pPage && m_pTitle) && !bTitleNonChange )
		{
            SetTitleName( pPage->GetTitleName().GetString() );
		}
	}
}

void CUIWindowObject::PageClose ()
{
	if ( m_pFrame )
	{
		m_pFrame->PageClose ();
	}
}

void CUIWindowObject::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	CUIGroup::SetGlobalPos( vPos );
}

void CUIWindowObject::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle( bVisible );
}

void CUIWindowObject::WindowOpen ()
{
	SetVisibleSingle ( TRUE );

	if ( m_pFrame )
	{
		m_pFrame->OpenSubControl();
	}
}

void CUIWindowObject::WindowClose ()
{
	SetVisibleSingle ( FALSE );

	if ( m_pFrame )
	{
		m_pFrame->CloseSubControl();
	}
}

const BOOL CUIWindowObject::IsPageOpen ( CUIPage* pPage )
{
	if ( m_pFrame )
	{
		return (m_pFrame->m_pPage == pPage);
	}

	return FALSE;
}

void CUIWindowObject::SetResize ( const char* szControlKeyword )
{
	if ( szControlKeyword == NULL )
	{
		return;
	}

	CUIControl TempControl(m_pEngineDevice); 
	TempControl.Create ( 1, szControlKeyword ); // MEMO

	UIRECT rcOldLocalPos	  = GetLocalPos ();
	UIRECT rcControlLoacalPos = TempControl.GetLocalPos ();

	AlignSubControl( rcOldLocalPos, rcControlLoacalPos );
	SetLocalPos( rcControlLoacalPos );

	if ( m_pParentController )
	{
		m_pParentController->WindowResize();
	}
}

void CUIWindowObject::SetResize ( CUIPage* pPage )
{
	if ( !pPage )
	{
		return;
	}

	UIRECT rcOldLocalPos	  = GetLocalPos ();
	UIRECT rcControlLoacalPos = pPage->GetLocalPos ();
	UIRECT rcTitlePos		  = UIRECT();
    
    if ( m_pTitle )
    {
        rcTitlePos = m_pTitle->GetLocalPos();
    }

	rcControlLoacalPos.left	  = rcOldLocalPos.left;
	rcControlLoacalPos.right  = rcOldLocalPos.left + rcControlLoacalPos.sizeX + 4.0f;
	rcControlLoacalPos.top    = rcOldLocalPos.top;
	rcControlLoacalPos.bottom = rcOldLocalPos.top  + rcControlLoacalPos.sizeY + rcTitlePos.sizeY;
	rcControlLoacalPos.sizeX  = rcControlLoacalPos.sizeX + 4.0f; 
	rcControlLoacalPos.sizeY  = rcControlLoacalPos.sizeY + rcTitlePos.sizeY; 

	AlignSubControl	( rcOldLocalPos, rcControlLoacalPos );

	SetLocalPos ( D3DXVECTOR2 ( rcControlLoacalPos.left, rcControlLoacalPos.top ) );

	UIRECT rcGlobalPos		  = GetGlobalPos ();
	UIRECT rcControlGlobalPos = rcGlobalPos;
	rcControlGlobalPos.right  = rcControlGlobalPos.left + rcControlLoacalPos.sizeX;
	rcControlGlobalPos.bottom = rcControlGlobalPos.top  + rcControlLoacalPos.sizeY;
	rcControlGlobalPos.sizeX  = rcControlLoacalPos.sizeX;
	rcControlGlobalPos.sizeY  = rcControlLoacalPos.sizeY;

	CUIGroup::SetGlobalPos( rcControlGlobalPos );

	if ( m_pParentController )
	{
		m_pParentController->WindowResize();
	}
}

void CUIWindowObject::SetResize ( UIRECT& rcSize )
{
	UIRECT rcOldLocalPos	  = GetLocalPos ();
	UIRECT rcControlLoacalPos = rcSize;
	UIRECT rcTitlePos		  = UIRECT();

    if ( m_pTitle )
    {
        rcTitlePos = m_pTitle->GetLocalPos();
    }

	rcControlLoacalPos.left	  = rcOldLocalPos.left;
	rcControlLoacalPos.right  = rcOldLocalPos.left + rcControlLoacalPos.sizeX + 4.0f;
	rcControlLoacalPos.top    = rcOldLocalPos.top;
	rcControlLoacalPos.bottom = rcOldLocalPos.top  + rcControlLoacalPos.sizeY + rcTitlePos.sizeY;
	rcControlLoacalPos.sizeX  = rcControlLoacalPos.sizeX + 4.0f; 
	rcControlLoacalPos.sizeY  = rcControlLoacalPos.sizeY + rcTitlePos.sizeY; 

	AlignSubControl	( rcOldLocalPos, rcControlLoacalPos );

	SetLocalPos ( D3DXVECTOR2 ( rcControlLoacalPos.left, rcControlLoacalPos.top ) );

	UIRECT rcGlobalPos		  = GetGlobalPos ();
	UIRECT rcControlGlobalPos = rcGlobalPos;
	rcControlGlobalPos.right  = rcControlGlobalPos.left + rcControlLoacalPos.sizeX;
	rcControlGlobalPos.bottom = rcControlGlobalPos.top  + rcControlLoacalPos.sizeY;
	rcControlGlobalPos.sizeX  = rcControlLoacalPos.sizeX;
	rcControlGlobalPos.sizeY  = rcControlLoacalPos.sizeY;

	CUIGroup::SetGlobalPos( rcControlGlobalPos );

	if ( m_pParentController )
	{
		m_pParentController->WindowResize();
	}
}

void CUIWindowObject::SetParentResize ( char* szControlKeyword )
{
	if ( szControlKeyword == NULL )
	{
		return;
	}

	CUIControl* pParent = GetParent();

	if ( !pParent )
	{
		return;
	}

	CUIControl TempControl(m_pEngineDevice); 
	TempControl.Create ( 1, szControlKeyword ); // MEMO

	UIRECT rcParentLocalPos   = pParent->GetLocalPos ();
	UIRECT rcControlLoacalPos = TempControl.GetLocalPos ();

	pParent->AlignSubControl ( rcParentLocalPos, rcControlLoacalPos );
	pParent->SetLocalPos     ( rcControlLoacalPos );

	UIRECT rcParentGlobalOldPos = pParent->GetGlobalPos ();
	UIRECT rcParentGlobalNewPos = rcParentGlobalOldPos;

	rcParentGlobalNewPos.left   = rcParentGlobalOldPos.left;
	rcParentGlobalNewPos.top    = rcParentGlobalOldPos.top;
	rcParentGlobalNewPos.right  = rcParentGlobalNewPos.left + rcControlLoacalPos.sizeX;
	rcParentGlobalNewPos.bottom = rcParentGlobalNewPos.top  + rcControlLoacalPos.sizeY;
	rcParentGlobalNewPos.sizeX  = rcControlLoacalPos.sizeX;
	rcParentGlobalNewPos.sizeY  = rcControlLoacalPos.sizeY;

	pParent->SetGlobalPos ( rcParentGlobalNewPos );
}

void CUIWindowObject::SetFramePos ( CUIPageFrame* pFrame )
{
	if ( !m_pBody || !pFrame )
	{
		return;
	}

	UIRECT rcTitlePos;
    
    if ( m_pTitle )
    {
        rcTitlePos = m_pTitle->GetLocalPos();
    }

	pFrame->SetLocalPos  ( D3DXVECTOR2( 2.0f, rcTitlePos.sizeY ) );

	UIRECT rcGlobalPos		= pFrame->GetGlobalPos ();
	UIRECT rcParentGlobPos	= GetGlobalPos ();
	rcGlobalPos.left   = rcParentGlobPos.left + 2.0f;
	rcGlobalPos.top	   = rcParentGlobPos.top + rcTitlePos.sizeY;
	rcGlobalPos.right  = rcGlobalPos.left + rcGlobalPos.sizeX;
	rcGlobalPos.bottom = rcGlobalPos.top  + rcGlobalPos.sizeY;

	pFrame->SetGlobalPos ( rcGlobalPos );
}

CUIPage* CUIWindowObject::GetPage ()
{
	if ( !m_pFrame || !m_pFrame->m_pPage )
	{
		return NULL;
	}

	return m_pFrame->m_pPage;
}

BOOL CUIWindowObject::IsParentExclusiveControl()
{
	CUIControl* pControl = GetParent();

	if ( pControl )
	{
		return pControl->IsExclusiveControl();
	}

	return FALSE;
}

void CUIWindowObject::ResetParentExclusiveControl()
{
	CUIControl* pControl = GetParent();

	if ( pControl )
	{
		pControl->ResetExclusiveControl();
	}
}

void CUIWindowObject::SetParentExclusiveControl()
{
	CUIControl* pControl = GetParent();

	if ( pControl )
	{
		pControl->SetExclusiveControl();
	}
}

void CUIWindowObject::RequestPush ( const DWORD dwID, const DWORD dwArg1 )
{
	m_Request.push ( SPAGEREQUEST(dwID, dwArg1) );
}

const SPAGEREQUEST CUIWindowObject::RequestPop ()
{
	if ( m_Request.empty() )
	{
		return SPAGEREQUEST( CUIWindowObjectController::WINDOW_PAGE_REQ_NONE );
	}

	SPAGEREQUEST sRequest = m_Request.front();
	m_Request.pop();

	return sRequest;
}

void CUIWindowObject::BeginOpenAnimation ( CUIPage* pDest )
{
    m_fAniTime = 0.0f;
    m_bAniPlay = FALSE;

    if ( !m_pFrame )
    {
        CreatePageFrame ( "WINDOW_OBJECT_BASIC_FRAME", true );
    }

	if ( m_dwFlag&FLAG_PAGE_CHANGE_RESIZE_WINDOW_ANIMATION )
	{			
		if ( !m_pFrame->m_pPage || (m_pFrame->m_pPage == pDest) )
		{
			SetResize ( pDest );
			return;
		}

		switch ( m_dwAniType )
		{
		default:
		case WINDOW_ANIMATION_NONE:
			{
				SetResize ( pDest );
				return;
			}
			break;

		case WINDOW_ANIMATION_LINEAR:
			{
				if ( !pDest || !m_pFrame || !m_pFrame->m_pPage )
				{
					return;
				}

				m_bAniPlay = TRUE;

				m_pFrame->m_pPage->GetOrgLocalPos(m_rcAniRectSrc);
				pDest			 ->GetOrgLocalPos(m_rcAniRectDest);
			}
			break;
		};
	}

	m_pAniDest = pDest;
}

void CUIWindowObject::SetPageAniType  ( const DWORD dwType )
{
	if ( !m_pFrame )
	{
        CreatePageFrame ( "WINDOW_OBJECT_BASIC_FRAME", true );
	}

	m_pFrame->m_dwAniType = dwType;
}

void CUIWindowObject::SetPageAniSpeed ( const FLOAT fSpeedPerSec )
{
	if ( !m_pFrame )
	{
        CreatePageFrame ( "WINDOW_OBJECT_BASIC_FRAME", true );
	}

	m_pFrame->m_fAniSpeedPerSec = fSpeedPerSec;
}
