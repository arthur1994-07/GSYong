// TextureSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxTools/Material/NSMaterialSetting.h"

#include "TextureSetDlg.h"


// CTextureSetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTextureSetDlg, CDialog)
CTextureSetDlg::CTextureSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureSetDlg::IDD, pParent)
	, m_pMaterialSetting(NULL)
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CTextureSetDlg::~CTextureSetDlg()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CTextureSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEXDXT, m_LCtrlTexDXT);
}


BEGIN_MESSAGE_MAP(CTextureSetDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_CAST_SHADOW, &CTextureSetDlg::OnBnClickedCheckCastShadow)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TEXDXT, &CTextureSetDlg::OnNMClickListTexdxt)
	ON_BN_CLICKED(IDC_CHECK_CAMERA_COLLISION, &CTextureSetDlg::OnBnClickedCheckCameraCollision)
	ON_BN_CLICKED(IDC_RADIO_DECAL_NULL, &CTextureSetDlg::OnBnClickedRadioDecalNull)
	ON_BN_CLICKED(IDC_RADIO_DECAL_ALPHABLEND, &CTextureSetDlg::OnBnClickedRadioDecalAlphablend)
	ON_BN_CLICKED(IDC_RADIO_DECAL_MULTIPLY, &CTextureSetDlg::OnBnClickedRadioDecalMultiply)
	ON_BN_CLICKED(IDC_RADIO_DECAL_ADD, &CTextureSetDlg::OnBnClickedRadioDecalAdd)
	ON_BN_CLICKED(IDC_RADIO_DECAL_ORDER_1, &CTextureSetDlg::OnBnClickedRadioDecalOrder1)
	ON_BN_CLICKED(IDC_RADIO_DECAL_ORDER_2, &CTextureSetDlg::OnBnClickedRadioDecalOrder2)
	ON_BN_CLICKED(IDC_RADIO_DECAL_ORDER_3, &CTextureSetDlg::OnBnClickedRadioDecalOrder3)
	ON_BN_CLICKED(IDC_RADIO_DECAL_ALPHATEST, &CTextureSetDlg::OnBnClickedRadioDecalAlphatest)
	ON_BN_CLICKED(IDC_RADIO_LIGHTMAP, &CTextureSetDlg::OnBnClickedRadioLightMap)
	ON_BN_CLICKED(IDC_RADIO_AMBIENT_CUBE, &CTextureSetDlg::OnBnClickedRadioAmbientCube)
	ON_BN_CLICKED(IDC_RADIO_VOXEL_COLOR, &CTextureSetDlg::OnBnClickedRadioVoxelColor)	
	ON_BN_CLICKED(IDC_CHECK_CUBEMAP, &CTextureSetDlg::OnBnClickedCheckCubemap)
	ON_EN_CHANGE(IDC_EDIT_CUBEMAP_VALUE, &CTextureSetDlg::OnEnChangeEditCubemapValue)
	ON_BN_CLICKED(IDC_CHECK_CUBEMAP_SPECMAP_MASK, &CTextureSetDlg::OnBnClickedCheckMaskCubeMapSpecularMap)
	ON_EN_CHANGE(IDC_EDIT_SPOTSPEC_POWER, &CTextureSetDlg::OnEnChangeEditSpotSpecPower)
	ON_EN_CHANGE(IDC_EDIT_SPOTSPEC_MULTI, &CTextureSetDlg::OnEnChangeEditSpotSpecMulti)
	ON_BN_CLICKED(IDC_CHECK_NORMALMAP, &CTextureSetDlg::OnBnClickedCheckNormalMap)
	ON_BN_CLICKED(IDC_CHECK_DECAL_NORMAL, &CTextureSetDlg::OnBnClickedCheckDecalNormal)
END_MESSAGE_MAP()


// CTextureSetDlg 메시지 처리기입니다.

