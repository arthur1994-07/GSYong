

function LoadUISwitch()
	
	-- 리스트 아이템 셋팅
	UISwitch_SetList()
	
	-- 리스트 클릭 이벤트 설정
	SetEvent( WIDGET_ID_UISWITCH, EVENT_LIST_ITEM_CLICK, "Ev_UISwitch_SelectList", "index", "UISwitchList" )
	-- 전체 선택 제거 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_UISWITCH, EVENT_MOUSE_CLICK, "Ev_UISwitch_UnCheckAllBtn", "", "UnCheckAllBtn" )
	-- 내부 UI 토글 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_UISWITCH, EVENT_MOUSE_CLICK, "Ev_UISwitch_InnerUIOnBtn", "", "InnerUIOnBtn" )
	SetEvent( WIDGET_ID_UISWITCH, EVENT_MOUSE_CLICK, "Ev_UISwitch_InnerUIOffBtn", "", "InnerUIOffBtn" )
	
end

-- 리스트 아이템 데이터 셋팅
function UISwitch_SetList()

	local tbSwitchList = {}	-- 리스트 담을 테이블
	
	for value in pairs( WIDGETS_LIST ) do
	
		local tbInfo = WIDGETS_LIST[ value ]
		
		-- 열려져 있는 UI는 체크상태로 만들기 위함
		local IsWidgetOpen = 0
		if IsOpenWidget( tbInfo[ "wWidgetID" ] ) == true then
			IsWidgetOpen = 1
		end
		
		-- 리스트 아이템 데이터 셋팅
		local tblist = 
		{
			"[" .. tbInfo[ "wWidgetID" ] .. "] " .. tbInfo[ "strDebuggingName" ],
			IsWidgetOpen
		}
		table.insert( tbSwitchList, tblist )
	
	end

	-- 아이템 렌더러 데이터 셋팅( "텍스트 필드", "멀티 셀렉트 여부" )
	local strParam = CreateParam( "label", "multiSelected" )
	SetDataEx( WIDGET_ID_UISWITCH, strParam, tbSwitchList, "UISwitchList", false )

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 리스트 클릭 이벤트
function Ev_UISwitch_SelectList( nIndex )

	-- 체크상태가 변경되기 전의 값이 저장된다.(클릭하기 전 상태의 체크여부)
	local bCheck = GetDataBool(WIDGET_ID_UISWITCH, nIndex, "multiSelected", "UISwitchList")
	
	local tbInfo = WIDGETS_LIST[ nIndex + 1 ]
	if nil == tbInfo then
		return
	end
	
	if false == bCheck then
	
		OpenWidget( tbInfo[ "wWidgetID" ] )
		
	else
	
		CloseWidget( tbInfo[ "wWidgetID" ] )
		
	end
	
end

-- 언체크 버튼 클릭 이벤트
function Ev_UISwitch_UnCheckAllBtn()

	for value in pairs( WIDGETS_LIST ) do
	
		local tbInfo = WIDGETS_LIST[ value ]
		CloseWidget( tbInfo[ "wWidgetID" ] )
		
	end
	
end

-- 내부 UI On 버튼 클릭 이벤트
function Ev_UISwitch_InnerUIOnBtn()
	
	-- 입력 텍스트 추가(입력 텍스트는 커맨트콘솔 창이 켜져있지 않더라도 입력이 가능하다.)
	local strInputText = "dspname 1"
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strInputText, TEXT_COLOR_WHITE, {}, {} )	--커맨트 콘솔창에 reloadwidget 2 텍스트 출력
	
	-- 커맨드 실행 및 결과 텍스트 추가
	local strResultText = DoCommand( strInputText )
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strResultText, TEXT_COLOR_RED, {}, {} )	--reloadwidget 2 실행
	
end

-- 내부 UI Off 버튼 클릭 이벤트
function Ev_UISwitch_InnerUIOffBtn()
	
	-- 입력 텍스트 추가(입력 텍스트는 커맨트콘솔 창이 켜져있지 않더라도 입력이 가능하다.)
	local strInputText = "dspname 0"
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strInputText, TEXT_COLOR_WHITE, {}, {} )	--커맨트 콘솔창에 reloadwidget 2 텍스트 출력
	
	-- 커맨드 실행 및 결과 텍스트 추가
	local strResultText = DoCommand( strInputText )
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strResultText, TEXT_COLOR_RED, {}, {} )	--reloadwidget 2 실행
	
end
