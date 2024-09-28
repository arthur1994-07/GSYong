// D:\Ran\LevelEdit\Edit\LevelCollision.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LevelEdit.h"

#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"
#include "./MobSetPage.h"

#include "../../enginelib/NaviMesh/Collision/SimpleCollision.h"
#include "../../enginelib/NaviMesh/Collision/LayerCollision.h"
#include "../../enginelib/DxTools/DxInputDevice.h"

#include "LevelCollision.h"


// CLevelCollision ��ȭ �����Դϴ�.

BOOL		CLevelCollision::g_bCheckViewNameCOLLISION = FALSE;

IMPLEMENT_DYNAMIC(CLevelCollision, CPropertyPage)

CLevelCollision::CLevelCollision()
	: CPropertyPage(CLevelCollision::IDD)
	, m_pSheetTab(NULL)
	, m_bLayer(FALSE)
{
	D3DXMatrixIdentity( &m_matWorld );
}

CLevelCollision::~CLevelCollision()
{
}

void CLevelCollision::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COLLISION, m_listCollision);
}


BEGIN_MESSAGE_MAP(CLevelCollision, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CLevelCollision::OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CLevelCollision::OnBnClickedButtonDel)
	ON_EN_SETFOCUS(IDC_EDIT_NAME, &CLevelCollision::OnEnSetfocusEditName)
	ON_BN_CLICKED(IDC_RADIO_PLANE, &CLevelCollision::OnBnClickedRadioPlane)
	ON_BN_CLICKED(IDC_RADIO_BOX, &CLevelCollision::OnBnClickedRadioBox)
	ON_BN_CLICKED(IDC_RADIO_CYLINDER, &CLevelCollision::OnBnClickedRadioCylinder)
	ON_EN_CHANGE(IDC_EDIT_MINUS_X, &CLevelCollision::OnEnChangeEditMinusX)
	ON_EN_CHANGE(IDC_EDIT_PLUS_X, &CLevelCollision::OnEnChangeEditPlusX)
	ON_EN_CHANGE(IDC_EDIT_MINUS_Z, &CLevelCollision::OnEnChangeEditMinusZ)
	ON_EN_CHANGE(IDC_EDIT_PLUS_Z, &CLevelCollision::OnEnChangeEditPlusZ)
	ON_EN_CHANGE(IDC_EDIT_ROTATION_Y, &CLevelCollision::OnEnChangeEditRotationY)
	ON_EN_CHANGE(IDC_EDIT_RADIUS, &CLevelCollision::OnEnChangeEditRadius)
	ON_LBN_SELCHANGE(IDC_LIST_COLLISION, &CLevelCollision::OnLbnSelchangeListCollision)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CLevelCollision::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_CHECK_VIEW_NAME, &CLevelCollision::OnBnClickedCheckViewName)
END_MESSAGE_MAP()


// CLevelCollision �޽��� ó�����Դϴ�.

void CLevelCollision::ActiveCollisionIndex()
{
	m_bLayer = FALSE;

	CWnd *pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->ShowWindow( FALSE );

	pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->SetWindowText( _T("") );

	UpdateContrl();
}

void CLevelCollision::ActiveCollisionLayerIndex()
{
	m_bLayer = TRUE;

	CWnd *pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->ShowWindow( TRUE );

	TSTRINGSTREAM strStringStream;
	strStringStream << "Layer : ";
	strStringStream << CMobSetPage::g_unLayer;
	pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->SetWindowText( strStringStream.str().c_str() );

	UpdateContrl();
}

UINT CLevelCollision::GetLayerIndex()
{
	if ( m_bLayer )
		return CMobSetPage::g_unLayer+1;
	else
		return 0;
}

void CLevelCollision::UpdateContrl ()
{
	ViewContrls ( FALSE );
	ListtingCollision ();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	pView->DeActiveEditMatrix();
}

