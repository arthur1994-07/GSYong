#include "pch.h"
#include "./GLogicEx.h"
#include "./Item/GLItemMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


GLCROWLOGIC::GLCROWLOGIC(void)
: m_dwGlobalID(0)
, m_sNativeID(0,0)
, m_pCrowData(NULL)

, m_dwHOLDBLOW(NULL)
, m_fSTATE_MOVE_RATE(1.0f)
, m_fSTATE_DELAY(1.0f)
, m_fSTATE_DAMAGE(1.0f)

, m_bSTATE_PANT(false)
, m_bSTATE_STUN(false)
, m_fATTVELO(0.0f)
, m_fSKILL_MOVE(0.0f)

, m_fHPRestorationRate( 0.f )
, m_fMPRestorationRate( 0.f )
, m_fSPRestorationRate( 0.f )
, m_fHPRestorationRatePlus( 0.f )
, m_fMPRestorationRatePlus( 0.f )
, m_fSPRestorationRatePlus( 0.f )

, m_nSUM_HIT(0)
, m_nSUM_AVOID(0)

, m_nSUM_DEFENSE(0)
, m_nDX_DAMAGE(0)

, m_nSUM_PIERCE(0) 

, m_fDamageRate(1.0f)
, m_fDefenseRate(1.0f)

, m_dwAType(0)

, m_dwNowHP(0)
, m_wNowMP(0)
, m_wNowSP(0)

, m_idACTIVESKILL(false)
, m_wACTIVESKILL_LVL(0)

, m_fTempVarHP(0.0f)
, m_fTempVarMP(0.0f)
, m_fTempVarSP(0.0f)
, m_wSchoolAuthed(SCHOOL_NONE)
, m_fMultipleHP(1.0f)
, m_fMoveVeloLimit_Min(-1.0f)
, m_fMoveVeloLimit_Max(1000.0f)
, m_fInfluenceY(0.f)
{
	for ( int i=0; i< SCROWDATA::EMMAXATTACK; ++i )
		m_fACTIONDELAY[i] = 0.0f;

	ZeroMemory(m_arrHP4School, sizeof(m_arrHP4School));
	ZeroMemory(m_arrIncHp4School, sizeof(m_arrIncHp4School));
	ZeroMemory(m_bSkillFactsSpecial, sizeof(m_bSkillFactsSpecial));
}

void GLCROWLOGIC::RESET_DATA ()
{
	int i(0);

	m_sNativeID = SNATIVEID(0,0);
	m_pCrowData = NULL;

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

	m_fATTVELO = 0.0f;
	m_fSKILL_MOVE = 0.0f;

	m_fHPRestorationRate = 0.f;
	m_fMPRestorationRate = 0.f;
	m_fSPRestorationRate = 0.f;

	m_nSUM_HIT = 0;
	m_nSUM_AVOID = 0;

	m_nSUM_DEFENSE = 0;
	m_nDX_DAMAGE = 0;

	m_nSUM_PIERCE = 0;

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_dwAType = 0;
	
	m_dwNowHP = 0;
	m_wNowMP = 0;
	m_wNowSP = 0;

	m_idACTIVESKILL = false;
	m_wACTIVESKILL_LVL = 0;

	m_fTempVarHP = 0.0f;
	m_fTempVarMP = 0.0f;
	m_fTempVarSP = 0.0f;
    m_fMultipleHP = 1.0f;

	m_sDamageSpec.RESET();	
	m_sDefenseSkill.RESET();
	m_sSUMRESIST.RESET();
	m_sRateViewRange.RESET();

	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
		m_sDurDamage[i].RESET();

	m_dwActionLimit = EMACTION_LIMIT_NONE;

	//
	//mjeon.CaptureTheField
	//
	ZeroMemory(m_arrHP4School, sizeof(m_arrHP4School));
    ZeroMemory(m_arrIncHp4School, sizeof(m_arrIncHp4School));
	ZeroMemory(m_bSkillFactsSpecial, sizeof(m_bSkillFactsSpecial));

	m_wSchoolAuthed = SCHOOL_NONE;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
	m_fInfluenceY = 0.0f;
}

