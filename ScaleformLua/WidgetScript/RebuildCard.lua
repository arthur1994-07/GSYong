
MAX_COUNT_REUILD_STATS = 6				-- 개조 스탯의 최대 개수 ( UI에 존재하는 체크박스의 개수와 같다 );
MAX_CHECKBOXCOUNT= 6
MAX_RANDOM_OPTION = 5					-- 랜덤 옵션 최대 개수

-- 개조 아이템 타입;
EMTYPE_ITEM_REMODEL_NONE			= 0
EMTYPE_ITEM_REMODEL_RANDOMOPTION	= 1
EMTYPE_ITEM_REMODEL_BASICSTATS		= 2
EMTYPE_ITEM_REMODEL_ADDSTATS		= 3
EMTYPE_ITEM_REMODEL_ITEMSKILL		= 4

g_nRebuildCard_ItemType = EMTYPE_ITEM_REMODEL_NONE

g_RebuildCard_bUseStemp = false			-- 인장카드 사용 유무(UseFeatures)
g_RebuildCard_bIsStemp = false			-- 인장카드 슬롯에 등록 유무

g_tbRebuildCard_ItemInfo = nil			-- 개조 대상 아이템 정보
g_tbRebuildCard_CacheItemInfo = nil		-- 개조시 아이템 정보
g_strRebuildCart_StempName = ""			-- 인장 카드 이름
g_RebuildCard_StempFixNum = 0			-- 인장 카드 옵션 고정 개수
g_RebuildCard_tbCheckOptionIndex = {}	-- 인장 카드 체크 옵션 인덱스

g_nRebuildCard_InvenPosX = -1			-- 개조 아이템 인벤 위치 X
g_nRebuildCard_InvenPosY = -1			-- 개조 아이템 인벤 위치 Y

-- 인장카드 ID
g_nRebuildCard_StampMID = -1
g_nRebuildCard_StampSID = -1

g_bRebuildCard_IsOpen = false
g_bRebuildType_KEEPORIGIN = false		-- 변경전 유지 시킬것인가?, 해당데이터는 초기데값이 서버와 동기화가 맞아야한다;

g_tbDisplaySize = {}
g_tbDisplayCacheSize = {}
g_nTooltipSize = 0
g_nCacheTooltipSize = 0

function IsRebuildCardOpen()

	return g_bRebuildCard_IsOpen

end

-- 개조카드 로드 완료
function LoadRebuildCard()
	
	-- 슬롯 바인딩
	SetString( WIDGET_ID_REBUILD_CARD, "binding", SLOT_REBUILD_CARD_ITEM_ID, "slotItem" )
	SetString( WIDGET_ID_REBUILD_CARD, "binding", SLOT_REBUILD_CARD_STEMP_ID, "slotStemp" )
	
	-- 툴팁 텍스트
	SetString( WIDGET_ID_REBUILD_CARD, "TooltipText", 
		GetGameInternalText( "ITEM_REBUILD_ITEM_SLOT_TOOLTIP", 0 ), "slotItem" )
	SetString( WIDGET_ID_REBUILD_CARD, "TooltipText", 
		GetGameInternalText( "ITEM_REBUILD_STAMP_SLOT_TOOLTIP", 0 ), "slotStemp" )
	
	-- 타이틀 텍스트
	SetString( WIDGET_ID_REBUILD_CARD, "title", GetGameInternalText( "ITEMREBUILD_WINDOW_TITLE", 0 ), "" )
	-- 개조 버튼
	SetString( WIDGET_ID_REBUILD_CARD, "label", GetGameInternalText( "ITEM_REBUILD_OK", 0 ), "btnOK" )
	-- 닫기 버튼
	SetString( WIDGET_ID_REBUILD_CARD, "label", GetGameInternalText( "ITEM_REBUILD_CANCEL", 0 ), "btnCancel" )
	-- 변경 전 유지 텍스트
	SetString( WIDGET_ID_REBUILD_CARD, "htmlText", GetGameInternalText( "ITEM_REBUILD_PRESERVE_BEFORE_CHANGE", 0 ), "Preserve_before_change" )
	
	-- 체크박스 (옵션 출력)
	for i = 1, MAX_COUNT_REUILD_STATS, 1 do
	
		SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, 
			"Event_RebuildCard_CheckBox", "","mcStampCheck.checkBox"..tostring( i ) )
		
		SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, 
			"Event_RebuildCard_CheckBox", "","mcStampCheck_addon.checkBox"..tostring( i ) )
			
		SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, 
			"Event_RebuildCard_CheckBox", "","mcStampCheck_basic.checkBox"..tostring( i ) )
	end
	
	SetEvent ( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, "Event_RebuildCard_ChangeRebuildType", "", "cbKeepOrigin" )
	
	-- 개조 버튼
	SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, "Event_RebuildCard_btnOK", "", "btnOK" )
	-- 닫기 버튼
	SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, "Event_RebuildCard_btnCancel", "", "btnCancel" )
	-- UI 종료
	SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_WIDGET_CLOSE, "CloseRebuildCard", "", "" )	
	-- 결과값 선택 & 롤백;
	SetEvent( WIDGET_ID_REBUILD_CARD, EVENT_BUTTON_CLICK, "Event_RebuildCard_btnSelectToCache", "", "btnSelectToCache" )
end

-- 개조카드 열기
function OpenRebuildCard()

	-- 휴지통 열려있음 닫자
	if true == IsOpenWidget( WIDGET_ID_INVENTORY_TRASH ) then
		CloseWidget( WIDGET_ID_INVENTORY_TRASH )
	end
	
	-- 타이틀 텍스트
	SetString( WIDGET_ID_REBUILD_CARD, "title", GetGameInternalText( "ITEMREBUILD_WINDOW_TITLE", g_nRebuildCard_ItemType ), "" )

	g_bRebuildCard_IsOpen = true

	MAX_RANDOM_OPTION = g_RebuildCard:GetOptionMaxSize()

	--인장카드 적용 유무
	if true == g_RebuildCard_bUseStemp then
	
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "slotStemp" )
		
	else
	
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "slotStemp" )
		
	end

	RebuildCard_Init()
	
	-- 가격
	SetString( WIDGET_ID_REBUILD_CARD, "text", "0", "labelMoney" )
	
	-- 개조 버튼 visible off
	SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "btnOK" )
	SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "btnSelectToCache" )
	
	RebuildCard_UIPosition()

end

