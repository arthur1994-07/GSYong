-- WIDGET_ID_NOTIFY_COMPETITION_TEXTURE

g_state = 0

function LoadNotifyCompetitionTexture()
	SetEvent(WIDGET_ID_NOTIFY_COMPETITION_TEXTURE, EVENT_MOUSE_CLICK, "EventNotifyCompetitionTextureClick", "", "instanceMain")

	SetInt(WIDGET_ID_NOTIFY_COMPETITION_TEXTURE, "y", GetScreenHeight()*0.075, "")	
	if g_state == 0 then
		SetString( WIDGET_ID_NOTIFY_COMPETITION_TEXTURE, "text", "BATTLE BEGIN", "instanceTextField.instanceText" )
	else
		SetString( WIDGET_ID_NOTIFY_COMPETITION_TEXTURE, "text", "BATTLE END", "instanceTextField.instanceText" )
	end
	
	
	local tableTimer = 
	{
		time = 3000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "EventNotifyCompetitionTextureClick"
	}
	SetTween( WIDGET_ID_NOTIFY_COMPETITION_TEXTURE, tableTimer, "", "", "" )	
end

function EventNotifyCompetitionTextureClick()
	CloseWidget( WIDGET_ID_NOTIFY_COMPETITION_TEXTURE )
end

-- nState = 0(begin), 1(end);
function ToggleNotifyCompetitionTexture(_nState)
	g_state = _nState
	
	OpenWidget( WIDGET_ID_NOTIFY_COMPETITION_TEXTURE )	
end

