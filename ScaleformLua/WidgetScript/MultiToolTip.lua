
-- 헤더 최소 height
ITEMMULTIINFO_HEADER_MIN_HEIGHT = 40

-- 멀티 툴팁 정보
g_tbMultiTooltip_Info = {}

-- 현재 드래그 중인 놈 SubID
g_nMultiTooltip_DraggingSubID = 0

-- 멀티 툴팁 로드 완료
function MultiToolTipLoad( nSubID )

	local tbInfo = g_tbMultiTooltip_Info[ nSubID ]
	if nil == tbInfo then
		return
	end
	
	-- 윈도우 visible 끄기
	SetWindowVisible( WIDGET_ID_MULTITOOLTIP, false, nSubID )
	
	-- 슬롯 ID 등록
	RegSlot( SLOT_ITEMBASE, MultiTooltip_CreateSlotID( nSubID ) )

	-- 멀티 툴팁 추가 슬롯
	for i = 1, TOOLTIP_ADD_SLOT_COUNT, 1 do
		RegSlot( SLOT_ITEMBASE, MultiTooltip_CreateAddSlotID( nSubID, i ) )
	end
	
	-- 멀티 툴팁 교환 슬롯
	for i = 1, TOOLTIP_ADD_TRADE_SLOT_COUNT, 1 do
		RegSlot( SLOT_ITEMBASE, MultiTooltip_CreateTradeSlotID( nSubID, i ) )
	end

	-- 닫기 버튼 이벤트 
	SetEvent( WIDGET_ID_MULTITOOLTIP, EVENT_BUTTON_CLICK, "Ev_MultiItemToolTip_Close", "", "btnClose", nSubID, nSubID )
	-- 드레그 이벤트
	SetEvent( WIDGET_ID_MULTITOOLTIP, EVENT_MOUSE_DOWN, "Ev_MultiItemToolTip_DragStart", "stageX;stageY", "", nSubID, nSubID )
	
	-- 커스텀 드래그 사용
	SetBool( WIDGET_ID_MULTITOOLTIP, "customDrag", true, "", nSubID )
	
	-- 슬롯 마우스 사용 끄기
	HitTestEnable( WIDGET_ID_MULTITOOLTIP, false, "slot", nSubID )
	
	-- 슬롯 바인딩
	SetString( WIDGET_ID_MULTITOOLTIP, "binding", MultiTooltip_CreateSlotID( nSubID ), "slot", nSubID )

	-- 추가 슬롯 바인딩
	for i = 1, ITEMLINKINFO_ADD_SLOT_TOTAL, 1 do
	
		SetString( WIDGET_ID_MULTITOOLTIP, "binding", 
			MultiTooltip_CreateAddSlotID( nSubID, i ), "addSlot" .. tostring( i ), nSubID )
			
	end
	
	-- 교환 슬롯 바인딩
	for i = 1, TOOLTIP_ADD_TRADE_SLOT_COUNT, 1 do
	
		SetString( WIDGET_ID_MULTITOOLTIP, "binding", 
			MultiTooltip_CreateTradeSlotID( nSubID, i ), "TradeSlot" .. tostring( i ), nSubID )
			
	end
	
	tbInfo[ "nHeaderGap" ] = GetInt( WIDGET_ID_MULTITOOLTIP, "y", "tfInfo", nSubID ) - (
		GetInt( WIDGET_ID_MULTITOOLTIP, "y", "tfHeader", nSubID ) + 
		GetInt( WIDGET_ID_MULTITOOLTIP, "height", "tfHeader", nSubID ) )
		
	tbInfo[ "nAddSlotGap" ] = GetInt( WIDGET_ID_MULTITOOLTIP, "y", "addSlot1", nSubID ) - (
		GetInt( WIDGET_ID_MULTITOOLTIP, "y", "tfInfo", nSubID ) + 
		GetInt( WIDGET_ID_MULTITOOLTIP, "height", "tfInfo", nSubID ) )
		
end

-- 멀티 툴팁 오픈
function OpenMultiToolTipInfo( nSubID )

	-- 아이템 정보 설정
	MultiToolTipInfo_SetInfo( nSubID )

end

