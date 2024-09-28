
#pragma once

#include "./AnimTabControl.h"

class CAnimPage;
class CsheetWithTabAnim;

class CAnimTabPage: public CPropertyPage
{
	DECLARE_DYNAMIC(CAnimTabPage)
protected:
	CsheetWithTabAnim	*m_pSheetTab;
	CAnimTabControl		m_tabAnim;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab );

	enum { IDD = IDD_DIALOG_ANIMTABPAGE };

public:
	CAnimTabPage(void);
	virtual ~CAnimTabPage(void);

public:
	CAnimPage*		GetAnimPage(void);
	CMultianimPage*	GetMultiAnimPage(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	// DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();
};