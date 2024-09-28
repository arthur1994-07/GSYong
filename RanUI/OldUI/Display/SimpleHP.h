#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"


class	CBasicProgressBar;
class   GLSummonClient;
class	GLGaeaClient;

class	CSimpleHP : public CUIGroup
{
public:
	CSimpleHP(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CSimpleHP ();

public:
	void	SetHP ( const int nNOW, const int nMAX );
	void	CreateSubControl ();
	
public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicLineBoxEx*		m_pHPBox;
	CBasicProgressBar*	m_pHP;

protected:
	GLGaeaClient*		m_pGaeaClient;

};