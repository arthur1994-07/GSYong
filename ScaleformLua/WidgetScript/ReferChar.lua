
MAX_CHARACTER_SLOT_COUNT = 5

-- 아래 정보들 전역으로 두면 캐싱할텐데, 아깝자않은가?;
g_ReferChar_tbStat = {} 		-- 스탯정보를 담을 테이블
g_ReferChar_tbTabpoint = {} 	-- 스킬 탭별 포인트정보
g_ReferChar_Name = "" 			-- 정보창 타겟이름

g_nReferChar_NameLabelX = 0
g_nReferChar_NameLabelWidth = 0

g_nReferChar_OrigWndWidth = 0
g_nReferChar_RePosX = 0
g_nReferChar_RePosY = 0

g_bReferChar_InitData = false

function LoadReferChar()

	-- 원래 윈도우 사이즈
	g_nReferChar_OrigWndWidth = GetInt( WIDGET_ID_REFERCHAR, "width", "" )
	
	-- 부가능력이 초기에는 꺼진 상태니 윈도우 사이즈 새로 잡아주기
	local nAddOnViewWidth = GetInt( WIDGET_ID_REFERCHAR, "width", "viewAddOn" )
	SetInt( WIDGET_ID_REFERCHAR, "customWidth", g_nReferChar_OrigWndWidth - nAddOnViewWidth, "" )
	
	-- 필요 없는 컴포넌트들 visible 끄기
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat1Up" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat2Up" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat3Up" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat4Up" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat5Up" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat1Down" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat2Down" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat3Down" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat4Down" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnStat5Down" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnInit" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnApply" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.weaponSlot.radioLeft" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.weaponSlot.radioRight" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnHelp1" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.btnHelp2" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "viewAddOn.btnHelp" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.mcCountryImg" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.labelClub" )
	
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.tfAtb" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.tfAtbPoint" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.tfLiving" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.tfLivingPoint" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.tfCsb" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.tfCsbScore" )
	
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.skill_type_a" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.skill_type_b" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.skill_type_c" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.skill_type_d" )
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.skill_type_e" )
	
	g_nReferChar_NameLabelX = GetInt( WIDGET_ID_REFERCHAR, "x", "tabChar.labelLevelName" )
	g_nReferChar_NameLabelWidth = GetInt( WIDGET_ID_REFERCHAR, "width", "tabChar.labelLevelName" )
	
	-- 슬롯 바인딩
	for i = 0, 14, 1 do
		SetString( WIDGET_ID_REFERCHAR, "binding",
			SLOT_REFERCHAR_ID .. tostring( i ), "tabChar.slot" .. tostring( i ) )
	end	
	for i = 15, 18, 1 do
		SetString( WIDGET_ID_REFERCHAR, "binding",
			SLOT_REFERCHAR_ID .. tostring( i ), "tabChar.weaponSlot.slot" .. tostring( i ) )
	end
	
	-- 장비 슬롯 툴팁
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 0 ), "tabChar.slot0" )	-- 모자
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 1 ), "tabChar.slot1" )	-- 상의
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 19 ), "tabChar.slot2" )	-- 허리띠
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 2 ), "tabChar.slot3" )	-- 하의
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 3 ), "tabChar.slot4" )	-- 장갑
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 4 ), "tabChar.slot5" )	-- 신발
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 20 ), "tabChar.slot6" )	-- 귀걸이
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 22 ), "tabChar.slot7" )	-- 장식
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 6 ), "tabChar.slot8" )	-- 목걸이
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 7 ), "tabChar.slot9" )	-- 팔찌
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 8 ), "tabChar.slot10" )	-- 반지1
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 8 ), "tabChar.slot11" )	-- 반지2
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 21 ), "tabChar.slot12" )	-- 카드
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 21 ), "tabChar.slot13" )	-- 카드
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 5 ), "tabChar.weaponSlot.slot15" )	-- 무기1
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0 ), "tabChar.weaponSlot.slot16" )	-- 보조1
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 5 ), "tabChar.weaponSlot.slot17" )	-- 무기2
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0 ), "tabChar.weaponSlot.slot18" )	-- 보조2
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "PRIVATE_MARKET_ITEM_TYPE", 7 ), "tabChar.slot14" )	-- 탈것
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "CHARINFO_TOOLTIP_WEAPONSWAP", 0 ), "tabChar.weaponSlot.radioLeft" )	-- 무기슬롯1
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameWordText( "CHARINFO_TOOLTIP_WEAPONSWAP", 1 ), "tabChar.weaponSlot.radioRight" )	-- 무기슬롯2
	
	-- 스킬보기 버튼
	SetString( WIDGET_ID_REFERCHAR, "label", GetGameWordText( "SKILL_NAME_STATIC", 0 ), "tabChar.btnSkill" )
	SetEvent( WIDGET_ID_REFERCHAR, EVENT_BUTTON_CLICK, "Ev_ReferChar_ShowSkill", "", "tabChar.btnSkill" )
	
	-- 부가능력 표시
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 18 ), "tabChar.labelAddOn" )
	SetEvent( WIDGET_ID_REFERCHAR, EVENT_BUTTON_CLICK, "Ev_ReferChar_ShowAddOn", "", "tabChar.btnAddOn" )
	
	-- 힘, 민첩, 정신, 체력, 근력 툴팁
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_POW_TOOLTIP", 0 ), "tabChar.Stat1" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_DEX_TOOLTIP", 0 ), "tabChar.Stat2" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_SPI_TOOLTIP", 0 ), "tabChar.Stat3" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_STR_TOOLTIP", 0 ), "tabChar.Stat4" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_STA_TOOLTIP", 0 ), "tabChar.Stat5" )
	
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 0 ), "tabChar.Stat1.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 1 ), "tabChar.Stat2.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 2 ), "tabChar.Stat3.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 4 ), "tabChar.Stat4.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 5 ), "tabChar.Stat5.tfName" )
	
	-- 세부 능력치
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ATTACK_MIN_TOOLTIP", 0 ), "tabChar.Detail1" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ATTACK_MAX_TOOLTIP", 0 ), "tabChar.Detail2" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_DEFENSE_TOOLTIP", 0 ), "tabChar.Detail3" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_FIGHT_TOOLTIP", 0 ), "tabChar.Detail4" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_SHOOT_TOOLTIP", 0 ), "tabChar.Detail5" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_STAMINA_TOOLTIP", 0 ), "tabChar.Detail6" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_HIT_TOOLTIP", 0 ), "tabChar.Detail7" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_AVOID_TOOLTIP", 0 ), "tabChar.Detail8" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_HP_TOOLTIP", 0 ), "tabChar.Detail9" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_MP_TOOLTIP", 0 ), "tabChar.Detail10" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_SP_TOOLTIP", 0 ), "tabChar.Detail11" )
	
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 15 ), "tabChar.Detail1.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 14 ), "tabChar.Detail2.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 3 ), "tabChar.Detail3.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 4 ), "tabChar.Detail4.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 5 ), "tabChar.Detail5.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 11 ), "tabChar.Detail6.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 9 ), "tabChar.Detail7.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 10 ), "tabChar.Detail8.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "ITEM_ADDON_INFO", 5 ), "tabChar.Detail9.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "ITEM_ADDON_INFO", 6 ), "tabChar.Detail10.tfName" )
	SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "ITEM_ADDON_INFO", 7 ), "tabChar.Detail11.tfName" )
	
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "viewAddOn" )

	-- 탈것 정보 서버에서 안날려줌.
	SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.slot14" )

	SetEvent( WIDGET_ID_REFERCHAR, EVENT_BUTTON_CLICK, "Ev_ReferChar_ViewAddon_Page", "", "viewAddOn.radioPage1", tostring( 1 ) );
	SetEvent( WIDGET_ID_REFERCHAR, EVENT_BUTTON_CLICK, "Ev_ReferChar_ViewAddon_Page", "", "viewAddOn.radioPage2", tostring( 2 ) );
	
	SetBool( WIDGET_ID_REFERCHAR, "selected", true, "inventory.radioPage1" )
	Ev_ReferChar_ViewAddon_Page("1")
	
