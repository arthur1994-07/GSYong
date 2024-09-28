#include "stdafx.h"
#include "../resource.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabAnim.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab2

IMPLEMENT_DYNAMIC(CsheetWithTabAnim, CPropertySheet)

CsheetWithTabAnim::CsheetWithTabAnim ( CRect& rect, CWnd* pWndParent )
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_iDistanceFromTop(0),
	 m_pSkinAniContrl(NULL)
{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

//	m_AnimPage.SetSheetTab ( this );		//	0
//	AddPage(&m_AnimPage);
	m_AnimTabPage.SetSheetTab( this );
	AddPage(&m_AnimTabPage);

	m_AnimsetPage.SetSheetTab ( this );		//	1
	AddPage(&m_AnimsetPage);

	m_SoundPage.SetSheetTab ( this );		//	2
	AddPage(&m_SoundPage);

	m_AniDivPage.SetSheetTab ( this );		//	3
	AddPage(&m_AniDivPage);

	m_StrikePage.SetSheetTab ( this );		//	4
	AddPage(&m_StrikePage);

	m_EffAniPage.SetSheetTab ( this );		//	5
	AddPage(&m_EffAniPage);

	m_EffNamePage.SetSheetTab ( this );		//	6
	AddPage(&m_EffNamePage);
}

CsheetWithTabAnim::~CsheetWithTabAnim()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTabAnim::OnInitDialog() 
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
		HWND hWnd = NULL;
		for (int i = 0; i < 4; ++i)
		{
			hWnd = ::GetDlgItem(m_hWnd, module_piButtons[i]);
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



BEGIN_MESSAGE_MAP(CsheetWithTabAnim, CPropertySheet)
	//{{AFX_MSG_MAP(CsheetWithTabAnim)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



HBRUSH CsheetWithTabAnim::CtlColor(CDC* pDc, UINT uCtlColor) 
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

void CsheetWithTabAnim::AnimSetActive ( CString sName )
{
	SetActivePage ( ANIMSETPAGEINDEX );
	m_AnimsetPage.SetAnimName ( sName );
}

void CsheetWithTabAnim::SetPages( DxSkinAniControlData* pSkinAniContrlData, DxSkinAniControl* pSkinAniContrl )
{
	m_pSkinAniContrl = pSkinAniContrl;
	m_pSkinAniContrlData = pSkinAniContrlData;

	SetActivePage ( ANIMPAGEINDEX );
	m_AnimTabPage.GetAnimPage()->AnimationListing ();
}

void CsheetWithTabAnim::ReSetPages ()
{
	m_pSkinAniContrl = NULL;
	m_pSkinAniContrlData = NULL;

	SetActivePage ( ANIMPAGEINDEX );
	m_AnimTabPage.GetAnimPage()->AnimationListing ();
}

void CsheetWithTabAnim::SoundSetActive( SChaSoundData* pChaSoundData )
{
	SetActivePage ( SOUNDPAGEINDEX );	
	m_SoundPage.SetSound ( pChaSoundData );
}

void CsheetWithTabAnim::AniDivActive ( CString sName )
{
	SetActivePage ( ANIDIVPAGEINDEX );
	m_AniDivPage.SetAnimName (sName );
}

void CsheetWithTabAnim::AniStrike ( CString sName )
{
	SetActivePage ( ANISTRIKE );
	m_StrikePage.SetAnimName (sName );
}

void CsheetWithTabAnim::AniEff ( CString sName )
{
	SetActivePage ( ANIEFFBONE );
	m_EffAniPage.SetAnimName (sName );	// 무슨 에니메이션을 사용하는지를 넘겨주기 위해서 사용.
}

void CsheetWithTabAnim::AniEffName( BOOL bChange )
{
	if( bChange )
		SetActivePage ( ANIEFFNAME );
	m_EffNamePage.SetData();
}

void CsheetWithTabAnim::ApplyAniContrl ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();


	if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
	{
		DxSkinCharData* pSkinCharData = pView->GetSkinCharData ();
		DxSkinChar* pSkinChar		  = pView->GetSkinChar();

		pSkinChar->SetCharData ( pSkinCharData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

		// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함.
		m_pSkinAniContrl = pSkinChar->m_spSkinAniControlThread.get();
	}
	else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
	{
		DxAttBoneData* pAttBoneData = pView->GetAttBoneData ();
		DxAttBone* pAttBone			= pView->GetAttBone();

		pAttBone->SetAttBoneData ( pAttBoneData, pView->GetD3dDevice() );

		// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함.
		m_pSkinAniContrl = pAttBone->m_spSkinAniControlThread.get();
	}
	else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
	{
		DxVehicleData* pVehicleData = pView->GetVehicleData ();
		DxVehicle* pVehicle			= pView->GetVehicle();

		pVehicle->SetVehiclePartData ( pVehicleData, pView->GetD3dDevice() );

		// SetCharData 내부에서 새로만들기 때문에 재 셋팅이 필요함.
		m_pSkinAniContrl = pVehicle->m_spSkinAniControlThread.get();
	}
}