#include "../../pch.h"
#include "../../../RanLogic/Msg/CharacterMsg.h"
#include "../../../RanLogic/Msg/ServerMsg.h"
#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../Server/CacheServer.h"
#include "../../FieldServer/GLGaeaServer.h"

#include "DbActionGame.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

///////////////////////////////////////////////////////////////////////////////
// 캐릭터 생성
CharacterCreateNew::CharacterCreateNew(
	int nIndex, // 캐릭터 인덱스
    DWORD dwUserNum, // 사용자번호
    DWORD dwSvrGrp, // 서버그룹번호
    CString strChaName, // 캐릭터이름
    WORD wSchool, // 캐릭터학교
    WORD wHair, // 캐릭터 머리모양
    WORD wFace, // 캐릭터 얼굴모양
	WORD wHairColor,
	WORD wSex,
    DWORD dwClient, 
    const std::string& UserIP, 
    USHORT uPort/* =0 */,
	BOOL	bJumping /* FALSE */)
{
    m_nIndex     = nIndex; 
    m_dwUserNum  = dwUserNum;
    m_dwSvrGrp   = dwSvrGrp;
    m_strChaName = strChaName;
    m_wSchool    = wSchool;
    m_wHair      = wHair;
    m_wFace      = wFace;
	m_wHairColor = wHairColor;
	m_wSex		 = wSex;
    m_ClientSlot   = dwClient;
    m_strUserIP  = UserIP;
    m_uPort      = uPort;
	m_bJumping = bJumping;
}

int CharacterCreateNew::Execute(NetServer* pServer)
{
    int nChaNum = 0; // 생성된 캐릭터 번호
    GLCHARLOGIC NewCharLogic;
	NewCharLogic.INIT_NEW_CHAR(
        (EMCHARINDEX) m_nIndex, 
		m_dwUserNum, 
		m_dwSvrGrp, 
		m_strChaName, 
		m_wSchool, 
		m_wHair, 
		m_wFace,
		m_wHairColor,
		m_wSex,
		m_bJumping
		);
    
    nChaNum = m_pDbManager->CreateNewCharacter(&NewCharLogic);
	m_pDbManager->CreateNewCharacterExtraWithAdo(&NewCharLogic);	//mjeon.ADO

    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    pAgentServer->MsgCreateCharacterBack(nChaNum, m_ClientSlot, m_dwUserNum, m_strUserIP.c_str(), m_uPort);
    return sc::db::DB_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
CharacterDelete::CharacterDelete(
    int nUserNum,
    int nChaNum,
    const char* szPass2,
    DWORD dwClient,
    const std::string& UserIP,
    USHORT uPort)    
{
	m_nUserNum = nUserNum;
	m_nChaNum  = nChaNum;
	m_strPass2 = szPass2;
	m_ClientSlot = dwClient;
	m_strUserIP= UserIP;
	m_uPort    = uPort;
}

int CharacterDelete::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	int nResult = m_pDbManager->DelCharacter(m_nUserNum, m_nChaNum, m_strPass2.GetString());
	pTemp->MsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_ClientSlot, m_strUserIP.c_str(), m_uPort);
	return NET_OK;
}

//! 캐릭터삭제
CharacterDeleteDaum::CharacterDeleteDaum(
    int nUserNum, int nChaNum, const char* szPass2, DWORD dwClient, const std::string& UserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;
	m_strPass2 = szPass2;
	m_ClientSlot  = dwClient;
	m_strUserIP = UserIP;
	m_uPort     = uPort;
}

int CharacterDeleteDaum::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
    int nResult = m_pDbManager->DaumDelCharacter(m_nUserNum, m_nChaNum, m_strPass2.GetString());
	pTemp->DaumMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_ClientSlot, m_strUserIP.c_str(), m_uPort);
	return NET_OK;
}


//
//mjeon.indonesia
//
CharacterDeleteIdn::CharacterDeleteIdn(
    int nUserNum, int nChaNum, DWORD dwClient, std::string &strUserID, std::string &str2ndPass, const std::string& UserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_ClientSlot  = dwClient;
	m_strUserIP = UserIP;
	m_uPort     = uPort;

	m_strId = strUserID;
	m_str2ndPass= str2ndPass;
}

int CharacterDeleteIdn::Execute(NetServer* pServer)
{
	if (!pServer)
    {
        sc::writeLogError(std::string("CharacterDeleteIdn::Execute - pServer is null."));

        return NET_ERROR;
    }

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>(pServer);

	if (pAgentServer == NULL)
    {
        sc::writeLogError(std::string("CharacterDeleteIdn::Execute - pAgentServer is null."));

		return NET_ERROR;
    }


	//
	//HTTP를 이용해서 VTC 인증
	//
	int nRet = pAgentServer->IdnAuthVTC_2ndPass(m_strId, m_str2ndPass);

	if(nRet == 0)
	{		
		nRet = m_pDbManager->IdnDelCharacter(m_nUserNum, m_nChaNum); //VTC auth succeed.
	}
	else
	{
        sc::writeLogError(sc::string::format("CharacterDeleteIdn::Execute() - VTC auth with 2nd password failed(%1%). ID: %2%, PW: %3%", nRet, m_strId, m_str2ndPass ));
		
		switch (nRet)
		{
		case -2:
			sc::writeLogError(std::string("CharacterDeleteIdn::Execute() - Must input 2nd password."));
			break;

		case -1:
			sc::writeLogError(std::string("CharacterDeleteIdn::Execute() - The account does not have 2nd password, please update the account."));
			break;

		case -50:
			sc::writeLogError(std::string("CharacterDeleteIdn::Execute() - The account does not exist."));
			break;		

		case -53:
			sc::writeLogError(std::string("CharacterDeleteIdn::Execute() - Password is invalid."));
			break;

		case -99:
			sc::writeLogError(std::string("CharacterDeleteIdn::Execute() - System Error."));
			break;

		default:
			sc::writeLogError(std::string("CharacterDeleteIdn::Execute() - Unkown Error."));			
			break;
		}
		//return NET_ERROR;

		nRet = -1;
	}

	pAgentServer->IdnMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nRet, m_ClientSlot, m_strUserIP.c_str(), m_uPort);

	return NET_OK;
}



/**
* GSP 캐릭터삭제
*/
CharacterDeleteGsp::CharacterDeleteGsp(
	int nUserNum,
	int nChaNum,
	DWORD dwClient,
    const std::string& UserIP,
	USHORT uPort )
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_ClientSlot  = dwClient;
	m_strUserIP = UserIP;
	m_uPort     = uPort;
}

int CharacterDeleteGsp::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
    int nResult = m_pDbManager->GspDelCharacter(m_nUserNum, m_nChaNum);
	pTemp->GspMsgSndChaDelInfoBack(
			m_nChaNum,
			m_nUserNum,
			nResult,
			m_ClientSlot,
			m_strUserIP.c_str(),
			m_uPort);
	return NET_OK;
}

/**
* Terra 캐릭터삭제
*/
CharacterDeleteTerra::CharacterDeleteTerra(int nUserNum, int nChaNum, DWORD dwClient, const std::string& UserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_ClientSlot  = dwClient;
	m_strUserIP = UserIP;
	m_uPort     = uPort;
}

int CharacterDeleteTerra::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
    int nResult = m_pDbManager->TerraDelCharacter(m_nUserNum, m_nChaNum);
	pTemp->TerraMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_ClientSlot, m_strUserIP.c_str(), m_uPort);
	return NET_OK;
}

/**
* EXCITE:캐릭터 삭제
*/
CharacterDeleteExciteJapan::CharacterDeleteExciteJapan(
    int nUserNum, int nChaNum, DWORD dwClient, const std::string& UserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_ClientSlot  = dwClient;
	m_strUserIP = UserIP;
	m_uPort     = uPort;
}

int CharacterDeleteExciteJapan::Execute(NetServer* pServer)
{
	if (!pServer) return NET_ERROR;
	
    int nResult = m_pDbManager->ExciteDelCharacter(m_nUserNum, m_nChaNum);
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->ExciteMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_ClientSlot, m_strUserIP.c_str(), m_uPort);
	return NET_OK;
}

/**
* JAPAN:캐릭터 삭제
*/
CharacterDeleteJapan::CharacterDeleteJapan(
    int nUserNum, int nChaNum, DWORD dwClient, const std::string& UserIP, USHORT uPort)
{
	m_nUserNum  = nUserNum;
	m_nChaNum   = nChaNum;

	m_ClientSlot  = dwClient;
	m_strUserIP = UserIP;
	m_uPort     = uPort;
}

int CharacterDeleteJapan::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;
	
    int nResult = m_pDbManager->JapanDelCharacter(m_nUserNum, m_nChaNum);
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->JapanMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_ClientSlot, m_strUserIP.c_str(), m_uPort);
	return NET_OK;
}


CharacterDeleteGs::CharacterDeleteGs(
    int nUserNum, int nChaNum, const char* szPass2, DWORD dwClient, const std::string& UserIP, USHORT uPort)
{
	m_nUserNum = nUserNum;
	m_nChaNum  = nChaNum;
	m_strPass2 = szPass2;
	m_ClientSlot = dwClient;
	m_strUserIP= UserIP;
	m_uPort    = uPort;
}

