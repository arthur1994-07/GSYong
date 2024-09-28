#include "pch.h"
#include "../../SigmaCore/Encrypt/RandomRC5Encrypt.h"

#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/WorldBattleMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Database/DbAction/DbActionUser.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::MsgLoginWbCA(DWORD ClientSlot, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOGIN_WB_CA Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
    {
        std::string ErrorMsg(
            sc::string::format(
                "CAgentServer::MsgLoginWbCA DataSize %1%", pPacket->DataSize()));
        sc::writeLogError(ErrorMsg);
        return;
    }

    if (pPacket->Crc() != sc::string::getCrc32(pPacket->DataBuffer(), pPacket->DataSize()))
    {
        std::string ErrorMsg(
            sc::string::format(
                "CAgentServer::MsgLoginWbCA CRC %1%", pPacket->Crc()));
        sc::writeLogError(ErrorMsg);
        return;
    }

    if (m_ServiceProvider != SP_WORLD_BATTLE)
    {
        NET_LOGIN_FEEDBACK_DATA nlfd;
        nlfd.nType	= NET_MSG_LOGIN_FB;
        nlfd.m_Result= EM_LOGIN_FB_WRONG_SP;
        SendClient(ClientSlot, &nlfd);
		CloseClient(ClientSlot, net::WORLDBATTLE);
        sc::writeLogError(sc::string::format("MsgLoginWbCA ServiceProvide %1%", (DWORD) m_ServiceProvider));
        return;
    }

    // �̹� �������̴�
    if (m_pClientManager->IsAccountPassing(ClientSlot))
        return;

    int Channel = 0;
    std::string EncUserID   = Data.m_UserId;
    std::string EncPassword = Data.m_Password;
    std::string PassKey     = Data.m_Key;
    
    // ��ȣǮ��
    sc::RandomRC5 DecRc5(PassKey);
    std::string UserID = DecRc5.GetDecrypt(EncUserID);
    std::string UserPassword = DecRc5.GetDecrypt(EncPassword);

    if (UserID.empty() || UserPassword.empty())
    {
        sc::writeLogError(std::string("MsgLoginWbCA ID/PWD empty"));
        return;
    }

    if (Channel < 0 || Channel >= m_nServerChannelNumber)
    {
        NET_LOGIN_FEEDBACK_DATA nlfd;
        nlfd.nType	= NET_MSG_LOGIN_FB;
        nlfd.m_Result= EM_LOGIN_FB_CH_FULL;
        SendClient(ClientSlot, &nlfd);
		CloseClient(ClientSlot, net::WORLDBATTLE);
        sc::writeLogError(sc::string::format("MsgLoginWbCA Invalid channel number (Channel:%d)", Channel));
        return;
    }

    if (IsChannelFull(Channel))
    {		
        NET_LOGIN_FEEDBACK_DATA nlfd;
        nlfd.nType	= NET_MSG_LOGIN_FB;
        nlfd.m_Result= EM_LOGIN_FB_CH_FULL;
        SendClient(ClientSlot, &nlfd);
        //CloseClient(dwClient); // ��������
        sc::writeLogWarn(sc::string::format("CH %d is FULL", Channel));
        return;
    }
    else
    {
        // �ű�������
        if (m_pClientManager->GetChannel(ClientSlot) == -1)
        {
            IncreaseChannelUser(Channel);
            m_pClientManager->SetChannel(ClientSlot, Channel);
        }
        else // ����������
        {            
        }
    }

    // ���������� ����
    m_pClientManager->SetAccountPassing(ClientSlot, true);
    m_pClientManager->SetUserID(ClientSlot, UserID);

    // DB �� ������û
    m_pClientManager->SetAccountPass(ClientSlot, false); // ������� ���� ����

    m_pAdoManager->AddUserJob(
        db::DbActionPtr(
            new WorldBattleUserCheck(
                UserID,
                UserPassword,
                m_pClientManager->GetClientIP(ClientSlot),
                m_nServerGroup,
                m_nServerNum,
                //0,
                ClientSlot,
                static_cast<DWORD>(m_ServiceProvider))));
}


