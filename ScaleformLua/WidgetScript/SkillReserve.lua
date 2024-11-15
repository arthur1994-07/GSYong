
-- 각 슬롯이 애니메이션 중인지 확인할 리스트
g_tbSkillReserve_IsAnim = {}

-- 모든 슬롯이 애니메이션 중일 때 스킬 예약이 날아오면 저장
g_tbSkillReserve_WaitQue = {}

-- 애니메이션 시작한 순서대로 저장
g_tbSkillReserve_FirstAnimOrder = {}
g_tbSkillReserve_SecondAnimOrder = {}

-- 스킬 사용 예약 로드
function LoadSkillReserve()

	-- 슬롯 바인딩, visible 끄기
	for i = 1, SKILL_RESERVE_SLOT_TOTAL, 1 do
	
		local strSlot = "mcSlot" .. tostring( i )
	
		SetString( WIDGET_ID_SKILL_RESERVE, "binding", 
			SLOT_SKILL_RESERVE_ID .. tostring( i ), strSlot .. ".slot" )
			
		SetBool( WIDGET_ID_SKILL_RESERVE, "visible", false, strSlot )
		
		-- 슬롯 개수만큼 초기화
		table.insert( g_tbSkillReserve_IsAnim, { -1, false } )
		
	end

end

----------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 스킬 예약
function SkillReserve_SetSkill( dwID )

	-- 애니메이션 사용 안하고 있는 슬롯 인덱스 찾기
	local nIndex = -1
	for value in pairs( g_tbSkillReserve_IsAnim ) do
	
		if 	dwID ~= g_tbSkillReserve_IsAnim[ value ][ 1 ] and
			false == g_tbSkillReserve_IsAnim[ value ][ 2 ] then
			
			nIndex = value
			
			break
			
		end
	
	end
	
	-- 스킬 퀵슬롯 예약 아이콘 보여주기 위해 예약 스킬 ID 저장
	table.insert( g_tbSQS_SkillReserved, dwID )
	
	-- 스킬 사용 예약시 슬롯에 예약표기 UI 출력
	local strReserveIconInst = SQS_GetSkillReserveIconInst( dwID )
	if nil ~= strReserveIconInst then
	
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, strReserveIconInst )
		
	end
	
	-- 전부 사용하고 있으면 따로 저장해두고 애니메이션 끝나면 하나씩 넣기
	if -1 == nIndex then
	
		-- [shhan][2015.11.26] 주석처리 함.
		--					스킬딜레이 없애는 물약 먹고 스킬 난사 할 경우 정상 동작되지 않는다.
		--					퀵슬롯에 보이는 스킬예약되었다는 표시가 정상동작되지 않는다.
		--					이 작업은 안하면 문제가 사라진다.
		--					이 작업이 안된다 해서 특별히 문제가 될 것도 없어보인다.
		--
		--table.insert( g_tbSkillReserve_WaitQue, dwID )	

	else
		-- 슬롯 스킬 설정
		SetSlot_Skill( SLOT_SKILL_RESERVE_ID .. tostring( nIndex ), dwID )
		
		-- 애니메이션 초기화
		local strSlot = "mcSlot" .. tostring( nIndex )
		SetInt( WIDGET_ID_SKILL_RESERVE, "x", 18, strSlot )
		SetInt( WIDGET_ID_SKILL_RESERVE, "y", 0, strSlot )
		SetInt( WIDGET_ID_SKILL_RESERVE, "scaleX", 1, strSlot )
		SetInt( WIDGET_ID_SKILL_RESERVE, "scaleY", 1, strSlot )
		SetBool( WIDGET_ID_SKILL_RESERVE, "visible", true, strSlot )
		
		-- 애니메이션 시작 인덱스 넣기
		table.insert( g_tbSkillReserve_FirstAnimOrder, nIndex )
		
		-- 해당 인덱스 슬롯 애니메이션 중으로 설정
		g_tbSkillReserve_IsAnim[ nIndex ] = { dwID, true }
		
		-- 애니메이션 설정
		local strParam = "scaleX;scaleY"
		local strValue = "2;2;"
		local tbQuickSet = 
		{
			time = 250,
			delay = 0,
			ease = EASE_NONE,
			loop = true,		-- [shhan][2015.11.26] false -> true
								-- 스킬딜레이 없애는 물약 먹고 스킬 난사 할 경우 정상 동작되지 않는다.
								-- g_tbSkillReserve_FirstAnimOrder 값이 하나가 남는 현상이 생겨버린다.
								-- false -> true 로 변경하니 그런 현상이 사라졌다. 이유는 모름.
			onComplete = "Ev_SkillReserve_MoveToQuickSlot"
		}
		
		SetTween( WIDGET_ID_SKILL_RESERVE, tbQuickSet, strParam, strValue, strSlot )
	end

end

----------------------------------------------------------------------------------------------------
-- 이벤트

-- 스킬 사용한 큇슬롯으로 이동 애니메이션
function Ev_SkillReserve_MoveToQuickSlot()

	-- 시작한 순서대로 들어올테니 맨앞에서 하나씩 빼기
	local nIndex = g_tbSkillReserve_FirstAnimOrder[ 1 ]
	table.remove( g_tbSkillReserve_FirstAnimOrder, 1 )
	
	-- 애니메이션 시작 인덱스 넣기
	table.insert( g_tbSkillReserve_SecondAnimOrder, nIndex )
	
	local nX, nY = SQS_GetSkillSlotPosition( g_tbSkillReserve_IsAnim[ nIndex ][ 1 ] )
	
	if true == IsOpenWidget( WIDGET_ID_SKILLQUICKSLOT ) then
	
		nX = nX - GetInt( WIDGET_ID_SKILL_RESERVE, "x", "" ) + 18
		nY = nY - GetInt( WIDGET_ID_SKILL_RESERVE, "y", "" ) + 18
	
	end
	
	-- 애니메이션 설정
	local strParam = "x;y;scaleX;scaleY;"
	local strValue = tostring( nX ) .. ";" .. tostring( nY ) .. ";" .. "1;1;"
	local tbQuickSet = 
	{
		time = 250,
		delay = 1000,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_SkillReserve_CompleteAnim"
	}
	
	local strSlot = "mcSlot" .. tostring( nIndex )
	SetTween( WIDGET_ID_SKILL_RESERVE, tbQuickSet, strParam, strValue, strSlot )

end

-- 애니메이션 완료
function Ev_SkillReserve_CompleteAnim()

	-- 시작한 순서대로 들어올테니 맨앞에서 하나씩 빼기
	local nIndex = g_tbSkillReserve_SecondAnimOrder[ 1 ]
	table.remove( g_tbSkillReserve_SecondAnimOrder, 1 )
	
	-- 해당 인덱스 슬롯 애니메이션 끝남
	g_tbSkillReserve_IsAnim[ nIndex ] = { -1, false }
	
	-- 대기중인 스킬 애니메이션 해주기
	if 0 ~= table.getn( g_tbSkillReserve_WaitQue ) then
	
		local nSkillID = g_tbSkillReserve_WaitQue[ 1 ]
		table.remove( g_tbSkillReserve_WaitQue, 1 )
		
		SkillReserve_SetSkill( nSkillID )
	
	-- 대기중인게 없으면 visible 끄기
	else
	
		local strSlot = "mcSlot" .. tostring( nIndex )
		SetBool( WIDGET_ID_SKILL_RESERVE, "visible", false, strSlot )
		
	end

end