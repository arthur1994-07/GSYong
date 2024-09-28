#pragma	once

#include "../../../EngineLib/GUInterface/UIWindow.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../InnerInterface.h"
#include <vector>

class CInnerInterface;
class CBasicButton;
class CBasicTextBox;
class CBasicVarTextBox;
class GLClub;
class GLGaeaClient;

class CPtoPWindow : public CUIWindow
{
private:
	enum
	{
		PTOPMENU_TRADE = ET_CONTROL_NEXT,
		PTOPMENU_WHISPER,
		PTOPMENU_PARTY,		
		PTOPMENU_CONFLICT,
		PTOPMENU_FRIEND,
		PTOPMENU_GUILD,
		PTOPMENU_UNION,
		PTOPMENU_GUILD_BATTLE,
		PTOPMENU_UNION_BATTLE,
        PTOPMENU_BIKE_BUTTON,
        PTOPMENU_FACEBOOK_BUTTON, // bjju.sns
        PTOPMENU_TWITTER_BUTTON,  // bjju.sns
		PTOPMENU_REFER_CHAR,
        PTOPMENU_GROUPCHAT_BUTTON,
	};

public:
    static const D3DCOLOR cCOLOR_ENABLE		= D3DCOLOR_ARGB(255, 255, 255, 255);
    static const D3DCOLOR cCOLOR_DISABLE	= D3DCOLOR_ARGB(150, 150, 150, 150);
    static const DWORD	  dwBUTTON_SIZE_X	= 35;

public:
	CPtoPWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPtoPWindow();

public:
	void	CreateSubControl();
	void	SetPtoPMenu( std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub );
	DWORD	GetPlayerID()	{ return m_dwGaeaID; }
	void	SetPlayerID(DWORD dwGaeaID) { m_dwGaeaID = dwGaeaID; }
    void    showGroupChat();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual void SetVisibleSingle(BOOL visible);

protected:
    void	ResizeControl( char* szWindowKeyword );
    void	SetEnableButton( const bool bEnable, CBasicButton* pButton );

private:
	CUIControl*	CreateControl( const char* szControl );
	CBasicButton*	CreateFlipButton( char* szButton, char* szButtonFlip, char* szButtonOver, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

	void	AddInfoNoSplit( CString strText, D3DCOLOR dwColor );
	void	RemoveAllInfo();

private:
	DWORD	m_dwGaeaID;
	CString	m_szPlayerName;
	float   m_CharacterHeight;

	BOOL	m_bSetThisFrame;
	int		m_nOverMenu;

    DWORD	m_dwDefaultButtonCount;

	CBasicTextBox*	m_pNameBox;
	CBasicVarTextBox*	m_pInfo;

    CBasicButton*	m_pTradeButton;
    CBasicButton*	m_pWhisperButton;
    CBasicButton*	m_pPartyButton;	
    CBasicButton*	m_pConflictButton;
    CBasicButton*	m_pFriendButton;

	CBasicButton*	m_pGuildButton;
	CBasicButton*	m_pUnionButton;
	CBasicButton*	m_pGuildBattleButton;
	CBasicButton*	m_pUnionBattleButton;
	
	CBasicButton*	m_pBikeButton;
	CUIControl*		m_pBikeButtonPosPrev;
	CUIControl*		m_pBikeButtonPosNext;

    CBasicButton*	m_pFaceBookButton;
    CBasicButton*	m_pTwitterButton;

	CBasicButton* m_pReferCharButton;

    CBasicButton* m_pGroupChatButton;
    
protected:
    CInnerInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;

};

class MyPtoPWindow : public IPtoPWindow, private CPtoPWindow
{
public:
    MyPtoPWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~MyPtoPWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership() override;

    virtual const UIRECT& GetUIWindowGlobalPos() override;
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos ) override;
    virtual void SetPtoPMenu( std::tr1::shared_ptr<GLCharClient> pCharClient, DWORD dwGaeaID, GLClubClient* pMyClub ) override;
    virtual DWORD GetPlayerID();
	virtual void  SetPlayerID(DWORD dwGaeaID);
    virtual void showGroupChat();
};
