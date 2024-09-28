// NsOcTree.h: interface for the CCollision class.
//
//	class DxAABBOctree : ���� �����Ϳ� �� ������ ������ Octree ������ ����ȭ�� OptimizeMesh �� �����.
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

// Note : ���� ������ �ƹ� ����Ÿ ���� ���� ������ ����� VERTEXCOLORTEX::FVF�� �����ϰ� �Ǿ���.
//		���� DxEffectTiling ���� D3DFVF_DIFFUSE�� �߰��ؼ� ����Ѵ�.
//		20060803 : ������ ���� VERTEXCOLORNORTEX�� �ٲ����. ������ �ݻ���� ����ϱ� ������.
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
		WORD wTemp;		// ����� ���߱� ����.
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

	NORMALCOLOR*	pNormalColor;		// Diffuse ���� ����ϰ� �� �Ŀ��� ������� �ʴ� ���̴�. Color�� �ִ°��� ��� �� ���̴�.

    std::tr1::shared_ptr<DxTangentMesh>    m_spTangentMesh;

	EM_LAND_TYPE			m_emLandType;	// Load �ϱ� �ٷ� �� Ȥ�� Make�� �� ���õȴ�.
	DWORD					m_dwFVF;
	UINT					m_nFVFSize;
	DWORD					m_dwVert;
	VERTEXNORCOLORTEX*		m_pVertSrc;		// ���� ���� ���� ���.	Diffuse�� memcpy�� Effect Mesh�� ���Ͽ� ���� & TangentMesh .. ������ �����ϸ� �ȵ�.
	DIRECTPOINTCOLOR*		m_pColor;		// Direct�� Point�� �÷�	// ��.. �ʿ� ������찡 �ִ�. EMLR_DAYNIGHT_OFF,EMLR_PIECE
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;			// 
	LPDIRECT3DVERTEXBUFFERQ	m_pVB_Checker;	// Checker �������� ���� ����.
	LPDIRECT3DINDEXBUFFERQ	m_pIB;
	//CRITICAL_SECTION		m_pCSLockVB;	// Thread�� VB�� ... .. . ���� ��ȭ.
	D3DXVECTOR2*			m_pLightMapUV;	// User�� ���� LightMap UV ��
	DWORD					m_dwCopyIndex;	// Ÿ Thread ���� VB Lock �� ��� Crash �� ���� ���ο��� �۾��� �ϱ� ���� �� ���� �ʿ���.

    BOOL        m_bTangentMesh_m_pVertSrc;  // m_pVertSrc�� ���ΰ��� TangentMesh �ΰ�~?

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
