#include "pch.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Club/ClubAuthGrade.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/ClubMsg.h"
#include "../../RanLogic/Msg/FriendMsg.h"

#include "../AgentServer/GLAgentServer.h"
#include "./GLClubAgent.h"

#include "../Database/DBAction/DbActionGameClub.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../Util/GLItemLimit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClubAgent::GLClubAgent(GLAgentServer* pServer)
    : m_pServer(pServer)
    //, m_lnStorageMoney(0)
    //, m_lnIncomeMoney(0)
    , m_bStorageLoadFromDb(false)
    , m_bStorageUpdate(false)
    , m_ChangeClubMarkTime(0)
	, m_CurrentClubStorageUser( GAEAID_NULL )
	, m_LastClubStorageUseTime( 0 )
	, m_CurrentClubStorageMoneyUser( GAEAID_NULL )
	, m_LastClubStorageMoneyUseTime( 0 )
{
    //     for (int i=0; i<MAX_CLUBSTORAGE; ++i)
    //     {
    //         m_cStorage[i].SetState(STORAGE_INVEN_X, STORAGE_INVEN_Y);
    //     }
}

GLClubAgent::~GLClubAgent()
{    
}

const GLClubAgent& GLClubAgent::operator = (const GLClub& value)
{
    if (this == &value)
        return *this;
    GLClub::operator = (value);

    return *this;
}

const GLClubAgent& GLClubAgent::operator = (const GLClubAgent& value)
{
    if (this == &value)
        return *this;

    GLClub::operator = (value);

    m_bStorageLoadFromDb = value.m_bStorageLoadFromDb;
    //m_lnIncomeMoney = value.m_lnIncomeMoney;
    //m_lnStorageMoney = value.m_lnStorageMoney;
    m_bStorageUpdate = value.m_bStorageUpdate;
	m_ChangeClubMarkTime = value.m_ChangeClubMarkTime;
	m_CurrentClubStorageUser = value.m_CurrentClubStorageUser;
	m_CurrentClubStorageMoneyUser = value.m_CurrentClubStorageMoneyUser;

    for (int i=0; i<MAX_CLUBSTORAGE; ++i)
        m_cStorage[i].Assign(value.m_cStorage[i]);

    return *this;
}

// void GLClubAgent::SetInfo(const GLCLUBINFO& sINFO)
// {
//     GLClub::SetInfo(sINFO);
// 
//     m_lnStorageMoney = sINFO.m_lnStorageMoney;
//     m_lnIncomeMoney = sINFO.m_lnIncomeMoney;
// }

void GLClubAgent::RESET()
{
    GLClub::RESET();

    m_lnStorageMoney = 0;
    m_lnIncomeMoney = 0;
    m_bStorageLoadFromDb = false;
    m_bStorageUpdate = false;
    for (int i=0; i<MAX_CLUBSTORAGE; ++i)
        m_cStorage[i].DeleteItemAll();    
}

void GLClubAgent::RESET_STORAGE()
{
    m_bStorageLoadFromDb = false;
    m_lnIncomeMoney = 0;
    m_lnStorageMoney = 0;
    m_bStorageUpdate = false;
    for (int i=0; i<MAX_CLUBSTORAGE; ++i)
        m_cStorage[i].DeleteItemAll();
}

BOOL GLClubAgent::SETSTORAGE_BYBUF(se::ByteStream& ByteStream)
{
    if (ByteStream.IsEmpty())
        return TRUE;

    DWORD dwChannel = 0;
    ByteStream >> dwChannel;
    if (dwChannel > MAX_CLUBSTORAGE)
        dwChannel = MAX_CLUBSTORAGE;

    for (DWORD i=0; i<dwChannel; ++i)
    {
        BOOL bOk = m_cStorage[i].SETITEM_BYBUFFER(ByteStream);
        if (!bOk)
            return TRUE;
    }

    return TRUE;
}

BOOL GLClubAgent::SETSTORAGE_BYBUF_FOR_MIGRATION(se::ByteStream& ByteStream)
{
	if (ByteStream.IsEmpty())
		return TRUE;

	DWORD dwChannel = 0;
	ByteStream >> dwChannel;
	if (dwChannel > MAX_CLUBSTORAGE)
		dwChannel = MAX_CLUBSTORAGE;

	for (DWORD i=0; i<dwChannel; ++i)
	{
		BOOL bOk = m_cStorage[i].SETITEM_BYBUFFER_FOR_MIGRATION(ByteStream);
		if (!bOk)
			return TRUE;
	}

	return TRUE;
}

BOOL GLClubAgent::GETSTORAGE_BYBUF(se::ByteStream& ByteStream) const
{
    ByteStream.ClearBuffer();
    ByteStream << DWORD(MAX_CLUBSTORAGE);
    for (DWORD i=0; i<MAX_CLUBSTORAGE; ++i)
        m_cStorage[i].GETITEM_BYBUFFER_FORSTORAGE(ByteStream);
    return TRUE;
}

BOOL GLClubAgent::SETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
{
	for ( size_t loop = 0; loop < MAX_CLUBSTORAGE; loop++ )
	{
		m_cStorage[loop].DeleteItemAll();
	}

	// 캐릭터 인벤토리를 제외한 개인락커, 클럽락커는 인벤토리의 배열 구조로 되어있다.
	// 각 인벤토리의 인덱스를 디비에 저장하기 보다는 x 좌표를 이렇게 계산해서 하는 방식으로 구현하였다.
	// 0번 인벤토리 제외, 1~n 까지의 인벤토리의 아이템의 x 좌표를 수정해서 db 에 저장한다.
	// 지금 캐릭터 인벤토리처럼 구조를 변경하는게 좋을 것 같지만, 각 락커의 경우 기간제로 인벤토리를 판매하기 때문에 어려움이 있다.
	// 락커 인덱스 하드코딩이므로 인벤토리 개수 변경이 있다면 주의한다.
	GASSERT( MAX_CLUBSTORAGE == 6 );

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];
		if ( sItem.wPosX >= 0 && sItem.wPosX < EStorageX )
		{
			m_cStorage[0].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX && sItem.wPosX < EStorageX*2 )
		{
			sItem.wPosX = sItem.wPosX - EStorageX;
			m_cStorage[1].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*2 && sItem.wPosX < EStorageX*3 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*2 );
			m_cStorage[2].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*3 && sItem.wPosX < EStorageX*4 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*3 );
			m_cStorage[3].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*4 && sItem.wPosX < EStorageX*5 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*4 );
			m_cStorage[4].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*5 && sItem.wPosX < EStorageX*6 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*5 );
			m_cStorage[5].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
	}

	return TRUE;
}