const char* GLCROWLOGIC::GETNAME ()
{
	GASSERT(m_pCrowData);
	return m_pCrowData->GetName();
}

const std::string GLCROWLOGIC::GetNameStr() const
{
    if (m_pCrowData)
    {
        return m_pCrowData->GetNameStr();
    }
    else
    {
        static std::string TempStr("");
        return TempStr;
    }
}

int GLCROWLOGIC::GETFORCE_LOW () const
{
	int nDamage = m_pCrowData->m_vecCrowAttack[m_dwAType].sDamage.wLow;
	
	if ((nDamage+m_nDX_DAMAGE) < 0)
        return 0;
    else
	    return nDamage + m_nDX_DAMAGE;
}

int GLCROWLOGIC::GETFORCE_HIGH () const
{
	int nDamage = m_pCrowData->m_vecCrowAttack[m_dwAType].sDamage.wHigh;

	if ((nDamage+m_nDX_DAMAGE) < 0)
        return 0;
    else
	    return nDamage + m_nDX_DAMAGE;
}


WORD GLCROWLOGIC::GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const
{
	int nRANGE = sSKILL.m_sBASIC.wTARRANGE;
	
	// 몬스터는 적용받지 않는다.
//	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		nRANGE += (int) GETSUM_RANGE_PSY_LONG();

//	if ( sSKILL.m_sBASIC.emIMPACT_SIDE==SIDE_ENEMY )	nRANGE += (int) GETSUM_TARRANGE_ALL();

//	if ( nRANGE < 0 ) nRANGE = 0;

	return (WORD)nRANGE;
}

WORD GLCROWLOGIC::GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const
{
	const SKILL::CDATA_LVL &sDATA_LVL = sSKILL.m_sAPPLY.sDATA_LVL[dwLEVEL];

	int nRANGE = sDATA_LVL.wAPPLYRANGE;

	// 몬스터는 적용받지 않는다.
//	if ( sSKILL.m_sBASIC.emAPPLY==SKILL::EMAPPLY_PHY_LONG )		nRANGE += (int) GETSUM_RANGE_PSY_LONG();

//	if ( sSKILL.m_sBASIC.emIMPACT_SIDE==SIDE_ENEMY )	nRANGE += (int) GETSUM_APPLYRANGE_ALL();
	
//	if ( nRANGE < 0 ) nRANGE = 0;

	return (WORD)nRANGE;
}

BOOL GLCROWLOGIC::INIT_DATA ()
{
	m_sNativeID = m_pCrowData->sNativeID;
	m_dwNowHP = GETMAXHP();
	m_wNowMP = m_pCrowData->m_wMP;

	//
	//mjeon.CaptureTheField
	//
	for (int i=0; i<SCHOOL_NUM; i++)
		m_arrHP4School[i] = GETMAXHP();

    for (int i=0; i<SCHOOL_NUM; i++)
        m_arrIncHp4School[i] = m_pCrowData->m_fRegenHP;

	m_wSchoolAuthed = SCHOOL_NONE;
	
	if ( m_pCrowData->IsInVisible() )
		m_bSkillFactsSpecial[EMSPECA_INVISIBLE] = true;
	if ( m_pCrowData->IsRecVisible() )
		m_bSkillFactsSpecial[EMSPECA_RECVISIBLE] = true;

	return TRUE;
}

DWORD GLCROWLOGIC::RECEIVE_DAMAGE ( const int nDamage )
{
	DWORD dwOLD = m_dwNowHP;

	DECREASE(m_dwNowHP, DWORD(nDamage));

	RECEIVE_DAMAGE_AFTER();
	GLCROWLOGIC::RELEASE_ENDURE_PROC();	
	
	return (dwOLD>m_dwNowHP) ? (dwOLD-m_dwNowHP) : 0;
}

