
-- 인덱스에 해당하는 단축키 타입
g_tbHotKeyCfg_IndexType = {}
g_tbHotKeyCfg_IndexType[ 1 ] = EMSHORTCUTS_QUICKSLOT0
g_tbHotKeyCfg_IndexType[ 2 ] = EMSHORTCUTS_QUICKSLOT1
g_tbHotKeyCfg_IndexType[ 3 ] = EMSHORTCUTS_QUICKSLOT2
g_tbHotKeyCfg_IndexType[ 4 ] = EMSHORTCUTS_QUICKSLOT3
g_tbHotKeyCfg_IndexType[ 5 ] = EMSHORTCUTS_QUICKSLOT4
g_tbHotKeyCfg_IndexType[ 6 ] = EMSHORTCUTS_QUICKSLOT5
g_tbHotKeyCfg_IndexType[ 7 ] = EMSHORTCUTS_SKILLSLOT0
g_tbHotKeyCfg_IndexType[ 8 ] = EMSHORTCUTS_SKILLSLOT1
g_tbHotKeyCfg_IndexType[ 9 ] = EMSHORTCUTS_SKILLSLOT2
g_tbHotKeyCfg_IndexType[ 10 ] = EMSHORTCUTS_SKILLSLOT3
g_tbHotKeyCfg_IndexType[ 11 ] = EMSHORTCUTS_SKILLSLOT4
g_tbHotKeyCfg_IndexType[ 12 ] = EMSHORTCUTS_SKILLSLOT5
g_tbHotKeyCfg_IndexType[ 13 ] = EMSHORTCUTS_SKILLSLOT6
g_tbHotKeyCfg_IndexType[ 14 ] = EMSHORTCUTS_SKILLSLOT7
g_tbHotKeyCfg_IndexType[ 15 ] = EMSHORTCUTS_SKILLSLOT8
g_tbHotKeyCfg_IndexType[ 16 ] = EMSHORTCUTS_SKILLSLOT9
g_tbHotKeyCfg_IndexType[ 17 ] = 0							-- 일반 공격 단축키 (사용안함.)
g_tbHotKeyCfg_IndexType[ 18 ] = EMSHORTCUTS_INVENTORY
g_tbHotKeyCfg_IndexType[ 19 ] = EMSHORTCUTS_CHARACTER
g_tbHotKeyCfg_IndexType[ 20 ] = EMSHORTCUTS_SKILL
g_tbHotKeyCfg_IndexType[ 21 ] = EMSHORTCUTS_PARTY
g_tbHotKeyCfg_IndexType[ 22 ] = EMSHORTCUTS_QUEST
g_tbHotKeyCfg_IndexType[ 23 ] = EMSHORTCUTS_CLUB
g_tbHotKeyCfg_IndexType[ 24 ] = EMSHORTCUTS_FRIEND
g_tbHotKeyCfg_IndexType[ 25 ] = EMSHORTCUTS_MINIMAP
g_tbHotKeyCfg_IndexType[ 26 ] = EMSHORTCUTS_CHATMACRO
g_tbHotKeyCfg_IndexType[ 27 ] = EMSHORTCUTS_ITEMBANK
g_tbHotKeyCfg_IndexType[ 28 ] = EMSHORTCUTS_POINTSHOP
g_tbHotKeyCfg_IndexType[ 29 ] = EMSHORTCUTS_RUNMODE
g_tbHotKeyCfg_IndexType[ 30 ] = EMSHORTCUTS_HELP
g_tbHotKeyCfg_IndexType[ 31 ] = EMSHORTCUTS_PET
g_tbHotKeyCfg_IndexType[ 32 ] = EMSHORTCUTS_PEACEMODE
g_tbHotKeyCfg_IndexType[ 33 ] = EMSHORTCUTS_FOURCEDPKMODE
g_tbHotKeyCfg_IndexType[ 34 ] = EMSHORTCUTS_PRODUCT

g_strHotKeyCfg_NONE = "(None)"	-- 단축키 설정 없음

g_bHotKeyCfg_IsOpen = false

g_nHotKeyCfg_InputKeyCode = -1

