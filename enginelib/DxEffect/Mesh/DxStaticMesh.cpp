#include "pch.h"

#include <algorithm>
#include "../../Common/StlFunctions.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/EBTime.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/Collision.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxLand/LightMap/LightMapObjectVEC.h"
#include "../../DxLand/LightMap/LightMapObject.h"
#include "../../DxLand/LightMap/NSLightMapTools.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxPiece/NSOctreeRenderList.h"
#include "../../DxLand/DxPiece/DxPieceManagerPSF.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../DxLand/DxPiece/DxPieceEdit.h"
#include "../../DxLand/DxLandMan.h"
#include "../../DxLand/DxLandDef.h"
#include "../TexEff/DxTextureEffNewFile.h"
#include "../TextureSetDXT.h"

#include "./DxStaticMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMesh::DxStaticMesh()
    : m_emRenderType(EMRT_OCTREE)
    , m_bLightMap(FALSE)
    , m_strLightMap(_T(""))
	, m_pList(NULL),
	m_pAlphaList(NULL),
	m_pAlphaSoftList(NULL),
	m_pAlphaSoftList01(NULL),
	m_pAlphaSoftList02(NULL),
	m_pMeshTree(NULL),
	m_pMeshAlphaTree(NULL),
	m_pMeshSoftAlphaTree(NULL),
	m_pMeshSoftAlphaTree01(NULL),
	m_pMeshSoftAlphaTree02(NULL),
	m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
{
}

DxStaticMesh::~DxStaticMesh()
{
	CleanUp();

	//// ������ �ݴ´�.
	//m_SFile.CloseFile();
}

void DxStaticMesh::StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//InitializeCriticalSection(&m_CSLockLoading);
	//m_sThread.StartThread( pd3dDevice, &m_SFile, &m_listLoadingData, &m_CSLockLoading );
}

void DxStaticMesh::EndThread()
{
	//DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	���� List�� �����.

	//m_sThread.EndThread();
	//DeleteCriticalSection(&m_CSLockLoading);
}

void DxStaticMesh::CleanUp()
{
	m_vMin = D3DXVECTOR3 (FLT_MAX,FLT_MAX,FLT_MAX);
	m_vMax = D3DXVECTOR3 (-FLT_MAX,-FLT_MAX,-FLT_MAX);

	m_pList = NULL;
	m_pAlphaList = NULL;
	m_pAlphaSoftList = NULL;
	m_pAlphaSoftList01 = NULL;
	m_pAlphaSoftList02 = NULL;

	SAFE_DELETE ( m_pMeshTree );
	SAFE_DELETE ( m_pMeshAlphaTree );
	SAFE_DELETE ( m_pMeshSoftAlphaTree );
	SAFE_DELETE ( m_pMeshSoftAlphaTree01 );
	SAFE_DELETE ( m_pMeshSoftAlphaTree02 );

	std::for_each ( m_mapMesh.begin(), m_mapMesh.end(), std_afunc::DeleteMapObject() );
	m_mapMesh.clear();

	std::for_each ( m_mapMeshALPHA.begin(), m_mapMeshALPHA.end(), std_afunc::DeleteMapObject() );
	m_mapMeshALPHA.clear();

	std::for_each ( m_mapMeshSOFTALPHA.begin(), m_mapMeshSOFTALPHA.end(), std_afunc::DeleteMapObject() );
	m_mapMeshSOFTALPHA.clear();

	std::for_each ( m_mapMeshSOFTALPHA01.begin(), m_mapMeshSOFTALPHA01.end(), std_afunc::DeleteMapObject() );
	m_mapMeshSOFTALPHA01.clear();

	std::for_each ( m_mapMeshSOFTALPHA02.begin(), m_mapMeshSOFTALPHA02.end(), std_afunc::DeleteMapObject() );
	m_mapMeshSOFTALPHA02.clear();
}

void DxStaticMesh::MakeAABBOCTree( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                  DxFrameMesh* const pFrameMesh, 
                                  DxPieceManagerPSF* pPieceManager,
								  LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
                                  const BOOL bProgress, 
                                  BOOL bPiece, 
                                  BOOL bLightMap, 
                                  const TSTRING& strLightMapName,
								  const DxTextureEffMan* pTextureEffMan,
								  const CMaterialSetting* pMaterialSetting,
								  BOOL bRan_1_0,
								  float fCellSize )
{
	CleanUp();

    m_bLightMap = bLightMap;
    m_strLightMap = strLightMapName;
	if ( pLightMapUV_MAIN )
	{
		m_wwLightMapUserUV_TEXEL.wA = static_cast<WORD>( pLightMapUV_MAIN->m_dwTextureWidth );
		m_wwLightMapUserUV_TEXEL.wB = static_cast<WORD>( pLightMapUV_MAIN->m_dwTextureHeight );
	}

    // LightMap�� �����ؾ� �Ѵٸ�.. �̸� �浹ó���� Mesh�� ������ �Ѵ�.
    DxStaticMesh* pStaticMesh(NULL);
    {
        if ( bLightMap )
        {
            pStaticMesh = new DxStaticMesh;

            // �� �ڽŵ� Coll_Texture�� �����.
            CreateAlphaTexture32Bit( pd3dDevice );

            if ( pStaticMesh )
            {
				pStaticMesh->MakeAABBOCTree( pd3dDevice, pFrameMesh, NULL, NULL, bProgress, bPiece, FALSE, strLightMapName, pTextureEffMan, pMaterialSetting, bRan_1_0, fCellSize );
                pStaticMesh->CreateAlphaTexture32Bit( pd3dDevice );     // StaticMesh�� ���� Coll_Texture�� �����.
            }
            if ( pPieceManager )
            {
                pPieceManager->CreateAlphaTexture32Bit( pd3dDevice );   // Piece �� Coll_Texture�� �����.
            }
        }
    }

    // StaticMesh�� ���ϱ� ���� ���� �۾��� �Ѵ�.
    {
	    if ( pFrameMesh )
	    {
			float fLightMapDetail = 1.f;
		    // SingleTexMeh�� �����.
		    NSSTATICMESH::ExportMaterialList( pd3dDevice, 
											pFrameMesh, 
											this, 
											pLightMapUV_MAIN, 
											bProgress, 
											bPiece, 
											bLightMap, 
											pStaticMesh, 
											pPieceManager, 
											strLightMapName, 
											fLightMapDetail,
											pTextureEffMan,
											pMaterialSetting,
											bRan_1_0,
											fCellSize );

		    // TexEff�� �´� �������� �ٲٱ� ����.
		    NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMesh );
		    NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshALPHA );
		    NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshSOFTALPHA );
		    NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshSOFTALPHA01 );
		    NSSTATICMESH::ConvertTexEffMesh( pd3dDevice, pFrameMesh->GetTextureEffMan(), m_mapMeshSOFTALPHA02 );

		    // Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
		    NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
		    NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
		    NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
		    NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
		    NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

		    // SingleMehs Tree�� �����Ѵ�.
		    COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
		    COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
		    COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
		    COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
		    COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

		    // �ִ밪�� �ּҰ��� ���Ѵ�.
		    SetAABBMaxMin ();
	    }
    }

    // ������ ����
    SAFE_DELETE( pStaticMesh );
}

void DxStaticMesh::MakeAABBOCTree( LPDIRECT3DDEVICEQ pd3dDevice, 
								  DxMeshes *pmsMeshs,
								  const CMaterialSetting *pMaterialSetting,
								  float fCellSize )
{
	CleanUp();

	if ( pmsMeshs )
	{
		float fLightMapDetail = 1.f;
		// SingleTexMeh�� �����.
		NSSTATICMESH::ExportMaterialList( pd3dDevice, 
										pmsMeshs, 
										this,
										pMaterialSetting,
										fCellSize );

		// Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
		NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

		// SingleMehs Tree�� �����Ѵ�.
		COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
		COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
		COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
		COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
		COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

		// �ִ밪�� �ּҰ��� ���Ѵ�.
		SetAABBMaxMin ();
	}
}
//
//void DxStaticMesh::MakeLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
//                                     const DxPieceEdit* pPieceEdit,
//                                     const TSTRING& strLightMapName, 
//                                     DxStaticMesh* pStaticMesh, 
//                                     DxPieceManager* pPieceManager, 
//                                     const D3DXMATRIX& matWorld, 
//                                     VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
//                                     DWORD& dwStartTextureSize,
//                                     WORDWORD& wwBaseTexelXY,
//                                     float& fLightMapDetail )
//{
//    DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };
//
//    int nCount(1);
//    int nSingleTexMeshSize(0);
//
//    for ( DWORD i=0; i<5; ++i )
//    {
//        DxSingleTexMesh* pCur = pListArray[i];
//        while( pCur )
//        {
//            ++nSingleTexMeshSize;
//	        pCur = pCur->m_pNext;
//        }
//    }
//
//    // �ٽ� ������ �� ��찡 �־ �ϴ� ����� ���´�.
//	NSLightMap::BackUp_Piece();
//
//    const DxTextureEffMan* pTextureEffMan(NULL);
//    if ( pPieceEdit )
//    {
//        pTextureEffMan = pPieceEdit->GetTextureEffMan();
//    }
//
//	BOOL bNewTexture(FALSE);
//	int nPrevTexID(-1);
//    int nMutiply(2);
//	DWORD dwBaseTextureSize(dwStartTextureSize);
//    for ( DWORD i=0; i<5; ++i )
//    {
//        DxSingleTexMesh* pCur = pListArray[i];
//        while( pCur )
//        {
//            // LightMap�� ��������, ������ �ƴ� �׸������� ������ ������� �ʵ��� �Ѵ�.
//            BOOL bDontMakeShadowLM = FALSE;
//            if ( pTextureEffMan )
//            {
//                const TextureData* pTextureData = pTextureEffMan->GetTextureData( pCur->GetTextureName() );
//                if ( pTextureData )
//                {
//					if ( pTextureData->m_dwPieceLM_Option & EMPLO_DONT_LM_SHADOW_RECEIVE )
//					{
//						bDontMakeShadowLM = TRUE;
//					}
//					else
//					{
//						bDontMakeShadowLM = FALSE;
//					}
//                }
//            }
//
//            std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ> spNew( std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ>(new SVEC_LIGHTMAPOBJ) );
//
//			// ���⼭ ���ʹ� fLightMapDetail �� ������ �ѱ��� �ʴ´�.
//            pCur->MakeLightMapPiece
//            ( 
//                pd3dDevice, 
//                strLightMapName, 
//                pStaticMesh, 
//                pPieceManager, 
//                matWorld, 
//                spNew.get(), 
//                dwStartTextureSize, 
//                fLightMapDetail,
//                bDontMakeShadowLM
//            );
//
//			// ����TexID�� ��´�.
//			if ( nPrevTexID == -1 )
//			{
//				if ( !spNew->m_vecLightMapObject.empty() )
//				{
//					nPrevTexID = spNew->m_vecLightMapObject[0]->GetTextureID();
//				}
//			}
//
//            // �ϴ� ���Ἲ üũ
//            if ( !spNew->m_vecLightMapObject.empty() && !vecsvecLightMapObj.empty() && !vecsvecLightMapObj[0]->m_vecLightMapObject.empty() )
//            {
//                // �ؽ��İ� �ٸ����� �׷�����.. �׸��� ������ �����Ǿ���ȴ�.
//                if ( spNew->m_vecLightMapObject[0]->GetTextureID() != nPrevTexID )
//                {
//					if ( bNewTexture )
//					{
//						dwStartTextureSize = dwBaseTextureSize*nMutiply;
//						nMutiply *= 2;
//
//						if ( dwStartTextureSize > 2048 )
//						{
//							// �ִ�ũ��� ������� ��� fLightMapDetail �� ���̴� ������� ����.
//							fLightMapDetail *= 0.7f;
//						}
//					}
//					bNewTexture = TRUE;
//
//					i = -1;
//					vecsvecLightMapObj.clear();
//
//                    // ���� RollBack �� �ȴ�.
//					NSLightMap::RollBack_Piece();
//
//					// AllLineBlock
//					NSLightMap::AllLineBlock();
//
//					// ���� ID���� �����Ѵ�.
//					nPrevTexID = spNew->m_vecLightMapObject[0]->GetTextureID();
//
//                    break;
//                }
//            }
//
//		    //   // �ϴ� ���Ἲ üũ
//		    //   if ( !spNew->m_vecLightMapObject.empty() && !vecsvecLightMapObj.empty() && !vecsvecLightMapObj[0]->m_vecLightMapObject.empty() )
//		    //   {
//		    //       // �ؽ��İ� �ٸ����� �׷�����.. �׸��� ������ �����Ǿ���ȴ�.
//		    //       if ( spNew->m_vecLightMapObject[0]->GetTextureID() != vecsvecLightMapObj[0]->m_vecLightMapObject[0]->GetTextureID() )
//		    //       {
//						//// Piece�� LightMap�� �ϳ��� �ؽ��Ŀ� ��� ����� �ؾ� �ϴµ�, 
//						//// pListArray �� �Ϻθ� ���� �ؽ��Ŀ� ����� �ؼ� �ٽ� ���ο� �ؽ��Ŀ� ����ϱ� �����̴�.
//
//						//// ����� �´ٴ� ���� ���ο� Piece�� ���ο� Texture�� ����� �Ѵٴ� ���̴�.
//
//						//// ó������ �׳� �⺻�������.. �� �ٽ� �õ��� ���� ����� �ø���.
//						//// �ؽ��� ����� �÷���, �ٽ� �׸��⸦ �����Ѵ�.
//
//						//if ( dwStartTextureSize >= 2048 )
//						//{
//						//	// �ִ�ũ��� ������� ��� fLightMapDetail �� ���̴� ������� ����.
//						//	fLightMapDetail *= 0.7f;
//						//}
//						//else
//						//{
//						//	dwStartTextureSize = dwBaseTextureSize * nMutiply;
//						//	nMutiply *= 2;
//						//}
//
//						//i = -1;
//						//vecsvecLightMapObj.clear();
//
//						//// ���� RollBack �� �ȴ�.
//						//NSLightMap::RollBack_Piece();
//
//		    //           break;
//		    //       }
//		    //   }
//
//            vecsvecLightMapObj.push_back( spNew );
//	        pCur = pCur->m_pNext;
//        }
//    }
//
//    //------- �ּ� Base UV �� ���Ѵ�.
//    wwBaseTexelXY = WORDWORD(4095,4095);
//    for ( DWORD i=0; i<vecsvecLightMapObj.size(); ++i )
//    {
//        for ( DWORD j=0; j<vecsvecLightMapObj[i]->m_vecLightMapObject.size(); ++j )
//        {
//            LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j].get();
//            //LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j];
//            if ( wwBaseTexelXY.wA > pLightMapObject->GetBaseTexelXY().wA )
//                wwBaseTexelXY.wA = pLightMapObject->GetBaseTexelXY().wA;
//            if ( wwBaseTexelXY.wB > pLightMapObject->GetBaseTexelXY().wB )
//                wwBaseTexelXY.wB = pLightMapObject->GetBaseTexelXY().wB;
//        }
//    }
//
//    // ������ ���´�.
//    for ( DWORD i=0; i<vecsvecLightMapObj.size(); ++i )
//    {
//        for ( DWORD j=0; j<vecsvecLightMapObj[i]->m_vecLightMapObject.size(); ++j )
//        {
//            LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j].get();
//            pLightMapObject->SetBaseTexelPieceXY( wwBaseTexelXY );
//        }
//    }
//
//    // �� ���̶� ���ǰ� ������ �� ������ ����� ������ �����Ѵ�.
//    {
//        NSLightMap::OneUseLineBlock();
//    }
//}