BOOL CTextureSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	RECT listRect;
	m_LCtrlTexDXT.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
	int nColWidth[13] = { int(nListWidth*0.35f), 
						int(nListWidth*0.05f),		// CastShadow
						int(nListWidth*0.05f),		// CameraCollision
						int(nListWidth*0.05f),		// LightMap
						int(nListWidth*0.10f),		// Decal
						int(nListWidth*0.05f),		// Order
						int(nListWidth*0.05f),		// Decal Normal
						int(nListWidth*0.05f),		// CubeMap
						int(nListWidth*0.05f),		// CubeMapValue
						int(nListWidth*0.05f),		// SpotSpec Power
						int(nListWidth*0.05f),		// SpotSpec Multi
						int(nListWidth*0.05f),		// Mask CubeMap Specular
						int(nListWidth*0.05f) };	// NormalMap

	m_LCtrlTexDXT.SetExtendedStyle ( m_LCtrlTexDXT.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_LCtrlTexDXT.InsertColumn ( 0, _T("Name"),				LVCFMT_CENTER, nColWidth[0] );
	m_LCtrlTexDXT.InsertColumn ( 1, _T("CastShadow"),		LVCFMT_CENTER, nColWidth[1] );
	m_LCtrlTexDXT.InsertColumn ( 2, _T("CameraCollision"),	LVCFMT_CENTER, nColWidth[2] );
	m_LCtrlTexDXT.InsertColumn ( 3, _T("LightMap"),			LVCFMT_CENTER, nColWidth[3] );
	m_LCtrlTexDXT.InsertColumn ( 4, _T("Decal"),			LVCFMT_CENTER, nColWidth[4] );
	m_LCtrlTexDXT.InsertColumn ( 5, _T("Order"),			LVCFMT_CENTER, nColWidth[5] );
	m_LCtrlTexDXT.InsertColumn ( 6, _T("D-Normal"),			LVCFMT_CENTER, nColWidth[6] );
	m_LCtrlTexDXT.InsertColumn ( 7, _T("CubeMap"),			LVCFMT_CENTER, nColWidth[7] );
	m_LCtrlTexDXT.InsertColumn ( 8, _T("CubeMapV"),			LVCFMT_CENTER, nColWidth[8] );
	m_LCtrlTexDXT.InsertColumn ( 9, _T("SpecPower"),		LVCFMT_CENTER, nColWidth[9] );
	m_LCtrlTexDXT.InsertColumn ( 10, _T("SpecMulti"),		LVCFMT_CENTER, nColWidth[10] );
	m_LCtrlTexDXT.InsertColumn ( 11, _T("Mask"),			LVCFMT_CENTER, nColWidth[11] );
	m_LCtrlTexDXT.InsertColumn ( 12, _T("NormalMap"),		LVCFMT_CENTER, nColWidth[12] );

	CButton* pButton = (CButton*)GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
	pButton->EnableWindow( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_EDIT_SPOTSPEC_POWER );
	pButton->EnableWindow( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_EDIT_SPOTSPEC_MULTI );
	pButton->EnableWindow( FALSE );

	ReSetListCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTextureSetDlg::ReSetListCtrl()
{
	m_LCtrlTexDXT.DeleteAllItems ();

	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for ( size_t i=0; i<m_pMaterialSetting->MaterialSize(); ++i )
	{
		if( m_pMaterialSetting->GetMaterial( i, strName, sSetting ) )
		{
			m_LCtrlTexDXT.InsertItem( i, strName.c_str() );

			if ( sSetting.m_bCastShadow )
				m_LCtrlTexDXT.SetItemText( i, 1, "TRUE" );
			else
				m_LCtrlTexDXT.SetItemText( i, 1, "FALSE" );

			if ( sSetting.m_bCameraCollision )
				m_LCtrlTexDXT.SetItemText( i, 2, "TRUE" );
			else
				m_LCtrlTexDXT.SetItemText( i, 2, "FALSE" );

			switch ( sSetting.m_emUnLightMapType )
			{
			case NSMaterialSetting::RENDER_TYPE_AMBIENTCUBE:
				m_LCtrlTexDXT.SetItemText( i, 3, _T("AC") );
				break;
			case NSMaterialSetting::RENDER_TYPE_VOXELCOLOR:
				m_LCtrlTexDXT.SetItemText( i, 3, _T("VC") );
				break;
			case NSMaterialSetting::RENDER_TYPE_LIGHTMAP:
			default:
				m_LCtrlTexDXT.SetItemText( i, 3, _T("LM") );
				break;
			};

			switch ( sSetting.m_emDecalType )
			{
			case NSMaterialSetting::DECAL_TYPE_ALPHABLEND:
				m_LCtrlTexDXT.SetItemText( i, 4, _T("AlphaBlend") );
				break;
			case NSMaterialSetting::DECAL_TYPE_ALPHATEST:
				m_LCtrlTexDXT.SetItemText( i, 4, _T("AlphaTest") );
				break;
			case NSMaterialSetting::DECAL_TYPE_MULTIPLY:
				m_LCtrlTexDXT.SetItemText( i, 4, _T("Multiply") );
				break;
			case NSMaterialSetting::DECAL_TYPE_ADD:
				m_LCtrlTexDXT.SetItemText( i, 4, _T("Add") );
				break;
			case NSMaterialSetting::DECAL_TYPE_NULL:
			default:
				m_LCtrlTexDXT.SetItemText( i, 4, _T("FALSE") );
				break;
			};

			switch ( sSetting.m_nDecalOrder )
			{
			case 2:
				m_LCtrlTexDXT.SetItemText( i, 5, _T("2") );
				break;
			case 3:
				m_LCtrlTexDXT.SetItemText( i, 5, _T("3") );
				break;
			case 1:
			default:
				m_LCtrlTexDXT.SetItemText( i, 5, _T("1") );
				break;
			};

			if ( sSetting.m_bDecalNormal )
				m_LCtrlTexDXT.SetItemText( i, 6, "TRUE" );
			else
				m_LCtrlTexDXT.SetItemText( i, 6, "FALSE" );

			if ( sSetting.m_bCubeMap )
				m_LCtrlTexDXT.SetItemText( i, 7, "TRUE" );
			else
				m_LCtrlTexDXT.SetItemText( i, 7, "FALSE" );

			char szValue[256];
			_gcvt ( sSetting.m_fCubeMapValue, 8, szValue );
			m_LCtrlTexDXT.SetItemText( i, 8, szValue );

			_gcvt ( sSetting.m_fSpotSpecPower, 9, szValue );
			m_LCtrlTexDXT.SetItemText( i, 9, szValue );

			_gcvt ( sSetting.m_fSpotSpecMulti, 10, szValue );
			m_LCtrlTexDXT.SetItemText( i, 10, szValue );

			if ( sSetting.m_bMaskCubeMapSpecularMap )
				m_LCtrlTexDXT.SetItemText( i, 11, "TRUE" );
			else
				m_LCtrlTexDXT.SetItemText( i, 11, "FALSE" );

			if ( sSetting.m_bNormalMap )
				m_LCtrlTexDXT.SetItemText( i, 12, "TRUE" );
			else
				m_LCtrlTexDXT.SetItemText( i, 12, "FALSE" );
		}
	}
}

void CTextureSetDlg::OnBnClickedCheckCastShadow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_CAST_SHADOW );
		sSetting.m_bCastShadow = pButton->GetCheck() ? TRUE : FALSE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::SetFocus( CListCtrl& listReplace, int nIndex )
{
	m_LCtrlTexDXT.SetSelectionMark( nIndex );								// 실제 선택됨.
	m_LCtrlTexDXT.SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED);	// 회색으로 표시됨.
	m_LCtrlTexDXT.EnsureVisible( nIndex, FALSE );							// 스크롤 밑에 있으면 옮겨줌.
	m_LCtrlTexDXT.SetFocus();												// 푸른색으로 표시됨.
}

