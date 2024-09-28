#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/G-Logic/Glogic.h"

#include "../GLogicEx.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "../Item/GLItemMan.h"
#include "../Crow/CrowAttack.h"
#include "./GLSummon.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLSUMMON& GLSUMMON::operator= ( const GLSUMMON& rvalue )
{
	m_emTYPE	  = rvalue.m_emTYPE;
	m_sSummonID   = rvalue.m_sSummonID;
	m_dwOwnerGaeaID	  = rvalue.m_dwOwnerGaeaID;
	m_sOwnerID		= rvalue.m_sOwnerID;
	m_dwNowHP	  = rvalue.m_dwNowHP;
	m_wNowMP	  = rvalue.m_wNowMP;

	m_fRemainedTime      = rvalue.m_fRemainedTime;
	m_fOwnerIncHP        = rvalue.m_fOwnerIncHP;
	m_wOwnerAttack       = rvalue.m_wOwnerAttack;
	m_wOwnerDefense      = rvalue.m_wOwnerDefense;
	m_wOwnerSkillLevel   = rvalue.m_wOwnerSkillLevel;
	m_cOwnerWeaponsGrade = rvalue.m_cOwnerWeaponsGrade;
	m_sOwnerBlow         = rvalue.m_sOwnerBlow;
	m_emOwnerItemAtt		= rvalue.m_emOwnerItemAtt;
    m_wOwnerSchool       = rvalue.m_wOwnerSchool;

	m_fRunArea             = rvalue.m_fRunArea;
	m_fWalkArea            = rvalue.m_fWalkArea;
	m_fOwnerDistance       = rvalue.m_fOwnerDistance;
	m_fOwnerDegree         = rvalue.m_fOwnerDegree;
	m_fOwnerDamagePer	   = rvalue.m_fOwnerDamagePer;
	m_fOwnerMovePer			= rvalue.m_fOwnerMovePer;
	m_bOwnerSpeed		   = rvalue.m_bOwnerSpeed;
	m_fPartyDistance       = rvalue.m_fPartyDistance;
	m_fMoveRefreshDistance = rvalue.m_fMoveRefreshDistance;

	m_emNeedItemAtt		 = rvalue.m_emNeedItemAtt;

	return *this;
}

void GLSUMMON::ASSIGN ( const GLSUMMON& sSummonData )
{
	m_emTYPE	  = sSummonData.m_emTYPE;
	m_sSummonID   = sSummonData.m_sSummonID;
	m_dwOwnerGaeaID	  = sSummonData.m_dwOwnerGaeaID;
	m_sOwnerID		= sSummonData.m_sOwnerID;
	m_dwNowHP	  = sSummonData.m_dwNowHP;
	m_wNowMP	  = sSummonData.m_wNowMP;

	m_fRemainedTime      = sSummonData.m_fRemainedTime;
	m_fOwnerIncHP        = sSummonData.m_fOwnerIncHP;
	m_wOwnerAttack       = sSummonData.m_wOwnerAttack;
	m_wOwnerDefense      = sSummonData.m_wOwnerDefense;
	m_wOwnerSkillLevel   = sSummonData.m_wOwnerSkillLevel;
	m_cOwnerWeaponsGrade = sSummonData.m_cOwnerWeaponsGrade;
    m_sOwnerBlow         = sSummonData.m_sOwnerBlow;
	m_emOwnerItemAtt		= sSummonData.m_emOwnerItemAtt;
    m_wOwnerSchool       = sSummonData.m_wOwnerSchool;

	m_fRunArea             = sSummonData.m_fRunArea;
	m_fWalkArea            = sSummonData.m_fWalkArea;
	m_fOwnerDistance       = sSummonData.m_fOwnerDistance;
	m_fOwnerDegree         = sSummonData.m_fOwnerDegree;
	m_fOwnerDamagePer	   = sSummonData.m_fOwnerDamagePer;
	m_fOwnerMovePer			= sSummonData.m_fOwnerMovePer;
	m_bOwnerSpeed		   = sSummonData.m_bOwnerSpeed;
	m_fPartyDistance       = sSummonData.m_fPartyDistance;
	m_fMoveRefreshDistance = sSummonData.m_fMoveRefreshDistance;

	m_emNeedItemAtt		   = sSummonData.m_emNeedItemAtt;
}

D3DXVECTOR3  GLSUMMON::GetRandomPosition ()
{
	//srand(unsigned int(time(NULL)));
	WORD idx = (WORD)(rand() % 8);
	return m_RandPos[idx];
}

const float GLSUMMON::GET_INTERACTION_RANGE() const
{ 
	return (m_pSummonCrowData->m_fRefenceActionableRange > 0.0f) ? m_pSummonCrowData->m_fRefenceActionableRange : GLCONST_CHAR::fNPC_INTERACTION_RANGE; 
}

bool GLSUMMON::ISSummonNPC_INTERACTIONABLE(
										const D3DXVECTOR3& vReqPos,
										const D3DXVECTOR3& vCurPos,
										const float fReqRadius,
										int nNpcAction) const
{
	if (!m_pSummonCrowData)
		return false;

	if (GETCROW() != CROW_SUMMON)
		return false;

	if (nNpcAction != SNpcTalk::EM_BASIC_NULL && !m_pSummonCrowData->IsBasicTalk(nNpcAction))
		return false;

	float fLength = GLCONST_CHAR::fNPC_INTERACTION_ADJUST + GET_INTERACTION_RANGE() + fReqRadius + GETBODYRADIUS();
	if (D3DXVec3LengthSq(&D3DXVECTOR3(vCurPos - vReqPos)) <= (fLength*fLength))
		return true;
	else
		return false;
}

void GLSUMMON::RESET ()
{
	m_emTYPE	    = SUMMON_TYPE_NONE;
	m_sSummonID     = NATIVEID_NULL();
	m_dwOwnerGaeaID 	    = 0;
	m_sOwnerID		= Faction::GLActorID();
	m_fRemainedTime = 0.0f;

	m_fOwnerIncHP        = 0.0f;
	m_wOwnerAttack       = 0;
	m_wOwnerDefense      = 0;
	m_wOwnerSkillLevel   = 0;
	m_cOwnerWeaponsGrade = 0;
    m_sOwnerBlow         = ITEM::SSATE_BLOW();
	m_emOwnerItemAtt	 = ITEMATT_NOTHING;
	m_emNeedItemAtt      = SKILL::SKILLATT_NOTHING;
    m_wOwnerSchool       = 0;

	m_sDamageSpec.RESET();
	m_sSUMRESIST.RESET();
	m_sDefenseSkill.RESET();
}

