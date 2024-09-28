//	BasicScrollBarEx Ŭ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ :
//	�α�
//		[2003.12.1]
//			@ ��������

#pragma	once

#include "BasicTextBox.h"

class CBasicTextBoxEx : public CBasicTextBox
{
public:
static	const int nNOT_VISIBLE;

public:
	CBasicTextBoxEx(EngineDeviceMan* pEngineDevice);
	virtual	~CBasicTextBoxEx ();

public:
	void	CreateMouseOver  ( D3DCOLOR dwMouseOverLineColor );
	void	CreateMouseClick ( D3DCOLOR dwMouseOverLineColor );

public:
	virtual	void	ClearText ();

protected:
	void	UpdateMousePos ( int x, int y );

public:
	void	UpdateMouseOverImage ();
	void	UpdateMouseClickImage ();

	int		GetSelectPos ();
	void	SetSelectPos ( int nIndex ) { m_nMouseOverLine = nIndex; }

public:
	void	SetSensitive ( const bool& bSensitive );	
	void	SetSensitiveView ( const bool& bSensitive );
	void	SetSelectInit( const bool& bSelectInit );

public:
    void    SetMouseOverColor ( D3DCOLOR dwMouseOverLineColor ) { m_dwMouseOverImageColor = dwMouseOverLineColor; }

public:
    bool    IsUsingMouseOver ()  { return !(!m_pMouseOverImage); }
    bool    IsUsingeMouseClick() { return !(!m_pMouseClickImage); }

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*	m_pMouseOverImage;
	D3DCOLOR	m_dwMouseOverImageColor;

	CUIControl* m_pMouseClickImage;

	bool	m_bSensitive;
	bool	m_bSensitiveView;

private:
	int		m_nMouseOverLine;

	int		m_nSELECTED_INDEX;

	bool	m_bSelectInit;
};