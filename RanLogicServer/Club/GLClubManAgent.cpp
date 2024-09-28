#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Club/GLClub.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/ClubMsg.h"
#include "../../RanLogic/Msg/FriendMsg.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Server/ServerCfg.h"
#include "../Util/GLItemLimit.h"
#include "./WorldBattleClubCountry.h"
#include "./GLClubAgent.h"
#include "./GLClubManAgent.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClubManAgent::GLClubManAgent(GLAgentServer* pServer)
    : m_pServer(pServer)
    , m_LastStorageSaveMin(0)
{
    m_pClubCountry = new wb::ClubCountry;
    m_AutoMemberKickTimer = GetTickCount();
}

GLClubManAgent::~GLClubManAgent()
{    
    //for (CLUB_AGENT_ITER iter=m_mapCLUB.begin(); iter!=m_mapCLUB.end(); ++iter)
    //{
    //	GLClubAgent* pClub = iter->second;
    //	SAFE_DELETE(pClub);
    //}

    delete m_pClubCountry;
    m_pClubCountry = NULL;
}

DWORD GLClubManAgent::FindClub(const std::string& strName)
{
    CLUBSNAME_ITER pos = m_mapCLUBNAME.find(strName);
    if (pos != m_mapCLUBNAME.end())
        return pos->second;
    else
        return CLUB_NULL;        
}

std::tr1::shared_ptr<GLClubAgent> GLClubManAgent::GetClub(DWORD dwID)
{
    CLUB_AGENT_ITER pos = m_mapCLUB.find(dwID);
    if (pos != m_mapCLUB.end())
    {
        return pos->second;
    }
    else
    {
        return std::tr1::shared_ptr<GLClubAgent> ((GLClubAgent*) 0);
    }
}

std::tr1::shared_ptr<GLClubAgent> GLClubManAgent::GetClub(const std::string& ClubName)
{
    DWORD ClubDbNum = FindClub(ClubName);
    if (ClubDbNum == CLUB_NULL)
        return std::tr1::shared_ptr<GLClubAgent> ((GLClubAgent*) 0);
    else
        return GetClub(ClubDbNum);
}

void GLClubManAgent::AddClub(std::tr1::shared_ptr<GLClubAgent> pClub)
{
    if (!pClub || !pClub->IsValid())
        return;

    m_mapCLUB.insert(CLUB_AGENT_VALUE(pClub->m_DbNum, pClub));
    m_mapCLUBNAME.insert(CLUBSNAME_VALUE(std::string(pClub->Name()), pClub->m_DbNum));

    if (m_pServer->GetServiceProvider() == SP_WORLD_BATTLE)
        m_pClubCountry->Insert(pClub->GetCountry(), pClub->DbNum(), true);
}

bool GLClubManAgent::DelClub(DWORD dwID)
{
    CLUB_AGENT_ITER pos = m_mapCLUB.find(dwID);
    if (pos == m_mapCLUB.end())
        return false;

    std::tr1::shared_ptr<GLClubAgent> pClub = pos->second;
    CLUBSNAME_ITER pos_name = m_mapCLUBNAME.find(std::string(pClub->Name()));
    if (pos_name != m_mapCLUBNAME.end())
        m_mapCLUBNAME.erase(pos_name);    
    m_mapCLUB.erase(pos);

    if (m_pServer->GetServiceProvider() == SP_WORLD_BATTLE)
        m_pClubCountry->Delete(pClub->DbNum());

    //SAFE_DELETE(pClub);
    return true;
}

bool GLClubManAgent::LoadFromDB(db::IDbManager* pDBMan, db::IDbManager* pAdoMan)
{
    if (!pDBMan)
        return false;

    if (!pAdoMan)
    {
        sc::writeLogError(std::string("GLClubManAgent::LoadFromDB pAdoMan==NULL"));
        return false;
    }

    std::vector<std::tr1::shared_ptr<GLClub> > vecClubInfo;
    int nRET = pDBMan->GetClubInfo(vecClubInfo);
    if (nRET != sc::db::DB_OK)
        return false;

    for (DWORD i=0; i<vecClubInfo.size(); ++i)
    {
        std::tr1::shared_ptr<GLClub> spGLClub = vecClubInfo[i];

        std::tr1::shared_ptr<GLClubAgent> spClubAgent(new GLClubAgent(m_pServer));

        *spClubAgent.get() = *spGLClub.get();

        DWORD ClubDbNum = spClubAgent->DbNum();

        LoadClubAuthFromDB(pAdoMan, ClubDbNum, spClubAgent);

        std::vector<GLCLUBMEMBER> vecMEMBER;
        nRET = pDBMan->GetClubMember(ClubDbNum, vecMEMBER);
        if (nRET != sc::db::DB_OK)
        {
            //SAFE_DELETE(spClubAgent);
            continue;
        }

        for (DWORD m=0; m<vecMEMBER.size(); ++m)
        {
            const GLCLUBMEMBER& sMEMBER = vecMEMBER[m];
            spClubAgent->AddMemberEx(sMEMBER.m_ChaDbNum, sMEMBER);

            //	클럽 CDM 권한
            if (spClubAgent->IsMemberFlgCDM(sMEMBER.m_ChaDbNum))
                spClubAgent->m_listCDMMembers.push_back(sMEMBER.m_ChaDbNum);
        }

        //	인증 가능자가 멤버에서 제외 되었을 경우 혹은 케릭터가 삭제되었을 경우. 인증권한 회수.
        if (spClubAgent->m_dwCDCertifior != 0)
        {
            if (!spClubAgent->IsMember(spClubAgent->m_dwCDCertifior))
                spClubAgent->m_dwCDCertifior = 0;
        }

        std::vector<GLCLUBBATTLE> vecBATTLE;
        nRET = pDBMan->GetClubBattleInfo(spGLClub->m_DbNum, vecBATTLE);
        if (nRET != sc::db::DB_OK)
        {
            // pClub memory leak?
            continue;
        }

        for (DWORD m=0; m<vecBATTLE.size(); ++m)
        {
            const GLCLUBBATTLE& sBATTLE = vecBATTLE[m];
            spClubAgent->ADDBATTLECLUB(sBATTLE);
        }

        // 클럽 등록.
        AddClub(spClubAgent);
    }

    // 클럽 배틀 관련 설정을 초기화 한다.
    InitClubBattle();

    // 연합 정보 가져오기.
    std::vector<std::pair<DWORD, DWORD> > vecAlliance;
    pDBMan->GetClubAlliance(vecAlliance);

    for ( size_t i=0; i<vecAlliance.size(); ++i )
    {
        const std::pair<DWORD,DWORD> &pairAlliance = vecAlliance[i];

        std::tr1::shared_ptr<GLClubAgent> pCLUB_A = GetClub ( pairAlliance.first );
        if ( !pCLUB_A )
            continue;

        std::tr1::shared_ptr<GLClubAgent> pCLUB_B = GetClub ( pairAlliance.second );
        if ( !pCLUB_B )
            continue;

        pCLUB_A->m_dwAlliance = pairAlliance.first;
        pCLUB_B->m_dwAlliance = pairAlliance.first;

        pCLUB_B->m_dwAllianceBattleWin = pCLUB_A->m_dwAllianceBattleWin;
        pCLUB_B->m_dwAllianceBattleLose = pCLUB_A->m_dwAllianceBattleLose;
        pCLUB_B->m_dwAllianceBattleDraw = pCLUB_A->m_dwAllianceBattleDraw;

        pCLUB_A->AddAlliance(pCLUB_A->m_dwAlliance, pCLUB_A->Name(), pCLUB_A->GetMasterName(), pCLUB_A->GetMarkVersion());
        pCLUB_A->AddAlliance(pCLUB_B->m_DbNum,      pCLUB_B->Name(), pCLUB_B->GetMasterName(), pCLUB_B->GetMarkVersion());
    }

    // 동맹배틀을 설정한다.
    InitAllianceBattle();

    // World battle 클럽/국가 정보 저장.
    //if (m_pServer->GetServiceProvider() == SP_WORLD_BATTLE)
    {
        for (CLUB_AGENT_CITER iter=m_mapCLUB.begin(); iter!=m_mapCLUB.end(); ++iter)
        {
            std::tr1::shared_ptr<GLClubAgent> pClub = iter->second;
            if (pClub)
            {
                m_pClubCountry->Insert(pClub->GetCountry(), pClub->DbNum(), false);
                m_pClubCountry->MakeCountryClub();
            }
        }
    }
    return true;
}

