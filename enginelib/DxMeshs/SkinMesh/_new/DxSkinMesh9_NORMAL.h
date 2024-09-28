#pragma once

namespace sc {
    class SerialFile;
}
class DxMaterialHLSL;
class DxSkeletonMaintain;

#include "./DxSkinMesh9.h"

//--------------------------------------------------------------------------------------
// Name: class DxSkinMesh9_NORMAL
// Desc: 
//--------------------------------------------------------------------------------------
class DxSkinMesh9_NORMAL : public DxSkinMesh9
{
public:
	static DWORD		VERSION;

protected:
	TCHAR			m_szFileName[32];		// 파일 이름.
	TCHAR			m_szSkinName[32];		// Skin xFile 이름.
	TCHAR			m_szSkeleton[32];		// Skeleton xFile 이름.

public:
	static		DWORD	m_dwMaxVertexBlendMatrices;

public:
	static HRESULT	StaticCreate (IDirect3DDevice9* pd3dDevice );
	static void StaticResetDevice( IDirect3DDevice9* pd3dDevice );
	static void StaticLost();
	static void StaticDestroy();

public:
	virtual void DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, 
									const CHARSETTING& sCharSetting, 
									const DxSkeletonMaintain* pSkeleton, 
									const D3DXMATRIX& matStaticLinkBone,
									const D3DXMATRIX* arrayBoneMatrices );

	virtual void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE );
	//void DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP );

	void PrepareOverlayValues( IDirect3DDevice9 *pd3dDevice, const EFFCHAR_PROPERTY_OVERLAY_TEXTURE& sOverlayVal, ID3DXEffect* pEffect, DWORD dwOverMaterial );

	virtual void DrawMeshOVERLAY( IDirect3DDevice9 *pd3dDevice, 
									const D3DXMATRIXA16* pBoneMatrices,
									const CHARSETTING& sCharSetting, 
									EFFCHAR_PROPERTY_OVERLAY_TEXTURE* sOverlayVal, 
									ID3DXEffect* pEffect, 
									DWORD dwOverMaterial, 
									BOOL bWorldIdentity=FALSE );

private:
	//void RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
	//						SMATERIAL_PIECE *pmtrlPiece );
	//void RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr );
	void RenderNewTex( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, int ipattr );

public:
	void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice );

private:
	void Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice );

	void CleanUp();

public:
	DxSkinMesh9_NORMAL();
	virtual ~DxSkinMesh9_NORMAL();
};