#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/stl_Func.h"

#include "../FieldServer/GLGaeaServer.h"
#include "./GLSummonField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLSummonField::SkillProcess ( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL );
	if ( pSkill == NULL )
		return FALSE;

	float fSkillRange = (float) ( GETBODYRADIUS() + GETSKILLRANGE_APPLY(*pSkill,m_wACTIVESKILL_LVL) + 2 );

	//	Note : 스킬이 적용되는 타겟 검사.
	//
	DWORD dwVALIDNUM(0);
	STARGETID sTARID;
	for ( WORD i = 0; i < m_wTARNUM; ++i )
	{
		sTARID.emCrow = m_sTARIDS[i].GETCROW();
		sTARID.GaeaId   = m_sTARIDS[i].GETID();
		ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, sTARID );
		if ( pACTOR == NULL )
			continue;

		/*
		if( m_emTYPE == SUMMON_TYPE_WAY_1 ||  m_emTYPE == SUMMON_TYPE_WAY_2 )
		{
			switch( pACTOR->GetCrow() )
			{
			case CROW_MOB :
				{
					GLCrow* pMob = dynamic_cast<GLCrow*>(pACTOR);

					if( pMob->IsEntranceExitSkillApply() ) 
					{
						m_sTARIDS[i].dwID = GAEAID_NULL;
						continue;
					}
				}
				break;
			case CROW_PC  :
				{
					GLChar* pUser = dynamic_cast<GLChar*>(pACTOR);

					if( pUser->IsEntranceExitSkillApply() ) 
					{
						m_sTARIDS[i].dwID = GAEAID_NULL;
						continue;
					}
				}
				break;
			}
		}*/
		
		float fReActionRange = (float) ( pACTOR->GetBodyRadius() + fSkillRange );
		fReActionRange *= GLCONST_CHAR::fREACT_VALID_SCALE;
		
		if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( fReActionRange, m_vPosition, pACTOR->GetPosition(), 5.f ) )
		{
			m_sTARIDS[i].dwID = GAEAID_NULL;
		}
		else
		{
			dwVALIDNUM++;
		}
	}

	if ( dwVALIDNUM==0 )
	{
		TurnAction ( GLAT_IDLE, false );
		return FALSE;
	}

	m_fattTIMER += fElapsedTime * GETATTVELO();

	if ( !m_sAttackProp.bUsed )
		return TRUE;

	SANIATTACK &sAniAttack = m_sAttackProp.sAniAttack;
	int        nTotalKeys  = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	int        nThisKey    = int(sAniAttack.m_dwSTime) + int(m_fattTIMER*UNITANIKEY_PERSEC);

	ServerActor* const pTargetActor = m_pGaeaServer->GetTarget(pLand, m_TargetID);	

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

void GLSummonField::SkillProc ( BOOL bLowSP )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( !m_sAttackProp.bUsed )
		return;

	if ( m_sAttackProp.skill_id.dwID != m_idACTIVESKILL.dwID )
	{
		return;
	}

	SANIATTACK &sAniAttack = m_sAttackProp.sAniAttack;

	//	스킬 정보 가져옴.
	SNATIVEID SkillID  = m_idACTIVESKILL;
	WORD     wSkillLevel = m_wACTIVESKILL_LVL;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( SkillID );
	if ( !pSkill )
		return;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wSkillLevel];

	// Note : 날씨 정보 가져옴
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
	STARGETID sTARID;
	for ( WORD i=0; i<m_wTARNUM; ++i )
	{
		const STARID &_sTARID = m_sTARIDS[i];

		sTARID.emCrow = _sTARID.GETCROW();
		sTARID.GaeaId   = _sTARID.GETID();
		ServerActor* pTargetActor = m_pGaeaServer->GetTarget ( pLand, sTARID );
		if ( pTargetActor == NULL )
			continue;

		/*======== 반격 적용 효과 결과 뽑아냄 =========================================================================================*/

		CSkillCAInterface cSkillCA;

		STARGETID sCId   = STARGETID(CROW_MOB,m_dwGaeaID);;
		STARGETID sCTaId = sTARID;

		bool bCaDamage = false;

		switch( sTARID.emCrow )
		{
		case CROW_PC :
			{
				GLChar* const pTarget = dynamic_cast<GLChar*>(pTargetActor);

				cSkillCA.Cal_LastApplyValueII( SkillID, wSkillLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( SkillID, wSkillLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

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

				cSkillCA.Cal_LastApplyValueII( SkillID, wSkillLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( SkillID, wSkillLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

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

			//	Note : 저항치. (주인의 속성을 가져온다)
			short nRESIST = pTargetActor->GETRESIST().GetElement( STATE_TO_ELEMENT(m_sOwnerBlow.emTYPE) );
			if ( nRESIST > 99 )
				nRESIST = 99;

			//	SKILL 기본 적용.
			//
			switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
			{
			case SKILL::EMFOR_HP:
				if ( sSKILL_DATA.fBASIC_VAR < 0.0f )		//	대상에게 피해를 주는 스킬.
				{
					//					bCRITICAL = CALCDAMAGE ( nVAR_HP, bShock, sTARID, m_pLandMan, pSkill, wSKILL_LVL, GLPeriod::GetInstance().GetWeather(), sAniAttack.m_wDivCount );
					dwDamageFlag = CALCDAMAGE ( nVAR_HP, m_dwGaeaID, sTARID, pLand, pSkill, wSkillLevel, dwWeather, sAniAttack.m_wDivCount );
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
			if ( CalcBlocking(pTargetActor, nVAR_HP, pSkill, wSkillLevel) == true )
			{
				BlockProc(sTARID, SkillID, wSkillLevel);
				continue;
			}
			else
			{
				//	Note : 스킬 특수 기능.
				//
				int nGATHER_HP(0), nGATHER_MP(0), nGATHER_SP(0);

				for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
				{
					const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSkillLevel];

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
					};
				}

				//	Note : 스킬 기본 영향.
				//
				SSKILLACT sSKILLACT;
				sSKILLACT.sID      = STARGETID(CROW_SUMMON,GetGaeaID());
				sSKILLACT.sID_TAR  = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
				sSKILLACT.fDELAY   = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
				sSKILLACT.emAPPLY  = pSkill->m_sBASIC.emAPPLY;
				sSKILLACT.dwDamageFlag = dwDamageFlag;

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

				if ( sSKILLACT.VALID() )
				{
					pLand->RegSkillAct ( sSKILLACT );
				}
			}
		}

		//	Note : 스킬 보조 영향.
		//
		SSKILLACTEX sSKILLACTEX;
		SSKILLACTEX sCASKILLACTEX;
		sSKILLACTEX.sID = STARGETID(GetCrow(),GetGaeaID());
		sSKILLACTEX.sID_TAR = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
		sSKILLACTEX.fDELAY = pSkill->m_sEXT_DATA.fDELAY4DAMAGE;

		sSKILLACTEX.idSKILL = SkillID;
		sSKILLACTEX.wSKILL_LVL = wSkillLevel;

		sCASKILLACTEX = sSKILLACTEX;

		//	Note : 상태 이상 유발.
		//         무기의 종속되는 스킬이라면 주인의 무기 속성을 가져온다. 

		ITEM::SSATE_BLOW sBLOW;
		sBLOW.emTYPE = EMBLOW_NONE;

		if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
		{
			sBLOW = m_sOwnerBlow;
		}
		else
		{
			const SKILL::SSTATE_BLOW &sSKILL_BLOW = pSkill->m_sAPPLY.sSTATE_BLOW[wSkillLevel];

			sBLOW.emTYPE = pSkill->m_sAPPLY.emSTATE_BLOW;
			sBLOW.fRATE  = sSKILL_BLOW.fRATE;
			sBLOW.fLIFE  = pSkill->m_sAPPLY.sDATA_LVL[wSkillLevel].fLIFE;
			sBLOW.fVAR1  = sSKILL_BLOW.fVAR1;
			sBLOW.fVAR2  = sSKILL_BLOW.fVAR2;
		}

		if ( sBLOW.emTYPE!=EMBLOW_NONE )
		{
			//	Note : 발생 확율 계산.
			//
			short nBLOWRESIST = pTargetActor->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBLOW.emTYPE) );
			if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

			//			float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, GLPeriod::GetInstance().GetWeather(), m_pLandMan->IsWeatherActive() );
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
					//
					sSKILLACTEX.sSTATEBLOW = sSTATEBLOW;
				}
			}
		}

		sCASKILLACTEX.sSTATEBLOW = sSKILLACTEX.sSTATEBLOW;

		//ApplySkillSpecial( pSkill,  sSKILLACTEX,  pTargetActor,  sTARID, wSkillLevel);
		ApplySkillSpecialCA( pSkill,  sSKILLACTEX,  sCASKILLACTEX, pTargetActor,  sTARID, wSkillLevel);

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
		if ( nALL_VARHP>0 && nALL_VARMP>0 && nALL_VARSP>0 ) // V560 A part of conditional expression is always false: nALL_VARHP > 0.
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
			nSUM_EXP = (pSkill->m_sBASIC.dwGRADE*(wSkillLevel+1)*100) / GETLEVEL();
			if (nSUM_EXP < 1)
                nSUM_EXP = 1;
			if (nSUM_EXP > 5)
                nSUM_EXP = 5;
		}
	}
}

