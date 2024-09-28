#pragma once

#include "IUIComponent.h"
#include "UIWindowData.h"

namespace GUIBase
{

class IUIFactory
{
public:
	virtual ~IUIFactory() { }

	virtual IUIComponent* CreateWindowForm(
		const std::string& strID,
		WINDOW_CFG& windowCFG,
		UIRECT rcGlobalPos ) = 0;
};

}