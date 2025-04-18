#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class CBasicProgressBar;
class GLGaeaClient;

class CSkillImage : public CUIGroup
{
public:
	CSkillImage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CSkillImage();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	void SetSkillProcess ( SNATIVEID sNativeID );
	void SetUseTwinkle( bool bUse )					{ m_bTwinkle = bUse; }

	void		ResetSkill ();

	virtual void	SetAdditional( const DWORD dwOption );
	virtual	void	SetSkill ( SNATIVEID sNativeID );

protected:
	virtual	void	CreateProgressBar ();

protected:
	SNATIVEID	m_sICONINDEX;
	CBasicProgressBar*	m_pSkillProcess;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float m_TotElapsedTime;
	bool m_bSkillUsed;
	bool m_bSkillEnd;

	bool m_bTwinkle;
};