
CTFREWARD_MAX_ITEM_LIST = 13

-- 결과 정보
g_tbCTFReward_RewardInfo = {}
-- 나의 결과 정보
g_tbCTFReward_MyRewardInfo = {}

-- 클라이언트 시간 정보
g_tbCTFReward_TimeInfo = {}
-- 윈도우 종료 시간 정보( 타이머 함수 호출용 카운트)
g_CTFReward_nWindowClosingTime = 0

-- 현재 활성화 중인 탭 인덱스
g_CTFReward_nActivatedTab =	0
-- 탭 내에서 내 랭크
g_CTFReward_MyRankInTab = 0

function LoadCTFReward()
	
	-- 탭 데이터
	local strTabData =
		GetGameWordText( "CTF_RESULT_BUTTON", 0 ) .. ";" ..			-- 전 체
		GetGameWordText( "CTF_RESULT_BUTTON", 1 ) .. ";" ..			-- 성문학원
		GetGameWordText( "CTF_RESULT_BUTTON", 2 ) .. ";" ..			-- 현암학원
		GetGameWordText( "CTF_RESULT_BUTTON", 3 ) .. ";"			-- 봉황학원
	
	-- 레이블
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 0 ), "Info.title_label.Rank" )				--	순위
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 1 ), "Info.title_label.School" )			--	학원
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 2 ), "Info.title_label.Class" )				--	클래스
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 3 ), "Info.title_label.CharacterName" )		--	캐릭터명
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 9 ), "Info.title_label.Damage" )			--	피해량
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 6 ), "Info.title_label.Heal" )				--	치유량
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 4 ), "Info.title_label.Kill" )				--	사살
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 5 ), "Info.title_label.Death" )				--	죽음
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 7 ), "Info.title_label.Revival" )			--	부활
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "CTF_RESULT_TEXT_BASE", 8 ), "Info.title_label.Score" )				--	기여도
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "label", GetGameWordText( "CTF_RESULT_BUTTON", 4 ), "Info.btnCancel" )					--	나가기

	-- <보상/강제나가기 적용시간> 레이블 설정 
	--CTFReward_SetTimeLabel()

	-- Event
	SetEvent( WIDGET_ID_CTF_RESULT_REWARD, EVENT_BUTTON_CLICK, "Event_CTFReward_btnCancel", "", "Info.btnCancel" )

	-- 초기 설정
	SetData( WIDGET_ID_CTF_RESULT_REWARD, strTabData, "buttonBar", false )											-- 탭 정보 입력
	SetInt( WIDGET_ID_CTF_RESULT_REWARD, "selectedIndex", 0, "buttonBar" )	-- 탭 초기 인덱스 선택
	
	SetEvent( WIDGET_ID_CTF_RESULT_REWARD, EVENT_INDEX_CLICK_CHANGE, "Event_CTFReward_btnbarTab", "index", "buttonBar" )	-- 탭 이벤트 등록
	SetEvent( WIDGET_ID_CTF_RESULT_REWARD, EVENT_SCROLL, "Ev_CTFReward_Scroll", "", "Info.List" )
	
	-- 보상결과창을 끌 수 없도록
	SetBool( WIDGET_ID_CTF_RESULT_REWARD, "visible", false, "closeBtn" )

end

-- 오픈
function OpenCTFReward()

	-- CTF 정보 갱신
	CTFReward_Update()

end

-- UI 열기 호출
function CTFReward_OpenWidget( nCloseTime )
	
	g_CTFReward_nWindowClosingTime = nCloseTime
	
	if false == IsOpenWidget( WIDGET_ID_CTF_RESULT_REWARD) then
		OpenWidget( WIDGET_ID_CTF_RESULT_REWARD)
	end
		
end

-- CTF 정보 갱신
function CTFReward_Update()

	CTFReward_Init_ListInfo()
	CTFReward_GetInfo()
	CTFReward_DisplayInfo()
	CTFReward_DisplayMyInfo()
	CTFReward_SetTimeLabel()
	
end

--CTF 정보 획득
function CTFReward_GetInfo()

	g_tbCTFReward_RewardInfo = {}
	
	g_tbCTFReward_MyRewardInfo = {}
	g_tbCTFReward_MyRewardInfo = g_CTField:GetMyResult()
	
	--탭 항목별 데이터 입력
	if 0 == g_CTFReward_nActivatedTab then
		g_tbCTFReward_RewardInfo = g_CTField:GetResultData()
	elseif 1 == g_CTFReward_nActivatedTab then
		g_tbCTFReward_RewardInfo = g_CTField:GetResultSM()
	elseif 2 == g_CTFReward_nActivatedTab then
		g_tbCTFReward_RewardInfo = g_CTField:GetResultHA()
	elseif 3 == g_CTFReward_nActivatedTab then
		g_tbCTFReward_RewardInfo = g_CTField:GetResultBA()
	end
	
