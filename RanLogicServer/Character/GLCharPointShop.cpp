#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../RanLogic/Msg/CharacterMsg.h"

#include "../Database/DBAction/DbActionGamePointShop.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLChar::PointShopPointSet(const CHARACTER_POINT& Point)
{
    m_PointShopPoint = Point;
    NET_CHAR_POINT_FA MsgFC(m_CharDbNum, Point);
    SendToAgent(&MsgFC);
}

//! ĳ������ PointShop Point �� �߰��Ѵ�.
//! PointRefundable ȯ�Ұ��� ����Ʈ
//! PointNotRefundable ȯ�ҺҰ� ����Ʈ
bool GLChar::PointShopPointAdd(__int64 PointRefundable, __int64 PointNotRefundable, pointshop::EM_POINT_CHANGE_TYPE Type)
{
    if (PointRefundable < 0 || PointNotRefundable < 0)
        return false;
    
    CHARACTER_POINT OldPoint = m_PointShopPoint;
    
    CHARACTER_POINT NewPoint = m_PointShopPoint;        
    NewPoint.AddRefundable(PointRefundable);
    NewPoint.AddNotRefundable(PointNotRefundable);
    PointShopPointSet(NewPoint);

    // DB �� ����
	// type �߰� : ����Ʈ ������°�� ������ ���ؼ� ���� 
    PointShopPointDbSave(NewPoint.Refundable(), NewPoint.NotRefundable(), 0, OldPoint.m_PointRefundable, OldPoint.m_PointNotRefundable, 0 );    

    // Send Log
    SNATIVEID Item;
    PointShopPointLog(OldPoint, NewPoint, Type, Item, 0);

    return true;
}

void GLChar::PointShopPointSub( const SNATIVEID& BuyItem, __int64 ItemPrice, int nTraceType )
{
    PointShopPointSub( ItemPrice, BuyItem, pointshop::BUY_ITEM, nTraceType );
}

void GLChar::PointShopPointSubByHoldem( const SNATIVEID& Chip, __int64 ChipPrice )
{
	PointShopPointSub( ChipPrice, Chip, pointshop::HOLDEM_SYSTEM,  pointshop::POINT_SUB_TYPE_HOLDEM );
}

void GLChar::PointShopPointSub( __int64 point, pointshop::EM_POINT_CHANGE_TYPE ePointChangeType, int nTraceType  )
{
    PointShopPointSub( point, SNATIVEID(0,0), ePointChangeType, nTraceType );
}

void GLChar::PointShopPointSub( __int64 point, const SNATIVEID& Item, pointshop::EM_POINT_CHANGE_TYPE ePointChangeType, int nTraceType )
{
    CHARACTER_POINT OldPoint = m_PointShopPoint;
    CHARACTER_POINT NewPoint = PointShopPointSub( point, nTraceType );

    // Send Log
    PointShopPointLog(OldPoint, NewPoint, ePointChangeType, Item, point);
}

CHARACTER_POINT GLChar::PointShopPointSub( __int64 point, int nTraceType )
{
	CHARACTER_POINT OldPoint = m_PointShopPoint;
    CHARACTER_POINT NewPoint = m_PointShopPoint;
    __int64 ItemPriceTemp = point;

    if( NewPoint.m_PointNotRefundable >= ItemPriceTemp )
    {
        // ȯ�� �Ұ� Point �� ���ݺ��� ���� ������ �����ϰ� ����;
        NewPoint.m_PointNotRefundable = NewPoint.m_PointNotRefundable - ItemPriceTemp;
    }
    else
    {
        // ȯ�� �Ұ� Point ����;
        ItemPriceTemp = ItemPriceTemp - NewPoint.m_PointNotRefundable;
        NewPoint.m_PointNotRefundable = 0;

        // ȯ�� ���� Point ����;
        NewPoint.m_PointRefundable = NewPoint.m_PointRefundable - ItemPriceTemp;
    }

    if( NewPoint.m_PointNotRefundable < 0 || NewPoint.m_PointRefundable < 0 )
        sc::writeLogFatal(std::string("PointShopBuyItem illegal value"));

    // Character Point ����;
    PointShopPointSet( NewPoint );

    // DB �� ����;
	// type �߰� : ����Ʈ ������°�� ������ ���ؼ� ���� 
    PointShopPointDbSave( NewPoint.Refundable(), NewPoint.NotRefundable(), nTraceType, OldPoint.m_PointRefundable, OldPoint.m_PointNotRefundable, point );

	//// Cache�� ����Ʈ ����;
	//PointShopPointUpdateCache( NewPoint );

    return NewPoint;
}

void GLChar::PointShopPointUpdateCache( const CHARACTER_POINT& sUpdatePoint )
{
	NET_CHAR_POINT_CHANGE_CACHE_FH MsgFH(
		m_CharDbNum,
		sUpdatePoint );
	SendToCache( &MsgFH );
}

//! PointShop ������ ����
//! �ݵ�� �����ؾ� �Ѵ�. �Լ��� ȣ���ϱ� ���� ���Ἲ�� üũ�ؾ� �Ѵ�.
void GLChar::PointShopBuyItem( const SNATIVEID& BuyItem, __int64 ItemPrice, const SITEMCUSTOM& ItemCustom )
{    
    PointShopPointSub( BuyItem, ItemPrice,  pointshop::POINT_SUB_TYPE_SHOP_BUY );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on PointShopBuyItem", m_CharDbNum ) );
	}

    // Item exchange log
	// ��� �� �α� ����
    LogItemExchange(
        ItemCustom,
        gamelog::ID_NONE, 0,
        gamelog::ID_CHAR, m_CharDbNum,
        ITEM_ROUTE_SHOP,
        1,
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_POINTSHOP,
        true);
}

void GLChar::PointShopPointLog(
    const CHARACTER_POINT& OldPoint,
    const CHARACTER_POINT& NewPoint,
    pointshop::EM_POINT_CHANGE_TYPE Type,
    const SNATIVEID& ItemId,
    __int64 ItemPrice)
{
    NET_CHAR_PS_POINT_CHANGE_FH MsgFH(
        OldPoint,
        NewPoint,
        m_CharDbNum,
        Type,
        ItemId,
        ItemPrice,
        m_dwUserID,
        m_dwServerID,
        m_wLevel);
    SendToCache(&MsgFH);
}

void GLChar::PointShopPointDbSave(__int64 PointRefundable, __int64 PointNotRefundable, int nType, __int64 OriPointRefundable, __int64 OriPointNotRefundable, __int64 SubValue )
{
	if( PointRefundable == 0 && PointNotRefundable == 0 )
	{
		sc::writeLogError(sc::string::format("[TRACE POINT] m_CharDbNum : %1%, nType : %2%", m_CharDbNum,  nType));
	}

	sc::writeLogInfo(sc::string::format("[TRACE POINT] m_CharDbNum : %1%, nType : %2%, Refundable : %3%, NotRefundable : %4%, OriRefundable : %5%, OriNotRefundable : %6%, SubValue : %7%", m_CharDbNum,  nType, PointRefundable, PointNotRefundable, OriPointRefundable, OriPointNotRefundable, SubValue ));

    AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterPointSet(
                m_CharDbNum,
                PointRefundable,
                PointNotRefundable)));
}