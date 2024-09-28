// DlgEdit.cpp : implementation file
//

#include "stdafx.h"

#include "../../MfcExLib/ExLibs/FECFileDialog.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../EngineLib/Common/SubPath.h"
#include "../../EngineLib/DxLand/DxPiece/DxPiece.h"

#include "../worldedit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "DlgPieceEdit.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CDlgPieceEdit dialog

CDlgPieceEdit::CDlgPieceEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPieceEdit::IDD, pParent)
    , m_bActive(TRUE)
    , CSettingIni(_T("WorldEdit.ini"), _T("DlgPieceEdit"))
	, m_bSlope(FALSE)
	, m_fSlopeRate(1.f)
{
	//{{AFX_DATA_INIT(CDlgPieceEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
  
void CDlgPieceEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPieceEdit)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_PIECE, m_ctrlPIECE);
	DDX_Control(pDX, IDC_CHECK_RANDOMROTATE, m_btnRandomRotate);
}


BEGIN_MESSAGE_MAP(CDlgPieceEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgPieceEdit)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_INSERT, &CDlgPieceEdit::OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgPieceEdit::OnBnClickedButtonDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PIECE, &CDlgPieceEdit::OnNMClickListPiece)
	ON_BN_CLICKED(IDC_BUTTON_INSERT_SPT, &CDlgPieceEdit::OnBnClickedButtonInsertSpt)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW_LEFT, &CDlgPieceEdit::OnBnClickedButtonPreviewLeft)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW_RIGHT, &CDlgPieceEdit::OnBnClickedButtonPreviewRight)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW_LEFT2, &CDlgPieceEdit::OnBnClickedButtonPreviewLeft2)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW_RIGHT2, &CDlgPieceEdit::OnBnClickedButtonPreviewRight2)
	ON_BN_CLICKED(IDC_BUTTON_SETANGLE, &CDlgPieceEdit::OnBnClickedButtonSetangle)
	ON_BN_CLICKED(IDC_CHECK_RANDOMROTATE, &CDlgPieceEdit::OnBnClickedCheckRandomrotate)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ALL, &CDlgPieceEdit::OnBnClickedButtonDeleteAll)
    ON_EN_CHANGE(IDC_EDIT_R_SCALE_MIN, &CDlgPieceEdit::OnEnChangeEditRScaleMin)
    ON_EN_CHANGE(IDC_EDIT_R_SCALE_MAX, &CDlgPieceEdit::OnEnChangeEditRScaleMax)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_LBUTTONUP()
    ON_WM_NCLBUTTONUP()
    ON_EN_SETFOCUS(IDC_EDIT_ANGLE, &CDlgPieceEdit::OnEnSetfocusEditAngle)
    ON_EN_SETFOCUS(IDC_EDIT_SCALE, &CDlgPieceEdit::OnEnSetfocusEditScale)
    ON_EN_SETFOCUS(IDC_EDIT_R_SCALE_MIN, &CDlgPieceEdit::OnEnSetfocusEditRScaleMin)
    ON_EN_SETFOCUS(IDC_EDIT_R_SCALE_MAX, &CDlgPieceEdit::OnEnSetfocusEditRScaleMax)
    ON_WM_PARENTNOTIFY()
    ON_EN_UPDATE(IDC_EDIT_R_SCALE_MIN, &CDlgPieceEdit::OnEnUpdateEditRScaleMin)
    ON_WM_ACTIVATE()
    ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_CHECK_SLOPE, &CDlgPieceEdit::OnBnClickedCheckSlope)
	ON_EN_CHANGE(IDC_EDIT_SLOPE_RATE, &CDlgPieceEdit::OnEnChangeEditSlopeRate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPieceEdit message handlers

BOOL CDlgPieceEdit::OnInitDialog()
{
	CDialog::OnInitDialog();    
	
	RECT listRect;
	m_ctrlPIECE.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[2] = { int(nListWidth*1.05f), int(nListWidth*0.55f) };

	m_ctrlPIECE.SetExtendedStyle ( m_ctrlPIECE.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ctrlPIECE.InsertColumn ( 0, _T("Piece"),	LVCFMT_CENTER, nColWidth[0] );
	
	// �̸����� â ���� �� ��ġ ���� By Monster7j
	m_cDlgPiecePreView.Create( CDlgPiecePreView::IDD ,this );
	m_cDlgPiecePreView.MoveWindow(186,46,150,150);
	
    CWnd* pWnd = GetDlgItem( IDC_EDIT_R_SCALE_MIN );
    pWnd->SetWindowText( _T("1.f") );

    pWnd = GetDlgItem( IDC_EDIT_R_SCALE_MAX );
    pWnd->SetWindowText( _T("1.f") );

	pWnd = GetDlgItem( IDC_EDIT_SLOPE_RATE );
	pWnd->SetWindowText( _T("1.f") );

    m_cDlgPiecePreView.m_vRandomScale.x = 1.f;
    m_cDlgPiecePreView.m_vRandomScale.y = 1.f;

    LoadPosition(this);

	return TRUE;
	// return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CDlgPieceEdit::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//UpdateLIST();

	return CDialog::PreCreateWindow(cs);
}

void CDlgPieceEdit::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
//	SAFE_DELETE(m_psheetWithTab);

	CWorldEditView::g_pPIECEEDIT = NULL;
	

	CDialog::PostNcDestroy();
}


void CDlgPieceEdit::OnBnClickedButtonInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	CString szFilter = _T("Piece File Ver_1_5 (*.pie15)|*.pie15;*.jpg|Piece File Ver_1_0 (*.pie)|*.pie;*.jpg|");
	CFECFileDialog dlg ( TRUE, _T(".pie15"), NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE);


	//TCHAR szPath[MAX_PATH] = {0};
	//::StringCchCopy(
	//	szPath,
	//	MAX_PATH,
	//	pFrame->m_szAppPath );
	//::StringCchCat(
	//	szPath,
	//	MAX_PATH,
	//	CORE_COMMON::SUBPATH::getInstance()->getObjFilePiece() );

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString strPathName;
		POSITION pos = dlg.GetStartPosition();
		while (pos != NULL)
		{
			strPathName = dlg.GetNextPathName(pos);
			CString strFileName = strPathName;
			int nPosition = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(nPosition+1);

			if ( sc::string::getFileExt( strFileName.GetString() ) == _T("pie") )
			{
				pView->GetPieceView()->InsertPIE( strFileName.GetString() );
			}
			if ( sc::string::getFileExt( strFileName.GetString() ) == _T("pie15") )
			{
				pView->GetPieceView()->InsertPIE( strFileName.GetString() );
			}
		}
		UpdateLIST();

		//pView->GetPieceView()->InsertPIE( dlg.GetFileName() );
		//UpdateLIST();
	}
}