BOOL GLClubAgent::GETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
{
	// 캐릭터 인벤토리를 제외한 개인락커, 클럽락커는 인벤토리의 배열 구조로 되어있다.
	// 각 인벤토리의 인덱스를 디비에 저장하기 보다는 x 좌표를 이렇게 계산해서 하는 방식으로 구현하였다.
	// 0번 인벤토리 제외, 1~n 까지의 인벤토리의 아이템의 x 좌표를 수정해서 db 에 저장한다.
	// 지금 캐릭터 인벤토리처럼 구조를 변경하는게 좋을 것 같지만, 각 락커의 경우 기간제로 인벤토리를 판매하기 때문에 어려움이 있다.
	// 락커 인덱스 하드코딩이므로 인벤토리 개수 변경이 있다면 주의한다.
	GASSERT( MAX_CLUBSTORAGE == 6 );

	for ( size_t x = 0; x < MAX_CLUBSTORAGE; x++ )
	{
		std::vector< SINVENITEM_SAVE > vecItemsTemp;
		GLInventoryStorage& Inven = m_cStorage[x];
		Inven.GET_ITEM_BY_VECTOR( vecItemsTemp );

		size_t ItemSize = vecItemsTemp.size();
		for ( size_t loop = 0; loop < ItemSize; loop++ )
		{
			SINVENITEM_SAVE& sItem = vecItemsTemp[loop];
			if ( 1 == x )
			{
				sItem.wPosX = sItem.wPosX + EStorageX;
			}
			else if ( 2 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*2 );
			}
			else if ( 3 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*3 );
			}
			else if ( 4 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*4 );
			}
			else if ( 5 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*5 );
			}
			vecItems.push_back( vecItemsTemp[loop] );
		}
	}

	return TRUE;
}

void GLClubAgent::SendClubClient(NET_MSG_GENERIC* pMsg)
{
    for (CLUBMEMBERS_CITER iter=m_mapMembers.begin(); iter!=m_mapMembers.end(); ++iter)
    {
        GLCharAG* pChar = m_pServer->GetCharByDbNum(iter->first);
        if (pChar)
            m_pServer->SENDTOCLIENT(pChar->ClientSlot(), pMsg);
    }
}

void GLClubAgent::SendClubClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        SendClubClient(&MsgPack);
}

void GLClubAgent::SetLastBattlePoint(DWORD dwClubID, WORD wKillPoint, WORD wDeathPoint)
{
    CLUB_BATTLE_DEL_ITER pos = m_mapBattleDel.find(dwClubID);
    if (pos==m_mapBattleDel.end())
        return;

    GLCLUBBATTLE_DEL * pClubBattleDel = &(*pos).second;
    pClubBattleDel->m_dwField++;
    pClubBattleDel->m_sClubBattle.m_wKillPoint += wKillPoint;
    pClubBattleDel->m_sClubBattle.m_wDeathPoint += wDeathPoint;

    DWORD dwFieldNum = m_pServer->GetFieldNum();

    if (pClubBattleDel->m_dwField >= dwFieldNum)
        pClubBattleDel->m_bIsComplated = true;
}

bool GLClubAgent::SetAuthorityMaster(DWORD dwCharID)
{
    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (!pMember)
        return false;	

	GLCharAG* pChar = m_pServer->GetCharByDbNum(dwCharID);
	if( !pChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on SetAuthorityMaster ", dwCharID));
	}

    m_MasterCharDbNum = dwCharID;
    SetMasterName(pMember->m_ChaName);
    SetMasterCharLevel(pMember->m_ChaLevel);

    //	신규 마스터 cdm권한 부여
    SetMemberCDMFlag(m_MasterCharDbNum, FALSE);

    pMember->m_Grade = 0;    

    // Flag 저장.
    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubMasterFlagsSet(m_DbNum,dwCharID, pMember->m_Grade)));    

    if (m_dwCDCertifior == dwCharID)
    {
        m_dwCDCertifior = 0; 

        m_pServer->AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubDeputySet(m_DbNum, m_dwCDCertifior)));        

        // 클럽 인증 CD 권한 설정에 대한 로그
		// 통계 및 로그 수정
        m_pServer->LogCharAction(
            m_MasterCharDbNum,		//	당사자.
            gamelog::ACT_CLUB_DEPUTY_MEMBER,	//	행위.
            gamelog::ID_CHAR, m_dwCDCertifior,	//	상대방.
            0,							//	exp
            0,							//	bright
            0,							//	life
            0,							//	money
			( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
            );
    }
    return true;
}

