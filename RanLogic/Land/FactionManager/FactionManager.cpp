#include "pch.h"

#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../GLUseFeatures.h"

#include "FactionManager.h"

namespace Faction
{

    GLFaction::GLFaction()
        : bAutoRemove(true)
    {
    }

    GLFaction::~GLFaction()
    {
    }

    const RelationVector& GLFaction::getRelationVector() const
    {
        return relationVector;
    }

    const FactionMemberVector& GLFaction::getMemberVector() const
    {
        return memberVector;
    }

    const bool GLFaction::isInActor(const GLActorID& _actorID) const
    {
        FactionMemberVector::const_iterator Iter = memberVector.begin();
        for ( ; Iter != memberVector.end(); Iter++ )
        {
            if ( *Iter == _actorID )
                return true;
        }
        return false;
    }

    const bool GLFaction::addActor(const GLActorID& _actorID)
    {
        if ( isInActor(_actorID) == false )
        {
            memberVector.push_back(_actorID);
            return true;
        }
        return false;
    }

    const bool GLFaction::removeActor(const GLActorID& _actorID)
    {
        FactionMemberVector::iterator Iter = memberVector.begin();
        for ( ; Iter != memberVector.end(); )
        {
            if ( *Iter == _actorID )
            {
                Iter = memberVector.erase(Iter);
                return true;
            }
            else
            {
                Iter++;
            }
        }
        return false;
    }

    const EMRELATION_TYPE GLFaction::getRelation(const FactionID& _targetFactionID) const
    {
        RelationVector::const_iterator Iter = relationVector.begin();
        for ( ; Iter != relationVector.end(); Iter++ )
        {
            if ( Iter->first == _targetFactionID )
            {
                return (EMRELATION_TYPE)Iter->second;
            }
        }
        return EMRELATION_ERROR;
    }

    const bool GLFaction::addRelation(const FactionID& _targetFactionID, const EMRELATION_TYPE _relation)
    {
        RelationVector::iterator Iter = relationVector.begin();
        for ( ; Iter != relationVector.end(); Iter++ )
        {
            if ( Iter->first == _targetFactionID )
            {
                if (Iter->second == _relation )
                    return false;

                Iter->second = _relation;
                return true;
            }
        }

        relationVector.push_back(RelationVectorPair(_targetFactionID,_relation));
        return true;
    }
    const bool GLFaction::removeRelation(const FactionID& _targetFactionID)
    {
        RelationVector::iterator Iter = relationVector.begin();
        for ( ; Iter != relationVector.end(); )
        {
            if ( Iter->first == _targetFactionID )
            {
                Iter = relationVector.erase(Iter);
                return true;
            }
            else
            {
                Iter++;
            }
        }

        return false;
    }

    const int GLFaction::getNMember() const
    {
        return (int)memberVector.size();
    }

    const bool GLFaction::isAutoRemove() const
    {
        return bAutoRemove;
    }

    void GLFaction::setAutoRemove(const bool _bAuto)
    {
        bAutoRemove = _bAuto;
    }


	int Manager::FACTION_MANAGER_COUNT = 0;

    Manager::Manager()
        : emHpVisibleRelation(EMRELATION_FLAG_ERROR)
		, FACTION_MANAGER_ID ( 0 )
    {
        actorVector.reserve(64);
        factionVector.reserve(8);

		// Faction Manager ������ ���� ���� ID�� �ο��Ѵ�;
		FACTION_MANAGER_ID = FACTION_MANAGER_COUNT;

		// ������ �ø���;
		FACTION_MANAGER_COUNT++;
    }

    Manager::~Manager()
    {
        FactionVector::iterator factionIter(factionVector.begin());
        for (; factionIter != factionVector.end(); factionIter++ )
        {
            SAFE_DELETE(factionIter->second);
        }
    }


    const EMRELATION_TYPE Manager::getRelation(const FactionID& _myFactionID, const FactionID& _targetFactionID)
    {
        GLFaction* _pMyFaction = getFaction(_myFactionID);
        if ( _pMyFaction == NULL )
            return EMRELATION_ERROR;

        return _pMyFaction->getRelation(_targetFactionID);
    }

    const EMRELATION_TYPE Manager::getRelation(const GLActorID& _myActorID, const GLActorID& _targetActorID)
    {
        const FactionID* pMyFactionID = getFactionID(_myActorID);
        const FactionID* pTargetFactionID = getFactionID(_targetActorID);

        if ( pMyFactionID && pTargetFactionID )
        {
            return getRelation( *pMyFactionID, *pTargetFactionID );
        }

        return EMRELATION_ERROR;
    }

