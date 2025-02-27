
MAX_CHARACTER_SLOT_COUNT = 5
CHANGE_CHARACTER_SLOT_WAITTIME = 10

g_nCharInfo_NameLabelX = 0
g_nCharInfo_NameLabelWidth = 0
g_nCharInfo_ClubLabelX = 0
g_nCharInfo_ClubLabelWidth = 0
 
-- 스탯 포인트 투자 수치
g_CharInfo_VirtualStat1 = 0
g_CharInfo_VirtualStat2 = 0
g_CharInfo_VirtualStat3 = 0
g_CharInfo_VirtualStat4 = 0
g_CharInfo_VirtualStat5 = 0

-- 원래 윈도우 사이즈
g_nCharInfo_OrigWndWidth = 0

-- 캐릭터 세부정보 UI 로드완료
function LoadCharInfo()

	-- 닫기
	SetEvent( WIDGET_ID_CHARINFO, EVENT_WIDGET_CLOSE, "CloseCharInfo", "", "" )
	
	-- 원래 윈도우 사이즈
	g_nCharInfo_OrigWndWidth = GetInt( WIDGET_ID_CHARINFO, "width", "" )
	
	g_nCharInfo_NameLabelX = GetInt( WIDGET_ID_CHARINFO, "x", "tabChar.labelLevelName" )
	g_nCharInfo_NameLabelWidth = GetInt( WIDGET_ID_CHARINFO, "width", "tabChar.labelLevelName" )
	g_nCharInfo_ClubLabelX = GetInt( WIDGET_ID_CHARINFO, "x", "tabChar.labelClub" )
	g_nCharInfo_ClubLabelWidth = GetInt( WIDGET_ID_CHARINFO, "width", "tabChar.labelClub" )

	-- 타이틀
	local strTitle = GetGameInternalText( "CHARACTER_WINDOW_MAIN_TITLE", 0 )
	strTitle = StringFormat( strTitle, g_MyChar:GetName() )
	SetString( WIDGET_ID_CHARINFO, "title", strTitle, "" )
	
	-- 슬롯 바인딩 ID 설정
	local strSlotInst = "tabChar.slot"
	for i = 0, 14, 1 do
		SetString( WIDGET_ID_CHARINFO, "binding", SLOT_CHARINFO_ID .. tostring( i ), strSlotInst .. tostring( i ) )
		SetBool( WIDGET_ID_CHARINFO, "holdable", true, strSlotInst .. tostring( i ) )
	end
	
	strSlotInst = "tabChar.weaponSlot.slot"
	for i = 15, 18, 1 do
	
		SetString( WIDGET_ID_CHARINFO, "binding", SLOT_CHARINFO_ID .. tostring( i ), strSlotInst .. tostring( i ) )
		SetBool( WIDGET_ID_CHARINFO, "holdable", true, strSlotInst .. tostring( i ) )
		
	end
	
	-- 스킬보기 버튼 visible 끄기 (정보보기 창에서만 사용)
	SetBool( WIDGET_ID_CHARINFO, "visible", false, "tabChar.btnSkill" )
	
	-- 성별 이미지 설정
	if 0 == g_MyChar.m_wSex then
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "female", "tabChar.mcGender" )
	
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "male", "tabChar.mcGender" )
	
	end
	
	-- 뱃지 타이틀
	SetEvent( WIDGET_ID_CHARINFO, EVENT_LIST_INDEX_CHANGE, "Ev_CharInfo_Title_Index", "index;", "tabChar.comboBadge" )
	
	-- 장비 슬롯 툴팁
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 0 ), "tabChar.slot0" )	-- 모자
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 1 ), "tabChar.slot1" )	-- 상의
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 19 ), "tabChar.slot2" )	-- 허리띠
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 2 ), "tabChar.slot3" )	-- 하의
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 3 ), "tabChar.slot4" )	-- 장갑
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 4 ), "tabChar.slot5" )	-- 신발
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 20 ), "tabChar.slot6" )	-- 귀걸이
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 22 ), "tabChar.slot7" )	-- 장식
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 6 ), "tabChar.slot8" )	-- 목걸이
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 7 ), "tabChar.slot9" )	-- 팔찌
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 8 ), "tabChar.slot10" )	-- 반지1
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 8 ), "tabChar.slot11" )	-- 반지2
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 21 ), "tabChar.slot12" )	-- 카드
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 21 ), "tabChar.slot13" )	-- 카드
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 5 ), "tabChar.weaponSlot.slot15" )	-- 무기1
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0 ), "tabChar.weaponSlot.slot16" )	-- 보조1
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "ITEM_INFO_SET_PARTS", 5 ), "tabChar.weaponSlot.slot17" )	-- 무기2
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0 ), "tabChar.weaponSlot.slot18" )	-- 보조2
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "PRIVATE_MARKET_ITEM_TYPE", 7 ), "tabChar.slot14" )	-- 탈것
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "CHARINFO_TOOLTIP_WEAPONSWAP", 0 ), "tabChar.weaponSlot.radioLeft" )	-- 무기슬롯1
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "CHARINFO_TOOLTIP_WEAPONSWAP", 1 ), "tabChar.weaponSlot.radioRight" )	-- 무기슬롯2
	
	-- 학원, 클래스, 캐릭터 슬롯
	local strSchoolClass = GetGameWordText( "FULL_ACADEMY_NAME", g_MyChar:GetSchool() ) .. ", " .. 
		GetCharClassIndexName( g_MyChar.nClassIndex ) .. ", " .. GetGameWordText( "CHARACTER_SLOT_NAME", g_MyChar:GetCurrentCharacterSlot() )
	SetString( WIDGET_ID_CHARINFO, "text", strSchoolClass, "tabChar.tfSchoolClass" )
	
	-- 부가능력 표시
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 18 ), "tabChar.labelAddOn" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_ShowAddOn", "", "tabChar.btnAddOn" )
	
	-- 성향
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_SOCIAL_STATIC", 5 ), "tabChar.tfAtb" )
	-- 생활점수
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_SOCIAL_STATIC", 2 ), "tabChar.tfLiving" )	
	-- 기여도
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_SOCIAL_STATIC", 7 ), "tabChar.tfCsb" )
	
	-- 도움말 버튼
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Event_CharInfo_Help1", "", "tabChar.btnHelp1" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Event_CharInfo_Help2", "", "tabChar.btnHelp2" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Event_CharInfo_Help", "", "viewAddOn.btnHelp" )
	
	-- 포인트
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STAT_POINT", 0 ), "tabChar.tfPoint" )
	
	-- 힘, 민첩, 정신, 체력, 근력 툴팁
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_POW_TOOLTIP", 0 ), "tabChar.Stat1" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_DEX_TOOLTIP", 0 ), "tabChar.Stat2" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_SPI_TOOLTIP", 0 ), "tabChar.Stat3" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_STR_TOOLTIP", 0 ), "tabChar.Stat4" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_STA_TOOLTIP", 0 ), "tabChar.Stat5" )
	
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 0 ), "tabChar.Stat1.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 1 ), "tabChar.Stat2.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 2 ), "tabChar.Stat3.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 4 ), "tabChar.Stat4.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STAT_STATIC", 5 ), "tabChar.Stat5.tfName" )
	
	SetString( WIDGET_ID_CHARINFO, "label", "+", "tabChar.btnStat1Up" )
	SetString( WIDGET_ID_CHARINFO, "label", "+", "tabChar.btnStat2Up" )
	SetString( WIDGET_ID_CHARINFO, "label", "+", "tabChar.btnStat3Up" )
	SetString( WIDGET_ID_CHARINFO, "label", "+", "tabChar.btnStat4Up" )
	SetString( WIDGET_ID_CHARINFO, "label", "+", "tabChar.btnStat5Up" )
	SetString( WIDGET_ID_CHARINFO, "label", "-", "tabChar.btnStat1Down" )
	SetString( WIDGET_ID_CHARINFO, "label", "-", "tabChar.btnStat2Down" )
	SetString( WIDGET_ID_CHARINFO, "label", "-", "tabChar.btnStat3Down" )
	SetString( WIDGET_ID_CHARINFO, "label", "-", "tabChar.btnStat4Down" )
	SetString( WIDGET_ID_CHARINFO, "label", "-", "tabChar.btnStat5Down" )
	
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatUp1_Click", "", "tabChar.btnStat1Up" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatUp2_Click", "", "tabChar.btnStat2Up" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatUp3_Click", "", "tabChar.btnStat3Up" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatUp4_Click", "", "tabChar.btnStat4Up" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatUp5_Click", "", "tabChar.btnStat5Up" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatDown1_Click", "", "tabChar.btnStat1Down" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatDown2_Click", "", "tabChar.btnStat2Down" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatDown3_Click", "", "tabChar.btnStat3Down" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatDown4_Click", "", "tabChar.btnStat4Down" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_StatDown5_Click", "", "tabChar.btnStat5Down" )
	
	-- 스텟 증감버튼 리피트 시간 설정(누르고 있을때 갱신시간)
	for i = 1, 5, 1 do
		SetInt( WIDGET_ID_CHARINFO, "repeatInterval", 10, "tabChar.btnStat" .. i .. "Up" )
		SetInt( WIDGET_ID_CHARINFO, "repeatInterval", 10, "tabChar.btnStat" .. i .. "Down" )
	end
	
	-- 초기화, 적용 버튼
	SetString( WIDGET_ID_CHARINFO, "label", GetGameWordText( "CHARACTER_STAT_INIT", 0 ), "tabChar.btnInit" )
	SetString( WIDGET_ID_CHARINFO, "label", GetGameWordText( "CHARACTER_STAT_APPLY", 0 ), "tabChar.btnApply" )
	
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_Init_Click", "", "tabChar.btnInit" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_Apply_Click", "", "tabChar.btnApply" )
	
	-- 세부 능력치
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ATTACK_MIN_TOOLTIP", 0 ), "tabChar.Detail1" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ATTACK_MAX_TOOLTIP", 0 ), "tabChar.Detail2" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_DEFENSE_TOOLTIP", 0 ), "tabChar.Detail3" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_FIGHT_TOOLTIP", 0 ), "tabChar.Detail4" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_SHOOT_TOOLTIP", 0 ), "tabChar.Detail5" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_STAMINA_TOOLTIP", 0 ), "tabChar.Detail6" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_HIT_TOOLTIP", 0 ), "tabChar.Detail7" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_AVOID_TOOLTIP", 0 ), "tabChar.Detail8" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_HP_TOOLTIP", 0 ), "tabChar.Detail9" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_MP_TOOLTIP", 0 ), "tabChar.Detail10" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_SP_TOOLTIP", 0 ), "tabChar.Detail11" )
	
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 15 ), "tabChar.Detail1.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 14 ), "tabChar.Detail2.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 3 ), "tabChar.Detail3.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 4 ), "tabChar.Detail4.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 5 ), "tabChar.Detail5.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 11 ), "tabChar.Detail6.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 9 ), "tabChar.Detail7.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 10 ), "tabChar.Detail8.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "ITEM_ADDON_INFO", 5 ), "tabChar.Detail9.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "ITEM_ADDON_INFO", 6 ), "tabChar.Detail10.tfName" )
	SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "ITEM_ADDON_INFO", 7 ), "tabChar.Detail11.tfName" )
	
	-- 부가 능력치
	SetBool( WIDGET_ID_CHARINFO, "visible", false, "viewAddOn" )
	
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_ViewAddon_Page", "", "viewAddOn.radioPage1", tostring( 1 ) );
	SetEvent( WIDGET_ID_CHARINFO, EVENT_BUTTON_CLICK, "Ev_CharInfo_ViewAddon_Page", "", "viewAddOn.radioPage2", tostring( 2 ) );
	
	SetBool( WIDGET_ID_CHARINFO, "selected", true, "inventory.radioPage1" )
	Ev_CharInfo_ViewAddon_Page("1")
	
	-- 부가능력이 초기에는 꺼진 상태니 윈도우 사이즈 새로 잡아주기
	local nAddOnViewWidth = GetInt( WIDGET_ID_CHARINFO, "width", "viewAddOn" )
	SetInt( WIDGET_ID_CHARINFO, "customWidth", g_nCharInfo_OrigWndWidth - nAddOnViewWidth, "" )
	
	CharInfo_UpdateChnageUpAbillity( true )

