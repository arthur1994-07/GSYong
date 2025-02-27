
-- 부스터 게이지 로드 완료
function LoadBoosterGauge()

	SetEvent( WIDGET_ID_BOOSTERGAUGE, EVENT_WIDGET_CLOSE, "CloseBoosterGauge", "", "" )
	
	SetDouble( WIDGET_ID_BOOSTERGAUGE, "maximum", 1.0, "bar" )
	SetDouble( WIDGET_ID_BOOSTERGAUGE, "minimum", 0.0, "bar" )
	SetDouble( WIDGET_ID_BOOSTERGAUGE, "value", 0.0, "bar" )

end

-- 오픈
function OpenBoosterGauge()

	-- 부스터 상태 초기화
	BoosterGauge_State( 0 )

	InsertFrameMove( BoosterGauge_FrameMove )

end

-- 클로즈
function CloseBoosterGauge()

	RemoveFrameMove( BoosterGauge_FrameMove )

end

-- 부스터 게이지 업데이트
function BoosterGauge_FrameMove( fElspTime )

	local fGauge = g_MyChar:GetVehicleBoosterGauge()
	SetDouble( WIDGET_ID_BOOSTERGAUGE, "value", fGauge, "bar" )

end

------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 탈것 탑승함
function BoosterGauge_RideVehicle( bBooster )

	if true == bBooster then
	
		OpenWidget( WIDGET_ID_BOOSTERGAUGE )
	
	else
	
		CloseWidget( WIDGET_ID_BOOSTERGAUGE )
	
	end

end

-- 부스터 상태
function BoosterGauge_State( nState )

	if false == IsOpenWidget( WIDGET_ID_BOOSTERGAUGE ) then
		return
	end
	
	local strFrame = ""
	
	-- 일반
	if 0 == nState then
	
		strFrame = "NONE"
	
	-- 부스터 준비
	elseif 1 == nState then
	
		strFrame = "READY"
	
	-- 부스터 사용
	elseif 2 == nState then
	
		strFrame = "BOOSTER"
		
	end
	
	CallFunction( WIDGET_ID_BOOSTERGAUGE, "gotoAndStop", strFrame, "mcEffect" )

end

-- 부스터 배움
function BoosterGauge_Learn( bRide )

	if true == bRide then
	
		BoosterGauge_RideVehicle( true )
	
	end

end