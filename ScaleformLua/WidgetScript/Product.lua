PRODUCT_FILTER_COUNT_TOTAL 		= 10	-- 필터 갯수
PRODUCT_LIST_ITEM_COUNT_TOTAL 	= 12	-- 제작목록 라인 수
PRODUCT_TYPE_COUNT_TOTAL 		= 12	-- 제작타입 수
PRODUCT_MATERIAL_COUNT_TOTAL 	= 5		-- 제작재료 슬롯 수
-------------------------------------------------------------------------------------------
g_Product_tbFilterData 		= {}	-- 제작필터에 들어갈 스트링 테이블
g_Product_tbProductList 	= {}	-- 제작 목록 리스트
g_Pruduct_tbItemNameList	= {}	-- 제작 목록 이름 리스트, 위에 삽입하려하였으나 list에게 전송하는 데이터임으로 따로 구성한다
g_Product_tbProductData 	= {}	-- 선택한 레시피 데이타
g_Product_tbRequirment 		= {}	-- 요구조건
g_Product_tbProductType 	= {}	-- 제작 목록 리스트 상태
g_Product_wStudyProductType = 0		-- 배우기버튼 클릭한 제작타입
g_Product_wSelectedTap 		= 1		-- 선택된 제작타입
g_Product_wSelectedFilter	= 0		-- 선택 필터
g_Product_wSelectedProduct	= 0		-- 선택 아이템
--------------------------------------------------------------------------------------------

-- 제작 로드 완료
function LoadProduct()

	-- Initialize Data
	Product_InitData()
	Product_InitProductData()
	
	-- Filter Setting
	Product_SetFilter()
	
	-- Tap Setting
	Product_SetTapButton()
	
	-- Event Setting
	Product_SetProductEvent()

	-- UI Setting(Text,Check)
	Product_SetProductUI()	
	
	--CallFunction( WIDGET_ID_PRODUCT, "gotoAndStop", "select", "ProductTypeList.Tap" .. tostring( g_Product_wSelectedTap ) )
	SetBool( WIDGET_ID_PRODUCT, "selected", true, "ProductTypeList.Tap" .. tostring( g_Product_wSelectedTap ) )
	
	for i = 1, 5, 1 do
	
		SetEvent( WIDGET_ID_PRODUCT, EVENT_MOUSE_ROLLOVER, "Ev_Product_MaterialRollOver", "", "Goods.infoViewGroup.MaterialSlot" .. i, i )
		SetEvent( WIDGET_ID_PRODUCT, EVENT_MOUSE_ROLLOUT, "Ev_Product_MaterialRollOut", "", "Goods.infoViewGroup.MaterialSlot" .. i, i )
	
	end
	
	SetEvent( WIDGET_ID_PRODUCT, EVENT_WIDGET_CLOSE, "CloseProduct", "", "" )
	
	-- 키보드 이벤트
	SetEvent( WIDGET_ID_PRODUCT, EVENT_KEY_UP, "Ev_Product_KeyUp", "keyCode", "" )
	
end

-- 제작 오픈
function OpenProduct()

	g_Msg:ReqProductTypeList()

	Product_UpdateShowSkill()
	Product_UpdateList()
	
	-- 프로그레스바가 켜져있다면, 제작 진행중으로 본다
	if IsOpenWidget( WIDGET_ID_PRODUCT_PROGRESS ) == true then
		Product_EnableProductComponent(false)
	else
		Product_EnableProductComponent(true)
	end
end

function Product_EnableProductComponent(bEnable)

	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "Goods.infoViewGroup" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "ShowSkillCheckBox" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "ShowProductAbleCheckBox" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "TypeListTextBox" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "GoodsNameTextBox" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "StudyPointTextBox" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "ProductFirterDropDownMenu" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "btnSearchText" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "productList" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "productList.list" )
	SetBool( WIDGET_ID_PRODUCT, "enabled", bEnable, "ProductTypeList" )
	
	-- 프로그래스바가 열리고 닫혔는가에 따라 버튼 이름 변경 	
	if( bEnable == true ) then
		SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText( "PRODUCT_BUTTON", 0 ), "Goods.ProductionButton" )
	else
		SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText( "PRODUCT_BUTTON", 1 ), "Goods.ProductionButton" )
	end
	
end

-- 제작 닫기
function CloseProduct()

	Inventory_SetVisibleHighlight( false, -1 )

end

-- Initialize Data
function Product_InitData()

	g_Product_tbFilterData 		= {}
	g_Product_tbProductList 	= {}
	g_Product_tbProductType 	= {}
	g_Product_wStudyProductType = 0
	g_Product_wSelectedTap 		= 13 -- 기본 선택 제작타입(전체))
	g_Product_wSelectedProduct 	= -1
	
	Product_MatchingEnum( 0 )	
	
end

