#pragma once

#include "../../SigmaCore/gassert.h"
#include "../s_NetGlobal.h"
#include "../Character/GLCharDefine.h"
#include "../Friend/FriendDefine.h"

namespace GLMSG
{

    //! 친구 그룹 생성 요청 Client->Agent
    struct NET_FRIEND_GROUP_NEW_CA : public NET_MSG_GENERIC
    {
        char m_GroupName[CHR_ID_LENGTH];

        NET_FRIEND_GROUP_NEW_CA(const std::string& GroupName)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_NEW_CA) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_NEW_CA);
            nType  = NET_MSG_FRIEND_GROUP_NEW_CA;
            StringCchCopy(m_GroupName, CHR_ID_LENGTH, GroupName.c_str());
        }
    };

    //! 친구 그룹 생성 요청 결과 Agent->Client
    struct NET_FRIEND_GROUP_NEW_AC : public NET_MSG_GENERIC
    {
        WORD m_GroupNum;
        char m_GroupName[CHR_ID_LENGTH];

        NET_FRIEND_GROUP_NEW_AC(WORD GroupNum, const std::string& GroupName)
            : m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_NEW_AC) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_NEW_AC);
            nType  = NET_MSG_FRIEND_GROUP_NEW_AC;
            StringCchCopy(m_GroupName, CHR_ID_LENGTH, GroupName.c_str());
        }
    };

    //! 친구 그룹 삭제 요청 Client->Agent
    struct NET_FRIEND_GROUP_DEL_CA : public NET_MSG_GENERIC
    {
        WORD m_GroupNum;

        NET_FRIEND_GROUP_DEL_CA(WORD GroupNum)
            : m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_DEL_CA) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_DEL_CA);
            nType  = NET_MSG_FRIEND_GROUP_DEL_CA;
        }
    };

    //! 친구 그룹 삭제 요청 결과 Agent->Client
    struct NET_FRIEND_GROUP_DEL_AC : public NET_MSG_GENERIC
    {
        WORD m_GroupNum;

        NET_FRIEND_GROUP_DEL_AC(WORD GroupNum)
            : m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_DEL_AC) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_DEL_AC);
            nType  = NET_MSG_FRIEND_GROUP_DEL_AC;
        }
    };

    //! 친구를 그룹에 포함 요청 Client->Agent
    struct NET_FRIEND_SET_GROUP_CA : public NET_MSG_GENERIC
    {
        DWORD m_FriendDbNum;
        WORD  m_GroupNum;

        NET_FRIEND_SET_GROUP_CA(DWORD FriendDbNum, WORD GroupNum)
            : m_FriendDbNum(FriendDbNum)
            , m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_SET_GROUP_CA) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_SET_GROUP_CA);
            nType  = NET_MSG_FRIEND_SET_GROUP_CA;
        }
    };

    //! 친구를 그룹에 포함 요청 결과 Agent->Client
    struct NET_FRIEND_SET_GROUP_AC : public NET_MSG_GENERIC
    {
        DWORD m_FriendDbNum;
        WORD  m_GroupNum;

        NET_FRIEND_SET_GROUP_AC(DWORD FriendDbNum, WORD GroupNum)
            : m_FriendDbNum(FriendDbNum)
            , m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_SET_GROUP_AC) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_SET_GROUP_AC);
            nType  = NET_MSG_FRIEND_SET_GROUP_AC;
        }
    };

    //! 친구 그룹 이름 변경 Client->Agent
    struct NET_FRIEND_GROUP_NAME_CHANGE_CA : public NET_MSG_GENERIC
    {
        WORD m_GroupNum;        
        char m_GroupName[CHR_ID_LENGTH];
        
        NET_FRIEND_GROUP_NAME_CHANGE_CA(WORD GroupNum, const std::string& GroupName)
            : m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_NAME_CHANGE_CA) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_NAME_CHANGE_CA);
            nType  = NET_MSG_FRIEND_GROUP_NAME_CHANGE_CA;
            StringCchCopy(m_GroupName, CHR_ID_LENGTH, GroupName.c_str());
        }
    };

    //! 친구 그룹 이름 변경 Agent->Client
    struct NET_FRIEND_GROUP_NAME_CHANGE_AC : public NET_MSG_GENERIC
    {
        WORD m_GroupNum;        
        char m_GroupName[CHR_ID_LENGTH];

        NET_FRIEND_GROUP_NAME_CHANGE_AC(WORD GroupNum, const std::string& GroupName)
            : m_GroupNum(GroupNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_NAME_CHANGE_AC) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_NAME_CHANGE_AC);
            nType  = NET_MSG_FRIEND_GROUP_NAME_CHANGE_AC;
            StringCchCopy(m_GroupName, CHR_ID_LENGTH, GroupName.c_str());
        }
    };

    struct NET_FRIEND_GROUP_REQ_CA : public NET_MSG_GENERIC
    {
        NET_FRIEND_GROUP_REQ_CA()
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_REQ_CA) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_REQ_CA);
            nType  = NET_MSG_FRIEND_GROUP_REQ_CA;
        }
    };

    struct NET_FRIEND_GROUP_REQ_AC : public NET_MSG_GENERIC
    {
        FRIEND_GROUP m_Group;

        NET_FRIEND_GROUP_REQ_AC(const FRIEND_GROUP& Group)
            : m_Group(Group)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_GROUP_REQ_AC) <= NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_FRIEND_GROUP_REQ_AC);
            nType  = NET_MSG_FRIEND_GROUP_REQ_AC;
        }
    };

    struct NET_OTHER_CHAR_LEVEL_CHANGE_AC
    {
        DWORD m_CharDbNum;
        WORD  m_ChaLevel;

        MSGPACK_DEFINE(m_CharDbNum, m_ChaLevel);

        NET_OTHER_CHAR_LEVEL_CHANGE_AC(DWORD FriendDbNum, WORD FriendLevel)
            : m_CharDbNum(FriendDbNum)
            , m_ChaLevel(FriendLevel)
        {
        }

        NET_OTHER_CHAR_LEVEL_CHANGE_AC()
            : m_CharDbNum(0)
            , m_ChaLevel(1)
        {
        }
    };

    struct NET_MAP_CHANGE_FRIEND_CLUB_AC
    {
        DWORD m_ChaDbNum;
        MapID m_MapID;

        MSGPACK_DEFINE(m_ChaDbNum, m_MapID);

        NET_MAP_CHANGE_FRIEND_CLUB_AC(DWORD ChaDbNum, const MapID& MapID)
            : m_ChaDbNum(ChaDbNum)
            , m_MapID(MapID)
        {
        }

        NET_MAP_CHANGE_FRIEND_CLUB_AC()
            : m_ChaDbNum(0)
        {
        }
    };

    struct NET_CHAR_STATE_ONLINE_AC
    {
        DWORD m_ChaDbNum;        
        MapID m_CurrentMap;
        float m_fPositionX, m_fPositionY, m_fPositionZ;
        BYTE  m_Channel;

        MSGPACK_DEFINE(m_ChaDbNum, m_CurrentMap, m_fPositionX, m_fPositionY, m_fPositionZ, m_Channel);

        NET_CHAR_STATE_ONLINE_AC()
            : m_ChaDbNum(0)
            , m_fPositionX(0.0f)
			, m_fPositionY(0.0f)
			, m_fPositionZ(0.0f)
            , m_Channel(0)
        {
        }
    };

    struct NET_CHAR_STATE_OFFLINE_AC
    {
        DWORD m_ChaDbNum;

        MSGPACK_DEFINE(m_ChaDbNum);

        NET_CHAR_STATE_OFFLINE_AC()
            : m_ChaDbNum(0)
        {
        }
    };

    struct NET_CHAR_POSITION_AC
    {
        DWORD m_ChaDbNum;
        int m_PosX;
        int m_PosY;

        MSGPACK_DEFINE(m_ChaDbNum, m_PosX, m_PosY);

        NET_CHAR_POSITION_AC()
            : m_ChaDbNum(0)
            , m_PosX(0)
            , m_PosY(0)
        {
        }
    };

    //! 친구 상태 요청
    struct NET_FRIEND_STATE_REQ_CA
    {
        DWORD m_MainChar;
        DWORD m_SubChar;

        MSGPACK_DEFINE(m_MainChar, m_SubChar);

        NET_FRIEND_STATE_REQ_CA()
            : m_MainChar(0)
            , m_SubChar(0)
        {
        }

        NET_FRIEND_STATE_REQ_CA(DWORD MainChar, DWORD SubChar)
            : m_MainChar(MainChar)
            , m_SubChar(SubChar)
        {
        }
    };

    //! 친구 상태 결과
    struct NET_FRIEND_STATE_REQ_AC
    {
        DWORD m_MainChar;
        DWORD m_SubChar;
        int   m_State; // EMFRIEND_FLAGS

        MSGPACK_DEFINE(m_MainChar, m_SubChar, m_State);

        NET_FRIEND_STATE_REQ_AC()
            : m_MainChar(0)
            , m_SubChar(0)
            , m_State(0) // EMFRIEND_OFF
        {
        }

        NET_FRIEND_STATE_REQ_AC(DWORD MainChar, DWORD SubChar, int /*EMFRIEND_FLAGS*/ Flag)
            : m_MainChar(MainChar)
            , m_SubChar(SubChar)
            , m_State(Flag)
        {
        }
    };

    //! 친구 상태 결과
    struct NET_FRIEND_STATE_REQ_DA : public NET_MSG_GENERIC
    {
        DWORD m_MainChar;
        DWORD m_SubChar;
        DWORD m_ReqChaDbNum;
        int   m_State; // EMFRIEND_FLAGS

        NET_FRIEND_STATE_REQ_DA(
            DWORD MainChar,
            DWORD SubChar,
            DWORD ReqChaDbNum,
            int State)
            : m_MainChar(MainChar)
            , m_SubChar(SubChar)
            , m_ReqChaDbNum(ReqChaDbNum)
            , m_State(State)
        {
            MIN_STATIC_ASSERT(sizeof(NET_FRIEND_STATE_REQ_DA) <= NET_DATA_BUFSIZE);

            nType  = NET_MSG_FRIEND_STATE_REQ_DA;
            dwSize = sizeof(NET_FRIEND_STATE_REQ_DA);
        }
    };

    struct NET_OTHER_CHAR_PARTY_CHANGE_AC
    {
        DWORD m_ChaDbNum;
        DWORD m_PartyNum;
		DWORD m_ExpeditionNum;

        MSGPACK_DEFINE(m_ChaDbNum, m_PartyNum, m_ExpeditionNum);

        NET_OTHER_CHAR_PARTY_CHANGE_AC()
            : m_ChaDbNum(0)
            , m_PartyNum(0)
			, m_ExpeditionNum(0)
        {
        }

        NET_OTHER_CHAR_PARTY_CHANGE_AC(DWORD ChaDbNum, GLPartyID PartyID)
            : m_ChaDbNum(ChaDbNum)
            , m_PartyNum(PartyID.GetPartyID())
			, m_ExpeditionNum(PartyID.GetExpeditionID())
        {
        }
    };

} // namespace GLMSG