BOOL GLSUMMON::LoadFile(const std::string& FileName, SUMMON_TYPE emType)
{
	if (FileName.empty())
        return FALSE;

	std::string strPath(GLOGIC::GetPath());
	strPath.append(FileName);

	gltexfile cFILE;
	cFILE.reg_sep("\t ,{}[]()|");

	if (GLOGIC::UseLogicZipFile()) // by 경대
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{
        sc::writeLogError(
            sc::string::format(
                "GLSUMMON::LoadFile %1% %2%",
                GLogicData::GetInstance().GetGlogicZipFile(), strPath));		
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION, true );

	cFILE.getflag( "emTYPE",	           1, 1, m_emTYPE, true );
	cFILE.getflag( "fRunArea",             1, 1, m_fRunArea, true );             // 달려야만 하는 거리
	cFILE.getflag( "fWalkArea",            1, 1, m_fWalkArea, true );            // 걸어야만 하는 거리
	cFILE.getflag( "fOwnerDistance",       1, 1, m_fOwnerDistance, true );       // 주인과의 유지 거리
	cFILE.getflag( "fOwnerDegree",         1, 1, m_fOwnerDegree, true );         // 주인과의 유지 각도
	cFILE.getflag( "fPartyDistance",       1, 1, m_fPartyDistance, true );       // 파티 인식 거리
	cFILE.getflag( "fMoveRefreshDistance", 1, 1, m_fMoveRefreshDistance, true ); // 이동 갱신 거리
	cFILE.getflag( "fOwnerDamagePer",	   1, 1, m_fOwnerDamagePer, true );	   // 주인의 데미지 계승률
	cFILE.getflag( "fOwnerMovePer",		1, 1, m_fOwnerMovePer, true );			// 주인의 이동속도 계승률
	cFILE.getflag( "bOwnerSpeed",		   1, 1, m_bOwnerSpeed, true );		   // 주인의 속도를 따라 가는지 여부

	GLCONST_SUMMON::pGLSUMMON[emType] = this;

	cFILE.getflag( "fSummonGateDistanceRatio", 1, 1, GLCONST_SUMMON::fSummonGateDistanceRatio[emType], true ); // 소환 게이트 생성 비율.
	cFILE.getflag( "dwIdleGestureTimeMax",     1, 1, GLCONST_SUMMON::dwIdleGestureTimeMax[emType], true     ); // IDLE Gesture 최대 대기 시간.
	cFILE.getflag( "dwIdleGestureTimeMin",     1, 1, GLCONST_SUMMON::dwIdleGestureTimeMin[emType], true     ); // IDLE Gesture 최소 대기 시간.
	cFILE.getflag( "dwMaxSummon",              1, 1, GLCONST_SUMMON::dwMaxSummon[emType], true              ); // 타입별 최대 소환 가능 갯수.
	cFILE.getflag( "wMaxMobTracking",          1, 1, GLCONST_SUMMON::wMaxMobTracking[emType], true		  ); // 타입별 몹 몰이 횟수.


	return TRUE;
}


void GLSUMMON::RESET_DATA ()
{
	int i(0);

	m_emTYPE	      = SUMMON_TYPE_NONE;
	m_sSummonID       = SNATIVEID(0,0);
	m_pSummonCrowData = NULL;

	for ( i=0; i< SCROWDATA::EMMAXATTACK; ++i )	
		m_fACTIONDELAY[i] = 0.0f;

	for ( i=0; i< CROW_SKILLFACT_SIZE; ++i )
		m_sSKILLFACT[i] = SSKILLFACT();

	m_dwHOLDBLOW = NULL;

	for ( i=0; i< EMBLOW_MULTI; ++i )
		m_sSTATEBLOWS[i] = SSTATEBLOW();

	m_fSTATE_MOVE_RATE = 1.0f;
	m_fSTATE_DELAY = 1.0f;
	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;
	//m_bINVISIBLE = false;
	//m_bRECVISIBLE = false;

	m_fATTVELO = 0.0f;
	m_fSKILL_MOVE = 0.0f;

	m_nSUM_HIT = 0;
	m_nSUM_AVOID = 0;

	m_fVariationRateHP = 0.0f;
    m_fVariationRateMP = 0.0f;
    m_fVariationRateSP = 0.0f;

    m_fVariationApplyRateHP = 1.0f;
    m_fVariationApplyRateMP = 1.0f;
    m_fVariationApplyRateSP = 1.0f;

	m_nSUM_DEFENSE = 0;
	m_nDX_DAMAGE   = 0;

	m_nSUM_PIERCE = 0;

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_dwAType = 0;
	m_bUsingEventSkill = false;

	m_dwNowHP = 0;
	m_wNowMP = 0;
	m_wNowSP = 0;

	m_fRemainedTime = 0;

	m_fOwnerIncHP        = 0.0f;
	m_wOwnerAttack       = 0;
	m_wOwnerDefense      = 0;
	m_wOwnerSkillLevel   = 0;
	m_cOwnerWeaponsGrade = 0;
	m_sOwnerBlow         = ITEM::SSATE_BLOW();
	m_emOwnerItemAtt		= ITEMATT_NOTHING;
	m_emNeedItemAtt      = SKILL::SKILLATT_NOTHING;
    m_wOwnerSchool       = 0;

	m_idACTIVESKILL = false;
	m_wACTIVESKILL_LVL = 0;

	m_bSafeZone = false;

	m_fTempVarHP = 0.0f;
	m_fTempVarMP = 0.0f;
	m_fTempVarSP = 0.0f;

	for ( i = 0; i < EMELEMENT_MAXNUM; ++i ) 
		m_sDurDamage[i].RESET();

	m_fRunArea             = 0.0f;
	m_fWalkArea            = 0.0f;
	m_fOwnerDistance       = 0.0f;
	m_fOwnerDegree         = 0.0f;
	m_fOwnerDamagePer	   = 0.0f;
	m_fOwnerMovePer			= 0.f;
	m_bOwnerSpeed		   = false;
	m_fPartyDistance       = 0.0f;
	m_fMoveRefreshDistance = 0.0f;

	m_emGestureState = EM_SUMMONGESTURE_NONE;
	m_dwGestureNum   = 0;

	m_fCurSpeed = 0.0f;

	m_dwCurSkillPage = 0;

	for ( i=0; i<EM_SUMMONGESTURE_MAX; ++i )
		m_emGestureableArray[i] = EM_SUMMONGESTURE_NONE;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
}

const char* GLSUMMON::GETNAME ()
{
	GASSERT(m_pSummonCrowData);
	return m_pSummonCrowData->GetName();
}

