// TileEffEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include <string>

#include "SheetWithTab.h"
#include "../SigmaCore/Util/Util.h"
#include "../EngineLib/Common/SUBPATH.H"
#include "../EngineLib/DxEffect/TexEff/DxTextureEffMan.h"
#include "../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/DxMethods.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceEffMan.h"
#include "../EngineLib/DxLand/DxPiece/DxPieceDef.h"
#include "../SigmaCore/util/util.h"

#include "./TextureSetDlg.h"

#include "StaticMeshCreate.h"

extern BOOL g_bPLAY_ANIMATION;

// CTileEffEdit ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CStaticMeshCreate, CPropertyPage)
CStaticMeshCreate::CStaticMeshCreate()
	: CPropertyPage(CStaticMeshCreate::IDD)
{
}

CStaticMeshCreate::~CStaticMeshCreate()
{
}

void CStaticMeshCreate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FRAME, m_TreeFrame);
	DDX_Control(pDX, IDC_LIST_EFF, m_listEff);
	DDX_Control(pDX, IDC_COMBO_EFF, m_comboEff);
}


BEGIN_MESSAGE_MAP(CStaticMeshCreate, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_XFILE_LOAD, OnBnClickedButtonXfileLoad)
	ON_BN_CLICKED(IDC_BUTTON_EFF_STATE, OnBnClickedButtonEffState)
	ON_BN_CLICKED(IDC_BUTTON_EFF_DELETE, OnBnClickedButtonEffDelete)
	ON_BN_CLICKED(IDC_BUTTON_EFF_INSERT, OnBnClickedButtonEffInsert)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_EFF, OnBnClickedButtonSingleEff)
	ON_BN_CLICKED(IDC_BUTTON_FRAME_RENDER, OnBnClickedButtonFrameRender)
	ON_BN_CLICKED(IDC_BUTTON_STATIC_RENDER, OnBnClickedButtonStaticRender)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_EFF, OnBnClickedButtonTextureEff)
	ON_BN_CLICKED(IDC_BUTTON_PEF_LOAD, OnBnClickedButtonPefLoad)
	ON_BN_CLICKED(IDC_BUTTON_PEF_SAVE, OnBnClickedButtonPefSave)
	ON_BN_CLICKED(IDC_BUTTON_PIE_LOAD, OnBnClickedButtonPieLoad)
	ON_BN_CLICKED(IDC_BUTTON_PIE_SAVE, OnBnClickedButtonPieSave)
	ON_BN_CLICKED(IDC_BUTTON_MORPH, OnBnClickedButtonMorph)
    ON_BN_CLICKED(IDC_CHECK_ALPHA_BLEND, &CStaticMeshCreate::OnBnClickedCheckAlphaBlend)
	ON_BN_CLICKED(IDC_CHECK_ANI_PLAY_ONCE, &CStaticMeshCreate::OnBnClickedCheckAniPlayOnce)
	ON_BN_CLICKED(IDC_CHECK_UNBAKED_LIGHTMAP, &CStaticMeshCreate::OnBnClickedCheckUnbakedLightmap)
	ON_BN_CLICKED(IDC_BUTTON_MESH_STATE, &CStaticMeshCreate::OnBnClickedButtonMeshState)
	ON_BN_CLICKED(IDC_CHECK_RAN_1_0, &CStaticMeshCreate::OnBnClickedCheckRan10)
	ON_BN_CLICKED(IDC_BUTTON_RESET_RADIUS, &CStaticMeshCreate::OnBnClickedButtonResetRadius)
	ON_EN_CHANGE(IDC_EDIT_VISIBLE_RADIUS, &CStaticMeshCreate::OnEnChangeEditVisibleRadius)
	ON_EN_CHANGE(IDC_EDIT_INVISIBLE_RADIUS, &CStaticMeshCreate::OnEnChangeEditInvisibleRadius)
	ON_BN_CLICKED(IDC_BUTTON_MATERIAL_SETTING, &CStaticMeshCreate::OnBnClickedButtonMaterialSetting)
