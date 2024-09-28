
bMacroOptionOpen = false

function LoadMacroOption()
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	local tbOption = g_Macro:GetAllOptionState()
	
	-- üũ �ڽ� ����;	
	for i = 1, 4, 1 do
	
		local strInstanceName = "cbSkillTary_F"..tostring(i)
		
		SetBool( WIDGET_ID_MACROOPTION, "selected", tbOption["bSkillTrayF"..tostring(i)], strInstanceName )
		SetString( WIDGET_ID_MACROOPTION, "text", 
			GetGameWordText( "MACROOPTIONWINDOW_COMPONENTNAME", 0 ) .. i, strInstanceName .. "_text" )
			
	end
	
	SetBool( WIDGET_ID_MACROOPTION, "selected", tbOption["bRemainDrug"], "cbReturnOp_ReaminDrugCount" )
	SetBool( WIDGET_ID_MACROOPTION, "selected", tbOption["bRemainMacro"], "cbReturnOp_ReaminMacroTime" )
	
	SetString( WIDGET_ID_MACROOPTION, "text", 
		GetGameWordText( "MACROOPTIONWINDOW_COMPONENTNAME", 1 ), "cbReturnOp_ReaminDrugCount_text" )
	SetString( WIDGET_ID_MACROOPTION, "text", 
		GetGameWordText( "MACROOPTIONWINDOW_COMPONENTNAME", 2 ), "cbReturnOp_ReaminMacroTime_text" )
	
	SetEvent(WIDGET_ID_MACROOPTION, EVENT_BUTTON_CLICK, "Ev_Mo_ReturnOp_RemainDrugCount", "", "cbReturnOp_ReaminDrugCount", "")	
	SetEvent(WIDGET_ID_MACROOPTION, EVENT_BUTTON_CLICK, "Ev_Mo_ReturnOp_ReaminMacroTime", "", "cbReturnOp_ReaminMacroTime", "")	
	
	-- ��ư ����
	SetEvent(WIDGET_ID_MACROOPTION, EVENT_BUTTON_CLICK, "Ev_Mo_ActiveToggle", "", "btStartEnd", "")	
	Mo_SetStartEndButton(tbOption["bActive"])
	
	SetEvent( WIDGET_ID_MACROOPTION, EVENT_WIDGET_CLOSE, "CloseMacroOption", "", "" )
end

function OpenMacroOption()
	bMacroOptionOpen = true
	Mo_SetStartEndButton(g_Macro:IsActive())
end

function CloseMacroOption()
	bMacroOptionOpen = false
end

function Ev_Mo_SkillFCheckBox(strTrayCount)
	local bSelected = GetBool(WIDGET_ID_MACROOPTION, "selected", "cbSkillTary_F"..strTrayCount)
	g_Macro:SetSkillTabEnable(tonumber(strTrayCount)-1, bSelected)
end

function Ev_Mo_ReturnOp_RemainDrugCount()
	if Mo_UseableReturnOption() == false then
		SetBool(WIDGET_ID_MACROOPTION, "selected", false, "cbReturnOp_ReaminDrugCount")
		return
	end
	local bSelected = GetBool(WIDGET_ID_MACROOPTION, "selected", "cbReturnOp_ReaminDrugCount")
	g_Macro:SetCheckRemainDrugCount(bSelected)
end

function Ev_Mo_ReturnOp_ReaminMacroTime()
	if Mo_UseableReturnOption() == false then
		SetBool(WIDGET_ID_MACROOPTION, "selected", false, "cbReturnOp_ReaminMacroTime")
		return
	end
	local bSelected = GetBool(WIDGET_ID_MACROOPTION, "selected", "cbReturnOp_ReaminMacroTime")
	g_Macro:SetCheckRemainMacroTime(bSelected)
end

function Mo_SetStartEndButton(bIsActive)
	-- ��� ���̶��
	if bIsActive == false then
		SetString(WIDGET_ID_MACROOPTION, "label", GetGameWordText("MACROOPTIONWINDOW_COMPONENTNAME", 3), "btStartEnd")
	else
		SetString(WIDGET_ID_MACROOPTION, "label", GetGameWordText("MACROOPTIONWINDOW_COMPONENTNAME", 4), "btStartEnd")
	end
end

function Ev_Mo_ActiveToggle()
	local bActive = g_Macro:IsActive() ~= true
	g_Macro:MacroOnOff(bActive)
end

function Mo_Update()
	if bMacroOptionOpen == true then
		Mo_SetStartEndButton(g_Macro:IsActive())
	end
end

function Mo_UseableReturnOption()
	local bReturn = g_MyChar:IsExistReturnSchoolCard()
	if bReturn == false then
		local strText = GetGameWordText( "MACRO_RETURNOPTION_ENABLEFAILD", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, strText, Event_MsgBox_Close )
	end
	return bReturn
end

-- Ŭ���̾�Ʈ���� ��ȯ ��ũ�� ȣ���� ��ȯ �������� ���� ��� ��ư�� ���� �Ѵ�. 
function Mo_ResetReMainButtons()

	SetBool( WIDGET_ID_MACROOPTION, "selected", false, "cbReturnOp_ReaminDrugCount" )
	SetBool( WIDGET_ID_MACROOPTION, "selected", false, "cbReturnOp_ReaminMacroTime" )
	
end
