// AroundEffPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../CharEdit.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../Dialog/ColorPickerDlg.h"
#include "SheetWithTabChar.h"

#include "AroundEffPage.h"
#include ".\aroundeffpage.h"

extern D3DXVECTOR3		g_vCLOSED_POINT;
extern D3DXVECTOR3		g_vCLOSED_NORMAL;
extern std::string		g_strPICK_BONE;

#pragma warning(disable:4996)

#include "../../SigmaCore/DebugInclude.h"

// CAroundEffPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAroundEffPage, CPropertyPage)
CAroundEffPage::CAroundEffPage()
	: CEffCharPage(CAroundEffPage::IDD)
{
	
}

CAroundEffPage::~CAroundEffPage()
{
}

void CAroundEffPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LINEBONE, m_AroundEffList);
	DDX_Control(pDX, IDC_SPEED_SLIDER1, m_TexSpeedSlider1);
	DDX_Control(pDX, IDC_SPEED_SLIDER2, m_TexSpeedSlider2);
}


BEGIN_MESSAGE_MAP(CAroundEffPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GET1, OnBnClickedButtonGet1)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND, OnBnClickedButtonTextureFind)
	ON_BN_CLICKED(IDC_BUTTON_TEXTURE_FIND2, OnBnClickedButtonTextureFind2)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_COLOR3, OnBnClickedButtonColor3)
END_MESSAGE_MAP()


// CAroundEffPage 메시지 처리기입니다.
BOOL CAroundEffPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	RECT listRect;
	m_AroundEffList.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nColWidth[3] = { int(nListWidth*0.15f), int(nListWidth*0.43f), int(nListWidth*0.43f) };

	m_AroundEffList.SetExtendedStyle ( m_AroundEffList.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_AroundEffList.InsertColumn ( 0, _T("ID"),		LVCFMT_CENTER, nColWidth[0] );
	m_AroundEffList.InsertColumn ( 1, _T("Pos"),	LVCFMT_CENTER, nColWidth[1] );
	m_AroundEffList.InsertColumn ( 2, _T("Bone"),	LVCFMT_CENTER, nColWidth[2] );


	SetDlgItemText( IDC_EDIT_XSIZE_MIN, "1.0" );
	SetDlgItemText( IDC_EDIT_XSIZE_MAX, "1.0" );
	SetDlgItemText( IDC_EDIT_YSIZE_MIN, "1.0" );
	SetDlgItemText( IDC_EDIT_YSIZE_MAX, "1.0" );
	SetDlgItemText( IDC_EDIT_LIFE_MIN, "5.0" );
	SetDlgItemText( IDC_EDIT_LIFE_MAX, "10.0" );
	SetDlgItemText( IDC_EDIT_LIFE_MIN2, "1.0" );
	SetDlgItemText( IDC_EDIT_LIFE_MAX2, "10.0" );
	SetDlgItemText( IDC_EDIT_MESH_DIST, "1.0" );
	SetDlgItemText( IDC_EDIT_TEXSPEED_MIN, "25.0" );
	SetDlgItemText( IDC_EDIT_TEXSPEED_MAX, "25.0" );
	SetDlgItemText( IDC_EDIT_COL2, "8" );
	SetDlgItemText( IDC_EDIT_ROW2, "8" );
	SetDlgItemText( IDC_EDIT_ALPHA4, "255" );
	SetDlgItemText( IDC_EDIT_TEXTURE, "" );

	m_TexSpeedSlider1.SetRange( 1, 50 );
	m_TexSpeedSlider2.SetRange( 1, 50 );

	m_TexSpeedSlider1.SetPos( 25 );
	m_TexSpeedSlider2.SetPos( 25 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAroundEffPage::SetProperty( EFFCHAR_PROPERTY *pProp )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	DxSkinPiece *pSkinPiece = pView->GetSkinPiece ();

	if( pProp )	m_Property = *((EFFCHAR_PROPERTY_CHAR_AROUND*)pProp);
	else
	{
		EFFCHAR_PROPERTY_CHAR_AROUND sProp;
		m_Property = sProp;
	}

	if ( !m_pEffChar )
	{
		m_pEffChar = (DxCharAroundEff*)DxEffCharMan::GetInstance().CreateEffect
			(
			DxCharAroundEff::TYPEID,
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

	CButton*	pButton;
	pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
	if( m_Property.m_dwFlag&USE_SEQUENCE )	pButton->SetCheck( TRUE );
	else									pButton->SetCheck( FALSE );

	// Note : Sequece
	char szTempChar[32] = {0,};
	sprintf( szTempChar, "%d", m_Property.m_dwSequenceCol );
	SetDlgItemText( IDC_EDIT_COL2, szTempChar );
	sprintf( szTempChar, "%d", m_Property.m_dwSequenceRow );
	SetDlgItemText( IDC_EDIT_ROW2, szTempChar );

	DWORD dwAlpha = ((m_Property.m_dwColor&0xff000000)>>24);
	sprintf( szTempChar, "%d", dwAlpha );
	SetDlgItemText( IDC_EDIT_ALPHA4, szTempChar );

	sprintf( szTempChar, "%.2f", m_Property.m_fMinTextureSpeed );
	SetDlgItemText( IDC_EDIT_TEXSPEED_MIN, szTempChar );	
	sprintf( szTempChar, "%.2f", m_Property.m_fMaxTextureSpeed );
	SetDlgItemText( IDC_EDIT_TEXSPEED_MAX, szTempChar );

	m_TexSpeedSlider1.SetPos( (int)m_Property.m_fMinTextureSpeed );
	m_TexSpeedSlider2.SetPos( (int)m_Property.m_fMaxTextureSpeed );

	sprintf( szTempChar, "%.2f", m_Property.m_vMinSize.x );
	SetDlgItemText( IDC_EDIT_XSIZE_MIN, szTempChar );
	sprintf( szTempChar, "%.2f", m_Property.m_vMaxSize.x );
	SetDlgItemText( IDC_EDIT_XSIZE_MAX, szTempChar );
	sprintf( szTempChar, "%.2f", m_Property.m_vMinSize.y );
	SetDlgItemText( IDC_EDIT_YSIZE_MIN, szTempChar );
	sprintf( szTempChar, "%.2f", m_Property.m_vMaxSize.y );
	SetDlgItemText( IDC_EDIT_YSIZE_MAX, szTempChar );

	sprintf( szTempChar, "%.2f", m_Property.m_fMinLifeTime );
	SetDlgItemText( IDC_EDIT_LIFE_MIN, szTempChar );
	sprintf( szTempChar, "%.2f", m_Property.m_fMaxLifeTime );
	SetDlgItemText( IDC_EDIT_LIFE_MAX, szTempChar );
	sprintf( szTempChar, "%.2f", m_Property.m_fMinRivalTime );
	SetDlgItemText( IDC_EDIT_LIFE_MIN2, szTempChar );
	sprintf( szTempChar, "%.2f", m_Property.m_fMinRivalTime );
	SetDlgItemText( IDC_EDIT_LIFE_MAX2, szTempChar );


	SetDlgItemText( IDC_EDIT_TEXTURE, m_Property.m_szTexture );

	UpdateEffectLIST();


	pView->SetRenderTypeWireframe();
}
void CAroundEffPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedCancel();
}

void CAroundEffPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    ReturnPage();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	pView->SetRenderTypeDefault();
}

void CAroundEffPage::OnBnClickedButtonGet1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( g_vCLOSED_POINT != D3DXVECTOR3(0.0f,0.0f,0.0f) )
	{
		char szTempChar[32] = {0,};
		sprintf( szTempChar, "%.2fx %.2fy %.2fz", g_vCLOSED_POINT.x, g_vCLOSED_POINT.y, g_vCLOSED_POINT.z );
		SetDlgItemText( IDC_EDIT_POS1, szTempChar );
		sprintf( szTempChar, "%s", g_strPICK_BONE.c_str() );
		SetDlgItemText( IDC_EDIT_POS2, szTempChar );
	}
}

