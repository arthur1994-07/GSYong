#include "pch.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "../../RanLogic/TexasHoldem/GLTexasHoldemSetting.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../Util/GLItemLimit.h"

#include "../Database/DBAction/DbActionGameTexasHoldem.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "../Crow/GLCrow.h"

#include "./GLTexasHoldemWeb.h"
#include "./GLTexasHoldemField.h"
#include "../../SigmaCore/DebugInclude.h"

namespace TexasHoldem
{


	GLTexasHoldemField::GLTexasHoldemField( GLGaeaServer* pServer )
		: m_pServer( pServer )
	{
		m_TexasHoldemStartGameDelayMgr.SetDelay( 1500 );
	}

	GLTexasHoldemField::~GLTexasHoldemField()
	{
	}

	void GLTexasHoldemField::FrameMove( float fElapsedTime )
	{

	}

	void GLTexasHoldemField::MessageProcess( NET_MSG_GENERIC* pMessage, DWORD ClientID /*= 0*/, DWORD GaeaID /*= 0 */ )
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
			MsgChipMapVersion( pMessage, ClientID, GaeaID );
			break;
		case ChipMap:
			MsgChipMap( pMessage, ClientID, GaeaID );
			break;
		case ChipMapEnd:
			MsgChipMapEnd( pMessage, ClientID, GaeaID );
			break;
		case UseCardGameCardCF:
			MsgUseCardGameCardCF( pMessage, ClientID, GaeaID );
			break;
		case OpenMainMenuCF:
			MsgOpenMainMenuCF( pMessage, ClientID, GaeaID );
			break;
		case CloseMainMenuCF:
			MsgCloseMainMenuCF( pMessage, ClientID, GaeaID );
			break;
		case ChipMapRequest:
			MsgChipMapRequest( pMessage, ClientID, GaeaID );
			break;
		case ChipPurchaseInGameMoney:
			MsgChipPurchaseInGameMoney( pMessage, ClientID, GaeaID );
			break;
		case ChipPurchaseInPoint:
			MsgChipPurchaseInPoint( pMessage, ClientID, GaeaID );
			break;
		case ChipMapRequestInner:
			MsgChipMapRequestInner( pMessage, ClientID, GaeaID );
			break;
		case StartGame:
			MsgStartGame( pMessage, ClientID, GaeaID );
			break;
		case EndGame:
			MsgEndGame( pMessage, ClientID, GaeaID );
			break;
		case ChipInner:
			MsgChipInner( pMessage, ClientID, GaeaID );
			break;
		case AuthKeyInner:
			MsgAuthKeyInner( pMessage, ClientID, GaeaID );
			break;
		default:
			break;
		}
	}

	void GLTexasHoldemField::SendChipMap( DWORD ClientID, bool bFirst /*= false */ )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION NetMsgVersion( GLTexasHoldemMgr::GetVersion() );
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgVersion );

		const std::vector< sChipPrice >& vecChip = GetChipMap();
		size_t ChipMapSize = vecChip.size();
		for ( size_t loop = 0; loop < ChipMapSize; loop++  )
		{
			GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP NetMsg( vecChip[loop] );
			m_pServer->SENDTOCLIENT( ClientID, &NetMsg );
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END NetMsg;
		NetMsg.bFirst = bFirst;
		m_pServer->SENDTOCLIENT( ClientID, &NetMsg );
	}

	void GLTexasHoldemField::MsgChipMapVersion( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_VERSION* ) pMessage;
		GLTexasHoldemMgr::Reset();
		GLTexasHoldemMgr::SetVersion( nmg->Version );

		sc::writeLogInfo( sc::string::format( "TexasHoldemChipMap version %1%", nmg->Version ) );
	}

	void GLTexasHoldemField::MsgChipMap( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP* ) pMessage;
		GLTexasHoldemMgr::InsertChipPrice( nmg->sChip );
	}

	void GLTexasHoldemField::MsgChipMapEnd( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_END* ) pMessage;
	}

	void GLTexasHoldemField::MsgUseCardGameCardCF( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_USE_CARDGAME_CARD* ) pMessage;

		GLMSG::NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB NetMsgFB;
		NetMsgFB.nResult = CheckInvenItem( pCharacter, nmg->m_PosX, nmg->m_PosY );
		if ( eSuccess != NetMsgFB.nResult )
		{
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		NetMsgFB.nResult = GLTexasHoldemMgr::CheckRequirement( pCharacter->GetLevel() );
		if ( eSuccess == NetMsgFB.nResult )
		{
			// 버전정보 넣어준다.
			NetMsgFB.Version = GLTexasHoldemMgr::GetVersion();
		}
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );

		// chip 정보를 넘겨준다.
		LONGLONG chip = pCharacter->GetTexasHoldemChip();
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE NetMsgChip( chip );
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgChip );
	}

	void GLTexasHoldemField::MsgOpenMainMenuCF( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_OPEN_MAIN_MENU_CF* ) pMessage;

		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHECK_REQUIREMENT_FB NetMsg;

		NetMsg.nResult = CheckNpc( pCharacter, nmg->dwNpcGaeaID );
		if ( eSuccess != NetMsg.nResult )
		{
			m_pServer->SENDTOCLIENT( ClientID, &NetMsg );
			return;
		}
		
		NetMsg.nResult = GLTexasHoldemMgr::CheckRequirement( pCharacter->GetLevel() );
		if ( eSuccess == NetMsg.nResult )
		{
			// 버전정보 넣어준다.
			NetMsg.Version = GLTexasHoldemMgr::GetVersion();
		}
		m_pServer->SENDTOCLIENT( ClientID, &NetMsg );

		// chip 정보를 넘겨준다.
		LONGLONG chip = pCharacter->GetTexasHoldemChip();
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE NetMsgChip( chip );
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgChip );
	}

	void GLTexasHoldemField::MsgCloseMainMenuCF( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_CF* ) pMessage;

		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CLOSE_MAIN_MENU_FC NetMsg;
		m_pServer->SENDTOCLIENT( ClientID, &NetMsg );
	}

	void GLTexasHoldemField::MsgChipMapRequest( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		SendChipMap( ClientID );
	}

	void GLTexasHoldemField::MsgChipPurchaseInGameMoney( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			// 통계 및 로그 수정
			// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
			sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgChipPurchaseInGameMoney ", GaeaID));
			return;
		}

		GLTexasHoldemSetting* const pSettings = GLTexasHoldemSetting::getInstance();
		if ( NULL == pSettings )
		{
			return;
		}

		if ( false == pSettings->CanChipPurchaseInGameMoney() )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_GAMEMONEY* ) pMessage;

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB NetMsgFB;
		NetMsgFB.nResult = CheckNpc( pCharacter, nmg->dwNpcGaeaID );
		if ( eSuccess != NetMsgFB.nResult )
		{
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		LONGLONG GameMoneyPrice = 0;
		bool bReturn = GetChipPriceOfGameMoney( nmg->Chip, GameMoneyPrice );
		if ( false == bReturn )
		{
			// 리스트에 없다.
			NetMsgFB.nResult = eWrongChip;
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		if ( GameMoneyPrice > pCharacter->GetInvenMoney() )
		{
			NetMsgFB.nResult = eLackOfGameMoney;
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		// 인벤토리 게임 머니에서 먼저 인출
		pCharacter->SubtractInvenMoneyServer( GameMoneyPrice, SUBTRACT_INVEN_MONEY_TEXASHOLDEM_CHIP_BUY );
		pCharacter->SendInvenMoneyUpdate();

		// 칩 구매 로그 기록
		// 통계 및 로그 수정
		pCharacter->LogMoneyExchange( gamelog::ID_CHAR, pCharacter->CharDbNum(), gamelog::ID_CHAR, 
			pCharacter->CharDbNum(), -GameMoneyPrice, MONEY_ROUTE_BUY_TEXASHOLDEM_CHIP, (pCharacter == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCharacter->GetCurrentMap(), LOG_MONEY_TEXASHOLDEM_BUY );
		// 통계 및 로그 수정
		pCharacter->LogMoneyExchange( gamelog::ID_CHAR, pCharacter->CharDbNum(), gamelog::ID_CHAR, 
			pCharacter->CharDbNum(), pCharacter->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, (pCharacter == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCharacter->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);

		// chip 을 넣어준다. 
		pCharacter->AddTexasHoldemChip( nmg->Chip );

		// chip 변경 사항 알림
		LONGLONG chip = pCharacter->GetTexasHoldemChip();
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE NetMsgChip( chip );
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgChip );

		// chip save
		// 구조상 더해주는 것을 저장한다.
		SaveChip( pCharacter->CharDbNum(), nmg->Chip, true );

		// chip log save
		SaveChipPurchaseLogGameMoney( pCharacter->CharDbNum(), nmg->Chip, GameMoneyPrice );

		// 피드백
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
	}

	void GLTexasHoldemField::MsgChipPurchaseInPoint( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			return;
		}

		GLTexasHoldemSetting* const pSettings = GLTexasHoldemSetting::getInstance();
		if ( NULL == pSettings )
		{
			return;
		}

		if ( false == pSettings->CanChipPurchaseInPoint() )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_IN_POINT* ) pMessage;

		GLMSG::NET_TEXAS_HOLD_EM_CHIP_PURCHASE_FB NetMsgFB;
		NetMsgFB.nResult = CheckNpc( pCharacter, nmg->dwNpcGaeaID );
		if ( eSuccess != NetMsgFB.nResult )
		{
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}


		LONGLONG PointPrice = 0;
		bool bReturn = GetChipPriceOfRanPoint( nmg->Chip, PointPrice );
		if ( false == bReturn )
		{
			// 리스트에 없다.
			NetMsgFB.nResult = eWrongChip;
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		if ( PointPrice > pCharacter->PointShopPoint() )
		{
			NetMsgFB.nResult = eLackOfPoint;
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		// 포인트 차감, 포인트 차감에 현재 구조상 아이템 아이디가 필요하다.
		// 기본 아이템을 추가를 할지? 아님 로직 수정이 필요하다.
		SNATIVEID sChip( 1, 1 );
		sChip.wMainID = pSettings->GetPointTransferItemMid();
		sChip.wSubID = pSettings->GetPointTransferItemSid();
		pCharacter->PointShopPointSubByHoldem( sChip, PointPrice );

		// chip 을 넣어준다. 
		pCharacter->AddTexasHoldemChip( nmg->Chip );

		// chip 변경 사항 알림
		LONGLONG chip = pCharacter->GetTexasHoldemChip();
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE NetMsgChip( chip );
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgChip );

		// chip save
		// 구조상 더해주는 것을 저장한다.
		SaveChip( pCharacter->CharDbNum(), nmg->Chip, true );

		// chip log save
		SaveChipPurchaseLogPoint( pCharacter->CharDbNum(), nmg->Chip, PointPrice );

		// 피드백
		m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
	}

	void GLTexasHoldemField::MsgChipMapRequestInner( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST NetMsg;
		m_pServer->SENDTOAGENT( &NetMsg );
	}
	
	void GLTexasHoldemField::MsgStartGame( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			return;
		}

		DWORD dwChaDbNum = pCharacter->CharDbNum();
		if ( m_TexasHoldemStartGameDelayMgr.IsSearchLimitTime( dwChaDbNum ) )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_START_GAME* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_START_GAME* ) pMessage;

		GLMSG::NET_TEXAS_HOLD_EM_START_GAME_FB NetMsgFB;
		NetMsgFB.nResult = CheckNpc( pCharacter, nmg->dwNpcGaeaID );
		if ( eSuccess != NetMsgFB.nResult )
		{
			m_pServer->SENDTOCLIENT( ClientID, &NetMsgFB );
			return;
		}

		// uuid 생성
		UUID sGUID;
		sc::SeqUniqueGUID::Instance()->getGUID( sGUID );

		SaveAuthKey( dwChaDbNum, sGUID );
	}

	void GLTexasHoldemField::MsgEndGame( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLChar* pCharacter = m_pServer->GetChar( GaeaID );
		if ( NULL == pCharacter )
		{
			return;
		}

		LoadChip( pCharacter->CharDbNum(), true );
	}

	void GLTexasHoldemField::MsgChipInner( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_INNER* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_CHIP_INNER* ) pMessage;

		GLChar* pCharacter = m_pServer->GetCharByDbNum( nmg->dwChaDbNum );
		if ( NULL == pCharacter )
		{
			return;
		}

		pCharacter->SetTexasHoldemChip( nmg->Chip );

		// chip 변경 사항 알림
		LONGLONG chip = pCharacter->GetTexasHoldemChip();
		GLMSG::NET_TEXAS_HOLD_EM_CHIP_UPDATE NetMsgChip( chip );
		m_pServer->SENDTOCLIENT( pCharacter->GETCLIENTID(), &NetMsgChip );

		if ( nmg->bEndGameNotify )
		{
			//! 캐릭터 이동불가상태 풀기
			pCharacter->SetMovement( CHAR_MOVEMENT_STATE_MINIGAME_TEXASHOLDEM, true );

			GLMSG::NET_TEXAS_HOLD_EM_END_GAME_FB NetMsgFB;
			m_pServer->SENDTOCLIENT( pCharacter->GETCLIENTID(), &NetMsgFB );

			int nSecond = m_TexasHoldemGameTimeMgr.GetGameTime( pCharacter->CharDbNum() );
			if ( -1 != nSecond )
			{
				// dbaction
				SaveGameTime( pCharacter->CharDbNum(), pCharacter->GetUserID(), nSecond );
			}
		}
	}

	void GLTexasHoldemField::MsgAuthKeyInner( NET_MSG_GENERIC* pMessage, DWORD ClientID, DWORD GaeaID )
	{
		GLMSG::NET_TEXAS_HOLD_EM_AUTHKEY_INNER* nmg = ( GLMSG::NET_TEXAS_HOLD_EM_AUTHKEY_INNER* ) pMessage;

		GLChar* pCharacter = m_pServer->GetCharByDbNum( nmg->dwChaDbNum );
		if ( NULL == pCharacter )
		{
			return;
		}

		GLMSG::NET_TEXAS_HOLD_EM_START_GAME_FB NetMsgFB;

		LONGLONG chip = pCharacter->GetTexasHoldemChip();
		if( chip <= 0 )
		{
			NetMsgFB.nResult = eWebErrorChip;
			m_pServer->SENDTOCLIENT( pCharacter->GETCLIENTID(), &NetMsgFB );
			return;
		}

		std::string strGUID = sc::string::uuidToString( nmg->sGUID );
		std::string strPOST = GetPostArgument( m_pServer->GetServerGroup(), pCharacter->CharDbNum(), strGUID );
		if ( strPOST.empty() )
		{
			NetMsgFB.nResult = ePostArgumentEmpty;
			m_pServer->SENDTOCLIENT( pCharacter->GETCLIENTID(), &NetMsgFB );
			return;
		}

		if ( ePostArgumentSize > static_cast< int >( strPOST.size() ) )
		{
			//! 캐릭터 이동불가상태 만들기
			pCharacter->SetMovement( CHAR_MOVEMENT_STATE_MINIGAME_TEXASHOLDEM, false );

			NetMsgFB.SetPostArgument( strPOST.c_str() );
			m_pServer->SENDTOCLIENT( pCharacter->GETCLIENTID(), &NetMsgFB );

			m_TexasHoldemGameTimeMgr.AddUser( pCharacter->CharDbNum() );
		}
		else
		{
			sc::writeLogError( "strPOST.size() > ePostArgumentSize" );
			NetMsgFB.nResult = ePostArgumentSizeError;
			m_pServer->SENDTOCLIENT( pCharacter->GETCLIENTID(), &NetMsgFB );
		}
	}

	void GLTexasHoldemField::SaveChip( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus /*= false*/ )
	{
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::TexasHoldemChipSave( dwChaDbNum, chip, bPlus ) ) );
	}

	void GLTexasHoldemField::SaveChipPurchaseLogGameMoney( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney )
	{
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::TexasHoldemChipLogSave( dwChaDbNum, chip, GameMoney, 0 ) ) );
	}

	void GLTexasHoldemField::SaveChipPurchaseLogPoint( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& Point )
	{
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::TexasHoldemChipLogSave( dwChaDbNum, chip, 0, Point ) ) );
	}

	int GLTexasHoldemField::CheckNpc( GLChar* pCharacter, DWORD dwNpcGaeaID )
	{
		if ( NULL == pCharacter )
		{
			return eCharacterIsNull;
		}

		if ( pCharacter->UserLevel() >= USER_GM3 )
		{
			return eSuccess;
		}

		const GLLandMan* pLand = pCharacter->GetLandMan();
		if ( NULL == pLand )
		{
			return eLandIsNull;
		}

		const GLCrow* pCrow = pLand->GetCrow( dwNpcGaeaID );
		if ( NULL == pCrow )
		{
			return eCrowIsNull;
		}

		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCrow->GetNativeId() );
		if ( NULL == pCrowData )
		{
			return eCrowIsNull;
		}

		if ( NPC_TYPE_TEXASHOLDEM != pCrowData->m_emNPCType )
		{
			return eCrowType;
		}
        if ( !pCrow->IsNpcInteractionAble(pCharacter->GetPosition(), pCharacter->GETBODYRADIUS()))
        {
            return eCrowDistance;
        }
		return eSuccess;
	}

	int GLTexasHoldemField::CheckInvenItem( GLChar* pCharacter, DWORD wPosX, DWORD wPosY )
	{
		if ( NULL == pCharacter )
		{
			// 통계 및 로그 수정
			// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
			sc::writeLogError(std::string("Can not find GetCharByDbNum on CheckInvenItem "));
			return eCharacterIsNull;
		}

		const GLLandMan* pLand = pCharacter->GetLandMan();
		if ( NULL == pLand )
		{
			return eLandIsNull;
		}

		const SINVENITEM* pInvenItem = pCharacter->InvenGetItem( wPosX, wPosY );
		if ( !pInvenItem )
		{
			return eItemIsNull;
		}

		const SITEM* pItemData = GLogicData::GetInstance().GetItem(pInvenItem->GetNativeID());
		if ( !pItemData )
		{
			return eItemIsNull;
		}

		if ( pCharacter->CheckCoolTime( pInvenItem->GetNativeID() ) )
		{
			return eItemCoolTime;
		}

		EMITEM_TYPE ItemType = pItemData->BasicType();
		if ( ItemType != ITEM_CARDGAME_CARD )
		{
			return eWrongItemType;
		}

		// 아이템사용 요구레벨 체크
		if( !pCharacter->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
		{
			return eRequireLevel;
		}

		// 아이템 삭제 로그
		GLItemLimit* pItemLimit = m_pServer->GetItemLimit();
		if ( pItemLimit )
		{
			// 통계 및 로그 수정
			pItemLimit->LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, pCharacter->CharDbNum(),
				gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, pInvenItem->TurnNum(), 
				(pCharacter == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCharacter->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TEXASHOLDEM,
				false );
		}

		// 아이템 삭제
		if ( !pCharacter->InvenDeleteItem( wPosX, wPosY, true, FALSE, 1785, true ) )
		{
			return eItemIsNull;
		}
		
		return eSuccess;
	}

	void GLTexasHoldemField::LoadChip( DWORD dwChaDbNum, bool bEndGameNotify /*= false*/ )
	{
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::TexasHoldemChipLoad( dwChaDbNum, bEndGameNotify ) ) );
	}

	void GLTexasHoldemField::SaveAuthKey( DWORD dwChaDbNum, UUID& sGUID )
	{
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::TexasHoldemAuthKeySave( dwChaDbNum, sGUID ) ) );
	}

	void GLTexasHoldemField::SaveGameTime( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond )
	{
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::TexasHoldemGameTimeSave( dwChaDbNum, dwUserNum, nSecond ) ) );
	}
}
