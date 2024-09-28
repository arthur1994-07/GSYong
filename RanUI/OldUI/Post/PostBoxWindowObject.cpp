#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"
#include "PostBoxPage.h"
#include "PostBoxPageFrame.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxWindowObject::CPostBoxWindowObject ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx	( pInterface, pEngineDevice )
	, m_pFrame		( NULL )
	, m_pd3dDevice	( NULL )
	, m_dwSlot		( CPostBoxWindow::SLOT_NULL )
{
	// Blank
}

CPostBoxWindowObject::~CPostBoxWindowObject ()
{
	m_pd3dDevice = NULL;
	m_pFrame	 = NULL;
}

void CPostBoxWindowObject::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	CreateSubControlEx();
}

void CPostBoxWindowObject::CreatePageFrame ( const char* szFrameKeyword )
{
	m_pFrame = new CPostBoxPageFrame(m_pEngineDevice);
	m_pFrame->CreateSub	( this, szFrameKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_WINDOW_OBJECT_FRAME );
	m_pFrame->SetAlignFlag ( UI_FLAG_LEFT | UI_FLAG_TOP );
	m_pFrame->SetVisibleSingle ( TRUE );
	RegisterControl ( m_pFrame );
}

void CPostBoxWindowObject::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE );
			}
		}
		break;

	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	��Ʈ�� ���ο� ���콺�� ��ġ�ϰ�,
			{
				DWORD dwButtonMsg = UIMSG_NOMESSAGE;
				if ( m_pClose )	dwButtonMsg = m_pClose->GetMessageEx ();
				if ( (dwMsg & UIMSG_LB_DOWN) && !CHECK_MOUSE_IN ( dwButtonMsg ) )	//	���콺�� ��Ŭ������ ������ ��,
				{	
					SetParentExclusiveControl();	//	�ܵ� ��Ʈ�ѷ� ����ϰ�,
					SetExclusiveControl();

					if ( !m_bFirstGap )				//	���� ������ ���� ����Ѵ�.
					{
						UIRECT rcPos = GetTopControl()->GetGlobalPos ();
					
						m_vGap.x = m_PosX - rcPos.left;
						m_vGap.y = m_PosY - rcPos.top;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	��Ŭ���ߴ����� ������
				{
					ResetParentExclusiveControl();	//	�ܵ� ��Ʈ���� �����ϰ�
					ResetExclusiveControl();
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	��Ʈ�� �����ۿ��� ��ư�� ���� ��찡
			{											//	�߻��ϴ���
				ResetParentExclusiveControl();		//	�ܵ� ��Ʈ���� �����ϰ�,
				ResetExclusiveControl();
			}
		}
		break;

	case POSTBOX_WINDOW_OBJECT_FRAME:
		AddMessageEx( dwMsg );
		break;

	};

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CPostBoxWindowObject::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
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

		// ���� ���� ���콺 ���� ��ư���� ������ ����
		if ( IsExclusiveSelfControl() )
		{		
			// �ֻ�� �����츦 �����δ�.
			UIRECT rcParent = pParent->GetGlobalPos();
			GetTopControl()->SetGlobalPos ( D3DXVECTOR2 ( x - m_vGap.x, y - m_vGap.y ) );
		}
	}
}

void CPostBoxWindowObject::PageOpen  ( CPostBoxPage* pPage )
{
	if ( m_pFrame )
	{
		m_pFrame->PageOpen ( this, pPage );
	}
}

void CPostBoxWindowObject::PageClose ()
{
	if ( m_pFrame )
	{
		m_pFrame->PageClose ();
	}
}

CBasicLineBox* CPostBoxWindowObject::CreateBackgroundControl ( char* szConatrolKeyword )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

CBasicTextBox* CPostBoxWindowObject::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CPostBoxWindowObject::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	CUIGroup::SetGlobalPos( vPos );
}

void CPostBoxWindowObject::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle( bVisible );
}

void CPostBoxWindowObject::WindowOpen ()
{
	SetVisibleSingle ( TRUE );

	if ( m_pFrame )
	{
		m_pFrame->OpenSubControl();
	}
}

void CPostBoxWindowObject::WindowClose ()
{
	SetVisibleSingle ( FALSE );

	if ( m_pFrame )
	{
		m_pFrame->CloseSubControl();
	}
}

const BOOL CPostBoxWindowObject::IsPageOpen ( CPostBoxPage* pPage )
{
	if ( m_pFrame )
	{
		return (m_pFrame->m_pPage == pPage);
	}
	
	return FALSE;
}

void CPostBoxWindowObject::SetResize ( const char* szControlKeyword )
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
}

void CPostBoxWindowObject::SetParentResize ( char* szControlKeyword )
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

CPostBoxPage* CPostBoxWindowObject::GetPage ()
{
	if ( !m_pFrame || !m_pFrame->m_pPage )
	{
		return NULL;
	}

	return m_pFrame->m_pPage;
}

BOOL CPostBoxWindowObject::IsParentExclusiveControl()
{
	CUIControl* pControl = GetParent();

	if ( pControl )
	{
		return pControl->IsExclusiveControl();
	}

	return FALSE;
}

void CPostBoxWindowObject::ResetParentExclusiveControl()
{
	CUIControl* pControl = GetParent();

	if ( pControl )
	{
		pControl->ResetExclusiveControl();
	}
}

void CPostBoxWindowObject::SetParentExclusiveControl()
{
	CUIControl* pControl = GetParent();

	if ( pControl )
	{
		pControl->SetExclusiveControl();
	}
}

void CPostBoxWindowObject::RequestPush ( const DWORD dwRequest )
{
	m_Request.push ( dwRequest );
}

const DWORD CPostBoxWindowObject::RequestPop ()
{
	if ( m_Request.empty() )
	{
		return CPostBoxWindow::POSTBOX_PAGE_REQ_NONE;
	}

	DWORD dwRequest = m_Request.front();
	m_Request.pop();

	return dwRequest;
}
