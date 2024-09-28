
-- ����Ʈ �������� ��� ����(������ ������ �����ؾ� �Ѵ�, �ϴ� ž��ũ ǥ��������� ����)
REWORD_MAX_ITEMLIST = 13

-- ������ ���� �ð� ����( Ÿ�̸� �Լ� ȣ��� ī��Ʈ)
g_Reward_nWindowClosingTime = 0

-- ��� ����
g_tbReward_RewardInfo = {}

function LoadPVEReward()

	SetBool( WIDGET_ID_PVEREWARD, "visible", false, "closeBtn" )

	-- ����
	local str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 0 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Rank" )
	-- �п�
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 1 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.School" )
	-- Ŭ����
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 2 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Class" )
	-- ĳ���͸�
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 3 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.CharacterName" )
	-- ���ط�
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 9 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Damage" )
	-- ġ����
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 6 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Heal" )
	-- ���ʽ� ����
	str = GetGameWordText( "COMPETITION_PVE", 20 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Bonus" )
	-- ������
	str = GetGameWordText( "CTF_RESULT_BUTTON", 4 )
	SetString( WIDGET_ID_PVEREWARD, "label", str, "Info.btnCalcel" )
	SetEvent( WIDGET_ID_PVEREWARD, EVENT_BUTTON_CLICK, "Ev_PVEReward_Exit", "", "Info.btnCalcel" )
	
end

-- ����
function OpenPVEReward()

	PVEReward_SetTimeLabel()
	PVEReward_Update()

end

-- ���� ����
function PVEReward_Update()

	PVEReward_DisplayInfo()
	PVEReward_SetTimeLabel()
	
end

-- ����Ʈ ����
function PVEReward_DisplayInfo()

	if nil == g_tbReward_RewardInfo then
		return
	end

	-- ��ũ ������ ��� ���̺�
	local tbRankList = {}
	for i = 1, REWORD_MAX_ITEMLIST, 1 do
	
		local tbData = g_tbReward_RewardInfo[ i ]
		if nil == tbData then
		
			CallFunction( WIDGET_ID_PVEREWARD, "gotoAndStop", "none", "Info.item" .. i )
			CallFunction( WIDGET_ID_PVEREWARD, "gotoAndStop", "none", "Info.item" .. i .. ".mcClass" )
		
		else
		
			-- ��ũ �ؽ�Ʈ --------------------
			local strRank = "-"
			if true == tbData[ "bMVP" ] then
				
				strRank = HTMLFontColor( "MVP", HTML_TEXT_COLOR_GREEN )
				
			end
			-----------------------------------
		
			-- Ŭ���� �̹��� ------------------
			strFrameID = PVEReward_ClassIndexToImgString( tbData[ "emCLASS" ] )
			CallFunction( WIDGET_ID_PVEREWARD, "gotoAndStop", strFrameID, "Info.item" .. i .. ".mcClass" )
			----------------------------------
			
			-- ġ���� --------------------------
			local strHeal = ""
			if 0 == tbData[ "nHEAL" ] then
				strHeal = "-"
			else
				strHeal = tostring( tbData[ "nHEAL" ] )
			end
			-----------------------------------
			
			-- ���� �ؽ�Ʈ ---------------------
			local strReward = " "
			if true == tbData[ "bExp" ] then
			
				strReward = "+Exp"
				
			end
			
			if true == tbData[ "bMoney" ] then
			
				-- �տ� exp �پ������� �޸� ���̱�
				if true == tbData[ "bExp" ] then
					strReward = strReward .. ", "
				end
				
				strReward = strReward .. "+Money"
				
			end
			-----------------------------------
			
			-- ��ũ ����
			local tb =
			{
				strRank,
				tbData[ "strNAME" ],
				tbData[ "nDAMAGE" ],
				strHeal,
				HTMLFontColor( strReward, HTML_TEXT_COLOR_GREEN ),
			}
			table.insert( tbRankList, tb )
		
		end
	
	end
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4", "label5" )
	SetDataEx( WIDGET_ID_PVEREWARD, strParam, tbRankList, "Info.List" )	
	
end

-- <����/���������� ����ð�> ���̺� ���� 
function PVEReward_SetTimeLabel()
	
	-- ��� 
	local strResult = GetGameWordText( "CTF_RESULT_TEXT", 0 ) .. " " .. 
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), 0, g_Reward_nWindowClosingTime ) .. ">"
	SetString( WIDGET_ID_PVEREWARD, "text", strResult, "Info.labelCloseWindow" )
	
	-- Ŭ���̾�Ʈ ����ð� Ÿ�̸� ����
	if 0 >= g_Reward_nWindowClosingTime then
		return
	else
	
		-- Sec �ʴ����� ����
		g_Reward_nWindowClosingTime = g_Reward_nWindowClosingTime - 1
		
	end
	
	-- Ÿ�̸� ȣ��
	local tbQuickSet_SetTimeLabel = 
	{
		time = 1000,			
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "PVEReward_SetTimeLabel"
	} 
	SetTween( WIDGET_ID_PVEREWARD, tbQuickSet_SetTimeLabel, "", "", "" )
	
