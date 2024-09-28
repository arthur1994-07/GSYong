#pragma once
#include "afxwin.h"
#include "../resource.h"

// CMobSetPage 대화 상자입니다.
class CsheetWithTab;
class GLMobSchedule;
class CMobSetPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMobSetPage)

public:

	typedef std::map<std::string,std::string> MAP_GROUPLEAD;
	typedef MAP_GROUPLEAD::iterator			  MAP_GROUPLEAD_ITER;

	CMobSetPage();
	virtual ~CMobSetPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MOBSETPAGE };

	enum EM_EDIT_TYPE
	{
		EM_EDIT_NONE = 0,
		EM_EDIT_SINGLE,
		EM_EDIT_MULTIPLE
	};

protected:
	CsheetWithTab*			m_pSheetTab;
	BOOL					m_bInit;

	// Ctrl + x, Ctrl + c, Ctrl + v
	bool					m_bCut;
	CString					m_strCopyMobName;
	CString					m_strBackMobName;

	MAP_GROUPLEAD			m_mapGroupLead;

public:
	static EM_EDIT_TYPE		m_emEditType;
	static BOOL				m_bMaterial;
	static GLMobSchedule	m_Schedule;
	static UINT				g_unLayer;

public:
	void UpdatePage ();

	BOOL UpdateDataSchedule ( BOOL bGetData, BOOL bNew=TRUE );

public:
	void	ActiveMobSetEdit ( EM_EDIT_TYPE emType = EM_EDIT_NONE );	
	void	ListingUpdate ();
	void	CheckGroupLeader();

	void	SetSelectSchedule ( GLMobSchedule* pMobSch );

public:
	// Ctrl + x, Ctrl + c, Ctrl + v
	void	CutMobSch();
	void	CopyMobSch();
	void	PasteMobSch();

public:
	void	SetNewID ( WORD MID, WORD SID );
	void	SetPC_GATE_REGEN ( DWORD dwGateID );

	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

	void	MobLayerUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonSchedule();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTest();

	CListBox m_ctrlMobList;
	afx_msg void OnBnClickedButtonCrowid();
	afx_msg void OnLbnDblclkListMobset();
	afx_msg void OnBnClickedButtonRendgen();
	afx_msg void OnBnClickedCheckRendgenpos();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonGateidPcRegen();
	afx_msg void OnBnClickedButtonBusLocation();
	afx_msg void OnBnClickedButtonCsvExport();
	afx_msg void OnBnClickedButtonCsvImport();
	afx_msg void OnBnClickedWeekCheck();
	afx_msg void OnDeltaposSpinRendgenposNum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinRendgenposDist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckLeader();
	afx_msg void OnBnClickedCheckGroupRegen();
	afx_msg void OnBnClickedButtonMultimodify();
	CListBox m_ctrlMobSelectList;
	afx_msg void OnBnClickedCheckMinimap();
	afx_msg void OnBnClickedCheckEachRegen();
	CComboBox m_comboMobLayer;
	afx_msg void OnBnClickedButtonMoblayerAdd();
	afx_msg void OnCbnSelchangeComboMoblayer();
	afx_msg void OnBnClickedButtonMoblayerDel();
	CEdit m_editActivityArea;
	CEdit m_editInfluenceY;
};
