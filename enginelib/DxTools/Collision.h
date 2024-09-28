// Collision.h: interface for the CCollision class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_COLLISION_H__064700F3_5173_436C_8A96_CB946BB77C7C__INCLUDED_
#define AFX_COLLISION_H__064700F3_5173_436C_8A96_CB946BB77C7C__INCLUDED_

#pragma once

#include "../../InternalCommonLib/dxstdafx.h"
#include "../DxLand/NSLandThreadDef.h"
#include "./DxVertexFVF.h"
#include "./DxCustomTypes.h"
#include "./Collision/CollisionDef.h"

#include <vector>    // 주병진

class DxLandMan;
class DxFrameMesh;
struct DxSingleMaterialMeshRenderOrder;

#define AABB_NONINDEX	(0xFFFFFFFF)

namespace sc {
    class SerialFile;
}

struct DxAABBNode
{
private:
	enum
	{
		VERSION				= 0x100,
		TRIANGLE_VERTEX_NUM = 3,
	};

public:
	union
	{
		struct { float fMaxX, fMaxY, fMaxZ; };
		struct { D3DXVECTOR3 vMax; };
	};

	union
	{
		struct { float fMinX, fMinY, fMinZ; };
		struct { D3DXVECTOR3 vMin; };
	};
	
private:
	DWORD m_dwFaceID;

	// [shhan[2014.11.27] 충돌처리를 위한 삼각형의 위치값을 가지고 있는다.
	//						VB, IB 의 Lock 을 하지 않고 이값을 가지고 충돌처리 검사를 하도록 한다.
	D3DXVECTOR3 m_vPosition[TRIANGLE_VERTEX_NUM];

public:
	void Import( DWORD dwFaceID, const D3DXVECTOR3& vPos1, const D3DXVECTOR3& vPos2, const D3DXVECTOR3& vPos3 );

	DWORD GetFaceID() const;
	BOOL IsEnableFace() const;

	const D3DXVECTOR3* GetPosition( DWORD dwIndex ) const;

public:
	DxAABBNode *pLeftChild;
	DxAABBNode *pRightChild;

	DxAABBNode () :
		fMaxX(0.0f),
		fMaxY(0.0f),
		fMaxZ(0.0f),
		fMinX(0.0f),
		fMinY(0.0f),
		fMinZ(0.0f),
		m_dwFaceID(AABB_NONINDEX),
		pLeftChild(NULL),
		pRightChild(NULL)
	{
	}

	~DxAABBNode ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	}

	BOOL IsCollision ( D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2 ) const;
	BOOL IsCollisionVolume ( CLIPVOLUME & cv ) const;
	BOOL IsCollisionExceptY ( D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2 ) const; // 주병진 추가 메소드

public:
	BOOL		SaveFile ( sc::SerialFile &SFile );
	BOOL		LoadFile ( sc::SerialFile &SFile );
	BOOL		LoadFile_PREV ( sc::SerialFile &SFile );
};

struct	DxFrame;
class	DxOctree;
class   DxTexEffBase;

class OBJAABB
{
public:
	typedef OBJAABB* POBJAABB;

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin ) = 0;
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin ) = 0;
	virtual OBJAABB* GetNext () = 0;
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor ) const					{ return FALSE; }
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, POBJAABB& pObj ) const	{ return FALSE; }

public:
	virtual void	FrameMove( const float fElapsedTime )																			{ return; }
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )													{ return S_OK; }
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bReflect, float fReflectHeight )				{ return S_OK; }
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )			{ return S_OK; }

	virtual HRESULT FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, sc::SerialFile* const SFile )	{ return S_OK; }

	virtual HRESULT RenderTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									const CLIPVOLUME &sCV,
									BOOL bMPShading )									{ return S_OK; }

	virtual HRESULT RenderDYNAMIC( const LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, 
									const CLIPVOLUME &pCV, 
									sc::SerialFile* const SFile )						{ return S_OK; }

	virtual HRESULT RenderShadowMap( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &pCV )												{ return S_OK; }
	virtual HRESULT Render( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &pCV, const char* szName )										{ return S_OK; }
	virtual HRESULT Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &pCV )													{ return S_OK; }

	// DxSingleMaterialMesh 에서만 쓰인다.
	// bTool = FALSE
	virtual BOOL RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, DxSingleMaterialMeshRenderOrder* pRenderOrder, DWORD& dwTexType, BOOL bTool, BOOL bUnKnownTextureMode )	{ return FALSE; }

	// DXLANDSKINOBJ 사용됨.
	virtual void InsertData ( const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength )	{}
	virtual void InsertDataForShadow()																			{}

//public:
//	virtual void InsertColorList( LOADINGDATALIST &listColorData )		{ return; }

public:
    OBJAABB()           
    {
    }

    virtual ~OBJAABB()  
    {
    }
};
typedef OBJAABB* POBJAABB;

struct OBJAABBNode
{
	union
	{
		struct { D3DXVECTOR3 vMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 vMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	OBJAABBNode *pLeftChild;
	OBJAABBNode *pRightChild;

