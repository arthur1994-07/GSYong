
--------------------------------------------------------------------------------------------------------------------
-- 스테이지 이벤트 관련

-- 스테이지 마우스 클릭 이벤트 
g_tbStageMouseClickFunc = {}
g_tbStageMouseClickFuncLater = {}

-- 스테이지 마우스 이동 이벤트 리스트
g_tbStageMouseMoveFunc = {}
-- 스테이지 마우스 업 이벤트 리스트
g_tbStageMouseUpFunc = {}

-- 스테이지 키 이벤트 리스트
g_tbStageKeyUpFunc = {}

function InsertStageMouseClick( func )

	if 0 == table.getn( g_tbStageMouseClickFunc ) then
		SetStageEvent( EVENT_MOUSE_CLICK, "Stage_MouseClickEvent", "stageX;stageY;" )
	end
	
	for value in pairs( g_tbStageMouseClickFunc ) do
		if func == g_tbStageMouseClickFunc[ value ] then
			return
		end
	end

	table.insert( g_tbStageMouseClickFunc, func )
	
end

function InsertStageMouseClickLater( func )

	if 0 == table.getn( g_tbStageMouseClickFunc ) then
		SetStageEvent( EVENT_MOUSE_CLICK, "Stage_MouseClickEvent", "stageX;stageY;" )
	end

	for value in pairs( g_tbStageMouseClickFuncLater ) do
		if func == g_tbStageMouseClickFuncLater[ value ] then
			return
		end
	end

	table.insert( g_tbStageMouseClickFuncLater, func )
	
end

function InsertStageMouseMove( func )

	if 0 == table.getn( g_tbStageMouseMoveFunc ) then
		SetStageEvent( EVENT_MOUSE_MOVE, "Stage_MouseMoveEvent", "stageX;stageY;" )
	end
	
	for value in pairs( g_tbStageMouseMoveFunc ) do
		if func == g_tbStageMouseMoveFunc[ value ] then
			return
		end
	end

	table.insert( g_tbStageMouseMoveFunc, func )
	
end

function InsertStageMouseUp( func )

	if 0 == table.getn( g_tbStageMouseUpFunc ) then
		SetStageEvent( EVENT_MOUSE_UP, "Stage_MouseUpEvent", "stageX;stageY;" )
	end
	
	for value in pairs( g_tbStageMouseUpFunc ) do
		if func == g_tbStageMouseUpFunc[ value ] then
			return
		end
	end

	table.insert( g_tbStageMouseUpFunc, func )
	
end

function RemoveStageMouseClick( func )

	for value in pairs( g_tbStageMouseClickFunc ) do
		if func == g_tbStageMouseClickFunc[ value ] then
			table.remove( g_tbStageMouseClickFunc, value )
			break
		end
	end
	
	if 0 == table.getn( g_tbStageMouseClickFunc ) then
		SetStageEvent( EVENT_MOUSE_CLICK, "", "" )
	end

end

function RemoveStageMouseMove( func )

	for value in pairs( g_tbStageMouseMoveFunc ) do
		if func == g_tbStageMouseMoveFunc[ value ] then
			table.remove( g_tbStageMouseMoveFunc, value )
			break
		end
	end
	
	if 0 == table.getn( g_tbStageMouseMoveFunc ) then
		SetStageEvent( EVENT_MOUSE_MOVE, "", "" )
	end

end

function RemoveStageMouseUp( func )

	for value in pairs( g_tbStageMouseUpFunc ) do
		if func == g_tbStageMouseUpFunc[ value ] then
			table.remove( g_tbStageMouseUpFunc, value )
			break
		end
	end
	
	if 0 == table.getn( g_tbStageMouseUpFunc ) then
		SetStageEvent( EVENT_MOUSE_UP, "", "" )
	end

end

-- 스테이지 키 이벤트 등록
function InsertStageKeyUp( func )

	-- 중복 체크
	for value in pairs( g_tbStageKeyUpFunc ) do
	
		if func == g_tbStageKeyUpFunc[ value ] then
			return
		end
	
	end
	
	table.insert( g_tbStageKeyUpFunc, func )

end

-- 스테이지 키 이벤트 삭제
function RemoveStageKeyUp( func )

	for value in pairs( g_tbStageKeyUpFunc ) do
	
		if func == g_tbStageKeyUpFunc[ value ] then
		
			table.remove( g_tbStageKeyUpFunc, value )
			return
			
		end
	
	end

end

--------------------------------------------------------------------------------------------------------------------

-- FrameMove 등록 테이블
g_tbStageFrameMove = {}

function InsertFrameMove( func )

	for value in pairs( g_tbStageFrameMove ) do
	
		if func == g_tbStageFrameMove[ value ] then
			return
		end
		
	end

	table.insert( g_tbStageFrameMove, func )
	
end

