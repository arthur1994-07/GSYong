ITEM_USE_PREVIEW = 	true		-- 착용 불가가 하나라도 있을 경우 미리보기 사용 않함(가능 = true/ 불가 = false)
SlushAndSpace = " / "			-- 슬러시가 들어가야 하는 텍스트에 사용

g_MaxClassNum = 9				-- 부서 종류의 최대수(남녀구분안한 상태)(신규 부서가 나올때마다 업데이트 해 주어야함)

tbMultiItemToolTip_ID =
{
	ITEM_MULTI_TOOLTIP1, ITEM_MULTI_TOOLTIP2, ITEM_MULTI_TOOLTIP3, ITEM_MULTI_TOOLTIP4, ITEM_MULTI_TOOLTIP5,
	ITEM_MULTI_TOOLTIP6, ITEM_MULTI_TOOLTIP7, ITEM_MULTI_TOOLTIP8, ITEM_MULTI_TOOLTIP9, ITEM_MULTI_TOOLTIP10
}

-- 추가 슬롯 개수 (CF_GetTooltipAddSlotCount 통해 슬롯 툴팁에 visible on 할 슬롯 개수를 가져온다. 미리 저장해놓는다.)
g_nItemTooltip_AddSlotCount = 0
-- 추가 슬롯 아이템 (툴팁 텍스트 만드는 도중에 아이템 리스트가 설정된다. 아이템 링크 정보 읽어올 때 필요)
g_tbItemTooltip_AddSlotItem = {}
-- 아이템 교환 기능 관련 슬롯
g_nItemTooltip_TradeSlotCount = 0
-- 아이템 교환 기능 관련 텍스트
g_sItemToolTip_TradeTypeText = ""
-- 툴팁 헤더 출력용(이름,학원,부서,요구레벨,요구스텟)

-- 아이템 교환 저장용
g_tbCopty_ItemTooltip_TradeCondtion = {}

function ItemToolTip_Header( tbHeader )

	if nil == tbHeader then
		return ""
	end

	local Header_Name = tbHeader["Name"]	-- 아이템 이름
	local MiddleToolTip = ""				-- 요구수치 삽입용 변수(폰트 크기설정을 위해 해당 궂이 해당 변수를 사용함)
	local HeaderToolTip = ""	-- 헤더 툴팁 초기화
	ITEM_USE_PREVIEW = 	true -- 미리보기 가능여부 초기화

	-- 아이템 이름 및 강화차수가 최상단을 차지하지 않을 확률은 지극히 적으므로 따로 함수로 빼지 않았음.
	--if 0 ~= tbHeader["Grade"] then
	--	HeaderToolTip = "+" .. tostring( tbHeader["Grade"] ) .. " "	-- 현재 강화차수 삽입
	--end

	HeaderToolTip = HeaderToolTip .. Header_Name[1]					-- 아이템 이름 삽입
	HeaderToolTip = HTMLFontColor( HeaderToolTip, TranslateHTMLColor( Header_Name[2] ) ) .. HTML_NEWLINE	-- 아이템 등급별 색상 삽입

	MiddleToolTip = ItemToolTip_Header_School( tbHeader, MiddleToolTip )	-- 요구 학원 출력
	MiddleToolTip = ItemToolTip_Header_Class( tbHeader, MiddleToolTip )		-- 요구 부서 및 성별 출력
	MiddleToolTip = ItemToolTip_Header_Level( tbHeader, MiddleToolTip )		-- 요구 레벨 출력
	MiddleToolTip = ItemToolTip_Header_Stat( tbHeader, MiddleToolTip )		-- 요구 스텟 출력

	HeaderToolTip = HeaderToolTip .. HTMLFontColorSize(MiddleToolTip, "", 12)
	return tostring(HeaderToolTip)

end
-- 요구 학원
function ItemToolTip_Header_School( tbHeader, MiddleToolTip )
	local Header_School = tbHeader["School"]	-- 요구 학원

	if "ALL" == Header_School then	-- 모든학원일때
		MiddleToolTip = MiddleToolTip .. GetGameWordText("FULL_ACADEMY_NAME", 3)	-- 모든학원 텍스트 삽입
	else
		MiddleToolTip = MiddleToolTip .. Header_School								-- 요구학원 텍스트 삽입
	end

	return MiddleToolTip
end
-- 요구 부서 및 성별
function ItemToolTip_Header_Class( tbHeader, MiddleToolTip )
	local Header_Class = tbHeader["Class"]		-- 요구 부서
	local Header_Sex = tbHeader["Sex"]			-- 요구 성별
	local ClassSize = table.getn( Header_Class )	-- 클래스 삽입을 위한 임시 변수

	local ClassTextColor = ""
	local ClassCheck = false		-- 착용 가능한 장비인지를 체크하기 위한 변수
	if "ALL" == Header_Class[1] then	-- 모든 클레스일 경우
		ClassTextColor = ClassTextColor .. "/" .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 0)	-- 모든 부서 텍스트 삽입
		ClassCheck = true
	elseif g_MaxClassNum == ClassSize then
		ClassTextColor = ClassTextColor .. "/" .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 0)	-- 모든 부서 텍스트 삽입
		ClassCheck = true
	else
		for i = 1, ClassSize, 1 do
			if	nil ~= Header_Class[i] then		-- 클레스 값이 nil이 아닐경우
				if GetToolTipClassText( Header_Class[i] ) == GetItemToolTip_ClassText( g_MyChar.nClassIndex ) then
					ClassTextColor = ClassTextColor .. "/" .. GetToolTipClassText( Header_Class[i] )	-- 각 부서 텍스트 삽입
					ClassCheck = true
				else
					ClassTextColor = ClassTextColor .. "/" .. GetToolTipClassText( Header_Class[i] )	-- 각 부서 텍스트 삽입
				end
			end
		end
	end
	if true == ClassCheck then
		MiddleToolTip = MiddleToolTip .. HTMLFontColor( ClassTextColor, HTML_TEXT_COLOR_WHITE )	-- 적용된 멀티툴팁 변수에 삽입(착용가능-흰색)
	else
		MiddleToolTip = MiddleToolTip .. HTMLFontColor( ClassTextColor, HTML_TEXT_COLOR_RED )	-- 적용된 멀티툴팁 변수에 삽입(착용불가-붉은색)
	end


	-- 자신과 동일한 성별이 아닐경우
	if false == Header_Sex[ "SameSex" ] then

		-- 남자일 경우
		if true == Header_Sex[ "bMan" ] then

			MiddleToolTip = MiddleToolTip .. "/" ..
				HTMLFontColor( GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_SEX", 1), HTML_TEXT_COLOR_RED ) .. "\n"

		-- 여자일 경우
		elseif false == Header_Sex[ "bMan" ] then

			MiddleToolTip = MiddleToolTip .. "/" ..
				HTMLFontColor( GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_SEX", 2), HTML_TEXT_COLOR_RED ) .. "\n"

		end

		-- 미리보기 불가
		ITEM_USE_PREVIEW =  false

	-- 모두 혹은 자신과 동일한 성별일 경우
	else

		-- 모두일 경우
		if "ALL" == Header_Sex[ "bMan" ] then

			MiddleToolTip = MiddleToolTip .. "\n"

		-- 남자일 경우
		elseif true == Header_Sex[ "bMan" ] then

			MiddleToolTip = MiddleToolTip .. "/" ..
				GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_SEX", 1) .. "\n"	-- 남자

		-- 여자일 경우
		elseif false == Header_Sex[ "bMan" ] then

			MiddleToolTip = MiddleToolTip .. "/" ..
				GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_SEX", 2) .. "\n"	-- 여자

		end

	end

	return MiddleToolTip

end

-- 요구레벨
function ItemToolTip_Header_Level( tbHeader, MiddleToolTip )
	local Header_Level = tbHeader["Level"]		-- 요구 레벨
	
	local nEditedWearableLevel = Header_Level["EditedWearableLevel"]		-- 착용레벨 랜덤옵션 효과로 조정되어야할 레벨
	if nil == nEditedWearableLevel then
		nEditedWearableLevel = 0
	end
	
	if 0 < tonumber(Header_Level["LevelDW"]) then			-- 하한선이 0이상일 경우(하한선 : 해당 레벨 이하이면 불가)
		if 0 < tonumber(Header_Level["LevelUP"]) then		-- 상한선이 0이상일 경우(상한선 : 해당 레벨 이상이면 불가)
			if g_MyChar:GetLevel() < tonumber(Header_Level["LevelDW"]) or g_MyChar:GetLevel() > tonumber(Header_Level["LevelUP"]) then	-- 상한선 혹은 하한선을 만족하지 않을 경우
				local DwLevel = GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 1) .. " : " .. tostring(Header_Level["LevelDW"]) .. "~" .. tostring(Header_Level["LevelUP"])
				MiddleToolTip = MiddleToolTip .. HTMLFontColor( DwLevel, HTML_TEXT_COLOR_RED )
			else	-- 상한선과 하한선 모두 만족할 경우
				MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 1) .. " : " .. tostring(Header_Level["LevelDW"]) .. "~" .. tostring(Header_Level["LevelUP"])
			end
			
			-- 조정된 착용레벨 표시
			if nEditedWearableLevel ~= 0 then
				local strEditedWearableLevel = " (" .. tostring( Header_Level["LevelDW"] + nEditedWearableLevel ) .. "~" .. tostring( Header_Level["LevelUP"] + nEditedWearableLevel ) .. ")"
				MiddleToolTip = MiddleToolTip .. HTMLFontColor( strEditedWearableLevel, HTML_TEXT_COLOR_CYAN )
			end
		else		-- 하한선 존재할 경우
			if g_MyChar:GetLevel() < tonumber(Header_Level["LevelDW"]) then	-- 하한선을 만족하지 못할 경우
				MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 1) .. " : " .. HTMLFontColor( tostring(Header_Level["LevelDW"]), HTML_TEXT_COLOR_RED )
			else	-- 하한선을 만족할 경우
				MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 1) .. " : " .. tostring(Header_Level["LevelDW"])
			end
			
			-- 조정된 착용레벨 표시
			if nEditedWearableLevel ~= 0 then
				local strEditedWearableLevel = " (" .. tostring( Header_Level["LevelDW"] + nEditedWearableLevel ) .. ")"
				MiddleToolTip = MiddleToolTip .. HTMLFontColor( strEditedWearableLevel, HTML_TEXT_COLOR_CYAN )
			end
		end
	else		-- 하한선이 존재하지 않는 경우(상한선만 존재)
		if 0 < tonumber(Header_Level["LevelUP"]) then						-- 상한선이 0이상일 경우(상한선 : 해당 레벨 이상이면 불가)
			if g_MyChar:GetLevel() > tonumber(Header_Level["LevelUP"]) then	-- 상한선이 충족되지 못할 경우
				local DwLevel = GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 1) .. " : " .. tostring(Header_Level["LevelUP"]) .. "~"
				MiddleToolTip = MiddleToolTip .. HTMLFontColor( DwLevel, HTML_TEXT_COLOR_RED )
			else	--상한선을 만족할 경우
				MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 1) .. " : " .. tostring(Header_Level["LevelUP"])
			end
			
			-- 조정된 착용레벨 표시
			if nEditedWearableLevel ~= 0 then
				local strEditedWearableLevel = " (" .. tostring( Header_Level["LevelUP"] + nEditedWearableLevel ) .. "~)"
				MiddleToolTip = MiddleToolTip .. HTMLFontColor( strEditedWearableLevel, HTML_TEXT_COLOR_CYAN )
			end
		else	-- 상한선과 하한선 둘다 없을 경우(레벨 관련 데이터가 없을 경우)

		end
	end

	return MiddleToolTip
