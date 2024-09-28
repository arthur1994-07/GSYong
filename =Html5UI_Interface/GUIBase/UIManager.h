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
	UI�� �Ѱ��ϴ� Manager;
------------------------------*/
class CUIManager
{
public:
	static CUIManager& GetInstance();

public:
	/// Device ó��;
	bool InitDeviceObjects();
	bool InvalidateDeviceObjects();
	bool RestoreDeviceObjects();
	bool DeleteDeviceObjects();

	/// Loop;
	bool FrameMove( float timeDelta );
	bool Render( float timeDelta );

public:
	//! ������ UIPage;
	inline const IPage* GetCurPage() const { return m_pPage; }

	// ���� �鸰 ������ ����;
	inline UIID GetHoldWindow() const { return m_holdUIID; }
	inline void RegisterHoldWindow( UIID uiID ) { m_holdUIID = uiID; }

	// ��Ŀ�� ��;
	inline UIID GetFocusWindow() const { return m_focusUIID; }
	inline UIID GetKeyFocusWindow() const { return m_keyFocusUIID; }
	inline void SetKeyFocusWindow( UIID uiID ) { m_keyFocusUIID = uiID; }

public:
	// ��Ŀ�� ���;
	void RegisterFocusWindow( UIID uiID );

	//! UI Stage ����;
	void ChangeUIStage( IPage* pPage );

	//! Mouse, Keyboard, Wheel�� �̺�Ʈ ����;
	bool FireMouseEvent( CUIMouseEvent* mouseEvent );
	bool FireWheelEvent( CUIWheelEvent* wheelEvent );
	bool FireKeyEvent( CUIKeyEvent* keyEvent );

public:
	/// ���;
	void UpdateWebPage();					// WebPage Update;

private:
	CUIManager();
	~CUIManager();

private:
	// ����� Page;
	IPage* m_pPage;

	// Hold Window ID
	UIID m_holdUIID;

	// Focus Window ID
	UIID m_focusUIID;

	// Key Focus Window ID
	UIID m_keyFocusUIID;
};

}