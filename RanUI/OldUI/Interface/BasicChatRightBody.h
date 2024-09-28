#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
//#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "UIEditBoxLink.h"
#include "../Interface/ChatType.h"

struct SRecordChatMsg
{
	CTime   recordTime;
	CString strChatMsg;
};

class CInnerInterface;
class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CUIEditBox;
class CBasicButton;
class CSystemMessageDisplay;
class CBasicTextButton;
class CBasicTextBox;
class CBasicTextBoxExLink;
class GLGaeaClient;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_RIGHTBODY = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CBasicChatRightBody : public CUIGroup
{
	static	const int nLIMITCHAR;
	static	const int nSTARTLINE;
	static	const int nLIMITLINE;
	static	const D3DCOLOR dwCLICKCOLOR;

public:
	enum
	{
		CHAT_EDITBOX = NO_ID + 1,
		CHAT_NATIVE_BUTTON,
		CHAT_ENGLISH_BUTTON,

		CHAT_CHANNEL_TOTAL,
		CHAT_CHANNEL_PARTY,
		CHAT_CHANNEL_CLUB,
		CHAT_CHANNEL_ALLIANCE,
		CHAT_CHANNEL_CHANNEL,
		CHAT_CHANNEL_SYSTEM,

		CHAT_TEXTBOX_ALL,
		CHAT_TEXTBOX_PARTY,
		CHAT_TEXTBOX_CLUB,
		CHAT_TEXTBOX_ALLIANCE,
		CHAT_TEXTBOX_CHANNEL,
		CHAT_TEXTBOX_SYSTEM,

		CHAT_TYPE_TEXT
	};

	//	간단한 상태 정의
	enum
	{
		OUT_OF_RANGE = -1,	//	채팅 로그 범위밖(사용하지 않고 있음)
	};

private:
	CUIControl*			m_pEditBoxBack;
	CUIEditBoxLink*		m_pEditBox;
	//	CSystemMessageDisplay* m_pSystemMessageDisplay;

private:	//	전체
	CBasicTextBoxExLink*	m_pTextBox_ALL;
	CBasicScrollBarEx*		m_pScrollBar_ALL;

	CBasicTextBoxExLink*	m_pTextBox_PARTY;
	CBasicScrollBarEx*		m_pScrollBar_PARTY;

	CBasicTextBoxExLink*	m_pTextBox_CLUB;
	CBasicScrollBarEx*		m_pScrollBar_CLUB;

	CBasicTextBoxExLink*	m_pTextBox_ALLIANCE;
	CBasicScrollBarEx*		m_pScrollBar_ALLIANCE;

	CBasicTextBoxExLink*	m_pTextBox_CHANNEL;
	CBasicScrollBarEx*		m_pScrollBar_CHANNEL;

	CBasicTextBoxExLink*	m_pTextBox_SYSTEM;
	CBasicScrollBarEx*		m_pScrollBar_SYSTEM;

	CUIControl*			m_pScrollBarDummy;

	CBasicButton*		m_pNativeButton;
	CBasicButton*		m_pEnglishButton;

	CBasicTextButton*	m_pChatTotalButton;
	CBasicTextButton*	m_pChatPartyButton;
	CBasicTextButton*	m_pChatClubButton;
	CBasicTextButton*	m_pChatAllianceButton;
	CBasicTextButton*	m_pChatChannelButton;
	CBasicTextButton*	m_pChatSystemButton;

