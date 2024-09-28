#include "stdafx.h"
#include "CharEditFileNotifier.h"
#include "CharStateMachine.h"
#include <sys/stat.h>

#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../EngineLib/DxMeshs/SkinMesh/DxCharPart.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

bool CTextureFileNotifier::Init()
{	
	SMATERIAL_PIECE* pMtlPiece = 0;
	DXMATERIALEX* pDxMtl = 0;
	Param() >> pMtlPiece >> pDxMtl >> Reset();

	TSTRING strPath;

	strPath = GetFile(pMtlPiece, pDxMtl);

	return SetFile(strPath);
}

int CTextureFileNotifier::OnNotify()
{
	DxCharPart* pCharPart = 0;
	SMATERIAL_PIECE* pMtlPiece = 0;
	LPD3DXMATERIAL pDxMtl = 0;
	Param() >> pMtlPiece >> pDxMtl >> pCharPart >> Reset();

	pMtlPiece->m_strTexture = GetFileName();    
	pMtlPiece->LoadTexture();

	if (pCharPart != 0)
	{
		pCharPart->ReSetPart_Detail();
	}

	return RETURNCODE_SUCCESS;
}

void CTextureFileNotifier::OnChangeFile(CParam& kParam)
{
	TCHAR* pOrg = 0, *pChange = 0;
	kParam >> pOrg >> pChange >> Reset();

	TSTRING strFile = pChange;
	TextureManager::GetInstance().FindFilePath(strFile);

	bool bResult = SetFile(strFile);

	if (bResult == true)
	{
		OnNotify();
	}
}

TSTRING CTextureFileNotifier::GetFile(SMATERIAL_PIECE* pMtlPiece, DXMATERIALEX* pDxMtl, bool bPath)
{
	TSTRING strFile;

    if (pMtlPiece == 0 || pDxMtl == 0)
    {
        return strFile;
    }

	if (pMtlPiece->m_textureRes.IsValid())
	{
		strFile = pMtlPiece->m_textureRes.GetFileName();
	}
	else
	{
		strFile = pDxMtl->m_textureRes.GetFileName();
	}

    if (bPath)
    {
        TextureManager::GetInstance().FindFilePath(strFile);
    }

    //assert(strFile.empty() == false);

	if ( strFile.empty() )
	{
		AfxMessageBox( _T("텍스쳐가 선언되어있지 않습니다.") );
	}

    return strFile;
}

bool CSkeletonFileNotifier::Init()
{
	DxSkinPiece* pSkinPiece = 0;
	Param() >> pSkinPiece >> Reset();

	TSTRING strPath = GetFile(pSkinPiece);

	return SetFile(strPath);;
}

int CSkeletonFileNotifier::OnNotify()
{
	DxSkinPiece* pSkinPiece = 0;
    LPDIRECT3DDEVICEQ pd3dDevice = 0;
	Param() >> pSkinPiece >> pd3dDevice >> Reset();

	if (pSkinPiece->IsValidSkinMesh() == false)
	{
		pSkinPiece->ReAttachSkinMesh( pd3dDevice );
	}
	else
	{
		// pSkinPiece->ReSetSkinMesh( 0 ); 이렇게 되어 있었다. 문제가 생길지도 모르겠다. [2014.03.05][shhan]
		pSkinPiece->ReSetSkinMesh( pd3dDevice );
	}    

    //pSkinPiece->UpdateEffect(pSkinPiece->m_vecEFFECT, pd3dDevice);

	return RETURNCODE_SUCCESS;
}

TSTRING CSkeletonFileNotifier::GetFile(DxSkinPiece* pSkinPiece, bool bPath)
{
    if (bPath)
    {
        return DxSkeletonManager::GetInstance().GetPath() + pSkinPiece->m_skeletonRes.GetFileName();
    }
    else
    {
        return pSkinPiece->m_skeletonRes.GetFileName();
    }
}

TSTRING CSkeletonFileNotifier::GetFile(DxSkinAniControl* pSkinAni, bool bPath)
{
    if (bPath)
    {
        return DxSkeletonManager::GetInstance().GetPath() + pSkinAni->GetSkeletonName();
    }
    else
    {
        return pSkinAni->GetSkeletonName();
    }
}

bool CMeshFileNotifier::Init()
{
	DxSkinPiece* pSkinPiece = 0;
    LPDIRECT3DDEVICEQ pd3dDevice = 0;
    const TCHAR* pFileName = 0;
	Param() >> pSkinPiece >> pd3dDevice >> pFileName >> Reset();

	TSTRING strPath = DxSkinMeshManager::GetInstance().GetPath() + pFileName;

	return SetFile(strPath);
}

