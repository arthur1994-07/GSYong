-- 시스템 메뉴창 ( ESC Menu ) - 등급 표시 UI
function LoadEscSystemMenu_Grade_Display()
	SetEvent( WIDGET_ID_ESC_GRADE_DISPLAY, EVENT_MOUSE_CLICK, "Grade_Display_MouseClick", "", "" )	
end
-- 등급표시 창에 마우스 클릭시 이벤트
function Grade_Display_MouseClick()
	CloseWidget( WIDGET_ID_ESC_GRADE_DISPLAY )
end