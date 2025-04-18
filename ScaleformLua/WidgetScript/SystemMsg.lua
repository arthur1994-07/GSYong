
-- 메시지
g_strSystemMsg_Text = ""
-- 텍스트 컬러
g_nSystemMsg_Color = 0

-- 최소 텍스트 height
g_nSystemMsg_MinHeight = 0
-- 배경 height 간격
g_nSystemMsg_HeightGap = 0

-- 시스템 메시지 UI 로드 완료
function LoadSystemMsg()

	HitTestEnable( WIDGET_ID_SYSTEMMSG, false, "" )

	-- 최소 height
	g_nSystemMsg_MinHeight = GetInt( WIDGET_ID_SYSTEMMSG, "height", "text" )
	
	-- 배경 height 간격
	g_nSystemMsg_HeightGap = GetInt( WIDGET_ID_SYSTEMMSG, "height", "bg" ) - g_nSystemMsg_MinHeight

end

function OpenSystemMsg()

	SystemMsg_Print()

end

-- 메시지 출력
function SystemMsg_PrintMsg( strText, nColor )

	g_strSystemMsg_Text = strText
	g_nSystemMsg_Color = nColor
	
	if false == IsOpenWidget( WIDGET_ID_SYSTEMMSG ) then
		
		OpenWidget( WIDGET_ID_SYSTEMMSG )
		return
		
	end
	
	SystemMsg_Print()

end

-- 출력
function SystemMsg_Print()

	local strMsg = HTMLFontColor( PrepareHTMLString( g_strSystemMsg_Text ), TranslateHTMLColor( g_nSystemMsg_Color ) )
	SetString( WIDGET_ID_SYSTEMMSG, "htmlText", strMsg, "text" )
	
	local nTextHeight = GetInt( WIDGET_ID_SYSTEMMSG, "textHeight", "text" ) + 4
	
	if g_nSystemMsg_MinHeight < nTextHeight then
	
		SetInt( WIDGET_ID_SYSTEMMSG, "height", nTextHeight, "text" )
		SetInt( WIDGET_ID_SYSTEMMSG, "height", nTextHeight + g_nSystemMsg_HeightGap, "bg" )
	
	end
	
	local tbQuickSet = 
	{
		time = 6000,		-- 메시지 보여질 시간
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_SystemMsg_ElpaseTime"
	}
	SetTween( WIDGET_ID_SYSTEMMSG, tbQuickSet, "", "", "" )

end

------------------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 종료
function Ev_SystemMsg_ElpaseTime()

	CloseWidget( WIDGET_ID_SYSTEMMSG )

end