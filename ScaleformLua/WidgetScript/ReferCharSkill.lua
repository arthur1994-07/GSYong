
g_bReferCharSkill_InitTab = false
g_bReferCharSkill_InitData = false

-- 정보보기 캐릭터 스킬 리스트
g_tbReferCharSkill_List = {}

-- 정보보기 스킬 UI 로드 완료
function LoadReferCharSkill()

	g_bReferCharSkill_InitTab = false

	-- 필요 없는 컴포넌트 visible 끄기 (스킬창에서만 사용)
	SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "labelSkillPoint" )
	SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "btnApply" )
	SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "btnReset" )
	SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "HelpBtn" )
	SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "btnStat_SkillReset" )
		
	-- 캐릭터 버튼 설정
	SetString( WIDGET_ID_REFERCHAR_SKILL, "label", GetGameWordText( "CHARACTER_NAME_STATIC", 0 ), "btnCharInfoSwap" )
	SetEvent( WIDGET_ID_REFERCHAR_SKILL, EVENT_BUTTON_CLICK, "Ev_ReferCharSkill_ShowChar", "", "btnCharInfoSwap" )
	
	-- 슬롯 바인딩
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
	
		SetString( WIDGET_ID_REFERCHAR_SKILL, "binding", SLOT_REFERCHAR_SKILL_ID .. tostring( i ),
			"item" .. tostring( i ) .. ".slot" )
			
		SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "item" .. tostring( i ) .. ".btnUp" )
		SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "item" .. tostring( i ) .. ".btnDown" )
	
	end
	
	-- 탭 변경 이벤트
	SetEvent( WIDGET_ID_REFERCHAR_SKILL, EVENT_INDEX_CLICK_CHANGE, "Ev_ReferCharSkill_ChangeTabIndex", "index", "tabPage" )
	
	-- 스킬 리스트 스크롤 이벤트
	SetEvent( WIDGET_ID_REFERCHAR_SKILL, EVENT_SCROLL, "Ev_ReferCharSkill_ListScroll", "", "exList.list" )

end

-- 정보보기 스킬 UI 오픈
function OpenReferCharSkill()

	if true == g_bReferCharSkill_InitData then
		return
	end

	-- 정보보기 캐릭터창 위치로 이동
	ReferCharSkill_RePos()

	-- 타이틀
	local str = StringFormat( GetGameInternalText( "CHARACTER_WINDOW_MAIN_TITLE", 0 ), g_ReferChar_Name )
	SetString( WIDGET_ID_REFERCHAR_SKILL, "title", str, "" )
	
	ReferCharSkill_UpdateTabText()
	SetInt( WIDGET_ID_REFERCHAR_SKILL, "selectedIndex", 0, "tabPage" )
	
	ReferCharSkill_SetListSlot( 0 )
	
	g_bReferCharSkill_InitData = true
	
end

-- 정보보기 캐릭터창 위치로 이동
function ReferCharSkill_RePos()

	local nWidth = GetInt( WIDGET_ID_REFERCHAR_SKILL, "width", "" )
	local nHeight = GetInt( WIDGET_ID_REFERCHAR_SKILL, "height", "" )
	
	local nNewX = g_nReferChar_RePosX
	local nNewY = g_nReferChar_RePosY
	
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	if nScreenWidth < nWidth + g_nReferChar_RePosX then
		nNewX = nScreenWidth - nWidth
	end
	
	if nScreenHeight < nHeight + g_nReferChar_RePosY then
		nNewY = nScreenHeight - nHeight
	end

	SetInt( WIDGET_ID_REFERCHAR_SKILL, "x", nNewX, "" )
	SetInt( WIDGET_ID_REFERCHAR_SKILL, "y", nNewY, "" )

end