-- Initialize Product Data
function Product_InitProductData()
	g_Product_tbProductData	= {}
	g_Product_tbRequirment 	= {}
	
	for i = 1, PRODUCT_MATERIAL_COUNT_TOTAL, 1 do
	
		ResetSlot( SLOT_PRODUCT_MATERIAL_ID .. tostring(i) )
		SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.MaterialText" .. tostring(i) )
		
	end
	
	ResetSlot( SLOT_PRODUCT_ID )
	
	SetString( WIDGET_ID_PRODUCT, "text", "" , "GoodsNameTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.ReqSkillTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.ReqLevelTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.IsProductTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.GainSkillTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.Result.ProductTimeTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.ProductGoodsMiddleTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.CostSumTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.ResultNumTextBox" )
	
end

-- Filter Setting
function Product_SetFilter()

	if 0 < table.getn( g_Product_tbFilterData ) then
		return
	end
	
	local nLocalFilterCount = PRODUCT_FILTER_COUNT_TOTAL
	if g_wENABLE_NEW_CLASS <= 3 then 
		nLocalFilterCount = nLocalFilterCount -1
		 -- 변술부 제거
	end
	
	for i = 1, nLocalFilterCount, 1 do
		
		local tbData = {}	
		tbData[ 1 ] = GetGameWordText("PRODUCT_LIST_FILTER", i-1 )
		tbData[ 2 ] = i-1
	
		g_Product_tbFilterData[ i ] = tbData
		
	end
	
	SetDataEx( WIDGET_ID_PRODUCT, "label;titleIndex;", g_Product_tbFilterData, "ProductFirterDropDownMenu", false )	
	SetInt( WIDGET_ID_PRODUCT, "selectedIndex", 0, "ProductFirterDropDownMenu" )
	
end

-- Tap Setting
function Product_SetTapButton()

	if false == IsLoadWidget( WIDGET_ID_PRODUCT ) then
		return
	end
	
	for i = 1, PRODUCT_TYPE_COUNT_TOTAL, 1 do
	
		local wFrame = "StudyNeed"
		local bVisible = true
		
		g_Product_tbProductType[ i ] = g_Product:GetStudyState( i - 1 )
		
		local strInst = "ProductTypeList.ProductStudyButton" .. tostring( i )
		
		-- 아이콘 없음.
		if g_Product_tbProductType[ i ] == 0 then
		
			SetBool( WIDGET_ID_PRODUCT, "visible", false, strInst )
			
		else
		
			SetBool( WIDGET_ID_PRODUCT, "visible", true, strInst )
		
			-- 공개 제작 분야
			if 	g_Product_tbProductType[ i ] == 1 then
				
				wFrame = "StudyPublic"
				
			-- 학습 필요
			elseif g_Product_tbProductType[ i ] == 2 then
			
				wFrame = "StudyNeed"
				
			-- 학습 완료
			elseif g_Product_tbProductType[ i ] == 3 then
			
				wFrame = "Studied"
			
			elseif 	g_Product_tbProductType[ i ] == 4 or	-- 학습 비활성화
					g_Product_tbProductType[ i ] == 5 then	-- 학습 불가
			
				wFrame = "StudyUnable"
			
			end
			
			CallFunction( WIDGET_ID_PRODUCT, "gotoAndStop", wFrame, strInst )
			
		end

	end
	
	for i = 1, PRODUCT_TYPE_COUNT_TOTAL, 1 do
		HitTestEnable(WIDGET_ID_PRODUCT, false, "ProductTypeList.ProductTypeText"..tostring(i) )
	end

	Product_UpdateShowSkill()
end

-- Event Setting
function Product_SetProductEvent()
	
	for i = 1, PRODUCT_TYPE_COUNT_TOTAL, 1 do
	
		SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_ProductTypeStudy", "", 
			"ProductTypeList.ProductStudyButton" .. tostring( i ), tostring( i ) )
		SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_ProductTap", "", 
			"ProductTypeList.Tap" .. tostring( i ), tostring( i ) )
		
		SetString( WIDGET_ID_PRODUCT, "TooltipText",
			GetGameWordText( "PRODUCT_TYPE_STRING", i ), "ProductTypeList.Tap" .. tostring( i ) )
		
	end
	
	--for i = 1, PRODUCT_LIST_ITEM_COUNT_TOTAL, 1 do
		--SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_SelectProduct", "", 
		--	"productList.item" .. tostring( i ), tostring( i ) )
	--end
	
	SetEvent( WIDGET_ID_PRODUCT, EVENT_LIST_INDEX_CHANGE, "Ev_Product_SelectProduct", "index", "productList.list" )
	
	SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_ProductTap", "", "ProductTypeList.Tap13", "13" )
	
	SetString( WIDGET_ID_PRODUCT, "TooltipText",
		GetGameWordText( "PRODUCT_TYPE_STRING", 0 ), "ProductTypeList.Tap13" )
	
	SetEvent( WIDGET_ID_PRODUCT, EVENT_LIST_INDEX_CHANGE, "Ev_Product_ClassFirter", "index;", "ProductFirterDropDownMenu" )
	SetEvent( WIDGET_ID_PRODUCT, EVENT_SCROLL, "Ev_Product_Product_Scroll", "", "productList.list" )
	SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Product_UpdateShowSkill", "", "ShowSkillCheckBox" )
	SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_ProductAbleCheckBox", "", "ShowProductAbleCheckBox" )
	
	SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_Help_Over", "", "HelpBtn" )
	
	SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_Production", "", "Goods.ProductionButton" )
	-- 검색 등록
	SetEvent( WIDGET_ID_PRODUCT, EVENT_BUTTON_CLICK, "Ev_Product_btnSearchText", "", "btnSearchText")