void CDlgPieceEdit::OnBnClickedButtonInsertSpt()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//CString szFilter = _T("SpeedTree File (*.spt)|*.spt");
	//CFileDialog dlg ( TRUE, _T(".spt"), NULL, OFN_HIDEREADONLY, szFilter, this );

	//TCHAR szPath[MAX_PATH] = {0};
	//StringCchCopy( szPath, MAX_PATH, pFrame->m_szAppPath );
	//StringCchCat( szPath, MAX_PATH, SUBPATH::SPEEDTREE_FILE );

	//CString StrInitDir = szPath;
	//dlg.m_ofn.lpstrInitialDir = StrInitDir;

	//if ( dlg.DoModal() == IDOK )
	//{
	//	pView->GetPieceView()->InsertSPT( dlg.GetFileName() );
	//	UpdateLIST();
	//}
}

void CDlgPieceEdit::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_ctrlPIECE.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}

	int nSize = m_ctrlPIECE.GetSelectedCount();
	if( nSize==0 )	return;

	int nItem = m_ctrlPIECE.GetNextSelectedItem( pos );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	pView->GetPieceView()->DeletePIE( nItem );
	
	UpdateLIST();
}

void CDlgPieceEdit::OnBnClickedButtonDeleteAll()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	pView->GetPieceView()->DeletePIE_ALL();

	UpdateLIST();
}

void CDlgPieceEdit::UpdateLIST()
{
	m_ctrlPIECE.DeleteAllItems();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	DWORD i=0;
	for( ; i<pView->GetPieceView()->m_vecPIE.size(); ++i )
	{
		m_ctrlPIECE.InsertItem( i, pView->GetPieceView()->m_vecPIE[i].c_str() );
	}
}

