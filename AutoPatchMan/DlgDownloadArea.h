#pragma once

#include "afxwin.h"
#include "../RanLogic/RANPARAM.h"
#include "TransparentHelper.h"
#include "StaticEx.h"
#include "BtnST.h"

// CDlgDownloadArea 대화 상자입니다.

class CDlgDownloadArea : public CDialog
{
	DECLARE_DYNAMIC(CDlgDownloadArea)

public:
	CDlgDownloadArea(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDownloadArea();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DOWNLOAD_AREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nDownloadArea;

	CDC* m_pMemDC;
	CBitmap* m_pOldBitmap;
	CBitmap* m_pBitmapMem;

	//		몸통 끌어서 움직이기 관련
	BOOL	m_bLBDown;
	CPoint	m_MouseClickPos;

	// Control
	CStaticEx m_stTitle;
	CStaticEx m_stArea;
	
	CButtonST m_btnOK;
	CButtonST m_radioDownArea1;
	CButtonST m_radioDownArea2;
	CButtonST m_radioDownArea3;


public:
	void BuildBkDC();
	LRESULT OnTransaprentBk( WPARAM wParam, LPARAM lParam);

	BOOL	InitCtrls	();
	BOOL	UpdateCtrls ();
	BOOL	InverseUpdateCtrls ();

	CString m_strAreaName[RANPARAM::MAX_CHINA_REGION];
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedOk();
};
