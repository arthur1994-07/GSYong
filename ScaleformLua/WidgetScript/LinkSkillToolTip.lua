
-- ��� �ּ� height
SKILLLINKINFO_HEADER_MIN_HEIGHT = 40

-- ���/���� ����
g_SkillLink_HeaderInfoGap = 0

g_SkillLink_Mid = 0		-- ��Ƽ��ų MID �Է°�
g_SkillLink_Sid = 0		-- ��Ƽ��ų SID �Է°�
g_SkillLink_Level = 0	-- ��Ƽ��ų SkillLevel �Է°�


-- ��ų ��ũ ���� UI �ε� �Ϸ�
function LoadLinkSkillToolTip()
	
	-- ���� ���콺 ��� ����
	SetBool( WIDGET_ID_SKILLLINKINFO, "mouseEnabled", false, "slot" )
	
	-- ���� ���ε�
	SetString( WIDGET_ID_SKILLLINKINFO, "binding", SLOT_LINKSKILLTOOLTIP_ID, "slot" )	
	
	g_SkillLink_HeaderInfoGap = GetInt( WIDGET_ID_SKILLLINKINFO, "y", "tfInfo" ) - ( 
		GetInt( WIDGET_ID_SKILLLINKINFO, "y", "tfHeader" ) + 
		GetInt( WIDGET_ID_SKILLLINKINFO, "height", "tfHeader" ) )
	
end

-- ��ų ��ũ ���� ����
function OpenLinkSkillToolTip()
	-- ��� ���� ��������
	local GetHeader = GenerateSkillInfoHeader( g_SkillLink_Mid, g_SkillLink_Sid, g_SkillLink_Level )
	-- ���� ���� ��������
	local GetInfo = GenerateSkillInfo( g_SkillLink_Mid, g_SkillLink_Sid, g_SkillLink_Level )	
	
	-- ���� ������ ���
	SetSlot_Skill( SLOT_LINKSKILLTOOLTIP_ID, MakeNativeID( g_SkillLink_Mid, g_SkillLink_Sid ) )
	--SetSlot_Skill( SLOT_LINKSKILLTOOLTIP_ID, MakeNativeID( g_SkillLink_Mid, g_SkillLink_Sid ) )
	
	-- ��ų ���� �ؽ�Ʈ
	SetString( WIDGET_ID_SKILLLINKINFO, "htmlText", GetHeader, "tfHeader")
	SetString( WIDGET_ID_SKILLLINKINFO, "htmlText", GetInfo, "tfInfo")
	
	-- ��ų ��ǥ �� �ʺ� ����
	SkillLinkInfo_SetPos()
end

-- ��ų ��ǥ �� �ʺ� ����
function SkillLinkInfo_SetPos()	

	
	local nTextHeight = 0
	
	------------------------------------------------------------
	-- ��� height ����
	local nNewHeaderHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLINKINFO, "textHeight", "tfHeader" )
	if nTextHeight < SKILLLINKINFO_HEADER_MIN_HEIGHT then
		nNewHeaderHeight = SKILLLINKINFO_HEADER_MIN_HEIGHT + 10
	else
		nNewHeaderHeight = nTextHeight + 10
	end
	
	SetInt( WIDGET_ID_SKILLLINKINFO, "height", nNewHeaderHeight, "tfHeader" )
	
	------------------------------------------------------------
	-- ���� y ����
	local nNewInfoY = GetInt( WIDGET_ID_SKILLLINKINFO, "y", "tfHeader" ) + nNewHeaderHeight + g_SkillLink_HeaderInfoGap
	SetInt( WIDGET_ID_SKILLLINKINFO, "y", nNewInfoY, "tfInfo" )
	
	------------------------------------------------------------
	-- ���� height ����
	local nNewInfoHeight = 0
	
	nTextHeight = GetInt( WIDGET_ID_SKILLLINKINFO, "textHeight", "tfInfo" )
	nNewInfoHeight = nTextHeight + 10
	
	SetInt( WIDGET_ID_SKILLLINKINFO, "height", nNewInfoHeight, "tfInfo" )
	
	------------------------------------------------------------
	-- �߰� ���� ���� �� ��� ������ ����	
	local nNewBgHeight = nNewHeaderHeight + nNewInfoHeight + 10	
	
	-- ��� height ����
	SetInt( WIDGET_ID_SKILLLINKINFO, "height", nNewBgHeight, "bg" )
	
	-- UI ��ġ ���
	SetInt( WIDGET_ID_SKILLLINKINFO, "x", 100, "" )
	SetInt( WIDGET_ID_SKILLLINKINFO, "y", 320, "" )
	
end
-- ��ũ��ų ȣ��
function CallLinkSkill()

	if false == IsOpenWidget( WIDGET_ID_SKILLLINKINFO ) then
		OpenWidget( WIDGET_ID_SKILLLINKINFO )
	else
		OpenLinkSkillToolTip()
	end		
end