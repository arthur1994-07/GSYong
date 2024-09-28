#include "../pch.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/PostRnMsg.h"

#include "./GLGaeaServer.h"
#include "../Database/DBAction/DbActionGamePost.h"
#include "../Character/GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



BOOL GLGaeaServer::MsgPostSend( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT( IsField() );

    if ( GLUseFeatures::GetInstance().IsUsingPostSendBlock() )
    {
        return FALSE;
    }

	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return FALSE;
	}

	GLMSG::SNET_POST_SEND* msg = ( GLMSG::SNET_POST_SEND* ) nmg;
	
	SPOSTINFO PostInfo;
	ZeroMemory( &PostInfo, sizeof( SPOSTINFO ) );

	GLChar* pChar = GetChar( dwGaeaID );

    if ( pChar == NULL )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_SENDER_NOTEXIST;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}

	// sender 확인
	if ( pChar->CharDbNum() != msg->iSender )
	{
		return FALSE;
	}

	// recver 확인
	if ( pChar->CharDbNum() == msg->iRecver )
	{
		sc::writeLogError( sc::string::format(
			"MsgPostSend() - recver is wrong. recver: %1%, chanum: %2%", 
			msg->iRecver, pChar->CharDbNum() ) );

		GLMSG::SNET_POST_SEND_FB sMsg;
		sMsg.byResut = EMPOST_SAME_ACCOUNT;
		sMsg.iSender = PostInfo.iSender;
		sMsg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &sMsg );

		return FALSE;
	}
	
	PostInfo.byPostState = msg->byPostState;
	PostInfo.byPostType = msg->byPostType;
	PostInfo.byPostClass = msg->byPostClass;
	PostInfo.iSender = msg->iSender;
	PostInfo.iRecver = msg->iRecver;
	PostInfo.nUserNumRecver	= msg->nUserNumRecver;
	PostInfo.nReqID = msg->nReqID;
	PostInfo.iDesign = msg->iDesign;
	PostInfo.llCharge = msg->llCharge;
	PostInfo.llAttachMoney = msg->llAttachMoney;
	PostInfo.bAttach01 = FALSE;
	PostInfo.xAttach01 = msg->xAttach01;
	PostInfo.yAttach01 = msg->yAttach01;
	PostInfo.nAttach01 = msg->nAttach01;
	PostInfo.idAttach01 = msg->idAttach01;
	PostInfo.llRestMoney = pChar->GetInvenMoney();
	PostInfo.dwUserNum = pChar->GetUserID();
	PostInfo.wLevel = pChar->m_wLevel;
	StringCchCopy( PostInfo.RecverName, sizeof( PostInfo.RecverName ), msg->RecverName );
	StringCchCopy( PostInfo.Title, sizeof( PostInfo.Title), msg->Title );
	StringCchCopy( PostInfo.Content, sizeof( PostInfo.Content), msg->Content );
	StringCchCopy( PostInfo.SenderName, sizeof( PostInfo.SenderName ), pChar->m_szName );

	BOOL bGM = FALSE;
	if ( pChar->UserLevel() >= USER_USER_GM )
	{
		bGM = TRUE;
		PostInfo.byPostClass = EMPOST_GM;
	}

	if ( !pChar->IsPOSTBOXUSEABLE( pChar->m_emPOSTBOXIDTYPE, pChar->m_sPOSTBOXID ) )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_NOT_AVAILABLE;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}
	
	
	//
	// 4. Is Recver's account same as Sender's?
	//
	// At this time, we cannot guarantee that the Recver is online.
	// Therefore, Recver's UserNum was requested (by Client) to DB but not Agent.
	//
	int nUserIDSender = pChar->GetUserID();
	if ( nUserIDSender == PostInfo.nUserNumRecver )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_SAME_ACCOUNT;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}

	if ( PostInfo.wLevel < GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_LOW_LEVEL;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}

	DWORD* pdwLastPostSend = &( pChar->m_dwLastPostSend );
	DWORD dwCurrentPostSend = ::GetTickCount();	


	//
	// 6. (GM-pass) Has the PostSend cooltime passed since last postsend ? (6 sec)
	//
	if ( !bGM && *pdwLastPostSend + GLCONST_POST::dwPOST_SEND_COOL_TIME_MS > dwCurrentPostSend )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_COOLTIME_REMAIN;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}

    if ( PostInfo.Title[0] == NULL || PostInfo.Content[0] == NULL )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_EMPTY_TITLE_CONTENT;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;	
	}

	// Attach01
	SITEMCUSTOM sInvenItem;
	if ( PostInfo.xAttach01 != USHRT_MAX || PostInfo.yAttach01 != USHRT_MAX )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_WRONG_ATTACH01;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;

		if ( PostInfo.nAttach01 == 0 )
		{
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		SINVENITEM* pPostItem = pChar->InvenGetItem( PostInfo.xAttach01, PostInfo.yAttach01 );
		if ( !pPostItem )
		{
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		if ( pPostItem->GetNativeID() != PostInfo.idAttach01 )
		{
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		if ( pPostItem->TurnNum() < PostInfo.nAttach01 )
		{
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		if ( pPostItem->sItemCustom.IS_GUID_NULL() )
		{
			sc::writeLogError( sc::string::format(
				"MsgPostSend() - pPostItem->sItemCustom IS_GUID_NULL chanum: %1%", pChar->CharDbNum() ) );
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		GLMSG::SNET_POST_SEND_FB msgNoTrade;
		msgNoTrade.byResut = EMPOST_NOTRADE_ATTACH01;
		msgNoTrade.iSender = PostInfo.iSender;
		msgNoTrade.nReqID  = PostInfo.nReqID;

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( PostInfo.idAttach01 );
		if ( !pITEM )
		{
			SENDTOCLIENT( dwClientID, &msgNoTrade );
			return FALSE;
		}

		//
		// 11-1. 팻카드이면서 팻이 활성화 되어 있으면 거래할 수 없다.
		// 활성화 되어 있지않다면 우편으로 보낼 수 있나? 밑에 거래 불가 체크에 걸리겠지?
		if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			PGLPETFIELD pMyPet = GetPET ( pChar->m_dwPetGUID );
			if ( pMyPet && pMyPet->IsValid () && pPostItem->sItemCustom.PetDbNum() == pMyPet->DbNum() )
			{
				SENDTOCLIENT( dwClientID, &msgNoTrade );
				return FALSE;
			}
		}

		//
		// (GM-pass)
		//
		if ( !bGM )
		{
			//
			// 11-2. 탈것은 거래할 수 없다.
			//
			if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pPostItem->sItemCustom.m_VehicleDbNum != 0 )
			{
				SENDTOCLIENT( dwClientID, &msgNoTrade );
				return FALSE;
			}

			//
			// 11-3. GMCharEdit로 넣은 아이템은 버리기 불가능하다.
			//
			if ( pPostItem->sItemCustom.IsGM_GENITEM() )
			{
				SENDTOCLIENT( dwClientID, &msgNoTrade );
				return FALSE;
			}

			//
			// 11-3. 기간제 아이템은 발송 불가
			//
			if ( pPostItem->sItemCustom.IsTimeLimit() )
			{
				SENDTOCLIENT( dwClientID, &msgNoTrade );
				return FALSE;
			}

			//
			// 11-4. 교환 가능한 아이템인가?
			//
			if ( !( pPostItem->sItemCustom.IsExchange() != NULL ) )
			{
				SENDTOCLIENT( dwClientID, &msgNoTrade );
				return FALSE;
			}

			sInvenItem = pPostItem->sItemCustom;
			PostInfo.bAttach01 = TRUE;
		}
	}

	BOOL bAttachDesign = PostInfo.iDesign ? TRUE : FALSE;
	DWORD dwPostFee = ( DWORD ) GLPost::GetCommision( bAttachDesign, PostInfo.bAttach01, PostInfo.llAttachMoney );

	if ( dwPostFee <= 0 )
	{
		sc::writeLogError( std::string( "PostSend: PostFee MUST greater than 0." ) );

		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_ETC;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;

		SENDTOCLIENT( dwClientID, &msg );

		return FALSE;
	}

	//
	// 12. (GM-deny) Is Charge valid ?
	//
	if ( PostInfo.llCharge > 0 )
	{
		if ( bGM )
		{
			GLMSG::SNET_POST_SEND_FB msg;
			msg.byResut = EMPOST_WRONG_CHARGE;
			msg.iSender = PostInfo.iSender;
			msg.nReqID  = PostInfo.nReqID;
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		if ( PostInfo.llCharge > 0 && ( PostInfo.llCharge > GLCONST_POST::llPOST_MAX_ATTACH_MONEY ) )
		{
			GLMSG::SNET_POST_SEND_FB msg;
			msg.byResut = EMPOST_EXCEED_MONEY;
			msg.iSender = PostInfo.iSender;
			msg.nReqID  = PostInfo.nReqID;
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

        if ( !PostInfo.bAttach01 )
		{
			GLMSG::SNET_POST_SEND_FB msg;
			msg.byResut = EMPOST_WRONG_CHARGE;
			msg.iSender = PostInfo.iSender;
			msg.nReqID  = PostInfo.nReqID;
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		if ( PostInfo.llRestMoney < dwPostFee )
		{
			GLMSG::SNET_POST_SEND_FB msg;
			msg.byResut = EMPOST_NOTENOUGH_MONEY;
			msg.iSender = PostInfo.iSender;
			msg.nReqID  = PostInfo.nReqID;
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}
	}
	else if ( PostInfo.llCharge < 0 )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_MINUS_MONEY;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}

	if ( PostInfo.llAttachMoney >= 0 )
	{
		//
		// 13. Attached money exceeds the limitation.
		//
		if ( PostInfo.llAttachMoney > 0 && ( PostInfo.llAttachMoney > GLCONST_POST::llPOST_MAX_ATTACH_MONEY ) )
		{
			GLMSG::SNET_POST_SEND_FB msg;
			msg.byResut = EMPOST_EXCEED_MONEY;
			msg.iSender = PostInfo.iSender;
			msg.nReqID  = PostInfo.nReqID;
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}

		//
		// 14. Does the character have enough money for attachedMoney ?
		//
		PostInfo.llRestMoney -= PostInfo.llAttachMoney;
		if ( PostInfo.llRestMoney < ( LONGLONG )dwPostFee )	// There is no money for PostFee.
		{
			GLMSG::SNET_POST_SEND_FB msg;
			msg.byResut = EMPOST_NOTENOUGH_MONEY;
			msg.iSender = PostInfo.iSender;
			msg.nReqID  = PostInfo.nReqID;
			SENDTOCLIENT( dwClientID, &msg );
			return FALSE;
		}
	}
	else if ( PostInfo.llAttachMoney < 0 )
	{
		GLMSG::SNET_POST_SEND_FB msg;
		msg.byResut = EMPOST_MINUS_MONEY;
		msg.iSender = PostInfo.iSender;
		msg.nReqID  = PostInfo.nReqID;
		SENDTOCLIENT( dwClientID, &msg );
		return FALSE;
	}

	PostInfo.dwPostFee = dwPostFee;

	// Update RestMoney for PostFee.
	PostInfo.llRestMoney -= ( LONGLONG )dwPostFee;

	// 첨부 물품이 있는경우에만 해당 물품 로그를 남긴다
	if ( PostInfo.bAttach01 )
	{
		pChar->LogItemExchange( sInvenItem, gamelog::ID_CHAR, PostInfo.iSender, gamelog::ID_CHAR, PostInfo.iRecver, ITEM_ROUTE_POST, sInvenItem.wTurnNum, MapID( SNATIVEID( false ), SNATIVEID( false ) ), LOG_ITEM_POST_ATTACHEDITEM, true );
	}

	//
	// Now send the Post to the recver. (put the postinfo into DB)
	//
	*pdwLastPostSend = dwCurrentPostSend;	// for cooltime calculation
	
	AddGameAdoJob(
		db::DbActionPtr(
		new CPostSend( dwClientID, PostInfo, sInvenItem ) ) );        

	//
	//mjeon.attendance
	//
	pChar->AttendanceCheckPostSend();

	return TRUE;
}

// #item
// 우편 첨부 아이템 받기
BOOL GLGaeaServer::MsgPostOpenAndTakeAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC *nmg )
{
	GASSERT( IsField() );

	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		sc::writeLogError( std::string( "MsgPostOpenAndTakeAF() - Wrong use of postsystem while it is OFF!" ) );
		return FALSE;
	}

	GLMSG::SNET_POST_OPEN_TAKE_AF* msg = ( GLMSG::SNET_POST_OPEN_TAKE_AF* ) nmg;

	BOOL bAttach01Accept	= msg->byAttach01Accept > 0;// Byte -> BOOL
	BOOL bMoneyAccept		= msg->byMoneyAccept > 0;	// Byte -> BOOL

    GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFA;	// final FB msg to Agent
    msgFA.result = EMPOST_OK;				// initialized to OK. it will be updated when a error occured.

	if ( dwClientID != m_dwAgentSlot )
	{
		sc::writeLogError( sc::string::format( "MsgPostOpenAndTakeAF() - (CHECK) Client is sending PostAF msg directly to the FieldServer !!", msg->aPOST.m_iRecver ) );
		return FALSE;
	}

	GLChar* pCha = GetCharByDbNum( msg->aPOST.m_iRecver );
	if ( !pCha )
	{
		msgFA.iRecver			= msg->aPOST.m_iRecver;

		msgFA.byAttach01Accept	= ( BYTE ) FALSE;
		msgFA.byMoneyAccept		= ( BYTE ) FALSE;
		msgFA.result			= EMPOST_RECVER_NOTEXIST;

		SENDTOAGENT( &msgFA );

		return FALSE;
	}

	if ( !pCha->IsPOSTBOXUSEABLE( pCha->m_emPOSTBOXIDTYPE, pCha->m_sPOSTBOXID ) )
	{
		msgFA.iRecver			= msg->aPOST.m_iRecver;

		msgFA.byAttach01Accept	= ( BYTE ) FALSE;
		msgFA.byMoneyAccept		= ( BYTE ) FALSE;
		msgFA.result			= EMPOST_NOT_AVAILABLE;

		SENDTOAGENT( &msgFA );

		return FALSE;
	}

	const SITEM* pItem = NULL;

	if ( bAttach01Accept )
	{
        // Check the Attach01 and Charge one more time.
        pItem = GLogicData::GetInstance().GetItem( msg->aPOST.m_sAttach01.GetNativeID() );
        if ( !pItem )
        {
            GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFA_FB;

            msgFA_FB.llPostID = msg->aPOST.m_llPostID;
            msgFA_FB.iRecver = msg->aPOST.m_iRecver;
            msgFA_FB.byAttach01Accept = ( BYTE ) FALSE;
            msgFA_FB.byMoneyAccept = ( BYTE ) FALSE;
            msgFA_FB.result = EMPOST_WRONG_ATTACH01;

            SENDTOAGENT( &msgFA_FB );

            return FALSE;
        }

		if ( !pCha->IsInsertToInven( msg->aPOST.m_sAttach01 ) )
		{
			GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFA_FB;

			msgFA_FB.llPostID = msg->aPOST.m_llPostID;
			msgFA_FB.iRecver = msg->aPOST.m_iRecver;
			msgFA_FB.byAttach01Accept = ( BYTE ) FALSE;
			msgFA_FB.byMoneyAccept = ( BYTE ) FALSE;
			msgFA_FB.result = EMPOST_NOTENOUGH_INVEN;

			SENDTOAGENT( &msgFA_FB );

			return FALSE;
		}
/*
        WORD wPosX, wPosY;
        BOOL bOk = pCha->InvenFindInsertable( wPosX, wPosY );

        // There is not enough inventory for the attach01.
        if ( !bOk || pCha->m_cInventory.GetAmountEmptyPile( msg->aPOST.m_sAttach01.GetNativeID(), FALSE ) < msg->aPOST.m_sAttach01.wTurnNum )
        {
            GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFA_FB;

            msgFA_FB.llPostID = msg->aPOST.m_llPostID;
            msgFA_FB.iRecver = msg->aPOST.m_iRecver;
            msgFA_FB.byAttach01Accept = ( BYTE ) FALSE;
            msgFA_FB.byMoneyAccept = ( BYTE ) FALSE;
            msgFA_FB.result = EMPOST_NOTENOUGH_INVEN;

            SENDTOAGENT( &msgFA_FB );

            return FALSE;
        }
*/

        if ( pCha->GetInvenMoney() + msg->aPOST.m_llAttachMoney < msg->aPOST.m_llCharge)	// Consider the attached money !!
        {
            GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFA_FB;

            msgFA_FB.llPostID = msg->aPOST.m_llPostID;
            msgFA_FB.iRecver = msg->aPOST.m_iRecver;
            msgFA_FB.byAttach01Accept= ( BYTE ) FALSE;
            msgFA_FB.byMoneyAccept = ( BYTE ) FALSE;
            msgFA_FB.result = EMPOST_NOTENOUGH_MONEY;

            SENDTOAGENT( &msgFA_FB );

            return FALSE;
        }
	}

    DWORD dwCharClientID = pCha->GETCLIENTID();

	//mjeon.인도네시아 아이템 복사 문제를 막기 위해 2차 필터를 둔다.
	//만일 임의의 상태 이상으로 인해 아래의 DbAction이 실패한다면, 해당 캐릭터는 이 우편의 첨부를 획득하기 위해
	//재접속을 해야할 것이다.
	if ( bAttach01Accept || bMoneyAccept )
	{
		if ( pCha->PostGetFlagTakenAttach( msg->aPOST.m_llPostID ) )
		{
			//이미 획득한 첨부에 대해 다시 획득시도를 하고 있다.

			sc::writeLogError( sc::string::format( "GLGaeaServer::MsgPostOpenAndTakeAF() - (CHECK) Char(%1%) is re-trying to take post-attachment!!", msg->aPOST.m_iRecver ) );			
			
			GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFB;

			msgFB.iRecver			= msg->aPOST.m_iRecver;

			msgFB.byAttach01Accept	= FALSE;
			msgFB.byMoneyAccept		= FALSE;
			msgFB.result			= EMPOST_REACCEPT_ITEM;	//or EMPOST_REACCEPT_MONEY

			SENDTOAGENT( &msgFB );

			return FALSE;
		}
		else
		{	
			pCha->PostSetFlagTakenAttach( msg->aPOST.m_llPostID );
		}
	}

	//
	//put the DbAction into the queue.
	//
	if ( m_pDBMan )
	{		
        AddGameAdoJob( 
            db::DbActionPtr( 
                new CPostOpenAndTake( msg, bMoneyAccept, bAttach01Accept, pCha->GetUserID() ) ) );
	}
	else
	{
		sc::writeLogInfo( "PostOpenAndTake: Could not get DBMan." );

		GLMSG::SNET_POST_OPEN_TAKE_AF_FB msgFB;

		msgFB.llPostID			= msg->aPOST.m_llPostID;
		msgFB.iRecver			= msg->aPOST.m_iRecver;
		msgFB.byAttach01Accept	= ( BYTE ) FALSE;
		msgFB.byMoneyAccept		= ( BYTE ) FALSE;
		msgFB.result			= EMPOST_ETC;

		SENDTOAGENT( &msgFB );

		return FALSE;
	}

	BOOL bResultInsertion = FALSE;
	GLMSG::SNETPC_INVEN_DEL_AND_INSERT msgItem;
	if ( bAttach01Accept )
	{
		if ( pItem->ISPILE() )
		{
			std::vector< UPDATED_ITEM > vecInsertedItems;
			pCha->InsertPileItem( msg->aPOST.m_sAttach01, msg->aPOST.m_sAttach01.wTurnNum, vecInsertedItems, true );

			size_t InsertedItemSize = vecInsertedItems.size();
			for ( size_t loop = 0; loop < InsertedItemSize; loop++ )
			{
				const SINVENITEM* pInvenItem =  pCha->InvenGetItem( vecInsertedItems[loop].sPosition.wMainID, vecInsertedItems[loop].sPosition.wSubID );
				if ( !pInvenItem )
				{
					continue;
				}

				pCha->LogItemExchange( 
					pInvenItem->sItemCustom,
					gamelog::ID_CHAR,
					msg->aPOST.m_iSender, 
					gamelog::ID_CHAR, 
					pCha->m_CharDbNum, 
					ITEM_ROUTE_POST,
					pInvenItem->sItemCustom.TurnNum(),
					( pCha == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCha->GetCurrentMap(), LOG_ITEM_POST_ATTACHEDITEM, 
					true );
			}
		}
		else
		{
			pCha->InvenInsertItem( msg->aPOST.m_sAttach01, true, 98759, true, false, true );
			
			pCha->LogItemExchange( 
				msg->aPOST.m_sAttach01,
				gamelog::ID_CHAR,
				msg->aPOST.m_iSender, 
				gamelog::ID_CHAR, 
				pCha->m_CharDbNum, 
				ITEM_ROUTE_POST,
				msg->aPOST.m_sAttach01.TurnNum(),
				( pCha == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCha->GetCurrentMap(), LOG_ITEM_POST_ATTACHEDITEM, 
				true );
		}
	}

    GLMSG::NET_CHAR_UPDATE_MONEY_FC msgMoneyUpdate(0);
    if (bMoneyAccept)
    {
        LONGLONG llTotalMoney = pCha->GetInvenMoney() + msg->aPOST.m_llAttachMoney;
        pCha->CheckMoneyUpdate( pCha->GetInvenMoney(), msg->aPOST.m_llAttachMoney, TRUE, "Post: Take Attached Money" );
        //pCha->SetInvenMoney(llTotalMoney);
		pCha->AddInvenMoneyServer( msg->aPOST.m_llAttachMoney, ADD_INVEN_MONEY_POST_ATTACHED_MONEY );

		pCha->LogMoneyExchange( gamelog::ID_CHAR, msg->aPOST.m_iSender, gamelog::ID_CHAR, pCha->m_CharDbNum, msg->aPOST.m_llAttachMoney, ITEM_ROUTE_POST, (pCha == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCha->GetCurrentMap(), LOG_MONEY_POST_ATTACHEDMONEY );
		pCha->LogMoneyExchange( gamelog::ID_CHAR, msg->aPOST.m_iSender, gamelog::ID_CHAR, pCha->m_CharDbNum, pCha->GetInvenMoney(), ITEM_ROUTE_POST, (pCha == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCha->GetCurrentMap(), LOG_MONEY_POST_ATTACHEDMONEY );

        msgMoneyUpdate.SetMoney(llTotalMoney);

        SENDTOCLIENT(dwCharClientID, &msgMoneyUpdate);

        //
        // DB handling will be done together with attach01 after attach01 process.
        // Therefore, all the next steps MUST guarantee reach to the DB process whether they succeed or not.
        //
    }

    if (bAttach01Accept)
    {
        //
        // User's money already has been checked before coming into this function.
        // That means we can guarantee that the user has enough money for the charge at this point.
        //
        if (msg->aPOST.m_llCharge > 0)
        {
            //
            // Returned mail's Charge is ignored.
            //
            if ( msg->aPOST.m_byPostState != EMPOST_RETURN )
            {
                LONGLONG llTotalMoney = pCha->GetInvenMoney() - msg->aPOST.m_llCharge;
                pCha->CheckMoneyUpdate( pCha->GetInvenMoney(), msg->aPOST.m_llCharge, FALSE, "Post: Charge for attached item" );
                //pCha->SetInvenMoney(llTotalMoney);
				pCha->SubtractInvenMoneyServer( msg->aPOST.m_llCharge, SUBTRACT_INVEN_MONEY_POST_CHARGE );

				pCha->LogMoneyExchange( gamelog::ID_CHAR, msg->aPOST.m_iSender, gamelog::ID_CHAR, pCha->m_CharDbNum, -msg->aPOST.m_llAttachMoney, ITEM_ROUTE_POST, (pCha == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCha->GetCurrentMap(), LOG_MONEY_POST_CHARGEDMONEY );
				pCha->LogMoneyExchange( gamelog::ID_CHAR, msg->aPOST.m_iSender, gamelog::ID_CHAR, pCha->m_CharDbNum, pCha->GetInvenMoney(), ITEM_ROUTE_POST, (pCha == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCha->GetCurrentMap(), LOG_MONEY_POST_CHARGEDMONEY );


                msgMoneyUpdate.lnMoney = llTotalMoney;
                SENDTOCLIENT(dwCharClientID,&msgMoneyUpdate);


                GLMSG::SNET_POST_NOTIFY msgNotifyFA;
                msgNotifyFA.iRecver = msg->aPOST.m_iSender;
                msgNotifyFA.llPostID = msg->aPOST.m_llPostID;

                SENDTOAGENT(&msgNotifyFA);
            }
        }
    }

    // At this point, all has done OK.

    msgFA.iRecver = msg->aPOST.m_iRecver;
    msgFA.llPostID = msg->aPOST.m_llPostID;
    msgFA.byAttach01Accept = (BYTE)bAttach01Accept;
    msgFA.byMoneyAccept = (BYTE)bMoneyAccept;

    SENDTOAGENT( &msgFA );

	return TRUE;
}


BOOL GLGaeaServer::MsgPostToDoAfterSendFF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("MsgPostToDoAfterSendFF() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}

	GLMSG::SNET_POST_TODO_AFTER_SEND_FF *msgToDo = (GLMSG::SNET_POST_TODO_AFTER_SEND_FF *) nmg;
	GLMSG::SNET_POST_SEND_FB msgSendFb;

	if (msgToDo->nReturn == sc::db::DB_OK)
	{
		//ToDo on the Server after completion of PostSend.
		//
		// 1. Send msg to notify changing of money in sender's inventory.
		// 2. Send msg for removing of the attached item in the sender's inventory.	
		// 3. Send SendPostFB to notify Succeed of PostSend to the Sender.
		// 4. Send Notify to the Recver via AgentServer.

		PGLCHAR pCha = (PGLCHAR) GetCharByDbNum(msgToDo->postinfo.iSender);

		if (pCha == NULL)
		{
			sc::writeLogError(sc::string::format("MsgPostToDoAfterSendFF() - Sender(%1%) does not exist! Rollback the post.", msgToDo->postinfo.iSender));
			goto _postsend_rollback;
		}

		LONGLONG llMoney = msgToDo->postinfo.llAttachMoney + static_cast<LONGLONG>(msgToDo->postinfo.dwPostFee);
		if ( 0 < llMoney )
		{
			//
			// 1. Change the amount of money in sender's inventory.		
			//

            LONGLONG llCurRestMoney = pCha->GetInvenMoney() - llMoney;
            if ( llCurRestMoney != msgToDo->postinfo.llRestMoney )
            {
                // error or try hacking.
                sc::writeLogError( sc::string::format( "Different Money between MsgPostSend and MsgPostToDoAfterSendFF : ChaNum-%1%, IP-%2%",
                    pCha->CharDbNum(), GetClientIP(pCha->ClientSlot()) ) );

                goto _postsend_rollback;
            }
            else if ( llCurRestMoney < 0 )
            {
                // not enough money.
                sc::writeLogError( sc::string::format( "Not enough money at MsgPostToDoAfterSendFF : ChaNum-%1%, IP-%2%",
                    pCha->CharDbNum(), GetClientIP(pCha->ClientSlot()) ) );

                goto _postsend_rollback;
            }

			pCha->CheckMoneyUpdate( pCha->GetInvenMoney(), llMoney, FALSE, "Post: Send Attached Money and Pay PostFee" );
			//pCha->SetInvenMoney(llCurRestMoney);
			pCha->SubtractInvenMoneyServer( llMoney, SUBTRACT_INVEN_MONEY_POST_FEE_AND_ATTACHED_MONEY );
			
			GLMSG::NET_CHAR_UPDATE_MONEY_FC msgMoneyUpdate(llCurRestMoney);
			SENDTOCLIENT(dwClientID, &msgMoneyUpdate);
		}

		if (msgToDo->postinfo.bAttach01)
		{
			SCHARDATA2& chadata2   = (SCHARDATA2 &) pCha->GetCharData2();

			SINVENITEM* pInvenItem = chadata2.m_cInventory.GetItem( msgToDo->postinfo.xAttach01, msgToDo->postinfo.yAttach01 );
			if( !pInvenItem )
				goto _postsend_rollback_item;

			if( pInvenItem->sItemCustom.GetNativeID() != msgToDo->postinfo.idAttach01 )
				goto _postsend_rollback_item;

			if( pInvenItem->sItemCustom.TurnNum() < msgToDo->postinfo.nAttach01 )
				goto _postsend_rollback_item;

			const SITEM* pItemInfo = GLogicData::GetInstance().GetItem( msgToDo->postinfo.idAttach01 );
			if( !pItemInfo )
				goto _postsend_rollback_item;

			if( pInvenItem->sItemCustom.TurnNum() > msgToDo->postinfo.nAttach01 )
			{
				pInvenItem->sItemCustom.wTurnNum -= msgToDo->postinfo.nAttach01;

				GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
				NetMsg_Inven_Update.dwChaNum = pCha->CharDbNum();
				NetMsg_Inven_Update.wPosX = msgToDo->postinfo.xAttach01;
				NetMsg_Inven_Update.wPosY = msgToDo->postinfo.yAttach01;
				NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
				SENDTOCLIENT( dwClientID, &NetMsg_Inven_Update );
			}
			else
			{
				pCha->InvenDeleteItem(
					msgToDo->postinfo.xAttach01,
					msgToDo->postinfo.yAttach01,
					true,
					FALSE, 
					1997, 
					true );
			}
		}

		//
		// 3. Send PostSend result to the Sender.
		//
		msgSendFb.byResut = EMPOST_OK;
		msgSendFb.iSender = msgToDo->postinfo.iSender;
		msgSendFb.nReqID  = msgToDo->postinfo.nReqID;
		SENDTOCLIENT(dwClientID, &msgSendFb);
		

		//
		// 4. Send Notify to the Recver via AgentServer.
		//
		GLMSG::SNET_POST_NOTIFY msgNotify;
		msgNotify.llPostID = msgToDo->postinfo.llPostID;	// completed postsend
		msgNotify.iRecver  = msgToDo->postinfo.iRecver;
		SENDTOAGENT(&msgNotify);

		return TRUE;		
	}
	else
	{
		msgSendFb.byResut = sc::db::DB_ERROR;		// failed postsend
		msgSendFb.iSender = msgToDo->postinfo.iSender;
		msgSendFb.nReqID  = msgToDo->postinfo.nReqID;
		SENDTOCLIENT(dwClientID, &msgSendFb);

		return FALSE;
	}

_postsend_rollback_item:

	msgSendFb.byResut = EMPOST_WRONG_ATTACH01;
	msgSendFb.iSender = msgToDo->postinfo.iSender;
	msgSendFb.nReqID  = msgToDo->postinfo.nReqID;
	SENDTOCLIENT( dwClientID, &msgSendFb );

_postsend_rollback:

	if (m_pAdoMan)
	{		
        AddGameAdoJob(
            db::DbActionPtr(
                new CPostSendRollBack(msgToDo->postinfo.llPostID)));
	}
	else
	{	
		sc::writeLogError(std::string("PoseSendRollBack: Could not get DBMan."));

		msgSendFb.byResut = sc::db::DB_ERROR;		// failed postsend
		msgSendFb.iSender = msgToDo->postinfo.iSender;
		msgSendFb.nReqID  = msgToDo->postinfo.nReqID;
		SENDTOCLIENT(dwClientID, &msgSendFb);

		return FALSE;
	}

	return FALSE;
}


BOOL GLGaeaServer::MsgReqPostBoxOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqPostBoxOpen( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqPostBoxOpenCard(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqPostBoxOpenCard( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqPostBoxClose(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqPostBoxClose( nmg );
    return TRUE;    
}


//
//From System To specific Client
//
BOOL GLGaeaServer::PostSendSystemMail( UINT nRecver, std::string &strRecverName, DWORD dwAttachedItemID, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit/* = true*/)
{
	GASSERT(IsField());

	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("PostSendSystemMail() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}

	PGLCHAR pChar = GetCharByDbNum(nRecver);

	
	int nUserIDSender = -1;
	int x=0, y=0;
	
	SPOSTINFO PostInfo;
	ZeroMemory(&PostInfo, sizeof(SPOSTINFO));
	
	PostInfo.byPostState	= EMPOST_UNREAD;

	if (dwAttachedItemID != SNATIVEID(false).dwID)
	{
		PostInfo.bAttach01 = TRUE;

		if (llAttachedMoney > 0)
		{
			PostInfo.byPostType		= EMPOST_MONEYITEM;
		}
		else
		{
			PostInfo.byPostType		= EMPOST_ITEM;
		}
	}
	else
	{
		if (llAttachedMoney > 0)
		{
			PostInfo.byPostType		= EMPOST_MONEY;
		}
		else
		{
			PostInfo.byPostType		= EMPOST_TEXT;
		}
	}

	PostInfo.byPostClass	= EMPOST_SYSTEM;
	PostInfo.iSender		= nSenderSystem;
	PostInfo.iRecver		= nRecver;

	
	StringCchCopy (PostInfo.RecverName, sizeof(PostInfo.RecverName), strRecverName.c_str());
	StringCchCopy (PostInfo.SenderName, sizeof(PostInfo.SenderName), strSenderName.c_str());

	
	//
	// SendDate MUST be set at SERVER-side.
	//		
	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//CTime cTime(st);
	//CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	//StringCchCopy(PostInfo.SendDate, sizeof(PostInfo.SendDate), strTime.GetString());

	//PostInfo.nReqID = msg->nReqID;

	PostInfo.iDesign		= iDesign;
	PostInfo.llCharge		= 0;
	StringCchCopy (PostInfo.Title, sizeof(PostInfo.Title), strTitle.c_str());
	StringCchCopy (PostInfo.Content, sizeof(PostInfo.Content), strContent.c_str());
	PostInfo.llAttachMoney	= llAttachedMoney;

	// Attach01	
	PostInfo.idAttach01 = dwAttachedItemID;

	
	
	//
	// 1.Does the post have valid title and content? not length 0.
	//
    if ( PostInfo.Title[0] == NULL || PostInfo.Content[0] == NULL )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): Title or Content is empty"));

		return FALSE;	
	}	
	
	if( PostInfo.bAttach01 )
	{
		//
		// 2.Check whether the attachment is valid or not
		//
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( PostInfo.idAttach01 );
		
		if ( !pITEM )
		{
			sc::writeLogError(sc::string::format("PostSendSystemMail(): AttachedItemID(%1%, %2%) is wrong.", PostInfo.idAttach01.wMainID, PostInfo.idAttach01.wSubID));
					
			PostInfo.bAttach01 = FALSE;
		}
		else
		{
			PostInfo.bAttach01 = TRUE;
		}

		////
		//// 3.Time-Limit item cannot be sent
		////
		// 출석부 보상으로 기간제 아이템이 포함되어 있어서 삭제
		//const SITEMCUSTOM& sItemCustom( PostInfo.idAttach01 );

		//if ( sItemCustom.IsTimeLimit() )
		//{
		//	sc::writeLogError(std::string("PostSendSystemMail(): Time-Limit item cannot be sent via System Mail."));

		//	return FALSE;
		//}
	}


	//
	// 4.Is Charge valid ?
	//
	if (PostInfo.llCharge > 0)
	{
		sc::writeLogError(std::string("PostSendSystemMail(): llCharge > 0. System mail cannot charge."));

		return FALSE;
	}
	else if ( PostInfo.llCharge < 0 )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): llCharge < 0."));

		return FALSE;
	}


	if (PostInfo.llAttachMoney >= 0)
	{
		//
		// 5. Attached money exceeds the limitation.
		//
		if ( true == bAttachMoneyLimit && PostInfo.llAttachMoney > 0 && (PostInfo.llAttachMoney > GLCONST_POST::llPOST_MAX_ATTACH_MONEY) )
		{
			sc::writeLogError(std::string("PostSendSystemMail(): AttachedMoney exceeds the limitation."));			

			return FALSE;
		}
	}
	else if ( PostInfo.llAttachMoney < 0 )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): AttachedMoney is less than 0."));

		return FALSE;
	}
	
	//
	// Now send the Post to the recver. (put the postinfo into DB)
	//	

	//
	// Generate the Item
	//
	SITEMCUSTOM itemcustomNew;
	NewItemMake(itemcustomNew, PostInfo.idAttach01, EMGEN_SYSTEM);
	itemcustomNew.GenerateBasicStat( false );
	itemcustomNew.GenerateLinkSkill();
	itemcustomNew.GenerateAddOption();


	if (m_pDBMan)
	{
		AddGameAdoJob(
            db::DbActionPtr(
                new CPostSendSystemMail(PostInfo, itemcustomNew)));
	}
	else
	{
		sc::writeLogError(std::string("PostSend: Could not get DBMan."));
		return FALSE;
	}

	return TRUE;
}


BOOL GLGaeaServer::PostSendSystemMail( DWORD dwRecverDbNum, std::string& strRecverName, CItemDrop* pItemDrop, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit /*= true*/)
{
	GASSERT(IsField());

	if ( NULL == pItemDrop )
	{
		return FALSE;
	}

	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("PostSendSystemMail() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}

	PGLCHAR pChar = GetCharByDbNum(dwRecverDbNum);


	int nUserIDSender = -1;
	int x=0, y=0;

	SPOSTINFO PostInfo;
	ZeroMemory(&PostInfo, sizeof(SPOSTINFO));

	PostInfo.byPostState	= EMPOST_UNREAD;

	const DWORD dwAttachedItemID = pItemDrop->sItemCustom.GetNativeID().dwID;
	if (dwAttachedItemID != SNATIVEID(false).dwID)
	{
		PostInfo.bAttach01 = TRUE;

		if (llAttachedMoney > 0)
		{
			PostInfo.byPostType		= EMPOST_MONEYITEM;
		}
		else
		{
			PostInfo.byPostType		= EMPOST_ITEM;
		}
	}
	else
	{
		if (llAttachedMoney > 0)
		{
			PostInfo.byPostType		= EMPOST_MONEY;
		}
		else
		{
			PostInfo.byPostType		= EMPOST_TEXT;
		}
	}

	PostInfo.byPostClass	= EMPOST_SYSTEM;
	PostInfo.iSender		= nSenderSystem;
	PostInfo.iRecver		= dwRecverDbNum;


	StringCchCopy (PostInfo.RecverName, sizeof(PostInfo.RecverName), strRecverName.c_str());
	StringCchCopy (PostInfo.SenderName, sizeof(PostInfo.SenderName), strSenderName.c_str());


	//
	// SendDate MUST be set at SERVER-side.
	//		
	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//CTime cTime(st);
	//CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	//StringCchCopy(PostInfo.SendDate, sizeof(PostInfo.SendDate), strTime.GetString());

	//PostInfo.nReqID = msg->nReqID;

	PostInfo.iDesign		= iDesign;
	PostInfo.llCharge		= 0;
	StringCchCopy (PostInfo.Title, sizeof(PostInfo.Title), strTitle.c_str());
	StringCchCopy (PostInfo.Content, sizeof(PostInfo.Content), strContent.c_str());
	PostInfo.llAttachMoney	= llAttachedMoney;

	// Attach01	
	PostInfo.idAttach01 = dwAttachedItemID;

	//
	// 1.Does the post have valid title and content? not length 0.
	//
	if ( PostInfo.Title[0] == NULL || PostInfo.Content[0] == NULL )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): Title or Content is empty"));
		return FALSE;	
	}	

	if( PostInfo.bAttach01 )
	{
		//
		// 2.Check whether the attachment is valid or not
		//
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( PostInfo.idAttach01 );

		if ( !pITEM )
		{
			sc::writeLogError(sc::string::format("PostSendSystemMail(): AttachedItemID(%1%, %2%) is wrong.", PostInfo.idAttach01.wMainID, PostInfo.idAttach01.wSubID));

			PostInfo.bAttach01 = FALSE;
		}
		else
			PostInfo.bAttach01 = TRUE;

		if ( pItemDrop->sItemCustom.IS_GUID_NULL() )
		{
			sc::writeLogError(std::string("PostSendSystemMail(): pItemDrop->sItemCustom.IS_GUID_NULL()."));
			return FALSE;
		}

		//
		// 3.Time-Limit item cannot be sent
		//
		// 우편 보상으로 시간제 아이템이 포함되어 있어서 다시 삭제
		//const SITEMCUSTOM& sItemCustom( PostInfo.idAttach01 );

		//if ( sItemCustom.IsTimeLimit() )
		//{
		//	sc::writeLogError(std::string("PostSendSystemMail(): Time-Limit item cannot be sent via System Mail."));

		//	return FALSE;
		//}
	}


	//
	// 4.Is Charge valid ?
	//
	if (PostInfo.llCharge > 0)
	{
		sc::writeLogError(std::string("PostSendSystemMail(): llCharge > 0. System mail cannot charge."));

		return FALSE;
	}
	else if ( PostInfo.llCharge < 0 )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): llCharge < 0."));

		return FALSE;
	}


	if (PostInfo.llAttachMoney >= 0)
	{
		//
		// 5. Attached money exceeds the limitation.
		//
		if ( true == bAttachMoneyLimit && PostInfo.llAttachMoney > 0 && (PostInfo.llAttachMoney > GLCONST_POST::llPOST_MAX_ATTACH_MONEY) )
		{
			sc::writeLogError(std::string("PostSendSystemMail(): AttachedMoney exceeds the limitation."));			

			return FALSE;
		}
	}
	else if ( PostInfo.llAttachMoney < 0 )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): AttachedMoney is less than 0."));

		return FALSE;
	}

	//
	// Now send the Post to the recver. (put the postinfo into DB)
	//	

	//
	// Generate the Item
	//	

	if (m_pDBMan)
	{
		AddGameAdoJob(
			db::DbActionPtr(
			new CPostSendSystemMail(PostInfo, pItemDrop->sItemCustom)));
	}
	else
	{
		sc::writeLogError(std::string("PostSend: Could not get DBMan."));
		return FALSE;
	}

	return TRUE;
}

