  
g_nSkill_SelectPage = 0			-- 선택한 페이지
g_nSkill_MaxTabIndex = 4		-- 최대 탭 인덱스

g_tbSkill_AllList = {}			-- 캐릭터 스킬 전체 리스트

g_tbSkill_TempSkillPoint = {}			-- 임시 스킬 포인트 리스트
g_tbSkill_TempSkillPointDetail = {}		-- 개별 스킬 포인트 투자값
g_tbSkill_TempSkillLevel = {}			-- 임시 투자한 스킬 레벨
g_tbSkill_TempSkillList = {}			-- 임시 투자 스킬 리스트
g_tbSkill_LevelUpSkill = {}				-- 스킬 레벨 올릴 리스트
g_nSkill_SkillUpMoney = 0				-- 스킬 레벨 올리기 위한 필요 금액

-- 탭 XML ID 리스트
local g_tbTabStringID = 
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
	
-- 스킬 UI 로드 완료 
function LoadSkill()

	-- 닫기
	SetEvent( WIDGET_ID_SKILL, EVENT_WIDGET_CLOSE, "Ev_CloseSkill", "", "" )
	
	-- 스킬 리스트 초기화 안되어 있으면 리스트 가져오기
	if 0 == table.getn( g_tbSkill_AllList ) then
		for i = 1, g_nSkill_MaxTabIndex, 1 do
			table.insert( g_tbSkill_AllList, g_MyChar:GetSkillList( i - 1 ) )
		end
	end
	
	-- 임시 스킬 포인트 리스트 초기화
	if 0 == table.getn( g_tbSkill_TempSkillPoint ) then
		for i = 1, g_nSkill_MaxTabIndex, 1 do
			g_tbSkill_TempSkillPoint[ i ] = 0
		end
	end

	-- 탭 설정
	local strTabData = ""
	for i = 1, g_nSkill_MaxTabIndex, 1 do
	
		local nUsingSkillPoint = g_MyChar:GetUsingSkillPoint( g_tbSkill_AllList[ i ] )
		
		local strID = g_tbTabStringID[ g_MyChar.nClassIndex + 1 ]
		strTabData = strTabData .. GetGameWordText( strID, i - 1 ) .. "(" .. tostring( nUsingSkillPoint ) .. ");"
	
	end
	
	SetData( WIDGET_ID_SKILL, strTabData, "tabPage", false )
	SetEvent( WIDGET_ID_SKILL, EVENT_INDEX_CLICK_CHANGE, "Ev_Skill_ChangeTabIndex", "index", "tabPage" )

	-- 슬롯 바인딩
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do	
	
		SetString( WIDGET_ID_SKILL, "binding", SLOT_SKILL_ID .. tostring( i ), "item" .. tostring( i ) .. ".slot" )	
		SetBool( WIDGET_ID_SKILL, "holdable", true, "item" .. tostring( i ) .. ".slot" )	
	
	end
	
	-- 리스트 스크롤 이벤트
	SetEvent( WIDGET_ID_SKILL, EVENT_SCROLL, "Ev_Skill_ScrollList", "", "exList.list" )
	
	-- 포인트 초기화 버튼 이벤트
	SetEvent( WIDGET_ID_SKILL, EVENT_BUTTON_CLICK, "Ev_Skill_btnReset", "", "btnReset" )
	-- 포인트 적용 버튼 이벤트
	SetEvent( WIDGET_ID_SKILL, EVENT_BUTTON_CLICK, "Ev_Skill_btnApply", "", "btnApply" )

	-- +,- 버튼 이벤트
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
	
		local strItemInst = "item" .. i .. "."
		SetEvent( WIDGET_ID_SKILL, EVENT_BUTTON_CLICK, "Ev_Skill_BtnUp", "", strItemInst .. "btnUp", i )
		SetEvent( WIDGET_ID_SKILL, EVENT_BUTTON_CLICK, "Ev_Skill_BtnDown", "", strItemInst .. "btnDown", i )
	
		SetEvent( WIDGET_ID_SKILL, EVENT_MOUSE_OVER, "Ev_Skill_BtnOver", "stageX;stageY", strItemInst .. "btnUp", i )
		SetEvent( WIDGET_ID_SKILL, EVENT_MOUSE_OUT, "Ev_Skill_BtnOut", "", strItemInst .. "btnUp", i )
		
		SetEvent( WIDGET_ID_SKILL, EVENT_MOUSE_OVER, "Ev_Skill_BtnOver", "stageX;stageY", strItemInst .. "mcNextLevelOver", i )
		SetEvent( WIDGET_ID_SKILL, EVENT_MOUSE_OUT, "Ev_Skill_BtnOut", "", strItemInst .. "mcNextLevelOver", i )
		SetBool( WIDGET_ID_SKILL, "visible", false, strItemInst .. "mcNextLevelOver" )
	
	end
	
	-- 탭 선택
	SetInt( WIDGET_ID_SKILL, "selectedIndex", g_nSkill_SelectPage, "tabPage" )
	
	-- 캐릭터 전환 버튼 VISIBLE 끄기 (정보보기에서 사용)
	SetBool( WIDGET_ID_SKILL, "visible", false, "btnCharInfoSwap" )
	
	Skill_UpdateTabText()
	Skill_UpdateList()
			
	-- 임시적용 포인트 초기화 버튼 툴팁
	SetString( WIDGET_ID_SKILL, "TooltipText", Skill_GetResetTooltipText(), "btnReset" )
	SetString( WIDGET_ID_SKILL, "TooltipText", Skill_GetApplyTooltipText(), "btnApply" )
	
	-- 도움말 버튼
	SetEvent( WIDGET_ID_SKILL, EVENT_BUTTON_CLICK, "Event_Skill_Help", "", "HelpBtn" )
	
	-- 스텟, 스킬 리셋 버튼 
	SetString( WIDGET_ID_SKILL, "label", GetGameWordText("SKILL_STAT_SKILL_RESET"), "btnStat_SkillReset")
	SetString( WIDGET_ID_SKILL, "TooltipText", Skill_GetResetStat_Skill_TooltipText(), "btnStat_SkillReset" )
	SetEvent( WIDGET_ID_SKILL, EVENT_BUTTON_CLICK, "Ev_Stat_Skill_btnReset", "", "btnStat_SkillReset" )
	
