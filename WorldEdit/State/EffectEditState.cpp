#include "stdafx.h"
#include "EffectEditState.h"
#include "SkinObjEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CEffectEditState::CEffectEditState() : 
    CLightEditState(new CSkinObjEditState)
{

}

int CEffectEditState::GetState()
{
    return SINGLEEFFPAGEINDEX;
}

void CEffectEditState::EditAsset(CParam& kParam)
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

void CEffectEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DXLANDEFF* pEffectList      = pSetLandMan->GetLandEffList();
    
    RemovSelection(pFrame->m_cDlgEdit.GetSheetChar()->m_SingleEffPage.m_EffListBox);

    DWORD i = 0;

    while (pEffectList)
    {
        if ( pAssetDlg->IsSelectItemText(pEffectList->GetEffectName()) )
        {
            pFrame->m_cDlgEdit.GetSheetChar()->m_SingleEffPage.m_EffListBox.SetSel(i);
            pFrame->m_cDlgEdit.GetSheetChar()->m_SingleEffPage.OnLbnSelchangeListEffect();
            break;
        }

        pEffectList = pEffectList->m_pNext;
        ++i;
    }
}