VEC_SK_TAR GLSummonField::DetectTarget ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind, const BOOL bEnemy )
{
	VEC_SK_TAR vecTargetID;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return vecTargetID;
	
	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if ( pOwner == NULL )
		return vecTargetID;

	float fTarLeng(0.0f);
	D3DXVECTOR3 vMax, vMin, vPos;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = m_pGaeaServer->GetChar ( dwGAEAID );

		if ( !pChar ) continue;
        EMIMPACT_SIDE _emTargetSide = (0!=bEnemy) ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR;
        if ( IsReActionable(pChar, _emTargetSide) == EMREACTIONABLE_FALSE )	continue;

		vPos = pChar->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
		{
			vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetGaeaID(), pChar->GetPosition() ) );
		}
	}

	for ( DWORD i=0; i<m_dwCROW_AMOUNT && i<m_vecCROW_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecCROW_VIEW[i];
		GLCrow *pCrow  = pLand->GetCrow( dwGAEAID );

		if ( !pCrow ) continue;
		EMIMPACT_SIDE _emTargetSide = (0!=bEnemy) ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR;
        if ( IsReActionable(pCrow, _emTargetSide) == EMREACTIONABLE_FALSE )	continue;

		vPos = pCrow->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
		{
			vecTargetID.push_back( STARGETID(pCrow->GetCrow(),  pCrow->GetGaeaID(), pCrow->GetPosition() ) );
		}
	}

	for ( DWORD i=0; i<m_dwSUMMON_AMOUNT && i<m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD          dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField *pSummon  = pLand->GetSummon(dwGAEAID);
		if (!pSummon)
            continue;

		ServerActor* pSummonOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
        //GLChar* pSummonOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
        if (!pSummonOwner)
            continue;

		EMIMPACT_SIDE _emTargetSide = (0!=bEnemy) ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR;
		if ( IsReActionable(pSummon, _emTargetSide) == EMREACTIONABLE_FALSE )
            continue;

		vPos = pSummon->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin ) )
		{
			vecTargetID.push_back( STARGETID(pSummon->GetCrow(), pSummon->GetGaeaID(), pSummon->GetPosition()) );
		}
	}

	if( vecTargetID.empty() ) return vecTargetID;

	std_afunc::CCompareTargetDist Comp(D3DXVECTOR3(0.0f,0.0f,0.0f));
	std::sort ( vecTargetID.begin(), vecTargetID.end(), Comp );
	if( vecTargetID.size() > dwMaxFind )
		vecTargetID.erase( vecTargetID.begin()+dwMaxFind, vecTargetID.end() );

	return vecTargetID;
}


