// LandSkinObj.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "LandSkinObj.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxLand/DxLandSkinObj.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "State/WorldStateMachine.h"

#include "LandSkinObj.h"

extern BOOL g_bSKINOBJECT_RENDER_NAME;

// CLandSkinObj ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLandSkinObj, CPropertyPage)
CLandSkinObj::CLandSkinObj()
	: CPropertyPage(CLandSkinObj::IDD),
	m_bDlgInit(FALSE),
	m_pLandSkinObj(NULL)
{
}

CLandSkinObj::~CLandSkinObj()
{
}

void CLandSkinObj::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SKINOBJ, m_listSkinObj);
}


BEGIN_MESSAGE_MAP(CLandSkinObj, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_SKINOBJAPPLY, OnBnClickedButtonSkinobjapply)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_CHECK_RENDER_NAME, &CLandSkinObj::OnBnClickedCheckRenderName)
	ON_BN_CLICKED(IDC_BUTTON_CUR_POS1, &CLandSkinObj::OnBnClickedButtonCurPos1)
	ON_BN_CLICKED(IDC_BUTTON_CUR_POS2, &CLandSkinObj::OnBnClickedButtonCurPos2)
	ON_BN_CLICKED(IDC_BUTTON_RESET_FADE, &CLandSkinObj::OnBnClickedButtonResetFade)
	ON_EN_CHANGE(IDC_EDIT_VISIBLE_RADIUS, &CLandSkinObj::OnEnChangeEditVisibleRadius)
	ON_EN_CHANGE(IDC_EDIT_INVISIBLE_RADIUS, &CLandSkinObj::OnEnChangeEditInvisibleRadius)
END_MESSAGE_MAP()


// CLandSkinObj �޽��� ó�����Դϴ�.
BOOL CLandSkinObj::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CLandSkinObj::ListtingSkinObj ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();


	CString StrOldSelect;
	int nIndex = m_listSkinObj.GetCurSel ();
	if ( nIndex==LB_ERR )
	{
		m_listSkinObj.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_listSkinObj.ResetContent ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan ();
	PLANDSKINOBJ pCur = pSetLandMan->GetLandSkinObjList ();
	while ( pCur )
	{
		int nIndex = m_listSkinObj.AddString ( pCur->m_szName );
		m_listSkinObj.SetItemData ( nIndex, (DWORD)pCur );

		pCur = pCur->m_pNext;
	}

	m_listSkinObj.SelectString ( 0, StrOldSelect );
}

void CLandSkinObj::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	m_pLandSkinObj = NULL;

	ViewContrls ( FALSE );
	ListtingSkinObj ();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	pView->DeActiveEditMatrix();
}

void CLandSkinObj::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )	return;

	DWORD dwContrls[] = { IDC_EDIT_NAME, IDC_EDIT_FINDSTR, IDC_BUTTON_SEARCH, IDC_BUTTON_ADD, IDC_BUTTON_SKINOBJAPPLY,
							IDC_STATIC_FADE, IDC_BUTTON_RESET_FADE,
							IDC_STATIC_VISIBLE_RADIUS, IDC_EDIT_VISIBLE_RADIUS, IDC_BUTTON_CUR_POS1,
							IDC_STATIC_INVISIBLE_RADIUS, IDC_EDIT_INVISIBLE_RADIUS, IDC_BUTTON_CUR_POS2 };
	DWORD dwNumContrls = sizeof(dwContrls) / sizeof(DWORD);

	CString Str;
	CWnd *pWnd;
	
	for ( DWORD i=0; i < dwNumContrls; i++ )
	{
		pWnd = GetDlgItem ( dwContrls[i] );
		pWnd->ShowWindow ( bView );
	}

	if ( bClear )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( "" );

		Str.Format( _T("%3.2f"), m_pLandSkinObj->m_fVisibleRadius );
		GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( _T("") );

		Str.Format( _T("%3.2f"), m_pLandSkinObj->m_fInvisibleRadius );
		GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( _T("") );
	}
	else if ( m_pLandSkinObj )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( m_pLandSkinObj->m_szName );

		pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( m_pLandSkinObj->m_szFileName );

		Str.Format( _T("%3.2f"), m_pLandSkinObj->m_fVisibleRadius );
		GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

		Str.Format( _T("%3.2f"), m_pLandSkinObj->m_fInvisibleRadius );
		GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
	}
}

