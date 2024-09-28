
OPEN_PARTYDISTRIBUTION_DICE = false

function LoadPartyDistribution_dice()
	-- 윈도우 타이틀 설정 및 포기, 입찰 버튼 활성화
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	PropertyArraySetting(tbProp,"label", "label", "label")
	PropertyArraySetting(tbInst,"btnGiveUp", "btnBidding", "btnDiscard")
	PropertyArraySetting(tbData, GetGameWordText( "PARTY_DICE_TENDER_WINDOW_TEXT", 1 )
							   , GetGameWordText( "PARTY_DICE_TENDER_WINDOW_TEXT", 2 )
							   , GetGameWordText( "PARTY_DISTRIBUTION_WINDOW_TEXT", 0 ))
							   
	PropertyArraySetting(tbType, "-s", "-s", "-s")
			
			
			
	-- 종료 이벤트
	SetEvent( WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_WIDGET_CLOSE, "ClosePartyDistribution_dice", "", "" )
	
	-- 슬롯 기본값 세팅/바인딩
	for i = 1, PARTYDISTRIBUTION_SLOT_TOTAL, 1 do
		-- 슬롯;
		local strSlot = "slot"..tostring(i)
		PropertyArraySetting(tbProp, "visible", "binding")
		PropertyArraySetting(tbInst, strSlot, strSlot)
		PropertyArraySetting(tbData, false, SLOT_PARTYDISTRIBUTION_ID..tostring(i))
		PropertyArraySetting(tbType, "-b", "-s")
		
	end
	SetArray(WIDGET_ID_PARTYDISTRIBUTION_DICE, tbProp, tbData, tbInst, tbType)
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_LIST_ITEM_UP, "Ev_PDD_ItemListClick", "buttonIdx;index;selected;", "list")
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_BUTTON_CLICK, "Ev_PDD_GiveupClick", "", "btnGiveUp")
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_BUTTON_CLICK, "Ev_PDD_BiddingClick", "", "btnBidding")
	
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_BUTTON_CLICK, "Ev_PDD_JUNKITEM_MsgBox", "", "btnDiscard")
	-- 버리기 기능 기존 코드가 다이스에 있어서 일단은 다이스 부분에 작업 .
	
	
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_SCROLL, "PDD_UpdateItemList", "", "list" )
	
end

function OpenPartyDistribution_dice()
	OPEN_PARTYDISTRIBUTION_DICE = true
	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	PropertyArraySetting(tbProp, "visible", "visible", "visible", "title")
	PropertyArraySetting(tbInst, "btnGiveUp", "btnBidding", "btnDiscard", "")
	PropertyArraySetting(tbType, "-b", "-b", "-b", "-s")	
	-- PartyDistributionUI_Manager.lua
	if IsDiceDistributionItemOption() then
		PropertyArraySetting(tbData, true, true, false
								, GetGameWordText( "PARTY_MODAL_ITEM_OPTION", 5 ) .. "(" .. GetGameWordText( "PARTY_MODAL_ITEM_OPTION_FILTER", g_Party:GetItemFilterOption() ) .. ")")
		SetBool( WIDGET_ID_PARTYDISTRIBUTION_DICE, "multiSelect", true, "list" )
	elseif IsLeaderShareDistributionItemOption() then
		PropertyArraySetting(tbData, false, false, true
								, GetGameWordText("PARTY_MODAL_ITEM_OPTION", 4) )
		SetBool( WIDGET_ID_PARTYDISTRIBUTION_DICE, "multiSelect", false, "list" )
	end
	SetArray(WIDGET_ID_PARTYDISTRIBUTION_DICE, tbProp, tbData, tbInst, tbType)
	
	PDD_Update()
end

function PDD_Update()

	if OPEN_PARTYDISTRIBUTION_DICE == false then
		return
	end
	PDD_SetItemListData()
	PDD_UpdateItemList()
	
end

function ClosePartyDistribution_dice()
	OPEN_PARTYDISTRIBUTION_DICE = false
end

