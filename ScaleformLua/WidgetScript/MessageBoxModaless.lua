
g_tbMsgBoxModalessQueue = {}			-- 메시지 큐 테이블

g_fMsgBoxModalessElspTime = 0			-- 흐른 시간
g_fMsgBoxModalessTimer = 0				-- 타이머
g_funcMsgBoxModalessTimerEvent = nil	-- 타이머 끝나고 호출될 함수

g_nCurrentMsgBoxModalessType = 0		-- 현재 메시지 박스 타입

g_funcMsgBoxModaless_EnterKey = nil		-- 엔터키 이벤트
g_funcMsgBoxModaless_EscKey = nil		-- ESC키 이벤트

g_tbMsgBoxModaless_EnterData = nil
g_tbMsgBoxModaless_ESCData = nil
g_tbMsgBoxModaless_TimerData = nil

-- 메시지 박스 로드 완료 
function LoadMessageBoxModaless()

	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_WIDGET_CLOSE, "CloseMessageBoxModaless", "", "" )

	MsgBoxModaless_Config()

end

-- 메시지 박스 오픈
function OpenMessageBoxModaless()

	MsgBoxModaless_Config()

	-- 메시지 팝
	MsgBoxModaless_PopMsg()

end

function CloseMessageBoxModaless()

	g_funcMsgBoxModaless_EnterKey = nil
	g_funcMsgBoxModaless_EscKey = nil
	g_tbMsgBoxModaless_EnterData = nil
	g_tbMsgBoxModaless_ESCData = nil
	g_tbMsgBoxModaless_TimerData = nil
	
	RemoveFrameMove( MsgBoxModaless_FrameMove )

end

-- 초기화
function MsgBoxModaless_Config()

	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "closeBtn" )
	
	local strOK = GetGameWordText( "MODAL_BUTTON", 0 )
	local strCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "label", strOK, "msgBoxOK.btnOK" )
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "label", strCancel, "msgBoxCancel.btnCancel" )
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "label", strOK, "msgBoxOKCancel.btnOK" )
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "label", strCancel, "msgBoxOKCancel.btnCancel" )
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "label", strOK, "msgBoxInput.btnOK" )
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "label", strCancel, "msgBoxInput.btnCancel" )
	
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_BUTTON_CLICK, "Ev_MsgBoxModaless_OK", "", "msgBoxOK.btnOK" )
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_BUTTON_CLICK, "Ev_MsgBoxModaless_Cancel", "", "msgBoxCancel.btnCancel" )
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_BUTTON_CLICK, "Ev_MsgBoxModaless_OK", "", "msgBoxOKCancel.btnOK" )
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_BUTTON_CLICK, "Ev_MsgBoxModaless_Cancel", "", "msgBoxOKCancel.btnCancel" )
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_BUTTON_CLICK, "Ev_MsgBoxModaless_OK", "", "msgBoxInput.btnOK" )
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_BUTTON_CLICK, "Ev_MsgBoxModaless_Cancel", "", "msgBoxInput.btnCancel" )
	
	-- input 키보드 이벤트
	SetEvent( WIDGET_ID_MESSAGEBOX_MODALESS, EVENT_KEY_UP, "Ev_MsgBoxModaless_InputKeyUp", "keyCode", "msgBoxInput.input" )

end

-- 시간 타이머 돌리기
function MsgBoxModaless_FrameMove( fElspTime )

	g_fMsgBoxModalessElspTime = g_fMsgBoxModalessElspTime + fElspTime
	if 1 > g_fMsgBoxModalessElspTime then
		return
	end
	
	g_fMsgBoxModalessElspTime = 0
	g_fMsgBoxModalessTimer = g_fMsgBoxModalessTimer - 1

	if 0 < g_fMsgBoxModalessTimer then
	
		local strTime = tostring( math.ceil( g_fMsgBoxModalessTimer ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "text", strTime, "MB_progressBar.tfTimer" )
	
	else
	
		g_fMsgBoxModalessTimer = 0
	
		-- 타이머 이벤트가 등록되어 있다면 해당 이벤트 호출
		if nil ~= g_funcMsgBoxModalessTimerEvent then
			g_funcMsgBoxModalessTimerEvent( g_tbMsgBoxModaless_TimerData )
		end
		
		RemoveFrameMove( MsgBoxModaless_FrameMove )
	
	end

end

-- 타입 설정
function MsgBoxModaless_SetType( nType )

	g_nCurrentMsgBoxModalessType = nType
	
	if MSG_BOX_TYPE_OK == g_nCurrentMsgBoxModalessType then
	
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", true, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_CANCEL == g_nCurrentMsgBoxModalessType then
	
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", true, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_OKCANCEL == g_nCurrentMsgBoxModalessType then
	
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", true, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_INPUT == g_nCurrentMsgBoxModalessType then
	
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", true, "msgBoxInput" )
	
	else
	
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "msgBoxInput" )
	
	end

