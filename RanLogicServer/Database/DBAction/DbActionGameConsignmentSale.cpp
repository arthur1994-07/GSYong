#include "pch.h"

#include "../../../SigmaCore/Util/DateTime.h"

#include "../../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../Server/CacheServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../Util/GLItemLimit.h"

#include "../../FieldServer/GLGaeaServer.h"

#include "../DbManager.h"
#include "DbActionGameConsignmentSale.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



namespace db
{

int ConsignmentSaleCheck::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
    if ( !pFieldServer )
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if ( !pGaeaServer || !m_pSubDbManager )
        return sc::db::DB_ERROR;

    if ( m_pSubDbManager->ConsignmentsSaleInsert( m_SaleItem ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    GLMSG::NET_CONSIGNMENT_ITEM_REGIST_REQ SendData;
    {
        SendData.SaleItem = m_SaleItem;
        SendData.fCommissionRate = m_fCommissionRate;
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ, PackBuffer );

	// LOG ���
	CTime timeTemp = m_SaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();
	m_pSubDbManager->ConsignmentSaleLogInsert(
		m_SaleItem.sItemcustom.guid,
		m_SaleItem.dwChaDbNum,
		m_SaleItem.SellType,
		private_market::STATE_REGIST,
		m_SaleItem.llPrice,
		m_dwFee,	//  ��Ͻ� ����Ʈ ���
		m_SaleItem.sItemcustom.Mid(),
		m_SaleItem.sItemcustom.Sid(),
		0, 
		m_SaleItem.wTotalTurnNum,
		pFieldServer->GetServerGroup()/*m_SaleItem.sItemcustom.wRanPointTradeCount*/,
		timeTemp.GetTime() / 3600  );

    return sc::db::DB_OK;
}

int ConsignmentSaleSendBack::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer && !m_pSubDbManager )
        return sc::db::DB_ERROR;

    // ������ ������ �ȵȴ�.
    GLChar* pChar = pFieldServer->GetGLGaeaServer()->GetCharByDbNum( m_PostInfo.iRecver );
    if( pChar == NULL )
        return sc::db::DB_ERROR;