end

-- UI Setting(Text,Check)
function Product_SetProductUI()
	-- 숙련도 보기
	SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_WINDOW_TEXT", 9) , "ShowSkillCheckBox_text"  )
	
	-- 제작가능항목 보기
	SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_WINDOW_TEXT", 2 ), "ShowProductAbleCheckBox_text"  )
	
	-- 제작목록 텍스트
	SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_TYPE_TEXT", 0 ) , "TypeListTextBox"  )
	
	-- 슬롯 바인딩 ID 설정
	strSlotInst = "Goods.infoViewGroup.MaterialSlot"
	for i = 1, 5, 1 do
		SetString( WIDGET_ID_PRODUCT, "binding", SLOT_PRODUCT_MATERIAL_ID .. tostring( i ), strSlotInst .. tostring( i ) )
	end
	
	SetString( WIDGET_ID_PRODUCT, "binding", SLOT_PRODUCT_ID, "Goods.infoViewGroup.Result.ResultItemSlot" )
	
	-- 필요물품
	local strTemp = "- " .. GetGameWordText("PRODUCT_WINDOW_TEXT", 6 ) .. " -"
	strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_GREENYELLOW )
	SetString( WIDGET_ID_PRODUCT, "htmlText", strTemp, "Goods.infoViewGroup.NeedMaterialTextBox" )
	
	-- 생산수량
	strTemp = " " .. GetGameWordText("PRODUCT_WINDOW_TEXT", 7 ) .. " :"
	strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_WHITE )
	SetString( WIDGET_ID_PRODUCT, "htmlText", strTemp, "Goods.infoViewGroup.ProductCountTextBox" )
	
	-- 소지금
	SetString( WIDGET_ID_PRODUCT, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "Goods.infoViewGroup.MoneyTextBox" )
	
	-- 제작하기 버튼
	SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText( "PRODUCT_BUTTON", 0 ), "Goods.ProductionButton" )
	-- 검색 버튼
	SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText("POINT_SHOP_SEARCH_BUTTON"), "btnSearchText" )
end

-----------------------------------------------------------------------------------------------------------------------------------------------
-- Update

function Product_Refresh()

	if false == IsOpenWidget( WIDGET_ID_PRODUCT ) then
		return
	end
	
	local nSelect = GetInt( WIDGET_ID_PRODUCT, "selectedIndex", "productList.list" )
	local nScroll = GetInt( WIDGET_ID_PRODUCT, "scrollPosition", "productList.list" )

	Product_UpdateProductList()
	
	SetInt( WIDGET_ID_PRODUCT, "selectedIndex", nSelect, "productList.list" )
	SetInt( WIDGET_ID_PRODUCT, "scrollPosition", nScroll, "productList.list" )
	
	Product_UpdateProductListScroll()
	Ev_Product_SelectProduct( nSelect )
	
	SetString( WIDGET_ID_PRODUCT, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "Goods.infoViewGroup.MoneyTextBox" )
end

function Product_UpdateType()

	Product_UpdateStudyPointText()
	Product_SetTapButton()
	Product_UpdateShowSkill()

end

function Product_RefreshGoods()	
	-- 텍스트
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.ReqLevelTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.ReqSkillTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.Result.GainSkillTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.Result.IsProductTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.Result.ProductTimeTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "" , "GoodsNameTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.ProductGoodsMiddleTextBox" )
	SetString( WIDGET_ID_PRODUCT, "text", "", "Goods.infoViewGroup.CostSumTextBox" )
	
	-- 슬롯
	ResetSlot( SLOT_PRODUCT_ID )
	
	for i = 1, PRODUCT_MATERIAL_COUNT_TOTAL, 1 do
	
		ResetSlot( SLOT_PRODUCT_MATERIAL_ID .. tostring( i ) )
		SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.MaterialText" .. tostring(i) )
		
	end
	
	-- 소지금
	SetString( WIDGET_ID_PRODUCT, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "Goods.infoViewGroup.MoneyTextBox" )
	
end

function Product_UpdateList()

	Product_UpdateStudyPointText()
	Product_UpdateProductList()
	Product_UpdateProductListScroll()
	Product_UpdateShowSkill()
	
end

