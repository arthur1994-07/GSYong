#pragma once

#include "UIDataType.h"

namespace GUIBase
{

	/// WebPage���� ���ؿ��� Callback ������ ���� ����ü;
	struct CALLBACK_MESSAGE
	{
		// Message
		std::string message;

		// ��ǥ ����;
		int x;
		int y;

		// Data ����;
		int value;

		// ���ڿ� ����;
		std::string text;

		CALLBACK_MESSAGE()
			: x( 0 )
			, y( 0 )
			, value( 0 ) { }
	};

}