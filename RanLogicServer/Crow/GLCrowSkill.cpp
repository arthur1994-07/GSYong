#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "./GLCrow.h"
#include "../Summon/GLSummonField.h"
#include "../FieldServer/GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLCrow::PreSkillProc ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	GASSERT(m_pAttackProp);
	if ( !m_pAttackProp )
		return FALSE;

	SNATIVEID sRunSkill = m_pAttackProp->skill_id;
	WORD wLevel = m_pAttackProp->skill_lev;

	//	Note : 스킬 수행 가능 검사.
	//
	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;
	WORD wStrikeNum = sAniAttack.m_wDivCount;
	
	EMSKILLCHECK emSkillCheck = CHECHSKILL ( m_dwAType );
	if ( emSkillCheck!=EMSKILL_OK && emSkillCheck!=EMSKILL_NOTSP)
	{
		return FALSE;
	}


	//	Note : 목표물의 위치를 항상 갱신해야 한다.
	//
	ServerActor *pActor = m_pGaeaServer->GetTarget(pLand, m_TargetID);
	if ( pActor )
		m_TargetID.vPos = pActor->GetPosition();

	//	Note : Skill 목표를 선택함.
	//
	D3DXVECTOR3 vTarPos(0,0,0);
	const BOOL bOK = SelectSkillTarget ( m_TargetID, &vTarPos, sRunSkill, wLevel );
	//const BOOL bOK = pLand->m_SkillTargetSystem.SelectSkillTarget( this, pActor, m_pAttackProp->skill_id, wLevel, GetPositionActor(), m_sTARIDS, m_wTARNUM);
	
	if ( bOK == FALSE )
		return FALSE;

	SETACTIVESKILL ( sRunSkill, wLevel );

	//	Note : 스킬이 발동됨.
	//
	TurnAction ( GLAT_SKILL );

	return TRUE;
}

void GLCrow::StartSkillProc ()
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )								return;

	//	Note : 스킬사용 소모 수치 감산.
	ACCOUNTSKILL ( 1 );

	//	Note : 스킬 메시지 발생.
	//
	GLMSG::SNETCROW_SKILL NetMsg(m_dwGaeaID);

	NetMsg.dwAType = m_dwAType;
	NetMsg.skill_id = m_idACTIVESKILL;
	NetMsg.skill_lev = m_wACTIVESKILL_LVL;
	NetMsg.vTARPOS = m_vTARPOS;
	NetMsg.vPos = m_vPosition;

	for (WORD i=0; i<m_wTARNUM; ++i)
        NetMsg.ADDTARGET(m_sTARIDS[i]);
	SendMsgViewAround(&NetMsg);
	
	STARGETID sTargetID = STARGETID ( CROW_MOB, m_dwGaeaID, m_vPosition );
	
	if( m_TargetID.emCrow == CROW_PC )
	{
		PGLCHAR pCHAR = m_pGaeaServer->GetChar ( m_TargetID.GaeaId );
		if ( pCHAR )
		{
			// PET Attack 처리
			PGLPETFIELD pEnemyPet = m_pGaeaServer->GetPET ( pCHAR->m_dwPetGUID );
			if ( pEnemyPet && pEnemyPet->IsValid () && !pEnemyPet->IsSTATE ( EM_PETACT_ATTACK ) )
			{
				D3DXVECTOR3 vOwnerPos, vDist;
				float fDist;
				vOwnerPos = pCHAR->GetPosition ();
				vDist = pEnemyPet->m_vPos - vOwnerPos;
				fDist = D3DXVec3Length(&vDist);

				// 일정거리 안에 있으면 
				if ( fDist <= GLCONST_PET::fWalkArea )
				{
					pEnemyPet->ReSetAllSTATE ();
					pEnemyPet->SetSTATE ( EM_PETACT_ATTACK );

					GLMSG::SNETPET_ATTACK NetMsg(sTargetID);
					m_pGaeaServer->SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsg );

					GLMSG::SNETPET_ATTACK_BRD NetMsgBRD(sTargetID);
					NetMsgBRD.dwGUID = pEnemyPet->m_dwGUID;
					pCHAR->SendMsgViewAround(&NetMsgBRD);
				}
			}
			// Summon Attack 처리 안함
		}
	}

	return;
}

