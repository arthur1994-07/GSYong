#pragma	once

#include "../Util/UIWindowEx.h"

#include "../Party/MiniPartyWindow.h"

#include "../PartyRenewal/MainWindow/ExpeditionInfoTap/ExpeditionInfoPartySlot/ExpeditionInfoGroup.h"
#include "../PartyRenewal/StateWindow/MemberStateGroup/MemberStateGroup.h"

class CInnerInterface;
class CMiniPartySlot;
class CPartyBuffInfo;
class GLGaeaClient;
class CMiniPartyWindow;

struct GLPartySlotClient;

// 파티창 따로 만들려니 양이 많다.
// 따라서 파티창을 상속받아서 사용한다.
class CTournamentExpeditionInfoGroup : public MemberStateGroup
{
public:
	CTournamentExpeditionInfoGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	void SetExpeditionSlot(const Faction::FactionMemberVector* pTeamInfo, const bool bSwapSlotMode = false, const bool bOn = false);

	//virtual	~CTournamentExpeditionInfoGroup ();
	void SetFaction( DWORD _factionid );
	DWORD m_Faction;
	GLGaeaClient* m_pGaeaClient;


};