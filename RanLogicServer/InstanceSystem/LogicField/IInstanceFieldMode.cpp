#include "pch.h"

#include "../../../enginelib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicDataMan.h"

#include "../../FieldServer/GLGaeaServer.h"
#include "../../Club/GLClubManField.h"
#include "../../Club/GLClubField.h"
#include "../../Country/GLVictoriousCountryManField.h"
#include "../../Util/GLItemLimit.h"

#include "./IInstanceFieldMode.h"

namespace InstanceSystem
{

	GLChar* IInstanceFieldMode::GetChar ( const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
			return NULL;

		return m_pServer->GetCharByDbNum( _dwChaDBNum );
	}


	ServerActor* IInstanceFieldMode::GetActor ( const Faction::GLActorID& _sActorID )
	{
		if ( NULL == m_pServer )
			return NULL;

		if ( NULL == m_pLandManager )
			return NULL;

		return m_pServer->GetActor( m_pLandManager, _sActorID );
	}


	const DWORD IInstanceFieldMode::GetHP ( const Faction::GLActorID& _sActorID )
	{
		const ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return 0;

		return pActor->GetNowHP();
	}


	const DWORD IInstanceFieldMode::GetMP ( const Faction::GLActorID& _sActorID )
	{
		const ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return 0;

		return pActor->GetNowMP();
	}


	const DWORD IInstanceFieldMode::GetSP ( const Faction::GLActorID& _sActorID )
	{
		const ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return 0;

		return pActor->GetNowSP();
	}


	const DWORD IInstanceFieldMode::GetMaxHP ( const Faction::GLActorID& _sActorID )
	{
		const ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return 0;

		return pActor->GetMaxHP();
	}


	const DWORD IInstanceFieldMode::GetMaxMP ( const Faction::GLActorID& _sActorID )
	{
		const ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return 0;

		return pActor->GetMaxMP();
	}


	const DWORD IInstanceFieldMode::GetMaxSP ( const Faction::GLActorID& _sActorID )
	{
		const ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return 0;

		return pActor->GetMaxSP();
	}


	const bool IInstanceFieldMode::SetHP ( const Faction::GLActorID& _sActorID, const DWORD _dwHP )
	{
		ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return false;

		switch( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* const pChar = static_cast< GLChar* >( pActor );
				if ( NULL == pChar )
					return false;

				pChar->m_sHP.nNow = _dwHP;

				// [자신에게] 회복상태 갱신;
				GLMSG::SNETPC_UPDATE_STATE sNetMsg;
				sNetMsg.sHP = pChar->m_sHP;
				sNetMsg.sMP =pChar->m_sMP;
				sNetMsg.sSP = pChar->m_sSP;
				pChar->SendToClient( &sNetMsg );

				// 주위에 체력 변화 알린다;
				GLMSG::SNETPC_UPDATE_STATE_BRD sNetMsgBrd;
				sNetMsgBrd.dwGaeaID = pChar->GetGaeaID();
				sNetMsgBrd.sHP = pChar->m_sHP;
				pChar->SendMsgViewAround( &sNetMsgBrd );

			}
			break;

		case CROW_MOB:
		case CROW_NPC:
		case CROW_MATERIAL:
			{
				GLCrow* const pCrow = static_cast< GLCrow* >( pActor );
				if ( NULL == pCrow )
					return false;

				pCrow->m_dwNowHP = _dwHP;

				GLMSG::SNETCROW_UPDATE_STATE sNetMsgBrd(
					pCrow->GetGaeaID(),
					pCrow->m_dwNowHP );
				pCrow->SendMsgViewAround( &sNetMsgBrd );
			}
			break;

		default:
			break;
		}