-- 개조 카드 닫기
function CloseRebuildCard()

	g_bRebuildCard_IsOpen = false
	
	g_RebuildCard_bUseStemp = false			-- 인장카드 사용 유무(UseFeatures)
	g_RebuildCard_bIsStemp = false			-- 인장카드 슬롯에 등록 유무

	g_nRebuildCard_StampMID = -1
	g_nRebuildCard_StampSID = -1
	
	g_Msg:ReqRebuildCloseWindow()
	
end

-- UI 위치 잡기
function RebuildCard_UIPosition()

	if false == IsOpenWidget( WIDGET_ID_INVENTORY ) then
		return
	end
	
	local nInvenX = GetInt( WIDGET_ID_INVENTORY, "x", "" )
	local nInvenY = GetInt( WIDGET_ID_INVENTORY, "y", "" )
	
	local nW = GetInt( WIDGET_ID_REBUILD_CARD, "width", "" )
	
	local nNewX = nInvenX - nW
	local nNewY = nInvenY
	
	if 0 > nNewX then
	
		local nInvenW = GetInt( WIDGET_ID_INVENTORY, "width", "" )
		nNewX = nInvenX + nInvenW
	
	end
	
	local nScreenW = GetScreenWidth()
	if nScreenW < nNewX + nW then
	
		nNewX = 0
	
	end
	
	SetInt( WIDGET_ID_REBUILD_CARD, "x", nNewX, "" )
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nNewY, "" )

end

-- UI 초기화
function RebuildCard_Init()

	g_RebuildCard_bIsStemp = false
	g_strRebuildCart_StempName = ""
	g_RebuildCard_StempFixNum = 0
	g_RebuildCard_tbCheckOptionIndex = {}
	
	MAX_RANDOM_OPTION = g_RebuildCard:GetOptionMaxSize()
	
	g_tbRebuildCard_ItemInfo = nil
	g_tbRebuildCard_CacheItemInfo = nil
	
	ResetSlot( SLOT_REBUILD_CARD_ITEM_ID )
	ResetSlot( SLOT_REBUILD_CARD_STEMP_ID )
	
	RebuildCard_Init_CheckBox()
	RebuildCard_UpdateInfo()
	
	g_bRebuildType_KEEPORIGIN = false
	SetBool( WIDGET_ID_REBUILD_CARD, "selected", g_bRebuildType_KEEPORIGIN, "cbKeepOrigin")
	
end

-- 업데이트
function RebuildCard_UpdateInfo()
	
	if false == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		return
	end

	-- 개조 비용
	RebuildCard_SetCost()
	-- 개조 설명 튤팁
	RebuildCard_SetCaption()
	-- 아이템 튤팁
	RebuildCard_DisplayTooltip()
	-- 개조 툴팁 태그
	RebuildCard_SetTag()
end

function RebuildCard_SetTag()
	if g_bRebuildType_KEEPORIGIN == true then
		SetString( WIDGET_ID_REBUILD_CARD, "text", GetGameWordText( "REBUILD_ITEMTAG_KEEPORIGIN", 0 ), "lbCurrent" )
		SetString( WIDGET_ID_REBUILD_CARD, "text", GetGameWordText( "REBUILD_ITEMTAG_KEEPORIGIN", 1 ), "lbCache" )
		SetString( WIDGET_ID_REBUILD_CARD, "label", GetGameWordText("REBUILD_BTN_REVERTTOCACHE", 0), "btnSelectToCache")
		
	else
		SetString( WIDGET_ID_REBUILD_CARD, "text", GetGameWordText( "REBUILD_ITEMTAG_APPLYRESULT", 0 ), "lbCurrent" )
		SetString( WIDGET_ID_REBUILD_CARD, "text", GetGameWordText( "REBUILD_ITEMTAG_APPLYRESULT", 1 ), "lbCache" )
		SetString( WIDGET_ID_REBUILD_CARD, "label", GetGameWordText("REBUILD_BTN_REVERTTOCACHE", 1), "btnSelectToCache")
		
	end
	
	if g_tbRebuildCard_CacheItemInfo == nil then
		SetBool( WIDGET_ID_REBUILD_CARD, "enabled", false, "btnSelectToCache" )
	else
		SetBool( WIDGET_ID_REBUILD_CARD, "enabled", true, "btnSelectToCache" )
	end
	
end

--개조 비용 레이블 설정
function RebuildCard_SetCost()

	local strCost = g_RebuildCard:GetRebuildCost()
	SetString( WIDGET_ID_REBUILD_CARD, "text", CurrencyFormat( strCost ), "labelMoney" )
	
end

-- 슬롯 상태별 설명 튤팁 변경
function RebuildCard_SetCaption()

	local strGuide = ""
	
	if nil ~= g_tbRebuildCard_ItemInfo then
	
		-- 인장 카드 사용중이라면 고정 옵션 선택 상태에 따라 설명 변경
		if true == g_RebuildCard_bIsStemp then
		
			local nCount = table.getn( g_RebuildCard_tbCheckOptionIndex )
		
			-- 모두 선택함
			if nCount == g_RebuildCard_StempFixNum then
			
				strGuide = GetGameInternalText( "ITEM_REBUILD_USE_ITEM_WITH_STAMP_CARD", 0 )
				
			-- 최대 고정 개수보다 덜 선택함
			else

				strGuide = string.format( GetGameInternalText( "ITEM_REBUILD_USE_ONLY_STAMP_CARD_NO_LIMIT_NEW", 0 ),
					g_RebuildCard_StempFixNum, g_RebuildCard_StempFixNum, g_strRebuildCart_StempName )
				
			end
			
		else
		
			strGuide = GetGameInternalText( "ITEM_REBUILD_USE_ITEM", 0 )
			
		end
		
	else
	
		strGuide = GetGameInternalText( "ITEM_REBUILD_USE_REBUILD", 0 )
		
	end
	
	SetString( WIDGET_ID_REBUILD_CARD, "text", strGuide, "labelCaption" )

end

function RebuildCard_GetDisplayTooltip(tbItemInfo)
	
	--임시 랜덤 옵션 선택 가능여부 확인용	
	local strTooltip = ""
	if nil ~= tbItemInfo then
	
		-- 경험치 배율
		strTooltip = AddNewLine( strTooltip, ItemToolTip_Ability_ExpMultiple( tbItemInfo["ExpMultiple"] ) )
		-- 저항
		strTooltip = AddNewLine( strTooltip, ItemToolTip_Ability_Resist( tbItemInfo ) )

	end	
	
	return strTooltip
	
end

