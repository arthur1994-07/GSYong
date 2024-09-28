#include "pch.h"

#include "../../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../../DxTools/RENDERPARAM.h"
#include "../DxSkinDefine.h"

#include "./DxSkinMesh9.h"
//#include "./DxSkinMesh9_CPU.h"
#include "./DxSkinMesh9_NORMAL.h"
#include "./DxSkinMesh9_HLSL.h"
#include "./DxSkinMesh9_Material.h"

#include "DxSkinMeshContainer9.h"

// ----------------------------------------------------------------------------
//#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------
//													M		A		I		N
//----------------------------------------------------------------------------------------------------------------------------------
HRESULT SMeshContainer::GenerateSkinnedMesh( IDirect3DDevice9 *pd3dDevice )
{
    if( !pSkinInfo )	return S_OK;

    SAFE_RELEASE( MeshData.pMesh );
    SAFE_RELEASE( m_pBoneCombinationBuf );

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		GenerateSkinnedMeshNON( pd3dDevice );
	}
	else
	{
		GenerateSkinnedMeshMaterial( pd3dDevice );
	}

    return S_OK;
}

HRESULT SMeshContainer::GenerateObjectMesh( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	//////////////////////////////////////////////////////////////////////////
	//						Attribute 얻어오기.
	DWORD dwNumAttributeGroups(0L);
	hr = MeshData.pMesh->GetAttributeTable(NULL, &dwNumAttributeGroups);
	if (FAILED(hr))
	{
		return S_OK;
	}

	// [shhan][2014.03.21] npc_teacher_as_bow.cps 파일이 갯수가 동일하지 않음.
	//					갯수가 동일 하지 않을 때는 갯수를 맞춰주는 작업이 필요함.
	//					NumMaterials 2. dwNumAttributeGroups 1. 인 경우가 생김.
	if ( NumMaterials != dwNumAttributeGroups )
	{
		D3DXATTRIBUTERANGE* pAttributeTable(NULL);
		pAttributeTable = new D3DXATTRIBUTERANGE[dwNumAttributeGroups];
		if (pAttributeTable == NULL)
		{
			hr = E_OUTOFMEMORY;
			return S_OK;
		}

		hr = MeshData.pMesh->GetAttributeTable(pAttributeTable, NULL);
		if (FAILED(hr))
		{
			return S_OK;
		}


		//////////////////////////////////////////////////////////////////////////
		// 기초작업
		DWORD dwFVF = MeshData.pMesh->GetFVF();
		UINT nSIZE = D3DXGetFVFVertexSize( dwFVF );
		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		MeshData.pMesh->GetDeclaration( pDecl );


		//////////////////////////////////////////////////////////////////////////
		//						Mesh 분리 작업이 시작된다.
		std::vector<WORDWORD>	wwSrcMove;
		for ( WORD i=0; i<dwNumAttributeGroups; i++ )
		{
			if ( pAttributeTable[i].AttribId != i )
			{
				wwSrcMove.push_back( WORDWORD( static_cast<WORD>(pAttributeTable[i].AttribId), i ) );	// Material (pAttributeTable[i].AttribId) 번이 (i) 으로 이동되어야 한다.
				pAttributeTable[i].AttribId = i;
			}
		}

		MeshData.pMesh->SetAttributeTable( pAttributeTable, dwNumAttributeGroups );



		//////////////////////////////////////////////////////////////////////////
		//								변경		m_pMaterialEx
		DXMATERIALEX*	pMaterialExBackUp = new DXMATERIALEX[ NumMaterials ];
		for ( DWORD i=0; i<NumMaterials; ++i )
		{
			pMaterialExBackUp[i] = m_pMaterialEx[i];
		}

		SAFE_DELETE_ARRAY ( m_pMaterialEx );
		m_pMaterialEx = new DXMATERIALEX[ dwNumAttributeGroups ];
		for ( DWORD i=0; i<dwNumAttributeGroups; ++i )
		{
			m_pMaterialEx[i] = pMaterialExBackUp[i];
		}

		for ( DWORD i=0; i<wwSrcMove.size(); ++i )
		{
			m_pMaterialEx[ wwSrcMove[i].wB ] = pMaterialExBackUp[ wwSrcMove[i].wA ];
		}

		SAFE_DELETE_ARRAY ( pMaterialExBackUp );



		//////////////////////////////////////////////////////////////////////////
		//								변경		pMaterials
		for( UINT i=0; i<NumMaterials; ++i )
			SAFE_DELETE_ARRAY( pMaterials[ i ].pTextureFilename );

		D3DXMATERIAL* pMaterialsBackUp = new D3DXMATERIAL[ NumMaterials ];
		for ( DWORD i=0; i<NumMaterials; ++i )
		{
			pMaterialsBackUp[i] = pMaterials[i];
		}

		SAFE_DELETE_ARRAY ( pMaterials );
		pMaterials = new D3DXMATERIAL[ dwNumAttributeGroups ];
		for ( DWORD i=0; i<dwNumAttributeGroups; ++i )
		{
			pMaterials[i] = pMaterialsBackUp[i];
		}

		for ( DWORD i=0; i<wwSrcMove.size(); ++i )
		{
			pMaterials[ wwSrcMove[i].wB ] = pMaterialsBackUp[ wwSrcMove[i].wA ];
		}

		SAFE_DELETE_ARRAY ( pMaterialsBackUp );

		for ( DWORD i=0; i<dwNumAttributeGroups; ++i )
		{
			SIZE_T tLen = 0;
			if( m_pMaterialEx[i].m_textureRes.GetFileName().empty() )
				continue;
			else
				tLen = strlen( m_pMaterialEx[i].m_textureRes.GetFileName().c_str() )+1;

			pMaterials[i].pTextureFilename = new char[tLen];
			StringCchCopy ( pMaterials[i].pTextureFilename, tLen, m_pMaterialEx[i].m_textureRes.GetFileName().c_str() );
		}


		//////////////////////////////////////////////////////////////////////////
		//								변경		NumMaterials
		NumMaterials = dwNumAttributeGroups;



		//////////////////////////////////////////////////////////////////////////
		//								마무리
		SAFE_DELETE_ARRAY(pAttributeTable);
	}



	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		return S_OK;
	}

	GenerateObjectMeshMaterial( pd3dDevice );

	return S_OK;
}