    //�������� ����´�.
    private_market::ConsignmentSaleItem SaleItem;
    if( m_pSubDbManager->ConsignmentsSaleGet( m_Guid, SaleItem ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    // �������� ���� �� ����.
    if( SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
    {	
        sc::writeLogError( sc::string::format("ConsignmentSaleSendBack::Execute() NATIVEID_NULL! ChaNum:%1%, ITEMGUID:%2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ));        
        return sc::db::DB_ERROR;
    }

    // ������ ������ ������ �÷��� ������ ��¥( ������ ���̳ʸ��� ������ �������� �ʱ� ���� )
    SaleItem.sItemcustom.wTurnNum = SaleItem.wTurnNum;

    // ���̻� DB�� ���� ���� �ʿ䰡 ����.
    if( m_pSubDbManager->ConsignmentsSaleDelete( m_Guid ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

	// ����Ʈ�ŷ� ������ �������� ��ũ��Ʈ �������� ����, ����Ʈ�ŷ� �ø��� �ڵ忡�� guid null üũ ����� ������, �ϰ�
	// ������ ���°� none �̰� guid �� guid null �̸� ���� �ڵ忡�� ������ guid �� ����, ������ ��ȯ���̱� ������ �����ۻ��µ� NONE �ϰŴ�. �⺻��
	// �����ϴ� �ڵ忡�� �Ʒ� �ڵ尡 �߰��Ǿ�����, ������ ��ȯ�� �������̰�, �ǸűⰣ�� ���� ������ �������� �κ��� �����̰�
	// ���� �������� �������� �������� ���ԵǴµ� ���� ÷�ξ������� �ް� �κ��� ������ �� sp_ItemCreate ���� �߻���
	// ���� guid �� null �̰� ������ ���´� db::DB_NONE �̴�.
	if ( SaleItem.sItemcustom.IS_GUID_NULL() && db::DB_NONE == SaleItem.sItemcustom.GetDbState() )
	{
		sc::SeqUniqueGUID::Instance()->getGUID( SaleItem.sItemcustom.guid );
		SaleItem.sItemcustom.SetDbState( db::DB_INSERT, true );
		sc::writeLogInfo( sc::string::format( "ConsignmentSaleSendBack, old item structure sale guid is %1%", sc::string::uuidToString( SaleItem.GetGUID() ).c_str() ) );
	}

	// [2/20/2017 khkim]
	// ������ �������� �̵��Ǿ guid �� ������� �ʵ��� �����ϸ鼭, ���λ����˻����� �ŷ��Ǵ� �������� ��񿡼� �����ǵ���
	// ����Ǿ���.(����Ʈ�ŷ��� ��� ����� �� �����ȴ�.) �����Ҹ� �ϴ� �������� �������� �űԷ� �־��ִ� ������� ����
	// �̷��� ����� ����Ǹ�, ������ �̹� ��ϵǾ� �ִ� �������� ��� ���� ó���� ���־�� �Ѵ�. (GmCharEdit �� �۾� �ʿ�)
	// GmEdit �۾� ���� �����ʰ�, ����Ʈ�ŷ� ��ϵǾ� �ִ� �����۵� ��� expire ó���Ѵ�. 
	// [ExpirationDate] = '1970-01-01 09:00:00.000' ���� ��� ������Ʈ �ߴ�.
	// '1999-12-31 00:00:00.000' �� __time64_t �� �����ϸ� 0 �̴�.
	// �� ������Ʈ���� �̹� ��ϵǾ� �ִ� �����۵��� ����ó���� �Ǿ��� ������ ����Ʈ�ŷ�ui���� ã�ư� �� �ۿ� ����. �̷� �����۵���
	// db::DB_INSERT �� �������� �ʴ´�.
	if ( SaleItem.ExpirationDate != 0 )
	{
		SaleItem.sItemcustom.SetDbState( db::DB_INSERT, true );
	}

    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB SendData;
    {
        SendData.SaleItem = SaleItem;
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    
    NET_MSG_PACK msg;
    if (msg.SetData(NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB, PackBuffer))
        pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );
        
    return sc::db::DB_OK;

}

int ConsignmentSalePurchase::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
    if ( !pFieldServer )
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if ( !pGaeaServer || !m_pSubDbManager )
        return sc::db::DB_ERROR;

    // �����ڰ� ������ �ȵȴ�.
    GLChar* pChar = pGaeaServer->GetCharByDbNum( m_PostInfo.iRecver );
    if ( pChar == NULL )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error pChar == NULL buyer: %1%", 
			m_PostInfo.iRecver ) );

        return sc::db::DB_ERROR;
	}

    // �������� ����´�.
    private_market::ConsignmentSaleItem SaleItem;
    if ( m_pSubDbManager->ConsignmentsSaleGet( m_Guid, SaleItem ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error ConsignmentsSaleGet fail buyer: %1% seller: %2% guid: %3%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );

        return sc::db::DB_ERROR;
	}

    // �������� ���� �� ����.
    if ( SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
    {
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() buyer: %1% seller: %2% guid: %3%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );

        return sc::db::DB_ERROR;
    }

    // �����ϴ� ���������� Ȯ��
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( SaleItem.sItemcustom.GetNativeID() );
    if ( !pITEM )
    {
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error !pITEM buyer: %1% seller: %2% guid: %3% mid: %4% sid: %5%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.GetNativeID().wMainID, SaleItem.sItemcustom.GetNativeID().wSubID ) );

        return sc::db::DB_ERROR;
    }

	// ������ �� �Ҹ��� ��� �α�
	// ReqItemPurchase ���� ����� ��ó���Ǿ���, ���⿡�� �α� ó��
	EMITEM_ROUTE ItemRoute = ITEM_ROUTE_CONSIGMENT_SALE;
	bool bServerIsDifferent = false;
	if ( SaleItem.ServerGroup != static_cast< int >( pFieldServer->GetServerGroup() ) )
	{
		bServerIsDifferent = true;
		ItemRoute = ITEM_ROUTE_CONSIGMENT_SALE_SERVERGROUP_DIFF;
	}

	switch ( m_MsgFB.nSellType )
	{
	case private_market::eSellType_ConsignmentSale_RanPoint:
		{
			pChar->LogMoneyExchangeWithItemGuid( 
				gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, 
				-m_MsgFB.llItemPriceTotal, 
				ItemRoute, 
				pChar->GetCurrentMap(), 
				LOG_RANPOINT_MINUS_BY_CONSIGEMENT, 
				SaleItem.sItemcustom.guid );
		}
		break;
	case private_market::eSellType_ConsignmentSale_Gold:
		{
			pChar->LogMoneyExchangeWithItemGuid( 
				gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, 
				-m_MsgFB.llItemPriceTotal, 
				ItemRoute, 
				pChar->GetCurrentMap(), 
				LOG_MONEY_MINUS_BY_CONSIGEMENT, 
				SaleItem.sItemcustom.guid );
		}
		break;
	}

	// ������ ����üũ
	if ( SaleItem.sItemcustom.wTurnNum < m_nPurchaseNumber )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error SaleItem.sItemcustom.wTurnNum < m_nPurchaseNumber buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );

