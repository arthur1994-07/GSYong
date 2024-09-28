#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;

class CTutorialSkipButton : public CUIGroup
{
public:
	CTutorialSkipButton(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialSkipButton();

protected:
	enum
	{
		TUTORIAL_SKIP_BUTTON = NO_ID + 1,
	};

public:
	void	CreateSubControl ();
	bool	GetSkipEnable() { return m_bSkipEnable; }
	void	SetSkipEnable( bool bSkipEnable );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicButton* m_pSkipButton;
	CBasicTextBox* m_pSkipText;
	bool		  m_bSkipEnable;

protected:
	CInnerInterface* m_pInterface;

};
