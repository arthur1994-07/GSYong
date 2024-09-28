#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CWbClubAuthorityEditRow : public CUIGroup
{
private:
    static const float fCHECKBOX_OFFSET;


private:
    enum
    {
        BACK = NO_ID + 1,
        CHECK_START = BACK + 1,
        CHECK_END = CHECK_START + club::AUTHORITY_GRADE - 1,
    };

public:
    CWbClubAuthorityEditRow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubAuthorityEditRow();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl();

    void InitAuthorityData( const std::string& strAuthorityNAme );
    void CheckAuthority( size_t nGradeIndex );

    bool IsAuthorityChecked( size_t nGradeIndex );

private:
    CBasicTextBox* m_pAuthorityText;
    CCheckBox* m_pCheckBox[ club::AUTHORITY_GRADE ];
};