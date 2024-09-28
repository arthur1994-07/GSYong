#pragma once

#include "../../../../InnerInterface.h"
#include "../../../Util/UIGroupHelper.h"

class GLGaeaClient;
class RnButton;
class CBasicTextBox;
class DPSSlot : public CUIGroupHelper
{
	enum
	{
		DPS_SLOT_RESET_BUTTON = NO_ID + 1,
	};
public:	
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;	

public:
	void CreateSubControl(const bool bTitle = false);
	void SetSlotInformation(const DWORD nIndex);
	const bool SetSelect(void);

public:
	DPSSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient, const DWORD nSlotIndex);
	virtual ~DPSSlot(void);

private:
	const DWORD m_nSlotIndex;
	GLGaeaClient* const m_pGaeaClient;
	
	CBasicTextBox* m_pName;
	CBasicTextBox* m_pDamage;
	CBasicTextBox* m_pTime;
	CBasicTextBox* m_pDPS;
	CBasicTextBox* m_pSkillID;
	RnButton* m_pResetButton;
	CUIControl* m_pSelectBox;
};
