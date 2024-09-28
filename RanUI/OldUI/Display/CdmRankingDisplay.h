#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class GLGaeaClient;

class CCdmRankingDisplay : public CUIGroup
{
private:
	CBasicTextBox*	m_pCdmRankingText;

public:
	CCdmRankingDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~CCdmRankingDisplay();

	void	CreateSubControl ();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:	
	void	RefreashCdmRanking();

protected:
	GLGaeaClient* m_pGaeaClient;

};