-- 슬롯 ID 만들기
function MultiTooltip_CreateSlotID( nSubID )

	return SLOT_MULTITOOLTIP_ID .. tostring( nSubID )

end

-- 추가 슬롯 ID 만들기
function MultiTooltip_CreateAddSlotID( nSubID, nIndex )

	return SLOT_MULTITOOLTIP_ADD_ID .. tostring( nSubID ) .. "_" .. tostring( nIndex )
	
end

-- 교환 슬롯 ID 만들기
function MultiTooltip_CreateTradeSlotID( nSubID, nIndex )

	return SLOT_MULTITOOLTIP_TRADE_ITEM_ID .. tostring( nSubID ) .. "_" .. tostring( nIndex )
	
end

-- 아이템 정보 설정
function MultiToolTipInfo_SetInfo( nSubID )

	local tbInfo = g_tbMultiTooltip_Info[ nSubID ]
	if nil == tbInfo then
		return
	end

	-- 슬롯 아이템 등록
	SetSlot_Item( MultiTooltip_CreateSlotID( nSubID ), tbInfo[ "nMID" ], tbInfo[ "nSID" ], 1 )
	
	-- 아이템 정보 텍스트
	SetString( WIDGET_ID_MULTITOOLTIP, "htmlText", tbInfo[ "strHeader" ], "tfHeader", nSubID )
	SetString( WIDGET_ID_MULTITOOLTIP, "htmlText", tbInfo[ "strInfo" ], "tfInfo", nSubID )
	
	local nTextHeight = 0
	
	------------------------------------------------------------
	-- 헤더 height 설정
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_MULTITOOLTIP, "textHeight", "tfHeader", nSubID )
	if nTextHeight < ITEMMULTIINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = ITEMMULTIINFO_HEADER_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + 10
	end
	
	SetInt( WIDGET_ID_MULTITOOLTIP, "height", nNewHeaderHeight, "tfHeader", nSubID )
	
	------------------------------------------------------------
	-- 인포 y 설정
	local nNewInfoY = GetInt( WIDGET_ID_MULTITOOLTIP, "y", "tfHeader", nSubID ) + 
		nNewHeaderHeight-- + tbInfo[ "nHeaderGap" ]
	SetInt( WIDGET_ID_MULTITOOLTIP, "y", nNewInfoY, "tfInfo", nSubID )
	
	------------------------------------------------------------
	-- 인포 height 설정
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_MULTITOOLTIP, "textHeight", "tfInfo", nSubID )
	nNewInfoHeight = nTextHeight + 10
	
	SetInt( WIDGET_ID_MULTITOOLTIP, "height", nNewInfoHeight, "tfInfo", nSubID )
	
	------------------------------------------------------------
	-- 추가 슬롯 설정 및 배경 사이즈 설정
	
	local nNewSlotY = GetInt( WIDGET_ID_MULTITOOLTIP, "y", "tfInfo", nSubID ) + 
		nNewInfoHeight + tbInfo[ "nAddSlotGap" ] + 5
	local nSlotHeight = GetInt( WIDGET_ID_MULTITOOLTIP, "height", "addSlot1", nSubID )
	
	local nNewBgHeight = nNewHeaderHeight + nNewInfoHeight + 10
	local bHasAddSlot = false
	
	local nAddSlotCount = 0
		
	for i = 1, TOOLTIP_ADD_SLOT_COUNT, 1 do
	
		local strInst = "addSlot" .. tostring( i )
	
		local tbItemID = tbInfo[ "tbBoxItem" ][ i ]
		if nil == tbItemID then
		
			SetBool( WIDGET_ID_MULTITOOLTIP, "visible", false, strInst, nSubID )
		
		else
		
			SetBool( WIDGET_ID_MULTITOOLTIP, "visible", true, strInst, nSubID )
			
			-- 슬롯 아이템 설정
			SetSlot_Item( MultiTooltip_CreateAddSlotID( nSubID, i ), tbItemID[ 1 ], tbItemID[ 2 ], tbItemID[ 3 ] )

			
			-- 슬롯 y 위치 잡기
			local nY = nNewSlotY
			if 20 < i then
				nY = nY + nSlotHeight * 2
			elseif 10 < i then
				nY = nY + nSlotHeight * 1
			end
			
			SetInt( WIDGET_ID_MULTITOOLTIP, "y", nY, strInst, nSubID )
			
			-- 슬롯 라인이 늘어날 때만 배경 사이즈 증가
			if 1 == i or 11 == i or 21 == i then

				nNewBgHeight = nNewBgHeight + nSlotHeight
				bHasAddSlot = true
				nAddSlotCount = nAddSlotCount + 1
				
			end
		
		end
		
	end
	
	---------- 교환 아이템 출력 
	-- 만약 상자 내부 아이템이 10개이상이면 이부분을 수정해야함.. 

	local SlotYTradePos = 40 * nAddSlotCount
	local SlotYLinePos = 20
	
	local bNotTradeCondition = false 
	if table.getn( tbInfo["tbTradeCondition"] ) > 0 then 
		SetString( WIDGET_ID_MULTITOOLTIP, "htmlText", GetGameWordText( "TRADE_ITEM_CONDITION", 0 ), "tfTrade", nSubID )
		bNotTradeCondition = true
	else
		SetString( WIDGET_ID_MULTITOOLTIP, "htmlText", "", "tfTrade", nSubID )
		bNotTradeCondition = false
	end
	
	------------------------------------------------------------
	-- 인포 y 설정
	local nNewTradeY = nNewSlotY +  SlotYTradePos
	SetInt( WIDGET_ID_MULTITOOLTIP, "y", nNewTradeY, "tfTrade", nSubID )
	------------------------------------------------------------
		
	for i = 1, TOOLTIP_ADD_TRADE_SLOT_COUNT, 1 do
		local strInst = "TradeSlot" .. tostring( i )
		local tbCondition = tbInfo["tbTradeCondition"][ i ]
		
		if( tbCondition == nil ) then
		
			SetBool( WIDGET_ID_MULTITOOLTIP, "visible", false, strInst, nSubID )	
			
		else
		
			SetBool( WIDGET_ID_MULTITOOLTIP, "visible", true, strInst, nSubID )	
			--ResetSlot( SLOT_TOOLTIP_TRADE_ITEM_ID .. tostring( i ) )
			SetSlot_Item( MultiTooltip_CreateTradeSlotID( nSubID, i ), tbCondition[ 1 ], tbCondition[ 2 ], tbCondition[ 3 ] )
			
		end
		
		-- 슬롯 y 위치 잡기
		local nY = nNewTradeY + SlotYLinePos
		if 10 < i then
			nY = nY + nSlotHeight * 2
		elseif 5 < i then
			nY = nY + nSlotHeight * 1
		end
			
		SetInt( WIDGET_ID_MULTITOOLTIP, "y", nY, strInst, nSubID )
						
		if bNotTradeCondition == true then
			-- 슬롯 라인이 늘어날 때만 배경 사이즈 증가
			if 1 == i or 6 == i then

				nNewBgHeight = nNewBgHeight + nSlotHeight
				bHasAddSlot = true
				
			end
		end
	end
	
	if true == bHasAddSlot then
		nNewBgHeight = nNewBgHeight + 10
		bHasAddSlot = false
	end
		
	-- 배경 height 설정
	SetInt( WIDGET_ID_MULTITOOLTIP, "height", nNewBgHeight, "bg", nSubID )
	
