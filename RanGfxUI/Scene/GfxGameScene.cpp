#include "StdAfx.h"
#include "GfxGameScene.h"

#include <Imm.h>

#include "../../SigmaCore/String/StringUtil.h"
#include "../../enginelib/GUInterface/Cursor.h"
#include "../Core/GfxKeyCode.h"
#include "../Display/GfxDisplay.h"

GfxGameScene::GfxGameScene(void)
: m_fMouseX( 0.0f )
, m_fMouseY( 0.0f )
, m_bCompostioning( false )
, m_bCopostionCheck( true )
{
}

GfxGameScene::~GfxGameScene(void)
{
	m_pDataBinder.Clear();
}

HRESULT GfxGameScene::OnInit( LPDIRECT3DDEVICE9 pDevice )
{
	m_strFilePath = NSGFXFILE::g_strGfxPath;
	m_strFilePath.append( NSGFXFILE::g_strMainFileName.c_str() );
	m_strFilePath = sc::string::ansi_to_utf8( m_strFilePath );

	return LoadMovie( pDevice );
}

HRESULT GfxGameScene::OnDestroyDevice()
{
	if( NULL == m_pMovie )
		return E_FAIL;

	GFx::Value arg, result;
	arg.SetBoolean( true );

	// swf widget �ε� ����
	m_pMovie->Invoke( "root.CF_UnloadWidgetAll", &result, &arg, 1 );

	return S_OK;
}

HRESULT GfxGameScene::OnFrameMove( float fElapsedTime )
{
	// �ν�Ʈ ����̽� ���¿��� advance ȣ���ϸ� â ������ �������� ��û ��������.
	// �ν�Ʈ ����̽� ���¿��� main.swf �ʱ�ȭ �������� ���꽺 ������� �Ѵ�.
	// ���� ������ ����� ���� ������ UI ��ġ�� �����ȴ�.
	if( true == GetCore()->GetExternalInterface()->IsCraeteCompleteGameMovie() &&
		true == GetCore()->IsLostDevice() )
		return E_FAIL;

	if( NULL == m_pMovie )
		return E_FAIL;

	m_pMovie->Advance( fElapsedTime, 0 );

#if defined( _RELEASED ) || defined( _DEBUG )
	AmpServer::GetInstance().AdvanceFrame();
#endif

	return S_OK;
}

HRESULT GfxGameScene::LoadMovie( LPDIRECT3DDEVICE9 pDevice )
{
	if( NULL != m_pMovie )
	{
		GFx::Value arg, result;
		arg.SetBoolean( true );

		m_pMovie->Invoke( "root.CF_UnloadWidgetAll", &result, &arg, 1 );
		m_pMovie.Clear();
	}

	__super::LoadMovie( pDevice );

	GfxExternalInterface* pInterface = GetCore()->GetExternalInterface();

	// ������ ���δ�
	m_pDataBinder = *new CLIKDataBindingAdapter( m_pMovie, pInterface );
	pInterface->SetGameDataBinder( m_pDataBinder );

	if( language::DEFAULT == RENDERPARAM::emLangSet ||
		language::KOREAN == RENDERPARAM::emLangSet )
	{
		GfxInterface::GetInstance()->GetCore()->GetIME()->SetActiveMovie( m_pMovie );
	}

	return S_OK;
}

void GfxGameScene::PreProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL == m_pMovie )
		return;

	bool imeUIMsg = ImmIsUIMessage( NULL, uMsg, wParam, lParam ) == 0 ? false : true;
	if( uMsg == WM_KEYDOWN		|| uMsg == WM_KEYUP		|| true == imeUIMsg ||
		uMsg == WM_LBUTTONDOWN	|| uMsg == WM_LBUTTONUP )
	{
		GFx::IMEWin32Event gfxEvent( GFx::IMEEvent::IME_PreProcessKeyboard, (UPInt) hWnd, uMsg, wParam, lParam, 0 );
		m_pMovie->HandleEvent( gfxEvent );
	}
}

