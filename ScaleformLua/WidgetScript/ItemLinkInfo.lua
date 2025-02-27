
-- 헤더 최소 height
ITEMLINKINFO_HEADER_MIN_HEIGHT = 40

-- 아이템 ID
g_nILI_MID = 0
g_nILI_SID = 0
g_nILI_RealMID = 0
g_nILI_RealSID = 0

-- 헤더 텍스트
g_strILI_Header = ""
-- 인포 텍스트
g_strILI_Info = ""

-- 박스 아이템
g_tbILI_Boxtem = {}

-- 헤더/인포 간격
g_nILI_HeaderInfoGap = 0
-- 인포/추가 슬롯 간격
g_nILI_InfoAddSlotGap = 0

-- 아이템 링크 정보 UI 로드 완료
function LoadItemLinkInfo()

	-- 닫기 버튼 이벤트 
	SetEvent( WIDGET_ID_ITEMLINKINFO, EVENT_BUTTON_CLICK, "Ev_ILI_Close", "", "btnClose" )
	
	SetBool( WIDGET_ID_ITEMLINKINFO, "tooltipEnabled", false, "slot" )
	
	-- 슬롯 바인딩
	SetString( WIDGET_ID_ITEMLINKINFO, "binding", SLOT_ILI_ID, "slot" )
	
	for i = 1, ITEMLINKINFO_ADD_SLOT_TOTAL, 1 do
		SetString( WIDGET_ID_ITEMLINKINFO, "binding", SLOT_ILI_ADD_ID .. tostring( i ), "addSlot" .. tostring( i ) )
	end
	
	-- 교환 슬롯 바인딩
	for i = 1, TOOLTIP_ADD_TRADE_SLOT_COUNT, 1 do
	
		SetString( WIDGET_ID_ITEMLINKINFO, "binding", SLOT_ILI_ADD_ID .. tostring( i ) , "TradeSlot" .. tostring( i ) )
			
	end
		
	g_nILI_HeaderInfoGap = GetInt( WIDGET_ID_ITEMLINKINFO, "y", "tfInfo" ) - ( 
		GetInt( WIDGET_ID_ITEMLINKINFO, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_ITEMLINKINFO, "height", "tfHeader" ) )
		
	g_nILI_InfoAddSlotGap = GetInt( WIDGET_ID_ITEMLINKINFO, "y", "addSlot1" ) - (
		GetInt( WIDGET_ID_ITEMLINKINFO, "y", "tfInfo" ) + 
		GetInt( WIDGET_ID_ITEMLINKINFO, "height", "tfInfo" ) )
	
end

-- 아이템 링크 정보 오픈
function OpenItemLinkInfo()
	-- 아이템 정보 설정
	ItemLinkInfo_SetInfo()

end

