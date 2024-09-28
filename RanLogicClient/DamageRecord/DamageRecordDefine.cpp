#include "pch.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "DamageRecordDefine.h"

namespace DamageRecord
{
	const char strAttribute[LOG_ATTRIBUTE_NSIZE][32] = 
	{
		"CharNum",
		"damage",
		"time",
		"DPS",
		"SkillID",
		"Level",
	};	

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	LogID::LogID(void)
		: nType(INVALID)
		, dwDbNum(INVALID)
	{
	}
	LogID::LogID(const LogID& rhs)
		: nType(rhs.nType)
		, dwDbNum(rhs.dwDbNum)
	{
	}
	LogID::LogID(const DWORD _dwDbNum)
		: nType(CROW_PC)
		, dwDbNum(_dwDbNum)
	{
	}
	LogID::LogID(const WORD _wMainID, const WORD _wSubID)
		: nType(CROW_MOB)
		, wMainID(_wMainID)
		, wSubID(_wSubID)
	{
	}

	const bool LogID::operator ==(const LogID& rhs) const
	{
		return (nType == rhs.nType) && (dwDbNum == rhs.dwDbNum);
	}
	const bool LogID::operator !=(const LogID& rhs) const
	{
		return (nType != rhs.nType) || (dwDbNum != rhs.dwDbNum);
	}
	const bool LogID::operator <(const LogID& rhs) const
	{
		if ( nType < rhs.nType )
			return true;
		else if ( nType == rhs.nType )
		{
			// TODO : compare dwDbNum.
			return dwDbNum < rhs.dwDbNum;
		}

		return false;		
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////

}