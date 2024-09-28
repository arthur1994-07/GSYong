#pragma once

#include <vector>
#include <boost/tr1/memory.hpp>

#include "../../PhysX/NSPhysX_Def.h"
#include "../TextureManager.h"

#include "./MaterialSkinDef.h"

struct Material;
struct MaterialSmall;
struct MATERIAL_SKIN_DATA;
struct DXMATERIALEX;
struct EffectMaterialData;
struct SKIN_SPEC_SSS_INFO_REFERENCE;
class DxClothColl;

namespace physx
{
	class PxCloth;
	struct PxClothCollisionSphere;
};


//////////////////////////////////////////////////////////////////////////
// DxCahrPart 에 들어가 있어서 개개로 동작된다.
class MaterialSkinMeshForCloth
{
private:
	//////////////////////////////////////////////////////////////////////////
	// Cloth 
	physx::PxCloth*				m_rCloth;
	LPD3DXMESH					m_pClothMesh;	// 천 시뮬레이션 렌더링. 정점 위치가 바뀌기 때문에 기본 Mesh 를 건들면 안된다. 천 시뮬레이션 On/Off 를 위함.
												// 그림자나 반사 등 렌더링의 이유로 공유하지 않고, 개개의 값을 가지고 있어야 한다.

	// CheckVersion_UpdateCloth
	// 버젼을 확인하고, UpdateCloth 로 해본다.
	// Opaque 에서 한번만 불려지도록 한다.
public:
	BOOL CheckVersionCreateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
									const D3DXMATRIX* arrayBoneMatrices,
									DWORD dwNumInfl,
									DWORD dwNumPaletteEntries,
									const SMATERIAL_PIECE& sMaterialSystem,
									const MaterialSmall& sMaterialSmall,
									const float* arrayInvWeight,
									const D3DXMATRIX* pmatAttParentBone,
									const DxClothColl* pClothColl,
									const D3DVERTEXELEMENT9* pELMT,
									const LPD3DXMESH pMesh,
									std::vector<LockParticle>& vecLockParticle,
									float fAttParentBoneScale );

	void UpdateCloth( const MATERIAL_SKIN_DATA& sMaterialData,
						DWORD dwNormalOffset,
						const std::vector<WORD>& vecClothIB,
						const D3DXMATRIX* pmatAttParentBone,
						const float* arrayInvWeight,
						const std::vector<LockParticle>& vecLockParticle,
						const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres );

	BOOL IsValid() const;

	void CleanUp();

	const LPD3DXMESH	GetClothMesh() const	{ return m_pClothMesh; }

public:
	MaterialSkinMeshForCloth();
	~MaterialSkinMeshForCloth();
};


//////////////////////////////////////////////////////////////////////////
// DxSkinPiece 것을 땡겨 쓰기 때문에 공유 된다.
class MaterialSkinMesh
{
private:
	DWORD				m_dwVer;				// Material 변경이 있으면 버젼이 올라가서 Mesh를 새로 업데이트 한다.
	LPD3DXMESH			m_pMesh;				// 기본적인 렌더링 시
	DXMATERIALEX*		m_rMaterialEX;			// 기본적인 텍스쳐 정보. <참조만한다>
	MATERIAL_SKIN_DATA	m_sMaterialData;		// Material 정보
	float				m_fTexDiffColor;		// MaterialSkinMesh::SetMaterialSetting 에서 m_vecTexDiffColor 을 참조로 값이 저장된다.
	LPDIRECT3DTEXTUREQ	m_rTexUpDownTexture;	// MaterialSkinMesh::SetMaterialSetting 에서 m_vecTexUpDownTexture 을 참조로 값이 저장된다.

	//////////////////////////////////////////////////////////////////////////
	// Cloth 공유되는 값.
	std::vector<WORD>			m_vecClothIB;			// 속도를 위해 Index 를 미리 복사해놓는다.
	std::vector<LockParticle>	m_vecLockParticle;		// Bone 에 영향받은 Position 을 미리 얻어놓는다.
	DWORD						m_dwNormalOffset;		// Normal Offset

public:
	// Insert
	BOOL Insert( LPDIRECT3DDEVICEQ pd3dDevice, 
					const LPD3DXMESH pMeshSRC, 
					DWORD dwVertexSize,
					LPD3DVERTEXELEMENT9 pDecl,
					DXMATERIALEX& sMaterialEX );


	// MaterialSetting
	void SetMaterialSetting( LPDIRECT3DDEVICEQ pd3dDevice, const EffectMaterialData* pEffectMaterialData, DWORD dwThisAttribId );
	void SetMaterialSettingGhosting( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetMaterialSettingGlow( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetMaterialSettingShadow( LPDIRECT3DDEVICEQ pd3dDevice );


private:
	BOOL CheckVersion_UpdateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
									const D3DXMATRIX* arrayBoneMatrices,
									DWORD dwNumInfl,
									DWORD dwNumPaletteEntries,
									const SMATERIAL_PIECE& sMaterialSystem,
									const MaterialSmall& sMaterialSmall,
									const float* arrayInvWeight,
									const D3DXMATRIX* pmatAttParentBone,
									const DxClothColl* pClothColl,
									const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
									MaterialSkinMeshForCloth* pMeshForCloth,
									float fAttParentBoneScale );

public:
	BOOL CheckVersionSkin_UpdateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
										const D3DXMATRIX* arrayBoneMatrices,
										DWORD dwNumInfl,
										DWORD dwNumPaletteEntries,
										const SMATERIAL_PIECE& sMaterialSystem,
										const float* arrayInvWeight,
										const D3DXMATRIX* pmatAttParentBone,
										const DxClothColl* pClothColl,
										const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
										MaterialSkinMeshForCloth* pMeshForCloth,
										float fAttParentBoneScale );

