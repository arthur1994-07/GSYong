
-- 보안코드 번호
local g_nRandomPassNum = 0

-- 로그인 UI 로드 완료
function LoadLogin()

	SetEvent( WIDGET_ID_LOGIN, EVENT_WIDGET_CLOSE, "CloseLogin", "", "" )

	-- 드래그 끄기
	SetBool( WIDGET_ID_LOGIN, "customDrag", true, "" )

	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_LOGIN, "visible", false, "closeBtn" )
	
	-- 보안코드 숨기기
	SetBool( WIDGET_ID_LOGIN, "visible", false, "labelPassNumber" )
	SetBool( WIDGET_ID_LOGIN, "visible", false, "inputPassNumber" )

	-- ID 텍스트
	local strID = GetGameWordText( "LOGIN_PAGE_IDPW", 0 )
	-- 패스워드 텍스트
	local strPassword = GetGameWordText( "LOGIN_PAGE_IDPW", 1 )
	-- ID 저장 텍스트
	local strSaveID = GetGameWordText( "LOGIN_PAGE_IDSAVE_BACK", 0 )
	-- 확인 텍스트
	local strOK = GetGameWordText( "GAMBLE_OKCANCEL", 0 )
	-- 취소 텍스트
	local strCancel = GetGameWordText( "GAMBLE_OKCANCEL", 1 )
	
	-- ID 레이블 텍스트 설정
	SetString( WIDGET_ID_LOGIN, "text", strID, "labelID" )
	-- 패스워드 레이블 텍스트 설정
	SetString( WIDGET_ID_LOGIN, "text", strPassword, "labelPassword" )
	-- ID 저장 체크박스 텍스트 설정
	SetString( WIDGET_ID_LOGIN, "text", strSaveID, "checkSaveID_text" )
	-- 확인 버튼 텍스트 설정
	SetString( WIDGET_ID_LOGIN, "label", strOK, "btnOK" )
	-- 취소 버튼 텍스트 설정
	SetString( WIDGET_ID_LOGIN, "label", strCancel, "btnCancel" )
	
	-- ID 인풋 키 입력 이벤트 설정
	SetEvent( WIDGET_ID_LOGIN, EVENT_KEY_DOWN, "Event_Login_inputID_KeyDown", "keyCode;", "inputID" )
	-- 패스워드 인풋 키 입력 이벤트 설정
	SetEvent( WIDGET_ID_LOGIN, EVENT_KEY_DOWN, "Event_Login_inputPassword_KeyDown", "keyCode;", "inputPassword" )
	-- 보안코드 인풋 키 입력 이벤트 설정
	SetEvent( WIDGET_ID_LOGIN, EVENT_KEY_DOWN, "Event_Login_inputPassNumber_KeyDown", "keyCode;", "inputPassNumber" )
	
	-- 아아디 저장 체크박스 클릭 이벤트 설정
	SetEvent( WIDGET_ID_LOGIN, EVENT_BUTTON_CLICK, "Event_Login_checkSaveID_Click", "", "checkSaveID" )
	-- 확인 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_LOGIN, EVENT_BUTTON_CLICK, "Event_Login_btnOK_Click", "", "btnOK" )
	-- 취소 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_LOGIN, EVENT_BUTTON_CLICK, "Event_Login_btnCancel_Click", "", "btnCancel" )
	
	-- 가상 키보드 데이터 셋팅을 위한 문
	SetEvent( WIDGET_ID_LOGIN, EVENT_FOCUS_IN, "Event_Login_inputID_FocusIn", "", "inputID" )
	SetEvent( WIDGET_ID_LOGIN, EVENT_FOCUS_IN, "Event_Login_inputPassword_FocusIn", "", "inputPassword" )
	SetEvent( WIDGET_ID_LOGIN, EVENT_FOCUS_IN, "Event_Login_inputPassNumber_FocusIn", "", "inputPassNumber" )
	
end

