#include "../pch.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Util/GLItemLimit.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLChar::InvenInsertItem( const SITEMCUSTOM& Item, bool bSendClient, int CallType, 
							  bool DbUpdate, bool Force /*= false*/, bool bNew /*= false*/ )
{
    WORD PosX = 0;
    WORD PosY = 0;
	if ( FALSE == m_cInventory.InsertItemGetPos( Item, PosX, PosY, Force ) )
		return false;

	SINVENITEM* pInsertItem = m_cInventory.GetItem( PosX, PosY );

	if ( true == DbUpdate )
	{
		if ( !pInsertItem )
		{
			sc::writeLogError( sc::string::format( "critical error InvenInsertItem1 pInsertItem is NULL chanum: %1%", m_CharDbNum ) );
			return true;
		}

		int DbState = pInsertItem->GetDbState();
		if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInsertItem->sSaveData, CallType ) ) );
		}
		else
		{
			// db::DB_DELETE 상태인데 여기로 들어오면?
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_CharDbNum, INVEN_INVEN, pInsertItem->sSaveData, CallType ) ) );
		}
		pInsertItem->SetDbState( db::DB_UPDATE, true );
	}

    if ( bSendClient && pInsertItem )
    {
		InvenInsertSend( pInsertItem, FALSE, false, bNew );
    }

    return true;
}

bool GLChar::InvenInsertItem( const SITEMCUSTOM& Item, WORD PosX, WORD PosY, bool bSendClient, int CallType, 
							  bool DbUpdate, bool Force /*= false*/, bool bNew /*= false*/ )
{
	if ( FALSE == m_cInventory.InsertItem( Item, PosX, PosY, Force ) )
		return false;

	SINVENITEM* pInsertItem = m_cInventory.GetItem( PosX, PosY );

	if ( true == DbUpdate )
	{
		if ( !pInsertItem )
		{
			sc::writeLogError( sc::string::format( "critical error InvenInsertItem2 pInsertItem is NULL chanum: %1%", m_CharDbNum ) );
			return true;
		}

		int DbState = pInsertItem->GetDbState();
		if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInsertItem->sSaveData, CallType ) ) );
		}
		else
		{
			// db::DB_DELETE 상태인데 여기로 들어오면?
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_CharDbNum, INVEN_INVEN, pInsertItem->sSaveData, CallType ) ) );
		}
		pInsertItem->SetDbState( db::DB_UPDATE, true );
	}

	if ( bSendClient && pInsertItem )
	{
		InvenInsertSend( pInsertItem, FALSE, false, bNew );
	}

	return true;
}

void GLChar::InvenUpdateItem( SINVENITEM* pInvenItem, bool JustInvenTypeUpdate /*= false*/ )
{
	if ( !pInvenItem )
	{
		sc::writeLogError( sc::string::format( "critical error InvenUpdateItem pInvenItem is NULL chanum: %1%", m_CharDbNum ) );
		return;
	}

	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 7977 ) ) );

		pInvenItem->SetDbState( db::DB_UPDATE, true );
	}
	else
	{
		if ( JustInvenTypeUpdate )
		{
			// 저장 위치 및 좌표만 변경된다.
			m_pGaeaServer->UpdateItemInvenType( m_CharDbNum, pInvenItem->sItemCustom, INVEN_INVEN, pInvenItem->wPosX, pInvenItem->wPosY );
		}
		else
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_CharDbNum, INVEN_INVEN, pInvenItem->sSaveData, 7977 ) ) );
		}
	}
}

void GLChar::InvenInsertSend( const SINVENITEM* pInsertItem, BOOL bVietnamInven /*= FALSE*/, 
							  bool bAllLine /*= false*/, bool bNew /*= false*/ )
{
	if ( pInsertItem )
	{
		GLMSG::SNETPC_INVEN_INSERT SendData( 
			*pInsertItem, 
			bVietnamInven, 
			bAllLine,
			bNew );
		msgpack::sbuffer BufferData;
		msgpack::pack( BufferData, SendData );
		SendToClient( NET_MSG_GCTRL_INVEN_INSERT, BufferData );
	}
}

