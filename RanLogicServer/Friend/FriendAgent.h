#ifndef _RAN_LOGIC_SERVER_FRIEND_AGENT_H_
#define _RAN_LOGIC_SERVER_FRIEND_AGENT_H_

#pragma once

#include "../../RanLogic/Friend/Friend.h"

class GLAgentServer;
class GLCharAG;

namespace friendlogic
{
    
class FriendAgent : public Friend
{
public:
    FriendAgent();
    virtual ~FriendAgent();
    FriendAgent& operator = (const FriendAgent& rhs);

protected:

public:
    using Friend::Add;
    bool Add(const SFRIEND& Info, GLAgentServer* pServer, GLCharAG* pChar);
    bool Add(const char* szFriend, WORD Group, GLAgentServer* pServer, GLCharAG* pChar);

    using Friend::Del;
    bool Del(const char* szName, GLAgentServer* pServer, GLCharAG* pChar);

    using Friend::GroupRename;
    bool GroupRename(WORD GroupNum, const std::string& NewName, GLAgentServer* pServer, GLCharAG* pChar);

    using Friend::GroupAdd;
    bool GroupAdd(const std::string& GroupName, GLAgentServer* pServer, GLCharAG* pChar);
    bool GroupDel(WORD GroupNum, GLAgentServer* pServer, GLCharAG* pChar);
    bool GroupSet(DWORD FriendDbNum, WORD Group, GLAgentServer* pServer, GLCharAG* pChar);

    bool Block(const char* szFriend, GLAgentServer* pServer, GLCharAG* pChar);
    void SetState(const char* szFriend, DWORD dwFlag, GLAgentServer* pServer, GLCharAG* pChar);
    
    //void FrameMove(float fTime, float fElapsedTime, DWORD ClientSlot, GLAgentServer* pServer);
    //void MsgPhoneNumberBrd(const char* ChaName, const char* NewPhoneNumber, GLAgentServer* pServer, GLCharAG* pChar);
    void MsgRenameBrd(const char* OldName, const char* NewName, GLAgentServer* pServer, GLCharAG* pChar);
    void MsgReqFriendList(GLAgentServer* pServer, GLCharAG* pChar);

    void MsgFriendGroupReq(GLAgentServer* pServer, GLCharAG* pChar);

    //void SendMyLevelToFriend(GLAgentServer* pServer, GLCharAG* pChar);
    //void SendMyMapToFriend(GLAgentServer* pServer, GLCharAG* pChar);

    void SendToFriend(NET_MSG_GENERIC* pMsg, GLAgentServer* pServer, bool OnlyValidFriend);
    void SendToFriend(
        GLAgentServer* pServer,
        EMNET_MSG MsgType,
        const msgpack::sbuffer& PackBuffer,
        bool OnlyValidFriend,
        bool UseCrc=false);

    void GetValidFriend(std::set<DWORD>& vOnlineChar) const;

    bool IsFriend(GLCharAG* pChar, GLCharAG* pTarChar);
    bool IsFriend(std::tr1::shared_ptr<SFRIEND> spFriend, std::tr1::shared_ptr<SFRIEND> spTarFriend);
    void SendOnlineMessage(const char* szFriend, bool bOnline, GLAgentServer* pServer, GLCharAG* pChar);
};

} // namespace friendlogic

#endif