
#include "mrcPrecompiled.h"

#include "mrcRegisterScriptObject.h"


namespace mrc
{
	static vector<funcRegisterScriptObject> sFuncRegisterScriptVec;


	void registerScriptObject( funcRegisterScriptObject func )
	{
		mrcASSERT(func);
		vector<funcRegisterScriptObject>::iterator it =	
			std::find(sFuncRegisterScriptVec.begin(), sFuncRegisterScriptVec.end(),func);
		mrcASSERT(sFuncRegisterScriptVec.end() == it);
		sFuncRegisterScriptVec.push_back(func);
	}

	void runRegisteredScriptObject()
	{
		BOOST_FOREACH(funcRegisterScriptObject func,sFuncRegisterScriptVec)
		{
			func();
		}
	}
}
