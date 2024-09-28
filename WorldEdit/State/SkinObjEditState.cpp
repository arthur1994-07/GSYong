#include "stdafx.h"
#include "SkinObjEditState.h"
#include "LandGateEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CSkinObjEditState::CSkinObjEditState() : 
    CEditState(new CLandGateEditState)
{

}

int CSkinObjEditState::GetState()
{
    return SKINOBJPAGEINDEX;
}

void CSkinObjEditState::EditAsset(CParam& kParam)
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
        pAssetDlg->Insert(hRoot, pStrItem, false, 0, true);
    }
}

void CSkinObjEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());  
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    PLANDSKINOBJ pSkinObj       = pSetLandMan->GetLandSkinObjList();

    DWORD i = 0;

    while (pSkinObj)
    {
        if (pAssetDlg->IsSelectItemText(pSkinObj->m_szName))
        {
            pFrame->m_cDlgEdit.GetSheetChar()->m_LandSkinObj.m_listSkinObj.SetCurSel(i);         
            break;
        }

        pSkinObj = pSkinObj->m_pNext;
        ++i;
    }
}
