#include "pch.h"


#include "../../../Common/SubPath.h"
#include "../../../../SigmaCore/File/SerialFile.h"
#include "../../../../SigmaCore/Util/Util.h"
#include "../../../DxTools/DxMethods.h"

#include "../../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../../DxTools/Light/DxLightMan.h"
#include "../../../DxTools/DxRenderStates.h"
#include "../../../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../../DxEffect/Char/DxEffCharHLSL.h"

#include "../DxMaterial.h"
#include "../DxSkinDefine.h"
#include "../DxSkeletonMaintain.h"
#include "../DxSkinMeshManager.h"
#include "../DxSkinPieceContainer.h"
#include "./DxSkinVB_ATT.h"
#include "./DxSkinMeshContainer9.h"
#include "./DxSkinMesh9.h"

#include "DxSkinMesh9_Material.h"

// ----------------------------------------------------------------------------
//#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD DxSkinMesh9_Material::VERSION = 0x0100;

DxSkinMesh9_Material::DxSkinMesh9_Material()
{
	StringCchCopy( m_szFileName, 32, _T("") );
	StringCchCopy( m_szSkinName, 32, _T("") );
	StringCchCopy( m_szSkeleton, 32, _T("") );
}

DxSkinMesh9_Material::~DxSkinMesh9_Material()
{
}

void DxSkinMesh9_Material::CleanUp()
{
}

HRESULT DxSkinMesh9_Material::StaticCreate (IDirect3DDevice9* pd3dDevice )
{
	DxSkinMesh9::StaticCreate( pd3dDevice );

	return S_OK;
}

HRESULT DxSkinMesh9_Material::StaticResetDevice( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr(S_OK);

	DxSkinMesh9::StaticResetDevice( pd3dDevice );

	return S_OK;
}

void DxSkinMesh9_Material::StaticLost()
{
	DxSkinMesh9::StaticLost();
}

void DxSkinMesh9_Material::StaticDestroy ()
{
	//SAFE_DELETE_ARRAY( g_pBoneMatrices );

	DxSkinMesh9::StaticDestroy();
}

void DxSkinMesh9_Material::RenderOpaque( IDirect3DDevice9 *pd3dDevice, 
										SMeshContainer *pMeshContainer,
										const D3DXMATRIX* arrayBoneMatrices,
										SMATERIAL_PIECE* arrayMaterialSystem, 
										const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
										const EffectMaterialData* pEffectMaterialData,
										LPDIRECT3DTEXTUREQ apOverlayTex[],
										const D3DXMATRIX* pmatAttParentBone,
										const DxClothColl* pClothColl,
										const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
										float fAlpha,
										const D3DXMATRIX& matStaticLinkBone,
										DxPhysXCloth* pPhysXCloth,
										float fAttParentBoneScale )
{
	DWORD dwThisAttribId(0);
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	D3DXMATRIX matTemp, matBoneMatrix_UP;

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
		{
			pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSetting( pd3dDevice, pEffectMaterialData, iAttrib, pBoneComb[iAttrib].AttribId );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderOpaque
			( 
				pd3dDevice, 
				iAttrib, 
				&arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries],
				pMeshContainer,
				pBoneComb[iAttrib].AttribId,
				arrayMaterialSystem[pBoneComb[iAttrib].AttribId],
				sSkinInfo,
				pEffectMaterialData,
				apOverlayTex[pBoneComb[iAttrib].AttribId],
				pmatAttParentBone,
				pClothColl,
				pvecClothCollSpheres,
				fAttParentBoneScale,
				fAlpha,
				pPhysXCloth
			);
		}
	}
	else
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSetting( pd3dDevice, pEffectMaterialData, iMaterial, iMaterial );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderObjectOpaque
			( 
				pd3dDevice, 
				iMaterial, 
				matStaticLinkBone,
				pMeshContainer,
				arrayMaterialSystem[iMaterial],
				pEffectMaterialData,
				apOverlayTex[iMaterial],
				pMeshContainer->m_arrayfInvWeight,
				pmatAttParentBone,
				pClothColl,
				pvecClothCollSpheres,
				fAttParentBoneScale,
				fAlpha,
				pPhysXCloth
			);
		}
	}
}

