//	���� ������
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.6]
//			@ �ۼ�
//

#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"

class	CInnerInterface;
class	CBasicButton;
class	CStoragePage;
class	CBasicTextBox;
class GLGaeaClient;

class CStorageWindow 
    : public  CUIWindowEx
    , private CUIControlNpcActionable
{
private:
static	const	int nDEFAULT_PAGE;
static	const	int nUSE_STORAGE_CHANNEL = 5;
static	const	int nMAX_STORAGE_CHANNEL = 5;

protected:
	enum
	{
		STORAGE_PAGE0 = ET_CONTROL_NEXT,
		STORAGE_PAGE1,
		STORAGE_PAGE2,
		STORAGE_PAGE3,
		STORAGE_PAGE4,
		STORAGE_PAGE_END = STORAGE_PAGE0 + nMAX_STORAGE_CHANNEL,
		STORAGE_MONEY_BUTTON,
		STORAGE_PLUS_MONEY_BUTTON,
		STORAGE_MINUS_MONEY_BUTTON,
		STORAGE_ADD_BUTTON,
		STORAGE_BUTTON0,
		STORAGE_BUTTON1,
		STORAGE_BUTTON2,
		STORAGE_BUTTON3,
		STORAGE_BUTTON4,
		STORAGE_BUTTON_END = STORAGE_BUTTON0 + nMAX_STORAGE_CHANNEL,
	};

public:
	CStorageWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CStorageWindow ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	void	SetVisiblePage ( int nPage );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitStorage ( SNATIVEID sNPCID, DWORD dwGlobID );

public:
    SNATIVEID  GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD      GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

public:
	void	SetBLOCK ();
	void	ResetBLOCK ();

public:
	void	GetSplitPos ( WORD* pwPosX, WORD* pwPosY );
	void	SetSplitPos ( WORD wPosX, WORD wPosY );

public:
	int		GetChannel ();

private:
	void	SetMoney ( LONGLONG Money );

private:
	CBasicButton*		CreateButton ( const char* szButton, const char* szButtonFlip, UIGUID ControlID );

private:
	int		m_nOpenPage;
	CStoragePage*	m_pPage[nUSE_STORAGE_CHANNEL];
	CBasicButton*	m_pPageButton[nUSE_STORAGE_CHANNEL];

private:
	CBasicTextBox*		m_pMoneyTextBox;
	CUIControl*			m_pBLOCK;

private:
	WORD	m_wSplitItemPosX;
	WORD	m_wSplitItemPosY;
	BOOL	m_bSplitItem;

};


//////////////////////////////////////////////////////////////////////////////////
//#pragma	once
//
//#include "../Util/UIWindowEx.h"
//
//class	CBasicButton;
//class	CStoragePage;
//class	CBasicTextBox;
class	CClubStoragePage;

class	CClubStorageWindow
    : public  CUIWindowEx	
    , private CUIControlNpcActionable
{
private:
static	const	int nDEFAULT_PAGE;
static	const	int nUSE_STORAGE_CHANNEL = 5;
static	const	int nMAX_STORAGE_CHANNEL = 5;

protected:
	enum
	{
		CLUB_STORAGE_PAGE0 = ET_CONTROL_NEXT,
		CLUB_STORAGE_PAGE1,
		CLUB_STORAGE_PAGE2,
		CLUB_STORAGE_PAGE3,
		CLUB_STORAGE_PAGE4,
		CLUB_STORAGE_PAGE_END = CLUB_STORAGE_PAGE0 + nMAX_STORAGE_CHANNEL,
		CLUB_STORAGE_MONEY_BUTTON,
		CLUB_STORAGE_PLUS_MONEY_BUTTON,
		CLUB_STORAGE_MINUS_MONEY_BUTTON,
		CLUB_STORAGE_ADD_BUTTON,
		CLUB_STORAGE_BUTTON0,
		CLUB_STORAGE_BUTTON1,
		CLUB_STORAGE_BUTTON2,
		CLUB_STORAGE_BUTTON3,
		CLUB_STORAGE_BUTTON4,
		CLUB_STORAGE_BUTTON_END = CLUB_STORAGE_BUTTON0 + nMAX_STORAGE_CHANNEL,
	};

public:
	CClubStorageWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CClubStorageWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	void	SetVisiblePage ( int nPage );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitStorage ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );

public:
    SNATIVEID  GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD      GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

public:
	void	SetBLOCK ();
	void	ResetBLOCK ();

public:
	void	GetSplitPos ( WORD* pwPosX, WORD* pwPosY );
	void	SetSplitPos ( WORD wPosX, WORD wPosY );

public:
	int		GetChannel ();

private:
	void	SetMoney ( LONGLONG Money );

private:
	CBasicButton*		CreateButton ( const char* szButton, const char* szButtonFlip, UIGUID ControlID );

private:
	int		m_nOpenPage;
	CClubStoragePage*	m_pPage[nUSE_STORAGE_CHANNEL];
	CBasicButton*		m_pPageButton[nUSE_STORAGE_CHANNEL];

private:
	CBasicTextBox*		m_pMoneyTextBox;
	CUIControl*			m_pBLOCK;

private:
	WORD	m_wSplitItemPosX;
	WORD	m_wSplitItemPosY;
	BOOL	m_bSplitItem;
};