void CLandSkinObj::OnBnClickedButtonNew ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();

	DXLANDSKINOBJ *pLandSkinObj;
	pLandSkinObj = new DXLANDSKINOBJ( pView->GetD3DDevice() );

	char szNewName[DXLANDSKINOBJ::LANDSKINOBJ_MAXSZ];
	
	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", pLandSkinObj->m_szName, i++ );
	} while ( pView->GetSetLandMan()->FindLandSkinObj(szNewName) );

	strcpy ( pLandSkinObj->m_szName, szNewName );

	pLandSkinObj->SetPosition( pView->GetCollisionPos() );

	m_pLandSkinObj = pLandSkinObj;
	pView->ActiveEditMatrix2( &m_pLandSkinObj->m_matWorld );
	pSetLandMan->AddLandSkinObj ( m_pLandSkinObj );
	pSetLandMan->BuildSkinObjTree ();

    WorldStateMachine::EditAsset(m_pLandSkinObj->m_szName);

	m_listSkinObj.SelectString ( 0, m_pLandSkinObj->m_szName );

	ViewContrls ( TRUE );
	ListtingSkinObj ();
}

void CLandSkinObj::OnBnClickedButtonEdit ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	int nIndex = m_listSkinObj.GetCurSel ();
	if ( nIndex==LB_ERR )	return;
	
	m_pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( nIndex );
	pView->ActiveEditMatrix2( &m_pLandSkinObj->m_matWorld );

	ViewContrls ( TRUE );

	D3DXVECTOR3 vPos( m_pLandSkinObj->m_matWorld._41, m_pLandSkinObj->m_matWorld._42, m_pLandSkinObj->m_matWorld._43 );
	DxViewPort::GetInstance().CameraJump( vPos );
}

void CLandSkinObj::OnBnClickedButtonDel ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();

	int nIndex = m_listSkinObj.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	DXLANDSKINOBJ* pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( nIndex );

    WorldStateMachine::EditAsset(pLandSkinObj->m_szName);

	pSetLandMan->DelLandSkinObj ( pLandSkinObj );
	pSetLandMan->BuildSkinObjTree ();

	//	���� �������� ����Ʈ�ϰ��.
	if ( pLandSkinObj== m_pLandSkinObj )
	{
		m_pLandSkinObj = NULL;
		ViewContrls ( FALSE );
		UpdateContrl ();
	}
	else
	{
		ListtingSkinObj ();
	}
}

void CLandSkinObj::OnBnClickedButtonSearch ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//	Note : ���� ���� ���̾˷α׸� ����.
	CString szFilter = "Skin Object ( *.chf ) |*.chf";
	CFileDialog dlg ( TRUE, ".chf", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( WriteName );
	}
}

void CLandSkinObj::OnBnClickedButtonSkinobjapply ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();

	CWnd *pWnd;
	char szName[DXLANDSKINOBJ::LANDSKINOBJ_MAXSZ] = {0};
	char szFileName[MAX_PATH] = {0};

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( szName, DXLANDSKINOBJ::LANDSKINOBJ_MAXSZ );

	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->GetWindowText ( szFileName, MAX_PATH );

	BOOL bExist = FALSE;
	DXLANDSKINOBJ* pCur = pSetLandMan->GetLandSkinObjList ();
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) && pCur!=m_pLandSkinObj )
		{
			MessageBox ( _T("Already exist same name object. can't register!"), _T("Warning") );
			bExist = TRUE;
			return;
		}

		pCur = pCur->m_pNext;
	}

	if ( szFileName[0] == NULL )
	{
		MessageBox ( "����Ʈ ������ �������� �ʾҽ��ϴ�.", "����" );
		return;
	}

	CString Str;
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->GetWindowText( Str );
	m_pLandSkinObj->m_fVisibleRadius = (float)_tstof( Str.GetString() );

	GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->GetWindowText( Str );
	m_pLandSkinObj->m_fInvisibleRadius = (float)_tstof( Str.GetString() );

	HRESULT hr;
	hr = m_pLandSkinObj->SetSkinChar ( szName, m_pLandSkinObj->m_matWorld, szFileName, pView->GetD3DDevice() );
	if ( FAILED(hr) )
	{
		MessageBox ( "������ ������ ��ü�� �ʱ�ȭ�Ͽ����� �����Ͽ����ϴ�.", "����" );
	}

	pSetLandMan->BuildSkinObjTree ();


	m_listSkinObj.SelectString ( 0, m_pLandSkinObj->m_szName );
	ListtingSkinObj ();
}