		return sc::db::DB_ERROR;
	}

    // ������ ������ ������ �÷��� ������ ��¥( ������ ���̳ʸ��� ������ �������� �ʱ� ���� )
    SaleItem.sItemcustom.wTurnNum =  SaleItem.wTurnNum;

    // ����Ʈ �ŷ� Ƚ���� ���� ��Ų��.
    SaleItem.sItemcustom.wRanPointTradeCount++;

    //����� -�� �ɼ� ������ 0�� �ɼ� �ִ�.
    if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum - m_nPurchaseNumber ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error ConsignmentsUpdateTurnNum fail buyer: %1% seller: %2% guid: %3%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );

        return sc::db::DB_ERROR;
	}

    // ���ӸӴ� �ŷ��ΰ�?
    bool bIsGameMoney = false;

    std::vector< private_market::GameMoneyItemFee > vecMoneyPeriodFee;
    vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();
    for( unsigned int i=0; i<vecMoneyPeriodFee.size(); i++ )
    {
        const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

        if ( value.sNativeID == SaleItem.sItemcustom.GetNativeID() )
        {
            bIsGameMoney = true;
            break;
        }
    }

    // ���ӸӴϸ� �����Ѱ�� ���� �������� ÷���Ѵ�.
    if ( bIsGameMoney )
    {
		// ������ ���ӸӴ� ������ 1�̶�� �����ϰ� �ڵ尡 �Ǿ� �־ �׿� ���߾ ������
        LONGLONG llGameMoney = pITEM->sBasicOp.dwSellPrice * m_nPurchaseNumber;

        m_PostInfo.bAttach01        = false;
        m_PostInfo.byPostType       = EMPOST_MONEY;                      // ���� Ÿ��
        m_PostInfo.llAttachMoney    = llGameMoney;      
        
        if ( m_pSubDbManager->PostSend( m_PostInfo, SITEMCUSTOM() ) == sc::db::DB_ERROR )
        {
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, m_MsgFB );

			NET_MSG_PACK msg;
			if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
				pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

			sc::writeLogError( sc::string::format( 
				"ConsignmentSalePurchase::Execute error PostSend(gamemoney) fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
				m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );

			// ������ ���� ������Ʈ�Ѱ� �ѹ�
			if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum ) == sc::db::DB_ERROR )
			{
				sc::writeLogError( sc::string::format( 
					"ConsignmentSalePurchase::Execute error updateturnnum fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
					m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );
			}

            return sc::db::DB_ERROR;
        }

		pChar->LogMoneyExchangeWithItemGuid( 
			gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pChar->m_CharDbNum, 
			llGameMoney, 
			ItemRoute, 
			pChar->GetCurrentMap(), 
			LOG_MONEY_CONSIGEMENT_SALE_MONEY_BUY_SENT_POST, 
			SaleItem.sItemcustom.guid );
    }
    else
    {
        m_PostInfo.bAttach01 = true;

        // ������ ����
        SITEMCUSTOM SendItemCustom = SaleItem.sItemcustom;
        SendItemCustom.wTurnNum = m_nPurchaseNumber;

		// ������ ����� �� ���� ������ guid �� ����ϵ��� ����Ǿ�����, ������ ���� ��������
		// ��ϵǾ� �ִ� �����۵� ������(���� ���̺�ó�� Į���� �߰��ؼ� ��ȯ ó���Ѵٴ��� �ϴ� ���ŷο� �۾��� ���� ����)
		// guid �� �ٽ� �ѹ� �־��ش�. ������ ��� �κ� �����ĺ��ʹ� ���ʿ��� �۾�������, ������ ��ϵ� ������ ������ �ʿ��ϴ�.
		// �������� ������� ������ �� �κ� �ּ�ó����
		// ������ �������� �� �������� Ű���� ����Ʈ�ŷ� Ű������ �����ۿ��ٰ� �־���µ�, �̷��� �ϴ� ������Ʈ �������鼭 �������� �����
		// �������� �Ǹ��ڰ� �ø� �� �������̴ϱ� ������ ���´� ������Ʈ���װ�, �ű⿡ Ű���� �����ؼ� �־��ִ� �翬�� �������� ������ ������Ʈ�� �ȵ�
		/*SendItemCustom.SetGuid( m_Guid );*/

		// ����Ʈ�ŷ� ������ �������� ��ũ��Ʈ �������� ����, ����Ʈ�ŷ� �ø��� �ڵ忡�� guid null üũ ����� ������, �ϰ�
		// ������ ���°� none �̰� guid �� guid null �̸� ���� �ڵ忡�� ������ guid �� ����, ������ ��ȯ���̱� ������ �����ۻ��µ� NONE �ϰŴ�. �⺻��
		if ( SendItemCustom.IS_GUID_NULL() && db::DB_NONE == SendItemCustom.GetDbState() )
		{
			sc::SeqUniqueGUID::Instance()->getGUID( SendItemCustom.guid );
			SendItemCustom.SetDbState( db::DB_INSERT, true );
			sc::writeLogInfo( sc::string::format( "ConsignmentSalePurchase, old item structure sale guid is %1%", sc::string::uuidToString( SaleItem.GetGUID() ).c_str() ) );
		}
		
		if ( pITEM->ISPILE() )
		{
			// ��ħ �������� �ű� guid �����Ѵ�.
			sc::SeqUniqueGUID::Instance()->getGUID( SendItemCustom.guid );
			SendItemCustom.GenerateBasicStat( false );
			SendItemCustom.GenerateLinkSkill();
			SendItemCustom.GenerateAddOption();
		}

		/*
		// ������ �ٸ���, ������ ���� ����
		if ( pGaeaServer->GetServerGroup() != SaleItem.ServerGroup )
		{
			// ���� ������ ���̺� ������ �������� �����Ǿ ���� ���̺��� ���� ��Ű�� �ʱ� ������
			// ������ �Դٰ��� �ϸ� ����Ű ������ ������ �ȵȴ�. 
			// ���� ��� 1������ A�������� 2�������� ���Ÿ� �ϰ�, �ٽ� A�������� 1�������� �����Ѵٸ� ������ �߻���
			// �׷��� Ű�� �űԷ� �����Ѵ�.
			sc::SeqUniqueGUID::Instance()->getGUID( SendItemCustom.guid );
			SendItemCustom.SetDbState( db::DB_INSERT, true );
		}
		*/

		// 2/20/2017 khkim
		// ������ �������� �̵��Ǿ guid �� ������� �ʵ��� �����ϸ鼭, ���λ����˻����� �ŷ��Ǵ� �������� ��񿡼� �����ǵ���
		// ����Ǿ���.(����Ʈ�ŷ��� ��� ����� �� �����ȴ�.) �����Ҹ� �ϴ� �������� �������� �űԷ� �־��ִ� ������� ����
		// �̷��� ����� ����Ǹ�, ������ �̹� ��ϵǾ� �ִ� �������� ��� ���� ó���� ���־�� �Ѵ�. (GmCharEdit �� �۾� �ʿ�)
		SendItemCustom.SetDbState( db::DB_INSERT, true );

        // ����(����) �����ϰ� �������� �������� ������.
        if ( m_pSubDbManager->PostSend( m_PostInfo, SendItemCustom ) == sc::db::DB_ERROR )
        {
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, m_MsgFB );

			NET_MSG_PACK msg;
			if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
				pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

			sc::writeLogError( sc::string::format( 
				"ConsignmentSalePurchase::Execute error PostSend(item) fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
				m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );

			// ������ ���� ������Ʈ�Ѱ� �ѹ�
			if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum ) == sc::db::DB_ERROR )
			{
				sc::writeLogError( sc::string::format( 
					"ConsignmentSalePurchase::Execute error updateturnnum fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
					m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );
			}

            return sc::db::DB_ERROR;
        }
    }

    // 1. Send Notify to the Recver via AgentServer
    GLMSG::SNET_POST_NOTIFY msgNotify;
    msgNotify.llPostID = m_PostInfo.llPostID;	// completed postsend
    msgNotify.iRecver  = m_PostInfo.iRecver;
    pGaeaServer->SENDTOAGENT( &msgNotify );

	if ( bServerIsDifferent )
	{
		pChar->LogItemExchange( 
			SaleItem.sItemcustom,
			gamelog::ID_CHAR,
			m_PostInfo.iSender, 
			gamelog::ID_CHAR, 
			m_PostInfo.iRecver, 
			ItemRoute,
			m_nPurchaseNumber,
			pChar->GetCurrentMap(), 
			LOG_ITEM_CONSIGEMENT_BUY_RECV_BY_POST,
			true );
	}
	else
	{
		pChar->LogItemExchange( 
			SaleItem.sItemcustom,
			gamelog::ID_CHAR,
			m_PostInfo.iSender, 
			gamelog::ID_CHAR, 
			m_PostInfo.iRecver, 
			ItemRoute,
			m_nPurchaseNumber,
			pChar->GetCurrentMap(), 
			LOG_ITEM_CONSIGEMENT_BUY_RECV_BY_POST,
			true );
	}

    // cache �� ���λ��� �Ǹŵ� ������ ���� ����
    GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH NetMsgFH;
    NetMsgFH.dwItemTurnNum = SaleItem.sItemcustom.wTurnNum - m_nPurchaseNumber;
    NetMsgFH.sItemGUID = m_Guid;
    NetMsgFH.bLog = true;
    pGaeaServer->SENDTOINTEGRATIONCACHE( &NetMsgFH );

    // �Ǹ��ڰ� ������ �ǸŵǾ����� �˸���.
	if ( false == bServerIsDifferent )
	{
		GLChar* pSeller = pGaeaServer->GetCharByDbNum( SaleItem.dwChaDbNum );
		if ( pSeller )
		{
			// �Ǹ��ڿ��� �ǸŸ� �˸�
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			{
				SendData.strGUID = SaleItem.GetStrGUID();
				SendData.nPurchaseCount = m_nPurchaseNumber;
			}
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			pGaeaServer->SENDTOCLIENT( pSeller->ClientSlot(), NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );
		}
	}

	// LOG ���
	CTime timeTemp = SaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();
	m_pSubDbManager->ConsignmentSaleLogInsert( 
		SaleItem.sItemcustom.guid,
		m_PostInfo.iRecver,
		SaleItem.SellType,
		private_market::STATE_BUY,
		SaleItem.llPrice,
		0,
		SaleItem.sItemcustom.Mid(),
		SaleItem.sItemcustom.Sid(),
		0,
		static_cast< WORD >( m_nPurchaseNumber )/*SaleItem.wTotalTurnNum*/,
		pFieldServer->GetServerGroup()/*SaleItem.sItemcustom.wRanPointTradeCount*/,
		timeTemp.GetTime() / 3600  );

    return sc::db::DB_OK;
}