end

-- 캐릭터 정보창 오픈
function OpenCharInfo()

	-- 레벨, 이름
	local strLevelName = "Lv. " .. tostring( g_MyChar:GetLevel() ) .. " " .. g_MyChar:GetName()
	SetString( WIDGET_ID_CHARINFO, "text", strLevelName, "tabChar.labelLevelName" )
	CallFunction( WIDGET_ID_CHARINFO, "validateNow", "", "tabChar.labelLevelName" )
	
	local nTextW = GetInt( WIDGET_ID_CHARINFO, "textWidth", "tabChar.labelLevelName.textField" ) + 4
	-- 이름 길이가 충분하면 가운데 정렬
	if nTextW < g_nCharInfo_NameLabelWidth then
	
		local nCenter = math.floor( (g_nCharInfo_NameLabelWidth - nTextW) / 2 )
		SetInt( WIDGET_ID_CHARINFO, "x", nCenter, "tabChar.labelLevelName" )
	
	-- 너무 길면 원래 위치로 초기화하고 툴팁으로 표기한다.
	else
	
		SetInt( WIDGET_ID_CHARINFO, "x", g_nCharInfo_NameLabelX, "tabChar.labelLevelName" )
	
	end
	
	-- 클럽 이름
	local strClubName = g_MyChar:GetClubName()
	if 0 == string.len( strClubName ) then
		strClubName = GetGameInternalText( "CHARACTER_WINDOW_EMPTY_CLUB", 0 )
	else
		strClubName = "[" .. strClubName .. "]"
	end
	SetString( WIDGET_ID_CHARINFO, "htmlText", strClubName, "tabChar.labelClub" )
	SetTextFormat( WIDGET_ID_CHARINFO, TEXT_COLOR_YELLOW, false, 0, 0, "tabChar.labelClub" )
	CallFunction( WIDGET_ID_CHARINFO, "validateNow", "", "tabChar.labelClub" )
	
	nTextW = GetInt( WIDGET_ID_CHARINFO, "textWidth", "tabChar.labelClub.textField" ) + 4
	-- 이름 길이가 충분하면 가운데 정렬
	if nTextW < g_nCharInfo_ClubLabelWidth then
	
		local nCenter = math.floor( (g_nCharInfo_ClubLabelWidth - nTextW) / 2 )
		SetInt( WIDGET_ID_CHARINFO, "x", nCenter, "tabChar.labelClub" )
	
	-- 너무 길면 원래 위치로 초기화하고 툴팁으로 표기한다.
	else
	
		SetInt( WIDGET_ID_CHARINFO, "x", g_nCharInfo_ClubLabelX, "tabChar.labelClub" )
	
	end
	
	-- 성향
	SetString( WIDGET_ID_CHARINFO, "text", g_MyChar.nAtb, "tabChar.tfAtbPoint" )

	-- 뱃지 타이틀
	local nTitleDataCount = 1
	local tbTitleData = {}
	local tbTitle = g_MyChar:GetTitle()
	for value in pairs( tbTitle ) do
	
		local tbObj = tbTitle[ value ]
		
		local strLabel = ""
		if tbObj[ 1 ] == "CAHRWINDOW_TITLE" then
			strLabel = HTMLFontColor( GetGameWordText( tbObj[ 1 ] ), TranslateHTMLColor( tbObj[ 2 ] ) )
		else
			strLabel = HTMLFontColor( GetGameInternalText( tbObj[ 1 ] ), TranslateHTMLColor( tbObj[ 2 ] ) )
		end
	
		local tbData = {}	
		tbData[ 1 ] = strLabel
		tbData[ 2 ] = tbObj[ 3 ]
		
		tbTitleData[ nTitleDataCount ] = tbData
		nTitleDataCount = nTitleDataCount + 1
	
	end
	SetDataEx( WIDGET_ID_CHARINFO, "label;titleIndex;", tbTitleData, "tabChar.comboBadge", false )	
	SetInt( WIDGET_ID_CHARINFO, "selectedIndex", g_MyChar:GetTitleSelectIndex(), "tabChar.comboBadge" )
	
	-- 극강부 슬롯?
	if  CHARCLASS_IDX_EXTREAM_M == g_MyChar.nClassIndex or
		CHARCLASS_IDX_EXTREAM_W ==  g_MyChar.nClassIndex then
		
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "EXTREAM", "tabChar.weaponSlot" )

		if true == g_MyChar:IsUseArmSub() then

			SetBool( WIDGET_ID_CHARINFO, "selected", false, "tabChar.weaponSlot.radioLeft" )
			SetBool( WIDGET_ID_CHARINFO, "selected", true, "tabChar.weaponSlot.radioRight" )
			HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioLeft" )
			HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioRight" )
			
		else

			SetBool( WIDGET_ID_CHARINFO, "selected", true, "tabChar.weaponSlot.radioLeft" )
			SetBool( WIDGET_ID_CHARINFO, "selected", false, "tabChar.weaponSlot.radioRight" )
			HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioLeft" )
			HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioRight" )
			
		end
		
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "ETC", "tabChar.weaponSlot" )
		
	end
	
	-- 극강부 무기타입 변경
	SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_CLICK, "Ev_CharInfo_WeaponSlotA_Click", "", "tabChar.weaponSlot.slot15" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_CLICK, "Ev_CharInfo_WeaponSlotA_Click", "", "tabChar.weaponSlot.slot16" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_CLICK, "Ev_CharInfo_WeaponSlotB_Click", "", "tabChar.weaponSlot.slot17" )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_CLICK, "Ev_CharInfo_WeaponSlotB_Click", "", "tabChar.weaponSlot.slot18" )
	
	-- 성향
	SetDouble( WIDGET_ID_CHARINFO, "text", g_MyChar.nAtb, "tabChar.tfAtbPoint" )
	-- 생활 점수
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar.nLivingPoint ), "tabChar.tfLivingPoint" )
	-- 기여도
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar.nCsb ), "tabChar.tfCsbScore" )
	-- 포인트
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar.nStatPoint ), "tabChar.tfPointVal" )
	
	CharInfo_UpdateChnageUpAbillity( false )
	
	-- 캐릭터 슬롯
	CharInfo_UpdateCharacterSlotInfo()
	
	-- 인벤토리에 캐릭터창 열림 알리기
	Inventory_SetOpenCharInfo( true )
	
	-- 국가 이미지 설정
	CharInfo_SetCountryImg( g_Client:GetSelectedCountryID() )
	
	CharInfo_UpdateSlot()
	CharInfo_UpdateStatBtn()
	
