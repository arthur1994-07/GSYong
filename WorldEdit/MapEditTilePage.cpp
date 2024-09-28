// WeatherPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"

#include "../EngineLib/DxLand/DxSetLandMan.h"

#include "../EngineLib/DxTools/TextureManager.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MapEditTilePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMapEditTilePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapEditTilePage, CPropertyPage)
CMapEditTilePage::CMapEditTilePage()
	: CPropertyPage(CMapEditTilePage::IDD),
	m_bDlgInit(FALSE)
{
}

CMapEditTilePage::~CMapEditTilePage()
{
}

void CMapEditTilePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_listData);
}


BEGIN_MESSAGE_MAP(CMapEditTilePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_FILE_SAVE, OnBnClickedButtonFileSave)
	ON_BN_CLICKED(IDC_BUTTON_FILE_LOAD, OnBnClickedButtonFileLoad)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_1, OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_2, OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_3, OnBnClickedButtonColor3)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_4, OnBnClickedButtonColor4)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_5, OnBnClickedButtonColor5)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_6, OnBnClickedButtonColor6)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_7, OnBnClickedButtonColor7)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_8, OnBnClickedButtonColor8)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_9, OnBnClickedButtonColor9)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_10, OnBnClickedButtonColor10)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_11, OnBnClickedButtonColor11)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_12, OnBnClickedButtonColor12)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_13, OnBnClickedButtonColor13)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_14, OnBnClickedButtonColor14)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_CHANGE, OnBnClickedButtonColorChange)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DATA, OnNMClickListData)
	ON_EN_CHANGE(IDC_EDIT_DETAIL_SCALE, OnEnChangeEditDetailScale)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL_TEX, OnBnClickedButtonDetailTex)
	ON_BN_CLICKED(IDC_CHECK_TILE, OnBnClickedCheckTile)
	ON_BN_CLICKED(IDC_CHECK_COLOR, OnBnClickedCheckColor)
END_MESSAGE_MAP()


// CMapEditTilePage 메시지 처리기입니다.

BOOL CMapEditTilePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//CWnd	*pWnd;
	//char	szValue[256];

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_sOldProperty	= m_sProperty	= DxFogMan::GetInstance().GetProperty();
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	m_bDlgInit = TRUE;

	m_listData.SetExtendedStyle ( m_listData.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_listData.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	int			ColumnCount = 2;	
	char		*ColumnName[2] = { "Name", "Detail Map" };
	int			ColumnWidthPercent[2] = { 40, 60 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_listData.InsertColumn (i, &lvcolumn);		
	}
	
	cClrCOLOR[0].SubclassDlgItem ( IDC_BUTTON_COLOR_1, this );
	cClrCOLOR[1].SubclassDlgItem ( IDC_BUTTON_COLOR_2, this );
	cClrCOLOR[2].SubclassDlgItem ( IDC_BUTTON_COLOR_3, this );
	cClrCOLOR[3].SubclassDlgItem ( IDC_BUTTON_COLOR_4, this );
	cClrCOLOR[4].SubclassDlgItem ( IDC_BUTTON_COLOR_5, this );
	cClrCOLOR[5].SubclassDlgItem ( IDC_BUTTON_COLOR_6, this );
	cClrCOLOR[6].SubclassDlgItem ( IDC_BUTTON_COLOR_7, this );
	cClrCOLOR[7].SubclassDlgItem ( IDC_BUTTON_COLOR_8, this );
	cClrCOLOR[8].SubclassDlgItem ( IDC_BUTTON_COLOR_9, this );
	cClrCOLOR[9].SubclassDlgItem ( IDC_BUTTON_COLOR_10, this );
	cClrCOLOR[10].SubclassDlgItem ( IDC_BUTTON_COLOR_11, this );
	cClrCOLOR[11].SubclassDlgItem ( IDC_BUTTON_COLOR_12, this );
	cClrCOLOR[12].SubclassDlgItem ( IDC_BUTTON_COLOR_13, this );
	cClrCOLOR[13].SubclassDlgItem ( IDC_BUTTON_COLOR_14, this );

	// 초기화
	for ( int i=0; i<14; ++i )
	{
		cClrCOLOR[i].SetWindowText ( "0" );
	}

	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_TILE );
	if ( pView->GetMapEditMan()->m_bTILE_DRAW )		pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );

	pButton = (CButton*)GetDlgItem ( IDC_CHECK_COLOR );
	if ( pView->GetMapEditMan()->m_bCOLOR_DRAW )	pButton->SetCheck ( TRUE );
	else											pButton->SetCheck ( FALSE );
	

	std::string cName;
	CString strText;
	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FILE_TILES );
	pWnd->GetWindowText ( strText );
	cName = strText;

	int nSize = cName.size();
	if ( nSize==0 )
	{
		pView->GetMapEditMan()->NewTileData ();
		pWnd = GetDlgItem ( IDC_EDIT_FILE_TILES );
		pWnd->SetWindowText ( pView->GetMapEditMan()->m_szDefaultFileName.c_str() );
	}

	ReSetColorButton ();

	ReSetListCtrl ();
	ReSetAllEditBox ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapEditTilePage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// 다시 초기화를 시킨다.
}

