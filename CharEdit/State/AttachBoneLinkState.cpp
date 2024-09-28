#include "stdafx.h"
#include "AttachBoneLinkState.h"
#include "VehicleState.h"

#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../Dialog/AssetDlg.h"
#include "../Properties/sheetWithTabChar.h"
#include "../Properties/sheetWithTabAnim.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/Archive/Archive.h"

CAttachBoneLinkState::CAttachBoneLinkState() :
	CEditState(new CVehicleState)
{

}

int CAttachBoneLinkState::GetState()
{
	return CHARTAB_ATTACHLINK;
}

void CAttachBoneLinkState::Reload(CParam& kParam)
{
	CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
	DxSkinPiece* pSkinPiece 	= pView->GetSkinPiece();
	DxAttBoneLink* pAttBoneLink = pView->GetAttBoneLink();

    const TCHAR* pFile = pAttBoneLink->GetFileName();

    if (pFile == 0 || _tcslen(pFile) == 0)
    {
        return;
    }

    // 삭제되면 문자열이
    // 삭제 되므로 백업해 두어야 한다.
    TSTRING strFile = pFile;

	DxAttBone* pAttPiece = pAttBoneLink->GetAttBone();

	for (int i = 0; i < ATTBONE_SIZE; i++)
	{
		DxCharPart* pAttBonePart	= pAttPiece->GetAttBonePiece(i);
		DxSkinPiece* pInSkinPiece	= pAttBonePart->GetPart();

		if (pInSkinPiece && pInSkinPiece->GetFileName() != 0)
		{
			RemovePiece(pInSkinPiece);
			RemovePart(pInSkinPiece);
		}
	}

	RemoveAni(pAttPiece->m_spSkinAniControlThread.get());

	if (pSkinPiece->GetFileName() != 0)
	{
		RemovePiece(pSkinPiece);
	}

	pTabChar->m_AttachLinkPage.LoadAblFile(strFile.c_str());
}

void CAttachBoneLinkState::Insert(CParam& kParam)
{
	CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);	
	CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();
	DxAttBoneLink* pAttBoneLink = pView->GetAttBoneLink();
	DxAttBone* pAttPiece		= pAttBoneLink->GetAttBone();	

	DxCharPart* pCharPart       = 0;
	DxSkinPiece* pSkinPiece     = 0;	

	for (int i = 0; i < ATTBONE_SIZE; i++)
	{
		pCharPart = pAttPiece->m_AttBoneArray + i;
		pSkinPiece = pCharPart->GetPart();		

		if (pSkinPiece != 0)
		{
			InsertTexture(pSkinPiece, pCharPart);
			InsertSkeleton(pCharPart, pSkinPiece, pAttPiece->m_spSkinAniControlThread.get(), pView);
			InsertMesh(pCharPart, pSkinPiece, pAttPiece->m_spSkinAniControlThread.get(), pView);
		}
	}

	InsertSkinAni(pAttPiece->m_spSkinAniControlThread.get());	

	pSkinPiece = pView->GetSkinPiece();

	if (pSkinPiece->m_pmcMesh == 0)
	{
		return;
	}

	InsertTexture(pSkinPiece, 0);
	InsertSkeleton(pSkinPiece, pDevice);
	InsertMesh(pSkinPiece, pDevice);
}

void CAttachBoneLinkState::Archive(CParam& kParam)
{
    CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);	
    CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();   
    DxAttBoneLink* pAttBoneLink = pView->GetAttBoneLink();
    DxSkinPiece* pSkinPiece     = pView->GetSkinPiece();
    DxSkeleton* pLinkSkeleton   = pView->GetAttBoneLinkSkeleton();

    const TCHAR* pFolder = 0;
    kParam >> pFolder;

    Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
    Archive::Archive(pAttBoneLink);
}

void CAttachBoneLinkState::Asset(CParam& kParam)
{
    CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);	
    CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    DxAttBoneLink* pAttBoneLink = pView->GetAttBoneLink();    
    DxSkinPiece* pSkinPiece     = pView->GetSkinPiece();
    DxSkeleton* pLinkSkeleton   = pView->GetAttBoneLinkSkeleton();
    CAssetDlg* pAssetDlg        = pFrame->GetAssetDlg();

    if (pAssetDlg == 0)
    {
        return;
    }

    pAssetDlg->Clear();

    // 어태치 본 링크는 어태치 본 외에
    // 링크될 본과 조각 정보도 같이 가진다.
    HTREEITEM hRoot = pAssetDlg->Insert(0, _T("ROOT"));
    AttBoneLinkAsset(pAttBoneLink, pAssetDlg, hRoot);
    SkeletonAsset(pAttBoneLink->m_strBaseBoneName.c_str(), pAssetDlg, hRoot);
    SkinPieceAsset(pSkinPiece, pAssetDlg, hRoot);
}