end

-- 닫기
function CloseCharInfo()

	-- 인벤토리에 캐릭터창 닫힘 알리기
	Inventory_SetOpenCharInfo( false )
	
	CharInfo_TurnOffHighlight()

end

-- 초기화
function CharInfo_Reset()

	g_nCharInfo_NameLabelX = 0
	g_nCharInfo_NameLabelWidth = 0
	g_nCharInfo_ClubLabelX = 0
	g_nCharInfo_ClubLabelWidth = 0
	
	g_CharInfo_VirtualStat1 = 0
	g_CharInfo_VirtualStat2 = 0
	g_CharInfo_VirtualStat3 = 0
	g_CharInfo_VirtualStat4 = 0
	g_CharInfo_VirtualStat5 = 0

end

-- 극강부 무기타입 A 란 선택
function Ev_CharInfo_WeaponSlotA_Click()

	if false == g_MyChar:IsUseArmSub() then
		return
	end

	ReleaseSlot()
	CharInfo_ArmSwap( false )
	g_Msg:ReqSlotChange()
	
end

function Ev_CharInfo_WeaponSlotB_Click()

	if true == g_MyChar:IsUseArmSub() then
		return
	end
	
	ReleaseSlot()
	CharInfo_ArmSwap( true )
	g_Msg:ReqSlotChange()
	
end

function ChangeCharacterSlot_EventProc ( _nSlotIndex )

	-- 현재 슬롯과 같은 슬롯에는 이벤트를 발생시키지 않는다;
	if ( _nSlotIndex == g_MyChar:GetCurrentCharacterSlot() ) then
	
		g_STRING_PROGRESS_BAR.Reset()
		return
		
	end
	
	-- 캐릭터 슬롯이 변경 가능한지 확인한다;
	if false == g_MyChar:IsChangeCharSlot( _nSlotIndex ) then
		
		g_STRING_PROGRESS_BAR.Reset()
		return
		
	end

	-- String ProgressBar 를 이용하여 딜레이를 주고 캐릭터 슬롯을 변경한다;
	g_STRING_PROGRESS_BAR.Initialize( CHANGE_CHARACTER_SLOT_WAITTIME, GetGameInternalText( "CHANGE_CHARSLOT_INFO_TEXT", 0 ), STRING_PROGRESSBAR_TYPE_CHANGE_CHARSLOT )
	g_STRING_PROGRESS_BAR.SetTrigger_Function( function ( _nSlotIndex ) g_MyChar:ReqChangeCharSlot( _nSlotIndex ) end )
	g_STRING_PROGRESS_BAR.SetTrigger_InsertValue( _nSlotIndex )
	g_STRING_PROGRESS_BAR.Open()
	
end

function Ev_CharInfo_CharacterSlot_Click0 ( )

	ChangeCharacterSlot_EventProc( 0 )
	
end

function Ev_CharInfo_CharacterSlot_Click1 ( )

	ChangeCharacterSlot_EventProc( 1 )
	
end

function Ev_CharInfo_CharacterSlot_Click2 ( )

	ChangeCharacterSlot_EventProc( 2 )
	
end

function Ev_CharInfo_CharacterSlot_Click3 ( )

	ChangeCharacterSlot_EventProc( 3 )
	
end

function Ev_CharInfo_CharacterSlot_Click4 ( )

	ChangeCharacterSlot_EventProc( 4 )
	
end

function CharInfo_SetEvent_OverlayTooltip( strInstance, strOverEvent, strOutEvent )

	SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_ROLLOVER, strOverEvent, "", strInstance )
	SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_ROLLOUT, strOutEvent, "", strInstance )

end

function CharInfo_IsEnableChangeUpAbillity()

	local nSumVirtualPoint = g_CharInfo_VirtualStat1 + g_CharInfo_VirtualStat2 + g_CharInfo_VirtualStat3 +
		g_CharInfo_VirtualStat4 + g_CharInfo_VirtualStat5
	
	if g_MyChar.nStatPoint <= nSumVirtualPoint then
		return false
	end
	
	return true

end

-- 캐릭터 정보 증감 비교란 출력
function CharInfo_SetChangeUpAbillity( nChange, strInstance )

	local strChange = ""
	local strIconAnim = ""
	
	if 0 == nChange then
	
		strChange = HTMLFontColor( tostring( nChange ), HTML_TEXT_COLOR_WHITE )
		strIconAnim = "NONE"
		
	elseif 0 < nChange then
	
		strChange = HTMLFontColor( tostring( nChange ), HTML_TEXT_COLOR_GREEN )
		strIconAnim = "UP"
		
	else
	
		strChange = HTMLFontColor( tostring( nChange ), HTML_TEXT_COLOR_RED )
		strIconAnim = "DOWN"
		
	end
	
	local strChangeInst = strInstance .. ".tfChange"
	local strIconInst = strInstance .. ".mcIcon"
	
	SetString( WIDGET_ID_CHARINFO, "htmlText", strChange, strChangeInst )
	CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", strIconAnim, strIconInst )

end

-- 캐릭터 정보 부가 능력치 증감 비교란 출력
function CharInfo_SetChangeUpAddOn( fChange, strInstance, bIsInteger )
	
	local strChange = string.format( "%.2f%%", fChange )
	if bIsInteger == true then
		strChange = fChange
	end
	
	local strIconAnim = ""
	
	if 0 == fChange then
	
		strChange = HTMLFontColor( strChange, HTML_TEXT_COLOR_WHITE )
		strIconAnim = "NONE"
		
	elseif 0 < fChange then
	
		strChange = HTMLFontColor( strChange, HTML_TEXT_COLOR_GREEN )
		strIconAnim = "UP"
		
	else	
		
		strChange = HTMLFontColor( strChange, HTML_TEXT_COLOR_RED )
		strIconAnim = "DOWN"
		
	end
	
	local strChangeInst = strInstance .. ".tfCompare"
	local strIconInst = strInstance .. ".mcIcon"
	
	SetString( WIDGET_ID_CHARINFO, "htmlText", strChange, strChangeInst )
	CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", strIconAnim, strIconInst )

end

-- 캐릭터 슬롯 관련 갱신
function CharInfo_UpdateCharacterSlotInfo ()

	local GetSlotString = function ( _nSlotIndex )
	
		if 0 == _nSlotIndex then
		
			return "a"
		
		elseif 1 == _nSlotIndex then
		
			return "b"
		
		elseif 2 == _nSlotIndex then
		
			return "c"
		
		elseif 3 == _nSlotIndex then
		
			return "d"
		
		elseif 4 == _nSlotIndex then
		
			return "e"
			
		end
		
	end
	
	-- 슬롯 툴팁을 세팅한다
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_CHANGE_SLOT_A_TOOLTIP", 0 ), "tabChar.skill_type_a" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_CHANGE_SLOT_B_TOOLTIP", 0 ), "tabChar.skill_type_b" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_CHANGE_SLOT_C_TOOLTIP", 0 ), "tabChar.skill_type_c" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_CHANGE_SLOT_D_TOOLTIP", 0 ), "tabChar.skill_type_d" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_CHANGE_SLOT_E_TOOLTIP", 0 ), "tabChar.skill_type_e" )
	
	-- 현재 사용중인 슬롯의 툴팁을 세팅한다
	local nCurrentCharSlot = g_MyChar:GetCurrentCharacterSlot()
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_CURRENT_SLOT_TOOLTIP", 0 ), "tabChar.skill_type_"..GetSlotString(nCurrentCharSlot) )

	-- 잠겨있는 슬롯을 설정한다
	for i = 0, MAX_CHARACTER_SLOT_COUNT-1, 1 do
		
		if g_Client:GetOpenCharacterSlotCount() < i then
		
			SetBool( WIDGET_ID_CHARINFO, "visible", false, "tabChar.skill_type_"..GetSlotString(i) )
			
		elseif true == g_MyChar:GetCharacterSlotLockState( i ) then
		
			SetBool( WIDGET_ID_CHARINFO, "visible", true, "tabChar.skill_type_"..GetSlotString(i) )
			SetBool( WIDGET_ID_CHARINFO, "enabled", false, "tabChar.skill_type_"..GetSlotString(i) )
			
		else
		
			SetBool( WIDGET_ID_CHARINFO, "visible", true, "tabChar.skill_type_"..GetSlotString(i) )
			SetBool( WIDGET_ID_CHARINFO, "enabled", true, "tabChar.skill_type_"..GetSlotString(i) )
			
		end
		
	end
	
	-- 현재 슬롯정보를 세팅한다
	SetBool( WIDGET_ID_CHARINFO, "selected", true, "tabChar.skill_type_"..GetSlotString(nCurrentCharSlot) )
	
	-- 이벤트를 설정한다
	for i = 0, MAX_CHARACTER_SLOT_COUNT-1, 1 do
		
		SetEvent( WIDGET_ID_CHARINFO, EVENT_MOUSE_CLICK, "Ev_CharInfo_CharacterSlot_Click"..tostring(i), "", "tabChar.skill_type_"..GetSlotString(i) )
		
	end
	
	-- 학원, 클래스, 캐릭터슬롯 정보를 다시 세팅한다
	local strSchoolClass = GetGameWordText( "FULL_ACADEMY_NAME", g_MyChar:GetSchool() ) .. ", " .. 
		GetCharClassIndexName( g_MyChar.nClassIndex ) .. ", " .. GetGameWordText( "CHARACTER_SLOT_NAME", g_MyChar:GetCurrentCharacterSlot() )
	SetString( WIDGET_ID_CHARINFO, "text", strSchoolClass, "tabChar.tfSchoolClass" )
	
