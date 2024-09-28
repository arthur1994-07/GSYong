#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../GLGaeaClient.h"
#include "../../ClientActor.h"
#include "../../Char/GLCharacter.h"
#include "../../Char/GLCharClient.h"
#include "FactionManagerClient.h"

namespace Faction
{

	ManagerClient::ManagerClient(GLGaeaClient* _pGaeaClient)
		: Manager ()
		, m_pGaeaClient(_pGaeaClient)
		, m_bFactionInfo( false )
	{
	}

	ManagerClient::~ManagerClient()
	{

	}

    void CALLBACK ManagerClient::messageProcedure(const NET_MSG_GENERIC* const _pMessage)
    {
		const GLMSG::NET_FACTION_MESSAGE* const _pFactionMessage((const GLMSG::NET_FACTION_MESSAGE* const)_pMessage);
        switch(_pFactionMessage->emType)
        {
        case EMMESSAGE_FACTION_INFO_FC:
            ManagerClient:: _messageFactionInformation(_pMessage);
            break;
        case EMMESSAGE_FACTION_CHANGE_FCB:
			ManagerClient::_messageFactionChange(_pMessage);
            break;
        case EMMESSAGE_FACTION_RELATION_INFO_FC:
            ManagerClient::_messageRelationInformation(_pMessage);
            break;
        case EMMESSAGE_FACTION_RELATION_CHANGE_FCB:
            ManagerClient::_messageRelationChange(_pMessage);
            break;
        case EMMESSAGE_FACTION_HP_VISIBLE_INFO_FC:
            ManagerClient::_messageHpVisible(_pMessage);
            break;
        }
    }

    void CALLBACK ManagerClient::_messageFactionInformation(const NET_MSG_GENERIC* const nmg)
    {
        const GLMSG::NET_FACTION_INFO* const _pNetMsg((const GLMSG::NET_FACTION_INFO* const)nmg);

		if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		{			
#ifdef FACTION_LOG_DETAIL
			if ( m_bFactionInfo )
			{
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, "---- Synchronize Faction Information ----" );
			}
#endif
		}

        const FactionID _factionID(_pNetMsg->facionID);
        CreateFactionClient( _factionID );

        const DWORD dwActorSize = _pNetMsg->nActor;
        for ( DWORD i = 0; i < dwActorSize; ++i )
        {
            SetFactionClient( _pNetMsg->actorList[i], _factionID );
        }