void CLevelCollision::ViewContrls( BOOL bView )
{
	DWORD dwContrls[] = { IDC_EDIT_NAME, IDC_STATIC_TYPE, IDC_RADIO_PLANE, IDC_RADIO_BOX, IDC_RADIO_CYLINDER,
							IDC_STATIC_MINUS_X, IDC_STATIC_MINUS_Z, IDC_STATIC_PLUS_X, IDC_STATIC_PLUS_Z, IDC_STATIC_ROTATION_Y, IDC_STATIC_RADIUS,
							IDC_EDIT_MINUS_X, IDC_EDIT_MINUS_Z, IDC_EDIT_PLUS_X, IDC_EDIT_PLUS_Z, IDC_EDIT_ROTATION_Y, IDC_EDIT_RADIUS,
							IDC_BUTTON_APPLY };
	DWORD dwNumContrls = sizeof(dwContrls) / sizeof(DWORD);

	CWnd *pWnd;

	for ( DWORD i=0; i < dwNumContrls; i++ )
	{
		pWnd = GetDlgItem ( dwContrls[i] );
		pWnd->ShowWindow ( bView );
	}
}

void CLevelCollision::ListtingCollision ()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CString StrOldSelect;
	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )
	{
		m_listCollision.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_listCollision.ResetContent ();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if ( !pGLLandMan )		return;

	const LayerCollision* pLayerCollision = pGLLandMan->GetLayerSimpleCollisionList( GetLayerIndex() );
	if( pLayerCollision )
	{
		for ( DWORD i=0; i<pLayerCollision->m_vecSimpleCollision.size(); ++i )
		{
			int nIndex = m_listCollision.AddString ( pLayerCollision->m_vecSimpleCollision[i]->m_szName );
			m_listCollision.SetItemData( nIndex, reinterpret_cast<DWORD_PTR>( pLayerCollision->m_vecSimpleCollision[i].get() ) );
		}
	}

	m_listCollision.SelectString ( 0, StrOldSelect );
}

void CLevelCollision::SettingData_Dlg()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	CWnd* pWnd;
	GetDlgItem( IDC_EDIT_NAME )->SetWindowText( pCollision->GetName() );

	CButton* pButton = (CButton*)GetDlgItem( IDC_RADIO_PLANE );
	if ( pCollision->m_emType == EMSCT_PLANE )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_BOX );
	if ( pCollision->m_emType == EMSCT_BOX )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_CYLINDER );
	if ( pCollision->m_emType == EMSCT_CYLINDER )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	
	//Str.Format ( "%4.1f", pCollision->GetMinusX() );
	//GetDlgItem ( IDC_EDIT_MINUS_X ) -> SetWindowText ( Str );
	//Str.Format ( "%4.1f", pCollision->GetMinusZ() );
	//GetDlgItem ( IDC_EDIT_MINUS_Z ) -> SetWindowText ( Str );
	//Str.Format ( "%4.1f", pCollision->GetPlusX() );
	//GetDlgItem ( IDC_EDIT_PLUS_X ) -> SetWindowText ( Str );
	//Str.Format ( "%4.1f", pCollision->GetPlusZ() );
	//GetDlgItem ( IDC_EDIT_PLUS_Z ) -> SetWindowText ( Str );
	//Str.Format ( "%4.1f", pCollision->GetRotationY() );
	//GetDlgItem ( IDC_EDIT_ROTATION_Y ) -> SetWindowText ( Str );
	//Str.Format ( "%4.1f", pCollision->GetRadius() );
	//GetDlgItem ( IDC_EDIT_RADIUS ) -> SetWindowText ( Str );

	BOOL bView = pCollision->IsMinusX();
	pWnd = GetDlgItem ( IDC_STATIC_MINUS_X );
	pWnd->ShowWindow ( bView );
	pWnd = GetDlgItem ( IDC_EDIT_MINUS_X );
	pWnd->ShowWindow ( bView );
	if ( bView )
	{
		Str.Format ( "%4.1f", pCollision->GetMinusX() );
		GetDlgItem ( IDC_EDIT_MINUS_X ) -> SetWindowText ( Str );
	}

	bView = pCollision->IsMinusZ();
	pWnd = GetDlgItem ( IDC_STATIC_MINUS_Z );
	pWnd->ShowWindow ( bView );
	pWnd = GetDlgItem ( IDC_EDIT_MINUS_Z );
	pWnd->ShowWindow ( bView );
	if ( bView )
	{
		Str.Format ( "%4.1f", pCollision->GetMinusZ() );
		GetDlgItem ( IDC_EDIT_MINUS_Z ) -> SetWindowText ( Str );
	}

	bView = pCollision->IsPlusX();
	pWnd = GetDlgItem ( IDC_STATIC_PLUS_X );
	pWnd->ShowWindow ( bView );
	pWnd = GetDlgItem ( IDC_EDIT_PLUS_X );
	pWnd->ShowWindow ( bView );
	if ( bView )
	{
		Str.Format ( "%4.1f", pCollision->GetPlusX() );
		GetDlgItem ( IDC_EDIT_PLUS_X ) -> SetWindowText ( Str );
	}

	bView = pCollision->IsPlusZ();
	pWnd = GetDlgItem ( IDC_STATIC_PLUS_Z );
	pWnd->ShowWindow ( bView );
	pWnd = GetDlgItem ( IDC_EDIT_PLUS_Z );
	pWnd->ShowWindow ( bView );
	if ( bView )
	{
		Str.Format ( "%4.1f", pCollision->GetPlusZ() );
		GetDlgItem ( IDC_EDIT_PLUS_Z ) -> SetWindowText ( Str );
	}

	bView = pCollision->IsRotationY();
	pWnd = GetDlgItem ( IDC_STATIC_ROTATION_Y );
	pWnd->ShowWindow ( bView );
	pWnd = GetDlgItem ( IDC_EDIT_ROTATION_Y );
	pWnd->ShowWindow ( bView );
	if ( bView )
	{
		Str.Format ( "%4.1f", pCollision->GetRotationY() );
		GetDlgItem ( IDC_EDIT_ROTATION_Y ) -> SetWindowText ( Str );
	}

	bView = pCollision->IsRadius();
	pWnd = GetDlgItem ( IDC_STATIC_RADIUS );
	pWnd->ShowWindow ( bView );
	pWnd = GetDlgItem ( IDC_EDIT_RADIUS );
	pWnd->ShowWindow ( bView );
	if ( bView )
	{
		Str.Format ( "%4.1f", pCollision->GetRadius() );
		GetDlgItem ( IDC_EDIT_RADIUS ) -> SetWindowText ( Str );
	}
}

