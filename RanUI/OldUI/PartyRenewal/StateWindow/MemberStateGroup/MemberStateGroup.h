#pragma once

#include "../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../Util/UIGroupHelper.h"

class CInnerInterface;
class GLGaeaClient;
class EngineDeviceMan;
class MemberStateSlot;
class MemberStateGroup : public CUIGroupHelper
{
protected:
	enum
	{
		MEMBER_STATE_SLOT0 = NO_ID + 1,
		MEMBER_STATE_SLOT1,
		MEMBER_STATE_SLOT2,
		MEMBER_STATE_SLOT3,
		MEMBER_STATE_SLOT4,
		MEMBER_STATE_SLOT5,
		MEMBER_STATE_SLOT6,
		MEMBER_STATE_SLOT7,
	};
public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;

	void CreateSubControl(void);

	void SetVisibleSkillEffect(const BOOL bShow);

	// 현재 파티상태창에서 보여지는 버프스킬들의 최대width값을 받아온다.
	float GetVisibleSkillEffectSizeX();

public:
	MemberStateGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~MemberStateGroup(void){};

public:
	GLGaeaClient* m_pGaeaClient;

	MemberStateSlot* m_pMemberSlot[MAXPARTY];
	D3DXVECTOR2 m_vMousePosition;
};