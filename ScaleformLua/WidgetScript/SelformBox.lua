
-- WIDGET_ID_SELFORMBOX
SELFORMBOX_SLOT_MAX = 30

g_nSelformBox_SelectPos = 0
g_tbSelformBox_ItemList = {}

function LoadSelformBox()

	-- 선택형 상저 설명, 선택, 취소;
	local str = HTMLFontColor( GetGameWordText( "SELFORMBOX_WINDOW_DESC_STATIC", 0 ), HTML_TEXT_COLOR_WHITE )
	SetString( WIDGET_ID_SELFORMBOX, "htmlText", str, "labelName" )
	SetString( WIDGET_ID_SELFORMBOX, "label", GetGameWordText( "SELFORMBOX_WINDOW_BTN", 0 ), "btnOK" )
	SetString( WIDGET_ID_SELFORMBOX, "label", GetGameWordText( "SELFORMBOX_WINDOW_BTN", 1 ), "btnCancel" )	
	
	SetEvent( WIDGET_ID_SELFORMBOX, EVENT_BUTTON_CLICK, "Ev_SelformBox_Select", "", "btnOK" )
	SetEvent( WIDGET_ID_SELFORMBOX, EVENT_BUTTON_CLICK, "Ev_SelformBox_Cancel", "", "btnCancel" )
	SetBool( WIDGET_ID_SELFORMBOX, "visible", false, "labelPMName")
	
	-- 슬롯 바인딩
	for i = 1, PRIVATE_MARKET_SLOT_COUNT_TOTAL, 1 do
		SetString( WIDGET_ID_SELFORMBOX, "binding", 
			SLOT_SELFORMBOX_ID .. tostring( i ), "Slot" .. tostring( i ) )
		
		SetEvent( WIDGET_ID_SELFORMBOX, EVENT_MOUSE_CLICK, "Ev_SelformBox_SlotClick", "buttonIdx", "Slot"..tostring(i),  tostring(i) )
	end
end

function Ev_SelformBox_SlotClick(nButtonIndex, nSlotIndex)
	
	local nSize = table.getn(g_tbSelformBox_ItemList)
	nSlotIndex = tonumber(nSlotIndex)
	if nButtonIndex ~= 0 or nSize < nSlotIndex then
		return
	end
	
	if g_nSelformBox_SelectPos ~= 0 then
		
		SetBool( WIDGET_ID_SELFORMBOX, "visible", false, "Slot"..tostring(g_nSelformBox_SelectPos) .. ".mcHighlight" )
		
	end
	
	g_nSelformBox_SelectPos = nSlotIndex
	SetBool( WIDGET_ID_SELFORMBOX, "visible", true, "Slot"..tostring(g_nSelformBox_SelectPos) .. ".mcHighlight" )
	
end

function OpenSelformBox()
end

function Ev_SelformBox_Select()
	
	if g_nSelformBox_SelectPos == 0 then
		return
	end
	
	OpenMsgBoxModaless( MSG_BOX_TYPE_OKCANCEL, GetGameWordText("SELFORMBOX_WINDOW_SELMODAL", 0)
		, Ev_SelformBox_ReqBuy, Event_MsgBoxModaless_Close )

end

function Ev_SelformBox_ReqBuy()
	
	-- 루아 테이블 및 슬롯 데이터는 1부터 시작함으로 -1해준다;
	g_MyChar:ReqSelformBoxSelect( g_nSelformBox_SelectPos-1 )
	CloseWidget(WIDGET_ID_SELFORMBOX)
	Event_MsgBoxModaless_Close()
	
end

function Ev_SelformBox_Cancel()
	CloseWidget(WIDGET_ID_SELFORMBOX)
end

function SelformBox_Reset()

	if g_nSelformBox_SelectPos ~= 0 then
		
		SetBool( WIDGET_ID_SELFORMBOX, "visible", false, "Slot"..tostring(g_nSelformBox_SelectPos) .. ".mcHighlight" )
		
	end

	g_nSelformBox_SelectPos = 0
	g_tbSelformBox_ItemList = {}
	
end

function SelformBox_SetBoxID( nItemMid, nItemSid )
	
	SelformBox_Reset()
	g_tbSelformBox_ItemList = GetItem_BoxItemlist( nItemMid, nItemSid )
	local nSize = table.getn(g_tbSelformBox_ItemList)
	
	-- 아래 상황이 발생하면 사이즈를 늘려야한다
	if nSize > SELFORMBOX_SLOT_MAX then
		nSize = SELFORMBOX_SLOT_MAX
	else
		for i = nSize + 1, SELFORMBOX_SLOT_MAX, 1 do
			ResetSlot( SLOT_SELFORMBOX_ID .. tostring( i ) )
		end	
	end
	
	for i = 1, nSize, 1 do

		local tbItem = g_tbSelformBox_ItemList[i]
		SetSlot_Item( SLOT_SELFORMBOX_ID .. tostring( i ), tbItem[1], tbItem[2], tbItem[3] )
		
	end
	
	OpenWidget( WIDGET_ID_SELFORMBOX )
end