END_MESSAGE_MAP()


// CStaticMeshCreate �޽��� ó�����Դϴ�.
BOOL CStaticMeshCreate::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = FRAMEMESH_VIEW;

	// Note : �ִϸ��̼��� Ȱ��ȭ ��Ų��.
	g_bPLAY_ANIMATION = TRUE;

	//	Note : �÷� ��ư ����
	//
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrFrame.SubclassDlgItem ( IDC_BUTTON_FRAME_RENDER, this );
	cClrStatic.SubclassDlgItem ( IDC_BUTTON_STATIC_RENDER, this );

	cClrFrame.SetColour ( cColorTEXT, cColorGRAY );
	cClrStatic.SetColour ( cColorTEXT, cColorGRAY );

	GetDlgItem( IDC_BUTTON_PEF_SAVE )->EnableWindow( FALSE );
	GetDlgItem( IDC_BUTTON_PEF_LOAD )->EnableWindow( FALSE );
	GetDlgItem( IDC_BUTTON_PIE_SAVE )->EnableWindow( FALSE );
	GetDlgItem( IDC_BUTTON_PIE_LOAD )->EnableWindow( FALSE );

	if ( pView->m_ViewType == FRAMEMESH_VIEW )
	{
		cClrFrame.SetColour ( cColorTEXT, cColorBLUE );

		GetDlgItem( IDC_BUTTON_PEF_SAVE )->EnableWindow( TRUE );
		GetDlgItem( IDC_BUTTON_PEF_LOAD )->EnableWindow( TRUE );
	}
	if ( pView->m_ViewType == STATICMESH_VIEW )	
	{
		cClrStatic.SetColour ( cColorTEXT, cColorBLUE );

		GetDlgItem( IDC_BUTTON_PIE_SAVE )->EnableWindow( TRUE );
		GetDlgItem( IDC_BUTTON_PIE_LOAD )->EnableWindow( TRUE );
	}

	ResetButton ();		// Button �缳��

	//	Note : ȿ�� Ÿ�Ե��� ���.
	DxEffectType *pCurEffectType = DxPieceEffMan::GetInstance().GetEffectList();
	while ( pCurEffectType )
	{
		int nIndex = m_comboEff.AddString ( pCurEffectType->NAME );
		m_comboEff.SetItemData ( nIndex, pCurEffectType->TYPEID );

		pCurEffectType = pCurEffectType->pNextType;
	}

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	DxViewPort::GetInstance().CameraJump( vPos );


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CStaticMeshCreate::OnSetActive()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = FRAMEMESH_VIEW;

	// Note : �ִϸ��̼��� Ȱ��ȭ ��Ų��.
	g_bPLAY_ANIMATION = TRUE;

	ResetButton ();		// Button �缳��

	OnBnClickedCheckRan10();	// Button ����.

	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	DxViewPort::GetInstance().CameraJump( vPos );

	return CPropertyPage::OnSetActive();
}

void CStaticMeshCreate::UpdatePage ()
{
}

