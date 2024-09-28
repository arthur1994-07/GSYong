

#ifndef _mrcRegisterScriptObject_h_
#define _mrcRegisterScriptObject_h_

#include "mrcConfig.h"

namespace mrc
{

	// 외부에서 스크립트 객체를 등록하는 함수 등록
	void registerScriptObject( funcRegisterScriptObject func );

	// 외부에서 등록한  스크립트 객체를 등록하는 함수를  실행
	void runRegisteredScriptObject();





}


#endif