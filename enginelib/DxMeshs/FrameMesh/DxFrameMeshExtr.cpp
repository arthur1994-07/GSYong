#include "pch.h"

#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxMeshs/FrameMesh/DxAnimationManager.h"
#include "../../DxLand/LightMap/LightMapObject.h"
#include "../../DxTools/DxMethods.h"
#include "DxReplaceContainer.h"
#include "DxFrameMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

char D3DEXMATERIAL::szMtrlTypes[MATERIAL_NSIZE][24] = 
{
	"NORMAL",
	"ALPHA",
	"ALPHA-NOZWRI",
	"ALPHA-NOTEST",
    "ALPHA-HARD",
	"ALPHA-SOFT"
};

DWORD DxFrame::dwFrameTypeNum = 2;
char DxFrame::szFrameType[2][24] = 
{
	"COMMON",
	"DOOR"
};

DWORD D3DEXMATERIAL_100::VERSION = 0x00000100;
DWORD D3DEXMATERIAL    ::VERSION = 0x00000101;


BOOL DxFrame::IsAlphaMapMesh ()
{
	DxMeshes *pmsMeshCur;
	pmsMeshCur = pmsMeshs;
	while ( pmsMeshCur )
	{
		if ( pmsMeshCur->m_exMaterials )
		{
			for ( DWORD i=0; i<pmsMeshCur->m_cMaterials; i++ )
			{
				if ( pmsMeshCur->m_exMaterials[i].MtrlType!=MATERIAL_NORMAL )		return TRUE;
			}
		}

		pmsMeshCur = pmsMeshCur->m_pMeshNext;
	}

	return FALSE;
}

//	Note : ȿ�� �߰�.
//
void DxFrameMesh::AddEffect ( DxEffectBase *pEffectAdd )
{
	pEffectAdd->pEffectNext = m_pEffectHead;
	m_pEffectHead = pEffectAdd;
}

//	Note : ȿ�� ����.
//
BOOL DxFrameMesh::DelEffect ( DxFrame *pDxFrame, DxEffectBase *pEffectDel )
{
	DxEffectBase *pEffectCur, *pEffectPrev;

	//	Note : �������� ����Ʈ ����Ʈ���� ����.
	//

	//	Note : ��, ����Ʈ
	//
	if ( pDxFrame->pEffectPrev == pEffectDel )
	{
		pDxFrame->pEffectPrev =  pEffectDel->pLocalFrameEffNext;
	}
	else
	{
		pEffectCur = pDxFrame->pEffectPrev;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pLocalFrameEffNext = pEffectDel->pLocalFrameEffNext;
				break;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}

	//	Note : ��, ����Ʈ
	//
	if ( pDxFrame->pEffect == pEffectDel )
	{
		pDxFrame->pEffect =  pEffectDel->pLocalFrameEffNext;
	}
	else
	{
		pEffectCur = pDxFrame->pEffect;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pLocalFrameEffNext = pEffectDel->pLocalFrameEffNext;
				break;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}

	//	Note : ��, ����Ʈ
	//
	if ( pDxFrame->pEffectNext == pEffectDel )
	{
		pDxFrame->pEffectNext =  pEffectDel->pLocalFrameEffNext;
	}
	else
	{
		pEffectCur = pDxFrame->pEffectNext;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pLocalFrameEffNext = pEffectDel->pLocalFrameEffNext;
				break;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}

	//	Note : ��ü�� ����Ʈ ����Ʈ���� ����.
	//
	if ( pEffectDel == m_pEffectHead )
	{
		m_pEffectHead = pEffectDel->pEffectNext;
		pEffectDel->pEffectNext = NULL;
		
		pEffectDel->CleanUp();
		delete pEffectDel;
	}
	else
	{
		pEffectCur = m_pEffectHead;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pEffectNext = pEffectCur->pEffectNext;
				pEffectCur->pEffectNext = NULL;

				pEffectDel->CleanUp();
				delete pEffectDel;

				return TRUE;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pEffectNext;
		}
	}

	//	Note : ���� ȿ���� ���ŵǸ鼭 Replace ������ ���ŵǸ� 
	//		���ĸ��� �����ϴ� �������� ��� ���ĸ� ����Ʈ�� ����.
	//
	if ( !pDxFrame->IsReplace () && pDxFrame->IsAlphaMapMesh () )
	{
		AddAlphaMapFrame ( pDxFrame );
	}

	return FALSE;
}

