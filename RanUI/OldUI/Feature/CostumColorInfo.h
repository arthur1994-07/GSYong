#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CCostumColorInfo : public CUIGroup
{
public:
	CCostumColorInfo(EngineDeviceMan* pEngineDevice);
	virtual ~CCostumColorInfo(void);

protected:
	enum
	{
		INFO_COLOR1_BACK		= NO_ID + 1,
		INFO_COLOR2_BACK		= NO_ID + 2,
		INFO_COLOR1_COLOR		= NO_ID + 3,
		INFO_COLOR2_COLOR		= NO_ID + 4,
	};

public:
	void	CreateSubControl ();

	void	SetSelectColor( DWORD dwMainColor, DWORD dwSubColor );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*	m_pColor1Back;
	CUIControl*	m_pColor2Back;
	CUIControl*	m_pColor1Color;
	CUIControl*	m_pColor2Color;
};
