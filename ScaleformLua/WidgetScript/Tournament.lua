-- 그룹 사이즈
TOURNAMENT_GROUP_SIZE = 8						-- 1개 조에 참가 가능한 인원 수

-- 아이디 강조 레이아웃
LIGHT_BLUE		=	"Highlight_Blue"
LIGHT_YELLOW	=	"Highlight_Yellow"

-- 승자 표기 라인
WINNER_LINE_DEFAULT			= "default;"
WINNER_LINE_UP				= "Top;"
WINNER_LINE_UP_SELECTED		= "Top_selected;"
WINNER_LINE_DOWN			= "Down;"
WINNER_LINE_DOWN_SELECTED	= "Down_selected;"


-- 변수
-- 대진표
g_Tournament_nMatchingSize = 1					-- 현재 출력중인 토너먼트 매칭 타입 32 16 8 4 2 1(강 == Group == 8명) 존재
g_Tournament_nSelectGroup = 0					-- 현재 선택된 그룹 임시 1 기본값 0아무것도 선택 안됨
g_Tournament_bGameStart = false					-- 현재 토너먼트가 진행중인지 판단
g_Tournament_nDefaultUIOffset = 0				--  1조 이하의 인원인 경우 출력방식의 예외 처리에 따른 출력 위치 보정값

g_tbTournament_GroupInfo = {}
g_tbTournament_LineState = {}					-- 현재 출력중인 라인의 출력 상태( 현재 상태를 저장하여 새로 들어오는 상태값과 비교용)

-- 토너먼트 배팅 정보
g_tbTournament_BettingInfo = {}
g_tbTournament_BettingInfo["llMinBettingSize"] = 0
g_tbTournament_BettingInfo["llMaxBettingSize"] = 0
g_tbTournament_BettingInfo["dwScriptID"] = 0

-- 토너먼트 배팅 그룹 정보
g_tbTournament_BettingGroupInfo = {}
g_tbTournament_BettingGroupInfo["dwUserDBID"] = 0
g_tbTournament_BettingGroupInfo["dwBettingGroup"] = -1		-- 필수
g_tbTournament_BettingGroupInfo["llBettingCount"] = 0
g_tbTournament_BettingGroupInfo["dwLeageID"] = 0
g_tbTournament_BettingGroupInfo["fBettingRatio"] = 0
	
function LoadTournament()
	
	-- 토너먼트 대진표 UI 갱신 주기
	local tbQuickSet = 
	 {
	  time = 3000,			
	  delay = 0,
	  ease = EASE_NONE,
	  loop = true,
	  onComplete = "Tournament_FrameMove"
	 } 
	--SetTween( WIDGET_ID_TOURNAMENT, tbQuickSet, "", "", "" )

	g_tbTournament_GroupInfo = {}
	Tournament_Update()

end

function Tournament_Init()

	g_tbTournament_GroupInfo = {}
	g_tbTournament_LineState = {}
	
	-- 토너먼트 배팅 그룹 정보	
	g_tbTournament_BettingInfo = {}
	g_tbTournament_BettingInfo["llMinBettingSize"] = 0
	g_tbTournament_BettingInfo["llMaxBettingSize"] = 0
	g_tbTournament_BettingInfo["dwScriptID"] = 0

	-- 토너먼트 배팅 그룹 정보
	g_tbTournament_BettingGroupInfo = {}
	g_tbTournament_BettingGroupInfo["dwUserDBID"] = 0
	g_tbTournament_BettingGroupInfo["dwBettingGroup"] = -1
	g_tbTournament_BettingGroupInfo["llBettingCount"] = 0
	g_tbTournament_BettingGroupInfo["dwLeageID"] = 0
	g_tbTournament_BettingGroupInfo["fBettingRatio"] = 0
	
end

function Tournament_FrameMove()
	g_tbTournament_GroupInfo = {}
	Tournament_Update()
end

function Tournament_Update()
	Tournament_GetInfo()											-- 1. 클라이언트로 부터 정보 획득 및 기본 정보 초기화
	Tournament_Init_Group_Info(g_Tournament_nMatchingSize)			-- 2. 클라이언트 정보를 바탕으로 UI기본 출력 설정
	Tournament_DisplayInfo()										-- 3. 클라이언트 정보 해당 컴포넌트별 입력
end