WORD GLSUMMON::GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const
{
	int nRANGE = sSKILL.m_sBASIC.wTARRANGE;

	// 소환수는 적용받지 않는다.
	//	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		nRANGE += (int) GETSUM_RANGE_PSY_LONG();

	//	타겟이 적일때만 적용
	//	if ( sSKILL.m_sBASIC.emIMPACT_SIDE==SIDE_ENEMY )	nRANGE += (int) GETSUM_TARRANGE_ALL();

	if ( nRANGE < 0 ) nRANGE = 0;

	return (WORD)nRANGE;
}


WORD GLSUMMON::GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const
{
	const SKILL::CDATA_LVL &sDATA_LVL = sSKILL.m_sAPPLY.sDATA_LVL[dwLEVEL];

	int nRANGE = sDATA_LVL.wAPPLYRANGE;

	// 소환수는 적용받지 않는다.
//	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		nRANGE += (int) GETSUM_RANGE_PSY_LONG();

	//	타겟이 적일때만 적용
//	if ( sSKILL.m_sBASIC.emIMPACT_SIDE==SIDE_ENEMY )	nRANGE += (int) GETSUM_APPLYRANGE_ALL();
	
	if ( nRANGE < 0 ) nRANGE = 0;

	return nRANGE;
}

const int GLSUMMON::GETMAXVIEWRANGE(void) const
{
	return int(m_sRateViewRange * MAX_VIEWRANGE);
}

BOOL GLSUMMON::INIT_DATA ()
{
	m_sSummonID = m_pSummonCrowData->sNativeID;
	m_dwNowHP   = m_pSummonCrowData->m_dwHP;
	m_wNowMP    = m_pSummonCrowData->m_wMP;

	return TRUE;
}

void GLSUMMON::GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA )
{
	if( pSSPECA == NULL || pSIMA == NULL ) return;

	std::vector<SKILL::SIMPACT_ADDON_LEVEL>::iterator _iSater = m_sSKILLFACT[dwSlot].vecADDON.begin();

	int iCnt = 0;

	while( _iSater != m_sSKILLFACT[dwSlot].vecADDON.end())
	{
		pSIMA[iCnt++] = static_cast<BYTE>(_iSater->emADDON);

		++_iSater;
	}

	iCnt = 0;

	BOOL bInsert = TRUE;
	BOOL bCheck  = FALSE;

	BYTE iValue  = 0;
	
	PGLSKILL pSkill = NULL;

	if( RF_IMMUNE( m_EffSkillVarSet ).IsOn() ) 
	{
		pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[dwSlot].sNATIVEID );		

		if( pSkill && pSkill->m_sAPPLY.sDATA_LVL[m_sSKILLFACT[dwSlot].wLEVEL].fBASIC_VAR < 0 )
		{
			bCheck = TRUE;
		}
	}

	std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iSSater = m_sSKILLFACT[dwSlot].vecSPEC.begin();

	while( _iSSater != m_sSKILLFACT[dwSlot].vecSPEC.end())
	{
		bInsert = TRUE;

		iValue = static_cast<BYTE>(_iSSater->emSPEC);

		if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( iValue ) )
			bInsert = FALSE;

		if( bInsert )	pSSPECA[iCnt++] = iValue;

		++_iSSater;
	}
}

void GLSUMMON::SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel )
{
	m_idACTIVESKILL    = skill_id;

	/*if ( m_emTYPE == SUMMON_TYPE_NORMAL )
		m_wACTIVESKILL_LVL = 0;
	else*/
	m_wACTIVESKILL_LVL = wLevel;
}

EMSKILLCHECK GLSUMMON::CHECHSKILL ( DWORD dwAType, bool bNotLearn, bool bEventSkill )
{
	if ( bEventSkill )
	{
		if ( dwAType >= m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage ).size() ||
			dwAType < 0 )
			return EMSKILL_FAILED;
	}
	else
	{
		if ( dwAType >= m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size() ||
			dwAType < 0 )
			return EMSKILL_FAILED;
	}

	SNATIVEID skill_id;
	WORD wskill_lev = 0;
	WORD wStrikeNum = 0;
	if ( bNotLearn )
	{
		skill_id  = m_sDefenseSkill.m_dwSkillID;
		wskill_lev = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		if ( bEventSkill )
		{
			const SCROWATTACK &sATTACK = m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage )[dwAType];
			skill_id   = sATTACK.skill_id;
			wskill_lev = m_wOwnerSkillLevel; // 주인의 소환 스킬레벨로
		}
		else
		{
			const SCROWATTACK &sATTACK = m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[dwAType];
			skill_id   = sATTACK.skill_id;
			wskill_lev = m_wOwnerSkillLevel; // 주인의 소환 스킬레벨로
		}

		//	Note : 스킬 딜래이 타임이 지낫는지 점검.
		//
		if ( m_fACTIONDELAY[dwAType] > 0.0f && !bEventSkill )	
			return EMSKILL_DELAYTIME; // 스킬 딜래이 시간이 지나지 않음.
	}

	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )															return EMSKILL_FAILED;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wskill_lev];

	//	Note : 사용할 수 있는 스킬종류인지 검사.
	//if ( pSkill->m_sBASIC.emROLE!=SKILL::EMROLE_NORMAL )					return EMSKILL_UNKNOWN;

	//	Note : 기초 체력 소모량 점검.
	//if ( GETHP() <= sSKILL_DATA.wUSE_HP*wStrikeNum )						return EMSKILL_NOTHP;
	//if ( GETMP() < sSKILL_DATA.wUSE_MP*wStrikeNum )							return EMSKILL_NOTMP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_HP_PTY;
	//sSKILL_DATA.wUSE_MP_PTY;

	//WORD wDisSP = sSKILL_DATA.wUSE_SP;
	//if ( GETSP() < wDisSP*wStrikeNum )									return EMSKILL_NOTSP;

	//	NEED : 향후 파티스킬이 구성되었을때.
	//
	//sSKILL_DATA.wUSE_SP_PTY;

	return EMSKILL_OK;
}

void GLSUMMON::ACCOUNTSKILL ( WORD wStrikeNum )
{
	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[m_wACTIVESKILL_LVL];

	//	스킬 딜래이 등록.
	float fDelayTime = sSKILL_DATA.fDELAYTIME;

	//	Note : 상태이상의 딜래이 감안.
	m_fACTIONDELAY[m_dwAType] = fDelayTime * m_fSTATE_DELAY;

	//	기초 체력 소모.
	DECREASE ( m_dwNowHP, DWORD(sSKILL_DATA.wUSE_HP*wStrikeNum) );
	DECREASE ( m_wNowMP, WORD(sSKILL_DATA.wUSE_MP*wStrikeNum) );
}