end

-----------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 멀티 툴팁 오픈
function MultiItemToolTipInfo_Open( _nMID, _nSID, _strHeader, _strInfo )

	local nSubID = 0

	-- 닫힌 툴팁 찾기
	for value in pairs( g_tbMultiTooltip_Info ) do
	
		if false == IsOpenWidget( WIDGET_ID_MULTITOOLTIP, value ) then
			nSubID = value
		end
	
	end

	-- 닫힌게 없다.
	if 0 == nSubID then
	
		-- 동일한 아이템이 열려있는지 확인
		for value in pairs( g_tbMultiTooltip_Info ) do
		
			local tbInfo = g_tbMultiTooltip_Info[ value ]
			
			if 	tbInfo[ "nMID" ] == _nMID and
				tbInfo[ "nSID" ] == _nSID then
				
				CallFunction( WIDGET_ID_MULTITOOLTIP, "onWindowSwapToFront", "", "", value )
				return
				
			end
		
		end
	
		-- 빈 테이블 추가
		table.insert( g_tbMultiTooltip_Info, {} )
		-- 추가한 테이블 인덱스(SubID)
		nSubID = table.getn( g_tbMultiTooltip_Info )
		
	end
	
	-- 툴팁 정보 삽입
	g_tbMultiTooltip_Info[ nSubID ] = 
	{
		-- 아이템 ID
		nMID = _nMID,
		nSID = _nSID,
		
		-- 툴팁 텍스트
		strHeader = _strHeader,
		strInfo = _strInfo,
		
		-- 아이템 툴팁에 있는 추가 슬롯 아이템은 변경 될 수 있으니 복사해서 가져와야된다.
		tbBoxItem = CopyTable( g_tbItemTooltip_AddSlotItem ),
		
		-- 교환 아이템 
		tbTradeCondition = CopyTable( g_tbCopty_ItemTooltip_TradeCondtion ),
		
		-- 헤더/인포 간격
		nHeaderGap = 0,
		-- 인포/추가슬롯 간격
		nAddSlotGap = 0,
		
		-- 드래그 관련 변수
		nMouseX = 0,
		nMouseY = 0,
		nDragX = 0,
		nDragY = 0
	}
	
	OpenWidget( WIDGET_ID_MULTITOOLTIP, nSubID )
	
