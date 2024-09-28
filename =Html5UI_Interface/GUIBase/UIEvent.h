#pragma once

#include "UIDataType.h"
#include <vector>

namespace GUIBase
{

// Event 관련 유틸리티 클래스;
class CEventUtil
{
public:
	static bool MouseIn( UIPOINT<int> point, UIRECT rcGlobalPos );
};

class CUIEvent
{
public:
	enum UIEventType
	{
		UI_NoType = -1,

		UI_Create,
		UI_Destroy,
		UI_Close,

		// Size Event
		UI_Size,

		// Mouse Event
		UI_MouseDown,
		UI_MouseUp,
		UI_MouseMove,
		UI_MouseLeave,

		// Mouse Wheel Event
		UI_Wheel,

		// Keyboard Event
		UI_KeyDown,
		UI_KeyUp,
		UI_Char,

		// Focus Event
		UI_SetFocus,
		UI_KillFocus,

		// Show Event
		UI_ShowWindow,
		UI_HideWindow,

		// Invoke Event
		UI_Invoke
	};

public:
	CUIEvent( UIID uiID, UIEventType type, double timestamp = 0.0 );
	virtual ~CUIEvent();

public:
	inline UIID GetUIID() const { return m_uiID; }
	inline void SetUIID( UIID uiID ) { m_uiID = uiID; }
	inline UIEventType GetEventType() const { return m_type; }
	inline void SetEventType( UIEventType type ) { m_type = type; }
	inline double GetTimeStamp() { return m_timestamp; }

private:
	UIID m_uiID;			// 전달될 UI ID ( 즉, Component에 Event가 전달될 때에만 쓰인다 );
	UIEventType m_type;		// Event Type
	double m_timestamp;		// Timestamp
};

//! Resize Event
class CUIResizeEvent : public CUIEvent
{
public:
	CUIResizeEvent( UIID uiID
		, UIEventType type
		, const UISIZE<float>& size
		, const UISIZE<float>& oldSize
		, double timestamp = 0.0 );
	virtual ~CUIResizeEvent();

	inline const UISIZE<float>& GetSize() const { return m_size; }
	inline const UISIZE<float>& GetOldSize() const { return m_oldSize; }

private:
	UISIZE<float> m_size;
	UISIZE<float> m_oldSize;
};

//! Input Event
class CUIInputEvent : public CUIEvent
{
public:
	enum KeyModifiers {
		ShiftKey    = 1 << 0,
		ControlKey  = 1 << 1,
		AltKey      = 1 << 2,
		MetaKey     = 1 << 3,
		CapsLockKey = 1 << 4
	};

public:
	CUIInputEvent( UIID uiID
		, UIEventType type
		, unsigned keyModifiers
		, double timestamp = 0.0 );
	virtual ~CUIInputEvent();

public:
	bool shiftKey() const { return m_keyModifiers & ShiftKey ? true : false; }
	bool controlKey() const { return m_keyModifiers & ControlKey ? true : false; }
	bool altKey() const { return m_keyModifiers & AltKey ? true : false; }
	bool metaKey() const { return m_keyModifiers & MetaKey ? true : false; }
	bool capsLockKey() const { return m_keyModifiers & CapsLockKey ? true : false; }

	KeyModifiers modifiers() const { return static_cast<KeyModifiers>( m_keyModifiers ); }

protected:
	unsigned m_keyModifiers;
};

//! Mouse Event
class CUIMouseEvent : public CUIInputEvent
{
public:
	enum Button {
		NoButton = -1,
		LeftButton,
		MiddleButton,
		RightButton
	};

public:
	CUIMouseEvent( UIID uiID
		, UIEventType type
		, unsigned keyModifiers
		, const UIPOINT<int>& position
		, const UIPOINT<int>& globalPosition
		, Button& button
		, float deltaX
		, float deltaY
		, float deltaZ
		, double timestamp = 0.0 );
	virtual ~CUIMouseEvent();