function Tournament_GetInfo()

	-- 클라이언트로 부터 정보 획득
	if nil == g_tbTournament_GroupInfo or 0 == table.getn(g_tbTournament_GroupInfo) then 
		
		local tbTournamentInfo = g_Competition:GetMatchingInstanceList( 0 )
		
		g_tbTournament_GroupInfo = {}
		g_tbTournament_GroupInfo = tbTournamentInfo[3]  -- 추후 자동으로 토너먼트 스크립트 아이디를 통해서 해당 PvP 테이블 뽑아 오게 수정
	end

	-- 기본 출력 변수 설정
	-- 기본 예외처리는 토너먼트 배팅가능이 되면 
	if nil ~= g_tbTournament_GroupInfo  or 0 ~= table.getn(g_tbTournament_GroupInfo) then

		-- 1. 리그 사이즈 정보
		if nil ~= g_tbTournament_GroupInfo then
			g_Tournament_nMatchingSize = g_tbTournament_GroupInfo["dwMatchingSize"] / 8	--초기 데이터		
		end
		
		-- 1.1 선택 그룹 변수 초기화 (매칭 사이즈 = 최대 그룹수)
		if g_Tournament_nSelectGroup > g_Tournament_nMatchingSize then
			g_Tournament_nSelectGroup = 1
		end
		
		-- 1.2 리그 사이즈 기본 1 ( 1.1 항목과 순서가 변경이 되면 안된다. g_Tournament_nSelectGroup 검색 조건)
		if 1 > g_Tournament_nMatchingSize or 0 ~= g_Tournament_nSelectGroup then 
			g_Tournament_nMatchingSize = 1		
		end
		
		-- 2. 배팅 정보
		if false == Tournament_GetTournametStart() then
			g_Msg:ReqGetBettingInfoCompetition( g_tbTournament_GroupInfo["Script"] )
		end
		
		-- 임시 출력 확인
		for nIndex in pairs(g_tbTournament_GroupInfo["tbTorunamentInfo"]) do
			local tbInfo = g_tbTournament_GroupInfo["tbTorunamentInfo"][nIndex]
			if nil ~= tbInfo then
			end
		end
		
	else
		-- 클라이언트 정보가 없는 경우
		--1. 리그 사이즈 정보( 예외)
		g_Tournament_nMatchingSize = 1
		
		-- 2. 배팅 정보(예외)
		if false == Tournament_GetTournametStart() then
			if nil ~= g_tbTournament_GroupInfo[3]["Script"] then
				g_Msg:ReqGetBettingInfoCompetition( g_tbTournament_GroupInfo["Script"] )
			end
		end
	end

end

function Tournament_DisplayInfo()
	
	--버튼 컴포넌트 정보 입력
	if 1 == g_Tournament_nMatchingSize then
		Tournament_DisplayGroupInfo( g_tbTournament_GroupInfo, g_Tournament_nSelectGroup )
	else
		Tournament_DisplayGroupListInfo ( g_tbTournament_GroupInfo, g_Tournament_nMatchingSize )
	end
	
	-- 토너먼트 추가 정보 입력	
	Tournament_DisplayCommonInfo( g_tbTournament_GroupInfo, g_Tournament_nMatchingSize ) 

end

-- 공용 레이블창 정보 입력
function Tournament_DisplayCommonInfo( tbTournamentInfo, nMatchingSize ) 

	SetString( WIDGET_ID_TOURNAMENT, "text", tostring(g_tbTournament_GroupInfo["nTournamentInfo"]), "Group.label1" )	
	SetString( WIDGET_ID_TOURNAMENT, "text", "2", "Group.label2" )	
	SetString( WIDGET_ID_TOURNAMENT, "text", "3", "Group.label3" )	
	
	-- 토너먼트 상태 파악 변수
	if nil ~= tbTournamentInfo["ContentsStatus"] then
		SetString( WIDGET_ID_TOURNAMENT, "text", tostring(tbTournamentInfo["ContentsStatus"]) , "Group.label7" )
	end
	
	-- 배팅 정보
	-- 배팅 전 정보
	SetString( WIDGET_ID_TOURNAMENT, "text", "Betting Info : " , "Group.label4" )	
	SetString( WIDGET_ID_TOURNAMENT, "text", tostring( g_tbTournament_BettingGroupInfo["llBettingCount"] ) .. "Group", "Group.label5" )	
	SetString( WIDGET_ID_TOURNAMENT, "text", tostring( g_tbTournament_BettingGroupInfo["fBettingRatio"] ) .. "Gold", "Group.label6" )	
	
	-- 배팅 후 정보
	
	if 0 == g_tbTournament_GroupInfo["nTournamentInfo"]then return end
	
	local nGroupIndex = g_tbTournament_BettingGroupInfo["dwBettingGroup"]
	
	if nil == nGroupIndex then 
	end
	
	if nGroupIndex > g_tbTournament_GroupInfo["nTournamentInfo"] or 0 > nGroupIndex then 
		return
	end
	
	local strGroupName = g_tbTournament_GroupInfo["tbTorunamentInfo"][nGroupIndex + 1]["strGroupName"]
	if "" == strGroupName or nil == strGroupName then
		return
	end
	
	SetString( WIDGET_ID_TOURNAMENT, "text", tostring( strGroupName ) , "Group.label4" )	
	SetString( WIDGET_ID_TOURNAMENT, "text", tostring( g_tbTournament_BettingGroupInfo["llBettingCount"] ), "Group.label5" )	
	SetString( WIDGET_ID_TOURNAMENT, "text", tostring( g_tbTournament_BettingGroupInfo["fBettingRatio"] ), "Group.label6" )
	
end

