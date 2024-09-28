
-- 해당 데이터는 텍스쳐 데이터와 동기화가 되어야한다;
SUMMONSTATE_ICON_PATH = "skill_trick"
SUMMONSTATE_ICON_IDX_X			=	6
SUMMONSTATE_NONHIRE_ICON_IDX_Y	=	10
SUMMONSTATE_DIE_ICON_IDX_Y		=	11
SUMMONSTATE_IDLE_ICON_IDX_Y		=	12
SUMMONSTATE_ATTACK_ICON_IDX_Y	=	13

g_dwAction = -1
g_dHPRatio = 1

bSummonStateHire = false
bSummonStateOpen = false
bSummonStateLoad = false

g_fSummonState_ElspTime = 0.0


function LoadSummonState()

	bSummonStateLoad = true
	
	SetInt( WIDGET_ID_SUMMONSTATE, "maximum", 100, "HpBar" )
	SetString( WIDGET_ID_SUMMONSTATE, "binding", SLOT_SUMMONSTATE_ID, "slot" )
	SetEvent( WIDGET_ID_SUMMONSTATE, EVENT_WIDGET_CLOSE, "CloseSummonState", "", "" )
	
end

function UnLoadSummonState()

	bSummonStateLoad = false
	
end

function OpenSummonState()

	SummonState_CheckWeapon()
	
	bSummonStateOpen = true
	
	-- 기본 슬롯/툴팁 세팅;
	local strTooltipText = ""
	local nIconIdxY = -1
	if bSummonStateHire == true then
		strTooltipText = StringFormat( GetGameInternalText("SUMMON_STATE_TOOLTIP_HIRE_TEXT"), g_MyChar:GetSummonName())
		nIconIdxY = SUMMONSTATE_DIE_ICON_IDX_Y
	else
		strTooltipText = GetGameInternalText("SUMMON_STATE_TOOLTIP_NONHIRE_TEXT")
		nIconIdxY = SUMMONSTATE_NONHIRE_ICON_IDX_Y
	end
	
	-- 롤오버시 툴팁;
	SetString( WIDGET_ID_SUMMONSTATE, "TooltipText", strTooltipText, "slot" )
	-- 아이콘 설정;
	local strIcon = string.format( "%s_%d_%d.png", SUMMONSTATE_ICON_PATH, SUMMONSTATE_ICON_IDX_X, nIconIdxY )
	SetSlot_SummonState(SLOT_SUMMONSTATE_ID, strIcon, nIconIdxY, SUMMONSTATE_ICON_IDX_X)
	
	InsertFrameMove( SummonState_FrameMove )
end

function CloseSummonState()
	bSummonStateOpen = false
	RemoveFrameMove( SummonState_FrameMove )
	g_dwAction = -1
	
	-- 타이머 호출
	local tbSetTimer = 
	{
		time = 1000*WIDGETS_LIST[WIDGET_ID_SUMMONSTATE].wDeleteTime,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "UnLoadSummonState"
	} 
	SetTween( WIDGET_ID_SUMMONSTATE, tbSetTimer, "", "", "" )
end

function SummonState_SetVisible(bVisible, bHire)

	bSummonStateHire = bHire
	if bVisible == true then
		if bSummonStateLoad == false or bSummonStateOpen == false then
			OpenWidget(WIDGET_ID_SUMMONSTATE)
		end
	else
		if bSummonStateLoad == true and bSummonStateOpen == true then
			CloseWidget(WIDGET_ID_SUMMONSTATE)
		end
	end
end

function SummonState_UpdateSummonAction( dwAction, bForceUpdate )
	
	if false == bForceUpdate and g_dwAction == dwAction then
		return
	end
	
	local nICON_Y = 0
	local strTooltipText = ""

	if LUAGLAT_IDLE == dwAction then
		nICON_Y = SUMMONSTATE_IDLE_ICON_IDX_Y
		strTooltipText = GetGameInternalText("SUMMON_STATE_TOOLTIP_IDLE_TEXT")
		
	elseif LUAGLAT_ATTACK == dwAction or LUAGLAT_SKILL == dwAction then
		nICON_Y = SUMMONSTATE_ATTACK_ICON_IDX_Y
		strTooltipText = GetGameInternalText("SUMMON_STATE_TOOLTIP_FIGHT_TEXT")
		
	elseif LUAGLAT_DIE == dwAction then
		nICON_Y = SUMMONSTATE_DIE_ICON_IDX_Y
		strTooltipText = StringFormat( GetGameInternalText("SUMMON_STATE_TOOLTIP_HIRE_TEXT"), g_MyChar:GetSummonName())
		
	end

	g_dwAction = dwAction
	
	-- 롤오버시 툴팁;
	SetString( WIDGET_ID_SUMMONSTATE, "TooltipText", strTooltipText, "slot" )
	-- 아이콘 설정;
	local strIcon = string.format( "%s_%d_%d.png", SUMMONSTATE_ICON_PATH, SUMMONSTATE_ICON_IDX_X, nICON_Y )
	SetSlot_SummonState(SLOT_SUMMONSTATE_ID, strIcon, nICON_Y, SUMMONSTATE_ICON_IDX_X)
end

function SummonState_FrameMove( fElapsedTime )
	if g_dwAction == -1 then
		return
	end
	
	g_fSummonState_ElspTime = g_fSummonState_ElspTime + fElapsedTime
	if 0.1 > g_fSummonState_ElspTime then
		return
	end
	
	local tbSummonHPInfo = g_MyChar:GetSummonHPInfo()
	if tbSummonHPInfo[1] == nil or tbSummonHPInfo[2] == nil then
		return
	end
	local fRatio = tbSummonHPInfo[1]/tbSummonHPInfo[2]
	
	-- 비율이 같다면 세팅하지않는다;
	if g_dHPRatio == ratio then
		return
	end
	g_dHPRatio = fRatio
	SetDouble( WIDGET_ID_SUMMONSTATE, "value", fRatio*100, "HpBar" )
end

function SummonState_CheckWeapon()

	if 	CHARCLASS_IDX_TRICKER_M ~= g_MyChar.nClassIndex and
		CHARCLASS_IDX_TRICKER_W ~= g_MyChar.nClassIndex then
		CloseWidget( WIDGET_ID_SUMMONSTATE )
		return
	end

	if g_MyChar:GetWeaponType() ~= ITEMATT_TRICK_WHIP then
	
		g_nBuffNormal_OffsetX = g_nBuffNormal_OrigX
		BuffNormal_SetOffsetPosition()
	
		CloseWidget( WIDGET_ID_SUMMONSTATE )
		
	else
	
		-- 극강부(무기 슬롯)이거나 마술부(소환수 슬롯)일 경우 추가 슬롯에 의하여 버트 UI 위치가 밀려난다;
		g_nBuffNormal_OffsetX = GetInt( WIDGET_ID_SUMMONSTATE, "x", "" ) + GetInt( WIDGET_ID_SUMMONSTATE, "width", "" )
		BuffNormal_SetOffsetPosition()
		
	end

end