function Product_UpdateStudyPointText()

	local nMaxPoint = g_Product:GetProductTypeMaxStudyPoint()
	local nCurrentPonit = g_Product:GetProductTypeStudyPoint()
	
	local strCombine = GetGameWordText( "PRODUCT_WINDOW_TEXT", 10 )
	strCombine = StringFormat( strCombine, nCurrentPonit, nMaxPoint - nCurrentPonit, nMaxPoint )
	strCombine = HTMLFontColor( strCombine, HTML_TEXT_COLOR_YELLOW )
	
	SetString( WIDGET_ID_PRODUCT, "htmlText", strCombine , "StudyPointTextBox"  )
	
end

-- 제작목록 테이블 갱신
function Product_UpdateProductList()

	local bShowProductAble = GetBool( WIDGET_ID_PRODUCT, "selected", "ShowProductAbleCheckBox" )
	local tbProduct = g_Product:GetProductList( g_Product_wSelectedTap-1,  g_Product_wSelectedFilter, bShowProductAble )
	
	-- 다음과 같이 깊은복사를 하여야 한다
	local tbProductListBackup = table.shallow_copy( g_Product_tbProductList )
	
	g_Product_tbProductList = {}
	g_Pruduct_tbItemNameList = {}
	
	for value in pairs( tbProduct ) do
	
		local tbObj = tbProduct[ value ]
		local strItem = tbObj[ "itemName" ]
		local tbNameData = { strItem, tbObj[ "nIndex" ] }
		table.insert(g_Pruduct_tbItemNameList, tbNameData)
		
		strItem = HTMLFontColor( strItem, TranslateHTMLColor( tbObj[ "dwColor" ] ) )
		
		if 0 < tbObj[ "nProductCount" ] then
		
			strItem = strItem .. 
				HTMLFontColor( " (" .. tostring( tbObj[ "nProductCount" ] ) .. ")", TranslateHTMLColor( 0xC0FF00 ) )
			
		end
		
		local tbData =
		{
			strItem,
		    tbObj[ "nStrCount" ],
		    tbObj[ "eState" ],
		    tbObj[ "dwColor" ],
		    tbObj[ "nProductCount" ],
		    tbObj[ "nIndex" ],
		}
		table.insert( g_Product_tbProductList, tbData )
	
	end
	
	-- 자동완성대화상자에 필요한 문자열들 설정;
	SetDataEx(WIDGET_ID_PRODUCT, "textdata;", g_Pruduct_tbItemNameList, "ProductAutoCompleteTB")

	-- 제작 리스트
	if table.getn(g_Product_tbProductList) == 0 then
	
		RemoveData( WIDGET_ID_PRODUCT, 0, "productList.list", true )
	
	else
	
		local strPamram = CreateParam( "label", "strCount", "nStrlen", "color", "nCount", "index" )	
		SetDataEx( WIDGET_ID_PRODUCT, strPamram, g_Product_tbProductList, "productList.list", false )
		
	end
	
	-- 현재 제대로 동작하지 않는다;
	
	-- 리스트 렌더러 데이터만 세팅한다, 그렇지않으면 아래에 세팅하지않는 데이터도 영향(기본값으로)받는다;
	-- list, UIComponent..로의 데이터는 유지시킨다
	--local tbWhereName = {"label", "strCount", "nStrlen", "dwColor", "nCount", "index"}
	--local tbWhereData = tbProductListBackup
	--local tbWhereType = {"-s", "-n", "-n", "-n", "-n", "-n"}
	--SetDataTerms( WIDGET_ID_PRODUCT, "productList.list"
	--					, tbWhereName, tbProductListBackup, tbWhereType
	--					, tbWhereName, g_Product_tbProductList, tbWhereType)
	-- 위에서 백업해서 사용하는 데이터의 경우 g_Product_tbProductList의 이전상태에 의존하는데;
	-- 해당 데이터는 ProductTypeList바뀌면 유효하지 않아진다(g_Product_tbProductList != tbProductListBackup)

end

-- 스크롤 이동시 제작목록 갱신
function Product_UpdateProductListScroll()

	local nOffset = GetInt( WIDGET_ID_PRODUCT, "scrollPosition", "productList.list" )
	
	for i = 1, PRODUCT_LIST_ITEM_COUNT_TOTAL, 1 do
	
		local strItemInst = "productList.item" .. tostring( i )
		local nIndex = i + nOffset
		
		if nIndex <= table.getn( g_Product_tbProductList ) then
			SetBool( WIDGET_ID_PRODUCT, "visible", true, strItemInst )
			
			local tbItem = g_Product_tbProductList[ nIndex ]
			local wFrame = "StateNothing"
			--재료 만족 여부
			local eState = tbItem[3]
			if eState == 0 then
				wFrame = "StateNothing"
			elseif eState == 1 then
				wFrame = "StatePart"
			elseif eState == 2 then
				wFrame = "StateEnough"
			elseif eState == 3 then
				wFrame = "StateNotLearn"
			end
			CallFunction( WIDGET_ID_PRODUCT, "gotoAndStop", wFrame, strItemInst .. ".status" )
			
		else
			--슬롯 감추기
			SetBool( WIDGET_ID_PRODUCT, "visible", false, strItemInst )
		end
		
	end
	
end