void CMapEditTilePage::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditTilePage::OnBnClickedButtonInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	CString strText;
	CWnd*	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( strText );

	pView->GetMapEditMan()->SetTileData ( strText.GetString() );

	ReSetListCtrl ();
}

void CMapEditTilePage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	CString strText;
	CWnd*	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( strText );

	pView->GetMapEditMan()->DelTileData ( strText.GetString() );

	ReSetListCtrl ();
}

void CMapEditTilePage::ReSetListCtrl ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxMapEditMan::TILING_EDIT_MAP_ITER iter = pView->GetMapEditMan()->m_mapTiles.begin();

	m_listData.DeleteAllItems ();

	int Amount = 0;

	for ( ; iter!=pView->GetMapEditMan()->m_mapTiles.end(); ++iter )
	{
		m_listData.InsertItem ( Amount, (*iter).first.c_str() );
		m_listData.SetItemText ( Amount, 1, (*iter).second.cName.c_str() );

		Amount++;
	}
}

void CMapEditTilePage::OnBnClickedButtonFileSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Tile List Data (*.tld)|*.tld";
	CFileDialog dlg ( FALSE, ".tel", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = pView->GetMapEditMan()->GetPathData();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_FILE_TILES );
		pWnd->SetWindowText ( FilePath );

		FilePath = StrInitDir + FilePath;

		strcpy ( cName, FilePath.GetString() );

		pView->GetMapEditMan()->SaveTileData ( cName );
	} // if ( dlg.DoModal() == IDOK )
}

void CMapEditTilePage::OnBnClickedButtonFileLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Tile List Data (*.tld)|*.tld";
	CFileDialog dlg ( TRUE, ".tel", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = pView->GetMapEditMan()->GetPathData();
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_FILE_TILES );
		pWnd->SetWindowText ( FilePath );

		FilePath = StrInitDir + FilePath;

		strcpy ( cName, FilePath.GetString() );

		pView->GetMapEditMan()->LoadTileData ( cName );
	} // if ( dlg.DoModal() == IDOK )

	ReSetListCtrl ();
}

void CMapEditTilePage::OnBnClickedButtonColor1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 0;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 1;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 2;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 3;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 4;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 5;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 6;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 7;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 8;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 9;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 10;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 11;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 12;

	ReSetColorButton ();
}

void CMapEditTilePage::OnBnClickedButtonColor14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_nColor_EDIT = 13;

	ReSetColorButton ();
}

void CMapEditTilePage::ReSetColorButton ()
{
	COLORREF cColorREF;
	COLORREF cColorREF1;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	// 초기화
	for ( int i=0; i<14; ++i )
	{
		cColorREF = RGB( pView->GetMapEditMan()->m_cColor_EDIT[i].r*255.f,
						pView->GetMapEditMan()->m_cColor_EDIT[i].g*255.f,
						pView->GetMapEditMan()->m_cColor_EDIT[i].b*255.f );

		cClrCOLOR[i].SetColour ( cColorREF, cColorREF );
	}

	// 셋팅
	int nNUM = pView->GetMapEditMan()->m_nColor_EDIT;

	cColorREF = RGB( pView->GetMapEditMan()->m_cColor_EDIT[nNUM].r*255.f,
						pView->GetMapEditMan()->m_cColor_EDIT[nNUM].g*255.f,
						pView->GetMapEditMan()->m_cColor_EDIT[nNUM].b*255.f );

	cColorREF1 = RGB(255, 255, 255);
	if ( pView->GetMapEditMan()->m_cColor_EDIT[nNUM].b > 0.5f )	cColorREF1 = RGB(0, 0, 0);
	else														cColorREF1 = RGB(255, 255, 255);

	cClrCOLOR[nNUM].SetColour ( cColorREF1, cColorREF );
}

