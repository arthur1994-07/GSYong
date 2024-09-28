
-- 리스트 한페이지 출력 개수(아이콘 개수랑 동일해야 한다, 하단 탑랭크 표기아이콘은 제외)
REWORD_MAX_ITEMLIST = 13

-- 윈도우 종료 시간 정보( 타이머 함수 호출용 카운트)
g_Reward_nWindowClosingTime = 0

-- 결과 정보
g_tbReward_RewardInfo = {}

function LoadPVEReward()

	SetBool( WIDGET_ID_PVEREWARD, "visible", false, "closeBtn" )

	-- 순위
	local str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 0 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Rank" )
	-- 학원
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 1 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.School" )
	-- 클래스
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 2 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Class" )
	-- 캐릭터명
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 3 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.CharacterName" )
	-- 피해량
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 9 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Damage" )
	-- 치유량
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 6 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Heal" )
	-- 보너스 보상
	str = GetGameWordText( "COMPETITION_PVE", 20 )
	SetString( WIDGET_ID_PVEREWARD, "text", str, "Info.title_label.Bonus" )
	-- 나가기
	str = GetGameWordText( "CTF_RESULT_BUTTON", 4 )
	SetString( WIDGET_ID_PVEREWARD, "label", str, "Info.btnCalcel" )
	SetEvent( WIDGET_ID_PVEREWARD, EVENT_BUTTON_CLICK, "Ev_PVEReward_Exit", "", "Info.btnCalcel" )
	
end

-- 오픈
function OpenPVEReward()

	PVEReward_SetTimeLabel()
	PVEReward_Update()

end

-- 정보 갱신
function PVEReward_Update()

	PVEReward_DisplayInfo()
	PVEReward_SetTimeLabel()
	
end

-- 리스트 설정
function PVEReward_DisplayInfo()

	if nil == g_tbReward_RewardInfo then
		return
	end

	-- 랭크 정보가 담길 테이블
	local tbRankList = {}
	for i = 1, REWORD_MAX_ITEMLIST, 1 do
	
		local tbData = g_tbReward_RewardInfo[ i ]
		if nil == tbData then
		
			CallFunction( WIDGET_ID_PVEREWARD, "gotoAndStop", "none", "Info.item" .. i )
			CallFunction( WIDGET_ID_PVEREWARD, "gotoAndStop", "none", "Info.item" .. i .. ".mcClass" )
		
		else
		
			-- 랭크 텍스트 --------------------
			local strRank = "-"
			if true == tbData[ "bMVP" ] then
				
				strRank = HTMLFontColor( "MVP", HTML_TEXT_COLOR_GREEN )
				
			end
			-----------------------------------
		
			-- 클래스 이미지 ------------------
			strFrameID = PVEReward_ClassIndexToImgString( tbData[ "emCLASS" ] )
			CallFunction( WIDGET_ID_PVEREWARD, "gotoAndStop", strFrameID, "Info.item" .. i .. ".mcClass" )
			----------------------------------
			
			-- 치유량 --------------------------
			local strHeal = ""
			if 0 == tbData[ "nHEAL" ] then
				strHeal = "-"
			else
				strHeal = tostring( tbData[ "nHEAL" ] )
			end
			-----------------------------------
			
			-- 보상 텍스트 ---------------------
			local strReward = " "
			if true == tbData[ "bExp" ] then
			
				strReward = "+Exp"
				
			end
			
			if true == tbData[ "bMoney" ] then
			
				-- 앞에 exp 붙어있으면 콤마 붙이기
				if true == tbData[ "bExp" ] then
					strReward = strReward .. ", "
				end
				
				strReward = strReward .. "+Money"
				
			end
			-----------------------------------
			
			-- 랭크 정보
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

-- <보상/강제나가기 적용시간> 레이블 설정 
function PVEReward_SetTimeLabel()
	
	-- 출력 
	local strResult = GetGameWordText( "CTF_RESULT_TEXT", 0 ) .. " " .. 
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), 0, g_Reward_nWindowClosingTime ) .. ">"
	SetString( WIDGET_ID_PVEREWARD, "text", strResult, "Info.labelCloseWindow" )
	
	-- 클라이언트 종료시간 타이머 설정
	if 0 >= g_Reward_nWindowClosingTime then
		return
	else
	
		-- Sec 초단위가 들어옴
		g_Reward_nWindowClosingTime = g_Reward_nWindowClosingTime - 1
		
	end
	
	-- 타이머 호출
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

-- 클래스 이미지 이름 반환
function PVEReward_ClassIndexToImgString( nClass )

	-- 소스
	local strClassImgName = "NONE"
	if CHARCLASS_IDX_FIGHTER_M == nClass then			-- 격투부 남
		strClassImgName = "FIGHTER_M"
	elseif CHARCLASS_IDX_ARMS_M == nClass then			-- 검도부 남
		strClassImgName = "ARMS_M"                   
	elseif CHARCLASS_IDX_ARCHER_W == nClass then		-- 양궁부 여
		strClassImgName = "ARCHER_W"                     
	elseif CHARCLASS_IDX_SPIRIT_W == nClass then		-- 기예부 여
		strClassImgName = "SPIRIT_W"                     
	elseif CHARCLASS_IDX_EXTREAM_M == nClass then		-- 극강부 남
		strClassImgName = "EXTREME_M"                    
	elseif CHARCLASS_IDX_EXTREAM_W == nClass then		-- 극강부 여
		strClassImgName = "EXTREME_W"                    
	elseif CHARCLASS_IDX_FIGHTER_W == nClass then		-- 격투부 여
		strClassImgName = "FIGHTER_W"                     
	elseif CHARCLASS_IDX_ARMS_W == nClass then			-- 검도부 여
		strClassImgName = "ARMS_W"                   
	elseif CHARCLASS_IDX_ARCHER_M == nClass then		-- 양궁부 남
		strClassImgName = "ARCHER_M"                     
	elseif CHARCLASS_IDX_SPIRIT_M == nClass then		-- 기예부 남
		strClassImgName = "SPIRIT_M"
	elseif CHARCLASS_IDX_SCIENTIST_M == nClass then		-- 과학부 남
		strClassImgName = "SCIENTIST_M"
	elseif CHARCLASS_IDX_SCIENTIST_W == nClass then		-- 과학부 여
		strClassImgName = "SCIENTIST_W"
	elseif CHARCLASS_IDX_ASSASSIN_M == nClass then		-- 인술부 남
		strClassImgName = "ASSASSIN_M"                   
	elseif CHARCLASS_IDX_ASSASSIN_W == nClass then		-- 인술부 여
		strClassImgName = "ASSASSIN_W"                   
	elseif CHARCLASS_IDX_TRICKER_M == nClass then		-- 마술부 남
		strClassImgName = "MAGICIAN_M"                        
	elseif CHARCLASS_IDX_TRICKER_W == nClass then		-- 마술부 여
		strClassImgName = "MAGICIAN_W"
	elseif CHARCLASS_IDX_ACTOR_M == nClass then			-- 연극부 남
		strClassImgName = "ActorM"
	elseif CHARCLASS_IDX_ACTOR_W == nClass then			-- 연극부 여
		strClassImgName = "ActorF"
	else
		strClassImgName = "NONE"
	end

	return strClassImgName

end

---------------------------------------------------------------------------------------
-- 클라이언트 콜

-- UI 열기 호출
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
-- 이벤트

function Ev_PVEReward_Exit()

	g_Msg:ReqPVEExit()
	CloseWidget( WIDGET_ID_PVEREWARD )
	
end