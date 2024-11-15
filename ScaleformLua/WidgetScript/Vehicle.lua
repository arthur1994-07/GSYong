
-- 현재 보여지고 있는 뷰 인스턴스 네임
g_strVehicle_ViewInst = ""
-- 현재 탈것 타입
g_nVehicle_Type = -1

-- 탈것창 로드 완료
function LoadVehicle()

	-- 이름 / 모델 / 연료
	SetString( WIDGET_ID_VEHICLE, "text", GetGameWordText( "VEHICLE_TEXT_STATIC", 0 ), "info.labelNameStatic" )
	SetString( WIDGET_ID_VEHICLE, "text", GetGameWordText( "VEHICLE_TEXT_STATIC", 1 ), "info.labelModelStatic" )
	SetString( WIDGET_ID_VEHICLE, "text", GetGameWordText( "VEHICLE_TEXT_STATIC", 2 ), "info.labelFuelStatic" )
	
	-- 부스터 효과 설명
	str = GetGameInternalText( "BIKE_BOOST_EXPLAIN", 0 )
	SetString( WIDGET_ID_VEHICLE, "text", str, "bike.infoBoost.labelBoostInfo" )
	SetString( WIDGET_ID_VEHICLE, "text", str, "car.infoBoost.labelBoostInfo" )
	SetString( WIDGET_ID_VEHICLE, "text", str, "rearcar.infoBoost.labelBoostInfo" )
	
	-- 기능
	SetString( WIDGET_ID_VEHICLE, "text", GetGameInternalText( "BIKE_BOOST_FUNCTION", 0 ), "bike.labelAbility" )
	SetString( WIDGET_ID_VEHICLE, "text", GetGameInternalText( "CAR_BOOST_FUNCTION", 0 ), "car.labelAbility" )
	SetString( WIDGET_ID_VEHICLE, "text", GetGameInternalText( "RICKSHAW_BOOST_FUNCTION", 0 ), "rearcar.labelAbility" )
	
	-- 슬롯 하이라이트 끄기
	SetBool( WIDGET_ID_VEHICLE, "visible", false, "board.highlight" )
	SetBool( WIDGET_ID_VEHICLE, "visible", false, "bike.highlight" )
	SetBool( WIDGET_ID_VEHICLE, "visible", false, "car.highlight" )
	SetBool( WIDGET_ID_VEHICLE, "visible", false, "rearcar.highlight" )
	
	-- 하이라이트 마우스 히트 테스트 끄기
	HitTestEnable( WIDGET_ID_VEHICLE, false, "board.highlight" )
	HitTestEnable( WIDGET_ID_VEHICLE, false, "bike.highlight" )
	HitTestEnable( WIDGET_ID_VEHICLE, false, "car.highlight" )
	HitTestEnable( WIDGET_ID_VEHICLE, false, "rearcar.highlight" )
	
	-- 슬롯 바인딩 ------------------------
	-- 종류별로 슬롯 개수가 다름.
	-- 보드, 리어카
	for i = 1, 4, 1 do
	
		local strIndex = tostring( i )
		SetString( WIDGET_ID_VEHICLE, "binding", SLOT_VEHICLE_ID .. strIndex, "board.slot" .. strIndex )
		SetString( WIDGET_ID_VEHICLE, "binding", SLOT_VEHICLE_ID .. strIndex, "rearcar.slot" .. strIndex )
	
	end
	
	-- 바이크
	for i = 1, 6, 1 do
	
		local strIndex = tostring( i )
		SetString( WIDGET_ID_VEHICLE, "binding", SLOT_VEHICLE_ID .. strIndex, "bike.slot" .. strIndex )
	
	end
	
	-- 자동차
	for i = 1, 5, 1 do
	
		local strIndex = tostring( i )
		SetString( WIDGET_ID_VEHICLE, "binding", SLOT_VEHICLE_ID .. strIndex, "car.slot" .. strIndex )
	
	end
	--------------------------------------

end

