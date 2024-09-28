#include "stdafx.h"
#include "UIWebView.h"
#include <GUIBase/UIEventManager.h>

#include <WebKit2/WKContextPrivate.h>
#include <WebKit2/WKURLCF.h>
#include <Webkit2/WebKit2_C.h>

namespace HTML5UI
{

/// 문자열 변환;
static std::string toSTD(WKStringRef string)
{
	size_t bufferSize = WKStringGetMaximumUTF8CStringSize( string );
	char* buffer = new char[ bufferSize ];
	size_t stringLength = WKStringGetUTF8CString( string, buffer, bufferSize );
	return std::string( buffer, stringLength - 1 );
}

/// UI Client Callbacks
// 새로운 윈도우 페이지를 생성;
static WKPageRef createNewPage(WKPageRef page, WKURLRequestRef request, WKDictionaryRef features, WKEventModifiers modifiers, WKEventMouseButton mouseButton, const void* clientInfo)
{
	return 0;
}

// 페이지를 보여줌;
static void showPage(WKPageRef page, const void *clientInfo)
{
}

// 끄기;
static void closePage(WKPageRef page, const void *clientInfo)
{
}

// JavaScript Invoke
static void runJavaScriptCallback(WKPageRef page, WKStringRef message, WKFrameRef frame, const void* clientInfo )
{
	static_cast<GUIBase::IWindow*>( const_cast<void*>(clientInfo))->UIPageCallback( toSTD(message) );
}

// JavaScript Alert
static void runJavaScriptAlert(WKPageRef page, WKStringRef alertText, WKFrameRef frame, const void* clientInfo)
{
	MessageBox( 0, toSTD(alertText).c_str(), "alert", 0 );
}

// JavaScript Confirm
static bool runJavaScriptConfirm(WKPageRef page, WKStringRef message, WKFrameRef frame, const void* clientInfo)
{
	return false;
}

// JavaScript Prompt
static WKStringRef runJavaScriptPrompt(WKPageRef page, WKStringRef message, WKStringRef defaultValue, WKFrameRef frame, const void* clientInfo)
{
	return 0;
}

static void setStatusText(WKPageRef page, WKStringRef text, const void* clientInfo)
{
}

// 마우스가 엘리먼트 위로 올라갈 때의 콜백;
static void mouseDidMoveOverElement(WKPageRef page, WKHitTestResultRef hitTestResult, WKEventModifiers modifiers, WKTypeRef userData, const void *clientInfo)
{
}

/// Html View
CUIWebView::CUIWebView( GUIBase::UIID uiID, bool bTransparent )
: GUIBase::IWebView( uiID, bTransparent )
, m_webView( NULL )
, m_pWindow( NULL ) { }

CUIWebView::~CUIWebView()
{
	SAFE_DELETE( m_pWindow );
}

void CUIWebView::Create( GUIBase::IWindow* pWindow )
{
	// Setting HtmlWindow
	m_pWindow = pWindow;

	// Context 생성;
	static WKContextRef context = WKContextCreate();

	// View 생성;
	RECT rt = { 0, 0, 0, 0 };
	m_webView = WKViewCreate( rt, context, 0, pWindow );

	// UI Client 콜백 설정;
	WKPageUIClient uiClient = {
		kWKPageUIClientCurrentVersion,
		pWindow,	// clientInfo
		0,          // createNewPage_deprecatedForUseWithV0
		showPage,
		closePage,
		0,          // takeFocus
		0,          // focus
		0,          // unfocus
		runJavaScriptCallback,
		runJavaScriptAlert,
		runJavaScriptConfirm,
		runJavaScriptPrompt,
		setStatusText,
		0,          // mouseDidMoveOverElement_deprecatedForUseWithV0
		0,          // missingPluginButtonClicked
		//0,        // didNotHandleKeyEvent
		//0,        // didNotHandleWheelEvent
		0,          // toolbarsAreVisible
		0,          // setToolbarsAreVisible
		0,          // menuBarIsVisible
		0,          // setMenuBarIsVisible
		0,          // statusBarIsVisible
		0,          // setStatusBarIsVisible
		0,          // isResizable
		0,          // setIsResizable
		0,          // getWindowFrame
		0,          // setWindowFrame
		0,          // runBeforeUnloadConfirmPanel
		0,          // didDraw - 추가적으로 렌더링할 사항;
		0,          // pageDidScroll
		0,          // exceededDatabaseQuota
		0,          // runOpenPanel
		0,          // decidePolicyForGeolocationPermissionRequest
		0,          // headerHeight
		0,          // footerHeight
		0,          // drawHeader
		0,          // drawFooter
		0,			// printFrame
		0,          // runModal
		0,          // didCompleteRubberBandForMainFrame
		0,          // saveDataToFileInDownloadsFolder
		0,          // shouldInterruptJavaScript
		createNewPage,
		mouseDidMoveOverElement,
	};

	// View의 Page에 UI Client 콜백 세팅;
	WKPageSetPageUIClient( WKViewGetPage( m_webView ), &uiClient );

	// WebView Window 세팅;
	WKViewSetIsInWindow( m_webView, true );

	// 투명도 설정;
	WKViewSetDrawsTransparentBackground( m_webView, true );
}

void CUIWebView::SetFrame( GUIBase::UIRECT rect )
{
	if( !m_webView )
		return;

	RECT rt = { 0, 0, 0, 0 };
	rt.bottom = rect.bottom;
	rt.left = rect.left;
	rt.right = rect.right;
	rt.top = rect.top;

	GUIBase::UISIZE<float> size = GUIBase::UISIZE<float>(
		rt.right - rt.left,
		rt.bottom - rt.top );

	GUIBase::CUIResizeEvent* resizeEvent =
		new GUIBase::CUIResizeEvent( m_uiID,
		GUIBase::CUIEvent::UI_Size, size, size, 0.f );

	if( m_pWindow )
		m_pWindow->FireUIEvent( resizeEvent );
}

void CUIWebView::ChangeUI( const std::wstring& urlString )
{
	if( !m_webView )
		return;

	if( urlString.empty() )
		return;

	CFStringRef string = CFStringCreateWithCharacters(0, (const UniChar*)urlString.data(), urlString.size());
	CFStringRef escapedString = CFURLCreateStringByAddingPercentEscapes(0, string, 0, 0, kCFStringEncodingUTF8);
	CFRelease(string);
	CFURLRef cfURL = CFURLCreateWithString(0, escapedString, 0);
	CFRelease(escapedString);

	WKURLRef url = WKURLCreateWithCFURL(cfURL);
	CFRelease(cfURL); 

	WKPageRef page = WKViewGetPage(m_webView);
	WKPageLoadURL(page, url);
	WKRelease(url);
}

void CUIWebView::UpdateWebPage()
{
	WKPageRef page = WKViewGetPage(m_webView);
	WKPageReloadFromOrigin( page );
}

bool CUIWebView::GetImgUpdateData( GUIBase::UI_IMAGE_DATA& uiImgData )
{
	if( !m_webView )
		return false;

	bool bUpdate = WKViewGetImgUpdateData( m_webView, uiImgData );
	if( !bUpdate )
		return false;
	return true;
}

/// Event 처리;
void CUIWebView::OnDestroy()
{
	if( !m_webView )
		return;

	std::tr1::shared_ptr<GUIBase::CUIEvent> uiEvent(
		new GUIBase::CUIEvent( m_uiID,
		GUIBase::CUIEvent::UI_Destroy ) );
	WKViewUpdate( m_webView, uiEvent.get() );
}

void CUIWebView::OnClose()
{
	if( !m_webView )
		return;

	std::tr1::shared_ptr<GUIBase::CUIEvent> uiEvent(
		new GUIBase::CUIEvent( m_uiID,
		GUIBase::CUIEvent::UI_Close ) );
	WKViewUpdate( m_webView, uiEvent.get() );
}

void CUIWebView::OnSize( std::tr1::shared_ptr<GUIBase::CUIResizeEvent> resizeEvent )
{
	if( !m_webView )
		return;

	WKViewUpdate( m_webView, resizeEvent.get() );
}

void CUIWebView::OnFocus( std::tr1::shared_ptr<GUIBase::CUIFocusEvent> focusEvent )
{
	if( !m_webView )
		return;

	WKViewUpdate( m_webView, focusEvent.get() );
}

void CUIWebView::OnShow( std::tr1::shared_ptr<GUIBase::CUIShowEvent> showEvent )
{
	if( !m_webView )
		return;

	WKViewUpdate( m_webView, showEvent.get() );
}

void CUIWebView::OnHide( std::tr1::shared_ptr<GUIBase::CUIShowEvent> hideEvent )
{
	if( !m_webView )
		return;

	WKViewUpdate( m_webView, hideEvent.get() );
}

bool CUIWebView::OnKeyEvent( std::tr1::shared_ptr<GUIBase::CUIKeyEvent> keyEvent )
{
	if( !m_webView )
		return true;

	WKViewUpdate( m_webView, keyEvent.get() );

	return true;
}

bool CUIWebView::OnMouseEvent( std::tr1::shared_ptr<GUIBase::CUIMouseEvent> mouseEvent )
{
	if( !m_webView )
		return true;

	WKViewUpdate( m_webView, mouseEvent.get() );

	return true;
}

bool CUIWebView::OnWheelEvent( std::tr1::shared_ptr<GUIBase::CUIWheelEvent> wheelEvent  )
{
	if( !m_webView )
		return true;

	WKViewUpdate( m_webView, wheelEvent.get() );

	return true;
}

bool CUIWebView::OnInvokeEvent( std::tr1::shared_ptr<GUIBase::CUIArrayInvokeEvent> invokeEvent )
{
	if( !m_webView )
		return true;

	WKViewUpdate( m_webView, invokeEvent.get() );

	return true;
}

}