BOOL GLClubAgent::InsertItem(const SITEMCUSTOM& Item)
{
    WORD PosX = 0;
    WORD PosY = 0;
    for (int i=0; i<MAX_CLUBSTORAGE; ++i)
    {
        if (m_cStorage[i].InsertItemGetPos(Item, PosX, PosY))
        {
            SINVENITEM InsertedItem;
            InsertedItem.wPosX = PosX;
            InsertedItem.wPosY = PosY;
            InsertedItem.sItemCustom = Item;

            GLMSG::SNET_CLUB_STORAGE_INSERT SendData;
            SendData.dwChannel = i;
            SendData.Data = InsertedItem;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);

            SendClubClient(NET_MSG_GCTRL_CLUB_STORAGE_INSERT, SendBuffer);

            m_bStorageUpdate = true;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL GLClubAgent::InsertItem(const SITEMCUSTOM& Item, DWORD Channel, WORD PosX, WORD PosY, bool bSendClient /* = true */)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;

    if (m_cStorage[Channel].InsertItem(Item, PosX, PosY))
    {
        SINVENITEM InsertedItem;
        InsertedItem.wPosX = PosX;
        InsertedItem.wPosY = PosY;
        InsertedItem.sItemCustom = Item;

        if ( bSendClient )
        {
            GLMSG::SNET_CLUB_STORAGE_INSERT SendData;
            SendData.dwChannel = Channel;
            SendData.Data = InsertedItem;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);

            SendClubClient(NET_MSG_GCTRL_CLUB_STORAGE_INSERT, SendBuffer);
        }

        m_bStorageUpdate = true;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLClubAgent::InsertItem( SINVENITEM_SAVE& sItem )
{
	// 뭐 같지만 시간 관계상 인벤토리 배열이 아닌 1개 짜리로 변경하지 않는한 이렇게 써야 한다.
	GASSERT( MAX_CLUBSTORAGE == 6 );

	if ( sItem.wPosX >= 0 && sItem.wPosX < EStorageX )
	{
		return m_cStorage[0].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
	}
	else if ( sItem.wPosX >= EStorageX && sItem.wPosX < EStorageX*2 )
	{
		sItem.wPosX = sItem.wPosX - EStorageX;
		return m_cStorage[1].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
	}
	else if ( sItem.wPosX >= EStorageX*2 && sItem.wPosX < EStorageX*3 )
	{
		sItem.wPosX = sItem.wPosX - ( EStorageX*2 );
		return m_cStorage[2].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
	}
	else if ( sItem.wPosX >= EStorageX*3 && sItem.wPosX < EStorageX*4 )
	{
		sItem.wPosX = sItem.wPosX - ( EStorageX*3 );
		return m_cStorage[3].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
	}
	else if ( sItem.wPosX >= EStorageX*4 && sItem.wPosX < EStorageX*5 )
	{
		sItem.wPosX = sItem.wPosX - ( EStorageX*4 );
		return m_cStorage[4].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
	}
	else if ( sItem.wPosX >= EStorageX*5 && sItem.wPosX < EStorageX*6 )
	{
		sItem.wPosX = sItem.wPosX - ( EStorageX*5 );
		return m_cStorage[5].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
	}
	else
	{
		return FALSE;
	}
}

bool GLClubAgent::DeleteItem(DWORD Channel, WORD PosX, WORD PosY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return false;

    if (m_cStorage[Channel].DeleteItem(PosX, PosY))
    {
        m_bStorageUpdate = true;
        return true;
    }
    else
    {
        return false;
    }
}

const SINVENITEM* GLClubAgent::GetItem(DWORD Channel, WORD PosX, WORD PosY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return NULL;
    else
        return m_cStorage[Channel].GetItem(PosX, PosY);
}

BOOL GLClubAgent::IsInsertable(DWORD Channel, WORD PosX, WORD PosY)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;
    else
        return m_cStorage[Channel].IsInsertable(PosX, PosY, false);
}
/*
BOOL GLClubAgent::SplitItem(DWORD Channel, WORD PosX, WORD PosY, WORD Split)
{
    if (Channel >= MAX_CLUBSTORAGE)
        return FALSE;

    BOOL bOk(FALSE);
    GLInventory& sInventory = m_cStorage[Channel];

    // 분리 요청 아이템 찾음.
    SINVENITEM* pInvenItem = sInventory.FindPosItem(PosX, PosY);
    if (!pInvenItem)
        return FALSE;

    // 아이템 정보 가져오기.
    SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if (!pItem)
        return FALSE;

    // 분리 가능한 아이템인지 점검.    
    if (!pItem->ISPILE())
        return FALSE;
    if (Split < 1)
        return FALSE;
    if (pInvenItem->sItemCustom.wTurnNum <= Split)
        return FALSE;

    // 원본 아이템의 변화된 갯수.
    WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - Split;

    // 분리되어 새로 생기는 아이템.
    SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
    sNEW_ITEM.wTurnNum = Split;

    // 새로 생기는 아이템 위치 찾기.
    WORD wInvenPosX(0), wInvenPosY(0);
    bOk = sInventory.FindInsertable(wInvenPosX, wInvenPosY);
    if (!bOk)
        return FALSE;

    // 새로 생긴 아이템 넣기.
    bOk = sInventory.InsertItem(sNEW_ITEM, wInvenPosX, wInvenPosY);
    if (!bOk)
        return FALSE;

    // 새로 생긴 아이템 정보 가져오기.
    SINVENITEM* pNewItem = sInventory.GetItem(wInvenPosX, wInvenPosY);
    if (!pNewItem)
        return FALSE;

    // 원본 아이템의 갯수 변경 시킴.
    pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

    // 인벤에 새로 분리되어 들어 가는 아이템.
    GLMSG::SNET_CLUB_STORAGE_INSERT SendData;
    SendData.dwChannel = Channel;
    SendData.Data = *pNewItem;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    SendClubClient(NET_MSG_GCTRL_CLUB_STORAGE_INSERT, SendBuffer);

    // 원본 아이템 갯수 변경.
    GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM NetMsgStorage;
    NetMsgStorage.dwChannel = Channel;
    NetMsgStorage.wPosX = PosX;
    NetMsgStorage.wPosY = PosY;
    NetMsgStorage.wTurnNum = wORGIN_SPLIT;
    SendClubClient(&NetMsgStorage);

    m_bStorageUpdate = true;

    return S_OK;
}
*/
void GLClubAgent::ClubStorageUpdateItem( DWORD Channel, WORD PosX, WORD PosY, bool JustInvenTypeUpdate /*= false*/ )
{
	if ( Channel >= MAX_CLUBSTORAGE )
		return;

	SINVENITEM* pInvenItem = m_cStorage[Channel].GetItem( PosX, PosY );

	return ClubStorageUpdateItem( Channel, pInvenItem, JustInvenTypeUpdate );
}

void GLClubAgent::ClubStorageUpdateItem( DWORD Channel, SINVENITEM* pInvenItem, bool JustInvenTypeUpdate /*= false*/ )
{
	if ( Channel >= MAX_CLUBSTORAGE )
		return;

	if ( !pInvenItem )
	{
		return;
	}

	int DbState = pInvenItem->GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		m_pServer->AddGameAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_DbNum, INVEN_CLUB_LOCKER, pInvenItem->sSaveData, 59190, Channel ) ) );

		pInvenItem->SetDbState( db::DB_UPDATE, true );
	}
	else
	{
		if ( JustInvenTypeUpdate )
		{
			// 저장 위치 및 좌표만 변경된다.
			m_pServer->UpdateItemInvenType( m_DbNum, pInvenItem->sItemCustom, INVEN_CLUB_LOCKER, pInvenItem->wPosX, pInvenItem->wPosY, Channel );
		}
		else
		{
			m_pServer->AddGameAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_DbNum, INVEN_CLUB_LOCKER, pInvenItem->sSaveData, 59190, Channel ) ) );
		}
	}
}

void GLClubAgent::SaveStorageToDB()
{
    if (!m_bStorageLoadFromDb)
        return;

    // 창고 인벤 저장.
	m_pServer->AddGameAdoJob( db::DbActionPtr( new db::ClubStorageWrite( m_DbNum, this ) ) );
    /*se::ByteStream cStream;
    BOOL bOK = GETSTORAGE_BYBUF(cStream);
    if (bOK)
    {
        LPBYTE pBuff(NULL);
        DWORD dwSize(NULL);
        cStream.GetBuffer(pBuff, dwSize);

        m_pServer->AddGameDbJob(
            db::DbActionPtr(
                new db::ClubStorageWrite(m_DbNum, (BYTE*) pBuff, dwSize)));
    }*/

    // Money Db에 저장.
    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubMoneySet(m_DbNum, GetStorageMoney())));

    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubIncomeMoneySet(m_DbNum, GetIncomeMoney())));

    sc::writeLogInfo(sc::string::format("SaveStorageToDB ClubDbNum %1%", m_DbNum));

    m_bStorageUpdate = false;
}

void GLClubAgent::SendClubStorage(DWORD ClientSlot)
{   
    // 클럽 창고 정보 리샛
    GLMSG::SNET_CLUB_STORAGE_RESET NetMsgClubStorageReset;
    m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgClubStorageReset);

    // 창고의 기본 정보 전송
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY NetMsgMoney(GetStorageMoney(), GetIncomeMoney());
    m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgMoney);

    // 아이템을 하나씩 전송
    GLMSG::SNET_CLUB_GETSTORAGE_ITEM NetMsgItem;

    // 클라이언트에 창고 전송
    // 루프를 돌면서 하나씩 전송
    DWORD dwRANK = m_dwRank;
    if (dwRANK >= MAX_CLUBSTORAGE)
        dwRANK = MAX_CLUBSTORAGE-1;

    for (DWORD i=0; i<=dwRANK; ++i)
    {
        NetMsgItem.m_Channel = i;
        const GLInventory::CELL_MAP* pInvenList = m_cStorage[i].GetItemList();        
        for (GLInventory::CELL_MAP_CITER iter = pInvenList->begin(); iter!=pInvenList->end(); ++iter)
        {
            SINVENITEM* pInvenItem = iter->second;
            if (pInvenItem)
            {
                NetMsgItem.m_ItemData = *pInvenItem;
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, NetMsgItem);
                m_pServer->SENDTOCLIENT(ClientSlot, NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM, SendBuffer);
            }
        }
    }

    // 전송 끝
    GLMSG::NET_CLUB_STORAGE_SEND_END MsgEnd;
    m_pServer->SENDTOCLIENT(ClientSlot, &MsgEnd);

    // 클럽의 수익 갱신 요청
    //MsgReqClubInComeReNew(NULL);    
}

