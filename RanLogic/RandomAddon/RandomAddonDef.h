
#pragma once

namespace RandomAddon
{
	enum GENERATE_OPTION
	{
		GENERATE_NONE		= 0x0,
		GENERATE_GEN		= 0x0001,
		GENERATE_REBUILD	= 0x0002,
		GENERATE_ONLYRANDOM	= 0x0004, // 랜덤 타입으로 설정되어있는 데이터만 임의의 데이터로 변경;
	};
};