end

function CharInfo_UpdateStatInfo( nCompare1, nCompare2, nCompare3, nCompare4, nCompare5 )

	local nStat1 = 0
	if nil == nCompare1 or 0 == nCompare1 then
		nStat1 = g_CharInfo_VirtualStat1
	else
		nStat1 = nCompare1
	end
	
	local nStat2 = 0
	if nil == nCompare2 or 0 == nCompare2 then
		nStat2 = g_CharInfo_VirtualStat2
	else
		nStat2 = nCompare2
	end
	
	local nStat3 = 0
	if nil == nCompare3 or 0 == nCompare3 then
		nStat3 = g_CharInfo_VirtualStat3
	else
		nStat3 = nCompare3
	end
	
	local nStat4 = 0
	if nil == nCompare4 or 0 == nCompare4 then
		nStat4 = g_CharInfo_VirtualStat4
	else
		nStat4 = nCompare4
	end
	
	local nStat5 = 0
	if nil == nCompare5 or 0 == nCompare5 then
		nStat5 = g_CharInfo_VirtualStat5
	else
		nStat5 = nCompare5
	end
	
	local strStat1temp = ""
	local strStat2temp = ""
	local strStat3temp = ""
	local strStat4temp = ""
	local strStat5temp = ""
	
	if 0 < nStat1 then
	
		strStat1temp = HTMLFontColor( nStat1, HTML_TEXT_COLOR_GREEN )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "tabChar.StatCompare1.mcIcon" )
		
	elseif 0 > nStat1 then
	
		strStat1temp = HTMLFontColor( nStat1, HTML_TEXT_COLOR_RED )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "tabChar.StatCompare1.mcIcon" )
		
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "tabChar.StatCompare1.mcIcon" )
		
	end
	
	if 0 < nStat2 then
	
		strStat2temp = HTMLFontColor( nStat2, HTML_TEXT_COLOR_GREEN )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "tabChar.StatCompare2.mcIcon" )
		
	elseif 0 > nStat2 then
	
		strStat2temp = HTMLFontColor( nStat2, HTML_TEXT_COLOR_RED )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "tabChar.StatCompare2.mcIcon" )
		
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "tabChar.StatCompare2.mcIcon" )
		
	end
	
	if 0 < nStat3 then
	
		strStat3temp = HTMLFontColor( nStat3, HTML_TEXT_COLOR_GREEN )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "tabChar.StatCompare3.mcIcon" )
		
	elseif 0 > nStat3 then
	
		strStat3temp = HTMLFontColor( nStat3, HTML_TEXT_COLOR_RED )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "tabChar.StatCompare3.mcIcon" )
		
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "tabChar.StatCompare3.mcIcon" )
		
	end
	
	if 0 < nStat4 then
	
		strStat4temp = HTMLFontColor( nStat4, HTML_TEXT_COLOR_GREEN )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "tabChar.StatCompare4.mcIcon" )
		
	elseif 0 > nStat4 then
	
		strStat4temp = HTMLFontColor( nStat4, HTML_TEXT_COLOR_RED )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "tabChar.StatCompare4.mcIcon" )
		
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "tabChar.StatCompare4.mcIcon" )
		
	end
	
	if 0 < nStat5 then
	
		strStat5temp = HTMLFontColor( nStat5, HTML_TEXT_COLOR_GREEN )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "tabChar.StatCompare5.mcIcon" )
		
	elseif 0 > nStat5 then
	
		strStat5temp = HTMLFontColor( nStat5, HTML_TEXT_COLOR_RED )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "tabChar.StatCompare5.mcIcon" )
		
	else
	
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "tabChar.StatCompare5.mcIcon" )
		
	end
	
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetPow_Skill() ), "tabChar.Stat1.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetDex_Skill() ), "tabChar.Stat2.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetSpi_Skill() ), "tabChar.Stat3.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetStr_Skill() ), "tabChar.Stat4.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetSta_Skill() ), "tabChar.Stat5.tfValue" )
	
	SetString( WIDGET_ID_CHARINFO, "htmlText", strStat1temp, "tabChar.StatCompare1.tfCompare" )
	SetString( WIDGET_ID_CHARINFO, "htmlText", strStat2temp, "tabChar.StatCompare2.tfCompare" )
	SetString( WIDGET_ID_CHARINFO, "htmlText", strStat3temp, "tabChar.StatCompare3.tfCompare" )
	SetString( WIDGET_ID_CHARINFO, "htmlText", strStat4temp, "tabChar.StatCompare4.tfCompare" )
	SetString( WIDGET_ID_CHARINFO, "htmlText", strStat5temp, "tabChar.StatCompare5.tfCompare" )

	local strStatPoint = tostring( g_MyChar.nStatPoint - g_CharInfo_VirtualStat1 - g_CharInfo_VirtualStat2 - 
			g_CharInfo_VirtualStat3 - g_CharInfo_VirtualStat4 - g_CharInfo_VirtualStat5 )
	SetString( WIDGET_ID_CHARINFO, "text", strStatPoint, "tabChar.tfPointVal" )

end

function CharInfo_UpdateStatBtn()

	local bVisible = false
	if 0 < g_MyChar.nStatPoint then
		bVisible = true
	end
	
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat1Up" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat2Up" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat3Up" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat4Up" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat5Up" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat1Down" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat2Down" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat3Down" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat4Down" )
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.btnStat5Down" )
	
	local bEnable = bVisible
	SetBool( WIDGET_ID_CHARINFO, "enabled", bEnable, "tabChar.btnInit" )
	SetBool( WIDGET_ID_CHARINFO, "enabled", bEnable, "tabChar.btnApply" )
	
end

function CharInfo_UpdateDetail()

	-- 최소 공력력
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetForce_Low() ), "tabChar.Detail1.tfValue" )
	-- 최대 공력력
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetForce_High() ), "tabChar.Detail2.tfValue" )
	-- 방어력
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetDefense() ), "tabChar.Detail3.tfValue" )
	-- 격투치
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetSumPowerAttribute( 0 ) ), "tabChar.Detail4.tfValue" )
	-- 사격치
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetSumPowerAttribute( 1 ) ), "tabChar.Detail5.tfValue" )
	-- 기력치
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetSumPowerAttribute( 2 ) ), "tabChar.Detail6.tfValue" )
	-- 명중률
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetHit() ), "tabChar.Detail7.tfValue" )
	-- 회피율
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetAvoid() ), "tabChar.Detail8.tfValue" )
	-- HP
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetMaxHP() ), "tabChar.Detail9.tfValue" )
	-- MP
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetMaxMP() ), "tabChar.Detail10.tfValue" )
	-- SP
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetMaxSP() ), "tabChar.Detail11.tfValue" )
	
	-- 가산효과 페이지별 세팅 필요
	if true == GetBool( WIDGET_ID_CHARINFO, "selected", "viewAddOn.radioPage1" ) then
		Ev_CharInfo_ViewAddon_Page("1")
	else
		Ev_CharInfo_ViewAddon_Page("2")
	end
	
end

