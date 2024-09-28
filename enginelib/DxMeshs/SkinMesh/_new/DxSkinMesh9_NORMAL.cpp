#include "pch.h"

#include "../../../../SigmaCore/File/SerialFile.h"
#include "../../../../SigmaCore/Util/Util.h"

#include "../../../DxEffect/Char/DxEffChar.h"
#include "../../../DxTools/DxRenderStates.h"
#include "../../../DxTools/Light/DxLightMan.h"
#include "../../../DxTools/RENDERPARAM.h"
#include "../../../DxTools/DxMethods.h"
#include "../../../Common/SubPath.h"
#include "../DxSkinPieceContainer.h"
#include "../DxSkeletonMaintain.h"
#include "../DxSkinMeshManager.h"
#include "../DxMaterial.h"
#include "./DxSkinVB_ATT.h"
#include "./DxSkinMeshContainer9.h"
#include "./DxSkinMesh9_HLSL.h"
#include "./DxSkinMesh9.h"

#include "DxSkinMesh9_NORMAL.h"

// ----------------------------------------------------------------------------
//#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define V(x)           { hr = x; }

DWORD DxSkinMesh9_NORMAL::VERSION = 0x0100;

DWORD	DxSkinMesh9_NORMAL::m_dwMaxVertexBlendMatrices = 0;



extern DWORD	SKINDRAWCOUNT;
extern BOOL		g_bCHAR_EDIT_PIECE_RENDER_FIXED;



DxSkinMesh9_NORMAL::DxSkinMesh9_NORMAL()
{
	StringCchCopy( m_szFileName, 32, _T("") );
	StringCchCopy( m_szSkinName, 32, _T("") );
	StringCchCopy( m_szSkeleton, 32, _T("") );
}

DxSkinMesh9_NORMAL::~DxSkinMesh9_NORMAL()
{
}

void DxSkinMesh9_NORMAL::CleanUp()
{

}

HRESULT DxSkinMesh9_NORMAL::StaticCreate (IDirect3DDevice9* pd3dDevice )
{
	DxSkinMesh9::StaticCreate( pd3dDevice );

	return S_OK;
}

void DxSkinMesh9_NORMAL::StaticResetDevice( IDirect3DDevice9* pd3dDevice )
{
	D3DCAPS9 d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );
	m_dwMaxVertexBlendMatrices = d3dCaps.MaxVertexBlendMatrices;

	DxSkinMesh9::StaticResetDevice( pd3dDevice );
}

void DxSkinMesh9_NORMAL::StaticLost()
{
	DxSkinMesh9::StaticLost();
}

void DxSkinMesh9_NORMAL::StaticDestroy ()
{
	DxSkinMesh9::StaticDestroy();
}

