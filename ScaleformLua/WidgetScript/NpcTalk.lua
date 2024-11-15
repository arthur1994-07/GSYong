
-- NPC 대화 타입에 따른 아이콘
NpcTalk_ICON_HEAL = "heal"
NpcTalk_ICON_TRADE = "trade"
NpcTalk_ICON_STARTPOINT = "startPoint"
NpcTalk_ICON_STORAGE = "storage"
NpcTalk_ICON_MARKET = "market"
NpcTalk_ICON_RESETPOINT = "resetPoint"
NpcTalk_ICON_BUS = "bus"
NpcTalk_ICON_QUEST_START = "questStart"
NpcTalk_ICON_QUEST_ING = "questProgress"
NpcTalk_ICON_QUEST_TALK = "talk"

g_nNpcTalk_TalkListCount = 7

-- 대화 정보 테이블
g_tbNpcTalk_TalkInfo = {}
-- 마켓탭 정보 테이블
g_tbNpcTalk_MarketTabInfo = {}

-- NPC 아이템 판매용 데이터
g_tbNpcTalk_SellItemData = {}
-- NPC 아이템 구매용 데이터
g_tbNpcTalk_BuyItemData = {}
-- NPC 카트 아이템 데이터
g_tbNpcTalk_CartItemData = {}

-- 카트 슬롯 비어있는지 여부
g_tbNpcTalk_EmptyCart = {}

-- 상점 탭으로 열기?
g_bNpcTalk_OpenMarket = false

g_bNpcTalk_IsOpen = false

function IsOpenNpcTalk()

	return g_bNpcTalk_IsOpen
	
end

-- NPC 대화 UI 로드 완료
function LoadNpcTalk()

	SetEvent(WIDGET_ID_NPCTALK, EVENT_WIDGET_CLOSE, "CloseNpcTalk", "", "")

	-- 탭 이벤트 설정
	SetEvent( WIDGET_ID_NPCTALK, EVENT_INDEX_CLICK_CHANGE, "Ev_NpcTalk_tabPage", "index", "tabPage" )
	-- 리스트 이벤트 설정
	SetEvent( WIDGET_ID_NPCTALK, EVENT_LIST_INDEX_CHANGE, "Ev_NpcTalk_listClick", "index", "pageTalk.list" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_SCROLL, "Ev_NpcTalk_listScroll", "", "pageTalk.list" )
	SetBool( WIDGET_ID_NPCTALK, "ImmediateUpdateSelectIndex", true, "pageTalk.list" )
	
	HitTestEnable( WIDGET_ID_NPCTALK, false, "pageTalk.text" )
	
	-- 마켓 슬롯 바인딩
	local strInst = "pageMarket.slotMarket"
	for i = 0, NPC_MARKET_SLOT_COUNT_TOTAL - 1, 1 do
	
		SetString( WIDGET_ID_NPCTALK, "binding", SLOT_NPCMARKET_ID .. tostring( i ), strInst .. tostring( i ) )
		SetBool( WIDGET_ID_NPCTALK, "visible", false, strInst .. tostring( i ) .. ".valueAmount" )
		SetBool( WIDGET_ID_NPCTALK, "holdable", false, strInst .. tostring( i ) )
		
	end
	
	-- 추천 아이템 슬롯 바인딩
	strInst = "pageMarket.mcRecommend"
	for i = 0, NPC_MARKET_RECOMMEND_SLOT_COUNT - 1, 1 do
	
		SetString( WIDGET_ID_NPCTALK, "binding", SLOT_NPCMARKET_RECOMMEND_ID .. tostring( i ),
			strInst .. tostring( i ) .. ".slot" )	
		SetBool( WIDGET_ID_NPCTALK, "visible", false, strInst .. tostring( i ) .. ".slot.valueAmount" )	
		
	end
	
	-- 카트 아이템 슬롯 바인딩
	strInst = "pageMarket.slotCart"
	for i = 0, NPC_MARKET_CART_SLOT_COUNT - 1, 1 do	
	
		local strSlotID = SLOT_NPCMARKET_CART_ID .. tostring( i )
		SetString( WIDGET_ID_NPCTALK, "binding", strSlotID, strInst .. tostring( i ) )
			
		ResetSlot( strSlotID )
		g_tbNpcTalk_EmptyCart[ i + 1 ] = false
		
	end
	
	-- 재매입 아이템 슬롯 바인딩
	-- 재매입 구입 버튼 텍스트, 이벤트 설정
	strInst = "pageRebuy.exList.mcSlotBtn"
	for i = 1, NPC_MARKET_REBUY_SLOT_COUNT, 1 do
	
		SetString( WIDGET_ID_NPCTALK, "binding",
			SLOT_NPCMARKET_REBUY_ID .. tostring( i ),
			strInst .. tostring( i ) .. ".slot" )
			
		SetString( WIDGET_ID_NPCTALK, "label",
			GetGameWordText( "MARKET_SEARCH_BUY_BUTTON", 0 ), 
			strInst .. tostring( i ) .. ".btn" )
			
		SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK,
			"Ev_NpcTalk_rebuyBtn", "", strInst .. tostring( i ) .. ".btn", tostring( i ) )
			
	end
	
	-- 아이템 광고 슬롯 바인딩, 버튼, 텍스트 설정
	SetString( WIDGET_ID_NPCTALK, "binding", SLOT_NPC_AD_ID, "slotAD" )
	SetSlot_Item_DWORD( SLOT_NPC_AD_ID, g_NpcTalk:GetAdItemID(), 1 )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "DIALOGUE_WINDOW_PROMOTION_TEXT", 0 ), "labelAD" )
	SetString( WIDGET_ID_NPCTALK, "label", GetGameWordText( "MARKET_SEARCH_BUY_BUTTON", 0 ), "btnAD" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_btnAD", "", "btnAD" )
	
	if g_UserFeatures:IsUsingPointShop() == false then
		SetBool( WIDGET_ID_NPCTALK, "enabled", "false", "btnAD" )
	end
	
	-- 텍스트 설정
	SetString( WIDGET_ID_NPCTALK, "text", GetGameWordText( "DIALOGUE_RECOMMEND_ITEM_TITLE", 0 ), "pageMarket.labelRecommend" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "DIALOGUE_WINDOW_USABLE_TEXT", 0 ), "pageMarket.checkUsableItem_text" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "DIALOGUE_WINDOW_BUYABLE_TEXT", 0 ), "pageMarket.checkPurchaseItem_text" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "MARKET_SELL_RIGHT_BUTTON_TEXT", 0 ), "pageMarket.checkOption1_text" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "MARKET_CONFIRM_SELL_ITEM_TEXT", 0 ), "pageMarket.checkOption2_text" )
	SetString( WIDGET_ID_NPCTALK, "label", GetGameWordText( "DIALOGUE_WINDOW_CLOSE_BUTTON", 0 ), "pageMarket.btnClose" )
	SetString( WIDGET_ID_NPCTALK, "label", GetGameWordText( "ITEM_REPAIR_UI", 0 ), "pageMarket.btnRepair" )
	SetString( WIDGET_ID_NPCTALK, "label", GetGameWordText( "POINT_SHOP_BUY_BUTTON_TEXT", 0 ), "pageMarket.btnPurchase" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameWordText( "MARKET_SEARCH_BUY_TOTAL_COST" ), "pageMarket.labelCostMoney" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameWordText( "MARKET_SEARCH_BUY_HAVE_MONEY" ), "pageMarket.labelHaveMoney" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "DIALOGUE_WINDOW_REBUY_SUPPLES", 0 ), "pageRebuy.checkRebuyOp1_text" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "DIALOGUE_WINDOW_ALL_DEL_ITEMS", 0 ), "pageRebuy.checkRebuyOp2_text" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "MARKET_SELL_RIGHT_BUTTON_TEXT", 0 ), "pageRebuy.checkOption1_text" )
	SetString( WIDGET_ID_NPCTALK, "text", GetGameInternalText( "MARKET_CONFIRM_SELL_ITEM_TEXT", 0 ), "pageRebuy.checkOption2_text" )
	SetString( WIDGET_ID_NPCTALK, "TooltipText", GetGameInternalText( "POINT_SHOP_CART_ICON_HELP" ), "pageMarket.btnEmpty" )
	
	SetString( WIDGET_ID_NPCTALK, "htmlText",
		HTMLFontColor( GetGameInternalText( "DIALOGUE_WINDOW_NOTIFY_ALL_DEL_ITEMS", 0 ), HTML_TEXT_COLOR_ORANGE ),
		"pageRebuy.labelInfo.textField" )
	
	-- 이벤트 설정
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkUsableItem", "", "pageMarket.checkUsableItem" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkPurchaseItem", "", "pageMarket.checkPurchaseItem" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkOption1", "", "pageMarket.checkOption1" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkOption2", "", "pageMarket.checkOption2" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_btnClose", "", "pageMarket.btnClose" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_btnRepair", "", "pageMarket.btnRepair" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_btnPurchase", "", "pageMarket.btnPurchase" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_btnEmpty", "", "pageMarket.btnEmpty" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkRebuyOp1", "", "pageRebuy.checkRebuyOp1" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkRebuyOp2", "", "pageRebuy.checkRebuyOp2" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkOption1", "", "pageRebuy.checkOption1" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_BUTTON_CLICK, "Ev_NpcTalk_checkOption2", "", "pageRebuy.checkOption2" )
	SetEvent( WIDGET_ID_NPCTALK, EVENT_SCROLL, "Ev_NpcTalk_RebuyListScroll", "", "pageRebuy.exList.list" )
	
	g_NpcTalk:RemoveCartAll()
	NpcTalk_SetCartTotalPrice()
	
	-- 아이템 내구도 기능?
	if false == g_UserFeatures:IsUsingItemDurability() then
		SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket.btnRepair" )
	end
	
