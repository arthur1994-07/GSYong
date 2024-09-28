// MeshTransformTab.cpp : implementation file
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "EffGroupPage.h"

#include "../Dialog/SetVar.h"
#include "../Dialog/SetMultiSlider.h"

#include "MeshTransformTab.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CMeshTransformTab dialog


CMeshTransformTab::CMeshTransformTab(CWnd* pParent /*=NULL*/)
	: CDialog(CMeshTransformTab::IDD, pParent),
	m_fRotateAngle(0.f)
{
	//{{AFX_DATA_INIT(CMeshTransformTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMeshTransformTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeshTransformTab)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_ctrlRotateDirection);
}


BEGIN_MESSAGE_MAP(CMeshTransformTab, CDialog)
	//{{AFX_MSG_MAP(CMeshTransformTab)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_READ, OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_READ2, OnBnClickedButtonRead2)
	ON_BN_CLICKED(IDC_BUTTON_READ3, OnBnClickedButtonRead3)
	ON_BN_CLICKED(IDC_BUTTON_GROUND, OnBnClickedButtonGround)
	ON_BN_CLICKED(IDC_BUTTON_DIRECT, OnBnClickedButtonDirect)
	ON_BN_CLICKED(IDC_CHECK_BLEND_MESH, OnBnClickedCheckBlendMesh)
	ON_BN_CLICKED(IDC_CHECK_BLUR, OnBnClickedCheckBlur)
	ON_BN_CLICKED(IDC_CHECK_ROTATE, OnBnClickedCheckRotate)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT, OnBnClickedButtonHeight)
	ON_BN_CLICKED(IDC_BUTTON_SCALE, OnBnClickedButtonScale)
	ON_BN_CLICKED(IDC_BUTTON_SCALEXYZ, OnBnClickedButtonScalexyz)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_DISABLE, OnBnClickedButtonHeightDisable)
	ON_BN_CLICKED(IDC_BUTTON_XYZSCALE_DISABLE, OnBnClickedButtonXyzscaleDisable)
	ON_BN_CLICKED(IDC_BUTTON_SCALE_DISABLE, OnBnClickedButtonScaleDisable)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshTransformTab message handlers

