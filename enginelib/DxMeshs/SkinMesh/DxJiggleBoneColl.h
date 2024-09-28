#pragma once

#include <map>
#include "../../../SigmaCore/File/SerialFile.h"
//#include "../../CoreCommon/String/SerialFile.h"

//#include "../../PhysX/NSPhysX.h"
#include "./DxSkeletonManager.h"
#include "./DxJointData.h"

struct DxBoneTrans;
struct DxSkeleton;
struct DxJointData;
class DxSkeletonMaintain;
//class CORE_COMMON::CSerialFile;


struct JiggleBoneCollData
{
	enum
	{
		VERSION = 0x0100,
	};

	enum MESHTYPE
	{
		EMMT_BOX,
		EMMT_SPHERE,
		EMMT_CAPSULE,
	};

	MESHTYPE		m_emMeshType;	// Save, Load
	D3DXVECTOR3		m_vPosition;	// Save, Load
	D3DXVECTOR3		m_vHalfScale;	// Save, Load
	TSTRING			m_strBoneName;	// Save, Load

	physx::PxRigidActor*	m_pActor;
	//const DxBoneTrans*		m_rBoneTrans;	// 참조값
	int						m_nBoneIndex;		// Dont Save Load



	void FrameMove( const DxSkeletonMaintain& sSkeletonMaintain );


	// 계산 Thread 가 동작되지않는 상황에서 호출된다.
	BOOL Import( const JiggleBoneCollData* pSrc, DxSkeleton* pSkeleton, float fScale );	// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.

	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwColor );

	BOOL CreateCollisionData( const DxSkeleton* pSkeleton, const TSTRING& strBoneName );

	void CleanUp();

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton );

	JiggleBoneCollData();
	~JiggleBoneCollData();
};

//------------------------------------------------------------------------------------------------------------
//								D	x		J	i	g	g	l	e		B	o	n	e
//------------------------------------------------------------------------------------------------------------
class DxJiggleBoneColl
{
private:
	enum
	{
		VERSION = 0x0101,
	};

private:
	typedef std::vector< std::tr1::shared_ptr<JiggleBoneCollData> >		VEC_JIGGLEBONECOLLDATA;
	typedef VEC_JIGGLEBONECOLLDATA::iterator							VEC_JIGGLEBONECOLLDATA_ITER;
	typedef VEC_JIGGLEBONECOLLDATA::const_iterator						VEC_JIGGLEBONECOLLDATA_CITER;
	VEC_JIGGLEBONECOLLDATA		m_vecCollisionMeshData;	// 모든 데이터를 가지고 있다.

	// Game
public:

	// 계산 Thread 가 동작되지않는 상황에서 호출된다.
	// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.
	void Import( const DxJiggleBoneColl* pJiggleBone, DxSkeleton* pSkeleton, float fScale );

	//
	void FrameMove( const DxSkeletonMaintain& sSkeletonMaintain );

	

public:
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

	// Edit
public:
	void Insert( const DxSkeleton* pSkeleton, const TSTRING& strBoneName );
	void Delete( int nIndex );
	JiggleBoneCollData* GetJiggleBoneCollData( int nIndex );

private:
	friend class CJiggleBoneCollPage;

public:
	void CleanUp();

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile, const DxSkeleton* pSkeleton );

public:
	DxJiggleBoneColl();
	~DxJiggleBoneColl();
};