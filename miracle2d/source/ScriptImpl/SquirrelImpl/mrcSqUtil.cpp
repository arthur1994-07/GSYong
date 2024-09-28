

#include "mrcPrecompiled.h"

#include "mrcSqUtil.h"
#include <squirrel.h>
#include "sqrat/sqratUtil.h"

namespace mrc
{
//////////////////////////////////////////////////////////////////////////

NString getSqError()
{
	SQStackInfos stack;
	if( SQ_OK != sq_stackinfos(Sqrat::DefaultVM::Get(),1,&stack) )
		return NString();

	return toNString("error : ") + toNString(stack.source)+ toNString(" ") + 
		toNString(stack.line) + toNString(" - function : ") + toNString(stack.funcname); 
}















//////////////////////////////////////////////////////////////////////////
}