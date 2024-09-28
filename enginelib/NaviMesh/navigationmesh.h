#ifndef NAVIGATIONMESH_H
#define NAVIGATIONMESH_H

#include <vector>
#include "../DxTools/DxCustomTypes.h"
#include "navigationcell.h"
#include "navigationheap.h"

namespace sc {
    class SerialFile;
}

class	NavigationPath;
struct	DxFrame;
struct	DxAABBNode;

struct	SNaviIntegrityLog
{
	DWORD CellID[ 10 ];
	char StrLog[ 256 ];

	SNaviIntegrityLog()
	{
		memset( StrLog, 0, sizeof( char ) * 256 );
	};
};

class NavigationMesh
{
public:
	typedef	std::vector< NavigationCell* > CELL_ARRAY;

private:	
	CELL_ARRAY m_CellArray;			//	�׺���̼� �� ����Ʈ
	DxAABBNode* m_pAABBTreeRoot;	//	AABB Ʈ��

	D3DXVECTOR3* m_pNaviVertex;		//	�׺���̼� �� ��ü ���� �迭
	int m_nNaviVertex;				//	�׺���̼� �� ��ü ���� ����

	NavigationCell** m_pDACell;		//	Index������ ���� Cell�� ���� ������ �迭

	int m_PathSession;				// path finding data...
	NavigationHeap m_NavHeap;		// path finding data...

protected:
	//	<--	NavigationMesh Render�� [DEBUG]
	LPDIRECT3DVERTEXBUFFERQ m_pVB;
	LPDIRECT3DINDEXBUFFERQ m_pIB;
	BOOL m_bIndex16Bit;
	DWORD* m_pErrorID;
	DWORD m_nErrorID;
	//	-->	NavigationMesh Render�� [DEBUG]

public:
	NavigationMesh();
	NavigationMesh( const NavigationMesh& src );	//mjeon.�δ��� ���� LandMan ���縦 �Ҷ� ���
	NavigationMesh& operator= ( NavigationMesh &rvalue );
	~NavigationMesh();

	void	Clear();
	void	Copy( const NavigationMesh& rvalue );

	HRESULT	CreateNaviMesh( DxFrame* pNaviFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	void	LinkCells();
	HRESULT	MakeAABBTree();
	BOOL	CheckIntegrity( const char* szDebugFullDir );
	void	GotoErrorPosition( DWORD* pIDs, DWORD nIDs );

	// TuringPoint �� ��´�.
	BOOL	GetTuringPoint( D3DXVECTOR3& vTurningPosition,
							DWORD dwCellID, 
							int nLinkWall, 
							const D3DXVECTOR3& vStartPos, 
							const D3DXVECTOR3& vTargetPos, 
							const D3DXVECTOR3& vEndPos,
							BOOL bTypeLine );

protected:
	HRESULT	AddCell( const DWORD& PointA, const DWORD& PointB, const DWORD& PointC, const DWORD& CellID );

	//	<--	NaviVertex �Լ���,
	HRESULT	SetNaviVertexIndexData( D3DXVECTOR3* pNaviVertex, int nNaviVertex, int nDACell );
	void	DelNaviVertexIndexData();
	//	-->	NaviVertex �Լ���

	void	CheckLink( DWORD* pUsedCell, DWORD StartCellID, DWORD GroupID );

public:
	//	<--	����Ʈ�� �̿��� ��ã�� �� ���� ����ֱ� �Լ���
	D3DXVECTOR3	SnapPointToCell( DWORD CellID, const D3DXVECTOR3& Point );
	D3DXVECTOR3	SnapPointToMesh( DWORD* CellOutID, const D3DXVECTOR3& Point );
	DWORD		FindClosestCell( const D3DXVECTOR3& Point)	const;
	//	-->	����Ʈ�� �̿��� ��ã�� �� ���� ����ֱ� �Լ���

	bool	LineOfSightTest( DWORD StartID, const D3DXVECTOR3& StartPos, DWORD EndID, const D3DXVECTOR3& EndPos );
	bool	BuildNavigationPath( NavigationPath& NavPath, DWORD StartID, const D3DXVECTOR3& StartPos, DWORD EndID, const D3DXVECTOR3& EndPos, float* pfPathDist = NULL );
	void	ResolveMotionOnMesh( const D3DXVECTOR3& StartPos, DWORD StartID, D3DXVECTOR3& EndPos, DWORD* EndID );

	int		TotalCells() const;
	NavigationCell* GetCell( int index );
	const bool GetAABB(D3DXVECTOR3& vMax, D3DXVECTOR3& vMin) const;

protected:
	//	<--	AABB Tree �����	
	HRESULT MakeAABBNode( DxAABBNode* pNode, D3DXMATRIX& matComb, DWORD* pCellIndex, DWORD nCellIndex, D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );
	HRESULT GetCenterDistNode( D3DXMATRIX& matComb, DWORD* pCellIndex, DWORD nCellIndex, D3DXVECTOR3& vMax, D3DXVECTOR3 &vMin );
	HRESULT GetSizeNode( D3DXMATRIX& matComb, DWORD* pCellIndex, DWORD nCellIndex,	D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );
	BOOL	IsWithInTriangle( D3DXMATRIX& matComb, LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3, float fDivision, DWORD dwAxis );
	void	IsCollision( DxAABBNode* pAABBCur, const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3& vCollision, DWORD &CollisionID ) const;
	void	GetAllCollisionCell( DWORD* pCollisionCellID, DWORD& CollisionIDCount, D3DXVECTOR3& vAMax, D3DXVECTOR3& vAMin, DxAABBNode* pAABBCur );
	//	-->	AABB Tree �����

public:
	BOOL	IsVailedCollision() { return m_pAABBTreeRoot != NULL; }
	void	IsCollision( const D3DXVECTOR3& vPoint1, const D3DXVECTOR3& vPoint2, D3DXVECTOR3& vCollision, DWORD& CollisionID, BOOL& bCollision ) const;

public:
	void	ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strFrameName, const TSTRING& strTexName );
	void	GetVertexData( std::vector<D3DXVECTOR3>& vecPosition );

	void	SaveFile( sc::SerialFile& SFile );
	void	LoadFile( sc::SerialFile& SFile );
	
	void	SaveFile( const TCHAR* pName );
	BOOL	LoadFile( const TCHAR* pName );

public:
	//	<--	NavigationMesh Render�� [DEBUG]
	HRESULT	CreateVBIB( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshData );
	HRESULT	DeleteVBIB();
	HRESULT	Render( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT	RenderNavigationForEditVerySlow( LPDIRECT3DDEVICEQ pd3dDevice );	// �̴ϸ� �׸��� ���
	//	-->	NavigationMesh Render�� [DEBUG]
};

#endif