function CharInfo_UpdateResist( nCompare1, nCompare2, nCompare3, nCompare4, nCompare5 )

	-- 가산효과 페이지별 세팅 필요
	if false == GetBool( WIDGET_ID_CHARINFO, "selected", "viewAddOn.radioPage1" ) then
		return
	end
	
	if nil == nCompare1 or 0 == nCompare1 then
	
		SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn13.tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn13.mcIcon" )
		
	else
	
		local str = ""
		if 0 < nCompare1 then
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_GREEN )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "viewAddOn.addOn13.mcIcon" )
			
		else
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_RED )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "viewAddOn.addOn13.mcIcon" )
			
		end
		
		SetString( WIDGET_ID_CHARINFO, "htmlText", str, "viewAddOn.addOn13.tfCompare" )
	
	end
	
	if nil == nCompare2 or 0 == nCompare2 then
	
		SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn14.tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn14.mcIcon" )
		
	else
	
		local str = ""
		if 0 < nCompare2 then
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_GREEN )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "viewAddOn.addOn14.mcIcon" )
			
		else
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_RED )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "viewAddOn.addOn14.mcIcon" )
			
		end
		
		SetString( WIDGET_ID_CHARINFO, "htmlText", str, "viewAddOn.addOn14.tfCompare" )
	
	end
	
	if nil == nCompare3 or 0 == nCompare3 then
	
		SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn15.tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn15.mcIcon" )
		
	else
	
		local str = ""
		if 0 < nCompare3 then
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_GREEN )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "viewAddOn.addOn15.mcIcon" )
			
		else
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_RED )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "viewAddOn.addOn15.mcIcon" )
			
		end
		
		SetString( WIDGET_ID_CHARINFO, "htmlText", str, "viewAddOn.addOn15.tfCompare" )
	
	end
	
	if nil == nCompare4 or 0 == nCompare4 then
	
		SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn16.tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn16.mcIcon" )
		
	else
	
		local str = ""
		if 0 < nCompare4 then
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_GREEN )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "viewAddOn.addOn16.mcIcon" )
			
		else
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_RED )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "viewAddOn.addOn16.mcIcon" )
			
		end
		
		SetString( WIDGET_ID_CHARINFO, "htmlText", str, "viewAddOn.addOn16.tfCompare" )
	
	end
	
	if nil == nCompare5 or 0 == nCompare5 then
	
		SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn17.tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn17.mcIcon" )
		
	else
	
		local str = ""
		if 0 < nCompare5 then
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_GREEN )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "UP", "viewAddOn.addOn17.mcIcon" )
			
		else
		
			str = HTMLFontColor( nCompare1, HTML_TEXT_COLOR_RED )
			CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "DOWN", "viewAddOn.addOn17.mcIcon" )
			
		end
		
		SetString( WIDGET_ID_CHARINFO, "htmlText", str, "viewAddOn.addOn17.tfCompare" )
		
	end

	-- 화염
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetResistFire() ), "viewAddOn.addOn13.tfValue" )
	-- 냉기
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetResistIce() ), "viewAddOn.addOn14.tfValue" )
	-- 전기
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetResistElectric() ), "viewAddOn.addOn15.tfValue" )
	-- 맹독
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetResistPoison() ), "viewAddOn.addOn16.tfValue" )
	-- 정기
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar:GetResistSpirit() ), "viewAddOn.addOn17.tfValue" )

end
-- 캐릭터 정보 증감 비교란 셋팅용
function CharInfo_UpdateChnageUpAbillity( bInit )

	local bVirtualStat = true
	if 	0 == g_CharInfo_VirtualStat1 and
		0 == g_CharInfo_VirtualStat2 and
		0 == g_CharInfo_VirtualStat3 and
		0 == g_CharInfo_VirtualStat4 and
		0 == g_CharInfo_VirtualStat5 then
		
		bVirtualStat = false
		
	end

	if false == bInit and true == bVirtualStat then
	
		local tbStat = g_MyChar:GetChangeUpAbillity( g_CharInfo_VirtualStat1, g_CharInfo_VirtualStat2,
			g_CharInfo_VirtualStat3, g_CharInfo_VirtualStat4, g_CharInfo_VirtualStat5 )
		
		if 11 ~= table.getn( tbStat ) then
			return
		end
		
		-- 스텟 분배를 통해 올라가는 능력치와 착용중인 능력치를 비교하여 가감표시
		for i = 1, table.getn( tbStat ), 1 do
			CharInfo_SetChangeUpAbillity( tbStat[ i ], "tabChar.Detail" .. tostring( i ) )
		end
	
	else
	
		for i = 1, 11, 1 do
			CharInfo_SetChangeUpAbillity( 0, "tabChar.Detail" .. tostring( i ) )
		end
	
	end
		
	local nStatPoint = g_MyChar.nStatPoint - ( g_CharInfo_VirtualStat1 + g_CharInfo_VirtualStat2 + g_CharInfo_VirtualStat3 +
		g_CharInfo_VirtualStat4 + g_CharInfo_VirtualStat5 )
		
	SetString( WIDGET_ID_CHARINFO, "text", tostring( nStatPoint ), "tabChar.tfPointVal" )

end

-- 국가 이미지 설정
function CharInfo_SetCountryImg( nCountryID )

	local bVisible = g_UserFeatures:IsUsingSelectCountry()
	local strFrameID = Display_GetCountryFrameID( nCountryID )
	
	SetBool( WIDGET_ID_CHARINFO, "visible", bVisible, "tabChar.mcCountryImg" )
	
	if true == bVisible then
	
		-- 프레임 이동
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndPlay", strFrameID, "tabChar.mcCountryImg" )
		
		-- 툴팁 설정
		local str = g_Client:GetSelectedCountryName()
		SetString( WIDGET_ID_CHARINFO, "TooltipText", str, "tabChar.mcCountryImg" )
	
	end

end

-- 장착 슬롯 위치 표시
function CharInfo_DisplayEquipSlot( nInvenPosX, nInvenPosY, bShow )

	if false == IsOpenWidget( WIDGET_ID_CHARINFO ) then
		return
	end
	
	if g_MyChar:WearableCheck(nInvenPosX, nInvenPosY) == false then
		return
	end
	
	local tbHighlightSlot = {}
	local nSuitType = g_MyChar:GetInvenItemSuitType( nInvenPosX, nInvenPosY )
	
	-- SUIT_HEADGEAR
	if 0 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_HEADGEAR )
	
	-- SUIT_UPPER
	elseif 1 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_UPPER )
	
	-- SUIT_LOWER
	elseif 2 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_LOWER )
	
	-- SUIT_HAND
	elseif 3 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_HAND )
	
	-- SUIT_FOOT
	elseif 4 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_FOOT )
	
	-- SUIT_HANDHELD
	elseif 5 == nSuitType then
	
		local nType = g_MyChar:GetInvenItemType( nInvenPosX, nInvenPosY )
		if 	ITEMTYPE_ARROW == nType or
			ITEMTYPE_BULLET == nType or
			ITEMTYPE_CHARM == nType or
			ITEMTYPE_CHANGE_ACTORS_DISGUISE_INFO == nType then
			
			table.insert( tbHighlightSlot, CHARINFO_SLOT_LHAND )
			
			-- 극강부
			if  CHARCLASS_IDX_EXTREAM_M == g_MyChar.nClassIndex or
				CHARCLASS_IDX_EXTREAM_W ==  g_MyChar.nClassIndex then
				
				table.insert( tbHighlightSlot, CHARINFO_SLOT_LHAND_S )
				
			end
			
		else
		
			table.insert( tbHighlightSlot, CHARINFO_SLOT_RHAND )
			
			-- 극강부
			if  CHARCLASS_IDX_EXTREAM_M == g_MyChar.nClassIndex or
				CHARCLASS_IDX_EXTREAM_W ==  g_MyChar.nClassIndex then
				
				table.insert( tbHighlightSlot, CHARINFO_SLOT_RHAND_S )
				
			end
		
		end

	-- SUIT_NECK
	elseif 6 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_NECK )
	
	-- SUIT_WRIST
	elseif 7 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_WRIST )
	
	-- SUIT_FINGER
	elseif 8 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_RFINGER )
		table.insert( tbHighlightSlot, CHARINFO_SLOT_LFINGER )
	
	-- SUIT_VEHICLE
	elseif 11 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_VEHICLE )
	
	-- SUIT_WAISTBAND
	elseif 19 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_WAISTBAND )
	
	-- SUIT_EARRING
	elseif 20 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_EARRINGS )
	
	-- SUIT_ACCESSORIES
	elseif 21 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_RACCESSORIES )
		table.insert( tbHighlightSlot, CHARINFO_SLOT_LACCESSORIES )
	
	-- SUIT_DECORATION
	elseif 22 == nSuitType then
	
		table.insert( tbHighlightSlot, CHARINFO_SLOT_DECORATION )
	
	else
	
		return
		
	end
	
	for value in pairs( tbHighlightSlot ) do
	
		local nSlot = tbHighlightSlot[ value ]
		local strInst = CharSlotToID( "slot", nSlot )
		
		if 	CHARINFO_SLOT_RHAND == nSlot or
			CHARINFO_SLOT_LHAND == nSlot or
			CHARINFO_SLOT_RHAND_S == nSlot or
			CHARINFO_SLOT_LHAND_S == nSlot then
			
			strInst = "tabChar.weaponSlot." .. strInst .. ".mcHighlight"
			
		else
		
			strInst = "tabChar." .. strInst .. ".mcHighlight"
		
		end
		
		SetBool( WIDGET_ID_CHARINFO, "visible", bShow, strInst )
	
	end

end

