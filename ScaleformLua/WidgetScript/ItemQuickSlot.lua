
g_bIQS_SlotCollapse = false		-- 현재 펼쳐진 상태인가?
b_bIQS_Lock = false				-- 잠금 상태인가?

g_nIQS_LastUpdatedTrayIndex = -1

-- 드래그 관련 저장 값
g_nIQS_DragMouseX = 0
g_nIQS_DragMouseY = 0
g_nIQS_DragPosX = 0
g_nIQS_DragPosY = 0

-- 원본 사이즈
g_nIQS_OrigWidth = 0

-- 아이템 퀵슬롯 UI 로드 완료
function LoadItemQuickSlot()

	g_nIQS_OrigWidth = GetInt( WIDGET_ID_ITEMQUICKSLOT, "width", "quickSlot.slotItem" )
	
	-- 슬롯
	SetString( WIDGET_ID_ITEMQUICKSLOT, "binding", SLOT_IQS_LAST_ID, "quickSlot.slotLastItem.slot" )
	for i = 0, 5, 1 do
	
		local strSlotID = SLOT_IQS_ID .. tostring( i )
		SetString( WIDGET_ID_ITEMQUICKSLOT, "binding", strSlotID, "quickSlot.slotItem.slot" .. tostring( i ) )
			
		SetSlot_IQS( strSlotID, g_MyChar:GetItemQuick( i ), i )
		
	end
	
	SetSlot_IQS( SLOT_IQS_LAST_ID, g_MyChar:GetItemQuick( nIndex ), -1 )
	
	-- 아이템 단축키 슬롯 텍스트 변경
	EV_IQS_SetSlotText()
	-- 접기/펴기 버튼 이벤트
	SetEvent( WIDGET_ID_ITEMQUICKSLOT, EVENT_BUTTON_CLICK, "Ev_IQS_btnCollapse", "", "quickSlot.btnMC.btnCollapse" )
	
	-- 잠금 버튼 이벤트
	SetEvent( WIDGET_ID_ITEMQUICKSLOT, EVENT_BUTTON_CLICK, "Ev_IQS_btnLock", "", "quickSlot.btnLockMC.btnLock" )
	-- 드래그 시작 이벤트
	SetEvent( WIDGET_ID_ITEMQUICKSLOT, EVENT_MOUSE_DOWN, "Ev_IQS_DragStart", "stageX;stageY", "quickSlot.titleBtn" )
	-- 잠금 상태 설정
	SetBool( WIDGET_ID_ITEMQUICKSLOT, "selected", b_bIQS_Lock, "quickSlot.btnLockMC.btnLock" )

end

--------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 슬롯 업데이트
function IQS_UpdateSlot()

	for i = 0, 5, 1 do		
		SetSlot_IQS( SLOT_IQS_ID .. tostring( i ), g_MyChar:GetItemQuick( i ), i )		
	end

end

-- 트레이 슬롯 업데이트
function IQS_UpdateTraySlot( nIndex )

	if nil == nIndex then
	
		nIndex = g_nIQS_LastUpdatedTrayIndex
		
	else
	
		g_nIQS_LastUpdatedTrayIndex = nIndex
	
	end

	SetSlot_IQS( SLOT_IQS_LAST_ID, g_MyChar:GetItemQuick( nIndex ), -1 )
	
end

---------------------------------------------------------------------------------------------------
-- 이벤트

-- 슬롯 텍스트 갱신 함수
function EV_IQS_SetSlotText()
	local strSlotInst = ""
	
	for i = 1, 6, 1 do		
		local strKey = g_InputMan:GetShortcutsToString( g_tbHotKeyCfg_IndexType[ i ] )		
		SetString( WIDGET_ID_ITEMQUICKSLOT, "text", strKey, strSlotInst .. "quickSlot.slotItem.tf" .. tostring( i ) )
	end
end

-- 접기/펴기 버튼 이벤트
function Ev_IQS_btnCollapse()

	if false == g_bIQS_SlotCollapse then
	
		g_bIQS_SlotCollapse = true
		CallFunction( WIDGET_ID_ITEMQUICKSLOT, "gotoAndStop", "COLLAPSE", "quickSlot" )
		
		-- 스냅핑을 위해 width 없애버리자
		SetInt( WIDGET_ID_ITEMQUICKSLOT, "width", 1, "quickSlot.slotItem" )
	
	else
	
		g_bIQS_SlotCollapse = false
		CallFunction( WIDGET_ID_ITEMQUICKSLOT, "gotoAndStop", "EXTEND", "quickSlot" )
		
		-- width 원본 값
		SetInt( WIDGET_ID_ITEMQUICKSLOT, "width", g_nIQS_OrigWidth, "quickSlot.slotItem" )
	
	end

end

-- 잠금 버튼 이벤트
function Ev_IQS_btnLock()

	b_bIQS_Lock = GetBool( WIDGET_ID_ITEMQUICKSLOT, "selected", "quickSlot.btnLockMC.btnLock" )

end

-- 드래그 시작 이벤트
function Ev_IQS_DragStart( nStageX, nStageY )

	-- 언락일 경우만 드래그 허용
	if false == b_bIQS_Lock then
	
		InsertStageMouseMove( Ev_IQS_Dragging )
		InsertStageMouseUp( Ev_IQS_DragEnd )
		
		g_nIQS_DragMouseX = nStageX
		g_nIQS_DragMouseY = nStageY
		
		g_nIQS_DragPosX = GetInt( WIDGET_ID_ITEMQUICKSLOT, "x", "" )
		g_nIQS_DragPosY = GetInt( WIDGET_ID_ITEMQUICKSLOT, "y", "" )
	
	end

end

-- 드래그 이동
function Ev_IQS_Dragging( nStageX, nStageY )
	
	local strInst = ""
	if false == g_bIQS_SlotCollapse then
	
		strInst = "quickSlot.slotItem.bg"
	
	else
	
		strInst = "quickSlot.slotLastItem.bg"
	
	end
	
	local nOffsetX = nStageX - g_nIQS_DragMouseX
	local nOffsetY = nStageY - g_nIQS_DragMouseY
	local tb = SnapCheck( WIDGET_ID_ITEMQUICKSLOT, g_nIQS_DragPosX + nOffsetX, g_nIQS_DragPosY + nOffsetY )
	
	local nNewX = tb[ 1 ]
	local nNewY = tb[ 2 ]
	
	local nWidth = GetInt( WIDGET_ID_ITEMQUICKSLOT, "width", strInst )
	local nHeight = GetInt( WIDGET_ID_ITEMQUICKSLOT, "height", strInst )
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
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
	
	
	SetInt( WIDGET_ID_ITEMQUICKSLOT, "x", nNewX, "" )
	SetInt( WIDGET_ID_ITEMQUICKSLOT, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_IQS_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_IQS_Dragging )
	RemoveStageMouseUp( Ev_IQS_DragEnd )

end