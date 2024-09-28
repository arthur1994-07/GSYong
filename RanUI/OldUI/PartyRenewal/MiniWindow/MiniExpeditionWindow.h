#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/TapSelectorEx.h"
#include "../../Util/UIWindowEx.h"

class MiniExpeditionInfoGroup;
class MiniExpeditionWindow : public CUIWindowEx, public CTapSelectorEx<DEFAULT_TAPBUTTON>
{
public:
	enum
	{
		EXPEDITION_TAP_1 = ET_CONTROL_NEXT + 1,
		EXPEDITION_TAP_2,
		EXPEDITION_TAP_3,
		EXPEDITION_TAP_4,
		EXPEDITION_TAP_5,
	};

public:
 	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
 	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg ) override;
 	virtual void SetVisibleSingle(BOOL bVisible) override;

    //미니파티 윈도우와 미니원정대 윈도우는 동일한 UI폼을 사용하여 만듬
    void SetMiniWindowForParty();
    void SetMiniWindowForExpedition();

 	virtual void CreateSubControl(void);

private:
	virtual void EventSelectedTap(UIGUID controlID);

public:
	MiniExpeditionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~MiniExpeditionWindow(void);

protected:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

    CUIControl* m_pExpeditionTap[MAX_PARTYGROUP];
	MiniExpeditionInfoGroup* m_pExpeditionInfoGroup;	
	DWORD m_dwTapPageIndex;
};

class MyMiniExpeditionWindow : public IMiniExpeditionWindow, public MiniExpeditionWindow
{
public:
	MyMiniExpeditionWindow( GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyMiniExpeditionWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
};