void DxStaticMesh::MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										 const TSTRING& strLightMapName, 
										 DxStaticMesh* pStaticMesh, 
										 DxPieceManagerPSF* pPieceManager, 
										 const D3DXMATRIX& matWorld, 
										 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										 DWORD& dwStartTextureSize,
										 WORDWORD& wwBaseTexelXY,
										 BOOL& bEnoughUsedPixelLightMap,
										 const WORDWORD& wwLightID4Shadow )
{
	DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };

	//const DxTextureEffMan* pTextureEffMan(NULL);
	//if ( pPieceEdit )
	//{
	//	pTextureEffMan = pPieceEdit->GetTextureEffMan();
	//}

	std::vector<BOOL>				vecReceiveShadowLM;
	std::vector<VERTEXNORCOLORTEX4>	vecVertices;
	std::vector<D3DXVECTOR2>		vecLightMapUV;
	for ( DWORD i=0; i<5; ++i )
	{
		DxSingleTexMesh* pCur = pListArray[i];
		while( pCur )
		{
			pCur->GetVertices( vecVertices, vecLightMapUV, vecReceiveShadowLM );
			pCur = pCur->m_pNext;
		}
	}

	if ( vecVertices.empty() )
		return;

	// ���ؽ� ������ ��´�.
	VERTEXNORCOLORTEX4* pVerticesVERTEX = new VERTEXNORCOLORTEX4[vecVertices.size()];
	for ( DWORD i=0; i<vecVertices.size(); ++i )
	{
		pVerticesVERTEX[i] = vecVertices[i];
	}
	

	// LightMap ������ ������ Color �� ����� 
	{
		std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ> spNew( std::tr1::shared_ptr<SVEC_LIGHTMAPOBJ>(new SVEC_LIGHTMAPOBJ) );
		spNew->m_dwLightMapFaceNUM = static_cast<DWORD>( vecVertices.size()/3 );

		NSLightMap::CreatePieceLightMapUserUV
		( 
			pd3dDevice,
			strLightMapName, 
			_T("_P"), 
			FALSE,
			TRUE,
			spNew->m_vecLightMapObject,
			spNew->m_dwLightMapFaceNUM, 
			reinterpret_cast<BYTE*>(pVerticesVERTEX),
			VERTEXNORCOLORTEX4::FVF,
			pStaticMesh,
			pPieceManager, 
			matWorld,
			dwStartTextureSize,
			bEnoughUsedPixelLightMap,
			vecReceiveShadowLM,
			m_wwLightMapUserUV_TEXEL,
			vecLightMapUV,
			wwLightID4Shadow
		);

		vecsvecLightMapObj.push_back( spNew );
	}

	SAFE_DELETE_ARRAY( pVerticesVERTEX );

	//------- �ּ� Base UV �� ���Ѵ�.
	wwBaseTexelXY = WORDWORD(4095,4095);
	for ( DWORD i=0; i<vecsvecLightMapObj.size(); ++i )
	{
		for ( DWORD j=0; j<vecsvecLightMapObj[i]->m_vecLightMapObject.size(); ++j )
		{
			LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j].get();

			if ( wwBaseTexelXY.wA > pLightMapObject->GetBaseTexelXY().wA )
				wwBaseTexelXY.wA = pLightMapObject->GetBaseTexelXY().wA;
			if ( wwBaseTexelXY.wB > pLightMapObject->GetBaseTexelXY().wB )
				wwBaseTexelXY.wB = pLightMapObject->GetBaseTexelXY().wB;
		}
	}

	// ������ ���´�.
	for ( DWORD i=0; i<vecsvecLightMapObj.size(); ++i )
	{
		for ( DWORD j=0; j<vecsvecLightMapObj[i]->m_vecLightMapObject.size(); ++j )
		{
			LightMapObject* pLightMapObject = vecsvecLightMapObj[i]->m_vecLightMapObject[j].get();
			pLightMapObject->SetBaseTexelPieceXY( wwBaseTexelXY );
		}
	}
}

//void DxStaticMesh::CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
//                                        const TSTRING& strLightMapName,
//                                        TSTRING& strTextureDay, 
//                                        TSTRING& strTextureNight,
//                                        TSTRING& strTextureAvgDay,
//                                        TSTRING& strTextureAvgNight,
//                                        VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
//                                        WORDWORD wwBaseTexelXY )
//{
//    DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };
//
//    int nCount(0);
//    for ( DWORD i=0; i<5; ++i )
//    {
//        DxSingleTexMesh* pCur = pListArray[i];
//        while( pCur )
//        {
//            // strTextureDay ���� ��ȸ ���� ���� ���;� �Ѵ�.
//            pCur->CreateLightMapPiece( pd3dDevice, 
//                                        strLightMapName,
//                                        strTextureDay, 
//                                        strTextureNight,
//                                        strTextureAvgDay,
//                                        strTextureAvgNight,
//                                        vecsvecLightMapObj[nCount++].get(),
//                                        wwBaseTexelXY );
//	        pCur = pCur->m_pNext;
//        }
//    }
//}

void DxStaticMesh::CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										   const TSTRING& strLightMapName,
										   TSTRING& strTextureCombined, 
										   TSTRING& strTextureDirect_Shadow_1,
										   TSTRING& strTextureDirect_Shadow_2,
										   TSTRING& strTextureIndirect,
										   VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										   WORDWORD wwBaseTexelXY )
{
	if ( vecsvecLightMapObj.empty() )
		return;

	// NSLightMap �ؽ���(m_vecTextureData) �� �����Ѵ�.
	// ������ �����۾�.
	NSLightMap::CreateLightMap2_PIECE( vecsvecLightMapObj[0]->m_vecLightMapObject, 
										strLightMapName, 
										_T("_P"), 
										FALSE,
										TRUE,
										strTextureCombined, 
										strTextureDirect_Shadow_1,
										strTextureDirect_Shadow_2,
										strTextureIndirect,
										wwBaseTexelXY,
										TRUE,
										TRUE );

}

void DxStaticMesh::CreateLightMapUVPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, 
												 const TSTRING& strLightMapName,
												 TSTRING& strTextureCombined, 
												 TSTRING& strTextureDirect_Shadow_1,
												 TSTRING& strTextureDirect_Shadow_2,
												 TSTRING& strTextureIndirect,
												 VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
												 WORDWORD wwBaseTexelXY,
												 BOOL bRan_1_0, 
												 float fCellSize )
{
	if ( vecsvecLightMapObj.empty() )
		return;

	DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };

	int nCount(0);
	for ( DWORD i=0; i<5; ++i )
	{
		DxSingleTexMesh* pCur = pListArray[i];
		while( pCur )
		{
			// strTextureDay ���� ��ȸ ���� ���� ���;� �Ѵ�.
			pCur->CreateLightMapUVPiece( pd3dDevice, 
										vecsvecLightMapObj[0]->m_vecLightMapObject[0].get(),
										strTextureCombined, 
										strTextureDirect_Shadow_1,
										strTextureDirect_Shadow_2,
										strTextureIndirect,
										wwBaseTexelXY,
										bRan_1_0,
										fCellSize );
			pCur = pCur->m_pNext;
		}
	}
}

void DxStaticMesh::LoadLightMapOLD( LPDIRECT3DDEVICEQ pd3dDevice )
{
    DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };

    for ( DWORD i=0; i<5; ++i )
    {
        DxSingleTexMesh* pCur = pListArray[i];
        while( pCur )
        {
            pCur->LoadLightMap( pd3dDevice );
	        pCur = pCur->m_pNext;
        }
    }
}

void DxStaticMesh::FrameMove( const float fElapsedTime )
{
    DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };

    for ( DWORD i=0; i<5; ++i )
    {
        DxSingleTexMesh* pCur = pListArray[i];
	    while( pCur )
	    {
		    pCur->FrameMove( fElapsedTime );
		    pCur = pCur->m_pNext;
	    }
    }

	//// Note : ���� ���� �� ���� ����ϸ� �ȵȴ�.
	//if( m_emRenderType==EMRT_PIECE )
 //       return;

	//// Note : �÷� ����� ���Ͽ��� ����Ʈ�� ����ִٸ� �ٽ� �����Ѵ�.
	//if( DxStaticMeshColor_THREAD::GetInstance().IsInsertListData() )
	//{
	//	DxStaticMeshColor_THREAD::GetInstance().EnterCS();
	//	{
	//		LOADINGDATALIST& sListColorData = DxStaticMeshColor_THREAD::GetInstance().m_listColorData;

	//		DxSingleTexMesh* pSTMesh = m_pList;
	//		while( pSTMesh )
	//		{
	//			pSTMesh->InsertColorList( sListColorData );
	//			pSTMesh = pSTMesh->m_pNext;
	//		}

	//		pSTMesh = m_pAlphaList;
	//		while( pSTMesh )
	//		{
	//			pSTMesh->InsertColorList( sListColorData );
	//			pSTMesh = pSTMesh->m_pNext;
	//		}

	//		pSTMesh = m_pAlphaSoftList;
	//		while( pSTMesh )
	//		{
	//			pSTMesh->InsertColorList( sListColorData );
	//			pSTMesh = pSTMesh->m_pNext;
	//		}

	//		pSTMesh = m_pAlphaSoftList01;
	//		while( pSTMesh )
	//		{
	//			pSTMesh->InsertColorList( sListColorData );
	//			pSTMesh = pSTMesh->m_pNext;
	//		}

	//		pSTMesh = m_pAlphaSoftList02;
	//		while( pSTMesh )
	//		{
	//			pSTMesh->InsertColorList( sListColorData );
	//			pSTMesh = pSTMesh->m_pNext;
	//		}

	//		//COLLISION::InsertColorList( m_pMeshTree, sListColorData );
	//		//COLLISION::InsertColorList( m_pMeshAlphaTree, sListColorData );
	//		//COLLISION::InsertColorList( m_pMeshSoftAlphaTree, sListColorData );
	//		//COLLISION::InsertColorList( m_pMeshSoftAlphaTree01, sListColorData );
	//		//COLLISION::InsertColorList( m_pMeshSoftAlphaTree02, sListColorData );
	//	}
	//	DxStaticMeshColor_THREAD::GetInstance().LeaveCS();
	//}
}

// Note : Thread Loading Render
void DxStaticMesh::Render_THREAD_( const LPDIRECT3DDEVICEQ pd3dDevice, 
								  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								  const CLIPVOLUME &sCV )
{
	//DxSingleTexMesh* pCur = m_pList;
	//while( pCur )
	//{
	//	pCur->RenderTHREAD( pd3dDevice, emCullingRenderType, sCV, listLoadingData, sCSLockLoading, FALSE );
	//	pCur = pCur->m_pNext;
	//}

	

	PROFILE_BEGIN("DxOctree::Render_THREAD_");
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, emCullingRenderType, sCV, m_pMeshTree, FALSE );
	PROFILE_END("DxOctree::Render_THREAD_");
}

void DxStaticMesh::Render_THREAD_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
									   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									   const CLIPVOLUME &sCV )
{
	//DxSingleTexMesh* pCur = m_pAlphaList;
	//while( pCur )
	//{
	//	pCur->RenderTHREAD( pd3dDevice, emCullingRenderType, sCV, listLoadingData, sCSLockLoading, FALSE );
	//	pCur = pCur->m_pNext;
	//}

	PROFILE_BEGIN("DxOctree::Render_THREAD_Alpha");
	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, emCullingRenderType, sCV, m_pMeshAlphaTree, FALSE );
	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	PROFILE_END("DxOctree::Render_THREAD_Alpha");
}

void DxStaticMesh::Render_THREAD_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
										   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
										   const CLIPVOLUME &sCV )
{
	PROFILE_BEGIN("DxOctree::Render_THREAD_SoftAlpha");
	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, emCullingRenderType, sCV, m_pMeshSoftAlphaTree, FALSE );
	//DxSingleTexMesh* pCur = m_pAlphaSoftList;
	//while( pCur )
	//{
	//	pCur->RenderTHREAD( pd3dDevice, emCullingRenderType, sCV, listLoadingData, sCSLockLoading, FALSE );
	//	pCur = pCur->m_pNext;
	//}

	float fBias;
	fBias = -0.0001f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, emCullingRenderType, sCV, m_pMeshSoftAlphaTree01, FALSE );
	//pCur = m_pAlphaSoftList01;
	//while( pCur )
	//{
	//	pCur->RenderTHREAD( pd3dDevice, emCullingRenderType, sCV, listLoadingData, sCSLockLoading, FALSE );
	//	pCur = pCur->m_pNext;
	//}

	fBias = -0.0002f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	COLLISION::RenderAABBTreeTHREAD( pd3dDevice, emCullingRenderType, sCV, m_pMeshSoftAlphaTree02, FALSE );
	//pCur = m_pAlphaSoftList02;
	//while( pCur )
	//{
	//	pCur->RenderTHREAD( pd3dDevice, emCullingRenderType, sCV, listLoadingData, sCSLockLoading, FALSE );
	//	pCur = pCur->m_pNext;
	//}

	fBias = 0.0000f;
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
	PROFILE_END("DxOctree::Render_THREAD_SoftAlpha");
}

//// Note : Dynamic Loading Render	|| General Render <SFile==NULL>
//void DxStaticMesh::Render_DYNAMIC_( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile )
//{
//	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshTree, SFile );
//}
//
//void DxStaticMesh::Render_DYNAMIC_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile )
//{
//	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
//	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshAlphaTree, SFile );
//	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
//}
//
//void DxStaticMesh::Render_DYNAMIC_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, sc::SerialFile* const SFile )
//{
//	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
//	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshSoftAlphaTree, SFile );
//	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshSoftAlphaTree01, SFile );
//	COLLISION::RenderAABBTreeDYNAMIC( pd3dDevice, sCV, m_pMeshSoftAlphaTree02, SFile );
//	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
//}

// Piece File Render
void DxStaticMesh::Render_PIECE_( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                    DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
                                    const D3DXMATRIX& matWorld, 
                                    const D3DXVECTOR2& vLightMapUV_Offset,
									float fAlpha, 
									int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
									LPDIRECT3DTEXTUREQ pTextureDay, 
									LPDIRECT3DTEXTUREQ pTextureNight,
									BOOL bMPShading )
{
	NSLightMapFX::SetLightMapUV_Multiply( m_wwLightMapUserUV_TEXEL );

	DxSingleTexMesh* pCur = m_pList;
	while( pCur )
	{
		pCur->RenderPiece( pd3dDevice, pmapTexEff, matWorld, TRUE, vLightMapUV_Offset, fAlpha, pTextureDay, pTextureNight );
		pCur = pCur->m_pNext;
	}
}

void DxStaticMesh::Render_PIECE_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                        DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
                                        const D3DXMATRIX& matWorld, 
                                        const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha, 
										int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight,
										BOOL bMPShading )
{
	NSLightMapFX::SetLightMapUV_Multiply( m_wwLightMapUserUV_TEXEL );

	// [shhan][2014.07.22]
	//				���⿡ �ְ� ��ܿ��� �ִ�. 
	//				�ߺ����� ���õǸ鼭 NSBloomHDR ��ɿ� �ɰ��� ���װ� �����. �Ͼ�� Ÿ�� ���� �߻�.
	//DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

	DxSingleTexMesh* pCur = m_pAlphaList;
	while( pCur )
	{
		pCur->RenderPiece( pd3dDevice, pmapTexEff, matWorld, TRUE, vLightMapUV_Offset, fAlpha, pTextureDay, pTextureNight );
		pCur = pCur->m_pNext;
	}

	//DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
}

void DxStaticMesh::Render_PIECE_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
                                            DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
                                            const D3DXMATRIX& matWorld, 
                                            const D3DXVECTOR2& vLightMapUV_Offset,
											float fAlpha, 
											int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
											LPDIRECT3DTEXTUREQ pTextureDay, 
											LPDIRECT3DTEXTUREQ pTextureNight,
											BOOL bMPShading )
{
	NSLightMapFX::SetLightMapUV_Multiply( m_wwLightMapUserUV_TEXEL );

	// [shhan][2014.07.22]
	//				���⿡ �ְ� ��ܿ��� �ִ�. 
	//				�ߺ����� ���õǸ鼭 NSBloomHDR ��ɿ� �ɰ��� ���װ� �����. �Ͼ�� Ÿ�� ���� �߻�.
	//DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

	DxSingleTexMesh* pCur;
	pCur = m_pAlphaSoftList;
	while( pCur )
	{
		pCur->RenderPiece( pd3dDevice, pmapTexEff, matWorld, TRUE, vLightMapUV_Offset, fAlpha, pTextureDay, pTextureNight );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList01;
	while( pCur )
	{
		pCur->RenderPiece( pd3dDevice, pmapTexEff, matWorld, TRUE, vLightMapUV_Offset, fAlpha, pTextureDay, pTextureNight );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList02;
	while( pCur )
	{
		pCur->RenderPiece( pd3dDevice, pmapTexEff, matWorld, TRUE, vLightMapUV_Offset, fAlpha, pTextureDay, pTextureNight );
		pCur = pCur->m_pNext;
	}

	//DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
}

void DxStaticMesh::Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	// [NAS]
	//DxSingleTexMesh* pCur(NULL);
	//DxSingleTexMesh* pPoint[] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };
	//for( DWORD i=0; i<5; ++i )
	//{
	//	pCur = pPoint[i];
	//	while( pCur )
	//	{
	//		pCur->RenderPiece( pd3dDevice );
	//		pCur = pCur->m_pNext;
	//	}
	//}
}

void DxStaticMesh::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	COLLISION::Render_Query( pd3dDevice, sCV, m_pMeshTree );
}

