#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/ClubMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Server/ServerCfg.h"
#include "../Util/GLItemLimit.h"
#include "./WorldBattleClubCountry.h"
#include "./GLClubField.h"
#include "./GLClubManField.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClubManField::GLClubManField(GLGaeaServer* pServer)
: m_pServer(pServer)
{
}

GLClubManField::~GLClubManField()
{
    GLClubField* pClub = NULL;

    //CLUB_FIELD_ITER iter = m_mapCLUB.begin();
    //for (; iter != m_mapCLUB.end(); ++iter)
    //{
    //	pClub = iter->second;
    //	SAFE_DELETE(pClub);
    //}
}

std::tr1::shared_ptr<GLClubField> GLClubManField::GetClub(DWORD dwID)
{
    CLUB_FIELD_ITER pos = m_mapCLUB.find(dwID);
    if (pos != m_mapCLUB.end())
    {
        return pos->second;
    }
    else
    {
        std::tr1::shared_ptr<GLClubField> NullPtr((GLClubField*) 0);
        return NullPtr;
    }

}

void GLClubManField::AddClub(std::tr1::shared_ptr<GLClubField> pClub)
{
    if (!pClub || !pClub->IsValid())
        return;

    m_mapCLUB.insert(CLUB_FIELD_VALUE(pClub->m_DbNum, pClub));
    m_mapCLUBNAME.insert(CLUBSNAME_VALUE(std::string(pClub->Name()), pClub->m_DbNum));    
}

bool GLClubManField::DelClub(DWORD dwID)
{
    CLUB_FIELD_ITER pos = m_mapCLUB.find(dwID);
    if (pos == m_mapCLUB.end())
        return false;

    std::tr1::shared_ptr<GLClubField> pClub = pos->second;

    // ����� ����
    const CLUBMEMBERS& ClubMembers = pClub->GetMembers();
    CLUBMEMBERS_CITER PosS = ClubMembers.begin();
    CLUBMEMBERS_CITER PosE = ClubMembers.end();
    GLChar* pMember = NULL;
    GLMSG::SNET_CLUB_DEL_2CLT NetMsgClt;
    GLMSG::SNET_CLUB_DEL_BRD NetMsgBrd;
    for ( ; PosS!=PosE; ++PosS)
    {
        const GLCLUBMEMBER& cMEMBER = PosS->second;
        pMember = m_pServer->GetCharByDbNum(cMEMBER.m_ChaDbNum);
        if (pMember)
        {
            // Ŭ�� ���� ����
            pMember->m_ClubDbNum = CLUB_NULL;
            // Ŭ���̾�Ʈ�� Ŭ�� ���� ����
            m_pServer->SENDTOCLIENT(pMember->ClientSlot(), &NetMsgClt);
            // �ֺ� ������� �˸�
            NetMsgBrd.dwGaeaID = pMember->GetGaeaID();
            pMember->SendMsgViewAround(&NetMsgBrd);
        }
    }

    // Ŭ���̸� ����
    CLUBSNAME_ITER pos_name = m_mapCLUBNAME.find(std::string(pClub->Name()));
    if (pos_name != m_mapCLUBNAME.end())
        m_mapCLUBNAME.erase(pos_name);

    // Ŭ�� ����
    //SAFE_DELETE(spClubField);
    m_mapCLUB.erase(pos);
    return true;
}

