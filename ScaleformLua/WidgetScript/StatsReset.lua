g_StatsReset_tbData = {}
g_StatsReset_tbEvenly = {} 
g_StatsReset_tbItem = {}
g_StatsReset_wReturnStats = 0	-- 반환설정한 능력치
g_StatsReset_wReturnPoint = 0	-- 반환가능 포인트

g_StatesReset_nSubtract_Pow = 0	-- 초기화 될 분배한 포인트(힘)
g_StatesReset_nSubtract_Dex = 0	-- 초기화 될 분배한 포인트(민첩)
g_StatesReset_nSubtract_Spi = 0	-- 초기화 될 분배한 포인트(정신)
g_StatesReset_nSubtract_Str = 0	-- 초기화 될 분배한 포인트(체력)
g_StatesReset_nSubtract_Sta = 0	-- 초기화 될 분배한 포인트(근력)

g_tbStatsReset_StatText = {}

-- 스탯 초기화 로드 완료
function LoadStatsReset()

	SetEvent( WIDGET_ID_STATS_RESET, EVENT_WIDGET_CLOSE, "CloseStatsReset", "", "" )
	
	g_tbStatsReset_StatText = 
	{
		GetGameWordText( "CHARACTER_STAT_STATIC", 0 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 1 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 2 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 4 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 5 ),
	}

	-- 현재스텟
	local str = HTMLFontColor( GetGameWordText( "STATSRESET_TOPIC_STATS_CURRENT", 0 ), HTML_TEXT_COLOR_ORANGE )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "infoStats" )
	
	str = HTMLFontColor( GetGameWordText( "STATSRESET_TOPIC_STATS_PREVIEW_SELECT", 0 ), HTML_TEXT_COLOR_ORANGE )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "setStats" )
	
	str = HTMLFontColor( GetGameWordText( "STATSRESET_TOPIC_ITEM_RETURN", 0 ), HTML_TEXT_COLOR_ORANGE )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnItem" )
	
	-- 포인트 텍스트
	str = GetGameWordText( "CHARACTER_STAT_POINT", 0 )
	SetString( WIDGET_ID_STATS_RESET, "text", str, "StaticInfoPoint" )
	
	str = GetGameWordText( "STATSRESET_BUTTON", 0 )
	SetString( WIDGET_ID_STATS_RESET, "label", str, "OkButton" )
	
	str = GetGameWordText( "STATSRESET_BUTTON", 1 )
	SetString( WIDGET_ID_STATS_RESET, "label", str, "CloseButton" )
	
	str = GetGameWordText( "STATSRESET_BUTTON", 2 )
	SetString( WIDGET_ID_STATS_RESET, "label", str, "RefreshButton" )
	
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnUp_Pow", "", "btnUpPow" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnUp_Dex", "", "btnUpDex" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnUp_Spi", "", "btnUpSpi" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnUp_Str", "", "btnUpStr" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnUp_Sta", "", "btnUpSta" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnDown_Pow", "", "btnDownPow" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnDown_Dex", "", "btnDownDex" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnDown_Spi", "", "btnDownSpi" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnDown_Str", "", "btnDownStr" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_BtnDown_Sta", "", "btnDownSta" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_Btn_Ok", "", "OkButton" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_Btn_Close", "", "CloseButton" )
	SetEvent( WIDGET_ID_STATS_RESET, EVENT_BUTTON_CLICK, "Ev_StatsReset_Btn_Refresh", "", "RefreshButton" )
	
end

-- 스탯 초기화 오픈
function OpenStatsReset()

	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 1 ], "StaticInfoPow" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 2 ], "StaticInfoDex" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 3 ], "StaticInfoSpi" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 4 ], "StaticInfoStr" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 5 ], "StaticInfoSta" )

	StatsReset_Intit()

end

function CloseStatsReset()

	g_StatsReset_tbData = {}
	g_StatsReset_tbEvenly = {} 
	g_StatsReset_tbItem = {}
	g_StatsReset_wReturnStats = 0
	g_StatsReset_wReturnPoint = 0

	g_StatesReset_nSubtract_Pow = 0
	g_StatesReset_nSubtract_Dex = 0
	g_StatesReset_nSubtract_Spi = 0
	g_StatesReset_nSubtract_Str = 0
	g_StatesReset_nSubtract_Sta = 0

