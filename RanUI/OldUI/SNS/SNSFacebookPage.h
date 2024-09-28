
// bjju.sns Ãß°¡

#pragma	once

#include "../Util/UIPage.h"

#include "../../../EngineLib/G-Logic/GLDefine.h"

class   CInnerInterface;
class   GLGaeaClient;
class   CBasicTextBox;
class   CBasicTextButton;
class   CBasicLineBoxSmart;
class   CUIMultiEditBoxSmart;
class   CUIEditBox;
class   CBasicComboBoxRollOver;
class   CSmartComboBoxBasic;
class   CCheckBox;

class CSNSFacebookPage : public CUIPage
{

protected:
	enum
	{
		PAGE_FACEBOOK = PAGE_NEXT,

		PAGE_EDITBOX_WALL,
		PAGE_BUTTON_WALL,

		PAGE_EDITBOX_PICTURE,
		PAGE_BUTTON_PICTURE,
		PAGE_COMBOBOX_PICTURE,
		PAGE_COMBOBOX_ROLLOVER_PICTURE,

		PAGE_EDITBOX_FRIEND,
		PAGE_BUTTON_FRIEND,

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
	CSNSFacebookPage (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSNSFacebookPage ();

protected:
	virtual void			CreateSubControl();
	virtual void			CloseSubControl ();

public:
	virtual void			Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void			TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void			OpenSubControl ();

protected:
	void					UpdateComboBox ();

public:
	void					ResetPictureCombo ( const BOOL bUpdate );

public:
	void					ClearEditWall	 ();
	void					ClearEditPicture ();

public:
	void					SetEnableWallButton ( const BOOL bEnable );
	void					SetEnablePicture	( const BOOL bEnable );

protected:
	CBasicLineBoxSmart*		CreateLineBox ( const char* szControl, UIGUID ControlID=NO_ID );

protected:
	void					FindSubDir ( CString strDir, CStringArray& FileArray, const CString& strWildcard="*.*" );

private:
	CBasicTextBox*			m_pStaticTextBox_WriteWall;
	CBasicTextBox*			m_pStaticTextBox_Picture;
	CBasicTextBox*			m_pStaticTextBox_Friend;

	CBasicLineBoxSmart*		m_pLineBox_WriteWall;
	CBasicLineBoxSmart*		m_pLineBox_Picture;
	CBasicLineBoxSmart*		m_pLineBox_Friend;

	CUIMultiEditBoxSmart*	m_pEditBox_WriteWall;
	CUIMultiEditBoxSmart*	m_pEditBox_Picture;
	CUIEditBox*				m_pEditBox_Friend;

	CSmartComboBoxBasic*	m_pComboBox_Picture;	
	CBasicComboBoxRollOver*	m_pComboBox_RollOver_Picture;

	CBasicTextButton*		m_pButton_WriteWall;
	CBasicTextButton*		m_pButton_Picture;
	CBasicTextButton*		m_pButton_Friend;
	CBasicTextButton*		m_pButton_Back;

	CCheckBox*				m_pCheckBox		[PAGE_CHECKBOX_MAX];
	CBasicTextBox*			m_pCheckBoxText	[PAGE_CHECKBOX_MAX];

protected:
	UIGUID					m_RollOverID;
	BOOL					m_bRollOverLBUP;
	DWORD					m_dwPicture;

protected:
    GLGaeaClient*           m_pGaeaClient;
    CInnerInterface*        m_pInterface;

};
