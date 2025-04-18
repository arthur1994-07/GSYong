g_tbBusStation_MapInfo = {}
g_tbBusStation_StationInfo = {}

g_tbBusStation_nSelectMapIndex = 0			-- 선택된 맵 인덱스 정보
g_tbBusStation_nSelectStationIndex = 0		-- 선택된 정류장 인덱스 정보
	
function LoadBusStation()

	g_tbBusStation_MapInfo = {}
	
	-- 선택된 맵 인덱스 정보
	g_tbBusStation_nSelectMapIndex = 0
	-- 선택된 정류장 인덱스 정보
	g_tbBusStation_nSelectStationIndex = 0

	-- 지역 구분
	SetString( WIDGET_ID_BUS_STATION, "text", GetGameWordText("BUS_TITLE", 0 ), "labelMainArea" )
	-- 버스 정류장
	SetString( WIDGET_ID_BUS_STATION, "text", GetGameWordText("BUS_TITLE", 1 ), "labelSubArea" )
	-- OK 버튼
	SetString( WIDGET_ID_BUS_STATION, "label", "O K", "btnOK" )
	
	-- 맵 리스트
	SetEvent( WIDGET_ID_BUS_STATION, EVENT_LIST_ITEM_CLICK, "Event_BusStation_MapList", "index;", "MapList" )
	-- 정류장 리스트
	SetEvent( WIDGET_ID_BUS_STATION, EVENT_LIST_ITEM_CLICK, "Event_BusStation_StationList", "index;", "StationList" )
	-- 확인 버튼
	SetEvent( WIDGET_ID_BUS_STATION, EVENT_BUTTON_CLICK, "Event_BusStation_btnOK", "", "btnOK" )
	
	-- 초기 위치 설정
	BusStation_SetDefaultPostion()	
	
	--업데이트
	BusStation_DisplyMapInfo()
	
	BusStation_SetTimer()
	
end

function BusStation_SetDefaultPostion()

	local nWidth = GetInt( WIDGET_ID_BUS_STATION, "width", "" )

	SetInt( WIDGET_ID_BUS_STATION, "x", 100, "" )
	SetInt( WIDGET_ID_BUS_STATION, "y", 100, "" )
end

function BusStation_DisplyMapInfo()

	-- 맵 정보 받아오기
	g_tbBusStation_MapInfo = {}
	g_tbBusStation_MapInfo = g_BusStation:GetMapInfo()
	
	local tbMapName = {}
	local nMapInfoSize = g_BusStation:GetMapInfoSize()
	for nIndex = 1, nMapInfoSize, 1 do
		
		local tbMapInfo = g_tbBusStation_MapInfo[nIndex]
		if nil ~= tbMapInfo then
			
			local tbTemp = {}
			tbTemp[1] = tbMapInfo["strMAP"]
			table.insert( tbMapName, tbTemp )
		 
		end -- if nil ~= tbMapInfo then
		
	end -- for nIndex = 1, nMapInfoSize, 1 do

	SetDataEx(WIDGET_ID_BUS_STATION, "label;", tbMapName, "MapList", false)
	
end

function Event_BusStation_MapList( nSelectedIndex )
	
	local nIndex = nSelectedIndex + 1
	if nil ~= g_tbBusStation_MapInfo[nIndex] then
		
		local tbStationName = {}
	
		local nStationInfo = g_tbBusStation_MapInfo[nIndex]["nStationInfo"]
		for nSubIndex = 1, nStationInfo, 1 do
		
			local tbStationInfo = g_tbBusStation_MapInfo[nIndex][nSubIndex]
			if nil ~= tbStationInfo then
				local tbTemp = {}
				tbTemp[1] = tbStationInfo["strSTATION"]
				table.insert( tbStationName, tbTemp )
			end
		end
		
		SetDataEx( WIDGET_ID_BUS_STATION, "label;", tbStationName, "StationList", false )

	end
	
end

function Event_BusStation_StationList( nSelectedIndex )
	
	local nIndex = GetInt( WIDGET_ID_BUS_STATION, "selectedIndex", "MapList" ) + 1
	
	if nil ~= g_tbBusStation_MapInfo[ nIndex ] then
		
		local tbStationInfo = g_tbBusStation_MapInfo[ nIndex ][ nSelectedIndex + 1 ]
		local dwMapID = g_tbBusStation_MapInfo[ nIndex ][ "dwMAPID" ]
		if nil ~= tbStationInfo then
			
			local tbMapGateInfo = g_Minimap:GetMapGateName( dwMapID )
			for value in pairs( tbMapGateInfo ) do
				local tbGateInfo = tbMapGateInfo[ value ]
		
				if tbStationInfo[ "dwGATEID" ] == tbGateInfo[ "GateID" ] then
				
					local tbDisplayMapInfo = {}
					tbDisplayMapInfo[ "nMapID" ] = dwMapID
					tbDisplayMapInfo[ "nPosX" ] = tbGateInfo[ "x" ]
					tbDisplayMapInfo[ "nPosY" ] = tbGateInfo[ "y" ]
					
					LargeMap_SearchCharMapPos( tbDisplayMapInfo )
					return
				end
				
			end
			
		end

	end
	
end

function Event_BusStation_btnOK()

	local nMapIndex = GetInt( WIDGET_ID_BUS_STATION, "selectedIndex", "MapList" ) + 1
	if nil ~= g_tbBusStation_MapInfo[ nMapIndex ] then
		
		local nStationIndex = GetInt( WIDGET_ID_BUS_STATION, "selectedIndex", "StationList" ) + 1
		local tbStationInfo = g_tbBusStation_MapInfo[ nMapIndex ][ nStationIndex ]
		
		if nil ~= tbStationInfo then
		
			CloseWidget( WIDGET_ID_BUS_STATION ) 
			
			local nLinkID = tbStationInfo["dwLINKID"]
			g_Msg:ReqBusStation( nLinkID )
			
		end
	
	end
	
end

function BusStation_SetTimer()

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "FrameMove_BusStation"
	}
	SetTween( WIDGET_ID_BUS_STATION, tbQuickSet, "", "", "" )
	
end

function FrameMove_BusStation()

	if g_BusStation:IsUseable() == false then
	
		CloseWidget( WIDGET_ID_BUS_STATION )
		
	else
	
		BusStation_SetTimer()
		
	end	
	
end