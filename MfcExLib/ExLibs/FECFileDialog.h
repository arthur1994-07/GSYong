#ifndef _FECFILEDIALOG_H_
#define _FECFILEDIALOG_H_

#pragma once

//! '���� ���û���'���� ���� ������ �Ѳ����� ����������
//! �ڵ����� ����ũ�⸦ �÷��ִ� '���� ���û���'
class CFECFileDialog : public CFileDialog
{
    DECLARE_DYNAMIC(CFECFileDialog)

public:
    CFECFileDialog(
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        CWnd* pParentWnd = NULL );

    virtual ~CFECFileDialog();// { if (Files) { delete[] Files; delete[] Folder; } }

    virtual int DoModal();
	CString GetNextPathName(POSITION &pos) const;
	POSITION GetStartPosition();

protected:
	BOOL bParsed;
	TCHAR *Folder;
	TCHAR *Files;
	virtual void OnFileNameChange();
    DECLARE_MESSAGE_MAP()
};

#endif // _FECFILEDIALOG_H_