	const Manager::EMFACTION_RETURN Manager::setRelation(const FactionID& _factionID, const FactionID& _targetFactionID, const EMRELATION_TYPE _emRelation)
    {
		// Base Faction ��ȿ�� �˻� �� ���� ó��;
        GLFaction* _pMyFaction = getFaction(_factionID);
        if ( _pMyFaction == NULL )
		{
			EMFACTION_RETURN emReturn = createFaction( _factionID );
			if ( EMFACTION_FAIL == emReturn )
			{
				if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
				{
					sc::writeLogError(
						sc::string::format( "[ Faction Info ] [ Failed Set Relation ( ManagerID : %1% ) ]",
						FACTION_MANAGER_ID ) );

					sc::writeLogError(
						sc::string::format( "[ Faction Info ] -> [ Base / FactionID : %1%, FactionType : %2% ]",
						_factionID.factionID_Num,
						_factionID.factionType ) );

					sc::writeLogError(
						sc::string::format( "[ Faction Info ] -> [ Target / FactionID : %1%, FactionType : %2% ]",
						_targetFactionID.factionID_Num,
						_targetFactionID.factionType ) );
				}

				return EMFACTION_FAIL;
			}

			_pMyFaction = getFaction(_factionID);
			if ( NULL == _pMyFaction )
			{
				sc::writeLogError(
					sc::string::format( "[ Faction Info ] [ Failed Set Relation ( ManagerID : %1% ) ]",
					FACTION_MANAGER_ID ) );

				sc::writeLogError(
					sc::string::format( "[ Faction Info ] -> [ Base / FactionID : %1%, FactionType : %2% ]",
					_factionID.factionID_Num,
					_factionID.factionType ) );

				sc::writeLogError(
					sc::string::format( "[ Faction Info ] -> [ Target / FactionID : %1%, FactionType : %2% ]",
					_targetFactionID.factionID_Num,
					_targetFactionID.factionType ) );

				return EMFACTION_FAIL;
			}
		}

		// Target Faction ��ȿ�� �˻� �� ���� ó��;
		GLFaction* _pTargetFaction = getFaction(_targetFactionID);
		if ( _pTargetFaction == NULL )
		{
			EMFACTION_RETURN emReturn = createFaction( _targetFactionID );
			if ( EMFACTION_FAIL == emReturn )
			{
				if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
				{
					sc::writeLogError(
						sc::string::format( "[ Faction Info ] [ Failed Set Relation ( ManagerID : %1% ) ]",
						FACTION_MANAGER_ID ) );

					sc::writeLogError(
						sc::string::format( "[ Faction Info ] -> [ Base / FactionID : %1%, FactionType : %2% ]",
						_factionID.factionID_Num,
						_factionID.factionType ) );

					sc::writeLogError(
						sc::string::format( "[ Faction Info ] -> [ Target / FactionID : %1%, FactionType : %2% ]",
						_targetFactionID.factionID_Num,
						_targetFactionID.factionType ) );
				}

				return EMFACTION_FAIL;
			}

			_pTargetFaction = getFaction( _targetFactionID );
			if ( NULL == _pTargetFaction )
			{
				sc::writeLogError(
					sc::string::format( "[ Faction Info ] [ Failed Set Relation ( ManagerID : %1% ) ]",
					FACTION_MANAGER_ID ) );

				sc::writeLogError(
					sc::string::format( "[ Faction Info ] -> [ Base / FactionID : %1%, FactionType : %2% ]",
					_factionID.factionID_Num,
					_factionID.factionType ) );

				sc::writeLogError(
					sc::string::format( "[ Faction Info ] -> [ Target / FactionID : %1%, FactionType : %2% ]",
					_targetFactionID.factionID_Num,
					_targetFactionID.factionType ) );

				return EMFACTION_FAIL;
			}
		}

		bool bReturn = _pMyFaction->addRelation(_targetFactionID, _emRelation);
        if ( bReturn )
		{
			if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
			{
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

#ifdef FACTION_LOG_DETAIL
				sc::writeLogInfo(
					sc::string::format( "[ Faction Info ] [ Succeed Set Relation ( ManagerID : %1% ) ]",
					FACTION_MANAGER_ID ) );

				sc::writeLogInfo(
					sc::string::format( "[ Faction Info ] -> [ Base / FactionID : %1%, FactionType : %2% ]",
					_factionID.factionID_Num,
					_factionID.factionType ) );

				sc::writeLogInfo(
					sc::string::format( "[ Faction Info ] -> [ Target / FactionID : %1%, FactionType : %2% ]",
					_targetFactionID.factionID_Num,
					_targetFactionID.factionType ) );

				sc::writeLogInfo(
					sc::string::format( "[ Faction Info ] -> [ Relation : %1% ]",
					strRelation ) );
#endif
			}
		}
		else
			return EMFACTION_ALREADY;

		return EMFACTION_SUCCESS;
    }

