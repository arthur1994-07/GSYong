

#pragma	once

#include <map>
#include <vector>

#include "../Util/UIGroupHelper.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class CBasicTextBox;
class RnButton;

struct GLCHARLOGIC;

class CBasicVarTextBox;

class RnStatInfoTap : public CUIGroupHelper
{
	enum 
	{
		QUESTION_BUTTON = 1,
		POINT_VALUE,

		STAT_POW_BASE_OVER_RECT,
		STAT_DEX_BASE_OVER_RECT,
		STAT_SPI_BASE_OVER_RECT,
		STAT_STR_BASE_OVER_RECT,
		STAT_STA_BASE_OVER_RECT,

		STAT_ATTACK_MIN_OVER_RECT,
		STAT_ATTACK_MAX_OVER_RECT,
		STAT_DEFENSE_OVER_RECT,
		STAT_FIGHT_OVER_RECT,
		STAT_SHOOT_OVER_RECT,
		STAT_STAMINA_OVER_RECT,
		STAT_HIT_OVER_RECT,
		STAT_AVOID_OVER_RECT,
		STAT_HP_OVER_RECT,
		STAT_MP_OVER_RECT,
		STAT_SP_OVER_RECT,

		STAT_FIRE_RESIST_RECT,
		STAT_COLD_RESIST_RECT,
		STAT_ELECTRIC_RESIST_RECT,
		STAT_POISON_RESIST_RECT,
		STAT_SPIRIT_RESIST_RECT,


		STAT_POW_BASE_VALUE,
		STAT_DEX_BASE_VALUE,
		STAT_SPI_BASE_VALUE,
		STAT_STR_BASE_VALUE,
		STAT_STA_BASE_VALUE,

		STAT_POW_UP_VALUE,
		STAT_DEX_UP_VALUE,
		STAT_SPI_UP_VALUE,
		STAT_STR_UP_VALUE,
		STAT_STA_UP_VALUE,

		STAT_POW_PLUS_BUTTON,
		STAT_POW_MINUS_BUTTON,
		STAT_DEX_PLUS_BUTTON,
		STAT_DEX_MINUS_BUTTON,
		STAT_SPI_PLUS_BUTTON,
		STAT_SPI_MINUS_BUTTON,
		STAT_STR_PLUS_BUTTON,
		STAT_STR_MINUS_BUTTON,
		STAT_STA_PLUS_BUTTON,
		STAT_STA_MINUS_BUTTON,

		STAT_INIT_BUTTON,
		STAT_APPLY_BUTTON,
		STAT_WND_COUNT,
	};
public:
	RnStatInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	void CreateSubControl();


	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );



private:
//	void ProcessVirtualStateButton();

	void InitVirtualPoint();
	int  GetRestVirtualPoint();
	void PlusVirtualPoint(int& virtualPoint);
	void MinusVirtualPoint(int& virtualPoint);

	int  GetVirtualPow();
	int  GetVirtualDex();
	int  GetVirtualSpi();
	int  GetVirtualStr();
	int  GetVirtualSta();

	bool IsUsedVirtualPoint();

	void ApplyInvenSlotItem( GLCHARLOGIC* pChar );

	WORD GetPoint();
	void SetPoint( const GLCHARLOGIC& sCharData );
	//	Stat	
	void SetPow( const GLCHARLOGIC& sCharData );
	void SetDex( const GLCHARLOGIC& sCharData );
	void SetSpi( const GLCHARLOGIC& sCharData );
	void SetStr( const GLCHARLOGIC& sCharData );
	void SetSta ( const GLCHARLOGIC& sCharData );

	//////////////////////////////////////////////////////////////////////////]
	void SetAttackMin(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData);
	void SetAttackMax(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData);
	void SetDefense(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData);
	void SetFight(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetShoot(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetStamina(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetHit(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetAvoid(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetHP(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetMP(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );
	void SetSP(const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData );

	//////////////////////////////////////////////////////////////////////////
	void SetFireResist(const GLCHARLOGIC& sCharData);
	void SetColdResist(const GLCHARLOGIC& sCharData);
	void SetElectricResist(const GLCHARLOGIC& sCharData);
	void SetPoisonResist(const GLCHARLOGIC& sCharData);
	void SetSpiritResist(const GLCHARLOGIC& sCharData);

	//////////////////////////////////////////////////////////////////////////

	void SetStatHelper(CBasicTextBox* pValueText,CBasicTextBox* pUpValueText,int originValue,int changeUp);
	void ChangeUpHelper(CBasicTextBox* pValueText,CBasicTextBox* pMarkText,CBasicTextBox* pChangeUpText,
		int originValue,int changeUp,CBasicTextBox* pMarkTextImage = NULL);
private:
	CBasicVarTextBox* m_pToolTip;

	typedef std::map<UIGUID,CUIControl*> OverRectMap;
	OverRectMap    m_OverRectMap;


	CBasicTextBox* m_pStatPointValue;

	CBasicTextBox* m_pStatPowValue;
	CBasicTextBox* m_pStatPowUpValue;

	CBasicTextBox* m_pStatDexValue;
	CBasicTextBox* m_pStatDexUpValue;

	CBasicTextBox* m_pStatSpiValue;
	CBasicTextBox* m_pStatSpiUpValue;

	CBasicTextBox* m_pStatStrValue;
	CBasicTextBox* m_pStatStrUpValue;

	CBasicTextBox* m_pStatStaValue;
	CBasicTextBox* m_pStatStaUpValue;

	//////////////////////////////////////////////////////////////////////////
	CBasicTextBox* m_pStatAttackMinValue;
	CBasicTextBox* m_pStatAttackMinMark;
	CBasicTextBox* m_pStatAttackMinMarkImage;
	CBasicTextBox* m_pStatAttackMinUpValue;

	CBasicTextBox* m_pStatAttackMaxValue;
	CBasicTextBox*  m_pStatAttackMaxMark;
	CBasicTextBox*  m_pStatAttackMaxMarkImage;
	CBasicTextBox* m_pStatAttackMaxUpValue;

	CBasicTextBox* m_pStatDefenseValue;
	CBasicTextBox* m_pStatDefenseMark;
	CBasicTextBox* m_pStatDefenseMarkImage;
	CBasicTextBox* m_pStatDefenseUpValue;

	CBasicTextBox* m_pStatFightValue;
	CBasicTextBox* m_pStatFightMark;
	CBasicTextBox* m_pStatFightMarkImage;
	CBasicTextBox* m_pStatFightUpValue;

	CBasicTextBox* m_pStatShootValue;
	CBasicTextBox* m_pStatShootMark;
	CBasicTextBox* m_pStatShootMarkImage;
	CBasicTextBox* m_pStatShootUpValue;

	CBasicTextBox* m_pStatStaminaValue;
	CBasicTextBox* m_pStatStaminaMark;
	CBasicTextBox* m_pStatStaminaMarkImage;
	CBasicTextBox* m_pStatStaminaUpValue;

	CBasicTextBox* m_pStatHitValue;
	CBasicTextBox* m_pStatHitMark;
	CBasicTextBox* m_pStatHitMarkImage;
	CBasicTextBox* m_pStatHitUpValue;

	CBasicTextBox* m_pStatAvoidValue;
	CBasicTextBox* m_pStatAvoidMark;
	CBasicTextBox* m_pStatAvoidMarkImage;
	CBasicTextBox* m_pStatAvoidUpValue;

	CBasicTextBox* m_pStatHpValue;
	CBasicTextBox* m_pStatHpMark;
	CBasicTextBox* m_pStatHpMarkImage;
	CBasicTextBox* m_pStatHpUpValue;

	CBasicTextBox* m_pStatMpValue;
	CBasicTextBox* m_pStatMpMark;
	CBasicTextBox* m_pStatMpMarkImage;
	CBasicTextBox* m_pStatMpUpValue;

	CBasicTextBox* m_pStatSpValue;
	CBasicTextBox* m_pStatSpMark;
	CBasicTextBox* m_pStatSpMarkImage;
	CBasicTextBox* m_pStatSpUpValue;


	CBasicTextBox* m_pFireResistValue;
	CBasicTextBox* m_pColdResistValue;
	CBasicTextBox* m_pElectricResistValue;
	CBasicTextBox* m_pPoisonResistValue;
	CBasicTextBox* m_pSpiritResistValue;


	std::vector<RnButton*> m_PlusMinusButtonVec;
	RnButton*              m_StatInitButton;
	RnButton*              m_StatApplyButton;

	GLGaeaClient*  m_pGaeaClient;
	CInnerInterface* m_pInterface;

	typedef std::auto_ptr<GLCHARLOGIC> AP_CharData;
	AP_CharData m_apCharData;

	int         m_VirtualPowPoint;
	int         m_VirtualDexPoint;
	int         m_VirtualSpiPoint;
	int         m_VirtualStrPoint;
	int         m_VirtualStaPoint;

};




