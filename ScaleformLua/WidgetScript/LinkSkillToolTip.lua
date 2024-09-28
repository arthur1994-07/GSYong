
-- 헤더 최소 height
SKILLLINKINFO_HEADER_MIN_HEIGHT = 40

-- 헤더/인포 간격
g_SkillLink_HeaderInfoGap = 0

g_SkillLink_Mid = 0		-- 멀티스킬 MID 입력값
g_SkillLink_Sid = 0		-- 멀티스킬 SID 입력값
g_SkillLink_Level = 0	-- 멀티스킬 SkillLevel 입력값


-- 스킬 링크 정보 UI 로드 완료
function LoadLinkSkillToolTip()
	
	-- 슬롯 마우스 사용 끄기
	SetBool( WIDGET_ID_SKILLLINKINFO, "mouseEnabled", false, "slot" )
	
	-- 슬롯 바인딩
	SetString( WIDGET_ID_SKILLLINKINFO, "binding", SLOT_LINKSKILLTOOLTIP_ID, "slot" )	
	
	g_SkillLink_HeaderInfoGap = GetInt( WIDGET_ID_SKILLLINKINFO, "y", "tfInfo" ) - ( 
		GetInt( WIDGET_ID_SKILLLINKINFO, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_SKILLLINKINFO, "height", "tfHeader" ) )
	
end

-- 스킬 링크 정보 오픈
function OpenLinkSkillToolTip()
	-- 헤더 정보 가져오기
	local GetHeader = GenerateSkillInfoHeader( g_SkillLink_Mid, g_SkillLink_Sid, g_SkillLink_Level )
	-- 내용 정보 가져오기
	local GetInfo = GenerateSkillInfo( g_SkillLink_Mid, g_SkillLink_Sid, g_SkillLink_Level )	
	
	-- 슬롯 아이템 등록
	SetSlot_Skill( SLOT_LINKSKILLTOOLTIP_ID, MakeNativeID( g_SkillLink_Mid, g_SkillLink_Sid ) )
	--SetSlot_Skill( SLOT_LINKSKILLTOOLTIP_ID, MakeNativeID( g_SkillLink_Mid, g_SkillLink_Sid ) )
	
	-- 스킬 정보 텍스트
	SetString( WIDGET_ID_SKILLLINKINFO, "htmlText", GetHeader, "tfHeader")
	SetString( WIDGET_ID_SKILLLINKINFO, "htmlText", GetInfo, "tfInfo")
	
	-- 스킬 좌표 및 너비 설정
	SkillLinkInfo_SetPos()
end

-- 스킬 좌표 및 너비 설정
function SkillLinkInfo_SetPos()	

	
	local nTextHeight = 0
	
	------------------------------------------------------------
	-- 헤더 height 설정
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLINKINFO, "textHeight", "tfHeader" )
	if nTextHeight < SKILLLINKINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = SKILLLINKINFO_HEADER_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + 10
	end
	
	SetInt( WIDGET_ID_SKILLLINKINFO, "height", nNewHeaderHeight, "tfHeader" )
	
	------------------------------------------------------------
	-- 인포 y 설정
	local nNewInfoY = GetInt( WIDGET_ID_SKILLLINKINFO, "y", "tfHeader" ) + nNewHeaderHeight + g_SkillLink_HeaderInfoGap
	SetInt( WIDGET_ID_SKILLLINKINFO, "y", nNewInfoY, "tfInfo" )
	
	------------------------------------------------------------
	-- 인포 height 설정
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLINKINFO, "textHeight", "tfInfo" )
	nNewInfoHeight = nTextHeight + 10
	
	SetInt( WIDGET_ID_SKILLLINKINFO, "height", nNewInfoHeight, "tfInfo" )
	
	------------------------------------------------------------
	-- 추가 슬롯 설정 및 배경 사이즈 설정	
	local nNewBgHeight = nNewHeaderHeight + nNewInfoHeight + 10	
	
	-- 배경 height 설정
	SetInt( WIDGET_ID_SKILLLINKINFO, "height", nNewBgHeight, "bg" )
	
	-- UI 위치 잡기
	SetInt( WIDGET_ID_SKILLLINKINFO, "x", 100, "" )
	SetInt( WIDGET_ID_SKILLLINKINFO, "y", 320, "" )
	
end
-- 링크스킬 호출
function CallLinkSkill()

	if false == IsOpenWidget( WIDGET_ID_SKILLLINKINFO ) then
		OpenWidget( WIDGET_ID_SKILLLINKINFO )
	else
		OpenLinkSkillToolTip()
	end		
end