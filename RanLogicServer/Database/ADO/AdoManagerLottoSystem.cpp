#include "pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Json/MinJson.h"
#include "./AdoManager.h"

#include "../../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "../../LottoSystem/GLLottoSystemManAgent.h"
#include "../../LottoSystem/LottoSystem.h"

#include "../../Character/GLCharAG.h"

#include "../../AgentServer/GLAgentServer.h"

#include <boost/tokenizer.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace LottoSystem;

namespace db
{
	// Load �ζ� �ý���;
	int AdoManager::LottoSystemLoad( unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		// 1. ���� Turn Num ã��;
		sc::db::AdoExt adoTurnNum( m_GameDBConnString );
		APPEND_IPARAM_BIGINT( adoTurnNum, "@SystemID", systemId );

		if( !adoTurnNum.Execute4Cmd( "dbo.sp_LottoSystemCurTurnSelect", adCmdStoredProc ) )
			return sc::db::DB_ERROR;

		unsigned int curTurnNum = 0;

		// ã�� ���ߴٴ� ���� �ϳ��� ���ٴ� ���̱� ������ ��÷�⸦ �����Ѵ�;
		if( adoTurnNum.GetEOF() || adoTurnNum.GetBOF() )
		{
			LOTTO_SYSTEM_VALUE sValue = GLLottoGlobalData::Instance()
				->GetLottoSystemValue( systemId );

			// ��÷�� ����;
			for( unsigned int i=0; i<sValue.nLottoTotal; ++i )
				LottoDrawingMachineNumSave( i+1, 0, 1, systemId, false );

			pLottoSystem->UpdateDrawingMachine();
		}
		else
		{
			adoTurnNum.GetCollect( "CurTurnNum", curTurnNum );
		}

		// ���� �� ����;
		pLottoSystem->SetCurTurnNum( curTurnNum );

		// 2. ���� Turn Save;
		LottoSystemTurnSave( curTurnNum, systemId );

		return nRet;
	}

