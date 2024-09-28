//#include "..\effecttool\directionedit.h"
// DirectionEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "sheetWithTab.h"
#include "DirectionEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DirectionEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DirectionEdit, CPropertyPage)
DirectionEdit::DirectionEdit()
	: CPropertyPage(DirectionEdit::IDD)
{
	m_dwOldPageIndex = 0;
	m_fScale = 1.0f;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

DirectionEdit::~DirectionEdit()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void DirectionEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_STATIC_POS, m_stDirection);
	DDX_Control(pDX, IDC_SCROLLBAR_Z, m_ZScroll);
	DDX_Control(pDX, IDC_SCROLLBAR_Y, m_YScroll);
	DDX_Control(pDX, IDC_SCROLLBAR_X, m_XScroll);
}


BEGIN_MESSAGE_MAP(DirectionEdit, CPropertyPage)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_EN_CHANGE(IDC_EDIT_X, &DirectionEdit::OnEnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_Y, &DirectionEdit::OnEnChangeEditY)
	ON_EN_CHANGE(IDC_EDIT_Z, &DirectionEdit::OnEnChangeEditZ)
END_MESSAGE_MAP()


// DirectionEdit �޽��� ó�����Դϴ�.

BOOL DirectionEdit::OnInitDialog()
{
	assert(m_pvDirection);

	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_stDirection.SubclassDlgItem ( IDC_STATIC_POS, this );

	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void DirectionEdit::UpdateContrl ()
{
	m_stDirection.m_vtCreatePos = *m_pvDirection;

	m_ToScale = 200.0f;
	m_ToScale *= m_fScale;

	m_XScroll.SetScrollRange ( 0, +800, TRUE );
	m_YScroll.SetScrollRange ( 0, +800, TRUE );
	m_ZScroll.SetScrollRange ( 0, +800, TRUE );

	m_XScroll.SetScrollPos ( int(400+m_pvDirection->x*m_ToScale), TRUE );
	m_YScroll.SetScrollPos ( int(400-m_pvDirection->y*m_ToScale), TRUE );
	m_ZScroll.SetScrollPos ( int(400-m_pvDirection->z*m_ToScale), TRUE );

	CString Str;
	Str.Format ( _T("%3.3f"), m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	//	Note : �� �ǵ����� �ֵ��� �ϱ� ����
	//
	m_fOldScale = m_fScale;
	m_pvOldDirection = *m_pvDirection;

	// Note : ���� �������� ����.
	NSDirectionRender::Update( *m_pvDirection );
}

void DirectionEdit::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nPos = uPos;

	// TODO: Add your message handler code here and/or call default
	switch ( nSBCode )
	{
	case SB_LINELEFT:
		nPos = m_XScroll.GetScrollPos();
		nPos -= 1;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_LINERIGHT:
		nPos = m_XScroll.GetScrollPos();
		nPos += 1;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_PAGELEFT:
		nPos = m_XScroll.GetScrollPos();
		nPos -= 20;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_PAGERIGHT:
		nPos = m_XScroll.GetScrollPos();
		nPos += 20;
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_THUMBPOSITION:
		m_pvDirection->x = (nPos-400) /m_ToScale;
		m_XScroll.SetScrollPos ( nPos, TRUE );
		break;

	case SB_THUMBTRACK:
		m_pvDirection->x = (nPos-400) /m_ToScale;
		break;
	};

	CString Str;
	Str.Format ( _T("%3.3f"), m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	// ���� �׸���.
	ReDraw();

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void DirectionEdit::OnVScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int nPos = uPos;

	// TODO: Add your message handler code here and/or call default
	if ( pScrollBar == GetDlgItem ( IDC_SCROLLBAR_Z ) )
	{
		switch ( nSBCode )
		{
		case SB_LINEUP:
			nPos = m_ZScroll.GetScrollPos();
			nPos -= 1;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_LINEDOWN:
			nPos = m_ZScroll.GetScrollPos();
			nPos += 1;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEUP:
			nPos = m_ZScroll.GetScrollPos();
			nPos -= 20;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEDOWN:
			nPos = m_ZScroll.GetScrollPos();
			nPos += 20;
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			m_ZScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_pvDirection->z = -(nPos-400) /m_ToScale;
			break;
		};
	}
	else if ( pScrollBar == GetDlgItem ( IDC_SCROLLBAR_Y ) )
	{
		switch ( nSBCode )
		{
		case SB_LINEUP:
			nPos = m_YScroll.GetScrollPos();
			nPos -= 1;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_LINEDOWN:
			nPos = m_YScroll.GetScrollPos();
			nPos += 1;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEUP:
			nPos = m_YScroll.GetScrollPos();
			nPos -= 20;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_PAGEDOWN:
			nPos = m_YScroll.GetScrollPos();
			nPos += 20;
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBPOSITION:
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			m_YScroll.SetScrollPos ( nPos, TRUE );
			break;

		case SB_THUMBTRACK:
			m_pvDirection->y = -(nPos-400) /m_ToScale;
			break;
		};
	}

	CString Str;
	Str.Format ( _T("%3.3f"), m_pvDirection->x );
	GetDlgItem(IDC_EDIT_X)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->y );
	GetDlgItem(IDC_EDIT_Y)->SetWindowText ( Str );

	Str.Format ( _T("%3.3f"), m_pvDirection->z );
	GetDlgItem(IDC_EDIT_Z)->SetWindowText ( Str );

	// ���� �׸���.
	ReDraw();

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void DirectionEdit::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	NSDirectionRender::SetActive( FALSE );

	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

void DirectionEdit::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	NSDirectionRender::SetActive( FALSE );

	m_fScale = m_fOldScale;
	*m_pvDirection = m_pvOldDirection;
	m_pSheetTab->SetActivePage ( m_dwOldPageIndex );
}

BOOL DirectionEdit::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	NSDirectionRender::SetActive( TRUE );

	return CPropertyPage::OnSetActive();
}

