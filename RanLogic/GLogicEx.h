#ifndef GLOGIXEX_H_
#define GLOGIXEX_H_

#pragma once

// GLCHARLOGIC, GLCROWLOGIC
//	�� Ŭ������ �������� ������ �Լ����� �������� �ٿ� "�ɸ��� ������", "NPC ������" ����
//	���� �޾Ƽ� ���� ������ �Լ��� �����ϰԵ�. ���� �Լ� �κ��� ���� ���� ����. ( ���� ȿ���� ����. )
//	��, �Լ� ������ �����ϰ� ����. ( �ٸ� ������ �ٿ����� ���������� �ζ��� �Լ� ���Ǹ� �����ϰ��Ͽ� ����. )
//
// ����(2012/07/03) ���� ������� ���ǰ� ���� ����;
// GLogic, GLogicCrow �� ClientActor, ServerActor�� ����� �׼�(�̵�, ��ų ��) ���� �۾� ��,
// ������ �ڵ带 �μ����� �̻� �ۼ��ؾ� �ϴ� ���ŷο��� ����;

#include <boost/tr1/unordered_map.hpp>
#include <list>

//#include "../InternalCommonLib/Interface/PlayerInterface.h"

#include "./Skill/GLSKILL.h"
#include "./Character/GLCharData.h"
#include "./Crow/GLCrowData.h"
#include "./GLogicData.h"
#include "./GLogicExDefine.h"
#include "./Msg/GLContrlMsg.h"

#include "./Skill/GLCountAttackSkill.h"
#include "./Skill/GLSkillEffVarSet.h"

using namespace NSSKILL_EFFSET;

#ifndef GLSKILL
	struct GLSKILL;
#endif //GLSKILL

struct SProductRecipe;

#define MOVE_OPTION_SHOW_EFF			0x00000001
#define MOVE_OPTION_NOT_TURN_IDLE		0x00000002


// �ᱹ ���� ����Ǿ���ϴ� ������ ������ ��ġ�� �ȵȴ�;
// ex) fIncR_AtkSpeed, fIncR_AtkSpeed_SoloPlay�� �ᱹ �ϳ��� �������� ġȯ�ǳ�;
// fIncR_AtkSpeed_SoloPlay�� �ٸ� ���� ���� �ִ밪�� ����Ǿ����;
struct SSUM_ITEM
{
	GLDWDATA	gdDamage;
	int			nDefense;

	int			nAvoidRate;
	int			nHitRate;

	int			nHP;
	int			nMP;
	int			nSP;

	float		fIncR_HP;
	float		fIncR_MP;
	float		fIncR_SP;

	float		fInc_HP;
	float		fInc_MP;
	float		fInc_SP;

	SCHARSTATS	sStats;
	SRESIST		sResist;

	int			nPA;
	int			nSA;
	int			nMA;

	float		fIncR_MoveSpeed;
	float		fIncR_AtkSpeed;
	float		fIncR_Critical;
	float		fIncR_CrushingBlow;
	float		fIncR_Ignore_BossDamage;		// ���������� �ǰݽ� ������ ����;
	float		fIncR_Critical_EmergencyMob;	// ���� ������ ������ ũ��Ƽ�� �߻��� ����;
	float		fIncR_Critical_EmergencyBoss;

	float		fInc_MoveSpeed;
	float		fInc_AtkSpeed;
	float		fInc_Critical;
	float		fInc_CrushingBlow;

	float		fIncR_ExpAddEffect;	// ����ġ ������ ȿ��'�� ������Ŵ;
	float		fIncR_DropGenMoney;	// �� ��ɽ� ����Ǵ� �� ������;
	float		fDecR_Damage_MeleeSkill; // ���� ���� ��ų �ǰݽ� ��� ������ ����;
	float		fDecR_Damage_RangeSkill; // ��� ��ų �ǰݽ� " ;
	float		fDecR_Damage_MagicSkill; // ���� ��ų �ǰݽ� " ;
	float		fIncR_AtkSpeed_SoloPlay; // �ַ��÷��̽� ���ݼӵ� ����;
	float		fIncR_AtkSpeed_PartyPlay; // �ַ��÷��̽� ���ݼӵ� ����;

	int			nPotion_HP_Inc;
	int			nPotion_MP_Inc;
	int			nPotion_SP_Inc;
	int			nReceive_CP_Inc;
	
	float		fExpMultiple;		//! ����ġ ���

	INT			nInc_BossFixedDamage;		// ������ ���ݽ� �߰� ���� ������;
	float		fDecR_Damage_EmergencyState;	// ���� ������ ��� ������ ����;
	INT			nInc_HP_SoloPlay;			// �ַ��÷��̽� HP���� (��Ƽ�� �ƴҽ�);
	INT			nInc_HP_PartyPlay;			// ��Ƽ�÷��̽� HP����;

	float		fInc_SkillDurationTAdd[SKILL::MAX_GRADE];	// ��޺� ��ų ���ӽð� ����;
	float		fIncR_SkillDamageAdd[SKILL::MAX_GRADE];		// ��޺� ��ų ������ ����;
	float		fDecR_SkillDelayT[SKILL::MAX_GRADE];			// ��޺� ��ų ������ ����;
	float		fIncR_CriticalDamage;						// ũ��Ƽ�� ������ ������;
	float		fIncR_CrushingBlowDamage;							// ����Ÿ�� ������ ������
	float		fIncR_LifePerKill;							// óġ�� ������ ȸ����;
	float		fIncR_MPPerKill;							// óġ�� MPȸ����;
	float		fIncR_SPPerKill;							// óġ�� SPȸ����;
	float		fIncR_HPMPSPPerKill;						// óġ�� HP,MP,SPȸ����;

	float		fIncR_AllMPRatioToAttack;					// ��ü MP ������ŭ ���ݷ�����;
	float		fIncR_AllSPRatioToHp;						// ��ü SP ������ŭ ü������;
	float		fIncR_AllHitRatioToMelee;					// ��ü ���� ������ŭ ����ġ ����;
	float		fIncR_AllHitRatioToRange;					// ��ü ���� ������ŭ ���ġ ����;
	float		fIncR_AllHitRatioToMagic;					// ��ü ���� ������ŭ ���ġ ����;

	SSUM_ITEM ()
		: nDefense(0)

		, nAvoidRate(0)
		, nHitRate(0)
		, nHP(0)
		, nMP(0)
		, nSP(0)
		, fIncR_HP(0.0f)
		, fIncR_MP(0.0f)
		, fIncR_SP(0.0f)

		, fInc_HP(0.0f)
		, fInc_MP(0.0f)
		, fInc_SP(0.0f)

		, nPA(0)
		, nSA(0)
		, nMA(0)
		, fIncR_MoveSpeed(0.0f)
		, fIncR_AtkSpeed(0.0f)
		, fIncR_Critical(0.0f)
		, fIncR_CrushingBlow(0.0f)
		, fIncR_Ignore_BossDamage(0.0f)
		, fIncR_Critical_EmergencyMob(0.0f)
		, fIncR_Critical_EmergencyBoss(0.0f)
		, fIncR_ExpAddEffect(0.0f)
		, fIncR_DropGenMoney(0.0f)
		, fDecR_Damage_MeleeSkill(0.0f)
		, fDecR_Damage_RangeSkill(0.0f)
		, fDecR_Damage_MagicSkill(0.0f)
		, fIncR_AtkSpeed_SoloPlay(0.0f)
		, fIncR_AtkSpeed_PartyPlay(0.0f)

		, fInc_MoveSpeed(0.0f)
		, fInc_AtkSpeed(0.0f)
		, fInc_Critical(0.0f)
		, fInc_CrushingBlow(0.0f)
		, nPotion_HP_Inc(0)
		, nPotion_MP_Inc(0)
		, nPotion_SP_Inc(0)
		, nReceive_CP_Inc(0)

		, fExpMultiple(1.0f)
		, nInc_BossFixedDamage(0)
		, fDecR_Damage_EmergencyState(0.0f)
		, nInc_HP_SoloPlay(0)
		, nInc_HP_PartyPlay(0)
		, fIncR_CriticalDamage(0.0f)
		, fIncR_CrushingBlowDamage(0.0f)
		, fIncR_LifePerKill(0.0f)
		, fIncR_MPPerKill(0.0f)
		, fIncR_SPPerKill(0.0f)
		, fIncR_HPMPSPPerKill(0.0f)
		, fIncR_AllMPRatioToAttack(0.0f)
		, fIncR_AllSPRatioToHp(0.0f)	
		, fIncR_AllHitRatioToMelee(0.0f)
		, fIncR_AllHitRatioToRange(0.0f)
		, fIncR_AllHitRatioToMagic(0.0f)
	{
		memset(fInc_SkillDurationTAdd, 0.0f, sizeof(float) * SKILL::MAX_GRADE);
		memset(fIncR_SkillDamageAdd, 0.0f, sizeof(float) * SKILL::MAX_GRADE);
		memset(fDecR_SkillDelayT, 0.0f, sizeof(float) * SKILL::MAX_GRADE);
	}

	void RESET ()
	{
		*this = SSUM_ITEM();
	}
};

namespace GLOGICEX
{
	EM_BRIGHT_FB GLSPACEGAP ( const EMBRIGHT emACTOR, const EMBRIGHT emRECEP, const EMBRIGHT emSPACE );

	//	���� ������ ���.
	int GLHITRATE ( const int nHit, const int nAvoid, const EM_BRIGHT_FB bFB );
	int GLHITRATE_RENEW(const int nHit, const int nAvoid, const INT nAtLv, const INT nDfLv);

	//	���� ��ũ Ȯ��.
	BOOL CHECKSHOCK ( const int AttackerLev, const int DefenserLev, const int nDamage, const bool bCritical );

	//	���� ���.
	int GLDEFENSE ( const int nDEFENSE, const EMBRIGHT emACTOR, const EMBRIGHT emRecep, const EMBRIGHT emSPACE );

	//	����ġ ȹ�� ( Ÿ�ݽ� )
	int GLATTACKEXP ( int AttackerLev, int DefenserLev, DWORD dwDamage, DWORD wMaxHP, DWORD dwBonusExp=0 );

	//	����ġ ȹ�� ( ���Ž� )
	int	GLKILLEXP ( int AttackerLev, int DefenserLev, DWORD dwBonusExp=0 );	
	
	//	�׾����� ����ġ ����.
	float GLDIE_DECEXP ( WORD wACTLEV );

	//	����ġ ���� ����
	float GLDIE_RECOVERYEXP ( WORD wACTLEV );

	//  ����ġ1�� ��
	float GLDIE_EXPMONEY ( WORD wACTLEV );

	//	LevelUp�� �ϱ� ���� ����ġ
	LONGLONG GLNEEDEXP ( WORD wLev );
	LONGLONG GLNEEDEXP2 ( WORD wLev );

