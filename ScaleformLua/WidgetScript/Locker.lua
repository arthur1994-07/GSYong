
g_nLocker_SelectPage = 0

g_nLocker_NpcID = -1
g_bLocker_OpenFromNPC = false

g_bLocker_IsOpenMsgBox = false
g_nLocker_InvenPosX = 0
g_nLocker_InvenPosY = 0

-- 락커 로드 완료
function LoadLocker()

	-- 슬롯 바인딩
	for i = 1, LOCKER_SLOT_TOTAL, 1 do
	
		local strSlot = "slot" .. tostring( i )
	
		SetString( WIDGET_ID_LOCKER, "binding", SLOT_LOCKER_ID .. tostring( i ), strSlot )
		SetBool( WIDGET_ID_LOCKER, "holdable", true, strSlot )
		
		SetBool( WIDGET_ID_LOCKER, "lock", false, strSlot )
		
	end
	
	-- 페이지 버튼 이벤트
	for i = 1, 5, 1 do
	
		SetEvent( WIDGET_ID_LOCKER, EVENT_BUTTON_CLICK, "Ev_Locker_pageClick", "", 
			"btnPage" .. tostring( i ), tostring( i ) )
			
		SetBool( WIDGET_ID_LOCKER, "keepSlot", true, "btnPage" .. tostring( i ) )
	
	end
	
	-- 페이지 선택
	SetBool( WIDGET_ID_LOCKER, "selected", true, "btnPage1" )
	
	-- 입금/출금 버튼
	SetString( WIDGET_ID_LOCKER, "label", GetGameWordText( "WB_CLUB_WINDOW_STORAGE_MONEY", 3 ), "btnIn" )
	SetString( WIDGET_ID_LOCKER, "label", GetGameWordText( "WB_CLUB_WINDOW_STORAGE_MONEY", 4 ), "btnOut" )
	SetEvent( WIDGET_ID_LOCKER, EVENT_BUTTON_CLICK, "Ev_Locker_InClick", "", "btnIn" )
	SetEvent( WIDGET_ID_LOCKER, EVENT_BUTTON_CLICK, "Ev_Locker_OutClick", "", "btnOut" )
	
	SetEvent( WIDGET_ID_LOCKER, EVENT_WIDGET_CLOSE, "CloseLocker", "", "" )

end

-- 락커 오픈
function OpenLocker()

	-- 선택 페이지 초기화
	g_nLocker_SelectPage = 0
	
	-- 1번 탭 선택
	SetBool( WIDGET_ID_LOCKER, "selected", true, "btnPage1" )
	for i = 2, 5, 1 do
		SetBool( WIDGET_ID_LOCKER, "selected", false, "btnPage" .. tostring( i ) )
	end

	-- 락커 정보 서버에 요청
	if true == g_Msg:ReqGetStorage( g_nLocker_SelectPage ) then
	
		-- 이미 정보를 받았다. 아이템 슬롯 업데이트
		Locker_UpdateItem( g_nLocker_SelectPage )
		
		-- 락커 돈 설정
		SetString( WIDGET_ID_LOCKER, "text", CurrencyFormat( g_MyChar:GetStorageMoney() ), "labelMoney" )
	
	end
	
	if true == g_bLocker_OpenFromNPC then
	
		InsertFrameMove( Locker_FrameMove )
	
	end

	Locker_ExtendTap_Use()
	Locker_Premium_Use()
	Locker_SetSlotLock( 1 )
	
end

-- 프레임 무브
function Locker_FrameMove( fElspTime )

	if false == g_NpcTalk:IsUseableFromID( g_nLocker_NpcID ) then
	
		CloseWidget( WIDGET_ID_LOCKER )
		RemoveFrameMove( Locker_FrameMove )
	
	end

end

-- 라커 확장 텝 사용가능 여부
function Locker_ExtendTap_Use()

	for i = 2, 4, 1 do
	
		local tbTime = g_MyChar:GetStorageTime( i - 1 )
		
		if true == tbTime[ "bIsUsable" ] then
		
			local strTooltip = StringFormat( GetGameWordText( "STORAGE_TIMELEFT_ONELINE", 0 ), 
				tbTime[ "nYear" ], tbTime[ "nMonth" ], tbTime[ "nDay" ], tbTime[ "nHour" ], tbTime[ "nMinute" ] )
			
			SetString( WIDGET_ID_LOCKER, "TooltipText", strTooltip, "btnPage" .. tostring( i ) )
		
		else
		
			local strTooltip = GetGameWordText( "STORAGE_USABLE", 1 )
			SetString( WIDGET_ID_LOCKER, "TooltipText", strTooltip, "btnPage" .. tostring( i ) )
		
		end
		
	end
	
