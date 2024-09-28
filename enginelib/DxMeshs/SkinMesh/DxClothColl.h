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

	int				m_nBoneIndex;		// Bone �� �ٲ� ��, Ȥ�� Tree ������ �ٲ� ��, ������ ���� �� �� �ֱ� ������ ������ �������� �Ѵ�.

	// �ӽ÷� ������ �ִ� ��. �Ź� ���Ѵ�.
	mutable D3DXVECTOR3		m_vPositionResult;	// ���� ����� ( FrameMove ���� ���Ǿ�����. )

	BOOL Import( const ClothCollData* pSrc, float fScale, const DxSkeletonMaintain* pSkeleton );	// ������ ����Ǳ� ���� �����͵��� ������ �����.

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

	VEC_CLOTH_COLLDATA		m_vecCollisionMeshData;	// ��� �����͸� ������ �ִ�.	// Save, Load, Edit
	std::vector<WORDWORD>	m_vecIndexPair;			// �����͵��� Pair�� �����.	// Save, Load ( Edit�� Insert() �ϸ� �ڵ����� ���������. )

	// ���� ���Ǵ� ��. �����Ű�ų� Edit ������ �ʴ´�.
private:
	VEC_CLOTH_COLLDATA		m_vecCollisionMesh_STATIC;	// ��ġ ������ ��
	VEC_CLOTH_COLLDATA		m_vecCollisionMesh_MOVED;	// ��ġ �̵��� ��
	VEC_CLOTH_COLLDATA		m_vecCollisionMesh_ERROR;	// ������ �� ��.



	//////////////////////////////////////////////////////////////////////////
	// Game
public:

	// �ٸ� Thread �󿡼� ����Ǵ� Data �� �ܺ� Data -> vecSpheres
	void FrameMove( std::vector<physx::PxClothCollisionSphere>& vecSpheres, 
					const DxSkeletonMaintain* pSkeleton, 
					const D3DXMATRIX& matWorld, 
					float fBaseBoneScale ) const;

	// FrameMove �� GetAllClothCollData ���� ���Ǵ� vecSpheres �� �ٸ���.
	void GetAllClothCollData( std::vector<physx::PxClothCollisionSphere>& vecSpheres, std::vector<DWORD>& vecIndexPairs, float fBaseBoneScale ) const;

private:
	// FrameMove �� GetAllClothCollData ���� ���Ǵ� vecSpheres �� �ٸ���.
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

	// FrameMove �� ���� ������ Refresh �Ѵ�.
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