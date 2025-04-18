
-- 아이템 슬롯 개수
ACTIVITY_SALES_ITEM_SLOT_MAX = 8

-- 판매 아이템 데이터
g_tbActivitySales = {}

-- 판매 등급
g_nActivitySalesGrade = 0

-- 특별활동 판매 아이템 로드 완료
function LoadActivitySalesItem()

	-- 이벤트
	SetEvent( WIDGET_ID_ACTIVITY_SALES_ITEM, EVENT_BUTTON_CLICK, "Ev_ActivitySalesItem_Close", "", "closeButton" )
	SetEvent( WIDGET_ID_ACTIVITY_SALES_ITEM, EVENT_SCROLL, "Ev_ActivitySalesItem_Scroll", "", "list.list" )
	
	-- 슬롯 바인딩
	for i = 1, ACTIVITY_SALES_ITEM_SLOT_MAX, 1 do
	
		local strInstSlot = "list.slot" .. tostring( i )
		SetString( WIDGET_ID_ACTIVITY_SALES_ITEM, "binding", SLOT_ACTIVITY_SALES_ID .. tostring( i ), strInstSlot )
		
	end
	
	-- 닫기 버튼 텍스트
	SetString( WIDGET_ID_ACTIVITY_SALES_ITEM, "label", 
		GetGameWordText("ACTIVITY_SALESITEM_WINDOW_BUTTON"), "closeButton" )
	
end

-- 특별활동 판매 아이템 오픈
function OpenActivitySalesItem()

	-- 타이틀 설정
	local strTitle = GetGameInternalText( "ACTIVITY_SALESITEM_WINDOW_TEXT_DESC" .. tostring( g_nActivitySalesGrade ) )
	SetString( WIDGET_ID_ACTIVITY_SALES_ITEM, "text", strTitle, "textDesc" )
	
	-- 아이템이 하나도 없으면 리스트 삭제
	if 0 == table.getn( g_tbActivitySales ) then
	
		RemoveData( WIDGET_ID_ACTIVITY_SALES_ITEM, 0, "list.list", true )
		
	-- 아이템 리스트 설정
	else
	
		-- 특별 활동 포인트
		local nActivityPoint = g_Activity:GetActivityPoint( g_nActivitySalesGrade )
	
		local tbList = {}
		for value in pairs( g_tbActivitySales ) do
		
			local tbItem = g_tbActivitySales[ value ]
			
			-- 활동 포인트
			local strPoint = StringFormat( GetGameWordText( "ACTIVITY_SALESITEM_ITEM_TEXT", 1 ), 
				GetGameWordText( "ITEM_BASIC_ACT_POINT", tbItem[ 3 ] ),
				tbItem[ 4 ] )
			if tbItem[ 4 ] < nActivityPoint then
				strPoint = HTMLFontColor( strPoint, HTML_TEXT_COLOR_WHITE )
			else
				strPoint = HTMLFontColor( strPoint, HTML_TEXT_COLOR_RED )
			end
			
			-- 가격
			local strPrice = StringFormat( GetGameWordText( "ACTIVITY_SALESITEM_ITEM_TEXT", 0 ), CurrencyFormat( tbItem[5] ) )
			
			local tbData = 
			{
				tbItem[ 2 ],	-- 아이템 이름
				strPoint,		-- 활동 포인트
				strPrice,		-- 가격
			}
			
			table.insert( tbList, tbData )
		
		end
	
		local strParam = CreateParam( "label1", "label2", "label3" )
		SetDataEx( WIDGET_ID_ACTIVITY_SALES_ITEM, strParam, tbList, "list.list", false )
		
	end
	
	Ev_ActivitySalesItem_Scroll()

end

-- 특별활동 판매 아이템 열기
function ActivitySalesItem_Open( nGrade )

	-- 판매 등급 설정
	g_nActivitySalesGrade = nGrade
	
	-- 판매 아이템 데이터 가져오기
	g_tbActivitySales = g_Activity:GetActivitySalesItemList( g_nActivitySalesGrade )
	
	if false == IsOpenWidget( WIDGET_ID_ACTIVITY_SALES_ITEM ) then
	
		OpenWidget( WIDGET_ID_ACTIVITY_SALES_ITEM )
		
	else
	
		OpenActivitySalesItem()
	
	end
	
end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- UI 닫기
function Ev_ActivitySalesItem_Close()

	CloseWidget( WIDGET_ID_ACTIVITY_SALES_ITEM )
	
end

-- 리스트 스크롤
function Ev_ActivitySalesItem_Scroll()

	if nil == g_tbActivitySales then
		return
	end
	
	-- 리스트 스크롤 위치
	local nOffset = GetInt( WIDGET_ID_ACTIVITY_SALES_ITEM, "scrollPosition", "list.list" )
	
	for i = 1, ACTIVITY_SALES_ITEM_SLOT_MAX, 1 do
	
		local tbItem = g_tbActivitySales[ i + nOffset ]
		if nil ~= tbItem then
		
			-- 슬롯 아이템 설정
			SetSlot_Item_DWORD( SLOT_ACTIVITY_SALES_ID .. tostring( i ), tbItem[ 1 ], 1 )
			
		else
		
			ResetSlot( SLOT_ACTIVITY_SALES_ID .. tostring( i ) )
		
		end
		
	end
	
end