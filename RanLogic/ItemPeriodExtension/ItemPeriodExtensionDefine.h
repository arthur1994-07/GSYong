
#pragma once


namespace ItemPeriodExtension
{
	enum { FILENAME_LENGTH = 128 };
	enum EMPERIODEX_TESTFB
	{
		EMPERIODEX_FB_FAIL = 0,				// 일반 오류;
		EMPERIODEX_FB_FAIL_DUPLICATION,		// 코스튬, 장착아이템 모두 적용 가능할 경우 오류;
		EMPERIODEX_FB_FAIL_LIMITCOUNT,		// 연장 횟수 오버;
		EMPERIODEX_FB_CHECK_EXPIRATIONDATE,	// 장비가 코스튬보다 만료일이 짧을 경우 확인;
		EMPERIODEX_FB_SUCCESS,				// 성공!;
		EMPERIODEX_FB_SUCCESS_BASE,			// 성공!;
		EMPERIODEX_FB_SUCCESS_COSTUME,		// 성공!;
	};
}