end

-- 스킬 오픈
function OpenSkill()

	Skill_UpdateAllList()

end

-- 리셋
function Skill_Reset()

	g_nSkill_SelectPage = 0

	g_tbSkill_AllList = {}

	for i = 1, g_nSkill_MaxTabIndex, 1 do
		g_tbSkill_TempSkillPoint[ i ] = 0
	end
		
	g_tbSkill_TempSkillPointDetail = {}
	g_tbSkill_TempSkillLevel = {}
	g_tbSkill_TempSkillList = {}
	g_tbSkill_LevelUpSkill = {}
	
	g_nSkill_SkillUpMoney = 0 

end

-- 투자 포인트 리셋
function Skill_ResetInvestPoint()

	for i = 1, g_nSkill_MaxTabIndex, 1 do
		g_tbSkill_TempSkillPoint[ i ] = 0
	end
	
	g_tbSkill_TempSkillPointDetail = {}
	g_tbSkill_TempSkillLevel = {}
	g_tbSkill_TempSkillList = {}
	g_tbSkill_LevelUpSkill = {}
	
	g_nSkill_SkillUpMoney = 0

end

-- 페이지 탭 텍스트 업데이트
function Skill_UpdateTabText()
	
	local strTabData = ""
	for i = 1, g_nSkill_MaxTabIndex, 1 do
	
		local nUsingSkillPoint = g_MyChar:GetUsingSkillPoint( g_tbSkill_AllList[ i ] ) + g_tbSkill_TempSkillPoint[ i ]
		local strID = g_tbTabStringID[ g_MyChar.nClassIndex + 1 ]
		
		local strParam = CreateParam( i - 1, GetGameWordText( strID, i - 1 ) .. "(" .. tostring( nUsingSkillPoint ) .. ")" )
		CallFunction( WIDGET_ID_SKILL, "ChangeButtonLabel", strParam, "tabPage" )
	
	end

end


-- 리스트 업데이트
function Skill_UpdateList()

	if false == IsLoadWidget( WIDGET_ID_SKILL ) then
		return
	end

	local tbListData = {}
	
	local tbSkillList = g_tbSkill_AllList[ g_nSkill_SelectPage + 1 ]
	for value in pairs( tbSkillList ) do
	
		-- 값이 수정되면 원본 테이블 값도 수정되니 복사해온다.
		local tbSkill = CopyTable( tbSkillList[ value ] )
		-- 포인트 투자한 스킬인지 확인
		local tbTempSkill = g_tbSkill_TempSkillList[ tostring( tbSkill[ "dwID" ] ) ]
		if nil ~= tbTempSkill then
		
			-- 포인트 투자한 스킬은 클라이언트에서 선행 스킬에 대한 정보가 안날라오니 여기서 추가
			tbTempSkill[ "dwNeedSkillID" ] = tbSkill[ "dwNeedSkillID" ]
			tbTempSkill[ "strNeedSkillName" ] = tbSkill[ "strNeedSkillName" ]
			tbTempSkill[ "bNeedSkill" ] = tbSkill[ "bNeedSkill" ]
			tbSkill = tbTempSkill
		
		end
		
		-- 선행 스킬 조건 검사, 이미 충족한다면 검사할 필요 없다.
		if false == tbSkill[ "bNeedSkill" ] then
			
			-- 포인트 투자한 스킬중에 선행 스킬이 있는지 확인
			local tbNeedSkill = g_tbSkill_TempSkillList[ tostring( tbSkill[ "dwNeedSkillID" ] ) ]
			if nil ~= tbNeedSkill then
			
				-- 스킬 레벨이 맞다면 선행 스킬 조건 충족
				if tbSkill[ "dwNeedSkillLevel" ] <= tbNeedSkill[ "wLevel" ] then
					tbSkill[ "bNeedSkill" ] = true
				end
				
			end
			
		end
		
		local tbData = {}
		tbData[ 1 ] = tbSkill[ "strName" ]	-- 스킬 이름
		tbData[ 2 ] = tbSkill[ "dwID" ]		-- 스킬 ID
		
		----------------------------------------------------------------------------------------
		
		-- 습득 불가?
		local bDisableLearn = false
		
		-- 캐릭터 레벨
		if tbSkill[ "dwNeedLevel" ] > g_MyChar:GetLevel() then
			bDisableLearn = true
		end
		
		-- 선행 스킬
		if false == tbSkill[ "bNeedSkill" ] then
			bDisableLearn = true
		end
		
		-- 스킬 포인트
		if 	g_MyChar.nSkillPoint - Skill_GetUsingPoint() < tbSkill[ "dwNeedPoint" ] and
			0 == tbSkill[ "wLevel" ] then
			bDisableLearn = true
		end
		
		-- 요구 능력치
		if 0 == tbSkill[ "bOnlyOneStat" ] then
		
			-- 힘
			if tbSkill[ "wPow" ] > g_MyChar:GetPow_Skill() then
				bDisableLearn = true
			end
			
			-- 민첩
			if tbSkill[ "wDex" ] > g_MyChar:GetDex_Skill() then
				bDisableLearn = true
			end
			
			-- 체력
			if tbSkill[ "wStr" ] > g_MyChar:GetStr_Skill() then
				bDisableLearn = true
			end
			
			-- 정신
			if tbSkill[ "wSpi" ] > g_MyChar:GetSpi_Skill() then
				bDisableLearn = true
			end
			
			-- 근력
			if tbSkill[ "wSta" ] > g_MyChar:GetSta_Skill() then
				bDisableLearn = true
			end
		
		else
		
			if  tbSkill[ "wPow" ] > g_MyChar:GetPow_Skill() and
				tbSkill[ "wDex" ] > g_MyChar:GetDex_Skill() and
				tbSkill[ "wStr" ] > g_MyChar:GetStr_Skill() and
				tbSkill[ "wSpi" ] > g_MyChar:GetSpi_Skill() and
				tbSkill[ "wSta" ] > g_MyChar:GetSta_Skill() then
				
				-- 힘
				if tbSkill[ "wPow" ] > g_MyChar:GetPow_Skill() then
					bDisableLearn = true
				end
				
				-- 민첩
				if tbSkill[ "wDex" ] > g_MyChar:GetDex_Skill() then
					bDisableLearn = true
				end
				
				-- 체력
				if tbSkill[ "wStr" ] > g_MyChar:GetStr_Skill() then
					bDisableLearn = true
				end
				
				-- 정신
				if tbSkill[ "wSpi" ] > g_MyChar:GetSpi_Skill() then
					bDisableLearn = true
				end
				
				-- 근력
				if tbSkill[ "wSta" ] > g_MyChar:GetSta_Skill() then
					bDisableLearn = true
				end
				
			end
		
		end
		
		-- 요구 머니
		if false == Skill_CheckMoney( tbSkill ) then
			bDisableLearn = true
		end
		
		-- 스킬 상태 텍스트 생성
		local strState = ""
		local nColor = TEXT_COLOR_DARKRED
		
		-- 레벨, 선행스킬, 스킬포인트, 요구 능력치 모두 만족
		if false == bDisableLearn then
		
			strState = tostring( tbSkill[ "dwGrade" ] ) .. GetGameWordText( "SKILL_SLOT", 0 ) .. 
				" Lv. " .. tostring( tbSkill[ "wLevel" ] )
			
			if nil ~= g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] then
			
				strState = strState ..  string.format( " (%d)", g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] )
				nColor = TEXT_COLOR_ORANGE
				
			else
			
				nColor = TEXT_COLOR_GRAY
			
			end
			
		else
		
			strState = GetGameWordText( "SKILL_DISABLE_LEARN", 0 )
			nColor = TEXT_COLOR_DARKRED
			
		end
		
		tbData[ 3 ] = strState
		tbData[ 4 ] = nColor
		----------------------------------------------------------------------------------------
		
		table.insert( tbListData, tbData )
	
	end
	
	SetDataEx( WIDGET_ID_SKILL, "label;skillID;label2;color2;", tbListData, "exList.list", false )

