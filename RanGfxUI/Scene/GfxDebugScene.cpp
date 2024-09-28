#include "StdAfx.h"
#include "GfxDebugScene.h"

#include "../Core/GfxKeyCode.h"

GfxDebugScene::GfxDebugScene()
{
}

GfxDebugScene::~GfxDebugScene()
{
}

HRESULT GfxDebugScene::OnInit( LPDIRECT3DDEVICE9 pDevice )
{
	m_strFilePath = NSGFXFILE::g_strGfxPath;
	m_strFilePath.append( NSGFXFILE::g_strDebugSceneName.c_str() );
	m_strFilePath = sc::string::ansi_to_utf8( m_strFilePath );

	return LoadMovie( pDevice );
}

bool GfxDebugScene::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging )
{
	if( NULL == m_pMovie )
		return false; 

	switch( uMsg )
	{
	case WM_SETFOCUS :
		{
			m_pMovie->HandleEvent( GFx::SetFocusEvent( UpdateModifiers() ) );
		}
		return false;

	case WM_KILLFOCUS :
		{
			// 웹 뷰에 포커스가 이동하면 킬 포커스 메시지가 날라온다.
			// 윈도우 포커스 잃을 때만 킬 포커스 처리 하도록 수정.
			if( 0 == wParam )
				m_pMovie->HandleEvent( GFx::Event( GFx::Event::KillFocus ) );
		}
		return false;
	}

	if( true == ProcessMouseEvent( hWnd, uMsg, wParam, lParam, bHit, bIsDragging ) )
		return true;
	if( true == ProcessKeyEvent( hWnd, uMsg, wParam, lParam ) )
		return true;

	return false;
}

bool GfxDebugScene::ProcessMouseEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging )
{
	float fMouseX = (float) LOWORD( lParam );
	float fMouseY = (float) HIWORD( lParam );
	float fScrollVal = 0.0f;
	int nMouseButtonIndex = 0;	// Left - 0, Right - 1, Middle = 2

	if( WM_MOUSEWHEEL != uMsg )
	{
		const D3DPRESENT_PARAMETERS& pp = GetCore()->GetPresentParams();
		if( fMouseX > pp.BackBufferWidth ||
			fMouseY > pp.BackBufferHeight )
			return false;
	}

	bool bMouseHitTest = false;
	bool bMouseEvent = false;
	GFx::Event::EventType emEvent = GFx::Event::Unknown;

	switch( uMsg )
	{
	case WM_MOUSEMOVE :
		{
			bMouseEvent = true;
			emEvent = GFx::Event::MouseMove;
		}
		break;

	case WM_LBUTTONDOWN :
	case WM_RBUTTONDOWN :
	case WM_MBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
	case WM_RBUTTONDBLCLK :
	case WM_MBUTTONDBLCLK :
		{
			SetCapture( hWnd );

			bMouseEvent = true;
			bMouseHitTest = true;
			emEvent = GFx::Event::MouseDown;

			if( WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg )
				nMouseButtonIndex = 0;
			else if( WM_RBUTTONDOWN == uMsg || WM_RBUTTONDBLCLK == uMsg )
				nMouseButtonIndex = 1;
			else if( WM_MBUTTONDOWN == uMsg || WM_MBUTTONDBLCLK == uMsg )
				nMouseButtonIndex = 2;
		}
		break;

	case WM_LBUTTONUP :
	case WM_RBUTTONUP :
	case WM_MBUTTONUP :
		{
			ReleaseCapture();

			bMouseEvent = true;
			bMouseHitTest = true;
			emEvent = GFx::Event::MouseUp;

			if( WM_LBUTTONUP == uMsg )
				nMouseButtonIndex = 0;
			else if( WM_RBUTTONUP == uMsg )
				nMouseButtonIndex = 1;
			else if( WM_MBUTTONUP == uMsg )
				nMouseButtonIndex = 2;
		}
		break;

	case WM_MOUSEWHEEL :
		{
			bMouseEvent = true;
			bMouseHitTest = true;
			emEvent = GFx::Event::MouseWheel;

			POINT wcl = { 0, 0 };
			ClientToScreen( hWnd, &wcl );
			fMouseX = static_cast<float>(int( SInt16( LOWORD( lParam ) ) ) - wcl.x);
			fMouseY = static_cast<float>(int( SInt16( HIWORD( lParam ) ) ) - wcl.y);

			fScrollVal = (float) SInt16( HIWORD( wParam ) ) / (float) WHEEL_DELTA * 3;
		}
		break;
	}

	if( true == bMouseEvent )
	{
		MouseEvent mevent( emEvent, nMouseButtonIndex, fMouseX, fMouseY, fScrollVal, 0 );
		m_pMovie->HandleEvent( mevent );

		if( true == bMouseHitTest )
		{
			if( true == m_pMovie->HitTest( fMouseX, fMouseY, Movie::HitTest_ShapesNoInvisible ) )
			{
				bHit = true;

				if( GFx::Event::MouseDown == emEvent )
					bIsDragging = true;
			}
			else
			{
				bHit = false;
			}
		}

		if( GFx::Event::MouseUp == emEvent )
			bIsDragging = false;

		if( GFx::Event::MouseWheel == emEvent || GFx::Event::MouseMove == emEvent )
			return true;
	}

	return false;
}

