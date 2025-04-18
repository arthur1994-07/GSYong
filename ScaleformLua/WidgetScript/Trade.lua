
-- 거래 등록된 인벤토리 아이템 위치
g_tbTrade_DisableInventory = {}

-- 거래할때 내가 입력한 금액
g_Trade_Money = 0

-- 거래 요청 받은 상태인가
g_bTrade_IsRecvMsg = false

function LoadTrade()

	SetEvent( WIDGET_ID_TRADE, EVENT_WIDGET_CLOSE, "CloseTrade", "", "" )

	-- 타이틀
	local strText = GetGameWordText( "TRADE_NAME_STATIC" )
	SetString( WIDGET_ID_TRADE, "title", strText, "" )
	
	-- 거래창 잠금
	strText = GetGameWordText( "TRADE_LOCK_BUTTON" )
	SetString( WIDGET_ID_TRADE, "label", strText, "Trade_Mine.Mine_Button_Lock" )
	SetString( WIDGET_ID_TRADE, "label", strText, "Trade_Oppsite.Oppsite_Button_Lock" )
	HitTestEnable( WIDGET_ID_TRADE, false, "Trade_Oppsite.Oppsite_Button_Lock" )
	SetEvent( WIDGET_ID_TRADE, EVENT_MOUSE_CLICK, "Event_Trade_Lock" ,"", "Trade_Mine.Mine_Button_Lock" )
	
	-- 승인
	strText = GetGameWordText( "TRADE_OKCANCEL_BUTTON", 0 )
	SetString( WIDGET_ID_TRADE, "label", strText, "Trade_Mine.Mine_Button_Approve" )	
	SetString( WIDGET_ID_TRADE, "label", strText, "Trade_Oppsite.Oppsite_Button_Approve" )
	HitTestEnable( WIDGET_ID_TRADE, false, "Trade_Oppsite.Oppsite_Button_Approve" )
	SetEvent( WIDGET_ID_TRADE, EVENT_MOUSE_CLICK, "Event_Trade_Agree" ,"", "Trade_Mine.Mine_Button_Approve" )
	
	-- 거절
	strText = GetGameWordText( "TRADE_OKCANCEL_BUTTON", 1 )
	SetString( WIDGET_ID_TRADE, "label", strText, "Trade_Mine.Mine_Button_Reffuse" )
	SetEvent( WIDGET_ID_TRADE, EVENT_MOUSE_CLICK, "Event_Trade_Close" ,"", "Trade_Mine.Mine_Button_Reffuse" )
	
	-- 닫기 버튼 이벤트
	SetEvent( WIDGET_ID_TRADE, EVENT_BUTTON_CLICK, "Event_Trade_Close" ,"", "closeBtn" )
	
	-- 금액 입력 팁
	strText = GetGameInternalText( "TRADE_INPUT_MONEY", 0 )
	SetString( WIDGET_ID_TRADE, "TooltipText", strText, "Trade_Mine.Mine_TextField_Money" )
	SetBool( WIDGET_ID_TRADE, "currencyUnit", true, "Trade_Mine.Mine_TextField_Money" )
	
	-- 금액 입력 버튼
	strText = GetGameWordText( "TRADE_BTN_NAME_MONEYINPUT", 0 )
	SetString( WIDGET_ID_TRADE, "label", strText, "Trade_Mine.Mine_Button_MoneyInput" )
	SetEvent( WIDGET_ID_TRADE, EVENT_MOUSE_CLICK, "Ev_Trade_OpenMoneyInput", "", "Trade_Mine.Mine_Button_MoneyInput" )
	
	-- 슬롯 바인딩
	for i = 1, TRADE_SLOT_TOTAL, 1 do
	
		-- 상대방 슬롯
		local strSlotID = SLOT_TRADE_ID .. tostring( i )
		SetString( WIDGET_ID_TRADE, "binding", strSlotID, "Trade_Oppsite.Oppsite_Slot" .. tostring( i ) )
		-- 내 슬롯
		local strMySlotID = SLOT_TRADE_ID .. tostring( i + TRADE_SLOT_TOTAL )	
		SetString( WIDGET_ID_TRADE, "binding", strMySlotID, "Trade_Mine.Mine_Slot" .. tostring( i ) )
	
	end
	
end