-- 그룹 전체 맵 정보 출력
function Tournament_DisplayGroupListInfo( tbTournamentInfo, nMatchingSize ) 
	
	if nil == tbTournamentInfo or 0 == tbTournamentInfo["nTournamentInfo"]then return end
	if 1 >= nMatchingSize then return end -- 1개조 이하의 인원은 이 출력 함수를 통과하지 않는다
	
	-- 본인아이디 강조를 위한 값	
	local strMyName = g_MyChar:GetName()-- 본인 ID
	local nMyNameIndex = 0				-- 본인 강조될 인덱스
	local nMyNameRound = 0				-- 자신의 라운드 위치
	
	-- 배팅 대상 강조를 위한 값	
	local nToToID = g_tbTournament_BettingGroupInfo["dwBettingGroup"]		-- 배팅 대상 ID
	local nToToIndex = -1													-- 배팅대상 강조될 인덱스
	local nToToRound = 0													-- 배팅대상 라운드 위치

	local nCurrentRound = tbTournamentInfo["dwCurrentRound"]		-- 현재 진행중인 라운드 정보( 그룹 내부 라운드)
	local nMaxIndex = tbTournamentInfo["nTournamentInfo"]			-- 토너먼트 인원 전체 개수
	if nil == nMaxIndex then
		nMaxIndex = 0
	end
	
	for nIndex = 1, nMaxIndex, 1 do
		
		local tbInfo = tbTournamentInfo["tbTorunamentInfo"][nIndex]
		
		if nil ~= tbInfo then
			
			local strPlayerName = "****"
			if "" ~= tbInfo["strGroupName"] then
				strPlayerName = tbInfo["strGroupName"]
			end
							
			-- 2.2.1 자신의 아이디 강조
			if strMyName == strPlayerName then
				-- 2.2.2 마지막 아이디 표기를 위해 최종 컴포넌트 아이디를 받아 둔다
				nMyNameIndex = nIndex
				nMyNameRound = tbInfo["dwPosition"]
			end
			
			-- 2.3.1 배팅 대상 강조
			if nToToID == tbInfo["nGROUPID"]  and -1 ~= tbInfo["nGROUPID"] then
				-- 2.3.2 마지막 대상 강조를 위해 최종 컴포넌트 아이디를 받아 둔다
				nToToIndex = nIndex
				nToToRound = tbInfo["dwPosition"]
			end
		
		end -- if nil ~= tbInfo then
		
	end
	
	-- 강조 정보 
	local strGroupID = "G"..tostring(nMatchingSize)
	local strComponentID = "Group." .. strGroupID .. "_"
	
	local nLeagueSize = tbTournamentInfo["dwMatchingSize"]		-- 현재 게임의 전체 리그 사이즈	

	-- 자신의 그룹 정보가 있음
	if 0 ~= nMyNameIndex then
		local nGroupID = Tournament_GetGroupNumber( nLeagueSize, nMyNameRound, nMyNameIndex )
		local strPath = strComponentID .. tostring(nGroupID)
		SetBool( WIDGET_ID_TOURNAMENT, "visible", true, strPath.."."..LIGHT_BLUE )
	end
	
	-- 배팅대상 그룹 정보가 있음
	if -1 ~= nToToIndex then
		local nGroupID = Tournament_GetGroupNumber( nLeagueSize, nToToRound, nToToIndex )
		local strPath = strComponentID .. tostring(nGroupID)
		SetBool( WIDGET_ID_TOURNAMENT, "visible", true, strPath.."."..LIGHT_YELLOW )
		g_tbTournament_BettingInfo["GroupID"] = nGroupID
	end

end

