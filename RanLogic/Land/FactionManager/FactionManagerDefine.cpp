#include "pch.h"

#include "FactionManagerDefine.h"

namespace Faction
{
    FactionID::FactionID(void)
        : factionType(EMFACTION_ERROR)
        , factionID_Num(GAEAID_NULL)
    {
    }
    FactionID::FactionID(const DWORD _factionType, const DWORD _factionID_Num)
        : factionType((EMFACTION_TYPE)_factionType)
        , factionID_Num(_factionID_Num)
    {
    }
    FactionID::FactionID(const FactionID& _rhs)
        : factionType(_rhs.factionType)
        , factionID_Num(_rhs.factionID_Num)
    {
    }

    /*GLActorID::GLActorID(void)
        : actorType(CROW_NULL)
        , actorID_Num(GAEAID_NULL)
    {
    }
    GLActorID::GLActorID(const DWORD _actorType, const DWORD _actorID_Num)
        : actorType((EMCROW)_actorType)
        , actorID_Num(_actorID_Num)
    {
    }
    GLActorID::GLActorID(const GLActorID& _rhs)
        : actorType(_rhs.actorType)
        , actorID_Num(_rhs.actorID_Num)
    {
    }*/
}