void GLCROWLOGIC::RELEASE_ENDURE_PROC(void)
{
	const DWORD nRELEASE_ENDURE = m_sRELEASE_ENDURE.nRELEASE_ENDURE;
	if ( nRELEASE_ENDURE )
	{ // 피격시 스킬 효과 해제 스킬이 걸려있었다면 처리;  // 일반 공격 피해;
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

DWORD GLCROWLOGIC::RECEIVE_HEAL(const WORD wHeal)
{
	const DWORD dwOLD = m_dwNowHP;
	m_dwNowHP += DWORD(wHeal);

	if ( m_dwNowHP > GETMAXHP() )
	{
		m_dwNowHP = GETMAXHP();
		return m_dwNowHP - dwOLD;
	}
	else
    {
		return DWORD(wHeal);
    }
}

const bool GLCROWLOGIC::IS_HAVE_SKILLFACT(const SNATIVEID& skill_id, OUT WORD& wOutSlot)
{
    if ( skill_id == SNATIVEID(false) )
    {
        return false;
    }

    for (size_t i=0; i<CROW_SKILLFACT_SIZE; ++i)
    {
        if ( m_sSKILLFACT[i].sNATIVEID == skill_id )
        {
            wOutSlot = (WORD)i;
            return true;
        }
    } 
    return false;
}

const bool GLCROWLOGIC::IS_HAVE_TYPES_IN_SKILLFACT(const EMIMPACT_ADDON emIMPACT_ADDON){
	for (size_t i = CROW_SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		const size_t ADDONSize = sSKEFF.vecADDON.size();
		for (size_t j = ADDONSize; j ; --j)
		{			
			if (sSKEFF.vecADDON[ADDONSize - j].emADDON == emIMPACT_ADDON )
				return true;
		}
	}
	return false;
}


const bool GLCROWLOGIC::IS_HAVE_SPEC_IN_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON){	
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
				return true;
		}
	}
	return false;
}

const bool GLCROWLOGIC::IS_HAVE_STYPES_IN_SKILLFACT(const SKILL::EMSPECIALSKILLTYPE emSSTYPE_ADDON){
	for (size_t i = CROW_SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;

		if ( sSKEFF.dwSpecialSkill == emSSTYPE_ADDON)
			return true;
	}
	return false;
}

const bool GLCROWLOGIC::IS_HAVE_SKILL_IN_SKILLFACT( const SNATIVEID& sNativeID )
{
	for (size_t i = SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		if( sSKEFF.sNATIVEID == sNativeID )
			return true;

	}
	return false;
}

//
//mjeon.CaptureTheField
//
DWORD GLCROWLOGIC::RECEIVE_DAMAGE_BY_SCHOOL ( const int nDamage, const WORD wSchool )
{
	GASSERT(wSchool < SCHOOL_NUM);

	DWORD dwOLD = m_arrHP4School[wSchool];

	DECREASE(m_arrHP4School[wSchool], DWORD(nDamage));
	
	return (dwOLD>m_arrHP4School[wSchool]) ? (dwOLD-m_arrHP4School[wSchool]) : 0;
}

void GLCROWLOGIC::PrevUPDATE_DATA(const float fElapsedTime)
{
	ZeroMemory(m_bSkillFactsSpecial, sizeof(m_bSkillFactsSpecial));

	m_fHPRestorationRate = m_pCrowData->m_fRegenHP;
	m_fMPRestorationRate = m_pCrowData->m_fRegenMP;
	m_fSPRestorationRate = m_pCrowData->m_fRegenSP;

	m_nSUM_HIT = static_cast<int>(m_pCrowData->m_wHitRate);
	m_nSUM_AVOID = static_cast<int>(m_pCrowData->m_wAvoidRate);

	m_nSUM_DEFENSE = static_cast<int>(m_pCrowData->m_wDefense);

	m_fSTATE_MOVE_RATE = 1.0f;
	m_fSTATE_DELAY = 1.0f;

	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;		

	m_fATTVELO = 0.0f;
	m_fSKILL_MOVE = 0.0f;

	m_nDX_DAMAGE = 0;

	m_nSUM_PIERCE = 0;

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;

	m_sDamageSpec.RESET();	
	m_sDefenseSkill.RESET();
	m_sINVISIBLE.RESET();	
	m_sRECVISIBLE.RESET();
	m_sRELEASE_ENDURE.RESET();
	m_sHALLUCINATE.RESET();
	m_sDOMINATION.RESET();
	m_sSTIGMA_TAKER.RESET();	
	m_sIncreaseEff.RESET();	
	m_sLINKHP.RESET();
	m_sRateViewRange.RESET();	

	m_sSUMRESIST = m_pCrowData->m_sResist;

	m_dwActionLimit = EMACTION_LIMIT_NONE;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

	m_SkillCAIndex.ResetData();

	RF_ENTRANCE_EXIT( m_EffSkillVarSet ).ResetValue();
	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
	RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
	RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
}

