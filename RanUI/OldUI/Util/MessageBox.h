#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MESSAGEBOX_POSITIVE = UIMSG_USER1;
const DWORD UIMSG_MESSAGEBOX_NEGATIVE = UIMSG_USER2;

////////////////////////////////////////////////////////////////////
class CBasicLineBoxSmart;
class CBasicTextBox;
class CUIEditBoxMan;
class RnButton;

class CMessageBox : public CUIGroup
{
private:
    enum
    {
        ID_BTN_OK_CENTER = NO_ID + 1,
        ID_BTN_OK,
        ID_BTN_CANCEL,
        ID_BTN_YES,
        ID_BTN_NO,
        ID_TEXT_BOX,
        ID_TEXT_TEXTMAN,
        ID_TEXT_BACKGROUND,
    };
public:
    enum BOXTYPE
    {
        TYPE_OK,
        TYPE_OKCANCEL,
        TYPE_YESNO,
        TYPE_INPUT,
    };
    enum BOXSTYLE
    {
        STYLE_DEFAULT
    };
    enum BOXTITLE
    {
        TITLE_ANNOUNCEMENT = 0,
        TITLE_QUESTION,
        TITLE_WARNING,
        TITLE_ERROR,
        TITLE_CRITICAL,
        TITLE_ENTER,
    };

	CMessageBox( EngineDeviceMan* pEngineDevice );
	virtual void        CreateSubControl( const BOXSTYLE eBoxStyle = STYLE_DEFAULT );
    void                OpenMessageBox( const std::string& strTitle, const std::string& strMessage, const BOXTYPE eBoxType = TYPE_OKCANCEL, unsigned int nIdentity = 0);
    void                OpenMessageBox( const BOXTITLE eBoxTitle, const std::string& strMessage, const BOXTYPE eBoxType = TYPE_OKCANCEL, unsigned int nIdentity = 0 );

    void                OpenMessageBoxMultiLine( const BOXTITLE eBoxTitle, const std::vector<std::string>& vecMessage, const BOXTYPE eBoxType = TYPE_OKCANCEL, unsigned int nIdentity = 0 );
    void                OpenMessageBoxMultiLine( const std::string& strTitle, const std::vector<std::string>& vecMessage, const BOXTYPE eBoxType = TYPE_OKCANCEL, unsigned int nIdentity = 0 );
    unsigned int        GetIdentifier();
    std::string         GetInputString();
    void                CloseMessageBox();

public:
    void                SetTitleText( const std::string strTitle );
    void                SetMessageText( const std::string strMessage );
    void                SetMessageText( const std::vector<std::string>& vecMessage );
    void                SetVisibleSingleTrue( unsigned int nIdentity );    // identifier 식별자
    void                SetVisibleSingleFalse();                             // identifier 식별자 반환

public:
    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    virtual	void        SetVisibleSingle ( BOOL bVisible );
    std::string         GetBoxTitle( const BOXTITLE& eBoxTitle );
    void                SetBoxType( const BOXTYPE& eBoxType );
    unsigned            m_nIdentity;
    CBasicTextBox*      m_pTitle;

    CBasicTextBox*      m_pMessage;

    CUIEditBoxMan*      m_pEditBox;
    CUIControl*         m_pEditBoxBG;

    RnButton*           m_pOk_Center;
    RnButton*           m_pYes;
    RnButton*           m_pNo;
    RnButton*           m_pOk;
    RnButton*           m_pCancel;
    
private:
    bool                m_bIsClick;
    std::string         m_strInputText;
	int					m_nTranslateLoopCount;
};