void CDlgPieceEdit::OnNMClickListPiece(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	POSITION pos = m_ctrlPIECE.GetFirstSelectedItemPosition();
	if( pos==NULL )
	{
		MessageBox( _T("Please. Check Point") );
		return;
	}

	int nSize = m_ctrlPIECE.GetSelectedCount();
	if( nSize==0 )	return;

	const TCHAR* pName;
	int nItem = m_ctrlPIECE.GetNextSelectedItem( pos );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	if( pName = pView->GetPieceView()->GetPieceName( nItem ) )
	{
		CWorldEditView::g_strPIECENAME = pName;
	}
	CWorldEditView::g_pPIECEEDIT = pView->GetPieceView()->GetPiece( nItem );

	DxPieceManagerPSF::g_emEDITMODE = DxPieceManagerPSF::MODE_INSERT;

	if( pView )	
        pView->m_sObjectMRS.SetMode( 0 );

	// ���õ� �ǽ��� ������ �̸����⿡ ���� ������ �Ѵ�. By Monster7j
	m_cDlgPiecePreView.SetPieceData(pView->GetPieceView()->GetPieceName( nItem ));

	D3DPRESENT_PARAMETERS presentParams;

	ZeroMemory(&presentParams, sizeof(presentParams));

	presentParams.Windowed = TRUE;
	presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
	presentParams.BackBufferWidth = 150;
	presentParams.BackBufferHeight = 150;
	presentParams.BackBufferFormat = D3DFORMAT(D3DADAPTER_DEFAULT);

	SAFE_RELEASE( pView->m_pSwapChainPreView );
	pView->GetD3DDevice()->CreateAdditionalSwapChain( &presentParams, &pView->m_pSwapChainPreView );

	DWORD dwLighting(0);
	DWORD dwCullMode(0);
	DWORD dwFillMode(0);
	if (pView->GetD3DDevice() )
	{
		pView->GetD3DDevice()->GetRenderState( D3DRS_LIGHTING, &dwLighting );
		pView->GetD3DDevice()->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
		pView->GetD3DDevice()->GetRenderState( D3DRS_FILLMODE, &dwFillMode );

		pView->GetD3DDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
		pView->GetD3DDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pView->GetD3DDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	}

	D3DXMATRIX matView, matProj;
	pView->GetD3DDevice()->GetTransform( D3DTS_VIEW, &matView );
	pView->GetD3DDevice()->GetTransform(D3DTS_PROJECTION, &matProj);

	m_cDlgPiecePreView.Setup();
	m_cDlgPiecePreView.SetCameraEX();
	m_cDlgPiecePreView.Render();

	pView->GetD3DDevice()->SetTransform( D3DTS_VIEW, &matView );
	pView->GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matProj);

	if (pView->GetD3DDevice() )
	{
		pView->GetD3DDevice()->SetRenderState( D3DRS_LIGHTING, dwLighting );
		pView->GetD3DDevice()->SetRenderState( D3DRS_CULLMODE, dwCullMode );
		pView->GetD3DDevice()->SetRenderState( D3DRS_FILLMODE, dwFillMode );
	}

	this->SetDlgItemInt(IDC_EDIT_ANGLE,0,FALSE);
	this->SetDlgItemInt(IDC_EDIT_SCALE,1,FALSE);

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
		pWnd->SetFocus();
	else
		pFrame->SetFocus();
}


void CDlgPieceEdit::PreInitDialog()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CDialog::PreInitDialog();
}



void CDlgPieceEdit::OnBnClickedButtonPreviewLeft()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_cDlgPiecePreView.SetRotateLeft();
	m_cDlgPiecePreView.Render();
	
	this->SetDlgItemInt(IDC_EDIT_ANGLE,m_cDlgPiecePreView.m_nPieceAngle,FALSE);

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}

void CDlgPieceEdit::OnBnClickedButtonPreviewRight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_cDlgPiecePreView.SetRotateRight();
	m_cDlgPiecePreView.Render();

	this->SetDlgItemInt(IDC_EDIT_ANGLE,m_cDlgPiecePreView.m_nPieceAngle,FALSE);

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}


