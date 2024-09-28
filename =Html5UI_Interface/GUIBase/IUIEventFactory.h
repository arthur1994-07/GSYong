#pragma once

#include "UIEvent.h"
#include "IWindow.h"

namespace GUIBase
{

/*--------------------------------------
	발생하는 이벤트를;
	UIEvent로 Converting하는;
	Factory Interface;
--------------------------------------*/
class IUIEventFactory
{
public:
	virtual ~IUIEventFactory() { }

	//! 마우스 이벤트;
	virtual void FireMouseEvent() = 0;

	//! 휠 이벤트;
	virtual void FireWheelEvent() = 0;

	//! 키보드 이벤트;
	virtual void FireKeyEvent() = 0;
};

}