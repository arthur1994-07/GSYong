
-- 타이머
g_fCTFlag_Time = 0

g_strCTFlag_GoalScore = ""
g_nCTFlag_GoalScoreColor = 0xFFFFFFFF

-- 팀 점수
g_nRedTeam_Score = 0
g_nBlueTeam_Score = 0

-- 깃발 이미지
g_strRedTeamInst = "ctf_flag_01"
g_strRedTeamFrame = "TeamRed"
g_strRedTeamFunc = "gotoAndStop"

g_strBlueTeamInst = "ctf_flag_02"
g_strBlueTeamFrame = "TeamBlue"
g_strBlueTeamFunc = "gotoAndStop"

-- 드래그 관련 변수
g_nCTFlagDB_MousePosX = 0
g_nCTFlagDB_MousePosY = 0
g_nCTFlagDB_XPos = 0
g_nCTFlagDB_YPos = 0

g_fCTFlag_BgAlpha = 0.0

-- 깃발전 현황판 로드 완료
function LoadCTFlagDashboard()

	-- 드래그 이벤트 설정
	SetEvent( WIDGET_ID_CTFLAG_DASHBOARD, EVENT_MOUSE_DOWN, "Ev_CTFlagDB_StartDrag", "stageX;stageY;", "mcBG" )

	SetEvent( WIDGET_ID_CTFLAG_DASHBOARD, EVENT_MOUSE_ROLLOVER, "Ev_CTFlagDB_BgRollOver", "", "mcBG" )
	SetEvent( WIDGET_ID_CTFLAG_DASHBOARD, EVENT_MOUSE_ROLLOUT, "Ev_CTFlagDB_BgRollOut", "", "mcBG" )
	
	g_fCTFlag_BgAlpha = GetDouble( WIDGET_ID_CTFLAG_DASHBOARD, "alpha", "mcBG" )
	SetDouble( WIDGET_ID_CTFLAG_DASHBOARD, "alpha", 0.0, "mcBG" )
	
	HitTestEnable( WIDGET_ID_CTFLAG_DASHBOARD, false, "ctf_flag_01" )
	HitTestEnable( WIDGET_ID_CTFLAG_DASHBOARD, false, "ctf_flag_02" )
	HitTestEnable( WIDGET_ID_CTFLAG_DASHBOARD, false, "db" )
	
end

-- 오픈
function OpenCTFlagDashboard()

	-- 텍스트 초기화 
	-- EventTimer에 의해서 UIShow 이전에 SetText를 통해서 세팅이 되는 부분이 있으나
	-- UI가 로드되어있지않다면 건너뛰어짐으로 다시한번 요청한다
	g_Client:TournamentTeamTextInfoRecall()
	SetString( WIDGET_ID_CTFLAG_DASHBOARD, "text", tostring( g_nRedTeam_Score ), "db.red.tfScore" )
	
	SetString( WIDGET_ID_CTFLAG_DASHBOARD, "text", tostring( g_nBlueTeam_Score ), "db.blue.tfScore" )
	
	-- 깃발 이미지 출력
	CallFunction( WIDGET_ID_CTFLAG_DASHBOARD, g_strRedTeamFunc, g_strRedTeamFrame, g_strRedTeamInst )		-- RED팀
	CallFunction( WIDGET_ID_CTFLAG_DASHBOARD, g_strBlueTeamFunc, g_strBlueTeamFrame, g_strBlueTeamInst )	-- BLUE팀
	
	CTFlag_SetTimer()
	
	CTFlag_SetObjectText( g_strCTFlag_GoalScore, g_nCTFlag_GoalScoreColor )
	
end

function CloseCTFlagDashboard ()

	g_nRedTeam_Score = 0
	g_nBlueTeam_Score = 0
	
	g_strRedTeamInst = "ctf_flag_01"
	g_strRedTeamFrame = "TeamRed"
	g_strRedTeamFunc = "gotoAndStop"

	g_strBlueTeamInst = "ctf_flag_02"
	g_strBlueTeamFrame = "TeamBlue"
	g_strBlueTeamFunc = "gotoAndStop"

end

-- 타이머 설정
function CTFlag_SetTimer()

	if 0 >= g_fCTFlag_Time then
	
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "text", "00:00", "db.tfTimer" )	
		return
		
	else
	
		local nMin = math.floor( g_fCTFlag_Time / 60 ) 
		local nSec = g_fCTFlag_Time % 60
		
		local str = string.format( "%02d:%02d", nMin, nSec )
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "text", str, "db.tfTimer" )
		
	end
	
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_CTFlag_Timer"
	}
	SetTween( WIDGET_ID_CTFLAG_DASHBOARD, tbQuickSet, "", "", "" )

end

---------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 타이머 설정
function CTFlag_SetTime( fTime )

	g_fCTFlag_Time = fTime
	
	if false == IsOpenWidget( WIDGET_ID_CTFLAG_DASHBOARD ) then
		return
	end
	
	CTFlag_SetTimer()

