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
    // 인던 리뉴얼/ HP가 0일때 죽일지 안죽일지에 대해서는 lua에서 제어를 할 수 있도록 함;	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
    {
        DWORD dwID(m_sAssault.GaeaId);

		// Mob일 경우는 그대로 넣으면 된다;
		// PC일 경우 DBNum을 쓰기 때문에 따로 넣어야 한다;
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

				// 현재는 소환수의 주인이 SUMMON일 경우는;
				// 생각하지 않는다;
				if ( pOwner && pOwner->GetCrow() == CROW_PC )
				{
					GLChar* pChar = static_cast< GLChar* >( pOwner );
					if ( pChar )
						dwID = pChar->CharDbNum();
				}
			}
		}

        // 만약 Event Died의 리턴값이 false일 경우 죽이는 처리를 하지 않는다는 뜻;
        if ( _pInstance->EventDie(CROW_PC, CharDbNum(), m_sAssault.emCrow, dwID) == false )
		{
			return;
		}
    }

    // PVE인던용 죽음 트리거;
    TriggerSystem::SServerDieMessage msg;
    msg.dwCharDbNum = CharDbNum();
    SendTriggerMessage(&msg);  

    // 아직 정해진 사망 패널티가 없다면;
    if ( m_bSetDeathPenalty == false )
    {
        // 일반적인 사망에 대한 패널티 처리( 경험치, 아이템, 금액 감소 등 );
        DeathPenaltyDefault();
    }
    m_bSetDeathPenalty = false;

	// 매크로 켜져있으면 끈다;
	if ( IsActState(EM_ACT_MACROMODE) )
	{
		MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SendToClient ( &msg );
	}

    // 아직 정해진 부활설정이 없다면;
    if ( m_sResurrectAbleState.isState(EM_RESURRECTION_TYPE_NOT_SET) )
    {
        // 기본적인 부활 타입을 정한다.;
        SetDefaultResurrection();
    }

    //실제 죽음 처리. 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 );
    if ( m_sCONFTING.IsCONFRONTING() )
        DoFalling_Proc_Confront();
    else
        DoFalling_Proc();

	// 같은 파티나 같은 Faction이 모두 죽은 상태라면 호출한다;
	Faction::ManagerField* pFactionManager = pLand->GetFactionManager();
	if ( NULL == pFactionManager )
		return;

	const Faction::FactionID* pFactionID = pFactionManager->getFactionID( GetActorID() );
	if ( NULL == pFactionID )
		return;

	// 같은 진영의 멤버를 얻어온다;
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

	// 모두 죽은 상태이면 콜백을 호출한다;
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

            // 대련도중 대련자가 아닌 캐릭터나 몹에게 죽을 경우.
            //
            if( m_sAssault != sCONFTID )
            {
                // 상대방 대련자 ID 백업. Reset 시에 무효화되기 때문에 미리 백업.
                //
                const DWORD dwTAR_ID = m_sCONFTING.dwTAR_ID;

                // (자신) 대련 종료.
                //
                ResetConfront(EMCONFRONT_END_FAIL);

                // 대련 상대방 찾음.
                //
                GLChar* pCONFT = GetChar(dwTAR_ID);
                if ( pCONFT )
                {
                    // 대련 상대방이 영향을 준 경험치 로그 제거시킴.
                    //
                    DAMAGELOG_ITER iter;
                    iter = m_cDamageLog.find ( pCONFT->m_dwUserID );
                    if ( iter!=m_cDamageLog.end() )		m_cDamageLog.erase ( iter );

                    // (상대방) 대련 종료.
                    //
                    pCONFT->ResetConfront ( EMCONFRONT_END_FAIL );
                }

                DoFalling_Proc();
                return;
            }

            // 상대방 대련자 ID 백업. Reset 시에 무효화되기 때문에 미리 백업.
            //
            const DWORD dwTAR_ID = m_sCONFTING.dwTAR_ID;
            const SCONFT_OPTION sOption = m_sCONFTING.sOption;

            // (자신) 대련 종료.
            //
            ResetConfront(EMCONFRONT_END_LOSS);

            // 대련 상대방 찾음.
            //
            GLChar* pCONFT = GetChar(dwTAR_ID);
            if ( pCONFT==NULL )	
                return;

            // (상대방) 대련 종료.
            //
            EMCONFRONT_END emCONFT_END = (pCONFT->GETHP()>0) ? EMCONFRONT_END_WIN : EMCONFRONT_END_LOSS;
            pCONFT->ResetConfront ( emCONFT_END );

            // 자신 생활점수 변화.
            //
            if ( GLCONST_CHAR::nCONFRONT_LOSS_LP!=0 && GLCONST_CHAR::nCONFRONT_WIN_LP!=0 )
            {
                float fLIFE_P(0);

                fLIFE_P = GLCONST_CHAR::nCONFRONT_LOSS_LP - (GETLEVEL()-pCONFT->GETLEVEL())/2.0f
                    + (sOption.fHP_RATE-sOption.fTAR_HP_RATE);
                LIMIT ( fLIFE_P, 0.0f, -20.0f );

                ReceiveLivingPoint ( (int)fLIFE_P );

                // 상대방 생활점수 변화.
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

                // (자신) 대련 종료.
                //
                ResetConfront(EMCONFRONT_END_NOTWIN);

                DoFalling_Proc();
                return;
            }

            // (자신) 대련 종료.
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

                // (자신) 대련 종료.
                //
                ResetConfront(EMCONFRONT_END_NOTWIN);

                DoFalling_Proc();
                return;
            }

            // (자신) 대련 종료.
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

    // 플레이어에 의해 죽었을 때에 대한 처리;
    if (m_sAssault.emCrow == CROW_PC &&
        m_sAssault.GaeaId != GAEAID_NULL) // 가끔 gaea id가 null임. hp소모스킬(분격)등을 사용시 죽을수도..;
    {
        DoFalling_Proc_By_PVP();
    }

    // 죽음에 관련된 로그 기록 처리;
    DoFalling_Proc_Log();

    // 죽음 행위에 대한 처리;
    if (pLand->m_bClubDeathMatchMap)
        DieProc_CDM();
    else
        DieProc();  // 죽음 처리;

    //---------------------- 죽음에 관련한 기타 처리 -------------------------;

    // 적대 행위자 삭제.;
    RemoveAllPlayHostiles();

    // 퀘스트 진행 목록중 죽을시 실패 옵션 검사후 처리.;
    DoQuestCheckLimitDie ();

    // 사망시 내구도 감소;
    if( m_sAssault.emCrow != CROW_PC )  // 사람에게 죽었다면 PVP로 처리;
        DurabilityDropDeath( FALSE );
    else
        DurabilityDropDeath( TRUE );

    // 죽인 상대방에게 CP 부여 및 알림;
    GLChar* pKillChar = GetChar(m_sAssault.GaeaId);		
    if ( pKillChar )
    {
        // pKillChar가 지배 당한 상태면 지배 시전자로 댓가 전이;
        STARGETID dwActorID = STARGETID(CROW_PC, m_dwGaeaID);
        pKillChar->ReceiveKillCP(dwActorID);
		pKillChar->NotifyKillSomeone(m_dwGaeaID);
    }

    // 플레이어 사망시 Q Box 효과를 삭제;
    if (m_sQITEMFACT.IsACTIVE())
    {
        m_sQITEMFACT.RESET();

        GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
        NetMsgBrd.dwGaeaID = m_dwGaeaID;

        // 종료되었을 경우 자신에게 알림.;
        SendToClient(&NetMsgBrd);

        // 종료되었을 경우 주변 사람에게 알림.;
        SendMsgViewAround(&NetMsgBrd);
    }

    // 배틀로얄 이벤트일때 쓰러지면 투명모드로 전환한다.;
    if ( GLCONST_CHAR::bBATTLEROYAL )
    {
        SetActState(EM_REQ_VISIBLEOFF);

        // (에이젼트서버) 메세지 발생.;
        GLMSG::SNETPC_ACTSTATE NetMsgFld;
        NetMsgFld.dwChaNum = CharDbNum();
        NetMsgFld.dwSvrGroupNum = SvrGroupNum();
        NetMsgFld.dwActState = GetActState();
        SendToAgent(&NetMsgFld);
    }
}

