
-- 이동할 맵 이름
g_strGateMsg_MapName = ""

g_bGateMsg_Close = false

-- 게이트 이동 로드 완료
function LoadGateMsg()

	SetEvent( WIDGET_ID_GATEMSG, EVENT_WIDGET_CLOSE, "CloseGateMsg", "", "" )

	SetEvent( WIDGET_ID_GATEMSG, EVENT_KEY_UP, "GateMsg_KeyUp_Space", "keyCode;", "" )
	SetEvent( WIDGET_ID_GATEMSG, EVENT_BUTTON_CLICK, "Ev_GateMsg_OK", "", "gate.btnOK" )
	SetString( WIDGET_ID_GATEMSG, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "gate.btnOK" )
	
end

-- 게이트 이동 오픈
function OpenGateMsg()

	if true == g_bGateMsg_Close then
	
		g_bGateMsg_Close = false
		CloseWidget( WIDGET_ID_GATEMSG )
		return
	
	end

	local str = g_strGateMsg_MapName .. GetGameInternalText( "MAPMOVE_MESSAGE", 0 )
	SetString( WIDGET_ID_GATEMSG, "text", str, "gate.tfText" )
	SetFocus( WIDGET_ID_GATEMSG, "" )
	
	InsertFrameMove( GateMsg_FrameMove )
	
end

function CloseGateMsg()
	
	g_bGateMsg_Close = false
	g_strGateMsg_MapName = ""

end

function GateMsg_FrameMove( dt )

	if STAGE_BG_GAME ~= GetStageBG() or 0 == g_Client:GetGateOpen() then
		
		RemoveFrameMove( GateMsg_FrameMove )
		CloseWidget( WIDGET_ID_GATEMSG )
		
	end

end

----------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 게이트 오픈
function GateMsg_Open( strMapName )

	if g_strGateMsg_MapName == strMapName then
		return
	end
	
	g_strGateMsg_MapName = strMapName
	g_bGateMsg_Close = false

	if false == IsOpenWidget( WIDGET_ID_GATEMSG ) then
	
		OpenWidget( WIDGET_ID_GATEMSG )
		
	else
	
		OpenGateMsg()
	
	end
	
end

-- 게이트 닫기
function GateMsg_Close()

	g_bGateMsg_Close = true
	CloseWidget( WIDGET_ID_GATEMSG )

end

----------------------------------------------------------------------------------------------
-- 이벤트

-- 게이트 이동 확인
function Ev_GateMsg_OK()

	if true == IsOpenWidget( WIDGET_ID_PRIVATE_MARKET_SEARCH ) then
		return
	end

	g_Msg:ReqGateOut( 0 )
	CloseWidget( WIDGET_ID_GATEMSG )

end


-- 메시지 박스 스페이스바 단축키
function GateMsg_KeyUp_Space( keyCode )

	if false == IsOpenWidget( WIDGET_ID_GATEMSG ) then
		return
	end
	
	if false == GetBool( WIDGET_ID_GATEMSG, "visible", "" ) then
	
		return false
		
	else
	
		if KEYCODE_SPACE == keyCode then
			-- 게이트 이동 이벤트 호출
			Ev_GateMsg_OK()
		end
		
	end

end