
-- 버프 툴팁 텍스트 저장용
g_nBuffLunchBox_ToolTipText = ""
-- 등록된 버프 시간 저장
g_nBuffLunchBox_Timer = 0

-- 등록된 버프 클라이언트 실제 위치
g_nBuffLunchBox_SlotPos = -1
-- 등록된 버프 스킬 ID
g_nBuffLunchBox_SkillID = -1

-- 도시락 버프 로드 완료
function LoadBuffLunchBox()	

	g_nBuffLunchBox_SlotPos = -1
	g_nBuffLunchBox_SkillID = -1
	
	SetEvent( WIDGET_ID_BUFFLUNCHBOX, EVENT_WIDGET_CLOSE, "CloseBuffLunchBox", "", "" )
	SetBool( WIDGET_ID_BUFFLUNCHBOX, "tooltipEnabled", false, "slotTimer.slot" )
	SetString( WIDGET_ID_BUFFLUNCHBOX, "binding", SLOT_BUFF_LUNCHBOX_ID, "slotTimer.slot" )
	SetDouble( WIDGET_ID_BUFFLUNCHBOX, "maximum", 100, "slotTimer.bar" )

end

-- 오픈
function OpenBuffLunchBox()

	SetBool( WIDGET_ID_BUFFLUNCHBOX, "visible", false, "slotTimer" )
	
	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		-- 도시락 버프만
		if 2 == tbBuff[ 2 ] then

			LunchBoxBuff_Reg( tbBuff[ 1 ], tbBuff[ 3 ], tbBuff[ 4 ], tbBuff[ 5 ], tbBuff[ 6 ] )
		
		end
	
	end

end

-- 닫기
function CloseBuffLunchBox()

	g_nBuffLunchBox_SlotPos = -1
	g_nBuffLunchBox_SkillID = -1

end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 버프 등록
function LunchBoxBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )

	if false == IsOpenWidget( WIDGET_ID_BUFFLUNCHBOX ) then
	
		OpenWidget( WIDGET_ID_BUFFLUNCHBOX )
		return
		
	end
	
	if 	g_nBuffLunchBox_SlotPos ~= dwSlot or
		g_nBuffLunchBox_SkillID ~= dwSkillID then
		
		SetBool( WIDGET_ID_BUFFLUNCHBOX, "visible", true, "slotTimer" )
	
		g_nBuffLunchBox_SlotPos = dwSlot
		g_nBuffLunchBox_SkillID = dwSkillID
		
		-- 스킬 아이콘 등록
		SetSlot_SkillLevel( SLOT_BUFF_LUNCHBOX_ID, g_nBuffLunchBox_SkillID, wLevel )
		
	end

	-- 타이머 초기화
	local fValue = fAge / fLife * 100
	SetDouble( WIDGET_ID_BUFFLUNCHBOX, "value", fValue, "slotTimer.bar" )
	
	-- 도시락 사용 가능한 맵인가?
	local bEnabled = g_MyChar:IsEnableLunchBox()
	SetBool( WIDGET_ID_BUFFLUNCHBOX, "enabled", bEnabled, "slotTimer.slot" )
	
	if true == bEnabled then
	
		-- 타이머 실행
		local tbQuickSet = 
		{
			time = fAge * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_LunchBoxBuff_Timer"
		}
		SetTween( WIDGET_ID_BUFFLUNCHBOX, tbQuickSet, "value", "0", "slotTimer.bar" )
	
	else
	
		local tbQuickSet = 
		{
			time = 0,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_BUFFLUNCHBOX, tbQuickSet, "value", tostring( fValue ), "slotTimer.bar" )
	
	end
	
	Ev_LunchBoxBuff_TextTimer()

end

