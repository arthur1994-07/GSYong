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

//! 캐릭터의 PointShop Point 를 추가한다.
//! PointRefundable 환불가능 포인트
//! PointNotRefundable 환불불가 포인트
bool GLChar::PointShopPointAdd(__int64 PointRefundable, __int64 PointNotRefundable, pointshop::EM_POINT_CHANGE_TYPE Type)
{
    if (PointRefundable < 0 || PointNotRefundable < 0)
        return false;
    
    CHARACTER_POINT OldPoint = m_PointShopPoint;
    
    CHARACTER_POINT NewPoint = m_PointShopPoint;        
    NewPoint.AddRefundable(PointRefundable);
    NewPoint.AddNotRefundable(PointNotRefundable);
    PointShopPointSet(NewPoint);

    // DB 에 저장
	// type 추가 : 포인트 사라지는경우 추적을 위해서 수정 
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
        // 환불 불가 Point 가 가격보다 높기 때문에 차감하고 종료;
        NewPoint.m_PointNotRefundable = NewPoint.m_PointNotRefundable - ItemPriceTemp;
    }
    else
    {
        // 환불 불가 Point 차감;
        ItemPriceTemp = ItemPriceTemp - NewPoint.m_PointNotRefundable;
        NewPoint.m_PointNotRefundable = 0;

        // 환불 가능 Point 차감;
        NewPoint.m_PointRefundable = NewPoint.m_PointRefundable - ItemPriceTemp;
    }

    if( NewPoint.m_PointNotRefundable < 0 || NewPoint.m_PointRefundable < 0 )
        sc::writeLogFatal(std::string("PointShopBuyItem illegal value"));

    // Character Point 변경;
    PointShopPointSet( NewPoint );

    // DB 에 저장;
	// type 추가 : 포인트 사라지는경우 추적을 위해서 수정 
    PointShopPointDbSave( NewPoint.Refundable(), NewPoint.NotRefundable(), nTraceType, OldPoint.m_PointRefundable, OldPoint.m_PointNotRefundable, point );

	//// Cache에 포인트 갱신;
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

//! PointShop 아이템 구매
//! 반드시 성공해야 한다. 함수롤 호출하기 전에 무결성을 체크해야 한다.
void GLChar::PointShopBuyItem( const SNATIVEID& BuyItem, __int64 ItemPrice, const SITEMCUSTOM& ItemCustom )
{    
    PointShopPointSub( BuyItem, ItemPrice,  pointshop::POINT_SUB_TYPE_SHOP_BUY );

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on PointShopBuyItem", m_CharDbNum ) );
	}

    // Item exchange log
	// 통계 및 로그 수정
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