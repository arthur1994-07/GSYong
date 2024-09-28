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

	TSTRING			m_strStartJiggleBone;	// ���ۺ��� ���۵Ǵ� ��			// Save, Load
	BOOL			m_bJiggleBoneColl;		// ���ۺ��� �浹ó�� �Ǵ°�~?	// Save, Load

	typedef std::vector< std::tr1::shared_ptr<DxJointData> >	VEC_JOINTDATA;
	typedef VEC_JOINTDATA::iterator							VEC_JOINTDATA_ITER;
	VEC_JOINTDATA	m_vecJointData;			// ����� Joint ������.										// Save, Load
	DWORD			m_dwJointNumberUSED;	// m_vecJointData�� ���� �Ȱ�. m_nJointNumber �� ����� ��.	// Save, Load

	VEC_BONE_TRANS_JOINT_DATA	m_vecBoneTransJointData;	// ������ Bone Point & Joint ����
	VEC_NXACTOR					m_vecNxActor;				// PhysX���� ���̴� Actor
	VEC_NXJOINT					m_vecNxJoint;				// PhysX���� ���̴� Joint

	BOOL CreateJiggleBone( DxSkeleton* pSkeleton, float fScale );
	void JiggleBoneMove_MainThread( const D3DXVECTOR3& vDir );
	void FrameMove_MainThread( const DxSkeletonMaintain& sSkeleton );
	BOOL Import( const JiggleBoneData* pSrc, DxSkeleton* pSkeleton, float fScale );	// ������ ����Ǳ� ���� �����͵��� ������ �����.



	// �ٸ� Thread ���� ȣ�� �ȴ�.
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
	VEC_JIGGLEBONEDATA	m_vecJiggleBoneData;	// ��� �����͸� ������ �ִ�.

	// Don't Save Load
private:
	D3DXVECTOR3 m_vPositionPREV;

	// Game
public:
	void CreateJiggleBone( DxSkeleton* pSkeleton, float fScale );


	// ��� Thread �� ���۵����ʴ� ��Ȳ���� ȣ��ȴ�.
	void Import( const DxJiggleBone* pJiggleBone, DxSkeleton* pSkeleton, float fScale );	// ������ ����Ǳ� ���� �����͵��� ������ �����.
	void FrameMove_MainThread( const DxSkeletonMaintain& sSkeleton, const D3DXMATRIX& matWorld );



	// �ٸ� Thread ���� ȣ�� �ȴ�.
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
