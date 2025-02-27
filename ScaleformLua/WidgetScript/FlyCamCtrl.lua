
-- 저장할 키프레임 카메라 위치
g_tbFlyCamCtrl_SavePos = {}

-- 플라잉 카메라 컨트롤 로드 완료
function LoadFlyCamCtrl()

	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_WIDGET_CLOSE, "CloseFlyCamCtrl", "", "" )

	-- 카메라 이동/회전 속도 텍스트
	SetString( WIDGET_ID_FLYCAMCTRL, "text", GetGameWordText( "FLYCAMERACONTROL_SLOT_NAME", 0 ), "labelCamMove" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", GetGameWordText( "FLYCAMERACONTROL_SLOT_NAME", 1 ), "labelCamRot" )
	
	-- 뉴머릭 스텝퍼 리피트 시간 설정
	SetInt( WIDGET_ID_FLYCAMCTRL, "repeatInterval", 50, "numCamMove.nextBtn" )
	SetInt( WIDGET_ID_FLYCAMCTRL, "repeatInterval", 50, "numCamMove.prevBtn" )
	SetInt( WIDGET_ID_FLYCAMCTRL, "repeatInterval", 50, "numCamRot.nextBtn" )
	SetInt( WIDGET_ID_FLYCAMCTRL, "repeatInterval", 50, "numCamRot.prevBtn" )
	
	-- 뉴머릭 스텝퍼 이벤트
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Move", "", "numCamMove.nextBtn" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Move", "", "numCamMove.prevBtn" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Rot", "", "numCamRot.nextBtn" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Rot", "", "numCamRot.prevBtn" )
	
	-- 리스트 이벤트
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_LIST_INDEX_CHANGE, "Ev_FlyCamCtrl_SelectList", "index", "listKeyFrame" )
	
	-- 키 설정 패널 visible 끄기
	SetBool( WIDGET_ID_FLYCAMCTRL, "visible", false, "keyCfg" )
	-- 키 설정 패널 인풋 이벤트
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_KEY_UP, "Ev_FlyCamCtrl_inputPos", "keyCode", "keyCfg.inputPosX", "x" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_KEY_UP, "Ev_FlyCamCtrl_inputPos", "keyCode", "keyCfg.inputPosY", "y" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_KEY_UP, "Ev_FlyCamCtrl_inputPos", "keyCode", "keyCfg.inputPosZ", "z" )
	-- 무브 타입 콤보박스 설정
	local tbMoveString = g_FlyCamCtrl:GetMoveToTypeString()
	local str = ""
	
	for value in pairs( tbMoveString ) do
		str = str .. tbMoveString[ value ] .. ";"
	end
	
	SetData( WIDGET_ID_FLYCAMCTRL, str, "keyCfg.comboType" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_LIST_INDEX_CHANGE, "Ev_FlyCamCtrl_comboType", "index", "keyCfg.comboType" )
	
	-- 현재 위치 버튼 이벤트
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_btnCurPos", "", "keyCfg.btnCurPos" )
	-- 저장 버튼 이벤트
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_btnSave", "", "keyCfg.btnSave" )
	
	-- FPS
	local nFPS = g_FlyCamCtrl:GetFPS()
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_KEY_UP, "Ev_FlyCamCtrl_KeyUpFPS", "keyCode", "inputFps" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_FOCUS_OUT, "Ev_FlyCamCtrl_FocusOutFPS", "", "inputFps" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( nFPS ), "inputFps" )
	
	-- 현재 프레임
	local nCurrentFrame = g_FlyCamCtrl:GetCurrentFrame()
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_KEY_UP, "Ev_FlyCamCtrl_KeyUpKey", "keyCode", "inputKey" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_FOCUS_OUT, "Ev_FlyCamCtrl_FocusOutKey", "", "inputKey" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( nCurrentFrame ), "inputKey" )
	
	-- 전체 프레임
	local nTotalFrame = g_FlyCamCtrl:GetTotalFrame()
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_KEY_UP, "Ev_FlyCamCtrl_KeyUpTotal", "keyCode", "inputTotal" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_FOCUS_OUT, "Ev_FlyCamCtrl_FocusOutTotal", "", "inputTotal" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( nTotalFrame ), "inputTotal" )
	
	-- 버튼 이벤트
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Add", "", "btnAdd" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Delete", "", "btnDel" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Play", "", "btnPlay" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Stop", "", "btnStop" )
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Rewind", "", "btnRewind" )
	
	-- 도움말 버튼
	SetEvent( WIDGET_ID_FLYCAMCTRL, EVENT_BUTTON_CLICK, "Ev_FlyCamCtrl_Help", "", "btnHelp" )
	
	-- 시간 진행 바 최대값 설정
	SetInt( WIDGET_ID_FLYCAMCTRL, "maximum", nTotalFrame, "barProg" )

