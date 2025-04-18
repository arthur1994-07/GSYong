
-- 일반 효과 개수
g_nCostumeStat_NORMALCOUNT 	= 4
-- 변화율 효과 개수
g_nCostumeStat_RATIOCOUNT 	= 9
-- 가산 효과 개수
g_nCostumeStat_ADDONCOUNT 	= 15

-- 투자한 스탯 포인트 테이블
g_tbConstumeStat_InvestPoint = {}
-- 투자한 스탯 종류별 개수, 총합
g_tbConstumeStat_InvestCount = {}

-- 능력치 부여할 아이템 인벤 위치
g_nCostumeStat_InvenItemPosX = 0
g_nCostumeStat_InvenItemPosY = 0

-- 해제서 인벤 위치
g_nCostumeStat_HoldItemPosX = 0
g_nCostumeStat_HoldItemPosY = 0

-- 코스튬 능력치 부여 로드 완료
function LoadCostumeStat()

	-- 닫기 이벤트
	SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_WIDGET_CLOSE, "CloseCostumeStat", "", "" )

	-- 주입 부위
	local str = GetGameWordText( "COSTUME_STAT_ASSIGN", 4 )
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, "part.labelTitle" )
	
	-- 보유 포인트
	str = GetGameWordText( "COSTUME_STAT_ASSIGN", 5 )
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, "point.labelTitle" )
	
	-- 취소
	str = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_COSTUMESTAT, "label", str, "btnCancel" )
	SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_BUTTON_CLICK, "Ev_CostumeStat_Cancel", "", "btnCancel" )
	
	-- 초기화
	str = GetGameWordText( "CHARACTER_STAT_INIT", 0 )
	SetString( WIDGET_ID_COSTUMESTAT, "label", str, "btnReset" )
	SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_BUTTON_CLICK, "Ev_CostumeStat_Reset", "", "btnReset" )
	
	-- 적용
	str = GetGameWordText( "CHARACTER_STAT_APPLY", 0 )
	SetString( WIDGET_ID_COSTUMESTAT, "label", str, "btnApply" )
	SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_BUTTON_CLICK, "Ev_CostumeStat_Apply", "", "btnApply" )
	
	-- 도움말
	str = GetGameInternalText( "COSTUME_STAT_EXPLAIN", 0 )
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, "textHelp" )
	
	-- 일반 효과
	local tbStr = 
	{
		GetGameWordText( "CHARACTER_STATUS_STATIC", 20 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 3 ),
		GetGameWordText( "ITEM_INFO_SET_BONUS", 78 ),
		"-",
	}
	
	for i = 1, g_nCostumeStat_NORMALCOUNT, 1 do
	
		local strInst = "normal" .. tostring( i )
		SetString( WIDGET_ID_COSTUMESTAT, "text", tbStr[ i ], strInst .. ".labelTitle" )
		
		local strParam = CreateParam( i )
		SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_INPUT, "Ev_CostumeStat_InputNormal", 
			"", strInst .. ".input", strParam )
		SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_FOCUS_OUT, "Ev_CostumeStat_InputNormal",
			"", strInst .. ".input", strParam )
	
	end
	
	-- 변화율 효과
	tbStr = 
	{
		GetGameWordText( "CHARACTER_STATUS_STATIC", 21 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 22 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 23 ),
		GetGameWordText( "ITEM_ADVANCED_INFO_SPECIAL_ADD", 4 ),
		GetGameWordText( "ITEM_ADVANCED_INFO_SPECIAL_ADD", 5 ),
		GetGameWordText( "ITEM_ADVANCED_INFO_SPECIAL_ADD", 6 ),
		GetGameWordText( "PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", 22 ),
		GetGameWordText( "PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", 23 ),
		"-",
	}
	
	for i = 1, g_nCostumeStat_RATIOCOUNT, 1 do
	
		local strInst = "ratio" .. tostring( i )
		SetString( WIDGET_ID_COSTUMESTAT, "text", tbStr[ i ], strInst .. ".labelTitle" )
		
		local strParam = CreateParam( i )
		SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_INPUT, "Ev_CostumeStat_InputRatio", 
			"", strInst .. ".input", strParam )
		SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_FOCUS_OUT, "Ev_CostumeStat_InputRatio",
			"", strInst .. ".input", strParam )
	
	end
	
	-- 가산 효과
	tbStr = 
	{
		GetGameWordText( "CHARACTER_STATUS_STATIC", 9 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 10 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 2 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 3 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 6 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 0 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 4 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 2 ),
		GetGameWordText( "CHARACTER_STAT_STATIC", 1 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 11 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 5 ),
		GetGameWordText( "CHARACTER_STATUS_STATIC", 4 ),
		"-",
		"-",
		"-",
	}
	
	for i = 1, g_nCostumeStat_ADDONCOUNT, 1 do
	
		local strInst = "addon" .. tostring( i )
		SetString( WIDGET_ID_COSTUMESTAT, "text", tbStr[ i ], strInst .. ".labelTitle" )
		
		local strParam = CreateParam( i )
		SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_INPUT, "Ev_CostumeStat_InputAddOn", 
			"", strInst .. ".input", strParam )
		SetEvent( WIDGET_ID_COSTUMESTAT, EVENT_FOCUS_OUT, "Ev_CostumeStat_InputAddOn",
			"", strInst .. ".input", strParam )
	
	end

