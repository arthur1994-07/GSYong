#pragma once

#include "../Util/UIWindowEx.h"
#include "../Util/MessageBox.h"

#include "./IFactionSystemUI.h"

class CInnerInterface;
class EngieDeviceMan;

class CFactionInfoWindow : public CUIWindowEx
{
public:
	CFactionInfoWindow ( GLGaeaClient* pClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	void CreateSubControl ( );

private:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

private:
	GLGaeaClient* m_pClient;
};


class MyFactionInfoWindow : public IFactionInfoWindow, public CFactionInfoWindow
{
public:
	MyFactionInfoWindow ( GLGaeaClient* pClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyFactionInfoWindow ( ) { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();
};