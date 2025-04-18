POINTSHOP_MAX_DISPLAY_ITEM = 20 							-- 최대 출력 가능한 아이템 갯수
--POINTSHOP_MAX_CART_ITEM = POINTSHOP_CART_SLOT_COUNT_TOTAL	-- 장바구니에 담을 수 있는 최대 아이템 갯수(수정시 클라이언트 내부 값도 같이 수정)
POINTSHOP_TIME_DISPLAY_CART_ERROR = 3000	-- N초 * 1000 , 카트 목록 가득 찰 시 에러메시지 출력 시간

g_PointShop_fElapsedTime = 0			
g_PointShop_bUpdateEvent = true 		-- 업데이트 할 이벤트가 존재

g_PointShop_strSearchText = ""			-- 검색에 포함할 텍스트
g_tbPointShop_SearchCategory = {}		-- 검색에 포함할 카테고리 리스트
g_tbPointShop_SearchSort = {}			-- 검색에 포함할 정렬 조건 리스트

g_PointShop_nSortState = 2				-- 아이템 목록 정렬 조건 0 : 내림차순 , 1 : 오름차순

g_tbPointShop_Item_All = {}				-- 전체 아이템 정보
g_tbPointShop_Item_Cart = {} 			-- 현재 카트 아이템 목록

g_tbPointShop_CategoryInfo = {} 		-- 카테고리 관련 정보

g_PointShop_nCurrentDisplayPage = 0 	-- 현재 선택된 페이지 0
g_tbPointShop_Page = {}					-- 출력 가능한 페이지 테이블	

g_PointShop_nCartItemPrice = 0			-- 카트에 담긴 물건의 총 가격

-- 인벤토리에 확장 버튼을 사용하기 위하여 존재
g_Inventory_ExtendButton = false		-- treu 일 경우 인벤토리 확장버튼을 이용하여 오픈한것

--이전 정보 저장 변수
g_tbPointShop_prevCategoryInfo = {}
g_PointShop_bSearchText = false
g_PointShop_bSelectAll = false

-- 강제 닫힘 (서버에서 데이터 다시 받아오기 위해)
g_bPointShop_ForceClosed = false

-- Npc Talk 에 있는 추천아이템의 이름이 담길 함수
g_tbNpcTalk_AD = ""