void CMapEditTilePage::OnBnClickedButtonColorChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	D3DXCOLOR	cColor;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	cColor = pView->GetMapEditMan()->m_cColor_EDIT[ pView->GetMapEditMan()->m_nColor_EDIT ];

	int R,G,B;
	R = (int)(cColor.r*255.f);
	G = (int)(cColor.g*255.f);
	B = (int)(cColor.b*255.f);
	COLORREF Color = RGB ( R, G, B );
	CColorPickerDlg colorDlg ( Color );

	if ( IDOK == colorDlg.DoModal () )
	{
		COLORREF retColor;
		retColor = colorDlg.GetColor ();

		int	R,G,B;
		R = GetRValue ( retColor );
		G = GetGValue ( retColor );
		B = GetBValue ( retColor );

		cColor.a = 1.f;
		cColor.r = (float)R/255.f;
		cColor.g = (float)G/255.f;
		cColor.b = (float)B/255.f;

		pView->GetMapEditMan()->m_cColor_EDIT[ pView->GetMapEditMan()->m_nColor_EDIT ] = cColor;

		ReSetColorButton ();
	}
}

void CMapEditTilePage::OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	char cName[256];
	CString strTex;

	POSITION	pos = m_listData.GetFirstSelectedItemPosition ();	
	int nItem = m_listData.GetNextSelectedItem ( pos );	

	strTex = m_listData.GetItemText ( nItem, 0 );
	strcpy ( cName, strTex.GetString() );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_szTileName_USE = cName;						// 현재 사용할 것 셋팅

	ReSetAllEditBox ();
}

void CMapEditTilePage::ReSetAllEditBox ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxMapEditMan::TILING_EDIT_MAP& maptiles	= pView->GetMapEditMan()->m_mapTiles;
	DxMapEditMan::TILING_EDIT_MAP_ITER iter = maptiles.find ( pView->GetMapEditMan()->m_szTileName_USE );

	if ( iter != maptiles.end() )
	{
		CWnd*	pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( (*iter).first.c_str() );

		pWnd = GetDlgItem ( IDC_EDIT_DETAIL_TEX );
		pWnd->SetWindowText ( (*iter).second.cName.c_str() );

		char szValue[256];
		pWnd = GetDlgItem ( IDC_EDIT_DETAIL_SCALE );
		_gcvt ( (*iter).second.fScale, 7, szValue );
		pWnd->SetWindowText ( szValue );
	}
}

void CMapEditTilePage::OnEnChangeEditDetailScale()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxMapEditMan::TILING_EDIT_MAP& maptiles	= pView->GetMapEditMan()->m_mapTiles;
	DxMapEditMan::TILING_EDIT_MAP_ITER iter = maptiles.find ( pView->GetMapEditMan()->m_szTileName_USE );

	if ( iter != maptiles.end() )
	{
		CString strTex;
		CWnd*	pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_DETAIL_SCALE );
		pWnd->GetWindowText ( strTex );
		(*iter).second.fScale = (float)atof ( strTex.GetString() );
	}
}

void CMapEditTilePage::OnBnClickedButtonDetailTex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_DETAIL_TEX );
		pWnd->SetWindowText ( FilePath );


		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		DxMapEditMan::TILING_EDIT_MAP& maptiles	= pView->GetMapEditMan()->m_mapTiles;
		DxMapEditMan::TILING_EDIT_MAP_ITER iter = maptiles.find ( pView->GetMapEditMan()->m_szTileName_USE );

		if ( iter != maptiles.end() )	(*iter).second.cName = FilePath.GetString();
	}
}

void CMapEditTilePage::OnBnClickedCheckTile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_TILE );
	if ( pButton->GetCheck() )	pView->GetMapEditMan()->m_bTILE_DRAW = TRUE;
	else						pView->GetMapEditMan()->m_bTILE_DRAW = FALSE;
}

void CMapEditTilePage::OnBnClickedCheckColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CButton* pButton;
	pButton = (CButton*)GetDlgItem ( IDC_CHECK_COLOR );
	if ( pButton->GetCheck() )	pView->GetMapEditMan()->m_bCOLOR_DRAW = TRUE;
	else						pView->GetMapEditMan()->m_bCOLOR_DRAW = FALSE;

}
