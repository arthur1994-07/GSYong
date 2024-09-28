#ifndef _RAN_LOGIC_SERVER_AGENT_MSG_H_
#define _RAN_LOGIC_SERVER_AGENT_MSG_H_

#pragma once

#include <memory>
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Character/GLCharDefine.h"

#include "../../RanLogic/Msg/GLContrlCharJoinMsg.h"

namespace friendlogic
{
    class FriendAgent;
}

namespace gamelogic
{
    class NotifyBase;
    class NotifyManAgent;
}

class GLProductAgent;
class GLAgentServer;
struct CHA_FRIEND;
struct FRIEND_GROUP;

class GLCHARAG_DATA
{
public:
    enum { VERSION=0x002, };

    GLCHARAG_DATA();
    virtual ~GLCHARAG_DATA();

    GLCHARAG_DATA& operator = (const GLCHARAG_DATA& cData);


    //typedef std::map<std::string, SFRIEND>		MAPFRIEND;
    //typedef MAPFRIEND::iterator					MAPFRIEND_ITER;
    //typedef MAPFRIEND::const_iterator			MAPFRIEND_CITER;
    //typedef MAPFRIEND::value_type               MAPFRIEND_VALUE;
    typedef std::vector<USER_ATTEND_INFO> VECATTEND;

public:
    DWORD						m_UserDbNum;				//	사용자 고유 DB 번호
    DWORD						m_dwUserLvl;				//	사용자 권한 Level, EMUSERTYPE
    __time64_t					m_tPREMIUM;					//	프리미엄 기한.
    __time64_t					m_tCHATBLOCK;				//	귓말 차단.


    DWORD						m_dwServerID;				//	서버군 ID.
    DWORD						m_CharDbNum;				//	케릭터 DB 번호
    char						m_szName[CHAR_SZNAME];		//	케릭터 이름. User ID Name
//  TCHAR						m_szPhoneNumber[SMS_RECEIVER];
    char						m_szUserName[USR_ID_LENGTH+1]; // User Account Name
    TCHAR						m_szIp[MAX_IP_LENGTH+1];	//  캐릭터 접속 IP

    EMIP_BONUS_CLASS			m_emIPBonus;				// 사이버까페

    EMCHARCLASS					m_emClass;					//	종족.
	WORD						m_wLevel;					//  캐릭터 레벨
    WORD						m_wSchool;					//	학원.

    DWORD						m_ClubDbNum;				//	클럽 번호
    __time64_t					m_tSECEDE;					//	클럽 탈퇴시간

    int							m_nBright;					//	속성.
    BOOL						m_bUseArmSub;				// 극강부를 위한 보조 무기 사용여부

    //	Note : 초기 시작될 맵, 맵 위치값.
    //
    SNATIVEID					m_sStartMapID;				//	초기 시작 맵.
    DWORD						m_dwStartGate;				//	초기 시작 개이트.
    D3DXVECTOR3					m_vStartPos;				//	초기 시작 위치.

    SChinaTime					m_sChinaTime;				//  중국 시간.
    SVietnamGainSystem			m_sVietnamSystem;			//  베트남 탐닉방지 시스템

    SEventTime					m_sEventTime;				//  이벤트 적용 시간.

    SNATIVEID					m_sSaveMapID;				//	종료 맵.
    D3DXVECTOR3					m_vSavePos;					//	종료 위치.

    //MAPFRIEND					m_mapFriend;				//	친구 정보.
    friendlogic::FriendAgent*   m_pFriend;                  // 친구 관리
    VECATTEND					m_vecAttend;				//	출석 정보.

    bool						m_bTracingUser;				//  현재 추적중인 유저인지 아닌지

    gamelogic::NotifyManAgent*  m_pNotify;

    GLProductAgent*               m_pProduct;

public:    
    //! 케릭터 DB 번호
    inline DWORD ChaDbNum() const { return m_CharDbNum; }

    //! 사용자 고유 DB 번호
    inline DWORD UserDbNum() const { return m_UserDbNum; }

    //! Friend ----------------------------------------------------------------
    bool AddFriend(const SFRIEND& Info);
    bool AddFriend(const CHA_FRIEND& Info);
    bool AddFriendGroup(const FRIEND_GROUP& Info);    
    void SetFriendLevel(DWORD FriendDbNum, WORD FriendLevel);
    void SetFriendMap(DWORD FriendDbNum, const MapID& FriendMap);
    
    //! Notify 게임알림
    bool AddNotify(std::tr1::shared_ptr<gamelogic::NotifyBase> spNotify);

    //! 캐릭터 레벨
    void SetChaLevel(WORD Level) { m_wLevel = Level; }
    WORD GetChaLevel() const { return m_wLevel; }
    
    //! 클럽 번호
    DWORD GetClubDbNum() const { return m_ClubDbNum; }
    void SetClubDbNum(DWORD ClubDbNum) { m_ClubDbNum = ClubDbNum; }

    //! 클럽 탈퇴시간
    void SetClubSecedeTime(const __time64_t& sTime) { m_tSECEDE = sTime; }
    __time64_t GetClubSecedeTime() const { return m_tSECEDE; }

    void SetUserId(const char* szId)
    {
        if (szId)
            StringCchCopy(m_szUserName, USR_ID_LENGTH+1, szId);
    }

    void SetIp(const char* szIp)
    {
        if (szIp)
            StringCchCopy(m_szIp, MAX_IP_LENGTH+1, szIp);
    }

	const TCHAR* const GetIp(void) const { return m_szIp; }

    //! 케릭터 이름. User ID Name
    const char* GetChaName() const { return m_szName; }
    void SetChaName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szName, CHAR_SZNAME, szName);
    }

    bool IsValidClub() const { return m_ClubDbNum != CLUB_NULL ? true : false; }

    // 직업, 종족
    EMCHARCLASS GetClass() const { return m_emClass; }
	const WORD GetSchool() const { return m_wSchool; }

    EMUSERTYPE UserType() const { return static_cast<EMUSERTYPE> (m_dwUserLvl); }
};

namespace GLMSG
{
    //! DB->Agent:캐릭터 Join
    //!	내부 메시지용
    struct NET_CHAR_JOIN_DA : public NET_MSG_GENERIC
    {
        DWORD				m_dwClientID;
        GLCHARAG_DATA*		m_pCharAgData;
		int					m_CharacterOption;

        enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(GLCHARAG_DATA*)+sizeof(int), };
        char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

        NET_CHAR_JOIN_DA () :
        m_dwClientID(0),
            m_pCharAgData(0),
			m_CharacterOption( 0 )
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_AGENT_REQ_JOIN;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_JOIN_DA)<=NET_DATA_BUFSIZE);
        }
    };

	// 2차비번
	struct NET_IN_CHAJOIN_2NDPASSWORD : public NET_MSG_GENERIC
    {
        DWORD m_dwClientID;
		DWORD m_dwUserDBNum;
        int m_nResult;				

        NET_IN_CHAJOIN_2NDPASSWORD () :
			m_dwClientID(0),
			m_dwUserDBNum(0),
			m_nResult(0)
        {
            dwSize = sizeof(*this);
            nType = NET_IN_MSG_CHAJOIN_2NDPASSWORD;
            MIN_STATIC_ASSERT(sizeof(NET_IN_CHAJOIN_2NDPASSWORD)<=NET_DATA_BUFSIZE);
        }
    };
	///////////////////////////////

} // namespace GLMSG

#endif