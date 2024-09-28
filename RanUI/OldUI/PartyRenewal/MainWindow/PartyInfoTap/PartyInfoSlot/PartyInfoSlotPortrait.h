#pragma once

#include "../../../../../../RanLogic/Character/GLCharDefine.h"
#include "../../../../Util/UIGroupHelper.h"

class CUIControl;

struct GLPartySlotClient;
class PartyInfoSlotPortrait : public CUIGroupHelper
{
public:	
	virtual void CreateSubControl(void);	
	void SetPortrait(const GLPartySlotClient* const pPartyClient, const bool bAvailable);

	void RESET(void);

public:
	PartyInfoSlotPortrait(EngineDeviceMan* pEngineDevice);

private:
	CUIControl* m_pClassPortraitLive[GLCI_NUM_ETC];
	CUIControl* m_pClassPortraitUnknown[GLCI_NUM_ETC];
	CUIControl* m_pClassPortraitDie;
};