
g_bSQS_SlotCollapse = false	-- 슬롯 접혀있는가?
g_bSQS_SlotDirRow = false	-- 슬롯 방향
g_nSQS_SlotGroupIndex = 1	-- 슬롯 그룹

g_nSQS_MousePosX = 0
g_nSQS_MousePosY = 0
g_nSQS_XPos = 0
g_nSQS_YPos = 0
g_bSQSISMove = true

g_tbSQS_SkillReserved = {}

g_nSQS_OrigColHeight = 0
g_nSQS_OrigRowWidth = 0

-- 스킬 퀵슬롯 로드 완료
function LoadSkillQuickSlot()

	g_nSQS_OrigColHeight = GetInt( WIDGET_ID_SKILLQUICKSLOT, "height", "quickSlot.sqs_col" )
	g_nSQS_OrigRowWidth = GetInt( WIDGET_ID_SKILLQUICKSLOT, "width", "quickSlot.sqs_row" )

	g_bSQS_SlotDirRow = false
	
	SQS_Init( "sqs_col.col" )
	SQS_Init( "sqs_row.row" )
	
end

function OpenSkillQuickSlot()

	SetCharacterSlotInfo()
	
	if false == g_bSQS_SlotDirRow then
	
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, "quickSlot.sqs_col" )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, "quickSlot.sqs_row" )
		
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "height", g_nSQS_OrigColHeight, "quickSlot.sqs_col" )
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "width", 1, "quickSlot.sqs_row" )
	
	else
	
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, "quickSlot.sqs_col" )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, "quickSlot.sqs_row" )
		
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "height", 1, "quickSlot.sqs_col" )
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "width", g_nSQS_OrigRowWidth, "quickSlot.sqs_row" )
	
	end

end

-- 초기화
function SQS_Init( strInst )

	local strInstTray = ""
	local strInstSlotGroup = ""
	if g_bSQS_SlotDirRow == true then
	
		strInstTray = "sqs_row"
		strInstSlotGroup = "row"
		strInst = "sqs_row.row"
		
	else
	
		strInstTray = "sqs_col"
		strInstSlotGroup = "col"
		strInst = "sqs_col.col"
		
	end

	local strSlotInst = "quickSlot." .. strInst .. ".slotMC."
	local strLockBtnInst = "quickSlot." .. strInst .. ".btnLockMC.btnLock"
	local strCollapseBtnInst = "quickSlot." .. strInst .. ".btnCollapseMC.btnCollapse"

	-- 단축키 버튼 텍스트
	SetString( WIDGET_ID_SKILLQUICKSLOT, "label", 
		"F" .. tostring( g_nSQS_SlotGroupIndex ), strSlotInst .. "btnShortcut" )
	
	-- 최근 사용 스킬
	SetString( WIDGET_ID_SKILLQUICKSLOT, "binding", SLOT_SQS_LAST_ID, "quickSlot."..strInstTray..".slotLastSkill")--strInst..".slotLastSkill" )
	SetBool( WIDGET_ID_SKILLQUICKSLOT, "useCoolDown", true, "quickSlot."..strInstTray..".slotLastSkill")--strInst..".slotLastSkill" )
	SetSlot_SQS( SLOT_SQS_LAST_ID, g_MyChar:GetLastSkillID(), -1 )
	
	-- 슬롯 설정
	for i = 0, SKILL_QUICK_SLOT_COUNT_TOTAL - 1, 1 do
	
		local strSlotID = SLOT_SQS_ID .. tostring( i )
		local strSlot = strSlotInst .. "slot" .. tostring( i )
		SetString( WIDGET_ID_SKILLQUICKSLOT, "binding", strSlotID, strSlot )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "holdable", g_bSQSISMove, strSlot )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "useCoolDown", true, strSlot )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "iconForceUpdate", true, strSlot )
		
		SetEvent( WIDGET_ID_SKILLQUICKSLOT, EVENT_MOUSE_CLICK, "Ev_SQS_SlotClick", "", strSlot, tostring( i ) )
		
		local nIndex = ( g_nSQS_SlotGroupIndex - 1 ) * SKILL_QUICK_SLOT_COUNT_TOTAL + i
		SetSlot_SQS( strSlotID, g_MyChar:GetSkillQuick( nIndex ), nIndex )
		
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, strSlotInst .. "Reserved" .. tostring( i ) )
		
	end
	
	-- 슬롯 텍스트 설정
	SQS_SetSlotText()
	
	-- 이벤트
	SetEvent( WIDGET_ID_SKILLQUICKSLOT, EVENT_BUTTON_CLICK, "Ev_SQS_btnShortcut", "", strSlotInst .. "btnShortcut" )
	SetEvent( WIDGET_ID_SKILLQUICKSLOT, EVENT_BUTTON_CLICK, "Ev_SQS_btnRotate", "", strSlotInst .. "btnRotate" )
	SetEvent( WIDGET_ID_SKILLQUICKSLOT, EVENT_BUTTON_CLICK, "Ev_SQS_Collapse", "", strCollapseBtnInst )
	SetEvent( WIDGET_ID_SKILLQUICKSLOT, EVENT_MOUSE_DOWN, "SQS_Move", "stageX;stageY", "quickSlot." .. strInstTray..".title.titleBtn" )
	SetEvent( WIDGET_ID_SKILLQUICKSLOT, EVENT_MOUSE_DOWN, "SQS_IsMove", "", strLockBtnInst, strInst )
	
	SetBool( WIDGET_ID_SKILLQUICKSLOT, "selected", not g_bSQSISMove, strLockBtnInst )

