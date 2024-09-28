#include "pch.h"

#include "../Character/GLCharDefine.h"

#include "GLActorDefine.h"

//-------------------------------------------------------------------//
#include "../../SigmaCore/DebugInclude.h"
//-------------------------------------------------------------------//

namespace Faction
{
	GLActorID::GLActorID(void)
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
	}
}