void GLCrow::SkillProc ( BOOL bLowSP )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !m_pAttackProp )
        return;

	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	//	스킬 정보 가져옴.
	SNATIVEID skill_id = m_idACTIVESKILL;
	WORD wSKILL_LVL = m_wACTIVESKILL_LVL;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id );
	if (!pSkill)
        return;

    if (wSKILL_LVL >= SKILL::MAX_LEVEL)
        wSKILL_LVL = SKILL::MAX_LEVEL-1;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL];

	// Note : 날씨정보 가져옴
	DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );

	int nEXP_NUM = 0;
	int nALL_LVL = 0;
	int nALL_VARHP = 0, nALL_FULLHP = 0;
	int nALL_VARMP = 0, nALL_FULLMP = 0;
	int nALL_VARSP = 0, nALL_FULLSP = 0;

	WORD wMAX_TAR = 0;
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
		wMAX_TAR = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1;
	else if ( pSkill->m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
		wMAX_TAR = sSKILL_DATA.wTARNUM * ( sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1 );
	else
		wMAX_TAR = sSKILL_DATA.wTARNUM;

	if ( m_wTARNUM > wMAX_TAR )
		m_wTARNUM = wMAX_TAR;

	//	Note : 스킬이 적용되는 타겟 검사.
	//	
	for ( WORD i=0; i<m_wTARNUM; ++i )
	{		
		const STARGETID& TargetID = m_sTARIDS[i];
		
		ServerActor* const pTargetActor = m_pGaeaServer->GetTarget(pLand, TargetID);
		if ( pTargetActor == NULL )
			continue;

		// GM 캐릭터 visible off 상태라면 스킬 적용 제외
		if( CROW_PC == pTargetActor->GetCrow() )
		{
			GLChar* pChar = m_pGaeaServer->GetChar( pTargetActor->GetGaeaID() );
			if( NULL == pChar )
				continue;

			if( USER_USER_GM <= pChar->UserLevel() &&
				true == pChar->IsActState( EM_REQ_VISIBLEOFF ) )
				continue;
		}

		/*======== 반격 적용 효과 결과 뽑아냄 =========================================================================================*/

		CSkillCAInterface cSkillCA;

		STARGETID sCId   = STARGETID(CROW_MOB,m_dwGaeaID);;
		STARGETID sCTaId = TargetID;

		bool bCaDamage = false;

		switch( TargetID.emCrow )
		{
		case CROW_PC :
			{
				GLChar* const pTarget = dynamic_cast<GLChar*>(pTargetActor);
				
				cSkillCA.Cal_LastApplyValueII( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;
				}
			}
			break;
		case CROW_MOB :
			{
				GLCrow* pTarget = dynamic_cast<GLCrow*>(pTargetActor);

				cSkillCA.Cal_LastApplyValueII( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;
				}
			}
			break;
		}
		/*===============================================================================================================================*/

		if ( pTargetActor->GetNowHP() != 0 )
		{
			//	Note : 물리 데미지 발생.
			//
			DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
			int nVAR_HP(0), nVAR_MP(0), nVAR_SP(0);

			//	저항치.
			short nRESIST = pTargetActor->GETRESIST().GetElement(pSkill->m_sAPPLY.emELEMENT);
			if ( nRESIST>99 )	nRESIST = 99;


			//	SKILL 기본 적용.
			//
			switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
			{
			case SKILL::EMFOR_HP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					dwDamageFlag = CALCDAMAGE ( nVAR_HP, m_dwGaeaID, TargetID, pLand, pSkill, wSKILL_LVL, dwWeather, sAniAttack.m_wDivCount );
					nVAR_HP = - nVAR_HP;
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_HP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;

			case SKILL::EMFOR_MP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_MP -= (int) ( nVAR - (nVAR*nRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_MP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;

			case SKILL::EMFOR_SP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_SP -= (int) ( nVAR - (nVAR*nRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_SP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;
			};

			//	Note : SP 부족시에는 변화 값을 반으로 조정.
			//
			if ( bLowSP )
			{
				nVAR_HP /= 2;
				nVAR_MP /= 2;
				nVAR_SP /= 2;
			}

			// Note : 블로킹 검사
			//
			if ( CalcBlocking(pTargetActor, nVAR_HP, pSkill, wSKILL_LVL) == true )
				BlockProc(TargetID, skill_id, wSKILL_LVL);
			else
			{
				//	Note : 스킬 특수 기능.
				//
				int nGATHER_HP(0), nGATHER_MP(0), nGATHER_SP(0);

				for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
				{
					const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSKILL_LVL];

					switch ( pSkill->m_sAPPLY.vecSPEC[k].emSPEC )
					{
					case EMSPECA_PUSHPULL:
						if (!pTargetActor->IsValidBody())
							break;
						if (sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR2)
							break;	//	Note : 발생확율 적용.
						dwDamageFlag |= DAMAGE_TYPE_SHOCK;
						break;

					case EMSPECA_HP_GATHER:
						nGATHER_HP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_MP_GATHER:
						nGATHER_MP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_SP_GATHER:
						nGATHER_SP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_SWAPPOS:
						ApplySkillSelfSwapPos( this, pSkill, wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL], 1, &m_sTARIDS[i], pLand);
						break;

					case EMSPECA_TELEPORT:
						ApplySkillSelfTeleport(m_pGaeaServer, this, pSkill, wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL], TargetID, m_wTARNUM, m_sTARIDS );
						break;
					case EMSPECA_VEHICLE_GETOFF: //봉주
						if( !pTargetActor->IsValidBody() )
							break;

						if( (sc::Random::RANDOM_POS() < sSKILL_SPEC.fVAR1) && (pTargetActor->GetCrow() == CROW_PC) )   // 효과 적용 성공율
						{
							GLChar *pTargetChar = dynamic_cast<GLChar *>(pTargetActor);
							if (pTargetChar)
								pTargetChar->ToggleVehicle(false);
						}
						break;
					case EMSPECA_LINK_HP:
						if( !pTargetActor->IsValidBody() )
							break;				
						const float fMaxHP = static_cast<float>(GetMaxHP());
						const float fNoWHP = static_cast<float>(GetNowHP());
						pTargetActor->SetMaxHpRate( fNoWHP / fMaxHP );
						//}
						break;	
					};
				}

				if( bCaDamage ) 
				{
					dwDamageFlag |= DAMAGE_TYPE_CA;
				}

				//	Note : 스킬 기본 영향.
				//
				SSKILLACT sSKILLACT;

				//sSKILLACT.sID = STARGETID(CROW_MOB, GetGaeaID());
				//sSKILLACT.sID_TAR = TargetID;
				sSKILLACT.sID = sCId;
				sSKILLACT.sID_TAR = sCTaId;
				sSKILLACT.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
				sSKILLACT.emAPPLY = pSkill->m_sBASIC.emAPPLY;
				sSKILLACT.sSkillID			= skill_id;
				sSKILLACT.wSkillLevel		= wSKILL_LVL;
				sSKILLACT.dwDamageFlag	= dwDamageFlag;

				//	Note : 체력 변화 저장.
				//
				sSKILLACT.nVAR_HP = nVAR_HP;
				sSKILLACT.nVAR_MP = nVAR_MP;
				sSKILLACT.nVAR_SP = nVAR_SP;

				//	Note : 체력 흡수 저장.
				//
				sSKILLACT.nGATHER_HP = nGATHER_HP;
				sSKILLACT.nGATHER_MP = nGATHER_MP;
				sSKILLACT.nGATHER_SP = nGATHER_SP;

                // 피격시 내구도 감소
                GLChar* pChar = m_pGaeaServer->GetChar( m_TargetID.GaeaId );
                if( pChar )
                {
                    // 공격 상대가 사람인지
                    if( m_emCrow == CROW_PC )
                        pChar->DurabilityDropHit( abs( nVAR_HP ), TRUE );
                    else
                        pChar->DurabilityDropHit( abs( nVAR_HP ), FALSE );
                }

				if ( sSKILLACT.VALID() )
					pLand->RegSkillAct ( sSKILLACT );
			}
		}

		//	Note : 스킬 보조 영향.
		//
		SSKILLACTEX sSKILLACTEX;
		SSKILLACTEX sCASKILLACTEX;
		sSKILLACTEX.sID = STARGETID(CROW_MOB,GetGaeaID());
		sSKILLACTEX.sID_TAR = TargetID;
		sSKILLACTEX.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;

		sSKILLACTEX.idSKILL = skill_id;
		sSKILLACTEX.wSKILL_LVL = wSKILL_LVL;

		sCASKILLACTEX = sSKILLACTEX;

		// 상태 이상 유발.
		ITEM::SSATE_BLOW sBLOW;
		//sBLOW.emTYPE = EMBLOW_NONE; // not needed

		const SKILL::SSTATE_BLOW &sSKILL_BLOW = pSkill->m_sAPPLY.sSTATE_BLOW[wSKILL_LVL];

		sBLOW.emTYPE = pSkill->m_sAPPLY.emSTATE_BLOW;
		sBLOW.fRATE = sSKILL_BLOW.fRATE;
		sBLOW.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL].fLIFE;
		sBLOW.fVAR1 = sSKILL_BLOW.fVAR1;
		sBLOW.fVAR2 = sSKILL_BLOW.fVAR2;
        
		if ( sBLOW.emTYPE!=EMBLOW_NONE )
		{
			//	Note : 발생 확율 계산.
			//
			short nBLOWRESIST = pTargetActor->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBLOW.emTYPE) );
			if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, GLPeriod::GetInstance().GetWeather(), pLand->IsWeatherActive() );
			DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );
			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, dwWeather, pLand->IsWeatherActive() );

			BOOL bBLOW(FALSE);
			if ( !(pTargetActor->GETHOLDBLOW()&STATE_TO_DISORDER(sBLOW.emTYPE)) )
			{
				bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBLOW.fRATE * fPOWER, GETLEVEL(), pTargetActor->GetLevel(), nBLOWRESIST );
			}

			if ( bBLOW )
			{
				//	Note : 상태이상 발생.
				//	
				SSTATEBLOW sSTATEBLOW;
				float fLIFE = sBLOW.fLIFE * fPOWER;
				fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );

				sSTATEBLOW.emBLOW = sBLOW.emTYPE;
				sSTATEBLOW.fAGE = fLIFE;
				sSTATEBLOW.fSTATE_VAR1 = sBLOW.fVAR1;
				sSTATEBLOW.fSTATE_VAR2 = sBLOW.fVAR2;
				
				if ( pTargetActor->STATEBLOW( sSTATEBLOW ) )
				{
					//	Note : 상태 이상 발생 저장.
					sSKILLACTEX.sSTATEBLOW = sSTATEBLOW;
				}
			}
		}

		//ApplySkillSspecial( pSkill,  sSKILLACTEX,  pTargetActor,  wSKILL_LVL);

		sCASKILLACTEX.sSTATEBLOW = sSKILLACTEX.sSTATEBLOW;

		ApplySkillSspecialCA( pSkill,  sSKILLACTEX,  sCASKILLACTEX, pTargetActor,  wSKILL_LVL);


		switch (pSkill->m_sAPPLY.emBASIC_TYPE)
		{
		case SKILL::EMFOR_CURE:
			//	Note : 상태 이상 치료.
			//
			sSKILLACTEX.dwCUREFLAG |= pSkill->m_sAPPLY.dwCUREFLAG;
			break;
		};

		if ( sSKILLACTEX.VALID() )
		{
			//pLand->RegSkillActEx ( sSKILLACTEX );
			pLand->RegSkillActExII ( sSKILLACTEX, sCASKILLACTEX );
		}
	}

	//	Note : 경험치 획득 산출.
	//
	int nSUM_EXP = 0;
	if ( nEXP_NUM!=0 )
	{
		//	Note : 회복에 기여했을 때만 경험치 발생.	( 공격시는 제외 )
		//
		if ( nALL_VARHP>0 && nALL_VARMP>0 && nALL_VARSP>0 ) // 항상 false 다. ???
		{
			nALL_LVL /= nEXP_NUM;

			nALL_VARHP = abs(nALL_VARHP) / nEXP_NUM;
			nALL_VARMP = abs(nALL_VARMP) / nEXP_NUM;
			nALL_VARSP = abs(nALL_VARSP) / nEXP_NUM;

			nALL_FULLHP /= nEXP_NUM;
			nALL_FULLMP /= nEXP_NUM;
			nALL_FULLSP /= nEXP_NUM;

			//	실제 변화량이 있을 때만 경험치를 산출.
			if ( nALL_VARHP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARHP,nALL_FULLHP);
			if ( nALL_VARMP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARMP,nALL_FULLMP);
			if ( nALL_VARSP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARSP,nALL_FULLSP);
		}
		//	Note : 보조 스킬 사용시 경험치.
		//
		else
		{
			nSUM_EXP = (pSkill->m_sBASIC.dwGRADE*(wSKILL_LVL+1)*100) / GETLEVEL();
			if ( nSUM_EXP < 1 )
                nSUM_EXP = 1;
			if ( nSUM_EXP > 5 )
                nSUM_EXP = 5;
		}
	}
}

BOOL GLCrow::SkillProcess ( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL );
	if ( pSkill == NULL )
		return FALSE;

	float fSkillRange = (float) ( GETBODYRADIUS() + GETSKILLRANGE_APPLY(*pSkill,m_wACTIVESKILL_LVL) + 2 );

	//	Note : 스킬이 적용되는 타겟 검사.
	//
	DWORD dwVALIDNUM(0);
	STARGETID TargetID;
	for ( WORD i = 0; i < m_wTARNUM; ++i )
	{
		const STARGETID& TargetID = m_sTARIDS[i];		
		ServerActor* const pACTOR = m_pGaeaServer->GetTarget(pLand, TargetID);
		if ( pACTOR == NULL )
			continue;

		float fReActionRange = (float) ( pACTOR->GetBodyRadius() + fSkillRange );

		D3DXVECTOR3 vDist = m_vPosition - pACTOR->GetPosition();
		float fDist = D3DXVec3Length(&vDist);

		if ( fReActionRange*GLCONST_CHAR::fREACT_VALID_SCALE < fDist )
			m_sTARIDS[i].dwID = GAEAID_NULL;
		else
			dwVALIDNUM++;
	}

	if ( dwVALIDNUM == 0 )
	{
		TurnAction ( GLAT_IDLE, false );
		return FALSE;
	}

	m_fattTIMER += fElapsedTime * GETATTVELO();

	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;
	int nTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	int nThisKey = int(sAniAttack.m_dwSTime) + int(m_fattTIMER*UNITANIKEY_PERSEC);

	ServerActor* const pTargetActor = m_pGaeaServer->GetTarget(pLand, m_TargetID);
	if ( pTargetActor == NULL )
	{ /* 위쪽에서 이미 타겟(들)에대 한 유효성 판정을 하고 있기 때문에,
		* pTargetActor이 NULL이어선 안됨;
		*/
		TurnAction ( GLAT_IDLE, false );
		return FALSE;
	}

	if ( !m_sHITARRAY.empty() )
	{
		SSTRIKE sStrike = *m_sHITARRAY.begin();

		if ( nThisKey >= sStrike.wDivKey )
		{
			if ( sStrike.bHit )
				SkillProc();
			else
				AvoidProc(m_idACTIVESKILL, m_wACTIVESKILL_LVL);

			//	큐 데이타가 처리된 경우 삭제한다.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
	//
	if ( nThisKey >= int(sAniAttack.m_dwETime) )
	{
		if ( !m_sHITARRAY.empty() )
		{
			while ( !m_sHITARRAY.empty() )
			{
				SSTRIKE sStrike = *m_sHITARRAY.begin();

				if ( sStrike.bHit )
					SkillProc ();
				else
					AvoidProc(m_idACTIVESKILL, m_wACTIVESKILL_LVL);

				//	큐 데이타가 처리된 경우 삭제한다.
				m_sHITARRAY.pop_front ();
			}
		}

		return FALSE;
	}

	return TRUE;
}

bool GLCrow::STATEBLOW ( const SSTATEBLOW &sStateBlow )
{
	if ( !m_pCrowData )
	{
		return false;
	}

	// if ( m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_STATE )
	if ( IsNoAllPenalty( m_pCrowData->m_dwNoPenaltyFlag ) )
	{
		return false;
	}

	int nIndex = 0;

	if ( sStateBlow.emBLOW <= EMBLOW_SINGLE )
		nIndex = 0;
	else
		nIndex = sStateBlow.emBLOW-EMBLOW_SINGLE;

	m_sSTATEBLOWS[nIndex] = sStateBlow;

	//	상태이상 ( 기절 ) 을 받을때 현제 행위 중단.
	if ( sStateBlow.emBLOW == EMBLOW_STUN )
	{
		TurnAction ( GLAT_IDLE );//, false );
	}

	return true;
}

void GLCrow::CURE_STATEBLOW ( DWORD dwCUREFLAG )
{
	BOOL bChanged = FALSE;

	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		if ( m_sSTATEBLOWS[i].emBLOW != EMBLOW_NONE )
			continue;

		bChanged = TRUE;
		if ( STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW) & dwCUREFLAG )
			m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
	}

	if ( !bChanged )	return;

	//	Note : 상태 이상 변화 MsgFB.
	//
	GLMSG::SNETPC_CURESTATEBLOW_BRD NetMsgBRD;
	NetMsgBRD.dwCUREFLAG = dwCUREFLAG;

	// 주변 클라이언트들에게 메세지 전송.
	SendMsgViewAround(&NetMsgBRD);
}