end

function OpenReferChar()

	if true == g_bReferChar_InitData then
		return
	end

	ReferChar_SetBasicStatData()
	ReferChar_SetBasicStatCompare()
	ReferChar_SetItem()
	SetBool( WIDGET_ID_REFERCHAR, "selected", true, "viewAddOn.radioPage1" )
	Ev_ReferChar_ViewAddon_Page(1)
	
	g_bReferChar_InitData = true
	
	-- 이전에 봤던 캐릭터 정보 이름 리스트
	local tbListData = {}
	local tbNameList = g_Refer:GetNameList( g_MyChar.dwServerID )
	for value in pairs( tbNameList ) do
	
		local tbData = 
		{
			tbNameList[ value ]
		}
		table.insert( tbListData, tbData )
	
	end
	
	local strParam = CreateParam( "label" )
	SetDataEx( WIDGET_ID_REFERCHAR, strParam, tbListData, "tabChar.comboBadge" )
	SetEvent( WIDGET_ID_REFERCHAR, EVENT_LIST_INDEX_CHANGE, "Ev_ReferChar_SelectName", "index", "tabChar.comboBadge" )
	SetInt( WIDGET_ID_REFERCHAR, "selectedIndex", 0, "tabChar.comboBadge" )
	
	for i = 0, MAX_CHARACTER_SLOT_COUNT-1, 1 do
		
		SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.__id"..tostring(i).."_" )
		
	end

end

