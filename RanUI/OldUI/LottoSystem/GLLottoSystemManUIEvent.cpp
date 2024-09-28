#include "stdafx.h"

#include "./GLLottoSystemManUI.h"

#include "./LottoMainUI.h"
#include "./LottoBuyListInfo.h"
#include "./LottoBuyLottoUI.h"
#include "./LottoWinManListInfo.h"
#include "./LottoDisplayUI.h"
#include "./LottoNotifyButton.h"

#include "../../../SigmaCore/Json/MinJson.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

// Logic;
#include "../../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"
#include "../../../RanLogicClient/LottoSystem/GLLottoGlobalDataClient.h"

void GLLottoSystemManUI::UIInitializeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pGaeaClient->PrintConsoleText(
		sc::string::format( "EUPDATE_INITIALIZE_SYSTEM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	if( m_pMainUI )
		m_pMainUI->InitializeUI( pLottoSystem );
}

void GLLottoSystemManUI::UIUpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pGaeaClient->PrintConsoleText(
		sc::string::format( "EUPDATE_TURN %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// ���� ��÷�� ����;
	if( m_pMainUI )
		m_pMainUI->UpdateAccumulateMoneyTurnProc( pLottoSystem );

	// ��÷�� ����Ʈ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		if( m_pLottoWinManListInfo )
		{
			m_pLottoWinManListInfo->UpdateWinManList( pLottoSystem );
			m_pLottoWinManListInfo->UpdateTurnProc( pLottoSystem );
		}
	}
}

void GLLottoSystemManUI::UIUpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pGaeaClient->PrintConsoleText(
		sc::string::format( "EUPDATE_DRAWING_DATE %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	// ��÷ ��¥ ����;
	if( m_pMainUI )
		m_pMainUI->UpdateDrawingDateProc( pLottoSystem );
}

void GLLottoSystemManUI::UIUpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	// ���� �ð� ����;
	if( m_pMainUI )
		m_pMainUI->UpdateProgressTimeProc( pLottoSystem );
}

void GLLottoSystemManUI::UIUpdateCurrentStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pGaeaClient->PrintConsoleText(
		sc::string::format( "EUPDATE_CURRENT_STATE %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// Ư�� ���� ��÷ ��ȣ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		const unsigned int nBuyCount = pLottoSystem->GetConfirmBuyCount();
		if( m_pLottoBuyListInfo && nBuyCount )
			m_pLottoBuyListInfo->UpdateChangeState( pLottoSystem );
	}

	if( m_pMainUI )
	{
		// ��÷ ��¥ ����;
		m_pMainUI->UpdateDrawingDateStateProc( pLottoSystem );

		// ��÷�� ����;
		m_pMainUI->UpdateMachineTurn( pLottoSystem );

		// ���Ÿ���Ʈ ����;
		m_pMainUI->UpdateBuyListStateProc( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIUpdateChangeStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pGaeaClient->PrintConsoleText(
		sc::string::format( "EUPDATE_CHANGE_STATE %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// Ư�� ���� ��÷ ��ȣ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		const unsigned int nBuyCount = pLottoSystem->GetConfirmBuyCount();
		if( m_pLottoBuyListInfo && nBuyCount )
			m_pLottoBuyListInfo->UpdateChangeState( pLottoSystem );
	}

	if( m_pMainUI )
	{
		// ��÷ ��¥ ����;
		m_pMainUI->UpdateDrawingDateStateProc( pLottoSystem );

		// ��÷�� ����;
		m_pMainUI->UpdateMachineTurn( pLottoSystem );

		// ���Ÿ���Ʈ ����;
		m_pMainUI->UpdateBuyListStateProc( pLottoSystem );
	}

	// ���� ���� �޽��� ���;
	switch( pLottoSystem->GetLottoSystemState() )
	{
	case LottoSystem::LOTTO_STATE_DRAWING:
		m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_LOTTO_START,
			sc::string::format( ID2GAMEINTEXT("LOTTO_DRAWING_STATE_START_TEXT"),
			pLottoSystem->GetCurTurnNum()+1 ).c_str() );

		m_pInterface->GetGaeaClient()->PrintConsoleText( 
			sc::string::format( ID2GAMEINTEXT("LOTTO_DRAWING_STATE_START_TEXT"),
			pLottoSystem->GetCurTurnNum()+1 ).c_str() );
		break;
	//case LottoSystem::LOTTO_STATE_BASIC:
	case LottoSystem::LOTTO_STATE_GATHERING:
		m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_LOTTO_END,
			sc::string::format( ID2GAMEINTEXT("LOTTO_DRAWING_STATE_END_TEXT"),
			pLottoSystem->GetCurTurnNum() ).c_str() );

		m_pInterface->GetGaeaClient()->PrintConsoleText(
			sc::string::format( ID2GAMEINTEXT("LOTTO_DRAWING_STATE_END_TEXT"),
			pLottoSystem->GetCurTurnNum() ).c_str() );
		break;
	}
}

void GLLottoSystemManUI::UIUpdateBuyListClearProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_BUY_LIST %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	// ���� ����Ʈ ����;
	if( m_pMainUI )
		m_pMainUI->UpdateBuyList( pLottoSystem );
}

void GLLottoSystemManUI::UIUpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_ACCUMULATE_MONEY %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// ���� ���� ��÷�� ����;
	if( m_pMainUI )
		m_pMainUI->UpdateAccumulateMoney( pLottoSystem );

	// Display UI ���� ��÷�� ����;
	if( m_nAccumulateMoneyLEDCheckTabIdx ==
		static_cast<unsigned int>( tabIdx ) )
	{
		if( m_pDisplayUI )
			m_pDisplayUI->UpdateAccumulateMoney( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIUpdateTurnAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_TURN_ACCUMULATE_MONEY %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// ��÷�� ����Ʈ�� Ư�� ���� ���� ��÷�� ���� ����;
	if( m_nCurTabIndex == tabIdx )
	{
		if( m_pLottoWinManListInfo )
			m_pLottoWinManListInfo->UpdateAccumulateMoneyProc( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIUpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_WIN_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	// ��÷ ��ȣ ����;
	if( m_pMainUI )
		m_pMainUI->UpdateWinNum( pLottoSystem );
}

void GLLottoSystemManUI::UIUpdateConfirmTurnBuyListProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_CONFIRM_TURN_BUY_LIST %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// Ư�� ���� ���� ����Ʈ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		const unsigned int nBuyCount = pLottoSystem->GetConfirmBuyCount();
		if( m_pLottoBuyListInfo && nBuyCount )
			m_pLottoBuyListInfo->UpdateBuyList( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIUpdateConfirmTurnWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_CONFIRM_TURN_WIN_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// Ư�� ���� ��÷ ��ȣ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		const unsigned int nBuyCount = pLottoSystem->GetConfirmBuyCount();
		if( m_pLottoBuyListInfo && nBuyCount )
			m_pLottoBuyListInfo->UpdateWinNumProc( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIUpdateTurnWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_TURN_WIN_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// Ư�� ���� ��÷ ��ȣ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		if( m_pLottoWinManListInfo )
			m_pLottoWinManListInfo->UpdateWinNumProc( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIUpdateWinManListProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_WIN_MAN_LIST %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	// Ư�� ���� ��÷�� ����Ʈ ����;
	if( m_nCurTabIndex == tabIdx )
	{
		if( m_pLottoWinManListInfo )
			m_pLottoWinManListInfo->UpdateWinManList( pLottoSystem );
	}
}

// ���� ������ ���� ����;
void GLLottoSystemManUI::UIUpdateHaveMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
	LottoSystem::LOTTO_SYSTEM_CLIENT_VALUE sValue = 
		LottoSystem::GLLottoGlobalDataClient::Instance()
		->GetLottoSystemValue( static_cast<unsigned int>(
		pLottoSystem->GetLottoSystemId() ) );
	int tabIdx = sValue.useIndex;

	if( m_nCurTabIndex == tabIdx )
	{
		if( m_pLottoBuyLottoUI )
			m_pLottoBuyLottoUI->UpdateHaveMoney( pLottoSystem );
	}
}

void GLLottoSystemManUI::UIViewRandomTypeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_RANDOM_TYPE %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	m_pInterface->GetGaeaClient()->PrintConsoleText( "Lotto DrawingType : Random");
}

void GLLottoSystemManUI::UIViewInventTypeProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_INVENT_TYPE %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	m_pInterface->GetGaeaClient()->PrintConsoleText( "Lotto DrawingType : Invent");
}

void GLLottoSystemManUI::UIViewIncorrectSetWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_INCORRECT_SET_WIN_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	m_pInterface->GetGaeaClient()->PrintConsoleText( "Failed Lotto Win Num Count");
}

void GLLottoSystemManUI::UIViewLockBuyProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_LOCK_BUY %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	//m_pGaeaClient->PrintConsoleText( "Completed to change buyLockState");
}

void GLLottoSystemManUI::UIViewLockChangeTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_LOCK_CHANGE_TURN %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	//m_pGaeaClient->PrintConsoleText( "Completed to change turnLockState");
}

