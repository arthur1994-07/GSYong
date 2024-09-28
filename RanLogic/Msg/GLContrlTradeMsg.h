#ifndef _GL_TRADE_MSG_H_
#define _GL_TRADE_MSG_H_

#pragma once

#include "GLContrlBaseMsg.h"

namespace GLMSG
{
//	#pragma pack(1)
	
    struct SNET_TRADE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		DWORD dwTargetID;
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입
		SNET_TRADE(DWORD _dwTargetID)
            : dwTargetID(_dwTargetID)
            , m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE);
			nType = NET_MSG_GCTRL_TRADE;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		DWORD dwTargetID;
		EMTRADE_ANS emAns;
		char szMasterName[CHAR_SZNAME];
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입

		SNET_TRADE_FB()
            : dwTargetID(0)
            , emAns(EMTRADE_OK)
            , m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE_FB);
			nType = NET_MSG_GCTRL_TRADE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_FB)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		DWORD dwMasterID;
		char szMasterName[CHAR_SZNAME];  
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입
        SNET_TRADE_TAR()
            : dwMasterID(0)
            , m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE_TAR);
			nType = NET_MSG_GCTRL_TRADE_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_TAR)<=NET_DATA_BUFSIZE);
		}
	};

	
	struct SNET_TRADE_TAR_ANS : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		DWORD dwMasterID;
		EMTRADE_ANS emAns;
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입

		SNET_TRADE_TAR_ANS()
            : dwMasterID(0)
            , emAns(EMTRADE_OK)
            , m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE_TAR_ANS);
			nType = NET_MSG_GCTRL_TRADE_TAR_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_TAR_ANS)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_AGREE : public NET_MSG_GENERIC
	{
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입
		SNET_TRADE_AGREE()
            : m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE_AGREE);
			nType = NET_MSG_GCTRL_TRADE_AGREE;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_AGREE)<=NET_DATA_BUFSIZE);
		}
	};

	
	struct SNET_TRADE_AGREE_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
        DWORD dwGaeaID;
		EMTRADE_AGREE_FB emFB;
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입
		
		SNET_TRADE_AGREE_TAR()
            : dwGaeaID(0)
            , emFB(EMTRADE_AGREE_OK)
            , m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE_AGREE_TAR);
			nType = NET_MSG_GCTRL_TRADE_AGREE_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_AGREE_TAR)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_LOCK : public NET_MSG_GENERIC
	{
		DWORD dwGaeaID;
        unsigned int m_Crc32; // 패킷조작 때문에 CRC32 삽입

		SNET_TRADE_LOCK()
            : dwGaeaID(0)
            , m_Crc32(0)
		{
			dwSize = sizeof(SNET_TRADE_LOCK);
			nType = NET_MSG_GCTRL_TRADE_LOCK;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_LOCK)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_LOCK_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD dwGaeaID;
		SNET_TRADE_LOCK_TAR()
            : dwGaeaID(0)
		{
			dwSize = sizeof(SNET_TRADE_LOCK_TAR);
			nType = NET_MSG_GCTRL_TRADE_LOCK_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_LOCK_TAR)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_ITEM_REGIST : public NET_MSG_GENERIC
	{
		WORD wPosX;
		WORD wPosY;
		WORD wInvenX;
		WORD wInvenY;

		SNET_TRADE_ITEM_REGIST()
            : wPosX(0)
            , wPosY(0)
            , wInvenX(0)
            , wInvenY(0)
		{
			dwSize = sizeof(SNET_TRADE_ITEM_REGIST);
			nType = NET_MSG_GCTRL_TRADE_ITEM_REGIST;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_ITEM_REGIST)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_ITEM_REGIST_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD dwGaeaID;
		SINVENITEM sInsert;

		SNET_TRADE_ITEM_REGIST_TAR()
            : dwGaeaID(0)
		{
			dwSize = sizeof(SNET_TRADE_ITEM_REGIST_TAR);
			nType = NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_ITEM_REGIST_TAR)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_ITEM_REMOVE : public NET_MSG_GENERIC
	{
		WORD wPosX;
		WORD wPosY;

		SNET_TRADE_ITEM_REMOVE()
            : wPosX(0)
            , wPosY(0)
		{
			dwSize = sizeof(SNET_TRADE_ITEM_REMOVE);
			nType = NET_MSG_GCTRL_TRADE_ITEM_REMOVE;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_ITEM_REMOVE)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_ITEM_REMOVE_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD dwGaeaID;
		WORD wPosX;
		WORD wPosY;

		SNET_TRADE_ITEM_REMOVE_TAR()
            : wPosX(0)
            , wPosY(0)
		{
			dwSize = sizeof(SNET_TRADE_ITEM_REMOVE_TAR);
			nType = NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_ITEM_REMOVE_TAR)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_MONEY : public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM=30342, };
        int m_MagicNum;
		LONGLONG lnMoney;

		SNET_TRADE_MONEY()
            : lnMoney(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(SNET_TRADE_MONEY);
			nType = NET_MSG_GCTRL_TRADE_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_MONEY)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_MONEY_TAR : public NET_MSG_GENERIC
	{
		DWORD dwGaeaID;
		LONGLONG lnMoney;

        SNET_TRADE_MONEY_TAR(DWORD _dwGaeaID, LONGLONG _lnMoney)
            : dwGaeaID(_dwGaeaID)
            , lnMoney(_lnMoney)
		{
			dwSize = sizeof(SNET_TRADE_MONEY_TAR);
			nType = NET_MSG_GCTRL_TRADE_MONEY_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_MONEY_TAR)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_COMPLETE_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_TRADE_COMPLETE_TAR()
		{
			dwSize = sizeof(SNET_TRADE_COMPLETE_TAR);
			nType = NET_MSG_GCTRL_TRADE_COMPLETE_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_COMPLETE_TAR)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_CANCEL : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_TRADE_CANCEL()
		{
			dwSize = sizeof(SNET_TRADE_CANCEL);
			nType = NET_MSG_GCTRL_TRADE_CANCEL;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_CANCEL)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRADE_CANCEL_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMTRADE_CANCEL_TAR emCancel;

		SNET_TRADE_CANCEL_TAR()
            : emCancel(EMTRADE_CANCEL_NORMAL)
		{
			dwSize = sizeof(SNET_TRADE_CANCEL_TAR);
			nType = NET_MSG_GCTRL_TRADE_CANCEL_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRADE_CANCEL_TAR)<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
//	#pragma pack()
} // namespace GLMSG

#endif // _GL_TRADE_MSG_H_