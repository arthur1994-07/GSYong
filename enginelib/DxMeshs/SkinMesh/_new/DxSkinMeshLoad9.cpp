#include "pch.h"

#include "../../../DxTools/RENDERPARAM.h"

#include "DxSkinMesh9.h"
#include "DxSkinMeshContainer9.h"
#include "../DxMaterial.h"
#include "../../../DxTools/TextureManager.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Allocates memory for a string to hold the name of a frame or mesh
//--------------------------------------------------------------------------------------
HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName )
{
    UINT cbLength;

    if( Name != NULL )
    {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
    HRESULT hr = S_OK;

    *ppNewFrame = NULL;

    SFrame *pFrame = NULL;

    pFrame = new SFrame;
    if (pFrame == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	hr = AllocateName ( Name, &pFrame->Name);
	if (FAILED(hr))
        goto e_Exit;

    *ppNewFrame = pFrame;
    pFrame = NULL;

e_Exit:
	SAFE_DELETE(pFrame);

    return hr;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer (
    LPCSTR Name,
    CONST D3DXMESHDATA *pMeshData,
    CONST D3DXMATERIAL *pMaterials, 
    CONST D3DXEFFECTINSTANCE *pEffectInstances, 
    DWORD NumMaterials, 
    CONST DWORD *pAdjacency, 
    LPD3DXSKININFO pSkinInfo, 
    LPD3DXMESHCONTAINER *ppNewMeshContainer ) 
{
    HRESULT hr;
    SMeshContainer *pMeshContainer = NULL;
    UINT NumFaces;
    UINT iMaterial;
    UINT iBone, cBones;
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;

    LPD3DXMESH pMesh = NULL;

    *ppNewMeshContainer = NULL;

    // this sample does not handle patch meshes, so fail when one is found
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // get the pMesh interface pointer out of the mesh data structure
    pMesh = pMeshData->pMesh;

	// ī��Ʈ�� �÷����´�.
	{
		// pMesh �� pMeshContainer->MeshData.pMesh �� �ƴ� MeshData.pMesh �̱� ������ AddRef �� �ʿ���.
		pMesh->AddRef();
	}

    //// this sample does not FVF compatible meshes, so fail when one is found
    //if (pMesh->GetFVF() == 0)
    //{
    //    hr = E_FAIL;
    //    goto e_Exit;
    //}

    // allocate the overloaded structure to return as a D3DXMESHCONTAINER
    pMeshContainer = new SMeshContainer;
    if (pMeshContainer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
    hr = AllocateName(Name, &pMeshContainer->Name);
    if (FAILED(hr))		goto e_Exit;        

    pMesh->GetDevice(&pd3dDevice);
    NumFaces = pMesh->GetNumFaces();

	//////////////////////////////////////////////////////////////////////////
	// Skin ���� �ٸ������� ������ ��´�. �⺻ FVF�� �����ϵ��� �Ѵ�.
	// Skin ���� D3DXMESHOPT_VERTEXCACHE �� ����ϱ� ������, ���� ������ ����Ǿ �ڴʰ� �ǵ��� ��.
	// Object �� OptimizeInplace �� ����Ͽ� D3DXMESHOPT_VERTEXCACHE �� ���� �� �� �ʰ� �۾��ؾ� ��.
	if ( !pSkinInfo )
	{
		//////////////////////////////////////////////////////////////////////////
		// ���ο� Exporter ������ decl �� ������ ������ �� �۾��� �ʿ��ϴ�.
		BOOL bDiffuse_ON(FALSE);
		{
			D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
			LPD3DVERTEXELEMENT9 pDeclCur;
			pMesh->GetDeclaration( pDecl );

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
		if ( bDiffuse_ON )
		{
			//// Create a new vertex declaration to hold all the required data
			//D3DVERTEXELEMENT9 VertexDecl[] =
			//{
			//	{ 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 },
			//	D3DDECL_END()
			//};

			//// �ӽ�Mesh
			//LPD3DXMESH pTempMesh = NULL;
			//hr = pMesh->CloneMesh( pMesh->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );
			//if (FAILED(hr))
			//	goto e_Exit;

			//// Diffuse �� �ִ�.
			//DWORD* pVertices;
			//pTempMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pVertices );
			//pMeshContainer->m_arrayfInvWeight = new float[ pTempMesh->GetNumVertices() ];
			//for ( DWORD i=0; i<pTempMesh->GetNumVertices(); ++i )
			//{
			//	pMeshContainer->m_arrayfInvWeight[i] = static_cast<float>( (pVertices[i] & 0xff000000) >> 24 ) / 255.f;
			//}
			//pTempMesh->UnlockVertexBuffer();

			//SAFE_RELEASE ( pTempMesh );



			//////////////////////////////////////////////////////////////////////////
			// 2���� �۾�������Ѵ�.
			// Create a new vertex declaration to hold all the required data
			D3DVERTEXELEMENT9 VertexDecl[] =
			{
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
				{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0 },
				D3DDECL_END()
			};

			// �ӽ�Mesh
			LPD3DXMESH pTempMesh = NULL;
			hr = pMesh->CloneMesh( pMesh->GetOptions(), VertexDecl, pd3dDevice, &pTempMesh );
			if (FAILED(hr))
				goto e_Exit;

			// Diffuse �� �ִ�.
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

			pMeshContainer->m_arrayfInvWeight = new float[ pTempMesh->GetNumVertices() ];
			for ( DWORD i=0; i<pTempMesh->GetNumVertices(); ++i )
			{
				pMeshContainer->m_arrayfInvWeight[i] = static_cast<float>( (pVertices[i].dwColor & 0xff000000) >> 24 ) / 255.f;

				if ( pMeshContainer->m_arrayfInvWeight[i] != 0.f )
				{
					pMeshContainer->m_arrayfInvWeight[i] = (fHighHeightY - pVertices[i].vPos.y)/(fHighHeightY-fLowHeightY_2);

					if ( pMeshContainer->m_arrayfInvWeight[i] <= 0.f )
						pMeshContainer->m_arrayfInvWeight[i] = 0.0001f;
				}
			}

			pTempMesh->UnlockVertexBuffer();

			SAFE_RELEASE ( pTempMesh );
		}

		//////////////////////////////////////////////////////////////////////////
		if (pMesh->GetFVF() == 0)
		{
			// clone the mesh to make room for the normals
			hr = pMesh->CloneMeshFVF ( pMesh->GetOptions(), D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, pd3dDevice, &pMeshContainer->MeshData.pMesh );
			if (FAILED(hr))
				goto e_Exit;

			SAFE_RELEASE ( pMesh );
			pMesh = pMeshContainer->MeshData.pMesh;
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// [shhan][2014.05.14] ���ο� Export ���� D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 �� ���� �ʴ´ٸ�, ���� ������ �̻��ϰ� �Ǵ� ������ �����.
		//						D3DDECLUSAGE_COLOR ���� D3DDECLTYPE_FLOAT4 �� �Ǿ�����鼭 ������. D3DDECLTYPE_D3DCOLOR �� ��������ϴµ�.
		//						CloneMeshFVF �� D3DFVF_DIFFUSE �� Ȱ���Ͽ� �����־���.
		if (pMesh->GetFVF() == 0)
		{
			// clone the mesh to make room for the normals
			hr = pMesh->CloneMeshFVF ( pMesh->GetOptions(), D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1, pd3dDevice, &pMeshContainer->MeshData.pMesh );
			if (FAILED(hr))
				goto e_Exit;

			SAFE_RELEASE ( pMesh );
			pMesh = pMeshContainer->MeshData.pMesh;
		}
	}

	//if( RENDERPARAM::emCharRenderTYPE==EMCRT_PIXEL )
	//{
	//	LPD3DXMESH pNewMesh = NULL;		

	//	// Rearrange the mesh as desired
	//	hr = FilterMesh( pd3dDevice, pMeshData->pMesh, &pNewMesh );
	//	if( FAILED(hr) )
	//		goto e_Exit;

	//	// Copy the pointer
	//	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	//	pMeshContainer->MeshData.pMesh = pNewMesh;
	//	pNewMesh = NULL;
	//}
	//else
	//{
		//// if no normals are in the mesh, add them
		//if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
		//{
		//	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		//	// clone the mesh to make room for the normals
		//	hr = pMesh->CloneMeshFVF ( pMesh->GetOptions(), 
		//								pMesh->GetFVF() | D3DFVF_NORMAL, 
		//								pd3dDevice, &pMeshContainer->MeshData.pMesh );
		//	if (FAILED(hr))
		//		goto e_Exit;

		//	// get the new pMesh pointer back out of the mesh container to use
		//	// NOTE: we do not release pMesh because we do not have a reference to it yet
		//	pMesh = pMeshContainer->MeshData.pMesh;

		//	// now generate the normals for the pmesh
		//	D3DXComputeNormals( pMesh, NULL );
		//}
		//else  // if no normals, just add a reference to the mesh for the mesh container
		//{
		//	pMeshContainer->MeshData.pMesh = pMesh;
		//	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		//	// pMesh �� pMeshContainer->MeshData.pMesh �� �ƴ� MeshData.pMesh �̱� ������ AddRef �� �ʿ���.
		//	pMesh->AddRef();
		//}
	//}


	//////////////////////////////////////////////////////////////////////////
	pMeshContainer->MeshData.pMesh = pMesh;
	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	//////////////////////////////////////////////////////////////////////////
    // allocate memory to contain the material information.  This sample uses
    //   the D3D9 materials and texture names instead of the EffectInstance style materials
    pMeshContainer->NumMaterials = new_max(1, NumMaterials);
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->m_pMaterialEx = new DXMATERIALEX[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];

    if( ( pMeshContainer->pMaterials == NULL ) ||
        ( pMeshContainer->m_pMaterialEx == NULL ) ||
        ( pMeshContainer->pAdjacency == NULL ) )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);

    // if materials provided, copy them
    if ( NumMaterials > 0 )
    {
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

        for ( iMaterial = 0; iMaterial < NumMaterials; iMaterial++ )
        {
			pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient = pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Power = 14.0f;

			SIZE_T tLen = 0;
			if( pMaterials[iMaterial].pTextureFilename )	tLen = strlen(pMaterials[iMaterial].pTextureFilename)+1;
			else											continue;

			pMeshContainer->pMaterials[iMaterial].pTextureFilename = new char[tLen];
			StringCchCopy ( pMeshContainer->pMaterials[iMaterial].pTextureFilename, tLen, pMaterials[iMaterial].pTextureFilename );

            if ( pMeshContainer->pMaterials[iMaterial].pTextureFilename!=NULL )
            {
                pMeshContainer->m_pMaterialEx[iMaterial].m_textureRes = TextureManager::GetInstance().LoadTexture(
                    pMeshContainer->pMaterials[iMaterial].pTextureFilename,
                    false,
                    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                    EMMM_CHAR_MIPMAP,
                    D3DFMT_UNKNOWN,
                    TEXTURE_RESIZE_NONE,
                    TEXTURE_CHAR,
                    m_bThread ? true : false,
					FALSE );

				if ( pMeshContainer->m_pMaterialEx[iMaterial].m_textureRes.GetTexture() )
				{
                    DWORD &dwFlags = pMeshContainer->m_pMaterialEx[iMaterial].dwFlags;

					//	Note : �ؽ��� �б⿡ �����ÿ� �� ���͸����� �ʱ� ���� ����.
					//
					TEXTURE_ALPHA emType = pMeshContainer->m_pMaterialEx[iMaterial].m_textureRes.GetAlphaType();
					if( emType==EMTT_ALPHA_HARD )			dwFlags = DXMATERIALEX::FLAG_ALPHA_HARD;
					else if( emType==EMTT_ALPHA_SOFT )		dwFlags = DXMATERIALEX::FLAG_ALPHA_SOFT;
					else if( emType==EMTT_ALPHA_SOFT01 )	dwFlags = DXMATERIALEX::FLAG_ALPHA_SOFT;
					else if( emType==EMTT_ALPHA_SOFT02 )	dwFlags = DXMATERIALEX::FLAG_ALPHA_SOFT;
				}
            }
        }
    }
    else // if no materials provided, use a default one
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
		pMeshContainer->pMaterials[0].MatD3D.Power = 14.0f;
    }

    // if there is skinning information, save off the required data and then setup for HW skinning
    if (pSkinInfo != NULL)
    {
        // first save off the SkinInfo and original mesh data
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->m_pOrigMesh = pMesh;
        pMesh->AddRef();

        // Will need an array of offset matrices to move the vertices from the figure space to the bone's space
        cBones = pSkinInfo->GetNumBones();
        pMeshContainer->m_pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->m_pBoneOffsetMatrices == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

		UINT nInverseScaleCount(0);

		// get each of the bone offset matrices so that we don't need to get them later
		for (iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->m_pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));

			// [shhan][2014.09.29] Max ���� Scale ���̳ʽ� �� ����ִ� ������ �ذ��ϱ� ���� �۾�.
			//						Skin �� ���� Export ���� �� �������� �� �� �ִ�.
			D3DXVECTOR3 vScale;
			D3DXVECTOR3 vPos;
			D3DXQUATERNION sQuat;
			D3DXMatrixToSQT( vScale, sQuat, vPos, pMeshContainer->m_pBoneOffsetMatrices[iBone] );
			if ( vScale.x < 0.f )
			{
				++nInverseScaleCount;
			}
		}

		// ������ ������ ��ü Inverse
		if ( nInverseScaleCount*2 > cBones )
		{
			for (iBone = 0; iBone < cBones; iBone++)
			{
				// [shhan][2014.09.29] Max ���� Scale ���̳ʽ� �� ����ִ� ������ �ذ��ϱ� ���� �۾�.
				//						Skin �� ���� Export ���� �� �������� �� �� �ִ�.
				//						��Ų ��ü�� �ι��� �� ���¶�.. ������ �ƴ� ����� �˻��ؼ� ������ �۾��� �Ѵ�.
				//						�̰��� ������ ���̳ʽ� Scale �� �Ǿ�� �Ѵ�.
				D3DXVECTOR3 vScale;
				D3DXVECTOR3 vPos;
				D3DXQUATERNION sQuat;
				D3DXMatrixToSQT( vScale, sQuat, vPos, pMeshContainer->m_pBoneOffsetMatrices[iBone] );
				if ( vScale.x > 0.f )
				{
					D3DXMATRIX matScaling;
					D3DXMatrixScaling( &matScaling, -1.f, -1.f, -1.f );
					D3DXMatrixMultiply( &pMeshContainer->m_pBoneOffsetMatrices[iBone], &pMeshContainer->m_pBoneOffsetMatrices[iBone], &matScaling );
				}
			}

			// ������ ���� �ʿ�� ���°� ����.
			//CDebugSet::ToLogFile( sc::string::format( _T("InverseSacle : %1% \n"), m_strSkinFile.c_str() ) );
		}
		else
		{
			// ������ Inverse �� �� ����.
			for (iBone = 0; iBone < cBones; iBone++)
			{
				// [shhan][2014.09.29] Max ���� Scale ���̳ʽ� �� ����ִ� ������ �ذ��ϱ� ���� �۾�.
				//						Skin �� ���� Export ���� �� �������� �� �� �ִ�.
				//						�̰��� ������ Plus Scale �� �Ǿ�� �Ѵ�.
				D3DXVECTOR3 vScale;
				D3DXVECTOR3 vPos;
				D3DXQUATERNION sQuat;
				D3DXMatrixToSQT( vScale, sQuat, vPos, pMeshContainer->m_pBoneOffsetMatrices[iBone] );
				if ( vScale.x < 0.f )
				{
					D3DXMATRIX matScaling;
					D3DXMatrixScaling( &matScaling, -1.f, -1.f, -1.f );
					D3DXMatrixMultiply( &pMeshContainer->m_pBoneOffsetMatrices[iBone], &pMeshContainer->m_pBoneOffsetMatrices[iBone], &matScaling );
				}
			}
		}

        // GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
		hr = pMeshContainer->GenerateSkinnedMesh( pd3dDevice );
        if (FAILED(hr))
            goto e_Exit;
    }
	else
	{
		hr = pMeshContainer->GenerateObjectMesh( pd3dDevice );
		if (FAILED(hr))
			goto e_Exit;
	}

	//	Note : ���ο� �޽� �����̳ʸ� ����.
//	pframeParent->AddMesh (pMeshContainer );		//	SFrame�� ���.
	m_pSkinMesh->AddMeshGlobal( pMeshContainer );	//	DxSkinMesh�� ���.

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;

e_Exit:
    SAFE_RELEASE( pd3dDevice );
    SAFE_DELETE( pMeshContainer );

    return hr;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
    SFrame* pFrame = (SFrame*)pFrameToFree;
    SAFE_DELETE( pFrame );

    return S_OK; 
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer ( LPD3DXMESHCONTAINER pMeshContainerBase )
{
    SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;
    SAFE_DELETE( pMeshContainer );

    return S_OK;
}

void DxSkinMesh9::AddMeshGlobal ( SMeshContainer *pmc )
{
	pmc->m_pGlobalNext = m_pMeshContainerHead;
	m_pMeshContainerHead = pmc;
}

HRESULT DxSkinMesh9::SetupNameOnMeshContainer ( LPD3DXFRAME pFrame )
{
    HRESULT hr;

    if ( pFrame->pMeshContainer != NULL )
    {
		if ( !pFrame->Name || !strlen(pFrame->Name) )
		{
			SFrame *pframeParent = FindParentFrame ( (SFrame*)pFrame );
			if( pframeParent && pframeParent->Name )
			{
				char *szMesh = "[Mesh]";
				size_t nStrLen = strlen(pframeParent->Name)+strlen(szMesh)+1;

				SAFE_DELETE_ARRAY(pFrame->Name);
				pFrame->Name = new char[ nStrLen ];
				memset( pFrame->Name, 0, sizeof(char)*nStrLen );

				StringCchCat( pFrame->Name, nStrLen, pframeParent->Name );
				StringCchCat( pFrame->Name, nStrLen, szMesh );
			}
		}

		// make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
		if ( !pFrame->pMeshContainer->Name || !strlen(pFrame->pMeshContainer->Name) )
		{
			SAFE_DELETE_ARRAY(pFrame->pMeshContainer->Name);
			hr = AllocateName(pFrame->Name, &pFrame->pMeshContainer->Name);
			if (FAILED(hr))				return hr;   
		}
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupNameOnMeshContainer(pFrame->pFrameSibling);
        if (FAILED(hr))				return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupNameOnMeshContainer(pFrame->pFrameFirstChild);
        if (FAILED(hr))				return hr;
    }

    return S_OK;
}