	// �ζ� ��÷�� ���� Select;
	int AdoManager::LottoDrawingMachineNumSelect(
		unsigned int lottoNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@LottoNum", lottoNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoDrawingMachineNumSelect", adCmdStoredProc ) )
		{
			return sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return sc::db::DB_ERROR;

		unsigned int accumulateCount = 0;
		unsigned int addCount = 0;

		ado.GetCollect( "AccumulateCount", accumulateCount );
		ado.GetCollect( "AddCount", addCount );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		CLottoDrawingMachine* pMachine =
			pLottoSystem->GetDrawingMachine();
		if( !pMachine )
			return sc::db::DB_ERROR;

		pMachine->SetNumInfo( lottoNum, accumulateCount, addCount );

		return nRet;
	}

	int AdoManager::LottoSystemCurTurnSelect( unsigned int lottoSystemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", lottoSystemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoSystemCurTurnSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return nRet;

		unsigned int curTurnNum = 0;

		ado.GetCollect( "CurTurnNum", curTurnNum );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( lottoSystemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		pLottoSystem->SetCurTurnNum( curTurnNum );

		return nRet;
	}

	// �ζ� ���� ��÷�� Select;
	int AdoManager::LottoAccumulateMoneySelect(
		DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoAccumulateMoneySelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return nRet;

		LONGLONG accumulateMoney = 0;

		ado.GetCollect( "AccumulateMoney", accumulateMoney );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		if( pLottoSystem->GetCurTurnNum() == turnNum )
			pLottoSystem->SetAccumulateMoney( accumulateMoney );
		else
		{
			// �ƿ� �ٸ� ȸ���� ��� ��Ŷ�� ������;
			if( dwDbNum != 0 )
			{
				// ���� ��Ŷ;
				GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC lottoMsgAC(
					systemId, turnNum, accumulateMoney );
				lottoMsgAC.m_emAC = LottoSystem::EMLOTTO_REQ_ACCUMULATE_MONEY_AC_OK;

				// ����;
				GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent( dwDbNum, dwServerGroup, &lottoMsgAC );
			}
		}

		return nRet;
	}

	// �ζ� ��÷��ȣ Select;
	int AdoManager::LottoWinNumSelect( DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoWinNumSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return nRet;

		std::string strWinNum = "";

		ado.GetCollect( "WinNum", strWinNum );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( systemId );

		// Setting;
		if( strWinNum != "" )
		{
			LOTTO_SYSTEM_LOTTO_NUM winNum;

			boost::char_separator<char> sep("|");
			tokenizer tok( strWinNum, sep );

			unsigned int count = 0;
			for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
			{
				if( count >= 0 && count < LottoSystem::MAX_DRAWING_NUM_LIST )
					winNum.lottoNum[ count++ ] = atoi( (*it).c_str() );
			}

			if( count >= sValue.nDrawingMain )
			{
				winNum.SetLottoSetState( true );
			
				if( pLottoSystem->GetCurTurnNum() == turnNum )
				{
					pLottoSystem->SetCurWinNum( winNum );
				}
				else 
				{
					if( pLottoSystem->GetCurTurnNum()-1 == turnNum )
					{
						pLottoSystem->SetPreWinNum( winNum );
					}
					
					// ���� ȸ���̰ų� �ƿ� �ٸ� ȸ���� ��� ��Ŷ�� ������;
					if( dwDbNum != 0 )
					{
						GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC lottoMsgAC(
							systemId, turnNum );
						lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
						lottoMsgAC.m_winnerNum = winNum;

						// ����;
						GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
							dwDbNum, dwServerGroup, &lottoMsgAC );
					}
				}
			}
		}
		else
		{
			CLottoDrawingMachine* pDrawingMachine
				= pLottoSystem->GetDrawingMachine();
			if( !pDrawingMachine )
				return sc::db::DB_ERROR;

			pLottoSystem->InitializeDrawingMachine();
			for( int i=0; i<pLottoSystem->GetDrawingTotalNum(); ++i )
			{
				unsigned int nNum = pDrawingMachine->StartDrawingMachine();
				if ( 0 == nNum )
				{
					pDrawingMachine->ClearDrawingMachine();
					return sc::db::DB_ERROR;
				}
			}
			if( pLottoSystem->GetCurTurnNum() == turnNum )
				pLottoSystem->UpdateWinNum( pDrawingMachine->GetWinNum() );
			else if( pLottoSystem->GetCurTurnNum()-1 == turnNum )
				pLottoSystem->UpdatePreWinNum( pDrawingMachine->GetWinNum() );

			pDrawingMachine->ClearDrawingMachine();
		}


		return nRet;
	}

	// �ζ� ���� ����Ʈ Select;
	int AdoManager::LottoCharBuyListSelect(
		DWORD dwDbNum,
		DWORD dwServerGroup, 
		unsigned int turnNum,
		unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( ado, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoCharBuyListSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
		{
			// �ƹ��͵� ���������� �ϳ��� ��Ŷ�� �������ν�;
			// ������Ʈ�� �̷�������� �Ѵ�;
			GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC lottoMsgAC(
				systemId, turnNum );
			lottoMsgAC.m_emAC = LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_OK;

			GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
				dwDbNum, dwServerGroup, &lottoMsgAC );

			GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC countMsgAC(
				systemId, turnNum, 0 );

			GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
				dwDbNum, dwServerGroup, &countMsgAC );

			return nRet;
		}

		int nBuyCount = 0;

		int buyType = 0;
		std::string strDrawingNum = "";
		int ranking = 0;
		int confirmTicket = 0;
		do
		{
			ado.GetCollect( "BuyType", buyType );
			ado.GetCollect( "DrawingNum", strDrawingNum );
			ado.GetCollect( "Ranking", ranking );
			ado.GetCollect( "ConfirmTicket", confirmTicket );
			

			LOTTO_SYSTEM_BUY tempBuy;
			tempBuy.lottoBuyType = static_cast<EMLOTTO_BUY_TYPE>( buyType );
			tempBuy.dwDbNum = dwDbNum;
			tempBuy.ranking = ranking;
			tempBuy.bConfirm = ( confirmTicket == 1 ) ? true : false;

			{
				boost::char_separator<char> sep("|");
				tokenizer tok( strDrawingNum, sep );

				int count = 0;
				for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
				{
					if( count >= 0 && count < LottoSystem::MAX_DRAWING_NUM_LIST )
						tempBuy.drawingNum[ count++ ] = atoi( (*it).c_str() );
				}
			}

			if( dwDbNum != 0 )
			{
				// ��Ŷ;
				GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC lottoMsgAC(
					systemId, turnNum );
				lottoMsgAC.m_emAC = LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_OK;
				lottoMsgAC.m_lotto = tempBuy;

				// ����;
				GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
					dwDbNum, dwServerGroup, &lottoMsgAC );

				nBuyCount++;
			}

		} while( ado.Next() );

		GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC lottoMsgAC(
			systemId, turnNum, nBuyCount );

		// ����;
		GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
			dwDbNum, dwServerGroup, &lottoMsgAC );

		return nRet;
	}

	// �ζ� ���� ����Ʈ Select;
	int AdoManager::LottoCharNonConfirmBuyListSelect(
		DWORD dwDbNum,
		DWORD dwServerGroup, 
		unsigned int systemId,
		int confirmTicket )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( ado, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );
		APPEND_IPARAM_INT( ado, "@ConfirmTicket", confirmTicket );

		if( !ado.Execute4Cmd( "dbo.sp_LottoCharNonConfirmBuyListSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
		{
			// �ƹ��͵� ���������� �ϳ��� ��Ŷ�� �������ν�;
			// ������Ʈ�� �̷�������� �Ѵ�;
			GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC lottoMsgAC(
				systemId, -1 );
			lottoMsgAC.m_emAC = LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_OK;

			GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
				dwDbNum, dwServerGroup, &lottoMsgAC );

			return nRet;
		}

		int curTurnNum = -1;
		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()->GetLottoSystem( systemId );
		if( pLottoSystem )
			curTurnNum = pLottoSystem->GetCurTurnNum();

		int nBuyCount = 0;

		int turnNum = -1;
		int buyType = 0;
		int ranking = 0;
		std::string strDrawingNum = "";
		do
		{
			ado.GetCollect( "TurnNum", turnNum );
			ado.GetCollect( "BuyType", buyType );
			ado.GetCollect( "Ranking", ranking );
			ado.GetCollect( "DrawingNum", strDrawingNum );

			LOTTO_SYSTEM_BUY tempBuy;
			tempBuy.lottoBuyType = static_cast<EMLOTTO_BUY_TYPE>( buyType );
			tempBuy.dwDbNum = dwDbNum;
			tempBuy.ranking = ranking;
			tempBuy.bConfirm = ( confirmTicket == 1 ) ? true : false;

			{
				boost::char_separator<char> sep("|");
				tokenizer tok( strDrawingNum, sep );

				int count = 0;
				for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
				{
					if( count >= 0 && count < LottoSystem::MAX_DRAWING_NUM_LIST )
						tempBuy.drawingNum[ count++ ] = atoi( (*it).c_str() );
				}
			}

			// ���� ���� Ȯ���� �ȵȰ��� �翬�ϱ� ������ ������ �ʴ´�;
			if( dwDbNum != 0 && ( curTurnNum != turnNum ) )
			{
				// ��Ŷ;
				GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_AC lottoMsgAC(
					systemId, turnNum );
				lottoMsgAC.m_emAC = LottoSystem::EMLOTTO_REQ_BUY_LIST_AC_OK;
				lottoMsgAC.m_lotto = tempBuy;

				// ����;
				GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
					dwDbNum, dwServerGroup, &lottoMsgAC );

				nBuyCount++;
			}

		} while( ado.Next() );

		GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_COUNT_AC lottoMsgAC(
			systemId, 0, nBuyCount );

		// turnNum�� �����ϸ� ��÷��ȣ�� �����Ѵ�;
		if( turnNum >= 0 )
			LottoWinNumSelect( dwDbNum, dwServerGroup, turnNum, systemId );

		return nRet;
	}

	// �ζ� ��÷�� ����Ʈ Select;
	int AdoManager::LottoWinManListSelect(
		DWORD dwDbNum,
		DWORD dwServerGroup, 
		unsigned int turnNum,
		unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoWinManListSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return nRet;

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		int ranking = 0;
		LONGLONG winMoney = 0;
		int winCount = 0;
		DWORD chaNum = 0;
		DWORD serverGroup = 0;
		std::string strChaName = "";
		DWORD dwItemMainID = 0;
		DWORD dwItemSubID = 0;
		DWORD dwItemCount = 0;
		do
		{
			ado.GetCollect( "Ranking", ranking );
			ado.GetCollect( "WinMoney", winMoney );
			ado.GetCollect( "WinCount", winCount );
			ado.GetCollect( "ChaNum", chaNum );
			ado.GetCollect( "ServerGroup", serverGroup );
			ado.GetCollect( "ChaName", strChaName );
			ado.GetCollect( "WinItemMainID", dwItemMainID );
			ado.GetCollect( "WinItemSubID", dwItemSubID );
			ado.GetCollect( "WinItemCount", dwItemCount );

			LOTTO_SYSTEM_WINNER_MAN winnerMan;
			winnerMan.ranking = ranking;
			winnerMan.winMoney = winMoney;
			winnerMan.winCount = winCount;
			winnerMan.dwDbNum = chaNum;
			winnerMan.dwServerGroup = serverGroup;
			winnerMan.sRewardItem = SNATIVEID( dwItemMainID, dwItemSubID );
			winnerMan.nRewardItemCount = static_cast< int >( dwItemCount );
			StringCchCopy( winnerMan.szChaName, CHAR_SZNAME, strChaName.c_str() );	 

			if( pLottoSystem->GetCurTurnNum() == turnNum )
				pLottoSystem->SetWinnerLotto( winnerMan );
			else
			{
				if( dwDbNum != 0 )
				{
					// ��Ŷ ������;
					GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_AC lottoMsgAC(
						systemId, turnNum );
					lottoMsgAC.m_winnerMan = winnerMan;
					lottoMsgAC.dwChaNum = dwDbNum;
					lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_MAN_LIST_AC_OK;

					GLLottoSystemManAgent::Instance()->SendToServerGroupFieldFromAgent(
						dwServerGroup, &lottoMsgAC );
				}
			}
		} while( ado.Next() );

		return nRet;
	}

	int AdoManager::LottoInventWinNumSelect( DWORD dwDbNum, DWORD dwServerGroup, unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );

		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoInventWinNumSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return nRet;

		LONGLONG lottoDrawingNumInt = 0;
		std::string strDrawingNum = "";
		int nDrawingNum = 0;
		do 
		{
			ado.GetCollect( "DrawingNumInt", lottoDrawingNumInt );
			ado.GetCollect( "DrawingNumCount", nDrawingNum );

			sc::db::AdoExt adoDrawingNum( m_GameDBConnString );

			APPEND_IPARAM_BIGINT( adoDrawingNum, "@DrawingNumInt", lottoDrawingNumInt );
			if( !adoDrawingNum.Execute4Cmd( "dbo.sp_LottoDrawingNumSelect", adCmdStoredProc ) )
			{
				nRet = sc::db::DB_ERROR;
			}

			if( adoDrawingNum.GetEOF() || adoDrawingNum.GetBOF() )
				return nRet;

			adoDrawingNum.GetCollect( "DrawingNum", strDrawingNum );

			LOTTO_SYSTEM_LOTTO_NUM winNum;

			boost::char_separator<char> sep("|");
			tokenizer tok( strDrawingNum, sep );

			unsigned int count = 0;
			for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
			{
				if( count >= 0 && count < LottoSystem::MAX_DRAWING_NUM_LIST )
					winNum.lottoNum[ count++ ] = atoi( (*it).c_str() );
			}

			GLMSG::NET_LOTTO_MESSAGE_INVENT_NUM_DSP_AC lottoMsgAC(
				systemId, nDrawingNum, winNum );
			GLLottoSystemManAgent::Instance()->SendToServerGroupClientFromAgent(
				dwDbNum, dwServerGroup, &lottoMsgAC );

		} while ( ado.Next() );

		return nRet;
	}

	// �ζ� ��÷�� ���� ����;
	int AdoManager::LottoDrawingMachineNumSave( unsigned int lottoNum,
		unsigned int accumulateCount,
		unsigned int addCount,
		unsigned int systemId,
		bool bUpdate )
	{
		int nRet = sc::db::DB_OK;

		if ( addCount <= 0 )
			addCount = 1;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_RPARAM_INT( ado );

		APPEND_IPARAM_INT( ado, "@LottoNum", lottoNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );
		APPEND_IPARAM_BIGINT( ado, "@AccumulateCount", accumulateCount );
		APPEND_IPARAM_INT( ado, "@AddCount", addCount );
		APPEND_IPARAM_INT( ado, "@Update", bUpdate ? 1 : 0 );

		APPEND_OPARAM_INT( ado, "@nReturn" );

		nRet = ExecuteStoredProcedureIntReturn( "dbo.sp_LottoDrawingMachineNumSave", ado, "@nReturn" );
		if( nRet != sc::db::DB_OK )
			sc::writeLogWarn( std::string("LottoDrawingMachineNumSave() : Sync Failure") );

		// ��÷�� ����;
		LottoDrawingMachineNumSelect( lottoNum, systemId );

		return nRet;
	}

	int AdoManager::LottoSystemTurnSave( unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		LOTTO_SYSTEM_VALUE sValue = GLLottoGlobalData::Instance()
			->GetLottoSystemValue( systemId );

		std::string strWinTypes = sValue.MakeJson_WinTypeVector();

		/*	@TurnNum int,
			@SystemID BIGINT,
			@MoneyType int,
			@TotalNum int,
			@DrawingTotalNum int,
			@DrawingMainNum int,
			@DrawingBonusNum int	*/

		// 1. ���� Turn ����;
		sc::db::AdoExt adoTurnSave( m_GameDBConnString );
		APPEND_IPARAM_INT( adoTurnSave, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( adoTurnSave, "@SystemID", systemId );
		APPEND_IPARAM_INT( adoTurnSave, "@MoneyType", sValue.emMoneyType );
		APPEND_IPARAM_INT( adoTurnSave, "@TotalNum", sValue.nLottoTotal );
		APPEND_IPARAM_INT( adoTurnSave, "@DrawingTotalNum", sValue.nDrawingTotal );
		APPEND_IPARAM_INT( adoTurnSave, "@DrawingMainNum", sValue.nDrawingMain );
		APPEND_IPARAM_INT( adoTurnSave, "@DrawingBonusNum", sValue.nDrawingBonus );
		APPEND_IPARAM_BIGINT( adoTurnSave, "@LottoTicketMoney", sValue.nLottoTicketMoney );
		APPEND_IPARAM_INT( adoTurnSave, "@TicketFee", sValue.nTicketFee );
		APPEND_IPARAM_VARCHAR( adoTurnSave, "@WinTypes", strWinTypes.c_str(), 512 );

		nRet = ExecuteStoredProcedure( "dbo.sp_LottoSystemTurnSave", adoTurnSave );
		if( nRet == sc::db::DB_ERROR )
			return nRet;

		// 2. ���� Turn�� ���� ���� ����;
		sc::db::AdoExt adoTurnInfo( m_GameDBConnString );
		APPEND_IPARAM_INT( adoTurnInfo, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( adoTurnInfo, "@SystemID", systemId );

		if( !adoTurnInfo.Execute4Cmd( "dbo.sp_LottoSystemTurnInfoSelect", adCmdStoredProc ) )
			return sc::db::DB_ERROR;

		if( adoTurnInfo.GetEOF() || adoTurnInfo.GetBOF() )
			return nRet;

		int moneyType = 0;
		int nTotalNum = 0;
		int nDrawingTotalNum = 0;
		int nDrawingMainNum = 0;
		int nDrawingBonusNum = 0;

		LONGLONG llLottoTicketMoney = 0;
		int ticketFee = 0;
		std::string strWinType;

		adoTurnInfo.GetCollect( "MoneyType", moneyType );
		adoTurnInfo.GetCollect( "TotalNum", nTotalNum );
		adoTurnInfo.GetCollect( "DrawingTotalNum", nDrawingTotalNum );
		adoTurnInfo.GetCollect( "DrawingMainNum", nDrawingMainNum );
		adoTurnInfo.GetCollect( "DrawingBonusNum", nDrawingBonusNum );
		adoTurnInfo.GetCollect( "LottoTicketMoney", llLottoTicketMoney );
		adoTurnInfo.GetCollect( "TicketFee", ticketFee );
		adoTurnInfo.GetCollect( "WinTypes", strWinType );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		if( llLottoTicketMoney == 0 )
			llLottoTicketMoney = sValue.nLottoTicketMoney;

		if( ticketFee == 0 )
			ticketFee = sValue.nTicketFee;

		if( strWinType.empty() )
			strWinType = strWinTypes;

		pLottoSystem->SetMoneyType( static_cast<EMLOTTO_MONEY_TYPE>( moneyType ) );
		pLottoSystem->SetTotalNum( nTotalNum );
		pLottoSystem->SetDrawingTotalNum( nDrawingTotalNum );
		pLottoSystem->SetDrawingMainNum( nDrawingMainNum );
		pLottoSystem->SetDrawingBonusNum( nDrawingBonusNum );
		pLottoSystem->SetLottoTicketMoney( llLottoTicketMoney );
		pLottoSystem->SetTicketFee( ticketFee );
		pLottoSystem->SetWinType( strWinType );

		// Total Num�� ����Ǿ����Ƿ� ��÷�� ����;
		pLottoSystem->InitializeDrawingMachine();
		for( int i=0; i<pLottoSystem->GetTotalNum(); ++i )
			LottoDrawingMachineNumSave( i+1, 0, 1, systemId, false );
		pLottoSystem->UpdateDrawingMachine();

		// 3. Accumulate Money ����;
		LottoAccumulateMoneySelect( 0, 0, turnNum, systemId );

		// 4. ���� ȸ���� ��÷��ȣ ��������;
		if( turnNum > 0 )
			LottoWinNumSelect( 0, 0, turnNum-1, systemId );

		// 5. �̹� ȸ���� ��÷��ȣ ��������;
		LottoWinNumSelect( 0, 0, turnNum, systemId );

		// 6. �̹� ȸ���� ��÷�� ����Ʈ ��������;
		LottoWinManListSelect( 0, 0, turnNum, systemId );

		return nRet;
	}

	// �ζ� ����;
	int AdoManager::LottoCharBuyListInsert(
		LONGLONG accumulateMoney,
		LONGLONG charge,
		int buyType,
		const std::string& drawingNum,
		LONGLONG drawingNumInt,
		int confirmTicket,
		DWORD dwUserNum,
		DWORD dwDbNum,
		DWORD dwServerGroup,
		char* szChaName,
		unsigned int turnNum,
		unsigned int systemId )
	{
		//@MoneyType int,
		//@BuyTicketMoney BIGINT,
		LOTTO_SYSTEM_VALUE sValue = GLLottoGlobalData::Instance()
			->GetLottoSystemValue( systemId );

		// ���� ���� ���Ÿ���Ʈ ����;
		sc::db::AdoExt adoBuyCount( m_GameDBConnString );

		APPEND_IPARAM_INT( adoBuyCount, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( adoBuyCount, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_BIGINT( adoBuyCount, "@SystemID", systemId );

		if( !adoBuyCount.Execute4Cmd( "dbo.sp_LottoChaTicketCountSelect", adCmdStoredProc ) )
		{
			return sc::db::DB_ERROR;
		}

		if( adoBuyCount.GetEOF() || adoBuyCount.GetBOF() )
			return sc::db::DB_ERROR;

		int nBuyCount = 0;	
		adoBuyCount.GetCollect( "BuyCount", nBuyCount );

		// ���� �ϵ��� ���Ÿ���Ʈ ����;
		sc::db::AdoExt adoPreBuyCount( m_GameDBConnString );

		APPEND_IPARAM_INT( adoPreBuyCount, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( adoPreBuyCount, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_BIGINT( adoPreBuyCount, "@SystemID", systemId );

		if( !adoPreBuyCount.Execute4Cmd( "dbo.sp_LottoChaPreTicketCountSelect", adCmdStoredProc ) )
		{
			return sc::db::DB_ERROR;
		}

		if( adoPreBuyCount.GetEOF() || adoPreBuyCount.GetBOF() )
			return sc::db::DB_ERROR;

		int nPreBuyCount = 0;	
		adoPreBuyCount.GetCollect( "BuyCount", nPreBuyCount );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		// �ִ� ���� ������ �Ѿ����Ƿ� �������� �ʴ´�;
		// ���� ���Ÿ���Ʈ ������ �����ϸ�( ��Ȯ�� ����Ʈ�� ���Ҵٸ� );
		if( nBuyCount >= static_cast<int>( sValue.nBuyMultiNum ) ||
			nPreBuyCount > 0 )
		{
			// ���� ���� ��Ŷ;
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF(
				systemId, dwDbNum );

			lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_FAIL;

			GLLottoSystemManAgent::Instance()->SendToServerGroupFieldFromAgent(
				dwServerGroup, &lottoMsgAF );

			return sc::db::DB_OK;
		}

		{
			// ���� ���� ��Ŷ;
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF(
				systemId, dwDbNum );

			lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_OK;
			lottoMsgAF.m_moneyType = pLottoSystem->GetMoneyType();
			lottoMsgAF.m_lottoTicketMoney = pLottoSystem->GetLottoTicketMoney();

			LOTTO_SYSTEM_BUY tempBuy;
			tempBuy.lottoBuyType = static_cast<EMLOTTO_BUY_TYPE>( buyType );
			tempBuy.dwDbNum = dwDbNum;
			tempBuy.dwUserNum = dwUserNum;
			tempBuy.dwServerGroup = dwServerGroup;
			StringCchCopy( tempBuy.szChaName, CHAR_SZNAME, szChaName );

			{
				boost::char_separator<char> sep("|");
				tokenizer tok( drawingNum, sep );

				int count = 0;
				for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
				{
					if( count >= 0 && count < LottoSystem::MAX_DRAWING_NUM_LIST )
						tempBuy.drawingNum[ count++ ] = atoi( (*it).c_str() );
				}
			}

			lottoMsgAF.m_lottoBuy = tempBuy;
			
			GLLottoSystemManAgent::Instance()->SendToServerGroupFieldFromAgent(
				dwServerGroup, &lottoMsgAF );
		}	

		sc::db::AdoExt ado( m_GameDBConnString );

		APPEND_RPARAM_INT( ado );

		APPEND_IPARAM_BIGINT( ado, "@AccumulateMoney", accumulateMoney );
		APPEND_IPARAM_BIGINT( ado, "@Charge", charge );
		APPEND_IPARAM_INT( ado, "@MoneyType", static_cast<int>( pLottoSystem->GetMoneyType() ) );
		APPEND_IPARAM_BIGINT( ado, "@BuyTicketMoney", pLottoSystem->GetLottoTicketMoney() );
		APPEND_IPARAM_INT( ado, "@BuyType", buyType );
		APPEND_IPARAM_VARCHAR( ado, "@DrawingNum", drawingNum.c_str(), 256 );
		APPEND_IPARAM_BIGINT( ado, "@DrawingNumInt", drawingNumInt );
		APPEND_IPARAM_INT( ado, "@ConfirmTicket", confirmTicket );
		APPEND_IPARAM_INT( ado, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( ado, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_VARCHAR( ado, "@ChaName", szChaName, CHAR_SZNAME );
		APPEND_IPARAM_INT( ado, "@TicketIdx", nBuyCount );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		APPEND_OPARAM_INT( ado, "@nReturn" );

		// ���� Log;
		LOTTO_SYSTEM_BUY_LOG buyLog;
		buyLog.systemId = systemId;
		buyLog.turnNum = turnNum;
		buyLog.chaNum = dwDbNum;
		buyLog.dwServerGroup = dwServerGroup;
		buyLog.dwUserNum = dwUserNum;
		buyLog.buyType = static_cast<LottoSystem::EMLOTTO_BUY_TYPE>( buyType );
		buyLog.drawingNum = drawingNum;

		int nReturn = ExecuteStoredProcedure( "dbo.sp_LottoCharBuyListInsert", ado );
		if( nReturn )
		{
			// TurnNum�� �ٸ��Ƿ� ���� Load �Ѵ�;
			LottoSystemLoad( systemId );

			// ���� Log;
			buyLog.turnNum = nReturn;
		}
		else if( nReturn == 0 )
		{
			// ����;
			CLottoSystem* pLottoSystem =
				GLLottoSystemManAgent::Instance()
				->GetLottoSystem( systemId );
			if( !pLottoSystem )
				return sc::db::DB_ERROR;

			if( pLottoSystem->GetCurTurnNum() == turnNum )
				pLottoSystem->AccumulateMoney( accumulateMoney );
		}
		else if( nReturn == sc::db::DB_ERROR )
			return sc::db::DB_ERROR;

		sc::db::AdoExt adoTicketID( m_GameDBConnString );

		APPEND_IPARAM_INT( adoTicketID, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( adoTicketID, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_INT( adoTicketID, "@SystemID", systemId );
		APPEND_IPARAM_INT( adoTicketID, "@TicketIdx", nBuyCount );

		if( !adoTicketID.Execute4Cmd( "dbo.sp_LottoBuyTicketIDSelect", adCmdStoredProc ) )
		{
			return sc::db::DB_ERROR;
		}

		if( adoTicketID.GetEOF() || adoTicketID.GetBOF() )
			return sc::db::DB_ERROR;

		int ticketID = 0;	
		adoTicketID.GetCollect( "Seq", ticketID );

		buyLog.dwTicketID = ticketID;

		// ���� Log;
		LottoSystemBuyLog( buyLog );

		return 0;
	}

	// �ζ� ��÷�� ����Ʈ ����;
	int AdoManager::LottoWinManListUpdate(
		unsigned int ranking,
		LONGLONG winMoney,
		SNATIVEID sItemID,
		DWORD dwItemCount,
		unsigned int turnNum,
		unsigned int systemId )
	{
		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@Ranking", ranking );
		APPEND_IPARAM_BIGINT( ado, "@WinMoney", winMoney );
		APPEND_IPARAM_INT( ado, "@WinItemMainID", sItemID.Mid() );
		APPEND_IPARAM_INT( ado, "@WinItemSubID", sItemID.Sid() );
		APPEND_IPARAM_INT( ado, "@WinItemCount", dwItemCount );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		return ExecuteStoredProcedure( "dbo.sp_LottoWinManListUpdate", ado );
	}

	int AdoManager::LottoWinManListInsert(
		unsigned int ranking,
		LONGLONG winNumInt,
		LONGLONG bonusNumInt,
		unsigned int systemId )
	{
		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@Ranking", ranking );
		APPEND_IPARAM_BIGINT( ado, "@WinNumInt", winNumInt );
		APPEND_IPARAM_BIGINT( ado, "@BonusNumInt", bonusNumInt );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		return ExecuteStoredProcedure( "dbo.sp_LottoWinManListInsert", ado );
	}

	// �ζ� ���� ��÷�� ����;
	int AdoManager::LottoAccumulateMoneyUpdate(
		LONGLONG accumulateMoney,
		unsigned int turnNum,
		unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_RPARAM_INT( ado );

		APPEND_IPARAM_BIGINT( ado, "@AccumulateMoney", accumulateMoney );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		APPEND_OPARAM_INT( ado, "@nReturn" );

		nRet = ExecuteStoredProcedureIntReturn( "dbo.sp_LottoAccumulateMoneyUpdate", ado, "@nReturn" );
		if( nRet != sc::db::DB_OK )
			sc::writeLogWarn( std::string("LottoAccumulateMoneyUpdate() : Sync Failure") );

		return nRet;
	}

	// �ζ� ��÷��ȣ ����;
	int AdoManager::LottoWinNumUpdate(
		const std::string& winNum,
		LONGLONG winNumInt,
		LONGLONG bonusNumInt,
		unsigned int turnNum,
		unsigned int systemId )
	{
		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_VARCHAR( ado, "@WinningNum", winNum.c_str(), 256 );
		APPEND_IPARAM_BIGINT( ado, "@WinningNumInt", winNumInt );
		APPEND_IPARAM_BIGINT( ado, "@BonusWinningNumInt", bonusNumInt );
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		return ExecuteStoredProcedure( "dbo.sp_LottoWinNumUpdate", ado );
	}

	int AdoManager::LottoCharNonConfirmBuyListUpdate(
		DWORD dwDbNum, DWORD dwServerGroup, LONGLONG invenMoney, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		APPEND_IPARAM_INT( ado, "@ChaNum", dwDbNum );
		APPEND_IPARAM_INT( ado, "@ServerGroup", dwServerGroup );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoCharNonConfirmBuyListUpdate", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( ado.GetEOF() || ado.GetBOF() )
			return nRet;

		DWORD dwTicketID = 0;
		int ranking = 0;
		LONGLONG winMoney = 0;
		unsigned int turnNum = 0;
		WORD wItemMainID = 0;
		WORD wItemSubID = 0;
		DWORD dwItemCount = 0;
		LONGLONG realInvenMoney = invenMoney;
		do
		{
			ado.GetCollect( "Seq", dwTicketID );
			ado.GetCollect( "Ranking", ranking );
			ado.GetCollect( "WinMoney", winMoney );
			ado.GetCollect( "WinItemMainID", wItemMainID );
			ado.GetCollect( "WinItemSubID", wItemSubID );
			ado.GetCollect( "WinItemCount", dwItemCount );
			ado.GetCollect( "TurnNum", turnNum );

			if( ranking != 0 )
			{
				// ��÷�� Ƽ�� ���� ����;
				LOTTO_SYSTEM_VALUE sValue = GLLottoGlobalData::Instance()
					->GetLottoSystemValue( systemId );

				CLottoSystem* pLottoSystem =
					GLLottoSystemManAgent::Instance()
					->GetLottoSystem( systemId );
				if( !pLottoSystem )
					return sc::db::DB_ERROR;

				GLMSG::NET_LOTTO_MESSAGE_REQ_POST_WIN_MONEY_AF postMsgAF(
					systemId, dwDbNum, turnNum, winMoney, ranking );
				postMsgAF.m_moneyType = pLottoSystem->GetMoneyType();
				postMsgAF.m_sWinItem = SNATIVEID( wItemMainID, wItemSubID );
				postMsgAF.m_dwWinItemCount = dwItemCount;

				GLLottoSystemManAgent::Instance()
					->SendToServerGroupFieldFromAgent( dwServerGroup, &postMsgAF );

				// ���� �α�;
				LottoSystemGiveWinTicketLog( systemId, dwTicketID, realInvenMoney );

				realInvenMoney += winMoney;
			}

		} while( ado.Next() );

		return nRet;
	}

	int AdoManager::LottoSystemNextTurn( unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		// 1. ���� Turn Num ã��;
		sc::db::AdoExt adoTurnNum( m_GameDBConnString );
		APPEND_IPARAM_BIGINT( adoTurnNum, "@SystemID", systemId );

		if( !adoTurnNum.Execute4Cmd( "dbo.sp_LottoSystemCurTurnSelect", adCmdStoredProc ) )
			return sc::db::DB_ERROR;

		if( adoTurnNum.GetEOF() || adoTurnNum.GetBOF() )
			return nRet;

		unsigned int curTurnNum = 0;

		adoTurnNum.GetCollect( "CurTurnNum", curTurnNum );

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		// ���� �� ����;
		curTurnNum += 1;

		pLottoSystem->SetCurTurnNum( curTurnNum );

		// 2. ���� Turn ����;
		LOTTO_SYSTEM_VALUE sValue = GLLottoGlobalData::Instance()
			->GetLottoSystemValue( systemId );

		std::string strWinTypes = sValue.MakeJson_WinTypeVector();

		sc::db::AdoExt adoTurnSave( m_GameDBConnString );
		APPEND_IPARAM_INT( adoTurnSave, "@TurnNum", curTurnNum );
		APPEND_IPARAM_BIGINT( adoTurnSave, "@SystemID", systemId );
		APPEND_IPARAM_INT( adoTurnSave, "@MoneyType", sValue.emMoneyType );
		APPEND_IPARAM_INT( adoTurnSave, "@TotalNum", sValue.nLottoTotal );
		APPEND_IPARAM_INT( adoTurnSave, "@DrawingTotalNum", sValue.nDrawingTotal );
		APPEND_IPARAM_INT( adoTurnSave, "@DrawingMainNum", sValue.nDrawingMain );
		APPEND_IPARAM_INT( adoTurnSave, "@DrawingBonusNum", sValue.nDrawingBonus );
		APPEND_IPARAM_BIGINT( adoTurnSave, "@LottoTicketMoney", sValue.nLottoTicketMoney );
		APPEND_IPARAM_INT( adoTurnSave, "@TicketFee", sValue.nTicketFee );
		APPEND_IPARAM_VARCHAR( adoTurnSave, "@WinTypes", strWinTypes.c_str(), 512 );

		nRet = ExecuteStoredProcedure( "dbo.sp_LottoSystemTurnSave", adoTurnSave );
		if( nRet == sc::db::DB_ERROR )
			return nRet;

		// 3. ���� Turn�� ���� ���� ����;
		sc::db::AdoExt adoTurnInfo( m_GameDBConnString );
		APPEND_IPARAM_INT( adoTurnInfo, "@TurnNum", curTurnNum );
		APPEND_IPARAM_BIGINT( adoTurnInfo, "@SystemID", systemId );

		if( !adoTurnInfo.Execute4Cmd( "dbo.sp_LottoSystemTurnInfoSelect", adCmdStoredProc ) )
			return sc::db::DB_ERROR;
		
		if( adoTurnInfo.GetEOF() || adoTurnInfo.GetBOF() )
			return nRet;

		int moneyType = 0;
		int nTotalNum = 0;
		int nDrawingTotalNum = 0;
		int nDrawingMainNum = 0;
		int nDrawingBonusNum = 0;

		LONGLONG llLottoTicketMoney = 0;
		int ticketFee = 0;
		std::string strWinType;

		adoTurnInfo.GetCollect( "MoneyType", moneyType );
		adoTurnInfo.GetCollect( "TotalNum", nTotalNum );
		adoTurnInfo.GetCollect( "DrawingTotalNum", nDrawingTotalNum );
		adoTurnInfo.GetCollect( "DrawingMainNum", nDrawingMainNum );
		adoTurnInfo.GetCollect( "DrawingBonusNum", nDrawingBonusNum );
		adoTurnInfo.GetCollect( "LottoTicketMoney", llLottoTicketMoney );
		adoTurnInfo.GetCollect( "TicketFee", ticketFee );
		adoTurnInfo.GetCollect( "WinTypes", strWinType );

		if( llLottoTicketMoney == 0 )
			llLottoTicketMoney = sValue.nLottoTicketMoney;

		if( ticketFee == 0 )
			ticketFee = sValue.nTicketFee;

		if( strWinType.empty() )
			strWinType = strWinTypes;

		pLottoSystem->SetMoneyType( static_cast<EMLOTTO_MONEY_TYPE>( moneyType ) );
		pLottoSystem->SetTotalNum( nTotalNum );
		pLottoSystem->SetDrawingTotalNum( nDrawingTotalNum );
		pLottoSystem->SetDrawingMainNum( nDrawingMainNum );
		pLottoSystem->SetDrawingBonusNum( nDrawingBonusNum );
		pLottoSystem->SetLottoTicketMoney( llLottoTicketMoney );
		pLottoSystem->SetTicketFee( ticketFee );
		pLottoSystem->SetWinType( strWinType );

		// System Value ����;
		pLottoSystem->BroadcastSyncSystemValue();

		// ��÷�� ����;
		for( int i=0; i<pLottoSystem->GetTotalNum(); ++i )
			LottoDrawingMachineNumSave( i+1, 0, 1, systemId, false );

		pLottoSystem->UpdateDrawingMachine();

		// 4. ���� ȸ���� ��÷��ȣ ��������;
		if( curTurnNum > 0 )
			LottoWinNumSelect( 0, 0, curTurnNum-1, systemId );

		// 5. �̹� ȸ���� ��÷��ȣ ��������;
		LottoWinNumSelect( 0, 0, curTurnNum, systemId );

		// 6. �̿�;
		if( pLottoSystem )
		{
			LONGLONG nextMoney = pLottoSystem->CalculateNextMoney();

			// �ζ� �ݾ� �̿�;
			nRet = LottoAccumulateMoneyUpdate( nextMoney, curTurnNum, systemId );
			if( nRet == sc::db::DB_ERROR )
				return nRet;

			// �ζ� �ݾ� ����;
			nRet = LottoAccumulateMoneySelect( 0, 0, curTurnNum, systemId );
			if( nRet == sc::db::DB_ERROR )
				return nRet;
		}

		return nRet;
	}

	int AdoManager::LottoConfirmWinMan( unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()->GetLottoSystem( systemId );
		if( pLottoSystem )
		{
			std::vector<LOTTO_WIN_NUM_INT> winNumList = pLottoSystem->GetWinNumList();

			for( unsigned int i=0; i<winNumList.size(); ++i )
			{
				LOTTO_WIN_NUM_INT winNumIntInfo = winNumList.at( i );

				for( unsigned int j=0; j<winNumIntInfo.winNumInt.size(); ++j )
				{
					LONGLONG winNumInt = winNumIntInfo.winNumInt.at( j );

					for( unsigned int k=0; k<winNumIntInfo.bonusNumInt.size(); ++k )
					{
						LONGLONG bonusNumInt = winNumIntInfo.bonusNumInt.at( k );

						// DBAction;
						nRet = LottoWinManListInsert(
							winNumIntInfo.ranking,
							winNumInt, bonusNumInt,
							systemId );
						if( nRet == sc::db::DB_ERROR )
							return nRet;
					}
				}
			}
		}

		return nRet;
	}

	int AdoManager::LottoChangeInventWinNum( unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );

		// 1. ���� �� ��ȣ �˻�;
		APPEND_IPARAM_INT( ado, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", systemId );

		if( !ado.Execute4Cmd( "dbo.sp_LottoInventWinNumSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()
			->GetLottoSystem( systemId );
		if( !pLottoSystem )
			return sc::db::DB_ERROR;

		// �ϳ��� ���Ե��� �ʾ��� ���;
		if( ado.GetEOF() || ado.GetBOF() )
		{
			pLottoSystem->GenerateWinNumList();
			return nRet;
		}

		LONGLONG lottoDrawingNumInt = 0;
		std::string strDrawingNum = "";
		ado.GetCollect( "DrawingNumInt", lottoDrawingNumInt );

		sc::db::AdoExt adoDrawingNum( m_GameDBConnString );

		APPEND_IPARAM_BIGINT( adoDrawingNum, "@DrawingNumInt", lottoDrawingNumInt );
		if( !adoDrawingNum.Execute4Cmd( "dbo.sp_LottoDrawingNumSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( adoDrawingNum.GetEOF() || adoDrawingNum.GetBOF() )
			return nRet;

		adoDrawingNum.GetCollect( "DrawingNum", strDrawingNum );

		// 2. ��÷ ��ȣ �����;
		std::vector<unsigned int> winNum;

		{
			boost::char_separator<char> sep("|");
			tokenizer tok( strDrawingNum, sep );
			
			for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
				winNum.push_back( atoi( (*it).c_str() ) );

			// ��÷��ȣ �ѹ� ����;
			random_shuffle( winNum.begin(), winNum.end() );
		}

		// 3. ���ʽ� ��ȣ ����;
		CLottoDrawingMachine* pDrawingMachine
			= pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return sc::db::DB_ERROR;

		while( true )
		{
			bool bDuplicate = false;
			unsigned int bonusNum = pDrawingMachine->StartDrawingMachine();
			for( unsigned int i=0; i<winNum.size(); ++i )
			{
				if( winNum.at( i ) == bonusNum )
					bDuplicate = true;
			}

			pDrawingMachine->ClearDrawingMachine();

			if ( 0 == bonusNum )
				return sc::db::DB_ERROR;

			if( !bDuplicate )
			{
				winNum.push_back( bonusNum );
				break;
			}
		}

		// 4. String �� ���� ȭ;
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( systemId );

		std::string strWinNum = "";
		LONGLONG winNumInt = 0;
		LONGLONG bonusNumInt = 0;
		for( unsigned int i=0; i<winNum.size(); ++i )
		{
			strWinNum += sc::string::format( "%d", winNum.at( i ) );
			strWinNum += "|";

			if( i < sValue.nDrawingMain )
			{
				winNumInt += static_cast<LONGLONG>(
					pow( 2.f, static_cast<int>( winNum.at( i ) - 1 ) ) );
			}
			else
			{
				bonusNumInt += static_cast<LONGLONG>(
					pow( 2.f, static_cast<int>( winNum.at( i ) - 1 ) ) );
			}
		}

		// 5. Update Win Num
		LOTTO_SYSTEM_LOTTO_NUM winnerNum;
		for( unsigned int i=0; i<winNum.size(); ++i )
			winnerNum.lottoNum[ i ] = winNum.at( i );
		winnerNum.SetLottoSetState( true );
		pLottoSystem->SetCurWinNum( winnerNum );

		nRet = LottoWinNumUpdate( strWinNum, winNumInt, bonusNumInt, turnNum, systemId );
		if( nRet == sc::db::DB_ERROR )
			return nRet;

		// 6. ���� �� ��ȣ�� �̿��Ͽ� ��÷�� ����Ʈ ã��;
		pLottoSystem->GenerateWinNumList();

		return nRet;
	}

	int AdoManager::LottoWinMoneyUpdate( unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		nRet = LottoWinManListSelect( 0, 0, turnNum, systemId );
		if( nRet == sc::db::DB_ERROR )
			return nRet;

		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()->GetLottoSystem( systemId );
		if( pLottoSystem )
		{
			// �� ��ŷ ���� ����;
			pLottoSystem->UpdateRankingInfo();

			// �� ��ŷ�� ��÷�ݾ� ����;
			LOTTO_SYSTEM_VALUE sValue =
				GLLottoGlobalData::Instance()->GetLottoSystemValue( systemId );

			LOTTO_SYSTEM_WINNER_MAN_LIST winnerManList = pLottoSystem->GetWinnerManList();
			for( unsigned int i=0; i<winnerManList.winnerManList.size(); ++i )
			{
				LOTTO_SYSTEM_WINNER_MAN winnerMan =
					winnerManList.winnerManList.at( i );

				std::vector<LOTTO_SYSTEM_WINNER_RANKING_INFO> rankingInfos =
					pLottoSystem->GetLottoRankingInfoList();
				int rankingIdx = winnerMan.ranking-1;

				// Give Money Type�� ���� ��÷�� ����� �ٸ�;
				EMGIVE_MONEY_TYPE giveMoneyType = ACCUMULATE_MONEY;
				if( rankingIdx >= 0 && rankingIdx < static_cast<int>( pLottoSystem->GetWinTypeCount() ) )
				{
					LOTTO_WIN_TYPE winType =
						pLottoSystem->GetWinType( rankingIdx );
					
					giveMoneyType = winType.emGiveMoneyType;
				}

				if( rankingIdx >= 0 && rankingIdx < static_cast<int>( rankingInfos.size() ) )
				{
					LOTTO_SYSTEM_WINNER_RANKING_INFO rankingInfo =
						rankingInfos.at( rankingIdx );

					LONGLONG allWinMoney = rankingInfo.winMoney;
					int count = rankingInfo.count;
					LONGLONG winMoney = 0;

					switch( giveMoneyType )
					{
					case ACCUMULATE_MONEY:
						if( count > 0 )
							winMoney = allWinMoney / count; 
						break;
					case FIXED_MONEY:
						winMoney = allWinMoney;
						break;
					}

					// DBAction ( ��÷�� ���� );
					LottoWinManListUpdate(
						winnerMan.ranking,
						winMoney,
						rankingInfo.sRewardItemID,
						rankingInfo.dwRewardItemCount,
						turnNum,
						systemId );
				}
			}
		}

		// ��÷ �α�;
		LottoSystemWinManLog( turnNum, systemId );

		return nRet;
	}

	int AdoManager::LottoSystemBuyLog(
		const LottoSystem::LOTTO_SYSTEM_BUY_LOG& logInfo )
	{
		sc::db::AdoExt ado( m_LogDBConnString );

		APPEND_IPARAM_BIGINT( ado, "@Seq", logInfo.dwTicketID );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", logInfo.systemId );
		APPEND_IPARAM_INT( ado, "@TurnNum", logInfo.turnNum );
		APPEND_IPARAM_INT( ado, "@UserNum", logInfo.dwUserNum );
		APPEND_IPARAM_INT( ado, "@ChaNum", logInfo.chaNum );
		APPEND_IPARAM_INT( ado, "@ServerGroup", logInfo.dwServerGroup );
		APPEND_IPARAM_INT( ado, "@BuyType", logInfo.buyType );
		APPEND_IPARAM_VARCHAR( ado, "@DrawingNum", logInfo.drawingNum.c_str(), 256 );

		return ExecuteStoredProcedure( "dbo.sp_LottoSystemBuyLog", ado );
	}

	int AdoManager::LottoSystemWinManLog( unsigned int turnNum, unsigned int systemId )
	{
		int nRet = sc::db::DB_OK;

		// ��÷�� �� ��÷�ݾ� �˻�;
		sc::db::AdoExt gameAdo( m_GameDBConnString );

		APPEND_IPARAM_INT( gameAdo, "@TurnNum", turnNum );
		APPEND_IPARAM_BIGINT( gameAdo, "@SystemID", systemId );

		if( !gameAdo.Execute4Cmd( "dbo.sp_LottoWinManBuyListSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( gameAdo.GetEOF() || gameAdo.GetBOF() )
			return nRet;

		DWORD dwTicketID = 0;
		DWORD dwDbNum = 0;
		DWORD dwServerGroup = 0;
		int ranking = 0;
		LONGLONG winMoney = 0;
		WORD wWinItemMainID = 0;
		WORD wWinItemSubID = 0;
		DWORD dwWinItemCount = 0;
		do 
		{
			gameAdo.GetCollect( "Seq", dwTicketID );
			gameAdo.GetCollect( "ChaNum", dwDbNum );
			gameAdo.GetCollect( "ServerGroup", dwServerGroup );
			gameAdo.GetCollect( "Ranking", ranking );
			gameAdo.GetCollect( "WinMoney", winMoney );
			gameAdo.GetCollect( "WinItemMainID", wWinItemMainID );
			gameAdo.GetCollect( "WinItemSubID", wWinItemSubID );
			gameAdo.GetCollect( "WinItemCount", dwWinItemCount );

			// ��÷�� Log;
			sc::db::AdoExt logAdo( m_LogDBConnString );

			APPEND_IPARAM_BIGINT( logAdo, "@TicketID", dwTicketID );
			APPEND_IPARAM_INT( logAdo, "@Ranking", ranking );
			APPEND_IPARAM_BIGINT( logAdo, "@WinMoney", winMoney );
			APPEND_IPARAM_INT( logAdo, "@WinItemMainID", wWinItemMainID );
			APPEND_IPARAM_INT( logAdo, "@WinItemSubID", wWinItemSubID );
			APPEND_IPARAM_INT( logAdo, "@WinItemCount", dwWinItemCount );

			nRet = ExecuteStoredProcedure( "dbo.sp_LottoSystemWinManLog", logAdo );

		} while ( gameAdo.Next() );

		return sc::db::DB_OK;
	}

	int AdoManager::LottoSystemGiveWinTicketLog( unsigned int systemId,
		DWORD dwTicketID, LONGLONG invenMoney )
	{
		int nRet = sc::db::DB_OK;
	
		// ��÷�� Log;
		sc::db::AdoExt logAdo( m_LogDBConnString );

		APPEND_IPARAM_BIGINT( logAdo, "@TicketID", dwTicketID );
		APPEND_IPARAM_BIGINT( logAdo, "@InvenMoney", invenMoney );

		nRet = ExecuteStoredProcedure( "dbo.sp_LottoSystemGiveWinTicketLog", logAdo );

		return sc::db::DB_OK;
	}

	// �ζ� �α�;
	int AdoManager::LottoSystemTurnInfoLog(
		const LottoSystem::LOTTO_SYSTEM_TURN_LOG& logInfo )
	{
		int nRet = sc::db::DB_OK;

		// ��÷��ȣ �˻�;
		sc::db::AdoExt gameAdo( m_GameDBConnString );

		APPEND_IPARAM_INT( gameAdo, "@TurnNum", logInfo.turnNum );
		APPEND_IPARAM_BIGINT( gameAdo, "@SystemID", logInfo.systemId );

		if( !gameAdo.Execute4Cmd( "dbo.sp_LottoWinNumSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( gameAdo.GetEOF() || gameAdo.GetBOF() )
			return nRet;

		std::string winNum;
		gameAdo.GetCollect( "WinNum", winNum );

		// �̹����� ���� ��÷�� �˻�;
		sc::db::AdoExt gameAdo2( m_GameDBConnString );

		APPEND_IPARAM_INT( gameAdo2, "@TurnNum", logInfo.turnNum );
		APPEND_IPARAM_BIGINT( gameAdo2, "@SystemID", logInfo.systemId );

		if( !gameAdo2.Execute4Cmd( "dbo.sp_LottoChargeSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if( gameAdo2.GetEOF() || gameAdo2.GetBOF() )
			return nRet;

		LONGLONG llCharge = 0;
		gameAdo2.GetCollect( "Charge", llCharge );

		// Log;
		sc::db::AdoExt ado( m_LogDBConnString );

		APPEND_IPARAM_INT( ado, "@MoneyType", logInfo.moneyType );
		APPEND_IPARAM_BIGINT( ado, "@SystemID", logInfo.systemId );
		APPEND_IPARAM_INT( ado, "@TurnNum", logInfo.turnNum );
		APPEND_IPARAM_VARCHAR( ado, "@WinNum", winNum.c_str(), 256 );

		APPEND_IPARAM_VARCHAR( ado, "@DrawingStartTime", logInfo.drawingStartTime.c_str(), 256 );
		APPEND_IPARAM_VARCHAR( ado, "@DrawingEndTime", logInfo.drawingEndTime.c_str(), 256 );

		APPEND_IPARAM_VARCHAR( ado, "@WinInfo", logInfo.winInfo.c_str(), 512 );
		APPEND_IPARAM_BIGINT( ado, "@TicketMoney", logInfo.ticketMoney );
		APPEND_IPARAM_INT( ado, "@TicketMaxCount", logInfo.ticketMaxCount );
		APPEND_IPARAM_BIGINT( ado, "@AccumulateMoney", logInfo.accumulateMoney );
		APPEND_IPARAM_BIGINT( ado, "@Charge", llCharge );
		APPEND_IPARAM_INT( ado, "@ChargePct", logInfo.chargePct );

		APPEND_IPARAM_VARCHAR( ado, "@RankingAllMoney", logInfo.rankingAllMoney.c_str(), 512 );
		APPEND_IPARAM_VARCHAR( ado, "@RankingAllCount", logInfo.rankingAllCount.c_str(), 512 );
		APPEND_IPARAM_VARCHAR( ado, "@RankingMoney", logInfo.rankingMoney.c_str(), 512 );

		APPEND_IPARAM_BIGINT( ado, "@ForwardMoney", logInfo.forwardMoney );

		return ExecuteStoredProcedure( "dbo.sp_LottoSystemTurnInfoLog", ado );
	}
}