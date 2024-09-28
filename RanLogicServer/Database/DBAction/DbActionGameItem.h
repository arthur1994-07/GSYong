#pragma once

#include "../../../SigmaCore/String/MinGuid.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Msg/ServerMsg.h"

#include "DbAction.h"

struct NET_CHA_BBA_INFO_HA;

namespace db
{

//! �ű� ������ ����
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
	DWORD m_Channel;	// ������Ŀ�� ��쿡 ����
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
	DWORD m_Channel;	// ������Ŀ�� ��쿡 ����
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
	DWORD m_Channel;	// ������Ŀ�� ��쿡 ����
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

// ������ ���� ��� ĳ���͵��� �������� ���̳ʸ����� ������ ���̺�� �����Ѵ�.
// ������ ĳ���͵� ����� �ϳ�?
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

// ������ ��ġ ����(��������̳� ��ǥ)
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
	DWORD m_Channel;	// ������Ŀ�� ��쿡 ����
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

// ItemDel ���̺� ������ �����ϱ�
// Item ���̺��� ������ �� Ʈ���� �ɾ ������ �������� �ڵ����� ItemDel ���̺�� �����ϸ� ���� ���ϰ� �ֱ� ������
// �������� ���� �����ϱ� ���� ItemDel ���̺� ���� �־��ִ� ��� �߰�
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
	DWORD m_Channel;	// ������Ŀ�� ��쿡 ����
};

}