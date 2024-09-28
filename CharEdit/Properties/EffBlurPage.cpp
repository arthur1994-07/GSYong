// EffBlurPage.cpp : 구현 파일입니다.
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

#include "EffBlurPage.h"
#include "../Dialog/EffHighDlg.h"
#include ".\effblurpage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffBlurPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CEffBlurPage, CPropertyPage)
CEffBlurPage::CEffBlurPage()
	: CEffCharPage(CEffBlurPage::IDD)
{
}

CEffBlurPage::~CEffBlurPage()
{
}

void CEffBlurPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TRACE_VERT, m_ComboVertsUP);
	DDX_Control(pDX, IDC_COMBO_TRACE_VERT2, m_ComboVertsDOWN);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CEffBlurPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_UP, OnBnClickedButtonDiffuseUp)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_DOWN, OnBnClickedButtonDiffuseDown)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND, OnCbnSelchangeComboBlend)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_CHECK_REFRACT, OnBnClickedCheckRefract)
	ON_BN_CLICKED(IDC_CHECK_ALL_ACTION, OnBnClickedCheckAllAction)
END_MESSAGE_MAP()


// CEffBlurPage 메시지 처리기입니다.

BOOL CEffBlurPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	int nIndex;

	//	Note : 윗 점
	//
	m_ComboVertsUP.ResetContent ();

	for ( int i=0; i<6; ++i )
	{
		nIndex = m_ComboVertsUP.AddString ( szTRACE_NAME[i] );
		m_ComboVertsUP.SetItemData ( nIndex, i );
	}


	//	Note : 아랫점
	//
	m_ComboVertsDOWN.ResetContent ();

	for ( int i=0; i<6; ++i )
	{
		nIndex = m_ComboVertsDOWN.AddString ( szTRACE_NAME[i] );
		m_ComboVertsDOWN.SetItemData ( nIndex, i );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CEffBlurPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_BLUR*)pProp);

	m_ComboVertsUP.SelectString ( 0, m_Property.m_szTraceUP );
	m_ComboVertsDOWN.SelectString ( 0, m_Property.m_szTraceDOWN );

	char szValue[256];

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_REFRACT );
	if ( m_Property.m_dwFlag & USEREFRACT )		pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	pButton = (CButton*) GetDlgItem ( IDC_CHECK_ALL_ACTION );
	if ( m_Property.m_dwFlag & USEALLACTION )	pButton->SetCheck ( TRUE );
	else										pButton->SetCheck ( FALSE );

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FACE );
	_gcvt ( m_Property.m_fFullLifeTemp, 7, szValue );
	pWnd->SetWindowText ( szValue );

	DWORD dwAlpha = ((m_Property.m_vColorUPTemp&0xff000000)>>24);
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_UP );
	_gcvt ( dwAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );

	dwAlpha = ((m_Property.m_vColorDOWNTemp&0xff000000)>>24);
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_DOWN );
	_gcvt ( dwAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );

	// Reset~!!
	int	nIndex;
	CString	strText;
	m_pComboBlend.ResetContent();

	strText.Format ( _T("1.밝게") );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 1 );
	strText.Format ( _T("2.밝게 2배") );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 2 );
	strText.Format ( _T("3.밝게 4배") );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 3 );
	strText.Format ( _T("4.어둡게") );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 4 );
	strText.Format ( _T("5.가리기") );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 5 );

	m_pComboBlend.SetCurSel ( m_Property.m_nBlend - 1 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szTexture );
}

void CEffBlurPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FACE );
	pWnd->GetWindowText ( strText );
	m_Property.m_fFullLifeTemp = (float)atof( strText.GetString() );


	DWORD		dwAlpha;
	D3DCOLOR	m_vColor;

	m_vColor = m_Property.m_vColorUPTemp;
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_UP );
	pWnd->GetWindowText ( strText );
	dwAlpha = atoi ( strText.GetString() );
	m_Property.m_vColorUPTemp = D3DCOLOR_ARGB ( dwAlpha, ((m_vColor&0xff0000)>>16), ((m_vColor&0xff00)>>8), (m_vColor&0xff) );

	m_vColor = m_Property.m_vColorDOWNTemp;
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_DOWN );
	pWnd->GetWindowText ( strText );
	dwAlpha = atoi ( strText.GetString() );
	m_Property.m_vColorDOWNTemp = D3DCOLOR_ARGB ( dwAlpha, ((m_vColor&0xff0000)>>16), ((m_vColor&0xff00)>>8), (m_vColor&0xff) );

	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_Property.m_nBlend = nData;


	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	//	Note : 윗점
	//
	nIndex = m_ComboVertsUP.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	DWORD dwData = m_ComboVertsUP.GetItemData ( nIndex );
	strcpy ( m_Property.m_szTraceUP, szTRACE_NAME[dwData] );

	//	Note : 아랫점
	//
	nIndex = m_ComboVertsDOWN.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	dwData = m_ComboVertsDOWN.GetItemData ( nIndex );
	strcpy ( m_Property.m_szTraceDOWN, szTRACE_NAME[dwData] );


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharBlur::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( _T("이팩트 생성에 실패하였습니다."), _T("ERROR"), MB_OK );
			return;
		}

		pSkinPiece->AddEffList ( m_pEffChar );
	}
	else
	{
		m_pEffChar->SetProperty ( &m_Property );
		m_pEffChar->Create ( pView->GetD3dDevice() );
	}

    EffectApply();
}

void CEffBlurPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedButtonApply();
	
    ReturnPage();
}

void CEffBlurPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();
}

void CEffBlurPage::OnBnClickedButtonDiffuseUp()
{
	D3DCOLOR &d3dColor = m_Property.m_vColorUPTemp;
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
		m_Property.m_vColorUPTemp = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
}

void CEffBlurPage::OnBnClickedButtonDiffuseDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DCOLOR &d3dColor = m_Property.m_vColorDOWNTemp;
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
		m_Property.m_vColorDOWNTemp = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
}

void CEffBlurPage::OnCbnSelchangeComboBlend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_Property.m_nBlend = nData;
}

void CEffBlurPage::OnBnClickedButtonTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText ( FilePath );
	}
}


void CEffBlurPage::OnBnClickedCheckRefract()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_REFRACT );

	if ( pButton->GetCheck () )		m_Property.m_dwFlag |= USEREFRACT;
	else							m_Property.m_dwFlag &= ~USEREFRACT;
}

void CEffBlurPage::OnBnClickedCheckAllAction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_ALL_ACTION );

	if ( pButton->GetCheck () )		m_Property.m_dwFlag |= USEALLACTION;
	else							m_Property.m_dwFlag &= ~USEALLACTION;

	
}
