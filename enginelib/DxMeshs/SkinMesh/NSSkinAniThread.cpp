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
	DWORD	g_dwThreadID;		//	������ ID
	HANDLE	g_hThread(NULL);	//	������ �ڵ�
	UINT	g_unPeriodMin;		//  System ���� ������ �ּ� Period

	// �⺻ ĳ����
	struct SKIN_DATA
	{
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;	// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;	// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
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
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;			// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;			// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxVehicleThreadNeedData>			m_spVehicleThreadNeedData;	// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
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
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;		// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxSkinAniControl>					m_spLinkSkinAniControl;	// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>		m_spThreadNeedData;		// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		float													m_fKeyJiggleWeight;
		float													m_fScale;
		D3DXMATRIX												m_matLocalRot;			// DxSkinAniControl::CalculateBoneAttBone_PureThread ���� ���ȴ�.

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

	// Vehicle �� Ÿ�� �ִ� ĳ����
	struct SKIN_DATA_CHAR_ON_VEHICLE
	{
		std::tr1::shared_ptr<DxSkinAniControl>					m_spSkinAniControl;			// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>	m_spThreadNeedData;			// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		std::tr1::shared_ptr<DxVehicleThreadNeedData>			m_spVehicleThreadNeedData;	// �����θ� �߾��µ� �ٸ������� ������ ���� �־ shared_ptr �� �ٲ�.
		DWORD													m_dwPassengerID;
		D3DXMATRIX												m_matLocalRot;				// DxSkinAniControl::CalculateBoneCharOnVehicle_PureThread ���� ���ȴ�.

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

		// operator= ���� �۾��������.
		CHATPARTBONEMATRIX( std::tr1::shared_ptr<CharPartBoneMatrices>&	spCharPartBoneMatrices )
			: m_spCharPartBoneMatrices(spCharPartBoneMatrices)
		{
		}
	};
	
	

	enum MODE
	{
		CALCULATE_DATA					= 0x0,	// ����� ����Ǵ� �ñ�
		COMPLETE_CHAR_DATA				= 0x1,	// ����
	};

	enum MODE_WAIT
	{
		CALCULATE_CHAR_ANI_DATA			= 0x0,	// ����� ����Ǵ� �ñ�
		COMPLETE_CHAR_ANI_DATA			= 0x1,	// ����
	};

	struct SKIN_ANIMATION_THREAD
	{
		// ������ ������.
		std::vector<SKIN_DATA>					m_vecSkinDataGet;
		std::vector<SKIN_DATA_VEHICLE>			m_vecSkinDataGet_Vehicle;
		std::vector<SKIN_DATA_ATTBONE>			m_vecSkinDataGet_AttBone;
		std::vector<SKIN_DATA_CHAR_ON_VEHICLE>	m_vecSkinDataGet_CharOnVehicle;

		// Render �۾�.
		std::vector<SKIN_DATA>					m_vecSkinData;
		std::vector<SKIN_DATA_VEHICLE>			m_vecSkinData_Vehicle;
		std::vector<SKIN_DATA_ATTBONE>			m_vecSkinData_AttBone;
		std::vector<SKIN_DATA_CHAR_ON_VEHICLE>	m_vecSkinData_CharOnVehicle;

		// CharPart �� BoneMatrices
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
			// Bone �� ����Ѵ�.
			// ������ ������ �߿��ϴ�.

			// ĳ���Ϳ� Vehicle �� ����Ѵ�.
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

			// ĳ���Ϳ� Vehicle �� ����Ѵ�.
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

			// Vehicle �� Ÿ�� �ִ� ĳ���͸� ����Ѵ�. ( Vehicle Bone ������ �ʿ���. )
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

			// AttachBone�� ����Ѵ�. ( Vehicle Bone Ȥ�� ĳ���� �� ������ �ʿ���. )
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
			// Bone �� ����Ѵ�.
			// ������ ������ �߿��ϴ�.

			// ĳ���Ϳ� Vehicle �� ����Ѵ�.
			for ( DWORD i=0; i<m_vecSkinData.size(); ++i )
			{
				if ( m_vecSkinData[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// ������ ������ ���´�.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData[i].m_spSkinAniControl->CalculateBone_PureThread
				( 
					m_vecSkinData[i].m_matLocalRot, 
					m_vecSkinData[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinData[i].m_spThreadNeedData->m_dwCharPartNUM,
					m_vecSkinData[i].m_spThreadNeedData->m_wwMoveParentBoneIDForSafeModeWeaponSlot
				);

				// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}

			// ĳ���Ϳ� Vehicle �� ����Ѵ�.
			for ( DWORD i=0; i<m_vecSkinData_Vehicle.size(); ++i )
			{
				if ( m_vecSkinData_Vehicle[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// ������ ������ ���´�.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData_Vehicle[i].m_spSkinAniControl->CalculateBoneVehicle_PureThread
				( 
					m_vecSkinData_Vehicle[i].m_spVehicleThreadNeedData,
					m_vecSkinData_Vehicle[i].m_matLocalRot, 
					m_vecSkinData_Vehicle[i].m_spThreadNeedData->m_arrayKeyJiggleWeight, 
					m_vecSkinData_Vehicle[i].m_spThreadNeedData->m_dwCharPartNUM
				);

				// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}

			// Vehicle �� Ÿ�� �ִ� ĳ���͸� ����Ѵ�. ( Vehicle Bone ������ �ʿ���. )
			for ( DWORD i=0; i<m_vecSkinData_CharOnVehicle.size(); ++i )
			{
				if ( m_vecSkinData_CharOnVehicle[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// ������ ������ ���´�.

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

				// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}

			// AttachBone�� ����Ѵ�. ( Vehicle Bone Ȥ�� ĳ���� �� ������ �ʿ���. )
			for ( DWORD i=0; i<m_vecSkinData_AttBone.size(); ++i )
			{
				if ( m_vecSkinData_AttBone[i].m_spSkinAniControl.use_count() <= 1 )
					continue;	// ������ ������ ���´�.

				// TAG_CalculateAnimationThread_1
				m_vecSkinData_AttBone[i].m_spSkinAniControl->CalculateBoneAttBone_PureThread
				( 
					m_vecSkinData_AttBone[i].m_matLocalRot,
					m_vecSkinData_AttBone[i].m_spLinkSkinAniControl,
					m_vecSkinData_AttBone[i].m_spThreadNeedData,
					m_vecSkinData_AttBone[i].m_fKeyJiggleWeight,
					m_vecSkinData_AttBone[i].m_fScale
				);

				// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}
		}

		void Thread_FrameMove( MODE_WAIT* pemMode, SKIN_ANIMATION_THREAD* pSkinAnimationWait )
		{
			Thread_FrameMove_Bone( pemMode, pSkinAnimationWait );

			// CalculateBone_PureThread �۾� �Ϸ� �Ŀ� �ؾ��Ѵ�. SwapBone ���� �־�� �Ѵ�.
			// DxSkinAniControl::m_pSkeletonBackUp ���� ����Ѵ�.
			for ( DWORD i=0; i<m_vecCharPartBoneMatrices.size(); ++i )
			{
				if ( m_vecCharPartBoneMatrices[i].m_spCharPartBoneMatrices.use_count() <= 1 )
					continue;	// ������ ������ ���´�.

				m_vecCharPartBoneMatrices[i].m_spCharPartBoneMatrices->FrameMove_PureThread();

				// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
				if ( pSkinAnimationWait )
				{
					pSkinAnimationWait->Thread_FrameMoveForWait( pemMode );
				}
			}
		}

		void SwapBoneForCharEditer( float fElapsedTime )
		{
			//////////////////////////////////////////////////////////////////////////
			// Complete �� Data ���� Swap �۾��� �Ѵ�.
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
			// Complete �� Data ���� Swap �۾��� �Ѵ�.
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

		// [shhan][2015.03.25] m_sSkinAnimationWait �� CopyGetToSet ���� ���� SwapBone �� ����� �Ѵ�.
		//						���� ������ �ʴ´ٸ� �������� �޸� ������ ���� ���� �ִ�.
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
		BOOL							m_bStop;				// �ı��� TRUE ����
		BOOL							m_bDestroy;				// �ı� ����.
		MODE							m_emMode;
		MODE_WAIT						m_emMode_Wait;
		DWORD							m_dwDelayCount;
		DWORD							m_dwCalAniTime;			// ��� �ð��� ���ϱ� ����. Animation ��
		DWORD							m_dwCalculateTime;		// ��� �ð��� ���ϱ� ����.

		// ĳ���� �ִϸ��̼�
		SKIN_ANIMATION_THREAD	m_sSkinAnimationWait;	// �۾��Ǳ������� �������� �ʴ´�.
		//SKIN_ANIMATION_THREAD	m_sSkinAnimationPass;	// �۾��Ϸ�� ������� �����Ѵ�.
		

		// ī�޶�� ĳ������ Ray �浹ó��.
		std::vector<ActorNameCollisionThread>	m_vecNameCollisionGet;
		std::vector<ActorNameCollisionThread>	m_vecNameCollision;

		// ĳ���Ϳ� ĳ������ Ray �浹ó��.
		std::vector<ActorNameCollisionThread>	m_vecCollisionByObjectGet;
		std::vector<ActorNameCollisionThread>	m_vecCollisionByObject;

		// ĳ���Ϳ� �����ִ�.. ������ ���õ� Diffuse �� PointLight �� ���
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
	// ���̵��� ���� ���� ������ ��������� ���� ����

	void DataCleanUp()
	{
		while (1)
		{
			Sleep(1);

			// ��� �߿��� ������ �ʴ´�.
			if ( g_sData.m_emMode == CALCULATE_DATA )
				continue;

			// ��� �߿��� ������ �ʴ´�.
			if ( g_sData.m_emMode_Wait == CALCULATE_CHAR_ANI_DATA )
				continue;

			// ����� �Ϸ�Ǿ����� �����.
			g_sData.DataReset();
			return;
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread

	// TAG_ProcessAnimationThread_2
	// TAG_ProcessAnimationThread_Roop ���������� ���� �ִ�.
	unsigned int WINAPI LoadThread( LPVOID pData )
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

			// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
#ifdef _RELEASED
			DWORD dwTime = timeGetTime();
#endif


			// �Ϸ�� ���¶�� ��� ��� ���ش�.
			// Wait_CompleteSkinAniControl() �۾��� �Ϸ�Ǹ� CALCULATE_DATA �� ����Ǽ� �ٽ� �۾��� ���̴�.
			if ( _pData->m_emMode_Wait == CALCULATE_CHAR_ANI_DATA )
			{
				_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );

				_pData->m_emMode_Wait = COMPLETE_CHAR_ANI_DATA;
			}


			// �Ϸ�� ���¶�� ��� ��� ���ش�.
			// Wait_CompleteSkinAniControl() �۾��� �Ϸ�Ǹ� CALCULATE_DATA �� ����Ǽ� �ٽ� �۾��� ���̴�.
			if ( _pData->m_emMode == CALCULATE_DATA )
			{
				// Pass ����
				//_pData->m_sSkinAnimationPass.Thread_FrameMove( &_pData->m_emMode_Wait, &_pData->m_sSkinAnimationWait );


				// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
#ifdef _RELEASED
				_pData->m_dwCalAniTime = timeGetTime()-dwTime;
#endif


				// �浹ó���� �Ѵ�.
				for ( DWORD i=0; i<_pData->m_vecNameCollision.size(); ++i )
				{
					_pData->m_vecNameCollision[i].Collision();

					// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
					_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );
				}

				for ( DWORD i=0; i<_pData->m_vecCollisionByObject.size(); ++i )
				{
					_pData->m_vecCollisionByObject[i].Collision();

					// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
					_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );
				}

				for ( DWORD i=0; i<_pData->m_vecActorWorldLight.size(); ++i )
				{
					_pData->m_vecActorWorldLight[i].CalculateActorWorldLight();

					// Wait �� ������ �Ǵ� ��Ȳ�� �Ǹ� �۾��� �ȴ�.
					_pData->m_sSkinAnimationWait.Thread_FrameMoveForWait( &_pData->m_emMode_Wait );
				}


				// Release ���� �ӵ��� �߿��ϴ� �̷��� �������� ���� ����.
#ifdef _RELEASED
				_pData->m_dwCalculateTime = timeGetTime()-dwTime;
#endif


				_pData->m_emMode = COMPLETE_CHAR_DATA;
			}
		}

		return 0;
	}



	//////////////////////////////////////////////////////////////////////////
	// Thread ������ ����

	BOOL StartThread()
	{
		// [shhan][2014.08.27] �Ϻ� ��ǻ���� timeGetTime �ػ󵵰� ���Ƽ� m_dwCalculateTime ���� ��µ� ������ �־ ������.
		//						Sleep ���� ������ �� �� �ֱ� ������ �ּ� Period ������ �����Ѵ�.
		//System���� ������ Resolution�� ���Ѵ�. 
		//��Ƽ�̵�� Ÿ�̸��� Resolution�� �ּҷ� �ϱ� ���� �ڵ�     
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

		// �Ϻ� ��ǻ���� timeGetTime �ػ󵵰� ���Ƽ� m_dwCalculateTime ���� ��µ� ������ �־ ������.
		timeEndPeriod( g_unPeriodMin );
	}



	//////////////////////////////////////////////////////////////////////////
	// ������ ���� �� ������ ����� �ϴ� �͵�.

	// �� �Լ��� FrameMove �� ����Ǵ� �������� �ҷ�����.
	// �� ������ COMPLETE_CHAR_DATA �� �ȵǾ� �ִٴ°� Frame Over �� �ȴٴ� ���̴�.
	// ��� ������ COMPLETE_CHAR_DATA �� �� ���� ��ٸ����� �Ѵ�.
	//
	// �� �Լ��� ������ �� �� ������ ������ �̷��� ���� ���õȴ�.
	// g_sData.m_emMode = CALCULATE_DATA;
	// g_sData.m_emMode_Wait = CALCULATE_DATA_MODE_WAIT;
	//
	void Wait_CompleteSkinAniControl( float fElapsedTime, BOOL bNotRendering )
	{
		// COMPLETE_CHAR_DATA_MODE_WAIT �� �� �� ���� ��ٸ���.
		while ( 1 )
		{
			if ( g_sData.m_emMode_Wait == COMPLETE_CHAR_ANI_DATA )
			{
				g_sData.m_sSkinAnimationWait.SwapBone( fElapsedTime );

				g_sData.m_sSkinAnimationWait.CopyGetToSet();

				g_sData.m_sSkinAnimationWait.ClearGetData();

				// �ּ�ȭ�� �Ͽ� Rendering �� �ʿ� ���� ���
				if ( bNotRendering )
				{
					g_sData.m_vecNameCollisionGet.clear();
					g_sData.m_vecCollisionByObjectGet.clear();
					g_sData.m_vecActorWorldLightGet.clear();

					CDebugSet::ToView( 12, _T("Wait_CompleteSkinAniControl - Thread : 0ff") );

					// return �� �����ν� CALCULATE_DATA �� CALCULATE_CHAR_ANI_DATA �� �ȵǵ��� �Ѵ�.
					return;
				}

				break;
			}

			// Ȥ�ó� CPU�� ���ϰ� ���� �� ������ ����.
			Sleep( 1 );
		}




		//////////////////////////////////////////////////////////////////////////
		// �۾� �Ϸᰡ �ȵǾ �̹��� �������� ��� ����ϰ�,
		// ������ �޴� ���� �۾��������� �ε��� �Ѵ�. CPU ���ϸ� ���̱� ����.
		// ���� �۾����ΰ� ��� ���ư���.
		if ( g_sData.m_emMode != COMPLETE_CHAR_DATA )
		{
			//////////////////////////////////////////////////////////////////////////
			// �������� �����ش�.
			//g_sData.m_sSkinAnimationPass.ClearGetData();

			g_sData.m_vecNameCollisionGet.clear();
			g_sData.m_vecCollisionByObjectGet.clear();
			g_sData.m_vecActorWorldLightGet.clear();

#ifdef _RELEASED
			++g_sData.m_dwDelayCount;
			CDebugSet::ToView( 12, _T("Wait_CompleteSkinAniControl - Thread : %d, %d, Delay"), g_sData.m_dwCalAniTime, g_sData.m_dwCalculateTime );
#endif


			//////////////////////////////////////////////////////////////////////////
			// Frame �� �ٲ�� ������ Wait ���� �۾����� �� ������ֱ� ���Ͽ� ������ �����Ѵ�.
			// �̰����� �°� g_sData.m_emMode_Wait == COMPLETE_CHAR_DATA_MODE_WAIT ���̱� ������ ������ ���༭ ����ϰ� �Ѵ�.
			g_sData.m_emMode_Wait = CALCULATE_CHAR_ANI_DATA;

			return;
		}



		//////////////////////////////////////////////////////////////////////////
		// ������ ���� �������� ����
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
		// Complete �� Data ���� Swap �۾��� �Ѵ�.
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
		// Wait �� ���а� �۾������� �Ű��ش�.
		//g_sData.m_sSkinAnimationPass.CopyGetToSet();

		g_sData.m_vecNameCollision		= g_sData.m_vecNameCollisionGet;
		g_sData.m_vecCollisionByObject	= g_sData.m_vecCollisionByObjectGet;
		g_sData.m_vecActorWorldLight	= g_sData.m_vecActorWorldLightGet;



		//////////////////////////////////////////////////////////////////////////
		// Get �� clear ���ش�.
		//g_sData.m_sSkinAnimationPass.ClearGetData();

		g_sData.m_vecNameCollisionGet.clear();
		g_sData.m_vecCollisionByObjectGet.clear();
		g_sData.m_vecActorWorldLightGet.clear();



		//////////////////////////////////////////////////////////////////////////
		// ����� ����Ƕ�� ���ش�.
		// ���ο� ������ �ȴ�.
		g_sData.m_emMode = CALCULATE_DATA;
		g_sData.m_emMode_Wait = CALCULATE_CHAR_ANI_DATA;
	}

	// CharEditer ������ ���δ�.
	// �̰� ����Ϸ��� LoadThread ���۵ǰ� ������ �ȵȴ�.
	// EndThread() �� �� �س����Ѵ�.
	void CalcluateSkinAniControlForCharEditer( float fElapsedTime )
	{
		// ���⼭ ȣ��Ǹ� Thread �󿡼� ������ �ʴ´�
		g_sData.m_sSkinAnimationWait.Thread_FrameMove_ForCharEditer();
		//g_sData.m_sSkinAnimationPass.Thread_FrameMove_ForCharEditer();

		// CalculateBone_PureThread �۾� �Ϸ� �Ŀ� �ؾ��Ѵ�. SwapBone ���� �־�� �Ѵ�.
		// DxSkinAniControl::m_pSkeletonBackUp ���� ����Ѵ�.
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
		// Get �� clear ���ش�.
		g_sData.m_sSkinAnimationWait.ClearGetData();
		//g_sData.m_sSkinAnimationPass.ClearGetData();
	}



	//////////////////////////////////////////////////////////////////////////
	// SkinAniControl ���

	// ��ϸ� �Ѵ�.
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

	// ��ϸ� �Ѵ�.
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
	// CharPart �� BoneMatrices ���
	// sSkeletonBackUp ����� ��������Ѵ�.
	// CalculateBone_PureThread ����� ���� �־��ִ� ���� ����� �ϱ� ������ �׷���.
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

	// Thread ������ �ѱ��.
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