	BOOL GLCALCULATEEXP ( WORD wCurLev, LONGLONG llCurExp, LONGLONG llCalExp, WORD &wExceptedLev, LONGLONG &llRemainExp, LONGLONG &llRemainMaxExp );
	BOOL GLCALCULATEEXP2 ( WORD wCurLev, LONGLONG llCurExp, LONGLONG llCalExp, WORD &wExceptedLev, LONGLONG &llRemainExp, LONGLONG &llRemainMaxExp );

	// ������ ���� ���� ����ġ
	LONGLONG GLNEEDACCUMULATEDEXP ( WORD wCurLev, WORD wNeedLev );
	LONGLONG GLNEEDACCUMULATEDEXP2 ( WORD wCurLev, WORD wNeedLev );

	//	�����̻� �߻��� �� �ִ� Ȯ�� ����.
	BOOL CHECKSTATEBLOW ( float fACTRATE, WORD wACTLEVEL, WORD wLEVEL, WORD wRESIST );

	//	��ų ������ ����.
	float SKILLDELAY ( DWORD dwSKILL_GRADE, WORD wSKILL_LEV, WORD wCHAR_LEVEL, float fDelay );

	//	Note : �Ķ��Ÿ ����.
	//		���ÿ��� int ������ ����� �����Ͽ� ����� �ս��� ���´�.
	//		�� ����ÿ��� WORD �� ���� ���� �̳��� ����.
	inline WORD VARIATION ( WORD &wNow, const WORD wMax, const int nValue )
	{
		int nOld = int(wNow);

		int nNew = nOld + nValue;

		//	WORD �� ���� ���� �̳��� ����.
		if ( nNew < 0 )					nNew = 0;
		if ( nNew > USHRT_MAX )			nNew = USHRT_MAX;

		wNow = nNew;
		if ( wNow >= wMax )				wNow = wMax;

		WORD wDX = (WORD) abs(nOld-int(wNow));
		return wDX;
	}

	//	Note : �Ķ��Ÿ ����.
	//		���ÿ��� int ������ ����� �����Ͽ� ����� �ս��� ���´�.
	//		�� ����ÿ��� WORD �� ���� ���� �̳��� ����.
	inline DWORD VARIATION ( DWORD &dwNow, const DWORD dwMax, const int nValue )
	{
		int nOld = int(dwNow);

		int nNew = nOld + nValue;

		//	WORD �� ���� ���� �̳��� ����.
		if ( nNew < 0 )					nNew = 0;

		dwNow = nNew;
		if ( dwNow >= dwMax )			dwNow = dwMax;

		DWORD dwDX = (DWORD) abs(nOld-int(dwNow));
		return dwDX;
	}

	inline const int VARIATION(int& valueTarget, const int maxValue, const int valueSource)
	{
		const int valueOld(valueTarget);
		int valueNew(valueTarget + valueSource);

		if ( valueNew < 0 )
			valueNew = 0;

		if ( valueNew > maxValue )
			valueNew = maxValue;

		valueTarget = valueNew;

		return ::abs(valueOld - valueNew);
	}

	//	������ �Ӽ��� ������.
	float WEATHER_ELEMENT_POW ( EMELEMENT emElement, DWORD dwWeather, BOOL bWeatherActive );

	float WEATHER_BLOW_POW ( EMSTATE_BLOW emBlow, DWORD dwWeather, BOOL bWeatherActive );

	//	Note : ü�� ���� ��ġ���� ��ȭ���� ���� �������ش�.
	//		���ÿ��� int ������ ����� �����Ͽ� ����� �ս��� ���´�.
	//		�� ����ÿ��� WORD �� ���� ���� �̳��� ����.
	inline void UPDATE_POINT ( GLPADATA &sPOINT, float &fELP_VAR, const float fVAR, const WORD wLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(sPOINT.wNow) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	WORD �� ���� ���� �̳��� ����.
		if ( nNEWP < 0 )			nNEWP = 0;
		if ( nNEWP > USHRT_MAX )	nNEWP = USHRT_MAX;

		if ( nNEWP < wLOW_LMT )		nNEWP = wLOW_LMT;

		sPOINT.wNow = nNEWP;
		sPOINT.LIMIT ();
	}

	inline void UPDATE_POINT ( GLDWDATA &sPOINT, float &fELP_VAR, const float fVAR, const int nLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(sPOINT.nNow) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	int �� ���� ���� �̳��� ����.
		if ( nNEWP < 0 )
			nNEWP = 0;

		if ( nNEWP < nLOW_LMT )
			nNEWP = nLOW_LMT;

		sPOINT.nNow = nNEWP;
		sPOINT.LIMIT ();
	}

	//	Note : ü�� ���� ��ġ���� ��ȭ���� ���� �������ش�.
	//		���ÿ��� int ������ ����� �����Ͽ� ����� �ս��� ���´�.
	//		�� ����ÿ��� WORD �� ���� ���� �̳��� ����.
	inline void UPDATE_POINT ( DWORD &dwNowP, float &fELP_VAR, const DWORD MaxP, const float fVAR, const DWORD dwLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(dwNowP) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	WORD �� ���� ���� �̳��� ����.
		if ( nNEWP < 0 )				nNEWP = 0;
		if ( nNEWP < (int)dwLOW_LMT )	nNEWP = dwLOW_LMT;

		dwNowP = nNEWP;
		if ( dwNowP > MaxP )		dwNowP = MaxP;
	}

	inline void UPDATE_POINT ( WORD &wNowP, float &fELP_VAR, const WORD MaxP, const float fVAR, const WORD wLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(wNowP) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	WORD �� ���� ���� �̳��� ����.
		if ( nNEWP < 0 )			nNEWP = 0;
		if ( nNEWP > USHRT_MAX )	nNEWP = USHRT_MAX;

		if ( nNEWP < wLOW_LMT )		nNEWP = wLOW_LMT;

		wNowP = nNEWP;
		if ( wNowP > MaxP )			wNowP = MaxP;
	}

    //! �̵��ӵ�
    float GetMoveBaseVelocity(float StateMove, float SkillMove, float ItemMove, float OptionMove);
    float GetMoveVelocity(
        BOOL bRun, // ����
        float RunVelo,  // �ٱ� �ӵ�
        float WalkVelo, // �ȱ� �ӵ�
        bool bUseBoost,  // Boost ��뿩��
        float MoveVelo, // +�̵��ӵ�
        float ItemVelo, // +������ �̵��ӵ�
        float MoveVeloLimitMin,  // �ּ� �̵��ӵ�;
        float MoveVeloLimitMax,  // �ִ� �̵��ӵ�;
        bool bCdmSafe = false,  // Club Death Match
        float CdmSpeed = 1.0f // Club Death Match safe time speed
    ); 

    //! ���ݼӵ�
    float GetAttackVelocity(float AttackVelo, float fAddAttackVelo);

}; // namespace GLOGICEX

enum EMBEGINATTACK_FB
{
	EMBEGINA_OK		= 0,
	EMBEGINA_SP		= 1,
	EMBEGINA_ARROW	= 2,
	EMBEGINA_BULLET = 3,
};

typedef std::list<DWORD>	            SUMMONIDLIST;
typedef SUMMONIDLIST::iterator		    SUMMONID_ITER;

typedef std::list<DWORD>	            LANDEVENTIDLIST;
typedef LANDEVENTIDLIST::iterator		LANDEVENTID_ITER;

//Serialization�� ���ԵǾ�� �� �׸� üũ�� ��ũ��
#define SERIALIZE

//Serialization - �ϴ� �� ����

//	Note : ���� �߰��ÿ�. RESET_DATA() �Լ��� ���� �ʱ�ȭ �־�ߵ�.
//
struct GLCHARLOGIC : public SCHARDATA2
//struct GLCHARLOGIC : public ic::CRttiObject< GLCHARLOGIC, SCHARDATA2, 1, ic::EInternal >
{
private:
	typedef std::map< DWORD, EMSKILL_VALIDATION >		SKILL_VALIDATION_MAP;
	typedef SKILL_VALIDATION_MAP::iterator				SKILL_VALIDATION_MAP_ITER;
	typedef SKILL_VALIDATION_MAP::const_iterator		SKILL_VALIDATION_MAP_CITER;

public:
	DWORD		m_dwGlobalID;						//�ش� ĳ������ ���������� ��ȿ�� ����ID

	SNATIVEID	m_idACTIVESKILL;				//	���� ��Ƽ��� ��ų.
	SNATIVEID	m_idIllusionSKILL;

	EMCHARINDEX		m_CHARINDEX;			// ����+����
	SKILL::DELAY_MAP			m_SKILLDELAY;			// ���� ��ų�� ������.

	SSKILLFACT	m_sSKILLFACT[SKILLFACT_SIZE];	// ��ųȿ����
	SSKILLFACT	m_sPETSKILLFACT[MAX_PETSKILLSLOT]; //  ���� ������ųȿ��

	VEC_LANDEFF_VALUE	m_vecLandEffect;		//  ���� ȿ����
	VEC_MAP_EFFECT		m_vecMapEffect;			//	�ʿ� ����Ǵ� ȿ����

	DWORD				m_dwActionLimit;					//	�ൿ �׼� ���� ����	
	DWORD				m_dwTransformSkill;				//  ���� ��ų �ѹ�
	SNATIVEID			m_IdSkillTransform;				// ���� ��ü ��ų �ε���

    //SUMMONIDLIST        m_dwSummonGUIDList;		// ��ȯ�� GUID ����Ʈ
    LANDEVENTIDLIST     m_dwLandEventList;		// ���� �̺�Ʈ GUID ����Ʈ

	DWORD				m_dwHOLDBLOW;					//	�����̻��� ������.
	bool					m_bSTATEBLOW;					//	�����̻��� �ϳ� �̻� �߻�������.
	SSTATEBLOW		m_sSTATEBLOWS[EMBLOW_MULTI];	//	�����̻�.	

	SQITEM_FACT			m_sQITEMFACT;				//	? ������ ���밪.
	SEVENT_FACT			m_sEVENTFACT;				//	�̺�Ʈ ? ������ ���밪.

	STAUNT					m_sTaunt;						// ���� ȿ��;
	SHALLUCINATE			m_sHALLUCINATE;			// ȯ�� ȿ��.
	SRELEASE_ENDURE	m_sRELEASE_ENDURE;		// �ǰݽ� ȿ�� ����.
	SINVISIBLE				m_sINVISIBLE;					// ���� �� ���Ű���.
	SRECVISIBLE			m_sRECVISIBLE;				// ���� ����.
	SDOMINATE				m_sDOMINATE;				// ���� ȿ��.
	SSTIGMA				m_sSTIGMA_USER;			// ���� ȿ�� - ���.
	SSTIGMA				m_sSTIGMA_TAKER;			// ���� ȿ�� - ����.
	SLINKHP					m_sLINKHP;						// ���� ȿ��	
	SINCREASEEFF			m_sIncreaseEff;				// ��ȭ ȿ��;
	SIGNORED_DAMAGE	m_sIgnoredDamage;			// ������ ����(����);
	SEXPERIENCERATE	m_ExperienceRate;			// ���� ����ġ ������;
	SVIEWRANGERATE	m_sRateViewRange;			// �þ� ����;
	SAIRBORNE				m_sAirborne;					// ���;
	SCLONE				m_sClone; //���� ȿ��

	CSkillCAIdx			m_SkillCAIndex;			// �ݰݸ���Ʈ

	float		m_fHP_TOTALRATE;				// �� ������
	float		m_fMP_TOTALRATE;
	float		m_fSP_TOTALRATE;

	float		m_fHP_RATE;						// active ��ų�� HP ������.
	float		m_fMP_RATE;						// active ��ų�� MP ������.
	float		m_fSP_RATE;						// active ��ų�� SP ������.

	float		m_fSTATE_MOVE_RATE;				// ���� �̻� �̵��ӵ�. ( 1.0f == 100% )
	float		m_fSTATE_MOVE_VALUE;			// �̵� �ӵ� (Ż�Ͱ� ������ ��ġ ����)
	float		m_fSTATE_DELAY;					// ���� �̻� ������. ( 1.0f == 100% ) (+)
	float		m_fSTATE_DAMAGE;				// ���� �̻� ����� ����.	

	float		m_fSKILL_LIFE_INC;				//  ��ų�ð� ����
	float		m_fSKILL_MOVE;					// ��ų �̵��ӵ�.
	float		m_fOPTION_MOVE;					// �߰����� �̵��ӵ�. ���忡���Ϳ��� ���.

	float		m_fSUM_RANGE_PSY_LONG;			// ���� ����� ���� ���ɰŸ� �ջ�. ( �����Ÿ�, ���뿵�� ) 
	float		m_fSUM_TARRANGE_ALL;			// �����Ÿ� ���� �ɼ� ( ��ü ��ų Ÿ�� ) 
	float		m_fSUM_APPLYRANGE_ALL;			// ���뿵�� ���� �ɼ� ( ��ü ��ų Ÿ�� )

	float		m_fDamageRate;					// ���ݷ� ��ȭ�� (��ų ������ + ������). EMSPECA
	float		m_fDefenseRate;					// ���� ��ȭ��. EMSPECA
	float		m_fHitRate;						// ���߷� ��ȭ��  EMSPECA
	float		m_fAvoidRate;					// ȸ���� ��ȭ��  EMSPECA

	float		m_fBasicRateHP;					// HP ��ȭ�� (�꽺ųȿ�� ���� ��ġ);
	float		m_fVariationRateHP;             // HP ��ȭ�� (��ü ��ġ���� %�� ����);
	float		m_fVariationRateMP;             // MP ��ȭ��;
	float		m_fVariationRateSP;             // SP ��ȭ��;
	
	float		m_fATTVELO;						// ��ų ���ݼӵ� ��ȭ.

	float		m_fSKILLDELAY;

	float		m_fEXP_RATE;					// ����ġ ������.
	float		m_fQItem_EXP_RATE;				// QItem ����ġ ������.
	float		m_fSkill_EXP_RATE;				// ��ų ����ġ ������.
	float		m_fRANDOM_BOX_EXP_RATE;			// �����ڽ� ����ġ ������.
	float		m_fGENITEM_RATE;				//	������ �߻���.
	float		m_fGENMONEY_RATE;				//	�ݾ� �߻���.
	
	int			m_nHP_ADD_SKILL;		//  HP ����. EMSPECA
	int			m_nMP_ADD_SKILL;		//  MP ����. EMSPECA
	int			m_nSP_ADD_SKILL;		//  SP ����. EMSPECA
	int			m_nCP_ADD_SKILL;		//  CP ����. EMSPECA

	int			m_nPotion_HP_Inc_SKILL; // HP ���� ȸ���� EMSPECA
	int			m_nPotion_MP_Inc_SKILL; // MP ���� ȸ���� EMSPECA
	int			m_nPotion_SP_Inc_SKILL; // SP ���� ȸ���� EMSPECA
	
	int			m_nPA_ADD;				// PA ���� EMSPECA
	int			m_nMA_ADD;				// MA ���� EMSPECA
	int			m_nSA_ADD;				// SA ���� EMSPECA

	float		m_fVariationApplyRateHP;		// HP ��ȭ�� ����� EMSPECA
	float		m_fVariationApplyRateMP;		// MP ��ȭ�� ����� EMSPECA
	float		m_fVariationApplyRateSP;		// SP ��ȭ�� ����� EMSPECA
	
	int			m_nGET_CP_VALUE;					// ȹ�� CP ������
	
    // ����: ��ȭ�� ����Ŀ� ���� �Ҽ��������� ������ �ӽ÷� �����ϱ� ���� �����Դϴ�.
	float		m_fTempVarHP;
	float		m_fTempVarMP;
	float		m_fTempVarSP;
	float		m_fTempVarCP;
	float		m_fCP_Timer;

	float		m_fJumpRange;

	float		m_fSUMMONACTIVETIME;		// ��ȯ�ð� ����.

	int			m_sumPowerAttribute[SKILL::EMAPPLY_NSIZE]; // ����/���/����;
	int			m_powerAttack;			    	//	�⺻ ���ݷ�.
	int			m_powerDefence;					//	�⺻ ����.
	
	int			m_nHP_ADD;				//  HP ����.  EMIMPACTA
	int			m_nMP_ADD;				//  MP ����.  EMIMPACTA
	int			m_nSP_ADD;				//  SP ����.  EMIMPACTA
	int			m_nCP_ADD;				//  CP ����.  EMIMPACTA
	
	int			m_nSUM_PIERCE;			//	���� ���� �ջ�.

	int			m_nHIT;					//	������.
	int			m_nSUM_HIT;				//	������ + ��ųȿ��. EMSPECA, EMFOR

	int			m_nAVOID;					//	ȸ����.
	int			m_nSUM_AVOID;			//	ȸ���� + ��ųȿ��. EMSPECA, EMFOR

	int			m_nDEFENSE_BODY;		//	���� ( DP + DEX+��� ).
	int			m_nDEFENSE;				//	���� ( DP + DEX*��� + ITEM_DEF ) * �������� ����.
	int			m_nDEFENSE_SKILL;		//	���� ( DP + DEX*��� + ITEM_DEF ) * �������� ���� + ��ų. EMSPECA

	bool		m_bSTATE_PANT;                  //	�����̻� �涱��.
	bool		m_bSTATE_STUN;                  //	�����̻� ����.
	bool		m_bSTATE_CONTROL;				//	ĳ���� ��Ʈ�� ����.

	bool		m_bUseArmSub;					// �ذ��θ� ���� ���� ���� ��뿩��	

	bool		m_bActionType[SKILL::EMACTION_TYPE_NSIZE];	// ����ȿ�� UI Ȯ��
	bool		m_bSkillFactsSpecial[EMSPECA_NSIZE];	// �ɷ��ִ� ����� ȿ��;
	bool		m_bSafeZone;

	DEFENSE_SKILL		m_sDefenseSkill;				//  �ߵ��� ��ų
	SCHARSTATS          m_sSUMSTATS;                    //	�⺻���� + ���ɼ� Stats.
	SCHARSTATS          m_sSUMSTATS_SKILL;              //	�⺻���� + ���ɼ� +��ų ���� Stats.

	SRESIST				m_sSUMRESIST;					//	���װ�. ( ������ + �нú� )
	SRESIST				m_sSUMRESIST_SKILL;			// ��ų ���װ� ( ������ + �нú� + ���� ) 
	SSUM_ITEM			m_sSUMITEM;						// ������ ���갪. 
	SPASSIVE_SKILL_DATA	m_sSUM_PASSIVE;		// �нú� ���갪.	

	WORD				m_wACCEPTP;						// ���� �����ۿ� �ɷ����� ��� ��ġ.
	WORD				m_wACCEPTP_SKILL;				// ���� �����ۿ� �ɷ����� ��� ��ġ ��ų ���ʽ�. EMSPECA
	WORD				m_wSUM_DisSP;					// �Ҹ� SP �ջ�.
	WORD				m_wATTRANGE;					// ���� ���� �Ÿ�.
	WORD				m_wATTRANGE_SKILL;				// ���� ���� �Ÿ� ��ų ���ʽ�. EMSPECA
	WORD				m_wSUM_ATTRANGE;				// ���� ���� �Ÿ�.

	GLDWDATA			m_gdDAMAGE;					//	�⺻ ���ݷ�. ( AP + ��ú� ��ų�� ����� ���� )
	GLDWDATA			m_gdDAMAGE_SKILL;			//	�⺻ ���ݷ� + temp ��ųȿ��. EMSPECA
	GLDWDATA			m_gdDAMAGE_BASIC;			//	�⺻ ���ݷ� + ������� + (����ġor���ġor����ġ).	

	DAMAGE_SPEC			m_sDamageSpec;					//	������ ���� �� �ݻ�
	
	SDURATION_DAMAGE	m_sDurDamage[EMELEMENT_MAXNUM];	//	���� ������
	
	GLITEM_ATT	m_emITEM_ATT;					//	������ ���� �Ӽ�.
	EMANI_SUBTYPE		m_emANISUBTYPE;					//	Item ���뿡 ���� ���ϸ��̼� ����.
	
	EMANI_MAINTYPE		m_emANIMAINSKILL;				//	��ų�� ���� ���ϸ��̼�.
	EMANI_SUBTYPE		m_emANISUBSKILL;				//	��ų�� ���� ���ϸ��̼�.
	EMANI_MAINTYPE		m_emANIVehicleSKILL;	    	//	��ų�� ���� ���ϸ��̼�.

	SITEM*				m_pITEMS[SLOT_TSIZE];

	

	MAPPLAYHOSTILE		m_mapPlayHostile;				//	���� ������. ( �ֹ� )

	EMPOSTBOXID_TYPE	m_emPOSTBOXIDTYPE;				//  ������� ������ ID Ÿ��
	SNATIVEID			m_sPOSTBOXID;					//  ������� ������ ID
	

	// ������ Ÿ��Ʋ�� TitleManager Ŭ�������� ������
	// ĳ���� ��ȸ �� ��� �־�δ� �������� ��� ���ε�,
	// ���� ���õ� Ÿ��Ʋ�� ���⿡ �־�δ� �͵� �����ҵ�..
	char				m_szTitle[ EM_TITLEID_MAXLENGTH ];

	CSkillEffSet		m_EffSkillVarSet;

	BOOL				m_bParty;

protected:
	EMVEHICLE_WHETHER	m_emVehicle;					//! Ż�� Ȱ��ȭ ����
    float				m_fVehicleSpeedRate;
    float				m_fVehicleSpeedVol;

	CHARACTER_POINT		m_PointShopPoint;		//! PointShop point

	WORD				m_ItemGarbagePosX;		//	������ ���� ������ ��ġ ����
	WORD				m_ItemGarbagePosY;

    float               m_fMoveVeloLimit_Min;   // ���� �̵��ӵ� ����;
    float               m_fMoveVeloLimit_Max;   // �ְ� �̵��ӵ� ����;


private:
	// ��ų ��ȿ�� ��;
	SKILL_VALIDATION_MAP	m_mapValidSkill;


public:
	GLCHARLOGIC();
	virtual ~GLCHARLOGIC();

    void RESET_DATA();
    VOID RegistProperty();
	void RESET_SKILL_DATA();

public:

	CSkillCAIdx* GetSkillCA() { return &m_SkillCAIndex; }

	void		 GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA );

	inline CHARACTER_POINT PointShopPointGet() const { return m_PointShopPoint; }
    __int64 PointShopPoint() const { return m_PointShopPoint.Total(); }

    inline bool IsCheckedSkillFlagSpecial(const DWORD dwFlag) const { return m_bSkillFactsSpecial[dwFlag]; }

	float GetQuestionTime()			{ return m_sQITEMFACT.fTime; }
    
    //! Ż�� Ȱ��ȭ ����
    EMVEHICLE_WHETHER VehicleState() const { return m_emVehicle; }
    float VehicleSpeedRate() const { return m_fVehicleSpeedRate; }
    void VehicleSpeedRateSet(float Rate) { m_fVehicleSpeedRate=Rate; }
    float VehicleSpeedVol() const { return m_fVehicleSpeedVol; }
    void VehicleSpeedVolSet(float Vol) { m_fVehicleSpeedVol=Vol; }

	inline bool IsEntranceExitSkillApply() { return RF_ENTRANCE_EXIT(m_EffSkillVarSet).IsOn(); }

