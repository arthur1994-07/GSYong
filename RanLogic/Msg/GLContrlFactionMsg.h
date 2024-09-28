#pragma once

#include "../Land/FactionManager/FactionManagerDefine.h"
#include "GLContrlBaseMsg.h"

namespace GLMSG
{

	// Faction Base Message;
	struct NET_FACTION_MESSAGE : public NET_MSG_GENERIC
	{
		const Faction::EMMESSAGE_TYPE emType;

		NET_FACTION_MESSAGE(const DWORD nSize, const Faction::EMMESSAGE_TYPE _emType)
			: NET_MSG_GENERIC(NET_MSG_FACTION_SYSTEM, nSize)
			, emType(_emType)
		{
			MIN_STATIC_ASSERT(sizeof(NET_FACTION_MESSAGE) <= NET_DATA_BUFSIZE);
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct NET_FACTION_INFO : public NET_FACTION_MESSAGE
	{
		enum
		{
            HEADER_NSIZE = sizeof(NET_FACTION_MESSAGE) + sizeof(DWORD) + sizeof(Faction::FactionID),
			NSIZE = (NET_DATA_BUFSIZE - HEADER_NSIZE) / sizeof(Faction::GLActorID),
		};		

		DWORD nActor;
        Faction::FactionID facionID;
        Faction::GLActorID actorList[NSIZE];

		const bool addActor(const Faction::GLActorID& _actorID)
		{
			if ( this->nActor == NSIZE )
				return false;

			this->actorList[this->nActor] = _actorID;
			this->dwSize += sizeof(Faction::GLActorID);
			++this->nActor;
			return true;
		}

		void reset(void)
		{
			this->nActor = 0;
			this->dwSize = HEADER_NSIZE;
		}

		const bool isValid(void) const
		{
			return this->nActor != 0;
		}

        NET_FACTION_INFO(void)
            : NET_FACTION_MESSAGE(HEADER_NSIZE, Faction::EMMESSAGE_FACTION_INFO_FC)
            , nActor(0)
		{
            memset(actorList, 0, sizeof(actorList));
			MIN_STATIC_ASSERT(sizeof(NET_FACTION_INFO) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_FACTION_CHANGE : public NET_FACTION_MESSAGE
	{
        Faction::FactionID facionID;
        Faction::GLActorID actorID;

        NET_FACTION_CHANGE(const Faction::FactionID _facionID, const Faction::GLActorID _actorID)
            : NET_FACTION_MESSAGE(sizeof(NET_FACTION_CHANGE), Faction::EMMESSAGE_FACTION_CHANGE_FCB)
			, facionID(_facionID)
            , actorID(_actorID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_FACTION_CHANGE) <= NET_DATA_BUFSIZE);
		}
	};

    struct NET_FACTION_RELATION_INFO: public NET_FACTION_MESSAGE
    {
        struct FactionRelationPair
        {
            Faction::FactionID faction;
            DWORD relation;
        };

        enum
        {
            HEADER_NSIZE = sizeof(NET_FACTION_MESSAGE) + sizeof(Faction::FactionID) + sizeof(DWORD),
            NSIZE = (NET_DATA_BUFSIZE - HEADER_NSIZE) / sizeof(FactionRelationPair),
        };	

        DWORD nFaction;
        Faction::FactionID sourceFaction;
        FactionRelationPair targetFactionRelation[NSIZE];

        const bool addRelation(const Faction::FactionID& _targetFaction, const Faction::EMRELATION_TYPE _relation)
        {
            if ( this->nFaction == NSIZE )
                return false;

            if ( _relation > Faction::EMRELATION_NSIZE )
                return false;

            this->targetFactionRelation[this->nFaction].faction = _targetFaction;
            this->targetFactionRelation[this->nFaction].relation = (DWORD)_relation;
            this->dwSize += (sizeof(Faction::FactionID) + sizeof(DWORD));
            ++this->nFaction;
            return true;
        }

        void reset(void)
        {
            this->nFaction = 0;
            this->dwSize = HEADER_NSIZE;
        }

        const bool isValid(void) const
        {
            return this->nFaction != 0;
        }

        NET_FACTION_RELATION_INFO(const Faction::FactionID& _sourceFaction)
            : NET_FACTION_MESSAGE(HEADER_NSIZE, Faction::EMMESSAGE_FACTION_RELATION_INFO_FC)
            , sourceFaction(_sourceFaction)
            , nFaction(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FACTION_RELATION_INFO) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_FACTION_RELATION_CHANGE: public NET_FACTION_MESSAGE
    {
        enum
        {
            CHANGE_FACTION_TO_FACTION,
            CHANGE_FACTION_TO_ALL,
            CHANGE_FACTION_FROM_ALL,
        };

        int emChangeType;
        Faction::FactionID sourceFaction;
        Faction::FactionID targetFaction;
        DWORD factionRelation;

        NET_FACTION_RELATION_CHANGE(int _emChangeType, const Faction::FactionID& _sourceFaction, const Faction::FactionID& _targetFaction, const Faction::EMRELATION_TYPE _factionRelation)
            : NET_FACTION_MESSAGE(sizeof(NET_FACTION_RELATION_CHANGE), Faction::EMMESSAGE_FACTION_RELATION_CHANGE_FCB)
            , emChangeType(_emChangeType)
            , sourceFaction(_sourceFaction)
            , targetFaction(_targetFaction)
            , factionRelation((DWORD)_factionRelation)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FACTION_RELATION_CHANGE) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_FACTION_HP_VISIBLE_INFO: public NET_FACTION_MESSAGE
    {
        DWORD emHpVisible;

        NET_FACTION_HP_VISIBLE_INFO(const Faction::EMRELATION_FLAG_TYPE _emHpVisible)
            : NET_FACTION_MESSAGE(sizeof(NET_FACTION_HP_VISIBLE_INFO), Faction::EMMESSAGE_FACTION_HP_VISIBLE_INFO_FC)
            , emHpVisible(_emHpVisible)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FACTION_HP_VISIBLE_INFO) <= NET_DATA_BUFSIZE);
        }
    };
}
