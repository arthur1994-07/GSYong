#include "pch.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubField.h"
#include "../Party/GLPartyField.h"
#include "../Guidance/GuidanceField.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../TriggerSystem/ServerDieMessage.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "./GLChar.h"


void GLChar::DoFalling()
{
    // �δ� ������/ HP�� 0�϶� ������ ���������� ���ؼ��� lua���� ��� �� �� �ֵ��� ��;	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )  // �δ��� �ƴ� ��� NULL;
    {
        DWORD dwID(m_sAssault.GaeaId);

		// Mob�� ���� �״�� ������ �ȴ�;
		// PC�� ��� DBNum�� ���� ������ ���� �־�� �Ѵ�;
		if ( m_sAssault.emCrow == CROW_PC )
		{
			GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
			if ( pChar )
				dwID = pChar->CharDbNum();
		}
		else if ( m_sAssault.emCrow == CROW_SUMMON )
		{
			GLSummonField* pSummon = pLand->GetSummon( m_sAssault.GaeaId );
			if ( pSummon )
			{
				ServerActor* pOwner = pSummon->GetOwner();

				// ����� ��ȯ���� ������ SUMMON�� ����;
				// �������� �ʴ´�;
				if ( pOwner && pOwner->GetCrow() == CROW_PC )
				{
					GLChar* pChar = static_cast< GLChar* >( pOwner );
					if ( pChar )
						dwID = pChar->CharDbNum();
				}
			}
		}

        // ���� Event Died�� ���ϰ��� false�� ��� ���̴� ó���� ���� �ʴ´ٴ� ��;
        if ( _pInstance->EventDie(CROW_PC, CharDbNum(), m_sAssault.emCrow, dwID) == false )
		{
			return;
		}
    }

    // PVE�δ��� ���� Ʈ����;
    TriggerSystem::SServerDieMessage msg;
    msg.dwCharDbNum = CharDbNum();
    SendTriggerMessage(&msg);  

    // ���� ������ ��� �г�Ƽ�� ���ٸ�;
    if ( m_bSetDeathPenalty == false )
    {
        // �Ϲ����� ����� ���� �г�Ƽ ó��( ����ġ, ������, �ݾ� ���� �� );
        DeathPenaltyDefault();
    }
    m_bSetDeathPenalty = false;

	// ��ũ�� ���������� ����;
	if ( IsActState(EM_ACT_MACROMODE) )
	{
		MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SendToClient ( &msg );
	}

    // ���� ������ ��Ȱ������ ���ٸ�;
    if ( m_sResurrectAbleState.isState(EM_RESURRECTION_TYPE_NOT_SET) )
    {
        // �⺻���� ��Ȱ Ÿ���� ���Ѵ�.;
        SetDefaultResurrection();
    }

    //���� ���� ó��. �������� ���� �߻�. ( ����ġ + ������ + �ݾ� );
    if ( m_sCONFTING.IsCONFRONTING() )
        DoFalling_Proc_Confront();
    else
        DoFalling_Proc();

	// ���� ��Ƽ�� ���� Faction�� ��� ���� ���¶�� ȣ���Ѵ�;
	Faction::ManagerField* pFactionManager = pLand->GetFactionManager();
	if ( NULL == pFactionManager )
		return;

	const Faction::FactionID* pFactionID = pFactionManager->getFactionID( GetActorID() );
	if ( NULL == pFactionID )
		return;

	// ���� ������ ����� ���´�;
	const Faction::FactionMemberVector* vecFactionMember =
		pFactionManager->getFactionMembers( *pFactionID );
	if ( NULL == vecFactionMember )
		return;

	for ( size_t i=0; i<vecFactionMember->size(); ++i )
	{
		Faction::GLActorID sActorID = vecFactionMember->at( i );
		GLChar* pMember = pLand->GetCharByDbNum( sActorID.actorID_Num );
		if ( NULL == pMember )
			continue;
		
		if ( pMember->IsAlive() )
			return;
	}

	// ��� ���� �����̸� �ݹ��� ȣ���Ѵ�;
	if ( _pInstance )
		_pInstance->EventAllDie( pFactionID->factionType, pFactionID->factionID_Num );
}

