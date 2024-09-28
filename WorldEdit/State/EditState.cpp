#include "stdafx.h"
#include "EditState.h"
#include "WorldEditFileNotifier.h"
#include "WorldStateMachineType.h"
#include <boost/bind.hpp>

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

#include "../../enginelib/DxLand/DxPiece/DxPieceManagerWLD.h"
#include "../../enginelib/DxLand/DxPiece/DxPieceContainer.h"
#include "../../enginelib/DxLand/DxPiece/DxPieceStaticAni.h"
#include "../../enginelib/DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../enginelib/DxLand/DxPiece/DxPieceAABB.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../enginelib/DxEffect/EffProj/DxEffProj.h"
#include "../../enginelib/DxEffect/Mesh/DxFrameAniMan.h"
#include "../../enginelib/DxEffect/Mesh/DxFrameAni.h"
#include "../../EngineLib/DxSound/DxSoundMan.h"
#include "../../EngineLib/DxSound/BgmSound.h"
#include "../../enginelib/Water/WaterSurf.h"
#include "../../enginelib/Water/WaterSurfPointMan.h"

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

void CEditState::Update(CParam& kParam)
{
    /* 
        현재 많은 피스들이 있을 때,
        파일 갱신기를 호출 할떄 버벅임이 발생 한다.
        예를들어 피스가 1000개 이상시 발생 한다.
        이를 일딴 막기 위해 파일 갱신 기능을 막아 둔다.
        향후 시간이 되면 수정해야 한다.

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
    */
}

bool CEditState::IsDuplicatedNotifier(const TCHAR* pName)
{
	if (pName == 0)
	{
		return false;
	}

	DWORD nSize = _tcslen(pName);

	if (nSize == 0)
	{
		return false;
	}

	CParam kTemp;
	kTemp << pName;
	return (m_kNotiferGroupContainer[GetState()].FindNotifier(kTemp) == 0);
}

void CEditState::InsertFrameMeshX(DxSetLandMan* pSetLandMan, CsheetWithTab* pSheetWithTab)
{
	CFileNotifier* pNotifier = new CFrameMeshXFileNotifier;

	if (pNotifier != 0)
	{
		pNotifier->Param() << pSetLandMan << pSheetWithTab;
		m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
	}
}

void CEditState::InsertMeshTexture(DxFrame* pFrameRoot)
{
	if (pFrameRoot != 0)
	{
		if (pFrameRoot->pmsMeshs != 0)
		{
			for (DWORD i = 0; i < pFrameRoot->pmsMeshs->m_cMaterials; ++i)
			{
				InsertTexture(pFrameRoot->pmsMeshs->m_strTextureFiles[i].GetString(), 
					pFrameRoot->pmsMeshs->m_pTextureRes + i);
			}
		}

		InsertMeshTexture(pFrameRoot->pframeFirstChild);
		InsertMeshTexture(pFrameRoot->pframeSibling);
	}
}

void CEditState::InsertTexture(const TCHAR* pFile, TextureManager::Resource* pRes)
{
	if (IsDuplicatedNotifier(pFile) == false)
	{
		return;
	}

	CFileNotifier* pNotifier = new CTextureFileNotifier;

	if (pNotifier != 0)
	{
		pNotifier->Param() << pFile << pRes;
		m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
	}
}

void CEditState::InsertPieceTexture(MAPSINGLETEXMESH& kMesh)
{
	const TCHAR* pFile = 0;
	MAPSINGLETEXMESH_ITER iter = kMesh.begin();

	for ( ; iter != kMesh.end(); ++iter)
	{
		pFile = iter->second->GetTextureName().c_str();

		InsertTexture(pFile, &iter->second->GetTextureRes());
	}
}

void CEditState::InsertPieceTexture(DxStaticAniFrame* pStaticAniFrame)
{
	if (pStaticAniFrame != 0)
	{
		DxStaticMesh* pStaticMesh = pStaticAniFrame->GetStaticMesh();

		if (pStaticMesh != 0)
		{
			InsertPieceTexture(pStaticMesh->m_mapMesh);
			InsertPieceTexture(pStaticMesh->m_mapMeshALPHA);
			InsertPieceTexture(pStaticMesh->m_mapMeshSOFTALPHA);
			InsertPieceTexture(pStaticMesh->m_mapMeshSOFTALPHA01);
			InsertPieceTexture(pStaticMesh->m_mapMeshSOFTALPHA02);
		}
	}
}

void CEditState::InsertReplacePiece(DxPieceManagerPSF* pPieceMng)
{	
	DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;
	DWORD dwSize = pPieceMng->GetChangePieceSize();

	for (DWORD i = 0; i < dwSize; ++i)
	{
		pPieceInfo = pPieceMng->GetChangeMATRIXPISNAME(i);

		if (pPieceInfo != 0)
		{
			CFileNotifier* pNotifier = new CPieceFileNotifier;

			if (pNotifier != 0)
			{
				pNotifier->Param() << pPieceInfo;
				m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
			}

			InsertPieceTexture(pPieceInfo->m_pPiece->GetStaticAniFrame());
		}
	}
}