end

-- 오픈
function OpenCostumeStat()

	-- 부위 텍스트
	local str = HTMLFontColor( GetGameWordText( "ITEM_INFO_SET_PARTS", g_CostumeStat:GetSuit() ), HTML_TEXT_COLOR_ORANGE )
	SetString( WIDGET_ID_COSTUMESTAT, "htmlText", str, "part.labelValue" )
	
	-- 초기화
	CostumeStat_Reset()
	
	-- 인벤 아이템 하이라이트
	local nIndex = Inventory_SlotPosToIndex( g_nCostumeStat_InvenItemPosX, g_nCostumeStat_InvenItemPosY )
	Inventory_SetVisibleHighlight( true, nIndex )

end

-- 클로즈
function CloseCostumeStat()

	Inventory_SetVisibleHighlight( false, -1 )

end

-- 데이터 리셋
function CostumeStat_Reset()

	for value in pairs( g_tbConstumeStat_InvestPoint ) do
		g_CostumeStat:ResetStatPos( value )
	end

	-- 투자 스탯 포인트 초기화
	g_tbConstumeStat_InvestPoint = {}
	g_tbConstumeStat_InvestCount = {}
	
	g_CostumeStat:SetSubPoint( 0 )
	
	-- 보유 포인트
	CostumeStat_SetPoint( g_CostumeStat:GetHavePoint() )
	
	-- 타이틀 초기화
	CostumeStat_SetTitle( 0, g_nCOSTUMESTAT_MAXINVEST_NORMAL, 1, "normalTitle" )
	CostumeStat_SetTitle( 0, g_nCOSTUMESTAT_MAXINVEST_VAR, 2, "ratioTitle" )
	CostumeStat_SetTitle( 0, g_nCOSTUMESTAT_MAXINVEST_ADDED, 3, "addonTitle" )
	
	-- 투자 포인트 초기화
	CostumeStat_SetInvestPoint( 0, "normalTitle" )
	CostumeStat_SetInvestPoint( 0, "ratioTitle" )
	CostumeStat_SetInvestPoint( 0, "addonTitle" )
	
	-- 변화값, 텍스트 인풋, 투자한 포인트 초기화 ----------------------------------------------------
	for i = 1, g_nCostumeStat_NORMALCOUNT, 1 do
	
		local strInst = "normal" .. tostring( i )
		SetString( WIDGET_ID_COSTUMESTAT, "text", "", strInst .. ".input" )
		
		local nStatType = CostumeStat_GetNormalStatType( i )
		CostumeStat_SetChangeValue( nStatType, 0, strInst )
		CostumeStat_SetMaxPerInvestPoint( nStatType, 0, strInst )

	end
	
	for i = 1, g_nCostumeStat_RATIOCOUNT, 1 do
	
		local strInst = "ratio" .. tostring( i )
		SetString( WIDGET_ID_COSTUMESTAT, "text", "", strInst .. ".input" )
		
		local nStatType = ConstumeStat_GetRatioStatType( i )
		CostumeStat_SetChangeValue( nStatType, 0, strInst )
		CostumeStat_SetMaxPerInvestPoint( nStatType, 0, strInst )
		
	end
	
	for i = 1, g_nCostumeStat_ADDONCOUNT, 1 do
	
		local strInst = "addon" .. tostring( i )
		SetString( WIDGET_ID_COSTUMESTAT, "text", "", strInst .. ".input" )
		
		local nStatType = CostumeStat_GetAddonStatType( i )
		CostumeStat_SetChangeValue( nStatType, 0, strInst )
		CostumeStat_SetMaxPerInvestPoint( nStatType, 0, strInst )
		
	end
	--------------------------------------------------------------------------------------------

