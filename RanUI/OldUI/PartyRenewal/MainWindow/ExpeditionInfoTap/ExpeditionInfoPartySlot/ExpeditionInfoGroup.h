#pragma once

#include "../../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../../InnerInterface.h"
#include "../../../../Util/UIGroupHelper.h"

class GLGaeaClient;
class GLPartyClient;
class ExpeditionInfoPartySlot;
class ExpeditionInfoGroup : public CUIGroupHelper
{
	enum
	{
		EXPEDITION_INFO_PARTY_SLOT0 = NO_ID + 1,
		EXPEDITION_INFO_PARTY_SLOT1,
		EXPEDITION_INFO_PARTY_SLOT2,
		EXPEDITION_INFO_PARTY_SLOT3,
		EXPEDITION_INFO_PARTY_SLOT4,
		EXPEDITION_INFO_PARTY_SLOT5,
		EXPEDITION_INFO_PARTY_SLOT6,
		EXPEDITION_INFO_PARTY_SLOT7,
	};
public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;	
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;

	const DWORD GetSelectedGaeaID(void) const;
	const DWORD GetSelectedGaeaID(const DWORD dwSelectedSlot) const;
	const DWORD GetSelectedSlot(void) const;
	const CString* GetName(const DWORD dwSelectedSlot) const;

	virtual void SetExpeditionSlot(GLPartyClient* const pParty, const bool bSwapSlotMode = false, const bool bOn = false);

	void CreateSubControl(void);

	void ResetSelect(void);
	void RESET(void);

protected:
	void TranslateUIMessage_SelectPartySlot(const DWORD dwSlotIndex);
	void TranslateUIMessage_ViewPopUp(const DWORD dwSlotIndex);

public:
	ExpeditionInfoGroup(EngineDeviceMan* pEngineDevice);

protected:
	ExpeditionInfoPartySlot* m_pExpeditionInfoMemberSlot[MAXPARTY];
	DWORD m_dwSelectedSlot;
};