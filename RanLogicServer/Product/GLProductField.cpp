#include "pch.h"

#include "../../SigmaCore/Math/Random.h"

#include "./GLProductField.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"

#include "../../RanLogic/Msg/ProductMsg.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbAction.h"
#include "../Database/DBAction/DbActionGameProduct.h"

GLProductField::GLProductField( GLGaeaServer* pGaeaServer )
    : m_pGaeaServer(pGaeaServer)
    , m_dwCurrentProductRecipeKey(UINT_MAX)
    , m_nProductCount(1)
    , m_nProductCurrentCount(0)
	, m_bConfirmMaterial(false)

{}

bool GLProductField::CheckProduct_FieldServer( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState , const WORD nCheckCount/* =1 */ )
{
    if ( pProductRecipe == NULL || pCharLogic == NULL )
    {
        eState = Product::EStateERROR;
        return false;
    }
    if ( nCheckCount <= 0 )
    {
        eState = Product::EStateERROR;
        return false;
    }

    BYTE eMaterialState;
    if ( CountProductMaterial( pProductRecipe, pCharLogic, eMaterialState ) < nCheckCount )
    {
        eState = eMaterialState;
        return false;
    }

    if ( CountProductMoney( pProductRecipe, pCharLogic->GetInvenMoney() ) < nCheckCount )
    {
        eState = Product::EStateNotMoney;
        return false;
    }

    if( nCheckCount == 1)
    {
        if ( CheckProductInven( pProductRecipe, pCharLogic ) == false )
        {
            eState = Product::EStateInven;
            return false;
        }
    }
    else
    {
        if ( CountProductInven( pProductRecipe, pCharLogic ) < nCheckCount )
        {
            eState = Product::EStateInven;
            return false;
        }
    }
    eState = Product::EStateEnough;
    return true;
}

bool GLProductField::CheckProductMaterial_FieldServer( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState )
{
	if ( pProductRecipe == NULL || pCharLogic == NULL )
	{
		eState = Product::EStateERROR;
		return false;
	}

	// ��� �� Ȯ��;
	for ( int i=0; i<ITEMMIX_ITEMNUM; ++i )
	{
		if ( pProductRecipe->sMeterialItem[ i ].sNID == NATIVEID_NULL() )
			continue;

		WORD nNum = static_cast< WORD >( pProductRecipe->sMeterialItem[ i ].nNum );
		if ( nNum <= 0 )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sMeterialItem[ i ].sNID );
		if ( !pItem )
			continue;

		for ( int j=0; j<nNum; ++j )
		{
			SINVENITEM* pInvenItem = pCharLogic->m_cInventory.FindItem(
				pProductRecipe->sMeterialItem[ i ].sNID, j );
			if ( !pInvenItem )
				continue;

			const SITEMCUSTOM& sItemCustom = pInvenItem->CustomItem();
			if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
			{
				eState = Product::EStateCostumeFail;
				return false;
			}
		}
	}


	return true;
}

