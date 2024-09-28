#pragma once

#include "UIDataType.h"
#include <string>

namespace GUIBase
{

class IWindow;
class CUIMouseEvent;
class CUIWheelEvent;
class CUIKeyEvent;
class IUIEventFactory;

/*------------------------------
	Page Interface
------------------------------*/
class IPage
{
public:
	IPage();
	virtual ~IPage() {}

public:
	/// 사용자가 직접 구현하게 하기 위해 추상화시킨 함수;
	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
	virtual bool FrameMove( float timeDelta ) = 0;
	virtual bool Render( float timeDelta ) = 0;

public:
	/// 기본;
	void UIInitialize();
	void UIDestroy();
	bool UIFrameMove( float timeDelta );
	bool UIRender( float timeDelta );

public:
	/// Mouse, Keyboard, Wheel 등의 이벤트 전달;
	bool FireMouseEvent( CUIMouseEvent* mouseEvent );
	bool FireWheelEvent( CUIWheelEvent* wheelEvent );
	bool FireKeyEvent( CUIKeyEvent* keyEvent );

public:
	// Update WebPage
	void UpdateWebPage();

protected:
	// Window
	//IWindow* m_pBackWindow;
	IWindow* m_pBasicWindow;
	//IWindow* m_pFrontWindow;
};

}