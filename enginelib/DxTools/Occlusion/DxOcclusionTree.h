/**
 * \date    2012/09/03
 * \author	shhan
 *
 * DxOcclusionTree - Vislble 되는 Index 들의 모임. Tree 형식으로 되어있음.
 * NSOcclusiontreeMaker - DxOcclusionTree 를 Tree 형식으로 만들어주는 namespace
 * 
 */
#pragma once

#include <vector>

#include "./NSOcclusionCharZone.h"
#include "./NSOcclusionCulling.h"

class DxLandMan;

struct DxOcclusionTree
{
private:
	enum
	{
		VERSION = 0x103,
	};

public:
	std::vector<D3DXVECTOR3>	m_vecPoint;			// 기준 위치.
	std::vector<D3DXVECTOR3>	m_vecPointChar;		// 기준 위치 캐릭터
	
public:
	D3DXVECTOR3 m_vMax;						// Save, Load	
	D3DXVECTOR3 m_vMin;						// Save, Load	

private:
	struct RenderIndex
	{
		SET_DWORD	m_setPiece;											// Save, Load	// 걸리는 PieceIndex.
		SET_DWORD	m_setOctree;										// Save, Load	// 걸리는 OctreeIndex
		SET_DWORD	m_setChar[NSOcclusionCharZone::HEIGHT_TYPE_SIZE];	// Save, Load	// 걸리는 OcclusionIndex - 캐릭터 쪽에 쓰임. 기본사이즈, 큰사이즈, 엄청큰사이즈

		BOOL IsHaveData();
		void CleanUp();
	};

	struct RenderIndex_VECTOR
	{
		VEC_DWORD	m_vecPiece;										
		VEC_DWORD	m_vecOctree;									
		VEC_DWORD	m_vecChar[NSOcclusionCharZone::HEIGHT_TYPE_SIZE];
	};

private:
	int			m_nID;						// Save, Load	// ID
	RenderIndex	m_sRenderIndex;				// Save, Load

public:
	DxOcclusionTree* m_pLeftChild;
	DxOcclusionTree* m_pRightChild;

public:
	void LastBoxNum( DWORD& dwBoxNum, DWORD& dwCount );
	void RenderBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );

	void Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxLandMan* pDxLandMan, 
				int& nCountCUR,
				BOOL bFieldOcclusion,
				BOOL bPieceOcclusion,
				BOOL bCharOcclusion );

	void BakePosition( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pDxLandMan, 
						const D3DXVECTOR3& vPos,
						BOOL bFieldOcclusion,
						BOOL bPieceOcclusion,
						BOOL bCharOcclusion );

	void BakeOneCell( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pDxLandMan, 
						int& nCountCUR,
						BOOL bFieldOcclusion,
						BOOL bPieceOcclusion,
						BOOL bCharOcclusion,
						float fSurfaceSize );

	void ReInsertPoint( const std::vector<D3DXVECTOR3>& vecPoint, const std::vector<D3DXVECTOR3>& vecPointChar );

	void GetRenderIndexPos( const D3DXVECTOR3& vPos, VEC_DWORD& vecPieceIndex, VEC_DWORD& vecOctreeIndex, VEC_DWORD vecCharIndex[] );

	//void GetCollCharZoneIndex( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, VEC_DWORD& vecCollCharZone );

	// 
	void SetOcclusionCharZone( LPDIRECT3DDEVICEQ pd3dDevice, int& nID_Count );

	// 데이터를 압축한다.
	void CompressData();

	// 압축데이터를 해제한다.
	void DeCompressData();

	// Point 를 최적화 한다.
	void OptimizePoint( DxLandMan* pDxLandMan, int nSumulationLength );

	// 데이터를 압축한다.
private:
	void GetRenderIndex( RenderIndex& sRenderIndex );
	void SetSameRenderIndex( RenderIndex& sRenderIndex );
	void DeleteIndex( RenderIndex& sRenderIndex );
	void CompressDataDetail( BOOL& bCompressed );

	// DeCompressData -> InsertData
	// 데이터를 넣어준다.
	void InsertData( const RenderIndex& sRenderIndex );

	void BakeOneCell_Detail( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxLandMan* pDxLandMan, 
							const D3DXVECTOR3& vFromPt,
							const D3DXVECTOR3& vLookatPt,
							BOOL bFieldOcclusion,
							BOOL bPieceOcclusion,
							BOOL bCharOcclusion,
							float fFOV,
							float fSurfaceWidth,
							float fSurfaceHeight,
							RenderIndex_VECTOR& sRenderIndex,
							DWORD dwCullMode );

public:
	void SaveMain( sc::SerialFile& SFile, const TSTRING& strMakeTimeWLD );
	void LoadMain( sc::SerialFile& SFile, TSTRING& strMakeTimeWLD );

private:
	void Save( sc::SerialFile& SFile );
	void Load( sc::SerialFile& SFile );
	void Load_101( sc::SerialFile& SFile );

public:
	DxOcclusionTree();
	~DxOcclusionTree();
};

namespace NSOcclusiontreeMaker
{
	void MakeTree 
	( 
		DxOcclusionTree** ppTree, 
		const std::vector<D3DXVECTOR3>& vecPoint, 
		const std::vector<D3DXVECTOR3>& vecPointChar, 
		const D3DXVECTOR3& vMax, 
		const D3DXVECTOR3& vMin, 
		float fSize 
	);
};