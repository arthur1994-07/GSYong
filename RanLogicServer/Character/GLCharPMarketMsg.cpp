#include "../pch.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Club/GLClubServer.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../Util/GLItemLimit.h"
#include "../Guidance/GLGuidance.h"
#include "../Database/DBAction/DbActionGamePet.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLChar::IsPrivateMarketOpen() const
{
    return m_pPrivateMarket->IsOpen();
}

void GLChar::PrivateMarketClose()
{
    m_pPrivateMarket->DoMarketClose();
}

void GLChar::PrivateMarketFindItem(const SSEARCHITEMDATA& sSearchData, std::vector<SFINDRESULT>& vecFindResult)
{
    m_pPrivateMarket->FindItem(sSearchData, vecFindResult);
}

const std::string& GLChar::GetPrivateMarektTitle() const
{
    return m_pPrivateMarket->GetTitle();
}

const MAPSEARCH& GLChar::GetPrivateMarketSearchItemList() const
{
    return m_pPrivateMarket->GetSearchItemList();
}

void GLChar::PrivateMarketSearchResultClear()
{
    m_PrivateMarketSearchResult.clear();
}

SSEARCHITEMESULT GLChar::GetPrivateMarketSearchResult(size_t Index)
{
    if (Index >= m_PrivateMarketSearchResult.size())
    {
        SSEARCHITEMESULT Temp;
        return Temp;
    }
    else
    {
        return m_PrivateMarketSearchResult[Index];
    }
}

void GLChar::AddPrivateMarketSearchResult(const SSEARCHITEMESULT& Data)
{
    m_PrivateMarketSearchResult.push_back(Data);
}

DWORD GLChar::GetPrivateMarketItemTurnNum( const UUID& sItemGUID )
{
	DWORD dwItemTurnNum = 0;
	SSALEITEM* pSALEITEM = m_pPrivateMarket->GetItem( sItemGUID );
	if ( pSALEITEM )
	{
		dwItemTurnNum = pSALEITEM->dwNUMBER;
	}
	return dwItemTurnNum;
}

SSALEITEM* GLChar::GetPrivateMarketItem( SNATIVEID sSALEPOS )
{
	return m_pPrivateMarket->GetItem( sSALEPOS );
}

