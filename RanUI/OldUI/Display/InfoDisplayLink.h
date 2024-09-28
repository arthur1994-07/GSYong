#pragma	once

#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"

class CInnerInterface;
class CBasicButton;

class CInfoDisplayLink : public CBasicVarTextBox
{
	enum 
	{
		BASIC_VAR_LINE_BOX_LINK = NO_ID + 1,
		BASIC_VAR_LINE_BOX_LINK_DUMMY,
		VAR_TEXT_BOX_LINK, 
		ET_CONTROL_BUTTON,
	};

public:
	CInfoDisplayLink(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CInfoDisplayLink();

	void	CreateSubControl ();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

	virtual void RePosControl ( int x, int y );

public:
	CBasicButton* m_pClose;
	
public:
	CInnerInterface* m_pInterface;
};