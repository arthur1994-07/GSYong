#pragma once

#include "UIEvent.h"
#include "IUIEventFactory.h"

namespace GUIBase
{

class IWindow;

/*------------------------------
	Event�� �Ѱ��ϴ� Manager;
------------------------------*/
class CUIEventManager
{
public:
	static CUIEventManager& GetInstance();

private:
	CUIEventManager();
	~CUIEventManager();

public:
	//! Event Update
	void EventUpdate();

public:
	//! ���콺 �̺�Ʈ �߻�;
	void FireMouseEvent();

	//! �� �̺�Ʈ �߻�;
	void FireWheelEvent();

	//! Ű���� �̺�Ʈ �߻�;
	void FireKeyEvent();

public:
	//! �̺�Ʈ ���丮 ���� ( �������̽� ���� );
	inline void SetEventFactory( IUIEventFactory* eventFactory )
	{
		SAFE_DELETE( m_eventFactory );
		m_eventFactory = eventFactory;
	}

private:
	// Event Factory
	IUIEventFactory* m_eventFactory;
};

}