VEC_SK_TAR GLSummonField::DetectTarget ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind, const BOOL bEnemy )
{
	VEC_SK_TAR vecTargetID;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return vecTargetID;

	ServerActor* pOwner = m_pGaeaServer->GetActor( pLand, m_sOwnerID );
    //GLChar* pOwner = m_pGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	if ( pOwner == NULL )
		return vecTargetID;

	float fTarLeng(0.0f);
	D3DXVECTOR3 vMax, vMin, vPos;

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<m_vecPC_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecPC_VIEW[i];
		GLChar *pChar = m_pGaeaServer->GetChar ( dwGAEAID );

		if ( !pChar ) continue;
		bool bEnemyTemp = (0!=bEnemy);
		if ( IsReActionable(pChar, bEnemyTemp ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR) == EMREACTIONABLE_FALSE )	continue;
		
		vPos = pChar->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( pLand->IsAbleAttack_Range_LayerSimpleCollision( nRange, pChar->GetPosition(), vDetectPos, 5.f ) )
		{
			switch( m_emTYPE )
			{
			case SUMMON_TYPE_WAY_1 :
			case SUMMON_TYPE_WAY_2 :
				{
					if( !pChar->IsEntranceExitSkillApply() )
						vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetGaeaID(), pChar->GetPosition() ) );
					//else
						//vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetGaeaID(), pChar->GetPosition() ) );
				}
				break;
			default :
				vecTargetID.push_back( STARGETID(pChar->GetCrow(),  pChar->GetGaeaID(), pChar->GetPosition() ) );
			}			
		}
	}

	for ( DWORD i=0; i<m_dwCROW_AMOUNT && i<m_vecCROW_VIEW.size(); ++i )
	{
		DWORD dwGAEAID = m_vecCROW_VIEW[i];
		GLCrow *pCrow  = pLand->GetCrow( dwGAEAID );

		if ( !pCrow ) continue;
		bool bEnemyTemp = (0!=bEnemy);
		if ( IsReActionable(pCrow, bEnemyTemp ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR) == EMREACTIONABLE_FALSE )	continue;

		vPos = pCrow->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( pLand->IsAbleAttack_Range_LayerSimpleCollision( nRange, pCrow->GetPosition(), vDetectPos, 5.f ) )
		{
			switch( m_emTYPE )
			{
			case SUMMON_TYPE_WAY_1 :
			case SUMMON_TYPE_WAY_2 :
				{
					if( !pCrow->IsEntranceExitSkillApply() && !pCrow->IsPosHold() && !pCrow->IsBoss() )
						vecTargetID.push_back( STARGETID(pCrow->GetCrow(),  pCrow->GetGaeaID(), pCrow->GetPosition() ) );
					//else
					//	vecTargetID.push_back( STARGETID(pCrow->GetCrow(),  pCrow->GetGaeaID(), pCrow->GetPosition() ) );
				}
				break;
			default :
				vecTargetID.push_back( STARGETID(pCrow->GetCrow(),  pCrow->GetGaeaID(), pCrow->GetPosition() ) );
			}						
		}
	}

	for ( DWORD i=0; i<m_dwSUMMON_AMOUNT && i<m_vecSUMMON_VIEW.size(); ++i )
	{
		DWORD          dwGAEAID = m_vecSUMMON_VIEW[i];
		GLSummonField* pSummon  = pLand->GetSummon(dwGAEAID);
		if (!pSummon /*|| pSummon->m_OwnerDbNum == GAEAID_NULL*/)
            continue;
		bool bEnemyTemp = (0!=bEnemy);
		if ( IsReActionable(pSummon, bEnemyTemp ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR) == EMREACTIONABLE_FALSE )
            continue;

		vPos = pSummon->GetPosition();

		vMax = vPos + D3DXVECTOR3(8,20,8);
		vMin = vPos + D3DXVECTOR3(-8,0,-8);

		if ( pLand->IsAbleAttack_Range_LayerSimpleCollision( nRange, pSummon->GetPosition(), vDetectPos, 5.f ) )
		{
			vecTargetID.push_back( STARGETID(pSummon->GetCrow(),  pSummon->GetGaeaID(), pSummon->GetPosition() ) );
		}
	}

	if( vecTargetID.empty() ) return vecTargetID;
	
	std_afunc::CCompareTargetDist Comp(vDetectPos);
	std::sort ( vecTargetID.begin(), vecTargetID.end(), Comp );

	if( vecTargetID.size() > dwMaxFind )
	{
		vecTargetID.erase( vecTargetID.begin()+dwMaxFind, vecTargetID.end() );
	}

	return vecTargetID;
}