void CStaticMeshCreate::OnBnClickedButtonXfileLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); // AfxGetApp()->m_pMainWnd;

	HRESULT hr = S_OK;
	CWnd *pWnd;

	//	Note : ���� ���͸� �����Ѵ�.
	CString szFilter = "Frame Mesh xFile |*.X|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(
        TRUE,
        ".x",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        szFilter,
		(CDlgEdit*)this);

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePieceEdit().c_str()); //SUBPATH::OBJ_FILE_PIECEEDIT);

	dlg.m_ofn.lpstrInitialDir = szPath;
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxPieceEdit* pPieceEdit = pView->GetPieceEdit();

		//	Note : ���� ������ ����.
		pPieceEdit->CleanUp();

		//	Note : ���ο� FrameMesh ����
		pPieceEdit->NewFrameMesh(pView->GetD3DDevice(), sFileName.GetString(), szPath);

		//	Note : ������ Ʈ�� ���.
		//
		SetTreeFrame( pPieceEdit->GetFrameRoot() );
		
		CWnd *pWnd = GetDlgItem ( IDC_EDIT_XFILE );
		pWnd->SetWindowText( sFileName.GetString() );

		pWnd = GetDlgItem ( IDC_EDIT_PEF_NAME );
		pWnd->SetWindowText ( "" );

        CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_ALPHA_BLEND );
		pButton->SetCheck( pPieceEdit->m_dwFlags & EMPO_PICK_ALPHA );

		pButton = (CButton*)GetDlgItem( IDC_CHECK_ANI_PLAY_ONCE );
		pButton->SetCheck( pPieceEdit->m_dwFlags & EMPO_ANI_PLAY_ONCE );

		pButton = (CButton*)GetDlgItem( IDC_CHECK_UNBAKED_LIGHTMAP );
		pButton->SetCheck( pPieceEdit->m_dwFlags & EMPO_UNSUPPORT_RAD );

		CString Str;
		Str.Format( _T("%3.2f"), pPieceEdit->m_fVisibleRadius );
		GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

		Str.Format( _T("%3.2f"), pPieceEdit->m_fInvisibleRadius );
		GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );

		ResetButton ();		// Button �缳��
	}
	return;

//e_ERROR:
	pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->SetWindowText ( "" );

	m_TreeFrame.DeleteAllItems();

	MessageBox ( "xFile Load Failed!" );
}

void CStaticMeshCreate::OnBnClickedButtonExport()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		MessageBox( "Please.. X-File Load" );
		return;
	}

	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_RAN_1_0 );
	pPieceEdit->Export( pView->GetD3DDevice(), static_cast<BOOL>( pButton->GetCheck() ) );

	ResetButton ();		// Button �缳��
	OnBnClickedButtonStaticRender();	// Static Render�� �⺻���� �ٲ۴�.
}

void CStaticMeshCreate::ResetButton ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CWnd* pWnd;
	if ( pView->GetPieceEdit()->GetFrameMesh() )
	{
		pWnd = GetDlgItem ( IDC_BUTTON_EXPORT );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_BUTTON_EXPORT );
		pWnd->EnableWindow ( FALSE );
	}
}

void CStaticMeshCreate::OnBnClickedButtonEffState()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//	Note : ȿ�� ���� ������ �̵�.
	int nIndex = m_listEff.GetCurSel ();
	if ( nIndex == -1 ) return;
	DxEffectBase *pEffect = (DxEffectBase*) m_listEff.GetItemData ( nIndex );

	SetActiveEffectPage( pEffect->GetTypeID(), pEffect );
}

void CStaticMeshCreate::OnBnClickedButtonEffDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//	Note : ���õ� ������.
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		return;
	}

	DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
	if( !pFrameMesh )
	{
		return;
	}

	DxFrame *pDxFrame = pFrameMesh->FindFrame( sSelectedFrame.GetString() );
	
	if ( !pDxFrame )	return;

	//	Note : ���õ� ����Ʈ.
	int nIndex = m_listEff.GetCurSel();
	if ( nIndex == -1 ) return;
	DxEffectBase *pEffect = (DxEffectBase*)m_listEff.GetItemData( nIndex );

	pFrameMesh->DelEffect( pDxFrame, pEffect );

	//	Note : ȿ���� ������.
	ListEffects( pDxFrame );
}