DWORD GLSUMMON::RECEIVE_DAMAGE ( const WORD wDamage )
{
	DWORD dwOLD = m_dwNowHP;

	DECREASE(m_dwNowHP, (DWORD)wDamage);

	RELEASE_ENDURE_PROC();

	if ( m_dwNowHP <= 0 )
	{
#ifdef _DEBUG
		sc::writeLogInfo ( sc::string::format( "---- Summon is Die. ----", wDamage ) );
		sc::writeLogInfo ( sc::string::format( "Summon Receive Damage : %1%", wDamage ) );
		sc::writeLogInfo ( sc::string::format( "Summon Old HP : %1%", dwOLD ) );
		sc::writeLogInfo ( sc::string::format( "Summon New HP : %1%", m_dwNowHP ) );
		sc::writeLogInfo ( sc::string::format( "------------------------", m_dwNowHP ) );
#endif
	}

	return (dwOLD>m_dwNowHP) ? (dwOLD-m_dwNowHP) : 0;
}

void GLSUMMON::RELEASE_ENDURE_PROC ()
{
	const DWORD nRELEASE_ENDURE = m_sRELEASE_ENDURE.nRELEASE_ENDURE;
	if ( nRELEASE_ENDURE )
	{
		// 피격시 스킬 효과 해제 스킬이 걸려있었다면 처리;
		// 일반 공격 피해;
		const DWORD dwPercentage = sc::Random::getInstance().GetPercent();
		for ( unsigned int _i = 0; _i < nRELEASE_ENDURE; ++_i )
		{			
			if ( dwPercentage < m_sRELEASE_ENDURE.dwActivePercentage[_i] )
			{	
				UPDATE_DATA_END(m_sRELEASE_ENDURE.dwSkillFactIndex[_i]);				
				continue;
			}			
		}
	}
}

DWORD GLSUMMON::RECEIVE_HEAL(const int nHeal)
{
	const DWORD dwOldHP = m_dwNowHP;
	m_dwNowHP += nHeal;
	if ( m_dwNowHP > m_pSummonCrowData->m_dwHP )
		m_dwNowHP = m_pSummonCrowData->m_dwHP;

	return (dwOldHP < m_dwNowHP) ? (m_dwNowHP - dwOldHP) : 0;

	/*m_dwNowHP += nHeal;
	if ( m_dwNowHP > m_pSummonCrowData->m_dwHP )
		m_dwNowHP = m_pSummonCrowData->m_dwHP;

	return m_dwNowHP;*/

}

void GLSUMMON::PrevUPDATE_DATA ( const float fElapsedTime )
{
	ZeroMemory(m_bSkillFactsSpecial, sizeof(m_bSkillFactsSpecial));

	m_nSUM_HIT     = static_cast<int>  (m_pSummonCrowData->m_wHitRate);
	m_nSUM_AVOID   = static_cast<int>  (m_pSummonCrowData->m_wAvoidRate);

	m_fVariationRateHP = m_fOwnerIncHP;   // 주인의 회복율
    m_fVariationRateMP = 0.3f * 0.01f;
    m_fVariationRateSP = 0.5f;

    m_fVariationApplyRateHP =  1.0f;
    m_fVariationApplyRateMP =  1.0f;
    m_fVariationApplyRateSP =  1.0f;

	m_nSUM_DEFENSE = m_wOwnerDefense; // 주인의 방어력
	m_nDX_DAMAGE   = m_wOwnerAttack;  // 주인의 공격력

	m_fSTATE_MOVE_RATE  = 1.0f;
	m_fSTATE_DELAY = 1.0f;

	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;

	m_fATTVELO    = 0.0f;
	m_fSKILL_MOVE = 0.0f;

	m_nSUM_PIERCE = 0;

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_sDamageSpec.RESET();
	m_sDefenseSkill.RESET();
	m_sINVISIBLE.RESET();	
	m_sRECVISIBLE.RESET();
	//m_bINVISIBLE  = false;
	//m_bRECVISIBLE = false;
	m_sRELEASE_ENDURE.RESET();
	m_sHALLUCINATE.RESET();
	m_sDOMINATION.RESET();
	m_sSTIGMA_TAKER.RESET();	
	m_sIncreaseEff.RESET();	
	m_sLINKHP.RESET();
	m_sRateViewRange.RESET();

	m_sSUMRESIST = m_pSummonCrowData->m_sResist;

	m_dwActionLimit = EMACTION_LIMIT_NONE;

	m_bSafeZone = false;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

	m_SkillCAIndex.ResetData();

	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
	RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
	RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
}

