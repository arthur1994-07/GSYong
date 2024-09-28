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

//! ���콺 �̺�Ʈ;
void CUIEventManager::FireMouseEvent()
{
	if( m_eventFactory )
		m_eventFactory->FireMouseEvent();
}

//! �� �̺�Ʈ;
void CUIEventManager::FireWheelEvent()
{
	if( m_eventFactory )
		m_eventFactory->FireWheelEvent();
}

//! Ű���� �̺�Ʈ;
void CUIEventManager::FireKeyEvent()
{
	if( m_eventFactory )
		m_eventFactory->FireKeyEvent();
}

}