end

-- 리스트 상태 업데이트
function Skill_UpdateListState( nScrollPos )

	local tbSkillList = g_tbSkill_AllList[ g_nSkill_SelectPage + 1 ]
	local nOffset = nScrollPos * SKILL_SLOT_COUNT_COL
	
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
	
		local strItemInst = "item" .. tostring( i ) .. "."
		
		local nIndex = i + nOffset
		if nIndex <= table.getn( tbSkillList ) then
		
			local tbSkill = CopyTable( tbSkillList[ nIndex ] )
			
			-- +,- 버튼 보이기
			SetBool( WIDGET_ID_SKILL, "visible", true, strItemInst .. "btnUp" )
			SetBool( WIDGET_ID_SKILL, "visible", true, strItemInst .. "btnDown" )
			
			-- +,- 버튼 상태
			local tbTempSkill = g_tbSkill_TempSkillList[ tostring( tbSkill[ "dwID" ] ) ]
			-- 포인트 투자한 스킬인지 확인
			if nil ~= tbTempSkill then
			
				tbTempSkill[ "dwNeedSkillID" ] = tbSkill[ "dwNeedSkillID" ]
				tbTempSkill[ "strNeedSkillName" ] = tbSkill[ "strNeedSkillName" ]
				tbTempSkill[ "bNeedSkill" ] = tbSkill[ "bNeedSkill" ]
				tbSkill = tbTempSkill
				
				-- - 버튼 활성
				SetBool( WIDGET_ID_SKILL, "enabled", true, strItemInst .. "btnDown" )
				
			else
			
				-- 포인트 투자한 스킬 아니면 - 버튼 비활성
				SetBool( WIDGET_ID_SKILL, "enabled", false, strItemInst .. "btnDown" )
				
			end
			
			if 0 == tbSkill[ "wLevel" ] then
			
				SetBool( WIDGET_ID_SKILL, "enabled", false, strItemInst .. "slot" )
			
			else
			
				SetBool( WIDGET_ID_SKILL, "enabled", true, strItemInst .. "slot" )
			
			end
			
			local nLevel = tbSkill[ "wLevel" ]
			if 0 < nLevel then
				nLevel = nLevel - 1
			end
			
			-- 슬롯 업데이트
			SetSlot_SkillLevel( SLOT_SKILL_ID .. tostring( i ), tbSkill[ "dwID" ], nLevel )
			SetBool( WIDGET_ID_SKILL, "visible", true, strItemInst .. "slot" )
			
			-- 패시브 스킬 드래그 안되도록
			SetBool( WIDGET_ID_SKILL, "holdable", not tbSkill[ "bPassive" ], strItemInst .. "slot" )
			
			-- 선행 스킬 조건 검사, 이미 충족한다면 검사할 필요 없다.
			if false == tbSkill[ "bNeedSkill" ] then
				
				-- 포인트 투자한 스킬중에 선행 스킬이 있는지 확인
				local tbNeedSkill = g_tbSkill_TempSkillList[ tostring( tbSkill[ "dwNeedSkillID" ] ) ]
				if nil ~= tbNeedSkill then
				
					-- 스킬 레벨이 맞다면 선행 스킬 조건 충족
					if  tbSkill[ "dwNeedSkillLevel" ] <= tbNeedSkill[ "wLevel" ] then
						tbSkill[ "bNeedSkill" ] = true
					end
					
				end
				
			end
			
			local wNextLevel, tbSkillID = tbSkill[ "wLevel" ], SplitNativeID( tostring( tbSkill[ "dwID" ] ) )
			
			-- + 버튼 상태 조건 확인 ( 스킬최대레벨 / 스킬포인트 / 캐릭터레벨 / 선행스킬 )
			if 	tbSkill[ "dwMaxLevel" ] <= wNextLevel or
				g_MyChar.nSkillPoint - Skill_GetUsingPoint() < tbSkill[ "dwNeedPoint" ] or
				g_MyChar:GetLevel() < tbSkill[ "dwNeedLevel" ] or
				false == tbSkill[ "bNeedSkill" ] or
				false == Skill_CheckStat( tbSkill ) or 
				false == Skill_CheckMoney( tbSkill ) then
				
				SetBool( WIDGET_ID_SKILL, "enabled", false, strItemInst .. "btnUp" )
				-- 스킬 툴팁은 최대레벨을 벗어나지만 않으면 무조건 보여준다;
				local bOverVisible = tbSkill[ "dwMaxLevel" ] > wNextLevel
				SetBool( WIDGET_ID_SKILL, "visible", bOverVisible, strItemInst .. "mcNextLevelOver" )
				
				RegShowTooltipEnableState( WIDGET_ID_SKILL, 0, strItemInst .. "mcNextLevelOver", tbSkillID[ 1 ], tbSkillID[ 2 ], wNextLevel, false )
				--PrintDebug("wNextLevel : " ..tostring(wNextLevel))
				
			else
			
				SetBool( WIDGET_ID_SKILL, "enabled", true, strItemInst .. "btnUp" )
				SetBool( WIDGET_ID_SKILL, "visible", false, strItemInst .. "mcNextLevelOver" )
				
				RegShowTooltipEnableState( WIDGET_ID_SKILL, 0, strItemInst .. "btnUp", tbSkillID[ 1 ], tbSkillID[ 2 ], wNextLevel, false )
				--PrintDebug("wNextLevel : " ..tostring(wNextLevel))
				
			end
			
		else
		
			-- 슬롯 감추기
			SetBool( WIDGET_ID_SKILL, "visible", false, strItemInst .. "slot" )
			
			-- +,- 버튼 감추기
			SetBool( WIDGET_ID_SKILL, "visible", false, strItemInst .. "btnUp" )
			SetBool( WIDGET_ID_SKILL, "visible", false, strItemInst .. "btnDown" )
			
		end
		
	end

