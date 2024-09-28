


#pragma	once


#include "../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;


class RnPetInfoTap : public CUIGroup
{
public:
	RnPetInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	void CreateSubControl();

	void DeleteUIMeshData();

};