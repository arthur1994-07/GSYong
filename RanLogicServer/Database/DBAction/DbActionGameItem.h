#pragma once

#include "../../../SigmaCore/String/MinGuid.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Msg/ServerMsg.h"

#include "DbAction.h"

struct NET_CHA_BBA_INFO_HA;

namespace db
{

//! 신규 아이템 삽입
class ItemInsert : public DbAction
{
public:
	ItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel = 0 );
	virtual ~ItemInsert() {};
	virtual int Execute( NetServer* pServer );

protected:
	DWORD m_ChaDbNum;
	INVEN_TYPE m_InvenType;
	SINVENITEM_SAVE m_Item;
	int m_CallType;
	DWORD m_Channel;	// 유저락커의 경우에 사용됨
};

class ItemUpdate : public DbAction
{
public:
	ItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel = 0 );
	virtual ~ItemUpdate() {};
	virtual int Execute( NetServer* pServer );

protected:
	DWORD m_ChaDbNum;
	INVEN_TYPE m_InvenType;
	SINVENITEM_SAVE m_Item;
	int m_CallType;
	DWORD m_Channel;	// 유저락커의 경우에 사용됨
};

class EntireItemUpdate : public DbAction
{
public:
	EntireItemUpdate( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel = 0 );
	virtual ~EntireItemUpdate() {};
	virtual int Execute( NetServer* pServer );

protected:
	DWORD m_ChaDbNum;
	INVEN_TYPE m_InvenType;
	SINVENITEM_SAVE m_Item;
	int m_CallType;
	DWORD m_Channel;	// 유저락커의 경우에 사용됨
};

class ItemDelete : public DbAction
{
public:
	ItemDelete( sc::MGUID& Guid, int State, int CallType );
	virtual ~ItemDelete() {};
	virtual int Execute( NetServer* pServer );

protected:
	sc::MGUID m_Guid;
	int m_State;
	int m_CallType;
};

class ItemSaleToNpc : public DbAction
{
public:
	ItemSaleToNpc( sc::MGUID& Guid, int State, int CallType );
	virtual ~ItemSaleToNpc() {};
	virtual int Execute( NetServer* pServer );

protected:
	sc::MGUID m_Guid;
	int m_State;
	int m_CallType;
};

// 계정에 속한 모든 캐릭터들의 아이템을 바이너리에서 아이템 테이블로 삽입한다.
// 삭제된 캐릭터도 해줘야 하나?
class ItemTransfer : public DbAction
{
public:
	ItemTransfer( DWORD UserNum, const std::vector< DWORD >& vecChaNum, const NET_CHA_BBA_INFO_HA* pmsg );
	virtual ~ItemTransfer() {};
	virtual int Execute( NetServer* pServer );

protected:
	DWORD m_UserNum;
	std::vector< DWORD > m_vecChaNum;
	NET_CHA_BBA_INFO_DA m_msg;
};

// 아이템 위치 변경(저장공간이나 좌표)
class ItemUpdatePos : public DbAction
{
public:
	ItemUpdatePos( DWORD ChaDbNum, INVEN_TYPE InvenType, sc::MGUID& Guid, WORD PosX, WORD PosY, int CallType, DWORD Channel = 0 );
	virtual ~ItemUpdatePos() {};
	virtual int Execute( NetServer* pServer );

protected:
	DWORD m_ChaDbNum;
	INVEN_TYPE m_InvenType;
	sc::MGUID m_Guid;
	WORD m_PosX;
	WORD m_PosY;
	int m_CallType;
	DWORD m_Channel;	// 유저락커의 경우에 사용됨
};

class ItemUpdateTurnNum : public DbAction
{
public:
	ItemUpdateTurnNum( sc::MGUID& Guid, WORD TurnNum, int CallType );
	virtual ~ItemUpdateTurnNum() {};
	virtual int Execute( NetServer* pServer );

protected:
	sc::MGUID m_Guid;
	WORD m_TurnNum;
	int m_CallType;
};

class ItemInsertError : public DbAction
{
public:
	ItemInsertError( sc::MGUID& Guid, int CallType, __time64_t& Date );
	virtual ~ItemInsertError() {};
	virtual int Execute( NetServer* pServer );

protected:
	sc::MGUID m_Guid;
	int m_CallType;
	__time64_t m_Date;
};

class CompletelyItemDelete : public DbAction
{
public:
	CompletelyItemDelete( sc::MGUID& Guid, int State, int CallType );
	virtual ~CompletelyItemDelete() {};
	virtual int Execute( NetServer* pServer );

protected:
	sc::MGUID m_Guid;
	int m_State;
	int m_CallType;
};

// ItemDel 테이블에 아이템 삽입하기
// Item 테이블에서 삭제될 때 트리거 걸어서 삭제된 아이템을 자동으로 ItemDel 테이블로 삽입하면 성능 저하가 있기 때문에
// 아이템을 완전 삭제하기 전에 ItemDel 테이블에 직접 넣어주는 기능 추가
class DeletedItemInsert : public DbAction
{
public:
	DeletedItemInsert( DWORD ChaDbNum, INVEN_TYPE InvenType, SINVENITEM_SAVE& Item, int CallType, DWORD Channel = 0 );
	virtual ~DeletedItemInsert() {};
	virtual int Execute( NetServer* pServer );

protected:
	DWORD m_ChaDbNum;
	INVEN_TYPE m_InvenType;
	SINVENITEM_SAVE m_Item;
	int m_CallType;
	DWORD m_Channel;	// 유저락커의 경우에 사용됨
};

}