end

-- 스킬 포인트 업데이트
function Skill_UpdateSkillPoint()

	-- 현재 페이지에 사용한 포인트
	SetString( WIDGET_ID_SKILL, "label",
		tostring( g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] ) .. " " .. GetGameWordText( "RNSKILLWINDOW_TITLE", 3 ),
		"btnReset" )
	
	-- 총 사용한 포인트
	local nTotalPoint = Skill_GetUsingPoint()
	SetString( WIDGET_ID_SKILL, "label",
		tostring( nTotalPoint ) .. " " .. GetGameWordText( "RNSKILLWINDOW_TITLE", 2 ),
		"btnApply" )
	
	-- 사용 가능한 포인트
	local nAvailablePoint = g_MyChar.nSkillPoint - nTotalPoint
	SetString( WIDGET_ID_SKILL, "text", 
		GetGameWordText( "QUEST_GIFT", 5 ) .. " : " .. tostring( nAvailablePoint ),
		"labelSkillPoint" )

end

-- 투자한 포인트
function Skill_GetUsingPoint()

	-- 총 사용한 포인트
	local nTotalPoint = 0
	for value in pairs( g_tbSkill_TempSkillPoint ) do
		nTotalPoint = nTotalPoint + g_tbSkill_TempSkillPoint[ value ]
	end
	
	return nTotalPoint

end

-- 현재 보여지고 있는 리스트 스킬 검색
function Skill_FindSkill( nIndex )

	local tbSkillList = g_tbSkill_AllList[ g_nSkill_SelectPage + 1 ]
	
	local nOffset = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" ) * SKILL_SLOT_COUNT_COL
	
	local nListIndex = nIndex + nOffset
	if nListIndex <= table.getn( tbSkillList ) then	
		return tbSkillList[ nListIndex ]
	end
	
	return nil

end

-- 전체 스킬 검색
function Skill_FindSkillAll( nID )

	for i = 1, g_nSkill_MaxTabIndex, 1 do
	
		local tbSkillList = g_tbSkill_AllList[ i ]
		if nil ~= tbSkillList then
		
			for value in pairs( tbSkillList ) do
			
				local tbSkill = tbSkillList[ value ]
				if tbSkill[ "dwID" ] == nID then
					return tbSkill
				end
			
			end
		
		end
	
	end
	
	return nil

end

