
-- 로드 완료
function LoadWarning()

	HitTestEnable( WIDGET_ID_WARNING, false, "" )

end

------------------------------------------------------------------------------
-- 클라리언트 콜

function Warning_Show( bShow )

	if true == bShow then
	
		OpenWidget( WIDGET_ID_WARNING )
	
	else
	
		CloseWidget( WIDGET_ID_WARNING )
	
	end

end