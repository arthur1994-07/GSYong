#ifndef _GL_CHARACTER_MSG_H_
#define _GL_CHARACTER_MSG_H_

#pragma once

#include "../../SigmaCore/gassert.h"

#include "../Character/GLCharDefine.h"
#include "../Character/CharNameCacheDefine.h"
#include "../PointShop/PointShopDefine.h"
#include "../s_NetGlobal.h"

//! Database->Cache Server:Character Point 정보
struct NET_CHAR_POINT_DH : public NET_MSG_GENERIC
{
    int m_ChaDbNum;
    CHARACTER_POINT m_Point;

    NET_CHAR_POINT_DH(int ChaDbNum, const CHARACTER_POINT& Point)
        : m_ChaDbNum(ChaDbNum)
        , m_Point(Point)
    {
        dwSize = sizeof(NET_CHAR_POINT_DH);
        nType = NET_MSG_CHAR_POINT_DH;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POINT_DH)<=NET_DATA_BUFSIZE);
    }

    inline int ChaDbNum() const { return m_ChaDbNum; }
    inline CHARACTER_POINT Point() const { return m_Point; }
};

//! Cache->Field:Character Point 정보
struct NET_CHAR_POINT_HF : public NET_CHAR_POINT_DH
{
    NET_CHAR_POINT_HF(int ChaDbNum, const CHARACTER_POINT& Point)
        : NET_CHAR_POINT_DH(ChaDbNum, Point)
    {
        dwSize = sizeof(NET_CHAR_POINT_HF);
        nType = NET_MSG_CHAR_POINT_HF;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POINT_HF)<=NET_DATA_BUFSIZE);
    }
};

//! Field->Agent:Character Point 정보
struct NET_CHAR_POINT_FA : public NET_MSG_GENERIC
{
	DWORD m_ChaDbNum;
    CHARACTER_POINT m_Point;

    NET_CHAR_POINT_FA(DWORD ChaDbNum, const CHARACTER_POINT& Point)
        : m_ChaDbNum(ChaDbNum)
		, m_Point(Point)
    {
        dwSize = sizeof(NET_CHAR_POINT_FA);
        nType = NET_MSG_CHAR_POINT_FA;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POINT_FA)<=NET_DATA_BUFSIZE);
    }

    inline CHARACTER_POINT Point() const { return m_Point; }
	inline DWORD ChaDbNum() const { return m_ChaDbNum; }
};

//! Field->Client:Character Point 충전량 알림
struct NET_CHAR_POINT_ADD_FC : public NET_MSG_GENERIC
{
    DWORD m_AddPoint; //! 충전된 Point 량
    NET_CHAR_POINT_ADD_FC(DWORD AddPoint)
        : m_AddPoint(AddPoint)
    {
        dwSize = sizeof(NET_CHAR_POINT_ADD_FC);
        nType = NET_MSG_CHAR_POINT_ADD_FC;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POINT_ADD_FC)<=NET_DATA_BUFSIZE);
    }

    inline DWORD AddPoint() const { return m_AddPoint; }
};

//! Agent->Client:Character Point 정보
struct NET_CHAR_POINT_AC : public NET_MSG_GENERIC
{	
    CHARACTER_POINT m_Point;

    NET_CHAR_POINT_AC(const CHARACTER_POINT& Point)
        : m_Point(Point)
    {
        dwSize = sizeof(NET_CHAR_POINT_AC);
        nType = NET_MSG_CHAR_POINT_AC;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POINT_AC)<=NET_DATA_BUFSIZE);
    }

    inline CHARACTER_POINT Point() const { return m_Point; }
};

//! Client->Field:GM 명령어로 Character Point 추가
struct NET_GM_CHAR_POINT_ADD_CF : public NET_MSG_GENERIC
{
    __int64 m_AddPoint; //! 추가할 Point
    NET_GM_CHAR_POINT_ADD_CF(__int64 AddPoint)
        : m_AddPoint(AddPoint)
    {
        dwSize = sizeof(NET_GM_CHAR_POINT_ADD_CF);
        nType = NET_MSG_GM_CHAR_POINT_ADD_CF;
        MIN_STATIC_ASSERT(sizeof(NET_GM_CHAR_POINT_ADD_CF)<=NET_DATA_BUFSIZE);
    }
    