bool GLChar::InvenDeleteItem( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, int CallType, bool DbUpdate )
{
	SINVENITEM* pDeleteItem = m_cInventory.GetItem( PosX, PosY );
	if ( !pDeleteItem )
	{
		return false;
	}

	sc::MGUID guid = pDeleteItem->GetGuid();
	int DbState = pDeleteItem->GetDbState();

	if ( !m_cInventory.DeleteItem( PosX, PosY ) )
	{
		return false;
	}

	if ( DbUpdate )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemDelete( guid, DbState, CallType ) ) );
	}

    if ( bSendClient )
        InvenDeleteSend( PosX, PosY, bVietnam );

	return true;
}

bool GLChar::InvenCompletelyDeleteItem( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, int CallType, bool bInsertDeletedItemTable /*= true*/ )
{
	SINVENITEM* pDeleteItem = m_cInventory.GetItem( PosX, PosY );
	if ( !pDeleteItem )
	{
		return false;
	}

	if ( bInsertDeletedItemTable )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::DeletedItemInsert( m_CharDbNum, INVEN_INVEN, pDeleteItem->sSaveData, 170217 ) ) );
	}

	sc::MGUID guid = pDeleteItem->GetGuid();
	int DbState = pDeleteItem->GetDbState();

	if ( !m_cInventory.DeleteItem( PosX, PosY ) )
	{
		return false;
	}

	AddGameAdoJob(
		db::DbActionPtr( 
		new db::CompletelyItemDelete( guid, DbState, CallType ) ) );

	if ( bSendClient )
		InvenDeleteSend( PosX, PosY, bVietnam );

	return true;
}

bool GLChar::ItemSaleToNpc( WORD PosX, WORD PosY, bool bSendClient, BOOL bVietnam, bool DbUpdate )
{
	SINVENITEM* pDeleteItem = m_cInventory.GetItem( PosX, PosY );
	if ( !pDeleteItem )
	{
		return false;
	}

	sc::MGUID guid = pDeleteItem->GetGuid();
	int DbState = pDeleteItem->GetDbState();

	if ( !m_cInventory.DeleteItem( PosX, PosY ) )
	{
		return false;
	}

	if ( DbUpdate )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemSaleToNpc( guid, DbState, DELETE_INVEN_ITEM_SELL_TO_NPC ) ) );
	}

	if ( bSendClient )
		InvenDeleteSend( PosX, PosY, bVietnam );

	return true;
}

bool GLChar::InvenCountDeleteItem( const SITEMCUSTOM& Item, int iCount, bool bSendClient, BOOL bVietnam )
{
	bool bAllDelete = true;
	for( int i = 0; i < iCount; ++ i )
	{
		SINVENITEM* pInvenItem = m_cInventory.FindItem( Item.GetNativeID() );	
		if ( pInvenItem )
		{
			if ( !InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, bSendClient, bVietnam, INVEN_COUNT_DELETE_ITEM, true ) )
				bAllDelete = false;
		}
		else
		{
			bAllDelete = false;
		}
	}

	return bAllDelete;
}

void GLChar::InvenDeleteSend( WORD PosX, WORD PosY, BOOL bVietnamInven )
{
    GLMSG::SNETPC_INVEN_DELETE Msg( PosX, PosY, bVietnamInven );
	Msg.dwChaNum = CharDbNum();
    SendToClient( &Msg );
}

SINVENITEM* GLChar::InvenGetItem( const WORD PosX, const WORD PosY )
{
	return m_cInventory.GetItem( PosX, PosY );
}

SINVENITEM* GLChar::InvenFindPosItem( const WORD wPosX, const WORD wPosY )
{
	return m_cInventory.FindPosItem( wPosX, wPosY );
}

DWORD GLChar::InvenGetAmountEmpty( bool bAllLine )
{
	return m_cInventory.GetAmountEmpty( bAllLine );
}

GLInventory& GLChar::GetInventory()
{
	return m_cInventory;
}