end

-- Ŭ���� �̹��� �̸� ��ȯ
function PVEReward_ClassIndexToImgString( nClass )

	-- �ҽ�
	local strClassImgName = "NONE"
	if CHARCLASS_IDX_FIGHTER_M == nClass then			-- ������ ��
		strClassImgName = "FIGHTER_M"
	elseif CHARCLASS_IDX_ARMS_M == nClass then			-- �˵��� ��
		strClassImgName = "ARMS_M"                   
	elseif CHARCLASS_IDX_ARCHER_W == nClass then		-- ��ú� ��
		strClassImgName = "ARCHER_W"                     
	elseif CHARCLASS_IDX_SPIRIT_W == nClass then		-- �⿹�� ��
		strClassImgName = "SPIRIT_W"                     
	elseif CHARCLASS_IDX_EXTREAM_M == nClass then		-- �ذ��� ��
		strClassImgName = "EXTREME_M"                    
	elseif CHARCLASS_IDX_EXTREAM_W == nClass then		-- �ذ��� ��
		strClassImgName = "EXTREME_W"                    
	elseif CHARCLASS_IDX_FIGHTER_W == nClass then		-- ������ ��
		strClassImgName = "FIGHTER_W"                     
	elseif CHARCLASS_IDX_ARMS_W == nClass then			-- �˵��� ��
		strClassImgName = "ARMS_W"                   
	elseif CHARCLASS_IDX_ARCHER_M == nClass then		-- ��ú� ��
		strClassImgName = "ARCHER_M"                     
	elseif CHARCLASS_IDX_SPIRIT_M == nClass then		-- �⿹�� ��
		strClassImgName = "SPIRIT_M"
	elseif CHARCLASS_IDX_SCIENTIST_M == nClass then		-- ���к� ��
		strClassImgName = "SCIENTIST_M"
	elseif CHARCLASS_IDX_SCIENTIST_W == nClass then		-- ���к� ��
		strClassImgName = "SCIENTIST_W"
	elseif CHARCLASS_IDX_ASSASSIN_M == nClass then		-- �μ��� ��
		strClassImgName = "ASSASSIN_M"                   
	elseif CHARCLASS_IDX_ASSASSIN_W == nClass then		-- �μ��� ��
		strClassImgName = "ASSASSIN_W"                   
	elseif CHARCLASS_IDX_TRICKER_M == nClass then		-- ������ ��
		strClassImgName = "MAGICIAN_M"                        
	elseif CHARCLASS_IDX_TRICKER_W == nClass then		-- ������ ��
		strClassImgName = "MAGICIAN_W"
	elseif CHARCLASS_IDX_ACTOR_M == nClass then			-- ���غ� ��
		strClassImgName = "ActorM"
	elseif CHARCLASS_IDX_ACTOR_W == nClass then			-- ���غ� ��
		strClassImgName = "ActorF"
	else
		strClassImgName = "NONE"
	end

	return strClassImgName

end

---------------------------------------------------------------------------------------
-- Ŭ���̾�Ʈ ��

-- UI ���� ȣ��
function PVEReward_OpenWidget( nCloseTime )
	
	g_Reward_nWindowClosingTime = nCloseTime
	g_tbReward_RewardInfo = g_PVEClient:GetResult()
	
	if true == IsOpenWidget( WIDGET_ID_PVEREWARD ) then
	
		PVEReward_Update()
		
	else
	
		OpenWidget( WIDGET_ID_PVEREWARD )
		
	end
	
end

---------------------------------------------------------------------------------------
-- �̺�Ʈ

function Ev_PVEReward_Exit()

	g_Msg:ReqPVEExit()
	CloseWidget( WIDGET_ID_PVEREWARD )
	
end