HRESULT GLChar::MsgPMarketTitle(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PMARKET_TITLE *pNetMsg = (GLMSG::SNETPC_PMARKET_TITLE *) nmg;

	GLMSG::SNETPC_PMARKET_TITLE_FB	NetMsgFB;

    const SMAPNODE *pMapNode = m_pGaeaServer->FindMapNode( GetCurrentMap() );
    if ( pMapNode && !pMapNode->IsPrivateMarketOpen() )	
        return S_FALSE;

	if (m_pPrivateMarket->IsOpen())
	{
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// �ʱ�ȭ�� ���Ѵ�.
	m_pPrivateMarket->DoMarketClose();
	m_pPrivateMarket->SetTITLE(pNetMsg->m_PrivateMarketTitle);

	NetMsgFB.SetTitle(m_pPrivateMarket->GetTitle());
	SendToClient( &NetMsgFB );

	if ( true == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		// cache ������ ���λ��� �Ǹ��� ���� �����Ѵ�.
		// ���� ���λ����� �Ʒ��� MsgPMarketOpen ���� ����������, �� �κп��� ����
		// �Ǹ��� ������ �������ش�.
		GLMSG::SNETPC_PMARKET_SELLER_REGISTER_FH NetMsgFH;
		NetMsgFH.dwChaDbNum = GetCharID();
		NetMsgFH.sMapID = GetCurrentMap().getGaeaMapID();
		NetMsgFH.wSchool = GetSchool();
		NetMsgFH.nChannel = GetServerChannel();
		NetMsgFH.nServerGroup = m_pGaeaServer->GetServerGroup();
		NetMsgFH.SetName( GetName() );

		SendToIntegrationCache( &NetMsgFH );
	}

	return S_OK;
}

HRESULT GLChar::MsgPMarketReqItem( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PMARKET_REGITEM *pNetMsg = ( GLMSG::SNETPC_PMARKET_REGITEM* ) nmg;
	GLMSG::SNETPC_PMARKET_REGITEM_FB NetMsgFB;

	if ( pNetMsg->llMoney <= 0 )
	{
		sc::writeLogError( sc::string::format( "MsgPMarketReqItem pNetMsg->llMoney is minus chanum:%1%", m_CharDbNum ) );
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	if ( m_pPrivateMarket->IsOpen() )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM* pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : ����� �� �ִ� �ѵ��� �Ѿ�� �ֽ��ϴ�.
	if ( m_pPrivateMarket->GetItemNum() >= EPrivateMarketTotalSlot )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_MAXNUM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// Note : ����� �� �ִ� �ݾ� �ѵ��� �Ѿ�� �ֽ��ϴ�.
	if ( pNetMsg->llMoney > LL_LIMITED_A_UNIT_PRICE )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_LIMITED_MONEY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	SNATIVEID nidITEM = pINVENITEM->sItemCustom.GetNativeID();
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( nidITEM );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// ��ī���̸鼭 ���� Ȱ��ȭ �Ǿ� ������ ������ ����� �� ����.
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		PGLPETFIELD pMyPet = GetMyPet();
		if ( pMyPet && pMyPet->IsValid () && pINVENITEM->sItemCustom.PetDbNum() == pMyPet->DbNum() )
            return S_FALSE;
	}

	if ( ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE ) && pINVENITEM->sItemCustom.m_VehicleDbNum != 0 )
	{
		return E_FAIL;
	}

	DWORD dwNum = pNetMsg->dwNum;

	//	Note : �̹� ��ϵ� ���������� �˻�.
	bool bREGPOS = m_pPrivateMarket->IsRegInvenPos( SNATIVEID( pNetMsg->wPosX, pNetMsg->wPosY ) );
	if ( bREGPOS )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_REGITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	�ŷ��ɼ�
	if ( !( pINVENITEM->sItemCustom.IsExchange() != NULL ) )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_NOSALE;
		SendToClient( &NetMsgFB );
		return FALSE;
	}
	
	// GMCharEdit �� ���� �������� �ǸŰ� �Ұ����ϴ�.
	if ( pINVENITEM->sItemCustom.IsGM_GENITEM() )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_NOSALE;
		SendToClient( &NetMsgFB );
		return FALSE;
	}

	//	Note : ��ħ ������ ��� ������ ������ �������� �̹� ��ϵǾ� �ִ��� �˻�.
	if ( pITEM->ISPILE() )
	{
		bool bREG = m_pPrivateMarket->IsRegItem ( nidITEM );
		if ( bREG )
		{
			NetMsgFB.emFB = EMPMARKET_REGITEM_FB_REGITEM;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}

		//	Note : �����ϰ� �ִ� ���� ��ŭ �Ǹ� ����.
		DWORD dwTURN = m_cInventory.CountTurnItem ( nidITEM );
		if ( dwNum >= dwTURN )
		{
			dwNum = dwTURN;
		}
	}

	if ( !pITEM->ISPILE() )
	{
		dwNum = 1;
	}

	SNATIVEID sSALEPOS;
	bool bPOS = m_pPrivateMarket->FindInsertPos( nidITEM, sSALEPOS );
	if ( !bPOS )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_MAXNUM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// Add Search Item Data
	bool bREG;
	const SNATIVEID& _mapID( m_mapID.getGaeaMapID() );
	if ( _mapID.wMainID == 22 && _mapID.wSubID == 0 )
	{
		bREG = m_pPrivateMarket->RegItem( *pINVENITEM, pNetMsg->llMoney, dwNum, sSALEPOS, TRUE );
	}else{
		bREG = m_pPrivateMarket->RegItem( *pINVENITEM, pNetMsg->llMoney, dwNum, sSALEPOS, FALSE );
	}

	if ( !bREG )
	{
		NetMsgFB.emFB = EMPMARKET_REGITEM_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	NetMsgFB.emFB = EMPMARKET_REGITEM_FB_OK;
	NetMsgFB.wPosX = pNetMsg->wPosX;
	NetMsgFB.wPosY = pNetMsg->wPosY;
	NetMsgFB.llMoney = pNetMsg->llMoney;
	NetMsgFB.dwNum = dwNum;
	NetMsgFB.sSALEPOS = sSALEPOS;
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgPMarketDisItem(NET_MSG_GENERIC* nmg)
{
	if ( m_pPrivateMarket->IsOpen() )
		return S_FALSE;

	GLMSG::SNETPC_PMARKET_DISITEM *pNetMsg = (GLMSG::SNETPC_PMARKET_DISITEM *) nmg;

	const SSALEITEM* pSALEITEM = m_pPrivateMarket->GetItem( pNetMsg->sSALEPOS );
	if ( !pSALEITEM )
		return S_FALSE;

	// Add Search Item Data
	const SNATIVEID& _mapID(m_mapID.getGaeaMapID());
	if ( _mapID.wMainID == 22 && _mapID.wSubID == 0 )
	{
		m_pPrivateMarket->DisItem ( pNetMsg->sSALEPOS , TRUE );
	}else{
		m_pPrivateMarket->DisItem ( pNetMsg->sSALEPOS , FALSE );
	}
	
	GLMSG::SNETPC_PMARKET_DISITEM_FB NetMsgFB;
	NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgPMarketOpen(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PMARKET_OPEN *pNetMsg = (GLMSG::SNETPC_PMARKET_OPEN *) nmg;
	GLMSG::SNETPC_PMARKET_OPEN_FB	NetMsgFB;

	//// ���� ���� �����̴°� ����
	//if ( !IsACTION ( GLAT_IDLE ) )
	//{
	//	NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
	//	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
	//	return S_FALSE;
	//}

	if ( m_pPrivateMarket->IsOpen() )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
		return S_FALSE;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pITEM->sBasicOp.emItemType != ITEM_PRIVATEMARKET )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return S_FALSE;
	}

	if ( m_pPrivateMarket->GetItemNum() == 0 )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_EMPTY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	bool bOPEN = m_pPrivateMarket->DoMarketOpen();
	if ( !bOPEN )
	{
		NetMsgFB.emFB = EMPMARKET_OPEN_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	[�ڽſ���] �ι� ������ �Ҹ��Ŵ.
	ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY );

	//	Note : �ڱ� �ڽſ���.
	NetMsgFB.emFB = EMPMARKET_OPEN_FB_OK;
	SendToClient( &NetMsgFB );

	//	Note : �ֺ� ��ο���.
	GLMSG::SNETPC_PMARKET_OPEN_BRD NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	NetMsgBRD.SetTitle(m_pPrivateMarket->GetTitle());
	SendMsgViewAround(&NetMsgBRD);

	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_MARKET_GRANT) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	const SNATIVEID& _gaeaMapID(m_mapID.getGaeaMapID());
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		// Add Search Item Data
		// ���� ������ �п� ���忡�� �����ٸ� ������ �˻� ����Ʈ�� �߰��Ѵ�.		
		if ( _gaeaMapID.wMainID == 22 && _gaeaMapID.wSubID == 0 )
		{
			m_pGaeaServer->InsertSearchShop( m_dwGaeaID );
		}
	}
	else
	{
		// �Ǹ��� ���(�̹� ��ϵǾ� �ִٸ� update �ȴ�.)
		GLMSG::SNETPC_PMARKET_SELLER_REGISTER_FH NetMsgFH;
		NetMsgFH.dwChaDbNum = GetCharID();
		NetMsgFH.sMapID = _gaeaMapID;
		NetMsgFH.wSchool = GetSchool();
		NetMsgFH.nChannel = GetServerChannel();
		NetMsgFH.nServerGroup = m_pGaeaServer->GetServerGroup();
		NetMsgFH.SetName( GetName() );
		SendToIntegrationCache( &NetMsgFH );

		// ������ ���
		GLPrivateMarket::MAPITEM& mapItemList = m_pPrivateMarket->GetItemList();
		GLPrivateMarket::MAPITEM_ITER iterPos = mapItemList.begin();
		GLPrivateMarket::MAPITEM_ITER iterEnd = mapItemList.end();

		GLLandMan *pLandMan = m_pGaeaServer->GetLand(_gaeaMapID);

		for ( ; iterPos != iterEnd; ++iterPos )
		{
			const SSALEITEM &sSALEITEM = ( *iterPos ).second;

			GLMSG::SNETPC_PMARKET_ITEM_REGISTER_FH NetMsgFH;
			NetMsgFH.dwChaDbNum = GetCharID();
			NetMsgFH.dwItemTurnNum = sSALEITEM.dwNUMBER;
			NetMsgFH.llItemPrice = sSALEITEM.llPRICE;
			NetMsgFH.sItemcustom = sSALEITEM.sITEMCUSTOM;
			NetMsgFH.sItemGUID = sSALEITEM.sGUID;
			NetMsgFH.fCommissionRate = pLandMan->GetCommissionRate();
			NetMsgFH.nServerGroup = m_pGaeaServer->GetServerGroup();

			SendToIntegrationCache( &NetMsgFH );
		}
	}

	SetMovement( CHAR_MOVEMENT_STATE_PRIVATE_MARKET, false );

	return S_OK;
}

