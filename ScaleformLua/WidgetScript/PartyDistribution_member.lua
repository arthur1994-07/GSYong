
OPEN_PARTYDISTRIBUTION_MEMBER = false
PARTYDISTRIBUTION_MEMBERITEM_TOTAL = 10

function LoadPartyDistribution_member()
	-- ������ Ÿ��Ʋ ����, �絵 ��ư Ȱ��ȭ
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	PropertyArraySetting(tbProp, "title", "label")
	PropertyArraySetting(tbInst, "", "btnTransfer")
	PropertyArraySetting(tbData
						, GetGameWordText("PARTY_DISTRIBUTION_WINDOW_TEXT", 2)
						, GetGameWordText("PARTY_DISTRIBUTION_WINDOW_TEXT", 1))
	PropertyArraySetting(tbType, "-s", "-s")	
	
	SetArray(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, tbProp, tbData, tbInst, tbType)
	
	-- ���� �̺�Ʈ
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, EVENT_WIDGET_CLOSE, "ClosePartyDistribution_member", "", "" )
	-- UI Event
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, EVENT_LIST_ITEM_UP, "Ev_PDM_ItemListClick", "buttonIdx;index;selected;", "list")
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, EVENT_BUTTON_CLICK, "Ev_PDM_TransferClick", "", "btnTransfer")
	SetEvent(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, EVENT_SCROLL, "PDM_UpdateItemList", "", "list" )
end

function OpenPartyDistribution_member()
	OPEN_PARTYDISTRIBUTION_MEMBER = true
	local nDiceUIPos_x = 0
	local nDiceUIPos_y = 0 
	if OPEN_PARTYDISTRIBUTION_DICE == true then
		nDiceUIPos_x = GetInt(WIDGET_ID_PARTYDISTRIBUTION_DICE, "x", "")
		nDiceUIPos_y = GetInt(WIDGET_ID_PARTYDISTRIBUTION_DICE, "y", "")
	end
	
	local nThis_Width = GetInt(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, "width", "")
	SetInt(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, "x", nDiceUIPos_x-nThis_Width, "")
	
	PDM_UpdateItemList()
end

function ClosePartyDistribution_member()
	OPEN_PARTYDISTRIBUTION_MEMBER = false
end

function Ev_PDM_ItemListClick(buttonIdx, index)
	-- ��Ŭ���� �ν�!
	if buttonIdx ~= 0 then
		return
	end
	g_Client:PartyDistribution_SelectMember(index)
end

function Ev_PDM_TransferClick()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PARTY_DISTRIBUTION_ITEM_TRANSFER_BTN", 0 ), 
		Ev_PDM_Transfer, Event_MsgBox_Close )
		
end

function Ev_PDM_Transfer()
	g_Client:PartyDistribution_TransferItem()
	Event_MsgBox_Close()
	-- ������ ��� ����
	PDD_Update()
end

function PDM_UpdateItemList()
	-- ����Ʈ ���ι��̴� ����;
	local tbPartyMemberList, tbProvideList = g_Client:PartyDistribution_GetMemberInfo(), {}
	local nPartyMemberSize = table.getn( tbPartyMemberList )
	for i = 1, nPartyMemberSize, 1 do
		local tbMember, tbProvide = tbPartyMemberList[i], {}
		
		local nSelect = 0
		if tbMember["select"] == true then
			nSelect = 1
		end
				
		tbProvide[1] = tbMember["name"]
		tbProvide[2] = nSelect	-- ���ÿ���
		table.insert(tbProvideList, tbProvide)
		
	end
	SetDataEx(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, "name;multiSelected", tbProvideList, "list", false)
	-- ������;
	
	-- http://172.16.2.87/redmine/issues/4117
	-- [��Ƽ]�����ͺй� ��Ƽ�� ���뿩�� ��� ���� ��û
	-- ItemTooltip.lua�� ItemToolTip_Header_Class �Լ� �����Ͽ����ϴ�. 
	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	local nScrollPos = GetInt( WIDGET_ID_PARTYDISTRIBUTION_MEMBER, "scrollPosition", "list" )
	for i = 1, PARTYDISTRIBUTION_MEMBERITEM_TOTAL, 1 do
		local strItemInst = "item" .. tostring( i )
				
		local nOffset = i
		
		
		if nOffset <= nPartyMemberSize then
			local tbMember = tbPartyMemberList[nOffset]
			local strClass = tbMember["class"]
			local strLevel = "Lv"..tbMember["level"]
			local strName = tbMember["name"]
				

			local tbItemList = g_Client:PartyDistribution_GetItemInfo()
			local nSize = table.getn(tbItemList)
			
			 for  i = 1, nSize, 1 do
				local tbItem = tbItemList[i]

				if( tbItem["select"] ==  true ) then
					
					local Class = tbItem["Class"]
					local ClassSize = table.getn( Class )
					local ClassCheck = false	
					
					
					if "ALL" ==  Class[1] then
						ClassCheck = true
						continue						
					end
					
					for j = 1, ClassSize, 1 do
						
						strClass = HTMLFontColor( PrepareHTMLData( tbMember["class"] ), HTML_TEXT_COLOR_WHITE )
					
						
						local InClass = Class[j]	
				
						 if InClass == nil then
							continue 
						 end
					
				
					
						if GetToolTipClassText( InClass ) == tbMember["class"] then
							break
						end
	
						strClass = HTMLFontColor( PrepareHTMLData( tbMember["class"] ), HTML_TEXT_COLOR_RED )
					end
					
				 end		
				
			 end
			
						
				
			PropertyArraySetting(tbProp, "visible", "htmlText", "htmlText", "htmlText")
			PropertyArraySetting(tbInst, strItemInst
									   , strItemInst..".tfClass"
									   , strItemInst..".tfLevel"
									   , strItemInst..".tfName")
			PropertyArraySetting(tbData, true, strClass, strLevel, strName)
			PropertyArraySetting(tbType, "-b", "-s", "-s", "-s")
			
		else
			PropertyArraySetting(tbProp, "visible")
			PropertyArraySetting(tbInst, strItemInst)
			PropertyArraySetting(tbData, false)
			PropertyArraySetting(tbType, "-b")
		end
		
	end
	SetArray(WIDGET_ID_PARTYDISTRIBUTION_MEMBER, tbProp, tbData, tbInst, tbType)
end