end

-- 오픈
function OpenFlyCamCtrl()

	InsertFrameMove( FrameMoveFlyCamCtrl )

	-- 카메라 이동 속도 설정
	local fValue = g_Client:GetFlyCamCtrlMoveVelo()
	FlyCamCtrl_SetMoveVelo( fValue )
	
	-- 카메라 회전 속도 설정
	fValue = g_Client:GetFlyCamCtrlRotVelo()
	FlyCamCtrl_SetRotateVelo( fValue )

end

-- 닫기
function CloseFlyCamCtrl()

	RemoveFrameMove( FrameMoveFlyCamCtrl )

end

-- 프레임 무브
function FrameMoveFlyCamCtrl( fElspTime )

	local tbPos = GetCameraPos()
	
	local strPos = string.format( "%.1f, %.1f, %.1f", tbPos[ "x" ], tbPos[ "y" ], tbPos[ "z" ] )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", strPos, "labelCurPos" )

end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 플라잉 카메라 에러 메시지
function FlyCamCtrl_MsgError( strError )

	OpenMsgBox( MSG_BOX_TYPE_OK, strError, Event_MsgBox_Close )

end

-- 플라잉 카메라 이동 속도 설정
function FlyCamCtrl_SetMoveVelo( fVelo )

	if false == IsOpenWidget( WIDGET_ID_FLYCAMCTRL ) then
		return
	end

	SetDouble( WIDGET_ID_FLYCAMCTRL, "value", fVelo, "numCamMove" )
	
end

-- 플라잉 카메라 회전 속도 설정
function FlyCamCtrl_SetRotateVelo( fVelo )

	if false == IsOpenWidget( WIDGET_ID_FLYCAMCTRL ) then
		return
	end
	
	SetDouble( WIDGET_ID_FLYCAMCTRL, "value", fVelo, "numCamRot" )
	
end

-- 플라잉 카메라 키프레임 추가
function FlyCamCtrl_AddKeyFrame( tbKeyFrame )

	local tbList = {}
	
	for value in pairs( tbKeyFrame ) do
	
		local tbData =
		{
			"KeyFrame " .. tostring( tbKeyFrame[ value ] ),
			tbKeyFrame[ value ]
		}
		
		table.insert( tbList, tbData )
	
	end
	
	SetDataEx( WIDGET_ID_FLYCAMCTRL, "label;key", tbList, "listKeyFrame", false )

end

-- 플라잉 카메라 키프레임 카메라 위치 가져오기
function FlyCamCtrl_GetKeyFramePos()

	if 3 ~= table.getn( g_tbFlyCamCtrl_SavePos ) then
		return { 0, 0, 0 }
	end
	
	return g_tbFlyCamCtrl_SavePos

end

-- 플라잉 카메라 프로그래스바 업데이트
function FlyCamCtrl_UpdateProgress( nCurrentKey )

	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( nCurrentKey ), "inputKey" )
	SetInt( WIDGET_ID_FLYCAMCTRL, "value", nCurrentKey, "barProg" )