function RemoveFrameMove( func )

	for value in pairs( g_tbStageFrameMove ) do
	
		if func == g_tbStageFrameMove[ value ] then
			table.remove( g_tbStageFrameMove, value )
			break
		end
		
	end

end

-- 업데이트
function FrameMove_Stage( fElapsedTime )

	for value in pairs( g_tbStageFrameMove ) do
	
		local func = g_tbStageFrameMove[ value ]
		func( fElapsedTime )
	
	end

end


-- 유틸리티 ------------------------------------------------------------------------------------------------

-- UI 토글
function ToggleUI( nID )

	if nID == WIDGET_ID_COMPETITION and true == g_Client:IsCharPrison() then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "UI_RECEIVE_PRISION_LIMIT", 0 ), TEXT_COLOR_RED )
		return
	end
	
	if false == IsOpenWidget( nID ) then
		OpenWidget( nID )
	else
		CloseWidget( nID )
	end

end

-- UI 화면 밖으로 안나가도록 설정
function PreventOutsideDrag( nID, nX, nY, nTop, nLeft, nBottom, nRight, SubID, strInst)

	if SubID == nil then
		SubID = 0
	end

	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	if strInst == nil then
		strInst = ""
	end

	local nWidth = GetInt( nID, "width", strInst , SubID)
	local nHeight = GetInt( nID, "height", strInst, SubID)
	
	if nLeft > nX then
		nX = nLeft
	end
	
	if nTop > nY then
		nY = nTop
	end
	
	if nScreenWidth + nRight < nX + nWidth then
		nX = nScreenWidth - nWidth + nRight
	end
	
	if nScreenHeight + nBottom < nY + nHeight then
		nY = nScreenHeight - nHeight + nBottom
	end
	
	return nX, nY

end

-- 윈도우 visible 설정
function SetWindowVisible( nID, bVisible, nSubID )

	SetBool( nID, "visible", bVisible, "closeBtn", nSubID )
	SetBool( nID, "visible", bVisible, "titleBtn", nSubID )
	SetBool( nID, "visible", bVisible, "mcFocus", nSubID )
	SetBool( nID, "visible", bVisible, "hit", nSubID )
	SetBool( nID, "visible", bVisible, "background", nSubID )
	HitTestEnable( nID, bVisible, "hit", nSubID )

end

-- 기본 UI visible 설정 ------------------------------------------------------------------------------------------

-- 서버 선택 스테이지
function ToServerSelectStage()

	-- 게임 외부에선 단축키 처리를 안한다. 스테이지 키 이벤트로 등록하자.
	InsertStageKeyUp( Ev_MsgBox_KeyUp )
	
	-- 채팅 로그 클리어
	ClearStoredChat( CHAT_LOG_BASIC )
	ClearStoredChat( CHAT_LOG_BASIC_PARTY )
	ClearStoredChat( CHAT_LOG_BASIC_CLUB )
	ClearStoredChat( CHAT_LOG_BASIC_ALLIANCE )
	ClearStoredChat( CHAT_LOG_BASIC_WHISPER )
	ClearStoredChat( CHAT_LOG_BASIC_SYSTEM )
	
	Init_BeforeJoinToGameStage()
		
	MsgBox_Reset()
	MsgBoxModaless_Reset()
	ServerSelect_Reset()
	CharSelect_Reset()
	Skill_Reset()
	QuickMenu_Reset()
	PMS_Reset()
	CharInfo_Reset()
	BasicChat_Reset()
	InteractMenu_Reset()
	GroupChat_Reset()
	ResetCommunity()
	RanMobile_Reset()
	
	if true == IsOpenWidget( WIDGET_ID_LOGIN ) then
	
		CloseWidget( WIDGET_ID_LOGIN )
		
		OpenWidget( WIDGET_ID_SERVERSELECT )
		
	else
	
		-- 로그 아웃 후 재접속시 로드되어 있는 UI에 이전 접속했던 캐릭터 데이터가 남아있을 수 있다.
		-- 모든 UI 제거하자.
		CloseAllWidget( true )
		
		OpenWidget( WIDGET_ID_MESSAGEBOX )
		OpenWidget( WIDGET_ID_SERVERSELECT )
		OpenWidget( WIDGET_ID_LETTERBOX )
		OpenWidget( WIDGET_ID_TITLE )
		OpenWidget( WIDGET_ID_FULLSCREEN )
		
	end
	
	SetStageBG( STAGE_BG_LOGIN )

end

-- 로그인 스테이지
function ToLoginStage()

	-- 게임 외부에선 단축키 처리를 안한다. 스테이지 키 이벤트로 등록하자.
	InsertStageKeyUp( Ev_MsgBox_KeyUp )

	MsgBox_Reset()
	Event_MsgBox_Close()
	ServerSelect_Reset()
	
	CloseWidget( WIDGET_ID_SERVERSELECT )
	CloseWidget( WIDGET_ID_SERVERCHANNELSELECT )
	
	OpenWidget( WIDGET_ID_LOGIN )
	
	SetStageBG( STAGE_BG_LOGIN )
	
