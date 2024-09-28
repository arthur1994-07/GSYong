#pragma once
#include "afxwin.h"
#include "../../EngineLib/DxCommon/dsutil.h"
#include "../../MfcExLib/ExLibs/SoundSourceMan.h"
#include "afxcmn.h"

// CSoundSourceDlg 대화 상자입니다.

class CSoundSourceDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoundSourceDlg)

public:
	CSoundSourceDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSoundSourceDlg();

protected:
    SSoundSource	m_SoundSource;

	CSound*			m_pSound;
	DWORD			m_BufferID;

protected:
	CSoundSourceMan	*m_pSoundSourceMan;

public:
	void			SetState ( CSoundSourceMan *pSoundSourceMan );
	void			GetSelectSoundSource ( SSoundSource &SoundSource );

	void			ReloadAllItems ();
	BOOL			CompareAllToken ( CString Left, CString Right );


// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SOUNDSOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:	
	CComboBox m_ctrlCategory;
	CString m_valKeyword;
	CListCtrl m_ctrlSoundSource;
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnNMDblclkListSoundsource(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboCategory();
};