int CharacterDeleteGs::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	int nResult = m_pDbManager->GsDelCharacter(m_nUserNum, m_nChaNum, m_strPass2.GetString());
	pTemp->GsMsgSndChaDelInfoBack(m_nChaNum, m_nUserNum, nResult, m_ClientSlot, m_strUserIP.c_str(), m_uPort);
	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
CharacterBasicInfoAgent::CharacterBasicInfoAgent(int nUserNum, int nSvrGrp, DWORD AgentClientSlot, DWORD CacheClientSlot, const char* szUserIP, USHORT uPort)
    : m_AgentClientSlot(AgentClientSlot)
{
	m_nUserNum  = nUserNum;
	m_nSvrGrp   = nSvrGrp;	
	m_ClientSlot  = CacheClientSlot;
	m_strUserIP = szUserIP;
	m_uPort     = uPort;
}	

int CharacterBasicInfoAgent::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    NET_CHA_BBA_INFO_DH MsgDC(m_ClientSlot, m_AgentClientSlot, m_uPort, m_strUserIP);
    
    std::vector<int> vChaNum;
	m_pDbManager->GetChaBAInfo(m_nUserNum, m_nSvrGrp, vChaNum);
    
    MsgDC.SetChaTotalNum(vChaNum.size());
    for (size_t i=0; i<vChaNum.size(); ++i)
        MsgDC.SetChaNum(i, vChaNum[i]);

    pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDC);
/*
    sc::writeLogInfo(
        sc::string::format(
            L"CharacterBasicInfoAgent::Execute User %1% ChaNum %2%",
            m_nUserNum,
            vChaNum.size()));
*/

	//pTemp->MsgSndChaBasicBAInfoBack(&m_msg, m_dwClient, m_strUserIP.GetString(), m_uPort);
/*
#if defined(TH_PARAM) || defined(_RELEASED)	|| defined(MY_PARAM) || defined(MYE_PARAM) 
	{
		// 태국 말레이시아에 생성한 유저수를 비교한다.
		int nCreateNum = m_pDbManager->GetUserTempInfo( m_nUserNum );
		if( nCreateNum != 0 && nCreateNum != MsgDC.m_ChaServerTotalNum )
		{
			sc::writeLogError(
                sc::string::format(
                    "DB:The count of creation user were wrong. creation count : %d Another count : %d",
                    MsgDC.m_ChaServerTotalNum,
                    nCreateNum));
		}
	}
#endif
*/
	return NET_OK;
}

CharacterBasicInfoAgentAll::CharacterBasicInfoAgentAll(
    int UserDbNum,
    int nSvrGrp,
    DWORD AgentClientSlot,
    DWORD CacheClientSlot,
    const char* szUserIP,
    USHORT uPort)
    : m_AgentClientSlot(AgentClientSlot)
    , m_UserDbNum(UserDbNum)
    , m_nSvrGrp(nSvrGrp)
{
    m_ClientSlot = CacheClientSlot;
    m_strUserIP = szUserIP;
    m_uPort = uPort;
}

CharacterBasicInfoAgentAll::~CharacterBasicInfoAgentAll()
{
} 
  
int CharacterBasicInfoAgentAll::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    if (m_UserDbNum <= 0)
        return NET_ERROR;
    
	// 아이템 변환
	std::vector< DWORD > vecChaNum;
	m_pDbManager->GetCharacterItemTransferTarget( m_UserDbNum, vecChaNum );
	m_pDbManager->TransferItems( m_UserDbNum, vecChaNum );

	int nRet;
    std::vector< SCHARINFO_LOBBY > vCharInfo;
	nRet = m_pDbManager->GetChaBInfoAll( m_UserDbNum, m_nSvrGrp, vCharInfo );

    if ( sc::db::DB_ERROR == nRet )
    {
        sc::writeLogError(
            sc::string::format(
            "CharacterBasicInfoAgentAll::Execute User %1% not exist",
            m_UserDbNum));
        return NET_ERROR;
    }
    else
    {
        CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);

        // 먼저 사용자 정보 전송
        NET_LOBBY_USER_INFO_DH MsgUserInfoDH(m_UserDbNum, m_ClientSlot, m_AgentClientSlot, m_strUserIP.c_str(), m_uPort);
        pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &MsgUserInfoDH);

        // 캐릭터 정보 전송
        for (size_t i=0; i<vCharInfo.size(); ++i)
        {
            NET_LOBBY_CHAR_INFO_DH2 SendData(m_UserDbNum, vCharInfo[i]);
            
            msgpack::sbuffer PackBuffer;
            msgpack::pack(PackBuffer, SendData);

            NET_MSG_PACK MsgInner;
            if (MsgInner.SetData(NET_MSG_LOBBY_CHAR_INFO_DH2, PackBuffer))
                pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &MsgInner);
            else
                sc::writeLogError("CharacterBasicInfoAgentAll SetData");
        }
        
        // 완료 정보 전송
        NET_LOBBY_USER_INFO_END_DH MsgLobbyUserInfoEndDH(m_UserDbNum);
        pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &MsgLobbyUserInfoEndDH);
        return NET_OK;
    }
}

CharacterBasicInfo::CharacterBasicInfo(int nUserNum, int nChaNum, DWORD AgentSlot, DWORD dwClient, const char* szUserIP, USHORT uPort)
    : m_UserDbNum(nUserNum)
    , m_ChaDbNum(nChaNum)
    , m_AgentSlot(AgentSlot)
{	
    m_ClientSlot = dwClient;
    m_strUserIP = szUserIP;
    m_uPort     = uPort;
}

int CharacterBasicInfo::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	if (m_UserDbNum <= 0 || m_ChaDbNum <= 0)
        return NET_ERROR;
		
    NET_LOBBY_CHAR_INFO_DH MsgDH(m_UserDbNum, m_ChaDbNum, m_ClientSlot, m_AgentSlot, m_strUserIP, m_uPort); 
	if (m_pDbManager->GetChaBInfo(m_UserDbNum, m_ChaDbNum, &MsgDH.m_ChaData) == sc::db::DB_ERROR)
	{
        sc::writeLogError(
            sc::string::format(
                "CharacterBasicInfo::Execute User %1% Char %2% not exist",
                m_UserDbNum, m_ChaDbNum));
		return NET_ERROR;
	}
	else
	{
        //CAgentServer::MsgSndChaBasicInfoBack
        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, MsgDH);

        NET_MSG_PACK InnerMag;
        if (InnerMag.SetData(NET_MSG_LOBBY_CHAR_INFO_DH, PackBuffer))
        {
            CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
		    pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &InnerMag);
		    return NET_OK;
        }
        else
        {
            sc::writeLogError("CharacterBasicInfo SetData");
            return NET_ERROR;
        }
	}
}


CharacterBasicInfoNew::CharacterBasicInfoNew(int nUserNum, int nChaNum, DWORD AgentSlot, DWORD dwClient, const char* szUserIP, USHORT uPort)
    : m_UserDbNum(nUserNum)
    , m_ChaDbNum(nChaNum)
    , m_AgentSlot(AgentSlot)
{	
    m_ClientSlot = dwClient;
    m_strUserIP = szUserIP;
    m_uPort     = uPort;
}

int CharacterBasicInfoNew::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    if (m_UserDbNum <= 0 || m_ChaDbNum <= 0)
        return NET_ERROR;

	int nRet;
	int nMigrationTarget = 0;
	NET_LOBBY_CHAR_INFO_NEW_DH MsgDH( m_UserDbNum, m_ChaDbNum, m_ClientSlot, m_AgentSlot, m_strUserIP.c_str(), m_uPort );
	nRet = m_pDbManager->GetChaBInfo( m_UserDbNum, m_ChaDbNum, &MsgDH.m_ChaData );

    if ( sc::db::DB_ERROR == nRet )
    {
        sc::writeLogError(
            sc::string::format(
            "CharacterBasicInfoNew::Execute User %1% Char %2% not exist",
            m_UserDbNum, m_ChaDbNum));
        return NET_ERROR;
    }
    else
    {
        //CAgentServer::MsgSndChaBasicInfoBack
        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, MsgDH);

        NET_MSG_PACK MsgInner;
        if (MsgInner.SetData(NET_MSG_LOBBY_NEW_CHAR_INFO_DH, PackBuffer))
        {
            CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
            pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &MsgInner);
            return NET_OK;
        }
        else
        {
            sc::writeLogError("CharacterBasicInfoNew SetData");
            return NET_ERROR;
        }
    }
}


CharacterInfoAndJoinAgent::CharacterInfoAndJoinAgent(
    DWORD dwClientID,
    DWORD dwUserID,
    DWORD dwUserLvl,
    __time64_t tPREMIUM,
    __time64_t tCHATBLOCK,
    DWORD dwServerID,
    DWORD dwCharID,
	int nUse2ndPass,
	const char *sz2ndPass)    // 2차비번
    : m_dwClientID(dwClientID)
    , m_tPremiumDate(tPREMIUM)
    , m_tChatBlock(tCHATBLOCK)
{
    m_pCharAgentData = new GLCHARAG_DATA;
	m_pCharAgentData->m_UserDbNum   = dwUserID;
	m_pCharAgentData->m_dwUserLvl  = dwUserLvl;
	m_pCharAgentData->m_dwServerID = dwServerID;
	m_pCharAgentData->m_CharDbNum   = dwCharID;
	m_str2ndPassword = sz2ndPass; // 2차비번
	m_nUse2ndPass = nUse2ndPass;
}

