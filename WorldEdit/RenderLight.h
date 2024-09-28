#pragma once


// CRenderLight 대화 상자입니다.

class CRenderLight : public CDialog
{
	DECLARE_DYNAMIC(CRenderLight)
public:
	BOOL	m_bNearLight;
	float	m_fSpeed;
	float	m_fColorPower;
	int		m_nRate;

public:
	CRenderLight(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRenderLight();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RENDERLIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditSpeed();
	afx_msg void OnEnChangeEditRate();
	afx_msg void OnEnChangeEditColorpower();
	afx_msg void OnBnClickedCheckNearlight();
};