		if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		{
#ifdef FACTION_LOG_DETAIL
			if ( m_bFactionInfo )
			{
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, "-----------------------------------------" );
			}
#endif
		}
    }

    void CALLBACK ManagerClient::_messageFactionChange(const NET_MSG_GENERIC* const nmg)
    {
        const GLMSG::NET_FACTION_CHANGE* const _pNetMsg((const GLMSG::NET_FACTION_CHANGE* const)nmg);

        SetFactionClient( _pNetMsg->actorID, _pNetMsg->facionID );	
    }

    void CALLBACK ManagerClient::_messageRelationInformation(const NET_MSG_GENERIC* const nmg)
    {
        const GLMSG::NET_FACTION_RELATION_INFO* const _pNetMsg((const GLMSG::NET_FACTION_RELATION_INFO* const)nmg);

		if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		{			
#ifdef FACTION_LOG_DETAIL
			if ( m_bFactionInfo )
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, "---- Synchronize Relation Information ----" );
#endif
		}

        const DWORD dwFactionSize = _pNetMsg->nFaction;
        for ( DWORD i = 0; i < dwFactionSize; ++i )
        {
			SetRelationClient(
				_pNetMsg->sourceFaction,
				_pNetMsg->targetFactionRelation[i].faction,
				(Faction::EMRELATION_TYPE)_pNetMsg->targetFactionRelation[i].relation );
        }

		if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		{
#ifdef FACTION_LOG_DETAIL
			if ( m_bFactionInfo )
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::AQUABLUE, "------------------------------------------" );
#endif
		}
    }

    void CALLBACK ManagerClient::_messageRelationChange(const NET_MSG_GENERIC* const nmg)
    {
        const GLMSG::NET_FACTION_RELATION_CHANGE* const _pNetMsg((const GLMSG::NET_FACTION_RELATION_CHANGE* const)nmg);

        switch ( _pNetMsg->emChangeType )
        {
        case GLMSG::NET_FACTION_RELATION_CHANGE::CHANGE_FACTION_TO_FACTION:
            SetRelationClient(
				_pNetMsg->sourceFaction,
				_pNetMsg->targetFaction,
				(Faction::EMRELATION_TYPE)_pNetMsg->factionRelation );
            break;
        case GLMSG::NET_FACTION_RELATION_CHANGE::CHANGE_FACTION_TO_ALL:
            SetRelationToAllClient(
				_pNetMsg->sourceFaction,
				(Faction::EMRELATION_TYPE)_pNetMsg->factionRelation );
            break;
        case GLMSG::NET_FACTION_RELATION_CHANGE::CHANGE_FACTION_FROM_ALL:
            SetRelationFromAllClient(
				_pNetMsg->sourceFaction,
				(Faction::EMRELATION_TYPE)_pNetMsg->factionRelation );
            break;
        }
    }

    void CALLBACK ManagerClient::_messageHpVisible(const NET_MSG_GENERIC* const nmg)
    {
        const GLMSG::NET_FACTION_HP_VISIBLE_INFO* const _pNetMsg((const GLMSG::NET_FACTION_HP_VISIBLE_INFO* const)nmg);

        Manager::setHPVisible((Faction::EMRELATION_FLAG_TYPE)_pNetMsg->emHpVisible);
    }

    void ManagerClient::SetMapID(const unsigned int _nMapID)
    {
        this->nMapID = _nMapID;
    }

    FactionID ManagerClient::getMyFaction()
    {
        GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
        GLActorID _myActorID(pCharacter->GetCrow(), pCharacter->CharDbNum());

		const FactionID* pFactionID = getFactionID( _myActorID );
		if ( pFactionID )
		{
			FactionID sFactionID(
				pFactionID->factionType,
				pFactionID->factionID_Num );
			return sFactionID;
		}

		return FactionID();
        //return getFactionID(_myActorID);
    }

    const EMRELATION_TYPE ManagerClient::getRelationClient(ClientActor* const pActor, ClientActor* const pTargetActor)
    {
		if ( pActor == NULL || pTargetActor == NULL )
			return EMRELATION_ERROR;

        const GLActorID _ActorID(pActor->GetCrow(), pActor->GetCharID());
        const GLActorID _TargetActorID(pTargetActor->GetCrow(), pTargetActor->GetCharID());
        return Manager::getRelation(_ActorID, _TargetActorID);
    }





	/// Client Wrapping Function;
	const bool ManagerClient::SetFactionClient (
		const GLActorID& _actorID,
		const FactionID& _factionID )
	{
		FactionID preFactionID;
		const EMFACTION_RETURN emReturn = Manager::setFaction( _actorID, _factionID, preFactionID );
		switch ( emReturn )
		{
		case EMFACTION_SUCCESS:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
				DWORD dwColor = NS_UITEXTCOLOR::LIGHTYELLOW;

				const EMRELATION_TYPE emRelationType =
					getRelation( getMyFaction(), _factionID );
				switch ( emRelationType )
				{
				case EMRELATION_ENEMY:
					dwColor = NS_UITEXTCOLOR::RED;
					break;

				case EMRELATION_NEUTRAL_ENEMY:
				case EMRELATION_NEUTRAL_ALLY:
					dwColor = NS_UITEXTCOLOR::GREEN;
					break;

				case EMRELATION_ALLY:
					dwColor = NS_UITEXTCOLOR::BLUE;
					break;

				case EMRELATION_ERROR:
					dwColor = NS_UITEXTCOLOR::DARKGRAY;
					break;
				}
			
				if ( m_bFactionInfo )
				{
					std::string strChaName;
					std::tr1::shared_ptr< GLCharClient > spChar =
						m_pGaeaClient->GetChar( _actorID.actorID_Num );
					if ( spChar )
					{
						strChaName = spChar->GetName();
					}

					m_pGaeaClient->PrintMsgText(
						dwColor,
						sc::string::format( "Succeed Set Faction ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						dwColor,
						sc::string::format( "-> ActorID : %1% ( %2% )",
						_actorID.actorID_Num,
						strChaName ) );

					m_pGaeaClient->PrintMsgText(
						dwColor,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );
				}
				
			}
			break;

		case EMFACTION_FAIL:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
				if ( m_bFactionInfo )
				{
					std::string strChaName;
					std::tr1::shared_ptr< GLCharClient > spChar =
						m_pGaeaClient->GetChar( _actorID.actorID_Num );
					if ( spChar )
					{
						strChaName = spChar->GetName();
					}

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "Failed Set Faction ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "-> ActorID : %1% ( %2% )",
						_actorID.actorID_Num,
						strChaName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );
				}
			}

			return false;
		}

		return true;
	}

	void ManagerClient::CreateFactionClient (
		const FactionID& _factionID )
	{
		const EMFACTION_RETURN emReturn = Manager::createFaction( _factionID );
		switch ( emReturn )
		{
		case EMFACTION_FAIL:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_PRIVATE,
						sc::string::format( "Failed Create Faction ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_PRIVATE,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );
				}
			}
			break;

		case EMFACTION_SUCCESS:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{		
#ifdef FACTION_LOG_DETAIL
				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_PRIVATE,
						sc::string::format( "Create Faction ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_PRIVATE,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );
				}
#endif
			}

			break;

		case EMFACTION_ALREADY:
			break;
		}	
	}

	const bool ManagerClient::SetRelationClient (
		const FactionID& _factionID,
		const FactionID& _targetFactionID,
		const EMRELATION_TYPE _emRelation )
	{
		const EMFACTION_RETURN emReturn =
			Manager::setRelation( _factionID, _targetFactionID, _emRelation );
		switch ( emReturn )
		{
		case EMFACTION_SUCCESS:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{				
#ifdef FACTION_LOG_DETAIL
				std::string strRelation;
				switch ( _emRelation )
				{
				case EMRELATION_ENEMY:
					strRelation = "Enemy";
					break;

				case EMRELATION_NEUTRAL_ENEMY:
					strRelation = "Neutral Enemy";
					break;

				case EMRELATION_NEUTRAL_ALLY:
					strRelation = "Neutral Ally";
					break;

				case EMRELATION_ALLY:
					strRelation = "Ally";
					break;

				case EMRELATION_ERROR:
					strRelation = "Error";
					break;
				}

				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "Succeed Set Relation ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "-> Base / FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "-> Target / FactionID : %1%, FactionType : %2%",
						_targetFactionID.factionID_Num,
						_targetFactionID.factionType ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "-> Relation : %1% ]",
						strRelation ) );
				}
