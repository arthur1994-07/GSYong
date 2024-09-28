#pragma once

namespace sc {
    class SerialFile;
}
#include "../../../DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../../../DxMeshs/SkinMesh/DxMaterialHLSL.h"

//--------------------------------------------------------------------------------------
// Name: class DxSkinMesh9_HLSL
// Desc: 
//--------------------------------------------------------------------------------------
class DxSkinMesh9_HLSL : public DxSkinMesh9
{
public:
	static DWORD	VERSION;

protected:
	TCHAR			m_szFileName[32];			// 파일 이름.
	TCHAR			m_szSkinName[32];		// Skin xFile 이름.
	TCHAR			m_szSkeleton[32];		// Skeleton xFile 이름.

public:
	static ID3DXEffect*	m_pFX;

protected:
	static ID3DXEffect*	m_pEffect;	// D3DX effect interface
	static LPDIRECT3DVERTEXDECLARATION9	m_pNormalDCRT;

public:
	static HRESULT	StaticCreate (IDirect3DDevice9* pd3dDevice );
	static HRESULT StaticResetDevice( IDirect3DDevice9* pd3dDevice );
	static void StaticLost();
	static void StaticDestroy();

public:
	static ID3DXEffect* GetEffect ()				{	return m_pEffect; }

public:
	virtual void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
									DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE );

//private:
//	void RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
//		SMATERIAL_PIECE *pmtrlPiece );
//
//	void RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr );

public:
	void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice );

private:
	void Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice );
	void LoadNormalMap( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice );

	void CleanUp();

public:
	DxSkinMesh9_HLSL();
	virtual ~DxSkinMesh9_HLSL();
};