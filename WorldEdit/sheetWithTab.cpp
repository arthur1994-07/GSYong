#include "stdafx.h"
#include "resource.h"

#include "sheetWithTab.h"
#include "State/WorldStateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_strEditSheet[EDITSHEET_NUM] =
{
	"FRAME EDIT (����)",				//	0
	"TILING EDIT (Ÿ�ϸ�)",				//	1
	"CLOTH EDIT (��ȿ��)",				//	2
	"LIGHT EDIT (��)",					//	3
	"DOT3 EDIT (��������)",			//	4
	"RIVER EDIT (��,�ٴ�)",				//	5
	"SHADOW EDIT (�׸��� ȿ��)",		//	6
	"FUR EDIT (�� ȿ��)",				//	7
	"SPECULAR EDIT (����ŧ��)",			//	8
	"RAINPOINT EDIT (����� ȿ��)",		//	9
	"GRASS EDIT (�ܵ�)",				//	10
	"SPECULAR2 EDIT (����ŧ��2)",		//	11
	"DIRECTION ( ���� )",				//	12
	"FRAME STATE (����Ӽ�����)",		//	13
	"REPLACEFRAME",						//	14
	"WEATHER STATE (����)",				//	15
	"NATURE EDIT (�ڿ� ����)",			//	16
	"WATERLIGHT EDIT (���� ����)",		//	17
	"RENDERSTATE EDIT (��������)",		//	18
	"CAMERA EDIT (ī�޶� ���� )",		//  19
	"SOUND EDIT (���� ���� )",		//  20
	"EFFECT EDIT (ȿ�� ���� )",			//  21
	"NATURE EDIT (�ڿ� ����(Ȧ������))",//	22
	"SPORE EDIT (����)",				//	23
	"SKINOBJ EDIT (��Ų������Ʈ)",		//	24
	"LAND-GATE EDIT (���Ա�)",			//	25
	"REFLECT EDIT (�ܼ��ݻ�)",			//	26
	"BGM EDIT ( ������� )",			//	27
	"MULTITEX EDIT (��Ƽ�ؽ���)",		//	28
	"NEON EDIT ( �׿� )",				//	29
	"SPECREFLECT EDIT ( ����ŧ��-�ݻ�)",//	30
	"LIGHTMAP EDIT ( ����Ʈ �� )",		//	31
	"GLOW EDIT ( �۷ο� ȿ�� )",		//	32
	"FOG EDIT ( ���� ���� )",			//	33
	"MAP EDIT ( �� ���� )",				//	34
	"PIECE EDIT ( �������ϼ��� )",		//	35
	"PIECE SINGLE EFF EDIT",			//	36
	"PIECE TEXTURE EFF EDIT",			//	37
	"PIECE OBJECT MANAGER",				//	38
	"PIECE MORPH EDIT",					//	39
    "PIECE POINT EDIT",					//	40
	"PIECE MESH STATE EDIT",		    //	41
	"CAMCOLL EDIT (ī�޶� �浹)",	    //	42
    "SKY BOX MANAGER",	                //	43
	"WATER SURF (�ĵ� ����)",           //	44
	"MAKE MINIMAP",						//	45
	"OCCLUSION CULLING",				//	46
	_T("PREFAB EDIT"),					//	47
	_T("CUBE_MAP"),						//	48
	_T("SHADOW_LIGHT (�׸��ڶ���Ʈ)"),	//	49
};

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0),
	 m_PrevPage(FRAMEPAGEINDEX)
{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_FramePage.SetSheetTab ( this );			//0
	AddPage(&m_FramePage);

	m_TilingPage.SetSheetTab ( this );			//1
	AddPage(&m_TilingPage);

	m_ClothPage.SetSheetTab ( this );			//2
	AddPage(&m_ClothPage);

	m_LightPage.SetSheetTab ( this );			//3
	AddPage(&m_LightPage);

	m_Dot3Page.SetSheetTab ( this );			//4
	AddPage(&m_Dot3Page);

	m_RiverPage.SetSheetTab ( this );			//5
	AddPage(&m_RiverPage);

	m_ShadowPage.SetSheetTab ( this );			//6
	AddPage(&m_ShadowPage);

	m_FurPage.SetSheetTab ( this );				//7
	AddPage(&m_FurPage);

	m_SpecularPage.SetSheetTab ( this );		//8
	AddPage(&m_SpecularPage);

	m_RainPointPage.SetSheetTab ( this );		//9
	AddPage(&m_RainPointPage);

	m_GrassPage.SetSheetTab ( this );			//10
	AddPage(&m_GrassPage);

	m_Specular2Page.SetSheetTab ( this );		//11
	AddPage(&m_Specular2Page);

	m_DirectPage.SetSheetTab ( this );			//12
	AddPage(&m_DirectPage);

	m_FrameStatePage.SetSheetTab ( this );		//13
	AddPage(&m_FrameStatePage);

	m_ReplaceFramePage.SetSheetTab ( this );	//14
	AddPage(&m_ReplaceFramePage);

	m_WeatherPage.SetSheetTab ( this );			//15
	AddPage(&m_WeatherPage);

	m_NaturePage.SetSheetTab ( this );			//16
	AddPage(&m_NaturePage);

	m_WaterLightPage.SetSheetTab ( this );		//17
	AddPage(&m_WaterLightPage);

	m_RenderStatePage.SetSheetTab ( this );		//18
	AddPage(&m_RenderStatePage);

	m_CameraPage.SetSheetTab ( this );			//19
	AddPage(&m_CameraPage);

	m_SoundPage.SetSheetTab ( this );			//20
	AddPage(&m_SoundPage);

	m_SingleEffPage.SetSheetTab ( this );		//21
	AddPage(&m_SingleEffPage);

	m_NaturePageS.SetSheetTab ( this );			//22
	AddPage(&m_NaturePageS);

	m_SporePage.SetSheetTab ( this );			//23
	AddPage(&m_SporePage);

	m_LandSkinObj.SetSheetTab ( this );			//24
	AddPage(&m_LandSkinObj);

	m_LandGatePage.SetSheetTab ( this );		//25
	AddPage(&m_LandGatePage);

	m_ReflectPage.SetSheetTab ( this );			//26
	AddPage(&m_ReflectPage);

	m_BgmEditPage.SetSheetTab ( this );			//27
	AddPage(&m_BgmEditPage);

	m_MultiTexPage.SetSheetTab ( this );		//28
	AddPage(&m_MultiTexPage);

	m_NeonPage.SetSheetTab ( this );			//29
	AddPage(&m_NeonPage);

	m_SpecReflectPage.SetSheetTab ( this );		//30
	AddPage(&m_SpecReflectPage);

	m_LightMapPage.SetSheetTab ( this );		//31
	AddPage(&m_LightMapPage);

	m_GlowPage.SetSheetTab ( this );			//32
	AddPage(&m_GlowPage);

	m_FogPage.SetSheetTab ( this );				//33
	AddPage(&m_FogPage);

	m_MapEditPage.SetSheetTab ( this );			//34
	AddPage(&m_MapEditPage);

	m_StaticMeshCreate.SetSheetTab ( this );	//35
	AddPage(&m_StaticMeshCreate);

	m_PieceSingleEffPage.SetSheetTab( this );	//36
	AddPage(&m_PieceSingleEffPage);

	m_PieceTexEffPage.SetSheetTab( this );		//37
	AddPage(&m_PieceTexEffPage);

	m_PieceManagerPage.SetSheetTab( this );		//38
	AddPage(&m_PieceManagerPage);

	m_PieceMorphPage.SetSheetTab( this );		//39
	AddPage(&m_PieceMorphPage);

    m_PiecePointPage.SetSheetTab( this );		//40
	AddPage(&m_PiecePointPage);

	m_PieceMeshStatePage.SetSheetTab( this );	//41
	AddPage(&m_PieceMeshStatePage);

	m_CamcollPage.SetSheetTab( this );	     	//42
	AddPage(&m_CamcollPage);

    m_SkyBoxManPage.SetSheetTab( this );		//43
	AddPage(&m_SkyBoxManPage);

	m_WaterSurfPage.SetSheetTab( this );		//44
	AddPage(&m_WaterSurfPage);

	m_MiniMapPage.SetSheetTab( this );			//45
	AddPage(&m_MiniMapPage);

	m_OcclusionPage.SetSheetTab( this );		//46
	AddPage(&m_OcclusionPage);

	m_PrefabPage.SetSheetTab( this );			//47
	AddPage(&m_PrefabPage);

	m_CubeMapPage.SetSheetTab( this );			//48
	AddPage(&m_CubeMapPage);

	m_ShadowLightPage.SetSheetTab( this );		//49
	AddPage(&m_ShadowLightPage);

	m_ClothForMaterialPage.SetSheetTab( this );	//50
	AddPage(&m_ClothForMaterialPage);
}

