
-- 총 중첩 증가율
g_nBuffExpLunchBox_TotalRate = 0
-- 버프 중첩 횟수
g_nBuffExpLunchBox_AccumCount = 0
-- 버프 중첩 최대 횟수
g_nBuffExpLunchBox_MaxCount = 0
-- 버프 이름
g_strBuffExpLunchBox_SkillName = ""

-- 등록된 버프 클라이언트 실제 위치
g_nBuffExpLunchBox_SlotPos = -1
-- 등록된 버프 스킬 ID
g_nBuffExpLunchBox_SkillID = -1
-- 등록된 버프 시간 저장
g_nBuffExpLunchBox_Timer = 0
-- 버프 툴팁 텍스트 저장용
g_nBuffExpLunchBox_ToolTipText = ""

-- 경험치 도시락 버프 로드 완료
function LoadBuffExpLunchBox()

	g_nBuffExpLunchBox_SlotPos = -1
	g_nBuffExpLunchBox_SkillID = -1

	SetEvent( WIDGET_ID_BUFFEXPLUNCHBOX, EVENT_WIDGET_CLOSE, "CloseBuffExpLunchBox", "", "" )
	
	SetBool( WIDGET_ID_BUFFEXPLUNCHBOX, "visible", false, "slotTimer" )
	SetBool( WIDGET_ID_BUFFEXPLUNCHBOX, "tooltipEnabled", false, "slotTimer.slot" )
	SetString( WIDGET_ID_BUFFEXPLUNCHBOX, "binding", SLOT_BUFF_EXP_LUNCHBOX_ID, "slotTimer.slot" )
	SetDouble( WIDGET_ID_BUFFEXPLUNCHBOX, "maximum", 100, "slotTimer.bar" )

end

-- 오픈
function OpenBuffExpLunchBox()

	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		-- 경험치 도시락 버프만
		if 3 == tbBuff[ 2 ] then
		
			ExpLunchBoxBuff_Reg( tbBuff[ 1 ], tbBuff[ 3 ], tbBuff[ 4 ], tbBuff[ 5 ], tbBuff[ 6 ] )
		
		end
	
	end

end

-- 클로즈
function CloseBuffExpLunchBox()

	g_nBuffExpLunchBox_SlotPos = -1
	g_nBuffExpLunchBox_SkillID = -1

end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 버프 등록
function ExpLunchBoxBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )

	if false == IsOpenWidget( WIDGET_ID_BUFFEXPLUNCHBOX ) then
	
		OpenWidget( WIDGET_ID_BUFFEXPLUNCHBOX )
		return
		
	end
	
	if 	g_nBuffExpLunchBox_SlotPos ~= dwSlot or
		g_nBuffExpLunchBox_SkillID ~= dwSkillID then
		
		SetBool( WIDGET_ID_BUFFEXPLUNCHBOX, "visible", true, "slotTimer" )
	
		g_nBuffExpLunchBox_SlotPos = dwSlot
		g_nBuffExpLunchBox_SkillID = dwSkillID
		
		-- 스킬 아이콘 등록
		SetSlot_SkillLevel( SLOT_BUFF_EXP_LUNCHBOX_ID, dwSkillID, wLevel )
		
	end
	
	-- 타이머 초기화
	local nValue = fAge / fLife * 100
	SetDouble( WIDGET_ID_BUFFEXPLUNCHBOX, "value", nValue, "slotTimer.bar" )
	
	-- 도시락 사용 가능한 맵인가?
	local bEnabled = g_MyChar:IsEnableLunchBox()
	SetBool( WIDGET_ID_BUFFEXPLUNCHBOX, "enabled", bEnabled, "slotTimer.slot" )
	
	-- 사용 가능하면 타이머 실행
	if true == bEnabled then
	
		local tbQuickSet = 
		{
			time = fAge * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_ExpLunchBoxBuff_TimeOver"
		}
		SetTween( WIDGET_ID_BUFFEXPLUNCHBOX, tbQuickSet, "value", "0", "slotTimer.bar" )

	-- 타이머 돌릴 필요 없다.
	else
	
		local tbQuickSet = 
		{
			time = 0,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_BUFFEXPLUNCHBOX, tbQuickSet, "value", tostring( nValue ), "slotTimer.bar" )
	
	end
	
end

-- 버프 데이터 셋팅
-- 증가율, 총 증가율, 중첩 횟수, 총 중첩 횟수, 스킬 이름, 스킬 설명, 시간( [ 1 ] : 일 [ 2 ] : 시 [ 3 ] : 분 [ 4 ] : 초 )
function ExpLunchBoxBuff_DataSet( dwIncreaseRate, dwTotalRate, dwAccumCount, dwMaxCount, strSkillName, strDesc, tbTime )

	g_nBuffExpLunchBox_TotalRate = dwTotalRate
	g_nBuffExpLunchBox_AccumCount = dwAccumCount
	g_nBuffExpLunchBox_MaxCount = dwMaxCount
	g_strBuffExpLunchBox_SkillName = strSkillName
	
	local strTime = ""
	g_nBuffExpLunchBox_Timer = 0
	if 0 ~= tbTime[1] then -- 일
		strTime = string.format( GetGameWordText( "TOURNAMENT_START_DATE" ), tbTime[1] )
		g_nBuffExpLunchBox_Timer = tbTime[1] * 12 * 60
	end
	if 0 ~= tbTime[2] then -- 시
		strTime = strTime .. " " .. tostring( tbTime[2] ) .. GetGameWordText( "ITEM_BASIC_TIME", 0 )
		g_nBuffExpLunchBox_Timer = g_nBuffExpLunchBox_Timer + ( tbTime[2] * 60 )
	end
	if 0 ~= tbTime[3] then -- 분
		strTime = strTime .. " " .. tostring( tbTime[3] ) .. GetGameWordText( "ITEM_BASIC_TIME", 1 )
		g_nBuffExpLunchBox_Timer = g_nBuffExpLunchBox_Timer + ( tbTime[3] )
	end
	if 0 ~= tbTime[4] then -- 초
		strTime = strTime .. " " .. tostring( tbTime[4] ) .. GetGameWordText( "ITEM_BASIC_TIME", 2 )
	end
	 
	ExpLunchBoxBuff_SetAccum( dwTotalRate, dwAccumCount, dwMaxCount, strDesc )
	
	-- 경험치 증가량 및 중첩횟수 등 시스템 메시지로 알려줌
	local strAccumPer = string.format( GetGameInternalText( "SKILLFACT_ACCUM_COUNT_PERCENT" ), dwIncreaseRate )
	
	local strChatMsg = string.format( GetGameInternalText("LUNCHBOX_USE"), strSkillName, strAccumPer, strTime ) .. 
		string.format( StringFormat( GetGameInternalText( "SKILLFACT_ACCUM_COUNT_INFORMATION" ), dwAccumCount, dwTotalRate ) )
	BasicChat_PrintSystemMsg( CHAT_SYSTEM, strChatMsg, TEXT_COLOR_PALEGREEN )
	
