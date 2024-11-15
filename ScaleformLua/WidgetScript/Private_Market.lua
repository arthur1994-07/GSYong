
-- 슬롯 개수
PIVATE_MARKET_SLOT_MAX = 24

-- 내 개인상점인가?
g_PrivateMarket_bOpener = false


-- 개인상점 UI 정보 -------------------------------------------

-- 선택한 아이템 개인상점 슬롯 위치
g_PrivateMarket_nSelectX = 0
g_PrivateMarket_nSelectY = 0

-- 개인상점 개설자 이름
g_PrivateMarket_StrName = ""
-- 개인상점 개설자 ChaNum
g_PrivateMarket_ChaNum = 0
-- 개인상점 타이틀
g_PrivateMarket_StrTitle = ""
-- 판매 등록된 아이템 리스트
g_PrivateMarket_tbItemList = {}
-- 업데이트 예약 여부
g_PrivateMarket_ReserveToUpdate = false
-- UI Open 여부
g_PrivateMarket_bUIOpen = false


-- Lua ---------------------------------------------------------

-- 개인상점 데이터 초기화
function PrivateMarket_Reset()

	g_PrivateMarket_bOpener = false
	
	g_PrivateMarket_nSelectX = 0
	g_PrivateMarket_nSelectY = 0
	
	g_PrivateMarket_StrName = ""
	g_PrivateMarket_ChaNum = 0
	g_PrivateMarket_StrTitle = ""
	
	g_PrivateMarket_tbItemList = {}
	
end

-- 개인상점 개설 요청
function PrivateMarket_ReqCreate( bOnlyUI )

	if true == g_PrivateMarket_bOpener then
		g_Msg:ReqPrivateMarketOpen( bOnlyUI )
	end

end

-- 개인상점 Slot 모두 갱신;
function PrivateMarket_ResetSlot()

	for i = 1, PIVATE_MARKET_SLOT_MAX, 1 do
		ResetSlot( SLOT_PRIVATE_MARKET_ID..tostring( i ) )
	end
	
	-- 등록된 아이템 인벤토리 표시
	Inventory_DisplayPrivateMarketRegItem( false )
	
end


-- Callback ----------------------------------------------------

-- 개인상점 이름 설정 메시지 박스 열기
function PrivateMarket_TitleMsgBox_Open()

	local tbItem = g_MyChar:GetItemPosInInvenByTable( ITEMTYPE_PRIVATEMARKET )
	if ( true == tbItem[ 1 ] ) then
	
		OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameInternalText( "PRIVATE_MARKET_MAKE_OPEN_MESSAGE" ), 
			Ev_PrivateMarket_TitleMsgBox_OpenOK, Event_MsgBox_Close )
		SetInt( WIDGET_ID_MESSAGEBOX, "maxByte", 32, "msgBoxInput.input" )
		
	end
	
end

-- 개인상점 아이템등록 UI 열기 응답
function PrivateMarket_MsgTitleFb( strTitle )

	g_PrivateMarket_bOpener = true
	g_PrivateMarket_StrTitle = strTitle
	
	if ( true == g_PrivateMarket_bUIOpen ) then
		return
	end	
	
	OpenWidget( WIDGET_ID_PRIVATE_MARKET )
end

-- 개인상점 개설 응답;
function PrivateMarket_MsgOpenFb()

	g_PrivateMarket_bOpener = true
	
	SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", false, "btnOK" )
	SetEvent( WIDGET_ID_PRIVATE_MARKET, EVENT_BUTTON_CLICK, "Ev_PrivateMarket_Close", "", "btnCancel" )
	
end

-- 타 캐릭터 개인상점 열기
function PrivateMarket_ReqPMInfo( strName, strMarket, nChaNum )

	if ( true == g_PrivateMarket_bUIOpen ) then
		return
	end	
	
	g_PrivateMarket_bOpener = false
	g_PrivateMarket_StrName = strName
	g_PrivateMarket_ChaNum = nChaNum
	g_PrivateMarket_StrTitle = strMarket
	
	OpenWidget( WIDGET_ID_PRIVATE_MARKET )
	
	if IsOpenWidget( WIDGET_ID_INVENTORY ) == false then
		OpenWidget(WIDGET_ID_INVENTORY)
	end
end