end

-- 일반 효과 스탯 타입
function CostumeStat_GetNormalStatType( nIndex )

	return nIndex

end

-- 변화율 효과 스탯 타입
function ConstumeStat_GetRatioStatType( nIndex )

	return nIndex + CS_ADD_EMPTY3

end

-- 가산 효과 스탯 타입
function CostumeStat_GetAddonStatType( nIndex )

	return nIndex + CS_EMPTY

end

-- 보유 포인트 설정
function CostumeStat_SetPoint( nPoint )

	local str = HTMLFontColor( tostring( nPoint ), HTML_TEXT_COLOR_PALEGREEN )
	SetString( WIDGET_ID_COSTUMESTAT, "htmlText", str, "point.labelValue" )

end

-- 효과 타이틀 설정
function CostumeStat_SetTitle( nCount, nMaxCount, nXmlIndex, strInst )

	local str = string.format( "%s ( %d / %d )", GetGameWordText( "COSTUME_STAT_ASSIGN", nXmlIndex ), 
		nCount, nMaxCount )
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, strInst .. ".labelTitle" )

end

-- 변화 값 설정
function CostumeStat_SetChangeValue( nStatType, value, strInst )

	local str = ""
	local nColor = TEXT_COLOR_WHITE
	local bColor = g_CostumeStat:IsInvestEnable( nStatType ) and 0 < g_CostumeStat:GetMaxStatPoint( nStatType )
	
	if 0 < value then
	
		str = "+"
		nColor = TEXT_COLOR_ORANGE
		
	elseif 0 > value then
	
		nColor = TEXT_COLOR_GRAY
		
	end
	
	-- 퍼센티지로 표기
	if 	nStatType == CS_EXPMULTIPLE or
		nStatType == CS_R_HP_INC or
		nStatType == CS_R_MP_INC or
		nStatType == CS_R_SP_INC or
		nStatType == CS_R_HMSP_INC or
		nStatType == CS_R_MOVESPEED or
		nStatType == CS_R_ATTACKSPEED or
		nStatType == CS_R_CRITICALHIT or
		nStatType == CS_R_STRIKEHIT then
		
		value = value * 100
		
		str = str .. string.format( "%.2f%%", value )
	
	-- 공란
	elseif 	nStatType == CS_EMPTY or
			nStatType == CS_ADD_EMPTY1 or
			nStatType == CS_ADD_EMPTY2 or
			nStatType == CS_ADD_EMPTY3 or
			nStatType == CS_R_EMPTY then
			
		str = "-"
		bColor = false
		
	-- 그 외 정수형
	else
	
		str = str .. string.format( "%d", value )
	
	end
	
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, strInst .. ".labelValue" )
	
	-- 폰트 색 변경
	if true == bColor then
	
		SetTextFormat( WIDGET_ID_COSTUMESTAT, nColor, false, 0, 0, strInst .. ".labelTitle" )
		SetTextFormat( WIDGET_ID_COSTUMESTAT, nColor, false, 0, 0, strInst .. ".labelValue" )
		
	end

end

-- 투자 포인트 값 설정
function CostumeStat_SetInvestPoint( nValue, strInst )

	local str = ""
	local nColor = TEXT_COLOR_WHITE
	
	if 0 < nValue then
	
		str = "+"
		nColor = TEXT_COLOR_PALEGREEN
	
	elseif 0 > nValue then
	
		nColor = TEXT_COLOR_RED
	
	end

	str = str .. string.format( "%d", nValue )
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, strInst .. ".labelValue" )
	
	-- 폰트 색 변경
	SetTextFormat( WIDGET_ID_COSTUMESTAT, nColor, false, 0, 0, strInst .. ".labelValue" )