public:
	EMCROW GETCROW () const			{ return CROW_PC; }
	WORD GETLEVEL () const			{ return m_wLevel; }

	WORD GetPow() const				{ return m_sSUMSTATS.wPow; }
	WORD GetDex() const				{ return m_sSUMSTATS.wDex; }
	WORD GetSpi() const				{ return m_sSUMSTATS.wSpi; }
	WORD GetStr() const				{ return m_sSUMSTATS.wStr; }
	WORD GetSta() const				{ return m_sSUMSTATS.wSta; }

	// ��ų ������ ���Ե� ����
	WORD GetPow_Skill() const		{ return m_sSUMSTATS_SKILL.wPow; }
	WORD GetDex_Skill() const				{ return m_sSUMSTATS_SKILL.wDex; }
	WORD GetSpi_Skill() const				{ return m_sSUMSTATS_SKILL.wSpi; }
	WORD GetStr_Skill() const				{ return m_sSUMSTATS_SKILL.wStr; }
	WORD GetSta_Skill() const				{ return m_sSUMSTATS_SKILL.wSta; }

	WORD GetAddedPow() const		{ return m_sStats.wPow; }
	WORD GetAddedDex() const		{ return m_sStats.wDex; }
	WORD GetAddedSpi() const		{ return m_sStats.wSpi; }
	WORD GetAddedStr() const		{ return m_sStats.wStr; }
	WORD GetAddedSta() const		{ return m_sStats.wSta; }
	
	const int GETHP () const			{ return m_sHP.nNow; }
	const int GETMAXHP () const			{ return m_sHP.nMax; }
    float GETMAXHPRATE() const      { return m_sLINKHP.fRef; }
    void  SETMAXHPRATE( float fRate ) { m_sLINKHP.fRef = fRate; }

	int GETMP () const				{ return m_sMP.nNow; }
	int GETMAXMP () const			{ return m_sMP.nMax; }

	int GETSP () const				{ return m_sSP.nNow; }
	int GETMAXSP () const			{ return m_sSP.nMax; }

	int GETCP () const				{ return m_sCP.wNow; }
	int GETMAXCP () const			{ return m_sCP.wMax; }

	LONGLONG GETEXP() const			{ return m_sExperience.lnNow; }
	LONGLONG GETMAXEXP() const		{ return m_sExperience.lnMax; }

	int GETHIT () const				{ return m_nSUM_HIT; }
	int GETAVOID () const			{ return m_nSUM_AVOID; }
	// RM# 1873 [9/14/2015 gbgim];
	// ���� �Ʒ��� �� �Լ��� UI ����� ���ؼ��� �����;
	float GetHitRate() const		{ return static_cast<float>(m_nSUM_HIT)/GLCONST_CHAR::fHitRatePercentUnit; }
	float GetAvoidRate() const		{ return static_cast<float>(m_nSUM_AVOID)/GLCONST_CHAR::fAvoidRatePercentUnit; }
	EMBRIGHT GETBRIGHT () const		{ if(m_nBright<0) return BRIGHT_DARK; return BRIGHT_LIGHT; }
	int GETBRIGHTPER () const		{ return m_nBright; }
	int GETFORCE_LOW () const		{ return m_gdDAMAGE_BASIC.nNow; }
	int GETFORCE_HIGH () const		{ return m_gdDAMAGE_BASIC.nMax; }
	int GETDEFENSE () const			{ return m_nDEFENSE_SKILL; }
	
	float GETSUM_RANGE_PSY_LONG () const { return m_fSUM_RANGE_PSY_LONG; }		//	�Ϲݰ����϶��� ����.
	
	float GETSUM_TARRANGE_ALL () const { return m_fSUM_TARRANGE_ALL; }
	float GETSUM_APPLYRANGE_ALL () const { return m_fSUM_APPLYRANGE_ALL; }

	int GETSUM_PIERCE () const		{ return m_nSUM_PIERCE; }
	
	WORD GETBODYRADIUS () const		{ return GLCONST_CHAR::wBODYRADIUS; }
	WORD GETATTACKRANGE () const	{ return m_wSUM_ATTRANGE; }	

	const int GETVISIBLESIGHT(void) const { return int(m_sRateViewRange * MAX_VIEWRANGE); }
	const float GETVISIBLESIGHT_RATE(void) const { return m_sRateViewRange.fRate; }

	FLOAT GetExpMultiple() const	{ return m_sSUMITEM.fExpMultiple; }

	const int GETSUMMONDAMGE ();

	const SCHARSTATS& GETSTATS_ADD () const;
	const SCHARSTATS& GETSTATS_ITEM () const;

	void OptionMoveUp();
	void OptionMoveDown();

	void SetUseArmSub( BOOL bSub )	{ m_bUseArmSub = bSub; }
	const bool IsUseArmSub(void) const		{ return m_bUseArmSub; }

	const EMSLOT GetCurRHand(void) const;
	const EMSLOT GetCurLHand(void) const;

	BOOL IsCurUseArm( EMSLOT emSlot );

	bool IsDefenseSkill()				{ return m_sDefenseSkill.m_bActive; }
	bool IsDefenseSkill( const SNATIVEID& sSkillID ) { return m_sDefenseSkill.m_bActive ? (m_sDefenseSkill.m_dwSkillID == sSkillID) : false; }	
	void SetDefenseSkill(bool bDefense)	{ m_sDefenseSkill.m_bActive = bDefense; }

	bool ReleaseCureEffect( DWORD dwCheckList );
	DWORD SkillEffectType( DWORD iReleaseType );

public:
	void SLOT_ITEM ( const SITEMCUSTOM &sItemCustom, EMSLOT _slot );
	void RELEASE_SLOT_ITEM ( EMSLOT _slot );

	const SITEMCUSTOM& GET_SLOT_ITEM(EMSLOT _slot) const { return m_PutOnItems[_slot]; }
	const SNATIVEID GET_SLOT_NID(EMSLOT _slot) const { return m_PutOnItems[_slot].GetNativeID(); }
	SITEM* GET_SLOT_ITEMDATA(EMSLOT _slot) { return m_pITEMS[_slot]; }
	BOOL VALID_SLOT_ITEM(EMSLOT _slot);
	BOOL VALID_SLOT_ITEM_ALLWEAPON ( EMSLOT _emSlot );

	virtual void HOLD_ITEM( const SITEMCUSTOM& sItemCustom );
	void RELEASE_HOLD_ITEM();
	const SITEMCUSTOM& GET_HOLD_ITEM() const { return m_PutOnItems[SLOT_HOLD]; }

    //! ���콺�� ��� �ִ� �������� �ִ°�?
	BOOL HaveHoldItem() const { return GET_HOLD_ITEM().GetNativeID()!=NATIVEID_NULL(); }

	const GLITEM_ATT CONVERT_ITEMATT ( GLITEM_ATT emAttack );

	BOOL ISLONGRANGE_ARMS ();
	BOOL ISLONGRANGE_ARMS ( GLITEM_ATT emAttack );
	SKILL::EMAPPLY GetStatType();

	BOOL ISHAVEITEM ( SNATIVEID sNID, DWORD dwNum );
    DWORD GETHAVEITEM( SNATIVEID sNID );
	SITEM* FindRevive( EMSLOT* pemSlot = NULL );

	SITEM* GET_ELMT_ITEM ();	//	�޼�, ������ �������߿��� 'BLOW' �Ӽ��� �ִ� �������� ��ȯ.

	WORD GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const;
	WORD GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const;

public:
	EMBEGINATTACK_FB BEGIN_ATTACK ( WORD wStrikeNum );
	BOOL VALID_LEVELUP () const;
	LONGLONG GET_LEVELUP_EXP () const;
	void LEVLEUP ( bool bInitNowExp = TRUE, bool bJumping = false, bool bCalExp = true );
	void STATSUP ( EMSTATS emStats );

	bool RESET_STATS_SKILL ( const WORD* pResetStats=NULL );
	bool RESET_SKILL ();

	bool RESET_STATS ( const WORD* pResetStats=NULL );
	
	// Note : �̻�� (������ �� ���� ����)
	bool RESET_STATS_OLD ( const WORD wDIS=USHRT_MAX ); 

	void GET_EVENLY_DECREASE_STATS ( WORD* pOutResetStats, WORD wAmount );

protected:
	void SUM_ITEM ();
	void SUM_ITEM_BASIC ();
	bool SUM_ITEM_BASIC_WEAPON( EMSLOT emSLOT, float fDurability, const SITEMCUSTOM& sItemCustom );

	void SUM_ITEM_COSTUM ();
	void SUM_ITEM_SET ();
	void SUM_ITEM_ADDOP_LIMIT();
	void GLCHARLOGIC::SUM_PASSIVE_BASIC( PGLSKILL pSkill, WORD wLev );
	void GLCHARLOGIC::SUM_PASSIVE_ADDON( PGLSKILL pSkill, WORD wLev );
	void GLCHARLOGIC::SUM_PASSIVE_SPEC( PGLSKILL pSkill, WORD wLev );
	void SUM_PASSIVE ();
	void SUM_ADDITION ( float fCONFT_POINT_RATE );

	void RESET_PASSIVE ();

	bool SUM_ITEM_BASIC_CS( const SITEMCUSTOM& rItemCustom, float fDurability, EMSLOT eSlot);

public:
	void INIT_NEW_CHAR ( const EMCHARINDEX _emCIndex, const DWORD dwUserID, const DWORD dwServerID, const char* szCharName,
		const WORD _wSchool=0, const WORD wHair=0, const WORD wFace=0, const WORD _wHairColor=0, const WORD _wSex=0, const BOOL bJumping = false );

	BOOL INIT_DATA ( BOOL bNEW, BOOL bReGen, float fCONFT_POINT_RATE = 1.0f, bool bInitNowExp = TRUE, bool bJumping = false );

	void INIT_RECOVER ( int nRECOVER=30 );