void CStaticMeshCreate::OnBnClickedButtonEffInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//	Note : ���õ� ȿ�� TYPE.
	DWORD nIndex = m_comboEff.GetCurSel ();
	if ( nIndex == -1 ) return;
	DWORD dwEffectType = m_comboEff.GetItemData( nIndex );

	//	Note : ���õ� ������.
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		return;
	}

	DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
	if( !pFrameMesh )
	{
		return;
	}

	DxFrame *pDxFrame = pFrameMesh->FindFrame( sSelectedFrame.GetString() );	
	if ( !pDxFrame )	return;

	DxEffectBase* pEffectBase = DxPieceEffMan::GetInstance().AdaptToDxFrame( dwEffectType, pFrameMesh, pDxFrame, pView->GetD3DDevice() );

	//	Note : ȿ���� ������.
	ListEffects ( pDxFrame );

	if ( !pEffectBase )
	{
		MessageBox ( "ȿ�� �ν���Ʈ ������ �����Ͽ����ϴ�.", "����", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	//	Note : ȿ�� ���� ������ �̵�.
	SetActiveEffectPage ( dwEffectType, pEffectBase );
}

//	Note : ������ �߰� �Ǵ� ����Ʈ ����Ʈ Page ���.
void CStaticMeshCreate::SetActiveEffectPage( DWORD TypeID, DxEffectBase* pEffectBase )
{
	if ( TypeID==DxEffectClothForMaterial::TYPEID )
		m_pSheetTab->ActiveEffectClothForMaterialPage( (DxEffectClothForMaterial*) pEffectBase );
	else
	{
		MessageBox ( "ȿ�� ���� �������̽��� �غ�Ǿ� ���� �ʽ��ϴ�.", "����", MB_OK|MB_ICONINFORMATION );
	}
}

void CStaticMeshCreate::SetTreeFrame ( DxFrame *pFrameRoot )
{
	m_TreeFrame.DeleteAllItems();

	HTREEITEM hParent = TVI_ROOT;

	if ( pFrameRoot )
		AddTreeFrameItem ( pFrameRoot, hParent );
}

void CStaticMeshCreate::AddTreeFrameItem ( DxFrame *pframeCur, HTREEITEM hParent )
{
	//	Note : Frame �� �̸��� �����ϴ� ��常�� ����Ѵ�.
	//
	HTREEITEM htreeitemCur;
	if ( pframeCur->szName )
	{
		htreeitemCur = m_TreeFrame.InsertItem ( pframeCur->szName, hParent );
		m_TreeFrame.SetItemData ( htreeitemCur, (DWORD)pframeCur );

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		//
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			AddTreeFrameItem ( pframeChild, htreeitemCur );
			pframeChild = pframeChild->pframeSibling;
		}
	}
}

void CStaticMeshCreate::OnBnClickedButtonFrameRender()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = FRAMEMESH_VIEW;

	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrFrame.SetColour ( cColorTEXT, cColorBLUE );
	cClrStatic.SetColour ( cColorTEXT, cColorGRAY );

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_BUTTON_PEF_SAVE );
	pWnd->EnableWindow( TRUE );
	pWnd = GetDlgItem( IDC_BUTTON_PEF_LOAD );
	pWnd->EnableWindow( TRUE );

	pWnd = GetDlgItem( IDC_BUTTON_PIE_SAVE );
	pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem( IDC_BUTTON_PIE_LOAD );
	pWnd->EnableWindow( FALSE );
}

void CStaticMeshCreate::OnBnClickedButtonStaticRender()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
	pView->m_ViewType = STATICMESH_VIEW;

	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrFrame.SetColour ( cColorTEXT, cColorGRAY );
	cClrStatic.SetColour ( cColorTEXT, cColorBLUE );

	CWnd* pWnd;
	pWnd = GetDlgItem( IDC_BUTTON_PIE_SAVE );
	pWnd->EnableWindow( TRUE );
	pWnd = GetDlgItem( IDC_BUTTON_PIE_LOAD );
	pWnd->EnableWindow( TRUE );

	pWnd = GetDlgItem( IDC_BUTTON_PEF_SAVE );
	pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem( IDC_BUTTON_PEF_LOAD );
	pWnd->EnableWindow( FALSE );
}

