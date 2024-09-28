// EffArrowPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "../Dialog/ColorPickerDlg.h"

#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "EffArrowPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffArrowPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEffArrowPage, CPropertyPage)
CEffArrowPage::CEffArrowPage()
	: CEffCharPage(CEffArrowPage::IDD)
{
}

CEffArrowPage::~CEffArrowPage()
{
}

void CEffArrowPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CEffArrowPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnBnClickedButtonColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_CHECK_SEC, OnBnClickedCheckSec)
	ON_BN_CLICKED(IDC_CHECK_SIZE, OnBnClickedCheckSize)
	ON_BN_CLICKED(IDC_CHECK_EFFECT, OnBnClickedCheckEffect)
	ON_BN_CLICKED(IDC_BUTTON_SELECT2, OnBnClickedButtonSelect2)
	ON_BN_CLICKED(IDC_BUTTON_SELECT3, OnBnClickedButtonSelect3)
END_MESSAGE_MAP()


// CEffArrowPage 메시지 처리기입니다.

void CEffArrowPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_ARROW*)pProp);

	int	nIndex;
	CString	strText;

	char szValue[256];

	CWnd *pWnd;
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RAND );
	if ( m_Property.m_dwFlag & USERANDROTATE )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_ARROW );
	if ( m_Property.m_dwFlag & USEARROW )		pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );


	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SEC );
	pWnd = GetDlgItem ( IDC_EDIT_SEC );
	if ( m_Property.m_dwFlag & USESEC )
	{
		pButton->SetCheck ( TRUE );
		pWnd->EnableWindow ( TRUE );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_ARROW );
		pButton->EnableWindow ( TRUE );
	}
	else
	{
		pButton->SetCheck ( FALSE );
		pWnd->EnableWindow ( FALSE );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_ARROW );
		pButton->EnableWindow ( FALSE );
	}

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SIZE );
	pWnd = GetDlgItem ( IDC_EDIT_SIZE );
	if ( m_Property.m_dwFlag & USESIZE )	
	{
		pButton->SetCheck ( TRUE );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton->SetCheck ( FALSE );
		pWnd->EnableWindow ( FALSE );
	}

	//	.............
	if ( m_Property.m_dwFlag & USEEFFECT )
	{
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_EFFECT );
		pButton->SetCheck ( TRUE );

		pWnd = GetDlgItem ( IDC_EDIT_BASE );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_EDIT_UPUP );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pButton = (CButton*) GetDlgItem ( IDC_CHECK_EFFECT );
		pButton->SetCheck ( FALSE );

		pWnd = GetDlgItem ( IDC_EDIT_BASE );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_EDIT_UPUP );
		pWnd->EnableWindow ( FALSE );
	}


	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	_gcvt ( m_Property.m_vEffEnd.y, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_TIME );
	_gcvt ( m_Property.m_fFullTime, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHATIME );
	_gcvt ( m_Property.m_fAlphaTime, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SETRATE );
	_gcvt ( m_Property.m_fSetRate, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_DEPTH );
	_gcvt ( m_Property.m_fDepth, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE );
	_gcvt ( m_Property.m_fSize, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_SEC );
	_gcvt ( m_Property.m_nSec, 7, szValue );
	pWnd->SetWindowText ( szValue );

	int	nAlpha = (m_Property.m_vColor>>24);
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA3 );
	_gcvt ( nAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->SetWindowText ( m_Property.m_szMeshFile );

	pWnd = GetDlgItem ( IDC_EDIT_BASE );
	pWnd->SetWindowText ( m_Property.m_szFileName_C );

	pWnd = GetDlgItem ( IDC_EDIT_UPUP );
	pWnd->SetWindowText ( m_Property.m_szFileName_R );


	m_pComboBlend.ResetContent();

	strText.Format ( "1.밝게" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 1 );
	strText.Format ( "2.밝게 2배" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 2 );
	strText.Format ( "3.밝게 4배" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 3 );
	strText.Format ( "4.어둡게" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 4 );
	strText.Format ( "5.가리기" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 5 );

	m_pComboBlend.SetCurSel ( m_Property.m_nBlend - 1 );
}

void CEffArrowPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;
	CWnd	*pWnd;

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	pWnd->GetWindowText ( strText );
	m_Property.m_vEffEnd.y = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_TIME );
	pWnd->GetWindowText ( strText );
	m_Property.m_fFullTime = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHATIME );
	pWnd->GetWindowText ( strText );
	m_Property.m_fAlphaTime = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SETRATE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fSetRate = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_DEPTH );
	pWnd->GetWindowText ( strText );
	m_Property.m_fDepth = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SIZE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fSize = (float)atof ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_SEC );
	pWnd->GetWindowText ( strText );
	m_Property.m_nSec = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_ALPHA3 );
	pWnd->GetWindowText ( strText );
	int		nAlpha	= atoi ( strText.GetString() );
	DWORD	vColor	= ((m_Property.m_vColor<<8)>>8);
	vColor			= vColor + (nAlpha<<24);
	m_Property.m_vColor = vColor;

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_RAND );
	if ( pButton->GetCheck () )		m_Property.m_dwFlag |= USERANDROTATE;
	else							m_Property.m_dwFlag &= ~USERANDROTATE;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_ARROW );
	if ( pButton->GetCheck () )		m_Property.m_dwFlag |= USEARROW;
	else							m_Property.m_dwFlag &= ~USEARROW;

	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );
	m_Property.m_nBlend = nData;


	const char*	szMeshFile;
	pWnd = GetDlgItem ( IDC_EDIT_FILE );
	pWnd->GetWindowText ( strText );
	szMeshFile = strText.GetString();
	strcpy ( m_Property.m_szMeshFile, szMeshFile );

	pWnd = GetDlgItem ( IDC_EDIT_BASE );
	pWnd->GetWindowText ( strText );
	szMeshFile = strText.GetString();
	strcpy ( m_Property.m_szFileName_C, szMeshFile );

	pWnd = GetDlgItem ( IDC_EDIT_UPUP );
	pWnd->GetWindowText ( strText );
	szMeshFile = strText.GetString();
	strcpy ( m_Property.m_szFileName_R, szMeshFile );

	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharArrow::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "이팩트 생성에 실패하였습니다.", "ERROR", MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->CleanUp();
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    EffectApply();
}

void CEffArrowPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();
	
    ReturnPage();
}

void CEffArrowPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffArrowPage::OnBnClickedButtonColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DCOLOR &d3dColor = m_Property.m_vColor;
	COLORREF retColor;
	COLORREF Color = RGB ( ((d3dColor&0xff0000)>>16), ((d3dColor&0xff00)>>8), (d3dColor&0xff) );
	CColorPickerDlg colorDlg ( Color );

	int	R,G,B;
	R = (d3dColor&0xff0000)>>16;
	G = (d3dColor&0xff00)>>8;
	B = d3dColor&0xff;

	if ( IDOK == colorDlg.DoModal () )
	{
		DWORD dwAlpha = ((d3dColor&0xff000000)>>24);

		retColor = colorDlg.GetColor ();

		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );
		m_Property.m_vColor = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
}

void CEffArrowPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd *pWnd;
	CString szFilter = "x File (*.x,*.mxf)|*.X;*.mxf|";
	CFileDialog dlg ( TRUE, ".x", NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = DxSimpleMeshMan::GetInstance().GetPath().c_str();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();		
		pWnd = GetDlgItem ( IDC_EDIT_FILE );
		pWnd->SetWindowText ( FilePath );
		const char *szTemp = FilePath.GetString();
		strcpy( m_Property.m_szMeshFile , szTemp );
	}	
}

void CEffArrowPage::OnBnClickedCheckSec()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	CWnd*		pWnd;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SEC );
	pWnd = GetDlgItem ( IDC_EDIT_SEC );
	if ( pButton->GetCheck () )
	{
		m_Property.m_dwFlag |= USESEC;
		pWnd->EnableWindow ( TRUE );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_ARROW );
		pButton->EnableWindow ( TRUE );
	}
	else							
	{
		m_Property.m_dwFlag &= ~USESEC;
		pWnd->EnableWindow ( TRUE );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_ARROW );
		pButton->EnableWindow ( FALSE );
	}
}

void CEffArrowPage::OnBnClickedCheckSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	CWnd*		pWnd;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_SIZE );
	pWnd = GetDlgItem ( IDC_EDIT_SIZE );
	if ( pButton->GetCheck () )
	{
		m_Property.m_dwFlag |= USESIZE;
		pWnd->EnableWindow ( TRUE );
	}
	else							
	{
		m_Property.m_dwFlag &= ~USESIZE;
		pWnd->EnableWindow ( TRUE );
	}
}


void CEffArrowPage::OnBnClickedCheckEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	CWnd*		pWnd;

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_EFFECT );

	if ( pButton->GetCheck () )
	{
		m_Property.m_dwFlag |= USEEFFECT;

		pWnd = GetDlgItem ( IDC_EDIT_BASE );
		pWnd->EnableWindow ( TRUE );

		pWnd = GetDlgItem ( IDC_EDIT_UPUP );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		m_Property.m_dwFlag &= ~USEEFFECT;

		pWnd = GetDlgItem ( IDC_EDIT_BASE );
		pWnd->EnableWindow ( FALSE );

		pWnd = GetDlgItem ( IDC_EDIT_UPUP );
		pWnd->EnableWindow ( FALSE );
	}
}

void CEffArrowPage::OnBnClickedButtonSelect2()		// Base
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FileName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_BASE );
		pWnd->SetWindowText ( FileName );
	}

}

void CEffArrowPage::OnBnClickedButtonSelect3()		// Up Up
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, this );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString FileName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_UPUP );
		pWnd->SetWindowText ( FileName );
	}
}
