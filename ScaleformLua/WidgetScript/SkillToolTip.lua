HeaderToolTip = ""		-- 스킬 툴팁 헤드부분 내용 취합용 
BodyToolTip = ""		-- 스킬 툴팁 몸체부분 내용 취합용
g_SkillLearh = ""		-- 스킬 배움 여부
g_SkillLevel = 0		-- 스킬레벨 전역사용용
g_SkillMaxLevel = 0		-- 스킬 최대 레벨
g_MultiSkillUse = false	-- 멀티스킬 존재 여부
g_MultiSkill = false	-- 멀티스킬 인지 여부
g_LinkSkillUse = false	-- 멀티스킬 존재 여부
g_LinkSkill = false		-- 멀티스킬 인지 여부
g_DelaySkillUse = false	-- 지연 스킬 존재 여부
g_DelaySkill = false	-- 지연 스킬 인지 여부
g_SkillToolTip_PlusButton = false	-- + 버튼을 통하여 출력된것인지 여부
-----------------------------------------------------------CF_GetSkillTooltipHeader--------------------------------------------------------
-- 스킬툴팁 헤더

function SkillToolTipHeader( tbHeader )

	if nil == tbHeader then
		return ""
	end

	-- 스킬 이름
	local HeaderToolTip = SkillToolTip_Header_Name( tbHeader["tbName"] )
	
	-- 버프슬롯에 대한 툴팁일 경우 스킬 이름과 레벨만 출력
	if tbHeader["SlotType"] == SLOT_BUFF then
		return HTMLFontColor( HeaderToolTip, HTML_TEXT_COLOR_GOLD )
	end
	
	-- 스킬 레벨
	-- 스킬 등급
	-- 스킬 속성 입력(도구의 종속, 화염, 냉기 등)
	local HeaderToolTip = string.format( "%s%s%s%s", 
		HeaderToolTip,
		SkillToolTip_Header_Level( tbHeader["tbLevel"] ),
		HTMLFontColorSize( SkillToolTip_Header_Grade( tbHeader["Grade"] ) , HTML_TEXT_COLOR_WHITE, 12),
		HTMLFontColorSize( SkillToolTip_Header_Attribute( tbHeader["Attribute"] ) , HTML_TEXT_COLOR_WHITE, 12 ) )

	return HeaderToolTip
	
end

-- 스킬 ID / 이름
function SkillToolTip_Header_Name( tbName )

	return tbName[ 3 ] .. " "
	
end

-- 스킬 마스터/현재 레벨
function SkillToolTip_Header_Level( tbLevel )

	local HeaderToolTip = ""

	if tbLevel == nil then
		
		HeaderToolTip = "\n"
		return HeaderToolTip
		
	end

	-- 최대 레벨
	g_SkillMaxLevel = tbLevel[ 1 ]
	-- 현재 레벨
	g_SkillLevel = tbLevel[ 2 ]
	
	if g_SkillLevel > 0 then
	
		HeaderToolTip = "(" .. g_SkillLevel .. "/" .. g_SkillMaxLevel .. "Lv)\n"
		
	else
	
		g_SkillLearh = true
		HeaderToolTip = "\n"
		
	end
	
	return HeaderToolTip
	
end

-- 스킬 등급
function SkillToolTip_Header_Grade( Grade )
	local HeaderToolTip = ""		-- 헤더 툴팁 믹스용
	
	HeaderToolTip = Grade .. GetGameWordText("SKILL_BASIC_INFO", 1) .. "/ "
	return tostring(HeaderToolTip)
end
-- 스킬 속성
function SkillToolTip_Header_Attribute( Attribute )
	local HeaderToolTip = ""		-- 헤더 툴팁 믹스용
	
	HeaderToolTip = GetGameWordText("SKILL_ATTRIBUTE_ELEMENT", tonumber(Attribute))
	return tostring(HeaderToolTip)
end

-- 개행 문자 삽입시 혹시 새라인이 추가되어있다면 지운다
-- 추가 툴팁에선 "\r\n"이 먹히지않으므로 "\n\n"으로 간다
function SkillTooltip_AddCarriageReturn( strTooltip )
	local nEnd = string.len(strTooltip)
	local strCopy = strTooltip
	while nEnd > 0 and (string.sub(strCopy, nEnd, nEnd) == "\n" or string.sub(strCopy, nEnd, nEnd) == "\r") do
		strCopy = string.sub(strCopy, 0, nEnd-1)
		nEnd = string.len(strCopy)
	end
	return strCopy.."\n\n"
end

function SkillTooltip_AddCarriage_String( strTooltip, strAddString )
	if strAddString:Validation() == false then
		return strTooltip
	end
	return SkillTooltip_AddCarriageReturn(strTooltip)..strAddString
end


