

#ifndef _mrcScriptType_h_
#define _mrcScriptType_h_

#include "mrcConfig.h"

#if MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_SQUIRREL
namespace Sqrat
{
	class Function;
	class Thread;
	class Object;
}
#elif MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_LUA


#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace mrc
{
	

#if MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_SQUIRREL
	class SqMain;
	class SqLayer;
//	class SqFunction;
	typedef Sqrat::Function SqFunction;
	typedef Sqrat::Thread SqThread;
	typedef Sqrat::Object SqObject;
	//////////////////////////////////////////////////////////////////////////

	typedef SqMain		 ScriptMain;
	typedef SqLayer		 ScriptLayer;
	typedef SqFunction	 ScriptFunction;
	typedef SqThread     ScriptThread;
	typedef SqObject	 ScriptObject;
	typedef mrcAutoPtr<ScriptFunction> AP_ScriptFunction;
	typedef mrcAutoPtr<SqThread>	   AP_ScriptThread;

#elif MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_LUA
	
	
#endif
	
	
	
	



}








#endif //_mrcScript_h_










