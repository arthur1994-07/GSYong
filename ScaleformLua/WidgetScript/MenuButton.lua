
-- 로드 완료
function LoadMenuButton()

	SetString( WIDGET_ID_MENUBUTTON, "label", GetGameWordText( "GAMEMENU", 14 ), "btnMenu" )
	SetEvent( WIDGET_ID_MENUBUTTON, EVENT_BUTTON_CLICK, "Ev_MenuButton_Show", "", "btnMenu" )

end

-- 메뉴 열기/닫기
function Ev_MenuButton_Show()

	Menu_Show()

end