#pragma	once

#include "PostBoxWindowObject.h"

class CInnerInterface;
class CBasicTextBox;
class CPostBoxPage;

class CPostBoxWindowSub : public CPostBoxWindowObject
{
public:
	enum
	{
		POSTBOX_WINDOW_SUB_NONE = POSTBOX_WINDOW_OBJECT_NEXT,
	};

public:
	CPostBoxWindowSub ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CPostBoxWindowSub ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void	CreateSubControlEx ();

public:
	virtual void	PageOpen  ( CPostBoxPage* pPage );

public:
	CBasicTextBox*	m_pHeadText;

};