end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 카메라 이동 속도 설정 이벤트
function Ev_FlyCamCtrl_Move()

	-- 카메라 이동 속도 설정
	local fValue = GetDouble( WIDGET_ID_FLYCAMCTRL, "value", "numCamMove" )
	g_Client:SetFlyCamCtrlMoveVelo( fValue )
	
end

-- 카메라 회전 속도 설정 이벤트 
function Ev_FlyCamCtrl_Rot()

	-- 카메라 회전 속도 설정
	local fValue = GetDouble( WIDGET_ID_FLYCAMCTRL, "value", "numCamRot" )
	g_Client:SetFlyCamCtrlRotVelo( fValue )

end

-- 리스트 선택
function Ev_FlyCamCtrl_SelectList( nIndex )

	if -1 == nIndex then
	
		SetBool( WIDGET_ID_FLYCAMCTRL, "visible", false, "keyCfg" )
		return
	
	end

	SetBool( WIDGET_ID_FLYCAMCTRL, "visible", true, "keyCfg" )
	
	local nKey = GetDataInt( WIDGET_ID_FLYCAMCTRL, nIndex, "key", "listKeyFrame" )
	
	
	-- 현재 프레임 설정
	g_FlyCamCtrl:SetCurrentFrame( nKey )
	
	local tbPos = g_FlyCamCtrl:GetKeyFramePos( nKey )
	
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tbPos[ "x" ], "keyCfg.inputPosX" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tbPos[ "y" ], "keyCfg.inputPosY" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tbPos[ "z" ], "keyCfg.inputPosZ" )
	
	SetInt( WIDGET_ID_FLYCAMCTRL, "value", nKey, "barProg" )	
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( nKey ), "inputKey" )

end

-- 키 설정 패널 위치 입력
function Ev_FlyCamCtrl_inputPos( nKeyCode, strInput )

	-- 포커스 이동 시키기
	if 	KEYCODE_ENTER == nKeyCode or
		KEYCODE_TAB == nKeyCode then
		
		if "x" == strInput then
		
			SetFocus( WIDGET_ID_FLYCAMCTRL, "keyCfg.inputPosY" )
		
		elseif "y" == strInput then
		
			SetFocus( WIDGET_ID_FLYCAMCTRL, "keyCfg.inputPosZ" )
		
		else
		
			SetFocus( WIDGET_ID_FLYCAMCTRL, "keyCfg.inputPosX" )
		
		end
		
	end

end

-- 무브 타입 변경
function Ev_FlyCamCtrl_comboType( nIndex )

	SetString( WIDGET_ID_FLYCAMCTRL, "text", "", "keyCfg.inputPosX" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", "", "keyCfg.inputPosY" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", "", "keyCfg.inputPosZ" )

end

-- 현재 위치 버튼 이벤트
function Ev_FlyCamCtrl_btnCurPos()

	local tbPos = GetCameraPos()
	
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( tbPos[ "x" ] ), "keyCfg.inputPosX" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( tbPos[ "y" ] ), "keyCfg.inputPosY" )
	SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( tbPos[ "z" ] ), "keyCfg.inputPosZ" )

end

-- 저장 버튼 이벤트
function Ev_FlyCamCtrl_btnSave()

	g_tbFlyCamCtrl_SavePos[ 1 ] = tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "keyCfg.inputPosX" ) )
	g_tbFlyCamCtrl_SavePos[ 2 ] = tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "keyCfg.inputPosY" ) )
	g_tbFlyCamCtrl_SavePos[ 3 ] = tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "keyCfg.inputPosZ" ) )
	
	g_FlyCamCtrl:SaveKeyFrame()
	
end

-- FPS 입력
function Ev_FlyCamCtrl_KeyUpFPS( nKeyCode )

	if KEYCODE_ENTER == nKeyCode then
	
		Ev_FlyCamCtrl_FocusOutFPS()
	
	end

end