HRESULT GLChar::MsgPMarketClose(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PMARKET_CLOSE *pNetMsg = (GLMSG::SNETPC_PMARKET_CLOSE *) nmg;

	m_pPrivateMarket->DoMarketClose();

	//	�ֺ����� ���λ��� ���� �˸�.
	GLMSG::SNETPC_PMARKET_CLOSE_BRD	NetMsgBRD;
	NetMsgBRD.dwGaeaID = m_dwGaeaID;
	SendMsgViewAround(&NetMsgBRD);

	//	�ڽſ��� ���λ��� ���� �˸�.
	SendToClient(&NetMsgBRD );

	// Add Search Item Data
	// ���� ������ �п� ���忡�� �����ٰ� �ݾҴٸ� ������ �˻� ����Ʈ���� �����Ѵ�.
	const SNATIVEID& _mapID(m_mapID.getGaeaMapID());
	if ( _mapID.wMainID == 22 && _mapID.wSubID == 0 )
	{
		m_pGaeaServer->EraseSearchShop( m_dwGaeaID );
	}

	if ( GLHIDESET::CheckHideSet(EMHIDE_STORE_CLOSE) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		// cache �� ���λ��� �ݴ´ٴ� �޽��� ����
		GLMSG::SNETPC_PMARKET_CLOSE_FH NetMsgFH;
		NetMsgFH.dwChaDbNum = CharDbNum();
		NetMsgFH.nServerGroup = m_pGaeaServer->GetServerGroup();
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( &NetMsgFH );
	}
	SetMovement( CHAR_MOVEMENT_STATE_PRIVATE_MARKET, true );

	return S_OK;
}

void GLChar::ReqNetMsg_PMarketItem ( DWORD dwClientID )
{
	GLMSG::SNETPC_PMARKET_ITEM_INFO_BRD NetMsgBrd;
	GLPrivateMarket::MAPITEM& mapITEM = m_pPrivateMarket->GetItemList();
	GLPrivateMarket::MAPITEM_ITER pos = mapITEM.begin();
	GLPrivateMarket::MAPITEM_ITER end = mapITEM.end();
	for ( ; pos!=end; ++pos )
	{
		const SSALEITEM &sSALEITEM = (*pos).second;

		
		NetMsgBrd.dwGaeaID = m_dwGaeaID;
		NetMsgBrd.sSALEPOS = sSALEITEM.sSALEPOS;
		NetMsgBrd.sINVENPOS = sSALEITEM.sINVENPOS;
		NetMsgBrd.sITEMCUSTOM = sSALEITEM.sITEMCUSTOM;
		NetMsgBrd.llPRICE = sSALEITEM.llPRICE;
		NetMsgBrd.dwNUMBER = sSALEITEM.dwNUMBER;
		NetMsgBrd.bSOLD = sSALEITEM.bSOLD;
		SendToClient(dwClientID, &NetMsgBrd);

		// ������ �������� ��ī���̸� ��ī�� ���� ��û
		const SITEM* pItem = GLogicData::GetInstance().GetItem( sSALEITEM.sITEMCUSTOM.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			GLPetField* pPetInfo = new GLPetField(m_pGaeaServer);			

			// DB�۾� ���н� Ȥ�� ������ �޸� ���������
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetGet(
                        pPetInfo,
                        sSALEITEM.sITEMCUSTOM.PetDbNum(),
                        dwClientID,
                        m_CharDbNum, 
                        0,
                        0,
                        false,
                        false,
                        true)));		
		}
	}
}

