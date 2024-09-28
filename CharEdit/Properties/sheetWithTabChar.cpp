#include "stdafx.h"
#include "../resource.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabBone.h"
#include "SheetWithTabAnim.h"
#include "SheetWithTabChar.h"
#include "sheetWithTabFrame.h"
#include "sheetWithTabPiece.h"

#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"

#include "../State/CharStateMachine.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#ifndef DEF_SKIN_SYNC
const char CsheetWithTabChar::m_szCharTabPages[CHARTAB_EFFECT_SINGLE][65] = 
{
	_T("character file"),
	_T("Piece file settings"),
	_T("skin object"),
	_T("Character Effect"),
	_T("Attach Bone"),
	_T("Attach Bone Link"),
	_T("Vehicle"),
	_T("Fly Camera")
};
#else
const char CsheetWithTabChar::m_szCharTabPages[CHARTAB_SIZE][65] = 
{
    _T("character file"),
    _T("Character Effect"),
    _T("Attach Bone"),
    _T("Attach Bone Link"),
    _T("Vehicle 파일")
};
#endif
/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab2

IMPLEMENT_DYNAMIC(CsheetWithTabChar, CPropertySheet)

CsheetWithTabChar::CsheetWithTabChar ( CRect& rect, CWnd* pWndParent ) :
    CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
