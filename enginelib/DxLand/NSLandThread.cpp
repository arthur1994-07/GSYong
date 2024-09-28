#include "pch.h"

#include <vector>
#include <process.h>
#include <boost/scoped_ptr.hpp>

#include "../../BugTrap/BugTrap.h"
#include "../DxTools/Light/NSShadowLight.h"
#include "../DxTools/DxEnvironment.h"
#include "../DxTools/DxShadowMap.h"
#include "./DxPiece/DxPieceInstancing.h"
#include "./DxCullingMeshForThread.h"
#include "./PieceClipVolumeThread.h"
#include "./PieceInstancingThread.h"
#include "./EntityRenderList.h"

#include "NSLandThread.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSLandThread
{
	DWORD	g_dwThreadID;		//	스레드 ID
	HANDLE	g_hThread(NULL);	//	스레드 핸들

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
		DWORD							m_dwCalculateTimeField;
		DWORD							m_dwCalculateTimePieceBase;
		DWORD							m_dwCalculateTimePieceReflect;

		// CV 작업해주고 정보를 넘겨주는 class
		boost::scoped_ptr<PieceClipVolumeThread>		m_scpPieceClipVolumeThread;
		
		// Entity 관련 모음.
		boost::weak_ptr<EntityRenderListSwap>			m_wpEntityRenderListSwap[TYPE_CULL_MODE_SIZE];		// Entity( Piece, LandSkin )

		// Field 관련 Mesh 모음.
		boost::scoped_ptr<DxFieldMeshForThreadPackage>	m_scpFieldMeshForThreadPackage[TYPE_CULL_MODE_SIZE];

		// Instancing Render 정보
		// Thread 상에서 공유한다.
		boost::weak_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_wpPieceInstancingMeshLM;							// 기본적인 Piece Instancing.
		boost::weak_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_wpPieceInstancingMeshReflection;					// 반사 Piece Instancing.
		boost::weak_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>	m_wpPieceInstancingPureMesh[TYPE_INSTANCING_PURE_MESH_SIZE];	// Shadow Piece Instancing.
		

		DATA()
			: m_bStop(FALSE)
			, m_bDestroy(FALSE)
			, m_emMode_Wait(COMPLETE_DATA_MODE)
			, m_dwCalculateTime(0)
			, m_dwCalculateTimeField(0)
			, m_dwCalculateTimePieceBase(0)
			, m_dwCalculateTimePieceReflect(0)
			, m_scpPieceClipVolumeThread(new PieceClipVolumeThread)
		{
			for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
			{
				//m_wpEntityRenderListSwap[i].reset( new EntityRenderListThread );
				m_wpEntityRenderListSwap[i].reset();
				m_scpFieldMeshForThreadPackage[i].reset( new DxFieldMeshForThreadPackage );
			}
		}

		void DataReset()
		{
			// 재설정
			m_scpPieceClipVolumeThread.reset(new PieceClipVolumeThread);

			// 재설정.
			for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
			{
				//m_wpEntityRenderListSwap[i].reset( new EntityRenderListThread );
				m_wpEntityRenderListSwap[i].reset();
				m_scpFieldMeshForThreadPackage[i].reset( new DxFieldMeshForThreadPackage );
			}

			m_wpPieceInstancingMeshLM.reset();
			m_wpPieceInstancingMeshReflection.reset();
			for ( DWORD i=0; i<TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
			{
				m_wpPieceInstancingPureMesh[i].reset();
			}
		}
	};
	DATA g_sData;



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

			// 지형 작업을 한다.
			for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
			{
				_pData->m_scpFieldMeshForThreadPackage[i]->CalculateTHREAD();
			}

			// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
			_pData->m_dwCalculateTimeField = timeGetTime()-dwTime;
#endif
			// Piece 작업을 한다.
			boost::shared_ptr<EntityRenderListSwap> spEntity = _pData->m_wpEntityRenderListSwap[TYPE_BASEMESH_CULL].lock();
			if ( spEntity )
			{
				boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>> spPieceInstancing = _pData->m_wpPieceInstancingMeshLM.lock();
				_pData->m_scpPieceClipVolumeThread->CalculateRenderListTHREAD
				( 
					TYPE_BASEMESH_CULL, 
					spEntity->GetEntityRenderListForThread().get(), 
					spPieceInstancing ? spPieceInstancing.get() : NULL
				);

				// 현재 Piece Alpha 관련 변경을 위해 사용된다.
				// CalculateRenderListTHREAD 안에 있었지만 중복 계산이 되어서 이곳으로 옮겼다.
				// 반사만 되는건 Alpha 변동이 안될 수 있다. ( 작업비용에 비해 보이는 문제가 작으므로 넘어가기로 한다. )
				spEntity->GetEntityRenderListForThread()->FrameMoveTHREAD( NSPieceQuickSort::g_fElapsedTime );
			}


			// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
			_pData->m_dwCalculateTimePieceBase = timeGetTime() - dwTime - _pData->m_dwCalculateTimeField;
#endif

			spEntity = _pData->m_wpEntityRenderListSwap[TYPE_REFLECTION_CULL].lock();
			if ( spEntity )
			{
				boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>> spPieceInstancing = _pData->m_wpPieceInstancingMeshReflection.lock();
				_pData->m_scpPieceClipVolumeThread->CalculateRenderListTHREAD
				( 
					TYPE_REFLECTION_CULL, 
					spEntity->GetEntityRenderListForThread().get(), 
					spPieceInstancing ? spPieceInstancing.get() : NULL
				);
			}

			// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
			_pData->m_dwCalculateTimePieceReflect = timeGetTime() - dwTime - _pData->m_dwCalculateTimeField - _pData->m_dwCalculateTimePieceBase;
#endif

			// 기본, Reflection, CSM#1, CSM#2  <Instancing 사용됨>
			for ( DWORD i=0; i<TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
			{
				boost::shared_ptr<EntityRenderListSwap> spEntity = _pData->m_wpEntityRenderListSwap[i].lock();
				if ( spEntity )
				{
					boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>> spPieceInstancing = _pData->m_wpPieceInstancingPureMesh[i].lock();
					_pData->m_scpPieceClipVolumeThread->CalculateRenderListTHREAD_Shadow
					( 
						static_cast<NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE>(i), 
						spEntity->GetEntityRenderListForThread().get(), 
						spPieceInstancing ? spPieceInstancing.get() : NULL
					);
				}
			}

			// SpotLight 만 동작. <Instancing 은 사용되지 않음>
			for ( DWORD i=TYPE_SPOTSHADOW_FIRST_CULL; i<TYPE_CULL_MODE_SIZE; ++i )
			{
				boost::shared_ptr<EntityRenderListSwap> spEntity = _pData->m_wpEntityRenderListSwap[i].lock();
				if ( spEntity )
				{
					_pData->m_scpPieceClipVolumeThread->CalculateRenderListTHREAD_Spot_Shadow
					( 
						static_cast<NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE>(i), 
						spEntity->GetEntityRenderListForThread().get()
					);
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
		g_hThread = (HANDLE) ::_beginthreadex( NULL,
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
	}




	//////////////////////////////////////////////////////////////////////////
	// 
	void Wait_FrameMove( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[],
							const CLIPVOLUME &cv,
							float fTime, 
							float fElapsedTime, 
							BOOL bUsedMaterialSystem,
							BOOL bUsedMaterialSystemPS_3_0,
							float fWaterHeight,
							BOOL bNotRendering )
	{
		while ( !NSLandThread::IsCompleteData() )
		{
		}

		// 최소화를 하여 Rendering 이 필요 없을 경우
		if ( bNotRendering )
		{
			CDebugSet::ToView( 13, _T("LandThread _ Work - Thread : 0ff") );

			return;
		}

		// 지형 Qulity 셋팅작업.
		TnL_MODE			emGroundQulityThreadPrev;
		{
			emGroundQulityThreadPrev = RENDERPARAM::g_emGroundQulityTHREAD;
			RENDERPARAM::g_emGroundQulityTHREAD = RENDERPARAM::g_emTnLMode;

			if ( bUsedMaterialSystemPS_3_0 )
			{
				//// 옵션은 CSM 이지만 지형이 CSM 옵션이 없을 경우 단계를 낮춰서 렌더링한다.
				//if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_CSM && !NSShadowLight::IsCascadeShadow() )
				//{
				//	RENDERPARAM::g_emGroundQulityTHREAD = TnL_PS_3_0_NM;
				//}
			}
			else if ( bUsedMaterialSystem )
			{
				// ps.2.0 초과 버젼을 ps.2.0 버젼으로 맞춰준다.
				if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_DF )
				{
					RENDERPARAM::g_emGroundQulityTHREAD = TnL_PS_3_0_DF;
				}
			}
			else
			{
				// ps.1.1 초과 버젼을 ps.1.1 버젼으로 맞춰준다.
				if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_1_1 )
				{
					RENDERPARAM::g_emGroundQulityTHREAD = TnL_PS_1_1;
				}
			}
		}

		// 값이 셋팅되어 있었다면
		if ( RENDERPARAM::g_emGroundQulity == TnL_NONE )
		{
			// 처음 셋팅되는거라면 현재것을 쓰도록 한다.
			RENDERPARAM::g_emGroundQulity = RENDERPARAM::g_emGroundQulityTHREAD;
		}
		else
		{
			// 이전 Thread 에서 사용했던것을 하용하도록 한다.
			RENDERPARAM::g_emGroundQulity = emGroundQulityThreadPrev;
		}

		// swap 을 한다.
		NSLandThread::Swap();

		// SpotLight 셋팅.
		// 지형 그림자를 생성하도록 한다. 
		// PS_3_0 이상일 경우만 동작한다.
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			// 그림자 렌더전에 NSShadowLight::FrameMove 를 호출해줘야한다.
			NSShadowLight::FrameMove( fElapsedTime );
		}

		// Shadow 에 필요한 Matrix 와 CV 를 구한다.
		DxShadowMap::GetInstance().CaculateViewProjForMaterialSystemALL();

		// CV 구하기
		const CLIPVOLUME* arraypCV[NSLandThread::TYPE_CULL_MODE_SIZE];
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			arraypCV[i] = NULL;
		}

		arraypCV[NSLandThread::TYPE_BASEMESH_CULL] = &cv;

		// WorldEdit 에서 Direction Light의 Shadow를 안쓰겠다고 하여 CascadeShadow 를 사용 불가됨.
		if ( NSShadowLight::IsCascadeShadow() )
		{
			// 현재 Qulity 옵션에서는 CascadeShadow 가 쓰일 수가 없다.
			// 지형이 이전 맵이라서 쓰일 수 없을 수도 있고,
			// 옵션을 낮춰서 쓰일 수 없을 수도 있다.
			if ( RENDERPARAM::g_emGroundQulityTHREAD == TnL_PS_3_0_CSM )
			{
				arraypCV[NSLandThread::TYPE_SHADOW_FIRST_CULL] = &DxShadowMap::GetInstance().GetClipVolume();
				arraypCV[NSLandThread::TYPE_SHADOW_SECOND_CULL] = &DxShadowMap::GetInstance().GetClipVolume_2nd();
			}
			else if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_NM )
			{
				arraypCV[NSLandThread::TYPE_SHADOW_FIRST_CULL] = &DxShadowMap::GetInstance().GetClipVolume();
			}
		}

		if ( RENDERPARAM::g_emGroundQulityTHREAD == TnL_PS_3_0_CSM )
		{
			if ( DxShadowMap::GetInstance().GetSpotLightID_PL1() != static_cast<DWORD>(-1) )
			{
				arraypCV[NSLandThread::TYPE_SPOTSHADOW_FIRST_CULL] = &DxShadowMap::GetInstance().GetClipVolume_PL1();
			}
			if ( DxShadowMap::GetInstance().GetSpotLightID_PL2() != static_cast<DWORD>(-1) )
			{
				arraypCV[NSLandThread::TYPE_SPOTSHADOW_SECOND_CULL] = &DxShadowMap::GetInstance().GetClipVolume_PL2();
			}
		}

		if ( DxEnvironment::GetInstance().IsReflectionCV() )
		{
			arraypCV[NSLandThread::TYPE_REFLECTION_CULL] = &DxEnvironment::GetInstance().GetReflectionCV();
		}

		// ElapsedTime 넣기 위함.
		NSPieceQuickSort::FrameMove( fTime, fElapsedTime );

		// 작업할 것을 Thread 에 알려주고, 시작을 알린다.
		NSLandThread::AddRenderList_CALCULATE_DATA_MODE
		( 
			spPieceRenderListSwap,
			arraypCV,
			DxViewPort::GetInstance().GetFromPt(), 
			DxViewPort::GetInstance().GetLookatPt(),
			bUsedMaterialSystem ? TRUE : FALSE,		// bTimeBlend,
			fWaterHeight,
			TRUE
		);
	}

	void Wait_FrameMoveRAD( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[],
							const CLIPVOLUME &cv )
	{
		//////////////////////////////////////////////////////////////////////////
		// 적당한 위치를 찾자.
		// 완료될 때까지 대기한다.
		while ( !NSLandThread::IsCompleteData() )
		{
		}

		// swap 을 한다.
		NSLandThread::Swap();

		// CV 구하기
		const CLIPVOLUME* arraypCV[NSLandThread::TYPE_CULL_MODE_SIZE];
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			arraypCV[i] = NULL;
		}

		arraypCV[NSLandThread::TYPE_BASEMESH_CULL] = &cv;

		// 작업할 것을 Thread 에 알려주고, 시작을 알린다.
		NSLandThread::AddRenderList_CALCULATE_DATA_MODE
		( 
			spPieceRenderListSwap,
			arraypCV,
			DxViewPort::GetInstance().GetFromPt(), 
			DxViewPort::GetInstance().GetLookatPt(),
			TRUE,
			0.f,
			TRUE
		);
	}

	void FrameMoveRAD( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[], const CLIPVOLUME &cv, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt )
	{
		// CV 구하기
		const CLIPVOLUME* arraypCV[NSLandThread::TYPE_CULL_MODE_SIZE];
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			arraypCV[i] = NULL;
		}

		arraypCV[NSLandThread::TYPE_BASEMESH_CULL] = &cv;

		// 작업할 것을 Thread 에 알려주고, 시작을 알린다.
		NSLandThread::AddRenderList_CALCULATE_DATA_MODE
		( 
			spPieceRenderListSwap,
			arraypCV,
			vFromPt, 
			vLookatPt,
			TRUE,
			0.f,
			TRUE
		);

		//////////////////////////////////////////////////////////////////////////
		// 적당한 위치를 찾자.
		// 완료될 때까지 대기한다.
		while ( !NSLandThread::IsCompleteData() )
		{
		}

		// swap 을 한다.
		NSLandThread::Swap();
	}

	// 계산 완료가 되었는지 확인한다.
	BOOL IsCompleteData()
	{
		if ( g_sData.m_emMode_Wait == COMPLETE_DATA_MODE )
		{
#ifdef _RELEASED
			CDebugSet::ToView( 13, _T("LandThread _ Work - Thread : %d, %d, %d, %d"), g_sData.m_dwCalculateTimeField, 
																						g_sData.m_dwCalculateTimePieceBase, 
																						g_sData.m_dwCalculateTimePieceReflect, 
																						g_sData.m_dwCalculateTime );
#endif
			return TRUE;
		}

		return FALSE;
	}

	// 이때는 Main Thread
	void AddRenderList_CALCULATE_DATA_MODE( const boost::shared_ptr<EntityRenderListSwap> spPieceRenderListSwap[],
											const CLIPVOLUME* arraypCV[],
											const D3DXVECTOR3& vFromPt, 
											const D3DXVECTOR3& vLookatPt, 
											BOOL bTimeBlend, 
											float fHeight,
											BOOL bWLD )
	{
		// Field
		for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
		{
			g_sData.m_scpFieldMeshForThreadPackage[i]->InsertNeedData( arraypCV[i] );
		}

		// Piece
		g_sData.m_scpPieceClipVolumeThread->InsertNeedData
		(
			arraypCV,
			vFromPt, 
			vLookatPt, 
			bTimeBlend,
			fHeight,
			bWLD
		);

		for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
		{
			if ( arraypCV[i] )
			{
				g_sData.m_wpEntityRenderListSwap[i] = spPieceRenderListSwap[i];
			}
			else
			{
				g_sData.m_wpEntityRenderListSwap[i].reset();
			}
		}

		// 계산을 진행하라고 셋팅한다.
		g_sData.m_emMode_Wait = CALCULATE_DATA_MODE;
	}

	void Swap()
	{
		// Field
		for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
		{
			g_sData.m_scpFieldMeshForThreadPackage[i]->Swap();
		}

		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			boost::shared_ptr<EntityRenderListSwap> spEntity = g_sData.m_wpEntityRenderListSwap[i].lock();
			if ( spEntity )
			{
				spEntity->Swap();
			}
		}

		// Piece
		boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>> spPieceInstancing = g_sData.m_wpPieceInstancingMeshLM.lock();
		if ( spPieceInstancing )
		{
			spPieceInstancing->Swap();
		}

		spPieceInstancing = g_sData.m_wpPieceInstancingMeshReflection.lock();
		if ( spPieceInstancing )
		{
			spPieceInstancing->Swap();
		}

		for ( DWORD i=0; i<TYPE_INSTANCING_PURE_MESH_SIZE; ++i )
		{
			boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>> spPieceInstancing = g_sData.m_wpPieceInstancingPureMesh[i].lock();
			if ( spPieceInstancing )
			{
				spPieceInstancing->Swap();
			}
		}
	}
	
	void AddCullingMesh( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emType, boost::shared_ptr<DxCullingMeshSwap>& spCullingMeshForThread )
	{
		g_sData.m_scpFieldMeshForThreadPackage[emType]->AddCullingMesh( spCullingMeshForThread );
	}

	void SetPieceAABB( const boost::shared_ptr<DxPieceAABB>& spPieceAABB )
	{
		g_sData.m_scpPieceClipVolumeThread->SetPieceAABB( spPieceAABB );
	}

	void SetLandSkinManager( const boost::shared_ptr<DxLandSkinManager>& spLandSkinManager )
	{
		g_sData.m_scpPieceClipVolumeThread->SetLandSkinManager( spLandSkinManager );
	}

	void SetPieceInstancingMeshLM( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxPieceAABB* pPieceAABB,
									const boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spPieceInstancingThread )
	{
		g_sData.m_wpPieceInstancingMeshLM = spPieceInstancingThread;
		boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spInstancing = g_sData.m_wpPieceInstancingMeshLM.lock();
		if ( spInstancing )
		{
			spInstancing->Import_MeshLM( pd3dDevice,  NSLandThread::TYPE_BASEMESH_CULL, pPieceAABB );
		}
	}

	void SetPieceInstancingMeshReflection( LPDIRECT3DDEVICEQ pd3dDevice, 
											DxPieceAABB* pPieceAABB,
											const boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spPieceInstancingThread )
	{
		g_sData.m_wpPieceInstancingMeshReflection = spPieceInstancingThread;
		boost::shared_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>& spInstancing = g_sData.m_wpPieceInstancingMeshReflection.lock();
		if ( spInstancing )
		{
			spInstancing->Import_MeshLM( pd3dDevice, NSLandThread::TYPE_REFLECTION_CULL, pPieceAABB );
		}
	}

	void SetPieceInstancingPureMesh( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxPieceAABB* pPieceAABB,
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emType, 
										const boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>& spPieceInstancingThread )
	{
		g_sData.m_wpPieceInstancingPureMesh[emType] = spPieceInstancingThread;
		boost::shared_ptr<PieceInstancingThread<DxStaticAniFrame*,DxPieceInstancingForShadow>>& spInstancing = g_sData.m_wpPieceInstancingPureMesh[emType].lock();
		if ( spInstancing )
		{
			spInstancing->Import_PureMesh( pd3dDevice, emType, pPieceAABB );
		}
	}
};