-- 단축키 설정
function LoadHotKeyCfg()

	local str = ""
	local strInst = ""

	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_WIDGET_CLOSE, "CloseHotKeyCfg", "", "" )

	-- 탭 설정
	local strTabData = GetGameWordText( "KEYSETTINGWINDOW_TAP_TYPE", 0 ) .. ";" .. 
		GetGameWordText( "KEYSETTINGWINDOW_TAP_TYPE", 1 ) .. ";"
	SetData( WIDGET_ID_HOTKEYCFG, strTabData, "tabPage", false )
	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_INDEX_CLICK_CHANGE, "Ev_HotKeyCfg_tabPage", "index", "tabPage" )	
	
	-- 텍스트, 이벤트 설정
	local nInstIndex = 0
	
	-- 퀵슬롯
	for i = 0, 5, 1 do
	
		str = GetGameWordText( "KEYSETTING_QUICKSLOT", i )
		strInst = "config.inputHotKey" .. nInstIndex
		
		SetString( WIDGET_ID_HOTKEYCFG, "text", str, "config.labelHotKey" .. tostring( nInstIndex ) )
		SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_KEY_UP, "Ev_HotKeyCfg_input", "keyCode", strInst )
		SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_FOCUS_OUT, "Ev_HotKeyCfg_inputFocusOut", "", strInst, nInstIndex )
		
		nInstIndex = nInstIndex + 1
		
	end
	
	-- 스킬 슬롯
	for i = 0, 9, 1 do
	
		str = GetGameWordText( "KEYSETTING_QUICKSKILL", i )
		strInst = "config.inputHotKey" .. nInstIndex
		
		SetString( WIDGET_ID_HOTKEYCFG, "text", str, "config.labelHotKey" .. tostring( nInstIndex ) )
		SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_KEY_UP, "Ev_HotKeyCfg_input", "keyCode", strInst )
		SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_FOCUS_OUT, "Ev_HotKeyCfg_inputFocusOut", "", strInst, nInstIndex )
		
		nInstIndex = nInstIndex + 1
		
	end
	
	-- 일반 공격 (사용안함.. 왜 있는지 모르겠음. 일단 기존 UI와 똑같이 만듦.)
	str = GetGameWordText( "KEY_SETTING_ATTACK", 0 )
	strInst = "config.inputHotKey" .. nInstIndex
	
	SetString( WIDGET_ID_HOTKEYCFG, "text", str, "config.labelHotKey" .. tostring( nInstIndex ) )
	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_KEY_UP, "Ev_HotKeyCfg_input", "keyCode", strInst )
	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_FOCUS_OUT, "Ev_HotKeyCfg_inputFocusOut", "", strInst, nInstIndex )
	
	if 0 == g_InputMan:GetCurrentControlType() then	
	
		SetString( WIDGET_ID_HOTKEYCFG, "defaultText", "Mouse(L)", "config.inputHotKey" .. tostring( nInstIndex ) )	
		
	else
	
		SetString( WIDGET_ID_HOTKEYCFG, "defaultText", "A", "config.inputHotKey" .. tostring( nInstIndex ) )
		
	end
	
	SetBool( WIDGET_ID_HOTKEYCFG, "enabled", false, "config.inputHotKey" .. tostring( nInstIndex ) )		
	
	nInstIndex = nInstIndex + 1
	
	-- 메뉴 슬롯
	for i = 0, 16, 1 do
	
		str = GetGameWordText( "KEYSETTING_MENUSHOTCUT", i )
		strInst = "config.inputHotKey" .. nInstIndex
		
		SetString( WIDGET_ID_HOTKEYCFG, "text", str, "config.labelHotKey" .. tostring( nInstIndex ) )
		SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_KEY_UP, "Ev_HotKeyCfg_input", "keyCode", strInst )
		SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_FOCUS_OUT, "Ev_HotKeyCfg_inputFocusOut", "", strInst, nInstIndex )
		
		nInstIndex = nInstIndex + 1
		
	end
	
	SetString( WIDGET_ID_HOTKEYCFG, "label", GetGameWordText( "KEYSETTING_APPLYOK", 0 ), "btnDefault" )
	SetString( WIDGET_ID_HOTKEYCFG, "label", GetGameWordText( "KEYSETTING_APPLYOK", 1 ), "btnApply" )
	SetString( WIDGET_ID_HOTKEYCFG, "label", GetGameWordText( "KEYSETTING_APPLYOK", 2 ), "btnOK" )
	
	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_BUTTON_CLICK, "Ev_HotKeyCfg_btnDefault", "", "btnDefault" )
	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_BUTTON_CLICK, "Ev_HotKeyCfg_btnApply", "", "btnApply" )
	SetEvent( WIDGET_ID_HOTKEYCFG, EVENT_BUTTON_CLICK, "Ev_HotKeyCfg_btnOK", "", "btnOK" )
	
	SetInt( WIDGET_ID_HOTKEYCFG, "selectedIndex", g_InputMan:GetCurrentControlType(), "tabPage" )

	if g_UserFeatures:IsUsingRenewProduct() == false then
		for nKey, nValue in pairs(g_tbHotKeyCfg_IndexType) do
			if nValue == EMSHORTCUTS_PRODUCT then
				SetBool( WIDGET_ID_HOTKEYCFG, "visible", false, "config.inputHotKey" .. tostring(nKey - 1) )
				SetBool( WIDGET_ID_HOTKEYCFG, "visible", false, "config.labelHotKey" .. tostring(nKey - 1) )
			end
		end	
	end
	
