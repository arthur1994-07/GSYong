#include "pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/Event/GLIPEvent.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../Crow/GLCrow.h"

#include "../Character/GLChar.h"
#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./ServerDamage.h"

#include "../../SigmaCore/DebugInclude.h"

int CalcDefenseRateOld::Calc(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level)
{
    if (Level == 0)
    {
        sc::writeLogError(std::string("SERVER_DAMAGE::CalcDamage Level 0"));
        return Damage;
    }
    
    // Original ����
    float fRATE = (1.0f - DefaultDefense*ItemDefense / GLCONST_CHAR::fDAMAGE_DEC_RATE);
    if (fRATE > 1.0f)
    {
        fRATE = 1.0f;
    }
    else if (fRATE < 0.0f)
    {
        fRATE = 0.0f;
    }
    else
    {

    }
    return int((float) Damage * fRATE);    
}

int CalcDefenseRate_20120126::Calc(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level)
{
    if (Level == 0)
    {
        sc::writeLogError(std::string("SERVER_DAMAGE::CalcDamage Level 0"));
        return Damage;
    }
    
    // ������� 2012-01-26 ���ر�, ���ر�
    const float fDecRate = 1.0f / (GLCONST_CHAR::fDAMAGE_DEC_RATE * (1.769f * (float)Level / 120.f));
	float fFinalRate(float(DefaultDefense * ItemDefense) * fDecRate);

	if (fFinalRate > GLCONST_CHAR::fDAMAGE_DEC_MAX_G )
    {
        fFinalRate = GLCONST_CHAR::fDAMAGE_DEC_MAX_G;
    }
    else if ( fFinalRate < 0.0f )
    {
        fFinalRate = 0.0f;
    }

    return int((float) Damage * (1.0f - fFinalRate));
}

ServerCalcDamage::ServerCalcDamage(bool UseNewDefenseRate)
{
    if (UseNewDefenseRate)
        m_pCalcDefenseRate = new CalcDefenseRate_20120126;
    else
        m_pCalcDefenseRate = new CalcDefenseRateOld;
}

ServerCalcDamage::~ServerCalcDamage()
{
    delete m_pCalcDefenseRate;
}

//! �� Damage ����� �ݿ�
int ServerCalcDamage::CalcDefenseRate(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level)
{
    return m_pCalcDefenseRate->Calc(CrowType, DefaultDefense, ItemDefense, Damage, Level);
}

//! ���� ���� Damage ����
int ServerCalcDamage::CalcLowDamage(EMCROW CrowType, int DamageOld, int Defense, float StateDamage)
{
    int ResultDamage = 0;
    int nNetDAMAGE = int((float) DamageOld * (1.0f-GLCONST_CHAR::fLOW_SEED_DAMAGE) - (float) Defense);

    if (nNetDAMAGE < 0)
        nNetDAMAGE = 0;

    if (nNetDAMAGE > 0)
        ResultDamage = DamageOld - Defense;
    else
        ResultDamage = int((float) DamageOld * GLCONST_CHAR::fLOW_SEED_DAMAGE * sc::Random::RANDOM_POS());

    return int(ResultDamage*StateDamage);
}

//! Damage �ִ� �ּҰ� ������ ���� Damage ����
int ServerCalcDamage::CalcRandomDamage(const int& nDamageLow, const int& nDamageHigh, int ExtForce)
{
    int DamageNow = int(nDamageLow + (nDamageHigh - nDamageLow) * sc::Random::RANDOM_POS());
    return (DamageNow + ExtForce); // ����ġ �ջ�
}

//! ����Ÿ�� �߻� ���� �Ǵ�
bool ServerCalcDamage::CalcCrushingBlow(int nCrushingBlow)
{
    if (nCrushingBlow > static_cast<int> (GLCONST_CHAR::dwCRUSHING_BLOW_MAX))
        nCrushingBlow = GLCONST_CHAR::dwCRUSHING_BLOW_MAX;

    if (nCrushingBlow > (sc::Random::RANDOM_POS()*100))
        return true;
    else
        return false;
}

//! ���ݷ� ��ȭ�� �ݿ�
void ServerCalcDamage::AdjuctDamage(EMCROW CrowType, int& nDamageLow, int& nDamageHigh, float DamageRate, float OwnerDamagePer)
{
    if ( DamageRate < 0 )
        DamageRate = 0.0f;

    if (CrowType == CROW_SUMMON)
    {
        // ������ ���ݷ� ��·� �ݿ�
        nDamageLow  = int(float(nDamageLow) * OwnerDamagePer);
        nDamageHigh = int(float(nDamageHigh) * OwnerDamagePer);
    }

    // ���ݷ� ��ȭ�� �ݿ�
    nDamageLow  = int(float(nDamageLow)  * DamageRate);
    nDamageHigh = int(float(nDamageHigh) * DamageRate);
}

void ServerCalcDamage::AdjustLevel(int& ndxLvl, int& nExtFORCE, int TarLevel, WORD MyLevel)
{
    ndxLvl = TarLevel - MyLevel;
    if (ndxLvl > 0)
        nExtFORCE = int(sc::Random::RANDOM_POS() * ndxLvl * 0.1f);
    if (ndxLvl > 5)
        ndxLvl = 5;
    if (ndxLvl < -5)
        ndxLvl = -5;
}

