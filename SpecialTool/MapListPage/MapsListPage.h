#pragma once

#include "afxcmn.h"
#include "../Resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../RanLogic/Land/GLMapList.h"

class	CsheetWithTab;
// CMapsListPage 대화 상자입니다.

struct SORTPARAM
{
	int sortColumn;
	bool sortDirect;
	CListCtrl* pList;
};

class CMapsListPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMapsListPage)

public:
	CMapsListPage( LOGFONT logfont );
	virtual ~CMapsListPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAPSLISTPAGE };

protected:
	CsheetWithTab*	m_pSheetTab;
	CFont*			m_pFont;

public:
	CListCtrl	m_ListMapsList;
	SNATIVEID	m_sFindID;
	std::string m_strFindMapName;
	bool		m_bEdit;
	
	INT			m_nResultSortColumn;
	bool		m_bResultSortDirect;
	NMHDR		m_sNMHDR;
	bool		m_bUpdate;

public:
	GLMapList	m_sMapsList;

public:
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

	void Init();
	void UpdateItems ();

	bool NewFile();
	bool LoadFile();
	bool LoadFileOld();
	bool SaveFile();

	bool SaveCsv();
	bool LoadCsv();

	bool IsEdit()	{ return m_bEdit; }
	static INT CALLBACK CompareFunc(LPARAM LParam1, LPARAM lParam2, LPARAM lParamSort );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnNMDblclkListMapslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonFindId();
	afx_msg void OnBnClickedButtonFindMapname();
	afx_msg void OnHdnItemclickListMapslist(NMHDR *pNMHDR, LRESULT *pResult );
    afx_msg void OnBnClickedButtonConvertToLua();
};
