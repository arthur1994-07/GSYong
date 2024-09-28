#pragma once
#include "afxcmn.h"

class CsheetWithTabChar;
class DxFootColl;

// CFootCollPage 대화 상자입니다.

class CFootCollPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFootCollPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

	// Matrix를 알맞게 변환한다.
	void UpdateKeyBoardMouse();

public:
	CFootCollPage();
	virtual ~CFootCollPage();

private:
	D3DXVECTOR3	*m_pSelectFootPoint;
	D3DXMATRIX	m_matWorld;	// 임시적으로 쓰이는 Matrix 값

	int		m_nParentSel;
	std::tr1::shared_ptr< DxFootColl >	m_pFootColl;

private:
	void UpdateButton( BOOL bPoint2, BOOL bPoint4 );
	void ActiveEditMatrix( const D3DXVECTOR3& vPos );

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FOOT_COLL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	CListCtrl m_ctrlFootColl;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListFootColl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioPoint2();
	afx_msg void OnBnClickedRadioPoint4();
	afx_msg void OnBnClickedButtonPoint2Front();
	afx_msg void OnBnClickedButtonPoint2Back();
	afx_msg void OnBnClickedButtonPoint4FrontLeft();
	afx_msg void OnBnClickedButtonPoint4FrontRight();
	afx_msg void OnBnClickedButtonPoint4BackLeft();
	afx_msg void OnBnClickedButtonPoint4BackRight();
	afx_msg void OnBnClickedRadioPointNone();
};