void GLClubManAgent::LoadClubAuthFromDB(
    db::IDbManager* pAdoMan,
    DWORD ClubDbNum,
    std::tr1::shared_ptr<GLClubAgent> pClub) 
{
    // 권한 설정 가져오기
    std::vector<club::AUTH_GRADE_DB> vAuth;
    vAuth.reserve(club::AUTHORITY_GRADE);
    pAdoMan->GetClubAuth(ClubDbNum, vAuth);
    if (vAuth.size() == 0)
    {
        // 권한이 설정된 것이 없다면. 과거 클럽이다.
        // 클럽 멤버의 모든 권한을 초기화 한다.
        pAdoMan->ResetClubPosition(ClubDbNum);
        // 설정파일에 설정된 기본 권한을 세팅한다.
        size_t GradeSize = GLogicData::GetInstance().GetClubGradeSize();
        for (size_t i=0; i<GradeSize; ++i)
            pClub->AddAuth(i, GLogicData::GetInstance().GetClubBaseGrade(i));
    }
    else
    {
        for (size_t i=0; i<vAuth.size(); ++i)
        {
            const club::AUTH_GRADE_DB& AuthGrade = vAuth[i];
            pClub->AddAuth(AuthGrade);
        }
    }
}

HRESULT GLClubManAgent::FrameMoveAgent(float fTime, float fElapsedTime, const CTime& CurrentTime)
{
    // 클럽 유지 관리
    std::vector<DWORD> vClubDel;

    for (CLUB_AGENT_ITER pos=m_mapCLUB.begin(); pos!=m_mapCLUB.end(); ++pos)
    {
        std::tr1::shared_ptr<GLClubAgent> spClub = pos->second;
        
        // 대련
        FrameMoveConfting(spClub, fElapsedTime);

        // 클럽 배틀
        FrameMoveClubBattle(spClub, fElapsedTime);

        // 해체
        FrameMoveDissolution(spClub, vClubDel);

		FrameMoveSaveClubStorageUser( spClub, CurrentTime );
    }

    // 클럽 정보 제거
    for (size_t i=0; i<vClubDel.size(); ++i)
        DelClub(vClubDel[i]);

    // Check auto club member kick out
    FrameMoveAutoKick();

    FrameMoveSaveClubStorage(CurrentTime);
    return S_OK;
}

//! 클럽 게임머니/수수료/창고 정보를 주기적으로 저장한다.
void GLClubManAgent::FrameMoveSaveClubStorage(const CTime& CurrentTime)
{
    if (m_LastStorageSaveMin == CurrentTime.GetMinute())
        return;

    m_LastStorageSaveMin = CurrentTime.GetMinute();

    if (m_LastStorageSaveMin == 1 || m_LastStorageSaveMin == 11 || m_LastStorageSaveMin == 21 || 
        m_LastStorageSaveMin == 31 || m_LastStorageSaveMin == 41 || m_LastStorageSaveMin == 51)
    {
        for (CLUB_AGENT_ITER iter=m_mapCLUB.begin(); iter!= m_mapCLUB.end(); ++iter)
        {
            std::tr1::shared_ptr<GLClubAgent> pClub = iter->second;
            if (pClub)
            {
                DWORD ClubDbNum = pClub->DbNum();            
                if (pClub->IsStorageUpdate() && pClub->IsValidStorage())
                    pClub->SaveStorageToDB();
            }
        }
    }
}

void GLClubManAgent::FrameMoveSaveClubStorageUser( std::tr1::shared_ptr< GLClubAgent > pClub, const CTime& CurrentTime )
{
	if ( pClub )
	{
		if ( GAEAID_NULL != pClub->GetClubStorageUser() && 180 < _difftime64( CurrentTime.GetTime(), pClub->GetLastClubStorageUseTime() ) )
		{
			sc::writeLogWarn( sc::string::format( "FrameMoveSaveClubStorageUser(item) club %1% cha %2%", pClub->m_DbNum, pClub->GetClubStorageUser() ) );
			pClub->ResetClubStorageUser();
		}
		if ( GAEAID_NULL != pClub->GetClubStorageMoneyUser() && 180 < _difftime64( CurrentTime.GetTime(), pClub->GetLastClubStorageMoneyUseTime() ) )
		{
			sc::writeLogWarn( sc::string::format( "FrameMoveSaveClubStorageUser(money) club %1% cha %2%", pClub->m_DbNum, pClub->GetClubStorageUser() ) );
			pClub->ResetClubStorageMoneyUser();
		}
	}
}

void GLClubManAgent::SaveStorage2DbAll()
{
    for (CLUB_AGENT_ITER iter=m_mapCLUB.begin(); iter!= m_mapCLUB.end(); ++iter)
    {
        std::tr1::shared_ptr<GLClubAgent> pClub = iter->second;
        if (pClub)
        {
            if (pClub->IsValidStorage())
            {
                pClub->SaveStorageToDB();
            }
        }
    }
}

