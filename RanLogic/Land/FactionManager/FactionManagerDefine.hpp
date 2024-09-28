#pragma once

namespace Faction
{
    __forceinline const FactionID& FactionID::operator =(const FactionID& _rhs)
    {
        ::memcpy(this, &_rhs, sizeof(FactionID));
        return *this;
    }
    __forceinline const bool FactionID::operator <(const FactionID& _rhs) const
    {
        if ( this->factionType < _rhs.factionType )
            return true;

        if ( this->factionType == _rhs.factionType )
            return this->factionID_Num < _rhs.factionID_Num;

        return false;
    }
    __forceinline const bool FactionID::operator ==(const FactionID& _rhs) const
    {
        return (this->factionType == _rhs.factionType) && (this->factionID_Num == _rhs.factionID_Num);
    }
    __forceinline const bool FactionID::operator !=(const FactionID& _rhs) const
    {
        return (this->factionType != _rhs.factionType) || (this->factionID_Num != _rhs.factionID_Num);
    }
    __forceinline void FactionID::reset()
    {
        factionType = EMFACTION_ERROR;
        factionID_Num = GAEAID_NULL;
    }

    /*__forceinline const GLActorID& GLActorID::operator =(const GLActorID& _rhs)
    {
        ::memcpy(this, &_rhs, sizeof(GLActorID));
        return *this;
    }
    __forceinline const bool GLActorID::operator <(const GLActorID& _rhs) const
    {
        if ( this->actorType < _rhs.actorType )
            return true;

        if ( this->actorType == _rhs.actorType )
            return this->actorID_Num < _rhs.actorID_Num;

        return false;
    }
    __forceinline const bool GLActorID::operator ==(const GLActorID& _rhs) const
    {
        return (this->actorType == _rhs.actorType) && (this->actorID_Num == _rhs.actorID_Num);
    }
    __forceinline const bool GLActorID::operator !=(const GLActorID& _rhs) const
    {
        return (this->actorType != _rhs.actorType) || (this->actorID_Num != _rhs.actorID_Num);
    }*/
}