CsheetWithTab::~CsheetWithTab()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTab::OnInitDialog() 
{
	HWND hTabWnd;
	CRect rectTabCtrl;

	if( m_Rect.right )
	{
		// resize the tab control
		hTabWnd = (HWND)::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
		ASSERT(hTabWnd != NULL);
		::GetWindowRect(hTabWnd, &rectTabCtrl);  // Tab-control rectangle (screen coordinates)
		ScreenToClient(rectTabCtrl);

		//	Note : ���� �������� ���� ������ �������� ������ �ʿ� �����Ƿ� �����Ѵ�.
		//
		// Is tab-control wider than specified width.
		if( !m_bNoTabs && rectTabCtrl.right > (m_Rect.Width() - 3) )
		{
			rectTabCtrl.right = m_Rect.Width() - 3; // Set new right position
		}

		// The tab-control does only need to show tabs
		rectTabCtrl.bottom = 32; // Height for tabs

		// move tab control
		::MoveWindow( hTabWnd, 0, m_iDistanceFromTop, rectTabCtrl.Width(), rectTabCtrl.bottom, TRUE );

		BOOL bResult = (BOOL)Default();          // Process messages

		// If propertysheet m_bNoTabs = TRUE hide the tabs
		if( m_bNoTabs )
		{
			::ShowWindow(hTabWnd, SW_HIDE);
			::EnableWindow(hTabWnd, FALSE);
		}

		// change tab style if scrolling tabs desired (stacked tabs are default)
		if (!m_bStacked)
		{
			hTabWnd = (HWND)SendMessage(PSM_GETTABCONTROL);
			if (hTabWnd != NULL)
			{
				CWnd::ModifyStyle(hTabWnd, TCS_MULTILINE, TCS_SINGLELINE, 0);
			}
		}

		// remove standard buttons 
		for (int i = 0; i < 4; i++)
		{
			HWND hWnd = ::GetDlgItem(m_hWnd, module_piButtons[i]);
			if (hWnd != NULL)
			{
				::ShowWindow(hWnd, SW_HIDE);
				::EnableWindow(hWnd, FALSE);
			}
		}

		// Move the propertysheet to specified position
		MoveWindow( m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom /*- cyDiff*/ );

		return bResult;
	}

	return CPropertySheet::OnInitDialog();
}