/*
int ConsignmentSalePurchaseForCacheServer::Execute( NetServer* pServer )
{
	CacheServer* pCacheServer = reinterpret_cast< CacheServer* >( pServer );
	if ( pCacheServer == NULL )
		return sc::db::DB_ERROR;

	GLMSG::NET_CONSIGNMENT_SALE_UPDATE_ITEM_ACK SendData;
	SendData.Result = -1;
	SendData.strBuyerName = m_BasicMsg.strBuyerName;
	SendData.dwBuyerDbNum = m_BasicMsg.dwBuyerDbNum;
	SendData.strSellerName = m_BasicMsg.strSellerName;
	SendData.dwSellerDbNum = m_BasicMsg.dwSellerDbNum;

	SendData.strGUID = m_BasicMsg.strGUID;
	SendData.nPurchaseCount = m_BasicMsg.nPurchaseCount;
	SendData.llItemPriceTotal = m_BasicMsg.llItemPriceTotal;
	SendData.nSellType = m_BasicMsg.nSellType;
	SendData.dwBuyerFieldSlot = m_BasicMsg.dwBuyerFieldSlot;

	// �������� ����´�.
	private_market::ConsignmentSaleItem SaleItem;
	if ( m_pSubDbManager->ConsignmentsSaleGet( m_Guid, SaleItem ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		return sc::db::DB_ERROR;
	}

	// �������� ���� �� ����.
	if ( SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
	{	
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		sc::writeLogError( sc::string::format( "ConsignmentSalePurchaseForCacheServer::Execute() NATIVEID_NULL! %1%, %2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );   
		return sc::db::DB_ERROR;
	}

	// �����ϴ� ���������� Ȯ��
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( SaleItem.sItemcustom.GetNativeID() );
	if ( !pITEM )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		sc::writeLogError( sc::string::format( "ConsignmentSalePurchaseForCacheServer::Execute() NATIVEID_NULL! %1%, %2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );   
		return sc::db::DB_ERROR;
	}

	// ������ ����üũ
	if ( SaleItem.sItemcustom.wTurnNum < m_nPurchaseNumber )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		return sc::db::DB_ERROR;
	}

	// ������ ������ ������ �÷��� ������ ��¥( ������ ���̳ʸ��� ������ �������� �ʱ� ���� )
	SaleItem.sItemcustom.wTurnNum =  SaleItem.wTurnNum;

	// ����Ʈ �ŷ� Ƚ���� ���� ��Ų��.
	SaleItem.sItemcustom.wRanPointTradeCount++;

	//����� -�� �ɼ� ������ 0�� �ɼ� �ִ�.
	if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum - m_nPurchaseNumber ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		sc::writeLogError( sc::string::format( "ConsignmentSalePurchaseForCacheServer::Execute() ConsignmentsUpdateTurnNum %1%, %2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );
		return sc::db::DB_ERROR;
	}

	SendData.Result = 0;
	SendData.Attach = SaleItem.sItemcustom;
	SendData.Attach.wTurnNum = m_nPurchaseNumber;

	// ������ ����� �� ���� ������ guid �� ����ϵ��� ����Ǿ�����, ������ ���� ��������
	// ��ϵǾ� �ִ� �����۵� ������(���� ���̺�ó�� Į���� �߰��ؼ� ��ȯ ó���Ѵٴ��� �ϴ� ���ŷο� �۾��� ���� ����)
	// guid �� �ٽ� �ѹ� �־��ش�. ������ ��� �κ� �����ĺ��ʹ� ���ʿ��� �۾�������, ������ ��ϵ� ������ ������ �ʿ��ϴ�.
	SendData.Attach.SetGuid( m_Guid );

	if ( pITEM->ISPILE() )
	{
		// ��ħ �������� �ű� guid �����Ѵ�.
		sc::SeqUniqueGUID::Instance()->getGUID( SendData.Attach.guid );
	}

	if ( !m_BasicMsg.bSameServerGroup )
	{
		SendData.Attach.SetDbState( db::DB_INSERT, true );
	}

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );

	NET_MSG_PACK MsgPack;
	if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
		pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

	return sc::db::DB_OK;
}
*/

