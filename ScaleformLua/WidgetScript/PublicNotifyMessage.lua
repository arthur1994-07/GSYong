
-- WIDGET_ID_PUBLIC_NOTIFY_MESSAGE;

g_strPNM_Msg = ""
g_nPNM_FontColor = 0
g_nPNM_FontSize = 0
g_nPNM_Time = 0

function LoadPublicNotifyMessage()

	HitTestEnable( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, false, "" )
	
	InitPublicNotifyMessage()
	
end

function InitPublicNotifyMessage()

	if 0 == g_nPNM_FontSize then
		g_nPNM_FontSize = 20
	end
	
	-- 배경 width 간격
	local nWidthGap = GetInt( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "width", "bg" ) - 
		GetInt( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "width", "tfMessage" )

	-- 텍스트 설정 ------------------------------
	local str = HTMLFontColorSize( g_strPNM_Msg, TranslateHTMLColor( g_nPNM_FontColor ), g_nPNM_FontSize )
	SetString( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "htmlText", str, "tfMessage" )
	
	-- 사이즈 잡기 ------------------------------
	local nTextWidth = GetInt( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "textWidth", "tfMessage" ) + 4
	local nBgWidth = nTextWidth + nWidthGap
	
	SetInt( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "width", nTextWidth, "tfMessage" )
	SetInt( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "width", nBgWidth, "bg" )
	
	-- 화면 가운데로 정렬 ------------------------
	local nNewX = (GetScreenWidth() - nBgWidth) / 2
	SetInt( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, "x", nNewX, "" )

	-- 출력 시간 설정 ----------------------------
	local tableTimer = 
	{
		time = g_nPNM_Time,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "EventPublicNotifyMessage"
	}
	SetTween( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE, tableTimer, "", "", "" )	

end

function EventPublicNotifyMessage()

	CloseWidget( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE )
	
end

function TogglePublicNotifyMessage(_stringOut, _colorFont, _sizeFont, _time)

	g_strPNM_Msg = _stringOut
	g_nPNM_FontColor = _colorFont
	g_nPNM_FontSize = _sizeFont
	g_nPNM_Time = _time * 1000
	
	if 0 == g_nPNM_Time then
		g_nPNM_Time = 10000
	end
	
	if true == IsOpenWidget( WIDGET_ID_PUBLIC_NOTIFY_MESSAGE ) then
	
		InitPublicNotifyMessage()
	
	else
	
		OpenWidget(WIDGET_ID_PUBLIC_NOTIFY_MESSAGE)	
	
	end
	
end