void DxSkinMesh9_Material::RenderHardAlpha( IDirect3DDevice9 *pd3dDevice, 
										   SMeshContainer *pMeshContainer,
										   const D3DXMATRIX* arrayBoneMatrices,
										   SMATERIAL_PIECE* arrayMaterialSystem, 
										   const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
										   const EffectMaterialData* pEffectMaterialData,
										   LPDIRECT3DTEXTUREQ apOverlayTex[],
										   float fAlpha,
										   const D3DXMATRIX& matStaticLinkBone,
										   const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwThisAttribId(0);
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
		{
			// RenderHardAlpha 가 RenderOpaque 바로 다음에 하기 때문에 셋팅할 필요는 없다.
			//pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSetting( pd3dDevice, pEffectMaterialData, iAttrib, pBoneComb[iAttrib].AttribId );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderHardAlpha
			( 
				pd3dDevice, 
				iAttrib, 
				&arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries],
				pMeshContainer,
				arrayMaterialSystem[pBoneComb[iAttrib].AttribId],
				sSkinInfo,
				pEffectMaterialData,
				apOverlayTex[pBoneComb[iAttrib].AttribId],
				fAlpha,
				pPhysXCloth
			);
		}
	}
	else
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			// RenderHardAlpha 가 RenderOpaque 바로 다음에 하기 때문에 셋팅할 필요는 없다.
			//pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSetting( pd3dDevice, pEffectMaterialData, iMaterial, iMaterial );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderObjectHardAlpha
			( 
				pd3dDevice, 
				iMaterial, 
				matStaticLinkBone,
				arrayMaterialSystem[iMaterial],
				pEffectMaterialData,
				apOverlayTex[iMaterial],
				fAlpha,
				pPhysXCloth
			);
		}
	}
}

void DxSkinMesh9_Material::RenderSoftAlpha( IDirect3DDevice9 *pd3dDevice, 
										   SMeshContainer *pMeshContainer,
										   const D3DXMATRIX* arrayBoneMatrices,
										   SMATERIAL_PIECE* arrayMaterialSystem, 
										   const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
										   const EffectMaterialData* pEffectMaterialData,
										   LPDIRECT3DTEXTUREQ apOverlayTex[],
										   float fAlpha,
										   const D3DXMATRIX& matStaticLinkBone,
										   const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwThisAttribId(0);
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
		{
			pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSetting( pd3dDevice, pEffectMaterialData, iAttrib, pBoneComb[iAttrib].AttribId );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderSoftAlpha
			( 
				pd3dDevice, 
				iAttrib, 
				&arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries],
				pMeshContainer,
				arrayMaterialSystem[pBoneComb[iAttrib].AttribId],
				sSkinInfo,
				pEffectMaterialData,
				apOverlayTex[pBoneComb[iAttrib].AttribId],
				fAlpha,
				pPhysXCloth
			);
		}
	}
	else
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSetting( pd3dDevice, pEffectMaterialData, iMaterial, iMaterial );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderObjectSoftAlpha
			( 
				pd3dDevice, 
				iMaterial, 
				matStaticLinkBone,
				arrayMaterialSystem[iMaterial],
				pEffectMaterialData,
				apOverlayTex[iMaterial],
				fAlpha,
				pPhysXCloth
			);
		}
	}
}

BOOL DxSkinMesh9_Material::RenderGhosting( IDirect3DDevice9 *pd3dDevice, 
										   SMeshContainer *pMeshContainer,
										   const D3DXMATRIX* arrayBoneMatrices,
										   SMATERIAL_PIECE* arrayMaterialSystem, 
										   const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
										   const EffectMaterialData* pEffectMaterialData,
										   LPDIRECT3DTEXTUREQ apOverlayTex[],
										   float fAlpha,
										   const D3DXMATRIX& matStaticLinkBone,
										   const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwThisAttribId(0);
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;
	BOOL bResult(TRUE);

	pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSettingGhosting( pd3dDevice );

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
		{
			bResult = pMeshContainer->m_spMultiMaterialSkinMesh->RenderGhosting
			( 
				pd3dDevice, 
				iAttrib, 
				&arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries],
				pMeshContainer,
				arrayMaterialSystem[pBoneComb[iAttrib].AttribId],
				sSkinInfo,
				pEffectMaterialData,
				apOverlayTex[pBoneComb[iAttrib].AttribId],
				fAlpha,
				pPhysXCloth
			);

			if ( !bResult )
				return FALSE;
		}
	}
	else
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			bResult = pMeshContainer->m_spMultiMaterialSkinMesh->RenderObjectGhosting
			( 
				pd3dDevice, 
				iMaterial, 
				matStaticLinkBone,
				arrayMaterialSystem[iMaterial],
				pEffectMaterialData,
				apOverlayTex[iMaterial],
				fAlpha,
				pPhysXCloth
			);
		}

		if ( !bResult )
			return FALSE;
	}

	return bResult;
}

