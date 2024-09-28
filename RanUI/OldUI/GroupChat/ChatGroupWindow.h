#pragma	once

#include "../Util/UIWindowEx.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;
class	GLGaeaClient;
class   CMinChatWindow;

// 설정 파일로 이동 할것
//const int MAX_GROUP_CHAT_ROOM_NUM = 10;
const int CHAT_ROOM_HEIGHT = 25;

class	CChatGroupWindow: public  CUIWindowEx
{
private:
    DWORD m_RoomCount;
	DWORD m_dwMaxRoomIndex;

public:
    CChatGroupWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CChatGroupWindow ();

public:
    void	CreateSubControl ();
    void    CreateMinChatWindow ( int nRoomIndex, const char* szControlKeyword, UIGUID WndID);
    void    ActiveChatRoom (int nRoomNumber, const char* szRoomName);
	void	MinChatWindowStateUpdate();


public:
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void    TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	int GetRoomIndex( DWORD dwRoomkey);

private:

	std::vector<CMinChatWindow*>     m_pMinChatWindow;


protected:
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
    EngineDeviceMan*    m_pEngineDevice;

};