void CLevelCollision::OnBnClickedButtonNew()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	SimpleCollisionPlane *pNew = new SimpleCollisionPlane();

	char szNewName[SimpleCollisionBase::NAME_MAXSZ];

	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", pNew->m_szName, i++ );
	} while ( pGLLandMan->FindLayerSimpleCollision( GetLayerIndex(), szNewName ) );

	StringCchCopy( pNew->m_szName, SimpleCollisionBase::NAME_MAXSZ, szNewName );

	pNew->m_vPosition = pView->GetCollisionPos();

	m_matWorld._41 = pView->GetCollisionPos().x;
	m_matWorld._42 = pView->GetCollisionPos().y;
	m_matWorld._43 = pView->GetCollisionPos().z;

	//m_pLandEff = pNew;
	pView->ActiveEditMatrix2( &m_matWorld );
	pGLLandMan->AddLayerSimpleCollision( GetLayerIndex(), boost::shared_ptr<SimpleCollisionBase>(pNew) );
	//pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );

	ListtingCollision();
	m_listCollision.SelectString ( 0, pNew->m_szName );

	ViewContrls ( TRUE );
	SettingData_Dlg();

	CString Str;
	SimpleCollisionBox sTempBox;
	Str.Format ( "%4.1f", sTempBox.GetMinusZ() );
	GetDlgItem ( IDC_EDIT_MINUS_Z ) -> SetWindowText ( Str );
	Str.Format ( "%4.1f", sTempBox.GetPlusZ() );
	GetDlgItem ( IDC_EDIT_PLUS_Z ) -> SetWindowText ( Str );

	SimpleCollisionCylinder sTempCylinder;
	Str.Format ( "%4.1f", sTempCylinder.GetRadius() );
	GetDlgItem ( IDC_EDIT_RADIUS ) -> SetWindowText ( Str );	
}

void CLevelCollision::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	//PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );

	pGLLandMan->DelLayerSimpleCollision ( GetLayerIndex(), nIndex );
	//pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );

	ViewContrls ( FALSE );

	pView->DeActiveEditMatrix();

	//SettingData_Dlg();
	ListtingCollision();

	////	���� �������� ����Ʈ�ϰ��.
	//if ( pLandEff== m_pLandEff )
	//{
	//	m_pLandEff = NULL;
	//	ViewContrls ( FALSE );
	//	UpdateContrl ();
	//}
	//else
	//{
	//	ListtingLandEff ();
	//}
}

