
g_strSetItem_Name = ""
g_nSetItem_WoreCount = 0
g_nSetItem_TotalCount = 0
g_tbSetItem_List = {}

-- 세트 아이템 로드 완료
function LoadSetItem()

	-- 타이틀 이름 xml이 internal에 있다. word로 옮기고 WidgetsList에서 설정하는게 좋을 듯
	SetString( WIDGET_ID_SETITEM, "label", GetGameInternalText( "ITEM_INFO_SET_PREVIEW_TITLE", 0 ), "titleBtn" )
	
	-- 슬롯 바인딩
	for i = 1, SETITEM_SLOT_TOTAL, 1 do
		SetString( WIDGET_ID_SETITEM, "binding", SLOT_SETITEM_ID .. tostring( i ), "slot" .. tostring( i ) )
	end
	
	-- 리스트 스크롤 이벤트
	SetEvent( WIDGET_ID_SETITEM, EVENT_SCROLL, "Ev_SetItem_ListScroll", "", "list" )

end

-- 세트 아이템 오픈
function OpenSetItem()

	local str = string.format( "%s (%d/%d)", g_strSetItem_Name, g_nSetItem_WoreCount, g_nSetItem_TotalCount )
	SetString( WIDGET_ID_SETITEM, "text", str, "labelTitle" )
	
	local tbListData = {}
	
	for value in pairs( g_tbSetItem_List ) do
	
		local tbItem = g_tbSetItem_List[ value ]
		
		local strColor = HTML_TEXT_COLOR_GRAY
		if true == tbItem[ 4 ] then
			strColor = HTML_TEXT_COLOR_GREEN
		end
		
		local tbData = 
		{
			HTMLFontColor( tbItem[ 3 ], strColor )
		}
		
		table.insert( tbListData, tbData )
	
	end
	
	SetInt( WIDGET_ID_SETITEM, "scrollPosition", 0, "list" )
	SetDataEx( WIDGET_ID_SETITEM, "label", tbListData, "list", false )
	
	SetItem_SetSlot()

end

-- 슬롯 아이템 설정
function SetItem_SetSlot()

	local nScroll = GetInt( WIDGET_ID_SETITEM, "scrollPosition", "list" )
	
	for i = 1, SETITEM_SLOT_TOTAL, 1 do
	
		local tbItem = g_tbSetItem_List[ i + nScroll ]
		if nil == tbItem then
		
			SetBool( WIDGET_ID_SETITEM, "visible", false, "slot" .. tostring( i ) )
		
		else
		
			SetBool( WIDGET_ID_SETITEM, "visible", true, "slot" .. tostring( i ) )
			SetSlot_Item( SLOT_SETITEM_ID .. tostring( i ), tbItem[ 1 ], tbItem[ 2 ], 1 )
		
		end
	
	end

end

------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 세트 아이템 미리보기 오픈
function SetItem_Open( strSetItemName, nWoreCount, nTotal, tbItemList )

	g_strSetItem_Name = strSetItemName
	g_nSetItem_WoreCount = nWoreCount
	g_nSetItem_TotalCount = nTotal
	g_tbSetItem_List = tbItemList
	
	if false == IsOpenWidget( WIDGET_ID_SETITEM ) then
	
		OpenWidget( WIDGET_ID_SETITEM )
		
	else
	
		OpenSetItem()
	
	end

end

------------------------------------------------------------------------------------------------------------------
-- 이벤트
function Ev_SetItem_ListScroll()

	SetItem_SetSlot()

end