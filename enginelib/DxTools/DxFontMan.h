#ifndef _DX_FONT_MAN_H_
#define _DX_FONT_MAN_H_

#pragma once

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <map>

#include "../../InternalCommonLib/dxstdafx.h"
#include "DxFontDefine.h"

class CD3DFontPar;

//extern char _DEFAULT_FONT[128];
extern std::string _DEFAULT_FONT;
extern DWORD _DEFAULT_FONT_FLAG;
extern DWORD _DEFAULT_FONT_SHADOW_FLAG;
extern DWORD _DEFAULT_FONT_SHADOW_EX_FLAG;
extern DWORD _DEFAULT_FONT_BOLD_FLAG;

#define STRING_NUM80	80

class CD3DFontPar
{
public:
	static int			nCodePage[language::LANG_NUM];
	static int			nCharSet[language::LANG_NUM];
	static char			szDefaultFontName[language::LANG_NUM][50];
	static TCHAR		szEnglishFontName[language::LANG_NUM][20];
	static char			szLocaleName[language::LANG_NUM][50];
	//CD3DFontPar*		pNextFont;
	//  [1/8/2014 gbgim];
	static WCHAR		szInstallLayoutOrTipName[language::LANG_NUM][250];
	static char			szVistaKeyboardLayout[language::LANG_NUM][250];
	static char			szKeyboardLayout[language::LANG_NUM][250];
	static HKL			HklList[language::LANG_NUM][10];

protected:
	int					m_nReference;

	language::LANGFLAG	m_emLangFlag;
	TCHAR				m_strFontName[STRING_NUM80];
	DWORD				m_dwFontHeight;
	DWORD				m_dwFontFlags;

	BOOL				m_bUsageCS;
	CRITICAL_SECTION	m_csLock;

public:
	static int GetCodePage(language::LANGFLAG nLang);
	static BOOL IsExistenceKeyboardLayout( const language::LANGFLAG nLang, const HKL hKeyboardLayout );
public:
	int		AddRef()					{ return ++m_nReference; }
	int		SubRef()					{ return --m_nReference; }

	TCHAR*	GetFontName()				{ return m_strFontName; }
	DWORD	GetFontHeight()				{ return m_dwFontHeight; }
	DWORD	GetFontFlags()				{ return m_dwFontFlags; }

	VOID	UsageCS( BOOL bUsageCS )	{ m_bUsageCS = bUsageCS; }

	inline VOID AddFlags( DWORD dwFlags )	{ m_dwFontFlags |= dwFlags; }
	inline VOID SubFlags( DWORD dwFlags )	{ m_dwFontFlags &= ~dwFlags; }

public:
	virtual INT		MultiToWide( const TCHAR* strText, INT iText, WCHAR* wstrText, INT iWText );
	virtual INT		WideToMulti( const WCHAR* wstrText, INT iWText, TCHAR* strText, INT iText );

	virtual VOID	PushText( const TCHAR* strText )							{}
	virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor,
		const TCHAR* strText, DWORD dwFlags = 0L, BOOL bImmediately = TRUE )				{ return S_OK; }
	virtual HRESULT GetTextExtent( const TCHAR* strText, SIZE &Size )						{ return S_OK; }
	virtual HRESULT GetTextExtent( const TCHAR* strText, const DWORD dwFlag, SIZE &Size )	{ return S_OK; }

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )			{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects()										{ return S_OK; }
	virtual HRESULT InvalidateDeviceObjects()									{ return S_OK; }
	virtual HRESULT DeleteDeviceObjects()										{ return S_OK; }

	virtual int		GetTextCountByWidth ( const TCHAR* strText, const float fWidth, const float fRangePer=0.0f ) { return 0; } // 폭 사이즈로 해당되는 텍스트의 갯수를 구한다

public:
	virtual HRESULT Create( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual HRESULT CleanUp() = 0;

public:
	CD3DFontPar()
		: m_bUsageCS( FALSE )
		, m_emLangFlag(language::DEFAULT)
		, m_dwFontHeight( 0 )
		, m_dwFontFlags( 0 )
		, m_nReference( 0 )
        //, pNextFont( NULL )
	{
		memset( m_strFontName, 0, sizeof(TCHAR)*STRING_NUM80 );
		InitializeCriticalSection( &m_csLock );
	}

    virtual ~CD3DFontPar()
	{
		DeleteCriticalSection( &m_csLock );
		//SAFE_DELETE( pNextFont );
	}

	language::LANGFLAG GetEmLang(){return m_emLangFlag;}
};

class DxFontMan
{
protected:
	enum EMFONTSYS { EMFONT_D3D, EMFONT_D3DX };

public:
	static LPD3DXSPRITE	g_pSprite;

protected:
	std::string			m_strPATH;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
    typedef boost::tuple<std::string, DWORD, DWORD> FONT_TUPLE;
    typedef std::map<FONT_TUPLE, CD3DFontPar*>      FONT_MAP;
    typedef FONT_MAP::iterator                      FONT_MAP_ITER;
    typedef FONT_MAP::const_iterator                FONT_MAP_CITER;
    typedef FONT_MAP::value_type                    FONT_MAP_VALUE;	
    FONT_MAP            m_FontData;
    //CD3DFontPar*		m_pFontListHead;

	EMFONTSYS			m_emFontSys;
	DWORD				m_dwBufferMode;

	language::LANGFLAG m_emLang;

protected:
	DxFontMan(void);
	virtual ~DxFontMan(void);

protected:
	//inline BOOL IsSameFont( CD3DFontPar *pD3dFontSrc, const TCHAR* szFontName, DWORD dwHeight, DWORD dwFlags );

public:
    CD3DFontPar* LoadDxFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags=0L);
    void DeleteDxFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags=0L);
	void DeleteDxFontAll();

    CD3DFontPar* FindFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags);

	void AddRef(CD3DFontPar* pD3dFont);
    void ReleaseDxFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags=0L);
	void ReleaseDxFont(CD3DFontPar *pD3dFont);

public:
	HRESULT SetFontSys(const CString& strFontSys, language::LANGFLAG dwLang );
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();

public:
	void SetPath( const char *szPath )	{ m_strPATH = szPath; }
	const char* GetPath()				{ return m_strPATH.c_str(); }
	int GetCodePage()					{ return CD3DFontPar::GetCodePage(m_emLang); }
	bool IsXFont()						{ return ( m_emFontSys == EMFONT_D3DX ); }

public:
	static DxFontMan& GetInstance();
};

namespace COMMENT
{
	extern std::string FONTSYS[2];
}

#endif // _DX_FONT_MAN_H_