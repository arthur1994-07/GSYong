#include "pch.h"

#include "../../EngineLib/Common/SUBPATH.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../RanLogic/GLogicDataMan.h"

#include "./GLCountryManServer.h"

namespace Country
{

	bool GLCountryManServer::LoadData ()
	{
		GLCountryMan::LoadData();

		return true;
	}

}