

#ifndef _mrcPrecompiled_h_
#define _mrcPrecompiled_h_

#include "miracle.h"

#include "mrcUtil/mrcSingleton.h"
#include "mrcUtil/mrcFileSystem.h"


#if MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_SQUIRREL

//////////////////////////////////////////////////////////////////////////



#include "mrcUtil/mrcSqratDefine.h"


#elif MIRACLE_SCRIPT_IMPLEMENT == MIRACLE_SCRIPT_LUA

#error yet, no support lua

#else

#error unknown script implement

#endif






#endif