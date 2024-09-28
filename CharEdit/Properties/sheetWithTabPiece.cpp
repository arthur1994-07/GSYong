#include "stdafx.h"
#include "../resource.h"
#include "SheetWithTabPiece.h"
#include "PieceContainerPage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../State/CharStateMachine.h"

#include "SkinPiecePage.h"
#include "EffSinglePage.h"
#include "EffBlurPage.h"
#include "EffCloneBlurPage.h"
#include "EffDustPage.h"
#include "EffShockPage.h"
#include "EffAttributePage.h"
#include "EffArrowPage.h"
#include "EffSpecularPage.h"
#include "EffLevelPage.h"
#include "EffEmitPage.h"
#include "EffAlphaPage.h"
#include "EffNeonPage.h"
#include "EffMarkPage.h"
#include "EffNoAlphaPage.h"
#include "EffReflection2Page.h"
#include "EffAmbientPage.h"
#include "EffDot3Page.h"
#include "EffMultiTexPage.h"
#include "EffGhostingPage.h"
#include "EffSpecularPage2.h"
#include "EffToonPage.h"
#include "EffTexDiffPage.h"
#include "EffParticlePage.h"
#include "EffBonePosEffPage.h"
#include "EffBoneListEffPage.h"
#include "EffUserColorPage.h"
#include "EffNormalMapPage.h"
#include "EffLine2BonePage.h"
#include "AroundEffPage.h"
#include "EffOverlayPage.h"
#include "EffTotalShaderPage.h"
#include "EffVisualMaterialPage.h"
#include "Effects/EffSplineSingleEffPage.h"
#include "EffWorldBattleFlagPage.h"
#include "MaterialPage.h"
#include "EffLine2BoneDPage.h"
#include "UserBackSlotPage.h"
#include "EffNonePage.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CsheetWithTabPiece, CPropertySheet)

BEGIN_MESSAGE_MAP(CsheetWithTabPiece, CPropertySheet)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

CsheetWithTabPiece::CsheetWithTabPiece(CRect& rect, CRect& rectAll, CWnd* pWndParent) :
    CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
,   m_bNoTabs(FALSE)
,   m_pWndParent(NULL)
,   m_iDistanceFromTop(0)
{
	m_pWndParent = pWndParent;
	m_Rect = rect;
    m_RectAll = rectAll;

    m_bNoTabs = TRUE;
    m_bStacked = FALSE;    

    CreateEffect();

    m_SkinPiecePage.reset(new CSkinPiecePage);    
    AddPage(m_SkinPiecePage.get());
}

CsheetWithTabPiece::~CsheetWithTabPiece ()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTabPiece::OnInitDialog() 
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

HBRUSH CsheetWithTabPiece::CtlColor(CDC* pDc, UINT uCtlColor) 
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

void CsheetWithTabPiece::CreateEffect(CPropertyPage* pPage)
{
    int nIndex = GetPageCount();

    if (pPage)
    {
        m_spEffList[nIndex].reset(pPage);

        /*
        if (nIndex == EMEFFCHAR_VISUAL_MATERIAL)
        {
            // 비주얼 매터리얼은 부모에 붙인다.
            CPieceContainerPage* pParentSheet = 
                static_cast<CPieceContainerPage*>(GetParent());

            if (pParentSheet)
            {
                pParentSheet->AddPage(pPage);
            }
        }
        */

        AddPage(pPage);
    }
    else
    {
        MIN_ASSERT(0 && _T("이펙트 메모리 할당 오류!!!"));
    }
}

void CsheetWithTabPiece::CreateEffect()
{
    CreateEffect(new CEffSinglePage);
    CreateEffect(new CEffBlurPage);
    CreateEffect(new CEffCloneBlurPage);
    CreateEffect(new CEffDustPage);
    CreateEffect(new CEffShockPage);
    CreateEffect(new CEffAttributePage);
    CreateEffect(new CEffArrowPage);
    CreateEffect(new CEffSpecularPage);
    CreateEffect(new CEffLevelPage);
    CreateEffect(new CEffEmitPage);
    CreateEffect(new CEffAlphaPage);
    CreateEffect(new CEffNeonPage);
    CreateEffect(new CEffMarkPage);
    CreateEffect(new CEffNoAlphaPage);
    CreateEffect(new CEffReflection2Page);
    CreateEffect(new CEffAmbientPage);
    CreateEffect(new CEffDot3Page);
    CreateEffect(new CEffMultiTexPage);
    CreateEffect(new CEffGhostingPage);
    CreateEffect(new CEffSpecularPage2);
    CreateEffect(new CEffToonPage);
    CreateEffect(new CEffTexDiffPage);
    CreateEffect(new CEffParticlePage);
    CreateEffect(new CEffBonePosEffPage);
    CreateEffect(new CEffBoneListEffPage);
    CreateEffect(new CEffUserColorPage);
    CreateEffect(new CEffNormalMapPage);
    CreateEffect(new CEffLine2BonePage);
    CreateEffect(new CAroundEffPage);
    CreateEffect(new CEffOverlayPage);
    CreateEffect(new CEffTotalShaderPage);
    CreateEffect(new CEffVisualMaterialPage);
    CreateEffect(new CEffSplineSingleEffPage);
    CreateEffect(new CEffWorldBattleFlagPage);
	CreateEffect(new CEffNonePage);

    // 아래는 툴에만 정의된 페이지 이다.
    CreateEffect(new CMaterialPage);
    CreateEffect(new CEffLine2BoneDPage);
	CreateEffect(new CUserBackSlotPage);
}