-- 숙련도
function Product_UpdateShowSkill()

	local bIsShowSkill = GetBool( WIDGET_ID_PRODUCT, "selected", "ShowSkillCheckBox" )
	
	for i = 1, PRODUCT_TYPE_COUNT_TOTAL, 1 do
	
		local wFrame = STR_STUDY_NEED
		local bVisible = true
		local bSkill = false
		
		if g_Product_tbProductType[i] == 1 then
		
			wFrame = STR_STUDY_PUBLIC
			bSkill = true
			
		end
		
		if g_Product_tbProductType[i] == 3 then
		
			wFrame = STR_STUDY_STUDIED
			bSkill = true
			
		end
		
		if bIsShowSkill and bSkill then
			bSkill = true
		else
			bSkill = false
		end
		
		local wSkill = g_Product:GetProductSkill( i-1 )
		SetString( WIDGET_ID_PRODUCT, "text", tostring(wSkill), "ProductTypeList.ProductTypeText" .. tostring( i ) )
		SetBool( WIDGET_ID_PRODUCT, "visible", bSkill, "ProductTypeList.ProductTypeText" .. tostring( i ) )
		
	end
	
end

-- 소지금
function Product_UpdateMoney( nMoney )

	if false == IsOpenWidget( WIDGET_ID_PRODUCT ) then
		return
	end

	SetString( WIDGET_ID_PRODUCT, "text", CurrencyFormat( nMoney ), "Goods.infoViewGroup.MoneyTextBox" )
	
end

-----------------------------------------------------------------------------------------------------------------------------------------------
-- Event

function Ev_Product_ProductTap( strType )
	local wType = tonumber( strType )

	if wType == g_Product_wSelectedTap then
	
		SetBool( WIDGET_ID_PRODUCT, "selected", true, "ProductTypeList.Tap" .. tostring( g_Product_wSelectedTap ) )
		return
		
	end
	
	SetBool( WIDGET_ID_PRODUCT, "selected", false, "ProductTypeList.Tap" .. tostring( g_Product_wSelectedTap ) )
	
	g_Product_wSelectedTap = wType
	
	
	if g_Product:IsUsableProductType( wType - 1 ) or wType == (PRODUCT_TYPE_COUNT_TOTAL+1) then
		SetBool( WIDGET_ID_PRODUCT, "visible", true, "Goods.ProductionButton" )
	else
		SetBool( WIDGET_ID_PRODUCT, "visible", false, "Goods.ProductionButton" )
	end	
	
	if wType == PRODUCT_TYPE_COUNT_TOTAL+1 then
		SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_TYPE_TEXT", 0 ) , "TypeListTextBox"  )
	else
		SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_TYPE_TEXT", wType ) , "TypeListTextBox"  )
	end
	g_Product_tbProductList = {}
	Product_Refresh()
end


function Refresh_ProductStudyTab() -- 제작 습득후 새로 고침을 위한 함수 

		SetBool( WIDGET_ID_PRODUCT, "selected", true, "ProductTypeList.Tap" .. tostring( g_Product_wSelectedTap ) )
		
		if g_Product:IsUsableProductType( g_Product_wSelectedTap - 1 ) or g_Product_wSelectedTap == (PRODUCT_TYPE_COUNT_TOTAL+1) then
			SetBool( WIDGET_ID_PRODUCT, "visible", true, "Goods.ProductionButton" )
		else
			SetBool( WIDGET_ID_PRODUCT, "visible", false, "Goods.ProductionButton" )
		end	
		
		if g_Product_wSelectedTap == PRODUCT_TYPE_COUNT_TOTAL+1 then
			SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_TYPE_TEXT", 0 ) , "TypeListTextBox"  )
		else
			SetString( WIDGET_ID_PRODUCT, "text", GetGameWordText( "PRODUCT_TYPE_TEXT", g_Product_wSelectedTap ) , "TypeListTextBox"  )
		end	
end


function Ev_Product_ProductTypeStudy( strStudy )

	local wStudy = tonumber( strStudy )

	if wStudy < 1 or wStudy > PRODUCT_TYPE_COUNT_TOTAL then
		g_Product_wStudyProductType = 0
		return
	end
	
	if g_Product_tbProductType[ wStudy ] == 2 then
	
		g_Product_wStudyProductType = wStudy - 1
		
		local str = StringFormat( GetGameInternalText("PRODUCT_WINDOW_STUDY_MSG"), GetGameWordText("PRODUCT_TYPE_STRING", wStudy) )			
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_Product_RequestProductTypeAdd, Event_MsgBox_Close )
		
	end
	
end

function Ev_Product_RequestProductTypeAdd()
	if g_Product_wStudyProductType < 1 or g_Product_wStudyProductType > (PRODUCT_TYPE_COUNT_TOTAL+1) then
		g_Product_wStudyProductType = 0
		return
	end
	g_Msg:ReqProductTypeAdd( g_Product_wStudyProductType )
end

