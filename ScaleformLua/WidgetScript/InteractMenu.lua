
-- 타겟 이름
g_strInteractMenu_Name = ""

g_bInteractMenu_BikeEnable = false

-- 상호작용 메뉴 로드 완료
function LoadInteractMenu()

	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_WIDGET_CLOSE, "CloseInteractMenu", "", "" )

	-- 거래 요청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 0 ), "menu.btnTrade" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_Trade", "", "menu.btnTrade" )
	
	-- 귓속말
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 1 ), "menu.btnWhisper" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_Whisper", "", "menu.btnWhisper" )
	
	-- 파티 초대
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 2 ), "menu.btnParty" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_InviteParty", "", "menu.btnParty" )
	
	-- 대련 신청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 3 ), "menu.btnConflict" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_Conflict", "", "menu.btnConflict" )
	
	-- 친구 추가
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 4 ), "menu.btnFriend" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_Friend", "", "menu.btnFriend" )
	
	-- 클럽 가입 요청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 5 ), "menu.btnClub" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_InviteClub", "", "menu.btnClub" )
	
	-- 동맹 가입 요청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 6 ), "menu.btnUnion" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_InviteUnion", "", "menu.btnUnion" )
	
	-- 클럽 배틀 신청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 7 ), "menu.btnClubBattle" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_ClubBattle", "", "menu.btnClubBattle" )
	
	-- 탈것 탑승 요청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 9 ), "menu.btnBike" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_WithBike", "", "menu.btnBike" )
	
	-- 동맹 배틀 신청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 8 ), "menu.btnUnionBattle" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_UnionBattle", "", "menu.btnUnionBattle" )
	
	-- 그룹 채팅 신청
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 11 ), "menu.btnGroupChat" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_GroupChat", "", "menu.btnGroupChat" )
	
	-- 정보 보기
	SetString( WIDGET_ID_INTERACT_MENU, "label", GetGameWordText( "P2PMENU", 10 ), "menu.btnInfo" )
	SetEvent( WIDGET_ID_INTERACT_MENU, EVENT_BUTTON_CLICK, "Ev_InteractMenu_ReferChar", "", "menu.btnInfo" )
	
end

function OpenInteractMenu()

	local tbMousePos = GetMousePos()	
	local nMousePosX = tbMousePos[ "x" ] - 10
	local nMousePosY = tbMousePos[ "y" ] - 30	
	nMousePosX, nMousePosY = PreventOutsideDrag( WIDGET_ID_INTERACT_MENU, nMousePosX, nMousePosY, 0, 0, 0, 0 )
	
	SetInt( WIDGET_ID_INTERACT_MENU, "x", nMousePosX, "" )
	SetInt( WIDGET_ID_INTERACT_MENU, "y", nMousePosY, "" )

	InteractMenu_EnableBike( g_bInteractMenu_BikeEnable )
	InsertStageMouseClick( InteractMenu_Close )
	
end

function CloseInteractMenu()

	RemoveStageMouseClick( InteractMenu_Close )

end

-- 닫기
function InteractMenu_Close()

	CloseWidget( WIDGET_ID_INTERACT_MENU )
	
end

function InteractMenu_Reset()

	g_bInteractMenu_BikeEnable = false

end

-- 탈것 탑승 버튼 활성 설정
function InteractMenu_EnableBike( bEnable )

	g_bInteractMenu_BikeEnable = bEnable

	if false == IsOpenWidget( WIDGET_ID_INTERACT_MENU ) then
		return
	end

	SetBool( WIDGET_ID_INTERACT_MENU, "enabled", g_bInteractMenu_BikeEnable, "menu.btnBike" )

end

-------------------------------------------------------------------------------------------------
-- 클라이언트 콜

function InteractMenu_Open( strName )

	g_strInteractMenu_Name = strName
	OpenWidget( WIDGET_ID_INTERACT_MENU )
	
end

-------------------------------------------------------------------------------------------------
-- 이벤트

-- 거래 신청
function Ev_InteractMenu_Trade()

	g_Msg:ReqTradeOpen()
	
	CloseWidget( WIDGET_ID_INTERACT_MENU )
	
end

-- 대련 신청
function Ev_InteractMenu_Conflict()

	if false == IsOpenWidget( WIDGET_ID_CONFLICT_REQUEST ) then
	
		OpenWidget( WIDGET_ID_CONFLICT_REQUEST )
		CloseWidget( WIDGET_ID_INTERACT_MENU )

	else
	
		CloseWidget( WIDGET_ID_CONFLICT_REQUEST )
		OpenWidget( WIDGET_ID_CONFLICT_REQUEST )
		CloseWidget( WIDGET_ID_INTERACT_MENU )

	end
	
end

-- 친구 추가 
function Ev_InteractMenu_Friend()

	CloseWidget( WIDGET_ID_INTERACT_MENU )
	
	if true == g_Friend:IsFriend( g_strInteractMenu_Name ) then
		CF_PrintMsg_SystemMsg( StringFormat( GetGameInternalText( "FRIEND_ADD_ALREADY" ), g_strInteractMenu_Name ) , TEXT_COLOR_RED )
		return
	end
	
	local str = StringFormat( GetGameInternalText( "FRIEND_ADD", 0 ), g_strInteractMenu_Name )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_InteractMenu_AddFriend, Event_MsgBox_Close )

