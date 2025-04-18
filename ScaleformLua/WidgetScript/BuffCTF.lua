
-- 등록된 버프 실제 클라이언트 위치
g_tbCtfBuff_SlotPos = {}

-- 등록된 버프 리스트
g_tbCtfBuff_List = {}

g_bCtfBuff_IsOpen = false

-- CTF 버프 로드 완료
function LoadBuffCTF()

	g_bCtfBuff_IsOpen = true
	
	for i = 1, BUFF_CTF_SLOT_TOTAL, 1 do
	
		local strSlot = "slotTimer" .. tostring( i )
		
		SetBool( WIDGET_ID_BUFFCTF, "visible", false, strSlot )
		SetString( WIDGET_ID_BUFFCTF, "binding", SLOT_BUFF_CTF_ID .. tostring( i ), strSlot .. ".slot" )
		SetDouble( WIDGET_ID_BUFFCTF, "maximum", 100, strSlot .. ".bar" )
		
	end

end

function OpenBuffCTF()

	g_tbCtfBuff_List = {}
	
	for i = 1, BUFF_CTF_SLOT_TOTAL, 1 do

		local strSlot = "slotTimer" .. tostring( i )
		SetBool( WIDGET_ID_BUFFCTF, "visible", false, "" )
		
	end

	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		-- CTF 버프만
		if 5 == tbBuff[ 2 ] then
		
			CtfBuff_Reg( tbBuff[ 1 ], tbBuff[ 3 ], tbBuff[ 4 ], tbBuff[ 5 ], tbBuff[ 6 ] )
		
		end
	
	end

end

-- 빈 슬롯 찾기
function CtfBuff_FindEmptySlot( dwSkillID )

	for i = 1, BUFF_CTF_SLOT_TOTAL, 1 do
	
		if dwSkillID == g_tbCtfBuff_List[ i ] or nil == g_tbCtfBuff_List[ i ] then
			return i
		end
	
	end
	
	return -1

end

-- 버프 슬롯 위치 정렬
function CtfBuff_AlignPos( nIndex )

	if nIndex < BUFF_CTF_SLOT_TOTAL then
	
		local nTemp = nIndex + 1
		while nTemp < BUFF_CTF_SLOT_TOTAL do
		
			local nPrevIndex = nTemp - 1
			local strPrevSlot = "slotTimer" .. tostring( nPrevIndex )
			
			-- 다음으로 등록된 버프가 없으면 끝내기
			if nil == g_tbCtfBuff_List[ nTemp ] then
			
				-- 등록된 버프가 하나면 초기화
				if nTemp == nIndex + 1 then
				
					g_tbCtfBuff_SlotPos[ nPrevIndex ] = nil
					g_tbCtfBuff_List[ nPrevIndex ] = nil
					
				end
				
				SetBool( WIDGET_ID_BUFFCTF, "visible", false, strPrevSlot )
			
				break
				
			end
			
			-- 다음으로 등록된 버프가 있으면 한칸씩 당겨주기
			local strSlot = "slotTimer" .. tostring( nTemp )
			
			local nValue = GetDouble( WIDGET_ID_BUFFCTF, "value", strSlot .. ".bar" )
			
			local dwSlot = g_tbCtfBuff_SlotPos[ nTemp ]
			local dwSkillID = g_tbCtfBuff_List[ nTemp ]
			
			g_tbCtfBuff_SlotPos[ nPrevIndex ] = dwSlot
			g_tbCtfBuff_SlotPos[ nTemp ] = nil
			
			g_tbCtfBuff_List[ nPrevIndex ] = dwSkillID
			g_tbCtfBuff_List[ nTemp ] = nil
			
			-- 버프 남은 시간
			local fAge = g_MyChar:GetSkillFactAge( dwSkillID )
			
			-- 스킬 아이콘
			ResetSlot( SLOT_BUFF_CTF_ID .. tostring( nTemp ) )
			SetSlot_Skill( SLOT_BUFF_CTF_ID .. tostring( nPrevIndex ), dwSkillID )
			
			-- 타이머 초기화
			SetDouble( WIDGET_ID_BUFFCTF, "value", nValue, strPrevSlot .. ".bar" )
			
			-- 타이머 실행
			local tbQuickSet = 
			{
				time = fAge * 1000,
				delay = 0,
				ease = EASE_NONE,
				loop = false,
				onComplete = "Ev_CtfBuff_Timer" .. tostring( nPrevIndex )
			}
			SetTween( WIDGET_ID_BUFFCTF, tbQuickSet, "value", "0", strPrevSlot .. ".bar" )
			
			nTemp = nTemp + 1
		
		end
	
	else
	
		local strSlot = "slotTimer" .. tostring( nIndex )
		SetBool( WIDGET_ID_BUFFCTF, "visible", false, strSlot )
	
	end

end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 버프 등록
function CtfBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )

	if false == g_bCtfBuff_IsOpen then
		return
	end

	local nIndex = CtfBuff_FindEmptySlot( dwSkillID )
	if -1 == nIndex then
		return
	end
	
	local strSlot = "slotTimer" .. tostring( nIndex )
	
	if 	g_tbCtfBuff_SlotPos[ nIndex ] ~= dwSlot or
		g_tbCtfBuff_List[ nIndex ] ~= dwSkillID then
		
		-- 등록된 스킬 저장
		g_tbCtfBuff_SlotPos[ nIndex ] = dwSlot
		g_tbCtfBuff_List[ nIndex ] = dwSkillID
		
		-- visible on
		SetBool( WIDGET_ID_BUFFCTF, "visible", true, strSlot )

		-- 스킬 아이콘 등록
		SetSlot_SkillLevel( SLOT_BUFF_CTF_ID .. tostring( nIndex ), dwSkillID, wLevel )
		
	end
	
	-- 타이머 초기화
	local nValue = fAge / fLife * 100
	SetDouble( WIDGET_ID_BUFFCTF, "value", nValue, strSlot .. ".bar" )
	
	-- 타이머 실행
	local tbQuickSet = 
	{
		time = fAge * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_CtfBuff_Timer" .. tostring( nIndex )
	}
	SetTween( WIDGET_ID_BUFFCTF, tbQuickSet, "value", "0", strSlot .. ".bar" )

end

-----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 이벤트
function Ev_CtfBuff_Timer1()
	CtfBuff_AlignPos( 1 )
end

function Ev_CtfBuff_Timer2()
	CtfBuff_AlignPos( 2 )
end

function Ev_CtfBuff_Timer3()
	CtfBuff_AlignPos( 3 )
end