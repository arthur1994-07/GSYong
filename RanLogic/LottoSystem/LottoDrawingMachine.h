#pragma once

#include "./GLLottoDataType.h"

namespace LottoSystem
{

	// �ζ� ��÷�� ����;
	class CLottoDrawingMachine
	{
	public:
		CLottoDrawingMachine();
		~CLottoDrawingMachine();

	public:
		// ��÷�� ��ȣ ���� �ʱ�ȭ �� ��÷�� ���� �ʱ�ȭ;
		void Initialize(
			unsigned int numCount,			// ��ȣ �� ����;
			unsigned int count,				// ��÷�� ��ȣ �� ����;
			unsigned int mainNumCount,		// Main ��ȣ ����;
			unsigned int bonusNumCount );	// Bonus ��ȣ ����;

	public:
		/* ��÷ ����;
		 * �ϳ��� ��ȣ�� ��ȯ;
		 * ������ ��� ������ �� ��ȯ �Ǿ��� ���;
		 * �� �̻� ���ڰ� ������ �ʴ´� ( 0 ��ȯ );	*/
		unsigned int StartDrawingMachine();

		// ��÷�� ���Ӱ� �����ϱ� ���� �ʱ�ȭ �Ѵ�;
		void ClearDrawingMachine();

		/* ��� ��ȣ�� �´��� Ȯ���Ѵ�;
		 * ���������� DB���� Ȯ���ϱ� ������ ������� �ʴ´�; */
		void ConfirmWinningNum(
			IN const unsigned int* drawingNums,	// ��÷ ��ȣ;
			IN const unsigned int* winNums,		// ��÷ ��ȣ;
			OUT bool* drawingNumStates,			// ��÷ ��ȣ �´��� Ȯ��;
			OUT int& mainNumCount,				// ���� ��ȣ �´� ����;
			OUT int& bonusNumCount );			// ���ʽ� ��ȣ �´� ����;

	public:
		// Ȯ���� ���� ������ ���� �ʱ�ȭ;
		void InitializeNumCount();

		// Ȯ���� ���� ������ ���� ����;
		void ChangeNumCount(
			unsigned int num,					// ����;
			int delta );						// ���� ����;

		// �����ϸ鼭 ���ÿ� Ȯ���� ����ǵ��� DrawingNums�� �����Ѵ�;
		void ChangeNumCountAndDrawingNums(
			unsigned int num,					// ����;
			int delta );						// ���� ����;

		// ���� ����;
		void AddAccumulateNum(
			unsigned int num,					// ����;
			unsigned int delta );				// ���� ����;

		// ��÷ ���� ����;
		void ChangeDrawingNums();

		// ���� �� AddCount ����;
		void SetNumInfo(
			unsigned int num,					// ����;
			unsigned int accumulateCount,		// ���� ����;
			unsigned int addCount );			// Ȯ���� ���� ��ȣ �߰� ����;

	public:
		inline std::vector<unsigned int>& GetWinNum() { return m_winNums; }
		inline const unsigned int GetNumCount() const { return m_nums.size(); }
		inline const unsigned int GetRealNumCount() const { return m_tempDrawingNums.size(); }

		inline const LOTTO_SYSTEM_NUM GetNumInfo( unsigned int num ) const
		{
			if( num > 0 && num <= m_nums.size() )
				return m_nums.at( num-1 );

			return LOTTO_SYSTEM_NUM();
		}

		inline const unsigned int GetWinNumCount() const { return m_winNumCount; }
		inline const bool IsInitialize ( void ) const { return m_bInitialize; }

	private:
		std::vector<LOTTO_SYSTEM_NUM> m_nums;		// ��÷ �� ����;
		std::vector<unsigned int> m_drawingNums;	// ��÷�� �� �ٱ���;
		std::vector<unsigned int> m_tempDrawingNums;// ��÷�� ���� ������ �ٱ���;

		std::vector<unsigned int> m_winNums;		// ��÷ ��ȣ;
		unsigned int m_winNumCount;					// ��÷�� ����;

		/// ���ó�� ����ؾ� �Ѵ�;
		// ��÷�� ������ ���ڱ� �ٲ�ų� �ϴ� ��Ȳ�� ����Ͽ�;
		// ���ȭ ��Ű���� �ʴ´�;
		unsigned int m_count;						// ��÷�� ����;
		unsigned int m_mainNumCount;				// ���� ��ȣ ����;
		unsigned int m_bonusNumCount;				// ���ʽ� ��ȣ ����;

		bool m_bInitialize;
	};

}