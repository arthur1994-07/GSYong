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

    // 변경 사항(삭제, 추가) 를 적용 한다.
    if (pAssetDlg->Delete(pStrItem) == false)
    {
        // 삭제가 아닐 경우 추가 이므로 추가 처리를 한다.
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

    // # 카메라 애니는 데이터가 없어 테스트를 해보지 못함. 
    // # 문제가 있을 수도 있음

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