-- 리스트 자체에 전달할 프로바이더 세팅;
function PDD_SetItemListData()
	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	local tbItemList, tbProvideList = g_Client:PartyDistribution_GetItemInfo(), {}
	local nItemListSize = table.getn( tbItemList )
		
	if IsDiceDistributionItemOption() == false then -- 다이스 분배가 아닌경우 ( -> 마스터 분배 ) 	
		local selectItem = GetInt( WIDGET_ID_PARTYDISTRIBUTION_DICE, "selectedIndex", "list" )
		if selectItem ~= nill then
			Ev_PDD_ItemListClick( selectItem, selectItem)
		end
	end		
		
	for i = 1, nItemListSize, 1 do
		local tbItem, tbProvide, nSelect = tbItemList[i], {}, 0
		if tbItem["select"] == true then
			nSelect = 1
		end
		tbProvide[1] = tbItem["NATIVEID"]
		tbProvide[2] = nSelect
	
		table.insert(tbProvideList, tbProvide)
	end
	if nItemListSize == 0 then
		ClosePartyDistributionUI()
	end
	SetDataEx(WIDGET_ID_PARTYDISTRIBUTION_DICE, "id;multiSelected", tbProvideList, "list", false)
end

-- 리스트 아이템렌더러 세팅;
function PDD_UpdateItemList()
	
	local tbItemList = g_Client:PartyDistribution_GetItemInfo()
	local nSize = table.getn(tbItemList)
	
	if nSize == 0 then
		ClosePartyDistributionUI()--CloseWidget(WIDGET_ID_PARTYDISTRIBUTION_DICE)
		return
	end

	-- 전달자;
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	local nScrollPos = GetInt( WIDGET_ID_PARTYDISTRIBUTION_DICE, "scrollPosition", "list" )
	
	-- 리스트 아이템 렌더러 속성 세팅;	
	for i = 1, PARTYDISTRIBUTION_SLOT_TOTAL, 1 do
		local strItemInst = "item" .. tostring( i )
		local nOffset = nScrollPos + i
		
		if nOffset <= nSize then
			local tbItem = tbItemList[nOffset]
			
			local strItem = HTMLFontColor( PrepareHTMLData( tbItem["name"] ), TranslateHTMLColor( tbItem["namecolor"] ) )
			--local strItem = HTMLFontColor( PrepareHTMLData( tbItem["name"] ), HTML_TEXT_COLOR_WHITE)
			--strItem = tbItem["name"] 
			
			--local strItem = HTMLFontColor( PrepareHTMLData( ItemName[1] ), TranslateHTMLColor( ItemName[2] ) )
			
			

			-- ItemTooltip.lua
			local strSchool, strClass, strLevel, strCondition = "", "", "", ""
			
			strSchool = ItemToolTip_Header_School(tbItem, "")
			strClass  = ItemToolTip_Header_Class(tbItem, "")
			strLevel  = ItemToolTip_Header_Level(tbItem, "")
			
			strCondition = __Condition_status(tbItem)			-- 능력치 비교;
			if strLevel ~= "" and strCondition ~= "" then
				strLevel = strLevel .. "/" .. strCondition
			elseif strCondition ~= "" then
				strLevel = strCondition
			end
			
			local nState = tbItem["state"] -- 0:입찰 완료, 1:입찰 불가능, 2:입찰 가능
			local bState = { false, false, false }
			bState[nState+1] = true
			
			-- 위에서 한번 검사를 했지만 UI상으로 데이터를 받아오면 문제가 생길 여지가 있어서
			-- 한번더 검사 하는 함수를 제작했습니다.
			-- _Check_CanTakeItem 은 이 아이템을 착용가능한지 구별하는 함수이며
			-- ItemTooltip의 함수들을 참고해서 제작했습니다.
			
			local bCheckUnTakeItem =  _Check_CanTakeItem(tbItem) == false
			bState[2] = bCheckUnTakeItem
			
			-- 주사위 분배 // 입찰시 X자가 주사위 아이콘을 가림 그래서 수정했습니다..
			if bState[1] == true 	then
				bState[2] = false
			end
			-- ItemVisible, SlotVisible, ItemName, School/CharClass
			PropertyArraySetting(tbProp, "visible", "visible", "htmlText", "htmlText", "htmlText"
									   , "visible", "visible", "visible")
			PropertyArraySetting(tbInst, strItemInst
										, "slot"..tostring(i)
										, strItemInst..".tfItemName"
										, strItemInst..".tfSchool"
										, strItemInst..".tfLevel"
										, strItemInst..".tenderIcon.tenderDice"
										, strItemInst..".tenderIcon.tenderX"
										, strItemInst..".tenderIcon.tenderEmpty")
			--PropertyArraySetting(tbData, true, true, strItem, strSchool..strClass, strLevel, , bState[2], bState[3] )
			PropertyArraySetting(tbData, true, true, strItem, strSchool..strClass, strLevel, bState[1], bState[2], bState[3] )
		
			PropertyArraySetting(tbType, "-b", "-b", "-s", "-s", "-s", "-b", "-b", "-b")
			SetSlot_PartyDistribution(SLOT_PARTYDISTRIBUTION_ID..tostring(i), tbItem["NATIVEID"], nOffset-1)
			
			--프로그레스바	설정
			-- tenderTime : 총시간
			-- percent : 진행률
			local tbQuickSet =
			{
				-- 남은 시간 설정
				time = tbItem["tenderTime"] * (1.0-tbItem["percent"]) * 1000,
				delay = 0,
				ease = EASE_NONE,
				loop = false,
				onComplete = ""
			}
			-- 현재 진행률 설정
			PropertyArraySetting(tbProp, "value")
			PropertyArraySetting(tbInst, strItemInst..".progressbar.progressbar")
			PropertyArraySetting(tbData, tbItem["percent"] * 100)
			PropertyArraySetting(tbType, "-n")
			-- 전체 진행률 설정
			SetTween( WIDGET_ID_PARTYDISTRIBUTION_DICE, tbQuickSet, "value", "100", strItemInst..".progressbar.progressbar" )
			