// GLCONST_CHAR::dwCRITICAL_DAMAGE = 100 == 100%
// fAddCriticalDamageIncreseR = 1 == 100%
int ServerCalcDamage::CalcCriticalDamage(bool bCritical
										 , bool bCrushingBlow, int nDamage
										 , float fAddCriticalDamageIncreseR
										 , float fAddCrushingDamageIncreseR)
{    
    if (bCritical)
        nDamage = int(float(nDamage) * (GLCONST_CHAR::dwCRITICAL_DAMAGE + fAddCriticalDamageIncreseR * 100.0f) * 0.01f);

    if (bCrushingBlow)
        nDamage = int(float(nDamage) * (GLCONST_CHAR::dwCRUSHING_BLOW_DAMAGE + fAddCrushingDamageIncreseR * 100.0f) * 0.01f);

    return nDamage;
}

//! Critical �߻� ���� �Ǵ�
bool ServerCalcDamage::CalcCritical(EMCROW CrowType, DWORD NowHp, DWORD MaxHp, int ndxLvl, int nIncCritical)
{
    if (MaxHp <= 0)
    {
        sc::writeLogError(sc::string::format("CalcCritical() - MaxHP(%1%) <= 0", MaxHp));
        return false;
    }

    int nPerHP = NowHp*100/MaxHp;

    if (nPerHP <= 10)
        nPerHP = 10;

    int nPercentCri = 1000 / nPerHP - 10 + ndxLvl;

    if (CrowType == CROW_PC)
        nPercentCri += nIncCritical;

    if (nPercentCri > (int)GLCONST_CHAR::dwCRITICAL_MAX)
        nPercentCri = GLCONST_CHAR::dwCRITICAL_MAX;

    if (nPercentCri < 0)
        nPercentCri = 0;

    //	ũ��Ƽ�� �߻� ���� �Ǵ�.
    if (nPercentCri > (sc::Random::RANDOM_POS()*100))
        return true;
    else
        return false;    
}

CalcDamage_2004::CalcDamage_2004(bool UseNewDefenseRate)
    : ServerCalcDamage(UseNewDefenseRate)
{
}

CalcDamage_2004::~CalcDamage_2004()
{
}

