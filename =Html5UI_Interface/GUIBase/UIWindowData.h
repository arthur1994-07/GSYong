#pragma once

#include "UIDataType.h"
#include "IUIComponent.h"

namespace GUIBase
{

	/// ������ ����;
	struct WINDOW_CFG
	{
		UIRECT rcWindowPos;				// ��ġ �� ũ��;
		std::wstring url;				// URL;
		IUIComponent* pParentWindow;	// �θ�;
	};

}