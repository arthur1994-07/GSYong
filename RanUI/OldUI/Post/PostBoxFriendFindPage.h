
#pragma	once

#include "PostBoxPage.h"
#include "../Util/TapSelector.h"

#include "../../../EngineLib/G-Logic/GLDefine.h"

#include "../../../RanLogicClient/Friend/GLFriendClient.h"

/*
class CBasicLineBox;
class CBasicLineBoxSmart;
class CBasicButton;
class CBasicTextBox;
class CBasicTextButton;
class CBasicComboBoxRollOver;
class CUIEditBox;
class CUIMultiEditBox;
class CItemImage;
class CSwapImage;
*/

class CBasicTextButton;
class CSmartComboBoxRollOver;

class CPostBoxFriendFindPage : public CPostBoxPage, public CTapSelector
{

protected:
	enum
	{
		POSTBOX_PAGE_FRIENDFIND = POSTBOX_PAGE_NEXT,

		POSTBOX_PAGE_FRIENDFIND_TAP_FRIEND,
		POSTBOX_PAGE_FRIENDFIND_TAP_CLUB,

		POSTBOX_PAGE_FRIENDFIND_BUTTON_OK,
		POSTBOX_PAGE_FRIENDFIND_BUTTON_CLOSE,

		POSTBOX_PAGE_FRIENDFIND_COMBOBOX_FRIEND,
		POSTBOX_PAGE_FRIENDFIND_COMBOBOX_CLUB,
	};

public:
	enum
	{
		TAP_FRIEND = POSTBOX_PAGE_FRIENDFIND_TAP_FRIEND,
		TAP_CLUB   = POSTBOX_PAGE_FRIENDFIND_TAP_CLUB,

		TAP_DEFAULT = TAP_FRIEND,
	};

	enum
	{
		DATA_DEFAULT = 0,
		DATA_OFFLINE = 1,
	};

public:
	static const D3DCOLOR cCOLOR_TEXT_DEFAULT	= D3DCOLOR_ARGB(255, 255, 255, 255);
	static const D3DCOLOR cCOLOR_TEXT_OFFLINE	= D3DCOLOR_ARGB(255, 150, 150, 150);
	static const D3DCOLOR cCOLOR_TEXT_SELECT	= D3DCOLOR_ARGB(255, 253, 167, 23);

public:
	CPostBoxFriendFindPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxFriendFindPage ();

protected:
	virtual void		CreateSubControl();;

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void		OpenSubControl ();

protected:
	void				LoadFriendList();
	void				LoadClubList();

protected:
	void				SetEnableOkButton ( const BOOL bEnable );

public:
	const CString&		GetSelectName () { return m_SelectName; }

private:
	CString					m_SelectName;
	INT						m_SelectIndex;

private:
	CBasicTextButton*		m_pButton_Ok;
	CBasicTextButton*		m_pButton_Close;

	CSmartComboBoxRollOver* m_pComboBox_Friend;
	CSmartComboBoxRollOver* m_pComboBox_Club;
};

