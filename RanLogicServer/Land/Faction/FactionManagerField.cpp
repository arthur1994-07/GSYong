#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "FactionManagerField.h"

namespace Faction
{
	ManagerField::ManagerField(GLGaeaServer* const _pGaeaServer, GLLandManager* const _pLandMananger)
		: Manager ()
		, m_pGaeaServer(_pGaeaServer)
        , m_pLandManager(_pLandMananger)
	{
	}

	ManagerField::~ManagerField()
	{
	}

    BOOL CALLBACK ManagerField::messageProcedure(const DWORD dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage)
    {
        /*GLMSG::NET_FACTION_MESSAGE* pNetMsg = (GLMSG::NET_FACTION_MESSAGE*) _pMessage;
        switch ( pNetMsg->emType )
        {
        default:
            return FALSE;
        }*/
        return TRUE;
    }

    const EMRELATION_TYPE ManagerField::getRelationField(ServerActor* const pActor, ServerActor* const pTargetActor)
    {
        if ( ( pActor == NULL ) || ( pTargetActor == NULL ) )
            return EMRELATION_ERROR;

        const GLActorID _ActorID(pActor->GetCrow(), pActor->GetCharID());
        const GLActorID _TargetActorID(pTargetActor->GetCrow(), pTargetActor->GetCharID());
        return Manager::getRelation(_ActorID, _TargetActorID);
    }

	const bool ManagerField::IsFactionMember ( const FactionID& factionID, ServerActor* const pActor )
	{
		// 팩션 체크;
		const GLFaction* glfaction = getFaction( factionID );
		if ( NULL == glfaction )
			return false;

		// 팩션 멤버 체크;
		const FactionMemberVector& memeberVector = glfaction->getMemberVector();
		FactionMemberVectorCIter _iter = memeberVector.begin();
		for ( ; _iter != memeberVector.end(); ++_iter )
		{
			if ( _iter->actorType != CROW_PC )
				continue;

			GLChar* pChar = m_pGaeaServer->GetCharByDbNum(
				_iter->actorID_Num );
			if ( NULL == pChar )
				continue;

			// 타겟 캐릭터 말고는 제외;
			if ( pChar->GetActorID() == pActor->GetActorID() )
				return true;
		}

		return false;
	}