end

-- 투자/최대 스탯 포인트 설정
function CostumeStat_SetMaxPerInvestPoint( nStatType, nValue, strInst )

	local nMax = g_CostumeStat:GetMaxStatPoint( nStatType )
	local str = ""
	local bColor = g_CostumeStat:IsInvestEnable( nStatType ) and 0 < nMax
	
	-- 공란
	if 	nStatType == CS_EMPTY or
		nStatType == CS_ADD_EMPTY1 or
		nStatType == CS_ADD_EMPTY2 or
		nStatType == CS_ADD_EMPTY3 or
		nStatType == CS_R_EMPTY then
		
		str = "-/-"
		bColor = false
		
		-- Disable
		CostumeStat_SetEnableInvest( false, strInst )

	else

		-- 사용 가능한지 확인
		local bEnable = g_CostumeStat:IsInvestEnable( nStatType ) and 0 < nMax
		CostumeStat_SetEnableInvest( bEnable, strInst )
	
		str = string.format( "%d/%d", nValue, nMax )
	
	end
	
	SetString( WIDGET_ID_COSTUMESTAT, "text", str, strInst .. ".labelPoint" )
	
	-- 폰트 컬러 변겅
	if true == bColor then
	
		local nColor = TEXT_COLOR_WHITE
		
		if 0 < nValue then
			nColor = TEXT_COLOR_ORANGE
		elseif 0 > nValue then
			nColor = TEXT_COLOR_GRAY
		end
		SetTextFormat( WIDGET_ID_COSTUMESTAT, nColor, false, 0, 0, strInst .. ".labelPoint" )
	
	end

end

-- 투자 가능 여부 설정
function CostumeStat_SetEnableInvest( bEnable, strInst )

	SetBool( WIDGET_ID_COSTUMESTAT, "enabled", bEnable, strInst .. ".labelTitle" )
	SetBool( WIDGET_ID_COSTUMESTAT, "enabled", bEnable, strInst .. ".labelValue" )
	SetBool( WIDGET_ID_COSTUMESTAT, "enabled", bEnable, strInst .. ".input" )
	SetBool( WIDGET_ID_COSTUMESTAT, "enabled", bEnable, strInst .. ".labelPoint" )

end

