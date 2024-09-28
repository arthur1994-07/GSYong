#pragma once

#include "GLContrlBaseMsg.h"

enum EMQUEST_COMPLETE_FB
{
	EMQUEST_COMPLETE_FB_FAIL	= 0,
	EMQUEST_COMPLETE_FB_OK		= 1,
	EMQUEST_COMPLETE_FB_NOINVEN	= 2,
};

namespace GLMSG
{
	//#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNET_QUEST_PROG_GIVEUP : public NET_MSG_GENERIC
	{
		DWORD   dwQUESTID;

		SNET_QUEST_PROG_GIVEUP (DWORD _dwQUESTID) :
			dwQUESTID(_dwQUESTID)
		{
			dwSize = sizeof(SNET_QUEST_PROG_GIVEUP);
			nType = NET_MSG_GCTRL_QUEST_PROG_GIVEUP;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_GIVEUP)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_QUEST_PROG_DEL : public NET_MSG_GENERIC
	{
		DWORD   dwQUESTID;

		SNET_QUEST_PROG_DEL (DWORD _dwQUESTID) :
			dwQUESTID(_dwQUESTID)
		{
			dwSize = sizeof(SNET_QUEST_PROG_DEL);
			nType = NET_MSG_GCTRL_QUEST_PROG_DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_DEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_STREAM : public NET_MSG_GENERIC
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD)-sizeof(DWORD)-sizeof(DWORD)-sizeof(BOOL), };

		DWORD				dwQID;
		DWORD				bLastSeq;
		DWORD				dwSeq;

		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_PROG_STREAM (DWORD _dwQID, DWORD _dwLengthStream) :
			dwQID(_dwQID),
			bLastSeq(FALSE),
			dwSeq(0),
			dwLengthStream(_dwLengthStream)
		{
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL);
			nType = NET_MSG_GCTRL_QUEST_PROG_STREAM;
            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_STREAM)<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD)  + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL) + sizeof(BYTE)*dwLengthStream;

			return true;
		}
	};

	struct SNET_QUEST_END_DEL : public NET_MSG_GENERIC
	{
		DWORD				dwQUESTID;

		SNET_QUEST_END_DEL (DWORD _dwQUESTID) :
			dwQUESTID(_dwQUESTID)
		{
			dwSize = sizeof(SNET_QUEST_END_DEL);
			nType = NET_MSG_GCTRL_QUEST_END_DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_END_DEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_END_STREAM : public NET_MSG_GENERIC
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD)-sizeof(DWORD)-sizeof(DWORD)-sizeof(BOOL), };

		DWORD				dwQID;
		DWORD				bLastSeq;
		DWORD				dwSeq;

		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_END_STREAM (DWORD _dwQID, DWORD _dwLengthStream) :
			dwQID(_dwQID),
			bLastSeq(FALSE),
			dwSeq(0),
			dwLengthStream(_dwLengthStream)
		{
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL);
            nType = NET_MSG_GCTRL_QUEST_END_STREAM;
            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_END_STREAM)<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

            dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL) + sizeof(BYTE)*dwLengthStream;
            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_END_STREAM)<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_PROG_STEP_STREAM : public NET_MSG_GENERIC
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)
			-sizeof(DWORD)-sizeof(DWORD)-sizeof(DWORD) -sizeof(DWORD) -sizeof(DWORD) -sizeof(BOOL), };

		DWORD				dwQID;
		DWORD				dwQSTEP;
		DWORD				dwFlag;

		BOOL				bLastSeq;
		DWORD				dwSeq; // 0이면 분할전송 안함. 1부터는 분할전송 

		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_PROG_STEP_STREAM (DWORD _dwQID, DWORD _dwQSTEP, DWORD _dwFlag, DWORD _dwLengthStream) :
			dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),
			dwFlag(_dwFlag),
			bLastSeq(FALSE),
			dwSeq(0),
			dwLengthStream(_dwLengthStream)
		{
			dwSize = sizeof(NET_MSG_GENERIC) +sizeof(DWORD)+sizeof(DWORD)+sizeof(DWORD) +sizeof(DWORD) +sizeof(DWORD) +sizeof(BOOL);
            nType = NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM;
            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_STEP_STREAM)<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL) + sizeof(BYTE)*dwLengthStream;

            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_STEP_STREAM)<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_PROG_INVEN : public NET_MSG_GENERIC
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC) -sizeof(DWORD) -sizeof(DWORD) -sizeof(DWORD) -sizeof(BOOL), };

		DWORD				dwQID;
		BOOL				bLastSeq;
		DWORD				dwSeq;
	
		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNET_QUEST_PROG_INVEN (DWORD _dwQID, DWORD _dwLengthStream) :
			dwQID(_dwQID),
			bLastSeq(FALSE),
			dwSeq(0),
			dwLengthStream(_dwLengthStream)
		{
			dwSize = sizeof(NET_MSG_GENERIC) +sizeof(DWORD) +sizeof(DWORD) +sizeof(DWORD) +sizeof(BOOL);
			nType = NET_MSG_GCTRL_QUEST_PROG_INVEN;
            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_INVEN)<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) +sizeof(DWORD) +sizeof(DWORD) + sizeof(BOOL) + sizeof(BYTE)*dwLengthStream;
            MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_INVEN)<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNET_QUEST_STEP_NPC_TALK : public NET_MSG_GENERIC
	{
		DWORD				dwNpcGlobID;
		DWORD				dwTalkID;

		DWORD				dwQUESTID;

		SNET_QUEST_STEP_NPC_TALK (DWORD _dwNpcGlobID, DWORD _dwTalkID, DWORD _dwQUESTID) :
			dwNpcGlobID(_dwNpcGlobID),
			dwTalkID(_dwTalkID),
			dwQUESTID(_dwQUESTID)
		{
			dwSize = sizeof(SNET_QUEST_STEP_NPC_TALK);
			nType = NET_MSG_GCTRL_QUEST_PROG_NPCTALK;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_STEP_NPC_TALK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_STEP_NPC_TALK_FB : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		SNET_QUEST_STEP_NPC_TALK_FB (DWORD _dwQID, DWORD _dwQSTEP) :
			dwQID(_dwQID),
			dwQSTEP(_dwQSTEP)
		{
			dwSize = sizeof(SNET_QUEST_STEP_NPC_TALK_FB);
			nType = NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_STEP_NPC_TALK_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_MOBKILL : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bMOBKILL_ALL;
		DWORD				dwMOBKILL;

		SNET_QUEST_PROG_MOBKILL (DWORD _dwQID, DWORD _dwQSTEP, bool _bMOBKILL_ALL, DWORD _dwMOBKILL) :
			dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),
			bMOBKILL_ALL(_bMOBKILL_ALL),
			dwMOBKILL(_dwMOBKILL)
		{
			dwSize = sizeof(SNET_QUEST_PROG_MOBKILL);
			nType = NET_MSG_GCTRL_QUEST_PROG_MOBKILL;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_MOBKILL)<=NET_DATA_BUFSIZE);
		}
	};
	struct SNET_QUEST_PARTY_PROG_MOBKILL : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bMOBKILL_ALL;

		SNET_QUEST_PARTY_PROG_MOBKILL (DWORD _dwQID, DWORD _dwQSTEP, bool _bMOBKILL_ALL) :
		dwQID(_dwQID),
		dwQSTEP(_dwQSTEP),
		bMOBKILL_ALL(_bMOBKILL_ALL)
		{
			dwSize = sizeof(SNET_QUEST_PARTY_PROG_MOBKILL);
			nType = NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PARTY_PROG_MOBKILL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_QITEM : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bQITEM;

		SNET_QUEST_PROG_QITEM (DWORD _dwQID, DWORD _dwQSTEP, bool _bQITEM) :
			dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),

			bQITEM(_bQITEM)
		{
			dwSize = sizeof(SNET_QUEST_PROG_QITEM);
			nType = NET_MSG_GCTRL_QUEST_PROG_QITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_QITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PARTY_PROG_QITEM : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bQITEM;

		SNET_QUEST_PARTY_PROG_QITEM (DWORD _dwQID, DWORD _dwQSTEP, bool _bQITEM) :
			dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),

			bQITEM(_bQITEM)
		{
			dwSize = sizeof(SNET_QUEST_PARTY_PROG_QITEM);
			nType = NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PARTY_PROG_QITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_ITEMUSE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bITEMUSE_ALL;
		DWORD				dwNUM_ITEMUSE;

		SNET_QUEST_PROG_ITEMUSE (DWORD _dwQID, DWORD _dwQSTEP, bool _bITEMUSE_ALL, DWORD _dwNUM_ITEMUSE) :
		dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),
			bITEMUSE_ALL(_bITEMUSE_ALL),
			dwNUM_ITEMUSE(_dwNUM_ITEMUSE)
		{
			dwSize = sizeof(SNET_QUEST_PROG_ITEMUSE);
			nType = NET_MSG_GCTRL_QUEST_PROG_ITEMUSE;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_ITEMUSE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_CONFTING : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bCONFTING_ALL;
		DWORD				dwNUM_CONFTING;

		SNET_QUEST_PROG_CONFTING (DWORD _dwQID, DWORD _dwQSTEP, bool _bCONFTING_ALL, DWORD _dwNUM_CONFTING) :
		dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),
			bCONFTING_ALL(_bCONFTING_ALL),
			dwNUM_CONFTING(_dwNUM_CONFTING)
		{
			dwSize = sizeof(SNET_QUEST_PROG_CONFTING);
			nType = NET_MSG_GCTRL_QUEST_PROG_CONFTING;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_CONFTING)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_ITEMGRINDING : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bITEMGRINDING_ALL;
		DWORD				dwNUM_ITEMGRINDING;

		SNET_QUEST_PROG_ITEMGRINDING (DWORD _dwQID, DWORD _dwQSTEP, bool _bITEMGRINDING_ALL, DWORD _dwNUM_ITEMGRINDING) :
		dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),
			bITEMGRINDING_ALL(_bITEMGRINDING_ALL),
			dwNUM_ITEMGRINDING(_dwNUM_ITEMGRINDING)
		{
			dwSize = sizeof(SNET_QUEST_PROG_ITEMGRINDING);
			nType = NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_ITEMGRINDING)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_LEVEL_UP : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bLEVELUP_ALL;
		DWORD				dwNUM_LEVELUP;

		SNET_QUEST_PROG_LEVEL_UP (DWORD _dwQID, DWORD _dwQSTEP, bool _bLEVELUP_ALL, DWORD _dwNUM_LEVELUP) :
		dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),
			bLEVELUP_ALL(_bLEVELUP_ALL),
			dwNUM_LEVELUP(_dwNUM_LEVELUP)
		{
			dwSize = sizeof(SNET_QUEST_PROG_LEVEL_UP);
			nType = NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_LEVEL_UP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_QUEST_PROG_REACHZONE : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		DWORD				dwQSTEP;

		bool				bREACHZONE;

		SNET_QUEST_PROG_REACHZONE (DWORD _dwQID, DWORD _dwQSTEP, bool _bREACHZONE) :
			dwQID(_dwQID),
			dwQSTEP(_dwQSTEP),

			bREACHZONE(_bREACHZONE)
		{
			dwSize = sizeof(SNET_QUEST_PROG_REACHZONE);
			nType = NET_MSG_GCTRL_QUEST_PROG_REACHZONE;
			MIN_STATIC_ASSERT(sizeof(SNET_QUEST_PROG_REACHZONE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_QUEST_PROG_INVEN_TURNNUM : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		WORD				wPosX;
		WORD				wPosY;

		WORD				wTurnNum;
		
		SNETPC_QUEST_PROG_INVEN_TURNNUM (DWORD _dwQID, WORD _wPosX, WORD _wPosY, WORD _wTurnNum) :
			dwQID(_dwQID),
			wPosX(_wPosX),
			wPosY(_wPosY),

			wTurnNum(_wTurnNum)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_INVEN_TURNNUM);
			nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_INVEN_TURNNUM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_INVEN_PICKUP : public NET_MSG_GENERIC
	{
		SNATIVEID			sNID_ITEM;
		
		SNETPC_QUEST_PROG_INVEN_PICKUP ( SNATIVEID& _sNID_ITEM ) :
			sNID_ITEM(_sNID_ITEM)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_INVEN_PICKUP);
			nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_INVEN_PICKUP)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_QUEST_PROG_INVEN_INSERT : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		SINVENITEM			Data;

		SNETPC_QUEST_PROG_INVEN_INSERT (DWORD _dwQID, SINVENITEM& _Data) :
			dwQID(_dwQID),
            Data(_Data)

		{
			dwSize = sizeof(SNETPC_QUEST_PROG_INVEN_INSERT);
			nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_INVEN_INSERT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_QUEST_PROG_INVEN_DELETE : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_QUEST_PROG_INVEN_DELETE (DWORD _dwQID, WORD _wPosX, WORD _wPosY) :
			dwQID(_dwQID),
			wPosX(_wPosX),
			wPosY(_wPosY)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_INVEN_DELETE);
			nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_INVEN_DELETE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_READ : public NET_MSG_GENERIC
	{
		DWORD				dwQID;

		SNETPC_QUEST_PROG_READ (DWORD _dwQID) :
			dwQID(_dwQID)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_READ);
			nType = NET_MSG_GCTRL_QUEST_PROG_READ;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_READ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_TIME : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		float				fELAPS;

		SNETPC_QUEST_PROG_TIME (DWORD _dwQID, float _fELAPS) :
			dwQID(_dwQID),
			fELAPS(_fELAPS)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_TIME);
			nType = NET_MSG_GCTRL_QUEST_PROG_TIME;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_TIME)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_TIMEOVER : public NET_MSG_GENERIC
	{
		DWORD				dwQID;

		SNETPC_QUEST_PROG_TIMEOVER (DWORD _dwQID) :
			dwQID(_dwQID)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_TIMEOVER);
			nType = NET_MSG_GCTRL_QUEST_PROG_TIMEOVER;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_TIMEOVER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_NONDIE : public NET_MSG_GENERIC
	{
		DWORD				dwQID;

		SNETPC_QUEST_PROG_NONDIE (DWORD _dwQID) :
			dwQID(_dwQID)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_NONDIE);
			nType = NET_MSG_GCTRL_QUEST_PROG_NONDIE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_NONDIE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_LEAVEMAP : public NET_MSG_GENERIC
	{
		DWORD				dwQID;

		SNETPC_QUEST_PROG_LEAVEMAP (DWORD _dwQID) :
		dwQID(_dwQID)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_LEAVEMAP);
			nType = NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_LEAVEMAP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_PROG_LEVEL : public NET_MSG_GENERIC
	{
		DWORD				dwQID;

		SNETPC_QUEST_PROG_LEVEL (DWORD _dwQID) :
			dwQID(_dwQID)
		{
			dwSize = sizeof(SNETPC_QUEST_PROG_LEVEL);
			nType = NET_MSG_GCTRL_QUEST_PROG_LEVEL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_PROG_LEVEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QUEST_COMPLETE_FB : public NET_MSG_GENERIC
	{
		DWORD				dwQID;
		EMQUEST_COMPLETE_FB	emFB;

		SNETPC_QUEST_COMPLETE_FB (DWORD _dwQID, EMQUEST_COMPLETE_FB	_emFB) :
			dwQID(_dwQID),
			emFB(_emFB)
		{
			dwSize = sizeof(SNETPC_QUEST_COMPLETE_FB);
			nType = NET_MSG_GCTRL_QUEST_COMPLETE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QUEST_COMPLETE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_QUEST_COMPLETE : public NET_MSG_GENERIC
	{
		DWORD				dwQID;

		SNETPC_REQ_QUEST_COMPLETE (DWORD _dwQID) :
			dwQID(_dwQID)
		{
			dwSize = sizeof(SNETPC_REQ_QUEST_COMPLETE);
			nType = NET_MSG_GCTRL_REQ_QUEST_COMPLETE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_QUEST_COMPLETE)<=NET_DATA_BUFSIZE);
		}
	};

	//struct SNETPC_RNATTENDANCE_QUEST_LIST_FC
	//{
	//	std::vector<DWORD> vecQuestIDList;
	//	MSGPACK_DEFINE(vecQuestIDList);
	//	//NET_MSG_RNATTENDANCE_QUEST_LIST_FC
	//	SNETPC_RNATTENDANCE_QUEST_LIST_FC()
	//	{}
	//};

	// Revert to default structure packing
	//#pragma pack()
};