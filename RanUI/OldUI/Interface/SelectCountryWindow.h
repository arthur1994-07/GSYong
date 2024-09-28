#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class GLGaeaClient;

class CSelectCountryNode;
class CBasicScrollBarEx;
class CBasicTextBox;
class CBasicLineBoxSmart;
class CUIEditBoxMan;
class CBasicComboBoxRollOver;
class RnButton;

class CSelectCountryWindow : public CUIOuterWindow
{
private:
    static const int NUM_NODE_PER_ROW = 5;
    static const int MAX_ROW_ONE_PAGE = 3;
    static const int MAX_NODE_ONE_PAGE = NUM_NODE_PER_ROW * MAX_ROW_ONE_PAGE;

    static const float NODE_HORIZONTAL_INTERVAL_OFFSET;
    static const float NODE_VERTICAL_INTERVAL_OFFSET;

    static const int NON_SELECT_COUNTRY_NODE = -1;

    static const int MAX_ROLLOVER_TEXT = 10;

    typedef std::list<CString> SRCLIST;
    typedef SRCLIST::iterator SRCLIST_ITER;

    enum
    {
        BACK = ET_CONTROL_NEXT + 1,
        SCROLL_BAR = ET_CONTROL_NEXT + 2,
        OK_BUTTON = ET_CONTROL_NEXT + 3,
        COUNTRY_NAME_SEARCH_BUTTON = ET_CONTROL_NEXT + 4,
        COUNTRY_NAME_ROLL_OVER = ET_CONTROL_NEXT + 5,
        COUNTRY_NAME_MAN = ET_CONTROL_NEXT + 6,
        COUNTRY_NAME_EDIT = ET_CONTROL_NEXT + 7,

        NODE_START = COUNTRY_NAME_EDIT + 1, // 항상 제일 마지막에 둬야한다.
    };

public:
    CSelectCountryWindow(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSelectCountryWindow();

private:
    GLGaeaClient* m_pGaeaClient;

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL isVisible );

public:
    void CreateSubControl();

    void ReLoadFlags();

    void ShowCountryClubCount();

private:
    void SetLocateAllCountryNodes();

    void SelectCountryNode( int nNodeIndex );
    void SetLocateSelectNode( int nNodeIndex );

    void SearchCountry();
    void SearchCountryNodeByInfoIndex( int nInfoIndex );

    void ComboInit();
    void UpdateComoboItem();
    void LoadSearchList();
    void ResetAll();

    void DoneSelectCountry();

    bool IsSelected();

private:
    CBasicTextBox* m_pTitle;
    CBasicLineBoxSmart* m_pCountryBack;
    CBasicScrollBarEx* m_pMainScrollBar;
    CSelectCountryNode* m_pCountryNode[ MAX_NODE_ONE_PAGE ];
    CUIControl* m_pCountryNodeTemp;
    CBasicLineBoxSmart* m_pSelectCountryNode;

    CUIEditBoxMan* m_pCountryNameEdit;
    CBasicComboBoxRollOver*	m_pComboRollSearchOver;
    RnButton* m_pSearchButton;

    CSelectCountryNode* m_pSelectCountryFlag;
    CBasicTextBox* m_pSelectCountryDescText;
    CBasicTextBox* m_pSelectCountryNotify;

    RnButton* m_pOKButton;

private:
    std::vector<wb::COUNTRY_INFO> m_vecCountryInfo;
    int m_nStartRowIndex;
    int m_nCountryInfoSize;
    int m_nSelectCountryNode;

    std::vector<CString> m_vecString;
    SRCLIST m_listSrcString;
    int m_nSelectString;
    int m_nComboPageNum;
    int m_nBeforeStrLength;
    std::string m_strBeforeEdit;
};