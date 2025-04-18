
g_nSecondPassword_InputType = -1			-- 2차 비번 입력 타입 ( 1 - 입력, 2 - 신규, 3 - 삭제시 )
g_strSecondPassword_InputFocusInst = ""		-- 입력할 인풋 인스턴스 네임
g_strSecondPassword = ""
-- 2차 비밀번호 오픈
function OpenSecondPassword()

	-- 확인/취소 텍스트
	SetString( WIDGET_ID_SECONDPASSWORD, "label", GetGameWordText( "MODAL_BUTTON", 0 ), "panelInput.btnOK" )
	SetString( WIDGET_ID_SECONDPASSWORD, "label", GetGameWordText( "MODAL_BUTTON", 1 ), "panelInput.btnCancel" )
	
	-- 확인/취소 이벤트
	SetEvent( WIDGET_ID_SECONDPASSWORD, EVENT_BUTTON_CLICK, "Ev_SecondPassword_OK", "", "panelInput.btnOK" )
	SetEvent( WIDGET_ID_SECONDPASSWORD, EVENT_BUTTON_CLICK, "Ev_SecondPassword_Close", "", "panelInput.btnCancel" )
	
	local nSP = GetServiceProvider()
	if ( nSP == SP_TAIWAN ) then
	
		SetBool( WIDGET_ID_SECONDPASSWORD, "visible", false, "bg_big" )
	
		-- 가상 키보드 UI 오픈
		g_OpenWidget_To_VrKb = WIDGET_ID_SECONDPASSWORD
		OpenWidget( WIDGET_ID_VIRTUARKEYBOARD )
	
		VirtuarKeyBoard_SetUI( WIDGET_ID_SECONDPASSWORD, "panelInput.inputPassword", "" )
		
		CallFunction( WIDGET_ID_SECONDPASSWORD, "gotoAndPlay", "INPUT", "panelInput" )
		
		-- 2차 비번 입력하세요.
		SetString( WIDGET_ID_SECONDPASSWORD, "text", 
			GetGameExternalText( "CHARACTERSTAGE_16", 0 ), "panelInput.labelTitle" )
			
		-- 인풋 텍스트 초기화
		SetString( WIDGET_ID_SECONDPASSWORD, "text", "", "panelInput.inputPassword" )
		
		-- 포커스 설정
		g_strSecondPassword_InputFocusInst = "panelInput.inputPassword"
		SetFocus( WIDGET_ID_SECONDPASSWORD, g_strSecondPassword_InputFocusInst )
		
		-- 랜덤 숫자 모두 숨기기
		for i = 0, 11, 1 do
		
			local strBtnInst = "btnNum" .. tostring( i )
			
			SetBool( WIDGET_ID_SECONDPASSWORD, "visible", false, strBtnInst )
		
		end
	
	else
	
		SetBool( WIDGET_ID_SECONDPASSWORD, "visible", true, "bg_big" )
		
		-- 입력
		if 1 == g_nSecondPassword_InputType or
		   3 == g_nSecondPassword_InputType then
		
			CallFunction( WIDGET_ID_SECONDPASSWORD, "gotoAndPlay", "INPUT", "panelInput" )
		
			-- 2차 비번 입력하세요.
			SetString( WIDGET_ID_SECONDPASSWORD, "text", 
				GetGameExternalText( "CHARACTERSTAGE_16", 0 ), "panelInput.labelTitle" )
				
			-- 인풋 텍스트 초기화
			SetString( WIDGET_ID_SECONDPASSWORD, "text", "", "panelInput.inputPassword" )
			
			-- 포커스 설정
			g_strSecondPassword_InputFocusInst = "panelInput.inputPassword"
			SetFocus( WIDGET_ID_SECONDPASSWORD, g_strSecondPassword_InputFocusInst )
		
		
		-- 신규 설정
		else
		
			CallFunction( WIDGET_ID_SECONDPASSWORD, "gotoAndPlay", "NEW", "panelInput" )
		
			-- 2차 비번 설정
			SetString( WIDGET_ID_SECONDPASSWORD, "text",
				GetGameExternalText( "CHARACTERSTAGE_20", 0 ), "panelInput.labelTitle" )
			-- 2차 비번 입력
			SetString( WIDGET_ID_SECONDPASSWORD, "text", 
				GetGameWordText( "PASS_SET_TEXT", 0 ), "panelInput.labelInput" )
			-- 2차 비번 재입력
			SetString( WIDGET_ID_SECONDPASSWORD, "text",
				GetGameWordText( "PASS_SET_TEXT", 1 ), "panelInput.labelReinput" )
				
			-- 인풋 텍스트 초기화
			SetString( WIDGET_ID_SECONDPASSWORD, "text", "", "panelInput.inputInput" )
			SetString( WIDGET_ID_SECONDPASSWORD, "text", "", "panelInput.inputReinput" )
			
			-- 포커스 설정
			g_strSecondPassword_InputFocusInst = "panelInput.inputInput"
			SetFocus( WIDGET_ID_SECONDPASSWORD, g_strSecondPassword_InputFocusInst )
		
		end
		
		-- 0 ~ 9까지 랜덤한 순서로 숫자가 담긴 테이블 생성
		local tbRandNum = SecondPassword_GenerateRandNum()
		
		-- 랜덤 숫자 차례대로 넣어주고 각 숫자에 맞는 이벤트 심기
		for i = 0, 9, 1 do
		
			local strBtnInst = "btnNum" .. tostring( i )
			local strNum = tostring( tbRandNum[ i + 1 ] )
			
			SetString( WIDGET_ID_SECONDPASSWORD, "label", strNum, strBtnInst )
			SetEvent( WIDGET_ID_SECONDPASSWORD, EVENT_BUTTON_CLICK, 
				"Ev_SecondPassword_Num" .. tostring( strNum ), "", strBtnInst )
		
		end
		
		-- 빈칸 이벤트 심기
		SetEvent( WIDGET_ID_SECONDPASSWORD, EVENT_BUTTON_CLICK, 
			"Ev_SecondPassword_Space", "", "btnNum10" )
		
		-- 백스페이스 이벤트 심기
		SetEvent( WIDGET_ID_SECONDPASSWORD, EVENT_BUTTON_CLICK, 
			"Ev_SecondPassword_Backspace", "", "btnNum11" )
	
	end