void GLChar::DoFalling_Proc_By_PVP()  // 플레이어에 의해 죽었을 때에 대한 처리;
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on DoFalling_Proc_By_PVP", m_CharDbNum ) );
	}

    DWORD dwKILL_ID(0);
    //
    // 자신에게 가장 최근에 공격을 가한자 (즉, 결정타를 가한 캐릭터)
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
	//mjeon.CaptureTheField.Kill&Death count 처리
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
		// 클럽배틀시 죽었을 경우 처리

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
	//	CDM 상황
	else if ( bClubDeathMatch )
	{
		GLClubDeathMatch* pCDM = m_pGaeaServer->ClubDeathMatchFind(pLand->m_dwClubMapID);
		if (pCDM)
		{
			//pCDM->AddCDMScore( pCHAR->m_dwGuild, m_dwGuild, pCHAR->m_dwCharID, m_dwCharID );
			m_pGaeaServer->ClubDeathMatchAddCdmScore(pCDM, pKillChar->m_ClubDbNum, m_ClubDbNum, pKillChar->m_CharDbNum, m_CharDbNum);

			//	POINT UPDATE 나랑, 상대방
			GLMSG::SNET_CLUB_DEATHMATCH_POINT_UPDATE NetMsg;
			NetMsg.bKillPoint = false;					
			m_pGaeaServer->SENDTOCLUBCLIENT_ONMAP ( pLand->GetBaseMapID().dwID, m_ClubDbNum, &NetMsg );

			NetMsg.bKillPoint = true;
			m_pGaeaServer->SENDTOCLUBCLIENT_ONMAP ( pKillChar->GetCurrentMap().getBaseMapID().Id(), pKillChar->m_ClubDbNum, &NetMsg );

		}

	}
	//	선도전 상황일때
	else if ( bGuidBattleMap ) 
	{
		GLGuidance* pGuid = m_pGaeaServer->GuidanceFind(pLand->m_dwClubMapID);
		if (pGuid) 
			pGuid->AddKillLog(pKillChar->m_CharDbNum, m_CharDbNum);

	}
	// 서로 적대 행위자일 경우.
	else if ( !IS_HOSTILE_ACTOR(pKillChar->m_CharDbNum) && pKillChar->IS_HOSTILE_ACTOR(m_CharDbNum) )
	{
		// 범죄자 등급인 경우 상대방에게 생활점수 줌.
		//			

		if ( ISOFFENDER() )
		{
			DWORD dwMY_PK_LEVEL = GET_PK_LEVEL();
			if ( dwMY_PK_LEVEL != UINT_MAX && dwMY_PK_LEVEL>0 )
			{
				// 생활점수 변화.
				nLIFE = 7-(GLCONST_CHAR::EMPK_STATE_LEVEL-dwMY_PK_LEVEL);
				pKillChar->ReceiveLivingPoint ( nLIFE );
			}
		}
		//	자신의 등급이 일반학생일 경우, 상대방 속성, 생점 조절.
		else
		{					
			bool bFreePKMap = pLand->IsFreePK();
			bool bSCHOOL_FREEPK = SchoolFreePkOn();
			bool bBRIGHTEVENT = IsBRIGHTEVENT(); // 성향 이벤트용						
			bool bPKMap = pLand->IsPKMap();					

			//	PK서버 같은 클럽인지 체크해야됨 
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

				// 속성수치 변화.
				pKillChar->m_nBright += nBRIGHT;

				GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
				NetMsg.nBright = pKillChar->m_nBright;
				pKillChar->SendToClient( &NetMsg );

				GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = pKillChar->m_dwGaeaID;
				NetMsgBrd.nBright = pKillChar->m_nBright;
				pKillChar->SendMsgViewAround(&NetMsgBrd);

				// 생활점수 변화.
				pKillChar->m_nLiving += nLIFE;

				GLMSG::SNETPC_UPDATE_LP NetMsgLp;
				NetMsgLp.nLP = pKillChar->m_nLiving;
				pKillChar->SendToClient(&NetMsgLp);
			}
		}
	}

	// 죽였을 경우 로그 기록.            
	// 통계 및 로그 수정
	LogCharAction(
		dwKILL_ID,                     // 당사자
		gamelog::ACT_KILL,			   // 행위
		gamelog::ID_CHAR, m_CharDbNum, // 상대방
		0,							//	exp
		nBRIGHT,					//	bright
		nLIFE,						//	life
		0,							//	money
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
		);
}

