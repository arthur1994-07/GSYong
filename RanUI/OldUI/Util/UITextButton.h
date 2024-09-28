
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_UITEXTBUTTON_PUSHUP = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CSwapImage;
class CBasicTextBox;
class CD3DFontPar;

class CUITextButton : public CUIGroup
{
protected:
    enum
    {
        TEXTBUTTON_NONE   = NO_ID,
        TEXTBUTTON_BUTTON,
    };

public:
    enum EMMODE
    {
        EMMODE_DEFAULT     = 0,
        EMMODE_FLIP        = 1,
        EMMODE_FLIP_MANUAL = 2,
    };

public:
    CUITextButton(EngineDeviceMan* pEngineDevice);
    virtual	~CUITextButton ();

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    virtual void CreateSubControl ( const char* szDefaultTexture, const char* szClickTexture, CD3DFontPar* pFont, CUITextButton::EMMODE emMode=EMMODE_DEFAULT );

public:
    virtual void SetOneLineText ( const CString& strText, const D3DCOLOR& TextColor=NS_UITEXTUTIL::WHITE );

public:
    void         Click  ( const bool bEnable );
    void         Toggle ();

protected:
    virtual void UIMessageDefault ( UIGUID ControlID, DWORD dwMsg );
    virtual void UIMessageFlip    ( UIGUID ControlID, DWORD dwMsg );

protected:
    CSwapImage*	    m_pImage_Default;
    CSwapImage*	    m_pImage_Click;
    CBasicTextBox*  m_pTextBox;

protected:
    CString         m_strText;
    EMMODE          m_emMode;
    BOOL            m_bDown;

};