void GLCrow::SKILL_EFFECT_REMOVE ( DWORD dwRemoveFlag )
{
    if ( dwRemoveFlag >= SKILL::EMACTION_TYPE_NSIZE )
        return;

    //일반적인 스킬의 효과만 제거한다. (몹의 경우는 도시락,시스템 버프가 없다.);
    for ( size_t i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
    {
        if ( i >= CROW_SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() )
            continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
        if ( !pSkill )
            continue;

        //제거 불가 옵션;
        if ( pSkill->m_sBASIC.bNonEffectRemove )
            continue;

        //해당하는 스킬타입인지?
        if ( pSkill->m_sBASIC.emACTION_TYPE != (SKILL::EMACTION_TYPE)dwRemoveFlag )
            continue;

		UPDATE_DATA_END(i);
    }
}

void GLCrow::DO_STUN_ACTION ()
{
	if ( IsNoAllPenalty() )
		return;

	TurnAction ( GLAT_IDLE );
}

void GLCrow::VAR_BODY_POINT( const EMCROW emActorCrow, const DWORD dwActorID, const DWORD dwVarTypeFlag, int& nVar_HP, int& nVar_MP, int& nVar_SP )
{
    if ( m_dwNowHP == 0 )
        return;

    if ( nVar_HP < 0 )
        nVar_HP = -(int)ReceiveDamage(emActorCrow, dwActorID, abs(nVar_HP), dwVarTypeFlag);  // 주의: -부호에 주의해야함;
    else if ( nVar_HP > 0 )
        nVar_HP = ReceiveHeal(emActorCrow, dwActorID, nVar_HP, dwVarTypeFlag);

    GLOGICEX::VARIATION ( m_wNowMP, GETMAXMP(), nVar_MP );
    GLOGICEX::VARIATION ( m_wNowSP, GETMAXSP(), nVar_SP );
}

//! 아이템 우선권을 주기위한 데미지 로그 기록
/*
void GLCrow::VAR_BODY_POINT (const EMCROW emACrow,
							 const DWORD dwAID,
							 const BOOL bPartySkill,
							 int nvar_hp,
							 int nvar_mp,
							 int nvar_sp)
{
	if ( m_dwNowHP==0 )		return;

	DWORD dwDxHP = GLOGICEX::VARIATION ( m_dwNowHP, GETMAXHP(), nvar_hp );
	GLOGICEX::VARIATION ( m_wNowMP, GETMAXMP(), nvar_mp );
	GLOGICEX::VARIATION ( m_wNowSP, GETMAXSP(), nvar_sp );

	BOOL bDamage = nvar_hp<0;

	if ( bDamage )
	{
		//	자신을 공격한 사람 등록.
		m_sAssault.emCrow = emACrow;
		m_sAssault.dwID = dwAID;

		//	데미지 기록.
		if ( emACrow == CROW_PC )
		{
			PGLCHAR pChar = m_pGLGaeaServer->GetChar ( dwAID );
			if ( pChar )
			{				
				// 자신이 준 데미지 이지만... 파티에 가입되어 있다면...
				if (pChar->m_dwPartyID != PARTY_NULL)
				{
					GLPARTY_FIELD *pParty = m_pGLGaeaServer->GetParty(pChar->m_dwPartyID);
					if (pParty != NULL)
					{
						pParty->m_dwMASTER; // 마스터의 ID
						m_pGLGaeaServer->GetChar(
					}
				}
				AddDamageLog ( m_cDamageLog, dwAID, pChar->m_dwUserID, dwDxHP );
			}
		}
	}

	HRESULT hrESCAPE = S_OK;
	EMCROWACT_UP emCROWACT = GetActPattern();
	int nDamageHP = abs(nvar_hp);
	if ( bDamage )		hrESCAPE = ESCAPE ( STARGETID ( emACrow, dwAID ), nDamageHP );
	
	if ( EMCROWACT_UP_ESCAPE != emCROWACT || hrESCAPE!=S_OK )
	{
		//	Note :타겟이 존제하지 않을때 공격자가 있으면 타겟으로 등록 시도.
		//
		if ( !IsFLAG(EMTARGET) && bDamage )
		{
			PGLCHAR pTarget = NULL;
			if ( emACrow==CROW_PC )		pTarget = m_pGLGaeaServer->GetChar(dwAID);
			
			if ( pTarget )
			{
				STARGETID sTARID(CROW_PC,pTarget->GetGaeaID(),pTarget->GetPosition());
				NewTarget ( sTARID );
			}
		}
	}
}
*/

DWORD GLCrow::SELECT_SKILLSLOT ( SNATIVEID skill_id, EMSKILLFACT_TYPE emSkillFactType )
{
	DWORD dwSELECT = UINT_MAX;

	//	Note : 타입에 따라 스킬이 들어갈 슬롯 선택.
	//
	switch ( emSkillFactType )
	{
	// Note : 일반 스킬
	case EMSKILLFACT_TYPE_NORMAL:
	case EMSKILLFACT_TYPE_FORCED_NORMAL:
		{
			for ( DWORD i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			{
                if (i >= CROW_SKILLFACT_SIZE)
                    continue;

				if ( m_sSKILLFACT[i].sNATIVEID==skill_id )
				{
					return dwSELECT = i;
				}
			}

			float fAGE = FLT_MAX;
			for ( DWORD i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			{
                if (i >= CROW_SKILLFACT_SIZE)
                    continue;

				if ( m_sSKILLFACT[i].sNATIVEID==NATIVEID_NULL() )
				{
					return dwSELECT = i;
				}

				if ( m_sSKILLFACT[i].fAGE < fAGE )
				{
					fAGE    = m_sSKILLFACT[i].fAGE;
					dwSELECT = i;
				}
			}
		}
		break;
	}

	return dwSELECT;
}

static void AddSkillFact_SpecialAddon_Rate_Crow( const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF, const DWORD dwAdditionFlag = NULL )
{
	SKILL::SSPEC_ADDON_LEVEL	sAddon;

	sAddon.emSPEC			= emAddon;	
	sAddon.sSPEC.fVAR1		= sSPEC.fVAR1 * sSPEC.fRate;
	sAddon.sSPEC.fVAR2		= sSPEC.fVAR2 * sSPEC.fRate2;
	sAddon.sSPEC.fRate		= sSPEC.fRate;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG | dwAdditionFlag;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );
} //AddSkillFact_SpecialAddon

static void AddSkillFact_SpecialAddon_Crow( const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF, const DWORD dwAdditionFlag = NULL)
{
	SKILL::SSPEC_ADDON_LEVEL	sAddon;

	sAddon.emSPEC			= emAddon;
	sAddon.sSPEC.fVAR1		= sSPEC.fVAR1;
	sAddon.sSPEC.fVAR2		= sSPEC.fVAR2;
	sAddon.sSPEC.fRate		= sSPEC.fRate;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG | dwAdditionFlag;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );
} //AddSkillFact_SpecialAddon

const bool GLCrow::RECEIVE_SKILLFACT_Stigma(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel)
{
	if ( sID.emCrow == CROW_PC	)
	{
		GLChar* _pChar = m_pGaeaServer->GetChar(sID.GaeaId);
		if ( !_pChar )
			return false;

		_pChar->m_sSTIGMA_USER.PushStigmaPack(sID, STARGETID(CROW_MOB, m_dwGaeaID), sSkillID, sSPEC);
	}
	return true;
}

const bool GLCrow::RECEIVE_SKILLFACT_Taunt( const STARGETID& sID )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

    // 몹과 몹은 싸우지 않는다.;
    // 그룹리더는 공격대상을 변경하지 않는다.;
    // (원래 그룹몹은 리더와 같은 대상을 때리게 되어있는데 이걸로 인해서 그 방식이 풀리는게 기획적인 문제를 일으킬 수 있음.);
    if ( sID.emCrow == CROW_MOB || IsGroupLeader() )
        return false;

    // 불리한 효과 방지인 경우도 도발에 걸리지 않음;
    if ( IsNoAllPenalty() )
        return false;

    ServerActor* pTarget = m_pGaeaServer->GetTarget ( pLand, sID );
    if ( !pTarget )
        return false;

    //	Note : 타겟으로 지정.
    m_TargetID.emCrow = sID.emCrow;
    m_TargetID.GaeaId = pTarget->GetGaeaID();
    m_TargetID.vPos = pTarget->GetPosition();

    SetFLAG(EMTARGET);

    return true;
}