-- 버프 중첩 확인
function Buff_LunchBoxOverlap( bOverlap, strPrevSkill, strCurSkill, wPosX, wPosY )

	local str = ""
	if true == bOverlap then
	
		str = string.format( GetGameInternalText( "SKILLFACT_ASK_OVERWRITE_1", 0 ), strPrevSkill )
	
	else
	
		str = string.format( GetGameInternalText( "SKILLFACT_ASK_OVERWRITE_0", 0 ), strPrevSkill, strCurSkill )
	
	end
	
	-- 메시지 박스 띄우기
	local tbData = { wPosX, wPosY }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_LunchBoxBuff_OverlapOK, Event_MsgBox_Close, tbData )

end

-- 버프 삭제
function LunchBoxBuff_Delete( nIndex )

	g_nBuffLunchBox_SlotPos = -1
	g_nBuffLunchBox_SkillID = -1
	
	SetBool( WIDGET_ID_BUFFLUNCHBOX, "visible", false, "slotTimer" )

end

-- 버프 데이터 셋팅
-- 스킬 이름, 스킬 설명, 시간( [ 1 ] : 일 [ 2 ] : 시 [ 3 ] : 분 [ 4 ] : 초 )
function LunchBoxBuff_DataSet( strSkillName, strDesc, tbTime )

	g_nBuffLunchBox_Timer = 0
	
	if 0 ~= tbTime[ 1 ] then -- 일
	
		g_nBuffLunchBox_Timer = tbTime[ 1 ] * 12 * 60
		
	end
	
	if 0 ~= tbTime[ 2 ] then -- 시
	
		g_nBuffLunchBox_Timer = g_nBuffLunchBox_Timer + ( tbTime[ 2 ] * 60 )
		
	end
	
	if 0 ~= tbTime[ 3 ] then -- 분
	
		g_nBuffLunchBox_Timer = g_nBuffLunchBox_Timer + ( tbTime[ 3 ] )
		
	end
	
	g_nBuffLunchBox_ToolTipText = string.format( "[%s]\n%s",
		HTMLFontColor( strSkillName, HTML_TEXT_COLOR_GOLD ),
		HTMLFontColor( strDesc, HTML_TEXT_COLOR_WHITE ) )
	
	Ev_LunchBoxBuff_TextTimer()
	
end

--------------------------------------------------------------------------------------------------------------
-- 이벤트 

-- 버프 시간 끝
function Ev_LunchBoxBuff_Timer()

	g_nBuffLunchBox_SlotPos = -1
	g_nBuffLunchBox_SkillID = -1
	
	SetBool( WIDGET_ID_BUFFLUNCHBOX, "visible", false, "slotTimer" )

end

-- 버프 중첩 확인 OK
function Ev_LunchBoxBuff_OverlapOK( tbData )
	
	-- 경험치 도시락 사용 서버에 요청
	g_Msg:ReqInvenLunchBox( tbData[ 1 ], tbData[ 2 ] )

	Event_MsgBox_Close()

end

-- 남은시간 갱신
function Ev_LunchBoxBuff_TextTimer()

	local strTime = GetGameInternalText( "SKILLFACT_LIFETIME" ) .. " : " .. 
		g_nBuffLunchBox_Timer .. GetGameWordText( "ITEM_BASIC_TIME", 1 )
	
	local str = string.format( "%s\n%s",
		g_nBuffLunchBox_ToolTipText,
		HTMLFontColor( strTime, HTML_TEXT_COLOR_WHITE ) )
		
	if false == g_MyChar:IsEnableLunchBox() then
	
		str = str .. "\n" .. HTMLFontColor( GetGameInternalText( "SKILLFACT_DISABLE", 0 ), HTML_TEXT_COLOR_RED )
	
	end
	
	SetString( WIDGET_ID_BUFFLUNCHBOX, "TooltipHtmlText", str, "slotTimer.slot" )
	
	-- 타이머 실행
	local tbQuickSet = 
	{
		time = 60000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_LunchBoxBuff_TextTimer"
	}
	SetTween( WIDGET_ID_BUFFLUNCHBOX, tbQuickSet, "", "", "" )
	
	if true == g_MyChar:IsEnableLunchBox() then
		g_nBuffLunchBox_Timer = g_nBuffLunchBox_Timer - 1
	end
	
end