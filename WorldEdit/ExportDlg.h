#include "afxwin.h"
#if !defined(AFX_EXPORTDLG_H__2DB4D1D9_A36E_4529_802F_775ED0C9570E__INCLUDED_)
#define AFX_EXPORTDLG_H__2DB4D1D9_A36E_4529_802F_775ED0C9570E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

struct	SExportParam
{
	int		m_Result;
	int		m_nFacecount;
	BOOL	m_bOneCell;
	BOOL    m_bCreateLightMap;
	BOOL	m_bRan_1_0_Export;
    TCHAR	m_szLightMapName[MAX_PATH];
    float   m_fLightMapDetail;

	BOOL	m_bRadiosity;
	int		m_nRadiosityPass;
	float	m_fRadiosityReflectance;
	float	m_fRadiosityFogBlend;
	DWORD	m_dwRadiosityFogLength;

    SExportParam()
        : m_bOneCell(FALSE)
		, m_fLightMapDetail(0.25f)
		, m_bRan_1_0_Export(FALSE)
    {
    }
};


class CExportDlg : public CDialog
{
// Construction
public:
	SExportParam		m_SParam;
	CExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportDlg)
	enum { IDD = IDD_DIALOG_EXPORT };
	CEdit	m_ctrlEditFacecount;
	CButton	m_ctrlButtonOK;
	CButton	m_ctrlButtonStart;
	CProgressCtrl	m_ctrlProgress;
	int		m_Facecount;
	CListBox m_ctrlListLog;
	CButton m_ctrlButtonCheckIntegrity;
	//}}AFX_DATA

private:
    void SetLightMap_SetLandMan();
	void DisplayRADFogColor();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	afx_msg void OnButtonExport();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonExportOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnBnClickedButtonExportCheckintegrity();
	CComboBox m_comboSampling;
	afx_msg void OnBnClickedCheckRan10Export();
	afx_msg void OnEnChangeEditRADFogBlend();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__2DB4D1D9_A36E_4529_802F_775ED0C9570E__INCLUDED_)