HRESULT GLClubManAgent::EndClubBattle( DWORD dwClubID_P, DWORD dwClubID_S )
{
    //  수정 요망
    //	클럽 유지 관리.
    GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsg;					
    GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltA;
    GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltB;

    GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumA;
    GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumB;

    std::tr1::shared_ptr<GLClubAgent> pClub_P = GetClub( dwClubID_P );
    if (!pClub_P)
        return E_FAIL;

    std::tr1::shared_ptr<GLClubAgent> pClub_S = GetClub( dwClubID_S );
    if (!pClub_S)
        return E_FAIL;

    GLCLUBBATTLE_DEL* pClubBattleDel = pClub_P->GetClubBattleDel( dwClubID_S );	
    if ( !pClubBattleDel )
        return E_FAIL;
    GLCLUBBATTLE& sClubBattle = pClubBattleDel->m_sClubBattle;

    //CString strText;
    std::string strText;

    if ( sClubBattle.m_wKillPoint == sClubBattle.m_wDeathPoint )
    {
        NetMsg.emFB = EMCLUB_BATTLE_OVER_DRAW;
        NetMsg.dwWinClubID = pClub_P->m_DbNum;
        NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

        NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_DRAW;
        NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_DRAW;
        EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
        EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
        strText = sc::string::format( ID2SERVERTEXT("CLUB_BATTLE_OVER_DRAW"), pClub_P->Name(), sClubBattle.m_szClubName );

        pClub_P->m_dwBattleDraw++;
        pClub_S->m_dwBattleDraw++;
    }
    else if ( sClubBattle.m_wKillPoint > sClubBattle.m_wDeathPoint ) 
    {
        NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
        NetMsg.dwWinClubID = pClub_P->m_DbNum;
        NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

        NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_WIN;
        NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_LOSE;
        EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
        EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
        strText = sc::string::format( ID2SERVERTEXT("CLUB_BATTLE_OVER_WIN"), pClub_P->Name(), sClubBattle.m_szClubName );

        pClub_P->m_dwBattleWin++;
        pClub_S->m_dwBattleLose++;
    }
    else
    {
        NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
        NetMsg.dwWinClubID = sClubBattle.m_dwCLUBID;
        NetMsg.dwLoseClubID = pClub_P->m_DbNum;

        NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_LOSE;
        NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_WIN;
        EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
        EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;
        strText = sc::string::format( ID2SERVERTEXT("CLUB_BATTLE_OVER_WIN"), sClubBattle.m_szClubName, pClub_P->Name() );

        pClub_P->m_dwBattleLose++;
        pClub_S->m_dwBattleWin++;
    }

    NetMsgCltA.dwClubID = sClubBattle.m_dwCLUBID;
    NetMsgCltA.wKillPoint = sClubBattle.m_wKillPoint;
    NetMsgCltA.wDeathPoint = sClubBattle.m_wDeathPoint;

    NetMsgCltB.dwClubID = pClub_P->m_DbNum;
    NetMsgCltB.wKillPoint = sClubBattle.m_wDeathPoint;
    NetMsgCltB.wDeathPoint = sClubBattle.m_wKillPoint;

    m_pServer->SENDTOALLCHANNEL ( &NetMsg );

    m_pServer->SENDTOCLUBCLIENT ( pClub_P->m_DbNum, &NetMsgCltA );
    m_pServer->SENDTOCLUBCLIENT ( sClubBattle.m_dwCLUBID, &NetMsgCltB );

    GLMSG::SNET_SERVER_GENERALCHAT SendData;
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

    strText = sc::string::format(
        ID2SERVERTEXT("CLUB_BATTLE_RESULT"),
        pClub_P->Name(), 
        pClub_P->m_dwBattleWin,
        pClub_P->m_dwBattleLose,
        pClub_P->m_dwBattleDraw);
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }


    strText = sc::string::format(
        ID2SERVERTEXT("CLUB_BATTLE_RESULT"),
        pClub_S->Name(), 
        pClub_S->m_dwBattleWin,
        pClub_S->m_dwBattleLose,
        pClub_S->m_dwBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

    //	db에 저장 ( A기준, B기준 저장 )
    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pClub_P->m_DbNum,
                sClubBattle.m_dwCLUBID,
                EnumA, 
                sClubBattle.m_wKillPoint,
                sClubBattle.m_wDeathPoint,
                sClubBattle.m_bAlliance)));

    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                sClubBattle.m_dwCLUBID,
                pClub_P->m_DbNum,
                EnumB, 
                sClubBattle.m_wDeathPoint,
                sClubBattle.m_wKillPoint,
                sClubBattle.m_bAlliance)));
    return S_OK;
}

void GLClubManAgent::InitAllianceBattle ()
{
    CLUB_AGENT_ITER pos = m_mapCLUB.begin();
    CLUB_AGENT_ITER end = m_mapCLUB.end();

    for ( ; pos!=end; ++pos )
    {
        std::tr1::shared_ptr<GLClubAgent> pClub = pos->second;

        if (!pClub->IsChief())
            continue;

        CLUB_BATTLE_ITER pos_B = pClub->m_mapBattle.begin();
        CLUB_BATTLE_ITER end_B = pClub->m_mapBattle.end();
        for ( ; pos_B!=end_B; ++pos_B)
        {
            GLCLUBBATTLE& sClubBattle = (*pos_B).second;

            if ( !sClubBattle.m_bAlliance )
                continue;

            CLUB_ALLIANCE_ITER pos_A = pClub->m_setAlliance.begin();
            CLUB_ALLIANCE_ITER end_A = pClub->m_setAlliance.end();
            for ( ; pos_A!=end_A; ++pos_A)
            {
                const GLCLUBALLIANCE& sALLIANCE = pos_A->second;
                std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub ( sALLIANCE.m_dwID );

                if ( !pCLUB )
                    continue;
                if ( pClub->m_DbNum == pCLUB->m_DbNum )
                    continue;

                pCLUB->ADDBATTLECLUB( sClubBattle );
            }
        }
    }
}

