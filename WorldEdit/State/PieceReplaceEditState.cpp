#include "stdafx.h"
#include "PieceReplaceEditState.h"
#include "WaterEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CPieceReplaceEditState::CPieceReplaceEditState() : 
    CLightEditState(new CWaterEditState)
{

}

int CPieceReplaceEditState::GetState()
{
    return PIECEMANAGERINDEX;
}

void CPieceReplaceEditState::EditAsset(CParam& kParam)
{
    SyncPieceReplace();
}

void CPieceReplaceEditState::SyncPieceReplace()
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());   
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxPieceManagerPSF* pPieceMng= pSetLandMan->GetPieceManager();

    HTREEITEM hRoot = pAssetDlg->Find(g_strEditSheet[GetState()]);
    HTREEITEM hFirstChild = pAssetDlg->GetChild(hRoot);

    DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;

    DWORD dwSize = pPieceMng->GetChangePieceSize();
    DWORD dwSiblingSize = pAssetDlg->GetSiblingSize(hFirstChild);
    DWORD dwGreatest = dwSiblingSize > dwSize ? dwSiblingSize : dwSize;

    for (DWORD i = 0; i < dwGreatest; ++i, pPieceInfo = 0)
    {
        if (i < dwSize)
        {
            pPieceInfo = pPieceMng->GetChangeMATRIXPISNAME(i);
        }

        HTREEITEM hTemp = pAssetDlg->GetNextSibling(hFirstChild);

        if (pPieceInfo == 0)
        {
            pAssetDlg->Delete(hFirstChild);            
        }
        else
        {
            // ����� ũ�ų� ���� ���
            if (hFirstChild)           
            {
                if(pAssetDlg->IsItemText(hFirstChild, pPieceInfo->m_pPisName) == false)
                {
                    // ����� ū�� ���� �ٸ� ���
                    // �����ϸ鼭 ���� ������ ã�´�.
                    if (dwSize < dwSiblingSize)
                    {
                        pAssetDlg->Delete(hFirstChild);
                        --i;
                        --dwSiblingSize;
                    }
                    else
                    {
                        // ���� ���� ũ�Ⱑ ���ٸ�
                        // ���ΰ��� �ؽ�Ʈ�� ���� �ش�.
                        pAssetDlg->SetItemText(hFirstChild, pPieceInfo->m_pPisName);
                    }
                }
            }
            else
            {
                InsertPiece(pPieceInfo, pAssetDlg, hRoot);
            }
        }

        hFirstChild = hTemp;
    }
}

void CPieceReplaceEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxPiece* pPiece = 0;
    kParam >> pPiece;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxPieceManagerPSF* pPieceMng= pSetLandMan->GetPieceManager();

    DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;

    if (pPiece && pPieceMng && pAssetDlg->IsClick2Tree())
    {
        DWORD dwSize = pPieceMng->GetChangePieceSize();

        for (DWORD i = 0; i < dwSize; ++i)
        {
            pPieceInfo = pPieceMng->GetChangeMATRIXPISNAME(i);

            if (pPiece == pPieceInfo->m_pPiece)
            {
                CListCtrl& kControl = pFrame->m_cDlgEdit.GetSheetChar()->m_PieceManagerPage.m_listReplace;

                RemoveAllSelected(kControl);

                kControl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
                LRESULT temp;
                pFrame->m_cDlgEdit.GetSheetChar()->m_PieceManagerPage.OnNMClickListReplace(0, &temp);
                kControl.EnsureVisible(i, TRUE);
            }
        }
    }
}

void CPieceReplaceEditState::RemoveAllSelected(CListCtrl& kControl)
{
    UINT nSelectCount = kControl.GetSelectedCount();

    int nItem = -1;

    if (nSelectCount > 0)
    {
        for (UINT i = 0; i < nSelectCount; ++i)
        {
            nItem = kControl.GetNextItem(nItem, LVNI_SELECTED);
            kControl.SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}