void CAgentServer::MsgWorldBattleLoginBack(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    if (m_ServiceProvider != SP_WORLD_BATTLE)
    {
        sc::writeLogError(std::string("CAgentServer::MsgWorldBattleLoginBack not world battle"));
        return;
    }

    if (ClientSlot != 0)
    {
        sc::writeLogError(std::string("CAgentServer::MsgWorldBattleLoginBack ClientSlot != 0"));
        return;
    }

    NET_LOGIN_FEEDBACK_DATA2* pPacket = (NET_LOGIN_FEEDBACK_DATA2*) pMsg;

    NET_LOGIN_FEEDBACK_DATA	MsgAC;
    MsgAC.nType = NET_MSG_LOGIN_FB;

    DWORD dwClientSlot = static_cast<DWORD> (pPacket->m_ClientSlot);
    
    // �α��� ����
    if (pPacket->m_emResult == EM_LOGIN_FB_SUB_OK)
    {        
        if (m_pClientManager->IsOnline(dwClientSlot) &&
            m_pClientManager->GetClientIP(dwClientSlot) == pPacket->szIp &&
            strcmp(m_pClientManager->GetUserID(dwClientSlot), pPacket->szUserid) == 0)
        {
			// Use Features ����ȭ;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClientSlot );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClientSlot );

            // �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
            WorldBattleLoginSuccess(MsgAC, pPacket, dwClientSlot);
        }        
        else if (!m_pClientManager->IsOnline(dwClientSlot))
        {
            // �α��μ��������� �̹� ���������϶�...
            // �α׾ƿ� ��Ų��.
            m_pAdoManager->AddUserJob(
                db::DbActionPtr(
                    new CUserLogoutSimple(pPacket->szUserid)));
            sc::writeLogWarn(
                sc::string::format("MsgLoginBack User Already Offline %1%", pPacket->szUserid));
        }        
        else 
        {
            // �α��μ���, �¶��������� ip, id �� Ʋ����...
            // ��������ڴ� �α׾ƿ� ��Ű��
            m_pAdoManager->AddUserJob(
                db::DbActionPtr(
                    new CUserLogoutSimple(pPacket->szUserid)));
            sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", pPacket->szUserid));
            // �������ڴ� ����Ų��.
            // ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
        }
    }
    else
    {
        WorldBattleLoginFail(dwClientSlot, pPacket, MsgAC);
    }
}