void CLevelCollision::OnEnSetfocusEditName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();		// ī�޶� �̵��� �ʵ��� �Ѵ�.
}


void CLevelCollision::OnBnClickedRadioPlane()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	SimpleCollisionPlane *pNew = new SimpleCollisionPlane();

	ChangeCollisionType( pGLLandMan, pCollision, pNew, nIndex );
}

void CLevelCollision::OnBnClickedRadioBox()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	SimpleCollisionBox *pNew = new SimpleCollisionBox();

	ChangeCollisionType( pGLLandMan, pCollision, pNew, nIndex );
}

void CLevelCollision::OnBnClickedRadioCylinder()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	SimpleCollisionCylinder *pNew = new SimpleCollisionCylinder();

	ChangeCollisionType( pGLLandMan, pCollision, pNew, nIndex );
}

void CLevelCollision::ChangeCollisionType( GLLandMan* pGLLandMan, const SimpleCollisionBase* pCollision, SimpleCollisionBase *pNew, int nIndex )
{
	CString Str;
	float fRotationY(0.f);
	GetDlgItem(IDC_EDIT_MINUS_X)->GetWindowText ( Str );
	pNew->SetMinusX( (float) atof ( Str.GetString() ) );
	GetDlgItem(IDC_EDIT_MINUS_Z)->GetWindowText ( Str );
	pNew->SetMinusZ( (float) atof ( Str.GetString() ) );
	GetDlgItem(IDC_EDIT_PLUS_X)->GetWindowText ( Str );
	pNew->SetPlusX( (float) atof ( Str.GetString() ) );
	GetDlgItem(IDC_EDIT_PLUS_Z)->GetWindowText ( Str );
	pNew->SetPlusZ( (float) atof ( Str.GetString() ) );
	GetDlgItem(IDC_EDIT_ROTATION_Y)->GetWindowText ( Str );
	pNew->SetRotationY( fRotationY = (float) atof ( Str.GetString() ) );
	GetDlgItem(IDC_EDIT_RADIUS)->GetWindowText ( Str );
	pNew->SetRadius( (float) atof ( Str.GetString() ) );

	TSTRING strName( pCollision->m_szName );
	std::string::size_type idxA(std::string::npos);
	std::string::size_type idxB(std::string::npos);
	switch ( pNew->m_emType )
	{
	case EMSCT_PLANE:
		idxA = strName.find( _T("BOX") );
		idxB = strName.find( _T("CYLINDER") );
		break;
	case EMSCT_BOX:
		idxA = strName.find( _T("PLANE") );
		idxB = strName.find( _T("CYLINDER") );
		break;
	case EMSCT_CYLINDER:
		idxA = strName.find( _T("PLANE") );
		idxB = strName.find( _T("BOX") );
		break;
	default:
		return;
	}

	if ( idxA != std::string::npos || idxB != std::string::npos )
	{
		// ������ ���ڰ� ������ �����Ѵ�.
		char szNewName[SimpleCollisionBase::NAME_MAXSZ];

		int i=0;
		do
		{
			sprintf ( szNewName, "%s[%d]", pNew->m_szName, i++ );
		} while ( pGLLandMan->FindLayerSimpleCollision( GetLayerIndex(), szNewName ) );

		StringCchCopy( pNew->m_szName, SimpleCollisionBase::NAME_MAXSZ, szNewName );
	}
	else
	{
		// ������ ���ڰ� ������ ���� ���� ������.
		StringCchCopy( pNew->m_szName, SimpleCollisionBase::NAME_MAXSZ, pCollision->m_szName );
	}

	pNew->SetPosisionRotation( pCollision->m_vPosition, fRotationY );

	pGLLandMan->ChangeLayerSimpleCollision( GetLayerIndex(), nIndex, boost::shared_ptr<SimpleCollisionBase>(pNew) );

	ListtingCollision();

	m_listCollision.SetCurSel ( nIndex );

	SettingData_Dlg();
}