CharacterInfoAndJoinAgent::~CharacterInfoAndJoinAgent()
{
    delete m_pCharAgentData;
}

int CharacterInfoAndJoinAgent::Execute(NetServer* pServer)
{
	int nRetCode(0);

	//	Note : 내부 메시지 발생.
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);

	EMSERVICE_PROVIDER sp = pAgentServer->GetServiceProvider();

	// 대만의 경우 캐릭터 선택시 2차비번 확인
	// GS, KR의 경우 2차비번 적용
	if( sp == SP_TAIWAN ||  sp == SP_GS ||  sp == SP_KOREA ||  sp == SP_KOREA_TEST ||  sp == SP_OFFICE_TEST )
	{
		if( m_nUse2ndPass != 0 )
		{
			if( m_str2ndPassword.empty() )
			{
				GLMSG::NET_IN_CHAJOIN_2NDPASSWORD NetMsg;
				NetMsg.m_dwClientID = m_dwClientID;
				NetMsg.m_dwUserDBNum = m_pCharAgentData->m_UserDbNum;
				NetMsg.m_nResult = EMPASS_FAIL;
				pAgentServer->InsertMsg( DBACTION_CLIENT_ID, (char*) &NetMsg );

				sc::writeLogError(sc::string::format("2nd Password is Empty. UseDBNum %1%, ChaDBNum %2%", m_pCharAgentData->m_UserDbNum, m_pCharAgentData->m_CharDbNum));
				return NET_OK;
			}

			// 2차비번
			// 2차비번 확인
			if( m_nUse2ndPass == 1 )
				nRetCode = m_pDbManager->Certification2ndPassword(m_pCharAgentData->m_UserDbNum, m_pCharAgentData->m_CharDbNum, m_str2ndPassword.c_str());
			else if( m_nUse2ndPass == 2 )
				nRetCode = m_pDbManager->Update2ndPassword(m_pCharAgentData->m_UserDbNum, m_pCharAgentData->m_CharDbNum, m_str2ndPassword.c_str());

			if( nRetCode == sc::db::DB_ERROR )
			{
				sc::writeLogError(sc::string::format("DB:Read 2nd Password Check FAIL. UseDBNum %1%, ChaDBNum %2%, Type %3%", m_pCharAgentData->m_UserDbNum, m_pCharAgentData->m_CharDbNum, m_nUse2ndPass));
				return NET_OK;
			}
			else
			{
				GLMSG::NET_IN_CHAJOIN_2NDPASSWORD NetMsg;
				NetMsg.m_dwClientID = m_dwClientID;
				NetMsg.m_dwUserDBNum = m_pCharAgentData->m_UserDbNum;
				NetMsg.m_nResult = nRetCode;
				pAgentServer->InsertMsg( DBACTION_CLIENT_ID, (char*) &NetMsg );

				// 2차비번 인증결과가 정상인 경우만 캐릭터 가져오기 진행
				if( nRetCode != sc::db::DB_OK )
					return NET_OK;
			}
		}
	}

	//	캐릭터 가져오기
	nRetCode = m_pDbManager->GetCharacterInfo(
						m_pCharAgentData->m_UserDbNum,
						m_pCharAgentData->m_CharDbNum,
						m_pCharAgentData);
	if ( nRetCode == sc::db::DB_ERROR )
	{
		sc::writeLogError(std::string("DB:Read Character DB failed"));
		return NET_OK;
	}

    m_pCharAgentData->m_tPREMIUM = m_tPremiumDate; // 프리미엄 만료기간
	m_pCharAgentData->m_tCHATBLOCK = m_tChatBlock;

        
    // Friend List --------------------------------------------------------
    {
	    std::vector<CHA_FRIEND> vecFriend;
	    nRetCode = m_pDbManager->GetChaFriend(m_pCharAgentData->ChaDbNum(), vecFriend);
	    if (nRetCode == sc::db::DB_ERROR)
	    {
		    sc::writeLogError(std::string("CharacterInfoAndJoinAgent Read Friend list failed"));
		    return NET_OK;
	    }

	    for (size_t i=0; i<vecFriend.size(); ++i)
            m_pCharAgentData->AddFriend(vecFriend[i]);

        // Friend Group -------------------------------------------------------
        std::vector<FRIEND_GROUP> FriendGroup;
        m_pDbManager->ChaFriendGroupSelect(m_pCharAgentData->ChaDbNum(), FriendGroup);
        for (size_t i=0; i<FriendGroup.size(); ++i)
            m_pCharAgentData->AddFriendGroup(FriendGroup[i]);
    }

    // Game Notify ------------------------------------------------------------    
    {
        std::vector<std::tr1::shared_ptr<gamelogic::NotifyBase> > vNotify;
        m_pDbManager->NotifySelect(m_pCharAgentData->ChaDbNum(), vNotify);
        for (size_t i=0; i<vNotify.size(); ++i)
            m_pCharAgentData->AddNotify(vNotify[i]);
    }
	
	nRetCode = m_pDbManager->LogUserAttend( m_pCharAgentData->m_UserDbNum, m_pCharAgentData->m_vecAttend );
	if ( nRetCode == sc::db::DB_ERROR )
	{
        sc::writeLogError(std::string("DB:Read Attend list failed"));
		return NET_OK;
	}

	int CharacterOption = 0;
	CharacterOption = m_pDbManager->GetCharacterOption( m_pCharAgentData->m_CharDbNum );
	if ( sc::db::DB_ERROR == CharacterOption )
	{
		sc::writeLogError( std::string( "DB:Read CharacterOption failed" ) );
		return NET_OK;
	}

	// 유저락커 잠금 상태 가져오기
// 	int UserInvenOption = 0;
// 	if ( sc::db::DB_ERROR == m_pDbManager->GetUserInvenOption( m_pCharAgentData->m_UserDbNum, UserInvenOption ) )
// 	{
// 		sc::writeLogError( std::string( "DB:Read GetUserInvenOption failed" ) );
// 		return NET_OK;
// 	}

	GLMSG::NET_CHAR_JOIN_DA NetMsg;
	NetMsg.m_dwClientID = m_dwClientID;
	NetMsg.m_pCharAgData = new GLCHARAG_DATA;
	NetMsg.m_CharacterOption = CharacterOption;
	*NetMsg.m_pCharAgData = *m_pCharAgentData;
	
	//pAgentServer->MsgAgentReqJoin( m_dwClientID, m_pCharAgentData );
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, (char*) &NetMsg );


	return NET_OK;
}


CharacterInfoAndJoinField::CharacterInfoAndJoinField( UINT nServerGroup, NET_GAME_JOIN_FIELDSVR *pMsg, SCHARDATA2* pCharData, float fMagicNum )
{	
	m_msgDF.m_pCharData = pCharData;

	if (!m_msgDF.m_pCharData)
	{
		sc::writeLogError(std::string("CharacterInfoAndJoinField() - m_pCharData is NULL."));
		return;
	}    

    m_msgDF.m_Country = pMsg->m_Country;
	m_msgDF.m_pCharData->SetUserID(pMsg->nUserNum);
	m_msgDF.m_pCharData->SetUserName(pMsg->szUID);
	
	m_msgDF.m_dwClientID				= DBACTION_CLIENT_ID;
	m_msgDF.m_PartyID					= pMsg->PartyID;
	m_msgDF.m_pCharData->m_dwUserLvl	= pMsg->dwUserLvl;
	m_msgDF.m_pCharData->m_dwServerID	= nServerGroup;
	m_msgDF.m_pCharData->m_CharDbNum	= pMsg->nChaNum;
	m_msgDF.m_dwGaeaID					= pMsg->dwGaeaID;	

	m_msgDF.m_sTIME.m_tPremiumDate		= pMsg->tPREMIUM;
	m_msgDF.m_sTIME.m_tChatBlock		= pMsg->tCHATBLOCK;

    m_msgDF.bChangeField                = pMsg->bChangeField;
	
	m_msgDF.m_sINFO.m_emType			= pMsg->emType;
	m_msgDF.m_sINFO.m_sStartMap			= pMsg->sStartMap;
	m_msgDF.m_sINFO.m_dwStartGate		= pMsg->dwStartGate;
	m_msgDF.m_sINFO.m_vStartPos			= pMsg->vStartPos;
	m_msgDF.m_sINFO.m_dwActState		= pMsg->dwActState;
	m_msgDF.m_sINFO.m_bUseArmSub		= pMsg->bUseArmSub;
	m_msgDF.m_sINFO.m_emIPBonus		= pMsg->emIPBonus;
	m_msgDF.m_sINFO.m_sChinaTime		= pMsg->sChinaTime;
	m_msgDF.m_sINFO.m_sVietnamSystem	= pMsg->sVietnamGainSystem;
	m_msgDF.m_sINFO.m_sEventTime		= pMsg->sEventTime;

	m_msgDF.fMagicNum = fMagicNum;

	m_msgDF.m_dwCountryID = pMsg->dwCountryID;

	m_msgDF.cKey = pMsg->ckey;
}

