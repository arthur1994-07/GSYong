/**
 * \date	2012/04/12
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <string>
#include <map>
#include <boost/tr1/memory.hpp>

#include "../../SigmaCore/File/SerialFile.h"

#include "../../RanLogic/Item/GLItemCustom.h"

class GLGaeaClient;

namespace WishList
{
	enum
	{
		ESlotX	= 6,
		ESlotY	= 10,
		EPage	= 5,

		ETotalLine	= ESlotY * EPage,
		ETotalSlot	= ESlotX * ETotalLine,

		ETypeNpcStore = 1,
		ETypePointStore,
		ETypePrivatePerson,
		ETypeChatLink,
		ETypeActivate,
		ETypeTrade,
		ETypeReferChar,
		ETypeInventory,
		ETypeBank,
		ETypeLocker,
		ETypeNpcStoreEx,

	};

	struct SItemBase
	{
		enum
		{
			Version = 1,
		};

		SITEMCUSTOM m_sItem;

		SItemBase();
		SItemBase( const SITEMCUSTOM& sItem );
		virtual ~SItemBase() {}

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return 0; }
	};

	struct SItemNpcStore : public SItemBase
	{
		enum
		{
			Version   = 1,
		};

		SNATIVEID m_sCrowID;
		SNATIVEID m_sMapID;
		int m_nMapX;
		int m_nMapY;
		LONGLONG m_nPrice;

		SItemNpcStore();
		SItemNpcStore( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeNpcStore; }
	};

	struct SItemNpcExStore : public SItemBase// : public SItemNpcStore
	{
		enum
		{
			Version = 1,
		};

		SNATIVEID m_sCrowID;
		SNATIVEID m_sMapID;
		int m_nMapX;
		int m_nMapY;
		LONGLONG m_nPrice;

		int m_iX;
		int m_iY;
		int m_iPage;
		
		SItemNpcExStore();
		SItemNpcExStore( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice, int iPosX, int iPosY, int iPage );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeNpcStoreEx; }
	};

	struct SItemPointStore : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		LONGLONG m_nPrice;

		SItemPointStore();
		SItemPointStore( const SITEMCUSTOM& sItem, LONGLONG nPrice );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypePointStore; }
	};

	struct SItemPrivatePerson : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		LONGLONG m_nPrice;
		std::string m_strCharName;

		SItemPrivatePerson();
		SItemPrivatePerson( const SITEMCUSTOM& sItem, LONGLONG nPrice, std::string& strCharName );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypePrivatePerson; }
	};

	struct SItemChatLink : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		std::string m_strCharName;

		SItemChatLink();
		SItemChatLink( const SITEMCUSTOM& sItem, std::string& strCharName );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeChatLink; }
	};

	struct SItemActivate : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		LONGLONG m_nPrice;

		SItemActivate();
		SItemActivate( const SITEMCUSTOM& sItem, LONGLONG nPrice );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeActivate; }
	};

	struct SItemTrade : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		std::string m_strCharName;

		SItemTrade();
		SItemTrade( const SITEMCUSTOM& sItem, std::string& strCharName );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeTrade; }
	};

	struct SItemReferChar : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		std::string m_strCharName;

		SItemReferChar();
		SItemReferChar( const SITEMCUSTOM& sItem, std::string& strCharName );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeReferChar; }
	};

	struct SItemInventory : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		SItemInventory();
		SItemInventory( const SITEMCUSTOM& sItem );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeInventory; }
	};

	struct SItemBank : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		SItemBank();
		SItemBank( const SITEMCUSTOM& sItem );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeBank; }
	};

	struct SItemLocker : public SItemBase
	{
		enum
		{
			Version = 1,
		};

		SItemLocker();
		SItemLocker( const SITEMCUSTOM& sItem );

		virtual void SaveFile( sc::SerialFile& SFile );
		virtual void LoadFile( sc::SerialFile& SFile );

		virtual BYTE GetType() { return ETypeLocker; }
	};

	typedef std::tr1::shared_ptr< SItemBase > ItemSPtr;
	typedef std::map< DWORD, ItemSPtr > ItemMap;
	typedef ItemMap::iterator ItemMapIter;
	typedef ItemMap::const_iterator ItemMapCIter;
	typedef ItemMap::value_type ItemMapValue;

	class CClient
	{
	private:
		enum
		{
			Version = 1,
		};

	private:
		GLGaeaClient* m_pGaeaClient;

		ItemMap m_mapItem;

	public:
		CClient( GLGaeaClient* pGaeaClient );
		~CClient();

	private:
		ItemSPtr NewItem( BYTE nType );
		DWORD GetEmptySlot();
		bool InsertItem( DWORD nSlot, ItemSPtr pItem );

	public:
		void Clear();
		void RemoveItem( DWORD nSlot );
		ItemSPtr GetItem( DWORD nSlot );

		bool NewNpcStoreItem( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice, DWORD nSlot = UINT_MAX );
		bool NewNpcExStoreItem( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice, int iPosX, int iPosY, int iPage, DWORD nSlot = UINT_MAX );

		bool NewPointStoreItem( const SITEMCUSTOM& sItem, LONGLONG nPrice, DWORD nSlot = UINT_MAX );
		bool NewPrivatePersonItem( const SITEMCUSTOM& sItem, LONGLONG nPrice, std::string& strCharName, DWORD nSlot = UINT_MAX );
		bool NewChatLinkItem( const SITEMCUSTOM& sItem, std::string& strCharName, DWORD nSlot = UINT_MAX );
		bool NewActivateItem( const SITEMCUSTOM& sItem, LONGLONG nPrice, DWORD nSlot = UINT_MAX );
		bool NewTradeItem( const SITEMCUSTOM& sItem, std::string& strCharName, DWORD nSlot = UINT_MAX );
		bool NewReferCharItem( const SITEMCUSTOM& sItem, std::string& strCharName, DWORD nSlot = UINT_MAX );
		bool NewInventoryItem( const SITEMCUSTOM& sItem, DWORD nSlot = UINT_MAX );
		bool NewBankItem( const SITEMCUSTOM& sItem, DWORD nSlot = UINT_MAX );
		bool NewLockerItem( const SITEMCUSTOM& sItem, DWORD nSlot = UINT_MAX );

	public:
		void SaveFile( WORD nServer, DWORD nCharDbNum );
		void LoadFile( WORD nServer, DWORD nCharDbNum );

	private:
		void SaveFile( sc::SerialFile& SFile );
		void LoadFile( sc::SerialFile& SFile, const TCHAR* pFileName );

	public:
		const ItemMap& GetItemMap() { return m_mapItem; }
	};
}