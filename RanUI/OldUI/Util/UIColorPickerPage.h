#pragma once

#include "../Util/UIPage.h"

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class CBasicTextButton;
class CBasicTextBox;
class CSwapImage;
class CUIColorPickerGroup;
class CUITextButton;

class CUIColorPickerPage : public CUIPage
{

public:
    enum
    {
        COLORPICKER_PAGE = PAGE_NEXT,

        COLORPICKER_PAGE_GROUP,
        COLORPICKER_PAGE_BUTTON_CANCEL,
        COLORPICKER_PAGE_BUTTON_OK,
    };

public:
    static const DWORD dwDEFAULTCOLOR  = 0xFFFFFFFF;
    static const DWORD dwRESTRICTCOLOR = 0xFF808080;

public:
    CUIColorPickerPage( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CUIColorPickerPage();

public:
    void	     CreateSubControl ();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual	void SetVisibleSingle( BOOL bVisible );

protected:
    virtual void OpenSubControl() {}

public:
    void         SetRestrict( const BOOL bRestrict );
    void         SetColor( const DWORD dwColor );
    const DWORD  GetColor();

private:
    CUIColorPickerGroup*  m_pPicker;
    CBasicTextBox*        m_pLabel;
    CUITextButton*        m_pButton_Cancel;
    CUITextButton*        m_pButton_Ok;

protected:
    CInnerInterface*      m_pInterface;

};
