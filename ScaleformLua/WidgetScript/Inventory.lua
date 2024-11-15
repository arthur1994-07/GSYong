  
-- 탭 인덱스
g_nInventory_PAGE_INVEN 	= 0
g_nInventory_PAGE_BANK 		= 1
g_nInventory_PAGE_WISHLIST 	= 2

g_nInventory_SelectInvenPage = 1		-- 선택중인 인벤 페이지
g_nInventory_SelectBankPage = 1			-- 선택중인 은행 페이지
g_nInventory_SelectWishListPage = 1		-- 선택중인 위시리스트 페이지

-- 은행 클릭한 슬롯 위치
g_nInventory_ClickBankPosX = -1
g_nInventory_ClickBankPosY = -1

-- 은행 창으로 오픈?
g_bInventory_OpenBank = false

-- 캐릭터창 열려있나?
g_bInventory_IsOpenCharInfo = false

-- 아이템 분해 중인가?
g_bInventory_ItemDecompose = false
-- 아이템 분해 위치
g_nInventory_ItemDecomposeX = -1
g_nInventory_ItemDecomposeY = -1

-- 인벤토리 UI 로드 완료
function LoadInventory()

	g_bInventory_ItemDecompose = false

	-- 탭 데이터
	local strTabData = GetGameWordText( "INVENTORY_TAP_NAME", 0 ) .. ";" .. 
		GetGameWordText( "INVENTORY_TAP_NAME", 1 ) .. ";" .. 
		GetGameWordText( "INVENTORY_TAP_NAME", 2 ) .. ";"
	SetData( WIDGET_ID_INVENTORY, strTabData, "btnbarTab", false )
	SetEvent( WIDGET_ID_INVENTORY, EVENT_INDEX_CLICK_CHANGE, "Ev_Inventory_btnbarTab", "index", "btnbarTab" )
	
	-- 하이라이트 설정
	HitTestEnable( WIDGET_ID_INVENTORY, false, "highlight" )
	SetBool( WIDGET_ID_INVENTORY, "visible", false, "highlight" )
	
	-- 비활성 탭 숨기기
	SetBool( WIDGET_ID_INVENTORY, "visible", false, "bank" )
	SetBool( WIDGET_ID_INVENTORY, "visible", false, "wishList" )
	
	-- 슬롯 바인딩 ID 설정
	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local strSlotNum = tostring( i )
		
		SetString( WIDGET_ID_INVENTORY, "binding", 
			SLOT_INVENTORY_ID .. strSlotNum, "inventory.slot" .. strSlotNum )
		SetString( WIDGET_ID_INVENTORY, "binding", 
			SLOT_INVENTORY_BANK_ID .. strSlotNum, "bank.slot" .. strSlotNum )
		SetString( WIDGET_ID_INVENTORY, "binding", 
			SLOT_INVENTORY_WISHLIST_ID .. strSlotNum, "wishList.slot" .. strSlotNum )
			
		SetBool( WIDGET_ID_INVENTORY, "holdable", true, "inventory.slot" .. strSlotNum )
		SetBool( WIDGET_ID_INVENTORY, "dragable", true, "inventory.slot" .. strSlotNum )
		SetBool( WIDGET_ID_INVENTORY, "holdable", false, "bank.slot" .. strSlotNum )
		SetBool( WIDGET_ID_INVENTORY, "holdable", false, "wishList.slot" .. strSlotNum )
		
		SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_ROLLOVER, "Ev_Inventory_InvenSlotOver", "", 
			"inventory.slot" .. strSlotNum, strSlotNum )
		SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_ROLLOUT, "Ev_Inventory_InvenSlotOut", "",
			"inventory.slot" .. strSlotNum, strSlotNum )
		SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_CLICK, "Ev_Inventory_InvenSlotClick", "", 
			"inventory.slot" .. strSlotNum, strSlotNum )
	
	end
	
	
	-- 페이지 라디오 버튼 이벤트 및 선택
	for i = 1, 5, 1 do
	
		local strInst = "inventory.radioPage" .. tostring( i )
		
		-- 인벤토리
		SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_InvenPage", "", strInst, tostring( i ) )
		SetBool( WIDGET_ID_INVENTORY, "keepSlot", true, strInst )
		
		if i == g_nInventory_SelectInvenPage then
			SetBool( WIDGET_ID_INVENTORY, "selected", true, strInst )
		else
			SetBool( WIDGET_ID_INVENTORY, "selected", false, strInst )
		end
		
		strInst = "bank.radioPage" .. tostring( i )
		
		-- 은행
		SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_BankPage", "", strInst, tostring( i ) )
		SetBool( WIDGET_ID_INVENTORY, "keepSlot", true, strInst )
		
		if i == g_nInventory_SelectBankPage then
			SetBool( WIDGET_ID_INVENTORY, "selected", true, strInst )
		else
			SetBool( WIDGET_ID_INVENTORY, "selected", false, strInst )
		end
		
		strInst = "wishList.radioPage" .. tostring( i )
		
		-- 위시리스트
		SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_WishListPage", "", strInst, tostring( i ) )
		SetBool( WIDGET_ID_INVENTORY, "keepSlot", true, strInst )
		
		if i == g_nInventory_SelectWishListPage then
			SetBool( WIDGET_ID_INVENTORY, "selected", true, strInst )
		else
			SetBool( WIDGET_ID_INVENTORY, "selected", false, strInst )
		end
		
	end
	
	-- 툴팁
	SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_HelpInven", "", "inventory.btnHelp" )
	SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_HelpBank", "", "bank.btnHelp" )
	SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_HelpWish", "", "wishList.btnHelp" )
	
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameInternalText( "INVENTORY_POINT_TEXT", 0 ), "bottom.iconPoint" )
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameInternalText( "INVENTORY_MONEY_TEXT", 0 ), "bottom.iconMoney" )
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameWordText( "INVENTORY_BUTTON_TEXT", 0 ), "bottom.btnLocker" )
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameWordText( "INVENTORY_BUTTON_TEXT", 1 ), "bottom.btnExtend" )
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameWordText( "INVENTORY_BUTTON_TEXT", 2 ), "bottom.btnTrash" )
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameWordText( "INVENTORY_BUTTON_TEXT", 3 ), "bottom.btnSort" )
	SetString( WIDGET_ID_INVENTORY, "TooltipText", GetGameWordText( "INVENTORY_BUTTON_TEXT", 4 ), "bottom.btnHammer" )
	
	-- 소지금 아이콘 클릭 이벤트
	SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_CLICK, "Ev_Inventory_iconMoneyClick", "", "bottom.iconMoney" )
	-- 포인트 클릭
	SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_CLICK, "Ev_Inventory_iconPointClick", "", "bottom.iconPoint" )
	
	-- 라커 버튼 이벤트
	SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_btnLocker", "", "bottom.btnLocker" )
	
	if g_UserFeatures:IsUsingPointShop() == true then
		-- 확장 버튼 이벤트
		SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_btnExtend", "", "bottom.btnExtend" )
	else
		SetBool( WIDGET_ID_INVENTORY, "enabled", false, "bottom.btnExtend" )
	end
	
	-- 휴지통 버튼 이벤트
	SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_btnTrash", "", "bottom.btnTrash" )
	-- 정렬 버튼 이벤트
	SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_btnSort", "", "bottom.btnSort" )
	
	-- 포인트 샵 머니
	SetString( WIDGET_ID_INVENTORY, "text", CurrencyFormat( g_MyChar:GetPointShopPoint() ), "bottom.labelPoint" )
	-- 소지금
	SetString( WIDGET_ID_INVENTORY, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "bottom.labelMoney" )
	
	-- 인벤 사용 가능 슬롯 설정
	Inventory_SetLockIven()
	
	-- 인벤 슬롯 아이템 설정
	Inventory_SetInvenSlotItem( g_nInventory_SelectInvenPage )
	-- 위시리스트 슬롯 아이템 설정
	Inventory_SetWishSlotItem( g_nInventory_SelectWishListPage )
	
	-- 은행 아이템 요청할 수 있는지 확인
	-- 요청할 수 없으면 바로 슬롯 아이템 설정
	-- 요청할 수 있으면 서버에서 정보를 받아올 떄까지 대기
	if false == g_Msg:TryRequestBankItemInfo() then
		Inventory_SetBankSlotItem( g_nInventory_SelectBankPage )
	end
	
	-- 은행창으로 오픈?
	if true == g_bInventory_OpenBank then

		SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_BANK, "btnbarTab" )
		
	-- 아니면 인벤으로 오픈
	else
	
		SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_INVEN, "btnbarTab" )
	
	end
	
	-- 아이템 분해
	if true == g_UserFeatures:IsUsingItemDecompose() then
	
		SetEvent( WIDGET_ID_INVENTORY, EVENT_BUTTON_CLICK, "Ev_Inventory_ItemDecompose", "", "bottom.btnHammer" )
	
	else
	
		SetBool( WIDGET_ID_INVENTORY, "enabled", false, "bottom.btnHammer" )
	
	end

