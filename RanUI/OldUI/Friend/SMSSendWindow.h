#pragma once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextBox;
class CUIMultiEditBox;
class GLGaeaClient;

class CSMSSendWindow : public CUIWindowEx
{
	static const int nLIMIT_CHAR;

	enum
	{
		SMS_SEND_EDITBOX_L01 = ET_CONTROL_NEXT,
		SMS_SEND_EDITBOX_L02,

		SMS_SEND_BUTTON,
		SMS_CANCEL_BUTTON
	};

public:
	CSMSSendWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CSMSSendWindow(void);

public:
	void CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void EDIT_END();
	void EDIT_BEGIN();

protected:
	CBasicTextBox * m_pNameTextBox;
	CUIMultiEditBox * m_pMultiEditBox;

protected:
	 GLGaeaClient* m_pGaeaClient;

};