,	m_bNoTabs(FALSE)
,	m_iDistanceFromTop(0)
#ifdef DEF_SKIN_SYNC
,   m_pCurPieceContainer(0)
#endif
{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_CharPage.SetSheetTab ( this );
	AddPage(&m_CharPage);

#ifndef DEF_SKIN_SYNC
	m_SkinPiecePage.SetSheetTab ( this );	
	AddPage(&m_SkinPiecePage);

	m_SkinObjPage.SetSheetTab ( this );		
	AddPage(&m_SkinObjPage);
#endif

	m_EffKeepManPage.SetSheetTab ( this );	
	AddPage(&m_EffKeepManPage);

	m_AttBonePage.SetSheetTab ( this );		
	AddPage(&m_AttBonePage);

	m_AttachLinkPage.SetSheetTab ( this );	
	AddPage(&m_AttachLinkPage);

	m_VehiclePage.SetSheetTab ( this );		
	AddPage(&m_VehiclePage);

	m_FlyCameraPage.SetSheetTab ( this );
	AddPage(&m_FlyCameraPage);

#ifndef DEF_SKIN_SYNC
	m_EffSingle.SetSheetTab ( this );		
	AddPage(&m_EffSingle);

	m_EffBlur.SetSheetTab ( this );			
	AddPage(&m_EffBlur);

	m_EffCloneBlur.SetSheetTab ( this );	
	AddPage(&m_EffCloneBlur);

	m_EffDust.SetSheetTab ( this );			
	AddPage(&m_EffDust);

	m_EffShock.SetSheetTab ( this );		
	AddPage(&m_EffShock);

	m_EffAttribute.SetSheetTab ( this );	
	AddPage(&m_EffAttribute);

	m_EffArrow.SetSheetTab ( this );		
	AddPage(&m_EffArrow);

	m_EffSpecular.SetSheetTab ( this );		
	AddPage(&m_EffSpecular);

	m_EffSpecular2.SetSheetTab ( this );	
	AddPage(&m_EffSpecular2);

	m_EffEmit.SetSheetTab ( this );			
	AddPage(&m_EffEmit);

	m_EffAlpha.SetSheetTab ( this );		
	AddPage(&m_EffAlpha);

	m_EffNeon.SetSheetTab ( this );			
	AddPage(&m_EffNeon);

	m_EffMark.SetSheetTab ( this );			
	AddPage(&m_EffMark);

	m_EffNoAlpha.SetSheetTab ( this );		
	AddPage(&m_EffNoAlpha);

	m_EffReflection2.SetSheetTab ( this );	
	AddPage(&m_EffReflection2);

	m_EffAmbient.SetSheetTab ( this );		
	AddPage(&m_EffAmbient);

	m_EffDot3.SetSheetTab ( this );			
	AddPage(&m_EffDot3);

	m_EffMultiTex.SetSheetTab ( this );		
	AddPage(&m_EffMultiTex);

	m_EffParticle.SetSheetTab ( this );		
	AddPage(&m_EffParticle);

	m_EffBonePosEff.SetSheetTab ( this );	
	AddPage(&m_EffBonePosEff);

	m_EffBoneListEff.SetSheetTab ( this );	
	AddPage(&m_EffBoneListEff);

	m_EffUserColor.SetSheetTab ( this );	
	AddPage(&m_EffUserColor);

	m_EffLine2Bone.SetSheetTab ( this );	
	AddPage(&m_EffLine2Bone);

	m_EffLine2BoneD.SetSheetTab ( this );	
	AddPage(&m_EffLine2BoneD);

	m_SkinMaterial.SetSheetTab ( this );	
	AddPage(&m_SkinMaterial);

	m_EffGhosting.SetSheetTab ( this );		
	AddPage(&m_EffGhosting);

	m_EffLevel.SetSheetTab ( this );		
	AddPage(&m_EffLevel);

	m_EffToon.SetSheetTab ( this );			
	AddPage(&m_EffToon);

	m_EffTexDiff.SetSheetTab ( this );		
	AddPage(&m_EffTexDiff);

	m_EffNormalMap.SetSheetTab ( this );	
	AddPage(&m_EffNormalMap);

	m_EffNone.SetSheetTab ( this );	
	AddPage(&m_EffNone);
#endif

	m_EffAniSingle.SetSheetTab ( this );	
	AddPage(&m_EffAniSingle);

	m_EffAniGhosting.SetSheetTab ( this );	
	AddPage(&m_EffAniGhosting);

	m_EffAniTrace.SetSheetTab ( this );		
	AddPage(&m_EffAniTrace);

	m_EffAniFaceOff.SetSheetTab ( this );	
	AddPage(&m_EffAniFaceOff);

	m_EffKeepCTEdge.SetSheetTab ( this );	
	AddPage(&m_EffKeepCTEdge);

	m_EffKeepCTShade.SetSheetTab ( this );	
	AddPage(&m_EffKeepCTShade);

	m_EffKeepCTColor.SetSheetTab ( this );	
	AddPage(&m_EffKeepCTColor);

	m_AniBoneScale.SetSheetTab ( this );	
	AddPage(&m_AniBoneScale);

	m_AniJiggleBlend.SetSheetTab ( this );	
	AddPage(&m_AniJiggleBlend);

#ifndef DEF_SKIN_SYNC
	m_AroundEff.SetSheetTab ( this );		
	AddPage(&m_AroundEff);

	m_EffOverlay.SetSheetTab( this );
	AddPage(&m_EffOverlay);

	m_EffTotalShader.SetSheetTab( this );
	AddPage(&m_EffTotalShader);

    m_EffVisualMaterial.SetSheetTab( this );
	AddPage(&m_EffVisualMaterial);

    m_EffSplineSingleEff.SetSheetTab( this );
	AddPage(&m_EffSplineSingleEff);	
#endif

	m_BoneSetting.SetSheetTab ( this );
	AddPage(&m_BoneSetting);

	m_JiggleBone.SetSheetTab ( this );
	AddPage(&m_JiggleBone);

	m_JiggleBoneColl.SetSheetTab ( this );
	AddPage(&m_JiggleBoneColl);

	m_ClothColl.SetSheetTab ( this );
	AddPage(&m_ClothColl);

#ifndef DEF_SKIN_SYNC
	m_EffWorldBattleFlag.SetSheetTab ( this );
	AddPage(&m_EffWorldBattleFlag);
#endif

	m_FootColl.SetSheetTab ( this );
	AddPage(&m_FootColl);

	m_sSkinInfo.SetSheetTab ( this );
	AddPage(&m_sSkinInfo);

	m_sWeaponScale.SetSheetTab ( this );
	AddPage(&m_sWeaponScale);

	m_sBillboard.SetSheetTab ( this );
	AddPage(&m_sBillboard);
}

CsheetWithTabChar::~CsheetWithTabChar ()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTabChar::OnInitDialog() 
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



BEGIN_MESSAGE_MAP(CsheetWithTabChar, CPropertySheet)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



HBRUSH CsheetWithTabChar::CtlColor(CDC* pDc, UINT uCtlColor) 
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

BOOL CsheetWithTabChar::SetActivePage ( int nPage )
{
	SetActivePageTab ( nPage );

	CPropertySheet::GetActivePage()->OnSetActive();

	return TRUE;
}