void GLCROWLOGIC::UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient )
{
    // 회복률 개선 [11/27/2014 hsshin]
    // 란 태초부터 회복률이 제대로 동작하지 않고 있었음;
    // 개선하기 위해서는 현재 몬스터의 밸런스작업과 함께 해야해서 스킬효과로 인한 변화율효과는 적용하지 않고
    // 변화량 효과만 적용하고 몹데이터의 회복률을 제대로 적용받도록 수정;
    // 2015년초 기획팀과 함께 개선 예정이나 그 이후 시점에 이 코드가 개선이 없다면 필히 개선요망;

    float fVariationHP(0.0f);   // HP 변화량 (실 수치로 적용됨);
    float fVariationMP(0.0f);   // MP 회복량;
    float fVariationSP(0.0f);   // SP 회복량;

   // float m_fHPRestorationRate(m_pCrowData->m_fRegenHP); // HP 변화율 (전체 수치에 비례하여 %로 적용됨);
   // float m_fMPRestorationRate(m_pCrowData->m_fRegenMP); // MP 회복율;
   // float m_fSPRestorationRate(m_pCrowData->m_fRegenSP); // SP 회복율;

    float fVariationApplyRateHP(1.0f);    // HP 변화 적용률 ( 변화을 적용하는 비율 );
    float fVariationApplyRateMP(1.0f);    // MP 변화 적용률;
    float fVariationApplyRateSP(1.0f);    // SP 변화 적용률;

	PrevUPDATE_DATA(fElapsedTime);

	SDURATION_DAMAGE	sDurDamage[EMELEMENT_MAXNUM];	//	임시 객체	

	for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )
			continue;
		if ( !sSKEFF.bEnable )
			continue;

        if ( !sSKEFF.IsAgeInfinite() )
        {
            sSKEFF.fAGE -= fElapsedTime;    
        }

		//	클라이언트가 아닐 경우 여기서 스킬 이펙트를 비활성화, 클라이언트는 UpdateSkillEffect()에서 함.
		if ( !bClient )
        {
            if ( !sSKEFF.IsAlive())			
				UPDATE_DATA_END(i, TRUE);
			else
			if( !m_SkillCAIndex.IsAlive(i) )
				UPDATE_DATA_END(i);
        }

		switch ( sSKEFF.emTYPE )
		{
		case SKILL::EMFOR_VARHP:
            //fVariationRateHP += sSKEFF.fMVAR;
			break;

		case SKILL::EMFOR_VARMP:
			m_fMPRestorationRate += sSKEFF.fMVAR;
			break;

		case SKILL::EMFOR_VARSP:
			m_fSPRestorationRate += sSKEFF.fMVAR;
			break;

        case SKILL::EMFOR_VARAP:
            //fVariationRateHP += sSKEFF.fMVAR;
            m_fMPRestorationRate += sSKEFF.fMVAR;
            m_fSPRestorationRate += sSKEFF.fMVAR;
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
                //fVariationRateHP += sSKEFF.vecADDON[j].fADDON_VAR;
				break;

			case EMIMPACTA_VARMP:
                m_fMPRestorationRate += sSKEFF.vecADDON[j].fADDON_VAR;
				break;

			case EMIMPACTA_VARSP:
				m_fSPRestorationRate += sSKEFF.vecADDON[j].fADDON_VAR;
				break;

            case EMIMPACTA_VARAP:
                //fVariationRateHP += sSKEFF.vecADDON[j].fADDON_VAR;
                m_fMPRestorationRate += sSKEFF.vecADDON[j].fADDON_VAR;
                m_fSPRestorationRate += sSKEFF.vecADDON[j].fADDON_VAR;
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
			case EMSPECA_FEIGN_DEATH   :
				RF_FEIGN_DEATH( m_EffSkillVarSet ).TurnOn( sSPEC.dwFLAG&FEIGN_DEATH_TARGETING_DISABLE, sSPEC.dwFLAG&FEIGN_DEATH_BUF_DEL, i );
				break;
			case EMSPECA_ENTRANCE_EXIT :
				RF_ENTRANCE_EXIT( m_EffSkillVarSet ).TurnOn();
				break;
			case EMSPECA_COUNTATTACK :
				m_SkillCAIndex.SetData( SNATIVEID(sSPEC.fVAR1, sSPEC.fVAR2 ), &sSKEFF.pcCAData, i, sSKEFF.sNATIVEID, (int)sSKEFF.wLEVEL );
				break;
			case EMSPECA_NONBLOW:
				m_dwHOLDBLOW = sSPEC.dwFLAG;
				break;

			case EMSPECA_MOVEVELO:
				m_fSKILL_MOVE += sSPEC.fVAR1;
				break;

			case EMSPECA_INVISIBLE:
				m_bSkillFactsSpecial[EMSPECA_INVISIBLE] = m_sINVISIBLE.SET(sSPEC);
				break;

			case EMSPECA_RECVISIBLE:	
				m_sRECVISIBLE.SET(sSPEC);
				break;

			case EMSPECA_ATTACKVELO:
				//	공격 속도 계념이므로 ( 공격 시간을 -0.1 (-10%) 감소 시키기 위해서는 부호 반전을 하여야함. )
				m_fATTVELO -= sSPEC.fVAR1;
				break;
			
			case EMSPECA_PSY_DAMAGE_REDUCE:
				if ( m_sDamageSpec.m_fPsyDamageReduce < sSPEC.fVAR1 )
					m_sDamageSpec.m_fPsyDamageReduce = sSPEC.fVAR1;
				break;
			
			case EMSPECA_MAGIC_DAMAGE_REDUCE:
				if ( m_sDamageSpec.m_fMagicDamageReduce < sSPEC.fVAR1 )
					m_sDamageSpec.m_fMagicDamageReduce = sSPEC.fVAR1;
				break;
			
			case EMSPECA_PSY_DAMAGE_REFLECTION:
				if ( m_sDamageSpec.m_fPsyDamageReflection < sSPEC.fVAR1 )
				{
					m_sDamageSpec.m_fPsyDamageReflection = sSPEC.fVAR1;
					m_sDamageSpec.m_fPsyDamageReflectionRate = sSPEC.fVAR2;
				}
				break;
			
			case EMSPECA_MAGIC_DAMAGE_REFLECTION:
				if ( m_sDamageSpec.m_fMagicDamageReflection < sSPEC.fVAR1 )
				{
					m_sDamageSpec.m_fMagicDamageReflection = sSPEC.fVAR1;
					m_sDamageSpec.m_fMagicDamageReflectionRate = sSPEC.fVAR2;
				}
				break;
			case EMSPECA_DEFENSE_SKILL_ACTIVE:
				{
					m_sDefenseSkill.m_dwSkillID = sSPEC.dwNativeID;
					m_sDefenseSkill.m_wLevel = (WORD)sSPEC.dwFLAG;
					m_sDefenseSkill.m_fRate = sSPEC.fVAR1;
				}
				break;
			case EMSPECA_DURATION_DAMAGE:
				{
					//	fVAR1 = 틱Time, fVAR2 = 데미지
					int nType = (int)sSPEC.dwFLAG;					
					if ( nType < 0 || nType >= EMELEMENT_MAXNUM )
						continue;

					// fVAR1 : TickTime, fVAR2 : Damage
					sDurDamage[nType].m_fTickTime = sSPEC.fVAR1;
					sDurDamage[nType].m_fDamage += sSPEC.fVAR2;
					sDurDamage[nType].m_sID = sSKEFF.sID;
				}
				break;
			case EMSPECA_CURSE:
				{
					if ( m_sDamageSpec.m_fDamageCurse < sSPEC.fVAR1 )
					{
						m_sDamageSpec.m_fDamageCurse = sSPEC.fVAR1;				
						m_sDamageSpec.m_sCurseTargetID = sSKEFF.sID;
					}
				}
				break;
			case EMSPECA_INCREASE_EFF:
				m_sIncreaseEff.SET(sSPEC);
				break;

			//	몹은 효과 없음
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
			case EMSPECA_RELEASE_ENDURE:		
				m_sRELEASE_ENDURE.SET(sSPEC, i);
				break;
			case EMSPECA_IMMUNE:
				m_sDamageSpec.m_emImmuneApplyType |= DWORD(sSPEC.fVAR1);
				m_sDamageSpec.m_emImmuneActionType |= DWORD(sSPEC.fVAR2);
				break;
			case EMSPECA_STIGMA:
				{
					STARGETID sSelfID(CROW_MOB, m_dwGlobalID);
					m_sSTIGMA_TAKER.PushStigmaPack(sSKEFF.sID, sSelfID, sSKEFF.sNATIVEID, sSPEC);
				}
				break;
			case EMSPECA_HALLUCINATE:
				if ( m_dwNowHP )
					m_sHALLUCINATE.SET(sSPEC);	
				break;
			case EMSPECA_DOMINATION:
				if ( m_dwNowHP )
					m_sDOMINATION.SET(sSPEC);
				break;
            case EMSPECA_LINK_HP:
                {
                    if ( static_cast<int>(sSPEC.fVAR2) == 1 )
                    {
                        if ( sSPEC.fVAR1 < m_sLINKHP.fTake )
                            m_sLINKHP.fTake = sSPEC.fVAR1;
                    }
                }
                break;
			case EMSPECA_ILLUSION:
				m_sDamageSpec.m_dwFlags |= DAMAGE_SPEC::FLAG_ILLUSION;
				break;

			case EMSPECA_DAMAGE_VALUE: 
				m_nDX_DAMAGE += static_cast<int>(sSPEC.fVAR1);
				break;
			case EMSPECA_DAMAGE_RATE: 
				m_fDamageRate			+= sSPEC.fVAR1;
				break;

			case EMSPECA_DEFENSE_VALUE:
				m_nSUM_DEFENSE		+= static_cast<int>(sSPEC.fVAR1);
				break;
			case EMSPECA_DEFENSE_RATE: 
				m_fDefenseRate			+= sSPEC.fVAR1;
				break;

			case EMSPECA_HIT_VALUE: 
				m_nSUM_HIT				+= static_cast<int>( sSPEC.fVAR1 );
				break;			

			case EMSPECA_AVOID_VALUE: 
				m_nSUM_AVOID			+= static_cast<int>( sSPEC.fVAR1 );
				break;		
			
			case EMSPECA_MOVEVELO_VALUE:
				m_fSKILL_MOVE += sSPEC.fVAR1;
				break;			

            case EMSPECA_HP_VARIATION_RATE:
                //fVariationRateHP += sSPEC.fVAR1;
                break;
            case EMSPECA_MP_VARIATION_RATE:
                m_fMPRestorationRate += sSPEC.fVAR1;
                break;
            case EMSPECA_SP_VARIATION_RATE:
                m_fSPRestorationRate += sSPEC.fVAR1;
                break;
            case EMSPECA_HMS_VARIATION_RATE:
                //fVariationRateHP += sSPEC.fVAR1;
                m_fMPRestorationRate += sSPEC.fVAR1;
                m_fSPRestorationRate += sSPEC.fVAR1;
                break;

			case EMSPECA_VIEW_RANGE_RATE:
				if ( sSPEC.fVAR1 < m_sRateViewRange.fRate )
					m_sRateViewRange = sSPEC.fVAR1;
				break;
			case EMSPECA_AIRBORNE:
				// RECEIVE_SKILLFACT()에서 이미 처리 되었으므로,
				// PrevUPDATE_DATA()에서 갱신만 해준다;
				break;
			case EMSPECA_DELAY_ACTION_SKILL:
				break;
			case EMSPECA_CLONE:
				m_fDamageRate			+= sSPEC.fVAR2 * sSPEC.fVAR1;
				m_sClone.SetClonNum( static_cast<int>(sSPEC.fVAR1) );
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
		if ( !bClient && sSTATEBLOW.fAGE <= 0.0f )	DISABLEBLOW(i);

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
            fVariationHP += sSTATEBLOW.fSTATE_VAR2;
			break;

        case EMBLOW_BURN:
            fVariationHP += sSTATEBLOW.fSTATE_VAR2;
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
            fVariationHP += sSTATEBLOW.fSTATE_VAR2;
			break;

        case EMBLOW_CURSE:
            fVariationHP += sSTATEBLOW.fSTATE_VAR2;
			fVariationApplyRateMP = 0.0f;
			break;
		};
	}

	//	지속 데미지 갱신
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].IsEqual( sDurDamage[i] ) )
		{
			m_sDurDamage[i] = sDurDamage[i];
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;
		}
	}

    m_fATTVELO += m_fSTATE_MOVE_RATE;

	//	방어력. 변화율 반영.
	m_nSUM_DEFENSE = int ( m_nSUM_DEFENSE * m_fDefenseRate );
	//	방어력이 - 값을 가지지 않게.
	if ( m_nSUM_DEFENSE < 0 )			m_nSUM_DEFENSE = 1;

	m_sSUMRESIST.LIMIT();	
	if ( m_pCrowData->IsInVisible() )
		m_bSkillFactsSpecial[EMSPECA_INVISIBLE] = true;
	if ( m_pCrowData->IsRecVisible() )
		m_bSkillFactsSpecial[EMSPECA_RECVISIBLE] = true;

	//	Note : 체력 변화.
	//
	float fElap = (fElapsedTime / GLCONST_CHAR::fUNIT_TIME);

	//
	//mjeon.CaptureTheField
	//
	//  damaged by a skill
	if (m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
	{
		for (WORD i=0; i<SCHOOL_NUM; i++)
		{
			if (i != m_wSchoolAuthed)
			{
                // 점령전 인증기는 인증되지 않은 학원에 한하여 자연 회복력만 계산;
				GLOGICEX::UPDATE_POINT ( m_arrHP4School[i], m_arrIncHp4School[i], GETMAXHP(), fElap * (GETMAXHP() * m_pCrowData->m_fRegenHP), 1 );
			}
		}
	}
	else
	{
        m_fTempVarHP += fVariationHP * fElapsedTime;
        m_fTempVarMP += fVariationMP * fElapsedTime;
        m_fTempVarSP += fVariationSP * fElapsedTime;

        fVariationApplyRateHP = max(0, fVariationApplyRateHP);
        fVariationApplyRateMP = max(0, fVariationApplyRateMP);
        fVariationApplyRateSP = max(0, fVariationApplyRateSP);
        GLOGICEX::UPDATE_POINT ( m_dwNowHP, m_fTempVarHP, GETMAXHP(), fElap * (GETMAXHP() * ( m_fHPRestorationRate + m_fHPRestorationRatePlus ) * fVariationApplyRateHP), 0 ); // 체력 변화량에 의해서 죽을 수도 있다!;
        GLOGICEX::UPDATE_POINT ( m_wNowMP, m_fTempVarMP, GETMAXMP(), fElap * (GETMAXMP() * ( m_fMPRestorationRate + m_fMPRestorationRatePlus ) * fVariationApplyRateMP), 0 );
        GLOGICEX::UPDATE_POINT ( m_wNowSP, m_fTempVarSP, GETMAXSP(), fElap * (GETMAXSP() * ( m_fSPRestorationRate + m_fSPRestorationRatePlus ) * fVariationApplyRateSP), 0 );
	}

    //! 스킬 고리효과 비율 적용
    if ( m_dwNowHP > (GETMAXHP() * m_sLINKHP.fTake) )
        m_dwNowHP = GETMAXHP() * m_sLINKHP.fTake;

	UPDATE_DATA_Additional(fElapsedTime);
}

