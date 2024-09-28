#pragma once

#include <vector>
#include <boost/tr1/memory.hpp>

#include "../TextureManager.h"
#include "./MaterialSkinDef.h"

struct DXMATERIALEX;
struct SMeshContainer;
struct SMATERIAL_PIECE;
struct SKINEFF_LEVEL_DATA;
struct SKIN_SPEC_SSS_INFO_REFERENCE;
class MaterialSkinMesh;
class DxClothColl;
class DxPhysXCloth;
class MaterialSkinMeshForCloth;

namespace physx
{
	struct PxClothCollisionSphere;
}


//////////////////////////////////////////////////////////////////////////
// EffectMaterialData 이 DxMultiMaterialSkinMesh 안에 있었지만,
// 공용으로 써야하는게 아닌 DxSkinChar의 DxCharPart 의 각각에 있어야 되서 옮기게 되었다.
// Opaque, HardAlpha, SoftAlpha 같이 정보는 유지되며 렌더순서가 바뀌어서, 원본참조로는 못쓰이게 되었다.
//////////////////////////////////////////////////////////////////////////
//
class DxMultiMaterialSkinMesh
{
private:
	enum TEX_TYPE_SKIN
	{
		TEX_TYPE_UNKNOWN,
		TEX_TYPE_OPAQUE,
		TEX_TYPE_HARDALPHA,
		TEX_TYPE_SOFTALPHA,
	};

private:
	std::vector<DWORD>									m_vecType;			// TEX_TYPE_SKIN
	//std::vector<DWORD>									m_vecMaterialID;	// TEX_TYPE_SKIN
	std::vector<std::tr1::shared_ptr<MaterialSkinMesh>>	m_vecMesh;

private:
	void SetTexType( DXMATERIALEX& sMaterialEX );
	void ChangeTexType( size_t unIndex, TextureResource& sTextureResource );

public:
	// CAllocateHierarchy::CreateMeshContainer 에서 시작. Material 과는 아직 연결 안된 상태.
	void Import_First( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pSrc, DXMATERIALEX* pMaterialEx, LPD3DXBONECOMBINATION pBoneComb );

	// MaterialSetting
	void SetMaterialSetting( LPDIRECT3DDEVICEQ pd3dDevice, const EffectMaterialData* pEffectMaterialData, int ipattr, DWORD dwThisAttribId );
	void SetMaterialSettingGhosting( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetMaterialSettingGlow( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetMaterialSettingShadow( LPDIRECT3DDEVICEQ pd3dDevice );



	//////////////////////////////////////////////////////////////////////////
	// Render Skinned
	void RenderOpaque
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX* pBoneMatrices,
		SMeshContainer *pMeshContainer,
		//DWORD dwNumPaletteEntries,
		//DWORD dwNumInfl, 
		DWORD dwThisAttribId, 
		SMATERIAL_PIECE& sMaterialSystem,
		const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		const D3DXMATRIX* pmatAttParentBone,
		const DxClothColl* pClothColl,
		const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
		float fAttParentBoneScale,
		float fAlpha,
		DxPhysXCloth* pPhysXCloth
	);

	void RenderHardAlpha
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX* pBoneMatrices,
		SMeshContainer *pMeshContainer,
		//DWORD dwNumPaletteEntries,
		//DWORD dwNumInfl, 
		SMATERIAL_PIECE& sMaterialSystem,
		const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		float fAlpha,
		const DxPhysXCloth* pPhysXCloth
	);

	void RenderSoftAlpha
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX* pBoneMatrices,
		SMeshContainer *pMeshContainer,
		//DWORD dwNumPaletteEntries,
		//DWORD dwNumInfl,
		//DXMATERIALEX* pMaterialEx, 
		SMATERIAL_PIECE& sMaterialSystem,
		const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		float fAlpha,
		const DxPhysXCloth* pPhysXCloth
	);

	BOOL RenderGhosting
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX* arrayBoneMatrices,
		SMeshContainer *pMeshContainer,
		SMATERIAL_PIECE& sMaterialSystem,
		const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		float fAlpha,
		const DxPhysXCloth* pPhysXCloth
	);

	void RenderGlow
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX* pBoneMatrices,
		DWORD dwNumPaletteEntries,
		DWORD dwNumInfl,
		DWORD dwThisAttribId, 
		SMATERIAL_PIECE& sMaterialSystem,
		LPDIRECT3DTEXTUREQ pGlowTex,
		const D3DXVECTOR4* pGlowColor,
		UINT unPass,
		const DxPhysXCloth* pPhysXCloth
	);

	void RenderShadow
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX* pBoneMatrices,
		DWORD dwNumPaletteEntries,
		DWORD dwNumInfl, 
		const D3DXMATRIX& matViewProj,
		SMATERIAL_PIECE& sMaterialSystem,
		BOOL bMaterialSystemWLD,
		const DxPhysXCloth* pPhysXCloth
	);


	//////////////////////////////////////////////////////////////////////////
	// Render Object
	void RenderObjectOpaque
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX& matWorld, 
		SMeshContainer *pMeshContainer,
		SMATERIAL_PIECE& sMaterialSystem,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		const float* arrayInvWeight,
		const D3DXMATRIX* pmatClothParentBone,
		const DxClothColl* pClothColl,
		const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
		float fAttParentBoneScale,
		float fAlpha,
		DxPhysXCloth* pPhysXCloth
	);

	void RenderObjectHardAlpha
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX& matWorld, 
		SMATERIAL_PIECE& sMaterialSystem,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		float fAlpha,
		const DxPhysXCloth* pPhysXCloth
	);

	void RenderObjectSoftAlpha
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX& matWorld,
		//DXMATERIALEX* pMaterialEx, 
		SMATERIAL_PIECE& sMaterialSystem,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		float fAlpha,
		const DxPhysXCloth* pPhysXCloth
	);

	BOOL RenderObjectGhosting
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX& matWorld,
		SMATERIAL_PIECE& sMaterialSystem,
		const EffectMaterialData* pEffectMaterialData,
		LPDIRECT3DTEXTUREQ pOverlayTex,
		float fAlpha,
		const DxPhysXCloth* pPhysXCloth
	);

	void RenderObjectGlow
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX& matWorld,
		SMATERIAL_PIECE& sMaterialSystem,
		LPDIRECT3DTEXTUREQ pGlowTex,
		const D3DXVECTOR4* pGlowColor,
		UINT unPass,
		const DxPhysXCloth* pPhysXCloth
	);

	void RenderObjectShadow
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		int ipattr,
		const D3DXMATRIX& matWorld,
		const D3DXMATRIX& matViewProj,
		SMATERIAL_PIECE& sMaterialSystem,
		BOOL bMaterialSystemWLD,
		const DxPhysXCloth* pPhysXCloth
	);

	void Export( std::vector<boost::shared_ptr<MaterialSkinMeshForCloth>>& vecMeshForCloth );

private:
	void CleanUp();

public:
	DxMultiMaterialSkinMesh();
	~DxMultiMaterialSkinMesh();
};