end

function StatsReset_Intit()

	local nPow = g_MyChar:GetPow()
	local nDex = g_MyChar:GetDex()
	local nSpi = g_MyChar:GetSpi()
	local nStr = g_MyChar:GetStr()
	local nSta = g_MyChar:GetSta()
	local nAddedPow = g_MyChar:GetAddedPow()
	local nAddedDex = g_MyChar:GetAddedDex()
	local nAddedSpi = g_MyChar:GetAddedSpi()
	local nAddedStr = g_MyChar:GetAddedStr()
	local nAddedSta = g_MyChar:GetAddedSta()

	-- 현재 자신의 스텟량
	SetString( WIDGET_ID_STATS_RESET, "text", nPow, "labelPowOrig" )
	SetString( WIDGET_ID_STATS_RESET, "text", nDex, "labelDexOrig" )
	SetString( WIDGET_ID_STATS_RESET, "text", nSpi, "labelSpiOrig" )
	SetString( WIDGET_ID_STATS_RESET, "text", nStr, "labelStrOrig" )
	SetString( WIDGET_ID_STATS_RESET, "text", nSta, "labelStaOrig" )
	-- 각 스텟에 투자된 포인트 량
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedPow, "labelPowAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedDex, "labelDexAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedSpi, "labelSpiAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedStr, "labelStrAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedSta, "labelStaAdd" )
	
	if g_StatsReset_tbData["bSelectable"] then
	
		local str = GetGameInternalText( "STATSRESET_RETURN_STATS", 0 ) .. " + 0/" .. g_StatsReset_tbData[ "wReturnStats" ]
		str = HTMLFontColor( str, HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnStatsPoint" )
		
		str = HTMLFontColor( g_StatsReset_tbData[ "wStatsPoint" ], HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "setPoint" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", true, "RefreshButton" )
		
	else		
	
		local str = GetGameInternalText( "STATSRESET_RETURN_STATS" ) ..	" +" .. g_StatsReset_tbData[ "wReturnStats" ]
		str = HTMLFontColor( str, HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnStatsPoint" )
		
		str = HTMLFontColor( g_StatsReset_tbData[ "wStatsPoint" ] + 
			g_StatsReset_tbData[ "wReturnStats" ], HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "setPoint" )
		
		nPow = nPow - g_StatesReset_nSubtract_Pow
		nDex = nDex - g_StatesReset_nSubtract_Dex
		nSpi = nSpi - g_StatesReset_nSubtract_Spi
		nStr = nStr - g_StatesReset_nSubtract_Str
		nSta = nSta - g_StatesReset_nSubtract_Sta
		nAddedPow = nAddedPow - g_StatesReset_nSubtract_Pow
		nAddedDex = nAddedDex - g_StatesReset_nSubtract_Dex
		nAddedSpi = nAddedSpi - g_StatesReset_nSubtract_Spi
		nAddedStr = nAddedStr - g_StatesReset_nSubtract_Str
		nAddedSta = nAddedSta - g_StatesReset_nSubtract_Sta

		SetBool( WIDGET_ID_STATS_RESET, "visible", false,"RefreshButton" )
		
	end
	
	-- 스텟 반환 후 적용스텟
	SetString( WIDGET_ID_STATS_RESET, "text", nPow, "labelPowReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", nDex, "labelDexReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", nSpi, "labelSpiReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", nStr, "labelStrReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", nSta, "labelStaReturn" )
	
	-- 스텟 반환 후 투자된 스텟
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedPow, "labelPowReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedDex, "labelDexReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedSpi, "labelSpiReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedStr, "labelStrReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+" .. nAddedSta, "labelStaReturnAdd" )
	
	-- 포인트(현재스텟 포인트)
	SetString( WIDGET_ID_STATS_RESET, "text", tostring( g_StatsReset_tbData[ "wStatsPoint" ] ), "infoPoint" )
	
	str = HTMLFontColor( GetGameInternalText( "STATSRESET_RETURN_SKILL", 0 ) .. 
		" +" .. g_StatsReset_tbData[ "wReturnSkillPoint" ], HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnSkillPoint" )
	
	-- 반환 게임머니
	str = HTMLFontColor( GetGameInternalText( "STATSRESET_RETURN_SKILLMONEY", 0 ) .. 
		": " .. tostring(g_MyChar:GetLearnedSkillCost()) , HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnSkillMoney" )
	
	str = HTMLFontColor( GetGameInternalText( "STATSRESET_RETURN_ITEM", 0 ) ..
		" " .. g_StatsReset_tbData[ "wReturnItem" ], HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnItemCount" )
	
	SetString( WIDGET_ID_STATS_RESET, "text", g_StatsReset_tbData[ "strTopText" ], "topText" )
	
	if 	g_StatsReset_wReturnStats == g_StatsReset_wReturnPoint or 
		false == g_StatsReset_tbData[ "bSelectable" ] then
		
		SetString( WIDGET_ID_STATS_RESET, "text", 
			GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_CONFIRMATION", 0 ), "BottomText" )
		
	else
	
		local strBottom = HTMLFontColor( g_StatsReset_wReturnPoint, HTML_TEXT_COLOR_RED ) .. 
			GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_REMAINED", 0 ) .. "\n" .. 
			GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_CONFIRMATION", 0 )
		SetString( WIDGET_ID_STATS_RESET, "htmlText", strBottom, "BottomText" )
		
	end
	
	if ITEMTYPE_SKP_STAT_RESET_A == g_StatsReset_tbData["emItemType"] then
	
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnUpPow" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnDownPow" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnUpDex" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnDownDex" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnUpSpi" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnDownSpi" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnUpStr" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnDownStr" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnUpSta" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", false, "btnDownSta" )
	
	else
	
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedPow ~= 0, "btnUpPow" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedPow ~= 0, "btnDownPow" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedDex ~= 0, "btnUpDex" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedDex ~= 0, "btnDownDex" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedSpi ~= 0, "btnUpSpi" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedSpi ~= 0, "btnDownSpi" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedStr ~= 0, "btnUpStr" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedStr ~= 0, "btnDownStr" )
		
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedSta ~= 0, "btnUpSta" )
		SetBool( WIDGET_ID_STATS_RESET, "visible", nAddedSta ~= 0, "btnDownSta" )
	
	end
	
	if 0 == table.getn( g_StatsReset_tbItem ) then
	
		RemoveData( WIDGET_ID_STATS_RESET, 0, "ReturnList", true )
	
	else
	
		SetDataEx( WIDGET_ID_STATS_RESET, "label1;label2;", g_StatsReset_tbItem, "ReturnList", false )
	
	end
	
	-- 스텟반환 증감버튼 리피트 시간 설정(누르고 있을때 갱신시간 - 추후 for 문으로 바꿀까 고려중)
	-- 증가버튼
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnUpPow" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnUpDex" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnUpSpi" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnUpStr" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnUpSta" )
	
	-- 감소버튼
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnDownPow" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnDownDex" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnDownSpi" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnDownStr" )
	SetInt( WIDGET_ID_STATS_RESET, "repeatInterval", 15, "btnDownSta" )

end

-- 스탯초기화  열기
function StatsReset_Open( tbData, tbEvenly, tbItem )

	if true == IsOpenWidget( WIDGET_ID_STATS_RESET ) then
	
		local str = GetGameInternalText( "INVEN_LOCK_MSG", 0 )
		CF_PrintMsg_SystemMsg( str, TEXT_COLOR_RED )
		return
	
	end
	
	g_StatsReset_tbData = tbData
	g_StatsReset_tbEvenly = tbEvenly
	g_StatsReset_tbItem = tbItem
	
	g_StatsReset_wReturnPoint = g_StatsReset_tbData["wReturnStats"]		-- 반환 가능 스텟포인트
	if table.getn(g_StatsReset_tbEvenly) then
	
		g_StatesReset_nSubtract_Pow = g_StatsReset_tbEvenly["pow"]		-- 해당 아이템을 사용했을 경우 반환될 힘(망각의 약E 에만 적용됨 - 올 스텟 초기화)
		g_StatesReset_nSubtract_Dex = g_StatsReset_tbEvenly["dex"]		-- 해당 아이템을 사용했을 경우 반환될 민첩(망각의 약E 에만 적용됨 - 올 스텟 초기화)
		g_StatesReset_nSubtract_Spi = g_StatsReset_tbEvenly["spi"]		-- 해당 아이템을 사용했을 경우 반환될 정신(망각의 약E 에만 적용됨 - 올 스텟 초기화)
		g_StatesReset_nSubtract_Str = g_StatsReset_tbEvenly["str"]		-- 해당 아이템을 사용했을 경우 반환될 체력(망각의 약E 에만 적용됨 - 올 스텟 초기화)
		g_StatesReset_nSubtract_Sta = g_StatsReset_tbEvenly["sta"]		-- 해당 아이템을 사용했을 경우 반환될 근력(망각의 약E 에만 적용됨 - 올 스텟 초기화)
		
	end
	
	OpenWidget( WIDGET_ID_STATS_RESET )	
	
end

function StatsReset_ReturnLabelColor( nRet )

	if 0 == nRet then
		return HTML_TEXT_COLOR_WHITE
	else
		return HTML_TEXT_COLOR_GOLD
	end

end

function StatsReset_ResetText()

	-- 스텟 반환 후 적용스텟
	SetString( WIDGET_ID_STATS_RESET, "htmlText", g_MyChar:GetPow() - g_StatesReset_nSubtract_Pow, "labelPowReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_MyChar:GetDex() - g_StatesReset_nSubtract_Dex, "labelDexReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_MyChar:GetSpi() - g_StatesReset_nSubtract_Spi, "labelSpiReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_MyChar:GetStr() - g_StatesReset_nSubtract_Str, "labelStrReturn" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_MyChar:GetSta() - g_StatesReset_nSubtract_Sta, "labelStaReturn" )
	
	-- 스텟 반환 후 투자된 스텟
	SetString( WIDGET_ID_STATS_RESET, "text", "+"..g_MyChar:GetAddedPow() - g_StatesReset_nSubtract_Pow, "labelPowReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+"..g_MyChar:GetAddedDex() - g_StatesReset_nSubtract_Dex, "labelDexReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+"..g_MyChar:GetAddedSpi() - g_StatesReset_nSubtract_Spi, "labelSpiReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+"..g_MyChar:GetAddedStr() - g_StatesReset_nSubtract_Str, "labelStrReturnAdd" )
	SetString( WIDGET_ID_STATS_RESET, "text", "+"..g_MyChar:GetAddedSta() - g_StatesReset_nSubtract_Sta, "labelStaReturnAdd" )
	
	local str = GetGameInternalText( "STATSRESET_RETURN_STATS" ) .. " + 0/" .. g_StatsReset_tbData[ "wReturnStats" ]
	str = HTMLFontColor( str, HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnStatsPoint" )
	
	str = HTMLFontColor( g_StatsReset_tbData[ "wStatsPoint" ], HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "setPoint" )
	
	str = HTMLFontColor( g_StatsReset_wReturnPoint, HTML_TEXT_COLOR_RED ) .. 
		GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_REMAINED", 0 ) .. "\n" .. 
		GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_CONFIRMATION", 0 )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "BottomText" )
	
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 1 ], "StaticInfoPow" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 2 ], "StaticInfoDex" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 3 ], "StaticInfoSpi" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 4 ], "StaticInfoStr" )
	SetString( WIDGET_ID_STATS_RESET, "text", g_tbStatsReset_StatText[ 5 ], "StaticInfoSta" )
	