// 아이템 거래 로그
// 통계 및 로그 수정
void GLChar::LogItemExchange( 
    const SITEMCUSTOM& sITEM, 
    gamelog::EMIDTYPE FromType, DWORD FromDbNum,	// 출발지
    gamelog::EMIDTYPE ToType, DWORD ToDbNum,		// 목적지
    EMITEM_ROUTE RouteType, 
    int nNum,										// 거래 개수
	const MapID &sMapID,							// FromDBNum 과 ToDBNum 이 항상 있는 것이 아니므로 여기서 MapID를 확인할 수 없으므로 받아와야 함. DBNum에 클럽DBNum이 들어오는 경우도 있으므로 주의
	EMITEM_ROUTE_DETAIL emFlagDetail, 
    bool bForceLog )
{    
    m_pItemLimit->LogItemExchange( 
        sITEM, 
        FromType, FromDbNum, 
        ToType, ToDbNum, 
        RouteType, 
        nNum, 
		sMapID, 
		emFlagDetail, 
        bForceLog );
}

//! 게임머니 거래로그
// 통계 및 로그 수정
void GLChar::LogMoneyExchange(
    gamelog::EMIDTYPE FromType, DWORD FromDbNum, // 출발지
    gamelog::EMIDTYPE ToType, DWORD ToDbNum, // 목적지
    LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail)
{    
    m_pItemLimit->LogMoneyExchange(
        FromType, FromDbNum,
        ToType, ToDbNum,
        lnPrice, Flag, sMapID, emFlagDetail);
}

void GLChar::LogMoneyExchangeWithItemGuid(
	gamelog::EMIDTYPE FromType, DWORD FromDbNum, 
	gamelog::EMIDTYPE ToType, DWORD ToDbNum, 
	LONGLONG lnPrice, EMITEM_ROUTE Flag, const MapID &sMapID, EMITEM_ROUTE_DETAIL emFlagDetail, sc::MGUID& guid )
{
	m_pItemLimit->LogMoneyExchangeWithItemGuid( 
		FromType, FromDbNum,
		ToType, ToDbNum,
		lnPrice, Flag, sMapID, emFlagDetail, guid );
}

//! 아이템 변환로그
void GLChar::LogItemConversion(
    const SITEMCUSTOM& sITEM,
    gamelog::EMIDTYPE FromType,
    DWORD FromDbNum,
    EMITEM_ROUTE emFlag, EMITEM_ROUTE_DETAIL emFlagDetail)
{    
    m_pItemLimit->LogItemConversion(sITEM, FromType, FromDbNum, emFlag, emFlagDetail);
}

//! 아이템 랜덤옵션 로그
void GLChar::LogItemRandomOption(const SITEMCUSTOM& sITEM)
{    
    m_pItemLimit->LogItemRandomOption(sITEM);
}

void GLChar::LogItemCostumeState(int iCharDbNum, const SITEMCUSTOM& sITEM)
{
	m_pItemLimit->LogItemCostumeState(iCharDbNum, sITEM);
}

void GLChar::LogItemAddonOption( const SITEMCUSTOM& sITEM )
{
	m_pItemLimit->LogItemAddonOption( sITEM );
}

void GLChar::LogItemLinkSkillOption( const SITEMCUSTOM& sITEM )
{
	m_pItemLimit->LogItemLinkSkillOption( sITEM );
}

void GLChar::LogItemBasicStatOption( const SITEMCUSTOM& sITEM )
{
	m_pItemLimit->LogItemBasicStatOption( sITEM );
}

// 통계 및 로그 수정
void GLChar::LogCharAction(
    DWORD dwCI, gamelog::EMACTION emACT, 
    gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
    __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID)
{
    m_pItemLimit->LogCharAction(
        dwCI, emACT, 
        emTAR, dwTAR, 
        nEXP, nBRIGHT, nLIFE, nMONEY, sMapID);
}

