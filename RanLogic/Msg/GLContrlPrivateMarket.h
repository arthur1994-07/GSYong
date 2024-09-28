#pragma once

#include <string>
#include <vector>

#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "GLContrlBaseMsg.h"

enum EMPMARKET_REGITEM_FB
{
	EMPMARKET_REGITEM_FB_FAIL			= 0,	//	�Ϲ� ����.
	EMPMARKET_REGITEM_FB_OK				= 1,	//	����.

	EMPMARKET_REGITEM_FB_MAXNUM			= 2,	//	��� �ѵ� �ʰ�.
	EMPMARKET_REGITEM_FB_REGITEM		= 3,	//	�̹� ��ϵ� ������.
	EMPMARKET_REGITEM_FB_NOSALE			= 4,	//	�Ǹ� ����.
	EMPMARKET_REGITEM_FB_LIMITED_MONEY	= 5, // ��� �ݾ� �ѵ� �ʰ�.
};

enum EMPMARKET_OPEN_FB
{
	EMPMARKET_OPEN_FB_FAIL			= 0,	//	�Ϲݿ���.
	EMPMARKET_OPEN_FB_OK			= 1,	//	����.
	EMPMARKET_OPEN_FB_ALREADY		= 2,	//	�̹� ���µǾ� ����.
	EMPMARKET_OPEN_FB_EMPTY			= 3,	//	��ϵ� �������� ����.
	EMPMARKET_OPEN_FB_NOITEM		= 4,	//	�㰡�� ����.
};

enum EMPMARKET_BUY_FB
{
	EMPMARKET_BUY_FB_FAIL			= 0,	//	�Ϲݿ���.
	EMPMARKET_BUY_FB_OK				= 1,	//	����.
	EMPMARKET_BUY_FB_NUM			= 2,	//	���� ����.
	EMPMARKET_BUY_FB_LOWMONEY		= 3,	//	�� ����.
	EMPMARKET_BUY_FB_SOLD			= 4,	//	�̹� �Ǹ������ ��ǰ.
	EMPMARKET_BUY_FB_NOINVEN		= 5,	//	�κ� ���� ����.
	EMPMARKET_BUY_FB_NOTIME			= 6,	//	��ȿ �Ⱓ ����.
	EMPMARKET_BUY_FB_HOLD			= 7,	//	������ �ŷ� ���� ������.
};

namespace GLMSG
{
	//	#pragma pack(1)

	struct SNETPC_PMARKET_TITLE : public NET_MSG_GENERIC
	{
		char m_PrivateMarketTitle[CHAT_MSG_SIZE+1];

		SNETPC_PMARKET_TITLE()
		{
			dwSize = sizeof(SNETPC_PMARKET_TITLE);
			nType = NET_MSG_GCTRL_PMARKET_TITLE;
			memset(m_PrivateMarketTitle, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_TITLE)<=NET_DATA_BUFSIZE);
		}