DWORD CalcDamage_2004::CalcDamage(
    GLChar* pChar,
    int& rResultDAMAGE,
    const DWORD dwGaeaID,
    const STARGETID &cTargetID,
    const GLLandMan* pLandMan,
    const GLSKILL* pSkill,
    const DWORD dwskill_lev,
    const DWORD dwWeatherFlag,
    const DWORD dwDivCount)    
{
    ServerActor *pActor = pChar->GetTarget(pLandMan, cTargetID);
    if (!pActor)
        return DAMAGE_TYPE_NONE;

    int nDEFENSE = pActor->GetDefense ();
    int nDEFAULT_DEFENSE = pActor->GetBodyDefense ();
    int nITEM_DEFENSE = pActor->GetItemDefense ();

    int nLEVEL = pActor->GetLevel ();
    float fSTATE_DAMAGE = pActor->GETSTATE_DAMAGE ();
    const SRESIST &sRESIST = pActor->GETRESIST ();

    EMSLOT emRHand = pChar->GetCurRHand();
    EMSLOT emLHand = pChar->GetCurLHand();

    WORD wGRADE = 0;
    const SITEMCUSTOM &sRHAND = pChar->GET_SLOT_ITEM(emRHand);
    if ( sRHAND.GetNativeID() != SNATIVEID(false) )
        wGRADE = sRHAND.GETGRADE(EMGRINDING_DAMAGE);

    int nCrushingBlow = (int)( pChar->m_sSUMITEM.fIncR_CrushingBlow * 100 );

    DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
    bool bShock = false;
    bool bCrushingBlow = false;
    bool bCritical = false; 
    bool bPsyDamage = true;

    const DAMAGE_SPEC& sDamageSpec = pActor->GetDamageSpec();
    float fDamageReduce = sDamageSpec.m_fPsyDamageReduce;
    float fDamageReflection = sDamageSpec.m_fPsyDamageReflection;
    float fDamageReflectionRate = sDamageSpec.m_fPsyDamageReflectionRate;
    float fDamageCurse = pChar->m_sDamageSpec.m_fDamageCurse;

    DEFENSE_SKILL sDefenseSkill = pActor->GetDefenseSkill();

    GLDWDATA gdDamage = pChar->m_gdDAMAGE_SKILL;
	DWORD dwApplyFlag = 0;
    if ( pSkill )
    {
        const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];

        //	Note : ��ų ���� ������ �Ҹ�Ǹ�, ������ ����ġ ����.
        //
        if ( sSKILL_DATA.wUSE_CHARMNUM > 0 )
        {
            /*SITEM* pItem = pChar->GET_SLOT_ITEMDATA(emLHand);
            if ( pItem )
            {
                GLPADATA &sDATA = pItem->sSuitOp.gdDamage;
				gdDamage += sDATA;
            }*/
			const SITEMCUSTOM& sItemCustom = pChar->GET_SLOT_ITEM( emLHand );
			gdDamage += sItemCustom.GetBasicAttackDamage();
        }

        float _fRESIST_G = 0;	
		dwApplyFlag = pSkill->m_sBASIC.emAPPLY;
        switch ( pSkill->m_sBASIC.emAPPLY )
        {
            //	���� ����.
        case SKILL::EMAPPLY_MELEE:
            gdDamage.nNow += pChar->m_sSUMITEM.gdDamage.nNow;
            gdDamage.nMax += pChar->m_sSUMITEM.gdDamage.nMax;

			gdDamage.VAR_PARAM ( pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] );
            _fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;			
            break;

            //	��Ÿ� ����.
        case SKILL::EMAPPLY_RANGE:
            gdDamage.nNow += pChar->m_sSUMITEM.gdDamage.nNow;
            gdDamage.nMax += pChar->m_sSUMITEM.gdDamage.nMax;

            gdDamage.VAR_PARAM ( pChar->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] );
            _fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;

            fDamageReflection = 0.0f;
            fDamageReflectionRate = 0.0f;

            break;

            //	���� ����.
        case SKILL::EMAPPLY_MAGIC:
            nDEFENSE = 0;						//	���� �����϶��� �Ϲ� ���� ���õ�.
            nDEFAULT_DEFENSE = 0;				//	���� �����϶��� �Ϲ� ���� ���õ�.
            nITEM_DEFENSE = 0;					//	���� �����϶��� �Ϲ� ���� ���õ�.
            gdDamage.VAR_PARAM ( pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] );
            _fRESIST_G = GLCONST_CHAR::fRESIST_G;

            fDamageReduce = sDamageSpec.m_fMagicDamageReduce;
            fDamageReflection = sDamageSpec.m_fMagicDamageReflection;
            fDamageReflectionRate = sDamageSpec.m_fMagicDamageReflectionRate;
            bPsyDamage = false;
            break;

        default:
            GASSERT(0&&"CALCDAMAGE() ���� �������� ������� �ʴ� ��ų.");
            break;
        };

        for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
        {
            if ( pSkill->m_sAPPLY.vecSPEC[i].emSPEC == EMSPECA_CRUSHING_BLOW )
            {
                nCrushingBlow += (int) ( pSkill->m_sAPPLY.vecSPEC[i].sSPEC[dwskill_lev].fVAR2 * 100 ) ;
            }
        }	

        EMELEMENT emELMT(EMELEMENT_SPIRIT);
        if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
        {
            SITEM* pITEM = pChar->GET_ELMT_ITEM ();
            if (pITEM)
                emELMT = STATE_TO_ELEMENT(pITEM->sSuitOp.sBLOW.emTYPE);
        }
        else
        {
            emELMT = pSkill->m_sAPPLY.emELEMENT;
        }

        short	nRESIST		= sRESIST.GetElement ( emELMT );
        float	fPOWER		= GLOGICEX::WEATHER_ELEMENT_POW ( emELMT, dwWeatherFlag, pLandMan->IsWeatherActive() );
        float	fSKILL_VAR	= sSKILL_DATA.fBASIC_VAR;		        //	��ų�� ������ ��꿡 ������ �ִ� ���� �⺻���� Ư������ �˻�.
        int		nVAR		= abs( int(fSKILL_VAR * fPOWER) );
		float fResistTotal = nRESIST * 0.01f * _fRESIST_G;
		fResistTotal = fResistTotal > GLCONST_CHAR::fREGIST_MAX_G ? GLCONST_CHAR::fREGIST_MAX_G : fResistTotal;
        nVAR = max( nVAR - (int)( nVAR * fResistTotal ), 0 );

        gdDamage.nNow	+= ( nVAR + (gdDamage.nNow  * wGRADE) / GLCONST_CHAR::fDAMAGE_GRADE_K );		// ������� ����ȴ�, ����� �ݿ���.
        gdDamage.nMax	+= ( nVAR + (gdDamage.nMax * wGRADE) / GLCONST_CHAR::fDAMAGE_GRADE_K );
    }
    else
    {
        gdDamage.nNow	+= pChar->m_sSUMITEM.gdDamage.nNow;
        gdDamage.nMax	+= pChar->m_sSUMITEM.gdDamage.nMax;
		
		if( pChar->ISLONGRANGE_ARMS() )
		{
			gdDamage.VAR_PARAM ( pChar->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] );	//	��Ÿ� ����.
			dwApplyFlag = SKILL::EMAPPLY_DWORD_RANGE;
		}
        else
		{
			gdDamage.VAR_PARAM ( pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] );	//	���� ����.
			dwApplyFlag = SKILL::EMAPPLY_DWORD_MELEE;
		}
    } //if..else

    // ���ݷ�. ��ȭ�� �ݿ�
    AdjuctDamage(CROW_PC, gdDamage.nNow, gdDamage.nMax, pChar->m_fDamageRate, 0.0f);

    // �߰� ����ġ ����
    int nExtFORCE = 0;
    int ndxLvl = 0;
    AdjustLevel(ndxLvl, nExtFORCE, nLEVEL, pChar->GETLEVEL());

    // Critical �߻� Ȯ��
    bCritical = CalcCritical(CROW_PC, pChar->GETHP(), pChar->GETMAXHP(), ndxLvl, 0);

    // ����Ÿ�� �߻� ���� �Ǵ�
    bCrushingBlow = CalcCrushingBlow(nCrushingBlow);

    // Damage �ִ� �ּҰ� ������ ���� Damage ����
    int nDAMAGE_OLD = CalcRandomDamage(gdDamage.nNow, gdDamage.nMax, nExtFORCE);

    // ���� ���� Damage ����
    rResultDAMAGE = CalcLowDamage(CROW_PC, nDAMAGE_OLD, nDEFENSE, fSTATE_DAMAGE);	

    // �� Damage ����� �ݿ�
    rResultDAMAGE = CalcDefenseRate(CROW_PC, nDEFAULT_DEFENSE, nITEM_DEFENSE, rResultDAMAGE, pChar->m_wLevel);

    // rResultDAMAGE = CalcCriticalDamage(bCritical, bCrushingBlow, rResultDAMAGE);
	::MessageBox( NULL, "test", "testMessageBox", MB_OK );
    if (rResultDAMAGE <= 1)
    {
        bCritical = false;
        bCrushingBlow = false;
        rResultDAMAGE = 1;
    }

    if (dwDivCount > 1)
        rResultDAMAGE /= dwDivCount;

    //	�ߵ� ��ų
    if (sDefenseSkill.m_dwSkillID != NATIVEID_NULL())
    {
        if (sDefenseSkill.m_fRate > (sc::Random::RANDOM_POS()*1))
        {
            STARGETID sActor(CROW_PC,dwGaeaID);
            pActor->DefenseSkill(sDefenseSkill.m_dwSkillID, sDefenseSkill.m_wLevel, sActor);
        }
    }

    //	������ ���
    if (fDamageReduce > 0.0f) 
    {
        int nDamageReduce = (int) (((rResultDAMAGE * fDamageReduce) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL);
        rResultDAMAGE -= nDamageReduce;

        if (bPsyDamage)
            dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
        else
            dwDamageFlag += DAMAGE_TYPE_MAGIC_REDUCE;
    }

    //	������ �ݻ� ���
    if (fDamageReflectionRate > 0.0f)
    {
        if (fDamageReflectionRate > (sc::Random::RANDOM_POS()*1))
        {
            int nDamageReflection = (int) (((rResultDAMAGE * fDamageReflection) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL);
            if (nDamageReflection > 0) 
            {
                if (bPsyDamage)
                    dwDamageFlag += DAMAGE_TYPE_PSY_REFLECTION;
                else
                    dwDamageFlag += DAMAGE_TYPE_MAGIC_REFLECTION;		

                // �ǰ��� ���忡�� �������� �ٽ� ������.
                STARGETID sActor(CROW_PC,dwGaeaID);
                pActor->DamageReflectionProc(nDamageReflection, sActor);
            }
        }
    }

    //	���� ���
    if (fDamageCurse > 0.0f) 
    {
        int nDamageCurse = (int)(rResultDAMAGE * fDamageCurse);
        if (nDamageCurse > 0) 
        {
            STARGETID sActor(pChar->GETCROW(),dwGaeaID);
            ServerActor *pMyActor = pChar->GetTarget(pLandMan, sActor);
            if (!pMyActor)
                return FALSE;

            STARGETID sCurseTargetID = pChar->m_sDamageSpec.m_sCurseTargetID;
            pMyActor->DamageCurse(nDamageCurse, sCurseTargetID, cTargetID.emCrow);
        }
    }

    bShock = (0!=GLOGICEX::CHECKSHOCK(pChar->GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical));

    if (bShock)
        dwDamageFlag += DAMAGE_TYPE_SHOCK;
    if (bCritical)
        dwDamageFlag += DAMAGE_TYPE_CRITICAL;
    if (bCrushingBlow)
        dwDamageFlag += DAMAGE_TYPE_CRUSHING_BLOW;

    return dwDamageFlag;
}
    
CalcDamage_20060328::CalcDamage_20060328(bool UseNewDefenseRate)
    : ServerCalcDamage(UseNewDefenseRate)
{
}

CalcDamage_20060328::~CalcDamage_20060328()
{
}

DWORD CalcDamage_20060328::CalcDamage(
    GLChar* pChar,
    int& rResultDAMAGE,
    const DWORD dwGaeaID,
    const STARGETID &cTargetID,
    const GLLandMan* pLandMan,
    const GLSKILL* pSkill,
    const DWORD dwskill_lev,
    const DWORD dwWeatherFlag,
    const DWORD dwDivCount)    
{
    ServerActor *pActorTarget = pChar->GetTarget(pLandMan, cTargetID);
    if (!pActorTarget)
        return DAMAGE_TYPE_NONE;

	SIGNORED_DAMAGE* const pIgnoredDamage = pActorTarget->GetIgnoredDamage();

    DAMAGE_SPEC*	pDamageSpec = pActorTarget->GetDamageSpecPtr();
	if( pDamageSpec->m_dwFlags & DAMAGE_SPEC::FLAG_ILLUSION )
	{
		if( pDamageSpec->m_wIllusionRemain > 0 )
		{
			float		fElapsedTime = GetCurrentFrameTime() - pDamageSpec->m_fIllusionLastTime;
			if( fElapsedTime > GLCONST_CHAR::fIllusionInterval )
			{
				pDamageSpec->m_fIllusionLastTime = GetCurrentFrameTime();
				pDamageSpec->m_wIllusionRemain--;
				return DAMAGE_TYPE_ILLUSION;
			} //if
		} //if
	} //if

    int nDEFENSE = pActorTarget->GetDefense ();
    int nDEFAULT_DEFENSE = pActorTarget->GetBodyDefense ();
    int nITEM_DEFENSE = pActorTarget->GetItemDefense ();

	if ( pActorTarget->GetCrow() == CROW_PC || pActorTarget->GetCrow() == CROW_SUMMON )
	{
		nDEFENSE *= GLCONST_CHAR::fReductionRate_PVP_PHY_Defense;
		nDEFAULT_DEFENSE *= GLCONST_CHAR::fReductionRate_PVP_PHY_Defense;
		nITEM_DEFENSE *= GLCONST_CHAR::fReductionRate_PVP_PHY_Defense;
	}

    int nLEVEL = pActorTarget->GetLevel ();
    float fSTATE_DAMAGE = pActorTarget->GETSTATE_DAMAGE ();
    const SRESIST &sRESIST = pActorTarget->GETRESIST (); // ����� �� ���װ�
    
    EMSLOT emLHand = pChar->GetCurLHand();	
    const WORD wGRADE(pChar->GetGradeAttack());

    int nCrushingBlow = (int)( pChar->m_sSUMITEM.fIncR_CrushingBlow * 100 );


    DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
    bool bShock = false;
    bool bCrushingBlow = false;
    bool bCritical = false; 
    bool bPsyDamage = true;

    float fDamageReduce = pDamageSpec->m_fPsyDamageReduce;
    float fDamageReflection = pDamageSpec->m_fPsyDamageReflection;
    float fDamageReflectionRate = pDamageSpec->m_fPsyDamageReflectionRate;
    float fDamageCurse = pChar->m_sDamageSpec.m_fDamageCurse;

    DEFENSE_SKILL sDefenseSkill = pActorTarget->GetDefenseSkill();

//    GLPADATA gdDamage = pChar->m_gdDAMAGE_SKILL;
	int nDamageLow(pChar->m_gdDAMAGE_SKILL.nNow); 
	int nDamageHigh(pChar->m_gdDAMAGE_SKILL.nMax);

	// Max MP �� Ư�� ������ŭ ���ݷ� ���� ȿ�� ����;
	{
		const float fAllMPRatioToAttack = pChar->GetIncR_AllMPRatioToAttack();
		const int nAllMPRatioToAttack = static_cast< const int >( pChar->GETMAXMP() * fAllMPRatioToAttack );

		nDamageLow += nAllMPRatioToAttack;
		nDamageHigh += nAllMPRatioToAttack;
	}

	DWORD dwApplyFlag = 0;
    if ( pSkill )
    {
        const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];

        //	Note : ��ų ���� ������ �Ҹ�Ǹ�, ������ ����ġ ����.
        //
        if ( sSKILL_DATA.wUSE_CHARMNUM > 0 )
        {
            /*SITEM* pItem = pChar->GET_SLOT_ITEMDATA(emLHand);
            if ( pItem )
            {
                GLPADATA &sDATA = pItem->sSuitOp.gdDamage;
                nDamageLow += int(sDATA.wLow);
                nDamageHigh += int(sDATA.wHigh);
            }*/

			const SITEMCUSTOM& sItemCustom = pChar->GET_SLOT_ITEM( emLHand );
			nDamageLow += sItemCustom.GetBasicAttackDamage().GetLow();
			nDamageHigh += sItemCustom.GetBasicAttackDamage().GetHigh();
        }		

        if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneActionType, pSkill->m_sBASIC.emACTION_TYPE) ||
            CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, pSkill->m_sBASIC.emAPPLY))
        {
            rResultDAMAGE = 0;
            return DAMAGE_TYPE_IMMUNE;
        }

		dwApplyFlag = 1 << pSkill->m_sBASIC.emAPPLY;
        float fRESIST_G = 0;		
        switch ( pSkill->m_sBASIC.emAPPLY )
        {
        case SKILL::EMAPPLY_MELEE: // ����;
			nDamageLow += (pChar->m_sSUMITEM.gdDamage.nNow + pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE]);
			nDamageHigh += (pChar->m_sSUMITEM.gdDamage.nMax + pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE]);

			// ���� ���ݽ� �Ӽ� ����ġ ���� ����
			// default.charset ���� ������
			// fRESIST_PHYSIC_G �� ��
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
            break;

        case SKILL::EMAPPLY_RANGE: // ���;
			nDamageLow += int(pChar->m_sSUMITEM.gdDamage.nNow) + pChar->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];
			nDamageHigh += int(pChar->m_sSUMITEM.gdDamage.nMax) + pChar->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];

			// ���� ���ݽ� �Ӽ� ����ġ ���� ����
			// default.charset ���� ������
			// fRESIST_PHYSIC_G �� ��
			fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
			fDamageReflection = 0.0f;
			fDamageReflectionRate = 0.0f;		
            break;

        case SKILL::EMAPPLY_MAGIC: // ����;
			nDEFENSE = 0;						//	���� �����϶��� �Ϲ� ���� ���õ�.
			nDEFAULT_DEFENSE = 0;				//	���� �����϶��� �Ϲ� ���� ���õ�.
			nITEM_DEFENSE = 0;					//	���� �����϶��� �Ϲ� ���� ���õ�.

			nDamageLow += pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC];
			nDamageHigh += pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC];

			// ���� ���ݽ� �Ӽ� ����ġ ���� ����
			// default.charset ���� ������
			// fRESIST_G �� ��
			fRESIST_G = GLCONST_CHAR::fRESIST_G;
			fDamageReduce = pDamageSpec->m_fMagicDamageReduce;
			fDamageReflection = pDamageSpec->m_fMagicDamageReflection;
			fDamageReflectionRate = pDamageSpec->m_fMagicDamageReflectionRate;
			bPsyDamage = false;
            break;

        default:
            GASSERT(0&&"CALCDAMAGE() ���� �������� ������� �ʴ� ��ų.");
            break;
        };

		if( pChar->IsSkillEffectFrame() )
		{
			for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
			{
				if ( pSkill->m_sAPPLY.vecSPEC[i].emSPEC == EMSPECA_CRUSHING_BLOW )
				{
					nCrushingBlow += (int) ( pSkill->m_sAPPLY.vecSPEC[i].sSPEC[dwskill_lev].fVAR2 * 100 ) ;
				}
			}	
		}

        EMELEMENT emELMT(EMELEMENT_SPIRIT);
        if ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM )
        {
            SITEM* pITEM = pChar->GET_ELMT_ITEM ();
            if ( pITEM )
                emELMT = STATE_TO_ELEMENT(pITEM->sSuitOp.sBLOW.emTYPE);
        }
        else
        {
            emELMT = pSkill->m_sAPPLY.emELEMENT;
        }

        short nRESIST = sRESIST.GetElement ( emELMT );
		if ( pActorTarget->GetCrow() == CROW_PC || pActorTarget->GetCrow() == CROW_SUMMON )
		{
			nRESIST *= GLCONST_CHAR::fReductionRate_PVP_MAGIC_Resistance;
		}

		if ( (pIgnoredDamage != NULL) && (pIgnoredDamage->isAvailable() == true) )
		{
			nRESIST = 0;
		}

        //	��ų�� ������ ��꿡 ������ �ִ� ���� �⺻���� Ư������ �˻�.
        // ��ų������ = �⺻��ų�������� ���� �������ݿ���
		int nVAR(::abs(int(sSKILL_DATA.fBASIC_VAR)));
		const float fSkillDamageAdd(pChar->GetIncR_SkillDamageAdd(pSkill->m_sBASIC.dwGRADE-1));
		nVAR += nVAR * fSkillDamageAdd;

		{ // ���� ��޿� ���� �ݿ���;
			const float fGrade(float(wGRADE) / GLCONST_CHAR::fDAMAGE_GRADE_K);
			// �ּ�/�ִ� ������ =  ��ų������ + (�ּ�/�ִ� ������) * �ݿ���
			nDamageLow  += int(float(nDamageLow) * fGrade + nVAR);
			nDamageHigh += int(float(nDamageHigh) * fGrade + nVAR);
		}

		if ( pActorTarget->GetCrow() == CROW_PC || pActorTarget->GetCrow() == CROW_SUMMON )
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

		// ���� ������ Ÿ���� ���°� ��ȣ�ۿ�Ǳ� ������ �̷�������Ѵ�;
		if ( cTargetID.emCrow == CROW_PC )
		{
			float fDec_Damage(0.0f);
			switch ( pSkill->m_sBASIC.emAPPLY )
			{
			case SKILL::EMAPPLY_MELEE:	fDec_Damage = pActorTarget->GetDecR_DamageMeleeSkill();	break;
			case SKILL::EMAPPLY_RANGE:	fDec_Damage = pActorTarget->GetDecR_DamageRangeSkill();	break;
			case SKILL::EMAPPLY_MAGIC:	fDec_Damage = pActorTarget->GetDecR_DamageMagicSkill();	break;
			}
			if ( fDec_Damage != 0.0f )
			{
				nDamageLow  -= int(float(nDamageLow) * fDec_Damage);
				nDamageHigh -= int(float(nDamageHigh) * fDec_Damage);
			}
		}

		{
			// ��ü���װ� = ����� ���װ� / 100 * �Ӽ�����ġ �������
			float fResistTotal = (float) ((float) nRESIST * 0.01f * fRESIST_G);
			fResistTotal = fResistTotal > GLCONST_CHAR::fREGIST_MAX_G ? GLCONST_CHAR::fREGIST_MAX_G : fResistTotal;

			// �ּ�/�ִ� ������ = ������-(������*��ü���װ�)
			nDamageLow  -= int(float(nDamageLow)  * fResistTotal);
			nDamageHigh -= int(float(nDamageHigh) * fResistTotal);
		}
        // �������� 0 ���� ������ 0 ���� �����Ѵ�.
        if ( nDamageLow < 0 )
            nDamageLow  = 0;
        if ( nDamageHigh < 0 )
            nDamageHigh = 0;
    }
    else
    {
        nDamageLow += pChar->m_sSUMITEM.gdDamage.nNow;
        nDamageHigh += pChar->m_sSUMITEM.gdDamage.nMax;
        if ( pChar->ISLONGRANGE_ARMS() )
        {
            if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, SKILL::EMAPPLY_RANGE) )
            {
                rResultDAMAGE = 0;
                return DAMAGE_TYPE_IMMUNE;
            }
			dwApplyFlag = SKILL::EMAPPLY_DWORD_RANGE;
			nDamageLow += pChar->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];
			nDamageHigh += pChar->m_sumPowerAttribute[SKILL::EMAPPLY_RANGE];
        }
        else
        {
            if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, SKILL::EMAPPLY_MELEE) )
            {
                rResultDAMAGE = 0;
                return DAMAGE_TYPE_IMMUNE;
			}
			dwApplyFlag = SKILL::EMAPPLY_DWORD_MELEE;
			nDamageLow += pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE];
			nDamageHigh += pChar->m_sumPowerAttribute[SKILL::EMAPPLY_MELEE];
		}

		if ( pActorTarget->GetCrow() == CROW_PC || pActorTarget->GetCrow() == CROW_SUMMON )
		{
			if ( pChar->ISLONGRANGE_ARMS() )
			{
				nDamageLow *= GLCONST_CHAR::fReductionRate_PVP_PHY_S_Damage;
				nDamageHigh *= GLCONST_CHAR::fReductionRate_PVP_PHY_S_Damage;
			}
			else
			{
				nDamageLow *= GLCONST_CHAR::fReductionRate_PVP_PHY_P_Damage;
				nDamageHigh *= GLCONST_CHAR::fReductionRate_PVP_PHY_P_Damage;
			}
		}
	}

    // �߰� ����ġ ����
    int nExtFORCE = 0;
    int ndxLvl = 0;
    AdjustLevel(ndxLvl, nExtFORCE, nLEVEL, pChar->GETLEVEL());	


	SINCREASEEFF* const pIncreaseEff = pActorTarget->GetIncreaseEff();

	// Critical �߻� Ȯ��
	int nIncCritical = int(pChar->m_sSUMITEM.fIncR_Critical * 100.0f);
	float _fDamageRate = pChar->m_fDamageRate;
	
	if ( pIncreaseEff )
	{
		if ( pIncreaseEff->dwApplyFlag & dwApplyFlag ) // ��ȭ ȿ��
		{	
			const int nIncreaseCrushingBlow = pIncreaseEff->GetIncreaseCrushingBlowRate(dwApplyFlag);
            const float fIncreaseRate = float(pIncreaseEff->GetIncreaseDamageRate(dwApplyFlag)) * 0.01f;
			nCrushingBlow += nIncreaseCrushingBlow;
			_fDamageRate += fIncreaseRate;
		}
	}

    // IP Event ����.
    if ( pChar->m_sEVENTFACT.IsIPEvent() )
    {
        nIncCritical += ran_event::GLIPEvent::Instance()->GetCriticalRate();
    }

	if ( pSkill && cTargetID.emCrow == CROW_MOB )
	{
		GLCrow* pCrow = pLandMan->GetCrow(cTargetID.GaeaId);
		if ( pCrow->m_pCrowData->IsBoss() )
		{
			// ���� ������ ���������� ũ��Ƽ�� ��� ȿ���� �ִٸ�;
			const float fIncRCritical_EmergencyBoss(pChar->GetIncRCritical_EmergencyBoss());
			if ( pCrow->IsActState(EM_ACT_EMERGENCY) && fIncRCritical_EmergencyBoss != 0.0f )
				nIncCritical += (fIncRCritical_EmergencyBoss * 100.0f);
		}
		else // �Ϲ� ��;
		{
			// ���� ������ ������ ũ��Ƽ�� ��� ȿ���� �ִٸ�;
			const float fIncRCritical_EmergencyMob(pChar->GetIncRCritical_EmergencyMob());
			if ( pCrow->IsActState(EM_ACT_EMERGENCY) && fIncRCritical_EmergencyMob != 0.0f )
				nIncCritical += (fIncRCritical_EmergencyMob * 100.0f);
		}
	}
	
	// ���ݷ�. ��ȭ�� �ݿ�
	AdjuctDamage(CROW_PC, nDamageLow, nDamageHigh, _fDamageRate, 0.0f);

	// Damage �ִ� �ּҰ� ������ ���� Damage ����
	int nDAMAGE_OLD = CalcRandomDamage(nDamageLow, nDamageHigh, nExtFORCE);	
	
	bCritical = CalcCritical(CROW_PC, pChar->GETHP(), pChar->GETMAXHP(), ndxLvl, nIncCritical );

    // ����Ÿ�� �߻� ���� �Ǵ�    
    bCrushingBlow = CalcCrushingBlow(nCrushingBlow);   
	
	if ( (pIgnoredDamage != NULL) && (pIgnoredDamage->isAvailable() == true) )
	{
		if ( dwDivCount > 1)
			nDAMAGE_OLD /= dwDivCount;
		if ( pIgnoredDamage->AbsorbDamage(nDAMAGE_OLD) == false )
			pActorTarget->SKILL_EFFECT_REMOVE(EMSPECA_IGNORED_DAMAGE);

		if ( nDAMAGE_OLD == 0 )
			return DAMAGE_TYPE_NONE;

		if ( dwDivCount > 1)
			nDAMAGE_OLD *= dwDivCount;
	}

    // ���� ���� Damage ����
    rResultDAMAGE = CalcLowDamage(CROW_PC, nDAMAGE_OLD, nDEFENSE, fSTATE_DAMAGE);	

    // �� Damage ����� �ݿ�
    rResultDAMAGE = CalcDefenseRate(CROW_PC, nDEFAULT_DEFENSE, nITEM_DEFENSE, rResultDAMAGE, pChar->m_wLevel);
