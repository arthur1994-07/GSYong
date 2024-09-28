#pragma once

#include "./GLLottoDefine.h"

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Json/MinJson.h"

#include "../../enginelib/G-Logic/GLDefine.h"

#include <vector>

namespace LottoSystem
{

	// ��ü �ζ� �ý����� ���� ���� ��;
	struct LOTTO_GLOBAL_VALUE
	{
		unsigned int		nViewList;					// ���� ��÷ ��� Ȯ�� ����;
		unsigned int		nViewCharName;				// ��÷ ĳ���� ID ���� ���� ����;
		unsigned int		nAggregateTime;			// ��÷ �Ϸ� �Ŀ� ���� �ݾ��� �������� �ʴ� �ð�;
		unsigned int		nAccumulateMoneyTime;		// ���� ��÷�� ���� �ð�;

		LOTTO_GLOBAL_VALUE ()
			: nViewList ( 0 )
			, nViewCharName ( 0 )
			, nAggregateTime ( 0 )
			, nAccumulateMoneyTime ( 10 )
		{

		}
	};

	// �ζ� �ý����� ��÷ ����;
	struct LOTTO_DRAWING_DATE
	{
		CTime				sTime;					// ��÷ �ð�;
		unsigned int		nDrawingTime;			// ��÷ ��;
		EMDAY_OF_THE_WEEK	emDrawingDayOfTheWeek;	// ��÷ ����;

		LOTTO_DRAWING_DATE ()
			: emDrawingDayOfTheWeek ( DAY_OF_THE_WEEK_NONE )
			, nDrawingTime ( 0 )
		{

		}

		// ���ϰ� �ʸ� �������� �Ͽ�;
		// ������ �������� �� ���� ����� ������;
		// �ð��� ���Ѵ�;
		void GenerateTime ()
		{
			CTime sTodayTime = CTime::GetCurrentTime();
			CTime sTempTime(
				sTodayTime.GetYear(),
				sTodayTime.GetMonth(),
				sTodayTime.GetDay(),
				0, 0, 0 );

			// ������ ��÷���� ���;
			if ( sTempTime.GetDayOfWeek() == emDrawingDayOfTheWeek )
			{
				unsigned int nTodayTime =
					sTodayTime.GetHour()*3600 +
					sTodayTime.GetMinute()*60 +
					sTodayTime.GetSecond();

				// ��÷ �ð��� ������ ���;
				if ( nTodayTime > nDrawingTime )
					sTime = sTempTime + CTimeSpan( 7, 0, 0, nDrawingTime );
				// ��÷ �ð��� �������� ���;
				else
					sTime = sTempTime + CTimeSpan( 0, 0, 0, nDrawingTime );
			}
			// ������ ��÷���� �ƴѰ��;
			else
			{
				// ���÷κ��� ������ ���ϱ��� ���� ��¥;
				int nDelta = ( ( emDrawingDayOfTheWeek + 7 ) - sTempTime.GetDayOfWeek() ) % 7;
				sTime = sTempTime + CTimeSpan( nDelta, 0, 0, nDrawingTime );
			}
		}

		// �ʸ� �̿��Ͽ� �� �ð��� ����Ͽ� ��ȯ�Ѵ�;
		inline unsigned int GetHour () { return nDrawingTime / 3600; }
		inline unsigned int GetMinute () { return ( nDrawingTime%3600 ) / 60; }
		inline unsigned int GetSecond () { return ( nDrawingTime%3600 ) % 60; }
	};

	// �ζ� �¸� Ÿ��;
	struct LOTTO_WIN_TYPE
	{
		unsigned int		nMainNum;			// ���� ��ȣ ��ġ ����;
		unsigned int		nBonusNum;			// ���ʽ� ��ȣ ��ġ ����;
		int					nGiveMoneyFct;		// ���� �ݾ��� �ۼ�Ʈ;
		LONGLONG			llGiveMoney;		// ���� �ݾ�;
		EMGIVE_MONEY_TYPE	emGiveMoneyType;	// �� ���� ���;

