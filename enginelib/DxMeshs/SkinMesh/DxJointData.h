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

	TSTRING	m_strBoneParent;	// Save, Load	�θ�
	TSTRING	m_strBoneJoint;		// Save, Load	Joint
	TSTRING	m_strBoneChild;		// Save, Load	�ڽ�
	DWORD m_dwTwist;			// Save, Load	�������� ����Ϸ��� m_fTwist / 180.f * D3DX_PI;
	float m_fTwistRestitution;	// Save, Load	
	float m_fTwistSpring;		// Save, Load	
	float m_fTwistDamping;		// Save, Load	
	DWORD m_dwSwingY;			// Save, Load	�������� ����Ϸ��� m_fSwing / 180.f * D3DX_PI;
	DWORD m_dwSwingZ;			// Save, Load	�������� ����Ϸ��� m_fSwing / 180.f * D3DX_PI;
	float m_fSwingRestitution;	// Save, Load	
	float m_fSwingSpring;		// Save, Load	
	float m_fSwingDamping;		// Save, Load	
	float m_fDensity;			// Save, Load	�е� ��ġ
	D3DXVECTOR3	m_vBoneDir;		// Save, Load	Edit �� �ʿ��� ��
	D3DXVECTOR3	m_vRotate;		// Save, Load	ȸ��

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile );

	DxJointData()
		: m_dwTwist(0)		// 0��
		, m_fTwistRestitution(0.f)
		, m_fTwistSpring(0.f)
		, m_fTwistDamping(0.f)
		, m_dwSwingY(20)	// 20��
		, m_dwSwingZ(20)	// 20��
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
typedef std::vector<DxBoneTransJointData>	VEC_BONE_TRANS_JOINT_DATA;		// ����
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
