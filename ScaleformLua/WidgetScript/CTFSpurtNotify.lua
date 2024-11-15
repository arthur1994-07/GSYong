
-- add text
-- > gameword:CTF_PROGRESS_SPURT_TEXT
nWinspurtSchool = 0

-- 배경 width 간격
g_nCTFSpurtNotify_WidthGap = 0

function LoadCTFSpurtNotify()

	-- 배경 width 간격
	g_nCTFSpurtNotify_WidthGap = GetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "width", "info.bg" ) - 
		GetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "width", "info.textField" )

end

-- 3개의 화력제어기중 2개를 한학원이 점령하게 되면 출력되는 알림;
-- (본인 학원)점령전 승리 임박 강조, (기타 학원)점령전 승리 학원 저지;
function OpenCTFSpurtNotify()

	-- 텍스트 설정 --------------------------------------------
	local str = ""
	if g_MyChar:GetSchool() + 1 == nWinspurtSchool then
	
		CallFunction( WIDGET_ID_CTF_SPURT_NOTIFY, "gotoAndPlay", "CTF_SPURT_NOTIFY", "" )
		
		str = GetGameWordText( "CTF_PROGRESS_SPURT_TEXT", 0 )

	else
	
		CallFunction( WIDGET_ID_CTF_SPURT_NOTIFY, "gotoAndStop", "CTF_WARNING_NOTIFY", "" )
		
		str = HTMLFontColor( GetGameWordText( "CTF_RESULT_BUTTON", nWinspurtSchool ) .. " ", HTML_TEXT_COLOR_BLUE )
		str = str .. HTMLFontColor( GetGameWordText( "CTF_PROGRESS_SPURT_TEXT", 1 ), HTML_TEXT_COLOR_RED )
		
	end
	SetString( WIDGET_ID_CTF_SPURT_NOTIFY, "htmlText", str, "info.textField" )
	
	-- 사이즈 설정 --------------------------------------------
	local nTextWidth = GetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "textWidth", "info.textField" ) + 4
	local nBgWidth = nTextWidth + g_nCTFSpurtNotify_WidthGap
	
	SetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "width", nTextWidth, "info.textField" )
	SetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "width", nBgWidth, "info.bg" )
	
	-- 위치 설정 ----------------------------------------------
	SetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "y", GetScreenHeight() * 0.1, "" )
	
	local nNewX = (GetScreenWidth() - nBgWidth) / 2
	SetInt( WIDGET_ID_CTF_SPURT_NOTIFY, "x", nNewX, "" )
	
	-- 타이머 설정
	InitSpurtNotifyTimer()
	
end

function test()

	--SetWinSpurtSchool( 1 )
	OpenWidget( WIDGET_ID_CTF_SPURT_NOTIFY )

end

function InitSpurtNotifyTimer()

	local tbQuickSet = 
	{
		time = 7000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "EventCall_TimeOut_CTFSpurt"
	}
	SetTween( WIDGET_ID_CTF_SPURT_NOTIFY, tbQuickSet, "", "", "" )
	
end

function EventCall_TimeOut_CTFSpurt()

	CloseWidget( WIDGET_ID_CTF_SPURT_NOTIFY )
	
end

function SetWinSpurtSchool( nSchool )

	nWinspurtSchool = nSchool
	
end