-- 슬롯 하이라이트 끄기
function CharInfo_TurnOffHighlight()

	for i = CHARINFO_SLOT_HEADGEAR, CHARINFO_SLOT_LACCESSORIES, 1 do
	
		local strInst = CharSlotToID( "slot", i )
		if 	CHARINFO_SLOT_RHAND == i or
			CHARINFO_SLOT_LHAND == i or
			CHARINFO_SLOT_RHAND_S == i or
			CHARINFO_SLOT_LHAND_S == i then
			
			strInst = "tabChar.weaponSlot." .. strInst .. ".mcHighlight"
			
		else
		
			strInst = "tabChar." .. strInst .. ".mcHighlight"
		
		end
		
		
		SetBool( WIDGET_ID_CHARINFO, "visible", false, strInst )
	
	end

end

--------------------------------------------------------------------------------------------------------------

function Ev_CharInfo_ShowAddOn()

	local bSelect = GetBool( WIDGET_ID_CHARINFO, "selected", "tabChar.btnAddOn" )
	
	SetBool( WIDGET_ID_CHARINFO, "visible", bSelect, "viewAddOn" )
	
	-- 윈도우 사이즈 새로 잡아주기
	if true == bSelect then
		
		SetInt( WIDGET_ID_CHARINFO, "customWidth", g_nCharInfo_OrigWndWidth, "" )
		
		local nX = GetInt( WIDGET_ID_CHARINFO, "x", "" )
		local nScreeenWidth = GetScreenWidth()
		
		if nScreeenWidth < nX + g_nCharInfo_OrigWndWidth then
		
			SetInt( WIDGET_ID_CHARINFO, "x", nScreeenWidth - g_nCharInfo_OrigWndWidth, "" )
		
		end
	
	else
	
		local nAddOnViewWidth = GetInt( WIDGET_ID_CHARINFO, "width", "viewAddOn" )
		SetInt( WIDGET_ID_CHARINFO, "customWidth", g_nCharInfo_OrigWndWidth - nAddOnViewWidth, "" )
	
	end

end
-- 성향, 기여도 등 도움말
function Event_CharInfo_Help1()
	
	local str = GetGameWordText( "CHARACTER_SOCIAL_STATIC", 5 ) .. "\n" .. 
		GetGameInternalText( "CHARACTER_WINDOW_ACADEMY_QUESTION_SENTENCE_1", 0 ) .. "\n\n" .. 
		GetGameWordText( "CHARACTER_SOCIAL_STATIC", 2 ) .. "\n" .. 
		GetGameInternalText( "CHARACTER_WINDOW_ACADEMY_QUESTION_SENTENCE_2", 0 )
	
	HelpTooltip_SetText( str )

end
-- 스텟 포인트 관련 도움말
function Event_CharInfo_Help2()

	local str = GetGameInternalText( "CHARACTER_WINDOW_POINT_QUESTION_BUTTON", 0 )

	HelpTooltip_SetText( str )

end
-- 부가정보창용 도움말
function Event_CharInfo_Help()

	local str = GetGameInternalText( "CHARACTER_ADDITION_WINDOW_POINT_QUESTION_BUTTON" )

	HelpTooltip_SetText( str )

end
-- 힘 +버튼 클릭
function Ev_CharInfo_StatUp1_Click()
	
	if false == CharInfo_IsEnableChangeUpAbillity() then
		return
	end
	
	g_CharInfo_VirtualStat1 = g_CharInfo_VirtualStat1 + 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )
	
end
-- 힘 -버튼 클릭
function Ev_CharInfo_StatDown1_Click()

	if 0 >= g_CharInfo_VirtualStat1 then
		return
	end
	
	g_CharInfo_VirtualStat1 = g_CharInfo_VirtualStat1 - 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 민첩 +버튼 클릭
function Ev_CharInfo_StatUp2_Click()

	if false == CharInfo_IsEnableChangeUpAbillity() then
		return
	end
	
	g_CharInfo_VirtualStat2 = g_CharInfo_VirtualStat2 + 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 민첩 -버튼 클릭
function Ev_CharInfo_StatDown2_Click()

	if 0 >= g_CharInfo_VirtualStat2 then
		return
	end
	
	g_CharInfo_VirtualStat2 = g_CharInfo_VirtualStat2 - 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 정신 +버튼 클릭
function Ev_CharInfo_StatUp3_Click()

	if false == CharInfo_IsEnableChangeUpAbillity() then
		return
	end
	
	g_CharInfo_VirtualStat3 = g_CharInfo_VirtualStat3 + 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 정신 -버튼 클릭
function Ev_CharInfo_StatDown3_Click()

	if 0 >= g_CharInfo_VirtualStat3 then
		return
	end
	
	g_CharInfo_VirtualStat3 = g_CharInfo_VirtualStat3 - 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 체력 +버튼 클릭
function Ev_CharInfo_StatUp4_Click()

	if false == CharInfo_IsEnableChangeUpAbillity() then
		return
	end
	
	g_CharInfo_VirtualStat4 = g_CharInfo_VirtualStat4 + 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 체력 -버튼 클릭
function Ev_CharInfo_StatDown4_Click()

	if 0 >= g_CharInfo_VirtualStat4 then
		return
	end
	
	g_CharInfo_VirtualStat4 = g_CharInfo_VirtualStat4 - 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 근력 +버튼 클릭
function Ev_CharInfo_StatUp5_Click()

	if false == CharInfo_IsEnableChangeUpAbillity() then
		return
	end
	
	g_CharInfo_VirtualStat5 = g_CharInfo_VirtualStat5 + 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end
-- 근력 -버튼 클릭
function Ev_CharInfo_StatDown5_Click()

	if 0 >= g_CharInfo_VirtualStat5 then
		return
	end
	
	g_CharInfo_VirtualStat5 = g_CharInfo_VirtualStat5 - 1
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end


function Ev_CharInfo_Init_Click()
	
	g_CharInfo_VirtualStat1 = 0
	g_CharInfo_VirtualStat2 = 0
	g_CharInfo_VirtualStat3 = 0
	g_CharInfo_VirtualStat4 = 0
	g_CharInfo_VirtualStat5 = 0
	
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )
	
end

function Ev_CharInfo_Apply_Click()

	g_Msg:ApplyStatPoint( g_CharInfo_VirtualStat1, g_CharInfo_VirtualStat2, g_CharInfo_VirtualStat3,
		g_CharInfo_VirtualStat4, g_CharInfo_VirtualStat5 )
		
	g_CharInfo_VirtualStat1 = 0
	g_CharInfo_VirtualStat2 = 0
	g_CharInfo_VirtualStat3 = 0
	g_CharInfo_VirtualStat4 = 0
	g_CharInfo_VirtualStat5 = 0
	
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateChnageUpAbillity( false )

end

function Ev_CharInfo_Title_Index( nIndex )

	local nTitleIndex = GetDataInt( WIDGET_ID_CHARINFO, nIndex, "titleIndex", "tabChar.comboBadge" )	
	g_Msg:ChangeTitle( nTitleIndex )	

end

---------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 스탯 적용 결과
function CharInfo_ApplyStatPoint()

	if false == IsLoadWidget( WIDGET_ID_CHARINFO ) then
		return
	end

	CharInfo_UpdateStatInfo()
	CharInfo_UpdateStatBtn()
	CharInfo_UpdateDetail()
	CharInfo_UpdateResist()
	
	-- 능력치가 변함에 따라 스킬창 갱신
	Skill_UpdateAllList()

end

-- 장비 슬롯 업데이트
function CharInfo_UpdateSlot()

	if false == IsLoadWidget( WIDGET_ID_CHARINFO ) then
		return
	end
	
	local tbItems = g_MyChar:GetPutOnItems()
	for value in pairs( tbItems ) do
	
		local tbItem = tbItems[ value ]
		SetSlot_CharInfo( CharSlotToID( SLOT_CHARINFO_ID, value - 1 ), value - 1 )
	
	end
	
	-- 장비 슬롯 업데이트가 일어났으니 능력치, 렌더텍스처도 같이 업데이트
	CharInfo_UpdateStatInfo()
	CharInfo_UpdateDetail()
	CharInfo_UpdateResist()
end

-- 장비 스왑
function CharInfo_ArmSwap( bUseArmSub )

	if false == IsOpenWidget( WIDGET_ID_CHARINFO ) then
		return
	end
	
	if true == bUseArmSub then
		
		SetBool( WIDGET_ID_CHARINFO, "selected", false, "tabChar.weaponSlot.radioLeft" )
		SetBool( WIDGET_ID_CHARINFO, "selected", true, "tabChar.weaponSlot.radioRight" )
		HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioLeft" )
		HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioRight" )
		
	else
		
		SetBool( WIDGET_ID_CHARINFO, "selected", true, "tabChar.weaponSlot.radioLeft" )
		SetBool( WIDGET_ID_CHARINFO, "selected", false, "tabChar.weaponSlot.radioRight" )
		HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioLeft" )
		HitTestEnable( WIDGET_ID_CHARINFO, false, "tabChar.weaponSlot.radioRight" )
		
	end
	
	CharInfo_UpdateSlot()

end

-- 장비 슬롯 개수 업데이트
function CharInfo_UpdatePutOnCount( nSlot, nCount )

	if false == IsLoadWidget( WIDGET_ID_CHARINFO ) then
		return
	end
	
	if 0 == nCount then
	
		ResetSlot( CharSlotToID( SLOT_CHARINFO_ID, nSlot ) )
	
	else
	
		SetSlot_CharInfo( CharSlotToID( SLOT_CHARINFO_ID, nSlot ), nSlot )
	
	end