DWORD GLSummonField::SELECT_SKILLSLOT ( SNATIVEID skill_id, EMSKILLFACT_TYPE emSkillFactType )
{
	DWORD dwSELECT = UINT_MAX;

	//	Note : 타입에 따라 스킬이 들어갈 슬롯 선택.
	//
	switch ( emSkillFactType )
	{
		// Note : 일반 스킬
	case EMSKILLFACT_TYPE_NORMAL:
		{
			for ( DWORD i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			{
                if (i < CROW_SKILLFACT_SIZE && m_sSKILLFACT[i].sNATIVEID == skill_id)
				{
					return dwSELECT = i;
				}
			}

			float fAGE = FLT_MAX;
			for ( DWORD i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			{
                if (i >= CROW_SKILLFACT_SIZE)
                    continue;

				if ( m_sSKILLFACT[i].sNATIVEID==SNATIVEID(false) )
				{
					return dwSELECT = i;
				}

				if ( m_sSKILLFACT[i].fAGE < fAGE )
				{
					fAGE     = m_sSKILLFACT[i].fAGE;
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

const bool GLSummonField::RECEIVE_SKILLFACT_Domination (
	const STARGETID& sID,
	const SKILL::SSPEC& sSPEC,
	const SNATIVEID& sSkillID,
	const WORD wLevel )
{
	if ( EMSPEC_DOMINATE_TYPE(sSPEC.dwFLAG) == EMSPEC_DOMINATE_TYPE_CASTER )
	{
		if ( sID.emCrow == CROW_PC)
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

const bool GLSummonField::RECEIVE_SKILLFACT_Stigma (
	const STARGETID& sID,
	const SKILL::SSPEC& sSPEC,
	const SNATIVEID& sSkillID,
	const WORD wLevel )
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

const bool GLSummonField::RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
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

const BOOL GLSummonField::RECEIVE_SKILLFACT(
    const SNATIVEID& skill_id,
    const WORD wlevel,
    DWORD &dwSELECT,
    const STARGETID& sID,
    const SSKILLFACT::STIME sTime,
    const DWORD dwCount,
    EMSKILLFACT_TYPE emSkillFactType,
	const float* const pLaunchTime, const float* const pLaunchValue)
{
	// Note : 슬롯 최대 사이즈로 초기화한다.
	dwSELECT = UINT_MAX;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )				return FALSE;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];

	BOOL bHOLD = FALSE;
	SSKILLFACT sSKILLEF;
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
		bHOLD = TRUE;
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = sSKILL_DATA.fBASIC_VAR;
		break;
	};

	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecADDON.size(); ++i )
	{
		bHOLD = TRUE;
		SKILL::SIMPACT_ADDON_LEVEL sAddon;
		
		sAddon.emADDON = pSkill->m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
		sAddon.fRate = pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

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
		case EMSPECA_DOMINATION:			
			if ( RECEIVE_SKILLFACT_Domination(sID, sSPEC, skill_id, wlevel) )
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_IGNORED_DAMAGE:
			bHOLD = TRUE;
			GLSUMMON::SET_IGNORED_DAMAGE(WORD(sSPEC.fVAR1), WORD(sSPEC.fVAR2));
			AddSkillFact_SpecialAddon_Rate_Crow(emAddon, sSPEC, sSKILLEF);
			break;

		case EMSPECA_STIGMA:
			bHOLD = true;
			RECEIVE_SKILLFACT_Stigma(sID, sSPEC, skill_id, wlevel);
			AddSkillFact_SpecialAddon_Crow( emAddon, sSPEC, sSKILLEF );	
			break;
		case EMSPECA_PROVOKE :
			{
				if( RECEIVE_SKILLFACT_Provoke( skill_id, sSPEC, wlevel, sID ) )
				{
					bHOLD = true;
					AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
				}				
			}
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			{
				RF_ACTIVE( m_EffSkillVarSet ).UpdateSkill( skill_id.dwID, wlevel, sSPEC.nVAR1, sSPEC.nVAR2 );

				bHOLD = true;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHOLD = true;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;		
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_INCREASE_EFF:
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
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_AIRBORNE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
			bHOLD = TRUE;

			SKILL::SSPEC_ADDON_LEVEL sAddon;

			sAddon.emSPEC = emAddon;
			sAddon.sSPEC.fVAR1 = sSPEC.fVAR1;
			sAddon.sSPEC.fVAR2 = sSPEC.fVAR2;
			sAddon.sSPEC.fRate = sSPEC.fRate; // 효과 비율 적용. by luxes.
			sAddon.sSPEC.fRate2 = sSPEC.fRate2; // 효과 비율 적용. by luxes.
			sAddon.sSPEC.dwFLAG = sSPEC.dwFLAG;
			sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

			sSKILLEF.vecSPEC.push_back( sAddon );

			break;
		};
	}

	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//

	if ( bHOLD )
	{
		if ( emSkillFactType == EMSKILLFACT_TYPE_NORMAL )
		{
			if ( m_pSummonCrowData->m_bNoAllPenalty )
				return TRUE;

			switch ( pSkill->m_sBASIC.emACTION_TYPE )
			{
			case SKILL::EMACTION_TYPE_NORMAL: 
				if ( m_pSummonCrowData->m_dwNoPenaltyFlag & EMCROWNOPENALTY_NORMAL ) 
					return TRUE;			
			case SKILL::EMACTION_TYPE_DEBUFF: 
				if ( m_pSummonCrowData->m_dwNoPenaltyFlag & EMCROWNOPENALTY_DEBUFF ) 
					return TRUE;			
			case SKILL::EMACTION_TYPE_LIMIT:  
				if ( m_pSummonCrowData->m_dwNoPenaltyFlag & EMCROWNOPENALTY_LIMIT  ) 
					return TRUE;			
			}
		}

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
            if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
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

	return TRUE;
}

const BOOL GLSummonField::RECEIVE_SKILLCA( const SSKILLACTEX &sACTEX )
{
	STARGETID sTar( GetCrow(), GetGaeaID() );

	if( (sTar != sACTEX.sID) || (sACTEX.sID == sACTEX.sID_TAR) ) return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sACTEX.idSKILL.wMainID, sACTEX.idSKILL.wSubID );
	if ( !pSkill )
		return FALSE;

	SSKILLFACT		sSKILLEF;
	BOOL bHOLD = FALSE;

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

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecADDON.size(); ++i )
	{
		bHOLD = TRUE;
		SKILL::SIMPACT_ADDON_LEVEL sAddon;

		sAddon.emADDON = tempSkill.m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = tempSkill.m_sAPPLY.vecADDON[i].fADDON_VAR[sACTEX.wSKILL_LVL];
		sAddon.fRate = tempSkill.m_sAPPLY.vecADDON[i].fRate[sACTEX.wSKILL_LVL]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON&		emAddon = tempSkill.m_sAPPLY.vecSPEC[i].emSPEC;
		const SKILL::SSPEC&		sSPEC = tempSkill.m_sAPPLY.vecSPEC[i].sSPEC[sACTEX.wSKILL_LVL];
		switch ( emAddon )
		{
		case EMSPECA_DOMINATION:			
			if ( RECEIVE_SKILLFACT_Domination(sSKILLACTEX.sID, sSPEC, sACTEX.idSKILL, sACTEX.wSKILL_LVL) )
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_IGNORED_DAMAGE:
			bHOLD = TRUE;
			GLSUMMON::SET_IGNORED_DAMAGE(WORD(sSPEC.fVAR1), WORD(sSPEC.fVAR2));
			AddSkillFact_SpecialAddon_Rate_Crow(emAddon, sSPEC, sSKILLEF);
			break;

		case EMSPECA_STIGMA:
			bHOLD = true;
			RECEIVE_SKILLFACT_Stigma(sSKILLACTEX.sID, sSPEC, sACTEX.idSKILL, sACTEX.wSKILL_LVL);
			AddSkillFact_SpecialAddon_Crow( emAddon, sSPEC, sSKILLEF );	
			break;

		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_INCREASE_EFF:
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
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_AIRBORNE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
		case EMSPECA_MOVEVELO_LIMIT:
			bHOLD = TRUE;

			SKILL::SSPEC_ADDON_LEVEL sAddon;

			sAddon.emSPEC = emAddon;
			sAddon.sSPEC.fVAR1 = sSPEC.fVAR1;
			sAddon.sSPEC.fVAR2 = sSPEC.fVAR2;
			sAddon.sSPEC.fRate = sSPEC.fRate; // 효과 비율 적용. by luxes.
			sAddon.sSPEC.fRate2 = sSPEC.fRate2; // 효과 비율 적용. by luxes.
			sAddon.sSPEC.dwFLAG = sSPEC.dwFLAG;
			sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

			sSKILLEF.vecSPEC.push_back( sAddon );

			break;
		};
	}

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

	if( bHOLD && !m_pSummonCrowData->m_bNoAllPenalty )		// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
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
		if ( sSKILLACTEX.dwRemoveFlag[i] == true  && IsSkillEnd(sACTEX.idSKILL))
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

const BOOL GLSummonField::RECEIVE_SKILLFACTII(
	const SNATIVEID& skill_id,
	const WORD wlevel,
	DWORD &dwSELECT,
	const STARGETID& sID,
	const SSKILLFACT::STIME sTime,
	const DWORD dwCount,
	EMSKILLFACT_TYPE emSkillFactType,
	const float* const pLaunchTime, const float* const pLaunchValue)
{
	// Note : 슬롯 최대 사이즈로 초기화한다.
	dwSELECT = UINT_MAX;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )				return FALSE;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];

	BOOL bHOLD = FALSE;
	SSKILLFACT sSKILLEF;


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
		bHOLD = TRUE;
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = sSKILL_DATA.fBASIC_VAR;
		break;
	};

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;
	
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

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecADDON.size(); ++i )
	{
		bHOLD = TRUE;
		SKILL::SIMPACT_ADDON_LEVEL sAddon;

		sAddon.emADDON = tempSkill.m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = tempSkill.m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
		sAddon.fRate = tempSkill.m_sAPPLY.vecADDON[i].fRate[wlevel]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON&		emAddon = tempSkill.m_sAPPLY.vecSPEC[i].emSPEC;
		const SKILL::SSPEC&		sSPEC = tempSkill.m_sAPPLY.vecSPEC[i].sSPEC[wlevel];
		switch ( emAddon )
		{
		case EMSPECA_DOMINATION:			
			if ( RECEIVE_SKILLFACT_Domination(sID, sSPEC, skill_id, wlevel) )
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Crow(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_IGNORED_DAMAGE:
			bHOLD = TRUE;
			GLSUMMON::SET_IGNORED_DAMAGE(WORD(sSPEC.fVAR1), WORD(sSPEC.fVAR2));
			AddSkillFact_SpecialAddon_Rate_Crow(emAddon, sSPEC, sSKILLEF);
			break;

		case EMSPECA_STIGMA:
			bHOLD = true;
			RECEIVE_SKILLFACT_Stigma(sID, sSPEC, skill_id, wlevel);
			AddSkillFact_SpecialAddon_Crow( emAddon, sSPEC, sSKILLEF );	
			break;

		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_INCREASE_EFF:
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
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_AIRBORNE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
		case EMSPECA_MOVEVELO_LIMIT:
			bHOLD = TRUE;

			SKILL::SSPEC_ADDON_LEVEL sAddon;

			sAddon.emSPEC = emAddon;
			sAddon.sSPEC.fVAR1 = sSPEC.fVAR1;
			sAddon.sSPEC.fVAR2 = sSPEC.fVAR2;
			sAddon.sSPEC.fRate = sSPEC.fRate; // 효과 비율 적용. by luxes.
			sAddon.sSPEC.fRate2 = sSPEC.fRate2; // 효과 비율 적용. by luxes.
			sAddon.sSPEC.dwFLAG = sSPEC.dwFLAG;
			sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

			sSKILLEF.vecSPEC.push_back( sAddon );

			break;
		};
	}

	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//

	if ( bHOLD && !m_pSummonCrowData->m_bNoAllPenalty )
	{
		switch ( pSkill->m_sBASIC.emACTION_TYPE )
		{
		case SKILL::EMACTION_TYPE_NORMAL: 
			if ( m_pSummonCrowData->m_dwNoPenaltyFlag & EMCROWNOPENALTY_NORMAL ) 
				return TRUE;			
		case SKILL::EMACTION_TYPE_DEBUFF: 
			if ( m_pSummonCrowData->m_dwNoPenaltyFlag & EMCROWNOPENALTY_DEBUFF ) 
				return TRUE;			
		case SKILL::EMACTION_TYPE_LIMIT:  
			if ( m_pSummonCrowData->m_dwNoPenaltyFlag & EMCROWNOPENALTY_LIMIT  ) 
				return TRUE;			
		}

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
			if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
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

	return TRUE;
}

const BOOL GLSummonField::REMOVE_SKILLFACT(const SNATIVEID& skill_id)
{
    for ( DWORD _i = SKILLFACT_SIZE; _i; --_i)
    {
        SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - _i];
        if (sSKEFF.sNATIVEID == NATIVEID_NULL())
            continue;

        if ( sSKEFF.bEnable == false )
            continue;		

        if ( sSKEFF.sNATIVEID != skill_id )
            continue;

        UPDATE_DATA_END(SKILLFACT_SIZE - _i);
    }
    return true;
}

const BOOL GLSummonField::REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON)
{
    for ( DWORD _i = SKILLFACT_SIZE; _i; --_i)
    {
        SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - _i];
        if (sSKEFF.sNATIVEID == NATIVEID_NULL())
            continue;

        if ( sSKEFF.bEnable == false )
            continue;		

        const DWORD nSpecSize = sSKEFF.vecSPEC.size();
        for ( DWORD _j = nSpecSize; _j ; --_j )
        {
            if (sSKEFF.vecSPEC[nSpecSize - _j].emSPEC == emSPEC_ADDON)
            {
                UPDATE_DATA_END(SKILLFACT_SIZE - _i);
                break;
            }
        }
    }	
    return true;
}

