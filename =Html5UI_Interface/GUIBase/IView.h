#pragma once

#include "UIDataType.h"

namespace GUIBase
{
	// WebKit¿ª ¿ß«— Interface;
	class IView
	{
	public:
		virtual UIRECT GetPosition() = 0;
	};
}