int ConsignmentSaleGetList::Execute( NetServer* pServer )
{
    if( m_pSubDbManager == FALSE )
        return sc::db::DB_ERROR;

    // ��Ŷ ����� Ŀ���ٸ� ���� �����ؾ� �Ѵ�.
    // ����� n������ ����Ŷ�� ��Ƽ� ���� �� �ִ�.
	// int nBunch = 7;

	// ��ó�� ������ġ�� �ڰԵǸ� ����������;
	// -64�� ������ ��Ŷ���� ���� �ҷ�, ���Ͱ��� �ֱ⶧��, ��ŷ���� ������� ���� �𸣱⶧���� 64����;
	// ���� �⺻ 32, �ҷ�4;
    int nBunch = ( (NET_DATA_BUFSIZE-64)/ sizeof ( private_market::ConsignmentSaleItem ) );

    // �������� ������.
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if( pFieldServer == NULL )
        return sc::db::DB_ERROR;

    private_market::SALE_ITEMS_VECTOR vecSaleItems;
    if( m_pSubDbManager->ConsignmentsSaleSelect( vecSaleItems, m_dwChaDbNum, pFieldServer->GetServerGroup() ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    if( pServer == NULL )
        return sc::db::DB_ERROR;

    GLChar* pChar = pFieldServer->GetGLGaeaServer()->GetCharByDbNum( m_dwChaDbNum );
    if( pChar && vecSaleItems.empty() == false )
    {
        m_nListCount = vecSaleItems.size();

        GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_ACK SendData;
        int nPacketCount=0;
        for( UINT i=1 ; i<=vecSaleItems.size() ; i++ )
        {            
            SendData.vecSaleItems.push_back( vecSaleItems[i-1] );

            if( i==vecSaleItems.size() )
                SendData.m_bIsLast = true;
            else if( i%nBunch==0 )
                SendData.m_bIsLast = false;
            else
                continue;

            msgpack::sbuffer PackBuffer;
            msgpack::pack( PackBuffer, SendData );
            pFieldServer->SendClient( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK, PackBuffer );
            SendData.vecSaleItems.clear();
        }
    }
    
    return sc::db::DB_OK;
}

int ConsignmentSaleGetListChche::Execute( NetServer* pServer )
{
    private_market::SALE_ITEMS_VECTOR vecSaleItems;
    if( m_pSubDbManager->ConsignmentsSaleSelect( vecSaleItems ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    // �������� ������.
    CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    if( pCacheServer == NULL )
        return sc::db::DB_ERROR;

    if( vecSaleItems.empty() == false )
    {
        GLMSG::NET_CONSIGNMENT_ITEM_REGIST_REQ SendData;

        for( UINT i=0 ; i<vecSaleItems.size() ; i++ )
        {            
            SendData.SaleItem = vecSaleItems[i];
            SendData.fCommissionRate  = m_fCommissionRate;

            msgpack::sbuffer PackBuffer;
            msgpack::pack( PackBuffer, SendData );

            NET_MSG_PACK SendData;
            if (SendData.SetData(NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ, PackBuffer))
                pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &SendData);
        }
    }
    return sc::db::DB_OK;
}


int ConsignmentSaleSlotExpansionDateUpdate::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer && !m_pSubDbManager )
        return sc::db::DB_ERROR;

    if( m_pSubDbManager == FALSE )
        return sc::db::DB_ERROR;

    if( m_pSubDbManager->ConfignmentSaleSlotExpansionDateSet( m_dwChaDbNum, m_tExpansionDate ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    return sc::db::DB_OK;
}


int ConsignmentSaleLogInsert::Execute( NetServer* pServer )
{
	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
	if (!pFieldServer)
		return sc::db::DB_ERROR;

	// LOG ���
	CTime timeTemp = m_SaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();
	
	if( m_pSubDbManager->ConsignmentSaleLogInsert(
			m_guid,
			m_SaleItem.dwChaDbNum,
			m_SaleItem.SellType,
			m_logType,
			m_SaleItem.llPrice,
			m_llFee,
			m_SaleItem.sItemcustom.Mid(),
			m_SaleItem.sItemcustom.Sid(),
			0, //m_SaleItem.sItemcustom.lnGenNum,
			m_SaleItem.wTurnNum/*m_SaleItem.wTotalTurnNum*/,
			pFieldServer->GetServerGroup()/*m_SaleItem.sItemcustom.wRanPointTradeCount*/,
			timeTemp.GetTime() / 3600 ) == sc::db::DB_ERROR )
	{
		return sc::db::DB_ERROR;
	}


    return sc::db::DB_OK;
}

} // namespace db