void GLSummonField::VAR_BODY_POINT( const EMCROW emActorCrow, const DWORD dwActorID, const DWORD dwVarTypeFlag, int& nVar_HP, int& nVar_MP, int& nVar_SP )
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

void GLSummonField::UPDATE_DATA_END ( const DWORD dwSkillfactIndex, BOOL bTimeDel )
{
	SSKILLFACT& sSKILLFACT = m_sSKILLFACT[ dwSkillfactIndex ];
	if ( sSKILLFACT.sNATIVEID==NATIVEID_NULL() )
		return;
	if ( !sSKILLFACT.bEnable )
		return;

	const size_t nSize = sSKILLFACT.vecSPEC.size();	

	GLMSG::SNET_SUMMON_END_SKILL NetMsgBrd( m_dwGaeaID, dwSkillfactIndex );
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
	RESETSKEFF(dwSkillfactIndex);

	SendMsgViewAround(&NetMsgBrd);
}

void GLSummonField::UPDATE_DATA_Additional ( const float fElapsedTime )
{
	// 일단 여기서..
	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID();

		if( !sID.ISNULL() )
		{
			if( IsViewInActor( sID ) ) {
				m_TargetID = sID;
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
				UPDATE_DATA_Additional_Stigma(sSKEFF, dwStigmaIndex, _i);
				++dwStigmaIndex;
				break;

			case EMSPECA_DOMINATION:
				UPDATE_DATA_Additional_Domination(sSPEC, sSKEFF, _i);
				break;
			case EMSPECA_PROVOKE :
				UPDATE_DATA_Additional_Provoke(sSPEC, sSKEFF, _i);
				break;
			}
		}
	}
}

void GLSummonField::UPDATE_DATA_Additional_Provoke(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex)
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

void GLSummonField::UPDATE_DATA_Additional_Stigma ( SSKILLFACT& sSKEFF, const DWORD dwStigmaIndex, const DWORD dwSkillfactIndex )
{
	const SSTIGMA::SSTIGMA_PACK* pSSTIGMA_PACK = m_sSTIGMA_TAKER.GetStigmaPack(dwStigmaIndex);
	ServerActor* pACTOR = GetTarget(pSSTIGMA_PACK->sUserID);
	if ( !pACTOR )
	{
		UPDATE_DATA_END(dwSkillfactIndex);
	}
	else
	{
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL )
		{
			UPDATE_DATA_END(dwSkillfactIndex);
			return;
		}

		if ( pSSTIGMA_PACK->dwAsignDistance && 
			!pLand->IsAbleAttack_Range_LayerSimpleCollision( pSSTIGMA_PACK->dwAsignDistance, pACTOR->GetPosition(), GetPosition(), 5.f ) )
		{
			UPDATE_DATA_END(dwSkillfactIndex);
		}
	}
}