    //! 추가할 Point
    inline __int64 AddPoint() const { return m_AddPoint; }
};

// Field -> Cache 캐시의 캐릭터 Point 갱신 패킷;
struct NET_CHAR_POINT_CHANGE_CACHE_FH : public NET_MSG_GENERIC
{
	const CHARACTER_POINT sUpdateChaPoint;
	const DWORD dwChaDbNum;

	NET_CHAR_POINT_CHANGE_CACHE_FH(
		const DWORD _dwCharDbNum,
		const CHARACTER_POINT _sUpdateChaPoint )
		: dwChaDbNum( _dwCharDbNum )
		, sUpdateChaPoint( _sUpdateChaPoint )
	{
		dwSize = sizeof( NET_CHAR_POINT_CHANGE_CACHE_FH );
		nType = NET_MSG_CHAR_CHANGE_POINT_CACHE_FH;
		MIN_STATIC_ASSERT( sizeof( NET_CHAR_POINT_CHANGE_CACHE_FH ) <= NET_DATA_BUFSIZE );
	}

	inline const DWORD GetChaDbNum() const { return dwChaDbNum; }
	inline const CHARACTER_POINT& GetUpdatePoint() const { return sUpdateChaPoint; }
};

//! Field->Cache:Character PointShop Point 변화 정보
struct NET_CHAR_PS_POINT_CHANGE_FH : public NET_MSG_GENERIC
{
    CHARACTER_POINT m_OldPoint; //! Old point
    CHARACTER_POINT m_NewPoint; //! New point
    DWORD m_ChaDbNum; //! Character db number
    pointshop::EM_POINT_CHANGE_TYPE m_ChangeType; //! Point change type
    SNATIVEID m_ItemId; //! Item Id
    __int64 m_Price; //! Item Price
    DWORD m_UserDbNum;
    int m_ServerNum;
    int m_ChaLevel;

    NET_CHAR_PS_POINT_CHANGE_FH(
        const CHARACTER_POINT& OldPoint,
        const CHARACTER_POINT& NewPoint,
        DWORD ChaDbNum,
        pointshop::EM_POINT_CHANGE_TYPE ChangeType,
        const SNATIVEID& ItemId,
        __int64 Price,
        DWORD UserDbNum,
        int ServerNum,
        int ChaLevel)
        : m_OldPoint(OldPoint)
        , m_NewPoint(NewPoint)
        , m_ChaDbNum(ChaDbNum)
        , m_ChangeType(ChangeType)
        , m_ItemId(ItemId)
        , m_Price(Price)
        , m_UserDbNum(UserDbNum)
        , m_ServerNum(ServerNum)
        , m_ChaLevel(ChaLevel)
    {
        dwSize = sizeof(NET_CHAR_PS_POINT_CHANGE_FH);
        nType = NET_MSG_CHAR_PS_POINT_CHANGE_FH;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_PS_POINT_CHANGE_FH)<=NET_DATA_BUFSIZE);
    }

    inline CHARACTER_POINT OldPoint() const { return m_OldPoint; }
    inline CHARACTER_POINT NewPoint() const { return m_NewPoint; }
    inline DWORD ChaDbNum() const { return m_ChaDbNum; }
    inline pointshop::EM_POINT_CHANGE_TYPE ChangeType() const { return m_ChangeType; }
    inline SNATIVEID ItemId() const { return m_ItemId; }
    inline __int64 Price() const { return m_Price; }
    inline DWORD UserDbNum() const { return m_UserDbNum; }
    inline int ServerNum() const { return m_ServerNum; }
    inline int ChaLevel() const { return m_ChaLevel; }
};