-- 아이템 정보 텍스트 설정
function RebuildCard_DisplayTooltip()
	
	g_nTooltipSize = 0
	g_nCacheTooltipSize = 0
	
	-- 기본능력치
	RebuildCard_DisplayTooltip_BasicStats()
	
	--임시 랜덤 옵션 선택 가능여부 확인용	
	local strTooltip, strTooltip_Cache 
		= RebuildCard_GetDisplayTooltip(g_tbRebuildCard_ItemInfo)
		, RebuildCard_GetDisplayTooltip(g_tbRebuildCard_CacheItemInfo)
	
	local nOffSet = GetDisplayPosition(true, EMTYPE_ITEM_REMODEL_BASICSTATS)--GetInt( WIDGET_ID_REBUILD_CARD, "y", "mcStampCheck_basic" )
	SetString( WIDGET_ID_REBUILD_CARD, "htmlText", strTooltip, "labelTooltip" )
	SetString( WIDGET_ID_REBUILD_CARD, "htmlText", strTooltip_Cache, "labelTooltip_cache" )
	
	if strTooltip ~= "" then
		SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet + 5 + g_tbDisplaySize[EMTYPE_ITEM_REMODEL_BASICSTATS]-5, "labelTooltip" )
		g_nTooltipSize = GetInt( WIDGET_ID_REBUILD_CARD, "textHeight", "labelTooltip" ) + 5
	end
	
	if strTooltip_Cache ~= "" then
		SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet + 5 + g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_BASICSTATS]-5, "labelTooltip_cache" )
		g_nCacheTooltipSize = GetInt( WIDGET_ID_REBUILD_CARD, "textHeight", "labelTooltip_cache" ) + 5
	end

	
	RebuildCard_DisplayTooltip_Addon()
	RebuildCard_DisplayTooltip_RandomOption()
	RebuildCard_DisplayTooltip_ItemSkill()

end

function GetDisplayPosition(bOrigin, nTYPE)

	-- 1. Basic
	-- 2. ItemTooltip(개조돌리지못하는)
	-- 3. Addon
	-- 4. RandomOption
	-- 5. ItemSkill
	--[[
		* Basic, ItemTooltip은 계산하지않는다, 아래 계산시 걍 offset으로 침
		* UI 사이즈는 번호 순대로 결정이되야함
		* RandomOption 위치를 구하려면 Basic, ItemTooltip이 계산되어있어야한다
	EMTYPE_ITEM_REMODEL_RANDOMOPTION	= 1
	EMTYPE_ITEM_REMODEL_BASICSTATS		= 2
	EMTYPE_ITEM_REMODEL_ADDSTATS		= 3
	EMTYPE_ITEM_REMODEL_ITEMSKILL		= 4
		]]--
	local nOffSet  = 0
	local tbTarget = {}
	local nTooltipSize = 0
	if bOrigin == true then
		nOffSet = GetInt( WIDGET_ID_REBUILD_CARD, "y", "mcStampCheck_basic" )
		tbTarget = g_tbDisplaySize
		nTooltipSize = g_nTooltipSize
		
	else
		nOffSet = GetInt( WIDGET_ID_REBUILD_CARD, "y", "mcStampCheck_basic_cache" )
		tbTarget = g_tbDisplayCacheSize
		nTooltipSize = g_nCacheTooltipSize
		
	end
	
	if nTYPE == EMTYPE_ITEM_REMODEL_BASICSTATS then
		return nOffSet
	end
	
	nOffSet = nOffSet + nTooltipSize + tbTarget[EMTYPE_ITEM_REMODEL_BASICSTATS]
	--PrintDebug(tostring(bOrigin) .. " 1'   "..tostring(nOffSet))
	if nTYPE == EMTYPE_ITEM_REMODEL_ADDSTATS then
		return nOffSet
	end
	
	nOffSet = nOffSet + tbTarget[EMTYPE_ITEM_REMODEL_ADDSTATS]
	--PrintDebug(tostring(bOrigin) .. " 2'   "..tostring(nOffSet))
	if nTYPE == EMTYPE_ITEM_REMODEL_RANDOMOPTION then
		return nOffSet
	end
	
	nOffSet = nOffSet + tbTarget[EMTYPE_ITEM_REMODEL_RANDOMOPTION]
	--PrintDebug(tostring(bOrigin) .. " 3'   "..tostring(nOffSet))
	if nTYPE == EMTYPE_ITEM_REMODEL_ITEMSKILL then
		return nOffSet
	end
	
end

-- 기본 능력치 ( 공격력, 방어력 ) 정보 텍스트 설정;
function RebuildCard_DisplayTooltip_BasicStats()
	
	g_tbDisplaySize[EMTYPE_ITEM_REMODEL_BASICSTATS] = 0
	g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_BASICSTATS] = 0
	
	if nil == g_tbRebuildCard_ItemInfo then
		return
	end
	
	local nVisibleCount = 0
	-- 공격력-방어력-명중률-회피율-소모SP
	local strDamage, strDefence, strHitRate, strAvoidRate, strCostSp
	 = ItemToolTip_Ability_Damage( g_tbRebuildCard_ItemInfo["Damage"] )
	 , ItemToolTip_Ability_Defence( g_tbRebuildCard_ItemInfo["Defence"] )
	 , ItemToolTip_Ability_HitRate( g_tbRebuildCard_ItemInfo["HitRate"] )
	 , ItemToolTip_Ability_AvoidRate( g_tbRebuildCard_ItemInfo["AvoidRate"] )
	 , ItemToolTip_Ability_CostSp( g_tbRebuildCard_ItemInfo["CostSp"] )
	
	local tbBasicString = {}
	table.insert(tbBasicString, strDamage)
	table.insert(tbBasicString, strDefence)
	table.insert(tbBasicString, strHitRate)
	table.insert(tbBasicString, strAvoidRate)
	table.insert(tbBasicString, strCostSp)
	for i = 1, 5, 1 do
		if tbBasicString[i] ~= "" then
			nVisibleCount = nVisibleCount + 1
			SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "mcStampCheck_basic.labelOption"..tostring( nVisibleCount ) )
			SetString( WIDGET_ID_REBUILD_CARD, "htmlText", tbBasicString[i], "mcStampCheck_basic.labelOption"..tostring( nVisibleCount ) )
		end
		
	end
	
	for i = nVisibleCount+1, 5, 1 do
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_basic.labelOption"..tostring( i ) )
	end

	if nVisibleCount ~= 0 then
		-- 22 is label size
		local nY = GetInt( WIDGET_ID_REBUILD_CARD, "y", "mcStampCheck_basic.labelOption"..tostring(nVisibleCount+1) )
		g_tbDisplaySize[EMTYPE_ITEM_REMODEL_BASICSTATS] = nY
		
	end

	-- 캐쉬
	nVisibleCount = 0
	if g_tbRebuildCard_CacheItemInfo ~= nil then
		strDamage	 = ItemToolTip_Ability_Damage( g_tbRebuildCard_CacheItemInfo["Damage"] )
		strDefence	 = ItemToolTip_Ability_Defence( g_tbRebuildCard_CacheItemInfo["Defence"] )
		strHitRate	 = ItemToolTip_Ability_HitRate( g_tbRebuildCard_CacheItemInfo["HitRate"] )
		strAvoidRate = ItemToolTip_Ability_AvoidRate( g_tbRebuildCard_CacheItemInfo["AvoidRate"] )
		strCostSp	 = ItemToolTip_Ability_CostSp( g_tbRebuildCard_CacheItemInfo["CostSp"] )
		local tbCacheString = {}
		table.insert(tbCacheString, strDamage)
		table.insert(tbCacheString, strDefence)
		table.insert(tbCacheString, strHitRate)
		table.insert(tbCacheString, strAvoidRate)
		table.insert(tbCacheString, strCostSp)
		for i = 1, 5, 1 do
			if tbCacheString[i] ~= "" then
				nVisibleCount = nVisibleCount + 1
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "mcStampCheck_basic_cache.labelOption"..tostring(nVisibleCount) )
				SetString( WIDGET_ID_REBUILD_CARD, "htmlText", tbCacheString[i], "mcStampCheck_basic_cache.labelOption"..tostring(nVisibleCount) )
			end
			
		end
		
		for i = nVisibleCount+1, 5, 1 do
			SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_basic_cache.labelOption"..tostring( i ) )
		end
		
		-- 22 is label size
		if nVisibleCount ~= 0 then
			local nY = GetInt( WIDGET_ID_REBUILD_CARD, "y", "mcStampCheck_basic_cache.labelOption"..tostring(nVisibleCount+1) )
			g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_BASICSTATS] = nY
			
		end
		
	end