-- 스킬 임시 투자
function Skill_TempInvest( bUp, nIndex )

	local tbSkill = Skill_FindSkill( nIndex )
	if nil == tbSkill then
		return false;
	end
	
	local strSkillID = tostring( tbSkill[ "dwID" ] )
	local nLevel = g_tbSkill_TempSkillLevel[ strSkillID ]
	
	if true == bUp then
	
		-- 0 레벨 오프셋
		local nLevelOffset = 0
		if 0 == tbSkill[ "wLevel" ] then
			nLevelOffset = 1
		end
		
		if nil == nLevel then
		
			if 0 == tbSkill[ "wLevel" ] then
			
				-- 0원 세팅시 널로인식한다;
				local nReqMoney = 0
				if tbSkill[ "dwMoney" ] ~= nil then
					nReqMoney = tbSkill[ "dwMoney" ]
				end
				nLevel = 0
				-- 0 레벨 스킬은 배우는데 필요한 금액 증감
				g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney + nReqMoney
				
			else
			
				nLevel = 1
				
				local nReqMoney = 0
				if tbSkill[ "dwMoneyLevelList" ][nLevel] ~= nil then
					nReqMoney = tbSkill[ "dwMoneyLevelList" ][nLevel]
				end

				g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney + nReqMoney
		
				
			end
			
		else
		
			nLevel = nLevel + 1
			
			local nReqMoney = 0
			if tbSkill[ "dwMoneyLevelList" ][nLevel] ~= nil then
				nReqMoney = tbSkill[ "dwMoneyLevelList" ][nLevel]
			end

			g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney + nReqMoney
		
		end
		
		-- 사용 포인트 계산
		local nUsingPoint = 0
		if 0 == tbSkill[ "wLevel" ] then
		
			nUsingPoint = g_MyChar:GetNeedSkillPoint( tbSkill[ "dwID" ], tbSkill[ "wLevel" ] + nLevel )
			g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] = 
				g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] + nUsingPoint
			
		else
		
			nUsingPoint = g_MyChar:GetNeedSkillPoint( tbSkill[ "dwID" ], tbSkill[ "wLevel" ] + nLevel - 1 )
			g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] = 
				g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] + nUsingPoint
				
		end
		
		if nil == g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] then
		
			g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = nUsingPoint
		
		else
		
			g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = 
				g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] + nUsingPoint
		
		end
		
		-- 탭 텍스트 사용 포인트 업데이트
		Skill_UpdateTabText()
		
		-- 스킬 레벨을 올릴 때 레벨 올린 순서대로 보내야한다.
		table.insert( g_tbSkill_LevelUpSkill, tbSkill[ "dwID" ] )
		
	else
	
		if nil == nLevel then
			return false;
		end
		
		-- 0 레벨 오프셋
		local nLevelOffset = 0
		if 0 == tbSkill[ "wLevel" ] then
			nLevelOffset = 1
		end
		
		-- 해당 스킬을 선행 스킬로 갖는 스킬이 있는지 확인
		local nNeedSkillLevel = Skill_IsPreSkillInTempSkill( tbSkill )
		if g_tbSkill_TempSkillList[ strSkillID ][ "wLevel" ] == nNeedSkillLevel then
			return  false;
		end
		
		local nReqMoney = 0
		if tbSkill[ "dwMoneyLevelList" ][nLevel] ~= nil then
			nReqMoney = tbSkill[ "dwMoneyLevelList" ][nLevel]
		end
		g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney - nReqMoney	
		
		nLevel = nLevel - 1
				
		-- 사용 포인트 계산
		local nUsingPoint = g_MyChar:GetNeedSkillPoint( tbSkill[ "dwID" ], tbSkill[ "wLevel" ] + nLevel + nLevelOffset )
		g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] = 
			g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] - nUsingPoint
			
		if nil ~= g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] then
		
			g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = 
				g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] - nUsingPoint
				
			if 0 >= g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] then
				g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = nil
			end
		
		end
		
		-- 스킬 레벨을 올릴 때 레벨 올린 순서대로 보내야한다.
		-- 테이블 맨 뒤에서부터 찾아 삭제한다.
		for i = table.getn( g_tbSkill_LevelUpSkill ), 1, -1 do
		
			if g_tbSkill_LevelUpSkill[ i ] == tbSkill[ "dwID" ] then
			
				table.remove( g_tbSkill_LevelUpSkill, i )
				break
			
			end
		
		end
		
		-- 탭 텍스트 사용 포인트 업데이트
		Skill_UpdateTabText()

		-- 원래 스킬 레벨로 돌아갔다면 테이블에서 제거, - 버튼 비활성
		if 0 >= nLevel + nLevelOffset then
		
			g_tbSkill_TempSkillLevel[ strSkillID ] = nil
			g_tbSkill_TempSkillList[ strSkillID ] = nil
			
			-- -- 0 레벨 스킬은 배우는데 필요한 금액 차감
			-- if 0 == tbSkill[ "wLevel" ] then
				-- -- 0원 세팅시 널로인식한다;
				-- local nReqMoney = 0
				-- if tbSkill[ "dwMoneyLevelList" ][nLevel] ~= nil then
					-- nReqMoney = tbSkill[ "dwMoneyLevelList" ][nLevel]
				-- end
				-- g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney - nReqMoney
			-- end
			
			local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )	
			Skill_UpdateList()
			Skill_UpdateListState( nScrollPos )
			Skill_UpdateSkillPoint()
			
			return false;
			
		--else
		end
			
	end
	
	-- 레벨 올린 스킬 정보 (현재 레벨이 아닌 다음 레벨의 정보가 필요하다.)
	local tbInvestSkill = g_MyChar:GetSkillInvest( tbSkill[ "dwID" ], nLevel + 1 )
	if nil == tbInvestSkill[ "dwID" ] then
		return  false;
	end

	g_tbSkill_TempSkillLevel[ strSkillID ] = nLevel
	g_tbSkill_TempSkillList[ strSkillID ] = tbInvestSkill
	
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )	
	Skill_UpdateList()
	Skill_UpdateListState( nScrollPos )
	Skill_UpdateSkillPoint()
	
	-- 해당 변수로 인하여 에러가 발생함
	--local nSlotIndex = nScrollPos * SKILL_SLOT_COUNT_COL + ( nIndex % SKILL_SLOT_COUNT_COL ) - 1

	SetSlot_SkillLevel( SLOT_SKILL_ID .. tostring( nIndex ), tbInvestSkill[ "dwID" ], tbInvestSkill[ "wLevel" ] - 1 )
	
	-- 다음스킬레벨 보기 툴팁 갱신
	if tbInvestSkill[ "wLevel" ] < tbSkill[ "dwMaxLevel" ] and bUp == true then
	
		local strSkillID_ToolTip = SplitNativeID( tostring( tbSkill[ "dwID" ] ) )
		--SkillLvPlusToolTip_Open( strSkillID_ToolTip[ 1 ], strSkillID_ToolTip[ 2 ], tbInvestSkill[ "wLevel" ] )
		
	end
	
	local tbSkillID = SplitNativeID( tostring( tbSkill[ "dwID" ] ) )
	local strItemInst = "item" .. tostring( nIndex ) .. ".mcNextLevelOver"
	UnregShowTooltipEnableState( WIDGET_ID_SKILL, 0, strItemInst )
	if bUp == true then
		RegShowTooltipEnableState( WIDGET_ID_SKILL, 0, strItemInst, tbSkillID[ 1 ], tbSkillID[ 2 ], tbInvestSkill[ "wLevel" ], true )
	end
	
	return true;
	
end

-- 포인트 투자한 스킬 중 해당 스킬이 선행 스킬인지 확인하고 맞다면 요구 보유 스킬 레벨 반환
function Skill_IsPreSkillInTempSkill( tbSkill )

	local nRet = -1

	for value in pairs( g_tbSkill_TempSkillList ) do
	
		local tbTempSkill = g_tbSkill_TempSkillList[ value ]
		if tbSkill[ "dwID" ] == tbTempSkill[ "dwNeedSkillID" ] then
		
			if  nRet < tbTempSkill[ "dwNeedSkillLevel" ] then
				nRet = tbTempSkill[ "dwNeedSkillLevel" ]
			end
		
		end
	
	end
	
	return nRet

end