bool GLClubManField::LoadFromDB(db::IDbManager* pDBMan, db::IDbManager* pAdoMan)
{
    if (!pDBMan)
        return false;

    std::vector<std::tr1::shared_ptr<GLClub> > vecClubInfo;
    int nRET(0);

    nRET = pDBMan->GetClubInfo(vecClubInfo);
    if (nRET != sc::db::DB_OK)
        return false;

    for (DWORD i=0; i<vecClubInfo.size(); ++i)
    {
        std::tr1::shared_ptr<GLClub> spGLClub = vecClubInfo[i];

        std::tr1::shared_ptr<GLClubField> spClubField = std::tr1::shared_ptr<GLClubField> (new GLClubField(m_pServer));
        *spClubField.get() = *spGLClub.get();

        DWORD ClubDbNum = spClubField->DbNum();
        
        bool bOldClub = false;
        LoadClubAuthFromDB(pAdoMan, ClubDbNum, spClubField, bOldClub);

        std::vector<GLCLUBMEMBER> vecMEMBER;
        nRET = pDBMan->GetClubMember(spGLClub->m_DbNum, vecMEMBER);
        if (nRET != sc::db::DB_OK)
            continue;
        
        for (DWORD m=0; m<vecMEMBER.size(); ++m)
        {
            const GLCLUBMEMBER& sMEMBER = vecMEMBER[m];
            if (bOldClub)
            {
                // ���� Ŭ���̶�� ������ ��� 0���� ����ξ�� �Ѵ�.
                // ���� ������ Agent server ���� �ϰԵȴ�. GLClubManAgent::LoadClubAuthFromDB ����
                spClubField->AddMember(
                    sMEMBER.m_ChaDbNum,
                    sMEMBER.m_ChaName.c_str(),
                    0,
                    sMEMBER.m_ChaLevel, 
                    sMEMBER.m_LastPlayTime,
                    false,
                    static_cast<EMCHARCLASS> (sMEMBER.m_ChaClass));
            }
            else
            {
                spClubField->AddMember(
                    sMEMBER.m_ChaDbNum,
                    sMEMBER.m_ChaName.c_str(),
                    sMEMBER.m_Grade, 
                    sMEMBER.m_ChaLevel,
                    sMEMBER.m_LastPlayTime,
                    false,
                    static_cast<EMCHARCLASS> (sMEMBER.m_ChaClass));
            }

            // Ŭ�� CDM ����
            if (spClubField->IsMemberFlgCDM(sMEMBER.m_ChaDbNum))
                spClubField->m_listCDMMembers.push_back(sMEMBER.m_ChaDbNum);
        }

        //	���� �����ڰ� ������� ���� �Ǿ��� ��� Ȥ�� �ɸ��Ͱ� �����Ǿ��� ���. �������� ȸ��.
        if (spClubField->m_dwCDCertifior != 0)
        {
            if (!spClubField->IsMember(spClubField->m_dwCDCertifior))
                spClubField->m_dwCDCertifior = 0;
        }

        std::vector<GLCLUBBATTLE> vecBATTLE;
        nRET = pDBMan->GetClubBattleInfo(spGLClub->m_DbNum, vecBATTLE);
        if (nRET != sc::db::DB_OK)
            continue;

        for (DWORD m=0; m<vecBATTLE.size(); ++m)
        {
            const GLCLUBBATTLE &sBATTLE = vecBATTLE[m];
            spClubField->ADDBATTLECLUB(sBATTLE);
        }

        // �ʵ� �����̸� Ŭ�� ������ 0���� �ʱ�ȭ ��Ų��.
        // AgentServer������ Ŭ�� ������ ������ �ֱ� �����̴�.
        spClubField->ResetProfitMoeny();

        // Ŭ�� ���.
        AddClub(spClubField);
    }

    // Ŭ�� ��Ʋ ���� ������ �ʱ�ȭ �Ѵ�.
    InitClubBattle();

    //	Note : ���� ���� ��������.
    std::vector< std::pair<DWORD,DWORD> > vecAlliance;
    pDBMan->GetClubAlliance ( vecAlliance );

    for ( size_t i=0; i<vecAlliance.size(); ++i )
    {
        const std::pair<DWORD,DWORD> &pairAlliance = vecAlliance[i];

        std::tr1::shared_ptr<GLClubField> pCLUB_A = GetClub ( pairAlliance.first );
        if (!pCLUB_A)
            continue;

        std::tr1::shared_ptr<GLClubField> pCLUB_B = GetClub ( pairAlliance.second );
        if (!pCLUB_B)
            continue;

        pCLUB_A->m_dwAlliance = pairAlliance.first;
        pCLUB_B->m_dwAlliance = pairAlliance.first;

        pCLUB_B->m_dwAllianceBattleWin = pCLUB_A->m_dwAllianceBattleWin;
        pCLUB_B->m_dwAllianceBattleLose = pCLUB_A->m_dwAllianceBattleLose;
        pCLUB_B->m_dwAllianceBattleDraw = pCLUB_A->m_dwAllianceBattleDraw;

        pCLUB_A->AddAlliance(pCLUB_A->m_dwAlliance, pCLUB_A->Name(), pCLUB_A->GetMasterName(), pCLUB_A->GetMarkVersion());
        pCLUB_A->AddAlliance(pCLUB_B->m_DbNum,      pCLUB_B->Name(), pCLUB_B->GetMasterName(), pCLUB_B->GetMarkVersion());
    }

    // ���͹�Ʋ�� �����Ѵ�.
    InitAllianceBattle ();
    return true;
}