HRESULT GLClubManAgent::EndAllianceBattle ( DWORD dwClubID_P, DWORD dwClubID_S )
{    
    GLMSG::SNET_CLUB_BATTLE_OVER_FLD NetMsg;					
    GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltA;
    GLMSG::SNET_CLUB_BATTLE_OVER_CLT NetMsgCltB;

    GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumA;
    GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumB;

    std::tr1::shared_ptr<GLClubAgent> pClub_P = GetClub( dwClubID_P );
    if ( !pClub_P )
        return E_FAIL;

    std::tr1::shared_ptr<GLClubAgent> pClub_S = GetClub( dwClubID_S );
    if ( !pClub_S )
        return E_FAIL;

    GLCLUBBATTLE_DEL* pClubBattleDel = pClub_P->GetClubBattleDel( dwClubID_S );	
    if ( !pClubBattleDel ) return E_FAIL;
    GLCLUBBATTLE& sClubBattle = pClubBattleDel->m_sClubBattle;

    //CString strText;
    std::string strText;

    if ( sClubBattle.m_wKillPoint == sClubBattle.m_wDeathPoint )
    {
        NetMsg.emFB = EMCLUB_BATTLE_OVER_DRAW;
        NetMsg.dwWinClubID = pClub_P->m_DbNum;
        NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

        NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_DRAW;
        NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_DRAW;
        EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
        EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;

        strText = sc::string::format( ID2SERVERTEXT("ALLIANCE_BATTLE_OVER_DRAW"), pClub_P->Name(), sClubBattle.m_szClubName );

        pClub_P->m_dwAllianceBattleDraw++;
        pClub_S->m_dwAllianceBattleDraw++;
    }
    else if ( sClubBattle.m_wKillPoint > sClubBattle.m_wDeathPoint ) 
    {
        NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
        NetMsg.dwWinClubID = pClub_P->m_DbNum;
        NetMsg.dwLoseClubID = sClubBattle.m_dwCLUBID;

        NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_WIN;
        NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_LOSE;
        EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
        EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;

        strText = sc::string::format( ID2SERVERTEXT("ALLIANCE_BATTLE_OVER_WIN"), pClub_P->Name(), sClubBattle.m_szClubName );

        pClub_P->m_dwAllianceBattleWin++;
        pClub_S->m_dwAllianceBattleLose++;
    }
    else
    {
        NetMsg.emFB = EMCLUB_BATTLE_OVER_WIN;
        NetMsg.dwWinClubID = sClubBattle.m_dwCLUBID;
        NetMsg.dwLoseClubID = pClub_P->m_DbNum;

        NetMsgCltA.emFB = EMCLUB_BATTLE_OVER_LOSE;
        NetMsgCltB.emFB = EMCLUB_BATTLE_OVER_WIN;
        EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
        EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;

        strText = sc::string::format( ID2SERVERTEXT("ALLIANCE_BATTLE_OVER_WIN"), sClubBattle.m_szClubName, pClub_P->Name() );

        pClub_P->m_dwAllianceBattleLose++;
        pClub_S->m_dwAllianceBattleWin++;
    }


    //	전적 공유
    CLUB_ALLIANCE_ITER pos = pClub_P->m_setAlliance.begin();
    CLUB_ALLIANCE_ITER end = pClub_P->m_setAlliance.end();
    for ( ; pos!=end; ++pos )
    {

        const GLCLUBALLIANCE &sALLIANCE = pos->second;
        std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub ( sALLIANCE.m_dwID );
        if ( !pCLUB )
            continue;

        pCLUB->m_dwAllianceBattleWin = pClub_P->m_dwAllianceBattleWin; 
        pCLUB->m_dwAllianceBattleLose = pClub_P->m_dwAllianceBattleLose; 
        pCLUB->m_dwAllianceBattleDraw = pClub_P->m_dwAllianceBattleDraw; 
    }

    pos = pClub_S->m_setAlliance.begin();
    end = pClub_S->m_setAlliance.end();
    for ( ; pos!=end; ++pos )
    {

        const GLCLUBALLIANCE& sALLIANCE = pos->second;
        std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub ( sALLIANCE.m_dwID );
        if ( !pCLUB )
            continue;

        pCLUB->m_dwAllianceBattleWin = pClub_S->m_dwAllianceBattleWin; 
        pCLUB->m_dwAllianceBattleLose = pClub_S->m_dwAllianceBattleLose; 
        pCLUB->m_dwAllianceBattleDraw = pClub_S->m_dwAllianceBattleDraw; 
    }

    NetMsgCltA.dwClubID = sClubBattle.m_dwCLUBID;
    NetMsgCltA.wKillPoint = sClubBattle.m_wKillPoint;
    NetMsgCltA.wDeathPoint = sClubBattle.m_wDeathPoint;

    NetMsgCltB.dwClubID = pClub_P->m_DbNum;
    NetMsgCltB.wKillPoint = sClubBattle.m_wDeathPoint;
    NetMsgCltB.wDeathPoint = sClubBattle.m_wKillPoint;

    m_pServer->SENDTOALLCHANNEL ( &NetMsg );

    // 전체 동맹원들에게 모두 알림
    m_pServer->SENDTOALLIANCECLIENT ( pClub_P->m_DbNum, &NetMsgCltA );
    m_pServer->SENDTOALLIANCECLIENT ( sClubBattle.m_dwCLUBID, &NetMsgCltB );

    GLMSG::SNET_SERVER_GENERALCHAT SendData;
    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

    strText = sc::string::format(
        ID2SERVERTEXT("ALLIANCE_BATTLE_RESULT"),
        pClub_P->Name(), 
        pClub_P->m_dwAllianceBattleWin,
        pClub_P->m_dwAllianceBattleLose,
        pClub_P->m_dwAllianceBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

    strText = sc::string::format(
        ID2SERVERTEXT("ALLIANCE_BATTLE_RESULT"),
        pClub_S->Name(), 
        pClub_S->m_dwAllianceBattleWin,
        pClub_S->m_dwAllianceBattleLose,
        pClub_S->m_dwAllianceBattleDraw);

    if (SendData.SETTEXT(strText))
    {
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
    }

    //	db에 저장 ( A기준, B기준 저장 )    
    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                pClub_P->m_DbNum,
                sClubBattle.m_dwCLUBID,
                EnumA, 
                sClubBattle.m_wKillPoint,
                sClubBattle.m_wDeathPoint,
                sClubBattle.m_bAlliance)));

    m_pServer->AddGameDbJob(
        db::DbActionPtr(
            new db::ClubBattleEnd(
                sClubBattle.m_dwCLUBID,
                pClub_P->m_DbNum,
                EnumB, 
                sClubBattle.m_wDeathPoint,
                sClubBattle.m_wKillPoint,
                sClubBattle.m_bAlliance)));
    return S_OK;
}

// 배틀 정보에 이름을 입력한다.
bool GLClubManAgent::InitClubBattle()
{
    // 수정요망
    CLUB_AGENT_ITER pos = m_mapCLUB.begin();
    CLUB_AGENT_ITER end = m_mapCLUB.end();
    for ( ; pos!=end; ++pos )
    {
        std::tr1::shared_ptr<GLClubAgent> pClubA = pos->second;

        CLUB_BATTLE_ITER pos_B = pClubA->m_mapBattle.begin();
        CLUB_BATTLE_ITER end_B = pClubA->m_mapBattle.end();
        CLUB_BATTLE_ITER del_B;
        for ( ; pos_B!=end_B; pos_B )
        {
            GLCLUBBATTLE& sClubBattle = pos_B->second;
            del_B = pos_B++;

            std::tr1::shared_ptr<GLClubAgent> pClubB = GetClub( sClubBattle.m_dwCLUBID );
            if (pClubB)
                sClubBattle.SetName(pClubB->Name());

            __time64_t curTime = CTime::GetCurrentTime().GetTime();

            // 배틀 종료				
            if (sClubBattle.m_tEndTime < curTime)
            {
                EndClubBattleAgent(pClubA->m_DbNum, sClubBattle.m_dwCLUBID);

                pClubA->DELBATTLECLUB( (*del_B).first );	
                std::tr1::shared_ptr<GLClubAgent> pDelBattleClub = GetClub( sClubBattle.m_dwCLUBID );
                if (pDelBattleClub)
                    pDelBattleClub->DELBATTLECLUB( pClubA->m_DbNum );	
            }
        }
    }

    return true;
}