	CBasicTextBox*		m_pChatTypeBox;

public:
	CBasicChatRightBody(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicChatRightBody ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ( CD3DFontPar*	pFont );

private:
	void	AddStringToNORMAL ( CString strTemp, D3DCOLOR dwColor, bool bGM = false );
	void	AddStringToNORMAL ( WORD wChatType, CString strTemp, D3DCOLOR dwColor, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM = false );
	void	SAVE_CHATLOG( CString strTemp, WORD wType );
	void	RECORD_CHAT( const CString strChatMsg, WORD wType );
    void    GetSeparateCharName( WORD wChatType, const CString& strText, VEC_LINK_TEXT_DATA& vecLinkTextData, std::string& strOutType, std::string& strOutName, std::string& strOutContent );
    void    GetSeparateCharName( const CString& strText, std::string& strOutType, std::string& strOutName, std::string& strOutContent );

public:
	void	AddChat ( const CString& strName, const CString& strMsg, const WORD& wType );
	void	AddStringToNORMALEx ( CString strTemp, D3DCOLOR dwColor, bool bGM = false );
	void	AddStringToChatEx ( CString strTemp, WORD wType, bool bGM = false );
	
	void	AddStringToChatLink( CString strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM = false );
	
	void	AddStringToSystemMessage ( CString strTemp, D3DCOLOR dwColor );
	void	AddChatMacro(CString strMsg);
	void	ChatLog( bool bChatLog, int nChatLogType );
	CString GET_RECORD_CHAT();

    void	ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString& strLinkName, DWORD dwColor );

private:
	CBasicTextBoxEx*	CreateTextBoxEx ( CD3DFontPar* pFont, UIGUID uID );
	CBasicTextBoxExLink*CreateTextBoxExLink ( CD3DFontPar* pFont, UIGUID uID );
	CBasicScrollBarEx*	CreateScrollBarEx ( const int& nTotalLine, const int& nVisibleLine );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

public:
	void	SET_PRIVATE_NAME ( const CString& strName );
	void	ADD_FRIEND ( const CString& strName ); // 친구이름을 리스트에 추가
	void	DEL_FRIEND ( const CString& strName ); // 친구이름을 리스트에서 삭제
	void	ADD_FRIEND_LIST ();

public:
	void	ADD_CHATLOG ( const CString& strChat ); // 채팅메세지를 리스트에 추가

public:
	void	BEGIN_NORMAL_CHAT ( const CString& strMessage = "" ); 
	void	BEGIN_PRIVATE_CHAT ( const CString& strName = "", const CString strMessage = "" );
	void	BEGIN_PARTY_CHAT ( const CString& strMessage = "" );	
	void	BEGIN_TOALL_CHAT ( const CString& strMessage = "" );
	void	BEGIN_GUILD_CHAT ( const CString& strMessage = "" );
	void	BEGIN_ALLIANCE_CHAT ( const CString& strMessage = "" );
	void	BEGIN_AREA_CHAT ( const CString& strMessage = "" );
	void	BEGIN_PARTY_RECRUIT_CHAT ( const CString& strMessage = "" );
	void	BEGIN_SYSTEM_CHAT ( const CString& strMessage = "" );
	void	BEGIN_FACTION_CHAT ( const CString& strMessage = "" );
	void	CHANGE_CHAT_TYPE( int nChatType );

    void    SendReplyAwayState( const std::string& strOrigin );

public:
	bool	IsEDIT_BEGIN ()									{ return m_pEditBox->IsBegin(); }
	const CString&	GET_EDIT_STRING()	{ return m_pEditBox->GetEditString(); } 

private:
	void	EDIT_BEGIN ( const CString& strMessage = "" );		
	void	EDIT_END ();
	void	EDIT_CLEAR()									{ m_pEditBox->ClearEdit (); }