void GLChar::DoFalling_Proc_Confront()
{
    switch ( m_sCONFTING.emTYPE )
    {
    case EMCONFT_ONE:
        {
            STARGETID sCONFTID(CROW_PC,m_sCONFTING.dwTAR_ID);

            // ��õ��� ����ڰ� �ƴ� ĳ���ͳ� ������ ���� ���.
            //
            if( m_sAssault != sCONFTID )
            {
                // ���� ����� ID ���. Reset �ÿ� ��ȿȭ�Ǳ� ������ �̸� ���.
                //
                const DWORD dwTAR_ID = m_sCONFTING.dwTAR_ID;

                // (�ڽ�) ��� ����.
                //
                ResetConfront(EMCONFRONT_END_FAIL);

                // ��� ���� ã��.
                //
                GLChar* pCONFT = GetChar(dwTAR_ID);
                if ( pCONFT )
                {
                    // ��� ������ ������ �� ����ġ �α� ���Ž�Ŵ.
                    //
                    DAMAGELOG_ITER iter;
                    iter = m_cDamageLog.find ( pCONFT->m_dwUserID );
                    if ( iter!=m_cDamageLog.end() )		m_cDamageLog.erase ( iter );

                    // (����) ��� ����.
                    //
                    pCONFT->ResetConfront ( EMCONFRONT_END_FAIL );
                }

                DoFalling_Proc();
                return;
            }

            // ���� ����� ID ���. Reset �ÿ� ��ȿȭ�Ǳ� ������ �̸� ���.
            //
            const DWORD dwTAR_ID = m_sCONFTING.dwTAR_ID;
            const SCONFT_OPTION sOption = m_sCONFTING.sOption;

            // (�ڽ�) ��� ����.
            //
            ResetConfront(EMCONFRONT_END_LOSS);

            // ��� ���� ã��.
            //
            GLChar* pCONFT = GetChar(dwTAR_ID);
            if ( pCONFT==NULL )	
                return;

            // (����) ��� ����.
            //
            EMCONFRONT_END emCONFT_END = (pCONFT->GETHP()>0) ? EMCONFRONT_END_WIN : EMCONFRONT_END_LOSS;
            pCONFT->ResetConfront ( emCONFT_END );

            // �ڽ� ��Ȱ���� ��ȭ.
            //
            if ( GLCONST_CHAR::nCONFRONT_LOSS_LP!=0 && GLCONST_CHAR::nCONFRONT_WIN_LP!=0 )
            {
                float fLIFE_P(0);

                fLIFE_P = GLCONST_CHAR::nCONFRONT_LOSS_LP - (GETLEVEL()-pCONFT->GETLEVEL())/2.0f
                    + (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
                LIMIT ( fLIFE_P, 0.0f, -20.0f );

                ReceiveLivingPoint ( (int)fLIFE_P );

                // ���� ��Ȱ���� ��ȭ.
                //
                fLIFE_P = GLCONST_CHAR::nCONFRONT_WIN_LP + (GETLEVEL()-pCONFT->GETLEVEL())/2.0f
                    + (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
                LIMIT ( fLIFE_P, 20.0f, 0.0f );

                pCONFT->ReceiveLivingPoint ( (int)fLIFE_P );
            }
        }
        break;

    case EMCONFT_PARTY:
        {
            bool breward(false);
            if ( m_sAssault.emCrow==CROW_PC )
            {
                GLPartyField* const pConftParty = GetParty(m_sCONFTING.dwTAR_ID);
                if ( pConftParty )
                    breward = pConftParty->isConfront(m_sAssault.GaeaId);
            }

            if ( !breward )
            {
                m_cDamageLog.clear();

                // (�ڽ�) ��� ����.
                //
                ResetConfront(EMCONFRONT_END_NOTWIN);

                DoFalling_Proc();
                return;
            }

            // (�ڽ�) ��� ����.
            //
            ResetConfront(EMCONFRONT_END_NOTWIN);
        }
        break;

    case EMCONFT_GUILD:
        {
            bool breward(false);
            if ( m_sAssault.emCrow==CROW_PC )
            {					
                GLChar* pCHAR = GetChar(m_sAssault.GaeaId);
                std::tr1::shared_ptr<GLClubField> pConftClub = GetClub(m_sCONFTING.dwTAR_ID);
                if ( pConftClub && pCHAR )
                {
                    breward = pConftClub->ISCONFT_MEMBER ( pCHAR->m_CharDbNum );
                }
            }

            if ( !breward )
            {
                m_cDamageLog.clear();

                // (�ڽ�) ��� ����.
                //
                ResetConfront(EMCONFRONT_END_NOTWIN);

                DoFalling_Proc();
                return;
            }

            // (�ڽ�) ��� ����.
            //
            ResetConfront(EMCONFRONT_END_NOTWIN);
        }
        break;
    }
}

void GLChar::DoFalling_Proc()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // �÷��̾ ���� �׾��� ���� ���� ó��;
    if (m_sAssault.emCrow == CROW_PC &&
        m_sAssault.GaeaId != GAEAID_NULL) // ���� gaea id�� null��. hp�Ҹ�ų(�а�)���� ���� ��������..;
    {
        DoFalling_Proc_By_PVP();
    }

    // ������ ���õ� �α� ��� ó��;
    DoFalling_Proc_Log();

    // ���� ������ ���� ó��;
    if (pLand->m_bClubDeathMatchMap)
        DieProc_CDM();
    else
        DieProc();  // ���� ó��;

    //---------------------- ������ ������ ��Ÿ ó�� -------------------------;

    // ���� ������ ����.;
    RemoveAllPlayHostiles();

    // ����Ʈ ���� ����� ������ ���� �ɼ� �˻��� ó��.;
    DoQuestCheckLimitDie ();

    // ����� ������ ����;
    if( m_sAssault.emCrow != CROW_PC )  // ������� �׾��ٸ� PVP�� ó��;
        DurabilityDropDeath( FALSE );
    else
        DurabilityDropDeath( TRUE );

    // ���� ���濡�� CP �ο� �� �˸�;
    GLChar* pKillChar = GetChar(m_sAssault.GaeaId);		
    if ( pKillChar )
    {
        // pKillChar�� ���� ���� ���¸� ���� �����ڷ� �� ����;
        STARGETID dwActorID = STARGETID(CROW_PC, m_dwGaeaID);
        pKillChar->ReceiveKillCP(dwActorID);
		pKillChar->NotifyKillSomeone(m_dwGaeaID);
    }

    // �÷��̾� ����� Q Box ȿ���� ����;
    if (m_sQITEMFACT.IsACTIVE())
    {
        m_sQITEMFACT.RESET();

        GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
        NetMsgBrd.dwGaeaID = m_dwGaeaID;

        // ����Ǿ��� ��� �ڽſ��� �˸�.;
        SendToClient(&NetMsgBrd);

        // ����Ǿ��� ��� �ֺ� ������� �˸�.;
        SendMsgViewAround(&NetMsgBrd);
    }

    // ��Ʋ�ξ� �̺�Ʈ�϶� �������� ������� ��ȯ�Ѵ�.;
    if ( GLCONST_CHAR::bBATTLEROYAL )
    {
        SetActState(EM_REQ_VISIBLEOFF);

        // (������Ʈ����) �޼��� �߻�.;
        GLMSG::SNETPC_ACTSTATE NetMsgFld;
        NetMsgFld.dwChaNum = CharDbNum();
        NetMsgFld.dwSvrGroupNum = SvrGroupNum();
        NetMsgFld.dwActState = GetActState();
        SendToAgent(&NetMsgFld);
    }
}

void GLChar::DoFalling_Proc_By_PVP()  // �÷��̾ ���� �׾��� ���� ���� ó��;
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on DoFalling_Proc_By_PVP", m_CharDbNum ) );
	}

    DWORD dwKILL_ID(0);
    //
    // �ڽſ��� ���� �ֱٿ� ������ ������ (��, ����Ÿ�� ���� ĳ����)
    //
    GLChar* const pKillChar = GetChar(m_sAssault.GaeaId);
    if ( pKillChar == NULL )
		return;
    
	dwKILL_ID = pKillChar->m_CharDbNum;
	int nBRIGHT(0), nLIFE(0);

	bool bClubBattle = false;
	bool bAllianceBattle = false;
	bool bClubDeathMatch = false;
	bool bGuidBattleMap = pLand->m_bGuidBattleMap;

	std::tr1::shared_ptr<GLClubField> pMyClub = GetMyClub();
	std::tr1::shared_ptr<GLClubField> pTarClub = GetClub(pKillChar->m_ClubDbNum);

	if ( pMyClub && pTarClub )
	{
		bClubBattle =  pMyClub->IsBattle( pKillChar->m_ClubDbNum );
		bAllianceBattle = pMyClub->IsBattleAlliance ( pTarClub->m_dwAlliance );
	}

	if (pLand->m_bClubDeathMatchMap)
		bClubDeathMatch = m_pGaeaServer->IsClubDeathMatch(pLand->m_dwClubMapID);

	//
	//mjeon.CaptureTheField.Kill&Death count ó��
	//
	if (IsCTFPlayer())
	{
		if (pKillChar->IsCTFPlayer())
		{
			pKillChar->CTFKill();
		}
		else
		{
			sc::writeLogError(sc::string::format("GLChar::DoFalling() - CTF Killer(%1%) is NOT a CTF Player!! Check it out!", pKillChar->CharDbNum()));
		}

		CTFDeath();
	}			
	else if ( ( bClubBattle || bAllianceBattle ) && pLand->IsClubBattleZone() )
	{
		// Ŭ����Ʋ�� �׾��� ��� ó��

		ClubBattleSetPoint( m_ClubDbNum, pKillChar->m_ClubDbNum, bAllianceBattle );

		DWORD dwClubID_P, dwClubID_S;
		if ( bAllianceBattle ) 
		{
			dwClubID_P = pMyClub->m_dwAlliance;
			dwClubID_S = pTarClub->m_dwAlliance;
		}
		else
		{
			dwClubID_P = m_ClubDbNum;
			dwClubID_S = pKillChar->m_ClubDbNum;
		}

		GLMSG::SNET_CLUB_BATTLE_POINT_UPDATE NetMsg;

		NetMsg.dwBattleClubID = dwClubID_S;
		NetMsg.bKillPoint = false;

		SendToClient( &NetMsg );

		NetMsg.dwBattleClubID = dwClubID_P;
		NetMsg.bKillPoint = true;

		pKillChar->SendToClient( &NetMsg );

	}
	//	CDM ��Ȳ
	else if ( bClubDeathMatch )
	{
		GLClubDeathMatch* pCDM = m_pGaeaServer->ClubDeathMatchFind(pLand->m_dwClubMapID);
		if (pCDM)
		{
			//pCDM->AddCDMScore( pCHAR->m_dwGuild, m_dwGuild, pCHAR->m_dwCharID, m_dwCharID );
			m_pGaeaServer->ClubDeathMatchAddCdmScore(pCDM, pKillChar->m_ClubDbNum, m_ClubDbNum, pKillChar->m_CharDbNum, m_CharDbNum);

			//	POINT UPDATE ����, ����
			GLMSG::SNET_CLUB_DEATHMATCH_POINT_UPDATE NetMsg;
			NetMsg.bKillPoint = false;					
			m_pGaeaServer->SENDTOCLUBCLIENT_ONMAP ( pLand->GetBaseMapID().dwID, m_ClubDbNum, &NetMsg );

			NetMsg.bKillPoint = true;
			m_pGaeaServer->SENDTOCLUBCLIENT_ONMAP ( pKillChar->GetCurrentMap().getBaseMapID().Id(), pKillChar->m_ClubDbNum, &NetMsg );

		}

	}
	//	������ ��Ȳ�϶�
	else if ( bGuidBattleMap ) 
	{
		GLGuidance* pGuid = m_pGaeaServer->GuidanceFind(pLand->m_dwClubMapID);
		if (pGuid) 
			pGuid->AddKillLog(pKillChar->m_CharDbNum, m_CharDbNum);

	}
	// ���� ���� �������� ���.
	else if ( !IS_HOSTILE_ACTOR(pKillChar->m_CharDbNum) && pKillChar->IS_HOSTILE_ACTOR(m_CharDbNum) )
	{
		// ������ ����� ��� ���濡�� ��Ȱ���� ��.
		//			

		if ( ISOFFENDER() )
		{
			DWORD dwMY_PK_LEVEL = GET_PK_LEVEL();
			if ( dwMY_PK_LEVEL != UINT_MAX && dwMY_PK_LEVEL>0 )
			{
				// ��Ȱ���� ��ȭ.
				nLIFE = 7-(GLCONST_CHAR::EMPK_STATE_LEVEL-dwMY_PK_LEVEL);
				pKillChar->ReceiveLivingPoint ( nLIFE );
			}
		}
		//	�ڽ��� ����� �Ϲ��л��� ���, ���� �Ӽ�, ���� ����.
		else
		{					
			bool bFreePKMap = pLand->IsFreePK();
			bool bSCHOOL_FREEPK = SchoolFreePkOn();
			bool bBRIGHTEVENT = IsBRIGHTEVENT(); // ���� �̺�Ʈ��						
			bool bPKMap = pLand->IsPKMap();					

			//	PK���� ���� Ŭ������ üũ�ؾߵ� 
			bool bPKServer = IsPKServer();
			if ( bPKServer ) 
			{
				if ( GetSchool() == pKillChar->GetSchool() ) bPKServer = false;
				else if ( pMyClub && pTarClub && 
					pMyClub->IsAllianceGuild_COMMON( pTarClub->m_DbNum, pTarClub->m_dwAlliance ) ) 	bPKServer = false;
			}


			if ( GetSchool() == pKillChar->GetSchool() )	bSCHOOL_FREEPK = false;

			if ( !( bSCHOOL_FREEPK ||bFreePKMap || bBRIGHTEVENT || bPKServer || bPKMap ) )
			{
				nBRIGHT = GLCONST_CHAR::nPK_KILL_BRIGHT_POINT;
				nLIFE = GLCONST_CHAR::nPK_KILL_LIVING_POINT;

				// �Ӽ���ġ ��ȭ.
				pKillChar->m_nBright += nBRIGHT;

				GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
				NetMsg.nBright = pKillChar->m_nBright;
				pKillChar->SendToClient( &NetMsg );

				GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = pKillChar->m_dwGaeaID;
				NetMsgBrd.nBright = pKillChar->m_nBright;
				pKillChar->SendMsgViewAround(&NetMsgBrd);

				// ��Ȱ���� ��ȭ.
				pKillChar->m_nLiving += nLIFE;

				GLMSG::SNETPC_UPDATE_LP NetMsgLp;
				NetMsgLp.nLP = pKillChar->m_nLiving;
				pKillChar->SendToClient(&NetMsgLp);
			}
		}
	}

	// �׿��� ��� �α� ���.            
	// ��� �� �α� ����
	LogCharAction(
		dwKILL_ID,                     // �����
		gamelog::ACT_KILL,			   // ����
		gamelog::ID_CHAR, m_CharDbNum, // ����
		0,							//	exp
		nBRIGHT,					//	bright
		nLIFE,						//	life
		0,							//	money
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
		);
}

