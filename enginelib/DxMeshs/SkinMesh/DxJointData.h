#pragma once

#include "../../../SigmaCore/File/SerialFile.h"
//#include "../../CoreCommon/String/SerialFile.h"

struct DxBoneTrans;


struct DxJointData
{
	enum
	{
		VERSION = 0x0104,
	};

	TSTRING	m_strBoneParent;	// Save, Load	부모
	TSTRING	m_strBoneJoint;		// Save, Load	Joint
	TSTRING	m_strBoneChild;		// Save, Load	자식
	DWORD m_dwTwist;			// Save, Load	라디언값으로 사용하려면 m_fTwist / 180.f * D3DX_PI;
	float m_fTwistRestitution;	// Save, Load	
	float m_fTwistSpring;		// Save, Load	
	float m_fTwistDamping;		// Save, Load	
	DWORD m_dwSwingY;			// Save, Load	라디언값으로 사용하려면 m_fSwing / 180.f * D3DX_PI;
	DWORD m_dwSwingZ;			// Save, Load	라디언값으로 사용하려면 m_fSwing / 180.f * D3DX_PI;
	float m_fSwingRestitution;	// Save, Load	
	float m_fSwingSpring;		// Save, Load	
	float m_fSwingDamping;		// Save, Load	
	float m_fDensity;			// Save, Load	밀도 수치
	D3DXVECTOR3	m_vBoneDir;		// Save, Load	Edit 시 필요한 값
	D3DXVECTOR3	m_vRotate;		// Save, Load	회전

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile );

	DxJointData()
		: m_dwTwist(0)		// 0도
		, m_fTwistRestitution(0.f)
		, m_fTwistSpring(0.f)
		, m_fTwistDamping(0.f)
		, m_dwSwingY(20)	// 20도
		, m_dwSwingZ(20)	// 20도
		, m_fSwingRestitution(0.f)
		, m_fSwingSpring(0.f)
		, m_fSwingDamping(0.f)
		, m_fDensity(1.f)
		, m_vBoneDir(0.f,1.f,0.f)
		, m_vRotate(0.f,0.f,0.f)
	{
	}
};


struct DxBoneTransJointData
{
	DxBoneTrans*	m_rBoneTransParent;
	DxBoneTrans*	m_rBoneTransJoint;
	DxBoneTrans*	m_rBoneTransChild;
	DxJointData		m_sJointData;

	DxBoneTransJointData()
		: m_rBoneTransParent(NULL)
		, m_rBoneTransJoint(NULL)
		, m_rBoneTransChild(NULL)
	{
	}
};
typedef std::vector<DxBoneTransJointData>	VEC_BONE_TRANS_JOINT_DATA;		// 참조
typedef VEC_BONE_TRANS_JOINT_DATA::iterator	VEC_BONE_TRANS_JOINT_DATA_ITER;


namespace physx
{
	class PxRigidActor;
	class PxJoint;
};

typedef std::vector<physx::PxRigidActor*> VEC_NXACTOR;
typedef std::vector<physx::PxRigidActor*> VEC_NXACTOR_ITER;
typedef std::vector<physx::PxJoint*> VEC_NXJOINT;
typedef std::vector<physx::PxJoint*> VEC_NXJOINT_ITER;
