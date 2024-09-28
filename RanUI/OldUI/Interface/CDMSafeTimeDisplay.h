#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface;
class	CBasicProgressBar;
class	GLGaeaClient;

class CCDMSafeTimeDisplay : public CUIGroup
{
public:
	CCDMSafeTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CCDMSafeTimeDisplay ();

public:
	void	CreateSubControl ();
	void	SetCDMSafeTime( float fCDMSafeTime ) { m_fLIFE_TIME = fCDMSafeTime; }

public:
	virtual	void SetVisibleSingle ( BOOL bVisible ); 
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float	m_fLEFT_TIME;
	float	m_fLIFE_TIME;

private:
	CBasicProgressBar*	m_pProgressBar;

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};