void CsheetWithTabChar::SetActivePageTab ( int nPage )
{
	int nOldPage = CPropertySheet::GetActiveIndex();
	
	CPropertySheet::SetActivePage ( nPage );

	CDlgEdit* pPARENT_DLG = (CDlgEdit*) GetParent();
	pPARENT_DLG->SetActiveSheetCombo ( false );

	switch ( nPage )
	{ 
	case CHARTAB_CHARFILE:
	case CHARTAB_CHAREFF:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_SKINCHAR;

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if( !pFrame )	return;
			CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			if( !pView )	return;
			CsheetWithTabAnim* pAnimTab = m_CharPage.GetAnimationTab ();			
			pAnimTab->m_pSkinAniContrl = pView->GetSkinChar()->m_spSkinAniControlThread.get();
			pAnimTab->m_pSkinAniContrlData = pView->GetSkinCharData()->GetSkinAniControlData().get();
			pAnimTab->AniEffName( FALSE );
			CsheetWithTabBone* pBoneTab = m_CharPage.GetBoneTab ();
			pBoneTab->m_pSkinAniContrl = pView->GetSkinChar()->m_spSkinAniControlThread.get();

			pView->DeActiveEditMatrix();

#ifndef DEF_SKIN_SYNC
			m_AttBonePage.OnBnClickedButtonNew();
			m_AttachLinkPage.OnBnClickedButtonNew();
			m_VehiclePage.OnBnClickedButtonNew();
#endif

			if( m_CharPage.GetFrameTab() )
				m_CharPage.GetFrameTab()->SetFrameEditType( EMFRAMEEDIT_CHAR );

#ifdef DEF_SKIN_SYNC
            m_pCurPieceContainer = &m_CharPage;
#endif
		}
		break;

#ifndef DEF_SKIN_SYNC
	case CHARTAB_CHAROBJ:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_SKINOBJ;			

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if( !pFrame )	return;
			CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			if( !pView )	return;
			CsheetWithTabAnim* pAnimTab = m_SkinObjPage.GetAnimationTab ();			
			pAnimTab->m_pSkinAniContrl = pView->GetSkinObject ();
			pAnimTab->m_pSkinAniContrlData = NULL;

			pView->DeActiveEditMatrix();

			m_AttBonePage.OnBnClickedButtonNew();
			m_AttachLinkPage.OnBnClickedButtonNew();
		}
		break;
#endif

#ifndef DEF_SKIN_SYNC
	case CHARTAB_CHARPIECE:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_SKINPIECE;
		}
		break;
#endif

	case CHARTAB_ATTBONEFILE:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_ATTBONE;


			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if( !pFrame )	return;
			CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			if( !pView )	return;

			CsheetWithTabAnim* pAnimTab = m_AttBonePage.GetAnimationTab ();			
			pAnimTab->m_pSkinAniContrl = pView->GetAttBone ()->m_spSkinAniControlThread.get();
			pAnimTab->m_pSkinAniContrlData = NULL;
			pAnimTab->AniEffName( FALSE );

			pView->DeActiveEditMatrix();

#ifndef DEF_SKIN_SYNC
			m_SkinPiecePage.OnBnClickedButtonNew();
 			m_CharPage.OnBnClickedButtonNew();
			m_VehiclePage.OnBnClickedButtonNew();
#endif

			if( m_AttBonePage.GetFrameTab() )
				m_AttBonePage.GetFrameTab()->SetFrameEditType( EMFRAMEEDIT_ATTBONE );
			
#ifdef DEF_SKIN_SYNC
            m_pCurPieceContainer = &m_AttBonePage;
#endif
		}
		break;
	case CHARTAB_ATTACHLINK:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_ATTLINK;

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if( !pFrame )	return;
			CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			if( !pView )	return;

			pView->DeActiveEditMatrix();

#ifndef DEF_SKIN_SYNC
			m_SkinPiecePage.OnBnClickedButtonNew();
#endif
			m_CharPage.OnBnClickedButtonNew();

			m_VehiclePage.OnBnClickedButtonNew();
		}
		break;
	case CHARTAB_VEHICLE:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_VEHICLE;

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if( !pFrame )	return;
			CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			if( !pView )	return;

			CsheetWithTabAnim* pAnimTab = m_VehiclePage.GetAnimationTab ();			
			pAnimTab->m_pSkinAniContrl = pView->GetVehicle ()->m_spSkinAniControlThread.get();
			pAnimTab->m_pSkinAniContrlData = NULL;
			pAnimTab->AniEffName( FALSE );

			pView->DeActiveEditMatrix();

#ifndef DEF_SKIN_SYNC
			m_AttBonePage.OnBnClickedButtonNew();
			m_AttachLinkPage.OnBnClickedButtonNew();
			m_SkinPiecePage.OnBnClickedButtonNew();
			m_CharPage.OnBnClickedButtonNew();
