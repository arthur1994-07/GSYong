
#pragma once


namespace ItemPeriodExtension
{
	enum { FILENAME_LENGTH = 128 };
	enum EMPERIODEX_TESTFB
	{
		EMPERIODEX_FB_FAIL = 0,				// �Ϲ� ����;
		EMPERIODEX_FB_FAIL_DUPLICATION,		// �ڽ�Ƭ, ���������� ��� ���� ������ ��� ����;
		EMPERIODEX_FB_FAIL_LIMITCOUNT,		// ���� Ƚ�� ����;
		EMPERIODEX_FB_CHECK_EXPIRATIONDATE,	// ��� �ڽ�Ƭ���� �������� ª�� ��� Ȯ��;
		EMPERIODEX_FB_SUCCESS,				// ����!;
		EMPERIODEX_FB_SUCCESS_BASE,			// ����!;
		EMPERIODEX_FB_SUCCESS_COSTUME,		// ����!;
	};
}