int CharacterInfoAndJoinField::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CharacterInfoAndJoinField::Execute. Use ADO");
        return NET_ERROR;
    }

    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);

	int nRetCode = sc::db::DB_ERROR;
	nRetCode = m_pDbManager->GetCharacterInfo(
		m_msgDF.m_pCharData->GetUserID(),
		(int) m_msgDF.m_pCharData->m_CharDbNum,
		m_msgDF.m_pCharData);

	//mjeon.ado
    m_pDbManager->GetCharacterInfoExtraWithAdo(
        m_msgDF.m_pCharData->GetUserID(),
        (int) m_msgDF.m_pCharData->m_CharDbNum,
        m_msgDF.m_pCharData);

	if (nRetCode == sc::db::DB_ERROR)
	{
		sc::writeLogError(sc::string::format("DB:Character DB read failed %1%", m_msgDF.m_pCharData->m_CharDbNum ));
        pFieldServer->ResetCreatingCharUserID(m_msgDF.m_pCharData->GetUserID());
		return NET_OK;
	}

	// 캐릭터 옵션 가져오기
	int CharacterOption = 0;
	CharacterOption = m_pDbManager->GetCharacterOption( static_cast< int >( m_msgDF.m_pCharData->m_CharDbNum ) );
	if ( sc::db::DB_ERROR == CharacterOption )
	{
		sc::writeLogError( sc::string::format( "DB:Character DB read failed(CharacterOption) %1%", m_msgDF.m_pCharData->m_CharDbNum ) );
		pFieldServer->ResetCreatingCharUserID( m_msgDF.m_pCharData->GetUserID() );
		return NET_OK;
	}
	m_msgDF.CharacterOption = CharacterOption;

	m_msgDF.m_pCharData->m_tPREMIUM = m_msgDF.m_sTIME.m_tPremiumDate;
	m_msgDF.m_pCharData->m_tCHATBLOCK = m_msgDF.m_sTIME.m_tChatBlock;
    //m_cCharData.m_tPREMIUM = m_tPremiumDate; // Premium Service 만료시간
	//m_cCharData.m_tCHATBLOCK = m_tChatBlock; // Chat Block 만료시간
    

	nRetCode = m_pDbManager->ReadUserInvenNew( m_msgDF.m_pCharData, m_msgDF.m_sINFO.m_emType == EMJOINTYPE_FIRST ? 1 : 0 );
	if (nRetCode == sc::db::DB_ERROR)
	{
		sc::writeLogError(sc::string::format("DB:UserInven DB read failed. %d",m_msgDF.m_pCharData->m_CharDbNum ));
        pFieldServer->ResetCreatingCharUserID(m_msgDF.m_pCharData->GetUserID());
		return NET_OK;
	}

	{
		GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
		if ( pGaeaServer )
		{
			pGaeaServer->UserLockerLock( m_msgDF.m_pCharData->GetUserID() );
		}
	}

	//sc::writeLogInfo ( sc::string::format( "[ User Inven ] [ Load User Inven, %1% ]", m_msgDF.m_pCharData->GetUserID() ) );

	if ( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == true )
	{
		// 실제 칩을 가지고 오는 것은 TexasHoldemChipSelectFromWeb 이나
		// 테이블에 캐릭터가 없는 경우 삽입해주는 용도로 쓰인다.
		nRetCode = m_pDbManager->TexasHoldemChipSelect( m_msgDF.m_pCharData );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:TexasHoldemChipSelect DB read failed. %d", m_msgDF.m_pCharData->m_CharDbNum ) );
			pFieldServer->ResetCreatingCharUserID( m_msgDF.m_pCharData->GetUserID() );
			return NET_OK;
		}

		LONGLONG chip = 0;
		nRetCode = m_pDbManager->TexasHoldemChipSelectFromWeb( pFieldServer->GetServerGroup(), m_msgDF.m_pCharData->m_CharDbNum, chip );
		if ( sc::db::DB_ERROR == nRetCode )
		{
			sc::writeLogError( sc::string::format( "DB:TexasHoldemChipSelectFromWeb DB read failed. %d", m_msgDF.m_pCharData->m_CharDbNum ) );
			pFieldServer->ResetCreatingCharUserID( m_msgDF.m_pCharData->GetUserID() );
			return NET_OK;
		}

		// 충전만 하고 게임을 한번도 하지 않은 경우 웹에는 칩이 0이다.
		// 홀덤 게임 시작을 했으면 웹에서 충전된 금액을 가지고 가면서 0으로 만들어 주는데
		// 이런 경우를 대비해서 게임디비의 칩에다가, 웹에서 가지고 온 칩을 더해서 클라에 알려주도록 한다.
		LONGLONG chipTotal = chip + m_msgDF.m_pCharData->GetTexasHoldemChip();
		m_msgDF.m_pCharData->SetTexasHoldemChip( chipTotal );
	}

    // 개인상점 위탁판매 슬롯 기한을 알아온다.
    if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() == true )
    {
        nRetCode = m_pDbManager->ConfignmentSaleSlotExpansionDateSelect( m_msgDF.m_pCharData );
        if ( sc::db::DB_ERROR == nRetCode )
        {
            sc::writeLogError( sc::string::format( "DB:ConfignmentSaleSlotExpansionDateSelect DB read failed. %d", m_msgDF.m_pCharData->m_CharDbNum ) );
            pFieldServer->ResetCreatingCharUserID( m_msgDF.m_pCharData->GetUserID() );
            return NET_OK;
        }
    }

	// 캐릭터 스텟/스킬 리셋 기능 확인
	{
		nRetCode = 0;
		nRetCode = m_pDbManager->GetChaResetInfo( m_msgDF.m_pCharData->m_CharDbNum );
		if ( nRetCode == 1 )
		{
			m_msgDF.bResetCharacter = true;
		}
	}
	
	// 기간제 아이템 시간 연장
	// 여기서 실행해야 한다.
	m_pDbManager->InspectionRewardGetMinute( m_msgDF.m_pCharData );

	pFieldServer->InsertMsg(DBACTION_CLIENT_ID, (char*) &m_msgDF);

	return NET_OK;
}


//! DB에서 캐릭터를 온라인 상태로 만든다.
CharacterOnline::CharacterOnline(int nChaNum)
    : m_nChaNum(nChaNum)
{	
}

int CharacterOnline::Execute(NetServer* pServer)
{
	return m_pDbManager->SetCharacterOnline(m_nChaNum);
}

//! 캐릭터를 오프라인 상태로 만든다.
CharacterOffline::CharacterOffline(int nChaNum)
    : m_nChaNum(nChaNum)
{
}

int CharacterOffline::Execute(NetServer* pServer)
{
	return m_pDbManager->SetCharacterOffline(m_nChaNum);
}



//------------------------ From DbActionLogic ------------------------




CharacterSave::CharacterSave()
: m_bAutoSave( false )
{
}

CharacterSave::CharacterSave( SCHARDATA2* pCHARDATA, bool bAutoSave /*= false*/ )
: m_bAutoSave( bAutoSave )
{
    SetInfo( pCHARDATA );
	m_dwGaeaID = GAEAID_NULL;
}

// WorldBattle에서 캐릭터 저장용으로 사용
CharacterSave::CharacterSave( SCHARDATA2* pCHARDATA, DWORD dwGaeaID )
: m_bAutoSave( false )
{
    SetInfo( pCHARDATA );
	m_dwGaeaID = dwGaeaID;
}


CharacterSave::~CharacterSave()
{
}

void CharacterSave::SetInfo( SCHARDATA2* pCHARDATA )
{
	if ( pCHARDATA )
        m_sCHARDATA.Assign( *pCHARDATA );
}