end

-- 반환설정 적용값 초기화
function StatsReset_ReturnSet()
	
end

-- 스탯 초기화 재계산
function StatsReset_ReCalc()

	if false == IsOpenWidget( WIDGET_ID_STATS_RESET ) then
		return
	end
	
	StatsReset_Intit()

end

-- 스탯 초기화 결과
function StatsReset_Result( nError, wStatNum, wSkillNum, wItemNum )

	-- 실패
	if 0 == nError then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMREQ_RESET_SKST_FB_FAIL", 0 ), TEXT_COLOR_RED )
	
	-- 성공
	elseif 1 == nError then
		CharInfo_UpdateStatInfo()
		CharInfo_UpdateStatBtn()
		CF_PrintMsg_SystemMsg( StringFormat( GetGameInternalText( "SKILLRESET_FB_OK", 0 ), 
			wStatNum, wSkillNum, CurrencyFormat( wItemNum )), TEXT_COLOR_PALEGREEN )
	
	-- 아이템이 없음
	elseif 2 == nError then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMREQ_RESET_SKST_FB_NOITEM", 0 ), TEXT_COLOR_RED )
	
	-- 인벤토리 공간 부족
	elseif 3 == nError then
	
		CF_PrintMsg_SystemMsg( StringFormat( 
		GetGameInternalText( "EMREQ_RESET_SKST_FB_NOINVEN", 0 ), wItemNum ), TEXT_COLOR_PALEGREEN )
	
	-- 스킬창에서 초기화 요청 성공
	elseif 4 == nError then
		CharInfo_UpdateStatInfo()
		CharInfo_UpdateStatBtn()
		CF_PrintMsg_SystemMsg( string.format( GetGameInternalText( "SKILLRESET_FB_OK2", 0 ), 
			wStatNum, wSkillNum ), TEXT_COLOR_PALEGREEN )
	
	end

