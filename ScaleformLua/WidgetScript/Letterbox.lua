
-- 레터 박스 오픈
function OpenLetterbox()

	Letterbox_UIPosition( GetScreenWidth(), GetScreenHeight() )

end

-- 위치 잡기
function Letterbox_UIPosition( nScreenWidth, nScreenHeight )

	if false == IsOpenWidget( WIDGET_ID_LETTERBOX ) then
		return
	end
	
	SetInt( WIDGET_ID_LETTERBOX, "width", nScreenWidth + 10, "boxUp" )
	SetInt( WIDGET_ID_LETTERBOX, "width", nScreenWidth + 10, "boxDown" )
	
	local nHeight = GetInt( WIDGET_ID_LETTERBOX, "height", "boxDown" )
	SetInt( WIDGET_ID_LETTERBOX, "y", nScreenHeight - nHeight, "boxDown" )

end