void GLSummonField::UPDATE_DATA_Additional_Domination ( SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex )
{
	if ( m_sDOMINATION.emController == EMSPEC_DOMINATE_TYPE_CASTER )
	{
		ServerActor* pACTOR = GetTarget(sSKEFF.sID);
		if ( !pACTOR )
		{
			UPDATE_DATA_END(dwSkillfactIndex);
		}
		else
		{
			GLLandMan* const pLand( GetLandMan() );
			if ( pLand == NULL )
			{
				UPDATE_DATA_END(dwSkillfactIndex);
				return;
			}

			if ( DWORD(sSPEC.fVAR2) && 
				!pLand->IsAbleAttack_Range_LayerSimpleCollision( m_sDOMINATION.fRadiusMoveable, pACTOR->GetPosition(), GetPosition(), 5.f ) )
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

void GLSummonField::CURE_STATEBLOW ( DWORD dwCUREFLAG )
{
	BOOL bChanged = FALSE;

	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		if ( m_sSTATEBLOWS[i].emBLOW!=EMBLOW_NONE )		continue;

		bChanged = TRUE;
		if ( STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW)&dwCUREFLAG )	m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
	}

	if ( !bChanged )	return;

	//	Note : 상태 이상 변화 MsgFB.
	//
	GLMSG::SNETPC_CURESTATEBLOW_BRD NetMsgBRD;
	NetMsgBRD.dwCUREFLAG = dwCUREFLAG;

	//	Note : 주변 클라이언트들에게 메세지 전송.
	//
	SendMsgViewAround(&NetMsgBRD);
}

bool GLSummonField::STATEBLOW ( const SSTATEBLOW &sStateBlow )
{
	if ( !m_pSummonCrowData )
	{
		return false;
	}

	if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_STATE )
	{
		return false;
	}

	int nIndex = 0;

	if ( sStateBlow.emBLOW <= EMBLOW_SINGLE )	nIndex = 0;
	else										nIndex = sStateBlow.emBLOW-EMBLOW_SINGLE;

	m_sSTATEBLOWS[nIndex] = sStateBlow;

	//	상태이상 ( 기절 ) 을 받을때 현제 행위 중단.
	if ( sStateBlow.emBLOW == EMBLOW_STUN )
	{
		TurnAction ( GLAT_IDLE );
		//SetActState( EM_SUMMONACT_STOP );		
	}

	return true;
}

//	끌어오기, 밀어내기 확률 계산 및 거리 확인
BOOL GLSummonField::CHECKPULLIN(const STARGETID &cTargetID, const GLLandMan* pLandMan, const D3DXVECTOR3& vPos, float fRate, float& fDist)
{
    ServerActor *pActor = m_pGaeaServer->GetTarget(pLandMan, cTargetID);
    if (!pActor)
        return FALSE;

    WORD nLevel = GETLEVEL();
    WORD nTarLevel = pActor->GetLevel();
    int nAvoid = pActor->GetAvoid();

    float fBasicRate = ( (float)( nLevel - nTarLevel ) / (float) GLCONST_CHAR::wMAX_LEVEL * (float)nAvoid ) * 0.01f;
    fBasicRate += fRate;
    if (fBasicRate > 1.0f)
    {
        fBasicRate = 1.0f;
    }
    else if (fBasicRate < 0.0f)
    {
        fBasicRate = 0.0f;
    }
    else
    {
    }

    if (fBasicRate > sc::Random::RANDOM_POS()) 
    {
        fDist = D3DXVec3Length ( &D3DXVECTOR3( GetPositionActor() - pActor->GetPositionActor() ) );
        fDist -= GLCONST_CHAR::wBODYRADIUS;
        fDist -= pActor->GetBodyRadius();
        fDist = -fDist;		
        return true;
    }

    return false;

}

// 끌어오기, 밀어내기 확률 계산 및 거리 확인
BOOL GLSummonField::CHECKKNOCKBACK(const STARGETID &cTargetID, const GLLandMan* pLandMan, const float fRate)
{
    ServerActor* pActor = m_pGaeaServer->GetTarget(pLandMan, cTargetID);
    if (!pActor)
        return FALSE;

    WORD nLevel = GETLEVEL();
    WORD nTarLevel = pActor->GetLevel();
    int nAvoid = pActor->GetAvoid();

    float fBasicRate = ( (float)( nLevel - nTarLevel ) / (float) GLCONST_CHAR::wMAX_LEVEL * (float)nAvoid ) * 0.01f;
    fBasicRate += fRate;
    if (fBasicRate > 1.0f)
    {
        fBasicRate = 1.0f;
    }
    else if (fBasicRate < 0.0f)
    {
        fBasicRate = 0.0f;
    }
    else
    {
    }

    if (fBasicRate > sc::Random::RANDOM_POS()) 
        return true;
    else
        return false;
}

