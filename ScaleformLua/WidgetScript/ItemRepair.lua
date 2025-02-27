
-- 리스트 개수
ITEMREPAIR_COUNT = ITEMREPAIR_SLOT_TOTAL

--[[ 수리 가능한 아이템 리스트
{
	{
		1 : MID
		2 : SID
		3 : 아이템 이름
		4 : 아이템 강화 수치
		5 : 현재 내구도
		6 : 최대 내구도
		7 : 내구도 퍼센티지
		8 : 수리 비용
		9 : 장착 슬롯 위치
	}
}
]]
g_tbItemRepair_List = {}

-- 로드 완료
function LoadItemRepair()

	-- 장착된 아이템 중 수리할 아이템을 선택하세요.
	local str = GetGameWordText( "ITEM_REPAIR_UI", 1 )
	SetString( WIDGET_ID_ITEMREPAIR, "text", str, "labelTip" )
	
	-- 총 수리비용
	str = GetGameWordText( "ITEM_REPAIR_UI", 2 )
	SetString( WIDGET_ID_ITEMREPAIR, "text", str, "labelTotalStatic" )
	
	-- 수리 버튼
	str = GetGameWordText( "ITEM_REPAIR_UI", 3 )
	SetString( WIDGET_ID_ITEMREPAIR, "label", str, "btnRepair" )
	SetEvent( WIDGET_ID_ITEMREPAIR, EVENT_BUTTON_CLICK, "Ev_ItemRepair_Repair", "", "btnRepair" )
	
	-- 전체 수리 버튼
	str = GetGameWordText( "ITEM_REPAIR_UI", 4 )
	SetString( WIDGET_ID_ITEMREPAIR, "label", str, "btnRepairAll" )
	SetEvent( WIDGET_ID_ITEMREPAIR, EVENT_BUTTON_CLICK, "Ev_ItemRepair_RepairAll", "", "btnRepairAll" )
	
	-- 슬롯 바인딩
	for i = 1, ITEMREPAIR_COUNT, 1 do
	
		SetString( WIDGET_ID_ITEMREPAIR, "binding", SLOT_ITEMREPAIR_ID .. i, "slot" .. i )
		--HitTestEnable( WIDGET_ID_ITEMREPAIR, false, "slot" .. i )
		
	end
	
	-- 리스트
	SetEvent( WIDGET_ID_ITEMREPAIR, EVENT_SCROLL, "Ev_ItemRepair_Scroll", "", "list" )

end

-- 오픈
function OpenItemRepair()

	ItemRepair_Update()

end

-- 총 수리비용 설정
function ItemRepair_SetTotalCost()

	local nTotalCost = 0
	for value in pairs( g_tbItemRepair_List ) do
		nTotalCost = nTotalCost + g_tbItemRepair_List[ value ][ 8 ]
	end
	
	local str = CurrencyFormat( nTotalCost )
	SetString( WIDGET_ID_ITEMREPAIR, "text", str, "labelTotal" )

end

-- 리스트 설정
function ItemRepair_SetList()

	local tbList = {}
	for value in pairs( g_tbItemRepair_List ) do
	
		local tbItem = g_tbItemRepair_List[ value ]
		local tbListData = {}
		
		-- 아이템 이름
		-- 강화되었으면 강화 수치 붙이기
		--if 0 ~= tbItem[ 4 ] then
		--	tbListData[ 1 ] = "+" .. tbItem[ 4 ] .. " " .. tbItem[ 3 ]
		--else
		--	tbListData[ 1 ] = tbItem[ 3 ]
		--end
		
		-- 아이템 이름
		tbListData[ 1 ] = HTMLFontColor( tbItem[ 3 ], TranslateHTMLColor( tbItem[ 10 ] ) )
		
		-- 내구도
		tbListData[ 2 ] = HTMLFontColor( string.format( "%d/%d (%d%%)", tbItem[ 5 ], tbItem[ 6 ], tbItem[ 7 ] ), 
			TranslateHTMLColor( tbItem[ 11 ] ) )
		
		-- 수리 비용
		tbListData[ 3 ] = CurrencyFormat( tbItem[ 8 ] )
		
		table.insert( tbList, tbListData )
	
	end
	
	if 0 == table.getn( g_tbItemRepair_List ) then
	
		RemoveData( WIDGET_ID_ITEMREPAIR, 0, "list", true )
		
	else
	
		local strParam = CreateParam( "label1", "label2", "label3" )
		SetDataEx( WIDGET_ID_ITEMREPAIR, strParam, tbList, "list", false )
		
	end

end

-- 슬롯 업데이트
function ItemRepair_UpdateSlot()

	local nScrollPos = GetInt( WIDGET_ID_ITEMREPAIR, "scrollPosition", "list" )

	for i = 1, ITEMREPAIR_COUNT, 1 do
	
		local tbItem = g_tbItemRepair_List[ i + nScrollPos ]
		if nil == tbItem then
		
			SetBool( WIDGET_ID_ITEMREPAIR, "visible", false, "slot" .. i )
		
		else
		
			-- 슬롯 설정
			SetBool( WIDGET_ID_ITEMREPAIR, "visible", true, "slot" .. i )
			SetSlot_ItemRepair( SLOT_ITEMREPAIR_ID .. i, tbItem[ 9 ] )
			
		end
	
	end

end

----------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 업데이트
function ItemRepair_Update()

	if false == IsOpenWidget( WIDGET_ID_ITEMREPAIR ) then
		return
	end

	g_tbItemRepair_List = g_MyChar:GetRepairableItem()
	
	-- 리스트 설정
	ItemRepair_SetList()
	-- 슬롯 업데이트
	ItemRepair_UpdateSlot()
	-- 총 수리비용
	ItemRepair_SetTotalCost()

end

----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 수리
function Ev_ItemRepair_Repair()

	local nIndex = GetInt( WIDGET_ID_ITEMREPAIR, "selectedIndex", "list" )
	if -1 == nIndex then
		return
	end
	
	local nScrollPos = GetInt( WIDGET_ID_ITEMREPAIR, "scrollPosition", "list" )
	nIndex = nIndex + nScrollPos
	
	local tbItem = g_tbItemRepair_List[ nIndex + 1 ]
	if nil == tbItem then
		return
	end
	
	local tb = 
	{
		tbItem[ 9 ]
	}
	
	g_Msg:ReqItemRepair( tb )

end

-- 전체 수리
function Ev_ItemRepair_RepairAll()

	local tb = {}
	for value in pairs( g_tbItemRepair_List ) do
	
		local tbItem = g_tbItemRepair_List[ value ]
		table.insert( tb, tbItem[ 9 ] )
	
	end
	
	if 0 == table.getn( tb ) then
		return
	end
	
	g_Msg:ReqItemRepair( tb )

end

-- 리스트 스크롤
function Ev_ItemRepair_Scroll()

	ItemRepair_UpdateSlot()

end