end

function OpenNpcTalk()

	if true == IsOpenWidget( WIDGET_ID_LOCKER ) then
		CloseWidget( WIDGET_ID_LOCKER )
	end

	g_bNpcTalk_IsOpen = true

	NpcTalk_SetTimer()
	
	-- 토크 데이터 설정
	NpcTalk_SetTalkData( true )
	
	-- 탭 visible 설정
	Ev_NpcTalk_tabPage( 0 )
	
	g_NpcTalk:SetRebuySupplies( g_NpcTalk:GetRebuySupplies() )
	
end

function CloseNpcTalk()

	g_bNpcTalk_IsOpen = false

	-- 타이머 제거
	local tbQuickSet = 
	{
		time = 0,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_NPCTALK, tbQuickSet, "", "", "" )
	
end

function NpcTalk_SetTimer()

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "FrameMove_NpcTalk"
	}
	SetTween( WIDGET_ID_NPCTALK, tbQuickSet, "", "", "" )
	
end

-- Npc 대화 데이터 설정
function NpcTalk_SetTalkData( bNew )

	-- 마켓 탭 정보 가져오기
	g_tbNpcTalk_MarketTabInfo = g_NpcTalk:GetMarketTab()
	
	-- UI가 새로 로드된 경우 데이터 프로바이더 생성
	if true == bNew then
	
		local strTabData = ""
		for i = 0, 4, 1 do
		
			-- 첫번째 버튼은 항상 대화 버튼이다.
			if 0 == i then
			
				strTabData = strTabData .. GetGameWordText( "DIALOGUE_WINDOW_STATIC", 0 ) .. ";"
				
			else
			
				local tbMarketData = g_tbNpcTalk_MarketTabInfo[ i ]
				-- 상점 관련 데이터가 없어도 탭 개수를 5개 유지하도록 함.
				if nil == tbMarketData then
				
					strTabData = strTabData .. " ;"
					
				else
				
					-- 재매입 탭인지 확인
					if false == tbMarketData[ "bRebuy" ] then
						strTabData = strTabData .. tbMarketData[ "strName" ] .. ";"
					else
						strTabData = strTabData .. GetGameWordText( "DIALOGUE_TAB_REBUY", 0 ) .. ";"
					end
					
				end
				
			end
			
		end
		
		-- 데이터 프로바이더 설정
		SetData( WIDGET_ID_NPCTALK, strTabData, "tabPage", false )
	
	-- 이미 켜져있는 상태라면 버튼 레이블 변경
	else
	
		for i = 0, 4, 1 do
		
			local strParam = tostring( i ) .. ";"
			
			-- 첫번째 버튼은 항상 대화 버튼이다.
			if 0 == i then
			
				strParam = strParam .. GetGameWordText( "DIALOGUE_WINDOW_STATIC", 0 )
				
			else
			
				local tbMarketData = g_tbNpcTalk_MarketTabInfo[ i ]
				if nil == tbMarketData then
				
					break
					
				else
				
					-- 재매입 탭인지 확인
					if false == tbMarketData[ "bRebuy" ] then
						strParam = strParam .. tbMarketData[ "strName" ]
					else
						strParam = strParam .. GetGameWordText( "DIALOGUE_TAB_REBUY", 0 )
					end
					
				end
				
			end
			
			-- 버튼 레이블 변경
			CallFunction( WIDGET_ID_NPCTALK, "ChangeButtonLabel", strParam, "tabPage" )	
			
		end
	
	end
	
	-- 상점 탭으로 열기?
	if 	true == g_bNpcTalk_OpenMarket and
		nil ~= g_tbNpcTalk_MarketTabInfo[ 1 ] then
		
		SetInt( WIDGET_ID_NPCTALK, "selectedIndex", 1, "tabPage" )
		g_bNpcTalk_OpenMarket = false
		
	end
	
	-- 탭 보여질 개수 설정
	CallFunction( WIDGET_ID_NPCTALK, "VisibleButtonCount", tostring( table.getn( g_tbNpcTalk_MarketTabInfo ) ) + 1, "tabPage" )
	-- 대화탭 선택
	SetInt( WIDGET_ID_NPCTALK, "selectedIndex", 0, "tabPage" )
	-- 대화 답변 선택문 로드
	NpcTalk_SetAnswerList()

	-- NPC 대화 텍스트 설정
	SetString( WIDGET_ID_NPCTALK, "text", g_NpcTalk:GetBasicTalk(), "pageTalk.text" )