		// ������ ����;
		SNATIVEID			sRewardItemID;
		DWORD				dwRewardItemCount;

		LOTTO_WIN_TYPE ( )
			: nMainNum ( 0 )
			, nBonusNum ( 0 )
			, emGiveMoneyType ( ACCUMULATE_MONEY )
			, nGiveMoneyFct ( 0 )
			, llGiveMoney ( 0 )
			, dwRewardItemCount ( 0 )
			, sRewardItemID ( SNATIVEID( false ) )
		{

		}
	};

	// �� �ζ� �ý��� ���� �������� ���� ��;
	struct LOTTO_SYSTEM_VALUE
	{
		unsigned int						nLottoTotal;				// ��ȣ �� ����;
		unsigned int						nDrawingTotal;				// ��÷ �� ����;
		unsigned int						nDrawingMain;				// �������� ��÷�� ����;
		unsigned int						nDrawingBonus;				// ���ʽ� ��÷ ����;
		int									nUseIndex;					// ����ϴ� �ζǽý��� Index;
		int									nDrawingType;				// ��÷ Ÿ��;
		unsigned int						nDrawingReadyTime;			// ��÷ ��� �ð�( �� );
		unsigned int						nDrawingTime;				// ��÷ �ð� ( �� );
		unsigned int						nLottoTicketMoney;			// �ζ� 1�� ���� ����;
		unsigned int						nBuyLevel;					// �ζ� ���� ���� �ּ� ����;
		unsigned int						nBuyMultiNum;				// �ִ� �ζ� ���� ����;
		int									nTicketFee;					// ���� �ݾ� ������ �ۼ�Ʈ;
		float								fEachOtherDrawingTime;		// ������ �ζ� ��ȣ ��÷ �ð�;
		bool								bUse;						// ��� ����;
		bool								bUseSort;					// ���� ��� ����;
		EMLOTTO_MONEY_TYPE					emMoneyType;				// ȭ�� Ÿ��;

		LONGLONG							llBuyInvenMoney;			// �ּ� ���� ���� ���� �ݾ�;

		std::vector< LOTTO_DRAWING_DATE >	vecDrawingDate;				// ��÷ ����;
		std::vector< LOTTO_WIN_TYPE >		vecWinType;					// �¸� Ÿ��;

		LOTTO_SYSTEM_VALUE ()
			: nLottoTotal ( 0 )
			, nDrawingTotal ( 0 )
			, nDrawingMain ( 0 )
			, nDrawingBonus ( 0 )
			, nUseIndex ( 0 )
			, nDrawingType ( 0 )
			, nDrawingReadyTime ( 600 )
			, nDrawingTime ( 600 )
			, nLottoTicketMoney ( 1000000 )
			, nBuyLevel ( 100 )
			, nBuyMultiNum ( 10 )
			, llBuyInvenMoney ( 10000000 )
			, nTicketFee ( 50 )
			, fEachOtherDrawingTime ( 0.f )
			, bUse ( false )
			, bUseSort ( false )
			, emMoneyType ( LOTTO_MONEY_TYPE_GAME_MONEY )
		{
			
		}

		LOTTO_SYSTEM_VALUE ( const LOTTO_SYSTEM_VALUE& me )
		{
			nLottoTotal = me.nLottoTotal;
			nDrawingTotal = me.nDrawingTotal;
			nDrawingMain = me.nDrawingMain;
			nDrawingBonus = me.nDrawingBonus;
			bUse = me.bUse;
			bUseSort = me.bUseSort;
			nUseIndex = me.nUseIndex;
			nDrawingType = me.nDrawingType;
			for( unsigned int i=0; i<me.vecDrawingDate.size(); ++i )
				vecDrawingDate.push_back( me.vecDrawingDate.at( i ) );
			nDrawingReadyTime = me.nDrawingReadyTime;
			nDrawingTime = me.nDrawingTime;
			emMoneyType = me.emMoneyType;
			nLottoTicketMoney = me.nLottoTicketMoney;
			nBuyLevel = me.nBuyLevel;
			nBuyMultiNum = me.nBuyMultiNum;
			llBuyInvenMoney = me.llBuyInvenMoney;
			nTicketFee = me.nTicketFee;
			for( unsigned int i=0; i<me.vecWinType.size(); ++i )
				vecWinType.push_back( me.vecWinType.at( i ) );
			fEachOtherDrawingTime = me.fEachOtherDrawingTime;
		}

