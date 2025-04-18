
g_PrivateMarketRegItemMsgBox_IsPile = false
g_PrivateMarketRegItemMsgBox_nPosX = 0
g_PrivateMarketRegItemMsgBox_nPosY = 0

function LoadPrivateMarketRegItemMsgBox()

	local str = GetGameInternalText( "PRIVATE_MARKET_SELL_WINDOW_TEXTBOX" )
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", str, "TextInfo" )
	
	str = GetGameWordText( "PRIVATE_MARKET_SELL_WINDOW_STATICTEXT", 0 )
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", str, "TextPrice" )
	SetBool( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "currencyUnit", true, "InputPrice" )
	
	str = GetGameWordText( "PRIVATE_MARKET_SELL_WINDOW_STATICTEXT", 1 )
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", str, "TextCount" )
	
	str = GetGameWordText( "PRIVATE_MARKET_MAKE_BUTTON", 0 )
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "label", str, "BtnOk" )
	
	str = GetGameWordText( "PRIVATE_MARKET_MAKE_BUTTON", 1 )
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "label", str, "BtnClose" )
	
	SetEvent( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, EVENT_BUTTON_CLICK, "PrivateMarketRegItemMsgBox_ReqRegItem", "", "BtnOk" )
	SetEvent( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, EVENT_BUTTON_CLICK, "PrivateMarketRegItemMsgBox_Close", "", "BtnClose" )
	
end
-- 개인상점 메시지 박스 오픈시
function PrivateMarketRegItemMsgBox_Open()
	
	-- ID 인풋 키 입력 이벤트 설정
	SetEvent( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, EVENT_KEY_DOWN, "Event_PrivateMsgBox_InputPrice_KeyDown", "keyCode;", "InputPrice" )	-- 가격
	SetEvent( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, EVENT_KEY_DOWN, "Event_PrivateMsgBox_InputCount_KeyDown", "keyCode;", "InputCount" )	-- 수량
	
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", "", "InputPrice" )
	SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", "", "InputCount" )
	
	-- 개수가 있는 아이템인지 여부
	if true == g_PrivateMarketRegItemMsgBox_IsPile then
	
		SetBool( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "visible", true, "TextCount" )
		SetBool( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "visible", true, "InputCount" )	
		-- 수량 기본값 1로 설정(이 문구가 없어도 내부적으로 값이 없을경우 1개로 인식한다.)
		SetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", "1", "InputCount" )
		
	else
	
		SetBool( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "visible", false, "TextCount" )
		SetBool( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "visible", false, "InputCount" )
		
	end
	
	SetFocus( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "InputPrice" )	-- 판매 금액 입력란 활성화
end

-- 아이템 등록
function PrivateMarketRegItemMsgBox_ReqRegItem()
	
	local nPrice = tonumber( GetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", "InputPrice" ) )
	local nCount = tonumber( GetString( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "text", "InputCount" ) )
	
	-- 판매 금액 체크
	if nil == nPrice or 0 == nPrice then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "POSTBOX_ERROR_MSG_SEND_BILLING_ZERO", 0 ), TEXT_COLOR_RED )
		SetFocus( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "InputPrice" )
		
		return
		
	end
	
	-- 수량 체크
	if false == GetBool( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "visible", "InputCount" ) then
	
		nCount = 1
		
	else
	
		if nil == nCount or 0 == nCount then
			nCount = 1
		end
		
	end
	
	-- nCount( 수량 ) 수량이 int값을 넘지 않도록 수정한다. 넘어버리면 0으로 전달 되는데 0으로 전달 될 경우는 1로 처리됨
	-- 그러므로 최대 값을 너어 전체 아이템이 설정되도록 수정
	if nCount > 100000000 then
		nCount = 100000000
	end
	
	g_Msg:ReqPrivateMarketRegItem( 
		g_PrivateMarketRegItemMsgBox_nPosX, g_PrivateMarketRegItemMsgBox_nPosY, nPrice, nCount )
		
	CloseWidget( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX )
	
end

-- 개인상점 금액 입력창 닫기
function PrivateMarketRegItemMsgBox_Close()

	CloseWidget( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX )
	
end

-- 개인상점 금액 입력란 위치값 설정
function PrivateMarketRegItemMsgBox_Init( bPile, nPosX, nPosY )

	OpenWidget( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX )
	
	g_PrivateMarketRegItemMsgBox_IsPile = bPile
	g_PrivateMarketRegItemMsgBox_nPosX = nPosX
	g_PrivateMarketRegItemMsgBox_nPosY = nPosY
	
end

-- 가격 입력란 인풋 키 입력 이벤트
function Event_PrivateMsgBox_InputPrice_KeyDown( nKeyCode )
	-- 탭 키
	if KEYCODE_TAB == nKeyCode then
		-- 수량입력 인풋으로 포커스 변경(수량입력 란이 있을 경우)
		if true == g_PrivateMarketRegItemMsgBox_IsPile then
			SetFocus( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "InputCount" )			
		end
		
	-- 엔터키 입력
	elseif KEYCODE_ENTER == nKeyCode then	
		-- 확인버튼 클릭
		PrivateMarketRegItemMsgBox_ReqRegItem()
		
	end

end
-- 수량 입력란 인풋 키 입력 이벤트
function Event_PrivateMsgBox_InputCount_KeyDown( nKeyCode )
	-- 탭 키
	if KEYCODE_TAB == nKeyCode then
		-- 수량입력 인풋으로 포커스 변경(수량입력 란이 있을 경우)
		SetFocus( WIDGET_ID_PRIVATE_MARKET_REG_ITEM_MSGBOX, "InputPrice" )	-- 판매 금액 입력란 활성화
		
	-- 엔터키 입력
	elseif KEYCODE_ENTER == nKeyCode then	
		-- 확인버튼 클릭
		PrivateMarketRegItemMsgBox_ReqRegItem()
		
	end

end