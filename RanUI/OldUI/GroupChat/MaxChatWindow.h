#pragma	once
#include "../Util/UIWindowEx.h"
#include "../Interface/UIEditBoxLink.h"
#include "../Interface/DynamicBasicTextBoxExLink.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;
class	GLGaeaClient;
class   RnButton;
class   CChatPopUpWindow;
class   CBasicLineBoxSmart;
class	CPostBoxPage;
class	CPostBoxWindowNaked;
class	CPostBoxWindowObject;
class	CPostBoxFriendFindPage;
class	CBasicVarTextBox;

class   CChatRoomList;


//그룹 채팅 윈도우
//그룹 채팅 원도우 UI중 최대화창 입니다.
class	CMaxChatWindow: public  CUIWindowEx

{
public:
	enum
	{
		SLOT_MAIN =  0,
		SLOT_MAX  =  3,
		SLOT_NULL = -1,
	};

    enum MAX_GROUP_CHAT
    {
        BUS_MAP_LIST = ET_CONTROL_NEXT,
        MAX_GROUP_CHAT_ROOM_NAME_SCROLLBAR,
        BUS_STOP_LIST,
        MAX_GROUP_CHAT_USERS_TALK_SCROLLBAR,
        BUS_MOVEBUTTON,

        MAX_GROUP_CHAT_ROOM_NAME_TEXT_BOX,
        MAX_GROUP_CHAT_USERS_NAME_TEXT_BOX,
        MAX_GROUP_CHAT_USERS_TALK_TEXT_BOX,
        MAX_GROUP_CHAT_TALK_INPUT_EDIT_BOX,
        MAX_GROUP_CHAT_INVITE_BUTTON,
        MAX_GROUP_CHAT_RESIZE_BUTTON,
        POP_UP_WINDOW,
		POSTBOX_PAGE_FRIENDFIND,
		POSTBOX_WINDOW_NAKED,
    };
private:
    BOOL        m_bFlag;
    BOOL		m_bFirstGap;
    D3DXVECTOR2	m_vGapOld;
    D3DXVECTOR2	m_vGapNew;
    D3DXVECTOR2 m_vOldMousePos;
    UIRECT      m_vOldUIPos;




    int			m_MousePosX;
    int			m_MousePosY;
public:
	DWORD		m_dwKey; //방아이디
    CMaxChatWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, LPDIRECT3DDEVICEQ pd3dDevice );
    virtual ~CMaxChatWindow ();

public:
    void	CreateSubControl ();	



public:
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
    void            TranslateMegThumb( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl );
    void            ScrollMeg( UIGUID cID, DWORD dwMsg, CUIControl * pUIControl );
    void            EDIT_BEGIN ( const CString& strMessage = "" );
    void            ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString& strLinkName, DWORD dwColor, const SITEMCUSTOM *pItemCustom );
    void            SetMaxChatWindoWTitel( CString strRoomName );
	CChatPopUpWindow* GetChatPopUpWindow();
	CBasicTextBoxExLink* GetTalkBox();
	void			SetRoomKey ( DWORD dwkey ) { m_dwKey = dwkey; };
	DWORD			GetRoomKey () { return m_dwKey; };
	BOOL			IsBeginEdit() { return m_pEditBox->IsBegin(); };
	BOOL			IsRoomFree() { return m_bRoomFree; };
	void			SetRoomFree( BOOL bstate ) { m_bRoomFree = bstate; };
	void			WindowPageOpenSlot( CPostBoxWindowObject* pWindow, CPostBoxPage* pPage, const DWORD dwSlot );
	void			StrTagChange(std::string strTag/*or strItemname*/, std::string strItemname/*or strTag*/, std::string &strChat);
	void			SetScroolBarUpDate(CBasicTextBoxExLink* pBasicTextBoxExLink);
	void			AddStringToNORMAL ( CString strTemp, D3DCOLOR dwColor, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM = false );
	void			GetSeparateCharName( const CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData, std::string& strOutType, std::string& strOutName, std::string& strOutContent );
	void			SetNewMsgState( BOOL bState );
	BOOL			ISNewMsgState();
	void			SetRoomMemberCount( DWORD dwCount ) { m_dwRoomMemberCount = dwCount; };
	void			EditBoxControlCopy();
	void			EditBoxControlPaste();

private:
    int		m_nSelectMap;
    int		m_nSelectStop;
	CString m_strRoomName;
	int		m_nNameListIndex;

private:
    CBasicTextBox*		m_pTitle;
    CBasicTextBox*		m_pStopTitle;
    CBasicTextBox*      m_pTalkInputBox;

	CUIControl*			m_pNameListBack;
    CBasicTextBoxExLink*	m_pNameList;
    CBasicScrollBarEx*  m_pMapListScrollBar;

	CUIControl*			m_pTalkListBoxBack;
    CBasicTextBoxExLink*	m_pTalkList;
    CBasicScrollBarEx*	m_pStopListScrollBar;

	CBasicLineBoxSmart* m_pEditBoxLine;
	CUIControl*			m_pEditBoxBack_BEGINEDIT;
	CUIControl*			m_pEditBoxBack_ENDEDIT;
    CUIEditBoxLink*		m_pEditBox;

    CBasicButton*		m_pMoveButton;
    RnButton*           m_pInviteButton;
    CBasicButton*       m_pMaxChatWindowSizeSetButton;

	CBasicVarTextBox*	m_pInfo;

    CBasicLineBoxSmart* m_pUpTextBoxLine;

    BOOL m_link;
	BOOL m_bNewMsg;
    DWORD m_dwColor;
	DWORD m_dwRoomMemberCount; // 현재 방인원
	DWORD m_nMaxRoomMemberCount; // 총방인원

public:
	int m_nTempNum;
	BOOL m_bRoomFree;
	bool m_bShowCommandPopup;

	//팝업메뉴 함수 변수
private:
	CString m_strCharName;
	DWORD m_dwTargetDbNum;
	void InfoMenu( int ClickIndex  = -1 );
	void FriendRegisterMenu( int ClickIndex  = -1 );
	void BlockRegisterMenu( int ClickIndex  = -1 );
	void BLockReleaseMenu( int ClickIndex  = -1 );
	void InviteMenu( int ClickIndex  = -1 );
	void GetoutRoomMenu( int ClickIndex  = -1 );



protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface*    m_pInterface;
	LPDIRECT3DDEVICEQ m_pd3dDevice; 

};
