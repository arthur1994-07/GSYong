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

    // 이미 인증중이다
    if (m_pClientManager->IsAccountPassing(ClientSlot))
        return;

    int Channel = 0;
    std::string EncUserID   = Data.m_UserId;
    std::string EncPassword = Data.m_Password;
    std::string PassKey     = Data.m_Key;
    
    // 암호풀기
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
        //CloseClient(dwClient); // 접속종료
        sc::writeLogWarn(sc::string::format("CH %d is FULL", Channel));
        return;
    }
    else
    {
        // 신규접속자
        if (m_pClientManager->GetChannel(ClientSlot) == -1)
        {
            IncreaseChannelUser(Channel);
            m_pClientManager->SetChannel(ClientSlot, Channel);
        }
        else // 기존접속자
        {            
        }
    }

    // 인증중으로 세팅
    m_pClientManager->SetAccountPassing(ClientSlot, true);
    m_pClientManager->SetUserID(ClientSlot, UserID);

    // DB 에 인증요청
    m_pClientManager->SetAccountPass(ClientSlot, false); // 인증통과 실패 세팅

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
    
    // 로그인 성공
    if (pPacket->m_emResult == EM_LOGIN_FB_SUB_OK)
    {        
        if (m_pClientManager->IsOnline(dwClientSlot) &&
            m_pClientManager->GetClientIP(dwClientSlot) == pPacket->szIp &&
            strcmp(m_pClientManager->GetUserID(dwClientSlot), pPacket->szUserid) == 0)
        {
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClientSlot );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClientSlot );

            // 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
            WorldBattleLoginSuccess(MsgAC, pPacket, dwClientSlot);
        }        
        else if (!m_pClientManager->IsOnline(dwClientSlot))
        {
            // 로그인성공이지만 이미 오프라인일때...
            // 로그아웃 시킨다.
            m_pAdoManager->AddUserJob(
                db::DbActionPtr(
                    new CUserLogoutSimple(pPacket->szUserid)));
            sc::writeLogWarn(
                sc::string::format("MsgLoginBack User Already Offline %1%", pPacket->szUserid));
        }        
        else 
        {
            // 로그인성공, 온라인이지만 ip, id 가 틀릴때...
            // 기존사용자는 로그아웃 시키고
            m_pAdoManager->AddUserJob(
                db::DbActionPtr(
                    new CUserLogoutSimple(pPacket->szUserid)));
            sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", pPacket->szUserid));
            // 현재사용자는 대기시킨다.
            // 현재 사용자가 로그인 요청중일 가능성이 있다.
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

    m_pClientManager->SetAccountPass  (dwClientSlot, true);                    // 인증통과했음
    m_pClientManager->SetUserDbNum    (dwClientSlot, pPacket->nUserNum);       // 유저번호
    m_pClientManager->SetLoginTime    (dwClientSlot);                          // 로그인 시간
    m_pClientManager->SetUserType     (dwClientSlot, pPacket->nUserType);      // 유저타입세팅
    m_pClientManager->SetChaRemain	  (dwClientSlot, pPacket->uChaRemain);     // 만들수 있는 캐릭터 갯수
    m_pClientManager->SetChaTestRemain(dwClientSlot, pPacket->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
    m_pClientManager->SetPremiumDate  (dwClientSlot, pPacket->tPremiumTime);   // 프리미엄 서비스 기간
    m_pClientManager->SetChatBlockDate(dwClientSlot, pPacket->tChatBlockTime); // 채팅 블록 기간
    m_pClientManager->SetCountry      (dwClientSlot, pPacket->m_Country);      // 국가 세팅
    //m_pClientManager->ThaiSetFlag     (dwClientSlot, nlfd2->wThaiFlag);      // Thailand 사용자 플래그
    //m_pClientManager->SetIPBonusClass (dwClientSlot, nlfd2->emIPBonus);      // Thailand 사용자 Class Type

/*
    // DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
    m_pClientManager->SetExtremeCreateDBM(dwClientSlot, pPacket->nExtremeM);
    m_pClientManager->SetExtremeCreateDBW(dwClientSlot, pPacket->nExtremeW);

    // 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업
    // 극강부 남자 생성조건이 0이하일 경우..
    if (pPacket->nExtremeM < 0)
    {
        pPacket->nExtremeW += pPacket->nExtremeM;
        pPacket->nExtremeM = 0;
    }

    // 극강부 여자 생성조건이 0이하일 경우..
    if (pPacket->nExtremeW < 0)
    {
        pPacket->nExtremeM += pPacket->nExtremeW;
        pPacket->nExtremeW = 0;
    }

    // 극강부 추가
    MsgAC.m_ExtremeM = pPacket->nExtremeM;
    MsgAC.m_ExtremeW = pPacket->nExtremeW;

    // 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
    m_pClientManager->SetExtremeCreateM(dwClientSlot, pPacket->nExtremeM);
    m_pClientManager->SetExtremeCreateW(dwClientSlot, pPacket->nExtremeW);
*/
    SendClient(dwClientSlot, &MsgAC); // 결과전송
}

void CAgentServer::WorldBattleLoginFail(DWORD ClientSlot, NET_LOGIN_FEEDBACK_DATA2* pPacket, NET_LOGIN_FEEDBACK_DATA& MsgAC)
{
    // 로그인 실패일때
    m_pClientManager->SetAccountPass(ClientSlot, false); // 인증통과 실패
    m_pClientManager->SetAccountPassing(ClientSlot, false); // 인증통과중 상태 false

    if ((m_pClientManager->IsOnline (ClientSlot) == true) &&
        (m_pClientManager->GetClientIP(ClientSlot) == pPacket->szIp) &&
        (strcmp (m_pClientManager->GetUserID (ClientSlot), pPacket->szUserid) == 0))
    {
        // 로그인 실패이고 온라인이고 IP, ID 일치
        MsgAC.m_Result = pPacket->m_emResult;
        SendClient(ClientSlot, &MsgAC);
    }        
    else if (m_pClientManager->IsOnline(ClientSlot) == false)
    {
        // 로그인 실패이고 이미 오프라인일때...
        sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", pPacket->szUserid ));
    }
    else
    {
        // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
        // 현재사용자는 대기
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

    // Client 로 결과 전송    
    SendClient(ClientSlot, &MsgAC);
}