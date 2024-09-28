#pragma	once

#include "../../../EngineLib/GUInterface/UIWindow.h"
#include "UIGroupHelper.h"
#include "MessageBox.h"

class	CInnerInterface;
class	CBasicButton;
class	CBasicTextBox;

class CUIWindowEx : public CUIWindow , public CUIGroupUtil
{
public:
	enum
	{
		ID_MESSAGEBOX = CUIWindow::ET_CONTROL_NEXT + 1,
		ID_NEXT = ID_MESSAGEBOX +  1,
	};

public:
	CUIWindowEx(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CUIWindowEx();

	void                CreateMessageBox();
	void                OpenMessageBox( const std::string& strTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0  );
	void                OpenMessageBox( const CMessageBox::BOXTITLE eBoxTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0 );

	void                OpenMessageBoxMultiLine( const CMessageBox::BOXTITLE eBoxTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0 );
	void                OpenMessageBoxMultiLine( const std::string& strTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0 );

	virtual	void        SetVisibleSingle( BOOL bVisible );
	void                CloseMessageBox();

public:
	virtual void		Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg ){};

	BOOL                IsOpenMessageBox();
	BOOL                IsOnMouse();
	std::string         GetInputString();

public:
	void	CreateBaseWindowBlack ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowBlackNoClose ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowMaxGroupChat ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowLightGray ( char* szWindowKeyword,const char* szWindowName );
    void	CreateBaseWindowLightGrayNoClose ( char* szWindowKeyword,const char* szWindowName );    
	void	CreateBaseWindowMiniParty ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowLargeMap ( char* szWindowKeyword, char* szWindowName );
	void	CreateBaseWindowBody	( char* szWindowKeyword ); // by luxes.
	void	CreateBaseWindowBodyLightGray ( char* szWindowKeyword );
    void	CreateBaseWindowBodyLightGrayNoTitle ( char* szWindowKeyword );
    void	CreateBaseWindowEmpty	( char* szWindowKeyword );

protected:
//	CUIControl*		CreateControl ( const char* szControl, const UIGUID& cID = NO_ID );
//	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType );
//	CBasicTextBox*	CreateStaticControl ( const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

protected:
	void	ResizeControl ( char* szWindowKeyword );

	virtual CUIGroup* GetUIGroupThis() { return this; }
	virtual EngineDeviceMan* GetEngineDeviceMan() { return m_pEngineDevice; }	

protected:
	CInnerInterface* m_pInterface;

private:
	CMessageBox*        m_pMessageBox;
	bool                bOnMouse;
};
