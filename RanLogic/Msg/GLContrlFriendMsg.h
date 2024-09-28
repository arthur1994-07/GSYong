#pragma once

#include "GLContrlBaseMsg.h"
#include "../SigmaCore/String/MinGuid.h"

//! 친구추가
enum EMFRIENDADD
{
	EMFRIENDADD_FAIL	 = 0, //! 일반 오류
	EMFRIENDADD_OK		 = 1, //! 추가됨
/*	EMFRIENDADD_REFUSE	 = 2, //! 거절*/
	EMFRIENDADD_ABSENCE	 = 3, //! 부재
    EMFRIENDADD_MAX      = 4, //! 친구등록 가능 개수 초과
};

enum EMFRIENDDEL
{
	EMFRIENDDEL_FAIL	= 0,	//	일반 오류.
	EMFRIENDDEL_OK		= 1,	//	삭제됨.
};

enum EMFRIENDBLOCK
{
	EMFRIENDBLOCK_FAIL	= 0,	//	일반 오류.
	EMFRIENDBLOCK_ON	= 1,	//	블럭조치.
	//EMFRIENDBLOCK_OFF	= 2,	//	블럭취소. //! 차단 해제는 친구 삭제를 의미한다.
};

enum EMFRIEND_DEL_FLAG
{
	EMFRIENDDEL_FLAG_NORMAL = 0,
	EMFRIENDDEL_FLAG_PARTY_BLOCK_OFF = 1,
};

enum EMWINDOW_TYPE
{
	EMFRIEND_WINDOW	= 0,	//	친구창
	EMCLUB_WINDOW	= 1,	//	클럽창
};

namespace GLMSG
{
	//#pragma pack(1)

	struct SNETPC_REQ_FRIENDLIST  : public NET_MSG_GENERIC
	{
		SNETPC_REQ_FRIENDLIST ()
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDLIST);
			nType = NET_MSG_REQ_FRIENDLIST;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDLIST)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_REQ_FRIENDADD : public NET_MSG_GENERIC
	{
		char szADD_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD(const char* FriendName)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDADD);
			nType = NET_MSG_REQ_FRIENDADD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDADD)<=NET_DATA_BUFSIZE);
			memset(szADD_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
            if (FriendName)
                StringCchCopyA(szADD_CHAR, CHR_ID_LENGTH+1, FriendName);
		}
	};

	struct SNETPC_REQ_FRIENDADD_LURE : public NET_MSG_GENERIC
	{
		char szREQ_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD_LURE(const char* Friend)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDADD_LURE);
			nType = NET_MSG_REQ_FRIENDADD_LURE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDADD_LURE)<=NET_DATA_BUFSIZE);
            memset(szREQ_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
            if (Friend)
                StringCchCopyA(szREQ_CHAR, CHR_ID_LENGTH+1, Friend);
		}
	};

	struct SNETPC_REQ_FRIENDADD_ANS : public NET_MSG_GENERIC
	{
		bool bOK;
		char szREQ_CHAR[CHR_ID_LENGTH+1];
        sc::MGUID Guid;

		SNETPC_REQ_FRIENDADD_ANS(bool _bOk, const char* Friend) 
			: bOK(_bOk)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDADD_ANS);
			nType = NET_MSG_REQ_FRIENDADD_ANS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDADD_ANS)<=NET_DATA_BUFSIZE);
			memset(szREQ_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
            if (Friend)
                StringCchCopyA(szREQ_CHAR, CHR_ID_LENGTH+1, Friend);

            Guid = sc::MGUID();
		}
	};

	struct SNETPC_REQ_FRIENDADD_FB : public NET_MSG_GENERIC
	{
		EMFRIENDADD emFB;
		char szADD_CHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDADD_FB(EMFRIENDADD _emFB, const char* Friend)
            : emFB(_emFB)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDADD_FB);
			nType = NET_MSG_REQ_FRIENDADD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDADD_FB)<=NET_DATA_BUFSIZE);
			memset(szADD_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
            if (Friend)
                StringCchCopyA(szADD_CHAR, CHR_ID_LENGTH+1, Friend);
		}
	};

	struct SNETPC_REQ_FRIENDDEL : public NET_MSG_GENERIC
	{
		char				szDEL_CHAR[CHR_ID_LENGTH+1];
		int					iDelFlag;

		SNETPC_REQ_FRIENDDEL ()
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDDEL);
			nType = NET_MSG_REQ_FRIENDDEL;
			iDelFlag = 0;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDDEL)<=NET_DATA_BUFSIZE);
			memset(szDEL_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDDEL_FB : public NET_MSG_GENERIC
	{
		EMFRIENDDEL			emFB;
		char				szDEL_CHAR[CHR_ID_LENGTH+1];
		int					iDelFlag;

		SNETPC_REQ_FRIENDDEL_FB (EMFRIENDDEL _emFB) 
			: emFB(_emFB)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDDEL_FB);
			nType = NET_MSG_REQ_FRIENDDEL_FB;
			iDelFlag = 0;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDDEL_FB)<=NET_DATA_BUFSIZE);
            memset(szDEL_CHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDBLOCK : public NET_MSG_GENERIC
	{
		char				szCHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDBLOCK () 
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDBLOCK);
			nType = NET_MSG_REQ_FRIENDBLOCK;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDBLOCK)<=NET_DATA_BUFSIZE);
			memset(szCHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};

	struct SNETPC_REQ_FRIENDBLOCK_FB : public NET_MSG_GENERIC
	{
		EMFRIENDBLOCK		emFB;
		char				szCHAR[CHR_ID_LENGTH+1];

		SNETPC_REQ_FRIENDBLOCK_FB (EMFRIENDBLOCK _emFB) :
			emFB(_emFB)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDBLOCK_FB);
			nType = NET_MSG_REQ_FRIENDBLOCK_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDBLOCK_FB)<=NET_DATA_BUFSIZE);
			memset(szCHAR, 0, sizeof(char) * (CHR_ID_LENGTH+1));
		}
	};
