#include "stdafx.h"
#include "LightEditState.h"
#include "CameraEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CLightEditState::CLightEditState(CEditState* pNext) : 
    CEditState(pNext == 0 ? new CCameraEditState : pNext)
{
    
}

int CLightEditState::GetState()
{
    return LIGHTPAGEINDEX;
}

void CLightEditState::EditAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());  
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    const TCHAR* pStrItem = 0;
    kParam >> pStrItem;

    // 변경 사항(삭제, 추가) 를 적용 한다.
    if (pAssetDlg->Delete(pStrItem) == false)
    {
        // 삭제가 아닐 경우 추가 이므로 추가 처리를 한다.
        HTREEITEM hRoot = pAssetDlg->Find(g_strEditSheet[GetState()]);
        pAssetDlg->Insert(hRoot, pStrItem, false, 0);
    }
}

void CLightEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());  
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxLightMan* pLightMan       = pSetLandMan->GetLightMan();

    RemovSelection(pFrame->m_cDlgEdit.GetSheetChar()->m_LightPage.m_ListLight);
    
    // 밤, 낮의 광원은 하드 코딩 되어 있어 어쩔 수 없다.
    if ( pAssetDlg->IsSelectItemText(_T("DIRECTIONAL-[NOON]")) )
    {
        pFrame->m_cDlgEdit.GetSheetChar()->m_LightPage.m_ListLight.SetSel(0);
        goto END;
    }
    else if ( pAssetDlg->IsSelectItemText(_T("DIRECTIONAL-[NIGHT]")) )
    {
        pFrame->m_cDlgEdit.GetSheetChar()->m_LightPage.m_ListLight.SetSel(1);
        goto END;
    }

    DWORD i = 2;

    BOOST_FOREACH(DXLIGHT* pListLight, pLightMan->GetLIST_DXLIGHT())
    {
        if (pAssetDlg->IsSelectItemText(pListLight->m_szLightName))
        {
            pFrame->m_cDlgEdit.GetSheetChar()->m_LightPage.m_ListLight.SetSel(i);
            goto END;
        }

        ++i;
    }

END:
    pFrame->m_cDlgEdit.GetSheetChar()->m_LightPage.OnLbnSelchangeListLight();
}

void CLightEditState::RemovSelection(CListBox& kListBox)
{
    int nCount = kListBox.GetSelCount();

    CArray<int, int> aryListBoxSel;
    aryListBoxSel.SetSize(nCount);
    kListBox.GetSelItems(nCount, aryListBoxSel.GetData());

    for (int i = 0; i < nCount; ++i)
    {
        kListBox.SetSel(aryListBoxSel[i], FALSE);
    }
}