		void SetTitle(const std::string& Title)
		{
			StringCchCopy(m_PrivateMarketTitle, CHAT_MSG_SIZE+1, Title.c_str());
		}
	};

	struct SNETPC_PMARKET_TITLE_FB : public NET_MSG_GENERIC
	{
		char m_PrivateMarketTitle[CHAT_MSG_SIZE+1];

		SNETPC_PMARKET_TITLE_FB()
		{
			dwSize = sizeof(SNETPC_PMARKET_TITLE_FB);
			nType = NET_MSG_GCTRL_PMARKET_TITLE_FB;
			memset(m_PrivateMarketTitle, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_TITLE_FB)<=NET_DATA_BUFSIZE);			
		}

		void SetTitle(const std::string& Title)
		{
			StringCchCopy(m_PrivateMarketTitle, CHAT_MSG_SIZE+1, Title.c_str());
		}
	};

	struct SNETPC_PMARKET_REGITEM : public NET_MSG_GENERIC
	{
		WORD wPosX;
		WORD wPosY;		
		LONGLONG llMoney;
		DWORD dwNum;

		SNETPC_PMARKET_REGITEM() 
			: wPosX(0)
			, wPosY(0)
			, llMoney(0)
			, dwNum(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_REGITEM);
			nType = NET_MSG_GCTRL_PMARKET_REGITEM;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_REGITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_REGITEM_FB : public NET_MSG_GENERIC
	{
		EMPMARKET_REGITEM_FB emFB;
		WORD wPosX;
		WORD wPosY;		
		LONGLONG llMoney;
		DWORD dwNum;
		SNATIVEID sSALEPOS;

		SNETPC_PMARKET_REGITEM_FB()
			: emFB(EMPMARKET_REGITEM_FB_FAIL)
			, wPosX(0)
			, wPosY(0)
			, llMoney(0)
			, dwNum(0)
			, sSALEPOS(false)
		{
			dwSize = sizeof(SNETPC_PMARKET_REGITEM_FB);
			nType = NET_MSG_GCTRL_PMARKET_REGITEM_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_REGITEM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_DISITEM : public NET_MSG_GENERIC
	{
		SNATIVEID sSALEPOS;

		SNETPC_PMARKET_DISITEM()
			: sSALEPOS(false)
		{
			dwSize = sizeof(SNETPC_PMARKET_DISITEM);
			nType = NET_MSG_GCTRL_PMARKET_DISITEM;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_DISITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_DISITEM_FB : public NET_MSG_GENERIC
	{
		SNATIVEID sSALEPOS;

		SNETPC_PMARKET_DISITEM_FB()
			: sSALEPOS(false)
		{
			dwSize = sizeof(SNETPC_PMARKET_DISITEM_FB);
			nType = NET_MSG_GCTRL_PMARKET_DISITEM_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_DISITEM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_OPEN : public NET_MSG_GENERIC
	{
		WORD wPosX;
		WORD wPosY;

		SNETPC_PMARKET_OPEN()
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_OPEN);
			nType = NET_MSG_GCTRL_PMARKET_OPEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_OPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_OPEN_FB : public NET_MSG_GENERIC
	{
		EMPMARKET_OPEN_FB emFB;

		SNETPC_PMARKET_OPEN_FB()
			: emFB(EMPMARKET_OPEN_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_PMARKET_OPEN_FB);
			nType = NET_MSG_GCTRL_PMARKET_OPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_OPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_OPEN_BRD : public SNETPC_BROAD
	{
		char m_PrivateMarketTitle[CHAT_MSG_SIZE+1];

		SNETPC_PMARKET_OPEN_BRD()
		{
			dwSize = sizeof(SNETPC_PMARKET_OPEN_BRD);
			nType = NET_MSG_GCTRL_PMARKET_OPEN_BRD;
			memset(m_PrivateMarketTitle, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_OPEN_BRD)<=NET_DATA_BUFSIZE);
		}

		void SetTitle(const std::string& Title)
		{            
			StringCchCopy(m_PrivateMarketTitle, CHAT_MSG_SIZE+1, Title.c_str());
		}
	};

	struct SNETPC_PMARKET_CLOSE : public NET_MSG_GENERIC
	{		
		SNETPC_PMARKET_CLOSE()
		{
			dwSize = sizeof(SNETPC_PMARKET_CLOSE);
			nType = NET_MSG_GCTRL_PMARKET_CLOSE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_CLOSE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_CLOSE_BRD : public SNETPC_BROAD
	{
		SNETPC_PMARKET_CLOSE_BRD ()
		{
			dwSize = sizeof(SNETPC_PMARKET_CLOSE_BRD);
			nType = NET_MSG_GCTRL_PMARKET_CLOSE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_CLOSE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	// �����Ҷ� SNETPC_PMARKET_ITEM_IS_HOLD_FH ���� �����ؾ� �Ѵ�.
	struct SNETPC_PMARKET_BUY : public NET_MSG_GENERIC
	{
		DWORD dwGaeaID;
		SNATIVEID sSALEPOS;
		DWORD dwNum;

		SNETPC_PMARKET_BUY()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PMARKET_BUY;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_BUY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_BUY_FB : public NET_MSG_GENERIC
	{
		EMPMARKET_BUY_FB emFB;
		DWORD dwGaeaID;
		SNATIVEID sSALEPOS;
		DWORD dwNum;

		SNETPC_PMARKET_BUY_FB()
			: emFB(EMPMARKET_BUY_FB_FAIL)
			, dwGaeaID(0)
			, sSALEPOS(false)
			, dwNum(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_BUY_FB);
			nType = NET_MSG_GCTRL_PMARKET_BUY_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_BUY_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_ITEM_INFO : public NET_MSG_GENERIC
	{
		DWORD dwGaeaID;

		SNETPC_PMARKET_ITEM_INFO()
			: dwGaeaID(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_ITEM_INFO);
			nType = NET_MSG_GCTRL_PMARKET_ITEM_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_ITEM_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_ITEM_INFO_BRD : public SNETPC_BROAD
	{
		SNATIVEID	sSALEPOS;
		SNATIVEID	sINVENPOS;
		SITEMCUSTOM	sITEMCUSTOM;
		LONGLONG	llPRICE;
		DWORD		dwNUMBER;
		bool		bSOLD;

		SNETPC_PMARKET_ITEM_INFO_BRD()
			: llPRICE(0)
			, dwNUMBER(0)
			, bSOLD(false)
		{
			dwSize = sizeof(SNETPC_PMARKET_ITEM_INFO_BRD);
			nType = NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_ITEM_INFO_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_ITEM_UPDATE_BRD : public SNETPC_BROAD
	{
		SNATIVEID	sSALEPOS;
		DWORD		dwNUMBER;
		bool		bSOLD;

		SNETPC_PMARKET_ITEM_UPDATE_BRD()
			: dwNUMBER(0)
			, bSOLD(false)
		{
			dwSize = sizeof(SNETPC_PMARKET_ITEM_UPDATE_BRD);
			nType = NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_ITEM_UPDATE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEARCH_ITEM //: public NET_MSG_GENERIC
	{
		SSEARCHITEMDATA sSearchData;

		MSGPACK_DEFINE(sSearchData);

		SNETPC_PMARKET_SEARCH_ITEM()
		{
			//dwSize = sizeof(SNETPC_PMARKET_SEARCH_ITEM);
			//nType = NET_MSG_GCTRL_PMARKET_SEARCH_ITEM;
			//MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_SEARCH_ITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEARCH_ITEM_RESULT : public NET_MSG_GENERIC
	{
		DWORD dwSearchNum;
		SSEARCHITEMESULT sSearchResult[MAX_SEARCH_RESULT];
		DWORD dwPageNum;

		SNETPC_PMARKET_SEARCH_ITEM_RESULT()
			: dwSearchNum(0)
			, dwPageNum(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_SEARCH_ITEM_RESULT);
			nType = NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_SEARCH_ITEM_RESULT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ : public NET_MSG_GENERIC
	{
		DWORD dwPageNum;

		SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ()
			: dwPageNum(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ);
			nType = NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	// ���λ��� �Ǹ��� ���� ���(Field->Cache)
	struct SNETPC_PMARKET_SELLER_REGISTER_FH : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		SNATIVEID sMapID;
		WORD wSchool;
		int nChannel;
		char szChaName[CHAR_SZNAME];
		int nServerGroup;

		SNETPC_PMARKET_SELLER_REGISTER_FH()
			: dwChaDbNum( 0 )
			, sMapID( false )
			, wSchool( 0 )
			, nChannel( 0 )
			, nServerGroup( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_SELLER_REGISTER_FH );
			nType = NET_MSG_GCTRL_PMARKET_SELLER_REGISTER_FH;

			::memset( szChaName, 0x00, sizeof( char ) * CHAR_SZNAME );

			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_SELLER_REGISTER_FH ) <= NET_DATA_BUFSIZE );
		}

		void SetName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szChaName, CHAR_SZNAME, szName );
		}
	};

	// ���λ��� �Ǹ� ������ ���(Field->Cache)
	struct SNETPC_PMARKET_ITEM_REGISTER_FH : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		DWORD dwItemTurnNum;
		LONGLONG llItemPrice;
		UUID sItemGUID;
		SITEMCUSTOM sItemcustom;
		float fCommissionRate;
		int nServerGroup;
		SNETPC_PMARKET_ITEM_REGISTER_FH()
			: dwChaDbNum( 0 )
			, dwItemTurnNum( 0 )
			, llItemPrice( 0 )
			, fCommissionRate( 0 )
			, nServerGroup( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_REGISTER_FH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_REGISTER_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_REGISTER_FH ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �˻� ��û(Client->Field)
	struct SNETPC_PMARKET_ITEM_SEARCH_BASIC_CF : public NET_MSG_GENERIC
	{
		private_market::sSaleItemSearchandSortBasic sBasic;

		MSGPACK_DEFINE( sBasic );

		SNETPC_PMARKET_ITEM_SEARCH_BASIC_CF()
		{
		}

		SNETPC_PMARKET_ITEM_SEARCH_BASIC_CF( private_market::sSaleItemSearchandSortBasic _sBasic )
			: sBasic( _sBasic )
		{
		}
	};

	// ���λ��� �˻� ��û(Field->Cache)
	struct SNETPC_PMARKET_ITEM_SEARCH_BASIC_FH : public NET_MSG_GENERIC
	{
		private_market::sSaleItemSearchandSortBasic sBasic;

		int nChannel;
		WORD wSchool;

		DWORD dwChaDbNum;

		int nServerGroup;

		MSGPACK_DEFINE( sBasic, nChannel, wSchool, dwChaDbNum, nServerGroup );

		SNETPC_PMARKET_ITEM_SEARCH_BASIC_FH()
			: nChannel( 0 )
			, wSchool( 0 )
			, dwChaDbNum( 0 )
			, nServerGroup( 0 )
		{
		}

		SNETPC_PMARKET_ITEM_SEARCH_BASIC_FH( private_market::sSaleItemSearchandSortBasic _sBasic, int _nChannel, WORD _wSchool, DWORD _dwChaDbNum, int _nServerGroup )
			: sBasic( _sBasic )
			, nChannel( _nChannel )
			, wSchool( _wSchool )
			, dwChaDbNum( _dwChaDbNum )
			, nServerGroup( _nServerGroup )
		{
		}
	};

	// ���λ��� �˻� ��û(������ �̸�����)(Client->Field)
	struct SNETPC_PMARKET_ITEM_SEARCH_NAME_CF : public NET_MSG_GENERIC
	{
		char szItemName[MAP_NAME_LENGTH];
		WORD wPage;
		int nSortOption;
		int nSortMethod;

		SNETPC_PMARKET_ITEM_SEARCH_NAME_CF()
			: wPage( 1 )
			, nSortOption( private_market::eItemSortOptionName )
			, nSortMethod( private_market::eItemSortMethodAsc )
		{
			::memset( szItemName, 0x00, sizeof( char ) * MAP_NAME_LENGTH );

			dwSize = sizeof( SNETPC_PMARKET_ITEM_SEARCH_NAME_CF );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_SEARCH_NAME_CF ) <= NET_DATA_BUFSIZE );
		}

		void SetName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szItemName, MAP_NAME_LENGTH, szName );
		}
	};

	// ���λ��� �˻� ��û(������ �̸�����)(Agent->Cache)
	struct SNETPC_PMARKET_ITEM_SEARCH_NAME_AH : public NET_MSG_GENERIC
	{
		char szItemName[MAP_NAME_LENGTH];
		WORD wPage;
		int nSortOption;
		int nSortMethod;

		int nChannel;
		WORD wSchool;

		DWORD dwChaDbNum;

		int nServerGroup;

		SNETPC_PMARKET_ITEM_SEARCH_NAME_AH()
			: wPage( 1 )
			, nSortOption( private_market::eItemSortOptionName )
			, nSortMethod( private_market::eItemSortMethodAsc )
			, nChannel( 0 )
			, wSchool( 0 )
			, dwChaDbNum( 0 )
			, nServerGroup( 0 )
		{
			::memset( szItemName, 0x00, sizeof( char ) * MAP_NAME_LENGTH );

			dwSize = sizeof( SNETPC_PMARKET_ITEM_SEARCH_NAME_AH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_AH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_SEARCH_NAME_AH ) <= NET_DATA_BUFSIZE );
		}

		void SetName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szItemName, MAP_NAME_LENGTH, szName );
		}
	};

	// ���λ��� �˻� ��� �� ������ ����(Cache->Field->Client)
	struct SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		WORD wItemTotalNum;

		SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC()
			: dwChaDbNum( 0 )
			, wItemTotalNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_NUM_TOTAL_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �˻� ��� ������ ����(Cache->Field->Client)
	struct SNETPC_PMARKET_ITEM_NUM_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		WORD wItemNum;

		SNETPC_PMARKET_ITEM_NUM_HFC()
			: dwChaDbNum( 0 )
			, wItemNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_NUM_HFC );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_NUM_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_NUM_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �˻� ���(Cache->Field->Client)
	struct SNETPC_PMARKET_ITEM_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		private_market::sSaleItem sItem;

		SNETPC_PMARKET_ITEM_HFC( DWORD _dwChaDbNum, const private_market::sSaleItem& _sItem )
			: dwChaDbNum( _dwChaDbNum )
			, sItem( _sItem )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_HFC );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �˻� ��� ��(Cache ->Field->Client)
	struct SNETPC_PMARKET_ITEM_END_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;

		SNETPC_PMARKET_ITEM_END_HFC()
			: dwChaDbNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_END_HFC );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_END_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_END_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ������ ���� ��û(Client->Field)
	struct SNETPC_PMARKET_ITEM_BUY_CF : public NET_MSG_GENERIC
	{
		DWORD dwItemTurnNum;
		UUID sItemGUID;
		LONGLONG llPrice;
        int nSellType;

		SNETPC_PMARKET_ITEM_BUY_CF()
			: dwItemTurnNum( 0 )
			, llPrice( 0 )
            , nSellType( private_market::eSellType_PrivateMarket )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_BUY_CF );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_BUY_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_BUY_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ������ ���� ��û(Field->Cache)
	struct SNETPC_PMARKET_ITEM_BUY_FH : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		DWORD dwItemTurnNum;
		UUID sItemGUID;
		char szName[CHAR_SZNAME];
		int nServerGroup;

		SNETPC_PMARKET_ITEM_BUY_FH()
			: dwChaDbNum( 0 )
			, dwItemTurnNum( 0 )
			, nServerGroup( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_BUY_FH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_BUY_FH;

			::memset( szName, 0x00, sizeof( char ) * CHAR_SZNAME );

			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_BUY_FH ) <= NET_DATA_BUFSIZE );
		}

		void SetName( const char* _szName )
		{
			if ( _szName )
				StringCchCopy( szName, CHAR_SZNAME, _szName );
		}
	};

	// ���λ��� ���Ŵ�� ����(Cache->Field)
	struct SNETPC_PMARKET_ITEM_PAY_MONEY_HF : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		LONGLONG llItemPriceTotal;
		UUID sItemGUID;

		SNETPC_PMARKET_ITEM_PAY_MONEY_HF()
			: dwChaDbNum( 0 )
			, llItemPriceTotal( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_PAY_MONEY_HF );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_HF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_PAY_MONEY_HF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���Ŵ�� ���� ���(Field->Cache)
	struct SNETPC_PMARKET_ITEM_PAY_MONEY_FH : public NET_MSG_GENERIC
	{
		char szBuyerName[CHAR_SZNAME];
		DWORD dwChaDbNum;
		LONGLONG llItemPriceTotal;
		UUID sItemGUID;
		int nResult;
        int nSellType;

		SNETPC_PMARKET_ITEM_PAY_MONEY_FH()
			: dwChaDbNum( 0 )
			, llItemPriceTotal( 0 )
			, nResult( 0 )
		{
			::memset( szBuyerName, 0x00, sizeof( char ) * CHAR_SZNAME );
			dwSize = sizeof( SNETPC_PMARKET_ITEM_PAY_MONEY_FH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_PAY_MONEY_FH ) <= NET_DATA_BUFSIZE );
		}

		void SetBuyerName( const char* _szBuyerName )
		{
			if ( _szBuyerName )
				StringCchCopy( szBuyerName, CHAR_SZNAME, _szBuyerName );
		}
	};

	// ���λ��� ���Ŵ�� �ѹ�(Cache->Field)
	struct SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		LONGLONG llItemPriceTotal;
		UUID sItemGUID;

		SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF()
			: dwChaDbNum( 0 )
			, llItemPriceTotal( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �Ǹ����� ������ ���� ó��(Cache->Field)
	struct SNETPC_PMARKET_ITEM_BUY_HF : public NET_MSG_GENERIC
	{
		char szBuyerName[CHAR_SZNAME];
		DWORD dwSellerChaDbNum;
		DWORD dwItemTurnNum;
		DWORD dwBuyerChaDbNum;
		LONGLONG llItemPrice;
		UUID sItemGUID;

		SNETPC_PMARKET_ITEM_BUY_HF()
			: dwSellerChaDbNum( 0 )
			, dwItemTurnNum( 0 )
			, dwBuyerChaDbNum( 0 )
			, llItemPrice( 0 )
		{
			::memset( szBuyerName, 0x00, sizeof( char ) * CHAR_SZNAME );
			dwSize = sizeof( SNETPC_PMARKET_ITEM_BUY_HF );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_BUY_HF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_BUY_HF ) <= NET_DATA_BUFSIZE );
		}

		void SetBuyerName( const char* _szBuyerName )
		{
			if ( _szBuyerName )
				StringCchCopy( szBuyerName, CHAR_SZNAME, _szBuyerName );
		}
	};

	// ���λ��� �Ǹ����� ������ ���� ó�� �ѹ�(Field->Cache)
	struct SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH : public NET_MSG_GENERIC
	{
		DWORD dwSellerChaDbNum;
		DWORD dwItemTurnNum;
		DWORD dwBuyerChaDbNum;
		LONGLONG llItemPrice;
		UUID sItemGUID;

		SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH()
			: dwSellerChaDbNum( 0 )
			, dwItemTurnNum( 0 )
			, dwBuyerChaDbNum( 0 )
			, llItemPrice( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_BUY_ROLLBACK_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �������� �������� �� ���� �ִ� ������ ���� ����(Field->Cache)
	struct SNETPC_PMARKET_ITEM_BUY_RESULT_FH : public NET_MSG_GENERIC
	{
		DWORD dwItemTurnNum;
		UUID sItemGUID;
		bool bUpdateSaleItem;	// SaleItem ������Ʈ �ʿ�
		bool bLog;				// �˻��ý������� ������ ����̴�. �α� ���ܶ�
		SITEMCUSTOM AttachItem;	// �����ڿ��� ���� ���� �������̴�.
		char szBuyerName[CHAR_SZNAME];
		char szSellerName[CHAR_SZNAME];

		SNETPC_PMARKET_ITEM_BUY_RESULT_FH()
			: dwItemTurnNum( 0 )
			, bUpdateSaleItem( true )
			, bLog( false )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_BUY_RESULT_FH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_BUY_RESULT_FH;
			::memset( szBuyerName, 0x00, sizeof( char ) * CHAR_SZNAME );
			::memset( szSellerName, 0x00, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_BUY_RESULT_FH ) <= NET_DATA_BUFSIZE );
		}

		void SetBuyerName( const char* _szBuyerName )
		{
			if ( _szBuyerName )
				StringCchCopy( szBuyerName, CHAR_SZNAME, _szBuyerName );
		}

		void SetSellerName( const char* _szSellerName )
		{
			if ( _szSellerName )
				StringCchCopy( szSellerName, CHAR_SZNAME, _szSellerName );
		}
	};

	// ���λ��� ������ ���� ���� ��û, �������� �ʵ�� ���� ���� ���� �Ѵ�.(Cache->Field)
	struct SNETPC_PMARKET_POST_SEND_HF : public NET_MSG_GENERIC
	{
		UUID sItemGUID;
		DWORD dwBuyerChaDbNum;
		DWORD dwSellerChaDbNum;
		SITEMCUSTOM AttachItem;	// �����ڿ��� ���� ���� �������̴�.
		char szBuyerName[CHAR_SZNAME];
		char szSellerName[CHAR_SZNAME];

		SNETPC_PMARKET_POST_SEND_HF()
			: dwBuyerChaDbNum( 0 )
			, dwSellerChaDbNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_POST_SEND_HF );
			nType = NET_MSG_GCTRL_PMARKET_POST_SEND_HF;
			::memset( szBuyerName, 0x00, sizeof( char ) * CHAR_SZNAME );
			::memset( szSellerName, 0x00, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_POST_SEND_HF ) <= NET_DATA_BUFSIZE );
		}

		void SetBuyerName( const char* _szBuyerName )
		{
			if ( _szBuyerName )
				StringCchCopy( szBuyerName, CHAR_SZNAME, _szBuyerName );
		}

		void SetSellerName( const char* _szSellerName )
		{
			if ( _szSellerName )
				StringCchCopy( szSellerName, CHAR_SZNAME, _szSellerName );
		}
	};

	// ���λ��� Ȧ����������� ����(Field->Cache)
	struct SNETPC_PMARKET_ITEM_IS_HOLD_FH : public NET_MSG_GENERIC
	{
		// --- SNETPC_PMARKET_BUY
		DWORD dwGaeaID;
		SNATIVEID sSALEPOS;
		DWORD dwNum;
		// -- SNETPC_PMARKET_BUY

		DWORD dwReqChaDbNum;
		UUID sItemGUID;
		char szName[CHAR_SZNAME];
		int nServerGroup;

		SNETPC_PMARKET_ITEM_IS_HOLD_FH()
			: dwGaeaID( 0 )
			, dwNum( 0 )
			, dwReqChaDbNum( 0 )
			, nServerGroup( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_IS_HOLD_FH );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_FH;

			::memset( szName, 0x00, sizeof( char ) * CHAR_SZNAME );

			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_IS_HOLD_FH ) <= NET_DATA_BUFSIZE );
		}

		void SetName( const char* _szName )
		{
			if ( _szName )
				StringCchCopy( szName, CHAR_SZNAME, _szName );
		}
	};

	// ���λ��� Ȧ����������� ���� ����(Cache->Field)
	struct SNETPC_PMARKET_ITEM_IS_HOLD_HF : public NET_MSG_GENERIC
	{
		// --- SNETPC_PMARKET_BUY
		DWORD dwGaeaID;
		SNATIVEID sSALEPOS;
		DWORD dwNum;
		// -- SNETPC_PMARKET_BUY

		DWORD dwReqChaDbNum;
		UUID sItemGUID;
		int nResult;

		SNETPC_PMARKET_ITEM_IS_HOLD_HF()
			: dwGaeaID( 0 )
			, dwNum( 0 )
			, dwReqChaDbNum( 0 )
			, nResult( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_ITEM_IS_HOLD_HF );
			nType = NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_HF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_ITEM_IS_HOLD_HF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �Ϲ� FB (Server->Client)
	struct SNETPC_PMARKET_COMMON_FB : public NET_MSG_GENERIC
	{
		int nFeedback;
		DWORD dwChaDbNum;
		UUID sItemGUID;
		DWORD dwItemTurnNum;

		SNETPC_PMARKET_COMMON_FB()
			: nFeedback( private_market::eSuccess )
			, dwChaDbNum( 0 )
			, dwItemTurnNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_COMMON_FB );
			nType = NET_MSG_GCTRL_PMARKET_COMMON_FB;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_COMMON_FB ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �ŷ����� ��û(Client->Field)
	struct SNETPC_PMARKET_LOG_CF : public NET_MSG_GENERIC
	{
		SNETPC_PMARKET_LOG_CF()
		{
			dwSize = sizeof( SNETPC_PMARKET_LOG_CF );
			nType = NET_MSG_GCTRL_PMARKET_LOG_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_LOG_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �ŷ����� ��û(Agent->Cache)
	struct SNETPC_PMARKET_LOG_FH : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		int nServerGroup;

		SNETPC_PMARKET_LOG_FH()
			: dwChaDbNum( 0 )
			, nServerGroup( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_LOG_FH );
			nType = NET_MSG_GCTRL_PMARKET_LOG_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_LOG_FH ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �ŷ����� ����(Cache->Field->Client)
	struct SNETPC_PMARKET_LOG_NUM_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		WORD wLogNum;

		SNETPC_PMARKET_LOG_NUM_HFC()
			: dwChaDbNum( 0 )
			, wLogNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_LOG_NUM_HFC );
			nType = NET_MSG_GCTRL_PMARKET_LOG_NUM_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_LOG_NUM_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �ŷ�����(Cache->Field->Client)
	struct SNETPC_PMARKET_LOG_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		private_market::sSaleLog sLog;

		MSGPACK_DEFINE( dwChaDbNum, sLog );

		SNETPC_PMARKET_LOG_HFC()
			: dwChaDbNum( 0 )
		{
		}

		SNETPC_PMARKET_LOG_HFC( DWORD _dwChaDbNum, const private_market::sSaleLog& _sLog )
			: dwChaDbNum( _dwChaDbNum )
			, sLog( _sLog )
		{
		}
	};

	// ���λ��� �ŷ����� ��(Cache ->Field->Client)
	struct SNETPC_PMARKET_LOG_END_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;

		SNETPC_PMARKET_LOG_END_HFC()
			: dwChaDbNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_LOG_END_HFC );
			nType = NET_MSG_GCTRL_PMARKET_LOG_END_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_LOG_END_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���� ���� ���� ��û(Client->Field)
	struct SNETPC_PMARKET_STATE_CF : public NET_MSG_GENERIC
	{
		SNETPC_PMARKET_STATE_CF()
		{
			dwSize = sizeof( SNETPC_PMARKET_STATE_CF );
			nType = NET_MSG_GCTRL_PMARKET_STATE_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_STATE_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���� ���� ���� ��û(Field->Cache)
	struct SNETPC_PMARKET_STATE_FH : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;

		SNETPC_PMARKET_STATE_FH()
			: dwChaDbNum( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_STATE_FH );
			nType = NET_MSG_GCTRL_PMARKET_STATE_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_STATE_FH ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���� ���� ���� ����(Cache->Field->Client)
	struct SNETPC_PMARKET_STATE_HFC : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;

		float fSearchDelay;
		float fLogDelay;

		WORD wSearchItemNumberPerPage;
		WORD wLogNumberPerPage;
		WORD wLogNumberMaxPerCharacter;

		bool bCheckChannel;
		bool bCheckSchool;
		bool bSaleCommission;
		bool bAllPage;
		int nSearchOption;

		SNETPC_PMARKET_STATE_HFC()
			: fSearchDelay( 0.0f )
			, fLogDelay( 0.0f )
			, wSearchItemNumberPerPage( 0 )
			, wLogNumberPerPage( 0 )
			, wLogNumberMaxPerCharacter( 0 )
			, bCheckChannel( false )
			, bCheckSchool( false )
			, bSaleCommission( false )
			, bAllPage( false )
			, nSearchOption( -1 )
		{
			dwSize = sizeof( SNETPC_PMARKET_STATE_HFC );
			nType = NET_MSG_GCTRL_PMARKET_STATE_HFC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_STATE_HFC ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���� ���� ���� ��û(Client->Agent)
	struct SNETPC_PMARKET_OPTION_CA : public NET_MSG_GENERIC
	{
		private_market::eCommandOptionType eOptionType;
		float fValue;
		bool bValue;
		int SearchOption;

		SNETPC_PMARKET_OPTION_CA()
			: eOptionType( private_market::eCommandOptionTypeSearchDelay )
			, fValue( 0.0f )
			, bValue( false )
			, SearchOption( -1 )
		{
			dwSize = sizeof( SNETPC_PMARKET_OPTION_CA );
			nType = NET_MSG_GCTRL_PMARKET_OPTION_CA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_OPTION_CA ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���� ���� ���� ��û(Field->Cache)
	struct SNETPC_PMARKET_OPTION_FH : public NET_MSG_GENERIC
	{
		private_market::eCommandOptionType eOptionType;
		float fValue;
		bool bValue;
		int SearchOption;

		SNETPC_PMARKET_OPTION_FH()
			: eOptionType( private_market::eCommandOptionTypeSearchDelay )
			, fValue( 0.0f )
			, bValue( false )
			, SearchOption( -1 )
		{
			dwSize = sizeof( SNETPC_PMARKET_OPTION_FH );
			nType = NET_MSG_GCTRL_PMARKET_OPTION_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_OPTION_FH ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� ���� ���� ��û(Agent->Field)
	struct SNETPC_PMARKET_OPTION_AF : public NET_MSG_GENERIC
	{
		DWORD ChaNum;
		private_market::eCommandOptionType eOptionType;
		float fValue;
		bool bValue;
		int SearchOption;

		SNETPC_PMARKET_OPTION_AF()
			: ChaNum( 0 )
			, eOptionType( private_market::eCommandOptionTypeSearchDelay )
			, fValue( 0.0f )
			, bValue( false )
			, SearchOption( -1 )
		{
			dwSize = sizeof( SNETPC_PMARKET_OPTION_AF );
			nType = NET_MSG_GCTRL_PMARKET_OPTION_AF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_OPTION_AF ) <= NET_DATA_BUFSIZE );
		}
	};

	// ���λ��� �ݴ´�.(Field->Cache)
	struct SNETPC_PMARKET_CLOSE_FH : public NET_MSG_GENERIC
	{
		DWORD dwChaDbNum;
		int nServerGroup;

		SNETPC_PMARKET_CLOSE_FH()
			: dwChaDbNum( 0 )
			, nServerGroup( 0 )
		{
			dwSize = sizeof( SNETPC_PMARKET_CLOSE_FH );
			nType = NET_MSG_GCTRL_PMARKET_CLOSE_FH;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_CLOSE_FH ) <= NET_DATA_BUFSIZE );
		}
	};

	//���λ��� �˻��ý��� TEST
	struct SNETPC_PMARKET_REGITEM_FH : public NET_MSG_GENERIC
	{
		DWORD dwCharDbNum;
		char Itemname[MAP_NAME_LENGTH+1];
		SSALEITEM sItemdata;

		SNETPC_PMARKET_REGITEM_FH() 
			: dwCharDbNum(0)
		{
			memset(Itemname, 0, sizeof(char) * (MAP_NAME_LENGTH+1));
			dwSize = sizeof(SNETPC_PMARKET_REGITEM_FH);
			nType = NET_MSG_GCTRL_PMARKET_REGITEM_FH;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_REGITEM_FH)<=NET_DATA_BUFSIZE);
		}
		void SetName(const char* Name)
		{
			if (Name)
				StringCchCopy(Itemname, MAP_NAME_LENGTH+1, Name);
		}
	};

	struct SNETPC_PMARKET_SEARCH_REQ_CAH : public NET_MSG_GENERIC
	{
		DWORD dwcharDbnum;
		DWORD dwGaeaID;

		SNETPC_PMARKET_SEARCH_REQ_CAH()
			: dwcharDbnum(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_SEARCH_REQ_CAH);
			nType = NET_MSG_GCTRL_PMARKET_SEARCH_REQ_CAH;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_SEARCH_REQ_CAH)<=NET_DATA_BUFSIZE);
		}
	};



	struct SNETPC_PMARKET_SEARCH_RESULT_HAC : public NET_MSG_GENERIC
	{
		DWORD dwcharDbnum;
		DWORD dwGaeaID;

		SNETPC_PMARKET_SEARCH_RESULT_HAC()
			: dwcharDbnum(0)
			, dwGaeaID(0)
		{
			dwSize = sizeof(SNETPC_PMARKET_SEARCH_RESULT_HAC);
			nType = NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HAC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PMARKET_SEARCH_RESULT_HAC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PMARKET_SEND_DATA_FF : public NET_MSG_GENERIC
	{
		SNETPC_PMARKET_SEND_DATA_FF()
		{
			dwSize = sizeof( SNETPC_PMARKET_SEND_DATA_FF );
			nType = NET_MSG_GCTRL_PMARKET_SEND_DATA_FF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PMARKET_SEND_DATA_FF ) <= NET_DATA_BUFSIZE );
		}
	};

	// Revert to default structure packing
	//	#pragma pack()
}