void GLChar::DoFalling_Proc_Log()  // �׾��� ���� �α� ����� ó��;
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on DoFalling_Proc_Log", m_CharDbNum ) );
	}

    // ���� ������ �α� ����;
    m_cDamageLog.clear();

    // Club log
	GLLandMan* const pLand( GetLandMan() );
    if (m_sAssault.emCrow == CROW_MOB && m_sAssault.GaeaId != GAEAID_NULL && m_ClubDbNum != CLUB_NULL && pLand)
    {
        const GLCrow* pCrow = pLand->GetCrow(m_sAssault.GaeaId);
        if (pCrow)
        {
            // Log check
            if (GLogicData::GetInstance().IsClubLogDeadByMonster(pCrow->GetNativeId()))
            {
                // [%1%]���� [%2%]���� ���д��߽��ϴ�.
                // Club log
                std::string ClubLog(
                    stringformat(
                    ID2GAMEINTEXT("CLUB_LOG_26"),
                    m_szName,
                    pCrow->GetNameStr()));
                m_pGaeaServer->SENDCLUBLOG(m_ClubDbNum, club::LOG_KILLED_BY_MONSTER, ClubLog);
            }
        }
    }


    DWORD dwKILL_ID(0);
    //
    // �ڽſ��� ���� �ֱٿ� ������ ������ (��, ����Ÿ�� ���� ĳ����)
    //
    GLChar* const pKillChar = GetChar(m_sAssault.GaeaId);
    if ( pKillChar )
    {
        dwKILL_ID = pKillChar->m_CharDbNum;

        LogPlayerKill(std::string(pKillChar->GetName()), std::string(m_szName), false);
        pKillChar->LogPlayerKill(std::string(pKillChar->GetName()), std::string(m_szName), true);
    }
    //// ������ �α� ���.
    // ��Ȱ�ϱ� ��ư�� ������ �ʾ� �α׸� ������� ��찡 �־ ������ ������ �����.
    {
        gamelog::EMIDTYPE emKILL = gamelog::ID_MOB;
        if (m_sAssault.emCrow==CROW_PC)
            emKILL = gamelog::ID_CHAR;

		// ��� �� �α� ����
        LogCharAction(
            m_CharDbNum,					//	�����.
            gamelog::ACT_DIE,				//	����.
            emKILL, dwKILL_ID,			//	����.
            m_lnDeathPenaltyEXP,					// exp
            0,							// bright
            0,							// life
            0,							// money
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
            );
    }

    // �������� ������ ���ؼ��� �α׸� ����;

    //#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
    //EMSERVICE_PROVIDER sp = GetServiceProvider();
	// ��� �������� Tracing ����� ����� �� �ֵ��� ����
    //if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
    //{
        /*if ( m_bTracingUser )
        {
            GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
            TracingMsg.nUserNum  = GetUserID();
            TracingMsg.SetAccount(m_szUID);

            CString strTemp;
            strTemp.Format( "Dead!!, [%s][%s], Money:[%I64d]", m_szUID, m_szName, GetInvenMoney() );
            TracingMsg.SetLog(strTemp.GetString());
            SendToAgent(&TracingMsg);
        }*/
    //}
    //#endif
}