bool GLProductField::ReqProduct( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    GLMSG::SNET_PRODUCT_REQ* pNetMsg = (GLMSG::SNET_PRODUCT_REQ*)nmg;

	if( pNetMsg->nCount <= 0 )
	{
		m_bConfirmMaterial = false;
		return false;
	}

    if( pChar == NULL)
	{
		m_bConfirmMaterial = false;
        return false;
	}

    if( !pChar->IsValidBody() )
	{
		m_bConfirmMaterial = false;
        return false;
	}

    if( pNetMsg->nKey != UINT_MAX )
    {
        // ���� ��û

        if( !pChar->isAction( GLAT_IDLE ) && !pNetMsg->bOngoing )
		{
			m_bConfirmMaterial = false;
            return false;
		}

        if ( pChar->IsCheckedSkillFlagSpecial( EMSPECA_HALLUCINATE ) )
		{
			m_bConfirmMaterial = false;
            return false;
		}

        EMVEHICLE_WHETHER eVehicleState = pChar->VehicleState();
        if( eVehicleState == EMVEHICLE_DRIVER || eVehicleState == EMVEHICLE_PASSENGER )
		{
			m_bConfirmMaterial = false;
            return false;
		}

        const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( pNetMsg->nKey );
        if( !pProductRecipe )
		{
			m_bConfirmMaterial = false;
            return false;
		}

        GLMSG::SNET_PRODUCT_FB NetMsgFb;
        NetMsgFb.dwChaDbNum = pChar->CharDbNum();
        NetMsgFb.nKey = pNetMsg->nKey;
        NetMsgFb.nCount = pNetMsg->nCount;
        NetMsgFb.dwCharGaeaID = pChar->GetGaeaID();

        if ( CheckProduct_FieldServer( pProductRecipe, pChar, NetMsgFb.nState/*, NetMsgFb.nCount*/ ) == false )
        {
            pChar->SendToClient( &NetMsgFb );

			m_bConfirmMaterial = false;

            return false;
        }

		if ( CheckProductMaterial_FieldServer( pProductRecipe, pChar, NetMsgFb.nState ) == false )
		{
			pChar->SendToClient( &NetMsgFb );

			m_bConfirmMaterial = false;

			return false;
		}

		// Ȯ�� ���θ� Ȯ���Ѵ�;
		if ( !m_bConfirmMaterial )
		{
			bool bConfirm = true;

			// Ȯ���ؾ� �� ��ᰡ �ִ��� Ȯ���Ѵ�;
			bConfirm = NeedConfirmProductMaterial( pNetMsg->nKey, pChar );
			if ( bConfirm )
			{
				// Ŭ���̾�Ʈ�� ���� ���� ���������� Ȯ���� ������ �Ǵ��Ѵ�;
				// Ŭ���̾�Ʈ���� ����ϰ� �ʹٸ� ��� ��Ŷ�� �� ���̴�;
				m_bConfirmMaterial = true;

				//sc::writeLogInfo ( sc::string::format("- Request Confirm - Count : %1%", m_nProductCurrentCount+1 ) );

				NetMsgFb.nState = Product::EStateNeedConfirm;
				pChar->SendToClient( &NetMsgFb );

				return true;
			}
		}

		// ������� �Ǽ��� ����Ͽ� �̰������� ��� Ȯ���� �ʱ�ȭ�Ѵ�;
		// ������ ���� �ѹ��� Ȯ���� �� �����Ƿ� ���⿡ �Դٴ� ���� Ȯ�� �Ϸ�Ǿ��ٴ� ���̴�;
		//m_bConfirmMaterial = false;
		
		if( pProductRecipe->fDelaySec >= 0.0f )
		{
			// ù��°�� ���;
			if ( m_nProductCurrentCount == 0 )
			{
				sc::writeLogInfo ( sc::string::format("- Start Product - Count : %1%", m_nProductCurrentCount+1 ) );

				NetMsgFb.nState = Product::EStateMotion;
				pChar->SendToClient( &NetMsgFb );

				// ���� ĳ���Ϳ��� ���� ���� ���ΰ��� ���̵��� �˸�
				pChar->SendMsgViewAround( &NetMsgFb );

				Reset();

				m_dwCurrentProductRecipeKey = pNetMsg->nKey;
				m_nProductCount = pNetMsg->nCount;		
			}
			// �ι�° �̻��� ī��Ʈ�� ���;
			else
			{
				sc::writeLogInfo ( sc::string::format("- Ongoing Product - Count : %1%", m_nProductCurrentCount+1 ) );

				NetMsgFb.nState = Product::EStateOngoingMotion;
				pChar->SendToClient( &NetMsgFb );

				// ���� ĳ���Ϳ��� ���� ���� ���ΰ��� ���̵��� �˸�
				pChar->SendMsgViewAround( &NetMsgFb );
			}

			pChar->SetMotion(
				EMANI_MAINTYPE( GLCONST_CHAR::sProductMotionID.wMainID ),
				EMANI_SUBTYPE( GLCONST_CHAR::sProductMotionID.wSubID ),
				pProductRecipe->fDelaySec,
				boost::bind( boost::mem_fn( &GLChar::CallbackProduct ), pChar ) );
		}
		/*else
		{
			GLMSG::SNET_PRODUCT_FA NetMsgFA;
			NetMsgFA.dwChaDbNum = pChar->CharDbNum();
			NetMsgFA.nKey = pNetMsg->nKey;
			NetMsgFA.nCount = pNetMsg->nCount;
			NetMsgFA.dwCharGaeaID = pChar->GetGaeaID();
			NetMsgFA.llMoney = pChar->GetInvenMoney();

			for( WORD i=0; i<pNetMsg->nCount; ++i )
			{
				DoProductMeterial( pNetMsg->nKey, pChar );
				bool bSuccess = DoProductResult( pNetMsg->nKey, pChar, &NetMsgFA );

				if( bSuccess == false )
				{
					NetMsgFb.nState = Product::EStateRateFail;
					pChar->SendToClient( &NetMsgFb );
				}
			}

			NetMsgFb.nState = Product::EStateComplete;
			pChar->SendToClient( &NetMsgFb );

		}*/
    }
    else
    {
		//sc::writeLogInfo ( "- Cancel Product -" );

        CancleProduct(pChar);
    }

    return true;
}