BEGIN_MESSAGE_MAP(CsheetWithTab, CPropertySheet)
	//{{AFX_MSG_MAP(CsheetWithTab)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



HBRUSH CsheetWithTab::CtlColor(CDC* pDc, UINT uCtlColor) 
{
   CWnd*    pwndParent;   // Parent window to property-sheet
   CDC*     pdcParent;    // Parent dc
   COLORREF color;        // color on parent dc           

   pwndParent = GetParent();
   ASSERT( pwndParent->IsKindOf( RUNTIME_CLASS( CWnd ) ) ); 

   pdcParent = pwndParent->GetDC();

   color = pdcParent->GetBkColor();    // Get parent color

   pwndParent->ReleaseDC( pdcParent );

   CBrush brush( color );              // Make a brush
	
	return (HBRUSH)brush;
}

void CsheetWithTab::SelectPieceList( DxPiece* pPiece, int nCurType )
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case PIECEMANAGERINDEX:
		m_PieceManagerPage.SelectPieceList( pPiece );   //, nCurType );
		break;

    case PIECEPOINTPAGEINDEX:
        m_PiecePointPage.SelectPieceList( pPiece, nCurType );   //, nCurType );
        break;
	};
}

void CsheetWithTab::SelectTexEffList( const TSTRING& strName )
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case PIECETEXEFFINDEX:
		m_PieceTexEffPage.SelectTexEffList( strName );
		break;
	};
}