void GLClubManField::LoadClubAuthFromDB(
    db::IDbManager* pAdoMan,
    DWORD ClubDbNum,
    std::tr1::shared_ptr<GLClubField> pClub,
    bool &OldClub)
{
    // ���� ���� ��������
    std::vector<club::AUTH_GRADE_DB> vAuth;
    vAuth.reserve(club::AUTHORITY_GRADE);
    pAdoMan->GetClubAuth(ClubDbNum, vAuth);
    if (vAuth.size() == 0)
    {
        // ������ ������ ���� ���ٸ�. ���� Ŭ���̴�.
        // �������Ͽ� ������ �⺻ ������ �����Ѵ�.
        size_t GradeSize = GLogicData::GetInstance().GetClubGradeSize();
        for (size_t i=0; i<GradeSize; ++i)
            pClub->AddAuth(i, GLogicData::GetInstance().GetClubBaseGrade(i));
        OldClub = true;
    }
    else
    {
        for (size_t i=0; i<vAuth.size(); ++i)
        {
            const club::AUTH_GRADE_DB& AuthGrade = vAuth[i];
            pClub->AddAuth(i, AuthGrade);
        }
        OldClub = false;
    }
}

HRESULT GLClubManField::FrameMoveField( float fTime, float fElapsedTime )
{
    // Ŭ�� ���� ����.
    //GLMSG::SNET_CLUB_MEMBER_POS NetMsgPos;

    //	Ŭ�� ���� ����.
    CLUB_FIELD_ITER pos = m_mapCLUB.begin();
    CLUB_FIELD_ITER end = m_mapCLUB.end();
    for ( ; pos!=end; ++pos )
    {
        std::tr1::shared_ptr<GLClubField> pClub = pos->second;

        //	���� ���� Ÿ�̸�.
        pClub->AddProfitTimer(fElapsedTime);

        //	Note : Ŭ���� ������ ������Ʈ ������ �ݿ�.
        if (pClub->GetProfitTimer() > 1800) // || pClub->GetProfitTimer() < 0.01f)
        {
            pClub->ResetProfitTimer();

            if (pClub->GetProfitMoney() > 0)
            {
                GLMSG::NET_CLUB_INCOME_MONEY_FA NetMsgAgt;
                NetMsgAgt.m_ClubDbNum = pClub->DbNum();
                NetMsgAgt.m_ProfitMoney = pClub->GetProfitMoney();
                m_pServer->SENDTOAGENT(&NetMsgAgt);

//                 sc::writeLogInfo(
//                     sc::string::format(
//                         "ClubIncomeMoney FrameMove ClubID %1% IncomeMoney %2% Time %3%",
//                         pClub->DbNum(),
//                         pClub->GetProfitMoney(),
//                         pClub->GetProfitTimer()));

                pClub->ResetProfitMoeny();
            }
        }
/*
        pClub->AddPosTimer(fElapsedTime);
        if (pClub->GetPosTimer() > 10.0f)
        {
            //	0 ~ 5.0f ������ ������ �ʱ�ȭ, Ŭ������ ���� �ӵ��� �����Ѵ�.
            pClub->SetPosTimer(sc::Random::getInstance().getFloatPercent() / 33.0f);
            NetMsgPos.RESET();

            CLUBMEMBERS& ClubMembers = pClub->GetMembers();
            CLUBMEMBERS_ITER mem_pos = ClubMembers.begin();
            CLUBMEMBERS_ITER mem_end = ClubMembers.end();
            for ( ; mem_pos!=mem_end; ++mem_pos)
            {
                GLChar* pCHAR = m_pServer->GetCharByDbNum(mem_pos->first);
                if (pCHAR)
                {
                    bool bOK = NetMsgPos.ADD(pCHAR->m_CharDbNum, D3DXVECTOR2(pCHAR->m_vPos.x,pCHAR->m_vPos.z));
                    if (!bOK)
                    {
                        SendClubClient(pClub->m_DbNum, &NetMsgPos, 0);
                        NetMsgPos.RESET();
                        NetMsgPos.ADD(pCHAR->m_CharDbNum, D3DXVECTOR2(pCHAR->m_vPos.x,pCHAR->m_vPos.z));
                    }
                }
            }

            if (NetMsgPos.dwNumber > 0)
                SendClubClient(pClub->m_DbNum, &NetMsgPos, 0);
        }
*/
        if ( GLCONST_CHAR::bCLUB_BATTLE )
        {

            pClub->m_fTIMER_BATTLE += fElapsedTime;
            if ( pClub->m_fTIMER_BATTLE > 300.0f )
            {
                //	0 ~ 10.0f ������ ������ �ʱ�ȭ, Ŭ������ ���� �ӵ��� �����Ѵ�.
                pClub->m_fTIMER_BATTLE = (sc::Random::getInstance().GetFloatPercent() +0.01) / 20.0f;

                // Note : Ŭ�� ��Ʋ ����.
                if ( pClub->GetBattleNum() > 0 )
                {
                    CLUB_BATTLE_ITER pos_B = pClub->m_mapBattle.begin();
                    CLUB_BATTLE_ITER end_B = pClub->m_mapBattle.end();
                    for ( ; pos_B!=end_B; ++pos_B )
                    {
                        GLCLUBBATTLE &sClubBattle = (*pos_B).second;

                        // Kill�� ��ȭ	
                        if ( sClubBattle.m_bKillUpdate )
                        {
                            GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE_AGT NetMsgAgt;
                            NetMsgAgt.dwClubID = pClub->m_DbNum;
                            NetMsgAgt.dwBattleClubID = sClubBattle.m_dwCLUBID;
                            NetMsgAgt.wKillPoint = sClubBattle.m_wKillPointTemp;
                            NetMsgAgt.wDeathPoint = sClubBattle.m_wDeathPointTemp;

                            sClubBattle.m_wKillPointDB += sClubBattle.m_wKillPointTemp;
                            sClubBattle.m_wDeathPointDB += sClubBattle.m_wDeathPointTemp;

                            sClubBattle.m_wKillPointTemp = 0;
                            sClubBattle.m_wDeathPointTemp = 0;
                            sClubBattle.m_bKillUpdate = false;

                            m_pServer->SENDTOAGENT( &NetMsgAgt );
                        }

                    }
                }
            }
        }
    }

    return S_OK;
}

