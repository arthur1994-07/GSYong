
g_strAttendanceComplete_Info = ""

-- 로드 완료
function LoadAttendaceComplete()

	HitTestEnable( WIDGET_ID_ATTENDANCECOMPLETE, false, "" )

end

-- 오픈
function OpenAttendaceComplete()

	-- 단축키 시스템 메시지
	local strMsg = StringFormat( GetGameWordText( "ATTENDANCE_TOOLTIP_TEXT", 1 ),  g_tbKyeCodeString[ KEYCODE_Y ] )
	CF_PrintMsg_SystemMsg( strMsg, TEXT_COLOR_LIGHTSKYBLUE )
	
	-- 완료한 과제 정보
	SetString( WIDGET_ID_ATTENDANCECOMPLETE, "text", g_strAttendanceComplete_Info, "tfInfo" )
	
	-- 학원 이미지
	local nSchool = g_MyChar:GetSchool()
	local strFrameID = ""
	if 0 == nSchool then
		strFrameID = "SM"
	elseif 1 == nSchool then
		strFrameID = "HA"
	elseif 2 == nSchool then
		strFrameID = "BH"
	else
		strFrameID = "NONE"
	end
	CallFunction( WIDGET_ID_ATTENDANCECOMPLETE, "gotoAndStop", strFrameID, "mcSchool" )
	
	-- 타이머 설정
	local tbQuickSet = 
	{
		time = 5000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_AttendanceComplete_TimeOver"
	}
	SetTween( WIDGET_ID_ATTENDANCECOMPLETE, tbQuickSet, "", "", "" )
	
end

-------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 켜기
function AttendanceComplete_Show( strInfo )

	g_strAttendanceComplete_Info = strInfo
	
	OpenWidget( WIDGET_ID_ATTENDANCECOMPLETE )

end

-------------------------------------------------------------------------------------------
-- 이벤트

-- 시간 끝
function Ev_AttendanceComplete_TimeOver()

	CloseWidget( WIDGET_ID_ATTENDANCECOMPLETE )

end