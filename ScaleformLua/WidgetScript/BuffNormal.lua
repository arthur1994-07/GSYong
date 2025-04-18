
-- 등록된 버프 클라이언트 실제 위치
g_nBuffNormal_SlotPos = {}

-- 등록된 버프 리스트
g_tbNormalBuff_List = {}

g_bNormalBuff_IsOpen = false

g_fBuffNormal_ElspTime = 0

g_nBuffNormal_OrigX = 0
g_nBuffNormal_OffsetX = 0

-- 일반 버프 로드 완료
function LoadBuffNormal()

	SetEvent( WIDGET_ID_BUFFNORMAL, EVENT_WIDGET_CLOSE, "CloseBuffNormal", "", "" )
	
	g_nBuffNormal_SlotPos = {}
	g_tbNormalBuff_List = {}

	for i = 1, BUFF_NORMAL_SLOT_TOTAL, 1 do
	
		local strSlot = "slotTimer" .. tostring( i )
		
		SetBool( WIDGET_ID_BUFFNORMAL, "visible", false, strSlot )
		SetBool( WIDGET_ID_BUFFNORMAL, "tooltipEnabled", false, strSlot .. ".slot" )
		SetInt( WIDGET_ID_BUFFNORMAL, "maximum", 100, strSlot .. ".bar" )
		SetString( WIDGET_ID_BUFFNORMAL, "binding", SLOT_BUFF_NORMAL_ID .. tostring( i ), strSlot .. ".slot" )
		
	end
	
	for value in pairs( WIDGETS_LIST ) do
	
		local tb = WIDGETS_LIST[ value ]
		if tb[ "wWidgetID" ] == WIDGET_ID_BUFFNORMAL then
		
			g_nBuffNormal_OrigX = tb[ "nOffsetX" ]
			break
		
		end
	
	end
	
end

function OpenBuffNormal()
	
	g_bNormalBuff_IsOpen = true
	
	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		-- 일반 버프만
		if 0 == tbBuff[ 2 ] then
		
			NormalBuff_Reg( tbBuff[ 1 ], tbBuff[ 3 ], tbBuff[ 7 ], tbBuff[ 4 ], tbBuff[ 5 ], tbBuff[ 6 ] )
		
		end
	
	end
	
	if 0 ~= table.getn( tbBuffList ) then
	
		InsertFrameMove( BuffNormal_FrameMove )
		
	else
	
		g_nBuffNormal_SlotPos = {}
		g_tbNormalBuff_List = {}
		
		for i = 1, BUFF_NORMAL_SLOT_TOTAL, 1 do
		
			local strSlot = "slotTimer" .. tostring( i )
			SetBool( WIDGET_ID_BUFFNORMAL, "visible", false, strSlot )
			
		end
	
	end
	
	BuffNormal_SetOffsetPosition()

end

function CloseBuffNormal()

	RemoveFrameMove( BuffNormal_FrameMove )
	
	g_bNormalBuff_IsOpen = false

end

-- 위치 오프셋 설정
function BuffNormal_SetOffsetPosition()

	if 0 == g_nBuffNormal_OffsetX then
		return
	end

	-- 극강부(무기 슬롯)이거나 마술부(소환수 슬롯)일 경우 추가 슬롯에 의하여 버트 UI 위치가 밀려난다;
	if	CHARCLASS_IDX_EXTREAM_M == g_MyChar.nClassIndex or
		CHARCLASS_IDX_EXTREAM_W == g_MyChar.nClassIndex or
		CHARCLASS_IDX_TRICKER_M == g_MyChar.nClassIndex or
		CHARCLASS_IDX_TRICKER_W == g_MyChar.nClassIndex then
	   
	   SetInt( WIDGET_ID_BUFFNORMAL, "x", g_nBuffNormal_OffsetX, "" )
		
	end

end