function LoadPointShop()

	--윈도우 타이틀
	SetString( WIDGET_ID_POINTSHOP, "title", GetGameInternalText( "POINT_SHOP_TITLE_TEXT"), "" )
	--검색 조건 텍스트
	SetString( WIDGET_ID_POINTSHOP, "text", g_PointShop_strSearchText, "ConditionList.ASCTB.textInput" )
	--검색 조건 체크박스
	SetString( WIDGET_ID_POINTSHOP, "text", GetGameInternalText("POINT_SHOP_RESEARCH_TEXT"), "ConditionList.cbSearchedText_text" )
	--검색 조건 버튼
	SetString( WIDGET_ID_POINTSHOP, "label", GetGameWordText("POINT_SHOP_SEARCH_BUTTON"), "ConditionList.btnSearchText" )
	
	-- 추후 스크립트에서 제거 되어야 함(플레시 파일 수정 후)
	SetBool( WIDGET_ID_POINTSHOP, "visible", false, "ErrorMsg" )
	
	-- 슬롯 바인딩
	for i = 1, POINTSHOP_SLOT_COUNT_TOTAL, 1 do
		SetString( WIDGET_ID_POINTSHOP, "binding", SLOT_POINTSHOP_ITEM_ID .. tostring( i ), "ItemList.".. "slot" .. tostring( i ) )
	end

	for i = 1, POINTSHOP_CART_SLOT_COUNT_TOTAL, 1 do
		SetString( WIDGET_ID_POINTSHOP, "binding", SLOT_POINTSHOP_CART_ID .. tostring( i ), "CartList.".. "slot" .. tostring( i ) )
	end
	
	--Event----------------------------
	--위젯 종료
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_WIDGET_CLOSE, "Event_PointShop_CloseWidget", "", "" )
	
	-- 초기화 버튼
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_btnReset", "", "btnReset" )
	
	-- 검색 등록
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_btnSearchText", "", "ConditionList.btnSearchText")
	
	-- 텍스트 검색 항목 포함
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_cbSearchText", "", "ConditionList.cbSearchedText")
	
	-- 전체 선택/해제
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_cbSelectAll", "", "ConditionList.cbSelectAll")
	
	-- 항목 체크 박스
	PointShop_GetItemInfo()
	for nIndex in pairs( g_tbPointShop_CategoryInfo ) do
		SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_SelectCategory", "", "ConditionList.checkBox"..tostring(nIndex) )
	end
	
	-- 정렬 버튼
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_btnSort", "", "ConditionList.btnSort")
	
	-- 아이템 페이지 변경
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_INDEX_CLICK_CHANGE, "Event_PointShop_ChangePage", "lastIndex;index;", "ItemList.optionStepper" )
	
	-- 카트 항목 비우기
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_btnDelCartList", "", "CartList.btnIcon")
	
	-- 구매 버튼
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_BUTTON_CLICK, "Event_PointShop_btnPurchase", "", "CartList.btnPurchase")
	
	-- 아이템 리스트 선택
	for i = 1, POINTSHOP_MAX_DISPLAY_ITEM, 1 do
	
		local strInst = "ItemList.item"..tostring( i )
		
		SetEvent( WIDGET_ID_POINTSHOP, EVENT_MOUSE_DOWN, "Event_PointShop_SelectedDisplayItem", 
			"buttonIdx", strInst, tostring( i ) )
			
		SetBool( WIDGET_ID_POINTSHOP, "TooltipEnabled", false, strInst )
		
	end

	-- 튤팁 설정
	SetString( WIDGET_ID_POINTSHOP, "TooltipText", GetGameInternalText( "POINT_SHOP_NOTIFY_CATEGORY_RELOAD" ), "btnReset" )
	SetString( WIDGET_ID_POINTSHOP, "TooltipText", GetGameInternalText( "POINT_SHOP_CART_ICON_HELP" ), "CartList.btnIcon" )
	
	-- 키보드 이벤트
	SetEvent( WIDGET_ID_POINTSHOP, EVENT_INPUT, "Ev_PointShop_KeyUp", "keyCode", "ConditionList.ASCTB" )
	
	local tbItemInfo = g_PointShop:GetItemList( "", tbCategoryList, tbSortList )	
	
	-- 아이템 갯수 확인
	local nIndex = 0
	local tbTextData = {}
	for nIndex in pairs(tbItemInfo) do
		local tbItem = tbItemInfo[nIndex]
		table.insert(tbTextData, {tbItem["strName"]} )
	end	
	
	SetDataEx( WIDGET_ID_POINTSHOP, "textdata;", tbTextData, "ConditionList.ASCTB" )
	
end

function OpenPointShop()

	--기본 UI 정보 설정(가변)
	-- 란 포인트
	PointShop_SetLabelRanPoint( g_MyChar:GetPointShopPoint() )	
	-- 구매버튼
	PointShop_SetLabelBtnPurchase()
	--정렬조건
	SetString( WIDGET_ID_POINTSHOP, "label", GetGameWordText("POINT_SHOP_PRICE_ORDER_TEXT", g_PointShop_nSortState), "ConditionList.btnSort" )
	--콤보 박스
	SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSearchText, "ConditionList.cbSearchedText" )
	SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSelectAll, "ConditionList.cbSelectAll" )
	
	if true == g_Inventory_ExtendButton then
		Event_Inventory_ExtendButton_Click()
	elseif "" ~= g_tbNpcTalk_AD then
		Event_PointShop_SetNpcTalkADItem()
	else
		--Event_PointShop_Else()
	end

	-- 포인트샾 출력에 필요한 정보 획득
	PointShop_Update()

end

