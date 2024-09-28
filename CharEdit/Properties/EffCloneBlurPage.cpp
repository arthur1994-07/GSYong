// EffCloneBlurPage.cpp : ���� �����Դϴ�.
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

#include "EffCloneBlurPage.h"

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CEffCloneBlurPage ��ȭ �����Դϴ�.
IMPLEMENT_DYNAMIC(CEffCloneBlurPage, CPropertyPage)
CEffCloneBlurPage::CEffCloneBlurPage()
	: CEffCharPage(CEffCloneBlurPage::IDD)
{
}

CEffCloneBlurPage::~CEffCloneBlurPage()
{
}

void CEffCloneBlurPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TRACE_VERT, m_ComboVertsUP);
	DDX_Control(pDX, IDC_COMBO_TRACE_VERT2, m_ComboVertsDOWN);
	DDX_Control(pDX, IDC_COMBO_BLEND, m_pComboBlend);
}


BEGIN_MESSAGE_MAP(CEffCloneBlurPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND, OnCbnSelchangeComboBlend)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_UP, OnBnClickedButtonDiffuseUp)
	ON_BN_CLICKED(IDC_BUTTON_DIFFUSE_DOWN, OnBnClickedButtonDiffuseDown)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE, OnBnClickedButtonTexture)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CEffCloneBlurPage �޽��� ó�����Դϴ�.

BOOL CEffCloneBlurPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	int nIndex;

	//	Note : �� ��
	//
	m_ComboVertsUP.ResetContent ();

	for ( int i=0; i<6; ++i )
	{
		nIndex = m_ComboVertsUP.AddString ( szTRACE_NAME[i] );
		m_ComboVertsUP.SetItemData ( nIndex, i );
	}



	//	Note : �Ʒ���
	//
	m_ComboVertsDOWN.ResetContent ();

	for ( int i=0; i<6; ++i )
	{
		nIndex = m_ComboVertsDOWN.AddString ( szTRACE_NAME[i] );
		m_ComboVertsDOWN.SetItemData ( nIndex, i );
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CEffCloneBlurPage::SetProperty ( EFFCHAR_PROPERTY *pProp )
{
	if ( pProp )
		m_Property = *((EFFCHAR_PROPERTY_CLONEBLUR*)pProp);

	m_ComboVertsUP.SelectString ( 0, m_Property.m_szTraceUP );
	m_ComboVertsDOWN.SelectString ( 0, m_Property.m_szTraceDOWN );

	char szValue[256];

	CButton*	pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_BEZIER );
	if ( m_Property.m_bBezier )	pButton->SetCheck ( TRUE );
	else						pButton->SetCheck ( FALSE );

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FACE );
	_gcvt ( m_Property.m_dwFaces, 7, szValue );
	pWnd->SetWindowText ( szValue );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH );
	_gcvt ( m_Property.m_fWidth, 7, szValue );
	pWnd->SetWindowText ( szValue );

	DWORD dwAlpha = ((m_Property.m_vColorUP&0xff000000)>>24);
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_UP );
	_gcvt ( dwAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );

	dwAlpha = ((m_Property.m_vColorDOWN&0xff000000)>>24);
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_DOWN );
	_gcvt ( dwAlpha, 7, szValue );
	pWnd->SetWindowText ( szValue );

	// Reset~!!
	int	nIndex;
	CString	strText;
	m_pComboBlend.ResetContent();

	strText.Format ( "1.���" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 1 );
	strText.Format ( "2.��� 2��" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 2 );
	strText.Format ( "3.��� 4��" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 3 );
	strText.Format ( "4.��Ӱ�" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 4 );
	strText.Format ( "5.������" );
	nIndex = m_pComboBlend.AddString ( strText );
	m_pComboBlend.SetItemData ( nIndex, 5 );

	m_pComboBlend.SetCurSel ( m_Property.m_nBlend - 1 );

	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->SetWindowText ( m_Property.m_szTexture );
}

void CEffCloneBlurPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	CString	strText;

	CButton* pButton;
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_BEZIER );
	if ( pButton->GetCheck () )	m_Property.m_bBezier = TRUE;
	else						m_Property.m_bBezier = FALSE;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FACE );
	pWnd->GetWindowText ( strText );
	m_Property.m_dwFaces = atoi ( strText.GetString() );

	pWnd = GetDlgItem ( IDC_EDIT_WIDTH );
	pWnd->GetWindowText ( strText );
	m_Property.m_fWidth = (float)atof ( strText.GetString() );

	DWORD		dwAlpha;
	D3DCOLOR	m_vColor;

	m_vColor = m_Property.m_vColorUP;
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_UP );
	pWnd->GetWindowText ( strText );
	dwAlpha = atoi ( strText.GetString() );
	m_Property.m_vColorUP = D3DCOLOR_ARGB ( dwAlpha, ((m_vColor&0xff0000)>>16), ((m_vColor&0xff00)>>8), (m_vColor&0xff) );

	m_vColor = m_Property.m_vColorDOWN;
	pWnd = GetDlgItem ( IDC_EDIT_ALPHA_DOWN );
	pWnd->GetWindowText ( strText );
	dwAlpha = atoi ( strText.GetString() );
	m_Property.m_vColorDOWN = D3DCOLOR_ARGB ( dwAlpha, ((m_vColor&0xff0000)>>16), ((m_vColor&0xff00)>>8), (m_vColor&0xff) );

	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_Property.m_nBlend = nData;


	const char* szTexture;
	pWnd = GetDlgItem ( IDC_EDIT_TEXTURE );
	pWnd->GetWindowText ( strText );
	szTexture = strText.GetString();
	strcpy ( m_Property.m_szTexture, szTexture );

	//	Note : ����
	//
	nIndex = m_ComboVertsUP.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	DWORD dwData = m_ComboVertsUP.GetItemData ( nIndex );
	strcpy ( m_Property.m_szTraceUP, szTRACE_NAME[dwData] );

	//	Note : �Ʒ���
	//
	nIndex = m_ComboVertsDOWN.GetCurSel ();
	if ( LB_ERR == nIndex )		return;
	dwData = m_ComboVertsDOWN.GetItemData ( nIndex );
	strcpy ( m_Property.m_szTraceDOWN, szTRACE_NAME[dwData] );


	if ( !m_pEffChar )
	{
		m_pEffChar = DxEffCharMan::GetInstance().CreateEffect
		(
			DxEffCharCloneBlur::TYPEID,
			&m_Property,
			NULL,
			pSkinPiece
		);

		if ( !m_pEffChar )
		{
			MessageBox ( "����Ʈ ������ �����Ͽ����ϴ�.", "ERROR", MB_OK );
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

void CEffCloneBlurPage::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedButtonApply();

    ReturnPage();
}

void CEffCloneBlurPage::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    ReturnPage();
}

void CEffCloneBlurPage::OnCbnSelchangeComboBlend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex, nData;
	nIndex = m_pComboBlend.GetCurSel();
	nData = m_pComboBlend.GetItemData ( nIndex );

	m_Property.m_nBlend = nData;
}

void CEffCloneBlurPage::OnBnClickedButtonDiffuseUp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	D3DCOLOR &d3dColor = m_Property.m_vColorUP;
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
		m_Property.m_vColorUP = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
}

void CEffCloneBlurPage::OnBnClickedButtonDiffuseDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	D3DCOLOR &d3dColor = m_Property.m_vColorDOWN;
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
		m_Property.m_vColorDOWN = D3DCOLOR_ARGB ( dwAlpha, R, G, B );
	}
}

void CEffCloneBlurPage::OnBnClickedButtonTexture()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
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