const bool GLCrow::RECEIVE_SKILLFACT_Domination(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel)
{
	if ( EMSPEC_DOMINATE_TYPE(sSPEC.dwFLAG) == EMSPEC_DOMINATE_TYPE_CASTER )
	{
		if ( sID.emCrow == CROW_PC	)
		{
			GLChar* _pChar = m_pGaeaServer->GetChar(sID.GaeaId);
			if ( !_pChar )
				return false;

			GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
			NetMsg.sSkillID = sSkillID;
			NetMsg.wLevel = wLevel;
			NetMsg.sTargetID = STARGETID(GETCROW(), m_dwGaeaID);		

			_pChar->SendToClient( &NetMsg );
		}		
	}
	return true;
}

const bool GLCrow::RECEIVE_SKILLFACT_Special( PGLSKILL pSkill, const SNATIVEID& sSkillID, const WORD wlevel, const DWORD dwCount, SSKILLFACT& sSKILLEF, const STARGETID sID ){
	bool bHold = false;
	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON&		emAddon = pSkill->m_sAPPLY.vecSPEC[i].emSPEC;
		const SKILL::SSPEC&		sSPEC = pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wlevel];		

		if ( RF_IMMUNE( m_EffSkillVarSet ).IsOn() )
		{
			if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( static_cast< BYTE >( emAddon ) ) )
			{
				continue;
			}
		}

		switch ( emAddon )
		{
		case EMSPECA_COUNTATTACK :
			{
				bHold = TRUE;
				sSKILLEF.pcCAData = pSkill->m_sAPPLY.sCAData.sSPEC_CA[wlevel];
				AddSkillFact_SpecialAddon_Crow( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_IGNORED_DAMAGE:
			bHold = true;
			GLCROWLOGIC::SET_IGNORED_DAMAGE(WORD(sSPEC.fVAR1), WORD(sSPEC.fVAR2));			
			AddSkillFact_SpecialAddon_Rate_Crow(emAddon, sSPEC, sSKILLEF);
			break;
        case EMSPECA_LINK_HP:
            {
				bHold = true;
				DWORD fLinkHP = static_cast<DWORD>(GetMaxHP() * m_sLINKHP.fRef);
				if ( m_dwNowHP > fLinkHP )
					m_dwNowHP = fLinkHP;

                SKILL::SSPEC_ADDON_LEVEL	sAddon;
                sAddon.emSPEC			= emAddon;
                sAddon.sSPEC.fVAR1		= m_sLINKHP.fRef;
                sAddon.sSPEC.fVAR2		= sSPEC.fVAR2;
                sAddon.sSPEC.fRate		= sSPEC.fRate;	
                sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	
                sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
                sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

                sSKILLEF.vecSPEC.push_back( sAddon );
            }
            break;
		case EMSPECA_DOMINATION:			
			if ( RECEIVE_SKILLFACT_Domination(sID, sSPEC, sSkillID, wlevel) )
			{
				bHold = true;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;

		case EMSPECA_ILLUSION:
			bHold = true;
			m_idIllusionSKILL = sSkillID;
			m_sDamageSpec.m_fIllusionJumpBound	= ((float)sSPEC.nVAR1) * 10.0f;
			m_sDamageSpec.m_wIllusionRemain		= sSPEC.nVAR2;
			AddSkillFact_SpecialAddon_Crow( emAddon, sSPEC, sSKILLEF );
			break; 
		case EMSPECA_STIGMA:
			bHold = true;
			RECEIVE_SKILLFACT_Stigma(sID, sSPEC, sSkillID, wlevel);
			AddSkillFact_SpecialAddon_Crow( emAddon, sSPEC, sSKILLEF );	
			break;
		case EMSPECA_TAUNT:
			bHold = true;
			AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			RECEIVE_SKILLFACT_Taunt( sID );
			break;
		case EMSPECA_ENTRANCE_EXIT :
			{
				if( RECEIVE_SKILLFACT_EntranceExit( sSkillID, sSPEC, wlevel, sID ) )
				{
					RF_ENTRANCE_EXIT( m_EffSkillVarSet ).TurnOn();

					bHold = true;
					AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
				}
			}
			break;
		case EMSPECA_PROVOKE :
			{
				if( RECEIVE_SKILLFACT_Provoke( sSkillID, sSPEC, wlevel, sID ) )
				{
					bHold = true;
					AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
				}				
			}
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			{
				RF_ACTIVE( m_EffSkillVarSet ).UpdateSkill( sSkillID.dwID, wlevel, sSPEC.nVAR1, sSPEC.nVAR2 );

				bHold = true;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHold = true;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;		
		case EMSPECA_REFDAMAGE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_SKILLDELAY:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_CURSE:
			bHold = true;
			AddSkillFact_SpecialAddon_Rate_Crow(emAddon, sSPEC, sSKILLEF);
			break;		
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_IMMUNE:		
		case EMSPECA_RELEASE_ENDURE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_SKILL_LINK:
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_VALUE	:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		case EMSPECA_CP_VALUE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_AIRBORNE:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
		case EMSPECA_FEIGN_DEATH :
            bHold = true;
            AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
            break;		
		};
	}

	return bHold;
}

const bool GLCrow::RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF ){
	const size_t nADDON = pSkill->m_sAPPLY.vecADDON.size();

	for ( DWORD i = 0; i < nADDON; ++i )
	{
		SKILL::SIMPACT_ADDON_LEVEL sAddon;

		sAddon.emADDON = pSkill->m_sAPPLY.vecADDON[i].emADDON;
		float fADDON_VAR = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel] * pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
		sAddon.fADDON_VAR = fADDON_VAR;
		sAddon.fRate = pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	return nADDON ? true : false;
}

const bool GLCrow::RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF ){
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:		
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fBASIC_VAR;
		return true;
		break;
	};
	return false;
}