DWORD GLSummonField::CALCDAMAGE(
    int& rResultDAMAGE, const DWORD dwGaeaID, const STARGETID &cTargetID, 
    const GLLandMan* pLandMan, const GLSKILL* pSkill/* =NULL */, DWORD dwskill_lev/* =0 */, 
    DWORD dwWeatherFlag/* =NULL */, DWORD dwDivCount/* =0 */ )
{
    ServerActor* pActor = m_pGaeaServer->GetTarget(pLandMan, cTargetID);
    if (!pActor)
        return FALSE;

	DAMAGE_SPEC*	pDamageSpec = pActor->GetDamageSpecPtr();
	if( pDamageSpec->m_dwFlags & DAMAGE_SPEC::FLAG_ILLUSION )
	{
		if( pDamageSpec->m_wIllusionRemain > 0 )
		{
			float fElapsedTime = GetCurrentFrameTime() - pDamageSpec->m_fIllusionLastTime;
			if( fElapsedTime > GLCONST_CHAR::fIllusionInterval )
			{
				pDamageSpec->m_fIllusionLastTime = GetCurrentFrameTime();
				pDamageSpec->m_wIllusionRemain--;
				return DAMAGE_TYPE_ILLUSION;
			} //if
		} //if
	} //if

    int nDEFENSE = pActor->GetDefense ();
    int nDEFAULT_DEFENSE = pActor->GetBodyDefense ();
    int nITEM_DEFENSE = pActor->GetItemDefense ();

	if ( pActor->GetCrow() == CROW_PC || pActor->GetCrow() == CROW_SUMMON )
	{
		nDEFENSE *= GLCONST_CHAR::fReductionRate_PVP_PHY_Defense;
		nDEFAULT_DEFENSE *= GLCONST_CHAR::fReductionRate_PVP_PHY_Defense;
		nITEM_DEFENSE *= GLCONST_CHAR::fReductionRate_PVP_PHY_Defense;
	}

    int nLEVEL = pActor->GetLevel ();
    float fSTATE_DAMAGE = pActor->GETSTATE_DAMAGE ();
    const SRESIST &sRESIST = pActor->GETRESIST ();

	int nDamageLow(m_nDX_DAMAGE);
	int nDamageHigh(m_nDX_DAMAGE);

    DWORD dwApplyFlag = SKILL::EMAPPLY_DWORD_NULL;
    DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
    bool bShock = false;
    bool bCrushingBlow = false;
    bool bCritical = false; 
    bool bPsyDamage = true;

    const DAMAGE_SPEC& sDamageSpec = pActor->GetDamageSpec();
    float fDamageReduce = sDamageSpec.m_fPsyDamageReduce;
    float fDamageReflection = sDamageSpec.m_fPsyDamageReflection;
    float fDamageReflectionRate = sDamageSpec.m_fPsyDamageReflectionRate;
    float fDamageCurse = sDamageSpec.m_fDamageCurse;

    DEFENSE_SKILL sDefenseSkill = pActor->GetDefenseSkill();

    if ( pSkill )
    {
        const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneActionType, pSkill->m_sBASIC.emACTION_TYPE) ||
			CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, pSkill->m_sBASIC.emAPPLY))
		{
			rResultDAMAGE = 0;
			return DAMAGE_TYPE_IMMUNE;
		}

        dwApplyFlag = pSkill->m_sBASIC.emAPPLY;
        float fRESIST_G = 0;
        switch ( pSkill->m_sBASIC.emAPPLY )
        {
            //	근접 공격.
        case SKILL::EMAPPLY_MELEE:
            fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
            break;

            //	장거리 공격.
        case SKILL::EMAPPLY_RANGE:
            fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;

            fDamageReflection = 0.0f;
            fDamageReflectionRate = 0.0f;
            break;

            //	마법 공격.
        case SKILL::EMAPPLY_MAGIC:
            nDEFENSE = 0;						//	마법 공격일때는 일반 방어력 무시됨.
            nDEFAULT_DEFENSE = 0;				//	마법 공격일때는 일반 방어력 무시됨.
            nITEM_DEFENSE = 0;					//	마법 공격일때는 일반 방어력 무시됨.
            fRESIST_G = GLCONST_CHAR::fRESIST_G;

            fDamageReduce = sDamageSpec.m_fMagicDamageReduce;
            fDamageReflection = sDamageSpec.m_fMagicDamageReflection;
            fDamageReflectionRate = sDamageSpec.m_fMagicDamageReflectionRate;
            bPsyDamage = false;
            break;
        };

        // Note : 주인의 무기속성을 가져온다.
        short nRESIST = sRESIST.GetElement ( STATE_TO_ELEMENT(m_sOwnerBlow.emTYPE) );

		int nVAR(::abs(int(sSKILL_DATA.fBASIC_VAR)));

		float fResistTotal(float(nRESIST) * 0.01f * fRESIST_G);
		if ( pActor->GetCrow() == CROW_PC || pActor->GetCrow() == CROW_SUMMON )
		{
			nRESIST *= GLCONST_CHAR::fReductionRate_PVP_MAGIC_Resistance;
		}

		fResistTotal = fResistTotal > GLCONST_CHAR::fREGIST_MAX_G ? GLCONST_CHAR::fREGIST_MAX_G : fResistTotal;
        nVAR = nVAR - (int) ( nVAR*fResistTotal );
        if ( nVAR<0 )
			nVAR = 0;

        // 반영율
        WORD  wGRADE = (WORD)m_cOwnerWeaponsGrade;
        float fGrade = (float)wGRADE / GLCONST_CHAR::fDAMAGE_GRADE_K;

        // 최소/최대 데미지 =  스킬데미지 + (최소/최대 데미지) * 반영율
        nDamageLow  += int(nVAR + ((float) nDamageLow  * fGrade));
        nDamageHigh += int(nVAR + ((float) nDamageHigh * fGrade));

		if ( pActor->GetCrow() == CROW_PC || pActor->GetCrow() == CROW_SUMMON )
		{
			switch ( pSkill->m_sBASIC.emAPPLY )
			{
			case SKILL::EMAPPLY_MELEE:
				nDamageLow *= GLCONST_CHAR::fReductionRate_PVP_PHY_P_Damage;
				nDamageHigh *= GLCONST_CHAR::fReductionRate_PVP_PHY_P_Damage;
				break;

			case SKILL::EMAPPLY_RANGE:
				nDamageLow *= GLCONST_CHAR::fReductionRate_PVP_PHY_S_Damage;
				nDamageHigh *= GLCONST_CHAR::fReductionRate_PVP_PHY_S_Damage;
				break;

			case SKILL::EMAPPLY_MAGIC:
				nDamageLow *= GLCONST_CHAR::fReductionRate_PVP_MAGIC_Damage;
				nDamageHigh *= GLCONST_CHAR::fReductionRate_PVP_MAGIC_Damage;
				break;
			}
		}
    }
    else
    {
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, SKILL::EMAPPLY_MELEE) )
		{				
			rResultDAMAGE = 0;
			return DAMAGE_TYPE_IMMUNE;
		}

        dwApplyFlag = SKILL::EMAPPLY_DWORD_MELEE;

		if ( pActor->GetCrow() == CROW_PC || pActor->GetCrow() == CROW_SUMMON )
		{
			nDamageLow *= GLCONST_CHAR::fReductionRate_PVP_PHY_P_Damage;
			nDamageHigh *= GLCONST_CHAR::fReductionRate_PVP_PHY_P_Damage;
		}
    }

    float fDamageRate = m_fDamageRate;
    SINCREASEEFF* const pIncreaseEff = pActor->GetIncreaseEff();
    if ( pIncreaseEff )
    {
        if ( pIncreaseEff->dwApplyFlag & dwApplyFlag ) // 강화 효과
        {
            // const int nIncreaseCrushingBlow = pIncreaseEff->GetIncreaseCrushingBlowRate(dwApplyFlag); // 소환수는 강타가 없는듯;
            const float fIncreaseRate = float(pIncreaseEff->GetIncreaseDamageRate(dwApplyFlag)) * 0.01f;
            fDamageRate += fIncreaseRate;
        }
    }

    // 공격력 변화율 반영
    m_pGaeaServer->AdjuctDamage(CROW_SUMMON, nDamageLow, nDamageHigh, fDamageRate, m_fOwnerDamagePer);

    // 추가 보정치 산출
    int nExtFORCE = 0;
    int ndxLvl = 0;
    m_pGaeaServer->AdjustLevel(ndxLvl, nExtFORCE, nLEVEL, GETLEVEL());

    // Critical 발생 확율
    bCritical = m_pGaeaServer->CalcCritical(CROW_SUMMON, GETHP(), GETMAXHP(), ndxLvl);

    // Damage 최대 최소값 사이의 랜덤 Damage 결정    
    int nDAMAGE_OLD = m_pGaeaServer->CalcRandomDamage(nDamageLow, nDamageHigh, nExtFORCE);

    // 최저 수용 Damage 산출
    rResultDAMAGE = m_pGaeaServer->CalcLowDamage(CROW_SUMMON, nDAMAGE_OLD, nDEFENSE, fSTATE_DAMAGE);

	SIGNORED_DAMAGE* const pIgnoredDamage = pActor->GetIgnoredDamage();
	if ( (pIgnoredDamage != NULL) && (pIgnoredDamage->isAvailable() == true) )
	{
		if ( pIgnoredDamage->AbsorbDamage(rResultDAMAGE) == false )
			pActor->SKILL_EFFECT_REMOVE(EMSPECA_IGNORED_DAMAGE);

		if ( rResultDAMAGE == 0 )
			return DAMAGE_TYPE_NONE;
	}

    // 방어구 Damage 흡수율 반영
    rResultDAMAGE = m_pGaeaServer->CalcDefenseRate(CROW_SUMMON, nDEFAULT_DEFENSE, nITEM_DEFENSE, rResultDAMAGE, GETLEVEL());

    rResultDAMAGE = m_pGaeaServer->CalcCriticalDamage(bCritical, false, rResultDAMAGE);

    if (rResultDAMAGE <= 1)
    {
        bCritical = false;
        rResultDAMAGE = 1;
    }

    if (dwDivCount > 1)
        rResultDAMAGE /= dwDivCount;

    //	발동 스킬
    if (sDefenseSkill.m_dwSkillID != NATIVEID_NULL())
    {
        if (sDefenseSkill.m_fRate > (sc::Random::RANDOM_POS()*1))
        {
            STARGETID sActor(GETCROW(),dwGaeaID);
            pActor->DefenseSkill(sDefenseSkill.m_dwSkillID, sDefenseSkill.m_wLevel, sActor);
        }
    }

    //	데미지 흡수
    if (fDamageReduce > 0.0f) 
    {
        int nDamageReduce = (int) (((rResultDAMAGE * fDamageReduce) * nLEVEL) / GLCONST_CHAR::wMAX_LEVEL);
        rResultDAMAGE -= nDamageReduce;

        if (bPsyDamage)
            dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
        else
            dwDamageFlag += DAMAGE_TYPE_MAGIC_REDUCE;
    }

    //	데미지 반사 계산
    if (fDamageReflectionRate > 0.0f)
    {
        if (fDamageReflectionRate > (sc::Random::RANDOM_POS()*1))
        {
            int nDamageReflection = (int) (((rResultDAMAGE * fDamageReflection) * nLEVEL) / GLCONST_CHAR::wMAX_LEVEL);
            if (nDamageReflection > 0) 
            {
                if (bPsyDamage)
                    dwDamageFlag += DAMAGE_TYPE_PSY_REFLECTION;
                else
                    dwDamageFlag += DAMAGE_TYPE_MAGIC_REFLECTION;


                // 피격자 입장에서 데미지를 다시 보낸다.
                STARGETID sActor(GETCROW(),dwGaeaID);
                pActor->DamageReflectionProc(nDamageReflection, sActor);
            }
        }
    }

    //	저주 계산
    if (fDamageCurse > 0.0f) 
    {
        int nDamageCurse = (int)(rResultDAMAGE * fDamageCurse);
        if (nDamageCurse > 0) 
        {
            STARGETID sActor(GETCROW(),dwGaeaID);
            ServerActor *pMyActor = m_pGaeaServer->GetTarget(pLandMan, sActor);
            if (!pMyActor)
                return FALSE;

            STARGETID sCurseTargetID = sDamageSpec.m_sCurseTargetID;
            pMyActor->DamageCurse(nDamageCurse, sCurseTargetID, cTargetID.emCrow);
        }
    }

    bShock = (0!=GLOGICEX::CHECKSHOCK(GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical));

    if (bShock)
        dwDamageFlag += DAMAGE_TYPE_SHOCK;
    if (bCritical)
        dwDamageFlag += DAMAGE_TYPE_CRITICAL;

    return dwDamageFlag;
}

