#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;

class CMiniGauge : public CUIGroup
{
public:
	CMiniGauge(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~CMiniGauge(void);

protected:
	enum
	{
		MINI_GAUGE_BACK			= NO_ID + 1,
		MINI_GAUGE_RED			= NO_ID + 2,
		MINI_GAUGE_YELLOW		= NO_ID + 3,
		MINI_GAUGE_GREEN		= NO_ID + 4,
		MINI_GAUGE_AUTO			= NO_ID + 5,
	};

public:
	void CreateSubControl ();

	void SetRedBar( float MaxPercent, bool AutoUpdate );
	void SetYellowBar( float MaxPercent, bool AutoUpdate );
	void SetGreenBar( float MaxPercent, bool AutoUpdate );

	void SetAutoBar( float MaxPercent, bool AutoUpdate );

    void GaugeUpdate( float CurrentValue );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl* m_pBack;
	CUIControl* m_pRed;
	CUIControl* m_pYellow;
	CUIControl* m_pGreen;

	float m_CurrentValue;
	float m_fTIME;

	int m_nType;

    bool m_bAutoUpdate;

protected:
	GLGaeaClient*	m_pGaeaClient;
};
