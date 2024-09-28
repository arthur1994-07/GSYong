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
    DWORD						m_UserDbNum;				//	����� ���� DB ��ȣ
    DWORD						m_dwUserLvl;				//	����� ���� Level, EMUSERTYPE
    __time64_t					m_tPREMIUM;					//	�����̾� ����.
    __time64_t					m_tCHATBLOCK;				//	�Ӹ� ����.


    DWORD						m_dwServerID;				//	������ ID.
    DWORD						m_CharDbNum;				//	�ɸ��� DB ��ȣ
    char						m_szName[CHAR_SZNAME];		//	�ɸ��� �̸�. User ID Name
//  TCHAR						m_szPhoneNumber[SMS_RECEIVER];
    char						m_szUserName[USR_ID_LENGTH+1]; // User Account Name
    TCHAR						m_szIp[MAX_IP_LENGTH+1];	//  ĳ���� ���� IP

    EMIP_BONUS_CLASS			m_emIPBonus;				// ���̹�����

    EMCHARCLASS					m_emClass;					//	����.
	WORD						m_wLevel;					//  ĳ���� ����
    WORD						m_wSchool;					//	�п�.

    DWORD						m_ClubDbNum;				//	Ŭ�� ��ȣ
    __time64_t					m_tSECEDE;					//	Ŭ�� Ż��ð�

    int							m_nBright;					//	�Ӽ�.
    BOOL						m_bUseArmSub;				// �ذ��θ� ���� ���� ���� ��뿩��

    //	Note : �ʱ� ���۵� ��, �� ��ġ��.
    //
    SNATIVEID					m_sStartMapID;				//	�ʱ� ���� ��.
    DWORD						m_dwStartGate;				//	�ʱ� ���� ����Ʈ.
    D3DXVECTOR3					m_vStartPos;				//	�ʱ� ���� ��ġ.

    SChinaTime					m_sChinaTime;				//  �߱� �ð�.
    SVietnamGainSystem			m_sVietnamSystem;			//  ��Ʈ�� Ž�й��� �ý���

    SEventTime					m_sEventTime;				//  �̺�Ʈ ���� �ð�.

    SNATIVEID					m_sSaveMapID;				//	���� ��.
    D3DXVECTOR3					m_vSavePos;					//	���� ��ġ.

    //MAPFRIEND					m_mapFriend;				//	ģ�� ����.
    friendlogic::FriendAgent*   m_pFriend;                  // ģ�� ����
    VECATTEND					m_vecAttend;				//	�⼮ ����.

    bool						m_bTracingUser;				//  ���� �������� �������� �ƴ���

    gamelogic::NotifyManAgent*  m_pNotify;

    GLProductAgent*               m_pProduct;

public:    
    //! �ɸ��� DB ��ȣ
    inline DWORD ChaDbNum() const { return m_CharDbNum; }

    //! ����� ���� DB ��ȣ
    inline DWORD UserDbNum() const { return m_UserDbNum; }

    //! Friend ----------------------------------------------------------------
    bool AddFriend(const SFRIEND& Info);
    bool AddFriend(const CHA_FRIEND& Info);
    bool AddFriendGroup(const FRIEND_GROUP& Info);    
    void SetFriendLevel(DWORD FriendDbNum, WORD FriendLevel);
    void SetFriendMap(DWORD FriendDbNum, const MapID& FriendMap);
    
    //! Notify ���Ӿ˸�
    bool AddNotify(std::tr1::shared_ptr<gamelogic::NotifyBase> spNotify);

    //! ĳ���� ����
    void SetChaLevel(WORD Level) { m_wLevel = Level; }
    WORD GetChaLevel() const { return m_wLevel; }
    
    //! Ŭ�� ��ȣ
    DWORD GetClubDbNum() const { return m_ClubDbNum; }
    void SetClubDbNum(DWORD ClubDbNum) { m_ClubDbNum = ClubDbNum; }

    //! Ŭ�� Ż��ð�
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

    //! �ɸ��� �̸�. User ID Name
    const char* GetChaName() const { return m_szName; }
    void SetChaName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szName, CHAR_SZNAME, szName);
    }

    bool IsValidClub() const { return m_ClubDbNum != CLUB_NULL ? true : false; }

    // ����, ����
    EMCHARCLASS GetClass() const { return m_emClass; }
	const WORD GetSchool() const { return m_wSchool; }

    EMUSERTYPE UserType() const { return static_cast<EMUSERTYPE> (m_dwUserLvl); }
};

namespace GLMSG
{
    //! DB->Agent:ĳ���� Join
    //!	���� �޽�����
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

	// 2�����
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