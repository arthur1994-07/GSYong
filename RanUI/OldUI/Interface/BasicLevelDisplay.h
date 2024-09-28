#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class GLGaeaClient;

class CBasicLevelDisplay : public CUIGroup
{
public:
	CBasicLevelDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicLevelDisplay ();

public:
	void CreateSubControl();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
    GLGaeaClient* m_pGaeaClient;
	CBasicTextBox*	m_pLevelText;
	WORD m_wLevelBACK;
};