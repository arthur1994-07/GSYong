#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Interface/UIEditBoxLink.h"
#include "../Interface/BasicTextBoxExLink.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicButton;
class	GLGaeaClient;
class   RnButton;
class	CBasicLineBoxSmart;


class	ChatPopUpWindowMenu: public  CUIWindowEx

{
	enum 
	{
		ID_MENUBOX = ET_CONTROL_NEXT,
		ID_LINEBOX,

	};


public:
	ChatPopUpWindowMenu ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~ChatPopUpWindowMenu ();

public:
	void	CreatePopMenuSub(CString strRoomName,CString strButtonPos);


public:
	virtual	void	Update (int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
	virtual	void	TranslateUIMessage (UIGUID cID, DWORD dwMsg);

private:

protected:
	GLGaeaClient*		m_pGaeaClient;
	CInnerInterface*    m_pInterface;

	CBasicLineBoxSmart*	m_pPopBGMemberLine;
	CBasicTextBoxEx*	m_pMenu;
	CUIControl*			m_pPopMenuBG;
};