-- 정보보기 데이터 설정
function ReferChar_SetBasicStatData()

	g_ReferChar_tbStat = g_Refer:GetInfoData()

	-- 타이틀
	local str = StringFormat( GetGameInternalText( "CHARACTER_WINDOW_MAIN_TITLE", 0 ), g_ReferChar_Name )
	SetString( WIDGET_ID_REFERCHAR, "title", str, "" )
	
	-- 레벨, 이름
	str = "Lv. " .. tostring( g_ReferChar_tbStat[ REFERCHAR_LEVEL ] ) .. " " .. g_ReferChar_Name
	SetString( WIDGET_ID_REFERCHAR, "text", str, "tabChar.labelLevelName" )
	CallFunction( WIDGET_ID_REFERCHAR, "validateNow", "", "tabChar.labelLevelName" )
	
	local nTextW = GetInt( WIDGET_ID_REFERCHAR, "textWidth", "tabChar.labelLevelName.textField" ) + 4
	-- 이름 길이가 충분하면 가운데 정렬
	if nTextW < g_nReferChar_NameLabelWidth then
	
		local nCenter = math.floor( (g_nReferChar_NameLabelWidth - nTextW) / 2 )
		SetInt( WIDGET_ID_REFERCHAR, "x", nCenter, "tabChar.labelLevelName" )
	
	-- 너무 길면 원래 위치로 초기화하고 툴팁으로 표기한다.
	else
	
		SetInt( WIDGET_ID_REFERCHAR, "x", g_nReferChar_NameLabelX, "tabChar.labelLevelName" )
	
	end
	
	-- 성별 이미지 설정
	if 0 == g_ReferChar_tbStat[ REFERCHAR_SEX ] then
	
		CallFunction( WIDGET_ID_REFERCHAR, "gotoAndStop", "female", "tabChar.mcGender" )
	
	else
	
		CallFunction( WIDGET_ID_REFERCHAR, "gotoAndStop", "male", "tabChar.mcGender" )
	
	end
	
	-- 스탯
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_POW ] ), "tabChar.Stat1.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_DEX ] ), "tabChar.Stat2.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_SPI ] ), "tabChar.Stat3.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_STR ] ), "tabChar.Stat4.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_STA ] ), "tabChar.Stat5.tfValue" )
	
	-- 세부 능력치
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_ATTACK_MIN ] ), "tabChar.Detail1.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_ATTACK_MAX ] ), "tabChar.Detail2.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_DEFENCE ] ), "tabChar.Detail3.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_MELEE ] ), "tabChar.Detail4.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_RANGE ] ), "tabChar.Detail5.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_MAGIC ] ), "tabChar.Detail6.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_HIT ] ), "tabChar.Detail7.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_AVOID ] ), "tabChar.Detail8.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_HP ] ), "tabChar.Detail9.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_MP ] ), "tabChar.Detail10.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( g_ReferChar_tbStat[ REFERCHAR_SP ] ), "tabChar.Detail11.tfValue" )
	
	-- 학원, 클래스
	local nClassIndex = GetCharClassFlagIndex( g_ReferChar_tbStat[ REFERCHAR_CLASS ] )
	local strSchoolClass = GetGameWordText( "FULL_ACADEMY_NAME", g_ReferChar_tbStat[ REFERCHAR_SCHOOL ] ) .. ", " .. 
		GetCharClassIndexName( nClassIndex )
	SetString( WIDGET_ID_REFERCHAR, "text", strSchoolClass, "tabChar.tfSchoolClass" )
	
	if 	CHARCLASS_IDX_EXTREAM_M == nClassIndex or 
		CHARCLASS_IDX_EXTREAM_W == nClassIndex then
		
		SetBool( WIDGET_ID_REFERCHAR, "visible", true, "tabChar.weaponSlot.slot17" )
		SetBool( WIDGET_ID_REFERCHAR, "visible", true, "tabChar.weaponSlot.slot18" )
		
	else
	
		SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.weaponSlot.slot17" )
		SetBool( WIDGET_ID_REFERCHAR, "visible", false, "tabChar.weaponSlot.slot18" )
		
	end
	
end

-- 내 캐릭터와 비교
function ReferChar_SetBasicStatCompare()

	-- 스탯
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetPow(), g_ReferChar_tbStat[ REFERCHAR_POW ], "tabChar.StatCompare1.mcIcon", "tabChar.StatCompare1.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetDex(), g_ReferChar_tbStat[ REFERCHAR_DEX ], "tabChar.StatCompare2.mcIcon", "tabChar.StatCompare2.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetSpi(), g_ReferChar_tbStat[ REFERCHAR_SPI ], "tabChar.StatCompare3.mcIcon", "tabChar.StatCompare3.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetStr(), g_ReferChar_tbStat[ REFERCHAR_STR ], "tabChar.StatCompare4.mcIcon", "tabChar.StatCompare4.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetSta(), g_ReferChar_tbStat[ REFERCHAR_STA ], "tabChar.StatCompare5.mcIcon", "tabChar.StatCompare5.tfCompare" )
	
	-- 세부 능력치
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetForce_Low(), g_ReferChar_tbStat[ REFERCHAR_ATTACK_MIN ], "tabChar.Detail1.mcIcon", "tabChar.Detail1.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetForce_High(), g_ReferChar_tbStat[ REFERCHAR_ATTACK_MAX ], "tabChar.Detail2.mcIcon", "tabChar.Detail2.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetDefense(), g_ReferChar_tbStat[ REFERCHAR_DEFENCE ], "tabChar.Detail3.mcIcon", "tabChar.Detail3.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetSumPowerAttribute( 0 ), g_ReferChar_tbStat[ REFERCHAR_MELEE ], "tabChar.Detail4.mcIcon", "tabChar.Detail4.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetSumPowerAttribute( 1 ), g_ReferChar_tbStat[ REFERCHAR_RANGE ], "tabChar.Detail5.mcIcon", "tabChar.Detail5.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetSumPowerAttribute( 2 ), g_ReferChar_tbStat[ REFERCHAR_MAGIC ], "tabChar.Detail6.mcIcon", "tabChar.Detail6.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetHit(), g_ReferChar_tbStat[ REFERCHAR_HIT ], "tabChar.Detail7.mcIcon", "tabChar.Detail7.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetAvoid(), g_ReferChar_tbStat[ REFERCHAR_AVOID ], "tabChar.Detail8.mcIcon", "tabChar.Detail8.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetMaxHP(), g_ReferChar_tbStat[ REFERCHAR_HP ], "tabChar.Detail9.mcIcon", "tabChar.Detail9.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetMaxMP(), g_ReferChar_tbStat[ REFERCHAR_MP ], "tabChar.Detail10.mcIcon", "tabChar.Detail10.tfChange" )
	ReferChar_SetCompareTextIcon( false, false, g_MyChar:GetMaxSP(), g_ReferChar_tbStat[ REFERCHAR_SP ], "tabChar.Detail11.mcIcon", "tabChar.Detail11.tfChange" )

