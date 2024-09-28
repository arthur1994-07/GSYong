#pragma once

#include "../../../../RanLogicClient/DamageRecord/DamageRecordDefine.h"
#include "../../../InnerInterface.h"
#include "../../Util/UIGroupHelper.h"

class DPSSlot;
class DPSPage : public CUIGroupHelper
{
	enum
	{
		DPS_SLOT0 = NO_ID + 1, // Title Slot;
		DPS_SLOT1, // Actor Slot;
		DPS_SLOT2,
		DPS_SLOT3,
		DPS_SLOT4,
		DPS_SLOT5,
		// DamageRecord::VIEW_SLOT_NSIZE
	};
public:	
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

public:
	const bool isSelected(void) const;
	const DWORD GetSelectIndex(void) const;
	void ResetSelectIndex(void);
	void CreateSubControl(void);

public:
	DPSPage(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~DPSPage(void){};

private:
	GLGaeaClient* const m_pGaeaClient;
	DPSSlot* m_pDPSSlot[DamageRecord::VIEW_SLOT_NSIZE + 1]; // +1 : ≈∏¿Ã∆≤ ΩΩ∑‘;
	
	DWORD m_nSelectIndex;
};