VOID DxStaticMesh::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for ( ; iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->CreateQuery( pd3dDevice );
	}
}

VOID DxStaticMesh::DestroyQuery()
{
	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for ( ; iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->DestroyQuery();
	}
}

void DxStaticMesh::ResetOcclusionQuery()
{
	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for ( ; iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->ResetOcclusionQuery();
	}
}

void DxStaticMesh::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for ( ; iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
	}
}

//
//// Note : Piece File Render
//void DxStaticMesh::Render( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
//{
//	MAPSINGLETEXMESH_ITER iter = m_mapMesh.find( szName );
//	if( iter!=m_mapMesh.end() )
//	{
//		(*iter).second->Render( pd3dDevice, sCV );
//	}
//}
//
//void DxStaticMesh::RenderAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
//{
//	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
//	{
//		MAPSINGLETEXMESH_ITER iter = m_mapMeshALPHA.find( szName );
//		if( iter!=m_mapMeshALPHA.end() )
//		{
//			(*iter).second->Render( pd3dDevice, sCV );
//		}
//	}
//	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
//}
//
//void DxStaticMesh::RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
//{
//	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
//	{
//		MAPSINGLETEXMESH_ITER iter = m_mapMeshSOFTALPHA.find( szName );
//		if( iter!=m_mapMeshSOFTALPHA.end() )
//		{
//			(*iter).second->Render( pd3dDevice, sCV );
//		}
//	}
//	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
//}
//
//void DxStaticMesh::RenderSoftAlpha01( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
//{
//	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
//	{
//		MAPSINGLETEXMESH_ITER iter = m_mapMeshSOFTALPHA01.find( szName );
//		if( iter!=m_mapMeshSOFTALPHA01.end() )
//		{
//			(*iter).second->Render( pd3dDevice, sCV );
//		}
//	}
//	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
//}
//
//void DxStaticMesh::RenderSoftAlpha02( const LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const CLIPVOLUME &sCV )
//{
//	DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );
//	{
//		MAPSINGLETEXMESH_ITER iter = m_mapMeshSOFTALPHA02.find( szName );
//		if( iter!=m_mapMeshSOFTALPHA02.end() )
//		{
//			(*iter).second->Render( pd3dDevice, sCV );
//		}
//	}
//	DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
//}

// Note : vEnd �� ���� ��� �پ���.
//		�׷��� ���� ���� ��ġ�� �ִ°��� ã�� �� �ִ�.
BOOL DxStaticMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	BOOL		bUse		= FALSE;

	MAPSINGLETEXMESH_CITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, emCullMode, bOnlyCamColl ) )	bUse = TRUE;
	}

	if ( !(emCullMode & EMCC_EXCEPT_ALPHATEX) )
	{
		iter = m_mapMeshALPHA.begin();
		for( ; iter!=m_mapMeshALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, emCullMode, bOnlyCamColl ) )	bUse = TRUE;
		}

		iter = m_mapMeshSOFTALPHA.begin();
		for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, emCullMode, bOnlyCamColl ) )	bUse = TRUE;
		}

		iter = m_mapMeshSOFTALPHA01.begin();
		for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, emCullMode, bOnlyCamColl ) )	bUse = TRUE;
		}

		iter = m_mapMeshSOFTALPHA02.begin();
		for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, szName, emCullMode, bOnlyCamColl ) )	bUse = TRUE;
		}
	}

	return bUse;
}

// Note : vEnd �� ���� ��� �پ���.
//		�׷��� ���� ���� ��ġ�� �ִ°��� ã�� �� �ִ�.
BOOL DxStaticMesh::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	BOOL		bUse		= FALSE;

	MAPSINGLETEXMESH_CITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode, bOnlyCamColl) )	bUse = TRUE;
	}

	if ( !(emCullMode & EMCC_EXCEPT_ALPHATEX) )
	{
		iter = m_mapMeshALPHA.begin();
		for( ; iter!=m_mapMeshALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode, bOnlyCamColl) )	bUse = TRUE;
		}

		iter = m_mapMeshSOFTALPHA.begin();
		for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode, bOnlyCamColl) )	bUse = TRUE;
		}

		iter = m_mapMeshSOFTALPHA01.begin();
		for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode, bOnlyCamColl) )	bUse = TRUE;
		}

		iter = m_mapMeshSOFTALPHA02.begin();
		for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLine( vStart, vEnd, vColl, vNor, emCullMode, bOnlyCamColl) )	bUse = TRUE;
		}
	}

	return bUse;
}

BOOL DxStaticMesh::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	MAPSINGLETEXMESH_CITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
			return TRUE;
	}

	if ( !(emCullMode & EMCC_EXCEPT_ALPHATEX) )
	{
		iter = m_mapMeshALPHA.begin();
		for( ; iter!=m_mapMeshALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		iter = m_mapMeshSOFTALPHA.begin();
		for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		iter = m_mapMeshSOFTALPHA01.begin();
		for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		iter = m_mapMeshSOFTALPHA02.begin();
		for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}
	}

	return FALSE;
}

// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
BOOL DxStaticMesh::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	MAPSINGLETEXMESH_CITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
            return TRUE;
	}

	if ( !(emCullMode & EMCC_EXCEPT_ALPHATEX) )
	{
		iter = m_mapMeshALPHA.begin();
		for( ; iter!=m_mapMeshALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
				return TRUE;
		}

		iter = m_mapMeshSOFTALPHA.begin();
		for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
				return TRUE;
		}

		iter = m_mapMeshSOFTALPHA01.begin();
		for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
				return TRUE;
		}

		iter = m_mapMeshSOFTALPHA02.begin();
		for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
		{
			if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
				return TRUE;
		}
	}

    return FALSE;
}

void DxStaticMesh::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // �ֺ��� �߰� �޼ҵ�
{
	BOOL bUse = FALSE;

	//Check
  	MAPSINGLETEXMESH_ITER iter = m_mapMesh.begin();
	for( ; iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}

	iter = m_mapMeshALPHA.begin();
	for( ; iter!=m_mapMeshALPHA.end(); ++iter )
	{
		(*iter).second->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}

	iter = m_mapMeshSOFTALPHA.begin();
	for( ; iter!=m_mapMeshSOFTALPHA.end(); ++iter )
	{
		(*iter).second->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}

	iter = m_mapMeshSOFTALPHA01.begin();
	for( ; iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
	{
		(*iter).second->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}

	iter = m_mapMeshSOFTALPHA02.begin();
	for( ; iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
	{
		(*iter).second->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}

	/*
	//Check_DYNAMIC
 	if ( m_pMeshTree )
	{
  		COLLISION::GetCollisionLineInTriListExceptY( m_pMeshTree, vStart, vEnd, TriList);
	}

	//Check_DYNAMIC_Alpha
	if ( m_pMeshAlphaTree )
	{
		COLLISION::GetCollisionLineInTriListExceptY( m_pMeshAlphaTree, vStart, vEnd, TriList );
	}

	//Check_PIECE
	DxSingleTexMesh* pCur;
	pCur = m_pList;
	while( pCur )
	{
		pCur->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		pCur = pCur->m_pNext;
	}

	//Check_PIECE_Alpha
	pCur = m_pAlphaList;
	while( pCur )
	{
		pCur->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList;
	while( pCur )
	{
		pCur->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList01;
	while( pCur )
	{
		pCur->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		pCur = pCur->m_pNext;
	}

	pCur = m_pAlphaSoftList02;
	while( pCur )
	{
		pCur->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		pCur = pCur->m_pNext;
	}
	*/
}

void DxStaticMesh::GetCollisionLineInTriListExceptYReplacePiece ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList ) // �ֺ��� �߰� �޼ҵ�
{

}

BOOL DxStaticMesh::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	//MAPSINGLETEXMESH* mapMesh_ARRAY[5] = { &m_mapMesh,
	//										&m_mapMeshALPHA,
	//										&m_mapMeshSOFTALPHA,
	//										&m_mapMeshSOFTALPHA01,
	//										&m_mapMeshSOFTALPHA02 };

	BOOL bExist(FALSE);
	//for ( DWORD i=0; i<5; ++i )
	//{
	//	if ( mapMesh_ARRAY[i]->empty() )
	//		continue;

	//	MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY[i]->begin();
	//	for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
	//	{
	//		if ( (*iter).second->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor ) )
	//		{
	//			bExist = TRUE;
	//		}
	//	}
	//}
	return bExist;
}

BOOL DxStaticMesh::IsLightMap()
{
	MAPSINGLETEXMESH* mapMesh_ARRAY[5] = { &m_mapMesh,
											&m_mapMeshALPHA,
											&m_mapMeshSOFTALPHA,
											&m_mapMeshSOFTALPHA01,
											&m_mapMeshSOFTALPHA02 };

	for ( DWORD i=0; i<5; ++i )
	{
		if ( mapMesh_ARRAY[i]->empty() )
			continue;

		MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY[i]->begin();
		for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
		{
			if ( (*iter).second->IsLightMap() )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void DxStaticMesh::LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };
	for ( DWORD i=0; i<5; ++i )
	{
		DxSingleTexMesh* pCur = pListArray[i];
		while( pCur )
		{
			pCur->LoadLightMap( pd3dDevice );
			pCur = pCur->m_pNext;
		}
	}
}

void DxStaticMesh::SetAABBMaxMin ()
{
	m_vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	D3DXVECTOR3	vMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	D3DXVECTOR3	vMin( FLT_MAX, FLT_MAX, FLT_MAX );

	MAPSINGLETEXMESH_ITER iter;
	for( iter=m_mapMesh.begin(); iter!=m_mapMesh.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshALPHA.begin(); iter!=m_mapMeshALPHA.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshSOFTALPHA.begin(); iter!=m_mapMeshSOFTALPHA.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshSOFTALPHA01.begin(); iter!=m_mapMeshSOFTALPHA01.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}

	for( iter=m_mapMeshSOFTALPHA02.begin(); iter!=m_mapMeshSOFTALPHA02.end(); ++iter )
	{
		(*iter).second->GetAABBSize( vMax, vMin );

		if ( m_vMax.x < vMax.x )	m_vMax.x = vMax.x;
		if ( m_vMax.y < vMax.y )	m_vMax.y = vMax.y;
		if ( m_vMax.z < vMax.z )	m_vMax.z = vMax.z;

		if ( m_vMin.x > vMin.x )	m_vMin.x = vMin.x;
		if ( m_vMin.y > vMin.y )	m_vMin.y = vMin.y;
		if ( m_vMin.z > vMin.z )	m_vMin.z = vMin.z;
	}
}

void DxStaticMesh::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, sc::SerialFile* const pSFile )
{
	float fDis = DxFogMan::GetInstance().GetFogRangeFar();
	D3DXVECTOR3 vMax = vPos + D3DXVECTOR3( fDis, fDis, fDis );
	D3DXVECTOR3 vMin = vPos - D3DXVECTOR3( fDis, fDis, fDis );

	BaseLoad( pd3dDevice, vMax, vMin, pSFile );
}

void DxStaticMesh::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const pSFile )
{
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshTree, pSFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshAlphaTree, pSFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshSoftAlphaTree, pSFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshSoftAlphaTree01, pSFile );
	COLLISION::LoadAABBTreeBOX( pd3dDevice, vMax, vMin, m_pMeshSoftAlphaTree02, pSFile );
}

//void DxStaticMesh::Save ( const char* szName )
//{
//	std::string strName = szName;
//
//	std::string::size_type idx;
//	idx = strName.find( ".wld" );
//	if( idx!=std::string::npos )	strName += '0';		// wld0 �� ����.
//
//	if ( m_SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )	Save( m_SFile, FALSE );
//
//	m_SFile.CloseFile();
//}
//
//void DxStaticMesh::Load ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
//{
//	std::string strName = szName;
//
//	std::string::size_type idx;
//	idx = strName.find( ".wld" );
//	if( idx!=std::string::npos )	strName += '0';		// wld0 �� ����.
//
//	m_SFile.CloseFile();
//	if ( m_SFile.OpenFile ( FOT_READ, strName.c_str() ) )	Load( pd3dDevice, m_SFile, TRUE, FALSE );
//}

void DxStaticMesh::Save ( sc::SerialFile& SFile, BOOL bPiece )
{
	DWORD	dwSize = 0;

	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		SFile << m_vMax;
		SFile << m_vMin;
		SFile << m_wwLightMapUserUV_TEXEL.dwData;
	}
	SFile.EndBlock();

	SingleTexMesh_Save( m_mapMesh,			SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshSOFTALPHA, SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshSOFTALPHA01, SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshSOFTALPHA02, SFile, bPiece );
	SingleTexMesh_Save( m_mapMeshALPHA,		SFile, bPiece );
}

