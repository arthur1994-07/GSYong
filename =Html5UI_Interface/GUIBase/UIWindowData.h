#pragma once

#include "UIDataType.h"
#include "IUIComponent.h"

namespace GUIBase
{

	/// 윈도우 정보;
	struct WINDOW_CFG
	{
		UIRECT rcWindowPos;				// 위치 및 크기;
		std::wstring url;				// URL;
		IUIComponent* pParentWindow;	// 부모;
	};

}