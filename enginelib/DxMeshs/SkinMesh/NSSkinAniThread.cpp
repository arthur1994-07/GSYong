#include "pch.h"

#include <vector>
#include <process.h>

#include "../../../BugTrap/BugTrap.h"

#include "./DxSkinAniControlThreadNeedData.h"
#include "./DxSkinAniControl.h"
#include "./NSSkinAniThreadDef.h"

#include "NSSkinAniThread.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSSkinAniThread
{
	DWORD	g_dwThreadID;		//	스레드 ID
	HANDLE	g_hThread(NULL);	//	스레드 핸들
	UINT	g_unPeriodMin;		//  System 에서 가능한 최소 Period

	// 기본 캐릭터
	struct SKIN_DATA
	{
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;	// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;	// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		D3DXMATRIX												m_matLocalRot;
		//float*								m_rarrayKeyJiggleWeight;
		//DWORD									m_dwCharPartNUM;

		SKIN_DATA( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl,
					std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
					const D3DXMATRIX& matLocalRot )
			: m_spSkinAniControl(spSkinAniControl)
			, m_spThreadNeedData(spThreadNeedData)
			, m_matLocalRot(matLocalRot)
		{
		}
	};

	// Vehicle
	struct SKIN_DATA_VEHICLE
	{
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;			// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;			// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxVehicleThreadNeedData>			m_spVehicleThreadNeedData;	// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		D3DXMATRIX												m_matLocalRot;
		//float*								m_rarrayKeyJiggleWeight;
		//DWORD									m_dwCharPartNUM;

		SKIN_DATA_VEHICLE( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl,
							std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
							std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleThreadNeedData,
							const D3DXMATRIX& matLocalRot )
			: m_spSkinAniControl(spSkinAniControl)
			, m_spThreadNeedData(spThreadNeedData)
			, m_spVehicleThreadNeedData(spVehicleThreadNeedData)
			, m_matLocalRot(matLocalRot)
		{
		}
	};

	// AttachBone
	struct SKIN_DATA_ATTBONE
	{
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;		// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxSkinAniControl>					m_spLinkSkinAniControl;	// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>		m_spThreadNeedData;		// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		float													m_fKeyJiggleWeight;
		float													m_fScale;
		D3DXMATRIX												m_matLocalRot;			// DxSkinAniControl::CalculateBoneAttBone_PureThread 에서 계산된다.

		SKIN_DATA_ATTBONE( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
							std::tr1::shared_ptr<DxSkinAniControl>&	spLinkSkinAniControl, 
							std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData,
							float fKeyJiggleWeight,
							float fScale )
			: m_spSkinAniControl(spSkinAniControl)
			, m_spLinkSkinAniControl(spLinkSkinAniControl)
			, m_spThreadNeedData(spThreadNeedData)
			, m_fKeyJiggleWeight(fKeyJiggleWeight)
			, m_fScale(fScale)
		{
		}
	};

	// Vehicle 에 타고 있는 캐릭터
	struct SKIN_DATA_CHAR_ON_VEHICLE
	{
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;			// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;			// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		std::tr1::shared_ptr<DxVehicleThreadNeedData>			m_spVehicleThreadNeedData;	// 참조로만 했었는데 다른곳에서 삭제할 수도 있어서 shared_ptr 로 바꿈.
		DWORD													m_dwPassengerID;
		D3DXMATRIX												m_matLocalRot;				// DxSkinAniControl::CalculateBoneCharOnVehicle_PureThread 에서 계산된다.

		SKIN_DATA_CHAR_ON_VEHICLE( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
									std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData, 
									std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleThreadNeedData,
									DWORD dwPassengerID )
			: m_spSkinAniControl(spSkinAniControl)
			, m_spThreadNeedData(spThreadNeedData)
			, m_spVehicleThreadNeedData(spVehicleThreadNeedData)
			, m_dwPassengerID(dwPassengerID)
		{
		}
	};

	struct CHATPARTBONEMATRIX
	{
		std::tr1::shared_ptr<CharPartBoneMatrices>	m_spCharPartBoneMatrices;

		// operator= 에도 작업해줘야함.
		CHATPARTBONEMATRIX( std::tr1::shared_ptr<CharPartBoneMatrices>&	spCharPartBoneMatrices )
			: m_spCharPartBoneMatrices(spCharPartBoneMatrices)
		{
		}
	};
	
	

	enum MODE
	{
		CALCULATE_DATA					= 0x0,	// 계산이 진행되는 시기
		COMPLETE_CHAR_DATA				= 0x1,	// 종료
	};

	enum MODE_WAIT
	{
		CALCULATE_CHAR_ANI_DATA			= 0x0,	// 계산이 진행되는 시기
		COMPLETE_CHAR_ANI_DATA			= 0x1,	// 종료
	};

	struct SKIN_ANIMATION_THREAD
	{
		// 모으는 데이터.
		std::vector<SKIN_DATA>					m_vecSkinDataGet;
		std::vector<SKIN_DATA_VEHICLE>			m_vecSkinDataGet_Vehicle;
		std::vector<SKIN_DATA_ATTBONE>			m_vecSkinDataGet_AttBone;
		std::vector<SKIN_DATA_CHAR_ON_VEHICLE>	m_vecSkinDataGet_CharOnVehicle;

		// Render 작업.
		std::vector<SKIN_DATA>					m_vecSkinData;
		std::vector<SKIN_DATA_VEHICLE>			m_vecSkinData_Vehicle;
		std::vector<SKIN_DATA_ATTBONE>			m_vecSkinData_AttBone;
		std::vector<SKIN_DATA_CHAR_ON_VEHICLE>	m_vecSkinData_CharOnVehicle;

		// CharPart 의 BoneMatrices
		std::vector<CHATPARTBONEMATRIX>			m_vecCharPartBoneMatricesGet;
		std::vector<CHATPARTBONEMATRIX>			m_vecCharPartBoneMatrices;

		void Thread_FrameMoveForWait( MODE_WAIT* pemMode )
		{
			if ( *pemMode == CALCULATE_CHAR_ANI_DATA )
			{
				Thread_FrameMove( pemMode, NULL );

				*pemMode = COMPLETE_CHAR_ANI_DATA;
			}
		}

		void Thread_FrameMove_ForCharEditer()
		{
			//////////////////////////////////////////////////////////////////////////
			// Bone 을 계산한다.
			// 순서가 정말로 중요하다.

			// 캐릭터와 Vehicle 을 계산한다.
			for ( DWORD i=0; i<m_vecSkinDataGet.size(); ++i )
			{
				// TAG_CalculateAnimationThread_1
				m_vecSkinDataGet[i].m_spSkinAniControl->CalculateBone_PureThread
				( 
					m_vecSkinDataGet[i].m_matLocalRot, 
					m_vecSkinDataGet[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinDataGet[i].m_spThreadNeedData->m_dwCharPartNUM,
					m_vecSkinDataGet[i].m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot
				);
			}

			// 캐릭터와 Vehicle 을 계산한다.
			for ( DWORD i=0; i<m_vecSkinDataGet_Vehicle.size(); ++i )
			{
				// TAG_CalculateAnimationThread_1
				m_vecSkinDataGet_Vehicle[i].m_spSkinAniControl->CalculateBoneVehicle_PureThread
				( 
					m_vecSkinDataGet_Vehicle[i].m_spVehicleThreadNeedData,
					m_vecSkinDataGet_Vehicle[i].m_matLocalRot, 
					m_vecSkinDataGet_Vehicle[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinDataGet_Vehicle[i].m_spThreadNeedData->m_dwCharPartNUM
				);
			}

			// Vehicle 을 타고 있는 캐릭터를 계산한다. ( Vehicle Bone 정보가 필요함. )
			for ( DWORD i=0; i<m_vecSkinDataGet_CharOnVehicle.size(); ++i )
			{
				// TAG_CalculateAnimationThread_1
				m_vecSkinDataGet_CharOnVehicle[i].m_spSkinAniControl->CalculateBoneCharOnVehicle_PureThread
				( 
					m_vecSkinDataGet_CharOnVehicle[i].m_matLocalRot, 
					m_vecSkinDataGet_CharOnVehicle[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinDataGet_CharOnVehicle[i].m_spThreadNeedData->m_dwCharPartNUM,
					m_vecSkinDataGet_CharOnVehicle[i].m_spVehicleThreadNeedData,
					m_vecSkinDataGet_CharOnVehicle[i].m_dwPassengerID,
					m_vecSkinDataGet_CharOnVehicle[i].m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot
				);
			}

			// AttachBone을 계산한다. ( Vehicle Bone 혹은 캐릭터 본 정보가 필요함. )
			for ( DWORD i=0; i<m_vecSkinDataGet_AttBone.size(); ++i )
			{
				// TAG_CalculateAnimationThread_1
				m_vecSkinDataGet_AttBone[i].m_spSkinAniControl->CalculateBoneAttBone_PureThread
				( 
					m_vecSkinDataGet_AttBone[i].m_matLocalRot,
					m_vecSkinDataGet_AttBone[i].m_spLinkSkinAniControl,
					m_vecSkinDataGet_AttBone[i].m_spThreadNeedData,
					m_vecSkinDataGet_AttBone[i].m_fKeyJiggleWeight,
					m_vecSkinDataGet_AttBone[i].m_fScale
				);
			}
		}

		void Thread_FrameMove_Bone( MODE_WAIT* pemMode, SKIN_ANIMATION_THREAD* pSkinAnimationWait )
		{
			//////////////////////////////////////////////////////////////////////////
			// Bone 을 계산한다.
			// 순서가 정말로 중요하다.

			// 캐릭터와 Vehicle 을 계산한다.
			for ( DWORD i=0; i<m_vecSkinData.size(); ++i )
			{
				if ( m_vecSkinData[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// 원본이 삭제된 상태다.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData[i].m_spSkinAniControl->CalculateBone_PureThread
				( 
					m_vecSkinData[i].m_matLocalRot, 
					m_vecSkinData[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinData[i].m_spThreadNeedData->m_dwCharPartNUM,
					m_vecSkinData[i].m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot
				);

				// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}

			// 캐릭터와 Vehicle 을 계산한다.
			for ( DWORD i=0; i<m_vecSkinData_Vehicle.size(); ++i )
			{
				if ( m_vecSkinData_Vehicle[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// 원본이 삭제된 상태다.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData_Vehicle[i].m_spSkinAniControl->CalculateBoneVehicle_PureThread
				( 
					m_vecSkinData_Vehicle[i].m_spVehicleThreadNeedData,
					m_vecSkinData_Vehicle[i].m_matLocalRot, 
					m_vecSkinData_Vehicle[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinData_Vehicle[i].m_spThreadNeedData->m_dwCharPartNUM
				);

				// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}

			// Vehicle 을 타고 있는 캐릭터를 계산한다. ( Vehicle Bone 정보가 필요함. )
			for ( DWORD i=0; i<m_vecSkinData_CharOnVehicle.size(); ++i )
			{
				if ( m_vecSkinData_CharOnVehicle[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// 원본이 삭제된 상태다.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData_CharOnVehicle[i].m_spSkinAniControl->CalculateBoneCharOnVehicle_PureThread
				( 
					m_vecSkinData_CharOnVehicle[i].m_matLocalRot, 
					m_vecSkinData_CharOnVehicle[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinData_CharOnVehicle[i].m_spThreadNeedData->m_dwCharPartNUM,
					m_vecSkinData_CharOnVehicle[i].m_spVehicleThreadNeedData,
					m_vecSkinData_CharOnVehicle[i].m_dwPassengerID,
					m_vecSkinData_CharOnVehicle[i].m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot
				);

				// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}

			// AttachBone을 계산한다. ( Vehicle Bone 혹은 캐릭터 본 정보가 필요함. )
			for ( DWORD i=0; i<m_vecSkinData_AttBone.size(); ++i )
			{
				if ( m_vecSkinData_AttBone[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// 원본이 삭제된 상태다.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData_AttBone[i].m_spSkinAniControl->CalculateBoneAttBone_PureThread
				( 
					m_vecSkinData_AttBone[i].m_matLocalRot,
					m_vecSkinData_AttBone[i].m_spLinkSkinAniControl,
					m_vecSkinData_AttBone[i].m_spThreadNeedData,
					m_vecSkinData_AttBone[i].m_fKeyJiggleWeight,
					m_vecSkinData_AttBone[i].m_fScale
				);

				// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}
		}

		void Thread_FrameMove( MODE_WAIT* pemMode, SKIN_ANIMATION_THREAD* pSkinAnimationWait )
		{
			Thread_FrameMove_Bone( pemMode, pSkinAnimationWait );

			// CalculateBone_PureThread 작업 완료 후에 해야한다. SwapBone 전에 있어야 한다.
			// DxSkinAniControl::m_pSkeletonBackUp 값을 사용한다.
			for ( DWORD i=0; i<m_vecCharPartBoneMatrices.size(); ++i )
			{
				if ( m_vecCharPartBoneMatrices[i].m_spCharPartBoneMatrices.use_count() <= 1 )
					continue;	// 원본이 삭제된 상태다.

				m_vecCharPartBoneMatrices[i].m_spCharPartBoneMatrices->FrameMove_PureThread();

				// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}
		}

		void SwapBoneForCharEditer( float fElapsedTime )
		{
			//////////////////////////////////////////////////////////////////////////
			// Complete 된 Data 들의 Swap 작업을 한다.
			for ( DWORD i=0; i<m_vecSkinDataGet.size(); ++i )
			{
				m_vecSkinDataGet[i].m_spSkinAniControl->SwapBone( m_vecSkinDataGet[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecSkinDataGet_Vehicle.size(); ++i )
			{
				m_vecSkinDataGet_Vehicle[i].m_spSkinAniControl->SwapBone( m_vecSkinDataGet_Vehicle[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecSkinDataGet_AttBone.size(); ++i )
			{
				m_vecSkinDataGet_AttBone[i].m_spSkinAniControl->SwapBone( m_vecSkinDataGet_AttBone[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecSkinDataGet_CharOnVehicle.size(); ++i )
			{
				m_vecSkinDataGet_CharOnVehicle[i].m_spSkinAniControl->SwapBone( m_vecSkinDataGet_CharOnVehicle[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecCharPartBoneMatricesGet.size(); ++i )
			{
				m_vecCharPartBoneMatricesGet[i].m_spCharPartBoneMatrices->SwapBoneMatrices( fElapsedTime );
			}
		}

		void SwapBone( float fElapsedTime )
		{
			//////////////////////////////////////////////////////////////////////////
			// Complete 된 Data 들의 Swap 작업을 한다.
			for ( DWORD i=0; i<m_vecSkinData.size(); ++i )
			{
				m_vecSkinData[i].m_spSkinAniControl->SwapBone( m_vecSkinData[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecSkinData_Vehicle.size(); ++i )
			{
				m_vecSkinData_Vehicle[i].m_spSkinAniControl->SwapBone( m_vecSkinData_Vehicle[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecSkinData_AttBone.size(); ++i )
			{
				m_vecSkinData_AttBone[i].m_spSkinAniControl->SwapBone( m_vecSkinData_AttBone[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecSkinData_CharOnVehicle.size(); ++i )
			{
				m_vecSkinData_CharOnVehicle[i].m_spSkinAniControl->SwapBone( m_vecSkinData_CharOnVehicle[i].m_matLocalRot );
			}
			for ( DWORD i=0; i<m_vecCharPartBoneMatrices.size(); ++i )
			{
				m_vecCharPartBoneMatrices[i].m_spCharPartBoneMatrices->SwapBoneMatrices( fElapsedTime );
			}
		}

		// [shhan][2015.03.25] m_sSkinAnimationWait 는 CopyGetToSet 삭제 전에 SwapBone 을 해줘야 한다.
		//						만약 해주지 않는다면 지속적인 메모리 증가가 생길 수가 있다.
		void CopyGetToSet()
		{
			m_vecSkinData = m_vecSkinDataGet;
			m_vecSkinData_Vehicle = m_vecSkinDataGet_Vehicle;
			m_vecSkinData_AttBone = m_vecSkinDataGet_AttBone;
			m_vecSkinData_CharOnVehicle = m_vecSkinDataGet_CharOnVehicle;
			m_vecCharPartBoneMatrices = m_vecCharPartBoneMatricesGet;
		}

		void ClearGetData()
		{
			m_vecSkinDataGet.clear();
			m_vecSkinDataGet_Vehicle.clear();
			m_vecSkinDataGet_AttBone.clear();
			m_vecSkinDataGet_CharOnVehicle.clear();

			m_vecCharPartBoneMatricesGet.clear();
		}

		void ClearSetData()
		{
			m_vecSkinData.clear();
			m_vecSkinData_Vehicle.clear();
			m_vecSkinData_AttBone.clear();
			m_vecSkinData_CharOnVehicle.clear();

			m_vecCharPartBoneMatrices.clear();
		}
	};

	struct DATA
	{		
		BOOL							m_bStop;				// 파괴전 TRUE 셋팅
		BOOL							m_bDestroy;				// 파괴 진행.
		MODE							m_emMode;
		MODE_WAIT						m_emMode_Wait;
		DWORD							m_dwDelayCount;
		DWORD							m_dwCalAniTime;			// 계산 시간을 구하기 위함. Animation 만
		DWORD							m_dwCalculateTime;		// 계산 시간을 구하기 위함.

		// 캐릭터 애니메이션
		SKIN_ANIMATION_THREAD	m_sSkinAnimationWait;	// 작업되기전에는 렌더하지 않는다.
		//SKIN_ANIMATION_THREAD	m_sSkinAnimationPass;	// 작업완료와 상관없이 렌더한다.
		

		// 카메라와 캐릭터의 Ray 충돌처리.
		std::vector<ActorNameCollisionThread>	m_vecNameCollisionGet;
		std::vector<ActorNameCollisionThread>	m_vecNameCollision;

		// 캐릭터와 캐릭터의 Ray 충돌처리.
		std::vector<ActorNameCollisionThread>	m_vecCollisionByObjectGet;
		std::vector<ActorNameCollisionThread>	m_vecCollisionByObject;

		// 캐릭터에 영향주는.. 지형에 셋팅된 Diffuse 와 PointLight 값 계산
		std::vector<ActorWorldLightThread>		m_vecActorWorldLightGet;
		std::vector<ActorWorldLightThread>		m_vecActorWorldLight;

		DATA()
			: m_bStop(FALSE)
			, m_bDestroy(FALSE)
			, m_emMode(COMPLETE_CHAR_DATA)
			, m_emMode_Wait(COMPLETE_CHAR_ANI_DATA)
			, m_dwCalAniTime(0)
			, m_dwCalculateTime(0)
			, m_dwDelayCount(0)
		{
		}

		void DataReset()
		{
			ClearGetData();
			ClearSetData();
		}

		void ClearGetData()
		{
			m_sSkinAnimationWait.ClearGetData();
			//m_sSkinAnimationPass.ClearGetData();

			m_vecNameCollisionGet.clear();
			m_vecCollisionByObjectGet.clear();
			m_vecActorWorldLightGet.clear();
		}

		void ClearSetData()
		{
			m_sSkinAnimationWait.ClearSetData();
			//m_sSkinAnimationPass.ClearSetData();

			m_vecNameCollision.clear();
			m_vecCollisionByObject.clear();
			m_vecActorWorldLight.clear();
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
			if ( g_sData.m_emMode == CALCULATE_DATA )
				continue;

			// 계산 중에는 지우지 않는다.
			if ( g_sData.m_emMode_Wait == CALCULATE_CHAR_ANI_DATA )
				continue;

			// 계산이 완료되었으니 지운다.
			g_sData.DataReset();
			return;
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread

	// TAG_ProcessAnimationThread_2
	// TAG_ProcessAnimationThread_Roop 지속적으로 돌고 있다.
	unsigned int WINAPI LoadThread( LPVOID pData )
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

			// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
			DWORD dwTime = timeGetTime();
#endif


			// 완료된 상태라면 잠시 대기 해준다.
			// Wait_CompleteSkinAniControl() 작업이 완료되면 CALCULATE_DATA 로 변경되서 다시 작업될 것이다.
			if ( _pData->m_emMode_Wait == CALCULATE_CHAR_ANI_DATA )
			{
				_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );

				_pData->m_emMode_Wait = COMPLETE_CHAR_ANI_DATA;
			}


			// 완료된 상태라면 잠시 대기 해준다.
			// Wait_CompleteSkinAniControl() 작업이 완료되면 CALCULATE_DATA 로 변경되서 다시 작업될 것이다.
			if ( _pData->m_emMode == CALCULATE_DATA )
			{
				// Pass 관련
				//_pData->m_sSkinAnimationPass.Thread_FrameMove( &_pData->m_emMode_Wait, &_pData->m_sSkinAnimationWait );


				// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
				_pData->m_dwCalAniTime = timeGetTime()-dwTime;
#endif


				// 충돌처리를 한다.
				for ( DWORD i=0; i<_pData->m_vecNameCollision.size(); ++i )
				{
					_pData->m_vecNameCollision[i].Collision();

					// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
					_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );
				}

				for ( DWORD i=0; i<_pData->m_vecCollisionByObject.size(); ++i )
				{
					_pData->m_vecCollisionByObject[i].Collision();

					// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
					_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );
				}

				for ( DWORD i=0; i<_pData->m_vecActorWorldLight.size(); ++i )
				{
					_pData->m_vecActorWorldLight[i].CalculateActorWorldLight();

					// Wait 를 돌려야 되는 상황이 되면 작업이 된다.
					_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );
				}


				// Release 때는 속도가 중요하니 이런걸 매프레임 하진 말자.
#ifdef _RELEASED
				_pData->m_dwCalculateTime = timeGetTime()-dwTime;
#endif


				_pData->m_emMode = COMPLETE_CHAR_DATA;
			}
		}

		return 0;
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread 생성과 삭제

	BOOL StartThread()
	{
		// [shhan][2014.08.27] 일부 컴퓨터의 timeGetTime 해상도가 낮아서 m_dwCalculateTime 값을 얻는데 문제가 있어서 수정함.
		//						Sleep 에도 영향을 줄 수 있기 때문에 최소 Period 값으로 셋팅한다.
		//System에서 가능한 Resolution을 구한다. 
		//멀티미디어 타이머의 Resolution을 최소로 하기 위한 코드     
		TIMECAPS tc;   
		timeGetDevCaps(&tc, sizeof(TIMECAPS));
		g_unPeriodMin = tc.wPeriodMin;
		if ( g_unPeriodMin < 1 )
			g_unPeriodMin = 1;
		timeBeginPeriod( g_unPeriodMin );

		g_hThread = (HANDLE) ::_beginthreadex( NULL,
											0,
											LoadThread,
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

		// 일부 컴퓨터의 timeGetTime 해상도가 낮아서 m_dwCalculateTime 값을 얻는데 문제가 있어서 수정함.
		timeEndPeriod( g_unPeriodMin );
	}



	//////////////////////////////////////////////////////////////////////////
	// 동작을 위해 매 프레임 해줘야 하는 것들.

	// 이 함수는 FrameMove 가 종료되는 시점에서 불려진다.
	// 이 때까지 COMPLETE_CHAR_DATA 가 안되어 있다는건 Frame Over 가 된다는 것이다.
	// 모든 정보가 COMPLETE_CHAR_DATA 될 때 까지 기다리도록 한다.
	//
	// 이 함수로 들어오면 이 두 변수는 무조건 이렇게 값이 셋팅된다.
	// g_sData.m_emMode = CALCULATE_DATA;
	// g_sData.m_emMode_Wait = CALCULATE_DATA_MODE_WAIT;
	//
	void Wait_CompleteSkinAniControl( float fElapsedTime, BOOL bNotRendering )
	{
		// COMPLETE_CHAR_DATA_MODE_WAIT 가 될 때 까지 기다린다.
		while ( 1 )
		{
			if ( g_sData.m_emMode_Wait == COMPLETE_CHAR_ANI_DATA )
			{
				g_sData.m_sSkinAnimationWait.SwapBone( fElapsedTime );

				g_sData.m_sSkinAnimationWait.CopyGetToSet();

				g_sData.m_sSkinAnimationWait.ClearGetData();

				// 최소화를 하여 Rendering 이 필요 없을 경우
				if ( bNotRendering )
				{
					g_sData.m_vecNameCollisionGet.clear();
					g_sData.m_vecCollisionByObjectGet.clear();
					g_sData.m_vecActorWorldLightGet.clear();

					CDebugSet::ToView( 12, _T("Wait_CompleteSkinAniControl - Thread : 0ff") );

					// return 을 함으로써 CALCULATE_DATA 와 CALCULATE_CHAR_ANI_DATA 이 안되도록 한다.
					return;
				}

				break;
			}

			// 혹시나 CPU를 심하게 잡을 수 있으니 넣자.
			Sleep( 1 );
		}




		//////////////////////////////////////////////////////////////////////////
		// 작업 완료가 안되어서 이번에 받은것은 모두 취소하고,
		// 다음에 받는 것을 작업예정으로 두도록 한다. CPU 부하를 줄이기 위함.
		// 지금 작업중인건 계속 돌아간다.
		if ( g_sData.m_emMode != COMPLETE_CHAR_DATA )
		{
			//////////////////////////////////////////////////////////////////////////
			// 받은것을 지워준다.
			//g_sData.m_sSkinAnimationPass.ClearGetData();

			g_sData.m_vecNameCollisionGet.clear();
			g_sData.m_vecCollisionByObjectGet.clear();
			g_sData.m_vecActorWorldLightGet.clear();

#ifdef _RELEASED
			++g_sData.m_dwDelayCount;
			CDebugSet::ToView( 12, _T("Wait_CompleteSkinAniControl - Thread : %d, %d, Delay"), g_sData.m_dwCalAniTime, g_sData.m_dwCalculateTime );
#endif


			//////////////////////////////////////////////////////////////////////////
			// Frame 이 바뀌기 때문에 Wait 관련 작업들을 재 계산해주기 위하여 셋팅을 변경한다.
			// 이곳까지 온건 g_sData.m_emMode_Wait == COMPLETE_CHAR_DATA_MODE_WAIT 값이기 때문에 변경을 해줘서 계산하게 한다.
			g_sData.m_emMode_Wait = CALCULATE_CHAR_ANI_DATA;

			return;
		}



		//////////////////////////////////////////////////////////////////////////
		// 딜레이 없는 정상적인 동작
#ifdef _RELEASED
		if ( g_sData.m_dwDelayCount >= 1 ) 
		{
			CDebugSet::ToView( 12, _T("Wait_CompleteSkinAniControl - Thread : %d, %d, Delay"), g_sData.m_dwCalAniTime, g_sData.m_dwCalculateTime );
		}
		else
		{
			CDebugSet::ToView( 12, _T("Wait_CompleteSkinAniControl - Thread : %d, %d"), g_sData.m_dwCalAniTime, g_sData.m_dwCalculateTime );
		}
		g_sData.m_dwDelayCount = 0;
#endif





		//////////////////////////////////////////////////////////////////////////
		// Complete 된 Data 들의 Swap 작업을 한다.
		//g_sData.m_sSkinAnimationPass.SwapBone( fElapsedTime );

		for ( DWORD i=0; i<g_sData.m_vecNameCollision.size(); ++i )
		{
			g_sData.m_vecNameCollision[i].SwapNameCollision();
		}
		for ( DWORD i=0; i<g_sData.m_vecCollisionByObject.size(); ++i )
		{
			g_sData.m_vecCollisionByObject[i].SwapNameCollision();
		}
		for ( DWORD i=0; i<g_sData.m_vecActorWorldLight.size(); ++i )
		{
			g_sData.m_vecActorWorldLight[i].SwapActorWorldLight();
		}



		//////////////////////////////////////////////////////////////////////////
		// Wait 에 나둔걸 작업쪽으로 옮겨준다.
		//g_sData.m_sSkinAnimationPass.CopyGetToSet();

		g_sData.m_vecNameCollision		= g_sData.m_vecNameCollisionGet;
		g_sData.m_vecCollisionByObject	= g_sData.m_vecCollisionByObjectGet;
		g_sData.m_vecActorWorldLight	= g_sData.m_vecActorWorldLightGet;



		//////////////////////////////////////////////////////////////////////////
		// Get 은 clear 해준다.
		//g_sData.m_sSkinAnimationPass.ClearGetData();

		g_sData.m_vecNameCollisionGet.clear();
		g_sData.m_vecCollisionByObjectGet.clear();
		g_sData.m_vecActorWorldLightGet.clear();



		//////////////////////////////////////////////////////////////////////////
		// 계산이 진행되라고 해준다.
		// 새로운 시작이 된다.
		g_sData.m_emMode = CALCULATE_DATA;
		g_sData.m_emMode_Wait = CALCULATE_CHAR_ANI_DATA;
	}

	// CharEditer 에서만 쓰인다.
	// 이걸 사용하려면 LoadThread 동작되고 있으면 안된다.
	// EndThread() 를 꼭 해놔야한다.
	void CalcluateSkinAniControlForCharEditer( float fElapsedTime )
	{
		// 여기서 호출되면 Thread 상에서 하지는 않는다
		g_sData.m_sSkinAnimationWait.Thread_FrameMove_ForCharEditer();
		//g_sData.m_sSkinAnimationPass.Thread_FrameMove_ForCharEditer();

		// CalculateBone_PureThread 작업 완료 후에 해야한다. SwapBone 전에 있어야 한다.
		// DxSkinAniControl::m_pSkeletonBackUp 값을 사용한다.
		for ( DWORD i=0; i<g_sData.m_sSkinAnimationWait.m_vecCharPartBoneMatricesGet.size(); ++i )
		{
			g_sData.m_sSkinAnimationWait.m_vecCharPartBoneMatricesGet[i].m_spCharPartBoneMatrices->FrameMove_PureThread();
		}
		//for ( DWORD i=0; i<g_sData.m_sSkinAnimationPass.m_vecCharPartBoneMatricesGet.size(); ++i )
		//{
		//	g_sData.m_sSkinAnimationPass.m_vecCharPartBoneMatricesGet[i].m_spCharPartBoneMatrices->FrameMove_PureThread();
		//}

		// Swap
		g_sData.m_sSkinAnimationWait.SwapBoneForCharEditer( fElapsedTime );
		//g_sData.m_sSkinAnimationPass.SwapBoneForCharEditer( fElapsedTime );


		//////////////////////////////////////////////////////////////////////////
		// Get 은 clear 해준다.
		g_sData.m_sSkinAnimationWait.ClearGetData();
		//g_sData.m_sSkinAnimationPass.ClearGetData();
	}



	//////////////////////////////////////////////////////////////////////////
	// SkinAniControl 등록

	// 등록만 한다.
	void AddAniControl( std::tr1::shared_ptr<DxSkinAniControl>& spSkinAniControl, std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData, const D3DXMATRIX& matLocalRot, BOOL bWait )
	{
		SKIN_DATA sData( spSkinAniControl, spThreadNeedData, matLocalRot );
		//if ( bWait )
		{
			g_sData.m_sSkinAnimationWait.m_vecSkinDataGet.push_back( sData );
		}
		//else
		//{
		//	g_sData.m_sSkinAnimationPass.m_vecSkinDataGet.push_back( sData );
		//}
	}

	void AddAniControl_Vehicle( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
								std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
								std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleNeedData,
								const D3DXMATRIX& matLocalRot, 
								BOOL bWait )
	{
		SKIN_DATA_VEHICLE sData( spSkinAniControl, spThreadNeedData, spVehicleNeedData, matLocalRot );
		//if ( bWait )
		{
			g_sData.m_sSkinAnimationWait.m_vecSkinDataGet_Vehicle.push_back( sData );
		}
		//else
		//{
		//	g_sData.m_sSkinAnimationPass.m_vecSkinDataGet_Vehicle.push_back( sData );
		//}
	}

	// 등록만 한다.
	void AddAniControl_AttBone( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
								std::tr1::shared_ptr<DxSkinAniControl>&	spLinkSkinAniControl, 
								std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData,
								float fKeyJiggleWeight, 
								float fScale,
								BOOL bWait )
	{
		SKIN_DATA_ATTBONE sData( spSkinAniControl, spLinkSkinAniControl, spThreadNeedData, fKeyJiggleWeight, fScale );
		//if ( bWait )
		{
			g_sData.m_sSkinAnimationWait.m_vecSkinDataGet_AttBone.push_back( sData );
		}
		//else
		//{
		//	g_sData.m_sSkinAnimationPass.m_vecSkinDataGet_AttBone.push_back( sData );
		//}
	}

	void AddAniControl_CharOnVehicle( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
										std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
										std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleNeedData,
										DWORD dwPassengerID, 
										BOOL bWait )
	{
		SKIN_DATA_CHAR_ON_VEHICLE sData( spSkinAniControl, spThreadNeedData, spVehicleNeedData, dwPassengerID );
		//if ( bWait )
		{
			g_sData.m_sSkinAnimationWait.m_vecSkinDataGet_CharOnVehicle.push_back( sData );
		}
		//else
		//{
		//	g_sData.m_sSkinAnimationPass.m_vecSkinDataGet_CharOnVehicle.push_back( sData );
		//}
	}




	//////////////////////////////////////////////////////////////////////////
	// CharPart 쪽 BoneMatrices 등록
	// sSkeletonBackUp 백업을 보내줘야한다.
	// CalculateBone_PureThread 계산한 값을 넣어주는 곳은 백업을 하기 때문에 그렇다.
	void AddCharPartBoneMatrices( std::tr1::shared_ptr<CharPartBoneMatrices>& spCharPartBoneMatrices, BOOL bWait )
	{
		CHATPARTBONEMATRIX sData( spCharPartBoneMatrices );
		//if ( bWait )
		{
			g_sData.m_sSkinAnimationWait.m_vecCharPartBoneMatricesGet.push_back( sData );
		}
		//else
		//{
		//	g_sData.m_sSkinAnimationPass.m_vecCharPartBoneMatricesGet.push_back( sData );
		//}
	}





	//////////////////////////////////////////////////////////////////////////
	// Name Collision

	// Thread 상으로 넘긴다.
	void AddNameCollision( std::tr1::shared_ptr<ActorNameCollision>& spNameColl, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd )
	{
		ActorNameCollisionThread sData( spNameColl, vStart, vEnd );
		g_sData.m_vecNameCollisionGet.push_back( sData );
	}

	void AddCollisionByObject( std::tr1::shared_ptr<ActorNameCollision>& spNameColl, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd )
	{
		ActorNameCollisionThread sData( spNameColl, vStart, vEnd );
		g_sData.m_vecCollisionByObjectGet.push_back( sData );
	}




	//////////////////////////////////////////////////////////////////////////
	// ActorWorldLight
	void AddActorWorldLight( std::tr1::shared_ptr<ActorWorldLight>& spActorWorldLight, const D3DXVECTOR3& vPos )
	{
		ActorWorldLightThread sData( spActorWorldLight, vPos );
		g_sData.m_vecActorWorldLightGet.push_back( sData );
	}
};