void GLChar::LogPetAction(int nPetNum, SNATIVEID nItemID, pet::EM_ACTION emAction, int nPetFull)
{
    m_pItemLimit->LogPetAction(nPetNum, nItemID, emAction, nPetFull);
}

void GLChar::LogVehicleAction(int nVehicleNum, SNATIVEID nItemID, EMVEHICLE_ACTION emAction, int nVehicleFull)
{
    m_pItemLimit->LogVehicleAction(nVehicleNum, nItemID, emAction, nVehicleFull);
}

//! 아이템 고유번호를 생성한다
// void GLChar::RegItemGen( __int64& GenNum, const SNATIVEID &sNID, EMITEMGEN emTYPE )
// {    
//     m_pItemLimit->RegItemGen( GenNum, sNID, emTYPE );
// }
// 
// void GLChar::RegItemGen( sc::MGUID& guid )
// {
// 	m_pItemLimit->RegItemGen( guid );
// }
void GLChar::SetPartyID(const GLPartyID& PartyID)
{
	ServerActor::SetPartyID(PartyID);
	if ( m_PartyID.isValidParty() )
		m_bParty = TRUE;
}

void GLChar::ResetPartyID(void)
{
	ServerActor::ResetPartyID();
	m_bParty = FALSE;
}

GLPartyField* const GLChar::GetMyParty() const
{
    return m_pGaeaServer->GetParty(GetPartyID());
}

GLPartyField* const GLChar::GetParty(const DWORD dwPartyID) const
{
    return m_pGaeaServer->GetParty(dwPartyID);
}
GLPartyField* const GLChar::GetParty(const GLPartyID& PartyID) const
{
	return m_pGaeaServer->GetParty(PartyID);
}

GLPetField* GLChar::GetMyPet()
{
    return m_pGaeaServer->GetPET(m_dwPetGUID);
}

GLPetField* GLChar::GetPet(DWORD PetId)
{
    return m_pGaeaServer->GetPET(PetId);
}

std::tr1::shared_ptr<GLClubField> GLChar::GetMyClub()
{
    return m_pGaeaServer->GetClub(m_ClubDbNum);
}

std::tr1::shared_ptr<GLClubField> GLChar::GetClub(DWORD ClubDbNum)
{
    return m_pGaeaServer->GetClub(ClubDbNum);
}

GLChar* GLChar::GetChar64(DWORD dwSvrGroupNum, DWORD dwChaNum)
{
    return m_pGaeaServer->GetChar64(dwSvrGroupNum, dwChaNum);
}

GLChar* GLChar::GetCharByDbNum(DWORD ChaDbNum)
{
	GASSERT(m_pGaeaServer->IsField());	//InstanceServer에서는 사용되면 안된다!

    return m_pGaeaServer->GetCharByDbNum(ChaDbNum);
}

GLChar* GLChar::GetChar(DWORD dwGaeaId)
{
    return m_pGaeaServer->GetChar(dwGaeaId);
}

ServerActor* GLChar::GetTarget(const STARGETID& sTargetID)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return NULL;

    return m_pGaeaServer->GetTarget(pLand, sTargetID);
}

ServerActor* GLChar::GetTarget(const GLLandMan* pLandMan, const STARGETID& sTargetID)
{
    return m_pGaeaServer->GetTarget(pLandMan, sTargetID);
}

DWORD GLChar::GetFieldSvrID() const	
{ 
    return m_pGaeaServer->GetFieldSvrID();
}

int GLChar::GetServerChannel() const
{ 
    return m_pGaeaServer->GetServerChannel();
}

SFIELDCROW* GLChar::NEW_FIELDCROW()
{
    return m_pGaeaServer->NEW_FIELDCROW();
}

void GLChar::RELEASE_FIELDCROW(SFIELDCROW* pFIELDCROW)
{
    m_pGaeaServer->RELEASE_FIELDCROW(pFIELDCROW);
}

DWORD GLChar::GetMaxClient() const
{ 
    return m_pGaeaServer->GetMaxClient();
}

bool GLChar::IsBRIGHTEVENT() const
{ 
    return m_pGaeaServer->IsBRIGHTEVENT();
}