#endif
			}
			break;

		case EMFACTION_FAIL:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "Failed Set Relation ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "-> Base / FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "-> Target / FactionID : %1%, FactionType : %2%",
						_targetFactionID.factionID_Num,
						_targetFactionID.factionType ) );
				}
			}

			return false;
		}

		return true;
	}

	const bool ManagerClient::SetRelationToAllClient (
		const FactionID& _factionID,
		const EMRELATION_TYPE _emRelation )
	{
		const EMFACTION_RETURN emReturn =
			Manager::setRelationToAll( _factionID, _emRelation );
		switch ( emReturn )
		{
		case EMFACTION_SUCCESS:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{				
#ifdef FACTION_LOG_DETAIL
				std::string strRelation;
				switch ( _emRelation )
				{
				case EMRELATION_ENEMY:
					strRelation = "Enemy";
					break;

				case EMRELATION_NEUTRAL_ENEMY:
					strRelation = "Neutral Enemy";
					break;

				case EMRELATION_NEUTRAL_ALLY:
					strRelation = "Neutral Ally";
					break;

				case EMRELATION_ALLY:
					strRelation = "Ally";
					break;

				case EMRELATION_ERROR:
					strRelation = "Error";
					break;
				}

				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "Succeed Set Relation To All ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "->  FactionID : %1%, FactionType : %2%]",
						_factionID.factionID_Num,
						_factionID.factionType ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "-> Relation : %1%",
						strRelation ) );
				}
#endif
			}
			break;

		case EMFACTION_FAIL:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "Failed Set Relation To All ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );
				}
			}

			return false;
		}

		return true;
	}

	const bool ManagerClient::SetRelationFromAllClient (
		const FactionID& _factionID,
		const EMRELATION_TYPE _emRelation )
	{
		const EMFACTION_RETURN emReturn =
			Manager::setRelationFromAll( _factionID, _emRelation );
		switch ( emReturn )
		{
		case EMFACTION_SUCCESS:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{				
#ifdef FACTION_LOG_DETAIL
				std::string strRelation;
				switch ( _emRelation )
				{
				case EMRELATION_ENEMY:
					strRelation = "Enemy";
					break;

				case EMRELATION_NEUTRAL_ENEMY:
					strRelation = "Neutral Enemy";
					break;

				case EMRELATION_NEUTRAL_ALLY:
					strRelation = "Neutral Ally";
					break;

				case EMRELATION_ALLY:
					strRelation = "Ally";
					break;

				case EMRELATION_ERROR:
					strRelation = "Error";
					break;
				}

				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "Succeed Set Relation From All ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::CHAT_SYSTEM,
						sc::string::format( "-> Relation : %1%",
						strRelation ) );
				}
#endif
			}
			break;

		case EMFACTION_FAIL:
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
				if ( m_bFactionInfo )
				{
					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "Failed Set Relation From All ( %1% )",
						m_strLandName ) );

					m_pGaeaClient->PrintMsgText(
						NS_UITEXTCOLOR::DARKGRAY,
						sc::string::format( "-> FactionID : %1%, FactionType : %2%",
						_factionID.factionID_Num,
						_factionID.factionType ) );
				}
			}

			return false;
		}

		return true;
	}
}