bool GfxGameScene::ProcessEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging )
{
	if( NULL == m_pMovie )
		return false; 

	switch( uMsg )
	{
		// TODO : ������ IME�� �̿��ؼ� �ӽ÷� �����. ���� ���Ŵ��
	case WM_IME_STARTCOMPOSITION:
		m_bCompostioning = true;
		m_bCopostionCheck = false;
		return false;

	case WM_IME_ENDCOMPOSITION:
		m_bCompostioning = false;
		m_bCopostionCheck = false;
		return false ;

	case WM_SETFOCUS :
		{
			m_pMovie->HandleEvent( GFx::SetFocusEvent( UpdateModifiers() ) );
		}
		return false;

	case WM_KILLFOCUS :
		{
			// �� �信 ��Ŀ���� �̵��ϸ� ų ��Ŀ�� �޽����� ����´�.
			// ������ ��Ŀ�� ���� ���� ų ��Ŀ�� ó�� �ϵ��� ����.
			if( 0 == wParam )
				m_pMovie->HandleEvent( GFx::Event( GFx::Event::KillFocus ) );

			// ��Ʈ ������ ������ ��Ŀ�� ������ ������ �̸� Ŭ�� Ȱ��ȭ �� ���¶� �ٸ� ���÷��̰� �Ⱥ��̴� ������.
			GfxDisplay::GetInstance().SetEnableItemClick( false );
		}
		return false;
	}

	if( true == ProcessMouseEvent( hWnd, uMsg, wParam, lParam, bHit, bIsDragging ) )
		return true;

	if( true == ProcessKeyEvent( hWnd, uMsg, wParam, lParam ) )
		return true;

	if( true == ProcessIMEEvent( hWnd, uMsg, wParam, lParam ) )
		return true;

	return false;
}

bool GfxGameScene::ProcessMouseEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHit, bool& bIsDragging )
{
	if( FALSE == CCursor::GetInstance().GetShowCursor() )
		return false;

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

			m_fMouseX = fMouseX;
			m_fMouseY = fMouseY;
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

bool GfxGameScene::ProcessKeyEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_SYSKEYDOWN :
	case WM_SYSKEYUP :
	case WM_KEYDOWN :
	case WM_KEYUP :
		{

			// TODO :  �ӽ� ������ڵ� ���� ����
			if( language::DEFAULT != RENDERPARAM::emLangSet &&
				language::KOREAN != RENDERPARAM::emLangSet )
			{
				if( m_bCompostioning == true)
					return false ;

				else if (  m_bCopostionCheck == false )
				{
					m_bCopostionCheck = true;
					return false;
				}

				if( m_bCopostionCheck == false )
					return false;
			}

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

bool GfxGameScene::ProcessIMEEvent( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_KEYDOWN:
	case WM_IME_COMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_NOTIFY:
	case WM_INPUTLANGCHANGE:
	case WM_IME_CHAR:
		{
			GFx::IMEWin32Event gfxEvent( GFx::IMEEvent::IME_Default, (UPInt) hWnd, uMsg, wParam, lParam, true );
			if( m_pMovie->HandleEvent( gfxEvent ) & GFx::Movie::HE_NoDefaultAction )
				return true;
		}
		break;
	}

	return false;
}

KeyModifiers GfxGameScene::UpdateModifiers( bool extendedKeyFlag )
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

void GfxGameScene::ProcessKey( unsigned vkCode, bool downFlag, bool extendedKeyFlag )
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

void GfxGameScene::OnKey( unsigned controllerIndex, KeyCode keyCode, unsigned wcharCode, bool downFlag, KeyModifiers mods )
{
	if( NULL == m_pMovie )
		return;

	// �ؽ�Ʈ ����� ���⼭ �� ��� �ѹ��� ������ ��찡 ����.
	// GfxClipboard ���� ó���ϵ��� ������.
	if( true == GfxInterface::GetInstance()->GetFocusInTextInput() &&
		true == mods.IsCtrlPressed() )
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

					//std::wstring str = GfxInterface::GetInstance()->EncodeText( strClipBoardText );
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

bool GfxGameScene::GetCapslockKeyState()
{
	if ( NSGFXKEYCODE::GetModifier() & NSGFXKEYCODE::KM_Caps )
		return true;

	return false;
}

void GfxGameScene::SetCapslockKeyState(bool bDown)
{
	bool bOn((::GetKeyState(VK_CAPITAL) & 1));
	if ( bDown != bOn )
	{
		keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
		keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
	}
}