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


struct JiggleBoneData
{
	enum
	{
		VERSION = 0x0102,
	};

	TSTRING			m_strStartJiggleBone;	// 지글본이 시작되는 본			// Save, Load
	BOOL			m_bJiggleBoneColl;		// 지글본이 충돌처리 되는가~?	// Save, Load

	typedef std::vector< std::tr1::shared_ptr<DxJointData> >	VEC_JOINTDATA;
	typedef VEC_JOINTDATA::iterator							VEC_JOINTDATA_ITER;
	VEC_JOINTDATA	m_vecJointData;			// 연결된 Joint 정보들.										// Save, Load
	DWORD			m_dwJointNumberUSED;	// m_vecJointData는 연결 된것. m_nJointNumber 는 사용할 것.	// Save, Load

	VEC_BONE_TRANS_JOINT_DATA	m_vecBoneTransJointData;	// 참조형 Bone Point & Joint 정보
	VEC_NXACTOR					m_vecNxActor;				// PhysX에서 쓰이는 Actor
	VEC_NXJOINT					m_vecNxJoint;				// PhysX에서 쓰이는 Joint

	BOOL CreateJiggleBone( DxSkeleton* pSkeleton, float fScale );
	void JiggleBoneMove_MainThread( const D3DXVECTOR3& vDir );
	void FrameMove_MainThread( const DxSkeletonMaintain& sSkeleton );
	BOOL Import( const JiggleBoneData* pSrc, DxSkeleton* pSkeleton, float fScale );	// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.



	// 다른 Thread 에서 호출 된다.
	void FrameMove_PureThread( DxSkeleton* pSkeleton, float fKeyJiggleWeight );



	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL CreateJointData( const DxSkeleton* pSkeleton, const TSTRING& strBoneName );
	void CreateJointSub( const DxBoneTrans* pBoneTras );

	void DeleteJoint( int nIndex );
	void ResetJoint();

	void SetCollision( BOOL bCollision );

	void CleanUp();

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile );

	JiggleBoneData();
	~JiggleBoneData();
};

//------------------------------------------------------------------------------------------------------------
//								D	x		J	i	g	g	l	e		B	o	n	e
//------------------------------------------------------------------------------------------------------------
class DxJiggleBone
{
private:
	enum
	{
		VERSION = 0x0101,
	};

private:
	typedef std::vector< std::tr1::shared_ptr<JiggleBoneData> >	VEC_JIGGLEBONEDATA;
	typedef VEC_JIGGLEBONEDATA::iterator						VEC_JIGGLEBONEDATA_ITER;
	typedef VEC_JIGGLEBONEDATA::const_iterator					VEC_JIGGLEBONEDATA_CITER;
	VEC_JIGGLEBONEDATA	m_vecJiggleBoneData;	// 모든 데이터를 가지고 있다.

	// Don't Save Load
private:
	D3DXVECTOR3 m_vPositionPREV;

	// Game
public:
	void CreateJiggleBone( DxSkeleton* pSkeleton, float fScale );


	// 계산 Thread 가 동작되지않는 상황에서 호출된다.
	void Import( const DxJiggleBone* pJiggleBone, DxSkeleton* pSkeleton, float fScale );	// 게임이 진행되기 위한 데이터들을 얻어오고 만든다.
	void FrameMove_MainThread( const DxSkeletonMaintain& sSkeleton, const D3DXMATRIX& matWorld );



	// 다른 Thread 에서 호출 된다.
	void FrameMove_PureThread( DxSkeleton* pSkeleton, float fKeyJiggleWeight );



public:
	void RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

	// Edit
public:
	void Insert( const DxSkeleton* pSkeleton, const TSTRING& strBoneName );
	void Delete( int nIndex );
	void SetCollision( int nIndex, BOOL bCollision );
	JiggleBoneData* GetJiggleBone( int nIndex );

private:
	friend class CJiggleBonePage;

public:
	void CleanUp();

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile );

public:
	DxJiggleBone();
	~DxJiggleBone();
};