end

-- 예 버튼 클릭시
function Ev_StatsReset_Btn_Ok()

	if g_StatsReset_tbData["bSelectable"] then		
	
		local strMsgBox = StringFormat( GetGameInternalText("MODAL_RESET_SKILLSTATS", 0), g_StatsReset_wReturnPoint-g_StatsReset_wReturnStats )		
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsgBox, Ev_StatsReset_ReqReset, Event_MsgBox_Close )
		
		return
		
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText("MODAL_RESET_SKILLSTATS_ALL" ),
		Ev_StatsReset_ReqReset, Event_MsgBox_Close )
	
end

-- 아니오 버튼 클릭시
function Ev_StatsReset_Btn_Close()

	CloseWidget( WIDGET_ID_STATS_RESET )
	
end

-- 재설정 버튼 클릭시
function Ev_StatsReset_ReqReset()

	local tbResetStats={}
	tbResetStats["pow"] = g_StatesReset_nSubtract_Pow
	tbResetStats["dex"] = g_StatesReset_nSubtract_Dex
	tbResetStats["spi"] = g_StatesReset_nSubtract_Spi
	tbResetStats["str"] = g_StatesReset_nSubtract_Str
	tbResetStats["sta"] = g_StatesReset_nSubtract_Sta
	
	local nReturnPoint = 0
	-- 구망각
	if ITEMTYPE_SKP_STAT_RESET_A == g_StatsReset_tbData["emItemType"] then
	
		nReturnPoint = g_StatsReset_tbData["dwOldDrugStatsCount"]
	
	-- 신망각
	elseif ITEMTYPE_SKP_STAT_RESET_B == g_StatsReset_tbData["emItemType"] then
	
		nReturnPoint = g_StatsReset_wReturnPoint
	
	end
	
	g_Msg:RequestResetSkillStats( tbResetStats, nReturnPoint, g_StatsReset_tbData["emItemType"] )
	
	Event_MsgBox_Close()
	
	CloseWidget( WIDGET_ID_STATS_RESET )
	
	Skill_ResetInvestPoint()
	