const BOOL GLCrow::RECEIVE_SKILLCA( const SSKILLACTEX &sACTEX )
{
	STARGETID sTar( GetCrow(), GetGaeaID() );

	if( (sTar != sACTEX.sID) || (sACTEX.sID == sACTEX.sID_TAR) ) return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sACTEX.idSKILL.wMainID, sACTEX.idSKILL.wSubID );
	if ( !pSkill )
		return FALSE;

	SSKILLFACT		sSKILLEF;
	BOOL bHold = FALSE;

	SSKILLACTEX			sSKILLACTEX;

	sSKILLACTEX.sID			= sACTEX.sID_TAR;//STARGETID(CROW_PC,m_dwGaeaID);
	sSKILLACTEX.fDELAY		= pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
	sSKILLACTEX.idSKILL		= sACTEX.idSKILL;
	sSKILLACTEX.wSKILL_LVL	= sACTEX.wSKILL_LVL;
	sSKILLACTEX.sID_TAR		= sACTEX.sID;

	bool bAllApply = false;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;

	ServerActor* const pTargetActor = GetTarget(sACTEX.sID_TAR);

	for( int i = 0; i < m_SkillCAIndex.m_Apply.size(); ++i)
	{		
		SKILL::SSPEC_ADDON* pTemp = m_SkillCAIndex.m_Apply[i];

		if( pTemp != NULL )
			tempSkill.m_sAPPLY.vecSPEC.push_back( *pTemp );
	}

	for( int i = 0; i < m_SkillCAIndex.m_Pact.size(); ++i)
	{
		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( m_SkillCAIndex.m_Pact[i]->emADDON ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}
	}

	if( m_SkillCAIndex.IsSimPacAddon() )
	{
		bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, sACTEX.wSKILL_LVL, sSKILLEF );
	}

	if( m_SkillCAIndex.IsSpecddon() )
		bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, sACTEX.idSKILL, sACTEX.wSKILL_LVL, _SKILLFACT_DEFAULTCUSTOMCOUNT, sSKILLEF, sACTEX.sID );

	if( pTargetActor ) {
		switch( pTargetActor->GetCrow() )
		{
		case CROW_PC :
			{
				GLChar* pChar = dynamic_cast<GLChar*>(pTargetActor);

				const SKILL::SSPEC_CA* pCA = pChar->GetSkillCA()->GetCaData();					

				if( pCA && pCA->bAllApply )
					bAllApply = true;
			}
			break;
		case CROW_MOB :
			{
				GLCrow*  pChar = dynamic_cast<GLCrow*>(pTargetActor);

				const SKILL::SSPEC_CA* pCA = pChar->GetSkillCA()->GetCaData();					

				if( pCA && pCA->bAllApply )
					bAllApply = true;				
			}
			break;
		}
	}

	if( bHold )		// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	{
		const SSKILLFACT::STIME	sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME);

		sSKILLEF.sNATIVEID	= sACTEX.idSKILL;
		sSKILLEF.wLEVEL		= sACTEX.wSKILL_LVL;
		sSKILLEF.sID		= sACTEX.sID_TAR;
		sSKILLEF.bEnable    = TRUE;

		if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	
			sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[sSKILLEF.wLEVEL].fLIFE;
		else 
			sSKILLEF.fAGE = sTime.fAGE;
		
		if ( !sSKILLEF.IsAlive() )
			return FALSE;

		DWORD dwSELECT = SELECT_SKILLSLOT ( sACTEX.idSKILL, EMSKILLFACT_TYPE_NORMAL );

		if (dwSELECT < CROW_SKILLFACT_SIZE)
			m_sSKILLFACT[dwSELECT] = sSKILLEF;

		if( dwSELECT != UINT_MAX )
		{
			GLMSG::SNETPC_SKILLHOLD_BRD		NetMsgBRD;
			NetMsgBRD.emCrow		 = CROW_MOB;
			NetMsgBRD.dwID			 = GetGaeaID();
			NetMsgBRD.skill_id		 = sACTEX.idSKILL;
			NetMsgBRD.wLEVEL		 = sACTEX.wSKILL_LVL;
			NetMsgBRD.wSELSLOT		 = static_cast<WORD>(dwSELECT);
			NetMsgBRD.fPARAM		 = GetMaxHpRate();
			//NetMsgBRD.sID            = sACTEX.sID;
			NetMsgBRD.sID            = sACTEX.sID_TAR;
			NetMsgBRD.sEffInfo.eType = GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_ATTACK;

			GetEffList( dwSELECT, NetMsgBRD.sEffInfo.iRecvEAIdx, NetMsgBRD.sEffInfo.iRecvSAIdx );

			m_pGaeaServer->SENDTOCLIENT ( GETCLIENTID(), &NetMsgBRD );

			SendMsgViewAround(&NetMsgBRD);			
		}
	}

	for ( DWORD i = 0 ; i < SKILL::EMACTION_TYPE_NSIZE; ++i )
	{
		if ( sSKILLACTEX.dwRemoveFlag[i] == true && IsSkillEnd(sACTEX.idSKILL))
			SKILL_EFFECT_REMOVE ( i );
	}

	if( bAllApply )
	{		
		if( sACTEX.sSTATEBLOW.emBLOW!=EMBLOW_NONE && STATEBLOW ( sACTEX.sSTATEBLOW ))
		{
			ServerActor* pACTOR = GetTarget ( sACTEX.sID_TAR );
			// 상태이상 발생 Msg.
			GLMSG::SNETPC_STATEBLOW_BRD		NetMsgState;
			NetMsgState.emCrow		= sACTEX.sID.emCrow;
			NetMsgState.dwID		= sACTEX.sID.GaeaId;
			NetMsgState.emBLOW		= sACTEX.sSTATEBLOW.emBLOW;
			NetMsgState.fAGE		= sACTEX.sSTATEBLOW.fAGE;
			NetMsgState.fSTATE_VAR1 = sACTEX.sSTATEBLOW.fSTATE_VAR1;
			NetMsgState.fSTATE_VAR2 = sACTEX.sSTATEBLOW.fSTATE_VAR2;

			if( pACTOR )
				NetMsgState.wSchool = pACTOR->GetSchool();

			// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
			//
			SendMsgViewAround(&NetMsgState);

			// 스킬 대상에게 메세지 전송.
			if ( sACTEX.sID_TAR.emCrow == CROW_PC )
				m_pGaeaServer->SENDTOCLIENT( GETCLIENTID (), &NetMsgState );

			if( pTargetActor ) {
				switch( pTargetActor->GetCrow() )
				{
				case CROW_PC :
					{
						GLChar* pChar = dynamic_cast<GLChar*>(pTargetActor);

						pChar->GetSkillCA()->SetUseCA( true );
					}
					break;
				case CROW_MOB :
					{
						GLCrow*  pChar = dynamic_cast<GLCrow*>(pTargetActor);

						pChar->GetSkillCA()->SetUseCA( true );

					}
					break;
				}
			}
		} 
	}

	return TRUE;
}

