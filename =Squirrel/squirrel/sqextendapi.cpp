


#include "sqpcheader.h"
#include "sqvm.h"
#include "sqstring.h"
#include "sqtable.h"
#include "sqarray.h"
#include "sqfuncproto.h"
#include "sqclosure.h"


void sqex_killgenerator(HSQOBJECT* obj)
{
	assert( sq_isgenerator(*obj) );
	_generator(*obj)->Kill();
}

SQBool sqex_isgeneratordead(HSQOBJECT* obj)
{
	assert( sq_isgenerator(*obj) );
	return _generator(*obj)->_state == SQGenerator::eDead;
}
