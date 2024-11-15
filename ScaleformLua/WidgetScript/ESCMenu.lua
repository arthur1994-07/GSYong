
-- 시스템 메뉴창 ( ESC Menu ) UI
function LoadEscSystemMenu()

	local nSP = GetServiceProvider()
	if nSP == SP_OFFICE_TEST or 
		nSP == SP_KOREA_TEST or 
		nSP == SP_KOREA then
		CallFunction( WIDGET_ID_ESC, "gotoAndStop", "ESC_STD_LABEL", "menu" )
	else
		if nSP == SP_TAIWAN then
			CallFunction( WIDGET_ID_ESC, "gotoAndStop", "ESC_TAIWAN_LABEL", "menu" )
		else
			CallFunction( WIDGET_ID_ESC, "gotoAndStop", "ESC_OTHER_LABEL", "menu" )
		end
	end
	
	-- 서버 선택
	local str = GetGameWordText( "ESC_MENU", 1 )
	SetString( WIDGET_ID_ESC, "label", str, "menu.btnServerSelect" )
	SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnServerSelect", "", "menu.btnServerSelect" )
	
	-- 옵션
	str = GetGameWordText( "ESC_MENU", 2 )
	SetString( WIDGET_ID_ESC, "label", str, "menu.btnOption" )
	SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnOption", "", "menu.btnOption" )
	
	-- 단축키 설정
	str = GetGameWordText( "ESC_MENU", 5 )
	SetString( WIDGET_ID_ESC, "label", str, "menu.btnKeyboardShortcut" )
	SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnHotKey", "", "menu.btnKeyboardShortcut" )
	
	-- 도움말
	str = GetGameWordText( "ESC_MENU", 3 )
	SetString( WIDGET_ID_ESC, "label", str, "menu.btnHelp" )
	SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnHelp", "", "menu.btnHelp" )
	
	if nSP == SP_OFFICE_TEST or 
		nSP == SP_KOREA_TEST or 
		nSP == SP_KOREA then
		
		-- 등급 표시
		str = GetGameWordText( "ESC_MENU", 6 )
		SetString( WIDGET_ID_ESC, "label", str, "menu.btnMarkOfRank" )
		SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnGrade", "", "menu.btnMarkOfRank" )
		
		-- 고객센터
		str = GetGameWordText( "ESC_MENU", 7 )
		SetString( WIDGET_ID_ESC, "label", str, "menu.btnCustomerService" )
		SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnCustomerCenter", "", "menu.btnCustomerService" )
		
		-- 포럼 바로가기
		str = GetGameWordText( "ESC_MENU", 9 )
		SetString( WIDGET_ID_ESC, "label", str, "menu.btnForum" )
		SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnForum", "", "menu.btnForum" )
		
	end
	
	if nSP == SP_TAIWAN then
		
		-- 포럼 바로가기
		str = GetGameWordText( "ESC_MENU", 9 )
		SetString( WIDGET_ID_ESC, "label", str, "menu.btnForum" )
		SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnForum", "", "menu.btnForum" )
		
		PrintDebug( "TAIWAN" );
		
	end
	
	-- 게임 종료
	str = GetGameWordText( "ESC_MENU", 4 )
	SetString( WIDGET_ID_ESC, "label", str, "menu.btnGameClose" )
	SetEvent( WIDGET_ID_ESC, EVENT_BUTTON_CLICK, "Ev_EscMenu_btnQuit", "", "menu.btnGameClose" )

end

-- 메세지 박스 열기 - 서버 선택 클릭 시
function EscMenu_ServerSelect_OpenMsgBox()

	local nRebuyCount = g_NpcTalk:GetRebuyCount()

	local strMsgBox = ""
	if true == g_NpcTalk:GetRebuyItemDeleteConfirm() and 0 ~= nRebuyCount then
	
		strMsgBox = StringFormat( 
			GetGameInternalText( "MODAL_ESC_SERVER_STAGE_CHECK_REBUY_ITEMS", 0 ), nRebuyCount )
	
	else
	
		strMsgBox = GetGameInternalText( "MODAL_ESC_SERVER_STAGE", 0 )
	
	end

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsgBox, EscMenu_ServerSelectTimerMsg, Event_MsgBox_Close )

end

