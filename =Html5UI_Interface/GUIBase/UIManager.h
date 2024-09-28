#pragma once

#include "UIDataType.h"

namespace GUIBase
{

class IPage;
class IUIEventFactory;
class CUIMouseEvent;
class CUIWheelEvent;
class CUIKeyEvent;

/*------------------------------
	UI를 총괄하는 Manager;
------------------------------*/
class CUIManager
{
public:
	static CUIManager& GetInstance();

public:
	/// Device 처리;
	bool InitDeviceObjects();
	bool InvalidateDeviceObjects();
	bool RestoreDeviceObjects();
	bool DeleteDeviceObjects();

	/// Loop;
	bool FrameMove( float timeDelta );
	bool Render( float timeDelta );

public:
	//! 현재의 UIPage;
	inline const IPage* GetCurPage() const { return m_pPage; }

	// 현재 들린 윈도우 관리;
	inline UIID GetHoldWindow() const { return m_holdUIID; }
	inline void RegisterHoldWindow( UIID uiID ) { m_holdUIID = uiID; }

	// 포커스 겟;
	inline UIID GetFocusWindow() const { return m_focusUIID; }
	inline UIID GetKeyFocusWindow() const { return m_keyFocusUIID; }
	inline void SetKeyFocusWindow( UIID uiID ) { m_keyFocusUIID = uiID; }

public:
	// 포커스 등록;
	void RegisterFocusWindow( UIID uiID );

	//! UI Stage 변경;
	void ChangeUIStage( IPage* pPage );

	//! Mouse, Keyboard, Wheel의 이벤트 전달;
	bool FireMouseEvent( CUIMouseEvent* mouseEvent );
	bool FireWheelEvent( CUIWheelEvent* wheelEvent );
	bool FireKeyEvent( CUIKeyEvent* keyEvent );

public:
	/// 기능;
	void UpdateWebPage();					// WebPage Update;

private:
	CUIManager();
	~CUIManager();

private:
	// 연결된 Page;
	IPage* m_pPage;

	// Hold Window ID
	UIID m_holdUIID;

	// Focus Window ID
	UIID m_focusUIID;

	// Key Focus Window ID
	UIID m_keyFocusUIID;
};

}