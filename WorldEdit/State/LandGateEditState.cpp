#include "stdafx.h"
#include "LandGateEditState.h"
#include "BGMEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CLandGateEditState::CLandGateEditState() : 
    CEditState(new CBGMEditState)
{

}

int CLandGateEditState::GetState()
{
    return LANDGATEPAGEINDEX;
}

void CLandGateEditState::EditAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView()); 
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();

    const TCHAR* pStrItem = 0;

    kParam >> pStrItem;

    if (pAssetDlg->Delete(pStrItem) == false)
    {
        HTREEITEM hRoot = pAssetDlg->Find(g_strEditSheet[GetState()]);
        pAssetDlg->Insert(hRoot, pStrItem, false, 0);
    }
}

void CLandGateEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());  
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();    
    DxLandGateMan* pLandGateMan = pSetLandMan->GetLandGateMan(); 

    DWORD i = 0;

    PDXLANDGATE pLandGate = pLandGateMan->GetListLandGate();

    while (pLandGate)
    {
        if (pAssetDlg->IsSelectItemText(pLandGate->m_szName))
        {
            pFrame->m_cDlgEdit.GetSheetChar()->m_LandGatePage.m_listLandGate.SetCurSel(i);
            break;
        }

        pLandGate = pLandGate->m_pNext;
        ++i;
    }
}