end

-- 라커 프리미엄 텝 사용가능 여부
function Locker_Premium_Use()

	if false == IsOpenWidget( WIDGET_ID_LOCKER ) then
		return
	end

	-- 1 : 프리미엄 여부 bool, 2 : 연 number, 3 : 월 number, 4 : 일 number, 5 : 시 number, 6 : 분 number
	local PremiumDate = g_MyChar:GetPremium()
	if true == PremiumDate[ 1 ] then
	
		local strTooltip = StringFormat( GetGameWordText( "STORAGE_TIMELEFT_ONELINE", 0 ), 
		PremiumDate[ 2 ], PremiumDate[ 3 ], PremiumDate[ 4 ], PremiumDate[ 5 ], PremiumDate[ 6 ] )
		
		SetString( WIDGET_ID_LOCKER, "TooltipText", strTooltip, "btnPage5" )
		
	else
	
		local strTooltip = GetGameWordText( "STORAGE_USABLE", 1 )
		SetString( WIDGET_ID_LOCKER, "TooltipText", strTooltip, "btnPage5" )
		
	end
	
	if false == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		OpenWidget( WIDGET_ID_INVENTORY )
	end
	
end

-- 락커 닫기
function CloseLocker()

	g_Msg:ReqStorageCloseCard()

end

-- 슬롯 사용 가능 설정
function Locker_SetSlotLock( nIndex )

	local tbTime = g_MyChar:GetStorageTime( nIndex - 1 )
	
	for i = 1, LOCKER_SLOT_TOTAL, 1 do
	
		local strSlot = "slot" .. tostring( i )
		SetBool( WIDGET_ID_LOCKER, "lock", not tbTime[ "bIsUsable" ], strSlot )
		
	end

end

-----------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- NPC를 통해 락커 열기
function Locker_OpenFromNPC( dwNpcID )

	g_nLocker_NpcID = dwNpcID
	g_bLocker_OpenFromNPC = true
	
	OpenWidget( WIDGET_ID_LOCKER )

end

-- 락커 연결 카드 사용 가능
function Locker_CanUseOpenCard( nPosX, nPosY )

	if true == g_bLocker_IsOpenMsgBox then
		return
	end

	g_bLocker_IsOpenMsgBox = true
	
	g_nLocker_InvenPosX = nPosX
	g_nLocker_InvenPosY = nPosY

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "INVEN_LOCKER_OPEN_ITEM_REQ", 0 ), 
		Ev_Locker_UseOpenCard, Ev_Locker_UseOpenCardCancel )

end

-- 락커 돈 업데이트
function Locker_UpdateMoney( nMoney )

	if false == IsOpenWidget( WIDGET_ID_LOCKER ) then
		return
	end

	SetString( WIDGET_ID_LOCKER, "text", CurrencyFormat( nMoney ), "labelMoney" )

end

-- 락커 아이템 업데이트
function Locker_UpdateItem( nIndex )

	if false == IsOpenWidget( WIDGET_ID_LOCKER ) then
		return
	end
	
	if g_nLocker_SelectPage ~= nIndex then
		return
	end
	
	for i = 1, LOCKER_SLOT_TOTAL, 1 do
	
		local nPosX = ( i - 1 ) % LOCKER_SLOT_COL
		local nPosY = math.floor( ( i - 1 ) / LOCKER_SLOT_COL )
		SetSlot_Locker( SLOT_LOCKER_ID .. tostring( i ), nIndex, nPosX, nPosY )
	
	end

end

-- 락커 연결 카드 사용
function Locker_UseOpenCard()

	CloseWidget( WIDGET_ID_POSTBOX )
	CloseWidget( WIDGET_ID_POSTBOX_SUB )
	
	g_bLocker_OpenFromNPC = false
	
	OpenWidget( WIDGET_ID_LOCKER )
	OpenWidget( WIDGET_ID_INVENTORY )

end

-- 락커 확장 성공, 락커 업데이트
function Locker_Update()

	if false == IsOpenWidget( WIDGET_ID_LOCKER ) then
		return
	end
	
	Locker_ExtendTap_Use()
	Locker_UpdateItem( g_nLocker_SelectPage )
	Locker_SetSlotLock( g_nLocker_SelectPage + 1 )
	Locker_Premium_Use()

end

