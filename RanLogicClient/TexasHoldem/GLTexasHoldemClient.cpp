#include "pch.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "../../RanLogic/TexasHoldem/GLTexasHoldemSetting.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Land/GLLandManClient.h"
#include "../Char/GLCharacter.h"
#include "../Stage/DxGlobalStage.h"

#include "../GLGaeaClient.h"

#include "GLTexasHoldemClient.h"

#include "../SigmaCore/DebugInclude.h"

namespace TexasHoldem
{
	GLTexasHoldemClient::GLTexasHoldemClient( GLGaeaClient* pGaeaClient )
		: m_pGaeaClient( pGaeaClient )
		, m_Timer( 0.0f )
		, m_dwNpcGaeaID( GAEAID_NULL )
	{
	}

	GLTexasHoldemClient::~GLTexasHoldemClient()
	{

	}

	void GLTexasHoldemClient::FrameMove( float fElapsedTime )
	{
		if ( GAEAID_NULL == m_dwNpcGaeaID )
		{
			return;
		}

		m_Timer += fElapsedTime;
		if ( m_Timer < 1.0f )
		{
			return;
		}
		m_Timer = 0.0f;

		//!! SF_TODO
		/*if ( m_pInterface->IsVisible( MINIGAME_TEXASHOLDEM_WINDOW ) )
		{
			if ( eSuccess != CheckNpc( m_dwNpcGaeaID ) )
			{
				CloseMainMenu();
			}
		}*/
	}