    void ManagerField::sendFactionInformation(GLChar* const pChar) const
    {
        // 메시지에 Faction과 Actor의 정보를 묶어서 보낸다;
        FactionVector::const_iterator _factionIter = factionVector.begin();
        for ( ; _factionIter != factionVector.end(); _factionIter++)
        {
            GLMSG::NET_FACTION_INFO _factionInformationMessage;
            _factionInformationMessage.facionID = _factionIter->first;

            GLFaction* _pFaction = _factionIter->second;
            const FactionMemberVector& _memeberVector = _pFaction->getMemberVector();
            if ( _memeberVector.size() == 0 )
            {
                m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), &_factionInformationMessage);
            }
            else
            {
                FactionMemberVectorCIter _memberIter = _memeberVector.begin();
                for ( ; _memberIter != _memeberVector.end(); ++_memberIter )
                {
                    if ( _factionInformationMessage.addActor(*_memberIter) == false )
                    {
                        m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), &_factionInformationMessage);
                        _factionInformationMessage.reset();
                    }
                }
                if ( _factionInformationMessage.isValid() )
                    m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), &_factionInformationMessage);
            }
        }
    }

    void ManagerField::sendFactionInformation() const
    {
        GLMSG::NET_FACTION_INFO _factionInformationMessage;

        // 메시지에 Faction과 Actor의 정보를 묶어서 보낸다;
        FactionVector::const_iterator _factionIter = factionVector.begin();
        for ( ; _factionIter != factionVector.end(); _factionIter++)
        {
            GLMSG::NET_FACTION_INFO _factionInformationMessage;
            _factionInformationMessage.facionID = _factionIter->first;

            GLFaction* _pFaction = _factionIter->second;
            const FactionMemberVector& _memeberVector = _pFaction->getMemberVector();
            if ( _memeberVector.size() == 0 )
            {
                m_pLandManager->sendMsgPC(&_factionInformationMessage);
            }
            else
            {
                FactionMemberVectorCIter _memberIter = _memeberVector.begin();
                for ( ; _memberIter != _memeberVector.end(); ++_memberIter )
                {
                    if ( _factionInformationMessage.addActor(*_memberIter) == false )
                    {
                        m_pLandManager->sendMsgPC(&_factionInformationMessage);
                        _factionInformationMessage.reset();
                    }
                }
                if ( _factionInformationMessage.isValid() )
                    m_pLandManager->sendMsgPC(&_factionInformationMessage);
            }
        }
    }
	

    void ManagerField::sendRelationInformation(GLChar* const pChar) const
    {
        // 메시지에 Faction마다 Relation의 정보를 묶어서 보낸다;
        FactionVector::const_iterator _factionIter = factionVector.begin();
        for ( ; _factionIter != factionVector.end(); _factionIter++ )
        {
            GLMSG::NET_FACTION_RELATION_INFO _factionRelationInfoMessage(_factionIter->first);

            const GLFaction* _pFaction = _factionIter->second;

            const RelationVector& _relationVector  = _pFaction->getRelationVector();
            RelationVector::const_iterator _relationIter = _relationVector.begin();
            for ( ; _relationIter != _relationVector.end(); _relationIter++ )
            {
                if ( _factionRelationInfoMessage.addRelation(_relationIter->first, (EMRELATION_TYPE)_relationIter->second) == false )
                {
                    m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), &_factionRelationInfoMessage);
                    _factionRelationInfoMessage.reset();
                }
            }
            if ( _factionRelationInfoMessage.isValid() )
                m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), &_factionRelationInfoMessage);
        }
    }

    void ManagerField::sendRelationInformation() const
    {
        // 메시지에 Faction마다 Relation의 정보를 묶어서 보낸다;
        FactionVector::const_iterator _factionIter = factionVector.begin();
        for ( ; _factionIter != factionVector.end(); _factionIter++ )
        {
            GLMSG::NET_FACTION_RELATION_INFO _factionRelationInfoMessage(_factionIter->first);

            const GLFaction* _pFaction = _factionIter->second;

            const RelationVector& _relationVector  = _pFaction->getRelationVector();
            RelationVector::const_iterator _relationIter = _relationVector.begin();
            for ( ; _relationIter != _relationVector.end(); _relationIter++ )
            {
                if ( _factionRelationInfoMessage.addRelation(_relationIter->first, (EMRELATION_TYPE)_relationIter->second) == false )
                {
                    m_pLandManager->sendMsgPC(&_factionRelationInfoMessage);
                    _factionRelationInfoMessage.reset();
                }
            }
            if ( _factionRelationInfoMessage.isValid() )
                m_pLandManager->sendMsgPC(&_factionRelationInfoMessage);
        }
    }

	const bool ManagerField::SendToFactionPrivate (
		const Faction::GLActorID& sActorID,
		const Faction::GLActorID& sTargetActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg )
	{
		// 보낸 사람 체크;
		if ( sActorID.actorType != CROW_PC )
			return false;

		GLChar* pMyChar = m_pGaeaServer->GetCharByDbNum(
			sActorID.actorID_Num );
		if ( NULL == pMyChar )
			return false;

		// 타겟 체크;
		if ( sTargetActorID.actorType != CROW_PC )
			return false;

		GLChar* pTargetChar = m_pGaeaServer->GetCharByDbNum(
			sTargetActorID.actorID_Num );
		if ( NULL == pTargetChar )
			return false;

		// 팩션 체크;
		const GLFaction* glfaction = getFaction( factionID );
		if ( glfaction == NULL )
			return false;

		// 팩션 멤버 체크 후 메시지 전송;
		const FactionMemberVector& memeberVector = glfaction->getMemberVector();
		FactionMemberVectorCIter _iter = memeberVector.begin();
		for ( ; _iter != memeberVector.end(); ++_iter )
		{
			if ( _iter->actorType != CROW_PC )
				continue;

			GLChar* pChar = m_pGaeaServer->GetCharByDbNum(
				_iter->actorID_Num );
			if ( NULL == pChar )
				continue;

			// 타겟 캐릭터 말고는 제외;
			if ( pChar->GetActorID() != pTargetChar->GetActorID() )
				continue;

			m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
		}

		return true;
	}

	const bool ManagerField::SendToAllyFactionPrivate ( 
		const Faction::GLActorID& sActorID,
		const Faction::GLActorID& sTargetActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//SendToFactionPrivate( sActorID, sTargetActorID, factionID, nmg );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				SendToFactionPrivate( sActorID, sTargetActorID, tempFactionID, nmg );
			}
		}

		return true;
	}

	const bool ManagerField::SendToFactionViewAround (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg,
		bool bExcludeMe )
	{
		// 보낸 사람 체크;
		if ( sActorID.actorType != CROW_PC )
			return false;

		GLChar* pMyChar = m_pGaeaServer->GetCharByDbNum(
			sActorID.actorID_Num );
		if ( NULL == pMyChar )
			return false;

		// 팩션 체크;
		const GLFaction* glfaction = getFaction( factionID );
		if ( glfaction == NULL )
			return false;

		// 팩션 멤버 체크 후 메시지 전송;
		const FactionMemberVector& memeberVector = glfaction->getMemberVector();
		FactionMemberVectorCIter _iter = memeberVector.begin();
		for ( ; _iter != memeberVector.end(); ++_iter )
		{
			if ( _iter->actorType != CROW_PC )
				continue;

			GLChar* pChar = m_pGaeaServer->GetCharByDbNum(
				_iter->actorID_Num );
			if ( NULL == pChar )
				continue;

			// 나 자신 제외;
			if ( pChar->GetActorID() == sActorID && bExcludeMe )
				continue;

			// View Around 내에 있는지 체크;
			if ( false == pMyChar->CheckBelongViewAround( pChar ) )
				continue;

			m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
		}

		return true;
	}

	const bool ManagerField::SendToAllyFactionViewAround (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg,
		bool bExcludeMe )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//SendToFactionViewAround( sActorID, factionID, nmg );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				SendToFactionViewAround( sActorID, tempFactionID, nmg, bExcludeMe );
			}
		}

		return true;
	}

	const bool ManagerField::SendToFactionExcludeMe (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg )
	{
		// 보낸 사람 체크;
		if ( sActorID.actorType != CROW_PC )
			return false;

		GLChar* pMyChar = m_pGaeaServer->GetCharByDbNum(
			sActorID.actorID_Num );
		if ( NULL == pMyChar )
			return false;

		// 팩션 체크;
		const GLFaction* glfaction = getFaction( factionID );
		if ( glfaction == NULL )
			return false;

		// 팩션 멤버 체크 후 메시지 전송;
		const FactionMemberVector& memeberVector = glfaction->getMemberVector();
		FactionMemberVectorCIter _iter = memeberVector.begin();
		for ( ; _iter != memeberVector.end(); ++_iter )
		{
			if ( _iter->actorType != CROW_PC )
				continue;

			GLChar* pChar = m_pGaeaServer->GetCharByDbNum(
				_iter->actorID_Num );
			if ( pChar == NULL )
				continue;

			// 나 자신 제외;
			if ( pChar->GetActorID() == sActorID )
				continue;

			m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
		}

		return true;
	}

	const bool ManagerField::SendToAllyFactionExcludeMe (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//SendToFactionExcludeMe( sActorID, factionID, nmg );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				SendToFactionExcludeMe( sActorID, tempFactionID, nmg );
			}
		}

		return true;
	}

	const bool ManagerField::SendToFactionParty (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg,
		bool bExcludeMe )
	{
		// 보낸 사람 체크;
		if ( sActorID.actorType != CROW_PC )
			return false;

		GLChar* pMyChar = m_pGaeaServer->GetCharByDbNum(
			sActorID.actorID_Num );
		if ( NULL == pMyChar )
			return false;

		// 팩션 체크;
		const GLFaction* glfaction = getFaction( factionID );
		if ( glfaction == NULL )
			return false;

		// 팩션 멤버 체크 후 메시지 전송;
		const FactionMemberVector& memeberVector = glfaction->getMemberVector();
		FactionMemberVectorCIter _iter = memeberVector.begin();
		for ( ; _iter != memeberVector.end(); ++_iter )
		{
			if ( _iter->actorType != CROW_PC )
				continue;

			GLChar* pChar = m_pGaeaServer->GetCharByDbNum(
				_iter->actorID_Num );
			if ( pChar == NULL )
				continue;

			// 나 자신 제외;
			if ( pChar->GetActorID() == sActorID && bExcludeMe )
				continue;

			// 같은 파티아니면 제외;
			GLPartyID glPartyID = pChar->GetPartyID();
			GLPartyID glMyPartyID = pMyChar->GetPartyID();
			if ( glPartyID != glMyPartyID )
				continue;

			m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
		}

		return true;
	}

	const bool ManagerField::SendToAllyFactionParty (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg,
		bool bExcludeMe )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//SendToFactionParty( sActorID, factionID, nmg );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				SendToFactionParty( sActorID, tempFactionID, nmg, bExcludeMe );
			}
		}

		return true;
	}

	const bool ManagerField::SendToFactionClub (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg,
		bool bExcludeMe )
	{
		// 보낸 사람 체크;
		if ( sActorID.actorType != CROW_PC )
			return false;

		GLChar* pMyChar = m_pGaeaServer->GetCharByDbNum(
			sActorID.actorID_Num );
		if ( NULL == pMyChar )
			return false;

		// 팩션 체크;
		const GLFaction* glfaction = getFaction( factionID );
		if ( glfaction == NULL )
			return false;

		// 팩션 멤버 체크 후 메시지 전송;
		const FactionMemberVector& memeberVector = glfaction->getMemberVector();
		FactionMemberVectorCIter _iter = memeberVector.begin();
		for ( ; _iter != memeberVector.end(); ++_iter )
		{
			if ( _iter->actorType != CROW_PC )
				continue;

			GLChar* pChar = m_pGaeaServer->GetCharByDbNum(
				_iter->actorID_Num );
			if ( pChar == NULL )
				continue;

			// 나 자신 제외;
			if ( pChar->GetActorID() == sActorID && bExcludeMe )
				continue;

			// 같은 클럽아니면 제외;
			DWORD dwClubDbNum = pChar->GetClubDbNum();
			DWORD dwMyClubDbNum = pMyChar->GetClubDbNum();
			if ( dwClubDbNum != dwMyClubDbNum )
				continue;

			m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
		}

		return true;
	}

	const bool ManagerField::SendToAllyFactionClub (
		const Faction::GLActorID& sActorID,
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg,
		bool bExcludeMe )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//SendToFactionClub( sActorID, factionID, nmg );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				SendToFactionClub( sActorID, tempFactionID, nmg, bExcludeMe );
			}
		}

		return true;
	}

    const bool ManagerField::sendMessageToFaction(const FactionID& factionID, NET_MSG_GENERIC* nmg)
    {
        const GLFaction* _faction = getFaction(factionID);
		if ( _faction == NULL )
			return false;

        const FactionMemberVector& _memeberVector = _faction->getMemberVector();
        FactionMemberVectorCIter _iter = _memeberVector.begin();
        for ( ; _iter != _memeberVector.end(); ++_iter )
        {
            if ( _iter->actorType != CROW_PC )
                continue;

            GLChar* pChar = m_pGaeaServer->GetCharByDbNum(_iter->actorID_Num);
            if ( pChar == NULL )
                continue;

            m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), nmg);
        }
        return true;
    }

	const bool ManagerField::sendMessageToAllyFaction (
		const FactionID& factionID,
		NET_MSG_GENERIC* nmg )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//sendMessageToFaction( factionID, nmg );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				sendMessageToFaction( tempFactionID, nmg );
			}
		}

		return true;
	}

    const bool ManagerField::sendMessageToFaction(const FactionID& factionID, const EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, const bool bUserCrc/*=false*/)
    {
        const GLFaction* _faction = getFaction(factionID);
		if ( _faction == NULL )
			return false;

		// 진영의 멤버 검색;
        const FactionMemberVector& _memeberVector = _faction->getMemberVector();
        FactionMemberVectorCIter _iter = _memeberVector.begin();
        for ( ; _iter != _memeberVector.end(); ++_iter )
        {
            if ( _iter->actorType != CROW_PC )
                continue;

            GLChar* pChar = m_pGaeaServer->GetCharByDbNum(_iter->actorID_Num);
            if ( pChar == NULL )
                continue;

            m_pGaeaServer->SENDTOCLIENT(pChar->GETCLIENTID(), MsgType, Buffer, bUserCrc);
        }
        return true;
    }

	const bool ManagerField::sendMessageToAllyFaction (
		const FactionID& factionID,
		const EMNET_MSG MsgType,
		const msgpack::sbuffer& Buffer,
		const bool bUserCrc )
	{
		const GLFaction* pMyFaction = getFaction( factionID );
		if ( NULL == pMyFaction )
			return false;

		//sendMessageToFaction( factionID, MsgType, Buffer, bUserCrc );

		// 진영과 동맹관계의 진영 검색;
		const RelationVector& relationVector = pMyFaction->getRelationVector();
		RelationVectorCIter relationIter = relationVector.begin();
		for ( ; relationIter != relationVector.end(); ++relationIter )
		{
			EMRELATION_TYPE emRelationType = static_cast< EMRELATION_TYPE >(
				relationIter->second );
			if ( emRelationType == EMRELATION_ALLY )
			{
				FactionID tempFactionID = relationIter->first;
				sendMessageToFaction( tempFactionID, MsgType, Buffer, bUserCrc );
			}
		}

		return true;
	}
}