---데이터 입출력 함수 ---
function PointShop_SetLabelRanPoint( nPoint )

	if false == IsOpenWidget( WIDGET_ID_POINTSHOP ) then
		return
	end
	
	local strRanPoint = GetGameWordText("POINT_SHOP_POINT_TEXT").." " .. 
		HTMLFontColor( CurrencyFormat( nPoint ), HTML_TEXT_COLOR_ORANGE )
		
	SetString( WIDGET_ID_POINTSHOP, "htmlText", strRanPoint, "CartList.labelRanPoint" )
	
end

function PointShop_SetLabelBtnPurchase()
	
	local nPrice = 0
	for nIndex in pairs(g_tbPointShop_Item_Cart) do
		local tbItem = g_tbPointShop_Item_Cart[nIndex]
		
		if nil ~= tbItem then
			nPrice = nPrice + tbItem["nPrice"]
		end
		
	end
	
	-- 텍스트 입력
	local nPriceCartItem = CurrencyFormat( nPrice )
	local strSpeace = ""
	if 0 < nPrice then
		--nPriceCartItem = CurrencyFormat( nPrice )
		strSpeace = "	"
	else
	    strSpeace = "     "
	end
	
	local strBtnPurchase = nPriceCartItem .. strSpeace .. GetGameWordText("POINT_SHOP_BUY_BUTTON_TEXT")
	SetString( WIDGET_ID_POINTSHOP, "label", strBtnPurchase, "CartList.btnPurchase" )
	
end

function PointShop_SetPageText( nSize )

	--카테고리 정보가 없다면 받아 온다
	if 0 == table.getn(g_tbPointShop_CategoryInfo) then
		PointShop_GetCategoryInfo()
	end

	-- 전체 아이템 갯수 획득
	local nListSize = nSize

	--페이지 수 계산
	local nMaxPage = nListSize / POINTSHOP_MAX_DISPLAY_ITEM
	if (nListSize % POINTSHOP_MAX_DISPLAY_ITEM) > 0 then
		 nMaxPage = nMaxPage + 1
	end

	--테이블 입력
	g_tbPointShop_Page = {}
	
	for i = 1, nMaxPage, 1 do
		local tbPageText = {}
		tbPageText[1] = tostring(i)
		table.insert( g_tbPointShop_Page, tbPageText)
	end

	SetDataEx( WIDGET_ID_POINTSHOP, "label;", g_tbPointShop_Page, "ItemList.optionStepper", false )
	
	--현재 페이지수 예외처리
	if g_PointShop_nCurrentDisplayPage > nMaxPage then
		g_PointShop_nCurrentDisplayPage = nMaxPage	
	end
	
	SetInt( WIDGET_ID_POINTSHOP, "selectedIndex", g_PointShop_nCurrentDisplayPage, "ItemList.optionStepper" )

end

function PointShop_SetCategoryInfo()

	-- 전체 선택 레이블
	SetString( WIDGET_ID_POINTSHOP, "text", GetGameWordText("POINT_SHOP_ALL_CATEGORY"), "ConditionList.cbSelectAll_text")
	
	local nCategoryCnt = 0
	for nIndex in pairs( g_tbPointShop_CategoryInfo ) do
		
		local tbCategory = g_tbPointShop_CategoryInfo[nIndex]
		if nil ~= tbCategory then
			
			-- 카테고리 레이블
			SetString( WIDGET_ID_POINTSHOP, "text", tostring(tbCategory["strName"]).."("..tostring(tbCategory["nSize"])..")"
			, "ConditionList.checkBox"..tostring(nIndex) .. "_text" )
			
			if true == tbCategory["bSelected"] then
				nCategoryCnt = nCategoryCnt + 1
			end
			
		end
	end
	
	--카테고리 선택
	--1. 초기 상태는 전체 선택과 동일하나 콤보 박스 체크는 하지 않는다
	--2. 검색어 포함 상태이나 검색어가 없는 경우는 전체선택으로 처리한다
	if nCategoryCnt < table.getn(g_tbPointShop_CategoryInfo) or true == g_PointShop_bSelectAll then
		for nIndex in pairs( g_tbPointShop_CategoryInfo ) do	
			local tbCategoryInfo = g_tbPointShop_CategoryInfo[nIndex]
			if true == tbCategoryInfo["bSelected"] then
				SetBool( WIDGET_ID_POINTSHOP, "selected", true, "ConditionList.checkBox"..tostring(nIndex) )
			else
				SetBool( WIDGET_ID_POINTSHOP, "selected", false, "ConditionList.checkBox"..tostring(nIndex) )
			end
			
		end
	end
		
