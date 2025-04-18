#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

//	사용자 메시지 정의
const DWORD UIMSG_EDIT_END = UIMSG_USER1;
const DWORD UIMSG_ADDNAME_TO_CHATBOX = UIMSG_USER2;

const DWORD UIMSG_RECEIVE_NOTE_VISIBLE = UIMSG_USER3;
const DWORD UIMSG_WRITE_NOTE_VISIBLE = UIMSG_USER4;

class	CInnerInterface;
class	CFriendWindowNormalPage;
class	CFriendWindowBlockPage;
class	CInBoxWindow;
class	CSentWindow;
class	CBasicTextButton;
class	GLGaeaClient;
struct	SFRIEND;

class	CFriendWindow : public CUIWindowEx
{
	enum
	{
		FRIEND_NORMAL_TAB_BUTTON = ET_CONTROL_NEXT,
		FRIEND_BLOCK_TAB_BUTTON,
		RECEIVE_TAB_BUTTON,
		SEND_TAB_BUTTON,

		FRIEND_NORMAL_PAGE,
		FRIEND_BLOCK_PAGE,
		FRIEND_INBOX_PAGE,
		FRIEND_SENT_PAGE,
	};

public:
	CFriendWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CFriendWindow();

public:
	void	CreateSubControl();

public:
	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	void	LoadFriendList();

	void	ADD_FRIEND_NAME_TO_EDITBOX( const CString& strName );
	void	ADD_FRIEND( const CString& strName );

	const CString& GET_FRIEND_NAME() const;
	const CString& GET_BLOCK_NAME() const;

public:
	void	EDIT_END();

private:
	CBasicTextButton*	CreateTextButton19( char* szButton, UIGUID ControlID , char* szText );	

private:
	void	ChangePage( const UIGUID& cSetVisiblePage );

public:
	virtual	void	TranslateUIMessage( UIGUID cID, DWORD dwMsg );

private:
	CBasicTextButton*	m_pNORMAL_TAB_BUTTON;
	CBasicTextButton*	m_pBLOCK_TAB_BUTTON;
	CBasicTextButton*	m_pRECEIVE_TAB_BUTTON;
	CBasicTextButton*	m_pSEND_TAB_BUTTON;

	CFriendWindowBlockPage*		m_pBlockPage;
	CFriendWindowNormalPage*	m_pNormalPage;
	CInBoxWindow*				m_pInBoxPage;	
	CSentWindow*				m_pSentPage;

protected:
	GLGaeaClient*				m_pGaeaClient;

public:
	std::tr1::shared_ptr<SFRIEND> GetFriendSMSInfo();

};

class MyFriendWindow : public IFriendWindow, private CFriendWindow
{
public:
    MyFriendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyFriendWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void AddFriendName_To_EditBox( const UI::String& strName );
    virtual void AddFriend( const UI::String& strName );
    virtual void LoadFriendList();
    virtual UI::String GetFriendName() const;
    virtual UI::String GetSelectBlockFriendName() const;
    virtual void EndEditFriendList();
    virtual std::tr1::shared_ptr<SFRIEND> GetFriendSMSInfo();

    virtual void AddFriendBlock( const std::string& strName, bool bBlock ){}
};