-- 로그인 UI 오픈
function OpenLogin()

	-- 저장된 유저 ID
	local strUserID = g_Login:SaveUserID()
	if 0 == string.len( strUserID ) then
	
		-- ID 인풋으로 포커스 설정
		SetFocus( WIDGET_ID_LOGIN, "inputID" )
		VirtuarKeyBoard_SetUI( WIDGET_ID_LOGIN, "inputID", "inputPassword")
		
	else
	
		-- ID 인풋 텍스트 설정
		SetString( WIDGET_ID_LOGIN, "text", strUserID, "inputID" )		
		-- 패스워드 인풋으로 포커스 설정
		SetFocus( WIDGET_ID_LOGIN, "inputPassword" )
		VirtuarKeyBoard_SetUI( WIDGET_ID_LOGIN, "inputPassword", "inputPassNumber")
		
	end
	
	-- ID 저장 여부
	local bSaveUserID = g_Login:IsSaveUserID()
	SetBool( WIDGET_ID_LOGIN, "selected", bSaveUserID, "checkSaveID" )
	
	-- 가상 키보드 UI 오픈
	g_OpenWidget_To_VrKb = WIDGET_ID_LOGIN
	OpenWidget( WIDGET_ID_VIRTUARKEYBOARD )
	
	-- 패스워드 텍스트 초기화
	SetString( WIDGET_ID_LOGIN, "text", "", "inputPassword" )
	-- 보안코드 텍스트 초기화
	SetString( WIDGET_ID_LOGIN, "text", "", "inputPassNumber" )
	
	InsertFrameMove( Update_RandomPassNumber )

end

-- 닫기
function CloseLogin()

	RemoveFrameMove( Update_RandomPassNumber )

	CloseWidget( WIDGET_ID_VIRTUARKEYBOARD )

end

-- ID 인풋 키 입력 이벤트
function Event_Login_inputID_KeyDown( nKeyCode )
	
	-- 탭 키
	if KEYCODE_TAB == nKeyCode then
	
		-- 패스워드 인풋으로 포커스 변경
		SetFocus( WIDGET_ID_LOGIN, "inputPassword" )
		
	elseif KEYCODE_ENTER == nKeyCode then
	
		-- 로그인
		Event_Login_btnOK_Click()
		
	end

end

-- 패스워드 인풋 키 입력 이벤트
function Event_Login_inputPassword_KeyDown( nKeyCode )
	
	-- 탭 키
	if KEYCODE_TAB == nKeyCode then
	
		-- 보안코드 인풋으로 포커스 변경
		SetFocus( WIDGET_ID_LOGIN, "inputPassNumber" )
		
	elseif KEYCODE_ENTER == nKeyCode then
	
		-- 로그인
		Event_Login_btnOK_Click()
		
	end

end

-- 보안코드 인풋 키 입력 이벤트
function Event_Login_inputPassNumber_KeyDown( nKeyCode )
	
	-- 탭 키
	if KEYCODE_TAB == nKeyCode then
	
		-- ID 인풋으로 포커스 변경
		SetFocus( WIDGET_ID_LOGIN, "inputID" )
		
	elseif KEYCODE_ENTER == nKeyCode then
	
		-- 로그인
		Event_Login_btnOK_Click()
		
	end

end

-- 아이디 저장 체크 버튼 클릭 이벤트
function Event_Login_checkSaveID_Click()

	-- 유저 아이디 저장 여부 설정
	local bSelected = GetBool( WIDGET_ID_LOGIN, "selected", "checkSaveID" )
	g_Login:SetSaveUserID( bSelected )
	
end

-- 확인 버튼 클릭 이벤트
function Event_Login_btnOK_Click()

	-- ID
	local strID = GetString( WIDGET_ID_LOGIN, "text", "inputID" )
	-- 패스워드
	local strPW = GetString( WIDGET_ID_LOGIN, "text", "inputPassword" )
	-- 보안코드
	local strRP = GetString( WIDGET_ID_LOGIN, "text", "inputPassNumber" )
	
	-- 아이디 및 패스워드를 입력하세요
	if 0 == string.len( strID ) or 0 == string.len( strPW ) then
	
		local strMsg = GetGameExternalText( "LOGINSTAGE_5", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_MsgBox_Close )
		return
		
	end
	
	-- 서비스 프로바이더가 중국일 경우 ID와 패스워드에 특수문자가 포함되어 있는지 확인
	if SP_CHINA == GetServiceProvider() then
	
		if true == CheckString( strID ) then
		
			OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "LOGIN_PAGE_ID_ERROR", 0 ), Event_MsgBox_Close )
			return
		
		end
		
		if true == CheckString( strPW ) then
		
			OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "LOGIN_PAGE_PW_ERROR", 0 ), Event_MsgBox_Close )
			return
		
		end
	
	end
	
	-- 로그인
	g_Msg:Login( strID, strPW, strRP, g_nServerChannelSelect_SelectIndex )
	
	-- 사용자 정보를 확인합니다. 잠시만 기다려 주십시오.
	--OpenMsgBox( MSG_BOX_TYPE_INFO, GetGameExternalText( "LOGINSTAGE_6", 0 ) )
	
