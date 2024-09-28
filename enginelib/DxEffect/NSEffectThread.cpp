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
	DWORD	g_dwThreadID;		//	������ ID
	HANDLE	g_hThread(NULL);	//	������ �ڵ�

	// SingleForThread �� ����ī��Ʈ�� �⺻������ 2 �� �ɰ��̴�.
	// Thread �ʿ� �Ѱ�, Main �ʿ� �Ѱ�.
	// Main �ʿ��� ����� ���ϰ� �Ǹ� ����� �ǰ�, ī��Ʈ�� 1�� �ȴ�.
	// �׷��� �ɶ� Thread �ʿ��� ī��Ʈ�� 1�� �͵鸸 �����ָ� �ȴ�.
	//
	// FrameMove �� Ư���������� Ÿ Thread �� ���߰�, g_listSingleEffectGet �ִ� ���� g_listSingleEffect �� ����ϰ�,
	// g_listSingleEffect �� �����Ǿ� �� ���� ���� �Ѵ�.
	//
	std::list<boost::shared_ptr<SingleForThread>>	g_listSingleEffect;

	// Main Thread ���� ��ϵȰ��� ������ �ִ´�.
	std::list<boost::shared_ptr<SingleForThread>>	g_listSingleEffectGet;

	enum MODE_WAIT
	{
		CALCULATE_DATA_MODE		= 0x0,	// ����� ����Ǵ� �ñ�
		COMPLETE_DATA_MODE		= 0x1,	// ����
	};

	struct DATA
	{		
		BOOL							m_bStop;				// �ı��� TRUE ����
		BOOL							m_bDestroy;				// �ı� ����.
		MODE_WAIT						m_emMode_Wait;
		DWORD							m_dwCalculateTime;		// ��� �ð��� ���ϱ� ����.

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
			// [shhan][2015.06.19] 2fps -> 0.9fps ���� �ϴ� �����.
			//						���������� CD3DApplication::FrameMove3DEnvironment �� Sleep(1000) ���� ���� Effect ���� �������� �ʰ� 33�谡 ��ø�Ǿ� ������. 
			//						1�ʴ� 1���������� �ð��� 0.03 ���� �Ǿ������ ������ 30�� �Ŀ� ��.
			//						1fps ���� �ϸ� �����ڰ� ���沨 ���Ƽ� 0.9 �� ��.
			//
			////Note : <�����ּ�> 2 fps  �ؾ� �ӵ�, ó�� �ε��� ���� �߻�. 
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
	// ���̵��� ���� ���� ������ ��������� ���� ����

	void DataCleanUp()
	{
		while (1)
		{
			Sleep(1);

			// ��� �߿��� ������ �ʴ´�.
			if ( g_sData.m_emMode_Wait == CALCULATE_DATA_MODE )
				continue;

			// ����� �Ϸ�Ǿ����� �����.
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
			// Sleep( 0 ) ���ý�, ���� Ŭ���̾�Ʈ�� ����ϸ� ���ΰ� Process �� �����Ϸ��� �ο� fps �� ��û �������� ������ �߻��ߴ�.
			Sleep( 1 );

			if ( _pData->m_bStop )
			{
				_pData->m_bDestroy = TRUE;
				return 0;
			}

			// CALCULATE_DATA_MODE �� �ɶ� ���� ��ٸ���.
			if ( g_sData.m_emMode_Wait == COMPLETE_DATA_MODE )
				continue;

			// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
			// �̰� ��Ȯ������ �ʴ�. ������ �� �̰��� Sleep �� �ɷ������� ��ġ�� ũ�� ���� ���� �ִ�.
#ifdef _RELEASED
			DWORD dwTime = timeGetTime();
#endif

			// Main Thread �� Render() �󿡼� ������ �Ǿ� �Ǹ� DxEffSingleGroup::m_bNeedSetVB ���� TRUE �� ���Ҵ�.
			// �װ� ���⼭ IsNeedSetVB �� Ȯ�� �� VertexBuffer ������ �Űܾ� �� �͸� �ű��.
			std::list<boost::shared_ptr<SingleForThread>>::iterator iter = g_listSingleEffect.begin();
			for ( ; iter!=g_listSingleEffect.end(); ++iter )
			{
				if ( (*iter)->IsRenderCulling() )
				{
					(*iter)->FrameMove( g_sCommonData.m_fElapsedTime );
					(*iter)->SetVertices( g_sCommonData.m_fElapsedTime, g_sCommonData.m_matCameraView );
				}
			}

			// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
#ifdef _RELEASED
			_pData->m_dwCalculateTime = timeGetTime()-dwTime;
#endif

			// �Ϸ�Ǿ��ٰ� �˸���.
			_pData->m_emMode_Wait = COMPLETE_DATA_MODE;
		}

		return 0;
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread ������ ����

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

	// ������ ĳ���� ����� ���� ������Ѵ�.
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

		// clear �� ������ ������ �޸� Leak �� �����. ( SPDXEFFSINGLEGROUP �� ������ ���� �̷������ �ʾ� TextureResource ������ ���� �̷������ �ʴ´�. )
		g_listSingleEffect.clear();
		g_listSingleEffectGet.clear();
	}




	//////////////////////////////////////////////////////////////////////////
	// 

	// ��� �Ϸᰡ �Ǿ����� Ȯ���Ѵ�.
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
			// Particle Thread �� �۾��� �ȵǾ��� ���, ��ƼŬ ���� Update �� ���� �ʵ��� �Ѵ�.
			return;
		}

		//// COMPLETE_DATA_MODE �� ������ ���.
		//while ( !IsCompleteData() )
		//{
		//}

		// �������� ����.
		g_sCommonData.InsertCommonData( g_fElapsedTime, DxViewPort::GetInstance().GetMatView() );
		g_fElapsedTime = 0.f;

		// ��� �� �� SingleForThread ����
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

		// ���ο� SingleForThread �߰�
		iter = g_listSingleEffectGet.begin();
		for ( ; iter!=g_listSingleEffectGet.end(); ++iter )
		{
			g_listSingleEffect.push_back( (*iter) );
		}
		g_listSingleEffectGet.clear();


		//////////////////////////////////////////////////////////////////////////
		// [shhan][2016.01.20] g_listSingleEffectGet ���� ��� ������ �Ǿ crash �� �߻��Ͽ�,
		//						g_listSingleEffectGet.clear(); �۾� �� bNotRendering Ȯ�� �� TRUE �� return �ϵ��� �Ѵ�.
		//////////////////////////////////////////////////////////////////////////
		// �ּ�ȭ�� �Ͽ� Rendering �� �ʿ� ���� ���
		if ( bNotRendering )
		{
#ifdef _RELEASED
			CDebugSet::ToView( 14, _T("EffectThread _ Work - Thread : 0ff") );
#endif
			return;
		}


		// swap �� �Ѵ�.
		NSEffectThread::Swap( pd3dDevice );

		// swap �Ϸ�Ǹ� Thread �� �ٽ� ����϶�� �˷��ش�.
		g_sData.m_emMode_Wait = CALCULATE_DATA_MODE;
	}

	//////////////////////////////////////////////////////////////////////////
	// �̶��� Main Thread
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