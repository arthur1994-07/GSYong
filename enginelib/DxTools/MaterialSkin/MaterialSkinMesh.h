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
// DxCahrPart �� �� �־ ������ ���۵ȴ�.
class MaterialSkinMeshForCloth
{
private:
	//////////////////////////////////////////////////////////////////////////
	// Cloth 
	physx::PxCloth*				m_rCloth;
	LPD3DXMESH					m_pClothMesh;	// õ �ùķ��̼� ������. ���� ��ġ�� �ٲ�� ������ �⺻ Mesh �� �ǵ�� �ȵȴ�. õ �ùķ��̼� On/Off �� ����.
												// �׸��ڳ� �ݻ� �� �������� ������ �������� �ʰ�, ������ ���� ������ �־�� �Ѵ�.

	// CheckVersion_UpdateCloth
	// ������ Ȯ���ϰ�, UpdateCloth �� �غ���.
	// Opaque ���� �ѹ��� �ҷ������� �Ѵ�.
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
// DxSkinPiece ���� ���� ���� ������ ���� �ȴ�.
class MaterialSkinMesh
{
private:
	DWORD				m_dwVer;				// Material ������ ������ ������ �ö󰡼� Mesh�� ���� ������Ʈ �Ѵ�.
	LPD3DXMESH			m_pMesh;				// �⺻���� ������ ��
	DXMATERIALEX*		m_rMaterialEX;			// �⺻���� �ؽ��� ����. <�������Ѵ�>
	MATERIAL_SKIN_DATA	m_sMaterialData;		// Material ����
	float				m_fTexDiffColor;		// MaterialSkinMesh::SetMaterialSetting ���� m_vecTexDiffColor �� ������ ���� ����ȴ�.
	LPDIRECT3DTEXTUREQ	m_rTexUpDownTexture;	// MaterialSkinMesh::SetMaterialSetting ���� m_vecTexUpDownTexture �� ������ ���� ����ȴ�.

	//////////////////////////////////////////////////////////////////////////
	// Cloth �����Ǵ� ��.
	std::vector<WORD>			m_vecClothIB;			// �ӵ��� ���� Index �� �̸� �����س��´�.
	std::vector<LockParticle>	m_vecLockParticle;		// Bone �� ������� Position �� �̸� �����´�.
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
	extern int g_nCreateCount;	// CharEdit ���� Ȯ�ο� ������

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