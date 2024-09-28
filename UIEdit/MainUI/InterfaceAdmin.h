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

struct UIContainer				// UI������ ������ �ִ� ����ü
{								// (���������� ��ü������ ���� �� �ҷ����� �� XML Export�� ���� �ʿ��ϴ�)
	CUIEditGroup* pControl;		// ��Ʈ�� �ּ�
	int			nTextureWidth;	// �ؽ����� ����
	int			nTextureHeight;	// �ؽ����� ����
	int			nControlType;	// ��Ʈ���� ����(���̾�α�,��ư,����ƽ,��Ʈ��......)
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

	// ���� ��Ʈ���� ��������
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

	//������ġ ������
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
	int		FindUIListIndex(CUIEditGroup* pFindUI);											// UIContainer�� ������ �˻��Ѵ�.(��ȯ���� �ش� �迭) 
	int		FindUIListIndex(CString strFindUI);	
	int		FindUIListIndex(UIGUID FindUI);																							// PS:�� �Լ��� UIContainer�� ��Ʈ���� ������ �˼� �ִ�. (���� �� -1 ��ȯ)
	void	CloseAllWindow ();															// ��� ���̾�α׸� �ݴ´�.
	virtual void	ReSizeWindow(int cx ,int cy);												// ������ ������ ����
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
	CUIEditGroup* GetSelectedControl() { return m_pSelectedControl; }						// ���� ����(��ŷ)�� ��Ʈ���� ������ �����´�.
	void	SetSelectedControl(int nSelNum, bool dlgUpdate = true);											// ���� ����(��ŷ)�� ��Ʈ���� ������ �����Ѵ�.	// (���ڴ� UIContainer�� �迭�� �������� �Ѵ�.)									

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

	BOOL	SaveXML(CString p_filename);												// Xml ����
	BOOL	LoadXML(CString p_filename);												// Xml �ҷ�����
	BOOL	SaveRUI(CString p_filename);												// ��ü���� ����

	void	EndTextEdit();
};