-- 요구 능력치 만족하는지 체크
function Skill_CheckStat( tbSkill )

	if nil == tbSkill then
		return false
	end

	-- 요구 능력치 하나라도 만족하면 되나?
	if 0 == tbSkill[ "bOnlyOneStat" ] then
		
		-- 힘
		if tbSkill[ "wPow" ] > g_MyChar:GetPow_Skill() then
			return false
		end
		
		-- 민첩
		if tbSkill[ "wDex" ] > g_MyChar:GetDex_Skill() then
			return false
		end
		
		-- 체력
		if tbSkill[ "wStr" ] > g_MyChar:GetStr_Skill() then
			return false
		end
		
		-- 정신
		if tbSkill[ "wSpi" ] > g_MyChar:GetSpi_Skill() then
			return false
		end
		
		-- 근력
		if tbSkill[ "wSta" ] > g_MyChar:GetSta_Skill() then
			return false
		end
	
	else
	
	
		if  tbSkill[ "wPow" ] ~= 0 and tbSkill[ "wPow" ] <= g_MyChar:GetPow_Skill() then
			PrintDebug( "wPow" )
			return true
		end
	
		if  tbSkill[ "wDex" ] ~= 0 and tbSkill[ "wDex" ] <= g_MyChar:GetDex_Skill() then
			PrintDebug( "wDex" )
			return true
		end
	
		if  tbSkill[ "wStr" ] ~= 0 and tbSkill[ "wPow" ] <= g_MyChar:GetStr_Skill() then
			PrintDebug( "wStr" )
			return true
		end
		
		if  tbSkill[ "wSpi" ] ~= 0 and tbSkill[ "wPow" ] <= g_MyChar:GetSpi_Skill() then
			PrintDebug( "wSpi" )
			return true
		end
		
		if  tbSkill[ "wSta" ] ~= 0 and tbSkill[ "wSta" ] <= g_MyChar:GetSta_Skill() then
			PrintDebug( "wSta" )
			return true	
		end
		
		return false
		
	end
	
	return true

end

-- 스킬 습득 요구 머니 체크
function Skill_CheckMoney( tbSkill )

	if nil == tbSkill then
		return false
	end
	
	-- 0레벨 아닌건 체크할 필요 없다.
	--if 0 ~= tbSkill[ "wLevel" ] then
	--	return true
	--end
	
	-- 0원 세팅시 널로인식한다;
	local nReqMoney = 0
	if tbSkill[ "dwMoney" ] ~= nil then
		nReqMoney = tonumber(tbSkill[ "dwMoney" ])
	end
	if g_MyChar:GetInvenMoney() < g_nSkill_SkillUpMoney + nReqMoney then
		return false
	end
	
	return true

end

-- 스킬 포인트 초기화 버튼 툴팁 생성
function Skill_GetResetTooltipText()

	local strTabData = ""
	local strID = g_tbTabStringID[ g_MyChar.nClassIndex + 1 ]
	
	strTabData = "'" .. GetGameWordText( strID, g_nSkill_SelectPage ) .. "' "
	strTabData = strTabData .. GetGameInternalText( "RNSKILLWINDOW_RESET_TOOLTIP", 0 )
	
	return strTabData
	
end

-- 스킬 포인트 적용 툴팁 생성
function Skill_GetApplyTooltipText()

	local strTabData = ""
	for i = 1, g_nSkill_MaxTabIndex, 1 do		
	
		local strID = g_tbTabStringID[ g_MyChar.nClassIndex + 1 ]
		strTabData = strTabData .. "'" .. GetGameWordText( strID, i - 1 ) .. "', "	
		
	end
	
	strTabData = strTabData .. GetGameInternalText( "RNSKILLWINDOW_CONFIRM_TOOLTIP", 0 )
	
	return strTabData
	
end

-- 스킬 스텟, 포인트 초기화 툴팁 
function Skill_GetResetStat_Skill_TooltipText()

	local strTabData = ""
	
	strTabData = GetGameInternalText( "RNSKILLWINDOW_SKILL_STAT_RESET_TOOLTIP", 0 )
	strTabData = StringFormat( strTabData, ( g_MyChar:GetLearnedSkillCost() ) )
	
	return strTabData

end

-- 배운 스킬 레벨
function Skill_GetLearnSkillLevel( nID )

	for i = 1, g_nSkill_MaxTabIndex, 1 do
	
		local tbSkillList = g_tbSkill_AllList[ i ]
		for value in pairs( tbSkillList ) do
		
			local tbSkill = tbSkillList[ value ]
			if tbSkill[ "dwID" ] == nID then
				return tbSkill[ "wLevel" ]
			end
		
		end
	
	end
	
	return -1

end

------------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 캐릭터 스킬 리스트 업데이트
function Skill_UpdateAllList()

	-- 투자 포인트 리셋
	Skill_ResetInvestPoint()

	-- 스킬 리스트 업데이트
	g_tbSkill_AllList = {}
	for i = 1, g_nSkill_MaxTabIndex, 1 do
		table.insert( g_tbSkill_AllList, g_MyChar:GetSkillList( i - 1 ) )
	end
	
	if false == IsOpenWidget( WIDGET_ID_SKILL ) then 
		return
	end
	
	-- 현재 스크롤 위치
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )
	
	Skill_UpdateTabText()
	Skill_UpdateList()
	Skill_UpdateListState( nScrollPos )
	Skill_UpdateSkillPoint()
	
	SetString( WIDGET_ID_SKILL, "TooltipText", Skill_GetResetStat_Skill_TooltipText(), "btnStat_SkillReset" )

end

-- 캐릭터 스킬 포인트 업데이트
function Skill_UpdatePoint()

	for i = 1, g_nSkill_MaxTabIndex, 1 do
		g_tbSkill_TempSkillPoint[ i ] = 0
	end
	
	if false == IsOpenWidget( WIDGET_ID_SKILL ) then
		return
	end
	
	Skill_UpdateSkillPoint()

end

