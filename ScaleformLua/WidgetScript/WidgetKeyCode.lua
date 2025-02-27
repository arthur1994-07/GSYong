-- 키 입력 코드 정의

-- 영문 키
KEYCODE_A = 65	
KEYCODE_B = 66	
KEYCODE_C = 67	
KEYCODE_D = 68	
KEYCODE_E = 69	
KEYCODE_F = 70	
KEYCODE_G = 71	
KEYCODE_H = 72	
KEYCODE_I = 73	
KEYCODE_J = 74	
KEYCODE_K = 75	
KEYCODE_L = 76	
KEYCODE_M = 77	
KEYCODE_N = 78	
KEYCODE_O = 79	
KEYCODE_P = 80	
KEYCODE_Q = 81	
KEYCODE_R = 82	
KEYCODE_S = 83	
KEYCODE_T = 84	
KEYCODE_U = 85	
KEYCODE_V = 86	
KEYCODE_W = 87	
KEYCODE_X = 88	
KEYCODE_Y = 89	
KEYCODE_Z = 90	


KEYCODE_SLASH = 191				-- /
KEYCODE_BACKSLASH = 220			-- \
KEYCODE_QUOTE = 222				-- '
KEYCODE_BACKQUOTE = 192			-- `
KEYCODE_PERIOD = 190			-- .
KEYCODE_COMMA = 188				-- ,
KEYCODE_LEFTBRACKET = 219		-- [
KEYCODE_RIGHTBRACKET = 221		-- ]
KEYCODE_MINUS = 189				-- -
KEYCODE_EQUAL = 187				-- =
KEYCODE_SEMICOLON = 186			-- ;


KEYCODE_ALT = 18				-- Alt
KEYCODE_BACKSPACE = 8			-- Backspace
KEYCODE_CONTROL = 17			-- Ctrl
KEYCODE_CAPS_LOCK = 20			-- CapsLock
KEYCODE_ESCAPE = 27				-- ESC
KEYCODE_ENTER = 13				-- Enter
KEYCODE_SHIFT = 16				-- Shift
KEYCODE_SPACE = 32				-- Space
KEYCODE_TAB = 9					-- Tab


KEYCODE_INSERT = 45				-- Insert
KEYCODE_DELETE = 46				-- Delete
KEYCODE_HOME = 36				-- Home
KEYCODE_END = 35				-- End
KEYCODE_PAGE_UP = 33			-- PageUp
KEYCODE_PAGE_DOWN = 34			-- PageDown


-- 방향키
KEYCODE_LEFT = 37
KEYCODE_RIGHT = 39
KEYCODE_UP = 38
KEYCODE_DOWN = 40

-- Function 키
KEYCODE_F1 = 112
KEYCODE_F10 = 121
KEYCODE_F11 = 122
KEYCODE_F12 = 123
KEYCODE_F13 = 124
KEYCODE_F14 = 125
KEYCODE_F15 = 126
KEYCODE_F2 = 113
KEYCODE_F3 = 114
KEYCODE_F4 = 115
KEYCODE_F5 = 116
KEYCODE_F6 = 117
KEYCODE_F7 = 118
KEYCODE_F8 = 119
KEYCODE_F9 = 120

-- 숫자
KEYCODE_NUMBER_0 = 48
KEYCODE_NUMBER_1 = 49
KEYCODE_NUMBER_2 = 50
KEYCODE_NUMBER_3 = 51
KEYCODE_NUMBER_4 = 52
KEYCODE_NUMBER_5 = 53
KEYCODE_NUMBER_6 = 54
KEYCODE_NUMBER_7 = 55
KEYCODE_NUMBER_8 = 56
KEYCODE_NUMBER_9 = 57

-- 숫자 패드
KEYCODE_NUMPAD = 21
KEYCODE_NUMPAD_NUMLOCK = 144
KEYCODE_NUMPAD_ADD = 107
KEYCODE_NUMPAD_DECIMAL = 110
KEYCODE_NUMPAD_DIVIDE = 111
KEYCODE_NUMPAD_ENTER = 108
KEYCODE_NUMPAD_MULTIPLY = 106
KEYCODE_NUMPAD_SUBTRACT = 109
KEYCODE_NUMPAD_0 = 96
KEYCODE_NUMPAD_1 = 97
KEYCODE_NUMPAD_2 = 98
KEYCODE_NUMPAD_3 = 99
KEYCODE_NUMPAD_4 = 100
KEYCODE_NUMPAD_5 = 101
KEYCODE_NUMPAD_6 = 102
KEYCODE_NUMPAD_7 = 103
KEYCODE_NUMPAD_8 = 104
KEYCODE_NUMPAD_9 = 105


-- 키코드에 해당하는 문자열
g_tbKyeCodeString = {}
g_tbKyeCodeString[ KEYCODE_A ] = "A"
g_tbKyeCodeString[ KEYCODE_B ] = "B"
g_tbKyeCodeString[ KEYCODE_C ] = "C"
g_tbKyeCodeString[ KEYCODE_D ] = "D"
g_tbKyeCodeString[ KEYCODE_E ] = "E"
g_tbKyeCodeString[ KEYCODE_F ] = "F"
g_tbKyeCodeString[ KEYCODE_G ] = "G"
g_tbKyeCodeString[ KEYCODE_H ] = "H"
g_tbKyeCodeString[ KEYCODE_I ] = "I"
g_tbKyeCodeString[ KEYCODE_J ] = "J"
g_tbKyeCodeString[ KEYCODE_K ] = "K"
g_tbKyeCodeString[ KEYCODE_L ] = "L"
g_tbKyeCodeString[ KEYCODE_M ] = "M"
g_tbKyeCodeString[ KEYCODE_N ] = "N"
g_tbKyeCodeString[ KEYCODE_O ] = "O"
g_tbKyeCodeString[ KEYCODE_P ] = "P"
g_tbKyeCodeString[ KEYCODE_Q ] = "Q"
g_tbKyeCodeString[ KEYCODE_R ] = "R"
g_tbKyeCodeString[ KEYCODE_S ] = "S"
g_tbKyeCodeString[ KEYCODE_T ] = "T"
g_tbKyeCodeString[ KEYCODE_U ] = "U"
g_tbKyeCodeString[ KEYCODE_V ] = "V"
g_tbKyeCodeString[ KEYCODE_W ] = "W"
g_tbKyeCodeString[ KEYCODE_X ] = "X"
g_tbKyeCodeString[ KEYCODE_Y ] = "Y"
g_tbKyeCodeString[ KEYCODE_Z ] = "Z"
g_tbKyeCodeString[ KEYCODE_SLASH ] = "/"
g_tbKyeCodeString[ KEYCODE_BACKSLASH ] = "\\"
g_tbKyeCodeString[ KEYCODE_QUOTE ] = "'"
g_tbKyeCodeString[ KEYCODE_BACKQUOTE ] = "`"
g_tbKyeCodeString[ KEYCODE_PERIOD ] = "."
g_tbKyeCodeString[ KEYCODE_COMMA ] = ","
g_tbKyeCodeString[ KEYCODE_LEFTBRACKET ] = "["
g_tbKyeCodeString[ KEYCODE_RIGHTBRACKET ] = "]"
g_tbKyeCodeString[ KEYCODE_MINUS ] = "-"
g_tbKyeCodeString[ KEYCODE_EQUAL ] = "="
g_tbKyeCodeString[ KEYCODE_SEMICOLON ] = ";"
g_tbKyeCodeString[ KEYCODE_ALT ] = "ALT"
g_tbKyeCodeString[ KEYCODE_BACKSPACE ] = "BACKSPACE"
g_tbKyeCodeString[ KEYCODE_CONTROL ] = "CTRL"
g_tbKyeCodeString[ KEYCODE_CAPS_LOCK ] = "CAPS LOCK"
g_tbKyeCodeString[ KEYCODE_ESCAPE ] = "ESC"
g_tbKyeCodeString[ KEYCODE_ENTER ] = "ENTER"
g_tbKyeCodeString[ KEYCODE_SHIFT ] = "SHIFT"
g_tbKyeCodeString[ KEYCODE_SPACE ] = "SPACE"
g_tbKyeCodeString[ KEYCODE_TAB ] = "TAB"
g_tbKyeCodeString[ KEYCODE_INSERT ] = "INSERT"
g_tbKyeCodeString[ KEYCODE_DELETE ] = "DELETE"
g_tbKyeCodeString[ KEYCODE_HOME ] = "HOME"
g_tbKyeCodeString[ KEYCODE_END ] = "END"
g_tbKyeCodeString[ KEYCODE_PAGE_UP ] = "PAGE UP"
g_tbKyeCodeString[ KEYCODE_PAGE_DOWN ] = "PAGE DOWN"
g_tbKyeCodeString[ KEYCODE_LEFT ] = "LEFT"
g_tbKyeCodeString[ KEYCODE_RIGHT ] = "RIGHT"
g_tbKyeCodeString[ KEYCODE_UP ] = "UP"
g_tbKyeCodeString[ KEYCODE_DOWN ] = "DOWN"
g_tbKyeCodeString[ KEYCODE_F1 ] = "F1"
g_tbKyeCodeString[ KEYCODE_F10 ] = "F10"
g_tbKyeCodeString[ KEYCODE_F11 ] = "F11"
g_tbKyeCodeString[ KEYCODE_F12 ] = "F12"
g_tbKyeCodeString[ KEYCODE_F13 ] = "F13"
g_tbKyeCodeString[ KEYCODE_F14 ] = "F14"
g_tbKyeCodeString[ KEYCODE_F15 ] = "F15"
g_tbKyeCodeString[ KEYCODE_F2 ] = "F2"
g_tbKyeCodeString[ KEYCODE_F3 ] = "F3"
g_tbKyeCodeString[ KEYCODE_F4 ] = "F4"
g_tbKyeCodeString[ KEYCODE_F5 ] = "F5"
g_tbKyeCodeString[ KEYCODE_F6 ] = "F6"
g_tbKyeCodeString[ KEYCODE_F7 ] = "F7"
g_tbKyeCodeString[ KEYCODE_F8 ] = "F8"
g_tbKyeCodeString[ KEYCODE_F9 ] = "F9"
g_tbKyeCodeString[ KEYCODE_NUMBER_0 ] = "0"
g_tbKyeCodeString[ KEYCODE_NUMBER_1 ] = "1"
g_tbKyeCodeString[ KEYCODE_NUMBER_2 ] = "2"
g_tbKyeCodeString[ KEYCODE_NUMBER_3 ] = "3"
g_tbKyeCodeString[ KEYCODE_NUMBER_4 ] = "4"
g_tbKyeCodeString[ KEYCODE_NUMBER_5 ] = "5"
g_tbKyeCodeString[ KEYCODE_NUMBER_6 ] = "6"
g_tbKyeCodeString[ KEYCODE_NUMBER_7 ] = "7"
g_tbKyeCodeString[ KEYCODE_NUMBER_8 ] = "8"
g_tbKyeCodeString[ KEYCODE_NUMBER_9 ] = "9"
g_tbKyeCodeString[ KEYCODE_NUMPAD ] = "NUMPAD"
g_tbKyeCodeString[ KEYCODE_NUMPAD_NUMLOCK ] = "NumLock"
g_tbKyeCodeString[ KEYCODE_NUMPAD_ADD ] = "+"
g_tbKyeCodeString[ KEYCODE_NUMPAD_DECIMAL ] = "."
g_tbKyeCodeString[ KEYCODE_NUMPAD_DIVIDE ] = "/"
g_tbKyeCodeString[ KEYCODE_NUMPAD_ENTER ] = "ENTER"
g_tbKyeCodeString[ KEYCODE_NUMPAD_MULTIPLY ] = "*"
g_tbKyeCodeString[ KEYCODE_NUMPAD_SUBTRACT ] = "-"
g_tbKyeCodeString[ KEYCODE_NUMPAD_0 ] = "0"
g_tbKyeCodeString[ KEYCODE_NUMPAD_1 ] = "1"
g_tbKyeCodeString[ KEYCODE_NUMPAD_2 ] = "2"
g_tbKyeCodeString[ KEYCODE_NUMPAD_3 ] = "3"
g_tbKyeCodeString[ KEYCODE_NUMPAD_4 ] = "4"
g_tbKyeCodeString[ KEYCODE_NUMPAD_5 ] = "5"
g_tbKyeCodeString[ KEYCODE_NUMPAD_6 ] = "6"
g_tbKyeCodeString[ KEYCODE_NUMPAD_7 ] = "7"
g_tbKyeCodeString[ KEYCODE_NUMPAD_8 ] = "8"
g_tbKyeCodeString[ KEYCODE_NUMPAD_9 ] = "9"
