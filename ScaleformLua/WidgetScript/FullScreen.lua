function LoadFullScreen()
	-- 전체 화면 버튼 이벤트
	SetEvent( WIDGET_ID_FULLSCREEN, EVENT_BUTTON_CLICK, "Ev_FullScreen_btnFullScreen", "", "FullScreen" )	
end


-- 이벤트

-- 전체화면 버튼 이벤트
function Ev_FullScreen_btnFullScreen()

	ChangeScreenMode()

end