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
        SKILLEFFECT_SLOT_SIZE = SKILLFACT_SIZE //��ų����ȿ�� �ִ� ����
	};

public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg){}	

	void CreateSubControl(const bool bSmall = false);
	void SetSkillEffect(const DWORD dwGaeaID);
	void ResetSlotSkillEffect();

	// ���� ��Ƽ����â���� �������� ������ų���� �ִ�width���� �޾ƿ´�.
	float GetVisibleSkillEffectSizeX();

public:
	MemberStateSlotSkillEffect(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~MemberStateSlotSkillEffect(void){}

private:
	GLGaeaClient* m_pGaeaClient;
	CUIControl* m_pDummySlot[SKILLEFFECT_SLOT_SIZE]; // ��ġ�� ������ ���� ���� ����;
	SkillEffectSlot* m_pSkillSlot[SKILLEFFECT_SLOT_SIZE];
	
};