#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLChatLinkDef.h"
#include "../../InnerInterface.h"


class CInnerInterface;
class CBasicChatLeftBar;
class CBasicChatRightBody;
//class CUIChatShowFlag;
class CBasicButton;
class GLGaeaClient;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE = UIMSG_USER1;
////////////////////////////////////////////////////////////////////
class CBasicChat : public CUIGroup
{
private:
	enum
	{
		LEFT_BAR = NO_ID + 1,
		RIGHT_BODY,
		CHAT_SHOW_FLAG,
		CHAT_FLAG_BUTTON,
	};

public:
	CBasicChat(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicChat();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

public:
	virtual	void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void	AddChat( const CString& strName, const CString& strMsg, const WORD& wType );
	void	AddStringToNormal( CString strTemp, D3DCOLOR dwColor );
	void	AddStringToChatEx( CString strTemp, WORD wType, bool bGM = false );
	void	AddStringToChatLink( CString strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM = false );
	void	AddStringToSystemMessage( CString strTemp, D3DCOLOR dwColor );
	void	AddChatMacro(CString strMsg );  // 매크로 형
	void	ChatLog( bool bChatLog, int nChatLogType );
	CString GET_RECORD_CHAT();

    void	SetChatOption(WORD ChatFlag);
    void    SetChatType(DWORD ChatType);

public:
	void	CHANGE_CHAT_TYPE();
	void	BEGIN_NORMAL_CHAT( const CString& strMessage = "" ); 
	void	BEGIN_PRIVATE_CHAT( const CString& strName = "", const CString& strMessage = "" );
	void	BEGIN_PARTY_CHAT( const CString& strMessage = "" );
	void	BEGIN_TOALL_CHAT( const CString& strMessage = "" );
	void	BEGIN_GUILD_CHAT( const CString& strMessage = "" );
	void	BEGIN_ALLIANCE_CHAT( const CString& strMessage = "" );
	void	BEGIN_AREA_CHAT( const CString& strMessage = "" );
	void	BEGIN_PARTY_RECRUIT_CHAT( const CString& strMessage = "" );	
	void	BEGIN_FACTION_CHAT( const CString& strMessage = "" );

	bool	IsCHAT_BEGIN();

	void	SET_PRIVATE_NAME( const CString& strName );

	void	ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString strLinkName, DWORD dwColor );

    void    SendReplyAwayState( const std::string& strPrivateName, const std::string& strMsg );

public:
    const UIRECT& GetChangStateButtonGlobalPos ();

public:
	void	REFRESH_FRIEND_LIST();
	void	FRIEND_LIST( CONST CString & strName, bool bOnline );

private:
	CBasicChatLeftBar*		m_pBasicChatLeftBar;
	CBasicChatRightBody*	m_pBasicChatRightBody;
private:
	BOOL		m_bFirstGap;
	D3DXVECTOR2	m_vGap;

	int			m_PosX;
	int			m_PosY;

private:

	//	좌측 채팅옵션 버튼
	CBasicButton*	m_pChatShowFlagButton;

protected:
	CInnerInterface* m_pInterface;

};

class MyBasicChat : public IBasicChatWindow, private CBasicChat
{
public:
    MyBasicChat( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyBasicChat()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void AddChatMacro( UI::String strMsg );
    virtual void AddStringToSystemMessage( UI::String strTemp, D3DCOLOR dwColor );
    virtual void AddStringToChatLink( UI::String strTemp, WORD wType, VEC_LINK_TEXT_DATA& vecLinkTextData, bool bGM = false );
    virtual void AddStringToChatEx( UI::String strTemp, WORD wType, bool bGM = false );
	virtual void AddStringToNomal( UI::String strTemp, D3DCOLOR dwColor );
    virtual void RefreshFriendList();
    virtual void ChatLog( bool bChatLog, int nChatLogType );
    virtual UI::String GetRecordChat();
    virtual void SetFriendOnlineList( CONST UI::String & strName, bool bOnline );
    virtual bool IsChatBegin();
    virtual void SetChatOption(WORD ChatFlag);
    virtual void SetChatType(DWORD ChatType);
    virtual void ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const UI::String strLinkName, DWORD dwColor );
    virtual void BeginPrivateChat( const UI::String& strName = "", const UI::String& strMessage = "" );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual const UIRECT& GetUIChangStateButtonGlobalPos();
    virtual void SetPrivateMsgCharName( const UI::String& strName );
    virtual void SetNoUpdate( BOOL bNoUpdate );
    virtual void SendReplyAwayState( const std::string& strPrivateName, const std::string& strMsg );
};