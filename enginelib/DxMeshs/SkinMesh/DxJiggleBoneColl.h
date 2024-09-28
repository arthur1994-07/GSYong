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
	//const DxBoneTrans*		m_rBoneTrans;	// ������
	int						m_nBoneIndex;		// Dont Save Load



	void FrameMove( const DxSkeletonMaintain& sSkeletonMaintain );


	// ��� Thread �� ���۵����ʴ� ��Ȳ���� ȣ��ȴ�.
	BOOL Import( const JiggleBoneCollData* pSrc, DxSkeleton* pSkeleton, float fScale );	// ������ ����Ǳ� ���� �����͵��� ������ �����.

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
	VEC_JIGGLEBONECOLLDATA		m_vecCollisionMeshData;	// ��� �����͸� ������ �ִ�.

	// Game
public:

	// ��� Thread �� ���۵����ʴ� ��Ȳ���� ȣ��ȴ�.
	// ������ ����Ǳ� ���� �����͵��� ������ �����.
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