end

-- 오픈
function OpenInventory()

	-- 개인상점 등록 아이템 표시
	Inventory_DisplayPrivateMarketRegItem( false )

end

-- 툴팁 이벤트 설정
function Inventory_SetEvent_Tooltip( strInstance, strOverEvent, strOutEvent )

	SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_ROLLOVER, strOverEvent, "", strInstance )
	SetEvent( WIDGET_ID_INVENTORY, EVENT_MOUSE_ROLLOUT, strOutEvent, "", strInstance )
	
end

-- 인벤토리 사용 가능 슬롯 설정
function Inventory_SetLockIven()

	local nInvenEnabledLine = g_MyChar:GetOnInvenLine() + g_nINVEN_DEFAULT_LINE
	
	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nPosY = math.floor( ( i + ( g_nInventory_SelectInvenPage - 1 ) * 
			INVEN_SLOT_COUNT_TOTAL ) / INVEN_SLOT_COUNT_COL )
		
		-- 사용 가능
		if nPosY < nInvenEnabledLine then
		
			SetBool( WIDGET_ID_INVENTORY, "lock", false, "inventory.slot" .. tostring( i ) )
			SetString( WIDGET_ID_INVENTORY, "TooltipText", "", "inventory.slot" .. tostring( i ) )
		
		-- 사용 불가능
		else
		
			SetBool( WIDGET_ID_INVENTORY, "lock", true, "inventory.slot" .. tostring( i ) )
			
			local str = GetGameInternalText( "INVENTORY_INVALID_SLOT", 0 )
			SetString( WIDGET_ID_INVENTORY, "TooltipText", str, "inventory.slot" .. tostring( i ) )
		
		end
	
	end
	
