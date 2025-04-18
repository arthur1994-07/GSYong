
-- 로드 완료
function LoadExtremeWeapon()

	SetString( WIDGET_ID_EXTREMEWEAPON, "binding", SLOT_EXTREMEWEAPON_ID, "slot" )
	SetBool( WIDGET_ID_EXTREMEWEAPON, "tooltipEnabled", false, "slot" )
	
end

-- 오픈
function OpenExtremeWeapon()

	-- 현재 착용중인 무기 설정
	ExtremeWeapon_SetItem()
	
	ExtremeWeapon_CheckWeapon()
	
end

-- 현재 착용중인 무기 설정
function ExtremeWeapon_SetItem()

	-- 극강부 아나면 필요 없다.
	if  CHARCLASS_IDX_EXTREAM_M ~= g_MyChar.nClassIndex and
		CHARCLASS_IDX_EXTREAM_W ~= g_MyChar.nClassIndex then
		
		CloseWidget( WIDGET_ID_EXTREMEWEAPON )
		return
		
	else
	
		-- 극강부(무기 슬롯)이거나 마술부(소환수 슬롯)일 경우 추가 슬롯에 의하여 버트 UI 위치가 밀려난다;
		g_nBuffNormal_OffsetX = GetInt( WIDGET_ID_EXTREMEWEAPON, "x", "" ) + GetInt( WIDGET_ID_EXTREMEWEAPON, "width", "" )
		BuffNormal_SetOffsetPosition()
		
	end
	
	-- 메인 무기
	local nSlot = CHARINFO_SLOT_RHAND
	
	-- 서브 무기 사용중?
	if true == g_MyChar:IsUseArmSub() then
		nSlot = CHARINFO_SLOT_RHAND_S
	end
	
	-- 현재 무기
	local tbItem = g_MyChar:GetPutOnItem( nSlot )
	
	-- 없으면 끈다;
	if false == tbItem[ 3 ] then
	
		CloseWidget( WIDGET_ID_EXTREMEWEAPON )
		return
		
	end
	
	-- 아이템 설정
	SetSlot_Item_NonStrengthen( SLOT_EXTREMEWEAPON_ID, tbItem[ 1 ], tbItem[ 2 ], 1 )
	-- 툴팁 아이템 이름 설정
	SetString( WIDGET_ID_EXTREMEWEAPON, "TooltipText", GetItemName( tbItem[ 1 ], tbItem[ 2 ] ), "slot" )

end

------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 극강부 무기 슬롯 체크
function ExtremeWeapon_Check()
	
	if false == IsOpenWidget( WIDGET_ID_EXTREMEWEAPON ) then

		OpenWidget( WIDGET_ID_EXTREMEWEAPON )
	
	else
	
		ExtremeWeapon_SetItem()
	
	end

end

function ExtremeWeapon_CheckWeapon()

	if 	CHARCLASS_IDX_EXTREAM_M ~= g_MyChar.nClassIndex and
		CHARCLASS_IDX_EXTREAM_W ~= g_MyChar.nClassIndex then
		return
	end

	local nWeaponType = g_MyChar:GetWeaponType()
	if 	ITEMATT_NOTHING == nWeaponType then
	
		g_nBuffNormal_OffsetX = g_nBuffNormal_OrigX
		BuffNormal_SetOffsetPosition()
		
	else
	
		-- 극강부(무기 슬롯)이거나 마술부(소환수 슬롯)일 경우 추가 슬롯에 의하여 버트 UI 위치가 밀려난다;
		g_nBuffNormal_OffsetX = GetInt( WIDGET_ID_EXTREMEWEAPON, "x", "" ) + GetInt( WIDGET_ID_EXTREMEWEAPON, "width", "" )
		BuffNormal_SetOffsetPosition()
		
	end

end