void GLLottoSystemManUI::UIUpdateNumDSPProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_NUM_DSP %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	int num = 0;
	sc::Json::find_value( outputObj, "num", num );
	int drawingCount = 0;
	sc::Json::find_value( outputObj, "drawingCount", drawingCount );

	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "%d : %d",
		num, drawingCount ) );
}

void GLLottoSystemManUI::UIUpdateNumProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	int num = 0;
	sc::Json::find_value( outputObj, "num", num );
	int addCount = 0;
	sc::Json::find_value( outputObj, "addCount", addCount );

	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "%d : %d",
		num, addCount ) );
}

void GLLottoSystemManUI::UIViewPostMessageProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_POST_MESSAGE %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	int turnNum = 0;
	sc::Json::find_value( outputObj, "turnNum", turnNum );
	int ranking = 0;
	sc::Json::find_value( outputObj, "ranking", ranking );
	std::string strWinMoney;
	sc::Json::find_value( outputObj, "winMoney", strWinMoney );

	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( ID2GAMEINTEXT( "LOTTO_GIVE_MONEY_POST_CONTENT_TEXT" ),
		turnNum+1, ranking, strWinMoney ) );
}

void GLLottoSystemManUI::UIViewNumDSPProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_NUM_DSP %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	std::string strText;
	sc::Json::find_value( outputObj, "context", strText );
	m_pInterface->GetGaeaClient()->PrintConsoleText( strText );
}