    const Manager::EMFACTION_RETURN Manager::setRelationToAll(const FactionID& _factionID, const EMRELATION_TYPE _emRelation)
    {
        FactionVector::iterator Iter = factionVector.begin();
        for( ; Iter != factionVector.end(); Iter++ )
        {
            const FactionID& _tarFactionID = Iter->first;
			setRelation( _factionID, _tarFactionID, _emRelation );
        }

        return EMFACTION_SUCCESS;
    }

    const Manager::EMFACTION_RETURN Manager::setRelationFromAll(const FactionID& _factionID, const EMRELATION_TYPE _emRelation)
    {
		FactionVector::iterator Iter = factionVector.begin();
		for( ; Iter != factionVector.end(); Iter++ )
		{
			const FactionID& _tarFactionID = Iter->first; 
			setRelation( _tarFactionID, _factionID, _emRelation );
		}

		return EMFACTION_SUCCESS;
    }

    const Manager::EMFACTION_RETURN Manager::createFaction(const FactionID& _factionID)
    {
		if ( NULL != getFaction(_factionID) )
			return EMFACTION_ALREADY;

        GLFaction* pFaction = generateFaction(_factionID);
		if ( NULL == pFaction )
		{
			sc::writeLogError( "[ Faction Info ] [ Manager::createFaction / Faction is NULL. ]" );

			return EMFACTION_FAIL;
		}

        pFaction->setAutoRemove(false);

		if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		{			
#ifdef FACTION_LOG_DETAIL
			sc::writeLogInfo(
				sc::string::format( "[ Faction Info ] [ Create Faction ( ManagerID : %1% ) ]",
				FACTION_MANAGER_ID ) );

			sc::writeLogInfo(
				sc::string::format( "[ Faction Info ] -> [ FactionID : %1%, FactionType : %2% ]",
				_factionID.factionID_Num,
				_factionID.factionType ) );
#endif
		}

		return EMFACTION_SUCCESS;
    }

