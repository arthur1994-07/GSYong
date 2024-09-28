#include "stdafx.h"
#include "CharPieceState.h"
#include "AttachBoneState.h"

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

CCharPieceState::CCharPieceState() :
	CEditState(new CAttachBoneState)
{

}

int CCharPieceState::GetState()
{
#ifndef DEF_SKIN_SYNC
	return CHARTAB_CHARPIECE;
#else
    return -1;
#endif
}

void CCharPieceState::SetFocus(CParam& kParam)
{
    int nOld = 0;
    kParam >> nOld;

#ifdef DEF_ERASE
	if (nOld != CHARTAB_CHARFILE)
	{
		return;
	}

	CMainFrame* pFrame		= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CCharEditView* pView	= static_cast<CCharEditView*>(pFrame->GetActiveView());
	DxSkinChar* pSkinChar	= pView->GetSkinChar();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();

    if (pSkinChar && pSkinPiece && pSkinChar->EffectSync(pSkinPiece, false))
    {
        CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
        DlgEdit.GetSheetChar()->m_SkinPiecePage.EffectListting();
    }
#endif
}

void CCharPieceState::Reload(CParam& kParam)
{
	CMainFrame* pFrame				= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CsheetWithTabChar* pTabChar		= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView			= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice       = pView->GetD3dDevice();
	DxSkinChar* pSkinChar			= pView->GetSkinChar();
	DxSkinCharData* pSkinCharData	= pView->GetSkinCharData();
	DxSkinPiece* pSkinPiece 		= pView->GetSkinPiece();    

	if (pSkinPiece == 0)
	{
		return;
	}

#ifdef DEF_ERASE
    // 삭제되면 문자열이
    // 삭제 되므로 백업해 두어야 한다.
    TSTRING strFile = pSkinPiece->GetFileName();

    RemovePiece(pSkinPiece);

	pTabChar->m_SkinPiecePage.LoadSkinPiece(strFile.c_str());

    // 조각을 새로 로딩 했으므로
    // 캐릭터와 동기화 시켜 준다.
    pSkinChar->ReAttachPart(pSkinPiece, pDevice);
    pSkinChar->ReAttachBoneForEdit();
    pSkinChar->ReAttachEffect();

    // 1번 랜더를 해줘야 동기화가 맞춰 진다;;
    pSkinChar->Render(pDevice, pView->GetSkinMatrix());
#endif
}

void CCharPieceState::Insert(CParam& kParam)
{
	CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();
	DxSkinPiece* pSkinPiece 	= pView->GetSkinPiece();

    DxSkinChar* pSkinChar       = pView->GetSkinChar();
    DxVehicle* pVehicle 	    = pView->GetVehicle();
    DxAttBone* pAttBone 	    = pView->GetAttBone();

	if (pSkinPiece->m_pmcMesh == 0)
	{
		return;
	}

    InsertLODData(pSkinChar, pSkinPiece);
    InsertLODData(pVehicle, pSkinPiece);
    InsertLODData(pAttBone, pSkinPiece);

	InsertTexture(pSkinPiece, 0);
	InsertSkeleton(pSkinPiece, pDevice);
	InsertMesh(pSkinPiece, pDevice);
}

void CCharPieceState::Archive(CParam& kParam)
{
    CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
    CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
    CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();
    DxSkinPiece* pSkinPiece 	= pView->GetSkinPiece();

    if (pSkinPiece->m_pmcMesh == 0)
    {
        return;
    }

    const TCHAR* pFolder = 0;
    kParam >> pFolder;

    Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
    Archive::Archive(pSkinPiece);
}

void CCharPieceState::Asset(CParam& kParam)
{
    CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
    CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
    CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    DxSkinPiece* pSkinPiece 	= pView->GetSkinPiece();
    CAssetDlg* pAssetDlg        = pFrame->GetAssetDlg();

    if (pAssetDlg == 0)
    {
        return;
    }

    pAssetDlg->Clear();

    HTREEITEM hRoot = pAssetDlg->Insert(0, _T("ROOT"));
    SkinPieceAsset(pSkinPiece, pAssetDlg, hRoot, true);    
}