#include "stdafx.h"
#include "WaterEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

#include "../enginelib/Water/NSWaterMain.h"

CWaterEditState::CWaterEditState() : 
    CEditState(0)
{

}

int CWaterEditState::GetState()
{
    return WATERSURFPAGEINDEX;
}

void CWaterEditState::EditAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();

    HTREEITEM hRoot = pAssetDlg->Find(g_strEditSheet[GetState()]);

    const TCHAR* pStrItem = 0;
    kParam >> pStrItem;

    if (_tcscmp(pStrItem, _T("DELETE")) == 0)
    {
        pAssetDlg->Delete(_T("New"));
    }
    else
    {
        pAssetDlg->Insert(hRoot, _T("New"), false, 0);
    }
}

void CWaterEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    WaterSurf* pWaterSurf       = NSWaterMainPSF::GetWaterSurf();

    DWORD dwIndex = pAssetDlg->GetSiblingIndexFromSelected();
    pFrame->m_cDlgEdit.GetSheetChar()->m_WaterSurfPage.m_listSurf.SetCurSel(dwIndex);
}