#include "stdafx.h"
#include "UIEvent.h"

namespace GUIBase
{

bool CEventUtil::MouseIn(  UIPOINT<int> point
						 , UIRECT rcGlobalPos )
{
	if( rcGlobalPos.left <= point.x &&
		rcGlobalPos.right >= point.x &&
		rcGlobalPos.top <= point.y &&
		rcGlobalPos.bottom >= point.y )
		return true;
	return false;
}

}