-- 탈것창 오픈
function OpenVehicle()

	-- 탈것 정보
	local tbVehicle = g_MyChar:GetPutOnVehicleInfo()
	
	-- 이름 / 모델
	SetString( WIDGET_ID_VEHICLE, "text", tbVehicle[ 2 ], "info.labelName" )
	SetString( WIDGET_ID_VEHICLE, "text", tbVehicle[ 3 ], "info.labelModel" )
	
	-- 남은 연료양 설정
	Vehicle_SetFuel( g_MyChar:VehicleFull() )
	
	local strInst = ""
	
	-- 탈것 타입에 따라 뷰 설정
	-- 없음
	if 	0 == table.getn( tbVehicle ) or 
		-1 == tbVehicle[ 1 ] then
	
		CloseWidget( WIDGET_ID_VEHICLE )
		
		g_nVehicle_Type = -1
		g_strVehicle_ViewInst = ""
		
		return
	
	-- 보드		
	elseif 0 == tbVehicle[ 1 ] then
	
		SetBool( WIDGET_ID_VEHICLE, "visible", true, "board" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "bike" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "car" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "rearcar" )
		
		strInst = "board"
		g_nVehicle_Type = 0
	
	-- 바이크
	elseif 1 == tbVehicle[ 1 ] then
	
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "board" )
		SetBool( WIDGET_ID_VEHICLE, "visible", true, "bike" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "car" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "rearcar" )
		
		strInst = "bike"
		g_nVehicle_Type = 1
	
	
	-- 자동차
	elseif 2 == tbVehicle[ 1 ] then
	
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "board" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "bike" )
		SetBool( WIDGET_ID_VEHICLE, "visible", true, "car" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "rearcar" )
		
		strInst = "car"
		g_nVehicle_Type = 2
	
	-- 리어카
	elseif 3 == tbVehicle[ 1 ] then
	
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "board" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "bike" )
		SetBool( WIDGET_ID_VEHICLE, "visible", false, "car" )
		SetBool( WIDGET_ID_VEHICLE, "visible", true, "rearcar" )
		
		strInst = "rearcar"
		g_nVehicle_Type = 3
		
	end
	
	-- 이동 속도 증가율
	local strMoveSpeed = ""
	if 0 ~= tbVehicle[ 6 ] then
	
		strMoveSpeed = string.format( "%s %.2f(+%.2f)", 
			GetGameWordText( "ITEM_ADDON_INFO", tbVehicle[ 4 ] ), tbVehicle[ 5 ], tbVehicle[ 6 ] )
	
	else
	
		strMoveSpeed = string.format( "%s %.2f", 
			GetGameWordText( "ITEM_ADDON_INFO", tbVehicle[ 4 ] ), tbVehicle[ 5 ] )
	
	end
	
	SetString( WIDGET_ID_VEHICLE, "htmlText", 
		HTMLFontColor( strMoveSpeed, HTML_TEXT_COLOR_GOLD ), strInst .. ".labelSpeed" )
		
	-- 보드는 부스터가 없다 (진짜 없나?)
	if 0 ~= tbVehicle[ 1 ] then
	
		-- 부스터 이미지 툴팁 / 부스터 배움 상태 텍스트
		local strBoostLearn = GetGameWordText( "BIKE_BOOST", 0 )
		local strTooltip = ""
		
		-- 부스터 배웠나?
		if 0 == tbVehicle[ 7 ] then
		
			strBoostLearn = HTMLFontColor( strBoostLearn .. GetGameWordText( "BIKE_BOOST_LEARNING", 0 ), HTML_TEXT_COLOR_GOLD )
			strTooltip = GetGameInternalText( "BIKE_BOOST_EXPLAIN_OVER_OFF", 0 )

		else
		
			strBoostLearn = HTMLFontColor( strBoostLearn .. GetGameWordText( "BIKE_BOOST_LEARNING", 1 ), HTML_TEXT_COLOR_GOLD )
			strTooltip = GetGameInternalText( "BIKE_BOOST_EXPLAIN_OVER_ON", 0 )

		end
		
		SetString( WIDGET_ID_VEHICLE, "htmlText", strBoostLearn, strInst .. ".infoBoost.labelBoost" )
		SetString( WIDGET_ID_VEHICLE, "TooltipText", strTooltip, strInst .. ".infoBoost.imgBoost" )

	end
	
	-- 부품 슬롯 아이템 설정
	local tbVehicleItem = tbVehicle[ 8 ]
	for value in pairs( tbVehicleItem ) do
	
		local tbItem = tbVehicleItem[ value ]
		SetSlot_ItemBornT( SLOT_VEHICLE_ID .. tostring( value ), tbItem[ 1 ], tbItem[ 2 ], 1, tbItem[ 3 ] )
	
	end
	
	-- 슬롯 하이라이트 끄기
	SetBool( WIDGET_ID_VEHICLE, "visible", false, strInst .. ".highlight" )
	
	g_strVehicle_ViewInst = strInst
	