end

-- 비교 텍스트, 아이콘 설정
function ReferChar_SetCompareTextIcon( bDecimal, bMulti100, nMyStat, nCompareStat, strIconInst, strValueInst )

	local nCompare = nMyStat - nCompareStat
	
	local strIcon = ""
	local strColor = ""
	
	if 0 == nCompare then
	
		strIcon = "-"
		strColor = HTML_TEXT_COLOR_WHITE
		
	elseif 0 < nCompare then
	
		strIcon = HTMLImage( "UPPER_ARROW", 15, 15 )
		strColor = HTML_TEXT_COLOR_GREEN
		
	else
	
		strIcon = HTMLImage( "LOWER_ARROW", 15, 15 )
		strColor = HTML_TEXT_COLOR_RED
		
	end
	
	local strAnim = "none"
	if 0 > nCompare then
		strAnim = "DOWN"
	elseif 0 < nCompare then
		strAnim = "UP"
	end
	CallFunction( WIDGET_ID_REFERCHAR, "gotoAndStop", strAnim, strIconInst )
	
	if true == bMulti100 then
	
		nCompare = nCompare * 100
	
	end
	
	if true == bDecimal then
	
		SetString( WIDGET_ID_REFERCHAR, "htmlText", 
			HTMLFontColor( string.format( "%.2f%%", nCompare ), strColor ), strValueInst )
			
	else
	
		SetString( WIDGET_ID_REFERCHAR, "htmlText", 
			HTMLFontColor( tostring( nCompare ), strColor ), strValueInst )
			
	end

end

-- 아이템 설정
function ReferChar_SetItem()

	local tbItems = g_Refer:GetPutOnItems()
	for value in pairs( tbItems ) do
	
		local tbItem = tbItems[ value ]
		SetSlot_ReferChar( CharSlotToID( SLOT_REFERCHAR_ID, value - 1 ), value - 1 )
	
	end

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 부가 능쳑치 펼침/접기
function Ev_ReferChar_ShowAddOn()

	local bSelect = GetBool( WIDGET_ID_REFERCHAR, "selected", "tabChar.btnAddOn" )
	
	SetBool( WIDGET_ID_REFERCHAR, "visible", bSelect, "viewAddOn" )
	
	-- 윈도우 사이즈 새로 잡아주기
	if true == bSelect then
		
		SetInt( WIDGET_ID_REFERCHAR, "customWidth", g_nReferChar_OrigWndWidth, "" )
		
		local nX = GetInt( WIDGET_ID_REFERCHAR, "x", "" )
		local nScreeenWidth = GetScreenWidth()
		
		if nScreeenWidth < nX + g_nReferChar_OrigWndWidth then
		
			SetInt( WIDGET_ID_REFERCHAR, "x", nScreeenWidth - g_nReferChar_OrigWndWidth, "" )
		
		end
	
	else
	
		local nAddOnViewWidth = GetInt( WIDGET_ID_REFERCHAR, "width", "viewAddOn" )
		SetInt( WIDGET_ID_REFERCHAR, "customWidth", g_nReferChar_OrigWndWidth - nAddOnViewWidth, "" )
	
	end

end

-- 스킬보기
function Ev_ReferChar_ShowSkill()

	g_nReferChar_RePosX = GetInt( WIDGET_ID_REFERCHAR, "x", "" )
	g_nReferChar_RePosY = GetInt( WIDGET_ID_REFERCHAR, "y", "" )

	OpenWidget( WIDGET_ID_REFERCHAR_SKILL )
	CloseWidget( WIDGET_ID_REFERCHAR )

end

-- 이름 선택
function Ev_ReferChar_SelectName( nIndex )

	if -1 == nIndex then
		return
	end

	local strName = GetDataString( WIDGET_ID_REFERCHAR, nIndex, "label", "tabChar.comboBadge" )
	
	-- 해당 캐릭터 정보 요청
	g_Msg:ReqReferChar( strName )
	g_ReferChar_Name = strName

end

----------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 정보보기창 캐릭터 이름 설정
function ReferChar_SetName( strName )

	g_ReferChar_Name = strName

end

-- 정보보기창 캐릭터 이름 얻기
function ReferChar_GetName()

	if nil == g_ReferChar_Name then
	
		return ""
		
	else
	
		return g_ReferChar_Name
		
	end
	
end