void GLChar::DieProc()
{
    // �������� Ŭ���̾�Ʈ�鿡�� �˸�.
    GLMSG::SNET_ACTION_BRD NetMsgBrd;
    NetMsgBrd.emCrow	= CROW_PC;
    NetMsgBrd.dwID		= m_dwGaeaID;
    NetMsgBrd.emAction	= GLAT_FALLING;
    NetMsgBrd.emCrowAssault = m_sAssault.emCrow;
    NetMsgBrd.dwGaeaIdAssault = m_sAssault.GaeaId;

    //	������ ����.
    SetActState(EM_ACT_DIE);

    // �ֺ��� Char ����.
    SendMsgViewAround(&NetMsgBrd);

    // �ڽ��� Ŭ���̾�Ʈ��.
    SendToClient(&NetMsgBrd);

    //	���� ��ȭ.
    TurnAction ( GLAT_FALLING );

    // �׾��� ��� ��ȯ���� ����
    //SummonAllRemove();
	RemoveAllChildCrow();

    // fucking Vehicle ó��;
    switch ( m_emVehicle )
    {
    case EMVEHICLE_OFF:
        break;
    case EMVEHICLE_DRIVER:
        {
            for( INT i = 1; i < MAX_PASSENGER_COUNT; i++ )
            {
                if( m_pVehicle->PassengerId(i) == GAEAID_NULL )
                    continue;

                GLChar* pCHAR = GetChar(m_pVehicle->PassengerId(i) );
                if ( pCHAR )
                {
                    pCHAR->ActiveWithVehicle( false, false );

                }

                m_pVehicle->PassengerIdSet(i, GAEAID_NULL);
            }
        }
        break;
    case EMVEHICLE_PASSENGER:
        {
            INT nIndex = -1;
            for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
            {
                if( m_pVehicle->PassengerId(i) == m_dwGaeaID )
                {
                    nIndex = i;
                    break;
                }
            }

            if ( nIndex > 0 && nIndex < MAX_PASSENGER_COUNT ) 
            {

                for( INT j = 0; j < MAX_PASSENGER_COUNT; j++ )
                {
                    if( m_pVehicle->PassengerId(j) == m_dwGaeaID || m_pVehicle->PassengerId(j) == GAEAID_NULL )
                        continue;

                    GLChar* pCHARSnd = GetChar(m_pVehicle->PassengerId(j) );
                    if( !pCHARSnd )
                        continue;

                    pCHARSnd->m_pVehicle->PassengerIdSet(nIndex, GAEAID_NULL);
                }
            }

            ActiveWithVehicle( false, false );
        }
        break;
    }
}