end

-- 탈것창 단축키 입력
function HotkeyVehicle()

	if false == IsOpenWidget( WIDGET_ID_VEHICLE ) then
	
		if false == g_MyChar:VehicleActiveValue() then
			return
		end
	
	end

	ToggleUI( WIDGET_ID_VEHICLE )

end

------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 연료 남은 양 설정
function Vehicle_SetFuel( nFuel )

	if false == IsOpenWidget( WIDGET_ID_VEHICLE ) then
		return
	end

	local fFuel = nFuel / 10
	local strFuel = string.format( "%.2f%%", fFuel )
	
	SetString( WIDGET_ID_VEHICLE, "text", strFuel, "info.labelFuel" )
	SetInt( WIDGET_ID_VEHICLE, "value", fFuel, "info.barFuel" )

end

-- 탈것 슬롯 바인딩 ID -> 파츠 타입
function Vehicle_Binding2Parts( strBinding )

	if strBinding == SLOT_VEHICLE_ID .. "1" then
		return 0
	elseif strBinding == SLOT_VEHICLE_ID .. "2" then
		return 1
	elseif strBinding == SLOT_VEHICLE_ID .. "3" then
		return 2
	elseif strBinding == SLOT_VEHICLE_ID .. "4" then
		return 3
	elseif strBinding == SLOT_VEHICLE_ID .. "5" then
		return 4
	elseif strBinding == SLOT_VEHICLE_ID .. "6" then
		return 5
	end
	
	return -1

end

-- UI 업데이트
function Vehicle_Update()

	if false == IsOpenWidget( WIDGET_ID_VEHICLE ) then
		return
	end
	
	-- UI 업데이트
	OpenVehicle()

end

-- 탈것 장착
function Vehicle_Equip()

	if true == IsOpenWidget( WIDGET_ID_VEHICLE ) then
	
		CloseWidget( WIDGET_ID_VEHICLE )
	
	end

end

-- 탈것 부품 슬롯 하이라이트
function Vehicle_SlotHighlight( nVehicleType, nPartsType )

	if false == IsOpenWidget( WIDGET_ID_VEHICLE ) then
		return
	end
	
	if g_nVehicle_Type ~= nVehicleType then
		return
	end

	local strInst = g_strVehicle_ViewInst .. ".slot" .. tostring( nPartsType + 1 )
	local strHightlight = g_strVehicle_ViewInst .. ".highlight"
	
	local nX = GetInt( WIDGET_ID_VEHICLE, "x", strInst )
	local nY = GetInt( WIDGET_ID_VEHICLE, "y", strInst )
	
	SetBool( WIDGET_ID_VEHICLE, "visible", true, strHightlight )
	SetInt( WIDGET_ID_VEHICLE, "x", nX, strHightlight )
	SetInt( WIDGET_ID_VEHICLE, "y", nY, strHightlight )
	
	InsertStageMouseClickLater( Ev_Vehicle_StageClick )

end

-----------------------------------------------------------------------------------------------
-- 이벤트

-- 스테이지 클릭 - 슬롯 하이라이트 끄기
function Ev_Vehicle_StageClick( nStageX, nStageY )

	RemoveStageMouseClick( Ev_Vehicle_StageClick )

	SetBool( WIDGET_ID_VEHICLE, "visible", false, g_strVehicle_ViewInst .. ".highlight" )

end