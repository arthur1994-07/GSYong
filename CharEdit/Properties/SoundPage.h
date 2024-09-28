#pragma once
#include "afxwin.h"
#include "../MfcExLib/ExLibs/SoundSourceMan.h"
#include "../EngineLib/DxSound/CharacterSound.h"

// CSoundPage ��ȭ �����Դϴ�.
class CsheetWithTabAnim;

class CSoundPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSoundPage)

public:
	CSoundPage();
	virtual ~CSoundPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SOUNDPAGE };	

private:
	SChaSoundData	m_DummyChaSoundData;
	SChaSoundData*	m_pChaSoundData;

private:
	CSoundSourceMan			m_SoundSourceMan;	

protected:
	CsheetWithTabAnim*		m_pSheetTab;

public:
	void	SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void	SetSound ( SChaSoundData* pChaSound/*int *pPlayFrameCount, WORD *pPlayFrame, char* szFileName*/ );

	void	FrameMarking ( BOOL bReset=FALSE );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_ctrlStartFrame;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	virtual BOOL OnInitDialog();
};