void CTextureSetDlg::OnNMClickListTexdxt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	int			nItem = m_LCtrlTexDXT.GetNextSelectedItem ( pos );	

	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	if ( !m_pMaterialSetting->GetMaterial( nItem, strName, sSetting ) )
	{
		AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
		return;
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_CAST_SHADOW );
	pButton->SetCheck( sSetting.m_bCastShadow );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_CAMERA_COLLISION );
	pButton->SetCheck( sSetting.m_bCameraCollision );



	// UNLIGHTMAP_TYPE
	pButton = (CButton*)GetDlgItem( IDC_RADIO_LIGHTMAP );
	if ( sSetting.m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_LIGHTMAP )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_AMBIENT_CUBE );
	if ( sSetting.m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_AMBIENTCUBE )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_VOXEL_COLOR );
	if ( sSetting.m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_VOXELCOLOR )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );


	// Decal
	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_NULL );
	if ( sSetting.m_emDecalType == NSMaterialSetting::DECAL_TYPE_NULL )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_ALPHABLEND );
	if ( sSetting.m_emDecalType == NSMaterialSetting::DECAL_TYPE_ALPHABLEND )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_ALPHATEST );
	if ( sSetting.m_emDecalType == NSMaterialSetting::DECAL_TYPE_ALPHATEST )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_MULTIPLY );
	if ( sSetting.m_emDecalType == NSMaterialSetting::DECAL_TYPE_MULTIPLY )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_ADD );
	if ( sSetting.m_emDecalType == NSMaterialSetting::DECAL_TYPE_ADD )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );


	// Decal Order
	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_ORDER_1 );
	if ( sSetting.m_nDecalOrder == 1 )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_ORDER_2 );
	if ( sSetting.m_nDecalOrder == 2 )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_RADIO_DECAL_ORDER_3 );
	if ( sSetting.m_nDecalOrder == 3 )
		pButton->SetCheck( TRUE );
	else
		pButton->SetCheck( FALSE );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_DECAL_NORMAL );
	pButton->SetCheck( sSetting.m_bDecalNormal );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_CUBEMAP );
	pButton->SetCheck( sSetting.m_bCubeMap );

	char szValue[256];
	pButton = (CButton*)GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
	_gcvt ( sSetting.m_fCubeMapValue, 7, szValue );
	pButton->SetWindowText( szValue );

	if ( sSetting.m_bCubeMap )
	{
		pButton = (CButton*)GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
		pButton->EnableWindow( TRUE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
		pButton->EnableWindow( FALSE );
	}

	pButton = (CButton*)GetDlgItem( IDC_EDIT_SPOTSPEC_POWER );
	pButton->EnableWindow( TRUE );
	pButton = (CButton*)GetDlgItem( IDC_EDIT_SPOTSPEC_MULTI );
	pButton->EnableWindow( TRUE );

	pButton = (CButton*)GetDlgItem( IDC_EDIT_SPOTSPEC_POWER );
	_gcvt ( sSetting.m_fSpotSpecPower, 7, szValue );
	pButton->SetWindowText( szValue );

	pButton = (CButton*)GetDlgItem( IDC_EDIT_SPOTSPEC_MULTI );
	_gcvt ( sSetting.m_fSpotSpecMulti, 7, szValue );
	pButton->SetWindowText( szValue );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_CUBEMAP_SPECMAP_MASK );
	pButton->SetCheck( sSetting.m_bMaskCubeMapSpecularMap );

	pButton = (CButton*)GetDlgItem( IDC_CHECK_NORMALMAP );
	pButton->SetCheck( sSetting.m_bNormalMap );
}

