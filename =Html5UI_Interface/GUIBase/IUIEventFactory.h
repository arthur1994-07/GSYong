#pragma once

#include "UIEvent.h"
#include "IWindow.h"

namespace GUIBase
{

/*--------------------------------------
	�߻��ϴ� �̺�Ʈ��;
	UIEvent�� Converting�ϴ�;
	Factory Interface;
--------------------------------------*/
class IUIEventFactory
{
public:
	virtual ~IUIEventFactory() { }

	//! ���콺 �̺�Ʈ;
	virtual void FireMouseEvent() = 0;

	//! �� �̺�Ʈ;
	virtual void FireWheelEvent() = 0;

	//! Ű���� �̺�Ʈ;
	virtual void FireKeyEvent() = 0;
};

}