end

-- 버프 삭제
function ExpLunchBoxBuff_Delete( nIndex )

	g_nBuffExpLunchBox_SlotPos = -1
	g_nBuffExpLunchBox_SkillID = -1
	
	SetBool( WIDGET_ID_BUFFEXPLUNCHBOX, "visible", false, "slotTimer" )

end

-- 버프 중첩 횟수 설정
function ExpLunchBoxBuff_SetAccum( dwTotalRate, dwAccumCount, dwMaxCount, strDesc )

	g_nBuffExpLunchBox_TotalRate = dwTotalRate
	g_nBuffExpLunchBox_AccumCount = dwAccumCount
	g_nBuffExpLunchBox_MaxCount = dwMaxCount
	
	SetString( WIDGET_ID_BUFFEXPLUNCHBOX, "text", g_nBuffExpLunchBox_AccumCount, "slotTimer.labelNum" )

	local strAccumPer = string.format( GetGameInternalText( "SKILLFACT_ACCUM_COUNT_PERCENT" ), dwTotalRate )
	local strAccumCnt = string.format( GetGameInternalText( "SKILLFACT_ACCUM_COUNT" ), dwAccumCount, dwMaxCount )

	if dwMaxCount > 1 then		-- 중첩 경험치 효과값과 일반 경험지 효과값 달라야해서 이렇게 수정 
		g_nBuffExpLunchBox_ToolTipText = string.format( "[%s]\n%s\n%s\n%s",
		HTMLFontColor( g_strBuffExpLunchBox_SkillName, HTML_TEXT_COLOR_GOLD ),
		HTMLFontColor( strAccumPer, HTML_TEXT_COLOR_WHITE ),
		HTMLFontColor( strDesc, HTML_TEXT_COLOR_WHITE ),
		HTMLFontColor( strAccumCnt, HTML_TEXT_COLOR_GOLD ) )
	else
		g_nBuffExpLunchBox_ToolTipText = string.format( "[%s]\n%s\n%s",
		HTMLFontColor( g_strBuffExpLunchBox_SkillName, HTML_TEXT_COLOR_GOLD ),
		HTMLFontColor( strDesc, HTML_TEXT_COLOR_WHITE ),
		HTMLFontColor( strAccumCnt, HTML_TEXT_COLOR_GOLD ) )
	end

	Ev_ExpLunchBoxBuff_Timer()

end

--------------------------------------------------------------------------------------------------------------
-- 이벤트 

-- 버프 시간 끝
function Ev_ExpLunchBoxBuff_TimeOver()

	g_nBuffExpLunchBox_SlotPos = -1
	g_nBuffExpLunchBox_SkillID = -1
	
	SetBool( WIDGET_ID_BUFFEXPLUNCHBOX, "visible", false, "slotTimer" )

end

-- 남은시간 갱신
function Ev_ExpLunchBoxBuff_Timer()

	local str = HTMLFontColor( GetGameInternalText( "SKILLFACT_LIFETIME" ) .. " : " .. 
		g_nBuffExpLunchBox_Timer .. GetGameWordText( "ITEM_BASIC_TIME", 1 ), HTML_TEXT_COLOR_WHITE )
	
	local strTime = string.format( "%s\n%s",
		g_nBuffExpLunchBox_ToolTipText, str )
		
	if false == g_MyChar:IsEnableLunchBox() then
	
		strTime = strTime .. "\n" .. HTMLFontColor( GetGameInternalText( "SKILLFACT_DISABLE", 0 ), HTML_TEXT_COLOR_RED )
	
	end
	
	SetString( WIDGET_ID_BUFFEXPLUNCHBOX, "TooltipHtmlText", strTime, "slotTimer.slot" )
	
	-- 타이머 실행(1분)
	if true == g_MyChar:IsEnableLunchBox() then
	
		local tbQuickSet = 
		{
			time = 60000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_ExpLunchBoxBuff_Timer"
		}
		SetTween( WIDGET_ID_BUFFEXPLUNCHBOX, tbQuickSet, "", "", "" )
	
	else
	
		local tbQuickSet = 
		{
			time = 0,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_BUFFEXPLUNCHBOX, tbQuickSet, "", "", "" )
	
	end
	
	g_nBuffExpLunchBox_Timer = g_nBuffExpLunchBox_Timer - 1
end