-----------------------------------------------------------CF_GetSkillTooltip--------------------------------------------------------
-- 스킬툴팁 몸체
-- 맥락이 바뀌는 부분에는 \r\n을 쓰는게맞으나 추가 효과 출력시 \r\n을 \n으로 인식하는것같다;
-- \r\n을 \n\n으로 바꿈     지 맛대로이다.. 텍스트 박스가 분리되어있는듯..;
-- 개행관련해서 복잡하다
function SkillToolTipBody( tbSkillData )

	if nil == tbSkillData then
		return ""
	end
	
	-- 슬롯 타입이 버프일 경우 설명만 출력
	if tbSkillData["SlotType"] == SLOT_BUFF then
		return BuffSkillSlot( tbSkillData["Explanation"] )
	end
	
	local BodyToolTip = ""
	
	-- 액티브
	if 0 == tbSkillData["Type"] then
	
		BodyToolTip = SkillToolTip_Body_ActionType( tbSkillData["ActionType"] ) .. 
			SkillToolTip_Body_Apply( tbSkillData["Apply"] ) .. 
			SkillToolTip_Body_Disguise( tbSkillData["DisguiseSkill"] )
		
	-- 패시브
	elseif 1 == tbSkillData["Type"] then
	
		BodyToolTip = GetGameWordText("SKILL_SLOT", 1).."\n"
		
	end

	-- 스킬 사용 조건(소모, 필요아이템 등등), 서브 스킬 툴팁에서는 나타내줄 필요없음
	local strBodyTooltip = ""
	if tbSkillData["ConditionCheck"] == true then
	
		strBodyTooltip = AddNewLine( strBodyTooltip, SkillToolTip_Body_Item( tbSkillData["tbItem"] ) )
		strBodyTooltip = AddNewLine( strBodyTooltip, SkillToolTip_Body_UseCondition( tbSkillData["tbUseCondition"] ) )
		strBodyTooltip = AddNewLine( strBodyTooltip, SkillToolTip_Body_Delay( tbSkillData["tbBasicAttribute"] ) )
	
		-- 스킬창 슬롯이거나 다음 레벨 툴팁용일때만 습득 조건 출력
		if  SLOT_SKILL == tbSkillData[ "SlotType" ] or 
			true == g_SkillToolTip_PlusButton then
			
			if 0 ~= string.len( strBodyTooltip ) then
				strBodyTooltip = strBodyTooltip .. "\n"
			end
				
			-- 습득 조건
			strBodyTooltip = AddNewLine( strBodyTooltip, SkillTooltip_GetRequire( tbSkillData ) )
				
			-- 습득시 게임 머니 소모는 1레벨일때만 
			--if 1 == g_SkillLevel then
				strBodyTooltip = AddNewLine( strBodyTooltip, SkillTooltip_GetMoney( tbSkillData[ "tbDemandMoney" ][ 2 ] ) )
			--end
			
		end
	end
	
	if strBodyTooltip:Validation() == true then
		BodyToolTip = BodyToolTip .. strBodyTooltip
	end

	-- 스킬 영역, 효과, 설명, 발동조건, 지연효과
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, SkillToolTip_Body_Range( tbSkillData ))
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, SkillToolTip_Body_Effect_Total( tbSkillData ))
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, tbSkillData["Explanation"])
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, SkillToolTip_BlowText( tbSkillData["tbBlow"] ))
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, SkillToolTip_LinkSpecialEffect( tbSkillData ))
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, SkillToolTip_DelayActionSkill( tbSkillData ))
	BodyToolTip = SkillTooltip_AddCarriage_String(BodyToolTip, SkillToolTip_ActiveSkill( tbSkillData ))
	
	if tbSkillData["ConditionCheck"] == true then
	
		if g_SkillToolTip_PlusButton == true then
	
			BodyToolTip = BodyToolTip .. "\n \n"  .. HTMLFontColor( GetGameInternalText("RNSKILL_CTRLCLICK_TOOLTIP" ), HTML_TEXT_COLOR_ORANGE )
		
		end
		
	end
	
	return BodyToolTip
end
-- 버프슬롯일때 몸체 출력(스킬 설명 부분을 출력한다.)
function BuffSkillSlot( Explanation )	
	return Explanation
end

-- 스킬 효과 타입
function SkillToolTip_Body_ActionType( ActionType )
	local BodyToolTip_Lower = ""
	BodyToolTip_Lower = GetGameWordText("SKILL_TOOLTIP_BASIC_TYPE", ActionType + 1)
	return BodyToolTip_Lower
end
-- 스킬 계열 타입
function SkillToolTip_Body_Apply( Apply )
	local BodyToolTip_Lower = ""
	
	BodyToolTip_Lower = "/ " .. GetGameWordText("SKILL_TOOLTIP_BASIC_TYPE", Apply + 5)
	return BodyToolTip_Lower
end

-- 스킬 사용 무기 ## 극강부 글러브 출력 되도록 만들어야 함.(히든웨폰)
function SkillToolTip_Body_Item( tbItem )
	local BodyToolTip_Lower = ""
	
	-- tbItem[1] : 왼손(현재 사용하지 않음), tbItem[2] : 오른손, tbItem[3] : 히든웨폰
	if 25 ~= tbItem[2] then
		BodyToolTip_Lower = GetGameWordText("SKILL_BASIC_INFO", 9) .. " : " .. GetGameWordText("SKILL_ADVANCED_USE_ITEM", tbItem[2])
	end
	
	if 25 ~= tbItem[3] and 0 ~= tbItem[3] then
		BodyToolTip_Lower = BodyToolTip_Lower .. ", " .. GetGameWordText("SKILL_ADVANCED_USE_ITEM", tbItem[3])
	elseif 25 == tbItem[3] then
		BodyToolTip_Lower = BodyToolTip_Lower
	end
	if BodyToolTip_Lower:Validation() == false then
		return ""
	end
	return BodyToolTip_Lower--.."\n"
end
-- 소모조건(현재 베터리,파티 HP/MP/SP 는 사용하지 않음)
function SkillToolTip_Body_UseCondition( tbUseCondition )
	local BodyToolTip_Lower = ""
	local UseCount = 0
	
	-- 소모품은 이제 사용하지 않는다 ( 기획변경, 2016. 09. 19 )
	-- UseFeatures 로 변경하여 주석을 제거한다
	if 0 < tbUseCondition[1] then	-- 화살
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 0) .. "(" .. tbUseCondition[1] .. ")  "
	end
	if 0 < tbUseCondition[2] then	-- 부적
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 1) .. "(" .. tbUseCondition[2] .. ")  "
	end
	if 0 < tbUseCondition[3] then	-- 총알
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 7) .. "(" .. tbUseCondition[3] .. ")  "
	end
	if 0 < tbUseCondition[4] then	-- 경험치
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 2) .. "(" .. tbUseCondition[4] .. ")  "
	end
	if 0 < tbUseCondition[5] then	-- HP
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 3) .. "(" .. tbUseCondition[5] .. ")  "
	end
	if 0 < tbUseCondition[6] then	-- MP
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 4) .. "(" .. tbUseCondition[6] .. ")  "		
	end
	if 0 < tbUseCondition[7] then	-- SP
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 5) .. "(" .. tbUseCondition[7] .. ")  "	
	end
	if 0 < tbUseCondition[8] then	-- CP
		UseCount = 1
		BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_CDATA_CVL_BASIC_USE_VALUE", 6) .. "(" .. tbUseCondition[8] .. ")  "
	end
	
	if UseCount == 1 then
		BodyToolTip_Lower = GetGameWordText("SKILL_BASIC_INFO", 10) .. " : " .. BodyToolTip_Lower
	end
	
	if BodyToolTip_Lower:Validation() == false then
		return ""
	end
	
	return BodyToolTip_Lower
