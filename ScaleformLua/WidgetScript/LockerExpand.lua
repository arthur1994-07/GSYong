
-- 락커 확장 가능 개수
LOCKER_EXPAND_ENABLE_COUNT = 3

-- 락커 확장 카드 인벤 위치
g_nLockerExpand_InvenPosX = -1
g_nLockerExpand_InvenPosY = -1

-- 선택한 락커 인덱스
g_nLockerExpand_SelectIndex = -1

-- 락커 확장 로드 완료
function LoadLockerExpand()

	-- 클릭 이벤트
	for i = 1, LOCKER_EXPAND_ENABLE_COUNT, 1 do
	
		local strIndex = tostring( i )
		local strInst = "locker" .. strIndex
		
		SetEvent( WIDGET_ID_LOCKER_EXPAND, EVENT_BUTTON_CLICK, 
			"Ev_LockerExpand_clickBtn", "", strInst .. ".btnExpand", strIndex )
		
		SetString( WIDGET_ID_LOCKER_EXPAND, "text", 
			GetGameWordText( "STORAGE_CARD_NUMBER", i - 1 ), strInst .. ".labelNum" )
	
	end

end

-- 락커 확장 오픈
function OpenLockerExpand()

	-- 락커 사용 가능 여부, 남은 시간 설정
	for i = 1, LOCKER_EXPAND_ENABLE_COUNT, 1 do
	
		local tbTime = g_MyChar:GetStorageTime( i )
		local strInst = "locker" ..  tostring( i )
		
		if true == tbTime[ "bIsUsable" ] then
		
			local str = HTMLFontColor( GetGameWordText( "STORAGE_USABLE", 0 ), HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_LOCKER_EXPAND, "htmlLabel", str, strInst .. ".btnExpand" )
			
			str = string.format( GetGameWordText( "STORAGE_TIMELEFT", 0 ),
				tbTime[ "nYear" ], tbTime[ "nMonth" ], tbTime[ "nDay" ], tbTime[ "nHour" ], tbTime[ "nMinute" ] )
			SetString( WIDGET_ID_LOCKER_EXPAND, "text", str, strInst .. ".labelDate" )
		
		else
		
			local str = HTMLFontColor( GetGameWordText( "STORAGE_USABLE", 1 ), HTML_TEXT_COLOR_RED )
			SetString( WIDGET_ID_LOCKER_EXPAND, "htmlLabel", str, strInst .. ".btnExpand" )
			SetString( WIDGET_ID_LOCKER_EXPAND, "text", "", strInst .. ".labelDate" )
		
		end
	
	end

end

------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 락커 확장 카드 사용
function LockerExpand_Open( wPosX, wPosY )

	g_nLockerExpand_InvenPosX = wPosX
	g_nLockerExpand_InvenPosY = wPosY
	
	OpenWidget( WIDGET_ID_LOCKER_EXPAND )

end

------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확장할 락커 선택
function Ev_LockerExpand_clickBtn( strIndex )

	g_nLockerExpand_SelectIndex = tonumber( strIndex )
	
	local str = StringFormat( GetGameInternalText( "STORAGE_CHARGE_CARD_CONFIRM", 0 ), g_nLockerExpand_SelectIndex + 1 )
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_LockerExpand_OK, Event_MsgBox_Close )

end

-- 락커 확장 확인
function Ev_LockerExpand_OK()

	-- 서버로 락커 확장 요청
	g_Msg:ReqStorageCard( g_nLockerExpand_InvenPosX, g_nLockerExpand_InvenPosY, g_nLockerExpand_SelectIndex )
	
	Event_MsgBox_Close()
	CloseWidget( WIDGET_ID_LOCKER_EXPAND )
	
	g_nLockerExpand_InvenPosX = -1
	g_nLockerExpand_InvenPosY = -1
	g_nLockerExpand_SelectIndex = -1

end