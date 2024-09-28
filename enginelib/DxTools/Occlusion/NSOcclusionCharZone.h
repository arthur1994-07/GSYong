/**
 * \date    2012/09/26
 * \author	shhan
 *
 * DxOcclusionChar - Occlusion CharZone 검사를 위한 Mehs와 Query를 가지고 있음..
 * NSOcclusionCharZone - DxOcclusionChar 정보를 가지고 있음. - 내부 정보는 Bake 시 만들어졌다 사라짐.
 */
#pragma once

#include <vector>

#include "../DxCustomTypes.h"
#include "../DxVertexFVF.h"

namespace sc
{
	class SerialFile;
};

namespace NSOcclusionCharZone
{
	enum
	{ 
		HEIGHT_TYPE_SIZE = 3,
	};
};

class DxOcclusionChar
{
private:
	int			m_nID;
	D3DXVECTOR3 m_vCenter;
	LPD3DXMESH	m_pMesh[NSOcclusionCharZone::HEIGHT_TYPE_SIZE];
	D3DXVECTOR3	m_vMax[NSOcclusionCharZone::HEIGHT_TYPE_SIZE];
	D3DXVECTOR3	m_vMin[NSOcclusionCharZone::HEIGHT_TYPE_SIZE];
	LPD3DXMESH	m_pSimpleMesh;

	LPDIRECT3DQUERY9	m_pd3dQuery;
	BOOL				m_bQueryTest;
	DWORD				m_dwCullPixels;

public:
	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT GetDataQuery( DWORD& dwCullPixels );
	VOID DestroyQuery();
	void ResetOcclusionQuery();
	void InsertSort( const CLIPVOLUME& sCV, const D3DXVECTOR3& vFromPt, int nHeightType );
	BOOL FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType );
	void CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, const std::vector<D3DXVECTOR3>& vecPoint, int nID );
	void Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType );
	void RenderSimple( LPDIRECT3DDEVICEQ pd3dDevice );
	const D3DXVECTOR3* GetMax( int nHeightType ) const;
	const D3DXVECTOR3* GetMin( int nHeightType ) const;
	int GetID()								{ return m_nID; }
	const D3DXVECTOR3& GetCenter() const	{ return m_vCenter; }

public:
	DxOcclusionChar();
	~DxOcclusionChar();
};


class DxOcclusionCharTree
{
public:
	int			m_nID;									// Save, Load
	D3DXVECTOR3 m_vMax;									// Save, Load
	D3DXVECTOR3 m_vMin;									// Save, Load

	std::vector<DxOcclusionChar*>	m_vecOcclusionChar;	// Tree 만들때 사용됨. Query 작업시 사용됨. // 참조값임.

public:
	DxOcclusionCharTree* m_pLeftChild;		// Save, Load
	DxOcclusionCharTree* m_pRightChild;		// Save, Load

public:
	void InsertSort( const CLIPVOLUME& sCV, const D3DXVECTOR3& vFromPt );
	void GetCollCharZoneIndex( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, VEC_DWORD& vecCollCharZone );

public:
	void Save( sc::SerialFile& SFile );
	void Load( sc::SerialFile& SFile );

public:
	DxOcclusionCharTree();
	~DxOcclusionCharTree();
};

namespace NSOcclusiontreeMaker
{
	void MakeTree( DxOcclusionCharTree** ppTree, std::vector<DxOcclusionChar*>& vecOcclusionChar );
};

namespace NSOcclusionCharZone
{
	extern float		g_fCharHeightAvg;
	extern float		g_fCharHeightHigh;
	extern float		g_fCharHeightHighMax;

	void CleanUp();

	void Insert( LPDIRECT3DDEVICEQ pd3dDevice, int nID, const std::vector<D3DXVECTOR3>& vecPoint );
	void MakeTree();

	void GetCollCharZoneIndex( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, VEC_DWORD& vecCollCharZone );

	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();

	void ResetOcclusionQuery();

	void InsertSort( const CLIPVOLUME& sCV );
	void Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType, VEC_DWORD& vecZoneIndex  );

	void Save( sc::SerialFile& SFile );
	void Load( sc::SerialFile& SFile );
};

namespace NSOcclusionCharZoneSort
{
	void Insert( float fLength, DxOcclusionChar* pOcclusionChar );
	void Sort();
	void Clear();
	void Render_Query( const CLIPVOLUME& sCV, LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType );
	void FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, int nHeightType, VEC_DWORD& vecZoneIndex );
};