struct NET_CHAR_WEB_SHOP_LOG : public NET_MSG_GENERIC
{
    __int64 m_Price;
    SNATIVEID m_ItemId;    
    DWORD m_UserDbNum;
    DWORD m_ChaDbNum;
    int m_ChaLevel;
    int m_ServerGroup;

    NET_CHAR_WEB_SHOP_LOG(
        const SNATIVEID& ItemId,
        __int64 Price,
        DWORD UserDbNum,
        DWORD ChaDbNum,
        int ChaLevel,
        int ServerGroup)
        : m_ItemId(ItemId)
        , m_Price(Price)
        , m_UserDbNum(UserDbNum)
        , m_ChaDbNum(ChaDbNum)
        , m_ChaLevel(ChaLevel)
        , m_ServerGroup(ServerGroup)
    {
        dwSize = sizeof(NET_CHAR_WEB_SHOP_LOG);
        nType = NET_MSG_CHAR_WEB_SHOP_LOG_FH;
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_WEB_SHOP_LOG)<=NET_DATA_BUFSIZE);
    }
};

struct NET_CHAR_POS_FA : public NET_MSG_GENERIC
{
    DWORD m_ChaDbNum;
    float m_fPosX;
	float m_fPosY;
    float m_fPosZ;

    NET_CHAR_POS_FA(DWORD ChaDbNum)
        : m_ChaDbNum(ChaDbNum)
        , m_fPosX(0.0f)
		, m_fPosY(0.0f)
        , m_fPosZ(0.0f)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POS_FA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CHAR_POS_FA);
        nType = NET_MSG_CHAR_POS_FA;
    }

    NET_CHAR_POS_FA(const DWORD ChaDbNum, const float fPosX, const float fPosY, const float fPosZ)
        : m_ChaDbNum(ChaDbNum)
        , m_fPosX(fPosX)
		, m_fPosY(fPosY)
        , m_fPosZ(fPosZ)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CHAR_POS_FA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CHAR_POS_FA);
        nType = NET_MSG_CHAR_POS_FA;
    }
};

//! 캐릭터 이름으로 캐릭터 DB 번호를 요청
struct CHA_DB_NUM_REQ_CA
{
    DWORD       m_ReqSeq;  //! 요청번호
    std::string m_ChaName; //! 캐릭터 이름
    
    MSGPACK_DEFINE(m_ChaName, m_ReqSeq);

    CHA_DB_NUM_REQ_CA()
        : m_ReqSeq(0)
    {
    }

    CHA_DB_NUM_REQ_CA(DWORD ReqSeq, const std::string& ChaName)
        : m_ReqSeq(ReqSeq)
        , m_ChaName(ChaName)
    {
    }
};

//! 캐릭터 이름으로 캐릭터 DB 번호 요청 결과
struct CHA_DB_NUM_REQ_AC
{
    DWORD m_ReqSeq;        //! 요청번호
    DWORD m_ChaDbNum;      //! 캐릭터 DB 번호
    DWORD m_UserDbNum;     //! 유저번호
    BYTE  m_UserType;      //! 유저 타입 EMUSERTYPE
    std::string m_ChaName; //! 캐릭터 이름

    MSGPACK_DEFINE(m_ReqSeq, m_ChaDbNum, m_UserDbNum, m_UserType, m_ChaName);

    CHA_DB_NUM_REQ_AC()
        : m_ReqSeq(0)
        , m_ChaDbNum(0)
        , m_UserDbNum(0)
        , m_UserType(USER_COMMON)
    {
    }

    CHA_DB_NUM_REQ_AC(
        DWORD ReqSeq,
        DWORD ChaDbNum,
        DWORD UserDbNum,
        BYTE UserType,
        const std::string& ChaName)
        : m_ReqSeq(ReqSeq)
        , m_ChaDbNum(ChaDbNum)
        , m_UserDbNum(UserDbNum)
        , m_UserType(UserType)
        , m_ChaName(ChaName)
    {
    }
};