void GLChar::DieProc_CDM()
{
    DieProc();  // ���� ó��;	

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // CDM�� �ϰ�� CDM���� SafeTime����;
    DWORD dwCount = m_pGaeaServer->ClubDeathMatchAccrueCount(pLand->m_dwClubMapID, m_CharDbNum);
    if (dwCount > CDM_ACCRUE_COUNT)
        dwCount = CDM_ACCRUE_COUNT;
    if (dwCount < 1)
        dwCount = 1;

    m_fCDMSafeTime = GLCONST_CHAR::fCDM_SAFE_TIME_ACCRUE[dwCount-1];		

    GLMSG::SNETPC_UPDATE_CDM_SAFETIME_BRD	NetMsgBrd;
    NetMsgBrd.dwGaeaID = m_dwGaeaID;
    NetMsgBrd.fCDMSafeTime = m_fCDMSafeTime;

    // ����Ǿ��� ��� �ڽſ��� �˸�.;
    SendToClient(&NetMsgBrd);

    // ����Ǿ��� ��� �ֺ� ������� �˸�.;
    SendMsgViewAround(&NetMsgBrd);
}

void GLChar::SetDeathPenalty_Exp(const __int64 _penaltyExp /*=0.0f*/)
{
    m_lnDeathPenaltyEXP = _penaltyExp;
}

