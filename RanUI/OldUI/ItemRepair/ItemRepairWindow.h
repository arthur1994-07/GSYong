#pragma	once
#include "../Util/UIWindowEx.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class CBasicTextBox;
class CItemImage;
struct SITEMCUSTOM;
 
class CBasicScrollBarEx;

class ItemRepairWindow : public CUIWindowEx
{
private:
    enum
    {
       LINE_MAX = 5,
    };
    enum
    {
        NOT_USE = ET_CONTROL_NEXT,
        UI_ID_BTN_REPAIR,
        UI_ID_BTN_REPAIR_ALL,
        UI_ID_LINE_0,
        UI_ID_LINE_1,
        UI_ID_LINE_2,
        UI_ID_LINE_3,
        UI_ID_LINE_4
    };
    class WindowLine : public CUIGroupHelper
    {
    private:
        enum { UI_ID_ICON = 100 };
    public:
        WindowLine( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
        void                CreateSubControl();
        virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );;
        virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

        void                SetData( const SITEMCUSTOM& itemCustom );
        void                ResetData();

    private:
        CInnerInterface*    m_pInterface;
        GLGaeaClient*       m_pGaeaClient;
        CItemImage*	        m_pItemImage;
        CBasicTextBox*      m_pTextLine0;
        CBasicTextBox*      m_pTextLine1_L;
        CBasicTextBox*      m_pTextLine1_R;
    private:
        SITEMCUSTOM         m_sItemCustom;
    };


public:
    ItemRepairWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    ~ItemRepairWindow();
    void                CreateSubControl();
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );;
    virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        Refresh();

    virtual void        ItemRepairCardUseReq( WORD wPosX, WORD wPosY );
protected:
    CInnerInterface*    m_pInterface;
    GLGaeaClient*       m_pGaeaClient;

private:
    ItemRepairWindow::WindowLine* m_Lines[LINE_MAX];
    CBasicScrollBarEx*	m_pScrollBar;
    CBasicTextBox* m_pText1;
    CBasicTextBox* m_pText2;
    CBasicLineBoxSmart*		m_pSeleted;

private:
    void                Repairing();
    void                RepairingAll();
    std::multimap<WORD, EMSLOT> m_mapRepair;
    int m_nScrollCurPos;
    int m_nSeletedLine;

};