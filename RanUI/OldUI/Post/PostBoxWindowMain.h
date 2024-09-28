#pragma	once

#include "PostBoxWindowObject.h"
#include "../Util/TapSelector.h"

class CInnerInterface;
class CBasicTextButton;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_POSTBOX_WINDOW_MAIN_TAP_CHANGE = UIMSG_POSTBOX_WINDOW_OBJECT_NEXT;
////////////////////////////////////////////////////////////////////

class CPostBoxWindowMain : public CPostBoxWindowObject, public CTapSelector
{
protected:
	enum
	{
		POSTBOX_WINDOW_MAIN_NONE = POSTBOX_WINDOW_OBJECT_NEXT,

		POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE,
		POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND,
		POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY,
		POSTBOX_WINDOW_MAIN_BUTTON_CLOSE,
	};

public:
	enum
	{
		TAP_RECEIVE = POSTBOX_WINDOW_MAIN_BUTTON_TAP_RECEIVE,
		TAP_SEND    = POSTBOX_WINDOW_MAIN_BUTTON_TAP_SEND,
		TAP_HISTORY = POSTBOX_WINDOW_MAIN_BUTTON_TAP_HISTORY,

		TAP_DEFAULT = TAP_RECEIVE,
	};

public:
	CPostBoxWindowMain ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CPostBoxWindowMain ();

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void		CreateSubControlEx ();
	
private:
	CBasicTextButton*	m_pButton_Close;

};
