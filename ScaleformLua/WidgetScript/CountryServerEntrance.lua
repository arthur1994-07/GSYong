
-- 난사군도 전장 서버 입장 오픈
function OpenCountryServerEntrance()

	if true == g_UserFeatures:IsUsingJoinWorldBattle() then
	
		SetString( WIDGET_ID_COUNTRYSERVERENTRANCE, "overlayImg", "mcEnter", "btnEntrance" )
		SetEvent( WIDGET_ID_COUNTRYSERVERENTRANCE, EVENT_BUTTON_CLICK, "Ev_CountryServerEntrance_Confirm", "", "btnEntrance" )
	
	elseif true == g_UserFeatures:IsUsingWorldBattle() then
	
		SetString( WIDGET_ID_COUNTRYSERVERENTRANCE, "overlayImg", "mcExit", "btnEntrance" )
		SetEvent( WIDGET_ID_COUNTRYSERVERENTRANCE, EVENT_BUTTON_CLICK, "Ev_CountryServerEntrance_Exit", "", "btnEntrance" )
	
	end

end

-----------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 전장 서버 입장 실패
function CountryServerEntrance_FailEnter( nError )

	local str = ""
	
	-- 실패
	if 0 == nError then
	
		str = GetGameInternalText( "WB_SERVER_ACCESS_ERROR", 0 )
	
	-- 전장 서버 오프라인
	elseif 1 == nError then
	
		str = GetGameInternalText( "WB_SERVER_ACCESS_ERROR", 0 )
	
	-- 캐릭터 아이템 유효하지 않음
	elseif 2 == nError then
	
		str = GetGameInternalText( "WB_SERVER_ACCESS_ERROR_INVALID_ITEM", 0 )
		
	-- 이동할 수 없는 위치
	elseif 3 == nError then
	
		str = StringFormat( GetGameInternalText( "PVE_ENTRANCE_ACK_CONFIRM_TEXT", 1 ), g_MyChar:GetName() )
	
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close, Event_MsgBox_Close )

end

-- 전장 서버 입장 대기
function CountryServerEntrance_EnterWait()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "WAITSERVER_MESSAGE", 0 ),
		Ev_CountryServerEntrance_Enter, nil, nil, nil, 5.0, Ev_CountryServerEntrance_Enter )

end

-----------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 전장 서버 입장 확인
function Ev_CountryServerEntrance_Confirm()

	local str = GetGameInternalText( "CONFIRM_JOIN_PVP_SERVER", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CountryServerEntrance_OK, Event_MsgBox_Close )

end

-- 전장 서버 입장 확인 OK
function Ev_CountryServerEntrance_OK()

	Event_MsgBox_Close()
	
	-- 전장 서버 입장 요청
	g_Msg:ReqJoiinWorldBattleServer()

end

-- 전장 서버 입장 대기 끝
function Ev_CountryServerEntrance_Enter()

	Event_MsgBox_Close()
	
	-- 월드 배틀 로비 스테이지 이동
	GameToLobbyStage( true )

end

-- 전장 서버 나가기
function Ev_CountryServerEntrance_Exit()

	local str = GetGameInternalText( "MODAL_ESC_SERVER_STAGE", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CountryServerEntrance_ExitWait, Event_MsgBox_Close )

end

-- 전장 서버 나가기 확인 - 대기
function Ev_CountryServerEntrance_ExitWait()

	Event_MsgBox_Close()
	
	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "WAITSERVER_MESSAGE", 0 ),
		Ev_CountryServerEntrance_ExitOK, nil, nil, nil, 5.0, Ev_CountryServerEntrance_ExitOK )

end

-- 전장 서버 나가기 확인
function Ev_CountryServerEntrance_ExitOK()

	Event_MsgBox_Close()
	
	GameToLobbyStage( false )
	
	-- 서버 연결을 끊어버리니 연결 시도해본다.
	--TryConnectLoginServer()

end