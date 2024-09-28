#pragma once

#include "IWindow.h"
#include "UIImageData.h"

namespace GUIBase
{

class IWebView
{
public:
	IWebView( UIID uiID, bool bTransparent = true ) : m_uiID( uiID ) { }
	virtual ~IWebView() { }

public:
	virtual void ChangeUI( const std::wstring& url ) = 0;
	virtual void Create( IWindow* window ) = 0;
	virtual void SetFrame( UIRECT ) = 0;

	/// Event 처리;
	virtual void OnDestroy() = 0;			// 윈도우의 완전한 파괴;
	virtual void OnClose() = 0;				// 윈도우 닫기;

	/// Event 처리;
	virtual void OnSize( std::tr1::shared_ptr<CUIResizeEvent> resizeEvent ) = 0;
	virtual void OnFocus( std::tr1::shared_ptr<CUIFocusEvent> focusEvent ) = 0;
	virtual void OnShow( std::tr1::shared_ptr<CUIShowEvent> showEvent ) = 0;
	virtual void OnHide( std::tr1::shared_ptr<CUIShowEvent> hideEvent ) = 0;
	virtual bool OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent ) = 0;
	virtual bool OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent ) = 0;
	virtual bool OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent ) = 0;
	virtual bool OnInvokeEvent( std::tr1::shared_ptr<CUIArrayInvokeEvent> invokeEvent ) = 0;

public:
	virtual void UpdateWebPage() = 0;

public:
	virtual bool GetImgUpdateData( UI_IMAGE_DATA& uiImgData ) = 0;

protected:
	UIID m_uiID;			// UI ID

	bool m_bTransparent;	// 백그라운드 투명도;
};

}