void DxFrameMesh::AddAlphaMapFrameAll ( DxFrame *pDxFrame )
{
	if ( pDxFrame->IsAlphaMapMesh () )		AddAlphaMapFrame(pDxFrame);

	DxFrame *pFrameChildCur = pDxFrame->pframeFirstChild;
	for ( ; pFrameChildCur; pFrameChildCur=pFrameChildCur->pframeSibling )
	{
		AddAlphaMapFrameAll ( pFrameChildCur );
	}
}

void DxFrameMesh::DelAlphaMapFrameAll ( DxFrame *pDxFrame )
{
	if ( pDxFrame->IsAlphaMapMesh () )		DelAlphaMapFrame(pDxFrame);

	DxFrame *pFrameChildCur = pDxFrame->pframeFirstChild;
	for ( ; pFrameChildCur; pFrameChildCur=pFrameChildCur->pframeSibling )
	{
		DelAlphaMapFrameAll ( pFrameChildCur );
	}
}

void DxFrameMesh::AddAlphaMapFrame ( DxFrame *pDxFrameAdd )
{
	GASSERT(pDxFrameAdd);

	//	Note :��� ����Ʈ�� �˻��� ������ ���� / ������ ���.
	//
	DxFrame* pDxFrameCur = m_pDxAlphaMapFrameHead;
	while ( pDxFrameCur )
	{
		if ( pDxFrameCur == pDxFrameAdd )	return;
		pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
	}

	pDxFrameAdd->pNextAlphaMapFrm = m_pDxAlphaMapFrameHead;
	m_pDxAlphaMapFrameHead = pDxFrameAdd;
}

void DxFrameMesh::DelAlphaMapFrame ( DxFrame *pDxFrameDel )
{
	GASSERT(pDxFrameDel);

	if ( m_pDxAlphaMapFrameHead == pDxFrameDel )
	{
		m_pDxAlphaMapFrameHead =  pDxFrameDel->pNextAlphaMapFrm;
	}
	else
	{
		DxFrame* pDxFrameCur = m_pDxAlphaMapFrameHead, *pDxFramePrev;
		while ( pDxFrameCur )
		{
			if ( pDxFrameCur == pDxFrameDel )
			{
				pDxFramePrev->pNextAlphaMapFrm = pDxFrameCur->pNextAlphaMapFrm;
				return;
			}

			pDxFramePrev = pDxFrameCur;
			pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
		}
	}
}

BOOL DxFrameMesh::AddPieceObj ( char *szFrameName, const char *szPieceName )
{
	GASSERT(szFrameName);
	GASSERT(szPieceName);

	//	Note : Frame ã��.
	//
	DxFrame *pDxFrame = FindFrame ( szFrameName );
	if ( !pDxFrame )	return FALSE;

	//	Note : ���� ġȯ ���� �����Ǿ� ������.
	//
	if ( pDxFrame->pRePlacePiece )
		DelPieceObj ( szFrameName );

	// Note : �̸��� ������ ���	
	if (!pDxFrame->szName)
        return FALSE;

	//	Note : ġȯ ���� ��������.
	DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szPieceName );
	if (!pPiece)
        return FALSE;

	DXPIECEOBJ *pPieceObj = new DXPIECEOBJ;
	pPieceObj->SetPiece ( pPiece );


	int nStrLen = static_cast<int>( strlen(pDxFrame->szName)+1 );
	pPieceObj->szFrameName = new char[nStrLen];
	StringCchCopy( pPieceObj->szFrameName, nStrLen, pDxFrame->szName );

	//	Note : �������� ��Ʈ������ ������Ʈ.
	//
	pPieceObj->matComb = pDxFrame->matCombined;

	if ( pPiece->bRendAni )
	{
	//	srand(GetTickCount());
		pPieceObj->fCurTime = (rand()%1000) * 0.01f;
	}

	//	Note : �����ӿ� ġȯ ������ ���.
	//
	pDxFrame->pRePlacePiece = pPieceObj;

	//	Note : ġȯ ���� ��Ͻ� ���ĸ� ������ ����Ʈ���� ���� �ؾ���.
	//
	DelAlphaMapFrameAll ( pDxFrame );

	//	Note : ����Ʈ�� ���.
	//
	pPieceObj->pNext = m_pPieceObjHead;
	m_pPieceObjHead = pPieceObj;

	return TRUE;
}

