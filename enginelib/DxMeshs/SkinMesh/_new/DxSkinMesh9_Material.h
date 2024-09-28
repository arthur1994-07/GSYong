#pragma once

namespace sc {
	class SerialFile;
}

namespace physx {
	struct PxClothCollisionSphere;
}

struct SKIN_SPEC_SSS_INFO_REFERENCE;
struct EffectMaterialData;
class DxClothColl;
class DxPhysXCloth;
class DxSkeletonMaintain;

#include "../../../DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../../../DxMeshs/SkinMesh/DxMaterialHLSL.h"


//--------------------------------------------------------------------------------------
// Name: class DxSkinMesh9_Material
// Desc: 
//--------------------------------------------------------------------------------------
class DxSkinMesh9_Material : public DxSkinMesh9
{
public:
	static DWORD	VERSION;

protected:
	TCHAR			m_szFileName[32];			// 파일 이름.
	TCHAR			m_szSkinName[32];		// Skin xFile 이름.
	TCHAR			m_szSkeleton[32];		// Skeleton xFile 이름.

public:
	static HRESULT	StaticCreate (IDirect3DDevice9* pd3dDevice );
	static HRESULT StaticResetDevice( IDirect3DDevice9* pd3dDevice );
	static void StaticLost();
	static void StaticDestroy();

public:
	void RenderOpaque( IDirect3DDevice9 *pd3dDevice, 
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
						float fAttParentBoneScale );

	void RenderHardAlpha( IDirect3DDevice9 *pd3dDevice, 
							SMeshContainer *pMeshContainer,
							const D3DXMATRIX* arrayBoneMatrices,
							SMATERIAL_PIECE* arrayMaterialSystem, 
							const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
							const EffectMaterialData* pEffectMaterialData,
							LPDIRECT3DTEXTUREQ apOverlayTex[],
							float fAlpha,
							const D3DXMATRIX& matStaticLinkBone,
							const DxPhysXCloth* pPhysXCloth );

	void RenderSoftAlpha( IDirect3DDevice9 *pd3dDevice, 
							SMeshContainer *pMeshContainer,
							const D3DXMATRIX* arrayBoneMatrices,
							SMATERIAL_PIECE* arrayMaterialSystem, 
							const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
							const EffectMaterialData* pEffectMaterialData,
							LPDIRECT3DTEXTUREQ apOverlayTex[],
							float fAlpha,
							const D3DXMATRIX& matStaticLinkBone,
							const DxPhysXCloth* pPhysXCloth );

	BOOL RenderGhosting( IDirect3DDevice9 *pd3dDevice, 
							SMeshContainer *pMeshContainer,
							const D3DXMATRIX* arrayBoneMatrices,
							SMATERIAL_PIECE* arrayMaterialSystem, 
							const SKIN_SPEC_SSS_INFO_REFERENCE& sSkinInfo,
							const EffectMaterialData* pEffectMaterialData,
							LPDIRECT3DTEXTUREQ apOverlayTex[],
							float fAlpha,
							const D3DXMATRIX& matStaticLinkBone,
							const DxPhysXCloth* pPhysXCloth );

	void RenderMaterialGlow( IDirect3DDevice9 *pd3dDevice, 
								SMeshContainer *pMeshContainer,
								const D3DXMATRIX* arrayBoneMatrices,
								SMATERIAL_PIECE* arrayMaterialSystem,
								DXMATERIAL_CHAR_EFF* arrayMaterialPiece,
								UINT unPass,								// 0 - 기본, 1 - GlowInLevel
								const D3DXMATRIX& matStaticLinkBone,
								const DxPhysXCloth* pPhysXCloth );	

	void RenderMaterialShadow( IDirect3DDevice9 *pd3dDevice, 
								SMeshContainer *pMeshContainer,
								const D3DXMATRIX* arrayBoneMatrices,
								SMATERIAL_PIECE* arrayMaterialSystem,
								const D3DXMATRIX& matViewProj,
								BOOL bMaterialSystemWLD,
								const D3DXMATRIX& matStaticLinkBone,
								const DxPhysXCloth* pPhysXCloth );


	//////////////////////////////////////////////////////////////////////////
	// 이곳에서는 사용하지 않는다.
public:
	//virtual void DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sCharSetting, const DxSkeletonMaintain* pSkeleton, const D3DXMATRIX& matStaticLinkBone ) {}
	virtual void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
									DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE ) {}
	virtual void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice ) {}
	virtual void Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice ) {}



private:
	void CleanUp();

public:
	DxSkinMesh9_Material();
	virtual ~DxSkinMesh9_Material();
};