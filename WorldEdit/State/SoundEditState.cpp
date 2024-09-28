#include "stdafx.h"
#include "SoundEditState.h"
#include "EffectEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CSoundEditState::CSoundEditState() : 
    CEditState(new CEffectEditState)
{

}

int CSoundEditState::GetState()
{
    return SOUNDPAGEINDEX;
}

void CSoundEditState::EditAsset(CParam& kParam)
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

void CSoundEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    CStaticSoundMan* pSoundMan  = pSetLandMan->GetStaticSoundMan();
    SVecSound* pSound           = pSoundMan->GetVecSoundHead();

    DWORD i = 0;

    while (pSound)
    {
        if (pAssetDlg->IsSelectItemText(pSound->m_szSoundName))
        {
            pFrame->m_cDlgEdit.GetSheetChar()->m_SoundPage.m_ctrlMapSound.SetCurSel(i);
            pFrame->m_cDlgEdit.GetSheetChar()->m_SoundPage.OnLbnSelchangeListMapsound();
            break;
        }

        pSound = pSound->m_pNext;
        ++i;
    }
}