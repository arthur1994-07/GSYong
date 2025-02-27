
-- 등록된 버프 클라이언트 실제 위치
g_tbSystemBuff_SlotPos = {}

-- 등록된 버프 리스트
g_tbSystemBuff_List = {}

g_bSystemBuff_IsOpen = false

-- 시스템 버프 로드 완료
function LoadBuffSystem()

	g_bSystemBuff_IsOpen = true
	
	for i = 1, BUFF_SYSTEM_SLOT_TOTAL, 1 do
	
		local strSlot = "slotTimer" .. tostring( i )
		
		SetBool( WIDGET_ID_BUFFSYSTEM, "visible", false, strSlot )
		SetString( WIDGET_ID_BUFFSYSTEM, "binding", SLOT_BUFF_SYSTEM_ID .. tostring( i ), strSlot .. ".slot" )
		SetDouble( WIDGET_ID_BUFFSYSTEM, "maximum", 100, strSlot .. ".bar" )
		
	end

end

function OpenBuffSystem()

	g_tbSystemBuff_List = {}
	
	for i = 1, BUFF_SYSTEM_SLOT_TOTAL, 1 do
	
		local strSlot = "slotTimer" .. tostring( i )
		SetBool( WIDGET_ID_BUFFSYSTEM, "visible", false, strSlot )
		
	end

	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		-- 시스템 버프만
		if 4 == tbBuff[ 2 ] then
		
			SystemBuff_Reg( tbBuff[ 1 ], tbBuff[ 3 ], tbBuff[ 4 ], tbBuff[ 5 ], tbBuff[ 6 ], tbBuff[ 8 ] )
		
		end
	
	end

end

-- 빈 슬롯 찾기
function SystemBuff_FindEmptySlot( dwSkillID )

	for i = 1, BUFF_SYSTEM_SLOT_TOTAL, 1 do
	
		if dwSkillID == g_tbSystemBuff_List[ i ] or nil == g_tbSystemBuff_List[ i ] then
			return i
		end
	
	end
	
	return -1

end

-- 버프 슬롯 위치 정렬
function SystemBuff_AlignPos( nIndex )

	if nIndex < BUFF_SYSTEM_SLOT_TOTAL then
	
		local nTemp = nIndex + 1
		while nTemp < BUFF_SYSTEM_SLOT_TOTAL do
		
			local nPrevIndex = nTemp - 1
			local strPrevSlot = "slotTimer" .. tostring( nPrevIndex )
			
			-- 다음으로 등록된 버프가 없으면 끝내기
			if nil == g_tbSystemBuff_List[ nTemp ] then
			
				-- 등록된 버프가 하나면 초기화
				if nTemp == nIndex + 1 then
				
					g_tbSystemBuff_SlotPos[ nPrevIndex ] = nil
					g_tbSystemBuff_List[ nPrevIndex ] = nil
					
				end
				
				SetBool( WIDGET_ID_BUFFSYSTEM, "visible", false, strPrevSlot )
			
				break
				
			end
			
			-- 다음으로 등록된 버프가 있으면 한칸씩 당겨주기
			local strSlot = "slotTimer" .. tostring( nTemp )
			
			local nValue = GetInt( WIDGET_ID_BUFFSYSTEM, "value", strSlot .. ".bar" )
			
			local dwSkillID = g_tbSystemBuff_List[ nTemp ]
			local dwSlot = g_tbSystemBuff_SlotPos[ nTemp ]
			
			g_tbSystemBuff_List[ nPrevIndex ] = dwSkillID
			g_tbSystemBuff_List[ nTemp ] = nil
			
			g_tbSystemBuff_SlotPos[ nPrevIndex ] = dwSlot
			g_tbSystemBuff_SlotPos[ nTemp ] = nil
			
			-- 버프 남은 시간
			local fAge = g_MyChar:GetSkillFactAge( dwSkillID )
			
			-- 스킬 아이콘
			ResetSlot( SLOT_BUFF_SYSTEM_ID .. tostring( nTemp ) )
			SetSlot_Skill( SLOT_BUFF_SYSTEM_ID .. tostring( nPrevIndex ), dwSkillID )
			
			-- 타이머 초기화
			SetDouble( WIDGET_ID_BUFFSYSTEM, "value", nValue, strPrevSlot .. ".bar" )
			
			-- 타이머 실행
			local tbQuickSet = 
			{
				time = fAge * 1000,
				delay = 0,
				ease = EASE_NONE,
				loop = false,
				onComplete = "Ev_SystemBuff_Timer" .. tostring( nPrevIndex )
			}
			SetTween( WIDGET_ID_BUFFSYSTEM, tbQuickSet, "value", "0", strPrevSlot .. ".bar" )
			
			nTemp = nTemp + 1
		
		end
	
	else
	
		local strSlot = "slotTimer" .. tostring( nIndex )
		SetBool( WIDGET_ID_BUFFSYSTEM, "visible", false, strSlot )
	
	end