end

-- 대화탭 답변 선택문 리스트 설정
function NpcTalk_SetAnswerList()

	g_tbNpcTalk_TalkInfo = g_NpcTalk:GetAnswerList()	
	local tbDataList = {}
	
	-- 대화 종료는 여기서 넣어준다.
	local tbAnswerQuit = {}
	tbAnswerQuit[ "nAnswerID" ] = -1
	tbAnswerQuit[ "nAnswerType" ] = TALK_BASIC_TALK
	tbAnswerQuit[ "strAnswer" ] = GetGameWordText( "DIALOGUE_TALK_BUTTON", 1 )
	tbAnswerQuit[ "strTooltip" ] = ""
	tbAnswerQuit[ "dwQuestBeginMoney" ] = 0
	tbAnswerQuit[ "dwActionNo" ] = EM_BASIC_NULL	
	table.insert( g_tbNpcTalk_TalkInfo, tbAnswerQuit )
	
	-- 해당 대화에 대한 답변 텍스트, 텍스트 컬러 설정
	for i = 1, table.getn( g_tbNpcTalk_TalkInfo ), 1 do
	
		local tbAnswer = g_tbNpcTalk_TalkInfo[ i ]
		if nil ~= tbAnswer then
		
			local strColor = HTML_TEXT_COLOR_WHITE
			
			-- 퀘스트 완료
			if TALK_QUEST_STEP == tbAnswer[ "nAnswerType" ] then
			
				strColor = HTML_TEXT_COLOR_YELLOW
				tbAnswer[ "strAnswer" ] = GetGameWordText( "DIALOGUE_TALK_BUTTON", 2 ) .. " " .. tbAnswer[ "strAnswer" ]
			
			-- 퀘스트 시작
			elseif TALK_QUEST_START == tbAnswer[ "nAnswerType" ] then
			
				strColor = HTML_TEXT_COLOR_FORESTGREEN
				tbAnswer[ "strAnswer" ] = GetGameWordText( "DIALOGUE_TALK_BUTTON", 3 ) .. " " .. tbAnswer[ "strAnswer" ]
				
				if 0 ~= tbAnswer[ "dwQuestBeginMoney" ] then
				
					local strQuestCost = "[" .. GetGameWordText( "QUEST_BEGIN_MONEY", 0 ) .. ":" .. tostring( tbAnswer[ "dwQuestBeginMoney" ] )
					tbAnswer[ "strAnswer" ] = tbAnswer[ "strAnswer" ] .. strQuestCost
					
				end 
			
			-- 퀘스트 진행 중
			elseif TALK_QUEST_ING == tbAnswer[ "nAnswerType" ] then
			
				strColor = HTML_TEXT_COLOR_DARKGRAY
				tbAnswer[ "strAnswer" ] = GetGameWordText( "DIALOGUE_TALK_BUTTON", 4 ) .. " " .. tbAnswer[ "strAnswer" ]
			
			end
			
			-- 툴팁
			local strTooltip = ""
			if 0 ~= string.len( tbAnswer[ "strTooltip" ] ) then
			
				strTooltip = HTMLFontColor( GetGameWordText( "QUEST_GIFT", 9 ), HTML_TEXT_COLOR_GREENYELLOW ) ..
					HTMLFontColor( tbAnswer[ "strTooltip" ], HTML_TEXT_COLOR_WHITE )
			
			end
			
			local tbData = {}
			tbData[ 1 ] = HTMLFontColor( PrepareHTMLData( tbAnswer[ "strAnswer" ] ), strColor )
			tbData[ 2 ] = tbAnswer[ "nAnswerID" ]
			tbData[ 3 ] = strTooltip
			tbDataList[ i ] = tbData
			
		end
		
	end
	
	SetInt( WIDGET_ID_NPCTALK, "selectedIndex", -1, "pageTalk.list" )
	
	local strParam = CreateParam( "label", "nAnswerID", "TooltipHtmlText" )
	SetDataEx( WIDGET_ID_NPCTALK, strParam, tbDataList, "pageTalk.list", false )
	
	Ev_NpcTalk_listScroll()

end

-- 상점 탭 업데이트
function NpcTalk_UpdateMarketTab( nIndex )

	-- 체크버튼 설정
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetUsableItem(), "pageMarket.checkUsableItem" )
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetPurchaseItem(), "pageMarket.checkPurchaseItem" )
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetSellRMouse(), "pageMarket.checkOption1" )
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetConfirmSellItem(), "pageMarket.checkOption2" )
	
	-- 상점 아이템 업데이트
	NpcTalk_UpdateSaleItem( nIndex )
	-- 추천 아이템 업데이트
	NpcTalk_UpdateRecommendItem( nIndex )
	
	-- 소지금
	SetString( WIDGET_ID_NPCTALK, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "pageMarket.labelMoney" )

end