	void	KEEP_LAST_STATE ();
private:
	void	SELECT_CHAT_PAGE ();
	void	UPDATE_CHAT_PAGE ( const BOOL& bFirstControl );
	void	UPDATE_CHAT_PAGE_TEXTBOX ( CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar );
	void	ADD_CHAT_PAGE_TEXTBOX ( const CString& strTemp, const D3DCOLOR& dwColor, CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar );
	void	ADD_CHAT_PAGE_TEXTBOX_LINK ( const CString& strTemp, const D3DCOLOR& dwColor, CBasicTextBoxExLink* pTextBox, 
										  CBasicScrollBarEx* pScrollBar, VEC_LINK_TEXT_DATA& vecLinkTextData, WORD wChatType, bool bGM  );

private:
	bool	SEND_CHAT_MESSAGE ();
	bool	SEND_CHAT_MESSAGE ( const CString& strOrigin, VEC_LINK_TEXT_DATA_EX vecTextLinkData = VEC_LINK_TEXT_DATA_EX() );
	bool	SEND_COMMON_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData );
	bool	SEND_PRIVATE_MESSAGE( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_PARTY_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_GUILD_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_ALLIANCE_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_AREA_MESSAGE( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_PARTY_RECRUIT_MESSAGE( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_TOALL_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData, bool bGM );
	bool	SEND_SYSTEM_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData );
	bool	SEND_FACTION_MESSAGE ( const CString& strOrigin, const VEC_LINK_TEXT_DATA_EX& vecTextLinkData );

	bool	AUTO_PRIVATE_CHAT( const CString& strData );
	bool	ADD_NAME_TO_POSTBOX ( const CString& strData );
	bool	ReferChar( const CString& strData );

private:
	void	ChangePrivateName ();
	void	ChangeLastChat ();
	void	ChangeChatType ();

	void	UPDATE_SHORTKEY ();

	bool	IS_FRIEND ( CString strName );
    CString	GET_ONLINE_FRIEND ( CString strName );

    bool CheckChatForOverInput( WORD wType, std::string& strChatMsg );

private:
	void	ShowSelfMessageOnHead ( const CString& strOrigin );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	HRESULT	RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SetChatFlagState( WORD wChatFlag )		{ m_wDisplayState = wChatFlag; }
	WORD	GetChatFlagState()						{ return m_wDisplayState; }
	void	AddChatFlagState( WORD wChatFlag )		{ m_wDisplayState |= wChatFlag; }

	void	SetIgnoreState ( bool bIgnoreState )	{ m_bIgnoreState = bIgnoreState; }

	void	CLASSIFY_CHATTYPE ();	// 채팅 단축명령어 확인
	DWORD	CHATTYPE_SIMBOL_CHECK( CString strOrigin );	// 채팅 심볼 체크
	
	DWORD	GetCHATTYPE()							{ return m_dwChatType; }
	void	SetCHATTYPE( DWORD dwChatType );
	int		GetDISPLAYTYPE()						{ return m_wDISPLAYTYPE; }

	void	SetFreezeControl( bool bFreeze )		{ m_bFREEZE = bFreeze; }

	void	SetVisibleChatButton( BOOL bVisible );

private:
	typedef std::list<CString>	NAMELIST;
	typedef NAMELIST::iterator	NAMELIST_ITER;	

	typedef	std::list<CString>	NORMAL_CHAT_LOG;
	typedef	NORMAL_CHAT_LOG::iterator	NORMAL_CHAT_LOG_ITER;

	typedef std::vector<SRecordChatMsg> RECORD_CHAT_LOG;
	typedef RECORD_CHAT_LOG::iterator	RECORD_CHAT_LOG_ITER;

protected: // 메세지 중복 입력(도배) 처리
	FLOAT	m_fLifeTime;
	INT		m_nInputCount;
	INT		m_nTimeCount;
	CString m_strLastMsg;
	BOOL	m_bPapering;

	RECORD_CHAT_LOG m_vecRecordChatMsg;

	VOID SetLifeTime( float fTime )					{ m_fLifeTime = fTime; }
	FLOAT GetLifeTime()								{ return m_fLifeTime; }
	VOID DecreaseTime( float fElapsedTime );			
	BOOL IsPapering( const CString& strOrigin );
	BOOL IsChatBlock();
	BOOL IsThaiCheck( const CString& strOrigin );
	BOOL IsVnCheck( const CString& strOrigin );

    void EditBoxControlCopy();
    void EditBoxControlPaste();

private:
	NORMAL_CHAT_LOG		m_ChatLog;
	int					m_ChatPos;
	int					m_ChatCountBack;

	NAMELIST	m_NameList;
	int			m_NamePos;
	int			m_NameCountBack;

	CString		m_strChatBack;
	CString		m_strPrivateName;	//	현재 귓속말 대상자
	CString		m_strRePrivateName;	//	마지막 귓속말 온 사람

	int			m_nFRIENDSIZE_BACK;

	bool	m_bFREEZE;
	bool	m_bMINIMUM_SIZE_TEXT;
	bool	m_bMINIMUM_SIZE_SCROLL;

	int		m_dwChatType;

	WORD	m_wDisplayState;
	WORD	m_wDISPLAYTYPE;

	bool	m_bChatLog;
	int		m_nChatLogType;

private:
	bool	m_bIgnoreState;

protected:
	CInnerInterface* m_pInterface;

};