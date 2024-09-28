#pragma once

#include <vector>
#include <map>

#include "./Character/GLCharDefine.h"
#include "./Item/GLItemCustom.h"
#include "./Party/GLPartyDefine.h"

//! 링크 항목 좌우 심볼
static const CString strLINK_LEFT_SIMBOL = ( _T("<") );	
static const CString strLINK_RIGHT_SIMBOL = ( _T(">") );	

//	링크 타입 10을 넘으면 안됩니다.
//	넘으면 새로 짜야함.
enum LINK_TYPE
{
	LINK_NONE		= 0,
	ITEM_LINK		= 1,
	PARTY_LINK		= 2,
	QUEST_LINK		= 3,
	SKILL_LINK		= 4,
	CROW_LINK		= 5,
	MAP_LINK		= 6,	

	LINK_TYPE_SIZE	= 7,
};

// 아이템 링크
enum ITEM_LINK_TYPE
{
	ITEM_LINK_NONE			= 0,
	ITEM_LINK_INVEN			= 1,
    ITEM_LINK_PUTON			= 2,
	ITEM_LINK_USER_STORAGE	= 3, 
	ITEM_LINK_CLUB_STORAGE	= 4,
	ITEM_LINK_CROW_SALE		= 5, 
	ITEM_LINK_NORMAL		= 6,	// SITEMCUSTOM이 SLINK_DATA_BASIC을 사용하지 않고 따로 전달 될 경우 사용

	ITEM_LINK_TYPE_SIZE		= 7,
};

// 파티 링크
enum PARTY_LINK_TYPE
{
	PARTY_LINK_NONE = 0,
    PARTY_LINK_RECRUIT = 1,	// 모집 링크
	PARTY_LINK_ACCEPT = 2,  // 파티 신청 수락
	PARTY_LINK_REFUSE = 3,  // 파티 거절
};

enum {
    CHAT_LINK_MAX			 = 3,		  // 한번에 링크걸수 있는 항목
    CHAT_LINK_DATA_MAX		 = 200,		  // 각 항목별 링크정보 최대치
    CHAT_LINK_BASIC_DATA_MAX = 1000,	  // 기본 링크정보 최대치
    CHAT_LINK_INDEX_MAX	     = USHRT_MAX, // 링크 IndexMax치
    CHAT_LINK_DATA_SIZE	     = 768,		  // 최대 링크 데이터 사이즈
};


struct SLINK_TYPE
{
	WORD wMType;
	WORD wSType;

    MSGPACK_DEFINE(wMType, wSType);

	SLINK_TYPE()
		: wMType(LINK_NONE)
		, wSType(ITEM_LINK_NONE)
	{
	}

	void RESET()
	{
		wMType = LINK_NONE;
		wSType = ITEM_LINK_NONE;
	}

	bool VALID() const 
	{
		if (wMType == LINK_NONE || wSType == ITEM_LINK_NONE)
            return false;
        else		
		    return true;
	}
};

struct SLINK_DATA_POS
{
	int nStart;
	int nEnd;

	MSGPACK_DEFINE(nStart, nEnd);

	SLINK_DATA_POS()
		: nStart (0)
		, nEnd(0)
	{
	}

	void RESET()
	{
		nStart = 0;
		nEnd = 0;
	}

	void ADD( int nAdd ) 
	{
		nStart += nAdd;
		nEnd += nAdd;
	}
};

//	링크정보 ( 해당 링크에 대한 정보가 담겨 있다 ) 
struct SLINK_DATA_BASIC
{
	SLINK_TYPE		sLinkType;
	SLINK_DATA_POS	sLinkPos;
	DWORD dwData1;
	DWORD dwData2;

    MSGPACK_DEFINE(sLinkType, sLinkPos, dwData1, dwData2);

	SLINK_DATA_BASIC ()
		: dwData1 (0)
		, dwData2 (0)
	{
	}

	void RESET()
	{
		sLinkType.RESET();
		sLinkPos.RESET();
		dwData1 = 0;
		dwData2 = 0;		
	}

	bool VALID() const 
	{
		return sLinkType.VALID();
	}
};

// 링크 정보 ( 링크 데이터에 Index를 가지고 있다. ) 
struct SLINK_DATA_INDEX
{
	SLINK_TYPE		sLinkType;
	SLINK_DATA_POS	sLinkPos;
	int nLinkIndex;

    MSGPACK_DEFINE(sLinkType, sLinkPos, nLinkIndex);

	SLINK_DATA_INDEX ()
		: nLinkIndex (-1)
	{
	}

	void RESET()
	{
		sLinkType.RESET();
		sLinkPos.RESET();
		nLinkIndex = -1;
	}