// ��Ʋ ������ �̸��� �Է��Ѵ�.
bool GLClubManField::InitClubBattle()
{
    // �������

    CLUB_FIELD_ITER pos = m_mapCLUB.begin();
    CLUB_FIELD_ITER end = m_mapCLUB.end();
    for ( ; pos!=end; ++pos )
    {
        std::tr1::shared_ptr<GLClubField> pClub = pos->second;

        CLUB_BATTLE_ITER pos_B = pClub->m_mapBattle.begin();
        CLUB_BATTLE_ITER end_B = pClub->m_mapBattle.end();
        CLUB_BATTLE_ITER del_B;
        for ( ; pos_B!=end_B; pos_B )
        {
            GLCLUBBATTLE &sClubBattle = (*pos_B).second;
            del_B = pos_B++;

            std::tr1::shared_ptr<GLClubField> pClub = GetClub( sClubBattle.m_dwCLUBID );
            if ( pClub )
                sClubBattle.SetName(pClub->Name());

            __time64_t curTime = CTime::GetCurrentTime().GetTime();

            // ��Ʋ ����				
            if ( sClubBattle.m_tEndTime < curTime )
            {
                EndClubBattleField( pClub->m_DbNum, sClubBattle.m_dwCLUBID );

                pClub->DELBATTLECLUB( (*del_B).first );	
                std::tr1::shared_ptr<GLClubField> pCLUB = GetClub( sClubBattle.m_dwCLUBID );
                if ( pCLUB )
                    pCLUB->DELBATTLECLUB( pClub->m_DbNum );	
            }
        }
    }

    return true;
}

