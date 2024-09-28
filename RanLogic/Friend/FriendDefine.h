#ifndef _GL_FRIEND_DEFINE_H_
#define _GL_FRIEND_DEFINE_H_

#pragma once

#include "../s_NetGlobal.h"

#define INVALID_FRIEND_GROUP_NUMBER SHRT_MAX

// 친구정보 Friend
struct CHA_FRIEND
{
    int nChaS;
    int nChaFlag;
    WORD m_Group;
    std::string m_ChaName; //[CHR_ID_LENGTH+1];
    DWORD m_ChaClass; // EMCHARCLASS
    WORD  m_ChaLevel;
    MapID m_ChaMap;

    CHA_FRIEND(WORD Group) 
        : nChaS(0)
        , nChaFlag(0)
        , m_Group(Group)
        , m_ChaClass(0)
        , m_ChaLevel(1)
    {
        //memset(m_ChaName, 0, sizeof(TCHAR) * (CHR_ID_LENGTH+1));
    };

    void SetFriendName(const std::string& FriendName)
    {
        m_ChaName = FriendName;
    }
};
typedef CHA_FRIEND* LPCHA_FRIEND;

// 친구정보(SMS 포함)
struct CHA_FRIEND_SMS
{
    int nChaS;
    int nChaFlag;
    TCHAR szCharName[CHR_ID_LENGTH+1];
    TCHAR szPhoneNumber[SMS_RECEIVER];

    CHA_FRIEND_SMS() 
        : nChaS(0)
        , nChaFlag(0)
    {
        memset( szCharName, 0, sizeof(TCHAR) * (CHR_ID_LENGTH+1) );
        memset( szPhoneNumber, 0, sizeof(TCHAR) * (SMS_RECEIVER) );
    };
};
typedef CHA_FRIEND_SMS* LPCHA_FRIEND_SMS;

struct FRIEND_GROUP
{
    WORD m_GroupNum;
    char m_GroupName[CHR_ID_LENGTH];

    FRIEND_GROUP()
        : m_GroupNum(0)
    {
        memset(m_GroupName, 0, CHR_ID_LENGTH);
    }

    FRIEND_GROUP(WORD GroupNum, const std::string& GroupName)
        : m_GroupNum(GroupNum)
    {
        SetName(GroupName);
    }

    void SetName(const char* GroupName)
    {
        if (GroupName)
            StringCchCopy(m_GroupName, CHR_ID_LENGTH, GroupName);
    }
    void SetName(const std::string& GroupName)
    {
        SetName(GroupName.c_str());
    }
};



#endif // _GL_FRIEND_DEFINE_H_