
g_nBasicChatOp_OptionFlagTemp = 0

-- 채팅 옵션 UI 로드 완료
function LoadBasicChatOp()

	-- 텍스트 설정
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION_WINDOW_STATIC", 1 ), "labelNormal" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION_WINDOW_STATIC", 2 ), "labelChannel" )
	
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 0 ), "checkNormal_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 2 ), "checkParty_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 3 ), "checkClub_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 4 ), "checkAllience_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 1 ), "checkPrivate_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 5 ), "checkArea_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 6 ), "checkPartyRecruit_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 8 ), "checkSystem_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 7 ), "checkAll_text" )
	SetString( WIDGET_ID_BASICCHATOP, "text", GetGameWordText( "CHAT_OPTION", 9 ), "checkFaction_text" )
	
	SetString( WIDGET_ID_BASICCHATOP, "label", GetGameWordText( "CHAT_OK_BUTTON", 0 ), "btnOK" )
	SetString( WIDGET_ID_BASICCHATOP, "label", GetGameWordText( "CHAT_OK_BUTTON", 1 ), "btnCancel" )
	
	-- 이벤트 설정
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckNormal", "", "checkNormal" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckParty", "", "checkParty" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckClub", "", "checkClub" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckAllience", "", "checkAllience" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckPrivate", "", "checkPrivate" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckArea", "", "checkArea" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckPartyRecruit", "", "checkPartyRecruit" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckSystem", "", "checkSystem" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckAll", "", "checkAll" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_CheckFaction", "", "checkFaction" )
	
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_btnOK", "", "btnOK" )
	SetEvent( WIDGET_ID_BASICCHATOP, EVENT_BUTTON_CLICK, "Ev_BasicChatOp_btnCancel", "", "btnCancel" )

end

-- 채팅 옵션 오픈
function OpenBasicChatOp()

	-- UI 위치 설정
	local nChatX = GetInt( WIDGET_ID_BASICCHAT, "x", "" )
	local nChatY = GetInt( WIDGET_ID_BASICCHAT, "y", "" )
	
	local nHeight = GetInt( WIDGET_ID_BASICCHATOP, "height", "" )
	local nNewY = nChatY - nHeight
	
	if 0 > nNewY then
		nNewY = nChatY + GetInt( WIDGET_ID_BASICCHAT, "height", "" )
	end

	SetInt( WIDGET_ID_BASICCHATOP, "x", nChatX, "" )
	SetInt( WIDGET_ID_BASICCHATOP, "y", nNewY, "" )
	
	BasicChatOp_SetCheck()
	
	g_nBasicChatOp_OptionFlagTemp = g_nBasicChat_OptionFlag

end

-- 체크 상태 설정
function BasicChatOp_SetCheck()

	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_NORMAL ), "checkNormal" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_PARTY ), "checkParty" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_GUILD ), "checkClub" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_ALLIANCE ), "checkAllience" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_PRIVATE ), "checkPrivate" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_AREA ), "checkArea" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_PARTY_RECRUIT ), "checkPartyRecruit" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_SYSTEM ), "checkSystem" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_TOALL ), "checkAll" )
	SetBool( WIDGET_ID_BASICCHATOP, "selected", ( g_nBasicChat_OptionFlag & CHAT_FACTION ), "checkFaction" )

end

-----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 체크 박스 
function Ev_BasicChatOp_CheckNormal()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkNormal" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_NORMAL )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_NORMAL )
	end
	
end

function Ev_BasicChatOp_CheckParty()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkParty" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_PARTY )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_PARTY )
	end
	
end

function Ev_BasicChatOp_CheckClub()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkClub" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_GUILD )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_GUILD )
	end
	
end

function Ev_BasicChatOp_CheckAllience()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkAllience" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_ALLIANCE )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_ALLIANCE )
	end
	
end

function Ev_BasicChatOp_CheckPrivate()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkPrivate" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_PRIVATE )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_PRIVATE )
	end
	
end

function Ev_BasicChatOp_CheckArea()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkArea" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_AREA )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_AREA )
	end
	
end

function Ev_BasicChatOp_CheckPartyRecruit()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkPartyRecruit" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_PARTY_RECRUIT )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_PARTY_RECRUIT )
	end
	
end

function Ev_BasicChatOp_CheckSystem()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkSystem" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_SYSTEM )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_SYSTEM )
	end
	
end

function Ev_BasicChatOp_CheckAll()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkAll" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_TOALL )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_TOALL )
	end
	
end

function Ev_BasicChatOp_CheckFaction()

	if true == GetBool( WIDGET_ID_BASICCHATOP, "selected", "checkFaction" ) then
		g_nBasicChatOp_OptionFlagTemp = FlagAdd( g_nBasicChatOp_OptionFlagTemp, CHAT_FACTION )
	else
		g_nBasicChatOp_OptionFlagTemp = FlagSub( g_nBasicChatOp_OptionFlagTemp, CHAT_FACTION )
	end

end

function Ev_BasicChatOp_btnOK()

	g_nBasicChat_OptionFlag = g_nBasicChatOp_OptionFlagTemp
	CloseWidget( WIDGET_ID_BASICCHATOP )
	
end

function Ev_BasicChatOp_btnCancel()

	CloseWidget( WIDGET_ID_BASICCHATOP )
	
end