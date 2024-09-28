#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Display/MiniGauge.h"

class CInnerInterface;
class CMiniGauge;
class CBasicButton;
class GLGaeaClient;

enum QBOX_TYPE
{
	QBOX_NONE = 0,
	QBOX_POWER_UP = 1,
	QBOX_POWER_UP_MAX = 2,
	QBOX_EXP_TIME = 3,
	QBOX_SPEED_UP = 4,
	QBOX_SPEED_UP_MAX = 5,
	QBOX_CRAZY_TIME = 6,
	QBOX_MADNESS_TIME = 7,
	QBOX_LUCKY = 8,

	QBOX_TYPE_SIZE = 9,
};

class CQBoxButton : public CUIGroup
{
public:
	CQBoxButton(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CQBoxButton(void);

protected:
    GLGaeaClient* m_pGaeaClient;

	enum
	{
		QBOX_NONE_BUTTON			= NO_ID + 1,
		QBOX_POWER_UP_BUTTON		= NO_ID + 2,
		QBOX_POWER_UP_MAX_BUTTON	= NO_ID + 3,
		QBOX_EXP_TIME_BUTTON		= NO_ID + 4,
		QBOX_SPEED_UP_BUTTON		= NO_ID + 5,
		QBOX_SPEED_UP_MAX_BUTTON	= NO_ID + 6,
		QBOX_CRAZY_TIME_BUTTON		= NO_ID + 7,
		QBOX_MADNESS_TIME_BUTTON	= NO_ID + 8,
		QBOX_LUCKY_BUTTON			= NO_ID + 9,
		QBOX_LOCK_BUTTON			= NO_ID + 10,
		QBOX_GAUGE					= NO_ID + 11,
		QBOX_ALARM					= NO_ID + 12,
	};

public:
	void	CreateSubControl ();
	bool	GetQBoxEnable() { return m_bQBoxEnable; }
	void	SetQBoxEnable( bool bQBoxEnable );

	int		GetQBoxType()	{ return m_nType; }

	void	SetQBoxType( int nType );
	void	ResetQBox();

	void	QBOXButtonDown();

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicButton*	m_pQBoxButton[ QBOX_TYPE_SIZE ];
	CUIControl*		m_pAlarmBlink;
	CUIControl*		m_pQBoxButtonLock;
	CMiniGauge*		m_pGauge;

	bool			m_bQBoxEnable;
	bool			m_bQBoxAlarm;
	float			m_fBLINK_TIME;

	int			  m_nType; // 큐박스 종류. by luxes.

protected:
	CInnerInterface* m_pInterface;

};
