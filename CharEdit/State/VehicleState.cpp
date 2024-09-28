#include "stdafx.h"
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

CVehicleState::CVehicleState() :
	CEditState(0)
{

}

int CVehicleState::GetState()
{
	return CHARTAB_VEHICLE;
}

void CVehicleState::Reload(CParam& kParam)
{
	CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
	DxVehicle* pVehicle			= pView->GetVehicle();
	DxVehicleData* pVehicleData	= pView->GetVehicleData();

    const TCHAR* pFile = pVehicleData->GetFileName();

    if (pFile == 0 || _tcslen(pFile) == 0)
    {
        return;
    }

    // 삭제되면 문자열이
    // 삭제 되므로 백업해 두어야 한다.
    TSTRING strFile = pFile;

	for (int i = 0; i < PART_V_SIZE; i++)
	{
		DxSkinPiece* pTemp = pVehicle->m_V_PartsArray[i].GetPart();

		if (pTemp && pTemp->GetFileName())
		{
			RemovePiece(pTemp);
			RemovePart(pTemp);
		}
	}

	RemoveAni(pVehicle->m_spSkinAniControlThread.get());
	pTabChar->m_VehiclePage.LoadVcfFile(strFile.c_str());
}

void CVehicleState::Insert(CParam& kParam)
{
	CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
	CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
	CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
	DxVehicle* pVehicle			= pView->GetVehicle();

	DxCharPart* pCharPart       = 0;
	DxSkinPiece* pSkinPiece     = 0;

    InsertLODData(pVehicle);

	for (int i = 0; i < PART_V_SIZE; i++)
	{
		pCharPart = pVehicle->m_V_PartsArray + i;
		pSkinPiece = pCharPart->GetPart();

        if (pSkinPiece != 0 && pSkinPiece->m_szMeshName)
        {
		    InsertTexture(pSkinPiece, pCharPart);
		    InsertSkeleton(pCharPart, pSkinPiece, pVehicle->m_spSkinAniControlThread.get(), pView);
		    InsertMesh(pCharPart, pSkinPiece, pVehicle->m_spSkinAniControlThread.get(), pView);
        }
	}

	InsertSkinAni(pVehicle->m_spSkinAniControlThread.get());
}

void CVehicleState::Archive(CParam& kParam)
{
    CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
    CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
    CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    LPDIRECT3DDEVICEQ pDevice   = pView->GetD3dDevice();    
    DxVehicleData* pVehicleData = pView->GetVehicleData();

    const TCHAR* pFolder = 0;
    kParam >> pFolder;

    Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);
    Archive::Archive(pVehicleData);
}

void CVehicleState::Asset(CParam& kParam)
{
    CMainFrame* pFrame			= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
    CsheetWithTabChar* pTabChar	= pFrame->m_cDlgEdit.GetSheetChar();
    CCharEditView* pView		= static_cast<CCharEditView*>(pFrame->GetActiveView());
    DxVehicle* pVehicle			= pView->GetVehicle();
    CAssetDlg* pAssetDlg        = pFrame->GetAssetDlg();

    if (pAssetDlg == 0)
    {
        return;
    }

    pAssetDlg->Clear();

    HTREEITEM hRoot = pAssetDlg->Insert(0, _T("ROOT"));
    VehicleAsset(pVehicle, pAssetDlg, hRoot);
}