// 	static INT nTest(0);
// 	nTest++;
// 	bCrushingBlow = nTest%2 == 0 ? true : false;
// 	bCritical = !bCrushingBlow;
    rResultDAMAGE = CalcCriticalDamage(bCritical, bCrushingBlow, rResultDAMAGE
		, pChar->GetIncR_CriticalDamage(), pChar->GetIncR_CrushingBlow());

    if (dwDivCount > 1)
        rResultDAMAGE /= dwDivCount;

    if (rResultDAMAGE <= 1)
    {
        bCritical = false;
        bCrushingBlow = false;
        rResultDAMAGE = 1;
    }

    //	�ߵ� ��ų
    if (sDefenseSkill.m_dwSkillID != NATIVEID_NULL())
    {
        if (sDefenseSkill.m_fRate > (sc::Random::RANDOM_POS()*1))
        {
            STARGETID sActor(CROW_PC,dwGaeaID);
            pActorTarget->DefenseSkill(sDefenseSkill.m_dwSkillID,  sDefenseSkill.m_wLevel, sActor);
        }
    }

    //	������ ���
    if (fDamageReduce > 0.0f) 
    {
        int nDamageReduce = (int) (((rResultDAMAGE * fDamageReduce) * nLEVEL) / GLCONST_CHAR::wMAX_LEVEL);
        rResultDAMAGE -= nDamageReduce;

        if (bPsyDamage)
            dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
        else
            dwDamageFlag += DAMAGE_TYPE_MAGIC_REDUCE;
    }

    //	������ �ݻ� ���
    if (fDamageReflectionRate > 0.0f)
    {
        if (fDamageReflectionRate > (sc::Random::RANDOM_POS()*1))
        {
            //int nDamageReflection = (int) (((rResultDAMAGE * fDamageReflection) * nLEVEL ) / GLCONST_CHAR::wMAX_LEVEL);
			int nDamageReflection = (int) (rResultDAMAGE * fDamageReflection);

            if (nDamageReflection > 0)
            {
                if (bPsyDamage)
                    dwDamageFlag += DAMAGE_TYPE_PSY_REFLECTION;
                else
                    dwDamageFlag += DAMAGE_TYPE_MAGIC_REFLECTION;

                // �ǰ��� ���忡�� �������� �ٽ� ������.
                STARGETID sActor(CROW_PC,dwGaeaID);
                pActorTarget->DamageReflectionProc(nDamageReflection, sActor);
            }
        }
    }

    //	���� ���
    if (fDamageCurse > 0.0f) 
    {
        int nDamageCurse = (int) (rResultDAMAGE * fDamageCurse);
        if (nDamageCurse > 0)
        {
            STARGETID sActor(pChar->GETCROW(),dwGaeaID);
            ServerActor *pMyActor = pChar->GetTarget(pLandMan, sActor);
            if (!pMyActor)
                return FALSE;

            STARGETID sCurseTargetID = pChar->m_sDamageSpec.m_sCurseTargetID;
            pMyActor->DamageCurse(nDamageCurse, sCurseTargetID, cTargetID.emCrow);
        }
    }

    bShock = (0!=GLOGICEX::CHECKSHOCK(pChar->GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical));

    if (bShock)
        dwDamageFlag += DAMAGE_TYPE_SHOCK;
    if (bCritical)
        dwDamageFlag += DAMAGE_TYPE_CRITICAL;
    if (bCrushingBlow)
        dwDamageFlag += DAMAGE_TYPE_CRUSHING_BLOW;
	
	if ( rResultDAMAGE < 0 )
		rResultDAMAGE = 1;

	if ( pSkill && cTargetID.emCrow == CROW_MOB )
	{
		// EMITEM_VAR::EMVAR_BOSSFIXEDDAMAGE
		// �������� ����Ǿ���Ѵ�;
		GLCrow* pCrow = pLandMan->GetCrow(cTargetID.GaeaId);
		if ( pCrow->m_pCrowData->IsBoss() )
		{
			int nBossFixedDamageMax(pChar->GetBossFixedDamage());
			if ( nBossFixedDamageMax )
			{
				rResultDAMAGE += nBossFixedDamageMax;
			}
		}
		// �Ϲ� ��;
		else
		{
		}
	}

	// ĳ���Ͱ� ���޻����� ��� ���ذ��� ȿ�� üũ�� ���ҽ�Ų��;
	if ( pActorTarget->GetCrow() == CROW_PC && pActorTarget->IsActState(EM_ACT_EMERGENCY) )
	{
		const float fDecRDamage(pActorTarget->GetDecRDamage_EmergencyState());
		rResultDAMAGE -= ((float)rResultDAMAGE * fDecRDamage);
	}

	if ( rResultDAMAGE < 0 )
		rResultDAMAGE = 0;

    return dwDamageFlag;
}