end

-- 슬롯 텍스트 갱신 함수
function SQS_SetSlotText()

	local strSlotInst = ""
	if false == g_bSQS_SlotDirRow then
		strSlotInst = "quickSlot.sqs_col.col.slotMC."
	else
		strSlotInst = "quickSlot.sqs_row.row.slotMC."
	end
	
	for i = 0, 9, 1 do		
		local strKey = g_InputMan:GetShortcutsToString( g_tbHotKeyCfg_IndexType[ i + 7 ] )		
		SetString( WIDGET_ID_SKILLQUICKSLOT, "text", strKey, strSlotInst .. "tf" .. tostring( i + 1 ) )
	end
	
end

-- 슬롯 그룹 설정
function SQS_SetGroup( nIndex )

	g_nSQS_SlotGroupIndex = nIndex

	local strShortcutLabel = "F" .. tostring( g_nSQS_SlotGroupIndex )
	
	if false == g_bSQS_SlotDirRow then
		SetString( WIDGET_ID_SKILLQUICKSLOT, "label", strShortcutLabel, "quickSlot.sqs_col.col.slotMC.btnShortcut" )
	else
		SetString( WIDGET_ID_SKILLQUICKSLOT, "label", strShortcutLabel, "quickSlot.sqs_row.row.slotMC.btnShortcut" )
	end
	
	SQS_UpdateSlot()
	
	SQS_SkillReserveIconVisibleOff()
	
	for value in pairs( g_tbSQS_SkillReserved ) do
	
		local nID = g_tbSQS_SkillReserved[ value ]
		
		local strInst = SQS_GetSkillReserveIconInst( nID )
		if nil ~= strInst then
		
			SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, strInst )
		
		end
	
	end

end

-- 해당 스킬이 등록된 슬롯 위치
function SQS_GetSkillSlotPosition( dwID )

	local nIndex = -1
	for i = 0, SKILL_QUICK_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nTemp = ( g_nSQS_SlotGroupIndex - 1 ) * SKILL_QUICK_SLOT_COUNT_TOTAL + i
		
		if dwID == g_MyChar:GetSkillQuick( nTemp ) then
		
			nIndex = i
			break
		
		end
		
	end
	
	if -1 == nIndex then
		return 0, 0
	end
	
	local nX = GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", "" )
	local nY = GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", "" )
	
	local strSlotInst = ""
	if false == g_bSQS_SlotDirRow then
		strSlotInst = "quickSlot.sqs_col.col"
	else
		strSlotInst = "quickSlot.sqs_row.row"
	end
	
	nX = nX + GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", strSlotInst )
	nY = nY + GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", strSlotInst )
	
	nX = nX + GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", strSlotInst .. ".slotMC.slot" .. tostring( nIndex ) )
	nY = nY + GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", strSlotInst .. ".slotMC.slot" .. tostring( nIndex ) )
	
	return nX, nY

end

-- 예약 스킬 아이콘 인스턴스
function SQS_GetSkillReserveIconInst( dwID )

	local nIndex = -1
	for i = 0, SKILL_QUICK_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nTemp = (g_nSQS_SlotGroupIndex - 1) * SKILL_QUICK_SLOT_COUNT_TOTAL + i
		
		if dwID == g_MyChar:GetSkillQuick( nTemp ) then
		
			nIndex = i
			break
		
		end
		
	end
	
	if -1 == nIndex then
		return nil
	end
	
	local strSlotInst = ""
	if false == g_bSQS_SlotDirRow then
		strSlotInst = "quickSlot.sqs_col.col"
	else
		strSlotInst = "quickSlot.sqs_row.row"
	end
	
	return strSlotInst .. ".slotMC.Reserved" .. tostring( nIndex ) 
	
