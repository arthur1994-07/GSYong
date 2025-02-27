
-- 선도전 지역 개수 (서버에서 던져주는 데이터가 다 사용되는게 아니라함.)
g_nCCA_AreaCount = 3

-- 선도전 수수료 지역 정보
g_tbCCA_GuidanceMapInfo = {}

-- 로드 완료
function LoadClubCommisionArea()

	-- 지역
	SetEvent( WIDGET_ID_CLUBCOMMISIONAREA, EVENT_LIST_INDEX_CHANGE, "Ev_CCA_SelectArea", "index", "comboArea" )

end

-- 오픈
function OpenClubCommisionArea()

	-- 선도전 수수료 맵 정보 요청
	g_Msg:ReqGuidanceMapName()

end

-- 지역 설정
function CCA_SetArea()

	local nSelectArea = 0

	local tbListData = {}
	for value in pairs( g_tbCCA_GuidanceMapInfo ) do
	
		if value <= g_nCCA_AreaCount then
		
			local tbInfo = g_tbCCA_GuidanceMapInfo[ value ]
			-- 수수료 맵이 있을 경우만 표시한다.
			if 0 < table.getn( tbInfo[ 3 ] ) then
			
				if 0 == nSelectArea then
					nSelectArea = value
				end
			
				local tbData = 
				{
					-- 데이터에 저장된 선도전 이름도 안쓴다 한다.
					--tbData[ 1 ]
					GetGameWordText( "RENEWAL_GUIDANCE_INFO_NAME_WORD", value - 1 ),
					value
				}
				table.insert( tbListData, tbData )
			
			end
		
		end
	
	end
	
	if 0 == table.getn( tbListData ) then
	
		RemoveData( WIDGET_ID_CLUBCOMMISIONAREA, 0, "comboArea", true )
		SetInt( WIDGET_ID_CLUBCOMMISIONAREA, "selectedIndex", -1, "comboArea" )
	
	else
	
		local strParam = CreateParam( "label", "tableIndex" )
		SetDataEx( WIDGET_ID_CLUBCOMMISIONAREA, strParam, tbListData, "comboArea", false )
		SetInt( WIDGET_ID_CLUBCOMMISIONAREA, "selectedIndex", 0, "comboArea" )
		
		-- 수수료 맵 리스트 설정
		CCA_SetCommisionMapList( nSelectArea )
	
	end

end

-- 수수료 맵 리스트 설정
function CCA_SetCommisionMapList( nIndex )

	local tbInfo = g_tbCCA_GuidanceMapInfo[ nIndex ]
	if nil == tbInfo then
	
		RemoveData( WIDGET_ID_CLUBCOMMISIONAREA, 0, "list", true )
		return
	
	end
	
	-- 수수료
	local str = GetGameWordText( "ITEM_BASIC_INFO", 6 ) .. " : " .. string.format( "%.0f%%", tbInfo[ 2 ] )
	SetString( WIDGET_ID_CLUBCOMMISIONAREA, "text", str, "labelCommision" )
	
	local tbListData = {}
	for value in pairs( tbInfo[ 3 ] ) do
	
		local strMapName = tbInfo[ 3 ][ value ]
		
		local tbData = 
		{
			value,			-- 인덱스
			strMapName,		-- 맵 이름
		}
		table.insert( tbListData, tbData )
	
	end
	
	if 0 == table.getn( tbListData ) then
	
		RemoveData( WIDGET_ID_CLUBCOMMISIONAREA, 0, "list", true )
	
	else
	
		local strParam = CreateParam( "label1", "label2" )
		SetDataEx( WIDGET_ID_CLUBCOMMISIONAREA, strParam, tbListData, "list", false )
	
	end

end

------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 업데이트
function ClubCommisionArea_Update()

	g_tbCCA_GuidanceMapInfo = g_Club:GetGudianceMapInfo()

	if false == IsOpenWidget( WIDGET_ID_CLUBCOMMISIONAREA ) then
		return
	end
	
	CCA_SetArea()

end

------------------------------------------------------------------------------------------------
-- 이벤트

-- 지역 선택
function Ev_CCA_SelectArea( nIndex )

	if -1 == nIndex then
		return
	end
	
	local nTableIndex = GetDataInt( WIDGET_ID_CLUBCOMMISIONAREA, nIndex, "tableIndex", "comboArea" )
	CCA_SetCommisionMapList( nTableIndex )

end