-- 가산능력치 기본 텍스트 설정
function ReferChar_ViewAddonPage1_DefaultStringSet()
	
	for i = 1, 23, 1 do
		SetBool( WIDGET_ID_REFERCHAR, "visible", true, "viewAddOn.addOn"..tostring(i) )
		SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", i+15 ), "viewAddOn.addOn"..tostring(i)..".tfName" )	
	
	end	
	
	ReferChar_ViewAddonPage1_ResetChangeUpAddon()
	
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_CRITICAL_TOOLTIP", 0 ), "viewAddOn.addOn1" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_CRUSHING_BLOW_TOOLTIP", 0 ), "viewAddOn.addOn2" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_MOVE_SPEED_TOOLTIP", 0 ), "viewAddOn.addOn3" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_ATK_SPEED_TOOLTIP", 0 ), "viewAddOn.addOn4" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_EXP_MULTIPLE_TOOLTIP", 0 ), "viewAddOn.addOn5" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_HP_TOOLTIP", 0 ), "viewAddOn.addOn6" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_MP_TOOLTIP", 0 ), "viewAddOn.addOn7" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_SP_TOOLTIP", 0 ), "viewAddOn.addOn8" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_CP_TOOLTIP", 0 ), "viewAddOn.addOn9" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_INT_HP_TOOLTIP", 0 ), "viewAddOn.addOn10" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_INT_MP_TOOLTIP", 0 ), "viewAddOn.addOn11" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_INT_SP_TOOLTIP", 0 ), "viewAddOn.addOn12" )
	
	local tbLmtInfo = g_MyChar:GetCharInfoDetailAddonLimitInfo()
	
	local strTooltip_00, strTooltip_01, strTooltip_02 , strTooltip_03
	strTooltip_00 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 0 )
	strTooltip_01 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 1 )
	strTooltip_00 = StringFormat(strTooltip_00, SetFloatToString(tbLmtInfo[18]*100,0) )
	strTooltip_01 = StringFormat(strTooltip_01, SetFloatToString(tbLmtInfo[19],0) )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTooltip_00, "viewAddOn.addOn18" )	
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTooltip_01, "viewAddOn.addOn19" )	
	
	strTooltip_00 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 2 )
	strTooltip_01 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 3 )
	strTooltip_02 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 4 )
	strTooltip_03 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 5 )
	strTooltip_00 = StringFormat(strTooltip_00, SetFloatToString(g_fEmergency_MobHPR*100, 0),  SetFloatToString(tbLmtInfo[20]*100,0))
	strTooltip_01 = StringFormat(strTooltip_01, SetFloatToString(g_fEmergency_BossHPR*100, 0), SetFloatToString(tbLmtInfo[21]*100,0))
	strTooltip_02 = StringFormat(strTooltip_02, SetFloatToString(g_fEmergency_CharHPR*100, 0), SetFloatToString(tbLmtInfo[22]*100,0))		
	strTooltip_03 = StringFormat(strTooltip_03, SetFloatToString(tbLmtInfo[23]*100,0) )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTooltip_00, "viewAddOn.addOn20" )	
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTooltip_01, "viewAddOn.addOn21" )	
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTooltip_02, "viewAddOn.addOn22" )	
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTooltip_03, "viewAddOn.addOn23" )	
	
	
	-- 저항 최대값
	local nPVPMax = math.floor( g_fREGIST_MAX_G / g_fRESIST_G * 100 )
	local nPVEMax = math.floor( g_fRESIST_MAX_CROW / g_fRESIST_G * 100 )
	
	-- 화염, 냉기, 전기, 맹독, 정기
	local strFire, strCold, strElec, strPisn, strSprt
		= GetGameInternalText( "CHARACTER_WINDOW_FIRE_RESIST_TOOLTIP_C", 0 )
		, GetGameInternalText( "CHARACTER_WINDOW_COLD_RESIST_TOOLTIP_C", 0 )
		, GetGameInternalText( "CHARACTER_WINDOW_ELECTRIC_RESIST_TOOLTIP_C", 0 )
		, GetGameInternalText( "CHARACTER_WINDOW_POISON_RESIST_TOOLTIP_C", 0 )
		, GetGameInternalText( "CHARACTER_WINDOW_SPIRIT_RESIST_TOOLTIP_C", 0 )
	strFire = StringFormat( strFire, nPVPMax, nPVEMax )
	strCold = StringFormat( strCold, nPVPMax, nPVEMax )
	strElec = StringFormat( strElec, nPVPMax, nPVEMax )
	strPisn = StringFormat( strPisn, nPVPMax, nPVEMax )
	strSprt = StringFormat( strSprt, nPVPMax, nPVEMax )
	
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strFire, "viewAddOn.addOn13" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strCold, "viewAddOn.addOn14" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strElec, "viewAddOn.addOn15" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strPisn, "viewAddOn.addOn16" )
	SetString( WIDGET_ID_REFERCHAR, "TooltipText", strSprt, "viewAddOn.addOn17" )
		
end

