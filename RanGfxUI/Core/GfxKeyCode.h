#pragma once

namespace NSGFXKEYCODE
{
	struct SKeyCode
	{
		WPARAM					nWinKeyCode;
		Scaleform::Key::Code	emSFKeyCode;
	};
	const SKeyCode g_sKeyCodeMap[] = 
	{
		{ VK_BACK,			Key::Backspace },
		{ VK_TAB,			Key::Tab },
		{ VK_CLEAR,			Key::Clear },
		{ VK_RETURN,		Key::Return },
		{ VK_SHIFT,			Key::Shift },
		{ VK_CONTROL,		Key::Control },
		{ VK_MENU,			Key::Alt },
		{ VK_PAUSE,			Key::Pause },
		{ VK_CAPITAL,		Key::CapsLock },
		{ VK_ESCAPE,		Key::Escape },
		{ VK_SPACE,			Key::Space },
		{ VK_PRIOR,			Key::PageUp },
		{ VK_NEXT,			Key::PageDown },
		{ VK_END,			Key::End },
		{ VK_HOME,			Key::Home },
		{ VK_LEFT,			Key::Left },
		{ VK_UP,			Key::Up },
		{ VK_RIGHT,			Key::Right },
		{ VK_DOWN,			Key::Down },
		{ VK_INSERT,		Key::Insert },
		{ VK_DELETE,		Key::Delete },
		{ VK_HELP,			Key::Help },
		{ VK_NUMLOCK,		Key::NumLock },
		{ VK_SCROLL,		Key::ScrollLock },
		{ VK_OEM_1,			Key::Semicolon },
		{ VK_OEM_PLUS,		Key::Equal },
		{ VK_OEM_COMMA,		Key::Comma },
		{ VK_OEM_MINUS,		Key::Minus },
		{ VK_OEM_PERIOD,	Key::Period },
		{ VK_OEM_2,			Key::Slash },
		{ VK_OEM_3,			Key::Bar },
		{ VK_OEM_4,			Key::BracketLeft },
		{ VK_OEM_5,			Key::Backslash },
		{ VK_OEM_6,			Key::BracketRight },
		{ VK_OEM_7,			Key::Quote },
		{ VK_OEM_AX,  		Key::OEM_AX },
		{ VK_OEM_102,  		Key::OEM_102 },
		{ VK_ICO_HELP, 		Key::ICO_HELP },
		{ VK_ICO_00,  		Key::ICO_00 },
		{ VK_SNAPSHOT, 		Key::PrintScreen },

		{ 0,           		Key::None }
	};

	enum EMKEYMODIFIER
	{
		KM_Control	= 0x1,
		KM_Alt		= 0x2,
		KM_Shift	= 0x4,
		KM_Num		= 0x8,
		KM_Caps		= 0x10,
		KM_Scroll	= 0x20
	};

	static int GetModifier()
	{
		int nModifier = 0;

		if( GetKeyState(VK_SHIFT) & 0x8000 )
			nModifier |= KM_Shift;
		if( GetKeyState(VK_CONTROL) & 0x8000 )
			nModifier |= KM_Control;
		if( GetKeyState(VK_MENU) & 0x8000 )
			nModifier |= KM_Alt;
		if( GetKeyState(VK_NUMLOCK) & 0x1 )
			nModifier |= KM_Num;
		if( GetKeyState(VK_CAPITAL) & 0x1 )
			nModifier |= KM_Caps;
		if( GetKeyState(VK_SCROLL) & 0x1 )
			nModifier |= KM_Scroll;

		return nModifier;
	}
}