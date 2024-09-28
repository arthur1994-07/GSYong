#include "stdafx.h"
#include "CharFileState.h"
#include "CharPieceState.h"

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

CCharFileState::CCharFileState() : 
	CEditState(new CCharPieceState)
{

}

int CCharFileState::GetState()
{
	return CHARTAB_CHARFILE;
}

void CCharFileState::SetFocus(CParam& kParam)
{
    int nOld = 0;
    kParam >> nOld;

#ifdef DEF_ERASE
	if (nOld != CHARTAB_CHARPIECE)
	{
		return;
	}

	CMainFrame* pFrame		= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CCharEditView* pView	= static_cast<CCharEditView*>(pFrame->GetActiveView());
	DxSkinChar* pSkinChar	= pView->GetSkinChar();
	DxSkinPiece* pSkinPiece = pView->GetSkinPiece();

    if (pSkinChar && pSkinPiece && pSkinChar->EffectSync(pSkinPiece, true))
    {
        CDlgEdit &DlgEdit = pFrame->m_cDlgEdit;
        DlgEdit.GetSheetChar()->m_CharPage.GetAnimationTab()->AniEffName(FALSE);
    }
#endif
}

void CCharFileState::Reload(CParam& kParam)
{
	CMainFrame* pFrame				= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CsheetWithTabChar* pTabChar		= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView			= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pd3dDevice    = pView->GetD3dDevice();
	DxSkinChar* pSkinChar			= pView->GetSkinChar();
	DxSkinCharData* pSkinCharData	= pView->GetSkinCharData();
	DxSkinPiece* pSkinPiece 		= pView->GetSkinPiece();

	TSTRING strFile = pSkinCharData->GetFileName();

	if (strFile.empty() == true)
	{
		return;
	}

    RemoveChar(pSkinChar);

	pTabChar->m_CharPage.LoadChfFile(strFile.c_str());

#ifdef DEF_ERASE
    if (pSkinPiece->m_szFileName)
    {
        // 캐릭터를 새로 로딩 했으므로
        // 피스와 동기화 시켜 준다.
        pSkinPiece->SetLODSkinMesh(pSkinPiece->m_kLODInfo.nLevel, pd3dDevice, FALSE, FALSE);
        pSkinPiece->NSThreadCORE_UpdateData(pd3dDevice);
        pSkinPiece->UpdateEffect(pSkinPiece->m_vecEFFECT, pd3dDevice);
    }
#endif
}

void CCharFileState::Insert(CParam& kParam)
{
	CMainFrame* pFrame		= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CCharEditView* pView	= static_cast<CCharEditView*>(pFrame->GetActiveView());
	DxSkinChar* pSkinChar	= pView->GetSkinChar();

	DxCharPart* pCharPart   = 0;
	DxSkinPiece* pSkinPiece = 0;    

    InsertLODData(pSkinChar);

	for (int i = 0; i < PIECE_SIZE; i++)
	{
		pCharPart = pSkinChar->m_PartArray + i;
		pSkinPiece = pCharPart->GetPart();

		if (pSkinPiece != 0 && pSkinPiece->m_szMeshName)
		{
			InsertTexture(pSkinPiece, pCharPart);
			InsertSkeleton(pCharPart, pSkinPiece, pSkinChar->m_spSkinAniControlThread.get(), pView);
			InsertMesh(pCharPart, pSkinPiece, pSkinChar->m_spSkinAniControlThread.get(), pView);
		}
	}

	InsertSkinAni(pSkinChar->m_spSkinAniControlThread.get());
}

void CCharFileState::Archive(CParam& kParam)
{
    CMainFrame* pFrame		    = static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
    CCharEditView* pView	    = static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();    
    DxSkinCharData* pSkinData   = pView->GetSkinCharData();    

    const TCHAR* pFolder    = 0;
    kParam >> pFolder;

    Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
    Archive::Archive(pSkinData);
}

void CCharFileState::Asset(CParam& kParam)
{
    CMainFrame* pFrame		= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
    CCharEditView* pView	= static_cast<CCharEditView*>(pFrame->GetActiveView());
    DxSkinChar* pSkinChar	= pView->GetSkinChar();
    CAssetDlg* pAssetDlg    = pFrame->GetAssetDlg();

    if (pAssetDlg == 0)
    {
        return;
    }

    pAssetDlg->Clear();

    HTREEITEM hRoot = pAssetDlg->Insert(0, _T("ROOT"));
    SkinCharAsset(pSkinChar, pAssetDlg, hRoot);
}