#pragma once

#include <vector>
#include <map>

#include "../../../SigmaCore/Lua/MinLua.h"

#include "../../../enginelib/G-Logic/TypeDefine.h"

#include "../../../RanLogic/Actor/GLActorDefine.h"

namespace InstanceSystem
{

	typedef				std::vector< DWORD >				VEC_DWORD;
	typedef				VEC_DWORD::iterator					VEC_DWORD_ITER;
	typedef				VEC_DWORD::const_iterator			VEC_DWORD_CITER;

	typedef				std::vector< SNATIVEID >			VEC_NATIVEID;
	typedef				VEC_NATIVEID::iterator				VEC_NATIVEID_ITER;
	typedef				VEC_NATIVEID::const_iterator		VEC_NATIVEID_CITER;

}