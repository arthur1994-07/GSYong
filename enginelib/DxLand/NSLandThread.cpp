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
	DWORD	g_dwThreadID;		//	������ ID
	HANDLE	g_hThread(NULL);	//	������ �ڵ�

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
		DWORD							m_dwCalculateTimeField;
		DWORD							m_dwCalculateTimePieceBase;
		DWORD							m_dwCalculateTimePieceReflect;

		// CV �۾����ְ� ������ �Ѱ��ִ� class
		boost::scoped_ptr<PieceClipVolumeThread>		m_scpPieceClipVolumeThread;
		
		// Entity ���� ����.
		boost::weak_ptr<EntityRenderListSwap>			m_wpEntityRenderListSwap[TYPE_CULL_MODE_SIZE];		// Entity( Piece, LandSkin )

		// Field ���� Mesh ����.
		boost::scoped_ptr<DxFieldMeshForThreadPackage>	m_scpFieldMeshForThreadPackage[TYPE_CULL_MODE_SIZE];

		// Instancing Render ����
		// Thread �󿡼� �����Ѵ�.
		boost::weak_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_wpPieceInstancingMeshLM;							// �⺻���� Piece Instancing.
		boost::weak_ptr<PieceInstancingThread<PIECE_INSTANCING_FIRST,DxPieceInstancing>>		m_wpPieceInstancingMeshReflection;					// �ݻ� Piece Instancing.
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
			// �缳��
			m_scpPieceClipVolumeThread.reset(new PieceClipVolumeThread);

			// �缳��.
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

			// ���� �۾��� �Ѵ�.
			for ( DWORD i=0; i<TYPE_CULL_MODE_SIZE; ++i )
			{
				_pData->m_scpFieldMeshForThreadPackage[i]->CalculateTHREAD();
			}

			// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
#ifdef _RELEASED
			_pData->m_dwCalculateTimeField = timeGetTime()-dwTime;
#endif
			// Piece �۾��� �Ѵ�.
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

				// ���� Piece Alpha ���� ������ ���� ���ȴ�.
				// CalculateRenderListTHREAD �ȿ� �־����� �ߺ� ����� �Ǿ �̰����� �Ű��.
				// �ݻ縸 �Ǵ°� Alpha ������ �ȵ� �� �ִ�. ( �۾���뿡 ���� ���̴� ������ �����Ƿ� �Ѿ��� �Ѵ�. )
				spEntity->GetEntityRenderListForThread()->FrameMoveTHREAD( NSPieceQuickSort::g_fElapsedTime );
			}


			// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
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

			// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
#ifdef _RELEASED
			_pData->m_dwCalculateTimePieceReflect = timeGetTime() - dwTime - _pData->m_dwCalculateTimeField - _pData->m_dwCalculateTimePieceBase;
