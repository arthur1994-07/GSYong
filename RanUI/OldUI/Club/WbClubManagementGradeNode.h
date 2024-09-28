#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CUIMultiEditBoxSmart;

class CWbClubManagementGradeNode : public CUIGroup
{
private:
    static const int nMAX_GRADENAME_LENGTH;

private:
    enum
    {
        BACK = NO_ID + 1,
        EDIT_GRADE,
        SAVE_BTN,
    };

public:
    CWbClubManagementGradeNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubManagementGradeNode();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl();

    void TranslateMultiEditBoxSmartMsg( CUIMultiEditBoxSmart* pMultiEditBoxSmart, DWORD dwMsg );

    void SetGradeData( int nIndex );

private:
    CBasicTextBox* m_pIndexText;
    CUIMultiEditBoxSmart* m_pGradeEditBox;
    CBasicTextBox* m_pGradeName;
    RnButton* m_pSaveBtn;

private:
    int m_nIndex;
};