end

-- 슬롯 인덱스를 x, y 위치로 변환
function Inventory_SlotIndexToPos( nIndex, nPage )

	local nPosX = ( nIndex + ( nPage - 1 ) * INVEN_SLOT_COUNT_TOTAL ) % INVEN_SLOT_COUNT_COL
	local nPosY = math.floor( ( nIndex + ( nPage - 1 ) * INVEN_SLOT_COUNT_TOTAL ) / INVEN_SLOT_COUNT_COL )
	
	return nPosX, nPosY

end

-- 슬롯 x, y 위치를 인덱스로 변환
function Inventory_SlotPosToIndex( nPosX, nPosY )

	return math.floor( ( nPosY * INVEN_SLOT_COUNT_COL + nPosX ) % INVEN_SLOT_COUNT_TOTAL )	

end

-- 인벤토리 슬롯 아이템 설정
function Inventory_SetInvenSlotItem( nPageIndex )

	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nPosX, nPosY = Inventory_SlotIndexToPos( i, nPageIndex )
		SetSlot_Inventory( SLOT_INVENTORY_ID .. tostring( i ), nPosX, nPosY )
		
		-- 아이템 분해 하이라이트
		if true == g_bInventory_ItemDecompose then
			Inventory_HighlightItemDecompose( nPosX, nPosY, "inventory.slot" .. i )
		end
	
	end
	
end

-- 은행 슬롯 아이템 설정
function Inventory_SetBankSlotItem( nPageIndex )

	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nPosX, nPosY = Inventory_SlotIndexToPos( i, nPageIndex )
		SetSlot_Bank( SLOT_INVENTORY_BANK_ID .. tostring( i ), nPosX, nPosY )
	
	end

end

-- 위시리스트 슬롯 아이템 설정
function Inventory_SetWishSlotItem( nPageIndex )

	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local nPosX, nPosY = Inventory_SlotIndexToPos( i, nPageIndex )
		SetSlot_Wishlist( SLOT_INVENTORY_WISHLIST_ID .. tostring( i ), nPosX, nPosY )
	
	end

end

-- 캐릭터창 열림 설정
function Inventory_SetOpenCharInfo( bOpen )

	g_bInventory_IsOpenCharInfo = bOpen

end

-- 아이템 분해 하이라이트
function Inventory_SetHighlightItemDecompose()

	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local strInst = "inventory.slot" .. i
	
		-- 드래그 가능 여부 설정
		SetBool( WIDGET_ID_INVENTORY, "holdable", not g_bInventory_ItemDecompose, strInst )
		
		local nPosX, nPosY = Inventory_SlotIndexToPos( i, g_nInventory_SelectInvenPage )
		Inventory_HighlightItemDecompose( nPosX, nPosY, strInst )

	end