end

function RebuildCard_DisplayTooltip_Addon()
	
	g_tbDisplaySize[EMTYPE_ITEM_REMODEL_ADDSTATS] = 0
	g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_ADDSTATS] = 0
	
	-- 위치 잡기
	local nOffSet = GetDisplayPosition(true, EMTYPE_ITEM_REMODEL_ADDSTATS)
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet, "mcStampCheck_addon" )
	
	nOffSet = GetDisplayPosition(false, EMTYPE_ITEM_REMODEL_ADDSTATS)
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet, "mcStampCheck_addon_cache" )
	
	if nil == g_tbRebuildCard_ItemInfo then
		return
	end
	
	local tbLabelName = { "mcStampCheck_addon", "mcStampCheck_addon_cache" }
	for nType = 1, 2, 1 do	
		local tbAddOpt = {}
		if nType == 1 then
			tbAddOpt = g_tbRebuildCard_ItemInfo["AddOpt"]
		else
			if g_tbRebuildCard_CacheItemInfo == nil then
				continue
			end
			tbAddOpt = g_tbRebuildCard_CacheItemInfo["AddOpt"]
		end
		
		local strLabel = tbLabelName[nType]
		local nSize = table.getn( tbAddOpt )
		local nSetCount = 0
		
		for i = 1, nSize, 1 do
			if nil ~= tbAddOpt[ i ] then
				nSetCount = nSetCount + 1
				local tbAddon = tbAddOpt[ i ]
				
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, strLabel..".labelOption"..tostring( i ) )
				-- INT
				if tbAddon[1] == true then
				
					local nValue = tbAddon[2]
				
					local str = ""
					if nValue >= 0 then
						str = string.format( "%s : +%d", GetGameWordText( "ITEM_ADDON_INFO", tbAddon[3] ), nValue )
					elseif nValue < 0 then
						str = string.format( "%s : %d", GetGameWordText( "ITEM_ADDON_INFO", tbAddon[3] ), nValue )
					end
					
					SetString( WIDGET_ID_REBUILD_CARD, "htmlText"
						, HTMLFontColor( str, HTML_TEXT_COLOR_GREEN )
						, strLabel..".labelOption" .. tostring( nSetCount ))
				-- FLOAT
				else
				
					local str = ""
					
					-- 소수점 아래자리가 많이 나올 수 있으므로 잘라준다;
					local nCount = 0
					local fValue = tonumber( tbAddon[ 2 ] ) % 1
					while fValue ~= 0 do
		
						nCount = nCount + 1
						if nCount == 2 then
							break
						end
		
						fValue = (fValue * 10) % 1
		
					end

					if fValue >= 0 then
					
						if tbAddon[ 4 ] == true then	
							str = string.format( "%s : +%." .. nCount .. "f%%"
								, GetGameWordText( "ITEM_ADDON_INFO", tbAddon[ 3 ] ),	tbAddon[2] )
						else
							str = string.format( "%s : +%." .. nCount .. "f"
								, GetGameWordText( "ITEM_ADDON_INFO", tbAddon[ 3 ] ),	tbAddon[2] )
						end
					
					elseif fValue < 0 then
					
						if tbAddon[ 4 ] == true then	
							str = string.format( "%s : %." .. nCount .. "f%%"
								, GetGameWordText( "ITEM_ADDON_INFO", tbAddon[ 3 ] ),	tbAddon[2] )
						else
							str = string.format( "%s : %." .. nCount .. "f"
								, GetGameWordText( "ITEM_ADDON_INFO", tbAddon[ 3 ] ),	tbAddon[2] )
						end
						
					end
					
					
					SetString( WIDGET_ID_REBUILD_CARD, "htmlText"
						, HTMLFontColor( str, HTML_TEXT_COLOR_GREEN )
						, strLabel..".labelOption" .. tostring( nSetCount ))						
				end

			end
		end
		
		if nSetCount ~= 0 then
			local nY = 22 + GetInt( WIDGET_ID_REBUILD_CARD, "y", strLabel..".labelOption" .. tostring( nSetCount ))
			if nType == 1 then
				g_tbDisplaySize[EMTYPE_ITEM_REMODEL_ADDSTATS] = nY
			else
				g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_ADDSTATS] = nY
			end
			
		end
		
		for i = nSetCount+1, 5, 1 do
			SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, strLabel..".labelOption"..tostring( i ) )
		end
	end
	
end