end

----------------------------------------이벤트-------------------------------------------------------------------------------

-- 닫기 버튼 클릭
function Ev_MultiItemToolTip_Close( strSubID )

	CloseWidget( WIDGET_ID_MULTITOOLTIP, tonumber( strSubID ) )
	
end

-- 드래그 이벤트 -------------------------------
-- 드래그 시작 이벤트
function Ev_MultiItemToolTip_DragStart( nStageX, nStageY, strSubID )

	local nSubID = tonumber( strSubID )

	local tbInfo = g_tbMultiTooltip_Info[ nSubID ]
	if nil == tbInfo then
		return
	end

	InsertStageMouseMove( Ev_MultiItemToolTip_Dragging )
	InsertStageMouseUp( Ev_MultiItemToolTip_DragEnd )
	
	tbInfo[ "nMouseX" ] = nStageX
	tbInfo[ "nMouseY" ] = nStageY
	
	tbInfo[ "nDragX" ] = GetInt( WIDGET_ID_MULTITOOLTIP, "x", "", nSubID )
	tbInfo[ "nDragY" ] = GetInt( WIDGET_ID_MULTITOOLTIP, "y", "", nSubID )
	
	g_nMultiTooltip_DraggingSubID = nSubID
	
end

-- 드래그 이동
function Ev_MultiItemToolTip_Dragging( nStageX, nStageY )

	-- 0일수 없다.
	if 0 == g_nMultiTooltip_DraggingSubID then
	
		RemoveStageMouseMove( Ev_MultiItemToolTip_Dragging )
		RemoveStageMouseUp( Ev_MultiItemToolTip_DragEnd )
		
		return
	
	end
	
	local tbInfo = g_tbMultiTooltip_Info[ g_nMultiTooltip_DraggingSubID ]
	if nil == tbInfo then
		return
	end

	local nOffsetX = 0
	local nOffsetY = 0
	nOffsetX = nStageX - tbInfo[ "nMouseX" ]
	nOffsetY = nStageY - tbInfo[ "nMouseY" ]
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_MULTITOOLTIP,
		tbInfo[ "nDragX" ] + nOffsetX, tbInfo[ "nDragY" ] + nOffsetY, 
		-24, -4, -24, 0,
		g_nMultiTooltip_DraggingSubID, "bg" )
		
	SetInt( WIDGET_ID_MULTITOOLTIP, "x", nNewX, "", g_nMultiTooltip_DraggingSubID )
	SetInt( WIDGET_ID_MULTITOOLTIP, "y", nNewY, "", g_nMultiTooltip_DraggingSubID )
	
end

-- 드래그 끝
function Ev_MultiItemToolTip_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_MultiItemToolTip_Dragging )
	RemoveStageMouseUp( Ev_MultiItemToolTip_DragEnd )

end