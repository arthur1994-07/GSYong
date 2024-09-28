#include "stdafx.h"
#include "GfxCore.h"

#include "../../RanThirdParty/GFx/Src/GFx/GFx_FontLib.h"
//#include "../../RanThirdParty/GFx/Src/GFx/GFx_FontProviderWin32.h"
#include "../../RanThirdParty/GFx/Src/GFx/GFx_FontProviderFT2.h"

#include "../enginelib/Common/FileSystem.h"
#include "./GfxRenderThread.h"

class GfxFileOpener : public Scaleform::GFx::FileOpener
{
public :
	GfxFileOpener() {}
	virtual ~GfxFileOpener() {}

public : 
	virtual Scaleform::File* OpenFile(
		const char* purl,
		int flags = FileConstants::Open_Read | FileConstants::Open_Buffered,
		int mode = FileConstants::Mode_ReadWrite );
};

Scaleform::File* GfxFileOpener::OpenFile( const char* purl, int flags, int mode )
{
	CString strPath = purl;

	int nExtPos = strPath.ReverseFind( '.' );
	if( -1 == nExtPos )
		return NULL;

	CString strExt = strPath.Right( strPath.GetLength() - nExtPos - 1 );
	if( 0 == strExt.GetLength() )
		return NULL;

	strExt = strExt.MakeLower();

	// MTF 로드 해본다.
	if( strExt == "dds" )
	{
		CString strMtfFileUTF8 = strPath.Left( nExtPos );
		strMtfFileUTF8 += ".mtf";

		std::string strMtfFileAnsi = sc::string::utf8_to_ansi( std::string( strMtfFileUTF8.GetBuffer() ) );

		UINT nDataSize = 0;
		CTypeSmartPtr spCTypeRawData = CFileSystem::getInstance().getFile( strMtfFileAnsi, nDataSize );
		if( NULL != spCTypeRawData )
			return new MemoryFile( strMtfFileUTF8.GetBuffer(), spCTypeRawData.ToBytePtr(), nDataSize );
	}
	else if( strExt == "mtf" )
	{
		UINT nDataSize = 0;
		CTypeSmartPtr spCTypeRawData = CFileSystem::getInstance().getFile( purl, nDataSize );
		if( NULL != spCTypeRawData )
			return new MemoryFile( purl, spCTypeRawData.ToBytePtr(), nDataSize );
	}

	return Scaleform::GFx::FileOpener::OpenFile( purl, flags, mode );
}

//-----------------------------------------------------------------------------------------------

// 4.3 sdk쪽 소스에서 경로 제대로 못잡는다.
// swf 경로 직접 잡아준다.
class GfxUrlBuilder : public Scaleform::GFx::URLBuilder
{
public :
	GfxUrlBuilder() {}
	virtual ~GfxUrlBuilder() {}

public : 
	virtual void BuildURL( String *ppath, const LocationInfo& loc );
};

void GfxUrlBuilder::BuildURL( String *ppath, const LocationInfo& loc )
{
	if( true == loc.ParentPath.IsEmpty() || true == IsPathAbsolute( loc.FileName ) )
	{
		*ppath = loc.FileName;
	}
	else
	{
		std::string strPath = NSGFXFILE::g_strGfxPath;
		strPath = sc::string::ansi_to_utf8( strPath );
		strPath.append( loc.FileName.ToCStr() );

		*ppath = String( strPath.c_str() );
	}
}

//-----------------------------------------------------------------------------------------------

class GfxClipboard : public GFx::Clipboard
{
public :
	GfxClipboard() {}
	virtual ~GfxClipboard() {}

	virtual void OnTextStore( const wchar_t* ptext, UPInt len );
};

void GfxClipboard::OnTextStore( const wchar_t* ptext, UPInt len )
{
	// 텍스트 복사
	//std::string str = sc::string::unicodeToAnsi( ptext, len );
	sc::string::stringToClipboard( ptext );
}

//-----------------------------------------------------------------------------------------------

GfxCore::GfxCore(void)
:	m_emFontLang( EMGFX_FONT_NONE )
,	m_bLostDevice( false )
{
}

GfxCore::GfxCore( const GfxCore& value )
{
}

GfxCore::~GfxCore(void)
{
	if ( m_pRenderHAL )
	{
		if( true == m_pRenderHAL->IsInitialized() )
			OnDestroyDevice();
	}

	m_pIME.Clear();
	m_pRenderHAL.Clear();
}