end

-- 아이템 분해 하이라이트
function Inventory_HighlightItemDecompose( nPosX, nPosY, strInst )

	-- 분해 가능한 아이템 하이라이트
	if true == g_bInventory_ItemDecompose then 
	
		if true == g_MyChar:CheckInvenItemDecompose( nPosX, nPosY ) then
		
			SetBool( WIDGET_ID_INVENTORY, "slotHighlight", true, strInst )
			SetBool( WIDGET_ID_INVENTORY, "enabled", true, strInst )
		
		else
		
			SetBool( WIDGET_ID_INVENTORY, "slotHighlight", false, strInst )
			SetBool( WIDGET_ID_INVENTORY, "enabled", false, strInst )
		
		end
		
	else
	
		SetBool( WIDGET_ID_INVENTORY, "slotHighlight", false, strInst )
		SetBool( WIDGET_ID_INVENTORY, "enabled", true, strInst )
	
	end

end

-- 인벤토리 슬롯 드래그 가능 여부 설정
function Inventory_SetDragableInvenSlot( bEnable )

	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local strInst = "inventory.slot" .. i
		SetBool( WIDGET_ID_INVENTORY, "holdable", bEnable, strInst )
		
	end

end

-- 개인상점 등록 아이템 표시
function Inventory_DisplayPrivateMarketRegItem( bForceEnable )

	if false == bForceEnable then
	
		if 	false == g_PrivateMarket_bOpener or
			false == IsOpenWidget( WIDGET_ID_PRIVATE_MARKET ) then
			
			return
			
		end
	
	end
	
	for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
	
		local strInst = "inventory.slot" .. i
		
		if true == bForceEnable then
		
			SetBool( WIDGET_ID_INVENTORY, "enabled", true, strInst )
		
		else
		
			local nPosX, nPosY = Inventory_SlotIndexToPos( i, g_nInventory_SelectInvenPage )
			
			local bSold = g_PrivateMarket:IsSold( nPosX, nPosY )
			if false == bSold then
			
				local bReg = g_PrivateMarket:IsRegInven( nPosX, nPosY )
				SetBool( WIDGET_ID_INVENTORY, "enabled", not bReg, strInst )
			
			else
			
				SetBool( WIDGET_ID_INVENTORY, "enabled", true, strInst )
			
			end
			
		end
	
	end

end

-----------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 소지금 업데이트
function Inventory_UpdateMoney( nMoney )

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	SetString( WIDGET_ID_INVENTORY, "text", CurrencyFormat( nMoney ), "bottom.labelMoney" )

end

-- 포인트샵 머니 업데이트
function Inventory_UpdatePoint( nPoint )

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	SetString( WIDGET_ID_INVENTORY, "text", CurrencyFormat( nPoint ), "bottom.labelPoint" )

end

-- 인벤토리 슬롯 전체 업데이트
function Inventory_InvenAllSlotUpdate()

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	Inventory_SetInvenSlotItem( g_nInventory_SelectInvenPage )
	
end

-- 인벤토리 슬롯 업데이트
function Inventory_InvenSlotUpdate( nPosX, nPosY )

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	local nMin = (g_nInventory_SelectInvenPage-1) * INVEN_SLOT_COUNT_ROW
	local nMax = g_nInventory_SelectInvenPage * INVEN_SLOT_COUNT_ROW
	if nMin <= nPosY and nPosY < nMax then

		local nIndex = Inventory_SlotPosToIndex( nPosX, nPosY )
		SetSlot_Inventory( SLOT_INVENTORY_ID .. tostring( nIndex ), nPosX, nPosY )
		
		if true == g_bInventory_ItemDecompose then
			Inventory_HighlightItemDecompose( nPosX, nPosY, "inventory.slot" .. nIndex )
		end
		
	end
	
end

-- 인벤토리 은행 슬롯 클릭
function Inventory_BankItemClick( strName, nPosX, nPosY )

	local strMsg = StringFormat( GetGameInternalText( "MODAL_ITEMBANK_GET_ITEM", 0 ), strName ) .. "\n" .. 
		GetGameInternalText( "MODAL_ITEMBANK_GET_ITEM", 1 )

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Inventory_BankItemClick, Event_MsgBox_Close )
	
	g_nInventory_ClickBankPosX = nPosX
	g_nInventory_ClickBankPosY = nPosY

end

-- 인벤토리 은행 전체 슬롯 업데이트
function Inventory_BankAllSlotUpdate()

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	Inventory_SetBankSlotItem( g_nInventory_SelectBankPage )