end

function CTFReward_DisplayInfo()
	
	if nil == g_tbCTFReward_RewardInfo then
		return
	end
	
	g_CTFReward_MyRankInTab = 0

	-- 랭크 정보가 담길 테이블
	local tbRankList = {}
	for value in pairs( g_tbCTFReward_RewardInfo ) do

		local tbRewardInfo = g_tbCTFReward_RewardInfo[ value ]
		if nil ~= tbRewardInfo then
			
			local tb = {}
			
			if tbRewardInfo[ "strNAME" ] == g_MyChar:GetName() then
			
				g_CTFReward_MyRankInTab = value
			
			end
			
			-- 랭크 정보
			tb[ 1 ] = tostring( value )
			-- 학원 
			--tb[ 2 ] = GetGameWordText( "ACADEMY_NAME", tbRewardInfo[ "emSCHOOL" ] )
			-- 부서
			--tb[ 3 ] = CTFReward_ClassIndexToImgString( tbRewardInfo[ "emCLASS" ] )
			-- 이름
			tb[ 2 ] = tostring( tbRewardInfo[ "strNAME" ] )
			-- 피해량
			tb[ 3 ] = tostring( tbRewardInfo[ "nDAMAGE" ] )
			
			-- 치유량( 0 일때는 - 표기)
			local strHeal = ""
			if 0 == tbRewardInfo[ "nHEAL" ] then
				strHeal = "-"
			else
				strHeal = tostring( tbRewardInfo[ "nHEAL" ] )
			end
			tb[ 4 ] = strHeal
			
			-- 사살
			tb[ 5 ] = tostring( tbRewardInfo[ "nKILL" ] )
			-- 죽음
			tb[ 6 ] = tostring( tbRewardInfo[ "nDEATH" ] )

			-- 부활( 0 일때는 - 표기)
			local strRebirth = ""
			if 0 == tbRewardInfo[ "nREBIRTH" ] then
				strRebirth = "-"
			else
				strRebirth = tostring( tbRewardInfo[ "nREBIRTH" ] )
			end
			tb[ 7 ] = strRebirth
			
			-- 기여도( 0 이상은 + 표기)
			local strScore = ""
			if 0 < tbRewardInfo[ "nCONTRIBUTION" ] then
				strScore = "+" .. tostring( tbRewardInfo[ "nCONTRIBUTION" ] )
			elseif 0 == tbRewardInfo[ "nCONTRIBUTION" ] then
				strScore =  tostring( tbRewardInfo[ "nCONTRIBUTION" ] )
			else
				strScore = "-" .. tostring( tbRewardInfo[ "nCONTRIBUTION" ] )
			end
			tb[ 8 ] = strScore
			
			table.insert( tbRankList, tb )
			
		end
		
	end
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4", "label5", 
		"label6", "label7", "label8" )
	SetDataEx( WIDGET_ID_CTF_RESULT_REWARD, strParam, tbRankList, "Info.List" )
	Ev_CTFReward_Scroll()
end

function Ev_CTFReward_Scroll()
	
	local nOffset = GetInt( WIDGET_ID_CTF_RESULT_REWARD, "scrollPosition", "Info.List" )
	for i = 0, CTFREWARD_MAX_ITEM_LIST-1, 1 do
		
		local tbRewardInfo = g_tbCTFReward_RewardInfo[ i+1 ]
		
		if tbRewardInfo ~= nil then

			SetListItemBool(WIDGET_ID_CTF_RESULT_REWARD, i, "school", "visible", true, "Info.List" )
			SetListItemBool(WIDGET_ID_CTF_RESULT_REWARD, i, "class_img", "visible", true, "Info.List" )
			
			local strFrameID, nSchool, strClass = "none", tbRewardInfo[ "emSCHOOL" ], ConvertFrameToClass(tbRewardInfo[ "emCLASS" ])
			if 0 == nSchool then
				strFrameID = "sm"
			elseif 1 == nSchool then
				strFrameID = "ha"
			elseif 2 == nSchool then
				strFrameID = "bh"
			end
			
			CallListItemFunc( WIDGET_ID_CTF_RESULT_REWARD, i, "school", "gotoAndStop", strFrameID, "Info.List" )
			CallListItemFunc( WIDGET_ID_CTF_RESULT_REWARD, i, "class_img", "gotoAndStop", strClass, "Info.List" )
			
		else
			SetListItemBool(WIDGET_ID_CTF_RESULT_REWARD, i, "school", "visible", false, "Info.List" )
			SetListItemBool(WIDGET_ID_CTF_RESULT_REWARD, i, "class_img", "visible", false, "Info.List" )
		end
	
	end
	