//----------------------------------------------------------------------------------------------------------------------------------
//											N	O	N		I	N	D	E	X	E	D
//----------------------------------------------------------------------------------------------------------------------------------
//void SMeshContainer::GenerateSkinnedMeshNON_public( IDirect3DDevice9 *pd3dDevice )
//{
//    if ( pSkinInfo )
//    {
//        GenerateSkinnedMeshNON( pd3dDevice );
//    }
//}

void SMeshContainer::GenerateSkinnedMeshNON( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

    SAFE_RELEASE( MeshData.pMesh );
    SAFE_RELEASE( m_pBoneCombinationBuf );
	hr = pSkinInfo->ConvertToBlendedMesh
                                (
                                    m_pOrigMesh,
                                    D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
                                    pAdjacency, 
                                    NULL, NULL, NULL, 
                                    &m_dwNumInfl,
                                    &m_dwNumAttributeGroups, 
                                    &m_pBoneCombinationBuf, 
                                    &MeshData.pMesh
                                );
    if (FAILED(hr))
	{
        return;
	}


	// 새로운 Exporter 는 Decl 로 넘어오기 때문에 FVF 형식에 맞지 않을 수 있어서 변경 작업을 함.
	{
		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1;	// | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
			if (!FAILED(hr))
			{
				MeshData.pMesh->Release();
				MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}
	}


	//emMethod = D3DNONINDEXED;

    // If the device can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all meshes to it
    // Thus we split the mesh in two parts: The part that uses at most 2 matrices and the rest. The first is
    // drawn using the device's HW vertex processing and the rest is drawn using SW vertex processing.
    LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationBuf->GetBufferPointer());

    // look for any set of bone combinations that do not fit the caps
    for ( m_iAttributeSW = 0; m_iAttributeSW < m_dwNumAttributeGroups; m_iAttributeSW++)
    {
        DWORD cInfl   = 0;

        for (DWORD iInfl = 0; iInfl < m_dwNumInfl; iInfl++)
        {
            if (rgBoneCombinations[m_iAttributeSW].BoneId[iInfl] != UINT_MAX)
            {
                ++cInfl;
            }
        }

        if (cInfl > d3dCaps.MaxVertexBlendMatrices)
        {
            break;
        }
    }

    // if there is both HW and SW, add the Software Processing flag
    if ( m_iAttributeSW < m_dwNumAttributeGroups )
    {
		BOOL bSoftWareNONINDEXED = TRUE;


        LPD3DXMESH pMeshTmp;

        hr = MeshData.pMesh->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|MeshData.pMesh->GetOptions(), 
                                            MeshData.pMesh->GetFVF(),
                                            pd3dDevice, &pMeshTmp);
        if (FAILED(hr))
        {
            return;
        }

        MeshData.pMesh->Release();
        MeshData.pMesh = pMeshTmp;
        pMeshTmp = NULL;
    }

	// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
    DWORD nNumBones = pSkinInfo->GetNumBones();
    if( nNumBones > DxSkinMesh9::g_NumBoneMatricesReserve )
        DxSkinMesh9::g_NumBoneMatricesReserve = nNumBones;
}

