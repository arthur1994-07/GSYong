#pragma once

#include "../../RanLogic/Item/ItemSet/GLItemSet.h"
#include "../../enginelib/G-Logic/TypeDefine.h"

// SetListInfoDialog ��ȭ �����Դϴ�.

class SetListInfoDialog : public CDialog
{
	DECLARE_DYNAMIC(SetListInfoDialog)

public:
	SetListInfoDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~SetListInfoDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SETLIST_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

public:
	void OutPutSetList();
	SNATIVEID GetReturnID() {return m_GiveToSetItemInfoDlg;}
private:
	CListCtrl m_SetITListList;							//��Ʈ����Ʈ - ����Ʈ ��Ʈ��
	std::vector<SSETITEM>		m_vecSetIteminfoList;		//��Ʈ������ ���� ����Ʈ
	SNATIVEID m_GiveToSetItemInfoDlg;					//��Ʈ������ info dlg�� �ѱ�� ���� �뵵

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelect();
};
