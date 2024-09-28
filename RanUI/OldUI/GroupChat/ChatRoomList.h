#pragma	once

#include "../Util/UIWindowEx.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;
class	GLGaeaClient;


class	CChatRoomList: public  CUIWindowEx
{
private:
    enum
    {
        GROUP_CHAT_ROOM_LIST = ET_CONTROL_NEXT,
    };
    int m_RoomCount;

public:
    CChatRoomList ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CChatRoomList ();

public:
    void	CreateSubControl ();
    void    CreateMinChatWindow ( int nRoomIndex, const char* szControlKeyword, UIGUID WndID, const char* szRoomName);


public:
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void    TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

private:
    CBasicTextBoxEx*	m_pRoomList;
 
protected:
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
    EngineDeviceMan*    m_pEngineDevice;

};