		std::string MakeJson_WinTypeVector ()
		{
			// Json String Value;
			std::string strJsonValue;

			for ( unsigned int i=0; i<vecWinType.size(); ++i )
			{
				LottoSystem::LOTTO_WIN_TYPE winType = vecWinType.at( i );

				std::string strWinType;

				json_spirit::Object inputObj;
				inputObj.push_back( json_spirit::Pair( "mainNum", static_cast<int>( winType.nMainNum ) ) );
				inputObj.push_back( json_spirit::Pair( "bonusNum", static_cast<int>( winType.nBonusNum ) ) );
				inputObj.push_back( json_spirit::Pair( "giveMoneyType", static_cast<int>( winType.emGiveMoneyType ) ) );

				switch( winType.emGiveMoneyType )
				{
				case LottoSystem::ACCUMULATE_MONEY:
					inputObj.push_back( json_spirit::Pair( "giveMoney", winType.nGiveMoneyFct ) );
					break;
				case LottoSystem::FIXED_MONEY:
					inputObj.push_back( json_spirit::Pair( "giveMoney", static_cast<int>( winType.llGiveMoney ) ) );
					break;
				}

				inputObj.push_back( json_spirit::Pair( "giveItemMainID", static_cast<int>( winType.sRewardItemID.Mid() ) ) );
				inputObj.push_back( json_spirit::Pair( "giveItemSubID", static_cast<int>( winType.sRewardItemID.Sid() ) ) );
				inputObj.push_back( json_spirit::Pair( "giveItemCount", static_cast<int>( winType.dwRewardItemCount ) ) );

				strWinType = json_spirit::write( inputObj );

				strJsonValue += strWinType;
				strJsonValue += '|';
			}
			
			return strJsonValue;
		}

	};

	// �� �ζ� �ý��� ���� �������� ���� �� ( Client );
	struct LOTTO_SYSTEM_CLIENT_VALUE
	{
		unsigned int nLottoTotal;				// ��ȣ �� ����;
		unsigned int nDrawingTotal;				// ��÷ �� ����;
		unsigned int nDrawingMain;				// �������� ��÷�� ����;
		unsigned int nDrawingBonus;				// ���ʽ� ��÷ ����;
		bool bUse;								// ��� ����;
		bool bUseSort;							// ���� ��� ����;
		int useIndex;							// ����ϴ� �ζǽý��� Index;
		int drawingType;						// ��÷ Ÿ��;
		EMLOTTO_MONEY_TYPE moneyType;			// ȭ�� Ÿ��;
		unsigned int lottoTicketMoney;			// �ζ� 1�� ���� ����;
		unsigned int buyLevel;					// �ζ� ���� ���� �ּ� ����;
		unsigned int buyMultiNum;				// �ִ� �ζ� ���� ����;
		LONGLONG buyInvenMoney;					// �ּ� ���� ���� ���� �ݾ�;
		int winTypeCount;						// Win Type�� ����;
		LOTTO_WIN_TYPE sLottoWinType1st;		// 1�� ��������;

		LOTTO_SYSTEM_CLIENT_VALUE()
			: nLottoTotal( 0 )
			, nDrawingTotal( 0 )
			, nDrawingMain( 0 )
			, nDrawingBonus( 0 )
			, bUse( false )
			, bUseSort( false )
			, useIndex( 0 )
			, drawingType( 0 )
			, moneyType( LOTTO_MONEY_TYPE_GAME_MONEY )
			, lottoTicketMoney( 1000000 )
			, buyLevel( 100 )
			, buyMultiNum( 10 )
			, buyInvenMoney( 10000000 )
			, winTypeCount( 0 )
		{

		}

