
g_bBonusTime_EventBuster = false	-- 부스터 타임인가?

g_nBonusTime_PlayTime = 0			-- 플레이 타임
g_nBonusTime_PalyTimeRemain = 0		-- 플레이 타임 남은 시간

g_nBonusTime_BusterTime = 0			-- 부스터 타임
g_nBonusTime_BusterTimeRemain = 0	-- 부스터 타임 남은 시간

g_fBonusTime_ExpGain = 0.0			-- 경험치 증가율
g_fBonusTime_ItemGain = 0.0			-- 아이템 획득 증가율

-- 업데이트 타이머
g_fBonusTime_Timer = 0

-- 툴팁 텍스트
g_strBonusTime_TooltipText = ""

-- 보너스 타임 게이지 로드 완료
function LoadBonusTime()

	SetEvent( WIDGET_ID_BONUSTIME, EVENT_WIDGET_CLOSE, "CloseBonusTime", "", "" )
	
	HitTestEnable( WIDGET_ID_BONUSTIME, false, "bt.labelText" )

	-- 보너스 타임 텍스트 설정
	local str = HTMLFontColor( "Bonus ", HTML_TEXT_COLOR_YELLOW ) .. HTMLFontColor( "Time Gauge", HTML_TEXT_COLOR_WHITE )
	SetString( WIDGET_ID_BONUSTIME, "htmlText", str, "bt.labelText" )
	
	g_strBonusTime_TooltipText = string.format( "%s\n%s", 
		GetGameInternalText( "BONUS_TIME_MESSAGE", 1 ),
		GetGameInternalText( "BONUS_TIME_MESSAGE", 2 ) )

end

-- 보너스 타임 게이지 오픈
function OpenBonusTime()

	BonusTime_SetGauge()
	
	SetFoward( WIDGET_ID_BUFFCTF )
	SetFoward( WIDGET_ID_BUFFSYSTEM )
	SetFoward( WIDGET_ID_BUFFEXPLUNCHBOX )
	SetFoward( WIDGET_ID_BUFFLUNCHBOX )

end

-- 닫기
function CloseBonusTime()

	RemoveFrameMove( BonusTime_FrameMove )

end

-- 업데이트
function BonusTime_FrameMove( fElspTime )

	g_fBonusTime_Timer = g_fBonusTime_Timer + fElspTime
	if 1 > g_fBonusTime_Timer then
		return
	end
	
	g_fBonusTime_Timer = 0
	
	if false == g_bBonusTime_EventBuster then
	
		g_nBonusTime_PalyTimeRemain = g_nBonusTime_PalyTimeRemain - 1
		
		local str = string.format( StringFormat( GetGameInternalText( "BONUS_TIME_MESSAGE", 0 ), 
			math.floor( g_fBonusTime_ExpGain ),
			math.floor( g_fBonusTime_ItemGain ),
			math.floor( g_nBonusTime_PalyTimeRemain ) ) ) .. "\n" .. 
			g_strBonusTime_TooltipText
		SetString( WIDGET_ID_BONUSTIME, "TooltipText", str, "bt.barGreen" )
	
	else
	
		g_nBonusTime_BusterTimeRemain = g_nBonusTime_BusterTimeRemain - 1
		
		local str = string.format( StringFormat( GetGameInternalText( "BONUS_TIME_MESSAGE", 0 ), 
			math.floor( g_fBonusTime_ExpGain ),
			math.floor( g_fBonusTime_ItemGain ),
			math.floor( g_nBonusTime_BusterTimeRemain ) ) ) .. "\n" .. 
			g_strBonusTime_TooltipText
		SetString( WIDGET_ID_BONUSTIME, "TooltipText", str, "bt.barRed" )
	
	end

end

-- 보너스 타임 게이지 설정
function BonusTime_SetGauge()

	if false == g_bBonusTime_EventBuster then
	
		SetBool( WIDGET_ID_BONUSTIME, "visible", false, "bt.barRed" )
		SetBool( WIDGET_ID_BONUSTIME, "visible", true, "bt.barGreen" )
		SetBool( WIDGET_ID_BONUSTIME, "visible", false, "bt.flame_bg" ) --뒤의 불타는 이미지 렌더링안함
		
		local nValue = g_nBonusTime_PlayTime - g_nBonusTime_PalyTimeRemain
		
		SetInt( WIDGET_ID_BONUSTIME, "maximum", g_nBonusTime_PlayTime, "bt.barGreen" )
		SetInt( WIDGET_ID_BONUSTIME, "value", nValue, "bt.barGreen" )
		
		local tbQuickSet = 
		{
			time = g_nBonusTime_PalyTimeRemain * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_BONUSTIME, tbQuickSet, "value", tostring( g_nBonusTime_PlayTime ), "bt.barGreen" )
		
	else
	
		SetBool( WIDGET_ID_BONUSTIME, "visible", true, "bt.barRed" )
		SetBool( WIDGET_ID_BONUSTIME, "visible", false, "bt.barGreen" )
		SetBool( WIDGET_ID_BONUSTIME, "visible", true, "bt.flame_bg" ) --뒤의 불타는 이미지 렌더링
		
		SetInt( WIDGET_ID_BONUSTIME, "maximum", g_nBonusTime_BusterTime, "bt.barRed" )
		SetInt( WIDGET_ID_BONUSTIME, "value", g_nBonusTime_BusterTimeRemain, "bt.barRed" )
		
		local tbQuickSet = 
		{
			time = g_nBonusTime_BusterTimeRemain * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_BONUSTIME, tbQuickSet, "value", "0", "bt.barRed" )
	
	end
	
	InsertFrameMove( BonusTime_FrameMove )

end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 보너스 타임 시작
function BonusTime_StartEvent( bEventBuster, nTime, nRemainTime, fExpGain, fItemGain )

	g_bBonusTime_EventBuster = bEventBuster
	if false == g_bBonusTime_EventBuster then
	
		if 0 == nRemainTime then
			nRemainTime = nTime
		end
	
		g_nBonusTime_PlayTime = nTime
		g_nBonusTime_PalyTimeRemain = nRemainTime
	
	else
	
		g_nBonusTime_BusterTime = nTime
		g_nBonusTime_BusterTimeRemain = nRemainTime
		
		if g_nBonusTime_BusterTime == g_nBonusTime_BusterTimeRemain then
			OpenWidget( WIDGET_ID_BONUSTIME_DISP )
		end
	
	end
	
	g_fBonusTime_ExpGain = fExpGain * 100
	g_fBonusTime_ItemGain = fItemGain * 100

	if false == IsOpenWidget( WIDGET_ID_BONUSTIME ) then
		OpenWidget( WIDGET_ID_BONUSTIME )
		return
	end
	
	BonusTime_SetGauge()

end