HRESULT GLClubManAgent::EndClubBattleAgent( DWORD dwClubID_P, DWORD dwClubID_S )
{
    // 수정요망
    GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumA;
    GLCLUBBATTLE::CLUB_BATTLE_ENUM EnumB;

    std::tr1::shared_ptr<GLClubAgent> pClub_P = GetClub( dwClubID_P );
    if ( !pClub_P )
        return E_FAIL;

    std::tr1::shared_ptr<GLClubAgent> pClub_S = GetClub( dwClubID_S );
    if ( !pClub_S )
        return E_FAIL;

    GLCLUBBATTLE* pClubBattle = pClub_P->GetClubBattle( dwClubID_S );
    if ( !pClubBattle )	return E_FAIL;

    if ( pClubBattle->m_bAlliance ) 
    {
        if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
        {
            EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
            EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;

            pClub_P->m_dwAllianceBattleDraw++;
            pClub_S->m_dwAllianceBattleDraw++;

        }
        else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
        {	
            EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
            EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;

            pClub_P->m_dwAllianceBattleWin++;
            pClub_S->m_dwAllianceBattleLose++;
        }
        else
        {
            EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
            EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;

            pClub_P->m_dwAllianceBattleLose++;
            pClub_S->m_dwAllianceBattleWin++;
        }
    }
    else
    {
        if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
        {
            EnumA = GLCLUBBATTLE::CLUB_BATTLE_DRAW;
            EnumB = GLCLUBBATTLE::CLUB_BATTLE_DRAW;

            pClub_P->m_dwBattleDraw++;
            pClub_S->m_dwBattleDraw++;

        }
        else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
        {	
            EnumA = GLCLUBBATTLE::CLUB_BATTLE_WIN;
            EnumB = GLCLUBBATTLE::CLUB_BATTLE_LOSE;

            pClub_P->m_dwBattleWin++;
            pClub_S->m_dwBattleLose++;
        }
        else
        {
            EnumA = GLCLUBBATTLE::CLUB_BATTLE_LOSE;
            EnumB = GLCLUBBATTLE::CLUB_BATTLE_WIN;

            pClub_P->m_dwBattleLose++;
            pClub_S->m_dwBattleWin++;
        }
    }

    //	db에 저장 ( P기준, S기준 저장 )
    m_pServer->AddGameDbJob(
        db::DbActionPtr(
        new db::ClubBattleEnd(
        pClub_P->m_DbNum,
        pClubBattle->m_dwCLUBID, 
        EnumA,
        pClubBattle->m_wKillPoint,
        pClubBattle->m_wDeathPoint,
        pClubBattle->m_bAlliance)));

    m_pServer->AddGameDbJob(
        db::DbActionPtr(
        new db::ClubBattleEnd(
        pClubBattle->m_dwCLUBID,
        pClub_P->m_DbNum,
        EnumB,
        pClubBattle->m_wDeathPoint,
        pClubBattle->m_wKillPoint,
        pClubBattle->m_bAlliance)));
    return S_OK;
}

void GLClubManAgent::DelAllianceBattle( DWORD dwClub_P, DWORD dwClub_S )
{
    std::tr1::shared_ptr<GLClubAgent> pCLUB_P = GetClub( dwClub_P );
    std::tr1::shared_ptr<GLClubAgent> pCLUB_S = GetClub( dwClub_S );

    if (!pCLUB_P || !pCLUB_S)
        return;

    CLUB_ALLIANCE_ITER pos = pCLUB_P->m_setAlliance.begin();
    CLUB_ALLIANCE_ITER end = pCLUB_P->m_setAlliance.end();
    for ( ; pos!=end; ++pos)
    {
        const GLCLUBALLIANCE& sALLIANCE = pos->second;
        std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub(sALLIANCE.m_dwID);
        if (pCLUB)
            pCLUB->DELBATTLECLUB( pCLUB_S->m_DbNum );
    }

    pos = pCLUB_S->m_setAlliance.begin();
    end = pCLUB_S->m_setAlliance.end();
    for ( ; pos!=end; ++pos )
    {
        const GLCLUBALLIANCE& sALLIANCE = pos->second;
        std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub(sALLIANCE.m_dwID);
        if (pCLUB)
            pCLUB->DELBATTLECLUB(pCLUB_P->m_DbNum);
    }
}
/*
void GLClubManAgent::ChaLevelUp(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel)
{
    std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(ClubDbNum);
    if (pClub)
    {
        if (pClub->MasterCharDbNum() == ChaDbNum)
            pClub->SetMasterCharLevel(ChaLevel);

        pClub->SetMemberState(ChaDbNum, ChaLevel);

        NET_CLUB_CHAR_LEVEL_CHANGE_AC MsgAC(ChaDbNum, ChaLevel);
        SendClubClient(ClubDbNum, &MsgAC);
    }
}
*/
const std::vector<wb::COUNTRY_CLUB_COUNT_INFO>& GLClubManAgent::GetCountry()
{
    return m_pClubCountry->GetCountry();
}

void GLClubManAgent::SendCountryClubInfo(DWORD ClientSlot, wb::EM_COUNTRY CountryCode)
{
    std::vector<DWORD> vClubDbNum;
    m_pClubCountry->GetClub(CountryCode, vClubDbNum);
    for (size_t i=0; i<vClubDbNum.size(); ++i)
    {
        DWORD ClubDbNum = vClubDbNum[i];
        std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(vClubDbNum[i]);
        SendCountryClubInfo(pClub, ClientSlot, jm::MSG_WORLD_BATTLE_CLUB_INFO);
    }

    json_spirit::Object JsonObjEnd;
    JsonObjEnd.push_back(json_spirit::Pair("M", jm::MSG_WORLD_BATTLE_CLUB_INFO_END));
    JsonObjEnd.push_back(json_spirit::Pair("1", CountryCode));
    NET_JSON MsgEndAC;
    if (MsgEndAC.SetData(json_spirit::write(JsonObjEnd)))
        m_pServer->SENDTOCLIENT(ClientSlot, &MsgEndAC);
}

