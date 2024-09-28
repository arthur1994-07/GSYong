#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CWbClubAuthorityEditRow;

class CWbClubManagementAuthorityEdit : public CUIGroup
{
private:
    static const float fROW_OFFSET;
    static const float fCOL_OFFSET;

private:
    enum
    {
        BACK = NO_ID + 1,
        OK_BTN,
        CANCEL_BTN,
    };

public:
    CWbClubManagementAuthorityEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubManagementAuthorityEdit();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

public:
    void CreateSubControl();

private:
    void Refresh();

    void CheckAuthorities( size_t nGradeIndex );
    void SaveAuthorities();

    club::AUTHORITY GetFlagByAuthorityIndex( size_t nAuthorityIndex );

private:
    std::vector<CWbClubAuthorityEditRow*> m_pVecAuthorityEditRow;
    CBasicTextBox* m_pGradeName[ club::AUTHORITY_GRADE ];

    RnButton* m_pOkBtn;
    RnButton* m_pCancelBtn;
};