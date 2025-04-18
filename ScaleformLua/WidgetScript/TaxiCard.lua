 
-- 택시 맵 정보
g_tbTaxiCard_MapInfo = {}

function LoadTaxiCard()
	
	-- 지역 구분
	SetString( WIDGET_ID_TAXI_CARD, "text", GetGameWordText( "TAXI_TITLE", 0 ), "labelMainArea" )
	-- 택시 정거장
	SetString( WIDGET_ID_TAXI_CARD, "text", GetGameWordText( "TAXI_TITLE", 1 ), "labelSubArea" )
	-- 요금
	SetString( WIDGET_ID_TAXI_CARD, "text", "0", "labelMoney" )
	-- OK 버튼
	SetString( WIDGET_ID_TAXI_CARD, "label", "O K", "btnOK" )
	
	-- 맵 리스트 선택
	SetEvent( WIDGET_ID_TAXI_CARD, EVENT_LIST_INDEX_CHANGE, "Event_TaxiCard_MapList", "index", "MapList" )
	-- 정류장 리스트 선택
	SetEvent( WIDGET_ID_TAXI_CARD, EVENT_LIST_INDEX_CHANGE, "Event_TaxiCard_StationList", "", "StationList" )
	-- 확인 버튼
	SetEvent( WIDGET_ID_TAXI_CARD, EVENT_BUTTON_CLICK, "Event_TaxiCard_btnOK", "", "btnOK" )
	
	-- 맵 리스트 설정
	TaxiCard_SetMapList()

end

-- 맵 리스트 설정
function TaxiCard_SetMapList()
	
	g_tbTaxiCard_MapInfo = g_TaxiCard:GetMapInfo()

	local tbList = {}
	for value in pairs( g_tbTaxiCard_MapInfo ) do
	
		local tbMapInfo = g_tbTaxiCard_MapInfo[ value ]
		
		-- 맵 이름
		local tbData =
		{
			tbMapInfo[ 1 ]
		}
		table.insert( tbList, tbData )
	
	end

	SetDataEx( WIDGET_ID_TAXI_CARD, "label", tbList, "MapList", false )
	SetInt( WIDGET_ID_TAXI_CARD, "selectedIndex", 0, "MapList" )
	
end

-- 정류장 리스트 설정
function TaxiCard_SetStationList( nIndex )
	
	local tbMap = g_tbTaxiCard_MapInfo[ nIndex + 1 ]
	if nil == tbMap then
		return
	end
	
	local tbStation = tbMap[ 2 ]
	if nil == tbStation then
		return
	end
	
	local tbList = {}
	for value in pairs( tbStation ) do
	
		-- 정류장 이름
		local tbData = 
		{
			tbStation[ value ]
		}
		table.insert( tbList, tbData )
	
	end
	
	SetDataEx( WIDGET_ID_TAXI_CARD, "label", tbList, "StationList", false )
	SetInt( WIDGET_ID_TAXI_CARD, "selectedIndex", 0, "StationList" )
	CallFunction( WIDGET_ID_TAXI_CARD, "validateNow", "", "StationList" )
	
	TaxiCard_SetCharge()
	
end

-- 요금 설정
function TaxiCard_SetCharge()

	local nMapIndex = GetInt( WIDGET_ID_TAXI_CARD, "selectedIndex", "MapList" )
	local nStationIndex = GetInt( WIDGET_ID_TAXI_CARD, "selectedIndex", "StationList" )
	
	local nCharge = g_TaxiCard:GetCalcTaxiCharge( nMapIndex, nStationIndex )
	SetString( WIDGET_ID_TAXI_CARD, "text", CurrencyFormat( nCharge ), "labelMoney" )

end

-- 맵 리스트 선택
function Event_TaxiCard_MapList( nIndex )
	
	TaxiCard_SetStationList( nIndex )
	
end

-- 정류장 리스트 선택
function Event_TaxiCard_StationList()
	
	TaxiCard_SetCharge()
	
end

-- 확인 버튼
function Event_TaxiCard_btnOK()

	local nMapIndex = GetInt( WIDGET_ID_TAXI_CARD, "selectedIndex", "MapList" )
	local nStationIndex = GetInt( WIDGET_ID_TAXI_CARD, "selectedIndex", "StationList" )

	g_Msg:ReqTaxiStation( nMapIndex, nStationIndex )
	CloseWidget( WIDGET_ID_TAXI_CARD )
	
end