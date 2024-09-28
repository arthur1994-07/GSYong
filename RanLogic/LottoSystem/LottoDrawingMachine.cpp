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

	// ��÷�� ��ȣ ���� �ʱ�ȭ �� ��÷�� ���� �ʱ�ȭ;
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
	
	// ��÷ ����;
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
				// ��ȣ �̱�;
				unsigned int drawingNum = m_tempDrawingNums.at( num );

				// �ߺ� Ȯ��;
				for( unsigned int i=0; i<m_winNums.size(); ++i )
				{
					if( m_winNums.at( i ) == drawingNum )
						bExit = false;
				}

				// �ߺ��� ���� ������ �ʾҴٸ�;
				if( bExit )
				{
					m_winNums.push_back( drawingNum );

					// ��ȣ ���� Ƚ�� ����;
					if( drawingNum > 0 && drawingNum <= m_nums.size() )
					{
						LOTTO_SYSTEM_NUM& numInfo = m_nums.at( drawingNum-1 );
						numInfo.drawingCount++;
					}

					// ��ȯ �� Count ��;
					returnNum = drawingNum;
					m_winNumCount++;

					// ���� �� ����;
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

	// ��÷�� ���Ӱ� �����ϱ� ���� �ʱ�ȭ �Ѵ�;
	void CLottoDrawingMachine::ClearDrawingMachine()
	{
		m_winNumCount = 0;
		m_winNums.clear();
		m_tempDrawingNums.clear();
		for( unsigned int i=0; i<m_drawingNums.size(); ++i )
			m_tempDrawingNums.push_back( m_drawingNums.at( i ) );
	}

	// ��� ��ȣ�� �´��� Ȯ���Ѵ�;
	void CLottoDrawingMachine::ConfirmWinningNum(
		const unsigned int* drawingNums,
		const unsigned int* winNums,
		bool* drawingNumStates,
		int& mainNumCount,
		int& bonusNumCount )
	{
		// m_count�� ��÷�ϴ� ��ü������ ����;
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
						// ���ʽ� ��ȣ�� 2���� �ƴϸ� ������ Ʋ���� ǥ�õ�;
						// ���ʽ� ��ȣ ����޴� ��ȣ�� �ޱ⶧����;
						bonusNumCount++;

						// 2���� ���;
						if( mainNumCount == m_mainNumCount-m_bonusNumCount )
							drawingNumStates[ j ] = true;
						// 2���� �ƴ� ���;
						else
							drawingNumStates[ j ] = false;
					}
				}
			}
		}
	}

	// Ȯ���� ���� ������ ���� �ʱ�ȭ;
	void CLottoDrawingMachine::InitializeNumCount()
	{
		for( unsigned int i=0; i<m_nums.size(); ++i )
		{
			m_nums.at( i ).addCount = 1;
		}

		// ������ ����� ��� DrawingNums �ٽ� ����;
		ChangeDrawingNums();
	}

	// Ȯ���� ���� ������ ���� ����;
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

	// ���� ����;
	void CLottoDrawingMachine::AddAccumulateNum( unsigned int num, unsigned int delta )
	{
		unsigned int realNum = num-1;
		if( realNum >= 0 && realNum < m_nums.size() )
		{
			m_nums.at( realNum ).drawingCount += delta;
		}
	}

	// ��÷ ���� ����;
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