end

function Ev_StatsReset_Btn_Refresh()

	g_StatsReset_wReturnPoint = g_StatsReset_tbData["wReturnStats"]
	g_StatsReset_wReturnStats = 0

	g_StatesReset_nSubtract_Pow = 0
	g_StatesReset_nSubtract_Dex = 0
	g_StatesReset_nSubtract_Spi = 0
	g_StatesReset_nSubtract_Str = 0
	g_StatesReset_nSubtract_Sta = 0
	
	StatsReset_ResetText()
	
end

function StatsReset_UpdateReturn( nOrig, nAdd, nChange, strInst )

	local strColor = StatsReset_ReturnLabelColor( nChange )
	
	local str = HTMLFontColor( nOrig - nChange, strColor )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, strInst )
	
	str = HTMLFontColor( "+" .. nAdd - nChange, strColor )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, strInst .. "Add" )
	
	Ev_StatsReset_UpdatePoints()

end

-- 힘 반환 취소시(▲ 버튼 클릭)
function Ev_StatsReset_BtnUp_Pow()

	if 0 < g_StatesReset_nSubtract_Pow then
	
		g_StatesReset_nSubtract_Pow = g_StatesReset_nSubtract_Pow - 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats -  1
		
		StatsReset_UpdateReturn( g_MyChar:GetPow(), g_MyChar:GetAddedPow(), g_StatesReset_nSubtract_Pow, "labelPowReturn" )
		
	end	
	
