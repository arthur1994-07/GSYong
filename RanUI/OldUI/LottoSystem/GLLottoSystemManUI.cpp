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

GLLottoSystemManUI::GLLottoSystemManUI()
: m_pInterface( NULL )
, m_bConfirm( true )
, m_nCurTabIndex( 0xffffffff )
, m_nAccumulateMoneyLEDCheckTabIdx( 0xffffffff )
, m_pMainUI( NULL )
, m_pLottoBuyLottoUI( NULL )
, m_pLottoBuyListInfo( NULL )
, m_pLottoWinManListInfo( NULL )
, m_pDisplayUI( NULL )
, m_pNotifyButton( NULL )
{

}

GLLottoSystemManUI::~GLLottoSystemManUI() { }

void GLLottoSystemManUI::Initialize()
{
	if( m_pInterface )
	{
		m_pMainUI = m_pInterface->GetLottoMainUI();
		m_pLottoBuyLottoUI = m_pInterface->GetLottoBuyLottoUI();
		m_pLottoBuyListInfo = m_pInterface->GetLottoBuyListInfo();
		m_pLottoWinManListInfo = m_pInterface->GetLottoWinManListInfo();
		m_pDisplayUI = m_pInterface->GetLottoDisplayUI();
		m_pNotifyButton = m_pInterface->GetLottoNotifyButton();
	}
}

void GLLottoSystemManUI::Destroy()
{
	m_pMainUI = NULL;
	m_pLottoBuyLottoUI = NULL;
	m_pLottoBuyListInfo = NULL;
	m_pLottoWinManListInfo = NULL;
	m_pDisplayUI = NULL;
	m_pNotifyButton = NULL;
}

bool GLLottoSystemManUI::Update( float fElapsedTime )
{
	// Update Machine;
	int lottoSystemCount = LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemCount();
	for( int i=0; i<lottoSystemCount; ++i )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()
			->GetLottoSystem( i );

		if( m_pMainUI )
			m_pMainUI->UpdateMachineAnimation( pLottoSystem, fElapsedTime );
	}

	// Event;
	EventProc( fElapsedTime );

	return true;
}

void GLLottoSystemManUI::EventProc( float fElapsedTime )
{
	int lottoSystemCount = LottoSystem::GLLottoSystemManClient::Instance()
		->GetLottoSystemCount();
	for( int i=0; i<lottoSystemCount; ++i )
	{
		LottoSystem::CLottoSystemClient* pLottoSystem =
			LottoSystem::GLLottoSystemManClient::Instance()
			->GetLottoSystem( i );

		if( !pLottoSystem )
			continue;

		LottoSystem::EVENT_BOX curEvent = LottoSystem::NONE;
		while( curEvent = static_cast<LottoSystem::EVENT_BOX>(
			pLottoSystem->NextEvent() ) )
		{
			switch( curEvent )
			{
			case LottoSystem::EUPDATE_INITIALIZE_SYSTEM:
				UIInitializeProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_TURN:					// 회차 변경 이벤트;
				UIUpdateTurnProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_DRAWING_DATE:			// 추첨일 전체 갱신;
				UIUpdateDrawingDateProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_PROGRESS_TIME:		// 진행상태 시간 갱신;
				UIUpdateProgressTimeProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_CURRENT_STATE:		// 현재상태 갱신;
				UIUpdateCurrentStateProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_CHANGE_STATE:			// 현재상태 변경 갱신;
				UIUpdateChangeStateProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_BUY_LIST:
				UIUpdateBuyListClearProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_CONFIRM_BUY_LIST:
				UIUpdateConfirmTurnBuyListProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_ACCUMULATE_MONEY:		// 누적 당첨금 갱신 이벤트;
				UIUpdateAccumulateMoneyProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_TURN_ACCUMULATE_MONEY:
				UIUpdateTurnAccumulateMoneyProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_WIN_NUM:
				UIUpdateWinNumProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_CONFIRM_TURN_WIN_NUM:
				UIUpdateConfirmTurnWinNumProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_TURN_WIN_NUM:
				UIUpdateTurnWinNumProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_WIN_MAN_LIST:
				UIUpdateWinManListProc( pLottoSystem );
				break;
			case LottoSystem::EUPDATE_HAVE_MONEY:
				UIUpdateHaveMoneyProc( pLottoSystem );
				break;
			case LottoSystem::EVIEW_RANDOM_TYPE:
				UIViewRandomTypeProc( pLottoSystem );
				break;
			case LottoSystem::EVIEW_INVENT_TYPE:
				UIViewInventTypeProc( pLottoSystem );
				break;
			case LottoSystem::EVIEW_INCORRECT_SET_WIN_NUM:
				UIViewIncorrectSetWinNumProc( pLottoSystem );
				break;
			case LottoSystem::EVIEW_LOCK_BUY:
				UIViewLockBuyProc( pLottoSystem );
				break;
			case LottoSystem::EVIEW_LOCK_CHANGE_TURN:
				UIViewLockChangeTurnProc( pLottoSystem );
				break;
			}
		}

		std::string curEventString = pLottoSystem->NextJsonEvent();
		while( !curEventString.empty() )
		{	
			json_spirit::mValue outputValue;

			if( !json_spirit::read( curEventString, outputValue ) )
				return;

			const json_spirit::mObject outputObj = outputValue.get_obj();

			int tempId = 0;
			if( !sc::Json::find_value( outputObj, "id", tempId ) )
				continue;

			LottoSystem::EVENT_BOX curEventId =
				static_cast<LottoSystem::EVENT_BOX>( tempId );

			switch( curEventId )
			{
			case LottoSystem::EUPDATE_NUM_DSP:
				UIUpdateNumDSPProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EUPDATE_NUM:
				UIUpdateNumProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EVIEW_POST_MESSAGE:
				UIViewPostMessageProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EVIEW_NUM_DSP:
				UIViewNumDSPProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EUPDATE_DRAWING_NUM:
				UIUpdateDrawingNumProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EVIEW_CORRECT_SET_WIN_NUM:
				UIViewCorrectSetWinNumProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EUPDATE_BUY_LIST:
				UIUpdateBuyListProc( pLottoSystem, outputObj );
				break;
			case LottoSystem::EUPDATE_BUY_LIST_COUNT:
				UIUpdateBuyListCountProc( pLottoSystem, outputObj );
				break;
			}

			curEventString = pLottoSystem->NextJsonEvent();
		}
	}
}

