

#pragma	once


#include "../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;


class RnHistoryInfoTap : public CUIGroup 
{
public:
	RnHistoryInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	void CreateSubControl();
};