#include "StdAfx.h"
#include "UIEventManager.h"

namespace GUIBase
{

CUIEventManager& CUIEventManager::GetInstance()
{
	static CUIEventManager instance;
	return instance;
}

CUIEventManager::CUIEventManager()
: m_eventFactory( NULL ) { }

CUIEventManager::~CUIEventManager()
{
	SAFE_DELETE( m_eventFactory );
}

void CUIEventManager::EventUpdate()
{
	// Input Event
	FireMouseEvent();
	FireWheelEvent();
	FireKeyEvent();
}

//! 마우스 이벤트;
void CUIEventManager::FireMouseEvent()
{
	if( m_eventFactory )
		m_eventFactory->FireMouseEvent();
}

//! 휠 이벤트;
void CUIEventManager::FireWheelEvent()
{
	if( m_eventFactory )
		m_eventFactory->FireWheelEvent();
}

//! 키보드 이벤트;
void CUIEventManager::FireKeyEvent()
{
	if( m_eventFactory )
		m_eventFactory->FireKeyEvent();
}

}