void CAroundEffPage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !g_strPICK_BONE.empty() && g_vCLOSED_POINT != D3DXVECTOR3(0.0f,0.0f,0.0f)  )
	{
		SAROUNDEFF_DATA insertData;
		insertData.vPos		   = g_vCLOSED_POINT;
		insertData.strBoneName = g_strPICK_BONE;
		insertData.vNormal	   = g_vCLOSED_NORMAL;

		char szTempChar[32] = {0,};
		GetDlgItemText( IDC_EDIT_MESH_DIST, szTempChar, 32 );
		insertData.fMeshDist = (float)atof( szTempChar );		
		m_pEffChar->InsertAroundEffect( insertData, TRUE );
		UpdateEffectLIST();
	}
}

void CAroundEffPage::UpdateEffectLIST()
{
	m_AroundEffList.DeleteAllItems();
    std::vector<SAROUNDEFF_DATA> vecEffect = m_pEffChar->GetAroundEffect();
	char szTempChar[32] = {0,};
	for( DWORD i = 0; i < vecEffect.size(); i++ )
	{
		sprintf( szTempChar, "%d", i );
		m_AroundEffList.InsertItem( i, szTempChar );
		sprintf( szTempChar, "%.2fx %.2fy %.2fz", vecEffect[i].vPos.x, vecEffect[i].vPos.y, vecEffect[i].vPos.z );
		m_AroundEffList.SetItemText( i, 1, szTempChar );
		sprintf( szTempChar, "%s", vecEffect[i].strBoneName.c_str() );
		m_AroundEffList.SetItemText( i, 2, szTempChar );
	}
}

