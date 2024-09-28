#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface;
struct	SNATIVEID;
class	CBasicProgressBar;
class	CSkillImage33;
class	GLGaeaClient;

class CPartyBuffInfoUnit : public CUIGroup
{
public:
	CPartyBuffInfoUnit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPartyBuffInfoUnit ();

public:
	void	CreateSubControl ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void	SetLeftTime ( const float& fLeftTime );
	void	SetSkill ( const DWORD& dwID, const float& fLifeTime, const CString& strSkillName );

private:
	float	m_fLEFT_TIME;
	float	m_fLIFE_TIME;

private:
	CString				m_strSkillName;
	CBasicProgressBar*	m_pProgressBar;
	CSkillImage33*		m_pSkillImage;

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};