void DxStaticMesh::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bDynamicLoad, const BOOL bPiece )
{
	CleanUp();	// ��� �ʱ�ȭ

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	switch ( dwVer )
	{
	case 0x0100:
		{
			Load_101( pd3dDevice, SFile, bDynamicLoad, bPiece );
			// Ver.0x0100 ���� Max, Min ������ �߸� �Ǿ� �־���.
			SetAABBMaxMin ();
		}
		break;
	
	case 0x0101:
		{
			Load_101( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		break;
	
	case 0x0102:
		{
			Load_102( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		break;
	
	case 0x0103:
		{
			Load_103( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		break;

    case 0x0104:
		{
			Load_104( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		break;

	case VERSION:
		{
			Load_105( pd3dDevice, SFile, bDynamicLoad, bPiece );
		}
		break;

	default:
		{
			DWORD dwCur = SFile.GetfTell();
			SFile.SetOffSet( dwCur+dwBufferSize );
		}
		break;
	};	

	return;
}

void DxStaticMesh::SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const DxTextureEffNewFile& sTexEffNewFile )
{
    const MAP_VECTEXEFF& mapTexEff = sTexEffNewFile.GetTexEff();
    MAP_VECTEXEFF_CITER iter = mapTexEff.begin();
    for ( ; iter!=mapTexEff.end(); ++iter )
    {
        SetTextureEffNewFile( pd3dDevice, m_mapMesh, (*iter).first, (*iter).second );
        SetTextureEffNewFile( pd3dDevice, m_mapMeshSOFTALPHA, (*iter).first, (*iter).second );
        SetTextureEffNewFile( pd3dDevice, m_mapMeshSOFTALPHA01, (*iter).first, (*iter).second );
        SetTextureEffNewFile( pd3dDevice, m_mapMeshSOFTALPHA02, (*iter).first, (*iter).second );
        SetTextureEffNewFile( pd3dDevice, m_mapMeshALPHA, (*iter).first, (*iter).second );
    }
}

void DxStaticMesh::SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, const TSTRING& strName, const VEC_TEXEFF& vecTexEff )
{
    MAPSINGLETEXMESH_ITER iter = mapMesh.lower_bound( NAME_LMID(strName,0, strName) );
    for ( ; iter!=mapMesh.end(); ++iter )
    {
        if ( (*iter).first.m_strMaterialName == strName )
        {
            (*iter).second->SetTextureEffNewFile( pd3dDevice, vecTexEff );
        }
        else
        {
            return;
        }
    }
}

// TexEff �� �����´�.
void DxStaticMesh::GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffNewFile& sTexEffNewFile )
{
    GetTexEff( pd3dDevice, m_mapMesh, sTexEffNewFile );
    GetTexEff( pd3dDevice, m_mapMeshSOFTALPHA, sTexEffNewFile );
    GetTexEff( pd3dDevice, m_mapMeshSOFTALPHA01, sTexEffNewFile );
    GetTexEff( pd3dDevice, m_mapMeshSOFTALPHA02, sTexEffNewFile );
    GetTexEff( pd3dDevice, m_mapMeshALPHA, sTexEffNewFile );
}

// TexEff �� �����´�.
void DxStaticMesh::GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, DxTextureEffNewFile& sTexEffNewFile )
{
    MAPSINGLETEXMESH_ITER iter;
    for ( iter=mapMesh.begin(); iter!=mapMesh.end(); ++iter )
    {
        VEC_TEXEFF vecTexEff;
	    if ( (*iter).second->GetTexEff( pd3dDevice, vecTexEff ) )
        {
            sTexEffNewFile.Insert( (*iter).first.m_strMaterialName, vecTexEff );
        }
        vecTexEff.clear();
    }
}

void DxStaticMesh::SingleTexMesh_Save( MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile, BOOL bPiece )
{
	DWORD	dwSize = 0;

	dwSize = (DWORD)mapMesh.size();
	SFile << dwSize;

	MAPSINGLETEXMESH_ITER iter;
	for ( iter=mapMesh.begin(); iter!=mapMesh.end(); ++iter )
	{
		SFile << (*iter).first.m_strMaterialName;
        SFile << (*iter).first.m_dwLightMapID;
		(*iter).second->Save( SFile, bPiece );
	}
}

void DxStaticMesh::SingleTexMesh_Load_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	DWORD	dwSize = 0;

	if( bPiece )	m_emRenderType = EMRT_PIECE;
	else			m_emRenderType = EMRT_OCTREE;

	std::string szName;
    DWORD dwLMID;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
        SFile >> dwLMID;
		DxSingleTexMesh* pNew = new DxSingleTexMesh;
		pNew->Load( pd3dDevice, SFile, bDynamicLoad, bPiece, FALSE );

		mapMesh.insert( std::make_pair ( NAME_LMID(szName,dwLMID,szName), pNew ) );
	}
}

void DxStaticMesh::SingleTexMesh_Load_Insert_OLD( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece )
{
	DWORD	dwSize = 0;

	if( bPiece )	m_emRenderType = EMRT_PIECE;
	else			m_emRenderType = EMRT_OCTREE;

	std::string szName;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
		DxSingleTexMesh* pNew = new DxSingleTexMesh;
		pNew->Load( pd3dDevice, SFile, bDynamicLoad, bPiece, FALSE );

		mapMesh.insert( std::make_pair ( NAME_LMID(szName,0,szName), pNew ) );
	}
}

void DxStaticMesh::Load_105( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece )
{
	SFile >> m_vMax;
	SFile >> m_vMin;
	SFile >> m_wwLightMapUserUV_TEXEL.dwData;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA01, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA02, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

	// SingleMehs Tree�� �����Ѵ�.
	COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
	COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMesh::Load_104( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

    s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA01, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshSOFTALPHA02, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

	// SingleMehs Tree�� �����Ѵ�.
	COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
	COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMesh::Load_103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

#ifndef  NDEBUG
	static float s_fStartTime = 0.0f;
	static float s_fEndTime = 0.f;

    s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
    
#endif

	// Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA01, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA02, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );


#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

	// SingleMehs Tree�� �����Ѵ�.
	COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
	COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif
}

void DxStaticMesh::Load_102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

#ifndef  NDEBUG
	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA01, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA02, SFile, bDynamicLoad, bPiece );

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "Static Load : %f", s_fEndTime );

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
#endif

	// Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMesh,				m_pList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshALPHA,			m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA,		m_pAlphaSoftList );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA01,	m_pAlphaSoftList01 );
	NSSTATICMESH::ConvertMAPtoLIST( m_mapMeshSOFTALPHA02,	m_pAlphaSoftList02 );

	// SingleMehs Tree�� �����Ѵ�.
	COLLISION::MakeAABBTree( m_pMeshTree,				m_pList );
	COLLISION::MakeAABBTree( m_pMeshAlphaTree,			m_pAlphaList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree,		m_pAlphaSoftList );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree01,	m_pAlphaSoftList01 );
	COLLISION::MakeAABBTree( m_pMeshSoftAlphaTree02,	m_pAlphaSoftList02 );

#ifndef  NDEBUG
	s_fEndTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	s_fEndTime = s_fEndTime - s_fStartTime;
	CDebugSet::ToListView ( "MakeAABBTree : %f", s_fEndTime );
#endif

}

void DxStaticMesh::Load_101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, const BOOL bPiece )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

	static float s_fStartTime = 0.f;
	static float s_fEndTime = 0.f;

	s_fStartTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

	// Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
	DWORD	dwSize = 0;
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMesh, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshALPHA, SFile, bDynamicLoad, bPiece );
	SingleTexMesh_Load_Insert_OLD( pd3dDevice, m_mapMeshSOFTALPHA, SFile, bDynamicLoad, bPiece );

	// Tree�� ����� ���� std::map ������ �Ϲ����� list �������� �����Ѵ�.
	NSSTATICMESH::ConvertMAPtoLIST ( m_mapMesh,			m_pList );
	NSSTATICMESH::ConvertMAPtoLIST ( m_mapMeshALPHA,	m_pAlphaList );
	NSSTATICMESH::ConvertMAPtoLIST ( m_mapMeshSOFTALPHA, m_pAlphaSoftList );

	// SingleMehs Tree�� �����Ѵ�.
	COLLISION::MakeAABBTree ( m_pMeshTree,			m_pList );
	COLLISION::MakeAABBTree ( m_pMeshAlphaTree,		m_pAlphaList );
	COLLISION::MakeAABBTree ( m_pMeshSoftAlphaTree, m_pAlphaSoftList );
}

BOOL DxStaticMesh::IsLightMapPiece()
{
    MAPSINGLETEXMESH mapMesh[5] = { m_mapMesh,
                                    m_mapMeshALPHA,
                                    m_mapMeshSOFTALPHA,
                                    m_mapMeshSOFTALPHA01,
                                    m_mapMeshSOFTALPHA02 };

    for ( DWORD i=0; i<5; ++i )
    {
        MAPSINGLETEXMESH_ITER iter = mapMesh[i].begin();
        for ( ; iter!=mapMesh[i].end(); ++iter )
        {
            if ( (*iter).second->IsLightMapPiece() )
                return TRUE;
        }
    }
    return FALSE;
}

void DxStaticMesh::SingleTexMesh_SaveLightMapPiece( MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile )
{
	DWORD	dwSize = 0;

	dwSize = (DWORD)mapMesh.size();
	SFile << dwSize;

	MAPSINGLETEXMESH_ITER iter;
	for ( iter=mapMesh.begin(); iter!=mapMesh.end(); ++iter )
	{
		SFile << (*iter).first.m_strMaterialName;
        SFile << (*iter).first.m_dwLightMapID;
		(*iter).second->SaveLightMapPiece( SFile );
	}
}

void DxStaticMesh::SingleTexMesh_LoadLightMapPiece_Insert( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile )
{
	DWORD	dwSize = 0;

	std::string szName;
    DWORD dwLMID(0);
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
        SFile >> dwLMID;
		DxSingleTexMesh* pNew = new DxSingleTexMesh;
		pNew->LoadLightMapPiece( pd3dDevice, SFile );

        mapMesh.insert( std::make_pair ( NAME_LMID(szName,dwLMID,szName), pNew ) );
	}
}

void DxStaticMesh::SingleTexMesh_LoadLightMapPiece_Insert_VER100( LPDIRECT3DDEVICEQ pd3dDevice, MAPSINGLETEXMESH& mapMesh, sc::SerialFile& SFile )
{
	DWORD	dwSize = 0;

	std::string szName;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> szName;
		DxSingleTexMesh* pNew = new DxSingleTexMesh;
		pNew->LoadLightMapPiece( pd3dDevice, SFile );

        mapMesh.insert( std::make_pair ( NAME_LMID(szName,0,szName), pNew ) );
	}
}

void DxStaticMesh::SaveLightMapPiece( sc::SerialFile& SFile )
{
    SFile << static_cast<DWORD>( VERSION_LM );

    SFile.BeginBlock();
    {
        // Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
	    SingleTexMesh_SaveLightMapPiece( m_mapMesh,			SFile );
        SingleTexMesh_SaveLightMapPiece( m_mapMeshALPHA,	SFile );
	    SingleTexMesh_SaveLightMapPiece( m_mapMeshSOFTALPHA, SFile );
	    SingleTexMesh_SaveLightMapPiece( m_mapMeshSOFTALPHA01, SFile );
	    SingleTexMesh_SaveLightMapPiece( m_mapMeshSOFTALPHA02, SFile );
    }
    SFile.EndBlock();
}

void DxStaticMesh::LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;
    
    if ( dwVer==0x0101 )
    {
        // Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
        SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mapMesh, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mapMeshALPHA, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mapMeshSOFTALPHA, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mapMeshSOFTALPHA01, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert( pd3dDevice, m_mapMeshSOFTALPHA02, SFile );
    }
    else if ( dwVer==0x0100 )
    {
        // Note : DxSingleTexMesh ���ο��� Version üũ�� �Ͼ��.
        SingleTexMesh_LoadLightMapPiece_Insert_VER100( pd3dDevice, m_mapMesh, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert_VER100( pd3dDevice, m_mapMeshALPHA, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert_VER100( pd3dDevice, m_mapMeshSOFTALPHA, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert_VER100( pd3dDevice, m_mapMeshSOFTALPHA01, SFile );
        SingleTexMesh_LoadLightMapPiece_Insert_VER100( pd3dDevice, m_mapMeshSOFTALPHA02, SFile );
    }
}

void DxStaticMesh::FlipLightMapRenderMan( DxStaticMeshBase* pStaticMeshBase )
{
	DxStaticMesh* pStaticMesh = MIN_BOOST::polymorphic_downcast<DxStaticMesh*>( pStaticMeshBase );

    MAPSINGLETEXMESH mapMeshSRC[5] = { pStaticMesh->m_mapMesh,
                                        pStaticMesh->m_mapMeshALPHA,
                                        pStaticMesh->m_mapMeshSOFTALPHA,
                                        pStaticMesh->m_mapMeshSOFTALPHA01,
                                        pStaticMesh->m_mapMeshSOFTALPHA02 };

    MAPSINGLETEXMESH mapMeshDEST[5] = { m_mapMesh,
                                        m_mapMeshALPHA,
                                        m_mapMeshSOFTALPHA,
                                        m_mapMeshSOFTALPHA01,
                                        m_mapMeshSOFTALPHA02 };

    // ������ �̵�
    for ( DWORD i=0; i<5; ++i )
    {
        if ( mapMeshDEST[i].empty() )
            continue;

        if ( mapMeshDEST[i].size() != mapMeshSRC[i].size() )
            continue;

        MAPSINGLETEXMESH_ITER iter_DEST = mapMeshDEST[i].begin();
        MAPSINGLETEXMESH_ITER iter_SRC = mapMeshSRC[i].begin();
        for ( ; iter_DEST!=mapMeshDEST[i].end(); ++iter_DEST, ++iter_SRC )
        {
            if ( (*iter_DEST).first != (*iter_SRC).first )
                continue;

            (*iter_DEST).second->FlipLightMapRenderMan( (*iter_SRC).second );
        }
    }
}

void DxStaticMesh::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
    MAPSINGLETEXMESH mapMesh_ARRAY[4] = { m_mapMeshALPHA,
                                        m_mapMeshSOFTALPHA,
                                        m_mapMeshSOFTALPHA01,
                                        m_mapMeshSOFTALPHA02 };

    // ������ �̵�
    for ( DWORD i=0; i<4; ++i )
    {
        if ( mapMesh_ARRAY[i].empty() )
            continue;

        MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY[i].begin();
        for ( ; iter!=mapMesh_ARRAY[i].end(); ++iter )
        {
            (*iter).second->CreateAlphaTexture32Bit( pd3dDevice );
        }
    }
}

void DxStaticMesh::GetClosedPoint( D3DXVECTOR3& vCollisionInOut )
{
    MAPSINGLETEXMESH mapMesh_ARRAY[5] = { m_mapMesh,
                                        m_mapMeshALPHA,
                                        m_mapMeshSOFTALPHA,
                                        m_mapMeshSOFTALPHA01,
                                        m_mapMeshSOFTALPHA02 };

    float fDistance(FLT_MAX);

    D3DXVECTOR3 vBasePos(vCollisionInOut);

    for ( DWORD i=0; i<5; ++i )
    {
        if ( mapMesh_ARRAY[i].empty() )
            continue;

        MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY[i].begin();
        for ( ; iter!=mapMesh_ARRAY[i].end(); ++iter )
        {
            (*iter).second->GetClosedPoint( vBasePos, vCollisionInOut, fDistance );
        }
    }
}

void DxStaticMesh::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	MAPSINGLETEXMESH* mapMesh_ARRAY[5] = { &m_mapMesh,
											&m_mapMeshALPHA,
											&m_mapMeshSOFTALPHA,
											&m_mapMeshSOFTALPHA01,
											&m_mapMeshSOFTALPHA02 };

	for ( DWORD i=0; i<5; ++i )
	{
		if ( mapMesh_ARRAY[i]->empty() )
			continue;

		MAPSINGLETEXMESH_ITER iter = mapMesh_ARRAY[i]->begin();
		for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
		{
			(*iter).second->ExportXFile( pxofsave, pDataObjectRoot, strPieceName, matWorld );
		}
	}
}

// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
BOOL DxStaticMesh::CreateTriangleLinkLightForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
												   DxStaticMeshCollBase* pStaticMesh, 
												   DxPieceManager* pPieceManager, 
												   std::vector<DWORD>& vecLightID, 
												   const D3DXMATRIX& matWorld,
												   const D3DLIGHTQ& sLight,
												   float fAttEndLineValue ) const
{
	const MAPSINGLETEXMESH* mapMesh_ARRAY[5] = { &m_mapMesh,
												&m_mapMeshALPHA,
												&m_mapMeshSOFTALPHA,
												&m_mapMeshSOFTALPHA01,
												&m_mapMeshSOFTALPHA02 };

	std::vector<VERTEXNORCOLORTEX4> vecVerticesS;
	std::vector<D3DXVECTOR2>		vecLightMapUV;
	std::vector<BOOL>				vecRceiveShadowLM;

	for ( DWORD i=0; i<5; ++i )
	{
		if ( mapMesh_ARRAY[i]->empty() )
			continue;

		MAPSINGLETEXMESH_CITER iter = mapMesh_ARRAY[i]->begin();
		for ( ; iter!=mapMesh_ARRAY[i]->end(); ++iter )
		{
			vecVerticesS.clear();
			vecLightMapUV.clear();
			vecRceiveShadowLM.clear();

			(*iter).second->GetVertices( vecVerticesS, vecLightMapUV, vecRceiveShadowLM );

			BOOL bOK = CreateTriangleLinkLightForPieceDetail
			( 
				pd3dDevice, 
				pStaticMesh, 
				pPieceManager, 
				vecLightID, 
				matWorld,
				vecVerticesS,
				sLight,
				fAttEndLineValue
			);

			if ( bOK )
				return TRUE;
		}
	}

	return FALSE;
}

// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
void DxStaticMesh::SetNSLandThread()
{
	DxSingleTexMesh* pListArray[5] = { m_pList, m_pAlphaList, m_pAlphaSoftList, m_pAlphaSoftList01, m_pAlphaSoftList02 };

	for ( DWORD i=0; i<5; ++i )
	{
		DxSingleTexMesh* pCur = pListArray[i];
		while( pCur )
		{
			pCur->CreateDxCullingMeshSwap();
			pCur = pCur->m_pNext;
		}
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										N	S		S	T	A	T	I	C		M	E	S	H
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSSTATICMESH
{
	DWORD			dwMAXFACE	= 21000;	// ���� ũ�� ��ü������ AABB �� ��ü�� Tree�� �����Ѵ�.
	DWORD			dwMAXDIS	= 2000;

	MAPSTATICDATA	g_mapMeshData;
	MAPSTATICDATA	g_mapMeshAlpha;
	MAPSTATICDATA	g_mapMeshAlphaSOFT;
	MAPSTATICDATA	g_mapMeshAlphaSOFT01;
	MAPSTATICDATA	g_mapMeshAlphaSOFT02;

    DWORD	g_dwLightTexID = 0;

	// [shhan][2016.11.16] ���� ������ �� �ʿ䰡 ����.
	//BOOL	g_b100000ErrorCertify(FALSE);	// 100000 �Ѵ°� ����msg �ѹ��� ��������.

	void MakeStaticMesh3 ( DxMeshes* pMesh, 
						const TCHAR* szMaterialName,
						const TCHAR* szTextureName,
                        MAPSTATICDATA& mapMesh, 
                        const std::vector<VERTEX_WORLD>& vecVertices, 
                        DWORD* pIndices, 
                        DWORD dwFaceStart, 
                        DWORD dwFaceCount, 
                        D3DXMATRIX& matWorld, 
                        DWORD dwLightTexID,
                        BOOL bCamColl,
						std::tr1::shared_ptr<LIGHTMAPUV> spLightMapUV,
						const std::vector<WORDWORD>& vecTriangle_wwLightID )
	{
        if ( dwFaceCount == 0 )
		{
			TSTRING strError = szMaterialName;
			strError += _T(" - dwFaceCount == 0");
			AfxMessageBox ( strError.c_str() );
            return;
		}
		//else if ( dwFaceCount >= 100000 )
		//{
		//	if ( !g_b100000ErrorCertify )
		//	{
		//		g_b100000ErrorCertify = TRUE;
		//		TSTRING strError = szMaterialName;
		//		strError += _T(" - dwFaceCount >= 100000");
		//		AfxMessageBox ( strError.c_str() );
		//	}
		//	return;
		//}

        NAME_LMID sNameLMID(szMaterialName,dwLightTexID,szTextureName);
		MAPSTATICDATA_ITER iter = mapMesh.find( sNameLMID );
		if ( iter==mapMesh.end() )	// New
		{
			STATICDATA* pData   = new STATICDATA;

            if ( !pMesh->m_vecLightMapObject.empty() )
            {
                pMesh->m_vecLightMapObject[0]->GetLightMapName
                ( 
                    pData->m_strTextureCombined,
                    pData->m_strTextureDirect_Shadow_1,
					pData->m_strTextureDirect_Shadow_2,
                    pData->m_strTextureIndirect
                );
            }

			DWORD dwIndexID(0);
			VERTEX_WORLD sVertices;
			for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
			{
				if ( !vecTriangle_wwLightID.empty() )
				{
					pData->m_vecTriangle_wwLightID.push_back( vecTriangle_wwLightID[i] );
				}

				for ( DWORD j=0; j<3; ++j )
				{
					dwIndexID = (i*3)+j;
					sVertices = vecVertices[pIndices[dwIndexID]];
					D3DXVec3TransformCoord ( &sVertices.vPos, &sVertices.vPos, &matWorld );
					D3DXVec3TransformNormal ( &sVertices.vNor, &sVertices.vNor, &matWorld );
					pData->m_vecVertices.push_back( sVertices );

					if ( dwIndexID < spLightMapUV->m_vecUV.size() )
					{
						pData->m_vecLightMapUV.push_back( spLightMapUV->m_vecUV[dwIndexID] );
					}
					else
					{
						// LightMap �� �����Ѵٰ� �����ϸ� �ɰ� ����.
						pData->m_vecLightMapUV.clear();
					}
				}
			}

			mapMesh.insert ( std::make_pair(sNameLMID,pData) );
		}
		else								// Add
		{
			STATICDATA* pData = (STATICDATA*)(*iter).second;

			//if ( (pData->m_vecVertices.size() + dwFaceCount) >= 100000 )
			//{
			//	if ( !g_b100000ErrorCertify )
			//	{
			//		g_b100000ErrorCertify = TRUE;
			//		TSTRING strError = szMaterialName;
			//		strError += _T(" - dwFaceCount >= 100000");
			//		AfxMessageBox ( strError.c_str() );
			//	}
			//	return;
			//}

			BOOL bError(FALSE);
			DWORD dwIndexID(0);
			VERTEX_WORLD sVertices;
			for ( DWORD i=dwFaceStart; i<dwFaceStart+dwFaceCount; ++i )
			{
				if ( !vecTriangle_wwLightID.empty() )
				{
					pData->m_vecTriangle_wwLightID.push_back( vecTriangle_wwLightID[i] );
				}

				for ( DWORD j=0; j<3; ++j )
				{
					dwIndexID = (i*3)+j;
					sVertices = vecVertices[pIndices[dwIndexID]];
					D3DXVec3TransformCoord ( &sVertices.vPos, &sVertices.vPos, &matWorld );
					D3DXVec3TransformNormal ( &sVertices.vNor, &sVertices.vNor, &matWorld );
					pData->m_vecVertices.push_back( sVertices );

					// ������ ����Ʈ���� ���õǾ� ���� ��츸 �߰������� �۾��Ѵ�.
					if ( !pData->m_vecLightMapUV.empty() )
					{
						if ( dwIndexID < spLightMapUV->m_vecUV.size() )
						{
							pData->m_vecLightMapUV.push_back( spLightMapUV->m_vecUV[dwIndexID] );
						}
						else
						{
							// ������ �־����� ������ ���� ����
							bError = TRUE;

							// LightMap �� �����Ѵٰ� �����ϸ� �ɰ� ����.
							pData->m_vecLightMapUV.clear();
						}
					}
					else if ( !spLightMapUV->m_vecUV.empty())
					{
						// ������ �������� ������ �ִ� ����
						bError = TRUE;			
					}
				}
			}

			if ( bError )
			{
				TSTRING strError = szMaterialName;
				strError += _T(" - LightMap�� ��� �Ѱ��� ���Ѱ� 2������ ������Դϴ�. Material�� �и����ּ���.");
				AfxMessageBox ( strError.c_str() );
			}
		}
	}

	void MakeStaticMesh2( DxMeshes* pMesh, D3DXMATRIX& matWorld, MAP_LIGHTMAPUV* pmapLightMapUV, const DxTextureEffMan* pTextureEffMan )
	{
		DWORD dwLightTexID(0);
		if ( !pMesh->m_vecLightMapObject.empty() )
		{
			dwLightTexID = pMesh->m_vecLightMapObject[0]->GetTextureID();
		}

		VERTEX_WORLD*	pVertices;

		DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
		DWORD dwFaces	 = pMesh->m_pLocalMesh->GetNumFaces();

		DWORD*	pIndices = new DWORD[dwFaces*3];
		if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		{
			DWORD*	pIndex32;
			pMesh->m_pLocalMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndex32 );
			memcpy ( pIndices, pIndex32, sizeof(DWORD)*dwFaces*3 );
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}
		else
		{
			WORD*	pIndex16;
			pMesh->m_pLocalMesh->LockIndexBuffer ( D3DLOCK_READONLY, (VOID**)&pIndex16 );
			for ( DWORD i=0; i<dwFaces*3; ++i )
			{
				pIndices[i] = (DWORD)pIndex16[i];
			}
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		}


		pMesh->m_pLocalMesh->LockVertexBuffer ( D3DLOCK_READONLY, (VOID**)&pVertices );

		// ����Ʈ �� ������ ����Ǿ���.
		VERTEX_WORLD sVerticesDest;
		std::vector<VERTEX_WORLD>	vecVertices;
		for ( DWORD i=0; i<dwVertices; ++i )
		{
			vecVertices.push_back( pVertices[i] );
		}

		pMesh->m_pLocalMesh->UnlockVertexBuffer();

		// �׷��� �����̳ʰ� LightMap UV �� �� ���Ҵٸ� �� UV�� ����ϵ��� �Ѵ�.
		std::tr1::shared_ptr<LIGHTMAPUV> spLightMapUV;
		if ( pmapLightMapUV )
		{
			MAP_LIGHTMAPUV_CITER iter = pmapLightMapUV->find( pMesh->m_szName );
			if ( iter!=pmapLightMapUV->end() )
			{
				spLightMapUV = (*iter).second;
			}
			else
			{
				spLightMapUV = std::tr1::shared_ptr<LIGHTMAPUV>(new LIGHTMAPUV);
			}
		}
		else
		{
			spLightMapUV = std::tr1::shared_ptr<LIGHTMAPUV>(new LIGHTMAPUV);
		}


		//// LightMap UV�� �����Ѵ�.
		//for ( DWORD i=0; i<pMesh->m_vecLightMapObject.size(); ++i )
		//{
		//	pMesh->m_vecLightMapObject[i]->SetLightMapUV( vecVertices, pIndices );
		//}

		DWORD dwAttibTableSize;
		pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );

		LPD3DXATTRIBUTERANGE pAttribTable = NULL;
		pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
		pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );

		// Note : ī�޶� �浹 ����
		BOOL bCamColl = FALSE;

		for ( DWORD i=0; i<dwAttibTableSize; ++i )
		{
			DWORD dwMatIndex = pAttribTable[i].AttribId;

			TCHAR szTextureName[64] = _T("");
			StringCchCopy( szTextureName, 64, pMesh->m_strTextureFiles[dwMatIndex].GetString() );

			MakeStaticMesh3
			( 
				pMesh, 
				pMesh->m_vecMaterialName[dwMatIndex].c_str(), 
				szTextureName, 
				g_mapMeshData, 
				vecVertices,
				pIndices, 
				pAttribTable[i].FaceStart, 
				pAttribTable[i].FaceCount, 
				matWorld, 
				dwLightTexID, 
				bCamColl, 
				spLightMapUV, 
				pMesh->m_vecTriangle_wwLightID 
			);
		}
		

		SAFE_DELETE_ARRAY ( pAttribTable );
		SAFE_DELETE_ARRAY ( pIndices );
	}

	void MakeStaticMesh2_Ran_1_0( DxMeshes* pMesh, D3DXMATRIX& matWorld, MAP_LIGHTMAPUV* pmapLightMapUV, const DxTextureEffMan* pTextureEffMan )
	{
  //      DWORD dwLightTexID(0);
  //      if ( !pMesh->m_vecLightMapObject.empty() )
  //      {
  //          dwLightTexID = pMesh->m_vecLightMapObject[0]->GetTextureID();
  //      }

		//VERTEX_WORLD*	pVertices;

		//DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
		//DWORD dwFaces	 = pMesh->m_pLocalMesh->GetNumFaces();

		//DWORD*	pIndices = new DWORD[dwFaces*3];
		//if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		//{
		//	DWORD*	pIndex32;
		//	pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
		//	memcpy ( pIndices, pIndex32, sizeof(DWORD)*dwFaces*3 );
		//	pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		//}
		//else
		//{
		//	WORD*	pIndex16;
		//	pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
		//	for ( DWORD i=0; i<dwFaces*3; ++i )
		//	{
		//		pIndices[i] = (DWORD)pIndex16[i];
		//	}
		//	pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		//}


		//pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

  //      // ����Ʈ �� ������ ����Ǿ���.
  //      VERTEX_WORLD sVerticesDest;
  //      std::vector<VERTEX_WORLD>	vecVertices;
  //      for ( DWORD i=0; i<dwVertices; ++i )
  //      {
  //          vecVertices.push_back( pVertices[i] );
  //      }

		//// �׷��� �����̳ʰ� LightMap UV �� �� ���Ҵٸ� �� UV�� ����ϵ��� �Ѵ�.
		//std::tr1::shared_ptr<LIGHTMAPUV> spLightMapUV;
		//if ( pmapLightMapUV )
		//{
		//	
		//	MAP_LIGHTMAPUV_CITER iter = pmapLightMapUV->find( pMesh->m_szName );
		//	if ( iter!=pmapLightMapUV->end() )
		//	{
		//		spLightMapUV = (*iter).second;
		//	}
		//	else
		//	{
		//		spLightMapUV = std::tr1::shared_ptr<LIGHTMAPUV>(new LIGHTMAPUV);
		//	}
		//}
		//else
		//{
		//	spLightMapUV = std::tr1::shared_ptr<LIGHTMAPUV>(new LIGHTMAPUV);
		//}
		//

  //      // LightMap UV�� �����Ѵ�.
  //      for ( DWORD i=0; i<pMesh->m_vecLightMapObject.size(); ++i )
  //      {
  //          pMesh->m_vecLightMapObject[i]->SetLightMapUV( vecVertices, pIndices, 1 );
  //      }

		//DWORD dwAttibTableSize;
		//pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );

		//LPD3DXATTRIBUTERANGE pAttribTable = NULL;
		//pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
		//pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );


		//for ( DWORD i=0; i<dwAttibTableSize; ++i )
		//{
		//	char szName[64] = "";
		//	DWORD dwAttribID = pAttribTable[i].AttribId;
		//	StringCchCopy( szName, 64, pMesh->m_strTextureFiles[dwAttribID].GetString() );

		//	// Note : ī�޶� �浹 ����
		//	BOOL bCamColl = FALSE;
		//	
		//	if ( pMesh->m_exMaterials[dwAttribID].bCamColl )
		//	{
		//		bCamColl = pMesh->m_exMaterials[dwAttribID].bCamColl;
		//	}

		//	// Note : ���� �������� �ؽ��ĸ� ����.
		//	int nType = 0;
		//	if( CTextureSetDXT::GetInstance().FindTexture( szName, nType ) )
		//	{
		//		if( nType==0 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshData, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( nType==1 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlpha, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( nType==2 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlphaSOFT, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( nType==3 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlphaSOFT01, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( nType==4 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlphaSOFT02, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );

		//		continue;
		//	}

		//	if ( pTextureEffMan )
		//	{
		//		DWORD dwType = pTextureEffMan->GetTexType( szName );

		//		if( dwType==TEXEFF_OPAQUE )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshData, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( dwType==TEXEFF_HARDALPHA )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlpha, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( dwType==TEXEFF_SOFTALPHA )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlphaSOFT, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( dwType==TEXEFF_SOFTALPHA_1 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlphaSOFT01, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		else if( dwType==TEXEFF_SOFTALPHA_2 )
		//			MakeStaticMesh3( pMesh, szName, szName, g_mapMeshAlphaSOFT02, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );

		//		if( dwType!=TEXEFF_DEFAULT )
		//			continue;
		//	}

		//	// Note : �⺻ �۾�
		//	D3DSURFACE_DESC sDesc;
		//	sDesc.Format = D3DFMT_UNKNOWN;
		//	if ( pMesh->m_pTextureRes[dwAttribID].GetTexture() )
  //              pMesh->m_pTextureRes[dwAttribID].GetTexture()->GetLevelDesc( 0, &sDesc );	// �ؽ��İ� ���� �͵��� ���� �� �ִ�.

		//	std::string strName;
		//	std::string strNewName;
		//	std::string::size_type idx;
		//	switch ( sDesc.Format )
		//	{
		//	case D3DFMT_DXT1:
		//		strName = szName;
		//		std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
		//		idx = strName.find("_a.");
		//		if( idx!=std::string::npos )
		//		{
		//			// ���� ���
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlpha, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}
		//		idx = strName.find("_a1.");
		//		if( idx!=std::string::npos )
		//		{
		//			// ���� ���
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlpha, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}
		//		idx = strName.find("_a2.");
		//		if( idx!=std::string::npos )
		//		{
		//			// ���� ���
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlpha, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}

		//		// ���� �ƴϴ�
		//		MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshData, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		break;

		//	case D3DFMT_DXT5:
		//		strName = szName;
		//		std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );

		//		idx = strName.find("_a1.");		// 2��° �Ѹ���.
		//		if( idx!=std::string::npos )
		//		{
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlphaSOFT01, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}
		//		idx = strName.find("_a2.");		// 3��° �Ѹ���.
		//		if( idx!=std::string::npos )
		//		{
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlphaSOFT02, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}
		//		idx = strName.find("_v.");
		//		if( idx!=std::string::npos )	// ���� �ƴϴ�.
		//		{
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshData, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}
		//		
		//		// �Ϲ����̴�. Soft ~!
		//		MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlphaSOFT, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		break;

		//		// ������ ������ ���� ��
		//	case D3DFMT_DXT2:
		//	case D3DFMT_DXT3:
		//	case D3DFMT_DXT4:
		//	case D3DFMT_A8R8G8B8:	// ���� �ٽ� �ϱ� �ٶ�.	���� �ִ� ��
		//	case D3DFMT_A1R5G5B5:
		//	case D3DFMT_A4R4G4B4:
		//	case D3DFMT_A8R3G3B2:
		//	case D3DFMT_A8:
		//		idx = strName.find("_v.");
		//		if( idx!=std::string::npos )	// ���� �ƴϴ�.
		//		{
		//			MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshData, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//			break;
		//		}

		//		MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshAlpha, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		break;

		//		// ���� �ٽ� �ϱ� �ٶ�.	���� ���� ��
		//	case D3DFMT_X8R8G8B8:
		//	case D3DFMT_X1R5G5B5:
		//	case D3DFMT_X4R4G4B4:
		//	case D3DFMT_R8G8B8:
		//	case D3DFMT_R5G6B5:
		//	case D3DFMT_R3G3B2:
		//	default:
		//		MakeStaticMesh3 ( pMesh, szName, szName, g_mapMeshData, vecVertices, pIndices, pAttribTable[i].FaceStart, pAttribTable[i].FaceCount, matWorld, dwLightTexID, bCamColl, spLightMapUV );
		//		break;
		//	};
		//}
		//pMesh->m_pLocalMesh->UnlockVertexBuffer();

		//SAFE_DELETE_ARRAY ( pAttribTable );
		//SAFE_DELETE_ARRAY ( pIndices );
	}

	void MakeStaticMesh1 ( DxFrame *pframeCur, MAP_LIGHTMAPUV* pmapLightMapUV, const DxTextureEffMan* pTextureEffMan, BOOL bRan_1_0 )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}

		if ( bRan_1_0 )
		{
			DxMeshes* pMesh = pframeCur->pmsMeshs;
			while ( pMesh )
			{
				MakeStaticMesh2_Ran_1_0 ( pMesh, pframeCur->matCombined, pmapLightMapUV, pTextureEffMan );
				pMesh = pMesh->m_pMeshNext;
			}
		}
		else
		{
			if ( pmapLightMapUV )
			{
				// ����Ʈ�� ������ ���� Frame ���� ��츸 ���۵ȴ�.
				MAP_LIGHTMAPUV_ITER iter = pmapLightMapUV->find ( pframeCur->szName );
				if ( iter != pmapLightMapUV->end() )
				{
					DxMeshes* pMesh = pframeCur->pmsMeshs;
					while ( pMesh )
					{
						MakeStaticMesh2 ( pMesh, pframeCur->matCombined, pmapLightMapUV, pTextureEffMan );
						pMesh = pMesh->m_pMeshNext;
					}
				}
			}
			else
			{
				DxMeshes* pMesh = pframeCur->pmsMeshs;
				while ( pMesh )
				{
					MakeStaticMesh2 ( pMesh, pframeCur->matCombined, pmapLightMapUV, pTextureEffMan );
					pMesh = pMesh->m_pMeshNext;
				}
			}
		}

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeStaticMesh1 ( pframeChild, pmapLightMapUV, pTextureEffMan, bRan_1_0 );
			pframeChild = pframeChild->pframeSibling;
		}
	}

	// Note : ���⿡�� �ϳ����� DxSingleTexMesh���� �����Ѵ�.
	void MakeStaticMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
                        DxStaticMesh* pStaticMesh,
                        const BOOL bProgress, 
                        BOOL bPiece,
                        BOOL bLightMap, 
                        const TSTRING& strLightMapName,
                        float fLightMapDetail, 
						BOOL bUserLightMapUV,
						const CMaterialSetting* pMaterialSetting,
						BOOL bRan_1_0,
						float fCellSize )
	{
		if( bProgress )
		{
            if ( bLightMap )
            {
                ExportProgress::SetStateString( "LightMap ������ - StaticMesh" );
            }

			ExportProgress::SetStateString ( "Optimize Mesh�� ���� ��...3/3" );
			ExportProgress::CurPos= 0;
			ExportProgress::EndPos = int(g_mapMeshData.size() + g_mapMeshAlpha.size() + g_mapMeshAlphaSOFT.size() + g_mapMeshAlphaSOFT01.size() + g_mapMeshAlphaSOFT02.size());
		}


        MAPSTATICDATA* pmapMeshAlphaArray[5] = { &g_mapMeshData,
                                                &g_mapMeshAlpha,
                                                &g_mapMeshAlphaSOFT,
                                                &g_mapMeshAlphaSOFT01,
                                                &g_mapMeshAlphaSOFT02 };

        MAPSINGLETEXMESH* pmapSingleTexMeshArray[5] = { &pStaticMesh->m_mapMesh,
                                                        &pStaticMesh->m_mapMeshALPHA,
                                                        &pStaticMesh->m_mapMeshSOFTALPHA,
                                                        &pStaticMesh->m_mapMeshSOFTALPHA01,
                                                        &pStaticMesh->m_mapMeshSOFTALPHA02 };

        // LightMap�� ��� �۾��ؾ� �� �͵�.
        if ( bLightMap )
        {
            // DxSingleTexMesh �� ����.
            {
                //TSTRING strTextureDay(_T(""));
                //TSTRING strTextureNight(_T(""));
                //TSTRING strTextureAvgDay(_T(""));
                //TSTRING strTextureAvgNight(_T(""));

                MAPSTATICDATA_ITER iter;
                for ( DWORD i=0; i<5; ++i )
                {
                    for ( iter=pmapMeshAlphaArray[i]->begin(); iter!=pmapMeshAlphaArray[i]->end(); ++iter )
		            {
                        if( bProgress )
                        {
                            TSTRINGSTREAM strStringStream;
                            strStringStream << "DxSingleTexMesh - ";
                            strStringStream << ExportProgress::CurPos+1;
                            strStringStream << "/";
                            strStringStream << ExportProgress::EndPos;
                            ExportProgress::SetStateString( strStringStream.str().c_str() );
                            ExportProgress::CurPos++;
                        }

                        // Vertex ������
                        DWORD dwFaceStartNum(0);
                        VERTEXNORCOLORTEX5* pVertices = new VERTEXNORCOLORTEX5[ (*iter).second->m_vecVertices.size() ];
                        for ( DWORD j=0; j<(*iter).second->m_vecVertices.size(); ++j )
                        {
                            pVertices[j].vPos = (*iter).second->m_vecVertices[j].vPos;
							pVertices[j].vNor = (*iter).second->m_vecVertices[j].vNor;
							pVertices[j].dwColor = (*iter).second->m_vecVertices[j].dwColor;
							pVertices[j].vTex[0] = (*iter).second->m_vecVertices[j].vTex[0];
							pVertices[j].vTex[1] = (*iter).second->m_vecVertices[j].vTex[1];
							pVertices[j].vTex[2] = (*iter).second->m_vecVertices[j].vTex[2];
							pVertices[j].vTex[3] = (*iter).second->m_vecVertices[j].vTex[3];

							if ( j < (*iter).second->m_vecLightMapUV.size() )
							{
								pVertices[j].vTex[4] = (*iter).second->m_vecLightMapUV[j];
							}
							else
							{
								pVertices[j].vTex[4] = D3DXVECTOR2(0.f,0.f);
							}
                        }

						// ����������.
						float fCubeMap(0.1f);
						float fSpotSpecPower(0.f);
						float fSpotSpecMulti(128.f);
						DWORD dwFlags=ENSTM_CAMCOLL;
						if ( pMaterialSetting )
						{
							const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( (*iter).first.m_strMaterialName );
							if ( pSetting )
							{
								dwFlags = pSetting->GetFlag( (*iter).second->m_vecLightMapUV.empty() );

								// Note : CubeMap Value
								fCubeMap = pSetting->m_fCubeMapValue;

								// Note : Spot Specular Power
								fSpotSpecPower = pSetting->m_fSpotSpecPower;

								// Note : Spot Specular Multi
								fSpotSpecMulti = pSetting->m_fSpotSpecMulti;
							}
						}

						{
                            DxSingleTexMesh* pNew = new DxSingleTexMesh;

                            // DxSingleTexMesh - LightMap ������ �۾��ϸ� �� �� ����.
                            {   
		                        int nResult = pNew->CreateLightMapForTriangleLinkLight
                                (
                                    pd3dDevice, 
									(*iter).first.m_strMaterialName.c_str(), 
									(*iter).first.m_strTextureName,
                                    static_cast<DWORD>( (*iter).second->m_vecVertices.size()/3 ),
                                    reinterpret_cast<BYTE*>( pVertices ), // �۾�����.
                                    dwFlags,
									MaterialValue(fCubeMap,fSpotSpecPower,fSpotSpecMulti),
                                    VERTEXNORCOLORTEX5::FVF,
                                    (*iter).second->m_strTextureCombined,
                                    (*iter).second->m_strTextureDirect_Shadow_1,
									(*iter).second->m_strTextureDirect_Shadow_2,
                                    (*iter).second->m_strTextureIndirect,
									bUserLightMapUV && !(*iter).second->m_vecLightMapUV.empty(),
									(*iter).second->m_vecTriangle_wwLightID,
									fCellSize
                                );

								if ( nResult == 1 )
								{
									pmapSingleTexMeshArray[i]->insert ( std::make_pair ( (*iter).first, pNew ) );
								}
								else if ( nResult == 0 )
								{
									TSTRING strError = (*iter).first.m_strMaterialName;
									strError += _T(" - ������ ������ �ƴմϴ�.");
									AfxMessageBox ( strError.c_str() );

									SAFE_DELETE ( pNew );
								}
								else if ( nResult == -1 )
								{
									TSTRING strError = (*iter).first.m_strMaterialName;
									strError += _T(" - ���������� �ʹ� ���Ƽ� new �� �Ұ��� �մϴ�.");
									AfxMessageBox ( strError.c_str() );

									SAFE_DELETE ( pNew );
								}
								else
								{
									TSTRING strError = (*iter).first.m_strMaterialName;
									strError += _T(" - �˼����� Error �� ������ϴ�.");
									AfxMessageBox ( strError.c_str() );

									SAFE_DELETE ( pNew );
								}
							}
						}

                        // ����� ������ ����
                        (*iter).second->m_vecLightMapObject.clear();
                        SAFE_DELETE_ARRAY( pVertices );

			            if( bProgress )	ExportProgress::CurPos++;
		            }
                }

                //// ������ �����۾�.
                //NSLightMap::CreateLightMapFinal( pd3dDevice, strLightMapName, _T("_S") );

                //// �ؽ��� ����.
                //TextureManager::GetInstance().RescanPath();

                //// SingleTexMesh ���� LightMap Texture �ε� ��.
                //{
                //    MAPSINGLETEXMESH_ITER iter;
                //    for ( DWORD i=0; i<5; ++i )
                //    {
                //        for ( iter=pmapSingleTexMeshArray[i]->begin(); iter!=pmapSingleTexMeshArray[i]->end(); ++iter )
		              //  {
                //            (*iter).second->LoadLightMap( pd3dDevice );
                //        }
                //    }
                //}
            }
        }
        else
        {
			
            MAPSTATICDATA_ITER iter;
            for ( DWORD i=0; i<5; ++i )
            {
                for ( iter=pmapMeshAlphaArray[i]->begin(); iter!=pmapMeshAlphaArray[i]->end(); ++iter )
		        {
					VERTEXNORCOLORTEX5* pVertices = new VERTEXNORCOLORTEX5[(*iter).second->m_vecVertices.size()];
					for ( DWORD z=0; z<(*iter).second->m_vecVertices.size(); ++z )
					{
						pVertices[z].vPos = (*iter).second->m_vecVertices[z].vPos;
						pVertices[z].vNor = (*iter).second->m_vecVertices[z].vNor;
						pVertices[z].dwColor = (*iter).second->m_vecVertices[z].dwColor;
						pVertices[z].vTex[0] = (*iter).second->m_vecVertices[z].vTex[0];
						pVertices[z].vTex[1] = (*iter).second->m_vecVertices[z].vTex[1];
						pVertices[z].vTex[2] = (*iter).second->m_vecVertices[z].vTex[2];
						pVertices[z].vTex[3] = (*iter).second->m_vecVertices[z].vTex[3];

						if ( z < (*iter).second->m_vecLightMapUV.size() )
						{
							pVertices[z].vTex[4] = (*iter).second->m_vecLightMapUV[z];
						}
						else
						{
							pVertices[z].vTex[4] = D3DXVECTOR2(0.f,0.f);
						}
					}

					//// LightMap �׸��ڸ� �����ϴ� ��ü���� ���ܽ�Ű�� TRUE
					//BOOL bDontCastShadowLM = FALSE;
					//BOOL bDontReceiveShadowLM = FALSE;
					//if ( pTextureEffMan )
					//{
					//	const TextureData* pTextureData = pTextureEffMan->GetTextureData( (*iter).first.m_strMaterialName.c_str() );
					//	if ( pTextureData )
					//	{
					//		if ( pTextureData->m_dwPieceLM_Option & EMPLO_DONT_LM_SHADOW_CAST )
					//		{
					//			bDontCastShadowLM = TRUE;
					//		}
					//		else
					//		{
					//			bDontCastShadowLM = FALSE;
					//		}

					//		if ( pTextureData->m_dwPieceLM_Option & EMPLO_DONT_LM_SHADOW_RECEIVE )
					//		{
					//			bDontReceiveShadowLM = TRUE;
					//		}
					//		else
					//		{
					//			bDontReceiveShadowLM = FALSE;
					//		}
					//	}
					//}

					// ����������.
					float fCubeMap(0.1f);
					float fSpotSpecPower(0.f);
					float fSpotSpecMulti(128.f);
					DWORD dwFlags=ENSTM_CAMCOLL;
					if ( pMaterialSetting )
					{
						const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( (*iter).first.m_strMaterialName );
						if ( pSetting )
						{
							dwFlags = pSetting->GetFlag( (*iter).second->m_vecLightMapUV.empty() );

							// Note : CubeMap Value
							fCubeMap = pSetting->m_fCubeMapValue;

							// Note : Spot Specular Power
							fSpotSpecPower = pSetting->m_fSpotSpecPower;

							// Note : Spot Specular Multi
							fSpotSpecMulti = pSetting->m_fSpotSpecMulti;
						}
					}


					//BOOL bCameraCollision = TRUE;
					//BOOL bDontCastShadowLM = FALSE;
					//BOOL bDontReceiveShadowLM = FALSE;
					//if ( pMaterialSetting )
					//{
					//	const NSMaterialSetting::SETTING* pSetting = pMaterialSetting->GetMaterialSetting( (*iter).first.m_strMaterialName );
					//	if ( pSetting )
					//	{
					//		bDontCastShadowLM = !pSetting->m_bCastShadow;
					//		bCameraCollision = pSetting->m_bCameraCollision;
					//	}
					//}

					DxSingleTexMesh* pNew = new DxSingleTexMesh;
					int nResult = pNew->Create
					( 
						pd3dDevice, 
						(*iter).first.m_strMaterialName.c_str(), 
						(*iter).first.m_strTextureName.c_str(), 
						static_cast<DWORD>( (*iter).second->m_vecVertices.size()/3 ),
						reinterpret_cast<BYTE*>( pVertices ),
						VERTEXNORCOLORTEX5::FVF, 
						bPiece, 
						dwFlags,
						MaterialValue(fCubeMap,fSpotSpecPower,fSpotSpecMulti),
						bUserLightMapUV,
						fCellSize 
					);

					if ( nResult == 1 )
					{
						pmapSingleTexMeshArray[i]->insert ( std::make_pair ( (*iter).first, pNew ) );
					}
					else if ( nResult == 0 )
					{
						TSTRING strError = (*iter).first.m_strMaterialName;
						strError += _T(" - ������ ������ �ƴմϴ�.");
						AfxMessageBox ( strError.c_str() );

						SAFE_DELETE ( pNew );
					}
					else if ( nResult == -1 )
					{
						TSTRING strError = (*iter).first.m_strMaterialName;
						strError += _T(" - ���������� �ʹ� ���Ƽ� new �� �Ұ��� �մϴ�.");
						AfxMessageBox ( strError.c_str() );

						SAFE_DELETE ( pNew );
					}
					else
					{
						TSTRING strError = (*iter).first.m_strMaterialName;
						strError += _T(" - �˼����� Error �� ������ϴ�.");
						AfxMessageBox ( strError.c_str() );

						SAFE_DELETE ( pNew );
					}

					SAFE_DELETE_ARRAY( pVertices );

					if( bProgress )	
						ExportProgress::CurPos++;
		        }
            }
        }
	}

	
	void FindNodeNUM ( DxFrame *pframeCur )
	{
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}
		else															ExportProgress::EndPos++;

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			FindNodeNUM ( pframeChild );
			pframeChild = pframeChild->pframeSibling;
		}
	}

    void ClearMeshData()
    {
        std::for_each ( g_mapMeshData.begin(), g_mapMeshData.end(), std_afunc::DeleteMapObject() );
		g_mapMeshData.clear();
		std::for_each ( g_mapMeshAlpha.begin(), g_mapMeshAlpha.end(), std_afunc::DeleteMapObject() );
		g_mapMeshAlpha.clear();
		std::for_each ( g_mapMeshAlphaSOFT.begin(), g_mapMeshAlphaSOFT.end(), std_afunc::DeleteMapObject() );
		g_mapMeshAlphaSOFT.clear();
		std::for_each ( g_mapMeshAlphaSOFT01.begin(), g_mapMeshAlphaSOFT01.end(), std_afunc::DeleteMapObject() );
		g_mapMeshAlphaSOFT01.clear();
		std::for_each ( g_mapMeshAlphaSOFT02.begin(), g_mapMeshAlphaSOFT02.end(), std_afunc::DeleteMapObject() );
		g_mapMeshAlphaSOFT02.clear();
    }

 //   void MakeLightMapMesh_2 ( LPDIRECT3DDEVICEQ pd3dDevice, 
 //                           DxMeshes* pMesh, 
 //                           D3DXMATRIX& matWorld, 
 //                           DxStaticMesh* pStaticMeshColl,
 //                           DxPieceManager* pPieceManager, 
 //                           const TSTRING& strLightMapName, 
 //                           float fLightMapDetail,
 //                           NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType )
	//{
	//	DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
	//	DWORD dwFaces	 = pMesh->m_pLocalMesh->GetNumFaces();

 //       // Index Buffer �� ����.
	//	VEC_DWORD vecIndex;
	//	if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
	//	{
 //           DWORD*	pIndex32;
	//		pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
 //           for ( DWORD i=0; i<dwFaces*3; ++i )
 //           {
 //               vecIndex.push_back( pIndex32[i] );
 //           }
 //           pMesh->m_pLocalMesh->UnlockIndexBuffer ();
	//		
	//	}
	//	else
	//	{
	//		WORD*	pIndex16;
	//		pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
	//		for ( DWORD i=0; i<dwFaces*3; ++i )
	//		{
 //               vecIndex.push_back( (DWORD)pIndex16[i] );
	//		}
	//	}

 //       // Vertex Buffer �� ����.
 //       VERTEX*	pVertices;
 //       VERTEX* pVerticesNEW = new VERTEX[dwFaces*3];
	//	pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
 //       for ( DWORD i=0; i<dwFaces*3; ++i )
 //       {
 //           D3DXVec3TransformCoord( &pVerticesNEW[i].vPos, &pVertices[ vecIndex[i] ].vPos, &matWorld );
 //           D3DXVec3TransformNormal( &pVerticesNEW[i].vNor, &pVertices[ vecIndex[i] ].vNor, &matWorld );
 //           pVerticesNEW[i].vTex = pVertices[ vecIndex[i] ].vTex;
 //       }
	//	pMesh->m_pLocalMesh->UnlockVertexBuffer();

 //       D3DXMATRIX matIdentity;
 //       D3DXMatrixIdentity( &matIdentity );

 //       // LightMap�� ��������.
 //       DWORD dwStartTextureSize(512);
 //       NSLightMap::CreateLightMap( pd3dDevice,
 //                                   strLightMapName, 
 //                                   _T("_S"), 
 //                                   FALSE,
 //                                   FALSE,
 //                                   pMesh->m_vecLightMapObject,
 //                                   dwFaces, 
 //                                   reinterpret_cast<BYTE*>(pVerticesNEW),
 //                                   VERTEX::FVF,
 //                                   pStaticMeshColl,
 //                                   pPieceManager,
 //                                   matIdentity,
 //                                   dwStartTextureSize,
 //                                   fLightMapDetail,
 //                                   emLightMapType,
 //                                   FALSE );

 //       SAFE_DELETE_ARRAY ( pVerticesNEW );
 //   }

	void MakeLightMapMeshUV ( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxMeshes* pMesh, 
								D3DXMATRIX& matWorld, 
								DxStaticMesh* pStaticMeshColl,
								DxPieceManagerPSF* pPieceManager, 
								const TSTRING& strLightMapName, 
								float fLightMapDetail,
								DWORD dwTextureID,
								WORDWORD wwLightMapUserUV_TEXEL, 
								LIGHTMAPUV* pLightMapUV )
	{
		DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
		DWORD dwFaces	 = pMesh->m_pLocalMesh->GetNumFaces();

		// 
		std::vector<BOOL>			vecReceiveShadowLM;
		for ( DWORD i=0; i<dwFaces; ++i )
		{
			vecReceiveShadowLM.push_back( FALSE );
		}

		// Index Buffer �� ����.
		VEC_DWORD vecIndex;
		if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		{
			DWORD*	pIndex32;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
			for ( DWORD i=0; i<dwFaces*3; ++i )
			{
				vecIndex.push_back( pIndex32[i] );
			}
			pMesh->m_pLocalMesh->UnlockIndexBuffer ();

		}
		else
		{
			WORD*	pIndex16;
			pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
			for ( DWORD i=0; i<dwFaces*3; ++i )
			{
				vecIndex.push_back( (DWORD)pIndex16[i] );
			}
		}

		// Vertex Buffer �� ����.
		VERTEX_WORLD* pVertices;
		VERTEX_WORLD* pVerticesNEW = new VERTEX_WORLD[dwFaces*3];
		pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
		for ( DWORD i=0; i<dwFaces*3; ++i )
		{
			pVerticesNEW[i] = pVertices[ vecIndex[i] ];

			D3DXVec3TransformCoord( &pVerticesNEW[i].vPos, &pVerticesNEW[i].vPos, &matWorld );
			D3DXVec3TransformNormal( &pVerticesNEW[i].vNor, &pVerticesNEW[i].vNor, &matWorld );
		}
		pMesh->m_pLocalMesh->UnlockVertexBuffer();

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );

		// LightMap�� ��������.
		NSLightMap::CreateFieldLightMapUserUV
		(
			pd3dDevice,
			strLightMapName, 
			_T("_S"), 
			FALSE,
			pMesh->m_vecLightMapObject,
			pMesh->m_vecTriangle_wwLightID,
			dwFaces, 
			reinterpret_cast<BYTE*>(pVerticesNEW),
			VERTEX_WORLD::FVF,
			pStaticMeshColl,
			pPieceManager,
			matIdentity,
			vecReceiveShadowLM,
			dwTextureID,
			wwLightMapUserUV_TEXEL,
			pLightMapUV
		);

		SAFE_DELETE_ARRAY ( pVerticesNEW );
	}

    void MakeLightMapMesh ( LPDIRECT3DDEVICEQ pd3dDevice, 
                            DxFrame *pframeCur, 
                            DxStaticMesh* pStaticMeshColl,
                            DxPieceManagerPSF* pPieceManager, 
							LIGHTMAPUV_MAIN& sLightMapUV_MAIN,
                            const TSTRING& strLightMapName, 
                            float fLightMapDetail,
                            BOOL bProgress )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}

		// ����Ʈ�� ������ ���� Frame ���� ��츸 ���۵ȴ�.
		MAP_LIGHTMAPUV_ITER iter = sLightMapUV_MAIN.m_mapLightMapUV.find ( pframeCur->szName );
		if ( iter != sLightMapUV_MAIN.m_mapLightMapUV.end() )
		{
			DxMeshes* pMesh = pframeCur->pmsMeshs;
			while ( pMesh )
			{
				MakeLightMapMeshUV
				( 
					pd3dDevice, 
					pMesh, 
					pframeCur->matCombined, 
					pStaticMeshColl, 
					pPieceManager, 
					strLightMapName, 
					fLightMapDetail * pMesh->m_fLightMapDetail, 
					sLightMapUV_MAIN.m_dwTextureID,
					WORDWORD( static_cast<WORD>(sLightMapUV_MAIN.m_dwTextureWidth), static_cast<WORD>(sLightMapUV_MAIN.m_dwTextureHeight) ),	// ��ü �ؽ��� ũ��
					(*iter).second.get()
				);
				
				pMesh = pMesh->m_pMeshNext;
			}

			// ���
			if( bProgress )
			{
				TSTRINGSTREAM strStringStream;
				strStringStream << "CreateLightMap - ";
				strStringStream << ExportProgress::CurPos+1;
				strStringStream << "/";
				strStringStream << ExportProgress::EndPos;
				ExportProgress::SetStateString( strStringStream.str().c_str() );
				ExportProgress::CurPos++;
			}
		}

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeLightMapMesh ( pd3dDevice, pframeChild, pStaticMeshColl, pPieceManager, sLightMapUV_MAIN, strLightMapName, fLightMapDetail, bProgress );
			pframeChild = pframeChild->pframeSibling;
		}
	}

    void MakeLightMapMeshNext_2 ( LPDIRECT3DDEVICEQ pd3dDevice, 
                                    DxMeshes* pMesh, 
                                    const TSTRING& strLightMapName )
	{
        TSTRING strTextureCombined;
        TSTRING strTextureDirect_Shadow_1;
		TSTRING strTextureDirect_Shadow_2;
        TSTRING strTextureIndirect;

        // ������ �����۾�.
        NSLightMap::CreateLightMap2_STATICMESH( pMesh->m_vecLightMapObject,
												strLightMapName, 
												_T("_S"), 
												FALSE,
												FALSE,
												strTextureCombined, 
												strTextureDirect_Shadow_1,
												strTextureDirect_Shadow_2,
												strTextureIndirect,
												WORDWORD(0,0) );
    }

    void MakeLightMapMeshNext( LPDIRECT3DDEVICEQ pd3dDevice, 
                            DxFrame *pframeCur, 
							MAP_LIGHTMAPUV& mapLightMapUV, 
                            const TSTRING& strLightMapName,
							BOOL bProgress )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}

		// ����Ʈ�� ������ ���� Frame ���� ��츸 ���۵ȴ�.
		if ( mapLightMapUV.end() != mapLightMapUV.find ( pframeCur->szName ) )
		{
			
			DxMeshes* pMesh = pframeCur->pmsMeshs;
			while ( pMesh )
			{
				MakeLightMapMeshNext_2 ( pd3dDevice, pMesh, strLightMapName );
				pMesh = pMesh->m_pMeshNext;
			}

			// ���
			if( bProgress )
			{
				TSTRINGSTREAM strStringStream;
				strStringStream << "CreateLightMap2 - ";
				strStringStream << ExportProgress::CurPos+1;
				strStringStream << "/";
				strStringStream << ExportProgress::EndPos;
				ExportProgress::SetStateString( strStringStream.str().c_str() );
				ExportProgress::CurPos++;
			}
		}

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeLightMapMeshNext ( pd3dDevice, pframeChild, mapLightMapUV, strLightMapName, bProgress );
			pframeChild = pframeChild->pframeSibling;
		}
	}

    void SetExportSize( DxFrame *pframeCur )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}

		++ExportProgress::EndPos;

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			SetExportSize( pframeChild );
			pframeChild = pframeChild->pframeSibling;
		}
	}


    //--------------------------------------------------------------------------------------------------------------------------------
    // Texture���� VM - 2Side �� ���̰� �ִ��� ����, �ִٸ� ������ �߰������� ������ش�.
    //--------------------------------------------------------------------------------------------------------------------------------
    void MakeVM2SideMesh_2 ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMesh, DxTextureEffMan* pTextureEffMan )
    {
        DWORD dwBaseAttibTableSize(0);
        DWORD dwAttibTableSize;
		pMesh->m_pLocalMesh->GetAttributeTable ( NULL, &dwAttibTableSize );

		LPD3DXATTRIBUTERANGE pAttribTable = NULL;
		pAttribTable = new D3DXATTRIBUTERANGE [ dwAttibTableSize ];
		pMesh->m_pLocalMesh->GetAttributeTable ( pAttribTable, &dwAttibTableSize );

        dwBaseAttibTableSize = dwAttibTableSize;
        for ( DWORD dwAtt=0; dwAtt <dwBaseAttibTableSize; ++dwAtt )
		{
			char szName[64] = "";
			DWORD dwMaterial = pAttribTable[ dwAtt ].AttribId;
			StringCchCopy( szName, 64, pMesh->m_strTextureFiles[dwMaterial].GetString() );

			//if ( pTextureEffMan->IsVM2Side( szName ) || NSMaterialManager::Is2SideMesh( pMesh->m_vecMaterialName[dwMaterial] ) )
            {
                // ���������� ���
		        DWORD dwVertices = pMesh->m_pLocalMesh->GetNumVertices();
		        DWORD dwFaces	 = pMesh->m_pLocalMesh->GetNumFaces();

		        DWORD*	pIndices = new DWORD[dwFaces*3];
		        if ( pMesh->m_pLocalMesh->GetOptions()&D3DXMESH_32BIT )
		        {
			        DWORD*	pIndex32;
			        pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
			        memcpy ( pIndices, pIndex32, sizeof(DWORD)*dwFaces*3 );
			        pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		        }
		        else
		        {
			        WORD*	pIndex16;
			        pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
			        for ( DWORD i=0; i<dwFaces*3; ++i )
			        {
				        pIndices[i] = (DWORD)pIndex16[i];
			        }
			        pMesh->m_pLocalMesh->UnlockIndexBuffer ();
		        }

                VERTEX_WORLD*	pVertices;
		        pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

                VERTEX_WORLD sVerticesDest;
                std::vector<VERTEX_WORLD>	vecVertices;
                for ( DWORD z=0; z<dwVertices; ++z )
                {
                    sVerticesDest = pVertices[z];
                    vecVertices.push_back( sVerticesDest );
                }

                pMesh->m_pLocalMesh->UnlockVertexBuffer ();


                // ���Ӱ� �����͸� �߰� �� ����
                {
                    DWORD dwFaceNUM = dwFaces + pAttribTable[ dwAtt ].FaceCount;
                    DWORD dwVertNUM = dwVertices + (pAttribTable[ dwAtt ].FaceCount*3);

                    DWORD dwFlags = 0L;
                    if ( dwVertNUM >= 65535 )
                        dwFlags = D3DXMESH_32BIT;

                    SAFE_RELEASE( pMesh->m_pLocalMesh );
                    D3DXCreateMeshFVF ( dwFaceNUM, dwVertNUM, dwFlags | D3DXMESH_MANAGED, VERTEX_WORLD::FVF, pd3dDevice, &pMesh->m_pLocalMesh );

                    // Vertex ���
                    {
                        VERTEX_WORLD*	pVertices;
		                pMesh->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

                        DWORD dwCount=0;
                        for ( ; dwCount<vecVertices.size(); ++dwCount )
                        {
                            pVertices[dwCount] = vecVertices[dwCount];
                        }

                        for ( DWORD z = pAttribTable[ dwAtt ].FaceStart; z < pAttribTable[ dwAtt ].FaceStart+pAttribTable[ dwAtt ].FaceCount; ++z )
                        {
                            for ( DWORD j=0; j<3; ++j )
                            {
                                sVerticesDest = vecVertices[pIndices[(z*3)+j]];
                                sVerticesDest.vNor = -sVerticesDest.vNor;
                                pVertices[dwCount++] = sVerticesDest;
                            }
                        }

                        pMesh->m_pLocalMesh->UnlockVertexBuffer ();
                    }

                    // Index ���
                    {
                        DWORD dwCount=0;
                        if ( dwFlags & D3DXMESH_32BIT )
                        {
                            DWORD*	pIndex32;
			                pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex32 );
			                memcpy ( pIndex32, pIndices, sizeof(DWORD)*dwFaces*3 );

                            dwCount = dwFaces*3;
                            for ( DWORD i=0; i < pAttribTable[ dwAtt ].FaceCount; ++i )
			                {
				                pIndex32[dwCount++] = dwVertices + i*3 + 0;
                                pIndex32[dwCount++] = dwVertices + i*3 + 2;
                                pIndex32[dwCount++] = dwVertices + i*3 + 1;
			                }

			                pMesh->m_pLocalMesh->UnlockIndexBuffer ();
                        }
                        else
                        {
                            WORD*	pIndex16;
			                pMesh->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndex16 );
			                for ( DWORD i=0; i<dwFaces*3; ++i )
			                {
				                pIndex16[i] = static_cast<WORD>( pIndices[i] );
			                }

                            dwCount = dwFaces*3;
                            for ( DWORD i=0; i < pAttribTable[ dwAtt ].FaceCount; ++i )
			                {
				                pIndex16[dwCount++] = static_cast<WORD>( dwVertices + i*3 + 0 );
                                pIndex16[dwCount++] = static_cast<WORD>( dwVertices + i*3 + 2 );
                                pIndex16[dwCount++] = static_cast<WORD>( dwVertices + i*3 + 1 );
			                }

			                pMesh->m_pLocalMesh->UnlockIndexBuffer ();
                        }
                    }
                    
                    // Att ���
                    {
                        DWORD dwAttibTableSizeNEW = dwAttibTableSize+1;

		                D3DXATTRIBUTERANGE* pAttribTableNEW = new D3DXATTRIBUTERANGE [ dwAttibTableSizeNEW ];
                        memcpy( pAttribTableNEW, pAttribTable, sizeof(D3DXATTRIBUTERANGE)*dwAttibTableSize );

                        pAttribTableNEW[ dwAttibTableSize ].AttribId = pAttribTable[ dwAtt ].AttribId;
                        pAttribTableNEW[ dwAttibTableSize ].FaceStart = dwFaces;
                        pAttribTableNEW[ dwAttibTableSize ].FaceCount = pAttribTable[ dwAtt ].FaceCount;
                        pAttribTableNEW[ dwAttibTableSize ].VertexStart = dwVertices;
                        pAttribTableNEW[ dwAttibTableSize ].VertexCount = pAttribTable[ dwAtt ].FaceCount*3;

                        pMesh->m_pLocalMesh->SetAttributeTable( pAttribTableNEW, dwAttibTableSizeNEW );

                        SAFE_DELETE_ARRAY( pAttribTable );

                        dwAttibTableSize = dwAttibTableSizeNEW;
                        pAttribTable = pAttribTableNEW;

                        dwAttibTableSizeNEW = 0;
                        pAttribTableNEW = NULL;
                    }
                }

                // ����
                SAFE_DELETE_ARRAY( pIndices );
            }
        }

        //pAttribTable[i].FaceStart, pAttribTable[i].FaceCount

        SAFE_DELETE( pAttribTable );

    }

    // Texture���� VM - 2Side �� ���̰� �ִ��� ����, �ִٸ� ������ �߰������� ������ش�.
    void MakeVM2SideMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, DxTextureEffMan* pTextureEffMan )
    {
        if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}

		DxMeshes* pMesh = pframeCur->pmsMeshs;
		while ( pMesh )
		{
			MakeVM2SideMesh_2 ( pd3dDevice, pMesh, pTextureEffMan );
			pMesh = pMesh->m_pMeshNext;
		}

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			MakeVM2SideMesh ( pd3dDevice, pframeChild, pTextureEffMan );
			pframeChild = pframeChild->pframeSibling;
		}
    }
    //--------------------------------------------------------------------------------------------------------------------------------


	void ExportMaterialList ( LPDIRECT3DDEVICEQ pd3dDevice, 
                                DxFrameMesh* const pFrameMesh, 
                                DxStaticMesh* pStaticMesh, 
								LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
                                BOOL bProgress, 
                                BOOL bPiece, 
                                BOOL bLightMap, 
                                DxStaticMesh* pStaticMeshColl,
                                DxPieceManagerPSF* pPieceManager, 
                                const TSTRING& strLightMapName,
                                float fLightMapDetail, 
								const DxTextureEffMan* pTextureEffMan, 
								const CMaterialSetting* pMaterialSetting,
								BOOL bRan_1_0,
								float fCellSize )
	{
		DxFrame* pFrameRoot = pFrameMesh->GetFrameRoot();
		if ( pFrameRoot )
		{
			ClearMeshData();

			if( bProgress )
			{
				ExportProgress::SetStateString ( "Optimize Mesh�� ���� ��...1/3" );
				ExportProgress::CurPos= 0;
				ExportProgress::EndPos = 0;

				FindNodeNUM( pFrameRoot );
			}

            if ( bLightMap && pLightMapUV_MAIN )
            {
				// �̰��� DxLandMan�� DxStaticMesh�� ���´�.

                // ExportProgress::EndPos �� �� ������ ��´�.
                SetExportSize( pFrameRoot );

                // Texture���� VM - 2Side �� ���̰� �ִ��� ����, �ִٸ� ������ �߰������� ������ش�.
                //MakeVM2SideMesh ( pd3dDevice, pFrameRoot, pFrameMesh->GetTextureEffMan() );

                // LightMap�� ��������. UV �� ���⿡�� �����Ǿ�����.
                MakeLightMapMesh ( pd3dDevice, pFrameRoot, pStaticMeshColl, pPieceManager, *pLightMapUV_MAIN, strLightMapName, fLightMapDetail, bProgress );

                {
                    // LightMap Texture�� Resize �Ѵ�
                    NSLightMap::ResizeLightMapTexture();

                    //// �ؽ��� ����.
                    //TextureManager::GetInstance().RescanPath();
                }

                // ���� ���� Color�� �ϳ��� �迭�� �ִ� �۾�.
                MakeLightMapMeshNext ( pd3dDevice, pFrameRoot, pLightMapUV_MAIN->m_mapLightMapUV, strLightMapName, bProgress );

                // Mesh ���� �۾�.
				MakeStaticMesh1( pFrameRoot, &pLightMapUV_MAIN->m_mapLightMapUV, pTextureEffMan, bRan_1_0 );

                if( bProgress )
			    {
				    ExportProgress::SetStateString ( "Optimize Mesh�� ���� ��...2/3" );
			    }

				BOOL bUserLightMapUV(FALSE);
				if ( pLightMapUV_MAIN )
				{
					bUserLightMapUV = TRUE;
				}

				// DxSingleTexMesh �� ����� �۾�.
				MakeStaticMesh
				( 
					pd3dDevice, 
					pStaticMesh, 
					bProgress, 
					bPiece,
					bLightMap, 
					strLightMapName, 
					fLightMapDetail, 
					bUserLightMapUV, 
					pMaterialSetting,
					bRan_1_0,
					fCellSize
				);
            }
            else
            {
				// Mesh ���� �۾�.
				if( pLightMapUV_MAIN )
				{
					MakeStaticMesh1( pFrameRoot, &pLightMapUV_MAIN->m_mapLightMapUV, pTextureEffMan, bRan_1_0 );
				}
				else
				{
					MakeStaticMesh1( pFrameRoot, NULL, pTextureEffMan, bRan_1_0 );
				}

                if( bProgress )
			    {
				    ExportProgress::SetStateString ( "Optimize Mesh�� ���� ��...2/3" );
			    }

				BOOL bUserLightMapUV(FALSE);
				if ( pLightMapUV_MAIN )
				{
					bUserLightMapUV = TRUE;
				}

				// DxSingleTexMesh �� ����� �۾�.
				MakeStaticMesh
				( 
					pd3dDevice,
					pStaticMesh, 
					bProgress, 
					bPiece,
					bLightMap, 
					strLightMapName,
					fLightMapDetail,
					bUserLightMapUV, 
					pMaterialSetting, 
					bRan_1_0,
					fCellSize
				);
            }

			ClearMeshData();
		}
	}

	void ExportMaterialList ( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxMeshes *pmsMeshs,
								DxStaticMesh *pStaticMesh, 
								const CMaterialSetting *pMaterialSetting,
								float fCellSize )
	{
		if ( !pmsMeshs )
			return;

		ClearMeshData();

		// Mesh ���� �۾�.
		DxMeshes* pMesh = pmsMeshs;
		while ( pMesh )
		{
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			MakeStaticMesh2 ( pMesh, matIdentity, NULL, NULL );
			pMesh = pMesh->m_pMeshNext;
		}

		// DxSingleTexMesh �� ����� �۾�.
		MakeStaticMesh
		( 
			pd3dDevice, 
			pStaticMesh, 
			FALSE, 
			TRUE,
			FALSE, 
			_T(""), 
			0.f, 
			FALSE, 
			pMaterialSetting,
			FALSE,
			fCellSize
		);

		ClearMeshData();
	}

	void ConvertMAPtoLIST ( MAPSINGLETEXMESH& mapData, PDXSINGLETEXMESH& pList )
	{
		MAPSINGLETEXMESH_ITER iter = mapData.begin();
		for ( ; iter!=mapData.end(); ++iter )
		{
			(*iter).second->m_pNext = pList;
			pList = (*iter).second;
		}
	}

	void ConvertTexEffMesh( const LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff, MAPSINGLETEXMESH& mapData )
	{
		if( !pTexEff )			return;

		MAPSINGLETEXMESH_ITER iter = mapData.begin();
		for( ; iter!=mapData.end(); ++iter )
		{
			(*iter).second->ConvertTexEffMesh( pd3dDevice, pTexEff );
		}
	}

    //-------------------------------------------------------------------------------------------------------------------------------------------------
    //                                          Mesh�� �����ϴ� �۾�.
    void SeperateMesh( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshIn, VEC_D3DXMESH& vecD3DXMESH )
    {
        LPD3DXMESH pTempMesh;
        pMeshIn->CloneMeshFVF( pMeshIn->GetOptions(), VERTEX_WORLD::FVF, pd3dDevice, &pTempMesh );

        // Attrib Table
        DWORD dwAttrib(0);
        D3DXATTRIBUTERANGE* pAttrib(NULL);
        {   
		    pTempMesh->GetAttributeTable( NULL, &dwAttrib );
		    pAttrib = new D3DXATTRIBUTERANGE[ dwAttrib ];
		    pTempMesh->GetAttributeTable( pAttrib, &dwAttrib );
	    }

        VERTEX_WORLD*	pVertices;
        WORD*			pIndices;
        pTempMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
		pTempMesh->LockIndexBuffer( 0L, (VOID**)&pIndices );
        {
            for ( DWORD j=0; j<dwAttrib; ++j )
            {
                // Mesh�� �����.
                DWORD dwVertNUM = pAttrib[j].VertexCount;
                DWORD dwFaceNUM = pAttrib[j].FaceCount;

                LPD3DXMESH pSimpleMesh;
                D3DXCreateMeshFVF( dwFaceNUM, dwVertNUM, D3DXMESH_MANAGED, VERTEX_WORLD::FVF, pd3dDevice, &pSimpleMesh );

                // ������ ����.
                {
                    BYTE*   pDest;
                    pSimpleMesh->LockVertexBuffer( 0, (VOID**)&pDest );
                    memcpy( pDest, &pVertices[pAttrib[j].VertexStart], sizeof(VERTEX_WORLD)*dwVertNUM );
                    pSimpleMesh->UnlockVertexBuffer();

                    WORD*   pDestIndices;
                    pSimpleMesh->LockIndexBuffer( 0, (VOID**)&pDestIndices );
                    memcpy( pDestIndices, pIndices, sizeof(WORD)*dwFaceNUM*3 );
                    for ( DWORD i=0; i<dwFaceNUM; ++i )
                    {
                        pDestIndices[i*3+0] = (WORD)(pIndices[(pAttrib[j].FaceStart+i)*3+0] - pAttrib[j].VertexStart);
			            pDestIndices[i*3+1] = (WORD)(pIndices[(pAttrib[j].FaceStart+i)*3+1] - pAttrib[j].VertexStart);
			            pDestIndices[i*3+2] = (WORD)(pIndices[(pAttrib[j].FaceStart+i)*3+2] - pAttrib[j].VertexStart);
                    }
                    pSimpleMesh->UnlockIndexBuffer();
                }

                // �߰�
                vecD3DXMESH.push_back( pSimpleMesh );
            }
        }
        pTempMesh->UnlockIndexBuffer();
        pTempMesh->UnlockVertexBuffer();

        SAFE_DELETE_ARRAY( pAttrib );
        SAFE_RELEASE( pTempMesh );
    }

	void RenderRadiosity2 ( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxMeshes* pMesh, 
							DxLandMan* pLandMan,
							LPDIRECT3DTEXTUREQ pTextureSystem, 
							float fRadiosityReflectance,
							float fRadiosityFogBlend,
							DWORD dwRadiosityFogLength )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );

		for ( DWORD i=0; i<pMesh->m_vecLightMapObject.size(); ++i )
		{
			pMesh->m_vecLightMapObject[i]->CreateTexelTriangleID();
			{
				pMesh->m_vecLightMapObject[i]->BakeRadiosity
				(
					pd3dDevice,
					pLandMan,
					matIdentity,
					pTextureSystem,
					fRadiosityReflectance, 
					fRadiosityFogBlend,
					dwRadiosityFogLength,
					-1,
					NSLightMap::g_nSampling,
					FALSE
				);
			}
			pMesh->m_vecLightMapObject[i]->DeleteTexelTriangleID();
		}

		NSLightMap::CreateLightMapInRadiosity( pMesh->m_vecLightMapObject );
	}

	void RenderRadiosity1 ( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxFrame *pframeCur, 
							DxLandMan* pLandMan,
							LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
							LPDIRECT3DTEXTUREQ pTextureSystem, 
							float fRadiosityReflectance,
							float fRadiosityFogBlend,
							DWORD dwRadiosityFogLength )
	{		
		if ( pframeCur->IsReplace() || pframeCur->pframeFromAnimate || pframeCur->bPieceUSE )
		{
			// Note : ���⼭�� �ƹ��͵� �ϸ� �ȵȴ�.
			return;
		}

		// ����Ʈ�� ������ ���� Frame ���� ��츸 ���۵ȴ�.
		MAP_LIGHTMAPUV_ITER iter = pLightMapUV_MAIN->m_mapLightMapUV.find ( pframeCur->szName );
		if ( iter != pLightMapUV_MAIN->m_mapLightMapUV.end() )
		{
			DxMeshes* pMesh = pframeCur->pmsMeshs;
			while ( pMesh )
			{
				RenderRadiosity2
				( 
					pd3dDevice, 
					pMesh, 
					pLandMan, 
					pTextureSystem, 
					fRadiosityReflectance,
					fRadiosityFogBlend,
					dwRadiosityFogLength
				);

				pMesh = pMesh->m_pMeshNext;
			}
		}

		//	Note : �ڽĵ��� ��������� ��� Ž��.
		DxFrame *pframeChild = pframeCur->pframeFirstChild;
		while (pframeChild != NULL)
		{
			RenderRadiosity1 ( pd3dDevice, 
								pframeChild, 
								pLandMan,
								pLightMapUV_MAIN,
								pTextureSystem, 
								fRadiosityReflectance,
								fRadiosityFogBlend,
								dwRadiosityFogLength );

			pframeChild = pframeChild->pframeSibling;
		}
	}

	// FrameMesh �� �۵��ȴ�.
	void RenderRadiosityForFrameMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxFrameMesh* const pFrameMesh, 
									DxLandMan* pLandMan,
									LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
									LPDIRECT3DTEXTUREQ pTextureSystem, 
									float fRadiosityReflectance,
									float fRadiosityFogBlend,
									DWORD dwRadiosityFogLength )
	{
		DxFrame* pFrameRoot = pFrameMesh->GetFrameRoot();
		if ( pFrameRoot )
		{
			RenderRadiosity1 ( pd3dDevice, 
								pFrameRoot, 
								pLandMan,
								pLightMapUV_MAIN,
								pTextureSystem, 
								fRadiosityReflectance,
								fRadiosityFogBlend,
								dwRadiosityFogLength );
		}
	}
}