const BOOL GLCrow::RECEIVE_SKILLFACT(
    const SNATIVEID& skill_id,
    const WORD wlevel,
    DWORD &dwSELECT,
    const STARGETID& sID,
    const SSKILLFACT::STIME sTime,
    const DWORD dwCount,
    EMSKILLFACT_TYPE emSkillFactType,
	const float* const pLaunchTime, const float* const pLaunchValue)
{
	// Note : Normal 타입이 아니면 튕긴다.
	if ( ( emSkillFactType != EMSKILLFACT_TYPE_NORMAL ) &&
		( emSkillFactType != EMSKILLFACT_TYPE_FORCED_NORMAL ) )
	{
		return FALSE;
	}

	// Note : 슬롯 최대 사이즈로 초기화한다.
	dwSELECT = UINT_MAX;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if (!pSkill)
        return FALSE;	
	
	SSKILLFACT sSKILLEF;
	bool bHold = false;

	bHold |= RECEIVE_SKILLFACT_Basic(   pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon(   pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( pSkill, skill_id, wlevel, dwCount, sSKILLEF, sID );
	

	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//		   현재 불리한 효과를 적용불가 기능이 체크되어 있으면 효과가 적용되지 않는다.
	if ( bHold )
	{
		if ( ( IsNoAllPenalty( pSkill->m_sBASIC.emACTION_TYPE ) == TRUE ) &&
			emSkillFactType == EMSKILLFACT_TYPE_NORMAL )
			goto NOPENALTY;

		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.sID		= sID;
		sSKILLEF.bEnable    = TRUE;

        if ( sTime.IsEXPIREDTIME )
        {
            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
        }
        else
        {
            if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE;
            else									    sSKILLEF.fAGE = sTime.fAGE;
        }

        if ( !sSKILLEF.IsAlive() )
        {
            return FALSE;
        }

		dwSELECT = SELECT_SKILLSLOT ( skill_id, emSkillFactType );
        if (dwSELECT < CROW_SKILLFACT_SIZE)
		    m_sSKILLFACT[dwSELECT] = sSKILLEF;
	}

NOPENALTY:

	return TRUE;
}

const BOOL GLCrow::RECEIVE_SKILLFACTII(
									 const SNATIVEID& skill_id,
									 const WORD wlevel,
									 DWORD &dwSELECT,
									 const STARGETID& sID,
									 const SSKILLFACT::STIME sTime,
									 const DWORD dwCount,
									 EMSKILLFACT_TYPE emSkillFactType,
									 const float* const pLaunchTime, const float* const pLaunchValue)
{
	// Note : Normal 타입이 아니면 튕긴다.
	if ( emSkillFactType != EMSKILLFACT_TYPE_NORMAL )
	{
		return FALSE;
	}

	// Note : 슬롯 최대 사이즈로 초기화한다.
	dwSELECT = UINT_MAX;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if (!pSkill)
		return FALSE;	

	SSKILLFACT sSKILLEF;
	bool bHold = false;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();
	
	for( int i = 0; i < m_SkillCAIndex.m_Apply.size(); ++i)
	{		
		SKILL::SSPEC_ADDON* pTemp = m_SkillCAIndex.m_Apply[i];

		if( pTemp != NULL )
			tempSkill.m_sAPPLY.vecSPEC.push_back( *pTemp );
	}

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;

	for( int i = 0; i < m_SkillCAIndex.m_Pact.size(); ++i)
	{
		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( m_SkillCAIndex.m_Pact[i]->emADDON ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}
	}

	bHold |= RECEIVE_SKILLFACT_Basic(   &tempSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon(   &tempSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, skill_id, wlevel, dwCount, sSKILLEF, sID );


	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//		   현재 불리한 효과를 적용불가 기능이 체크되어 있으면 효과가 적용되지 않는다.
	if ( bHold )
	{
		if ( IsNoAllPenalty( pSkill->m_sBASIC.emACTION_TYPE ) == TRUE )
			goto NOPENALTY;

		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.sID		= sID;
		sSKILLEF.bEnable    = TRUE;

		if ( sTime.IsEXPIREDTIME )
		{
			sSKILLEF.SetAgeInfinite();
			sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
		}
		else
		{
			if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE;
			else									    sSKILLEF.fAGE = sTime.fAGE;
		}

		if ( !sSKILLEF.IsAlive() )
		{
			return FALSE;
		}

		dwSELECT = SELECT_SKILLSLOT ( skill_id, emSkillFactType );
		if (dwSELECT < CROW_SKILLFACT_SIZE)
			m_sSKILLFACT[dwSELECT] = sSKILLEF;
	}

NOPENALTY:

	return TRUE;
}

const BOOL GLCrow::REMOVE_SKILLFACT(const SNATIVEID& skill_id)
{
    GLCROWLOGIC::RemoveSkillFactBySkillID(skill_id);
    return true;
}

const BOOL GLCrow::REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON)
{
    GLCROWLOGIC::RemoveSkillFactBySpecial(emSPEC_ADDON);
    return true;
}

/**
 * 지속 효과가 시간 만료 및 무기 교체, 해제 등에 의해 종료된 경우 호출 되어야 함; 
 * 시간 만료의 경우 클라쪽에서도 처리 하고 있으므로 따로 메세지를 보내지 않아도 좋지만,
 * 기타 조건에 의해 만료되는 경우 메세지를 보내주어야 함;
 * 기존 란 온라인의 지속효과 로직의 경우 중간 변화에 대해 유연하지 못함;
 *
 * @ Param dwSkillfactIndex		[in] 만료된 지속효과의 인덱스;
**/
void GLCrow::UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel )
{
	SSKILLFACT& sSKILLFACT = m_sSKILLFACT[dwSkillfactIndex];
	if ( sSKILLFACT.sNATIVEID==NATIVEID_NULL() )
		return;
	if ( !sSKILLFACT.bEnable )
		return;

	const size_t nSize = sSKILLFACT.vecSPEC.size();	
	
	GLMSG::SNETCROW_END_SKILL NetMsgBrd(m_dwGaeaID, dwSkillfactIndex);
	for ( unsigned int _i = 0; _i < nSize; ++_i )
	{ // 특별히 필요한 작업이 있다면 주석 해제 후, 이곳에 한다;
		const SKILL::SSPEC_ADDON_LEVEL& spec = sSKILLFACT.vecSPEC[_i];
		switch ( spec.emSPEC )
		{
		case EMSPECA_COUNTATTACK :
			{	
				if( bTimeDel )
				{
					SelfSkillStart( SNATIVEID(spec.sSPEC.fVAR1,spec.sSPEC.fVAR2), sSKILLFACT.wLEVEL );
				}

				m_SkillCAIndex.AllReset();
			}
			break; 
		case EMSPECA_IGNORED_DAMAGE:
			m_sIgnoredDamage.RESET();	
			break;
		case EMSPECA_DELAY_ACTION_SKILL:
			DelayActionSkillStart( sSKILLFACT.sID, SNATIVEID(spec.sSPEC.fVAR1,spec.sSPEC.fVAR2), sSKILLFACT.wLEVEL );
			break;
		case EMSPECA_PROVOKE :
			RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
			RF_ACTIVE( m_EffSkillVarSet ).DeleteSkill( sSKILLFACT.sNATIVEID.dwID );
			break;
 		}
 	}
	sSKILLFACT.RESET();
	
	SendMsgViewAround(&NetMsgBrd);
}

/*
 * 인술부가 추가 되면서 거리에 의해 해제되는 조건이 생김;
 * 기존 구조가 GLogicPC/NPC 와 GLActor 양쪽을 상속 받는 구조로 되어 있어,
 * GLogicPC/NPC 쪽에서는 위치 값을 참조 할 수가 없음;
 * 해서 아래 UPDATE_DATA_Additional() 함수를 통해 -
 * GLogicPC/NPC단에서 처리 할 수 없는 작업을 하도록 함;
 * 아래 함수의 구조를 올바르다 할 수 없음;
 * 허나 선결 조건으로 GLogicPC/NPC와 GLActor에 대한 정리가 이루어져야 함;
*/
void GLCrow::UPDATE_DATA_Additional(const float fElapsedTime)
{	
	// 일단 여기서..
	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID();

		if( !sID.ISNULL() )
		{
			if( IsViewInActor( sID ) ) {
				NewTarget( sID, TRUE );
			}

			RF_PROVOKE( m_EffSkillVarSet ).ResetPreTarget();
		}
	}

	DWORD dwStigmaIndex = 0;
	for ( int _i = 0; _i < CROW_SKILLFACT_SIZE; ++_i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[_i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	
			continue;
		if ( !sSKEFF.bEnable )
			continue;
		
		for ( DWORD j = 0; j < sSKEFF.vecSPEC.size(); ++j )
		{
			SKILL::SSPEC& sSPEC = sSKEFF.vecSPEC[j].sSPEC;
			switch ( sSKEFF.vecSPEC[j].emSPEC )
			{
			case EMSPECA_STIGMA:
				GLCrow::UPDATE_DATA_Additional_Stigma(sSKEFF, dwStigmaIndex, _i);
				++dwStigmaIndex;
				break;
			
			case EMSPECA_DOMINATION:
				GLCrow::UPDATE_DATA_Additional_Domination(sSPEC, sSKEFF, _i);				
				break;
			case EMSPECA_PROVOKE :
				GLCrow::UPDATE_DATA_Additional_Provoke(sSPEC, sSKEFF, _i);
				break;
			}
		}
	}
}

void GLCrow::UPDATE_DATA_Additional_Domination(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex)
{
	if ( m_sDOMINATION.emController == EMSPEC_DOMINATE_TYPE_CASTER )
	{
		ServerActor* pACTOR = GetTarget(sSKEFF.sID);
		if ( !pACTOR )	
			UPDATE_DATA_END(dwSkillfactIndex);
		else
		{
			const D3DXVECTOR3& vDistanceVector = pACTOR->GetPosition() - GetPosition();
			const float fDistance = ::D3DXVec3Length(&vDistanceVector);
			if ( DWORD(sSPEC.fVAR2) && (fDistance > m_sDOMINATION.fRadiusMoveable) )
			{
				UPDATE_DATA_END(dwSkillfactIndex);
				if ( pACTOR->GetCrow() == CROW_PC )
				{						
					GLChar* _pChar = m_pGaeaServer->GetChar(pACTOR->GetGaeaID());						
					GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
					NetMsg.emFB = EMDOMINATE_MOVETO_FB_OUT_OF_DISTANCE;
					_pChar->m_sDOMINATE.bDominate = false;
					_pChar->SendToClient( &NetMsg );
				}				
			}
		}
	}	
}

void GLCrow::UPDATE_DATA_Additional_Stigma(SSKILLFACT& sSKEFF, const DWORD dwStigmaIndex, const DWORD dwSkillfactIndex)
{
	const SSTIGMA::SSTIGMA_PACK* pSSTIGMA_PACK = m_sSTIGMA_TAKER.GetStigmaPack(dwStigmaIndex);
	ServerActor* pACTOR = GetTarget(pSSTIGMA_PACK->sUserID);
	if ( !pACTOR )
		UPDATE_DATA_END(dwSkillfactIndex);
	else
	{
		const D3DXVECTOR3& vDistanceVector = pACTOR->GetPosition() - GetPosition();
		const DWORD dwDistance = DWORD(::D3DXVec3Length(&vDistanceVector));
		if ( pSSTIGMA_PACK->dwAsignDistance && (dwDistance > pSSTIGMA_PACK->dwAsignDistance) )
			UPDATE_DATA_END(dwSkillfactIndex);
	}
}

void GLCrow::UPDATE_DATA_Additional_Provoke(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex)
{
	ServerActor* pACTOR = GetTarget( sSKEFF.sID );
	if ( !pACTOR )
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}

	SCONFTING* pConfting = pACTOR->GETCONFTING();
	if ( ( pACTOR->IsValidBody() == FALSE ) || ( pConfting && pConfting->IsPOWERFULTIME() ) )   //플레이어 사망,대련종료, 맵이동 등 상황에서 도발 해제;
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}

	GLLandMan* const pLand(GetLandMan());
	if ( pLand == NULL )
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}

	WORD wDistance = (WORD)sSPEC.fVAR1;

	if ( wDistance == 0xFFFF )	wDistance = 0;

	wDistance = wDistance*10;

	if( !pLand->IsAbleAttack_Range_LayerSimpleCollision( wDistance, pACTOR->GetPosition(), GetPosition(), 5.f ) )
		UPDATE_DATA_END( dwSkillfactIndex );
}

