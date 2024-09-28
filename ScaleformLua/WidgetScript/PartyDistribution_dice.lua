
OPEN_PARTYDISTRIBUTION_DICE = false

function LoadPartyDistribution_dice()
	-- ������ Ÿ��Ʋ ���� �� ����, ���� ��ư Ȱ��ȭ
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	PropertyArraySetting(tbProp,"label", "label", "label")
	PropertyArraySetting(tbInst,"btnGiveUp", "btnBidding", "btnDiscard")
	PropertyArraySetting(tbData, GetGameWordText( "PARTY_DICE_TENDER_WINDOW_TEXT", 1 )
							   , GetGameWordText( "PARTY_DICE_TENDER_WINDOW_TEXT", 2 )
							   , GetGameWordText( "PARTY_DISTRIBUTION_WINDOW_TEXT", 0 ))
							   
	PropertyArraySetting(tbType, "-s", "-s", "-s")
			
			
			
	-- ���� �̺�Ʈ
	SetEvent( WIDGET_ID_PARTYDISTRIBUTION_DICE, EVENT_WIDGET_CLOSE, "ClosePartyDistribution_dice", "", "" )
	
	-- ���� �⺻�� ����/���ε�
	for i = 1, PARTYDISTRIBUTION_SLOT_TOTAL, 1 do
		-- ����;
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
	-- ������ ��� ���� �ڵ尡 ���̽��� �־ �ϴ��� ���̽� �κп� �۾� .
	
	
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

-- ����Ʈ ��ü�� ������ ���ι��̴� ����;
function PDD_SetItemListData()
	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	local tbItemList, tbProvideList = g_Client:PartyDistribution_GetItemInfo(), {}
	local nItemListSize = table.getn( tbItemList )
		
	if IsDiceDistributionItemOption() == false then -- ���̽� �й谡 �ƴѰ�� ( -> ������ �й� ) 	
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

-- ����Ʈ �����۷����� ����;
function PDD_UpdateItemList()
	
	local tbItemList = g_Client:PartyDistribution_GetItemInfo()
	local nSize = table.getn(tbItemList)
	
	if nSize == 0 then
		ClosePartyDistributionUI()--CloseWidget(WIDGET_ID_PARTYDISTRIBUTION_DICE)
		return
	end

	-- ������;
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	local nScrollPos = GetInt( WIDGET_ID_PARTYDISTRIBUTION_DICE, "scrollPosition", "list" )
	
	-- ����Ʈ ������ ������ �Ӽ� ����;	
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
			
			strCondition = __Condition_status(tbItem)			-- �ɷ�ġ ��;
			if strLevel ~= "" and strCondition ~= "" then
				strLevel = strLevel .. "/" .. strCondition
			elseif strCondition ~= "" then
				strLevel = strCondition
			end
			
			local nState = tbItem["state"] -- 0:���� �Ϸ�, 1:���� �Ұ���, 2:���� ����
			local bState = { false, false, false }
			bState[nState+1] = true
			
			-- ������ �ѹ� �˻縦 ������ UI������ �����͸� �޾ƿ��� ������ ���� ������ �־
			-- �ѹ��� �˻� �ϴ� �Լ��� �����߽��ϴ�.
			-- _Check_CanTakeItem �� �� �������� ���밡������ �����ϴ� �Լ��̸�
			-- ItemTooltip�� �Լ����� �����ؼ� �����߽��ϴ�.
			
			local bCheckUnTakeItem =  _Check_CanTakeItem(tbItem) == false
			bState[2] = bCheckUnTakeItem
			
			-- �ֻ��� �й� // ������ X�ڰ� �ֻ��� �������� ���� �׷��� �����߽��ϴ�..
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
			
			--���α׷�����	����
			-- tenderTime : �ѽð�
			-- percent : �����
			local tbQuickSet =
			{
				-- ���� �ð� ����
				time = tbItem["tenderTime"] * (1.0-tbItem["percent"]) * 1000,
				delay = 0,
				ease = EASE_NONE,
				loop = false,
				onComplete = ""
			}
			-- ���� ����� ����
			PropertyArraySetting(tbProp, "value")
			PropertyArraySetting(tbInst, strItemInst..".progressbar.progressbar")
			PropertyArraySetting(tbData, tbItem["percent"] * 100)
			PropertyArraySetting(tbType, "-n")
			-- ��ü ����� ����
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
	-- ��Ŭ���� �ν�!
	if buttonIdx ~= 0 then
		return
	end
	
		
	-- ���� ����� ���̽��� �ִ� ��� �״�� ����Ǿ� ������ �ϳ��� �Ǿ���ϴµ�
	-- ������ �������ǰ� UI�����θ� �ϳ��� ���õǰ� ��������־
	-- ������ �й�ÿ��� ���� �� ��� �ٸ� ������ ��� �ʱ�ȭ �ǰ� �����߽��ϴ�.
	--	SetBool( WIDGET_ID_PARTYDISTRIBUTION_DICE, "multiSelect", false, "list" ) // UI������ ���߼����� �������� �����ϴ� �κ�
	
	if IsDiceDistributionItemOption() == false then -- ���̽� �й谡 �ƴѰ�� ( -> ������ �й� ) 
		g_Client:PartyResetSelectAll()
	end
	 
	
	g_Client:PartyDistribution_SelectItem(index)	-- toggle	
