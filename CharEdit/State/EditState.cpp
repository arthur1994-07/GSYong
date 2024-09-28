#include "stdafx.h"
#include "EditState.h"
#include "CharEditFileNotifier.h"
#include "CharStateMachineType.h"
#include "LODData.h"

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

NotifierGroupContainer	m_kNotiferGroupContainer;

void CEditState::SendMessage(CParam& kParam)
{
    int nState = 0;
    kParam >> nState;

    if (nState == GetState())
    {
        void (CEditState::*fun) (CParam&) = 0;
        kParam >> fun;

        if (fun != 0)
        {
            (this->*fun)(kParam);
        }

        return;
    }

    kParam >> Reset();

    if (m_spNext != 0)
    {
        m_spNext->SendMessage(kParam);
    }
}

CEditState::~CEditState()
{

}

void CEditState::Destroy()
{
	m_kNotiferGroupContainer.clear();
}

void CEditState::Clear(CParam& kParam)
{
	m_kNotiferGroupContainer[GetState()].Clear();
}

void CEditState::Change(CParam& kParam)
{
	CFileNotifier* pNotifier = 0;

    kParam.Remove();

	BOOST_FOREACH(NotifierGroupContainer::value_type& it, m_kNotiferGroupContainer)	
	{
		pNotifier = it.second.FindNotifier(kParam);

		if (pNotifier != 0)
		{
			static_cast<CFileNotifier*>(pNotifier)->OnChangeFile(kParam);		
		}
	}
}

void CEditState::Update(CParam& kParam)
{
	int n = CFileNotifier::RETURNCODE_FAIL;

	BOOST_FOREACH(NotifierGroupContainer::value_type& it, m_kNotiferGroupContainer)	
	{
		n |= it.second.Notify();

		if (n & CFileNotifier::RETURNCODE_EXIT)
		{
			break;
		}
	}

    kParam << n;
}

void CEditState::InsertTexture(DxSkinPiece* pSkinPiece, DxCharPart* pPart)
{
	CFileNotifier* pNotifier = 0;

	for (DWORD m = 0; m < pSkinPiece->m_dwMaterialNum; ++m)
	{
        if (pSkinPiece->m_pmcMesh == 0)
        {
            continue;
        }

	    pNotifier = new CTextureFileNotifier;

	    if (pNotifier != 0)
	    {
		    pNotifier->Param() << (pSkinPiece->m_arrayMaterialPiece + m);
            pNotifier->Param() << (pSkinPiece->m_pmcMesh->m_pMaterialEx + m);
            pNotifier->Param() << pPart;
		    m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
	    }
	}
}

void CEditState::InsertMesh(DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice)
{
    CFileNotifier* pNotifier = new CMeshFileNotifier;

    if (pNotifier != 0)
    {
        pNotifier->Param() << pSkinPiece << pd3dDevice;
        pNotifier->Param() << pSkinPiece->m_szXFileName;
        m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
    }
}

void CEditState::InsertMesh(DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DxSkinAniControl* pSkinAni, CCharEditView* pView)
{
    CFileNotifier* pNotifier = new CPartMeshFileNotifier;

    if (pNotifier != 0)
    {
        pNotifier->Param() << pCharPart << pSkinPiece << pSkinAni;
        pNotifier->Param() << pSkinPiece->m_szXFileName;
        m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
    }
}

void CEditState::InsertSkeleton(DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice)
{
	CFileNotifier* pNotifier = new CSkeletonFileNotifier;
	
	if (pNotifier != 0)
	{
		pNotifier->Param() << pSkinPiece << pd3dDevice;
		m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
	}
}

void CEditState::InsertSkeleton(DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DxSkinAniControl* pSkinAni, CCharEditView* pView)
{
	CFileNotifier* pNotifier = new CPartSkeletonFileNotifier;

	if (pNotifier != 0)
	{
		pNotifier->Param() << pCharPart << pSkinPiece << pSkinAni << pView;
		m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
	}
}

void CEditState::InsertSkinAni(DxSkinAniControl* pSkinAniCtrl)
{
	CFileNotifier* pNotifier = 0;

	const DxSkinAniControl::VECNODE& kAniFile = pSkinAniCtrl->GetAniList();

	BOOST_FOREACH(const ANIMCONTNODE* p, kAniFile)
	{
		pNotifier = new CAnimationFileNotifier;

		if (pNotifier != 0)
		{
			pNotifier->Param() << p->m_pDxSkinAni;
			m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
		}
	}
}