function Ev_Product_ClassFirter( nIndex )	
	local nSelectedFilter = GetDataUInt( WIDGET_ID_PRODUCT, nIndex, "titleIndex", "ProductFirterDropDownMenu" )
	local strSelectedFilter = GetDataString( WIDGET_ID_PRODUCT, nIndex, "label", "ProductFirterDropDownMenu" )
	
	Product_MatchingEnum( nSelectedFilter )
	g_Product_tbProductList = {}
	Product_Refresh()
end

function Product_MatchingEnum( nSelectedFilter )

	if nSelectedFilter == 1 then
		g_Product_wSelectedFilter = 1
	elseif	 nSelectedFilter == 2 then
		g_Product_wSelectedFilter = 0
	elseif	 nSelectedFilter == 3 then
		g_Product_wSelectedFilter = 2
	elseif	 nSelectedFilter == 4 then
		g_Product_wSelectedFilter = 3
	elseif	 nSelectedFilter == 5 then
		g_Product_wSelectedFilter = 5
	elseif	 nSelectedFilter == 6 then
		g_Product_wSelectedFilter = 4
	elseif	 nSelectedFilter == 7 then
		g_Product_wSelectedFilter = 6
	elseif	 nSelectedFilter == 8 then
		g_Product_wSelectedFilter = 7
	elseif nSelectedFilter == 9 then
		g_Product_wSelectedFilter = 9
	else
		g_Product_wSelectedFilter = 8
	end
	
end

-- 완료시 페이지 갱신을 위해 Ev_Product_SelectProduct 콜되어 갱신이되지만;
-- 해당 완료는 서버에서 답장을 보냈을때 갱신이 된다; 하지만 참조값은;
-- 굳이 클라데이터 다시 참조할 이유가없으므로 UI의 ProductProgress의 데이터를 참조한다;
-- 이로인해 동기화가 안맞을 수 있다;
-- 상태진행바를 통해서도 갱신이되도록한다;
function Product_CountEdit(nCount, nCompeteCount)
	local strCount = tostring(g_ProductProgress_nProductCount-g_ProductProgress_nCompleteCount)
	SetString( WIDGET_ID_PRODUCT, "text", strCount, "Goods.infoViewGroup.ProductCountEdit" )
end