void DxSkinMesh9_NORMAL::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, 
										   const CHARSETTING& sSetting, 
										   const DxSkeletonMaintain* pSkeleton, 
										   const D3DXMATRIX& matStaticLinkBone,
										   const D3DXMATRIX* arrayBoneMatrices )
{
	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)sSetting.pMeshContainerBase;

	DWORD dwNumInfl(0);
	UINT dwNumAttributeGroups(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
	UINT iMaterial;
	UINT NumBlend;
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

	UINT iMatrixIndex;
	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	DWORD dwAlphaBlendEnable, dwAlphaOP;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		//if( pMeshContainer->emMethod == D3DNONINDEXED )
		{
			AttribIdPrev = UNUSED32; 
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());

			// Draw using default vtx processing of the device (typically HW)
			dwNumAttributeGroups = pMeshContainer->m_dwNumAttributeGroups;
			for( iAttrib = 0; iAttrib < dwNumAttributeGroups; ++iAttrib )
			{
				pBoneCur = &pBoneComb[iAttrib];
				dwThisAttribId = pBoneCur->AttribId;
				pMaterialEx = &pMeshContainer->m_pMaterialEx[dwThisAttribId];
				if( !pMaterialEx )	continue;

				// Note : 텍스쳐가 있을 경우와 없을 경우
				if( !pMaterialEx->m_textureRes.IsValid() )
				{
					// Note : 먼저 로드가 되었는지 살펴본다.
					CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
				}

				NumBlend = 0;
				dwNumInfl = pMeshContainer->m_dwNumInfl;
				for (DWORD i = 0; i < dwNumInfl; ++i)
				{
					if (pBoneCur->BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if( m_dwMaxVertexBlendMatrices >= NumBlend + 1 )
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						iMatrixIndex = pBoneCur->BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumInfl + i] );
						}
					}

					//if( bHeight )
					//{
					//	// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					//	for (DWORD i = 0; i < dwNumInfl; ++i)
					//	{
					//		iMatrixIndex = pBoneCur->BoneId[i];
					//		if (iMatrixIndex != UINT_MAX)
					//		{
					//			//matBoneMatrix_UP = *pMeshContainer->m_ppBoneMatrixPtrs[iMatrixIndex];

					//			matBoneMatrix_UP = pSkeleton->FindBone_Index( pMeshContainer->m_arrayBoneIndex[iMatrixIndex] )->m_matCombined;
					//			matBoneMatrix_UP._42 += m_fHeight;

					//			D3DXMatrixMultiply( &matTemp, &pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );

					//			pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
					//		}
					//	}
					//}
					//else
					//{
					//	// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					//	for (DWORD i = 0; i < dwNumInfl; ++i)
					//	{
					//		iMatrixIndex = pBoneCur->BoneId[i];
					//		if (iMatrixIndex != UINT_MAX)
					//		{
					//			//D3DXMatrixMultiply( &matTemp, &pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->m_ppBoneMatrixPtrs[iMatrixIndex] );
					//			D3DXMatrixMultiply
					//			( 
					//				&matTemp, 
					//				&pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex], 
					//				&pSkeleton->FindBone_Index( pMeshContainer->m_arrayBoneIndex[iMatrixIndex] )->m_matCombined
					//			);
					//			pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
					//		}
					//	}
					//}


					pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
					{
						pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D );
						//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
						AttribIdPrev = dwThisAttribId;
					}

					BOOL bDraw(FALSE);
					switch( sSetting.emRDOP )
					{
					case CTOP_BASE:
						//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
						//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
						bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && !sSetting.pmtrlSpecular[dwThisAttribId].IsType( EMCET_NULL ) );
						if( bDraw )
						{
							++SKINDRAWCOUNT;
							RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular, sSetting.m_r_apOverlayTex );
						}
						break;

					case CTOP_NEWTEX:
						++SKINDRAWCOUNT;
						RenderNewTex( pd3dDevice, pMeshContainer->MeshData.pMesh, iAttrib );
						break;
					};
				}
			}

			// If necessary, draw parts that HW could not handle using SW
			if (pMeshContainer->m_iAttributeSW < pMeshContainer->m_dwNumAttributeGroups)
			{
				AttribIdPrev = UNUSED32; 
				V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
				for (iAttrib = pMeshContainer->m_iAttributeSW; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
				{
					pBoneCur = &pBoneComb[iAttrib];
					dwThisAttribId = pBoneCur->AttribId;
					pMaterialEx = &pMeshContainer->m_pMaterialEx[dwThisAttribId];
					if( !pMaterialEx )	continue;

					// Note : 텍스쳐가 있을 경우와 없을 경우
					if( !pMaterialEx->m_textureRes.IsValid() )
					{
						// Note : 먼저 로드가 되었는지 살펴본다.
						CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
					}

					NumBlend = 0;
					dwNumInfl = pMeshContainer->m_dwNumInfl;
					for (DWORD i = 0; i < dwNumInfl; ++i)
					{
						if (pBoneCur->BoneId[i] != UINT_MAX)
						{
							NumBlend = i;
						}
					}

					if( m_dwMaxVertexBlendMatrices < NumBlend + 1)
					{
						// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
						for (DWORD i = 0; i < dwNumInfl; ++i)
						{
							iMatrixIndex = pBoneCur->BoneId[i];
							if (iMatrixIndex != UINT_MAX)
							{
								pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumInfl + i] );
							}
						}

						//// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
						//for (DWORD i = 0; i < dwNumInfl; ++i)
						//{
						//	iMatrixIndex = pBoneCur->BoneId[i];
						//	if (iMatrixIndex != UINT_MAX)
						//	{
						//		//matBoneMatrix_UP = *pMeshContainer->m_ppBoneMatrixPtrs[iMatrixIndex];

						//		matBoneMatrix_UP = pSkeleton->FindBone_Index( pMeshContainer->m_arrayBoneIndex[iMatrixIndex] )->m_matCombined;
						//		matBoneMatrix_UP._42 += m_fHeight;

						//		D3DXMatrixMultiply( &matTemp, &pMeshContainer->m_pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
						//		V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
						//	}
						//}

						V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );

						// lookup the material used for this subset of faces
						if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
						{
							V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
							//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
							AttribIdPrev = dwThisAttribId;
						}

						BOOL bDraw(FALSE);
						switch( sSetting.emRDOP )
						{
						case CTOP_BASE:
							//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
							//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
							bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && !sSetting.pmtrlSpecular[dwThisAttribId].IsType( EMCET_NULL ) );
							if( bDraw )
							{
								RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular, sSetting.m_r_apOverlayTex );
							}
							break;

						case CTOP_NEWTEX:
							RenderNewTex( pd3dDevice, pMeshContainer->MeshData.pMesh, iAttrib );
							break;
						};
					}
				}
				V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
			}
			V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0) );
		} // pMeshContainer->emMethod == D3DNONINDEXED End
		//else if( pMeshContainer->emMethod == D3DINDEXED || pMeshContainer->emMethod == D3DINDEXEDHLSLVS )
		//{
		//	ID3DXEffect* pEffect = DxSkinMesh9_HLSL::GetEffect();	// D3DX effect interface

		//	// first check for skinning
		//	if ( pEffect && pMeshContainer->pSkinInfo != NULL)
		//	{
		//		D3DXVECTOR4 vVector;
		//		D3DCOLORVALUE cDiffuse = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse;
		//		D3DCOLORVALUE cAmbient = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient;

		//		vVector.x = cDiffuse.r;
		//		vVector.y = cDiffuse.g;
		//		vVector.z = cDiffuse.b;
		//		vVector.w = cDiffuse.a;
		//		V( pEffect->SetVector( "MaterialDiffuse", &vVector ) );

		//		vVector.x = cAmbient.r;
		//		vVector.y = cAmbient.g;
		//		vVector.z = cAmbient.b;
		//		vVector.w = cAmbient.a;
		//		V( pEffect->SetVector( "MaterialAmbient", &vVector ) );

		//		D3DXMATRIX matViewProj, matView, matProj;
		//		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
		//		pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
		//		D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
		//		pEffect->SetMatrix( "mViewProj", &matViewProj );

		//		// Set Light for vertex shader
		//		D3DXVECTOR4 vLightDir( 0.0f, 1.0f, -1.0f, 0.0f );
		//		D3DXVec4Normalize( &vLightDir, &vLightDir );
		//		V( pEffect->SetVector( "lhtDir", &vLightDir) );

		//		if (pMeshContainer->UseSoftwareVP)
		//		{
		//			// If hw or pure hw vertex processing is forced, we can't render the
		//			// mesh, so just exit out.  Typical applications should create
		//			// a device with appropriate vertex processing capability for this
		//			// skinning method.
		//			if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		//				return;

		//			V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
		//		}

		//		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_sMeshDataHLSL.m_pBoneCombinationBuf->GetBufferPointer());
		//		for (iAttrib = 0; iAttrib < pMeshContainer->m_sMeshDataHLSL.m_NumAttributeGroups; ++iAttrib)
		//		{ 
		//			if( bHeight )
		//			{
		//				// first calculate all the world matrices
		//				for (DWORD iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
		//				{
		//					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
		//					if (iMatrixIndex != UINT_MAX)
		//					{
		//						//	char shadow map - 높이 조절.
		//						matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
		//						matBoneMatrix_UP._42 += m_fHeight;

		//						D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
		//						//D3DXMatrixMultiply(&m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
		//					}
		//				}
		//			}
		//			else
		//			{
		//				// first calculate all the world matrices
		//				for (DWORD iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
		//				{
		//					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
		//					if (iMatrixIndex != UINT_MAX)
		//					{
		//						D3DXMatrixMultiply(&g_pBoneMatrices[iPaletteEntry], &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
		//					}
		//				}
		//			}
		//			V( pEffect->SetMatrixArray( "mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries) );

		//			//// Sum of all ambient and emissive contribution
		//			//D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
		//			//D3DXCOLOR color2(.25, .25, .25, 1.0);
		//			//D3DXCOLOR ambEmm;
		//			//D3DXColorModulate(&ambEmm, &color1, &color2);
		//			//ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

		//			// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
		//			//		V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[pBoneComb[iAttrib].AttribId].pTexture ) );

		//			// Set CurNumBones to select the correct vertex shader for the number of bones
		//			V( pEffect->SetInt( "CurNumBones", pMeshContainer->m_sMeshDataHLSL.m_NumInfl -1) );

		//			// Start the effect now all parameters have been updated
		//			UINT numPasses;
		//			V( pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE ) ); 
		//			for( UINT iPass = 0; iPass < numPasses; iPass++ )
		//			{
		//				V( pEffect->BeginPass( iPass ) );

		//				// draw the subset with the current world matrix palette and material state
		//				//   V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );

		//				//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
		//				//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
		//				DWORD dwThisAttribId = pBoneComb[iAttrib].AttribId;
		//				BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && !sSetting.pmtrlSpecular[dwThisAttribId].IsType( EMCET_NULL ) );
		//				if ( bDraw )
		//				{
		//					++SKINDRAWCOUNT;
		//					RenderDefault ( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, sSetting.pmtrlPiece, sSetting.pmtrlSpecular );
		//				}

		//				V( pEffect->EndPass() );
		//			}

		//			V( pEffect->End() );

		//			V( pd3dDevice->SetVertexShader(NULL) );
		//		}

		//		// remember to reset back to hw vertex processing if software was required
		//		if (pMeshContainer->UseSoftwareVP)
		//		{
		//			V( pd3dDevice->SetSoftwareVertexProcessing(FALSE) );
		//		}
		//	}		
		//}
	}
	else  // standard mesh, just draw it after setting material properties
	{
		if( g_bCHAR_EDIT_PIECE_RENDER_FIXED )
		{
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity) );
		}
		else
		{
			//D3DXMATRIX matCombined;
			////D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppBoneMatrixPtrs[1], pMeshContainer->m_ppBoneMatrixPtrs[2] );
			//D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppStaticMatrixPtrs[1], &pSkeleton->FindBone_Index( pMeshContainer->m_arrayBoneIndex[0] )->m_matCombined );
			//D3DXMatrixMultiply ( &matCombined, pMeshContainer->m_ppStaticMatrixPtrs[0], &matCombined );

			//matCombined._42 += m_fHeight;
			//m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.

			V( pd3dDevice->SetTransform(D3DTS_WORLD, &matStaticLinkBone) );
		}

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			DXMATERIALEX* pMaterialEx = &pMeshContainer->m_pMaterialEx[iMaterial];
			if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->m_textureRes.IsValid() )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
			}

			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
			//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );

			//	Note :	bOriginDraw = TRUE - 조건이 안 맞더라도 최소 한번은 뿌린다.
			//			bOriginDraw = FALSE - 조건이 안 맞으면 뿌리지 않는다.
			BOOL bDraw = m_bOriginDraw || ( sSetting.pmtrlSpecular && !sSetting.pmtrlSpecular[iMaterial].IsType( EMCET_NULL ) );
			if ( bDraw )
			{
				RenderDefault ( pd3dDevice, pMeshContainer, iMaterial, iMaterial, sSetting.pmtrlPiece, sSetting.pmtrlSpecular, sSetting.m_r_apOverlayTex );
			}
		}
	}

	//// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
}