void CAgentServer::WorldBattleLoginSuccess(NET_LOGIN_FEEDBACK_DATA& MsgAC, NET_LOGIN_FEEDBACK_DATA2* pPacket, DWORD dwClientSlot)
{
    MsgAC.m_Country = pPacket->m_Country;
    MsgAC.m_Result = pPacket->m_emResult;

    if (pPacket->uChaRemain > MAX_CHAR_LENGTH)
    {
        sc::writeLogError(sc::string::format("MsgWorldBattleLoginBack ChaRemain %1%", pPacket->uChaRemain));
        pPacket->uChaRemain = 0;
    }
    if (pPacket->uChaTestRemain > MAX_CHAR_LENGTH)
    {
        sc::writeLogError(sc::string::format("MsgWorldBattleLoginBack ChaTestRemain %1%", pPacket->uChaTestRemain));
        pPacket->uChaTestRemain = 0;
    }

    MsgAC.m_ChaRemain = pPacket->uChaRemain;

    m_pClientManager->SetAccountPass  (dwClientSlot, true);                    // �����������
    m_pClientManager->SetUserDbNum    (dwClientSlot, pPacket->nUserNum);       // ������ȣ
    m_pClientManager->SetLoginTime    (dwClientSlot);                          // �α��� �ð�
    m_pClientManager->SetUserType     (dwClientSlot, pPacket->nUserType);      // ����Ÿ�Լ���
    m_pClientManager->SetChaRemain	  (dwClientSlot, pPacket->uChaRemain);     // ����� �ִ� ĳ���� ����
    m_pClientManager->SetChaTestRemain(dwClientSlot, pPacket->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
    m_pClientManager->SetPremiumDate  (dwClientSlot, pPacket->tPremiumTime);   // �����̾� ���� �Ⱓ
    m_pClientManager->SetChatBlockDate(dwClientSlot, pPacket->tChatBlockTime); // ä�� ��� �Ⱓ
    m_pClientManager->SetCountry      (dwClientSlot, pPacket->m_Country);      // ���� ����
    //m_pClientManager->ThaiSetFlag     (dwClientSlot, nlfd2->wThaiFlag);      // Thailand ����� �÷���
    //m_pClientManager->SetIPBonusClass (dwClientSlot, nlfd2->emIPBonus);      // Thailand ����� Class Type

/*
    // DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
    m_pClientManager->SetExtremeCreateDBM(dwClientSlot, pPacket->nExtremeM);
    m_pClientManager->SetExtremeCreateDBW(dwClientSlot, pPacket->nExtremeW);

    // ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�
    // �ذ��� ���� ���������� 0������ ���..
    if (pPacket->nExtremeM < 0)
    {
        pPacket->nExtremeW += pPacket->nExtremeM;
        pPacket->nExtremeM = 0;
    }

    // �ذ��� ���� ���������� 0������ ���..
    if (pPacket->nExtremeW < 0)
    {
        pPacket->nExtremeM += pPacket->nExtremeW;
        pPacket->nExtremeW = 0;
    }

    // �ذ��� �߰�
    MsgAC.m_ExtremeM = pPacket->nExtremeM;
    MsgAC.m_ExtremeW = pPacket->nExtremeW;

    // �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
    m_pClientManager->SetExtremeCreateM(dwClientSlot, pPacket->nExtremeM);
    m_pClientManager->SetExtremeCreateW(dwClientSlot, pPacket->nExtremeW);
*/
    SendClient(dwClientSlot, &MsgAC); // �������
}

void CAgentServer::WorldBattleLoginFail(DWORD ClientSlot, NET_LOGIN_FEEDBACK_DATA2* pPacket, NET_LOGIN_FEEDBACK_DATA& MsgAC)
{
    // �α��� �����϶�
    m_pClientManager->SetAccountPass(ClientSlot, false); // ������� ����
    m_pClientManager->SetAccountPassing(ClientSlot, false); // ��������� ���� false

    if ((m_pClientManager->IsOnline (ClientSlot) == true) &&
        (m_pClientManager->GetClientIP(ClientSlot) == pPacket->szIp) &&
        (strcmp (m_pClientManager->GetUserID (ClientSlot), pPacket->szUserid) == 0))
    {
        // �α��� �����̰� �¶����̰� IP, ID ��ġ
        MsgAC.m_Result = pPacket->m_emResult;
        SendClient(ClientSlot, &MsgAC);
    }        
    else if (m_pClientManager->IsOnline(ClientSlot) == false)
    {
        // �α��� �����̰� �̹� ���������϶�...
        sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", pPacket->szUserid ));
    }
    else
    {
        // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
        // �������ڴ� ���
        sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", pPacket->szUserid));
    }
}

void CAgentServer::MsgUserChangeCountry(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
    wb::NET_USER_CHANGE_COUNTRY_CA* pPacket = (wb::NET_USER_CHANGE_COUNTRY_CA*) pMsg;
    wb::EM_COUNTRY UserCountry = m_pClientManager->GetCountry(ClientSlot);
    wb::EM_COUNTRY NewUserCountry = pPacket->m_Country;
    
    wb::NET_USER_CHANGE_COUNTRY_AC MsgAC(NewUserCountry, wb::USER_COUNTRY_CHANGE_SUCCESS);
    if (UserCountry != wb::UNKNOWN_COUNTRY)
    {
        MsgAC.m_Result = wb::USER_COUNTRY_CHANGE_USER_UNKNOWN_COUNTRY;
        SendClient(ClientSlot, &MsgAC);
        return;
    }
    if (NewUserCountry == wb::UNKNOWN_COUNTRY)
    {
        MsgAC.m_Result = wb::USER_COUNTRY_CHANGE_NEW_UNKNOWN_COUNTRY;
        SendClient(ClientSlot, &MsgAC);
        return;
    }
    if (UserCountry == NewUserCountry)
    {
        MsgAC.m_Result = wb::USER_COUNTRY_CHANGE_SAME_COUNTRY;
        SendClient(ClientSlot, &MsgAC);
        return;
    }
    if (!GLogicData::GetInstance().wbIsExistCountry(NewUserCountry))
    {
        MsgAC.m_Result = wb::USER_COUNTRY_CHANGE_NOT_EXIST_COUNTRY;
        SendClient(ClientSlot, &MsgAC);
        return;
    }

    int UserDbNum = m_pClientManager->UserDbNum(ClientSlot);
    m_pClientManager->SetCountry(ClientSlot, NewUserCountry);
    m_pAdoManager->AddUserJob(
        db::DbActionPtr(
            new UserCountryUpdate(UserDbNum, NewUserCountry)));

    // Client �� ��� ����    
    SendClient(ClientSlot, &MsgAC);
}