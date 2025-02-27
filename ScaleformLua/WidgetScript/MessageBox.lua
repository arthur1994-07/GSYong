
-- 메시지 박스 타입
MSG_BOX_TYPE_OK 			= 1		-- OK 버튼만 있음
MSG_BOX_TYPE_CANCEL 		= 2		-- Cancel 버튼만 있음
MSG_BOX_TYPE_OKCANCEL 		= 3		-- OK, Cancel 버튼
MSG_BOX_TYPE_INPUT 			= 4		-- Input, OK, Cancel
MSG_BOX_TYPE_INPUT_MONEY	= 5		-- 돈입력
MSG_BOX_TYPE_INFO 			= 6		-- 텍스트만 있음.
MSG_BOX_TYPE_INPUT_ONLYNUM	= 7		-- 숫자만 입력가능(개수입력시)

g_tbMsgBoxQueue = {}			-- 메시지 큐 테이블

g_fMsgBoxElspTime = 0			-- 흐른 시간
g_fMsgBoxTimer = 0				-- 타이머
g_funcMsgBoxTimerEvent = nil	-- 타이머 끝나고 호출될 함수

g_nCurrentMsgBoxType = 0		-- 현재 메시지 박스 타입

g_funcMsgBox_EnterKey = nil		-- 엔터키 이벤트
g_funcMsgBox_EscKey = nil		-- ESC키 이벤트

g_tbMsgBox_EnterData = nil
g_tbMsgBox_ESCData = nil
g_tbMsgBox_TimerData = nil

-- 메시지 박스 로드 완료 
function LoadMessageBox()

	MsgBox_Config()

end

-- 메시지 박스 오픈
function OpenMessageBox()

	MsgBox_Config()

	-- 메시지 팝
	MsgBox_PopMsg()

end

-- 초기화
function MsgBox_Config()

	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "closeBtn" )
	
	local strOK = GetGameWordText( "MODAL_BUTTON", 0 )
	local strCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	
	SetString( WIDGET_ID_MESSAGEBOX, "label", strOK, "msgBoxOK.btnOK" )
	SetString( WIDGET_ID_MESSAGEBOX, "label", strCancel, "msgBoxCancel.btnCancel" )
	SetString( WIDGET_ID_MESSAGEBOX, "label", strOK, "msgBoxOKCancel.btnOK" )
	SetString( WIDGET_ID_MESSAGEBOX, "label", strCancel, "msgBoxOKCancel.btnCancel" )
	SetString( WIDGET_ID_MESSAGEBOX, "label", strOK, "msgBoxInput.btnOK" )
	SetString( WIDGET_ID_MESSAGEBOX, "label", strCancel, "msgBoxInput.btnCancel" )
	
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_BUTTON_CLICK, "Ev_MsgBox_OK", "", "msgBoxOK.btnOK" )
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_BUTTON_CLICK, "Ev_MsgBox_Cancel", "", "msgBoxCancel.btnCancel" )
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_BUTTON_CLICK, "Ev_MsgBox_OK", "", "msgBoxOKCancel.btnOK" )
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_BUTTON_CLICK, "Ev_MsgBox_Cancel", "", "msgBoxOKCancel.btnCancel" )
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_BUTTON_CLICK, "Ev_MsgBox_OK", "", "msgBoxInput.btnOK" )
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_BUTTON_CLICK, "Ev_MsgBox_Cancel", "", "msgBoxInput.btnCancel" )
	
	-- input 키보드 이벤트
	SetEvent( WIDGET_ID_MESSAGEBOX, EVENT_KEY_UP, "Ev_MsgBox_InputKeyUp", "keyCode", "msgBoxInput.input" )

end