LONGLONG GLClubAgent::GetStorageMoney() const 
{ 
    return m_lnStorageMoney; 
}

void GLClubAgent::SetStorageMoney(LONGLONG Money)
{ 
    m_bStorageUpdate = true;
    m_lnStorageMoney = Money; 
    if (m_lnStorageMoney < 0)
        m_lnStorageMoney = 0;
}

void GLClubAgent::AddStorageMoney(LONGLONG Money)
{ 
    m_bStorageUpdate = true;
    m_lnStorageMoney += Money;
    if (m_lnStorageMoney < 0)
        m_lnStorageMoney = 0;
}

void GLClubAgent::SubtractStorageMoney(LONGLONG Money)
{
    m_bStorageUpdate = true;
    m_lnStorageMoney -= Money;
    if (m_lnStorageMoney < 0)
        m_lnStorageMoney = 0;
}

LONGLONG GLClubAgent::GetIncomeMoney() const 
{ 
    return m_lnIncomeMoney;
}

void GLClubAgent::SetIncomeMoney(LONGLONG Money)
{ 
    m_bStorageUpdate = true;
    m_lnIncomeMoney = Money;
    if (m_lnIncomeMoney < 0)
        m_lnIncomeMoney = 0;
}

void GLClubAgent::AddIncomeMoney(LONGLONG AddMoney)
{ 
    m_bStorageUpdate = true;
    m_lnIncomeMoney += AddMoney;
    if (m_lnIncomeMoney < 0)
        m_lnIncomeMoney = 0;
}

bool GLClubAgent::AddAuth(size_t Index, const club::AUTH_GRADE& Grade)
{
    if (!GLClub::AddAuth(Index, Grade))
        return false;

    // DB 저장
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
        new db::InsertClubAuth(
        m_DbNum, Index, Grade.m_Flag, Grade.m_Desc)));
    return true;
}

bool GLClubAgent::AddAuth(const club::AUTH_GRADE_DB& Grade)
{
    club::AUTH_GRADE Data(Grade.m_Desc, Grade.m_Flag);
    return GLClub::AddAuth(Grade.m_Index, Data);
}

bool GLClubAgent::ChangeAuth(size_t Index, club::AUTH_GRADE& Grade)
{
    if (!GLClub::ChangeAuth(Index, Grade))
        return false;

    // DB 저장
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::UpdateClubAuth(
                m_DbNum, Index, Grade.m_Flag, Grade.m_Desc)));
    return true;
}

bool GLClubAgent::SetGradeFlag(GLCharAG* pChar, size_t Index, DWORD Flag)
{
    if (!pChar || !GLClub::SetGradeFlag(Index, Flag))
        return false;

    if (!HaveAuthChangeAuthority(pChar->ChaDbNum()))
        return false;

    club::AUTH_GRADE AuthData = GetAuth(Index);
    if (!ChangeAuth(Index, AuthData))
        return false;

    // Log
    std::string Log(sc::string::format(ID2GAMEINTEXT("CLUB_LOG_10"), pChar->GetChaName()));
    m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_CLUB_AUTH_CHANGE, Log);

    club::GRADE_FLAG NewFlag(Index, Flag);

    // club 원들에게 알림
    NET_CLUB_GRADE_FLAG_CHANGE_AC SendDataAC;
    SendDataAC.m_ChaName = pChar->GetChaName();
    SendDataAC.m_Data.push_back(NewFlag);
    
    msgpack::sbuffer SendBufferAC;
    msgpack::pack(SendBufferAC, SendDataAC);

    SendClubClient(NET_MSG_CLUB_GRADE_FLAG_CHANGE_AC, SendBufferAC);

    // Field Server 에 알림
    NET_CLUB_GRADE_FLAG_CHANGE_AF SendDataAF(m_DbNum);
    SendDataAF.m_Data.push_back(NewFlag);

    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendDataAF);

    m_pServer->SENDTOALLCHANNEL(NET_MSG_CLUB_GRADE_FLAG_CHANGE_AF, SendBufferAF);

    return true;
}

bool GLClubAgent::SetGradeFlag(GLCharAG* pChar, const std::vector<club::GRADE_FLAG>& Data)
{
    if (!pChar)
        return false;

    if (!HaveAuthChangeAuthority(pChar->ChaDbNum())) 
        return false;    

    for (size_t i=0; i<Data.size(); ++i)
    {
        club::GRADE_FLAG GradeFlag = Data[i];
        if (!GLClub::SetGradeFlag(GradeFlag.m_Index, GradeFlag.m_Flag))
            continue;

        club::AUTH_GRADE AuthData = GetAuth(GradeFlag.m_Index);
        if (!ChangeAuth(GradeFlag.m_Index, AuthData))
            continue;
    }

    // Log
    std::string Log(sc::string::format(ID2GAMEINTEXT("CLUB_LOG_10"), pChar->GetChaName()));
    m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_CLUB_AUTH_CHANGE, Log);

    // club 원들에게 알림
    NET_CLUB_GRADE_FLAG_CHANGE_AC SendDataAC;
    SendDataAC.m_ChaName = pChar->GetChaName();
    SendDataAC.m_Data = Data;

    msgpack::sbuffer SendBufferAC;
    msgpack::pack(SendBufferAC, SendDataAC);
    SendClubClient(NET_MSG_CLUB_GRADE_FLAG_CHANGE_AC, SendBufferAC);

    // Field Server 에 알림
    NET_CLUB_GRADE_FLAG_CHANGE_AF SendDataAF(m_DbNum);
    SendDataAF.m_Data = Data;

    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendDataAF);
   
    m_pServer->SENDTOALLCHANNEL(NET_MSG_CLUB_GRADE_FLAG_CHANGE_AF, SendBufferAF);

    return true;
}

bool GLClubAgent::ChangeAuth(size_t IndexA, size_t IndexB)
{
    if (!GLClub::ChangeAuth(IndexA, IndexB))
        return false;

    // DB 저장
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
        new db::ChangeClubAuthIndex(
        m_DbNum, IndexA, IndexB)));
    return true;
}

