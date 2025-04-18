
-- 보너스 타임 디스플레이 로드 완료
function LoadBonusTimeDisplay()

	HitTestEnable( WIDGET_ID_BONUSTIME_DISP, false, "" )

end

-- 보너스 타임 디스플레이 오픈
function OpenBonusTimeDisplay()

	CallFunction( WIDGET_ID_BONUSTIME_DISP, "gotoAndPlay", "1", "" )
	
	local tbQuickSet = 
	{
		time = 3000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_BonusTimeDisp_CompleteTimer"
	}
	SetTween( WIDGET_ID_BONUSTIME_DISP, tbQuickSet, "", "", "" )

end

-------------------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 끝
function Ev_BonusTimeDisp_CompleteTimer()

	CloseWidget( WIDGET_ID_BONUSTIME_DISP )

end