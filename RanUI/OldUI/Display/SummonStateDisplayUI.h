#pragma once

#include "../Util/UIGroupHelperNonClose.h"
#include "../../InnerInterface.h"

#include "./MiniGauge.h"
#include "./ISummonStateDisplayUI.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CSwapImage;

class CSummonStateDisplayUI : public CUIGroupHelperNonClose
{
private:
	enum
	{
		ENotifyButton		= NO_ID +1
	};

public:
	CSummonStateDisplayUI( GLGaeaClient* pClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CSummonStateDisplayUI();

	void CreateSubControl();

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
	GLGaeaClient* m_pClient;

	CSwapImage* m_pSummonImage;
	CMiniGauge* m_pHPGauge;

	std::string m_strTooltip;
	std::string m_strSummonName;
};

class MySummonStateDisplayUI : public ISummonStateDisplayUI, public CSummonStateDisplayUI
{
public:
	MySummonStateDisplayUI( GLGaeaClient* pClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MySummonStateDisplayUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();
};