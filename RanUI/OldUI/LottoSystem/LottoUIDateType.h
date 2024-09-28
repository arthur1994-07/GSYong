#pragma once

namespace LottoSystem
{
	const unsigned int LOTTO_SLOT_NUM_CONT = 6;
	const unsigned int LOTTO_MAX_PAGE_SIZE = 2;
	const unsigned int HELP_TEXT_COUNT = 3;

	struct LOTTO_WINMAN_LIST_PROPERTY_MEMBER
	{
		std::string ranking;
		std::string name;
		std::string count;
		std::string winMoney;
	};
}