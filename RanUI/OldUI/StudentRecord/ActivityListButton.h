#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CBasicButton;

class ActivityListButton : public CUIGroup
{
private:

public:
	ActivityListButton(EngineDeviceMan* pEngineDevice);
	virtual	~ActivityListButton();
    
    void	CreateSubControl ();


    void    SetText ( const CString& strText );
    void    SetFlip ( BOOL bFlip );


    CBasicTextBox* m_pTextBox;
    CBasicButton*  m_pButton;

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};