-- 랜덤 옵션 정보 텍스트 설정
function RebuildCard_DisplayTooltip_RandomOption()

	g_tbDisplaySize[EMTYPE_ITEM_REMODEL_RANDOMOPTION] = 0
	g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_RANDOMOPTION] = 0

	-- 위치잡기	
	local nOffSet = GetDisplayPosition(true, EMTYPE_ITEM_REMODEL_RANDOMOPTION)
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet, "mcStampCheck" )
	nOffSet = GetDisplayPosition(false, EMTYPE_ITEM_REMODEL_RANDOMOPTION)
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet, "mcStampCheck_cache" )
	
	if nil == g_tbRebuildCard_ItemInfo then
		return
	end
	
	--정보 입력
	local tbRebuildRanOpt = g_RebuildCard:GetRebuild_RandomOpt()
	local tbKeyData = { "origin", "cache" }
	local tbLabelName = { "mcStampCheck", "mcStampCheck_cache" }
	for nType = 1, 2, 1 do
		local strKey = tbKeyData[nType]
		local strLabel = tbLabelName[nType]
		
		local nSetCount = 0
		for i = 1, MAX_COUNT_REUILD_STATS, 1 do
			local tbOption = tbRebuildRanOpt[ strKey ][i]
			local strCheckBox = strLabel..".checkBox" .. tostring( i )	
			
			if nil == tbOption then
				--옵션이 존재하지 않는 경우
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, strCheckBox )
				SetString( WIDGET_ID_REBUILD_CARD, "text", "", strLabel..".labelOption"..tostring( i ) )
			
			else
				local str = ""
				if tbOption[ "fValue" ] >= 0 then
					str = GetGameWordText( "ITEM_RANDOM_OPTION", tbOption[ "nType" ] ) .. " : +"
				elseif tbOption[ "fValue" ] < 0 then
					str = GetGameWordText( "ITEM_RANDOM_OPTION", tbOption[ "nType" ] ) .. " : "
				end
				
				local strValue 
				if true == tbOption[ "bPerRandOpt" ] then
					strValue = string.format( "%.2f", tbOption[ "fValue" ] )
				else
					strValue = tostring( tbOption[ "fValue" ] )
				end
			
				if 0 == ItemTooltip_GetRandomOptPrintType(tbOption[ "nType" ]) then
			
					str = str .. strValue .. "%"
					
				elseif 1 == ItemTooltip_GetRandomOptPrintType(tbOption[ "nType" ]) then
			
					str = str .. strValue
				
				end
			
				SetString( WIDGET_ID_REBUILD_CARD, "htmlText", 
					HTMLFontColor( str, HTML_TEXT_COLOR_CYAN ), strLabel..".labelOption" .. tostring( i ) )
				
				nSetCount = nSetCount + 1
			end
		end
		
		if nSetCount ~= 0 then
			-- 22 is label height
			local nY = 22 + GetInt( WIDGET_ID_REBUILD_CARD, "y", strLabel..".labelOption" .. tostring( nSetCount ))
			if nType == 1 then
				g_tbDisplaySize[EMTYPE_ITEM_REMODEL_RANDOMOPTION] = nY
			else
				g_tbDisplayCacheSize[EMTYPE_ITEM_REMODEL_RANDOMOPTION] = nY
			end
			
		end
		
	end
	
end

-- 아이템 스킬 정보 텍스트 설정;
function RebuildCard_DisplayTooltip_ItemSkill ()

	local nOffSet = GetDisplayPosition(true, EMTYPE_ITEM_REMODEL_ITEMSKILL)
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet, "labelItemLink" )
	nOffSet = GetDisplayPosition(false, EMTYPE_ITEM_REMODEL_ITEMSKILL)
	SetInt( WIDGET_ID_REBUILD_CARD, "y", nOffSet, "labelItemLink_cache" )

	if nil == g_tbRebuildCard_ItemInfo then
		return
	end
	
	local str = ItemToolTip_SkillLink(g_tbRebuildCard_ItemInfo)
	SetString( WIDGET_ID_REBUILD_CARD, "htmlText", str, "labelItemLink" )
	
	if g_tbRebuildCard_CacheItemInfo ~= nil then
		local str = ItemToolTip_SkillLink(g_tbRebuildCard_CacheItemInfo)
		SetString( WIDGET_ID_REBUILD_CARD, "htmlText", str, "labelItemLink_cache" )
	
	end
end

--체크박스 초기화
function RebuildCard_Init_CheckBox()

	for i = 1, MAX_CHECKBOXCOUNT, 1 do
	
		SetString( WIDGET_ID_REBUILD_CARD, "text", "", "mcStampCheck.labelOption"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck.checkBox"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck.checkBox"..tostring( i ) )
		
		SetString( WIDGET_ID_REBUILD_CARD, "text", "", "mcStampCheck_cache.labelOption"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck_cache.checkBox"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_cache.checkBox"..tostring( i ) )

		SetString( WIDGET_ID_REBUILD_CARD, "htmlText", "", "mcStampCheck_addon.labelOption"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck_addon.checkBox"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_addon.checkBox"..tostring( i ) )		
		
		SetString( WIDGET_ID_REBUILD_CARD, "text", "", "mcStampCheck_addon_cache.labelOption"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck_addon_cache.checkBox"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_addon_cache.checkBox"..tostring( i ) )		
		
		SetString( WIDGET_ID_REBUILD_CARD, "text", "", "mcStampCheck_basic.labelOption"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck_basic.checkBox"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_basic.checkBox"..tostring( i ) )		
		
		SetString( WIDGET_ID_REBUILD_CARD, "text", "", "mcStampCheck_basic_cache.labelOption"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck_basic_cache.checkBox"..tostring( i ) )
		SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_basic_cache.checkBox"..tostring( i ) )		
		
	end
	
	
	-- 인장카드를 셋팅하지않으면 체크박스 크기만큼 당겨준다
	-- 기본옵션
	local nCheckBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_basic.checkBox" )
	local nCacheBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_basic_cache.checkBox" )
	for i = 1, MAX_CHECKBOXCOUNT, 1 do
		SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX, "mcStampCheck_basic.labelOption"..tostring( i ) )
		SetInt( WIDGET_ID_REBUILD_CARD, "x", nCacheBoxX, "mcStampCheck_basic_cache.labelOption"..tostring( i ) )
	end
	
	-- 랜덤옵션
	nCheckBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck.checkBox" )
	nCacheBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_cache.checkBox" )
	for i = 1, MAX_CHECKBOXCOUNT, 1 do
		SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX, "mcStampCheck.labelOption"..tostring( i ) )
		SetInt( WIDGET_ID_REBUILD_CARD, "x", nCacheBoxX, "mcStampCheck_cache.labelOption"..tostring( i ) )
	end
	
	-- 가산효과
	nCheckBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_addon.checkBox" )
	nCacheBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_addon_cache.checkBox" )
	for i = 1, MAX_CHECKBOXCOUNT, 1 do
		SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX, "mcStampCheck_addon.labelOption"..tostring( i ) )
		SetInt( WIDGET_ID_REBUILD_CARD, "x", nCacheBoxX, "mcStampCheck_addon_cache.labelOption"..tostring( i ) )
	end
	
	-- 아이템스킬
	SetString( WIDGET_ID_REBUILD_CARD, "text", "", "labelItemLink" )
	SetString( WIDGET_ID_REBUILD_CARD, "text", "", "labelItemLink_cache" )
	
