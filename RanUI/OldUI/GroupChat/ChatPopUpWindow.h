#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Interface/UIEditBoxLink.h"
#include "../Interface/BasicTextBoxExLink.h"

class	ChatPopUpWindowMenu;  
class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicButton;
class	GLGaeaClient;
class   RnButton;
class	CBasicLineBoxSmart;

const int POP_UP_WINDOW_MAX_MENU = 6;
class	CChatPopUpWindow: public  CUIWindowEx

{
public:
    enum EM_MAX_GROUP_CHAT
    {
        POP_UP_WINDOW = ET_CONTROL_NEXT,
        POP_UP_MENU_INFO,
        POP_UP_MENU_FRIEND_REGISTER,
        POP_UP_MENU_BLOCK_REGISTER,
        POP_UP_MENU_BLOCK_RELEASE,
        POP_UP_MENU_INVITE,
        POP_UP_MENU_GET_OUT_ROOM,
		POP_UP_MENU_BG_MEMBER,
		POP_UP_MENU_BG_MASTER,
    };

public:
    CChatPopUpWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~CChatPopUpWindow ();

public:
    void		CreateSubControl ();
	void		CreatePopMenu ( CString strRoomName,const char* strButtonPos, UIGUID nID);
	CUIControl*	CreatePopMenuBG(CString strButtonPos, EM_MAX_GROUP_CHAT enMENU);

    void    ShowPopUpWindow(int nMousePosX, int nMousePosY);
    void    HidePopUpWindow();
	void	SetRoomName(CString srtName);
	void	SetName(CString srtCharmName);
	void	SetDwKey(DWORD dwkey);

public:
    virtual	void	Update (int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
    virtual	void	TranslateUIMessage (UIGUID cID, DWORD dwMsg);

private:

protected:
	CBasicLineBoxSmart*	m_pPopBGMemberLine;
	CBasicLineBoxSmart*	m_pPopBGMasterLine;
	CUIControl*			m_pPopBGMember;
	CUIControl*			m_pPopBGMaster;
    GLGaeaClient*		m_pGaeaClient;
    //RnButton*			m_pPopMenu[POP_UP_WINDOW_MAX_MENU];
	//CBasicTextBoxEx*			m_pPopMenu[POP_UP_WINDOW_MAX_MENU];
	//ChatPopUpWindowMenu* m_pPopMenu[POP_UP_WINDOW_MAX_MENU];
	std::vector<ChatPopUpWindowMenu*> m_pPopMenu;
	CString				m_strName;
	CString				m_strCharName;
	DWORD				m_dwKey;

};
