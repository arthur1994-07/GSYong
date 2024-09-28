#pragma once

#include "../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../InnerInterface.h"
#include "../../../Util/UIGroupHelper.h"
#include "../../../display/ItemInfoSimpleToolTip.h"


class RnProgressBar;
class GLGaeaClient;
class EngineDeviceMan;
class ItemInfoSimpleToolTip;
struct GLTenderItemCustomClient;
class TenderSlot : public CUIGroupHelper
{
public:		
	virtual void SetVisibleSingle(BOOL bVisible) override;

	void CreateSubControl(void);	

	void SetSelect(const bool bSelect);
	void SetProgress(const float fPercent);
	void SetState(const DWORD emTenderState);
	void SetSlotInfo(GLTenderItemCustomClient* const pItemCustom);
	void ResetSlotInfo(void);	

	const bool isSelected(void) const;

public:
	TenderSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~TenderSlot(void);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	
	CUIControl* m_pSelectBox;
	CUIControl* m_pTenderStateIcon[EMPARTY_TENDER_STATE_NSIZE];
	RnProgressBar* m_pProgressBar;

	ItemInfoSimpleToolTip* m_pTenderItemInfo;	
//	Timer* m_pTimer;	
};