end

-- 취소 버튼 클릭 이벤트
function Event_Login_btnCancel_Click()

	-- 서버 선택 스테이지로
	ToServerSelectStage()

end
-------------------------------------가상 키보드 연결 기능-----------------------------------------------
-- ID 입력란에 포커스가 들어올때 이벤트 발생
function Event_Login_inputID_FocusIn()

	TransInputEn()

	-- 가상 키보드 포커스 셋팅
	VirtuarKeyBoard_SetUI( WIDGET_ID_LOGIN, "inputID", "inputPassword")
	
end

-- 비밀번호 입력란에 포커스가 들어올때 이벤트 발생
function Event_Login_inputPassword_FocusIn()

	-- 가상 키보드 포커스 셋팅
	VirtuarKeyBoard_SetUI( WIDGET_ID_LOGIN, "inputPassword", "inputPassNumber")
	
end

-- 보안코드 입력란에 포커스가 들어올때 이벤트 발생
function Event_Login_inputPassNumber_FocusIn()

	-- 가상 키보드 포커스 셋팅
	VirtuarKeyBoard_SetUI( WIDGET_ID_LOGIN, "inputPassNumber", "inputID")
	
end
---------------------------------------------------------------------------------------------------------
-- 로그인 실패 메시지 ------------------------------------------------------------------------------------

function Login_Error_Msg( strText, funcEvent )

	OpenMsgBox( MSG_BOX_TYPE_OK, strText, funcEvent )

end

---------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 보안코드 Random PassNum 갱신;
function Update_RandomPassNumber ()

	if false == IsOpenWidget( WIDGET_ID_LOGIN ) then
	
		return
		
	end
	
	-- 대만일 경우에만 보안코드를 보여준다
	local nSP = GetServiceProvider()
	if ( nSP == SP_TAIWAN or nSP == SP_OFFICE_TEST )  then
	
		-- Random PassNum 확인
		local nPassNumber = g_Login:GetRandomPassNumber()
		if g_nRandomPassNum == nPassNumber then
			
			return
		
		end
		
		PrintDebug( "Change RandomPassNumber ( " .. tostring( g_nRandomPassNum ) .. " -> " .. tostring( nPassNumber ) .. " )" )
		-- 세팅
		g_nRandomPassNum = nPassNumber
	
		-- 보안코드 텍스트
		local nPassNumber = g_Login:GetRandomPassNumber()
		local strPassNumber = GetGameExternalText( "LOGIN_PAGE_RAND_NUMBER", 0 )	
		strPassNumber = StringFormat( strPassNumber, nPassNumber )
	
		-- 보안코드 레이블 텍스트 설정
		SetString( WIDGET_ID_LOGIN, "text", strPassNumber, "labelPassNumber" )
	
		SetBool( WIDGET_ID_LOGIN, "visible", true, "labelPassNumber" )
		SetBool( WIDGET_ID_LOGIN, "visible", true, "inputPassNumber" )
		SetString( WIDGET_ID_LOGIN, "Restrict", "all", "inputPassNumber" )
		
	else
	
		SetBool( WIDGET_ID_LOGIN, "visible", false, "labelPassNumber" )
		SetBool( WIDGET_ID_LOGIN, "visible", false, "inputPassNumber" )
		
	end	
	
end