end

-- 체크박스 업데이트
function RebuildCard_UpdateCheck()
	
	if g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_RANDOMOPTION then 
		-- 최대 개수만큼 모두 체크 했으면 남은 체크 박스 visible off
		if g_RebuildCard_StempFixNum == table.getn( g_RebuildCard_tbCheckOptionIndex ) then
	
			for i = 1, MAX_COUNT_REUILD_STATS, 1 do		
				local bVisible = false			
				for value in pairs( g_RebuildCard_tbCheckOptionIndex ) do			
					if i == g_RebuildCard_tbCheckOptionIndex[ value ] then				
						bVisible = true
						break
				
					end
			
				end			
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", bVisible, "mcStampCheck.checkBox" .. tostring( i ) )
		
			end
		
		else
	
			for i = 1, MAX_COUNT_REUILD_STATS, 1 do
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "mcStampCheck.checkBox" .. tostring( i ) )
			end

		end
		
		-- 위치 보정
		local nCheckBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_addon.checkBox" )
		if g_RebuildCard_bIsStemp == true then
			for i = 1, MAX_CHECKBOXCOUNT, 1 do
				SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX + 20, "mcStampCheck.labelOption"..tostring( i ) )
			end
		
		else
			for i = 1, MAX_CHECKBOXCOUNT, 1 do
				SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX, "mcStampCheck.labelOption"..tostring( i ) )
			end
		
		end	
		
	-- 기본옵션
	elseif g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_BASICSTATS then 
		-- 최대 개수만큼 모두 체크 했으면 남은 체크 박스 visible off
		if g_RebuildCard_StempFixNum == table.getn( g_RebuildCard_tbCheckOptionIndex ) then
	
			for i = 1, MAX_CHECKBOXCOUNT, 1 do		
				local bVisible = false			
				for value in pairs( g_RebuildCard_tbCheckOptionIndex ) do			
					if i == g_RebuildCard_tbCheckOptionIndex[ value ] then				
						bVisible = true
						break
				
					end
			
				end			

				SetBool( WIDGET_ID_REBUILD_CARD, "visible", bVisible, "mcStampCheck_basic.checkBox" .. tostring( i ) )
		
			end
		
		else		
			local nCheckVisibleCount = 0
			local tbDamage = g_tbRebuildCard_ItemInfo["Damage"]
			-- 공격력 데이터가 없다면 출력하지 않음
			if tbDamage ~= nil then
				if 	0 ~= tbDamage[ "Min" ] or 0 ~= tbDamage[ "Max" ] or 0 ~= tbDamage[ "Extra" ] or
					0 ~= tbDamage[ "Extra" ] or 0 ~= tbDamage[ "RndOpt" ] then
					nCheckVisibleCount = nCheckVisibleCount + 1
				end
			end
			
			local tbDefence = g_tbRebuildCard_ItemInfo["Defence"]
			-- 방어력 데이터가 없다면 출력하지 않음
			if tbDefence ~= nil then
				if 0 == tbDefence["Def"] or 0 == tbDefence["RndOpt"] or 0 == tbDefence["Extra"] then
					nCheckVisibleCount = nCheckVisibleCount + 1
				end
			end
			
			-- 위치 보정 기준값
			local nCheckBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_basic.checkBox1" )
			for i = 1, nCheckVisibleCount, 1 do
				SetBool( WIDGET_ID_REBUILD_CARD444, "visible", true, "mcStampCheck_basic.checkBox" .. tostring( i ) )
				SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX + 20, "mcStampCheck_basic.labelOption" .. tostring( i ) )
				
			end
			
			for i = nCheckVisibleCount + 1, MAX_CHECKBOXCOUNT, 1 do
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", false, "mcStampCheck_basic.checkBox" .. tostring( i ) )
				SetBool( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX, "mcStampCheck_basic.labelOption" .. tostring( i ) )
			end
			
		end
	-- 가산효과
	elseif g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_ADDSTATS then 
		-- 최대 개수만큼 모두 체크 했으면 남은 체크 박스 visible off
		if g_RebuildCard_StempFixNum == table.getn( g_RebuildCard_tbCheckOptionIndex ) then
	
			for i = 1, MAX_CHECKBOXCOUNT, 1 do		
				local bVisible = false			
				for value in pairs( g_RebuildCard_tbCheckOptionIndex ) do			
					if i == g_RebuildCard_tbCheckOptionIndex[ value ] then				
						bVisible = true
						break
				
					end
			
				end			

				SetBool( WIDGET_ID_REBUILD_CARD, "visible", bVisible, "mcStampCheck_addon.checkBox" .. tostring( i ) )
		
			end
		
		else
	
			-- 가산 효과는 인덱스 사이즈만큼 던져주기때문에 받아서 유효한지 확인후 활성화한다;
			local tbAddOpt = g_tbRebuildCard_ItemInfo["AddOpt"]
			local nSize = table.getn( tbAddOpt )
			local nVaildCount = 0
			for i = 1, nSize, 1 do
				if nil ~= tbAddOpt[ i ] then
					nVaildCount = nVaildCount + 1
				end
			end
		
			for i = 1, nVaildCount, 1 do
				SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "mcStampCheck_addon.checkBox" .. tostring( i ) )
			end

		end
		
		-- 위치 보정
		local nCheckBoxX = GetInt ( WIDGET_ID_REBUILD_CARD, "x", "mcStampCheck_addon.checkBox" )
		if g_RebuildCard_bIsStemp == true then
			for i = 1, MAX_CHECKBOXCOUNT, 1 do
				SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX + 20, "mcStampCheck_addon.labelOption"..tostring( i ) )
			end
		
		else
			for i = 1, MAX_CHECKBOXCOUNT, 1 do
				SetInt( WIDGET_ID_REBUILD_CARD, "x", nCheckBoxX, "mcStampCheck_addon.labelOption"..tostring( i ) )
			end
		
		end	
	end
	
