#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class GLGaeaClient;

class CWbClubBasicInfoGrade;
class RnButton;

class CWbClubBasicInfoCommand : public CUIGroup
{
private:
    static const float fPOPUP_OFFSET;

private:
    enum
    {
        BACK = NO_ID + 1,
        BAN_BTN,
        DELEGATE_BTN,
        CD_AUTHORIZE_BTN,
        CHANGE_GRADE_BTN,
        SET_GRADE_POPUP,
    };

public:
    CWbClubBasicInfoCommand(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubBasicInfoCommand();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    bool SetCommandData( DWORD dwCharDbNum, const char* strCharName, DWORD dwGrade );

private:
    bool CheckVisibleButtons();

    void ShowButtons();
    void CDAuthorizeBtnTextUpdate();

    void AllUnShow();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL isVisible );

private:
    CBasicLineBoxSmart* m_pLineBox;
    RnButton* m_pBanBtn;
    RnButton* m_pDelegateBtn;
    RnButton* m_pCDAuthorizeBtn;
    RnButton* m_pChangeGradeBtn;

private:
    bool m_bInitPostion;

    DWORD m_dwCharDbNum;
    std::string m_strCharName;
    DWORD m_dwGrade;
};