end

function PointShop_GetCategorySize( nCategory )
	
	local tbCategory = {}
	tbCategory["nCategory"] = (nCategory - 1)
	local tbCategoryList = {}
	table.insert( tbCategoryList, tbCategory )
	
	local tbSortList = {}
	
	local tbItemInfo = g_PointShop:GetItemList( "", tbCategoryList, tbSortList )	
	
	-- 아이템 갯수 확인
	local nItemCount = 0
	for nIndex in pairs(tbItemInfo) do
		nItemCount = nItemCount + 1	
	end	
	
	return nItemCount
end

--UI 리로드 상태인지 초기상태인지에 따라 카테고리 선택상태 확인이 달라진다
function PointShop_GetCategorySelectedState( nIndex )
	
	--초기상태
	if 0 == table.getn(g_tbPointShop_prevCategoryInfo) then
		
		--카테고리 선택 상태
		if true == GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.checkBox"..tostring(nIndex) ) then
			return true
		end
	
	else--리로드
		local tbCategoryInfo = g_tbPointShop_prevCategoryInfo[nIndex]

		g_PointShop_strSearchText = ""
		g_PointShop_strSearchText = GetString(WIDGET_ID_POINTSHOP, "text", "ConditionList.ASCTB.textInput")
		if nil ~= tbCategoryInfo then
			 if true == g_PointShop_bSearchText and "" == g_PointShop_strSearchText then
				--tbCategoryInfo["bSelected"] = true 
				return true --예외 조건 텍스트가 없지만 강제로 검색을 선택 했을경우 전체 선택과 동일한 효과를 만든다
			else
				return tbCategoryInfo["bSelected"] 
			end
			
		end
	end
	
	return false
end

-- 로직 함수 --
function PointShop_Reset ()
	
	--체크박스 선택 해제
	local bSelectAll = false
	SetBool( WIDGET_ID_POINTSHOP, "selected", bSelectAll, "ConditionList.cbSearchedText" )
	SetBool( WIDGET_ID_POINTSHOP, "selected", bSelectAll, "ConditionList.cbSelectAll" )
	for nIndex in pairs( g_tbPointShop_CategoryInfo ) do
		SetBool( WIDGET_ID_POINTSHOP, "selected", bSelectAll, "ConditionList.checkBox"..tostring(nIndex) )
	end
	
	for i = 1, POINTSHOP_CART_SLOT_COUNT_TOTAL, 1 do
		ResetSlot( SLOT_POINTSHOP_CART_ID .. tostring( i ))
	end
	
	g_PointShop_fElapsedTime = 0			

	g_PointShop_strSearchText = ""				
	g_tbPointShop_SearchCategory = {}		
	g_tbPointShop_SearchSort = {}			
	g_PointShop_nSortState = 2				

	g_tbPointShop_Item_All = {}				
	g_tbPointShop_Item_Cart = {} 			

	g_tbPointShop_CategoryInfo = {} 			

	g_PointShop_nCurrentDisplayPage = 0 				
	g_tbPointShop_Page = {}						

	g_PointShop_nCartItemPrice = 0					
	
	g_tbPointShop_prevCategoryInfo = {}
	g_PointShop_bSearchText = false
	g_PointShop_bSelectAll = false
	g_Inventory_ExtendButton = false

	--검색 조건 텍스트
	SetString( WIDGET_ID_POINTSHOP, "text", g_PointShop_strSearchText, "ConditionList.ASCTB.textInput" )
	
	--정렬조건
	SetString( WIDGET_ID_POINTSHOP, "label", GetGameWordText("POINT_SHOP_PRICE_ORDER_TEXT", g_PointShop_nSortState), "ConditionList.btnSort" )
	
	g_PointShop:DelCartList()
	
	PointShop_Update()
		