void GLProductField::CancleProduct(GLChar* pChar )
{
    // ��� ��û
    if( pChar->IsProducting() )
    {
        //pChar->m_fMotionSec = FLT_MAX;
        pChar->TurnAction( GLAT_IDLE );

		GLMSG::SNET_PRODUCT_FB NetMsgFb;
		NetMsgFb.dwChaDbNum = pChar->CharDbNum();
		NetMsgFb.nKey = UINT_MAX;
		NetMsgFb.nState = Product::EStateCancel;
		NetMsgFb.dwCharGaeaID = pChar->GetGaeaID();

		pChar->SendToClient( &NetMsgFb );
		// ��� ���� �� ����ϴ� ���̹Ƿ� �������� �˸�
		pChar->SendMsgViewAround( &NetMsgFb );
    }

	// ��������Ƿ� �����Ѵ�;
	Reset();
}

bool GLProductField::LearnProductBook( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    if( !pChar->IsValidBody() )
        return false;

    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK* pNetMsg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK*)nmg;

    SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( pNetMsg->nPosX, pNetMsg->nPosY );
    if( !pInvenItem )
        return false;

    if( pNetMsg->nItemID != pInvenItem->sItemCustom.GetNativeID().Id() )
        return false;

    if( pChar->CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return false;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if( !pItem || pItem->sBasicOp.emItemType != ITEM_PRODUCT_BOOK )
        return false;

	// Note : ���� ���� Ȯ��
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return false;
	}

    DWORD nProductBook = pItem->sSkillBookOp.sSkill_ID.dwID;

    pChar->ConsumeInvenItem( pNetMsg->nPosX, pNetMsg->nPosY, true );

    std::vector<DWORD> setLearnProductBook;
    setLearnProductBook.push_back( nProductBook );

    m_pGaeaServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ProductBookSave( pChar->CharDbNum(), setLearnProductBook )));

    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB NetMsgFB;
    NetMsgFB.dwChaDbNum = pChar->CharDbNum();
    NetMsgFB.nItemID = pItem->BasicId().dwID;
    pChar->SendToAgent( &NetMsgFB );

    return true;
}

bool GLProductField::TypeReset( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_DEL_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return false;

    bool bOk = CheckTypeReset( nmg, pChar );

    GLMSG::NET_PRODUCT_TYPE_DEL_ACK SendData;
    {
        SendData.m_vecProductType = RecvData.m_vecProductType;
        SendData.m_dwChaDbNum = RecvData.m_dwChaDbNum;
        SendData.m_bOK = bOk;
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaServer->SENDTOAGENT( NET_MSG_PRODUCT_TYPE_DEL_ACK, PackBuffer );

    return true;
}

bool GLProductField::NeedConfirmProductMaterial( DWORD nKey, GLChar* pChar )
{
	if ( NULL == pChar )
	{
		// ��� �� �α� ����;
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��;
		sc::writeLogError( "Can not find GetCharByDbNum on ConfirmProductMaterial " );
		return false;
	}

	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nKey );
	if ( !pProductRecipe )
		return false;

	for ( int i=0; i<ITEMMIX_ITEMNUM; ++i )
	{
		if ( pProductRecipe->sMeterialItem[ i ].sNID == NATIVEID_NULL() )
			continue;

		WORD nNum = static_cast< WORD >( pProductRecipe->sMeterialItem[ i ].nNum );
		if ( nNum <= 0 )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sMeterialItem[ i ].sNID );
		if ( !pItem )
			continue;

		for ( int j=0; j<nNum; ++j )
		{
			SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem(
				pProductRecipe->sMeterialItem[ i ].sNID, j );
			if ( !pInvenItem )
				continue;

			const SITEMCUSTOM& sItemCustom = pInvenItem->CustomItem();
			if ( sItemCustom.IsSetRandOpt() )
			{
				return true;
			}
		}
	}

	return false;
}

