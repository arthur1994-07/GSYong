
-- 선택한 캐릭터 이름
g_strClubBattleApply_TargetName = ""

-- 동맹 배틀 신청인가?
g_bClubBattleApply_IsAlliance = false

-- 로드 완료
function LoadClubBattleApply()

	local str = ""
	
	-- 클럽 이름
	str = GetGameWordText( "CLUB_BATTLE_MODAL", 0 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", str, "labelClubNameStatic" )
	
	-- 진행 시간
	str = GetGameWordText( "CLUB_BATTLE_MODAL", 1 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", str, "labelTime" )
	
	-- 시
	str = GetGameWordText( "CLUB_BATTLE_MODAL", 2 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", str, "labelHour" )
	
	-- 분
	str = GetGameWordText( "CLUB_BATTLE_MODAL", 3 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", str, "labelMin" )
	SetEvent( WIDGET_ID_CLUBBATTLEAPPLY, EVENT_FOCUS_OUT, "Ev_ClubBattleApply_CheckMin", "", "inputMin" )
	SetEvent( WIDGET_ID_CLUBBATTLEAPPLY, EVENT_KEY_UP, "Ev_ClubBattleApply_CheckMin", "", "inputMin" )
	
	-- 정보
	str = GetGameInternalText( "CLUB_BATTLE_MODAL_INFO", 0 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", str, "labelInfo" )
	
	-- 확인
	str = GetGameWordText( "MODAL_BUTTON", 0 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "label", str, "btnOK" )
	SetEvent( WIDGET_ID_CLUBBATTLEAPPLY, EVENT_BUTTON_CLICK, "Ev_ClubBattleApply_OK", "", "btnOK" )
	
	-- 취소
	str = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "label", str, "btnCancel" )
	SetEvent( WIDGET_ID_CLUBBATTLEAPPLY, EVENT_BUTTON_CLICK, "Ev_ClubBattleApply_Cancel", "", "btnCancel" )

end

-- 오픈
function OpenClubBattleApply()

	-- 시간 설정값 초기화
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", "0", "inputHour" )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", "10", "inputMin" )
	
	-- 상대방 클럽 이름
	local strClubName = g_Client:GetOhterCharClubName( g_strClubBattleApply_TargetName )
	SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", strClubName, "labelClubName" )

end

-- 배틀 신청 오픈
function ClubBattleApply_Open( strName, bAlliance )

	g_strClubBattleApply_TargetName = strName
	g_bClubBattleApply_IsAlliance = bAlliance
	
	OpenWidget( WIDGET_ID_CLUBBATTLEAPPLY )

end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 분 단위 입력 체크
function Ev_ClubBattleApply_CheckMin()

	local nMin = tonumber( GetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", "inputMin" ) )
	if nil == nMin then
		return
	end
	
	if 60 <= nMin then
	
		SetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", "59", "inputMin" )
	
	end

end

-- 확인
function Ev_ClubBattleApply_OK()

	local nHour = tonumber( GetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", "inputHour" ) )
	local nMin = tonumber( GetString( WIDGET_ID_CLUBBATTLEAPPLY, "text", "inputMin" ) )
	
	if nil == nHour or nil == nMin then
		return
	end
	
	-- 둘 다 0이면 안된다.
	if 0 == nHour and 0 == nMin then
		return
	end

	-- 클럽 배틀 신청
	if false == g_bClubBattleApply_IsAlliance then
	
		g_Msg:ReqClubBattle( g_strClubBattleApply_TargetName, nHour, nMin )
	
	-- 동맹 배틀 신청
	else
	
		g_Msg:ReqAllianceBattle( g_strClubBattleApply_TargetName, nHour, nMin )
	
	end
	
	CloseWidget( WIDGET_ID_CLUBBATTLEAPPLY )

end

-- 취소
function Ev_ClubBattleApply_Cancel()

	CloseWidget( WIDGET_ID_CLUBBATTLEAPPLY )

end