function OpenTrade()

	-- 켜 있으면 안되는 UI들 다 끄자.
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_HAIR )
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_FACE )
	CloseWidget( WIDGET_ID_CHANGE_CARD_GENDER )
	CloseWidget( WIDGET_ID_PRIVATE_MARKET_SEARCH )
	CloseWidget( WIDGET_ID_VEHICLECOLOR )
	CloseWidget( WIDGET_ID_PETCOLOR )
	CloseWidget( WIDGET_ID_COSTUMECOLOR )
	CloseWidget( WIDGET_ID_REBUILD_CARD )
	CloseWidget( WIDGET_ID_TAXI_CARD )
	CloseWidget( WIDGET_ID_BUS_STATION )
	CloseWidget( WIDGET_ID_HAIRCOLOR )
	CloseWidget( WIDGET_ID_COSTUMESTAT )

	-- 거래 슬롯 초기화
	Trade_InitSlot()

	-- 잠금/승인 버튼
	SetBool( WIDGET_ID_TRADE, "enabled", true, "Trade_Mine.Mine_Button_Lock" )
	SetBool( WIDGET_ID_TRADE, "enabled", true, "Trade_Mine.Mine_Button_Approve")	
	SetBool( WIDGET_ID_TRADE, "enabled", true, "Trade_Oppsite.Oppsite_Button_Lock")
	SetBool( WIDGET_ID_TRADE, "enabled", true, "Trade_Oppsite.Oppsite_Button_Approve")
	SetBool( WIDGET_ID_TRADE, "enabled", true, "Trade_Mine.Mine_Button_MoneyInput" )
			
	-- 상대방 이름
	local strText = g_strInteractMenu_Name
	SetString( WIDGET_ID_TRADE, "text", strText, "Trade_Oppsite.Oppsite_TextField_Name" )
	
	-- 내 이름
	strText = g_MyChar:GetName()
	SetString( WIDGET_ID_TRADE, "text", strText, "Trade_Mine.Mine_TextField_Name" )
	
	-- 거래 금액
	strText = "0"
	SetString( WIDGET_ID_TRADE, "text", strText, "Trade_Mine.Mine_TextField_Money" )
	SetString( WIDGET_ID_TRADE, "text", strText, "Trade_Oppsite.Oppsite_TextField_Money" )
	
	-- 거래 금액 초기화
	g_Trade_Money = 0
	
	-- 인벤토리 열기
	OpenWidget( WIDGET_ID_INVENTORY )
	
	-- 인게임 클릭 안되도록 설정
	SetFakeModal( true )

end

function CloseTrade()

	SetFakeModal( false )
	
	-- 거절 메시지 서버로 쏴야함
	g_Msg:ReqTradeCancel()
	
	-- 인벤 아이템 다시 활성화
	Trade_SetRegInvenItemEnable( true )
	-- 인벤 아이템 초기화
	g_tbTrade_DisableInventory = {}

end

-- 거래 슬롯 인덱스 -> 위치
function Trade_SlotIndexToPos( nIndex )

	-- 인덱스는 1부터 시작
	nIndex = nIndex - 1

	local nPosX = nIndex % TRADE_SLOT_COL
	local nPosY = math.floor( nIndex / TRADE_SLOT_COL )
	
	return nPosX, nPosY

end

-- 거래 슬롯 위치 -> 인덱스
function Trade_SlotPosToIndex( nPosX, nPosY )

	-- 인덱스 1부터 시작하니 끝에 1 더하자
	return nPosY * TRADE_SLOT_COL + nPosX + 1

end

-- 거래 등록된 인벤 아이템 활성화 설정
function Trade_SetRegInvenItemEnable( bEnable )

	for value in pairs( g_tbTrade_DisableInventory ) do
	
		local tbInvenPos = g_tbTrade_DisableInventory[ value ]
		local nPageIndex = math.floor( tbInvenPos[ 2 ] / INVEN_SLOT_COUNT_ROW ) + 1
		
		-- 아이템이 인벤토리 현재 페이지에 있으면 업데이트
		if nPageIndex == g_nInventory_SelectInvenPage then

			local nIndex = Inventory_SlotPosToIndex( tbInvenPos[ 1 ], tbInvenPos[ 2 ] )
			SetBool( WIDGET_ID_INVENTORY, "enabled", bEnable, "inventory.slot" .. nIndex )
		
		end
	
	end

end

-- 트레이드 슬롯 설정
function Trade_InitSlot()

	for i = 1, TRADE_SLOT_TOTAL, 1 do
	
		local nPosX, nPosY = Trade_SlotIndexToPos( i )
		
		SetSlot_Trade( SLOT_TRADE_ID .. tostring( i ), nPosX, nPosY, false )
		SetSlot_Trade( SLOT_TRADE_ID .. tostring( i + TRADE_SLOT_TOTAL ), nPosX, nPosY, true )
		
	end
	
end

