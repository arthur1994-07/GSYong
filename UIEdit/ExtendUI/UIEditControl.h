#pragma	once

#include "./UIEditGroup.h"

class CUIEditControl : public CUIEditGroup
{
public:
	CUIEditControl(EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin);
	virtual	~CUIEditControl();

};
