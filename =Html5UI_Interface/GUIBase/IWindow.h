#pragma once

#include "IUIComponent.h"
#include "UIImageData.h"
#include "UIJsonParser.h"
#include "UICallbackMessage.h"

#include <list>
#include <queue>

namespace GUIBase
{

class IWebView;
class CUIArrayInvokeEvent;

class IWindow : public IUIComponent
{
public:
	typedef std::list< UIID >				WINDOW_LIST;
	typedef WINDOW_LIST::iterator			WINDOW_LIST_ITER;
	typedef WINDOW_LIST::reverse_iterator	WINDOW_LIST_RITER;
	typedef WINDOW_LIST::const_iterator		WINDOW_LIST_CITER;

public:
	IWindow( UIID uiID
		, const std::string& uiStrID
		, UIRECT rcParentGlobalPos
		, WINDOW_CFG windowCFG
		, IWebView* webView );	
	virtual ~IWindow();

public:
	// Callback Message를 위한 MsgProc;
	virtual bool MsgProc( const CALLBACK_MESSAGE msg ) = 0;

	virtual void Initialize() { }
	virtual void Destroy() { }
	virtual bool FrameMove( float timeDelta ) { return true; }
	virtual bool Render( float timeDelta );

public:
	/// 기본적으로 이루어져야 하는 부분;
	void UIInitialize();
	void UIDestroy();
	bool UIFrameMove( float timeDelta );
	bool UIRender( float timeDelta );

public:
	/// Event 처리;
	virtual void OnSize( std::tr1::shared_ptr<CUIResizeEvent> resizeEvent );
	virtual void OnFocus( std::tr1::shared_ptr<CUIFocusEvent> focusEvent );
	virtual void OnShow( std::tr1::shared_ptr<CUIShowEvent> showEvent );
	virtual void OnHide( std::tr1::shared_ptr<CUIShowEvent> hideEvent );
	virtual bool OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent );
	virtual bool OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent );
	virtual bool OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent );
	virtual bool OnInvokeEvent( std::tr1::shared_ptr<CUIInvokeEvent> invokeEvent );

public:
	/// Virtual 함수;
	virtual void ShowComponent();
	virtual void HideComponent();

public:
	/// Event 발생 함수;
	// EventManager에서 발생한 Event가 들어와서 Window에;
	// 맞을 경우 발생 한다;
	// Invoke, Mouse, Keyboard, Wheel 등의 이벤트 전달;
	virtual bool FireMouseEvent( CUIMouseEvent* mouseEvent );
	virtual bool FireWheelEvent( CUIWheelEvent* wheelEvent );
	virtual bool FireKeyEvent( CUIKeyEvent* keyEvent );
	virtual bool FireInvokeEvent( Json::Value& root );

public:
	/// Member 함수;
	// WebPage Update
	void UpdateWebPage();

	// Event 등록;
	void FireUIEvent( CUIEvent* uiEvent );

	// Event 처리;
	void EventProc();

	// Array Invoke;
	// 등록된 Invoke들을 하나로 합쳐서 보냄; 
	void FireArrayInvokeEvent();

	// Callback Message 처리;
	void UIPageCallback( const std::string& message );

	// 자식 윈도우 추가;
	bool AddChildWindowID( UIID uiID );

	// 자식 윈도우 반환;
	UIID GetChildWindowID();

private:
	/// Private 함수;
	// Update Image Data
	bool LoadImgUpdateData();

public:
	/// Inline 함수;
	// 윈도우를 Block;
	inline void BlockWindow() { m_bBlock = true; }
	inline void UnblockWindow() { m_bBlock = false; }
	inline bool GetBlockState() { return m_bBlock; }

	inline const UIPOINT<int>& GetMouseGap() const { return m_mouseGap; }

protected:
	// 자식 윈도우 ID 관리;
	// 윈도우 자체는 WindowManager에서 관리한다;
	WINDOW_LIST m_windowLists;

	//! URL
	std::wstring m_url;

	//! WebPage에서 들어오는 Message Queue;
	std::queue< CALLBACK_MESSAGE > m_msgQueue;

	//! WebPage로 보내는 Event Queue;
	std::queue< std::tr1::shared_ptr< CUIEvent > > m_pEventQueue;

	//! Array Invoke Data;
	std::vector< std::tr1::shared_ptr< CUIInvokeEvent > > m_invokeEvents;

	//! WebKit과 연동되는 View;
	std::tr1::shared_ptr< IWebView > m_webView;

	//! Image Data
	UI_IMAGE_DATA m_uiImgData;

	//! Block
	bool m_bBlock;

	//! Drag
	UIPOINT<int> m_mouseGap;
};

}