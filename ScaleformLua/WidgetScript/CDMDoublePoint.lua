
-- 로드 완료
function LoadCDMDoublePoint()

	HitTestEnable( WIDGET_ID_CDMDOUBLEPOINT, false, "" )

end

--------------------------------------------------------------------------------
-- 클라이언트 콜

-- 열기/닫기
function CDMDoublePoint_Show( bVisible )

	if true == bVisible then
	
		OpenWidget( WIDGET_ID_CDMDOUBLEPOINT )
	
	else
	
		CloseWidget( WIDGET_ID_CDMDOUBLEPOINT )
	
	end

end