void CMeshTransformTab::Init ( MESH_PROPERTY* pProp )
{
	m_pProperty = pProp;

	CWnd*		pWnd;
	CButton*	pButton;

	{
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_ROTATE );
		if ( m_pProperty->m_dwFlag & USEROTATE )	pButton->SetCheck ( TRUE );
		else										pButton->SetCheck ( FALSE );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_BLEND_MESH );
		if ( m_pProperty->m_dwFlag & USEBLENDMESH)
		{
			pButton->SetCheck ( TRUE );

			pWnd = GetDlgItem ( IDC_BUTTON_READ2 );
			pWnd->EnableWindow ( TRUE );

			pWnd = GetDlgItem ( IDC_BUTTON_READ3 );
			pWnd->EnableWindow ( TRUE );

			pWnd = GetDlgItem ( IDC_EDIT_MORPHNUM );
			pWnd->EnableWindow ( TRUE );
		}
		else
		{
			pButton->SetCheck ( FALSE );

			pWnd = GetDlgItem ( IDC_BUTTON_READ2 );
			pWnd->EnableWindow ( FALSE );

			pWnd = GetDlgItem ( IDC_BUTTON_READ3 );
			pWnd->EnableWindow ( FALSE );

			pWnd = GetDlgItem ( IDC_EDIT_MORPHNUM );
			pWnd->EnableWindow ( FALSE );
		}

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_BLUR );
		if ( m_pProperty->m_dwFlag & USEBLUR)
		{
			pButton->SetCheck ( TRUE );

			pWnd = GetDlgItem ( IDC_EDIT_BLUR_NUM );
			pWnd->EnableWindow ( TRUE );
		}
		else
		{
			pButton->SetCheck ( FALSE );

			pWnd = GetDlgItem ( IDC_EDIT_BLUR_NUM );
			pWnd->EnableWindow ( FALSE );
		}

		//	Note: ȸ�� ���� �κ��� �ɼ� 
		pWnd = GetDlgItem( IDC_COMBO_DIRECTION );
		pWnd->EnableWindow( m_pProperty->m_dwFlag & USEROTATE );

		pWnd = GetDlgItem( IDC_EDIT_ROTATE_ANGLE );
		pWnd->EnableWindow( m_pProperty->m_dwFlag & USEROTATE );

		pWnd = GetDlgItem ( IDC_EDIT_FILE );
		pWnd->SetWindowText ( m_pProperty->m_szMeshFile );

		pWnd = GetDlgItem ( IDC_EDIT_FILE2 );
		pWnd->SetWindowText ( m_pProperty->m_szMeshFile1 );

		pWnd = GetDlgItem ( IDC_EDIT_FILE3 );
		pWnd->SetWindowText ( m_pProperty->m_szMeshFile2 );

		m_ctrlRotateDirection.ResetContent();
		m_ctrlRotateDirection.AddString( "��ǥ�� ����" );
		m_ctrlRotateDirection.AddString( "��������1" ); 
		m_ctrlRotateDirection.AddString( "��������2" );
		m_ctrlRotateDirection.AddString( "��������3" );
		m_ctrlRotateDirection.AddString( "X�� ȸ��" );
		m_ctrlRotateDirection.AddString( "Y�� ȸ��" );
		m_ctrlRotateDirection.AddString( "Z�� ȸ��" );
		m_ctrlRotateDirection.SetCurSel( m_pProperty->m_nRotationType );	

		m_fRotateAngle = m_pProperty->m_fRotationAngle;

		char	szValue[256];

		pWnd = GetDlgItem ( IDC_EDIT_ROTATE_ANGLE );
		_gcvt ( m_pProperty->m_fRotationAngle, 7, szValue );
		pWnd->SetWindowText ( szValue );		

		pWnd = GetDlgItem ( IDC_EDIT_MORPHNUM );
		_gcvt ( m_pProperty->m_fMorphRoopNum, 7, szValue );
		pWnd->SetWindowText ( szValue );

		pWnd = GetDlgItem ( IDC_EDIT_BLUR_NUM );
		_gcvt ( m_pProperty->m_nBlurObject, 7, szValue );
		pWnd->SetWindowText ( szValue );
	}

	{
		m_cClrGround.SubclassDlgItem ( IDC_BUTTON_GROUND, this );
		m_cClrDirect.SubclassDlgItem ( IDC_BUTTON_DIRECT, this );
		m_cClrHeight.SubclassDlgItem ( IDC_BUTTON_HEIGHT, this );
		m_cClrScale.SubclassDlgItem ( IDC_BUTTON_SCALE, this );
		m_cClrXYZScale.SubclassDlgItem ( IDC_BUTTON_SCALEXYZ, this );

		COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
		cColorGRAY = RGB(212, 208, 200);
		cColorBLUE = RGB(192, 192, 255);
		cColorTEXT = RGB(0, 0, 0);

		if ( pProp->m_dwFlag & USEGOTOCENTER )	m_cClrGround.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrGround.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEDIRECTION )	m_cClrDirect.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrDirect.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USEHEIGHT_MESH )	m_cClrHeight.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrHeight.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USESCALE )		m_cClrScale.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrScale.SetColour ( cColorTEXT, cColorGRAY );

		if ( pProp->m_dwFlag & USESIZEXYZ )		m_cClrXYZScale.SetColour ( cColorTEXT, cColorBLUE );
		else									m_cClrXYZScale.SetColour ( cColorTEXT, cColorGRAY );
	}
}