-- 개인상점 목록 갱신;
function PrivateMarket_Update( tbItemList, bMe, nChaNum )

	if ( g_PrivateMarket_ChaNum ~= nChaNum ) then
	
		PrintDebug( "Failed PrivateMarket_Update : "..tostring( g_PrivateMarket_ChaNum ).." / "..tostring( nChaNum ) )
		return;
		
	end
	
	if ( g_PrivateMarket_bOpener == bMe ) then
		
		PrivateMarket_ResetSlot()
		
	end
	
	g_PrivateMarket_tbItemList = tbItemList
	
	if ( false == g_PrivateMarket_bUIOpen ) then
	
		
		g_PrivateMarket_ReserveToUpdate = true
		
		return
		
	end
	
	
	local nItem = table.getn( g_PrivateMarket_tbItemList )
	if nItem == 0 then
		return
	end
	
	for i = 1, PRIVATE_MARKET_SLOT_COUNT_TOTAL, 1 do
	
		local tbItem = g_PrivateMarket_tbItemList[ i ]
		if nil ~= tbItem then
		
			SetBool( WIDGET_ID_PRIVATE_MARKET, "enabled", not tbItem[ 3 ], "Slot" .. i )
		
			SetSlot_PrivateMarket( SLOT_PRIVATE_MARKET_ID .. i, 
				tbItem[ 1 ], tbItem[ 2 ], g_PrivateMarket_bOpener )
				
		else
		
			SetBool( WIDGET_ID_PRIVATE_MARKET, "enabled", true, "Slot" .. i )
			
		end
	
	end
	
	-- 등록된 아이템 인벤토리 표시
	Inventory_DisplayPrivateMarketRegItem( false )
	
end

-- 일반 구입;
function PrivateMarket_ReqBuy( nPosX, nPosY, strItem )

	g_PrivateMarket_nSelectX = nPosX
	g_PrivateMarket_nSelectY = nPosY
	
	local strMsg = StringFormat( GetGameInternalText("BUY_ITEM_CONFIRM"), strItem )
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_PrivateMarket_ReqBuy_Ok, Event_MsgBox_Close )

end

-- 수량 체크 구입;
function PrivateMarket_ReqBuyEx( nPosX, nPosY )

	g_PrivateMarket_nSelectX = nPosX
	g_PrivateMarket_nSelectY = nPosY
	
	local str = GetGameInternalText( "BUY_PILE_ITEM", 0 )
	
	OpenMsgBox( MSG_BOX_TYPE_INPUT_ONLYNUM, str, Ev_PrivateMarket_ReqBuyEx_Ok, Event_MsgBox_Close )
	SetString( WIDGET_ID_MESSAGEBOX, "Restrict", "number", "msgBoxInput.input" )
	
end

-- 개인상점 개설자인지 확인;
function PrivateMarket_IsOpener ( )

	if ( false == g_PrivateMarket_bUIOpen ) then
	
		return false
		
	end
	
	return g_PrivateMarket_bOpener

end

-- 개인상점 UI를 열었는지 확인;
function PrivateMarket_IsOpen ( )

	return g_PrivateMarket_bUIOpen
	
end


-- Event -------------------------------------------------------

-- 개인상점 로드 완료
function LoadPrivateMarket()

	-- UI 닫기 이벤트
	SetEvent( WIDGET_ID_PRIVATE_MARKET, EVENT_WIDGET_CLOSE, "ClosePrivateMarket", "", "" )
	
	-- 확인
	SetString( WIDGET_ID_PRIVATE_MARKET, "label", GetGameWordText( "PRIVATE_MARKET_MAKE_BUTTON", 0 ), "btnOK" )
	SetEvent( WIDGET_ID_PRIVATE_MARKET, EVENT_BUTTON_CLICK, "Ev_PrivateMarket_Create", "", "btnOK" )
	
	-- 취소
	SetString( WIDGET_ID_PRIVATE_MARKET, "label", GetGameWordText( "PRIVATE_MARKET_MAKE_BUTTON", 1 ), "btnCancel" )	
	SetEvent( WIDGET_ID_PRIVATE_MARKET, EVENT_BUTTON_CLICK, "Ev_PrivateMarket_Close", "", "btnCancel" )
	
	-- 슬롯 바인딩
	for i = 1, PIVATE_MARKET_SLOT_MAX, 1 do
	
		SetString( WIDGET_ID_PRIVATE_MARKET, "binding", 
			SLOT_PRIVATE_MARKET_ID .. tostring( i ), "Slot" .. tostring( i ) )
			
	end
	
end

