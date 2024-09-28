#pragma once

#include "../../../../Util/UIGroupHelper.h"
#include "../../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../../../RanLogic/Character/GLCharData.h"

class SkillEffectSlot;

class CUIControl;
class GLGaeaClient;
struct GLPartySlotClient;
class MemberStateSlotSkillEffect : public CUIGroupHelper
{
	enum
	{
		SKILLEFFECT_SLOT0 = NO_ID + 1,
        SKILLEFFECT_SLOT_SIZE = SKILLFACT_SIZE //스킬지속효과 최대 갯수
	};

public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg){}	

	void CreateSubControl(const bool bSmall = false);
	void SetSkillEffect(const DWORD dwGaeaID);
	void ResetSlotSkillEffect();

	// 현재 파티상태창에서 보여지는 버프스킬들의 최대width값을 받아온다.
	float GetVisibleSkillEffectSizeX();

public:
	MemberStateSlotSkillEffect(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~MemberStateSlotSkillEffect(void){}

private:
	GLGaeaClient* m_pGaeaClient;
	CUIControl* m_pDummySlot[SKILLEFFECT_SLOT_SIZE]; // 위치값 참조를 위한 더미 슬롯;
	SkillEffectSlot* m_pSkillSlot[SKILLEFFECT_SLOT_SIZE];
	
};