end

-- 포인트샵 카트 업데이트
function PointShop_Update()
	PointShop_GetItemInfo()
	PointShop_DisplayItem()
	PointShop_UpdateCart()
	
	
	g_tbPointShop_prevCategoryInfo = g_tbPointShop_CategoryInfo
end

-- 클라이언트콜 에러 메시지별 동작 처리
-- -1 : 입력받은 아이템 가격 0원
--  0 : 카트 리스트 풀
--  1 : 정상 동작
function PointShop_HandleError( nError )

	if -1 == nError then
	
	elseif 0 == nError then
		-- 에러 메시지 입력
		local strResult = GetGameInternalText("POINT_SHOP_NOTIFY_FULL_CART_TEXT0") .. " \n" .. GetGameInternalText("POINT_SHOP_NOTIFY_FULL_CART_TEXT1")
		OpenMsgBox( MSG_BOX_TYPE_OK, strResult, Event_MsgBox_Close )
	elseif 1 == nError then
	
	-- 카트에서 항목이 제거될때 nError 에 nil 이 전송된다
	elseif nil == nError then
		local nIndex = table.getn(g_tbPointShop_Item_Cart)
		ResetSlot( SLOT_POINTSHOP_CART_ID .. tostring( nIndex + 1 ))	-- 현재 테이블의 개수를 기준으로 하여 삭제될 슬롯번호 책정
	end
end

function PointShop_GetCategoryInfo ()
	g_tbPointShop_CategoryInfo = {}
	
	--카테고리 리스트 명칭 획득
	local tbCategoryName = g_PointShop:GetCategoryList()
	--카테고리 리스트 기타 항목 추가
	table.insert( tbCategoryName, GetGameWordText("POINT_SHOP_ETC_CATEGORY") )
	
	local nSelectedCategoryCnt = 0
	--카테고리 정보 입력
	for nIndex in pairs(tbCategoryName) do
		local tbCategory = {}
		
		local bSelected = PointShop_GetCategorySelectedState( nIndex )
		if true == bSelected then
			nSelectedCategoryCnt = nSelectedCategoryCnt + 1
		end
		tbCategory["bSelected"] = bSelected;
		tbCategory["strName"] = tbCategoryName[nIndex]
		tbCategory["nSize"] = PointShop_GetCategorySize( nIndex )
		
		table.insert( g_tbPointShop_CategoryInfo, tbCategory )
		
	end
	
	--카테고리 선택 상태에 따른 추가 설정
	--아무것도 선택되지 않은 상태는 전체선택과 동일한 결과를 만든다
	if 0 == nSelectedCategoryCnt then
		for nIndex in pairs(g_tbPointShop_CategoryInfo) do
			local tbCategory = g_tbPointShop_CategoryInfo[nIndex]
			if nil ~= tbCategory then
				tbCategory["bSelected"] = true
			end	
		end
	end	
	
	-- 이전 상태 정보 사용 후 초기화
	g_tbPointShop_prevCategoryInfo = {} 
end

--검색 조건 획득
function PointShop_GetSearchCondition()	
	-- 텍스트 조건
	g_PointShop_strSearchText = ""
	if true == GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.cbSearchedText" ) then
		g_PointShop_strSearchText = GetString(WIDGET_ID_POINTSHOP, "text", "ConditionList.ASCTB.textInput")
	end

	--카테고리 조건
	g_tbPointShop_SearchCategory = {}
	for nIndex in pairs(g_tbPointShop_CategoryInfo) do
		
		local tbCategory = g_tbPointShop_CategoryInfo[nIndex]
		
		if nil ~= tbCategory then
			
			if true == tbCategory["bSelected"] then	
				local tbType = {}
				tbType["nCategory"] = (nIndex - 1) -- C++ 배열은 0부터
				table.insert( g_tbPointShop_SearchCategory, tbType )
			end
			
		end
		
	end
	
	-- 정렬 조건
	g_tbPointShop_SearchSort = {}
	local tbOrder = {}
	tbOrder["nOrder"] = g_PointShop_nSortState
	table.insert( g_tbPointShop_SearchSort, tbOrder )
	
