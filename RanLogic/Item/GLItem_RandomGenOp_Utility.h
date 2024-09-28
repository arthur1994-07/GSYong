
#pragma once

// 랜덤 옵션, 랜덤 가산 옵션, 랜덤 기본 능력치 등등;
// 아이템옵션에서 랜덤으로 발생할 수 있는 옵션들에 필요한 유틸리티들이다;

namespace ITEM_RANDOMOP_UTILL
{
	// 인장 카드로 사용될 고정 옵션;
	class FixOption
	{
	public:
		// 아래 옵션들을 담는 자료구조중 가장 큰양으로 설정해야한다;
		// RandomOpt::NSIZE
		// SITEMCUSTOM_ADDON::ADDON_SIZE
		enum
		{
			FIX_MAXSIZE = 6,
		};

	public:
		FixOption(void);
		virtual ~FixOption(void) {}

	public:
		const FixOption& operator =(const ITEM_RANDOMOP_UTILL::FixOption& rhs);
		const bool operator ==(const BYTE& _cOptType) const;

	public:
		void push(const BYTE _cOptType, const DWORD nFixMaximum);
		void clear(void);
		const bool isValid(void) const;
		const DWORD getNSize(void) const;

		const BYTE GetOptType ( const UINT _nIndex ) const;

	private:
		BYTE cOptType[FIX_MAXSIZE]; // 랜덤 옵션 종류;
		DWORD nSize;
	};
};