end

-- 캐릭터 슬롯 정보 세팅
function SetCharacterSlotInfo ()

	local strCharSlotType = ""
	local nCurrentCharSlot = g_MyChar:GetCurrentCharacterSlot()
	if 0 == nCurrentCharSlot then
	
		strCharSlotType = "A"
		
	elseif 1 == nCurrentCharSlot then
	
		strCharSlotType = "B"
		
	elseif 2 == nCurrentCharSlot then
	
		strCharSlotType = "C"
		
	elseif 3 == nCurrentCharSlot then
	
		strCharSlotType = "D"
		
	elseif 4 == nCurrentCharSlot then
	
		strCharSlotType = "E"
		
	end
	
	SetString( WIDGET_ID_SKILLQUICKSLOT, "text", strCharSlotType, "quickSlot.sqs_col.skill_type" )
	SetString( WIDGET_ID_SKILLQUICKSLOT, "text", strCharSlotType, "quickSlot.sqs_row.skill_type" )
	
end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 최근 사용한 스킬
function SQS_LastSkillID( nID )

	SetSlot_SQS( SLOT_SQS_LAST_ID, nID, -1 )
	
	-- 예약된 스킬이면 예약 아이콘 끄기
	local strReserveIconInst = SQS_GetSkillReserveIconInst( nID )
	if nil ~= strReserveIconInst then
	
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, strReserveIconInst )
		
	end
	
	for value in pairs( g_tbSQS_SkillReserved ) do
	
		if nID == g_tbSQS_SkillReserved[ value ] then
		
			table.remove( g_tbSQS_SkillReserved, value )
			break
		
		end
	
	end
	
end

-- 슬롯 업데이트
function SQS_UpdateSlot()

	SetCharacterSlotInfo()
	
	local strCharSlotType = ""
	local nCurrentCharSlot = g_MyChar:GetCurrentCharacterSlot()
	if 0 == nCurrentCharSlot then
	
		strCharSlotType = "A"
		
	elseif 1 == nCurrentCharSlot then
	
		strCharSlotType = "B"
		
	elseif 2 == nCurrentCharSlot then
	
		strCharSlotType = "C"
		
	elseif 3 == nCurrentCharSlot then
	
		strCharSlotType = "D"
		
	elseif 4 == nCurrentCharSlot then
	
		strCharSlotType = "E"
		
	end
	
	SetString( WIDGET_ID_SKILLQUICKSLOT, "text", strCharSlotType, "quickSlot.sqs_col.title" )

	for i = 0, SKILL_QUICK_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nIndex = ( g_nSQS_SlotGroupIndex - 1 ) * SKILL_QUICK_SLOT_COUNT_TOTAL + i
		SetSlot_SQS( SLOT_SQS_ID .. tostring( i ), g_MyChar:GetSkillQuick( nIndex ), nIndex )
		
	end

end

-- 슬롯 업데이트
function SQS_UpdateSlotIndex( nIndex )

	local nMin = ( g_nSQS_SlotGroupIndex - 1 ) * SKILL_QUICK_SLOT_COUNT_TOTAL
	local nMax = nMin + SKILL_QUICK_SLOT_COUNT_TOTAL
	
	if nMin > nIndex or nMax <= nIndex then
		return
	end
	
	local nSlotID = nIndex % SKILL_QUICK_SLOT_COUNT_TOTAL
	SetSlot_SQS( SLOT_SQS_ID .. tostring( nSlotID ), g_MyChar:GetSkillQuick( nIndex ), nIndex )

end

-- 스킬 예약 아이콘 visible 끄기
function SQS_SkillReserveIconVisibleOff()

	local strSlotInst = ""
	if false == g_bSQS_SlotDirRow then
		strSlotInst = "quickSlot.sqs_col.col"
	else
		strSlotInst = "quickSlot.sqs_row.row"
	end

	for i = 0, 9, 1 do
	
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, strSlotInst .. ".slotMC.Reserved" .. tostring( i ) )
		
	end

end

-- 스킬 예약 취소
function SQS_SkillReservationCancelAll()

	SQS_SkillReserveIconVisibleOff()
	
	-- 예약 스킬 전부 날리기
	g_tbSQS_SkillReserved = {}

end