-- 캐릭터 장비 변경
function Skill_UpdateListAndListState()

	if false == IsLoadWidget( WIDGET_ID_SKILL ) then
		return
	end

	Skill_UpdateList()
	Skill_UpdateListState( GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" ) )

end

------------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탭 변경 이벤트
function Ev_Skill_ChangeTabIndex( nIndex )

	g_nSkill_SelectPage = nIndex
	
	-- 현재 스크롤 위치
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )
	
	Skill_UpdateList()
	Skill_UpdateListState( nScrollPos )
	Skill_UpdateSkillPoint()
	
	-- 툴팁 갱신
	SetString( WIDGET_ID_SKILL, "TooltipText", Skill_GetResetTooltipText(), "btnReset" )
	
end

-- 리스트 스크롤 이벤트
function Ev_Skill_ScrollList()

	-- 현재 스크롤 위치
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )
	
	Skill_UpdateListState( nScrollPos )

end

-- 포인트 초기화 버튼 이벤트
function Ev_Skill_btnReset()

	g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] = 0
	
	local tbSkillList = g_tbSkill_AllList[ g_nSkill_SelectPage + 1 ]
	for value in pairs( tbSkillList ) do
	
		local tbSkill = tbSkillList[ value ]
		local strSkillID = tostring( tbSkill[ "dwID" ] )
		
		local nLevelUpCount = 0
		for value2 in pairs( g_tbSkill_LevelUpSkill ) do
		
			if g_tbSkill_LevelUpSkill[ value2 ] == tbSkill[ "dwID" ] then
			
				nLevelUpCount = nLevelUpCount + 1
				g_tbSkill_LevelUpSkill[ value2 ] = nil
			
			end
		
		end
		
		if nil ~= g_tbSkill_TempSkillLevel[ strSkillID ] then
		
			if 0 == nLevelUpCount - g_tbSkill_TempSkillLevel[ strSkillID ] - 1 then
			
				-- 0원 세팅시 널로인식한다;
				-- local nReqMoney = 0
				-- if tbSkill[ "dwMoney" ] ~= nil then
					-- nReqMoney = tbSkill[ "dwMoney" ]
				-- end
				g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney - g_nSkill_SkillUpMoney
			
			end
		
			g_tbSkill_TempSkillLevel[ strSkillID ] = nil
		
		end
		
		if nil ~= g_tbSkill_TempSkillList[ strSkillID ] then
		
			g_tbSkill_TempSkillList[ strSkillID ] = nil
		
		end
		
		if nil ~= g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] then
		
			g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = nil
		
		end
	
	end
	
	-- 현재 스크롤 위치
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )
	
	Skill_UpdateList()
	Skill_UpdateListState( nScrollPos )
	Skill_UpdateSkillPoint()
	Skill_UpdateTabText()

end

-- 포인트 적용 버튼 이벤트
function Ev_Skill_btnApply()

	local bEnd = true
	for value in pairs( g_tbSkill_TempSkillList ) do
		bEnd = false
		break
	end
	
	if true == bEnd then
		return
	end

	local str = StringFormat( GetGameInternalText( "RNSKILLWINDOW_CONFIRM", 0 ), CurrencyFormat( g_nSkill_SkillUpMoney ) )

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_Skill_ReqApplySkillPoint, Event_MsgBox_Close )

end

-- 포인트 적용 메시지 이벤트
function Ev_Skill_ReqApplySkillPoint()

	g_Msg:SkillLevelUp( g_tbSkill_LevelUpSkill )
	
	Skill_ResetInvestPoint()
	Skill_UpdateListAndListState()
	Skill_UpdateTabText()
	Skill_UpdateSkillPoint()
	
	Event_MsgBox_Close()

end

-- +, - 버튼 이벤트
function Ev_Skill_BtnUp( strIndex )

	local strItemInst = "item" .. tostring( strIndex ) .. "." .. "btnUp"
	local bEnabled = GetBool( WIDGET_ID_SKILL, "enabled", strItemInst )
	if bEnabled == false then
		return ;
	end	

	if g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() == true then

		Skill_Ctrl_BtnUp( strIndex )
	
		return ;
		
	else
		Skill_TempInvest( true, tonumber( strIndex ) )
	end
end

function Skill_Ctrl_BtnUp( strIndex )

	local tbSkill = Skill_FindSkill( tonumber(strIndex) )

	local strID = tostring(tbSkill[ "dwID" ]);
	
	local nCurrentLv = 0
	if g_tbSkill_TempSkillLevel[ strID ] ~= nil then
		nCurrentLv = g_tbSkill_TempSkillLevel[ strID ] + 1
	end
		
	local tbLearnCost = g_MyChar:GetLearnSkillMaxLevel( tbSkill[ "dwID" ], nCurrentLv  )
	local maxLevel = tbLearnCost["Level"]
		
	g_nSkill_SkillUpMoney = tonumber( tbLearnCost["ReserveMoney"] );
	
	g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] = 
		g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] + tbLearnCost["UsingSkillPoint"];
			
	-- 레벨 올린 스킬 정보 (현재 레벨이 아닌 다음 레벨의 정보가 필요하다.)
	local tbInvestSkill = g_MyChar:GetSkillInvest( tbSkill[ "dwID" ], maxLevel + 1 )
	if nil == tbInvestSkill[ "dwID" ] then
		return  ;
	end

	g_tbSkill_TempSkillLevel[ strID ] = maxLevel
	g_tbSkill_TempSkillList[ strID ] = tbInvestSkill
	
	Skill_UpdateTabText()
		
	local nIndex = 0
	if nCurrentLv ~= nil then
		nIndex = nIndex + nCurrentLv
	end
	
	if tbSkill[ "wLevel" ] ~= 0 then
		nIndex = nIndex + tbSkill[ "wLevel" ] + 1
		maxLevel = maxLevel + tbSkill[ "wLevel" ]
	end
	
	while nIndex <= maxLevel do
		
		table.insert( g_tbSkill_LevelUpSkill, tbSkill[ "dwID" ] )
		nIndex = nIndex + 1
			
	end
	
	if nil == g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] then
		
		g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = tbLearnCost["UsingSkillPoint"]
			
	else
		
		g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = 
		g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] + tbLearnCost["UsingSkillPoint"]
		
	end
	
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )	
	
	Skill_UpdateList()
	Skill_UpdateListState( nScrollPos )
	Skill_UpdateSkillPoint()

end