-- 1개조 그룹 내부 정보 출력
function Tournament_DisplayGroupInfo( tbTournamentInfo, nSelectGroup )
	
	
	if nil == tbTournamentInfo or 0 == tbTournamentInfo["nTournamentInfo"]then return end
	if 1 > nSelectGroup then nSelectGroup = 1 end									-- 선택된 조가 1보다 작은데 호출된 경우 디폴트 값으로 1조의 정보를 호출한다

	-- 예외. 1조 이하의 토너먼트가 열릴 경우 최소 인원수에 따른 디폴트 값이 생긴다( 생략 불가 )
	-- UI 출력 위치 변화
	local nRoundCount = 0
	local nMaxIndex = tbTournamentInfo["nTournamentInfo"]
	for i = 1, nMaxIndex, 1 do
		if 1 == tbTournamentInfo["tbTorunamentInfo"][i]["dwPosition"] then
			nRoundCount = nRoundCount + 1
		end
	end
	
	g_Tournament_nDefaultUIOffset = 0							-- 1 조이하의 인원에서 출력 인원 예외
	-- 총 1그룹 출력 개수 15 - 1라운드 속한인원수 
	if 5 < nRoundCount  then
		g_Tournament_nDefaultUIOffset = 0
	elseif 2 < nRoundCount  then
		g_Tournament_nDefaultUIOffset = 8
	elseif 1 < nRoundCount  then
		g_Tournament_nDefaultUIOffset = 12
	else
		g_Tournament_nDefaultUIOffset = 14
	end
	
	-- 본인아이디 강조를 위한 값	
	local strMyName = g_MyChar:GetName()-- 본인 ID
	local strNameID = ""				-- 본인 강조될 컴포넌트 ID

	-- 배팅 대상 강조를 위한 값	
	local nToToID	 = g_tbTournament_BettingGroupInfo["dwBettingGroup"]	-- 배팅 대상 ID
	local strToToID = ""									-- 본인 강조될 컴포넌트 ID
	local nToToIndex = 0									-- 배팅 대상 최종 인덱스
	local nToToRound = 0									-- 배팅 대상이 진행한 최종 라운드
	
	-- 정보 출력-------------------------
	local strGroupID = "G1"			-- 조 단위 출력 정보
	local tbLineInfo = {}			-- 승자 표기라인 출력 정보 저장
	
	-- 1. 선택그룹 배열 Offset 계산	
	local nCurrentRound = tbTournamentInfo["dwCurrentRound"]		-- 현재 진행중인 라운드 정보
	if 4 < nCurrentRound then 
		nCurrentRound = 4											-- 스케일폼 UI는 1조 4개 라운드 이상은 표시 될 필요가 없다 이후도 필요하다면 UI 구조 변경이 필요함
	end					
	local nLeagueSize = tbTournamentInfo["dwMatchingSize"]		-- 현재 게임의 전체 리그 사이즈
	
	local nRoundOffset = 0		-- 각 라운드별 데이터 시작 위치
	local nStart = 0			-- 선택 그룹 정보 시작 인덱스 
	local nEnd = 0				-- 선택 그룹 정보 끝 인덱스

	local nDisplayCount = 1					-- 출력된 아이템 개수 카운트
	for nRound = 1, nCurrentRound, 1 do		-- 오프셋 결정을 위한 for문 라운드 구분 없이 모든 데이터가 1차원으로 되어 있음
		--!! 배열을 읽어 오는 오프셋 위치값 계산( 변경 불가 변경시 서버 로직 구조도 변경 되어야함 )
		-- 예외.Lua에서는 0값 계산에 문제가 있음 예외처리
		-- 각 라운드별 최초 배열 시작지점
		if 1 == nRound then 
			nRoundOffset = 1 														
		else 
			nRoundOffset = nRoundOffset + ( nLeagueSize / (2 ^ (nRound - 2)) ) 		 
		end
		
		nStart = nRoundOffset + ( TOURNAMENT_GROUP_SIZE / (2 ^ (nRound - 1)) ) * (nSelectGroup - 1)	-- 라운드 내부 선택 그룹 데이터 시작 위치	
		
		-- 최대 라운드는 1조당 4라운드가 진행된다. 현재 함수는 1조 내부를 표현하기 때문에 4를 고정값으로 사용한다 2^4 = 16
		--if nLeagueSize == math.pow(2, nRound) then 	
		if 4 == nRound then			 
			if 0 < g_Tournament_nDefaultUIOffset then
				nEnd = nMaxIndex
			else
				nEnd = nStart															-- lua for문 특성 예외 처리 마지막 인덱스 위치 ==
			end
			
		else
			nEnd = nStart + (TOURNAMENT_GROUP_SIZE / nRound) - 1					-- 라운드 내부 선택 그룹 데이터 종료 위치 
		end
		

		-- 배열이 라운드별 정렬이 되어 있어 라운드별 for문을 다 돌아야 한다 최대 라운드는 4 (1~4)
		for nIndex = nStart, nEnd, 1 do
			
			local tbInfo = tbTournamentInfo["tbTorunamentInfo"][nIndex]
			
			if nil ~= tbInfo then
				
				-- 2.1 대진표 아이디 입력
				-- 2.1.1 아이디 생성(주의! tbInfo["dwIndex"]가 컴포넌트의 위치를 나타낸다. 현재 클라이언트 정보 전송시 미리 정렬된 값이 들어옴)
				local strComponentID = "Group." .. strGroupID .. "_"..tostring( g_Tournament_nDefaultUIOffset + nDisplayCount )		-- 1조 이하일떄는 nDefaultOffset 가 0이 넘는다
				
				-- 2.1.2 버튼 컴포넌트에 입력
				local strPlayerName = "****"
				if "" ~= tbInfo["strGroupName"] then
					strPlayerName = tbInfo["strGroupName"]
				end
				SetString( WIDGET_ID_TOURNAMENT, "label", strPlayerName, strComponentID..".btn" )			-- 텍스트
				
				-- 2.2.1 자신의 아이디 강조
				if strMyName == strPlayerName then
					-- 2.2.2 마지막 아이디 표기를 위해 최종 컴포넌트 아이디를 받아 둔다
					strNameID = strComponentID
				end
				
				-- 2.3.1 배팅 대상 강조
				if nToToID == tbInfo["nGROUPID"] and -1 ~= tbInfo["nGROUPID"] then
					-- 2.3.2 마지막 대상 강조를 위해 최종 컴포넌트 아이디를 받아 둔다
					strToToID = strComponentID
					nToToIndex = nIndex
					nToToRound = tbInfo["dwPosition"]
				end
				
				-- 2.4.1 라인 출력 정보 저장
				tbLineInfo[ g_Tournament_nDefaultUIOffset + nDisplayCount ] =  tbInfo["nGROUPID"]		-- 기존 UI 구조체 저장 정보를 따르기 위해 해당 값을 사용
				nDisplayCount = nDisplayCount + 1
			
			end -- if nil ~= tbInfo then
			
		end
		
		-- 1개조 최대 출력개수를 넘긴다면 출력 종료
		if 15 < (g_Tournament_nDefaultUIOffset + nDisplayCount) then break end
		
	end -- for nRound = 1, nCurrentRound, 1 do	
	
	-- 2.2.3 본인 아이디 강조 해당 컴포넌트 아이디가 담겨 있다
	if "" ~= strNameID then
		SetBool( WIDGET_ID_TOURNAMENT, "visible", true, strNameID.."."..LIGHT_BLUE )
	end
	
	-- 2.3.3 배팅 대상 강조 해당 컴포넌트 아이디가 담겨 있다
	if "" ~= strToToID then
		SetBool( WIDGET_ID_TOURNAMENT, "visible", true, strToToID.."."..LIGHT_YELLOW )
		g_tbTournament_BettingInfo["GroupID"] = Tournament_GetGroupNumber( nLeagueSize, nToToRound, nToToIndex )
	end
		
	-- 그룹 내부 추가 정보 입력 승자표기라인, 버튼 비활성화, 
	Tournament_DisplayGroupInfoEx( tbLineInfo, g_Tournament_nDefaultUIOffset )