--	PDD_Update()
--	PDD_UpdateItemList()
	PDM_UpdateItemList()
	-- ��� Ŭ������ ���� ����Ұ��� ���������� �����ϱ�����
	
end


-- ���� �� ����
function Ev_PDD_GiveupClick()
	g_Client:PartyDistribution_AbadonItem()
	PDD_Update()
	--ClosePartyDistributionUI()
end

-- ����
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
	-- ��;ü��;���ŷ�;��ø��;����;�ٷ�;
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
	
--  �п� ���� ���߿� �߰� ���� �Լ��� ����..?	
	local Item_School = tbItem["School"]	-- �䱸 �п�
	if "ALL" ~= Item_School then
		
		if g_MyChar:GetSchool() ~= Item_School then
			return false
		end	
		
	end	
--	g_MyChar

-- Ŭ���� �˻� ---------------------------------------------------------------------

	local Header_Class = tbItem["Class"]		-- �䱸 �μ�
	local Header_Sex = tbItem["Sex"]			-- �䱸 ����
	local ClassSize = table.getn( Header_Class )	-- Ŭ���� ������ ���� �ӽ� ����
	
	local ClassCheck = false		-- ���� ������ ��������� üũ�ϱ� ���� ����
	if "ALL" == Header_Class[1] then	-- ��� Ŭ������ ���
		ClassCheck = true		
	elseif g_MaxClassNum == ClassSize then
		ClassCheck = true	
	else
		for i = 1, ClassSize, 1 do			
			if	nil ~= Header_Class[i] then		-- Ŭ���� ���� nil�� �ƴҰ��			
				if GetToolTipClassText( Header_Class[i] ) == GetItemToolTip_ClassText( g_MyChar.nClassIndex ) then
					ClassCheck = true
				end
			end
		end
	end
	if false == ClassCheck then
		return false
	end		
	
	-- �ڽŰ� ������ ������ �ƴҰ��
	if false == Header_Sex[ "SameSex" ] then
			return false
	end
	
----------------------------------------------------------------------------------
	
----------- �䱸 ����
	local Header_Level = tbItem["Level"]		-- �䱸 ����

	if 0 < tonumber(Header_Level["LevelDW"]) then			-- ���Ѽ��� 0�̻��� ���(���Ѽ� : �ش� ���� �����̸� �Ұ�)
		if 0 < tonumber(Header_Level["LevelUP"]) then		-- ���Ѽ��� 0�̻��� ���(���Ѽ� : �ش� ���� �̻��̸� �Ұ�)
			if g_MyChar:GetLevel() < tonumber(Header_Level["LevelDW"]) or g_MyChar:GetLevel() > tonumber(Header_Level["LevelUP"]) then	-- ���Ѽ� Ȥ�� ���Ѽ��� �������� ���� ���
				return false
			end
		else		-- ���Ѽ� ������ ���
			if g_MyChar:GetLevel() < tonumber(Header_Level["LevelDW"]) then	-- ���Ѽ��� �������� ���� ���
				return false
			end
		end
	else		-- ���Ѽ��� �������� �ʴ� ���(���Ѽ��� ����)
		if 0 < tonumber(Header_Level["LevelUP"]) then						-- ���Ѽ��� 0�̻��� ���(���Ѽ� : �ش� ���� �̻��̸� �Ұ�)
			if g_MyChar:GetLevel() > tonumber(Header_Level["LevelUP"]) then	-- ���Ѽ��� �������� ���� ���
				return false
			end
		end
	end
	
----------------------------------------------------------------------------------
-------------------- �䱸 ����

	local Header_Stat = tbItem["Stat"]		-- �䱸 ����
	if Header_Stat ~= nil then
	
	local Header_Level = tbItem["Level"]		-- �䱸 ����
	local DwStat_Char = Header_Stat["Char"]		-- ĳ���� ���� ����
	local DwStat_Item = Header_Stat["Item"]		-- ������ �䱸 ����

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











