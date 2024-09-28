#pragma once
#include "afxcmn.h"

class CsheetWithTabChar;
class DxFootColl;

// CFootCollPage ��ȭ �����Դϴ�.

class CFootCollPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFootCollPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

	// Matrix�� �˸°� ��ȯ�Ѵ�.
	void UpdateKeyBoardMouse();

public:
	CFootCollPage();
	virtual ~CFootCollPage();

private:
	D3DXVECTOR3	*m_pSelectFootPoint;
	D3DXMATRIX	m_matWorld;	// �ӽ������� ���̴� Matrix ��

	int		m_nParentSel;
	std::tr1::shared_ptr< DxFootColl >	m_pFootColl;

private:
	void UpdateButton( BOOL bPoint2, BOOL bPoint4 );
	void ActiveEditMatrix( const D3DXVECTOR3& vPos );

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_FOOT_COLL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