--			if tbItem["select"] == true then
--				PropertyArraySetting(tbProp, "selectedIndex")
--				PropertyArraySetting(tbInst, "list")
--				PropertyArraySetting(tbData, nOffset-1)
--				PropertyArraySetting(tbType, "-n")
--			end
			
		else
			PropertyArraySetting(tbProp, "visible", "visible")
			PropertyArraySetting(tbInst, strItemInst, "slot"..tostring(i))
			PropertyArraySetting(tbData, false, false)
			PropertyArraySetting(tbType, "-b", "-b")
			
		end
	end
	SetArray(WIDGET_ID_PARTYDISTRIBUTION_DICE, tbProp, tbData, tbInst, tbType)
end

function Ev_PDD_ItemListClick(buttonIdx, index)
	-- 좌클릭만 인식!
	if buttonIdx ~= 0 then
		return
	end
	
		
	-- 기존 방식이 다이스에 있던 방식 그대로 적용되어 선택이 하나만 되어야하는데
	-- 선택은 여러개되고 UI상으로만 하나만 선택되게 만들어져있어서
	-- 마스터 분배시에는 선택 할 경우 다른 선택이 모두 초기화 되게 제작했습니다.
	--	SetBool( WIDGET_ID_PARTYDISTRIBUTION_DICE, "multiSelect", false, "list" ) // UI상으로 다중선택이 가능한지 결정하는 부분
	
	if IsDiceDistributionItemOption() == false then -- 다이스 분배가 아닌경우 ( -> 마스터 분배 ) 
		g_Client:PartyResetSelectAll()
	end
	 
	
	g_Client:PartyDistribution_SelectItem(index)	-- toggle	
--	PDD_Update()
--	PDD_UpdateItemList()
	PDM_UpdateItemList()
	-- 멤버 클래스에 따라 착용불가시 빨강색으로 변경하기위해
	
end


-- 입찰 외 포기
function Ev_PDD_GiveupClick()
	g_Client:PartyDistribution_AbadonItem()
	PDD_Update()
	--ClosePartyDistributionUI()
end

-- 입찰
function Ev_PDD_BiddingClick()
	g_Client:PartyDistribution_Bindding()
end

function Ev_PDD_JUNKITEM_MsgBox()
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PARTY_DISTRIBUTION_ITEM_JUNK_BTN", 0 ), 
		Ev_PDD_JUNKITEM, Event_MsgBox_Close )
		
end


function Ev_PDD_JUNKITEM()
	g_Client:PartyDistribution_JunkItem()
	Event_MsgBox_Close()
end

function __Condition_status(tbItem)
	-- 힘;체력;정신력;민첩성;지력;근력;
	local tbCondition, tbCharacter = tbItem["conditionStatus"], tbItem["characterStatus"]
	local bExistFrontValue, strCondition = false, ""
	for i = 1, 6, 1 do
		if tbCondition[i] == 0 then
			continue
		end
		
		if bExistFrontValue == true then
			strCondition = strCondition.."/"
		end
		if tbCondition[i] <= tbCharacter[i] then
			strCondition = strCondition..HTMLFontColor( GetGameWordText("ITEM_ADVANCED_INFO_CONDITION_STATS", i) .. " ".. tostring(tbCondition[i]), HTML_TEXT_COLOR_WHITE)
		else
			strCondition = strCondition..HTMLFontColor( GetGameWordText("ITEM_ADVANCED_INFO_CONDITION_STATS", i) .. " ".. tostring(tbCondition[i]), TEXT_COLOR_RED)
		end			
		bExistFrontValue = true
	end
	return strCondition