end

-- 인벤토리 은행 슬롯 업데이트
function Inventory_BankSlotUpdate( nPosX, nPosY )

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end

	local nIndex = Inventory_SlotPosToIndex( nPosX, nPosY )
	SetSlot_Bank( SLOT_INVENTORY_BANK_ID .. tostring( nIndex ), nPosX, nPosY )

end

-- 인벤토리 위시리스트 슬롯 업데이트
function Inventory_WishSlotUpdate( nPosX, nPosY )

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	local nIndex = Inventory_SlotPosToIndex( nPosX, nPosY )
	SetSlot_Wishlist( SLOT_INVENTORY_WISHLIST_ID .. tostring( nIndex ), nPosX, nPosY )

end

-- 인벤토리 슬롯 확장
function Inventory_AddInvenLine()

	Inventory_SetLockIven()

end

-- 인벤토리 단축키
function Inventory_HotKeyInven()

	-- 인벤토리로 열거임.
	g_bInventory_OpenBank = false

	-- 로드 안됐으면  UI 열기
	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
	
		OpenWidget( WIDGET_ID_INVENTORY )
		
	-- UI 르도되었으면
	else
	
		if false == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		
			OpenWidget( WIDGET_ID_INVENTORY )
			SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_INVEN, "btnbarTab" )
			
		else
			
			-- 인벤 페이지 활성이 아니면 인벤 페이지로 이동
			if false == GetBool( WIDGET_ID_INVENTORY, "visible", "inventory" ) then
			
				SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_INVEN, "btnbarTab" )
				
			-- 인벤 페이지가 활성이면 UI 끄기
			else
			
				CloseWidget( WIDGET_ID_INVENTORY )
			
			end
		
		end
		
	end

end

-- 은행 단축키
function Inventory_HotKeyBank()

	-- 은행으로 열거임.
	g_bInventory_OpenBank = true
	
	-- 로드 안됐으면 UI 열기
	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
	
		OpenWidget( WIDGET_ID_INVENTORY )
		
	-- UI 로드되었으면
	else
	
		if false == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		
			OpenWidget( WIDGET_ID_INVENTORY )
			SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_BANK, "btnbarTab" )
			
		else
		
			-- 은행 페이지 활성이 아니면 은행 페이지로 이동
			if false == GetBool( WIDGET_ID_INVENTORY, "visible", "bank" ) then
			
				SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_BANK, "btnbarTab" )
				
			-- 은행 페이지가 활성이면 UI 끄기
			else
			
				CloseWidget( WIDGET_ID_INVENTORY )
			
			end
			
		end
		
	end

end

-- 인벤토리 정렬 가능한가?
function IsInvenItemSort()

	if 	true == IsOpenWidget( WIDGET_ID_INVENTORY_TRASH ) or
		true == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) or
		true == IsOpenWidget( WIDGET_ID_PRIVATE_MARKET ) then
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "ITEM_SORT_DISABLE", 0 ), TEXT_COLOR_RED )
		
		return false
		
	end
	
	return true

end

-- 하이라이트 켜기/끄기
function Inventory_SetVisibleHighlight( bVisible, nIndex )

	if false == IsLoadWidget( WIDGET_ID_INVENTORY ) then
		return
	end

	SetBool( WIDGET_ID_INVENTORY, "visible", bVisible, "highlight" )
	
	if false == bVisible or -1 == nIndex then
		return
	end
	
	local strInst = ""
	
	local nTab = GetInt( WIDGET_ID_INVENTORY, "selectedIndex", "btnbarTab" )
	if 0 == nTab then
	
		strInst = "inventory"
	
	elseif 1 == nTab then
	
		strInst = "bank"
	
	elseif 2 == nTab then
	
		strInst = "wishList"
	
	else
	
		return
		
	end
	
	local nX = GetInt( WIDGET_ID_INVENTORY, "x", strInst ) + 
		GetInt( WIDGET_ID_INVENTORY, "x", strInst .. ".slot" .. nIndex )
	local nY = GetInt( WIDGET_ID_INVENTORY, "y", strInst ) +
		GetInt( WIDGET_ID_INVENTORY, "y", strInst .. ".slot" .. nIndex )
		
	SetInt( WIDGET_ID_INVENTORY, "x", nX, "highlight" )
	SetInt( WIDGET_ID_INVENTORY, "y", nY, "highlight" )

end

-- 인벤토리에 특정 아이템 하이라이트
function Inventory_HighlightItem( nMID, nSID, nCount )

