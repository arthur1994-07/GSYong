#pragma once

#include "../../EngineLib/GUInterface/UIMan.h"

class	CUIEditGroup;
class   CDlgUIEdit;
class	CMainUIRight;
class	CSetParentDlg;
class	CUIControl;
class   CSetStaticTextDlg;
class	CLoadStaticListDlg;
class   CSetTipTextDlg;
class	CLoadTipListDlg;
class	CSetAlignDlg;

enum
{
	//	//SELECT_PARENT  = NO_ID + 1,
	//	//SELECT_STATIC_DIALOG,
	//	//LOAD_STATIC_LIST,
	//	//SELECT_TIP_DIALOG,
	//	//LOAD_TIP_LIST,
	//	//SET_ALIGN_DIALOG,
	MAIN_WINDOW_UI = NO_ID + 1,
	WINDOW_LAST,
	WINDOW_NUM = WINDOW_LAST - MAIN_WINDOW_UI,
};

enum
{
	DIALOG_CON,
	PICURE_CON,
	BUTTON_CON,
	BUTTON_IN_CON,
	BUTTON_CLICK_CON,
	STATIC_CON,
	EDIT_CON,
	PROGRESS_CON,
	ANIMATION_CON,
	ANIMATION_SUB_CON,
	BUTTON_TEXT_CON,
};

enum
{
	COMMON,
	ENGLISH,
	KOREA,
	JAPAN,
	HONGKONG,
	MALAYSIA,
	MALAYSIA_ENGLISH,
	TAIWAN,
	INDONESIA,
	PHILIPPINE,
	THAI_LAND,
	THAI_LAND_ENGLISH,
	VIETNAM_TOOL,
	CHINA,
	TS,
	COUNTRY_NUM,
};

struct GraphLineColor
{
	GraphLineColor(int _a, int _r, int _g, int _b)
		: a( _a )
		, r( _r )
		, g( _g )
		, b( _b )
	{
	}

	int a;
	int r;
	int g;
	int b;
};

struct UIContainer				// UI정보를 가지고 있는 구조체
{								// (이정보들은 자체파일의 저장 및 불러오기 및 XML Export을 위해 필요하다)
	CUIEditGroup* pControl;		// 콘트롤 주소
	int			nTextureWidth;	// 텍스쳐의 넓이
	int			nTextureHeight;	// 텍스쳐의 높이
	int			nControlType;	// 콘트롤의 종류(다이얼로그,버튼,스태틱,콘트롤......)
};

class EngineDeviceMan;

class CInterfaceAdmin : public CUIMan
{
	enum {
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN,
		KEY_NUM };

public:
	std::vector<UIContainer> m_pUIContainer;
	UIContainer				 m_sTempContainer;		

	GraphLineColor		m_LineColor;
	CUIEditGroup*		m_pSelectedControl;
	int		m_nTotalWindowNum;											
	//static const CString strCountry[COUNTRY_NUM];
	CString strCountry;

	CDlgUIEdit*	m_pMainDlg;
	BOOL	m_bListSelect;

	// 각각 콘트롤의 생성숫자
	int		m_nAnimaitionCtrlNum;
	int		m_nStaticCtrlNum;															
	int		m_nDialogCtrlNum;
	int		m_nPicCtrlNum;
	int		m_nProgressCtrlNum;
	int		m_nButtonCtrlNum;
	int		m_nEditCtrlNum;
	int		m_nScrollFormNum;

	int		m_nLineInterval;
	BOOL	m_bGraphPaper;

	int		m_nViewWidth;
	int		m_nViewHeight;

	int		m_nLastViewWidth;
	int		m_nLastViewHeight;

	int		m_nCountryCode;

	float	m_fInputElapsedTime;
	BOOL	m_bKeyInput[KEY_NUM];

	//이전위치 사이즈
	CPoint	m_PrevPos;
	CPoint	m_PrevSize;

private:
	void ResetControl();

public:
	CInterfaceAdmin(EngineDeviceMan* pEngineDevice);
	virtual ~CInterfaceAdmin();

private:
    EngineDeviceMan* m_pEngineDevice;
	
public:
	void Init(CDlgUIEdit* dialog);

	// Note : for DX
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();
	virtual	HRESULT FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
	virtual HRESULT FinalCleanup ();
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	// Note : for List
	void	AddUIList(CUIEditGroup* pCreateUI, int nControlType);						
	void	CopyUIList(int nUINum);														
	void	PasteUIList();																
	void	DelUIList(CUIEditGroup* pDeleteUI);											
	void	SortUIList();
	void	ClearUIList();

	CUIEditGroup*		FindUIListCtrl(CString strFindUI);
	CUIEditGroup*       FindUIListCtrl(UIGUID FindUI);
	int		FindUIListIndex(CUIEditGroup* pFindUI);											// UIContainer에 정보를 검색한다.(반환값은 해당 배열) 
	int		FindUIListIndex(CString strFindUI);	
	int		FindUIListIndex(UIGUID FindUI);																							// PS:이 함수로 UIContainer에 콘트롤의 유무를 알수 있다. (없을 시 -1 반환)
	void	CloseAllWindow ();															// 모든 다이얼로그를 닫는다.
	virtual void	ReSizeWindow(int cx ,int cy);												// 윈도우 사이즈 변경
	void	SetViewSize(int nWidth, int nHeight);
	void	AlignControlAll();
	void	ResetMainUI();

	// Note : Create UI
	void	MakeDynamicDialog();														
	void	MakeDynamicControl();														
	void	MakeDynamicButton();														
	void	MakeDynamicStatic();														
	void	MakeDynamicAnimation();														
	void	MakeDynamicScroll();

	int		GetSelectedControlType();
	CUIEditGroup* GetSelectedControl() { return m_pSelectedControl; }						// 현재 선택(픽킹)된 콘트롤의 정보를 가져온다.
	void	SetSelectedControl(int nSelNum, bool dlgUpdate = true);											// 현재 선택(픽킹)된 콘트롤의 정보를 저장한다.	// (숫자는 UIContainer의 배열을 기준으로 한다.)									

	void	SetParentControl(CUIEditGroup* pSelpUI, CUIEditGroup* pParentUI);
	void	DeleteParentControl();

	int		GetLineInterval() { return m_nLineInterval; }
	void	SetLineInterval(int nInterval) { m_nLineInterval = nInterval; } 

	BOOL	IsGraphVisible() {return  m_bGraphPaper; }
	void	SetGraphVisible(BOOL bVisible) { m_bGraphPaper = bVisible; } 

	GraphLineColor GetGraphLineColor() {return m_LineColor;}
	void	 SetGraphLineColor(int nA,int nR,int nG,int nB);

	void	KeyInputCheck(float fElapsedTime );

	void	SetCountry(int nCode);
	int		GetCountry() { return m_nCountryCode; }

	BOOL	SaveXML(CString p_filename);												// Xml 저장
	BOOL	LoadXML(CString p_filename);												// Xml 불러오기
	BOOL	SaveRUI(CString p_filename);												// 자체파일 저장

	void	EndTextEdit();
};