#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBonusTimeType;
class GLGaeaClient;

class CBonusTimeDisplay : public CUIGroup
{
public:
	enum
	{
		BONUS_TIME = NO_ID + 1,
		MAX_TYPE = 1
	};

public:
	CBonusTimeDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CBonusTimeDisplay();

private:
    GLGaeaClient* m_pGaeaClient;
    CBonusTimeType* m_pBonus_TYPE;
    CUIControl* m_pBonus_KEEP;
    CUIControl* m_pPositionControl;	

public:
	void CreateSubControl();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	bool START();
	bool RESET();
	bool STOP();
	bool KEEP_START();
	void KEEP_STOP();
};