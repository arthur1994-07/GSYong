#include "pch.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#include "GLCharacter.h"

HRESULT GLCharacter::UpdateAnimation ( float fTime, float fElapsedTime, const EMACTIONTYPE Action )
{
	HRESULT hr=S_OK;

	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform )) == NULL )
	{
		pRenderChar = m_pSkinChar;
	}

	if( !pRenderChar )
		return E_FAIL;

	BOOL bLowSP = ( float(m_sSP.nNow) / float(m_sSP.nMax) ) <= GLCONST_CHAR::fLOWSP_MOTION;
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();

	BOOL bPeaceZone = pLand ? pLand->IsPeaceZone() : FALSE;
	if ( !bPeaceZone && IsActState(EM_ACT_PEACEMODE) )		bPeaceZone = TRUE;

	BOOL bFreeze = FALSE;

	EMANI_MAINTYPE emMType;
	EMANI_SUBTYPE emSType;

	if ( m_emVehicle && m_sVehicle.m_emTYPE == VEHICLE_TYPE_BOARD )
	{
		emSType = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + m_sVehicle.m_emTYPE);
		//emSType = AN_SUB_HOVERBOARD;
	}
	else
	{
		emSType = AN_SUB_NONE;
	}

	switch ( Action )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime += fElapsedTime;

			BOOL bUseBoost = FALSE;
			BOOL* pOnceBoost = NULL;
			SITEM* pItem = NULL;
			if ( bPeaceZone && pRenderChar->GETANI ( AN_PLACID, AN_SUB_NONE ) )
			{
				emMType = AN_PLACID;
			}
			else
			{
				BOOL bPANT(FALSE); // 헐떡거림.

				if ( pRenderChar->GETANI ( AN_GUARD_L, m_emANISUBTYPE ) )
				{
					bPANT = bLowSP || m_bSTATE_PANT;
					for ( int i=0; i<EMBLOW_MULTI; ++i )
					{
						if ( m_sSTATEBLOWS[i].emBLOW == EMBLOW_STUN || m_sSTATEBLOWS[i].emBLOW == EMBLOW_MAD )		bPANT = TRUE;
					}
				}

				if ( bPANT )
				{
					emMType = AN_GUARD_L;
					emSType = m_emANISUBTYPE;
				}
				else
				{
					emMType = AN_GUARD_N;
					emSType = m_emANISUBTYPE;
				}
			}

			std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar(m_sVehicle.PassengerId(0));				
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
			if( pCharClient )
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharClient->GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
				bUseBoost = pCharClient->VehicleUseBoost();
				pOnceBoost = pCharClient->VehicleOnceBoostRef();
			}
			else if (pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(0))
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharacter->GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
				bUseBoost = pCharacter->VehicleUseBoost();
				pOnceBoost = &pCharacter->m_sVehicle.m_bOnceBoost;
			}

			if ( pItem && pItem->VehicleType() != VEHICLE_TYPE_BOARD )
			{
				switch ( pItem->VehicleType() )
				{
				case VEHICLE_TYPE_BIKE:
					emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_CAR:
					emMType = EMANI_MAINTYPE( AN_CAR_A + pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_REARCAR:
					emMType = EMANI_MAINTYPE( AN_REARCAR_A + pItem->sVehicle.emPartsType );
					break;
				}
				emSType = (m_emVehicle == EMVEHICLE_DRIVER) ? AN_SUB_DRIVER_STOP : AN_SUB_PASSENGER_STOP;
				// idle일때 부스터 써져있는 이유가 있다, 부스터 사용후 바로 멈춰도 부스트 애니메이션은 보여줘야한다;
				if( bUseBoost )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						if( pOnceBoost && (*pOnceBoost) == TRUE)
						{
							emSType = AN_SUB_DRIVER_BOOST;	
						}							
					}
					else if( m_emVehicle == EMVEHICLE_PASSENGER )
					{
						emSType = AN_SUB_PASSENGER_WALK;
					}
				}
			}

			if ( m_sHALLUCINATE.bEmoticon )
			{
				if ( !pRenderChar->IsANI_SELECTANI(AN_GESTURE, m_sHALLUCINATE.emAni_SubType) )
				{
					SELECTANI ( pRenderChar, emMType, emSType );
				}
			}
			else
			{
				SELECTANI ( pRenderChar, emMType, emSType );
			}

			if( m_sVehicle.m_pSkinChar )
			{
				if( bUseBoost && pOnceBoost )
				{
					if( (*pOnceBoost) == TRUE )
					{
						m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_BOOSTER );			
						if( m_sVehicle.m_pSkinChar->IsANI_ISENDANIM( AN_VEHICLE_BOOSTER ) )
						{
							(*pOnceBoost) = FALSE;
						}
					}
					else
					{
						m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_STAY );
					}
				}
				else
				{
					m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_STAY );			
				}
			}


			m_bAttackable = TRUE;
		}
		break;

	case GLAT_MOVE:
		{
			emMType = IsActState(EM_ACT_RUN) ? AN_RUN : AN_WALK;
			if ( !m_emVehicle )
			{
				emSType = bPeaceZone ? AN_SUB_NONE : m_emANISUBTYPE;
			}

			std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar(m_sVehicle.PassengerId(0));
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
			SITEM* pItem = NULL;
			BOOL bUseBoost = FALSE;
			BOOL* pOnceBoost = NULL;

			if( pCharClient )
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharClient->GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
				bUseBoost = pCharClient->VehicleUseBoost();
				pOnceBoost = pCharClient->VehicleOnceBoostRef();
			}
			else if (pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(0))
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharacter->GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
				bUseBoost = pCharacter->VehicleUseBoost();
				pOnceBoost = pCharacter->VehicleOnceBoostRef();
			}
			if ( pItem && pItem->VehicleType() != VEHICLE_TYPE_BOARD )
			{
				switch ( pItem->VehicleType() )
				{
				case VEHICLE_TYPE_BIKE:
					emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_CAR:
					emMType = EMANI_MAINTYPE( AN_CAR_A + pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_REARCAR:
					emMType = EMANI_MAINTYPE( AN_REARCAR_A + pItem->sVehicle.emPartsType );
					break;
				}
				if ( m_emVehicle == EMVEHICLE_DRIVER )
					emSType = IsActState(EM_ACT_RUN) ? AN_SUB_DRIVER_RUN : AN_SUB_DRIVER_WALK;
				else // else if(EMVEHICLE_PASSENGER)
					emSType = IsActState(EM_ACT_RUN) ? AN_SUB_PASSENGER_RUN : AN_SUB_PASSENGER_WALK;

				if( bUseBoost )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						if( pOnceBoost && (*pOnceBoost) == TRUE)
						{
							emSType = AN_SUB_DRIVER_BOOST;			
						}							
					}
					else if( m_emVehicle == EMVEHICLE_PASSENGER )
					{
						emSType = AN_SUB_PASSENGER_WALK;
					}
				}
			}
			SELECTANI ( pRenderChar, emMType, emSType );

			if( m_sVehicle.m_pSkinChar )
			{
				if( bUseBoost && pOnceBoost )
				{
					if( (*pOnceBoost) == TRUE )
					{
						m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_BOOSTER );			
						if( m_sVehicle.m_pSkinChar->IsANI_ISENDANIM( AN_VEHICLE_BOOSTER ) )
							(*pOnceBoost) = FALSE;
					}
					else
					{
						m_sVehicle.m_pSkinChar->SELECTANI( IsActState(EM_ACT_RUN) ? AN_VEHICLE_RUN : AN_VEHICLE_WALK );
					}
				}
				else
				{
					m_sVehicle.m_pSkinChar->SELECTANI( IsActState(EM_ACT_RUN) ? AN_VEHICLE_RUN : AN_VEHICLE_WALK );
				}
			}
		}
		break;

	case GLAT_ATTACK:
		{
			EMSLOT emRHand = GetCurRHand();
			EMSLOT emLHand = GetCurLHand();

			EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );
			SELECTANI ( pRenderChar, AN_ATTACK, emANISUBTYPE, NULL, m_dwANISUBSELECT );

			if ( pRenderChar->IsANI_ISENDANIM( AN_ATTACK, emANISUBTYPE, m_dwANISUBSELECT ) || m_emVehicle )
			{
				TurnAction ( GLAT_IDLE );
			}
		}
		break;

	case GLAT_SKILL_WAIT:
		{
			SELECTSKILLANI (pRenderChar, GLCHARLOGIC::m_emANIMAINSKILL, GLCHARLOGIC::m_emANISUBSKILL );

			if( m_sVehicle.m_pSkinChar )			
				m_sVehicle.m_pSkinChar->SELECTANI( GLCHARLOGIC::m_emANIVehicleSKILL );	

			// 애니메이션이 로드 되어서 작동이 된다.
			if ( pRenderChar->GETCURMTYPE() == GLCHARLOGIC::m_emANIMAINSKILL && pRenderChar->GETCURSTYPE() == GLCHARLOGIC::m_emANISUBSKILL )
			{
				TurnAction ( GLAT_SKILL );
			}
		}
		break;

	case GLAT_SKILL:
		{
			SELECTSKILLANI (pRenderChar, GLCHARLOGIC::m_emANIMAINSKILL, GLCHARLOGIC::m_emANISUBSKILL );

			if( m_sVehicle.m_pSkinChar )			
				m_sVehicle.m_pSkinChar->SELECTANI( GLCHARLOGIC::m_emANIVehicleSKILL );	

			if( m_pWorkingSkill->IsEmpty() )
			{
				if ( ( !m_pSkinChar->IsCurAniConInfo(ACF_LOOP) && pRenderChar->IsANI_ISENDANIM( GLCHARLOGIC::m_emANIMAINSKILL, GLCHARLOGIC::m_emANISUBSKILL ) ) || m_emVehicle )
					TurnAction ( GLAT_IDLE );
			}
		}
		break;

	case GLAT_TALK:
		{
			BOOL bOK = IsANI_SELECTANI ( pRenderChar, AN_GESTURE, EMANI_SUBTYPE(m_dwANISUBGESTURE) );

			if ( !bOK )
			{
				TurnAction ( GLAT_IDLE );

				if ( IsAwayGesturing() )
					SetAwayGesture( false );

				break;
			}
			
			if ( !m_pSkinChar->IsCurAniConInfo(ACF_LOOP) && pRenderChar->IsANI_ISENDANIM( AN_GESTURE, EMANI_SUBTYPE(m_dwANISUBGESTURE) ) )
			{
				TurnAction ( GLAT_IDLE );

				if ( IsAwayGesturing() )
					SetAwayGesture( false );
			}
		}
		break;

	case GLAT_MOTION:
		{
			SELECTANI( pRenderChar, m_eMotionMID, m_eMotionSID );

			if( m_fMotionSec == FLT_MAX )
			{
				if( pRenderChar->IsANI_ISENDANIM( m_eMotionMID, m_eMotionSID ) )
				{
					TOSTARTTIME( pRenderChar );
				}
			}
			else
			{
				m_fMotionSec -= fElapsedTime;
				if( m_fMotionSec <= 0.0f )
				{
					TurnAction( GLAT_IDLE );
					m_fMotionSec = FLT_MAX;
				}
				else
				{
					if( pRenderChar->IsANI_ISENDANIM( m_eMotionMID, m_eMotionSID ) )
					{
						TOSTARTTIME( pRenderChar );
					}
				}
			}
		}
		break;

	case GLAT_SHOCK:

		if ( !m_emVehicle )
		{
			emSType = m_emANISUBTYPE;
		}

		SELECTANI ( pRenderChar, AN_SHOCK, emSType );
		//		if ( m_pSkinChar->IsANI_ISENDANIM () )	TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:		
		if ( m_emVehicle || !pRenderChar->IsANI_SELECTANI ( AN_SPEC, (EMANI_SUBTYPE)m_dwANISUBSELECT ) )
		{
			SELECTANI ( pRenderChar, AN_GUARD_N, (EMANI_SUBTYPE)emSType );
		}

		break;

	case GLAT_FALLING:

		emMType = AN_DIE;
		emSType = AN_SUB_NONE;
		if( m_sVehicle.m_pSkinChar )
		{
			m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_DOWN );

			SITEM* pItem = NULL;
			pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
			if( pItem )
			{
				if( pItem->VehicleType() == VEHICLE_TYPE_BIKE )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
						emSType = AN_SUB_DRIVER_DIE;
					}
				}
			}
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );
		}
		else
		{
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );
		}

		if ( pRenderChar->IsANI_ISENDANIM ( emMType, emSType ) )
		{
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->TOENDTIME();
			}
			TurnAction ( GLAT_DIE );
		}
		break;
	case GLAT_FEIGN_FALLING :
		emMType = AN_DIE;
		emSType = AN_SUB_NONE;
		if( m_sVehicle.m_pSkinChar )
		{
			m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_DOWN );

			SITEM* pItem = NULL;
			pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
			if( pItem )
			{
				if( pItem->VehicleType() == VEHICLE_TYPE_BIKE )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
						emSType = AN_SUB_DRIVER_DIE;
					}
				}
			}
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );
		}
		else
		{
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );
		}

		if ( pRenderChar->IsANI_ISENDANIM ( emMType, emSType ) )
		{
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->TOENDTIME();
			}
			TurnAction ( GLAT_FEIGN_DIE );
		}
		break;
	case GLAT_FEIGN_DIE :
	case GLAT_DIE:
		bFreeze = TRUE;
		TOENDTIME( pRenderChar );

		if( m_sVehicle.m_pSkinChar )
		{
			m_sVehicle.m_pSkinChar->TOENDTIME();
		}
		else
		{
			SELECTANI ( pRenderChar, AN_DIE, emSType, EMANI_ENDFREEZE );
			TOENDTIME( pRenderChar );
		}
		break;

	case GLAT_CONFT_END:
		{
			EMANI_MAINTYPE emMTYPE(AN_CONFT_LOSS);
			if ( IsActState(EM_ACT_CONFT_WIN) )
				emMTYPE = AN_CONFT_WIN;

			SELECTANI ( pRenderChar, emMTYPE, AN_SUB_NONE );
			if ( pRenderChar->IsANI_ISENDANIM ( emMTYPE, AN_SUB_NONE ) )
				TurnAction ( GLAT_IDLE );
		}
		break;
	case GLAT_GATHERING:
		{
			BOOL bOK = IsANI_SELECTANI ( pRenderChar, AN_GATHERING, EMANI_SUBTYPE(m_dwANISUBGESTURE) );

			if ( !bOK )
			{
				TurnAction ( GLAT_IDLE );
				break;
			}
			
			if ( !m_pSkinChar->IsCurAniConInfo(ACF_LOOP) && pRenderChar->IsANI_ISENDANIM ( AN_GATHERING, EMANI_SUBTYPE(m_dwANISUBGESTURE) ) )
			{
				TurnAction ( GLAT_IDLE );
			}
		}
		break;
	};

	//	Note : 스킨 업데이트.
	//
	SetPositionSkin ( pRenderChar, GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );
	//SetPosition ( GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );

	BOOL bContinue = isAction(GLAT_ATTACK, GLAT_SKILL);

	//	Note : 상태 이상에 따라 모션 속도를 조정한다.
	//
	float fSkinAniElap = fElapsedTime;
	switch ( Action )
	{
	case GLAT_MOVE:
		//  [6/5/2014 gbgim]
		// - 원래 탈 것을 탈 경우, 이동속도에 따라 탈것의 애니메이션 속도도 같이 증가했으나;
		//   데스바이크가 추가되면서 부스트 이펙트가 이동속도에 따라 다르게 나온다며 기획팀에서;
		//   이동속도와 상관없이 애니메이션은 동일하게 나와야한다고 결정함 CRM#2365;
		if ( VehicleState() == EMVEHICLE_OFF )
		{
			fSkinAniElap *= GLCHARLOGIC::GETMOVE_TOTAL();
		}
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= (GLCHARLOGIC::GETATTVELO() + GLCHARLOGIC::GETATT_ITEM());
		// fSkinAniElap += GLCHARLOGIC::GETATT_ITEM(); // 절대치
		break;
	};


	CalculateMatrix( GetPositionActor() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );


	// CalculateMatrix 에서 Matrix 값은 다 얻었다.
	// 여기는 그 계산된 Matrix 값을 넣는 장소
	if ( !IsActState(EM_REQ_VISIBLENONE) )
	{
		// 바이크 타면 언제가 되었든 계산은 되야 한다.
		// 드라이버면 나와 내가 태운 캐릭터의 FrameMove 를 해야 한다.
		// Vehicle을 탔지만 드라이버가 아닐경우는 계산하지 않는다. 다른곳에서 하기 때문.
		if ( m_emVehicle == EMVEHICLE_DRIVER )
		{
			DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;
			if( pVehicle )
			{
				// Vehicle FrameMove
				pVehicle->FrameMove ( fTime, fSkinAniElap, TRUE, m_matBikeTrans, bContinue, bFreeze );

				// 오토바이에 탄 내 자신 FrameMove
				pRenderChar->FrameMoveCharOnVehicle ( fTime, fSkinAniElap, bContinue, bFreeze, !bPeaceZone, pVehicle, 0, TRUE );

				// Vehicle 에 탄 Char FrameMove
				for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
				{
					std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
					if( pCharClient )
					{
						pCharClient->GetSkinChar()->FrameMoveCharOnVehicle ( fTime, fSkinAniElap, bContinue, bFreeze, !bPeaceZone, pVehicle, i, TRUE );
					}
				}
			}
			else
			{
				// pVehicle가 NULL 이라면, 보드를 타고 있을 경우임.
				pRenderChar->FrameMove ( fTime, fSkinAniElap, TRUE, GetTransMatrix(), bContinue, bFreeze, !bPeaceZone );
			}
		}
		else if ( m_emVehicle == EMVEHICLE_PASSENGER )	// 탑승자일 경우 GLCharClient 에서 FrameMove 를 해준다.
		{
			// 아무작업도 안함.
		}
		else
		{

			// 아무것도 탑승하지 않는 상태임.
			FrameMoveSkin ( pRenderChar, fTime, fSkinAniElap, TRUE, GetTransMatrix(), bContinue, bFreeze, !bPeaceZone );
		}
	}

	//CalculateMatrix();


	//if ( !IsActState(EM_REQ_VISIBLENONE) )
	//{
	//	// 바이크 타면 언제가 되었든 계산은 되야 한다.
	//	if( m_emVehicle )
	//	{
	//		// 드라이버면 나와 내가 태운 캐릭터의 FrameMove 를 해야 한다.
	//		// 탔지만 드라이버가 아닐경우는 계산하지 않는다. 다른곳에서 하기 때문.
	//		if( m_emVehicle == EMVEHICLE_DRIVER )
	//		{
	//			if ( m_sVehicle.m_pSkinChar )
	//			{
	//				m_sVehicle.m_pSkinChar->FrameMove ( fTime, fSkinAniElap, m_matBikeTrans, bContinue, bFreeze );
	//			}

	//			pRenderChar->FrameMoveCharOnVehicle( fTime, fSkinAniElap, bContinue, bFreeze, !bPeaceZone, m_sVehicle.m_pSkinChar, GetVehiclePassengerID() );
	//		}			
	//	}
	//	else
	//	{
	//		pRenderChar->FrameMove ( fTime, fSkinAniElap, m_matTrans, bContinue, bFreeze, !bPeaceZone );
	//	}
	//}

	//	Note : 비 전투 지역일때 표시 안되야 하는 아이템(단검,투척) 인지 검사후 랜더 끄기.
	//
	if ( bPeaceZone )
	{
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		SITEM *pItemR = GET_SLOT_ITEMDATA ( emRHand );
		SITEM *pItemL = GET_SLOT_ITEMDATA ( emLHand );

		if ( pItemR )
		{
			BOOL bRend = !( pItemR->sSuitOp.emAttack==ITEMATT_DAGGER || pItemR->sSuitOp.emAttack==ITEMATT_THROW );
			SetPartRend ( pRenderChar, PIECE_RHAND, bRend );
		}
		if ( pItemL )
		{
			BOOL bRend = !( pItemL->sSuitOp.emAttack==ITEMATT_DAGGER || pItemL->sSuitOp.emAttack==ITEMATT_THROW );
			SetPartRend ( pRenderChar, PIECE_LHAND, bRend );
		}
	}
	else
	{
		SetPartRend ( pRenderChar, PIECE_RHAND, TRUE );
		SetPartRend ( pRenderChar, PIECE_LHAND, TRUE );
	}

	return S_OK;
}
