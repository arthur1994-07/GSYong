// ExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "../EngineLib/DxLand/LightMap/NSLightMapTools.h"
#include "../EngineLib/DxLand/DxLandMan.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "worldedit.h"
#include "ExportDlg.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "CheckIntegrityNaviMeshDlg.h"
#include "../EngineLib/DxLand/LightMap/NSLightMap.h"
#include "../EngineLib/Common/SUBPATH.H"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int WINAPI ExportWorld( LPVOID	pParam );

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog


CExportDlg::CExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportDlg)	
	m_Facecount = 0;
	//}}AFX_DATA_INIT
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Control(pDX, IDC_EDIT_EXPORT_FACECOUNT, m_ctrlEditFacecount);
	DDX_Control(pDX, IDC_BUTTON_EXPORT_OK, m_ctrlButtonOK);
	DDX_Control(pDX, IDC_BUTTON_EXPORT, m_ctrlButtonStart);
	DDX_Control(pDX, IDC_PROGRESS_EXPORT, m_ctrlProgress);
	DDX_Text(pDX, IDC_EDIT_EXPORT_FACECOUNT, m_Facecount);
	DDX_Control(pDX, IDC_LIST_EXPORT_LOG, m_ctrlListLog);
	DDX_Control(pDX, IDC_BUTTON_EXPORT_CHECKINTEGRITY, m_ctrlButtonCheckIntegrity);
	//}}AFX_DATA_MAP	
	DDX_Control(pDX, IDC_COMBO_SAMPLING, m_comboSampling);
}


BEGIN_MESSAGE_MAP(CExportDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDlg)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_OK, OnButtonExportOK)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_CHECKINTEGRITY, OnBnClickedButtonExportCheckintegrity)
	//}}AFX_MSG_MAP	
	ON_BN_CLICKED(IDC_CHECK_RAN_1_0_EXPORT, &CExportDlg::OnBnClickedCheckRan10Export)
	ON_EN_CHANGE(IDC_EDIT_RADIOSITY_FOG_BLEND, &CExportDlg::OnEnChangeEditRADFogBlend)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers

static	const	int	ID_EXPORT			=	1;
static	const	int	THREAD_MSG_START	=	1;
static	const	int	THREAD_MSG_SUCCEED	=	2;
static	const	int	THREAD_MSG_FAILED	=	3;
void CExportDlg::OnButtonExport() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	
	if ( m_Facecount < 1000 )
	{
		if ( m_Facecount < 5 )
		{
			MessageBox ( "페이스 수가 최소한 5개는 되어야 합니다.", "확인", MB_ICONSTOP );
			return;
		}
		if ( IDNO == MessageBox ( "설정하신 값이 너무 작습니다.\n"
			"경우에 따라서는 많은 시간이 소요됩니다.\n계속하시겠습니까?", "질문", MB_YESNO | MB_ICONQUESTION ) )
		{
			return;
		}
	}

	CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_ONE_CELL ) );
	m_SParam.m_bOneCell = pButton->GetCheck();	

	pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_LIGHTMAP ) );
	m_SParam.m_bCreateLightMap = pButton->GetCheck();

	pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_RAN_1_0_EXPORT ) );
	m_SParam.m_bRan_1_0_Export = pButton->GetCheck();

	m_SParam.m_bRadiosity = GetWin_Check( this, IDC_CHECK_RADIOSITY );
	m_SParam.m_nRadiosityPass = GetWin_Num_int ( this, IDC_EDIT_RADIOSITY_PASS );
	m_SParam.m_fRadiosityReflectance = GetWin_Num_float ( this, IDC_EDIT_RADIOSITY_REFLECTANCE );
	m_SParam.m_fRadiosityFogBlend = GetWin_Num_float ( this, IDC_EDIT_RADIOSITY_FOG_BLEND );
	m_SParam.m_dwRadiosityFogLength = GetWin_Num_int ( this, IDC_EDIT_RADIOSITY_FOG_LENGTH );

	if ( m_SParam.m_bRadiosity )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
		DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
		if ( pSetLandMan )
		{
			VEC_TSTRING vecErrorPiece;
			pSetLandMan->CheckPieceForRadiosity( vecErrorPiece );

			if ( !vecErrorPiece.empty() )
			{
				TSTRING strName(_T("Radiosity 를 생성할 수 없는 Piece가 있습니다.\n새로 Export가 필요합니다.\n다른맵(psf)에 셋팅되어있다면, 다른이름으로 Piece 만들어주세요.\n\n"));
				for ( DWORD i=0; i<vecErrorPiece.size(); ++i )
				{
					strName += _T("[");
					strName += vecErrorPiece[i];
					strName += _T("] \n");
				}

				AfxMessageBox( strName.c_str() );
				return;
			}
		}
	}
	
	m_ctrlButtonStart.EnableWindow ( FALSE );
	m_ctrlButtonOK.EnableWindow ( FALSE );
	m_ctrlEditFacecount.EnableWindow ( FALSE );

	//	Note	:	0 나누기 방지
	ExportProgress::EndPos = 1;
	ExportProgress::CurPos = 0;
	memset ( ExportProgress::szState, 0, sizeof ( char ) * MAX_PATH );

	m_ctrlProgress.SetRange ( 0, 100 );
	m_ctrlProgress.SetPos ( 0 );

	SetTimer ( ID_EXPORT, 5, NULL );

    {	
	    m_SParam.m_nFacecount = m_Facecount;
	    m_SParam.m_Result = THREAD_MSG_START;

        SetLightMap_SetLandMan();

		pButton = (CButton*)GetDlgItem( IDC_CHECK_NO_DXT1 );
		NSLightMap::g_bNoDXT1 = pButton->GetCheck();

		int nIndex = m_comboSampling.GetCurSel();
		NSLightMap::g_nSampling = m_comboSampling.GetItemData ( nIndex );

	    //CreateThread( NULL, 0, ExportWorld, &m_SParam, 0, NULL );
	    ::_beginthreadex(
		    NULL,
		    0,
		    ExportWorld,
		    &m_SParam,
		    0,
		    NULL );
    }
}


void CExportDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	switch ( nIDEvent )
	{
		case ID_EXPORT:
		{			
			if ( ExportProgress::EndPos <= 0 )
			{
                ExportProgress::EndPos = 1;
			}
			int Pos = (int)( ( ExportProgress::CurPos * 100 ) / ExportProgress::EndPos );
			m_ctrlProgress.SetPos ( Pos );

			char	szString[256];
			ExportProgress::GetStateString ( szString );
			if ( strlen ( szString ) )
			{
				CString	Str = szString;

				int nIndex = m_ctrlListLog.AddString ( Str );
				m_ctrlListLog.SetCurSel ( nIndex );
			}
			
			if ( m_SParam.m_Result == THREAD_MSG_SUCCEED )
			{				
				KillTimer ( ID_EXPORT );	

				int nIndex = m_ctrlListLog.AddString ( "Export 완료" );
				m_ctrlListLog.SetCurSel ( nIndex );

				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();				
				DxLandMan* pDxLandMan = pView->GetLandMan ();

				//	<--	무결성 검사
				if ( !pDxLandMan->IsNaviMeshIntegrity() )
				{
					m_ctrlButtonCheckIntegrity.EnableWindow ( TRUE );
				}
				else
				{
					m_ctrlButtonOK.EnableWindow ( TRUE );
				}
				//	-->	무결성 검사
			}
			else if ( m_SParam.m_Result == THREAD_MSG_FAILED )
			{
				KillTimer ( ID_EXPORT );
				MessageBox ( "Export에 실패하였습니다.", "확인", MB_ICONSTOP );
				EndDialog ( 0 );		
			}
			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}


unsigned int WINAPI ExportWorld(
	LPVOID pParam)
{
	HRESULT hr = S_OK;

	SExportParam	*pExportParam = (SExportParam *)pParam;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//	Note : 디버그 절대 경로 가져오기
    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	TCHAR szFullPath[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szFullPath, CSIDL_PERSONAL, FALSE );
	StringCchCat( szFullPath, MAX_PATH, pPath->DebugInfoRoot().c_str() ); //SUBPATH::DEBUGINFO_ROOT );

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
	DxLandMan* pLandMan = pView->GetLandMan();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();

	pLandMan->CleanUp ();

	////////////////////////////////////////////////////////////
	//	Note	:	SKY	
	//	적절한 데이타를 Octree로 넘겨준다.
	//	한개의 박스안에 최대 폴리곤 수 : 10000
	//	트리의 최대 깊이 : 10000 --> 무한대	
	if ( pExportParam->m_bRan_1_0_Export )
	{
		hr = pLandMan->Import_Ran_1_0( pExportParam->m_nFacecount, 
										100000, 
										pSetLandMan, 
										pd3dDevice, 
										szFullPath,
										pExportParam->m_bCreateLightMap, 
										pExportParam->m_bCreateLightMap ? pExportParam->m_szLightMapName : TSTRING(_T("")), 
										pSetLandMan->GetLightMapDetail(),
										pPath );
	}
	else
	{
		// Radiosity 일 경우 Ambient 값을 0 로 사용해야 하기 때문에 이렇게 작업됨.
		NSLightMapTools::SetRadiosityMode( pExportParam->m_bRadiosity );

		hr = pLandMan->Import( pExportParam->m_nFacecount, 
								pExportParam->m_bOneCell, 
								pSetLandMan, 
								pd3dDevice, 
								szFullPath,
								pPath,
								pExportParam->m_bCreateLightMap, 
								pExportParam->m_bCreateLightMap ? pExportParam->m_szLightMapName : TSTRING(_T("")), 
								pExportParam->m_bRadiosity,
								pExportParam->m_nRadiosityPass,
								pExportParam->m_fRadiosityReflectance,
								pExportParam->m_fRadiosityFogBlend,
								pExportParam->m_dwRadiosityFogLength);

		NSLightMapTools::SetRadiosityMode( FALSE );
	}

	if ( FAILED(hr) )
	{
		pLandMan->CleanUp ();

		pExportParam->m_Result = THREAD_MSG_FAILED;
		//ExitThread( 0 );
		return 0;
	}
	
	pExportParam->m_Result = THREAD_MSG_SUCCEED;
	//ExitThread( 0 );

	return 0;
}

void CExportDlg::OnButtonExportOK() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	pView->TempCreateChar ();

	CDialog::OnOK();
}

