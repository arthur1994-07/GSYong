#include "pch.h"

#include "DxJointData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
//							D	x		J	i	g	g	l	e		D	a	t	a
//------------------------------------------------------------------------------------------------------------
void DxJointData::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << m_strBoneParent;
		SFile << m_strBoneJoint;
		SFile << m_strBoneChild;
		SFile << m_dwTwist;
		SFile << m_fTwistRestitution;
		SFile << m_fTwistSpring;
		SFile << m_fTwistDamping;
		SFile << m_dwSwingY;
		SFile << m_dwSwingZ;
		SFile << m_fSwingRestitution;
		SFile << m_fSwingSpring;
		SFile << m_fSwingDamping;
		SFile << m_fDensity;
		SFile << m_vBoneDir;
		SFile << m_vRotate;
	}
	SFile.EndBlock();
}

void DxJointData::Load( sc::BaseStream& SFile )
{
	DWORD dwVersion, dwBufferSize;
	SFile >> dwVersion;
	SFile >> dwBufferSize;

	if( dwVersion == VERSION )
	{
		SFile >> m_strBoneParent;
		SFile >> m_strBoneJoint;
		SFile >> m_strBoneChild;
		SFile >> m_dwTwist;
		SFile >> m_fTwistRestitution;
		SFile >> m_fTwistSpring;
		SFile >> m_fTwistDamping;
		SFile >> m_dwSwingY;
		SFile >> m_dwSwingZ;
		SFile >> m_fSwingRestitution;
		SFile >> m_fSwingSpring;
		SFile >> m_fSwingDamping;
		SFile >> m_fDensity;
		SFile >> m_vBoneDir;
		SFile >> m_vRotate;
	}
	else if( dwVersion == 0x0103 )
	{
		SFile >> m_strBoneParent;
		SFile >> m_strBoneJoint;
		SFile >> m_dwTwist;
		SFile >> m_fTwistRestitution;
		SFile >> m_fTwistSpring;
		SFile >> m_fTwistDamping;
		SFile >> m_dwSwingY;
		SFile >> m_dwSwingZ;
		SFile >> m_fSwingRestitution;
		SFile >> m_fSwingSpring;
		SFile >> m_fSwingDamping;
		SFile >> m_fDensity;
		SFile >> m_vBoneDir;
		SFile >> m_vRotate;
	}
	else if( dwVersion == 0x0102 )
	{
		SFile >> m_strBoneParent;
		SFile >> m_strBoneJoint;
		SFile >> m_dwTwist;
		SFile >> m_dwSwingY;
		SFile >> m_dwSwingZ;
		SFile >> m_fDensity;
		SFile >> m_vBoneDir;
		SFile >> m_vRotate;
	}
	else if ( dwVersion == 0x0101 )
	{
		SFile >> m_strBoneParent;
		SFile >> m_strBoneJoint;
		SFile >> m_dwTwist;
		SFile >> m_dwSwingY;
		SFile >> m_fDensity;
		SFile >> m_vBoneDir;
		SFile >> m_vRotate;

		m_dwSwingZ = m_dwSwingY;
	}
	else if ( dwVersion == 0x0100 )
	{
		SFile >> m_strBoneParent;
		SFile >> m_strBoneJoint;
		SFile >> m_dwTwist;
		SFile >> m_dwSwingY;
		SFile >> m_fDensity;

		m_dwSwingZ = m_dwSwingY;
	}
}