-- 클라이언트 손상.
function Login_FailLoadClientVersion()

	Login_Error_Msg( GetGameExternalText( "LOGIN_FAILED_VER_FILE", 0 ), Event_MsgBox_CloseGame )
	
end

-- 버전이 다름.
function Login_DefferentVersion()

	Login_Error_Msg( GetGameExternalText( "LOGIN_FAILED_INVALID_VER", 0 ), Event_MsgBox_CloseGame )
	
end

-- 로그인 실패 메시지
function Login_FailLogin( nError )

	-- 로그인 실패
	if 1 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGIN_FAILED", 0 ), Event_MsgBox_LoginStage )

	-- 시스템 에러로 인한 로그인 실패.
	elseif 2 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_8", 0 ), Event_MsgBox_LoginStage )
		
	-- 허가된 사용시간 에러
	elseif 3 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_9", 0 ), Event_MsgBox_LoginStage )
		
	-- 중복된 접속
	elseif 4 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_10", 0 ), Event_MsgBox_LoginStage )
		
	-- ID/PW 불일치
	elseif 5 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_11", 0 ), Event_MsgBox_LoginStage )
		
	-- 접속 불가 IP
	elseif 6 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_12", 0 ), Event_MsgBox_LoginStage )
		
	-- 접속 불가 ID
	elseif 7 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_BLOCK", 0 ), Event_MsgBox_LoginStage )
		
	-- 비정상적인 로그인 데이터
	elseif 8 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_INVALID_LOGIN", 0 ), Event_MsgBox_CloseGame )
		
	-- 인증시간 초과된 유저
	elseif 9 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_EXPIRED_USER", 0 ), Event_MsgBox_CloseGame )
	
	-- GID가 없음.
	elseif 10 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_NOGID", 0 ), Event_MsgBox_CloseGame )
		
	-- UID가 없음.
	elseif 11 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_NOUID", 0 ), Event_MsgBox_CloseGame )
		
	-- 유저 삽입 실패
	elseif 12 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_UNKNOWN_FAILED", 0 ), Event_MsgBox_CloseGame )
		
	-- 주민번호 오류
	elseif 13 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_SSN_FAILED", 0 ), Event_MsgBox_CloseGame )
		
	-- 성인인증 실패
	elseif 14 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_ADULT_FAILED", 0 ), Event_MsgBox_CloseGame )
		
	-- 최대 동접인원 도달
	elseif 15 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_FULL", 0 ), Event_MsgBox_ServerSelectStage )

	-- 18세 미만 3시간 경과
	elseif 16 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_18LESS_TIME_FAILED", 0 ), Event_MsgBox_CloseGame )
	
	-- 18세 미만 접속 불가능 시간
	elseif 17 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_18LESS_FAILED", 0 ), Event_MsgBox_CloseGame )
	
	-- 18세 이상 접속 불가능 시간
	elseif 18 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_18MORE_FAILED", 0 ), Event_MsgBox_CloseGame )
		
	-- 잘못된 보안코드 
	elseif 19 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_RANDOM_PASS_FAILED", 0 ), Event_MsgBox_LoginStage )
		
	-- 오프라인 상태
	elseif 21 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGIN_ALREADYOFFLINE", 0 ), Event_MsgBox_CloseGame )
		
	-- 베타키 인증 실패
	elseif 23 == nError then
		Login_Error_Msg( GetGameExternalText( "EM_LOGIN_FB_SUB_BETAKEY", 0 ), Event_MsgBox_CloseGame )
	
	-- 말레이시아 접속 불가능한 IP
	elseif 24 == nError then
		Login_Error_Msg( GetGameExternalText( "EM_LOGIN_FB_SUB_IMPOSSIBLEIP", 0 ), Event_MsgBox_CloseGame )
	
	-- 셧다운
	elseif 26 == nError then
		Login_Error_Msg( GetGameExternalText( "EM_LOGIN_FB_KOR_SHUTDOWN", 0 ), Event_MsgBox_CloseGame )

	-- 아이템 변환 중이라 접속할 수 없다
	elseif 29 == nError then
		Login_Error_Msg( GetGameExternalText( "LOGINSTAGE_ITEM_TRANSFERING", 0 ), Event_MsgBox_LoginStage )
		
	end
	
end