//----------------------------------------------------------------------------------------------------------------------------------
//													H		L		S		L
//----------------------------------------------------------------------------------------------------------------------------------
void SMeshContainer::GenerateSkinnedMeshHLSL( IDirect3DDevice9 *pd3dDevice, bool &bUseSoftwareVP )
{
	HRESULT hr(S_OK);

	D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

	// Get palette size
    // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
    // (96 - 9) /3 i.e. Maximum constant count - used constants 
    UINT MaxMatrices = 60;
    m_dwNumPaletteEntries = new_min(MaxMatrices, pSkinInfo->GetNumBones());

    DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
    if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
    {
        //UseSoftwareVP = false;
        Flags |= D3DXMESH_MANAGED;
    }
    else
    {
        //UseSoftwareVP = true;
        bUseSoftwareVP = true;
        Flags |= D3DXMESH_SYSTEMMEM;
    }

    SAFE_RELEASE(MeshData.pMesh);

    hr = pSkinInfo->ConvertToIndexedBlendedMesh
                                            (
                                            m_pOrigMesh,
                                            Flags, 
                                            m_dwNumPaletteEntries, 
                                            pAdjacency, 
                                            NULL, NULL, NULL,             
                                            &m_dwNumInfl,
                                            &m_dwNumAttributeGroups, 
                                            &m_pBoneCombinationBuf, 
                                            &MeshData.pMesh);
    if (FAILED(hr))
	{
		return;
	}

	//emMethod = D3DINDEXEDHLSLVS;

    // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
    DWORD NewFVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
    if (NewFVF != MeshData.pMesh->GetFVF())
    {
        LPD3DXMESH pMesh;
        hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
        if (!FAILED(hr))
        {
            MeshData.pMesh->Release();
            MeshData.pMesh = pMesh;
            pMesh = NULL;
        }
    }

    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;
    hr = MeshData.pMesh->GetDeclaration(pDecl);
    if (FAILED(hr))
	{
		return;
	}

    // the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
    //   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
    //          this is more of a "cast" operation
    pDeclCur = pDecl;
    while (pDeclCur->Stream != 0xff)
    {
        if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
            pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
        pDeclCur++;
    }

    hr = MeshData.pMesh->UpdateSemantics(pDecl);
    if (FAILED(hr))
	{
        return;
	}

    // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
    DWORD nNumBones = pSkinInfo->GetNumBones();
    if( nNumBones > DxSkinMesh9::g_NumBoneMatricesReserve )
        DxSkinMesh9::g_NumBoneMatricesReserve = nNumBones;
}