void CTextureSetDlg::OnBnClickedCheckCameraCollision()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_CAMERA_COLLISION );
		sSetting.m_bCameraCollision = pButton->GetCheck() ? TRUE : FALSE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalNull()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emDecalType = NSMaterialSetting::DECAL_TYPE_NULL;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalAlphablend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emDecalType = NSMaterialSetting::DECAL_TYPE_ALPHABLEND;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalAlphatest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emDecalType = NSMaterialSetting::DECAL_TYPE_ALPHATEST;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalMultiply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emDecalType = NSMaterialSetting::DECAL_TYPE_MULTIPLY;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emDecalType = NSMaterialSetting::DECAL_TYPE_ADD;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalOrder1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_nDecalOrder = 1;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalOrder2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_nDecalOrder = 2;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioDecalOrder3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_nDecalOrder = 3;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedCheckDecalNormal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_DECAL_NORMAL );
		sSetting.m_bDecalNormal = pButton->GetCheck() ? TRUE : FALSE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioLightMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emUnLightMapType = NSMaterialSetting::RENDER_TYPE_LIGHTMAP;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioAmbientCube()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emUnLightMapType = NSMaterialSetting::RENDER_TYPE_AMBIENTCUBE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedRadioVoxelColor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_emUnLightMapType = NSMaterialSetting::RENDER_TYPE_VOXELCOLOR;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedCheckCubemap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_CUBEMAP );
		sSetting.m_bCubeMap = pButton->GetCheck() ? TRUE : FALSE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_CUBEMAP );
	if ( pButton->GetCheck() )
	{
		pButton = (CButton*)GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
		pButton->EnableWindow( TRUE );
	}
	else
	{
		pButton = (CButton*)GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
		pButton->EnableWindow( FALSE );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnEnChangeEditCubemapValue()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_CUBEMAP_VALUE );
	pWnd->GetWindowText( strText );

	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_fCubeMapValue = (float)atof (strText.GetString());

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	pWnd->SetFocus();

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnEnChangeEditSpotSpecPower()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_SPOTSPEC_POWER );
	pWnd->GetWindowText( strText );

	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_fSpotSpecPower = (float)atof (strText.GetString());

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	pWnd->SetFocus();

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnEnChangeEditSpotSpecMulti()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}

	CString strText;
	CWnd* pWnd = GetDlgItem( IDC_EDIT_SPOTSPEC_MULTI );
	pWnd->GetWindowText( strText );

	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		sSetting.m_fSpotSpecMulti = (float)atof (strText.GetString());

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	pWnd->SetFocus();

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedCheckMaskCubeMapSpecularMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_CUBEMAP_SPECMAP_MASK );
		sSetting.m_bMaskCubeMapSpecularMap = pButton->GetCheck() ? TRUE : FALSE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}

