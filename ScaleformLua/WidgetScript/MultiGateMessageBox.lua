
-- 게이트 테이블
g_tbMultiGateMsg_MapName = {}

g_bMultiGateMsg_Close = false

-- 멀티 게이트 메시지 로드 완료
function LoadMultiGateMsg()

	SetEvent( WIDGET_ID_MULTIGATEMSG, EVENT_WIDGET_CLOSE, "CloseMultiGateMsg", "", "" )

	-- 확인
	SetString( WIDGET_ID_MULTIGATEMSG, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "gate.btnOK" )
	SetEvent( WIDGET_ID_MULTIGATEMSG, EVENT_BUTTON_CLICK, "Ev_MultiGateMsg_OK", "", "gate.btnOK" )
	
	-- 팁
	SetString( WIDGET_ID_MULTIGATEMSG, "text", GetGameInternalText( "MULTI_GATE_INFORMATIOIN_TEXT", 0 ), "gate.labelTip" )
	
end

-- 멀티 게이트 메시지 오픈
function OpenMultiGateMsg()	

	if true == g_bMultiGateMsg_Close then
	
		g_bMultiGateMsg_Close = false
		CloseWidget( WIDGET_ID_MULTIGATEMSG )
		return
	
	end

	InsertStageKeyUp( Ev_MultiGateMsg_KeyUp )

	local strParam = CreateParamForTable( g_tbMultiGateMsg_MapName )
	SetData( WIDGET_ID_MULTIGATEMSG, strParam, "gate.listMapName" )
	SetInt( WIDGET_ID_MULTIGATEMSG, "selectedIndex", 0, "gate.listMapName" )
	
	InsertFrameMove( MultiGateMsg_FrameMove )

end

function CloseMultiGateMsg()

	g_bMultiGateMsg_Close = false
	RemoveStageKeyUp( Ev_MultiGateMsg_KeyUp )

end

function MultiGateMsg_FrameMove( dt )

	if STAGE_BG_GAME ~= GetStageBG() or 0 == g_Client:GetGateOpen() then
		
		RemoveFrameMove( MultiGateMsg_FrameMove )
		CloseWidget( WIDGET_ID_MULTIGATEMSG )
		
	end

end

----------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 게이트 오픈
function MultiGateMsg_Open( tbMapName )

	g_tbMultiGateMsg_MapName = tbMapName
	g_bMultiGateMsg_Close = false

	if true == IsOpenWidget( WIDGET_ID_MULTIGATEMSG ) then
		return
	end
	
	OpenWidget( WIDGET_ID_MULTIGATEMSG )

end

-- 게이트 닫기
function MultiGateMsg_Close()

	g_bMultiGateMsg_Close = true
	CloseWidget( WIDGET_ID_MULTIGATEMSG )

end

----------------------------------------------------------------------------------------------
-- 이벤트

-- 게이트 이동 확인
function Ev_MultiGateMsg_OK()

	if true == IsOpenWidget( WIDGET_ID_PRIVATE_MARKET_SEARCH ) then
		return
	end

	local nIndex = GetInt( WIDGET_ID_MULTIGATEMSG, "selectedIndex", "gate.listMapName" )
	if -1 == nIndex then
		return
	end
	
	g_Msg:ReqGateOut( nIndex )

end

-- 메시지 박스 스페이스바 단축키
function Ev_MultiGateMsg_KeyUp( keyCode )

	if false == IsOpenWidget( WIDGET_ID_MULTIGATEMSG ) then
		return
	end
	
	if KEYCODE_SPACE == keyCode then
	
		Ev_MultiGateMsg_OK()
		
	end

end