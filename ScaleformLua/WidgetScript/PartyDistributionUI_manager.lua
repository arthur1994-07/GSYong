
-- GLPartyDefine.h:EMPARTY_ITEM_OPTION를 따른다
DEF_PDI_LEADER_SHARE	= 4	-- 파티/원정대장(분배);
DEF_PDI_DICE			= 5	-- 주사위;	

-- DEF_PDI_LEADER_SHARE or DEF_PDI_DICE;
g_PDI_Type				= DEF_PDI_LEADER_SHARE

function IsDiceDistributionItemOption()
	return DEF_PDI_DICE == g_PDI_Type
end

function IsLeaderShareDistributionItemOption()
	return DEF_PDI_LEADER_SHARE == g_PDI_Type
end

function ToggleDistributionUI()
	g_PDI_Type = g_Party:GetItemOption()
	
	local isOpen = false
	if IsLeaderShareDistributionItemOption() then 
		if OPEN_PARTYDISTRIBUTION_DICE == true or OPEN_PARTYDISTRIBUTION_MEMBER == true then
			isOpen = true
		end
		
	elseif IsDiceDistributionItemOption() then
		if OPEN_PARTYDISTRIBUTION_DICE == true then
			isOpen = true
		end
		
	end
	
	if isOpen == true then
		CloseWidget(WIDGET_ID_PARTYDISTRIBUTION_DICE)
		CloseWidget(WIDGET_ID_PARTYDISTRIBUTION_MEMBER)
	else
		OpenWidget(WIDGET_ID_PARTYDISTRIBUTION_DICE)
		if IsLeaderShareDistributionItemOption() then
			OpenWidget(WIDGET_ID_PARTYDISTRIBUTION_MEMBER)
		end
	end
end

function ClosePartyDistributionUI()
	CloseWidget(WIDGET_ID_PARTYDISTRIBUTION_DICE)
	if IsLeaderShareDistributionItemOption() then
		CloseWidget(WIDGET_ID_PARTYDISTRIBUTION_MEMBER)
	end
end
