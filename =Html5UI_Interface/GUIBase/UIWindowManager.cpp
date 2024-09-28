#include "StdAfx.h"
#include "UIWindowManager.h"
#include "IUIFactory.h"
#include "IWindow.h"

namespace GUIBase
{

CUIWindowManager& CUIWindowManager::GetInstance()
{
	static CUIWindowManager instance;
	return instance;
}

CUIWindowManager::CUIWindowManager()
: m_autoMakeID( 1 )
, m_pUIFactory( NULL ) { }

CUIWindowManager::~CUIWindowManager ()
{
	RemoveAll ();
}

void CUIWindowManager::RemoveAll ( bool bDELETE )
{
	if( bDELETE )
	{
		WINDOW_LIST_ITER iter = m_windowList.begin();
		WINDOW_LIST_ITER iter_end = m_windowList.end();
		for( ; iter != iter_end; ++iter )
			SAFE_DELETE ( *iter );
	}

	m_windowList.clear();
	m_windowMap.clear();
}

bool CUIWindowManager::IsEmpty()
{	
	return m_windowList.empty();
}

int	CUIWindowManager::Size()
{
	return (int)m_windowList.size();
}

IWindow* CUIWindowManager::GetWindow( UIID uiID )
{
	WINDOW_MAP_ITER found = m_windowMap.find( uiID );
	if( found != m_windowMap.end() )
		return *(found->second);
	else
		return NULL;
}

UIID CUIWindowManager::LoadUIIDUsingStrID( const std::string& strID )
{
	WINDOW_LIST_ITER iter = m_windowList.begin();
	WINDOW_LIST_ITER iter_end = m_windowList.end();
	for( ; iter != iter_end; ++iter )
	{
		IWindow* pWindow = *iter;
		if( pWindow->GetStrID() == strID )
			return pWindow->GetUIID();
	}

	return NO_ID;
}

bool CUIWindowManager::InsertWindow(  const std::string& strID
									, WINDOW_CFG& windowCFG
									, UIRECT rcGlobalPos )
{
	IWindow* pNewWindow = NULL;
	if( m_pUIFactory )
	{
		pNewWindow = static_cast<IWindow*>( m_pUIFactory->CreateWindowForm(
			strID, windowCFG, rcGlobalPos ) );
	}

	if( !pNewWindow )
		return false;

	UIID uiID = pNewWindow->GetUIID();	
	bool bNO_ID = ( uiID == NO_ID );

	// 아이디가 없다면;
	if( bNO_ID )
	{
		uiID = ( m_autoMakeID++ ) + FORCE_ID;
		if( INT_MAX <= uiID )
			return false;

		pNewWindow->SetUIID( uiID );		
		bNO_ID = false;
	}

	// ID 중복검사;
	if( !bNO_ID && GetWindow( uiID ) )
		return false;

	m_windowList.push_back( pNewWindow );

	// 맵에 등록;
	if( !bNO_ID )
		m_windowMap.insert( std::make_pair( uiID, --m_windowList.end() ) );

	return true;
}

bool CUIWindowManager::EraseWindow( UIID uiID, bool bDelete )
{
	WINDOW_MAP_ITER found = m_windowMap.find( uiID );
	if( found != m_windowMap.end() )
	{
		if( bDelete )
			SAFE_DELETE( *(found->second) );

		m_windowList.erase( found->second );
		m_windowMap.erase( found );
		return true;
	}

	return false;
}

void CUIWindowManager::ChangeUIFactory( IUIFactory* pUIFactory )
{
	SAFE_DELETE( m_pUIFactory );
	m_pUIFactory = pUIFactory;
}

}