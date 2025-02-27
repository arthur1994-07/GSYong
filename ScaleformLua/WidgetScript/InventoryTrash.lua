
-- 휴지통 UI 로드 완료
function LoadInventoryTrash()

	SetEvent( WIDGET_ID_INVENTORY_TRASH, EVENT_WIDGET_CLOSE, "CloseInventoryTrash", "", "" )

	-- 슬롯 ID 등록
	SetString( WIDGET_ID_INVENTORY_TRASH, "binding", SLOT_INVENTORY_TRASH_ID, "slot" )
	SetBool( WIDGET_ID_INVENTORY_TRASH, "holdable", true, "slot" )

	-- 텍스트 설정
	SetString( WIDGET_ID_INVENTORY_TRASH, "text", GetGameInternalText( "ITEM_GARBAGE_INFO", 0 ), "label" )
	SetString( WIDGET_ID_INVENTORY_TRASH, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "btnOK" )
	SetString( WIDGET_ID_INVENTORY_TRASH, "label", GetGameWordText( "GAMBLE_OKCANCEL", 1 ), "btnCancel" )
	
	-- 확인/취소 버튼 이벤트
	SetEvent( WIDGET_ID_INVENTORY_TRASH, EVENT_BUTTON_CLICK, "Ev_InvenTrash_OK", "", "btnOK" )
	SetEvent( WIDGET_ID_INVENTORY_TRASH, EVENT_BUTTON_CLICK, "Ev_InvenTrash_Cancel", "", "btnCancel" )

end

-- 휴지통 오픈
function OpenInventoryTrash()

	-- 개조카드 열려있음 닫자
	if true == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		CloseWidget( WIDGET_ID_REBUILD_CARD )
	end

	-- 인벤토리 UI가 열려 있다면 창 위치 재정렬
	if true == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		
		local nInvenX = GetInt( WIDGET_ID_INVENTORY, "x", "" )
		local nInvenY = GetInt( WIDGET_ID_INVENTORY, "y", "" )
		local nInvenW = GetInt( WIDGET_ID_INVENTORY, "width", "" )
		
		local nWidth = GetInt( WIDGET_ID_INVENTORY_TRASH, "width", "" )
		
		local nNewX = nInvenX - nWidth
		if 0 > nNewX then
			nNewX = nInvenX + nInvenW
		end
		
		SetInt( WIDGET_ID_INVENTORY_TRASH, "x", nNewX, "" )
		SetInt( WIDGET_ID_INVENTORY_TRASH, "y", nInvenY, "" )
		
	end
	
	-- 슬롯에 등록된 아이템 초기화
	ResetSlot( SLOT_INVENTORY_TRASH_ID )
	
	SetFakeModal( true )

end

function CloseInventoryTrash()

	SetFakeModal( false )

end

-------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 아이템 버리기 서버 요청 결과
function IvenTrash_TrashResult( nError )

	if 0 == nError then			--	아이템 삭제에 실패.
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 0 ), TEXT_COLOR_RED )
		
	elseif 1 == nError then		--	아이템 사용에 성공.
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 1 ), TEXT_COLOR_RED )
		CloseWidget( WIDGET_ID_INVENTORY_TRASH )
		
	elseif 2 == nError then		--	아이템이 없음.
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 2 ), TEXT_COLOR_RED )
		CloseWidget( WIDGET_ID_INVENTORY_TRASH )
		
	elseif 3 == nError then		--	삭제할수 없는 아이템입니다.
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 3 ), TEXT_COLOR_RED )
		
	elseif 4 == nError then		--	소지금이 부족합니다.
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 4 ), TEXT_COLOR_RED )
		
	else
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 0 ), TEXT_COLOR_RED )
		
	end

end

-------------------------------------------------------------------------------------------------------------------

-- 확인 버튼 이벤트
function Ev_InvenTrash_OK()

	-- 아이템 버릴 수 있는지 확인
	local nError = g_MyChar:CheckTrashItem()
	
	-- 성공
	if 0 == nError then
		
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "ITEM_GARBAGE_OKQUESTION", 0 ),
			Ev_InvenTrash_TrashOK, Event_MsgBox_Close )
	
	-- 등록된 아이템 없음, 인벤에 아이템이 없음, 아이템을 찾을 수 없음.
	elseif 1 == nError or 2 == nError or 4 == nError then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 2 ), TEXT_COLOR_RED )
		
	-- 돈이 부족함.
	elseif 3 == nError then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMGARBAGE_RESULT_FB_MSG", 4 ), TEXT_COLOR_RED )
		
	end

end

-- 취소 버튼 이벤트
function Ev_InvenTrash_Cancel()
	CloseWidget( WIDGET_ID_INVENTORY_TRASH )
end

-- 아이템 버리기 확인 메시지 이벤트
function Ev_InvenTrash_TrashOK()

	-- 아이템 버리기 서버에 요청
	g_Msg:TrashItem()
	Event_MsgBox_Close()

end