end

function OpenHotKeyCfg()

	g_bHotKeyCfg_IsOpen = true

end

function CloseHotKeyCfg()

	g_bHotKeyCfg_IsOpen = false

end

function IsOpenHotKeyCfg()

	return g_bHotKeyCfg_IsOpen

end

-- 키 코드 문자열 설정
function HotKeyCfg_SetKeyCodeString( nKeyCode, nIndex )

	-- 텍스트 비워주기
	local strInst = "config.inputHotKey" .. tostring( nIndex )
	SetString( WIDGET_ID_HOTKEYCFG, "text", "", strInst )
	
	-- 일부 키는 단축키로 사용할 수 없음.
	if  KEYCODE_ESCAPE == nKeyCode or KEYCODE_F1 == nKeyCode or KEYCODE_F2 == nKeyCode or KEYCODE_F3 == nKeyCode or 
		KEYCODE_F4 == nKeyCode or KEYCODE_F5 == nKeyCode or KEYCODE_F6 == nKeyCode or KEYCODE_F7 == nKeyCode or 
		KEYCODE_F8 == nKeyCode or KEYCODE_F9 == nKeyCode or KEYCODE_F10 == nKeyCode or KEYCODE_F11 == nKeyCode or 
		KEYCODE_F12 == nKeyCode or KEYCODE_INSERT == nKeyCode or KEYCODE_DELETE == nKeyCode or KEYCODE_HOME == nKeyCode or 
		KEYCODE_END == nKeyCode or KEYCODE_PAGE_UP == nKeyCode or KEYCODE_PAGE_DOWN == nKeyCode or KEYCODE_LEFT == nKeyCode or
		KEYCODE_RIGHT == nKeyCode or KEYCODE_UP == nKeyCode or KEYCODE_DOWN == nKeyCode or KEYCODE_ALT == nKeyCode or
		KEYCODE_CONTROL == nKeyCode or KEYCODE_CAPS_LOCK == nKeyCode or KEYCODE_NUMPAD_ENTER == nKeyCode or KEYCODE_TAB == nKeyCode or
		KEYCODE_SHIFT == nKeyCode or KEYCODE_BACKSPACE == nKeyCode or KEYCODE_ENTER == nKeyCode then
		return
	end
	
	if  KEYCODE_O == nKeyCode  then
		
		local keycodeErrorstr = StringFormat( GetGameInternalText( "MODAL_KEY_SETTING_DISABLE", 0 ), " O " )
		OpenMsgBox( MSG_BOX_TYPE_OK, keycodeErrorstr, Event_MsgBox_Close )
		return ;
		
	end

	-- 키코드에 해당하는 문자열 찾기
	local strKeyString = g_tbKyeCodeString[ nKeyCode ]
	if nil == strKeyString then
		return
	end
	
	-- 단축키 없음. 
	if KEYCODE_SPACE == nKeyCode or KEYCODE_NUMPAD_ADD == nKeyCode then
		strKeyString = g_strHotKeyCfg_NONE
	end
	
	-- 텍스트 설정
	SetString( WIDGET_ID_HOTKEYCFG, "defaultText", strKeyString, strInst )
	
	g_nHotKeyCfg_InputKeyCode = -1

end

-- 중복된 키 입력값이 있는지 확인
function HotKeyCfg_CheckConfig()

	local tbInputData = {}
	for i = 0, 33, 1 do
	
		local strKey = GetString( WIDGET_ID_HOTKEYCFG, "defaultText", "config.inputHotKey" .. tostring( i ) )
		if 0 == string.len( strKey ) then
			return false
		end

		-- 단축키 설정이 없는 경우는 검사하지 않는다.
		if strKey ~= g_strHotKeyCfg_NONE then
		
			-- 같은 키가 있는지 확인하고 있다면 메시지 박스 띄워줌.
			if nil == tbInputData[ strKey ] then
				tbInputData[ strKey ] = i
			else
			
				local strMsg = "'" .. strKey .. "'" .. GetGameInternalText( "MODAL_KEY_SETTING_ERROR", 0 )
				OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_MsgBox_Close )
				
				return false
				
			end
		
		end
	
	end

	return true

end

-- 설정값 저장
function HotKeyCfg_SaveConfig()

	-- 중복된 키 설정값이 있음.
	if false == HotKeyCfg_CheckConfig() then
		return false
	end

	for i = 0, 33, 1 do
	
		-- 16번 일반 공격 단축키 사용안함.
		if 16 ~= i then
			local strKey = GetString( WIDGET_ID_HOTKEYCFG, "defaultText", "config.inputHotKey" .. tostring( i ) )
			g_InputMan:SetShortcutsDataFromString( g_tbHotKeyCfg_IndexType[ i + 1 ], strKey )
		end

	end
	
	return true