void DxFrameMesh::DelPieceObj ( char *szFrameName )
{
	GASSERT(szFrameName);
	if ( !m_pPieceObjHead )		return;

	DXPIECEOBJ* pPieceObjCur = m_pPieceObjHead, *pPieceObjPrev;

	//	Note : Frame ã��.
	//
	DxFrame *pDxFrame = FindFrame ( szFrameName );
	if ( !pDxFrame )	return;

	//	Note : ġȯ ���� ���Ž� ���ĸ� ������ ����Ʈ�� �߰� �ؾ���.
	//
	AddAlphaMapFrameAll ( pDxFrame );

	if ( m_pPieceObjHead == pDxFrame->pRePlacePiece )
	{
		m_pPieceObjHead = pPieceObjCur->pNext;

		pDxFrame->pRePlacePiece = NULL;
		pPieceObjCur->pNext = NULL;
		SAFE_DELETE(pPieceObjCur);
	}
	else
	{
		while ( pPieceObjCur )
		{
			if ( pPieceObjCur == pDxFrame->pRePlacePiece )
			{
				pPieceObjPrev->pNext = pPieceObjCur->pNext;

				pDxFrame->pRePlacePiece = NULL;
				pPieceObjCur->pNext = NULL;
				SAFE_DELETE(pPieceObjCur);
				break;
			}

			pPieceObjPrev = pPieceObjCur;
			pPieceObjCur = pPieceObjCur->pNext;
		}
	}
}

void DxFrameMesh::MakePieceAABBTree ()
{
	DxReplaceContainer::MakeAABBTree ( m_pPieceObjTree, m_pPieceObjHead );
}

void DxFrameMesh::CleanPieceAABBTree ()
{
	SAFE_DELETE(m_pPieceObjTree);
}

void DxFrameMesh::MakeAnimationList ()
{
	DxAnimationMan *pAnimManCur = pAnimManHead;
	while ( pAnimManCur )
	{
		pAnimManCur->SetDxFrameRoot(NULL);
		pAnimManCur->SetAnimationHead(NULL);

		pAnimManCur = pAnimManCur->m_pNext;
	}

	SAFE_DELETE(pAnimManHead);


	MakeAnimationList ( m_pDxFrame );
}

void DxFrameMesh::MakeAnimationList ( DxFrame * pDxFrame )
{
	DxFrame		*pframeCur = pDxFrame->pframeFirstChild;
	while ( pframeCur )
	{
		if ( pframeCur->pframeFromAnimate )
		{
			DxAnimationMan* pNewAniMan = new DxAnimationMan;
			pNewAniMan->SetDxFrameRoot ( pframeCur );

			//	Note : ����Ʈ�� ����.
			//
			pNewAniMan->m_pNext = pAnimManHead;
			pAnimManHead = pNewAniMan;
		}
		else
		{
			MakeAnimationList ( pframeCur );
		}

		pframeCur = pframeCur->pframeSibling;
	}
}