void GLSUMMON::UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient )
{
	PrevUPDATE_DATA( fElapsedTime );

	SDURATION_DAMAGE	sDurDamage[EMELEMENT_MAXNUM];

	for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;
		if ( !sSKEFF.bEnable )						continue;

        if ( !sSKEFF.IsAgeInfinite() )
        {
            sSKEFF.fAGE -= fElapsedTime;    
        }

		//	클라이언트가 아닐 경우 여기서 스킬 이펙트를 비활성화, 클라이언트는 UpdateSkillEffect()에서 함.
        if ( !bClient )
        {
            if ( !sSKEFF.IsAlive() )
            {
				UPDATE_DATA_END( i, TRUE );
                //RESETSKEFF(i);
            }
			else
			if( !m_SkillCAIndex.IsAlive(i) )
			{
				UPDATE_DATA_END( i );
			}
        }

		switch ( sSKEFF.emTYPE )
		{
		case SKILL::EMFOR_VARHP:
			//m_fVariationRateHP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARMP:
			m_fVariationRateMP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARSP:
			m_fVariationRateSP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARAP:
			//m_fVariationRateHP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			m_fVariationRateMP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			m_fVariationRateSP += (fElapsedTime/GLCONST_CHAR::fUNIT_TIME) * (sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_DEFENSE:
			m_nSUM_DEFENSE += (int) sSKEFF.fMVAR;
			break;

		case SKILL::EMFOR_HITRATE:
			m_nSUM_HIT += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_AVOIDRATE:
			m_nSUM_AVOID += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARDAMAGE:
			m_nDX_DAMAGE += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_VARDEFENSE:
			m_nSUM_DEFENSE += int(sSKEFF.fMVAR);
			break;

		case SKILL::EMFOR_RESIST:
			m_sSUMRESIST += int(sSKEFF.fMVAR);
			break;
		};


		for ( DWORD j = 0; j < sSKEFF.vecADDON.size(); ++j )
		{

			switch ( sSKEFF.vecADDON[j].emADDON )
			{
			case EMIMPACTA_HITRATE:
				m_nSUM_HIT += int(sSKEFF.vecADDON[j].fADDON_VAR);
				break;

			case EMIMPACTA_AVOIDRATE:
				m_nSUM_AVOID += int(sSKEFF.vecADDON[j].fADDON_VAR);
				break;

			case EMIMPACTA_DAMAGE:
				m_nDX_DAMAGE += int(sSKEFF.vecADDON[j].fADDON_VAR);
				break;

			case EMIMPACTA_DEFENSE:
				m_nSUM_DEFENSE += int(sSKEFF.vecADDON[j].fADDON_VAR);
				break;

			case EMIMPACTA_VARHP:
				//m_fVariationRateHP += sSKEFF.vecADDON[j].fADDON_VAR*fElapsedTime;
				break;

			case EMIMPACTA_VARMP:
				m_fVariationRateMP += sSKEFF.vecADDON[j].fADDON_VAR*fElapsedTime;
				break;

			case EMIMPACTA_VARSP:
				m_fVariationRateSP += sSKEFF.vecADDON[j].fADDON_VAR*fElapsedTime;
				break;

			case EMIMPACTA_VARAP:
				//m_fVariationRateHP += sSKEFF.vecADDON[j].fADDON_VAR*fElapsedTime;
				m_fVariationRateMP += sSKEFF.vecADDON[j].fADDON_VAR*fElapsedTime;
				m_fVariationRateSP += sSKEFF.vecADDON[j].fADDON_VAR*fElapsedTime;
				break;

			case EMIMPACTA_DAMAGE_RATE:
				m_fDamageRate += sSKEFF.vecADDON[j].fADDON_VAR;
				break;

			case EMIMPACTA_DEFENSE_RATE:
				m_fDefenseRate += sSKEFF.vecADDON[j].fADDON_VAR;
				break;

			case EMIMPACTA_RESIST:
				m_sSUMRESIST += int(sSKEFF.vecADDON[j].fADDON_VAR);
				break;
			case EMIMPACTA_STATS:
				break;
			};
		}
		
			
		m_dwHOLDBLOW = NULL;
		for ( DWORD j = 0; j < sSKEFF.vecSPEC.size(); ++j )
		{
			SKILL::SSPEC& sSPEC = sSKEFF.vecSPEC[j].sSPEC;

			m_bSkillFactsSpecial[sSKEFF.vecSPEC[j].emSPEC] = true;
			switch ( sSKEFF.vecSPEC[j].emSPEC )
			{
			case EMSPECA_ACTIVATE_DAMAGE :
				{
					if( bClient ) {
						RF_ACTIVE( m_EffSkillVarSet ).TurnOn();
					}
					else {
						RF_ACTIVE( m_EffSkillVarSet ).TurnOn( sSKEFF.sNATIVEID.dwID, sSKEFF.wLEVEL, sSPEC.nVAR1, sSPEC.nVAR2 );
					}
				}
				break;
			case EMSPECA_SPECIAL_IMMUNE :
				RF_IMMUNE( m_EffSkillVarSet ).TurnOn( !bClient ? (BYTE)sSPEC.nVAR1 : 0 );
				break;
			case EMSPECA_PROVOKE    :
				{
					WORD wValue = (WORD)sSPEC.fVAR1;

					if( wValue == NULL_DISTANCE ) wValue = 0;

					RF_PROVOKE( m_EffSkillVarSet ).TurnOn( sSKEFF.sID, wValue );
				}				
				break;
			case EMSPECA_COUNTATTACK :
				m_SkillCAIndex.SetData( SNATIVEID(sSPEC.fVAR1, sSPEC.fVAR2 ),  &sSKEFF.pcCAData, i, sSKEFF.sNATIVEID, (int)sSKEFF.wLEVEL );
				break;
			case EMSPECA_NONBLOW:
				m_dwHOLDBLOW = sSKEFF.vecSPEC[j].sSPEC.dwFLAG;
				break;

			case EMSPECA_RANGE_PSY_LONG:
				break;

			case EMSPECA_MOVEVELO:
				m_fSKILL_MOVE += sSKEFF.vecSPEC[j].sSPEC.fVAR1;
				break;

			case EMSPECA_INVISIBLE:
				m_bSkillFactsSpecial[EMSPECA_INVISIBLE] = m_sINVISIBLE.SET(sSPEC);
				//m_bINVISIBLE = true;
				break;

			case EMSPECA_RECVISIBLE:
				m_sRECVISIBLE.SET(sSPEC);
				//m_bRECVISIBLE = true;
				break;

			case EMSPECA_ATTACKVELO:
				//	공격 속도 계념이므로 ( 공격 시간을 -0.1 (-10%) 감소 시키기 위해서는 부호 반전을 하여야함. )
				m_fATTVELO -= sSKEFF.vecSPEC[j].sSPEC.fVAR1;
				break;

			case EMSPECA_PSY_DAMAGE_REDUCE:
				if ( m_sDamageSpec.m_fPsyDamageReduce < sSKEFF.vecSPEC[j].sSPEC.fVAR1 )
					m_sDamageSpec.m_fPsyDamageReduce = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
				break;

			case EMSPECA_MAGIC_DAMAGE_REDUCE:
				if ( m_sDamageSpec.m_fMagicDamageReduce < sSKEFF.vecSPEC[j].sSPEC.fVAR1 )
					m_sDamageSpec.m_fMagicDamageReduce = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
				break;

			case EMSPECA_PSY_DAMAGE_REFLECTION:
				if ( m_sDamageSpec.m_fPsyDamageReflection < sSKEFF.vecSPEC[j].sSPEC.fVAR1 )
				{
					m_sDamageSpec.m_fPsyDamageReflection = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
					m_sDamageSpec.m_fPsyDamageReflectionRate = sSKEFF.vecSPEC[j].sSPEC.fVAR2;
				}
				break;

			case EMSPECA_MAGIC_DAMAGE_REFLECTION:
				if ( m_sDamageSpec.m_fMagicDamageReflection < sSKEFF.vecSPEC[j].sSPEC.fVAR1 )
				{
					m_sDamageSpec.m_fMagicDamageReflection = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
					m_sDamageSpec.m_fMagicDamageReflectionRate = sSKEFF.vecSPEC[j].sSPEC.fVAR2;
				}
				break;
			case EMSPECA_DEFENSE_SKILL_ACTIVE:
				{
					m_sDefenseSkill.m_dwSkillID = sSKEFF.vecSPEC[j].sSPEC.dwNativeID;
					m_sDefenseSkill.m_wLevel = (WORD)sSKEFF.vecSPEC[j].sSPEC.dwFLAG;
					m_sDefenseSkill.m_fRate = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
				}
				break;
			case EMSPECA_DURATION_DAMAGE:
				{
					int nType = (int)sSKEFF.vecSPEC[j].sSPEC.dwFLAG;					
					if ( nType < 0 || nType >= EMELEMENT_MAXNUM ) continue;


					if ( sDurDamage[nType].VAILD() ) 
					{
						// 가장 강한건지 계산한다.
						float fTickDamage1 = sSKEFF.vecSPEC[j].sSPEC.fVAR2 / sSKEFF.vecSPEC[j].sSPEC.fVAR1;
						float fTickDamage2 = sDurDamage[nType].m_fDamage / sDurDamage[nType].m_fTickTime;

						if ( fTickDamage1 <= fTickDamage2 ) continue;
					}

					// fVAR1 : TickTime, fVAR2 : Damage

					sDurDamage[nType].m_fTickTime = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
					sDurDamage[nType].m_fDamage = sSKEFF.vecSPEC[j].sSPEC.fVAR2;
					sDurDamage[nType].m_sID = sSKEFF.sID;
				}
				break;
			case EMSPECA_CURSE:
				{
					if ( m_sDamageSpec.m_fDamageCurse < sSKEFF.vecSPEC[j].sSPEC.fVAR1 )
					{
						m_sDamageSpec.m_fDamageCurse = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
						m_sDamageSpec.m_sCurseTargetID = sSKEFF.sID;
					}
				}
				break;

			case EMSPECA_INCREASE_EFF:
				m_sIncreaseEff.SET(sSPEC);
				break;

			case EMSPECA_TARRANGE_ALL:
				break;
			case EMSPECA_APPLYRANGE_ALL:
				break;
			
			case EMSPECA_STUN:
				{
					m_dwActionLimit |= EMACTION_LIMIT_ITEM;
					//	다른 항목은 상태이상 기절 효과로 대체
					m_fSTATE_MOVE_RATE = 0.0f;								//	기절.
					m_bSTATE_STUN = true;
				}
				break;
			case EMSPECA_NON_DRUG:
				{
				}
				break;
			case EMSPECA_NON_SKILL:
				{
				}
				break;
			case EMSPECA_IGNORED_DAMAGE:
				{
				}
				break;
			case EMSPECA_VIEW_RANGE_RATE:
				{
					if ( sSKEFF.vecSPEC[j].sSPEC.fVAR1 < m_sRateViewRange.fRate )
						m_sRateViewRange = sSKEFF.vecSPEC[j].sSPEC.fVAR1;
				}
				break;

			case EMSPECA_STIGMA:
				{
					STARGETID sSelfID( CROW_SUMMON, m_dwGlobalID );
					m_sSTIGMA_TAKER.PushStigmaPack(sSKEFF.sID, sSelfID, sSKEFF.sNATIVEID, sSPEC);
				}
				break;

			case EMSPECA_HALLUCINATE:
				if ( m_dwNowHP )
					m_sHALLUCINATE.SET(sSPEC);	
				break;

			case EMSPECA_RELEASE_ENDURE:		
				m_sRELEASE_ENDURE.SET(sSPEC, i);
				break;

			case EMSPECA_DOMINATION:
				if ( m_dwNowHP )
					m_sDOMINATION.SET(sSPEC);
				break;
			case EMSPECA_CLONE:
				m_fDamageRate			+= sSPEC.fVAR2 * sSPEC.fVAR1;
				m_sClone.SetClonNum( static_cast<int>(sSPEC.fVAR1) );
				break;

            case EMSPECA_HP_VARIATION_RATE:
                //m_fVariationRateHP += sSPEC.fVAR1;
                break;
            case EMSPECA_MP_VARIATION_RATE:
                m_fVariationRateMP += sSPEC.fVAR1;
                break;
            case EMSPECA_SP_VARIATION_RATE:
                m_fVariationRateSP += sSPEC.fVAR1;
                break;
            case EMSPECA_HMS_VARIATION_RATE:
                //m_fVariationRateHP += sSPEC.fVAR1;
                m_fVariationRateMP += sSPEC.fVAR1;
                m_fVariationRateSP += sSPEC.fVAR1;
                break;

            case EMSPECA_MOVEVELO_LIMIT:
                {
                    if ( sSPEC.fVAR1 >= 0.0f )
                        m_fMoveVeloLimit_Min = max(m_fMoveVeloLimit_Min, sSPEC.fVAR1); // fVar1 : 최소 이속제한 (1.0 == 100%) 더 큰 값으로 갱신된다.;
                    if ( sSPEC.fVAR2 >= 0.0f )
                        m_fMoveVeloLimit_Max = min(m_fMoveVeloLimit_Max, sSPEC.fVAR2); // fVar2 : 최대 이속제한 (1.0 == 100%) 더 작은 값으로 갱신된다.;
                }
                break;
			};			
		}
	}

	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		SSTATEBLOW &sSTATEBLOW = m_sSTATEBLOWS[i];
		if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )
            continue;

		sSTATEBLOW.fAGE -= fElapsedTime;
		if ( !bClient && sSTATEBLOW.fAGE <= 0.0f )
            DISABLEBLOW(i);

		switch ( sSTATEBLOW.emBLOW )
		{
		case EMBLOW_NUMB:
			m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fSTATE_DELAY += sSTATEBLOW.fSTATE_VAR2;			//	딜래이 증가.
			break;

		case EMBLOW_STUN:
			m_fSTATE_MOVE_RATE = 0.0f;								//	기절.
			m_bSTATE_PANT = true;
			m_bSTATE_STUN = true;
			break;

		case EMBLOW_STONE:
			m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fTempVarHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);
			break;

		case EMBLOW_BURN:
			m_fTempVarHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);	//	지속타격
			break;

		case EMBLOW_FROZEN:
			m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
			m_fSTATE_DAMAGE += sSTATEBLOW.fSTATE_VAR2;			//	타격증폭.
			break;

		case EMBLOW_MAD:
			m_nSUM_HIT = 30;
			m_nSUM_AVOID = 30;
			m_bSTATE_PANT = true;
			break;

		case EMBLOW_POISON:
			m_fTempVarHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);	//	지속타격
			break;

		case EMBLOW_CURSE:
			m_fTempVarHP += (sSTATEBLOW.fSTATE_VAR2*fElapsedTime);	//	지속타격
			m_fVariationApplyRateMP = 0.0f;
			break;
		};
	}

	if ( m_pSummonCrowData->IsInVisible() )
		m_bSkillFactsSpecial[EMSPECA_INVISIBLE] = true;
	if ( m_pSummonCrowData->IsRecVisible() )
		m_bSkillFactsSpecial[EMSPECA_RECVISIBLE] = true;

	//	지속 데미지 갱신
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].IsEqual( sDurDamage[i] ) )
		{
			m_sDurDamage[i] = sDurDamage[i];
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;
		}
	}

	// Note : 지형 효과 처리
	for ( size_t i = 0; i < m_vecLandEffect.size(); ++i )	
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() ) continue;

		switch( landEffect.emLandEffectType ) 
		{
		case EMLANDEFFECT_ATK_SPEED: // 공격속도
			m_fATTVELO	  += landEffect.fValue;
			break;
		case EMLANDEFFECT_MOVE_SPEED: // 이동속도
			m_fSKILL_MOVE += landEffect.fValue;
			break;
		case EMLANDEFFECT_HP_RATE:	  // HP 증폭율
			break;
		case EMLANDEFFECT_MP_RATE:    // MP 증폭율
			break;
		case EMLANDEFFECT_RECOVER_RATE: // 회복율
			m_fVariationRateHP	  += landEffect.fValue;
			m_fVariationRateMP	  += landEffect.fValue;
			m_fVariationRateSP	  += landEffect.fValue;
			break;
		case EMLANDEFFECT_DAMAGE_RATE:	// 공격치 변화율
			m_fDamageRate += landEffect.fValue;
			break;
		case EMLANDEFFECT_DEFENSE_RATE:	// 방어치 변화율
			m_fDefenseRate += landEffect.fValue;
			break;
		case EMLANDEFFECT_RESIST_RATE:	// 저항 수치			
			m_sSUMRESIST += (int) landEffect.fValue;
			break;

		case EMLANDEFFECT_SAFE_ZONE:
			m_bSafeZone = true;
			break;
		}
	}

	//	맵 효과들
	for ( size_t i = 0; i < m_vecMapEffect.size(); ++i ) 
	{
		SMAP_EFFECT mapEffect = m_vecMapEffect[i];

		if ( mapEffect.emType == EMMAP_EFFECT_NONE ) continue;

		switch( mapEffect.emType ) 
		{
		case EMMAP_EFFECT_ATTACK_SPEED: // 공격속도
			m_fATTVELO	  += mapEffect.fRate;
			break;
		case EMMAP_EFFECT_MOVE_SPEED: // 이동속도
			m_fSKILL_MOVE += mapEffect.fRate;
			break;
		case EMMAP_EFFECT_HP_RATE:	  // HP 증폭율
			break;
		case EMMAP_EFFECT_MP_RATE:    // MP 증폭율
			break;
		case EMMAP_EFFECT_ALL_RATE: // 회복율
			m_fVariationRateHP	  += mapEffect.fRate;
			m_fVariationRateMP	  += mapEffect.fRate;
			m_fVariationRateSP	  += mapEffect.fRate;
			break;
		case EMMAP_EFFECT_DAMAGE_RATE:	// 공격치 변화율
			m_fDamageRate += mapEffect.fRate;
			break;
		case EMMAP_EFFECT_DEFENSE_RATE:	// 방어치 변화율
			m_fDefenseRate += mapEffect.fRate;
			break;
		}
	}

	m_fATTVELO += m_fSTATE_MOVE_RATE;

	//if ( m_pSummonCrowData->IsInVisible() )  m_bINVISIBLE = true;
	//if ( m_pSummonCrowData->IsRecVisible() ) m_bRECVISIBLE = true;

	//	방어력. 변화율 반영.
	m_nSUM_DEFENSE = int ( m_nSUM_DEFENSE * m_fDefenseRate );
	//	방어력이 - 값을 가지지 않게.
	if ( m_nSUM_DEFENSE < 0 )			m_nSUM_DEFENSE = 1;

	m_sSUMRESIST.LIMIT();

	//	Note : 체력 변화.	
	float fElap = (fElapsedTime/GLCONST_CHAR::fUNIT_TIME);

    m_fVariationApplyRateHP = max(0, m_fVariationApplyRateHP); // 변화 적용률은 -값을 가질 수 없다.
    m_fVariationApplyRateMP = max(0, m_fVariationApplyRateMP);
    m_fVariationApplyRateSP = max(0, m_fVariationApplyRateSP);
	GLOGICEX::UPDATE_POINT ( m_dwNowHP, m_fTempVarHP, GETMAXHP(), fElap*((GETMAXHP()*m_fVariationRateHP) * m_fVariationApplyRateHP),  0 );  // HP변화량으로 죽을 수 있다.;
	GLOGICEX::UPDATE_POINT ( m_wNowMP,  m_fTempVarMP, GETMAXMP(), fElap*((GETMAXMP()*m_fVariationRateMP) * m_fVariationApplyRateMP), 0 );
	GLOGICEX::UPDATE_POINT ( m_wNowSP,  m_fTempVarSP, GETMAXSP(), fElap*((GETMAXSP()*m_fVariationRateSP) * m_fVariationApplyRateSP), 0 );

	UPDATE_DATA_Additional( fElapsedTime );
}

