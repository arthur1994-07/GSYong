#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CWbClubManagementGradeNode;

class CWbClubManagementGradeNameEdit : public CUIGroup
{
private:
    static const float fGRADE_OFFSET;

private:
    enum
    {
        BACK = NO_ID + 1,
        GRADE_NODE_START = BACK + 1,
        GRADE_NODE_END = GRADE_NODE_START + club::AUTHORITY_GRADE - 1,
        CLOSE_BTN = GRADE_NODE_END + 1,
    };

public:
    CWbClubManagementGradeNameEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubManagementGradeNameEdit();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL isVisible );

public:
    void CreateSubControl();

private:
    void SetGradeData();

private:
    CBasicLineBoxSmart* m_pBack;

    CWbClubManagementGradeNode* m_pGradeNode[ club::AUTHORITY_GRADE ];

    RnButton* m_pCloseButton;
};