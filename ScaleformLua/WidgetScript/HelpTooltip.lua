
g_strHelpTooltip_text = ""

-- 툴팁 UI 로드 완료
function LoadHelpTooltip()

	if "" == g_strHelpTooltip_text then
	
		CloseWidget( WIDGET_ID_TOOLTIP )
		
	else
	
		SetString( WIDGET_ID_TOOLTIP, "text", g_strHelpTooltip_text, "tf" )
	
	end

end

-- 툴팁 텍스트 설정
function HelpTooltip_SetText( strText )

	g_strHelpTooltip_text = strText

	if true == IsLoadWidget( WIDGET_ID_TOOLTIP ) then
	
		SetString( WIDGET_ID_TOOLTIP, "text", strText, "tf" )
	
	end
	
	OpenWidget( WIDGET_ID_TOOLTIP )

end