-- 개인상점 오픈
function OpenPrivateMarket()
	
	g_PrivateMarket_bUIOpen = true
	
	for i = 1, PIVATE_MARKET_SLOT_MAX, 1 do
	
		SetBool( WIDGET_ID_PRIVATE_MARKET, "enabled", true, "Slot" .. tostring( i ) )
	
	end

	-- 개설자인가?
	if true == g_PrivateMarket_bOpener then
	
		-- 내 캐릭터 이름 설정
		g_PrivateMarket_ChaNum = g_MyChar:GetDBNum()
		g_PrivateMarket_StrName = g_MyChar:GetName()
		SetString( WIDGET_ID_PRIVATE_MARKET, "text", g_PrivateMarket_StrName, "labelName" )
		
		-- 확인/취소 버튼 visible on
		SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", true, "btnOK" )
		SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", true, "btnCancel" )
		
		-- 인벤토리 드래그 가능 여부 설정
		Inventory_SetDragableInvenSlot( false )
		
		SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", false, "closeBtn" )
		
	else
	
		-- 이름 설정
		SetString( WIDGET_ID_PRIVATE_MARKET, "text", g_PrivateMarket_StrName, "labelName" )
		
		-- 확인/취소 버튼 visible off
		SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", false, "btnOK" )
		SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", false, "btnCancel" )
		
		SetBool( WIDGET_ID_PRIVATE_MARKET, "visible", true, "closeBtn" )
	
	end
	
	-- 타이틀 이름 설정
	SetWString( WIDGET_ID_PRIVATE_MARKET, "text", g_PrivateMarket_StrTitle, "labelPMName" )
	
	-- 업데이트 예약 처리
	if ( true == g_PrivateMarket_ReserveToUpdate ) then
		
		PrivateMarket_Update( g_PrivateMarket_tbItemList, g_PrivateMarket_bOpener, g_PrivateMarket_ChaNum )
		g_PrivateMarket_ReserveToUpdate = false
		
	else
	
		PrivateMarket_ResetSlot()
		
	end
	
	-- 인게임 클릭 안되도록 설정
	SetFakeModal( true )

end

-- 개인상점 닫기
function ClosePrivateMarket()

	-- MessageBox 는 무조건 같이 닫는다;
	Event_MsgBox_Close()

	-- 개인 상점 닫기 요청
	g_Msg:ReqPrivateMarketClose()
	
	-- 개인상점 데이터 초기화
	PrivateMarket_Reset()
	
	-- 인벤토리 드래그 가능 여부 설정
	Inventory_SetDragableInvenSlot( true )
	
	-- 인게임 다시 클릭 되도록 함
	SetFakeModal( false )
	
	-- 인벤토리 등록 아이템 표시 제거
	Inventory_DisplayPrivateMarketRegItem( true )
	
	g_PrivateMarket_bUIOpen = false
	
end

-- 개인상점 닫기 ( 취소버튼 클릭 );
function Ev_PrivateMarket_Close()

	-- 개설자일 경우 상점 종료 메시지
	if true == g_PrivateMarket_bOpener then
		
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PRIVATE_MARKET_QUIT" ),
			Ev_PrivateMarket_CloseOK, Event_MsgBox_Close )
		
		return
		
	end
	
	ClosePrivateMarket()
	CloseWidget( WIDGET_ID_PRIVATE_MARKET )
	
end

-- 개인상점 이름 설정 메시지 박스 확인
function Ev_PrivateMarket_TitleMsgBox_OpenOK()

	local strInput = GetMsgBoxInput()
	
	-- 상점 이름이 있어야 개설 가능
	if strInput ~= "" then

		-- 개인상점 아이템등록 UI 열기
		g_Msg:ReqPrivateMarketRegItemOpen( strInput )
		
	else
	
		return
		
	end
	
	SetInt( WIDGET_ID_MESSAGEBOX, "maxByte", 0, "msgBoxInput.input" )
	Event_MsgBox_Close()
	
end

-- 개인상점 개설 확인;
function Ev_PrivateMarket_Create()

	local str = GetGameInternalText( "PRIVATE_MARKET_ONLY_UI", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str,	Ev_PrivateMarket_CreateOnlyUI, Ev_PrivateMarket_CreateOK )
	
end

-- 개인상점 개설 (최저사양 모드);
function Ev_PrivateMarket_CreateOnlyUI()

	Event_MsgBox_Close()
	PrivateMarket_ReqCreate( true )
	

end

-- 개인상점 개설;
function Ev_PrivateMarket_CreateOK()

	Event_MsgBox_Close()
	PrivateMarket_ReqCreate( false )
	

end

-- 개설된 개인상점 닫기 OK Event;
function Ev_PrivateMarket_CloseOK()

	Event_MsgBox_Close()
	
	CloseWidget( WIDGET_ID_PRIVATE_MARKET )
	
	
end

-- 일반 구입 OK Event;
function Ev_PrivateMarket_ReqBuy_Ok()

	g_Msg:ReqPMarketBuyItem( g_PrivateMarket_nSelectX, g_PrivateMarket_nSelectY, 1 )
	Event_MsgBox_Close()
	
	
end

-- 수량 체크 구입 OK Event;
function Ev_PrivateMarket_ReqBuyEx_Ok()

	-- 수량이 없으면 구입하지 않는다;
	local nNum = tonumber( GetMsgBoxInput() )
	if ( nil == nNum ) or ( 0 == nNum ) then
	
		return
		
	end
	
	g_Msg:ReqPMarketBuyItem( g_PrivateMarket_nSelectX, g_PrivateMarket_nSelectY, nNum )
	Event_MsgBox_Close()
	
	
end

----------------------------------------------------------------