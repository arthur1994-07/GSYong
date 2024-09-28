#pragma once

#include "FactionManagerDefine.h"

//#define FACTION_LOG_DETAIL

namespace Faction
{
    class GLFaction
    {
    protected:
        FactionMemberVector memberVector;
        RelationVector relationVector;
        bool bAutoRemove;

    public:
        const RelationVector& getRelationVector() const;
        const FactionMemberVector& getMemberVector() const;

        const bool isInActor(const GLActorID& _actorID) const;
        const bool addActor(const GLActorID& _actorID);
        const bool removeActor(const GLActorID& _actorID);

        const EMRELATION_TYPE getRelation(const FactionID& _targetFactionID) const;
        const bool addRelation(const FactionID& _targetFactionID, const EMRELATION_TYPE _relation);
        const bool removeRelation(const FactionID& _targetFactionID);

        const int getNMember() const;
        const bool isAutoRemove() const;
        void setAutoRemove(const bool _bAuto);

    public:
        GLFaction();
        virtual ~GLFaction();
    };

    typedef std::pair<FactionID, GLFaction*> FactionVectorPair;
    typedef std::vector<FactionVectorPair> FactionVector;

    class Manager
    {
	public:
		enum EMFACTION_RETURN
		{
			EMFACTION_FAIL,
			EMFACTION_SUCCESS,
			EMFACTION_ALREADY,
		};

    protected:
        ActorVector actorVector;
        FactionVector factionVector;

        // 특정 진영관계에따라 HP표시를 할것인지를 제어할 수 있도록한 플래그;
        EMRELATION_FLAG_TYPE emHpVisibleRelation; 

    public:
        const EMRELATION_TYPE getRelation(const FactionID& _myFactionID, const FactionID& _targetFactionID);
        const EMRELATION_TYPE getRelation(const GLActorID& _myActorID, const GLActorID& _targetActorID);

        const EMFACTION_RETURN setRelation(const FactionID& _factionID, const FactionID& _targetFactionID, const EMRELATION_TYPE _emRelation);
        const EMFACTION_RETURN setRelationToAll(const FactionID& _factionID, const EMRELATION_TYPE _emRelation);
        const EMFACTION_RETURN setRelationFromAll(const FactionID& _factionID, const EMRELATION_TYPE _emRelation);

        const EMFACTION_RETURN createFaction(const FactionID& _factionID);
        const EMFACTION_RETURN setFaction(const GLActorID& _actorID, const FactionID& _factionID, FactionID& refPreFactionID );        

        const EMRELATION_FLAG_TYPE getHPVisible() const;  
        const bool setHPVisible(const EMRELATION_FLAG_TYPE _emHpVisible);

        const unsigned int getNFaction(void) const;
        const int getNMember(const FactionID& _factionID);

        const FactionID* getFactionID(const GLActorID& _actorID) const;
        const FactionMemberVector* getFactionMembers(const FactionID& _factionID);

        void removeFaction(const FactionID& _factionID);
        void removeActor(const GLActorID& _actorID);
	
	public:
		inline ActorVector GetActorList ( ) { return actorVector; }

    protected:
        GLFaction* getFaction(const FactionID& _factionID);
        GLFaction* generateFaction(const FactionID& _factionID);

    public:
        Manager();
        virtual ~Manager();

	protected:
		int FACTION_MANAGER_ID;

	protected:
		static int FACTION_MANAGER_COUNT;
    };
};

#include "FactionManager.hpp"