-- 금액 입력란에 입력된 금액을 서버에 전송
function Trade_InputMoney() 

	local nMoney = 0
	
	if true == GetBool( WIDGET_ID_TRADE, "enabled", "Trade_Mine.Mine_Button_Lock" ) then
	
		-- 거래금액 입력란에 입력한 데이터를 가져옴
		local strMoney = GetString( WIDGET_ID_TRADE, "text", "Trade_Mine.Mine_TextField_Money" )
		if strMoney == "" then
		
			-- 입력란에 값이 없을 시 시스템 메시지 출력
			CF_PrintMsg_SystemMsg( GetGameInternalText( "POSTBOX_ERROR_MSG_SEND_CONTENTS", 0 ), TEXT_COLOR_RED )
			return
			
		end

		nMoney = tonumber( InvCurrencyFormat( strMoney ) )
		if g_MyChar:GetInvenMoney() < nMoney then
		
			-- 입력한 금액이 소지 금액보다 적을 시 시스템 메시지 출력
			CF_PrintMsg_SystemMsg( GetGameInternalText( "POSTBOX_MSG_TAKE_CHAR_MONEY_LACK", 0 ), TEXT_COLOR_RED )
			
			nMoney = g_MyChar:GetInvenMoney()
			
		end
		
	else
	
		nMoney = g_Trade_Money
	
	end
	
	if true == g_Msg:ReqTradeMoney( nMoney ) then
	
		SetString( WIDGET_ID_TRADE, "text", CurrencyFormat( nMoney ), "Trade_Mine.Mine_TextField_Money" )
		g_Trade_Money = nMoney
		
	else
	
		SetString( WIDGET_ID_TRADE, "text", CurrencyFormat( g_Trade_Money ), "Trade_Mine.Mine_TextField_Money" )
		
	end
	
end

-- 거래창 열기
function Open_Trade_UI()

	if true ~= IsOpenWidget( WIDGET_ID_TRADE ) then
		OpenWidget( WIDGET_ID_TRADE )
	end	
	
end

-- 상대방 돈 입력창 
function SetTarTextMoney( Tar_Money )

	SetString( WIDGET_ID_TRADE, "text", CurrencyFormat( Tar_Money ), "Trade_Oppsite.Oppsite_TextField_Money" )
	
end

-- 상대방 잠금 버튼 설정
function SetTarButtonLock( bLock )

	SetBool( WIDGET_ID_TRADE, "enabled", not bLock, "Trade_Oppsite.Oppsite_Button_Lock" )
	
	Trade_CheckApprove()
	
end

-- 상대방 승인 버튼
function SetTarButtonAgree( bAgree ) 

	SetBool( WIDGET_ID_TRADE, "enabled", not bAgree, "Trade_Oppsite.Oppsite_Button_Approve" )
	
end

-- 내 잠금버튼
function SetMineButtonLock( bLock )

	SetBool( WIDGET_ID_TRADE, "enabled", not bLock, "Trade_Mine.Mine_Button_Lock" )
	
	Trade_CheckApprove()
	
end

-- 내 승인 버튼
function SetMineButtonAgree( bAgree )

	SetBool( WIDGET_ID_TRADE, "enabled", not bAgree, "Trade_Mine.Mine_Button_Approve" )
	
end

-- 내 잠금 버튼 설정 얻어오기
function GetPropertyLockButton() 

	return (not GetBool( WIDGET_ID_TRADE, "enabled", "Trade_Mine.Mine_Button_Lock" ))
	
end

-- 슬롯 업데이트
function Trade_UpdateSlot( nPosX, nPosY, bMySlot )

	local nIndex = Trade_SlotPosToIndex( nPosX, nPosY )
	
	if true == bMySlot then
	
		nIndex = nIndex + TRADE_SLOT_TOTAL
		
	end
	
	SetSlot_Trade( SLOT_TRADE_ID .. tostring( nIndex ), nPosX, nPosY, bMySlot )
	
end

-- 등록된 아이템 인벤토리 비활성
function Trade_UpdateInvenSlot( nPosX, nPosY, bEnable )

	-- 아이템이 인벤토리 현재 페이지에 있으면 바로 업데이트
	if math.floor( nPosY / INVEN_SLOT_COUNT_ROW ) + 1 == g_nInventory_SelectInvenPage then

		local nIndex = Inventory_SlotPosToIndex( nPosX, nPosY )
		SetBool( WIDGET_ID_INVENTORY, "enabled", bEnable, "inventory.slot" .. nIndex )
	
	end
	
	-- 등록된 인벤 아이템 위치 제거
	if true == bEnable then
	
		for value in pairs( g_tbTrade_DisableInventory ) do
		
			local tbInvenPos = g_tbTrade_DisableInventory[ value ]
			if tbInvenPos[ 1 ] == nPosX and tbInvenPos[ 2 ] == nPosY then
			
				table.remove( g_tbTrade_DisableInventory, value )
				break
			
			end
		
		end
	
	-- 인벤 아이템 위치 추가
	else
	
		local tb = 
		{
			nPosX,
			nPosY
		}
		table.insert( g_tbTrade_DisableInventory, tb )
	
	end

