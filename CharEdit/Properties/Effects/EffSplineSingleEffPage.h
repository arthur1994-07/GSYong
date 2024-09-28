#pragma once

#include "../EffCharPage.h"
#include "../../../EngineLib/DxEffect/Char/DxEffCharSplineSingleEff.h"

// CEffSplineSingleEffPage 대화 상자입니다.

class CEffSplineSingleEffPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffSplineSingleEffPage)

protected:
	EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF	m_Property;

private:
    D3DXMATRIX  m_matWorld;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );
    void UpdateKeyBoardMouse();

private:
    void ResetSplinePoint( int nCur );
    void UpdatePageMouseLClickUP();

public:
	CEffSplineSingleEffPage();   // 표준 생성자입니다.
	virtual ~CEffSplineSingleEffPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_SPLINE_SINGLE_EFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonApply();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonSpointInsert();
    afx_msg void OnBnClickedButtonSpointDelete();
    afx_msg void OnBnClickedButtonSpointUp();
    afx_msg void OnBnClickedButtonSpointDown();
    afx_msg void OnBnClickedButtonSelect();
    CListBox m_listSplinePoint;
    afx_msg void OnBnClickedButtonGetbonename();
    afx_msg void OnLbnSelchangeListSplinePoint();
    virtual BOOL OnKillActive();
};