//! 캐릭터 이름으로 캐릭터 DB 번호 요청 결과
struct CHA_DB_NUM_REQ_DA
{
    DWORD m_ClientSlot;
    DWORD m_ReqSeq;        //! 요청번호
    DWORD m_ChaDbNum;      //! 캐릭터 DB 번호
    DWORD m_UserDbNum;     //! 유저번호
    BYTE  m_UserType;      //! 유저 타입 EMUSERTYPE
	std::string m_ChaName; //! 캐릭터 이름

	int   m_iClass;		   //! 유저클래스
	int	  m_iLv;			   //! 유저레벨

    int   m_ReqType;       //! 요청대상 //namespace charnamechache::REQUEST_TYPE
    std::string m_ReqCharName; //! 요청한 캐릭터 이름
	
    //MSGPACK_DEFINE(m_ClientSlot, m_ReqSeq, m_ChaDbNum, m_UserDbNum, m_UserType, m_ChaName, m_ReqType, m_ReqCharName);
	MSGPACK_DEFINE(m_ClientSlot, m_ReqSeq, m_ChaDbNum, m_UserDbNum, m_UserType, m_ChaName, m_iClass, m_iLv, m_ReqType, m_ReqCharName);

    CHA_DB_NUM_REQ_DA()
        : m_ClientSlot(0)
        , m_ReqSeq(0)
        , m_ChaDbNum(0)
        , m_UserDbNum(0)
        , m_UserType(USER_COMMON)
		, m_iClass(0)
		, m_iLv(0)
        , m_ReqType(charnamechache::REQUEST_CLIENT)
    {
    }

    CHA_DB_NUM_REQ_DA(
        DWORD ClientSlot,
        DWORD ReqSeq,
        DWORD ChaDbNum,
        DWORD UserDbNum,
        BYTE UserType,
        const std::string& ChaName, 
        int ReqType,
        const std::string& ReqCharName)
        : m_ClientSlot(ClientSlot)
        , m_ReqSeq(ReqSeq)
        , m_ChaDbNum(ChaDbNum)
        , m_UserDbNum(UserDbNum)
        , m_UserType(UserType)
        , m_ChaName(ChaName)
        , m_ReqType(ReqType)
        , m_ReqCharName(ReqCharName)
    {
    }

	CHA_DB_NUM_REQ_DA(
		DWORD ClientSlot,
		DWORD ReqSeq,
		DWORD ChaDbNum,
		DWORD UserDbNum,
		BYTE UserType,
		const std::string& ChaName, 
		int iClass,
		int iLv,
		int ReqType,
		const std::string& ReqCharName)
		: m_ClientSlot(ClientSlot)
		, m_ReqSeq(ReqSeq)
		, m_ChaDbNum(ChaDbNum)
		, m_UserDbNum(UserDbNum)
		, m_UserType(UserType)
		, m_ChaName(ChaName)
		, m_ReqType(ReqType)
		, m_ReqCharName(ReqCharName)
		, m_iClass( iClass )
		, m_iLv(iLv )
	{
	}
};

//! 게임머니 변화 알림GLCONST_CHAR::NotifyMoney 금액보다 큰 금액이 변화된 경우
struct NET_CHAR_MONEY_NOTIFY_FA : public NET_MSG_GENERIC
{	
	DWORD dwChaDbNum;
	__time64_t tDate;
	LONGLONG llOldMoney;
	LONGLONG llNewMoney;
	int nCause;
	int nChannel;
	int nField;

	NET_CHAR_MONEY_NOTIFY_FA()
		: dwChaDbNum( 0 )
		, tDate( 0 )
		, llOldMoney( 0 )
		, llNewMoney( 0 )
		, nCause( 0 )
		, nChannel(0)
		, nField(0)
	{
		dwSize = sizeof( NET_CHAR_MONEY_NOTIFY_FA );
		nType = NET_MSG_CHAR_MONEY_NOTIFY_FA;
		MIN_STATIC_ASSERT( sizeof( NET_CHAR_MONEY_NOTIFY_FA ) <= NET_DATA_BUFSIZE );
	}
};

#endif // _GL_CHARACTER_MSG_H_