void CEditState::InsertPiece(DxPieceManagerPSF* pPieceMng)
{	
	DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;
	DWORD dwSize = pPieceMng->GetPointPieceSize();

	for (DWORD i = 0; i < dwSize; ++i)
	{
		pPieceInfo = pPieceMng->GetMATRIXPISNAME(i);	

		if (pPieceInfo != 0)
		{
			CFileNotifier* pNotifier = new CPieceFileNotifier;

			if (pNotifier != 0)
			{
				pNotifier->Param() << pPieceInfo;
				m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
			}

			if (pPieceInfo->m_pPiece)
			{
				InsertPieceTexture(pPieceInfo->m_pPiece->GetStaticAniFrame());
			}
		}
	}
}

void CEditState::InsertPiece(DxPieceManagerWLD* pPieceMng)
{
	DxPieceAABB* pPieceAABB = pPieceMng->GetPieceAABB();
	if ( !pPieceAABB )
		return;

	VEC_PIECE& vecPiece = pPieceAABB->GetVecPiece();

	for ( DWORD i=0; i<vecPiece.size(); ++i )
	{
		DxPiece* pPiece = vecPiece[i];
		if (pPiece)
		{
			CFileNotifier* pNotifier = new CPieceWldFileNotifier;
			if ( pNotifier != 0)
			{
				pNotifier->Param() << pPiece;
				m_kNotiferGroupContainer[GetState()].InsertNotifier(pNotifier);
			}

			InsertPieceTexture(pPiece->GetStaticAniFrame());
		}
	}
}

void CEditState::RemoveAllTexture()
{
	TextureManager::GetInstance().DeleteCacheDataAll();
}

void CEditState::RemoveAllPiece()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceEdit* pPieceEdit = pView->GetPieceEdit();	
	DxPieceContainer::GetInstance().CleanUp();
	DxPieceContainer::GetInstancePSF().CleanUp();
}

void CEditState::InsertFrame(DxFrame* pFrame, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    if (pFrame)
    {
        if (hNode == 0)
        {
            hNode = pAssetDlg->Insert(hNode, g_strEditSheet[FRAMEPAGEINDEX], false, 0);
        }

        HTREEITEM hParent = pAssetDlg->Insert(hNode, pFrame->szName, false, reinterpret_cast<DWORD>(pFrame));

        if (pFrame->pmsMeshs != 0 && pFrame->pmsMeshs->m_cMaterials > 0)
        {
            pAssetDlg->Insert(hParent, pFrame->pmsMeshs->m_pTextureRes->GetFileName().c_str(), false);
        }

        InsertFrame(pFrame->pframeSibling, pAssetDlg, hNode);
        InsertFrame(pFrame->pframeFirstChild, pAssetDlg, hParent);
    }
}

void CEditState::InsertPieceTexture(MAPSINGLETEXMESH& kMesh, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    const TCHAR* pFile = 0;
    MAPSINGLETEXMESH_ITER iter = kMesh.begin();

    for ( ; iter != kMesh.end(); ++iter)
    {
        pAssetDlg->Insert(hNode, iter->second->GetTextureName().c_str(), false);
    }
}

void CEditState::InsertPieceTexture(DxStaticAniFrame* pStaticAniFrame, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    if (pStaticAniFrame != 0)
    {
        DxStaticMesh* pStaticMesh = pStaticAniFrame->GetStaticMesh();

        if (pStaticMesh != 0)
        {
            InsertPieceTexture(pStaticMesh->m_mapMesh, pAssetDlg, hNode);
            InsertPieceTexture(pStaticMesh->m_mapMeshALPHA, pAssetDlg, hNode);
            InsertPieceTexture(pStaticMesh->m_mapMeshSOFTALPHA, pAssetDlg, hNode);
            InsertPieceTexture(pStaticMesh->m_mapMeshSOFTALPHA01, pAssetDlg, hNode);
            InsertPieceTexture(pStaticMesh->m_mapMeshSOFTALPHA02, pAssetDlg, hNode);
        }

        DxFrameAniMan* pFrameAniMan = pStaticAniFrame->GetFrameAniMan();

        if (pFrameAniMan != 0)
        {
            DxFrameAni* pFrameAni = pFrameAniMan->GetFrameAni();

            if (pFrameAni != 0)
            {
                size_t unSize = pFrameAni->GetSizeFrameAniVBTEX();

                for (size_t i = 0; i < unSize; ++i)
                {
                    DxFrameAniVBTEX* pVBTEX = pFrameAni->GetFrameAniVBTEX(i);

                    if (pVBTEX != 0)
                    {
                        pAssetDlg->Insert(hNode, pVBTEX->GetTextureRes().GetFileName().c_str(), false);
                    }
                }                                
            }
        }
    }
}

void CEditState::InsertPiece(DxPieceManagerPSF* pPieceMng, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;
    DWORD dwSize = pPieceMng->GetPointPieceSize();

    if (hNode == 0)
    {
        hNode = pAssetDlg->Insert(hNode, g_strEditSheet[PIECEPOINTPAGEINDEX], false);
    }

    for (DWORD i = 0; i < dwSize; ++i)
    {
        pPieceInfo = pPieceMng->GetMATRIXPISNAME(i);	

        if (pPieceInfo != 0)
        {
            InsertPiece(pPieceInfo, pAssetDlg, hNode);
        }
    }
}