end

-- 기본설정 값으로 설정
function HotKeyCfg_SetDefault( nIndex )

	for i = 0, 33, 1 do
		-- 16번 일반 공격 단축키 사용안함.
		if 16 ~= i then
			local strKey = g_InputMan:GetDefaultShortcutsToString( nIndex, g_tbHotKeyCfg_IndexType[ i + 1 ] )
			SetString( WIDGET_ID_HOTKEYCFG, "defaultText", strKey, "config.inputHotKey" .. tostring( i ) )
		else
		
			if 0 == nIndex then
				SetString( WIDGET_ID_HOTKEYCFG, "defaultText", "Mouse(L)", "config.inputHotKey" .. tostring( i ) )
			else
				SetString( WIDGET_ID_HOTKEYCFG, "defaultText", "A", "config.inputHotKey" .. tostring( i ) )
			end
		
		end
	end

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탭 이벤트
function Ev_HotKeyCfg_tabPage( nIndex )

	-- 설정한 키 타입과 같으면 설정한 키값 가져오기
	if nIndex == g_InputMan:GetCurrentControlType() then
	
		for i = 0, 33, 1 do
			-- 16번 일반 공격 단축키 사용안함.
			if 16 ~= i then
				local strKey = g_InputMan:GetShortcutsToString( g_tbHotKeyCfg_IndexType[ i + 1 ] )
				SetString( WIDGET_ID_HOTKEYCFG, "defaultText", strKey, "config.inputHotKey" .. tostring( i ) )
			else
			
				if 0 == nIndex then
					SetString( WIDGET_ID_HOTKEYCFG, "defaultText", "Mouse(L)", "config.inputHotKey" .. tostring( i ) )
				else
					SetString( WIDGET_ID_HOTKEYCFG, "defaultText", "A", "config.inputHotKey" .. tostring( i ) )
				end
			
			end
		end
	
	-- 아니면 해당 타입의 기본 설정 키값 가져오기
	else
	
		HotKeyCfg_SetDefault( nIndex )
	
	end

end

-- 기본설정 버튼
function Ev_HotKeyCfg_btnDefault()
	HotKeyCfg_SetDefault( GetInt( WIDGET_ID_HOTKEYCFG, "selectedIndex", "tabPage" ) )
end

-- 적용 버튼
function Ev_HotKeyCfg_btnApply()

	-- 키 타입이 변경되었다면 컨트롤 타입 초기화
	local nSelect = GetInt( WIDGET_ID_HOTKEYCFG, "selectedIndex", "tabPage" )
	if nSelect ~= g_InputMan:GetCurrentControlType() then
		g_InputMan:InitControlType( nSelect )
	end
	
	-- 단축키 설정 저장
	HotKeyCfg_SaveConfig()
	-- 스킬 단축키 슬롯 텍스트 변경
	SQS_SetSlotText()
	-- 아이템 단축키 슬롯 텍스트 변경
	EV_IQS_SetSlotText()
	-- 퀵메뉴창 단축키 텍스트 변경
	Ev_QuickMenu_HotKeyText()
end

-- 확인 버튼
function Ev_HotKeyCfg_btnOK()

	-- 키 타입이 변경되었다면 컨트롤 타입 초기화
	local nSelect = GetInt( WIDGET_ID_HOTKEYCFG, "selectedIndex", "tabPage" )
	if nSelect ~= g_InputMan:GetCurrentControlType() then
		g_InputMan:InitControlType( nSelect )
	end
	
	-- 단축키 설정 저장, 성공하면 UI 닫기
	if true == HotKeyCfg_SaveConfig() then
		-- 스킬 단축키 슬롯 텍스트 변경
		SQS_SetSlotText()
		-- 아이템 단축키 슬롯 텍스트 변경
		EV_IQS_SetSlotText()
		-- 퀵메뉴창 단축키 텍스트 변경
		Ev_QuickMenu_HotKeyText()
		CloseWidget( WIDGET_ID_HOTKEYCFG )
	end
	
end

-- 인풋 이벤트
function Ev_HotKeyCfg_input( nKeyCode )

	g_nHotKeyCfg_InputKeyCode = nKeyCode
	SetFocus( -1, "" )
	
end

-- 포커스 아웃
function Ev_HotKeyCfg_inputFocusOut( strIndex )

	HotKeyCfg_SetKeyCodeString( g_nHotKeyCfg_InputKeyCode, tonumber( strIndex ) )

end