void CLevelCollision::OnEnChangeEditMinusX()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	GetDlgItem(IDC_EDIT_MINUS_X)->GetWindowText ( Str );
	pCollision->SetMinusX( (float) atof ( Str.GetString() ) );
}

void CLevelCollision::OnEnChangeEditPlusX()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	GetDlgItem(IDC_EDIT_PLUS_X)->GetWindowText ( Str );
	pCollision->SetPlusX( (float) atof ( Str.GetString() ) );
}

void CLevelCollision::OnEnChangeEditMinusZ()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	GetDlgItem(IDC_EDIT_MINUS_Z)->GetWindowText ( Str );
	pCollision->SetMinusZ( (float) atof ( Str.GetString() ) );
}

void CLevelCollision::OnEnChangeEditPlusZ()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	GetDlgItem(IDC_EDIT_PLUS_Z)->GetWindowText ( Str );
	pCollision->SetPlusZ( (float) atof ( Str.GetString() ) );
}

void CLevelCollision::OnEnChangeEditRotationY()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	GetDlgItem(IDC_EDIT_ROTATION_Y)->GetWindowText ( Str );
	pCollision->SetRotationY( (float) atof ( Str.GetString() ) );
}

void CLevelCollision::OnEnChangeEditRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	CString Str;
	GetDlgItem(IDC_EDIT_RADIUS)->GetWindowText ( Str );
	pCollision->SetRadius( (float) atof ( Str.GetString() ) );
}

void CLevelCollision::OnLbnSelchangeListCollision()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ViewContrls ( TRUE );
	SettingData_Dlg();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	m_matWorld._41 = pCollision->m_vPosition.x;
	m_matWorld._42 = pCollision->m_vPosition.y;
	m_matWorld._43 = pCollision->m_vPosition.z;

	//m_pLandEff = pNew;
	pView->ActiveEditMatrix2( &m_matWorld );

	DxViewPort::GetInstance().CameraJump( pCollision->m_vPosition );
}

void CLevelCollision::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	int nIndex = m_listCollision.GetCurSel ();
	if ( nIndex==LB_ERR )	
		return;

	SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

	// �̸��� ����Ǵ��� Ȯ���Ѵ�.
	CString Str;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_NAME );
	pWnd->GetWindowText( Str );
	const SimpleCollisionBase* pFind = pGLLandMan->FindLayerSimpleCollision( GetLayerIndex(), Str.GetString() );
	if ( pFind == pCollision || pFind == NULL )
	{
		StringCchCopy( pCollision->m_szName, SimpleCollisionBase::NAME_MAXSZ, Str.GetString() );

		ViewContrls ( FALSE );

		pView->DeActiveEditMatrix();

		ListtingCollision();
		//m_listCollision.SetCurSel( -1 );
	}
	else
	{
		AfxMessageBox( _T("������ �̸��� �浹��ü�� �ֽ��ϴ�. �ٸ��̸����� ������ �ּ���.") );
	}
}

void CLevelCollision::OnBnClickedCheckViewName()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_VIEW_NAME );
	if ( pButton->GetCheck() )
	{
		g_bCheckViewNameCOLLISION = TRUE;
	}
	else
	{
		g_bCheckViewNameCOLLISION = FALSE;
	}
}

BOOL CLevelCollision::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_VIEW_NAME );
	pButton->SetCheck( g_bCheckViewNameCOLLISION );

	return CPropertyPage::OnSetActive();
}

BOOL CLevelCollision::OnKillActive()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();

	return CPropertyPage::OnKillActive();
}

void CLevelCollision::UpdateKeyBoardMouse()
{
	if( DxInputDevice::GetInstance().IsMouseInGameScreen( TRUE ) )
	{
		if ( !(DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_IDLE) )
		{
			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd();
			CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
			GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
			GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
			if (!pGLLandMan)
				return;

			int nIndex = m_listCollision.GetCurSel ();
			if ( nIndex==LB_ERR )	
				return;

			SimpleCollisionBase* pCollision = reinterpret_cast<SimpleCollisionBase*>( m_listCollision.GetItemData( nIndex ) );

			D3DXVECTOR3 vPosition( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
			pCollision->SetPosisionRotation( vPosition, pCollision->GetRotationY() );
		}
	}
}