void CEditState::InsertLODData(DxSkinChar* pkSkinChar, DxSkinPiece* pSkinPiece)
{
    CFileNotifier* pNotifier = 0;
    
#ifndef DEF_SKIN_SYNC
    if (pSkinPiece)
    {
        pNotifier = new CLODData(pkSkinChar, pSkinPiece);
    }
    else
#endif
    {
        pNotifier = new CLODData(pkSkinChar);
    }

    if (pNotifier != 0)
    {
        m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
    }
}

void CEditState::InsertLODData(DxAttBone* pkAttBone, DxSkinPiece* pSkinPiece)
{
    CFileNotifier* pNotifier = 0;

#ifndef DEF_SKIN_SYNC
    if (pSkinPiece)
    {
        pNotifier = new CLODData(pkAttBone, pSkinPiece);
    }
    else
#endif
    {
        pNotifier = new CLODData(pkAttBone);
    }

    if (pNotifier != 0)
    {
        m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
    }
}

void CEditState::InsertLODData(DxVehicle* pkVehicle, DxSkinPiece* pSkinPiece)
{
    CFileNotifier* pNotifier = 0;

#ifndef DEF_SKIN_SYNC
    if (pSkinPiece)
    {
        pNotifier = new CLODData(pkVehicle, pSkinPiece);
    }
    else
#endif
    {
        pNotifier = new CLODData(pkVehicle);
    }

    if (pNotifier != 0)
    {
        m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
    }
}

void CEditState::RemovePiece(DxSkinChar* pSkinChar)
{
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		DxSkinPiece* pPiece = pSkinChar->m_PartArray[i].GetPart();

		if (pPiece != 0 && pPiece->GetFileName() != 0)
		{
			RemovePiece(pPiece);
		}
	}
}

void CEditState::RemovePiece(DxSkinChar* pSkinChar, const TCHAR* pCpsFile)
{
	for (int i = 0; i < PIECE_SIZE; ++i)
	{
		DxSkinPiece* pPiece = pSkinChar->m_PartArray[i].GetPart();

		if (pPiece != 0 && CFileNotifier::IsSameFile(pPiece->GetFileName(), pCpsFile))
		{
			RemovePiece(pPiece);
		}
	}
}

void CEditState::RemovePiece(DxSkinPiece* pSkinPiece)
{
	if (pSkinPiece->m_pmcMesh != 0)
	{
		for (DWORD m = 0; m < pSkinPiece->m_dwMaterialNum; ++m)
		{
			TextureManager::GetInstance().DeleteData(pSkinPiece->m_pmcMesh->pMaterials[m].pTextureFilename);
		}
	}

	if (pSkinPiece->m_szXFileName != 0)
	{
		DxSkinMeshManager::GetInstance().DeleteData(pSkinPiece->m_szXFileName);
	}

	if (pSkinPiece->m_szSkeleton != 0)
	{
		DxSkeletonManager::GetInstance().DeleteData(pSkinPiece->m_szSkeleton);
	}
}

void CEditState::RemoveAni(DxSkinAniControl* pAniCtrl)
{
	const DxSkinAniControl::VECNODE& kAniFile = pAniCtrl->GetAniList();

	BOOST_FOREACH(const ANIMCONTNODE* p, kAniFile)
	{
		DxSkinAnimationManager::GetInstance().DeleteData(p->m_pDxSkinAni->m_strCfgFile);
		DxAnimationListManager::GetInstance().DeleteXFile(p->m_pDxSkinAni->m_strXFile);
		DxAnimationListManager::GetInstance().DeleteData(p->m_pDxSkinAni->m_strBinFile);
	}
}

void CEditState::RemovePart(DxSkinPiece* pSkinPiece)
{
	char ext[_MAX_EXT] = {0};
	_splitpath_s(pSkinPiece->m_szFileName, 0, 0, 0, 0, 0, 0, ext, _MAX_EXT);

	CString strExt(ext);
	strExt.MakeUpper();

    TSTRING strName = pSkinPiece->m_szFileName;

	if (strExt == _T(".ABL"))
	{
		DxAttBoneLinkContainer::GetInstance().DeleteLink(strName.c_str());
	}
	else
	{
		DxSkinPieceContainer::GetInstance().DeletePiece(pSkinPiece);
	}
}

void CEditState::RemoveChar(DxSkinChar* pSkinChar)
{
    for (int i = 0; i < PIECE_SIZE; i++)
    {
        DxSkinPiece* pTemp = pSkinChar->m_PartArray[i].GetPart();

        if (pTemp && pTemp->GetFileName())
        {
            RemovePiece(pTemp);
            RemovePart(pTemp);
        }
    }

    RemoveAni(pSkinChar->m_spSkinAniControlThread.get());
}