void GLClubManAgent::SendCountryClubInfo(std::tr1::shared_ptr<GLClubAgent> pClub, DWORD ClientSlot, jm::EM_MSG_TYPE MsgType) 
{
    if (pClub)
    {
        json_spirit::Object JsonObj;
        JsonObj.push_back(json_spirit::Pair("M", MsgType));
        JsonObj.push_back(json_spirit::Pair("1", (int) pClub->GetCountry()));
        JsonObj.push_back(json_spirit::Pair("2", (int) pClub->DbNum()));
        JsonObj.push_back(json_spirit::Pair("3", pClub->Name()));
        JsonObj.push_back(json_spirit::Pair("4", (int) pClub->GetRank()));
        JsonObj.push_back(json_spirit::Pair("5", pClub->GetMasterLevel()));
        JsonObj.push_back(json_spirit::Pair("6", pClub->GetMasterName()));
        JsonObj.push_back(json_spirit::Pair("7", pClub->GetOrganizeDate()));
        JsonObj.push_back(json_spirit::Pair("8", (int) pClub->GetMemberNum()));
        JsonObj.push_back(json_spirit::Pair("9", pClub->GetNotice()));
        NET_JSON MsgAC;
        if (MsgAC.SetData(json_spirit::write(JsonObj)))
            m_pServer->SENDTOCLIENT(ClientSlot, &MsgAC);
    }
}

void GLClubManAgent::SendClubClient(DWORD ClubDbNum, NET_MSG_GENERIC* pMsg)
{
    if (ClubDbNum != CLUB_NULL && pMsg)
    {
        std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(ClubDbNum);
        if (pClub)
            pClub->SendClubClient(pMsg);
    }
}

void GLClubManAgent::SendClubClient(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    if (dwClubID != CLUB_NULL)
    {
        std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(dwClubID);
        if (pClub)
            pClub->SendClubClient(MsgType, PackBuffer, UseCrc);
    }
}
/*
void GLClubManAgent::SetMemberMap(DWORD ClubDbNum, DWORD ChaDbNum, const SNATIVEID& MapId)
{
    std::tr1::shared_ptr<GLClubAgent> pClub = GetClub(ClubDbNum);
    if (pClub)
        pClub->SetMemberState(ChaDbNum, MapId);
}
*/
void GLClubManAgent::SAVESTORAGE2DB(DWORD ClubDbNum)
{
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(ClubDbNum);
    if (spClub)
        spClub->SaveStorageToDB();
}

void GLClubManAgent::SendClubInfoToAllMember(DWORD ClubDbNum)
{
    // 클럽원들에게 클럽 정보 전송
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(ClubDbNum);
    if (spClub)
        spClub->SendClubInfoToAllMember();
}

void GLClubManAgent::SendClubInfo(DWORD ClientSlot, DWORD ClubDbNum)
{
    std::tr1::shared_ptr<GLClubAgent> spClub = GetClub(ClubDbNum);
    if (spClub)
        spClub->SendClubInfo(ClientSlot);
	// Club info end
    GLMSG::NET_LOBBY_CLUB_INFO_END MsgEnd;
    m_pServer->SENDTOCLIENT(ClientSlot, &MsgEnd);
}

void GLClubManAgent::SendClubInfoAuth(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot)
{
    if (spClub)
        spClub->SendClubInfoAuth(ClientSlot);
}

void GLClubManAgent::SendClubInfoBattle(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot)
{
    if (spClub)
        spClub->SendClubInfoBattle(ClientSlot);
}

void GLClubManAgent::SendClubInfoAlliance(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot) 
{
    if (spClub)
        spClub->SendClubInfoAlliance(ClientSlot);
}

void GLClubManAgent::SendClubInfoBase(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot) 
{
    if (spClub)
        spClub->SendClubInfoBase(ClientSlot);    
}

void GLClubManAgent::SendClubInfoMember(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot)
{
    if (spClub)
        spClub->SendClubInfoMember(ClientSlot);
}

void GLClubManAgent::FrameMoveConfting( std::tr1::shared_ptr<GLClubAgent> pClub, float fElapsedTime )
{
    // 대련 패배 점검
    bool bCONFT_END(false);
    std::string strCLUB_A;
    std::string strCLUB_B;
    EMCONFRONT_END emEND_A(EMCONFRONT_END_CLOSS);
    EMCONFRONT_END emEND_B(EMCONFRONT_END_CWIN);

    if ( pClub->ISCONFTING() )
    {
        if ( pClub->ISCONFT_LOSS() )
        {
            std::tr1::shared_ptr<GLClubAgent> pConftClub = GetClub(pClub->m_dwconftCLUB);

            if ( !pConftClub )
            {
                emEND_A = EMCONFRONT_END_FAIL;
                emEND_B = EMCONFRONT_END_FAIL;
            }
            else if ( pConftClub->ISCONFT_LOSS() )
            {
                emEND_A = EMCONFRONT_END_CLOSS;
                emEND_B = EMCONFRONT_END_CLOSS;
            }

            bCONFT_END = true;
            strCLUB_A = pClub->Name();
            if (pConftClub)
                strCLUB_B = pConftClub->Name();


            // 클럽 대련이 종료됨을 알림 to (필드)
            GLMSG::SNETPC_CONFRONTCLB_END2_FLD NetMsgFld(pClub->m_DbNum, pClub->m_dwconftCLUB, emEND_A, emEND_B);
            m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

            {
                CLUBCONFT_ITER conft_pos = pClub->m_setConftCURRENT.begin();
                CLUBCONFT_ITER conft_end = pClub->m_setConftCURRENT.end();
                for ( ; conft_pos!=conft_end; ++conft_pos )
                {
                    PGLCHARAG pMEMBER = m_pServer->GetCharByDbNum(*conft_pos);
                    if (pMEMBER)
                        pMEMBER->ResetConfrontOk();
                }
            }

            if ( pConftClub )
            {
                CLUBCONFT_ITER conft_pos = pConftClub->m_setConftCURRENT.begin();
                CLUBCONFT_ITER conft_end = pConftClub->m_setConftCURRENT.end();
                for ( ; conft_pos!=conft_end; ++conft_pos )
                {
                    GLCharAG* pMEMBER = m_pServer->GetCharByDbNum(*conft_pos);
                    if (pMEMBER)
                        pMEMBER->ResetConfrontOk();
                }
            }

            pClub->CLEARCONFT();
            if (pConftClub)
                pConftClub->CLEARCONFT();
        }


        pClub->m_fconftTIMER += fElapsedTime;
        if ( pClub->m_fconftTIMER > GLCONST_CHAR::fCONFRONT_TIME )
        {
            std::tr1::shared_ptr<GLClubAgent> pConftClub = GetClub(pClub->m_dwconftCLUB);

            emEND_A = (EMCONFRONT_END_CTIME);
            emEND_B = (EMCONFRONT_END_CTIME);

            bCONFT_END = true;
            strCLUB_A = pClub->Name();
            if (pConftClub)
                strCLUB_B = pConftClub->Name();

            //	Note : 클럽 대련이 종료됨을 알림 to (필드)
            //
            GLMSG::SNETPC_CONFRONTCLB_END2_FLD NetMsgFld(pClub->m_DbNum, pClub->m_dwconftCLUB, emEND_A, emEND_B);;
            m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

            {
                CLUBCONFT_ITER conft_pos = pClub->m_setConftCURRENT.begin();
                CLUBCONFT_ITER conft_end = pClub->m_setConftCURRENT.end();
                for ( ; conft_pos!=conft_end; ++conft_pos )
                {
                    GLCharAG* pMEMBER = m_pServer->GetCharByDbNum ( *conft_pos );
                    if (pMEMBER)
                        pMEMBER->ResetConfrontOk ();
                }
            }

            if ( pConftClub )
            {
                CLUBCONFT_ITER conft_pos = pConftClub->m_setConftCURRENT.begin();
                CLUBCONFT_ITER conft_end = pConftClub->m_setConftCURRENT.end();
                for ( ; conft_pos!=conft_end; ++conft_pos )
                {
                    GLCharAG* pMEMBER = m_pServer->GetCharByDbNum ( *conft_pos );
                    if (pMEMBER)
                        pMEMBER->ResetConfrontOk ();
                }
            }

            pClub->CLEARCONFT();
            if (pConftClub)
                pConftClub->CLEARCONFT();
        }

        if ( bCONFT_END )
        {
            std::string strCUR_MAP;
            //CString strText;
            std::string strText;

            PGLCHARAG pCHAR_MASTER = m_pServer->GetCharByDbNum ( pClub->MasterCharDbNum() );
            if (pCHAR_MASTER)
                strCUR_MAP = m_pServer->GetMapName(pCHAR_MASTER->GetCurrentMap().getBaseMapID());

            if ( emEND_B==EMCONFRONT_END_CWIN )
            {
                strText = sc::string::format(
                    ID2SERVERTEXT("EMCONFRONT_END_CWIN"), 
                    strCUR_MAP.c_str(),
                    strCLUB_B.c_str(),
                    strCLUB_A.c_str(),
                    strCLUB_B.c_str());
            }
            else
            {
                strText = sc::string::format(
                    ID2SERVERTEXT("EMCONFRONT_END_CDRAWN"), 
                    strCUR_MAP.c_str(),
                    strCLUB_A.c_str(),
                    strCLUB_B.c_str());
            }

            GLMSG::SNET_SERVER_GENERALCHAT SendData;
            if (SendData.SETTEXT(strText))
            {
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);
                m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
            }
        }
    }
}

