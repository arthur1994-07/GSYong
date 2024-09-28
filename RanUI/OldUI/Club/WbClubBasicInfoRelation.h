#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicVarTextBox;

class CWbClubBasicInfoRelation : public CUIGroup,  public CTapSelectorEx<DEFAULT_RADIO_BUTTON>
{
private:
    static const int NON_SELECT;

private:
    enum
    {
        BACK = NO_ID + 1,
        RADIO_BASIC_INFO,
        RADIO_ALLIANCE,
        RADIO_ENEMY,
        BASIC_INFO_0,
        BASIC_INFO_1,
        RELATION_0,
        RELATION_1,
        ALLIANCE_BTN,
        ARMISTICE_BTN,
        SUBMISSION_BTN,
    };

public:
    CWbClubBasicInfoRelation(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubBasicInfoRelation();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

    virtual void EventSelectedTap( UIGUID ControlID );

    int GetCurTab();
	const DWORD GetClubAllianceID();

    void ShowClubAllianceList();
    void ShowClubBattleList();
    void RefreshGuidanceMap();

public:
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
    void AllUnshow();
    void ShowBasicInfo();
    void ShowRelationBtns();

    void ShowClubMarks();

    bool IsSelected( size_t MaxSize );

    void SelectList( UIRECT& rcPart );
    void UnSelect();

	bool IsSelectedIndex();

	//  [12/11/2012 gbgim]
	void ResetToolTip();

private:
    DEFAULT_RADIO_BUTTON* m_pRadioBasicInfo;
    DEFAULT_RADIO_BUTTON* m_pRadioAlliance;
    DEFAULT_RADIO_BUTTON* m_pRadioEnemy;

    CBasicTextBoxEx* m_pBasicInfo0;
    CBasicTextBoxEx* m_pBasicInfo1;
    CBasicLineBoxSmart* m_pSepLine;
    CBasicScrollBarEx* m_pBasicScroll;

    CUIControl* m_pRelationBody;
    CBasicTextBoxEx* m_pRelation0;
    CBasicTextBoxEx* m_pRelation1;
    CBasicScrollBarEx* m_pRelationScroll;
    CUIControl* m_pSelect;

    RnButton* m_pAllianceBtn;
    RnButton* m_pArmisticeBtn;
    RnButton* m_pSubmissionBtn;

    std::vector<CUIControl*> m_pClubMarkVec;

	// [12/11/2012 gbgim]
	CBasicVarTextBox*	m_pClubChiefToolTip;


private:
    int m_nCurRadioTab;
    int m_nSelectIndex;

    std::vector<GLCLUBALLIANCE> m_vecClubAllinace;
    std::vector<GLCLUBBATTLE> m_vecClubBattle;
};