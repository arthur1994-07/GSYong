//
//	최초 작성자 : 성기엽
//		[2003.11.27]
//
#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

class	CBasicTextBox;
class	CBasicLineBoxEx;
class	CD3DFontPar;

class	CBasicVarTextBox : public CUIGroup
{
private:
	static	const	float	fMOUSEPOINT_GAP;

public:
	CBasicVarTextBox (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicVarTextBox ();

public:
	void	CreateSubControl ();

public:
	void	ClearText();
	int		AddTextNoSplit(const CString& strText, D3DCOLOR dwColor);
	int		SetTextNoSplit(const CString& strText, D3DCOLOR dwColor);
	void	SetText(const CString& strText, D3DCOLOR dwColor);
	void	AddString(int nIndex, const CString& strText, const D3DCOLOR& dwColor);
	int		AddText(const CString& strText, D3DCOLOR dwColor);
	void	AddTextLongestLineSplit(const CString& strText, D3DCOLOR dwColor);

public:
	void	SetTextAlign ( int nALIGN );
	void	SetUseOverColor ( bool bUseColor );
	void	SetOverColor ( const D3DCOLOR& dwColor );
	void	SetLineInterval ( const float fLineInterval );
	void	SetBlockMouseTracking ( bool bBlock );
	void	SetMousePointGap (const D3DXVECTOR2& vGap );

public:
	int		GetCount ();
	float	GetLongestLine ();
	UIRECT	CalcSize();

public:
	virtual void RePosControl ( int x, int y );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

protected:
	CBasicTextBox*		m_pTextBox;
	CBasicLineBoxEx*	m_pLineBox;
	D3DXVECTOR2			m_vMousePointGap;

	CUIControl*			m_pSelfDummy;
	CD3DFontPar*		m_pFont9;

	bool				m_bBLOCK_MOUSETRACKING;
};