end

-- 업데이트
function CharInfo_Update()

	-- 타이틀
	local strTitle = GetGameInternalText( "CHARACTER_WINDOW_MAIN_TITLE", 0 )
	strTitle = StringFormat( strTitle, g_MyChar:GetName() )
	SetString( WIDGET_ID_CHARINFO, "title", strTitle, "" )
	
	-- 레벨, 이름
	local strLevelName = "Lv. " .. tostring( g_MyChar:GetLevel() ) .. " " .. g_MyChar:GetName()
	SetString( WIDGET_ID_CHARINFO, "text", strLevelName, "tabChar.labelLevelName" )
	
	-- 생활 점수
	SetString( WIDGET_ID_CHARINFO, "text", tostring( g_MyChar.nLivingPoint ), "tabChar.tfLivingPoint" )
	
	-- 성향
	SetString( WIDGET_ID_CHARINFO, "text", g_MyChar.nAtb, "tabChar.tfAtbPoint" )
	
	-- 캐릭터 슬롯
	CharInfo_UpdateCharacterSlotInfo()
	
end

-- 일정 시간 뒤 캐릭터 능력치 업데이트
function CharInfo_UpdateTerm()

	if false == IsOpenWidget( WIDGET_ID_CHARINFO ) then
		return
	end

	-- 타이머 설정
	local tbQuickSet = 
	{
		time = 250,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "CharInfo_ApplyStatPoint"
	}
	SetTween( WIDGET_ID_CHARINFO, tbQuickSet, "", "", "" )

end

-- 가산능력치 기본 텍스트 설정
function CharInfo_ViewAddonPage1_DefaultStringSet()
	
	for i = 1, 23, 1 do
		SetBool( WIDGET_ID_CHARINFO, "visible", true, "viewAddOn.addOn"..tostring(i) )
		SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", i+15 ), "viewAddOn.addOn"..tostring(i)..".tfName" )	
	
	end	
	
	CharInfo_ViewAddonPage1_ResetChangeUpAddon()
	
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_CRITICAL_TOOLTIP", 0 ), "viewAddOn.addOn1" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_CRUSHING_BLOW_TOOLTIP", 0 ), "viewAddOn.addOn2" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_MOVE_SPEED_TOOLTIP", 0 ), "viewAddOn.addOn3" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_ATK_SPEED_TOOLTIP", 0 ), "viewAddOn.addOn4" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_EXP_MULTIPLE_TOOLTIP", 0 ), "viewAddOn.addOn5" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_HP_TOOLTIP", 0 ), "viewAddOn.addOn6" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_MP_TOOLTIP", 0 ), "viewAddOn.addOn7" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_SP_TOOLTIP", 0 ), "viewAddOn.addOn8" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_PER_CP_TOOLTIP", 0 ), "viewAddOn.addOn9" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_INT_HP_TOOLTIP", 0 ), "viewAddOn.addOn10" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_INT_MP_TOOLTIP", 0 ), "viewAddOn.addOn11" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameInternalText( "CHARACTER_WINDOW_ADDITION_INC_INT_SP_TOOLTIP", 0 ), "viewAddOn.addOn12" )
	
	--for i = 0, 5, 1 do
	--	SetString( WIDGET_ID_CHARINFO, "TooltipText", GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", i ), "viewAddOn.addOn"..tostring(i+18) )	
	--end
	local tbLmtInfo = g_MyChar:GetCharInfoDetailAddonLimitInfo()
	
	local strTooltip_00, strTooltip_01, strTooltip_02 , strTooltip_03
	strTooltip_00 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 0 )
	strTooltip_01 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 1 )
	strTooltip_00 = StringFormat(strTooltip_00, SetFloatToString(tbLmtInfo[18]*100,0) )
	strTooltip_01 = StringFormat(strTooltip_01, SetFloatToString(tbLmtInfo[19],0) )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strTooltip_00, "viewAddOn.addOn18" )	
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strTooltip_01, "viewAddOn.addOn19" )	
	
	strTooltip_00 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 2 )
	strTooltip_01 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 3 )
	strTooltip_02 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 4 )
	strTooltip_03 = GetGameWordText( "CHARACTER_WINDOW_ADDON_TOOLTIP", 5 )
	strTooltip_00 = StringFormat(strTooltip_00, SetFloatToString(g_fEmergency_MobHPR*100, 0),  SetFloatToString(tbLmtInfo[20]*100,0))
	strTooltip_01 = StringFormat(strTooltip_01, SetFloatToString(g_fEmergency_BossHPR*100, 0), SetFloatToString(tbLmtInfo[21]*100,0))
	strTooltip_02 = StringFormat(strTooltip_02, SetFloatToString(g_fEmergency_CharHPR*100, 0), SetFloatToString(tbLmtInfo[22]*100,0))		
	strTooltip_03 = StringFormat(strTooltip_03, SetFloatToString(tbLmtInfo[23]*100,0) )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strTooltip_00, "viewAddOn.addOn20" )	
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strTooltip_01, "viewAddOn.addOn21" )	
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strTooltip_02, "viewAddOn.addOn22" )	
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strTooltip_03, "viewAddOn.addOn23" )	
	
	
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

	SetString( WIDGET_ID_CHARINFO, "TooltipText", strFire, "viewAddOn.addOn13" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strCold, "viewAddOn.addOn14" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strElec, "viewAddOn.addOn15" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strPisn, "viewAddOn.addOn16" )
	SetString( WIDGET_ID_CHARINFO, "TooltipText", strSprt, "viewAddOn.addOn17" )
		
end

-- 캐릭터 능력치 적용
function CharInfo_ViewAddonPage1_CharAbilitySet()
	
	local tbAddonInfo = g_MyChar:GetCharInfoDetailAddonInfo()
	
	-- [크리티컬][강한타격][이동속도, 공격속도 증가율][경험치 증가율][HP, MP, SP, CP 회복율];
	-- [포션 HP, MP, SP 증가량];
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["CriticalRate"] * 100 ), "viewAddOn.addOn1.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["CrushingBlow"] * 100 ), "viewAddOn.addOn2.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["MoveSpeed"]	* 100 - 100 ), "viewAddOn.addOn3.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["AtkSpeed"]		* 100 - 100 ), "viewAddOn.addOn4.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["ExpRate"]		* 100 - 100 ), "viewAddOn.addOn5.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncrHpTotal"]	* 100 ), "viewAddOn.addOn6.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncrMpTotal"]	* 100 ), "viewAddOn.addOn7.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncrSpTotal"]	* 100 ), "viewAddOn.addOn8.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["ReceiveCP"]	* 100 ), "viewAddOn.addOn9.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tbAddonInfo["PotionHpInc"], "viewAddOn.addOn10.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tbAddonInfo["PotionMpInc"], "viewAddOn.addOn11.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tbAddonInfo["PotionSpInc"], "viewAddOn.addOn12.tfValue" )

	-- [저항 화염, 냉기, 전기, 맹독, 정기]
	SetString( WIDGET_ID_CHARINFO, "text", tostring( tbAddonInfo["ResistFire"] ), "viewAddOn.addOn13.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( tbAddonInfo["ResistCold"] ), "viewAddOn.addOn14.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( tbAddonInfo["ResistElec"] ), "viewAddOn.addOn15.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( tbAddonInfo["ResistPisn"] ), "viewAddOn.addOn16.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( tbAddonInfo["ResistSprt"] ), "viewAddOn.addOn17.tfValue" )
	
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncR_Ignore_BossDamage"] * 100 ),		"viewAddOn.addOn18.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tostring( tbAddonInfo["Inc_BossFixedDamage"] ),								"viewAddOn.addOn19.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncR_Critical_EmergencyMob"] * 100 ),	"viewAddOn.addOn20.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncR_Critical_EmergencyBoss"] * 100 ),	"viewAddOn.addOn21.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["Dec_Damage_EmergencyState"] * 100 ),	"viewAddOn.addOn22.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncR_DropGenMoney"] * 100 ),			"viewAddOn.addOn23.tfValue" )
																  	
end																  	

-- 인벤 착용 아이템 임시 적용 스탯 비교
function CharInfo_ViewAddonPage1_Compare( nPosX, nPosY )																  	
	
	local tbAddonInfo = g_MyChar:GetCharTempPuonInfoDetailAddonInfo(nPosX, nPosY, g_CharInfo_VirtualStat1, g_CharInfo_VirtualStat2,
		g_CharInfo_VirtualStat3, g_CharInfo_VirtualStat4, g_CharInfo_VirtualStat5)
		
	if tbAddonInfo == nil or table.getn(tbAddonInfo) == 0 then
		return
	end

	for i = 1, 23, 1 do

		-- 옵션에 따라 소수점 이하로 계산되고 실제 출력시 퍼센테이지로 출력되는것들이 있는데;
		-- 그 옵션들을 보정해주기 위함이다(0.999 -> 99.9%);
		-- Comment.lua의 ITEMADDON의 보정 수치 읽어와서 참조하여 계산하면 되지만;
		-- 굳이 그러필요가 있나 싶어서 따로 겟하지는 않는다;	
		local bInteger = false
		if i == 10 or i == 11 or 
			i == 12 or i == 13 or 
			i == 14 or i == 15 or 
			i == 16 or i == 17 or 
			i == 19 then
			
			bInteger = true
			
		else
		
			tbAddonInfo[ i ] = tbAddonInfo[ i ] * 100.0
			
		end
		
		CharInfo_SetChangeUpAddOn( tbAddonInfo[ i ], "viewAddOn.addOn" .. i, bInteger )
		
	end	
												  	
