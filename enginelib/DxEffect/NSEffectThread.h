//////////////////////////////////////////////////////////////////////////
// [shhan][2015.07.17]	���� - Effect�� CPU ����� Thread �� ������ ����.
//						
#pragma once

#include <boost/shared_ptr.hpp>

class ParticleEmitter;
class SingleForThread;
class DxEffSingleGroup;
class DxEffectParticleSys;
class DxEffectGround;

namespace NSEffectThread
{
	//////////////////////////////////////////////////////////////////////////
	// ���̵��� ���� ���� ������ ��������� ���� ����

	// ����
	void DataCleanUp();




	//////////////////////////////////////////////////////////////////////////
	// Thread ������ ����

	// ����
	BOOL StartThread();

	// ����
	void EndThread();




	//////////////////////////////////////////////////////////////////////////
	// 

	void Wait_FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, BOOL bNotRendering=FALSE );

	boost::shared_ptr<SingleForThread> CreateParticleGroup( boost::shared_ptr<ParticleEmitter>& spParicleEmitter,
																boost::shared_ptr<DxEffSingleGroup>& spThisGroup,
																const DxEffectParticleSys* pParticleSysSource );

	boost::shared_ptr<SingleForThread> CreateGround( boost::shared_ptr<DxEffSingleGroup>& spThisGroup, const DxEffectGround* pGroundSource );
};