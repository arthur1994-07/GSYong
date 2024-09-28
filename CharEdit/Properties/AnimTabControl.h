
#pragma once

class CAnimPage;
class CMultianimPage;
class CsheetWithTabAnim;

class CAnimTabControl : public CTabCtrl
{
public:
	enum { 
		ATC_ANIMPAGE = 0,
		ATC_MULTIANIMPAGE,
		ATC_NUMBEROFPAGE
	};

	CDialog*	m_tabPages[ATC_NUMBEROFPAGE];
	INT			m_tabCurrent;
	
	CsheetWithTabAnim	*m_pSheetTab;

public:
	CAnimTabControl(void);
	virtual ~CAnimTabControl(void);

public:
	void Init (void);

	CAnimPage*		GetAnimPage(void);
	CMultianimPage*	GetMultiAnimPage(void);
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	void SetRectangle();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};