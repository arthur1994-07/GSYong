

#ifndef _mrcScript_h_
#define _mrcScript_h_

#include "mrcConfig.h"


#if MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_SQUIRREL

#include "ScriptImpl/SquirrelImpl/mrcSqLayer.h"
#include "ScriptImpl/SquirrelImpl/mrcSqLayerMgr.h"
#include "ScriptImpl/SquirrelImpl/mrcSqMain.h"
#include "ScriptImpl/SquirrelImpl/mrcSqSpriteDefault.h"


#elif MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_LUA

#error yet, no support lua

#else

#error unknown script implement

#endif

#include "mrcUtil/mrcScriptType.h"







#endif //_mrcScript_h_