end

-- 아이템 데이터 획득 및 출력에 필요한 요소별 분할
function PointShop_GetItemInfo ()
	--검색 조건 획득
	PointShop_GetCategoryInfo ()
	PointShop_GetSearchCondition()
		
	--실제 데이터 획득
	g_tbPointShop_Item_All = {}
	g_tbPointShop_Item_All = g_PointShop:GetItemList(g_PointShop_strSearchText, g_tbPointShop_SearchCategory, g_tbPointShop_SearchSort)	
			
end

--아이템 출력
function PointShop_DisplayItem()
	--초기 아이템 리스트 UI는 보이지 않는다
	for i = 1, POINTSHOP_SLOT_COUNT_TOTAL, 1 do
		SetBool( WIDGET_ID_POINTSHOP, "visible", 0, "ItemList.item"..tostring(i) )
		SetBool( WIDGET_ID_POINTSHOP, "visible", 0, "ItemList.slot"..tostring(i) )
	end
	
	--카테고리 선택 정보 초기화
	for nCategory in pairs(g_tbPointShop_CategoryInfo) do
		local tbCategoryInfo = g_tbPointShop_CategoryInfo[nCategory]
		if nil ~= tbCategoryInfo then
			tbCategoryInfo["bSelected"] = false
		end
	end
			
	--아이템 출력
	local tbDisplayItemName = {}													--출력 될 아이템 이름 저장
	local nOffset = g_PointShop_nCurrentDisplayPage * POINTSHOP_MAX_DISPLAY_ITEM	--어디서 부터 출력
	local nDisplayCnt = 0															--출력 되는 아이템 카운트

	-- 포인트 텍스트
	local strPoint = GetGameWordText( "MARKET_RPPAGE_TEXT", 33 )
	
	for nIndex in pairs(g_tbPointShop_Item_All) do
	
		local tbItem = g_tbPointShop_Item_All[nIndex]
		
		if nil ~= tbItem then
			nOffset = nOffset - 1
			if 0 > nOffset then
				
				nDisplayCnt = nDisplayCnt + 1
				if nDisplayCnt <= POINTSHOP_MAX_DISPLAY_ITEM then
				
					SetSlot_PointShop( SLOT_POINTSHOP_ITEM_ID .. tostring( nDisplayCnt ), 
						tbItem[ "wMID" ], tbItem[ "wSID" ], tbItem[ "nCount" ] )
					
					SetBool( WIDGET_ID_POINTSHOP, "visible", 1, "ItemList.item"..tostring(nDisplayCnt) )
					SetBool( WIDGET_ID_POINTSHOP, "visible", 1, "ItemList.slot"..tostring(nDisplayCnt) )

					local tbName =
					{
						tbItem["strName"],
						HTMLFontColor( CurrencyFormat( tbItem[ "nPrice" ] ) .. " " .. strPoint, HTML_TEXT_COLOR_ORANGE ),
					}
					table.insert( tbDisplayItemName, tbName )
					
				end	
				
			end--nOffset
			
			--카테고리 선택 확인 0 번은 중복 카테고리 아이템으로 그 외 아이템으로 판단한다
			if 0 ~= tbItem["nCategory"] then 
				local tbCategory = g_tbPointShop_CategoryInfo[tbItem["nCategory"]]
				if nil ~= tbCategory then
					tbCategory["bSelected"] = true	
				end
			end
			
		end
		
	end
	
	--아이템 명칭 입력
	local strParam = CreateParam( "label1", "label2" )
	SetDataEx(WIDGET_ID_POINTSHOP, strParam, tbDisplayItemName, "ItemList.list", false)
	
	--출력 가능 페이지 수 입력
	PointShop_SetPageText( nDisplayCnt + (g_PointShop_nCurrentDisplayPage * POINTSHOP_MAX_DISPLAY_ITEM))
	
	--카테고리 정보 입력
	PointShop_SetCategoryInfo()

end

