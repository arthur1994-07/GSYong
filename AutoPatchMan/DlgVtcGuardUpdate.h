#pragma once


// DlgVtcGuardUpdate ��ȭ �����Դϴ�.

class DlgVtcGuardUpdate : public CDialog
{
	DECLARE_DYNAMIC(DlgVtcGuardUpdate)

public:
	DlgVtcGuardUpdate(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgVtcGuardUpdate();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_VTC_GUARD_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