-- 스킬 탭 텍스트 업데이트
function ReferCharSkill_UpdateTabText()

	-- 탭 XML ID 리스트
	local tbTabStringID = 
	{
		"SKILL_TAB_TITLE_FIGHTER",
		"SKILL_TAB_TITLE_ARMS",
		"SKILL_TAB_TITLE_ARCHER",
		"SKILL_TAB_TITLE_SPIRIT",
		"SKILL_TAB_TITLE_EXTREME_M",
		"SKILL_TAB_TITLE_EXTREME_M",
		"SKILL_TAB_TITLE_FIGHTER",
		"SKILL_TAB_TITLE_ARMS",
		"SKILL_TAB_TITLE_ARCHER",
		"SKILL_TAB_TITLE_SPIRIT",
		"SKILL_TAB_TITLE_SCIENTIST",
		"SKILL_TAB_TITLE_SCIENTIST",
		"SKILL_TAB_TITLE_ASSASSIN",
		"SKILL_TAB_TITLE_ASSASSIN",
		"SKILL_TAB_TITLE_TRICKER",
		"SKILL_TAB_TITLE_TRICKER",
		"SKILL_TAB_TITLE_ETC",
		"SKILL_TAB_TITLE_ETC",
		"SKILL_TAB_TITLE_ACTOR_M",
		"SKILL_TAB_TITLE_ACTOR_M",
	}
	
	-- 탭 설정
	local tbSkillPoint = g_Refer:GetSkillTabPoint()
	
	-- 탭 데이터가 초기화 되었다면 텍스트만 변경
	if true == g_bReferCharSkill_InitTab then
	
		for i = 1, 4, 1 do

			local nUsingSkillPoint = tbSkillPoint[ i ]
			
			local strID = tbTabStringID[ g_Refer:GetCharClass() + 1 ]
			local strParam = CreateParam( i - 1,
				GetGameWordText( strID, i - 1 ) .. "(" .. nUsingSkillPoint .. ")" )

			CallFunction( WIDGET_ID_REFERCHAR_SKILL, "ChangeButtonLabel", strParam, "tabPage" )
			
		end
	
	-- 탭 데이터 초기화
	else
	
		local strTabData = ""
		for i = 1, 4, 1 do
		
			local nUsingSkillPoint = tbSkillPoint[ i ]
			local strID = tbTabStringID[ g_Refer:GetCharClass() + 1 ]
			
			strTabData = strTabData .. GetGameWordText( strID, i - 1 ) .. "(" .. tostring( nUsingSkillPoint ) .. ");"
		
		end
		SetData( WIDGET_ID_REFERCHAR_SKILL, strTabData, "tabPage", false )
	
		g_bReferCharSkill_InitTab = true
	
	end

end

-- 스킬 리스트 설정
function ReferCharSkill_SetListSlot( nIndex )

	g_tbReferCharSkill_List = g_Refer:GetSkillData( nIndex )
	
	-- nullptr 체크
	if nil == g_tbReferCharSkill_List then
	
		return
		
	end
	
	local tbListData = {}
	for i = 1, #g_tbReferCharSkill_List, 1 do
	
		local tbSkill = g_tbReferCharSkill_List[ i ]
		if nil == tbSkill then
		
			local tbData = 
			{
				"",	-- 스킬 이름
				"",	-- 스킬 ID
				"",			-- 스킬 레벨
			}
			
			table.insert( tbListData, tbData )
		
		else
		
			local str = tostring( tbSkill[ 3 ] ) .. GetGameWordText( "SKILL_SLOT", 0 ) .. 
			" Lv. " .. tostring( tbSkill[ 2 ] )
		
			local tbData = 
			{
				tbSkill[ 4 ],	-- 스킬 이름
				tbSkill[ 1 ],	-- 스킬 ID
				str,			-- 스킬 레벨
			}
			
			table.insert( tbListData, tbData )
		
		end
	
	end
	
	SetInt( WIDGET_ID_REFERCHAR_SKILL, "scrollPosition", 0, "exList.list" )
	SetDataEx( WIDGET_ID_REFERCHAR_SKILL, "label;skillID;label2", tbListData, "exList.list", false )
	
	ReferCharSkill_UpdateScroll()

end

-- 스킬 리스트 스크롤 업데이트
function ReferCharSkill_UpdateScroll()

	local nScrollPos = GetInt( WIDGET_ID_REFERCHAR_SKILL, "scrollPosition", "exList.list" )

	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
	
		local nIndex = i + ( nScrollPos * SKILL_SLOT_COUNT_COL )
		
		local tbSkill = g_tbReferCharSkill_List[ nIndex ]
		if nil == tbSkill then
		
			SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", false, "item" .. tostring( i ) .. ".slot" )
		
		else
		
			SetBool( WIDGET_ID_REFERCHAR_SKILL, "visible", true, "item" .. tostring( i ) .. ".slot" )
			
			local bEnable = false
			local nLevel = tbSkill[ 2 ]
			if 0 ~= tbSkill[ 2 ] then
			
				bEnable = true
				nLevel = nLevel - 1
				
			end
			
			SetSlot_SkillLevel( SLOT_REFERCHAR_SKILL_ID .. tostring( i ), tbSkill[ 1 ], nLevel )
			SetBool( WIDGET_ID_REFERCHAR_SKILL, "enabled", bEnable, "item" .. tostring( i ) .. ".slot" )
		
		end
	
	end

end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 캐릭터 보기
function Ev_ReferCharSkill_ShowChar()

	g_nReferChar_RePosX = GetInt( WIDGET_ID_REFERCHAR_SKILL, "x", "" )
	g_nReferChar_RePosY = GetInt( WIDGET_ID_REFERCHAR_SKILL, "y", "" )

	CloseWidget( WIDGET_ID_REFERCHAR_SKILL )
	OpenWidget( WIDGET_ID_REFERCHAR )

end

-- 탭 인덱스 변경
function Ev_ReferCharSkill_ChangeTabIndex( nIndex )

	ReferCharSkill_SetListSlot( nIndex )

end

-- 스킬 리스트 스크롤
function Ev_ReferCharSkill_ListScroll()

	ReferCharSkill_UpdateScroll()

end