void GLSUMMON::UPDATE_DATA_Additional ( const float fElapsedTime )
{
	// 강제 지정되어있는 스킬효과를 적용한다;
	for ( int i=0; i<EMSPECA_NSIZE; ++i )
	{
		m_bSkillFactsSpecial[ i ] = m_bForcedSkillFactsSpecial[ i ];
	}
}

float GLSUMMON::GETATTVELO()
{
    return GLOGICEX::GetAttackVelocity(m_fATTVELO, 0.0f);
	//return m_fATTVELO<0.0f?0.0f:m_fATTVELO;
}

float GLSUMMON::GETMOVEVELO ()
{
    return GLOGICEX::GetMoveBaseVelocity(m_fSTATE_MOVE_RATE, m_fSKILL_MOVE, 0.0f, 0.0f);
	//float fMOVE = m_fSTATE_MOVE_RATE + m_fSKILL_MOVE;
	//return fMOVE<0.0f?0.0f:fMOVE;
}

void GLSUMMON::DISABLEALLLANDEFF()
{
	if ( !m_vecLandEffect.empty() ) m_vecLandEffect.clear();
}

void GLSUMMON::ADDLANDEFF( SLANDEFFECT_VALUE& landEffect )
{
	if ( EMLANDEFFECT_MULTI > m_vecLandEffect.size() ) 
		m_vecLandEffect.push_back( landEffect );
}