end	

-- 임시 적용 스탯 비교 후 원래대로 돌리는 기능
function CharInfo_ViewAddonPage1_ResetChangeUpAddon()
	-- 실수
	for i = 1, 23, 1 do
		SetString( WIDGET_ID_CHARINFO, "text", "0.00%", "viewAddOn.addOn" .. tostring(i) .. ".tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn" .. tostring( i ) .. ".mcIcon" )
		
	end
	-- 정수
	for i = 10, 17, 1 do
		SetString( WIDGET_ID_CHARINFO, "text", 0, "viewAddOn.addOn" .. tostring(i) .. ".tfCompare" )
	end
	SetString( WIDGET_ID_CHARINFO, "text", 0, "viewAddOn.addOn19.tfCompare" )
	
end

-- 가산능력치 기본 텍스트 설정
function CharInfo_ViewAddonPage2_DefaultStringSet()	
	
	for i = 1, 18, 1 do
		SetBool( WIDGET_ID_CHARINFO, "visible", true, "viewAddOn.addOn"..tostring(i) )		
		SetString( WIDGET_ID_CHARINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", i+38 ), "viewAddOn.addOn" .. tostring(i) .. ".tfName" )
	end
	CharInfo_ViewAddonPage2_ResetChangeUpAddon()
	
	for i = 19, 23, 1 do
		SetBool( WIDGET_ID_CHARINFO, "visible", false, "viewAddOn.addOn"..tostring(i) )
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
		
		SetString( WIDGET_ID_CHARINFO, "TooltipText", strTip, "viewAddOn.addOn"..tostring(i) )	
		
	end
	
end

-- 캐릭터 능력치 적용
function CharInfo_ViewAddonPage2_CharAbilitySet()

	local tbAddonInfo = g_MyChar:GetCharInfoDetailAddonInfo()
	-- 실수
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["DecR_Damage_MeleeSkill"] * 100 ),	"viewAddOn.addOn1.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["DecR_Damage_RangeSkill"] * 100 ),	"viewAddOn.addOn2.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["DecR_Damage_MagicSkill"] * 100 ),	"viewAddOn.addOn3.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncR_AtkSpeed_SoloPlay"] * 100 ),	"viewAddOn.addOn6.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["IncR_AtkSpeed_PartyPlay"] * 100 ),	"viewAddOn.addOn7.tfValue" )
	-- 정수
	SetString( WIDGET_ID_CHARINFO, "text", tbAddonInfo["Inc_HP_SoloPlay"], "viewAddOn.addOn4.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", tbAddonInfo["Inc_HP_PartyPlay"], "viewAddOn.addOn5.tfValue" )
	
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_CriticalDamage"	] * 100 ),	"viewAddOn.addOn8.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_CrushingBlow"		] * 100 ),	"viewAddOn.addOn9.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_LifePerKill"		] * 100 ),	"viewAddOn.addOn10.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_MPPerKill"			] * 100 ),	"viewAddOn.addOn11.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_SPPerKill"			] * 100 ),	"viewAddOn.addOn12.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_HPMPSPPerKill"		] * 100 ),	"viewAddOn.addOn13.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_AllMPRatioToAttack"] * 100 ),	"viewAddOn.addOn14.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_AllSPRatioToHp"	] * 100 ),	"viewAddOn.addOn15.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_AllHitRatioToMelee"] * 100 ),	"viewAddOn.addOn16.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_AllHitRatioToRange"] * 100 ),	"viewAddOn.addOn17.tfValue" )
	SetString( WIDGET_ID_CHARINFO, "text", string.format( "%.2f%%", tbAddonInfo["GetIncR_AllHitRatioToMagic"] * 100 ),	"viewAddOn.addOn18.tfValue" )
																																		
end																																		
																																		
-- 인벤 착용 아이템 임시 적용 스탯 비교																							   
function CharInfo_ViewAddonPage2_Compare( nPosX, nPosY )																				

	local tbAddonInfo = g_MyChar:GetCharTempPuonInfoDetailAddonInfo(nPosX, nPosY, g_CharInfo_VirtualStat1, g_CharInfo_VirtualStat2,
		g_CharInfo_VirtualStat3, g_CharInfo_VirtualStat4, g_CharInfo_VirtualStat5)
	
	if tbAddonInfo == nil or table.getn(tbAddonInfo) == 0 then
		return
	end
	
	for i = 1, 18, 1 do
		local nIdx = i + 23
		local bInteger = false
		if i == 4 or i == 5 then
		
			bInteger = true
			
		else
			
			tbAddonInfo[nIdx] = tbAddonInfo[nIdx] * 100.0
			
		end
		CharInfo_SetChangeUpAddOn( tbAddonInfo[nIdx], "viewAddOn.addOn" .. i, bInteger )
	end
	
end

-- 임시 적용 스탯 비교 후 원래대로 돌리는 기능
function CharInfo_ViewAddonPage2_ResetChangeUpAddon()
	-- 실수
	for i = 1, 18, 1 do
		SetString( WIDGET_ID_CHARINFO, "text", "0.00%", "viewAddOn.addOn" .. tostring(i) .. ".tfCompare" )
		CallFunction( WIDGET_ID_CHARINFO, "gotoAndStop", "NONE", "viewAddOn.addOn" .. tostring( i ) .. ".mcIcon" )
	end
	
	-- 정수
	SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn4.tfCompare" )
	SetString( WIDGET_ID_CHARINFO, "text", "0", "viewAddOn.addOn5.tfCompare" )
	
end

-- 가산 옵션 자세히 보기 페이지 선택
function Ev_CharInfo_ViewAddon_Page( strIndex )
	
	local nIndex = tonumber( strIndex )
	if 		nIndex == 1 then
		CharInfo_ViewAddonPage1_DefaultStringSet()
		CharInfo_ViewAddonPage1_CharAbilitySet()
		
	elseif	nIndex == 2 then
		CharInfo_ViewAddonPage2_DefaultStringSet()
		CharInfo_ViewAddonPage2_CharAbilitySet()
		
	end
	
end

-- 인벤 착용 아이템 임시 적용 스탯 비교
function CharInfo_ViewAddon_CompareInvenItem(nPosX, nPosY)
	
	----------------------------------------------------------------
	-- 스탯, 기본 옵션 비교
	----------------------------------------------------------------
	local tbStat = g_MyChar:TempPutOnStatInvenItem( nPosX, nPosY, g_CharInfo_VirtualStat1, g_CharInfo_VirtualStat2,
		g_CharInfo_VirtualStat3, g_CharInfo_VirtualStat4, g_CharInfo_VirtualStat5 )
	
	if tbStat ~= nil then
		for i = 1, 11, 1 do
			if tbStat[ i ] ~= nil then
				CharInfo_SetChangeUpAbillity( tbStat[ i ], "tabChar.Detail" .. tostring( i ) )
			end
		end	
		CharInfo_UpdateStatInfo( tbStat[ 24 ], tbStat[ 25 ], tbStat[ 26 ], tbStat[ 27 ], tbStat[ 28 ] )
		
	end
	
	----------------------------------------------------------------
	-- 가산옵션 추가 페이지에서 비교
	----------------------------------------------------------------
	if false == GetBool( WIDGET_ID_CHARINFO, "visible", "viewAddOn" ) then
		return
	end
	
	local nSelectIdx = 1
	for i = 1, 2, 1 do				
		if GetBool( WIDGET_ID_CHARINFO, "selected", "viewAddOn.radioPage" .. i ) == true then
			nSelectIdx = i
		end
	end	
	
	if nSelectIdx == 1 then
		CharInfo_ViewAddonPage1_Compare(nPosX, nPosY)
	else
		CharInfo_ViewAddonPage2_Compare(nPosX, nPosY)
	end
	
end

-- 임시 적용 스탯 비교 후 원래대로 돌리는 기능
function CharInfo_ViewAddonPage_ResetChange()
	
	if false == GetBool( WIDGET_ID_CHARINFO, "visible", "viewAddOn" ) then
		return
	end
	
	local nSelectIdx = 1
	for i = 1, 2, 1 do				
		if GetBool( WIDGET_ID_CHARINFO, "selected", "viewAddOn.radioPage" .. i ) == true then
			nSelectIdx = i
		end
	end	
	
	if nSelectIdx == 1 then
		CharInfo_ViewAddonPage1_ResetChangeUpAddon()
	else
		CharInfo_ViewAddonPage2_ResetChangeUpAddon()
	end
	
end