void CMeshTransformTab::Apply ()
{
	CString strText;
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szMeshFile, strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_FILE2 );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szMeshFile1, strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_FILE3 );
	pWnd->GetWindowText ( strText );
	strcpy ( m_pProperty->m_szMeshFile2, strText.GetString() );

	if( m_pProperty->m_dwFlag & USEROTATE )
	{
		m_pProperty->m_nRotationType = m_ctrlRotateDirection.GetCurSel();
		m_pProperty->m_fRotationAngle = m_fRotateAngle;
	}

	if( m_pProperty->m_dwFlag & USEROTATE )
	{
		m_pProperty->m_nRotationType = m_ctrlRotateDirection.GetCurSel();

		pWnd = GetDlgItem ( IDC_EDIT_ROTATE_ANGLE );
		pWnd->GetWindowText ( strText );
		m_pProperty->m_fRotationAngle = (float) atof ( strText.GetString() );
	} // if( m_pProperty->m_dwFlag & USEROTATE )

	pWnd = GetDlgItem ( IDC_EDIT_MORPHNUM );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_fMorphRoopNum = (float) atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_BLUR_NUM );
	pWnd->GetWindowText ( strText );
	m_pProperty->m_nBlurObject = atoi ( strText.GetString() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CMeshTransformTab::OnBnClickedButtonRead()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CWnd *pWnd;
	CString szFilter = "x File (*.x)|*.x|";
	CFileDialog dlg ( TRUE, ".x", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = DxSimpleMeshMan::GetInstance().GetPath().c_str();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();		
		pWnd = GetDlgItem ( IDC_EDIT_FILE );
		pWnd->SetWindowText ( FilePath );
		strcpy( m_pProperty->m_szMeshFile , FilePath.GetString() );
	}	

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CMeshTransformTab::OnBnClickedButtonRead2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CWnd *pWnd;
	CString szFilter = "x File (*.x)|*.x|";
	CFileDialog dlg ( TRUE, ".x", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = DxSimpleMeshMan::GetInstance().GetPath().c_str();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();		
		pWnd = GetDlgItem ( IDC_EDIT_FILE2 );
		pWnd->SetWindowText ( FilePath );
		strcpy( m_pProperty->m_szMeshFile1 , FilePath.GetString() );
	}	

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CMeshTransformTab::OnBnClickedButtonRead3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CWnd *pWnd;
	CString szFilter = "x File (*.x)|*.x|";
	CFileDialog dlg ( TRUE, ".x", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = DxSimpleMeshMan::GetInstance().GetPath().c_str();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();		
		pWnd = GetDlgItem ( IDC_EDIT_FILE3 );
		pWnd->SetWindowText ( FilePath );
		strcpy( m_pProperty->m_szMeshFile2 , FilePath.GetString() );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CMeshTransformTab::OnBnClickedCheckBlendMesh()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	DxEffGroupPlayer::GetInstance().DeleteEff ( CEffGroupPage::m_EffPropGroup.m_strFileName.c_str() );
	m_pProperty->CleanUp ();

	CWnd	*pWnd;
	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_BLEND_MESH );

	if ( pButton->GetCheck() )
	{
		m_pProperty->m_dwFlag |= USEBLENDMESH;

		pWnd = GetDlgItem ( IDC_BUTTON_READ2 );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_BUTTON_READ3 );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_EDIT_MORPHNUM );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_pProperty->m_dwFlag &= ~USEBLENDMESH;
		pWnd = GetDlgItem ( IDC_BUTTON_READ2 );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_BUTTON_READ3 );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_EDIT_MORPHNUM );
		pWnd->EnableWindow ( FALSE );
	}
}

void CMeshTransformTab::OnBnClickedCheckBlur()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*	pWnd;
	CButton *pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_BLUR );

	if ( pButton->GetCheck() )
	{
		m_pProperty->m_dwFlag |= USEBLUR;

		pWnd = GetDlgItem ( IDC_EDIT_BLUR_NUM );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_pProperty->m_dwFlag &= ~USEBLUR;

		pWnd = GetDlgItem ( IDC_EDIT_BLUR_NUM );
		pWnd->EnableWindow ( FALSE );
	}
}

void CMeshTransformTab::OnBnClickedCheckRotate()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd *pWnd;
	CButton *pButton = (CButton *)GetDlgItem( IDC_CHECK_ROTATE );
	if ( pButton->GetCheck() )	m_pProperty->m_dwFlag |= USEROTATE;
	else						m_pProperty->m_dwFlag &= ~USEROTATE;

	pWnd = GetDlgItem( IDC_COMBO_DIRECTION );
	pWnd->EnableWindow( m_pProperty->m_dwFlag & USEROTATE );

	pWnd = GetDlgItem( IDC_EDIT_ROTATE_ANGLE );
	pWnd->EnableWindow( m_pProperty->m_dwFlag & USEROTATE );
}