-- 포인트 투자
function CostumeStat_InvestPoint( nStatType, nValue )

	if 0 == nValue then
	
		g_tbConstumeStat_InvestPoint[ nStatType ] = nil
		g_CostumeStat:ResetStatPos( nStatType )
	
	else 
	
		g_tbConstumeStat_InvestPoint[ nStatType ] = nValue
		g_CostumeStat:SetNumeric( nStatType, nValue )
	
	end
	
	-- 투자 개수, 투자 총합
	local tbInvestCount =
	{
		{ 0, 0 },	-- 일반
		{ 0, 0 },	-- 가산
		{ 0, 0 },	-- 변화율
	}
	
	local nTotal = 0
	for value in pairs( g_tbConstumeStat_InvestPoint ) do
	
		-- 일반 투자 개수
		if value <= CS_EMPTY then
		
			tbInvestCount[ 1 ][ 1 ] = tbInvestCount[ 1 ][ 1 ] + 1
			tbInvestCount[ 1 ][ 2 ] = tbInvestCount[ 1 ][ 2 ] + g_tbConstumeStat_InvestPoint[ value ]
		
		-- 가산 투자 개수
		elseif value <= CS_ADD_EMPTY3 then
		
			tbInvestCount[ 2 ][ 1 ] = tbInvestCount[ 2 ][ 1 ] + 1
			tbInvestCount[ 2 ][ 2 ] = tbInvestCount[ 2 ][ 2 ] + g_tbConstumeStat_InvestPoint[ value ]
		
		-- 변화율 투자 개수
		elseif value <= CS_R_EMPTY then
		
			tbInvestCount[ 3 ][ 1 ] = tbInvestCount[ 3 ][ 1 ] + 1
			tbInvestCount[ 3 ][ 2 ] = tbInvestCount[ 3 ][ 2 ] + g_tbConstumeStat_InvestPoint[ value ]
		
		end
	
		nTotal = nTotal + g_tbConstumeStat_InvestPoint[ value ]
		
	end
	
	-- 전체 투자한 포인트 설정
	g_CostumeStat:SetSubPoint( nTotal )
	
	-- 보유 포인트 변경
	CostumeStat_SetPoint( g_CostumeStat:GetRemainPoint() )
	
	-- 타이틀 업데이트
	CostumeStat_SetTitle( tbInvestCount[ 1 ][ 1 ], g_nCOSTUMESTAT_MAXINVEST_NORMAL, 1, "normalTitle" )
	CostumeStat_SetTitle( tbInvestCount[ 3 ][ 1 ], g_nCOSTUMESTAT_MAXINVEST_VAR, 2, "ratioTitle" )
	CostumeStat_SetTitle( tbInvestCount[ 2 ][ 1 ], g_nCOSTUMESTAT_MAXINVEST_ADDED, 3, "addonTitle" )
	
	-- 종류별 투자 개수 업데이트
	CostumeStat_SetInvestPoint( tbInvestCount[ 1 ][ 2 ], "normalTitle" )
	CostumeStat_SetInvestPoint( tbInvestCount[ 3 ][ 2 ], "ratioTitle" )
	CostumeStat_SetInvestPoint( tbInvestCount[ 2 ][ 2 ], "addonTitle" )
	
	g_tbConstumeStat_InvestCount = tbInvestCount

end

-- 비활성 체크
function CostumeStat_CheckEnable()

	-- 남은 포인트가 있으면 각 스탯 종류별로 최대 투자 개수 체크
	if 0 < g_CostumeStat:GetRemainPoint() then
	
		-- 일반 최대 투자 개수 체크
		if g_tbConstumeStat_InvestCount[ 1 ][ 1 ] >= g_nCOSTUMESTAT_MAXINVEST_NORMAL then
		
			for i = CS_EXPMULTIPLE, CS_RESIST, 1 do
			
				if nil == g_tbConstumeStat_InvestPoint[ i ] then
					CostumeStat_SetEnableInvest( false, "normal" .. tostring( i ) )
				end
			
			end
			
		else
		
			for i = CS_EXPMULTIPLE, CS_RESIST, 1 do
			
				local bEnabled = g_CostumeStat:IsInvestEnable( i ) and 0 < g_CostumeStat:GetMaxStatPoint( i )
				CostumeStat_SetEnableInvest( bEnabled, "normal" .. tostring( i ) )
				
			end
			
		end

		-- 변화율 최대 투자 개수 체크
		if g_tbConstumeStat_InvestCount[ 3 ][ 1 ] >= g_nCOSTUMESTAT_MAXINVEST_VAR then
		
			for i = CS_R_HP_INC, CS_R_STRIKEHIT, 1 do
			
				if nil == g_tbConstumeStat_InvestPoint[ i ] then
					CostumeStat_SetEnableInvest( false, "ratio" .. tostring( i - CS_R_HP_INC + 1 ) )
				end
			
			end
		
		else
		
			for i = CS_R_HP_INC, CS_R_STRIKEHIT, 1 do
				
				local bEnabled = g_CostumeStat:IsInvestEnable( i ) and 0 < g_CostumeStat:GetMaxStatPoint( i )
				CostumeStat_SetEnableInvest( bEnabled, "ratio" .. tostring( i - CS_R_HP_INC + 1 ) )
				
			end
		
		end
		
		-- 가산 최대 투자 개수 체크
		if g_tbConstumeStat_InvestCount[ 2 ][ 1 ] >= g_nCOSTUMESTAT_MAXINVEST_ADDED then
		
			for i = CS_ADD_HITRATE, CS_ADD_MELEE, 1 do
			
				if nil == g_tbConstumeStat_InvestPoint[ i ] then
					CostumeStat_SetEnableInvest( false, "addon" .. tostring( i - CS_ADD_HITRATE + 1 ) )
				end
			
			end
		
		else
		
			for i = CS_ADD_HITRATE, CS_ADD_MELEE, 1 do
			
				local bEnabled = g_CostumeStat:IsInvestEnable( i ) and 0 < g_CostumeStat:GetMaxStatPoint( i )
				CostumeStat_SetEnableInvest( bEnabled, "addon" .. tostring( i - CS_ADD_HITRATE + 1 ) )
				
			end
		
		end
	
	-- 포인트 모두 소진했으면 투자한 스탯 제외 전부 Disable
	else
	
		for i = CS_EXPMULTIPLE, CS_RESIST, 1 do
			
			if nil == g_tbConstumeStat_InvestPoint[ i ] then
				CostumeStat_SetEnableInvest( false, "normal" .. tostring( i ) )
			end
			
		end
		
		for i = CS_R_HP_INC, CS_R_STRIKEHIT, 1 do
		
			if nil == g_tbConstumeStat_InvestPoint[ i ] then
				CostumeStat_SetEnableInvest( false, "ratio" .. tostring( i - CS_R_HP_INC + 1 ) )
			end
		
		end
		
		for i = CS_ADD_HITRATE, CS_ADD_MELEE, 1 do
		
			if nil == g_tbConstumeStat_InvestPoint[ i ] then
				CostumeStat_SetEnableInvest( false, "addon" .. tostring( i - CS_ADD_HITRATE + 1 ) )
			end
		
		end
	
	end