end

-- 정보 보기
function Ev_InteractMenu_ReferChar()

	if false == IsOpenWidget(WIDGET_ID_REFERCHAR) then
	
		g_Msg:ReqReferChar( g_strInteractMenu_Name )
		g_ReferChar_Name = g_strInteractMenu_Name
		
		CloseWidget( WIDGET_ID_INTERACT_MENU )
		
	else
	
		g_Msg:ReqReferChar( g_strInteractMenu_Name )
		
		CloseWidget( WIDGET_ID_INTERACT_MENU )
		CloseWidget( WIDGET_ID_REFERCHAR )

	end
	
end

-- 그룹 채팅 확인 메시지
function Ev_InteractMenu_GroupChat()

	GroupChat_InviteMsg( g_strInteractMenu_Name )
	
end

-- 파티 신청 확인 메시지
function Ev_InteractMenu_InviteParty()

	MsgInviteParty( g_strInteractMenu_Name )
	
end

-- 클럽 초대
function Ev_InteractMenu_ClubJoin()

	MsgInviteClubJoin( g_strInteractMenu_Name )
	
end

-- 귓속말
function Ev_InteractMenu_Whisper()

	CloseWidget( WIDGET_ID_INTERACT_MENU )

	if 0 == string.len( g_strInteractMenu_Name ) then
		return
	end

	BasicChat_SetWhisper( g_strInteractMenu_Name )
	
end

-- 탈것 탑승 요청
function Ev_InteractMenu_WithBike()

	if false == g_MyChar:IsRideAVehicleNow() then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE5", 0 ), TEXT_COLOR_RED )
		
	else
	
		g_Msg:ReqBikePutOnWith()
		
	end
	CloseWidget( WIDGET_ID_INTERACT_MENU )
end

-- 클럽 초대
function Ev_InteractMenu_InviteClub()

	if  false == g_MyChar:HaveClubJoinAutority() and 
		false == g_MyChar:IsClubMaster() then

		CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE2", 0 ), TEXT_COLOR_RED )
		return

	end
	
	Ev_InteractMenu_ClubJoin()
	
end

-- 동맹 초대
function Ev_InteractMenu_InviteUnion()

	CloseWidget( WIDGET_ID_INTERACT_MENU )
	
	if  false == g_MyChar:HaveClubAllianceAuthority() and
		false == g_MyChar:IsClubMaster() then	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE2", 0 ), TEXT_COLOR_RED )
		return
	end
	
	local str = StringFormat( GetGameInternalText( "UNION_JOIN", 0 ), g_strInteractMenu_Name )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_InteractMenu_ClubUnionJoin, Event_MsgBox_Close )
	
end

-- 클럽 배틀 신청
function Ev_InteractMenu_ClubBattle()

	CloseWidget( WIDGET_ID_INTERACT_MENU )

	if  false == g_MyChar:HaveClubBattleAuthority() and
		false == g_MyChar:IsClubMaster() then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE3", 0 ), TEXT_COLOR_RED )
		return
		
	end
	
	ClubBattleApply_Open( g_strInteractMenu_Name, false )
	
end

-- 동맹 배틀 신청
function Ev_InteractMenu_UnionBattle()

	CloseWidget( WIDGET_ID_INTERACT_MENU )

	if false == g_MyChar:IsAllianceMaster() then	
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE4", 0 ), TEXT_COLOR_RED )
		return
		
	end
	
	ClubBattleApply_Open( g_strInteractMenu_Name, true )
	
end

-- 파티 요청
function Event_MsgBox_PartyReq( tbData )

	if nil == tbData[ 1 ] then
		tbData[ 1 ] = g_strInteractMenu_Name
	end
	
	g_Msg:LureNameParty( tbData[ 1 ] )
	
	Event_MsgBox_Close()
	
end

-- 파티 초대(내가 할 때)
function MsgInviteParty(strName)

	local StrMessage = StringFormat( GetGameInternalText( "PARTY_QUESTION", 0 ), strName )
	local tbData = { strName }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, StrMessage, Event_MsgBox_PartyReq, Event_MsgBox_Close, tbData )
	
end

-- 클럽 초대 OK
function Event_MsgBox_ClubJoin( tbData )

	g_Msg:ReqClubJoinByCharname( tbData[ 1 ] )
	
	Event_MsgBox_Close()
	
end

-- 클럽 초대
function MsgInviteClubJoin( StrTarName )

	local StrMessage = StringFormat( GetGameInternalText( "CLUB_JOIN", 0 ), StrTarName )
	local tbData = { StrTarName }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, StrMessage, Event_MsgBox_ClubJoin, Event_MsgBox_Close, tbData )
	
end

-- 친구 추가 메시지
function Ev_InteractMenu_AddFriend()

	g_Msg:ReqFriendAdd( g_strInteractMenu_Name )
	
	Event_MsgBox_Close()

end

function Ev_InteractMenu_ClubUnionJoin()
	g_Msg:ReqClubAlliance()
	Event_MsgBox_Close()
end
