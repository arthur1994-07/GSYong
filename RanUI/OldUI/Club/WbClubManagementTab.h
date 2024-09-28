#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class GLGaeaClient;

class CSwapImage;
class CSmartComboBoxRollOver;

class CWbClubManagementNickNameEdit;
class CWbClubManagementGradeNameEdit;
class CWbClubManagementAuthorityEdit;
class CWbClubInfoNoticeEdit;

class CWbClubManagementTab : public CUIGroup
{
private:
    static const int nMAX_AUTO_BAN_DAY;

private:
    enum
    {
        BACK = NO_ID + 1,
        NICK_NAME_BTN,
        GRADE_NAME_BTN,
        AUTHORITY_BTN,
        //CHECK_TO_NEWCOMER,
        //CHECK_TO_OTHER,
        //PUBLIC_SAVE_BTN,
        CHECK_AUTO_BAN,
        AUTO_BAN_SAVE_BTN,
        LEAVE_CLUB_BTN,
        AUTO_BAN_GRADE_ROLLOVER,
        AUTO_BAN_GRADE_BTN,
        AUTO_BAN_DAY_ROLLOVER,
        AUTO_BAN_DAY_BTN,
        CLUB_MARK_EIDT_BTN,
        INTRODUCE_EDIT_BTN,
        BREAK_UP_CLUB_BTN,
    };

public:
    CWbClubManagementTab(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubManagementTab();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    void ClubUpdateNewbieNotice();

private:
    void ShowNickName();
    void UpdateAutoBanRollOver();
    void ShowClubMark();
    //void CheckClubPublic();
    void SetAutoban();
    void ShowButtons();

    //void SavePublic();
    void SaveAutoBan();
    void LeaveClub();
    void ChangeClubMark();
	void ChangeDissolutionTime();

    void EnableAutoban();
    void DisableAutoban();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

private:
    CBasicTextBox* m_pNickNameText;
    RnButton* m_pNickNameBtn;
    CWbClubManagementNickNameEdit* m_pNickNameEdit;
    CWbClubManagementGradeNameEdit* m_pGradeNameEdit;
    CWbClubManagementAuthorityEdit* m_pAuthorityEdit;
    RnButton* m_pGradeEditBtn;
    RnButton* m_pAuthorityEditBtn;

    //CCheckBox* m_pCheckPublicToNewcomer;
    //CCheckBox* m_pCheckPublicToOther;
    //RnButton* m_pPublicSaveBtn;

    CCheckBox* m_pCheckAutoBan;
    RnButton* m_pAutoBanSaveBtn;

    CBasicTextBox* m_pAutoBanGradeText;
    CSmartComboBoxRollOver* m_pAutoBanGradeRollOver;
    CBasicButton* m_pAutoBanGradeBtn;

    CBasicTextBox* m_pAutoBanDayText;
    CSmartComboBoxRollOver* m_pAutoBanDayRollOver;
    CBasicButton* m_pAutoBanDayBtn;

    RnButton* m_pLeaveClubBtn;
    RnButton* m_pBreakUpClubBtn;
	CBasicTextBox* m_pBreakUpClubText;

	CBasicTextBox* m_pClubMarkLeftBackground;
	CBasicTextBox* m_pClubMarkRightBackground;
	CSwapImage* m_pClubMarkBackground;
    CUIControl* m_pClubMark;
    RnButton* m_pClubMarkEditBtn;

    //CBasicTextBox* m_pIntroduceText;
    //RnButton* m_pIntroduceEditBtn;

    CWbClubInfoNoticeEdit* m_pClubIntroduceEdit;

private:
    int m_nSelectGradeIndex;
    int m_nSelectDayIndex;
};