end

-- 메시지 박스 리셋
function MsgBoxModaless_Reset()

	-- 메시지 큐 비워주기
	g_tbMsgBoxModalessQueue = {}

end

-- 메시지 박스 초기화
function MsgBoxModaless_Init( nType, strText, funcEnter, funcESC, tbEnterData, tbESCData, fTime, funcTimerEvent, tbTimerData )

	MsgBoxModaless_SetType( nType )
	
	g_funcMsgBoxModaless_EnterKey = funcEnter
	g_funcMsgBoxModaless_EscKey = funcESC
	g_funcMsgBoxModalessTimerEvent = funcTimerEvent
	g_tbMsgBoxModaless_EnterData = tbEnterData
	g_tbMsgBoxModaless_ESCData = tbESCData
	g_tbMsgBoxModaless_TimerData = tbTimerData

	-- 타이머 설정
	MsgBoxModaless_SetTimer( fTime )
	
	-- 메시지 텍스트 설정
	SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "htmlText", strText, "textField" )
	
	if nType == MSG_BOX_TYPE_INPUT then
	
		SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "text", "", "msgBoxInput.input" )
		SetFocus( WIDGET_ID_MESSAGEBOX_MODALESS, "msgBoxInput.input" )
		
	else
	
		SetFocus( WIDGET_ID_MESSAGEBOX_MODALESS, "" )
		
	end

end

-- 메시지 팝
function MsgBoxModaless_PopMsg()

	-- 메시지 큐가 비어있으면 끄기
	if 0 == table.getn( g_tbMsgBoxModalessQueue ) then
	
		CloseWidget( WIDGET_ID_MESSAGEBOX_MODALESS )
		
		g_fMsgBoxModalessTimer = 0
		g_funcMsgBoxModalessTimerEvent = nil
		g_funcMsgBoxModaless_EnterKey = nil
		g_funcMsgBoxModaless_EscKey = nil
		g_tbMsgBoxModaless_EnterData = nil
		g_tbMsgBoxModaless_ESCData = nil
		
		RemoveFrameMove( MsgBoxModaless_FrameMove )
		
	-- 큐에서 꺼내와 메시지 박스 초기화
	else
	
		MsgBoxModaless_Init( g_tbMsgBoxModalessQueue[ 1 ].nMsgType, g_tbMsgBoxModalessQueue[ 1 ].strMsgText, 
			g_tbMsgBoxModalessQueue[ 1 ].funcMsgEnter, g_tbMsgBoxModalessQueue[ 1 ].funcMsgESC,
			g_tbMsgBoxModalessQueue[ 1 ].tbMsgEnterData, g_tbMsgBoxModalessQueue[ 1 ].tbMsgESCData,
			g_tbMsgBoxModalessQueue[ 1 ].fMsgTime, g_tbMsgBoxModalessQueue[ 1 ].funcMsgTimerEvent, g_tbMsgBoxModalessQueue[ 1 ].tbMsgTimerData )
			
		-- 큐 지우기
		table.remove( g_tbMsgBoxModalessQueue, 1 )
	
	end

end

-- 타이머 설정
function MsgBoxModaless_SetTimer( fTime )

	if nil == fTime or "number" ~= type( fTime ) then
	
		g_fMsgBoxModalessTimer = 0
		RemoveFrameMove( MsgBoxModaless_FrameMove )
		
	else
	
		g_fMsgBoxModalessTimer = fTime
		
	end
	
	-- 타이머 돌리기
	if 0 < g_fMsgBoxModalessTimer then
	
		-- 프로그래스 바 보이기
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", true, "MB_progressBar" )
		
		SetInt( WIDGET_ID_MESSAGEBOX_MODALESS, "value", 100, "MB_progressBar.progressbar" )
		
		local tbQuickSet = 
		{
			time = g_fMsgBoxModalessTimer * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_MESSAGEBOX_MODALESS, tbQuickSet, "value", "0", "MB_progressBar.progressbar" )
		
		-- 타이머 텍스트 설정
		local strTime = tostring( math.ceil( g_fMsgBoxModalessTimer ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "text", strTime, "MB_progressBar.tfTimer" )
		
		-- 타이머 설정
		InsertFrameMove( MsgBoxModaless_FrameMove )
		g_fMsgBoxModalessElspTime = 0
		
	-- 타이머 없음
	else
	
		-- 프로그래스 숨기기
		SetBool( WIDGET_ID_MESSAGEBOX_MODALESS, "visible", false, "MB_progressBar" )
		
	end
	
end

---------------------------------------------------------------------------------------------------------------
-- 유틸리티

-- 메시지 박스 오픈
function OpenMsgBoxModaless( nType, strText, funcEnter, funcESC, tbEnterData, tbESCData, fTime, funcTimerEvent, tbTimerData )

	local tbMsg = 
	{
		nMsgType = nType,
		strMsgText = strText,
		
		funcMsgEnter = funcEnter,
		funcMsgESC = funcESC,
		
		tbMsgEnterData = tbEnterData,
		tbMsgESCData = tbESCData,
		
		fMsgTime = fTime,
		funcMsgTimerEvent = funcTimerEvent,
		tbMsgTimerData = tbTimerData,
	}
	table.insert( g_tbMsgBoxModalessQueue, tbMsg )
	
	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX_MODALESS ) then
	
		OpenWidget( WIDGET_ID_MESSAGEBOX_MODALESS )
	
	end