-- 현재 스킬 탭 인덱스
function SQS_CurrentTab()
	return g_nSQS_SlotGroupIndex - 1
end

-- 스킬 퀵슬롯 탭당 개수
function SQS_TabSlotCount()
	return SKILL_QUICK_SLOT_COUNT_TOTAL
end

-- 스킬 퀵슬롯 무기 슬롯 스왑
function SQS_ArmSlotSwap( bMain )

	local nIndex = g_nSQS_SlotGroupIndex
	
	if true == bMain then
	
		if 3 == g_nSQS_SlotGroupIndex then
			nIndex = 1
		elseif 4 == g_nSQS_SlotGroupIndex then
			nIndex = 2
		end
	
	else
	
		if 1 == g_nSQS_SlotGroupIndex then
			nIndex = 3
		elseif 2 == g_nSQS_SlotGroupIndex then
			nIndex = 4
		end
	
	end
	
	SQS_SetGroup( nIndex )

end

--------------------------------------------------------------------------------------------------------------
-- 이벤트

-- F1 ~ 단축키
function Ev_SQS_btnShortcut()

	g_nSQS_SlotGroupIndex = g_nSQS_SlotGroupIndex + 1
	
	if 4 < g_nSQS_SlotGroupIndex then
		g_nSQS_SlotGroupIndex = 1
	end
	
	SQS_SetGroup( g_nSQS_SlotGroupIndex )

end

-- 회전
function Ev_SQS_btnRotate()

	if false == g_bSQS_SlotDirRow then
	
		g_bSQS_SlotDirRow = true
		
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, "quickSlot.sqs_col" )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, "quickSlot.sqs_row" )
		
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "height", 1, "quickSlot.sqs_col" )
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "width", g_nSQS_OrigRowWidth, "quickSlot.sqs_row" )
		
		SQS_Init( "sqs_row.row" )

		if true == g_bSQS_SlotCollapse then
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "COLLAPSE", "quickSlot.sqs_row.row" )
		else
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "EXTEND", "quickSlot.sqs_row.row" )
		end
		
	else
	
		g_bSQS_SlotDirRow = false

		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, "quickSlot.sqs_col" )
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", false, "quickSlot.sqs_row" )
		
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "height", g_nSQS_OrigColHeight, "quickSlot.sqs_col" )
		SetInt( WIDGET_ID_SKILLQUICKSLOT, "width", 1, "quickSlot.sqs_row" )
		
		SQS_Init( "sqs_col.col" )
		
		if true == g_bSQS_SlotCollapse then
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "COLLAPSE", "quickSlot.sqs_col.col" )
		else
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "EXTEND", "quickSlot.sqs_row.row" )
		end
		
	end
	
	local nNewX = GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", "" )
	local nNewY = GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", "" )
	SQS_SetMousPos( nNewX, nNewY )
	
	SetInt( WIDGET_ID_SKILLQUICKSLOT, "x", nNewX, "" )
	SetInt( WIDGET_ID_SKILLQUICKSLOT, "y", nNewY, "" )
	
	for value in pairs( g_tbSQS_SkillReserved ) do
	
		local nID = g_tbSQS_SkillReserved[ value ]
		
		local strInst = SQS_GetSkillReserveIconInst( nID )
		if nil ~= strInst then
		
			SetBool( WIDGET_ID_SKILLQUICKSLOT, "visible", true, strInst )
		
		end
	
	end
	
end

-- 접기/펴기 이벤트
function Ev_SQS_Collapse()

	if false == g_bSQS_SlotDirRow then
	
		if false == g_bSQS_SlotCollapse then
		
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "COLLAPSE", "quickSlot.sqs_col.col" )
			g_bSQS_SlotCollapse = true
			
		else
		
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "EXTEND", "quickSlot.sqs_col.col" )
			g_bSQS_SlotCollapse = false
		
		end
		
		g_nSQS_OrigColHeight = GetInt( WIDGET_ID_SKILLQUICKSLOT, "height", "quickSlot.sqs_col" )
		
	else
	
		if false == g_bSQS_SlotCollapse then
		
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "COLLAPSE", "quickSlot.sqs_row.row" )
			g_bSQS_SlotCollapse = true
		
		else
		
			CallFunction( WIDGET_ID_SKILLQUICKSLOT, "gotoAndStop", "EXTEND", "quickSlot.sqs_row.row" )
			g_bSQS_SlotCollapse = false
		
		end
		
		g_nSQS_OrigRowWidth = GetInt( WIDGET_ID_SKILLQUICKSLOT, "width", "quickSlot.sqs_row" )
		
	end
	
