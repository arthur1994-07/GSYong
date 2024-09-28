#pragma once

#include "../../enginelib/G-Logic/GLDefine.h"

// namespace는 나중에 한번에 제거한다;
namespace Faction
{

	class GLActorID
	{
	public:
		__forceinline const GLActorID& operator =(const GLActorID& _rhs);
		__forceinline const bool operator <(const GLActorID& _rhs) const;
		__forceinline const bool operator ==(const GLActorID& _rhs) const;
		__forceinline const bool operator !=(const GLActorID& _rhs) const;

		GLActorID(void);
		GLActorID(const DWORD _actorType, const DWORD _actorID_Num);
		GLActorID(const GLActorID& _rhs);

		const EMCROW actorType;
		const DWORD actorID_Num; // 몹 : GaeaID, 플레이어 : dbNum;
	};

	__forceinline const GLActorID& GLActorID::operator =(const GLActorID& _rhs)
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
	}

}