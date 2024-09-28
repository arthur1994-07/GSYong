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

//! ������� 2012-01-26 ���ر�, ���ر�
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

    //! �� Damage ����� �ݿ�
    int CalcDefenseRate(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level);

    //! ���� ���� Damage ����
    virtual int CalcLowDamage(EMCROW CrowType, int DamageOld, int Defense, float StateDamage);

    //! Damage �ִ� �ּҰ� ������ ���� Damage ����
    virtual int CalcRandomDamage(const int& nDamageLow, const int& nDamageHigh, int ExtForce);

    //! ����Ÿ�� �߻� ���� �Ǵ�
    virtual bool CalcCrushingBlow(int nCrushingBlow);

    //! Critical �߻� ���� �Ǵ�
    virtual bool CalcCritical(EMCROW CrowType, DWORD NowHp, DWORD MaxHp, int ndxLvl, int nIncCritical);

	/* Ư�� ��Ȳ������ ������ ����;
	 * @bCritical, ũ��Ƽ�� �߻� ����;
	 * @bCrushingBlow, ����Ÿ�� �߻� ����;
	 * @Damage, ���� ������;
	 * @fAddCriticalDamageIncreseR, ũ��Ƽ�� �߻��� �߰� ������;
	*/
    virtual int CalcCriticalDamage(bool bCritical, bool bCrushingBlow, int Damage, float fAddCriticalDamageIncreseR = 0.0f, float fAddCrushingDamageIncreseR = 0.0f );

    virtual void AdjustLevel(int& ndxLvl, int& nExtFORCE, int TarLevel, WORD MyLevel);

    //! ���ݷ� ��ȭ�� �ݿ�
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

//! 2006-03-28 Jgkim ��ȹ�� ��û���� ������ ���� ������
class CalcDamage_20060328 : public ServerCalcDamage
{
public:
    CalcDamage_20060328(bool UseNewDefenseRate);
    virtual ~CalcDamage_20060328();

    //! 2006-03-28 Jgkim ��ȹ�� ��û���� ������ ���� ������
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