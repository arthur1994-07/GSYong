#pragma once
#include "afxwin.h"

#include "../EngineLib/G-Logic/GLDefine.h"
#include "../RanLogic/Land/GLMapList.h"


// CMapDialog 대화 상자입니다.

class CMapDialog : public CDialog
{
	DECLARE_DYNAMIC(CMapDialog)

public:
	CMapDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAPDIALOG };

public:
	SNATIVEID	m_sMapID;
	GLMapList	m_sMapList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_cListMap;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedOk();
};