bool GLChar::SchoolFreePkOn() const
{
    return m_pGaeaServer->SchoolFreePkOn();
}

bool GLChar::IsPKServer() const
{
    return m_pGaeaServer->IsPKServer();
}

bool GLChar::IsReserveServerStop() const 
{ 
    return m_pGaeaServer->IsReserveServerStop();
}

void GLChar::HOLD_ITEM(const SITEMCUSTOM& sItemCustom)
{
    GLCHARLOGIC::HOLD_ITEM(sItemCustom);

    GLMSG::SNETPC_PUTON_UPDATE Data;
    Data.m_Slot = SLOT_HOLD;
    Data.m_Item = sItemCustom;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, Data);

    SendToClient(NET_MSG_GCTRL_PUTON_UPDATE, SendBuffer);
}

BOOL GLChar::InvenFindInsertable( WORD& wPosX, WORD& wPosY, bool bAllLine /*= false*/ )
{
	return m_cInventory.FindInsertable( wPosX, wPosY, bAllLine );
}

bool GLChar::IsInsertToInven( const SITEMCUSTOM& Item, bool Force /*= false*/ )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( Item.GetNativeID() );
	if ( !pItem )
		return false;

	if ( pItem->ISPILE() )
	{
		//	겹침 아이템일 경우.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//	넣기 요청된 아이템수. ( 잔여량. )
		WORD wREQINSRTNUM = ( Item.wTurnNum );

		BOOL bITEM_SPACE = m_cInventory.ValidPileInsrt( wREQINSRTNUM, sNID, wPILENUM, Force );
		if ( !bITEM_SPACE )
			return false;
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY, Force );
		if ( !bOk )
			return false;
	}

	return true;
}

void GLChar::RELEASE_SLOT_ITEM_SERVER( EMSLOT _slot, bool DbUpdate /*= false*/ )
{
	const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM( _slot );
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
	{
		return;
	}
	
	sc::MGUID guid = sItemCustom.GetGuid();
	int DbState = sItemCustom.GetDbState();

	RELEASE_SLOT_ITEM( _slot );

	if ( DbUpdate )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemDelete( guid, DbState, DELETE_PUTON_ITEM_RELEASE_SLOT ) ) );
	}
}

void GLChar::SLOT_ITEM_SERVER( SITEMCUSTOM& sItemCustom, EMSLOT _slot, bool DbUpdate /*= false*/ )
{
	if ( DbUpdate )
	{
		SINVENITEM_SAVE sItem;
		sItem.sItemCustom = sItemCustom;
		sItem.wPosX = static_cast< WORD >( _slot );
		int DbState = sItemCustom.GetDbState();
		if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
		{
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemInsert( m_CharDbNum, INVEN_PUTON, sItem, INSERT_PUTON_ITEM_SLOT_ITEM_SERVER ) ) );
		}
		else
		{
			// db::DB_DELETE 상태인데 여기로 들어오면?
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_CharDbNum, INVEN_PUTON, sItem, UPDATE_PUTON_ITEM_SLOT_ITEM_SERVER ) ) );
		}
		sItemCustom.SetDbState( db::DB_UPDATE, true );
	}

	SLOT_ITEM( sItemCustom, _slot );
}

void GLChar::SlotUpdateItem( SITEMCUSTOM& sItemCustom, EMSLOT _slot, bool JustInvenTypeUpdate /*= false*/ )
{
	SINVENITEM_SAVE sItem;
	sItem.sItemCustom = sItemCustom;
	sItem.wPosX = static_cast< WORD >( _slot );
	int DbState = sItemCustom.GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_CharDbNum, INVEN_PUTON, sItem, UPDATE_PUTON_ITEM_SLOT_UPDATE_ITEM ) ) );
	}
	else
	{
		if ( JustInvenTypeUpdate )
		{
			// 저장 위치 및 좌표만 변경된다.
			m_pGaeaServer->UpdateItemInvenType( m_CharDbNum, sItemCustom, INVEN_PUTON, sItem.wPosX, sItem.wPosY );
		}
		else
		{
			// db::DB_DELETE 상태인데 여기로 들어오면?
			AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_CharDbNum, INVEN_PUTON, sItem, UPDATE_PUTON_ITEM_SLOT_UPDATE_ITEM ) ) );
		}
	}

	sItemCustom.SetDbState( db::DB_UPDATE, true );
}

