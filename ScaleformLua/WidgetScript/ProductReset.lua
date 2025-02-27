
g_tbStudiedProduct = {}			-- 배운 제작 Type들
g_tbCheckedList = {}			-- 선택된 제작 List
g_nStudiedProduct = 0			-- 배운 제작 수
g_nResetAble = 0				-- 초기화 가능 수

g_wProductResetUseItemX = -1	-- 사용한 제작망각서 인벤위치
g_wProductResetUseItemY = -1	-- 사용한 제작망각서 인벤위치

g_nChecked = 0					-- 현재 체크한 수

function LoadProductReset()
	
	-- 하단 텍스트
	local strBottom = GetGameInternalText( "PRODUCT_RESET_BOTTOM_TEXT" )
	SetString( WIDGET_ID_PRODUCTRESET, "text", strBottom, "BottomText" )
	
	-- 확인버튼 텍스트
	local strOk = GetGameWordText( "MODAL_BUTTON", 2 )
	SetString( WIDGET_ID_PRODUCTRESET, "label", strOk, "BtnOK" )
	
	-- 취소버튼 텍스트
	local strNo = GetGameWordText( "MODAL_BUTTON", 3 )
	SetString( WIDGET_ID_PRODUCTRESET, "label", strNo, "BtnNo" )
	
	-- 체크 이벤트
	SetEvent( WIDGET_ID_PRODUCTRESET, EVENT_BUTTON_CLICK, "Event_ProductReset_btnCheck1", "", "CheckList1" )
	SetEvent( WIDGET_ID_PRODUCTRESET, EVENT_BUTTON_CLICK, "Event_ProductReset_btnCheck2", "", "CheckList2" )
	SetEvent( WIDGET_ID_PRODUCTRESET, EVENT_BUTTON_CLICK, "Event_ProductReset_btnCheck3", "", "CheckList3" )
	SetEvent( WIDGET_ID_PRODUCTRESET, EVENT_BUTTON_CLICK, "Event_ProductReset_btnCheck4", "", "CheckList4" )
	
	-- 버튼 이벤트
	SetEvent( WIDGET_ID_PRODUCTRESET, EVENT_BUTTON_CLICK, "Event_ProductReset_BtnOk", "", "BtnOK" )
	SetEvent( WIDGET_ID_PRODUCTRESET, EVENT_BUTTON_CLICK, "Event_ProductReset_BtnNo", "", "BtnNo" )
	
end

-- 제작 리셋 오픈
function OpenProductReset()

	-- 상단 텍스트
	local strTop = GetGameInternalText( "PRODUCT_RESET_TOP_TEXT" )
	strTop = StringFormat( strTop, g_nResetAble )
	SetString( WIDGET_ID_PRODUCTRESET, "text", strTop, "TopText" )

	for i = 1, 4, 1 do
	
		local strInst = "CheckList" .. tostring( i )
		
		SetBool( WIDGET_ID_PRODUCTRESET, "selected", false, strInst )
		
		if i <= g_nStudiedProduct then
		
			SetBool( WIDGET_ID_PRODUCTRESET, "visible", true, strInst )
			
			local nIndex = g_tbStudiedProduct[ i ] + 1
			local strCheck = GetGameWordText( "PRODUCT_TYPE_STRING", nIndex )
			SetString( WIDGET_ID_PRODUCTRESET, "text", strCheck, strInst .. "_text" )
		
		else
		
			SetBool( WIDGET_ID_PRODUCTRESET, "visible", false, strInst )
		
		end
	
	end

end

-- 제작 리셋 데이터 초기화
function ProductReset_Reset()

	g_tbStudiedProduct = {}
	g_tbCheckedList = { -1, -1, -1, -1 }
	
	g_nStudiedProduct = 0
	g_nResetAble = 0
	g_wProductResetUseItemX = -1
	g_wProductResetUseItemY = -1

	g_nChecked = 0
	
end

-- 체크 개수 업데이트
function ProductReset_UpdateCheckCount()

	g_nChecked = 0
	
	if GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList1" ) then
		g_nChecked = g_nChecked + 1
	end
	
	if GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList2" ) then
		g_nChecked = g_nChecked + 1
	end
	
	if GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList3" ) then
		g_nChecked = g_nChecked + 1
	end
	
	if GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList4" ) then
		g_nChecked = g_nChecked + 1
	end	
	
end

-- 체크된 리스트 업데이트
function ProductReset_UpdateCheckedList()

	if true == GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList1" ) then
		g_tbCheckedList[ 1 ] = g_tbStudiedProduct[ 1 ]
	end
	
	if true == GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList2" ) then
		g_tbCheckedList[ 2 ] = g_tbStudiedProduct[ 2 ]
	end
	
	if true == GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList3" ) then
		g_tbCheckedList[ 3 ] = g_tbStudiedProduct[ 3 ]
	end
	
	if true == GetBool( WIDGET_ID_PRODUCTRESET, "selected", "CheckList4" ) then
		g_tbCheckedList[ 4 ] = g_tbStudiedProduct[ 4 ]
	end
	
end

---------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 제작초기화 윈도우 열기
function ProductReset_Open( wAble, wPosX, wPosY )

	if true == IsOpenWidget( WIDGET_ID_PRODUCT ) then
		CloseWidget( WIDGET_ID_PRODUCT )
	end
	
	ProductReset_Reset()
	
	g_nResetAble = wAble
	g_wProductResetUseItemX = wPosX
	g_wProductResetUseItemY = wPosY
	
	g_tbStudiedProduct = g_Product:GetStudiedProducts()
	g_nStudiedProduct = table.getn( g_tbStudiedProduct )
	
	if false == IsOpenWidget( WIDGET_ID_PRODUCTRESET ) then
		OpenWidget( WIDGET_ID_PRODUCTRESET )
	else
		OpenProductReset()
	end
	
end

---------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인
function Event_ProductReset_BtnOk()

	ProductReset_UpdateCheckCount()
	ProductReset_UpdateCheckedList()

	if 0 < g_nChecked then
		g_Msg:ReqProductTypeDelete( g_tbCheckedList, g_wProductResetUseItemX, g_wProductResetUseItemY )
	end
	
	ProductReset_Reset()
	
	CloseWidget( WIDGET_ID_PRODUCTRESET )
	
end

-- 취소
function Event_ProductReset_BtnNo()

	CloseWidget( WIDGET_ID_PRODUCTRESET )
	ProductReset_Reset()
	
end

-- 1번 체크 
function Event_ProductReset_btnCheck1()

	ProductReset_UpdateCheckCount()
	
	if g_nResetAble < g_nChecked then
		SetBool( WIDGET_ID_PRODUCTRESET, "selected", false, "CheckList1" )
	end
	
end

-- 2번 체크 
function Event_ProductReset_btnCheck2()

	ProductReset_UpdateCheckCount()
	
	if g_nResetAble < g_nChecked then
		SetBool( WIDGET_ID_PRODUCTRESET, "selected", false, "CheckList2" )
	end
	
end

-- 3번 체크 
function Event_ProductReset_btnCheck3()

	ProductReset_UpdateCheckCount()
	
	if g_nResetAble < g_nChecked then
		SetBool( WIDGET_ID_PRODUCTRESET, "selected", false, "CheckList3" )
	end
	
end

-- 4번 체크 
function Event_ProductReset_btnCheck4()

	ProductReset_UpdateCheckCount()
	
	if g_nResetAble < g_nChecked then
		SetBool( WIDGET_ID_PRODUCTRESET, "selected", false, "CheckList4" )
	end
	
end