#endif

			// �⺻, Reflection, CSM#1, CSM#2  <Instancing ����>
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

			// SpotLight �� ����. <Instancing �� ������ ����>
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

		// �ּ�ȭ�� �Ͽ� Rendering �� �ʿ� ���� ���
		if ( bNotRendering )
		{
			CDebugSet::ToView( 13, _T("LandThread _ Work - Thread : 0ff") );

			return;
		}

		// ���� Qulity �����۾�.
		TnL_MODE			emGroundQulityThreadPrev;
		{
			emGroundQulityThreadPrev = RENDERPARAM::g_emGroundQulityTHREAD;
			RENDERPARAM::g_emGroundQulityTHREAD = RENDERPARAM::g_emTnLMode;

			if ( bUsedMaterialSystemPS_3_0 )
			{
				//// �ɼ��� CSM ������ ������ CSM �ɼ��� ���� ��� �ܰ踦 ���缭 �������Ѵ�.
				//if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_CSM && !NSShadowLight::IsCascadeShadow() )
				//{
				//	RENDERPARAM::g_emGroundQulityTHREAD = TnL_PS_3_0_NM;
				//}
			}
			else if ( bUsedMaterialSystem )
			{
				// ps.2.0 �ʰ� ������ ps.2.0 �������� �����ش�.
				if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_3_0_DF )
				{
					RENDERPARAM::g_emGroundQulityTHREAD = TnL_PS_3_0_DF;
				}
			}
			else
			{
				// ps.1.1 �ʰ� ������ ps.1.1 �������� �����ش�.
				if ( RENDERPARAM::g_emGroundQulityTHREAD >= TnL_PS_1_1 )
				{
					RENDERPARAM::g_emGroundQulityTHREAD = TnL_PS_1_1;
				}
			}
		}

		// ���� ���õǾ� �־��ٸ�
		if ( RENDERPARAM::g_emGroundQulity == TnL_NONE )
		{
			// ó�� ���õǴ°Ŷ�� ������� ������ �Ѵ�.
			RENDERPARAM::g_emGroundQulity = RENDERPARAM::g_emGroundQulityTHREAD;
		}
		else
		{
			// ���� Thread ���� ����ߴ����� �Ͽ��ϵ��� �Ѵ�.
			RENDERPARAM::g_emGroundQulity = emGroundQulityThreadPrev;
		}

		// swap �� �Ѵ�.
		NSLandThread::Swap();

		// SpotLight ����.
		// ���� �׸��ڸ� �����ϵ��� �Ѵ�. 
		// PS_3_0 �̻��� ��츸 �����Ѵ�.
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			// �׸��� �������� NSShadowLight::FrameMove �� ȣ��������Ѵ�.
			NSShadowLight::FrameMove( fElapsedTime );
		}

		// Shadow �� �ʿ��� Matrix �� CV �� ���Ѵ�.
		DxShadowMap::GetInstance().CaculateViewProjForMaterialSystemALL();

		// CV ���ϱ�
		const CLIPVOLUME* arraypCV[NSLandThread::TYPE_CULL_MODE_SIZE];
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			arraypCV[i] = NULL;
		}

		arraypCV[NSLandThread::TYPE_BASEMESH_CULL] = &cv;

		// WorldEdit ���� Direction Light�� Shadow�� �Ⱦ��ڴٰ� �Ͽ� CascadeShadow �� ��� �Ұ���.
		if ( NSShadowLight::IsCascadeShadow() )
		{
			// ���� Qulity �ɼǿ����� CascadeShadow �� ���� ���� ����.
			// ������ ���� ���̶� ���� �� ���� ���� �ְ�,
			// �ɼ��� ���缭 ���� �� ���� ���� �ִ�.
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

		// ElapsedTime �ֱ� ����.
		NSPieceQuickSort::FrameMove( fTime, fElapsedTime );

		// �۾��� ���� Thread �� �˷��ְ�, ������ �˸���.
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
		// ������ ��ġ�� ã��.
		// �Ϸ�� ������ ����Ѵ�.
		while ( !NSLandThread::IsCompleteData() )
		{
		}

		// swap �� �Ѵ�.
		NSLandThread::Swap();

		// CV ���ϱ�
		const CLIPVOLUME* arraypCV[NSLandThread::TYPE_CULL_MODE_SIZE];
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			arraypCV[i] = NULL;
		}

		arraypCV[NSLandThread::TYPE_BASEMESH_CULL] = &cv;

		// �۾��� ���� Thread �� �˷��ְ�, ������ �˸���.
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
		// CV ���ϱ�
		const CLIPVOLUME* arraypCV[NSLandThread::TYPE_CULL_MODE_SIZE];
		for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
		{
			arraypCV[i] = NULL;
		}

		arraypCV[NSLandThread::TYPE_BASEMESH_CULL] = &cv;

		// �۾��� ���� Thread �� �˷��ְ�, ������ �˸���.
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
		// ������ ��ġ�� ã��.
		// �Ϸ�� ������ ����Ѵ�.
		while ( !NSLandThread::IsCompleteData() )
		{
		}

		// swap �� �Ѵ�.
		NSLandThread::Swap();
	}

	// ��� �Ϸᰡ �Ǿ����� Ȯ���Ѵ�.
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

	// �̶��� Main Thread
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

		// ����� �����϶�� �����Ѵ�.
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