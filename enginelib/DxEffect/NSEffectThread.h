//////////////////////////////////////////////////////////////////////////
// [shhan][2015.07.17]	목적 - Effect의 CPU 계산을 Thread 로 돌리기 위함.
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
	// 맵이동과 관련 이전 데이터 사라짐으로 인한 정리

	// 정리
	void DataCleanUp();




	//////////////////////////////////////////////////////////////////////////
	// Thread 생성과 삭제

	// 생성
	BOOL StartThread();

	// 삭제
	void EndThread();




	//////////////////////////////////////////////////////////////////////////
	// 

	void Wait_FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, BOOL bNotRendering=FALSE );

	boost::shared_ptr<SingleForThread> CreateParticleGroup( boost::shared_ptr<ParticleEmitter>& spParicleEmitter,
																boost::shared_ptr<DxEffSingleGroup>& spThisGroup,
																const DxEffectParticleSys* pParticleSysSource );

	boost::shared_ptr<SingleForThread> CreateGround( boost::shared_ptr<DxEffSingleGroup>& spThisGroup, const DxEffectGround* pGroundSource );
};