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
	// Callback Message�� ���� MsgProc;
	virtual bool MsgProc( const CALLBACK_MESSAGE msg ) = 0;

	virtual void Initialize() { }
	virtual void Destroy() { }
	virtual bool FrameMove( float timeDelta ) { return true; }
	virtual bool Render( float timeDelta );

public:
	/// �⺻������ �̷������ �ϴ� �κ�;
	void UIInitialize();
	void UIDestroy();
	bool UIFrameMove( float timeDelta );
	bool UIRender( float timeDelta );

public:
	/// Event ó��;
	virtual void OnSize( std::tr1::shared_ptr<CUIResizeEvent> resizeEvent );
	virtual void OnFocus( std::tr1::shared_ptr<CUIFocusEvent> focusEvent );
	virtual void OnShow( std::tr1::shared_ptr<CUIShowEvent> showEvent );
	virtual void OnHide( std::tr1::shared_ptr<CUIShowEvent> hideEvent );
	virtual bool OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent );
	virtual bool OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent );
	virtual bool OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent );
	virtual bool OnInvokeEvent( std::tr1::shared_ptr<CUIInvokeEvent> invokeEvent );

public:
	/// Virtual �Լ�;
	virtual void ShowComponent();
	virtual void HideComponent();

public:
	/// Event �߻� �Լ�;
	// EventManager���� �߻��� Event�� ���ͼ� Window��;
	// ���� ��� �߻� �Ѵ�;
	// Invoke, Mouse, Keyboard, Wheel ���� �̺�Ʈ ����;
	virtual bool FireMouseEvent( CUIMouseEvent* mouseEvent );
	virtual bool FireWheelEvent( CUIWheelEvent* wheelEvent );
	virtual bool FireKeyEvent( CUIKeyEvent* keyEvent );
	virtual bool FireInvokeEvent( Json::Value& root );

public:
	/// Member �Լ�;
	// WebPage Update
	void UpdateWebPage();

	// Event ���;
	void FireUIEvent( CUIEvent* uiEvent );

	// Event ó��;
	void EventProc();

	// Array Invoke;
	// ��ϵ� Invoke���� �ϳ��� ���ļ� ����; 
	void FireArrayInvokeEvent();

	// Callback Message ó��;
	void UIPageCallback( const std::string& message );

	// �ڽ� ������ �߰�;
	bool AddChildWindowID( UIID uiID );

	// �ڽ� ������ ��ȯ;
	UIID GetChildWindowID();

private:
	/// Private �Լ�;
	// Update Image Data
	bool LoadImgUpdateData();

public:
	/// Inline �Լ�;
	// �����츦 Block;
	inline void BlockWindow() { m_bBlock = true; }
	inline void UnblockWindow() { m_bBlock = false; }
	inline bool GetBlockState() { return m_bBlock; }

	inline const UIPOINT<int>& GetMouseGap() const { return m_mouseGap; }

protected:
	// �ڽ� ������ ID ����;
	// ������ ��ü�� WindowManager���� �����Ѵ�;
	WINDOW_LIST m_windowLists;

	//! URL
	std::wstring m_url;

	//! WebPage���� ������ Message Queue;
	std::queue< CALLBACK_MESSAGE > m_msgQueue;

	//! WebPage�� ������ Event Queue;
	std::queue< std::tr1::shared_ptr< CUIEvent > > m_pEventQueue;

	//! Array Invoke Data;
	std::vector< std::tr1::shared_ptr< CUIInvokeEvent > > m_invokeEvents;

	//! WebKit�� �����Ǵ� View;
	std::tr1::shared_ptr< IWebView > m_webView;

	//! Image Data
	UI_IMAGE_DATA m_uiImgData;

	//! Block
	bool m_bBlock;

	//! Drag
	UIPOINT<int> m_mouseGap;
};

}