BOOL CExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlButtonCheckIntegrity.EnableWindow ( FALSE );
	m_Facecount = 1000;
	UpdateData(FALSE);

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
    DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
    if ( pSetLandMan )
    {
		StringCchCopy( m_SParam.m_szLightMapName, MAX_PATH, pSetLandMan->GetLightMapName().c_str() );
    }

    CWnd* pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_NAME );
    pWnd->SetWindowText( m_SParam.m_szLightMapName );
   
	CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_ONE_CELL ) );
	pButton->SetCheck( FALSE );

    pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_LIGHTMAP ) );
    pButton->SetCheck( TRUE );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_NO_DXT1 );
	pButton->SetCheck( TRUE );

	m_comboSampling.ResetContent();

	CString strText;
	strText.Format ( "1 sample" );
	int nIndex = m_comboSampling.AddString ( strText );
	m_comboSampling.SetItemData ( nIndex, 1 );
	//strText.Format ( "Quincunx (2.x)" );
	//nIndex = m_comboSampling.AddString ( strText );
	//m_comboSampling.SetItemData ( nIndex, 2 );
	strText.Format ( "2x2 grid" );
	nIndex = m_comboSampling.AddString ( strText );
	m_comboSampling.SetItemData ( nIndex, 3 );
	strText.Format ( "2x2 RGSS" );
	nIndex = m_comboSampling.AddString ( strText );
	m_comboSampling.SetItemData ( nIndex, 4 );
	strText.Format ( "8 rooks" );
	nIndex = m_comboSampling.AddString ( strText );
	m_comboSampling.SetItemData ( nIndex, 5 );
	strText.Format ( "4x4 grid" );
	nIndex = m_comboSampling.AddString ( strText );
	m_comboSampling.SetItemData ( nIndex, 6 );

	m_comboSampling.SetCurSel ( 0 );

	SetWin_Check( this, IDC_CHECK_RADIOSITY, TRUE );
	SetWin_Num_int ( this, IDC_EDIT_RADIOSITY_PASS, 3 );
	SetWin_Num_float ( this, IDC_EDIT_RADIOSITY_REFLECTANCE, 1.f );
	SetWin_Num_float ( this, IDC_EDIT_RADIOSITY_FOG_BLEND, 0.45f );
	SetWin_Num_float ( this, IDC_EDIT_RADIOSITY_FOG_LENGTH, 900 );

	if ( pSetLandMan )
	{
		DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
		if ( pLight )
		{
			SetWin_Num_int ( this, IDC_EDIT_AMBIENT_R, static_cast<int>( 255*pLight->m_Light.Ambient.r ) );
			SetWin_Num_int ( this, IDC_EDIT_AMBIENT_G, static_cast<int>( 255*pLight->m_Light.Ambient.g ) );
			SetWin_Num_int ( this, IDC_EDIT_AMBIENT_B, static_cast<int>( 255*pLight->m_Light.Ambient.b ) );
			SetWin_Num_int ( this, IDC_EDIT_AMBIENT_ALL, static_cast<int>( 255*(pLight->m_Light.Ambient.r+pLight->m_Light.Ambient.g+pLight->m_Light.Ambient.b) ) );
		}

		DisplayRADFogColor();
	}

	pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_RAN_1_0_EXPORT ) );
	pButton->SetCheck( FALSE );

	pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_LIGHTMAP ) );
	pButton->ShowWindow( FALSE );	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExportDlg::OnBnClickedButtonExportCheckintegrity()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditDoc* pDoc = (CWorldEditDoc *) pFrame->GetActiveDocument ();	
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
	
	if ( !pDoc->m_pCheckIntegrityDlg )
	{
		pDoc->m_pCheckIntegrityDlg = new CCheckIntegrityNaviMeshDlg;
		pDoc->m_pCheckIntegrityDlg->Create ( IDD_DIALOG_CHECKINTEGRITY );
	}

	pView->TempCreateChar ();

	CDialog::OnOK();
}