end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 버프 등록
function SystemBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife, bExpired )

	if false == g_bSystemBuff_IsOpen then
		return
	end

	local nIndex = SystemBuff_FindEmptySlot( dwSkillID )
	if -1 == nIndex then
		return
	end
	
	local strSlot = "slotTimer" .. tostring( nIndex )
	
	if 	g_tbSystemBuff_SlotPos[ nIndex ] ~= dwSlot or
		g_tbSystemBuff_List[ nIndex ] ~= dwSkillID then
		
		-- 등록된 스킬 저장
		g_tbSystemBuff_SlotPos[ nIndex ] = dwSlot
		g_tbSystemBuff_List[ nIndex ] = dwSkillID
		
		-- visible on
		SetBool( WIDGET_ID_BUFFSYSTEM, "visible", true, strSlot )

		-- 스킬 아이콘 등록
		SetSlot_SkillLevel( SLOT_BUFF_SYSTEM_ID .. tostring( nIndex ), dwSkillID, wLevel )
		
	end
	
	if false == bExpired then
	
		-- 타이머 초기화
		local nValue = fAge / fLife * 100
		SetDouble( WIDGET_ID_BUFFSYSTEM, "value", nValue, strSlot .. ".bar" )
		
		-- 타이머 실행
		local tbQuickSet = 
		{
			time = fAge * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_SystemBuff_Timer" .. tostring( nIndex )
		}
		SetTween( WIDGET_ID_BUFFSYSTEM, tbQuickSet, "value", "0", strSlot .. ".bar" )
	
	else
	
		SetDouble( WIDGET_ID_BUFFSYSTEM, "value", 100, strSlot .. ".bar" )
	
	end

end

-- 버프 삭제
function SystemBuff_Delete( nIndex )

	local nBuffIndex = 0
	
	for value in pairs( g_tbSystemBuff_SlotPos ) do
	
		if nIndex == g_tbSystemBuff_SlotPos[ value ] then
			
			g_tbSystemBuff_SlotPos[ value ] = nil
			nBuffIndex = value
			
			break
			
		end
	
	end
	
	if 0 == nBuffIndex then
		return
	end
	
	g_tbSystemBuff_List[ nBuffIndex ] = nil
	
	SystemBuff_AlignPos( nBuffIndex )

	-- 버프 슬롯 하나가 안사라지는 문제가 있다.
	-- 하나도 없으면 visible 다 끄자.
	if 0 == table.getn( g_tbSystemBuff_List ) then
	
		for i = 1, BUFF_SYSTEM_SLOT_TOTAL, 1 do
		
			local strSlot = "slotTimer" .. tostring( i )
			SetBool( WIDGET_ID_BUFFSYSTEM, "visible", false, strSlot )
			
		end
	
	end

end

-----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 이벤트
function Ev_SystemBuff_Timer1()
	SystemBuff_AlignPos( 1 )
end

function Ev_SystemBuff_Timer2()
	SystemBuff_AlignPos( 2 )
end

function Ev_SystemBuff_Timer3()
	SystemBuff_AlignPos( 3 )
end

function Ev_SystemBuff_Timer4()
	SystemBuff_AlignPos( 4 )
end

function Ev_SystemBuff_Timer5()
	SystemBuff_AlignPos( 5 )
end