int CharacterSave::Execute( NetServer* pServer )
{
	if ( NULL == pServer )
        return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
		return NET_ERROR;

    if ( m_pDbManager->GetConnectionType() != db::ADO )
    {
        sc::writeLogError("CharacterSave::Execute. Use ADO.");
        return NET_ERROR;
    }

	int nDBRet = m_pDbManager->SaveCharacter( &m_sCHARDATA, m_bAutoSave );

	{
		GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
		if ( pGaeaServer && false == m_bAutoSave )
		{
			pGaeaServer->UserLockerUnLock( m_sCHARDATA.GetUserID() );
		}
	}

	if ( sc::db::DB_ERROR_OUTOFMEMORY == nDBRet )
	{
		if ( false == pFieldServer->GetReservationServerStop() )
		{
			pFieldServer->ResetSaveDBUserID( m_sCHARDATA.GetUserID() );
		}

		GLMSG::SNETPC_MSG_CHA_SAVE_END_FA NetMsgFA( m_sCHARDATA.GetUserID(), m_sCHARDATA.CharDbNum() );
		pFieldServer->SendAgent( &NetMsgFA );

		pFieldServer->SaveCharacterInSqlite( &m_sCHARDATA );
		return sc::db::DB_ERROR_OUTOFMEMORY;
	}
	else if( sc::db::DB_OK_CHAR_ERROR == nDBRet )
	{
		GLMSG::SNETPC_MSG_CHA_VALIDATION_FA NetMsg( m_sCHARDATA.GetUserID() );
		NetMsg.sStats = m_sCHARDATA.m_sStats;
		NetMsg.dwAttack = m_sCHARDATA.m_powerAttack;
		NetMsg.dwDefence = m_sCHARDATA.m_powerDefence;

		pFieldServer->SendAgent( &NetMsg );
	}
	else if ( sc::db::DB_OK == nDBRet )
	{
// 		switch ( pFieldServer->GetServiceProvider() )
// 		{
// 		case SP_GS:
// 			{
// 				m_pDbManager->SetMigrationTarget( m_sCHARDATA.m_CharDbNum );
// 			}
// 			break;
// 		default:
// 			break;
// 		}
	}

    m_pDbManager->SaveCharacterExtraWithAdo( &m_sCHARDATA );	//mjeon.ado

	// 필드 서버 자동 재시작 기능이 동작하면 서버가 종료된다. 원래 이 부분에서 SNETPC_MSG_CHA_SAVE_DF 를 InsertMsg 하지만 메시지 처리가 되지않는다.
	// 업데이트 스레드에서 EndUpdateThread 함수가 제일 밑에서 호출되기 때문에 업데이트 스레드에 있는 RecvMsgProcess, SendClientFinal 등의 메시지 처리 함수는 동작하기 때문이다.
	// 예전에 필리핀에서 캐릭터 저장을 할 때 디비 저장이 느릴 경우 다른 채널에서 접속을 시도할 경우 이전 데이터를 불러오는 문제가 발생해서 아이템 복사가 있었다.
	// 그래서 캐릭터 저장할 때 필드 서버는 에이전트에 캐릭터 저장중인 유저번호를 전송해서 필드 서버에서 캐릭터 저장이 완료될 때까지 다른 채널에서도 접속하지 못하도록
	// 하는 장치를 추가하였다. 필드 서버 자동 재시작 기능이 동작할 때 필드 서버에서 캐릭터 저장을 한다고 에이전트에 알리는 메시지는 전송되었지만, 캐릭터 저장이 완료
	// 되었다는 메시지는 에이전트에 전송되지 않아, 캐릭터 저장이 완료되었으나, 게임에 접속할 수 없는 문제를 발견하였고, 이를 해결하기 위해 서버 종료할 때
	// 클라이언트 접속 종료 함수를 업데이트 스레드 종료후로 내렸고(위치 변경하지 않으면 에이전트와 접속 종료되어 메시지 전송 불가), 
	// 캐릭터 저장 dbaction 에서 직접 에이전트로 캐릭터 저장 종료 메시지를 전송하도록 수정하였다.
	if ( false == pFieldServer->GetReservationServerStop() )
	{
		pFieldServer->ResetSaveDBUserID( m_sCHARDATA.GetUserID() );
	}
	
	GLMSG::SNETPC_MSG_CHA_SAVE_END_FA NetMsgFA( m_sCHARDATA.GetUserID(), m_sCHARDATA.CharDbNum() );
	pFieldServer->SendAgent( &NetMsgFA );


	if( m_dwGaeaID != GAEAID_NULL )
	{
		GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE NetMsgWB;
		NetMsgWB.dwGaeaID = m_dwGaeaID;
		pFieldServer->InsertMsg(DBACTION_CLIENT_ID, (char*) &NetMsgWB );
	}

	return NET_OK;
}

CharacterDbToAgentMessage::CharacterDbToAgentMessage () 
    : m_dwClientID(UINT_MAX)
{
	memset(m_aryMsg, 0, NET_DATA_BUFSIZE);
	NET_MSG_GENERIC *nmg = (NET_MSG_GENERIC*) m_aryMsg;
	nmg->SetSize( sizeof(NET_MSG_GENERIC) );
	nmg->nType = NET_MSG_BASE_ZERO;
}

CharacterDbToAgentMessage::CharacterDbToAgentMessage(DWORD dwClient, NET_MSG_GENERIC* nmg)
    : m_dwClientID(UINT_MAX)
{
    memset(m_aryMsg, 0, NET_DATA_BUFSIZE);
    NET_MSG_GENERIC* pMag = (NET_MSG_GENERIC*) m_aryMsg;
    pMag->SetSize( sizeof(NET_MSG_GENERIC) );
    pMag->nType = NET_MSG_BASE_ZERO;
    SetMsg(dwClient, nmg);
}

CharacterDbToAgentMessage::CharacterDbToAgentMessage(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
    : m_dwClientID(UINT_MAX)
{
    memset(m_aryMsg, 0, NET_DATA_BUFSIZE);
    NET_MSG_GENERIC* pMag = (NET_MSG_GENERIC*) m_aryMsg;
    pMag->SetSize( sizeof(NET_MSG_GENERIC) );
    pMag->nType = NET_MSG_BASE_ZERO;

    NET_MSG_PACK MsgPack;

    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
    {
        SetMsg(dwClient, &MsgPack);
    }
}

CharacterDbToAgentMessage::~CharacterDbToAgentMessage()
{
}

bool CharacterDbToAgentMessage::SetMsg ( DWORD dwClient, NET_MSG_GENERIC *nmg )
{
	if (nmg == NULL)
	{
		return false;
	}
	else
	{
		if (nmg->Size() < NET_DATA_BUFSIZE) 
		{
			m_dwClientID = dwClient;
			memcpy ( m_aryMsg, nmg, sizeof(BYTE) * nmg->Size() );
			return true;
		}
		else
		{
			return false;
		}
	}
}

int CharacterDbToAgentMessage::Execute (NetServer* pServer)
{
	if (!pServer)
	{
		return NET_ERROR;
	}
	else
	{
		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		//pTemp->SendAgent ( m_dwClientID, (NET_MSG_GENERIC*) m_aryMsg );

		pTemp->SendAgent((NET_MSG_GENERIC*) m_aryMsg);

		return NET_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 캐릭터추가카드 아이템
// 영구적으로 생성가능한 캐릭터 숫자를 1 증가시킨다.
CharacterCreateNumIncrease::CharacterCreateNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CharacterCreateNumIncrease::Execute(NetServer* pServer)
{    
	return m_pDbManager->UserChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// 해당 락커 만료시간을 정한다.
// 1 번 5 번 락커는 정할 수 없음.
// 2, 3, 4 번 락커만 만료시간을 정할 수 있다.
// 입력
// nChaNum : 캐릭터번호
// nStorageNum : 스토리지 번호
// tTime : 만료시간
// 출력
// -1 : sc::db::DB_ERROR
//  0 : sc::db::DB_OK
StorageDateSet::StorageDateSet(int nUserNum, int nStorageNum, __time64_t tTime)
{
	m_nUserNum    = nUserNum;
	m_nStorageNum = nStorageNum;
	m_tTime       = tTime;
}

int StorageDateSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaStorageDate(m_nUserNum, m_nStorageNum, m_tTime);
}

///////////////////////////////////////////////////////////////////////////////
// 해당 캐릭터의 인벤토리 추가 줄수를 세팅한다.
// 입력
// nChaNum : 캐릭터번호
// wInvenLine : 추가줄수 
// 주의 
// 최초 0 에서 한줄 추가시 1 을 입력
// 현재 한줄 상태에서 또 한줄 추가시 2 를 입력 (최종 줄수를 입력한다)
// 현재 두줄 상태에서 한줄 추가시 3 을 입력
// 최대 3까지만 입력가능.
InvenAddNumSet::InvenAddNumSet(int nChaNum, WORD wInvenLine)
{
	m_nChaNum    = nChaNum;
	m_wInvenLine = wInvenLine;
}

int InvenAddNumSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaInvenNum(m_nChaNum, m_wInvenLine);
}

///////////////////////////////////////////////////////////////////////////////
// 해당 사용자의 프리미엄 기간을 세팅한다.
PremiumTimeSet::PremiumTimeSet(int nUserNum, __time64_t tPremiumTime)
{
	m_nUserNum     = nUserNum;
	m_tPremiumTime = tPremiumTime;
}

int PremiumTimeSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetPremiumTime(m_nUserNum, m_tPremiumTime);
}

///////////////////////////////////////////////////////////////////////////
// 해당사용자의 채팅블록시간을 가져온다.
// nUserNum : 사용자번호
UserChatBlockTimeGet::UserChatBlockTimeGet(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int UserChatBlockTimeGet::Execute(NetServer* pServer)
{
	// 사용자의 채팅 블록시간을 가져온다.
	m_pDbManager->GetChatBlockTime(m_nUserNum);

	return 0;
}

///////////////////////////////////////////////////////////////////////////
// 해당사용자의 채팅블록시간을 설정한다.
// nUserNum : 사용자번호
// tBlockTime : 채팅 블록 만료시간
UserChatBlockTimeSet::UserChatBlockTimeSet(int nUserNum, __time64_t tBlockTime)
{
	m_nUserNum = nUserNum;
	m_tBlockTime = tBlockTime;
}

int UserChatBlockTimeSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChatBlockTime(m_nUserNum, m_tBlockTime);
}

///////////////////////////////////////////////////////////////////////////////
// 직전귀환 카드 사용을 위한 포지션을 저장한다.
CharacterLastCallPosSet::CharacterLastCallPosSet(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos)
{
	m_nChaNum = nChaNum;
	m_dwMapID = dwMapID;
	m_vPos    = vPos;
}

int CharacterLastCallPosSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetLastCallPos(m_nChaNum, m_dwMapID, m_vPos);
}

