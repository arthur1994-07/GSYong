
-- 리스트 개수
g_nCTFlagResult_ListCount = 16

-- 나가기 시간
g_nCTFlagResult_OutTime = 0
-- 타이틀 정보
g_tbCTFlagResult_Title = {}
-- 결과 정보 테이블
g_tbCTFlagResult_Info = {}
g_tbCTFlagResult_MyInfo = {}

-- 로드 완료
function LoadCTFlagResult()

	-- 닫기 버튼
	SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "closeBtn" )
	SetEvent( WIDGET_ID_CTFLAGRESULT, EVENT_WIDGET_CLOSE, "CloseCTFlagResult", "", "" )
	
	local str = ""
	
	-- 나가기
	str = GetGameWordText( "CTF_RESULT_BUTTON", 4 )
	SetString( WIDGET_ID_CTFLAGRESULT, "label", str, "btnOut" )
	SetEvent( WIDGET_ID_CTFLAGRESULT, EVENT_BUTTON_CLICK, "Ev_CTFlagResult_Out", "", "btnOut" )
	
	-- 리스트
	SetEvent( WIDGET_ID_CTFLAGRESULT, EVENT_SCROLL, "Ev_CTFlagResult_Scroll", "", "list" )
	
	-- 보상
	for i = 1, g_nCTFlagResult_ListCount, 1 do
	
		SetEvent( WIDGET_ID_CTFLAGRESULT, EVENT_MOUSE_ROLLOVER, "Ev_CTFlag_RewardRollOver", "", "btnHelp" .. i, i )
		SetEvent( WIDGET_ID_CTFLAGRESULT, EVENT_MOUSE_ROLLOUT, "Ev_CTFlag_RewardRollOut", "", "btnHelp" .. i )
	
	end

	-- 아이템
	str = GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 0 ) .. " :"
	SetString( WIDGET_ID_CTFLAGRESULT, "text", str, "mcReward.label" )
	
	HitTestEnable( WIDGET_ID_CTFLAGRESULT, false, "mcReward" )	
	for i = 1, CTFLAG_REWARD_SLOT_TOTAL, 1 do
	
		SetString( WIDGET_ID_CTFLAGRESULT, "binding", SLOT_CTFLAG_REWARD .. i, "mcReward.slot" .. i )
	
	end

end

-- 오픈
function OpenCTFlagResult()

	SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "mcReward" )

	-- 타이틀 설정
	CTFlagResult_SetTitleHeader()
	-- 리스트 설정
	CTFlagResult_SetList()
	-- 나가기 타이머
	CTFlagResult_SetTimer()

end

-- 클로즈
function CloseCTFlagResult()

	g_tbCTFlagResult_Title = {}
	g_tbCTFlagResult_Info = {}

end

-- 타이틀 설정
function CTFlagResult_SetTitleHeader()

	for value in pairs( g_tbCTFlagResult_Title ) do
	
		local strInst = "labelTitle" .. value
		SetString( WIDGET_ID_CTFLAGRESULT, "text", g_tbCTFlagResult_Title[ value ], strInst )
	
	end

end

-- 리스트 설정
function CTFlagResult_SetList()

	local tbDataList = {}
	for value in pairs( g_tbCTFlagResult_Info ) do
	
		local tbInfo = g_tbCTFlagResult_Info[ value ]
		
		local tbData = 
		{
			tbInfo[ 4 ],	-- 캐릭터 이름
			tbInfo[ 5 ],	-- 깃발 탈취 수
			tbInfo[ 6 ],	-- 사살 수
			tbInfo[ 7 ],	-- 사망 수
		}
		table.insert( tbDataList, tbData )
		
		-- 자기자신
		if ( tbInfo[ 8 ] == 1 ) then
		
			SetString( WIDGET_ID_CTFLAGRESULT, "text", tbInfo[ 4 ], "MyResult.textField1" )
			SetString( WIDGET_ID_CTFLAGRESULT, "text", tbInfo[ 5 ], "MyResult.textField2" )
			SetString( WIDGET_ID_CTFLAGRESULT, "text", tbInfo[ 6 ], "MyResult.textField3" )
			SetString( WIDGET_ID_CTFLAGRESULT, "text", tbInfo[ 7 ], "MyResult.textField4" )
			
		end
	
	end
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4" )
	SetDataEx( WIDGET_ID_CTFLAGRESULT, strParam, tbDataList, "list", false )
	
	SetInt( WIDGET_ID_CTFLAGRESULT, "scrollPosition", 0, "list" )
	Ev_CTFlagResult_Scroll()

