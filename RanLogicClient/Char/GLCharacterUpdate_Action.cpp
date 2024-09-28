#include "pch.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"

#include "../Land/GLLandManClient.h"
#include "../GLogicExClient.h"
#include "../GLGaeaClient.h"

#include "GLCharacter.h"

HRESULT GLCharacter::UpdateAction(float fTime, float fElapsedTime)
{
	//	Note : ������ ó��.
	//
	switch ( GetAction() )
	{
	case GLAT_IDLE:
		//1044 = sizeof(SDROP_CHAR); �ֺ��� �޸�
		//1052 = sizeof(GLMSG::SNETDROP_PC); �ֺ��� �޸�

		if( m_fIdleTime > 20.0f )
		{
			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
			BOOL bPeaceZone = pLand ? pLand->IsPeaceZone() : FALSE;

			if ( !bPeaceZone && !IsActState(EM_ACT_PEACEMODE) )
				ReqTogglePeaceMode();
		} //if
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:						// �ɸ��� �̵� ������Ʈ.
		{
			ClientActor::SetMaxSpeed(GetMoveVelo());

			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
			if ( pLand )
			{
				if ( FAILED( ClientActor::Update( pLand->GetvecLayerCollision(), fElapsedTime, TRUE)) )
					return E_FAIL;
			}
			else
			{
				std::vector<boost::shared_ptr<LayerCollision>> vecLayerCollision;
				if ( FAILED( ClientActor::Update( vecLayerCollision, fElapsedTime, TRUE)) )
					return E_FAIL;
			}

			// [shhan][2014.10.23] WorldEdit ������ m_pGaeaClient->GetLandManClient() �� NULL �̶� üũ�ϰ� ��.
			if ( m_pGaeaClient->GetLandManClient() && m_pGaeaClient->GetLandManClient()->IsObstacle(  m_actorMove.Position(), m_actorMove.GetPreviousPosition() ))
			{
				m_actorMove.Stop ();
				ClientActor::SetPositionActor(m_actorMove.GetPreviousPosition());
				//ClientActor::SetPosition(m_actorMove.GetPreviousPosition());
			}
			else
			{
				if( !ClientActor::PathIsActive() && m_emVehicle != EMVEHICLE_PASSENGER )
				{
					ClientActor::StopActorMove();
					TurnAction ( GLAT_IDLE );
				} //if
			}

			const D3DXVECTOR3& vPosition = ClientActor::GetPositionActor();
			ClientActor::SetPosition(vPosition);														// �ɸ��� ���� ��ġ ������Ʈ.
			D3DXVECTOR3 vMovement = ClientActor::GetNextPosition();		// �ɸ��� ���� ���� ������Ʈ.
			if( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
			{
				D3DXVECTOR3 vDirection = vMovement - vPosition;
				if ( !DxIsMinVector(vDirection,0.2f) )
				{
					D3DXVec3Normalize ( &vDirection, &vDirection );
					ClientActor::SetDirect(vDirection);
				} //if
			} //if
		}
		break;

	case GLAT_ATTACK:
		{
			AttackProc ( fElapsedTime );

			ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sActionID );
			if ( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - GetPosition();			// �ɸ��� ���� ���� ������Ʈ.
				D3DXVec3Normalize ( &vDirection, &vDirection );
				ClientActor::SetDirect(vDirection);
			} //if
		}
		break;

	case GLAT_SKILL:
		{
			if( !SkillProc( fElapsedTime ) )
				TurnAction( GLAT_IDLE );

			const D3DXVECTOR3 vReDirect = 
				UpdateSkillDirection(m_pGaeaClient, GetPosition(), GetDirect(), m_idACTIVESKILL, m_sSkillTargetData.TargetID.vPos, m_sSkillTargetData.TargetIDs);	// ���� �������� ȸ��.
			ClientActor::SetDirect(vReDirect);
		}		
		break;

	case GLAT_SHOCK:
		TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		{
			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
			if ( pLand )
			{
				ClientActor::Update ( pLand->GetvecLayerCollision(), fElapsedTime, TRUE );
			}
			else
			{
				std::vector<boost::shared_ptr<LayerCollision>> vecLayerCollision;
				ClientActor::Update ( vecLayerCollision, fElapsedTime, TRUE );
			}

			if ( !ClientActor::PathIsActive() )
			{						
				if ( m_pSkinChar->IsCurAniConInfo(ACF_LOOP) || m_pSkinChar->ISENDANIM () )
				{
					ClientActor::StopActorMove();
					TurnAction ( GLAT_IDLE );
				}
			}
		}
		break;

	case GLAT_FALLING:
		break;
	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	}; //switch

	if( m_sCONFTING.IsCONFRONTING() )
	{
		if( !m_sCONFTING.IsFIGHTING() )
		{
			DWORD dwCOUNT = m_sCONFTING.UPDATE(fElapsedTime);
			
			if( EMCONFT_COUNT != dwCOUNT )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_ConftDisplay,
					"-n", int( dwCOUNT + 1 ) );
			}
		} //if
	} //if

	return S_OK;
}