bool GLClubAgent::ChangeMemberGrade(DWORD MemberDbNum, size_t Grade)
{
    if (!GLClub::ChangeMemberGrade(MemberDbNum, Grade))
        return false;

    // DB save
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
        new db::ChangeClubMemberGrade(
        MemberDbNum,
        static_cast<int> (Grade))));
    
    GLCLUBMEMBER* pMember = GetMember(MemberDbNum);
    if (pMember)
    {
		// Club log
		// [%1%]님의 계급이 [%2%]으로 변경되었습니다.
		if ( m_pServer->GetServiceProvider() == SP_GS )
		{
			std::string ClubLog(
				sc::string::format(
				ID2GAMEINTEXT("CLUB_LOG_29"),
				pMember->GetNameStr(),
				sc::string::utf8_to_ansi(GetGradeName(Grade))));
			m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_CLUB_AUTH_CHANGE, ClubLog);
		}
		else
		{
			std::string ClubLog(
				sc::string::format(
				ID2GAMEINTEXT("CLUB_LOG_29"),
				pMember->GetNameStr(),
				GetGradeName(Grade)));
			m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_CLUB_AUTH_CHANGE, ClubLog);
		}
    }
    return true;
}

bool GLClubAgent::ChangeGradeName(size_t GradeIndex, const std::string& GradeName, DWORD ReqChaDbNum)
{
    if (!GLClub::ChangeGradeName(GradeIndex, GradeName, ReqChaDbNum))
        return false;

    club::AUTH_GRADE GradeInfo = GetAuth(GradeIndex);

    // DB Save
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::UpdateClubAuth(
                m_DbNum,
                GradeIndex,
                GradeInfo.m_Flag,
                GradeInfo.m_Desc)));
    return true;
}

bool GLClubAgent::PublicSet(DWORD Flag, GLCharAG* pChar)
{
    if (!pChar)
        return false;

    if (!HaveAuthChangeAuthority(pChar->ChaDbNum()))
        return false;

    GLClub::PublicSet(Flag);

    // db 저장
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubPublicFlagUpdate(
                m_DbNum,
                Flag)));

    // 클럽원들에게 알림
    NET_CLUB_PULIC_FLAG_CHANGE_AC MsgAC(Flag, pChar->GetChaName());
    SendClubClient(&MsgAC);
    
    // Club log
    std::string ClubLog(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_11"), pChar->GetChaName()));
    m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_CLUB_AUTH_CHANGE, ClubLog);

    // field 서버에 알림
    NET_CLUB_PULIC_FLAG_CHANGE_AF MsgAF(m_DbNum, Flag);
    m_pServer->SENDTOALLCHANNEL(&MsgAF);

    return true;
}

void GLClubAgent::AutoKickOut()
{
    // auto kick out 을 사용하는지 체크
    if (!m_AutoKickOut.m_Use)
        return;

    __time64_t CurrentTime = sc::time::GetCurrentTime();
    std::vector<DWORD> vDelMember;
    
    // 자동 탈퇴시킬 멤버 선정
    for (CLUBMEMBERS_CITER iter=m_mapMembers.begin(); iter!=m_mapMembers.end(); ++iter)
    {
        // Check member auto kick out
        const GLCLUBMEMBER& Member = iter->second;
        
        // Master 는 제외
        if (Member.m_ChaDbNum == m_MasterCharDbNum)
            continue;
        
        // 계급 체크
        if (Member.GetGrade() > m_AutoKickOut.m_GradeIndex)
            continue;

        // 마지막 play 타임 체크
        __time64_t LastPlayTime = Member.GetLastPlayTime();
        __time64_t GapTime = CurrentTime - LastPlayTime;
        if (GapTime <= 0)
            continue;

        WORD GapDay = static_cast<WORD> (GapTime/86400); // 하루 60초*60분*24시간
        if (GapDay > m_AutoKickOut.m_Day)
        {
            std::string LogStr(
                sc::string::format(
                    "Club Auto Kick ChaDbNum %1% ClubDbNum %2% Time %3%>%4% CurTime %5% LastPlayTime %6%",
                    Member.m_ChaDbNum,
                    m_DbNum,
                    GapDay,
                    m_AutoKickOut.m_Day,
                    CurrentTime,
                    LastPlayTime));
            sc::writeLogWarn(LogStr);
            vDelMember.push_back(Member.m_ChaDbNum);
        }
    }

    for (size_t i=0; i<vDelMember.size(); ++i)
        DelMember(vDelMember[i], 0, std::string(""), true);
}

bool GLClubAgent::DelMember(DWORD dwCharID, DWORD ActionChaDbNum, const std::string& ActionChaName, bool bIsKick)
{
    if (m_MasterCharDbNum == dwCharID)
        return false;

    CLUBMEMBERS_ITER pos = m_mapMembers.find(dwCharID);
    if (pos == m_mapMembers.end())
        return false;

	bool IsAutoJoinClub = m_pServer->IsAutoJoinClub( m_DbNum );

    GLCLUBMEMBER& Member = pos->second;

    GLCharAG* pChar = m_pServer->GetCharByDbNum(dwCharID);
    if (pChar)
    {
		// 캐릭터가 online 상태일때 탈퇴(당)한 경우;
		const MapID& _mapID = pChar->GetCurrentMap();
		GLAGLandManager* _pLandMan(m_pServer->GetLandManager(_mapID));
		if ( _pLandMan )
		{
			// 구 CDM진행중에 클럽 탈퇴 안되도록 해달라는 요청 관련 수정[8/1/2014 hsshin];
			// 구 CDM에서 해당 랜드가 CDM전투맵인지를 검사하는 코드를 추가함;
			// 최대한 구 CDM을 빨리 버려버리고 이 코드를 삭제하는게 좋다.;
			if ( m_pServer->IsClubDeathMatchMap(_mapID.getBaseMapID()) == true ) 
				return false;
				
			InstanceSystem::InstanceAgent* const _pInstance = _pLandMan->getScriptInstance();
			if ( _pInstance )
				_pInstance->eventClubOut(m_DbNum, dwCharID, ActionChaDbNum);
		}
        pChar->SetClubDbNum(CLUB_NULL);

		// http://172.16.2.87/redmine/issues/4577
		if ( false == IsAutoJoinClub )
		{
			pChar->SetClubSecedeTime(sc::time::GetCurrentTime());
		}
    }

    if (ActionChaDbNum == 0)
    {
        // 클럽 맴버 탈퇴 로그 기록.
		// 통계 및 로그 수정
		// 캐릭터가 접속중이 아닐때 클럽에서 탈퇴시키는경우에는 MapID가 -1로 설정
        m_pServer->LogCharAction(
            m_MasterCharDbNum,		//	당사자.
            gamelog::ACT_CLUB_SECEDE_MEMBER,//	행위.
            gamelog::ID_CHAR, dwCharID,	//	상대방.
            0,							//	exp
            0,							//	bright
            0,							//	life
            0,							//	money
			( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
            );
    }
    else
    {
		// 통계 및 로그 수정
		// 캐릭터가 접속중이 아닐때 클럽에서 탈퇴시키는경우에는 MapID가 -1로 설정
        m_pServer->LogCharAction(
            ActionChaDbNum, // 당사자
            gamelog::ACT_CLUB_KICK_MEMBER, // 행위
            gamelog::ID_CHAR, dwCharID, // 상대방
            0, // exp
            0, // bright
            0, // life
            0,  // money
			( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
            );
    }

    if (ActionChaName.empty())
    {
        std::string ClubLog(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_3"),
                Member.GetName()));
        m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_MEMBER, ClubLog);
    }
    else
    {
        std::string ClubLog(
            sc::string::format(
                ID2GAMEINTEXT("CLUB_LOG_4"),
                ActionChaName,
                Member.GetName()));
        m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_MEMBER, ClubLog);
    }
    
    // CDM 권한 제거
    SetMemberCDMFlag(dwCharID, FALSE);

	// http://172.16.2.87/redmine/issues/4577
	if ( IsAutoJoinClub )
	{
		// DB에서 삭제.	
		m_pServer->AddGameAdoJob( db::DbActionPtr( new db::ClubMemberDeleteWithNoPenalty( dwCharID ) ) );
	}
	else
	{
		// DB에서 삭제.	
		m_pServer->AddGameAdoJob(
			db::DbActionPtr(
			new db::ClubMemberDelete(dwCharID)));
	}

    // CD 인증 권한자 체크
    DWORD OldCd = m_dwCDCertifior;
    if (OldCd == dwCharID)
    {
        // CD 인증자를 Master 로 세팅
        m_dwCDCertifior = m_MasterCharDbNum;

        // DB 저장
        m_pServer->AddGameDbJob(
            db::DbActionPtr(
                new db::ClubDeputySet(
                    m_DbNum,
                    m_dwCDCertifior)));

        // CD 인증권한 변경 알림
        GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AC MsgAC(m_dwCDCertifior);
        SendClubClient(&MsgAC);

        // 모든 서버에 알림
        GLMSG::NET_CLUB_CD_CERTIFIER_CHANGE_AF MsgAF(m_DbNum, m_dwCDCertifior, OldCd);
        m_pServer->SENDTOALLCHANNEL(&MsgAF);
    }
	
	
    // Club member 삭제 필드에 알림.
    GLMSG::NET_CLUB_MEMBER_DEL_AF NetMsgFld(m_DbNum, dwCharID, sc::time::GetCurrentTime(), Member.GetName(),  bIsKick);
    m_pServer->SENDTOALLCHANNEL(&NetMsgFld);
        
    // Member 삭제
    GLClub::DelMember(dwCharID);

    return true;
}

