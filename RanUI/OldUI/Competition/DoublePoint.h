#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface;
class	DoublePintType;
class	GLGaeaClient;

class	DoublePoint : public CUIGroup
{
public:
	enum
	{
		DOUBLE_POINT = NO_ID + 1,

		MAXTYPE = 1
	};

public:
	DoublePoint(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~DoublePoint ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void SetVisibleSingle(BOOL bIsVisible);

public:
	bool	START ( UIGUID cID );
	bool	RESET ( UIGUID cID );
	bool	STOP ( UIGUID cID );

	void	ALLSTOP ();

public:
	bool	KEEP_START ( UIGUID cID );
	void	KEEP_STOP ();

private:
	DoublePintType*	m_pQI_TYPE[MAXTYPE];
	CUIControl*			m_QI_TYPE_KEEP[MAXTYPE];

	CUIControl*		m_pPositionControl;	

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};