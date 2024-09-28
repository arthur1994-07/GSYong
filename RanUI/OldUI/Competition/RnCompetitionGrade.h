#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CTipBox;
class CInnerInterface;
enum EM_INSTANCE_TYPE_MAIN;

class RnCompetitionInstanceTypeMain : public CUIGroup
{
private:
	enum
	{
		INSTANCE_TYPE_BACK = NO_ID + 1,
		INSTANCE_TYPE_TEXT,
		INSTANCE_TYPE_OPEN,
		INSTANCE_TYPE_CLOSE,
		INSTANCE_TYPE_OVER,
		INSTANCE_TYPE_CLOSE_OVER,
	};

public:
	RnCompetitionInstanceTypeMain(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~RnCompetitionInstanceTypeMain ();

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
public:
	std::string m_strType;
};