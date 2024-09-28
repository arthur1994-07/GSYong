#pragma once

#include <GUIBase/IUIFactory.h>

class GLGaeaClient;
class EngineDeviceMan;

namespace HTML5UI
{

class CUIGameFactory : public GUIBase::IUIFactory
{
public:
	/// Game UIÀÇ UIID;
	enum GAME_WINDOW_ID
	{
		ROOT_UI = 1,
		INVENTORY = 2,
		WEBPAGE = 3
	};

public:
	CUIGameFactory( GLGaeaClient* pGaeaClient,
		EngineDeviceMan* pEngineDevice );
	virtual ~CUIGameFactory();

	virtual GUIBase::IUIComponent* CreateWindowForm(
		const std::string& strID,
		GUIBase::WINDOW_CFG& windowCFG,
		GUIBase::UIRECT rcGlobalPos );

private:
	GLGaeaClient* m_pGaeaClient;
	EngineDeviceMan* m_pEngineDevice;
};

}