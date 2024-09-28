#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CUIMultiEditBoxSmart;

class CWbClubManagementNickNameEdit : public CUIGroup
{
private:
    static const int nMAX_NICKNAME_LENGTH;
private:
    enum
    {
        BACK = NO_ID + 1,
        OK_BUTTON,
        CANCEL_BUTTON,
        EDIT_NICKNAME,
    };

public:
    CWbClubManagementNickNameEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubManagementNickNameEdit();

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

private:
    CBasicTextBox* m_pTitle;
    CUIMultiEditBoxSmart* m_pNickNameEditBox;
    CBasicTextBox* m_pLimitText;
    RnButton* m_pOkButton;
    RnButton* m_pCancelButton;
};