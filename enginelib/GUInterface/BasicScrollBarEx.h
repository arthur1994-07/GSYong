#pragma	once

#include "../Enginelib/GUInterface/BasicScrollBar.h"

class	CBasicScrollBarEx : public CBasicScrollBar
{
public:
	CBasicScrollBarEx (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicScrollBarEx ();

public:
    void	CreateBaseScrollBar ( char* szBaseScrollBar, const bool& bVERTICAL = true );
};