DxAnimationMan* DxFrameMesh::FindAniRootFrame ( DxFrame *pDxFrame )
{
	DxAnimationMan *pAniManCur = pAnimManHead;
	while ( pAniManCur )
	{
		if ( pAniManCur->GetDxFrameRoot() == pDxFrame )
			return pAniManCur;

		pAniManCur = pAniManCur->m_pNext;
	}

	return NULL;
}

void	DxFrame::DxFrameNodeCount ( DWORD &nNodeCount )
{	
	nNodeCount++;

	DxFrame		*pframeChild = pframeFirstChild;
	while ( pframeChild )
	{
		pframeChild->DxFrameNodeCount ( nNodeCount );
		pframeChild = pframeChild->pframeSibling;
	}
}

//	Note : ȿ�� ��ü�� ġȯ ���� ���� ��ü���� ���� ��� ��� �ؼ�
//		��Ʈ���� ���ҿ��� ����ϰ� ����.
//
BOOL DxFrame::IsReplace ()
{
	if ( bFlag&DXFRM_NAVIFRAME )				return TRUE;

	DxEffectBase* pEffCur = pEffect;
	while ( pEffCur )
	{
		if ( pEffCur->GetFlag()&(_EFF_REPLACE|_EFF_REPLACE_AFTER|_EFF_REPLACE_AFTER_1) )	return TRUE;
		pEffCur = pEffCur->pLocalFrameEffNext;
	}

	return pRePlacePiece!=NULL;
}	

float DxFrame::GetLightMapDetail()
{
	if ( pmsMeshs )
	{
        return pmsMeshs->m_fLightMapDetail;
	}
    else
    {
        return 1.f;
    }
}

LightMapObject_Data DxFrame::GetLightMapData()
{
    if ( pmsMeshs )
	{
        return pmsMeshs->GetLightMapData();
	}

    return LightMapObject_Data();
}

LightMapObject_Data DxMeshes::GetLightMapData()
{
    if ( m_vecLightMapObject.empty() )
        return LightMapObject_Data();

    LightMapObject_Data sData;
    m_vecLightMapObject[0]->Export( sData );

    return sData;
}