end

-- 민첩 반환 취소시(▲ 버튼 클릭)
function Ev_StatsReset_BtnUp_Dex()

	if 0 < g_StatesReset_nSubtract_Dex then
	
		g_StatesReset_nSubtract_Dex = g_StatesReset_nSubtract_Dex - 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats - 1
		
		StatsReset_UpdateReturn( g_MyChar:GetDex(), g_MyChar:GetAddedDex(), g_StatesReset_nSubtract_Dex, "labelDexReturn" )

	end	
	
end

-- 정신 반환 취소시(▲ 버튼 클릭)
function Ev_StatsReset_BtnUp_Spi()

	if 0 < g_StatesReset_nSubtract_Spi then
	
		g_StatesReset_nSubtract_Spi = g_StatesReset_nSubtract_Spi - 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats - 1
		
		StatsReset_UpdateReturn( g_MyChar:GetSpi(), g_MyChar:GetAddedSpi(), g_StatesReset_nSubtract_Spi, "labelSpiReturn" )
		
	end
	
end

-- 체력 반환 취소시(▲ 버튼 클릭)
function Ev_StatsReset_BtnUp_Str()

	if 0 < g_StatesReset_nSubtract_Str then
	
		g_StatesReset_nSubtract_Str = g_StatesReset_nSubtract_Str - 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats - 1
		
		StatsReset_UpdateReturn( g_MyChar:GetStr(), g_MyChar:GetAddedStr(), g_StatesReset_nSubtract_Str, "labelStrReturn" )
		
	end	
	
end

-- 근력 반환 취소시(▲ 버튼 클릭)
function Ev_StatsReset_BtnUp_Sta()

	if 0 < g_StatesReset_nSubtract_Sta then
	
		g_StatesReset_nSubtract_Sta = g_StatesReset_nSubtract_Sta - 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats - 1
		
		StatsReset_UpdateReturn( g_MyChar:GetSta(), g_MyChar:GetAddedSta(), g_StatesReset_nSubtract_Sta, "labelStaReturn" )
		
	end		
	
end

-- 힘 반환시(▼ 버튼 클릭)
function Ev_StatsReset_BtnDown_Pow()

	if 	g_StatesReset_nSubtract_Pow < g_MyChar:GetAddedPow() and 
		g_StatsReset_wReturnStats < g_StatsReset_wReturnPoint then
		
		g_StatesReset_nSubtract_Pow = g_StatesReset_nSubtract_Pow + 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats + 1
		
		StatsReset_UpdateReturn( g_MyChar:GetPow(), g_MyChar:GetAddedPow(), g_StatesReset_nSubtract_Pow, "labelPowReturn" )
		
	end		
	
end

-- 민첩 반환시(▼ 버튼 클릭)
function Ev_StatsReset_BtnDown_Dex()

	if 	g_StatesReset_nSubtract_Dex < g_MyChar:GetAddedDex() and
		g_StatsReset_wReturnStats < g_StatsReset_wReturnPoint then
	
		g_StatesReset_nSubtract_Dex = g_StatesReset_nSubtract_Dex + 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats + 1
		
		StatsReset_UpdateReturn( g_MyChar:GetDex(), g_MyChar:GetAddedDex(), g_StatesReset_nSubtract_Dex, "labelDexReturn" )
		
	end	
	
end

