
g_funcFR_TimerEvent = nil		-- 타이머 끝나고 호출될 함수
g_fRessurectionTime = 0

-- 기존의 메시지 박스를 이용해서 뿌려준다;
function LoadForceResurrection()
end

function OpenForceResurrection()
	FR_SetTimer( g_fRessurectionTime )
end

function OpenForceResurrection_SetTime(fTime)
	if false == IsOpenWidget( WIDGET_ID_FORCERESSURECTION ) then
		g_fRessurectionTime = fTime
		OpenWidget( WIDGET_ID_FORCERESSURECTION )
	else
		FR_SetTimer( fTime )
	end
end

-- 강제부활상태에서의 스킬부활
function OpenForceResurrection_Skill()

	OpenMsgBox(
		MSG_BOX_TYPE_OKCANCEL,
		GetGameInternalText( "ForceRebirthSkill", 0 ),
		Ev_Resurrection_ForceAndSkill,
		Event_MsgBox_Close )
		
end

-- 타이머 설정
function FR_SetTimer( fTime )

	if nil == fTime or "number" ~= type( fTime ) then
		g_fRessurectionTime = 0
		return
	else
		g_fRessurectionTime = fTime
	end
	
	-- 타이머 돌리기
	if 0 < g_fRessurectionTime then
	
		-- 프로그래스 바 보이기
		SetBool( WIDGET_ID_FORCERESSURECTION, "visible", true, "MB_progressBar" )
		
		SetInt( WIDGET_ID_FORCERESSURECTION, "value", 100, "MB_progressBar.progressbar" )
		
		local tbQuickSet = 
		{
			time = g_fRessurectionTime * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_FORCERESSURECTION, tbQuickSet, "value", "0", "MB_progressBar.progressbar" )
		
		-- 타이머 텍스트 설정
		local strTime = tostring( math.ceil( g_fRessurectionTime ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "MB_progressBar.tfTimer" )
		
		strTime = StringFormat( GetGameInternalText( "NOTIFY_FORCE_REBIRTH" ), math.ceil( g_fRessurectionTime ) )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "textField" )
		-- 초당 타이머 설정
		FR_SetSecondTimer()
		
	-- 타이머 없음
	else	
		-- 프로그래스 숨기기
		SetBool( WIDGET_ID_FORCERESSURECTION, "visible", false, "MB_progressBar" )
		
	end	
end

-- 초당 타이머 설정
function FR_SetSecondTimer()

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_FR_SecTimer"
	}
	SetTween( WIDGET_ID_FORCERESSURECTION, tbQuickSet, "", "", "" )

end

function Ev_FR_SecTimer()
	-- 1초 빼기
	g_fRessurectionTime = g_fRessurectionTime - 1

	if 0 <= g_fRessurectionTime then
	
		local strTime = tostring( math.ceil( g_fRessurectionTime ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "MB_progressBar.tfTimer" )
		
		strTime = StringFormat( GetGameInternalText( "NOTIFY_FORCE_REBIRTH" ), math.ceil( g_fRessurectionTime ) )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "textField" )
		
		FR_SetSecondTimer()
	
	else
	
		g_fRessurectionTime = 0
		CloseWidget(WIDGET_ID_FORCERESSURECTION)
	
		-- 타이머 이벤트가 등록되어 있다면 해당 이벤트 호출
		if nil ~= g_funcFR_TimerEvent then
			g_funcFR_TimerEvent()
		end
	
	end
end

-- 강제부활 시 스킬부활일때의 이벤트
function Ev_Resurrection_ForceAndSkill()

	g_Msg:ResurrectSkill()
	CloseWidget( WIDGET_ID_FORCERESSURECTION )
	
	Event_MsgBox_Close()
	
end