end

function CTFReward_DisplayMyInfo()
	
	if nil == g_tbCTFReward_MyRewardInfo or "" == g_tbCTFReward_MyRewardInfo["strNAME"]then
		return
	end

	-- 출력에 사용될 컴포넌트 아이디 설정
	local strListID = "Info.MyInfo" 

	-- 출력
	local tbRewardInfo = g_CTField:GetMyResult()
	SetBool( WIDGET_ID_CTF_RESULT_REWARD, "visible", true,  "Info.MyInfo" )
	
	-- 랭크
	local strRank = ""
	if 0 == g_CTFReward_MyRankInTab then
		strRank = "-"
	else
		strRank = tostring( g_CTFReward_MyRankInTab )
	end
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", strRank , strListID .. ".textField1" )
	
	--SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", GetGameWordText( "ACADEMY_NAME", tbRewardInfo["emSCHOOL"] ), strListID..".School" )
	--SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", CTFReward_ClassIndexToImgString( tbRewardInfo["emCLASS"] ), strListID..".Class" )
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", tostring(tbRewardInfo["strNAME"]), strListID..".textField2" )
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", tostring(tbRewardInfo["nDAMAGE"]), strListID..".textField3" )
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", tostring(tbRewardInfo["nKILL"]), strListID..".textField5" )
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", tostring(tbRewardInfo["nDEATH"]), strListID..".textField6" )
	
	-- 치유량( 0 일때는 - 표기)
	local strHeal = ""
	if 0 == tbRewardInfo["nHEAL"] then
		strHeal = "-"
	else
		strHeal = tostring( tbRewardInfo["nHEAL"] )
	end
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", strHeal, strListID..".textField4" )

	-- 부활( 0 일때는 - 표기)
	local strRebirth = ""
	if 0 == tbRewardInfo["nREBIRTH"] then
		strRebirth = "-"
	else
		strRebirth = tostring( tbRewardInfo["nREBIRTH"] )
	end
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", strRebirth, strListID..".textField7" )
	
	-- 기여도( 0 이상은 + 표기)
	local strScore = ""
	if 0 < tbRewardInfo["nCONTRIBUTION"] then
		strScore = "+" .. tostring( tbRewardInfo["nCONTRIBUTION"] )
	elseif 0 == tbRewardInfo["nCONTRIBUTION"] then
		strScore =  tostring( tbRewardInfo["nCONTRIBUTION"] )
	else
		strScore = "-" .. tostring( tbRewardInfo["nCONTRIBUTION"] )
	end
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", strScore, strListID..".textField8" )
	
	-- 아이콘 설정
	local strFrameID, nSchool, strClass = "none", tbRewardInfo[ "emSCHOOL" ], ConvertFrameToClass(tbRewardInfo[ "emCLASS" ])
	if 0 == nSchool then
		strFrameID = "sm"
	elseif 1 == nSchool then
		strFrameID = "ha"
	elseif 2 == nSchool then
		strFrameID = "bh"
	end
	
	CallFunction( WIDGET_ID_CTF_RESULT_REWARD, "gotoAndStop", strFrameID, strListID..".school" )
	CallFunction( WIDGET_ID_CTF_RESULT_REWARD, "gotoAndStop", strClass, strListID..".class_img" )
end