void CEditState::InsertReplacePiece(DxPieceManagerPSF* pPieceMng, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;
    DWORD dwSize = pPieceMng->GetChangePieceSize();

    if (hNode == 0)
    {
        hNode = pAssetDlg->Insert(hNode, g_strEditSheet[PIECEMANAGERINDEX], false);
    }

    for (DWORD i = 0; i < dwSize; ++i)
    {
        pPieceInfo = pPieceMng->GetChangeMATRIXPISNAME(i);

        if (pPieceInfo != 0)
        {
            InsertPiece(pPieceInfo, pAssetDlg, hNode);
        }
    }
}

void CEditState::InsertLight(DxLightMan* pLightMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[LIGHTPAGEINDEX], false, 0);

    pAssetDlg->Insert(hChild, _T("DIRECTIONAL-[NOON]"), false, 0);
    pAssetDlg->Insert(hChild, _T("DIRECTIONAL-[NIGHT]"), false, 0);

    LIST_DXLIGHT kList = pLightMan->GetLIST_DXLIGHT();

    if (kList.empty() == false)
    {
        BOOST_FOREACH(DXLIGHT* pLight, kList)
        {
            pAssetDlg->Insert(hChild, pLight->m_szLightName, false, 0);
        }
    }
}

void CEditState::InserWeather(DxWeatherMan* pWeatherMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{   
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[WEATHERPAGEINDEX], false, 0);
}

void CEditState::InserCamera(DxCamAniMan* pCameraMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[CAMERAPAGEINDEX], false, 0);

    DxCameraAni* pCamera = pCameraMan->GetCameraList();

    while (pCamera)
    {
        pAssetDlg->Insert(hChild, pCamera->m_szName, false, 0);
        pCamera = pCamera->m_pNext;
    }
}

void CEditState::InserEffect(DXLANDEFF* pEffectList, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[SINGLEEFFPAGEINDEX], false, 0);  

    while (pEffectList)
    {
        pAssetDlg->Insert(hChild, pEffectList->GetEffectName(), false, 0);
        pEffectList = pEffectList->m_pNext;
    }
}

void CEditState::InserSound(CStaticSoundMan* pSoundMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[SOUNDPAGEINDEX], false, 0);

    SVecSound* pSound = pSoundMan->GetVecSoundHead();

    while (pSound)
    {
        pAssetDlg->Insert(hChild, pSound->m_szSoundName, false, 0);
        pSound = pSound->m_pNext;
    }
}

void CEditState::InserSkinObj(DXLANDSKINOBJ* pSkinObj, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[SKINOBJPAGEINDEX], false, 0);

    while (pSkinObj)
    {
        pAssetDlg->Insert(hChild, pSkinObj->m_szName, false, 0);
        pSkinObj = pSkinObj->m_pNext;
    }
}

void CEditState::InserLandGate(DxLandGateMan* pLandGateMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[LANDGATEPAGEINDEX], false, 0);

    PDXLANDGATE pLandGate = pLandGateMan->GetListLandGate();

    while (pLandGate)
    {
        pAssetDlg->Insert(hChild, pLandGate->m_szName, false, 0);
        pLandGate = pLandGate->m_pNext;
    }
}

void CEditState::InserBGM(SBGMDATA* pBGMMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[BGMEDITPAGEINDEX], false, 0);
    pAssetDlg->Insert(hChild, pBGMMan->strFileName.GetBuffer(), false, 0);
}

void CEditState::InserFog(FOG_PROPERTY* pFogMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[FOGPAGEINDEX], false, 0);
}

void CEditState::InserSkyBox(SKY_PROPERTY* pSkyBoxMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[SKYBOXMANINDEX], false, 0);
}

void CEditState::InserCollision(CCollisionMap* pCollMan, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[CAMCOLLPAGEINDEX], false, 0);
}

void CEditState::InserWater(WaterSurf* pWaterSurf, CAssetDlg* pAssetDlg, HTREEITEM hNode)
{    
    HTREEITEM hChild = pAssetDlg->Insert(hNode, g_strEditSheet[WATERSURFPAGEINDEX], false, 0);

    BOOST_FOREACH(WaterSurfPointMan* pSurf, pWaterSurf->GetWaterPointMan())
    {
        pAssetDlg->Insert(hChild, _T("New"), false, 0);
    }
}

void CEditState::InsertPiece(DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo, CAssetDlg* pAssetDlg, HTREEITEM hParent)
{
    HTREEITEM hChild = pAssetDlg->Insert(hParent, pPieceInfo->m_pPisName, false, reinterpret_cast<DWORD>(pPieceInfo->m_pPiece));

    if (pPieceInfo->m_pPiece)
    {
        InsertPieceTexture(pPieceInfo->m_pPiece->GetStaticAniFrame(), pAssetDlg, hChild);
    }
}