public:
	void			PrevUPDATE_DATA(const float fElapsedTime);
	
	void			UPDATE_DATA_SetItemBonus( );
	void			UPDATE_DATA_SkillAddon( const SKILL::SIMPACT_ADDON_LEVEL& addon, int& nSUM_PA, int& nSUM_SA, int& nSUM_MA );
	void			UPDATE_DATA_SkillSpecial( const SSKILLFACT& sSKEFF, const SKILL::SSPEC_ADDON_LEVEL& spec, SDURATION_DAMAGE* sDurDamage, const DWORD dwSkillFactSlot, BOOL bClient = FALSE );
	void 			UPDATE_DATA_SkillFacts( float	fElapsedTime, BOOL bClient, int& nSUM_PA, int& nSUM_SA, int& nSUM_MA, SDURATION_DAMAGE* sDurDamage );
	void 			UPDATE_DATA_BlowMulti( float fElapsedTime, BOOL bClient );
	void 			UPDATE_DATA_QItemFact();
	void 			UPDATE_DATA_LandEffects();
	void 			UPDATE_DATA_MapEffects();
	void 			UPDATE_DATA_PetSkill( float fElapsedTime );
	void 			UPDATE_DATA_Stats( int& nSUM_PA, int& nSUM_SA, int& nSUM_MA );
	void 			UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient=FALSE, float fCONFT_POINT_RATE=1.0f );
	void 			UPDATE_MAX_POINT ( float fCONFT_POINT_RATE, const bool bRATE_MAINTENANCE=true );
	bool 			UPDATE_QITEMFACT ( float fElapsedTime );
	virtual void	UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE, BOOL bForceEnd = FALSE, BOOL bOverLab = FALSE ){;} // ����ȿ�� ���� �� ȣ��;
	virtual void	UPDATE_DATA_Additional(const float fElapsedTime){;} // Update_data ���� �� ȣ��;

	virtual void	Play_ScreenThrow( DWORD dwIndex, const STARGETID& sID ) {};
	virtual void	Stop_ScreenThrow( DWORD dwIndex ) {};

	virtual void	Play_CameraHit( WORD wEffNum ) {};
	virtual void	Stop_CameraHit( WORD wEffNum ) {};

	virtual void	End_FeignDeath( BOOL bForce ) {};

	virtual void	Play_Disguise( int iIdx, BOOL bOn ) {};
	virtual void	Stop_Disguise() {};

	virtual void	Play_ActiveDamage() {};
	virtual void	Stop_ActiveDamage() {};
	
	void Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID );

public:
	EMSLOTCHECK_RESULT	CHECKSLOT_ITEM ( SNATIVEID sNativeID, EMSLOT emSlot );
	BOOL			ACCEPT_ITEM( const SITEMCUSTOM& sItemCustom );
	BOOL 			ISEMPTY_SLOT ( SNATIVEID sNativeID, EMSLOT emSlot );
	WORD			CALC_ACCEPTP ( const SITEMCUSTOM& sItemCustom );
	WORD 			CALC_ACCEPTP_EX ( const SITEMCUSTOM& sItemCustom );
	BOOL 			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom );
	BOOL			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, EMINVEN_USE_ITEM& emMsg ); //������ ����Ȯ�� : ���ܰ�� enum�� emMsg�� ���� �� FALSE ���� 
	BOOL 			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, SNATIVEID sDisguiseID );
	BOOL 			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, SNATIVEID sDisguiseID, EMINVEN_USE_ITEM& emMsg );
	virtual BOOL 	CHECK_ACTIVITY_POINT ( const SITEM& sItem ) { return TRUE; }
	virtual BOOL 	CHECK_CONTRIBUTION_POINT ( const SITEM& sItem ) { return TRUE; }

	// ��� ��� ��ų�� ��ȿ���� üũ�Ͽ� �����Ѵ�;
	void						SET_VALIDATION_EXPSKILL_MAP ();

	// Ư�� ��ų�� ��ȿ���� üũ�Ѵ� ( ���õ� �ʿ��� ã�´� );
	const EMSKILL_VALIDATION	VALIDATION_EXPSKILL ( const DWORD _dwSkillID );

private:
	// ������ ��ȿ���� üũ�ϴ� �Լ��̴� ( Private );
	const EMSKILL_VALIDATION	_ValidationSkill ( const SNATIVEID& _sSkillID );

public:	
	WORD RECEIVE_DAMAGE(const int nDamage);
		virtual void RECEIVE_DAMAGE_AFTER(void){;}
		void RELEASE_ENDURE_PROC(void);
	WORD RECEIVE_HEAL(const int nHeal);

public:
	BOOL RECEIVE_QITEMFACT ( const SNATIVEID &nidITEM );
	//	Memo :	�̺�Ʈ�� ���� �޴� ?������ ȿ���� �����Ѵ�.
	void RECEIVE_EVENTFACT_BEGIN ( const EMGM_EVENT_TYPE emType, const WORD wValue );
	//	Memo :	�̺�Ʈ�� ���� �޴� ?������ ȿ���� �����Ѵ�.
	void RECEIVE_EVENTFACT_END ( const EMGM_EVENT_TYPE emType );

public:
	BOOL ISLEARNED_SKILL( SNATIVEID skill_id, WORD wLEVEL=0 );
	BOOL ISMASTER_SKILL ( SNATIVEID skill_id );
	SCHARSKILL* GETLEARNED_SKILL ( SNATIVEID skill_id );

	EMSKILL_LEARNCHECK CHECKLEARNABLE_SKILL ( SNATIVEID skill_id );
	EMSKILL_LEARNCHECK CHECKLEARNABLE_SKILL ( SNATIVEID skill_id, WORD wLevel );
	void LEARN_SKILL ( SNATIVEID skill_id );
	void LEARN_SKILL_SIMPLE( SNATIVEID skill_id );
	VOID LVLUP_SKILL ( SNATIVEID skill_id, WORD wToLevel );

	bool LEARN_SKILL_QUEST ( SNATIVEID skill_id );

	void LEARN_CHANGE_SKILL( const GLSKILL* pOrgSkill, WORD wLv );
	BOOL ISLEARNED_SKILLEX( SNATIVEID skill_id, SCHARSKILL& charSkill );
	void ChangeToOrgSkill(const SNATIVEID& rID, SNATIVEID& sOrgSkill );
	void CHANGE_CHARSKILL( const SCHARSKILL& OrgValue, SCHARSKILL& ChangeValue );
	void UPDATE_CHANGE_CHARSKILL( const SCHARSKILL& OrgValue );

	void SAVE_CURRENT_SLOT_INFO ( void );
	const bool CHANGE_CHARACTER_SLOT ( const WORD _wSlotIndex, const bool _bForce = false );

	const bool GET_CHARACTER_SLOT_LOCKSTATE ( const WORD _wSlotIndex ) const;
	const WORD GET_CURRENT_CHARACTER_SLOT ( void ) const;

	const bool IS_USEABLE_SKILL ( const SNATIVEID& sSkillID, const WORD wSkillLevel ) const;

public:
	virtual float GETCRITICALVELO();
	// �Ʒ��� ������ ���ϴ°ǵ�, Update_data���� �Ź� ���ϰ� �߰������� �ش� �Լ��� ���ϸ鼭;
	// �߰� ������ �Ѵ�, �ǵ�������;
	// �ᱹ�� Update_data���� ���س� �����ʹ� �ٸ������� ��������ʰ� �Ʒ������Ѵ�;
	virtual float GETATTVELO();
	virtual float GETMOVEVELO();
	virtual float GETEXPVELO();	// �ΰ�����â �����ֱ� ���� �Լ�
	virtual float GETCRITICAL_ITEM();
	virtual float GETATT_ITEM(); // ����ġ ����
	virtual float GETMOVE_ITEM(); // ����ġ ���� % 

	virtual float GETCRITICAL_TOTAL(); // GETCRITICALVELO() + GETCRITICAL
	virtual float GETATT_TOTAL(); // GETATTVELO() + GETATT_ITEM()
	virtual float GETMOVE_TOTAL(); // GETMOVEVELO() + GETMOVE_ITEM() 

	virtual float	GetInc_SkillDurationTAdd(WORD wGrade)	{ if ( wGrade < 0 || SKILL::MAX_GRADE <= wGrade ) { return 0.f; } return m_sSUMITEM.fInc_SkillDurationTAdd[wGrade]; }
	virtual float	GetIncR_SkillDamageAdd(WORD wGrade)	{ if ( wGrade < 0 || SKILL::MAX_GRADE <= wGrade ) { return 0.f; } return m_sSUMITEM.fIncR_SkillDamageAdd[wGrade]; }
	virtual float	GetDecR_SkillDelayT(WORD wGrade)	{ if ( wGrade < 0 || SKILL::MAX_GRADE <= wGrade ) { return 0.f; } return m_sSUMITEM.fDecR_SkillDelayT[wGrade]; }
	virtual float	GetIncR_CriticalDamage()	{ return m_sSUMITEM.fIncR_CriticalDamage; }
	virtual float	GetIncR_CrushingBlow()		{ return m_sSUMITEM.fIncR_CrushingBlowDamage; }
	virtual float	GetIncR_LifePerKill()		{ return m_sSUMITEM.fIncR_LifePerKill; }
	virtual float	GetIncR_MPPerKill()		{ return m_sSUMITEM.fIncR_MPPerKill; }
	virtual float	GetIncR_SPPerKill()		{ return m_sSUMITEM.fIncR_SPPerKill; }
	virtual float	GetIncR_HPMPSPPerKill()	{ return m_sSUMITEM.fIncR_HPMPSPPerKill; }
	virtual float	GetIncR_AllMPRatioToAttack()		{ return m_sSUMITEM.fIncR_AllMPRatioToAttack; }
	virtual float	GetIncR_AllSPRatioToHp()			{ return m_sSUMITEM.fIncR_AllSPRatioToHp; }
	virtual float	GetIncR_AllHitRatioToMelee()		{ return m_sSUMITEM.fIncR_AllHitRatioToMelee; }
	virtual float	GetIncR_AllHitRatioToRange()		{ return m_sSUMITEM.fIncR_AllHitRatioToRange; }
	virtual float	GetIncR_AllHitRatioToMagic()		{ return m_sSUMITEM.fIncR_AllHitRatioToMagic; }

public:
	//	Note : ������ ���. ( pk ���� ),	true : �ű� ��Ͻ�, false : ���� ��� ���Ž�.
	bool ADD_PLAYHOSTILE ( const DWORD dwCHARID, const BOOL bBAD, const float fTime );
	bool DEL_PLAYHOSTILE ( const DWORD dwCHARID );
	void DEL_PLAYHOSTILE_ALL ();

public:
	//	Note : ���� ����Ʈ�� ��� �Ǿ� �ִ°�?
	bool IS_PLAYHOSTILE ( DWORD dwCHARID );
	//	Note : ����������?
	bool IS_HOSTILE_ACTOR ( DWORD dwCHARID );
	//	Note : �ڽ��� �������ΰ�?
	bool ISOFFENDER ();

	bool ISPLAYKILLING ();