void GLChar::RestoreDeathPenalty_Exp(const __int64 _penaltyExp)
{
    if (m_lnDeathPenaltyEXP > m_sExperience.lnNow)
        m_lnDeathPenaltyEXP = m_sExperience.lnNow;

    m_lnDeathPenaltyEXP -= _penaltyExp;

    if ( m_lnDeathPenaltyEXP < 0 )
        m_lnDeathPenaltyEXP = 0;

    //if (m_lnDeathPenaltyEXP >= _lnRestorableEXP)
    //m_lnDeathPenaltyEXP -= _lnRestorableEXP;
}

void GLChar::DROP_PUTONITEM( DWORD _dwNUM, float _fRATE )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on DROP_PUTONITEM", m_CharDbNum ) );
	}

    for ( DWORD i=0; i<_dwNUM; ++i )
    {
        if ( sc::Random::getInstance().IsSucceed( _fRATE ) )
        {
            DWORD dwSLOT_NUM( 0 );

            DWORD dwSLOT_LIST[SLOT_NSIZE_S_2];
            for ( int j=0; j<SLOT_NSIZE_S_2; ++j )
            {
                EMSLOT emSLOT = EMSLOT( j );

                if ( VALID_SLOT_ITEM( emSLOT ) && !GET_SLOT_ITEM( emSLOT ).IsGM_GENITEM() )
                {
                    SITEM* pITEM = GET_SLOT_ITEMDATA( emSLOT );			
					if ( pITEM != NULL )
					{					
						//	�ŷ��ɼ�;
						const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM( emSLOT );
						if ( ( sItemCustom.IsThrow() != NULL ) )
							dwSLOT_LIST[dwSLOT_NUM++] = j;
					}
                }
            }

            if ( dwSLOT_NUM == 0 )
                break;

            DWORD dwSLOT = ( DWORD )( sc::Random::getInstance().GetValue() % dwSLOT_NUM );
            if ( dwSLOT >= SLOT_NSIZE_S_2 )
                break;

            dwSLOT = dwSLOT_LIST[dwSLOT];
            if ( !VALID_SLOT_ITEM( EMSLOT( dwSLOT ) ) )
                break;

            // ����� ������� �ʴ´�.;
            if ( dwSLOT == SLOT_VEHICLE )
                break;

            SITEMCUSTOM sITEM_CUSTOM = m_PutOnItems[dwSLOT];

            D3DXVECTOR3 vGenPos = D3DXVECTOR3( m_vPosition ) + D3DXVECTOR3( sc::Random::RANDOM_NUM() * 4.0f, 0.0f, sc::Random::RANDOM_NUM() * 4.0f );
            BOOL bCollision;
            D3DXVECTOR3 vCollision;
            bCollision = pLand->IsCollisionNavi
                (
                D3DXVECTOR3( vGenPos ) + D3DXVECTOR3( 0, +5, 0 ),
                D3DXVECTOR3( vGenPos ) + D3DXVECTOR3( 0, -5, 0 ),
                vCollision
                );

            if ( bCollision )
            {
                // �������� ���� ���� ��� ���;
				// ��� �� �α� ����
                LogItemExchange( 
                    sITEM_CUSTOM, 
                    gamelog::ID_CHAR, m_CharDbNum, 
                    gamelog::ID_CHAR, 0, 
                    ITEM_ROUTE_PK_DROP, sITEM_CUSTOM.wTurnNum, 
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DROP_ITEM_BY_PK,
					false );

                PGLPETFIELD pMyPet = GetMyPet();
                if ( pMyPet && pMyPet->IsValid () ) 
                {
                    for ( WORD i=0; i<pMyPet->m_wMaxSkillSlot; ++i )
                    {
                        // Note : ���� �� ��ų Ȯ�� �α� ;
						// ��� �� �α� ����
                        LogCharAction(
                            m_CharDbNum,		// �����.;
                            gamelog::ACT_PET_SKILL, // ����.;
                            gamelog::ID_CHAR, 0,  0,				
                            pMyPet->m_sActiveSkillID[i].wMainID,			// Pet Skill MID;
                            pMyPet->m_sActiveSkillID[i].wSubID, 			// Pet Skill SID;
                            0, 
							( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap());
                    }
                }

                //	������ ����.;
				RELEASE_SLOT_ITEM_SERVER( ( EMSLOT ) dwSLOT );

				// ��� ó��
				SINVENITEM_SAVE sItem;
				sItem.sItemCustom = sITEM_CUSTOM;
				int DbState = sItem.sItemCustom.GetDbState();
				if ( db::DB_NONE ==  DbState || db::DB_INSERT == DbState )
				{
					m_pGaeaServer->AddGameAdoJob(
						db::DbActionPtr(
						new db::ItemInsert( m_CharDbNum, INVEN_DELETED, sItem, 797346 ) ) );

					sITEM_CUSTOM.SetDbState( db::DB_UPDATE, true );
				}
				else
				{
					m_pGaeaServer->AddGameAdoJob(
						db::DbActionPtr(
						new db::ItemUpdate( m_CharDbNum, INVEN_DELETED, sItem, 797346 ) ) );
				}

                //	[�ڽſ���] SLOT�� �־��� ������ ����.;
                GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release( ( EMSLOT ) dwSLOT );
                SendToClient( &NetMsg_PutOn_Release );

                //	������ �߻�.;
                pLand->DropItem( vGenPos, &sITEM_CUSTOM );
            }
        }
    }
}