void GLClubManAgent::FrameMoveClubBattle( std::tr1::shared_ptr<GLClubAgent> pClub, float fElapsedTime )
{
    if ( GLCONST_CHAR::bCLUB_BATTLE )
    {
        // Note : 클럽 배틀 점검.
        if ( pClub->GetBattleNum() > 0 )
        {
            CLUB_BATTLE_ITER del;
            CLUB_BATTLE_ITER pos = pClub->m_mapBattle.begin();
            CLUB_BATTLE_ITER end = pClub->m_mapBattle.end();
            for ( ; pos!=end; )
            {
                GLCLUBBATTLE &sClubBattle = (*pos).second;
                del = pos++;

                __time64_t curTime = CTime::GetCurrentTime().GetTime();

                //	배틀 종료				
                if ( sClubBattle.m_tEndTime < curTime )
                {
                    DWORD dwClubID = CLUB_NULL;

                    if ( sClubBattle.m_bAlliance ) 
                    {
                        dwClubID = pClub->m_dwAlliance;

                        DelAllianceBattle( dwClubID, sClubBattle.m_dwCLUBID );
                    }
                    else
                    {
                        pClub->DELBATTLECLUB( (*del).first );
                        std::tr1::shared_ptr<GLClubAgent> pCLUB = GetClub( sClubBattle.m_dwCLUBID );
                        if (pCLUB)
                            pCLUB->DELBATTLECLUB( pClub->m_DbNum );
                        dwClubID = pClub->m_DbNum;
                    }

                    GLCLUBBATTLE_DEL sClubBattleDel;
                    sClubBattleDel.m_sClubBattle = sClubBattle;

                    pClub->m_mapBattleDel[sClubBattle.m_dwCLUBID] = sClubBattleDel;

                    GLMSG::SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD NetMsg;
                    NetMsg.dwClubID_A = dwClubID;
                    NetMsg.dwClubID_B = sClubBattle.m_dwCLUBID;
                    m_pServer->SENDTOALLCHANNEL ( &NetMsg );
                }				
            }
        }

        // 종료 예정인 클럽 배틀 확인
        if ( pClub->GetBattleDelNum() > 0 )
        {
            CLUB_BATTLE_DEL_ITER del;
            CLUB_BATTLE_DEL_ITER pos = pClub->m_mapBattleDel.begin();
            CLUB_BATTLE_DEL_ITER end = pClub->m_mapBattleDel.end();

            for ( ; pos!=end; )
            {
                GLCLUBBATTLE_DEL &sClubBattleDel = (*pos).second;
                del = pos++;

                //	배틀 종료				
                if ( sClubBattleDel.m_bIsComplated )
                {
                    if ( sClubBattleDel.m_sClubBattle.m_bAlliance )
                    {
                        EndAllianceBattle( pClub->m_DbNum, sClubBattleDel.m_sClubBattle.m_dwCLUBID );
                    }
                    else
                    {							
                        EndClubBattle( pClub->m_DbNum, sClubBattleDel.m_sClubBattle.m_dwCLUBID );
                    }
                    pClub->m_mapBattleDel.erase(del);
                }

            }
        }


        pClub->m_fTIMER_BATTLE += fElapsedTime;
        // 클럽 배틀 관련 Timer 
        if ( pClub->m_fTIMER_BATTLE > 300.0f )
        {
            //	0 ~ 10.0f 사이의 값으로 초기화, 클럽마다 갱신 속도를 조절한다.
            pClub->m_fTIMER_BATTLE = (sc::Random::getInstance().GetFloatPercent()+0.1) / 10.0f;

            // Note : 클럽 배틀 점검.
            if ( pClub->GetBattleNum() > 0  )
            {
                CLUB_BATTLE_ITER pos = pClub->m_mapBattle.begin();
                CLUB_BATTLE_ITER end = pClub->m_mapBattle.end();
                for ( ; pos!=end; ++pos)
                {
                    GLCLUBBATTLE &sClubBattle = (*pos).second;

                    // Kill수 변화	
                    if ( sClubBattle.m_bKillUpdate )
                    {
                        GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE NetMsg;

                        NetMsg.dwBattleClubID = sClubBattle.m_dwCLUBID;
                        NetMsg.wKillPoint = sClubBattle.m_wKillPoint;
                        NetMsg.wDeathPoint = sClubBattle.m_wDeathPoint;						

                        m_pServer->SENDTOCLUBCLIENT(pClub->m_DbNum, &NetMsg);

                        GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_FLD	NetMsgFld;

                        NetMsgFld.dwClubID = pClub->m_DbNum;
                        NetMsgFld.dwBattleClubID = sClubBattle.m_dwCLUBID;
                        NetMsgFld.wKillPoint = sClubBattle.m_wKillPoint;
                        NetMsgFld.wDeathPoint = sClubBattle.m_wDeathPoint;

                        m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

                        sClubBattle.m_bKillUpdate = false;


                    }

                }
            }
        }


        // 시작 예정중인 클럽 배틀
        {
            CLUB_BATTLE_ITER del;
            CLUB_BATTLE_ITER pos = pClub->m_mapBattleReady.begin();
            CLUB_BATTLE_ITER end = pClub->m_mapBattleReady.end();
            CTime cCurTime( CTime::GetCurrentTime() );

            for ( ; pos!=end; )
            {
                GLCLUBBATTLE& sClubBattle = (*pos).second;
                del = pos++;

                if ( cCurTime > sClubBattle.m_tStartTime )
                {
                    // 클럽 배틀 시작
                    m_pServer->StartClubBattle( pClub->m_DbNum, sClubBattle );
                    pClub->m_mapBattleReady.erase( del );
                }
            }
        }
    }
}