HRESULT GLChar::MsgPMarketItemInfo(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PMARKET_ITEM_INFO *pNetMsg = (GLMSG::SNETPC_PMARKET_ITEM_INFO *) nmg;
	
	GLChar* pCHAR = GetChar(pNetMsg->dwGaeaID);
	if (!pCHAR)
        return S_FALSE;

	pCHAR->ReqNetMsg_PMarketItem(m_ClientSlot);

	return S_OK;
}

HRESULT GLChar::MsgPMarketBuy( NET_MSG_GENERIC* nmg )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GLMSG::SNETPC_PMARKET_BUY* pNetMsg = ( GLMSG::SNETPC_PMARKET_BUY* ) nmg;
	GLMSG::SNETPC_PMARKET_BUY_FB NetMsgFB;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgPMarketBuy", m_CharDbNum ) );
	}

	GLChar* pSeller = GetChar( pNetMsg->dwGaeaID );
	if ( NULL == pSeller )
	{
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : ���λ����� ���»������� �˻�.
	if ( false == pSeller->m_pPrivateMarket->IsOpen() )
	{
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : �Ǹ����� ��ǰ ����.
	const SSALEITEM* pSALEITEM = pSeller->m_pPrivateMarket->GetItem( pNetMsg->sSALEPOS );
	if ( NULL == pSALEITEM )
	{
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : �Ǹ����� ��ǰ�� �̹� �ǸſϷ����� �˻�.
	if ( pSALEITEM->bSOLD )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_SOLD;
		NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
		NetMsgFB.dwNum = 0;
		NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pSALEITEM->sITEMCUSTOM.GetNativeID() );
	if ( NULL == pITEM )
		return S_FALSE;

	// ���Ѻ� ������
	if ( pSALEITEM->sITEMCUSTOM.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL )  )
	{
		CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
		CTime cTIME_LMT( pSALEITEM->sITEMCUSTOM.tBORNTIME );
		cTIME_LMT += cSPAN + pSALEITEM->sITEMCUSTOM.GetPeriodExBasic();

		const CTime cTIME_CUR = CTime::GetCurrentTime();

		if ( cTIME_CUR > cTIME_LMT )
		{
			NetMsgFB.emFB = EMPMARKET_BUY_FB_NOTIME;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}
	}

	// �ڽ�Ƭ
	if ( SNATIVEID( false ) != pSALEITEM->sITEMCUSTOM.nidDISGUISE )
	{
		if ( 0 != pSALEITEM->sITEMCUSTOM.tDISGUISE )
		{
			CTime cTIME_LMT( pSALEITEM->sITEMCUSTOM.tDISGUISE + pSALEITEM->sITEMCUSTOM.GetPeriodExDisguise());
			if ( 1970 != cTIME_LMT.GetYear() )
			{
				const CTime cTIME_CUR = CTime::GetCurrentTime();

				if ( cTIME_CUR > cTIME_LMT )
				{
					NetMsgFB.emFB = EMPMARKET_BUY_FB_NOTIME;
					SendToClient( &NetMsgFB );
					return S_FALSE;
				}
			}
		}
	}

	DWORD dwNum = pNetMsg->dwNum;
	if ( false == pITEM->ISPILE() )
	{
		dwNum = 1;
	}

	//	Note : ������ ���� ��ŭ ���� �ִ��� �˻�.
	if ( pSALEITEM->dwNUMBER < dwNum )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_NUM;
		NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
		NetMsgFB.dwNum = pSALEITEM->dwNUMBER;
		NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	//	Note : �ڱ��� ������� �˻�.
	if ( GetInvenMoney() < ( dwNum * pSALEITEM->llPRICE ) )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_LOWMONEY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	BOOL bITEM_SPACE( FALSE );
	if ( pITEM->ISPILE() )
	{
		WORD wPILENUM = pITEM->sDrugOp.wPileNum;
		SNATIVEID sNID = pITEM->sBasicOp.sNativeID;
		WORD wREQINSRTNUM = static_cast< WORD >( dwNum );

		bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM );
	}
	else
	{
		WORD wPosX( 0 ), wPosY( 0 );
		bITEM_SPACE = m_cInventory.FindInsertable( wPosX, wPosY );
	}

	if ( FALSE == bITEM_SPACE )
	{
		NetMsgFB.emFB = EMPMARKET_BUY_FB_NOINVEN;
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//	Note : ���λ����� ��ϵ� �������̶� ������ �κ��丮�� �ִ� �������� ��ġ�ϴ��� �˻�.
	//
	BOOL bVALID_ITEM( FALSE );
	if ( pITEM->ISPILE() )
	{
		DWORD dwTURN = pSeller->m_cInventory.CountTurnItem( pSALEITEM->sITEMCUSTOM.GetNativeID() );
		if ( dwTURN >= dwNum )
            bVALID_ITEM = TRUE;
	}
	else
	{
		SINVENITEM* pINVENITEM = pSeller->m_cInventory.GetItem( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID );
		if ( pINVENITEM )
		{
			if ( pINVENITEM->sItemCustom == pSALEITEM->sITEMCUSTOM )
			{
				bVALID_ITEM = TRUE;
			}
		}
	}

	if ( FALSE == bVALID_ITEM )
	{
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// Add Search Item Data
	//	Note : ���� ������ ���� ����.
	bool bSALE = false;
	const SNATIVEID& _mapID(m_mapID.getGaeaMapID());
	if ( _mapID.wMainID == 22 && _mapID.wSubID == 0 )
	{
		bSALE = pSeller->m_pPrivateMarket->DoSale ( pNetMsg->sSALEPOS, static_cast< WORD >( dwNum ), TRUE );
	}
    else
    {
		bSALE = pSeller->m_pPrivateMarket->DoSale ( pNetMsg->sSALEPOS, static_cast< WORD >( dwNum ), FALSE );
	}
	
    if ( false == bSALE )
	{
		SendToClient( &NetMsgFB );
		return E_FAIL;
	}

	//	Note : �ŷ� ����.
	if ( false == pITEM->ISPILE() )
	{
		WORD wPosX( 0 ), wPosY( 0 );
		BOOL bFOUND = m_cInventory.FindInsertable( wPosX, wPosY );
		if ( FALSE == bFOUND )
		{
			GASSERT( 0 && "GLChar::MsgPMarketBuy() critical error FindInsertable." );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		// ��񿡼� �������� �ʰ� �޸𸮿����� �����ϰ� �ؿ��� ������ ������ �����Ѵ�.
		if ( false == pSeller->InvenDeleteItem( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID, true, FALSE, DELETE_INVEN_ITEM_PMARKET_SELL, false ) )
		{
			GASSERT( 0 && "GLChar::MsgPMarketBuy() critical error InvenDeleteItem." );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		// ��� insert �ؾ� �ϴ� �������� �ƴϴ�.  �������� ���¶�� pSALEITEM->sITEMCUSTOM �� ������ ���´� db::DB_UPDATE �̾�� �Ѵ�. 
		BOOL bINSERT = InvenInsertItem( pSALEITEM->sITEMCUSTOM, wPosX, wPosY, true, 
			INSERT_INVEN_ITEM_PMARKET_BUY, true, false, true );
		if ( FALSE == bINSERT )
		{
			GASSERT( 0 && "GLChar::MsgPMarketBuy() critical error InvenInsertItem." );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}
	}
	else
	{
		// ��ħ �������� �������ְ�, �ٽ� �־��ִ� ������� �Ѵ�.
		bool bDELETE = pSeller->DeletePileItem( pSALEITEM->sITEMCUSTOM.GetNativeID(), static_cast< WORD >( dwNum ) );
		if ( false == bDELETE )
		{
			GASSERT( 0 && "GLChar::MsgPMarketBuy() critical error DeletePileItem." );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}

		//	Note : �������� �κ��� �־��ش�.
		// #item ������ �α״� ���߿� ��������.
		std::vector< UPDATED_ITEM > vecInsertedItems;
		bool bINSERT = InsertPileItem( pSALEITEM->sITEMCUSTOM, static_cast< WORD >( dwNum ), vecInsertedItems, true );
		if ( false == bINSERT )
		{
			GASSERT( 0 && "GLChar::MsgPMarketBuy() critical error InsertPileItem." );
			SendToClient( &NetMsgFB );
			return E_FAIL;
		}
	}

	if ( pSALEITEM )
	{
		GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = pSeller->m_dwGaeaID;

		NetMsgBrd.sSALEPOS = pNetMsg->sSALEPOS;
		NetMsgBrd.dwNUMBER = pSALEITEM->dwNUMBER;
		NetMsgBrd.bSOLD = pSALEITEM->bSOLD;

		SendToClient( &NetMsgBrd );
		pSeller->SendToClient( &NetMsgBrd );
		pSeller->SendMsgViewAround( &NetMsgBrd );
	}

	//	Note : ����, �Ǹ� ���� FB.
	NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
	NetMsgFB.emFB = EMPMARKET_BUY_FB_OK;
	NetMsgFB.sSALEPOS = pNetMsg->sSALEPOS;
	NetMsgFB.dwNum = pNetMsg->dwNum;
	SendToClient( &NetMsgFB );
	pSeller->SendToClient( &NetMsgFB );

	//	Note : �� ����.
	volatile LONGLONG lnPRICE = dwNum * pSALEITEM->llPRICE;

	CheckMoneyUpdate( GetInvenMoney(), lnPRICE, FALSE, "Buy From PC." );

	SubtractInvenMoneyServer( lnPRICE, SUBTRACT_INVEN_MONEY_ITEM_BUY_PRIVATE_MARKET_DIRECT );

	volatile float fCommissionRate = pLand->GetCommissionRate();
	volatile float fCommissionRate_C = ( 100.0f - fCommissionRate ) * 0.01f;
	volatile LONGLONG lnGIVE = LONGLONG( DOUBLE( lnPRICE ) * fCommissionRate_C );

	if ( private_market::GLPrivateMarketDefine::Instance()->GetSaleCommission() )
	{
        volatile LONGLONG lnCommission = lnPRICE-lnGIVE;

		if ( CLUB_NULL != pLand->m_dwGuidClubID )
		{
			GLGuidance* pGuid = pLand->m_pGuidance;
			if ( pGuid && false == pGuid->m_bBattle )
			{
                //	Note : Ŭ���� ���� �߻�.
                std::tr1::shared_ptr< GLClubField > pCLUB = GetClub( pLand->m_dwGuidClubID );
				if ( pCLUB )
				{
					pCLUB->AddProfitMoeny( lnCommission );

                    // Ŭ�� ������ 1�� �̻��� ��� �α׸� �����.
                    if ( lnCommission > EMCLUBINCOMEMONEY_LOG )
                    {
                        // ��� �� �α� ����
                        LogMoneyExchange(
                            gamelog::ID_CHAR,
                            pSeller->m_CharDbNum, 
                            gamelog::ID_CLUB,
                            pCLUB->m_DbNum,
                            lnCommission,
                            MONEY_ROUTE_CLUB_INCOME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION );
                    }
				}
			}
		}
        pLand->AddBuyCommissionMoney( lnCommission );
	}
	else
	{
		// �����ᰡ ����. �� ������ �����ᰡ �ִ��� ���� ������ �켱�Ѵ�.
		lnGIVE = lnPRICE;
	}

	pSeller->CheckMoneyUpdate( pSeller->GetInvenMoney(), lnGIVE, TRUE, "Sale To PC." );

	pSeller->AddInvenMoneyServer( lnGIVE, ADD_INVEN_MONEY_ITEM_SELL_TO_PRIVATE_MARKET_DIRECT );

	// �������� ���� ���� ��� ���.
	// ������� ���� ������ �ŷ��� ���� ������ ������û���� ���ؼ� ������ ��ħ ������� ������ �α׸� ����� ����    
	// ��� �� �α� ����
	LogItemExchange(
        pSALEITEM->sITEMCUSTOM,
        gamelog::ID_CHAR, pSeller->m_CharDbNum,
        gamelog::ID_CHAR, m_CharDbNum,
        ITEM_ROUTE_CHAR, dwNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ),SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_PRIVATE_MARKET_BUY_ITEM,
		false );

	//	Note : �� ���.
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, pSeller->m_CharDbNum, lnGIVE, ITEM_ROUTE_CHAR, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_PRIVATE_MARKET_BUY_ITEM );
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, pSeller->m_CharDbNum, gamelog::ID_CHAR, 0, pSeller->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap() , LOG_MONEY_CHAR_INVEN_CURRENT );

	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount( m_szUID );

		CString strTemp;
		strTemp.Format(
			"Buy From PC, [%s][%s], PC Have Money:[%I64d], MarketPC[%s][%s], MarketPC Have Money:[%I64d], Buy price:[%I64d] ",
			m_szUID,
			m_szName,
			GetInvenMoney(),
			pSeller->m_szUID,
			pSeller->m_szName,
			GetInvenMoney(),
			lnGIVE );

		TracingMsg.SetLog( strTemp.GetString() );

		SendToAgent( &TracingMsg );
	}*/

	// ������ �κ� �ݾ� ��ȭ.
	// �ݾ� ��ȭ Ŭ���̾�Ʈ�� �˷���.
	SendInvenMoneyUpdate();

	// �Ǹ��� �κ� �ݾ� ��ȭ.
	// �ݾ� ��ȭ Ŭ���̾�Ʈ�� �˷���.	
	pSeller->SendInvenMoneyUpdate();

	if ( true == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		// cache �� ���λ��� �Ǹŵ� ������ ���� ����
		GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH NetMsgFH;
		NetMsgFH.dwItemTurnNum = pSALEITEM->dwNUMBER;
		NetMsgFH.sItemGUID = pSALEITEM->sGUID;
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( &NetMsgFH );
	}

	return S_OK;
}

