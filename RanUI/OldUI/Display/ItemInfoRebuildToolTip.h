#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Item/GLItemCustom.h"
#include "../../../RanLogic/FSM/TMiniFSM.h"
#include "../Util/UIGroupHelper.h"

class CBasicButton;
class CBasicTextBox;

class CInnerInterface;
class EngineDeviceMan;

class ItemInfoRebuildRandomOptionSlot : public CUIGroupHelper
{
	enum
	{
		EMCHECK_BOX = NO_ID + 1,
	};
public:	
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	void CreateSubControl(void);	

	void hideCheckBox(void);
	void showCheckBox(void);

	void resetCheck(void);
	const bool isChecked(void) const;	

public:
	ItemInfoRebuildRandomOptionSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~ItemInfoRebuildRandomOptionSlot(void);

public:	
	CBasicTextBox* m_pInformation;
	CCheckBox* m_pCheckBox;
	DWORD m_nCountFixOption;

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;		
};

class ItemInfoRebuildToolTip : public CUIGroupHelper
{		
public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	void CreateSubControl(void);
	
	void setItemInformation(const SITEMCUSTOM& itemCustom);
	void setVisibleCheckBox(const bool bVisible, const DWORD nMaxCheckable = 0);
	void resetCheckBox(void);
	void setCheckBox(const SITEMCUSTOM& sItemCustom, const RandomOpt::FixOption& fixOption);

private:
	void _setItemBasicInformation(const SITEMCUSTOM& itemCustom, const SITEM* const pItemData);
	void _setItemAdditionalInformation(const SITEMCUSTOM& itemCustom, const SITEM* const pItemData);
	void _setItemRandomInformation(const SITEMCUSTOM& itemCustom, const SITEM* const pItemData);
	void _setItemRandomInformation_makeText(int nRandOptType, float fVal, const DWORD nCountFix, bool bIsPerRandOpt, DWORD dwAddAbility, const DWORD nIndex, const SITEM* const pItemData);

	void _resizeInformationBox(void);
	void _updateShowCheckBox(const bool bShow);
	void _cutZero(CString& cstr, int ncount);

public:
	ItemInfoRebuildToolTip(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~ItemInfoRebuildToolTip(void);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CBasicTextBox* m_pBasicInformation;	
	CBasicTextBox* m_pAdditionalInformation;	
	ItemInfoRebuildRandomOptionSlot* m_pRandomInformation[RandomOpt::NSIZE];

	CUIControl* m_pResistSpirit;
	CUIControl* m_pResistFire;
	CUIControl* m_pResistIce;
	CUIControl* m_pResistElectric;
	CUIControl* m_pResistPoison;

	CBasicTextBox* m_pResistSpiritText;
	CBasicTextBox* m_pResistFireText;
	CBasicTextBox* m_pResistIceText;
	CBasicTextBox* m_pResistElectricText;
	CBasicTextBox* m_pResistPoisonText;

	SITEMCUSTOM m_ItemCustom;
	D3DXVECTOR2 m_vMousePosition;

	unsignedState m_stateCheckBox;
	DWORD m_nMaxCheckable;
	bool m_bVisibleCheckBox;
	bool m_bDisplayResist;
	float m_fDurabilityPerformance;
};