void GLCrow::ApplySkillSspecialCA(PGLSKILL pSkill, SSKILLACTEX &sSKILLACTEX, SSKILLACTEX &sCASKILLACTEX, ServerActor* const pTargetActor, WORD wSKILL_LVL)
{
	//if( pTarget->GetCrow() == CROW_PC )
	{
		CSkillCAIdx* pCASkill = NULL;

		switch( pTargetActor->GetCrow() )
		{
		case CROW_PC :
			{
				GLChar* pTarChar = dynamic_cast<GLChar*>(pTargetActor);

				pCASkill = pTarChar->GetSkillCA();
			}
			break;
		case CROW_MOB :
			{
				GLCrow* pTarChar = dynamic_cast<GLCrow*>(pTargetActor);

				pCASkill = pTarChar->GetSkillCA();
			}
			break;
		default :
			{
				ApplySkillSspecial( pSkill, sSKILLACTEX, pTargetActor, wSKILL_LVL  );
			}
			return;
		}

		GLSKILL tempSkill = *pSkill;

		std::vector<SKILL::SSPEC_ADDON>::iterator   _viter;

		tempSkill.m_sAPPLY.vecSPEC.clear();

		for( int i = 0; i < pCASkill->m_Apply.size(); ++i)
		{
			_viter = std::find_if( pSkill->m_sAPPLY.vecSPEC.begin(), pSkill->m_sAPPLY.vecSPEC.end(), SKILL::SSPEC_ADDON( pCASkill->m_Apply[i]->emSPEC) );

			if( _viter != pSkill->m_sAPPLY.vecSPEC.end() )
			{
				tempSkill.m_sAPPLY.vecSPEC.push_back( *_viter );
			}
		}

		ApplySkillSspecial(&tempSkill, sSKILLACTEX, pTargetActor, wSKILL_LVL );

		if( GetGaeaID() == pTargetActor->GetGaeaID() ) return;

		tempSkill.m_sAPPLY.vecSPEC.clear();

		for( int i = 0; i < m_SkillCAIndex.m_Apply.size(); ++i)
		{
			_viter = std::find_if( pSkill->m_sAPPLY.vecSPEC.begin(), pSkill->m_sAPPLY.vecSPEC.end(), SKILL::SSPEC_ADDON( m_SkillCAIndex.m_Apply[i]->emSPEC) );

			if( _viter != pSkill->m_sAPPLY.vecSPEC.end() )
			{
				tempSkill.m_sAPPLY.vecSPEC.push_back( *_viter );
			}
		}

		ApplySkillSspecial(&tempSkill, sCASKILLACTEX, this, wSKILL_LVL );
	}

}
void GLCrow::ApplySkillSspecial(PGLSKILL pSkill, SSKILLACTEX &sSKILLACTEX, ServerActor* const pTargetActor, WORD wSKILL_LVL)
{
	//	Note : 스킬 특수 기능.
	//
	for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
	{
		const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSKILL_LVL];

		if ( pTargetActor->IsImmuneToSkill( static_cast< BYTE >( pSkill->m_sAPPLY.vecSPEC[k].emSPEC ) ) )
			continue;

		switch ( pSkill->m_sAPPLY.vecSPEC[k].emSPEC )
		{
		case EMSPECA_HP_DIV:
		case EMSPECA_MP_DIV:
		case EMSPECA_SP_DIV:
			break;

		case EMSPECA_RECBLOW:
			//	Note : 상태 이상 치료.
			//
			sSKILLACTEX.dwCUREFLAG |= sSKILL_SPEC.dwFLAG;
			break;
		case EMSPECA_EFFECT_REMOVE:
			{
				if ( sSKILL_SPEC.dwFLAG >= 0 && sSKILL_SPEC.dwFLAG < SKILL::EMACTION_TYPE_NSIZE )
					sSKILLACTEX.dwRemoveFlag[sSKILL_SPEC.dwFLAG] = true;
			}
			break;

		case EMSPECA_PULLIN:
			{
				if ( !pTargetActor->IsValidBody() )
					break;

				float fDist = 0.0f;
				if ( !CheckPullIn( this, pTargetActor, sSKILL_SPEC.fVAR1, fDist ) )
					break;

				sSKILLACTEX.fPUSH_PULL = fDist;				
				sSKILLACTEX.fPUSH_SPEED = sSKILL_SPEC.fVAR2;
				sSKILLACTEX.dwAniType = AN_SUB_PULLIN;
			}
			break;
		case EMSPECA_ONWARD:
			{
				if ( !pTargetActor->IsValidBody() )
					break;

				float fDist = 0.0f;
				if ( !CheckPullIn( this, pTargetActor, sSKILL_SPEC.fVAR1, fDist ) )
					break;

				sSKILLACTEX.fPUSH_PULL = fDist;				
				sSKILLACTEX.fPUSH_SPEED = sSKILL_SPEC.fVAR2;
				sSKILLACTEX.dwAniType = sSKILL_SPEC.dwFLAG;

				sSKILLACTEX.bOnward = true;
			}
			break;
		case EMSPECA_KNOCKBACK:
			{
				if ( !pTargetActor->IsValidBody() )
					break;

				if ( !CheckKnockBack( this, pTargetActor, sSKILL_SPEC.fVAR1 ) )
					break;				

				sSKILLACTEX.fPUSH_PULL = sSKILL_SPEC.fVAR2;				
				sSKILLACTEX.fPUSH_SPEED = GLCONST_CHAR::fPUSHPULL_VELO;
				sSKILLACTEX.dwAniType = AN_SUB_KNOCKBACK;
			}
			break;
		case EMSPECA_STUN:
			{
				if ( !pTargetActor->IsValidBody() )
					break;

				pTargetActor->DO_STUN_ACTION();
			}
			break;
		case EMSPECA_VEHICLE_GETOFF: //봉주
			if( !pTargetActor->IsValidBody() )
				break;

			if( sc::Random::RANDOM_POS() < sSKILL_SPEC.fVAR1 )   // 효과 적용 성공율
			{
				GLChar *pTargetChar = dynamic_cast<GLChar *>(pTargetActor);
				if (pTargetChar)
					pTargetChar->ToggleVehicle(false);
			}
			break;
		case EMSPECA_TURN_STATE_BATTLE:
			if( sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR1 ||
				pTargetActor->GetCrow() != CROW_PC )
				break;		
			((GLChar*)pTargetActor)->AddPlayHostileEachOther( ((GLChar*)pTargetActor)->GetCharID(), FALSE, sSKILL_SPEC.fVAR2 );
			break;

		case EMSPECA_TURN_STATE_PEACE:
			if( pTargetActor->GetCrow() != CROW_PC )
				break;

			((GLChar*)pTargetActor)->RemovePlayHostileEachOther( ((GLChar*)pTargetActor)->GetCharID() );
			break;
		case EMSPECA_SUMMON_CHANGE_SKILLPAGE:
		case EMSPECA_SUMMON_EVENT_SKILL:
		case EMSPECA_SUMMON_TARGETING:
		case EMSPECA_SUMMON_RUNAWAY:
			// 현재 몹도 소환 가능하도록 되어있지 않다;
			break;

		case EMSPECA_REMOVE_SKILL_EFFECT:
			{
				RemoveSkillFactBySpecial( static_cast< EMSPEC_ADDON >( sSKILL_SPEC.nVAR1 ) );
			}
			break;
		};			
	}
}