void DxSkinMesh9_NORMAL::DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
										  DXMATERIAL_CHAR_EFF* pmtrlSpecular, BOOL bWorldIdentity )
{
}
//
//void DxSkinMesh9_NORMAL::DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP )
//{
//	HRESULT hr;
//	SMeshContainer *pMeshContainer = (SMeshContainer*)pMeshContainerBase;
//
//	if( !pMeshContainer->m_pSlimMesh )				return;
//	if( !pMeshContainer->m_pSlimMesh->m_pScaleVB )	return;
//
//	DWORD dwNumInfl(0);
//	UINT dwNumAttributeGroups(0);
//	DWORD dwThisAttribId(0);
//	DXMATERIALEX* pMaterialEx(NULL);
//	UINT iMaterial;
//	UINT NumBlend;
//	UINT iAttrib;
//	DWORD AttribIdPrev;
//	LPD3DXBONECOMBINATION pBoneComb;
//	LPD3DXBONECOMBINATION pBoneCur;
//
//	UINT iMatrixIndex;
//	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;
//
//
//	// Note : 일단 미리 해 놓는다.
//	LPDIRECT3DINDEXBUFFER9	pIB;
//	pMeshContainer->MeshData.pMesh->GetIndexBuffer( &pIB );
//	pd3dDevice->SetIndices( pIB );
//
//
//	// first check for skinning
//	if (pMeshContainer->pSkinInfo != NULL)
//	{
//		AttribIdPrev = UNUSED32; 
//		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
//
//		// Draw using default vtx processing of the device (typically HW)
//		dwNumAttributeGroups = pMeshContainer->m_dwNumAttributeGroups;
//		for( iAttrib = 0; iAttrib < dwNumAttributeGroups; ++iAttrib )
//		{
//			pBoneCur = &pBoneComb[iAttrib];
//			dwThisAttribId = pBoneCur->AttribId;
//			pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
//			if( !pMaterialEx )	continue;
//
//			// Note : 텍스쳐가 있을 경우와 없을 경우
//			if( !pMaterialEx->m_textureRes.IsValid() )
//			{
//				// Note : 먼저 로드가 되었는지 살펴본다.
//				CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
//			}
//
//			NumBlend = 0;
//			dwNumInfl = pMeshContainer->NumInfl;
//			for (DWORD i = 0; i < dwNumInfl; ++i)
//			{
//				if (pBoneCur->BoneId[i] != UINT_MAX)
//				{
//					NumBlend = i;
//				}
//			}
//
//			if( m_dwMaxVertexBlendMatrices >= NumBlend + 1 )
//			{
//				// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
//				for (DWORD i = 0; i < dwNumInfl; ++i)
//				{
//					iMatrixIndex = pBoneCur->BoneId[i];
//					if (iMatrixIndex != UINT_MAX)
//					{
//						D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
//						pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
//					}
//				}
//
//
//				pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
//
//				// lookup the material used for this subset of faces
//				if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
//				{
//					pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D );
//					AttribIdPrev = dwThisAttribId;
//				}
//
//				++SKINDRAWCOUNT;
//
//				switch( emCTOP )
//				{
//				case CTOP_SHADOW:
//					RenderCartoon( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );
//					break;
//				case CTOP_EDGE:
//					RenderEdge( pd3dDevice, pMeshContainer, iAttrib );
//					break;
//				case CTOP_NEWTEX:
//					RenderNewTex( pd3dDevice, pMeshContainer, iAttrib );
//					break;
//				}
//			}
//		}
//
//		// If necessary, draw parts that HW could not handle using SW
//		if (pMeshContainer->iAttributeSW < pMeshContainer->m_dwNumAttributeGroups)
//		{
//			AttribIdPrev = UNUSED32; 
//			V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
//			for (iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
//			{
//				pBoneCur = &pBoneComb[iAttrib];
//				dwThisAttribId = pBoneCur->AttribId;
//				pMaterialEx = &pMeshContainer->pMaterialEx[dwThisAttribId];
//				if( !pMaterialEx )	continue;
//
//				// Note : 텍스쳐가 있을 경우와 없을 경우
//				if( !pMaterialEx->m_textureRes.IsValid() )
//				{
//					// Note : 먼저 로드가 되었는지 살펴본다.
//					CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
//				}
//
//				NumBlend = 0;
//				dwNumInfl = pMeshContainer->NumInfl;
//				for (DWORD i = 0; i < dwNumInfl; ++i)
//				{
//					if (pBoneCur->BoneId[i] != UINT_MAX)
//					{
//						NumBlend = i;
//					}
//				}
//
//				if( m_dwMaxVertexBlendMatrices < NumBlend + 1)
//				{
//					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
//					for (DWORD i = 0; i < dwNumInfl; ++i)
//					{
//						iMatrixIndex = pBoneCur->BoneId[i];
//						if (iMatrixIndex != UINT_MAX)
//						{
//							matBoneMatrix_UP = *pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex];
//							matBoneMatrix_UP._42 += m_fHeight;
//
//							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], &matBoneMatrix_UP );
//							V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
//						}
//					}
//
//					V( pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend) );
//
//					// lookup the material used for this subset of faces
//					if ((AttribIdPrev != dwThisAttribId) || (AttribIdPrev == UNUSED32))
//					{
//						V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
//						//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[dwThisAttribId].pTexture ) );
//						AttribIdPrev = dwThisAttribId;
//					}
//
//					switch( emCTOP )
//					{
//					case CTOP_SHADOW:
//						RenderCartoon( pd3dDevice, pMeshContainer, iAttrib, dwThisAttribId, pmtrlPiece );
//						break;
//					case CTOP_EDGE:
//						RenderEdge( pd3dDevice, pMeshContainer, iAttrib );
//						break;
//					case CTOP_NEWTEX:
//						RenderNewTex( pd3dDevice, pMeshContainer, iAttrib );
//						break;
//					}
//				}
//			}
//			V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
//		}
//
//		V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0) );
//	}
//	else  // standard mesh, just draw it after setting material properties
//	{
//		D3DXMATRIX matCombined;
//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[1], pMeshContainer->ppBoneMatrixPtrs[2] );
//		D3DXMatrixMultiply ( &matCombined, pMeshContainer->ppBoneMatrixPtrs[0], &matCombined );
//
//		matCombined._42 += m_fHeight;
//		m_matWorld = matCombined;		// 단독 Eff 에서 쓰일 것이다.
//
//		V( pd3dDevice->SetTransform(D3DTS_WORLD, &matCombined) );
//
//		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
//		{
//			DXMATERIALEX* pMaterialEx = &pMeshContainer->pMaterialEx[iMaterial];
//			if( !pMaterialEx )	continue;
//
//			// Note : 텍스쳐가 있을 경우와 없을 경우
//			if( !pMaterialEx->m_textureRes.IsValid() )
//			{
//				// Note : 먼저 로드가 되었는지 살펴본다.
//				CheckLoadTexture( pd3dDevice, pMeshContainer, iMaterial );
//			}
//
//			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
//			//	V( pd3dDevice->SetTexture( 0, pMeshContainer->pMaterialEx[iMaterial].pTexture ) );
//
//			switch( emCTOP )
//			{
//			case CTOP_SHADOW:
//				RenderCartoon( pd3dDevice, pMeshContainer, iMaterial, iMaterial, pmtrlPiece );
//				break;
//			case CTOP_EDGE:
//				RenderEdge( pd3dDevice, pMeshContainer, iMaterial );
//				break;
//			case CTOP_NEWTEX:
//				RenderNewTex( pd3dDevice, pMeshContainer, iMaterial );
//				break;
//			}
//		}
//	}
//}