HRESULT DxFrameMesh::ExportLightMapUV( MAP_LIGHTMAPUV& mapLightMapUV, DxFrame *pframeCur )
{
	HRESULT hr(S_OK);

	if ( !pframeCur )
		return S_OK;

	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )
		return S_OK;

	//	Note : �޽� FVF ����.
	//
	DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
	while( pmsMeshs )
	{			
		hr = pmsMeshs->ExportLightMapUV( mapLightMapUV, m_scpMaterialSetting.get() );
		if (FAILED(hr))
			return hr;

		pmsMeshs = pmsMeshs->m_pMeshNext;
	}

	//	Note : �ڽ� ���� ���.
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
	while (pframeChild!=NULL)
	{
		hr = ExportLightMapUV ( mapLightMapUV, pframeChild );
		if ( FAILED(hr) )
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

void DxFrameMesh::CheckLightMapUV( DxFrame *pframeCur )
{
	if ( !pframeCur )
		return;

	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )
		return;

	//	Note : �޽� FVF ����.
	//
	DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
	while( pmsMeshs != NULL )
	{
		DWORD dwLightMapIndex = pmsMeshs->m_dwLightMapIndex;
		if ( dwLightMapIndex>=0 && dwLightMapIndex<=3 )
		{
			D3DXVECTOR2 vMin( FLT_MAX, FLT_MAX );
			D3DXVECTOR2 vMax( -FLT_MAX, -FLT_MAX );
			VERTEX_WORLD* pVerticesLMUV;
			WORD* pIndies;
			DWORD dwVerticesNUM = pmsMeshs->m_pLocalMesh->GetNumVertices();
			DWORD dwFacesNUM = pmsMeshs->m_pLocalMesh->GetNumFaces();
			pmsMeshs->m_pLocalMesh->LockVertexBuffer( 0L, (VOID**)&pVerticesLMUV );
			pmsMeshs->m_pLocalMesh->LockIndexBuffer( 0L, (VOID**)&pIndies );

			for ( DWORD x=0; x<dwVerticesNUM; ++x )
			{
				if ( vMin.x > pVerticesLMUV[x].vTex[ dwLightMapIndex ].x )
					vMin.x = pVerticesLMUV[x].vTex[ dwLightMapIndex ].x;
				if ( vMin.y > pVerticesLMUV[x].vTex[ dwLightMapIndex ].y )
					vMin.y = pVerticesLMUV[x].vTex[ dwLightMapIndex ].y;
				if ( vMax.x < pVerticesLMUV[x].vTex[ dwLightMapIndex ].x )
					vMax.x = pVerticesLMUV[x].vTex[ dwLightMapIndex ].x;
				if ( vMax.y < pVerticesLMUV[x].vTex[ dwLightMapIndex ].y )
					vMax.y = pVerticesLMUV[x].vTex[ dwLightMapIndex ].y;
			}

			if ( vMin.x < 0.f || vMin.y < 0.f || vMax.x > 1.f || vMax.y > 1.f )
			{
				TSTRING strText;
				strText = pframeCur->szName;
				strText += _T(" - UV ��ġ�� 0~1 ���̸� ������ϴ�.");
				AfxMessageBox( strText.c_str() );
			}

			//D3DXVECTOR2 vTex[3];
			//for ( DWORD x=0; x<dwFacesNUM; ++x )
			//{
			//	vTex[0] = pVerticesLMUV[ pIndies[x*3+0] ].vTex;
			//	vTex[1] = pVerticesLMUV[ pIndies[x*3+1] ].vTex;
			//	vTex[2] = pVerticesLMUV[ pIndies[x*3+2] ].vTex;

			//	if ( vTex[0] == vTex[1] || vTex[0] == vTex[2] || vTex[1] == vTex[2] )
			//	{
			//		TSTRINGSTREAM strStringStream;
			//		strStringStream << pframeCur->szName;
			//		strStringStream << _T("���� �ﰢ���� UV�� �����ϴ�. [");
			//		strStringStream << x;
			//		strStringStream << _T("]");
			//		AfxMessageBox( strStringStream.str().c_str() );
			//	}
			//}
			pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
			pmsMeshs->m_pLocalMesh->UnlockIndexBuffer();
		}

		pmsMeshs = pmsMeshs->m_pMeshNext;
	}

	//	Note : �ڽ� ���� ���.
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
	while (pframeChild!=NULL)
	{
		CheckLightMapUV ( pframeChild );
		pframeChild = pframeChild->pframeSibling;
	}
}