-- 서버 변경 타이머 메시지
function EscMenu_ServerSelectTimerMsg()

	Event_MsgBox_Close()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "WAITSERVER_MESSAGE", 0 ), 
		Ev_EscMenu_MsgBoxServerSelect_OK, nil, nil, nil, 5, Ev_EscMenu_MsgBoxServerSelect_OK )

end

--------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 버튼 이벤트 - 서버 선택
function Ev_EscMenu_btnServerSelect()
	
	CloseWidget( WIDGET_ID_ESC )
	
	-- 메세지 박스 열기 - 서버 선택 클릭 시
	EscMenu_ServerSelect_OpenMsgBox()

end

-- 메세지 박스 열기 - 서버 선택 예 클릭 시 서버 선택으로 이동
function Ev_EscMenu_MsgBoxServerSelect_OK()

	Event_MsgBox_Close()

	GameToLobbyStage( false )
	
	-- 서버 연결을 끊어버리니 연결 시도해본다.
	TryConnectLoginServer()
	
end

-- 버튼 이벤트 - 옵션
function Ev_EscMenu_btnOption()

	-- UI 가 닫혀있으면은 UI를 연다
	if	false == IsOpenWidget( WIDGET_ID_GAMEOPTION ) then
		OpenWidget( WIDGET_ID_GAMEOPTION )
	end
	
	CloseWidget( WIDGET_ID_ESC )
	
end

-- 버튼 이벤트 - 단축키 설정
function Ev_EscMenu_btnHotKey()

	-- UI 가 닫혀있으면은 UI를 연다
	if	false == IsOpenWidget( WIDGET_ID_HOTKEYCFG ) then
		OpenWidget( WIDGET_ID_HOTKEYCFG )
	end

	CloseWidget( WIDGET_ID_ESC )
	
end

-- 버튼 이벤트 - 도움말
function Ev_EscMenu_btnHelp()

	OpenWidget( WIDGET_ID_HELP )
	CloseWidget( WIDGET_ID_ESC )
	
end

-- 버튼 이벤트 - 등급 표시
function Ev_EscMenu_btnGrade()
	
	OpenWidget( WIDGET_ID_ESC_GRADE_DISPLAY )
	CloseWidget( WIDGET_ID_ESC )
	
end

-- 버튼 이벤트 - 고객센터	CONFIRM_CUSTOMER_CENTER
function Ev_EscMenu_btnCustomerCenter()

	CloseWidget( WIDGET_ID_ESC )
	
	EscMenu_CustomerCenter_OpenMsgBox()
	
end

-- 포럼 바로가기
function Ev_EscMenu_btnForum()

	CloseWidget( WIDGET_ID_ESC )
	OpenForum()

end

-- 메세지 박스 열기 - 고객센터 클릭시
function EscMenu_CustomerCenter_OpenMsgBox()

	-- 고객센터 연결 시 게임실행 창이 최소화됩니다.	CONFIRM_CUSTOMER_CENTER
	-- 연결하시겠습니까?
	local str = GetGameInternalText( "CONFIRM_CUSTOMER_CENTER", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_EscMenu_MsgBox_CustomerCenter_OK, Event_MsgBox_Close )
	
end

-- 메세지 박스 열기 - 고객센터 클릭 예 클릭 시 고객센터으로 이동
function Ev_EscMenu_MsgBox_CustomerCenter_OK()

	Event_MsgBox_Close()
	ToCustomerCenter()
	
end

-- 버튼 이벤트 - 게임 종료
function Ev_EscMenu_btnQuit()

	local nRebuyCount = g_NpcTalk:GetRebuyCount()

	local str = ""
	if true == g_NpcTalk:GetRebuyItemDeleteConfirm() and 0 ~= nRebuyCount then
	
		str = StringFormat( 
			GetGameInternalText( "MODAL_ESC_MENU_CLOSE_CHECK_REBUY_ITEMS", 0 ), g_NpcTalk:GetRebuyCount() )
	
	else
	
		str =  GetGameInternalText( "CLOSE_GAME", 0 )
	
	end

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_EseMenu_CloseGame, Event_MsgBox_Close )
	
end

-- 게임 종료
function Ev_EseMenu_CloseGame()

	Event_MsgBox_Close()
	CloseGame()

end