#pragma	once

#include "ModalWindow.h"

class COuterInterface;

class CModalWindowOuter : public CModalWindow
{

public:
	CModalWindowOuter ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CModalWindowOuter ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	COuterInterface* m_pInterface;

};
