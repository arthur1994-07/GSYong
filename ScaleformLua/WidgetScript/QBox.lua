
-- 큐박스 로드 완료
function LoadQBox()

	SetBool( WIDGET_ID_QBOX, "visible", false, "mcQBoxText" )
	SetBool( WIDGET_ID_QBOX, "visible", false, "mcQBoxTextOnce" )
	
	QBox_UIPosition( GetScreenWidth() )

end

-- 일회성 효과 일정 시간 후 자동 끄기
function QBox_AutoHidden()

	local tbQuickSet = 
	{
		time = 2000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_QBox_AutoHidden"
	}
	SetTween( WIDGET_ID_QBOX, tbQuickSet, "", "", "mcQBoxTextOnce" )

end

-- 지속성 효과 일정 시간 후 알파값 줄이기
function QBox_AutoAlpha()

	local tbQuickSet = 
	{
		time = 2000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_QBOX, tbQuickSet, "alpha", "0.5", "mcQBoxText" )

end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 창 사이즈 변경
function QBox_UIPosition( nScreenWidth )

	if false == IsOpenWidget( WIDGET_ID_QBOX ) then
		return
	end
	
	SetInt( WIDGET_ID_QBOX, "x", nScreenWidth / 2, "mcQBoxText" )
	SetInt( WIDGET_ID_QBOX, "y", 200, "mcQBoxText" )
	
	SetInt( WIDGET_ID_QBOX, "x", nScreenWidth / 2, "mcQBoxTextOnce" )
	SetInt( WIDGET_ID_QBOX, "y", 280, "mcQBoxTextOnce" )

end

-- 큐박스 습득
function QBox_Take( nType )

	local strAnim = ""
	local funcAutoHidden = nil
	local bOnce = false

	-- Speed Up
	if 1 == nType then
	
		strAnim = "SpeedUp"
		funcAutoHidden = QBox_AutoAlpha
	
	-- Crazy
	elseif 2 == nType then
	
		strAnim = "CrazyTime"
		funcAutoHidden = QBox_AutoAlpha
	
	-- Power Up
	elseif 3 == nType then
	
		strAnim = "PowerUp"
		funcAutoHidden = QBox_AutoAlpha
	
	-- EXP Time
	elseif 4 == nType then
	
		strAnim = "EXPTime"
		funcAutoHidden = QBox_AutoAlpha
	
	-- EXP Get
	elseif 5 == nType then
	
		strAnim = "EXPGet"
		funcAutoHidden = QBox_AutoHidden
		bOnce = true
	
	-- Lucky
	elseif 6 == nType then
	
		strAnim = "Lucky"
		funcAutoHidden = QBox_AutoAlpha
	
	-- Bomb
	elseif 7 == nType then
	
		strAnim = "Bomb"
		funcAutoHidden = QBox_AutoHidden
		bOnce = true
	
	-- Oh No
	elseif 8 == nType then
	
		strAnim = "OhNo"
		funcAutoHidden = QBox_AutoHidden
		bOnce = true
	
	-- Speed Up Max
	elseif 9 == nType then
	
		strAnim = "SpeedUpMax"
		funcAutoHidden = QBox_AutoAlpha
	
	-- Madness
	elseif 10 == nType then
	
		strAnim = "MadTime"
		funcAutoHidden = QBox_AutoAlpha
	
	-- Power Up Max
	elseif 11 == nType then
	
		strAnim = "PowerUpMax"
		funcAutoHidden = QBox_AutoAlpha
	
	-- Heal
	elseif 12 == nType then
	
		strAnim = "Heal"
		funcAutoHidden = QBox_AutoHidden
		bOnce = true
	
	else
	
		SetBool( WIDGET_ID_QBOX, "visible", false, "mcQBoxText" )
		SetBool( WIDGET_ID_QBOX, "visible", false, "mcQBoxTextOnce" )
		return
		
	end
	
	if false == bOnce then
	
		CallFunction( WIDGET_ID_QBOX, "gotoAndPlay", strAnim, "mcQBoxText" )
		SetDouble( WIDGET_ID_QBOX, "alpha", 1, "mcQBoxText" )
		SetBool( WIDGET_ID_QBOX, "visible", true, "mcQBoxText" )
	
	else
	
		CallFunction( WIDGET_ID_QBOX, "gotoAndPlay", strAnim, "mcQBoxTextOnce" )
		SetDouble( WIDGET_ID_QBOX, "alpha", 1, "mcQBoxTextOnce" )
		SetBool( WIDGET_ID_QBOX, "visible", true, "mcQBoxTextOnce" )
	
	end
	
	funcAutoHidden()

end

-- 지속성 큐박스 리셋
function QBox_Reset()

	SetBool( WIDGET_ID_QBOX, "visible", false, "mcQBoxText" )

end

--------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 일회성 큐박스 효과 visible 끄기
function Ev_QBox_AutoHidden()

	SetBool( WIDGET_ID_QBOX, "visible", false, "mcQBoxTextOnce" )

end