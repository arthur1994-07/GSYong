#pragma	once

#include "UIDataType.h"
#include "UIPosData.h"
#include "UIWindowData.h"

#include <hash_map>
#include <list>

namespace GUIBase
{

class IWindow;
class IUIFactory;

class CUIWindowManager
{
public:
	typedef std::list< IWindow* >						WINDOW_LIST;
	typedef	WINDOW_LIST::iterator						WINDOW_LIST_ITER;
	typedef	WINDOW_LIST::const_iterator					WINDOW_LIST_CITER;
	typedef	WINDOW_LIST::reverse_iterator				WINDOW_LIST_RITER;

private:
	typedef stdext::hash_map< UIID, WINDOW_LIST_ITER >	WINDOW_MAP;	
	typedef WINDOW_MAP::iterator						WINDOW_MAP_ITER;
	typedef WINDOW_MAP::const_iterator					WINDOW_MAP_CITER;

public:
	static CUIWindowManager& GetInstance();

private:
	CUIWindowManager ();
	~CUIWindowManager ();

public:
	bool IsEmpty();	
	int Size();
	void RemoveAll( bool bDelete = true );

public:
	bool InsertWindow( const std::string& strID
		, WINDOW_CFG& windowCFG
		, UIRECT rcGlobalPos = UIRECT( 0, 0, 0, 0 ) );
	bool EraseWindow( UIID uiID, bool bDelete = true );

	IWindow* GetWindow( UIID uiID );

	UIID LoadUIIDUsingStrID( const std::string& strID );

	void ChangeUIFactory( IUIFactory* pUIFactory );

	const WINDOW_LIST& GetWindowList() const { return m_windowList; }

private:
	WINDOW_MAP	m_windowMap;
	WINDOW_LIST	m_windowList;

	int m_autoMakeID;

	IUIFactory* m_pUIFactory;
};

}