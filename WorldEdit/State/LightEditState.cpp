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

    // ���� ����(����, �߰�) �� ���� �Ѵ�.
    if (pAssetDlg->Delete(pStrItem) == false)
    {
        // ������ �ƴ� ��� �߰� �̹Ƿ� �߰� ó���� �Ѵ�.
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
    
    // ��, ���� ������ �ϵ� �ڵ� �Ǿ� �־� ��¿ �� ����.
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