void DxSkinMesh9_NORMAL::DrawMeshOVERLAY( IDirect3DDevice9 *pd3dDevice, 
										 const D3DXMATRIXA16* pBoneMatrices,
										 const CHARSETTING& sCharSetting, 
										 EFFCHAR_PROPERTY_OVERLAY_TEXTURE* sOverlayVal, 
										 ID3DXEffect* pEffect, 
										 DWORD dwOverMaterial, 
										 BOOL bWorldIdentity/* =FALSE */ )
{
	HRESULT hr;
	SMeshContainer *pMeshContainer = (SMeshContainer*)sCharSetting.pMeshContainerBase;

	DWORD dwNumInfl(0);
	UINT dwNumAttributeGroups(0);
	DWORD dwThisAttribId(0);
	DXMATERIALEX* pMaterialEx(NULL);
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;
	LPD3DXBONECOMBINATION pBoneCur;

	UINT iPaletteEntry;
	UINT iMatrixIndex;
	D3DXMATRIXA16 matTemp, matBoneMatrix_UP;

	DWORD dwAlphaBlendEnable, dwAlphaOP;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

	BOOL	bHeight(FALSE);	// 높이 변화가 있다./없다.
	if( m_fHeight!=0.f)	bHeight = TRUE;

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		D3DXMATRIX matView;
		pd3dDevice->GetTransform( D3DTS_VIEW, &matView );

		//if (pMeshContainer->UseSoftwareVP)
		//{
		//	// If hw or pure hw vertex processing is forced, we can't render the
		//	// mesh, so just exit out.  Typical applications should create
		//	// a device with appropriate vertex processing capability for this
		//	// skinning method.
		//	if( m_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		//		return;

		//	V( pd3dDevice->SetSoftwareVertexProcessing(TRUE) );
		//}


		AttribIdPrev = UNUSED32; 
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_sMeshDataHLSL.m_pBoneCombinationBuf->GetBufferPointer());

		// Draw using default vtx processing of the device (typically HW)
		dwNumAttributeGroups = pMeshContainer->m_sMeshDataHLSL.m_NumAttributeGroups;
		//for( iAttrib = 0; iAttrib < m_dwNumAttributeGroups; ++iAttrib )
		iAttrib = dwOverMaterial;
		{
			pBoneCur = &pBoneComb[iAttrib];
			dwThisAttribId = pBoneCur->AttribId;
			pMaterialEx = &pMeshContainer->m_pMaterialEx[dwThisAttribId];
			//if( !pMaterialEx )	continue;

			// Note : 텍스쳐가 있을 경우와 없을 경우
			if( !pMaterialEx->m_textureRes.IsValid() )
			{
				// Note : 먼저 로드가 되었는지 살펴본다.
				CheckLoadTexture( pd3dDevice, pMeshContainer, dwThisAttribId);
			}

			// Memo : onverlay mesh texture 설정.
			SMATERIAL_PIECE* pmtrlPiece = sCharSetting.pmtrlPiece;
			LPDIRECT3DTEXTUREQ	pMeshTexture(NULL);
			if( pmtrlPiece )
			{
				if ( pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture() )
					pMeshTexture = pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture();
			}
			if( !pMeshTexture )
				pMeshTexture = pMeshContainer->m_pMaterialEx[dwThisAttribId].m_textureRes.GetTexture();

			//if( dwThisAttribId == dwOverMaterial )
			if ( sOverlayVal[ dwThisAttribId ].m_bOverlay )
			{
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->m_dwNumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply( &g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
											&pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries+iPaletteEntry], 
											&matView );
					}
				}

				V( pEffect->SetMatrixArray( "mWorldMatrixArray", &g_pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries], pMeshContainer->m_dwNumPaletteEntries) );

				V( pEffect->SetInt( "CurNumBones", pMeshContainer->m_sMeshDataHLSL.m_NumInfl -1) );

				UINT numPasses;
				//V( pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESAMPLERSTATE|D3DXFX_DONOTSAVESHADERSTATE ) );//D3DXFX_DONOTSAVESTATE ) ); 
				pEffect->Begin( &numPasses, 0 );

				V( pEffect->SetTexture( "m_MeshTexture", pMeshTexture ) );

				for( UINT iPass = 0; iPass < numPasses; iPass++ )
				{
					V( pEffect->BeginPass( iPass ) );
					RenderNewTex( pd3dDevice, pMeshContainer->m_sMeshDataHLSL.m_MeshData.pMesh, iAttrib );
					V( pEffect->EndPass() );
				}

				V( pEffect->End() );

				V( pd3dDevice->SetVertexShader(NULL) );
				V( pd3dDevice->SetPixelShader(NULL) );
				V( pd3dDevice->SetTexture( 0, NULL ) );
			}
			else
			{
				
				ID3DXEffect* pSkinnedEffect = DxSkinMesh9_HLSL::GetEffect();	// D3DX effect interface

				if ( pSkinnedEffect )
				{
					V( pSkinnedEffect->SetMatrixArray( "mWorldMatrixArray", &pBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries], pMeshContainer->m_dwNumPaletteEntries) );
					V( pSkinnedEffect->SetInt( "CurNumBones", pMeshContainer->m_sMeshDataHLSL.m_NumInfl -1) );

					// Start the effect now all parameters have been updated
					UINT numPasses;
					V( pSkinnedEffect->Begin( &numPasses, 0 ) );

					V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[dwThisAttribId].MatD3D ) );
					V( pd3dDevice->SetTexture( 0, pMeshTexture ) );
					pSkinnedEffect->CommitChanges();

					for( UINT iPass = 0; iPass < numPasses; iPass++ )
					{
						V( pSkinnedEffect->BeginPass( iPass ) );
						RenderNewTex( pd3dDevice, pMeshContainer->m_sMeshDataHLSL.m_MeshData.pMesh, iAttrib );
						V( pSkinnedEffect->EndPass() );
					}

					V( pSkinnedEffect->End() );

					V( pd3dDevice->SetVertexShader(NULL) );
					V( pd3dDevice->SetTexture( 0, NULL ) );
				}
			}
		}
	}

	V( pd3dDevice->SetVertexShader(NULL) );
	V( pd3dDevice->SetPixelShader(NULL) );

	// Note : 캐릭터에서 변하는 Setting 들을 되돌려 놓는다.
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );
}
//
//void DxSkinMesh9_NORMAL::RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
//									   SMATERIAL_PIECE *pmtrlPiece )
//{
//	LPDIRECT3DTEXTUREQ	pTexture(NULL);
//	if( pmtrlPiece )
//	{
//		if ( pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture() )
//            pTexture = pmtrlPiece[dwThisAttribId].m_textureRes.GetTexture();
//	}
//	if( !pTexture )
//        pTexture = pmcMesh->pMaterialEx[dwThisAttribId].m_textureRes.GetTexture();
//
//	pd3dDevice->SetTexture( 0, pTexture );
//
//	pmcMesh->MeshData.pMesh->DrawSubset( ipattr );
//}