//----------------------------------------------------------------------------------------------------------------------------------
//												S	O	F	T		W	A	R	E
//----------------------------------------------------------------------------------------------------------------------------------
void SMeshContainer::GenerateSkinnedMeshCPU( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	SAFE_RELEASE(MeshData.pMesh);
//
//	NumPaletteEntries = new_min(256, pSkinInfo->GetNumBones());
//    hr = pSkinInfo->ConvertToIndexedBlendedMesh
//                                            (
//                                            pOrigMesh,
//                                            D3DXMESHOPT_VERTEXCACHE | D3DXMESH_SYSTEMMEM, 
//                                            NumPaletteEntries, 
//                                            pAdjacency, 
//                                            NULL, NULL, NULL,             
//                                            &NumInfl,
//                                            &m_dwNumAttributeGroups, 
//                                            &pBoneCombinationBuf, 
//                                            &MeshData.pMesh);
//    if (FAILED(hr))
//    {
//        return;
//	}
//
//	emMethod = D3DINDEXED;
//
//	//hr = pSkinInfo->ConvertToBlendedMesh
////                                 (
////                                     pOrigMesh,
////                                     D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
////                                     pAdjacency, 
////                                     NULL, NULL, NULL, 
////                                     &NumInfl,
////                                     &m_dwNumAttributeGroups, 
////                                     &pBoneCombinationBuf, 
////                                     &MeshData.pMesh
////                                 );
////      if (FAILED(hr))
////          goto e_Exit;
//
//
//	DWORD NewFVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
//    if (NewFVF != MeshData.pMesh->GetFVF())
//    {
//        LPD3DXMESH pMesh;
//        hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
//        if (!FAILED(hr))
//        {
//            MeshData.pMesh->Release();
//            MeshData.pMesh = pMesh;
//            pMesh = NULL;
//        }
//    }
//
//	LPD3DXMESH pMesh;
//    hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), MeshData.pMesh->GetFVF(), pd3dDevice, &pMesh);
//
//	LPD3DXMESH pMeshLOD[2];
//	CreateLOD_SMeshCPU( pd3dDevice, pMesh, pAdjacency, pMeshLOD[0], pMeshLOD[1]);
//	m_sMeshLOD[0].Convert( pd3dDevice, pMeshLOD[0] );
//	m_sMeshLOD[1].Convert( pd3dDevice, pMeshLOD[1] );
//
//	SAFE_RELEASE( pMeshLOD[0] );
//	SAFE_RELEASE( pMeshLOD[1] );
//
//	SAFE_RELEASE( pMesh );
//
//	//{
//	//	m_dwVertexNUM = MeshData.pMesh->GetNumVertices();
//	//	m_dwFVF = MeshData.pMesh->GetFVF();
//	//	DWORD dwSizeFVF = D3DXGetFVFVertexSize( m_dwFVF );
//	//	DWORD dwVertices = MeshData.pMesh->GetNumVertices();
//	//	
//	//	SAFE_DELETE_ARRAY( m_pVB_DATA );
//
//	//	BYTE* pSrc;
//	//	MeshData.pMesh->LockVertexBuffer( 0L, (VOID**)&pSrc );
//
//	//	switch( m_dwFVF )
//	//	{
//	//	case 0x1116:
//	//		ConvertSkinnedMesh_0( m_pVB_DATA, pSrc, dwVertices );
//	//		break;
//	//	case 0x1118:
//	//		ConvertSkinnedMesh_1( m_pVB_DATA, pSrc, dwVertices );
//	//		break;
//	//	case 0x111a:
//	//		ConvertSkinnedMesh_2( m_pVB_DATA, pSrc, dwVertices );
//	//		break;
//	//	case 0x111c:
//	//		ConvertSkinnedMesh_3( m_pVB_DATA, pSrc, dwVertices );
//	//		break;
//	//	case 0x111e:
//	//		ConvertSkinnedMesh_4( m_pVB_DATA, pSrc, dwVertices );
//	//		break;
//	//	}
//
//	//	MeshData.pMesh->UnlockVertexBuffer();
//	//}
//
//	{
//		LPD3DXMESH pMesh;
//        hr = MeshData.pMesh->CloneMeshFVF( D3DXMESH_WRITEONLY|D3DXMESH_SYSTEMMEM, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, pd3dDevice, &pMesh );
//        if (!FAILED(hr))
//        {
//            MeshData.pMesh->Release();
//            MeshData.pMesh = pMesh;
//            pMesh = NULL;
//        }
//	}
//
//    hr = MeshData.pMesh->GetAttributeTable(NULL, &m_dwNumAttributeGroups);
//    if (FAILED(hr))
//    {
//        return;
//	}
//
//	SAFE_DELETE_ARRAY(pAttributeTable);
//    pAttributeTable  = new D3DXATTRIBUTERANGE[m_dwNumAttributeGroups];
//    if (pAttributeTable == NULL)
//    {
//        hr = E_OUTOFMEMORY;
//        return;
//    }
//
//    hr = MeshData.pMesh->GetAttributeTable(pAttributeTable, NULL);
//    if (FAILED(hr))
//    {
//        return;
//	}
//
//    // allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
//    DWORD nNumBones = pSkinInfo->GetNumBones();
//    if( nNumBones > DxSkinMesh9_CPU::g_NumBoneMatricesReserve )
//        DxSkinMesh9_CPU::g_NumBoneMatricesReserve = nNumBones;
}