bool GLClubAgent::AutoKickOutSet(const club::AUTO_KICK_OUT& AutoKickOut, GLCharAG* pChar)
{
    // NET_CLUB_AUTO_KICKC_CA NET_MSG_CLUB_AUTO_KICKC_CA
    if (!pChar)
        return false;

    if (!HaveMemberKickAuthority(pChar->ChaDbNum()))
        return false;

    GLClub::AutoKickOutSet(AutoKickOut);

    // club log
    // %1% 님이 자동 탈퇴 조건을 변경하였습니다.
    std::string LogMsg(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_12"), pChar->GetChaName()));
    m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_MEMBER, LogMsg);

    // club 원들에게 알림
    CLUB_AUTO_KICK_AC KickData(pChar->GetChaName(), AutoKickOut);
    msgpack::sbuffer PackBufferAC;
    msgpack::pack(PackBufferAC, KickData);
    SendClubClient(NET_MSG_CLUB_AUTO_KICK_AC, PackBufferAC);

    // 전체서버에 알림
    NET_CLUB_AUTO_KICKC_AF KickAF(m_DbNum, AutoKickOut);
    msgpack::sbuffer PackBufferAF;
    msgpack::pack(PackBufferAF, KickAF);
    m_pServer->SENDTOALLCHANNEL(NET_MSG_CLUB_AUTO_KICK_AF, PackBufferAF);

    // db 저장
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubAutoKickUpdate(
                m_DbNum,
                AutoKickOut.m_GradeIndex,
                AutoKickOut.m_Day,
                AutoKickOut.m_Use)));
    return true;
}

void GLClubAgent::SetNoticeNewbie(const std::string& Notice, GLCharAG* pChar)
{
    // NET_CLUB_NEWBIE_NOTICE_CA NET_MSG_CLUB_NEWBIE_NOTICE_CA
    if (!pChar)
        return;
    if (!HaveNoticeChangeAuthority(pChar->ChaDbNum()))
        return;

    GLClub::SetNoticeNewbie(Notice);

    // club log
    // %1%님이 신입생 소개글을 변경하였습니다.
    std::string LogMsg(
        sc::string::format(
            ID2GAMEINTEXT("CLUB_LOG_13"), pChar->GetChaName()));
    m_pServer->SENDCLUBLOG(m_DbNum, club::LOG_CLUB_NOTICE, LogMsg);    

    // club 원들에게 알림
    NET_CLUB_NEWBIE_NOTICE_AC SendDataAC;
    SendDataAC.m_ChaName = pChar->GetChaName();
    SendDataAC.m_NoticeNewbie = Notice;

    msgpack::sbuffer SendBufferAC;
    msgpack::pack(SendBufferAC, SendDataAC);
    SendClubClient(NET_MSG_CLUB_NEWBIE_NOTICE_AC, SendBufferAC);

    // 전체서버 알림
    NET_CLUB_NEWBIE_NOTICE_AF SendDataAF;
    SendDataAF.m_ClubDbNum = m_DbNum;
    SendDataAF.m_NoticeNewbie = Notice;

    msgpack::sbuffer SendBufferAF;
    msgpack::pack(SendBufferAF, SendDataAF);
    m_pServer->SENDTOALLCHANNEL(NET_MSG_CLUB_NEWBIE_NOTICE_AF, SendBufferAF);

    // db 저장
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubNewbieNoticeUpdate(
                m_DbNum,
                Notice)));
}

void GLClubAgent::SetMemberState(DWORD ChaDbNum, const MapID& MapId)
{
    GLClub::SetMemberState(ChaDbNum, MapId);

//     GLMSG::NET_MAP_CHANGE_FRIEND_CLUB_AC SendData(ChaDbNum, MapId);
//     msgpack::sbuffer SendBuffer;
//     msgpack::pack(SendBuffer, SendData);
// 
//     SendClubClient(NET_MSG_MAP_CHANGE_FRIEND_CLUB_AC, SendBuffer);
}

void GLClubAgent::SetMemberState(DWORD dwCharID, bool bONLINE, int nCHANNEL, const MapID& nidMAP)
{
    GLClub::SetMemberState(dwCharID, bONLINE, nCHANNEL, nidMAP);
}

void GLClubAgent::SetMemberState(DWORD ChaDbNum, WORD ChaLevel)
{
    GLClub::SetMemberState(ChaDbNum, ChaLevel);
}

void GLClubAgent::SetMemberPos(DWORD dwCharID, const D3DXVECTOR3& vecPOS)
{
    GLClub::SetMemberPos(dwCharID, vecPOS);
}

