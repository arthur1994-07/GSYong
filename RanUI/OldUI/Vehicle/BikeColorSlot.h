#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CBasicTextBox;

class CBikeColorSlot : public CUIGroup
{
public:
	CBikeColorSlot(EngineDeviceMan* pEngineDevice);
	virtual	~CBikeColorSlot ();

public:
	void CreateSubControl ();

	CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	D3DCOLOR GetColor() { return m_vColor; }
	void SetColor( D3DCOLOR vColor );
	void SetSelect( BOOL bSelect );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	D3DCOLOR	m_vColor;

	CUIControl*	m_pSelect;
	CUIControl*	m_pColor;

};