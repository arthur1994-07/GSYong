#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../SigmaCore/String/MinGuid.h"

class CMessageBox;

class CBasicLineBoxSmart;
class CBasicScrollBarEx;
class NotifyClientMan;
class INotifyClientBase;
enum NOTIFY_TYPE;

class CNotifyWindow : public CUIGroup
{
private:
    typedef std::vector<sc::MGUID> MESSAGE_VECTOR;

    enum eIDENTI
    {
        //nIdentity : MessageBox를 식별하기 위해서 사용한다.
        IDENTI_NOT_USE = 0,
        IDENTI_ADD_FRIEND,
        IDENTI_GROUPCHAR_INVITE,
        IDENTI_CLUB_INVITE,
        IDENTI_PARTY_INVITE,
    };

    enum
    {
        LineTextControlCount = 7,
        NewMessageAlarmTime = 5000,
    };

    enum
    {
        ID_NOT_USE = NO_ID,
        ID_MESSAGEBOX,
        ID_WINDOW_BG,
        ID_WINDOW_TITLE,
        ID_SCROOLBAR,

        ID_TEXTBOX = 100,
        ID_TEXTBOX_START = ID_TEXTBOX,
        ID_TEXTBOX_END = ID_TEXTBOX + LineTextControlCount,

    };
public:
    CNotifyWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    void                CreateSubControl();
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    void                TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
    virtual void        SetVisibleSingle( BOOL bVisible );
    virtual void        NewMessage();
    virtual void        Refresh();
    
protected:
    CInnerInterface*    m_pInterface;
    GLGaeaClient*       m_pGaeaClient;
    NotifyClientMan*    m_pNotiftMan;

private: 
    CBasicTextBox*      m_pTitleText;
    CBasicLineBoxSmart* m_pTitleBackGround;
    CBasicLineBoxSmart* m_pBackGroundA;
    CBasicLineBoxSmart* m_pBackGroundB;

    CMessageBox*        m_pMessageBox;
    
    CBasicTextBox*      m_pLineTextControl[LineTextControlCount];
    CBasicLineBoxSmart* m_pLineBoxSeleted;
    CBasicScrollBarEx*  m_pScrollBar;
    unsigned int        m_nViewPosOld;

private:
    void                SetVisibleAllControl( BOOL bVisible );
    void                GetData();
    void                SetTextBoxList( unsigned int nViewPos );
    void                DeleteNotifyMessage();

    MESSAGE_VECTOR      m_vecNotifyMessage;    
    int                 m_onMouse;
    //unsigned int        m_nSelected;
    std::tr1::shared_ptr<INotifyClientBase> m_spSeleted;

    float               m_fNewMessageTime;

};