int GLChar::MsgPrivateMarketItemBuyHF( NET_MSG_GENERIC* nmg )
{
	// ����� this �� �Ǹ����̴�. ����(���λ��� �˻��ý������� �����ϴ� ��� ����� �´�.)
	GLMSG::SNETPC_PMARKET_ITEM_BUY_HF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_HF* )nmg;
	GLMSG::SNETPC_PMARKET_BUY_FB NetMsgFB;
	EMPMARKET_BUY_FB emFB = EMPMARKET_BUY_FB_FAIL;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar ) 
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgPrivateMarketItemBuyHF", m_CharDbNum ) );
	}

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return emFB;

	//	Note : ���λ����� ���»������� �˻�.
	if ( false == m_pPrivateMarket->IsOpen() )
	{
		return emFB;
	}

	//	Note : �Ǹ����� ��ǰ ����.
	SSALEITEM *pSALEITEM = m_pPrivateMarket->GetItem( pPacket->sItemGUID );
	if ( NULL == pSALEITEM )
	{
		return emFB;
	}

	//	Note : �Ǹ����� ��ǰ�� �̹� �ǸſϷ����� �˻�.
	if ( pSALEITEM->bSOLD )
	{
		emFB = EMPMARKET_BUY_FB_SOLD;
		return emFB;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pSALEITEM->sITEMCUSTOM.GetNativeID() );
	if ( NULL == pITEM )
	{
		return emFB;
	}

	// ���Ѻ� ������
	if ( pSALEITEM->sITEMCUSTOM.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
	{
		CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
		CTime cTIME_LMT( pSALEITEM->sITEMCUSTOM.tBORNTIME );
		cTIME_LMT += cSPAN + pSALEITEM->sITEMCUSTOM.GetPeriodExBasic();

		const CTime cTIME_CUR = CTime::GetCurrentTime();

		if ( cTIME_CUR > cTIME_LMT )
		{
			emFB = EMPMARKET_BUY_FB_NOTIME;
			return emFB;
		}
	}

	// �ڽ�Ƭ
	if ( SNATIVEID( false ) != pSALEITEM->sITEMCUSTOM.nidDISGUISE )
	{
		if ( 0 != pSALEITEM->sITEMCUSTOM.tDISGUISE )
		{
			CTime cTIME_LMT( pSALEITEM->sITEMCUSTOM.tDISGUISE + pSALEITEM->sITEMCUSTOM.GetPeriodExDisguise() );
			if ( 1970 != cTIME_LMT.GetYear() )
			{
				const CTime cTIME_CUR = CTime::GetCurrentTime();

				if ( cTIME_CUR > cTIME_LMT )
				{
					emFB = EMPMARKET_BUY_FB_NOTIME;
					return emFB;
				}
			}
		}
	}

	DWORD dwNum = pPacket->dwItemTurnNum;
	if ( false == pITEM->ISPILE() )
	{
		dwNum = 1;
	}

	//	Note : ������ ���� ��ŭ ���� �ִ��� �˻�.
	if ( pSALEITEM->dwNUMBER < dwNum )
	{
		emFB = EMPMARKET_BUY_FB_NUM;
		return emFB;
	}

	//	Note : ���λ����� ��ϵ� �������̶� ������ �κ��丮�� �ִ� �������� ��ġ�ϴ��� �˻�.
	//
	BOOL bVALID_ITEM( FALSE );
	if ( pITEM->ISPILE() )
	{
		DWORD dwTURN = m_cInventory.CountTurnItem( pSALEITEM->sITEMCUSTOM.GetNativeID() );
		if ( dwTURN >= dwNum )
            bVALID_ITEM = TRUE;
	}
	else
	{
		SINVENITEM *pINVENITEM = m_cInventory.GetItem( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID );
		if ( pINVENITEM )
		{
			if ( pINVENITEM->sItemCustom == pSALEITEM->sITEMCUSTOM )
			{
				bVALID_ITEM = TRUE;
			}
		}
	}

	if ( FALSE == bVALID_ITEM )
	{
		return emFB;
	}

	if ( LONGLONG( pPacket->llItemPrice * pPacket->dwItemTurnNum ) != LONGLONG( pSALEITEM->llPRICE * dwNum ) )
	{
		sc::writeLogError( sc::string::format( 
			"MsgPrivateMarketItemBuyHF money error buyer chanum/price/turn %1%/%2%/%3% seller chanum/price/turn %4%/%5%/%6%", 
			pPacket->dwBuyerChaDbNum, pPacket->llItemPrice, pPacket->dwItemTurnNum, m_CharDbNum, pSALEITEM->llPRICE, dwNum ) );
		return emFB;
	}

	// Add Search Item Data
	//	Note : ���� ������ ���� ����.
	bool bSALE = m_pPrivateMarket->DoSale( pSALEITEM->sSALEPOS, static_cast< WORD >( dwNum ), TRUE );
    if ( false == bSALE )
	{
		return emFB;
	}

	//	Note : �ŷ� ����.
	if ( false == pITEM->ISPILE() )
	{
		/*if ( false == InvenDeleteItem( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID, true, FALSE, 79, true ) )*/
		if ( false == InvenCompletelyDeleteItem( pSALEITEM->sINVENPOS.wMainID, pSALEITEM->sINVENPOS.wSubID, true, FALSE, 79 ) )
		{
			GASSERT( 0 && "GLChar::MsgPrivateMarketItemBuyHF() critical error InvenDeleteItem." );
			return emFB;
		}
	}
	else
	{
		bool bDELETE = DeletePileItem ( pSALEITEM->sITEMCUSTOM.GetNativeID(), static_cast< WORD >( dwNum ) );
		if ( false == bDELETE )
		{
			GASSERT( 0 && "GLChar::MsgPrivateMarketItemBuyHF() critical error DeletePileItem." );
			return emFB;
		}
	}

	// �������� ���� ������ ���� ����
	pSALEITEM->sITEMCUSTOM.wTurnNum = static_cast< WORD >( dwNum );

	// �������� �����۳־��ش�.
	std::string strRecverName(pPacket->szBuyerName);
	std::string strSellerName;
	strSellerName = sc::string::format( "[PM]%s", GetName());
	SITEMCUSTOM AttachItem( pSALEITEM->sITEMCUSTOM );

	if ( pITEM->ISPILE() )
	{
		// ��ħ ������ �������� ��� guid �� �ű� �������ش�. �ƴϸ� ���� guid ���� ��� �������� ���� �ȴ�.
		sc::SeqUniqueGUID::Instance()->getGUID( AttachItem.guid );
		AttachItem.GenerateBasicStat( false );
		AttachItem.GenerateLinkSkill();
		AttachItem.GenerateAddOption();
	}

	// ������ �޶� ������ ���Ű� ���������鼭 ���⿡�� ���� ������ �ʴ´�.
	/*
	BOOL bResult = m_pGaeaServer->PostSendSystemMail(
		pPacket->dwBuyerChaDbNum, 
		strRecverName, 
		AttachItem, 
		0, 
		ATTENDANCE_REWARD_POST_DESIGN, 
		pPacket->dwSellerChaDbNum, 
		strSellerName, 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_BUY_POST_TITLE" ) ), 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_BUY_POST_CONTENT" ) ), 
		false
		);
	
	if ( FALSE == bResult )
	{
		// critical error
		GASSERT( 0 && "GLChar::MsgPrivateMarketItemBuyHF() critical error PostSendSystemMail." );
		return emFB;
	}
	*/

	if ( pSALEITEM )
	{
		GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID = m_dwGaeaID;

		NetMsgBrd.sSALEPOS = pSALEITEM->sSALEPOS;
		NetMsgBrd.dwNUMBER = pSALEITEM->dwNUMBER;
		NetMsgBrd.bSOLD = pSALEITEM->bSOLD;

		SendToClient( &NetMsgBrd );
		SendMsgViewAround( &NetMsgBrd );
	}

	//	Note : ����, �Ǹ� ���� FB.
	NetMsgFB.dwGaeaID = m_dwGaeaID;
	NetMsgFB.emFB = EMPMARKET_BUY_FB_OK;
	NetMsgFB.sSALEPOS = pSALEITEM->sSALEPOS;
	NetMsgFB.dwNum = dwNum;
	SendToClient( &NetMsgFB );

	//	Note : �� ����.
	volatile LONGLONG lnPRICE = dwNum * pSALEITEM->llPRICE;

	volatile float fCommissionRate = pLand->GetCommissionRate();
	volatile float fCommissionRate_C = ( 100.0f - fCommissionRate ) * 0.01f;
	volatile LONGLONG lnGIVE = LONGLONG( DOUBLE( lnPRICE ) * fCommissionRate_C );

	if ( private_market::GLPrivateMarketDefine::Instance()->GetSaleCommission() )
	{
        volatile LONGLONG lnCommission = lnPRICE - lnGIVE;
		if ( CLUB_NULL != pLand->m_dwGuidClubID )
		{
			GLGuidance* pGuid = pLand->m_pGuidance;
			if ( pGuid && false == pGuid->m_bBattle )
			{
				//	Note : Ŭ���� ���� �߻�.
				std::tr1::shared_ptr< GLClubField > pCLUB = GetClub( pLand->m_dwGuidClubID );
				if ( pCLUB )
				{
					pCLUB->AddProfitMoeny( lnCommission );

					// Ŭ�� ������ 1�� �̻��� ��� �α׸� �����.
					if ( lnCommission > EMCLUBINCOMEMONEY_LOG )
					{
						// ��� �� �α� ����
						LogMoneyExchange(
							gamelog::ID_CHAR,
							m_CharDbNum, 
							gamelog::ID_CLUB,
							pCLUB->m_DbNum,
							lnCommission,
							MONEY_ROUTE_CLUB_INCOME, 
							( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_GUIDANCE_COMMISSION);
					}
				}
			}
		}
        pLand->AddBuyCommissionMoney(lnCommission);
	}
	else
	{
		// �����ᰡ ����. �� ������ �����ᰡ �ִ��� ���� ������ �켱�Ѵ�.
		lnGIVE = lnPRICE;
	}

	if ( lnGIVE > LONGLONG( pPacket->llItemPrice * pPacket->dwItemTurnNum ) )
	{
		sc::writeLogError( sc::string::format( 
			"MsgPrivateMarketItemBuyHF money error buyer chanum/price/turn %1%/%2%/%3% seller chanum/price/turn %4%/%5%/%6% lnGIVE %7%", 
			pPacket->dwBuyerChaDbNum, pPacket->llItemPrice, pPacket->dwItemTurnNum, m_CharDbNum, pSALEITEM->llPRICE, dwNum, lnGIVE ) );

		lnGIVE = LONGLONG( pPacket->llItemPrice * pPacket->dwItemTurnNum );
	}

	CheckMoneyUpdate( GetInvenMoney(), lnGIVE, TRUE, "Private market sell item." );

	AddInvenMoneyServer( lnGIVE, ADD_INVEN_MONEY_ITEM_SELL_TO_PRIVATE_MARKET_SEARCH );

	// �������� ���� ���� ��� ���.
	// ������� ���� ������ �ŷ��� ���� ������ ������û���� ���ؼ� ������ ��ħ ������� ������ �α׸� ����� ����    
	// ��� �� �α� ����
	LogItemExchange(
        AttachItem, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CHAR, pPacket->dwBuyerChaDbNum, 
        ITEM_ROUTE_CHAR, dwNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_PRIVATE_MARKET_BUY_ITEM,
		false );

	//	Note : �� ���.
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, pPacket->dwBuyerChaDbNum, gamelog::ID_CHAR, m_CharDbNum, lnGIVE, ITEM_ROUTE_CHAR, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_PRIVATE_MARKET_BUY_ITEM );
	
	// �������� ���� �Ӵϴ� �� �� ����. - ���λ��� �˻� �ý������� ������ ���
	// ��� �� �α� ����
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

	// NET_LOG_UPDATE_TRACINGCHAR TracingMsg �� �� ����. - ���λ��� �˻� �ý������� ������ ��� ���Ŀ� �߰�

	// �Ǹ��� �κ� �ݾ� ��ȭ.
	// �ݾ� ��ȭ Ŭ���̾�Ʈ�� �˷���.
	SendInvenMoneyUpdate();
	
	emFB = EMPMARKET_BUY_FB_OK;

	// cache �� ���λ��� �Ǹŵ� ������ ���� ����
	GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH NetMsgFH;
	NetMsgFH.dwItemTurnNum = pSALEITEM->dwNUMBER;
	NetMsgFH.sItemGUID = pSALEITEM->sGUID;
	NetMsgFH.bLog = true;
	NetMsgFH.AttachItem = AttachItem;
	NetMsgFH.SetBuyerName( pPacket->szBuyerName );
	NetMsgFH.SetSellerName( GetName() );
	m_pGaeaServer->SENDTOINTEGRATIONCACHE( &NetMsgFH );

	return emFB;
}
