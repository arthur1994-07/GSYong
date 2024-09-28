// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : 정점 데이터와 면 갯수를 넣으면 Octree 형식의 최적화된 OptimizeMesh 를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/tr1/memory.hpp>

#include "../../DxTools/DxVertexFVF.h"
#include "../AABBBinaryTree/AABBBinaryTreeBase.h"
#include "../LightMap/LightMapDef.h"
#include "../DxLandDef.h"

struct DxAABBNode;
class DxTexEffBase;
class DxTangentMesh;

// Note : 예전 버젼은 아무 데이타 형이 가능 했지만 현재는 VERTEXCOLORTEX::FVF만 가능하게 되었다.
//		현재 DxEffectTiling 만이 D3DFVF_DIFFUSE를 추가해서 사용한다.
//		20060803 : 데이터 형이 VERTEXCOLORNORTEX로 바뀌었다. 이유는 반사맵을 사용하기 위함임.
class OBJOCTree : public AABBBinaryTreeBase
{
public:
	struct DIRECTPOINTCOLOR
	{
		WORD wAlpha;
		WORD wDayR;
		WORD wDayG;
		WORD wDayB;
		WORD wNightR;
		WORD wNightG;
		WORD wNightB;
		WORD wTemp;		// 사이즈를 맞추기 위함.
	};

	struct NORMALCOLOR
	{
		D3DXVECTOR3 vNor;
		D3DXCOLOR	cColor;

		NORMALCOLOR() :
			vNor(0.f,0.f,0.f),
			cColor(1.f,1.f,1.f,1.f)
		{
		}
	};

	enum
	{
		VERSION = 0x102,
		VERSION_OCTREE = 0x10002,
	};

	NORMALCOLOR*	pNormalColor;		// Diffuse 값을 계산하고 난 후에는 사용하지 않는 값이다. Color가 있는것을 대비 한 것이다.

    std::tr1::shared_ptr<DxTangentMesh>    m_spTangentMesh;

	EM_LAND_TYPE			m_emLandType;	// Load 하기 바로 전 혹은 Make할 때 셋팅된다.
	DWORD					m_dwFVF;
	UINT					m_nFVFSize;
	DWORD					m_dwVert;
	VERTEXNORCOLORTEX*		m_pVertSrc;		// 저장 시켜 놓을 장소.	Diffuse의 memcpy와 Effect Mesh를 위하여 존재 & TangentMesh .. 구조를 변경하면 안됨.
	DIRECTPOINTCOLOR*		m_pColor;		// Direct와 Point의 컬러	// 음.. 필요 없을경우가 있다. EMLR_DAYNIGHT_OFF,EMLR_PIECE
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// 
	LPDIRECT3DVERTEXBUFFERQ	m_pVB_Checker;	// Checker 렌더링시 만들어서 쓰임.
	LPDIRECT3DINDEXBUFFERQ	m_pIB;
	//CRITICAL_SECTION		m_pCSLockVB;	// Thread시 VB의 ... .. . 단일 변화.
	D3DXVECTOR2*			m_pLightMapUV;	// User가 만든 LightMap UV 값
	DWORD					m_dwCopyIndex;	// 타 Thread 에서 VB Lock 이 계속 Crash 가 나서 메인에서 작업을 하기 위해 이 값이 필요함.

    BOOL        m_bTangentMesh_m_pVertSrc;  // m_pVertSrc의 내부값이 TangentMesh 인가~?

    // virtual
public:
	virtual BOOL IsLoad() const;
	virtual void DrawEff( const LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void RenderOctreeEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff );
	virtual void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bAlphaTex );
	virtual void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex );
	virtual BOOL DynamicLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread );
	virtual void Save( sc::SerialFile& SFile, BOOL bPiece );
	virtual void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );

	virtual void GetVertexNum( DWORD& dwVertexNum );
	virtual void GetVectorVertices( std::vector<VERTEXNORCOLORTEX4>& vecVertices, std::vector<D3DXVECTOR2>& vecLightMapUV );
	virtual void GetVectorVertices( std::vector<VERTEXNORCOLORTEX5>& vecVertices );
	virtual void Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bDynamicLoad, BOOL bPiece );

public:
	void CleanUp();

	void CreateBASE( const LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces );
	void CreateVertSrc( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert );
	void CreateNormalColor( BYTE* pByte, const DWORD dwFVF, const DWORD dwVert );
	void ComputeNormalColor( const DWORD dwVert );
	void ComputeNormalColorDAYNIGHT_OFF( const DWORD dwVert );
	void CopyVertSrcToVB( BYTE* pByte, VERTEXNORCOLORTEX* pVertSrc, LPDIRECT3DVERTEXBUFFERQ	pVB, const DWORD dwVert, DWORD dwFVF );
	void CopyMeshToIB( WORD* pSrc, LPDIRECT3DINDEXBUFFERQ pIB, const DWORD dwFaces );
	void CovertPieceSetting( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUserLightMapUV );
    void CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );
    void CreateTangentColorMesh( LPDIRECT3DDEVICEQ pd3dDevice );
	void CreateCheckerMesh( LPDIRECT3DDEVICEQ pd3dDevice );

	//void InsertColorList( LOADINGDATALIST &listColorData );
	//void FrameMoveCOLOR();
	void ComputeCOLOR( const DWORD& dwStart, const DWORD& dwEnd );

	//void ThreadLoad( LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
    void RenderVisualMaterial_Color( LPDIRECT3DDEVICEQ pd3dDevice, DxTexEffBase* pEff, BOOL bRealPL, const D3DXMATRIX& matWorld, float fAlpha );


    OBJOCTree* GetLeftChild_OBJOCTree();
    OBJOCTree* GetRightChild_OBJOCTree();
	
	void Load_VER100( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

private:
	BOOL OctreeLoad( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread );
	BOOL OctreeLoad_10000( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread );
	BOOL OctreeLoad_10001( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const BOOL bThread );
	BOOL OctreeLoadOLD( const LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DWORD dwFVF, const BOOL bThread );

public:
	OBJOCTree();
	virtual ~OBJOCTree();
};
//typedef OBJOCTree* POBJOCTREE;
