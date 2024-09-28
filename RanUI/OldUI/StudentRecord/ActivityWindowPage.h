#pragma	once

#include "../Util/UIPage.h"

#include "../../../RanLogic/Activity/ActivityBase.h"

class CBasicScrollBarEx;
class ActivityGrade;
class CTipBox;
class CCheckBox;
class CBasicButton;
class ActivityMainPage;
class CInnerInterface;
class GLGaeaClient;

//-----------------------------------------------------------------------------------------------//


class CActivityWindowPage : public CUIPage
{
private:

    enum UI_SPEC_ACT_GATEGORY
    {
        UI_SPEC_ACT_GATEGORY_ALL   = 0,
        UI_SPEC_ACT_GATEGORY_LEVEL = 1,
        UI_SPEC_ACT_GATEGORY_KILL  = 2,
        UI_SPEC_ACT_GATEGORY_TAKE  = 3,
        UI_SPEC_ACT_GATEGORY_MAP   = 4,
        UI_SPEC_ACT_GATEGORY_USE   = 5,
        UI_SPEC_ACT_GATEGORY_SIZE  = 6,
    };

    enum 
    {
        UI_ACTIVITY_CLASS_HIGH    = 0,
        UI_ACTIVITY_CLASS_MEDIUM  = 1,
        UI_ACTIVITY_CLASS_LOW     = 2,
        UI_ACTIVITY_CLASS_SIZE    = 3,
    };

	enum
	{
        SPEC_ACT_LIST_TYPE_ALL = PAGE_NEXT, 
        SPEC_ACT_LIST_TYPE_PROG,
        SPEC_ACT_LIST_TYPE_COMP,
        SPEC_ACT_LIST_TYPE_NOTCOMP,        
        
        SPEC_ACT_LIST_SELECT,
        SPEC_ACT_QUESTION_BUTTON,

        SPEC_ACT_SALESITEM_BUTTON_LOW,
        SPEC_ACT_SALESITEM_BUTTON_MEDIUM,
        SPEC_ACT_SALESITEM_BUTTON_HIGH,

        //  Á© ¹Ø¿¡ µÎÀÚ °è»êÇÏ±â ±ÍÂú´Ù.
        SPEC_ACT_REPORT_GRADE, 

        SPEC_ACT_REPORT_TYPE            = SPEC_ACT_REPORT_GRADE + UI_ACTIVITY_CLASS_SIZE,
        SPEC_ACT_SALESITEM_BUTTON_BEGIN = SPEC_ACT_SALESITEM_BUTTON_LOW,
	};


public:
	CActivityWindowPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CActivityWindowPage();

public:
    CBasicScrollBarEx*      m_pReportListScroll;
    ActivityGrade*          m_pSpecActGrade[UI_ACTIVITY_CLASS_SIZE];
    CBasicTextBox*          m_pSpecActType[UI_ACTIVITY_CLASS_SIZE*UI_SPEC_ACT_GATEGORY_SIZE];

    ActivityMainPage*       m_pSpecActMainPage;

    CCheckBox*              m_pProgressDisplay;
    CTipBox*    			m_pSelect;

    CBasicTextBox*      m_pActivityCountBox[UI_ACTIVITY_CLASS_SIZE];
    CBasicTextBox*      m_pActivityPointBox[UI_ACTIVITY_CLASS_SIZE];
    CBasicButton*       m_pActivitySalesItem[UI_ACTIVITY_CLASS_SIZE];
    
    int     m_nReportScrollPos;
    int     m_nReportSelect;

    int     m_nMainType;
    int     m_nSubType;

    static const int nMaxReportSpecAct;

protected:
    CInnerInterface* m_pInterface;
    GLGaeaClient*    m_pGaeaClient;


public:
	void	CreateSubControl ();
    CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID = NO_ID );
    
    void UpdateActivity();
    void UpdateActivityPoint();
    void UpdateReportPosControl();
    void ReCalcReportTotalLine();


    void SetLoadPage( int nMainType, int nSubType );

    ENUM_ACTIVITY_CLASS    GetClass( int nMainType );
    ENUM_ACTIVITY_CATEGORY GetCateGory ( UI_SPEC_ACT_GATEGORY emType );

public:
    const bool   IsProgressDisplay();
    void         SetProgressDisplay( const bool bEnable );

public:
    virtual void BeginSubControl ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};
