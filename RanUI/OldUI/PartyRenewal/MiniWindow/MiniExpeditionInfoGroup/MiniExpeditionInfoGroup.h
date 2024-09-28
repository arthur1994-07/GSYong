#pragma once

#include "../../../../../RanLogic/Party/GLPartyDefine.h"
#include "../../../../InnerInterface.h"
#include "../../../Util/UIGroupHelper.h"

class GLGaeaClient;
class GLPartyClient;
class CPartyBuffInfo;
class MiniExpeditionInfoPartySlot;
class MiniExpeditionInfoGroup : public CUIGroupHelper
{
protected:
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
		EXPEDITION_INFO_SKILLEFFECT,
	};
public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);	

	const DWORD GetSelectedGaeaID(void) const;
	const DWORD GetSelectedSlot(void) const;
	const CString& GetName(void) const;

	virtual void SetExpeditionSlot(GLPartyClient* const pParty, const DWORD dwPartyIndex = DEFSLOT_NULL);	

	virtual void CreateSubControl(void);

	void ResetSelect(void);
	void RESET(void);

private:
	void TranslateUIMessage_ViewPopUp(const DWORD dwSlotIndex);

public:
	MiniExpeditionInfoGroup(EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface, GLGaeaClient* pGaeaClient);

protected:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	MiniExpeditionInfoPartySlot* m_pExpeditionInfoMemberSlot[MAXPARTY];
	CPartyBuffInfo* m_pPartyBuffInfo;
	DWORD m_dwPartyIndex;
	DWORD m_dwSelectedSlot;
private:
	
};