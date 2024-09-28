#pragma once

#include "./GLLottoDefine.h"

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Json/MinJson.h"

#include "../../enginelib/G-Logic/GLDefine.h"

#include <vector>

namespace LottoSystem
{

	// 전체 로또 시스템의 공통 설정 값;
	struct LOTTO_GLOBAL_VALUE
	{
		unsigned int		nViewList;					// 예전 당첨 기록 확인 개수;
		unsigned int		nViewCharName;				// 당첨 캐릭터 ID 글자 제한 개수;
		unsigned int		nAggregateTime;			// 추첨 완료 후에 누적 금액을 보여주지 않는 시간;
		unsigned int		nAccumulateMoneyTime;		// 누적 당첨금 갱신 시간;

		LOTTO_GLOBAL_VALUE ()
			: nViewList ( 0 )
			, nViewCharName ( 0 )
			, nAggregateTime ( 0 )
			, nAccumulateMoneyTime ( 10 )
		{

		}
	};

	// 로또 시스템의 추첨 일자;
	struct LOTTO_DRAWING_DATE
	{
		CTime				sTime;					// 추첨 시간;
		unsigned int		nDrawingTime;			// 추첨 초;
		EMDAY_OF_THE_WEEK	emDrawingDayOfTheWeek;	// 추첨 요일;

		LOTTO_DRAWING_DATE ()
			: emDrawingDayOfTheWeek ( DAY_OF_THE_WEEK_NONE )
			, nDrawingTime ( 0 )
		{

		}

		// 요일과 초를 바탕으로 하여;
		// 오늘을 기준으로 한 가장 가까운 요일의;
		// 시간을 구한다;
		void GenerateTime ()
		{
			CTime sTodayTime = CTime::GetCurrentTime();
			CTime sTempTime(
				sTodayTime.GetYear(),
				sTodayTime.GetMonth(),
				sTodayTime.GetDay(),
				0, 0, 0 );

			// 오늘이 추첨일인 경우;
			if ( sTempTime.GetDayOfWeek() == emDrawingDayOfTheWeek )
			{
				unsigned int nTodayTime =
					sTodayTime.GetHour()*3600 +
					sTodayTime.GetMinute()*60 +
					sTodayTime.GetSecond();

				// 추첨 시간이 지났을 경우;
				if ( nTodayTime > nDrawingTime )
					sTime = sTempTime + CTimeSpan( 7, 0, 0, nDrawingTime );
				// 추첨 시간이 안지났을 경우;
				else
					sTime = sTempTime + CTimeSpan( 0, 0, 0, nDrawingTime );
			}
			// 오늘이 추첨일이 아닌경우;
			else
			{
				// 오늘로부터 설정된 요일까지 남은 날짜;
				int nDelta = ( ( emDrawingDayOfTheWeek + 7 ) - sTempTime.GetDayOfWeek() ) % 7;
				sTime = sTempTime + CTimeSpan( nDelta, 0, 0, nDrawingTime );
			}
		}

		// 초를 이용하여 각 시간을 계산하여 반환한다;
		inline unsigned int GetHour () { return nDrawingTime / 3600; }
		inline unsigned int GetMinute () { return ( nDrawingTime%3600 ) / 60; }
		inline unsigned int GetSecond () { return ( nDrawingTime%3600 ) % 60; }
	};

	// 로또 승리 타입;
	struct LOTTO_WIN_TYPE
	{
		unsigned int		nMainNum;			// 메인 번호 일치 개수;
		unsigned int		nBonusNum;			// 보너스 번호 일치 개수;
		int					nGiveMoneyFct;		// 누적 금액의 퍼센트;
		LONGLONG			llGiveMoney;		// 고정 금액;
		EMGIVE_MONEY_TYPE	emGiveMoneyType;	// 돈 지급 방법;

		// 아이템 보상;
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

	// 각 로또 시스템 별로 독립적인 설정 값;
	struct LOTTO_SYSTEM_VALUE
	{
		unsigned int						nLottoTotal;				// 번호 총 개수;
		unsigned int						nDrawingTotal;				// 추첨 총 개수;
		unsigned int						nDrawingMain;				// 메인으로 추첨할 개수;
		unsigned int						nDrawingBonus;				// 보너스 추첨 개수;
		int									nUseIndex;					// 사용하는 로또시스템 Index;
		int									nDrawingType;				// 추첨 타입;
		unsigned int						nDrawingReadyTime;			// 추첨 대기 시간( 초 );
		unsigned int						nDrawingTime;				// 추첨 시간 ( 초 );
		unsigned int						nLottoTicketMoney;			// 로또 1장 구매 가격;
		unsigned int						nBuyLevel;					// 로또 구입 가능 최소 레벨;
		unsigned int						nBuyMultiNum;				// 최대 로또 구입 개수;
		int									nTicketFee;					// 누적 금액 수수료 퍼센트;
		float								fEachOtherDrawingTime;		// 각각의 로또 번호 추첨 시간;
		bool								bUse;						// 사용 여부;
		bool								bUseSort;					// 정렬 사용 여부;
		EMLOTTO_MONEY_TYPE					emMoneyType;				// 화폐 타입;

