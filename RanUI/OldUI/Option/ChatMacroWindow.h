#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CD3DFontPar;
class CUIEditBox;

#define CHAT_MACRO_SIZE	10
#define CHATMACRO_EDIT_LIMIT 40


class CChatMacroWindow : public CUIWindowEx
{
protected:
	enum
	{
		CHAT_MACRO_EDIT0 = ET_CONTROL_NEXT + 1,
		CHAT_MACRO_EDIT1 ,
		CHAT_MACRO_EDIT2 ,
		CHAT_MACRO_EDIT3 ,
		CHAT_MACRO_EDIT4 ,
		CHAT_MACRO_EDIT5 ,
		CHAT_MACRO_EDIT6 ,
		CHAT_MACRO_EDIT7 ,
		CHAT_MACRO_EDIT8 ,
		CHAT_MACRO_EDIT9 ,
		CHAT_MACRO_OK
	};
public:
	CChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CChatMacroWindow();

public:
	void	CreateSubControl();
	void	EDIT_END(int num);
	void	EDIT_BEGIN(int num, const CString& strName);

	void SetChatMacro();
	void GetChatMacro();
	void SetDefaultMacro();

public:
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	CD3DFontPar*	m_pFont9;
	CUIEditBox*	m_pEditBox[CHAT_MACRO_SIZE];
	
	CString m_ChatMacro[CHAT_MACRO_SIZE];

};

class MyChatMacroWindow : public IChatMacroWindow, private CChatMacroWindow
{
public:
    MyChatMacroWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    ~MyChatMacroWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void GetChatMacro();
};