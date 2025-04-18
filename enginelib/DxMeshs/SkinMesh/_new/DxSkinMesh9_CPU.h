#pragma once

namespace sc {
    class SerialFile;
}
class DxMaterialHLSL;

#include "../../../DxTools/DxVertexFVF.h"
#include "../../../DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
//
////--------------------------------------------------------------------------------------
//// Name: class DxSkinMesh9_NORMAL
//// Desc: 
////--------------------------------------------------------------------------------------
//class DxSkinMesh9_CPU : public DxSkinMesh9
//{
//public:
//	static DWORD		VERSION;
//
//protected:
//	TCHAR			m_szFileName[32];		// 파일 이름.
//	TCHAR			m_szSkinName[32];		// Skin xFile 이름.
//	TCHAR			m_szSkeleton[32];		// Skeleton xFile 이름.
//
//public:
//	static void StaticResetDevice( IDirect3DDevice9* pd3dDevice );
//	static void StaticLost();
//
//public:
//	virtual void DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sCharSetting, const DxSkeletonMaintain* pSkeleton );
//	virtual void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
//									DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE );
//
//private:
//	void RenderSceneGraph( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, const D3DXATTRIBUTERANGE& sAttribTable, DWORD dwIndex );
//	void RenderDirect( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, const D3DXATTRIBUTERANGE& sAttribTable );
//
//	//void RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
//	//						SMATERIAL_PIECE *pmtrlPiece );
//
//	//void RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr );
//
//	HRESULT RenderDefaultCPU( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
//							SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular, DWORD dwVertexStart );
//
//private:
//	void UpdateSkinnedMesh_0( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices );
//	void UpdateSkinnedMesh_1( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices );
//	void UpdateSkinnedMesh_2( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices );
//	void UpdateSkinnedMesh_3( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices );
//	void UpdateSkinnedMesh_4( VERTEX* pDest, BYTE* pSrcIN, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices );
//	HRESULT UpdateSkinnedMesh_SSE( BYTE* pDest, BYTE* pSrc, DWORD dwSkinFVF, D3DXMATRIXA16* pBoneMatrices, DWORD dwStart, DWORD dwNumVertices );
//
//private:
//	void PhysiqueTransform_SSE( VERTEX* pResult, BYTE* pBoneVertex, DWORD dwBoneSize, DWORD dwVertexNum, D3DXMATRIXA16* pAniMatrix );
//
//public:
//	virtual void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice );
//
//private:
//	virtual void Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice );
//
//	void LoadToon( TCHAR* pName, IDirect3DDevice9 *pd3dDevice );
//	void LoadToon( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice );
//
//	void LoadLOD( TCHAR* pName, IDirect3DDevice9 *pd3dDevice );
//	void LoadLOD( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice );
//
//	void CleanUp();
//
//public:
//	DxSkinMesh9_CPU();
//	virtual ~DxSkinMesh9_CPU();
//};


// typedef struct __declspec(intrin_type) __declspec(align(16)) __m128 {
//    float m128_f32[4];
// } __m128;
//
//struct SMatrix4_SSE  
//{
//	union 
//	{
//	    float	    m[16];
//	    __m128	   _m[4];
//	};
//
//	void mul(const SMatrix4_SSE& left, const SMatrix4_SSE& right);
//	void add(const SMatrix4_SSE& left, const SMatrix4_SSE& right);
//};
//