end

-- 그룹 내부 추가 정보 입력 승자표기라인, 버튼 비활성화, 
function Tournament_DisplayGroupInfoEx( tbLineInfo, g_Tournament_nDefaultUIOffset )

	if nil == tbLineInfo then return end

	local nMaxLine = 7				-- 한 조 라인 인덱스는 7개
	
	-- 출력될 컴포넌트를 기준으로 해당 인덱스 정보에 접근하여 데이터를 결정한다( 수정 금지 ) 
	local nGroupIndex1		= 1
	local nGroupIndex2		= 2
	local nTargetGroupIndex	= 9
	
	local nLineDisplayState = 0				-- 현재 출력된 라인의 상태값을 저장할 변수 0 : 기본, 1 : Up, 2 : Down
		
	for nLine = 1, nMaxLine, 1 do
		
		local strLineID = "Group.Line" .. tostring(nLine)

		local tbGroup1 = tbLineInfo[nGroupIndex1]				-- 그룹 정보 1
		local tbGroup2 = tbLineInfo[nGroupIndex2]				-- 그룹 정보 2
		local tbTargetGroup = tbLineInfo[nTargetGroupIndex]		-- 목표 그룹 정보
		
		nLineDisplayState = 0

		-- 1. 라인 표기
		if -1 ~= tbTargetGroup or nil ~= tbTargetGroup then			
			
			if tbGroup1 == tbGroup2 then
				-- 대진표가 비어 있는 상태		
				nLineDisplayState = 0			
				-- 목표 대상에 값이 없음
				CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_DEFAULT, strLineID )
				
			elseif tbTargetGroup == tbGroup1 then
				
				nLineDisplayState = 1			-- Up 상태
				-- UP 레이아웃 출력 상태 이전에 출력된 상태인지 판단을 한다
				if g_tbTournament_LineState[nGroupIndex1] ~= tbTargetGroup then
					CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_UP, strLineID )
				else
					CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_UP_SELECTED, strLineID )
				end
						
			elseif tbTargetGroup == tbGroup2 then
				
				nLineDisplayState = 2			-- Down 상태
				-- DOWN 레이아웃 출력 상태 이전에 출력된 상태인지 판단을 한다
				if g_tbTournament_LineState[nGroupIndex2] ~= tbTargetGroup then
					CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_DOWN, strLineID )
				else
					CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_DOWN_SELECTED, strLineID )
				end
				
			end
						
		else
			
			nLineDisplayState = 0				-- 기본 상태
			-- 목표 대상에 값이 없음
			CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_DEFAULT, strLineID )

		end
		
		-- 2. 버튼 비활성화 ( 패자 표기)
		if 1 == nLineDisplayState then
			local strBtnID = "Group.G1_" .. tostring(nGroupIndex2) .. ".btn"
			--SetString( WIDGET_ID_TOURNAMENT, "label", "", strBtnID )
			SetBool( WIDGET_ID_TOURNAMENT, "enabled", false, strBtnID )
		elseif 2 == nLineDisplayState then
			local strBtnID = "Group.G1_" .. tostring(nGroupIndex1) .. ".btn"
			--SetString( WIDGET_ID_TOURNAMENT, "label", "", strBtnID )
			SetBool( WIDGET_ID_TOURNAMENT, "enabled", false, strBtnID )
		end
			
		-- 인덱스 값 증가
		nGroupIndex1 = nGroupIndex1 + 2
		nGroupIndex2 = nGroupIndex2 + 2
		nTargetGroupIndex = nTargetGroupIndex + 1
		
	end
	
	-- 현재 라인 출력 정보 저장
	g_tbTournament_LineState = {}	-- 라인 정보가 저장될 테이블 초기화
	g_tbTournament_LineState = tbLineInfo
	
end

-- 입력된 리그사이즈(~강), 현재 타겟 라운드, 인덱스값으로 현재 속해있는 그룹번호를 반환
function Tournament_GetGroupNumber( nLeagueSize, nTargetRound, nTargetIndex )
	
	local nMyGroup = 0												-- 내 그룹 정보
	local nLeagueSize = nLeagueSize--tbTournamentInfo["dwMatchingSize"]		-- 현재 게임의 전체 리그 사이즈(~강)

	local nMyRound = nTargetRound --tbMyInfo["dwPosition"]
	
	local nMaxRoundIndex = 0
	local nRoundOffset = 0		-- 자신이 속한 라운드의 시작 최소 인덱스
	for i = 1, nMyRound, 1 do
		-- 각 라운드별 최대 값 계산
		nRoundOffset = nRoundOffset + ( nLeagueSize / (2 ^ (i - 1)) )	-- 해당 라운드까지 포함된 배열의 개수
		nMaxRoundIndex = nLeagueSize / ( 2 ^ (i - 1) )							-- 해당 라운드의 배열 개수
	end
	
	-- 해당 구문에서 자신의 조 구함
	for i = (nLeagueSize / TOURNAMENT_GROUP_SIZE ), 1, -1 do

		if nRoundOffset < nTargetIndex then
			nMyGroup = 0	-- 0 이 되면 그룹 찾지 못함
			break;
		end
		
		-- 해당 값 보다 아래라면 그룹이 있다
		local nGroupMax = nRoundOffset - (nMaxRoundIndex / i) 
		if nTargetIndex > nGroupMax then
			nMyGroup = i
			break
		end
	end
		
	return nMyGroup
