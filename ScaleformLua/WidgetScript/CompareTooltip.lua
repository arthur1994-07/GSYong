
g_wCompareTooltip_MID = -1
g_wCompareTooltip_SID = -1
g_strCompareTooltip_Header = ""
g_strComapreTooltip_Tooltip = ""

g_nCompareTooltip_HeaderInfoGap = 0

-- 비교 툴팁 로드 완료
function LoadCompareTooltip()

	HitTestEnable( WIDGET_ID_COMPARETOOLTIP, false, "" )

	-- 닫기 버튼 끄기
	SetBool( WIDGET_ID_COMPARETOOLTIP, "visible", false, "btnClose" )
	-- 슬롯 바인딩
	SetString( WIDGET_ID_COMPARETOOLTIP, "binding", SLOT_COMPARETOOLTIP_ID, "slot" )
	
	g_nCompareTooltip_HeaderInfoGap = GetInt( WIDGET_ID_COMPARETOOLTIP, "y", "tfInfo" ) - ( 
		GetInt( WIDGET_ID_COMPARETOOLTIP, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_COMPARETOOLTIP, "height", "tfHeader" ) )
		
	-- 추가 슬롯 필요 없음.
	for i = 1, ITEMLINKINFO_ADD_SLOT_TOTAL, 1 do
	
		local strInst = "addSlot" .. tostring( i )
		SetBool( WIDGET_ID_COMPARETOOLTIP, "visible", false, strInst )
		
	end

end

-- 비교 툴팁 오픈
function OpenCompareTooltip()

	-- 슬롯 아이템 등록
	SetSlot_Item( SLOT_COMPARETOOLTIP_ID, g_wCompareTooltip_MID, g_wCompareTooltip_SID, 1 )
	
	-- 아이템 정보 텍스트
	SetString( WIDGET_ID_COMPARETOOLTIP, "htmlText", g_strCompareTooltip_Header, "tfHeader" )
	SetString( WIDGET_ID_COMPARETOOLTIP, "htmlText", g_strComapreTooltip_Tooltip, "tfInfo" )
	
	local nTextHeight = 0
	
	------------------------------------------------------------
	-- 헤더 height 설정
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_COMPARETOOLTIP, "textHeight", "tfHeader" )
	if nTextHeight < ITEMLINKINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = ITEMLINKINFO_HEADER_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + 10
	end
	
	SetInt( WIDGET_ID_COMPARETOOLTIP, "height", nNewHeaderHeight, "tfHeader" )
	
	------------------------------------------------------------
	-- 인포 y 설정
	local nNewInfoY = GetInt( WIDGET_ID_COMPARETOOLTIP, "y", "tfHeader" ) + 
		nNewHeaderHeight + g_nCompareTooltip_HeaderInfoGap
	SetInt( WIDGET_ID_COMPARETOOLTIP, "y", nNewInfoY, "tfInfo" )
	
	------------------------------------------------------------
	-- 인포 height 설정
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_COMPARETOOLTIP, "textHeight", "tfInfo" )
	nNewInfoHeight = nTextHeight + 10
	
	SetInt( WIDGET_ID_COMPARETOOLTIP, "height", nNewInfoHeight, "tfInfo" )
	
	------------------------------------------------------------
	-- 배경 사이즈 설정

	-- 배경 height 설정
	SetInt( WIDGET_ID_COMPARETOOLTIP, "height", nNewHeaderHeight + nNewInfoHeight + 10, "bg" )
	
	-- 위치 설정
	local tbPos = GetSlotTooltipPosSize()
	
	local nNewX = tbPos[ 1 ] + tbPos[ 3 ]
	local nNewY = tbPos[ 2 ]
	
	local nW = GetInt( WIDGET_ID_COMPARETOOLTIP, "width", "" )
	if GetScreenWidth() < nNewX + nW then
	
		SetInt( WIDGET_ID_COMPARETOOLTIP, "x", tbPos[ 1 ] - nW - 1, "" )
		
	else
	
		SetInt( WIDGET_ID_COMPARETOOLTIP, "x", nNewX + 1, "" )
	
	end
	
	SetInt( WIDGET_ID_COMPARETOOLTIP, "y", nNewY, "" )

end

------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 툴팁 설정
function CompareTooltip_SetInfo( wMID, wSID, strHeader, strTooltip )

	g_wCompareTooltip_MID = wMID
	g_wCompareTooltip_SID = wSID
	
	g_strCompareTooltip_Header = strHeader
	g_strComapreTooltip_Tooltip = strTooltip
	
	if false == IsOpenWidget( WIDGET_ID_COMPARETOOLTIP ) then
	
		OpenWidget( WIDGET_ID_COMPARETOOLTIP )
	
	else
	
		OpenCompareTooltip()
	
	end

end