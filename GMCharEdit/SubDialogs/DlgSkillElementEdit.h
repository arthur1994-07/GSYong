#pragma once


// CDlgSkillElementEdit ��ȭ �����Դϴ�.

class CDlgSkillElementEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkillElementEdit)

public:
	CDlgSkillElementEdit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSkillElementEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_SKILL_ELEMENT_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    CString m_strMainID;
    CString m_strSubID;
    CString m_strSkillName;
    CString m_strLevel;
    afx_msg void OnBnClickedOk();
};
