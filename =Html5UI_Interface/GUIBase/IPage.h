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
	/// ����ڰ� ���� �����ϰ� �ϱ� ���� �߻�ȭ��Ų �Լ�;
	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
	virtual bool FrameMove( float timeDelta ) = 0;
	virtual bool Render( float timeDelta ) = 0;

public:
	/// �⺻;
	void UIInitialize();
	void UIDestroy();
	bool UIFrameMove( float timeDelta );
	bool UIRender( float timeDelta );

public:
	/// Mouse, Keyboard, Wheel ���� �̺�Ʈ ����;
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