end

-- 나가기 타이머
function CTFlagResult_SetTimer()

	if 0 > g_nCTFlagResult_OutTime then
		return
	end

	local nMin = math.floor( g_nCTFlagResult_OutTime / 60 )
	local nSec = math.floor( g_nCTFlagResult_OutTime % 60 )
	
	local str = string.format( "%s %s>", 
		GetGameWordText( "CTF_RESULT_TEXT", 0 ),
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), nMin, nSec ) )
	SetString( WIDGET_ID_CTFLAGRESULT, "text", str, "labelInfo" )
	
	g_nCTFlagResult_OutTime = g_nCTFlagResult_OutTime - 1
	
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "CTFlagResult_SetTimer"
	}
	SetTween( WIDGET_ID_CTFLAGRESULT, tbQuickSet, "", "", "labelInfo" )

end

-- 클래스 이미지 이름 반환
function CTFlagResult_ClassIndexToImgString( nClass )

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

--------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 타이틀 설정
function CTFlagResult_SetTitle( tbTitle )

	-- 타이틀 정보
	g_tbCTFlagResult_Title = tbTitle

end

-- 결과 정보 추가
function CTFlagResult_AddInfo( tbInfo )

	table.insert( g_tbCTFlagResult_Info, tbInfo )
	
	-- 본인 캐릭터 기록;
	if ( tbInfo[ 8 ] == 1 ) then
		
		g_tbCTFlagResult_MyInfo = tbInfo
			
	end

end

-- 오픈
function CTFlagResult_Open( nOutTime )

	-- 나가기 시간
	g_nCTFlagResult_OutTime = nOutTime
	
	if false == IsOpenWidget( WIDGET_ID_CTFLAGRESULT ) then
	
		OpenWidget( WIDGET_ID_CTFLAGRESULT )
	
	else
	
		-- 타이틀 설정
		CTFlagResult_SetTitleHeader()
		-- 리스트 설정
		CTFlagResult_SetList()
		-- 나가기 타이머
		CTFlagResult_SetTimer()
	
	end

end

--------------------------------------------------------------------------------------
-- 이벤트

-- 나가기
function Ev_CTFlagResult_Out()

	-- 인던 나가기 요청
	g_Msg:InstanceManRequestClose()
	
	CloseWidget( WIDGET_ID_CTFLAGRESULT )

end

