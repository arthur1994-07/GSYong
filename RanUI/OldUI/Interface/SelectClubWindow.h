#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class GLGaeaClient;

class CSelectCountryNode;
class CBasicTextBox;
class CSelectClubList;
class CSelectClubListField;
class CBasicScrollBarEx;
class RnButton;
class CUIEditBoxMan;
class CBasicComboBoxRollOver;

class CSelectClubWindow : public CUIOuterWindow
{
private:
    enum
    {
        BACK = ET_CONTROL_NEXT + 1,
        MAKE_CLUB_BTN = BACK + 1,
        CLUB_NAME_MAN = BACK + 2,
        CLUB_NAME_EDIT = BACK + 3,
        CLUB_NAME_SEARCH_BUTTON = BACK + 4,
        CLUB_NAME_ROLL_OVER = BACK + 5,
        CLUB_JOIN_BUTTON = BACK + 6,
        CLUB_CLOSE_BUTTON = BACK + 7,
    };

    static const int MAX_ROLLOVER_TEXT = 7;
    static const int NON_SELECT_CLUB_LIST = -1;

    typedef std::list<CString> SRCLIST;
    typedef SRCLIST::iterator SRCLIST_ITER;

public:
    CSelectClubWindow(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSelectClubWindow();

private:
    GLGaeaClient* m_pGaeaClient;

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    void LoadSearchList();
    void ComboInit();
    void UpdateComoboItem();
    void ResetSelectClub();

    void JoinClub();
    void MakeNewClub();

    void GetCountryByServiceProvider( wb::EM_COUNTRY& Country );

public:
    void CreateSubControl();

    void SetCountryInfo( const wb::EM_COUNTRY& emCountry );
    void ReloadClubList();

    void SetClubListOrder( wb::search::EM_ORDER_TYPE Type, wb::search::EM_ORDER_ASC_DSC Order );
    void SearchClub();
    void SelectClubList( int nIndex );

private:
    CSelectCountryNode* m_pFlag;
    CBasicTextBox* m_pCountryInfoText;
    CSelectClubList* m_pClubList;

    RnButton* m_pMakeClubBtn;
    CUIEditBoxMan* m_pClubNameEdit;
    RnButton* m_pSearchButton;
    CBasicComboBoxRollOver*	m_pComboRollSearchOver;

    CBasicTextBox* m_pSelectClubInfo;
    CBasicTextBox* m_pNotify;
    RnButton* m_pJoinButton;
    RnButton* m_pCloseButton;

    std::vector<CString> m_vecString;
    SRCLIST m_listSrcString;
    int m_nSelectString;
    int m_nComboPageNum;
    int m_nBeforeStrLength;
    std::string m_strBeforeEdit;

    int m_nSelectClubIndex;
};