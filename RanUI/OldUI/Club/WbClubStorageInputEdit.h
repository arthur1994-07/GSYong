

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class CInnerInterface;
class GLGaeaClient;

class CWbClubStorageInputEdit : public CUIGroup
{
private:
    static const int nLIMIT_MONEY_CHAR;
private:
    enum
    {
        BACK = NO_ID + 1,
        EDIT_BOX,
        OK_BTN,
        CANCEL_BTN,
    };

    enum EM_INPUT_TYPE
    {
        NONE_TYPE,
        DEPOSIT_TYPE,
        WITHDRAW_TYPE,
    };

public:
    CWbClubStorageInputEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubStorageInputEdit();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    void TranslateMultiEditBoxSmartMsg( CUIEditBox* pMultiEditBoxSmart, DWORD dwMsg );

    void SetDepositType();
    void SetWithDrawType();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

private:
    CBasicTextBox* m_pTitle;
    CBasicTextBox* m_pStatic0;
    CBasicTextBox* m_pStatic1;
    CBasicTextBox* m_pText;
    CUIEditBox* m_pEditBox;

    RnButton* m_pOkBtn;
    RnButton* m_pCancelBtn;

private:
    EM_INPUT_TYPE m_emType;
};