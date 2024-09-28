#pragma	once

#include "UIGroup.h"

class CDCFontPar;
class CBasicCarrat;

// 기존의 형식을 따라가기 위해서 Bit로사용하지 않음;
enum
{
	EDIT_LEFT_ALIGN,
	EDIT_CENTER_ALIGN,
	EDIT_RIGHT_ALIGN,
	EDIT_CENTER_XY_ALIGN
};

class CUIEditBox : public CUIGroup
{
public:
	static const int	nDEFAULT_LIMIT_INPUT;
	static const float	fLINE_END_GAP;
	static const int	nNOT_USE_MONEY_FORMAT;

protected:
	static UIGUID		DEBUG_BEGIN_ID;
	static UIGUID		DEBUG_END_ID;

	bool				m_bModeNative;
	bool				m_bBeginEdit;
	bool				m_bLimited;
	int					m_nLimitInput;

	CBasicCarrat*		m_pCarrat;
	DWORD				m_dwCarratDiffuse;

	CString				m_strText;
	CString				m_strRenderText;
	CString				m_strRenderTextR;
	D3DXVECTOR2			m_vRenderPos;

	int					m_nTabIndex;
	BOOL				m_bHide;
	bool				m_bUseNumber;
	bool				m_bUseDecimalNumber;
	int					m_nMoneyUNIT;
	CString				m_strUNIT_SYMBOL;
	int					m_nAlign;
	D3DCOLOR			m_dwTextColor;
	bool				m_bVALID_INPUT;
	bool				m_bWideCarret;

protected:
	BOOL				m_bMinusSign;
	BOOL				m_bKeyInput;

protected:
	int					m_xViewPos;
	int					m_xInsertPos;
	int					m_xInsertPosR;
	float				m_fCarratPos;
	float				m_fCarratPosY;
	float				m_fCaretSizeX;

public:
	CUIEditBox (EngineDeviceMan* pEngineDevice);
	virtual	~CUIEditBox ();

public:
	void CreateCarrat (const char* szCarrat, BOOL bUseRender, DWORD dwDiffuse );
	virtual void SetFont ( CD3DFontPar* pFont );

	virtual void SetGlobalPos ( const UIRECT& rcPos );
	virtual void SetGlobalPos ( const D3DXVECTOR2& vPos );

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void UpdateRenderText();
	virtual void UpdatePosition ();

	virtual void GetInsertPos();
	virtual void SetInsertPos();

	const UIRECT& GetCarratGlobalPos();
	const UIRECT& GetCarratLocalPos();

	int MultiToWidePos( CString* strMulti, int iMultiPos );
	int WideToMultiPos( CStringW* strWide, int iWidePos );

protected:
	void FLUSH_STRING_BUFFER ();
	void SET_STRING_BUFFER ( const char* pStr );
	void INSERT_STRING_BUFFER ( const char* pStr );

public:
	virtual bool BeginEdit();
	bool IsBegin()							{ return m_bBeginEdit; }
	bool EndEdit();
	virtual void ClearEdit();
	bool IsLimited()						{ return m_bLimited; }
	bool IsEmpty()							{ return m_strText.IsEmpty(); }

public:
	bool IsVALID_INPUT()					{ return m_bVALID_INPUT; }

	void SetLimitInput( int nLimitInput )	{ m_nLimitInput = nLimitInput; }
	int	GetLimitInput()						{ return m_nLimitInput; }

	void DoMODE_TOGGLE();
	bool IsMODE_NATIVE()					{ return m_bModeNative; }
	void SetMODE_NATIVE( bool bNative )		{ m_bModeNative = bNative; }

	const CString& GetEditString() const	{ return m_strText; }
	int GetEditLength()						{ return m_strText.GetLength (); }

	void SetEditString ( const CString& strInput );
	void InsertString ( const CString& strInput );

	void DisableKeyInput()					{ m_bKeyInput = FALSE; }
	void UsableKeyInput()					{ m_bKeyInput = TRUE; }
	BOOL IsLimitedKeyInput()				{ return m_bKeyInput; } // 2차비번

public:
    virtual void SetDiffuseAlpha ( WORD wAlpha );
	virtual void SetRenderText(const CString& strRenderText ) { m_strRenderText = strRenderText; }
	virtual void SetMinusSign( BOOL bUse )				{ m_bMinusSign = bUse; }

	void SetTabIndex( int nTabIndex )		{ m_nTabIndex = nTabIndex; }
	int	GetTabIndex( void )					{ return m_nTabIndex; }

	virtual void SetHide( BOOL bHide )				{ m_bHide = bHide; }
	BOOL IsHide()							{ return m_bHide; }

	virtual void DoUSE_NUMBER( bool bUse )			{ m_bUseNumber = bUse; }
	virtual void DoUSE_DECIMALNUMBER( bool bUse )			{ m_bUseDecimalNumber = bUse; }
	bool IsUSE_NUMBER()						{ return m_bUseNumber; }
	bool IsUSE_DECIMALNUMBER()				{ return m_bUseDecimalNumber; }

	virtual bool SET_MONEY_FORMAT( int nMoneyUNIT, const CString& strUNIT_SYMBOL = "," );
	int	GET_MONEY_FORMAT()					{ return m_nMoneyUNIT; }
	CString	GET_MONEY_SYMBOL();

	void SetTextColor( const D3DCOLOR& dwTextColor )	{ m_dwTextColor = dwTextColor; }
	const D3DCOLOR&	GetTextColor () const	{ return m_dwTextColor; }

	void SetAlign( int nAlign )				{ m_nAlign = nAlign; }
	int GetAlign()							{ return m_nAlign; }
};