-- 재매입 탭 업데이트
function NpcTalk_UpdateRebuyTab()

	-- 대화 탭이 선택되어 있으면 업데이트 할 필요 없음.
	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )
	if 0 == nIndex then	
		return
	else
	
		local tbMarket = g_tbNpcTalk_MarketTabInfo[ nIndex ]
		if nil == tbMarket or faslse == tbMarket[ "bRebuy" ] then
			return
		end
		
	end

	-- 스크롤 오프셋 구하기
	local nScrollPos = GetInt( WIDGET_ID_NPCTALK, "scrollPosition", "pageRebuy.exList.list" )
	local nListOffset = nScrollPos * NPC_MARKET_REBUY_SLOT_COL
	
	-- 재매입 아이템 데이터 가져오기
	local tbRebuy = g_NpcTalk:GetRebuy()
	
	-- 재매입 아이템 리스트 설정
	local strListData = ""
	for value in pairs( tbRebuy ) do
	
		local tbItem = tbRebuy[ value ]
		strListData = strListData .. tbItem[ "strName" ] .. ";"
		
	end
	
	if 0 ~= string.len( strListData ) then
		SetData( WIDGET_ID_NPCTALK, strListData, "pageRebuy.exList.list", false )
	-- 재매입 아이템이 없으면 리스트 비워주기
	else
		RemoveData( WIDGET_ID_NPCTALK, 0, "pageRebuy.exList.list", true )
	end	
	
	for i = 1, NPC_MARKET_REBUY_SLOT_COUNT, 1 do
	
		local strInst = "pageRebuy.exList.mcSlotBtn" .. tostring( i )
		
		local tbItem = tbRebuy[ i + nListOffset ]
		if nil ~= tbItem then
		
			-- 재매입 아이템 설정
			SetSlot_Rebuy( SLOT_NPCMARKET_REBUY_ID .. tostring( i ), i + nListOffset, tbItem[ "wMID" ], tbItem[ "wSID" ])	
		
			-- 버튼, 슬롯 보이기
			SetBool( WIDGET_ID_NPCTALK, "visible", true, strInst .. ".slot" )
			SetBool( WIDGET_ID_NPCTALK, "visible", true, strInst .. ".btn" )
			
			-- 가격 텍스트 설정, 소지금보다 비싸면 빨간색으로 표기
			local strPrice = ""
			if g_MyChar:GetInvenMoney() < tbItem[ "nPrice" ] then
				strPrice = HTMLFontColor( CurrencyFormat( tbItem[ "nPrice" ] ), HTML_TEXT_COLOR_RED )
			else
				strPrice = HTMLFontColor( CurrencyFormat( tbItem[ "nPrice" ] ), HTML_TEXT_COLOR_WHITE )
			end			
			SetString( WIDGET_ID_NPCTALK, "htmlText", strPrice, strInst .. ".textField" )
		
		else
		
			-- 버튼, 슬롯 숨기기
			SetBool( WIDGET_ID_NPCTALK, "visible", false, strInst .. ".slot" )
			SetBool( WIDGET_ID_NPCTALK, "visible", false, strInst .. ".btn" )
			-- 가격 텍스트 비우기
			SetString( WIDGET_ID_NPCTALK, "htmlText", "", strInst .. ".textField" )
		
		end
	
	end
	
	-- 체크버튼 설정
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetRebuySupplies(), "pageRebuy.checkRebuyOp1" )
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetRebuyItemDeleteConfirm(), "pageRebuy.checkRebuyOp2" )
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetSellRMouse(), "pageRebuy.checkOption1" )
	SetBool( WIDGET_ID_NPCTALK, "selected", g_NpcTalk:GetConfirmSellItem(), "pageRebuy.checkOption2" )

end

-- 상점 아이템 업데이트
function NpcTalk_UpdateSaleItem( nIndex )

	local tbSaleItemList = g_NpcTalk:GetSaleItem( nIndex )	
	local tbNoResetList = {}	-- 초기화 안할 슬롯
	
	-- 슬롯에 아이템 설정
	for value in pairs( tbSaleItemList ) do
	
		local tbSaleItem = tbSaleItemList[ value ]
		if nil ~= tbSaleItem then
		
			local bEnabledAlpha = 1
			
			-- 사용 가능 아이템?
			if true == GetBool( WIDGET_ID_NPCTALK, "selected", "pageMarket.checkUsableItem" ) then
			
				if false == tbSaleItem[ "bUseable" ] then
					bEnabledAlpha = 0.25
				end
			
			end
			
			-- 교환 조건 불만족
			if true == GetBool( WIDGET_ID_NPCTALK, "selected", "pageMarket.checkPurchaseItem" ) then
			
				if false == tbSaleItem[ "bExchangeable" ] then
				
					bEnabledAlpha = 0.25
				
				end
				
			end
			
			local nPos = tbSaleItem[ "nPosY" ] * NPC_MARKET_SLOT_COUNT_COL + tbSaleItem[ "nPosX" ]
			SetSlot_NpcMarket( SLOT_NPCMARKET_ID .. tostring( nPos ), tbSaleItem[ "MID" ], tbSaleItem[ "SID" ], 1,
				tbSaleItem[ "nPosX" ], tbSaleItem[ "nPosY" ] )
				
			--SetBool( WIDGET_ID_NPCTALK, "enabled", bEnabled, "pageMarket.slotMarket" .. nPos )
			SetDouble( WIDGET_ID_NPCTALK, "alpha", bEnabledAlpha, "pageMarket.slotMarket" .. nPos )
			
			tbNoResetList[ nPos ] = 0
			
		end
		
	end
	
	-- 아이템이 없는 슬롯 리셋
	for i = 0, NPC_MARKET_SLOT_COUNT_TOTAL - 1, 1 do
	
		if nil == tbNoResetList[ i ] then
		
			--SetBool( WIDGET_ID_NPCTALK, "enabled", true, "pageMarket.slotMarket" .. i )
			SetDouble( WIDGET_ID_NPCTALK, "alpha", 1, "pageMarket.slotMarket" .. i )
			ResetSlot( SLOT_NPCMARKET_ID .. tostring( i ) )
			
		end
	
	end

end