BOOL IsSameFace( const WORD awIndicesSRC[], const WORD awIndicesLMUV[], const VERTEX* pVerticesSRC, const VERTEX* pVerticesLMUV, VEC_DWORD& vecIndex )
{
	vecIndex.clear();
	for ( DWORD x=0; x<3; ++x )
	{
		for ( DWORD y=0; y<3; ++y )
		{
			if ( NSMATH::IsOffsetSamePos3( pVerticesSRC[awIndicesSRC[x]].vPos, pVerticesLMUV[awIndicesLMUV[y]].vPos, 0.01f ) &&
				NSMATH::IsOffsetSamePos3( pVerticesSRC[awIndicesSRC[x]].vNor, pVerticesLMUV[awIndicesLMUV[y]].vNor, 0.01f ) )
			{
				vecIndex.push_back( y );
				break;
			}
		}
	}

	if ( vecIndex.size() == 3 )
	{
		if ( vecIndex[0] == vecIndex[1] )
			return FALSE;
		if ( vecIndex[0] == vecIndex[2] )
			return FALSE;
		if ( vecIndex[1] == vecIndex[2] )
			return FALSE;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HRESULT DxMeshes::ExportLightMapUV( MAP_LIGHTMAPUV& mapLightMapUV, CMaterialSetting* pMaterialSetting )
{
	DWORD awIndicesLMUV[3];
	VERTEX_WORLD* pVerticesLMUV;
	std::tr1::shared_ptr<LIGHTMAPUV> spLightMapUV( new LIGHTMAPUV );
	spLightMapUV->m_wwTextureSize = m_wwLightMapUserUV_TextureSize;

	VEC_DWORD vecIndex;
	DWORD dwFaceSRC = m_pLocalMesh->GetNumFaces();

	m_pLocalMesh->LockVertexBuffer( 0L, (VOID**)&pVerticesLMUV );

	if ( !m_bEnableLightMap || m_dwLightMapIndex < 0 || m_dwLightMapIndex > 3 )
	{
		// spLightMapUV�� ������ �ʿ����.
		spLightMapUV->m_wwTextureSize = WORDWORD(0,0);

		//for ( DWORD x=0; x<dwFaceSRC; ++x )
		//{
		//	for ( DWORD i=0; i<3; ++i )
		//	{
		//		spLightMapUV->m_vecUV.push_back( D3DXVECTOR2(0.f,0.f) );
		//	}
		//}
	}
	else
	{
		// MaterialSetting�� ����Ʈ�� ������ �ִ��� Ȯ������.
		int nTYPE_LIGHTMAP_COUNT(0);
		for ( DWORD i=0; i<m_vecMaterialName.size(); ++i )
		{
			if ( pMaterialSetting )
			{
				const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( m_vecMaterialName[i].c_str() );
				if ( pSetting )
				{
					if ( pSetting->m_emUnLightMapType == NSMaterialSetting::RENDER_TYPE_LIGHTMAP )
					{
						++nTYPE_LIGHTMAP_COUNT;
						break;
					}
				}
			}
		}

		if ( nTYPE_LIGHTMAP_COUNT == 0 )
		{
			// spLightMapUV�� ������ �ʿ����.
			spLightMapUV->m_wwTextureSize = WORDWORD(0,0);
		}
		else
		{
			if ( m_pLocalMesh->GetOptions() & D3DXMESH_32BIT )
			{
				DWORD* pIndicesLMUV;
				m_pLocalMesh->LockIndexBuffer( 0L, (VOID**)&pIndicesLMUV );
				for ( DWORD x=0; x<dwFaceSRC; ++x )
				{
					awIndicesLMUV[0] = pIndicesLMUV[x*3+0];
					awIndicesLMUV[1] = pIndicesLMUV[x*3+1];
					awIndicesLMUV[2] = pIndicesLMUV[x*3+2];

					for ( DWORD i=0; i<3; ++i )
					{
						spLightMapUV->m_vecUV.push_back( pVerticesLMUV[ awIndicesLMUV[i] ].vTex[ m_dwLightMapIndex ] );
					}
				}
				m_pLocalMesh->UnlockIndexBuffer();
			}
			else
			{
				WORD* pIndicesLMUV;
				m_pLocalMesh->LockIndexBuffer( 0L, (VOID**)&pIndicesLMUV );
				for ( DWORD x=0; x<dwFaceSRC; ++x )
				{
					awIndicesLMUV[0] = pIndicesLMUV[x*3+0];
					awIndicesLMUV[1] = pIndicesLMUV[x*3+1];
					awIndicesLMUV[2] = pIndicesLMUV[x*3+2];

					for ( DWORD i=0; i<3; ++i )
					{
						spLightMapUV->m_vecUV.push_back( pVerticesLMUV[ awIndicesLMUV[i] ].vTex[ m_dwLightMapIndex ] );
					}
				}
				m_pLocalMesh->UnlockIndexBuffer();
			}
		}
	}
	
	m_pLocalMesh->UnlockVertexBuffer();

	mapLightMapUV.insert( MAP_LIGHTMAPUV_VTYPE( m_szName, spLightMapUV ) );

	return S_OK;
}