#pragma	once

#include "../Util/UIWindowEx.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;
class	GLGaeaClient;
class   RnButton;
class   CBasicLineBoxSmart;


//그룹채팅 원도우.
//그룹채팅 원도우 최소화 창입니다.
//방장이름 닫기 버튼을 표시 합니다.
class	CMinChatWindow: public  CUIWindowEx
{
public:
    enum MIN_GROUP_CHAT
    {
        MIN_GROUP_CHAT_EXIT_BUTTON = ET_CONTROL_NEXT,
        MIN_GROUP_CHAT_TEXTBOX,
        MIN_GROUP_CHAT_MAX,
    };

	enum ROOM_STATE
	{
		ROOM_STATE_NOMAL,
		ROOM_STATE_OPEN,
		ROOM_STATE_MSG,
	};


public:
    bool bButtonState;
    bool bMouseIn;
	DWORD m_dwKey;
public:
    CMinChatWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CMinChatWindow ();

public:
    void    SetMinChatRoomName( CString strRoomName );
    CString GetMinChatRoomName ();
    void    SetBackGroundColor();
    void    SetOutLineColor();
	void	SetBoolNewMsg(bool bFlag);
    BOOL    IsMouseIn();
    

public:
    void	CreateMinChatRoom( CString strRoomName );
	void	SetRoomState( ROOM_STATE emState );


public:
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void    TranslateUIMessage ( UIGUID cID, DWORD dwMsg );


private:
    CBasicTextBox*		m_pMinChatRoomNameBox;
    CBasicButton*		m_pMinChatRoomExitButton;
    CBasicLineBoxSmart* m_pUpTextBoxLine;
	CUIControl*			m_pMinChatRoomBG_NOMAL;
	CUIControl*			m_pMinChatRoomBG_OPEN;
	CUIControl*			m_pMinChatRoomBG_MSG;

protected:
    GLGaeaClient* m_pGaeaClient;

};
