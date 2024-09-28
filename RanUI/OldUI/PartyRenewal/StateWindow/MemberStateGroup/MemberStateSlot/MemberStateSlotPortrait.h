#pragma once

#include "../../../../../../RanLogic/Character/GLCharDefine.h"
#include "../../../../Util/UIGroupHelper.h"

class CUIControl;

struct GLPartySlotClient;
class GLGaeaClient;
class MemberStateSlotPortrait : public CUIGroupHelper
{
public:
	void SetPortrait(std::tr1::shared_ptr<GLCharClient> pPartyClient, const bool bAvailable);
	void SetPortrait(const GLPartySlotClient* const pPartyClient, const bool bAvailable);
	void SetPortrait(GLCharacter* pChar, const bool bAvailable);
	void CreatesubControl(void);

	void RESET(void);

public:
	MemberStateSlotPortrait(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~MemberStateSlotPortrait(void){}

private:
	GLGaeaClient* const m_pGaeaClient;
	CUIControl* m_pClassPortraitLive[GLCI_NUM_ETC];	
	CUIControl* m_pClassPortraitUnknown[GLCI_NUM_ETC];	
	CUIControl* m_pClassPortraitDie;
};