void GLSUMMON::DELLANDEFF( SLANDEFFECT_VALUE& landEffect )
{
	VEC_LANDEFF_VALUE_ITER pos = std::find( m_vecLandEffect.begin(), m_vecLandEffect.end(), landEffect );
	if ( pos != m_vecLandEffect.end() ) m_vecLandEffect.erase( pos );
}

void GLSUMMON::DISABLEALLMAPEFF()
{
	if ( !m_vecMapEffect.empty() ) m_vecMapEffect.clear();
}

void GLSUMMON::ADDMAPEFF( SMAP_EFFECT& mapEffect )
{
	if ( EMLANDEFFECT_MULTI > m_vecMapEffect.size() ) 
		m_vecMapEffect.push_back( mapEffect );
}

void GLSUMMON::DELMAPEFF( SMAP_EFFECT& mapEffect )
{
	VEC_MAP_EFFECT_ITER pos = std::find( m_vecMapEffect.begin(), m_vecMapEffect.end(), mapEffect );
	if ( pos != m_vecMapEffect.end() ) m_vecMapEffect.erase( pos );
}

bool GLSUMMON::VALIDACTION()
{
    DWORD dwSize = (DWORD) m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
    for (DWORD i=0; i<dwSize; ++i)
    {
        if (m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[i].bUsed)
            return true;
    }
    return true;
}