#endif

			if( m_VehiclePage.GetFrameTab() )
				m_VehiclePage.GetFrameTab()->SetFrameEditType( EMFRAMEEDIT_VEHICLE );

#ifdef DEF_SKIN_SYNC
            m_pCurPieceContainer = &m_VehiclePage;
#endif
		}
        break;

	case CHARTAB_FLYCAMERA:
		{
			pPARENT_DLG->SetActiveSheetCombo ( true );
			CCharEditView::m_emObjType = EOBJ_FLYCAMERA;

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			if( !pFrame )	return;
			CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
			if( !pView )	return;
			CsheetWithTabAnim* pAnimTab = m_FlyCameraPage.GetAnimationTab();
			pAnimTab->m_pSkinAniContrl = pView->GetSkinChar()->m_spSkinAniControlThread.get();
			pAnimTab->m_pSkinAniContrlData = pView->GetSkinCharData()->GetSkinAniControlData().get();
			pAnimTab->AniEffName( FALSE );
			CsheetWithTabBone* pBoneTab = m_CharPage.GetBoneTab ();
			pBoneTab->m_pSkinAniContrl = pView->GetSkinChar()->m_spSkinAniControlThread.get();

			pView->DeActiveEditMatrix();
		}
		break;
    default:
#ifdef DEF_SKIN_SYNC
        m_pCurPieceContainer = 0;
#endif
        break;
	};

	// 정보들을 Refresh 해 주도록 한다.
	// 천 시뮬레이션 과 ClothColl 과의 연결이 원할하지 못해서 이렇게 작업함. ( cps 에서 셋팅 후 chf 로 넣을 시 충돌이 안되는 경향이 있었다. )
	NSMaterialSkinManager::OnlyVersionUpForTool();

	CharStateMachine::SetFocus(nOldPage, nPage);
}

// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
void CsheetWithTabChar::ChangeTimeOfTimeLine( int nPage, DWORD dwToTime )
{
	switch( nPage )
	{
	case CHARTAB_EFFANI_SINGLE:
		m_EffAniSingle.ChangeTimeOfTimeLine( dwToTime );
		break;
	}
}

// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
void CsheetWithTabChar::OnButtonPlayOfCDlgTimeLine( int nPage )
{
	switch( nPage )
	{
	case CHARTAB_EFFANI_SINGLE:
		m_EffAniSingle.OnButtonPlayOfCDlgTimeLine();
		break;
	}
}

void CsheetWithTabChar::SetSpinControl( int nPage, int nRadius, int nHeight, int nSpin3, int nSpin4, int nColorHeight )
{
#ifndef DEF_SKIN_SYNC
	switch( nPage )
	{
	case CHARTAB_CHARFILE:
		m_CharPage.SetSpinControl( nRadius, nHeight, nColorHeight );
		break;
	case CHARTAB_OVERLAY: // by luxes.
		m_EffOverlay.SetSpinControl( nColorHeight );
		m_EffOverlay.FrameMove();
		break;
	case CHARTAB_BILLBOARD:
		m_sBillboard.SetSpinControl( nRadius, nHeight, nSpin3, nSpin4 );
		break;
	}
#else
    if (m_pCurPieceContainer)
    {
        m_pCurPieceContainer->SetSpinControl(nRadius, nHeight, nColorHeight);
    }
#endif
}

void CsheetWithTabChar::ResetDialog( int nPage )
{
#ifndef DEF_SKIN_SYNC
    switch( nPage )
	{
	case CHARTAB_VISUAL_MATERIAL:
		m_EffVisualMaterial.VmfxReload_DialogReset();
		break;
	}
#else
    if (m_pCurPieceContainer)
    {
        m_pCurPieceContainer->ResetDialog();
    }
#endif
}

void CsheetWithTabChar::UpdateKeyBoardMouse( int nPage )
{
	switch( nPage )
	{
	case CHARTAB_FOOT_COLL:
		m_FootColl.UpdateKeyBoardMouse();
		break;
	}

#ifndef DEF_SKIN_SYNC
    switch( nPage )
	{
	case CHARTAB_SPLINE_SINGLE_EFF:
		m_EffSplineSingleEff.UpdateKeyBoardMouse();
		break;
	}
#else
    if (m_pCurPieceContainer)
    {
        m_pCurPieceContainer->UpdateKeyBoardMouse();
    }
#endif
}