-- 시간 타이머 돌리기
function MsgBox_FrameMove( fElspTime )

	g_fMsgBoxElspTime = g_fMsgBoxElspTime + fElspTime
	if 1 > g_fMsgBoxElspTime then
		return
	end
	
	g_fMsgBoxElspTime = 0
	g_fMsgBoxTimer = g_fMsgBoxTimer - 1

	if 0 < g_fMsgBoxTimer then
	
		local strTime = tostring( math.ceil( g_fMsgBoxTimer ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_MESSAGEBOX, "text", strTime, "MB_progressBar.tfTimer" )
	
	else
	
		g_fMsgBoxTimer = 0
	
		-- 타이머 이벤트가 등록되어 있다면 해당 이벤트 호출
		if nil ~= g_funcMsgBoxTimerEvent then
			g_funcMsgBoxTimerEvent( g_tbMsgBox_TimerData )
		end
		
		RemoveFrameMove( MsgBox_FrameMove )
	
	end

end

-- 타입 설정
function MsgBox_SetType( nType )

	g_nCurrentMsgBoxType = nType
	
	if MSG_BOX_TYPE_OK == g_nCurrentMsgBoxType then
	
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", true, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_CANCEL == g_nCurrentMsgBoxType then
	
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", true, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_OKCANCEL == g_nCurrentMsgBoxType then
	
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", true, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_INPUT == g_nCurrentMsgBoxType 
		or MSG_BOX_TYPE_INPUT_MONEY == g_nCurrentMsgBoxType 
		or MSG_BOX_TYPE_INPUT_ONLYNUM == g_nCurrentMsgBoxType then
	
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", true, "msgBoxInput" )
	
	else
	
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "msgBoxInput" )
	
	end
	
	if MSG_BOX_TYPE_INPUT_ONLYNUM == g_nCurrentMsgBoxType then
		SetBool( WIDGET_ID_MESSAGEBOX, "currencyUnit", false, "msgBoxInput.input" )
		SetString( WIDGET_ID_MESSAGEBOX, "Restrict", "number", "msgBoxInput.input" )
	
	elseif MSG_BOX_TYPE_INPUT_MONEY == g_nCurrentMsgBoxType then
		SetBool( WIDGET_ID_MESSAGEBOX, "currencyUnit", true, "msgBoxInput.input" )
		SetString( WIDGET_ID_MESSAGEBOX, "Restrict", "number", "msgBoxInput.input" )
		
	else
		SetBool( WIDGET_ID_MESSAGEBOX, "currencyUnit", false, "msgBoxInput.input" )
		SetString( WIDGET_ID_MESSAGEBOX, "Restrict", "all", "msgBoxInput.input" )
		
	end

end

-- 메시지 박스 리셋
function MsgBox_Reset()

	-- 메시지 큐 비워주기
	g_tbMsgBoxQueue = {}

end

-- 메시지 박스 초기화
function MsgBox_Init( nType, strText, funcEnter, funcESC, tbEnterData, tbESCData, fTime, funcTimerEvent, tbTimerData )

	MsgBox_SetType( nType )
	
	g_funcMsgBox_EnterKey = funcEnter
	g_funcMsgBox_EscKey = funcESC
	g_funcMsgBoxTimerEvent = funcTimerEvent
	g_tbMsgBox_EnterData = tbEnterData
	g_tbMsgBox_ESCData = tbESCData
	g_tbMsgBox_TimerData = tbTimerData

	-- 타이머 설정
	MsgBox_SetTimer( fTime )
	
	-- 메시지 텍스트 설정
	SetString( WIDGET_ID_MESSAGEBOX, "htmlText", strText, "textField" )
	if nType == MSG_BOX_TYPE_INPUT 
		or nType == MSG_BOX_TYPE_INPUT_MONEY 
		or nType == MSG_BOX_TYPE_INPUT_ONLYNUM then
		SetString( WIDGET_ID_MESSAGEBOX, "text", "", "msgBoxInput.input" )
		-- ESC로 끄고 다시 켜질 때 아래 포커스 명령하나로만으로는 정상동작되지않음
		-- ESC로 메세지박스를 닫을 경우 'UI 꺼짐 이벤트' 후 포커스 UI '포커스 잃음 이벤트'가 발생하는거 같은데
		-- 포커스 잃는 이벤트가 예약만되고 다음번 UI Open시 적용되는거같다
		SetFocus( WIDGET_ID_MESSAGEBOX, "" )
		SetFocus( WIDGET_ID_MESSAGEBOX, "msgBoxInput.input" )

	else
		SetFocus( WIDGET_ID_MESSAGEBOX, "" )

	end

