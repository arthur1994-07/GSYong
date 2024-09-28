#include "StdAfx.h"
#include "UIEventFactory.h"

#include <GUIBase/UIDataType.h>
#include <GUIBase/UIManager.h>
#include "../EngineLib/DeviceMan.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/Common/DXInputString.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

namespace HTML5UI
{

//! ���콺 �̺�Ʈ;
void CUIEventFactory::FireMouseEvent()
{
	// DInput�� �̿��Ͽ� Input ������ ȹ��;
	DxInputDevice& input = DxInputDevice::GetInstance();

	/// Mouse
	GUIBase::CUIEvent::UIEventType eventType = GUIBase::CUIEvent::UI_NoType;
	GUIBase::CUIMouseEvent::Button mouseButton = GUIBase::CUIMouseEvent::NoButton;
	GUIBase::UIPOINT<int> mousePoint( 0, 0, 0 );
	input.GetMouseLocate( mousePoint.x, mousePoint.y, mousePoint.z );
	int deltaX = 0; int deltaY = 0; int deltaZ = 0;
	input.GetMouseMove( deltaX, deltaY, deltaZ );

	// Mouse Move ����;
	if( input.IsMouseMove() )
	{
		eventType = GUIBase::CUIEvent::UI_MouseMove;
		if( input.GetMouseState( DXMOUSE_LEFT ) == DXKEY_DOWNED)
			mouseButton = GUIBase::CUIMouseEvent::LeftButton;
		else if( input.GetMouseState( DXMOUSE_MIDDLE ) == DXKEY_DOWNED )
			mouseButton = GUIBase::CUIMouseEvent::MiddleButton;
		else if( input.GetMouseState( DXMOUSE_RIGHT ) == DXKEY_DOWNED )
			mouseButton = GUIBase::CUIMouseEvent::RightButton;
	}

	// Mouse Down ����;
	if( input.GetMouseState( DXMOUSE_LEFT ) == DXKEY_DOWN ||
		input.GetMouseState( DXMOUSE_MIDDLE ) == DXKEY_DOWN ||
		input.GetMouseState( DXMOUSE_RIGHT ) == DXKEY_DOWN )
	{
		eventType = GUIBase::CUIEvent::UI_MouseDown;
		if( input.GetMouseState( DXMOUSE_LEFT ) == DXKEY_DOWN )
			mouseButton = GUIBase::CUIMouseEvent::LeftButton;
		else if( input.GetMouseState( DXMOUSE_MIDDLE ) == DXKEY_DOWN )
			mouseButton = GUIBase::CUIMouseEvent::MiddleButton;
		else if( input.GetMouseState( DXMOUSE_RIGHT ) == DXKEY_DOWN )
			mouseButton = GUIBase::CUIMouseEvent::RightButton;
	}

	// Mouse Up ����;
	if( input.GetMouseState( DXMOUSE_LEFT ) == DXKEY_UP ||
		input.GetMouseState( DXMOUSE_MIDDLE ) == DXKEY_UP ||
		input.GetMouseState( DXMOUSE_RIGHT ) == DXKEY_UP )
	{
		eventType = GUIBase::CUIEvent::UI_MouseUp;
		if( input.GetMouseState( DXMOUSE_LEFT ) == DXKEY_UP )
			mouseButton = GUIBase::CUIMouseEvent::LeftButton;
		else if( input.GetMouseState( DXMOUSE_MIDDLE ) == DXKEY_UP )
			mouseButton = GUIBase::CUIMouseEvent::MiddleButton;
		else if( input.GetMouseState( DXMOUSE_RIGHT ) == DXKEY_UP )
			mouseButton = GUIBase::CUIMouseEvent::RightButton;
	}

	if( eventType == GUIBase::CUIEvent::UI_NoType )
		return;

	// Key Modifiers
	unsigned modifiers = 0;
	if( input.IsCtrlDown() )
		modifiers |= GUIBase::CUIInputEvent::ControlKey;
	if( input.IsShiftDown() )
		modifiers |= GUIBase::CUIInputEvent::ShiftKey;
	if( input.IsAltDown() )
		modifiers |= GUIBase::CUIInputEvent::AltKey;
	if( input.IsCtrlDown() && input.IsAltDown() )
		modifiers |= GUIBase::CUIInputEvent::MetaKey;
	GUIBase::CUIInputEvent::KeyModifiers keyModifiers
		= static_cast<GUIBase::CUIInputEvent::KeyModifiers>( modifiers );

	// Create Mouse Event
	GUIBase::CUIMouseEvent* mouseEvent =
		new GUIBase::CUIMouseEvent( GUIBase::NO_ID
		, eventType
		, keyModifiers
		, mousePoint
		, mousePoint
		, mouseButton
		, static_cast<float>( deltaX )
		, static_cast<float>( deltaY )
		, static_cast<float>( deltaZ ) );

	// Event ����;
	GUIBase::CUIManager::GetInstance().FireMouseEvent( mouseEvent );
}

//! �� �̺�Ʈ;
void CUIEventFactory::FireWheelEvent()
{
	// DInput�� �̿��Ͽ� Input ������ ȹ��;
	DxInputDevice& input = DxInputDevice::GetInstance();

	// Delta�� ȹ��, Delta���� ������ return;
	float delta = -input.GetMouseMoveZ() / static_cast<float>(WHEEL_DELTA);
	if( !delta )
		return;

	// ���콺�� ���� ��ġ ����;
	GUIBase::CUIEvent::UIEventType eventType = GUIBase::CUIEvent::UI_Wheel;
	GUIBase::UIPOINT<int> mousePoint( 0, 0, 0 );
	input.GetMouseLocate( mousePoint.x, mousePoint.y, mousePoint.z );

	// Key Modifiers
	unsigned modifiers = 0;
	if( input.IsCtrlDown() )
		modifiers |= GUIBase::CUIInputEvent::ControlKey;
	if( input.IsShiftDown() )
		modifiers |= GUIBase::CUIInputEvent::ShiftKey;
	if( input.IsAltDown() )
		modifiers |= GUIBase::CUIInputEvent::AltKey;
	if( input.IsCtrlDown() && input.IsAltDown() )
		modifiers |= GUIBase::CUIInputEvent::MetaKey;
	GUIBase::CUIInputEvent::KeyModifiers keyModifiers
		= static_cast<GUIBase::CUIInputEvent::KeyModifiers>( modifiers );

	// Create Mouse Event
	GUIBase::CUIWheelEvent* wheelEvent =
		new GUIBase::CUIWheelEvent( GUIBase::NO_ID
		, eventType
		, keyModifiers
		, mousePoint
		, mousePoint
		, delta );

	// Event ����;
	GUIBase::CUIManager::GetInstance().FireWheelEvent( wheelEvent );
}

// DX����� KetCode�� ������ ������� ��ȯ;
// ( WebKit������ KeyCode�� ������ ������� �ؼ��� );
static int windowsKeyCodeForKeyEvent( int keyCode )
{
	return MapVirtualKey( keyCode, 1 );
}

// Ű������ �̺�Ʈ Ÿ�� ȹ��;
static inline GUIBase::CUIEvent::UIEventType keyboardEventTypeForEvent( int& keyCode, std::wstring& text )
{
	// DInput�� �̿��Ͽ� Input ������ ȹ��;
	DxInputDevice& input = DxInputDevice::GetInstance();

	for( int i=0; i<256; ++i )
	{
		DWORD keyState = input.GetKeyState( i );
		if( keyState & DXKEY_DOWN )
		{
			keyCode = windowsKeyCodeForKeyEvent( i );
			switch( i )
			{
			case DIK_1:
				text = L"1";
				break;
			case DIK_2:
				text = L"2";
				break;
			case DIK_3:
				text = L"3";
				break;
			case DIK_4:
				text = L"4";
				break;
			case DIK_5:
				text = L"5";
				break;
			case DIK_6:
				text = L"6";
				break;
			case DIK_7:
				text = L"7";
				break;
			case DIK_8:
				text = L"8";
				break;
			case DIK_9:
				text = L"9";
				break;
			case DIK_0:
				text = L"0";
				break;

			case DIK_Q:
				text = L"Q";
				break;
			case DIK_W:
				text = L"W";
				break;
			case DIK_E:
				text = L"E";
				break;
			case DIK_R:
				text = L"R";
				break;
			case DIK_T:
				text = L"T";
				break;
			case DIK_Y:
				text = L"Y";
				break;
			}
			return GUIBase::CUIEvent::UI_KeyDown;
		}

		if( keyState & DXKEY_UP )
			return GUIBase::CUIEvent::UI_KeyUp;
	}

	return GUIBase::CUIEvent::UI_NoType;
}

// Key Input ����;
static void BeginKeyInput( DXInputString* pInputString )
{
	if( pInputString->IsOn() )
		return;

	pInputString->ConverseEngToNative();

	pInputString->OnInput();
	pInputString->SetString();
}

// Key Input ����;
static void EndKeyInput( DXInputString* pInputString )
{
	if( !pInputString->IsOn() )
		return;

	pInputString->ConverseNativeToEng();

	pInputString->OffInput();
	pInputString->SetString();
}

//! Ű���� �̺�Ʈ;
void CUIEventFactory::FireKeyEvent()
{
	// DInput�� �̿��Ͽ� Input ������ ȹ��;
	DxInputDevice& input = DxInputDevice::GetInstance();
	DXInputString* pInputString = m_pEngineDevice->GetInputString();
	CString strText;

	// Input ����;
	BeginKeyInput( pInputString );

	// On �������� üũ;
	if( !pInputString->IsOn() )
	{
		pInputString->OffInput();
		pInputString->SetString();

		return;
	}
	else
	{
		switch( RENDERPARAM::emSERVICE_TYPE )
		{
		case RENDERPARAM::EMSERVICE_INDONESIA:
		case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
		case RENDERPARAM::EMSERVICE_PHILIPPINES:
			if( pInputString->IsNativeMode() )
				pInputString->ConverseNativeToEng();
			break;
		};

		pInputString->ConverseFullToHalf();

		CString strInput = pInputString->GetString();

		// �����Է� �̿ܿ� ������� Ű�� ������ ���, ��� ����;
		bool bVALID_INPUT = true;
		if( strInput[0] == '-' ) bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_DOWNED ) bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU ) & DXKEY_DOWNED ) bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RCONTROL ) & DXKEY_DOWNED ) bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RMENU ) & DXKEY_DOWNED ) bVALID_INPUT = false;

		if( !bVALID_INPUT )
		{
			pInputString->SetString();
			pInputString->SetString( strText );
			return;
		}

		strText = strInput;
	}

	int keyCode = 0;
	std::wstring text						= L"";//GUIBase::ToUnicode( strText.GetString() );
	GUIBase::CUIEvent::UIEventType type		= keyboardEventTypeForEvent( keyCode, text );
	text									= GUIBase::ToUnicode( strText.GetString() );
	if( type == GUIBase::CUIEvent::UI_NoType )
		return;
	
	int nativeVirtualKeyCode				= keyCode;

	if( !keyCode )
		return;

	if( pInputString->CheckComposition() )
		return;
	else
		pInputString->SetString();

	// Key Modifiers
	unsigned modifiers = 0;
	if( input.IsCtrlDown() )
		modifiers |= GUIBase::CUIInputEvent::ControlKey;
	if( input.IsShiftDown() )
		modifiers |= GUIBase::CUIInputEvent::ShiftKey;
	if( input.IsAltDown() )
		modifiers |= GUIBase::CUIInputEvent::AltKey;
	if( input.IsCtrlDown() && input.IsAltDown() )
		modifiers |= GUIBase::CUIInputEvent::MetaKey;
	GUIBase::CUIInputEvent::KeyModifiers keyModifiers
		= static_cast<GUIBase::CUIInputEvent::KeyModifiers>( modifiers );

	// Event ����;
	GUIBase::CUIKeyEvent* keyEvent
		= new GUIBase::CUIKeyEvent(
		GUIBase::NO_ID,
		type,
		modifiers,
		text,
		keyCode,
		nativeVirtualKeyCode );

	// Char �̺�Ʈ ����;
	if( text != L"" )// && pInputString->IsOnChar() )
	{
		type = GUIBase::CUIEvent::UI_Char;
		keyEvent->SetEventType( type );
	}
	GUIBase::CUIManager::GetInstance().FireKeyEvent( keyEvent );
}

CUIEventFactory::CUIEventFactory( EngineDeviceMan* pEngineDevice )
: m_pEngineDevice( pEngineDevice )
{
	// DInput�� �̿��Ͽ� Input ������ ȹ��;
	DxInputDevice& input = DxInputDevice::GetInstance();
	DXInputString* pInputString = m_pEngineDevice->GetInputString();

	// Input ����;
	//BeginKeyInput( pInputString );
}

CUIEventFactory::~CUIEventFactory()
{
	EndKeyInput( m_pEngineDevice->GetInputString() );
	//EndKeyInput( m_pEngineDevice->GetInputString() );
}

}