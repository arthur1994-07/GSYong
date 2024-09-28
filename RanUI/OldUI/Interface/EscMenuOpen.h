#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;

class	CEscMenuOpen : public CUIGroup
{
protected:
	enum
	{
		ESCMENU_OPEN_BUTTON = NO_ID + 1,
	};

public:
	CEscMenuOpen(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CEscMenuOpen ();

public:
	void	CreateSubControl ();

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	CInnerInterface* m_pInterface;

};