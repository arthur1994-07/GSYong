#pragma once

#include "UIEvent.h"
#include "IUIEventFactory.h"

namespace GUIBase
{

class IWindow;

/*------------------------------
	Event를 총괄하는 Manager;
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
	//! 마우스 이벤트 발생;
	void FireMouseEvent();

	//! 휠 이벤트 발생;
	void FireWheelEvent();

	//! 키보드 이벤트 발생;
	void FireKeyEvent();

public:
	//! 이벤트 팩토리 세팅 ( 인터페이스 구현 );
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