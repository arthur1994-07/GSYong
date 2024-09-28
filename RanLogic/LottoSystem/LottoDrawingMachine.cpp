#include "../pch.h"

#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "./LottoDrawingMachine.h"

namespace LottoSystem
{

	CLottoDrawingMachine::CLottoDrawingMachine()
		: m_winNumCount( 0 )
		, m_count( 0 )
		, m_mainNumCount( 0 )
		, m_bonusNumCount( 0 )
		, m_bInitialize ( false )
	{
	}

	CLottoDrawingMachine::~CLottoDrawingMachine() { }

	// 추첨기 번호 개수 초기화 및 추첨할 개수 초기화;
	void CLottoDrawingMachine::Initialize(
		unsigned int numCount,
		unsigned int count,
		unsigned int mainNumCount,
		unsigned int bonusNumCount )
	{
		if ( numCount == 0 )
		{
			sc::writeLogError( "Lotto Drawing Machine numCount 0. ( Initialize )" );
		}

		m_nums.clear();

		for( unsigned int i=0; i<numCount; ++i )
			m_nums.push_back( LOTTO_SYSTEM_NUM( i+1 ) );

		m_count = count;
		m_mainNumCount = mainNumCount;
		m_bonusNumCount = bonusNumCount;

		m_bInitialize = true;
	}
	
	// 추첨 시작;
	unsigned int CLottoDrawingMachine::StartDrawingMachine()
	{
		unsigned int returnNum = 0;

		if( m_winNumCount >= m_count )
			return returnNum;

		if ( GetRealNumCount() <= 0 )
		{
			sc::writeLogError( "Drawing Machine Error!!!! Don't Setting!!" );
			return returnNum;
		}

		while( true )
		{
			bool bExit = true;

			unsigned int num = static_cast<unsigned int>(
				sc::Random::getInstance().GetValue( 0, GetRealNumCount()-1 ) );

			if( num >= 0 && num < m_tempDrawingNums.size() )
			{
				// 번호 뽑기;
				unsigned int drawingNum = m_tempDrawingNums.at( num );

				// 중복 확인;
				for( unsigned int i=0; i<m_winNums.size(); ++i )
				{
					if( m_winNums.at( i ) == drawingNum )
						bExit = false;
				}

				// 중복된 값이 나오지 않았다면;
				if( bExit )
				{
					m_winNums.push_back( drawingNum );

					// 번호 뽑은 횟수 누적;
					if( drawingNum > 0 && drawingNum <= m_nums.size() )
					{
						LOTTO_SYSTEM_NUM& numInfo = m_nums.at( drawingNum-1 );
						numInfo.drawingCount++;
					}

					// 반환 및 Count 업;
					returnNum = drawingNum;
					m_winNumCount++;

					// 뽑힌 값 제거;
					m_tempDrawingNums.clear();
					for( unsigned int i=0; i<m_drawingNums.size(); ++i )
					{
						bool bDuplicate = false;
						unsigned int tempNum = m_drawingNums.at( i );
						for( unsigned int j=0; j<m_winNums.size(); ++j )
						{
							if( m_winNums.at( j ) == tempNum )
								bDuplicate = true;
						}
						
						if( !bDuplicate )
							m_tempDrawingNums.push_back( tempNum );
					}

					return returnNum;
				}
			}
			else
				return 0;
		}

		return returnNum;
	}

	// 추첨을 새롭게 시작하기 위해 초기화 한다;
	void CLottoDrawingMachine::ClearDrawingMachine()
	{
		m_winNumCount = 0;
		m_winNums.clear();
		m_tempDrawingNums.clear();
		for( unsigned int i=0; i<m_drawingNums.size(); ++i )
			m_tempDrawingNums.push_back( m_drawingNums.at( i ) );
	}

	// 몇개의 번호가 맞는지 확인한다;
	void CLottoDrawingMachine::ConfirmWinningNum(
		const unsigned int* drawingNums,
		const unsigned int* winNums,
		bool* drawingNumStates,
		int& mainNumCount,
		int& bonusNumCount )
	{
		// m_count는 추첨하는 전체개수와 같다;
		for( unsigned int i=0; i<m_count; ++i )
		{
			if( winNums[ i ] == 0 )
				break;

			for( unsigned int j=0; j<m_mainNumCount; ++j )
			{
				if( drawingNums[ j ] == 0 )
					break;

				if( winNums[ i ] == drawingNums[ j ] )
				{
					if( i < m_mainNumCount )
					{
						mainNumCount++;
						drawingNumStates[ j ] = true;
					}
					else
					{
						// 보너스 번호는 2등이 아니면 무조건 틀리게 표시됨;
						// 보너스 번호 적용받는 번호만 받기때문임;
						bonusNumCount++;

						// 2등일 경우;
						if( mainNumCount == m_mainNumCount-m_bonusNumCount )
							drawingNumStates[ j ] = true;
						// 2등이 아닐 경우;
						else
							drawingNumStates[ j ] = false;
					}
				}
			}
		}
	}

	// 확률을 위한 숫자의 개수 초기화;
	void CLottoDrawingMachine::InitializeNumCount()
	{
		for( unsigned int i=0; i<m_nums.size(); ++i )
		{
			m_nums.at( i ).addCount = 1;
		}

		// 개수가 변경될 경우 DrawingNums 다시 세팅;
		ChangeDrawingNums();
	}

	// 확률을 위한 숫자의 개수 변경;
	void CLottoDrawingMachine::ChangeNumCount( unsigned int num, int delta )
	{
		unsigned int realNum = num-1;
		if( realNum >= 0 && realNum < m_nums.size() )
		{
			m_nums.at( realNum ).addCount += delta;
			if( m_nums.at( realNum ).addCount <= 0 )
				m_nums.at( realNum ).addCount = 1;
		}
	}

	void CLottoDrawingMachine::ChangeNumCountAndDrawingNums( unsigned int num, int delta )
	{
		ChangeNumCount( num, delta );
		ChangeDrawingNums();
	}

	// 숫자 누적;
	void CLottoDrawingMachine::AddAccumulateNum( unsigned int num, unsigned int delta )
	{
		unsigned int realNum = num-1;
		if( realNum >= 0 && realNum < m_nums.size() )
		{
			m_nums.at( realNum ).drawingCount += delta;
		}
	}

	// 추첨 숫자 변경;
	void CLottoDrawingMachine::ChangeDrawingNums()
	{
		if ( m_nums.size() <= 0 )
		{
			sc::writeLogError( "Lotto Drawing Machine Num Size 0." );
			return;
		}

		m_drawingNums.clear();
		m_tempDrawingNums.clear();
		for( unsigned int i=0; i<m_nums.size(); ++i )
		{
			LOTTO_SYSTEM_NUM tempNum = m_nums.at( i );
			for( int j=0; j<tempNum.addCount; ++j )
			{
				m_drawingNums.push_back( tempNum.num );
				m_tempDrawingNums.push_back( tempNum.num );
			}
		}
	}

	void CLottoDrawingMachine::SetNumInfo(
		unsigned int num,
		unsigned int accumulateCount,
		unsigned int addCount )
	{
		unsigned int realNum = num-1;
		if( realNum >= 0 && realNum < m_nums.size() )
		{
			m_nums.at( realNum ).addCount = addCount;
			m_nums.at( realNum ).drawingCount = accumulateCount;

			if( m_nums.at( realNum ).addCount < 0 )
				m_nums.at( realNum ).addCount = 1;

			if( m_nums.at( realNum ).drawingCount < 0 )
				m_nums.at( realNum ).drawingCount = 0;
		}
	}

}