end
-- 요구스텟
function ItemToolTip_Header_Stat( tbHeader, MiddleToolTip )
	local Header_Stat = tbHeader["Stat"]		-- 요구 스텟
	local Header_Level = tbHeader["Level"]		-- 요구 레벨
	local DwStat_Char = Header_Stat["Char"]		-- 캐릭터 보유 스텟
	local DwStat_Item = Header_Stat["Item"]		-- 아이템 요구 스텟

	if 0 < tonumber(DwStat_Item["Pow"]) then
		if 0 < tonumber(Header_Level["LevelDW"]) or 0 < tonumber(Header_Level["LevelUP"]) then
			MiddleToolTip = MiddleToolTip .. "/"
		end
		if tonumber(DwStat_Char["Pow"]) < tonumber(DwStat_Item["Pow"]) then
			MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 2) .. " : " .. HTMLFontColor( tostring(DwStat_Item["Pow"]), HTML_TEXT_COLOR_RED )
		else
			MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 2) .. " : " .. tostring(DwStat_Item["Pow"])
		end
	end

	if 0 < tonumber(DwStat_Item["Dex"]) then
		if 0 < tonumber(DwStat_Item["Pow"]) then
			MiddleToolTip = MiddleToolTip .. "/"
		end
		if tonumber(DwStat_Char["Dex"]) < tonumber(DwStat_Item["Dex"]) then
			MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 3) .. " : " .. HTMLFontColor( tostring(DwStat_Item["Dex"]), HTML_TEXT_COLOR_RED )
		else
			MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 3) .. " : " .. tostring(DwStat_Item["Dex"])
		end
	end


	if 0 < tonumber(DwStat_Item["Spi"]) then
		if 0 < tonumber(DwStat_Item["Dex"]) then
			MiddleToolTip = MiddleToolTip .. "/"
		end
		if tonumber(DwStat_Char["Spi"]) < tonumber(DwStat_Item["Spi"]) then
			MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 4) .. " : " .. HTMLFontColor( tostring(DwStat_Item["Spi"]), HTML_TEXT_COLOR_RED )
		else
			MiddleToolTip = MiddleToolTip .. GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE", 4) .. " : " .. tostring(DwStat_Item["Spi"])
		end
	end

	return MiddleToolTip
end

---------------------------------------------------------------------------------------------------
-- 아이템 툴팁 출력부분

function ItemToolTip_Insert( tbItemData )
	
	-- 추가 슬롯 개수 초기화
	g_nItemTooltip_AddSlotCount = 0
	-- 추가 슬롯 아이템 테이블 초기화
	g_tbItemTooltip_AddSlotItem = {}
	-- 교환 아이템 슬롯갯수 및 텍스트 초기화
	g_nItemTooltip_TradeSlotCount = 0
	g_sItemToolTip_TradeTypeText = ""

	if nil == tbItemData then
		return ""
	end

	-- 아이템 종류 및 색변경 여부
	local ToolTip_TypeData = tbItemData["TypeData"]

	-- 미리보기 가능여부 초기화
	ITEM_USE_PREVIEW = 	true
	
	-- 아이템 등급
	local nItemLevel = ToolTip_TypeData["Level"] + 1
	local strItemLevel = ""
	strItemLevel = "["..GetGameWordText("ITEM_INFO_LEVEL", nItemLevel ).."] "

	-- 아이템 타입
	local strType = ""
	if true == ToolTip_TypeData["Weapon"] then

		strType = strItemLevel..GetGameWordText("ITEM_BASIC_INFO", 11) .. "(" .. tostring(ToolTip_TypeData["Type"]) .. ")"

		local nRealType = tonumber(ToolTip_TypeData["RealType"])
		if ITEMATT_WEAPON_STR == nRealType then
		
			strType = strType .. "(" .. GetGameWordText("ITEM_SHOP_SEARCH_STATUS", 1) .. ")"
			
		elseif ITEMATT_WEAPON_DEX == nRealType then
		
			strType = strType .. "(" .. GetGameWordText("ITEM_SHOP_SEARCH_STATUS", 0) .. ")"
		
		elseif ITEMATT_WEAPON_SPI == nRealType then
		
			strType = strType .. "(" .. GetGameWordText("ITEM_SHOP_SEARCH_STATUS", 2) .. ")"
		
		end
		
	else

		strType = strItemLevel..tostring(ToolTip_TypeData["Type"])

	end
	strType = HTMLFontColor( strType, TranslateHTMLColor( ToolTip_TypeData["LevelColor"] ) )

	-- 코스튬 이름
	local strCostume = ""

	-- 코스튬 아이템인가? (코스튬 표시는 아이템 타입과 한줄 표기)
	if nil ~= ToolTip_TypeData["DisguiseOk"] and true == ToolTip_TypeData["DisguiseOk"] then

		local strCostumeTemp = ""
		if true == ToolTip_TypeData[ "ColorChange" ] then

			strCostumeTemp = "(" .. GetGameWordText("PRODUCT_TYPE_STRING", 12) .. " " ..
				GetGameWordText( "ITEM_SUIT_COLOR_CHANGE", 0 ) .. ")"

		else

			strCostumeTemp = "(" .. GetGameWordText("PRODUCT_TYPE_STRING", 12) .. ")"

		end

		strCostumeTemp = HTMLFontColor( strCostumeTemp, HTML_TEXT_COLOR_PALEGREEN )
		strType = strType .. strCostumeTemp

	else

		-- 코스튬 입혔나? (코스튬 표시는 아이템 타입과 한줄 표기)
		local tbDisguise = ToolTip_TypeData[ "Disguise" ]
		if nil ~= tbDisguise then

			local strCostumeTemp = ""
			if true == tbDisguise[ "ColorChange" ] then

				strCostumeTemp = "(" .. GetGameWordText("PRODUCT_TYPE_STRING", 12) .. " " ..
					GetGameWordText( "ITEM_SUIT_COLOR_CHANGE", 0 ) .. ")"

			else

				strCostumeTemp = "(" .. GetGameWordText("PRODUCT_TYPE_STRING", 12) .. ")"

			end

			strCostumeTemp = HTMLFontColor( strCostumeTemp , HTML_TEXT_COLOR_PALEGREEN )
			strType = strType .. strCostumeTemp

			-- 코스튬 이름 (코스튬 입혔으면 코스튬 아이템 이름은 다음줄에 표기)
			local strName = tbDisguise[ "Name" ]
			if nil ~= strName then

				strCostume = HTMLFontColor( string.format( "%s : %s",
					GetGameWordText( "PRODUCT_TYPE_STRING", 12 ), strName ),
					HTML_TEXT_COLOR_PALEGREEN )

			end

		end

	end
	
	-- 상자 내용물 미리보기가 있을경우 해당 텍스트 출력
	local strPreview = ""
	if tbItemData["BoxPreview"] == true then
		strPreview = GetGameInternalText("HELP_SHOW_ITEM_TOOLTIP")
	end

	-- 아이템 착용이 가능한경우 아이템 착용 미리보기 텍스트 출력
	if tbItemData["IsSuitable"] == true then

		if 0 == string.len( strPreview ) then
			strPreview = GetGameWordText("ITEM_USEPREVIEW_RN")
		else
			strPreview = AddNewLineFront( strPreview, GetGameWordText("ITEM_USEPREVIEW_RN") )
		end

	end

	-- 세트 아이템일 경우 세트 아이템 구성보기 텍스트 출력
	if nil ~= tbItemData[ "SetItemName" ] then

		if 0 == string.len( strPreview ) then
			strPreview = GetGameInternalText( "ITEM_INFO_SET_PREVIEW_TOOLTIP", 0 )
		else
			strPreview = AddNewLineFront( strPreview, GetGameInternalText( "ITEM_INFO_SET_PREVIEW_TOOLTIP", 0 ) )
		end

	end

	-- 수량
	local strCount = ""
	if false == ToolTip_TypeData["PutOn"] then

		strCount = ItemToolTip_Ability_StackNum( tbItemData["Num"] )

		if 0 == string.len( strCount ) then
			strCount = ItemToolTip_Type_Special( tbItemData )
		else
			strCount = AddNewLineFront( strCount, ItemToolTip_Type_Special( tbItemData ) )
		end

	end

	-- 내구도
	local strDurability = ItemToolTip_Durability( tbItemData["Durability"] )
	-- 기본옵션
	local strBasicOp = ItemToolTip_Ability_Basic( tbItemData )
	-- 랜덤옵션
	local strRandomOp = ItemToolTip_Ability_RandomOption( tbItemData["tbRandOpt"] )
	
	-- 가산옵션
	local strAddOp = ItemToolTip_Ability_AddOption( tbItemData["AddOpt"] )
	
	-- 특수옵션 (특수 옵션은 가산옵션에 문단 나누지 않고 바로 다음 줄에 출력한다.)
	--[[
	if 0 == string.len( strAddOp ) then
		strAddOp = ItemToolTip_Ability_SpecialOption( tbItemData["SpecialOpt"] )
	else
		strAddOp = AddNewLineFront( strAddOp, ItemToolTip_Ability_SpecialOption( tbItemData["SpecialOpt"] ) )
	end
	]]--

	-- 경험치 배율
	local strExp = ItemToolTip_Ability_ExpMultiple( tbItemData["ExpMultiple"] )
	-- 저항
	local strResist = ItemToolTip_Ability_Resist( tbItemData )
	-- 상태이상
	local strStateBlow = ItemToolTip_Ability_StateBlow( tbItemData["StateBlow"] )
	-- 스킬 링크
	local strSkillLink = ItemToolTip_SkillLink( tbItemData )
	-- 세티 아이템
	local strSetItem = ItemToolTip_Ability_SetItem( tbItemData )
	-- 아이템 설명
	local strExplainEx = ItemToolTip_Ability_ExplainEx( tbItemData["ExplainEx"] )
	-- 아이템 설명
	local strExplain = ItemToolTip_Ability_Explain( tbItemData["Explain"] )
	-- 아이템 거래설정
	local strTradeCondition = ItemToolTip_Ability_TrCond( tbItemData["TrCond"] )
	-- 특별활동
	local strActPoint = ItemToolTip_Ability_ActPoint( tbItemData["ActPoint"] )
	-- 기여도
	local strContribution = ItemToolTip_Ability_Contribution( tbItemData["Contribution"] )
	-- 남은 기간
	local strTime = ItemToolTip_Ability_Time( tbItemData["Time"], ToolTip_TypeData )
	-- 아이템 가격(구매/판매/구매아이템/구매생활점수 등)
	local strPrice = ItemToolTip_Ability_Price( tbItemData, tbItemData["TrCond"] )
	-- 귀환 위치
	local strDrugCall = ItemTooltip_DrugCall( tbItemData[ "tbDrugCall" ] )
	-- 위시 리스트
	local strWishList = ItemTooltip_WishListInfo( tbItemData["WishList"] )

	local strTooltip = ""

	strTooltip = strType
	strTooltip = AddNewLineFront( strTooltip, strCostume )

	strTooltip = AddStringSection( strTooltip, strDrugCall )
	strTooltip = AddNewLineFront( strTooltip, strDrugCall )

	--strTooltip = AddStringSection( strTooltip, strCount )
	strTooltip = AddNewLineFront( strTooltip, strCount )

	--strTooltip = AddStringSection( strTooltip, strDurability )
	strTooltip = AddNewLineFront( strTooltip, strDurability )

	strTooltip = AddStringSection( strTooltip, strBasicOp )
	strTooltip = AddNewLineFront( strTooltip, strBasicOp )

	--strTooltip = AddStringSection( strTooltip, strRandomOp )
	strTooltip = AddNewLineFront( strTooltip, strRandomOp )

	strTooltip = AddStringSection( strTooltip, strAddOp )
	strTooltip = AddNewLineFront( strTooltip, strAddOp )

	--strTooltip = AddStringSection( strTooltip, strExp )
	strTooltip = AddNewLineFront( strTooltip, strExp )

	--strTooltip = AddStringSection( strTooltip, strResist )
	strTooltip = AddNewLineFront( strTooltip, strResist )

	strTooltip = AddStringSection( strTooltip, strStateBlow )
	strTooltip = AddNewLineFront( strTooltip, strStateBlow )

	--strTooltip = AddStringSection( strTooltip, strSkillLink )
	strTooltip = AddNewLineFront( strTooltip, strSkillLink )

	strTooltip = AddStringSection( strTooltip, strSetItem )
	strTooltip = AddNewLineFront( strTooltip, strSetItem )

	strTooltip = AddStringSection( strTooltip, strExplainEx )
	strTooltip = AddNewLineFront( strTooltip, strExplainEx )

	strTooltip = AddStringSection( strTooltip, strExplain )
	strTooltip = AddNewLineFront( strTooltip, strExplain )

	strTooltip = AddStringSection( strTooltip, strPreview )
	strTooltip = AddNewLineFront( strTooltip, strPreview )

	strTooltip = AddStringSection( strTooltip, strTradeCondition )
	strTooltip = AddNewLineFront( strTooltip, strTradeCondition )

	strTooltip = AddStringSection( strTooltip, strActPoint )
	strTooltip = AddNewLineFront( strTooltip, strActPoint )

	strTooltip = AddStringSection( strTooltip, strContribution )
	strTooltip = AddNewLineFront( strTooltip, strContribution )

	strTooltip = AddStringSection( strTooltip, strTime )
	strTooltip = AddNewLineFront( strTooltip, strTime )

	strTooltip = AddStringSection( strTooltip, strPrice )
	strTooltip = AddNewLineFront( strTooltip, strPrice )
	
	strTooltip = AddStringSection( strTooltip, strWishList )
	strTooltip = AddNewLineFront( strTooltip, strWishList )

	return strTooltip

