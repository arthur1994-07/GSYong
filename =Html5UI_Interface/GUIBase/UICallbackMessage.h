#pragma once

#include "UIDataType.h"

namespace GUIBase
{

	/// WebPage에서 전해오는 Callback 정보를 담을 구조체;
	struct CALLBACK_MESSAGE
	{
		// Message
		std::string message;

		// 좌표 정보;
		int x;
		int y;

		// Data 정보;
		int value;

		// 문자열 정보;
		std::string text;

		CALLBACK_MESSAGE()
			: x( 0 )
			, y( 0 )
			, value( 0 ) { }
	};

}