-- 캐릭터 능력치 적용
function ReferChar_ViewAddonPage1_CharAbilitySet()

	local tbAddon = g_Refer:GetCharInfoDetailAddonInfo()
	-- [크리티컬][강한타격][이동속도, 공격속도 증가율][경험치 증가율][HP, MP, SP, CP 회복율];
	-- [포션 HP, MP, SP 증가량];
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["CriticalRate"] * 100 ), "viewAddOn.addOn1.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["CrushingBlow"] * 100 ), "viewAddOn.addOn2.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["MoveSpeed"]	* 100 - 100 ), "viewAddOn.addOn3.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["AtkSpeed"]	* 100 - 100 ), "viewAddOn.addOn4.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["ExpRate"]		* 100 - 100 ), "viewAddOn.addOn5.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncrHpTotal"]	* 100 ), "viewAddOn.addOn6.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncrMpTotal"]	* 100 ), "viewAddOn.addOn7.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncrSpTotal"]	* 100 ), "viewAddOn.addOn8.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["ReceiveCP"]	* 100 ), "viewAddOn.addOn9.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tbAddon["PotionHpInc"], "viewAddOn.addOn10.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tbAddon["PotionMpInc"], "viewAddOn.addOn11.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tbAddon["PotionSpInc"], "viewAddOn.addOn12.tfValue" )

	-- [저항 화염, 냉기, 전기, 맹독, 정기]
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( tbAddon["ResistFire"] ), "viewAddOn.addOn13.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( tbAddon["ResistCold"] ), "viewAddOn.addOn14.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( tbAddon["ResistElec"] ), "viewAddOn.addOn15.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( tbAddon["ResistPisn"] ), "viewAddOn.addOn16.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( tbAddon["ResistSprt"] ), "viewAddOn.addOn17.tfValue" )
	
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncR_Ignore_BossDamage"] * 100 ),		"viewAddOn.addOn18.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tostring( tbAddon["Inc_BossFixedDamage"] ),								"viewAddOn.addOn19.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncR_Critical_EmergencyMob"] * 100 ),	"viewAddOn.addOn20.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncR_Critical_EmergencyBoss"] * 100 ),	"viewAddOn.addOn21.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["Dec_Damage_EmergencyState"] * 100 ),						"viewAddOn.addOn22.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncR_DropGenMoney"] * 100 ),			"viewAddOn.addOn23.tfValue" )
																  	
end												

-- 임시 적용 스탯 비교 후 원래대로 돌리는 기능
function ReferChar_ViewAddonPage1_ResetChangeUpAddon()
	-- 실수
	for i = 1, 23, 1 do
		SetString( WIDGET_ID_REFERCHAR, "text", "0.00%", "viewAddOn.addOn" .. tostring(i) .. ".tfCompare" )
		CallFunction( WIDGET_ID_REFERCHAR, "gotoAndStop", "NONE", "viewAddOn.addOn" .. tostring( i ) .. ".mcIcon" )
		
	end
	-- 정수
	for i = 10, 17, 1 do
		SetString( WIDGET_ID_REFERCHAR, "text", 0, "viewAddOn.addOn" .. tostring(i) .. ".tfCompare" )
	end
	SetString( WIDGET_ID_REFERCHAR, "text", 0, "viewAddOn.addOn19.tfCompare" )
	SetString( WIDGET_ID_REFERCHAR, "text", 0, "viewAddOn.addOn22.tfCompare" )
	
end

-- 내 캐릭터와 비교
function ReferChar_ViewAddonPage1_Compare()

	local tbMyChar = g_MyChar:GetCharInfoDetailAddonInfo()
	local tbRefer = g_Refer:GetCharInfoDetailAddonInfo() 
	
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["CriticalRate"],	tbRefer["CriticalRate"],	"viewAddOn.addOn1.mcIcon", "viewAddOn.addOn1.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["CrushingBlow"],	tbRefer["CrushingBlow"],	"viewAddOn.addOn2.mcIcon", "viewAddOn.addOn2.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["MoveSpeed"],		tbRefer["MoveSpeed"],		"viewAddOn.addOn3.mcIcon", "viewAddOn.addOn3.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["AtkSpeed"],		tbRefer["AtkSpeed"],		"viewAddOn.addOn4.mcIcon", "viewAddOn.addOn4.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["ExpRate"],		tbRefer["ExpRate"],			"viewAddOn.addOn5.mcIcon", "viewAddOn.addOn5.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncrHpTotal"],	tbRefer["IncrHpTotal"],		"viewAddOn.addOn6.mcIcon", "viewAddOn.addOn6.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncrMpTotal"],	tbRefer["IncrMpTotal"],		"viewAddOn.addOn7.mcIcon", "viewAddOn.addOn7.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncrSpTotal"],	tbRefer["IncrSpTotal"],		"viewAddOn.addOn8.mcIcon", "viewAddOn.addOn8.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["ReceiveCP"],		tbRefer["ReceiveCP"],		"viewAddOn.addOn9.mcIcon", "viewAddOn.addOn9.tfCompare" )
	ReferChar_SetCompareTextIcon( true, false,	tbMyChar["PotionHpInc"],	tbRefer["PotionHpInc"], 	"viewAddOn.addOn10.mcIcon", "viewAddOn.addOn10.tfCompare" )
	ReferChar_SetCompareTextIcon( true, false,	tbMyChar["PotionMpInc"],	tbRefer["PotionMpInc"], 	"viewAddOn.addOn11.mcIcon", "viewAddOn.addOn11.tfCompare" )
	ReferChar_SetCompareTextIcon( true, false,	tbMyChar["PotionSpInc"],	tbRefer["PotionSpInc"], 	"viewAddOn.addOn12.mcIcon", "viewAddOn.addOn12.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false, tbMyChar["ResistFire"],		tbRefer["ResistFire"],		"viewAddOn.addOn13.mcIcon", "viewAddOn.addOn13.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false,	tbMyChar["ResistCold"],		tbRefer["ResistCold"],		"viewAddOn.addOn14.mcIcon", "viewAddOn.addOn14.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false,	tbMyChar["ResistElec"],		tbRefer["ResistElec"],		"viewAddOn.addOn15.mcIcon", "viewAddOn.addOn15.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false,	tbMyChar["ResistPisn"],		tbRefer["ResistPisn"],		"viewAddOn.addOn16.mcIcon", "viewAddOn.addOn16.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false,	tbMyChar["ResistSprt"],		tbRefer["ResistSprt"],		"viewAddOn.addOn17.mcIcon", "viewAddOn.addOn17.tfCompare" )	
	
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncR_Ignore_BossDamage"],			tbRefer["IncR_Ignore_BossDamage"], 		"viewAddOn.addOn18.mcIcon", "viewAddOn.addOn18.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false, tbMyChar["Inc_BossFixedDamage"],			tbRefer["Inc_BossFixedDamage"],			"viewAddOn.addOn19.mcIcon", "viewAddOn.addOn19.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncR_Critical_EmergencyMob"],		tbRefer["IncR_Critical_EmergencyMob"],	"viewAddOn.addOn20.mcIcon", "viewAddOn.addOn20.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncR_Critical_EmergencyBoss"],	tbRefer["IncR_Critical_EmergencyBoss"],	"viewAddOn.addOn21.mcIcon", "viewAddOn.addOn21.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["Dec_Damage_EmergencyState"],		tbRefer["Dec_Damage_EmergencyState"],	"viewAddOn.addOn22.mcIcon", "viewAddOn.addOn22.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncR_DropGenMoney"],				tbRefer["IncR_DropGenMoney"],			"viewAddOn.addOn23.mcIcon", "viewAddOn.addOn23.tfCompare" )	