end

--대진표 초기화 함수
function Tournament_Init_Group_Info( nGroupSize )
	
	-- 0.대진표 사이즈에 따른 변수 설정
	local strGroupID = ""					-- 현재 선택된 대진표 아이디( x강)
	local nLineLevel = 0					-- 컴포넌트 나뉜 등급 개수
	local nTotalLine = nGroupSize - 2		-- 전체 라인 컴포넌트 개수 
	
	if 32 == nGroupSize then
		strGroupID = "G32"
	elseif 16 == nGroupSize then
		strGroupID = "G16"
	elseif 8 == nGroupSize then
		strGroupID = "G8"
	elseif 4 == nGroupSize then
		strGroupID = "G4"
	elseif 2 == nGroupSize then
		strGroupID = "G2"
		nTotalLine = 1			-- 예외(컴포넌트 구성에 따른 예외)
	elseif 1 == nGroupSize then
		strGroupID = "G1"
		nGroupSize = 15			-- 예외(컴포넌트 구성에 따른 예외)
		nTotalLine = 6
	end
	CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndStop", strGroupID, "Group" )
	
	-- 해당 대진표 내부 컴포넌트 초기화
	-- 1. 아이디 표기 부분
	for nIndex = 1, nGroupSize, 1 do
	
		-- 1.1 아이디 생성
		local strComponentID = "Group." .. strGroupID .. "_"..tostring( nIndex ).."."
		-- 1.2 버튼 컴포넌트 초기화
		SetString( WIDGET_ID_TOURNAMENT, "label", "Group"..tostring(nIndex), strComponentID.."btn" )			-- 텍스트
		SetBool( WIDGET_ID_TOURNAMENT, "visible", false, strComponentID..LIGHT_BLUE )	-- 강조 파랑
		SetBool( WIDGET_ID_TOURNAMENT, "visible", false, strComponentID..LIGHT_YELLOW )	-- 강조 노랑
		--이벤트 등록
		SetEvent( WIDGET_ID_TOURNAMENT, EVENT_BUTTON_CLICK, "Event_Tournament_btn"..tostring(nIndex), "", strComponentID.."btn" )
		--SetEvent( WIDGET_ID_TOURNAMENT, EVENT_BUTTON_CLICK, "Event_Tournament_btn", "", strUp )
	
	end
	
	--2. 승자 라인 초기화 
	for nIndex = 1, nTotalLine, 1 do
		local strComponentID = "Group.Line" .. tostring(nIndex)
		CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_DEFAULT, strComponentID )
		--CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_DOWN, strComponentID )
		--CallFunction( WIDGET_ID_TOURNAMENT, "gotoAndPlay", WINNER_LINE_UP, strComponentID )
	end
	
	-- 3. 예외 이벤트( 조 내부 표현)
	if 15 == nGroupSize then
		SetString( WIDGET_ID_TOURNAMENT, "label", "Back", "Group.btnBack" )
		SetEvent( WIDGET_ID_TOURNAMENT, EVENT_BUTTON_CLICK, "Event_Tournament_btnBack", "", "Group.btnBack" )	
	end
	
end

function Tournament_SetVisible_Group_Highlight( strGroup, bVisibleBlue, bVisibleYellow )
	
	local strComponentID = "Group"..tostring
	SetBool( WIDGET_ID_TOURNAMENT, "visible", bVisibleBlue, "Group.G16_L1.Up" )
	SetBool( WIDGET_ID_TOURNAMENT, "visible", bVisibleYellow, "Group.G16_L1.Up" )
	
end


function Tournament_SetBettingInfo( llMinBettingSize, llMaxBettingSize, dwScriptID )
	
	g_tbTournament_BettingInfo["llMinBettingSize"] = llMinBettingSize
	g_tbTournament_BettingInfo["llMaxBettingSize"] = llMaxBettingSize
	g_tbTournament_BettingInfo["dwScriptID"] = dwScriptID
	
	
end

function Tournament_SetBettingGroupInfo( dwUserDBID, dwBettingGroup, llBettingCount, dwLeageID, fBettingRatio )
	
	g_tbTournament_BettingGroupInfo["dwUserDBID"] = dwUserDBID
	g_tbTournament_BettingGroupInfo["dwBettingGroup"] = dwBettingGroup
	g_tbTournament_BettingGroupInfo["llBettingCount"] = llBettingCount 
	g_tbTournament_BettingGroupInfo["dwLeageID"] = dwLeageID
	g_tbTournament_BettingGroupInfo["fBettingRatio"] = fBettingRatio 
	
	Tournament_Update()
end

-- Matching 입장 확인 UI
function Matching_ConfirmJoin ( _bShow, _nTime )
	-- MessageBox 호출;
	if ( _bShow ) then
		OpenMsgBox ( MSG_BOX_TYPE_OKCANCEL, string.format( GetGameInternalText("COMPETITION_PVE_ENREANCE_TEXT"), GetGameWordText("MATCHING_CONTENTS_NAME") ), Event_Matching_Confirm_Join_btnOk, Event_Matching_Confirm_Join_btnCancel, nil, nil, _nTime, Event_Matching_Confirm_Join_btnCancel, nil )
		
	else
		Event_MsgBox_Close()
	end