public:
	// Note : �����ִ� SKILLFACT ������ �ִ°�
	bool IS_REMAIND_SKILLFACTARRAY ( EMSKILLFACT_TYPE emType );
	// Note : �ش� ��ų�� �ߺ��Ǵ°�
	bool IS_DUPLICATE_SKILLFACT (const GLSKILL* pSkill, EMSKILLFACT_TYPE emType, WORD &wOutSlot );
    // Note : �ش� ��ų�� ������ �ִ°�
	bool IS_HAVE_SKILLFACT ( const SNATIVEID skill_id, OUT WORD &wOutSlot );
    bool IS_HAVE_SKILLFACT (  const SNATIVEID skill_id, const WORD wLevel, OUT WORD &wOutSlot, const bool bOnlyEnable=true );

	const bool IS_HAVE_TYPES_IN_SKILLFACT(const EMIMPACT_ADDON emIMPACT_ADDON);
	const bool IS_HAVE_SPEC_IN_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON);
	const bool IS_HAVE_STYPES_IN_SKILLFACT(const SKILL::EMSPECIALSKILLTYPE emSSTYPE_ADDON);
	const bool IS_HAVE_SKILL_IN_SKILLFACT( const SNATIVEID& sNativeID );

public:
	enum
	{
		EGarbageCard,	// ������ ī�� ���
		EGarbageMoney,	// ���� �Ӵ� ���
		EGarbagePoint,	// �ΰ��� ĳ�� ���
		EGarbageFail,	// ������ �̿� �Ұ���
	};
	bool IsValidGarbageUse( const SNATIVEID& sDeleteItem, BYTE& nType, LONGLONG& nCost );

public:
	//	Note : ���� pk ������ �˾ƺ���. ( UINT_MAX �ϰ�� pk �ش���� ����. )
	DWORD GET_PK_LEVEL ();

	//	Note : ������ ����ġ ���� pk ���.
	float GET_PK_DECEXP_RATE ();
	std::string GET_PK_NAME ();
	DWORD GET_PK_COLOR ();
	float GET_PK_SHOP2BUY ();
	float GET_PK_SHOP2SALE ();

	DWORD GET_PK_ITEMDROP_NUM ();	

public:
	DWORD DOGRINDING(
        SITEMCUSTOM &sCusItem,
        const SITEM *pHold,
        SITEMCUSTOM& sHoldItemCustom,
        DWORD dwGrindingOption,
        BYTE& cRESULTGRADE);
	EMGRINDINGCONDITION GRINDING_CONDITION_CHECK ( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom );
	WORD GRINDING_CONSUME_NUM( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom );

	EMGRINDINGCONDITION GRINDING_TL_CONDITION_CHECK ( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom, CTime time = 0 );
	void DOTLGRINDING( SITEMCUSTOM &sCusItem, const SITEM *pHold, BYTE& cRESULTGRADE, bool bReEnchant = false);

	BOOL	DoItemSkill( const PGLSKILL pSkill, WORD wEMLinkType, std::vector<sItemSkill>& vinfo );

public:
	BOOL ISSTATEBLOW() const  { return m_bSTATEBLOW; }
	
    //! Skill fact ------------------------------------------------------------
    void 				RESETSKEFF(size_t i);
    void 				RESETAGESKEFF(size_t i);
    void 				SetSkillFact(size_t i, const SSKILLFACT& Fact);
    void 				SetSkillFact(size_t i, BOOL Enable);
    const SSKILLFACT&	GetSkillFact(size_t i) const;
    SSKILLFACT&			GetSkillFact(size_t i);
    const SNATIVEID&	GetSkillFactId(size_t i) const;
    SNATIVEID&			GetSkillFactId(size_t i);
    float				GetSkillFactAge(size_t i) const;
    void				RemoveSkillFactBySkillID(const SNATIVEID& skill_id);
	void				RemoveSkillFactBySpecialForce(const EMSPEC_ADDON emSPEC_ADDON);
	void				RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON);
	void				RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON, int iSlotNum);
	void				RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON);
	void				RemoveSkillFactofSimpact(const SNATIVEID& skill_id, const EMIMPACT_ADDON* emSPEC_ADDON);
	void				RemoveSkillFactofNormalBuf();
	void				RemoveSkillFactofNormalAll();

    void				SetFactTime(const float fVAR, const DWORD dwFLAG); // ����
    float               CheckEndureVaryInSPEC( const SKILL::EMACTION_TYPE emACTION_TYPE); //

	void 				DISABLEBLOW(int i)	  { m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; }	
	void 				RESETAGEBLOW(int i)  { m_sSTATEBLOWS[i].fAGE = 0.0f; }

	void 				DISABLEALLLANDEFF();
	void 				SETENABLEALLLUNCHBOX( const BOOL bEnable );
	void 				ADDLANDEFF( SLANDEFFECT_VALUE& landEffect );
	void 				DELLANDEFF( SLANDEFFECT_VALUE& landEffect );

	void 				ADDMAPEFF( SMAP_EFFECT& mapEffect );
	void 				DELMAPEFF( SMAP_EFFECT& mapEffect );
	void 				DISABLEALLMAPEFF();

	void 				UPDATESKILLDELAY ( float fElapsedTime );
    bool				SET_SKILLDELAY_INC ( float fINCR, float fChance, DWORD dwFLAG, const SNATIVEID& skill_id );
	void				SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage);
    void				SET_MAXHP_RATE ( const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF, float fMaxHpRate );
    void				SET_CP_DUR ( GLPADATA& sCP, float fValue, float& fTime );

	void				SETACTIVESKILL ( SNATIVEID skill_id );
	SNATIVEID			GETACTIVESKILL ()					{ return m_idACTIVESKILL; }
	EMSKILLCHECK		CHECHSKILL ( SNATIVEID skill_id, WORD wStrikeNum, EMVEHICLE_WHETHER emVehicle, DWORD dwCheckOption=0 );
	bool				CHECKPROBABILITYSKILL(const PGLSKILL pSkill, const WORD wLevel);
	BOOL				GetSkillLevel( const SNATIVEID& idSkill, SCHARSKILL& sSkill );


	virtual void		ACCOUNTSKILL ( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer=false );
	void				ACCOUNTBIKESKILL( SNATIVEID skill_id, WORD wStrikeNum, bool bServer=false );
	
public:
	virtual void ReSelectAnimation() {}

public:
        // ������ ������ , ����
        virtual void DurabilityDropHit( unsigned int nDamage, BOOL bPvp/* = FALSE  */);
        virtual void DurabilityDropUseSkill();
        virtual void DurabilityDropDeath( BOOL bPvp = FALSE );
        virtual void ItemRepairing( EMSLOT _slot );
        virtual void Durabilitiy_INIT_DATA( EMSLOT slot );   // ������ �϶��ϸ� ��� ������ �����Ѵ�.
protected:
	virtual void SummonStateVisibleUpdate(void){}
private:
        float m_DurabilitySlotItemPerformance[SLOT_TSIZE];

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SCHARDATA2>(*this);

		ar & m_dwGlobalID;
		ar & m_CHARINDEX;
	}
};

//	Note : ���� �߰��ÿ�. RESET_DATA() �Լ��� ���� �ʱ�ȭ �־�ߵ�.
//
struct GLCROWLOGIC
{
	enum EMCONTROL
	{
		EMTARGET		= 0x00000001,
		EMATTACKABLE	= 0x00000002,
		EMTRACING		= 0x00000004,
		EMSTAY			= 0x00000008,
		EMJUMPTO		= 0x00000010,

		EMPREORDER		= 0x00000100,
	};

	DWORD				m_dwGlobalID;				//	���� �޸� �ε����� ( �ش� GLLandMan������ ����ũ�� )
	SNATIVEID			m_sNativeID;				//	Crow NativeID.
	SCROWDATA*		m_pCrowData;				//	Crow �⺻ ����.

	

	SSKILLFACT	m_sSKILLFACT[CROW_SKILLFACT_SIZE];	//	��ųȿ����.
	DWORD			m_dwHOLDBLOW;					//	�����̻��� ������.

	SSTATEBLOW	m_sSTATEBLOWS[EMBLOW_MULTI];	//	�����̻�.

	float				m_fACTIONDELAY[SCROWDATA::EMMAXATTACK];	//	���� cool time;
	float				m_fSTATE_MOVE_RATE;						//	���� �̻� �̵��ӵ�. ( 1.0f == 100% )
	float				m_fSTATE_DELAY;							//	���� �̻� ������. ( 1.0f == 100% ) (+)
	float				m_fSTATE_DAMAGE;						//	���� �̻� ����� ����.

	float				m_fDamageRate;							//	���ݷ� ��ȭ��.
	float				m_fDefenseRate;							//	���� ��ȭ��.

    // ����: ��ȭ�� ����Ŀ� ���� �Ҽ��������� ������ �ӽ÷� �����ϱ� ���� �����Դϴ�.
	float				m_fTempVarHP;
	float				m_fTempVarMP;
	float				m_fTempVarSP;

	float               m_fMultipleHP;	                // HP ���

	float				m_fATTVELO;						//	���ݼӵ�.
	float				m_fSKILL_MOVE;					//	��ų �̵��ӵ�.
	
	bool				m_bSkillFactsSpecial[EMSPECA_NSIZE];
	bool				m_bSTATE_PANT;                  //	�����̻� �涱��.
	bool				m_bSTATE_STUN;                  //	�����̻� ����.	
	SINVISIBLE				m_sINVISIBLE;				// ����.
	SRECVISIBLE			m_sRECVISIBLE;			// ���Ű���.
	SHALLUCINATE			m_sHALLUCINATE;		// ȯ�� ȿ��.
	SRELEASE_ENDURE	m_sRELEASE_ENDURE;	// �ǰݽ� ȿ�� ����.
	SDOMINATE				m_sDOMINATION;			// ���� ȿ��.
	SSTIGMA				m_sSTIGMA_USER;		// ���� ȿ�� - ���.
	SSTIGMA				m_sSTIGMA_TAKER;		// ���� ȿ�� - ����.
	SLINKHP					m_sLINKHP;					// ���� ȿ��.
	SINCREASEEFF			m_sIncreaseEff;			// ��ȭ ȿ��;
	SIGNORED_DAMAGE	m_sIgnoredDamage;		// ������ ����;
	SVIEWRANGERATE	m_sRateViewRange;		// �þ� ����;
	SAIRBORNE				m_sAirborne;				// ���;
	SCLONE				m_sClone;// ����ȿ��

	CSkillCAIdx			m_SkillCAIndex;			// �ݰݸ���Ʈ
	
	float				m_fHPRestorationRate;			// HP ȸ����;
	float				m_fMPRestorationRate;			// MP ȸ����;
	float				m_fSPRestorationRate;			// SP ȸ����;

	float				m_fHPRestorationRatePlus;		// HP ȸ���� ��ȭ��;
	float				m_fMPRestorationRatePlus;		// MP ȸ���� ��ȭ��;
	float				m_fSPRestorationRatePlus;		// SP ȸ���� ��ȭ��;