bool GLSUMMON::VALIDSKILL ()
{
    DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
    for (DWORD i=0; i<dwSize; ++i)
    {
        if (m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[i].bUsed && 
            m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[i].skill_id!=SNATIVEID(false))
            return true;
    }
    return true;
}

bool GLSUMMON::VALIDATTACK ()
{
    DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
    for (DWORD i=0; i<dwSize; ++i)
    {
        if (m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[i].bUsed && 
            m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[i].skill_id==SNATIVEID(false))
            return true;
    }
    return true;
}

const SUMMON_TYPE GLSUMMON::GETSUMMONTYPE() const
{
	return m_emTYPE;
}

WORD GLSUMMON::GETSUMMON_BODYRADIUS() const
{ 
    return m_pSummonCrowData->m_sAction.m_wBodyRadius;
}

WORD GLSUMMON::GETSUMMON_ATTACKRANGE() const
{ 
	DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
	if ( m_dwAType >= dwSize || m_dwAType < 0 )
		return 0;

    return m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType].wRange;
}

EMCROW GLSUMMON::GETCROW() const
{ 
    return CROW_SUMMON;
}

WORD GLSUMMON::GETLEVEL() const
{ 
    return m_pSummonCrowData->m_wLevel;
}

DWORD GLSUMMON::GETHP() const
{ 
    return m_dwNowHP;
}

DWORD GLSUMMON::GETMAXHP() const
{ 
    return m_pSummonCrowData->m_dwHP;
}

float GLSUMMON::GETMAXHPRATE() const
{
    return m_sLINKHP.fRef;
}

void GLSUMMON::SETMAXHPRATE(float fRate) 
{
    m_sLINKHP.fRef = fRate;
}

int GLSUMMON::GETMP() const
{ 
    return m_wNowMP;
}

int GLSUMMON::GETMAXMP() const
{ 
    return m_pSummonCrowData->m_wMP;
}

int GLSUMMON::GETSP() const
{ 
    return m_wNowSP;
}

int GLSUMMON::GETMAXSP() const
{ 
    return m_pSummonCrowData->m_wSP;
}

int GLSUMMON::GETHIT() const
{ 
    return m_nSUM_HIT;
}

int GLSUMMON::GETAVOID() const
{ 
    return m_nSUM_AVOID;
}

EMBRIGHT GLSUMMON::GETBRIGHT() const
{ 
    return m_pSummonCrowData->m_emBright;
}

int GLSUMMON::GETDEFENSE() const
{ 
    return m_nSUM_DEFENSE;
}

int GLSUMMON::GETSUM_PIERCE() const
{ 
    return m_nSUM_PIERCE;
}

WORD GLSUMMON::GETBODYRADIUS() const
{ 
    return m_pSummonCrowData->m_sAction.m_wBodyRadius;
}

WORD GLSUMMON::GETATTACKRANGE() const
{ 
	DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
	if ( m_dwAType >= dwSize || m_dwAType < 0 )
		return 0;

	return m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType].wRange;
}

void GLSUMMON::SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage)
{
	m_sIgnoredDamage.nIgnoreCount = nIgnoreCount;
	m_sIgnoredDamage.nIgnoreDamage = nIgnoreDamage;
}

void GLSUMMON::Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID )
{
	RF_ACTIVE( m_EffSkillVarSet ).Accumulate( dwDamage, eCrow, dwID );
}

const SCROWATTACK* GLSUMMON::GetCrowAttack()
{
	DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
	if ( m_dwAType >= dwSize || m_dwAType < 0 )
		return NULL;

	return &m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType];
}

void GLSUMMON::RemoveSkillFactBySkillID ( const SNATIVEID& skill_id )
{
	for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if ( sSKEFF.bEnable == false )
			continue;		

		if ( sSKEFF.sNATIVEID != skill_id )
			continue;

		UPDATE_DATA_END(CROW_SKILLFACT_SIZE - _i);
	}
}

void GLSUMMON::RemoveSkillFactBySpecial ( const EMSPEC_ADDON emSPEC_ADDON )
{
	for (size_t i = CROW_SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		const size_t SpecSize = sSKEFF.vecSPEC.size();
		for (size_t j = SpecSize; j ; --j)
		{
			if (sSKEFF.vecSPEC[SpecSize - j].emSPEC == emSPEC_ADDON)
			{
				UPDATE_DATA_END(CROW_SKILLFACT_SIZE - i);
				break;
			}
		}
	}
}

void GLSUMMON::RemoveSkillFactofSpecial ( const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON )
{
	for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if ( sSKEFF.bEnable == false )
			continue;		

		if ( sSKEFF.sNATIVEID != skill_id )
			continue;

		std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iter;

		for( int i = 0; i < SKILL::MAX_ADDON; ++i )
		{
			_iter = sSKEFF.vecSPEC.begin();

			while( _iter != sSKEFF.vecSPEC.end() )
			{
				if( _iter->emSPEC == emSPEC_ADDON[i] )
				{
					sSKEFF.vecSPEC.erase(_iter);
				}

				++_iter;
			}
		}

		return;
	}
}

namespace COMMENT
{
	std::string SUMMON_TYPE[SUMMON_TYPE_SIZE] = { "소환수_일반(미사용)", "소환수_공격", "소환수_자폭", "소환수_버프", "소환수_통로1", "소환수_통로2", "소환수_NPC" };
};