-- 추천 아이템 업데이트
function NpcTalk_UpdateRecommendItem( nIndex )

	local tbRecommendItemList = g_NpcTalk:GetRcommendSaleItem( NPC_MARKET_RECOMMEND_SLOT_COUNT, nIndex )
	
	for i = 0, NPC_MARKET_RECOMMEND_SLOT_COUNT - 1, 1 do
	
		local strInst = "pageMarket.mcRecommend" .. tostring( i )
	
		local tbRecommendItem = tbRecommendItemList[ i + 1 ]
		if nil ~= tbRecommendItem then
		
			SetSlot_NpcMarket( SLOT_NPCMARKET_RECOMMEND_ID .. tostring( i ),
				tbRecommendItem[ "MID" ], tbRecommendItem[ "SID" ], 1,
				tbRecommendItem[ "nPosX" ], tbRecommendItem[ "nPosY" ] )
			SetString( WIDGET_ID_NPCTALK, "text", tbRecommendItem[ "strName" ], strInst .. ".labelName" )
			
			local strPrice = ""
			if false == tbRecommendItem[ "bExchange" ] then
			
				local nColor = GetMoneyColor( tbRecommendItem[ "nPrice" ] )
			
				-- 소수점 오류가 생기므로 수수료가 100( 1.0 ) 일때는 넘어가도록하자. 
				if tbRecommendItem["fBuyRate"] ~= 100 then
					strPrice = CurrencyFormat( math.floor( tbRecommendItem[ "nPrice" ] *  0.01 ) * tbRecommendItem[ "fBuyRate" ] )
				else
					strPrice = CurrencyFormat( math.floor( tbRecommendItem[ "nPrice" ] ) )
				end
				strPrice = HTMLFontColor( strPrice, TranslateHTMLColor( nColor ) )
				
			end
			
			SetString( WIDGET_ID_NPCTALK, "htmlText", strPrice, strInst .. ".labelPrice" )
			
		else
		
			ResetSlot( SLOT_NPCMARKET_RECOMMEND_ID .. tostring( i ) )
			SetString( WIDGET_ID_NPCTALK, "text", "", strInst .. ".labelName" )
			SetString( WIDGET_ID_NPCTALK, "text", "", strInst .. ".labelPrice" )
		
		end
	
	end
	
end

-- 재구입 버튼 활성화 설정
function NpcTalk_EnableRebuyBtn( bEnabled )

	for i = 1, NPC_MARKET_REBUY_SLOT_COUNT, 1 do
		
		SetBool( WIDGET_ID_NPCTALK, "enabled", bEnabled, "pageRebuy.exList.mcSlotBtn" .. tostring( i ) .. ".btn" )
		
	end

end

-----------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- NPC 대화 시도 성공
function NpcTalk_OpenTalk( bOpen )
	--InsertFrameMove( FrameMove_NpcTalk )
	
	if false == bOpen then
	
		CloseWidget( WIDGET_ID_NPCTALK )
	
	else
	
		if false == IsLoadWidget( WIDGET_ID_NPCTALK ) then
		
			OpenWidget( WIDGET_ID_NPCTALK )
			
		else
		
			if false == IsOpenWidget( WIDGET_ID_NPCTALK ) then
				OpenWidget( WIDGET_ID_NPCTALK )
			end
			
			-- 탭 데이터 설정
			NpcTalk_SetTalkData( false )
			-- 탭 visible 설정
			Ev_NpcTalk_tabPage( 0 )
		end
	
	end

end

-- 마켓탭 활성되어 있는지 확인
function NpcTalk_IsMarketTabOpen()

	if false == IsOpenWidget( WIDGET_ID_NPCTALK ) then
		return false
	end
	
	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )
	if 0 == nIndex then
	
		return false
		
	else
	
		local tbMarket = g_tbNpcTalk_MarketTabInfo[ nIndex ]
		if nil == tbMarket then
			return false
		end
		
		return true
		
	end
	
	return false

end

-- Npc 대화창 활성된 마켓탭 인덱스
function NpcTalk_GetActiveMarket()

	if false == IsOpenWidget( WIDGET_ID_NPCTALK ) then
		return -1
	end
	
	return GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" ) - 1

end

-- NPC에게 아이템 팔기
function NpcTalk_SellItem( bSale, bConfirm, strItemName, nPosX, nPosY )

	if true == bConfirm then
	
		if false == bSale then		
			OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "IMPOSSIBLE_SALE_ITEM", 0 ), Event_MsgBox_Close )
			return		
		end
		
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, StringFormat( GetGameInternalText( "SELL_ITEM_CONFIRM", 0 ), strItemName ),
			Ev_NpcTalk_ConfirmSellItem, Event_MsgBox_Close )
		
		g_tbNpcTalk_SellItemData = {}
		g_tbNpcTalk_SellItemData[ "nPosX" ] = nPosX
		g_tbNpcTalk_SellItemData[ "nPosY" ] = nPosY
	
	else
	
		if false == bSale then
			BasicChat_PrintSystemMsg( CHAT_SYSTEM, GetGameInternalText( "IMPOSSIBLE_SALE_ITEM", 0 ),
				TEXT_COLOR_LIGHTSKYBLUE )
			return		
		end
		
		g_Msg:SellItemNpcTalk( nPosX, nPosY )
	
	end

end

-- NPC 아이템 구매
function NpcTalk_BuyItem( strItemName, nPosX, nPosY, bPile )

	if true == bPile then
	
		OpenMsgBox( MSG_BOX_TYPE_INPUT_ONLYNUM, GetGameInternalText( "BUY_PILE_ITEM", 0 ), Ev_NpcTalk_BuyInputItemOK, 
			Event_MsgBox_Close )
	
	else
	
		local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )
	
		local tbMarket = g_tbNpcTalk_MarketTabInfo[ nIndex ]
		if nil == tbMarket then
			return
		end
	
		local str = ""
		if true == tbMarket[ "bExchange" ] then
		
			str = GetGameInternalText( "TRADE_ITEM_CONFIRM", 0 )
		
		else
		
			str = GetGameInternalText( "BUY_ITEM_CONFIRM", 0 )
		
		end
	
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, StringFormat( str, strItemName ),
			Ev_NpcTalk_BuyItemOK, Event_MsgBox_Close )
	
	end
	
	g_tbNpcTalk_BuyItemData = {}
	g_tbNpcTalk_BuyItemData[ "nPosX" ] = nPosX
	g_tbNpcTalk_BuyItemData[ "nPosY" ] = nPosY

end

