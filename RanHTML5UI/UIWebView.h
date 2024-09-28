#pragma once

#include <GUIBase/IWebView.h>
#include <queue>

struct OpaqueWKView;

namespace HTML5UI
{

class CUIWebView : public GUIBase::IWebView
{
public:
	CUIWebView( GUIBase::UIID uiID, bool bTransparent = true );
	virtual ~CUIWebView();

public:
	virtual void ChangeUI( const std::wstring& url );
	virtual void Create( GUIBase::IWindow* pWindow );
	virtual void SetFrame( GUIBase::UIRECT );

	/// Event 처리;
	virtual void OnDestroy();			// 윈도우의 완전한 파괴;
	virtual void OnClose();				// 윈도우 닫기;
	virtual void OnSize( std::tr1::shared_ptr<GUIBase::CUIResizeEvent> resizeEvent );
	virtual void OnFocus( std::tr1::shared_ptr<GUIBase::CUIFocusEvent> focusEvent );
	virtual void OnShow( std::tr1::shared_ptr<GUIBase::CUIShowEvent> showEvent );
	virtual void OnHide( std::tr1::shared_ptr<GUIBase::CUIShowEvent> hideEvent );
	virtual bool OnKeyEvent( std::tr1::shared_ptr<GUIBase::CUIKeyEvent> keyEvent );
	virtual bool OnMouseEvent( std::tr1::shared_ptr<GUIBase::CUIMouseEvent> mouseEvent );
	virtual bool OnWheelEvent( std::tr1::shared_ptr<GUIBase::CUIWheelEvent> wheelEvent );
	virtual bool OnInvokeEvent( std::tr1::shared_ptr<GUIBase::CUIArrayInvokeEvent> invokeEvent );

public:
	virtual void UpdateWebPage();

public:
	virtual bool GetImgUpdateData( GUIBase::UI_IMAGE_DATA& uiImgData );

private:
	const OpaqueWKView* m_webView;

	GUIBase::IWindow* m_pWindow;
};

}