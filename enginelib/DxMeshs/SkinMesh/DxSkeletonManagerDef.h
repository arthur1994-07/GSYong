#pragma once

enum EMBODY
{
	EMBODY_ROOT		= 1,	// Root부터 BIP01 까지
	EMBODY_UPBODY	= 2,	// 상체								- 굳이 상체를 움직이고 싶을 때만 셋팅한다.
	EMBODY_DOWNBODY	= 4,	// 하체								- 하체를 셋팅하는 것은 없다. < 있다면 많은 수정을 해야함 >

	EMBODY_DEFAULT	= EMBODY_ROOT|EMBODY_UPBODY|EMBODY_DOWNBODY,
};