void GLClubManField::InitAllianceBattle ()
{
    CLUB_FIELD_ITER pos = m_mapCLUB.begin();
    CLUB_FIELD_ITER end = m_mapCLUB.end();

    for ( ; pos!=end; ++pos )
    {
        std::tr1::shared_ptr<GLClubField> pClub = pos->second;

        if (!pClub->IsChief())
            continue;

        CLUB_BATTLE_ITER pos_B = pClub->m_mapBattle.begin();
        CLUB_BATTLE_ITER end_B = pClub->m_mapBattle.end();
        for ( ; pos_B!=end_B; ++pos_B)
        {
            GLCLUBBATTLE &sClubBattle = (*pos_B).second;

            if (!sClubBattle.m_bAlliance)
                continue;

            CLUB_ALLIANCE_ITER pos_A = pClub->m_setAlliance.begin();
            CLUB_ALLIANCE_ITER end_A = pClub->m_setAlliance.end();
            for ( ; pos_A!=end_A; ++pos_A )
            {
                const GLCLUBALLIANCE &sALLIANCE = pos_A->second;
                std::tr1::shared_ptr<GLClubField> pCLUB = GetClub ( sALLIANCE.m_dwID );

                if ( !pCLUB )
                    continue;
                if ( pClub->m_DbNum == pCLUB->m_DbNum )
                    continue;

                pCLUB->ADDBATTLECLUB( sClubBattle );
            }
        }
    }
}

void GLClubManField::SaveClubBattle()
{
    CLUB_FIELD_ITER pos = m_mapCLUB.begin();
    CLUB_FIELD_ITER end = m_mapCLUB.end();

    for (; pos!=end; ++pos)
    {
        std::tr1::shared_ptr<GLClubField> pClub = pos->second;
        if (pClub->GetBattleNum() > 0)
        {
            CLUB_BATTLE_ITER pos = pClub->m_mapBattle.begin();
            CLUB_BATTLE_ITER end = pClub->m_mapBattle.end();
            for (; pos!=end; ++pos)
            {
                GLCLUBBATTLE& sClubBattle = (*pos).second;

                //	������Ŭ���� ���
                if (sClubBattle.m_bAlliance && pClub->m_DbNum != pClub->m_dwAlliance)
                    continue;

                // db�� ���� ( A����, B���� ���� )
                m_pServer->AddGameDbJob(
                    db::DbActionPtr(
                    new db::ClubBattleSave(
                    pClub->m_DbNum, 
                    sClubBattle.m_dwCLUBID, 
                    sClubBattle.m_wKillPointDB + sClubBattle.m_wKillPointTemp, 
                    sClubBattle.m_wDeathPointDB + sClubBattle.m_wDeathPointTemp)));
            }
        }
    }
}


// ��Ʋ ������ ����
HRESULT GLClubManField::EndClubBattleField(DWORD dwClubID_P, DWORD dwClubID_S)
{
    std::tr1::shared_ptr<GLClubField> pClub_P = GetClub( dwClubID_P );
    if (!pClub_P)
        return E_FAIL;

    std::tr1::shared_ptr<GLClubField> pClub_S = GetClub( dwClubID_S );
    if (!pClub_S)
        return E_FAIL;

    GLCLUBBATTLE* pClubBattle = pClub_P->GetClubBattle( dwClubID_S );
    if ( !pClubBattle )	return E_FAIL;

    if ( pClubBattle->m_bAlliance )
    {
        if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
        {
            pClub_P->m_dwAllianceBattleDraw++;
            pClub_S->m_dwAllianceBattleDraw++;
        }
        else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
        {
            pClub_P->m_dwAllianceBattleWin++;
            pClub_S->m_dwAllianceBattleLose++;
        }
        else
        {
            pClub_P->m_dwAllianceBattleLose++;
            pClub_S->m_dwAllianceBattleWin++;
        }	
    }
    else
    {
        if ( pClubBattle->m_wKillPoint == pClubBattle->m_wDeathPoint )
        {
            pClub_P->m_dwBattleDraw++;
            pClub_S->m_dwBattleDraw++;
        }
        else if ( pClubBattle->m_wKillPoint > pClubBattle->m_wDeathPoint ) 
        {
            pClub_P->m_dwBattleWin++;
            pClub_S->m_dwBattleLose++;
        }
        else
        {
            pClub_P->m_dwBattleLose++;
            pClub_S->m_dwBattleWin++;
        }	
    }

    return S_OK;
}

void GLClubManField::ChaLevelUp(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel)
{
    std::tr1::shared_ptr<GLClubField> pClub = GetClub(ClubDbNum);
    if (pClub)
    {
        if (pClub->MasterCharDbNum() == ChaDbNum)
            pClub->SetMasterCharLevel(ChaLevel);

        pClub->SetMemberState(ChaDbNum, ChaLevel);
    }
}

void GLClubManField::SendClubClient(DWORD ClubDbNum, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, DWORD ExceptMember, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        SendClubClient(ClubDbNum, &MsgPack, ExceptMember);
}

