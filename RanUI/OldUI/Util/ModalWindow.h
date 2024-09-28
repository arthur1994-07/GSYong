#pragma	once

#include "../../../EngineLib/GUInterface/UIModal.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h" // 2차비번

enum
{
	OKCANCEL,
	YESNO,
	OK,
	CANCEL,
	EDITBOX,
	EDITBOX_PETNAME,
	EDITBOX_NUMBER,
	EDITBOX_MONEY,
	MODAL_EDITBOX_LOTTERY,
	MODAL_FILE_NAME,
	NO_CONTROL
};

enum
{	
	MODAL_INFOMATION,
	MODAL_QUESTION,
	MODAL_WARNING,
	MODAL_ERROR,
	MODAL_CRITICAL,
	MODAL_INPUT,
	MODAL_NOTITLE
};

class	CBasicTextBox;
class	CBasicTextButton;
class	CUIEditBox;
class	CBasicLineBox;
class	CPassKeyboard; // 2차비번
class	CRandPassKeyboard; // 2차비번
class	CUIEditBoxMan;

class	CModalWindow : public CUIModal
{
	static	const int nLIMIT_CHAR;
	static	const int nMONEY_UNIT;
	static	const CString strMONEY_SYMBOL;
	static	const int nLIMIT_MONEY_CHAR;
	static	const int nLIMIT_NUMBER_CHAR;

	static	UIGUID	m_nMODAL_NEWID;

protected:
	UIGUID	m_PrevPageID;

    CBasicTextBox*		m_pTextBox;
    CBasicTextBox*		m_pTextBoxTail;
	CBasicTextButton*	m_pOK;
	CBasicTextButton*	m_pCANCEL;
	CBasicTextButton*	m_pYES;
	CBasicTextButton*	m_pNO;
	CUIEditBox*			m_pEditBox;
    CBasicLineBox*		m_pEditBoxBack;
    CBasicLineBox*		m_pLineBoxWindow;
    CBasicLineBox*		m_pLineBoxWindowTipbox;
    CBasicLineBox*		m_pLineBoxTextBox;
    CBasicLineBox*		m_pLineBoxTipbox;
	CBasicLineBox*		m_pLineBoxEditMan;
	CBasicLineBox*		m_pLineBoxEditMan2;

	// 2차 비밀번호 - 신규 생성 설정
	CBasicLineBox*		m_pLineBoxBackground;
	CBasicTextBox*		m_pTextBoxPassSetTitle;			// 텍스트 - 2차 비밀번호 설정
	CBasicTextBox*		m_pTextBoxPassSetInput;			// 텍스트 - 2차 비밀번호 입력
	CBasicTextBox*		m_pTextBoxPassSetReInput;		// 텍스트 - 2차 비밀번호 재입력

	CPassKeyboard*		m_pPassKeyboard;		// 2차비번 - 로그인할 때 뜸 ( 키보드 )
	CRandPassKeyboard*	m_pRandPassKeyboard;	// 2차비번 - 숫자 랜덤 ( 설정되어있을때, 신규 )
	CUIEditBoxMan*		m_pEditBoxMan;			// 2차비번

	CUIControl*	m_p1BUTTON;
	CUIControl*	m_p2BUTTONLEFT;
	CUIControl*	m_p2BUTTONRIGHT;

	int	m_nModalType;
	int	m_nData1;
	int	m_nData2;
	D3DXVECTOR3 m_vData; // by luxes.

public:
	bool m_bTEST_MULTI;

protected:
	enum
	{
		MODAL_OK = ET_CONTROL_NEXT,
		MODAL_CANCEL,
		MODAL_YES,
		MODAL_NO,
		MODAL_EDITBOX,
		MODAL_EDITBOX_MAN,
		MODAL_EDITBOX_MAN_INPUT,
		LOGIN_EDIT_PWD,
		LOGIN_EDIT_2ND_PWD,
	};

public:
	CModalWindow (EngineDeviceMan* pEngineDevice);
	virtual	~CModalWindow ();

public:
	void	CreateBaseModal ( char* szWidowKeyword );
	void	CreateSubControl ();

protected:
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetPrevPageID ( UIGUID ControlID )		{ m_PrevPageID = ControlID; }
	UIGUID	GetPrevPageID ()						{ return m_PrevPageID; }

	void	SetHide ( BOOL bHide )					{ m_pEditBox->SetHide( bHide ); }

	CString	GetEditString ()						{ return m_pEditBox->GetEditString(); }

	CUIEditBox *GetEditBox() { return m_pEditBox; } // 2차비번
	CUIEditBoxMan* GetEditBoxMan() { return m_pEditBoxMan; }
	BOOL Is2ndPWDModal();
	int CheckNew2ndPassString();
	CString GetNew2ndPassString();

public:
	//virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) = 0;
	//virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )													= 0;
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void	DoModal       ( const std::string& strText, int nModalTitle, int nModalType, UIGUID CallerID, bool bShowPassKey = false, bool bTwian = false );
    void	DoModalTipBox ( const std::string& strText, const std::string& strTip, int nModalTitle, int nModalType, UIGUID CallerID );
//	void	DoMODAL ( const CString& strMessage, const CString& strTitle, const int nType, const UIGUID CallerID );

public:
	void	SetModalData ( int nData1, int nData2 );
	void	SetModalData(const D3DXVECTOR3& vData); // by luxes.
	void	GetModalData ( int* pnData1, int* pnData2 );
	void	GetModalData ( D3DXVECTOR3* vData ); // by luxes.

private:
	void	ReArrangeControl ( int nModalType, BOOL bUseLimitedInput = FALSE );

protected:
    void	ResizeControl     ( char* szWindowKeyword );
	void	ModalWindowResize ( char* szWindowKeyword );
	bool	IsEditBoxType();

public:
	static int MAKE_MODAL_NEWID()					{ return m_nMODAL_NEWID++; }

};