end

-- 가산능력치 기본 텍스트 설정
function ReferChar_ViewAddonPage2_DefaultStringSet()	
	
	for i = 1, 18, 1 do
		SetBool( WIDGET_ID_REFERCHAR, "visible", true, "viewAddOn.addOn"..tostring(i) )		
		SetString( WIDGET_ID_REFERCHAR, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", i+38 ), "viewAddOn.addOn" .. tostring(i) .. ".tfName" )
	end
	ReferChar_ViewAddonPage2_ResetChangeUpAddon()
	
	for i = 19, 23, 1 do
		SetBool( WIDGET_ID_REFERCHAR, "visible", false, "viewAddOn.addOn"..tostring(i) )
	end
	
	local tbLmtInfo = g_MyChar:GetCharInfoDetailAddonLimitInfo()
	-- 툴팁 세팅;
	for i = 1, 18, 1 do
		local strTip = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", i + 5 )
		
		local nLmtInfoIndex = i+23
		local nValue = tbLmtInfo[nLmtInfoIndex]
		local fValue = tbLmtInfo[nLmtInfoIndex] * 100
		
		-- 기존의 처리와 같이 하드코딩으로 처리했다
		-- 관련부분은 처음에만 잘 조정하면 거의 변경될 일이 없기에 이렇게 한것으로 보인다
		-- 수정이 필요할 경우 버그가 생길 확률이 너무 높은부분이며 사실 개선이 필요하다
		if 27 == nLmtInfoIndex or 28 == nLmtInfoIndex then
			
			strTip = StringFormat(strTip, SetFloatToString(nValue,0) )
		
		else
		
			strTip = StringFormat(strTip, SetFloatToString(fValue,0) )
		
		end
		
		SetString( WIDGET_ID_REFERCHAR, "TooltipText", strTip, "viewAddOn.addOn"..tostring(i) )	
		
	end
	
end

function ReferChar_ViewAddonPage2_CharAbilitySet()

	local tbAddon = g_Refer:GetCharInfoDetailAddonInfo()
	-- 실수
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["DecR_Damage_MeleeSkill"] * 100 ),	"viewAddOn.addOn1.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["DecR_Damage_RangeSkill"] * 100 ),	"viewAddOn.addOn2.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["DecR_Damage_MagicSkill"] * 100 ),	"viewAddOn.addOn3.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncR_AtkSpeed_SoloPlay"] * 100 ),	"viewAddOn.addOn6.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["IncR_AtkSpeed_PartyPlay"] * 100 ),	"viewAddOn.addOn7.tfValue" )
	-- 정수
	SetString( WIDGET_ID_REFERCHAR, "text", tbAddon["Inc_HP_SoloPlay"], "viewAddOn.addOn4.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", tbAddon["Inc_HP_PartyPlay"], "viewAddOn.addOn5.tfValue" )

	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_CriticalDamage"		] * 100 ),	"viewAddOn.addOn8.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_CrushingBlow"			] * 100 ),	"viewAddOn.addOn9.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_LifePerKill"			] * 100 ),	"viewAddOn.addOn10.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_MPPerKill"			] * 100 ),	"viewAddOn.addOn11.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_SPPerKill"			] * 100 ),	"viewAddOn.addOn12.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_HPMPSPPerKill"		] * 100 ),	"viewAddOn.addOn13.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_AllMPRatioToAttack"	] * 100 ),	"viewAddOn.addOn14.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_AllSPRatioToHp"		] * 100 ),	"viewAddOn.addOn15.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_AllHitRatioToMelee"	] * 100 ),	"viewAddOn.addOn16.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_AllHitRatioToRange"	] * 100 ),	"viewAddOn.addOn17.tfValue" )
	SetString( WIDGET_ID_REFERCHAR, "text", string.format( "%.2f%%", tbAddon["GetIncR_AllHitRatioToMagic"	] * 100 ),	"viewAddOn.addOn18.tfValue" )
	
end