void GLClubManAgent::FrameMoveDissolution(std::tr1::shared_ptr<GLClubAgent> pClub, std::vector<DWORD>& vClubDel)
{
    // 클럽 해체 점검
    if (!pClub->CheckDissolution())
        return;

    DWORD ClubDbNum = pClub->DbNum();

    EMCONFRONT_END emEND_A(EMCONFRONT_END_CLOSS);
    EMCONFRONT_END emEND_B(EMCONFRONT_END_CWIN);
    
    std::string strCLUB_A;
    std::string strCLUB_B;

    // 대련도중이면 대련 취소
    if (pClub->ISCONFTING())
    {
        std::tr1::shared_ptr<GLClubAgent> pConftClub = GetClub(pClub->m_dwconftCLUB);

        emEND_A = (EMCONFRONT_END_FAIL);
        emEND_B = (EMCONFRONT_END_FAIL);

        // 클럽 대련이 종료됨을 알림 to (필드)
        GLMSG::SNETPC_CONFRONTCLB_END2_FLD NetMsgFld(pClub->m_DbNum, pClub->m_dwconftCLUB, emEND_A, emEND_B);;
        m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

        pClub->CLEARCONFT();
        if (pConftClub)
            pConftClub->CLEARCONFT();

        std::string strCUR_MAP;
        //CString strText;
        std::string strText;
        GLCharAG* pCHAR_MASTER = m_pServer->GetCharByDbNum(pClub->MasterCharDbNum());
        if (pCHAR_MASTER)
            strCUR_MAP = m_pServer->GetMapName(pCHAR_MASTER->GetCurrentMap().getBaseMapID());

        strText = sc::string::format(
            ID2SERVERTEXT("EMCONFRONT_END_CDRAWN"),
            strCUR_MAP.c_str(),
            strCLUB_A.c_str(),
            strCLUB_B.c_str());

        GLMSG::SNET_SERVER_GENERALCHAT SendData;
        if (SendData.SETTEXT(strText))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
            m_pServer->SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
    }

    GLCharAG* pCHAR = m_pServer->GetCharByDbNum(pClub->MasterCharDbNum());
    if (pCHAR)
        pCHAR->m_ClubDbNum = CLUB_NULL;
	else
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on FrameMoveDissolution ", pClub->MasterCharDbNum()));
	}
    
    CLUBMEMBERS& ClubMembers = pClub->GetMembers();    
    for (CLUBMEMBERS_ITER pos=ClubMembers.begin(); pos!=ClubMembers.end(); ++pos)
    {
        const GLCLUBMEMBER& cMEMBER = pos->second;
        GLCharAG* pMember = m_pServer->GetCharByDbNum(cMEMBER.m_ChaDbNum);
        if (pMember)                
            pMember->m_ClubDbNum = CLUB_NULL; // 클럽 정보 제거
    }

    // 클럽 삭제 로그 기록
	// 통계 및 로그 수정
    m_pServer->LogCharAction(
        pClub->MasterCharDbNum(),			//	당사자.
        gamelog::ACT_CLUB_DELETE,		//	행위.
        gamelog::ID_CLUB, pClub->m_DbNum,		//	상대방.
        0,							//	exp
        0,							//	bright
        0,							//	life
        0,							//	money
		(pCHAR==NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCHAR->GetCurrentMap()
        );

    // 클럽 삭제 메시지
    GLMSG::SNET_CLUB_DEL_2FLD NetMsgFld;
    NetMsgFld.dwClubID = pClub->m_DbNum;
    m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

    // DB에서 제거.
    m_pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ClubDelete(pClub->m_DbNum,pClub->MasterCharDbNum())));

    // 클럽 정보 제거 예약
    vClubDel.push_back(ClubDbNum);
}

void GLClubManAgent::FrameMoveAutoKick()
{
    // 시간 체크
    DWORD NewAutoMemberKickTimer = GetTickCount();
    DWORD ElspTime = NewAutoMemberKickTimer - m_AutoMemberKickTimer;
    if (ElspTime < 3600000)
        return;
    m_AutoMemberKickTimer = NewAutoMemberKickTimer;
    
    for (CLUB_AGENT_ITER pos=m_mapCLUB.begin(); pos!=m_mapCLUB.end(); ++pos)
    {
        std::tr1::shared_ptr<GLClubAgent> spClub = pos->second;
        if (spClub)
            spClub->AutoKickOut();
    }
}

bool GLClubManAgent::ClubRename( DWORD ClubDbNum, const std::string& strNewName )
{
	CLUB_AGENT_ITER Club = m_mapCLUB.find( ClubDbNum );
	if ( Club == m_mapCLUB.end() )
		return false;

	std::tr1::shared_ptr< GLClubAgent > pClub = Club->second;
	if ( !pClub )
		return false;

	CLUBSNAME_ITER Name = m_mapCLUBNAME.find( std::string( pClub->Name() ) );
	if ( Name == m_mapCLUBNAME.end() )
		return false;

	pClub->SetName( strNewName.c_str() );

	m_mapCLUBNAME.erase( Name );
	m_mapCLUBNAME.insert( CLUBSNAME_VALUE( strNewName, pClub->m_DbNum ) );

	if ( pClub->IsAlliance() )
	{
		CLUB_AGENT_ITER ChiefClub = m_mapCLUB.find( pClub->m_dwAlliance );
		if ( ChiefClub == m_mapCLUB.end() )
			return false;

		std::tr1::shared_ptr< GLClubAgent > pChiefClub = ChiefClub->second;
		if ( !pChiefClub )
			return false;

		CLUB_ALLIANCE_ITER iter = pChiefClub->m_setAlliance.find( ClubDbNum );
		if ( iter == pChiefClub->m_setAlliance.end() )
			return true;

		GLCLUBALLIANCE& Alliance = iter->second;
		StringCchCopy( Alliance.m_szName, CHAR_SZNAME, strNewName.c_str() );
	}

	return true;
}