-- 인벤토리 위치 변경
function Locker_SwapItem( nPosX1, nPosY1, nPosX2, nPosY2 )

	Inventory_InvenSlotUpdate( nPosX1, nPosY1 )
	
	if false == g_bLocker_IsOpenMsgBox then
		return
	end
	
	if 	nPosX1 == g_nLocker_InvenPosX and
		nPosY1 == g_nLocker_InvenPosY then
		
		g_nLocker_InvenPosX = nPosX2
		g_nLocker_InvenPosY = nPosY2
		
	elseif	nPosX2 == g_nLocker_InvenPosX and
			nPosY2 == g_nLocker_InvenPosY then
		
		g_nLocker_InvenPosX = nPosX1
		g_nLocker_InvenPosY = nPosY1
		
	end

end

-----------------------------------------------------------------------------------------------------
-- 이벤트

-- 락커 연결 카드 사용 확인
function Ev_Locker_UseOpenCard()

	g_Msg:ReqStorageOpenCard( g_nLocker_InvenPosX, g_nLocker_InvenPosY )
	
	g_bLocker_IsOpenMsgBox = false
	Event_MsgBox_Close()

end

-- 락커 연결 카드 사용 취소
function Ev_Locker_UseOpenCardCancel()

	g_bLocker_IsOpenMsgBox = false
	Event_MsgBox_Close()

end

-- 입금 버튼 클릭
function Ev_Locker_InClick()

	SetBool( WIDGET_ID_MESSAGEBOX, "currencyUnit", true, "msgBoxInput.input" )

	-- 입력 메시지
	OpenMsgBox( MSG_BOX_TYPE_INPUT_MONEY, GetGameInternalText( "PLAYINTERFACE_31", 0 ), 
		Ev_Locker_InMoneyOK, Event_MsgBox_Close )
		
end

-- 출금 버튼 클릭
function Ev_Locker_OutClick()

	-- 입력 메시지
	OpenMsgBox( MSG_BOX_TYPE_INPUT_MONEY, GetGameInternalText( "PLAYINTERFACE_32", 0 ), 
		Ev_Locker_OutMoneyOK, Event_MsgBox_Close )

end

-- 입금 확인
function Ev_Locker_InMoneyOK()
	
	if GetMsgBoxInput() == "" then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PLAYINTERFACE_15", 0 ), TEXT_COLOR_RED )
		return
	end
	
	local nMoney = math.floor( tonumber( GetMsgBoxInput() ) )	
	local InvenMoney = tonumber( g_MyChar:GetInvenMoney() )
	if nMoney > InvenMoney then
		nMoney = InvenMoney
	end
	if 0 < nMoney then
	
		if false == g_Msg:ReqStorageSaveMoney( nMoney ) then
		
			CF_PrintMsg_SystemMsg( GetGameInternalText( "REFER_CHAR_REQUEST_DELAY", 0 ), TEXT_COLOR_RED )
		
		end
	
	end
	
	Event_MsgBox_Close()

end

-- 출금 확인
function Ev_Locker_OutMoneyOK()
	if GetMsgBoxInput() == "" then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "PLAYINTERFACE_16", 0 ), TEXT_COLOR_RED )
		return
	end
	
	local nMoney = math.floor( tonumber( GetMsgBoxInput() ) )
	local LockerMoney = tonumber( g_MyChar:GetStorageMoney() )
	if nMoney > LockerMoney then
		nMoney = LockerMoney
	end
	if 0 < nMoney then
	
		if false == g_Msg:ReqStorageDrawMoney( nMoney ) then
		
			CF_PrintMsg_SystemMsg( GetGameInternalText( "REFER_CHAR_REQUEST_DELAY", 0 ), TEXT_COLOR_RED )
		
		end
	
	end
	
	Event_MsgBox_Close()

end

-- 페이지 버튼 클릭
function Ev_Locker_pageClick( strIndex )

	local nIndex = tonumber( strIndex )
	for i = 1, 5, 1 do
	
		local bSelect = false
		if i == nIndex then
			bSelect = true
		end
		
		SetBool( WIDGET_ID_LOCKER, "selected", bSelect, "btnPage" .. tostring( i ) )
	
	end
	
	g_nLocker_SelectPage = nIndex - 1
	
	-- 락커 정보 서버에 요청
	g_Msg:ReqGetStorage( g_nLocker_SelectPage )
	
	-- 아이템 슬롯 업데이트
	Locker_UpdateItem( g_nLocker_SelectPage )
	Locker_SetSlotLock( nIndex )

end