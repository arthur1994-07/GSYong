#include "stdafx.h"
#include "../resource.h"

#include "EffSheetWithTab.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CString g_strEditSheet[30] =
{
	"GROUP EDIT",				//	0
	"Particle EDIT",			//	1
	"Direction EDIT",			//	2
	"Ground EDIT",				//	3
	"Mesh   EDIT",				//  4
	"Sequence EDIT",			//  5
	"GroupProp EDIT",			//  6
	"Decal EDIT",				//  7
	"Camera EDIT",				//  8
	"Sound EDIT",				//  9
	"BlurSys EDIT",				//  10
	"Lighting EDIT",			//  11
	"MoveRotate EDIT",			//  12
	"SkinMesh EDIT",			//  13
	"Wave EDIT",				//  14
	"PointLight EDIT",			//  15
	"MoveTarget EDIT",			//  16
	"ScreenEffect EDIT",		//  17
};

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0)
{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_EffGroupPage.SetSheetTab ( this );
	AddPage ( &m_EffGroupPage );

	m_ParticleSysPage.SetSheetTab ( this );
	AddPage ( &m_ParticleSysPage );

	m_DirectPage.SetSheetTab ( this );
	AddPage ( &m_DirectPage );

	m_GroundPage.SetSheetTab ( this );
	AddPage ( &m_GroundPage );

	m_MeshPage.SetSheetTab ( this );
	AddPage( &m_MeshPage );

	m_SequencePage.SetSheetTab ( this );
	AddPage( &m_SequencePage );

	m_GroupPropPage.SetSheetTab ( this );
	AddPage( &m_GroupPropPage );

	m_DecalPage.SetSheetTab ( this );
	AddPage( &m_DecalPage );

	m_CameraPage.SetSheetTab ( this );
	AddPage( &m_CameraPage );

	m_SoundPage.SetSheetTab ( this );
	AddPage( &m_SoundPage );

	m_BlurSysPage.SetSheetTab ( this );
	AddPage( &m_BlurSysPage );

	m_LightingPage.SetSheetTab ( this );
	AddPage( &m_LightingPage );

	m_MoveRotatePage.SetSheetTab ( this );
	AddPage( &m_MoveRotatePage );

	m_SkinMeshPage.SetSheetTab ( this );
	AddPage( &m_SkinMeshPage );

	m_WavePage.SetSheetTab ( this );
	AddPage( &m_WavePage );

	m_PointLightPage.SetSheetTab ( this );
	AddPage( &m_PointLightPage );

	m_MoveTargetPage.SetSheetTab ( this );
	AddPage( &m_MoveTargetPage );

	m_TargetInfluencePage.SetSheetTab( this );
	AddPage( &m_TargetInfluencePage );

	m_ScreenEffectPage.SetSheetTab( this );
	AddPage( &m_ScreenEffectPage );
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

		//	Note : 탭을 보여주지 않을 때에는 사이즈의 조정이 필요 없으므로 무시한다.
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