void DxSkinMesh9_Material::RenderMaterialGlow( IDirect3DDevice9 *pd3dDevice, 
												SMeshContainer *pMeshContainer,
												const D3DXMATRIX* arrayBoneMatrices,
												SMATERIAL_PIECE* arrayMaterialSystem,
												DXMATERIAL_CHAR_EFF* arrayMaterialPiece,
												UINT unPass,
												const D3DXMATRIX& matStaticLinkBone,
												const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwThisAttribId(0);
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	D3DXMATRIX matTemp, matBoneMatrix_UP;

	pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSettingGlow( pd3dDevice );

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
		{
			if ( !arrayMaterialPiece[pBoneComb[iAttrib].AttribId].IsType( EMCET_BASE ) )
				continue;

			D3DXVECTOR4 vColor( arrayMaterialPiece[pBoneComb[iAttrib].AttribId].d3dMaterial.Diffuse.r,
								arrayMaterialPiece[pBoneComb[iAttrib].AttribId].d3dMaterial.Diffuse.g,
								arrayMaterialPiece[pBoneComb[iAttrib].AttribId].d3dMaterial.Diffuse.b,
								arrayMaterialPiece[pBoneComb[iAttrib].AttribId].d3dMaterial.Diffuse.a );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderGlow
			( 
				pd3dDevice, 
				iAttrib, 
				&arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries],
				pMeshContainer->m_dwNumPaletteEntries, 
				pMeshContainer->m_dwNumInfl,
				pBoneComb[iAttrib].AttribId,
				arrayMaterialSystem[pBoneComb[iAttrib].AttribId],
				arrayMaterialPiece[pBoneComb[iAttrib].AttribId].GetTexture(),
				&vColor,
				unPass,
				pPhysXCloth
			);
		}
	}
	else
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			if ( !arrayMaterialPiece[iMaterial].IsType( EMCET_BASE ) )
				continue;

			D3DXVECTOR4 vColor( arrayMaterialPiece[iMaterial].d3dMaterial.Diffuse.r,
								arrayMaterialPiece[iMaterial].d3dMaterial.Diffuse.g,
								arrayMaterialPiece[iMaterial].d3dMaterial.Diffuse.b,
								arrayMaterialPiece[iMaterial].d3dMaterial.Diffuse.a );

			pMeshContainer->m_spMultiMaterialSkinMesh->RenderObjectGlow
			( 
				pd3dDevice, 
				iMaterial, 
				matStaticLinkBone,
				arrayMaterialSystem[iMaterial],
				arrayMaterialPiece[iMaterial].GetTexture(),
				&vColor,
				unPass,
				pPhysXCloth

			);
		}
	}
}

void DxSkinMesh9_Material::RenderMaterialShadow( IDirect3DDevice9 *pd3dDevice,
												SMeshContainer *pMeshContainer,
												const D3DXMATRIX* arrayBoneMatrices,
												SMATERIAL_PIECE* arrayMaterialSystem,
												const D3DXMATRIX& matViewProj,
												BOOL bMaterialSystemWLD,
												const D3DXMATRIX& matStaticLinkBone,
												const DxPhysXCloth* pPhysXCloth )
{
	DWORD dwThisAttribId(0);
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	D3DXMATRIX matTemp, matBoneMatrix_UP;

	pMeshContainer->m_spMultiMaterialSkinMesh->SetMaterialSettingShadow( pd3dDevice );

	// first check for skinning
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->m_dwNumAttributeGroups; ++iAttrib)
		{
			pMeshContainer->m_spMultiMaterialSkinMesh->RenderShadow
			( 
				pd3dDevice, 
				iAttrib, 
				&arrayBoneMatrices[iAttrib*pMeshContainer->m_dwNumPaletteEntries],
				pMeshContainer->m_dwNumPaletteEntries, 
				pMeshContainer->m_dwNumInfl, 
				matViewProj,
				arrayMaterialSystem[pBoneComb[iAttrib].AttribId],
				bMaterialSystemWLD,
				pPhysXCloth
			);
		}

	}
	else
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			pMeshContainer->m_spMultiMaterialSkinMesh->RenderObjectShadow
			( 
				pd3dDevice, 
				iMaterial, 
				matStaticLinkBone, 
				matViewProj,
				arrayMaterialSystem[iMaterial],
				bMaterialSystemWLD,
				pPhysXCloth
			);
		}
	}
}