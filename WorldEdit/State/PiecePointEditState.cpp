#include "stdafx.h"
#include "PiecePointEditState.h"
#include "PieceReplaceEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

#include <boost/timer.hpp>

CPiecePointEditState::CPiecePointEditState()
:   CLightEditState(new CPieceReplaceEditState)
,   m_bUpdate(false)
,   m_dwGreatest(0)
,   m_hResumeChild(0)
{

}

int CPiecePointEditState::GetState()
{
    return PIECEPOINTPAGEINDEX;
}

void CPiecePointEditState::EditAsset(CParam& kParam)
{
    m_bUpdate = true;
}

void CPiecePointEditState::SyncPiecePoint(double dTimer)
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

    DWORD dwSize = pPieceMng->GetPointPieceSize();
    DWORD dwSiblingSize = pAssetDlg->GetSiblingSize(hFirstChild);
    DWORD dwGreatest = dwSiblingSize > dwSize ? dwSiblingSize : dwSize;

    boost::timer timer;

    if (m_bUpdate == false)
    {
        hFirstChild = m_hResumeChild;
    }
    else
    {
        m_dwGreatest = dwGreatest;
        m_bUpdate = false;
    }

    for (DWORD i = dwGreatest - m_dwGreatest; i < dwGreatest; ++i, pPieceInfo = 0, --m_dwGreatest)
    {
        if (timer.elapsed() >= dTimer)
        {
            break;
        }

        if (i < dwSize)
        {
            pPieceInfo = pPieceMng->GetMATRIXPISNAME(i);
        }

        HTREEITEM hTemp = pAssetDlg->GetNextSibling(hFirstChild);

        if (pPieceInfo == 0)
        {
            pAssetDlg->Delete(hFirstChild);
        }
        else
        {
            // 어셋이 크거나 같을 경우
            if (hFirstChild)
            {
                if(pAssetDlg->IsItemText(hFirstChild, pPieceInfo->m_pPisName) == false)
                {
                    // 어셋이 큰데 서로 다를 경우
                    // 삭제하면서 같을 때까지 찾는다.
                    if (dwSize < dwSiblingSize)
                    {
                        pAssetDlg->Delete(hFirstChild);
                        --i;
                        --dwSiblingSize;
                    }
                    else
                    {
                        // 만약 서로 크기가 같다면
                        // 서로간에 텍스트를 맞춰 준다.
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
        m_hResumeChild = hTemp;
    }
}

void CPiecePointEditState::SelectAsset(CParam& kParam)
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
        DWORD dwSize = pPieceMng->GetPointPieceSize();

        for (DWORD i = 0; i < dwSize; ++i)
        {
            pPieceInfo = pPieceMng->GetMATRIXPISNAME(i);

            if (pPiece == pPieceInfo->m_pPiece)
            {
                CListCtrl& kControl = pFrame->m_cDlgEdit.GetSheetChar()->m_PiecePointPage.m_listReplace;

                RemoveAllSelected(kControl);

                kControl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
                LRESULT temp;
                pFrame->m_cDlgEdit.GetSheetChar()->m_PiecePointPage.OnNMClickListReplace(0, &temp);
                kControl.EnsureVisible(i, TRUE);
            }
        }
    }
}

void CPiecePointEditState::RemoveAllSelected(CListCtrl& kControl)
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

void CPiecePointEditState::Update(CParam& kParam)
{
    if (m_dwGreatest > 0 || m_bUpdate)
    {
        SyncPiecePoint();
    }

    CLightEditState::Update(kParam);
}