void GLChar::DoFalling_Proc_Log()  // 죽었을 때에 로그 남기는 처리;
{
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on DoFalling_Proc_Log", m_CharDbNum ) );
	}

    // 누적 데미지 로그 삭제;
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
                // [%1%]님이 [%2%]에게 제압당했습니다.
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
    // 자신에게 가장 최근에 공격을 가한자 (즉, 결정타를 가한 캐릭터)
    //
    GLChar* const pKillChar = GetChar(m_sAssault.GaeaId);
    if ( pKillChar )
    {
        dwKILL_ID = pKillChar->m_CharDbNum;

        LogPlayerKill(std::string(pKillChar->GetName()), std::string(m_szName), false);
        pKillChar->LogPlayerKill(std::string(pKillChar->GetName()), std::string(m_szName), true);
    }
    //// 죽을때 로그 기록.
    // 부활하기 버튼이 나오지 않아 로그를 못남기는 경우가 있어서 죽으면 무조건 남긴다.
    {
        gamelog::EMIDTYPE emKILL = gamelog::ID_MOB;
        if (m_sAssault.emCrow==CROW_PC)
            emKILL = gamelog::ID_CHAR;

		// 통계 및 로그 수정
        LogCharAction(
            m_CharDbNum,					//	당사자.
            gamelog::ACT_DIE,				//	행위.
            emKILL, dwKILL_ID,			//	상대방.
            m_lnDeathPenaltyEXP,					// exp
            0,							// bright
            0,							// life
            0,							// money
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
            );
    }

    // 추적중인 유저에 대해서는 로그를 남김;

    //#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
    //EMSERVICE_PROVIDER sp = GetServiceProvider();
	// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
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
    // 쓰러짐을 클라이언트들에게 알림.
    GLMSG::SNET_ACTION_BRD NetMsgBrd;
    NetMsgBrd.emCrow	= CROW_PC;
    NetMsgBrd.dwID		= m_dwGaeaID;
    NetMsgBrd.emAction	= GLAT_FALLING;
    NetMsgBrd.emCrowAssault = m_sAssault.emCrow;
    NetMsgBrd.dwGaeaIdAssault = m_sAssault.GaeaId;

    //	쓰러짐 설정.
    SetActState(EM_ACT_DIE);

    // 주변의 Char 에게.
    SendMsgViewAround(&NetMsgBrd);

    // 자신의 클라이언트에.
    SendToClient(&NetMsgBrd);

    //	행위 변화.
    TurnAction ( GLAT_FALLING );

    // 죽었을 경우 소환수를 해제
    //SummonAllRemove();
	RemoveAllChildCrow();

    // fucking Vehicle 처리;
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
    DieProc();  // 죽음 처리;	

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // CDM맵 일경우 CDM전용 SafeTime적용;
    DWORD dwCount = m_pGaeaServer->ClubDeathMatchAccrueCount(pLand->m_dwClubMapID, m_CharDbNum);
    if (dwCount > CDM_ACCRUE_COUNT)
        dwCount = CDM_ACCRUE_COUNT;
    if (dwCount < 1)
        dwCount = 1;

    m_fCDMSafeTime = GLCONST_CHAR::fCDM_SAFE_TIME_ACCRUE[dwCount-1];		

    GLMSG::SNETPC_UPDATE_CDM_SAFETIME_BRD	NetMsgBrd;
    NetMsgBrd.dwGaeaID = m_dwGaeaID;
    NetMsgBrd.fCDMSafeTime = m_fCDMSafeTime;

    // 종료되었을 경우 자신에게 알림.;
    SendToClient(&NetMsgBrd);

    // 종료되었을 경우 주변 사람에게 알림.;
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
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
						//	거래옵션;
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

            // 보드는 드랍되지 않는다.;
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
                // 아이템의 소유 이전 경로 기록;
				// 통계 및 로그 수정
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
                        // Note : 현재 펫 스킬 확인 로그 ;
						// 통계 및 로그 수정
                        LogCharAction(
                            m_CharDbNum,		// 당사자.;
                            gamelog::ACT_PET_SKILL, // 행위.;
                            gamelog::ID_CHAR, 0,  0,				
                            pMyPet->m_sActiveSkillID[i].wMainID,			// Pet Skill MID;
                            pMyPet->m_sActiveSkillID[i].wSubID, 			// Pet Skill SID;
                            0, 
							( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap());
                    }
                }

                //	아이템 제거.;
				RELEASE_SLOT_ITEM_SERVER( ( EMSLOT ) dwSLOT );

				// 디비 처리
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

                //	[자신에게] SLOT에 있었던 아이탬 제거.;
                GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release( ( EMSLOT ) dwSLOT );
                SendToClient( &NetMsg_PutOn_Release );

                //	아이템 발생.;
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

    //---------------- 경험치 하락 패널티 관련 ----------------------------
    // 범죄자 이거나 몹한테 죽었을경우;
    if (ISOFFENDER() || m_sAssault.emCrow != CROW_PC)
    {
        const __int64 _tmpExp = __int64(GLOGICEX::GLDIE_DECEXP(GETLEVEL()) * 0.01f * GET_LEVELUP_EXP());		
        if (_bConfront)
        {
            //m_nDECEXP /= 10;  // 대련시 경험치 손상은 반감시킨다.
            SetDeathPenalty_Exp();  // 대련시 경험치 손상은 없음.
        }
        else
        {
            SetDeathPenalty_Exp(__int64(_tmpExp * GET_PK_DECEXP_RATE() / 100.0f));
        }
    }

    // 성향 이벤트 중에는 경험치 감소하지 않고 아이템도 드랍되지 않음;
    if ( IsBRIGHTEVENT() ) 
    {
        SetDeathPenalty_Exp();
        return;
    }

    // 클럽 배틀중이면 경험치 손실 및 아이템 드랍 없음
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

    // 경험치 NonDrop 맵일경우;
    if (!pLand->IsDecreaseExpMap())
        SetDeathPenalty_Exp();

    // 경험치 하락방지 아이템(시간제 귀혼주) 착용시 경험치 하락 방지;
    bool bItemRebirth = FindRevive() ? true : false;
    if ( bItemRebirth )
    {
        // 소모성 귀혼주이면 자동 귀혼주 사용;
        SITEM* pITEM = GET_SLOT_ITEMDATA(SLOT_DECORATION);
        if ( (pITEM != NULL) && !pITEM->ISINSTANCE() )
            SetDeathPenalty_Exp();
    }


    //---------------- 아이템 드랍 패널티 관련 ----------------------------
    // 드롭맵일 경우 아이템 드롭. ( 대련제외, CTF 제외, 펫아이템 보호스킬 제외);
    if (pLand->IsItemDrop() && !_bConfront && !IsCTFPlayer())	//mjeon.CaptureTheField
    {
        //	펫 아이템 보호중인지 확인;
        GLPetField* pMyPet = GetMyPet();
        if ( !pMyPet || !pMyPet->IsValid () || !pMyPet->IsItemProtect() ) 
        {
            //	범죄자일 경우;
            if ( ISOFFENDER() )
            {
                DWORD dwDROP_NUM = GET_PK_ITEMDROP_NUM();
                float fDROP_RATE = GET_PK_ITEMDROP_RATE();
                DROP_PUTONITEM ( dwDROP_NUM, fDROP_RATE );
            }
            //	일반적으로 죽을경우.
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