BOOL GLChar::InvenIsInsertable( const WORD wPosX, const WORD wPosY, bool bLOAD /*= false*/ )
{
	return m_cInventory.IsInsertable( wPosX, wPosY, bLOAD );
}

bool GLChar::InvenSwapTestInvenItem( SINVENITEM* pInvenItem, SINVENITEM* pHoldInvenItem )
{
	if ( !pInvenItem || !pHoldInvenItem )
	{
		return false;
	}

	SINVENITEM sInvenBackUp = *pInvenItem;
	SINVENITEM sHoldInvenBackUp = *pHoldInvenItem;

	GLInventory CloneInventory;
	CloneInventory.Assign( m_cInventory );

	WORD wInvenPosX = sInvenBackUp.wPosX;
	WORD wInvenPosY = sInvenBackUp.wPosY;

	if ( !CloneInventory.DeleteItem( wInvenPosX, wInvenPosY ) )
	{
		return false;
	}

	if ( !CloneInventory.IsInsertable( wInvenPosX, wInvenPosY ) )
	{
		return false;
	}

	if ( !CloneInventory.InsertItem( sHoldInvenBackUp.CustomItem(), wInvenPosX, wInvenPosY ) )
	{
		return false;
	}

	SINVENITEM* pInsertedInvenItem = CloneInventory.GetItem( wInvenPosX, wInvenPosY );
	if ( NULL == pInsertedInvenItem )
	{
		return false;
	}

	WORD wHoldPosX = sHoldInvenBackUp.wPosX;
	WORD wHoldPosY = sHoldInvenBackUp.wPosY;

	if ( !CloneInventory.DeleteItem( wHoldPosX, wHoldPosY ) )
	{
		return false;
	}

	if ( !CloneInventory.IsInsertable( wHoldPosX, wHoldPosY ) )
	{
		return false;
	}

	if ( !CloneInventory.InsertItem( sInvenBackUp.CustomItem(), wHoldPosX, wHoldPosY ) )
	{
		return false;
	}

	SINVENITEM* pHeldInvenItem = CloneInventory.GetItem( wHoldPosX, wHoldPosY );
	if ( NULL == pHeldInvenItem )
	{
		return false;
	}

	return true;
}

bool GLChar::DropItem( SITEMCUSTOM& Item, const D3DXVECTOR3& Position, bool Drop /*= true*/ )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( !pLand )
		return false;

	SINVENITEM_SAVE sItem;
	sItem.sItemCustom = Item;
	int DbState = sItem.sItemCustom.GetDbState();
	if ( db::DB_NONE ==  DbState || db::DB_INSERT == DbState )
	{
		m_pGaeaServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::ItemInsert( m_CharDbNum, INVEN_DELETED, sItem, 797345 ) ) );

		sItem.sItemCustom.SetDbState( db::DB_UPDATE, true );
	}
	else
	{
		m_pGaeaServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::ItemUpdate( m_CharDbNum, INVEN_DELETED, sItem, 797345 ) ) );
	}

	if ( Drop )
	{
		if ( NULL == pLand->DropItem( Position, &( sItem.sItemCustom ), EMGROUP_ONE, m_dwGaeaID ) )
		{
			// 로그
			std::string strGuid = sc::string::uuidToString( Item.GetGuid() );
			sc::writeLogError( sc::string::format( "critical error DropItem fail. chanum: %1% guid: %2%", 
				m_CharDbNum, strGuid.c_str() ) );

			m_pGaeaServer->InsertErrorItem( sItem.sItemCustom.GetGuid(), 797345 );

			return false;
		}
	}

	return true;
}
