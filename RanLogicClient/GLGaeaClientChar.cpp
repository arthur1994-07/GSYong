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

    // �̹� ��û�ߴ��� �˻�
    DWORD PreviousReqNum = m_pChaDbNumNameCache->GetReqNum(ReqChaName);
    if (PreviousReqNum != 0)
    {
        // �̹� ��û�߱� ������ ���� ��û��ȣ�� �����Ѵ�.
        return PreviousReqNum;
    }
    else
    {
        // �űԷ� ��û�Ѵ�
        static volatile LONG ChaDbNumReqSeq = 0;
        ChaDbNumReqSeq = InterlockedIncrement(&ChaDbNumReqSeq);

        CHA_DB_NUM_REQ_CA SendData(ChaDbNumReqSeq, ReqChaName);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        NETSENDTOAGENT(NET_MSG_CHAR_DB_NUM_REQ_CA, SendBuffer);

        // ��û ���
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

    // GLMSG::SNET_POST_REQ_CHANUM �� ����� ����� �Ѵ�.?
    // ��û��ȣ�� �ĺ��ؼ� ����ϸ�ȴ�.
    //Data.m_ReqSeq;    //! ��û��ȣ
    // ĳ���� ��ȣ�� 0 �̶�� �������� �ʴ� ĳ�����̴�.
    //Data.m_ChaDbNum;  //! ĳ���� DB ��ȣ
    //Data.m_UserDbNum; //! ������ȣ
    //Data.m_UserType;  //! ���� Ÿ�� EMUSERTYPE
    //Data.m_ChaName;   //! ĳ���� �̸�
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