HRESULT GfxCore::OnCreateDevice( HWND hWnd, LPDIRECT3DDEVICE9 pDevice )
{
	m_GfxLoader.SetLog( Ptr< Scaleform::Log >( *new GFxPlayerLog() ) );
	m_GfxLoader.SetFileOpener( Ptr< GfxFileOpener >( *new GfxFileOpener() ) );
	m_GfxLoader.SetURLBuilder( Ptr< GfxUrlBuilder >( *new GfxUrlBuilder() ) );

	m_pTextClipboard = *new GfxClipboard();
	m_GfxLoader.SetClipboard( m_pTextClipboard );

	if( FAILED( InitRenderer( pDevice ) ) )
		return E_FAIL;
	if( FAILED( InitFont() ) )
		return E_FAIL;

	if( language::DEFAULT == RENDERPARAM::emLangSet ||
		language::KOREAN == RENDERPARAM::emLangSet )
	{
		if( FAILED( InitIME( hWnd ) ) )
			return E_FAIL;
	}

	// AS3 지원
	Ptr< GFx::ASSupport > pAS3Support = *new GFx::AS3Support();
	m_GfxLoader.SetAS3Support( pAS3Support );

	// AS 콜백 인터페이스
	m_pEIHandler = *new GfxExternalInterface;
	m_pEIHandler->InitCallbackFunc();
	m_GfxLoader.SetExternalInterface( m_pEIHandler );

	GlyphCacheParams gcParams;
	gcParams.TextureWidth = 1024;
	gcParams.TextureHeight = 1024;
	gcParams.NumTextures = 1;
	gcParams.MaxSlotHeight = 48;
	gcParams.SlotPadding = 2;
	gcParams.TexUpdWidth = 256;
	gcParams.TexUpdHeight = 512;
	m_pRenderThread->SetGlyphCacheParams( gcParams );

	ToleranceParams tolParams;
	m_pRenderThread->SetToleranceParams( tolParams );

	return S_OK;
}

HRESULT GfxCore::OnResetDevice( LPDIRECT3DDEVICE9 pDevice )
{
	if( NULL == m_pRenderHAL )
		return E_FAIL;

	D3DVIEWPORT9 vp;
	pDevice->GetViewport( &vp );

	// 최소화 상태다. 걍 리턴하자.
	//if( 10 == vp.Width && 10 == vp.Height )
	//	return S_OK;

	LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
	pDevice->CreateStateBlock( D3DSBT_ALL, &pStateBlock );
	pStateBlock->Capture();

	if( false == m_pRenderHAL->IsInitialized() )
	{
		m_pRenderHAL->InitHAL( D3D9::HALInitParams(	pDevice, m_PresentParams,
			Render::D3D9::HALConfig_NoSceneCalls, m_pRenderThread->GetThreadId() ) );
	}
	else
	{
		//if( true == m_bLostDevice )
		{
			m_pRenderHAL->RestoreAfterReset();
			//m_bLostDevice = false;
		}
	}

	pStateBlock->Apply();
	SAFE_RELEASE( pStateBlock );
	
	return S_OK;
}

HRESULT GfxCore::OnLostDevice()
{
	if( NULL == m_pRenderHAL )
		return E_FAIL;

	//if( false == m_bLostDevice )
	{
		m_pRenderHAL->PrepareForReset();
		//m_bLostDevice = true;
	}

	return S_OK;
}

HRESULT GfxCore::OnDestroyDevice()
{
	if( NULL != m_pRenderThread )
		m_pRenderThread->ExitThread();

	m_pRenderHAL->ShutdownHAL();

	return S_OK;
}

HRESULT GfxCore::InitRenderer( LPDIRECT3DDEVICE9 pDevice )
{
	m_pRenderThread = *new GfxRenderThread( (ThreadingType) TT_AutoDetect );
	m_pRenderThread->StartThread();

	// 렌더러 생성
	m_pRenderHAL = *new Render::D3D9::HAL( m_pRenderThread );
	m_pRenderThread->m_pDevice = pDevice;
	m_pRenderThread->m_pHAL = m_pRenderHAL;

	LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
	pDevice->CreateStateBlock( D3DSBT_ALL, &pStateBlock );
	pStateBlock->Capture();

	if( false == m_pRenderHAL->InitHAL( 
			D3D9::HALInitParams( pDevice, m_PresentParams, 
			Render::D3D9::HALConfig_NoSceneCalls, m_pRenderThread->GetThreadId() ) ) )
		return E_FAIL;

	pStateBlock->Apply();
	SAFE_RELEASE( pStateBlock );

	Ptr< GFx::ImageFileHandlerRegistry > pImgReg = 
		*new GFx::ImageFileHandlerRegistry( GFx::ImageFileHandlerRegistry::AddDefaultHandlers );
	m_GfxLoader.SetImageFileHandlerRegistry( pImgReg );

	return S_OK;
}