end

-- 2차 비번 입력 타입 설정 및 오픈
function SecondPassword_Open( nType )

	CharSelect_SetEnable( false )
	
	g_nSecondPassword_InputType = nType
	OpenWidget( WIDGET_ID_SECONDPASSWORD )

end

-- 포커스 설정된 인풋에 숫자 입력
function SecondPassword_InputNumber( nNum )

	-- 텍스트 넣을 인풋이 없으면 끝냄
	if "" == g_strSecondPassword_InputFocusInst then
		return
	end
	
	-- 클릭한 숫자 입력 ( 10일 경우 Backspace 취급한다 )
	local str = GetString( WIDGET_ID_SECONDPASSWORD, "text", g_strSecondPassword_InputFocusInst )
	local nNumCount = string.len( str )
	if ( 10 == nNum ) then
	
		str = string.sub( str, 1, nNumCount-1 )
		
	elseif ( -1 == nNum ) then
		
		str = str
		
	else
	
		-- 네글자 이상이면 끝냄
		if ( 4 <= nNumCount ) then
		
			return
			
		end
	
		str = str .. tostring( nNum )
		
	end
	
	-- 변경된 문자열 세팅
	SetString( WIDGET_ID_SECONDPASSWORD, "text", str, g_strSecondPassword_InputFocusInst )

	-- 네글자 이상이면 타입에 따라 포커스 이동
	if 4 <= string.len( str ) then
	
		-- 신규 설정
		if ( 1 ~= g_nSecondPassword_InputType ) and
		   ( 3 ~= g_nSecondPassword_InputType ) then
		
			-- 2차 비번 입력이면 재입력으로 포커스 이동
			if "panelInput.inputInput" == g_strSecondPassword_InputFocusInst then
			
				g_strSecondPassword_InputFocusInst = "panelInput.inputReinput"
				SetFocus( WIDGET_ID_SECONDPASSWORD, g_strSecondPassword_InputFocusInst )
			
			end
		
		end
		
	else
	
		-- 숫자 버튼 클릭하면 포커스가 뺐긴다. 여기서 다시 설정해주기
		SetFocus( WIDGET_ID_SECONDPASSWORD, g_strSecondPassword_InputFocusInst )
	
	end
	
end

-- 0 ~ 9까지 랜덤한 순서로 숫자가 담긴 테이블 생성
function SecondPassword_GenerateRandNum()

	local tb = {}

	while( 10 ~= table.getn( tb ) ) do
	
		local n = math.random( 0, 9 )
		
		local bInsert = true	
		for value in pairs( tb ) do
		
			if n == tb[ value ] then
				bInsert = false
				break
			end
		
		end
		
		if true == bInsert then
			table.insert( tb, n )
		end
	
	end
	
	return tb