-- 아이템 정보 설정
function ItemLinkInfo_SetInfo()	

	-- 슬롯 아이템 등록
	SetSlot_Item_NonStrengthen( SLOT_ILI_ID, g_nILI_MID, g_nILI_SID, 1, g_nILI_RealMID, g_nILI_RealSID )
	
	-- 아이템 정보 텍스트
	SetString( WIDGET_ID_ITEMLINKINFO, "htmlText", g_strILI_Header, "tfHeader" )
	SetString( WIDGET_ID_ITEMLINKINFO, "htmlText", g_strILI_Info, "tfInfo" )
	
	local nTextHeight = 0
	
	------------------------------------------------------------
	-- 헤더 height 설정
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_ITEMLINKINFO, "textHeight", "tfHeader" )
	if nTextHeight < ITEMLINKINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = ITEMLINKINFO_HEADER_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + 10
	end
	
	SetInt( WIDGET_ID_ITEMLINKINFO, "height", nNewHeaderHeight, "tfHeader" )
	
	------------------------------------------------------------
	-- 인포 y 설정
	local nNewInfoY = GetInt( WIDGET_ID_ITEMLINKINFO, "y", "tfHeader" ) + nNewHeaderHeight + g_nILI_HeaderInfoGap
	SetInt( WIDGET_ID_ITEMLINKINFO, "y", nNewInfoY, "tfInfo" )
	
	------------------------------------------------------------
	-- 인포 height 설정
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_ITEMLINKINFO, "textHeight", "tfInfo" )
	nNewInfoHeight = nTextHeight + 10
	
	SetInt( WIDGET_ID_ITEMLINKINFO, "height", nNewInfoHeight, "tfInfo" )
	
	------------------------------------------------------------
	-- 추가 슬롯 설정 및 배경 사이즈 설정
	
	local nNewSlotY = GetInt( WIDGET_ID_ITEMLINKINFO, "y", "tfInfo" ) + nNewInfoHeight + g_nILI_InfoAddSlotGap
	local nSlotHeight = GetInt( WIDGET_ID_ITEMLINKINFO, "height", "addSlot1" )
	
	local nNewBgHeight = nNewHeaderHeight + nNewInfoHeight + 10
	local bHasAddSlot = false
	
	for i = 1, ITEMLINKINFO_ADD_SLOT_TOTAL, 1 do
	
		local strInst = "addSlot" .. tostring( i )
	
		local tbItemID = g_tbILI_Boxtem[ i ]
		if nil == tbItemID then
		
			SetBool( WIDGET_ID_ITEMLINKINFO, "visible", false, strInst )
		
		else
		
			SetBool( WIDGET_ID_ITEMLINKINFO, "visible", true, strInst )
			
			-- 슬롯 아이템 설정
			SetSlot_Item( SLOT_ILI_ADD_ID .. tostring( i ), tbItemID[ 1 ], tbItemID[ 2 ], 1 )
			
			-- 슬롯 y 위치 잡기
			local nY = nNewSlotY
			if 20 < i then
				nY = nY + nSlotHeight * 2
			elseif 10 < i then
				nY = nY + nSlotHeight * 1
			end
			
			SetInt( WIDGET_ID_ITEMLINKINFO, "y", nY, strInst )
			
			-- 슬롯 라인이 늘어날 때만 배경 사이즈 증가
			if 1 == i or 11 == i or 21 == i then

				nNewBgHeight = nNewBgHeight + nSlotHeight
				bHasAddSlot = true
				
			end
		
		end
		
	end
	
	if true == bHasAddSlot then
		nNewBgHeight = nNewBgHeight + 10
	end
	
	
	-- 교환 슬롯 제거
	SetString( WIDGET_ID_ITEMLINKINFO, "htmlText", "", "tfTrade" )
	
	
	for i = 1, TOOLTIP_ADD_TRADE_SLOT_COUNT, 1 do
		local strInst = "TradeSlot" .. tostring( i )
		SetBool( WIDGET_ID_ITEMLINKINFO, "visible", false, strInst )
	end
	
	-- 배경 height 설정
	SetInt( WIDGET_ID_ITEMLINKINFO, "height", nNewBgHeight, "bg" )
	
	-- UI 위치 잡기
	SetInt( WIDGET_ID_ITEMLINKINFO, "y", GetScreenHeight() - nNewBgHeight - 70, "" )
	
end

-----------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 아이템 링크 클릭
function ItemLinkInfo_Open( nMID, nSID, nRealMID, nRealSID, strHeader, strInfo )
	
	g_nILI_MID = nMID
	g_nILI_SID = nSID
	g_nILI_RealMID = nRealMID
	g_nILI_RealSID = nRealSID
	g_strILI_Header = strHeader
	g_strILI_Info = strInfo
	g_tbILI_Boxtem = g_tbItemTooltip_AddSlotItem
	
	if false == IsOpenWidget( WIDGET_ID_ITEMLINKINFO ) then
		OpenWidget( WIDGET_ID_ITEMLINKINFO )
	else
		ItemLinkInfo_SetInfo()
	end

end

-----------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 닫기 버튼 클릭
function Ev_ILI_Close()
	CloseWidget( WIDGET_ID_ITEMLINKINFO )
end
