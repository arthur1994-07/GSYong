
CTF_START = 0				-- 오렌지
CTF_CAPTURED_BY_SM = 1		-- 노랑
CTF_CAPTURED_BY_HA = 2		-- 파랑
CTF_CAPTURED_BY_BH = 3		-- 노랑
CTF_STOP = 4				-- 노랑
CTF_READY = 5

g_nCTFNotify_Type = 0
g_strCTFNotify_Msg = ""

-- 학원별 인증 횟수; 1-성문, 2-현암, 3-봉황
g_nAuthenticatorCount = {}

-- 배경 height 간격
g_nCTFNotify_HeightGap = 0

function LoadCTFNotify()

	HitTestEnable( WIDGET_ID_CTF_NOTIFY_DISPLAY, false, "" )

end

function OpenCTFNotify()

	CTFNotify_SetInfo()
	
end

function CTFNotify_SetInfo()

	if 	CTF_START == g_nCTFNotify_Type or
		CTF_STOP == g_nCTFNotify_Type then
	
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", true, "mcStart" )
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", false, "mcCapture" )
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", false, "mcMsg" )
		
		local str =  ""
		if 	CTF_START == g_nCTFNotify_Type then
		
			str = HTMLFontColor( g_strCTFNotify_Msg, HTML_TEXT_COLOR_ORANGE )
		
		elseif CTF_STOP == g_nCTFNotify_Type then
		
			str = HTMLFontColor( g_strCTFNotify_Msg, HTML_TEXT_COLOR_YELLOW )
		
		end
		SetString( WIDGET_ID_CTF_NOTIFY_DISPLAY, "htmlText", str, "mcStart.tf" )
	
	elseif 	CTF_CAPTURED_BY_SM == g_nCTFNotify_Type or
			CTF_CAPTURED_BY_HA == g_nCTFNotify_Type or
			CTF_CAPTURED_BY_BH == g_nCTFNotify_Type then
			
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", false, "mcStart" )
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", true, "mcCapture" )
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", false, "mcMsg" )
		
		local str =  ""
		if 	CTF_CAPTURED_BY_SM == g_nCTFNotify_Type then
		
			str = HTMLFontColor( g_strCTFNotify_Msg, HTML_TEXT_COLOR_YELLOW )
			CallFunction( WIDGET_ID_CTF_NOTIFY_DISPLAY, "gotoAndStop", "SM", "mcCapture.mcSchool" )
		
		elseif CTF_CAPTURED_BY_HA == g_nCTFNotify_Type then
		
			str = HTMLFontColor( g_strCTFNotify_Msg, HTML_TEXT_COLOR_BLUE )
			CallFunction( WIDGET_ID_CTF_NOTIFY_DISPLAY, "gotoAndStop", "HA", "mcCapture.mcSchool" )
			
		elseif CTF_CAPTURED_BY_BH == g_nCTFNotify_Type then
		
			str = HTMLFontColor( g_strCTFNotify_Msg, HTML_TEXT_COLOR_YELLOW )
			CallFunction( WIDGET_ID_CTF_NOTIFY_DISPLAY, "gotoAndStop", "BH", "mcCapture.mcSchool" )
		
		end
		SetString( WIDGET_ID_CTF_NOTIFY_DISPLAY, "htmlText", str, "mcCapture.tf" )
		
	elseif CTF_READY == g_nCTFNotify_Type then
	
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", false, "mcStart" )
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", false, "mcCapture" )
		SetBool( WIDGET_ID_CTF_NOTIFY_DISPLAY, "visible", true, "mcMsg" )
		
		SetString( WIDGET_ID_CTF_NOTIFY_DISPLAY, "text", g_strCTFNotify_Msg, "mcMsg.tf" )
		
		-- 배경 간격
		local nGap = GetInt( WIDGET_ID_CTF_NOTIFY_DISPLAY, "height", "mcMsg.bg" ) - 
			GetInt( WIDGET_ID_CTF_NOTIFY_DISPLAY, "height", "mcMsg.tf" )
	
		local nTextHeight = GetInt( WIDGET_ID_CTF_NOTIFY_DISPLAY, "textHeight", "mcMsg.tf" ) + 4
		SetInt( WIDGET_ID_CTF_NOTIFY_DISPLAY, "height", nTextHeight, "mcMsg.tf" )
		SetInt( WIDGET_ID_CTF_NOTIFY_DISPLAY, "height", nTextHeight + nGap, "mcMsg.bg" )
		
	end
	
	SetInt( WIDGET_ID_CTF_NOTIFY_DISPLAY, "y", GetScreenHeight() * 0.1, "" )

	-- 알림 꺼줄 타이머 초기화;
	local tbQuickSet = 
	{
		time = 5000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "EventCall_TimeOut_CTFNotify"
	}
	SetTween( WIDGET_ID_CTF_NOTIFY_DISPLAY, tbQuickSet, "", "", "" )

end

----------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 점령, 입장
function CTFAuthCountInit( nSMAuthCnt, nHAAuthCnt, nBHAuthCnt )

	g_nAuthenticatorCount[ 1 ] = nSMAuthCnt
	g_nAuthenticatorCount[ 2 ] = nHAAuthCnt
	g_nAuthenticatorCount[ 3 ] = nBHAuthCnt
	
end

-- 점령
function DisplayProgressNotify( nProgress )

	g_nCTFNotify_Type = nProgress
	g_strCTFNotify_Msg = GetGameWordText( "CTF_PROGRESS_TEXT", g_nCTFNotify_Type )
	
	if false == IsOpenWidget( WIDGET_ID_CTF_NOTIFY_DISPLAY ) then
	
		OpenWidget( WIDGET_ID_CTF_NOTIFY_DISPLAY )
		
	else
	
		CTFNotify_SetInfo()
		
	end
	
	-- 학원 범위
	if  nProgress >= CTF_CAPTURED_BY_SM and
		nProgress <= CTF_CAPTURED_BY_BH then
		
		local nCountCheck = g_nSchoolNum - 1
		if g_nAuthenticatorCount[ nProgress ] == nCountCheck then
		
			if false == IsOpenWidget( WIDGET_ID_CTF_SPURT_NOTIFY ) then
			
				SetWinSpurtSchool( nProgress )
				OpenWidget( WIDGET_ID_CTF_SPURT_NOTIFY )
				
			end
			
		end
		
	end	
	
end

-- 대기 타입은 따로 처리한다, 출력 위치, 출력방식이 다르다;
function DisplayProgressNotify_Ready( nRemainMinutes )

	g_nCTFNotify_Type = CTF_READY
	g_strCTFNotify_Msg = StringFormat( GetGameWordText( "CTF_PROGRESS_TEXT", g_nCTFNotify_Type ), nRemainMinutes )
	
	if false == IsOpenWidget( WIDGET_ID_CTF_NOTIFY_DISPLAY ) then
	
		OpenWidget( WIDGET_ID_CTF_NOTIFY_DISPLAY )
		
	else
	
		CTFNotify_SetInfo()
		
	end
	
end

----------------------------------------------------------------------------------------------
-- 이벤트

function EventCall_TimeOut_CTFNotify()

	CloseWidget( WIDGET_ID_CTF_NOTIFY_DISPLAY )
	
end