end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 개조 카드 아이템 사용
function RebuildCard_Open( bUseStemp, nRebuildCard_ItemType )

	g_RebuildCard_bUseStemp = bUseStemp
	g_nRebuildCard_ItemType = nRebuildCard_ItemType
	
	local IsOpen = IsOpenWidget( WIDGET_ID_REBUILD_CARD )
	if false == IsOpen then
		OpenWidget( WIDGET_ID_REBUILD_CARD )
	end
	
	return IsOpen
	
end

-- 개조 아이템 등록
function RebuildCard_RegistItem( wPosX, wPosY, tbItemInfo, nFixedOptionIndex )

	if false == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		return
	end
	
	-- 먼저 현재 등록된 아이템을 제거한다
	RebuildCard_RemoveItem()
	
	g_tbRebuildCard_ItemInfo = nil
	g_tbRebuildCard_CacheItemInfo = nil
	
	-- 새로운 아이템이 등록되면 인장카드 제거한다.
	if 	g_nRebuildCard_InvenPosX ~= wPosX or
		g_nRebuildCard_InvenPosY ~= wPosY then
		
		RebuildCard_RemoveStemp()
		
	end
	
	g_nRebuildCard_InvenPosX = wPosX
	g_nRebuildCard_InvenPosY = wPosY

	SetSlot_RebuildCard( SLOT_REBUILD_CARD_ITEM_ID, wPosX, wPosY )
	
	-- 개조 버튼 visible on
	SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "btnOK" )
	SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, "btnSelectToCache" )
	
	g_tbRebuildCard_ItemInfo = tbItemInfo
	
	if nil ~= nFixedOptionIndex then
	
		RebuildCard_Init_CheckBox()
		
		for i = 0, nFixedOptionIndex, 1 do
			table.insert( g_RebuildCard_tbCheckOptionIndex, i )
		end
	
		for i = 1, MAX_COUNT_REUILD_STATS, 1 do
			
			if i - 1 <= nFixedOptionIndex then
				SetBool( WIDGET_ID_REBUILD_CARD, "selected", true, "mcStampCheck.checkBox" .. tostring( i ) )
			end
			
		end
		
		if false == g_RebuildCard_bIsStemp then
			RebuildCard_UpdateCheck()
		else
			
			-- 인장카드 갱신
			g_RebuildCard_tbCheckOptionIndex = {}
			g_RebuildCard:UpdateFixedOption()
	
		end
	
	end
	
	--RebuildCard_UpdateInfo()
	
	-- 툴팁 텍스트 제거
	SetString( WIDGET_ID_REBUILD_CARD, "TooltipText", "", "slotItem" )
	
end

function RebuildCard_RegistCacheItem(tbCacheItemInfo)
	g_tbRebuildCard_CacheItemInfo = tbCacheItemInfo
	
end

-- 슬롯에서 아이템 제거
function RebuildCard_RemoveItem()

	if false == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		return
	end

	ResetSlot( SLOT_REBUILD_CARD_ITEM_ID )
	ResetSlot( SLOT_REBUILD_CARD_STEMP_ID )
	
	g_tbRebuildCard_ItemInfo = nil
	g_tbRebuildCard_CacheItemInfo = nil
	
	--체크박스 초기화
	RebuildCard_Init_CheckBox()
	RebuildCard_UpdateInfo()
	
	-- 다시 툴팁 텍스트 추가
	SetString( WIDGET_ID_REBUILD_CARD, "TooltipText", 
		GetGameInternalText( "ITEM_REBUILD_ITEM_SLOT_TOOLTIP", 0 ), "slotItem" )
	
end

-- 인장 아이템 등록
function RebuildCard_RegistStemp( wMID, wSID, strName, nFixNum )

	if false == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		return
	end
	
	if g_RebuildCard_StempFixNum ~= nFixNum then
		RebuildCard_Init_CheckBox()
	end
	
	g_nRebuildCard_StampMID = wMID
	g_nRebuildCard_StampSID = wSID
	
	local nCount = g_MyChar:GetInvenItemCount( wMID, wSID )
	SetSlot_Item( SLOT_REBUILD_CARD_STEMP_ID, wMID, wSID, nCount )
	
	g_RebuildCard_bIsStemp = true
	g_strRebuildCart_StempName = strName
	g_RebuildCard_StempFixNum = nFixNum
	g_RebuildCard_tbCheckOptionIndex = {}
	
	RebuildCard_UpdateInfo()
	Event_RebuildCard_CheckBox()
	
	-- 툴팁 텍스트 제거
	SetString( WIDGET_ID_REBUILD_CARD, "TooltipText", "", "slotStemp" )
	
end

-- 슬롯에서 인장 제거
function RebuildCard_RemoveStemp()

	if false == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		return
	end
	
	g_nRebuildCard_StampMID = -1
	g_nRebuildCard_StampSID = -1
	ResetSlot( SLOT_REBUILD_CARD_STEMP_ID )
	
	g_RebuildCard_bIsStemp = false
	g_strRebuildCart_StempName = ""
	g_RebuildCard_StempFixNum = 0
	g_RebuildCard_tbCheckOptionIndex = {}
	
	--체크박스 초기화
	RebuildCard_Init_CheckBox()
	RebuildCard_UpdateInfo()
	
	-- 다시 툴팁 텍스트 추가
	SetString( WIDGET_ID_REBUILD_CARD, "TooltipText", 
		GetGameInternalText( "ITEM_REBUILD_STAMP_SLOT_TOOLTIP", 0 ), "slotStemp" )
	
end

-- 업데이트
function RebuildCard_Update( tbItemInfo, tbCacheItemInfo, nFixedOptionIndex )
	
	if false == IsOpenWidget( WIDGET_ID_REBUILD_CARD ) then
		return
	end
	
	RebuildCard_RegistItem( g_nRebuildCard_InvenPosX, g_nRebuildCard_InvenPosY, tbItemInfo, nFixedOptionIndex )
	RebuildCard_RegistCacheItem( tbCacheItemInfo )
	RebuildCard_UpdateInfo()
	
	local nCount = g_MyChar:GetInvenItemCount( g_nRebuildCard_StampMID, g_nRebuildCard_StampSID )
	SetSlot_Item( SLOT_REBUILD_CARD_STEMP_ID, g_nRebuildCard_StampMID, g_nRebuildCard_StampSID, nCount )
	
