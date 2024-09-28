#pragma once

#include "../resource.h"
#include "../../MfcExLib/ExLibs/SoundSourceMan.h"

class CsheetWithTab;
struct SMovSound;


// CEffectSoundEdit ��ȭ �����Դϴ�.

class CEffectSoundEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffectSoundEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }	

public:
	DWORD			m_dwOldPageIndex;
	SMovSound*		m_pMovSound;

protected:
	CSoundSourceMan			m_SoundSourceMan;
	SSoundSource*			m_pSoundSource;

public:
	void SetProperty ( SMovSound* pMovSound, DWORD dwPageIndex );

public:
	CEffectSoundEdit();
	virtual ~CEffectSoundEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGetsound();
	virtual BOOL OnInitDialog();
	CString m_MinRange;
	CString m_MaxRange;
	CString m_valBufferCount;
	CString m_valSound;
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonDel();
};