BOOL CsheetWithTabPiece::SetActivePage(int nPage)
{
    BOOL bResult = FALSE;
    int nCur = GetActiveIndex();

    if (nPage == PIECEPAGE && nCur == nPage && m_SkinPiecePage->IsWindowEnabled())
    {
        bResult = m_SkinPiecePage->OnSetActive();
    }
    else
    {
        bResult = CPropertySheet::SetActivePage(nPage);
    }

    if (bResult)
    {
        m_SkinPiecePage->InitSkinPiece();
    }

    return bResult;
}

void CsheetWithTabPiece::SetSpinControl(int nColorHeight)
{
    int nAct = GetActiveIndex();

    if (nAct < EMEFFCHAR_SIZE)
    {
        CEffCharPage* pPage = static_cast<CEffCharPage*>(m_spEffList[nAct].get());

        if (pPage)
        {
            pPage->SetSpinControl(nColorHeight);
        }
    }
}

void CsheetWithTabPiece::ResetDialog()
{
    int nAct = GetActiveIndex();

    if (nAct < EMEFFCHAR_SIZE)
    {
        CEffCharPage* pPage = static_cast<CEffCharPage*>(m_spEffList[nAct].get());

        if (pPage)
        {
            pPage->ResetDialog();
        }
    }
}

void CsheetWithTabPiece::UpdateKeyBoardMouse()
{
    int nAct = GetActiveIndex();

    if (nAct < EMEFFCHAR_SIZE)
    {
        CEffCharPage* pPage = static_cast<CEffCharPage*>(m_spEffList[nAct].get());

        if (pPage)
        {
            pPage->UpdateKeyBoardMouse();
        }
    }
}

void CsheetWithTabPiece::ReSetPages()
{
    CPieceContainerPage* pParent = static_cast<CPieceContainerPage*>(GetParent());

    if (pParent)
    {
        pParent->ReSetPages();
    }

    SetActivePage(PIECEPAGE);
    ShowWindow(SW_HIDE);
}

void CsheetWithTabPiece::ResetPiece()
{
    CPieceContainerPage* pParent = static_cast<CPieceContainerPage*>(GetParent());

    if (pParent)
    {
        pParent->SkinPieceClear();
    }

    CharStateMachine::ClearFileNotifier();
    CharStateMachine::InsertFileNotifier();
}

void CsheetWithTabPiece::SetSkinPiece(DxSkinPiece* pSkinPiece)
{
    CPieceContainerPage* pParent = static_cast<CPieceContainerPage*>(GetParent());

    if (pParent)
    {
        pParent->SetSkinPiece(pSkinPiece);
    }

    CharStateMachine::InsertFileNotifier();
}

void CsheetWithTabPiece::SwapSkinPiece(int nParts)
{
    CPieceContainerPage* pParent = static_cast<CPieceContainerPage*>(GetParent());

    if (pParent)
    {
        pParent->SwapSkinPiece(nParts);
    }

    CharStateMachine::ClearFileNotifier();
    CharStateMachine::InsertFileNotifier();
}

void CsheetWithTabPiece::ReplaceSkinPiece(int nParts)
{
    CPieceContainerPage* pParent = static_cast<CPieceContainerPage*>(GetParent());

    if (pParent)
    {
        pParent->ReplaceSkinPiece(nParts);
    }

    CharStateMachine::ClearFileNotifier();
    CharStateMachine::InsertFileNotifier();
}
void CsheetWithTabPiece::FullWindowsSize(bool bFull)
{
    if (bFull)
    {
        MoveWindow(m_RectAll.left, m_RectAll.top, m_RectAll.right, m_RectAll.bottom);
    }
    else
    {
        MoveWindow(m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom);
    }
}
