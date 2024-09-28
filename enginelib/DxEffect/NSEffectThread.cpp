#include "pch.h"

#include <vector>
#include <process.h>
#include <boost/scoped_ptr.hpp>

#include "../../BugTrap/BugTrap.h"

#include "../DxEffect/Single/Thread/ParticlesForThread.h"
#include "../DxEffect/Single/Thread/GroundForThread.h"
#include "../DxEffect/Single/Thread/SingleForThread.h"
#include "../DxTools/DxViewPort.h"

#include "NSEffectThread.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSEffectThread
{
	DWORD	g_dwThreadID;		//	스레드 ID
	HANDLE	g_hThread(NULL);	//	스레드 핸들

	// SingleForThread 의 참조카운트는 기본적으로 2 가 될것이다.
	// Thread 쪽에 한개, Main 쪽에 한개.
	// Main 쪽에서 사용을 안하게 되면 지우게 되고, 카운트는 1이 된다.
	// 그렇게 될때 Thread 쪽에서 카운트가 1인 것들만 지워주면 된다.
	//
	// FrameMove 의 특정시점에서 타 Thread 를 멈추고, g_listSingleEffectGet 있는 것을 g_listSingleEffect 에 등록하고,
	// g_listSingleEffect 에 삭제되야 할 것을 삭제 한다.
	//
	std::list<boost::shared_ptr<SingleForThread>>	g_listSingleEffect;

	// Main Thread 에서 등록된것을 가지고 있는다.
	std::list<boost::shared_ptr<SingleForThread>>	g_listSingleEffectGet;

	enum MODE_WAIT
	{
		CALCULATE_DATA_MODE		= 0x0,	// 계산이 진행되는 시기
		COMPLETE_DATA_MODE		= 0x1,	// 종료
	};

	struct DATA
	{		
		BOOL							m_bStop;				// 파괴전 TRUE 셋팅
		BOOL							m_bDestroy;				// 파괴 진행.
		MODE_WAIT						m_emMode_Wait;
		DWORD							m_dwCalculateTime;		// 계산 시간을 구하기 위함.

		DATA()
			: m_bStop(FALSE)
			, m_bDestroy(FALSE)
			, m_emMode_Wait(COMPLETE_DATA_MODE)
			, m_dwCalculateTime(0)
		{
		}

		void DataReset()
		{
		}
	};
	DATA g_sData;

	struct CommonData
	{
		float			m_fElapsedTime;
		D3DXMATRIX		m_matCameraView;

		CommonData()
			: m_fElapsedTime(0.f)
		{
			D3DXMatrixIdentity( &m_matCameraView );
		}

		void InsertCommonData( float fElapsedTime, const D3DXMATRIX& matCameraView )
		{
			// [shhan][2015.06.19] 2fps -> 0.9fps 으로 일단 변경됨.
			//						변경이유는 CD3DApplication::FrameMove3DEnvironment 쪽 Sleep(1000) 으로 인해 Effect 들이 지워지지 않고 33배가 중첩되어 버린다. 
			//						1초당 1개씩이지만 시간은 0.03 간게 되어버러서 삭제가 30배 후에 됨.
			//						1fps 으로 하면 엇박자가 생길꺼 같아서 0.9 로 함.
			//
			////Note : <이전주석> 2 fps  극악 속도, 처음 로딩시 많이 발생. 
			////
			if ( fElapsedTime > 1.1f )
				fElapsedTime = 0.03f;

			m_fElapsedTime = fElapsedTime;
			m_matCameraView = matCameraView;
		}
	};
	CommonData	g_sCommonData;
	float		g_fElapsedTime(0.f);


	//////////////////////////////////////////////////////////////////////////
	// 맵이동과 관련 이전 데이터 사라짐으로 인한 정리

	void DataCleanUp()
	{
		while (1)
		{
			Sleep(1);

			// 계산 중에는 지우지 않는다.
			if ( g_sData.m_emMode_Wait == CALCULATE_DATA_MODE )
				continue;

			// 계산이 완료되었으니 지운다.
			g_sData.DataReset();
			return;
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread

	unsigned int WINAPI WorkThread( LPVOID pData )
	{
		BT_SetTerminate();

		DATA* _pData = (DATA*)pData;

		while (1)
		{
			// Sleep( 0 ) 셋팅시, 다중 클라이언트를 사용하면 서로가 Process 를 점유하려는 싸움에 fps 가 엄청 느려지는 문제가 발생했다.
			Sleep( 1 );

			if ( _pData->m_bStop )
			{
				_pData->m_bDestroy = TRUE;
				return 0;
			}

			// CALCULATE_DATA_MODE 가 될때 까지 기다리자.
			if ( g_sData.m_emMode_Wait == COMPLETE_DATA_MODE )
				continue;

			// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
			// 이게 정확하지는 않다. 문제는 또 이곳에 Sleep 이 걸려버리면 수치가 크게 나올 수도 있다.
#ifdef _RELEASED
			DWORD dwTime = timeGetTime();
#endif

			// Main Thread 쪽 Render() 상에서 렌더가 되야 되면 DxEffSingleGroup::m_bNeedSetVB 값을 TRUE 해 놓았다.
			// 그걸 여기서 IsNeedSetVB 로 확인 후 VertexBuffer 쪽으로 옮겨야 될 것만 옮긴다.
			std::list<boost::shared_ptr<SingleForThread>>::iterator iter = g_listSingleEffect.begin();
			for ( ; iter!=g_listSingleEffect.end(); ++iter )
			{
				if ( (*iter)->IsRenderCulling() )
				{
					(*iter)->FrameMove( g_sCommonData.m_fElapsedTime );
					(*iter)->SetVertices( g_sCommonData.m_fElapsedTime, g_sCommonData.m_matCameraView );
				}
			}

			// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
			_pData->m_dwCalculateTime = timeGetTime()-dwTime;
#endif

			// 완료되었다고 알린다.
			_pData->m_emMode_Wait = COMPLETE_DATA_MODE;
		}

		return 0;
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread 생성과 삭제

	BOOL StartThread()
	{
		g_hThread = (HANDLE) ::_beginthreadex(	NULL,
												0,
												WorkThread,
												&g_sData,
												0,
												(unsigned int*) &g_dwThreadID );
		if ( !g_hThread ) 
			return FALSE;

		return TRUE;
	}

	// 삭제는 캐릭터 지우기 전에 해줘야한다.
	void EndThread()
	{
		if ( !g_hThread )
			return;

		g_sData.m_bStop = TRUE;

		while(1)
		{
			Sleep(1);

			if ( g_sData.m_bDestroy )
				break;
		}

		CloseHandle( g_hThread );
		g_hThread = NULL;

		// clear 를 해주지 않으면 메모리 Leak 이 생긴다. ( SPDXEFFSINGLEGROUP 쪽 삭제가 제때 이루어지지 않아 TextureResource 삭제도 제때 이루어지지 않는다. )
		g_listSingleEffect.clear();
		g_listSingleEffectGet.clear();
	}




	//////////////////////////////////////////////////////////////////////////
	// 

	// 계산 완료가 되었는지 확인한다.
	BOOL IsCompleteData()
	{
		if ( g_sData.m_emMode_Wait == COMPLETE_DATA_MODE )
		{
#ifdef _RELEASED
			CDebugSet::ToView( 14, _T("EffectThread _ Work - Thread : %d"), g_sData.m_dwCalculateTime );
#endif
			return TRUE;
		}

		return FALSE;
	}

	void Swap( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		std::list<boost::shared_ptr<SingleForThread>>::iterator iter = g_listSingleEffect.begin();
		for ( ; iter!=g_listSingleEffect.end(); ++iter )
		{
			(*iter)->Swap( pd3dDevice );
		}
	}

	void Wait_FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, BOOL bNotRendering )
	{
		if ( !pd3dDevice )
			return;

		g_fElapsedTime += fElapsedTime;

		if ( !IsCompleteData() )
		{
			// Particle Thread 쪽 작업이 안되었을 경우, 파티클 정보 Update 는 하지 않도록 한다.
			return;
		}

		//// COMPLETE_DATA_MODE 될 때까지 대기.
		//while ( !IsCompleteData() )
		//{
		//}

		// 공통정보 셋팅.
		g_sCommonData.InsertCommonData( g_fElapsedTime, DxViewPort::GetInstance().GetMatView() );
		g_fElapsedTime = 0.f;

		// 사용 다 된 SingleForThread 삭제
		std::list<boost::shared_ptr<SingleForThread>>::iterator iter = g_listSingleEffect.begin();
		for ( ; iter!=g_listSingleEffect.end(); )
		{
			if ( (*iter).unique() )
			{
				iter = g_listSingleEffect.erase( iter );
			}
			else
			{
				++iter;
			}
		}

		// 새로운 SingleForThread 추가
		iter = g_listSingleEffectGet.begin();
		for ( ; iter!=g_listSingleEffectGet.end(); ++iter )
		{
			g_listSingleEffect.push_back( (*iter) );
		}
		g_listSingleEffectGet.clear();


		//////////////////////////////////////////////////////////////////////////
		// [shhan][2016.01.20] g_listSingleEffectGet 값이 계속 누적이 되어서 crash 가 발생하여,
		//						g_listSingleEffectGet.clear(); 작업 후 bNotRendering 확인 후 TRUE 시 return 하도록 한다.
		//////////////////////////////////////////////////////////////////////////
		// 최소화를 하여 Rendering 이 필요 없을 경우
		if ( bNotRendering )
		{
#ifdef _RELEASED
			CDebugSet::ToView( 14, _T("EffectThread _ Work - Thread : 0ff") );
#endif
			return;
		}


		// swap 을 한다.
		NSEffectThread::Swap( pd3dDevice );

		// swap 완료되면 Thread 에 다시 계산하라고 알려준다.
		g_sData.m_emMode_Wait = CALCULATE_DATA_MODE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 이때는 Main Thread
	boost::shared_ptr<SingleForThread> CreateParticleGroup( boost::shared_ptr<ParticleEmitter>& spParicleEmitter,
																boost::shared_ptr<DxEffSingleGroup>& spThisGroup,
																const DxEffectParticleSys* pParticleSysSource )
	{
		boost::shared_ptr<SingleForThread> spParticles(new ParticlesForThread( spParicleEmitter, spThisGroup, pParticleSysSource ));
		g_listSingleEffectGet.push_back( spParticles );

		return spParticles;
	}

	boost::shared_ptr<SingleForThread> CreateGround( boost::shared_ptr<DxEffSingleGroup>& spThisGroup, const DxEffectGround* pGroundSource )
	{
		boost::shared_ptr<SingleForThread> spGround(new GroundForThread( spThisGroup, pGroundSource ));
		g_listSingleEffectGet.push_back( spGround );

		return spGround;
	}
};