#pragma once

#include "../../InnerInterface.h"
#include "../Util/UIGroupHelper.h"
#include "../PartyRenewal/MiniWindow/MiniExpeditionInfoGroup/MiniExpeditionInfoMemberSlot/MiniExpeditionInfoMemberSlot.h"

class GLGaeaClient;
struct GLPartySlotClient;
class TournamentExpeditionInfoPartySlot : public MiniExpeditionInfoPartySlot
{	
public:
	void SetInformation(DWORD pPartySlot);
	void SetMasterInformation(DWORD pPartySlot, const bool bMaster);
public:
	TournamentExpeditionInfoPartySlot(EngineDeviceMan* pEngineDevice, const DWORD dwPartySlot,GLGaeaClient* pGaeaClient);

private:
	GLGaeaClient* m_pGaeaClient;
};