end

-- 인장 체크 갱신
function RebuildCard_Update_FixdOption ( _nIndex )

	if -1 == _nIndex then
	
		Event_RebuildCard_CheckBox()
		
		return
		
	end

	local nUIIndex = _nIndex + 1
	table.insert( g_RebuildCard_tbCheckOptionIndex, nUIIndex )
	
	RebuildCard_UpdateCheck()
	RebuildCard_UpdateInfo()
	
	local strCheckName = ""
	if g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_RANDOMOPTION then
		strCheckName = "mcStampCheck.checkBox"
	elseif g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_BASICSTATS then
		strCheckName = "mcStampCheck_basic.checkBox"
	elseif g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_ADDSTATS then
		strCheckName = "mcStampCheck_addon.checkBox"
	end
	
	SetBool( WIDGET_ID_REBUILD_CARD, "visible", true, strCheckName .. tostring( nUIIndex ) )
	SetBool( WIDGET_ID_REBUILD_CARD, "selected", true, strCheckName .. tostring( nUIIndex ) )
	
end

-- 인장 체크 해제
function RebuildCard_StampUncheck()

	g_RebuildCard_tbCheckOptionIndex = {}
	
	for i = 1, MAX_COUNT_REUILD_STATS, 1 do
	
		SetBool( WIDGET_ID_REBUILD_CARD, "selected", false, "mcStampCheck.checkBox" .. tostring( i ) )
	
	end

end

-- 개조 확정 및 취소 팝업;
function RebuildCard_FixPopupUI()

	local str = GetGameInternalText( "TEXT_CONFIRM_UNLOCK_CHARACTER_SLOT_ITEM_MESSAGE", 0 )
		
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_OK_RebuildCard_FixPopupUI, Ev_GameMsg_Cancel_RebuildCard_FixPopupUI )
	
end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

function Event_RebuildCard_CheckBox()
	
	-- 고정 옵션 초기화
	g_RebuildCard:ResetFixedOption()
	
	g_RebuildCard_tbCheckOptionIndex = {}
	
	local strCheckName = ""
	if g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_RANDOMOPTION then
		strCheckName = "mcStampCheck.checkBox"
	elseif g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_BASICSTATS then
		strCheckName = "mcStampCheck_basic.checkBox"
	elseif g_nRebuildCard_ItemType == EMTYPE_ITEM_REMODEL_ADDSTATS then
		strCheckName = "mcStampCheck_addon.checkBox"
	end
		

	--현재 선택된 체크박스 확인
	for i = 1, MAX_COUNT_REUILD_STATS, 1 do
		--[[
		EMTYPE_ITEM_REMODEL_NONE			= 0
		EMTYPE_ITEM_REMODEL_RANDOMOPTION	= 1
		EMTYPE_ITEM_REMODEL_BASICSTATS		= 2
		EMTYPE_ITEM_REMODEL_ADDSTATS		= 3
		EMTYPE_ITEM_REMODEL_ITEMSKILL		= 4
		]]--
		--체크 된 체크박스 확인
		local bCheck = GetBool( WIDGET_ID_REBUILD_CARD, "selected", strCheckName .. tostring( i ) )
		if true == bCheck then
		
			table.insert( g_RebuildCard_tbCheckOptionIndex, i )
			g_RebuildCard:SetFixedOption( i - 1 )
			
		end
		
	end
	
	RebuildCard_UpdateCheck()
	RebuildCard_UpdateInfo()
	
end

--닫기 버튼
function Event_RebuildCard_btnCancel()

	CloseRebuildCard()
	CloseWidget( WIDGET_ID_REBUILD_CARD )
	
end

--개조 버튼
function Event_RebuildCard_btnOK()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "ITEM_REBUILD_OKQUESTION", 0 ), 
		Event_RebuildCard_btnOK_MsgBox_btnOk, Event_MsgBox_Close )
	
end

-- 개조 버튼 개조 확인 메시지 박스 OK
function Event_RebuildCard_btnOK_MsgBox_btnOk()

	Event_MsgBox_Close()
	
	-- 듀얼 인장카드 올린 상태에서 체크 없이 개조 성공하고 바로 체크 하나만해서 개조 시도하면 인장카드로 변경되어 개조된다.
	-- 체크 개수 맞는지 여기서 확인 한번 해본다.
	local nCheckCount = table.getn( g_RebuildCard_tbCheckOptionIndex )
	if 	0 ~= nCheckCount and
		g_RebuildCard_StempFixNum ~= nCheckCount then
		
		CF_PrintMsg_SystemMsgBox( GetGameInternalText( "EMREQ_REMODELOPEN_FB_STAMP_ERROR", 0 ), TEXT_COLOR_RED )
		return
		
	end
	
	g_Msg:ReqRebuildItem()
	
end

-- 키보드 키 입력시 들어올 함수(처리는 CF_HotKey_Confirm 함수에서 한다.)
function Ev_RebuildCard_KeyUp()

	if IsOpenWidget( WIDGET_ID_REBUILD_CARD ) == false then
		return false
	end
		
	if GetBool( WIDGET_ID_REBUILD_CARD, "visible", "btnOK" ) == true then
		Event_RebuildCard_btnOK()
		return true
	end	
	
	return false
	
end

function Ev_GameMsg_OK_RebuildCard_FixPopupUI ()

	Event_MsgBox_Close()
	
	g_Msg:ReqRebuildItem_Fix()
	
end

function Ev_GameMsg_Cancel_RebuildCard_FixPopupUI ()

	Event_MsgBox_Close()
	
	g_Msg:ReqRebuildItem_Cancel()
	
end

function Event_RebuildCard_ChangeRebuildType()
	-- 해당 선택시 패킷이 날라감으로 막바꾸진 않는다;
	local bReq = true
	if g_bRebuildType_KEEPORIGIN == true then
		bReq = false
		
	end
	
	g_Msg:ReqChangeRebuildType( bReq )

end

function Event_RebuildCard_btnSelectToCache()
	if g_tbRebuildCard_CacheItemInfo == nil then
		return
		
	end
	
	g_Msg:ReqRebuild_SelectCache()
	
end

function RebuildCard_ChangedRebuildType(bType)
	g_bRebuildType_KEEPORIGIN = bType
	RebuildCard_SetTag()
	SetBool( WIDGET_ID_REBUILD_CARD, "selected", g_bRebuildType_KEEPORIGIN, "cbKeepOrigin")
	
end