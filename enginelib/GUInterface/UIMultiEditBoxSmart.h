#pragma once

#include "../GUInterface/UIEditBox.h"

#include <vector>

struct SMULTISTRINGINFO
{
	CString		Data;
	D3DXVECTOR2	Pos;

	SMULTISTRINGINFO()
		: Data ( "" )
		, Pos  ( D3DXVECTOR2(0.0f,0.0f) )
	{
	}
};

class CBasicScrollBarEx;

class CUIMultiEditBoxSmart : public CUIEditBox
{

protected:
	static const UINT nDEFAULT_SCROLL_BORDER = 9;

protected:
	enum
	{
		MULTIEDITBOX_NONE = NO_ID,
		MULTIEDITBOX_SCROLLBAR,
	};

protected:
	typedef std::vector<SMULTISTRINGINFO>		MULTISTRING;
	typedef MULTISTRING::iterator				MULTISTRINGITER;
	typedef MULTISTRING::const_iterator			MULTISTRINGCITER;

protected:
	INT		m_nMousePosX;
	INT		m_nMousePosY;

	UINT	m_nVScrollBorder;
	UINT	m_nHScrollBorder;

	INT		m_nVertical;

	CString m_strCarrotPos;
	CString m_strCarrotPosR;

	float	m_fCarratPosY;
	LONG	m_nViewPosX;
	LONG	m_nViewPosY;

	bool	m_bUpdateChar;	  // 입력 키 업데이트 필요시
	UINT	m_nUpdateKeyDown; // 입력 키 이외의 업데이트 필요시

	bool	m_bVScroll;	 // 수직 스크롤
	bool	m_bHScroll;	 // 수평 스크롤 미구현
	bool	m_bAutoTurn; // 자동개행

protected:
	MULTISTRING m_MultiString;
	MULTISTRING m_MultiRenderString;

protected:
	CBasicScrollBarEx* m_pVScrollBar;

public:
	CUIMultiEditBoxSmart(EngineDeviceMan* pEngineDevice);
	virtual ~CUIMultiEditBoxSmart ();

public:
	virtual void	CreateScrollVertical ( const UINT nBorder=nDEFAULT_SCROLL_BORDER );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual void	UpdateRenderText();
	virtual void	UpdatePosition ();

public:
	virtual bool	BeginEdit();
	virtual bool	EndEdit();
	virtual void	ClearEdit();

public:
	virtual int		GetEditWidth  ();
	virtual int		GetEditHeight ();

public:
	virtual void	SetInsertPos();

public:
	virtual void	MouseDownLButtonToEdit ( const int nPosX, const int nPosY );

public:
	INT				WideToMultiPos ( CString& strSource, INT iWidePos );

public:
	void			SetAutoTurn		  ( const bool bEnable ) { m_bAutoTurn = bEnable; } // 자동개행
	void			SetScrollVertical ( const bool bEnable ); // 수직 스크롤
	void			SetbUpdateChar( const bool bBool ) { m_bUpdateChar = bBool; } // 키보드를 입력받지않고 화면 업데이트시 사용

protected:
	void			UpdateMultiString		( const CString& strRender, MULTISTRING& strMult );
	void			UpdateMultiRenderString ( MULTISTRING& strMulti );
	void			UpdateScrollByCarrat	( MULTISTRING& strMulti, const int nMousePosX, const int nMousePosY );

protected:
	void			GetLineStringByIndex	( const CString& strIn, CString& strOut, int nIndex );

protected:
	void			GetIndexPosition		( const MULTISTRING& strMulti, const int nIndex, int& nPosX, int& nPosY );  // X는 라인의 문자열 인덱스, Y는 해당 라인의 넘버
	int				SetCarratPosition		( MULTISTRING& strMulti, const int nPosX, const int nPosY );				// X는 라인의 문자열 인덱스, Y는 해당 라인의 넘버
};

