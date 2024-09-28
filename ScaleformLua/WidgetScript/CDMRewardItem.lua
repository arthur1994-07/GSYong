
rewardList = {}
function LoadCDMRewardItem()
end

function OpenCDMRewardItem(_rewardlist)
	if false == IsOpenWidget(WIDGET_ID_CDMREWARDITEM) then
		OpenWidget(WIDGET_ID_CDMREWARDITEM)
	end
	CDMRewardSlotInit()
	SetCDMRewardItemList(_rewardlist)
	UpdateCDMRewardItemList()
end

function CDMRewardSlotInit()
	for i = 0, CDMREWARDITEM_SLOT_TOTAL-1, 1 do
		local slotNameID = SLOT_CDM_REWARDITEM..tostring(i)
		local slotWidgetID = "RewardSlot"..tostring(i)..".ItemSlot"
		ResetSlot(slotNameID)
		SetString( WIDGET_ID_CDMREWARDITEM, "binding", slotNameID, slotWidgetID )
		SetBool( WIDGET_ID_CDMREWARDITEM, "holdable", false, slotWidgetID )
	end
end

function SetCDMRewardItemList(_rewardlist)
	-- 1. mid, 2. sid, 3. amount
	rewardList = _rewardlist
end

function UpdateCDMRewardItemList()
	local nSize = table.getn(rewardList)
	SetSlot_Item(SLOT_CDM_REWARDITEM.."0", rewardList[1], rewardList[2], rewardList[3])
	local szItemName = GetItemName(rewardList[1], rewardList[2])
	SetString(WIDGET_ID_CDMREWARDITEM, "htmlText", szItemName, "RewardSlot0.textField")
end