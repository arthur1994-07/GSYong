
// bjju.sns Ãß°¡

#pragma	once

#include "../Util/UIPage.h"

#include "../EngineLib/G-Logic/GLDefine.h"

class   CInnerInterface;
class   GLGaeaClient;
class   CBasicTextBox;
class   CBasicTextButton;
class   CBasicLineBoxSmart;
class   CUIMultiEditBoxSmart;
class   CUIEditBox;
class   CCheckBox;

class CSNSTwitterPage : public CUIPage
{

protected:
	enum
	{
		PAGE_TWITTER = PAGE_NEXT,

		PAGE_EDITBOX_TWEET,
		PAGE_BUTTON_TWEET,

		PAGE_EDITBOX_FOLLOWER,
		PAGE_BUTTON_FOLLOW,
		PAGE_BUTTON_UNFOLLOW,

		PAGE_CHECKBOX_TEMP1,
		PAGE_CHECKBOX_TEMP2,
		PAGE_CHECKBOX_TEMP3,
		PAGE_CHECKBOX_TEMP4,

		PAGE_BUTTON_BACK,

	};

	enum
	{
		INDEX_CHECK_BOX_LEVELUP,
		INDEX_CHECK_BOX_GRINDING,
		INDEX_CHECK_BOX_TEMP3,
		INDEX_CHECK_BOX_TEMP4,

		PAGE_CHECKBOX_BEGIN = PAGE_CHECKBOX_TEMP1,
		PAGE_CHECKBOX_MAX	= 4,
	};

public:
	CSNSTwitterPage (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSNSTwitterPage ();

protected:
	virtual void			CreateSubControl();
	virtual void			CloseSubControl ();

public:
	virtual void			Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void			TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void					ClearEditTweet	 ();

public:
	void					SetEnableTweet		( const BOOL bEnable );
	void					SetEnableFollower	( const BOOL bEnable );

protected:
	virtual void			OpenSubControl ();

protected:
	CBasicLineBoxSmart*		CreateLineBox ( const char* szControl, UIGUID ControlID=NO_ID );

private:
	CBasicTextBox*			m_pStaticTextBox_Tweet;
	CBasicTextBox*			m_pStaticTextBox_Follower;

	CBasicLineBoxSmart*		m_pLineBox_Tweet;
	CBasicLineBoxSmart*		m_pLineBox_Follower;

	CUIMultiEditBoxSmart*	m_pEditBox_Tweet;
	CUIEditBox*				m_pEditBox_Follower;

	CBasicTextButton*		m_pButton_Tweet;
	CBasicTextButton*		m_pButton_Follow;
	CBasicTextButton*		m_pButton_Unfollow;
	CBasicTextButton*		m_pButton_Back;

	CCheckBox*				m_pCheckBox		[PAGE_CHECKBOX_MAX];
	CBasicTextBox*			m_pCheckBoxText	[PAGE_CHECKBOX_MAX];

protected:
    GLGaeaClient*           m_pGaeaClient;
    CInnerInterface*        m_pInterface;

};