	int					m_nSUM_HIT;						//	������ + ��ųȿ��.
	int					m_nSUM_AVOID;					//	ȸ���� + ��ųȿ��.

	int					m_nSUM_DEFENSE;				//	���� ( DP + POW*��� + ITEM_DEF ) * �������� ���� + ��ų.
	int					m_nDX_DAMAGE;					//	�⺻ ���ݷ�. + ��ųȿ��.

	int					m_nSUM_PIERCE;					//	�߰� ���� ����.
	
	SRESIST				m_sSUMRESIST;				//	���װ�.	
	
	DAMAGE_SPEC			m_sDamageSpec;					//	������ ���� �� �ݻ�
	DEFENSE_SKILL		m_sDefenseSkill;				//  �ߵ��� ��ų

	SNATIVEID			m_idIllusionSKILL;

	SDURATION_DAMAGE	m_sDurDamage[EMELEMENT_MAXNUM];	//	���� ������ ����

	DWORD				m_dwActionLimit;				//	�ൿ �׼� ���� ����

	DWORD				m_dwAType;						//	���� �Ӽ� ����.
	
	DWORD				m_dwNowHP;						//	������.
	WORD				m_wNowMP;						//	���ŷ�.
	WORD				m_wNowSP;						//	���ŷ�.

	//
	//mjeon.CaptureTheField
	//
	DWORD				m_arrHP4School[SCHOOL_NUM];		//  CTF���� �� �п����� �����Ǵ� �������� HP
	float					m_arrIncHp4School[SCHOOL_NUM];  //  CTF���� �� �п����� �����Ǵ� �������� ȸ����    
	WORD				m_wSchoolAuthed;				//  CTF���� ���� �� �����⸦ ������ �п� ��ȣ

	SNATIVEID			m_idACTIVESKILL;				//	Ȱ�� ��ų.
	WORD				m_wACTIVESKILL_LVL;				//	Ȱ�� ��ų  ����.

    float               m_fMoveVeloLimit_Min;   // ���� �̵��ӵ� ����;
    float               m_fMoveVeloLimit_Max;   // �ְ� �̵��ӵ� ����;

	float				m_fInfluenceY;

	CSkillEffCrowSet	m_EffSkillVarSet;

	//	Note : ���� �߰��ÿ�. RESET_DATA() �Լ��� ���� �ʱ�ȭ �־�ߵ�.
	//

	GLCROWLOGIC(void);
    virtual ~GLCROWLOGIC() {}

	void RESET_DATA();

	bool VALIDACTION();
	bool VALIDSKILL();
	bool VALIDATTACK();
	EMCROW GETCROW() const;
	WORD GETLEVEL() const;
	const char* GETNAME();
    const std::string GetNameStr() const;
	
	DWORD GETHP() const;
	DWORD GETMAXHP() const;
	float GETMAXHPRATE() const;
    void  SETMAXHPRATE( float fRate );
    void  SetMultipleHP( float fMultiple );
    float GetMultipleHP() const;

	int GETMP() const;
	int GETMAXMP() const;

	int GETSP() const;
	int GETMAXSP() const;

	int GETHIT() const;
	int GETAVOID() const;

	float GETHPRESTORATIONRATE() const;
	float GETMPRESTORATIONRATE() const;
	float GETSPRESTORATIONRATE() const;

	float GetHPRestorationRatePlus () const { return m_fHPRestorationRatePlus; }
	float GetMPRestorationRatePlus () const { return m_fMPRestorationRatePlus; }
	float GetSPRestorationRatePlus () const { return m_fSPRestorationRatePlus; }

	void SetHPRestorationRatePlus( float fRate );
	void SetMPRestorationRatePlus( float fRate );
	void SetSPRestorationRatePlus( float fRate );

	EMBRIGHT GETBRIGHT() const;

	int GETFORCE_LOW () const;
	int GETFORCE_HIGH () const;
	int GETDEFENSE () const;

	int GETSUM_PIERCE () const;

	CSkillCAIdx* GetSkillCA() { return &m_SkillCAIndex; }

	void		 GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA );

	const int GETVISIBLESIGHT(void) const { return int(m_sRateViewRange * MAX_VIEWRANGE); }
	const float GETVISIBLESIGHT_RATE(void) const { return m_sRateViewRange.fRate; }

	WORD GETBODYRADIUS () const;
	WORD GETATTACKRANGE () const;

	WORD GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const;
	WORD GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const;

	BOOL INIT_DATA ();	
	DWORD RECEIVE_DAMAGE(const int nDamage);
		virtual void RECEIVE_DAMAGE_AFTER(void){;}
		void RELEASE_ENDURE_PROC(void);
	DWORD RECEIVE_HEAL(const WORD wHeal);
	DWORD RECEIVE_DAMAGE_BY_SCHOOL(const int nDamage, const WORD wSchool);
	DWORD RECEIVE_HEAL_BY_SCHOOL(const int nHeal, const WORD wSchool){return 0;}

	void RESETSKEFF(size_t i );
	void DISABLEBLOW ( int i );

	void PrevUPDATE_DATA(const float fElapsedTime);
	void UPDATE_DATA(float fTime, float fElapsedTime, BOOL bClient=FALSE);		
	virtual void	UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE ){;} // ����ȿ�� ���� �� ȣ��; // ���� �Լ� �ȵ�; RanLogicClient/ReferChar���� GLogic ���;
	virtual void	UPDATE_DATA_Additional(const float fElapsedTime){;} // Update_data ���� �� ȣ��;

	void Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID );

	void SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage);

	void SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel );
	SNATIVEID GETACTIVESKILL ();
	EMSKILLCHECK CHECHSKILL ( DWORD dwAType, bool bNotLearn = false );
	void ACCOUNTSKILL ( WORD wStrikeNum );

	virtual float GETATTVELO();
	virtual float GETMOVEVELO();
    virtual float GETATT_TOTAL(); // GETATTVELO() + GETATT_ITEM()
    virtual float GETMOVE_TOTAL(); // GETMOVEVELO() + GETMOVE_ITEM() 

	bool IsDefenseSkill();
	void SetDefenseSkill(bool bDefense);

	inline bool IsCheckedSkillFlagSpecial(const DWORD dwFlag) const { return m_bSkillFactsSpecial[dwFlag]; }	
	inline bool IsEntranceExitSkillApply() { return RF_ENTRANCE_EXIT(m_EffSkillVarSet).IsOn(); }

    void RemoveSkillFactBySkillID(const SNATIVEID& skill_id);
	void RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON);
	void RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON);

	void RemoveSkillFactofSimpact(const SNATIVEID& skill_id, const EMIMPACT_ADDON* emSPEC_ADDON);

    const bool IS_HAVE_SKILLFACT(const SNATIVEID& skill_id, OUT WORD& wOutSlot);
	const bool IS_HAVE_TYPES_IN_SKILLFACT(const EMIMPACT_ADDON emIMPACT_ADDON);
	const bool IS_HAVE_SPEC_IN_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON);
	const bool IS_HAVE_STYPES_IN_SKILLFACT(const SKILL::EMSPECIALSKILLTYPE emSSTYPE_ADDON);
	const bool IS_HAVE_SKILL_IN_SKILLFACT( const SNATIVEID& sNativeID );

    //! NPC���� �ൿ ������ �Ÿ�����
    bool ISNPC_INTERACTIONABLE(
        const D3DXVECTOR3& vReqPos,
        const D3DXVECTOR3& vCurPos,
        const float fReqRadius,
        int nNpcAction) const;

    const float GET_INTERACTION_RANGE() const { return (m_pCrowData->m_fRefenceActionableRange > 0.0f) ? m_pCrowData->m_fRefenceActionableRange : GLCONST_CHAR::fNPC_INTERACTION_RANGE; }

    //! Crow NativeID
    const SNATIVEID& GetNativeId(void) const { return m_sNativeID; }

	inline bool IsBoss()    { if(!m_pCrowData) return false; return m_pCrowData->IsBoss(); }
	inline bool IsPosHold() { if(!m_pCrowData) return false; return m_pCrowData->IsPosHold(); }
};

EMANI_SUBTYPE CHECK_ANISUB (const SITEM* pRHAND, const SITEM* pLHAND, const EMCHARINDEX _emClass );
EMANI_SUBTYPE CHECK_ATTACK_ANISUB (const SITEM* pRHAND, const SITEM* pLHAND, const EMCHARINDEX _emClass );

//! ������ ȹ�� ������ �ֱ�����...
//! ������ �α׸� ����ϱ� ���� ����ü
struct SDAMAGELOG
{
	DWORD dwUserID; ///< User ID
	DWORD dwGaeaID; ///< ���̾� ID
	DWORD dwDamage; ///< �� ������	

	SDAMAGELOG () 
		: dwUserID(0)
		, dwGaeaID(0)
		, dwDamage(0)
	{
	}

	SDAMAGELOG ( DWORD dwuserid, DWORD dwgaeaid, DWORD dwdamage ) 
		: dwUserID(dwuserid)
		, dwGaeaID(dwgaeaid)
		, dwDamage(dwdamage)
	{
	}
};

typedef std::map<DWORD, SDAMAGELOG> DAMAGELOG;
typedef DAMAGELOG::iterator	        DAMAGELOG_ITER;

inline void AddDamageLog ( DAMAGELOG &cDamageLog, DWORD dwGaeaID, DWORD dwUserID, DWORD dwDamage )
{
	DAMAGELOG_ITER iter = cDamageLog.find ( dwUserID );
	DAMAGELOG_ITER iter_end = cDamageLog.end();
	
	if ( iter != iter_end )
	{
		SDAMAGELOG &sDamageLog = (*iter).second;
		if ( sDamageLog.dwUserID == dwUserID )	dwDamage += sDamageLog.dwDamage;
	}
	
	cDamageLog[dwUserID] = SDAMAGELOG(dwUserID,dwGaeaID,dwDamage);
}

//! �߰�:2006-04-12 Jgkim
//! ��Ƽ ������ �αױ���� ���� ����ü
struct SDAMAGELOGPARTY
{
	GLPartyID m_PartyID; ///< ��Ƽ ������ȣ
	DWORD m_dwDamage; ///< ��Ƽ�� �� ������

	SDAMAGELOGPARTY()
		: m_dwDamage(0)
	{
	}

	SDAMAGELOGPARTY(const GLPartyID& PartyID, DWORD dwDamage)
		: m_PartyID(PartyID)
		, m_dwDamage(dwDamage)
	{
	}
};
typedef std::map<GLPartyID, SDAMAGELOGPARTY> DAMAGELOGPARTY;
typedef DAMAGELOGPARTY::iterator		 DAMAGELOGPARTY_ITER;
inline void AddDamageLogParty(DAMAGELOGPARTY &cDamageLogParty, const GLPartyID& PartyID, DWORD dwDamage)
{
	if ( PartyID.isValidParty() == false )
		return;

	DAMAGELOGPARTY_ITER iter = cDamageLogParty.find(PartyID);
	if ( iter == cDamageLogParty.end() )
	{
		cDamageLogParty[PartyID] = SDAMAGELOGPARTY(PartyID, dwDamage);
		return;
	}

	SDAMAGELOGPARTY& sDamageLogParty = iter->second;
	if ( sDamageLogParty.m_PartyID != PartyID )
		return;

	sDamageLogParty.m_dwDamage += dwDamage;
}