-- FPS 포커스 아웃
function Ev_FlyCamCtrl_FocusOutFPS()

	-- FPS 설정
	local nFPS = math.floor( tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "inputFps" ) ) )
	g_FlyCamCtrl:SetFPS( nFPS )

end

-- 현재 프레임 입력
function Ev_FlyCamCtrl_KeyUpKey( nKeyCode )

	if KEYCODE_ENTER == nKeyCode then
	
		Ev_FlyCamCtrl_FocusOutKey()
	
	end

end

-- 현재 프레임 포커스 아웃
function Ev_FlyCamCtrl_FocusOutKey()

	local nTotal = math.floor( tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "inputTotal" ) ) )
	local nKey = math.floor( tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "inputKey" ) ) )
	
	-- 전체 프레임보다 클 수 없다.
	if nTotal < nKey then
	
		nKey = nTotal
		SetString( WIDGET_ID_FLYCAMCTRL, "text", tostring( nKey ), "inputKey" )
		
	end
	
	-- 현재 프레임 설정
	g_FlyCamCtrl:SetCurrentFrame( nKey )
	
	-- 시간 진행 바 설정
	SetInt( WIDGET_ID_FLYCAMCTRL, "value", nKey, "barProg" )
	
end

-- 전체 프레임 입력
function Ev_FlyCamCtrl_KeyUpTotal( nKeyCode )

	if KEYCODE_ENTER == nKeyCode then
	
		Ev_FlyCamCtrl_FocusOutTotal()
	
	end

end

-- 전체 프레임 포커스 아웃
function Ev_FlyCamCtrl_FocusOutTotal()

	-- 전체 프레임 설정
	local nTotal = math.floor( tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "inputTotal" ) ) )
	g_FlyCamCtrl:SetTotalFrame( nTotal )
	
	-- 시간 진행 바 최대값 설정
	SetInt( WIDGET_ID_FLYCAMCTRL, "maximum", nTotal, "barProg" )

end

-- 키프레임 추가
function Ev_FlyCamCtrl_Add()

	local nKey = math.floor( tonumber( GetString( WIDGET_ID_FLYCAMCTRL, "text", "inputKey" ) ) )
	g_FlyCamCtrl:AddKeyFrame( nKey )

end

-- 키프레임 삭제
function Ev_FlyCamCtrl_Delete()

	local nSelect = GetInt( WIDGET_ID_FLYCAMCTRL, "selectedIndex", "listKeyFrame" )
	-- 선택된게 없으면 끝내기
	if -1 == nSelect then
		return
	end
	
	local nKey = GetDataInt( WIDGET_ID_FLYCAMCTRL, nSelect, "key", "listKeyFrame" )
	g_FlyCamCtrl:DeleteKeyFrame( nKey )
	
	RemoveData( WIDGET_ID_FLYCAMCTRL, nSelect, "listKeyFrame", false )

end

-- 플레이
function Ev_FlyCamCtrl_Play()

	g_FlyCamCtrl:Play()

end

-- 스탑
function Ev_FlyCamCtrl_Stop()

	g_FlyCamCtrl:Stop()

end

-- 리와인드
function Ev_FlyCamCtrl_Rewind()

	g_FlyCamCtrl:Rewind()

end

-- 도움말 버튼
function Ev_FlyCamCtrl_Help()

	local str = 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 0 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_MOVE_FRONT ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_MOVE_LEFT ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_MOVE_BACK ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_MOVE_RIGHT ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 1 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_UP ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_DOWN ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_LEFT ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_RIGHT ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 2 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 3 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 4 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_CLEAR_INTERPOLATION_TIME ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 5 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 6 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 7 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 8 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_PLAY ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 9 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_STOP ) ) .. "\n" .. 
		StringFormat( GetGameWordText( "FLYCAMERAWINDOW_HELPTEXT", 10 ),
			g_InputMan:GetShortcutsToString( EMSHORTCUTS_CAMERA_REWIND_PLAY ) )

	HelpTooltip_SetText( str )

end