end

-- 깃발 소유자 이름 설정
function CTFlag_SetName( bLeft, strName, dwColor )
	local str = HTMLFontColor( strName, TranslateHTMLColor( dwColor ) )
	
	if true == bLeft then
	
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "htmlText", str, "db.red.tfName" )
	
	else
	
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "htmlText", str, "db.blue.tfName" )
	
	end

end

-- 점수 설정
function CTFlag_SetScore( bLeft, nScore, dwColor )

	local str = HTMLFontColor( tostring( nScore ), TranslateHTMLColor( dwColor ) )
	
	if true == bLeft then
	
		g_nRedTeam_Score = nScore
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "htmlText", str, "db.red.tfScore" )
		
	else
	
		g_nBlueTeam_Score = nScore
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "htmlText", str, "db.blue.tfScore" )
	
	end

end

-- 깃발 이미지 설정
function CTFlag_SetImage( bLeft, bBlink )

	if true == bLeft then
	
		g_strRedTeamInst = "ctf_flag_01"
	
		if true == bBlink then
		
			g_strRedTeamFrame = "UsurpRedFlag"
			g_strRedTeamFunc = "gotoAndPlay"
		
		else
		
			g_strRedTeamFrame = "TeamRed"
			g_strRedTeamFunc = "gotoAndStop"
			
		end
		
		CallFunction( WIDGET_ID_CTFLAG_DASHBOARD, g_strRedTeamFunc, g_strRedTeamFrame, g_strRedTeamInst )
	
	else
	
		g_strBlueTeamInst = "ctf_flag_02"
	
		if true == bBlink then
		
			g_strBlueTeamFrame = "UsurpBlueFlag"
			g_strBlueTeamFunc = "gotoAndPlay"
		
		else
		
			g_strBlueTeamFrame = "TeamBlue"
			g_strBlueTeamFunc = "gotoAndStop"
		
		end
		
		CallFunction( WIDGET_ID_CTFLAG_DASHBOARD, g_strBlueTeamFunc, g_strBlueTeamFrame, g_strBlueTeamInst )
	
	end

end

-- 목표점수
function CTFlag_SetObjectText( strText, nColor )

	g_strCTFlag_GoalScore = strText
	g_nCTFlag_GoalScoreColor = nColor

	if true == IsOpenWidget( WIDGET_ID_CTFLAG_DASHBOARD ) then
	
		local str = HTMLFontColor( strText, TranslateHTMLColor( nColor ) )
		SetString( WIDGET_ID_CTFLAG_DASHBOARD, "htmlText", str, "db.labelGoalScore" )
	
	end

end

---------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 이벤트
function Ev_CTFlag_Timer()

	g_fCTFlag_Time = g_fCTFlag_Time - 1
	
	CTFlag_SetTimer()

end

-- 배경 롤오버
function Ev_CTFlagDB_BgRollOver()

	SetDouble( WIDGET_ID_CTFLAG_DASHBOARD, "alpha", g_fCTFlag_BgAlpha, "mcBG" )

end

-- 배경 롤아웃
function Ev_CTFlagDB_BgRollOut()

	SetDouble( WIDGET_ID_CTFLAG_DASHBOARD, "alpha", 0.0, "mcBG" )

end

-- 드래그 시작
function Ev_CTFlagDB_StartDrag( nStageX, nStageY )

	InsertStageMouseMove( Ev_CTFlagDB_MoveDrag )
	InsertStageMouseUp( Ev_CTFlagDB_EndDrag )
	
	g_nCTFlagDB_MousePosX = nStageX
	g_nCTFlagDB_MousePosY = nStageY
	
	g_nCTFlagDB_XPos = GetInt( WIDGET_ID_CTFLAG_DASHBOARD, "x", "" )
	g_nCTFlagDB_YPos = GetInt( WIDGET_ID_CTFLAG_DASHBOARD, "y", "" )

end

-- 드래그 이동
function Ev_CTFlagDB_MoveDrag( nStageX, nStageY )

	local nOffsetX = nStageX - g_nCTFlagDB_MousePosX
	local nOffsetY = nStageY - g_nCTFlagDB_MousePosY
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_CTFLAG_DASHBOARD, 
		g_nCTFlagDB_XPos + nOffsetX, g_nCTFlagDB_YPos + nOffsetY, 
		0, 0, 0, 0 )
	
	SetInt( WIDGET_ID_CTFLAG_DASHBOARD, "x", nNewX, "" )
	SetInt( WIDGET_ID_CTFLAG_DASHBOARD, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_CTFlagDB_EndDrag( nStageX, nStageY )

	RemoveStageMouseMove( Ev_CTFlagDB_MoveDrag )
	RemoveStageMouseUp( Ev_CTFlagDB_EndDrag )

end