void GLClubAgent::SendClubInfoToAllMember()
{
    // 클럽원들에게 클럽 정보 전송
    for (CLUBMEMBERS_ITER pos=m_mapMembers.begin(); pos!=m_mapMembers.end(); ++pos)
    {
        const GLCLUBMEMBER& MemberInfo = pos->second;
        GLCharAG* pChar = m_pServer->GetCharByDbNum(MemberInfo.GetChaDbNum());
        if (pChar)
            SendClubInfo(pChar->ClientSlot());
    }
}

void GLClubAgent::SendClubInfo(DWORD ClientSlot)
{
    SendClubInfoBase(ClientSlot);    
    SendClubInfoMember(ClientSlot);
    SendClubInfoAlliance(ClientSlot);
    SendClubInfoBattle(ClientSlot);
    SendClubInfoAuth(ClientSlot);

    // Club info end
    //GLMSG::NET_LOBBY_CLUB_INFO_END MsgEnd;
    //m_pServer->SENDTOCLIENT(ClientSlot, &MsgEnd);
}

void GLClubAgent::SendClubInfoBase(DWORD ClientSlot)
{
    MIN_STATIC_ASSERT(GLClub::VERSION <= 0x0007);
    MIN_STATIC_ASSERT(GLMSG::SNETLOBBY_CLUB_INFO::VERSION <= 0x0002);

    GLMSG::SNETLOBBY_CLUB_INFO SendData;
    SendData.dwClubID       = m_DbNum;
    SendData.SetClubName(m_szName);
    SendData.dwMasterID     = m_MasterCharDbNum;
    SendData.dwMasterLevel  = m_MasterLevel;
    SendData.SetMasterName(m_szMasterName);
    SendData.dwCDCertifior  = m_dwCDCertifior;
    SendData.dwRank         = m_dwRank;
    SendData.dwMarkVER      = m_dwMarkVER;
    SendData.tOrganize      = m_tOrganize;
    SendData.tDissolution   = m_tDissolution;
    SendData.dwAlliance     = m_dwAlliance;
    SendData.dwMEMBER_NUM   = GetMemberNum();
    SendData.dwALLIANCE_NUM = GetAllianceNum();
    SendData.dwBATTLE_NUM   = GetBattleNum();
    SendData.dwBattleWin	= m_dwBattleWin;
    SendData.dwBattleLose	= m_dwBattleLose;
    SendData.dwBattleDraw	= m_dwBattleDraw;

    if (m_dwAlliance != CLUB_NULL)
    {
        std::tr1::shared_ptr<GLClubAgent> spCLUB_A = m_pServer->GetClub(m_dwAlliance);
        if (spCLUB_A)
        {
            SendData.dwALLIANCE_NUM = spCLUB_A->GetAllianceNum();
            SendData.dwAllianceBattleWin	= spCLUB_A->m_dwAllianceBattleWin;
            SendData.dwAllianceBattleLose	= spCLUB_A->m_dwAllianceBattleLose;
            SendData.dwAllianceBattleDraw	= spCLUB_A->m_dwAllianceBattleDraw;
        }
    }

    SendData.m_PublicType  = GetPublicFlag();
    SendData.m_AutoKickOut = GetAutoKickOut();

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    m_pServer->SENDTOCLIENT(ClientSlot, NET_MSG_LOBBY_CLUB_INFO, SendBuffer);

    // Send Extra data
    GLMSG::SNETLOBBY_CLUB_INFO_EX SendDataEx;
    SendDataEx.SetNotice(m_szNotice);
    SendDataEx.SetNoticeDate(GetNoticeDate());
    SendDataEx.SetNoticeCharName(GetNoticeChaName());
    SendDataEx.SetNoticeNewbie(GetNoticeNewbie());

    msgpack::sbuffer SendBufferEx;
    msgpack::pack(SendBufferEx, SendDataEx);
    m_pServer->SENDTOCLIENT(ClientSlot, NET_MSG_LOBBY_CLUB_INFO_EX, SendBufferEx);
}

void GLClubAgent::SendClubInfoMember(DWORD ClientSlot)
{
    MIN_STATIC_ASSERT(GLCLUBMEMBER::VERSION <= 0x0004);
/*
    GLMSG::SNETLOBBY_CLUB_MEMBER NetMsgMember;
    CLUBMEMBERS& Members = spClub->GetMembers();
    CLUBMEMBERS_CITER pos = Members.begin();
    CLUBMEMBERS_CITER end = Members.end();
    for ( ; pos!=end; ++pos)
    {
        const GLCLUBMEMBER& sMEMBER = pos->second;
        bool bOK = NetMsgMember.ADDMEMBER(sMEMBER.dwID, sMEMBER.m_Grade, sMEMBER.m_szName, sMEMBER.m_ChaLevel);
        if (!bOK)
        {
            // 메시지 전송.
            m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgMember);
            NetMsgMember.RESET();
            NetMsgMember.ADDMEMBER(sMEMBER.dwID, sMEMBER.m_Grade, sMEMBER.m_szName, sMEMBER.m_ChaLevel);
        }
    }

    // 메시지 전송.
    if (NetMsgMember.dwMemberNum > 0)
    {
        m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgMember);
        NetMsgMember.RESET();
    }
*/      


    // 묶어서 보내는 코드 추가


    int nLimite=0;
    while( sizeof(GLCLUBMEMBER) * nLimite < NET_DATA_BUFSIZE )
        nLimite++;

    SCLUBMEMBER member;
    int i=0;
    for (CLUBMEMBERS_CITER pos=m_mapMembers.begin(); pos!=m_mapMembers.end(); ++pos)
    {
        i++;
        const GLCLUBMEMBER& sMEMBER = pos->second;
        member.vecClubMember.push_back( sMEMBER );

        CLUBMEMBERS_CITER posTemp = pos; ++posTemp;
        if( i==nLimite || posTemp==m_mapMembers.end() )
        {
            msgpack::sbuffer PackBuffer;
            msgpack::pack(PackBuffer, member);
            //NET_DATA_BUFSIZE
            NET_MSG_PACK MsgAC;
            if (MsgAC.SetData(NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK, PackBuffer))
                m_pServer->SENDTOCLIENT(ClientSlot, &MsgAC);
            else
                sc::writeLogError("GLClubManAgent::SendClubInfoMember SetData");

            i=0;
            member.vecClubMember.clear();
        }

    }

    NET_MSG_GENERIC MsgEnd;
    MsgEnd.nType = NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK_END;
    m_pServer->SENDTOCLIENT(ClientSlot, &MsgEnd);
}

