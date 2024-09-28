#pragma once

#include <string>
#include <vector>


enum EMCURSOR
{
	CURSOR_NORMAL	 = 0,
	CURSOR_ATTACK	 = 1,
	CURSOR_HAND		 = 2,
	CURSOR_SCROLL	 = 3,
	CURSOR_TALK		 = 4,
	CURSOR_NONATTACK = 5,
	CURSOR_POSTBOX	 = 6,
	CURSOR_DECOMPOSE = 7,
};

class CCursor
{
public:
	typedef std::vector<HCURSOR>	VECCURSOR; // MEMO

protected:
	BOOL				m_bGameCursor;
	EMCURSOR			m_emType;
	BOOL				m_bShowCursor;

	std::string			m_strCurNormal;
	std::string			m_strCurAttack;
	std::string			m_strCurSelect;
	std::string			m_strCurScroll;
	std::string			m_strCurTalk;
	std::string			m_strCurNonAttack;
	std::string			m_strCurPostBox;
	std::string			m_strCurDecompose;

	std::string			m_strPath;
	HWND				m_hWnd;

	HCURSOR				m_hCurDefault;
	HCURSOR				m_hCurNormal;
	VECCURSOR			m_vecCurAttack;

	HCURSOR				m_hCurSelect;
	HCURSOR				m_hCurScroll;
	HCURSOR				m_hCurTalk;
	HCURSOR             m_hCurNonAttack;
	HCURSOR				m_hCurPostBox;
	HCURSOR				m_hCurDecompose;

public:
	void SetGameCursor ( BOOL _bCursor );
	void SetCursorType ( EMCURSOR emType );
	EMCURSOR GetCurCursorType() const { return m_emType; }
	HCURSOR GetCurCursor ();
	void SetShowCursor ( BOOL bShow );
	BOOL GetShowCursor();

	void SetCursorNow ();

public:
	HRESULT OneTimeSceneInit ( char *szPath, HWND hWnd );
	HRESULT InitDeviceObjects();
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pD3DDevice, int x, int y );

protected:
	CCursor(void);

public:
	~CCursor(void);

public:
	static CCursor& GetInstance();
};
