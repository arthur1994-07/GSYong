#pragma once

#include "afxcmn.h"

#include "../../RanLogic/Crow/GLCrowData.h"

class CMobSearchDialog : public CDialog
{
	DECLARE_DYNAMIC(CMobSearchDialog)

public:
	CMobSearchDialog ( CWnd* pParent = NULL );
	virtual ~CMobSearchDialog ();

	enum { IDD = IDD_DIALOG_MOB_SEARCH };

protected:
	virtual void DoDataExchange ( CDataExchange* pDX );

private:
	SNATIVEID	m_sSummonMobID;
	BOOL		m_bConfirm;

public:
	const SNATIVEID& GetSummonMobID ();

	const char*	GetFormatName ( PCROWDATA pItem );

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
};