-- 임시 적용 스탯 비교 후 원래대로 돌리는 기능
function ReferChar_ViewAddonPage2_ResetChangeUpAddon()
	-- 실수
	for i = 1, 18, 1 do
		SetString( WIDGET_ID_REFERCHAR, "text", "0.00%", "viewAddOn.addOn" .. tostring(i) .. ".tfCompare" )
		CallFunction( WIDGET_ID_REFERCHAR, "gotoAndStop", "NONE", "viewAddOn.addOn" .. tostring( i ) .. ".mcIcon" )
	end
	
	-- 정수
	SetString( WIDGET_ID_REFERCHAR, "text", "0", "viewAddOn.addOn4.tfCompare" )
	SetString( WIDGET_ID_REFERCHAR, "text", "0", "viewAddOn.addOn5.tfCompare" )
	
end

-- 내 캐릭터와 비교
function ReferChar_ViewAddonPage2_Compare()

	local tbMyChar = g_MyChar:GetCharInfoDetailAddonInfo()
	local tbRefer = g_Refer:GetCharInfoDetailAddonInfo() 
	
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["DecR_Damage_MeleeSkill"],		tbRefer["DecR_Damage_MeleeSkill"],	"viewAddOn.addOn1.mcIcon", "viewAddOn.addOn1.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["DecR_Damage_RangeSkill"],		tbRefer["DecR_Damage_RangeSkill"],	"viewAddOn.addOn2.mcIcon", "viewAddOn.addOn2.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["DecR_Damage_MagicSkill"],		tbRefer["DecR_Damage_MagicSkill"],	"viewAddOn.addOn3.mcIcon", "viewAddOn.addOn3.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncR_AtkSpeed_SoloPlay"],		tbRefer["IncR_AtkSpeed_SoloPlay"],	"viewAddOn.addOn6.mcIcon", "viewAddOn.addOn6.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["IncR_AtkSpeed_PartyPlay"],	tbRefer["IncR_AtkSpeed_PartyPlay"],	"viewAddOn.addOn7.mcIcon", "viewAddOn.addOn7.tfCompare" )
	
	ReferChar_SetCompareTextIcon( false, false,	tbMyChar["Inc_HP_SoloPlay"],	tbRefer["Inc_HP_SoloPlay"],			"viewAddOn.addOn4.mcIcon", "viewAddOn.addOn4.tfCompare" )
	ReferChar_SetCompareTextIcon( false, false,	tbMyChar["Inc_HP_PartyPlay"],	tbRefer["Inc_HP_PartyPlay"],		"viewAddOn.addOn5.mcIcon", "viewAddOn.addOn5.tfCompare" )
	
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_CriticalDamage"		],		tbRefer["GetIncR_CriticalDamage"	],	"viewAddOn.addOn8.mcIcon", "viewAddOn.addOn8.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_CrushingBlow"			],		tbRefer["GetIncR_CrushingBlow"		],	"viewAddOn.addOn9.mcIcon", "viewAddOn.addOn9.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_LifePerKill"			],		tbRefer["GetIncR_LifePerKill"		],	"viewAddOn.addOn10.mcIcon", "viewAddOn.addOn10.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_MPPerKill"			],		tbRefer["GetIncR_MPPerKill"			],	"viewAddOn.addOn11.mcIcon", "viewAddOn.addOn11.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_SPPerKill"			],		tbRefer["GetIncR_SPPerKill"			],	"viewAddOn.addOn12.mcIcon", "viewAddOn.addOn12.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_HPMPSPPerKill"		],		tbRefer["GetIncR_HPMPSPPerKill"		],	"viewAddOn.addOn13.mcIcon", "viewAddOn.addOn13.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_AllMPRatioToAttack"	],		tbRefer["GetIncR_AllMPRatioToAttack"],	"viewAddOn.addOn14.mcIcon", "viewAddOn.addOn14.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_AllSPRatioToHp"		],		tbRefer["GetIncR_AllSPRatioToHp"	],	"viewAddOn.addOn15.mcIcon", "viewAddOn.addOn15.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_AllHitRatioToMelee"	],		tbRefer["GetIncR_AllHitRatioToMelee"],	"viewAddOn.addOn16.mcIcon", "viewAddOn.addOn16.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_AllHitRatioToRange"	],		tbRefer["GetIncR_AllHitRatioToRange"],	"viewAddOn.addOn17.mcIcon", "viewAddOn.addOn17.tfCompare" )
	ReferChar_SetCompareTextIcon( true, true,	tbMyChar["GetIncR_AllHitRatioToMagic"	],		tbRefer["GetIncR_AllHitRatioToMagic"],	"viewAddOn.addOn18.mcIcon", "viewAddOn.addOn18.tfCompare" )
	
end

-- 가산 옵션 자세히 보기 페이지 선택
function Ev_ReferChar_ViewAddon_Page( strIndex )
	
	local nIndex = tonumber( strIndex )
	if 		nIndex == 1 then
		ReferChar_ViewAddonPage1_DefaultStringSet()
		ReferChar_ViewAddonPage1_CharAbilitySet()
		ReferChar_ViewAddonPage1_Compare()
		
	elseif	nIndex == 2 then
		ReferChar_ViewAddonPage2_DefaultStringSet()
		ReferChar_ViewAddonPage2_CharAbilitySet()
		ReferChar_ViewAddonPage2_Compare()
		
	end
	
end