	bool VALID() const 
	{
		return sLinkType.VALID();
	}
};

//	서버 -> 클라이언트
struct SLINK_DATA_RECV
{
	SLINK_TYPE		sLinkType;
	SLINK_DATA_POS	sLinkPos;
	DWORD dwSize;

    MSGPACK_DEFINE(sLinkType, sLinkPos, dwSize);

	SLINK_DATA_RECV()
		: dwSize(0)
	{
	}

	void RESET()
	{
		sLinkType.RESET();
		sLinkPos.RESET();
		dwSize = 0;
	}

	bool VALID() const 
	{
		if ( sLinkType.VALID() && dwSize > 0 ) return true;

		return false;
	}
};

struct SLINK_TEXT_DATA_EX
{
	SLINK_DATA_BASIC sLinkDataBasic;
	//!! 링크 위치는 SLINK_DATA_BASIC로 옮김.
	//!! 기존 소스 동작을 위해 일단 놔둠. 스케일폼 적용이 끝나면 삭제하자.
	SLINK_DATA_POS	 sLinkPos;
	DWORD	dwTextColor;
	SITEMCUSTOM sItemcustom;

    MSGPACK_DEFINE(sLinkDataBasic, sLinkPos, dwTextColor);

	SLINK_TEXT_DATA_EX()
		: dwTextColor(0)
	{
	}

	void RESET()
	{
		sLinkDataBasic.RESET();
		sLinkPos.RESET();
		dwTextColor = 0;
	}

	bool VALID() const 
	{
		return sLinkDataBasic.VALID();
	}

	bool operator < ( const SLINK_TEXT_DATA_EX& sLinkTextData )
	{
		if ( sLinkPos.nStart < sLinkTextData.sLinkPos.nStart ) return true;
		
		return false;
	}		
};

struct SLINK_TEXT_DATA
{
	SLINK_DATA_POS	sLinkPos;
	DWORD			dwTextColor;
	int				nLinkTypeIndex;		// 링크타입 Index

    MSGPACK_DEFINE(sLinkPos, dwTextColor, nLinkTypeIndex);

	SLINK_TEXT_DATA()
		: dwTextColor(0)
		, nLinkTypeIndex (UINT_MAX)
	{
	}

	void RESET()
	{
		sLinkPos.RESET();
		dwTextColor = 0;
		nLinkTypeIndex = 0;
	}

	bool VALID() const 
	{
		return nLinkTypeIndex != UINT_MAX;
	}

	bool operator < ( const SLINK_TEXT_DATA& sLinkTextData )
	{
		if ( sLinkPos.nStart < sLinkTextData.sLinkPos.nStart ) return true;
		
		return false;
	}		
};

struct SLINK_DATA_ITEM
{
	std::string strCharName;
	SITEMCUSTOM sItem;

    MSGPACK_DEFINE(strCharName, sItem);
};

typedef std::vector<SLINK_DATA_BASIC>		VEC_LINK_DATA_BASIC;
typedef VEC_LINK_DATA_BASIC::iterator		VEC_LINK_DATA_BASIC_ITER;

typedef std::vector<SLINK_TEXT_DATA_EX>		VEC_LINK_TEXT_DATA_EX;
typedef VEC_LINK_TEXT_DATA_EX::iterator		VEC_LINK_TEXT_DATA_EX_ITER;

typedef std::vector<SLINK_TEXT_DATA>		VEC_LINK_TEXT_DATA;
typedef VEC_LINK_TEXT_DATA::iterator		VEC_LINK_TEXT_DATA_ITER;
typedef VEC_LINK_TEXT_DATA::const_iterator	VEC_LINK_TEXT_DATA_CITER;

typedef std::vector<SLINK_DATA_POS>			VEC_LINK_DATA_POS;
typedef VEC_LINK_DATA_POS::iterator			VEC_LINK_DATA_POS_ITER;

typedef std::map<int,SLINK_DATA_INDEX>		MAP_LINK_DATA_INDEX;
typedef MAP_LINK_DATA_INDEX::iterator		MAP_LINK_DATA_INDEX_ITER;

typedef std::map<DWORD,SLINK_DATA_ITEM>		MAP_ITEM_LINK_DATA_INDEX;
typedef MAP_ITEM_LINK_DATA_INDEX::iterator	MAP_ITEM_LINK_DATA_INDEX_ITER;

typedef std::map<DWORD,GLPARTY_LINK>		MAP_PARTY_LINK_DATA_INDEX;
typedef MAP_PARTY_LINK_DATA_INDEX::iterator	MAP_PARTY_LINK_DATA_INDEX_ITER;