float GLCROWLOGIC::GETATTVELO()
{
    return GLOGICEX::GetAttackVelocity(m_fATTVELO, 0.0f);
	//return m_fATTVELO<0.0f?0.0f:m_fATTVELO;
}

float GLCROWLOGIC::GETMOVEVELO()
{
    return GLOGICEX::GetMoveBaseVelocity(m_fSTATE_MOVE_RATE, m_fSKILL_MOVE, 0.0f, 0.0f);
	//float fMOVE = m_fSTATE_MOVE_RATE + m_fSKILL_MOVE;
	//return fMOVE<0.0f?0.0f:fMOVE;
}

float GLCROWLOGIC::GETATT_TOTAL()
{
    return GETATTVELO();
}

float GLCROWLOGIC::GETMOVE_TOTAL()
{
    //float _fMoveVelo = GETMOVE_TOTAL();	// 이 코드 정상인가~?

	float _fMoveVelo = GETMOVEVELO();

    if (m_fMoveVeloLimit_Min > 0.0f)
        _fMoveVelo = max(_fMoveVelo, m_fMoveVeloLimit_Min);

    _fMoveVelo = min(_fMoveVelo, m_fMoveVeloLimit_Max);

    return _fMoveVelo;
}

void GLCROWLOGIC::SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel )
{
	m_idACTIVESKILL = skill_id;
	m_wACTIVESKILL_LVL = wLevel;
}