///////////////////////////////////////////////////////////////////////////
// 해당 캐릭터의 경험치를 세팅한다.
// 입력
// nChaNum : 캐릭터번호
// llExp : 경험치
// 주의 
// 캐릭터번호는 0 보다 커야한다.
// 경험치는 0 보다 커야한다.
CharacterExpSet::CharacterExpSet(int nChaNum, LONGLONG llExp)
    : m_nChaNum(0)
    , m_llExp(0)
{
	m_nChaNum = nChaNum;
	m_llExp = llExp;
}

int CharacterExpSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaExp(m_nChaNum, m_llExp);
}

///////////////////////////////////////////////////////////////////////////
// 해당 캐릭터의 경험치를 가져온다.
// 입력
// nChaNum : 캐릭터번호
// 리턴
// LONGLONG : 경험치
// 주의 
// 캐릭터번호는 0 보다 커야한다.
// 경험치가 0 보다 작으면 에러이다.
CharacterExpGet::CharacterExpGet(int nChaNum)
    : m_nChaNum(0)
{
	m_nChaNum = nChaNum;
}

int CharacterExpGet::Execute(NetServer* pServer)
{
	LONGLONG llChaExp;
	llChaExp = m_pDbManager->GetChaExp(m_nChaNum);
	if (llChaExp < 0)
	{
		// 에러
		return sc::db::DB_ERROR;
	}
	else
	{
		// 가져온 경험치로 작업을...
		return sc::db::DB_OK;
	}
}

/**
* 캐릭터 이름을 변경한다.
* m_dwChaNum 캐릭터번호
* m_szCharName 변경할 캐릭터이름
*/
CharacterRename::CharacterRename(
    DWORD dwClientID, 
    DWORD dwChaNum, 
    const char* szCharName,
    WORD wPosX,
    WORD wPosY )
{	
	memset(m_szCharName, 0, sizeof(char) * CHR_ID_LENGTH);
	m_ClientSlot = dwClientID;
	m_dwChaNum = dwChaNum;	
	m_wPosX = wPosX;
	m_wPosY = wPosY;
	if (szCharName != NULL)
	{
		StringCchCopy(m_szCharName, CHR_ID_LENGTH, szCharName);
	}
}

int CharacterRename::Execute (NetServer* pServer)
{
	if (!pServer)
	{
		return sc::db::DB_ERROR;
	}
	else
	{
		int nRESULT = m_pDbManager->RenameCharacter(m_dwChaNum, m_szCharName);

		GLMSG::SNETPC_INVEN_RENAME_FROM_DB NetMsgDB2Fld;
		NetMsgDB2Fld.dwCharID = m_dwChaNum;

		if ( nRESULT == sc::db::DB_OK ) // 이름변경 성공
		{
			NetMsgDB2Fld.emFB = EMINVEN_RENAME_FROM_DB_OK;
			NetMsgDB2Fld.wPosX = m_wPosX;
			NetMsgDB2Fld.wPosY = m_wPosY;
			//NetMsg.dwCharID = m_dwChaNum;
			StringCchCopy ( NetMsgDB2Fld.szName, CHAR_SZNAME, m_szCharName );
		}
		else if( nRESULT == -2 )
		{
			NetMsgDB2Fld.emFB = EMINVEN_RENAME_FROM_DB_BAD;
			StringCchCopy ( NetMsgDB2Fld.szName, CHAR_SZNAME, m_szCharName );
		}
		else // 이름변경 실패
		{
			NetMsgDB2Fld.emFB = EMINVEN_RENAME_FROM_DB_FAIL;
			StringCchCopy ( NetMsgDB2Fld.szName, CHAR_SZNAME, m_szCharName );
		}

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(m_ClientSlot, (char*) &NetMsgDB2Fld );
		return nRESULT;
	}
}


//! 캐릭터의 머리색을 변경한다.
//! m_dwChaNum : 캐릭터번호
//! m_nHairColor : 머리색깔
CharacterHairColorSet::CharacterHairColorSet(
								   DWORD dwChaNum,
								   int nHairColor )
{
	m_dwChaNum = dwChaNum;
	m_nHairColor = nHairColor;
}

int CharacterHairColorSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaHairColor(m_dwChaNum, m_nHairColor);
}

//! 캐릭터의 헤어스타일을 변경한다.
//! m_dwChaNum : 캐릭터 번호
//! m_nHairStyle : 헤어스타일
CharacterHairStyleSet::CharacterHairStyleSet(DWORD dwChaNum, int nHairStyle)
{
	m_dwChaNum = dwChaNum;
	m_nHairStyle = nHairStyle;
}

int CharacterHairStyleSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaHairStyle(m_dwChaNum, m_nHairStyle);
}

//! 캐릭터의 얼굴스타일을 변경한다.
//! m_dwChaNum : 캐릭터 번호
//! m_nFaceStyle : 얼굴스타일
CharacterFaceStyleSet::CharacterFaceStyleSet(DWORD dwChaNum, int nFaceStyle)
{
	m_dwChaNum = dwChaNum;
	m_nFaceStyle = nFaceStyle;
}

int CharacterFaceStyleSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaFaceStyle(m_dwChaNum, m_nFaceStyle);
}


// 캐릭터 성별을 변경한다.
CharacterGenderChangeSet::CharacterGenderChangeSet(
	DWORD dwChaNum, 
	int nClass,
	int nSex,
	int nFace, 
	int nHair, 
	int nHairColor  ) 
	: m_dwChaNum ( dwChaNum )
	, m_nClass ( nClass )
	, m_nSex ( nSex )
	, m_nFace ( nFace )
	, m_nHair ( nHair )
	, m_nHairColor ( nHairColor )
{
}

int CharacterGenderChangeSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaGenderChange(m_dwChaNum, m_nClass,m_nSex, m_nFace,m_nHair,m_nHairColor );
}

CharacterSchoolChangeSet::CharacterSchoolChangeSet(
	DWORD dwChaNum, 
	int nSchool ) 
	: m_dwChaNum ( dwChaNum )
	, m_nSchool ( nSchool )
{
}

int CharacterSchoolChangeSet::Execute(NetServer* pServer)
{
	return m_pDbManager->SetChaSchoolChange(m_dwChaNum, m_nSchool );
}


//! 캐릭터의 위치정보를 저장한다.
//! \param dwChaNum 캐릭터 번호
//! \param dwStartmapID 시작하는 맵 번호
//! \param dwStartGate 시작하는 게이트 번호
//! \param fStartPosX 시작하는 X 좌표
//! \param fStartPosY 시작하는 Y 좌표
//! \param fStartPosZ 시작하는 Z 좌표
//! \param dwSaveMapID 마지막으로 플레이한 맵 번호
//! \param fSavePosX 마지막으로 플레이한 X 좌표
//! \param fSavePosY 마지막으로 플레이한 Y 좌표
//! \param fSavePosZ 마지막으로 플레이한 Z 좌표
//! \param dwLastCallMapID 시작 귀환카드에 저장된 맵 번호
//! \param fLastCallPosX 시작 귀환카드에 저장된 X 좌표
//! \param fLastCallPosY 시작 귀환카드에 저장된 Y 좌표
//! \param fLastCallPosZ 시작 귀환카드에 저장된 Z 좌표
//! \return sc::db::DB_OK, sc::db::DB_ERROR
CharacterPositionSet::CharacterPositionSet(
    DWORD dwClientID,
    DWORD dwChaNum,
    DWORD dwFieldServer,
    DWORD dwStartMapID,
    DWORD dwStartGate,
    FLOAT fStartPosX,
    FLOAT fStartPosY,
    FLOAT fStartPosZ,
    DWORD dwSaveMapID,
    FLOAT fSavePosX,
    FLOAT fSavePosY,
    FLOAT fSavePosZ,
    DWORD dwLastCallMapID,
    FLOAT fLastCallPosX,
    FLOAT fLastCallPosY,
    FLOAT fLastCallPosZ )
    : m_dwChaNum(dwChaNum)
    , m_dwFieldServer(dwFieldServer)
    , m_dwStartMapID(dwStartMapID)
    , m_dwStartGate(dwStartGate)
    , m_fStartPosX(fStartPosX)
    , m_fStartPosY(fStartPosY)
    , m_fStartPosZ(fStartPosZ)
    , m_dwSaveMapID(dwSaveMapID)
    , m_fSavePosX(fSavePosX)
    , m_fSavePosY(fSavePosY)
    , m_fSavePosZ(fSavePosZ)
    , m_dwLastCallMapID(dwLastCallMapID)
    , m_fLastCallPosX(fLastCallPosX)
    , m_fLastCallPosY(fLastCallPosY)
    , m_fLastCallPosZ(fLastCallPosZ)
{
	m_ClientSlot = dwClientID;
}

int CharacterPositionSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
        return NET_ERROR;

	int nResult = m_pDbManager->SaveChaPos(m_dwChaNum,
		m_dwStartMapID,
		m_dwStartGate,
		m_fStartPosX,
		m_fStartPosY,
		m_fStartPosZ,
		m_dwSaveMapID,
		m_fSavePosX,
		m_fSavePosY,
		m_fSavePosZ,
		m_dwLastCallMapID,
		m_fLastCallPosX,
		m_fLastCallPosY,
		m_fLastCallPosZ);

	GLMSG::SNETPC_SAVECHARPOSRST_FROMDB NetMsgDb2Agt;

	if ( nResult == sc::db::DB_OK )
	{
		NetMsgDb2Agt.emFB		   = EMSAVECHARPOS_FB_OK;
		NetMsgDb2Agt.dwCharID	   = m_dwChaNum;
		NetMsgDb2Agt.dwFieldServer = m_dwFieldServer;
	}

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);
	pTemp->InsertMsg (m_ClientSlot, (char*) &NetMsgDb2Agt );

	return nResult;
}

UserAttendInsert::UserAttendInsert( 
									 DWORD dwUserID
									 , CString strUserName
									 , __time64_t tAttendTime
									 , int nComboAttend
									 , int nAttendReward
									 , SNATIVEID sItemID )
{
	m_dwUserID = dwUserID;
	m_strUserName = strUserName;
	m_nComboAttend = nComboAttend;
	m_nAttendReward = nAttendReward;
	m_tAttendTime = tAttendTime;
	m_sItemID = sItemID;
}

int UserAttendInsert::Execute(NetServer* pServer)
{
	return m_pDbManager->InsertUserAttend(
        m_dwUserID,
        m_strUserName,
        m_tAttendTime, 
		m_nComboAttend,
        m_nAttendReward,
        m_sItemID.wMainID,
        m_sItemID.wSubID);
}

ShopItemMapInsert::ShopItemMapInsert(int nItemMID, int nItemSID)
{
	m_nItemMID = nItemMID;
	m_nItemSID = nItemSID;
}

int ShopItemMapInsert::Execute(NetServer* pServer)
{
	return m_pDbManager->InsertShopItemMap ( m_nItemMID, m_nItemSID );
}

ThaiCafeClassCheck::ThaiCafeClassCheck( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP )
{
	m_ClientSlot	   = dwClientID;
	m_dwCClassType = dwCClassType;
	StringCchCopy(m_szIp, MAX_IP_LENGTH+1, pszIP);
}

int ThaiCafeClassCheck::Execute(NetServer* pServer)
{
	int nResult = m_pDbManager->ThaiUserClassCheck( m_szIp );
	if( nResult != m_dwCClassType )
	{
		GLMSG::SNET_CYBERCAFECLASS_UPDATE NetMsg;
		NetMsg.emIPBonus = (EMIP_BONUS_CLASS)nResult;

		CAgentServer* pTemp1 = reinterpret_cast<CAgentServer*> (pServer);
		pTemp1->InsertMsg ( m_ClientSlot, (char*) &NetMsg );
	}
	return nResult;
}

MyCafeClassCheck::MyCafeClassCheck( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP )
{
	m_ClientSlot	   = dwClientID;
	m_dwCClassType = dwCClassType;
	StringCchCopy(m_szIp, MAX_IP_LENGTH+1, pszIP);
}

int MyCafeClassCheck::Execute(NetServer* pServer)
{
	int nResult = m_pDbManager->MyUserClassCheck( m_szIp );
	if( nResult != m_dwCClassType )
	{
		GLMSG::SNET_CYBERCAFECLASS_UPDATE NetMsg;
		NetMsg.emIPBonus = (EMIP_BONUS_CLASS)nResult;

		CAgentServer* pTemp1 = reinterpret_cast<CAgentServer*> (pServer);
		pTemp1->InsertMsg ( m_ClientSlot, (char*) &NetMsg );
	}
	return nResult;
}

PhCafeClassCheck::PhCafeClassCheck( const DWORD dwClientID, const DWORD dwCClassType, const TCHAR *pszIP )
{
	m_ClientSlot	   = dwClientID;
	m_dwCClassType = dwCClassType;
	StringCchCopy(m_szIp, MAX_IP_LENGTH+1, pszIP);
}

int PhCafeClassCheck::Execute(NetServer* pServer)
{
	int nResult = m_pDbManager->PhUserClassCheck( m_szIp );
	if( nResult != m_dwCClassType )
	{
		GLMSG::SNET_CYBERCAFECLASS_UPDATE NetMsg;
		NetMsg.emIPBonus = (EMIP_BONUS_CLASS)nResult;

		CAgentServer* pTemp1 = reinterpret_cast<CAgentServer*> (pServer);
		pTemp1->InsertMsg ( m_ClientSlot, (char*) &NetMsg );
	}
	return nResult;
}

UserMoneyAddUpdate::UserMoneyAddUpdate( const DWORD dwUserID, const LONGLONG lnUserMoney )
{
	m_dwUserID = dwUserID;
	m_lnUserMoney = lnUserMoney;
}

int UserMoneyAddUpdate::Execute(NetServer* pServer)
{
	return m_pDbManager->UpdateUserMoneyAdd ( m_dwUserID, m_lnUserMoney );
}

UserLastInfoAddUpdate::UserLastInfoAddUpdate( const DWORD dwUserID, const LONGLONG lnUserMoney )
{
	m_dwUserID = dwUserID;
	m_lnUserMoney = lnUserMoney;
}

int UserLastInfoAddUpdate::Execute(NetServer* pServer)
{
	return m_pDbManager->UpdateUserLastInfoAdd ( m_dwUserID, m_lnUserMoney );
}

/*
CharacterPhoneNumberSet::CharacterPhoneNumberSet(
    DWORD dwClientNum,
    DWORD dwChaNum,
    const TCHAR* szPhoneNumber )
{
	if (dwChaNum > 0 &&
		m_szPhoneNumber != NULL)
	{
		m_dwClient = dwClientNum;
		m_dwChaNum = dwChaNum;
		StringCchCopy( m_szPhoneNumber, SMS_RECEIVER, szPhoneNumber );
		m_bSMS = true;
	}
	else
	{
		m_bSMS = false;
	}
}

int CharacterPhoneNumberSet::Execute(NetServer* pServer)
{
	if (m_bSMS == true)
	{
		int nRESULT = m_pDbManager->SetChaPhoneNumber( m_dwChaNum, m_szPhoneNumber );

		GLMSG::SNETPC_PHONE_NUMBER_FROM_DB NetMsgDB2Fld;
		NetMsgDB2Fld.dwCharID = m_dwChaNum;

		if( nRESULT == sc::db::DB_OK ) // 이름변경 성공
		{
			NetMsgDB2Fld.emFB = EMSMS_PHONE_NUMBER_FROM_DB_OK;
			StringCchCopy( NetMsgDB2Fld.szPhoneNumber, SMS_RECEIVER, m_szPhoneNumber );
		}
		else // 이름변경 실패
		{
			NetMsgDB2Fld.emFB = EMSMS_PHONE_NUMBER_FROM_DB_FAIL;
			StringCchCopy ( NetMsgDB2Fld.szPhoneNumber, SMS_RECEIVER, m_szPhoneNumber );
		}

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(m_dwClient, (char*) &NetMsgDB2Fld );

		return nRESULT;
	}
	else
	{
		return sc::db::DB_ERROR;
	}
}
*/

//! Character 의 환불/환불 불가 포인트를 가져온다
CharacterPointGet::CharacterPointGet(int ChaDbNum)
    : m_ChaDbNum(ChaDbNum)
{
}

CharacterPointGet::~CharacterPointGet()
{
}

int CharacterPointGet::Execute(NetServer* pServer)
{
    __int64 PointRefundable = 0;
    __int64 PointNotRefundable = 0;
    m_pDbManager->GetChaPoint(m_ChaDbNum, PointRefundable, PointNotRefundable);
    
    CHARACTER_POINT CharPoint(PointRefundable, PointNotRefundable);
    NET_CHAR_POINT_DH Msg(m_ChaDbNum, CharPoint);
    pServer->InsertMsg(DBACTION_CLIENT_ID, &Msg);

    return sc::db::DB_OK;
}


GetChaExtraInfo::GetChaExtraInfo(DWORD dwClientSlot, UINT nChaNum)
	:m_nChaNum(nChaNum)
{
	m_ClientSlot = dwClientSlot;
}

GetChaExtraInfo::~GetChaExtraInfo()
{
}

int GetChaExtraInfo::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	int nContributionPoint = 0;
	m_pDbManager->LoadChaExtraInfo(m_nChaNum, nContributionPoint);

	GLMSG::SNET_GET_CHA_EXTRA_INFO_DH msg;
	msg.dwChaNum			= m_nChaNum;
	msg.nContributionPoint	= nContributionPoint;

	CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    pCacheServer->InsertMsg(m_ClientSlot, &msg);

	return sc::db::DB_OK;
}


SetChaExtraInfo::SetChaExtraInfo(UINT nChaNum, int nContributionPoint)
	:m_nChaNum(nChaNum)
	,m_nContributionPoint(nContributionPoint)
{	
}

SetChaExtraInfo::~SetChaExtraInfo()
{
}

