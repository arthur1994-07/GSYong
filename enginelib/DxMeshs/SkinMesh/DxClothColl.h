#pragma once

#include <map>

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxVertexFVF.h"

#include "./DxSkeletonManager.h"
#include "./DxJointData.h"

struct DxBoneTrans;
struct DxSkeleton;
struct DxJointData;
//class CORE_COMMON::CSerialFile;

namespace physx
{
	struct PxClothCollisionSphere;
};


struct ClothCollData
{
	enum
	{
		VERSION = 0x0100,
	};

	D3DXVECTOR3		m_vPosition;	// Save, Load
	float			m_fRadius;		// Save, Load
	TSTRING			m_strBoneName;	// Save, Load

	int				m_nBoneIndex;		// Bone 이 바뀔 시, 혹은 Tree 구성이 바뀔 시, 순서가 변동 될 수 있기 때문에 생성시 얻어오도록 한다.

	// 임시로 가지고 있는 값. 매번 번한다.
	mutable D3DXVECTOR3		m_vPositionResult;	// 최종 결과값 ( FrameMove 에서 계산되어진다. )

	BOOL Import( const ClothCollData* pSrc, float fScale, const DxSkeletonMaintain* pSkeleton );	// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.

	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton, DWORD dwColor );

	BOOL CreateCollisionData( const DxSkeleton* pSkeleton, const TSTRING& strBoneName );

	void FrameMove_ERROR( const DxSkeletonMaintain* pSkeleton, const D3DXMATRIX& matWorld ) const;
	void FrameMove_STATIC( const DxSkeletonMaintain* pSkeleton ) const;
	void FrameMove_MOVED( const DxSkeletonMaintain* pSkeleton ) const;

	void CleanUp();

	void Save( sc::SerialFile& SFile );
	BOOL Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton );

	ClothCollData();
	~ClothCollData();
};

//------------------------------------------------------------------------------------------------------------
//								D	x		C	l	o	t	h		C	o	l	l
//------------------------------------------------------------------------------------------------------------
class DxClothColl
{
private:
	enum
	{
		VERSION = 0x0100,
	};

private:
	typedef std::vector< std::tr1::shared_ptr<ClothCollData> >	VEC_CLOTH_COLLDATA;
	typedef VEC_CLOTH_COLLDATA::iterator						VEC_CLOTH_COLLDATA_ITER;
	typedef VEC_CLOTH_COLLDATA::const_iterator					VEC_CLOTH_COLLDATA_CITER;

	VEC_CLOTH_COLLDATA		m_vecCollisionMeshData;	// 모든 데이터를 가지고 있다.	// Save, Load, Edit
	std::vector<WORDWORD>	m_vecIndexPair;			// 데이터들을 Pair로 만든다.	// Save, Load ( Edit상 Insert() 하면 자동으로 만들어진다. )

	// 실제 사용되는 것. 저장시키거나 Edit 하지는 않는다.
private:
	VEC_CLOTH_COLLDATA		m_vecCollisionMesh_STATIC;	// 위치 고정인 것
	VEC_CLOTH_COLLDATA		m_vecCollisionMesh_MOVED;	// 위치 이동된 것
	VEC_CLOTH_COLLDATA		m_vecCollisionMesh_ERROR;	// 에러가 뜬 것.



	//////////////////////////////////////////////////////////////////////////
	// Game
public:

	// 다른 Thread 상에서 변경되는 Data 는 외부 Data -> vecSpheres
	void FrameMove( std::vector<physx::PxClothCollisionSphere>& vecSpheres, 
					const DxSkeletonMaintain* pSkeleton, 
					const D3DXMATRIX& matWorld, 
					float fBaseBoneScale ) const;

	// FrameMove 과 GetAllClothCollData 에서 사용되는 vecSpheres 는 다르다.
	void GetAllClothCollData( std::vector<physx::PxClothCollisionSphere>& vecSpheres, std::vector<DWORD>& vecIndexPairs, float fBaseBoneScale ) const;

private:
	// FrameMove 과 GetAllClothCollData 에서 사용되는 vecSpheres 는 다르다.
	void GetAllClothCollData( std::vector<physx::PxClothCollisionSphere>& vecSpheres, float fBaseBoneScale ) const;



	//////////////////////////////////////////////////////////////////////////
	// Edit
public:
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pSkeleton ) const;

	// Edit
public:
	void Insert( const DxSkeleton* pSkeleton, const TSTRING& strBoneName );
	void Delete( const DxSkeleton* pSkeleton, int nIndex );
	ClothCollData* GetClothCollData( int nIndex );

	// FrameMove 를 위한 정보를 Refresh 한다.
	void RefreshCollisionMeshForFrameMove();

	// Edit
private:
	void CreateIndexPair( const DxSkeleton* pSkeleton );


	//////////////////////////////////////////////////////////////////////////
private:
	friend class CClothCollPage;


	//////////////////////////////////////////////////////////////////////////
public:
	void CleanUp();

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton );

public:
	DxClothColl();
	~DxClothColl();
};