function PointShop_UpdateCart()
	-- 초기화
	g_tbPointShop_Item_Cart = {}
	g_PointShop_nCartItemPrice = 0
	
	--데이터 입력
	g_tbPointShop_Item_Cart = g_PointShop:GetCartList()
	local nDisplayCnt = 1
	for nIndex in pairs(g_tbPointShop_Item_Cart) do
		
		local tbItem = g_tbPointShop_Item_Cart[nIndex]
		if nil ~= tbItem then
			SetSlot_PointShopCart( SLOT_POINTSHOP_CART_ID .. tostring( nDisplayCnt ), tbItem[ "wMID" ], tbItem[ "wSID" ], nDisplayCnt )
			g_PointShop_nCartItemPrice = g_PointShop_nCartItemPrice + tbItem[ "nPrice" ]
			nDisplayCnt = nDisplayCnt + 1
		end
	end
	
	PointShop_SetLabelBtnPurchase()
	
end

-- 포인트샵 카트 아이템 총 가격
function PointShop_GetCartItemPrice()

	return g_PointShop_nCartItemPrice

end


------EVENT------------------------------------------------------
--위젯 종료
function Event_PointShop_CloseWidget()
	--UI재오픈시 해당 정보를 입력하기 위함
	g_tbPointShop_prevCategoryInfo = g_tbPointShop_CategoryInfo
	g_PointShop_bSearchText = GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.cbSearchedText" )
	g_PointShop_bSelectAll = GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.cbSelectAll" )
	
	g_PointShop_strSearchText = {}
	g_PointShop_strSearchText = GetString(WIDGET_ID_POINTSHOP, "text", "ConditionList.ASCTB.textInput")
	
	g_Inventory_ExtendButton = false	-- 위젯 종료할때 인벤토리를 통해 연 정보 초기화
end

--리셋
function Event_PointShop_btnReset()
	PointShop_Reset()
end

--검색 버튼
function Event_PointShop_btnSearchText()
	
	g_PointShop_nCurrentDisplayPage = 0

	g_PointShop_strSearchText = ""
	g_PointShop_strSearchText = GetString(WIDGET_ID_POINTSHOP, "text", "ConditionList.ASCTB.textInput")
	
	--검색 할 키워드가 없다면 검색어 포함 카테고리을 선택하지 않는다
	if "" ~= g_PointShop_strSearchText then
		g_PointShop_bSearchText = true
	else
		g_PointShop_bSearchText = false
	end
	
	SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSearchText, "ConditionList.cbSearchedText" )	
	PointShop_Update()
	
end

--검색시 포함 체크
function Event_PointShop_cbSearchText()
	g_PointShop_bSearchText = GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.cbSearchedText" )
	
	if "" ~= GetString(WIDGET_ID_POINTSHOP, "text", "ConditionList.ASCTB.textInput") then
		g_PointShop_bSelectAll = true
		SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSelectAll, "ConditionList.cbSelectAll" )
		g_PointShop_nCurrentDisplayPage = 0
		PointShop_Update()
	end
end

--페이지 변경
function Event_PointShop_ChangePage( nLastIndex, nIndex )
	g_PointShop_nCurrentDisplayPage = nIndex
	PointShop_Update()
end

--전체 선택
function Event_PointShop_cbSelectAll()
	
	g_PointShop_bSelectAll = GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.cbSelectAll" )
	
	--모든 카테고리 선택
	for nIndex in pairs( g_tbPointShop_CategoryInfo ) do
		g_tbPointShop_CategoryInfo[nIndex]["bSelected"] = g_PointShop_bSelectAll
		SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSelectAll, "ConditionList.checkBox"..tostring(nIndex) )
	end
	
	g_PointShop_nCurrentDisplayPage = 0
	SetInt( WIDGET_ID_POINTSHOP, "selectedIndex", g_PointShop_nCurrentDisplayPage, "ItemList.optionStepper" )
	
	PointShop_Update()
	
end

