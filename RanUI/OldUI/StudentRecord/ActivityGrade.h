#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CTipBox;
class CInnerInterface;

class ActivityGrade : public CUIGroup
{
private:
	enum
	{
		SPEC_ACT_GRADE_BACK = NO_ID + 1,
		SPEC_ACT_GRADE_TEXT,
		SPEC_ACT_GRADE_OPEN,
		SPEC_ACT_GRADE_CLOSE,
		SPEC_ACT_GRADE_OPEN_OVER,
		SPEC_ACT_GRADE_CLOSE_OVER,
	};

public:
	ActivityGrade(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~ActivityGrade ();

protected:
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	bool	IsOpen();
	void	Open();
	void	Close();

	void	SetText( CString strText );

	void	MouseOver( bool bOver );

//	void	SetID( DWORD dwID, bool bOpen = false );
//	DWORD	GetID()	{	return m_dwID;	}

private:
	CBasicTextBox*	m_pText;
	CUIControl*		m_pOpen;
	CUIControl*		m_pClose;
	CUIControl*		m_pOpenOver;
	CUIControl*		m_pCloseOver;

	CTipBox*		m_pBack;

private:
//	DWORD			m_dwID;
};