    const Manager::EMFACTION_RETURN Manager::setFaction(const GLActorID& _actorID, const FactionID& _factionID, FactionID& refPreFactionID)
    {
        bool _bExist(false);
        ActorVector::iterator actorIter = actorVector.begin();
        for (; actorIter != actorVector.end(); actorIter++ )
        {
            if ( actorIter->first == _actorID )
            {
                FactionID& _currentFaction = actorIter->second;
				refPreFactionID = _currentFaction;

                // �����ϰ� �ϴ� �Ѽ��� �̹� ������ �Ѽ��ϰ�� �����ϰ� ����;
                if ( _currentFaction == _factionID )
                    return EMFACTION_ALREADY;

                // ������ Faction�� ���� �־��ٸ� ����;
                GLFaction* _pFaction = getFaction(_currentFaction);
                _pFaction->removeActor(_actorID);
                // (����!) ���� �Ѽ��� ����� 0���� �Ǹ� �Ѽ��� �ı��Ѵ�.(�Ѽ��� �ڵ� �ı� ������ ��쿡�� �����Ѵ�.);
                // ���࿡ �Ѽ��� �����Ǳ� ���Ѵٸ� ������ ����Actor���� �־ �ذ��ϵ��� �Ѵ�.;
                if ( _pFaction->isAutoRemove() && (_pFaction->getNMember() == 0) )
                {
                    removeFaction(_currentFaction);
                }
                _currentFaction = _factionID;

                _bExist = true;
                break;
            }
        }

        if ( _bExist == false )
        {
            actorVector.push_back(ActorFactionPair(_actorID,_factionID));
        }

        // ������ �Ѽ��� �ִٸ� �����Ѽ���, ���ٸ� ���� ������ �Ѽ��� ���´�;
        GLFaction* pNewFaction = generateFaction(_factionID);
		if ( NULL == pNewFaction )
		{
			sc::writeLogError( "[ Faction Info ] [ Manager::setFaction / Faction is NULL. ]" );

			return EMFACTION_FAIL;
		}

        // ���ο� faction�� ���;
        pNewFaction->addActor(_actorID);

		if ( GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		{
			sc::writeLogInfo(
				sc::string::format( "[ Faction Info ] [ Succeed Set Faction ( ManagerID : %1% ) ]",
				FACTION_MANAGER_ID ) );

			sc::writeLogInfo(
				sc::string::format( "[ Faction Info ] -> [ ActorID : %1% ]",
				_actorID.actorID_Num ) );

			sc::writeLogInfo(
				sc::string::format( "[ Faction Info ] -> [ FactionID : %1%, FactionType : %2% ]",
				_factionID.factionID_Num,
				_factionID.factionType ) );
		}

        return EMFACTION_SUCCESS;
    }

    const EMRELATION_FLAG_TYPE Manager::getHPVisible() const
    {
        return emHpVisibleRelation;
    }

    const bool Manager::setHPVisible(const EMRELATION_FLAG_TYPE _emHpVisible)
    {
        const bool bChanged( emHpVisibleRelation != _emHpVisible );
        emHpVisibleRelation = _emHpVisible;
        return bChanged;
    }

    const unsigned int Manager::getNFaction(void) const
    {
        return factionVector.size();
    }

    const int Manager::getNMember(const FactionID& _factionID)
    {
        GLFaction* pFaction = getFaction(_factionID);
        if ( pFaction == NULL )
            return 0;

        return pFaction->getNMember();
    }

    const FactionID* Manager::getFactionID(const GLActorID& _actorID) const
    {
        ActorVector::const_iterator actorIter = actorVector.begin();
        for (; actorIter != actorVector.end(); actorIter++ )
        {
            if ( actorIter->first == _actorID )
            {
                return &(actorIter->second);
            }
        }
        return NULL;
    }

    const FactionMemberVector* Manager::getFactionMembers(const FactionID& _factionID)
    {
        GLFaction* pFaction = getFaction(_factionID);
        if ( pFaction == NULL )
            return NULL;

        return &pFaction->getMemberVector();
    }

    void Manager::removeFaction(const FactionID& _factionID)
    {
        FactionVector::iterator factionIter(factionVector.begin());
        for (; factionIter != factionVector.end(); )
        {
            GLFaction* _pFaction = factionIter->second;

            // �ش� Faction�� ������;
            if ( factionIter->first == _factionID )
            {
                const FactionMemberVector& _pMemberVector = _pFaction->getMemberVector();
                FactionMemberVector::const_iterator Iter = _pMemberVector.begin();
                for (; Iter != _pMemberVector.end(); Iter++ )
                {
                    ActorVector::const_iterator actorIter(actorVector.begin());
                    for (; actorIter != actorVector.end(); )
                    {
                        if ( actorIter->first == (*Iter) )
                        {
                            actorIter = actorVector.erase(actorIter);
                            break;
                        }
                        else
                        {
                            actorIter++;
                        }
                    }
                }

                SAFE_DELETE(factionIter->second);
                factionIter = factionVector.erase(factionIter);
            }
            else
            {
                // �׿��� Faction������ �ش� Faction���� ���踦 ������;
                _pFaction->removeRelation(_factionID);

                factionIter++;
            }
        }
    }
    void Manager::removeActor(const GLActorID& _actorID)
    {
        ActorVector::const_iterator actorIter = actorVector.begin();
        for (; actorIter != actorVector.end(); )
        {
            if ( actorIter->first == _actorID )
            {
                const FactionID _factionID = actorIter->second;

				actorIter = actorVector.erase(actorIter);

                GLFaction* _pFaction = getFaction(_factionID);
				if ( _pFaction == NULL )
					return ;

				_pFaction->removeActor(_actorID);

                // (����!) ���� �Ѽ��� ����� 0���� �Ǹ� �Ѽ��� �ı��Ѵ�.(�Ѽ��� �ڵ� �ı� ������ ��쿡�� �����Ѵ�.);
                // ���࿡ �Ѽ��� �����Ǳ� ���Ѵٸ� ������ ����Actor���� �־ �ذ��ϵ��� �Ѵ�.;
                if ( _pFaction->isAutoRemove() && (_pFaction->getNMember() == 0) )
                {
                    removeFaction(_factionID);
                }
                return;
            }
            else
            {
                actorIter++;
            }
        }
    }

    GLFaction* Manager::getFaction(const FactionID& _factionID)
    {
        FactionVector::iterator factionIter(factionVector.begin());
        for (; factionIter != factionVector.end(); factionIter++ )
        {
            if ( factionIter->first == _factionID )
            {
                return factionIter->second;
            }
        }
        return NULL;
    }

    GLFaction* Manager::generateFaction(const FactionID& _factionID)
    {
        GLFaction* pNewFaction = getFaction(_factionID);
        // ������ �����ϴ� Faction�� ��� �ش� �Ѽ��� ����;
        if ( pNewFaction )
            return pNewFaction;

        // ������ �������� ���� Faction�� ��� ���� ����;
        pNewFaction = new GLFaction();
		if ( NULL == pNewFaction )
			return NULL;

		pNewFaction->setAutoRemove( false );

        factionVector.push_back(FactionVectorPair(_factionID, pNewFaction));

        // ���ο� �Ѽ��� ��������� ������� �۾���;

        // �⺻������ �ڽ��� �������� ����� �����̴�.;
        pNewFaction->addRelation(_factionID, EMRELATION_ALLY);

        return pNewFaction;
    }
}