	inline void SetPosition( const UIPOINT<int>& position ) { m_position = position; }
	inline const UIPOINT<int>& GetPosition() const { return m_position; }
	inline const UIPOINT<int>& GetGlobalPosition() const { return m_globalPosition; }
	inline Button GetButton() { return m_button; }
	inline void SetButton( Button& button ) { m_button = button; }
	inline float GetDeltaX() const { return m_deltaX; }
	inline float GetDeltaY() const { return m_deltaY; }
	inline float GetDeltaZ() const { return m_deltaZ; }

private:
	UIPOINT<int> m_position;
	UIPOINT<int> m_globalPosition;
	Button m_button;
	float m_deltaX;
	float m_deltaY;
	float m_deltaZ;
};

//! Wheel Event
class CUIWheelEvent : public CUIInputEvent
{
public:
	CUIWheelEvent( UIID uiID
		, UIEventType type
		, unsigned keyModifiers
		, const UIPOINT<int>& position
		, const UIPOINT<int>& globalPosition
		, float delta
		, double timestamp = 0.0 );
	virtual ~CUIWheelEvent();

	inline void SetPosition( const UIPOINT<int>& position ) { m_position = position; }
	inline const UIPOINT<int> GetPosition() const { return m_position; }
	inline const UIPOINT<int> GetGlobalPosition() const { return m_globalPosition; }
	inline const float GetDelta() const { return m_delta; }

private:
	UIPOINT<int> m_position;
	UIPOINT<int> m_globalPosition;
	float m_delta;
};

//! Keyboard Event
class CUIKeyEvent : public CUIInputEvent
{
public:
	CUIKeyEvent( UIID uiID
		, UIEventType type
		, unsigned keyModifiers
		, const std::wstring& text
		, int key
		, int nativeVirtualKeyCode
		, double timestamp = 0.0 );
	virtual ~CUIKeyEvent();

	inline const std::wstring& text() const { return m_text; }
	inline int key() const { return m_key; }
	inline int nativeVirtualKeyCode() const { return m_nativeVirtualKeyCode; }

private:
	std::wstring m_text;
	int m_key;
	int m_nativeVirtualKeyCode;
};

//! Focus Event
class CUIFocusEvent : public CUIEvent
{
public:
	CUIFocusEvent( UIID uiID
		, UIEventType type
		, bool bFocusable
		, double timestamp = 0.0 );
	virtual ~CUIFocusEvent();

	inline bool isFocusable() const { return m_bFocusable; }

private:
	bool m_bFocusable;
};

//! Show Event
class CUIShowEvent : public CUIEvent
{
public:
	CUIShowEvent( UIID uiID
		, UIEventType type
		, bool bVisible
		, double timestamp = 0.0 );
	virtual ~CUIShowEvent();

	inline bool isVisible() const { return m_bVisible; }

private:
	bool m_bVisible;
};

//! Invoke Event
class CUIInvokeEvent : public CUIEvent
{
public:
	CUIInvokeEvent( UIID uiID
		, UIEventType type
		, const std::wstring& message
		, double timestamp = 0.0 );
	virtual ~CUIInvokeEvent();

	inline const std::wstring& GetInvokeMessage() const { return m_message; }

private:
	std::wstring m_message;
};

//! Invoke Event
class CUIArrayInvokeEvent : public CUIEvent
{
public:
	CUIArrayInvokeEvent( UIID uiID
		, UIEventType type
		, unsigned int length
		, double timestamp = 0.0 );
	virtual ~CUIArrayInvokeEvent();

	inline unsigned int GetLength() const { return m_length; }

	inline bool CUIArrayInvokeEvent::GetInvokeMessage(
		std::wstring& message, unsigned int index )
	{
		if( index >= 0 && index < m_length )
		{
			message = m_messages[ index ];
			return true;
		}
		return false;
	}

	void AddInvokeMessage( const std::wstring& message, unsigned int index );

private:
	unsigned int m_length;
	std::wstring* m_messages;
};

}