	OBJAABB *pObject;

	OBJAABBNode () :
		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		vMin(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		pLeftChild(NULL),
		pRightChild(NULL),
		pObject(NULL)
	{
	}

	~OBJAABBNode ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	}
};
typedef OBJAABBNode* POBJAABBNODE;

//////////////////////////////////////////////////////////////////////////
//	
namespace COLLISION
{
	BOOL IsCollision2DLineToLine( const D3DXVECTOR2& AP1, const D3DXVECTOR2& AP2, const D3DXVECTOR2& BP1, const D3DXVECTOR2& BP2, D3DXVECTOR2& vOut ); //주병진 추가메소드

	BOOL IsCollision2DTriangleToSquare( D3DXVECTOR2 sLineTriangle[], D3DXVECTOR2 sCross[], D3DXVECTOR2 sLineSquare[] );

	//class Functor_TriangleTransform //주병진 추가 Functor
	//{

	//public:

		//void Functor_TriangleTransform() ( const D3DXMATRIX& matTransform ) 
		//{
		//	D3DXVec3TransformCoord(&Triangle.vTri1, &Triangle.vTri1, &matTransform);
		//	D3DXVec3TransformCoord(&Triangle.vTri2, &Triangle.vTri2, &matTransform);
		//	D3DXVec3TransformCoord(&Triangle.vTri3, &Triangle.vTri3, &matTransform); 

		//	D3DXVec3TransformCoord(&Triangle.vCollision, &Triangle.vCollision, &matTransform);
		//}

	//public:

	//	void operator () ( COLLISION::TRIANGLE& Triangle )
	//	{
	//		D3DXVec3TransformCoord(&Triangle.vTri1, &Triangle.vTri1, &m_Matrix);
	//		D3DXVec3TransformCoord(&Triangle.vTri2, &Triangle.vTri2, &m_Matrix);
	//		D3DXVec3TransformCoord(&Triangle.vTri3, &Triangle.vTri3, &m_Matrix); 

	//		D3DXVec3TransformCoord(&Triangle.vCollision, &Triangle.vCollision, &m_Matrix); 
	//	}

	//private:

	//	D3DXMATRIX m_Matrix;

	//};

	//주병진 End

	//extern D3DXVECTOR3 vColTestStart;
	//extern D3DXVECTOR3 vColTestEnd;

	BOOL IsLineTriangleCollision ( const D3DXVECTOR3 *pTri0, const D3DXVECTOR3 *pTri1, const D3DXVECTOR3 *pTri2,
								const D3DXVECTOR3 *pPoint1, const D3DXVECTOR3 *pPoint2,
								D3DXVECTOR3 *pCollision, D3DXVECTOR3 *pNormal, EMCOLLMODE emCullMode, BOOL bSmallHeight );	// pNormal=NULL, bFrontColl=FALSE, bSmallHeight=TRUE

	BOOL IsLineTriangleCollision ( const D3DXVECTOR3 *pTri0, const D3DXVECTOR3 *pTri1, const D3DXVECTOR3 *pTri2,
		const D3DXVECTOR3 *pPoint1, const D3DXVECTOR3 *pPoint2,
		D3DXVECTOR3 *pCollision, float& t, float& u, float& v, D3DXVECTOR3 *pNormal, EMCOLLMODE emCullMode, BOOL bSmallHeight );

	BOOL IsSpherePointCollision ( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCenter, const float fRadius );

	//BOOL IsSpherePlolygonCollision ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
	//							const D3DXVECTOR3 &vCenter, const float fRadius );

	//extern BOOL g_bCheckAddPlane;
	//extern BOOL bCollisionCheck;
	BOOL IsCollisionVolume( const CLIPVOLUME& cv, const D3DXVECTOR3 &vCenter, float fLength );
	BOOL IsCollisionVolume( const CLIPVOLUME& cv, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );
	BOOL IsCollisionVolumeOrtho( const CLIPVOLUME& cv, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );
	BOOL IsCollisionAABBToAABB( const D3DXVECTOR3& vAMax, const D3DXVECTOR3& vAMin, const D3DXVECTOR3& vBMax, const D3DXVECTOR3& vBMin );
	BOOL IsCollisionLineToAABB( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin );
	
    BOOL IsCollisionLineToSphere(
		const D3DXVECTOR3& vStart,
		const D3DXVECTOR3& vEnd,
		const D3DXVECTOR3& vCenter,
		const float fLength,
		D3DXVECTOR3* pCollision = NULL );

    BOOL IsCollisionLineToSphere_Limit(
		const D3DXVECTOR3& vStart,
		const D3DXVECTOR3& vEnd,
		const D3DXVECTOR3& vCenter,
		const float fLength );

	inline BOOL IsfEqual ( float a, float b )
	{
		float dx = a - b;
		if ( dx < 0.001f && dx > -0.001f )	return TRUE;
		return FALSE;
	}