-- 리스트 스크롤
function Ev_CTFlagResult_Scroll()

	local nScroll = GetInt( WIDGET_ID_CTFLAGRESULT, "scrollPosition", "list" )
	
	for i = 1, g_nCTFlagResult_ListCount, 1 do
	
		local strInst = "item" .. i
	
		local tbInfo = g_tbCTFlagResult_Info[ i + nScroll ]
		if nil == tbInfo then
		
			CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", "none", strInst .. ".mcWin" )
			CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", "none", strInst .. ".mcSchool" )
			CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", "none", strInst .. ".mcClass" )
			
			SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "btnHelp" .. i )
		
		else
		
			local strFrameID = "none"
		
			-- 순위 -----------------------------------
			-- win
			if 0 == tbInfo[ 1 ] then
				strFrameID = "win"
			-- lose
			else
				strFrameID = "lose"
			end
			CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", strFrameID, strInst .. ".mcWin" )
			
			-- 학원 -----------------------------------
			if 0 == tbInfo[ 2 ] then
				strFrameID = "sm"
			elseif 1 == tbInfo[ 2 ] then
				strFrameID = "ha"
			elseif 2 == tbInfo[ 2 ] then
				strFrameID = "bh"
			else
				strFrameID = "none"
			end
			CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", strFrameID, strInst .. ".mcSchool" )
			
			-- 클래스 -----------------------------------
			CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", 
				CTFlagResult_ClassIndexToImgString( tbInfo[ 3 ] ), strInst .. ".mcClass" )
		
		end
	
	end
	
	-- 자기자신
	if ( nil == g_tbCTFlagResult_MyInfo ) then
	
		CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", "none", "MyResult.mcWin" )
		CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", "none", "MyResult.mcSchool" )
		CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", "none", "MyResult.mcClass" )
			
		SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "MybtnHelp" )
	
	else
	
		local strFrameID = "none"
		
		-- 순위 -----------------------------------
		-- win
		if 0 == g_tbCTFlagResult_MyInfo[ 1 ] then
			strFrameID = "win"
		-- lose
		else
			strFrameID = "lose"
		end
		CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", strFrameID, "MyResult.mcWin" )
		
		-- 학원 -----------------------------------
		if 0 == g_tbCTFlagResult_MyInfo[ 2 ] then
			strFrameID = "sm"
		elseif 1 == g_tbCTFlagResult_MyInfo[ 2 ] then
			strFrameID = "ha"
		elseif 2 == g_tbCTFlagResult_MyInfo[ 2 ] then
			strFrameID = "bh"
		else
			strFrameID = "none"
		end
		CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", strFrameID, "MyResult.mcSchool" )
		
		-- 클래스 -----------------------------------
		CallFunction( WIDGET_ID_CTFLAGRESULT, "gotoAndStop", 
			CTFlagResult_ClassIndexToImgString( g_tbCTFlagResult_MyInfo[ 3 ] ), "MyResult.mcClass" )
				
	end

end

-- 보상 롤오버
function Ev_CTFlag_RewardRollOver( strIndex )

	local nIndex = tonumber( strIndex )
	if nil == nIndex then
		return
	end
	
	local nScroll = GetInt( WIDGET_ID_CTFLAGRESULT, "scrollPosition", "list" )
	local tbInfo = g_tbCTFlagResult_Info[ nIndex + nScroll ]
	
	local tbReward = tbInfo[ 9 ]

	-- 보상 패널 설정
	if 0 == table.getn( tbReward ) then
	
		SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "mcReward" )
	
	else

		SetBool( WIDGET_ID_CTFLAGRESULT, "visible", true, "mcReward" )
		
		-- 보상 패널 위치
		local nBtnX = GetInt( WIDGET_ID_CTFLAGRESULT, "x", "btnHelp" .. nIndex )
		local nBtnY = GetInt( WIDGET_ID_CTFLAGRESULT, "y", "btnHelp" .. nIndex )
		SetInt( WIDGET_ID_CTFLAGRESULT, "x", nBtnX + 16, "mcReward" )
		SetInt( WIDGET_ID_CTFLAGRESULT, "y", nBtnY + 16, "mcReward" )
		
		-- 보상 아이템 슬롯 설정
		for i = 1, CTFLAG_REWARD_SLOT_TOTAL, 1 do
		
			local tbItem = tbReward[ i ]
			if nil == tbItem then
			
				SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "mcReward.slot" .. i )
			
			else
			
				SetBool( WIDGET_ID_CTFLAGRESULT, "visible", true, "mcReward.slot" .. i )
				SetSlot_Item( SLOT_CTFLAG_REWARD .. i, tbItem[ 1 ], tbItem[ 2 ], tbItem[ 3 ] )
			
			end
		
		end
	
	end

end

-- 보상 롤아웃
function Ev_CTFlag_RewardRollOut()

	SetBool( WIDGET_ID_CTFLAGRESULT, "visible", false, "mcReward" )

end