end
-- 딜레이
function SkillToolTip_Body_Delay( tbBasicAttribute )
	local BodyToolTip_Lower = GetGameWordText("SKILL_CDATA_CVL_BASIC", 0) .. " : "
	if 0 == tbBasicAttribute[2] then
		return ""
	end
	BodyToolTip_Lower = BodyToolTip_Lower .. string.format( "%.1f", tbBasicAttribute[2] ) .. GetGameWordText("WAITSERVER_TIMELEFT_UNIT", 0) --.. "\n"
	return BodyToolTip_Lower
end

-- 스킬 영역
function SkillToolTip_Body_Range( tbSkillData )

	local BodyToolTip_Lower = ""
	
	-- {(기본효과값), (딜레이(쿨타임)), (지속시간), (적용범위), (적용각도), (관통수), (타겟수)}
	local tbBasic_Attribute = tbSkillData["tbBasicAttribute"]
	
	-- 자신
	if 0 == tbSkillData["Target"] then
	
		-- '목표' 자신
		if 0 == tbSkillData["Area"] then
		
			BodyToolTip_Lower = BodyToolTip_Lower .. GetGameInternalText("SKILL_AREA_SELF_SELF", 0)
			
		-- '목표' 주위
		elseif 1 == tbSkillData["Area"] then
		
			-- 자기편에게
			if 0 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
				StringFormat( GetGameInternalText( "SKILL_AREA_SELF_ZONE_OUR", 0 ),
					SetFloatToString( tbBasic_Attribute[ 4 ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			-- 적에게
			elseif 1 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_SELF_ZONE_ENEMY", 0 ),
					SetFloatToString( tbBasic_Attribute[ 4 ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			-- 모두에게
			elseif 2 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_SELF_ZONE_ANYBODY", 0 ),
					SetFloatToString( tbBasic_Attribute[ 4 ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			end
			
		-- '목표' 주위 지속
		elseif 2 == tbSkillData["Area"] then
		
			--BodyToolTip_Lower = BodyToolTip_Lower .. "" .. ""

		-- '목표' 를 향한 부채꼴
		elseif 3 == tbSkillData["Area"] then
		
			--BodyToolTip_Lower = BodyToolTip_Lower .. "" .. ""
			
		end
		
	-- 타겟
	elseif 1 == tbSkillData["Target"] then
	
		-- '목표' 자신
		if 0 == tbSkillData["Area"] then
		
			-- 자기편에게
			if 0 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_SELF_OUR", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ) )
				
			-- 적에게
			elseif 1 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_SELF_ENEMY", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ) )
				
			-- 모두에게
			elseif 2 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_SELF_ANYBODY", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ) )
				
			end
			
		-- '목표' 주위
		elseif 1 == tbSkillData["Area"] then
		
			-- 자기편에게
			if 0 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_ZONE_OUR", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
					SetFloatToString( tbBasic_Attribute[ 4 ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			-- 적에게
			elseif 1 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower ..
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_ZONE_ENEMY", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
					SetFloatToString( tbBasic_Attribute[ 4 ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			-- 모두에게
			elseif 2 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_ZONE_ANYBODY", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
					SetFloatToString( tbBasic_Attribute[ 4 ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			end
			
		-- '목표' 주위 지속
		elseif 2 == tbSkillData["Area"] then
		
			--BodyToolTip_Lower = BodyToolTip_Lower .. "" .. ""

		-- '목표' 를 향한 부채꼴
		elseif 3 == tbSkillData["Area"] then
		
			-- 적에게
			if 1 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_TARGET_FANWIZE_ENEMY", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
					tostring( tbBasic_Attribute[ 5 ] ),
					tostring( tbBasic_Attribute[ 7 ] ) )
				
			else
			
				--BodyToolTip_Lower = BodyToolTip_Lower .. "" .. ""

			end
			
		end
		
	-- 자신부터 타겟까지
	elseif 2 == tbSkillData["Target"] then
	
		-- 적에게
		if 1 == tbSkillData["Part"] then
		
			BodyToolTip_Lower = BodyToolTip_Lower .. 
				StringFormat( GetGameInternalText( "SKILL_AREA_TOSPEC_ENEMY", 0 ),
				SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
				tostring( tbBasic_Attribute[ 7 ] ),
				tostring( tbBasic_Attribute[ 6 ] ) )
			
		else
		
			--BodyToolTip_Lower = BodyToolTip_Lower .. "" .. ""

		end
		
	-- 지면위치 주변타겟
	elseif 3 == tbSkillData["Target"] then
	
		-- 자기편에게
		if 0 == tbSkillData["Part"] then
		
			BodyToolTip_Lower = BodyToolTip_Lower .. 
				StringFormat( GetGameInternalText( "SKILL_AREA_ZONE_OUR", 0 ),
				SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
				tostring( tbBasic_Attribute[ 7 ] ) )

		-- 적에게
		elseif 1 == tbSkillData["Part"] then
		
			BodyToolTip_Lower = BodyToolTip_Lower .. 
				StringFormat( GetGameInternalText( "SKILL_AREA_ZONE_ENEMY", 0 ),
				SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
				tostring( tbBasic_Attribute[ 7 ] ) )
			
		-- 모두에게
		elseif 2 == tbSkillData["Part"] then
		
			BodyToolTip_Lower = BodyToolTip_Lower .. 
				StringFormat( GetGameInternalText( "SKILL_AREA_ZONE_ANYBODY", 0 ),
				SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ),
				tostring( tbBasic_Attribute[ 7 ] ) )
			
		end
		
	-- 지면위치
	elseif 4 == tbSkillData["Target"] then
	
		-- 목표 주위
		if 0 == tbSkillData["Area"] then
		
			-- 자기편에게
			if 0 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_LAND_RANGE", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ) )
				
			end
			
		-- 목표 주위
		elseif 1 == tbSkillData["Area"] then
		
			-- 자기편에게
			if 0 == tbSkillData["Part"] then
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_LAND_RANGE", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ) )
				
			else
			
				BodyToolTip_Lower = BodyToolTip_Lower .. 
					StringFormat( GetGameInternalText( "SKILL_AREA_LAND_RANGE", 0 ),
					SetFloatToString( tbSkillData[ "Range" ] / 10, 1 ) )
				
			end
			
		else
		
			--BodyToolTip_Lower = BodyToolTip_Lower .. "" .. ""

		end
		
	end
	
	
	if 0 ~= string.len( BodyToolTip_Lower ) then
	
		BodyToolTip_Lower = GetGameWordText("SKILL_BASIC_INFO", 12) .. "\n" .. BodyToolTip_Lower
		
	end
	
	return BodyToolTip_Lower
	
end

-- 스킬 효과
function SkillToolTip_Body_Effect_Total( tbSkillData )

	-- 스킬 기본 효과
	local BasicVar = tbSkillData[ "tbBasicAttribute" ]
	-- 스킬 슬롯 타입(WidgetDef.lua 파일 참고)
	local SkillSlotType = tbSkillData[ "SlotType" ]
	-- 추가효과
	local tbSideEffect = tbSkillData[ "tbSideEffect" ]
	-- 특수효과-반격
	local tbSpecialEffectCA = tbSkillData[ "tbSpecialEffectCA" ]
	
	local strRet = ""
	
	local strApplyType = SkillToolTip_Body_Effect_ApplyType( tbSkillData[ "ApplyType" ] )
	if strApplyType ~= nil and strApplyType:Validation() then
	
		strRet = strRet .. "\n"  .. strApplyType
		
	end
	
	for i = 1, 5, 1 do
	
		local strSideEffect = SkillToolTip_Body_Effect_SideEffect( tbSideEffect[i], BasicVar )
		if strSideEffect ~= nil and strSideEffect:Validation() then
		
			strRet = string.format( "%s\n%s", strRet, strSideEffect )
			
		end
		
	end
	
	local tbSpecialEffStr = tbSkillData[ "tbSpecialEffectStr" ]
	for value in pairs( tbSpecialEffStr ) do
	
		if 0 ~= string.len( tbSpecialEffStr[ value ] ) then
		
			strRet = string.format( "%s\n%s", strRet, tbSpecialEffStr[ value ] )
		
		end
		
	end
	
	if 0 == string.len( strRet ) then
		return ""
	end
	
	strRet = HTMLFontColor( GetGameWordText( "SKILL_BASIC_INFO", 13 ) .. strRet , HTML_TEXT_COLOR_GREEN )

	return strRet
	
end

-- 스킬 일반 효과
function SkillToolTip_Body_Effect_ApplyType( ApplyType )
	
	if nil == ApplyType or 0 == string.len( ApplyType ) then
		return ""
	end
	
	return HTMLFontColor( ApplyType, HTML_TEXT_COLOR_GREEN )
	
end

-- 스킬 추가 효과
function SkillToolTip_Body_Effect_SideEffect( tbSideEffect, BasicVar )
	local BodyToolTip_Lower = ""
	local Fluctuation = ""			-- 증가/감소 텍스트 저장
	local Value = 0					-- 증가 값 저장
	local Rate = 0					-- 발생 확률 저장
	
	if nil == tbSideEffect then
		return ""
	end
	if 0 < tbSideEffect[2] then			-- 추가효과 값이 0 이상(증가)
		Fluctuation = GetGameInternalText("SKILL_APPLYTYPE_FLUCTUATION", 0)
		if 1 > tbSideEffect[2] then		-- 추가효과 값이 소수일때
			Value = SetFloatToString(tbSideEffect[2], 1)
		elseif 1 <= tbSideEffect[2] then	-- 추가효과 값이 정수일때
			Value = tonumber(SetFloatToString(tbSideEffect[2],1))
		end
	elseif 0 > tbSideEffect[2] then						-- 추과효과 값이 0 이하(감소)
		Fluctuation = GetGameInternalText("SKILL_APPLYTYPE_FLUCTUATION", 1)
		if -1 > tbSideEffect[2] then					-- 추가효과 값이 소수일 경우
			Value = math.abs(SetFloatToString(tbSideEffect[2], 1))		-- 값이 음수이므로 추후 표기를 위해 정수값으로 변경하여 넣어준다.
		elseif -1 <= tbSideEffect[2] then				-- 추가효과 값이 정수일 경우
			Value = SetFloatToString( tbSideEffect[2] * -1, 1)
		end
	end
	
	local fDurationTime = ""
	if BasicVar[3]%1 == 0 then		-- 자연수
		fDurationTime = SetFloatToString(BasicVar[3], 0)
	else
		fDurationTime = SetFloatToString(BasicVar[3], 1)
	end
	
	if 0 == tbSideEffect[1] then
		BodyToolTip_Lower = StringFormat(GetGameInternalText("SKILL_SIDEEFFECT_INDEXFORTEXT", SetFloatToString(tbSideEffect[1],1)), GetGameWordText("SKILL_SIDEEFFECT_INDEX", SetFloatToString(tbSideEffect[1],1)), Value, tbSkillDamage[1], tbSkillDamage[2], Fluctuation) --.. "\n"	-- 내용
	else
		if 0 < BasicVar[3] then
			BodyToolTip_Lower = StringFormat(GetGameInternalText("SKILL_APPLYTYPE_DURATIONTIME", 0), fDurationTime) --.. " " 		-- 지속시간
			.. StringFormat(GetGameInternalText("SKILL_SIDEEFFECT_INDEXFORTEXT", SetFloatToString(tbSideEffect[1],1)), GetGameWordText("SKILL_SIDEEFFECT_INDEX", SetFloatToString(tbSideEffect[1],1)), Value, Fluctuation) --.. "\n"	-- 내용
		elseif 0 == BasicVar[3] then
			BodyToolTip_Lower = StringFormat(GetGameInternalText("SKILL_SIDEEFFECT_INDEXFORTEXT", SetFloatToString(tbSideEffect[1],1)), GetGameWordText("SKILL_SIDEEFFECT_INDEX", SetFloatToString(tbSideEffect[1],1)), Value, Fluctuation) --.. "\n"
		end		
	end
	
	return tostring( BodyToolTip_Lower )
end

-- 스킬속성(마비, 기절, 석화, 화염, 냉동, 착란, 중독, 저주)
function SkillToolTip_dwFlagVal( dwFlag )
	local BodyToolTip_Lower = ""
	local Num = dwFlag
    
    for i = 1, 8, 1 do
		if 0 == Num then
			return BodyToolTip_Lower
		elseif Num % 2 == 1 or Num == 1 then
			if "" == BodyToolTip_Lower then
				BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("PRIVATE_MARKET_ITEM_STATE_BLOW", i)
			else
				BodyToolTip_Lower = BodyToolTip_Lower .. ", " .. GetGameWordText("PRIVATE_MARKET_ITEM_STATE_BLOW", i)
			end		
		end
		Num = math.floor( Num / 2 )
	end
	
	return BodyToolTip_Lower
end

function SkillToolTip_BlowText(tbBlow)
	if nil == tbBlow then
		return ""
	end
	
	-- emBLOW의 값이된다;
	local EMBLOW_NONE		= 0	--	상태 이상 유발 없음.
	local EMBLOW_NUMB		= 1	--	마비.
	local EMBLOW_STUN		= 2	--	기절.
	local EMBLOW_STONE		= 3	--	석화.
	local EMBLOW_BURN		= 4	--	화염.
	local EMBLOW_FROZEN		= 5	--	냉동.
	local EMBLOW_SINGLE		= 5	--	단독으로만 발생하는 "상태이상" 효과.
	local EMBLOW_MAD		= 6	--	착란.
	local EMBLOW_POISON		= 7	--	중독.
	local EMBLOW_CURSE		= 8	--	저주.
	local EMBLOW_PANT		= 9	--	헐떡임.
	local EMBLOW_SIZE		= 10
	local EMBLOW_MULTI		= 4
	
	local strText = ""
	local emBLOW = tbBlow[0]
	local fLife = tbBlow[1]
	local fRate = tbBlow[2]
	local fVAR1 = tbBlow[3]
	local fVAR2 = tbBlow[4]
	
	local emStateBlow = 0
	-- emStateBlow 값이 된다;
	local VALUE_COUNT_0 = 0
    local VALUE_COUNT_1_VAR1 = 1
    local VALUE_COUNT_1_VAR2 = 2
    local VALUE_COUNT_2 = 3
	
	if fVAR1 ~= 0.0 then
		if fVAR2 ~= 0.0 then
			emStateBlow = VALUE_COUNT_2
		else
			emStateBlow = VALUE_COUNT_1_VAR1
		end
	else
		if fVAR2 ~= 0.0 then
			emStateBlow = VALUE_COUNT_1_VAR2
		end
	end
	if fVAR1 < 0.0 then
		fVAR1 = -fVAR1;
	end
	if fVAR2 < 0.0 then
		fVAR2 = -fVAR2;
	end
	
	if emBLOW == EMBLOW_NONE then
		return ""
	elseif emBLOW == EMBLOW_NUMB then			-- 마비
		if emStateBlow == VALUE_COUNT_2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_NUMB" ), fRate, fLife, SetFloatToString(fVAR1*100, 0) , SetFloatToString(fVAR2*100, 0)  )
		elseif emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_NUMB_1" ), fRate, fLife, fVAR1 , fVAR2  )
		elseif emStateBlow == VALUE_COUNT_1_VAR2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_NUMB_2" ), fRate, fLife, fVAR1 , fVAR2 )
		end
	elseif emBLOW == EMBLOW_STUN then		-- 기절
		strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_STUN" ), fRate, SetFloatToString(fLife, 1) )
	elseif emBLOW == EMBLOW_STONE then		-- 석화
		if emStateBlow == VALUE_COUNT_2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_STONE" ), fRate, fLife, fVAR1 , fVAR2 )
		elseif emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_STONE_1" ), fRate, fLife, fVAR1  )
		elseif emStateBlow == VALUE_COUNT_1_VAR2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_STONE_2" ), fRate, fLife, fVAR2 )
		end
	elseif emBLOW == EMBLOW_BURN then		-- 화염
		if emStateBlow == VALUE_COUNT_2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_BURN" ), fRate, fLife, fVAR1 , fVAR2)
		elseif emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_BURN_1" ), fRate, fLife, fVAR1 )
		elseif emStateBlow == VALUE_COUNT_1_VAR2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_BURN_2" ), fRate, fLife, fVAR2 )
		end
	elseif emBLOW == EMBLOW_FROZEN then		-- 냉동
		if emStateBlow == VALUE_COUNT_2 then
			strText = StringFormatPtEx(GetGameInternalText("DES_STATE_BLOW_FROZEN" ), fRate, SetFloatToString(fLife, 1), fVAR1*100.0 , fVAR2*100.0 )
			-- strText = string.format(GetGameInternalText("DES_STATE_BLOW_FROZEN" ), fRate, fLife, fVAR1 , fVAR2 )
		elseif emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormatPtEx(GetGameInternalText("DES_STATE_BLOW_FROZEN_1" ), fRate, fLife, fVAR1*100.0 )
			-- strText = string.format(GetGameInternalText("DES_STATE_BLOW_FROZEN_1" ), fRate, fLife, fVAR1 )
		elseif emStateBlow == VALUE_COUNT_1_VAR2 then
			strText = StringFormatPtEx(GetGameInternalText("DES_STATE_BLOW_FROZEN_2" ), fRate, fLife, fVAR2*100.0 )
			-- strText = string.format(GetGameInternalText("DES_STATE_BLOW_FROZEN_2" ), fRate, fLife, fVAR2 )
		end
	elseif emBLOW == EMBLOW_MAD then		-- 착란
		if emStateBlow == VALUE_COUNT_2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_MAD" ), fRate, fLife, fVAR1 , fVAR2 )
		elseif emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_MAD_1" ), fRate, fLife, fVAR1 )
		elseif emStateBlow == VALUE_COUNT_1_VAR2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_MAD_2" ), fRate, fLife, fVAR2 )
		end
	elseif emBLOW == EMBLOW_POISON then		-- 중독
		if emStateBlow == VALUE_COUNT_2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_POISON" ), fRate, fLife, fVAR1 , fVAR2 )
		elseif emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_POISON_1" ), fRate, fLife, fVAR1 )
		elseif emStateBlow == VALUE_COUNT_1_VAR2 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_POISON_2" ), fRate, fLife, fVAR2 )
		end
	elseif emBLOW == EMBLOW_CURSE then		-- 저..저주
		if emStateBlow == VALUE_COUNT_1_VAR1 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_CURSE" ), fLife, fRate, fVAR1 )
		elseif emStateBlow == VALUE_COUNT_0 then
			strText = StringFormat(GetGameInternalText("DES_STATE_BLOW_CURSE" ), fLife )
		end	
	else
		return ""
	end
	
	if strText == "" then
		return ""
	end
	strText = strText:replace("%%%%", "%")
	strText = strText:replace("%%", "%")
	strText = HTMLFontColor( strText, HTML_TEXT_COLOR_YELLOW )
	return strText
end

-- 발동 조건
function SkillToolTip_LinkSpecialEffect(tbSkillData)
	local SubSkillData = tbSkillData["SubSkill"]
	if SubSkillData[2] == nil then
		return ""
	end
	local tbSpecialEffect = tbSkillData["tbSpecialEffect"]
	local BodyToolTip_Lower = ""
	for i = 1, #tbSpecialEffect do
		local tbSpec = tbSpecialEffect[i]
		if tbSpec ~= nil then
			local dwSkillMID = tbSpec[7]
			local dwSkillSID = tbSpec[8]
			local dwLinkMID = tbSpec[9]
			local dwLinkSID = tbSpec[10]
			local fData = tbSpec[2]*100
			if IsValidNativeID(dwSkillMID, dwSkillSID) and IsValidNativeID(dwLinkMID, dwLinkSID) then
				BodyToolTip_Lower = BodyToolTip_Lower.."\n"..GetGameInternalText("SKILL_SPEC_LINK")
				BodyToolTip_Lower = StringFormat(BodyToolTip_Lower, GetSkillName(dwLinkMID, dwLinkSID), SetFloatToString(fData, 0), GetSkillName(dwSkillMID, dwSkillSID))
				-- strText = HTMLFontColor( strText, HTML_TEXT_COLOR_GOLD )
				BodyToolTip_Lower = BodyToolTip_Lower:replace("%%%%", "%")
				BodyToolTip_Lower = BodyToolTip_Lower:replace("%%", "%")
			end
		end
	end
	if BodyToolTip_Lower:Validation() == false then
		return ""
	end
	BodyToolTip_Lower = GetGameWordText("SKILL_BASIC_INFO", 14) .. BodyToolTip_Lower
	BodyToolTip_Lower = HTMLFontColor( BodyToolTip_Lower, HTML_TEXT_COLOR_GOLD )
	return BodyToolTip_Lower
end

-- 지연 스킬
function SkillToolTip_DelayActionSkill(tbSkillData)
	local SubSkillData = tbSkillData["SubSkill"]
	if SubSkillData[3] == nil then
		return ""
	end
	
	local tbDelaySkillData = SubSkillData[3]
	local tbBasicAtrb = tbSkillData["tbBasicAttribute"]
	local fLife = tbBasicAtrb[3]	
	local BodyToolTip_Lower = GetGameInternalText("SKILL_SPEC_ADDON_99")--StringFormat( GetGameInternalText("ALLIANCE_BATTLE_ASK"), 1, 2, 3)
	BodyToolTip_Lower = BodyToolTip_Lower:replace("\r\n", "\n")
	BodyToolTip_Lower = StringFormat(BodyToolTip_Lower, SetFloatToString(fLife, 1), GetSkillName(tbDelaySkillData[1], tbDelaySkillData[2]))
	
	if BodyToolTip_Lower:Validation() == false then
		return ""
	end
	if BodyToolTip_Lower:Validation() == true then
		BodyToolTip_Lower = HTMLFontColor( BodyToolTip_Lower, HTML_TEXT_COLOR_GOLD )
	end
	return BodyToolTip_Lower
end


-- 스킬타입(물리근접, 물리사격, 마법작용)
function SkillToolTip_ApplyText( dwFlag )
	local BodyToolTip_Lower = ""
	local Num = dwFlag
    
    for i = 1, 3, 1 do
		if 0 == Num then
			return BodyToolTip_Lower
		elseif Num % 2 == 1 or Num == 1 then		-- A % B == (A와 B를 나눈 나머지) 이다.
			if "" == BodyToolTip_Lower then
				BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_TOOLTIP_BASIC_TYPE", i + 4)
			else
				BodyToolTip_Lower = BodyToolTip_Lower .. ", " .. GetGameWordText("SKILL_TOOLTIP_BASIC_TYPE", i + 4)
			end		
		end
		Num = math.floor( Num / 2 )
	end
	
	return BodyToolTip_Lower
end
-- 증감 텍스트(증가, 감소)
function SkillToolTip_Fluctuation( Num )
	if 0 < Num then
		return GetGameInternalText("SKILL_APPLYTYPE_FLUCTUATION", 0)
	elseif 0 > Num then
		return GetGameInternalText("SKILL_APPLYTYPE_FLUCTUATION", 1)
	elseif 0 == Num then
		return ""
	end
end
-- 액션타입(일반, 버프, 디버프, 리미트)
function SkillToolTip_ActionType( dwFlag )
	local BodyToolTip_Lower = ""
	local Num = dwFlag
    
    for i = 1, 4, 1 do
		if 0 == Num then
			return BodyToolTip_Lower
		elseif Num % 2 == 1 or Num == 1 then		-- A % B == (A와 B를 나눈 나머지) 이다.
			if "" == BodyToolTip_Lower then
				BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_TOOLTIP_BASIC_TYPE", i)
			else
				BodyToolTip_Lower = BodyToolTip_Lower .. ", " .. GetGameWordText("SKILL_TOOLTIP_BASIC_TYPE", i)
			end		
		end
		Num = math.floor( Num / 2 )
	end
	
	return BodyToolTip_Lower
end
-- 낙인효과 타입(일반효과, 스킬효과)
function SkillToolTip_Brand( dwFlag )
	local BodyToolTip_Lower = ""
	local Num = dwFlag
    
    for i = 1, 2, 1 do
		if 0 == Num then
			return BodyToolTip_Lower
		elseif Num % 2 == 1 or Num == 1 then		-- A % B == (A와 B를 나눈 나머지) 이다.
			if "" == BodyToolTip_Lower then
				BodyToolTip_Lower = BodyToolTip_Lower .. GetGameWordText("SKILL_BRAND_INDEX", i)
			else
				BodyToolTip_Lower = BodyToolTip_Lower .. ", " .. GetGameWordText("SKILL_BRAND_INDEX", i)
			end		
		end
		Num = math.floor( Num / 2 )
	end
	
	return BodyToolTip_Lower

end
-- X%확률로 를 출력
function Probability( Num )	
	return StringFormat(GetGameInternalText("SKILL_APPLYTYPE_PROBABILITY", 0), SetFormatNumPoint( Num, 2)) .. " "
end


function SkillToolTip_Body_Disguise( Apply )
	local BodyToolTip_Lower = ""
		
	if Apply == 1 or Apply == 2 then
		BodyToolTip_Lower = "/ " .. HTMLFontColor( GetGameWordText("SKILL_TOOLTIP_DISGUISE_TYPE", Apply), HTML_TEXT_COLOR_YELLOW ).."\n"
	else
		BodyToolTip_Lower = "\n"
	end
	
	return tostring( BodyToolTip_Lower )
end

function SkillToolTip_ActiveSkill(tbSkillData)
	
	if tbSkillData == nil then return "" end 
	
	local basicVar		  = tbSkillData["tbBasicAttribute"]
	local tbSpecialEffect = tbSkillData["tbSpecialEffect"]
		
	if tbSpecialEffect == nil then return "" end
	
	-- 지속시간 -----------------------------------------------
	local fLife = basicVar[3]
	local fTime = ""
	
	if fLife % 1 == 0 then
		fTime = SetFloatToString( fLife, 0 )
	else
		fTime = SetFloatToString( fLife, 1 )
	end
	-----------------------------------------------------------
	
	local BodyToolTip_Lower = ""
	
	for i = 1, 5, 1 do
	
		local tEffect = tbSpecialEffect[ i ]
		
		if tEffect == nil or tEffect[1] ~= 110 then
			continue
		end
		
		local subNum       = 0
		local attackCnt    = tEffect[ 11 ]
		local damageValue  = tEffect[ 12 ]
		local strSkillname = GetSkillName( tEffect[ 7 ], tEffect[ 8 ] )
		
		if attackCnt > 0 and damageValue > 0 then
			subNum = 3 else subNum = 0
		end 

		if subNum == 0 and attackCnt > 0 then
			subNum = 1
		end
		
		if subNum == 0 and damageValue > 0 then
			subNum = 2
		end

		if  subNum == 0 then
		
			local strGameTxt = GetGameInternalText("SKILL_SPEC_ADDON_110", 0)
			strGameTxt = strGameTxt:replace("\r\n", "\n")
			
			BodyToolTip_Lower = BodyToolTip_Lower .. StringFormat(strGameTxt, fTime)
			
		elseif subNum == 1 then
		
			local strGameTxt = GetGameInternalText("SKILL_SPEC_ADDON_110_1", 0)
			strGameTxt = strGameTxt:replace("\r\n", "\n")
			
			BodyToolTip_Lower = BodyToolTip_Lower .. StringFormat(strGameTxt, fTime, attackCnt, strSkillname )
			
		elseif subNum == 2 then
		
			local strGameTxt = GetGameInternalText("SKILL_SPEC_ADDON_110_2", 0)
			strGameTxt = strGameTxt:replace("\r\n", "\n")
			
			BodyToolTip_Lower = BodyToolTip_Lower .. StringFormat(strGameTxt, fTime, damageValue, strSkillname )
			
		elseif subNum == 3 then
		
			local strGameTxt = GetGameInternalText("SKILL_SPEC_ADDON_110_3", 0)
			strGameTxt = strGameTxt:replace("\r\n", "\n")
			
			BodyToolTip_Lower = BodyToolTip_Lower .. StringFormat(strGameTxt, fTime, attackCnt, damageValue, strSkillname )
			
		end
		
		if BodyToolTip_Lower:Validation() == true then
		
			BodyToolTip_Lower = BodyToolTip_Lower .. "\n"
			
		end
		
	end
	
	if BodyToolTip_Lower:Validation() == true then
	
		BodyToolTip_Lower = GetGameWordText("ACTIVE_SKILL_TEXT", 0) .. "\n" .. BodyToolTip_Lower
		BodyToolTip_Lower = HTMLFontColor( BodyToolTip_Lower, HTML_TEXT_COLOR_GOLD )
		
	else
	
	    BodyToolTip_Lower = ""
		
	end
	
	return BodyToolTip_Lower
	
end

-- 스킬 요구 금액(요구 금액쪽에 배움여부도 있음)
function SkillTooltip_GetMoney( nMoney )

	local strMoney = ""	
	if 0 < nMoney then
	
		strMoney = GetGameWordText( "RNSKILL_SLOT_NEED", 1 ) .. " : " .. CurrencyFormat( nMoney )
		
		if nMoney > g_MyChar:GetInvenMoney() then
			strMoney = HTMLFontColor( strMoney, HTML_TEXT_COLOR_RED )
		end
		
	end
	
	return strMoney
	
end

function SkillTooltip_GetRequire( tbSkillData )

	local tbRequire = tbSkillData[ "Require" ]
	-- tbSkillData["DisguiseSkill"]    GLCharDefine:DISGUISE_
	-- 0:None, 1:Before, 2:After
	if nil == tbRequire or tbSkillData["DisguiseSkill"] == 2 then
		return ""
	end
	
	local strRequire = ""
	
	local bOnlyOnStats = tbRequire[ 11 ]

	local strOnlyStats_or =  " " .. GetGameWordText( "SKILL_ADVANCED_INFO_STATS_OR_TEXT") .. " "
	local bMakeOrCheck = false
	
	-- 스킬 포인트
	if 0 ~= tbRequire[ 2 ] then
	
		local strTemp = string.format( "%s %d", GetGameWordText( "RNSKILLWINDOW_SLOT", 2 ), tbRequire[ 2 ] )
		if tbRequire[ 2 ] > g_MyChar.nSkillPoint - Skill_GetUsingPoint() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end
		
		strRequire = strRequire .. strTemp

	end
	
	-- 레벨
	if 0 ~= tbRequire[ 1 ] then
	
		if 0 ~= string.len( strRequire ) then
			strRequire = strRequire .. "   "
		end
	
		local strTemp = string.format( "%s %d", GetGameWordText( "SKILL_ADVANCED_INFO", 3 ), tbRequire[ 1 ] )
		if tbRequire[ 1 ] > g_MyChar:GetLevel() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end
				
		strRequire = strRequire .. strTemp
	
	end
	
	-- 힘
	if 0 ~= tbRequire[ 3 ] then
	
		if 0 ~= string.len( strRequire ) and bMakeOrCheck == false then
			strRequire = strRequire .. "   "
		end
	
		if bOnlyOnStats == 1 then 
			if bMakeOrCheck == true then
				strRequire = strRequire .. strOnlyStats_or
			else
				bMakeOrCheck = true
			end
		end
	
		local strTemp = string.format( "%s %d", GetGameWordText( "SKILL_ADVANCED_INFO_STATS", 0 ), tbRequire[ 3 ] )
		if tbRequire[ 3 ] > g_MyChar:GetPow_Skill() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end	
		
		strRequire = strRequire .. strTemp
	
	end
	
	-- 민첩
	if 0 ~= tbRequire[ 6 ] then
	
		if 0 ~= string.len( strRequire ) and bMakeOrCheck == false then
			strRequire = strRequire .. "   "
		end
	
		if bOnlyOnStats == 1 then 
			if bMakeOrCheck == true then
				strRequire = strRequire .. strOnlyStats_or
			else
				bMakeOrCheck = true
			end
		end
	
		local strTemp = string.format( "%s %d", GetGameWordText( "SKILL_ADVANCED_INFO_STATS", 3 ), tbRequire[ 6 ] )
		if tbRequire[ 6 ] > g_MyChar:GetDex_Skill() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end
				
		strRequire = strRequire .. strTemp
	
	end
	
	-- 정신
	if 0 ~= tbRequire[ 5 ] then
	
		if 0 ~= string.len( strRequire ) and bMakeOrCheck == false then
			strRequire = strRequire .. "   "
		end

		if bOnlyOnStats == 1 then 
			if bMakeOrCheck == true then
				strRequire = strRequire .. strOnlyStats_or
			else
				bMakeOrCheck = true
			end
		end
		
		local strTemp = string.format( "%s %d", GetGameWordText( "SKILL_ADVANCED_INFO_STATS", 2 ), tbRequire[ 5 ] )
		if tbRequire[ 5 ] > g_MyChar:GetSpi_Skill() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end
		
		strRequire = strRequire .. strTemp
	
	end
	
	-- 체력
	if 0 ~= tbRequire[ 4 ] then
	
		if 0 ~= string.len( strRequire ) and bMakeOrCheck == false then
			strRequire = strRequire .. "   "
		end
	
		if bOnlyOnStats == 1 then 
			if bMakeOrCheck == true then
				strRequire = strRequire .. strOnlyStats_or
			else
				bMakeOrCheck = true
			end
		end
	
		local strTemp = string.format( "%s %d", GetGameWordText( "SKILL_ADVANCED_INFO_STATS", 1 ), tbRequire[ 4 ] )
		if tbRequire[ 4 ] > g_MyChar:GetStr_Skill() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end
		
		strRequire = strRequire .. strTemp
	
	end
	
	-- 근력
	if 0 ~= tbRequire[ 7 ] then
	
		if 0 ~= string.len( strRequire ) and bMakeOrCheck == false then
			strRequire = strRequire .. "   "
		end
	
		if bOnlyOnStats == 1 then 
			if bMakeOrCheck == true then
				strRequire = strRequire .. strOnlyStats_or
			else
				bMakeOrCheck = true
			end
		end
	
		local strTemp = string.format( "%s %d", GetGameWordText( "SKILL_ADVANCED_INFO_STATS", 5 ), tbRequire[ 7 ] )
		if tbRequire[ 7 ] > g_MyChar:GetSta_Skill() then
			strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
		end
				
		strRequire = strRequire .. strTemp
	
	end
	
	-- 선행 스킬
	if -1 ~= tbRequire[ 9 ] then
	
		if 0 ~= string.len( strRequire ) then
			strRequire = strRequire .. "   "
		end
	
		local strTemp = string.format( "%s - %s Lv. %d",
			GetGameWordText( "RNSKILLWINDOW_SLOT", 1 ), tbRequire[ 8 ], tbRequire[ 9 ] )
		
		local nSkillID = tbRequire[ 10 ]
		local tbSkill = g_tbSkill_TempSkillList[ tostring( nSkillID ) ]
		if nil ~= tbSkill then
		
			if tbRequire[ 9 ] > tbSkill[ "wLevel" ] then
				strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
			end
			
		else
		
			if tbRequire[ 9 ] > Skill_GetLearnSkillLevel( nSkillID ) then
				strTemp = HTMLFontColor( strTemp, HTML_TEXT_COLOR_RED )
			end
		
		end
		
		strRequire = strRequire .. strTemp
	
	end

	
	if 0 ~= string.len( strRequire ) then
		
		strRequire = string.format( "[%s]\n%s", GetGameWordText( "SKILL_BASIC_INFO", 11 ), strRequire )
		
	end
	
	return strRequire

end