void CLandSkinObj::OnBnClickedButtonAdd ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	DxSetLandMan *pSetLandMan = pView->GetSetLandMan();

	CWnd *pWnd;
	char szName[DXLANDSKINOBJ::LANDSKINOBJ_MAXSZ] = {0};
	char szFileName[MAX_PATH] = {0};

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( szName, DXLANDSKINOBJ::LANDSKINOBJ_MAXSZ );

	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->GetWindowText ( szFileName, MAX_PATH );

	BOOL bExist = FALSE;
	DXLANDSKINOBJ* pCur = pSetLandMan->GetLandSkinObjList ();
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) && pCur!=m_pLandSkinObj )
		{
			MessageBox ( _T("Already exist same name object. can't register!"), _T("����") );
			bExist = TRUE;
			return;
		}

		pCur = pCur->m_pNext;
	}

	if ( szFileName[0] == NULL )
	{
		MessageBox ( "����Ʈ ������ �������� �ʾҽ��ϴ�.", "����" );
		return;
	}

	HRESULT hr;
	hr = m_pLandSkinObj->SetSkinChar ( szName, m_pLandSkinObj->m_matWorld, szFileName, pView->GetD3DDevice() );
	if ( FAILED(hr) )
	{
		MessageBox ( "������ ������ ��ü�� �ʱ�ȭ�Ͽ����� �����Ͽ����ϴ�.", "����" );
	}

	pSetLandMan->BuildSkinObjTree ();

	m_listSkinObj.SelectString ( 0, m_pLandSkinObj->m_szName );
	ListtingSkinObj ();

	m_pLandSkinObj = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();
}

BOOL CLandSkinObj::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_RENDER_NAME );
	pButton->SetCheck( FALSE );
	g_bSKINOBJECT_RENDER_NAME = FALSE;

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_EDIT_VISIBLE_RADIUS );
	pWnd->SetWindowText( _T("0.f") );
	pWnd = GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS );
	pWnd->SetWindowText( _T("0.f") );

	return CPropertyPage::OnSetActive();
}

BOOL CLandSkinObj::OnKillActive()
{
	m_pLandSkinObj = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();

	g_bSKINOBJECT_RENDER_NAME = FALSE;

	return CPropertyPage::OnKillActive();
}

void CLandSkinObj::OnBnClickedCheckRenderName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_RENDER_NAME );
	if ( pButton->GetCheck() )
	{
		g_bSKINOBJECT_RENDER_NAME = TRUE;
	}
	else
	{
		g_bSKINOBJECT_RENDER_NAME = FALSE;
	}
}


/*
DXLIGHT* pLightCur;
for( int i=0; i<m_ListLight.GetCount(); ++i)
{
if( i==0 || i==1 )
{
if( bDirc )
{
// Note : Direction���� 2�� �ɷȴ�.
if( m_ListLight.GetSel(i) > 0 )
{
MessageBox( "Direction�� �ߺ� ���õǾ����ϴ�." );
ListingLights();
return;
}
}

// Note : Direction 1�� �ɷȴ�.
if( m_ListLight.GetSel(i) > 0 )	bDirc = TRUE;
continue;
}

pLightCur = (DXLIGHT*)m_ListLight.GetItemData(i);
if( m_ListLight.GetSel(i) > 0 )	pLightCur->m_bEditCheck = TRUE;
else							pLightCur->m_bEditCheck = FALSE;

// Note : �ɷ��ִ�.
if( pLightCur->m_bEditCheck && bDirc )
{
// Note : Direction�� 1�� ���õ� �� �� �ɸ��ٸ� ���� �ڵ� �� Dialog �ʱ�ȭ �� Return;
MessageBox( "Direction�� ���ÿ� Point�� ���õǾ����ϴ�." );
ListingLights();
return;
}
}
*/


