#pragma	once

#include "../Enginelib/GUInterface/BasicScrollBar.h"

class	CBasicScrollBarNoTHUMB : public CBasicScrollBar
{
public:
	CBasicScrollBarNoTHUMB (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicScrollBarNoTHUMB ();

public:
	void	CreateBaseScrollBar ( char* szBaseScrollBar, const bool& bVERTICAL = true );
	void	SetVisibleSingleUp(bool bVisible);
	void	SetVisibleSingleDowon(bool bVisible);
};