#pragma once


// CEffEditBar 대화 상자입니다.
class CsheetWithTab;
class CEffEditBar : public CDialog
{
	DECLARE_DYNAMIC(CEffEditBar)

public:
	CsheetWithTab*	m_psheetWithTab;

public:
	void SelectEffProp ( DWORD dwProp );

public:
	CEffEditBar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffEditBar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void PostNcDestroy();
};
