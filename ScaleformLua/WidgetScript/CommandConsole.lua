
-- 입력 히스토리
COMMAND_INPUT_HISTORY = {}

-- 입력될 현재 히스토리 인덱스
COMMAND_INPUT_CUR_INDEX = 0
-- 히스토리 최대 갯수
COMMAND_INPUT_MAX_COUNT = 10

-- 로드 완료
function LoadCommandConsole() 

	-- console ------------------------------------------------------------------------------
	-- 입력 이벤트
	SetEvent( WIDGET_ID_COMMAND_CONSOLE, EVENT_KEY_UP, "Command_KeyUpEvent", "keyCode;", "console" )	
 
	-- console.log --------------------------------------------------------------------------	
	-- logID 설정
	SetInt( WIDGET_ID_COMMAND_CONSOLE, "logID", CHAT_LOG_COMMAND_CONSOLE, "console.log" )
	
 	-- console.input --------------------------------------------------------------------------	
	-- 입력 이벤트 
	SetEvent( WIDGET_ID_COMMAND_CONSOLE, EVENT_KEY_UP, "Command_Input_KeyUpEvent", "keyCode;", "console.input" )
	-- 포커스 이벤트
	SetEvent( WIDGET_ID_COMMAND_CONSOLE, EVENT_FOCUS_OUT, "Command_Input_FocusOutEvent", "", "console.input" )
	SetEvent( WIDGET_ID_COMMAND_CONSOLE, EVENT_FOCUS_IN, "TransInputEn", "", "console.input" )

	--------------------------------------------------------------------------------------------
	-- 저장된 채팅 메시지 출력
	AddStoredChat( CHAT_LOG_COMMAND_CONSOLE )

end

-- 오픈
function OpenCommandConsole()

	CallFunction( WIDGET_ID_COMMAND_CONSOLE, "gotoAndPlay", "1", "" )

	-- 화면 가로 사이즈
	local nScreenWidth = GetScreenWidth()
	
	-- 가로 사이즈 설정
	SetInt( WIDGET_ID_COMMAND_CONSOLE, "width", nScreenWidth, "console.log" )
	SetInt( WIDGET_ID_COMMAND_CONSOLE, "width", nScreenWidth, "console.input" )
	
	-- 포커스 설정
	SetFocus( WIDGET_ID_COMMAND_CONSOLE, "console.input" )

end

-----------------------------------------------------------------------------------------------------------------
-- Method
 
 -- 사이즈 조정
function ResetSize_CommandConsole( nScreenWidth )
	
	-- UI가 닫혀 있으면 종료
	if  false == IsOpenWidget( WIDGET_ID_COMMAND_CONSOLE ) then
		return
	end
	
	-- 화면 사이즈에 맞춰 크기 재수정
	SetInt( WIDGET_ID_COMMAND_CONSOLE, "width", nScreenWidth, "console.log" )
	SetInt( WIDGET_ID_COMMAND_CONSOLE, "width", nScreenWidth, "console.input" )
	
end

-- 히스토리에 저장된 현재 인덱스 출력
function PrintHistory_CommandConsole_Input( nMoveIndex )

	-- 인덱스 이동
	COMMAND_INPUT_CUR_INDEX = COMMAND_INPUT_CUR_INDEX + nMoveIndex
	
	if  COMMAND_INPUT_CUR_INDEX < 1 then
	
		COMMAND_INPUT_CUR_INDEX = 1
		
	elseif  COMMAND_INPUT_CUR_INDEX > table.getn( COMMAND_INPUT_HISTORY ) then
	
		COMMAND_INPUT_CUR_INDEX = table.getn( COMMAND_INPUT_HISTORY )
		
	end

	SetString( WIDGET_ID_COMMAND_CONSOLE, "text", COMMAND_INPUT_HISTORY[ COMMAND_INPUT_CUR_INDEX ], "console.input" )

	--텍스트 선택
	local params = "0;" .. string.len( COMMAND_INPUT_HISTORY[ COMMAND_INPUT_CUR_INDEX ] )
	CallFunction( WIDGET_ID_COMMAND_CONSOLE, "SetSelection", params, "console.input" )

end

-- 입력 히스토리 인덱스 초기화
function ResetIndex_CommandConsole_Input()

	COMMAND_INPUT_CUR_INDEX = table.getn( COMMAND_INPUT_HISTORY ) + 1

end
 
-----------------------------------------------------------------------------------------------------------------
-- Event Handler
 
-- 입력 이벤트
function Command_KeyUpEvent( nKeyCode )
	
	-- 창 닫기
	if  nKeyCode == KEYCODE_ESCAPE then
	
		-- 포커스 설정
		SetFocus( -1, "" )
	
		CloseWidget( WIDGET_ID_COMMAND_CONSOLE )
		
		-- 입력 인덱스 초기화
		ResetIndex_CommandConsole_Input()
		
	end
	
end
 
-- console.input 입력 이벤트
function Command_Input_KeyUpEvent( nKeyCode )

	-- Enter 키 명령 날려줌
	if  nKeyCode == KEYCODE_ENTER then
	
		-- 입력 텍스트 추가
		local strInputText = GetString( WIDGET_ID_COMMAND_CONSOLE, "text", "console.input" )
		AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strInputText, TEXT_COLOR_WHITE, {}, {} )
		
		-- input 비우기
		SetString( WIDGET_ID_COMMAND_CONSOLE, "text", "", "console.input" )
		
		-- 커맨드 실행 및 결과 텍스트 추가
		local strResultText = DoCommand( strInputText )
		AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strResultText, TEXT_COLOR_RED, {}, {} )

		-- 빈 문자열이 아니면 히스토리에 추가
		if  strInputText ~= "" then
			-- 히스토리에 추가
			table.insert( COMMAND_INPUT_HISTORY, strInputText )
			
			-- 히스토리 최대 갯수 넘으면 1번 인덱스 삭제
			if  COMMAND_INPUT_MAX_COUNT < table.getn( COMMAND_INPUT_HISTORY ) then
				table.remove( COMMAND_INPUT_HISTORY, 1 )
			end
			
			-- 입력 인덱스 초기화
			ResetIndex_CommandConsole_Input()
		end
		
	end
	
	-- 저장된 히스토리가 있다면
	if  0 < table.getn( COMMAND_INPUT_HISTORY ) then
	
		-- Up키 이전 입력 히스토리
		if  nKeyCode == KEYCODE_PAGE_UP then
			
			-- 인덱스 이동, 텍스트 출력
			PrintHistory_CommandConsole_Input( -1 )
			
		-- Down키 다음 입력 히스토리
		elseif  nKeyCode == KEYCODE_PAGE_DOWN then
			
			-- 인덱스 이동, 텍스트 출력
			PrintHistory_CommandConsole_Input( 1 )
			
		end
		
	end
	
end

-- console.input 포커스 이벤트
function Command_Input_FocusOutEvent()

	-- 포커스 나가면 입력 인덱스 초기화
	ResetIndex_CommandConsole_Input()

end