-- 정신 반환시(▼ 버튼 클릭)
function Ev_StatsReset_BtnDown_Spi()

	if 	g_StatesReset_nSubtract_Spi <  g_MyChar:GetAddedSpi() and 
		g_StatsReset_wReturnStats < g_StatsReset_wReturnPoint then
		
		g_StatesReset_nSubtract_Spi = g_StatesReset_nSubtract_Spi + 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats + 1
		
		StatsReset_UpdateReturn( g_MyChar:GetSpi(), g_MyChar:GetAddedSpi(), g_StatesReset_nSubtract_Spi, "labelSpiReturn" )
		
	end
	
end

-- 체력 반환시(▼ 버튼 클릭)
function Ev_StatsReset_BtnDown_Str()

	if 	g_StatesReset_nSubtract_Str < g_MyChar:GetAddedStr() and 
		g_StatsReset_wReturnStats < g_StatsReset_wReturnPoint then
		
		g_StatesReset_nSubtract_Str = g_StatesReset_nSubtract_Str + 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats + 1
		
		StatsReset_UpdateReturn( g_MyChar:GetStr(), g_MyChar:GetAddedStr(), g_StatesReset_nSubtract_Str, "labelStrReturn" )
		
	end	
	
end

-- 근력 반환시(▼ 버튼 클릭)
function Ev_StatsReset_BtnDown_Sta()

	if 	g_StatesReset_nSubtract_Sta < g_MyChar:GetAddedSta() and 
		g_StatsReset_wReturnStats < g_StatsReset_wReturnPoint  then
	
		g_StatesReset_nSubtract_Sta = g_StatesReset_nSubtract_Sta + 1
		g_StatsReset_wReturnStats = g_StatsReset_wReturnStats + 1
		
		StatsReset_UpdateReturn( g_MyChar:GetSta(), g_MyChar:GetAddedSta(), g_StatesReset_nSubtract_Sta, "labelStaReturn" )
		
	end	
	
end

function Ev_StatsReset_UpdatePoints()

	local str = HTMLFontColor( g_tbStatsReset_StatText[ 1 ], StatsReset_ReturnLabelColor( g_StatesReset_nSubtract_Pow ) )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "StaticInfoPow" )
	
	str = HTMLFontColor( g_tbStatsReset_StatText[ 2 ], StatsReset_ReturnLabelColor( g_StatesReset_nSubtract_Dex ) )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "StaticInfoDex" )
	
	str = HTMLFontColor( g_tbStatsReset_StatText[ 3 ], StatsReset_ReturnLabelColor( g_StatesReset_nSubtract_Spi ) )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "StaticInfoSpi" )
	
	str = HTMLFontColor( g_tbStatsReset_StatText[ 4 ], StatsReset_ReturnLabelColor( g_StatesReset_nSubtract_Str ) )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "StaticInfoStr" )
	
	str = HTMLFontColor( g_tbStatsReset_StatText[ 5 ], StatsReset_ReturnLabelColor( g_StatesReset_nSubtract_Sta ) )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "StaticInfoSta" )

	if g_StatsReset_wReturnStats == g_StatsReset_wReturnPoint then
	
		SetString( WIDGET_ID_STATS_RESET, "text", 
			GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_CONFIRMATION" ), "BottomText" )	
		
	else
	
		local strBottom = HTMLFontColor( g_StatsReset_wReturnPoint - g_StatsReset_wReturnStats, HTML_TEXT_COLOR_RED ) .. 
			GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_REMAINED", 0 ) .. "\n" .. 
			GetGameInternalText( "STATSRESET_INFO_TAIL_TEXT_CONFIRMATION", 0 )
			
		SetString( WIDGET_ID_STATS_RESET, "htmlText", strBottom, "BottomText" )
		
	end
	
	local str = HTMLFontColor( GetGameInternalText( "STATSRESET_RETURN_STATS", 0 ) .. " +" ..
		g_StatsReset_wReturnStats .. "/" .. g_StatsReset_tbData[ "wReturnStats" ], HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "returnStatsPoint" )
	
	str = HTMLFontColor( g_StatsReset_tbData[ "wStatsPoint" ] + g_StatsReset_wReturnStats, HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_STATS_RESET, "htmlText", str, "setPoint" )
	
end