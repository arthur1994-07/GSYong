#pragma once

#include "../../../../RanLogicClient/DamageRecord/DamageRecordDefine.h"
#include "../../../InnerInterface.h"
#include "../../Util/UIGroupHelper.h"

class CBasicScrollBarEx;
class DetailSlot;
class DetailPage : public CUIGroupHelper
{
	enum
	{
		DETAIL_SLOT0 = NO_ID + 1,
		DETAIL_SLOT1,
		DETAIL_SLOT2,
		DETAIL_SLOT3,
		DETAIL_SLOT4,
		DETAIL_SLOT5,
		DETAIL_SLOT6,
		DETAIL_SLOT7,
		DETAIL_NSIZE = (DETAIL_SLOT7 - DETAIL_SLOT0),
	};
public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;

public:
	void CreateSubControl(void);
	void SetActorIndex(const DWORD nActorIndex);

public:
	DetailPage(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~DetailPage(void){};

private:
	GLGaeaClient* const m_pGaeaClient;
	CBasicScrollBarEx* m_pScrollBar;
	DetailSlot* m_pDetailSlot[DETAIL_NSIZE + 1]; // +1 : ≈∏¿Ã∆≤ ΩΩ∑‘;
	DWORD m_nCurrentBeginIndex;	
	DWORD m_nSelectActorIndex;
};