EMSKILLCHECK GLCROWLOGIC::CHECHSKILL ( DWORD dwAType,  bool bNotLearn )
{
	GASSERT(dwAType<m_pCrowData->m_vecCrowAttack.size());

	SNATIVEID skill_id;
	WORD wskill_lev = 0;
	WORD wStrikeNum = 0;
	if ( bNotLearn )
	{
		skill_id = m_sDefenseSkill.m_dwSkillID;
		wskill_lev = m_sDefenseSkill.m_wLevel;
	}
	else
	{
        const SCROWATTACK &sATTACK = m_pCrowData->m_vecCrowAttack[dwAType];
		skill_id = sATTACK.skill_id;
		wskill_lev = sATTACK.skill_lev;

		//	Note : 스킬 딜래이 타임이 지낫는지 점검.
		//
		if ( m_fACTIONDELAY[dwAType] > 0.0f )	
			return EMSKILL_DELAYTIME; // 스킬 딜래이 시간이 지나지 않음.
	}




	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if (!pSkill)
        return EMSKILL_FAILED;
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

void GLCROWLOGIC::ACCOUNTSKILL ( WORD wStrikeNum )
{
	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[m_wACTIVESKILL_LVL];

	//	스킬 딜래이 등록.
	float fDelayTime = GLOGICEX::SKILLDELAY(pSkill->m_sBASIC.dwGRADE,m_wACTIVESKILL_LVL,GETLEVEL(),sSKILL_DATA.fDELAYTIME);
	
	//	Note : 상태이상의 딜래이 감안.
	m_fACTIONDELAY[m_dwAType] = fDelayTime * m_fSTATE_DELAY;

	//	기초 체력 소모.
	DECREASE ( m_dwNowHP, DWORD(sSKILL_DATA.wUSE_HP*wStrikeNum) );
	DECREASE ( m_wNowMP, WORD(sSKILL_DATA.wUSE_MP*wStrikeNum) );
}

bool GLCROWLOGIC::ISNPC_INTERACTIONABLE(
    const D3DXVECTOR3& vReqPos,
    const D3DXVECTOR3& vCurPos,
    const float fReqRadius,
    int nNpcAction) const
{
    if (!m_pCrowData)
        return false;

    if (GETCROW() != CROW_NPC)
        return false;

    if (nNpcAction != SNpcTalk::EM_BASIC_NULL && !m_pCrowData->IsBasicTalk(nNpcAction))
        return false;

    float fLength = GLCONST_CHAR::fNPC_INTERACTION_ADJUST + GET_INTERACTION_RANGE() + fReqRadius + GETBODYRADIUS();
    if (D3DXVec3LengthSq(&D3DXVECTOR3(vCurPos - vReqPos)) <= (fLength*fLength))
        return true;
    else
        return false;
}

void GLCROWLOGIC::SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage)
{
	m_sIgnoredDamage.nIgnoreCount = nIgnoreCount;
	m_sIgnoredDamage.nIgnoreDamage = nIgnoreDamage;
}

void GLCROWLOGIC::Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID )
{
	RF_ACTIVE( m_EffSkillVarSet ).Accumulate( dwDamage, eCrow, dwID );
}