	void GLTexasHoldemClient::MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID /*= 0*/, DWORD GaeaID /*= 0 */ )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		if ( NULL == pMessage )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_MESSAGE* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_MESSAGE* ) pMessage;

		switch ( nmg->eType )
		{
		case ChipMapVersion:
			MsgChipMapVersion( pMessage );
			break;
		case ChipMap:
			MsgChipMap( pMessage );
			break;
		case ChipMapEnd:
			MsgChipMapEnd( pMessage );
			break;
		case CloseMainMenuFC:
			MsgCloseMainMenu( pMessage );
			break;
		case CheckRequirementFB:
			MsgCheckRequirementFB( pMessage );
			break;
		case ChipUpdate:
			MsgChipUpdate( pMessage );
			break;
		case ChipPurchaseFB:
			MsgChipPurchaseFB( pMessage );
			break;
		case StartGameFB:
			MsgStartGameFB( pMessage );
			break;
		case EndGameFB:
			MsgEndGameFB( pMessage );
			break;
		default:
			break;
		}
	}

	void GLTexasHoldemClient::DisplayMsg( int nMsg )
	{
		/*
		eSuccess = 300, 
		eRequireLevel,	// 게임을 시작하려는데 레벨 조건이 만족되지 않는다. 
		eLandIsNull, 
		eCrowIsNull, 
		eCrowDistance, 
		*/

		switch ( nMsg )
		{
		case eSuccess:
			break;
		case eRequireLevel:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT( "TEXASHOLDEM_ERROR_LEVEL" ), GLTexasHoldemSetting::getInstance()->GetRequirLevel() ) );
			break;
		case eLandIsNull:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eLandIsNull" );
			break;
		case eCrowIsNull:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eCrowIsNull" );
			break;
		case eItemIsNull:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eItemIsNull" );
			break;
		case eWrongItemType:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eWrongItemType" );
			break;
		case eItemCoolTime:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eItemCoolTime" );
			break;
		case eCrowType:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eCrowType" );
			break;
		case eCrowDistance:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eCrowDistance" );
			break;
		case eWrongChip:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, "eWrongChip" );
			break;
		case eLackOfGameMoney:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_EXCHANGE_NOT_ENOUGH") );
			break;
		case eLackOfPoint:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_EXCHANGE_NOT_ENOUGH") );
			break;
		case eWebErrorUser:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_ERROR_USER") );
			break;
		case eWebErrorAuth:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_ERROR_AUTH") );
			break;
		case eWebErrorChip:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_ERROR_CHARGE_CHIP") );
			break;
		case eWebErrorUnknown:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_ERROR_UNKNOWN") );
			break;
		case eCantPurchaseChipInGameMoney:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_ERROR_CHIP_GAMEMONEY") );
			break;
		case eCantPurchaseChipInPoint:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEXASHOLDEM_ERROR_CHIP_POINT") );
			break;
		default:
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, std::string( "TexasHoldem unknown message" ) );
			break;
		}
	}

	void GLTexasHoldemClient::MsgChipMapVersion( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION* ) pMessage;
		GLTexasHoldemMgr::Reset();
		GLTexasHoldemMgr::SetVersion( nmg->Version );
	}

	void GLTexasHoldemClient::MsgChipMap( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP* ) pMessage;
		GLTexasHoldemMgr::InsertChipPrice( nmg->sChip );
	}

	void GLTexasHoldemClient::MsgChipMapEnd( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END* ) pMessage;
		
		// nmg->bFirst true 일 경우는 필드서버에 처음에 캐릭터가 생성되었을 경우이고
		// nmg->bFirst false 일 경우는 클라이언트가 가지고 있는 칩 버전이 필드서버와
		// 달라서 칩을 다시 경우이다.
		if ( false == nmg->bFirst )
		{
			// '칩정보, 게임시작, 칩구매 버튼 있는 ui' 오픈
			//!! SF_TODO
			//m_pInterface->OpenTexasHoldemWindow();
		}
	}

	void GLTexasHoldemClient::MsgCheckRequirementFB( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB* ) pMessage;

		// 텍사스홀덤 npc 를 클릭을 하면 서버에서 오는 피드백 메시지
		if ( eSuccess != nmg->nResult )
		{
			DisplayMsg( nmg->nResult );
			CloseMainMenu();
			return;
		}

		// 버전정보가 서버와 다르다면 칩정보를 요청 한다.
		// 칩정보를 다시 받은후에 '칩정보, 게임시작, 칩구매 버튼 있는 ui' 오픈한다.
		// 버전정보가 같다면 바로 '칩정보, 게임시작, 칩구매 버튼 있는 ui' 오픈한다.
		// 참고로 칩은 GLCharacter::GetTexasHoldemChip() 안에 있다.
		if ( GetVersion() == nmg->Version )
		{
			// '칩정보, 게임시작, 칩구매 버튼 있는 ui' 오픈
			//!! SF_TODO
			//m_pInterface->OpenTexasHoldemWindow();
		}
		else
		{
			SetVersion( nmg->Version );
			RequestChipMap();
		}
	}

	void GLTexasHoldemClient::MsgCloseMainMenu( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC* ) pMessage;

		// '칩정보, 게임시작, 칩구매 버튼 있는 ui' 끄기
		//!! SF_TODO
		//m_pInterface->CloseTexasHoldemWindow();
	}

	void GLTexasHoldemClient::MsgChipUpdate( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE* ) pMessage;

		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		pCharacter->SetTexasHoldemChip( nmg->Chip );
	}

	void GLTexasHoldemClient::MsgChipPurchaseFB( NET_MSG_GENERIC* pMessage )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB* ) pMessage;

		if ( eSuccess != nmg->nResult )
		{
			DisplayMsg( nmg->nResult );
			return;
		}
	}

	void GLTexasHoldemClient::MsgStartGameFB( NET_MSG_GENERIC* pMessage )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		GLMSG::NET_TEXAS_HOLD_EM_START_GAME_FB* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_START_GAME_FB* ) pMessage;

		if ( eSuccess != nmg->nResult )
		{
			DisplayMsg( nmg->nResult );
			return;
		}

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		// 메인메뉴 닫는다. 캐릭터 이동불가 상태 만들고 푸는 것은 웹윈도우가 열릴때, 닫힐 때이다.
		CloseMainMenu();

		std::string strURL = GLTexasHoldemSetting::getInstance()->GetAddress();
		std::string strPOST( nmg->szPostArgument );
		//!! SF_TODO
		//m_pInterface->SetTexasHoldemURL( strURL, strPOST );
		//m_pInterface->RunTexasHoldemWindow();
	}

	void GLTexasHoldemClient::MsgEndGameFB( NET_MSG_GENERIC* pMessage )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		GLMSG::NET_TEXAS_HOLD_EM_END_GAME_FB* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_END_GAME_FB* ) pMessage;

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		// ui process
		//!! SF_TODO
		//m_pInterface->StopTexasHoldemWindow();

	}

	void GLTexasHoldemClient::Offline()
	{
		// 유저 동시 접속으로 인하여 로그아웃될 경우 여기로 옴;

		//!! SF_TODO
		//if( m_pInterface->UiIsVisibleGroup( MINIGAME_TEXASHOLDEM_WEB_WINDOW ) == FALSE )
		//	return;

		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		//!! SF_TODO
		//m_pInterface->StopTexasHoldemWindow();
	}

	void GLTexasHoldemClient::RequestChipMap()
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST NetMsg;
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	}

	int GLTexasHoldemClient::CheckNpc( DWORD dwNpcGaeaID )
	{
		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return eCharacterIsNull;
		}

		if ( pCharacter->UserLevel() >= USER_GM3 )
		{
			return eSuccess;
		}

		GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
		if ( NULL == pLand )
			return eLandIsNull;

		//! GM커맨드에서 NULL을 보낸다.
		//! 위에서 GM일 경우는 eSuccess리턴하고
		//! GM이 아닐 경우 NULL인지 검사한다.
		if ( NULL == dwNpcGaeaID )
			return eCrowIsNull;

		const std::tr1::shared_ptr< GLCrowClient > pCrow = pLand->GetCrow( dwNpcGaeaID );
		if ( NULL == pCrow )
			return eCrowIsNull;

		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCrow->GetNativeId() );
		if ( NULL == pCrowData )
		{
			return eCrowIsNull;
		}

		if ( NPC_TYPE_TEXASHOLDEM != pCrowData->m_emNPCType )
		{
			return eCrowType;
		}

        if ( !pCrow->IsNpcActionable(pCharacter->GetPosition(), pCharacter->GETBODYRADIUS()) )
            return eCrowDistance;

		return eSuccess;
	}

	void GLTexasHoldemClient::OpenMainMenu( DWORD dwNpcGaeaID )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		// 텍사스홀덤 npc 클릭할 때 이 메시지를 호출한다.
		int nResult = CheckNpc( dwNpcGaeaID );
		if ( eSuccess != nResult )
		{
			DisplayMsg( nResult );
			return;
		}

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		if ( TexasHoldem::eSuccess != GLTexasHoldemMgr::CheckRequirement( pCharacter->GETLEVEL() ) )
		{
			DisplayMsg( eRequireLevel );
			return;
		}

		m_dwNpcGaeaID = dwNpcGaeaID;

		GLMSG::NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF NetMsg;
		NetMsg.dwNpcGaeaID = dwNpcGaeaID;
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	}

	void GLTexasHoldemClient::CloseMainMenu()
	{
		// 텍사스홀덤 메인 윈도우 닫을 경우 이 메시지를 호출한다.
		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF NetMsg;
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	}

	void GLTexasHoldemClient::RequestChipPurchaseInGameMoney( LONGLONG Chip )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		GLTexasHoldemSetting* const pSettings = GLTexasHoldemSetting::getInstance();
		if ( NULL == pSettings )
		{
			return;
		}

		if ( false == pSettings->CanChipPurchaseInGameMoney() )
		{
			DisplayMsg( eCantPurchaseChipInGameMoney );
			return;
		}

		int nResult = CheckNpc( m_dwNpcGaeaID );
		if ( eSuccess != nResult )
		{
			DisplayMsg( nResult );
			return;
		}

		LONGLONG GameMoneyPrice = 0;
		bool bReturn = GetChipPriceOfGameMoney( Chip, GameMoneyPrice );
		if ( false == bReturn )
		{
			// 리스트에 없다.
			DisplayMsg( eWrongChip );
			return;
		}

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		if ( GameMoneyPrice > pCharacter->GetInvenMoney() )
		{
			DisplayMsg( eLackOfGameMoney );
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY NetMsg( Chip, m_dwNpcGaeaID );
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	}

	void GLTexasHoldemClient::RequestChipPurchaseInPoint( LONGLONG Chip )
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		GLTexasHoldemSetting* const pSettings = GLTexasHoldemSetting::getInstance();
		if ( NULL == pSettings )
		{
			return;
		}

		if ( false == pSettings->CanChipPurchaseInPoint() )
		{
			DisplayMsg( eCantPurchaseChipInPoint );
			return;
		}

		int nResult = CheckNpc( m_dwNpcGaeaID );
		if ( eSuccess != nResult )
		{
			DisplayMsg( nResult );
			return;
		}

		LONGLONG PointPrice = 0;
		bool bReturn = GetChipPriceOfRanPoint( Chip, PointPrice );
		if ( false == bReturn )
		{
			// 리스트에 없다.
			DisplayMsg( eWrongChip );
			return;
		}

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return;
		}

		if ( PointPrice > pCharacter->PointShopPoint() )
		{
			DisplayMsg( eLackOfPoint );
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT NetMsg( Chip, m_dwNpcGaeaID );
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	}

	void GLTexasHoldemClient::RequestInvenDrugCardGameCard(WORD PosX, WORD PosY)
	{
		if( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return;

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
			return;

		//!! SF_TODO
		//if ( m_pInterface->UiIsVisibleGroup( MINIGAME_TEXASHOLDEM_WINDOW ) )
		//	return;

		GLMSG::NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD NetMsg(PosX, PosY);
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	}

	int GLTexasHoldemClient::StartGame()
	{
		if ( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == false )
			return -1;

		const GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( NULL == pCharacter )
		{
			return -1;
		}

		int nResult = CheckNpc( m_dwNpcGaeaID );
		if ( eSuccess != nResult )
		{
			DisplayMsg( nResult );
			return nResult;
		}

		GLMSG::NET_TEXAS_HOLD_EM_START_GAME NetMsg;
		NetMsg.dwNpcGaeaID = m_dwNpcGaeaID;
		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
		return nResult;
	}

	void GLTexasHoldemClient::EndGame()
	{
		DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
		CNetClient* pNetClient = pGlobalStage->GetNetClient();
		if ( pNetClient->IsOnline() && pNetClient->IsOnlineField() )
		{
			GLMSG::NET_TEXAS_HOLD_EM_END_GAME NetMsg;
			m_pGaeaClient->NETSENDTOFIELD( &NetMsg );			
		}
		else
		{
			//!! SF_TODO
			//m_pInterface->StopTexasHoldemWindow();
		}
	}

	void GLTexasHoldemClient::ResetData()
	{
		GLTexasHoldemMgr::Reset();
		m_Timer = 0.0f;
		m_dwNpcGaeaID = 0;
	}
}
