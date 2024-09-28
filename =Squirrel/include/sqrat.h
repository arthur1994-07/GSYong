//
// Sqrat: Squirrel C++ Binding Utility
//

//
// Copyright (c) 2009 Brandon Jones
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//	1. The origin of this software must not be misrepresented; you must not
//	claim that you wrote the original software. If you use this software
//	in a product, an acknowledgment in the product documentation would be
//	appreciated but is not required.
//
//	2. Altered source versions must be plainly marked as such, and must not be
//	misrepresented as being the original software.
//
//	3. This notice may not be removed or altered from any source
//	distribution.
//

#if !defined(_SCRAT_MAIN_H_)
#define _SCRAT_MAIN_H_

#include <squirrel.h>

#ifndef SQRAT_ASSERT_MSG
//#error define Error Assert
#define SQRAT_ASSERT_MSG(TEST,MSG) assert(TEST && MSG)
#endif

#ifndef SQRAT_ASSERT
//#error define Error Assert
#define SQRAT_ASSERT(TEST) assert( (TEST) && _SC("normal assert"))
#endif

#ifndef SQRAT_ASSERT_ARG_TYPE
#define SQRAT_ASSERT_ARG_TYPE(TEST,VM,ARG,MSG) SQRAT_ASSERT_MSG(TEST,MSG)
#endif

#ifndef SQRAT_ASSERT_RETURN_TYPE
#define SQRAT_ASSERT_RETURN_TYPE(TEST,VM,ARG,MSG) SQRAT_ASSERT_RETURN_TYPE(TEST,MSG)
#endif

#ifndef SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN
#define SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN() 
#endif

#ifndef SQRAT_ASSERT_OVERLOADING_ARG_TYPE
#define SQRAT_ASSERT_OVERLOADING_ARG_TYPE(OVERLOADING_STR) 
#endif

#ifndef SQRAT_ASSERT_ARG_COUNT
#define SQRAT_ASSERT_ARG_COUNT(VM,COUNT) SQRAT_ASSERT( sq_gettop(VM)-2 == COUNT )
#endif

#ifndef SQRAT_VAR_GET_CLASS_NAME
#define SQRAT_VAR_GET_CLASS_NAME() string(_SC("class"))
#endif

#ifndef SQRAT_GET_CLASS_NAME
#define SQRAT_GET_CLASS_NAME(T) string(_SC("class"))
#endif


#include "sqrat/sqratTable.h"
#include "sqrat/sqratClass.h"
#include "sqrat/sqratFunction.h"
#include "sqrat/sqratConst.h"
#include "sqrat/sqratUtil.h"
#include "sqrat/sqratScript.h"

#include "sqrat/sqratImpClass.h"

#endif
