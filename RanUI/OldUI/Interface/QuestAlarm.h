#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CQuestAlarm : public CUIGroup
{
private:
static	const	float	fBLINK_TIME_LOOP;

private:
	enum
	{
		QUEST_ALARM_BUTTON = NO_ID + 1,
	};

	bool m_bAlarm;
	CUIControl*	m_pAlarmBlink;
	float m_fBLINK_TIME;

public:
	CQuestAlarm(EngineDeviceMan* pEngineDevice);
	virtual	~CQuestAlarm ();

public:
	void SET_ALARM( bool bAlram );

	void CreateSubControl();

public:
	virtual	void	SetVisibleSingle ( BOOL bVisible );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};