#pragma once
#include <string>

// CCrowData 대화 상자입니다.
class	CsheetWithTab;
struct	SCROWDATA;
class CCrowData : public CPropertyPage
{
	DECLARE_DYNAMIC(CCrowData)

public:
	CCrowData( LOGFONT logfont );
	virtual ~CCrowData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CROWDATA_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;
	
	SCROWDATA*		m_pDummyItem;
	SCROWDATA*		m_pItem;

	BOOL			m_bDlgInit;

	std::string		strKeyName;
	std::string		strName;
	CFont*			m_pFont;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( SCROWDATA* pItem );

protected:
	void	UpdateItems ();
	void	InverseUpdateItems ();
	void	InitDefaultCtrls ();

public:
	void	InitAllItem ();
	void	SetReadMode ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonSkinfile();
	afx_msg void OnBnClickedButtonTalkfile();
	afx_msg void OnBnClickedButtonFallingEffect();
	afx_msg void OnBnClickedButtonBlowEffect();
	afx_msg void OnBnClickedButtonItemgenfile();
    afx_msg void OnBnClickedButtonItemgenfile2();
	afx_msg void OnBnClickedButtonBirthEffect();
	afx_msg void OnBnClickedButtonSalefile01();
	afx_msg void OnBnClickedButtonSalefile02();
	afx_msg void OnBnClickedButtonSalefile03();
	afx_msg void OnEnChangeEditCrowname();
	afx_msg void OnBnClickedCheckAutodropout();
	afx_msg void OnBnClickedButtonTribe();
	afx_msg void OnBnClickedButtonCrow();
	afx_msg void OnBnClickedButtonBright();
	afx_msg void OnBnClickedButtonPetType();
	afx_msg void OnBnClickedButtonSummonType();
	afx_msg void OnBnClickedButtonNPCType();
	afx_msg void OnBnClickedButtonHpDnAct();
	afx_msg void OnBnClickedButtonHpDnAtkSet();
	afx_msg void OnBnClickedButtonHpListAdd();
	afx_msg void OnBnClickedButtonHpListDel();
	afx_msg void OnBnClickedCheckBarrier();
    afx_msg void OnBnClickedInteractionRange();
};
