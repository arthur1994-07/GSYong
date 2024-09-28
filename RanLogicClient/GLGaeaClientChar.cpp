#include "pch.h"
#include "../RanLogic/Msg/CharacterMsg.h"

#include "./Char/CharNameDbNumCache.h"
#include "./Char/GLCharacter.h"
#include "./GLGaeaClient.h"

#include "../RanUI/OldUI/GroupChat/MaxChatWindow.h"
#include "../RanUI/OldUI/GroupChat/ChatPopUpWindow.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCharacter* GLGaeaClient::GetCharacter()
{ 
    return m_pCharacter;
}

const D3DXVECTOR3& GLGaeaClient::GetCharacterPos()
{ 
    return m_pCharacter->GetPosition();
}

SCHARDATA2& GLGaeaClient::GetCharacterData()
{ 
    return m_pCharacter->GetCharData();
}

GLCHARLOGIC& GLGaeaClient::GetCharacterLogic()
{ 
    return m_pCharacter->GetCharLogic();
}

DWORD GLGaeaClient::ReqChaDbNum(const std::string& ChaName)
{
    std::string ReqChaName(ChaName);
    sc::string::trim(ReqChaName);
    if (ReqChaName.length() < 2 || ReqChaName.length() >= CHAR_SZNAME)
        return 0;

    // 이미 요청했는지 검사
    DWORD PreviousReqNum = m_pChaDbNumNameCache->GetReqNum(ReqChaName);
    if (PreviousReqNum != 0)
    {
        // 이미 요청했기 때문에 이전 요청번호를 리턴한다.
        return PreviousReqNum;
    }
    else
    {
        // 신규로 요청한다
        static volatile LONG ChaDbNumReqSeq = 0;
        ChaDbNumReqSeq = InterlockedIncrement(&ChaDbNumReqSeq);

        CHA_DB_NUM_REQ_CA SendData(ChaDbNumReqSeq, ReqChaName);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        NETSENDTOAGENT(NET_MSG_CHAR_DB_NUM_REQ_CA, SendBuffer);

        // 요청 등록
        m_pChaDbNumNameCache->AddReqList(ReqChaName, ChaDbNumReqSeq);

        return ChaDbNumReqSeq;
    }
}

void GLGaeaClient::MsgCharDbNumReqAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    CHA_DB_NUM_REQ_AC Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

    m_pChaDbNumNameCache->Add(Data.m_ChaName, Data.m_ChaDbNum);

    // GLMSG::SNET_POST_REQ_CHANUM 도 비슷한 기능을 한다.?
    // 요청번호로 식별해서 사용하면된다.
    //Data.m_ReqSeq;    //! 요청번호
    // 캐릭터 번호가 0 이라면 존재하지 않는 캐릭터이다.
    //Data.m_ChaDbNum;  //! 캐릭터 DB 번호
    //Data.m_UserDbNum; //! 유저번호
    //Data.m_UserType;  //! 유저 타입 EMUSERTYPE
    //Data.m_ChaName;   //! 캐릭터 이름
}

void GLGaeaClient::AddCharacterCache(DWORD ChaDbNum, const std::string& ChaName)
{
    m_pChaDbNumNameCache->Add(ChaName, ChaDbNum);
}

DWORD GLGaeaClient::FindChaDbNumAtCache(const std::string& ChaName)
{
    return m_pChaDbNumNameCache->FindByChaName(ChaName);
}

std::string GLGaeaClient::FindChaNameAtCache(DWORD ChaDbNum)
{
    return m_pChaDbNumNameCache->FindByDbNum(ChaDbNum);
}

void GLGaeaClient::DisplayChaChche()
{
    m_pChaDbNumNameCache->Display();
}