//----------------------------------------------------------------------------------------------------------------------------------
//													Material System
//----------------------------------------------------------------------------------------------------------------------------------
void SMeshContainer::GenerateSkinnedMeshMaterial( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	// Get palette size
	// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
	// (96 - 9) /3 i.e. Maximum constant count - used constants 
	UINT MaxMatrices = 60;
	m_dwNumPaletteEntries = new_min(MaxMatrices, pSkinInfo->GetNumBones());

	DWORD dwFlags = D3DXMESHOPT_VERTEXCACHE | D3DXMESH_MANAGED;

	SAFE_RELEASE(MeshData.pMesh);

	hr = pSkinInfo->ConvertToIndexedBlendedMesh
		(
		m_pOrigMesh,
		dwFlags, 
		m_dwNumPaletteEntries, 
		pAdjacency, 
		NULL, NULL, NULL,             
		&m_dwNumInfl,
		&m_dwNumAttributeGroups, 
		&m_pBoneCombinationBuf, 
		&MeshData.pMesh);
	if (FAILED(hr))
	{
		return;
	}

	// Cloth 용 InvWeight 를 얻는다.
	{
		// 새로운 Exporter 에서는 decl 로 보내기 때문에 이 작업이 필요하다.
		BOOL bDiffuse_ON(FALSE);
		{
			D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
			LPD3DVERTEXELEMENT9 pDeclCur;
			MeshData.pMesh->GetDeclaration( pDecl );

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while (pDeclCur->Stream != 0xff)
			{
				if ((pDeclCur->Usage == D3DDECLUSAGE_COLOR) && (pDeclCur->UsageIndex == 0))
				{
					bDiffuse_ON = TRUE;
					break;
				}

				pDeclCur++;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// D3DXMESHOPT_VERTEXCACHE 로 인해 늦게 정보를 얻어야 함.
		if ( bDiffuse_ON )
		{
			//// Create a new vertex declaration to hold all the required data
			//D3DVERTEXELEMENT9 VertexDecl[] =
			//{
			//	{ 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 },
			//	D3DDECL_END()
			//};

			//// 임시Mesh
			//LPD3DXMESH pTempMesh = NULL;
			//hr = MeshData.pMesh->CloneMesh( MeshData.pMesh->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );
			//if (FAILED(hr))
			//	return;

			//// Diffuse 가 있다.
			//DWORD* pVertices;
			//pTempMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pVertices );
			//m_arrayfInvWeight = new float[ pTempMesh->GetNumVertices() ];
			//for ( DWORD i=0; i<pTempMesh->GetNumVertices(); ++i )
			//{
			//	m_arrayfInvWeight[i] = static_cast<float>( (pVertices[i] & 0xff000000) >> 24 ) / 255.f;
			//}
			//pTempMesh->UnlockVertexBuffer();

			//SAFE_RELEASE ( pTempMesh );



			//////////////////////////////////////////////////////////////////////////
			// 2군대 작업해줘야한다.
			// Create a new vertex declaration to hold all the required data
			D3DVERTEXELEMENT9 VertexDecl[] =
			{
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
				{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 },
				D3DDECL_END()
			};

			// 임시Mesh
			LPD3DXMESH pTempMesh = NULL;
			hr = MeshData.pMesh->CloneMesh( MeshData.pMesh->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );
			if (FAILED(hr))
				return;

			// Diffuse 가 있다.
			VERTEXCOLOR* pVertices;
			pTempMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pVertices );

			float fLowHeightY = 10000.f;
			float fLowHeightY_2 = 10000.f;
			float fHighHeightY = -10000.f;
			for ( DWORD i=0; i<pTempMesh->GetNumVertices(); ++i )
			{
				if ( fHighHeightY < pVertices[i].vPos.y )
				{
					fHighHeightY = pVertices[i].vPos.y;
				}
				if ( fLowHeightY > pVertices[i].vPos.y )
				{
					fLowHeightY_2 = fLowHeightY;
					fLowHeightY = pVertices[i].vPos.y;
				}
			}

			m_arrayfInvWeight = new float[ pTempMesh->GetNumVertices() ];
			for ( DWORD i=0; i<pTempMesh->GetNumVertices(); ++i )
			{
				m_arrayfInvWeight[i] = static_cast<float>( (pVertices[i].dwColor & 0xff000000) >> 24 ) / 255.f;

				if ( m_arrayfInvWeight[i] != 0.f )
				{
					m_arrayfInvWeight[i] = (fHighHeightY - pVertices[i].vPos.y)/(fHighHeightY-fLowHeightY_2);

					if ( m_arrayfInvWeight[i] <= 0.f )
						m_arrayfInvWeight[i] = 0.0001f;
				}
			}

			pTempMesh->UnlockVertexBuffer();

			SAFE_RELEASE ( pTempMesh );
		}
	}
	
	// 알맞는 기본 FVF 로 셋팅한다.
	if ( MeshData.pMesh->GetFVF() == 0 )
	{
		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = MeshData.pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 알맞는 Decl 를 구한다.
		BOOL bAddSetting(FALSE);
		DWORD dwIndex(0);
		DWORD dwOffset(0);
		D3DVERTEXELEMENT9 VertexDecl[MAX_FVF_DECL_SIZE];
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			// 그냥 이렇게 보기 안 좋은 코드로 함.
			// 원하는 값만 쓰도록 함.
			bAddSetting = FALSE;
			if ((pDeclCur->Usage == D3DDECLUSAGE_POSITION) && (pDeclCur->UsageIndex == 0))
			{
				bAddSetting = TRUE;
			}
			else if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDWEIGHT))
			{
				bAddSetting = TRUE;
			}
			else if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES))
			{
				bAddSetting = TRUE;
			}
			else if ((pDeclCur->Usage == D3DDECLUSAGE_NORMAL))
			{
				bAddSetting = TRUE;
			}
			else if ((pDeclCur->Usage == D3DDECLUSAGE_TEXCOORD) && (pDeclCur->UsageIndex == 0))
			{
				bAddSetting = TRUE;
			}
			else if ((pDeclCur->Usage == D3DDECLUSAGE_COLOR) && (pDeclCur->UsageIndex == 0))
			{
				VertexDecl[dwIndex] = *pDeclCur;
				VertexDecl[dwIndex].Offset = static_cast<WORD>( dwOffset );
				VertexDecl[dwIndex].Type = D3DDECLTYPE_D3DCOLOR;
				dwOffset += 4;
				++dwIndex;
			}

			// 셋팅.
			if ( bAddSetting )
			{
				VertexDecl[dwIndex] = *pDeclCur;
				VertexDecl[dwIndex].Offset = static_cast<WORD>( dwOffset );

				switch ( VertexDecl[dwIndex].Type )
				{
				case D3DDECLTYPE_FLOAT1:
					dwOffset += 4;
					break;

				case D3DDECLTYPE_FLOAT2:
					dwOffset += 8;
					break;

				case D3DDECLTYPE_FLOAT3:
					dwOffset += 12;
					break;

				case D3DDECLTYPE_FLOAT4:
					dwOffset += 16;
					break;

				case D3DDECLTYPE_D3DCOLOR:
					dwOffset += 4;
					break;

				case D3DDECLTYPE_UBYTE4:
					dwOffset += 4;
					break;
				};
				++dwIndex;
			}

			pDeclCur++;
		}
		VertexDecl[dwIndex] = *pDeclCur;

		// 임시Mesh
		LPD3DXMESH pTempMesh = NULL;
		hr = MeshData.pMesh->CloneMesh( MeshData.pMesh->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );
		if (FAILED(hr))
			return;

		SAFE_RELEASE ( MeshData.pMesh );
		MeshData.pMesh = pTempMesh;
	}
	else
	{
		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = MeshData.pMesh->CloneMeshFVF(MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
			if (!FAILED(hr))
			{
				MeshData.pMesh->Release();
				MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}

		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = MeshData.pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
		{
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 다음 CloneMesh or CloneMeshFVF 에서의 문제가 발생하지 않기 위해서 작업한다.
		// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
		//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
		//          this is more of a "cast" operation
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
			{
				pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
				break;
			}
			pDeclCur++;
		}

		//////////////////////////////////////////////////////////////////////////
		// SkinMesh를 HLSL로 렌더링한다면, CloneMeshFVF 후에 꼭 작업해 줘야하는 것.
		hr = MeshData.pMesh->UpdateSemantics(pDecl);
		if (FAILED(hr))
		{
			return;
		}
	}

	// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
	//DWORD nNumBones = pSkinInfo->GetNumBones();

	// AniThread 작업되면서 DxSkinMesh9::g_pBoneMatrices 사용법이 달라짐.
	// 그래서 새로운 방법에 맞는 사이즈 변경이 필요하게 됨.
	DWORD nNumBones = m_dwNumAttributeGroups * m_dwNumPaletteEntries;
	if( nNumBones > DxSkinMesh9::g_NumBoneMatricesReserve )
		DxSkinMesh9::g_NumBoneMatricesReserve = nNumBones;

	m_spMultiMaterialSkinMesh = std::tr1::shared_ptr<DxMultiMaterialSkinMesh>(new DxMultiMaterialSkinMesh);
	m_spMultiMaterialSkinMesh->Import_First( pd3dDevice, MeshData.pMesh, m_pMaterialEx, reinterpret_cast<LPD3DXBONECOMBINATION>(m_pBoneCombinationBuf->GetBufferPointer()) );
}