void CStaticMeshCreate::SelectChangedTreeFrame()
{
	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( pPieceEdit )
	{
		DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
		if( pFrameMesh )
		{
			DxFrame *pDxFrame = pFrameMesh->FindFrame( sSelectedFrame.GetString() );

			//	Note : ȿ���� ������.
			ListEffects ( pDxFrame );
		}
	}
}

void CStaticMeshCreate::OnBnClickedButtonSingleEff()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		MessageBox( "Please.. X-File Load" );
		return;
	}

	DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
	if( !pFrameMesh )
	{
		MessageBox( "pFrameMesh Point NULL, X-File Load" );
		return;
	}

	m_pSheetTab->SetActivePageEX( PIECESINGLEEFFINDEX, STATICMESHCREATEINDEX );
}

void CStaticMeshCreate::OnBnClickedButtonTextureEff()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		MessageBox( "Please.. X-File Load" );
		return;
	}

	DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
	if( !pFrameMesh )
	{
		MessageBox( "pFrameMesh Point NULL, X-File Load" );
		return;
	}

	DxTextureEffMan* pTextureEff = pFrameMesh->GetTextureEffMan();
	if( !pTextureEff )
	{
		MessageBox( "DxTextureEffMan Pointer NULL" );
		return;
	}

	if( !pTextureEff->GetTextureListNUM() )
	{
		MessageBox( "Texture 0 Num. X File�� �ؽ��İ� �ϳ��� ���׿�." );
		return;
	}

	m_pSheetTab->SetActivePageEX( PIECETEXEFFINDEX, STATICMESHCREATEINDEX );
}

void CStaticMeshCreate::OnBnClickedButtonMorph()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_pSheetTab->SetActivePageEX( PIECEMORPHINDEX, STATICMESHCREATEINDEX );
}

void CStaticMeshCreate::ListEffects( DxFrame *pDxFrame )
{
	if ( !pDxFrame )	return;

	m_listEff.ResetContent ();

	//	Note : ��, ����Ʈ
	DxEffectBase* pEffectCur = pDxFrame->pEffect;
	while ( pEffectCur )
	{
		int nIndex = m_listEff.AddString( pEffectCur->GetName() );
		m_listEff.SetItemData( nIndex, (DWORD)pEffectCur );

		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}
}

void CStaticMeshCreate::OnBnClickedButtonPefLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();

	CString szFilter = "Piece Edit File (*.pef)|*.pef";
	CFileDialog dlg ( TRUE, ".pef", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePieceEdit().c_str()); //SUBPATH::OBJ_FILE_PIECEEDIT);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxPieceEdit* pPieceEdit = pView->GetPieceEdit();

		//	Note : ���� ������ ����.
		pPieceEdit->CleanUp();

		//	Note : ���ο� FrameMesh ����
		pPieceEdit->LoadPEF( pView->GetD3DDevice(), sFileName.GetString(), szPath );
		
		CWnd *pWnd = GetDlgItem( IDC_EDIT_PEF_NAME );
		pWnd->SetWindowText( sFileName.GetString() );

		//	Note : ������ Ʈ�� ���.
		//
		SetTreeFrame ( pPieceEdit->GetFrameRoot() );
		
		pWnd = GetDlgItem ( IDC_EDIT_XFILE );
		pWnd->SetWindowText ( pPieceEdit->GetFrameName() );

        CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_ALPHA_BLEND );
		pButton->SetCheck( pPieceEdit->m_dwFlags & EMPO_PICK_ALPHA );

		pButton = (CButton*)GetDlgItem( IDC_CHECK_ANI_PLAY_ONCE );
		pButton->SetCheck( pPieceEdit->m_dwFlags & EMPO_ANI_PLAY_ONCE );

		pButton = (CButton*)GetDlgItem( IDC_CHECK_UNBAKED_LIGHTMAP );
		pButton->SetCheck( pPieceEdit->m_dwFlags & EMPO_UNSUPPORT_RAD );

		CString Str;
		Str.Format( _T("%3.2f"), pPieceEdit->m_fVisibleRadius );
		GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

		Str.Format( _T("%3.2f"), pPieceEdit->m_fInvisibleRadius );
		GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );

		ResetButton ();		// Button �缳��
	}
}