void CsheetWithTab::UpdatePage_Always ()
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case TILINGPAGEINDEX:
		m_TilingPage.UpdatePage();
		break;
	};
}

void CsheetWithTab::UpdatePage_Event( BOOL bUp )
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case SINGLEEFFPAGEINDEX:
		if( bUp )	m_SingleEffPage.UpdatePageClickUP();	// �̵��� ������ Tree�� ������
		break;
	case LIGHTPAGEINDEX:
		if( bUp )	m_LightPage.UpdatePageClickUP();		// �̵��� ������ Tree�� ������
		break;
	case PIECEMANAGERINDEX:
		if( !bUp )	m_PieceManagerPage.UpdatePage();		// �ؽ��� �˾Ƴ��� ����
		break;
	case CLOTHPAGEINDEX:
		if( !bUp )	m_ClothPage.UpdatePage();				// �ؽ��� �˾Ƴ��� ����
		break;
	case CLOTH4MATERIALINDEX:
		if( !bUp )	m_ClothForMaterialPage.UpdatePage();				// �ؽ��� �˾Ƴ��� ����
		break;
	};
}

void CsheetWithTab::UpdatePage_ListChange()
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case SINGLEEFFPAGEINDEX:
		m_SingleEffPage.UpdatePageClickDOWN();	// ���λ����� ������ ���� List �� ����
		break;
	case LIGHTPAGEINDEX:
		m_LightPage.UpdatePageClickDOWN();		// ���λ����� ������ ���� List �� ����
		break;
	case SOUNDPAGEINDEX:
		m_SoundPage.UpdatePageClickDOWN();
		break;
	case PREFABPAGEINDEX:
		m_PrefabPage.UpdatePage();					// ���λ����� ������ ���� List �� ����
		break;
	};
}

void CsheetWithTab::UpdatePage_List_AddLastItem()
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case PIECEPOINTPAGEINDEX:
		m_PiecePointPage.UpdatePage_List_AddLastItem();		// ���λ����� ������ ���� List �� ����
		break;
	case SHADOWLIGHTINDEX:
		m_ShadowLightPage.UpdatePage_List_AddLastItem();	// ���λ����� ������ ���� List �� ����
		break;
	};
}

void CsheetWithTab::KeyBoard_Enter()
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
	case LIGHTPAGEINDEX:
		m_LightPage.KeyBoard_Enter();
		break;
	};
}

void CsheetWithTab::UpdateKeyBoardMouse()
{
	int nIndex = GetActiveIndex();

	switch ( nIndex )
	{
    case GRASSPAGEINDEX:
        m_GrassPage.UpdateKeyBoardMouse();
        break;

	case TILINGPAGEINDEX:
		m_TilingPage.UpdateKeyBoardMouse();
		break;

    case PIECEPOINTPAGEINDEX:
        m_PiecePointPage.UpdateKeyBoardMouse();
        break;

	case PREFABPAGEINDEX:
		m_PrefabPage.UpdateKeyBoardMouse();			// Make Tree
		break;

	case SHADOWLIGHTINDEX:
		m_ShadowLightPage.UpdateKeyBoardMouse();
		break;
	};
}

void CsheetWithTab::SetActivePageEX( int nNewPage, int BackUp )
{
	WorldStateMachine::SetFocus(m_PrevPage, nNewPage);

	m_PrevPage = (EDITSHEET)BackUp;
	SetActivePage( nNewPage );
}

void CsheetWithTab::SetActivePageEX( int nNewPage )
{
	m_PrevPage = (EDITSHEET)GetActiveIndex();
	SetActivePage( nNewPage );
}

void CsheetWithTab::SetActiveBackUpPage( int nCurPage )
{
	SetActivePage( m_PrevPage );
	m_PrevPage = (EDITSHEET)nCurPage; 
}


