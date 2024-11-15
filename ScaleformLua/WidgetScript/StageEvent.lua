
--------------------------------------------------------------------------------
-- StageInputEvent Manager
StageInputEventManager = {}
StageInputEventManager.new = function ( )

	----------------------------------------------------------------------------
	-- String ProgressBar instance
	local instance = {}
	
	
	----------------------------------------------------------------------------
	-- Private Variable
	local m_bCtrl = false
	local m_bAlt = false
	local m_bShift = false
	
	local m_nKeyCode = 0
	
	----------------------------------------------------------------------------
	-- Function List
	instance.IsCtrlDown = function ( )
	
		return m_bCtrl
		
	end
	
	instance.IsAltDown = function ( )
	
		return m_bAlt
		
	end
	
	instance.IsShiftDown = function ( )
	
		return m_bShift
		
	end
	
	instance.GetDownKeyCode = function ( )
	
		return m_nKeyCode
		
	end
	
	instance.EventKeyDown = function ( _nKeyCode )
	
		m_nKeyCode = _nKeyCode
		
		if KEYCODE_CONTROL == _nKeyCode then
		
			m_bCtrl = true
			
		elseif KEYCODE_ALT == _nKeyCode then
		
			m_bAlt = true
			
		elseif KEYCODE_SHIFT == _nKeyCode then
		
			m_bShift = true
			
		end
		
	end
	
	instance.EventKeyUp = function ( _nKeyCode )
	
		m_nKeyCode = 0
		
		if KEYCODE_CONTROL == _nKeyCode then
		
			m_bCtrl = false
			
		elseif KEYCODE_ALT == _nKeyCode then
		
			m_bAlt = false
			
		elseif KEYCODE_SHIFT == _nKeyCode then
		
			m_bShift = false
			
		end
		
	end
	
	
	----------------------------------------------------------------------------
	
	return instance
	
end


--------------------------------------------------------------------------------
g_STAGE_INPUT_EVENT_MANAGER = StageInputEventManager.new()


--------------------------------------------------------------------------------
-- 스테이지 키다운 이벤트
function Stage_KeyDownEvent ( nKeyCode )

	g_STAGE_INPUT_EVENT_MANAGER.EventKeyDown( nKeyCode )
	
	-- Ctrl + 1~5 일 경우 캐릭터 슬롯을 변경시킨다
	if KEYCODE_NUMBER_1 == nKeyCode and true == g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() then
	
		ChangeCharacterSlot_EventProc( 0 )
		
	elseif KEYCODE_NUMBER_2 == nKeyCode and true == g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() then
	
		ChangeCharacterSlot_EventProc( 1 )
		
	elseif KEYCODE_NUMBER_3 == nKeyCode and true == g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() then
	
		ChangeCharacterSlot_EventProc( 2 )
		
	elseif KEYCODE_NUMBER_4 == nKeyCode and true == g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() then
	
		ChangeCharacterSlot_EventProc( 3 )
		
	elseif KEYCODE_NUMBER_5 == nKeyCode and true == g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() then
	
		ChangeCharacterSlot_EventProc( 4 )
		
	end
	
end


--------------------------------------------------------------------------------
-- 스테이지 키업 이벤트
function Stage_KeyEvent( nKeyCode )

	g_STAGE_INPUT_EVENT_MANAGER.EventKeyUp( nKeyCode )

	-- 디버깅 상태일 때만 아래 단축키 처리
	if true == g_bWidgetDebug then
	
		-- 명령창
		if nKeyCode == KEYCODE_INSERT then
		
			if false == IsOpenWidget( WIDGET_ID_COMMAND_CONSOLE ) then
				OpenWidget( WIDGET_ID_COMMAND_CONSOLE )
			else
				SetFocus( WIDGET_ID_COMMAND_CONSOLE, "console.input" )
			end
			
		-- 루아/swf 리로드
		elseif nKeyCode == KEYCODE_F8 then
		
			ReloadWidget()
		
		-- 게임 스테이지 UI 로드
		elseif nKeyCode == KEYCODE_F7 then
		
			ToGameStage()
		
		-- UI 스위치
		elseif nKeyCode == KEYCODE_F6 then
		
			ToggleUI( WIDGET_ID_UISWITCH )
			
		-- 디버깅 씬
		elseif nKeyCode == KEYCODE_F5 then
		
			ToggleDebugScene()
			
		end
	
	end

	-- 등록된 스테이지 키 이벤트 함수 호출
	for value in pairs( g_tbStageKeyUpFunc ) do
	
		g_tbStageKeyUpFunc[ value ]( nKeyCode )
	
	end

end


--------------------------------------------------------------------------------
-- 스테이지 마우스 클릭 이벤트
function Stage_MouseClickEvent( nStageX, nStageY )

	for value in pairs( g_tbStageMouseClickFunc ) do
	
		local func = g_tbStageMouseClickFunc[ value ]
		func( nStageX, nStageY )
	
	end
	
	for value in pairs( g_tbStageMouseClickFuncLater ) do
		InsertStageMouseClick( g_tbStageMouseClickFuncLater[ value ] )
	end
	
	g_tbStageMouseClickFuncLater = {}

end


--------------------------------------------------------------------------------
-- 스테이지 마우스 이동 이벤트
function Stage_MouseMoveEvent( nStageX, nStageY )

	for value in pairs( g_tbStageMouseMoveFunc ) do
	
		local func = g_tbStageMouseMoveFunc[ value ]
		func( nStageX, nStageY )
	
	end

end


--------------------------------------------------------------------------------
-- 스테이지 마우스 업 이벤트
function Stage_MouseUpEvent( nStageX, nStageY )

	for value in pairs( g_tbStageMouseUpFunc ) do
	
		local func = g_tbStageMouseUpFunc[ value ]
		func( nStageX, nStageY )
	
	end

end

-------------------------------------------------------------------------------------------------------

--리로드
function ReloadWidget()

	-- 입력 텍스트 추가(입력 텍스트는 커맨트콘솔 창이 켜져있지 않더라도 입력이 가능하다.)
	local strInputText = "reloadwidget 2"
	-- 커맨트 콘솔창에 reloadwidget 2 텍스트 출력
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strInputText, TEXT_COLOR_WHITE, {}, {} )

	-- 커맨드 실행 및 결과 텍스트 추가
	local strResultText = DoCommand( strInputText )
	-- reloadwidget 2 실행
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strResultText, TEXT_COLOR_RED, {}, {} )
	
end

-- 전체 UI 오픈
function OpenAllWidget()

	for value in pairs( WIDGETS_LIST ) do
	
		local nID = WIDGETS_LIST[ value ][ "wWidgetID" ]
		OpenWidget( nID )
	
	end

end