void CStaticMeshCreate::OnBnClickedButtonPefSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString strXFileName;
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->GetWindowText( strXFileName );

	std::string strName = sc::util::ChangeExtName( strXFileName.GetString(), "" );

	CString szFilter = "Piece Edit File (*.pef)|*.pef";
	CFileDialog dlg( FALSE, ".pef", strName.c_str(), OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePieceEdit().c_str()); //SUBPATH::OBJ_FILE_PIECEEDIT);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_PEF_NAME );
		pWnd->SetWindowText( FilePath );

		FilePath = StrInitDir + FilePath;

		strcpy ( cName, FilePath.GetString() );

		pView->GetPieceEdit()->SavePEF( cName, strXFileName.GetString() );

		ResetButton ();		// Button �缳��
	}
}

void CStaticMeshCreate::OnBnClickedButtonPieLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString szFilter = "Piece File Ver_1_5 (*.pie15)|*.pie15|Piece File Ver_1_0 (*.pie)|*.pie|";
	CFileDialog dlg ( TRUE, ".pie15", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE);

	char szPathPieceEdit[MAX_PATH] = {0};
	strcpy(szPathPieceEdit, pFrame->m_szAppPath);
	strcat(szPathPieceEdit, pPath->ObjFilePieceEdit().c_str()); //SUBPATH::OBJ_FILE_PIECE);

	CString StrInitDir( szPath );
	dlg.m_ofn.lpstrInitialDir = StrInitDir.GetString();

	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxPieceEdit* pPieceEdit = pView->GetPieceEdit();

		//	Note : ���� ������ ����.
		pPieceEdit->CleanUp();

		CString strFullPath = StrInitDir + sFileName;

		//	Note : ���ο� FrameMesh ����
		pPieceEdit->LoadPIE( pView->GetD3DDevice(), strFullPath.GetString(), szPathPieceEdit, sFileName.GetString() );
		
		CWnd *pWnd = GetDlgItem( IDC_EDIT_PIE_NAME );
		pWnd->SetWindowText( sFileName.GetString() );

		ResetButton ();		// Button �缳��
	}
}

void CStaticMeshCreate::OnBnClickedButtonPieSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString strXFileName;
	CWnd *pWnd = GetDlgItem ( IDC_EDIT_XFILE );
	pWnd->GetWindowText( strXFileName );

	std::string strName = sc::util::ChangeExtName( strXFileName.GetString(), "" );

	CString szFilter = "Piece File All (*.pie15,*.pie)|*.pie15;*.pie|";
	CFileDialog dlg ( FALSE, ".pie", strName.c_str(), OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_PIE_NAME );
		pWnd->SetWindowText( FilePath );

		FilePath = StrInitDir + FilePath;

		strcpy( cName, FilePath.GetString() );

		pView->GetPieceEdit()->SavePIE( cName );

		ResetButton ();		// Button �缳��
	}
}

void CStaticMeshCreate::OnBnClickedCheckAlphaBlend()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		return;
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_ALPHA_BLEND );
	if ( pButton->GetCheck() )
    {
        pPieceEdit->m_dwFlags |= EMPO_PICK_ALPHA;
    }
    else
    {
        pPieceEdit->m_dwFlags &= ~EMPO_PICK_ALPHA;
    }
}

void CStaticMeshCreate::OnBnClickedCheckAniPlayOnce()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		return;
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_ANI_PLAY_ONCE );
	if ( pButton->GetCheck() )
    {
        pPieceEdit->m_dwFlags |= EMPO_ANI_PLAY_ONCE;
    }
    else
    {
        pPieceEdit->m_dwFlags &= ~EMPO_ANI_PLAY_ONCE;
    }
}

