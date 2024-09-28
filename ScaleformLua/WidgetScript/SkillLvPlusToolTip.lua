
-- 헤더 최소 height
SKILLLVPLUSINFO_HEADER_MIN_HEIGHT = 40

-- 타입 최소 height
SKILLLVPLUSINFO_TYPE_MIN_HEIGHT = 20

-- 헤더/인포 간격
g_SkillLVPlus_HeaderInfoGap = 0

g_nSkillLVPlus_MID = -1
g_nSkillLVPlus_SID = -1
g_nSkillLVPlus_Level = 0
g_nSkillLVPlus_PosX = 0
g_nSkillLVPlus_PosY = 0

function LoadSkillLvPlusToolTip()
	
	-- 툴팁 히트테스트 끄기
	HitTestEnable( WIDGET_ID_SKILLLVPLUSINFO, false, "" )
	
	-- 슬롯 바인딩
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "binding", SLOT_SKILLLVPLUSTOOLTIP_ID, "slot" )	
	SetBool( WIDGET_ID_SKILLLVPLUSINFO, "tooltipEnabled", false, "slot" )	
	
	-- 툴팁 타입 출력
	local tfType = HTMLFontColor( GetGameWordText( "SKILL_NEXTLEVEL" ), HTML_TEXT_COLOR_SKYBLUE)
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "htmlText", tfType, "tfType" )
	
	g_SkillLVPlus_HeaderInfoGap = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfInfo" ) - ( 
		GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", "tfHeader" ) +
		GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfType" ) + 
		GetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", "tfType" ) )
	
end

function OpenSkillLvPlusToolTip()

	if false == g_SkillToolTip_PlusButton then
	
		CloseWidget( WIDGET_ID_SKILLLVPLUSINFO )
		return
		
	end

	SkillLvPlusToolTip_Update()

end

function SkillLvPlusToolTip_Open( nMID, nSID, nLEVEL, nPosX, nPosY )

	g_nSkillLVPlus_MID = nMID
	g_nSkillLVPlus_SID = nSID
	g_nSkillLVPlus_Level = nLEVEL
	
	if nil ~= nPosX then
		g_nSkillLVPlus_PosX = nPosX
	end
	
	if nil ~= nPosY then
		g_nSkillLVPlus_PosY = nPosY
	end

	if false == IsOpenWidget( WIDGET_ID_SKILLLVPLUSINFO ) then
	
		OpenWidget( WIDGET_ID_SKILLLVPLUSINFO )
		return
		
	end
	
	SkillLvPlusToolTip_Update()
	
end

-- 
function SkillLvPlusToolTip_Update()

	-- 헤더 정보 가져오기
	local strHeader = GenerateSkillInfoHeader( g_nSkillLVPlus_MID, g_nSkillLVPlus_SID, g_nSkillLVPlus_Level )
	-- 내용 정보 가져오기
	local strInfo = GenerateSkillInfo( g_nSkillLVPlus_MID, g_nSkillLVPlus_SID, g_nSkillLVPlus_Level )		
	-- 슬롯 아이템 등록
	SetSlot_Skill( SLOT_SKILLLVPLUSTOOLTIP_ID, MakeNativeID( g_nSkillLVPlus_MID, g_nSkillLVPlus_SID ) )
	
	-- 스킬 정보 텍스트
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "htmlText", strHeader, "tfHeader")
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "htmlText", strInfo, "tfInfo")
	
	-- 스킬 좌표 및 너비 설정
	if nil ~= g_nSkillLVPlus_PosX and nil ~= g_nSkillLVPlus_PosY then
		SkillLvPlusInfo_SetPos()
	end

end

-- 스킬 좌표 및 너비 설정
function SkillLvPlusInfo_SetPos()	

	local nTextHeight = 0
	
	------------------------------------------------------------
	-- 헤더 height 설정
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "textHeight", "tfHeader" )
	if nTextHeight < SKILLLVPLUSINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = SKILLLVPLUSINFO_HEADER_MIN_HEIGHT + SKILLLVPLUSINFO_TYPE_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + SKILLLVPLUSINFO_TYPE_MIN_HEIGHT + 10
	end
	
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", nNewHeaderHeight, "tfHeader" )
	
	------------------------------------------------------------
	-- 인포 y 설정
	local nNewInfoY = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfType" ) + nNewHeaderHeight + g_SkillLVPlus_HeaderInfoGap
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", nNewInfoY, "tfInfo" )
	
	------------------------------------------------------------
	-- 인포 height 설정
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "textHeight", "tfInfo" )
	nNewInfoHeight = nTextHeight + 15
	
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", nNewInfoHeight, "tfInfo" )
	
	------------------------------------------------------------
	-- 추가 슬롯 설정 및 배경 사이즈 설정	
	local nNewBgHeight = nNewHeaderHeight + nNewInfoHeight + 10	
	
	-- 배경 height 설정
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", nNewBgHeight, "bg" )
	
	-- 툴팁의 위치가 화면 밖으로 빠져나가는것을 방지하기 위한 문
	local nWidth = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "width", "")
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	local nPosX = g_nSkillLVPlus_PosX
	local nPosY = g_nSkillLVPlus_PosY
	
	if nPosX + nWidth > nScreenWidth then
		nPosX = nScreenWidth - nWidth
	end
	
	if nPosY + nNewBgHeight > nScreenHeight then
		nPosY = nScreenHeight - nNewBgHeight
	end
	
	-- UI 위치 잡기
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "x", nPosX, "" )
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", nPosY, "" )
	
end