void CDlgPieceEdit::OnBnClickedButtonPreviewLeft2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_cDlgPiecePreView.SetScaleDown();
	m_cDlgPiecePreView.Render();

	this->SetDlgItemInt(IDC_EDIT_SCALE,int(m_cDlgPiecePreView.m_fPieceScale),FALSE);

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}

void CDlgPieceEdit::OnBnClickedButtonPreviewRight2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_cDlgPiecePreView.SetScaleUp();
	m_cDlgPiecePreView.Render();

	this->SetDlgItemInt(IDC_EDIT_SCALE,int(m_cDlgPiecePreView.m_fPieceScale),FALSE);

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}


void CDlgPieceEdit::OnBnClickedButtonSetangle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strScale;

	int nAngle = this->GetDlgItemInt(IDC_EDIT_ANGLE,NULL,FALSE);
	this->GetDlgItemText(IDC_EDIT_SCALE,strScale);

	m_cDlgPiecePreView.SetRotate(nAngle);
	m_cDlgPiecePreView.SetScale( (float)_tstof(strScale) );
	m_cDlgPiecePreView.Render();

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}

void CDlgPieceEdit::OnBnClickedCheckRandomrotate()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_nRandom = m_btnRandomRotate.GetCheck();

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}

void CDlgPieceEdit::OnEnChangeEditRScaleMin()
{
    // TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
    // CDialog::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
    // ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
    // �ش� �˸� �޽����� �����ϴ�.

    // TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString strText;
    CWnd* pWnd = GetDlgItem( IDC_EDIT_R_SCALE_MIN );
    pWnd->GetWindowText( strText );
    m_cDlgPiecePreView.m_vRandomScale.x = (float) atof ( strText.GetString() );
}

void CDlgPieceEdit::OnEnChangeEditRScaleMax()
{
    // TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
    // CDialog::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
    // ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
    // �ش� �˸� �޽����� �����ϴ�.

    // TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString strText;
    CWnd* pWnd = GetDlgItem( IDC_EDIT_R_SCALE_MAX );
    pWnd->GetWindowText( strText );
    m_cDlgPiecePreView.m_vRandomScale.y = (float) atof ( strText.GetString() );
}

void CDlgPieceEdit::OnSetFocus(CWnd* pOldWnd)
{
    CDialog::OnSetFocus(pOldWnd);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnKillFocus(CWnd* pNewWnd)
{
    CDialog::OnKillFocus(pNewWnd);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

    CDialog::OnLButtonUp(nFlags, point);
}

void CDlgPieceEdit::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

    CDialog::OnNcLButtonUp(nHitTest, point);
}

void CDlgPieceEdit::OnEnSetfocusEditAngle()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnEnSetfocusEditScale()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnEnSetfocusEditRScaleMin()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnEnSetfocusEditRScaleMax()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnParentNotify(UINT message, LPARAM lParam)
{
    CDialog::OnParentNotify(message, lParam);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnEnUpdateEditRScaleMin()
{
    // TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
    // CDialog::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
    // IParam ����ũ�� OR �����Ͽ� ������ ENM_SCROLL �÷��׸� �����Ͽ�
    // ���� ��Ʈ���� �ٲ� �ؽ�Ʈ�� ǥ���Ϸ��� ���� ��Ÿ���ϴ�.

    // TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CDlgPieceEdit::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialog::OnActivate(nState, pWndOther, bMinimized);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    if ( nState & WA_ACTIVE )
    {
        m_bActive = TRUE;
    }
    else
    {
        m_bActive = FALSE;
    }
}

void CDlgPieceEdit::OnBnClickedCheckSlope()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_SLOPE );
	if ( pButton->GetCheck() )
	{
		m_bSlope = TRUE;
	}
	else
	{
		m_bSlope = FALSE;
	}

	// �θ� Focus �� �ش�.
	CWnd* pWnd = GetParent();
	if ( pWnd )
	{
		pWnd->SetFocus();
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		pFrame->SetFocus();
	}
}

void CDlgPieceEdit::OnEnChangeEditSlopeRate()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// __super::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_SLOPE_RATE );
	pWnd->GetWindowText( strText );
	m_fSlopeRate = (float) atof ( strText.GetString() );
}