--카테고리 선택 이벤트
function Event_PointShop_SelectCategory()
	
	--카테고리 선택을 정상적으로 하기 위해 검색 포함항목을 체크 해제 해준다(선택 항목)
	g_PointShop_bSearchText = false
	SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSearchText, "ConditionList.cbSearchedText" )
	
	g_PointShop_bSelectAll = false
	SetBool( WIDGET_ID_POINTSHOP, "selected", g_PointShop_bSelectAll, "ConditionList.cbSelectAll" )
	--
	for nIndex in pairs( g_tbPointShop_CategoryInfo ) do
		local tbCategoryInfo = g_tbPointShop_CategoryInfo[nIndex]
		tbCategoryInfo["bSelected"] = GetBool( WIDGET_ID_POINTSHOP, "selected", "ConditionList.checkBox"..tostring(nIndex) );
	end
	
	g_PointShop_nCurrentDisplayPage = 0
	SetInt( WIDGET_ID_POINTSHOP, "selectedIndex", g_PointShop_nCurrentDisplayPage, "ItemList.optionStepper" )
	
	PointShop_Update()
end

--정렬 버튼
function Event_PointShop_btnSort()
		
	if g_PointShop_nSortState >= 2 then
		g_PointShop_nSortState = 0
	else
		g_PointShop_nSortState = g_PointShop_nSortState + 1
	end 
	
	SetString( WIDGET_ID_POINTSHOP, "label", GetGameWordText("POINT_SHOP_PRICE_ORDER_TEXT", g_PointShop_nSortState), "ConditionList.btnSort" )
	
	PointShop_Update()
	
end

-- 카트 비우기
function Event_PointShop_btnDelCartList()
	for i = 1, POINTSHOP_CART_SLOT_COUNT_TOTAL, 1 do
		ResetSlot( SLOT_POINTSHOP_CART_ID .. tostring( i ))
	end
	g_PointShop:DelCartList()
	PointShop_Update()
end

--구매 버튼
function Event_PointShop_btnPurchase()
	local nResult = g_Msg:BuyCartListPointShop()
	g_PointShop:DelCartList()
	for i = 1, POINTSHOP_CART_SLOT_COUNT_TOTAL, 1 do
		ResetSlot( SLOT_POINTSHOP_CART_ID .. tostring( i ))
	end
	PointShop_Update()
end

-- 아이템 리스트 선택 이벤트 1~20
function Event_PointShop_SelectedDisplayItem( buttonIdx, strIndex )

	if 1 == buttonIdx then
	
		local nIndex = g_PointShop_nCurrentDisplayPage * POINTSHOP_SLOT_COUNT_TOTAL + (tonumber( strIndex ) - 1)
		g_PointShop:AddCartList( nIndex )
		
	end
	
end

--------------------------인벤토리 UI의 확장 버튼용 이벤트 모음--------------------------
-- 확장 버튼을 눌러 포인트샾 UI 오픈시
function Event_Inventory_ExtendButton_Click()
	SetString(WIDGET_ID_POINTSHOP, "text", GetGameWordText("INVENTORY_NAME_STATIC", 0), "ConditionList.ASCTB.textInput")
	Event_PointShop_btnSearchText()		-- ConditionList.textInput 에 입력된 조건으로 검색
end
-- 확장 버튼을 누르지 않고 포인트샾 UI 오픈시
function Event_PointShop_Else()
	PointShop_Reset()		-- UI 오픈시 포인트샾 검색정보를 리셋함(열때마다 적용)
end

------------------------ NPC Talk 에서 추천아이템 구입 버튼을 클릭하였을 경우----------------------
function Event_PointShop_SetNpcTalkADItem()
	SetString(WIDGET_ID_POINTSHOP, "text", g_tbNpcTalk_AD, "ConditionList.ASCTB.textInput")
	Event_PointShop_btnSearchText()		-- ConditionList.textInput 에 입력된 조건으로 검색
	g_tbNpcTalk_AD = ""
end
----------------------------------이벤트 모음--
-- 포인트샾 키보드 입력 함수
function Ev_PointShop_KeyUp( nKeyCode )

	if false == IsOpenWidget( WIDGET_ID_POINTSHOP ) then
		return
	end
	
	if KEYCODE_ENTER == nKeyCode then
		Event_PointShop_btnSearchText()
	end

end