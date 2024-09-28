#pragma once

#include "../GfxDef.h"
#include "../GfxExternalInterface.h"
#include "./GfxRenderThread.h"
#include "./GfxFontConfigParser.h"
#include "./CLIKDataBinding.h"

class GfxCore
{
public :
	GfxCore(void);
	GfxCore( const GfxCore& value );
	~GfxCore(void);

protected :
	Scaleform::GFx::System	m_GfxSystem;
	Scaleform::GFx::Loader	m_GfxLoader;

	Ptr< GfxRenderThread >					m_pRenderThread;
	Ptr< Scaleform::Render::D3D9::HAL >		m_pRenderHAL;
	Ptr< GfxExternalInterface >				m_pEIHandler;
	Ptr< GFx::IME::GFxIMEManagerWin32 >		m_pIME;
	Ptr< Scaleform::GFx::Clipboard >		m_pTextClipboard;

	D3DPRESENT_PARAMETERS					m_PresentParams;
	FileStat								m_sFontConfigFileStats;
	FontConfigSet							m_FontConfigs;
	bool									m_bLostDevice;

protected :
	EMGFX_FONT_LANG		m_emFontLang;		// 폰트 언어

public :
	virtual HRESULT OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pDevice );
	virtual HRESULT OnResetDevice( LPDIRECT3DDEVICE9 pDevice );
	virtual HRESULT OnLostDevice();
	virtual HRESULT OnDestroyDevice();

protected :
	HRESULT InitRenderer( LPDIRECT3DDEVICE9 pDevice );
	HRESULT InitFont();
	HRESULT InitIME( HWND hWnd );

	// 폰트 설정파일 로드
	bool LoadFontConfig( GFx::FontProviderFT2* pFontProvider );
	// 현재 폰트
	FontConfig* GetCurrentFontConfig();

public :
	// D3DPRESENT_PARAMETERS 저장
	void StorePresentParameters( D3DPRESENT_PARAMETERS pp );

	// 폰트 언어 설정
	void SetFontLanguage( EMGFX_FONT_LANG emLang ) { m_emFontLang = emLang; }
	
public :
	Scaleform::GFx::Loader& GetLoader() { return m_GfxLoader; }

	GfxRenderThread* GetRenderThread() { return m_pRenderThread; }
	Scaleform::Render::D3D9::HAL* GetRenderHAL() { return m_pRenderHAL; }
	GfxExternalInterface* GetExternalInterface() { return m_pEIHandler; }
	GFx::IME::GFxIMEManagerWin32* GetIME() { return m_pIME; }
	GFx::Clipboard* GetTextClipboard() { return m_pTextClipboard; }

	const D3DPRESENT_PARAMETERS& GetPresentParams() { return m_PresentParams; }
	bool IsLostDevice() { return m_bLostDevice; }
};