if false == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	if nCount == 0 or nil == nCount then
		return
	end

	local nTab = GetInt( WIDGET_ID_INVENTORY, "selectedIndex", "btnbarTab" )
	if 0 ~= nTab then
		return
	end

	local nAryIndex = {}
	local nIdxCount = 0
	
	if -1 == nMID or -1 == nSID then
	
		Inventory_SetVisibleMultiHighlight( false, -1, nil )
		
	else
		local nSelectRadioPage = 0
		
		for i = 1, 5, 1 do				
			if GetBool( WIDGET_ID_INVENTORY, "selected", "inventory.radioPage" .. i ) == true then
					nSelectRadioPage = i - 1
			end
		end		
				
		local CheckCount = nCount
				
		-- 카운트 인덱스는 0부터 시작하기때문에 -1 해줘야함
		for idx = 0, nCount - 1, 1 do

			local tbItemPos = g_MyChar:FindInvenItemPos( nMID, nSID, idx )
			if 	nil ~= tbItemPos[ 1 ] and nil ~= tbItemPos[ 2 ] then			
			 -- 자동 페이지 변경이 동작하지 않도록 설정
				---- 페이지 변경
				--local nPage = math.floor( tbItemPos[ 2 ] / INVEN_SLOT_COUNT_ROW ) + 1
				--for i = 1, 5, 1 do				
					--local bSelect = false
					--if i == nPage then
						--bSelect = true
					--end				
					--SetBool( WIDGET_ID_INVENTORY, "selected", bSelect, "inventory.radioPage" .. i )				
				--end				
				--Ev_Inventory_InvenPage( nPage )		
								
				-- 하이라이트 설정			-- 현재 선택한 페이지만 하이라이트 설정
				if (tbItemPos[2] % 10)+(nSelectRadioPage*10) == tbItemPos[2] then
					nAryIndex[nIdxCount + 1] = Inventory_SlotPosToIndex( tbItemPos[ 1 ], tbItemPos[ 2 ] )
					nIdxCount = nIdxCount + 1
				end

				local itemCount = g_MyChar:GetSlotTurnNumCountItem( tbItemPos[1], tbItemPos[2] )
				CheckCount = CheckCount - itemCount

				if CheckCount <= 0 then
					break 					
				end
				
			end
		end
		
		Inventory_SetVisibleMultiHighlight( true, nIdxCount, nAryIndex )
	end
end


function Inventory_SetVisibleMultiHighlight( bVisible, nCount, aryIndex )
	if false == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		return
	end

	local strInst = ""
	
	local nTab = GetInt( WIDGET_ID_INVENTORY, "selectedIndex", "btnbarTab" )
	if 0 == nTab then
	
		strInst = "inventory"
	
	elseif 1 == nTab then
	
		strInst = "bank"
	
	elseif 2 == nTab then
	
		strInst = "wishList"
	
	else
		return
		
	end
	
	if false == bVisible then
		for i = 0, INVEN_SLOT_COUNT_TOTAL - 1, 1 do
			SetBool( WIDGET_ID_INVENTORY, "visible", bVisible, strInst .. ".slot" .. i .. ".mcHighlight")
		end	
	else
		for i = 1, nCount, 1 do
			SetBool( WIDGET_ID_INVENTORY, "visible", bVisible, strInst .. ".slot" .. aryIndex[i] .. ".mcHighlight" )
		end
	end
end

-- 아이템 분해중인가?
function Inventory_IsItemDecompose()

	return g_bInventory_ItemDecompose
	
end

-- 아이템 분해 확인
function Inventory_ConfirmItemDecompose( nPosX, nPosY )

	g_nInventory_ItemDecomposeX = nPosX
	g_nInventory_ItemDecomposeY = nPosY
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "ITEM_DECOMPOSE_OKQUESTION", 0 ),
		Ev_Intentory_ItemDecomposeOK, Event_MsgBox_Close )

end

-----------------------------------------------------------------------------------------------------
-- 이벤트 