function Ev_Product_SelectProduct( nIndex )

	g_Product_wSelectedProduct = nIndex
	
	if -1 ==  nIndex then
		return
	end
	
	Product_InitProductData()
	
	local nOffset = GetDataInt( WIDGET_ID_PRODUCT, nIndex, "index", "productList.list" )
	if nOffset < table.getn( g_Product_tbProductList ) then
		
		g_Product_tbProductData = g_Product:GetSelectedProductData( nOffset )
		g_Product_tbRequirment = g_Product:GetSelectedProductRequirment( nOffset )
		
	end
	
	local nMaterial = table.getn( g_Product_tbProductData )
	if nMaterial <= 0 then
		return
	end
	
	-- 제작항목 관련 텍스트	
	local strText = ""
	
	if g_Product_tbRequirment[1] then
	
		-- 레벨
		strText = g_Product_tbRequirment[2]
		strText = HTMLFontColor( strText, HTML_TEXT_COLOR_RED )
		SetString( WIDGET_ID_PRODUCT, "htmlText", strText, "Goods.infoViewGroup.Result.ReqLevelTextBox" )
		
		-- 숙련도
		strText = g_Product_tbRequirment[3]
		strText = HTMLFontColor( strText, HTML_TEXT_COLOR_RED )
		SetString( WIDGET_ID_PRODUCT, "htmlText", strText, "Goods.infoViewGroup.Result.ReqSkillTextBox" )
		
		-- 경험치획득
		strText = g_Product_tbRequirment[4]
		strText = HTMLFontColor( strText, HTML_TEXT_COLOR_ORANGE )
		SetString( WIDGET_ID_PRODUCT, "htmlText", strText, "Goods.infoViewGroup.Result.GainSkillTextBox" )

	else
	
		-- 레시피 미습득
		strText = GetGameWordText( "PRODUCT_WINDOW_TEXT", 4 )
		strText = HTMLFontColor( strText, HTML_TEXT_COLOR_RED )
		SetString( WIDGET_ID_PRODUCT, "htmlText", strText , "Goods.infoViewGroup.Result.IsProductTextBox" )
		
	end
	
	-- 제작가능
	if g_Product_tbRequirment[5] then
		strText = GetGameWordText( "PRODUCT_WINDOW_TEXT", 3 )
		strText = HTMLFontColor( strText, "#0BFF00" )
		SetString( WIDGET_ID_PRODUCT, "htmlText", strText , "Goods.infoViewGroup.Result.IsProductTextBox" )
		-- 제작 가능일 경우 제작개수 자동 1 입력 및 입력란 활성화
		if IsOpenWidget(WIDGET_ID_PRODUCT_PROGRESS) == true then
			local strCount = tostring(g_ProductProgress_nProductCount-g_ProductProgress_nCompleteCount)
			SetString( WIDGET_ID_PRODUCT, "text", strCount, "Goods.infoViewGroup.ProductCountEdit" )
		else
			SetString( WIDGET_ID_PRODUCT, "text", "1" , "Goods.infoViewGroup.ProductCountEdit" )
		end
		
		--SetFocus( WIDGET_ID_PRODUCT, "Goods.infoViewGroup.ProductCountEdit" )
		-- 제작 개수 입력란에 입력된 문자(금액)길이를 가져와서 넣어줌
		local strParam = CreateParam( 0, string.len(GetString(WIDGET_ID_PRODUCT, "text", "Goods.infoViewGroup.ProductCountEdit")) )
		-- 제작 개수 입력란의 텍스트를 선택상태(마우스로 긁은상태)로 만드는 명령어
		CallFunction( WIDGET_ID_PRODUCT, "SetSelection", strParam, "Goods.infoViewGroup.ProductCountEdit" )
		
		-- 제작 가능하면 제작 시간을 출력한다;
		local strTimeText = GetGameWordText( "PRODUCT_WINDOW_TEXT", 5 )
		strTimeText = HTMLFontColor( strTimeText, HTML_TEXT_COLOR_WHITE )
		local strDelay = tostring(g_Product:GetSelectedProductDelay(nOffset)).." "..GetGameWordText( "ITEM_BASIC_TIME", 2 )
		strDelay = HTMLFontColor( strDelay, HTML_TEXT_COLOR_ORANGE  )
		strTimeText = strTimeText .. " " .. strDelay
		SetString( WIDGET_ID_PRODUCT, "htmlText", strTimeText , "Goods.infoViewGroup.Result.ProductTimeTextBox" )
		
	else
		SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.ProductCountEdit" )
		
		-- 제작 시간대신에 제작 확률로 변경됨
		local strRate = GetGameWordText( "PRODUCT_WINDOW_TEXT", 15 ) .. " "
		local strRateText = tostring( g_Product:GetSelectedProductRate( nOffset ) ) .. "%" 
		strRate = HTMLFontColor( strRate, HTML_TEXT_COLOR_WHITE )	
		strRateText = HTMLFontColor( strRateText, HTML_TEXT_COLOR_ORANGE )
		strText = strRate .. strRateText	
		SetString( WIDGET_ID_PRODUCT, "htmlText", strText , "Goods.infoViewGroup.Result.ProductTimeTextBox" )
	end
	
	-- 상단 텍스트(아이템 이름)
	local strItem = g_Product:GetSelectedProductName( nOffset )
	SetString( WIDGET_ID_PRODUCT, "text", strItem , "GoodsNameTextBox" )
	
	-- 하단 텍스트(msg)
	strText = HTMLFontColor( g_Product_tbRequirment["msg"], g_Product_tbRequirment["msgColor"] )
	SetString( WIDGET_ID_PRODUCT, "htmlText", strText, "Goods.infoViewGroup.ProductGoodsMiddleTextBox" )
	
	-- 제작비용
	local dwCost = g_Product:GetSelectedProductCost( nOffset )
	strText = HTMLFontColor( GetGameWordText("PRODUCT_WINDOW_TEXT", 8 ) .. " : " ..CurrencyFormat( dwCost ), HTML_TEXT_COLOR_ORANGE )
	SetString( WIDGET_ID_PRODUCT, "htmlText", strText, "Goods.infoViewGroup.CostSumTextBox" )
	
	-- 슬롯
	-- 제작 아이템 슬롯
	local tbResultData = g_Product:GetSelectedProductResultData(nOffset)
	local dwItemID, strNum = tbResultData[1], HTMLFontColor( tostring(tbResultData[2]), HTML_TEXT_COLOR_WHITE )
	
	SetSlot_Item_DWORD( SLOT_PRODUCT_ID, dwItemID, 1 )

	SetString( WIDGET_ID_PRODUCT, "htmlText", strNum, "Goods.infoViewGroup.Result.ResultNumTextBox" )
	
	-- 제작 재료 슬롯
	for i = 1, PRODUCT_MATERIAL_COUNT_TOTAL, 1 do
	
		local tbData = g_Product_tbProductData[ i ]
		if i < nMaterial+1 then
		
			SetSlot_Item_DWORD( SLOT_PRODUCT_MATERIAL_ID .. tostring(i), tbData[1], 1 )
			
			local strMaterial = tostring(tbData[3]) .. "/" .. tostring(tbData[2])
			
			if tbData[3] < tbData[2] then
				strMaterial = HTMLFontColor( strMaterial, HTML_TEXT_COLOR_RED )			
			else
				strMaterial = HTMLFontColor( strMaterial, HTML_TEXT_COLOR_WHITE )
			end
			
			SetString( WIDGET_ID_PRODUCT, "htmlText", strMaterial , "Goods.infoViewGroup.MaterialText" .. tostring(i) )
		else
		
			ResetSlot( SLOT_PRODUCT_MATERIAL_ID .. tostring(i) )
			SetString( WIDGET_ID_PRODUCT, "text", "" , "Goods.infoViewGroup.MaterialText" .. tostring(i) )
		end
		
	end
	