inline bool CHECHSKILL_ITEM ( SKILL::GLSKILL_ATT emSKILL, GLITEM_ATT emITEM, const bool bHiddenWeapon )
{
	// �ذ��� �۷��� ���� �� ��ų ��� ���� ����;
	if ( emITEM == ITEMATT_EXTREME_GLOVE && bHiddenWeapon )
	{
		return true;
	}

	switch ( emSKILL ) 
	{
	case SKILL::SKILLATT_NOTHING:
		return ( emITEM==ITEMATT_NOTHING );
	case SKILL::SKILLATT_SWORD:
		return ( emITEM==ITEMATT_SWORD || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_SABER:
		return ( emITEM==ITEMATT_SABER || emITEM==ITEMATT_WEAPON_STR );	
	case SKILL::SKILLATT_SWORDSABER:
		return ( emITEM==ITEMATT_SWORD || emITEM==ITEMATT_SABER || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_DAGGER:
		return ( emITEM==ITEMATT_DAGGER );
	case SKILL::SKILLATT_SPEAR:
		return ( emITEM==ITEMATT_SPEAR || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_STICK:
		return ( emITEM==ITEMATT_STICK );
	case SKILL::SKILLATT_GWON:
		return ( emITEM==ITEMATT_GWON || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_BOW:
		return ( emITEM==ITEMATT_BOW || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_THROW:
		return ( emITEM==ITEMATT_THROW );	
	case SKILL::SKILLATT_PISTOL:
		return ( emITEM==ITEMATT_PISTOL || emITEM==ITEMATT_WEAPON_DEX );
	case SKILL::SKILLATT_RAILGUN:
		return ( emITEM==ITEMATT_RAILGUN || emITEM==ITEMATT_WEAPON_STR );
	case SKILL::SKILLATT_PORTALGUN:
		return ( emITEM==ITEMATT_PORTALGUN || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_GUN:
		return ( emITEM==ITEMATT_PISTOL || emITEM==ITEMATT_RAILGUN || emITEM==ITEMATT_PORTALGUN || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_RIFLE:
		return ( emITEM==ITEMATT_RAILGUN || emITEM==ITEMATT_PORTALGUN || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_SPI );
    case SKILL::SKILLATT_SIDE:
        return ( emITEM==ITEMATT_SIDE || emITEM==ITEMATT_WEAPON_STR );
    case SKILL::SKILLATT_DUALSPEAR:
        return ( emITEM==ITEMATT_DUALSPEAR || emITEM==ITEMATT_WEAPON_DEX );
    case SKILL::SKILLATT_THROWING_KNIFE:
        return ( emITEM==ITEMATT_THROWING_KNIFE || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_EXTREME_GLOVE:
		return ( emITEM==ITEMATT_EXTREME_GLOVE || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_TRICK_STICK:
		return ( emITEM==ITEMATT_TRICK_STICK || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_TRICK_BOX:
		return ( emITEM==ITEMATT_TRICK_BOX || emITEM==ITEMATT_WEAPON_DEX );
	case SKILL::SKILLATT_TRICK_WHIP:
		return ( emITEM==ITEMATT_TRICK_WHIP || emITEM==ITEMATT_WEAPON_STR );
	case SKILL::SKILLATT_ACTOR_SHIELD:
		return ( emITEM==ITEMATT_ACTOR_SHIELD || emITEM==ITEMATT_WEAPON_DEX );
	case SKILL::SKILLATT_ACTOR_HAMMER:
		return ( emITEM==ITEMATT_ACTOR_HAMMER || emITEM==ITEMATT_WEAPON_STR );
	case SKILL::SKILLATT_ACTOR_UMBRELLA:
		return ( emITEM==ITEMATT_ACTOR_UMBRELLA || emITEM==ITEMATT_WEAPON_SPI );
	};

	return false;
}

inline SKILL::GLSKILL_ATT ItemAtt2SkillAtt( GLITEM_ATT emItem, const DWORD _dwClass )
{
	switch( emItem ) 
	{
	case ITEMATT_NOTHING :
		return SKILL::SKILLATT_NOTHING;

	case ITEMATT_SWORD :
		return SKILL::SKILLATT_SWORD;

	case ITEMATT_SABER :
		return SKILL::SKILLATT_SABER;

	case ITEMATT_DAGGER :
		return SKILL::SKILLATT_DAGGER;

	case ITEMATT_SPEAR :
		return SKILL::SKILLATT_SPEAR;

	case ITEMATT_STICK :
		return SKILL::SKILLATT_STICK;

	case ITEMATT_GWON :
		return SKILL::SKILLATT_GWON;

	case ITEMATT_BOW :
		return SKILL::SKILLATT_BOW;

	case ITEMATT_THROW :
		return SKILL::SKILLATT_THROW;

	case ITEMATT_PISTOL :
		return SKILL::SKILLATT_PISTOL;

	case ITEMATT_RAILGUN :
		return SKILL::SKILLATT_RAILGUN;

	case ITEMATT_PORTALGUN :
		return SKILL::SKILLATT_PORTALGUN;

	case ITEMATT_SIDE :
		return SKILL::SKILLATT_SIDE;

	case ITEMATT_DUALSPEAR :
		return SKILL::SKILLATT_DUALSPEAR;

	case ITEMATT_THROWING_KNIFE :
		return SKILL::SKILLATT_THROWING_KNIFE;

	case ITEMATT_EXTREME_GLOVE :
		return SKILL::SKILLATT_EXTREME_GLOVE;

	case ITEMATT_TRICK_STICK :
		return SKILL::SKILLATT_TRICK_STICK;

	case ITEMATT_TRICK_BOX :
		return SKILL::SKILLATT_TRICK_BOX;

	case ITEMATT_TRICK_WHIP :
		return SKILL::SKILLATT_TRICK_WHIP;

	case ITEMATT_ACTOR_SHIELD :
		return SKILL::SKILLATT_ACTOR_SHIELD;

	case ITEMATT_ACTOR_HAMMER :
		return SKILL::SKILLATT_ACTOR_HAMMER;

	case ITEMATT_ACTOR_UMBRELLA :
		return SKILL::SKILLATT_ACTOR_UMBRELLA;

	case ITEMATT_WEAPON_STR:
		{
			if ( _dwClass & GLCC_FIGHTER )
				return SKILL::SKILLATT_GWON;
			else if ( _dwClass & GLCC_ARMS )
				return SKILL::SKILLATT_SABER;
			else if ( _dwClass & GLCC_ARCHER )
				return SKILL::SKILLATT_BOW;
			else if ( _dwClass & GLCC_SPIRIT )
				return SKILL::SKILLATT_SPEAR;
			else if ( _dwClass & GLCC_EXTREME )
				return SKILL::SKILLATT_EXTREME_GLOVE;
			else if ( _dwClass & GLCC_SCIENTIST )
				return SKILL::SKILLATT_RAILGUN;
			else if ( _dwClass & GLCC_ASSASSIN )
				return SKILL::SKILLATT_SIDE;
			else if ( _dwClass & GLCC_TRICKER )
				return SKILL::SKILLATT_TRICK_WHIP;
			else if ( _dwClass & GLCC_ETC )
				return SKILL::SKILLATT_NOTHING;
			else if ( _dwClass & GLCC_ACTOR )
				return SKILL::SKILLATT_ACTOR_HAMMER;
		}
		return SKILL::SKILLATT_NOTHING;

	case ITEMATT_WEAPON_DEX:
		{
			if ( _dwClass & GLCC_FIGHTER )
				return SKILL::SKILLATT_GWON;
			else if ( _dwClass & GLCC_ARMS )
				return SKILL::SKILLATT_SWORD;
			else if ( _dwClass & GLCC_ARCHER )
				return SKILL::SKILLATT_BOW;
			else if ( _dwClass & GLCC_SPIRIT )
				return SKILL::SKILLATT_SPEAR;
			else if ( _dwClass & GLCC_EXTREME )
				return SKILL::SKILLATT_EXTREME_GLOVE;
			else if ( _dwClass & GLCC_SCIENTIST )
				return SKILL::SKILLATT_PISTOL;
			else if ( _dwClass & GLCC_ASSASSIN )
				return SKILL::SKILLATT_DUALSPEAR;
			else if ( _dwClass & GLCC_TRICKER )
				return SKILL::SKILLATT_TRICK_BOX;
			else if ( _dwClass & GLCC_ETC )
				return SKILL::SKILLATT_NOTHING;
			else if ( _dwClass & GLCC_ACTOR )
				return SKILL::SKILLATT_ACTOR_SHIELD;
		}
		return SKILL::SKILLATT_NOTHING;

	case ITEMATT_WEAPON_SPI:
		{
			if ( _dwClass & GLCC_FIGHTER )
				return SKILL::SKILLATT_GWON;
			else if ( _dwClass & GLCC_ARMS )
				return SKILL::SKILLATT_SWORD;
			else if ( _dwClass & GLCC_ARCHER )
				return SKILL::SKILLATT_BOW;
			else if ( _dwClass & GLCC_SPIRIT )
				return SKILL::SKILLATT_SPEAR;
			else if ( _dwClass & GLCC_EXTREME )
				return SKILL::SKILLATT_EXTREME_GLOVE;
			else if ( _dwClass & GLCC_SCIENTIST )
				return SKILL::SKILLATT_PORTALGUN;
			else if ( _dwClass & GLCC_ASSASSIN )
				return SKILL::SKILLATT_THROWING_KNIFE;
			else if ( _dwClass & GLCC_TRICKER )
				return SKILL::SKILLATT_TRICK_STICK;
			else if ( _dwClass & GLCC_ETC )
				return SKILL::SKILLATT_NOTHING;
			else if ( _dwClass & GLCC_ACTOR )
				return SKILL::SKILLATT_ACTOR_UMBRELLA;
		}
		return SKILL::SKILLATT_NOTHING;

	default :
		return SKILL::SKILLATT_NOTHING;
	}
}

inline void GetRandomPos( D3DXVECTOR3* pDestPos, D3DXVECTOR3* pOrgPos, float fDist )
{
	D3DXVECTOR3			vDir;
	vDir.x = (((((float) rand())/RAND_MAX)*2.f)-1.f);
	vDir.y = (((((float) rand())/RAND_MAX)*2.f)-1.f);
	vDir.z = (((((float) rand())/RAND_MAX)*2.f)-1.f);

	D3DXVec3Normalize( &vDir, &vDir );

	pDestPos->x = pOrgPos->x + ( vDir.x * fDist );
	pDestPos->y = pOrgPos->y + ( vDir.y * fDist );
	pDestPos->z = pOrgPos->z + ( vDir.z * fDist );
} //GetRandomPos

#endif // GLOGIXEX_H_