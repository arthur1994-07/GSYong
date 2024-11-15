
-- 등록된 버프 클라이언트 실제 위치
g_nBuffAdd_SlotPos = -1
-- 등록된 버프 스킬 ID
g_nBuffAdd_SkillID = -1

g_bAddBuff_IsOpen = false

-- 추가 버프 로드 완료
function LoadBuffAdd()

	g_nBuffAdd_SlotPos = -1
	g_nBuffAdd_SkillID = -1

	g_bAddBuff_IsOpen = true
	
	SetBool( WIDGET_ID_BUFFADD, "visible", false, "slotTimer" )
	SetString( WIDGET_ID_BUFFADD, "binding", SLOT_BUFF_ADD_ID, "slotTimer.slot" )
	SetDouble( WIDGET_ID_BUFFADD, "maximum", 100, "slotTimer.bar" )
	
	local tbBuffList = g_MyChar:GetBuffList()
	for value in pairs( tbBuffList ) do
	
		local tbBuff = tbBuffList[ value ]
		-- 추가 버프만
		if 1 == tbBuff[ 2 ] then
		
			AddBuff_Reg( tbBuff[ 1 ], tbBuff[ 3 ], tbBuff[ 4 ], tbBuff[ 5 ], tbBuff[ 6 ] )
		
		end
	
	end

end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 버프 등록
function AddBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )

	if false == g_bAddBuff_IsOpen then
		return
	end

	if 	g_nBuffAdd_SlotPos ~= dwSlot or
		g_nBuffAdd_SkillID ~= dwSkillID then
		
		SetBool( WIDGET_ID_BUFFADD, "visible", true, "slotTimer" )
	
		g_nBuffAdd_SlotPos = dwSlot
		g_nBuffAdd_SkillID = dwSkillID
		
		-- 스킬 아이콘 등록
		SetSlot_SkillLevel( SLOT_BUFF_ADD_ID, g_nBuffAdd_SkillID, wLevel )
		
	end

	-- 타이머 초기화
	local nValue = fAge / fLife * 100
	SetDouble( WIDGET_ID_BUFFADD, "value", nValue, "slotTimer.bar" )
	
	-- 타이머 실행
	local tbQuickSet = 
	{
		time = fAge * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_AddBuff_Timer"
	}
	SetTween( WIDGET_ID_BUFFADD, tbQuickSet, "value", "0", "slotTimer.bar" )

end

-- 버프 삭제
function AddBuff_Delete( nIndex )

	g_nBuffAdd_SlotPos = -1
	g_nBuffAdd_SkillID = -1
	
	SetBool( WIDGET_ID_BUFFADD, "visible", false, "slotTimer" )

end

--------------------------------------------------------------------------------------------------------------
-- 이벤트 

-- 버프 시간 끝
function Ev_AddBuff_Timer()

	g_nBuffAdd_SlotPos = -1
	
	SetBool( WIDGET_ID_BUFFADD, "visible", false, "slotTimer" )

end