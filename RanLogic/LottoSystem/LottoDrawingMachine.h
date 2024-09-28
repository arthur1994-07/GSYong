#pragma once

#include "./GLLottoDataType.h"

namespace LottoSystem
{

	// 로또 추첨기 로직;
	class CLottoDrawingMachine
	{
	public:
		CLottoDrawingMachine();
		~CLottoDrawingMachine();

	public:
		// 추첨기 번호 개수 초기화 및 추첨할 개수 초기화;
		void Initialize(
			unsigned int numCount,			// 번호 총 개수;
			unsigned int count,				// 추첨할 번호 총 개수;
			unsigned int mainNumCount,		// Main 번호 개수;
			unsigned int bonusNumCount );	// Bonus 번호 개수;

	public:
		/* 추첨 시작;
		 * 하나의 번호를 반환;
		 * 설정된 모든 개수가 다 반환 되었을 경우;
		 * 더 이상 숫자가 나오지 않는다 ( 0 반환 );	*/
		unsigned int StartDrawingMachine();

		// 추첨을 새롭게 시작하기 위해 초기화 한다;
		void ClearDrawingMachine();

		/* 몇개의 번호가 맞는지 확인한다;
		 * 서버에서는 DB에서 확인하기 때문에 사용하지 않는다; */
		void ConfirmWinningNum(
			IN const unsigned int* drawingNums,	// 추첨 번호;
			IN const unsigned int* winNums,		// 당첨 번호;
			OUT bool* drawingNumStates,			// 추첨 번호 맞는지 확인;
			OUT int& mainNumCount,				// 메인 번호 맞는 개수;
			OUT int& bonusNumCount );			// 보너스 번호 맞는 개수;

	public:
		// 확률을 위한 숫자의 개수 초기화;
		void InitializeNumCount();

		// 확률을 위한 숫자의 개수 변경;
		void ChangeNumCount(
			unsigned int num,					// 숫자;
			int delta );						// 변경 개수;

		// 변경하면서 동시에 확률이 적용되도록 DrawingNums을 변경한다;
		void ChangeNumCountAndDrawingNums(
			unsigned int num,					// 숫자;
			int delta );						// 변경 개수;

		// 숫자 누적;
		void AddAccumulateNum(
			unsigned int num,					// 숫자;
			unsigned int delta );				// 변경 개수;

		// 추첨 숫자 변경;
		void ChangeDrawingNums();

		// 누적 및 AddCount 세팅;
		void SetNumInfo(
			unsigned int num,					// 숫자;
			unsigned int accumulateCount,		// 누적 개수;
			unsigned int addCount );			// 확률을 위한 번호 추가 개수;

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
		std::vector<LOTTO_SYSTEM_NUM> m_nums;		// 추첨 공 정보;
		std::vector<unsigned int> m_drawingNums;	// 추첨할 공 바구니;
		std::vector<unsigned int> m_tempDrawingNums;// 추첨된 공을 제거한 바구니;

		std::vector<unsigned int> m_winNums;		// 당첨 번호;
		unsigned int m_winNumCount;					// 추첨한 개수;

		/// 상수처럼 취급해야 한다;
		// 추첨할 개수가 갑자기 바뀌거나 하는 상황을 대비하여;
		// 상수화 시키지는 않는다;
		unsigned int m_count;						// 추첨할 개수;
		unsigned int m_mainNumCount;				// 메인 번호 개수;
		unsigned int m_bonusNumCount;				// 보너스 번호 개수;

		bool m_bInitialize;
	};

}