void CMeshTransformTab::OnBnClickedButtonHeight()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;
	DlgSetVar.m_StrTitle = "<����, ��> ���� �̵�";
	DlgSetVar.m_StrComm1 = "���� ��ġ";
	DlgSetVar.m_StrComm3 = "�� ��ġ";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_fStart		= m_pProperty->m_vHeight.x;
	DlgSetVar.m_fEnd		= m_pProperty->m_vHeight.y;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_vHeight.x = DlgSetVar.m_fStart;
		m_pProperty->m_vHeight.y = DlgSetVar.m_fEnd;


		m_pProperty->m_dwFlag |= USEHEIGHT_MESH;

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrHeight.SetColour ( cColorTEXT, cColorBLUE );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CMeshTransformTab::OnBnClickedButtonScale()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetMultiSlider DlgSetSlider;
	DlgSetSlider.m_StrTitle = "�޽� ũ�� ����";
	DlgSetSlider.m_StrTemp4 = "ũ��";

	DlgSetSlider.m_values[0] = m_pProperty->m_fSizeRate1;
	DlgSetSlider.m_values[1] = m_pProperty->m_fSizeRate2;

	DlgSetSlider.m_fStart = m_pProperty->m_fSizeStart;
	DlgSetSlider.m_fMid1 = m_pProperty->m_fSizeMid1;
	DlgSetSlider.m_fMid2 = m_pProperty->m_fSizeMid2;
	DlgSetSlider.m_fEnd = m_pProperty->m_fSizeEnd;
	
	if( DlgSetSlider.DoModal() == IDOK )
	{
		m_pProperty->m_fSizeRate1 = DlgSetSlider.m_values[0];
		m_pProperty->m_fSizeRate2 = DlgSetSlider.m_values[1];

		m_pProperty->m_fSizeStart = DlgSetSlider.m_fStart;
		m_pProperty->m_fSizeMid1 = DlgSetSlider.m_fMid1;
		m_pProperty->m_fSizeMid2 = DlgSetSlider.m_fMid2;
		m_pProperty->m_fSizeEnd = DlgSetSlider.m_fEnd;


		m_pProperty->m_dwFlag |= USESCALE;

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrScale.SetColour ( cColorTEXT, cColorBLUE );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CMeshTransformTab::OnBnClickedButtonScalexyz()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	pView->SetCurClick( FALSE );

	CSetVar DlgSetVar;

	DlgSetVar.m_StrTitle = "XYZ Scale";
	DlgSetVar.m_StrComm1 = "x��";
	DlgSetVar.m_StrComm2 = "y��";
	DlgSetVar.m_StrComm3 = "z��";
	DlgSetVar.m_dwFlag = USESTARTSCROLL | USESTARTVARSCROLL | USEENDSCROLL;
	DlgSetVar.m_bZeroBase = TRUE;
	DlgSetVar.m_ToScale = 40.0f;

	DlgSetVar.m_fStart		= m_pProperty->m_vSizeXYZ.x;
	DlgSetVar.m_fStartVar	= m_pProperty->m_vSizeXYZ.y;
	DlgSetVar.m_fEnd		= m_pProperty->m_vSizeXYZ.z;
	
	if ( IDOK == DlgSetVar.DoModal() )
	{
		m_pProperty->m_vSizeXYZ.x = DlgSetVar.m_fStart;
		m_pProperty->m_vSizeXYZ.y = DlgSetVar.m_fStartVar;
		m_pProperty->m_vSizeXYZ.z = DlgSetVar.m_fEnd;


		m_pProperty->m_dwFlag |= USESIZEXYZ;

		COLORREF cColorBLUE = RGB(192, 192, 255);
		COLORREF cColorTEXT = RGB(0, 0, 0);
		m_cClrXYZScale.SetColour ( cColorTEXT, cColorBLUE );
	}

	pView->SetCurClick( TRUE );	// Ŀ�� Ŭ���� �츰��.
}

void CMeshTransformTab::OnBnClickedButtonGround()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEGOTOCENTER;

	if ( m_pProperty->m_dwFlag & USEGOTOCENTER )	m_cClrGround.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrGround.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshTransformTab::OnBnClickedButtonDirect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(212, 208, 200);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	m_pProperty->m_dwFlag ^= USEDIRECTION;

	if ( m_pProperty->m_dwFlag & USEDIRECTION )		m_cClrDirect.SetColour ( cColorTEXT, cColorBLUE );
	else											m_cClrDirect.SetColour ( cColorTEXT, cColorGRAY );
}
void CMeshTransformTab::OnBnClickedButtonHeightDisable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_dwFlag &= ~USEHEIGHT_MESH;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrHeight.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshTransformTab::OnBnClickedButtonXyzscaleDisable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_dwFlag &= ~USESIZEXYZ;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrXYZScale.SetColour ( cColorTEXT, cColorGRAY );
}

void CMeshTransformTab::OnBnClickedButtonScaleDisable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pProperty->m_dwFlag &= ~USESCALE;

	COLORREF cColorTEXT	= RGB(0, 0, 0);
	COLORREF cColorGRAY = RGB(212, 208, 200);
	m_cClrScale.SetColour ( cColorTEXT, cColorGRAY );
}