void GLCrow::SkillProc ( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !m_pAttackProp )
		return;

	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id );
	if (!pSkill)
		return;

	if (wSKILL_LVL >= SKILL::MAX_LEVEL)
		wSKILL_LVL = SKILL::MAX_LEVEL-1;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL];

	// Note : 날씨정보 가져옴
	DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );

	int nEXP_NUM = 0;
	int nALL_LVL = 0;
	int nALL_VARHP = 0, nALL_FULLHP = 0;
	int nALL_VARMP = 0, nALL_FULLMP = 0;
	int nALL_VARSP = 0, nALL_FULLSP = 0;

	WORD wMAX_TAR = 0;
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
		wMAX_TAR = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1;
	else if ( pSkill->m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
		wMAX_TAR = sSKILL_DATA.wTARNUM * ( sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1 );
	else
		wMAX_TAR = sSKILL_DATA.wTARNUM;

	if ( wTARNUM > wMAX_TAR )
		wTARNUM = wMAX_TAR;

	//	Note : 스킬이 적용되는 타겟 검사.
	//	
	for ( WORD i=0; i<wTARNUM; ++i )
	{		
		const STARGETID& TargetID = sTARIDS[i];

		ServerActor* const pTargetActor = m_pGaeaServer->GetTarget(pLand, TargetID);
		if ( pTargetActor == NULL )
			continue;

		// GM 캐릭터 visible off 상태라면 스킬 적용 제외
		if( CROW_PC == pTargetActor->GetCrow() )
		{
			GLChar* pChar = m_pGaeaServer->GetChar( pTargetActor->GetGaeaID() );
			if( NULL == pChar )
				continue;

			if( USER_USER_GM <= pChar->UserLevel() &&
				true == pChar->IsActState( EM_REQ_VISIBLEOFF ) )
				continue;
		}

		/*======== 반격 적용 효과 결과 뽑아냄 =========================================================================================*/

		CSkillCAInterface cSkillCA;

		STARGETID sCId   = STARGETID(CROW_MOB,m_dwGaeaID);;
		STARGETID sCTaId = TargetID;

		bool bCaDamage = false;

		switch( TargetID.emCrow )
		{
		case CROW_PC :
			{
				GLChar* const pTarget = dynamic_cast<GLChar*>(pTargetActor);

				cSkillCA.Cal_LastApplyValueII( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;
				}
			}
			break;
		case CROW_MOB :
			{
				GLCrow* pTarget = dynamic_cast<GLCrow*>(pTargetActor);

				cSkillCA.Cal_LastApplyValueII( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( skill_id, wSKILL_LVL, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;
				}
			}
			break;
		}

		if ( pTargetActor->GetNowHP() != 0 )
		{
			//	Note : 물리 데미지 발생.
			//
			DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
			int nVAR_HP(0), nVAR_MP(0), nVAR_SP(0);

			//	저항치.
			short nRESIST = pTargetActor->GETRESIST().GetElement(pSkill->m_sAPPLY.emELEMENT);
			if ( nRESIST>99 )	nRESIST = 99;


			//	SKILL 기본 적용.
			//
			switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
			{
			case SKILL::EMFOR_HP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					dwDamageFlag = CALCDAMAGE ( nVAR_HP, m_dwGaeaID, TargetID, pLand, pSkill, wSKILL_LVL, dwWeather, sAniAttack.m_wDivCount );
					nVAR_HP = - nVAR_HP;
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_HP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;

			case SKILL::EMFOR_MP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_MP -= (int) ( nVAR - (nVAR*nRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_MP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;

			case SKILL::EMFOR_SP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					int nVAR = int(-sSKILL_DATA.fBASIC_VAR);
					nVAR_SP -= (int) ( nVAR - (nVAR*nRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );
				}
				else										//	대상을 회복시켜 주는 스킬.
				{
					nVAR_SP += (int) ( sSKILL_DATA.fBASIC_VAR );
				}
				break;
			};

			//	Note : SP 부족시에는 변화 값을 반으로 조정.
			//
			if ( bLowSP )
			{
				nVAR_HP /= 2;
				nVAR_MP /= 2;
				nVAR_SP /= 2;
			}

			// Note : 블로킹 검사
			//
			if ( CalcBlocking(pTargetActor, nVAR_HP, pSkill, wSKILL_LVL) == true )
				BlockProc(TargetID, skill_id, wSKILL_LVL);
			else
			{
				//	Note : 스킬 특수 기능.
				//
				int nGATHER_HP(0), nGATHER_MP(0), nGATHER_SP(0);

				for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
				{
					const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSKILL_LVL];

					switch ( pSkill->m_sAPPLY.vecSPEC[k].emSPEC )
					{
					case EMSPECA_PUSHPULL:
						if (!pTargetActor->IsValidBody())
							break;
						if (sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR2)
							break;	//	Note : 발생확율 적용.
						dwDamageFlag |= DAMAGE_TYPE_SHOCK;
						break;

					case EMSPECA_HP_GATHER:
						nGATHER_HP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_MP_GATHER:
						nGATHER_MP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_SP_GATHER:
						nGATHER_SP += int ( abs(nVAR_HP) * sSKILL_SPEC.fVAR1 );
						break;

					case EMSPECA_SWAPPOS:
						ApplySkillSelfSwapPos( this, pSkill, wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL], 1, &sTARIDS[i], pLand);
						break;

					case EMSPECA_TELEPORT:
						ApplySkillSelfTeleport(m_pGaeaServer, this, pSkill, wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL], TargetID, wTARNUM, sTARIDS );
						break;
					case EMSPECA_VEHICLE_GETOFF: //봉주
						if( !pTargetActor->IsValidBody() )
							break;

						if( (sc::Random::RANDOM_POS() < sSKILL_SPEC.fVAR1) && (pTargetActor->GetCrow() == CROW_PC) )   // 효과 적용 성공율
						{
							GLChar *pTargetChar = dynamic_cast<GLChar *>(pTargetActor);
							if (pTargetChar)
								pTargetChar->ToggleVehicle(false);
						}
						break;
					case EMSPECA_LINK_HP:
						if( !pTargetActor->IsValidBody() )
							break;				
						const float fMaxHP = static_cast<float>(GetMaxHP());
						const float fNoWHP = static_cast<float>(GetNowHP());
						pTargetActor->SetMaxHpRate( fNoWHP / fMaxHP );
						//}
						break;	
					};
				}

				if( bCaDamage ) 
				{
					dwDamageFlag |= DAMAGE_TYPE_CA;
				}

				//	Note : 스킬 기본 영향.
				//
				SSKILLACT sSKILLACT;
				//sSKILLACT.sID = STARGETID(CROW_MOB, GetGaeaID());
				//sSKILLACT.sID_TAR = TargetID;
				sSKILLACT.sID     = sCId;
				sSKILLACT.sID_TAR = sCTaId;
				sSKILLACT.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
				sSKILLACT.emAPPLY = pSkill->m_sBASIC.emAPPLY;
				sSKILLACT.sSkillID			= skill_id;
				sSKILLACT.wSkillLevel		= wSKILL_LVL;
				sSKILLACT.dwDamageFlag	= dwDamageFlag;

				//	Note : 체력 변화 저장.
				//
				sSKILLACT.nVAR_HP = nVAR_HP;
				sSKILLACT.nVAR_MP = nVAR_MP;
				sSKILLACT.nVAR_SP = nVAR_SP;

				//	Note : 체력 흡수 저장.
				//
				sSKILLACT.nGATHER_HP = nGATHER_HP;
				sSKILLACT.nGATHER_MP = nGATHER_MP;
				sSKILLACT.nGATHER_SP = nGATHER_SP;

				// 피격시 내구도 감소
				GLChar* pChar = m_pGaeaServer->GetChar( TargetID.GaeaId );
				if( pChar )
				{
					// 공격 상대가 사람인지
					if( m_emCrow == CROW_PC )
						pChar->DurabilityDropHit( abs( nVAR_HP ), TRUE );
					else
						pChar->DurabilityDropHit( abs( nVAR_HP ), FALSE );
				}

				if ( sSKILLACT.VALID() )
					pLand->RegSkillAct ( sSKILLACT );
			}
		}

		//	Note : 스킬 보조 영향.
		//
		SSKILLACTEX sSKILLACTEX;
		sSKILLACTEX.sID = STARGETID(CROW_MOB,GetGaeaID());
		sSKILLACTEX.sID_TAR = TargetID;
		sSKILLACTEX.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;

		sSKILLACTEX.idSKILL = skill_id;
		sSKILLACTEX.wSKILL_LVL = wSKILL_LVL;

		// 상태 이상 유발.
		ITEM::SSATE_BLOW sBLOW;
		//sBLOW.emTYPE = EMBLOW_NONE; // not needed

		const SKILL::SSTATE_BLOW &sSKILL_BLOW = pSkill->m_sAPPLY.sSTATE_BLOW[wSKILL_LVL];

		sBLOW.emTYPE = pSkill->m_sAPPLY.emSTATE_BLOW;
		sBLOW.fRATE = sSKILL_BLOW.fRATE;
		sBLOW.fLIFE = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL].fLIFE;
		sBLOW.fVAR1 = sSKILL_BLOW.fVAR1;
		sBLOW.fVAR2 = sSKILL_BLOW.fVAR2;

		if ( sBLOW.emTYPE!=EMBLOW_NONE )
		{
			//	Note : 발생 확율 계산.
			//
			short nBLOWRESIST = pTargetActor->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBLOW.emTYPE) );
			if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

			//			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, GLPeriod::GetInstance().GetWeather(), pLand->IsWeatherActive() );
			DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );
			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, dwWeather, pLand->IsWeatherActive() );

			BOOL bBLOW(FALSE);
			if ( !(pTargetActor->GETHOLDBLOW()&STATE_TO_DISORDER(sBLOW.emTYPE)) )
			{
				bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBLOW.fRATE * fPOWER, GETLEVEL(), pTargetActor->GetLevel(), nBLOWRESIST );
			}

			if ( bBLOW )
			{
				//	Note : 상태이상 발생.
				//	
				SSTATEBLOW sSTATEBLOW;
				float fLIFE = sBLOW.fLIFE * fPOWER;
				fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );

				sSTATEBLOW.emBLOW = sBLOW.emTYPE;
				sSTATEBLOW.fAGE = fLIFE;
				sSTATEBLOW.fSTATE_VAR1 = sBLOW.fVAR1;
				sSTATEBLOW.fSTATE_VAR2 = sBLOW.fVAR2;

				if ( pTargetActor->STATEBLOW( sSTATEBLOW ) )
				{
					//	Note : 상태 이상 발생 저장.
					sSKILLACTEX.sSTATEBLOW = sSTATEBLOW;
				}
			}
		}

		ApplySkillSspecial( pSkill,  sSKILLACTEX,  pTargetActor,  wSKILL_LVL);


		switch (pSkill->m_sAPPLY.emBASIC_TYPE)
		{
		case SKILL::EMFOR_CURE:
			//	Note : 상태 이상 치료.
			//
			sSKILLACTEX.dwCUREFLAG |= pSkill->m_sAPPLY.dwCUREFLAG;
			break;
		};

		if ( sSKILLACTEX.VALID() )
		{
			pLand->RegSkillActEx ( sSKILLACTEX );
		}
	}

	//	Note : 경험치 획득 산출.
	//
	int nSUM_EXP = 0;
	if ( nEXP_NUM!=0 )
	{
		//	Note : 회복에 기여했을 때만 경험치 발생.	( 공격시는 제외 )
		//
		if ( nALL_VARHP>0 && nALL_VARMP>0 && nALL_VARSP>0 ) // 항상 false 다. ???
		{
			nALL_LVL /= nEXP_NUM;

			nALL_VARHP = abs(nALL_VARHP) / nEXP_NUM;
			nALL_VARMP = abs(nALL_VARMP) / nEXP_NUM;
			nALL_VARSP = abs(nALL_VARSP) / nEXP_NUM;

			nALL_FULLHP /= nEXP_NUM;
			nALL_FULLMP /= nEXP_NUM;
			nALL_FULLSP /= nEXP_NUM;

			//	실제 변화량이 있을 때만 경험치를 산출.
			if ( nALL_VARHP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARHP,nALL_FULLHP);
			if ( nALL_VARMP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARMP,nALL_FULLMP);
			if ( nALL_VARSP!=0 )	nSUM_EXP += GLOGICEX::GLATTACKEXP(GETLEVEL(),nALL_LVL,nALL_VARSP,nALL_FULLSP);
		}
		//	Note : 보조 스킬 사용시 경험치.
		//
		else
		{
			nSUM_EXP = (pSkill->m_sBASIC.dwGRADE*(wSKILL_LVL+1)*100) / GETLEVEL();
			if ( nSUM_EXP < 1 )
				nSUM_EXP = 1;
			if ( nSUM_EXP > 5 )
				nSUM_EXP = 5;
		}
	}
}

void GLCrow::ApplySkill( BOOL bLowSP, SNATIVEID skill_id, WORD wSKILL_LVL, WORD wTARNUM, STARID *sTARIDS )
{
	SkillProc( bLowSP, skill_id, wSKILL_LVL, wTARNUM, sTARIDS  );
}

const bool GLCrow::RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	if( !CheckStateToGo() || IsBoss() || IsPosHold() ) return false;
	if( RF_ENTRANCE_EXIT( m_EffSkillVarSet ).IsOn() ) return false;

	GLLandMan* const pLand(GetLandMan());

	if ( pLand == NULL ) return NULL;

	SNATIVEID sTargetMobID = SNATIVEID( (DWORD)sSPEC.fVAR1, (DWORD)sSPEC.fVAR2 );

	if ( sID.emCrow != CROW_SUMMON || sTargetMobID == NATIVEID_NULL() ) return false;

	PGLSUMMONFIELD pSummon = dynamic_cast<PGLSUMMONFIELD>( GetTarget(sID) );

	if( pSummon == NULL ) return false;

	ServerActor* pOwner = pSummon->GetOwner();

	if( (pOwner == NULL) ||
		(pOwner && pOwner->GetCrow() != CROW_PC) ) return false;

	CROWIDLIST_ITER _iter = pOwner->m_listCrowID.begin();

	DWORD dwID = pOwner->GetGaeaID();

	while( _iter != pOwner->m_listCrowID.end() )
	{
		Faction::GLActorID sDestID = *_iter;

		switch( sDestID.actorType )
		{
		case CROW_SUMMON :
			{
				PGLSUMMONFIELD pSummonDst = pLand->GetSummon(sDestID.actorID_Num);

				if( pSummonDst && pSummonDst->m_sSummonID == sTargetMobID )
				{
					if( pSummonDst->GetOwner() && pSummonDst->GetOwner()->GetGaeaID() == dwID )
					{
						D3DXVECTOR3 vDest = pSummonDst->GetPosition();
						D3DXVECTOR3 vDestRandomPos;
						
						GetRandomPos( &vDestRandomPos, &vDest, sc::Random::RandomNumber( 10.f, 30.f ) );

						vDestRandomPos.y = vDest.y;
						
						return MoveTo( &vDestRandomPos, true, MOVE_OPTION_NOT_TURN_IDLE );
					}					
				}
			}			
			break;
		}

		++_iter;
	}

	return false;
}

const bool GLCrow::RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	if( IsCheckedSkillFlagSpecial( EMSPECA_COUNTATTACK ) || IsCheckedSkillFlagSpecial(EMSPECA_DOMINATION ) )
	{
		return false;
	}

	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		if( RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID().ISNULL() )
			RF_PROVOKE( m_EffSkillVarSet ).SetPreTargetID( GetTargetID() );
	}

	return true;
}