void CExportDlg::SetLightMap_SetLandMan()
{
    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();
    DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
    if ( pSetLandMan )
    {
        CString strText;
        CWnd* pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_NAME );
        pWnd->GetWindowText( strText );
		StringCchCopy( m_SParam.m_szLightMapName, MAX_PATH, strText.GetString() );

        //pWnd = GetDlgItem( IDC_EDIT_LIGHTMAP_DETAIL );
        //pWnd->GetWindowText( strText );
        //m_SParam.m_fLightMapDetail = static_cast<float>( atof( strText.GetString() ) );

        pSetLandMan->SetLightMapInfo( m_SParam.m_szLightMapName );
    }
}

void CExportDlg::OnBnClickedCheckRan10Export()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CButton* pButton;
	pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_RAN_1_0_EXPORT ) );
	if ( pButton->GetCheck() )
	{
		pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_LIGHTMAP ) );
		pButton->ShowWindow( TRUE );
	}
	else
	{
		pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_LIGHTMAP ) );
		pButton->ShowWindow( FALSE );
	}
}

void CExportDlg::OnEnChangeEditRADFogBlend()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DisplayRADFogColor();
}

void CExportDlg::DisplayRADFogColor()
{
	DWORD dwFogColor(0L);
	D3DXVECTOR3 vDirectLightAmbient(0.f,0.f,0.f);
	const FOG_PROPERTY& sFogProperty = DxFogMan::GetInstance().GetProperty();
	if ( sFogProperty.m_bSkyFogEnable )
	{
		dwFogColor = sFogProperty.m_cFogColor_Day;
	}
	else
	{
		dwFogColor = sFogProperty.m_cFogColor;
	}

	D3DLIGHTQ* pDirectLight = &DxLightMan::GetInstance()->GetDirectNoonLight()->m_Light;
	vDirectLightAmbient = D3DXVECTOR3( pDirectLight->Ambient.r, pDirectLight->Ambient.g, pDirectLight->Ambient.b );

	// Fog 색을 정한다.
	D3DXCOLOR colorClear;
	colorClear.r = static_cast<float>( (dwFogColor&0xff0000) >> 16 );
	colorClear.g = static_cast<float>( (dwFogColor&0xff00) >> 8 );
	colorClear.b = static_cast<float>( (dwFogColor&0xff) );

	D3DXVECTOR3 vFogColor(colorClear.r/255.f,colorClear.g/255.f,colorClear.b/255.f);
	float fAmbientALL = vDirectLightAmbient.x + vDirectLightAmbient.y + vDirectLightAmbient.z;
	float fFogColorALL = vFogColor.x + vFogColor.y + vFogColor.z;

	if ( fFogColorALL > 0.0001f )
	{
		vFogColor.x *= fAmbientALL / fFogColorALL;
		vFogColor.y *= fAmbientALL / fFogColorALL;
		vFogColor.z *= fAmbientALL / fFogColorALL;
	}
	else
	{
		vFogColor.x = vDirectLightAmbient.x;
		vFogColor.y = vDirectLightAmbient.y;
		vFogColor.z = vDirectLightAmbient.z;
	}



	float fFogPower = 1.f;

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_RADIOSITY_FOG_BLEND );
	pWnd->GetWindowText( strText );
	float fFogBlend = (float)_tstof( strText.GetString() );

	{
		float fR = static_cast<float>(vFogColor.x*255);	//static_cast<DWORD>(((colorClear&0xff0000) >> 16));
		float fG = static_cast<float>(vFogColor.y*255);	//static_cast<DWORD>(((colorClear&0xff00) >> 8));
		float fB = static_cast<float>(vFogColor.z*255);	//static_cast<DWORD>(((colorClear&0xff)));
		float fGray = (fR + fG + fB) * (1.f-fFogBlend) * 0.333f;
		fR = fR * fFogBlend + fGray;
		fG = fG * fFogBlend + fGray;
		fB = fB * fFogBlend + fGray;
		DWORD dwR = static_cast<DWORD>( fR * fFogPower );
		DWORD dwG = static_cast<DWORD>( fG * fFogPower );
		DWORD dwB = static_cast<DWORD>( fB * fFogPower );
		if ( dwR > 255 )	dwR = 255;
		if ( dwG > 255 )	dwG = 255;
		if ( dwB > 255 )	dwB = 255;

		SetWin_Num_int ( this, IDC_EDIT_RAD_FOG_R, dwR );
		SetWin_Num_int ( this, IDC_EDIT_RAD_FOG_G, dwG );
		SetWin_Num_int ( this, IDC_EDIT_RAD_FOG_B, dwB );
		SetWin_Num_int ( this, IDC_EDIT_RAD_FOG_ALL, dwR + dwG + dwB );
	}
}