#pragma	once

#include "../Util/UIWindowEx.h"
#include "../PartyRenewal/MiniWindow/MiniExpeditionInfoGroup/MiniExpeditionInfoGroup.h"

class CInnerInterface;
class CMiniPartySlot;
class CPartyBuffInfo;
class GLGaeaClient;
class CMiniPartyWindow;
class TournamentExpeditionInfoPartySlot;

namespace MatchingSystem
{
	struct TournamentTeamInfo;
}
class MiniExpeditionInfoGroup;

struct GLPartySlotClient;

// 파티창 따로 만들려니 양이 많다.
// 따라서 파티창을 상속받아서 사용한다.
class CTournamentMiniExpeditionInfoGroup : public MiniExpeditionInfoGroup
{
protected:
	enum
	{
		TOURNAMENT_INFO_PARTY_SLOT0 = NO_ID + 10,
		TOURNAMENT_INFO_PARTY_SLOT1,
		TOURNAMENT_INFO_PARTY_SLOT2,
		TOURNAMENT_INFO_PARTY_SLOT3,
		TOURNAMENT_INFO_PARTY_SLOT4,
		TOURNAMENT_INFO_PARTY_SLOT5,
		TOURNAMENT_INFO_PARTY_SLOT6,
		TOURNAMENT_INFO_PARTY_SLOT7,
		TOURNAMENT_INFO_SKILLEFFECT,
	};
public:
	CTournamentMiniExpeditionInfoGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CTournamentMiniExpeditionInfoGroup ();

	
	void CreateSubControl(void);
	void SetExpeditionSlot( const Faction::FactionMemberVector* pTeamInfo, const DWORD dwPartyIndex  );
public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:

	TournamentExpeditionInfoPartySlot* m_pTournamentExpeditionInfoMemberSlot[MAXPARTY];

	DWORD m_dwFaction;

};