end

-- 승인 버튼 활성 체크
function Trade_CheckApprove()

	if 	false == GetBool( WIDGET_ID_TRADE, "enabled", "Trade_Mine.Mine_Button_Lock" ) and
		false == GetBool( WIDGET_ID_TRADE, "enabled", "Trade_Oppsite.Oppsite_Button_Lock" ) then
		
		SetBool( WIDGET_ID_TRADE, "enabled", true, "Trade_Mine.Mine_Button_Approve" )
				
	end
end

--------------------------------------MessageBox------------------------------------------------------------------
-- 거래창 메시지박스 UI 호출
function Trade_RecvMsg( strCharName )

	if true == g_bTrade_IsRecvMsg then
	
		Event_MsgBox_Close()
	
	end
	
	g_bTrade_IsRecvMsg = true

	CloseWidget( WIDGET_ID_INTERACT_MENU )

	g_strInteractMenu_Name = strCharName
	local str = StringFormat( GetGameInternalText( "TRADE_TAR", 0 ), g_strInteractMenu_Name )

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Event_MsgBox_TradeOpen, Ev_Trade_MsgRefuse )
	
end

-- 거래 확인
function Event_MsgBox_TradeOpen()

	Event_MsgBox_Close()
	if false == IsOpenWidget( WIDGET_ID_TRADE ) then
		g_Msg:ReqTradeAns( true )
	end
	
	g_bTrade_IsRecvMsg = false
	
end

-- 거래 거절
function Ev_Trade_MsgRefuse()

	Event_MsgBox_Close()
	
	g_Msg:ReqTradeAns( false )
	g_bTrade_IsRecvMsg = false

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트 

-- 거래 잠금 버튼 이벤트 
function Event_Trade_Lock()

	SetBool( WIDGET_ID_TRADE, "enabled", false, "Trade_Mine.Mine_Button_Lock" )
	SetBool( WIDGET_ID_TRADE, "enabled", false, "Trade_Mine.Mine_Button_MoneyInput" )
	
	g_Msg:ReqTradeLock()
	
	Trade_CheckApprove()
	
end

-- 승인 버튼 이벤트
function Event_Trade_Agree()

	g_Msg:ReqTradeAgree()
	
end

-- 거래창 x버튼이나, 거절 메시지로 끌때
function Event_Trade_Close()

	Event_MsgBox_Close()	
	
	-- 거절 메시지 서버로 쏴야함
	g_Msg:ReqTradeCancel()
	
end

-- 거래 등록된 인벤토리 아이템 활성화
-- 거래 아이템을 등록할 때 서버에서 메시지를 받아서 특정 인덱스를(슬롯) 비활성화 시킨다. 페이지를 구분해서 인덱스를(슬롯) 비활성화 시키지만
-- 페이지를 변경해도 그 인덱스는 계속해서 비활성화 되어 있는 문제가 있어서
-- 페이지를 이동할 때 먼저 비활성화된 인덱스를 활성화를 먼저 시켜준다.
-- http://172.16.2.87/redmine/issues/4114
function Trade_InvenItemEnable()

	for value in pairs( g_tbTrade_DisableInventory ) do
	
		local tbInvenPos = g_tbTrade_DisableInventory[ value ]
		local nIndex = Inventory_SlotPosToIndex( tbInvenPos[ 1 ], tbInvenPos[ 2 ] )
		SetBool( WIDGET_ID_INVENTORY, "enabled", true, "inventory.slot" .. nIndex )
	
	end

end

function Ev_Trade_OpenMoneyInput()

	SetBool( WIDGET_ID_MESSAGEBOX, "currencyUnit", true, "msgBoxInput.input" )

	-- 입력 메시지
	OpenMsgBox( MSG_BOX_TYPE_INPUT_MONEY, GetGameInternalText( "TRADE_MONEY", 0 ), 
		Ev_Trade_EnteredMoney, Event_MsgBox_Close )

end

function Ev_Trade_EnteredMoney()
	
	if GetMsgBoxInput() == "" then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PLAYINTERFACE_15", 0 ), TEXT_COLOR_RED )
		return
	end
	
	local nMoney = math.floor( tonumber( GetMsgBoxInput() ) )
	local InvenMoney = tonumber( g_MyChar:GetInvenMoney() )
	if nMoney > InvenMoney then
		nMoney = InvenMoney
	end
	
	SetString( WIDGET_ID_TRADE, "text", tostring(nMoney), "Trade_Mine.Mine_TextField_Money" )
	Trade_InputMoney()
	Event_MsgBox_Close()
	
end