-- 카트에 아이템 추가
function NpcTalk_AddCart( strItemName, wMID, wSID, wPosX, wPosY, bPile, wApplyNum, wPileNum )

	local nEmptySlotIndex = -1

	-- 빈 카트 슬롯 찾기
	for i = 1, table.getn( g_tbNpcTalk_EmptyCart ), 1 do
	
		if false == g_tbNpcTalk_EmptyCart[ i ] then
			nEmptySlotIndex = i - 1
			break
		end
		
	end
	
	-- -1 이면 카트 가득참
	if -1 == nEmptySlotIndex then
	
		local strMsg = GetGameInternalText( "POINT_SHOP_NOTIFY_FULL_CART_TEXT0", 0 ) .. "\n" .. 
			GetGameInternalText( "POINT_SHOP_NOTIFY_FULL_CART_TEXT1", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_MsgBox_Close )
	
		return
		
	end

	if true == bPile then
	
		if 1 == wApplyNum then
		
			OpenMsgBox( MSG_BOX_TYPE_INPUT_ONLYNUM, GetGameInternalText( "BUY_PILE_ITEM", 0 ),
				Ev_NpcTalk_CartInputItemOK, Event_MsgBox_Close )
			
			g_tbNpcTalk_CartItemData = {}
			g_tbNpcTalk_CartItemData[ "wMID" ] = wMID
			g_tbNpcTalk_CartItemData[ "wSID" ] = wSID
			g_tbNpcTalk_CartItemData[ "wPosX" ] = wPosX
			g_tbNpcTalk_CartItemData[ "wPosY" ] = wPosY
			g_tbNpcTalk_CartItemData[ "nEmptySlotIndex" ] = nEmptySlotIndex
			g_tbNpcTalk_CartItemData[ "wPileNum" ] = wPileNum
			
			return
		
		else

			local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" ) - 1
			g_NpcTalk:AddCart( nIndex, wMID, wSID, wPosX, wPosY, 1 )
			SetSlot_NpcMarketCart( SLOT_NPCMARKET_CART_ID .. tostring( nEmptySlotIndex ), wMID, wSID, wApplyNum, nEmptySlotIndex )
		
		end
	
	else
	
		local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" ) - 1
		g_NpcTalk:AddCart( nIndex, wMID, wSID, wPosX, wPosY, 1 )
		SetSlot_NpcMarketCart( SLOT_NPCMARKET_CART_ID .. tostring( nEmptySlotIndex ), wMID, wSID, 1, nEmptySlotIndex )
	
	end
	
	g_tbNpcTalk_EmptyCart[ nEmptySlotIndex + 1 ] = true
	
	NpcTalk_SetCartTotalPrice()

end

-- 카트 아이템 제거
function NpcTalk_RemoveCart( wIndex )

	local tbCart = g_NpcTalk:GetCart()
	
	for i = 0, NPC_MARKET_CART_SLOT_COUNT - 1, 1 do
	
		local tbItem = tbCart[ i + 1 ]
		if nil ~= tbItem then
		
			SetSlot_NpcMarketCart( SLOT_NPCMARKET_CART_ID .. tostring( i ), 
				tbItem[ "wMID" ], tbItem[ "wSID" ], tbItem[ "wCount" ], i )
			g_tbNpcTalk_EmptyCart[ i + 1 ] = true
		
		else
		
			ResetSlot( SLOT_NPCMARKET_CART_ID .. tostring( i ) )
			g_tbNpcTalk_EmptyCart[ i + 1 ] = false
		
		end
	
	end
	
	NpcTalk_SetCartTotalPrice()

end

-- 소지금 업데이트
function NpcTalk_UpdateMoney( nMoney )

	if false == IsOpenWidget( WIDGET_ID_NPCTALK ) then
		return
	end

	SetString( WIDGET_ID_NPCTALK, "text", CurrencyFormat( nMoney ), "pageMarket.labelMoney" )
	
	NpcTalk_UpdateRebuyTab()

end

-- 카트에 넣은 아이템 총 가격 설정
function NpcTalk_SetCartTotalPrice()

	local nCartItemPrice = g_NpcTalk:GetCartTotalPrice()
	local nMyMoney = g_MyChar:GetInvenMoney()
	
	local nColor = TEXT_COLOR_WHITE
	if nMyMoney < nCartItemPrice then	
	
		nColor = TEXT_COLOR_RED

	end
	
	SetString( WIDGET_ID_NPCTALK, "text", CurrencyFormat( nCartItemPrice ), "pageMarket.labelCartTotalPrice" )
	SetTextFormat( WIDGET_ID_NPCTALK, nColor, false, 0, 0, "pageMarket.labelCartTotalPrice" )

end

-- 재매입
function NpcTalk_Rebuy( nIndex )

	local nScrollPos = GetInt( WIDGET_ID_NPCTALK, "scrollPosition", "pageRebuy.exList.list" )
	nIndex = nIndex + ( nScrollPos * NPC_MARKET_REBUY_SLOT_COL ) - 1
	
	return g_Msg:RebuyNpcTalk( nIndex )

end

-- 마켓탭 열기
function NpcTalk_OpenMarket()

	if false == IsOpenWidget( WIDGET_ID_NPCTALK ) then
	
		g_bNpcTalk_OpenMarket = true
		OpenWidget( WIDGET_ID_NPCTALK )
	
	else
	
		if nil ~= g_tbNpcTalk_MarketTabInfo[ 1 ] then
			
			SetInt( WIDGET_ID_NPCTALK, "selectedIndex", 1, "tabPage" )
			
		end
	
	end

end

-- 클럽 창설
function NpcTalk_CreateClub()

	local str = GetGameInternalText( "CLUB_MAKE_OPEN_MESSAGE", 0 )
	local tbOkData = { true }
	local tbCancelData = { false }
	OpenMsgBox( MSG_BOX_TYPE_INPUT, str, Ev_NpcTalk_CreateClub, Ev_NpcTalk_CreateClub, tbOkData, tbCancelData )

end

-----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탭 변경 이벤트
function Ev_NpcTalk_tabPage( nIndex )
	
	-- 0번은 무조건 대화탭
	if 0 == nIndex then
	
		SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageTalk" )
		SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket" )
		SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageRebuy" )
		
		-- 대화창이 열릴때 장바구니를 비워 다른 Npc 를 통해 등록된 장바구니가 현재 Npc 에게 등록되어 있는 현상방지
		Ev_NpcTalk_btnEmpty()
		
	else
	
		local tbMarket = g_tbNpcTalk_MarketTabInfo[ nIndex ]
		if nil == tbMarket then
			return
		end
		
		-- 재매입 탭이 아니면 상점 탭 활성
		if false == tbMarket[ "bRebuy" ] then
		
			SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageTalk" )
			SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageMarket" )
			SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageRebuy" )
			
			NpcTalk_UpdateMarketTab( nIndex - 1 )
			
			-- 교환 탭
			if true == tbMarket[ "bExchange" ] then
			
				-- 교환하기
				local str = GetGameWordText( "EXCHANGE_SHOP_BUY_BUTTON_TEXT", 0 )
				SetString( WIDGET_ID_NPCTALK, "label", str, "pageMarket.btnPurchase" )
				
				-- 교환 가능한 아이템
				str = GetGameInternalText( "DIALOGUE_WINDOW_EXCHANGEABLE_TEXT",0 )
				SetString( WIDGET_ID_NPCTALK, "text", str, "pageMarket.checkPurchaseItem_text" )
				
				-- 무기 부스터 및 구매 금액 visible 끄기
				SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket.btnRepair" )
				SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket.labelCostMoney" )
				SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket.labelCartTotalPrice" )
				SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket.bgPrice" )
			
			-- 일반 상점
			else
			
				-- 구입하기
				local str = GetGameWordText( "POINT_SHOP_BUY_BUTTON_TEXT", 0 )
				SetString( WIDGET_ID_NPCTALK, "label", str, "pageMarket.btnPurchase" )
				
				-- 구입 가능한 아이템
				str = GetGameInternalText( "DIALOGUE_WINDOW_BUYABLE_TEXT",0 )
				SetString( WIDGET_ID_NPCTALK, "text", str, "pageMarket.checkPurchaseItem_text" )
				-- 무기 부스터 및 구매 금액 visible 켜기
				if true == g_UserFeatures:IsUsingItemDurability() then
					SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageMarket.btnRepair" )
				end
				SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageMarket.labelCostMoney" )
				SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageMarket.labelCartTotalPrice" )
				SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageMarket.bgPrice" )
			
			end
		
		else
		
			SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageTalk" )
			SetBool( WIDGET_ID_NPCTALK, "visible", false, "pageMarket" )
			SetBool( WIDGET_ID_NPCTALK, "visible", true, "pageRebuy" )
			
			NpcTalk_UpdateRebuyTab()
		
		end
	
	end
	