end

function Event_Matching_Confirm_Join_btnOk ()
	g_Competition:ConfirmJoin_OK()
	
	Event_MsgBox_Close()
end

function Event_Matching_Confirm_Join_btnCancel ()
	g_Competition:ConfirmJoin_Cancel()
	
	Event_MsgBox_Close()
end

-- 토너먼트가 시작되었는지 판단하는 함수 true : 시작, false : 시작안함
function Tournament_GetTournametStart()

	-- 먼저 토너먼트 정보가 있는지 판단
	if nil == g_tbTournament_GroupInfo  or 0 == table.getn(g_tbTournament_GroupInfo) then
		return false
	end
	
	-- 토너먼트 상태 파악 변수
	if nil ~= g_tbTournament_GroupInfo["ContentsStatus"] then
		if 3 == g_tbTournament_GroupInfo["ContentsStatus"] then
			g_Tournament_bGameStart = true -- EMMATCHING_CONTENTS_IN_PROGRESS 3
			return g_Tournament_bGameStart
		end
	end
	
	-- 추가 확인 로직
	-- 토너먼트 상태 파악(기존 구조가 이렇게 되어 있어서 다른 방식으로 시작 확인 불가능)
	
	-- 이미 토너먼트 시작 검사를 수행하여 결과가 있는 경우 아래 결과 생략
	if true == g_Tournament_bGameStart then 
		return g_Tournament_bGameStart
	end
	
	local nTournamentInfo	= g_tbTournament_GroupInfo["nTournamentInfo"]		-- 토너먼트 참가인원 정보 개수
	local tbTournamentInfo	= g_tbTournament_GroupInfo["tbTorunamentInfo"]		-- 토너먼트 참가인원 정보
	
	for i = 1, nTournamentInfo, 1 do
		local tbInfo = tbTournamentInfo[i]
		if nil ~= tbInfo then
			if 1 < tbInfo["dwPosition"] then	-- 1라운드 이상의 수가 있다면 시작된것임(기존 로직)
				g_Tournament_bGameStart = true
				break							-- 2라운드 진출자가 1명이라도 있다면 게임이 진행중임
			end
			
			g_Tournament_bGameStart = false
		end 
	end

	return g_Tournament_bGameStart
	
end

function Tournament_WatchingAndBetting()
	
	if true == Tournament_GetTournametStart() then
		-- 게임이 진행 중이라면 관전
		Tournament_ReqWatching()
	else
		-- 게임이 진행 중이지 않다면 배팅
		Tournament_ReqBetting()
	end
	
end

-- 관전 관련 로직 처리
function Tournament_ReqWatching()
	
	if nil == g_tbTournament_GroupInfo or 0 == g_tbTournament_GroupInfo["nTournamentInfo"]then return end
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText("ITEM_REBUILD_OKQUESTION", 0 ), Event_MsgBox_Close, Event_MsgBox_Close)
	
end

-- 관전 버튼 확인 메시지 박스 OK
function Event_Tournament_Watching_MsgBox_btnOk()
	
	local nGroupIndex = g_Tournament_nSelectGroup - g_Tournament_nDefaultUIOffset
	if nGroupIndex > g_tbTournament_GroupInfo["nTournamentInfo"] then
		return
	end
	
	local strGroupID = "G"..tostring(g_Tournament_nMatchingSize)
	local strComponentID = "Group." .. strGroupID .. "_"..tostring( g_Tournament_nSelectGroup )..".btn"
	local strSelectName = GetString( WIDGET_ID_TOURNAMENT, "label", strComponentID )
	
	if ""~= strSelectName and 16 > nGroupIndex then
		g_Msg:ReqObserveTournament( g_tbTournament_GroupInfo["ClientScriptID"], strSelectName )
	end
	Event_MsgBox_Close()
end

-- 관전 버튼 확인 메시지 박스 Cancel
function Event_Tournament_Watching_MsgBox_btnCancle()
	Event_MsgBox_Close()
	Tournament_Update()
end

