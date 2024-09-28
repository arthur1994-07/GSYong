// SkillEditDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"

#include <boost/function.hpp>

class CsheetWithTab;
class EngineDeviceMan;
class SkillViewDlg;
class SkillIconImage;

struct GLSKILL;

// CSkillEditDlg ��ȭ ����
class CSkillEditDlg : public CDialog
{
// ����
public:
	CSkillEditDlg(CWnd* pParent = NULL);	// ǥ�� ������
    virtual ~CSkillEditDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_SKILLEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

public:
	CsheetWithTab	*m_pSheetTab;
	char			m_szAppPath[MAX_PATH];
	CString			m_strAppPath;

protected:
	CToolBar    m_wndToolBar;
    EngineDeviceMan* m_pEngineDevice;

	// Skill Viewer
	SkillViewDlg* m_pSkillViewDlg;

//SaveIcon 
private:
	SkillIconImage	*m_pSkillIconImage;
	static int CALLBACK BrowseCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData );
//end SaveIcon

protected:
	bool DoSave ();

protected:
	void MakeToolBar ();

public:
	void EnableToobar ( UINT uIDEnableItem, BOOL bEnable );
	void EnableMenu ( UINT uIDEnableItem, BOOL bEnable );

	void SetReadMode();

	// Skill Viewer ����
	void SetSkillView( GLSKILL* pSkill );

	void ExportExcel ( boost::function<void (int)> PercentCallbackFunc=NULL );
	bool IsSkillLevelChecked ( WORD wLevel );

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PostNcDestroy();
public:		
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuitemExit();
	afx_msg void OnMenuitemNew();
	afx_msg void OnBnClickedButtonUpdateSync();
//	afx_msg void OnNcDestroy();
//	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonReloadStrtbl();
	afx_msg void OnBnClickedButtonStrtblSync();
	afx_msg void OnBnClickedCsvSave();
	afx_msg void OnBnClickedCsvLoad();
	afx_msg void OnBnClickedIconPaser();
	afx_msg void OnBnClickedBtnExcelStrTblImport();
	afx_msg void OnBnClickedBtnExcelStrTblExport();
	afx_msg void OnBnClickedBtnXmlConvert();
	afx_msg void OnBnClickedBtnExcelExport();
};