HRESULT GfxCore::InitFont()
{
	Ptr< GFx::FontProviderFT2 > pFontProvider = *new GFx::FontProviderFT2();

	// 폰트 설정 파일 로드
	if( false == LoadFontConfig( pFontProvider ) ) 
		return E_FAIL;

	m_GfxLoader.SetFontProvider( pFontProvider );

	// 폰트 라이브러리 생성
	Ptr< FontLib > pFontLib = *new FontLib;
	m_GfxLoader.SetFontLib( pFontLib );

	// 기본 폰트 라이브러리 설정
	std::string strFontLib = NSGFXFILE::g_strFontLib;
	strFontLib.append( ".swf" );
	m_GfxLoader.SetDefaultFontLibName( strFontLib.c_str() );

	FontConfig* pFontConfig = GetCurrentFontConfig();
	if( NULL != pFontConfig )
	{
		std::vector< FontConfig::SFontFile >::iterator iter = pFontConfig->vecFontFile.begin();
		std::vector< FontConfig::SFontFile >::iterator iterEnd = pFontConfig->vecFontFile.end();
		for( ; iter != iterEnd; ++iter )
		{
			const FontConfig::SFontFile& sFontFile = (*iter);

			std::string strPath = NSGFXFILE::g_strGfxPath;
			strPath.append( sFontFile.strFile.c_str() );
			pFontProvider->MapFontToFile( sFontFile.strName.c_str(), sFontFile.dwFontFlag, 
				strPath.c_str(), 0, Font::HintAll, Font::HintAll );
		}

		pFontConfig->Apply( &m_GfxLoader );
	}

	return S_OK;
}

HRESULT GfxCore::InitIME( HWND hWnd )
{
	// IME 생성
	std::string strIMEPathName = NSGFXFILE::g_strGfxPath;
	strIMEPathName.append( NSGFXFILE::g_strIMEFileName.c_str() );
	strIMEPathName = sc::string::ansi_to_utf8( strIMEPathName );

	std::string strIMEXMLName = NSGFXFILE::g_strGfxPath;
	strIMEXMLName.append( "\\ime.xml" );
	strIMEXMLName = sc::string::ansi_to_utf8( strIMEXMLName );

	m_pIME = *new GFx::IME::GFxIMEManagerWin32( hWnd );
	m_pIME->Init( m_GfxLoader.GetLog(), m_GfxLoader.GetFileOpener(), strIMEXMLName.c_str(), true );
	m_pIME->SetIMEMoviePath( strIMEPathName.c_str() );
	m_GfxLoader.SetIMEManager( m_pIME );

	m_pIME->SetEnabled( false );

	return S_OK;
}

bool GfxCore::LoadFontConfig( GFx::FontProviderFT2* pFontProvider )
{
	// fontconfig.txt 로드, 파싱
	std::string strFontConfigPathName = NSGFXFILE::g_strGfxPath;
	strFontConfigPathName.append( NSGFXFILE::g_strFontConfigFileName );
	strFontConfigPathName = sc::string::ansi_to_utf8( strFontConfigPathName );

	if( false == Scaleform::SysFile::GetFileStat( &m_sFontConfigFileStats, strFontConfigPathName.c_str() ) )
		return false;

	ConfigParser parser( strFontConfigPathName.c_str() );

	m_FontConfigs.Parse( &parser, pFontProvider );
	if( m_FontConfigs.GetSize() <= 0 )
		return false;

	return true;
}

FontConfig* GfxCore::GetCurrentFontConfig()
{
	if( EMGFX_FONT_NONE == m_emFontLang ) 
		return NULL;

	FontConfig* pFontConfig = NULL;
	int  nIndex = (int) m_emFontLang;
	bool bOK = true;

	pFontConfig = m_FontConfigs[ nIndex ];

	// fontlib 파일이 존재하는지 확인
	for( unsigned i = 0; i < pFontConfig->FontLibFiles.GetSize(); ++i )
	{
		Scaleform::SysFile file( pFontConfig->FontLibFiles[ i ] );
		if( false == file.IsValid() )
		{
			bOK = false;
			fprintf( stderr, "Fontlib file '%s' cannot be found. Skipping config '%s'..\n",
				pFontConfig->FontLibFiles[ i ].ToCStr(), pFontConfig->ConfigName.ToCStr() );
			break;
		}
	}

	if( false == bOK )
		return NULL;

	return m_FontConfigs[ nIndex ];
}

void GfxCore::StorePresentParameters( D3DPRESENT_PARAMETERS pp )
{
	m_PresentParams = pp;
}