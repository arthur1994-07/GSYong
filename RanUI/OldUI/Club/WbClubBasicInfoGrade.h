#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class GLGaeaClient;

class RnButton;

class CWbClubBasicInfoGrade : public CUIGroup,  public CTapSelectorEx<DEFAULT_RADIO_BUTTON>
{
private:
    static const float fRADIO_OFFSET;

private:
    enum
    {
        BACK = NO_ID + 1,
        RADIO_START = BACK + 1,
        RADIO_END = RADIO_START + club::AUTHORITY_GRADE - 1,
        OK_BUTTON = RADIO_END + 1,
        CANCEL_BUTTON = RADIO_END + 2,
    };

public:
    CWbClubBasicInfoGrade(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubBasicInfoGrade();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    void SetGradeData( DWORD dwCharDbNum, const char* strCharName, DWORD dwGrade );

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

    virtual void EventSelectedTap( UIGUID ControlID );

private:
    bool IsSelected();

private:
    CBasicLineBoxSmart* m_pLineBox;
    CBasicTextBox* m_pTitle;
    DEFAULT_RADIO_BUTTON* m_pRadioButton[ club::AUTHORITY_GRADE ];

    RnButton* m_pOkButton;
    RnButton* m_pCancelButton;

private:
    DWORD m_dwID;
    int m_nGradeIndex;
};