//void DxSkinMesh9_NORMAL::RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr )
//{
//	pd3dDevice->SetFVF( pmcMesh->m_pSlimMesh->m_dwScaleFVF );
//	pd3dDevice->SetStreamSource( 0, pmcMesh->m_pSlimMesh->m_pScaleVB, 0, pmcMesh->m_pSlimMesh->m_dwScaleSizeFVF );
//
//	const D3DXATTRIBUTERANGE* pAttribTable = pmcMesh->m_pSlimMesh->m_pAttribTable;
//	const D3DXATTRIBUTERANGE& sAttribTable = pAttribTable[ pAttribTable[ipattr].AttribId ];
//
//	pd3dDevice->DrawIndexedPrimitive
//		(
//		D3DPT_TRIANGLELIST, 
//		0, 
//		sAttribTable.VertexStart, 
//		sAttribTable.VertexCount, 
//		sAttribTable.FaceStart*3, 
//		sAttribTable.FaceCount 
//		);
//}

void DxSkinMesh9_NORMAL::RenderNewTex( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, int ipattr )
{
	pMesh->DrawSubset( ipattr );
}

void DxSkinMesh9_NORMAL::Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice )
{
	CleanUp();

	sc::SerialFile	SFile;
	std::string strName = DxSkinMeshManager::GetInstance().GetPath();
	strName += pName;

	strName = sc::util::ChangeExtName( strName.c_str(), _T("enm") );

	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if ( !bOpened )		return;

	DWORD dwVer;
	SFile >> dwVer;

	Load( SFile, pd3dDevice );

	SFile.CloseFile();
}

void DxSkinMesh9_NORMAL::Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice )
{
	DWORD dwCount(0L);
	SMeshContainer* pCur = m_pMeshContainerHead;
	while( pCur )
	{
		++dwCount;
		pCur = pCur->m_pGlobalNext;	
	}

	DWORD dwCountLOAD(0L);
	DWORD dwBufferSize(0L);
	SFile >> dwCountLOAD;
	SFile >> dwBufferSize;
	if( dwCount != dwCountLOAD )
	{
		SFile.SetOffSet( SFile.GetfTell()+dwBufferSize );
		return;
	}

	pCur = m_pMeshContainerHead;
	while( pCur )
	{
		pCur->LoadNORMAL( pd3dDevice, SFile );
		pCur = pCur->m_pGlobalNext;	
	}
}
