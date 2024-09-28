#pragma once
#include "afxwin.h"
#include "../MfcExLib/ExLibs/SoundSourceMan.h"

struct	SVecSound;
class	CStaticSoundMan;
class	CsheetWithTab;

// CWorldSoundPage 대화 상자입니다.

class CWorldSoundPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CWorldSoundPage)

protected:
	CsheetWithTab	*m_pSheetTab;
	BOOL			m_bDlgInit;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void UpdateContrl ();
	void ListingUpdate ();
	void UpdatePageClickDOWN();

public:
	CWorldSoundPage();
	virtual ~CWorldSoundPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SOUND };

protected:
	CSoundSourceMan		m_SoundSourceMan;
	CStaticSoundMan*	m_pStaticSoundMan;

	SSoundSource	m_SoundSource;
	SVecSound*		m_pVecSound;

	int				m_LastUseItem;


protected:
	void	ActiveSoundEdit ( BOOL bShow, BOOL bNew );
	void	DefaultValueSet ( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_valSound;
	int m_valBufferCount;
	afx_msg void OnBnClickedButtonGetsound();
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonCancel();
	CListBox m_ctrlMapSound;
	CString m_MapSoundName;
	BOOL	m_bLoop;
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedCheckRenderRange();
	afx_msg void OnBnClickedCheckEnablesound();
	afx_msg void OnLbnSelchangeListMapsound();
	afx_msg void OnBnClickedCheckDspName();
	CString m_MinRange;
	CString m_MaxRange;	
	CString m_Rolloff;
	CString m_Distance;
	CString m_Doppler;
	CString m_VelocityX;
	CString m_VelocityY;
	CString m_VelocityZ;
	CString m_LoopTime;
	CEdit m_ctrlLoopTime;
	CString m_Volume;	
};
