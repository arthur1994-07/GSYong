
#pragma once

// ���� �ɼ�, ���� ���� �ɼ�, ���� �⺻ �ɷ�ġ ���;
// �����ۿɼǿ��� �������� �߻��� �� �ִ� �ɼǵ鿡 �ʿ��� ��ƿ��Ƽ���̴�;

namespace ITEM_RANDOMOP_UTILL
{
	// ���� ī��� ���� ���� �ɼ�;
	class FixOption
	{
	public:
		// �Ʒ� �ɼǵ��� ��� �ڷᱸ���� ���� ū������ �����ؾ��Ѵ�;
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
		BYTE cOptType[FIX_MAXSIZE]; // ���� �ɼ� ����;
		DWORD nSize;
	};
};