		void ServerToClient( const LOTTO_SYSTEM_VALUE& serverValue )
		{
			/*nLottoTotal = serverValue.nLottoTotal;
			nDrawingTotal = serverValue.nDrawingTotal;
			nDrawingMain = serverValue.nDrawingMain;
			nDrawingBonus = serverValue.nDrawingBonus;*/
			bUse = serverValue.bUse;
			bUseSort = serverValue.bUseSort;
			useIndex = serverValue.nUseIndex;
			drawingType = serverValue.nDrawingType;
			//moneyType = serverValue.moneyType;
			//lottoTicketMoney = serverValue.lottoTicketMoney;
			buyLevel = serverValue.nBuyLevel;
			buyMultiNum = serverValue.nBuyMultiNum;
			buyInvenMoney = serverValue.llBuyInvenMoney;
			//winTypeCount = serverValue.winTypes.size();
			if ( false == serverValue.vecWinType.empty() )
			{
				sLottoWinType1st = serverValue.vecWinType.at( 0 );
			}
		}
	};

	// �ζ� ���� ���;
	struct LOTTO_SYSTEM_BUY
	{
		EMLOTTO_BUY_TYPE lottoBuyType;						// �ζ� ���� Ÿ��;
		unsigned int drawingNum[ MAX_DRAWING_NUM_LIST ];	// �ζ� ��ȣ;
		bool drawingNumState[ MAX_DRAWING_NUM_LIST ];		// ��÷������ �̸� ������ �ִ�;
		unsigned int ranking;								// ��ŷ ������ �̸� ������ �ִ�;
		bool bConfirm;										// Ȯ���� �ζ��ΰ�?;
		DWORD dwUserNum;									// ĳ������ UserNum �� �����ΰ�?;
		DWORD dwDbNum;										// � ĳ������ �ζ��ΰ�?;
		DWORD dwServerGroup;								// ĳ������ �������� ����ΰ�?;
		char szChaName[ CHAR_SZNAME ];

		LOTTO_SYSTEM_BUY()
			: lottoBuyType( NONE_BUY_TYPE )
			, dwUserNum( 0 )
			, dwDbNum( 0 )
			, dwServerGroup( 0 )
			, ranking( 0 )
			, bConfirm( false )
		{
			memset( drawingNum, 0, sizeof(unsigned int) * MAX_DRAWING_NUM_LIST );
			memset( drawingNumState, false, sizeof(bool) * MAX_DRAWING_NUM_LIST );
			memset( szChaName, 0, sizeof(char) * CHAR_SZNAME );
		}

		LOTTO_SYSTEM_BUY( const LOTTO_SYSTEM_BUY& me )
		{
			lottoBuyType = me.lottoBuyType;
			for( int i=0; i<MAX_DRAWING_NUM_LIST; ++i )
			{
				drawingNum[ i ] = me.drawingNum[ i ];
				drawingNumState[ i ] = me.drawingNumState[ i ];
			}
			dwUserNum = me.dwUserNum;
			dwDbNum = me.dwDbNum;
			dwServerGroup = me.dwServerGroup;
			ranking = me.ranking;
			bConfirm = me.bConfirm;
			StringCchCopy( szChaName, CHAR_SZNAME, me.szChaName );
		}
	};

	// �ζ� ��ȣ;
	struct LOTTO_SYSTEM_LOTTO_NUM
	{
		unsigned int lottoNum[ MAX_DRAWING_NUM_LIST ];		// �ζ� ��ȣ;
		bool bSet;											// �ζ� ��ȣ ���� ����;

		LOTTO_SYSTEM_LOTTO_NUM()
			: bSet( false )
		{
			memset( lottoNum, 0, sizeof(unsigned int) * MAX_DRAWING_NUM_LIST );
		}

		LOTTO_SYSTEM_LOTTO_NUM( const LOTTO_SYSTEM_LOTTO_NUM& me )
			: bSet( me.bSet )
		{
			memset( lottoNum, 0, sizeof(unsigned int) * MAX_DRAWING_NUM_LIST );

			for( int i=0; i<MAX_DRAWING_NUM_LIST; ++i )
				lottoNum[ i ] = me.lottoNum[ i ];
		}

