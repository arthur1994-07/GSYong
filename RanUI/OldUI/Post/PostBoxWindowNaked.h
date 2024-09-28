#pragma	once

#include "PostBoxWindowObject.h"

class CInnerInterface;

class CPostBoxWindowNaked : public CPostBoxWindowObject
{
protected:
	enum
	{
		POSTBOX_WINDOW_NAKED_NONE = POSTBOX_WINDOW_OBJECT_NEXT,
	};

public:
	CPostBoxWindowNaked ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CPostBoxWindowNaked ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void	CreateSubControlEx ();

public:
	virtual void	PageOpen  ( CPostBoxPage* pPage );

};