void CTextureSetDlg::OnBnClickedCheckNormalMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCount = m_LCtrlTexDXT.GetSelectedCount();
	if( !nCount )
		return;

	POSITION	pos   = m_LCtrlTexDXT.GetFirstSelectedItemPosition ();	
	if( pos == NULL )
		return;

	INT* nDeleteList = new INT[ nCount ];
	INT i = 0;
	while( pos )
	{
		nDeleteList[ i++ ] = m_LCtrlTexDXT.GetNextSelectedItem( pos );
	}


	TSTRING strName;
	NSMaterialSetting::SETTING sSetting;
	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		if ( !m_pMaterialSetting->GetMaterial( nCurIndex, strName, sSetting ) )
		{
			AfxMessageBox( _T("Setting 정보가 정상이 아닙니다.") );
			return;
		}

		CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_NORMALMAP );
		sSetting.m_bNormalMap = pButton->GetCheck() ? TRUE : FALSE;

		m_pMaterialSetting->SetMaterialSetting( strName, sSetting );
	}

	ReSetListCtrl();

	for( i=0; i<nCount; ++i )
	{
		INT nCurIndex = nDeleteList[ i ];

		// 선택후 위치로 이동
		SetFocus( m_LCtrlTexDXT, nCurIndex );
	}

	SAFE_DELETE_ARRAY( nDeleteList );
}
