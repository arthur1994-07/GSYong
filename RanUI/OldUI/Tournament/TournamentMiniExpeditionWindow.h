#pragma once

#include "../../InnerInterface.h"
#include "../Util/TapSelectorEx.h"
#include "../Util/UIWindowEx.h"

#include "../PartyRenewal/MiniWindow/MiniExpeditionWindow.h"

class GLGaeaClient;
class CInnerInterface;
class CTournamentMiniExpeditionInfoGroup;

class MiniExpeditionInfoGroup;
class TournamentMiniExpeditionWindow : public MiniExpeditionWindow
{
public: 

	//Tap 이 전역으로 ID가 입력되기 때문에..  전역으로 Tap ID 부여를 지정한다.
	static DWORD TournamentTapID;

	DWORD TOR_EXPEDITION_TAP_1;


public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg ) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;

	void	CreateSubControl(void);

private:
	virtual void EventSelectedTap(UIGUID controlID);

public:
	TournamentMiniExpeditionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~TournamentMiniExpeditionWindow(void);

	virtual void CreateUIWindowAndRegisterOwnership(WORD wFlag);

public:
	void SetFaction(DWORD _Faction) { m_Faction = _Faction, m_isInit = true; }
private:
	DWORD m_Faction;
	bool m_bNotFound;
	bool m_isInit;

protected:
	CTournamentMiniExpeditionInfoGroup* m_pExpeditionInfoGroup;	
};