void GLClubManField::SendClubClient(DWORD ClubDbNum, NET_MSG_GENERIC* pMsg, DWORD ExceptMember)
{
    std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(ClubDbNum);
    if (pCLUB)
    {
        CLUBMEMBERS& Members = pCLUB->GetMembers();
        CLUBMEMBERS_ITER pos = Members.begin();
        CLUBMEMBERS_ITER end = Members.end();    	
        GLChar* pChar = NULL;
        for ( ; pos!=end; ++pos)
        {
            DWORD ChaDbNum = pos->first;
            if (ChaDbNum != ExceptMember)
            {
                pChar = m_pServer->GetCharByDbNum(ChaDbNum);
                if (pChar)
                    m_pServer->SENDTOCLIENT(pChar->ClientSlot(), pMsg);
            }
        }
    }
}

DWORD GLClubManField::FindClub( const std::string& strName )
{
	CLUBSNAME_ITER pos = m_mapCLUBNAME.find( strName );
	if ( pos != m_mapCLUBNAME.end() )
		return pos->second;
	else
		return CLUB_NULL;        
}

bool GLClubManField::ClubRename( DWORD ClubDbNum, const std::string& strNewName )
{
	CLUB_FIELD_ITER Club = m_mapCLUB.find( ClubDbNum );
	if ( Club == m_mapCLUB.end() )
		return false;

	std::tr1::shared_ptr< GLClubField > pClub = Club->second;
	if ( !pClub )
	{
		return false;
	}

	CLUBSNAME_ITER Name = m_mapCLUBNAME.find( std::string( pClub->Name() ) );
	if ( Name == m_mapCLUBNAME.end() )
		return false;
	
	pClub->SetName( strNewName.c_str() );

	m_mapCLUBNAME.erase( Name );
	m_mapCLUBNAME.insert( CLUBSNAME_VALUE( strNewName, pClub->m_DbNum ) );

	// ����鿡�� Ŭ�� �̸� ���� �˸�, �ֺ����� �˸�
	const CLUBMEMBERS& ClubMembers = pClub->GetMembers();
	CLUBMEMBERS_CITER PosS = ClubMembers.begin();
	CLUBMEMBERS_CITER PosE = ClubMembers.end();
	GLChar* pMember = NULL;
	GLMSG::SNET_CLUB_RENAME_2CLT NetMsgClt;
	GLMSG::SNET_CLUB_RENAME_BRD NetMsgBrd;
	NetMsgClt.SetClubName( strNewName.c_str() );
	NetMsgBrd.SetClubName( strNewName.c_str() );
	for ( ; PosS != PosE; ++PosS )
	{
		const GLCLUBMEMBER& cMEMBER = PosS->second;
		pMember = m_pServer->GetCharByDbNum( cMEMBER.m_ChaDbNum );
		if ( pMember )
		{
			m_pServer->SENDTOCLIENT( pMember->ClientSlot(), &NetMsgClt );
			NetMsgBrd.dwGaeaID = pMember->GetGaeaID();
			pMember->SendMsgViewAround( &NetMsgBrd );
		}
	}

	// ���� Ŭ�� ó��
	if ( pClub->IsAlliance() )
	{
		CLUB_FIELD_ITER ChiefClub = m_mapCLUB.find( pClub->m_dwAlliance );
		if ( ChiefClub == m_mapCLUB.end() )
			return false;

		std::tr1::shared_ptr< GLClubField > pChiefClub = ChiefClub->second;
		if ( !pChiefClub )
			return false;

		CLUB_ALLIANCE_ITER pos = pChiefClub->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pChiefClub->m_setAlliance.end();
		GLMSG::SNET_CLUB_RENAME_2ALLIANCE NetMsgAlliance;
		NetMsgAlliance.ClubDbNum = ClubDbNum;
		NetMsgAlliance.SetClubName( strNewName.c_str() );
		for ( ; pos != end; ++pos )
		{
			GLCLUBALLIANCE& sALLIANCE = pos->second;

			if ( sALLIANCE.m_dwID == ClubDbNum )
				StringCchCopy( sALLIANCE.m_szName, CHAR_SZNAME, strNewName.c_str() );

			m_pServer->SENDTOCLUBCLIENT( sALLIANCE.m_dwID, &NetMsgAlliance );
		}
	}

	return true;
}