void GLProductField::DoProductMeterial( DWORD nKey, GLChar* pChar )
{
    if( pChar == NULL )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError(std::string("Can not find GetCharByDbNum on DoProductMeterial "));
        return;
	}

	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nKey );
	if( !pProductRecipe )
		return;


	for( INT i=0; i<ITEMMIX_ITEMNUM; ++i )
	{
		if( pProductRecipe->sMeterialItem[ i ].sNID == NATIVEID_NULL() )
			continue;

		WORD nNum = static_cast< WORD >( pProductRecipe->sMeterialItem[ i ].nNum );
		if( nNum <= 0 )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sMeterialItem[ i ].sNID );
		if( !pItem )
			continue;

		if( pItem->ISPILE() )
		{
            SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( pProductRecipe->sMeterialItem[ i ].sNID);
            if( !pInvenItem )
                continue;

            if( !pChar->DeletePileItem( pProductRecipe->sMeterialItem[ i ].sNID, nNum ) )
				continue;

			// ��� �� �α� ����
            pChar->LogItemExchange(
                pInvenItem->sItemCustom,
                gamelog::ID_CHAR,
                pChar->m_CharDbNum,
                gamelog::ID_CHAR,
                0,
                ITEM_ROUTE_ITEM_MIX,
                nNum,
				( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_MIX_DEL_ITEM,
                true );

			// Product Exchange Log;
			PRODUCT_EXCHANGE_LOG logData;
			logData.sITEM = pInvenItem->sItemCustom;
			//logData.routeType = ITEM_ROUTE_ITEM_MIX;
			logData.nNum = nNum;
			logData.dwDbNum = pChar->m_CharDbNum;
			logData.dwProductID = 0;
			logData.dwProductType = MAXWORD;
			logData.tracePrice = pChar->GetInvenMoney();
			logData.point = 0;
			logData.totalPoint = 0;

			GLMSG::NET_PRODUCT_EXCHANGE_LOG_FA logMsg( pChar->m_CharDbNum, logData );
			m_pGaeaServer->SENDTOAGENT( &logMsg );
		}
		else
		{
			for( WORD j=0; j<nNum; ++j )
			{
				SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( pProductRecipe->sMeterialItem[ i ].sNID );
				if( !pInvenItem )
					continue;

				if( !pChar->InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 1987, true ) )
					continue;

                // ��� �� �α� ����
				pChar->LogItemExchange(
					pInvenItem->sItemCustom,
					gamelog::ID_CHAR,
					pChar->m_CharDbNum,
					gamelog::ID_CHAR,
					0, 
					ITEM_ROUTE_ITEM_MIX,
					pInvenItem->sItemCustom.wTurnNum,
					( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_MIX_DEL_ITEM,
					true );

				// Product Exchange Log;
				PRODUCT_EXCHANGE_LOG logData;
				logData.sITEM = pInvenItem->sItemCustom;
				//logData.routeType = ITEM_ROUTE_ITEM_MIX;
				logData.nNum = pInvenItem->sItemCustom.wTurnNum;
				logData.dwDbNum = pChar->m_CharDbNum;
				logData.dwProductID = 0;
				logData.dwProductType = MAXWORD;
				logData.tracePrice = pChar->GetInvenMoney();
				logData.point = 0;
				logData.totalPoint = 0;

				GLMSG::NET_PRODUCT_EXCHANGE_LOG_FA logMsg( pChar->m_CharDbNum, logData );
				m_pGaeaServer->SENDTOAGENT( &logMsg );
			}
		}
	}

	if( pProductRecipe->dwPrice > 0 )
	{
		pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), pProductRecipe->dwPrice, FALSE, "ITEM_MIX" );

		pChar->SubtractInvenMoneyServer( pProductRecipe->dwPrice, SUBTRACT_INVEN_MONEY_ITEM_MIX );

		LONGLONG MixPrice = static_cast< LONGLONG >( pProductRecipe->dwPrice );
		// ��� �� �α� ����
		pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, -MixPrice, ITEM_ROUTE_ITEM_MIX, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_MIX_CHARGE );
		// ��� �� �α� ����
		pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, pChar->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

		pChar->SendInvenMoneyUpdate();
	}
}