end

function _Check_CanTakeItem(tbItem)
	local CanTakeItem = false
	
--  학원 쪽은 나중에 추가 관련 함수가 없음..?	
	local Item_School = tbItem["School"]	-- 요구 학원
	if "ALL" ~= Item_School then
		
		if g_MyChar:GetSchool() ~= Item_School then
			return false
		end	
		
	end	
--	g_MyChar

-- 클래스 검사 ---------------------------------------------------------------------

	local Header_Class = tbItem["Class"]		-- 요구 부서
	local Header_Sex = tbItem["Sex"]			-- 요구 성별
	local ClassSize = table.getn( Header_Class )	-- 클래스 삽입을 위한 임시 변수
	
	local ClassCheck = false		-- 착용 가능한 장비인지를 체크하기 위한 변수
	if "ALL" == Header_Class[1] then	-- 모든 클레스일 경우
		ClassCheck = true		
	elseif g_MaxClassNum == ClassSize then
		ClassCheck = true	
	else
		for i = 1, ClassSize, 1 do			
			if	nil ~= Header_Class[i] then		-- 클레스 값이 nil이 아닐경우			
				if GetToolTipClassText( Header_Class[i] ) == GetItemToolTip_ClassText( g_MyChar.nClassIndex ) then
					ClassCheck = true
				end
			end
		end
	end
	if false == ClassCheck then
		return false
	end		
	
	-- 자신과 동일한 성별이 아닐경우
	if false == Header_Sex[ "SameSex" ] then
			return false
	end
	
----------------------------------------------------------------------------------
	
----------- 요구 레밸
	local Header_Level = tbItem["Level"]		-- 요구 레벨

	if 0 < tonumber(Header_Level["LevelDW"]) then			-- 하한선이 0이상일 경우(하한선 : 해당 레벨 이하이면 불가)
		if 0 < tonumber(Header_Level["LevelUP"]) then		-- 상한선이 0이상일 경우(상한선 : 해당 레벨 이상이면 불가)
			if g_MyChar:GetLevel() < tonumber(Header_Level["LevelDW"]) or g_MyChar:GetLevel() > tonumber(Header_Level["LevelUP"]) then	-- 상한선 혹은 하한선을 만족하지 않을 경우
				return false
			end
		else		-- 하한선 존재할 경우
			if g_MyChar:GetLevel() < tonumber(Header_Level["LevelDW"]) then	-- 하한선을 만족하지 못할 경우
				return false
			end
		end
	else		-- 하한선이 존재하지 않는 경우(상한선만 존재)
		if 0 < tonumber(Header_Level["LevelUP"]) then						-- 상한선이 0이상일 경우(상한선 : 해당 레벨 이상이면 불가)
			if g_MyChar:GetLevel() > tonumber(Header_Level["LevelUP"]) then	-- 상한선이 충족되지 못할 경우
				return false
			end
		end
	end
	
----------------------------------------------------------------------------------
-------------------- 요구 스텟

	local Header_Stat = tbItem["Stat"]		-- 요구 스텟
	if Header_Stat ~= nil then
	
	local Header_Level = tbItem["Level"]		-- 요구 레벨
	local DwStat_Char = Header_Stat["Char"]		-- 캐릭터 보유 스텟
	local DwStat_Item = Header_Stat["Item"]		-- 아이템 요구 스텟

	if DwStat_Char ~= nil and DwStat_Item ~= nil then
	
		if 0 < tonumber(DwStat_Item["Pow"]) then
			if tonumber(DwStat_Char["Pow"]) < tonumber(DwStat_Item["Pow"]) then
				return false
			end
		end
	
		if 0 < tonumber(DwStat_Item["Dex"]) then
			if tonumber(DwStat_Char["Dex"]) < tonumber(DwStat_Item["Dex"]) then
				return false
			end
		end
	
		if 0 < tonumber(DwStat_Item["Spi"]) then
			if tonumber(DwStat_Char["Spi"]) < tonumber(DwStat_Item["Spi"]) then
				return false	
			end
		end
	end
	
	end
----------------------------------------------------------------------------------

	return true
end