void GLLottoSystemManUI::UIUpdateDrawingNumProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_DRAWING_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	int drawingNum = 0;
	sc::Json::find_value( outputObj, "drawingNum", drawingNum );
	if( m_pMainUI )
		m_pMainUI->UpdateMachine( pLottoSystem, drawingNum );
}

void GLLottoSystemManUI::UIViewCorrectSetWinNumProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EVIEW_CORRECT_SET_WIN_NUM %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	std::string strText;
	sc::Json::find_value( outputObj, "winNum", strText );
	m_pInterface->GetGaeaClient()->PrintConsoleText( strText );
}

void GLLottoSystemManUI::UIUpdateBuyListProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_BUY_LIST %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	int index = 0;
	sc::Json::find_value( outputObj, "index", index );
	if( m_pMainUI )
		m_pMainUI->UpdateBuyList( pLottoSystem, index );
}

void GLLottoSystemManUI::UIUpdateBuyListCountProc(
	const LottoSystem::CLottoSystemClient* pLottoSystem,
	const json_spirit::mObject outputObj )
{
#ifdef _EVENT_BOX_MESSAGE_LOG
	m_pInterface->GetGaeaClient()->PrintConsoleText(
		sc::string::format( "EUPDATE_BUY_LIST_COUNT %d",
		static_cast<int>( pLottoSystem->GetLottoSystemId() ) ) );
#endif

	int count = 0;
	sc::Json::find_value( outputObj, "count", count );
	if( m_pMainUI )
		m_pMainUI->UpdateBuyListCount( pLottoSystem, count );
}