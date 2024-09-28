#pragma once

namespace RandomOptionTool
{
	// [type-main] [type-sub] [type-value] [value-index]
	enum TYPE
	{
		TYPE_INFORMATION_BASIC	= (1 << 24),
		TYPE_TYPE_DROP					= (2 << 24),
		TYPE_TYPE_REMODEL			= (3 << 24),

		TYPE_INFORMATION_TYPE		= (1 << 16),
		TYPE_INFORMATION_VALUE	= (2 << 16),
	};
}