void CAroundEffPage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_AroundEffList.GetFirstSelectedItemPosition();
	int nIndex = m_AroundEffList.GetNextSelectedItem(pos);
	if ( nIndex==LB_ERR )	return;

	m_pEffChar->DeleteAroundEffect(nIndex);

	UpdateEffectLIST();
}

void CAroundEffPage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();


	char szTempChar[32] = {0,};

	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_SEQUENCE2);
	if( pButton->GetCheck() )	m_Property.m_dwFlag |= USE_SEQUENCE;
	else						m_Property.m_dwFlag &= ~USE_SEQUENCE;

	GetDlgItemText( IDC_EDIT_XSIZE_MIN, szTempChar, 32 );
	m_Property.m_vMinSize.x = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_XSIZE_MAX, szTempChar, 32 );
	m_Property.m_vMaxSize.x = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_YSIZE_MIN, szTempChar, 32 );
	m_Property.m_vMinSize.y = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_YSIZE_MAX, szTempChar, 32 );
	m_Property.m_vMaxSize.y = (float)atof( szTempChar );

	GetDlgItemText( IDC_EDIT_COL2, szTempChar, 32 );
	m_Property.m_dwSequenceCol = atoi( szTempChar );
	GetDlgItemText( IDC_EDIT_ROW2, szTempChar, 32 );
	m_Property.m_dwSequenceRow = atoi( szTempChar );
	GetDlgItemText( IDC_EDIT_TEXTURE, szTempChar, 32 );
	strcpy( m_Property.m_szTexture, szTempChar );

	GetDlgItemText( IDC_EDIT_LIFE_MIN, szTempChar, 32 );
	m_Property.m_fMinLifeTime = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_LIFE_MAX, szTempChar, 32 );
	m_Property.m_fMaxLifeTime = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_LIFE_MIN2, szTempChar, 32 );
	m_Property.m_fMinRivalTime = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_LIFE_MAX2, szTempChar, 32 );
	m_Property.m_fMaxRivalTime = (float)atof( szTempChar );

	GetDlgItemText( IDC_EDIT_ALPHA4, szTempChar, 32 );
	DWORD dwAlpha = atoi( szTempChar );
	DWORD dwColor = m_Property.m_dwColor & 0xffffff;
	m_Property.m_dwColor = dwAlpha<<24;
	m_Property.m_dwColor += dwColor;

	GetDlgItemText( IDC_EDIT_TEXSPEED_MIN, szTempChar, 32 );
	m_Property.m_fMinTextureSpeed = (float)atof( szTempChar );
	GetDlgItemText( IDC_EDIT_TEXSPEED_MAX, szTempChar, 32 );
	m_Property.m_fMaxTextureSpeed = (float)atof( szTempChar );

	m_pEffChar->SetProperty ( &m_Property );
	m_pEffChar->Create ( pView->GetD3dDevice() );

    EffectApply();
}

void CAroundEffPage::OnBnClickedButtonTextureFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_TEXTURE );
		pWnd->SetWindowText( FilePath );
	}
}

void CAroundEffPage::OnBnClickedButtonTextureFind2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem( IDC_EDIT_TEXTURE2 );
		pWnd->SetWindowText( FilePath );
	}
}



void CAroundEffPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int sliderPos1 = m_TexSpeedSlider1.GetPos();
	int sliderPos2 = m_TexSpeedSlider2.GetPos();

	if( sliderPos1 > sliderPos2 )
	{
		m_TexSpeedSlider2.SetPos(sliderPos1);
	}

	sliderPos1 = m_TexSpeedSlider1.GetPos();
	sliderPos2 = m_TexSpeedSlider2.GetPos();

	char szTempChar[32];
	sprintf( szTempChar, "%.2f", (float)sliderPos1 );
	SetDlgItemText( IDC_EDIT_TEXSPEED_MIN, szTempChar );
	sprintf( szTempChar, "%.2f", (float)sliderPos2 );
	SetDlgItemText( IDC_EDIT_TEXSPEED_MAX, szTempChar );

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAroundEffPage::OnBnClickedButtonColor3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwAlpha = (m_Property.m_dwColor&0xff000000);
	int ThisR = int ( (m_Property.m_dwColor&0xff0000)>>16 );
	int ThisG = int ( (m_Property.m_dwColor&0xff00)>>8 );
	int ThisB = int ( m_Property.m_dwColor&0xff );

	CColorPickerDlg ColorPicker ( RGB(ThisR,ThisG,ThisB) );

	if ( IDOK == ColorPicker.DoModal() )
	{
		COLORREF SelectColor = ColorPicker.GetColor ();

		m_Property.m_dwColor = dwAlpha << 24;
		m_Property.m_dwColor += (GetRValue(SelectColor)<<16);
		m_Property.m_dwColor += (GetGValue(SelectColor)<<8);
		m_Property.m_dwColor += GetBValue(SelectColor);
	}
}