void GLSummonField::ApplySkillSpecial( PGLSKILL pSkill, SSKILLACTEX& sSKILLACTEX, ServerActor* const pTargetActor,STARGETID sTARID, WORD wSkillLevel)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	//	Note : 스킬 특수 기능.
	//
	for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
	{
		const SKILL::SSPEC &sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSkillLevel];

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
				ApplySkillTargetPullin( this, pTargetActor, sSKILL_SPEC, sSKILLACTEX );
			}
			break;
		case EMSPECA_ONWARD:
			{
				ApplySkillTargetOnward( this, pTargetActor, pTargetActor->GetTargetID(), sSKILL_SPEC, sSKILLACTEX );

				/*if ( !pTargetActor->IsValidBody() )
					break;

				float fDist = 0.0f;
				if ( !CHECKPULLIN( sTARID, pLand, m_vPosition, sSKILL_SPEC.fVAR1, fDist ) )
					break;

				sSKILLACTEX.fPUSH_PULL = fDist;				
				sSKILLACTEX.fPUSH_SPEED = sSKILL_SPEC.fVAR2;
				sSKILLACTEX.dwAniType = sSKILL_SPEC.dwFLAG;

				sSKILLACTEX.bOnward = true;*/
			}
			break;
		case EMSPECA_KNOCKBACK:
			{
				ApplySkillTargetKnockback( this, pTargetActor, sSKILL_SPEC, sSKILLACTEX );
				/*if ( !pTargetActor->IsValidBody() )
					break;

				if ( !CHECKKNOCKBACK( sTARID, pLand, sSKILL_SPEC.fVAR1 ) )
					break;				

				sSKILLACTEX.fPUSH_PULL = sSKILL_SPEC.fVAR2;				
				sSKILLACTEX.fPUSH_SPEED = GLCONST_CHAR::fPUSHPULL_VELO;
				sSKILLACTEX.dwAniType = AN_SUB_KNOCKBACK;*/
			}
			break;
		case EMSPECA_STUN:
			{
				ApplySkillTargetStun( pTargetActor );
			}
			break;
		case EMSPECA_SUMMON_CHANGE_SKILLPAGE:
		case EMSPECA_SUMMON_EVENT_SKILL:
		case EMSPECA_SUMMON_TARGETING:
		case EMSPECA_SUMMON_RUNAWAY:
			// 현재 소환수도 소환 가능하도록 되어있지 않다;
			break;

		case EMSPECA_REMOVE_SKILL_EFFECT:
			{
				RemoveSkillFactBySpecial( static_cast< EMSPEC_ADDON >( sSKILL_SPEC.nVAR1 ) );
			}
			break;
		};
	}
}

void GLSummonField::ApplySkillSpecialCA( PGLSKILL pSkill, SSKILLACTEX& sSKILLACTEX, SSKILLACTEX& sCASKILLACTEX, ServerActor* const pTargetActor,STARGETID sTARID, WORD wSkillLevel)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

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
				ApplySkillSpecial( pSkill, sSKILLACTEX, pTargetActor, sTARID, wSkillLevel   );
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

		ApplySkillSpecial(&tempSkill, sSKILLACTEX, pTargetActor, sTARID, wSkillLevel );

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

		ApplySkillSpecial(&tempSkill, sCASKILLACTEX, this, sTARID, wSkillLevel );
	}
}