BOOL DirectionEdit::OnKillActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	NSDirectionRender::SetActive( FALSE );

	return CPropertyPage::OnKillActive();
}

void DirectionEdit::OnEnChangeEditX()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString Str;
	GetDlgItem(IDC_EDIT_X)->GetWindowText( Str );
	m_pvDirection->x = (float) _tstof( Str.GetString() );

	// ���� �׸���.
	ReDraw();
}

void DirectionEdit::OnEnChangeEditY()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString Str;
	GetDlgItem(IDC_EDIT_Y)->GetWindowText( Str );
	m_pvDirection->y = (float) _tstof( Str.GetString() );

	// ���� �׸���.
	ReDraw();
}

void DirectionEdit::OnEnChangeEditZ()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString Str;
	GetDlgItem(IDC_EDIT_Z)->GetWindowText( Str );
	m_pvDirection->z = (float) _tstof( Str.GetString() );

	// ���� �׸���.
	ReDraw();
}

void DirectionEdit::ReDraw()
{
	// m_stDirection �� �׸���.
	m_stDirection.m_vtCreatePos = *m_pvDirection;
	m_stDirection.RedrawWindow ();

	// Note : ���� �������� ����.
	NSDirectionRender::Update( *m_pvDirection );
}

#include "../EngineLib/DxTools/EditMeshs.h"

namespace NSDirectionRender
{
	BOOL		g_bActiveRender(FALSE);
	D3DXVECTOR3	g_vDir;

	void SetActive( BOOL bActive )
	{
		g_bActiveRender = bActive;
	}

	void Update( const D3DXVECTOR3& vDir )
	{
		g_vDir = vDir;
	}

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vLookat )
	{
		if( !g_bActiveRender )
			return;

		DWORD dwFogEnable;
		pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		{
			D3DXVECTOR3	vPos1 = vLookat + g_vDir * 10000.f;
			D3DXVECTOR3	vPos2 = vLookat - g_vDir * 10000.f;
			EDITMESHS::RENDERLINE( pd3dDevice, vPos1, vPos2, 0xffffffff );
		}
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
	}
};