		LONGLONG							llBuyInvenMoney;			// 최소 구입 가능 소유 금액;

		std::vector< LOTTO_DRAWING_DATE >	vecDrawingDate;				// 추첨 일자;
		std::vector< LOTTO_WIN_TYPE >		vecWinType;					// 승리 타입;

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

	// 각 로또 시스템 별로 독립적인 설정 값 ( Client );
	struct LOTTO_SYSTEM_CLIENT_VALUE
	{
		unsigned int nLottoTotal;				// 번호 총 개수;
		unsigned int nDrawingTotal;				// 추첨 총 개수;
		unsigned int nDrawingMain;				// 메인으로 추첨할 개수;
		unsigned int nDrawingBonus;				// 보너스 추첨 개수;
		bool bUse;								// 사용 여부;
		bool bUseSort;							// 정렬 사용 여부;
		int useIndex;							// 사용하는 로또시스템 Index;
		int drawingType;						// 추첨 타입;
		EMLOTTO_MONEY_TYPE moneyType;			// 화폐 타입;
		unsigned int lottoTicketMoney;			// 로또 1장 구매 가격;
		unsigned int buyLevel;					// 로또 구입 가능 최소 레벨;
		unsigned int buyMultiNum;				// 최대 로또 구입 개수;
		LONGLONG buyInvenMoney;					// 최소 구입 가능 소유 금액;
		int winTypeCount;						// Win Type의 개수;
		LOTTO_WIN_TYPE sLottoWinType1st;		// 1등 보상정보;

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

	// 로또 구매 목록;
	struct LOTTO_SYSTEM_BUY
	{
		EMLOTTO_BUY_TYPE lottoBuyType;						// 로또 구매 타입;
		unsigned int drawingNum[ MAX_DRAWING_NUM_LIST ];	// 로또 번호;
		bool drawingNumState[ MAX_DRAWING_NUM_LIST ];		// 당첨정보를 미리 가지고 있다;
		unsigned int ranking;								// 랭킹 정보도 미리 가지고 있다;
		bool bConfirm;										// 확인한 로또인가?;
		DWORD dwUserNum;									// 캐릭터의 UserNum 은 무엇인가?;
		DWORD dwDbNum;										// 어떤 캐릭터의 로또인가?;
		DWORD dwServerGroup;								// 캐릭터의 서버군은 어디인가?;
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

	// 로또 번호;
	struct LOTTO_SYSTEM_LOTTO_NUM
	{
		unsigned int lottoNum[ MAX_DRAWING_NUM_LIST ];		// 로또 번호;
		bool bSet;											// 로또 번호 세팅 여부;

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

	// 로또 당첨자 정보;
	struct LOTTO_SYSTEM_WINNER_MAN
	{
		unsigned int ranking;		// 랭킹;
		DWORD dwDbNum;				// 대표 캐릭터;
		DWORD dwServerGroup;		// 캐릭터의 서버군;
		char szChaName[ CHAR_SZNAME ];
		LONGLONG winMoney;			// 당첨금;
		int winCount;				// 당첨자 수;
		SNATIVEID sRewardItem;		// 당첨아이템;
		int nRewardItemCount;		// 당첨아이템 갯수;

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

	// 로또 당첨자 리스트;
	struct LOTTO_SYSTEM_WINNER_MAN_LIST
	{
		std::vector<LOTTO_SYSTEM_WINNER_MAN> winnerManList;		// 당첨 티켓 정보;
		std::vector<std::string> winnerManNameList;				// 당첨 티켓의 당첨자 명;

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

	// 로또 당첨자 랭킹 관련 정보;
	struct LOTTO_SYSTEM_WINNER_RANKING_INFO
	{
		unsigned int	ranking;			// 로또 순위;
		unsigned int	count;				// 로또 순위의 당첨자 수;
		LONGLONG		winMoney;			// 로또 순위에 따른 금액;

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

	// 로또 추첨기를 위한 번호;
	struct LOTTO_SYSTEM_NUM
	{
		unsigned int num;				// 로또 번호;
		unsigned int drawingCount;		// 번호를 뽑은 총 개수;
		int addCount;					// 추첨기에 추가로 들어갈 개수;

		LOTTO_SYSTEM_NUM( unsigned int _num = 0 )
			: num( _num )
			, drawingCount( 0 )
			, addCount( 1 )
		{

		}
	};

}