/*
	struct SNETPC_REQ_FRIENDINFO : public NET_MSG_GENERIC
	{
		SFRIEND sFRIEND;

		SNETPC_REQ_FRIENDINFO(const SFRIEND& Info)
            : sFRIEND(Info)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDINFO);
			nType = NET_MSG_FRIENDINFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDINFO)<=NET_DATA_BUFSIZE);
		}
	};
*/
	struct SNETPC_REQ_FRIENDSTATE //: public NET_MSG_GENERIC
	{		
		int					m_FriendFlag;
        int					m_Channel;
        std::string 		m_FriendName; //[CHR_ID_LENGTH+1];
		bool				m_Online;

        MSGPACK_DEFINE(m_FriendFlag, m_Channel, m_FriendName, m_Online);

        SNETPC_REQ_FRIENDSTATE()
            : m_FriendFlag(0)
            , m_Channel(0)
            , m_Online(false)
        {
        }

		SNETPC_REQ_FRIENDSTATE (int ChaFlag, bool bONLINE, int nCHANNEL) 
			: m_FriendFlag(ChaFlag)
			, m_Online(bONLINE)
			, m_Channel(nCHANNEL)
		{
            //MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDSTATE)<=NET_DATA_BUFSIZE);
			//dwSize = sizeof(SNETPC_REQ_FRIENDSTATE);
			//nType = NET_MSG_FRIENDSTATE;						
		}
	};
/*
	struct SNETPC_REQ_FRIENDPHONENUMBER_CLT : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		char				szName[CHAR_SZNAME];
		char				szNewPhoneNumber[SMS_RECEIVER];

		SNETPC_REQ_FRIENDPHONENUMBER_CLT (DWORD _dwGaeaID) 
			: dwGaeaID(_dwGaeaID)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDPHONENUMBER_CLT);
			nType = NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDPHONENUMBER_CLT)<=NET_DATA_BUFSIZE);
			memset(szName, 0, CHAR_SZNAME);
			memset(szNewPhoneNumber, 0, SMS_RECEIVER);
		}
	};
*/
	struct SNETPC_REQ_FRIENDRENAME_CLT : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		char				szOldName[CHAR_SZNAME];
		char				szNewName[CHAR_SZNAME];

		SNETPC_REQ_FRIENDRENAME_CLT (DWORD _dwGaeaID) 
			: dwGaeaID(_dwGaeaID)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDRENAME_CLT);
			nType = NET_MSG_GCTRL_FRIEND_RENAME_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDRENAME_CLT)<=NET_DATA_BUFSIZE);
			memset(szOldName, 0, CHAR_SZNAME);
			memset(szNewName, 0, CHAR_SZNAME);
		}
	};

	struct SNETPC_REQ_FRIENDRENAME_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		char				szOldName[CHAR_SZNAME];
		char				szNewName[CHAR_SZNAME];

		SNETPC_REQ_FRIENDRENAME_FLD (DWORD _dwGaeaID) 
			: dwGaeaID(_dwGaeaID)
		{
			dwSize = sizeof(SNETPC_REQ_FRIENDRENAME_FLD);
			nType = NET_MSG_GCTRL_FRIEND_RENAME_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIENDRENAME_FLD)<=NET_DATA_BUFSIZE);
			memset(szOldName, 0, CHAR_SZNAME);
			memset(szNewName, 0, CHAR_SZNAME);
		}
	};
/*
	struct SNETPC_REQ_FRIEND_CLUB_OPEN : public NET_MSG_GENERIC
	{
		EMWINDOW_TYPE		emTYPE;
		bool				bOpen;

		SNETPC_REQ_FRIEND_CLUB_OPEN (EMWINDOW_TYPE _emTYPE, bool _bOpen)
			: emTYPE(_emTYPE)
			, bOpen(_bOpen)
		{
			dwSize = sizeof(SNETPC_REQ_FRIEND_CLUB_OPEN);
			nType = NET_MSG_REQ_FRIEND_CLUB_OPEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FRIEND_CLUB_OPEN)<=NET_DATA_BUFSIZE);
		}
	};
*/
	// Revert to default structure packing
	//#pragma pack()
};