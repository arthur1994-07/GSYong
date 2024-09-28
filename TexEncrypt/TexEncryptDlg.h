// TexEncryptDlg.h : ��� ����
//
#pragma once
#include "afxwin.h"


// CTexEncryptDlg ��ȭ ����
class CTexEncryptDlg : public CDialog
{
// ����
public:
	CTexEncryptDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_TEXENCRYPT_DIALOG };

	enum 
	{
		ENCRYPT_TEX_FILE,
		ENCRYPT_TEX_FOLDER,
		DECRYPT_TEX_FILE,
		DECRYPT_TEX_FOLDER,
		ENCRYPT_SKIN_FILE,
		ENCRYPT_SKIN_FOLDER,
		DECRYPT_SKIN_FILE,
		DECRYPT_SKIN_FOLDER,
		ENCRYPT_WIDGET_SCRIPT,
		DECRYPT_WIDGET_SCRIPT,
		OPTION_SIZE,
	};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����



// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cCombo;
	CEdit m_cEdit;
	bool m_bDelete;

	afx_msg void OnBnClickedOpenButton();
	afx_msg void OnBnClickedRunButton();


public:
	CString m_strAppPath;

	void SetAppPath();
	void Init();
	void OpenEncryptTexFile();
	void OpenDecryptTexFile();
	void OpenEncryptSkinFile();
	void OpenDecryptSkinFile();
	void OpenSelectFolder();
	afx_msg void OnCbnSelchangeOptionCombo();

	BOOL RunEncryptTexFile( const CString& strInput, const CString& strOutput );
	BOOL RunEncryptTexFolder( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptTexFile( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptTexFolder( const CString& strInput, const CString& strOutput );
	
	BOOL RunEncryptSkinFile( const CString& strInput, const CString& strOutput );
	BOOL RunEncryptSkinFolder( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptSkinFile( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptSkinFolder( const CString& strInput, const CString& strOutput );

	void AddEditString( const char* szInfo );

private:
	BOOL RunEncryptWidgetScriptFolder( const CString& strInput, const CString& strOutput );
	BOOL RunEncryptWidgetScript( const CString& strInput, const CString& strOutput );
	BOOL Compile_lua_luc( const TSTRING& strCommand );
};

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData);