end
-- 제작하기 버튼 클릭시
function Ev_Product_Production()

	local dwKey = g_Product:GetSelectedProductKey( g_Product_wSelectedProduct )
	
	-- 제작
	if IsOpenWidget( WIDGET_ID_PRODUCT_PROGRESS ) == false then
	
		-- 입력된 개수
		local nCount = tonumber( GetString( WIDGET_ID_PRODUCT, "text", "Goods.infoViewGroup.ProductCountEdit" ) )
		
		-- 개수 입력 안됨
		if nCount == nil then
			return
		end
		
		if 0 < nCount then
			g_Msg:ReqProduct( dwKey, nCount )
		end
		
	-- 제작 진행중이라면 제작 취소
	else

		g_Msg:ReqProductCancel()
		
	end
	
end

function Ev_Product_Help_Over()
	
	local str = GetGameInternalText( "PRODUCT_WINDOW_HELP_TEXT_STUDY", 0 ) .. "\n\n"
	str = str .. GetGameInternalText( "PRODUCT_WINDOW_HELP_TEXT_LOCKED", 0 ) .. "\n\n"
	str = str .. GetGameInternalText( "PRODUCT_WINDOW_HELP_TEXT_SKILL", 0 )

	HelpTooltip_SetText( str )
	
end

function Ev_Product_Product_Scroll()
	Product_UpdateProductListScroll()
end

function Ev_Product_ProductAbleCheckBox()

	Product_Refresh()
	
end

function Ev_Product_MaterialRollOver( strIndex )

	local nIndex = tonumber( strIndex )
	
	local tbData = g_Product_tbProductData[ nIndex ]
	if nil == tbData then
	
		Inventory_SetVisibleMultiHighlight( false, -1, nil )
		return
	
	end

	local tbItemID = SplitNativeID( tbData[ 1 ] )
	local tbItemPos = g_MyChar:FindInvenItemPos( tbItemID[1], tbItemID[2], 0 )
	if 2 ~= table.getn( tbItemPos ) then
		return
	end
	
	local nPageIndex = math.floor(tbItemPos[2]/g_nInvenPageYCount)+1
	Inventory_MovePage(nPageIndex)
	
	Inventory_HighlightItem( tbItemID[ 1 ], tbItemID[ 2 ], tbData[ 2 ] )

	--g_nInvenPageYCount
	--local tbItemPos = g_MyChar:FindInvenItemPos( tbItemID[ 1 ], tbItemID[ 2 ], 0 )
end

function Ev_Product_MaterialRollOut( strIndex )

	Inventory_SetVisibleMultiHighlight( false, -1, nil )

end

function Ev_Product_SearchText()
	local strInputText = GetString(WIDGET_ID_PRODUCT, "text", "ProductAutoCompleteTB.textInput")
	local lCount = 0
	for value in pairs( g_Pruduct_tbItemNameList ) do
	
		local tbItem = g_Pruduct_tbItemNameList[ value ]

		if AnsiToUTF8( tbItem[1] ) == strInputText then
			local nIndex = tbItem[2]
			Ev_Product_SelectProduct(nIndex)
			SetInt( WIDGET_ID_PRODUCT, "scrollPosition", nIndex, "productList.list" )
			SetInt( WIDGET_ID_PRODUCT, "selectedIndex", nIndex, "productList.list" )
		end

	end
end

function Product_CountProduct()
	

	local nCount = tonumber( GetString( WIDGET_ID_PRODUCT, "text", "Goods.infoViewGroup.ProductCountEdit" ) )
		
		-- 개수 입력 안됨
	if nCount == nil then
		return
	end
	
	local dwKey = g_Product:GetSelectedProductKey( g_Product_wSelectedProduct )
	local nMaxCount = tonumber( g_Product:GetCountProduct(dwKey) ) --제작 가능 개수
	if nMaxCount == nil then
		return
	end
	
	
	if nCount > nMaxCount then
		nCount = nMaxCount	
	end
	
	SetString( WIDGET_ID_PRODUCT, "text", tostring(nCount) , "Goods.infoViewGroup.ProductCountEdit" )
end

function Ev_Product_KeyUp( nKeyCode )
	if false == IsLoadWidget( WIDGET_ID_PRODUCT ) then
		return false
	end
	
	if false == IsOpenWidget( WIDGET_ID_PRODUCT ) then	
		return false		
	end
	
	Product_CountProduct() -- 제작가능 개수 업데이트
	
	if KEYCODE_ENTER == nKeyCode then
		Ev_Product_SearchText()
	end
end

function Ev_Product_btnSearchText()
	Ev_Product_SearchText()
end

-----------------------------------------------------------------------------------------------------------------------------------------------
-- Msg
function MsgProductTypeAddAck()
	Product_SetTapButton()
	Product_UpdateStudyPointText()
	Event_MsgBox_Close()
	
	Refresh_ProductStudyTab()
end