void CStaticMeshCreate::OnBnClickedCheckUnbakedLightmap()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxPieceEdit *pPieceEdit = pView->GetPieceEdit();
	if( !pPieceEdit )
	{
		return;
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_UNBAKED_LIGHTMAP );
	if ( pButton->GetCheck() )
	{
		pPieceEdit->m_dwFlags |= EMPO_UNSUPPORT_RAD;
	}
	else
	{
		pPieceEdit->m_dwFlags &= ~EMPO_UNSUPPORT_RAD;
	}
}

void CStaticMeshCreate::OnBnClickedButtonMeshState()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();


	HTREEITEM htreeitemCur = m_TreeFrame.GetSelectedItem();
	if ( htreeitemCur == NULL )	
		return;

	CString sSelectedFrame = m_TreeFrame.GetItemText ( htreeitemCur );

	DxFrameMesh *pFrameMesh = pView->GetPieceEdit()->GetFrameMesh();
	if ( !pFrameMesh )
		return;

	DxFrame *pDxFrame = pFrameMesh->FindFrame ( sSelectedFrame.GetString() );
	if ( !pDxFrame )	
		return;

	if ( pDxFrame->pmsMeshs )
	{
		m_pSheetTab->ActivePieceMeshStatePage ( pDxFrame );
	}
	else
	{
		AfxMessageBox( _T("Mesh ������ ������� �ʽ��ϴ�.") );
	}
}

void CStaticMeshCreate::OnBnClickedCheckRan10()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	BOOL bShowWindow(FALSE);
	CButton* pButton;
	pButton = (CButton*)GetDlgItem( IDC_CHECK_RAN_1_0 );
	if ( pButton->GetCheck() )
	{
		bShowWindow = TRUE;
	}

	pButton = (CButton*)GetDlgItem( IDC_CHECK_UNBAKED_LIGHTMAP );
	pButton->ShowWindow( bShowWindow );
}

void CStaticMeshCreate::OnBnClickedButtonResetRadius()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *)pFrame->GetActiveView();
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if ( !pPieceEdit )
		return;

	pPieceEdit->m_fVisibleRadius = DEFAULT_VISIBLE_RADIUS;
	pPieceEdit->m_fInvisibleRadius = DEFAULT_INVISIBLE_RADIUS;

	CString Str;

	Str.Format( _T("%3.2f"), pPieceEdit->m_fVisibleRadius );
	GetDlgItem( IDC_EDIT_VISIBLE_RADIUS )->SetWindowText( Str );

	Str.Format( _T("%3.2f"), pPieceEdit->m_fInvisibleRadius );
	GetDlgItem( IDC_EDIT_INVISIBLE_RADIUS )->SetWindowText( Str );
}

void CStaticMeshCreate::OnEnChangeEditVisibleRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*)pFrame->GetActiveView();

	CString Str;
	GetDlgItem(IDC_EDIT_VISIBLE_RADIUS)->GetWindowText( Str );
	pView->GetPieceEdit()->m_fVisibleRadius = (float)_tstof( Str.GetString() );
}

void CStaticMeshCreate::OnEnChangeEditInvisibleRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CPropertyPage::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*)pFrame->GetActiveView();

	CString Str;
	GetDlgItem(IDC_EDIT_INVISIBLE_RADIUS)->GetWindowText( Str );
	pView->GetPieceEdit()->m_fInvisibleRadius = (float)_tstof( Str.GetString() );
}

void CStaticMeshCreate::OnBnClickedButtonMaterialSetting()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();
	if ( !pPieceEdit )
		return;

	DxFrameMesh* pFrameMesh = pPieceEdit->GetFrameMesh();
	if ( !pFrameMesh )
		return;

	
	CTextureSetDlg sDlg;
	sDlg.SetMaterial( pFrameMesh->GetMaterialSetting() );
	if ( IDOK == sDlg.DoModal () )
	{
		NSMaterialManager::OnLostDevice();
		NSMaterialManager::OnResetDevice( pView->GetD3DDevice() );
	}
}