end

-- 스킬 슬롯 클릭
function Ev_SQS_SlotClick( strIndex )

	-- 잠김 상태일 경우 런스킬에 등록
	if false == g_bSQSISMove then
	
		g_Msg:ReqSkillRunSet( ( g_nSQS_SlotGroupIndex - 1 ) * 10 + tonumber( strIndex ) )
	
	end

end

-- 창위치이동
function SQS_Move( nStageX, nStageY )

	if g_bSQSISMove == true then
	
		InsertStageMouseMove( SQS_SetMousPos )
		InsertStageMouseUp( SQS_MoveEnd )
		
		g_nSQS_MousePosX = nStageX
		g_nSQS_MousePosY = nStageY
		
		g_nSQS_XPos = GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", "" )
		g_nSQS_YPos = GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", "" )
		
	end
	
end

function SQS_SetMousPos( nStageX, nStageY )

	local nWidth = 0
	local nHeight = 0
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	if false == g_bSQS_SlotDirRow then
	
		nWidth = GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", "quickSlot.sqs_col.col" )
		nHeight = GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", "quickSlot.sqs_col.col" )
		
		if true == g_bSQS_SlotCollapse then
		
			nWidth = nWidth + GetInt( WIDGET_ID_SKILLQUICKSLOT, "width", "quickSlot.sqs_col.col.bg" )
			nHeight = nHeight + GetInt( WIDGET_ID_SKILLQUICKSLOT, "height", "quickSlot.sqs_col.col.bg" )
		
		else
		
			nWidth = nWidth + GetInt( WIDGET_ID_SKILLQUICKSLOT, "width", "quickSlot.sqs_col.col.slotMC.bg" )
			nHeight = nHeight + GetInt( WIDGET_ID_SKILLQUICKSLOT, "height", "quickSlot.sqs_col.col.slotMC.bg" )
		
		end
		
	else

		nWidth = GetInt( WIDGET_ID_SKILLQUICKSLOT, "x", "quickSlot.sqs_row.row" )
		nHeight = GetInt( WIDGET_ID_SKILLQUICKSLOT, "y", "quickSlot.sqs_row.row" )
		
		if true == g_bSQS_SlotCollapse then
		
			nWidth = nWidth + GetInt( WIDGET_ID_SKILLQUICKSLOT, "width", "quickSlot.sqs_row.row.bg" )
			nHeight = nHeight + GetInt( WIDGET_ID_SKILLQUICKSLOT, "height", "quickSlot.sqs_row.row.bg" )
		
		else
		
			nWidth = nWidth + GetInt( WIDGET_ID_SKILLQUICKSLOT, "width", "quickSlot.sqs_row.row.slotMC.bg" )
			nHeight = nHeight + GetInt( WIDGET_ID_SKILLQUICKSLOT, "height", "quickSlot.sqs_row.row.slotMC.bg" )
		
		end
		
	end
	
	local nOffsetX = nStageX - g_nSQS_MousePosX
	local nOffsetY = nStageY - g_nSQS_MousePosY
	local tb = SnapCheck( WIDGET_ID_BASICINFO, g_nSQS_XPos + nOffsetX, g_nSQS_YPos + nOffsetY )
	
	local nNewX = tb[ 1 ]
	local nNewY = tb[ 2 ]
	
	if 0 > nNewX then
		nNewX = 0
	end
	
	if 0 > nNewY then
		nNewY = 0
	end
	
	if nScreenWidth < nNewX + nWidth then
		nNewX = nScreenWidth - nWidth
	end
	
	if nScreenHeight < nNewY + nHeight then
		nNewY = nScreenHeight - nHeight
	end
	
	
	SetInt( WIDGET_ID_SKILLQUICKSLOT, "x", nNewX, "" )
	SetInt( WIDGET_ID_SKILLQUICKSLOT, "y", nNewY, "" )
	
end

function SQS_MoveEnd( nStageX, nStageY )

	RemoveStageMouseMove( SQS_SetMousPos )
	RemoveStageMouseUp( SQS_MoveEnd )
	
end

function SQS_IsMove( strInst )

	g_bSQSISMove = not g_bSQSISMove
	
	for i = 0, SKILL_QUICK_SLOT_COUNT_TOTAL - 1, 1 do
		
		SetBool( WIDGET_ID_SKILLQUICKSLOT, "holdable", g_bSQSISMove,
			"quickSlot." .. strInst .. ".slotMC.slot" .. tostring( i ) )
			
	end

end