	inline BOOL IsWithInPoint ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin,
							const D3DXVECTOR3 &vPos )
	{
		if ( vMax.x < vPos.x || vMax.y < vPos.y || vMax.z < vPos.z )	return FALSE;
		if ( vMin.x > vPos.x || vMin.y > vPos.y || vMin.z > vPos.z )	return FALSE;

		return TRUE;
	}

	HRESULT CalculateBoundingBox ( const D3DXMATRIX *const pmatComb, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin,
								PBYTE pbPoints, UINT cVertices, DWORD dwFVF );

	void TransformAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const D3DXMATRIX &matTrans );

	//extern DWORD dwLeafCount;
	void MakeAABBTree ( POBJAABBNODE &pTreeHead, POBJAABB pListHead, BOOL bQUAD=FALSE );

	POBJAABB IsCollisonPointToOBJAABBTree ( POBJAABBNODE &pTreeNode, const D3DXVECTOR3 &vPos );

//	extern DWORD dwRendAABB;
	HRESULT RenderAABBTree ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB );

	HRESULT RenderAABBTreeReflect ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, POBJAABBNODE &pNode, float fReflectHeight );

	HRESULT RenderAABBTreeFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB, DxLandMan* pLandMan );


	HRESULT GetSizeNode ( D3DXMATRIX &matComb,
									WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
									WORD *pwFaces, DWORD dwNumFaces,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	BOOL IsWithInTriangle ( D3DXMATRIX &matComb,
							LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3,
							float fDivision, DWORD dwAxis );

	HRESULT GetCenterDistNode ( D3DXMATRIX &matComb,
									WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
									WORD *pwFaces, DWORD dwNumFaces,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	void CalcAxisTable ( float SizeX, float SizeY, float SizeZ, PDWORD pdwAxis );

	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb,
										WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
										WORD *pwFaces, DWORD dwNumFaces,
										D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	void MINDETECTAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const float fRate, const float fMinLeng );

	void CollisionLineToTreePARENT ( OBJAABBNode* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									POBJAABB& pOBJ, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor );

	void CollisionLineToTree ( OBJAABBNode* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									POBJAABB& pOBJ, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor );

	bool CalculatePlane2Line ( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, D3DXVECTOR3 &vCol );

	void	FrameMove( const float fElapsedTime, OBJAABBNode* pNode );
	void	FrameMoveAABBTree ( OBJAABBNode* pNode, float fElapsedTime, const CLIPVOLUME &sCV, const CLIPVOLUME* pReflectCV );
	HRESULT RenderAABBTree ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, const char* szName );

	HRESULT RenderAABBTreeTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, 
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
									const CLIPVOLUME &sCV, 
									OBJAABBNode* pNode, 
									BOOL bMPShading );

	//HRESULT RenderAABBTreeDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, sc::SerialFile* const SFile );
	HRESULT RenderAABBTreeShadowMap( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode );
	HRESULT Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode );
	HRESULT RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice,const CLIPVOLUME &sCV, OBJAABBNode* pNode, DxSingleMaterialMeshRenderOrder* pRenderOrder, DWORD& dwTexType );
	HRESULT LoadAABBTreeBOX ( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, OBJAABBNode* pNode, sc::SerialFile* const SFile );
	//void InsertColorList( OBJAABBNode* pNode, LOADINGDATALIST& listColorData );

	// DXLANDSKINOBJ 사용됨.
	void InsertData( OBJAABBNode* pNode, const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength );
	void InsertDataForShadow( OBJAABBNode* pNode, const CLIPVOLUME &sCV );

    BOOL IntersectTriangle( 
		const D3DXVECTOR3& orig, 
		const D3DXVECTOR3& dir, 
		const D3DXVECTOR3& v0, 
		const D3DXVECTOR3& v1, 
		const D3DXVECTOR3& v2, 
		FLOAT* t, 
		FLOAT* u, 
		FLOAT* v,
		const FLOAT fMinColl,	// const FLOAT fMinColl = 0.000001f,
		EMCOLLMODE emCullMode );		// const bool bBackColl = true

	void IntersectTriangle_GetUV(
		const D3DXVECTOR3& orig,
		const D3DXVECTOR3& dir,
		const D3DXVECTOR3& v0,
		const D3DXVECTOR3& v1,
		const D3DXVECTOR3& v2,
		FLOAT* u,
		FLOAT* v );

	BOOL IntersectTriangle( 
		const D3DXVECTOR3& orig, 
		const D3DXVECTOR3& dir, 
		const D3DXVECTOR3& v0, 
		const D3DXVECTOR3& v1, 
		const D3DXVECTOR3& v2, 
		FLOAT* t,
		const FLOAT fMinColl,	// const FLOAT fMinColl = 0.000001f,
		EMCOLLMODE emCullMode );		// const bool bBackColl = true

	BOOL IsSamePosition( const D3DXVECTOR3& sV1, const D3DXVECTOR3& sV2 );
}

#endif // AFX_COLLISION_H__064700F3_5173_436C_8A96_CB946BB77C7C__INCLUDED_
