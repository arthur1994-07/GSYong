#pragma once

#include "../../../../../../RanLogic/Character/GLCharDefine.h"
#include "../../../../Util/UIGroupHelper.h"

struct GLPartySlotClient;
class EngineDeviceMan;
class GLGaeaClient;
class CUIControl;
class MemberStateSlotPortrait;
class MemberStateSlotInformation;
class MemberStateSlotSkillEffect;
class MemberStateSlot : public CUIGroupHelper
{
private:
	enum
	{
		MEMBER_STATE_SLOT_PORTRAIT = NO_ID + 1,
		MEMBER_STATE_SLOT_INFORMATION,
		MEMBER_STATE_SLOT_SKILLEFFECT,
	};

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	void SetMemberStateMasterSlot(const GLPartySlotClient* const pPartyClient, const bool bMaster);
	void SetMemberStateSlot(const GLPartySlotClient* const pPartyClient);
	void SetMemberStateSlot(GLCharacter* pChar);
	void SetMemberStateSlot(std::tr1::shared_ptr<GLCharClient> pPartySlot);
	void SetVisibleSkillEffect(const BOOL bShow);
	void CreateSubControl(void);

	float GetVisibleSkillLimitSizeX();
	
	const UIRECT GetSlotPos();
	const UIRECT& GetSlotPortraitPos();
	const UIRECT& GetSlotInfoPos();
	const UIRECT& GetSkillEffectPos();

	void RESET(void);

public:
	MemberStateSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient, const DWORD dwSlotIndex);
	~MemberStateSlot(void){}

private:	
	GLGaeaClient* m_pGaeaClient;

	CUIControl* m_pMasterCrown[2];
	MemberStateSlotPortrait* m_pMemberStateSlotPortrait;
	MemberStateSlotInformation* m_pMemberStateSlotInformation;
	MemberStateSlotSkillEffect* m_pMemberStateSlotSkillEffect;
	const DWORD m_dwSlotIndex;

};