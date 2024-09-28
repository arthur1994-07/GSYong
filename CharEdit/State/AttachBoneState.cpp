#include "stdafx.h"
#include "AttachBoneState.h"
#include "AttachBoneLinkState.h"

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

CAttachBoneState::CAttachBoneState() :
	CEditState(new CAttachBoneLinkState)
{

}

int CAttachBoneState::GetState()
{
	return CHARTAB_ATTBONEFILE;
}

void CAttachBoneState::Reload(CParam& kParam)
{
	CMainFrame* pFrame				= (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CsheetWithTabChar* pTabChar		= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView			= (CCharEditView*)pFrame->GetActiveView();
	DxAttBone* pAttBone				= pView->GetAttBone();
	DxAttBoneData* pAttBoneData		= pView->GetAttBoneData();

	const TCHAR* pFile = pAttBoneData->GetFileName();

	if (pFile == 0 || _tcslen(pFile) == 0)
	{
        return;
    }

    // 삭제되면 문자열이
    // 삭제 되므로 백업해 두어야 한다.
    TSTRING strFile = pFile;

	for (int i = 0; i < ATTBONE_SIZE; i++)
	{
		DxSkinPiece* pTemp = pAttBone->m_AttBoneArray[i].GetPart();

		if (pTemp && pTemp->GetFileName())
		{
			RemovePiece(pTemp);
			RemovePart(pTemp);
		}
	}

	RemoveAni(pAttBone->m_spSkinAniControlThread.get());

	pTabChar->m_AttBonePage.LoadAbfFile(strFile.c_str());    
}

void CAttachBoneState::Insert(CParam& kParam)
{
	CMainFrame* pFrame			= (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView		= (CCharEditView*)pFrame->GetActiveView();
	DxAttBone* pAttPiece		= pView->GetAttBone();	

	DxCharPart* pCharPart       = 0;
	DxSkinPiece* pSkinPiece     = 0;

    InsertLODData(pAttPiece);

	for (int i = 0; i < ATTBONE_SIZE; i++)
	{
		pCharPart = pAttPiece->m_AttBoneArray + i;
		pSkinPiece = pCharPart->GetPart();		

		if (pSkinPiece != 0 && pSkinPiece->m_szMeshName)
		{
			InsertTexture(pSkinPiece, pCharPart);
			InsertSkeleton(pCharPart, pSkinPiece, pAttPiece->m_spSkinAniControlThread.get(), pView);
			InsertMesh(pCharPart, pSkinPiece, pAttPiece->m_spSkinAniControlThread.get(), pView);
		}
	}

	InsertSkinAni(pAttPiece->m_spSkinAniControlThread.get());
}

void CAttachBoneState::Archive(CParam& kParam)
{
    CMainFrame* pFrame			= (CMainFrame*)AfxGetApp()->m_pMainWnd;
    CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
    CCharEditView* pView		= (CCharEditView*)pFrame->GetActiveView();
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();   
    DxAttBoneData* pAttData		= pView->GetAttBoneData();

    const TCHAR* pFolder = 0;
    kParam >> pFolder;

    Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
    Archive::Archive(pAttData);
}

void CAttachBoneState::Asset(CParam& kParam)
{
    CMainFrame* pFrame			= (CMainFrame*)AfxGetApp()->m_pMainWnd;
    CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
    CCharEditView* pView		= (CCharEditView*)pFrame->GetActiveView();
    DxAttBone* pAttPiece		= pView->GetAttBone();
    CAssetDlg* pAssetDlg        = pFrame->GetAssetDlg();

    if (pAssetDlg == 0)
    {
        return;
    }

    pAssetDlg->Clear();

    HTREEITEM hRoot = pAssetDlg->Insert(0, _T("ROOT"));
    AttBoneAsset(pAttPiece, pAssetDlg, hRoot);
}