bool GfxDebugScene::ProcessKeyEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_SYSKEYDOWN :
	case WM_SYSKEYUP :
	case WM_KEYDOWN :
	case WM_KEYUP :
		{
			bool downFlag = ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN ) ? true : false;
			UInt8 scanCode = (UInt8) ( ( lParam >> 16 ) & 0xff );
			bool  extendedKeyFlag = ( lParam & ( 1 << 24 ) ) != 0;
			if( wParam == VK_SHIFT && scanCode == 54 )
				extendedKeyFlag = true;

			ProcessKey( (unsigned) wParam, downFlag, extendedKeyFlag );

			if( uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP )
				break;

			return true;
		}
		break;

	case WM_CHAR :
		{
			OnKey( 0, Key::None, (unsigned) wParam, true, UpdateModifiers() );
		}
		break;
	}

	return false;
}

KeyModifiers GfxDebugScene::UpdateModifiers( bool extendedKeyFlag )
{
	KeyModifiers mods;
	if( GetKeyState( VK_SHIFT ) & 0x8000 )
		mods.SetShiftPressed( true );
	if( GetKeyState( VK_CONTROL) & 0x8000 )
		mods.SetCtrlPressed( true );
	if( GetKeyState( VK_MENU ) & 0x8000 )
		mods.SetAltPressed( true );
	if( GetKeyState( VK_NUMLOCK ) & 0x1 )
		mods.SetNumToggled( true );
	if( GetKeyState( VK_CAPITAL ) & 0x1 )
		mods.SetCapsToggled( true );
	if( GetKeyState( VK_SCROLL ) & 0x1 )
		mods.SetScrollToggled( true );
	if( extendedKeyFlag )
		mods.SetExtendedKey( true );

	m_KeyMod = mods;

	return mods;
}

void GfxDebugScene::ProcessKey( unsigned vkCode, bool downFlag, bool extendedKeyFlag )
{
	Key::Code emKeyCode = Key::None;
	if( vkCode >= 'A' && vkCode <= 'Z' )
	{
		emKeyCode = (Key::Code) ( (vkCode - 'A') + Key::A );
	}
	else if( vkCode >= VK_F1 && vkCode <= VK_F15 )
	{
		emKeyCode = (Key::Code) ( (vkCode - VK_F1) + Key::F1 );
	}
	else if( vkCode >= '0' && vkCode <= '9' )
	{
		emKeyCode = (Key::Code) ( (vkCode - '0') + Key::Num0 );
	}
	else if( vkCode >= VK_NUMPAD0 && vkCode <= VK_DIVIDE )
	{
		emKeyCode = (Key::Code) ( (vkCode - VK_NUMPAD0) + Key::KP_0 );
	}
	else
	{
		for( int i = 0; NSGFXKEYCODE::g_sKeyCodeMap[ i ].nWinKeyCode != 0; ++i )
		{
			if( vkCode == (unsigned) NSGFXKEYCODE::g_sKeyCodeMap[ i ].nWinKeyCode )
			{
				emKeyCode = NSGFXKEYCODE::g_sKeyCodeMap[ i ].emSFKeyCode;
				break;
			}
		}
	}

	OnKey( 0, emKeyCode, 0, downFlag, UpdateModifiers( extendedKeyFlag ) );
}

void GfxDebugScene::OnKey( unsigned controllerIndex, KeyCode keyCode, unsigned wcharCode, bool downFlag, KeyModifiers mods )
{
	if( NULL == m_pMovie )
		return;

	if( true == mods.IsCtrlPressed() )
	{
		GFx::Clipboard* pClipboard = m_pMovie->GetClipboard();
		if( NULL != pClipboard )
		{
			switch( keyCode )
			{
			case Key::V :
				{
					std::wstring strClipBoardText;
					sc::string::clipboardToString( NULL, strClipBoardText );

					pClipboard->SetText( strClipBoardText.c_str(), strClipBoardText.length() );
				}
				break;
			}
		}
	}

	if( true == downFlag && Key::None == keyCode && 0 != wcharCode )
	{
		CharEvent gfxEvent( wcharCode );
		m_pMovie->HandleEvent( gfxEvent );
	}

	if( Key::None != keyCode )
	{
		KeyEvent gfxEvent( downFlag ? KeyEvent::KeyDown : KeyEvent::KeyUp,
			keyCode, 0, wcharCode, mods, (UInt8) controllerIndex );

		m_pMovie->HandleEvent( gfxEvent );
	}
}