BOOL GLGaeaServer::PostSendSystemMail( DWORD dwRecverDbNum, std::string& strRecverName, SITEMCUSTOM& sItem, LONGLONG llAttachedMoney, UINT iDesign, UINT nSenderSystem, std::string &strSenderName, std::string &strTitle, std::string &strContent, bool bAttachMoneyLimit /*= true*/ )
{
	GASSERT(IsField());

	if (!GLUseFeatures::GetInstance().IsUsingPost())
	{
		sc::writeLogError(std::string("PostSendSystemMail() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}

	PGLCHAR pChar = GetCharByDbNum(dwRecverDbNum);


	int nUserIDSender = -1;
	int x=0, y=0;

	SPOSTINFO PostInfo;
	ZeroMemory(&PostInfo, sizeof(SPOSTINFO));

	PostInfo.byPostState	= EMPOST_UNREAD;

	const DWORD dwAttachedItemID = sItem.GetNativeID().dwID;
	if (dwAttachedItemID != SNATIVEID(false).dwID)
	{
		PostInfo.bAttach01 = TRUE;

		if (llAttachedMoney > 0)
		{
			PostInfo.byPostType		= EMPOST_MONEYITEM;
		}
		else
		{
			PostInfo.byPostType		= EMPOST_ITEM;
		}
	}
	else
	{
		if (llAttachedMoney > 0)
		{
			PostInfo.byPostType		= EMPOST_MONEY;
		}
		else
		{
			PostInfo.byPostType		= EMPOST_TEXT;
		}
	}

	PostInfo.byPostClass	= EMPOST_SYSTEM;
	PostInfo.iSender		= nSenderSystem;
	PostInfo.iRecver		= dwRecverDbNum;


	StringCchCopy (PostInfo.RecverName, sizeof(PostInfo.RecverName), strRecverName.c_str());
	StringCchCopy (PostInfo.SenderName, sizeof(PostInfo.SenderName), strSenderName.c_str());


	//
	// SendDate MUST be set at SERVER-side.
	//		
	//SYSTEMTIME st;
	//GetLocalTime(&st);
	//CTime cTime(st);
	//CString strTime = cTime.Format("%Y-%m-%d %H:%M:%S");
	//StringCchCopy(PostInfo.SendDate, sizeof(PostInfo.SendDate), strTime.GetString());

	//PostInfo.nReqID = msg->nReqID;

	PostInfo.iDesign		= iDesign;
	PostInfo.llCharge		= 0;
	StringCchCopy (PostInfo.Title, sizeof(PostInfo.Title), strTitle.c_str());
	StringCchCopy (PostInfo.Content, sizeof(PostInfo.Content), strContent.c_str());
	PostInfo.llAttachMoney	= llAttachedMoney;

	// Attach01	
	PostInfo.idAttach01 = dwAttachedItemID;

	//
	// 1.Does the post have valid title and content? not length 0.
	//
	if ( PostInfo.Title[0] == NULL || PostInfo.Content[0] == NULL )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): Title or Content is empty"));
		return FALSE;	
	}	

	if( PostInfo.bAttach01 )
	{
		//
		// 2.Check whether the attachment is valid or not
		//
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( PostInfo.idAttach01 );

		if ( !pITEM )
		{
			sc::writeLogError(sc::string::format("PostSendSystemMail(): AttachedItemID(%1%, %2%) is wrong.", PostInfo.idAttach01.wMainID, PostInfo.idAttach01.wSubID));

			PostInfo.bAttach01 = FALSE;
		}
		else
			PostInfo.bAttach01 = TRUE;

		if ( sItem.IS_GUID_NULL() )
		{
			sc::writeLogError(std::string("PostSendSystemMail(): sItem.IS_GUID_NULL()."));
			return FALSE;
		}

		////
		//// 3.Time-Limit item cannot be sent
		////
		// 출석부 보상으로 기간제 아이템이 포함되어 있어서 삭제
		//const SITEMCUSTOM& sItemCustom( PostInfo.idAttach01 );

		//if ( sItemCustom.IsTimeLimit() )
		//{
		//	sc::writeLogError(std::string("PostSendSystemMail(): Time-Limit item cannot be sent via System Mail."));

		//	return FALSE;
		//}
	}


	//
	// 4.Is Charge valid ?
	//
	if (PostInfo.llCharge > 0)
	{
		sc::writeLogError(std::string("PostSendSystemMail(): llCharge > 0. System mail cannot charge."));

		return FALSE;
	}
	else if ( PostInfo.llCharge < 0 )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): llCharge < 0."));

		return FALSE;
	}


	if (PostInfo.llAttachMoney >= 0)
	{
		//
		// 5. Attached money exceeds the limitation.
		//
		if ( true == bAttachMoneyLimit && PostInfo.llAttachMoney > 0 && (PostInfo.llAttachMoney > GLCONST_POST::llPOST_MAX_ATTACH_MONEY) )
		{
			sc::writeLogError(std::string("PostSendSystemMail(): AttachedMoney exceeds the limitation."));			

			return FALSE;
		}
	}
	else if ( PostInfo.llAttachMoney < 0 )
	{
		sc::writeLogError(std::string("PostSendSystemMail(): AttachedMoney is less than 0."));

		return FALSE;
	}

	//
	// Now send the Post to the recver. (put the postinfo into DB)
	//	

	//
	// Generate the Item
	//	

	if (m_pDBMan)
	{
		AddGameAdoJob(
			db::DbActionPtr(
			new CPostSendSystemMail(PostInfo, sItem)));
	}
	else
	{
		sc::writeLogError(std::string("PostSend: Could not get DBMan."));
		return FALSE;
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewListReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );

	if (pChar)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

		GLMSG::NET_POST_RENEW_LIST_REQ RecvData;
		if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
			return FALSE;

		RecvData.dwRecvCha = pChar->CharDbNum();

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);

		SENDTOCACHE( NET_MSG_POST_RENEW_LIST_REQ, SendBuffer );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewListAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_LIST_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;

	GLChar* pChar = GetCharByDbNum( RecvData.dwRecvCha );


	if (pChar)
		SENDTOCLIENT( pChar->GETCLIENTID(), nmg );

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewSendReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );

	if( GLUseFeatures::GetInstance().IsUsingPostSendBlock() )
	{
		if( pChar )
			sc::writeLogError( sc::string::format("Post Hacking Message! [IP:%1%][ChaNum:%2%]", GetClientIP( dwClientID ),  pChar->CharDbNum() ));
		else
			sc::writeLogError( sc::string::format("Post Hacking Message! [IP:%1%]", GetClientIP( dwClientID )));

		return FALSE;
	}

	GASSERT(IsField());

	if (!GLUseFeatures::GetInstance().IsUsingPostRenew())
	{
		sc::writeLogError(std::string("MsgPostSend() - Wrong use of postsystem while it is OFF!"));
		return FALSE;
	}

	if (pChar)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

		// 다른 대부분 정보는 DB처리가 된후에 채워넣지만
		GLMSG::NET_POST_RENEW_SEND_REQ_FOR_CLIENT RecvData;
		if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
			return FALSE;
		
		AddGameAdoJob( 
			db::DbActionPtr( 
			new db::CPostRnCharCheck( RecvData, pChar->CharDbNum() ) ) );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewSendAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_SEND_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;

	GLChar* pSenderChar = GetCharByDbNum( RecvData.postInfo.dwSendChaNum );
	if ( pSenderChar )
	{
		GLMSG::NET_POST_RENEW_SEND_ACK_FOR_CLIENT SendData;	// 우편 전송 성공여부
		SendData.emCode = RecvData.emCode;

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);

		SENDTOCLIENT( pSenderChar->GETCLIENTID(), NET_MSG_POST_RENEW_SEND_ACK, SendBuffer );
	}

	GLChar* pRecvChar = GetCharByDbNum( RecvData.postInfo.dwRecvChaNum );
	if ( pRecvChar )
	{
		GLMSG::NET_POST_RENEW_NEW_POST SendData;	// 새 우편 알림
		SendData.postInfo = RecvData.postInfo;

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);

		SENDTOCLIENT( pRecvChar->GETCLIENTID(), NET_MSG_POST_RENEW_NEW_POST, SendBuffer );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewChaCheckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// DB에서 온 메시지
	if( DBACTION_CLIENT_ID != dwClientID )
		return FALSE;

	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_CHA_CHECH_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;

	if ( RecvData.emCode != EMPOST_SEND_OK )
	{
		GLChar* pChar = GetCharByDbNum( RecvData.postInfo.dwSendChaNum );

		if (pChar)
			SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
	}
	else
	{
		if (m_spPostField)
			m_spPostField->SendPost( 
				RecvData.postInfo.dwSendChaNum, 
				RecvData.postInfo.dwRecvChaNum, 
				RecvData.postInfo.strTitle, 
				RecvData.postInfo.strContent,
				RecvData.postInfo.wStyle,
				(EMPOST_MONEY_TYPE)RecvData.postInfo.wMoneyType,
				RecvData.postInfo.llMoney,
				RecvData.postInfo.sItemCutom );
	}
	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewDelReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );

	if (pChar)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

		GLMSG::NET_POST_RENEW_DEL_REQ RecvData;
		if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
			return FALSE;

		RecvData.dwChaNum = pChar->CharDbNum();

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);


		SENDTOCACHE( NET_MSG_POST_RENEW_DEL_REQ, SendBuffer );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewDelAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_DEL_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;

	GLChar* pChar = GetCharByDbNum( RecvData.dwChaNum );
	if ( pChar )
	{
		SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewAttachReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );

	if (pChar)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

		GLMSG::NET_POST_RENEW_ATTACH_REQ RecvData;
		if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
			return FALSE;

		RecvData.dwChaNum = pChar->CharDbNum();

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);


		SENDTOCACHE( NET_MSG_POST_RENEW_ATTACH_REQ, SendBuffer );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewAttachAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// 캐쉬에서 온 패킷
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_ATTACH_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;


	// 대금 청구라면 우선 돈부터 뺀다. 없는경우 캐쉬서버 롤백
	GLChar* pChar = GetCharByDbNum( RecvData.dwRecvChaNum );
	if (pChar)
	{
		//pChar
		switch ( RecvData.emMoneyType )
		{
		case EMPOST_MONEY_CHARGE_GOLD:
			if( pChar->GetInvenMoney() < RecvData.llMoney )
			{
				// 캐쉬로 롤백 전송 롤백하자.
				GLMSG::NET_POST_RENEW_ATTACH_ROLLBACK_REQ SendData;
				SendData.dwPostID = RecvData.dwPostID;

				msgpack::sbuffer SendBuffer;
				msgpack::pack(SendBuffer, SendData);

				SENDTOCACHE( NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ, SendBuffer );

				// 잔액 부족 클라에게 통보
				//EMPOST_ATTACH_NOT_ENOUGH_MONEY;
				return FALSE;
			}

			pChar->SubtractInvenMoneyAndUpdate( RecvData.llMoney, SUBTRACT_INVEN_MONEY_POST_CHARGE );
			pChar->LogMoneyExchange( gamelog::ID_CHAR, RecvData.dwSendChaNum, gamelog::ID_CHAR, pChar->m_CharDbNum, -RecvData.llMoney, ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_INFO_POST_BY_CONSIGEMENT );
			pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, pChar->m_CharDbNum, pChar->GetInvenMoney(), ITEM_ROUTE_CONSIGMENT_SALE, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CURRENT_INVEN_BY_CONSIGEMENT );
			break;
		case EMPOST_MONEY_CHARGE_POINT:
			if( pChar->PointShopPoint() < RecvData.llMoney )
			{
				// 캐쉬로 롤백 전송 롤백하자.
				GLMSG::NET_POST_RENEW_ATTACH_ROLLBACK_REQ SendData;
				SendData.dwPostID = RecvData.dwPostID;

				msgpack::sbuffer SendBuffer;
				msgpack::pack(SendBuffer, SendData);

				SENDTOCACHE( NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ, SendBuffer );

				// 잔액 부족 클라에게 통보
				//EMPOST_ATTACH_NOT_ENOUGH_POINT;
				return FALSE;
			}

			pChar->PointShopPointSub( RecvData.llMoney, pointshop::POST_SYSTEM,  pointshop::POINT_SUB_TYPE_CHARGE_POINT );
			break;
		}
	}


	//????????????????? // 여기서 대금 청구는 확인부터 하자..
	// OK가 아닌데 왜 패킷이 날아왔지? 캐쉬에서 검사후에 보낸다.
	if (RecvData.emCode != EMPOST_ATTACH_OK)
	{
		// 캐쉬서버 롤백
		return FALSE;
	}

	// DB에서 삭제하도록 하자.
	AddGameAdoJob( db::DbActionPtr ( new db::CPostRnDelAttach( this, RecvData ) ) );

	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewAttachAckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// DB에서 온 메시지
	// 캐쉬서버에서 온 메시지와 타입이 동일하지만 처리하는 부분이 다르다.
	if( DBACTION_CLIENT_ID != dwClientID )
		return FALSE;

	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_ATTACH_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;

	if (m_spPostField)
	{
		// 첨부물 관리.
		EMPOST_ATTACH_CODE emAttachCode = m_spPostField->MsgTakeAttachAck( 
			RecvData.dwSendChaNum, 
			RecvData.dwRecvChaNum, 
			RecvData.dwPostID, 
			RecvData.sItem, 
			(EMPOST_MONEY_TYPE)RecvData.emMoneyType,
			RecvData.llMoney, 
			(EMPOST_ATTACH_CODE)RecvData.emCode );

		if( emAttachCode != EMPOST_ATTACH_OK )
		{
			// 캐쉬로 롤백 전송 롤백하자.
			// 

			GLMSG::NET_POST_RENEW_ATTACH_ROLLBACK_REQ SendData;
			SendData.dwPostID = RecvData.dwPostID;

			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, SendData);

			SENDTOCACHE( NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ, SendBuffer );
			return FALSE; 
		}

		GLChar* pChar = GetCharByDbNum( RecvData.dwRecvChaNum );
		if( pChar )
		{
			// 클라 전용 메시지
			// 타입은 동일하다. (NET_MSG_POST_RENEW_ATTACH_ACK)
			GLMSG::NET_POST_RENEW_ATTACH_ACK_FOR_CLIENT SendData;
			SendData.dwPostID = RecvData.dwPostID;
			SendData.emCode = emAttachCode;

			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, SendData);

			SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_POST_RENEW_ATTACH_ACK, SendBuffer );
		}


	}
	return TRUE;
}