void GLChar::DeathPenaltyDefault()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    const bool _bConfront = m_sCONFTING.IsCONFRONTING();

    //---------------- ����ġ �϶� �г�Ƽ ���� ----------------------------
    // ������ �̰ų� ������ �׾������;
    if (ISOFFENDER() || m_sAssault.emCrow != CROW_PC)
    {
        const __int64 _tmpExp = __int64(GLOGICEX::GLDIE_DECEXP(GETLEVEL()) * 0.01f * GET_LEVELUP_EXP());		
        if (_bConfront)
        {
            //m_nDECEXP /= 10;  // ��ý� ����ġ �ջ��� �ݰ���Ų��.
            SetDeathPenalty_Exp();  // ��ý� ����ġ �ջ��� ����.
        }
        else
        {
            SetDeathPenalty_Exp(__int64(_tmpExp * GET_PK_DECEXP_RATE() / 100.0f));
        }
    }

    // ���� �̺�Ʈ �߿��� ����ġ �������� �ʰ� �����۵� ������� ����;
    if ( IsBRIGHTEVENT() ) 
    {
        SetDeathPenalty_Exp();
        return;
    }

    // Ŭ�� ��Ʋ���̸� ����ġ �ս� �� ������ ��� ����
    if ( m_sAssault.emCrow==CROW_PC && pLand->IsClubBattleZone() )
    {		
        std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
        if ( pCLUB )
        {
            GLChar* pChar = GetChar( m_sAssault.GaeaId );
            if ( pChar )
            {
                if ( pCLUB->IsBattle( pChar->m_ClubDbNum ) )
                {
                    SetDeathPenalty_Exp();
                    return;
                }

                std::tr1::shared_ptr<GLClubField> pCLUB_Tar = GetClub(pChar->m_ClubDbNum);
                if ( pCLUB_Tar && pCLUB->IsBattleAlliance( pCLUB_Tar->m_dwAlliance ) )
                {
                    SetDeathPenalty_Exp();
                    return;
                }
            }
        }
    }

    // ����ġ NonDrop ���ϰ��;
    if (!pLand->IsDecreaseExpMap())
        SetDeathPenalty_Exp();

    // ����ġ �϶����� ������(�ð��� ��ȥ��) ����� ����ġ �϶� ����;
    bool bItemRebirth = FindRevive() ? true : false;
    if ( bItemRebirth )
    {
        // �Ҹ� ��ȥ���̸� �ڵ� ��ȥ�� ���;
        SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_DECORATION);
        if ( (pITEM != NULL) && !pITEM->ISINSTANCE() )
            SetDeathPenalty_Exp();
    }


    //---------------- ������ ��� �г�Ƽ ���� ----------------------------
    // ��Ӹ��� ��� ������ ���. ( �������, CTF ����, ������� ��ȣ��ų ����);
    if (pLand->IsItemDrop() && !_bConfront && !IsCTFPlayer())	//mjeon.CaptureTheField
    {
        //	�� ������ ��ȣ������ Ȯ��;
        GLPetField* pMyPet = GetMyPet();
        if ( !pMyPet || !pMyPet->IsValid () || !pMyPet->IsItemProtect() ) 
        {
            //	�������� ���;
            if ( ISOFFENDER() )
            {
                DWORD dwDROP_NUM = GET_PK_ITEMDROP_NUM();
                float fDROP_RATE = GET_PK_ITEMDROP_RATE();
                DROP_PUTONITEM ( dwDROP_NUM, fDROP_RATE );
            }
            //	�Ϲ������� �������.
            else if (GLCONST_CHAR::fNONPK_ITEM_DROP >= 0.001)
            {
                DROP_PUTONITEM ( 1, GLCONST_CHAR::fNONPK_ITEM_DROP );
            }
        }
    }

    m_bSetDeathPenalty = true;
}

void GLChar::DeathPenalty(const unsigned int _nPenaltyExp, const unsigned int _nDropItem, const float _fDropRate)
{
    if ( _nPenaltyExp > 0 ) 
        SetDeathPenalty_Exp(_nPenaltyExp);

    if ( _nDropItem > 0 )
        DROP_PUTONITEM (_nDropItem, _fDropRate);

    m_bSetDeathPenalty = true;
}

void GLChar::UpdateForcedResurrect( const float _fElapseTime)
{
    if ( IsActState(EM_ACT_DIE) && ( m_sResurrectAbleState.isState(EM_RESURRECTION_FORCED) ) )
    {
        m_fForcedResurrectTime -= _fElapseTime;
        if ( m_fForcedResurrectTime < 0.0f )
        {
            m_fForcedResurrectTime = 0.0f;
            Resurrect(EM_RESURRECTION_FORCED);
        }
    }
}