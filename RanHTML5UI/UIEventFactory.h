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
	//! ���콺 �̺�Ʈ;
	virtual void FireMouseEvent();

	//! �� �̺�Ʈ;
	virtual void FireWheelEvent();

	//! Ű���� �̺�Ʈ;
	virtual void FireKeyEvent();

private:
	EngineDeviceMan* m_pEngineDevice;
};

}