end

-- 내구도
function ItemToolTip_Durability( Durability )

	local ToolTipText_Sum = ""

	-- 내구도 Max이 있을경우(내구도 사용시 Max값을 사용하지 않을 수 없으므로 해당 조건으로 검사)
	if nil ~= Durability["Max"] then

		-- 내구도 현재/최대 값(%)
		ToolTipText_Sum = string.format( "%s : %d/%d(%.0f%%)",
			GetGameWordText("ITEM_BASIC_INFO", 18),
			Durability["Now"],
			Durability["Max"],
			Durability["Percent"] )

		-- 내구도 상태변화에 따른 색상 삽입
		ToolTipText_Sum = HTMLFontColor( ToolTipText_Sum , TranslateHTMLColor( Durability["Color"] ) )

		return ToolTipText_Sum

	end

	return ""

end

-- 기본옵션
function ItemToolTip_Ability_Basic( tbItemData )

	local strOption = ""
	local strOptionTemp = ""

	-- 공격력
	strOption = ItemToolTip_Ability_Damage( tbItemData["Damage"] )

	-- 방어력
	strOptionTemp = ItemToolTip_Ability_Defence( tbItemData["Defence"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	-- 명중률
	strOptionTemp = ItemToolTip_Ability_HitRate( tbItemData["HitRate"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	-- 회피율
	strOptionTemp = ItemToolTip_Ability_AvoidRate( tbItemData["AvoidRate"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	-- 소모 Sp
	strOptionTemp = ItemToolTip_Ability_CostSp( tbItemData["CostSp"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	return strOption

end

-- 개조 UI 기본옵션
function ItemToolTip_Ability_Basic_Rebuild ( tbItemData )

	local strOption = ""
	local strOptionTemp = ""

	-- 공격력
	if ( false == tbItemData["Damage"]["bRebuild"] ) then
	
		strOption = ItemToolTip_Ability_Damage( tbItemData["Damage"] )
	
	end

	-- 방어력
	if ( false == tbItemData["Defence"]["bRebuild"] ) then
	
		strOptionTemp = ItemToolTip_Ability_Defence( tbItemData["Defence"] )
		if 0 == string.len( strOption ) then
			strOption = strOptionTemp
		elseif 0 ~= string.len( strOptionTemp ) then
			strOption = AddNewLineFront( strOption, strOptionTemp )
		end
		
	end
	
	-- 명중률
	strOptionTemp = ItemToolTip_Ability_HitRate( tbItemData["HitRate"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	-- 회피율
	strOptionTemp = ItemToolTip_Ability_AvoidRate( tbItemData["AvoidRate"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	-- 소모 Sp
	strOptionTemp = ItemToolTip_Ability_CostSp( tbItemData["CostSp"] )
	if 0 == string.len( strOption ) then
		strOption = strOptionTemp
	elseif 0 ~= string.len( strOptionTemp ) then
		strOption = AddNewLineFront( strOption, strOptionTemp )
	end

	return strOption
	
end

-- 공격력
function ItemToolTip_Ability_Damage( Damage )

	if ( true == Damage[ "bDontCreate" ] ) and ( true == Damage[ "RandomStat" ] ) then
		return GetGameWordText( "ITEM_ADVANCED_INFO", 0 ) .. " : " .. GetGameWordText( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 0 )
	end
	
	-- 공격력 데이터가 없다면 출력하지 않음
	if 	0 == Damage[ "Min" ] and 0 == Damage[ "Max" ] and 0 == Damage[ "Extra" ] and
		0 == Damage[ "Extra" ] and 0 == Damage[ "RndOpt" ] then
		return ""
	end

	local ToolTipText_Sum = ""
	local DamageMin = tostring( Damage[ "Min" ] )
	local DamageMax = tostring( Damage[ "Max" ] )

	-- 렌덤옵션 데미지 값이 있을 경우
	if 0 < Damage[ "RndOpt" ] then

		-- 렌덤옵션 데미지 값 삽입
		local InsertColor = "(+" .. tostring( Damage[ "RndOpt" ] ) .. ")"
		-- 최소 공격력에 렌덤옵션 색상 삽입
		DamageMin = DamageMin .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_CYAN )
		-- 최대 공격력에 렌덤옵션 색상 삽입
		DamageMax = DamageMax .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_CYAN )

	end

	-- 강화수치 데미지 값이 잇을 경우
	if 0 < Damage[ "Extra" ] then

		-- 강화수치 데미지 값 삽입
		local InsertColor = "(+" .. tostring( Damage[ "Extra" ] ) .. ")"
		-- 최소 공격력에 렌덤옵션 색상 삽입
		DamageMin = DamageMin .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_YELLOW )
		-- 최대 공격력에 렌덤옵션 색상 삽입
		DamageMax = DamageMax .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_YELLOW )

	end

	-- 최소~최대 공격력
	ToolTipText_Sum = GetGameWordText( "ITEM_ADVANCED_INFO", 0 ) .. " : " .. DamageMin .. " ~ " .. DamageMax

	return ToolTipText_Sum

end

-- 방어력
function ItemToolTip_Ability_Defence( Defence )

	local ToolTipText_Sum = ""
	
	if ( true == Defence[ "bDontCreate" ] ) and ( true == Defence[ "RandomStat" ] ) then
		return GetGameWordText( "ITEM_ADVANCED_INFO", 1 ) .. " : " .. GetGameWordText( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 0 )
	end

	-- 방어력 데이터가 없다면 출력하지 않음
	if 0 == Defence["Def"] and 0 == Defence["RndOpt"] and 0 == Defence["Extra"] then
		return ""
	end

	-- 방어력
	ToolTipText_Sum = GetGameWordText("ITEM_ADVANCED_INFO", 1) .. " : " .. tostring(Defence["Def"])

	-- 렌덤옵션 방어력 값이 있을 경우
	if 0 < Defence["RndOpt"] then

		-- 렌덤옵션 방어력 값 삽입
		local InsertColor = "(+" .. tostring(Defence["RndOpt"]) .. ")"
		-- 렌덤옵션 색상 삽입
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_CYAN )

	end

	-- 강화수치 방어력 값이 잇을 경우
	if 0 < Defence["Extra"] then

		-- 강화수치 방어력 값 삽입
		local InsertColor = "(+" .. tostring(Defence["Extra"]) .. ")"
		-- 강화수치 색상 삽입
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_YELLOW )

	end

	return ToolTipText_Sum

end
-- 명중률
function ItemToolTip_Ability_HitRate( HitRate )
	local ToolTipText_Sum = ""
	if 0 == HitRate["Hit"] and 0 == HitRate["RndOpt"] then
		return ""	-- 명중률 데이터가 없다면 출력하지 않음
	end
	ToolTipText_Sum = GetGameWordText("ITEM_ADVANCED_INFO", 3) .. " : " .. tostring(HitRate["Hit"])
	if 0 < HitRate["RndOpt"] then	-- 렌덤옵션 명중률 값이 있을 경우
		local InsertColor = "(+" .. tostring(HitRate["RndOpt"]) .. ")"
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_CYAN )
	end
	return ToolTipText_Sum
end
-- 회피율
function ItemToolTip_Ability_AvoidRate( AvoidRate )
	local ToolTipText_Sum = ""
	if 0 == AvoidRate["Avoid"] and 0 == AvoidRate["RndOpt"] then
		return ""	-- 명중률 데이터가 없다면 출력하지 않음
	end
	ToolTipText_Sum = GetGameWordText("ITEM_ADVANCED_INFO", 4) .. " : " .. tostring(AvoidRate["Avoid"])
	if 0 < AvoidRate["RndOpt"] then	-- 렌덤옵션 명중률 값이 있을 경우
		local InsertColor = "(+" .. tostring(AvoidRate["RndOpt"]) .. ")"
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_CYAN )
	end
	return ToolTipText_Sum
end
-- 소모 sp
function ItemToolTip_Ability_CostSp( CostSp )
	local ToolTipText_Sum = ""
	if 0 == CostSp["Sp"] and 0 == CostSp["RndOpt"] then
		return ""	-- 명중률 데이터가 없다면 출력하지 않음
	end
	ToolTipText_Sum = GetGameWordText("ITEM_ADVANCED_INFO", 7) .. " : " .. tostring(CostSp["Sp"])
	if 0 < CostSp["RndOpt"] then	-- 렌덤옵션 명중률 값이 있을 경우
		local InsertColor = "(+" .. tostring(CostSp["RndOpt"]) .. ")"
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( InsertColor , HTML_TEXT_COLOR_CYAN )
	end
	return ToolTipText_Sum
end

-- 랜덤옵션
function ItemToolTip_Ability_RandomOption( RandOpt )

	local strRandomOption = ""
	if nil == RandOpt then
	
		return strRandomOption;
		
	end
	
	local RandOpt_Type = RandOpt[ 1 ]
	local RandOpt_Data = RandOpt[ 2 ]
	local RandOpt_IsFloat = RandOpt[ 3 ]
	local bRandomOption = RandOpt[ 4 ]
	local wMinCount = RandOpt[ 5 ]
	local wMaxCount = RandOpt[ 6 ]
	local bDontCreate = RandOpt[ 7 ]
	
	local nSize = table.getn(RandOpt_Type)
	for i = 1, nSize, 1 do

		if 0 ~= RandOpt_Type[ i ] then
			--[[
			local strValue
			if true == RandOpt_IsFloat[i] and 0 == ItemTooltip_GetRandomOptPrintType( RandOpt_Type[ i ] ) then
				strValue = string.format( "%.2f", RandOpt_Data[ i ] / 100 )
			elseif true == RandOpt_IsFloat[i] then
				strValue = string.format( "%.2f", RandOpt_Data[ i ] )
			else
				strValue = tostring( RandOpt_Data[ i ] )
			end
			]]--
			
			local nValue = RandOpt_Data[ i ]
			
			local strValue = ""
			if true == RandOpt_IsFloat[i] then
				strValue = string.format( "%.2f", nValue )
			else
				strValue = tostring( nValue )
			end
			
			if 0 == ItemTooltip_GetRandomOptPrintType( RandOpt_Type[ i ] ) then

				if 0 ~= string.len( strRandomOption ) then
					strRandomOption = strRandomOption .. "\n"
				end

				if nValue >= 0 then
				
					local str = GetGameWordText( "ITEM_RANDOM_OPTION", RandOpt_Type[ i ] ) .. " : +" .. strValue .. "%" .. ItemTooltip_AddAssistantText(RandOpt_Type[ i ])
					strRandomOption = strRandomOption .. HTMLFontColor( str , HTML_TEXT_COLOR_CYAN )
				
				elseif nValue < 0 then
				
					local str = GetGameWordText( "ITEM_RANDOM_OPTION", RandOpt_Type[ i ] ) .. " : " .. strValue .. "%" .. ItemTooltip_AddAssistantText(RandOpt_Type[ i ])
					strRandomOption = strRandomOption .. HTMLFontColor( str , HTML_TEXT_COLOR_CYAN )
				
				end

			elseif 1 == ItemTooltip_GetRandomOptPrintType( RandOpt_Type[ i ] ) then

				if 0 ~= string.len( strRandomOption ) then
					strRandomOption = strRandomOption .. "\n"
				end

				if nValue >= 0 then
				
					local str = GetGameWordText( "ITEM_RANDOM_OPTION", RandOpt_Type[ i ] ) .. " : +" .. strValue .. ItemTooltip_AddAssistantText(RandOpt_Type[ i ])--[[tostring( RandOpt_Data[ i ] )]]--  .. ItemTooltip_AddAssistantText(RandOpt_Type[ i ])
					strRandomOption = strRandomOption .. HTMLFontColor( str , HTML_TEXT_COLOR_CYAN )
				
				elseif nValue < 0 then
				
					local str = GetGameWordText( "ITEM_RANDOM_OPTION", RandOpt_Type[ i ] ) .. " : " .. strValue .. ItemTooltip_AddAssistantText(RandOpt_Type[ i ])--[[tostring( RandOpt_Data[ i ] )]]--  .. ItemTooltip_AddAssistantText(RandOpt_Type[ i ])
					strRandomOption = strRandomOption .. HTMLFontColor( str , HTML_TEXT_COLOR_CYAN )
				
				end
				
			end

		end

	end
	
	if "" == strRandomOption then
	
		if true == bRandomOption then
		
			local strTemp = GetGameWordText( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 2 )
			strRandomOption = strRandomOption .. HTMLFontColor( string.format( strTemp, wMinCount, wMaxCount ), HTML_TEXT_COLOR_CYAN )
			
		end
		
	end
	
	return strRandomOption

end

-- 랜덤 옵션 출력 타입 (0 - 소수, 1 - 정수)
function ItemTooltip_GetRandomOptPrintType( RandOptTypeNum )

	if 	1 == RandOptTypeNum or		-- 공격력 비율
		2 == RandOptTypeNum or		-- 방어력 비율
		3 == RandOptTypeNum or		-- 명중률
		4 == RandOptTypeNum or		-- 회피율
		8 == RandOptTypeNum or		-- hp 회복 비율
		9 == RandOptTypeNum or		-- mp 회복 비율
		10 == RandOptTypeNum or		-- sp 회복 비율
		11 == RandOptTypeNum or		-- hp, mp, sp 회복 비율
		14 == RandOptTypeNum or		-- 공격 영역 비율
		17 == RandOptTypeNum or		-- 이동 속도 비율
		(59 <= RandOptTypeNum and RandOptTypeNum <= 83) or -- 등급 스킬 데미지 증가율
		109	== RandOptTypeNum or	-- 크리티컬 데미지
		110	== RandOptTypeNum or	-- 강한타격 데미지
		112 == RandOptTypeNum or
		113 == RandOptTypeNum or
		114 == RandOptTypeNum or
		115 == RandOptTypeNum or
		116 == RandOptTypeNum or
		117 == RandOptTypeNum or
		118 == RandOptTypeNum or
		119 == RandOptTypeNum or
		120 == RandOptTypeNum then	

		return 0

	else

		return 1

	end

end

-- 랜덤 옵션 출력 타입
function ItemTooltip_AddAssistantText( RandOptTypeNum )

	if 	34 <= RandOptTypeNum and RandOptTypeNum <= 58 then

		return GetGameWordText( "ITEM_BASIC_TIME", 2 )

	end
	
	return ""
	
end

-- 가산옵션
function ItemToolTip_Ability_AddOption( tbAddOpt )

	local strAddOn = ""
	if tbAddOpt == nil then
		return ""
	end
	
	local nSize = table.getn( tbAddOpt )
	for i = 1, nSize, 1 do
	
		if nil ~= tbAddOpt[ i ] then
		
			if 0 ~= string.len( strAddOn ) then
				strAddOn = strAddOn .. "\n"
			end
			
			local tbAddon = tbAddOpt[ i ]
			
			-- Random
			if tbAddon[3] == 49 then
			
				local nValue = tonumber( tbAddon[2] )
				
				if 0 < nValue then
				
					local strTemp = GetGameWordText("ITEM_BASIC_RANDOM_STAT_PREVIEW", 3)
					strAddOn = strAddOn .. HTMLFontColor(string.format( strTemp, nValue ), HTML_TEXT_COLOR_GREEN )
					
				end
				
			-- INT
			elseif tbAddon[1] == true then
			
				local nValue = tbAddon[2]
				if nValue >= 0 then
				
					local str = string.format( "%s : +%d", GetGameWordText( "ITEM_ADDON_INFO", tbAddon[3] ), nValue )
					strAddOn = strAddOn .. HTMLFontColor( str , HTML_TEXT_COLOR_GREEN )
					
				elseif nValue < 0 then
				
					local str = string.format( "%s : %d", GetGameWordText( "ITEM_ADDON_INFO", tbAddon[3] ), nValue )
					strAddOn = strAddOn .. HTMLFontColor( str , HTML_TEXT_COLOR_GREEN )
					
				end
				
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
				
				
				strAddOn = strAddOn .. HTMLFontColor( str, HTML_TEXT_COLOR_GREEN )
			end

		end

	end

	return strAddOn

end

-- 특수옵션
function ItemToolTip_Ability_SpecialOption( tbSpecialOpt )

	local strSpecialOpt = ""

	for i = 1, table.getn( tbSpecialOpt ), 1 do

		if nil ~= tbSpecialOpt[ i ] then

			local tbSpecialOption = tbSpecialOpt[ i ]
			if nil ~= tbSpecialOption[ 1 ] then

				if 0 ~= string.len( strSpecialOpt ) then
					strSpecialOpt = strSpecialOpt .. "\n"
				end

				local nCount = 0
				local fValue = tonumber( tbSpecialOption[ 1 ] ) % 1
				--PrintDebug( " tostring: "..tostring(fValue))
				while fValue ~= 0 do

					nCount = nCount + 1
					if nCount == 2 then
						break
					end

					fValue = (fValue * 10) % 1

				end

				local str = ""
				if true == tbSpecialOption[ 3 ] then

					str = string.format( "%s : %." .. nCount .. "f",
						GetGameWordText( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", tbSpecialOption[ 2 ] ),
						tbSpecialOption[ 1 ] )

				else

					str = string.format( "%s : %." .. nCount .. "f%%",
						GetGameWordText( "ITEM_ADVANCED_INFO_SPECIAL_ADD", tbSpecialOption[ 2 ] ),
						tbSpecialOption[ 1 ] )

				end
				
				--PrintDebug(str .. "   " .. tostring(tbSpecialOption[ 2 ]) .. "   " .. tostring(nCount) )

				strSpecialOpt = strSpecialOpt .. HTMLFontColor( str, HTML_TEXT_COLOR_GREEN )

			end

		end

	end

	return strSpecialOpt

end

-- 경험치 배율
function ItemToolTip_Ability_ExpMultiple( ExpMultiple )

	local ToolTipText_Sum = ""
	-- 경험치 배율 값이 존재할경우
	if ExpMultiple ~= nil and nil ~= ExpMultiple["Exp"] then

		ToolTipText_Sum = HTMLFontColor(
			GetGameWordText( "ITEM_ADVANCED_INFO_SPECIAL_EX", 0 ) ..
			" : " .. tostring( ExpMultiple[ "Exp" ] ) .. "%", HTML_TEXT_COLOR_GREEN )

	end

	return ToolTipText_Sum

end

-- 저항
function ItemToolTip_Ability_Resist( tbItemData )

	local strBlank = "    "
	local strResist = ""

	local Resist = tbItemData["Resist"]
	local ResistAdd = tbItemData["ResistAdd"]

	-- 정기
	if nil ~= Resist["Spirit"] then

		if 0 ~= string.len( strResist ) then
			strResist = strResist .. strBlank
		end

		strResist = strResist ..
			HTMLImage( "Spirit", 11, 11 ) .. HTMLFontColor( tostring(Resist["Spirit"]) , HTML_TEXT_COLOR_GREEN )

		-- 강화 저항 수치가 있을경우
		if nil ~= ResistAdd["Spirit"] and 0 < ResistAdd["Spirit"] then

			local ColorOnToText = "(+" .. tostring(ResistAdd["Spirit"]) .. ")"
			strResist = strResist .. HTMLFontColor( ColorOnToText , HTML_TEXT_COLOR_YELLOW )

		end

	end

	-- 화염
	if nil ~= Resist["Fire"] then

		if 0 ~= string.len( strResist ) then
			strResist = strResist .. strBlank
		end

		strResist = strResist ..
			HTMLImage( "Fire", 11, 11 ) .. HTMLFontColor( tostring(Resist["Fire"]) , HTML_TEXT_COLOR_GREEN )

		-- 강화 저항 수치가 있을경우
		if nil ~= ResistAdd["Fire"] and 0 < ResistAdd["Fire"] then

			local ColorOnToText = "(+" .. tostring(ResistAdd["Fire"]) .. ")"
			strResist = strResist .. HTMLFontColor( ColorOnToText , HTML_TEXT_COLOR_YELLOW )

		end

	end

	-- 빙결
	if nil ~= Resist["Ice"] then

		if 0 ~= string.len( strResist ) then
			strResist = strResist .. strBlank
		end

		strResist = strResist ..
			HTMLImage( "Ice", 11, 11 ) .. HTMLFontColor( tostring(Resist["Ice"]) , HTML_TEXT_COLOR_GREEN )

		-- 강화 저항 수치가 있을경우
		if nil ~= ResistAdd["Ice"] and 0 < ResistAdd["Ice"] then

			local ColorOnToText = "(+" .. tostring(ResistAdd["Ice"]) .. ")"
			strResist = strResist .. HTMLFontColor( ColorOnToText , HTML_TEXT_COLOR_YELLOW )

		end

	end

	-- 전기
	if nil ~= Resist["Elec"] then

		if 0 ~= string.len( strResist ) then
			strResist = strResist .. strBlank
		end

		strResist = strResist ..
			HTMLImage( "Elec", 11, 11 ) .. HTMLFontColor( tostring(Resist["Elec"]) , HTML_TEXT_COLOR_GREEN )

		-- 강화 저항 수치가 있을경우
		if nil ~= ResistAdd["Elec"] and 0 < ResistAdd["Elec"] then

			local ColorOnToText = "(+" .. tostring(ResistAdd["Elec"]) .. ")"
			strResist = strResist .. HTMLFontColor( ColorOnToText , HTML_TEXT_COLOR_YELLOW )

		end

	end

	-- 독
	if nil ~= Resist["Poison"] then

		if 0 ~= string.len( strResist ) then
			strResist = strResist .. strBlank
		end

		strResist = strResist ..
			HTMLImage( "Poison", 11, 11 ) .. HTMLFontColor( tostring(Resist["Poison"]) , HTML_TEXT_COLOR_GREEN )

		-- 강화 저항 수치가 있을경우
		if nil ~= ResistAdd["Poison"] and 0 < ResistAdd["Poison"] then

			local ColorOnToText = "(+" .. tostring(ResistAdd["Poison"]) .. ")"
			strResist = strResist .. HTMLFontColor( ColorOnToText , HTML_TEXT_COLOR_YELLOW )

		end

	end

	return strResist

end

-- 상태이상
function ItemToolTip_Ability_StateBlow( StateBlow )

	local strStateBlow = ""
	if nil ~= StateBlow["Type"] then

		-- 마비
		if 1 == StateBlow["Type"] then

			-- 확률,시간,이속,쿨타임
			strStateBlow = StringFormatPtEx( GetGameInternalText( "DES_STATE_BLOW_NUMB_RN", 0 ),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 기절
		elseif 2 == StateBlow["Type"] then

			-- 확률,시간
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_STUN_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 석화
		elseif 3 == StateBlow["Type"] then

			-- 확률,시간,공격/이속,초당 피해
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_STONE_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 화염
		elseif 4 == StateBlow["Type"] then

			-- 확률,시간,초당 피해
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_BURN_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 냉동
		elseif 5 == StateBlow["Type"] then

			-- 확률,시간, 공격/이속,피해량증가
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_FROZEN_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 착란
		elseif 6 == StateBlow["Type"] then

			-- 확률,시간, 명중률, 회피율
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_MAD_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 중독
		elseif 7 == StateBlow["Type"] then

			-- 확률,시간, 초당 피해
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_POISON_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		-- 저주
		elseif 8 == StateBlow["Type"] then

			-- 확률,시간, 초당피해, [MP회복불가]
			strStateBlow = StringFormatPtEx( GetGameInternalText("DES_STATE_BLOW_CURSE_RN", 0),
				StateBlow["Rate"], StateBlow["Time"], StateBlow["Value1"], StateBlow["Value2"] )

		end

	end

	if 0 ~= string.len( strStateBlow ) then

		strStateBlow = HTMLFontColor( string.replace( string.format( strStateBlow ), "\r\n", "\n" ), HTML_TEXT_COLOR_YELLOW )

	end

	return strStateBlow

end

-- 아이템 링크 스킬
function ItemToolTip_SkillLink( SkillLink )

	local strSkillLink = ""

	-- 확률,시간,이속,쿨타임
	local data = SkillLink[ "ItemSkLink" ]
	if nil ~= data then
	
		if ( true == data[ 1 ] ) then
		
			-- 생성하기 전인 아이템이며 아이템 링크 스킬이 개조가능한 경우 출력한다;
			if ( true == data[ 7 ] ) and ( true == data[ 8 ] ) then
			
				strSkillLink = GetGameWordText( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 1 )
				return HTMLFontColor( string.format( strSkillLink ), HTML_TEXT_COLOR_DODGERBLUE )
				
			else
			
				strSkillLink = StringFormat( GetGameInternalText( "ITEM_SKILL_LINK_INFO", data[ 6 ] ), data[ 4 ], data[ 5 ] )
				return HTMLFontColor( string.format( strSkillLink ), HTML_TEXT_COLOR_DODGERBLUE )
	
			end
			
		elseif ( false == data[ 1 ] ) and ( true == data[ 7 ] ) and ( true == data[ 8 ] ) then
		
			strSkillLink = GetGameWordText( "ITEM_BASIC_RANDOM_STAT_PREVIEW", 1 )
			return HTMLFontColor( string.format( strSkillLink ), HTML_TEXT_COLOR_DODGERBLUE )
			
		end
		
	end

	return strSkillLink

end

-- 개조 UI 아이템 링크 스킬
function ItemToolTip_SkillLink_Rebuild( SkillLink )

	local strSkillLink = ""

	-- 확률,시간,이속,쿨타임
	local data = SkillLink[ "ItemSkLink" ]
	
	-- 개조가능하지 않은 상태라면 출력한다;
	if ( false == data[ 8 ] ) then
		
		strSkillLink = ItemToolTip_SkillLink( SkillLink )
	
	end

	return strSkillLink

end

-- 아이템 설명
function ItemToolTip_Ability_Explain( Explain )

	local ToolTipText_Sum = ""

	if nil ~= Explain["Comment"] and  "" ~= Explain["Comment"] then

		ToolTipText_Sum = tostring(Explain["Comment"])

	end

	return ToolTipText_Sum

end

function ItemToolTip_Ability_ExplainEx( ExplainEx )

	local ToolTipText_Sum = ""

	if "" ~= ExplainEx["CommentEx"] and nil ~= ExplainEx["CommentEx"] then

		ToolTipText_Sum = tostring(ExplainEx["CommentEx"])

	end

	return ToolTipText_Sum

end

-- 아이템 거래 설정
function ItemToolTip_Ability_TrCond( TrCond )

	g_tbCopty_ItemTooltip_TradeCondtion = {}

	local strCondition = ""

	if 	false == TrCond[ "Sale" ] or
		false == TrCond[ "Throw" ] or
		false == TrCond[ "Trade" ] then

		-- 상점 판매
		if true == TrCond["Sale"] then

			strCondition = strCondition .. GetGameWordText("ITEM_INFO_TRADE_RN", 0)

		else

			strCondition = strCondition .. HTMLFontColor( GetGameWordText("ITEM_INFO_TRADE_RN", 0) , HTML_TEXT_COLOR_GRAY )

		end

		strCondition = strCondition .. " / "

		-- 버리기
		if true == TrCond["Throw"] then

			strCondition = strCondition .. GetGameWordText("ITEM_INFO_TRADE_RN", 1)

		else

			strCondition = strCondition .. HTMLFontColor( GetGameWordText("ITEM_INFO_TRADE_RN", 1) , HTML_TEXT_COLOR_GRAY )

		end

		strCondition = strCondition .. " / "

		-- 거래
		if true == TrCond["Trade"] then

			strCondition = strCondition .. GetGameWordText("ITEM_INFO_TRADE_RN", 3)

		else

			strCondition = strCondition .. HTMLFontColor( GetGameWordText("ITEM_INFO_TRADE_RN", 3) , HTML_TEXT_COLOR_GRAY )

		end

		-- 란 포인트 거래
		if nil ~= TrCond["RanTr"] then

			strCondition = strCondition .. " / "

			if true == TrCond["RanTr"] then

				strCondition = strCondition .. GetGameWordText("ITEM_INFO_TRADE_RN", 4)
				strCondition = strCondition .. tostring(TrCond["RanTrCount"])

			else

				strCondition = strCondition .. HTMLFontColor( GetGameWordText("ITEM_INFO_TRADE_RN", 4) , HTML_TEXT_COLOR_GRAY )

			end

		end

	end

	-- 창고 보관 불가능할 때만
	if true == TrCond["Locker"] then

		if 0 ~= string.len( strCondition ) then
			strCondition = strCondition .. " / "
		end

		strCondition = strCondition .. HTMLFontColor( GetGameWordText( "ITEM_INFO_TRADE_RN", 6 ), HTML_TEXT_COLOR_GRAY )

	else

		if ( false == TrCond[ "Trade" ] ) then

			if 0 ~= string.len( strCondition ) then
				strCondition = strCondition .. " / "
			end

			strCondition = strCondition .. GetGameWordText( "ITEM_INFO_TRADE_RN", 6 )

		end

	end


	-- 포장 가능할 때만
	if true == TrCond[ "BoxWrappable" ] then

		if 0 ~= string.len( strCondition ) then
			strCondition = strCondition .. " / "
		end

		strCondition = strCondition .. GetGameWordText( "ITEM_INFO_TRADE_RN", 5 )

	end

	return strCondition

end

-- 아이템 가격
function ItemToolTip_Ability_Price( tbItemData, tbItemCondData )

	-- 인벤토리면 마켓 열려있을때만 가격 출력한다.
	if SLOT_INVENTORY == tbItemData[ "SlotType" ] then

		if false == NpcTalk_IsMarketTabOpen() then
			return ""
		end

		-- http://172.16.2.87/redmine/issues/4372 상점판매가 가능할 경우에만 가격 표시
		if false == tbItemCondData["Sale"] then
			return ""
		end

	-- 개인 상점이나 NPC마켓 아니면 가격 출력할 필요 없다.
	elseif 	SLOT_PRIVATE_MARKET ~= tbItemData[ "SlotType" ] and
			SLOT_NPCMARKET ~= tbItemData[ "SlotType" ] then

			return ""

	end

	local ToolTipText_Sum = ""

	-- 구매/판매 가격
	local tbPrice = tbItemData["Price"]
	-- 아이템교환/포인트교환/생활점수교환
	local tbTradeCondition = tbItemData["TradeCondition"]

	if nil == tbPrice then

		-- 개인상점 아이템 가격
		local nPMarketPrice = tbItemData[ "PMarketPrice" ]
		if nil ~= nPMarketPrice then

			local strColor = TranslateHTMLColor( GetMoneyColor( nPMarketPrice ) )
			local strPrice = HTMLFontColor( GetGameWordText( "ITEM_BASIC_INFO", 16 ) .. " : " .. CurrencyFormat(nPMarketPrice), strColor)

			-- 개당 구입 가격 : xxx
			ToolTipText_Sum = HTMLFontColor( strPrice, HTML_TEXT_COLOR_MAGENTA )

		end

	else

		-- 기본상태(판매일때)
		if tbTradeCondition == nil then

			local sellPrice	= math.floor( tbPrice[2] * ( tbPrice[4] / 100 ) )

			local tbNum = tbItemData["Num"]
			local Count = tonumber(tbNum["TurnNum"]) -- 수량
			if Count ~= nil then
				sellPrice = sellPrice * Count
			end

			ToolTipText_Sum = GetGameWordText("ITEM_BASIC_INFO", 14) .. " : " .. 
				HTMLFontColor( CurrencyFormat( sellPrice ), TranslateHTMLColor( GetMoneyColor( sellPrice ) ) )

		-- NPC 상점 슬롯(구매상태)
		elseif tbTradeCondition[1] == nil then
		
			local BuyPrice = 0

			if tbPrice[1] < 100000000 then
				BuyPrice = math.floor( ( tbPrice[1] *  0.01 ) ) * tbPrice[3] 
			else
			
				local itemPrice = math.floor( ( tbPrice[1] * 0.001 ))
				itemPrice = math.floor( ( math.floor( itemPrice *  0.01 ) ) * tbPrice[3] ) 
	
				local stritemPrice = tostring( itemPrice ) .. "000"
				
				BuyPrice =  tonumber(stritemPrice)			
			end
			
			local strColor = ""
			if BuyPrice > g_MyChar:GetInvenMoney() then
				strColor = HTML_TEXT_COLOR_RED
			else
				strColor = TranslateHTMLColor( GetMoneyColor( BuyPrice ) )
			end
		
			ToolTipText_Sum = GetGameWordText("ITEM_BASIC_INFO", 13) .. " : " ..
				HTMLFontColor( CurrencyFormat( BuyPrice ), strColor )

		-- NPC 상점 슬롯 중 아이템/포인트/생활점수 교환일 경우
		else

			local tbTrade = tbTradeCondition[ 1 ]

			-- 아이템 교환
			if tbTrade[4] == 0 then
		
				g_nItemTooltip_TradeSlotCount = table.getn( tbTradeCondition )
				g_sItemToolTip_TradeTypeText = GetGameWordText( "TRADE_ITEM_CONDITION", 0 )
				g_tbCopty_ItemTooltip_TradeCondtion = CopyTable(tbTradeCondition)	
				
				for i = 1, g_nItemTooltip_TradeSlotCount, 1 do

					ResetSlot( SLOT_TOOLTIP_TRADE_ITEM_ID .. tostring( i ) )

					local tbCondition = tbTradeCondition[ i ]
					SetSlot_Item( SLOT_TOOLTIP_TRADE_ITEM_ID .. tostring( i ), tbCondition[ 1 ], tbCondition[ 2 ], tbCondition[ 3 ] )

				end

			-- 포인트 교환
			elseif tbTrade[4] == 1 then

				g_sItemToolTip_TradeTypeText = GetGameWordText( "TRADE_ITEM_CONDITION", 1 )
					tostring( tbCondition[ 3 ] )

			-- 생활점수 교환
			elseif tbTrade[4] == 2 then

				g_sItemToolTip_TradeTypeText = GetGameWordText( "TRADE_ITEM_CONDITION", 2 )
					tostring( tbCondition[ 3 ] )

			end

		end

	end

	return ToolTipText_Sum

end

-- 특별활동 포인트
function ItemToolTip_Ability_ActPoint( ActPoint )

	local ToolTipText_Sum = ""

	-- (착용조건)특활 포인트가 있을 경우
	if nil ~= ActPoint["ComparePoint"] then

		-- 착용 조건을 충족할 경우
		if true == ActPoint["ComparePoint"] then

			-- 상한 조건이 있을경우(해당 포인트가 넘으면 착용 불가)
			if nil ~= ActPoint["ReqActPointUP"] then

				ToolTipText_Sum = StringFormat( GetGameWordText("ACTIVITY_SALESITEM_ITEM_TEXT", 1), tostring(ActPoint["Type"]), tostring(ActPoint["ReqActPointUP"]))

			-- 하한 조건이 있을경우(해당 포인트보다 부족하면 착용 불가)
			elseif nil ~= ActPoint["ReqPointDW"] then

				ToolTipText_Sum = StringFormat( GetGameWordText("ACTIVITY_SALESITEM_ITEM_TEXT", 1), tostring(ActPoint["Type"]), tostring(ActPoint["ReqPointDW"]))

			end

		-- 착용 조건을 충족하지 못할 경우
		else

			if nil ~= ActPoint["ReqActPointUP"] then

				ToolTipText_Sum = StringFormat( GetGameWordText("ACTIVITY_SALESITEM_ITEM_TEXT", 1), tostring(ActPoint["Type"]), tostring(ActPoint["ReqActPointUP"]))
				ToolTipText_Sum = HTMLFontColor( ToolTipText_Sum , HTML_TEXT_COLOR_RED )

			elseif nil ~= ActPoint["ReqPointDW"] then

				ToolTipText_Sum = StringFormat( GetGameWordText("ACTIVITY_SALESITEM_ITEM_TEXT", 1), tostring(ActPoint["Type"]), tostring(ActPoint["ReqPointDW"]))
				ToolTipText_Sum = HTMLFontColor( ToolTipText_Sum , HTML_TEXT_COLOR_RED )

			end

		end

	end

	return ToolTipText_Sum

end

-- 기여도 포인트
function ItemToolTip_Ability_Contribution( Contribution )

	local ToolTipText_Sum = ""

	-- (착용조건)기여도 포인트가 있을 경우
	if nil ~= Contribution["CompareContri"] then

		-- 착용 조건을 충족한 경우
		if true == Contribution["CompareContri"] then

			ToolTipText_Sum = GetGameWordText("RN_COMPETITION_REWARD", 0) .. " " .. tostring(Contribution["ReqContribution"])

		-- 착용 조건을 충족하지 못한 경우
		else

			ToolTipText_Sum = GetGameWordText("RN_COMPETITION_REWARD", 0) .. " " .. tostring(Contribution["ReqContribution"])
			ToolTipText_Sum = HTMLFontColor( ToolTipText_Sum , HTML_TEXT_COLOR_RED )

		end

	end

	return ToolTipText_Sum

end

-- 기간
function ItemToolTip_Ability_Time( tbTime, tbTypeData )

	local strTime = ""

	-- 기간이 있는 아이템일 경우
	if nil ~= tbTime[ "EndTime" ] then

		if nil ~= tbTypeData[ "DisguiseOk" ] and true == tbTypeData[ "DisguiseOk" ] then

			local str = GetGameWordText( "ITEM_BASIC_INFO", 7 ) .. " : " .. tbTime[ "EndTime" ]
			strTime = strTime .. HTMLFontColor( str , HTML_TEXT_COLOR_ORANGE )

		else

			local str = GetGameWordText( "ITEM_BASIC_INFO", 4 ) .. " : " .. tbTime[ "EndTime" ]
			strTime = strTime .. HTMLFontColor( str , HTML_TEXT_COLOR_ORANGE )

		end

	end

	-- 코스튬 시간
	if nil ~= tbTime[ "DisguiseTime" ] then

		if 0 ~= string.len( strTime ) then
			strTime = strTime .. "\n"
		end

		local str = GetGameWordText( "ITEM_BASIC_INFO", 7 ) .. " : " .. tbTime[ "DisguiseTime" ]
		strTime = strTime .. HTMLFontColor( str , HTML_TEXT_COLOR_ORANGE )

	end

	-- 코스튬 능력치 기간
	if nil ~= tbTime[ "CSAbilityEndTime" ] then

		if 0 ~= string.len( strTime ) then
			strTime = strTime .. "\n"
		end

		local str = GetGameWordText( "ITEM_BASIC_INFO", 20 ) .. " : " .. tbTime[ "CSAbilityEndTime" ]
		strTime = strTime .. HTMLFontColor( str , HTML_TEXT_COLOR_ORANGE )

	end

	-- 일시강화 기간
	if nil ~= tbTime["TEEndTime"] then

		if 0 ~= string.len( strTime ) then
			strTime = strTime .. "\n"
		end

		local str = GetGameWordText( "ITEM_BASIC_INFO", 22 ) .. " : " ..
			"(+" .. tostring( tbTime[ "TempEnchant" ] ) .. ") " .. tbTime[ "TEEndTime" ]
		strTime = strTime .. HTMLFontColor( str , HTML_TEXT_COLOR_ORANGE )

	end

	return strTime

end

-- 세트아이템
function ItemToolTip_Ability_SetItem( tbItemData )

	local strSetItem = ""

	-- 세트아이템 이름(착용개수/총개수)
	if nil ~= tbItemData[ "SetItemName" ] then

		local tbSetItemName = tbItemData[ "SetItemName" ]
		strSetItem = HTMLFontColor( tbSetItemName[ "SetItemName" ] , HTML_TEXT_COLOR_GREEN ) .. "\n"

	end

	-- 세트아이템 부위
	if nil ~= tbItemData[ "SetItemPart" ] then

		local tbSetItemPart = tbItemData[ "SetItemPart" ]

		local str = ""

		-- 착용 중인 아이템 색상 삽입
		for i = 1, table.getn( tbSetItemPart[ 1 ] ), 1 do

			if 0 ~= string.len( str ) then
				str = str .. SlushAndSpace
			end

			local PutOnText = tbSetItemPart[ 1 ]
			str = str .. GetGameWordText( "ITEM_INFO_SET_PARTS", PutOnText[ i ] )

		end

		-- 착용 중이지 않은 아이템 색상 삽입
		for i = 1, table.getn( tbSetItemPart[ 2 ] ), 1 do

			if 0 ~= string.len( str ) then
				str = str .. SlushAndSpace
			end

			local TakeOffText = tbSetItemPart[ 2 ]
			str = str .. HTMLFontColor( GetGameWordText( "ITEM_INFO_SET_PARTS", TakeOffText[ i ] ) , HTML_TEXT_COLOR_GRAY )

		end

		strSetItem = string.format( "%s- %s", strSetItem, str )

	end

	-- 세트아이템 효과
	if nil ~= tbItemData[ "SetItemBonus" ] then

		local tbItemBonus = tbItemData[ "SetItemBonus" ]
		local tbSetItemPart = tbItemData[ "SetItemPart" ]

		local Comparison = 0

		for i = 1, table.getn( tbItemBonus ), 1 do

			local tbBonus = tbItemBonus[ i ]

			-- 효과 수치
			local nValue = tbBonus[ "BonusNum" ]
			if true == tbBonus[ "IsPercentage" ] then
				nValue = string.format( "%.2f%%", nValue * 100 )
			end

			-- 세트 만족한 상태에 따라 텍스트 컬러
			local strColor = HTML_TEXT_COLOR_GREEN
			if table.getn( tbSetItemPart[ 1 ] ) < tbBonus[ "Piece" ] then
				strColor = HTML_TEXT_COLOR_GRAY
			end

			-- 다른 세트면 한줄 개행하고 몇 세트인지 표기
			if Comparison ~= tbBonus[ "Piece" ] then

				strSetItem = strSetItem .. "\n"

				local str = tostring( tbBonus[ "Piece" ] ) .. GetGameWordText( "ITEM_INFO_SET", 0 )
				strSetItem = strSetItem .. HTMLFontColor( str, strColor )

			-- 같으면 효과 뒤에 콤마 추가
			else

				strSetItem = strSetItem ..  HTMLFontColor( ", ", strColor )

			end

			-- 세트 효과
			local str = GetGameWordText( "ITEM_INFO_SET_BONUS", tbBonus[ "BonusName" ] ) ..
				" +" .. tostring( nValue )
			strSetItem = strSetItem .. HTMLFontColor( str , strColor )

			Comparison = tbBonus[ "Piece" ]

		end

	end

	return strSetItem

end

-- 갯수
function ItemToolTip_Ability_StackNum( Num )

	local ToolTipText_Sum = ""

	-- 최대 중첩수가 1이 아니라면
	if 1 < Num[ "PileNum" ] then

		ToolTipText_Sum = string.format( "%s : %d/%d",
			GetGameWordText("ITEM_TURN_INFO", 0),
			Num["TurnNum"],
			Num["PileNum"] )

	end

	return ToolTipText_Sum

end

-- 특수 타입
function ItemToolTip_Type_Special( tbItemData )

	local ToolTipText_Sum = ""
	-- 아이템 종류 및 색변경 여부
	local ToolTip_TypeData = tbItemData["TypeData"]

	-- 쿨타임(재사용 대기시간)
	ToolTipText_Sum = ItemToolTip_Type_CoolTime( tbItemData["CoolTime"] )

	-- 스킬 문서
	if nil ~= tbItemData["NeedSk"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_NeedSkill( tbItemData["NeedSk"] )
	-- 펫 스킬
	elseif nil ~= tbItemData["PetSkill"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_PetSkill( tbItemData["PetSkill"] )
	-- 탈것
	elseif nil ~= tbItemData["Vehicle"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_Vehicle( tbItemData["Vehicle"] )
	-- 펫 카드
	elseif nil ~= tbItemData["PetCard"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_PetCard( tbItemData["PetCard"] )
	-- 도시락
	elseif nil ~= tbItemData["LunchBox"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_LunchBox( tbItemData["LunchBox"] )
	-- 펫 듀얼스킬 카드
	elseif nil ~= tbItemData["PetDualSk"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_PetDualSk( tbItemData["PetDualSk"] )
	-- 포인트 카드
	elseif nil ~= tbItemData["PointCard"] then
		ToolTipText_Sum = ToolTipText_Sum .. ItemToolTip_Ability_PointCard( tbItemData["PointCard"] )
	-- 박스 아이템
	elseif nil ~= tbItemData[ "tbBoxItem" ] then

		-- 추가 슬롯 개수 설정
		g_nItemTooltip_AddSlotCount = table.getn( tbItemData[ "tbBoxItem" ] )

		-- 추가 슬롯 아이템 설정
		for i = 1, TOOLTIP_ADD_SLOT_COUNT, 1 do

			local tbItem = tbItemData[ "tbBoxItem" ][ i ]
			if nil ~= tbItem then
				SetSlot_Item( SLOT_TOOLTIP_ADD_ITEM_ID .. tostring( i ), tbItem[ 1 ], tbItem[ 2 ], tbItem[ 3 ] )
			else
				ResetSlot( SLOT_TOOLTIP_ADD_ITEM_ID .. tostring( i ) )
			end

		end

		-- 아이템 링크 정보에서 쓰일 수 있으니 테이블 복사해둔다.
		g_tbItemTooltip_AddSlotItem = CopyTable( tbItemData[ "tbBoxItem" ] )

	-- 약품류 인가
	elseif "" ~= tbItemData["DrugUseEffect"] then
		ToolTipText_Sum = ToolTipText_Sum .. tostring( tbItemData["DrugUseEffect"] )
	end

	return ToolTipText_Sum

end

function ItemToolTip_Ability_NeedSkill( NeedSk )

	local ToolTipText_Sum = ""

	-- 거리
	if nil ~= NeedSk["Range"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum ..
			GetGameWordText("SKILL_ADVANCED_TARGET_RANGE", 0) .. " : " .. tostring(NeedSk["Range"])

	end

	-- 주 사용도구
	if nil ~= NeedSk["RItem"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum ..
			GetGameWordText("SKILL_ADVANCED_USE_ITEM_LR_HAND", 1) .. " : " .. GetGameWordText("SKILL_ADVANCED_USE_ITEM", NeedSk["RItem"] + 1)
	end

	-- 보조 사용도구
	if nil ~= NeedSk["LItem"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum ..
			GetGameWordText("SKILL_ADVANCED_USE_ITEM_LR_HAND", 0) .. " : " .. GetGameWordText("SKILL_ADVANCED_USE_ITEM", NeedSk["LItem"] + 1)

	end

	-- 이미 배운 스킬일 경우
	if true == NeedSk["Already"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( GetGameWordText("ITEM_SKILL_CONDITION", 0) , HTML_TEXT_COLOR_RED )

	else

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		local strRequire = ""

		-- 요구 스킬
		if nil ~= NeedSk["SkName"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			-- 요구 스킬을 내가 배웠다면.
			if true == NeedSk["SkCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO", 0).. " : " .. tostring(NeedSk["SkName"])

			-- 요구 스킬을 내가 배우지 못했다면 글자색상 붉은색
			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO", 0).. " : " .. tostring(NeedSk["SkName"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 스킬 레벨
		if nil ~= NeedSk["SkLevel"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			-- 보유 요구 스킬레벨이 대상 스킬의 요구 스킬레벨 이상이라면
			if true == NeedSk["SkLevelCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO", 1).. " : " .. tostring(NeedSk["SkLevel"])

			-- 요구 스킬레벨을 만족하지 못했다면 글자색상 붉은색
			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO", 1).. " : " .. tostring(NeedSk["SkLevel"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 포인트
		if nil ~= NeedSk["Exp"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			-- 요구 포인트 보다 보유 포인트가 많다면
			if true == NeedSk["ExpCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO", 2).. " : " .. tostring(NeedSk["Exp"])

			-- 요구 포인트보다 보유포인트가 작을경우 글자색상 붉은색
			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO", 2).. " : " .. tostring(NeedSk["Exp"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 레벨
		if nil ~= NeedSk["Level"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			-- 내 레벨이 요구레벨보다 높다면
			if true == NeedSk["LevelCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO", 3).. " : " .. tostring(NeedSk["Level"])

			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO", 3).. " : " .. tostring(NeedSk["Level"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 힘
		if nil ~= NeedSk["Pow"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			-- 내 힘이 요구 힘보다 높다면
			if true == NeedSk["PowCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO_STATS", 0).. " : " .. tostring(NeedSk["Pow"])

			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO_STATS", 0) .. " : " .. tostring(NeedSk["Pow"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 체력
		if nil ~= NeedSk["Str"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			if true == NeedSk["StrCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO_STATS", 1).. " : " .. tostring(NeedSk["Str"])

			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO_STATS", 1) .. " : " .. tostring(NeedSk["Str"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 정신
		if nil ~= NeedSk["Spi"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			if true == NeedSk["SpiCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO_STATS", 2).. " : " .. tostring(NeedSk["Spi"])

			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO_STATS", 2) .. " : " .. tostring(NeedSk["Spi"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 민첩
		if nil ~= NeedSk["Dex"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			if true == NeedSk["DexCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO_STATS", 3).. " : " .. tostring(NeedSk["Dex"])

			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO_STATS", 3) .. " : " .. tostring(NeedSk["Dex"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		-- 요구 근력
		if nil ~= NeedSk["Sta"] then

			if 0 ~= string.len( strRequire ) then
				strRequire = strRequire .. "\n"
			end

			if true == NeedSk["StaCompare"] then

				strRequire = strRequire ..
					GetGameWordText("SKILL_ADVANCED_INFO_STATS", 4).. " : " .. tostring(NeedSk["Sta"])

			else

				local NeedStatColor = GetGameWordText("SKILL_ADVANCED_INFO_STATS", 4) .. " : " .. tostring(NeedSk["Sta"])
				strRequire = strRequire ..
					HTMLFontColor( NeedStatColor , HTML_TEXT_COLOR_RED )

			end

		end

		if 0 ~= string.len( strRequire ) then

			ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText( "ITEM_SKILL_CONDITION", 3 ) .. "\n" .. strRequire

		end

	end

	return ToolTipText_Sum

end

-- 펫 스킬 카드
function ItemToolTip_Ability_PetSkill( PetSkill )

	local ToolTipText_Sum = ""

	-- 이미 배운 스킬일 경우
	if true == PetSkill["Already"] then
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( GetGameWordText("ITEM_SKILL_CONDITION", 0) , HTML_TEXT_COLOR_RED )
	end

	return ToolTipText_Sum

end

-- 탈것
function ItemToolTip_Ability_Vehicle( Vehicle )

	local ToolTipText_Sum = ""

	-- 파츠 개수 새기위한 크기책정
	local VehicleSize = table.getn( Vehicle )
	if 0 < VehicleSize then

		for i = 1, VehicleSize, 1 do

			if 0 ~= string.len( ToolTipText_Sum ) then
				ToolTipText_Sum = ToolTipText_Sum .. "\n"
			end

			local VehicleCount = Vehicle[ i ]
			-- 파츠 이름 삽입
			ToolTipText_Sum = ToolTipText_Sum ..
				GetGameWordText( "ITEM_VEHICLE_SLOT", VehicleCount[ "Parts" ] ) .. VehicleCount[ "Name" ]

		end

	end

	-- 부스터 사용 가능 Index가 존재하는가.
	if nil ~= Vehicle[ "BoostLearn" ] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		-- 부스터 사용 가능여부(0 : 불가/ 1 : 가능)
		ToolTipText_Sum = ToolTipText_Sum ..
			GetGameWordText( "ITEM_VEHICLE_BOOST_LEARN", Vehicle[ "BoostLearn" ] )

	end

	-- 베터리 잔량 정보 존재여부
	if nil ~= Vehicle[ "Battery" ] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		-- 베터리 잔량 출력
		ToolTipText_Sum = ToolTipText_Sum ..
			GetGameWordText( "ITEM_VEHICLE_BATTERY", 0 ) .. string.format( "%.2f%%", Vehicle[ "Battery" ] )

	end

	return ToolTipText_Sum

end

-- 펫카드
function ItemToolTip_Ability_PetCard( PetCard )

	local ToolTipText_Sum = ""
	local PetCardCount = 0

	-- 펫 타입
	if nil ~= PetCard["Type"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum .. tostring(PetCard["Type"])

	end

	-- 펫 포만도
	if nil ~= PetCard["Satiety"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum ..
			GetGameWordText("ITEM_PET_FULL", 0) .. " " .. string.format( "%.2f%%", PetCard["Satiety"] )

	end

	-- 펫 듀얼스킬 습득 여부
	if nil ~= PetCard["DualSk"] then

		if true == PetCard["DualSk"] then

			if 0 ~= string.len( ToolTipText_Sum ) then
				ToolTipText_Sum = ToolTipText_Sum .. "\n"
			end

			ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText("ITEM_PET_DUALSKILL", 1)

		else

			if 0 ~= string.len( ToolTipText_Sum ) then
				ToolTipText_Sum = ToolTipText_Sum .. "\n"
			end

			ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText("ITEM_PET_DUALSKILL", 0)

		end

	end

	-- 펫 A파츠 이름
	if nil ~= PetCard["PartAName"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText("ITEM_PET_SLOTA", 0) .. " " .. tostring(PetCard["PartAName"])

	end

	-- 펫 B파츠 이름
	if nil ~= PetCard["PartBName"] then

		if 0 ~= string.len( ToolTipText_Sum ) then
			ToolTipText_Sum = ToolTipText_Sum .. "\n"
		end

		ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText("ITEM_PET_SLOTB", 0) .. " " .. tostring(PetCard["PartBName"])

	end

	local PetSkillSize = table.getn( PetCard )
	for i = 1, PetSkillSize, 1 do

		local PetSkill = PetCard[i]
		-- 사용 가능한 스킬인가.(false가 사용가능)
		if false == PetSkill[1] then

			-- 처음 1회만 [사용 가능 스킬] 문구를 넣어주기 위해서.
			if 0 == PetCardCount then

				if 0 ~= string.len( ToolTipText_Sum ) then
					ToolTipText_Sum = ToolTipText_Sum .. "\n"
				end

				ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText("ITEM_PET_SKILLS", 0)
				PetCardCount = PetCardCount + 1

			end

			if 0 ~= string.len( ToolTipText_Sum ) then
				ToolTipText_Sum = ToolTipText_Sum .. "\n"
			end

			-- 스킬 이름 삽입
			ToolTipText_Sum = ToolTipText_Sum .. tostring(PetSkill[2])

		end

	end

	if nil ~= PetCard["DisguiseTime"] then
		ToolTipText_Sum = ToolTipText_Sum .. "\n[" .. GetGameWordText("ITEM_PET_USE_SKINPACK", 0) .. "]\n"
			.. GetGameWordText("ITEM_BASIC_INFO", 9 ) .. " : " .. tostring(PetCard["DisguiseTime"] ) .. "  sec"
	end

	return ToolTipText_Sum

end

-- 도시락
function ItemToolTip_Ability_LunchBox( LunchBox )

	local ToolTipText_Sum = ""

	if nil ~= LunchBox["Explain"] then

		-- 도시락 설명 삽입
		ToolTipText_Sum = "\n" .. tostring(LunchBox["Explain"])

	end

	return ToolTipText_Sum

end

-- 펫 듀얼스킬 카드
function ItemToolTip_Ability_PetDualSk( PetDualSk )

	local ToolTipText_Sum = ""

	-- 배웠는지 여부
	if true == PetDualSk["Learn"] then

		-- 이미 배운 스킬문서 텍스트 출력(붉은색)
		ToolTipText_Sum = ToolTipText_Sum .. HTMLFontColor( GetGameWordText("ITEM_SKILL_CONDITION", 0) , HTML_TEXT_COLOR_RED )

	end

	return ToolTipText_Sum

end

-- 포인트 카드
function ItemToolTip_Ability_PointCard( PointCard )

	local ToolTipText_Sum = ""

	-- 획득 포인트가 있는지
	if nil ~= PointCard["Point"] then

		-- 획득 포인트량
		ToolTipText_Sum = StringFormat(GetGameWordText("POINT_CARD_POINT_VOLUME", 0), PointCard["Point"] )

	end

	return ToolTipText_Sum

end

-- 랜덤상자
function ItemToolTip_Ability_Random( Random )

	local ToolTipText_Sum = ""

	-- 랜덤 상자인지 여부
	if true == Random["Random"] then

		ToolTipText_Sum = ToolTipText_Sum .. GetGameWordText("PRODUCT_TYPE_STRING", 11)

	end

	return ToolTipText_Sum

end

-- 쿨타임
function ItemToolTip_Type_CoolTime( CoolTime )

	local ToolTipText_Sum = ""
	local MaxCoolTime = CoolTime[1]
	local NowCoolTime = CoolTime[2]

	-- 아이템 사용시 적용 될 쿨타입
	if nil ~= MaxCoolTime["Hour"] and nil ~= MaxCoolTime["Minute"] and nil ~= MaxCoolTime["Second"] then
		ToolTipText_Sum = GetGameWordText("ITEM_BASIC_INFO", 8) .. " : "
		if 0 < MaxCoolTime["Hour"] then
			ToolTipText_Sum = ToolTipText_Sum .. tostring(MaxCoolTime["Hour"]) .. GetGameWordText("ITEM_BASIC_TIME", 0) .. " "
		end

		if 0 < MaxCoolTime["Minute"] then
			ToolTipText_Sum = ToolTipText_Sum .. tostring(MaxCoolTime["Minute"]) .. GetGameWordText("ITEM_BASIC_TIME", 1) .. " "
		end

		if 0 < MaxCoolTime["Second"] then
			ToolTipText_Sum = ToolTipText_Sum .. tostring(MaxCoolTime["Second"]) .. GetGameWordText("ITEM_BASIC_TIME", 2)
		end
	end

	-- 현재 아이템의 쿨타임
	if nil ~= NowCoolTime["Hour"] and nil ~= NowCoolTime["Minute"] and nil ~= NowCoolTime["Second"] then
		ToolTipText_Sum = ToolTipText_Sum .. "\n" .. GetGameWordText("ITEM_BASIC_INFO", 9) .. " : "
		if 0 < NowCoolTime["Hour"] then
			ToolTipText_Sum = ToolTipText_Sum .. tostring(NowCoolTime["Hour"]) .. GetGameWordText("ITEM_BASIC_TIME", 0) .. " "
		end

		if 0 < NowCoolTime["Minute"] then
			ToolTipText_Sum = ToolTipText_Sum .. tostring(NowCoolTime["Minute"]) .. GetGameWordText("ITEM_BASIC_TIME", 1) .. " "
		end

		if 0 < NowCoolTime["Second"] then
			ToolTipText_Sum = ToolTipText_Sum .. tostring(NowCoolTime["Second"]) .. GetGameWordText("ITEM_BASIC_TIME", 2)
		end
	end

	return ToolTipText_Sum

end

function ItemTooltip_WishListInfo( tbWishList )

	local ToolTipText_Sum = ""
	local strHead = ""
	local strContent = ""
	
	if tbWishList == nil then
		
		return ;
		
	end 
	
	if tbWishList["Type"] ~= nil then
	
		local nType = tbWishList["Type"]
		local strFrom = tbWishList["from"]
		
		strHead = GetGameWordText( "WISHLIST_TOOLTIP" , 0  )
		strHead = HTMLFontColor( strHead, HTML_TEXT_COLOR_GREEN )
		
		if nType == 1 then			-- NPC 상점
			
			if strFrom ~= nil then
				strContent = strFrom
			end
			
		elseif nType == 2 then		-- 포인트 상점
		
			strContent = GetGameWordText( "WISHLIST_TOOLTIP" , 1 )
		
		elseif nType == 3 then		-- 개인 상점
		
			if strFrom ~= nil then
				strContent = StringFormat( GetGameWordText( "WISHLIST_TOOLTIP" , 2 ), strFrom )
			end
		
		elseif nType == 4 then		-- 채팅 링크
			
			if strFrom ~= nil then
				strContent = StringFormat( GetGameWordText( "WISHLIST_TOOLTIP" , 3 ), strFrom )
			end
		
		elseif nType == 5 then		-- Activate ? 
		
		elseif nType == 6 then		-- 교환 ( 개언 거래 )
		
			if strFrom ~= nil then
				strContent = StringFormat( GetGameWordText( "WISHLIST_TOOLTIP" , 4 ), strFrom )
			end
		
		elseif nType == 7 then		-- 타 캐릭터 착용 장비
		
			if strFrom ~= nil then
				strContent = StringFormat( GetGameWordText( "WISHLIST_TOOLTIP" , 5 ), strFrom )
			end
		
		elseif nType == 8 then		-- 내 인벤토리 
		
			strContent = GetGameWordText( "WISHLIST_TOOLTIP" , 6 )
		
		elseif nType == 9 then		-- 은행
		
			strContent = GetGameWordText( "WISHLIST_TOOLTIP" , 7 )
		
		elseif nType == 10 then		-- 락커
		
			strContent = GetGameWordText( "WISHLIST_TOOLTIP" , 8 )
		
		elseif nType == 11 then -- NPC상점 EX(?)
			
			if strFrom ~= nil then
				strContent = strFrom
			end
			
		else
				
		end 	
	end
	
	ToolTipText_Sum = strHead .. "\n" .. strContent

	return ToolTipText_Sum
	
end

function ItemTooltip_DrugCall( tbDrugCall )

	if nil == tbDrugCall then
		return ""
	end

	if 4 ~= table.getn( tbDrugCall ) then
		return ""
	end

	-- 사용 정보
	local str = GetGameWordText( "ITEM_BASIC_INFO", 17 ) .. "\n"

	-- 위치 정보 없음.
	if false == tbDrugCall[ 1 ] then

		--str = str .. HTMLFontColor( GetGameWordText( "ITEM_CALL_MAP", 2 ), HTML_TEXT_COLOR_RED )
		-- 위치 정보 없는경우 표시하지 않도록 변경
		return ""

	-- 귀환 맵, 좌표
	else

		str = str .. string.format( "%s : %s\n%s : %d, %d",
			GetGameWordText( "ITEM_CALL_MAP", 0 ),
			tbDrugCall[ 2 ],
			GetGameWordText( "ITEM_CALL_MAP", 1 ),
			tbDrugCall[ 3 ],
			tbDrugCall[ 4 ] )

	end

	return str

end

-- 내구도 관련gbgim
function GetItemDurability( tbItemData )

	local strTooltip = ""
	strTooltip = ItemToolTip_Ability_Damage( tbItemData["BASIC"]["Damage"] )
	strTooltip = AddNewLineFront( strTooltip, ItemToolTip_Ability_Defence( tbItemData["BASIC"]["Defence"] ) )
	strTooltip = AddNewLineFront( strTooltip, ItemToolTip_Ability_HitRate( tbItemData["BASIC"]["HitRate"] ) )
	strTooltip = AddNewLineFront( strTooltip, ItemToolTip_Ability_AvoidRate( tbItemData["BASIC"]["AvoidRate"] ) )
	strTooltip = AddNewLineFront( strTooltip, ItemToolTip_Ability_CostSp( tbItemData["BASIC"]["CostSp"] ) )

	-- 랜덤옵션
	local strRandomOp = ItemToolTip_Ability_RandomOption( tbItemData["RandomOpt"] )

	-- 가산옵션
	local strAddOp = ItemToolTip_Ability_AddOption( tbItemData["DuraAdd_AddOpt"] )

	-- 특수옵션
	--[[
	if 0 == string.len( strAddOp ) then
		strAddOp = ItemToolTip_Ability_SpecialOption( tbItemData["Dura_SpecialOpt"] )
	else
		strAddOp = AddNewLineFront( strAddOp, ItemToolTip_Ability_SpecialOption( tbItemData["Dura_SpecialOpt"] ) )
	end
	]]--

	-- 경험치 배율
	local strExp = ItemToolTip_Ability_ExpMultiple( tbItemData["Dura_ExpMultiple"] )
	-- 저항
	local strResist = ItemToolTip_Ability_Resist( tbItemData )

	-----------------------------------------------------------------------------------------

	--strTooltip = AddStringSection( strTooltip, strRandomOp )
	strTooltip = AddNewLineFront( strTooltip, strRandomOp )

	strTooltip = AddStringSection( strTooltip, strAddOp )
	strTooltip = AddNewLineFront( strTooltip, strAddOp )

	--strTooltip = AddStringSection( strTooltip, strExp )
	strTooltip = AddNewLineFront( strTooltip, strExp )

	--strTooltip = AddStringSection( strTooltip, strResist )
	strTooltip = AddNewLineFront( strTooltip, strResist )

	return strTooltip

end

function GetItemHeadDurability( tbItemData )
	local ToolTipText_Sum = ""

	if 0 ~= tbItemData["Head"]["Grade"]["Grade"] then

	end

	ToolTipText_Sum = GetGameWordText( "DURABILITY_ITEM" )
	ToolTipText_Sum = HTMLFontColor( ToolTipText_Sum, TranslateHTMLColor( tbItemData["Head"]["Grade"]["Grade"] ) ) .. HTML_NEWLINE	-- 아이템 등급별 색상 삽입
	return ToolTipText_Sum
end