end

-- 포인트 투자 업데이트
function CostumeStat_UpdateInvest( nStatType, nValue, strInst )

	-- 이전 투자 포인트
	local nPrevInvestValue = g_tbConstumeStat_InvestPoint[ nStatType ]
	if nil == nPrevInvestValue then
		nPrevInvestValue = 0
	end

	-- 현재 남은 포인트, 이전 투자했던 포인트 더하자
	local nRemainPt = g_CostumeStat:GetRemainPoint() + nPrevInvestValue
	-- 최대 스탯
	local nStatMaxPt = g_CostumeStat:GetMaxStatPoint( nStatType )
	
	-- 최대값 체크
	if nStatMaxPt < nValue then
		nValue = nStatMaxPt
	end
	
	-- 남은 포인트 체크
	if 0 > nRemainPt - nValue then
		nValue = nRemainPt
	end
	
	-- 인풋 텍스트 변경
	if 0 < nValue then
	
		SetString( WIDGET_ID_COSTUMESTAT, "text", tostring( nValue ), strInst .. ".input" )
	
	else
	
		SetString( WIDGET_ID_COSTUMESTAT, "text", "", strInst .. ".input" )
	
	end

	-- 투자 포인트 합산
	CostumeStat_InvestPoint( nStatType, nValue )
	-- 스탯 변화값
	CostumeStat_SetChangeValue( nStatType, nValue * g_CostumeStat:GetStatInc( nStatType ), strInst )
	-- 투자/최대 스탯 설정
	CostumeStat_SetMaxPerInvestPoint( nStatType, nValue, strInst )

	-- 비활성 체크
	CostumeStat_CheckEnable()

end

--------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 사용 확인
function CostumeStat_Use( nType, nInvenPosX, nInvenPosY )

	g_nCostumeStat_InvenItemPosX = nInvenPosX
	g_nCostumeStat_InvenItemPosY = nInvenPosY

	local str = ""
	
	if -1 == nType then
	
		OpenWidget( WIDGET_ID_COSTUMESTAT )
		return
	
	-- 기존 능력치 대체
	elseif 0 == nType then
	
		str = GetGameInternalText( "MODAL_REQ_COSTUME_STAT_APPLY_MSG4", 0 )
	
	-- 만료 후 적용
	elseif 1 == nType then
	
		str = GetGameInternalText( "MODAL_REQ_COSTUME_STAT_APPLY_MSG5", 0 )
	
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CostumeStat_UseOK, Event_MsgBox_Close )

end

-- 에러 출력
function CostumeStat_Error( nError )

	-- 적용가능한 아이템이 아닙니다.
	if 0 == nError then
	
		CF_PrintMsg_SystemMsgBox( GetGameInternalText( "ERROR_COSTUME_MSG5", 0 ), TEXT_COLOR_RED )
	
	end