	BOOL CheckVersionObject_UpdateCloth( LPDIRECT3DDEVICEQ pd3dDevice, 
										const SMATERIAL_PIECE& sMaterialSystem,
										const float* arrayInvWeight,
										const D3DXMATRIX* pmatAttParentBone,
										const DxClothColl* pClothColl,
										const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
										MaterialSkinMeshForCloth* pMeshForCloth,
										float fAttParentBoneScale );


	// Render
	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				const D3DXMATRIX* pBoneMatrices,
				SMeshContainer *pMeshContainer,
				SMATERIAL_PIECE& sMaterialSystem,
				const EffectMaterialData* pEffectMaterialData,
				const SKIN_SPEC_SSS_INFO_REFERENCE* pSkinInfo,
				LPDIRECT3DTEXTUREQ pOverlayTex,
				float fAlpha,
				const MaterialSkinMeshForCloth* pMeshForCloth );

	BOOL RenderObject( LPDIRECT3DDEVICEQ pd3dDevice, 
						const D3DXMATRIX& matWorld, 
						SMATERIAL_PIECE& sMaterialSystem,
						const EffectMaterialData* pEffectMaterialData,
						LPDIRECT3DTEXTUREQ pOverlayTex,
						float fAlpha,
						const MaterialSkinMeshForCloth* pMeshForCloth );

	BOOL RenderGhosting_Skin( LPDIRECT3DDEVICEQ pd3dDevice, 
							const D3DXMATRIX* arrayBoneMatrices,
							SMeshContainer *pMeshContainer,
							SMATERIAL_PIECE& sMaterialSystem,
							const EffectMaterialData* pEffectMaterialData,
							const SKIN_SPEC_SSS_INFO_REFERENCE* pSkinInfo,
							LPDIRECT3DTEXTUREQ pOverlayTex,
							float fAlpha );

	BOOL RenderGhosting_Object( LPDIRECT3DDEVICEQ pd3dDevice,
								const D3DXMATRIX& matWorld, 
								SMATERIAL_PIECE& sMaterialSystem,
								const EffectMaterialData* pEffectMaterialData,
								LPDIRECT3DTEXTUREQ pOverlayTex,
								float fAlpha );

	BOOL RenderGlow_Skin( LPDIRECT3DDEVICEQ pd3dDevice, 
							const D3DXMATRIX* pBoneMatrices,
							DWORD dwNumPaletteEntries,
							DWORD dwNumInfl, 
							SMATERIAL_PIECE& sMaterialSystem,
							const EffectMaterialData* pEffectMaterialData,
							const SKIN_SPEC_SSS_INFO_REFERENCE* pSkinInfo,
							LPDIRECT3DTEXTUREQ pGlowTex,
							const D3DXVECTOR4* pGlowColor,
							UINT unPass,
							const MaterialSkinMeshForCloth* pMeshForCloth );

	BOOL RenderGlow_Object( LPDIRECT3DDEVICEQ pd3dDevice, 
							const D3DXMATRIX& matWorld, 
							SMATERIAL_PIECE& sMaterialSystem,
							const EffectMaterialData* pEffectMaterialData,
							LPDIRECT3DTEXTUREQ pGlowTex,
							const D3DXVECTOR4* pGlowColor,
							UINT unPass,
							const MaterialSkinMeshForCloth* pMeshForCloth );

	BOOL RenderShadow_Skin( LPDIRECT3DDEVICEQ pd3dDevice, 
							const D3DXMATRIX* pBoneMatrices,
							DWORD dwNumPaletteEntries,
							DWORD dwNumInfl,
							const D3DXMATRIX& matViewProj,
							SMATERIAL_PIECE& sMaterialSystem,
							BOOL bMaterialSystemWLD,
							BOOL bOpaqueTex,
							const MaterialSkinMeshForCloth* pMeshForCloth );

	BOOL RenderShadow_Object( LPDIRECT3DDEVICEQ pd3dDevice,
								const D3DXMATRIX& matWorld, 
								const D3DXMATRIX& matViewProj,
								SMATERIAL_PIECE& sMaterialSystem,
								BOOL bMaterialSystemWLD,
								BOOL bOpaqueTex,
								const MaterialSkinMeshForCloth* pMeshForCloth );

private:
	ID3DXEffect* SetCommonFX( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE& sMaterialSystem, const EffectMaterialData* pEffectMaterialData );
	void RenderCommon( ID3DXEffect* pFX, LPD3DXMESH pMesh );
	void RenderCommonPass( ID3DXEffect* pFX, LPD3DXMESH pMesh, UINT unPass );

public:
	MaterialSkinMesh();
	~MaterialSkinMesh();
};

namespace NSMATERIALSKINMESH
{
	extern int g_nCreateCount;	// CharEdit 에서 확인용 데이터

	//BOOL CreateMesh
	//( 
	//	LPDIRECT3DDEVICEQ pd3dDevice, 
	//	const MaterialSmall* pMaterialSmall, 
	//	const LPD3DXMESH pSrcMesh, 
	//	LPD3DXMESH& pMeshOUT 
	//);

	BOOL CreateMesh
	( 
		LPDIRECT3DDEVICEQ pd3dDevice, 
		const MaterialSmall* pMaterialSmall, 
		LPD3DXMESH& pMesh_IN_OUT 
	);
};