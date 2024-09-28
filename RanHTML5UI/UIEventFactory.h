#pragma once

#include <GUIBase/IUIEventFactory.h>

class EngineDeviceMan;

namespace HTML5UI
{

class CUIEventFactory : public GUIBase::IUIEventFactory
{
public:
	CUIEventFactory( EngineDeviceMan* pEngineDevice );
	~CUIEventFactory();

public:
	//! 마우스 이벤트;
	virtual void FireMouseEvent();

	//! 휠 이벤트;
	virtual void FireWheelEvent();

	//! 키보드 이벤트;
	virtual void FireKeyEvent();

private:
	EngineDeviceMan* m_pEngineDevice;
};

}