-- 탭 변경 이벤트
function Ev_Inventory_btnbarTab( nIndex )

	if g_nInventory_PAGE_INVEN == nIndex then
	
		SetBool( WIDGET_ID_INVENTORY, "visible", true, "inventory" )
		SetBool( WIDGET_ID_INVENTORY, "visible", false, "bank" )
		SetBool( WIDGET_ID_INVENTORY, "visible", false, "wishList" )
		
		Inventory_SetInvenSlotItem( g_nInventory_SelectInvenPage )
		
	elseif g_nInventory_PAGE_BANK == nIndex then
	
		SetBool( WIDGET_ID_INVENTORY, "visible", false, "inventory" )
		SetBool( WIDGET_ID_INVENTORY, "visible", true, "bank" )
		SetBool( WIDGET_ID_INVENTORY, "visible", false, "wishList" )
		
		Inventory_SetBankSlotItem( g_nInventory_SelectBankPage )
		
		g_Msg:ReqItemBankInfo( false )
		
		local str = GetGameInternalText( "BANK_UPDATE_INFO", 0 )
		CF_PrintMsg_SystemMsg( str, TEXT_COLOR_RED )
	
	elseif g_nInventory_PAGE_WISHLIST == nIndex then
	
		SetBool( WIDGET_ID_INVENTORY, "visible", false, "inventory" )
		SetBool( WIDGET_ID_INVENTORY, "visible", false, "bank" )
		SetBool( WIDGET_ID_INVENTORY, "visible", true, "wishList" )
		
		Inventory_SetWishSlotItem( g_nInventory_SelectWishListPage )
	
	end

end

-- 인벤 페이지 라디오 버튼 이벤트
function Ev_Inventory_InvenPage( strIndex )

	local nIndex = tonumber( strIndex )
	
	g_nInventory_SelectInvenPage = nIndex
	Inventory_SetInvenSlotItem( g_nInventory_SelectInvenPage )
	Inventory_SetLockIven()
	
	if true == PrivateMarket_IsOpener() then

		-- 개인상점 인벤토리 등록된 아이템 표시
		Inventory_DisplayPrivateMarketRegItem( false )
	
		Inventory_SetDragableInvenSlot( false )
		return
		
	end
	
	-- 거래 등록된 인벤 아이템 있으면 슬롯 비활성화
	if true == IsOpenWidget( WIDGET_ID_TRADE ) then
	
		Trade_InvenItemEnable()
		Trade_SetRegInvenItemEnable( false )
		
	end
	
	-- 분해 하이라이트 설정
	if true == g_bInventory_ItemDecompose then
	
		Inventory_SetHighlightItemDecompose()
	
	end
	
end

-- 은행 페이지 라디오 버튼 이벤트
function Ev_Inventory_BankPage( strIndex )

	local nIndex = tonumber( strIndex )

	g_nInventory_SelectBankPage = nIndex
	Inventory_SetBankSlotItem( g_nInventory_SelectBankPage )
	
end

-- 위시리스트 페이지 라디오 버튼 이벤트
function Ev_Inventory_WishListPage( strIndex )

	local nIndex = tonumber( strIndex )

	g_nInventory_SelectWishListPage = nIndex
	Inventory_SetWishSlotItem( g_nInventory_SelectWishListPage )
	
end

-- 도움말
function Ev_Inventory_HelpInven()

	HelpTooltip_SetText( GetGameInternalText( "INVENTORY_HELP_INVEN", 0 ) )

end

function Ev_Inventory_HelpBank()

	HelpTooltip_SetText( GetGameInternalText( "INVENTORY_HELP_BANK", 0 ) )

end

function Ev_Inventory_HelpWish()

	HelpTooltip_SetText( GetGameInternalText( "INVENTORY_HELP_WISH", 0 ) )

end

-- 소지금 아이콘 클릭 이벤트
function Ev_Inventory_iconMoneyClick()

	-- 아무것도 안열려 있으면 돈 바닥에 버리기
	if true == g_bMONEY_DROP2FIELD then
	
		OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameInternalText( "MONEYTO_FIELD", 0 ),
			Ev_Inventory_DropMoney, Event_MsgBox_Close )
		
	end

end

-- 포인트샵 열기
function Ev_Inventory_iconPointClick()

	OpenWidget( WIDGET_ID_POINTSHOP )
	
end

-- 돈 버리기 확인
function Ev_Inventory_DropMoney()

	local nMoney = tonumber( GetMsgBoxInput() )
	local nInvenMoney = g_MyChar:GetInvenMoney()
	
	if nMoney > nInvenMoney then
		nMoney = nInvenMoney
	end
	
	g_Msg:MoneyToField( nMoney )
	Event_MsgBox_Close()

end

-- 라커 버튼 이벤트
function Ev_Inventory_btnLocker()

	-- 락커 이미 열려있음
	if true == IsOpenWidget( WIDGET_ID_LOCKER ) then
		return
	end
	
	-- 락커 연결 메시지 박스 열렸나
	if true == g_bLocker_IsOpenMsgBox then
		return
	end

	-- 락커 열기 시도
	if false == g_Msg:ReqStorageOpenFromInven() then
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "INVEN_LOCKER_OPEN_FAIL", 0 ), TEXT_COLOR_RED )
		
	end