void CEditState::SkinPieceAsset(DxSkinPiece* pSkinPiece, CAssetDlg* pDlg, HTREEITEM hRoot, bool bBone)
{
    HTREEITEM hSkin = 0;
    TCHAR temp[MAX_PATH] = {0};
    TSTRING strBone, strSkin, strTexture;

    // 본 파일
    if (bBone)
    {        
        strBone = CSkeletonFileNotifier::GetFile(pSkinPiece, false);

        if (strBone.empty() == false)
        {
            _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("본"), strBone.c_str());
            pDlg->Insert(hRoot, temp);
        }
    }

    // 스킨 파일    
    strSkin = CMeshFileNotifier::GetFile(pSkinPiece, false);

    if (strSkin.empty() == false)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("스킨"), strSkin.c_str());
        hSkin = pDlg->Insert(hRoot, temp);
    }

    // 텍스처 파일
    for (DWORD m = 0; m < pSkinPiece->m_dwMaterialNum; ++m)
    {
        TSTRING strTexture = CTextureFileNotifier::GetFile(pSkinPiece->m_arrayMaterialPiece + m, 
            pSkinPiece->m_pmcMesh->m_pMaterialEx + m, false);
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("텍스처"), strTexture.c_str());  
        pDlg->Insert(hSkin, temp);
    }
}

void CEditState::SkinCharAsset(DxSkinChar* pSkinChar, CAssetDlg* pDlg, HTREEITEM hRoot)
{
    TCHAR temp[MAX_PATH]            = {0};
    DxCharPart* pCharPart           = 0;
    DxSkinPiece* pSkinPiece         = 0;
    DxSkinAniControl* pSkinAniCtrl  = 0;

    // 캐릭터 설정 파일(chf)
    if (pSkinChar->GetFileName() && _tcsclen(pSkinChar->GetFileName()) > 0)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("캐릭터"), pSkinChar->GetFileName());
        hRoot = pDlg->Insert(hRoot, temp);
    }

    // 본 파일
    TSTRING strBone = CSkeletonFileNotifier::GetFile(pSkinChar->m_spSkinAniControlThread.get(), false);

    if (strBone.empty() == false)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("본"), strBone.c_str());
        pDlg->Insert(hRoot, temp);
    }

    // 파츠 파일
    HTREEITEM hPart = 0;

    for (int i = 0; i < PIECE_SIZE; i++)
    {
        pCharPart = pSkinChar->m_PartArray + i;
        pSkinPiece = pCharPart->GetPart();
        pSkinAniCtrl = pSkinChar->m_spSkinAniControlThread.get();

        if (pSkinPiece && pSkinPiece->m_pmcMesh)
        {
            // 조각 설정 파일(cps, aps, vps)
            _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), DxSkinPiece::m_szPIECETYPE[i], pSkinPiece->GetFileName());
            HTREEITEM hPiece = pDlg->Insert(hRoot, temp);
            SkinPieceAsset(pSkinPiece, pDlg, hPiece);
        }
    }

    // 애니 파일
    AnimationAsset(pSkinChar->m_spSkinAniControlThread.get(), pDlg, hRoot);
}

void CEditState::AnimationAsset(DxSkinAniControl* pAniCtrl, CAssetDlg* pDlg, HTREEITEM hRoot)
{
    TCHAR temp[MAX_PATH] = {0};
    const DxSkinAniControl::VECNODE& kAniFile = pAniCtrl->GetAniList();

    TSTRING strMain;

    HTREEITEM hAni = 0;

    BOOST_FOREACH(const ANIMCONTNODE* p, kAniFile)
    {
        if (hRoot)
        {
            hAni = pDlg->Insert(hRoot, _T("애니"));
            hRoot = 0;
        }

        strMain = CAnimationFileNotifier::GetFile(p->m_pDxSkinAni, false);
        pDlg->Insert(hAni, strMain.c_str());        

        strMain = CAnimationFileNotifier::GetCFGFile(p->m_pDxSkinAni, false);
        pDlg->Insert(hAni, strMain.c_str());        
    }
}