		return true;
	}


	const bool IInstanceFieldMode::SetMP ( const Faction::GLActorID& _sActorID, const DWORD _dwMP )
	{
		ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return false;

		switch( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* const pChar = static_cast< GLChar* >( pActor );
				if ( NULL == pChar )
					return false;

				pChar->m_sMP.nNow = _dwMP;
			}
			break;

		case CROW_MOB:
		case CROW_NPC:
		case CROW_MATERIAL:
			break;

		default:
			break;
		}

		return true;
	}


	const bool IInstanceFieldMode::SetSP ( const Faction::GLActorID& _sActorID, const DWORD _dwSP )
	{
		ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return false;

		switch( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* const pChar = static_cast< GLChar* >( pActor );
				if ( NULL == pChar )
					return false;

				pChar->m_sSP.nNow = _dwSP;
			}
			break;

		case CROW_MOB:
		case CROW_NPC:
		case CROW_MATERIAL:
			break;

		default:
			break;
		}

		return true;
	}


	const bool IInstanceFieldMode::SetCTICountry (
		const SNATIVEID& _sMapID,
		const DWORD _dwCountryID,
		bool _bUpdate )
	{
		Country::SVICTORIOUS_COUNTRY sVCountry;
		sVCountry.emFlag = Country::CAPTURE_THE_ISLANDS;
		sVCountry.sMapID = _sMapID;
		sVCountry.sCountryInfo.dwID = _dwCountryID;

		Country::GLVictoriousCountryManField* pVCountryMan =
			Country::GLVictoriousCountryManField::Instance();
		if ( pVCountryMan )
		{
			pVCountryMan->SendAddVCountryFA( sVCountry, _bUpdate );

			return true;
		}

		return false;
	}


	const DWORD IInstanceFieldMode::GetClubID ( const DWORD _dwChaDBNum )
	{
		const GLChar* const pChar = GetChar( _dwChaDBNum );
		if ( NULL == pChar )
			return 0;

		const DWORD dwClubID = pChar->GetClubDbNum();
		if ( CLUB_NULL == dwClubID )
			return 0;

		return dwClubID;
	}


	const DWORD IInstanceFieldMode::GetCountry ( const DWORD _dwChaDBNum )
	{
		const GLChar* const pChar = GetChar( _dwChaDBNum );
		if ( NULL == pChar )
			return Country::UNKNOWN_COUNTRY_INFO;

		const Country::SCOUNTRY_INFO& sCountryInfo =
			pChar->GetCountryInfo();

		return sCountryInfo.dwID;
	}


	const DWORD IInstanceFieldMode::GetSchool ( const DWORD _dwChaDBNum )
	{
		const GLChar* const pChar = GetChar( _dwChaDBNum );
		if ( NULL == pChar )
			return SCHOOL_NONE;

		return pChar->GetSchool();
	}


	const std::string IInstanceFieldMode::GetClubName ( const DWORD _dwClubID )
	{
		static std::string strDefaultReturnValue;

		if ( NULL == m_pServer )
			return strDefaultReturnValue;

		GLClubManField* pClubMan = m_pServer->GetClubMan();
		if ( NULL == pClubMan )
			return strDefaultReturnValue;

		std::tr1::shared_ptr< GLClubField > spClub = pClubMan->GetClub( _dwClubID );
		if ( NULL == spClub )
			return strDefaultReturnValue;

		return spClub->Name();
	}


	const std::string IInstanceFieldMode::GetClubMasterName ( const DWORD _dwClubID )
	{
		static std::string strDefaultReturnValue;

		if ( NULL == m_pServer )
			return strDefaultReturnValue;

		GLClubManField* pClubMan = m_pServer->GetClubMan();
		if ( NULL == pClubMan )
			return strDefaultReturnValue;

		std::tr1::shared_ptr< GLClubField > spClub = pClubMan->GetClub( _dwClubID );
		if ( NULL == spClub )
			return strDefaultReturnValue;

		return spClub->GetMasterName();
	}


	const D3DXVECTOR3 IInstanceFieldMode::GetActorPosition ( const Faction::GLActorID& _sActorID )
	{
		static D3DXVECTOR3 sDefaultPosition;

		ServerActor* const pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return sDefaultPosition;

		return pActor->GetPosition();
	}


	const std::string IInstanceFieldMode::GetCountryName ( const DWORD _dwCountryID )
	{
		static std::string strDefaultReturnValue;

		Country::GLCountryManServer* pCountryMan = Country::GLCountryManServer::Instance();
		if ( NULL == pCountryMan )
			return strDefaultReturnValue;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( _dwCountryID );
		if ( NULL == pCountry )
			return strDefaultReturnValue;

		return pCountry->GetCountryName();
	}


	const std::string IInstanceFieldMode::GetActorName ( const Faction::GLActorID& _sActorID )
	{
		static std::string strDefaultReturnValue;

		ServerActor* pActor = GetActor( _sActorID );
		if ( NULL == pActor )
			return strDefaultReturnValue;

		switch ( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* const pChar = static_cast< GLChar* >( pActor );
				if ( NULL == pChar )
					return strDefaultReturnValue;

				return pChar->GetName();
			}
			break;

		case CROW_MOB:
		case CROW_NPC:
		case CROW_MATERIAL:
			{
				GLCrow* const pCrow = static_cast< GLCrow* >( pActor );
				if ( NULL == pCrow )
					return strDefaultReturnValue;

				return pCrow->GETNAME();
			}
			break;

		default:
			break;
		}

		return strDefaultReturnValue;
	}


	const bool IInstanceFieldMode::IsAllianceClub ( const DWORD _dwClubID, const DWORD _dwTargetClubID )
	{
		if ( NULL == m_pServer )
			return false;

		std::tr1::shared_ptr< GLClubField > spClub;
		std::tr1::shared_ptr< GLClubField > spTargetClub;

		GLClubManField* pClubMan = m_pServer->GetClubMan();
		if ( NULL == pClubMan )
			return false;

		spClub = pClubMan->GetClub( _dwClubID );
		if ( NULL == spClub )
			return false;

		spTargetClub = pClubMan->GetClub( _dwTargetClubID );
		if ( NULL == spTargetClub )
			return false;

		// 동맹클럽인지 확인한다;
		if ( false == spClub->IsAlliance() )
			return false;

		if ( spClub->IsAllianceGuild_COMMON( spTargetClub->m_DbNum, spTargetClub->m_dwAlliance ) )
			return true;

		return false;
	}


	const bool IInstanceFieldMode::JoinParty ( const DWORD _dwMasterChaDBNum, const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pMasterChar = m_pServer->GetCharByDbNum( _dwMasterChaDBNum );
		if ( NULL == pMasterChar )
			return false;

		GLChar* const pMemberChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pMemberChar )
			return false;

		GLMSG::NET_INSTANCE_JOIN_PARTY_FA sNetMsg(
			pMasterChar->GetGaeaID(),
			pMemberChar->GetGaeaID(),
			SPartyOption() );
		m_pServer->SENDTOAGENT( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::OutParty ( const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLMSG::NET_INSTANCE_OUT_PARTY_FA sNetMsg( pChar->GetGaeaID() );
		gpGaeaServer->SENDTOAGENT( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::ClearParty ( const DWORD _dwPartyID )
	{
		GLMSG::NET_INSTANCE_CLEAR_PARTY_FA sNetMsg(_dwPartyID);
		gpGaeaServer->SENDTOAGENT( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::SetViewRange (
		const DWORD _dwChaDBNum,
		const DWORD _dwViewRange,
		const float _fDuration )
	{
		if ( NULL == m_pServer )
			return false;

		if ( _dwViewRange <= 0 )
			return false;

		if ( _fDuration <= 0.f )
			return false ;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// 뷰어라운드 세팅;
		pChar->SetViewAround( (unsigned int)_dwViewRange, _fDuration );

		return true;
	}


	const DWORD IInstanceFieldMode::GetItemCount (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sItemID )
	{
		if ( NULL == m_pServer )
			return 0;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return 0;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( _sItemID );
		if ( NULL == pItem )
			return 0;

		return pChar->m_cInventory.CountTurnItem( _sItemID );
	}


	const bool IInstanceFieldMode::AddItem (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sItemID,
		const DWORD _dwItemCount,
		const DWORD _dwGradeAttack,
		const DWORD _dwGradeDefense,
		const bool _bRandomOption )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( _sItemID );
		if ( NULL == pItem )
			return false;

		if ( pItem->ISPILE() )
		{
			SITEMCUSTOM sITEM_NEW( _sItemID );
			sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
			sITEM_NEW.wTurnNum = _dwItemCount;
			sITEM_NEW.cGenType = EMGEN_ITEMMIX;

			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			pChar->LogItemBasicStatOption( sITEM_NEW );
			pChar->LogItemLinkSkillOption( sITEM_NEW );
			pChar->LogItemAddonOption( sITEM_NEW );

			sITEM_NEW.cDAMAGE = _dwGradeAttack;
			sITEM_NEW.cDEFENSE = _dwGradeDefense;

			if ( _bRandomOption )
			{
				if( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
					pChar->LogItemRandomOption( sITEM_NEW );
			}

			CItemDrop cDropItem;
			cDropItem.sItemCustom = sITEM_NEW;
			if ( pChar->IsInsertToInven( &cDropItem ) )
			{
				pChar->InsertToInven( &cDropItem, true, false, false, false, true );
			}
			else
			{
				pChar->DropItem( cDropItem.sItemCustom, pChar->GetPosition() );
			}
		}
		else
		{
			for( int i = 0; i < _dwItemCount; i++ )
			{
				SITEMCUSTOM sITEM_NEW( _sItemID );
				sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
				sITEM_NEW.cGenType = EMGEN_ITEMMIX;

				sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
				sITEM_NEW.GenerateBasicStat( false );
				sITEM_NEW.GenerateLinkSkill();
				sITEM_NEW.GenerateAddOption();

				pChar->LogItemBasicStatOption( sITEM_NEW );
				pChar->LogItemLinkSkillOption( sITEM_NEW );
				pChar->LogItemAddonOption( sITEM_NEW );

				sITEM_NEW.cDAMAGE = _dwGradeAttack;
				sITEM_NEW.cDEFENSE = _dwGradeDefense;

				if ( _bRandomOption )
				{
					if( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
						pChar->LogItemRandomOption( sITEM_NEW );
				}

				CItemDrop cDropItem;
				cDropItem.sItemCustom = sITEM_NEW;
				if ( pChar->IsInsertToInven( &cDropItem ) )
				{
					pChar->InsertToInven( &cDropItem, true, false, false, false, true );
				}
				else
				{
					pChar->DropItem( cDropItem.sItemCustom, pChar->GetPosition() );
				}
			}
		}

		return true;
	}


	const bool IInstanceFieldMode::RemoveItem (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sItemID,
		const DWORD _dwItemCount )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( _sItemID );
		if ( NULL == pItem )
			return false;

		if ( pItem->ISPILE() )
		{
			SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( _sItemID );
			if( NULL == pInvenItem )
				return false;

			if( false == pChar->DeletePileItem( _sItemID, _dwItemCount ) )
				return false;

			// 통계 및 로그 수정;
			pChar->LogItemExchange(
				pInvenItem->sItemCustom,
				gamelog::ID_CHAR,
				pChar->m_CharDbNum,
				gamelog::ID_CHAR,
				0, 
				ITEM_ROUTE_SYSTEM,
				pInvenItem->sItemCustom.wTurnNum,
				( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(),
				LOG_ITEM_DEL_ITEM_BY_LUA,
				true );
		}
		else
		{
			for ( int i = 0; i < _dwItemCount; ++i )
			{
				SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( _sItemID );
				if ( NULL == pInvenItem )
					break;

				if ( false == pChar->InvenDeleteItem(
					pInvenItem->wPosX,
					pInvenItem->wPosY,
					true,
					FALSE,
					1897,
					true ) )
					return false;

				// 통계 및 로그 수정;
				pChar->LogItemExchange(
					pInvenItem->sItemCustom,
					gamelog::ID_CHAR,
					pChar->m_CharDbNum,
					gamelog::ID_CHAR,
					0, 
					ITEM_ROUTE_SYSTEM,
					1,
					( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(),
					LOG_ITEM_DEL_ITEM_BY_LUA,
					true );
			}
		}

		return true;
	}


	const bool IInstanceFieldMode::SetPositionInstance (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sMapID,
		const D3DXVECTOR3& _vPosition )
	{
		if ( NULL == m_pManagerField )
			return false;

		if ( CROW_PC != _sActorID.actorType )
			return false;

		// 최초 이동 요청인 경우 field단에서는 생성 여부에 상관 없이 사용 가능 하지만,;
		// targetMap은 아직 생성 되어 있지 않은 점에 유의 해야 한다;
		const InformationPosition sReserveSetPosition(
			_sActorID,
			_sMapID,                 
			Faction::GLActorID( CROW_NULL, GAEAID_NULL ),
			_vPosition.x, _vPosition.z,
			InstanceSystem::EMMOVE_INSTANCE2INSTANCE );

		return m_pManagerField->requestMoveToPosition(
			m_sMapID.getGaeaMapID().Mid(),
			sReserveSetPosition );
	}


	const bool IInstanceFieldMode::SetObjectPositionInstance (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sMapID,
		const Faction::GLActorID& _sTargetActorID )
	{
		if ( NULL == m_pManagerField )
			return false;

		if ( CROW_PC != _sActorID.actorType )
			return false;

		// 최초 이동 요청인 경우 field단에서는 생성 여부에 상관 없이 사용 가능 하지만,;
		// targetMap은 아직 생성 되어 있지 않은 점에 유의 해야 한다;
		const InformationPosition sReserveSetPosition(
			_sActorID,
			_sMapID,                 
			_sTargetActorID,
			0.f, 0.f,
			InstanceSystem::EMMOVE_INSTANCE2INSTANCE );

		return m_pManagerField->requestMoveToPosition(
			m_sMapID.getGaeaMapID().Mid(),
			sReserveSetPosition );
	}


	const bool IInstanceFieldMode::SetPositionField (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sMapID,
		const D3DXVECTOR3& _vPosition )
	{
		if ( NULL == m_pManagerField )
			return false;

		if ( CROW_PC != _sActorID.actorType )
			return false;

		// SetObjectPositionInstance()/SetPositionInstance() 와는 달리,
		// player만 인던 바깥으로 이동 시킬 수 있다;
		const InformationPosition sReserveSetPosition(
			_sActorID,
			_sMapID,                 
			Faction::GLActorID(CROW_NULL, GAEAID_NULL),
			_vPosition.x, _vPosition.z,
			InstanceSystem::EMMOVE_INSTANCE2FIELD );

		return m_pManagerField->requestMoveToPosition(
			m_sMapID.getGaeaMapID().Mid(),
			sReserveSetPosition );
	}


	const bool IInstanceFieldMode::SetGatePositionField (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sMapID,
		const DWORD _dwGateID )
	{
		if ( NULL == m_pManagerField )
			return false;

		if ( CROW_PC != _sActorID.actorType )
			return false;

		// SetObjectPositionInstance()/SetPositionInstance() 와는 달리,
		// player만 인던 바깥으로 이동 시킬 수 있다;
		const InformationPosition sReserveSetPosition(
			_sActorID,
			_sMapID,                 
			Faction::GLActorID( CROW_GATE, _dwGateID ),
			0.f, 0.f,
			InstanceSystem::EMMOVE_INSTANCE2FIELD );

		return m_pManagerField->requestMoveToPosition(
			m_sMapID.getGaeaMapID().Mid(),
			sReserveSetPosition );
	}


	const bool IInstanceFieldMode::SetReturnPosition (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sMapID,
		const DWORD _dwGateID )
	{
		if ( NULL == m_pServer )
			return false;

		if ( CROW_PC != _sActorID.actorType )
			return false;

		// Return Map Information
		MapInformation sReturnMapInformation = MapInformation(
			_sMapID,
			D3DXVECTOR3( 0.f, 0.f, 0.f ),
			Faction::GLActorID( CROW_GATE, _dwGateID ) );

		// Agent 에 알려 갱신한다;
		GLMSG::NET_SET_RETURNMAP_INFORMATION_FA sNetMsgFA(
			m_sMapID.getGaeaMapID().Mid(),
			m_sMapID.getBaseMapID() );
		sNetMsgFA.dwChaDBNum = _sActorID.actorID_Num;
		sNetMsgFA.sReturnMapInformation = sReturnMapInformation;

		m_pServer->SENDTOAGENT( &sNetMsgFA );
	}


	const bool IInstanceFieldMode::SetResurrectionForced (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sMapID,
		const DWORD _dwGateID,
		const float _fTime,
		const float _fRestore )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetResurrectionForced(
			false,
			_sMapID,
			_dwGateID,
			D3DXVECTOR3( 0.f, 0.f, 0.f),
			_fTime,
			_fRestore );

		return true;
	}


	const bool IInstanceFieldMode::SetResurrectionForcedPosition (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sMapID,
		const D3DXVECTOR3& _vPosition,
		const float _fTime,
		const float _fRestore )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetResurrectionForced(
			false,
			_sMapID,
			GAEAID_NULL,
			_vPosition,
			_fTime,
			_fRestore );

		return true;
	}


	const bool IInstanceFieldMode::SetResurrectionForcedInstance (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sMapID,
		const DWORD _dwGateID,
		const float _fTime,
		const float _fRestore )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetResurrectionForced(
			true,
			_sMapID,
			_dwGateID,
			D3DXVECTOR3( 0.f, 0.f, 0.f),
			_fTime,
			_fRestore );

		return true;
	}


	const bool IInstanceFieldMode::SetResurrectionForcedInstancePosition (
		const DWORD _dwChaDBNum,
		const SNATIVEID& _sMapID,
		const D3DXVECTOR3& _vPosition,
		const float _fTime,
		const float _fRestore )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetResurrectionForced(
			true,
			_sMapID,
			GAEAID_NULL,
			_vPosition,
			_fTime,
			_fRestore );

		return true;
	}


	const bool IInstanceFieldMode::SetResurrectionDisable (
		const DWORD _dwChaDBNum,
		const bool _bDisableSkill )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetResurrectionDisable( _bDisableSkill );

		return true;
	}


	const bool IInstanceFieldMode::SetResurrectionDisableSkill (
		const DWORD _dwChaDBNum,
		const bool _bDisableSkill )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetResurrectionDisableSkill( _bDisableSkill );

		return true;
	}


	const bool IInstanceFieldMode::SetDeathPenalty (
		const DWORD _dwChaDBNum,
		const DWORD _dwDropExp,
		const DWORD _dwDropItemNum,
		const float _fDropItemRate )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->DeathPenalty( _dwDropExp, _dwDropItemNum, _fDropItemRate );

		return true;
	}


	const bool IInstanceFieldMode::SetJoinSuspendInstance ( const bool _bLock )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA sNetMsg(
			m_sMapID.getGaeaMapID().Mid(),
			SNATIVEID( false ),
			_bLock );

		m_pServer->SENDTOMYSELF( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::SetJoinLockInstance ( const bool _bLock )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_JOIN_LOCK_FA sNetMsg(
			m_sMapID.getGaeaMapID().Mid(),
			SNATIVEID( false ),
			_bLock );
		m_pServer->SENDTOMYSELF( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::SetJoinLockType ( const bool _bLock )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_JOIN_LOCK_FA sNetMsg(
			GAEAID_NULL,
			m_sMapID.getBaseMapID(),
			_bLock );

		m_pServer->SENDTOMYSELF( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::ActiveFactionChatting ( void )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->ActiveFactionChatting();

		return true;
	}


	const bool IInstanceFieldMode::InactiveFactionChatting ( void )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->InactiveFactionChatting();

		return true;
	}


	const bool IInstanceFieldMode::SetHPVisible ( const bool _bHPVisible )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->setHpVisible(
			_bHPVisible ? Faction::EM_RELATION_FLAG_ALL : Faction::EM_RELATION_FLAG_NONE );

		return true;
	}


	const bool IInstanceFieldMode::SetCountryMark ( const DWORD _dwChaDBNum, const bool _bCountryMark )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// 캐릭터에게 Country Mark 정보를 보낸다;
		GLMSG::NET_SHOW_COUNTRY_MARK_FC sMsg;
		sMsg.m_bShowCountryMark = _bCountryMark;

		m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sMsg );

		return true;
	}


	const bool IInstanceFieldMode::SetVisible ( const DWORD _dwChaDBNum, const bool _bVisible )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		if ( _bVisible )
			pChar->ReSetActState( EM_REQ_VISIBLEOFF );
		else
			pChar->SetActState( EM_REQ_VISIBLEOFF );

		GLMSG::NET_INSTANCE_SET_ACT_STATE_BRD sNetMsgBrd( pChar->GetGaeaID(), EM_REQ_VISIBLEOFF, (!_bVisible) );
		pChar->SendMsgViewAround( &sNetMsgBrd );
		m_pServer->SENDTOCLIENT( pChar->GETCLIENTID(), &sNetMsgBrd );

		return true;
	}


	const bool IInstanceFieldMode::SetMoveLock ( const DWORD _dwChaDBNum, const bool _bMoveLock )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		pChar->SetMovement( CHAR_MOVEMENT_INSTANCE_SCRIPT, _bMoveLock );

		return true;
	}
	

	const DWORD IInstanceFieldMode::RegistTimer (
		const DWORD _nTimeSec,
		const DWORD _dwParam0,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const bool _bLoop )
	{
		if ( NULL == m_pInstanceField )
			return MAXDWORD;

		if ( m_pInstanceField->IsReserveDestroy() )
			return MAXDWORD;

		const DWORD hTimer = m_pInstanceField->registTimer(
			_nTimeSec,
			_dwParam0,
			_dwParam1,
			_dwParam2,
			_dwParam3,
			_bLoop );

		return hTimer;
	}


	const bool IInstanceFieldMode::RemoveTimer ( const DWORD _hTimer )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( m_pInstanceField->IsReserveDestroy() )
			return false;

		if ( -1 == m_pInstanceField->removeTimer( _hTimer ) )
			return false;

		return true;
	}


	const bool IInstanceFieldMode::RemoveAllTimer ( void )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( m_pInstanceField->IsReserveDestroy() )
			return false;

		return m_pInstanceField->removeAllTimer();
	}


	const bool IInstanceFieldMode::IsRegistTimer ( const DWORD _hTimer )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( m_pInstanceField->IsReserveDestroy() )
			return false;

		return m_pInstanceField->isRegistTimer( _hTimer );
	}


	const bool IInstanceFieldMode::SuspendTimer ( const DWORD _hTimer )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( m_pInstanceField->IsReserveDestroy() )
			return false;

		if ( -1 == m_pInstanceField->suspendTimer( _hTimer ) )
			return false;

		return true;
	}


	const bool IInstanceFieldMode::ResumeTimer ( const DWORD _hTimer )
	{
		if ( NULL == m_pInstanceField )
			return false;

		if ( m_pInstanceField->IsReserveDestroy() )
			return false;

		if ( -1 == m_pInstanceField->resumeTimer( _hTimer ) )
			return false;

		return true;
	}


	const int IInstanceFieldMode::GetLeftTime ( const DWORD _hTimer )
	{
		int nTime = -1;
		if ( NULL == m_pInstanceField )
			return nTime;

		if ( m_pInstanceField->IsReserveDestroy() )
			return nTime;
		
		nTime = m_pInstanceField->getLeftTime( _hTimer );

		return nTime;
	}


	const Faction::FactionMemberVector* IInstanceFieldMode::GetFactionList (
		const Faction::FactionID& _sFactionID )
	{
		if ( NULL == m_pLandManager )
			return NULL;

		Faction::ManagerField* const pFactionManager( m_pLandManager->GetFactionManager() );
		if ( NULL == pFactionManager )
			return NULL;

		return pFactionManager->getFactionMembers( _sFactionID );
	}


	const bool IInstanceFieldMode::SetFaction (
		const Faction::GLActorID& _sActorID,
		const Faction::FactionID& _sFactionID )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->setFactionID( _sActorID, _sFactionID );

		return true;
	}


	const bool IInstanceFieldMode::SetRelation (
		const Faction::FactionID& _sMyFactionID,
		const Faction::FactionID& _sTargetFactionID,
		const Faction::EMRELATION_TYPE _sRelationType )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->setRelation( _sMyFactionID, _sTargetFactionID, _sRelationType );

		return true;
	}


	const bool IInstanceFieldMode::SetRelationToAll (
		const Faction::FactionID& _sFactionID,
		const Faction::EMRELATION_TYPE _sRelationType )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->setRelationToAll( _sFactionID, _sRelationType );

		return true;
	}


	const bool IInstanceFieldMode::SetRelationFromAll (
		const Faction::FactionID& _sFactionID,
		const Faction::EMRELATION_TYPE _sRelationType )
	{
		if ( NULL == m_pLandManager )
			return false;

		m_pLandManager->setRelationFromAll( _sFactionID, _sRelationType );

		return true;
	}


	const bool IInstanceFieldMode::AddMobLayer (
		const SNATIVEID& _sMapID,
		const DWORD _dwLayerIndex )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLLandMan* pLand = m_pLandManager->getLand( _sMapID );
		if ( NULL == pLand )
			return false;

		pLand->AddMobSchManLayer( _dwLayerIndex, false );

		return true;
	}


	const bool IInstanceFieldMode::DelMobLayer (
		const SNATIVEID& _sMapID,
		const DWORD _dwLayerIndex )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLLandMan* pLand = m_pLandManager->getLand( _sMapID );
		if ( NULL == pLand )
			return false;

		pLand->DelMobSchManOnceLayer( _dwLayerIndex );

		return true;
	}


	const bool IInstanceFieldMode::DelAllMobLayer (
		const SNATIVEID& _sMapID )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLLandMan* pLand = m_pLandManager->getLand( _sMapID );
		if ( NULL == pLand )
			return false;

		pLand->DelAllMobSchManOnceLayer();

		return true;
	}


	const bool IInstanceFieldMode::RegistActorTrigger (
		const Faction::GLActorID& _sActorID,
		const DWORD _dwTriggerType,
		const float _dwTriggerRange )
	{
		if ( NULL == m_pManagerField )
			return false;

		return m_pManagerField->reserveRegistActorTrigger(
			m_sMapID.getGaeaMapID().Mid(),
			_sActorID,
			_dwTriggerType,
			_dwTriggerRange );
	}


	const bool IInstanceFieldMode::RegistActorZoneTrigger (
		const Faction::GLActorID& _sActorID,
		const DWORD _dwTriggerType,
		const D3DXVECTOR3& _vPosition )
	{
		if ( NULL == m_pManagerField )
			return false;

		return m_pManagerField->reserveRegistActorTrigger(
			m_sMapID.getGaeaMapID().Mid(),
			_sActorID,
			_dwTriggerType,
			_vPosition.x,
			_vPosition.z,
			100.0f );
	}


	const bool IInstanceFieldMode::RemoveActorTrigger (
		const Faction::GLActorID& _sActorID )
	{
		if ( NULL == m_pManagerField )
			return false;

		return m_pManagerField->reserveRemoveActorTrigger(
			m_sMapID.getGaeaMapID().Mid(),
			_sActorID );
	}


	const DWORD IInstanceFieldMode::DropCrow (
		const SNATIVEID& _sMapID,
		const SNATIVEID& _sCrowID,
		const D3DXVECTOR3& _vPosition,
		const float _fRotate,
		const DWORD _dwLayerIndex )
	{
		if ( NULL == m_pLandManager )
			return GAEAID_NULL;

		DWORD dwGaeaID = GAEAID_NULL;
		GLLandMan* pLand = m_pLandManager->getLand( _sMapID );
		if ( pLand )
		{
			dwGaeaID = pLand->DropCrow(
				_sCrowID,
				_vPosition.x,
				_vPosition.z,
				_fRotate,
				_dwLayerIndex );
		}

		return dwGaeaID;
	}


	const bool IInstanceFieldMode::ApplySkill (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sSkillID,
		const DWORD _dwSkillLevel,
		const float _fSkillTime,
		const float _fSkillLifeTime,
		const DWORD _dwSkillFactType )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		STARGETID sTargetID( CROW_NULL, GAEAID_NULL );
		SSKILLFACT::STIME sTime( _fSkillTime );
		sTime.fLIFE = ( _fSkillLifeTime <= 0.f ) ? _fSkillTime : _fSkillLifeTime;
		DWORD dwSlot = 0;
		if ( false == pActor->RECEIVE_SKILLFACT(
			_sSkillID,
			_dwSkillLevel,
			dwSlot,
			sTargetID,
			sTime,
			_SKILLFACT_DEFAULTCUSTOMCOUNT,
			static_cast< const EMSKILLFACT_TYPE >( _dwSkillFactType ) ) )
		{
			return false;
		}

		if ( UINT_MAX == dwSlot )
		{
			return false;
		}

		GLMSG::SNETPC_SKILLHOLD_BRD sNetMsgBRD;
		sNetMsgBRD.emCrow = pActor->GetCrow();
		sNetMsgBRD.dwID = pActor->GetGaeaID();
		sNetMsgBRD.skill_id = _sSkillID;
		sNetMsgBRD.wLEVEL = _dwSkillLevel;
		sNetMsgBRD.wSELSLOT = static_cast< WORD >( dwSlot );
		sNetMsgBRD.sTIME = sTime;
		sNetMsgBRD.sID = sTargetID;

		// 스킬대상과 주변 클라이언트들에게 메세지 전송;
		pActor->SendMsgViewAround( &sNetMsgBRD );

		if ( CROW_PC == pActor->GetCrow() )
		{
			// 유저의 경우 자신에게도 메세지 전송;
			pActor->SendToClient( &sNetMsgBRD );
		}

		return true;
	}


	const bool IInstanceFieldMode::ApplySkill_Faction (
		const Faction::FactionID& _sFactionID,
		const SNATIVEID& _sSkillID,
		const DWORD _dwSkillLevel,
		const float _fSkillTime,
		const DWORD _dwSkillFactType )
	{
		if ( NULL == m_pLandManager )
			return false;

		Faction::ManagerField* const pFactionManager = m_pLandManager->GetFactionManager();
		if ( NULL == pFactionManager )
			return false;

		const Faction::FactionMemberVector* vecFactionMember =
			pFactionManager->getFactionMembers( _sFactionID );
		if ( NULL == vecFactionMember )
			return false;

		if ( vecFactionMember->empty() )
			return false;

		Faction::FactionMemberVectorCIter bIter = vecFactionMember->begin();
		Faction::FactionMemberVectorCIter eIter = vecFactionMember->end();
		for ( ; bIter != eIter; bIter++ )
		{
			ServerActor* pActor = NULL;
			switch( bIter->actorType )
			{
			case CROW_PC:
				{
					GLChar* const pChar = m_pServer->GetCharByDbNum( bIter->actorID_Num );
					if ( NULL == pChar )
						break;

					pActor = pChar;
				}
				break;

			case CROW_MOB:
			case CROW_NPC:
			case CROW_MATERIAL:
				{
					GLCrow* const pCrow = m_pLandManager->getCrow( bIter->actorID_Num );
					if ( NULL == pCrow ) 
						break;

					pActor = pCrow;
				}
				break;
			}

			if ( NULL == pActor )
				return false;

			STARGETID sTargetID( CROW_NULL, GAEAID_NULL );
			DWORD dwSlot = 0;
			if ( false == pActor->RECEIVE_SKILLFACT(
				_sSkillID,
				_dwSkillLevel,
				dwSlot,
				sTargetID,
				_fSkillTime,
				_SKILLFACT_DEFAULTCUSTOMCOUNT,
				static_cast< const EMSKILLFACT_TYPE >( _dwSkillFactType ) ) )
			{
				return false;
			}

			if ( UINT_MAX == dwSlot )
			{
				return false;
			}

			GLMSG::SNETPC_SKILLHOLD_BRD sNetMsgBRD;
			sNetMsgBRD.emCrow = pActor->GetCrow();
			sNetMsgBRD.dwID = pActor->GetGaeaID();
			sNetMsgBRD.skill_id = _sSkillID;
			sNetMsgBRD.wLEVEL = _dwSkillLevel;
			sNetMsgBRD.wSELSLOT = static_cast< WORD >( dwSlot );
			sNetMsgBRD.sTIME = _fSkillTime;
			sNetMsgBRD.sID = sTargetID;

			// 스킬대상과 주변 클라이언트들에게 메세지 전송;
			pActor->SendMsgViewAround( &sNetMsgBRD );

			switch( pActor->GetCrow() )
			{
			case CROW_PC:
				{
					// 유저의 경우 자신에게도 메세지 전송;
					pActor->SendToClient( &sNetMsgBRD );
				}
				break;
			}
		}

		return true;
	}


	const bool IInstanceFieldMode::IsHaveSkillFact (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sSkillID )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		WORD wOutSlot = 0;
		switch( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* pChar = (GLChar*)pActor;
				return pChar->IS_HAVE_SKILLFACT( _sSkillID, wOutSlot );
			}
			break;
		case CROW_NPC:
		case CROW_MOB:
			{
				GLCrow* pCrow = (GLCrow*)pActor;
				return pCrow->IS_HAVE_SKILLFACT( _sSkillID, wOutSlot ); 
			}
			break;
		}

		return false;
	}


	const bool IInstanceFieldMode::RemoveSkillFact (
		const Faction::GLActorID& _sActorID,
		const SNATIVEID& _sSkillID )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		return pActor->REMOVE_SKILLFACT( _sSkillID );
	}


	const bool IInstanceFieldMode::RemoveSkillFact (
		const Faction::GLActorID& _sActorID,
		const EMSPEC_ADDON _emSkillEffect )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		return pActor->REMOVE_SKILLFACT( _emSkillEffect );
	}


	const bool IInstanceFieldMode::SetMotion ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMotionID, const float _fMotionTime )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		switch( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* const pChar = (GLChar*)pActor;

				const float fMotionTime = ( _fMotionTime == 0.f ) ? FLT_MAX : _fMotionTime;
				pChar->SetMotion(
					static_cast< EMANI_MAINTYPE >( _sMotionID.Mid() ),
					static_cast< EMANI_SUBTYPE >( _sMotionID.Sid() ),
					fMotionTime,
					boost::bind( boost::mem_fn( &GLChar::EventMotionFinish ), pChar ) );

				GLMSG::NET_INSTANCE_SET_MOTION_BRD sNetMsgBrd(
					pChar->GetGaeaID(),
					_sMotionID.Mid(),
					_sMotionID.Sid(),
					fMotionTime );
				pChar->SendMsgViewAround( &sNetMsgBrd );

				m_pServer->SENDTOCLIENT( pChar->GETCLIENTID(), &sNetMsgBrd );
			}
			break;

		case CROW_MOB:
		case CROW_NPC:
		case CROW_MATERIAL:
			break;

		default:
			break;
		}

		return true;
	}


	const bool IInstanceFieldMode::IsMotion ( const Faction::GLActorID& _sActorID, const SNATIVEID& _sMotionID )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		switch( pActor->GetCrow() )
		{
		case CROW_PC:
			{
				GLChar* const pChar = (GLChar*)pActor;

				return pChar->IsMotion(
					static_cast< EMANI_MAINTYPE >( _sMotionID.Mid() ),
					static_cast< EMANI_SUBTYPE >( _sMotionID.Sid() ) );
			}
			break;

		case CROW_MOB:
		case CROW_NPC:
		case CROW_MATERIAL:
			break;

		default:
			break;
		}

		return false;
	}


	const bool IInstanceFieldMode::ResetMotion ( const Faction::GLActorID& _sActorID )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		if ( pActor->isAction( GLAT_MOTION ) )
		{
			pActor->TurnAction( GLAT_IDLE );

			GLMSG::SNET_ACTION_BRD sNetMsgBrd;
			sNetMsgBrd.emCrow = pActor->GetCrow();
			sNetMsgBrd.dwID = pActor->GetGaeaID();
			sNetMsgBrd.emAction	= GLAT_IDLE;
			pActor->SendMsgViewAround( &sNetMsgBrd );

			if( pActor->GetCrow() == CROW_PC )
			{
				m_pServer->SENDTOCLIENT(
					pActor->GETCLIENTID(),
					&sNetMsgBrd );
			}
		}

		return true;
	}


	const bool IInstanceFieldMode::AddExp (
		const DWORD _dwChaDBNum,
		const float _fExp,
		const bool _bNotify )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// 주의! 이벤트와 경험치 증감효과에 영향을 받는다;
		pChar->ReceiveExpApply( _fExp, _bNotify );

		return true;
	}


	const bool IInstanceFieldMode::SendSystemMail (
		const DWORD _dwChaDBNum,
		const std::string& _strTitle,
		const std::string& _strContents,
		const LONGLONG _llAttatchedMoney,
		const SNATIVEID& _sAttatchedItem,
		const DWORD _dwAttatchedItemCount,
		const DWORD _dwContentsTextIndex )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;
            
		ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
			ProvideRewardSystem::GLProvideRewardManager::Instance();
		if ( NULL == pProvideRewardMan )
			return false;

		bool bResult = true;
		if ( _sAttatchedItem.IsValidNativeID() )
		{
			ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
			if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			{
				provideRewardData.SetCharacter( pChar->GetCharID() );
				provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN );
				provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY_AND_ITEM );
				provideRewardData.SetRewardValue( _llAttatchedMoney, _sAttatchedItem.Mid(), _sAttatchedItem.Sid(), _dwAttatchedItemCount );
				provideRewardData.SetTextIndex( _dwContentsTextIndex );
			}
			else
			{	    
				SITEMCUSTOM newItem;
				m_pServer->NewItemMake(newItem, _sAttatchedItem, EMGEN_SYSTEM);
				if ( newItem.GetNativeID() != SNATIVEID(false) )
				{
					const SITEM* pITEM = GLogicData::GetInstance().GetItem( newItem.GetNativeID() );
					if ( pITEM == NULL )
						return 0;

					const int maxPileNum = pITEM->sDrugOp.GetPileNum();
					newItem.TurnNumSet( min(maxPileNum, (int)(_dwAttatchedItemCount ? _dwAttatchedItemCount : 1)) );

					newItem.GenerateBasicStat( false );
					newItem.GenerateLinkSkill();
					newItem.GenerateAddOption();

					m_pServer->GetItemLimit()->LogItemBasicStatOption( newItem );
					m_pServer->GetItemLimit()->LogItemLinkSkillOption( newItem );
					m_pServer->GetItemLimit()->LogItemAddonOption( newItem );

					if ( newItem.GENERATE_RANDOM_OPT( false ) )
					{
						GLItemLimit* pItemLimit = m_pServer->GetItemLimit();
						pItemLimit->LogItemRandomOption(newItem);
					}
				}

					// 우편을 보냄;
				bResult = m_pServer->PostSendSystemMail(
					pChar->GetCharID(),
					std::string(pChar->m_szName),
					newItem,
					_llAttatchedMoney,
					ATTENDANCE_REWARD_POST_DESIGN,
					ATTENDANCE_REWARD_POST_SENDER,
					std::string( ID2SERVERTEXT("SYSTEM_POST_SENDER_NAME") ),
					std::string( _strTitle ),
					std::string( _strContents )
					);
			}

			pProvideRewardMan->ProvideReward( provideRewardData );
		}
		else
		{
			ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
			if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			{
				provideRewardData.SetCharacter( pChar->GetCharID() );
				provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_LOGIN );
				provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY );
				provideRewardData.SetRewardValue( _llAttatchedMoney );
				provideRewardData.SetTextIndex( 0 );
			}
			else
			{
				provideRewardData.SetCharacter( pChar->GetCharID() );
				provideRewardData.SetProvideType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMPROVIDE_TYPE_IMMEDIATELY );
				provideRewardData.SetRewardType( ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE_MONEY );
				provideRewardData.SetRewardValue( _llAttatchedMoney );
				provideRewardData.SetTextIndex( 0 );
			}
		}  
		
		return bResult;
	}


	const bool IInstanceFieldMode::SendSystemMailFaction (
		const Faction::FactionID& _sFactionID,
		const std::string& _strTitle,
		const std::string& _strContents,
		const LONGLONG _llAttatchedMoney,
		const SNATIVEID& _sAttatchedItem,
		const DWORD _dwAttatchedItemCount,
		const DWORD _dwContentsTextIndex )
	{
		const Faction::FactionMemberVector* vecFactionMember = GetFactionList( _sFactionID );
		if ( NULL == vecFactionMember )
			return false;

		if ( 0 == vecFactionMember->size() )
			return true;

		Faction::FactionMemberVectorCIter iter = vecFactionMember->begin();
		Faction::FactionMemberVectorCIter eIter = vecFactionMember->end();
		for ( int i = 1; iter != eIter; iter++, i++ )
		{
			const Faction::GLActorID sActorID = *iter;
			if ( CROW_PC != sActorID.actorType )
				continue;

			if ( false == SendSystemMail(
				sActorID.actorID_Num,
				_strTitle,
				_strContents,
				_llAttatchedMoney,
				_sAttatchedItem,
				_dwAttatchedItemCount,
				_dwContentsTextIndex ) )
			{
				continue;
			}
		}

		return true;
	}


	const bool IInstanceFieldMode::DoEffectPosition (
		const DWORD _dwChaDBNum,
		const std::string& _strEffect,
		const SNATIVEID& _sMapID,
		const D3DXVECTOR3& _vPosition )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		// 현재 해당 맵에 있지 않으면 이펙트 뿌리지 않음;
		if ( pChar->GetCurrentMap().getBaseMapID() != _sMapID )
			return false;

		GLMSG::NET_INSTANCE_EFFECT sNetMsg;
		sNetMsg._strEffectName = _strEffect;
		sNetMsg._fEffectZonePosX = _vPosition.x;
		sNetMsg._fEffectZonePosY = _vPosition.y;
		sNetMsg._fEffectZonePosZ = _vPosition.z;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pServer->SENDTOCLIENT( 
			pChar->GETCLIENTID(),
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::DoEffectPositionAll (
		const std::string& _strEffect,
		const SNATIVEID& _sMapID,
		const D3DXVECTOR3& _vPosition )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLLandMan* pLand = m_pLandManager->getLand( _sMapID );
		if ( NULL == pLand )
			return false;

		GLMSG::NET_INSTANCE_EFFECT sNetMsg;
		sNetMsg._strEffectName = _strEffect;
		sNetMsg._fEffectZonePosX = _vPosition.x;
		sNetMsg._fEffectZonePosY = _vPosition.y;
		sNetMsg._fEffectZonePosZ = _vPosition.z;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		pLand->SendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::DoEffectActor (
		const DWORD _dwChaDBNum,
		const std::string& _strEffect,
		const Faction::GLActorID& _sActorID )
	{
		if ( NULL == m_pServer )
			return false;

		if ( NULL == m_pLandManager )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		GLMSG::NET_INSTANCE_EFFECT sNetMsg;
		sNetMsg._strEffectName = _strEffect;
		sNetMsg._targetActorType = pActor->GetCrow();
		sNetMsg._targetActorID= pActor->GetGaeaID();

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		gpGaeaServer->SENDTOCLIENT(
			pChar->GETCLIENTID(),
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::DoEffectActorAll (
		const std::string& _strEffect,
		const Faction::GLActorID& _sActorID )
	{
		if ( NULL == m_pLandManager )
			return false;

		ServerActor* pActor = m_pLandManager->getActor( _sActorID );
		if ( NULL == pActor )
			return false;

		GLMSG::NET_INSTANCE_EFFECT sNetMsg;
		sNetMsg._strEffectName = _strEffect;
		sNetMsg._targetActorType = pActor->GetCrow();
		sNetMsg._targetActorID= pActor->GetGaeaID();

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		m_pLandManager->sendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}

	const bool IInstanceFieldMode::DoOut ( const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_REQUEST_OUT sNetMsg( m_sMapID.getGaeaMapID().Mid(), _dwChaDBNum );
		m_pServer->SENDTOAGENT( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::DoJoin ( const DWORD _dwChaDBNum )
	{
		if ( NULL == m_pServer )
			return false;

		const InformationPosition sInformationPosition(
			Faction::GLActorID( CROW_PC, _dwChaDBNum ),
			m_sMapID.getGaeaMapID(),
			Faction::GLActorID(), 
			0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE );

		GLMSG::NET_INSTANCE_REQUEST_JOIN sNetMsg(
			sInformationPosition,
			InstanceSystem::EMAUTHORITY_NORMAL_FORCED );
		m_pServer->SENDTOAGENT( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::DoMoveToPosition (
		const Faction::GLActorID& _sActorID,
		const D3DXVECTOR3& _vPosition )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_DO_MOVE_TO sNetMsg(
			m_sMapID.getGaeaMapID().Mid(),
			_sActorID,
			Faction::GLActorID(),
			_vPosition);
		m_pServer->SENDTOMYSELF( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::DoMoveToObjectPosition (
		const Faction::GLActorID& _sActorID,
		const Faction::GLActorID& _sTargetActorID )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_DO_MOVE_TO sNetMsg(
			m_sMapID.getGaeaMapID().Mid(),
			_sActorID,
			_sTargetActorID );
		m_pServer->SENDTOMYSELF( &sNetMsg );

		return true;
	}


	const bool IInstanceFieldMode::DoDestroy ()
	{
		if ( NULL == m_pManagerField )
			return false;

		m_pManagerField->requestDestroy( m_sMapID.getGaeaMapID().Mid() );

		return true;
	}


	const bool IInstanceFieldMode::SendCustomMessage (
		const DWORD _dwDestType,
		const DWORD _DwDestID,
		const DWORD _dwParam1,
		const DWORD _dwParam2,
		const DWORD _dwParam3,
		const DWORD _dwParam4 )
	{
		if ( NULL == m_pServer )
			return false;

		GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE sNetMsg( _dwParam1, _dwParam2, _dwParam3, _dwParam4 );
		sNetMsg.emSrcType = EM_MESSAGE_FIELD_INSTANCE;
		sNetMsg.dwSrcID = m_sMapID.getGaeaMapID().Mid();
		sNetMsg.emDestType = _dwDestType;
		sNetMsg.dwDestID = _DwDestID;

		switch ( _dwDestType )
		{
		case EM_MESSAGE_USER:
			{
				GLChar* pChar = m_pServer->GetCharByDbNum( _DwDestID );
				if ( pChar )
				{
					m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );
				}
				else
				{
					m_pServer->SENDTOAGENT( &sNetMsg );
				}
			}
			break;

		//case EM_MESSAGE_MY_INSTANCE:    // sNetMsg.dwDestID 의미없음;
		case EM_MESSAGE_FIELD_BASE:
			{
				sc::LuaInstance* const _pLuaInstance( InstanceSystem::getScript( m_sMapID.getBaseMapID() ) );

				if ( _pLuaInstance->PushFunctionEx( "luaFieldEventCustomMessage" ) )
				{
					_pLuaInstance->PushInteger( EM_MESSAGE_FIELD_INSTANCE );
					_pLuaInstance->PushInteger( m_sMapID.getGaeaMapID().Mid() );
					_pLuaInstance->PushInteger( _dwParam1 );
					_pLuaInstance->PushInteger( _dwParam2 );
					_pLuaInstance->PushInteger( _dwParam3 );
					_pLuaInstance->PushInteger( _dwParam4 );
					_pLuaInstance->CallFunction( 6, 0 );
				}
			}
			break;

		case EM_MESSAGE_FIELD_INSTANCE:
		case EM_MESSAGE_AGENT_BASE:
		case EM_MESSAGE_AGENT_INSTANCE:
			{
				m_pServer->SENDTOAGENT( &sNetMsg );
			}
			break;
		}

		return true;
	}


	const bool IInstanceFieldMode::CallbackFunction (
		const std::string& _strFunctionName,
		const VEC_CALLBACK_PARAMETER& _vecParameter )
	{
		if ( NULL == m_pLua )
			return false;

		if ( false == m_pLua->IsExist( _strFunctionName.c_str() ) )
			return false;

		// 함수 등록;
		m_pLua->PushFunction( _strFunctionName.c_str() );

		// Parameter 등록;
		for ( VEC_CALLBACK_PARAMETER_CITER iter = _vecParameter.begin();
			iter != _vecParameter.end(); ++iter )
		{
			const SCALLBACK_PARAMETER& sParameter = *iter;
			switch ( sParameter.emType )
			{
			case SCALLBACK_PARAMETER::EMCALLBACK_PARAMETER_TYPE_INTEGER:
				m_pLua->PushInteger( sParameter.nValue );
				break;

			case SCALLBACK_PARAMETER::EMCALLBACK_PARAMETER_TYPE_NUMBER:
				m_pLua->PushNumber( sParameter.fValue );
				break;
			}
		}

		// 함수 Call;
		return m_pLua->CallFunction( _vecParameter.size(), 0 );
	}


	const bool IInstanceFieldMode::PrintChatMsg (
		const DWORD _dwChaDBNum,
		const std::string& _strText,
		const DWORD _dwTextColor )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLMSG::NET_INSTANCE_UI_CHATMSG sNetMsg;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pServer->SENDTOCLIENT(
			pChar->GETCLIENTID(),
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintChatXMLMsg (
		const DWORD _dwChaDBNum,
		const InstanceSystem::InstanceXmlString& _sXMLString,
		const DWORD _dwTextColor )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
		sNetMsg.xmlString = _sXMLString;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pServer->SENDTOCLIENT(
			pChar->GETCLIENTID(),
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintChatMsgFaction (
		const Faction::FactionID& _sFactionID,
		const std::string& _strText,
		const DWORD _dwTextColor,
		const bool _bAlly )
	{
		if ( NULL == m_pLandManager )
			return false;

		Faction::ManagerField* const pFactionMan = m_pLandManager->GetFactionManager();
		if ( NULL == pFactionMan )
			return false;

		GLMSG::NET_INSTANCE_UI_CHATMSG sNetMsg;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		if ( _bAlly )
		{
			pFactionMan->sendMessageToAllyFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}
		else
		{
			pFactionMan->sendMessageToFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}

		return true;
	}


	const bool IInstanceFieldMode::PrintChatXMLMsgFaction (
		const Faction::FactionID& _sFactionID,
		const InstanceSystem::InstanceXmlString& _sXMLString,
		const DWORD _dwTextColor,
		const bool _bAlly )
	{
		if ( NULL == m_pLandManager )
			return false;

		Faction::ManagerField* const pFactionMan = m_pLandManager->GetFactionManager();
		if ( NULL == pFactionMan )
			return false;

		GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
		sNetMsg.xmlString = _sXMLString;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		if ( _bAlly )
		{
			pFactionMan->sendMessageToAllyFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}
		else
		{
			pFactionMan->sendMessageToFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}

		return true;
	}


	const bool IInstanceFieldMode::PrintChatMsgAll (
		const std::string& _strText,
		const DWORD _dwTextColor )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLMSG::NET_INSTANCE_UI_CHATMSG sNetMsg;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pLandManager->sendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintChatXMLMsgAll (
		const InstanceSystem::InstanceXmlString& _sXMLString,
		const DWORD _dwTextColor )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
		sNetMsg.xmlString = _sXMLString;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pLandManager->sendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintSimpleMsg (
		const DWORD _dwChaDBNum,
		const float _fTime,
		const std::string& _strText,
		const DWORD _dwTextColor,
		const DWORD _dwFontSize )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLMSG::NET_INSTANCE_UI_SIMPLE_MSG sNetMsg;
		sNetMsg.fShowTime = _fTime;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;
		sNetMsg.dwFontSize = _dwFontSize;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pServer->SENDTOCLIENT(
			pChar->GETCLIENTID(),
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintSimpleMsgFaction (
		const Faction::FactionID& _sFactionID,
		const float _fTime,
		const std::string& _strText,
		const DWORD _dwTextColor,
		const DWORD _dwFontSize,
		const bool _bAlly )
	{
		if ( NULL == m_pLandManager )
			return false;

		Faction::ManagerField* const pFactionMan = m_pLandManager->GetFactionManager();
		if ( NULL == pFactionMan )
			return false;

		GLMSG::NET_INSTANCE_UI_SIMPLE_MSG sNetMsg;
		sNetMsg.fShowTime = _fTime;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;
		sNetMsg.dwFontSize = _dwFontSize;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		if ( _bAlly )
		{
			pFactionMan->sendMessageToAllyFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}
		else
		{
			pFactionMan->sendMessageToFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}

		return true;
	}


	const bool IInstanceFieldMode::PrintSimpleMsgAll (
		const float _fTime,
		const std::string& _strText,
		const DWORD _dwTextColor,
		const DWORD _dwFontSize )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLMSG::NET_INSTANCE_UI_SIMPLE_MSG sNetMsg;
		sNetMsg.fShowTime = _fTime;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;
		sNetMsg.dwFontSize = _dwFontSize;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pLandManager->sendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintSimpleXMLMsgAll (
		const float _fTime,
		const InstanceSystem::InstanceXmlString& _sXMLString,
		const DWORD _dwTextColor,
		const DWORD _dwFontSize )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLMSG::NET_INSTANCE_UI_SIMPLE_XMLMSG sNetMsg;
		sNetMsg.fShowTime = _fTime;
		sNetMsg.xmlString = _sXMLString;
		sNetMsg.dwColor = _dwTextColor;
		sNetMsg.dwFontSize = _dwFontSize;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pLandManager->sendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintTextDlgMsg (
		const DWORD _dwChaDBNum,
		const std::string& _strText,
		const DWORD _dwTextColor )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* const pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG sNetMsg;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pServer->SENDTOCLIENT(
			pChar->GETCLIENTID(),
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::PrintTextDlgMsgFaction (
		const Faction::FactionID& _sFactionID,
		const std::string& _strText,
		const DWORD _dwTextColor,
		const bool _bAlly )
	{
		if ( NULL == m_pLandManager )
			return false;

		Faction::ManagerField* const pFactionMan = m_pLandManager->GetFactionManager();
		if ( NULL == pFactionMan )
			return false;

		GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG sNetMsg;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );

		if ( _bAlly )
		{
			pFactionMan->sendMessageToAllyFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}
		else
		{
			pFactionMan->sendMessageToFaction(
				_sFactionID,
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );
		}

		return true;
	}


	const bool IInstanceFieldMode::PrintTextDlgMsgAll (
		const std::string& _strText,
		const DWORD _dwTextColor )
	{
		if ( NULL == m_pLandManager )
			return false;

		GLMSG::NET_INSTANCE_UI_TEXT_DLG_MSG sNetMsg;
		sNetMsg.strText = _strText;
		sNetMsg.dwColor = _dwTextColor;

		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, sNetMsg );
		m_pLandManager->sendMsgPC(
			NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
			SendBuffer );

		return true;
	}


	const bool IInstanceFieldMode::UI_Auth ( const DWORD _dwChaDBNum, const bool _bVisible, const float _fTime )
	{
		if ( NULL == m_pServer )
			return false;

		GLChar* pChar = m_pServer->GetCharByDbNum( _dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLMSG::NET_INSTANCE_CTI_UI_AUTH_FC sNetMsg;
		sNetMsg.bVisible = _bVisible;
		sNetMsg.fAuthTime = _fTime;

		m_pServer->SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );
	}

}