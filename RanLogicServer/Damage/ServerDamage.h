#ifndef _GLOGIC_SERVER_DAMAGE_H_
#define _GLOGIC_SERVER_DAMAGE_H_

#pragma once

class GLChar;

class ICalcDefenseRate
{
public:
    ICalcDefenseRate() {}
    virtual ~ICalcDefenseRate() {}
    virtual int Calc(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level) = 0;
};

class CalcDefenseRateOld : public ICalcDefenseRate
{
public:
    CalcDefenseRateOld() {}
    virtual ~CalcDefenseRateOld() {}
    virtual int Calc(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level) override;
};

//! 변경공식 2012-01-26 이준규, 김준권
class CalcDefenseRate_20120126 : public ICalcDefenseRate
{
public:
    CalcDefenseRate_20120126() {}
    virtual ~CalcDefenseRate_20120126() {}
    virtual int Calc(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level) override;
};

class ServerCalcDamage
{
public:
    // GLUseFeatures::GetInstance().IsUsingNewDefenseRate()
    ServerCalcDamage(bool UseNewDefenseRate);
    virtual ~ServerCalcDamage();

protected:
    ICalcDefenseRate* m_pCalcDefenseRate;

public:
    virtual DWORD CalcDamage(
        GLChar* pChar,
        int& rResultDAMAGE,
        const DWORD dwGaeaID,
        const STARGETID &cTargetID,
        const GLLandMan* pLandMan,
        const GLSKILL* pSkill,
        const DWORD dwskill_lev,
        const DWORD dwWeatherFlag,
        const DWORD dwDivCount) = 0;

    //! 방어구 Damage 흡수율 반영
    int CalcDefenseRate(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level);

    //! 최저 수용 Damage 산출
    virtual int CalcLowDamage(EMCROW CrowType, int DamageOld, int Defense, float StateDamage);

    //! Damage 최대 최소값 사이의 랜덤 Damage 결정
    virtual int CalcRandomDamage(const int& nDamageLow, const int& nDamageHigh, int ExtForce);

    //! 강한타격 발생 여부 판단
    virtual bool CalcCrushingBlow(int nCrushingBlow);

    //! Critical 발생 여부 판단
    virtual bool CalcCritical(EMCROW CrowType, DWORD NowHp, DWORD MaxHp, int ndxLvl, int nIncCritical);

	/* 특정 상황에따른 데미지 증감;
	 * @bCritical, 크리티컬 발생 여부;
	 * @bCrushingBlow, 강한타격 발생 여부;
	 * @Damage, 순수 데미지;
	 * @fAddCriticalDamageIncreseR, 크리티컬 발생시 추가 데미지;
	*/
    virtual int CalcCriticalDamage(bool bCritical, bool bCrushingBlow, int Damage, float fAddCriticalDamageIncreseR = 0.0f, float fAddCrushingDamageIncreseR = 0.0f );

    virtual void AdjustLevel(int& ndxLvl, int& nExtFORCE, int TarLevel, WORD MyLevel);

    //! 공격력 변화율 반영
    virtual void AdjuctDamage(EMCROW CrowType, int& nDamageLow, int& nDamageHigh, float DamageRate, float OwnerDamagePer);
};

class CalcDamage_2004 : public ServerCalcDamage
{
public:
    CalcDamage_2004(bool UseNewDefenseRate);
    virtual ~CalcDamage_2004();

    virtual DWORD CalcDamage(
        GLChar* pChar,
        int& rResultDAMAGE,
        const DWORD dwGaeaID,
        const STARGETID &cTargetID,
        const GLLandMan* pLandMan,
        const GLSKILL* pSkill,
        const DWORD dwskill_lev,
        const DWORD dwWeatherFlag,
        const DWORD dwDivCount) override;
};

//! 2006-03-28 Jgkim 기획팀 요청으로 데미지 공식 변경함
class CalcDamage_20060328 : public ServerCalcDamage
{
public:
    CalcDamage_20060328(bool UseNewDefenseRate);
    virtual ~CalcDamage_20060328();

    //! 2006-03-28 Jgkim 기획팀 요청으로 데미지 공식 변경함
    virtual DWORD CalcDamage(
        GLChar* pChar,
        int& rResultDAMAGE,
        const DWORD dwGaeaID,
        const STARGETID &cTargetID,
        const GLLandMan* pLandMan,
        const GLSKILL* pSkill,
        const DWORD dwskill_lev,
        const DWORD dwWeatherFlag,
        const DWORD dwDivCount) override;
};

#endif // _GLOGIC_SERVER_DAMAGE_H_