#pragma once

#include "UIDataType.h"

namespace GUIBase
{
	// WebKit�� ���� Interface;
	class IView
	{
	public:
		virtual UIRECT GetPosition() = 0;
	};
}