function BuffNormal_FrameMove( fElspTime )

	g_fBuffNormal_ElspTime = g_fBuffNormal_ElspTime + fElspTime
	if 0.1 > g_fBuffNormal_ElspTime then
		return
	end
	
	if ( #g_tbNormalBuff_List == 0 ) then
		
		if false == NormalBuff_CheckCount() then
			return
		end
		
	end
	
	g_fBuffNormal_ElspTime = 0

	for value in pairs( g_tbNormalBuff_List ) do
	
		local nSkillID = g_tbNormalBuff_List[ value ][ 1 ]
		local fLife = g_tbNormalBuff_List[ value ][ 2 ]
		
		local fAge = g_MyChar:GetSkillFactAge( nSkillID )
		
		local fRatio = fAge / fLife * 100
		SetDouble( WIDGET_ID_BUFFNORMAL, "value", fRatio, "slotTimer" .. tostring( value ) .. ".bar" )
	
	end

end

-- 버프 개수 체크
function NormalBuff_CheckCount()

	local nBuffCount = 0
	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		if 0 == tbBuff[ 2 ] then
		
			nBuffCount = nBuffCount + 1
		
		end
	
	end
	
	if 0 == nBuffCount then
	
		g_nBuffNormal_SlotPos = {}
		g_tbNormalBuff_List = {}
	
		for i = 1, BUFF_NORMAL_SLOT_TOTAL, 1 do
	
			local strSlot = "slotTimer" .. tostring( i )
			SetBool( WIDGET_ID_BUFFNORMAL, "visible", false, strSlot )
			
		end
		
		RemoveFrameMove( BuffNormal_FrameMove )
		
		return false
	
	end
	
	return true

end

-- 빈 슬롯 찾기
function NormalBuff_FindEmptySlot( dwSkillID )

	for i = 1, BUFF_NORMAL_SLOT_TOTAL, 1 do
	
		if nil == g_tbNormalBuff_List[ i ] or dwSkillID == g_tbNormalBuff_List[ i ][ 1 ] then
			return i
		end
	
	end
	
	return -1

end

-- 버프 슬롯 위치 정렬
function NormalBuff_AlignPos( nIndex )

	if ( nIndex <= 0 ) then
	
		PrintDebug( "Invalid BuffSlot Index!!!"..tostring( nIndex ) )
		
	end

	if nIndex < BUFF_NORMAL_SLOT_TOTAL then
	
		local nTemp = nIndex + 1
		for i = nTemp, BUFF_NORMAL_SLOT_TOTAL+1, 1 do
		
			local nPrevIndex = nTemp - 1
			local strPrevSlot = "slotTimer" .. tostring( nPrevIndex )
			
			-- 다음으로 등록된 버프가 없으면 끝내기
			if nil == g_tbNormalBuff_List[ nTemp ] then
			
				-- 등록된 버프가 하나면 초기화
				if nTemp == nIndex + 1 then
				
					g_tbNormalBuff_List[ nPrevIndex ] = nil
					g_nBuffNormal_SlotPos[ nPrevIndex ] = nil
					
				end
				
				SetBool( WIDGET_ID_BUFFNORMAL, "visible", false, strPrevSlot )
			
				break
				
			end
			
			-- 다음으로 등록된 버프가 있으면 한칸씩 당겨주기
			local strSlot = "slotTimer" .. tostring( nTemp )
			
			local nValue = GetInt( WIDGET_ID_BUFFNORMAL, "value", strSlot .. ".bar" )
			local dwSkillID = g_tbNormalBuff_List[ nTemp ][ 1 ]
			local fLifeTIme = g_tbNormalBuff_List[ nTemp ][ 2 ]
			local strName = g_tbNormalBuff_List[ nTemp ][ 3 ]
			local dwSlot = g_nBuffNormal_SlotPos[ nTemp ]
			
			if nil == g_tbNormalBuff_List[ nPrevIndex ] then
				g_tbNormalBuff_List[ nPrevIndex ] = {}
			end
			
			g_tbNormalBuff_List[ nPrevIndex ][ 1 ] = dwSkillID
			g_tbNormalBuff_List[ nPrevIndex ][ 2 ] = fLifeTIme
			g_tbNormalBuff_List[ nPrevIndex ][ 3 ] = strName
			g_tbNormalBuff_List[ nTemp ] = nil
			
			g_nBuffNormal_SlotPos[ nPrevIndex ] = dwSlot
			g_nBuffNormal_SlotPos[ nTemp ] = nil
			
			-- 버프 남은 시간
			local fAge = g_MyChar:GetSkillFactAge( dwSkillID )
			
			-- 스킬 아이콘
			ResetSlot( SLOT_BUFF_NORMAL_ID .. tostring( nTemp ) )
			SetSlot_Skill( SLOT_BUFF_NORMAL_ID .. tostring( nPrevIndex ), dwSkillID )
			
			-- 스킬 이름
			SetString( WIDGET_ID_BUFFNORMAL, "TooltipText", strName, strPrevSlot .. ".slot" )
			
			nTemp = nTemp + 1
		
		end
	
	else
	
		local strSlot = "slotTimer" .. tostring( nIndex )
		SetBool( WIDGET_ID_BUFFNORMAL, "visible", false, strSlot )
	
	end

end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 버프 등록
function NormalBuff_Reg( dwSlot, dwSkillID, strName, wLevel, fAge, fLife )

	if false == g_bNormalBuff_IsOpen then
		return
	end

	local nIndex = NormalBuff_FindEmptySlot( dwSkillID )
	if -1 == nIndex then
		return
	end
	
	local strSlot = "slotTimer" .. tostring( nIndex )
	
	-- 등록된 스킬 저장
	-- 같은 슬롯 위치에 같은 스킬이면 스킬 아이콘 설정할 필요 없다. (프레임 드랍 심해짐)
	if 	g_nBuffNormal_SlotPos[ nIndex ] ~= dwSlot or
		(nil ~= g_tbNormalBuff_List[ nIndex ] and
		g_tbNormalBuff_List[ nIndex ][ 1 ] ~= dwSkillID) then
		
		g_nBuffNormal_SlotPos[ nIndex ] = dwSlot
		g_tbNormalBuff_List[ nIndex ] = {}
		g_tbNormalBuff_List[ nIndex ][ 1 ] = dwSkillID
		g_tbNormalBuff_List[ nIndex ][ 2 ] = fLife
		g_tbNormalBuff_List[ nIndex ][ 3 ] = strName
		
		-- visible on
		SetBool( WIDGET_ID_BUFFNORMAL, "visible", true, strSlot )
		
		-- 스킬 아이콘 등록
		SetSlot_SkillLevel( SLOT_BUFF_NORMAL_ID .. tostring( nIndex ), dwSkillID, wLevel )
		
		-- 스킬 이름
		SetString( WIDGET_ID_BUFFNORMAL, "TooltipText", strName, strSlot .. ".slot" )
		
	end
	
	InsertFrameMove( BuffNormal_FrameMove )

end

-- 버프 삭제
function NormalBuff_Delete( nIndex )

	local nBuffIndex = 0
	
	for value in pairs( g_nBuffNormal_SlotPos ) do
	
		if nIndex == g_nBuffNormal_SlotPos[ value ] then
			
			g_nBuffNormal_SlotPos[ value ] = nil
			nBuffIndex = value
			
			break
			
		end
	
	end
	
	if 0 == nBuffIndex then
		return
	end
	
	g_tbNormalBuff_List[ nBuffIndex ] = nil
	NormalBuff_AlignPos( nBuffIndex )

end

-----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 이벤트
function Ev_NormalBuff_Timer1()
	NormalBuff_AlignPos( 1 )
end

function Ev_NormalBuff_Timer2()
	NormalBuff_AlignPos( 2 )
end

function Ev_NormalBuff_Timer3()
	NormalBuff_AlignPos( 3 )
end

function Ev_NormalBuff_Timer4()
	NormalBuff_AlignPos( 4 )
end

function Ev_NormalBuff_Timer5()
	NormalBuff_AlignPos( 5 )
end

function Ev_NormalBuff_Timer6()
	NormalBuff_AlignPos( 6 )
end

function Ev_NormalBuff_Timer7()
	NormalBuff_AlignPos( 7 )
end

function Ev_NormalBuff_Timer8()
	NormalBuff_AlignPos( 8 )
end

function Ev_NormalBuff_Timer9()
	NormalBuff_AlignPos( 9 )
end

function Ev_NormalBuff_Timer10()
	NormalBuff_AlignPos( 10 )
end

function Ev_NormalBuff_Timer11()
	NormalBuff_AlignPos( 11 )
end

function Ev_NormalBuff_Timer12()
	NormalBuff_AlignPos( 12 )
end

function Ev_NormalBuff_Timer13()
	NormalBuff_AlignPos( 13 )
end

function Ev_NormalBuff_Timer14()
	NormalBuff_AlignPos( 14 )
end