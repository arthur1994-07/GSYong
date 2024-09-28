#pragma once


// CDlgSkillElementEdit 대화 상자입니다.

class CDlgSkillElementEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkillElementEdit)

public:
	CDlgSkillElementEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSkillElementEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SKILL_ELEMENT_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    CString m_strMainID;
    CString m_strSubID;
    CString m_strSkillName;
    CString m_strLevel;
    afx_msg void OnBnClickedOk();
};