end

-- 메시지 팝
function MsgBox_PopMsg()

	-- 메시지 큐가 비어있으면 끄기
	if 0 == table.getn( g_tbMsgBoxQueue ) then
	
		CloseWidget( WIDGET_ID_MESSAGEBOX )
		
		g_fMsgBoxTimer = 0
		g_funcMsgBoxTimerEvent = nil
		g_funcMsgBox_EnterKey = nil
		g_funcMsgBox_EscKey = nil
		g_tbMsgBox_EnterData = nil
		g_tbMsgBox_ESCData = nil
		
		RemoveFrameMove( MsgBox_FrameMove )
		
	-- 큐에서 꺼내와 메시지 박스 초기화
	else
	
		MsgBox_Init( g_tbMsgBoxQueue[ 1 ].nMsgType, g_tbMsgBoxQueue[ 1 ].strMsgText, 
			g_tbMsgBoxQueue[ 1 ].funcMsgEnter, g_tbMsgBoxQueue[ 1 ].funcMsgESC,
			g_tbMsgBoxQueue[ 1 ].tbMsgEnterData, g_tbMsgBoxQueue[ 1 ].tbMsgESCData,
			g_tbMsgBoxQueue[ 1 ].fMsgTime, g_tbMsgBoxQueue[ 1 ].funcMsgTimerEvent, g_tbMsgBoxQueue[ 1 ].tbMsgTimerData )
			
		-- 큐 지우기
		table.remove( g_tbMsgBoxQueue, 1 )
	
	end

end

-- 타이머 설정
function MsgBox_SetTimer( fTime )

	if nil == fTime or "number" ~= type( fTime ) then
	
		g_fMsgBoxTimer = 0
		RemoveFrameMove( MsgBox_FrameMove )
		
	else
	
		g_fMsgBoxTimer = fTime
		
	end
	
	-- 타이머 돌리기
	if 0 < g_fMsgBoxTimer then
	
		-- 프로그래스 바 보이기
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", true, "MB_progressBar" )
		
		SetInt( WIDGET_ID_MESSAGEBOX, "value", 100, "MB_progressBar.progressbar" )
		
		local tbQuickSet = 
		{
			time = g_fMsgBoxTimer * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_MESSAGEBOX, tbQuickSet, "value", "0", "MB_progressBar.progressbar" )
		
		-- 타이머 텍스트 설정
		local strTime = tostring( math.ceil( g_fMsgBoxTimer ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_MESSAGEBOX, "text", strTime, "MB_progressBar.tfTimer" )
		
		-- 타이머 설정
		InsertFrameMove( MsgBox_FrameMove )
		g_fMsgBoxElspTime = 0
		
	-- 타이머 없음
	else
	
		-- 프로그래스 숨기기
		SetBool( WIDGET_ID_MESSAGEBOX, "visible", false, "MB_progressBar" )
		
	end
	
end

---------------------------------------------------------------------------------------------------------------
-- 유틸리티

-- 메시지 박스 오픈
function OpenMsgBox( nType, strText, funcEnter, funcESC, tbEnterData, tbESCData, fTime, funcTimerEvent, tbTimerData )

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
	table.insert( g_tbMsgBoxQueue, tbMsg )
	
	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX ) then
	
		OpenWidget( WIDGET_ID_MESSAGEBOX )
	
	end

end

-- 유저 입력 텍스트 얻기
function GetMsgBoxInput()

	-- 메시지 박스 타입이 input 이 아니면 빈문자열 반환
	if g_nCurrentMsgBoxType ~= MSG_BOX_TYPE_INPUT 
		and g_nCurrentMsgBoxType ~= MSG_BOX_TYPE_INPUT_MONEY 
		and g_nCurrentMsgBoxType ~= MSG_BOX_TYPE_INPUT_ONLYNUM then
		return ""
	end
	return GetString( WIDGET_ID_MESSAGEBOX, "text", "msgBoxInput.input" )

end