function Ev_Skill_BtnDown( strIndex )

	local strItemInst = "item" .. tostring( strIndex ) .. "." .. "btnDown"
	local bEnabled = GetBool( WIDGET_ID_SKILL, "enabled", strItemInst )
	if bEnabled == false then
		return ;
	end
	if g_STAGE_INPUT_EVENT_MANAGER.IsCtrlDown() == true then
	
		Skill_Ctrl_BtnDown( strIndex )
		-- while( Skill_TempInvest( false, tonumber( strIndex ) )) do
				
			-- local bEnabled = GetBool( WIDGET_ID_SKILL, "enabled", strItemInst )
			-- if bEnabled == false then
				-- return ;
			-- end	
				
		-- end
		
		return ;

	else
	
		Skill_TempInvest( false, tonumber( strIndex ) )
	
	end

end

function Skill_Ctrl_BtnDown( strIndex )

	local tbSkill = Skill_FindSkill( tonumber(strIndex) )

	local strID = tostring(tbSkill[ "dwID" ]);
	
	local nCurrentLv = 0
	
	if g_tbSkill_TempSkillLevel[ strID ] ~= nil then
		nCurrentLv = g_tbSkill_TempSkillLevel[ strID ] + 1
	end
		
	local tbLearnCost = g_MyChar:GetLearnSkillMinLevel( tbSkill[ "dwID" ], nCurrentLv  )
	if tbLearnCost == nil then
		return ;
	end
	
	local minLevel = tbLearnCost["Level"]
	g_nSkill_SkillUpMoney = g_nSkill_SkillUpMoney - tbLearnCost["ReserveMoney"];
		
	g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] = 
		g_tbSkill_TempSkillPoint[ g_nSkill_SelectPage + 1 ] - tbLearnCost["UsingSkillPoint"];
		
	-- 레벨 올린 스킬 정보 (현재 레벨이 아닌 다음 레벨의 정보가 필요하다.)
	local tbInvestSkill = g_MyChar:GetSkillInvest( tbSkill[ "dwID" ], minLevel  )
	if nil == tbInvestSkill[ "dwID" ] then
		return  ;
	end

	g_tbSkill_TempSkillLevel[ strID ] = minLevel
	g_tbSkill_TempSkillList[ strID ] = tbInvestSkill
	
	Skill_UpdateTabText()
		
	local nIndex = 0
	if nCurrentLv ~= nil then
		nIndex = nCurrentLv
	end
	
	-- 0 레벨 오프셋
	local nLevelOffset = 0
	if 0 == tbSkill[ "wLevel" ] then
		nLevelOffset = 1
	end
	
	for i = table.getn( g_tbSkill_LevelUpSkill ), 1, -1 do
		
		if g_tbSkill_LevelUpSkill[ i ] == tbSkill[ "dwID" ] then
			
			table.remove( g_tbSkill_LevelUpSkill, i )			
	
		end
		
	end
	
			
	g_tbSkill_TempSkillLevel[ strID ] = nil
	g_tbSkill_TempSkillList[ strID ] = nil
	
	g_tbSkill_TempSkillPointDetail[ tbSkill[ "dwID" ] ] = nil
			
	local nScrollPos = GetInt( WIDGET_ID_SKILL, "scrollPosition", "exList.list" )	
	Skill_UpdateList()
	Skill_UpdateListState( nScrollPos )
	Skill_UpdateSkillPoint()
	
	--Skill_TempInvest( false, tonumber( strIndex ) )

end

function Ev_CloseSkill()

	-- +,- 버튼 이벤트
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
	
		local strItemInst = "item" .. i .. "."
		
		CallFunction( WIDGET_ID_SKILL, "ResetAutoRepeat", "", strItemInst .. "btnUp" )
		CallFunction( WIDGET_ID_SKILL, "ResetAutoRepeat", "", strItemInst .. "btnDown" )
				
	end	

	Ev_Skill_BtnOut()
end

-- + 버튼 마우스 오버
function Ev_Skill_BtnOver( nStageX, nStageY, strIndex )

	local tbSkill = Skill_FindSkill( tonumber( strIndex ) )
	if nil == tbSkill then
		return
	end
	
	local tbSkillID = SplitNativeID( tostring( tbSkill[ "dwID" ] ) )
	local nLevel = tbSkill[ "wLevel" ]
	
	local nTempSkillLevel = g_tbSkill_TempSkillLevel[ tostring( tbSkill[ "dwID" ] ) ]
	if nil ~= nTempSkillLevel then
	
		if 0 == nLevel then
			nLevel = 1
		end
		
		nLevel = nLevel + nTempSkillLevel
		
	end
	
	if tbSkill[ "dwMaxLevel" ] <= nLevel then
		return
	end

	
	local tbInvestSkill = g_MyChar:GetSkillInvest( tbSkill[ "dwID" ], nLevel )
	
	local strItemInst = "item" .. strIndex .. "." .. "btnUp"
	g_SkillToolTip_PlusButton = true
	
	RegShowTooltipEnableState( WIDGET_ID_SKILL, 0, strItemInst, tbSkillID[ 1 ], tbSkillID[ 2 ], nLevel, true )
	
end

-- + 버튼 마우스 아웃
function Ev_Skill_BtnOut()

	-- 멀티스킬과 링크스킬창 까지 모두 닫아주어야 한다.
	CloseWidget( WIDGET_ID_SKILLLVPLUSINFO )
	
	g_SkillToolTip_PlusButton = false
	
end

-- 헬프 버튼(?) 클릭
function Event_Skill_Help()

	local str = GetGameInternalText( "RNSKILL_TOOLTIP_HELP_TEXT", 0 )
	HelpTooltip_SetText( str )
	
end

function Ev_Stat_Skill_btnReset()

	local strMsg = StringFormat( GetGameInternalText( "EMREQ_RESET_SKST_FB_OK_MONEY", 0 ), ( g_MyChar:GetLearnedSkillCost() ))
	
	PrintDebug(strMsg)
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Stat_Skill_Reset_OK, Event_MsgBox_Close )

end

function Ev_Stat_Skill_Reset_OK()
	
	g_MyChar:ResetSkillStatsBySkillUI()
	
	Event_MsgBox_Close()

end

