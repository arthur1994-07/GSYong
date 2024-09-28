#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CUIMultiEditBoxSmart;

class CWbClubInfoNoticeEdit : public CUIGroup
{
private:
    enum
    {
        BACK = NO_ID + 1,
        OK_BUTTON,
        CANCEL_BUTTON,
        NOTICE_EDIT,
    };

    enum EM_NOTICE_TYPE
    {
        NONE_TYPE,
        CLUB_NOTICE_TYPE,
        CLUB_INTRODUCE_TYPE,
    };

public:
    CWbClubInfoNoticeEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubInfoNoticeEdit();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL isVisible );

public:
    void CreateSubControl();

    void TranslateMultiEditBoxSmartMsg( CUIMultiEditBoxSmart* pMultiEditBoxSmart, DWORD dwMsg );

    void SetClubNoticeType();
    void SetClubIntroduceType();

    void SetLimitInput( int nLimitInput );

private:
    void PressOKButton();

private:
    CBasicTextBox* m_pTitle;
    CUIMultiEditBoxSmart* m_pNoticeEditBox;
    CBasicTextBox* m_pLimitText;
    RnButton* m_pOkButton;
    RnButton* m_pCancelButton;

private:
    EM_NOTICE_TYPE m_emType;
};