TSTRING CMeshFileNotifier::GetFile(DxSkinPiece* pSkinPiece, bool bPath)
{
    if (bPath)
    {
        return DxSkinMeshManager::GetInstance().GetPath() + pSkinPiece->m_skinMeshRes.GetFileName();
    }
    else
    {
        return pSkinPiece->m_skinMeshRes.GetFileName();
    }
}

bool CPartSkeletonFileNotifier::Init()
{
	DxCharPart* pCharPart = 0;
	DxSkinPiece* pSkinPiece = 0;
	DxSkinAniControl* pSkinAniCtrl = 0;
	Param() >> pCharPart >> pSkinPiece >> pSkinAniCtrl >> Reset();

    if (pSkinPiece->m_szSkeleton == 0)
    {
        return false;
    }

	TSTRING strPath = DxSkeletonManager::GetInstance().GetPath() + pSkinPiece->m_szSkeleton;

	return SetFile(strPath);
}

int CPartSkeletonFileNotifier::OnNotify()
{
	DxCharPart* pCharPart = 0;
	DxSkinPiece* pSkinPiece = 0;
	DxSkinAniControl* pSkinAni = 0;
    CCharEditView* pView = 0;
	Param() >> pCharPart >> pSkinPiece >> pSkinAni >> pView >> Reset();

	if (pSkinPiece->IsValidSkinMesh() == false)
	{
		pSkinPiece->ReAttachSkinMesh(pView->GetD3dDevice());
	}
	else
	{
		pSkinPiece->ReSetSkinMesh(pView->GetD3dDevice());
	}

	pCharPart->ReSetPart_Detail();
#ifdef DEF_ERASE
    pCharPart->ReAttachEffect();
#endif

    pSkinAni->ReAttachBoneForEdit();

#ifndef DEF_SKIN_SYNC
    // 1번 랜더를 해줘야 동기화가 맞춰 진다;;    
    //pSkinAni->CalculateBone_PureThread_HARD( pView->GetSkinMatrix() );

	//pSkinAni->FrameMoveJiggleBoneON( 1.f, pView->GetSkinMatrix() );
#endif

	const DxSkinAniControl::VECNODE& kAniFile = pSkinAni->GetAniList();

	// 본이 재로딩 되므로 애니도 재로딩 한다.
	BOOST_FOREACH(const ANIMCONTNODE* p, kAniFile)
	{
		p->m_pDxSkinAni->CreateAnimationDataFromTool(TRUE);
	}

	return RETURNCODE_SUCCESS;
}

bool CPartMeshFileNotifier::Init()
{
	DxCharPart* pCharPart = 0;
	DxSkinPiece* pSkinPiece = 0;
	DxSkinAniControl* pSkinAniCtrl = 0;
    CCharEditView* pView = 0;
    const TCHAR* pFileName = 0;
	Param() >> pCharPart >> pSkinPiece >> pSkinAniCtrl >> pView >> pFileName >> Reset();

	TSTRING strPath = DxSkinMeshManager::GetInstance().GetPath() + pFileName;

	return SetFile(strPath);
}

bool CAnimationFileNotifier::Init()
{
	DxSkinAnimation* pSkinAni = 0;
	Param() >> pSkinAni >> Reset();

	TSTRING strPath = DxSkinAnimationManager::GetInstance().GetPath() + pSkinAni->m_strXFile;

	return SetFile(strPath);
}

int CAnimationFileNotifier::OnNotify()
{
	DxSkinAnimation* pSkinAni = 0;
	Param() >> pSkinAni >> Reset();

	pSkinAni->CreateAnimationDataFromTool(TRUE);

	return RETURNCODE_SUCCESS;
}

TSTRING CAnimationFileNotifier::GetCFGFile(DxSkinAnimation* pSkinAnimation, bool bPath)
{
    if (bPath)
    {
        return DxSkinAnimationManager::GetInstance().GetPath() + pSkinAnimation->m_strCfgFile;
    }
    else
    {
        return pSkinAnimation->m_strCfgFile;
    }
}

TSTRING CAnimationFileNotifier::GetFile(DxSkinAnimation* pSkinAnimation, bool bPath)
{
    if (bPath)
    {
        return DxSkinAnimationManager::GetInstance().GetPath() + pSkinAnimation->GetAniListResource().GetFileName();
    }
    else
    {
        return pSkinAnimation->GetAniListResource().GetFileName();
    }
}