		void Clear()
		{
			memset( lottoNum, 0, sizeof(unsigned int) * MAX_DRAWING_NUM_LIST );

			bSet = false;
		}

		void SetLottoSetState( bool bSet )
		{
			this->bSet = bSet;
		}
	};

	// �ζ� ��÷�� ����;
	struct LOTTO_SYSTEM_WINNER_MAN
	{
		unsigned int ranking;		// ��ŷ;
		DWORD dwDbNum;				// ��ǥ ĳ����;
		DWORD dwServerGroup;		// ĳ������ ������;
		char szChaName[ CHAR_SZNAME ];
		LONGLONG winMoney;			// ��÷��;
		int winCount;				// ��÷�� ��;
		SNATIVEID sRewardItem;		// ��÷������;
		int nRewardItemCount;		// ��÷������ ����;

		LOTTO_SYSTEM_WINNER_MAN()
			: ranking( 0 )
			, dwDbNum( 0 )
			, dwServerGroup( 0 )
			, winMoney( 0 )
			, winCount( 0 )
			, sRewardItem( SNATIVEID( false ) )
			, nRewardItemCount( 1 )
		{
			memset( szChaName, 0, sizeof(char) * CHAR_SZNAME );
		}

		LOTTO_SYSTEM_WINNER_MAN( const LOTTO_SYSTEM_WINNER_MAN& me )
		{
			ranking = me.ranking;
			dwDbNum = me.dwDbNum;
			dwServerGroup = me.dwServerGroup;
			winMoney = me.winMoney;
			winCount = me.winCount;
			StringCchCopy( szChaName, CHAR_SZNAME, me.szChaName );
			sRewardItem = me.sRewardItem;
			nRewardItemCount = me.nRewardItemCount;
		}
	};

	// �ζ� ��÷�� ����Ʈ;
	struct LOTTO_SYSTEM_WINNER_MAN_LIST
	{
		std::vector<LOTTO_SYSTEM_WINNER_MAN> winnerManList;		// ��÷ Ƽ�� ����;
		std::vector<std::string> winnerManNameList;				// ��÷ Ƽ���� ��÷�� ��;

		LOTTO_SYSTEM_WINNER_MAN_LIST() { }

		LOTTO_SYSTEM_WINNER_MAN_LIST( const LOTTO_SYSTEM_WINNER_MAN_LIST& me )
		{
			for( unsigned int i=0; i<me.winnerManList.size(); ++i )
				winnerManList.push_back( me.winnerManList.at( i ) );

			for( unsigned int i=0; i<me.winnerManNameList.size(); ++i )
				winnerManNameList.push_back( me.winnerManNameList.at( i ) );
		}

		void Clear()
		{
			winnerManList.clear();
			winnerManNameList.clear();
		}
	};

	// �ζ� ��÷�� ��ŷ ���� ����;
	struct LOTTO_SYSTEM_WINNER_RANKING_INFO
	{
		unsigned int	ranking;			// �ζ� ����;
		unsigned int	count;				// �ζ� ������ ��÷�� ��;
		LONGLONG		winMoney;			// �ζ� ������ ���� �ݾ�;

		SNATIVEID		sRewardItemID;
		DWORD			dwRewardItemCount;

		LOTTO_SYSTEM_WINNER_RANKING_INFO ()
			: ranking ( 0 )
			, count ( 0 )
			, winMoney ( 0 )
			, dwRewardItemCount ( 0 )
		{

		}
	};

	// �ζ� ��÷�⸦ ���� ��ȣ;
	struct LOTTO_SYSTEM_NUM
	{
		unsigned int num;				// �ζ� ��ȣ;
		unsigned int drawingCount;		// ��ȣ�� ���� �� ����;
		int addCount;					// ��÷�⿡ �߰��� �� ����;

		LOTTO_SYSTEM_NUM( unsigned int _num = 0 )
			: num( _num )
			, drawingCount( 0 )
			, addCount( 1 )
		{

		}
	};

}