void CEditState::AttBoneAsset(DxAttBone* pAttPiece, CAssetDlg* pDlg, HTREEITEM hRoot)
{
    TCHAR temp[MAX_PATH]            = {0};
    DxCharPart* pCharPart           = 0;
    DxSkinPiece* pSkinPiece         = 0;
    DxSkinAniControl* pSkinAniCtrl  = 0;

    // 어태치 본 설정 파일(abf)
    if (pAttPiece->GetFileName() && _tcsclen(pAttPiece->GetFileName()) > 0)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("어태치 본"), pAttPiece->GetFileName());
        hRoot = pDlg->Insert(hRoot, temp);
    }

    // 본 파일
    TSTRING strBone = CSkeletonFileNotifier::GetFile(pAttPiece->m_spSkinAniControlThread.get(), false);

    if (strBone.empty() == false)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("본"), strBone.c_str());
        pDlg->Insert(hRoot, temp);
    }

    // 파츠 파일
    HTREEITEM hPart = 0;

    for (int i = 0; i < ATTBONE_SIZE; i++)
    {
        pCharPart = pAttPiece->m_AttBoneArray + i;
        pSkinPiece = pCharPart->GetPart();
        pSkinAniCtrl = pAttPiece->m_spSkinAniControlThread.get();

        if (pSkinPiece && pSkinPiece->m_pmcMesh)
        {
            // 조각 설정 파일(cps, aps, vps)
            _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), DxSkinPiece::m_szATTBONETYPE[i], pSkinPiece->GetFileName());
            HTREEITEM hPiece = pDlg->Insert(hRoot, temp);
            SkinPieceAsset(pSkinPiece, pDlg, hPiece);
        }
    }

    // 애니 파일
    AnimationAsset(pAttPiece->m_spSkinAniControlThread.get(), pDlg, hRoot);
}

void CEditState::AttBoneLinkAsset(DxAttBoneLink* pAttLink, CAssetDlg* pDlg, HTREEITEM hRoot)
{
    TCHAR temp[MAX_PATH] = {0};

    // 어태치 본 링크 설정 파일(abl)
    if (pAttLink->GetFileName() && _tcsclen(pAttLink->GetFileName()) > 0)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("어태치 본 링크"), pAttLink->GetFileName());
        hRoot = pDlg->Insert(hRoot, temp);
    }

    // 어태치 본
    AttBoneAsset(pAttLink->GetAttBone(), pDlg, hRoot);
}

void CEditState::SkeletonAsset(const TCHAR* pSkeletonFile, CAssetDlg* pDlg, HTREEITEM hRoot)
{
    TCHAR temp[MAX_PATH] = {0};

    // 본 파일
    if (pSkeletonFile && _tcsclen(pSkeletonFile) > 0)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("본"), pSkeletonFile);
        pDlg->Insert(hRoot, temp);
    }
}

void CEditState::VehicleAsset(DxVehicle* pVehicle, CAssetDlg* pDlg, HTREEITEM hRoot)
{
    TCHAR temp[MAX_PATH]            = {0};
    DxCharPart* pCharPart           = 0;
    DxSkinPiece* pSkinPiece         = 0;
    DxSkinAniControl* pSkinAniCtrl  = 0;

    // 비어클 설정 파일(vbf)
    if (pVehicle->GetFileName() && _tcsclen(pVehicle->GetFileName()) > 0)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("비어클"), pVehicle->GetFileName());
        hRoot = pDlg->Insert(hRoot, temp);
    }

    // 본 파일
    TSTRING strBone = CSkeletonFileNotifier::GetFile(pVehicle->m_spSkinAniControlThread.get(), false);

    if (strBone.empty() == false)
    {
        _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), _T("본"), strBone.c_str());
        pDlg->Insert(hRoot, temp);
    }

    // 파츠 파일
    HTREEITEM hPart = 0;

    for (int i = 0; i < PART_V_SIZE; i++)
    {
        pCharPart = pVehicle->m_V_PartsArray + i;
        pSkinPiece = pCharPart->GetPart();
        pSkinAniCtrl = pVehicle->m_spSkinAniControlThread.get();

        if (pSkinPiece && pSkinPiece->m_pmcMesh)
        {            
            // 조각 설정 파일(cps, aps, vps)
            _sntprintf_s(temp, MAX_PATH, _T("%s\t%s"), DxSkinPiece::m_szV_PARTSTYPE[i], pSkinPiece->GetFileName());
            HTREEITEM hPiece = pDlg->Insert(hRoot, temp);
            SkinPieceAsset(pSkinPiece, pDlg, hPiece);
        }
    }

    // 애니 파일
    AnimationAsset(pVehicle->m_spSkinAniControlThread.get(), pDlg, hRoot);
}
