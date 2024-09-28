#pragma once
#include "afxwin.h"


// CParticleHighDlg 대화 상자입니다.

class CParticleHighDlg : public CDialog
{
	DECLARE_DYNAMIC(CParticleHighDlg)

public:
	int		m_nBlend;
	BOOL	m_bGCollision;
	BOOL	m_bGotoCenter;
	BOOL	m_bCollision;
	BOOL	m_bAttraction;
	BOOL	m_bCenter;
	BOOL	m_bParentMove;
	BOOL	m_bParentRotate;
	BOOL	m_bParentRotateWorld;
	BOOL	m_bDirection;
	BOOL	m_bBillBoardAll;
	BOOL	m_bBillBoardUp;
	BOOL	m_bBillBoardGround;
	BOOL	m_bEffThrow;
	BOOL	m_bFromTo;
	BOOL	m_bHeightNO;


protected:
	void	SetStatic ();

public:
	CParticleHighDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CParticleHighDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PARTICLE_HIGH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedCheckGcollision();
	afx_msg void OnBnClickedCheckGotocenter();
	afx_msg void OnBnClickedCheckCollision();
	afx_msg void OnBnClickedCheckAttraction();
	afx_msg void OnBnClickedCheckParent();
	afx_msg void OnBnClickedCheckDirection();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboBlend();
	afx_msg void OnBnClickedCheckCenter();
	afx_msg void OnBnClickedCheckParentMove();
	afx_msg void OnBnClickedCheckParentRotateWorld();
	afx_msg void OnBnClickedCheckBillall();
	afx_msg void OnBnClickedCheckBillup();
	afx_msg void OnBnClickedCheckGround();
	afx_msg void OnBnClickedCheckThrow();
	afx_msg void OnBnClickedCheckFromto();
	afx_msg void OnBnClickedCheckHeightNo();
};