void GLClubAgent::SendClubInfoAlliance(DWORD ClientSlot)
{
    if (m_dwAlliance != CLUB_NULL)
    {				
        std::tr1::shared_ptr<GLClubAgent> pCLUB_A = m_pServer->GetClub(m_dwAlliance);
        if (pCLUB_A)
        {
            GLMSG::SNETLOBY_CLUB_ANCE_2CLT NetMsgAlliance;
            CLUB_ALLIANCE_ITER pos = pCLUB_A->m_setAlliance.begin();
            CLUB_ALLIANCE_ITER end = pCLUB_A->m_setAlliance.end();
            for ( ; pos!=end; ++pos)
            {
                const GLCLUBALLIANCE& sALLIANCE = pos->second;
                bool bOK = NetMsgAlliance.ADDALLIANCE(sALLIANCE.m_dwID, sALLIANCE.m_szName, sALLIANCE.m_szMasterName, sALLIANCE.m_MarkVersion);
                if (!bOK)
                {
                    // 메시지 전송
                    m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgAlliance);
                    NetMsgAlliance.RESET();
                    NetMsgAlliance.ADDALLIANCE(sALLIANCE.m_dwID, sALLIANCE.m_szName, sALLIANCE.m_szMasterName, sALLIANCE.m_MarkVersion);
                }
            }

            // 메시지 전송
            if (NetMsgAlliance.dwAllianceNum > 0)
            {
                m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgAlliance);
                NetMsgAlliance.RESET();
            }
        }
    }
}

void GLClubAgent::SendClubInfoBattle(DWORD ClientSlot)
{
    GLMSG::SNETLOBBY_CLUB_BATTLE NetMsgBattle;
        
    for (CLUB_BATTLE_ITER pos=m_mapBattle.begin(); pos!=m_mapBattle.end(); ++pos)
    {
        const GLCLUBBATTLE& sBattle = (*pos).second;
        GLCLUBBATTLE_LOBY sBattleLoby = sBattle;
        bool bOK = NetMsgBattle.ADDBATTLE(sBattleLoby);
        if (!bOK)
        {
            // 메시지 전송
            m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgBattle);
            NetMsgBattle.RESET();
            NetMsgBattle.ADDBATTLE(sBattleLoby);
        }
    }

    // 메시지 전송
    if (NetMsgBattle.dwBattleNum > 0)
    {
        m_pServer->SENDTOCLIENT(ClientSlot, &NetMsgBattle);
        NetMsgBattle.RESET();
    }
}

void GLClubAgent::SendClubInfoAuth(DWORD ClientSlot)
{
    size_t GradeSize = GLogicData::GetInstance().GetClubGradeSize();
    for (size_t i=0; i<GradeSize; ++i)
    {        
        GLMSG::NET_CLUB_AUTH_FC MsgAC(static_cast<BYTE> (i), GetAuth(i));
        m_pServer->SENDTOCLIENT(ClientSlot, &MsgAC);
    }
}

DWORD GLClubAgent::GetChangeClubMarkTime()
{
    return m_ChangeClubMarkTime;
}

void GLClubAgent::SetChangeClubMarkTime( DWORD CurTime )
{
    m_ChangeClubMarkTime = CurTime;
}

bool GLClubAgent::ReqClubStorageUse( DWORD ChaDbNum )
{
	if ( GAEAID_NULL == m_CurrentClubStorageUser )
	{
		m_CurrentClubStorageUser = ChaDbNum;
		m_LastClubStorageUseTime = sc::time::GetCurrentTime();
		return true;
	}
	else
	{
		return false;
	}
}

void GLClubAgent::ResetClubStorageUser()
{
	m_CurrentClubStorageUser = GAEAID_NULL;
}

std::string GLClubAgent::GetClubStorageUserName()
{
	std::string Name( "" );
	if ( m_pServer && GAEAID_NULL != m_CurrentClubStorageUser )
	{
		Name = m_pServer->GetChaNameByDbNum( m_CurrentClubStorageUser );
	}
	return Name;
}

bool GLClubAgent::ReqClubStorageMoneyUse( DWORD ChaDbNum )
{
	if ( GAEAID_NULL == m_CurrentClubStorageMoneyUser )
	{
		m_CurrentClubStorageMoneyUser = ChaDbNum;
		m_LastClubStorageMoneyUseTime = sc::time::GetCurrentTime();
		return true;
	}
	else
	{
		return false;
	}
}

void GLClubAgent::ResetClubStorageMoneyUser()
{
	m_CurrentClubStorageMoneyUser = GAEAID_NULL;
}

std::string GLClubAgent::GetClubStorageMoneyUserName()
{
	std::string Name( "" );
	if ( m_pServer && GAEAID_NULL != m_CurrentClubStorageMoneyUser )
	{
		Name = m_pServer->GetChaNameByDbNum( m_CurrentClubStorageMoneyUser );
	}
	return Name;
}

void GLClubAgent::CopyStorage( DWORD Channel, GLInventoryStorage& Target )
{
	if ( Channel >= MAX_CLUBSTORAGE )
		return;

	Target.Assign( m_cStorage[Channel] );
}

void GLClubAgent::PasteStorage( DWORD Channel, GLInventoryStorage& Source )
{
	if ( Channel >= MAX_CLUBSTORAGE )
		return;

	m_cStorage[Channel].Assign( Source );
}

SINVENITEM* GLClubAgent::InsertItemNew( const SITEMCUSTOM& Item, DWORD Channel, WORD PosX, WORD PosY, bool bSendClient /*= true*/ )
{
	SINVENITEM* pInsertedItem = NULL;

	if ( Channel >= MAX_CLUBSTORAGE )
		return pInsertedItem;

	pInsertedItem = m_cStorage[Channel].InsertItemNew( Item, PosX, PosY );
	if ( pInsertedItem )
	{
		if ( bSendClient )
		{
			GLMSG::SNET_CLUB_STORAGE_INSERT SendData;
			SendData.dwChannel = Channel;
			SendData.Data = *pInsertedItem;

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, SendData );

			SendClubClient( NET_MSG_GCTRL_CLUB_STORAGE_INSERT, SendBuffer );
		}

		m_bStorageUpdate = true;
		return pInsertedItem;
	}
	
	return pInsertedItem;
}

BOOL GLClubAgent::ValidateVehicleRandomOption()
{
	BOOL Result = TRUE;
	for ( int i = 0; i < MAX_CLUBSTORAGE; ++i )
	{
		if ( FALSE == m_cStorage[i].ValidateVehicleRandomOption() )
		{
			Result = FALSE;
		}
	}
	return Result;
}

BOOL GLClubAgent::ValidateWrappedItem( std::vector< SITEMCUSTOM >& vecPostItems, std::vector< SINVENITEM* >& vecUnWrappedItems )
{
	BOOL Result = TRUE;
	for ( int i = 0; i < MAX_CLUBSTORAGE; ++i )
	{
		if ( FALSE == m_cStorage[i].ValidateWrappedItem( vecPostItems, vecUnWrappedItems ) )
		{
			Result = FALSE;
		}
	}
	return Result;
}

BOOL GLClubAgent::ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems )
{
	BOOL Result = TRUE;
	for ( int i = 0; i < MAX_CLUBSTORAGE; ++i )
	{
		if ( FALSE == m_cStorage[i].ValidateRandomOption( vecDeleteItems ) )
		{
			Result = FALSE;
		}
	}
	return Result;
}