end

-- 캐릭터 선택 스테이지
function ToCharacterSelectStage()

	-- 게임 외부에선 단축키 처리를 안한다. 스테이지 키 이벤트로 등록하자.
	InsertStageKeyUp( Ev_MsgBox_KeyUp )

	MsgBox_Reset()
	CloseAllWidget( false )
	if g_Client:IsWorldBattleStage() == false then
		OpenWidget( WIDGET_ID_LETTERBOX )
		OpenWidget( WIDGET_ID_MESSAGEBOX )
		OpenWidget( WIDGET_ID_CHARSELECT )
		OpenWidget( WIDGET_ID_FULLSCREEN )
		OpenWidget( WIDGET_ID_TITLE )
	
		if true == g_UserFeatures:IsUsingSelectCountry() then
			OpenWidget( WIDGET_ID_COUNTRYSELECT )
		end
	end
	
	SetStageBG( STAGE_BG_SELECT_CHAR )

end

-- 캐릭터 생성 스테이지
function ToCharacterCreateStage()

	-- 게임 외부에선 단축키 처리를 안한다. 스테이지 키 이벤트로 등록하자.
	InsertStageKeyUp( Ev_MsgBox_KeyUp )

	MsgBox_Reset()
	
	CloseAllWidget( false )
	OpenWidget( WIDGET_ID_LETTERBOX )
	OpenWidget( WIDGET_ID_MESSAGEBOX )
	OpenWidget( WIDGET_ID_CHARCREATE )
	OpenWidget( WIDGET_ID_FULLSCREEN )
	
	SetStageBG( STAGE_BG_CREATE_CHAR )
	
end

-- 게임 스테이지
function ToGameStage()

	-- 스테이지 키 이벤트로 삭제
	RemoveStageKeyUp( Ev_MsgBox_KeyUp )

	MsgBox_Reset()
	
	CloseAllWidget( false )

	OpenWidget( WIDGET_ID_MESSAGEBOX )
	OpenWidget( WIDGET_ID_REPORTMESSAGEBOX )
	OpenWidget( WIDGET_ID_SKILL_RESERVE )
	OpenWidget( WIDGET_ID_BASICINFO )
	OpenWidget( WIDGET_ID_ITEMQUICKSLOT )
	OpenWidget( WIDGET_ID_SKILLQUICKSLOT )
	OpenWidget( WIDGET_ID_BASICCHAT )
	OpenWidget( WIDGET_ID_MENU )
	OpenWidget( WIDGET_ID_MENUBUTTON )
	OpenWidget( WIDGET_ID_QUICKMENU )
	OpenWidget( WIDGET_ID_CURSOR_TARGET_INFO )
	OpenWidget( WIDGET_ID_QUEST_HELPER )
	OpenWidget( WIDGET_ID_BUFFNORMAL )
	OpenWidget( WIDGET_ID_BUFFLUNCHBOX )
	OpenWidget( WIDGET_ID_BUFFEXPLUNCHBOX )
	OpenWidget( WIDGET_ID_BUFFADD )
	OpenWidget( WIDGET_ID_BUFFCTF )
	OpenWidget( WIDGET_ID_BUFFSYSTEM )
	OpenWidget( WIDGET_ID_QBOX )
	OpenWidget( WIDGET_ID_MINIMAP )
	OpenWidget( WIDGET_ID_TIPMSG )
	OpenWidget( WIDGET_ID_SUMMONSTATE )
	OpenWidget( WIDGET_ID_EXTREMEWEAPON )
	
	if 	true == g_UserFeatures:IsUsingJoinWorldBattle() or 
		true == g_UserFeatures:IsUsingWorldBattle() then
		
		OpenWidget( WIDGET_ID_COUNTRYSERVERENTRANCE )
		
	end

	Party_Refresh()
	ExtremeWeapon_Check()
	
	SetStageBG( STAGE_BG_GAME )

end

-- NPC 관련 UI 닫기
function CloseAllNpc()

	if true == IsOpenNpcTalk() then
	
		CloseWidget( WIDGET_ID_NPCTALK )
	
	end
	
	if true == IsClubOpen() then
	
		CloseWidget( WIDGET_ID_CLUB )
		
	end
	
	if true == IsRebuildCardOpen() then
	
		CloseWidget( WIDGET_ID_REBUILD_CARD )
	
	end

end

-- 게임 스테이지 들어가기전 초기화해야될것들;
-- 캐릭터별 정보를 담고있는 루아 전역변수들을 초기화해줘야한다;
function Init_BeforeJoinToGameStage()

	g_tbParty_MemberList = {}
	g_Invitation = {}
	
end