#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Memory/SmartPtr.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"

#include "../../RanLogic/GLogicFile.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "./GLLottoGlobalData.h"

namespace LottoSystem
{

	// 로또 스크립트;
	const bool GLLottoGlobalData::LoadScriptFile ( const std::string& _strFileName )
	{
		sc::lua_init();

		if ( !sc::lua_doFile( _strFileName ) )
			return false;


		/// Lotto System 의 공통 정보;
		LuaPlus::LuaObject basicGroup = sc::lua_getGlobalFromName( "Basic" );
		if ( basicGroup.IsNil() )
		{
			GASSERT( 0 && "[ Lotto System ] [ Failed Load 'Lotto Basic Group' ]" );

			return false;
		}

		m_sLottoGlobalValue.nViewList = sc::GetIntegerByName( basicGroup, "view_list_num" );
		m_sLottoGlobalValue.nViewCharName = sc::GetIntegerByName( basicGroup, "view_char_num" );
		m_sLottoGlobalValue.nAggregateTime = sc::GetIntegerByName( basicGroup, "aggregate_time" );
		m_sLottoGlobalValue.nAccumulateMoneyTime = sc::GetIntegerByName( basicGroup, "accumulate_money_time" );


		/// Lotto System 각각의 독립적인 정보;
		LuaPlus::LuaObject lottoSystemListGroup = sc::lua_getGlobalFromName( "LottoSystem" );
		if ( lottoSystemListGroup.IsNil() )
		{
			GASSERT( 0 && "[ Lotto System ] [ Failed Load 'Lotto System List Group' ]" );

			return false;
		}

		for ( LuaPlus::LuaTableIterator iter( lottoSystemListGroup ); iter; iter.Next() )
		{
			// List 개수만큼 로또 시스템을 추가한다;
			LuaPlus::LuaObject lottoSystemGroup( iter.GetValue() );

			LOTTO_SYSTEM_VALUE lottoSystemValue;
			lottoSystemValue.nLottoTotal = sc::GetIntegerByName( lottoSystemGroup, "lotto_total_num" );
			lottoSystemValue.nDrawingTotal = sc::GetIntegerByName( lottoSystemGroup, "drawing_total_num" );
			lottoSystemValue.nDrawingMain = sc::GetIntegerByName( lottoSystemGroup, "drawing_main_num" );
			lottoSystemValue.nDrawingBonus = sc::GetIntegerByName( lottoSystemGroup, "drawing_bonus_num" );
			lottoSystemValue.bUse = static_cast<bool>( sc::GetIntegerByName( lottoSystemGroup, "use" ) );
			lottoSystemValue.bUseSort = static_cast<bool>( sc::GetIntegerByName( lottoSystemGroup, "use_sort" ) );
			lottoSystemValue.nDrawingType = sc::GetIntegerByName( lottoSystemGroup, "drawingType" );

			for( LuaPlus::LuaTableIterator drawingDataIter( lottoSystemGroup.GetByName( "drawing_date" ) ); drawingDataIter; drawingDataIter.Next() )
			{
				LuaPlus::LuaTableIterator drawingDateNode( drawingDataIter.GetValue() );
				LOTTO_DRAWING_DATE drawingDate;
				drawingDate.emDrawingDayOfTheWeek =
					static_cast<EMDAY_OF_THE_WEEK>(
					sc::GetIntegerByObject( drawingDateNode.GetValue() ) );
				drawingDateNode.Next();
				unsigned int hour = sc::GetIntegerByObject( drawingDateNode.GetValue() );
				drawingDateNode.Next();
				unsigned int minute = sc::GetIntegerByObject( drawingDateNode.GetValue() );
				drawingDateNode.Next();
				unsigned int second = sc::GetIntegerByObject( drawingDateNode.GetValue() );
				drawingDateNode.Next();
				drawingDate.nDrawingTime = hour*3600 + minute*60 + second;

				lottoSystemValue.vecDrawingDate.push_back( drawingDate );
			}

			lottoSystemValue.nDrawingReadyTime = sc::GetIntegerByName( lottoSystemGroup, "drawing_ready_time" );
			lottoSystemValue.nDrawingTime = sc::GetIntegerByName( lottoSystemGroup, "drawing_time" );
			lottoSystemValue.emMoneyType = static_cast<EMLOTTO_MONEY_TYPE>( sc::GetIntegerByName( lottoSystemGroup, "money_type" ) );
			lottoSystemValue.nLottoTicketMoney = sc::GetIntegerByName( lottoSystemGroup, "lotto_ticket_money" );
			lottoSystemValue.nBuyLevel = sc::GetIntegerByName( lottoSystemGroup, "buy_level" );
			lottoSystemValue.nBuyMultiNum = sc::GetIntegerByName( lottoSystemGroup, "buy_multi_num" );
			lottoSystemValue.llBuyInvenMoney = sc::GetIntegerByName( lottoSystemGroup, "buy_inven_money" );
			lottoSystemValue.nTicketFee = sc::GetIntegerByName( lottoSystemGroup, "ticket_fee" );
			if( lottoSystemValue.nTicketFee < 0 )
				lottoSystemValue.nTicketFee = 0;
			else if( lottoSystemValue.nTicketFee > 100 )
				lottoSystemValue.nTicketFee = 100;

			// 승리 타입 설정 필요;
			int AccGiveMoneyFct = 0;
			unsigned int WinTypeCount = 0;
			for( LuaPlus::LuaTableIterator winTypeIter( lottoSystemGroup.GetByName( "win_type" ) ); winTypeIter; winTypeIter.Next() )
			{
				if( WinTypeCount > MAX_RANKING_NUM )
				{
					GASSERT( 0 && "Load 'win_Type' Error!! win_type count error!" );
					return false;
				}

				LuaPlus::LuaTableIterator winTypeGroupNode( winTypeIter.GetValue() );
				LOTTO_WIN_TYPE tempWinType;
				tempWinType.nMainNum = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
				winTypeGroupNode.Next();
				tempWinType.nBonusNum = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
				winTypeGroupNode.Next();
				tempWinType.emGiveMoneyType = static_cast<EMGIVE_MONEY_TYPE>( sc::GetIntegerByObject( winTypeGroupNode.GetValue() ) );
				winTypeGroupNode.Next();

				switch( tempWinType.emGiveMoneyType )
				{
				case ACCUMULATE_MONEY:
					tempWinType.nGiveMoneyFct = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
					if( tempWinType.nGiveMoneyFct < 0 )
						tempWinType.nGiveMoneyFct = 0;
					else if( tempWinType.nGiveMoneyFct > 100 )
						tempWinType.nGiveMoneyFct = 100;

					if( AccGiveMoneyFct + tempWinType.nGiveMoneyFct > 100 )
					{
						tempWinType.nGiveMoneyFct =
							tempWinType.nGiveMoneyFct -
							( AccGiveMoneyFct + tempWinType.nGiveMoneyFct - 100 );
					}
					AccGiveMoneyFct += tempWinType.nGiveMoneyFct;

					if( AccGiveMoneyFct > 100 || AccGiveMoneyFct < 0 )
					{
						GASSERT( 0 && "Win Type GiveMoneyFct Error!!" );
						return false;
					}
					break;

				case FIXED_MONEY:
					tempWinType.llGiveMoney = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
					break;
				}
				winTypeGroupNode.Next();

				// 아이템;
				tempWinType.sRewardItemID.wMainID = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
				winTypeGroupNode.Next();

				tempWinType.sRewardItemID.wSubID = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
				winTypeGroupNode.Next();

				tempWinType.dwRewardItemCount = sc::GetIntegerByObject( winTypeGroupNode.GetValue() );
				winTypeGroupNode.Next();

				lottoSystemValue.vecWinType.push_back( tempWinType );
				WinTypeCount++;
			}

			float fDevide = static_cast<float>( lottoSystemValue.nDrawingTotal-1 );
			if( fDevide == 0.f )
				lottoSystemValue.fEachOtherDrawingTime = 0xffffff;
			else
			{
				// 여유시간 12초를 둔다;
				lottoSystemValue.fEachOtherDrawingTime = floor( ( lottoSystemValue.nDrawingTime-12.f )/fDevide );
			}

			// Lotto System 추가;
			m_lottoSystemValues[ m_systemCount ] = lottoSystemValue;

			// Index 구하기;
			if( m_systemCount > 0 &&
				m_lottoSystemValues[ m_systemCount-1 ].bUse )
			{
				m_lottoSystemValues[ m_systemCount ].nUseIndex =
					m_lottoSystemValues[ m_systemCount-1 ].nUseIndex+1;
			}

			m_systemCount++;
		}


		sc::lua_destroyState();

		return true;
	}

}