bool GLProductField::DoProductResult( DWORD nKey, GLChar* pChar, GLMSG::SNET_PRODUCT_FA* pMsg )
{
    if( pChar == NULL )
        return false;

	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nKey );
	if( !pProductRecipe )
		return false;
	
	float fRate = (float)pProductRecipe->dwRate;
	if( !sc::Random::getInstance().IsSucceed( fRate ) )
		return false;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );
	if( !pItem )
		return false;

	if( pItem->ISPILE() )
	{
		SITEMCUSTOM sITEM_NEW( pProductRecipe->sResultItem.sNID );

		sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
		sITEM_NEW.wTurnNum = pProductRecipe->sResultItem.nNum;
		sITEM_NEW.cGenType = EMGEN_ITEMMIX;
		//sITEM_NEW.cChnID = (BYTE)pChar->GetServerChannel();
		//sITEM_NEW.cFieldID = (BYTE)pChar->GetFieldSvrID();
		sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );

		sITEM_NEW.cDAMAGE = pProductRecipe->nGradeAttack;
		sITEM_NEW.cDEFENSE = pProductRecipe->nGradeDefense;

		sITEM_NEW.GenerateBasicStat( false );
		sITEM_NEW.GenerateLinkSkill();
		sITEM_NEW.GenerateAddOption();

		pChar->LogItemBasicStatOption( sITEM_NEW );
		pChar->LogItemLinkSkillOption( sITEM_NEW );
		pChar->LogItemAddonOption( sITEM_NEW );

		if( pProductRecipe->bRandomOption )
		{
			if( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
				pChar->LogItemRandomOption( sITEM_NEW );
		}

		CItemDrop cDropItem;
		cDropItem.sItemCustom = sITEM_NEW;
		pChar->InsertToInven( &cDropItem, true, false, false, false, true );

		//pMsg->iResultItemMakeNum = sITEM_NEW.lnGenNum;
		pMsg->nState = Product::EStateSuccess;
		m_pGaeaServer->SENDTOAGENT( pMsg );
	}
	else
	{
		for( BYTE i=0; i<pProductRecipe->sResultItem.nNum; ++i )
		{
			SITEMCUSTOM sITEM_NEW( pProductRecipe->sResultItem.sNID );
			sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
			sITEM_NEW.cGenType = EMGEN_ITEMMIX;
			//sITEM_NEW.cChnID = (BYTE)pChar->GetServerChannel();
			//sITEM_NEW.cFieldID = (BYTE)pChar->GetFieldSvrID();
			sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );

			sITEM_NEW.cDAMAGE = pProductRecipe->nGradeAttack;
			sITEM_NEW.cDEFENSE = pProductRecipe->nGradeDefense;

			sITEM_NEW.GenerateBasicStat( false );
			sITEM_NEW.GenerateLinkSkill();
			sITEM_NEW.GenerateAddOption();

			pChar->LogItemBasicStatOption( sITEM_NEW );
			pChar->LogItemLinkSkillOption( sITEM_NEW );
			pChar->LogItemAddonOption( sITEM_NEW );

			if( pProductRecipe->bRandomOption )
			{
				if( sITEM_NEW.GENERATE_RANDOM_OPT( false ) )
					pChar->LogItemRandomOption( sITEM_NEW );
			}

			CItemDrop cDropItem;
			cDropItem.sItemCustom = sITEM_NEW;
			pChar->InsertToInven( &cDropItem, true, false, false, false, true );

			//pMsg->iResultItemMakeNum = sITEM_NEW.lnGenNum;
			pMsg->nState = Product::EStateSuccess;
			m_pGaeaServer->SENDTOAGENT( pMsg );
		}
	}

	// ������ �Ϸ�Ǿ��� ������ ��� Ȯ���� �ʱ�ȭ�Ѵ�;
	// �������� ���� ������ ����� ��츦 ���ؼ� ��� �������� �ʴ´�;
	m_bConfirmMaterial = false;

	return true;
}

bool GLProductField::CheckTypeReset( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    if( pChar == NULL )
        return false;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_PRODUCT_TYPE_DEL_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return false;

    SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( RecvData.m_nInvecPosX, RecvData.m_nInvecPosY );
    if( !pInvenItem )
        return false;

    if( pChar->CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return false;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if( !pItem || pItem->sBasicOp.emItemType != ITEM_PRODUCT_RESET )
        return false;

	// Note : ���� ���� Ȯ��
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return false;
	}

    WORD wAmount = pItem->sDrugOp.dwCureVolume;  

    if( wAmount < RecvData.m_vecProductType.size() )
        return false;

    if( pChar->ConsumeInvenItem( RecvData.m_nInvecPosX, RecvData.m_nInvecPosY, true ) == false )
        return false;

    return true;
}

void GLProductField::Reset()
{
	//sc::writeLogInfo ( "- Reset Product -" );

    m_dwCurrentProductRecipeKey = UINT_MAX;
    m_nProductCount = 1;
    m_nProductCurrentCount = 0;
	m_bConfirmMaterial = false;
}