end

-- 능력치 해제
function CostumeStat_Release( wPosX, wPosY, wHoldPosX, wHoldPosY )

	g_nCostumeStat_InvenItemPosX = wPosX
	g_nCostumeStat_InvenItemPosY = wPosY
	g_nCostumeStat_HoldItemPosX = wHoldPosX
	g_nCostumeStat_HoldItemPosY = wHoldPosY
	
	local str = GetGameInternalText( "MODAL_REQ_COSTUME_STAT_RELEASE_MSG", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CostumeStat_ReleaseOK, Event_MsgBox_Close )

end

--------------------------------------------------------------------------------------------------
-- 이벤트

-- 사용 확인 OK
function Ev_CostumeStat_UseOK()

	Event_MsgBox_Close()
	OpenWidget( WIDGET_ID_COSTUMESTAT )

end

-- 해제서 사용 OK
function Ev_CostumeStat_ReleaseOK()

	Event_MsgBox_Close()

	g_Msg:ReqCostumeStatRelease( 
		g_nCostumeStat_InvenItemPosX, g_nCostumeStat_InvenItemPosY,
		g_nCostumeStat_HoldItemPosX, g_nCostumeStat_HoldItemPosY )

end

-- 취소
function Ev_CostumeStat_Cancel()

	CloseWidget( WIDGET_ID_COSTUMESTAT )

end

-- 초기화
function Ev_CostumeStat_Reset()

	CostumeStat_Reset()

end

-- 적용
function Ev_CostumeStat_Apply()

	local str = ""

	local nRemainPt = g_CostumeStat:GetRemainPoint()
	
	-- 선택한 옵션 없음.
	if nRemainPt == g_CostumeStat:GetHavePoint() then
	
		str = GetGameInternalText( "MODAL_REQ_COSTUME_STAT_APPLY_MSG3", 0 )
		
		OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close, Event_MsgBox_Close )
		
		return
	
	-- 포인트 남아있음.
	elseif 0 < nRemainPt then
	
		str = GetGameInternalText( "MODAL_REQ_COSTUME_STAT_APPLY_MSG2", 0 )
	
	-- 이대로 적용?
	else
	
		str = GetGameInternalText( "MODAL_REQ_COSTUME_STAT_APPLY_MSG1", 0 )
	
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CostumeStat_ApplyOK, Event_MsgBox_Close )

end

-- 적용 확인
function Ev_CostumeStat_ApplyOK()

	Event_MsgBox_Close()
	
	-- 서버에 적용 요청 
	g_Msg:ReqCostumeStatCToS()
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_COSTUMESTAT )

end

-- 일반 효과 입력
function Ev_CostumeStat_InputNormal( strIndex )

	local strInst = "normal" .. strIndex
	
	local nValue = tonumber( GetString( WIDGET_ID_COSTUMESTAT, "text", strInst .. ".input" ) )
	if nil == nValue then
		nValue = 0
	end
	
	CostumeStat_UpdateInvest( CostumeStat_GetNormalStatType( tonumber( strIndex ) ), nValue, strInst )

end

-- 변화율 효과 입력
function Ev_CostumeStat_InputRatio( strIndex )

	local strInst = "ratio" .. strIndex
	
	local nValue = tonumber( GetString( WIDGET_ID_COSTUMESTAT, "text", strInst .. ".input" ) )
	if nil == nValue then
		nValue = 0
	end
	
	CostumeStat_UpdateInvest( ConstumeStat_GetRatioStatType( tonumber( strIndex ) ), nValue, strInst )

end

-- 가산 효과 입력
function Ev_CostumeStat_InputAddOn( strIndex )

	local strInst = "addon" .. strIndex
	
	local nValue = tonumber( GetString( WIDGET_ID_COSTUMESTAT, "text", strInst .. ".input" ) )
	if nil == nValue then
		nValue = 0
	end
	
	CostumeStat_UpdateInvest( CostumeStat_GetAddonStatType( tonumber( strIndex ) ), nValue, strInst )

end