end

-- 대화 리스트 인덱스 변경
function Ev_NpcTalk_listClick( nIndex )

	if -1 == nIndex then
		return
	end
	
	local nAnswerID = GetDataInt( WIDGET_ID_NPCTALK, nIndex, "nAnswerID", "pageTalk.list" )
	
	-- ID가 -1이면 대화종료
	if -1 == nAnswerID then
	
		CloseWidget( WIDGET_ID_NPCTALK )
		
	else
	
		if true == g_Msg:SelectNpcTalk( nAnswerID ) then
		
			-- NPC 대화 텍스트 설정
			SetString( WIDGET_ID_NPCTALK, "text", g_NpcTalk:GetBasicTalk(), "pageTalk.text" )		
			-- 대화 리스트 설정
			NpcTalk_SetAnswerList()
			
		else
		
			SetInt( WIDGET_ID_NPCTALK, "selectedIndex", -1, "pageTalk.list" )
-- RM# 4429 참조
-- 아래 의도의 상황이 발생하면 안된다;
--			CloseWidget( WIDGET_ID_NPCTALK )
			
		end
	
	end

end

-- 리스트 스크롤
function Ev_NpcTalk_listScroll()

	local nScrollPos = GetInt( WIDGET_ID_NPCTALK, "scrollPosition", "pageTalk.list" )

	for i = 1, g_nNpcTalk_TalkListCount, 1 do
	
		local strInst = "pageTalk.answer" .. i .. ".mcIcon"
	
		local tbAnswer = g_tbNpcTalk_TalkInfo[ i + nScrollPos ]
		if nil == tbAnswer then
		
			SetBool( WIDGET_ID_NPCTALK, "visible", false, strInst )
		
		else
		
			local bVisible = true
		
			local strIcon = ""
			if TALK_FIRST_TIME == tbAnswer[ "nAnswerType" ] then
			
				strIcon = NpcTalk_ICON_QUEST_TALK
				
			elseif TALK_QUEST_STEP == tbAnswer[ "nAnswerType" ] then
			
				strIcon = NpcTalk_ICON_QUEST_START
			
			elseif TALK_QUEST_START == tbAnswer[ "nAnswerType" ] then
			
				strIcon = NpcTalk_ICON_QUEST_START
			
			elseif TALK_QUEST_ING == tbAnswer[ "nAnswerType" ] then
			
				strIcon = NpcTalk_ICON_QUEST_ING
			
			elseif TALK_BASIC_TALK == tbAnswer[ "nAnswerType" ] then
			
				if EM_CURE == tbAnswer[ "dwActionNo" ] then
					strIcon = NpcTalk_ICON_HEAL
				elseif EM_STARTPOINT == tbAnswer[ "dwActionNo" ] then
					strIcon = NpcTalk_ICON_STARTPOINT
				else
					strIcon = NpcTalk_ICON_QUEST_TALK
				end
				
			else
			
				bVisible = false
			
			end

			SetBool( WIDGET_ID_NPCTALK, "visible", bVisible, strInst )
			CallFunction( WIDGET_ID_NPCTALK, "gotoAndStop", strIcon, strInst )
			
		end
	
	end

end

-- 사용 가능 아이템
function Ev_NpcTalk_checkUsableItem()

	local bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageMarket.checkUsableItem" )
	g_NpcTalk:SetUsableItem( bCheck )
	
	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )	
	NpcTalk_UpdateRecommendItem( nIndex - 1 )
	NpcTalk_UpdateMarketTab( nIndex - 1 )
	
end

-- 구입 가능한 아이템
function Ev_NpcTalk_checkPurchaseItem()

	local bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageMarket.checkPurchaseItem" )
	g_NpcTalk:SetPurchaseItem( bCheck )
	
	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )	
	NpcTalk_UpdateRecommendItem( nIndex - 1 )
	NpcTalk_UpdateMarketTab( nIndex - 1 )

end

-- 마우스 우클릭 아이템 판매
function Ev_NpcTalk_checkOption1()

	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )
	local tbMarket = g_tbNpcTalk_MarketTabInfo[ nIndex ]
	if nil == tbMarket then
		return
	end

	local bCheck = false
	if true == tbMarket[ "bRebuy" ] then
		bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageRebuy.checkOption1" )
	else
		bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageMarket.checkOption1" )
	end
	
	g_NpcTalk:SetSellRMouse( bCheck )

end

-- 아이템 판매시 확인
function Ev_NpcTalk_checkOption2()

	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )
	local tbMarket = g_tbNpcTalk_MarketTabInfo[ nIndex ]
	if nil == tbMarket then
		return
	end
	
	local bCheck = false
	if true == tbMarket[ "bRebuy" ] then
		bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageRebuy.checkOption2" )
	else
		bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageMarket.checkOption2" )
	end
	
	g_NpcTalk:SetConfirmSellItem( bCheck )

end

-- 재매입 리스트 스크롤 이벤트
function Ev_NpcTalk_RebuyListScroll()

	NpcTalk_UpdateRebuyTab()

end

-- 소모품 재매입 저장
function Ev_NpcTalk_checkRebuyOp1()

	local bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageRebuy.checkRebuyOp1" )
	g_NpcTalk:SetRebuySupplies( bCheck )

end