end

-- 확장 버튼 이벤트
function Ev_Inventory_btnExtend()
	PointShop_Reset()
	
	-- 확장버튼을 눌러 포인트샾 오픈 상태로 변경
	g_Inventory_ExtendButton = true
	
	if IsOpenWidget( WIDGET_ID_POINTSHOP ) == false then
		OpenWidget( WIDGET_ID_POINTSHOP )
	else
		OpenPointShop()
	end
	--PointShop_Update()
end

-- 휴지통 버튼 이벤트
function Ev_Inventory_btnTrash()

	if false == IsOpenWidget( WIDGET_ID_INVENTORY_TRASH ) then
		SetInt( WIDGET_ID_INVENTORY, "selectedIndex", g_nInventory_PAGE_INVEN, "btnbarTab" )
	end

	ToggleUI( WIDGET_ID_INVENTORY_TRASH )
	
end

-- 정렬 버튼 이벤트
function Ev_Inventory_btnSort()

	g_Msg:InvenItemSort()
	
end

-- 인벤토리 은행 아이템 클릭, 메시지 박스 예
function Ev_Inventory_BankItemClick()

	-- 은행 아이템 인벤으로 옮겨달라 서버에 요청
	g_Msg:ReqChargedItemTo( g_nInventory_ClickBankPosX, g_nInventory_ClickBankPosY )
	Event_MsgBox_Close()

end

-- 인벤토리 슬롯 마우스 오버, 캐릭터 능력 증감치 비교
function Ev_Inventory_InvenSlotOver( strSlotIndex )

	-- 캐릭터창이 안열려 있으면 할 필요 없다.
	if false == g_bInventory_IsOpenCharInfo then
		return
	end
	
	local nPosX, nPosY = Inventory_SlotIndexToPos( tonumber( strSlotIndex ), g_nInventory_SelectInvenPage )	
	CharInfo_ViewAddon_CompareInvenItem( nPosX, nPosY )
	CharInfo_DisplayEquipSlot( nPosX, nPosY, true )

end

-- 인벤토리 슬롯 마우스 아웃, 캐릭터 능력 증감치 없애기
function Ev_Inventory_InvenSlotOut( strSlotIndex )

	-- 캐릭터창이 안열려 있으면 할 필요 없다.
	if false == g_bInventory_IsOpenCharInfo then
		return
	end
	
	CharInfo_UpdateChnageUpAbillity( false )
	CharInfo_ViewAddonPage_ResetChange()
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateResist()
	
	local nPosX, nPosY = Inventory_SlotIndexToPos( tonumber( strSlotIndex ), g_nInventory_SelectInvenPage )	
	CharInfo_DisplayEquipSlot( nPosX, nPosY, false )

end

-- 슬롯 클릭
function Ev_Inventory_InvenSlotClick( strSlotIndex )

	Ev_Inventory_InvenSlotOut( strSlotIndex )

end

-- 아이템 분해 버튼 클릭
function Ev_Inventory_ItemDecompose()

	g_bInventory_ItemDecompose = not g_bInventory_ItemDecompose
	
	if true == g_bInventory_ItemDecompose then
	
		if 	true == IsOpenWidget( WIDGET_ID_TRADE ) or
			true == IsOpenWidget( WIDGET_ID_PRIVATE_MARKET ) then
			
			CF_PrintMsg_SystemMsg( GetGameInternalText( "INVEN_DECOMPOSE_UNFITSTATE", 0 ), TEXT_COLOR_RED )
			return
			
		end
	
	end
	
	-- 하이라이트 설정
	Inventory_SetHighlightItemDecompose()

end

-- 아이템 분해 메시지 확인
function Ev_Intentory_ItemDecomposeOK()

	Event_MsgBox_Close()
	
	g_Msg:ReqInvenItemDecompose( g_nInventory_ItemDecomposeX, g_nInventory_ItemDecomposeY )

end

function Inventory_MovePage(nIndex)
	if IsOpenWidget(WIDGET_ID_INVENTORY) == false then
		-- g_nInventory_SelectInvenPage = nIndex
		OpenWidget( WIDGET_ID_INVENTORY )
	end
	SetBool( WIDGET_ID_INVENTORY, "selected", true, "inventory.radioPage" .. nIndex )
	Ev_Inventory_InvenPage(nIndex)
end