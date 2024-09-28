
-- ��� �ּ� height
SKILLLVPLUSINFO_HEADER_MIN_HEIGHT = 40

-- Ÿ�� �ּ� height
SKILLLVPLUSINFO_TYPE_MIN_HEIGHT = 20

-- ���/���� ����
g_SkillLVPlus_HeaderInfoGap = 0

g_nSkillLVPlus_MID = -1
g_nSkillLVPlus_SID = -1
g_nSkillLVPlus_Level = 0
g_nSkillLVPlus_PosX = 0
g_nSkillLVPlus_PosY = 0

function LoadSkillLvPlusToolTip()
	
	-- ���� ��Ʈ�׽�Ʈ ����
	HitTestEnable( WIDGET_ID_SKILLLVPLUSINFO, false, "" )
	
	-- ���� ���ε�
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "binding", SLOT_SKILLLVPLUSTOOLTIP_ID, "slot" )	
	SetBool( WIDGET_ID_SKILLLVPLUSINFO, "tooltipEnabled", false, "slot" )	
	
	-- ���� Ÿ�� ���
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

	-- ��� ���� ��������
	local strHeader = GenerateSkillInfoHeader( g_nSkillLVPlus_MID, g_nSkillLVPlus_SID, g_nSkillLVPlus_Level )
	-- ���� ���� ��������
	local strInfo = GenerateSkillInfo( g_nSkillLVPlus_MID, g_nSkillLVPlus_SID, g_nSkillLVPlus_Level )		
	-- ���� ������ ���
	SetSlot_Skill( SLOT_SKILLLVPLUSTOOLTIP_ID, MakeNativeID( g_nSkillLVPlus_MID, g_nSkillLVPlus_SID ) )
	
	-- ��ų ���� �ؽ�Ʈ
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "htmlText", strHeader, "tfHeader")
	SetString( WIDGET_ID_SKILLLVPLUSINFO, "htmlText", strInfo, "tfInfo")
	
	-- ��ų ��ǥ �� �ʺ� ����
	if nil ~= g_nSkillLVPlus_PosX and nil ~= g_nSkillLVPlus_PosY then
		SkillLvPlusInfo_SetPos()
	end

end

-- ��ų ��ǥ �� �ʺ� ����
function SkillLvPlusInfo_SetPos()	

	local nTextHeight = 0
	
	------------------------------------------------------------
	-- ��� height ����
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "textHeight", "tfHeader" )
	if nTextHeight < SKILLLVPLUSINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = SKILLLVPLUSINFO_HEADER_MIN_HEIGHT + SKILLLVPLUSINFO_TYPE_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + SKILLLVPLUSINFO_TYPE_MIN_HEIGHT + 10
	end
	
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", nNewHeaderHeight, "tfHeader" )
	
	------------------------------------------------------------
	-- ���� y ����
	local nNewInfoY = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", "tfType" ) + nNewHeaderHeight + g_SkillLVPlus_HeaderInfoGap
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", nNewInfoY, "tfInfo" )
	
	------------------------------------------------------------
	-- ���� height ����
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLVPLUSINFO, "textHeight", "tfInfo" )
	nNewInfoHeight = nTextHeight + 15
	
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", nNewInfoHeight, "tfInfo" )
	
	------------------------------------------------------------
	-- �߰� ���� ���� �� ��� ������ ����	
	local nNewBgHeight = nNewHeaderHeight + nNewInfoHeight + 10	
	
	-- ��� height ����
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "height", nNewBgHeight, "bg" )
	
	-- ������ ��ġ�� ȭ�� ������ ���������°��� �����ϱ� ���� ��
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
	
	-- UI ��ġ ���
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "x", nPosX, "" )
	SetInt( WIDGET_ID_SKILLLVPLUSINFO, "y", nPosY, "" )
	
end