-- <보상/강제나가기 적용시간> 레이블 설정 
function CTFReward_SetTimeLabel()
	
	-- 출력 
	local strResult = GetGameWordText("CTF_RESULT_TEXT", 0) .. 
					string.format( GetGameWordText("CTF_RESULT_TEXT", 1), 0, g_CTFReward_nWindowClosingTime ) .. ">"
					
	SetString( WIDGET_ID_CTF_RESULT_REWARD, "text", strResult, "Info.labelCloseWindow" )
	
	-- 클라이언트 종료시간 타이머 설정
	if 0 >= g_CTFReward_nWindowClosingTime then
		--Event_CTFReward_btnCancel()
		return
	else	
		g_CTFReward_nWindowClosingTime = g_CTFReward_nWindowClosingTime - 1		-- Sec 초단위가 들어옴
		
	end
	
	-- 타이머 호출
	local tbQuickSet_SetTimeLabel = 
	 {
	  time = 1000,			
	  delay = 0,
	  ease = EASE_NONE,
	  loop = false,
	  onComplete = "CTFReward_SetTimeLabel"
	 } 
	SetTween( WIDGET_ID_CTF_RESULT_REWARD, tbQuickSet_SetTimeLabel, "", "", "" )

end

-- 클래스 이미지 이름 반환
function CTFReward_ClassIndexToImgString( nClass )
	-- 소스
	local strClassImgName = "NONE"
	if CHARCLASS_IDX_FIGHTER_M == nClass then				-- 격투부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 0 )
	elseif CHARCLASS_IDX_ARMS_M == nClass then				-- 검도부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 1 )
	elseif CHARCLASS_IDX_ARCHER_W == nClass then			-- 양궁부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 2 )
	elseif CHARCLASS_IDX_SPIRIT_W == nClass then			-- 기예부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 3 )
	elseif CHARCLASS_IDX_EXTREAM_M == nClass then			-- 극강부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 4 )
	elseif CHARCLASS_IDX_EXTREAM_W == nClass then			-- 극강부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 4 )
	elseif CHARCLASS_IDX_FIGHTER_W == nClass then			-- 격투부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 0 )
	elseif CHARCLASS_IDX_ARMS_W == nClass then				-- 검도부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 1 )
	elseif CHARCLASS_IDX_ARCHER_M == nClass then			-- 양궁부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 2 )
	elseif CHARCLASS_IDX_SPIRIT_M == nClass then			-- 기예부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 3 )
	elseif CHARCLASS_IDX_SCIENTIST_M == nClass then		-- 과학부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 5 )
	elseif CHARCLASS_IDX_SCIENTIST_W == nClass then		-- 과학부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 5 )
	elseif CHARCLASS_IDX_ASSASSIN_M == nClass then			-- 인술부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 6 )
	elseif CHARCLASS_IDX_ASSASSIN_W == nClass then			-- 인술부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 6 )
	elseif CHARCLASS_IDX_TRICKER_M == nClass then			-- 마술부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 7 )
	elseif CHARCLASS_IDX_TRICKER_W == nClass then			-- 마술부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 7 )
	elseif CHARCLASS_IDX_ACTOR_M == nClass then			-- 연극부 남
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 9 )
	elseif CHARCLASS_IDX_ACTOR_W == nClass then			-- 연극부 여
		strClassImgName = GetGameWordText( "ITEM_SHOP_SEARCH_CLASS", 9 )
	else
		strClassImgName = "NONE"
	end

	return strClassImgName
	--elseif CHARCLASS_ETC_M == nClassIndex then
	--elseif CHARCLASS_ETC_W == nClassIndex then
end

function CTFReward_Init()
	g_tbCTFReward_TimeInfo = {}	
	g_CTFReward_nWindowClosingTime = 0
	g_CTFReward_nActivatedTab	=	0 
	-- 리스트 관련 정보 일괄 초기화
	CTFReward_Init_ListInfo()	
end

-- 리스트 관련 정보 일괄 초기화
function CTFReward_Init_ListInfo()

	local strListID = "Info.List" 
	
	for i = 1, CTFREWARD_MAX_ITEM_LIST, 1 do
		-- 2.1. 리스트 레이블 비우기
		-- field 2번 이후
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField1", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField2", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField3", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField4", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField5", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField6", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField7", "htmlText", "", strListID )
		SetListItemString( WIDGET_ID_CTF_RESULT_REWARD, i - 1, "textField8", "htmlText", "", strListID )
		
	end
	
	-- 2.2 리스트 field1 비우기
	RemoveData( WIDGET_ID_CTF_RESULT_REWARD, 1, strListID, true )
end

--Event------------------------------------------------------------------------------
function Event_CTFReward_btnbarTab( nSelectedTap )
	g_CTFReward_nActivatedTab = nSelectedTap
	CTFReward_Update()
end

-- 윈도우 종료 및 CTF 나가기
function Event_CTFReward_btnCancel()
	g_Msg:ReqCTFieldQuit()
	CloseWidget( WIDGET_ID_CTF_RESULT_REWARD )
end