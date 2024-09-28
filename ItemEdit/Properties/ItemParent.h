#pragma once

#include <string>

class CsheetWithTab;
class CIconPreview;
struct SITEM;
struct SSETITEM;

class CItemParent : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemParent)

public:
	CItemParent( LOGFONT logfont );
	virtual ~CItemParent();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMPARENT_DIALOG };

    enum
    {
        TIMER_ITEMPARENT_SAVE_STATE_ID = 11,

        SAVE_MESSAGE_APPEAR_TIME = 1000,
    };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	SITEM*			m_pDummyItem;
	SITEM*			m_pItem;

	BOOL			m_bDlgInit;

	std::string		strKeyName;
	std::string		strName;
	std::string		strKeyDesc;
	std::string		strDesc;
	CFont*			m_pFont;

	CIconPreview*	m_pIconPreview;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( SITEM* pItem );
    void    ShowSaveState();

protected:
	void	UpdateItems ();
	BOOL	InverseUpdateItems ();
	void	InitDefaultCtrls ();
	void	SetInvenIconImage();

	//////////////////////////////////////////////////////////////////////////////
	//	리팩토링 적용펑션
public:
	void	InitAllItem ();
	void	SetReadMode ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonInvenfile();
	afx_msg void OnBnClickedButtonFieldfile();
	afx_msg void OnBnClickedButtonGrindingSimulator();

	afx_msg void OnBnClickedButtonSkinEffectfile();
	afx_msg void OnBnClickedButtonEffectfile();
	afx_msg void OnBnClickedButtonSkinTarEffectfile();
	afx_msg void OnBnClickedButtonGeneralEffectfile();
	afx_msg void OnEnChangeEditItemname();
	afx_msg void OnEnChangeEditItemdesc();

	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioMan();
	afx_msg void OnBnClickedRadioWoman();
	afx_msg void OnBnClickedButtonItemType();
	afx_msg void OnBnClickedButtonWearingSet();
	afx_msg void OnEnChangeEditInveniconX();
	afx_msg void OnEnChangeEditInveniconY();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonCheckInfo();
	afx_msg void OnBnClickedButtonDecompose();
};
