#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogicClient/Friend/GLFriendClient.h"

class	CInnerInterface;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicTextButton;
class	CUIEditBox;
class	GLGaeaClient;

class	CFriendWindowNormalPage : public CUIGroup
{
private:
static	const int nLIMIT_CHAR;

private:
	enum
	{
		FRIEND_LIST_TEXTBOX = NO_ID + 1,
		FRIEND_LIST_SCROLLBAR,
		FRIEND_LIST_ADDBUTTON,
		FRIEND_LIST_DELBUTTON,
		FRIEND_LIST_BLOCKBUTTON,
		FRIEND_LIST_MOVEBUTTON,
		FRIEND_LIST_PARTYBUTTON,
		//FRIEND_LIST_NOTEBUTTON,
		FRIEND_EDITBOX
	};

	enum
	{
		ONLINE = 0,
		OFFLINE
	};

public:
	CFriendWindowNormalPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CFriendWindowNormalPage ();

public:
	void	CreateSubControl ();
	CBasicTextButton*	CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText );	

public:
	void	LoadFriendList ();
	void	EDIT_END ();
	void	EDIT_BEGIN ( const CString& strName = "" );

public:
	void	ADD_FRIEND ( const CString& strName );
	void	DEL_FRIEND ( const CString& strName );
	void	ADD_BLOCK ( const CString& strName );
	void	MOVETO_FRIEND ( const CString& strName );
	void	PARTY_FRIEND ( const CString& strName );
	const CString& GET_FRIEND_NAME () const						{ return m_strSelectName; }

private:
	BOOL	GetFriend ( const int nCharID, CString & strName );

public:
	void	ADD_FRIEND_NAME_TO_EDITBOX ( const CString& strName );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

private:
	void LoadFriend(std::tr1::shared_ptr<SFRIEND> sFriend);

private:
	int		m_nSelectIndex;
	CString	m_strSelectName;

private:
	CBasicTextBoxEx*	m_pListText;
	CBasicScrollBarEx*	m_pListScrollBar;

	CBasicTextButton*	m_pADD;
	CBasicTextButton*	m_pDEL;
	CBasicTextButton*	m_pBLOCK;
	CBasicTextButton*	m_pMOVE;
	CBasicTextButton*	m_pPARTY;

	//CBasicTextButton*	m_pNote;

	CUIEditBox*			m_pEditBox;

	BOOL				m_bINIT;

	std::tr1::shared_ptr<SFRIEND> m_sFriend;

protected:
	BOOL SetFriendInfo( const int nCharID );

public:
    std::tr1::shared_ptr<SFRIEND> GetFriendInfo() { return m_sFriend; }

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};