BOOL GLGaeaServer::MsgPostRenewReadReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );

	if (pChar)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

		GLMSG::NET_POST_RENEW_READ_REQ RecvData;
		if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
			return FALSE;

		RecvData.dwChaNum = pChar->CharDbNum();

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);


		SENDTOCACHE( NET_MSG_POST_RENEW_READ_REQ, SendBuffer );
	}

	return TRUE;
}


BOOL GLGaeaServer::MsgPostRenewReturnReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );

	if (pChar)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

		GLMSG::NET_POST_RENEW_ATTACH_RETURN_REQ RecvData;
		if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
			return FALSE;

		RecvData.dwReturnChaNum		= pChar->CharDbNum();
		RecvData.strReturnChaName	= pChar->GetName();
		RecvData.dwReturnUserNum	= pChar->GetUserID();

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);

		SENDTOCACHE( NET_MSG_POST_RENEW_RETURN_REQ, SendBuffer );
	}


	return TRUE;
}


BOOL GLGaeaServer::MsgPostRenewReturnAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;

	GLMSG::NET_POST_RENEW_ATTACH_RETURN_ACK RecvData;
	if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
		return FALSE;

	GLChar* pReturnChar = GetCharByDbNum( RecvData.postInfo.dwSendChaNum );
	if ( pReturnChar )
	{
		GLMSG::NET_POST_RENEW_ATTACH_RETURN_ACK_FOR_CLIENT SendData;	// 우편 반송 성공여부
		SendData.emCode = RecvData.emCode;

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);

		SENDTOCLIENT( pReturnChar->GETCLIENTID(), NET_MSG_POST_RENEW_RETURN_ACK, SendBuffer );
	}

	GLChar* pRecvChar = GetCharByDbNum( RecvData.postInfo.dwRecvChaNum );
	if ( pRecvChar )
	{
		GLMSG::NET_POST_RENEW_NEW_POST SendData;	// 새 우편 알림
		SendData.postInfo = RecvData.postInfo;

		msgpack::sbuffer SendBuffer;
		msgpack::pack(SendBuffer, RecvData);

		SENDTOCLIENT( pRecvChar->GETCLIENTID(), NET_MSG_POST_RENEW_NEW_POST, SendBuffer );
	}
	return TRUE;
}