int SetChaExtraInfo::Execute(NetServer* pServer)
{
	if (!pServer)
        return NET_ERROR;

	int nContributionPoint = 0;
	return m_pDbManager->SaveChaExtraInfo(m_nChaNum, m_nContributionPoint);
}


GetChaDbNum::GetChaDbNum( DWORD ClientSlot, DWORD ReqSeq, const std::string& ChaName, charnamechache::REQUEST_TYPE ReqType, const char* ReqCharName /*= NULL */ )
    : m_ChaName(ChaName)
    , m_ReqSeq(ReqSeq)
    , m_ReqType(ReqType)
{
    m_ClientSlot = ClientSlot;

    if( ReqCharName != NULL )
        m_ReqCharName = ReqCharName;
}

GetChaDbNum::~GetChaDbNum()
{
}

int GetChaDbNum::Execute(NetServer* pServer)
{
    if (!pServer)
        return sc::db::DB_ERROR;
    
    CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
    if (!pAgentServer)
        return sc::db::DB_ERROR;

    std::string ChaName(m_ChaName);
    sc::string::SqlInjectionCheck(ChaName);
    int ChaDbNum = 0;
    int UserDbNum = 0;
    int nUserType = 0; // EMUSERTYPE USER_COMMON
    
    if (m_pDbManager->GetChaNum(ChaName.c_str(), &ChaDbNum, &UserDbNum) != sc::db::DB_ERROR)
    {        
        nUserType = m_pDbManager->GetUserType(UserDbNum);                
    }

    CHA_DB_NUM_REQ_DA Data(
        m_ClientSlot, 
        m_ReqSeq, 
        ChaDbNum, 
        UserDbNum, 
        static_cast<BYTE> (nUserType), 
        m_ChaName, 
        m_ReqType,
        m_ReqCharName);
    
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, Data);

    NET_MSG_PACK MsgDA;
    if (MsgDA.SetData(NET_MSG_CHAR_DB_NUM_REQ_DA, SendBuffer))
        pAgentServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDA);
    return sc::db::DB_OK;
}

GetinfofromChaDb::GetinfofromChaDb( DWORD ClientSlot, DWORD ReqSeq, const std::string& ChaName, charnamechache::REQUEST_TYPE ReqType, const char* ReqCharName /*= NULL */ )
: m_ChaName(ChaName)
, m_ReqSeq(ReqSeq)
, m_ReqType(ReqType)
{
	m_ClientSlot = ClientSlot;

	if( ReqCharName != NULL )
		m_ReqCharName = ReqCharName;
}

GetinfofromChaDb::~GetinfofromChaDb()
{
}

int GetinfofromChaDb::Execute(NetServer* pServer)
{
	if (!pServer)
		return sc::db::DB_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
	if (!pAgentServer)
		return sc::db::DB_ERROR;

	std::string ChaName(m_ChaName);
	sc::string::SqlInjectionCheck(ChaName);
	int ChaDbNum = 0;
	int UserDbNum = 0;
	int nUserType = 0; // EMUSERTYPE USER_COMMON

	int iClass = 0;
	int iLv    = 0;	
	
	if (m_pDbManager->GetinfofromCharDb(ChaName.c_str(), &ChaDbNum, &UserDbNum, &iClass, &iLv) != sc::db::DB_ERROR)
	{        
		nUserType = m_pDbManager->GetUserType(UserDbNum);                
	}

	CHA_DB_NUM_REQ_DA Data(
		m_ClientSlot, 
		m_ReqSeq, 
		ChaDbNum, 
		UserDbNum, 
		static_cast<BYTE> (nUserType), 
		m_ChaName, 
		iClass,
		iLv,
		m_ReqType,
		m_ReqCharName);

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, Data);

	NET_MSG_PACK MsgDA;
	if (MsgDA.SetData(NET_MSG_CHAR_DB_NUM_REQ_DA, SendBuffer))
		pAgentServer->InsertMsg(DBACTION_CLIENT_ID, &MsgDA);
	return sc::db::DB_OK;
}

CharacterInventoryAndGameMoneySave::CharacterInventoryAndGameMoneySave( SCHARDATA2* pCHARDATA )
{
	if ( pCHARDATA )
	{
		m_sCHARDATA.Assign( *pCHARDATA );
	}
}

CharacterInventoryAndGameMoneySave::~CharacterInventoryAndGameMoneySave()
{
}

int CharacterInventoryAndGameMoneySave::Execute( NetServer* pServer )
{
	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( !pFieldServer )
		return sc::db::DB_ERROR;

	int nDBRet = m_pDbManager->SaveInventoryAndGameMoney( &m_sCHARDATA );

	if( nDBRet == sc::db::DB_OK_CHAR_ERROR )
	{
		GLMSG::SNETPC_MSG_CHA_VALIDATION_FA NetMsgFA( m_sCHARDATA.GetUserID() );
		NetMsgFA.sStats = m_sCHARDATA.m_sStats;
		NetMsgFA.dwAttack = m_sCHARDATA.m_powerAttack;
		NetMsgFA.dwDefence = m_sCHARDATA.m_powerDefence;
		pFieldServer->SendAgent( &NetMsgFA );
		nDBRet = sc::db::DB_OK;
	}
	return nDBRet;
}

TransDBDataToWorldBattle::TransDBDataToWorldBattle(	SCHARDATA2 *pChaData2, DWORD dwChaNum ) 
	: m_dwChaNum ( dwChaNum )
{
	if ( pChaData2 )
		m_sCharData2.Assign( *pChaData2 );
}

int TransDBDataToWorldBattle::Execute(NetServer* pServer)
{
	sc::writeLogInfo( "[ Battle Server ] [ To start it that the database execute 'dbo.sp_TransWorldBattle'. ]" );

	int nDBRet = m_pDbManager->TransDBDataToWorldBattle(&m_sCharData2, m_dwChaNum );

	if( nDBRet != sc::db::DB_ERROR )
	{
		sc::writeLogInfo( "[ Battle Server ] [ Success 'dbo.sp_TransWorldBattle'. ]" );

		GLMSG::SNETPC_TRANS_TO_WORLDBATTLE_DB NetMsg;
		NetMsg.dwChaDBNum = m_dwChaNum;
		NetMsg.nRet = nDBRet;

		CFieldServer* pFieldSvr = reinterpret_cast<CFieldServer*> (pServer);
		pFieldSvr->InsertMsg(DBACTION_CLIENT_ID, (char*) &NetMsg );
	}
	else
	{
		sc::writeLogError( "[ Battle Server ] [ Fail 'dbo.sp_TransWorldBattle'. ]" );
	}

	/*
		// Skill 
    SaveCharacterSkill(pCharData2);
	// Skill Quick Slot
    SaveCharacterSkillQuickSlot(pCharData2);
	// Action Quick Slot
    SaveCharacterActionQuickSlot(pCharData2);	
	// Put on item
	if ( sc::db::DB_ERROR_OUTOFMEMORY == SaveCharacterPutOnItems(pCharData2) )
	{
		return sc::db::DB_ERROR_OUTOFMEMORY;
	}
		// Inventory
	if ( sc::db::DB_ERROR_OUTOFMEMORY == SaveCharacterInventory(pCharData2) )
	{
		return sc::db::DB_ERROR_OUTOFMEMORY;
	}
	SaveCharacterCoolTime(pCharData2);
    SaveCharacterSkillFact(pCharData2);

	SaveCharacterHireSummon(pCharData2);
    // 사용자 인벤토리 저장
    SaveUserInven(pCharData2);
	
	*/

	return nDBRet;
}

ReserveServerStop::ReserveServerStop()
{
}

int ReserveServerStop::Execute( NetServer* pServer )
{
	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( !pFieldServer )
		return sc::db::DB_ERROR;

	pFieldServer->ForceStop( SERVER_STOP_JUST_RESTART );
	return sc::db::DB_OK;
}


SetCharacterOption::SetCharacterOption( DWORD ChaNum, int CharacterOption )
: m_ChaNum( ChaNum )
, m_CharacterOption( CharacterOption )
{
}

int SetCharacterOption::Execute( NetServer* pServer )
{
	int Return = m_pDbManager->SetCharacterOption( m_ChaNum, m_CharacterOption );
	return Return;
}


CalculateExpCompressor::CalculateExpCompressor( DWORD dwChaDbNum, __int64 nCurExp, __int64 nCompressedExp )
	: m_dwChaNum ( dwChaDbNum )
	, m_llCurExp( nCurExp)
	, m_llCompressedExp( nCompressedExp )
{

}

int CalculateExpCompressor::Execute(NetServer* pServer)
{
	int nDBRet = m_pDbManager->CalculateExpCompressor( m_dwChaNum, m_llCurExp, m_llCompressedExp );

	return nDBRet;
}


UpdateUserLockerOption::UpdateUserLockerOption( DWORD UserNum, int Option )
: m_UserNum( UserNum )
, m_Option( Option )
{
}

UpdateUserLockerOption::~UpdateUserLockerOption()
{
}

int UpdateUserLockerOption::Execute( NetServer* pServer )
{
	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "UpdateUserLockerOption::Execute. Use ADO." );
		return NET_ERROR;
	}

	return m_pDbManager->UpdateUserLockerOption( m_UserNum, m_Option );
}

} // namespace db

