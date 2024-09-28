#include "stdafx.h"
#include "CameraEditState.h"
#include "SoundEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

CCameraEditState::CCameraEditState() : 
    CEditState(new CSoundEditState)
{

}

int CCameraEditState::GetState()
{
    return CAMERAPAGEINDEX;
}

void CCameraEditState::EditAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView()); 
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();

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

void CCameraEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView()); 
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxCamAniMan* pCameraMan     = pSetLandMan->GetCameraAniMan();
    DxCameraAni* pCamera        = pCameraMan->GetCameraList();

    DWORD i = 0;

    // # ī�޶� �ִϴ� �����Ͱ� ���� �׽�Ʈ�� �غ��� ����. 
    // # ������ ���� ���� ����

    while (pCamera)
    {        
        if (pAssetDlg->IsSelectItemText(pCamera->m_szName))
        {
            pFrame->m_cDlgEdit.GetSheetChar()->m_CameraPage.m_listAnimation.SetCurSel(i);
            break;
        }

        pCamera = pCamera->m_pNext;
        ++i;
    }
}