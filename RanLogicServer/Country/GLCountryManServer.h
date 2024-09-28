#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "../../RanLogic/Country/GLCountryMan.h"

#include "../../RanLogic/Msg/GLContrlCountry.h"

namespace Country
{

	/************************************************************************/
	/* Server Country Manager ( Virtual Class )                              */
	/************************************************************************/
	class GLCountryManServer : public GLCountryMan, public CSingletone< GLCountryManServer >
	{
	protected:
		friend CSingletone;

	private:
		GLCountryManServer () : GLCountryMan ( ) { }
		~GLCountryManServer () { }

	public:
		virtual bool LoadData ();
	};

}