void SMeshContainer::GenerateObjectMeshMaterial( IDirect3DDevice9 *pd3dDevice )
{
	HRESULT hr(S_OK);

	m_spMultiMaterialSkinMesh = std::tr1::shared_ptr<DxMultiMaterialSkinMesh>(new DxMultiMaterialSkinMesh);
	m_spMultiMaterialSkinMesh->Import_First( pd3dDevice, MeshData.pMesh, m_pMaterialEx, NULL );
}

void SMeshContainer::GenerateSkinnedMeshNormalMap( IDirect3DDevice9 *pd3dDevice )
{
	GenerateSkinnedMeshNormalMap( pd3dDevice, m_sMeshDataHLSL, D3DFVF_NORMAL | D3DFVF_TEX1 );
}

void SMeshContainer::GenerateSkinnedMeshNormalMap( IDirect3DDevice9 *pd3dDevice, MESHDATA_HLSL &sMeshData, DWORD dwFVF_ExceptPOSITION_MASK )
{
	HRESULT hr = S_OK;
	D3DCAPS9 d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );

	if( pSkinInfo == NULL )	return;	



	UINT MaxMatrices = 60; 
	m_dwNumPaletteEntries = new_min(MaxMatrices, pSkinInfo->GetNumBones());

	DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
	if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
	{
		//UseSoftwareVP = false;
		Flags |= D3DXMESH_MANAGED;
	}
	else
	{
		//UseSoftwareVP = true;
		Flags |= D3DXMESH_SYSTEMMEM;
	}

	SAFE_RELEASE(sMeshData.m_MeshData.pMesh);

	hr = pSkinInfo->ConvertToIndexedBlendedMesh
		(
		m_pOrigMesh,
		Flags, 
		m_dwNumPaletteEntries, 
		pAdjacency, 
		NULL, NULL, NULL,             
		&sMeshData.m_NumInfl,
		&sMeshData.m_NumAttributeGroups, 
		&sMeshData.m_pBoneCombinationBuf, 
		&sMeshData.m_MeshData.pMesh);
	
	if (FAILED(hr))
		return;

	// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
	DWORD NewFVF = (sMeshData.m_MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | dwFVF_ExceptPOSITION_MASK | D3DFVF_LASTBETA_UBYTE4;
	if (NewFVF != sMeshData.m_MeshData.pMesh->GetFVF())
	{
		LPD3DXMESH pMesh;
		hr = sMeshData.m_MeshData.pMesh->CloneMeshFVF(sMeshData.m_MeshData.pMesh->GetOptions(), NewFVF, pd3dDevice, &pMesh);
		if (!FAILED(hr))
		{
			sMeshData.m_MeshData.pMesh->Release();
			sMeshData.m_MeshData.pMesh = pMesh;
			pMesh = NULL;
		}
	}

	D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
	LPD3DVERTEXELEMENT9 pDeclCur;
	hr = sMeshData.m_MeshData.pMesh->GetDeclaration(pDecl);
	if (FAILED(hr))
		return;

	// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
	//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
	//          this is more of a "cast" operation
	pDeclCur = pDecl;
	while (pDeclCur->Stream != 0xff)
	{
		if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
			pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
		pDeclCur++;
	}

	hr = sMeshData.m_MeshData.pMesh->UpdateSemantics(pDecl);
	if (FAILED(hr))
		return;

	// allocate a buffer for bone matrices, but only if another mesh has not allocated one of the same size or larger
    DWORD nNumBones = pSkinInfo->GetNumBones();
    if( nNumBones > DxSkinMesh9::g_NumBoneMatricesReserve )
        DxSkinMesh9::g_NumBoneMatricesReserve = nNumBones;
}
//
//void SMeshContainer::CreateShadowForMaterial( IDirect3DDevice9 *pd3dDevice )
//{
//	if ( pSkinInfo )
//	{
//		if ( !m_sMeshDataShadow.m_MeshData.pMesh )
//		{
//			GenerateSkinnedMeshNormalMap( pd3dDevice, m_sMeshDataShadow, 0L );
//		}
//	}
//}