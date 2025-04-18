
TIPMSG_nColor = TEXT_COLOR_WHITE
TIPMSG_nVisibleTime = 6000
TIPMSG_nHideTime = 4000
g_bTipMsg_ShowTip = false

-- 팁 메시지 UI 로드 완료
function LoadTipMsg()

	HitTestEnable( WIDGET_ID_TIPMSG, false, "" )
	
end

-- 팁 메시지 오픈
function OpenTipMsg()

	local tbGameOption = GetGameOption()
	g_bTipMsg_ShowTip = tbGameOption[ "bTip" ]
	
	if g_bTipMsg_ShowTip == false then
	
		CloseWidget(WIDGET_ID_TIPMSG)
		return
		
	end
	
	TipMsg_Print()

end

-- 메세지 출력
function TipMsg_Print()

	local strMsg = GetTipText_RandomIntex()
	
	local strColor = TranslateHTMLColor( TIPMSG_nColor )
	local strTipMsg = HTMLFontColor( PrepareHTMLString( strMsg ), strColor )

	SetBool( WIDGET_ID_TIPMSG, "visible", true, "textField" )
	SetString( WIDGET_ID_TIPMSG, "htmlText", strTipMsg, "textField" )
	--SetBool( WIDGET_ID_TIPMSG, "visible", false, "BG" )
	
	local tbQuickSet = 
	{
		time = TIPMSG_nVisibleTime,		-- 메시지 보여질 시간
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_TipMsg_EndTime"
	}
	SetTween( WIDGET_ID_TIPMSG, tbQuickSet, "", "", "" )
	
end

function Ev_TipMsg_EndTime()
	
	SetBool( WIDGET_ID_TIPMSG, "visible", false, "textField" )
	
	if g_bTipMsg_ShowTip == false then
		return
	end
	
	local tbQuickSet = 
	{
		time = TIPMSG_nHideTime,		-- 메시지 보여질 시간
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "TipMsg_Print"
	}
	SetTween( WIDGET_ID_TIPMSG, tbQuickSet, "", "", "" )
	
end


function TipMsg_Show( bShow )

	g_bTipMsg_ShowTip = bShow
	
	if true == g_bTipMsg_ShowTip then
	
		OpenWidget( WIDGET_ID_TIPMSG )
	
	else
	
		CloseWidget( WIDGET_ID_TIPMSG )
	
	end
	
end