-- 접속종료 시 재매입 아이템 확인
function Ev_NpcTalk_checkRebuyOp2()

	local bCheck = GetBool( WIDGET_ID_NPCTALK, "selected", "pageRebuy.checkRebuyOp2" )
	g_NpcTalk:SetRebuyItemDeleteConfirm( bCheck )

end

-- 닫기
function Ev_NpcTalk_btnClose()
	CloseWidget( WIDGET_ID_NPCTALK )
end

-- 아이템 수리
function Ev_NpcTalk_btnRepair()

	OpenWidget( WIDGET_ID_ITEMREPAIR )

end

-- 구매하기
function Ev_NpcTalk_btnPurchase()
	-- 교환/ 돈으로 구입으로 타입이 나뉘어 져있으므로 돈으로만 검사하면안된다;
	--[[
	local nCartItemPrice = g_NpcTalk:GetCartTotalPrice()
	local nMyMoney = g_MyChar:GetInvenMoney()
	if nMyMoney < nCartItemPrice then
		return
	end
	]]--
	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" )	
	if g_Msg:BuyCartItemNpcTalk( nIndex - 1 ) == true then
		for i = 0, NPC_MARKET_CART_SLOT_COUNT - 1, 1 do
			g_tbNpcTalk_EmptyCart[ i + 1 ] = false
			ResetSlot( SLOT_NPCMARKET_CART_ID .. tostring( i ) )
		end
	end
end

-- 카트 비우기
function Ev_NpcTalk_btnEmpty()

	g_NpcTalk:RemoveCartAll()	
	NpcTalk_SetCartTotalPrice()
	
	for i = 0, NPC_MARKET_CART_SLOT_COUNT - 1, 1 do
	
		g_tbNpcTalk_EmptyCart[ i + 1 ] = false
		ResetSlot( SLOT_NPCMARKET_CART_ID .. tostring( i ) )
	
	end

end

-- 아이템 판매 확인
function Ev_NpcTalk_ConfirmSellItem()

	g_Msg:SellItemNpcTalk( g_tbNpcTalk_SellItemData[ "nPosX" ], g_tbNpcTalk_SellItemData[ "nPosY" ] )
	Event_MsgBox_Close()

end

-- 아이템 구매 개수 입력 확인
function Ev_NpcTalk_BuyInputItemOK()

	local nCount = tonumber( GetMsgBoxInput() )
	if nil == nCount or 1 > nCount or 65535 < nCount then
		return
	end
	
	nCount = math.floor( nCount )
	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" ) - 1
	
	g_Msg:BuyItemNpcTalk( nIndex, g_tbNpcTalk_BuyItemData[ "nPosX" ], g_tbNpcTalk_BuyItemData[ "nPosY" ], nCount )
	Event_MsgBox_Close()

end

-- 아이템 구매 확인
function Ev_NpcTalk_BuyItemOK()

	local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" ) - 1
	
	g_Msg:BuyItemNpcTalk( nIndex, g_tbNpcTalk_BuyItemData[ "nPosX" ], g_tbNpcTalk_BuyItemData[ "nPosY" ], 1 )
	Event_MsgBox_Close()

end

-- 카트에 넣을 개수 입력 확인
function Ev_NpcTalk_CartInputItemOK()

	local nCount = tonumber( GetMsgBoxInput() )
	if nil == nCount or 1 > nCount or 65535 < nCount then
		return
	end
	
	while 0 < nCount do
	
		-- 빈 카트 슬롯 찾기
		local nEmptySlotIndex = -1
		for i = 1, table.getn( g_tbNpcTalk_EmptyCart ), 1 do
		
			if false == g_tbNpcTalk_EmptyCart[ i ] then
				nEmptySlotIndex = i - 1
				break
			end
			
		end
		
		-- 빈 카트 없다.
		if -1 == nEmptySlotIndex then
			break
		end
		
		local nTempCount = nCount
	
		-- 카트에 들어갈 놈이 아이템 겹침 최대 개수 넘는지 확인
		if g_tbNpcTalk_CartItemData[ "wPileNum" ] < nTempCount then
		
			nTempCount = g_tbNpcTalk_CartItemData[ "wPileNum" ]
			
			-- 카트에 담을 개수 까기
			nCount = nCount - g_tbNpcTalk_CartItemData[ "wPileNum" ]
			
		-- 전부 다 담음
		else
		
			nCount = 0
		
		end
		
		local nIndex = GetInt( WIDGET_ID_NPCTALK, "selectedIndex", "tabPage" ) - 1

		-- 카트에 넣기
		g_NpcTalk:AddCart( nIndex, g_tbNpcTalk_CartItemData[ "wMID" ], g_tbNpcTalk_CartItemData[ "wSID" ],
			g_tbNpcTalk_CartItemData[ "wPosX" ], g_tbNpcTalk_CartItemData[ "wPosY" ], nTempCount )
			
		-- 슬롯 아이템 설정
		SetSlot_NpcMarketCart( SLOT_NPCMARKET_CART_ID .. tostring( nEmptySlotIndex ),
			g_tbNpcTalk_CartItemData[ "wMID" ], g_tbNpcTalk_CartItemData[ "wSID" ],
			nTempCount,
			nEmptySlotIndex )
			
		-- 현재 카트 사용중임
		g_tbNpcTalk_EmptyCart[ nEmptySlotIndex + 1 ] = true
	
	end
	
	NpcTalk_SetCartTotalPrice()
	Event_MsgBox_Close()

end

-- 재매입 구입 버튼
function Ev_NpcTalk_rebuyBtn( strIndex )

	if false == NpcTalk_Rebuy( tonumber( strIndex ) ) then
		return
	end
	
	NpcTalk_EnableRebuyBtn( false )
	
end

-- 광고 구입 버튼 클릭(원용)
function Ev_NpcTalk_btnAD()
	PointShop_Reset()
	
	local tbID = SplitNativeID( g_NpcTalk:GetAdItemID() )
	g_tbNpcTalk_AD = g_NpcTalk:GetAdItemSearchString()
	
	if IsOpenWidget( WIDGET_ID_POINTSHOP ) == false then
		OpenWidget( WIDGET_ID_POINTSHOP )
	else
		OpenPointShop()
	end
	
end

-- 클럽 창설 메시지
function Ev_NpcTalk_CreateClub( tbData )

	if true == tbData[ 1 ] then
	
		local strClubName = GetMsgBoxInput()
		g_Msg:ReqClubNew( strClubName )
	
	end
	
	Event_MsgBox_Close()

end

function FrameMove_NpcTalk()

	if g_NpcTalk:IsUseable() == false then
	
		CloseWidget(WIDGET_ID_NPCTALK)		
	else
	
		NpcTalk_SetTimer()
		
	end	
	
end