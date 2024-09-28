#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"
#include "../../RanLogic/Country/GLCountryMan.h"
#include "../Widget/GLWidgetScript.h"

namespace Country
{

	/************************************************************************/
	/* Client Country Manager ( Virtual Class )                              */
	/************************************************************************/
	class GLCountryManClient : public GLCountryMan, public CSingletone< GLCountryManClient >
	{
	protected:
		friend CSingletone;

	private:
		GLCountryManClient () : GLCountryMan ( ) { }
		~GLCountryManClient () { }

	public:
		virtual bool LoadData ();

	public :
		LuaTable GetContinentList();
		LuaTable GetCountryList( DWORD dwContinentID );
	};

}