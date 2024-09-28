#pragma	once

#include "../Util/WebWindowBase.h"

class CInnerInterface;

class CHelpWindowWeb : public CWebWindowBase
{
public:
	CHelpWindowWeb( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};