end

------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 2차 비번 결과
function SecondPassword_Result( nError )

	-- 접속 대기 메시지박스 떠있을테니 끄기
	Event_MsgBox_Close()
	
	-- 실패
	if -2 == nError then
		
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_17", 0 ),
			Event_MsgBox_Close, Event_MsgBox_Close )
	
	-- 블락
	elseif -3 == nError then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_18", 0 ),
			Event_MsgBox_Close, Event_MsgBox_Close )
	
	-- 성공
	else
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_19", 0 ),
			Event_MsgBox_Close, Event_MsgBox_Close )
	
	end

end

------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인
function Ev_SecondPassword_OK()

	local strPassword = ""

	-- 입력
	if 1 == g_nSecondPassword_InputType then
	
		strPassword = GetString( WIDGET_ID_SECONDPASSWORD, "text", "panelInput.inputPassword" )
		g_Msg:JoinGame2ndPassword( strPassword )
		
	
	elseif 3 == g_nSecondPassword_InputType then
	
		g_strSecondPassword = GetString( WIDGET_ID_SECONDPASSWORD, "text", "panelInput.inputPassword" )
		Event_CharSelect_btnDelete_DeleteCount()
		
	-- 신규 설정
	else
	
		strPassword = GetString( WIDGET_ID_SECONDPASSWORD, "text", "panelInput.inputInput" )
		local strReInput = GetString( WIDGET_ID_SECONDPASSWORD, "text", "panelInput.inputReinput" )
		
		-- 입력한 비번이 다름
		if strPassword ~= strReInput then
		
			OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_MISMATCH_2ND_PWD", 0 ),
				Ev_SecondPassword_Incorrect, Ev_SecondPassword_Incorrect )
			
			return
		
		end
		
		g_Msg:JoinGame2ndPassword( strPassword )
	
	end	
	
	CloseWidget( WIDGET_ID_SECONDPASSWORD )
	
	local nSP = GetServiceProvider()
	if ( nSP == SP_TAIWAN ) then
	
		CloseWidget( WIDGET_ID_VIRTUARKEYBOARD )
		
	end
	
	CharSelect_SetEnable( true )

end

-- 취소
function Ev_SecondPassword_Close()

	CloseWidget( WIDGET_ID_SECONDPASSWORD )
	
	local nSP = GetServiceProvider()
	if ( nSP == SP_TAIWAN ) then
	
		CloseWidget( WIDGET_ID_VIRTUARKEYBOARD )
		
	end
	
	CharSelect_SetEnable( true )
	
end

-- 입력한 비번 다름. 다시 설정
function Ev_SecondPassword_Incorrect()

	Event_MsgBox_Close()
	
	-- 인풋 텍스트 초기화
	SetString( WIDGET_ID_SECONDPASSWORD, "text", "", "panelInput.inputInput" )
	SetString( WIDGET_ID_SECONDPASSWORD, "text", "", "panelInput.inputReinput" )
	
	-- 포커스 설정
	g_strSecondPassword_InputFocusInst = "panelInput.inputInput"
	SetFocus( WIDGET_ID_SECONDPASSWORD, g_strSecondPassword_InputFocusInst )

end

-- 숫자 클릭
function Ev_SecondPassword_Num0()
	SecondPassword_InputNumber( 0 )
end

function Ev_SecondPassword_Num1()
	SecondPassword_InputNumber( 1 )
end

function Ev_SecondPassword_Num2()
	SecondPassword_InputNumber( 2 )
end

function Ev_SecondPassword_Num3()
	SecondPassword_InputNumber( 3 )
end

function Ev_SecondPassword_Num4()
	SecondPassword_InputNumber( 4 )
end

function Ev_SecondPassword_Num5()
	SecondPassword_InputNumber( 5 )
end

function Ev_SecondPassword_Num6()
	SecondPassword_InputNumber( 6 )
end

function Ev_SecondPassword_Num7()
	SecondPassword_InputNumber( 7 )
end

function Ev_SecondPassword_Num8()
	SecondPassword_InputNumber( 8 )
end

function Ev_SecondPassword_Num9()
	SecondPassword_InputNumber( 9 )
end

function Ev_SecondPassword_Space()
	SecondPassword_InputNumber( -1 )
end

function Ev_SecondPassword_Backspace()
	-- 값을 숫자로 통일하기 위해 10의 경우 Backspace 취급을 한다
	SecondPassword_InputNumber( 10 )
end