void CLandSkinObj::OnBnClickedButtonCurPos1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();

	//INT nCount = m_listSkinObj.GetSelCount();
	//if( nCount > 1 )
	//{
	//	AfxMessageBox( _T("���߼����� �Ұ����մϴ�.") );
	//	return;
	//}

	//int nIndex = m_listSkinObj.GetCurSel ();
	//if ( nIndex==LB_ERR )	return;

	//m_pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( nIndex );
	//if ( !m_pLandSkinObj )
	//	return;

	D3DXVECTOR3 LookAtPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vDir = m_pLandSkinObj->m_vCenter - LookAtPt; // ��ü�� ���������� ����� �ش�.
	float fLengthLookat = D3DXVec3Length( &vDir ); // ī�޶� ������ �Ÿ�
	fLengthLookat -= m_pLandSkinObj->m_fRadius; // ��ü�� ���������� ����� �ش�.

	CString Str;
	Str.Format( _T("%3.2f"), fLengthLookat );
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

	/*m_pLandSkinObj->m_fVisibleRadius = fLengthLookat;

	CString Str;
	Str.Format( _T("%3.2f"), m_pLandSkinObj->m_fVisibleRadius );
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );*/
}

void CLandSkinObj::OnBnClickedButtonCurPos2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	//
	//INT nCount = m_listSkinObj.GetSelCount();
	//if( nCount > 1 )
	//{
	//	AfxMessageBox( _T("���߼����� �Ұ����մϴ�.") );
	//	return;
	//}

	//int nIndex = m_listSkinObj.GetCurSel ();
	//if ( nIndex==LB_ERR )	return;

	//m_pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( nIndex );
	//if ( !m_pLandSkinObj )
	//	return;

	D3DXVECTOR3 LookAtPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vDir = m_pLandSkinObj->m_vCenter - LookAtPt; // ��ü�� ���������� ����� �ش�.
	float fLengthLookat = D3DXVec3Length( &vDir ); // ī�޶� ������ �Ÿ�
	fLengthLookat -= m_pLandSkinObj->m_fRadius; // ��ü�� ���������� ����� �ش�.

	CString Str;
	Str.Format( _T("%3.2f"), fLengthLookat );
	GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );

	//m_pLandSkinObj->m_fInvisibleRadius = fLengthLookat;

	//CString Str;
	//Str.Format( _T("%3.2f"), m_pLandSkinObj->m_fInvisibleRadius );
	//GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
}

void CLandSkinObj::OnBnClickedButtonResetFade()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();
	
	//INT nCount = m_listSkinObj.GetSelCount();
	//if( nCount == 0 )
	//{
	//	MessageBox( _T("Please. Check Point") );
	//	return;
	//}

	//for( int i=0; i<m_listSkinObj.GetCount(); ++i)
	//{
	//	if( m_listSkinObj.GetSel(i) > 0 )
	//	{
	//		DXLANDSKINOBJ* pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( i );
	//		if ( pLandSkinObj )
	//		{
	//			pLandSkinObj->m_fVisibleRadius = DEFAULT_VISIBLE_RADIUS;
	//			pLandSkinObj->m_fInvisibleRadius = DEFAULT_INVISIBLE_RADIUS;
	//		}
	//	}
	//}

	CString Str;

	Str.Format( _T("%3.2f"), DEFAULT_VISIBLE_RADIUS );
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

	Str.Format( _T("%3.2f"), DEFAULT_INVISIBLE_RADIUS );
	GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
}

void CLandSkinObj::OnEnChangeEditVisibleRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	//// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();

	//CString Str;
	//GetDlgItem(IDC_EDIT_VISIBLE_RADIUS)->GetWindowText( Str );

	//for( int i=0; i<m_listSkinObj.GetCount(); ++i)
	//{
	//	if( m_listSkinObj.GetSel(i) > 0 )
	//	{
	//		DXLANDSKINOBJ* pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( i );
	//		if ( pLandSkinObj )
	//		{
	//			pLandSkinObj->m_fVisibleRadius = (float)_tstof( Str.GetString() );
	//		}
	//	}
	//}
}

void CLandSkinObj::OnEnChangeEditInvisibleRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	//// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pMainFrame->GetActiveView ();

	//CString Str;
	//GetDlgItem(IDC_EDIT_INVISIBLE_RADIUS)->GetWindowText( Str );

	//for( int i=0; i<m_listSkinObj.GetCount(); ++i)
	//{
	//	if( m_listSkinObj.GetSel(i) > 0 )
	//	{
	//		DXLANDSKINOBJ* pLandSkinObj = (DXLANDSKINOBJ*) m_listSkinObj.GetItemData ( i );
	//		if ( pLandSkinObj )
	//		{
	//			pLandSkinObj->m_fInvisibleRadius = (float)_tstof( Str.GetString() );
	//		}
	//	}
	//}
}