-- 메시지 박스 열려있는지 확인
function IsOpenMsgBox()

	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX ) then
		
		return false
		
	else
	
		return true
		
	end

end

---------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 메시지 박스 엔터키 단축키
function MsgBox_HotKey_Enter()

	if false == IsLoadWidget( WIDGET_ID_MESSAGEBOX ) then
		return false
	end
	
	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX ) then
	
		return false
		
	else
	
		-- 엔터키 단축키 이벤트 호출
		if nil ~= g_funcMsgBox_EnterKey and "" ~= g_funcMsgBox_EnterKey then
			g_funcMsgBox_EnterKey( g_tbMsgBox_EnterData )
		end
		
		return true
	
	end

end

-- 메시지 박스 ESC키 단축키
function MsgBox_HotKey_ESC()

	if false == IsLoadWidget( WIDGET_ID_MESSAGEBOX ) then
		return false
	end

	if false == IsOpenWidget( WIDGET_ID_MESSAGEBOX ) then
	
		return false
		
	else
	
		-- ESC키 단축키 이벤트 호출
		if nil ~= g_funcMsgBox_EscKey and "" ~= g_funcMsgBox_EscKey  then
			g_funcMsgBox_EscKey( g_tbMsgBox_ESCData )
		end
		
		return true
		
	end

end


---------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인
function Ev_MsgBox_OK()

	if nil ~= g_funcMsgBox_EnterKey and "" ~= g_funcMsgBox_EnterKey then
	
		g_funcMsgBox_EnterKey( g_tbMsgBox_EnterData )
		
	end

end

-- 취소
function Ev_MsgBox_Cancel()

	if nil ~= g_funcMsgBox_EscKey and "" ~= g_funcMsgBox_EscKey  then
	
		g_funcMsgBox_EscKey( g_tbMsgBox_ESCData )
		
	end

end

-- 키보드 이벤트 (단축키 체크는 캐릭터 접속까지 끝나야한다. 게임 외부에서 처리를 위해 이벤트로 처리해주자.)
function Ev_MsgBox_KeyUp( nKeyCode )

	if KEYCODE_ESCAPE == nKeyCode then
	
		MsgBox_HotKey_ESC()	
	
	elseif KEYCODE_ENTER == nKeyCode then
	
		MsgBox_HotKey_Enter()
	
	end

end

-- Input 키보드 이벤트
function Ev_MsgBox_InputKeyUp( nKeyCode )

	if MSG_BOX_TYPE_INPUT ~= g_nCurrentMsgBoxType 
		and MSG_BOX_TYPE_INPUT_MONEY ~= g_nCurrentMsgBoxType 
		and MSG_BOX_TYPE_INPUT_ONLYNUM ~= g_nCurrentMsgBoxType then
		return
	end
	
	if KEYCODE_ESCAPE == nKeyCode then
	
		MsgBox_HotKey_ESC()	
	
	elseif KEYCODE_ENTER == nKeyCode then
	
		MsgBox_HotKey_Enter()
	
	end

end

-- 닫기 이벤트
function Event_MsgBox_Close()

	if g_nCurrentMsgBoxType == MSG_BOX_TYPE_INPUT 
		and MSG_BOX_TYPE_INPUT_MONEY == g_nCurrentMsgBoxType 
		and MSG_BOX_TYPE_INPUT_ONLYNUM == g_nCurrentMsgBoxType then
	
		SetString( WIDGET_ID_MESSAGEBOX, "Restrict", "all", "msgBoxInput.input" )
		SetInt( WIDGET_ID_MESSAGEBOX, "maxChars", 0, "msgBoxInput.input" )
		SetInt( WIDGET_ID_MESSAGEBOX, "maxByte", 0, "msgBoxInput.input" )
		
	end

	MsgBox_PopMsg()
	
end

-- 게임 종료
function Event_MsgBox_CloseGame()
	CloseGame()
end

-- 로그인 스테이지로 이동
function Event_MsgBox_LoginStage()
	ToLoginStage()
end

-- 서버 선택 스테이지로 이동
function Event_MsgBox_ServerSelectStage()
	ToServerSelectStage()
end