-- 배팅관련 로직 처리
function Tournament_ReqBetting()
	
	-- 선택한 버튼에 해당하는 유저 정보가 있는지 확인
	if nil == g_tbTournament_GroupInfo or 0 == g_tbTournament_GroupInfo["nTournamentInfo"]then return end
	
	local nGroupIndex = g_Tournament_nSelectGroup - g_Tournament_nDefaultUIOffset
	if nGroupIndex > g_tbTournament_GroupInfo["nTournamentInfo"] then
		return
	end

	-- 배팅금액이 0이어야 UI가 동작
	if 0 == g_tbTournament_BettingGroupInfo["llBettingCount"] then
		local strMessage = StringFormat( GetGameInternalText("TOURNAMENT_BETTING_MESSAGEBOX"), strCharName, tostring(g_tbTournament_BettingInfo["llMinBettingSize"]), tostring(g_tbTournament_BettingInfo["llMaxBettingSize"]) )
		OpenMsgBox( MSG_BOX_TYPE_INPUT, strMessage, Event_MsgBox_Close, Event_MsgBox_Close)
	else
		local strMessage = "Cancel Betting"
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMessage, Event_MsgBox_Close, Event_MsgBox_Close)
	end	
	
		
	--케릭터 이름 획득
	local strComponentID = "Group.G" .. tostring(g_Tournament_nMatchingSize) .. "_" .. tostring(g_Tournament_nSelectGroup)..".btn"
	local strCharName = GetString( WIDGET_ID_TOURNAMENT, "label", strComponentID)
	
	local nEmptySum = 0			-- 이름 없는 공간 
	local nMaxIndex = g_tbTournament_GroupInfo["nTournamentInfo"]
	for i = 1, nMaxIndex, 1 do
		if strCharName == g_tbTournament_GroupInfo["tbTorunamentInfo"][i]["strGroupName"] then
			break
		end
		
		if "*****" == g_tbTournament_GroupInfo["tbTorunamentInfo"][i]["strGroupName"] then
			nEmptySum = nEmptySum + 1
		end
		 
	end
	
	nGroupIndex = nGroupIndex - nEmptySum
	if 0 > nGroupIndex then
		return
	end 

	--g_Msg:ReqReferCharCompetition( nGroupIndex, g_tbTournament_GroupInfo["ClientScriptID"] )
	
end

-- 배팅 버튼 확인 메시지 박스 OK
function Event_Tournament_Betting_MsgBox_btnOk()
	
	-- 1. 입력된 배팅 금액 확인
	local nMoney = tonumber(GetMsgBoxInput())
	if tonumber(g_tbTournament_BettingInfo["llMinBettingSize"]) > nMoney or tonumber(g_tbTournament_BettingInfo["llMaxBettingSize"]) < nMoney then 
		Event_MsgBox_Close()
		return
	end
	
	-- 2. 선택된 인덱스 정보 확인
	if nil == g_tbTournament_GroupInfo or 0 == g_tbTournament_GroupInfo["nTournamentInfo"]then return end
	
	
	local nGroupIndex = g_Tournament_nSelectGroup - g_Tournament_nDefaultUIOffset
	if nGroupIndex > g_tbTournament_GroupInfo["nTournamentInfo"] then
		Event_MsgBox_Close()
		return
	end
	local dwIndex = g_tbTournament_GroupInfo["tbTorunamentInfo"][nGroupIndex]["dwIndex"]
	if nil == dwIndex or -1 == dwIndex then
		Event_MsgBox_Close()
		return
	end
	
	-- 클라이언트로 요청
	g_Msg:ReqSetTournamentBettingMoney( dwIndex, GetMsgBoxInput(), g_tbTournament_GroupInfo["ClientScriptID"] )
	Event_MsgBox_Close()
	
end

-- 배팅 버튼 확인 메시지 박스 Cancel
function Event_Tournament_Betting_MsgBox_btnCancle()
	Event_MsgBox_Close()
end

-- 배팅 취소 확인 확인 버튼
function Event_Tournament_BettingCancel_MsgBox_btnOk()

	if nil == g_tbTournament_GroupInfo or 0 == g_tbTournament_GroupInfo["nTournamentInfo"]then return end
	
	local nGroupIndex = g_Tournament_nSelectGroup - g_Tournament_nDefaultUIOffset
	if nGroupIndex > g_tbTournament_GroupInfo["nTournamentInfo"] then
		return
	end
	
	local nGROUPID = g_tbTournament_GroupInfo["tbTorunamentInfo"][nGroupIndex]["nGROUPID"]
	if nil == nGROUPID then
		return
	end
	
	g_Msg:ReqCancelTournamentBettingMoney(nGroupIndex)
	Event_MsgBox_Close()
end


-- 배팅 취소 확인 취소 버튼
function Event_Tournament_BettingCancel_MsgBox_btnCancle()
	Event_MsgBox_Close()
end

--Event-------------------------------------------
function Event_Tournament_btnBack()
	
	g_Tournament_nMatchingSize = 0
	g_Tournament_nSelectGroup = 0
	Tournament_Update()
	
end

function Event_Tournament_btn()
	g_Tournament_nSelectGroup = 1
	Tournament_Update()
end

function Event_Tournament_btn1()
	
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 1
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end

function Event_Tournament_btn2()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 2
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 2
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn3()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 3
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 3
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn4()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 4
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 4
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn5()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 5
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 5
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn6()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 6
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 6
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn7()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 7
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 7
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn8()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 8
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 8
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn9()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 9
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 9
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn10()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 10
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 10
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn11()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 11
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 11
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn12()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 12
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 12
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn13()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 13
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 13
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn14()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 14
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 14
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn15()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 15
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 15
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn16()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 16
		Tournament_Update()
	else
		g_Tournament_nSelectGroup = 16
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn17()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 17
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn18()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 18
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn19()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 19
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn20()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 20
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn21()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 21
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn22()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 22
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn23()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 23
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn24()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 24
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn25()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 25
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn26()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 26
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn27()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 27
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn28()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 28
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn29()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 29
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn30()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 30
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn31()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 31
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end
function Event_Tournament_btn32()
	-- 이전에 이미 매칭 사이즈가 1이라면 관전이나 배팅 UI로 넘어 가야함
	if 1 ~= g_Tournament_nMatchingSize then
		g_Tournament_nMatchingSize = 1
		g_Tournament_nSelectGroup = 32
		Tournament_Update()
	else
		Tournament_WatchingAndBetting()
	end
end