end

-- 유저 입력 텍스트 얻기
function GetMsgBoxModalessInput()

	-- 메시지 박스 타입이 input 이 아니면 빈문자열 반환
	if g_nCurrentMsgBoxModalessType ~= MSG_BOX_TYPE_INPUT then
		return ""
	end
	
	return GetString( WIDGET_ID_MESSAGEBOX_MODALESS, "text", "msgBoxInput.input" )

end

-- 메시지 박스 열려있는지 확인
function IsOpenMsgBoxModaless()

	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX_MODALESS ) then
		
		return false
		
	else
	
		return true
		
	end

end

---------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 메시지 박스 엔터키 단축키
function MsgBoxModaless_HotKey_Enter()

	if false == IsLoadWidget( WIDGET_ID_MESSAGEBOX_MODALESS ) then
		return false
	end
	
	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX_MODALESS ) then
	
		return false
		
	else
	
		-- 엔터키 단축키 이벤트 호출
		if nil ~= g_funcMsgBoxModaless_EnterKey and "" ~= g_funcMsgBoxModaless_EnterKey then
			g_funcMsgBoxModaless_EnterKey( g_tbMsgBoxModaless_EnterData )
		end
		
		return true
	
	end

end

-- 메시지 박스 ESC키 단축키
function MsgBoxModaless_HotKey_ESC()

	if false == IsLoadWidget( WIDGET_ID_MESSAGEBOX_MODALESS ) then
		return false
	end

	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX_MODALESS ) then
	
		return false
		
	else
	
		-- ESC키 단축키 이벤트 호출
		if nil ~= g_funcMsgBoxModaless_EscKey and "" ~= g_funcMsgBoxModaless_EscKey  then
			g_funcMsgBoxModaless_EscKey( g_tbMsgBoxModaless_ESCData )
		end
		
		return true
		
	end

end


---------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인
function Ev_MsgBoxModaless_OK()

	if nil ~= g_funcMsgBoxModaless_EnterKey and "" ~= g_funcMsgBoxModaless_EnterKey then
	
		g_funcMsgBoxModaless_EnterKey( g_tbMsgBoxModaless_EnterData )
		
	end

end

-- 취소
function Ev_MsgBoxModaless_Cancel()

	if nil ~= g_funcMsgBoxModaless_EscKey and "" ~= g_funcMsgBoxModaless_EscKey  then
	
		g_funcMsgBoxModaless_EscKey( g_tbMsgBoxModaless_ESCData )
		
	end

end

-- 키보드 이벤트 (단축키 체크는 캐릭터 접속까지 끝나야한다. 게임 외부에서 처리를 위해 이벤트로 처리해주자.)
function Ev_MsgBoxModaless_KeyUp( nKeyCode )

	if KEYCODE_ESCAPE == nKeyCode then
	
		MsgBoxModaless_HotKey_ESC()	
	
	elseif KEYCODE_ENTER == nKeyCode then
	
		MsgBoxModaless_HotKey_Enter()
	
	end

end

-- Input 키보드 이벤트
function Ev_MsgBoxModaless_InputKeyUp( nKeyCode )

	if MSG_BOX_TYPE_INPUT ~= g_nCurrentMsgBoxModalessType then
		return
	end
	
	if KEYCODE_ESCAPE == nKeyCode then
	
		MsgBoxModaless_HotKey_ESC()	
	
	elseif KEYCODE_ENTER == nKeyCode then
	
		MsgBoxModaless_HotKey_Enter()
	
	end

end

-- 닫기 이벤트
function Event_MsgBoxModaless_Close()

	if g_nCurrentMsgBoxModalessType == MSG_BOX_TYPE_INPUT then
	
		SetString( WIDGET_ID_MESSAGEBOX_MODALESS, "Restrict", "all", "msgBoxInput.input" )
		SetInt( WIDGET_ID_MESSAGEBOX_MODALESS, "maxChars", 0, "msgBoxInput.input" )
		SetInt( WIDGET_ID_MESSAGEBOX_MODALESS, "maxByte", 0, "msgBoxInput.input" )
		
	end

	MsgBoxModaless_PopMsg()
	
end