void GLLottoSystemManUI::ChangeTabIndex( unsigned int nTabIndex )
{
	m_nCurTabIndex = nTabIndex;

	if( m_pLottoBuyListInfo )
		m_pLottoBuyListInfo->ChangeMainTab();

	if( m_pLottoWinManListInfo )
		m_pLottoWinManListInfo->ChangeMainTab();

	if( m_pLottoBuyListInfo && m_pLottoBuyListInfo->IsVisible() )
		m_pInterface->UiHideGroup( LOTTO_BUY_LIST_INFO );

	if( m_pLottoBuyLottoUI && m_pLottoBuyLottoUI->IsVisible() )
		m_pInterface->UiHideGroup( LOTTO_BUY_LOTTO_UI );

	if( m_pLottoWinManListInfo && m_pLottoWinManListInfo->IsVisible() )
		m_pInterface->UiHideGroup( LOTTO_WIN_MAN_LIST_INFO );
}

void GLLottoSystemManUI::ChangeTurn( unsigned int nTabIndex )
{
	m_bConfirm = false;
}

void GLLottoSystemManUI::ClearLottoNum()
{
	m_lottoNum.Clear();

	m_buyType = LottoSystem::NONE_BUY_TYPE;
}

void GLLottoSystemManUI::OpenMessageBoxMainUI( const CMessageBox::BOXTITLE eBoxTitle
											  , const std::string& strMessage
											  , const CMessageBox::BOXTYPE eBoxType
											  , unsigned int nIdentity )
{
	if( m_pMainUI )
		m_pMainUI->OpenMessageBox( eBoxTitle, strMessage, eBoxType, nIdentity );
}

void GLLottoSystemManUI::InactiveConfirmButtonMainUI()
{
	if( m_pMainUI )
	{
		m_pMainUI->InactiveConfirmButton();
		m_bConfirm = true;
	}
}

void GLLottoSystemManUI::ActiveConfirmButtonMainUI()
{
	if( m_pMainUI )
	{
		m_pMainUI->ActiveConfirmButton();
		m_bConfirm = false;
	}
}

bool GLLottoSystemManUI::IsChildWindowOpen()
{
	return( ( m_pLottoBuyLottoUI && m_pLottoBuyLottoUI->IsVisible() ) ||
		( m_pLottoBuyListInfo && m_pLottoBuyListInfo->IsVisible() ) ||
		( m_pLottoWinManListInfo && m_pLottoWinManListInfo->IsVisible() ) );
}

bool GLLottoSystemManUI::IsBuyLottoUIOpen()
{
	return( m_pLottoBuyLottoUI && m_pLottoBuyLottoUI->IsVisible() );
}