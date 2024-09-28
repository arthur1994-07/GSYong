g_VrKb_OpenUI = ""		-- 가상 키보드 외에 활성화 되어 있는 UI
g_VrKb_SelectInst = ""	-- 활성화 되어 있는 UI 에서 선택된 인스턴스
g_VrKb_TAP = ""			-- 활성화 되어 있는 UI 에서 선택된 인스턴스 다음 인스턴스 값 넣어줌
g_OpenWidget_To_VrKb = WIDGET_ID_VIRTUARKEYBOARD


-- 가상 키보드 로드
function LoadVirtuarKeyBoard()
	
	for i = 1, 26, 1 do
		SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_BUTTON_CLICK, "Click_VrKb_SmallButton", "", "Button_Small.Button_s" .. tostring(i), i )
	end
	for i = 1, 26, 1 do
		SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_BUTTON_CLICK, "Click_VrKb_CapitalButton", "", "Button_Capital.Button_C" .. tostring(i), i )
	end
	for i = 1, 10, 1 do
		SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_BUTTON_CLICK, "Click_VrKb_NumberButton", "", "Button_" .. tostring(i), i )
	end
	
	SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_BUTTON_CLICK, "Click_VrKb_CapsLock", "", "Button_CapsLock" )
	SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_BUTTON_CLICK, "Click_VrKb_BackSpace", "", "Button_back" )
	SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_BUTTON_CLICK, "Click_VrKb_TAP", "", "Button_TAP" )
	
	VirtuarKeyBoard_Pos( )
	
	SetEvent(WIDGET_ID_VIRTUARKEYBOARD, EVENT_WIDGET_CLOSE, "Ev_VKB_WidgetClose", "", "" )
	
	InsertFrameMove( VKB_FrameMove )
end

function  Ev_VKB_WidgetClose()
	RemoveFrameMove( VKB_FrameMove )
end

function VKB_FrameMove(fElapsedTime)
	local bVisible = GetBool( WIDGET_ID_VIRTUARKEYBOARD, "selected", "Button_CapsLock")
	local bCapsKey = GetCapslockKeyState()
	if bVisible ~= bCapsKey then
		--Click_VrKb_CapsLock()
		SetBool( WIDGET_ID_VIRTUARKEYBOARD, "selected", bCapsKey, "Button_CapsLock")
		Click_VrKb_CapsLock()		
	end
end

function VirtuarKeyBoard_Pos( )
	if false == IsOpenWidget( WIDGET_ID_VIRTUARKEYBOARD ) then
		return
	end

	local VrKb_PosX = (( GetInt( g_OpenWidget_To_VrKb, "width", "") - GetInt( WIDGET_ID_VIRTUARKEYBOARD, "width", "") ) / 2 ) + GetInt( g_OpenWidget_To_VrKb, "x", "" )
	local VrKb_PosY = GetInt( g_OpenWidget_To_VrKb, "y", "" ) + GetInt( g_OpenWidget_To_VrKb, "height", "" ) + 10
	local nSP = GetServiceProvider()
	if ( nSP == SP_TAIWAN ) then
	
		if g_OpenWidget_To_VrKb == WIDGET_ID_SECONDPASSWORD then
		
			VrKb_PosY = VrKb_PosY - 114
			
		end
		
	end
	
	SetInt( WIDGET_ID_VIRTUARKEYBOARD, "x", VrKb_PosX, "")
	SetInt( WIDGET_ID_VIRTUARKEYBOARD, "y", VrKb_PosY, "")	
end
-- 텝 버튼 클릭
function Click_VrKb_TAP()
	SetFocus( g_VrKb_OpenUI, g_VrKb_TAP )
end
-- CapsLock 버튼을 클릭
function Click_VrKb_CapsLock()
	if false == GetBool( WIDGET_ID_VIRTUARKEYBOARD, "selected", "Button_CapsLock") then		-- Caps Lock 버튼이 눌러지지 않은 상태
		SetBool( WIDGET_ID_VIRTUARKEYBOARD, "visible", true, "Button_Small")
		SetBool( WIDGET_ID_VIRTUARKEYBOARD, "visible", false, "Button_Capital")
		SetCapslockKeyState(false)
	elseif true == GetBool( WIDGET_ID_VIRTUARKEYBOARD, "selected", "Button_CapsLock") then		-- Caps Lock 버튼이 눌러진 상태
		SetBool( WIDGET_ID_VIRTUARKEYBOARD, "visible", false, "Button_Small")
		SetBool( WIDGET_ID_VIRTUARKEYBOARD, "visible", true, "Button_Capital")
		SetCapslockKeyState(true)
	end
end
-- BackSpace ← 버튼을 클릭
function Click_VrKb_BackSpace()
	local TextData = GetString(g_VrKb_OpenUI, "text", g_VrKb_SelectInst)
	local TextDataSum = string.sub( TextData, 1, string.len( TextData ) - 1 )
	if "" ~= g_VrKb_OpenUI then
		SetString(g_VrKb_OpenUI, "text", TextDataSum, g_VrKb_SelectInst)
		SetFocus( g_VrKb_OpenUI, g_VrKb_SelectInst )
	end	
end
-- 가상 키보드로 입력된 값을 어느 UI 에 출력할지 셋팅
function VirtuarKeyBoard_SetUI( OpenUI, SelectInst, TapInst )
	g_VrKb_OpenUI = OpenUI
	g_VrKb_SelectInst = SelectInst
	g_VrKb_TAP = TapInst
end
-- 소문자 알파벳 버튼들 클릭
function Click_VrKb_SmallButton( Index )
	ReturnToUI( GetString(WIDGET_ID_VIRTUARKEYBOARD, "label", "Button_Small.Button_s" .. tostring(Index)) )	-- 해당 버튼 라벨에 있는 값을 가져와 넣어준다.
end
-- 대문자 알파벳 버튼들 클릭
function Click_VrKb_CapitalButton( Index )
	ReturnToUI( GetString(WIDGET_ID_VIRTUARKEYBOARD, "label", "Button_Capital.Button_C" .. tostring(Index)) )	-- 해당 버튼 라벨에 있는 값을 가져와 넣어준다.
end
-- 숫자 버튼들 클릭
function Click_VrKb_NumberButton( Index )
	ReturnToUI( GetString(WIDGET_ID_VIRTUARKEYBOARD, "label", "Button_" .. tostring(Index)) )	-- 해당 버튼 라벨에 있는 값을 가져와 넣어준다.
end

-- 선택되어 있던 UI 에 입력한 값 출력
function ReturnToUI( TextData )
	local TextDataSum = GetString(g_VrKb_OpenUI, "text", g_VrKb_SelectInst) .. TextData
	if "" ~= g_VrKb_OpenUI then
		SetString(g_VrKb_OpenUI, "text", TextDataSum, g_VrKb_SelectInst)
		SetFocus( g_VrKb_OpenUI, g_VrKb_SelectInst )
		
		local nLen = string.len( TextDataSum )
		local strParam = CreateParam( nLen, nLen )
		CallFunction( g_VrKb_OpenUI, "setSelection", strParam, g_VrKb_SelectInst .. ".textField" )
	end
end