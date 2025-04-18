
g_nCompetition_CATEGORY_EXCEPTION = 0	-- 인던 시스템 관리 안되는 인던 (점령전)
g_nCompetition_CATEGORY_PVP = 1			-- PVP 인던
g_nCompetition_CATEGORY_PVE = 2			-- PVE 인던

g_nCompetition_TYPE_CTF = 0				-- 점령전
g_nCompetition_TYPE_TOURNAMENT = 1		-- 토너먼트
g_nCompetition_TYPE_CDM = 2				-- 클럽데스매치
g_nCompetition_TYPE_CTFLAG = 3			-- 깃발전
g_nCompetition_TYPE_UNLIMIT = 4			-- Trigger System 인던
g_nCompetition_TYPE_WAIL1 = 5			-- 돌파형 인던

-- CTF 랭커 개수
g_nCompetition_CTFRANKCOUNT = 10
-- CDM 랭커 개수
g_nCompetition_CDMRANKCOUNT = 8

-- 선택한 인던 타입
g_nCompetition_SelectType = -1

-- 인던 시스템 관리 안되는 인던 (점령전), 점령전은 PVP 에 속한다;
g_tbCompetition_ExceptInst = {}

-- Matching System 인던;
g_tbCompetition_Matching_PVPInst = {}
g_tbCompetition_Matching_PVEInst = {}

-- Trigger System 인던;
g_tbCompetition_PVEInst = {}

-- 인던 남은 시간
g_nCompetition_Hour = 0
g_nCompetition_Min = 0

-- 입장 확인 메시지 보이는 중?
g_bCompetition_ShowEntranceMsg = false
g_bCompetition_Open = false

-- 전장 로드 완료
function LoadCompetition()

	-- UI 닫기 이벤트
	SetEvent( WIDGET_ID_COMPETITION, EVENT_WIDGET_CLOSE, "CloseCompetition", "", "" )
	
	-- 트리 이벤트
	SetEvent( WIDGET_ID_COMPETITION, EVENT_LIST_ITEM_CLICK, "Ev_Competition_SelectTree", "index", "tree" )
	
	-- CDM/CTF 정보 visible끄기
	SetBool( WIDGET_ID_COMPETITION, "visible", false, "infoCDM" )
	SetBool( WIDGET_ID_COMPETITION, "visible", false, "infoCTF" )
	
	-- 다음 전투
	local str = GetGameWordText( "RNCOMPETITION_NEXT_WAR", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelNextBattle" )
	
	-- 랭크
	str = GetGameWordText( "RNCOMPETITION_RANK", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelRank" )
	
	-- 인스턴스 전장 리스트
	str = GetGameWordText( "RNCOMPETITION_INSTANCE_LIST", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelInst" )
	
	-- 목표
	str = GetGameWordText( "RNCOMPETITION_OBJECT", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelMission" )
	
	-- 상세 정보
	str = GetGameWordText( "RNCOMPETITION_MORE_INFO", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelMissionInfo" )
	
	-- 보상
	str = GetGameWordText( "RNCOMPETITION_REWARD", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelReward" )
	
	-- 정보 보기
	str = GetGameWordText( "COMPETITION_BUTTON_TEXT_INFO", 0 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnInfo" )
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_ShowInfo", "", "infoStage.btnInfo" )
	
	-- 인스턴스 전장 리스트 이벤트
	SetEvent( WIDGET_ID_COMPETITION, EVENT_LIST_INDEX_CHANGE, "Ev_Competition_SelectInstList", "index", "infoStage.listInst" )
	
	-- CTF 이전 정보
	-- 닫기
	str = GetGameWordText( "COMPETITION_TOPRANKING", 0 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoCTF.btnCancel" )
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_PrevInfoClose", "", "infoCTF.btnCancel" )
	-- 랭커 Top 10
	str = GetGameWordText( "COMPETITION_TOPRANKING", 1 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCTF.labelTitle" )
	-- 순위
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCTF.labelRank" )
	-- 학원
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 1 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCTF.labelSchool" )
	-- 클래스
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 2 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCTF.labelClass" )
	-- 캐릭터명
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 3 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCTF.labelName" )
	
	-- CDM 이전 정보
	-- 닫기
	str = GetGameWordText( "COMPETITION_TOPRANKING", 0 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoCDM.btnCancel" )
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_PrevInfoClose", "", "infoCDM.btnCancel" )
	-- 랭커 Top 10
	str = GetGameWordText( "COMPETITION_TOPRANKING", 1 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCDM.labelTitle" )
	-- 순위
	str = GetGameWordText( "CTF_RESULT_TEXT_BASE", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCDM.labelRank" )
	-- 클럽마크/명
	str = GetGameWordText( "RN_CDM_CLUB_NAME_TEXT", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCDM.labelClub" )
	-- 클럽마스터
	str = GetGameWordText( "RN_CDM_CLUB_MASTER_NAME_TEXT", 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCDM.labelName" )
	
	-- CDM 이전 정보 리스트 초기화
	for i = 1, g_nCompetition_CDMRANKCOUNT, 1 do
	
		local strInst = "infoCDM.item" .. tostring( i )
	
		SetString( WIDGET_ID_COMPETITION, "text", "", strInst .. ".tfClub" )
		SetString( WIDGET_ID_COMPETITION, "text", "", strInst .. ".tfName" )
		SetBool( WIDGET_ID_COMPETITION, "visible", false, strInst .. ".imgClub" )
	
	end
	
	-- 인던 트리 생성
	Competition_CreateTree()
	
	-- 점령전 정보 설정
	Competition_SetInfoExceptInst()

end

-- 전장 오픈
function OpenCompetition()

	g_bCompetition_Open = true
	
	Competition_SortInfoTable()
	
	Competition_UpdateCTF()
	Competition_UpdatePVP()
	Competition_UpdatePVE()
	local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "tree" )
	Ev_Competition_SelectTree( nIndex )

end

function CloseCompetition()

	g_bCompetition_Open = false
	
end

function IsOpenCompetition()

	return g_bCompetition_Open
	
end

-- 인던 트리 생성
function Competition_CreateTree()

	-- 인던 정보 받아와 분류별로 정리
	Competition_SortInfoTable()

	local tbTree = {}
	local strObjectID = CreateParam( "label", "depth", "likeFolder", "category", "infoID", "leaf" )
	
	local strPVP = GetGameInternalText( "RNCOMPETITION_MAIN_TYPE_PVP", 0 )
	local strPVE = GetGameInternalText( "RNCOMPETITION_MAIN_TYPE_PVE", 0 )
	
	-- 인던 시스템 관리 안되는 인던 (점령전) --------------
	local tbLeaf = {}
	if g_UserFeatures:IsUsingCaptureTheField() == true then
		
		table.insert( tbLeaf,
			{
				GetGameInternalText( g_tbCompetition_ExceptInst[ 2 ], 0 ),	-- label
				1,                                                          -- depth
				false,														-- likeFolder
				g_nCompetition_CATEGORY_EXCEPTION,							-- category
				"CTF",														-- infoID
				{}                                                          -- leaf
			} )
			
	end
	
	-- PVP ----------------------------------------------
	for value in pairs( g_tbCompetition_Matching_PVPInst ) do
	
		local tbPVPInfo = g_tbCompetition_Matching_PVPInst[ value ]
		
		local tbPVP = 
		{
			GetGameInternalText( value, 0 ),			-- label
			1,											-- depth
			false,										-- likeFolder
			g_nCompetition_CATEGORY_PVP,				-- category
			value,										-- infoID
			{}											-- leaf
		}
		
		table.insert( tbLeaf, tbPVP )
	
	end
	table.insert( tbTree, { strPVP, 0, true, -1, "", tbLeaf } )
	
	-- PVE ----------------------------------------------
	tbLeaf = {}
	for value in pairs( g_tbCompetition_Matching_PVEInst ) do
	
		local tbPVEInfo = g_tbCompetition_Matching_PVEInst[ value ]
		
		local tbPVE = 
		{
			GetGameInternalText( value, 0 ),			-- label
			1,											-- depth
			false,										-- likeFolder
			g_nCompetition_CATEGORY_PVE,				-- category
			value,										-- infoID
			{}											-- leaf
		}
		table.insert( tbLeaf, tbPVE )
	
	end
	
	for value in pairs( g_tbCompetition_PVEInst ) do
	
		local tbPVEInfo = g_tbCompetition_PVEInst[ value ]
		local tbPVE = 
		{
			GetGameInternalText( value, 0 ),			-- label
			1,                                          -- depth
			false,                                      -- likeFolder
			g_nCompetition_CATEGORY_PVE,                -- category
			value,                                      -- infoID
			{}                                          -- leaf
		}
		
		table.insert( tbLeaf, tbPVE )
		
	end
	table.insert( tbTree, { strPVE, 0, true, -1, "", tbLeaf } )
	
	SetDataComplex( WIDGET_ID_COMPETITION, strObjectID, tbTree, "tree", false )

end

-- 인던 정보 받아와 분류 별로 정리한다.
function Competition_SortInfoTable()

	-- 점령전 하나밖에 없다.
	g_tbCompetition_ExceptInst = g_Competition:GetExceptionPvPInstanceList()
	
	-- PVP
	Competition_SortInfoTable_MatchingPVP()
	
	-- PVE
	Competition_SortInfoTable_MatchingPVE()
	Competition_SortInfoTablePVE()

end

-- Matching PVP 인던 정보 받아와 분류 별로 정리한다.
function Competition_SortInfoTable_MatchingPVP()

	g_tbCompetition_Matching_PVPInst = {}

	local tbPVPList = g_Competition:GetMatchingInstanceList( 0 )
	
	for value in pairs( tbPVPList ) do
	
		local tbInfo = tbPVPList[ value ]
	
		local tbInfoList = nil
		if nil == g_tbCompetition_Matching_PVPInst[ tbInfo[ 2 ] ] then
		
			tbInfoList = {}
		
		else
		
			tbInfoList = g_tbCompetition_Matching_PVPInst[ tbInfo[ 2 ] ]
		
		end
		
		table.insert( tbInfoList, tbInfo )
		g_tbCompetition_Matching_PVPInst[ tbInfo[ 2 ] ] = tbInfoList
	
	end

end

-- Matching PVE 인던 정보 받아와 분류 별로 정리한다.
function Competition_SortInfoTable_MatchingPVE()

	g_tbCompetition_Matching_PVEInst = {}

	local tbPVEList = g_Competition:GetMatchingInstanceList( 1 )
	
	for value in pairs( tbPVEList ) do
	
		local tbInfo = tbPVEList[ value ]
	
		local tbInfoList = nil
		if nil == g_tbCompetition_Matching_PVEInst[ tbInfo[ 2 ] ] then
		
			tbInfoList = {}
		
		else
		
			tbInfoList = g_tbCompetition_Matching_PVEInst[ tbInfo[ 2 ] ]
		
		end
		
		table.insert( tbInfoList, tbInfo )
		g_tbCompetition_Matching_PVEInst[ tbInfo[ 2 ] ] = tbInfoList
	
	end

end

-- PVE 인던 정보 받아와 분류 별로 정리한다.
function Competition_SortInfoTablePVE()

	g_tbCompetition_PVEInst = {}

	local tbPVEList = g_Competition:GetPvEInstanceList()
	
	for value in pairs( tbPVEList ) do
	
		local tbInfo = tbPVEList[ value ]
	
		local tbInfoList = nil
		if nil == g_tbCompetition_PVEInst[ tbInfo[ 2 ] ] then
		
			tbInfoList = {}
		
		else
		
			tbInfoList = g_tbCompetition_PVEInst[ tbInfo[ 2 ] ]
		
		end
		
		table.insert( tbInfoList, tbInfo )
		g_tbCompetition_PVEInst[ tbInfo[ 2 ] ] = tbInfoList
	
	end

end

-- Matching PVP 트리 선택
function Competition_Select_MatchingPVPTree( nIndex )

	-- 인던 정보 ID
	local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "tree" )
	if 0 == string.len( strInfoID ) then
		return
	end
	
	-- 없으면 종료
	local tbInfoList = g_tbCompetition_Matching_PVPInst[ strInfoID ]
	if nil == tbInfoList then
		return
	end
	
	-- 인스턴스 전장 리스트 설정
	local tbListData = {}
	for value in pairs( tbInfoList ) do
	
		local tbInfo = tbInfoList[ value ]
		local str = GetGameInternalText( tbInfo[ 3 ], 0 )
		if ( tbInfo[ 27 ] > 0 ) then
			str = string.format( "%s(%d)",  str, tbInfo[ 26 ] )
		end
	
		local tbData = 
		{
			str,
			g_nCompetition_CATEGORY_PVP,
			strInfoID,
			value
		}
		
		table.insert( tbListData, tbData )
	
	end
	
	local strParam = CreateParam( "label", "category", "infoID", "infoIndex" )
	SetDataEx( WIDGET_ID_COMPETITION, strParam, tbListData, "infoStage.listInst", false )
	
	-- 0번 인덱스 선택
	if 0 ~= GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" ) then
	
		SetInt( WIDGET_ID_COMPETITION, "selectedIndex", 0, "infoStage.listInst" )
	
	-- 현재 선택된게 0번이면 이벤트 호출 안된다. 직접 업데이트 하자
	else
	
		Competition_SetInfo_Matching( strInfoID, 1 )
	
	end

end

-- Matching PVE 트리 선택
function Competition_Select_MatchingPVETree( nIndex )

	-- 인던 정보 ID
	local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "tree" )
	if 0 == string.len( strInfoID ) then
		return
	end
	
	-- 없으면 종료
	local tbInfoList = g_tbCompetition_Matching_PVEInst[ strInfoID ]
	if nil == tbInfoList then
		return
	end
	
	-- 인스턴스 전장 리스트 설정
	local tbListData = {}
	for value in pairs( tbInfoList ) do
	
		local tbInfo = tbInfoList[ value ]
		local str = GetGameInternalText( tbInfo[ 3 ], 0 )
		if ( tbInfo[ 27 ] > 0 ) then
			str = string.format( "%s(%d)",  str, tbInfo[ 26 ] )
		end
	
		local tbData = 
		{
			str,
			g_nCompetition_CATEGORY_PVE,
			strInfoID,
			value
		}
		
		table.insert( tbListData, tbData )
	
	end
	
	local strParam = CreateParam( "label", "category", "infoID", "infoIndex" )
	SetDataEx( WIDGET_ID_COMPETITION, strParam, tbListData, "infoStage.listInst", false )
	
	-- 0번 인덱스 선택
	if 0 ~= GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" ) then
	
		SetInt( WIDGET_ID_COMPETITION, "selectedIndex", 0, "infoStage.listInst" )
	
	-- 현재 선택된게 0번이면 이벤트 호출 안된다. 직접 업데이트 하자
	else
	
		Competition_SetInfo_Matching( strInfoID, 1 )
	
	end

end

-- Trigger System PVE 트리 선택
function Competition_SelectPVETree( nIndex )

	-- 인던 정보 ID
	local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "tree" )
	if 0 == string.len( strInfoID ) then
		return
	end
	
	-- 없으면 종료
	local tbInfoList = g_tbCompetition_PVEInst[ strInfoID ]
	if nil == tbInfoList then
		return
	end
	
	local nListIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" )
	
	-- 인스턴스 전장 리스트 설정
	local tbListData = {}
	for value in pairs( tbInfoList ) do
	
		local tbInfo = tbInfoList[ value ]
	
		local bEnable = true
		local nCount = 0
		local nTime = 0
		
		local str = ""
		for value in pairs( tbInfo[ 11 ] ) do

			local tbLevelInfo = tbInfo[ 11 ][ value ]
			
			local nGrade = 21 + tbLevelInfo[ 1 ]
			str = GetGameWordText( "COMPETITION_PVE", nGrade ) .. " "
			
			if false == tbLevelInfo[ 4 ] then
			
				bEnable = false
				
			end
			
			nCount = nCount + tbLevelInfo[ 5 ]
			nTime = nTime + tbLevelInfo[ 6 ]
	
		end
		
		local strColor = HTML_TEXT_COLOR_WHITE
		if false == bEnable then
			
			strColor = HTML_TEXT_COLOR_RED
			
		else
		
			strColor = HTML_TEXT_COLOR_WHITE
			
		end
		
		str = str .. GetGameWordText( tbInfo[ 3 ], 0 )
		
		if 0 ~= nTime then
		
			str = str .. "(" .. GetGameWordText( "COMPETITION_PVE", 24 ) .. " " .. 
				math.floor( nTime / 60 ) .. GetGameWordText( "ITEM_BASIC_TIME", 1 ) .. " " ..
				math.floor( nTime % 60 ) .. GetGameWordText( "ITEM_BASIC_TIME", 2 ) .. ")"
			
			local tbQuickSet = 
			{
				time = 1000,
				delay = 0,
				ease = EASE_NONE,
				loop = false,
				onComplete = "Ev_Competition_BindOffTimer"
			}
			SetTween( WIDGET_ID_COMPETITION, tbQuickSet, "", "", "infoStage.listInst" )
		
		end
		
		str = str .. StringFormat( GetGameInternalText( "WAIL_OF_THE_BUILDING_REMTODAY_TEXT", 0 ), tbInfo[ 7 ] - nCount )
	
		local tbData = 
		{
			HTMLFontColor( str, strColor ),
			g_nCompetition_CATEGORY_PVE,
			strInfoID,
			value
		}
		
		table.insert( tbListData, tbData )
	
	end
	
	local strParam = CreateParam( "label", "category", "infoID", "infoIndex" )
	SetDataEx( WIDGET_ID_COMPETITION, strParam, tbListData, "infoStage.listInst", false )
	
	Competition_SetInfoPVE( strInfoID, nListIndex + 1 )

end

-- 인던 이미지 설정
function Competition_SetInstImg( nID )

	-- 서버 인던 관련 스크립트에서 맵 ID 설정한다. 맞춰서 맵 이미지 설정하기
	if 180 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "CDM", "infoStage.img" )
	
	elseif 186 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "Tournament", "infoStage.img" )
		
	elseif 189 == nID or 190 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "CTFlag", "infoStage.img" )
		
	elseif 233 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "WailBuilding", "infoStage.img" )
	
	elseif 245 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "ExternalBuilding", "infoStage.img" )
	
	elseif 246 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "LimitDesert", "infoStage.img" )
		
	elseif 261 == nID then
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "DimensionLaboratory", "infoStage.img" )
		
	else
	
		CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "NONE", "infoStage.img" )
	
	end

end

-- 인던 시스템 관리 안되는 인던 정보 설정
function Competition_SetInfoExceptInst()
	
	-- CTF가 비활성화인 경우 CDM을 보여준다
	if g_UserFeatures:IsUsingCaptureTheField() == false then
		Competition_SetInfo_Matching("RNCOMPETITION_SUB_TYPE_CDM", 1)
		return
	end

	g_nCompetition_SelectType = g_nCompetition_TYPE_CTF

	-- 점령전 이미지 설정
	CallFunction( WIDGET_ID_COMPETITION, "gotoAndPlay", "CTF", "infoStage.img" )

	-- 점령전 하나밖에 없음.
	local tbInfo = g_tbCompetition_ExceptInst
	
	-- 맵 이름
	SetString( WIDGET_ID_COMPETITION, "text", tbInfo[ 6 ], "infoStage.labelName" )
	
	-- 목표
	local str = StringFormat( GetGameInternalText( tbInfo[ 3 ], 0 ), 3 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.tfMission" )
	
	-- 상세 정보
	local strVS = " " .. GetGameWordText( tbInfo[ 4 ], 3 ) .. " "
	str = -- 대진
		GetGameWordText( tbInfo[ 4 ], 2 ) .. " " .. 
		HTMLImage( "PARTY_INFO_SCHOOL_SM", 17, 12 ) .. strVS ..
		HTMLImage( "PARTY_INFO_SCHOOL_HA", 17, 12 ) .. strVS ..
		HTMLImage( "PARTY_INFO_SCHOOL_BH", 17, 12 ) .. "\n" ..	
		-- 진행 인원
		GetGameWordText( tbInfo[ 4 ], 4 ) .. " " .. 
		StringFormat( GetGameWordText( "COMPETITION_CTF_BATTLE_INFO_PLAYERS", 0 ), tbInfo[ 11 ] ) .. "\n" .. 
		-- 참가 레벨
		GetGameWordText( tbInfo[ 4 ], 5 ) .. " " .. 
		StringFormat( GetGameWordText( "COMPETITION_CTF_BATTLE_INFO_MAXLEVEL", 0 ), tbInfo[ 12 ] )
	
	local tbConsecutivelyInfo = g_Competition:GetCTFConsecutivelyCaptureSchool()
	if tbConsecutivelyInfo ~= nil then
		if tbConsecutivelyInfo[1] == g_MyChar:GetSchool() then
			str = str.."\n"..
				HTMLFontColor( string.format( GetGameInternalText( "CAPTURETHEFIELD_CONSECUTIVELY", 0 ), tostring(tbConsecutivelyInfo[2])) , HTML_TEXT_COLOR_RED )
			SetBool( WIDGET_ID_COMPETITION, "enabled", false, "infoStage.btnOK" )
		end
	end
	
	SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.tfMissionInfo" )
	
	
	-- 보상
	str = -- 점령자
		GetGameWordText( tbInfo[ 5 ], 1 ) .. 
		GetGameWordText( "COMPETITION_CTF_BATTLE_AWARD_TYPE", 1 ) .. ", " .. 
		StringFormat( GetGameWordText( "COMPETITION_CTF_BATTLE_AWARD_TYPE", 3 ), 4 ) .. "\n" .. 
		--비 점령자
		GetGameWordText( tbInfo[ 5 ], 2 ) .. 
		GetGameWordText( "COMPETITION_CTF_BATTLE_AWARD_TYPE", 1 )
	str = HTMLFontColor( str, HTML_TEXT_COLOR_GREENYELLOW )
	SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.tfReward" )
	
	-- 시간
	-- 상태 : 시작 안함, 준비
	if 0 == tbInfo[ 13 ] or 1 == tbInfo[ 13 ] then
	
		str = string.format( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 8 ), tbInfo[ 23 ], tbInfo[ 24 ] )
		
		g_nCompetition_Hour = tbInfo[ 23 ]
		g_nCompetition_Min = tbInfo[ 24 ]
		
		-- 시간 타이머 설정
		local tbQuickSet = 
		{
			time = tbInfo[ 25 ] * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_Competition_Timer"
		}
		SetTween( WIDGET_ID_COMPETITION, tbQuickSet, "", "", "infoStage.labelBettleTime" )
	
	-- 진행중
	else
	
		str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 )
	
	end
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelBettleTime" )
	
	-- 참여 조건 안됨
	if ( true == tbInfo[ 18 ] ) or ( 1 == g_Client:IsInstantMap() ) then
	
		-- 신청 불가
		str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 3 ), HTML_TEXT_COLOR_RED )
		SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
		
		-- 참여하기
		str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 0 )
		SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
		
		SetBool( WIDGET_ID_COMPETITION, "enabled", false, "infoStage.btnOK" )
	
	else
	
		-- 상태 : 준비, 시작
		if	1 == tbInfo[ 13 ] or
			2 == tbInfo[ 13 ] then
			
			if true == tbInfo[ 20 ] then
			
				-- 입장 대기
				str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 5 ), HTML_TEXT_COLOR_ORANGE )
				SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
				
				-- 취소하기 
				str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 1 )
				SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
				SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_JoinCancel", "", "infoStage.btnOK" )
			
			else
			
				-- CTF 진행중 아님
				if 0 == tbInfo[ 8 ] then
				
					-- 신청 가능
					str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 6 ), HTML_TEXT_COLOR_GREENYELLOW )
					SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
					
					-- 참여하기
					str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 0 )
					SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
					SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_Join", "", "infoStage.btnOK" )
				
				-- 진행중
				else
				
					-- 참여중
					str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 4 ), HTML_TEXT_COLOR_WHITE )
					SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
					
					-- 나가기
					str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 2 )
					SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
					SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_Exit", "", "infoStage.btnOK" )
				
				end
			
			end
			local btnEnable = true
			local tbConsecutivelyInfo = g_Competition:GetCTFConsecutivelyCaptureSchool()
			if tbConsecutivelyInfo ~= nil then
				if tbConsecutivelyInfo[1] == g_MyChar:GetSchool() then
					btnEnable = false	
				end
			end
			SetBool( WIDGET_ID_COMPETITION, "enabled", btnEnable, "infoStage.btnOK" )
			
		else
		
			-- 신청 불가
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 3 ), HTML_TEXT_COLOR_RED )
			SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
			
			-- 참여하기
			str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 0 )
			SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
			
			SetBool( WIDGET_ID_COMPETITION, "enabled", false, "infoStage.btnOK" )
			
		end
	
	end
	
	-- 인던 리스트
	local tbListData = {}
	table.insert( tbListData, 
		{
			tbInfo[ 6 ],
			g_nCompetition_CATEGORY_EXCEPTION,
			"",
			1
		} )
	
	local strParam = CreateParam( "label", "category", "infoID", "infoIndex" )
	SetDataEx( WIDGET_ID_COMPETITION, strParam, tbListData, "infoStage.listInst", false )
	SetInt( WIDGET_ID_COMPETITION, "selectedIndex", 0, "infoStage.listInst" )
	
	-- 이전 CTF 시간 정보 설정
	if 	0 == tbInfo[ 27 ] and
		0 == tbInfo[ 28 ] and
		0 == tbInfo[ 29 ] and
		0 == tbInfo[ 30 ] then
		
		str = "(--:-- ~ --:--)"
		
	else
	
		str = string.format( "(%02d:%02d ~ %02d:%02d)", 
			tbInfo[ 27 ], tbInfo[ 28 ], tbInfo[ 29 ], tbInfo[ 30 ] )
	
	end
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoCTF.labelTime" )
	
	-- 이전 CTF 정보
	local tbListData = {}
	for i = 1, g_nCompetition_CTFRANKCOUNT, 1 do
	
		local strInst = "infoCTF.item" .. tostring( i )
	
		local tbRanker = tbInfo[ 26 ][ i ]
		if nil == tbRanker then
		
			SetBool( WIDGET_ID_COMPETITION, "visible", false, strInst .. ".textField" )
			SetBool( WIDGET_ID_COMPETITION, "visible", false, strInst .. ".imgSchool" )
			SetBool( WIDGET_ID_COMPETITION, "visible", false, strInst .. ".tfClass" )
			SetBool( WIDGET_ID_COMPETITION, "visible", false, strInst .. ".tfName" )
		
		else
		
			-- 리스트 데이터 추가
			table.insert( tbListData, { tostring( i ) } )
		
			SetBool( WIDGET_ID_COMPETITION, "visible", true, strInst .. ".textField" )
			SetBool( WIDGET_ID_COMPETITION, "visible", true, strInst .. ".imgSchool" )
			SetBool( WIDGET_ID_COMPETITION, "visible", true, strInst .. ".tfClass" )
			SetBool( WIDGET_ID_COMPETITION, "visible", true, strInst .. ".tfName" )
		
			-- 학원
			if 0 == tbRanker[ 1 ] then
			
				CallFunction( WIDGET_ID_COMPETITION, "gotoAndStop", "SCHOOL0", strInst .. ".imgSchool" )
			
			elseif 1 == tbRanker[ 1 ] then
			
				CallFunction( WIDGET_ID_COMPETITION, "gotoAndStop", "SCHOOL1", strInst .. ".imgSchool" )
			
			elseif 2 == tbRanker[ 1 ] then
			
				CallFunction( WIDGET_ID_COMPETITION, "gotoAndStop", "SCHOOL2", strInst .. ".imgSchool" )
			
			else
			
				CallFunction( WIDGET_ID_COMPETITION, "gotoAndStop", "NONE", strInst .. ".imgSchool" )
			
			end
			
			-- 클래스 마크
			str = ConvertFrameToClass( CharClassToCharclassIndex( tbRanker[ 2 ] ) )
			CallListItemFunc( WIDGET_ID_COMPETITION, i-1, "tfClass", "gotoAndStop", str, "infoCTF.list" )
			
			-- 캐릭터명
			SetString( WIDGET_ID_COMPETITION, "text", tbRanker[ 3 ], strInst .. ".tfName" )
		
		end
	
	end
	SetDataEx( WIDGET_ID_COMPETITION, "label", tbListData, "infoCTF.list", false )
	
	-- 닫기
	str = GetGameWordText( "COMPETITION_WINDOW", 1 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnCancel" )
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_Close", "", "infoStage.btnCancel" )
	
	SetBool( WIDGET_ID_COMPETITION, "enabled", true, "infoStage.btnCancel" )
	
	-- 정보보기 활성화
	SetBool( WIDGET_ID_COMPETITION, "enabled", true, "infoStage.btnInfo" )

end

-- PVP 인던 정보 설정
function Competition_SetInfo_Matching( strInfoID, nInfoIndex )

	-- 인던 정보 리스트 없으면 종료
	local tbInfoList = g_tbCompetition_Matching_PVPInst[ strInfoID ]
	if nil == tbInfoList then
		tbInfoList = g_tbCompetition_Matching_PVEInst[ strInfoID ]
		if ( nil == tbInfoList ) then
			return
		end
	end
	
	-- 인던 정보 없으면 종료
	local tbInfo = tbInfoList[ nInfoIndex ]
	if nil == tbInfo then
		return
	end
	
	-- 선택 인던을 갱신한다
	g_Competition:SelectInstanceDungeon( tbInfo[ 16 ], tbInfo[ 15 ] )
	
	-- 인던이 분류별로 정확하게 구분되어 있지가 않다.
	-- 그냥 인던 분류 텍스트 XML ID로 구분하자.
	if tbInfo[ 2 ] == "RNCOMPETITION_SUB_TYPE_CDM" then
	
		g_nCompetition_SelectType = g_nCompetition_TYPE_CDM
	
	elseif tbInfo[ 2 ] == "RNCOMPETITION_SUB_TYPE_CTFLAG" then
	
		g_nCompetition_SelectType = g_nCompetition_TYPE_CTFLAG
	
	elseif tbInfo[ 2 ] == "RNC_SUB_TYPE_TOURNAMENT" then
	
		g_nCompetition_SelectType = g_nCompetition_TYPE_TOURNAMENT
		
	elseif tbInfo[ 2 ] == "RNCOMPETITION_SUB_TYPE_WAIL_1" then

		g_nCompetition_SelectType = g_nCompetition_TYPE_WAIL1
	
	end
	
	-- 정보보기 버튼 사용 여부
	SetBool( WIDGET_ID_COMPETITION, "enabled", tbInfo[ 25 ], "infoStage.btnInfo" )
	
	-- 맵 이미지 설정
	Competition_SetInstImg( tbInfo[ 7 ] )
	
	-- 맵 이름
	local str = GetGameInternalText( tbInfo[ 3 ], 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelName" )
	
	-- 목표
	str = GetGameInternalText( tbInfo[ 4 ], 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.tfMission" )
	
	-- 상세 설명
	str = GetGameInternalText( tbInfo[ 5 ], 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.tfMissionInfo" )
	
	-- 보상
	str = HTMLFontColor( GetGameInternalText( tbInfo[ 6 ], 0 ), HTML_TEXT_COLOR_GREENYELLOW )
	SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.tfReward" )
	
	-- 시간
	if -1 ~= tbInfo[ 24 ] then
	
		-- 1일 이상 남은 경우 날짜 표시
		if 1 < tbInfo[ 22 ] then
		
			str = StringFormat( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 9 ), tbInfo[ 22 ] )
		
		-- 시간 표시
		else
		
			g_nCompetition_Hour = math.floor( tbInfo[ 23 ] / 60 )
			g_nCompetition_Min = tbInfo[ 23 ] % 60
			
			str = string.format( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 8 ), 
				g_nCompetition_Hour, g_nCompetition_Min )
			
			-- 시간 타이머 설정
			local tbQuickSet = 
			{
				time = 60000,
				delay = 0,
				ease = EASE_NONE,
				loop = false,
				onComplete = "Ev_Competition_Timer"
			}
			SetTween( WIDGET_ID_COMPETITION, tbQuickSet, "", "", "infoStage.labelBettleTime" )
		
		end
	
	-- 진행중
	else
	
		str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 )
	
	end
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelBettleTime" )
	
	-- 신청 상태
	SetBool( WIDGET_ID_COMPETITION, "enabled", true, "infoStage.btnOK" )
	
	local nStatus = tbInfo[ 15 ]	
	if 0 ~= nStatus then
	
		-- 참여중
		if ( tbInfo[ 16 ] == tbInfo[ 13 ] ) then
			
			-- 등록됨
			if 1 == nStatus then
			
				str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 5 ), HTML_TEXT_COLOR_ORANGE )
				SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
				
				str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 1 )
				SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
				
			-- 플레이 중
			elseif 2 == nStatus then
			
				str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 ), HTML_TEXT_COLOR_WHITE )
				SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
				
				-- 인던 타입 : 재입장 가능
				if 2 == tbInfo[ 8 ] then
				
					str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 11 )
					SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
				
				-- 그 외
				else
				
					-- 참여하기
					str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 0 )
					SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
				
				end
			
			end
			
		-- 참여안함
		else
		
			-- 인던 상태 : 신청 가능(신청 가능해도 참여 안할 경우 불가), 신청 불가
			if 0 == tbInfo[ 9 ] or 1 == tbInfo[ 9 ] then
			
				str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 3 ), HTML_TEXT_COLOR_RED )
				SetBool( WIDGET_ID_COMPETITION, "enabled", false, "infoStage.btnOK" )
			
			-- 인던 상태 : 입장 대기
			elseif 2 == tbInfo[ 9 ] then
			
				str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 5 ), HTML_TEXT_COLOR_WHITE )
			
			-- 인던 상태 : 진행 중
			elseif 3 == tbInfo[ 9 ] then
			
				str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 ), HTML_TEXT_COLOR_WHITE )
			
			end
			SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
			
			-- 참여하기
			str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 0 )
			SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
		
		end
	
	else
	
		-- 참여하기
		str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 0 )
		SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
		
		-- 인던 상태 : 신청 가능
		if 0 == tbInfo[ 9 ] then
		
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 6 ), HTML_TEXT_COLOR_GREENYELLOW )
		
		-- 인던 상태 : 신청 불가
		elseif 1 == tbInfo[ 9 ] then
		
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 3 ), HTML_TEXT_COLOR_RED )
		
		-- 인던 상태 : 입장 대기
		elseif 2 == tbInfo[ 9 ] then
		
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 5 ), HTML_TEXT_COLOR_WHITE )
		
		-- 인던 상태 : 진행 중
		elseif 3 == tbInfo[ 9 ] then
		
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 ), HTML_TEXT_COLOR_WHITE )
		
		end
		
		-- 재도전이 가능한 경우에는 버튼을 재도전으로 바꾼다
		if ( true == tbInfo[ 28 ] ) then
	
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 ), HTML_TEXT_COLOR_WHITE )
			
		end
		
		-- 인던안에 있을 경우 무조건 신청불가상태가 된다
		if ( 1 == g_Client:IsInstantMap() ) then
			
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 3 ), HTML_TEXT_COLOR_RED )
			
		end
		
		SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
		
	end
	
	-- 참여/취소 등 이벤트
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_Join", "", "infoStage.btnOK" )
	
	-- 닫기
	str = GetGameWordText( "COMPETITION_WINDOW", 1 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnCancel" )
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_Close", "", "infoStage.btnCancel" )
	
	local btnEnable = true
		
	-- 인던안에 있을경우 비활성화시킨다
	if ( 1 == g_Client:IsInstantMap() ) then
		
		btnEnable = false
		
	-- 재입장이 가능할 경우 활성화시킨다
	elseif ( true == tbInfo[ 28 ] ) then
		
		btnEnable = true
		
		-- 버튼을 재입장으로 바꾼다
		local strOKBtn = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 11 )
		SetString( WIDGET_ID_COMPETITION, "label", strOKBtn, "infoStage.btnOK" )
		
	-- 입장횟수가 0일 경우 비활성화시킨다
	elseif ( tbInfo[ 27 ] > 0 ) and ( tbInfo[ 26 ] <= 0 ) then
	
		btnEnable = false
		
	-- 신청불가 상태일 경우 비활성화시킨다 ( 취소하기 상태일 경우는 활성화시킨다 )
	elseif ( 1 == tbInfo[ 9 ] ) and ( nStatus ~= 1 ) then
		
		btnEnable = false
			
	else
		
		if ( g_nCompetition_TYPE_CTFLAG == g_nCompetition_SelectType ) then
		
			local tbConsecutivelyInfo = g_Competition:GetCTFConsecutivelyCaptureSchool()
			if tbConsecutivelyInfo ~= nil then
				
				if tbConsecutivelyInfo[1] == g_MyChar:GetSchool() then
					btnEnable = false	
				end
					
			end
				
		end
		
	end

	SetBool( WIDGET_ID_COMPETITION, "enabled", btnEnable, "infoStage.btnOK" )
	SetBool( WIDGET_ID_COMPETITION, "enabled", true, "infoStage.btnCancel" )
	
end

-- PVE 인던 정보 설정
function Competition_SetInfoPVE( strInfoID, nInfoIndex )

	-- 인던 정보 리스트 없으면 종료
	local tbInfoList = g_tbCompetition_PVEInst[ strInfoID ]
	if nil == tbInfoList then
		return
	end
	
	-- 인던 정보 없으면 종료
	local tbInfo = tbInfoList[ nInfoIndex ]
	if nil == tbInfo then
		return
	end
	
	-- 인던이 분류별로 정확하게 구분되어 있지가 않다.
	-- 그냥 인던 분류 텍스트 XML ID로 구분하자.
	if tbInfo[ 2 ] == "RNCOMPETITION_SUB_TYPE_WAIL" then
	
		g_nCompetition_SelectType = g_nCompetition_TYPE_UNLIMIT
	
	end
	
	-- 정보보기 비활성화
	SetBool( WIDGET_ID_COMPETITION, "enabled", false, "infoStage.btnInfo" )
	
	-- 맵 이미지 설정
	Competition_SetInstImg( tbInfo[ 12 ] )
	
	-- 맵 이름
	local str = GetGameWordText( tbInfo[ 3 ], 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelName" )
	
	-- 목표
	str = GetGameInternalText( tbInfo[ 4 ], 0 )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.tfMission" )
	
	-- 상세 설명
	for value in pairs( tbInfo[ 11 ] ) do

		local tbLevelInfo = tbInfo[ 11 ][ value ]
		
		str = StringFormat( GetGameInternalText( tbInfo[ 5 ], 0 ),
			tbInfo[ 7 ], "", tbInfo[ 9 ], tbLevelInfo[ 2 ], tbLevelInfo[ 3 ] )
	
	end
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.tfMissionInfo" )
	
	-- 보상
	str = HTMLFontColor( GetGameInternalText( tbInfo[ 6 ], 0 ), HTML_TEXT_COLOR_GREENYELLOW )
	SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.tfReward" )
	
	-- 시간 텍스트 제거
	SetString( WIDGET_ID_COMPETITION, "text", "", "infoStage.labelBettleTime" )
	
	-- 시간 타이머 제거
	local tbQuickSet = 
	{
		time = 0,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_COMPETITION, tbQuickSet, "", "", "infoStage.labelBettleTime" )
	
	-- 입장
	str = GetGameWordText( "COMPETITION_PVE", 0 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnOK" )
	
	local bEnable = true
	local bRejoin = false
	
	for value in pairs( tbInfo[ 11 ] ) do

		local tbLevelInfo = tbInfo[ 11 ][ value ]
		if ( false == tbLevelInfo[ 4 ] ) or ( 1 == g_Client:IsInstantMap() ) then
		
			bEnable = false
			
		elseif 0 ~= tbLevelInfo[ 6 ] then
		
			bRejoin = true
			
		end

	end
	
	-- 신청 가능
	if true == bEnable then
	
		if ( true == bRejoin ) then
		
			local strBtnOK = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 11 )
			SetString( WIDGET_ID_COMPETITION, "label", strBtnOK, "infoStage.btnOK" )
			
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 7 ), HTML_TEXT_COLOR_GREENYELLOW )
			
		else
		
			local strBtnOK = GetGameWordText( "COMPETITION_PVE", 0 )
			SetString( WIDGET_ID_COMPETITION, "label", strBtnOK, "infoStage.btnOK" )
		
			str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 6 ), HTML_TEXT_COLOR_GREENYELLOW )
			
		end
		
	-- 신청 불가
	else
	
		str = HTMLFontColor( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 3 ), HTML_TEXT_COLOR_RED )
	
	end
	SetString( WIDGET_ID_COMPETITION, "htmlText", str, "infoStage.labelAvailable" )
	
	-- 귀속해제
	str = GetGameWordText( "COMPETITION_PVE", 1 )
	SetString( WIDGET_ID_COMPETITION, "label", str, "infoStage.btnCancel" )
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_BindOff", "", "infoStage.btnCancel" )
	
	-- 입장
	SetEvent( WIDGET_ID_COMPETITION, EVENT_BUTTON_CLICK, "Ev_Competition_Join", "", "infoStage.btnOK" )
	
	SetBool( WIDGET_ID_COMPETITION, "enabled", bEnable, "infoStage.btnOK" )
	SetBool( WIDGET_ID_COMPETITION, "enabled", bEnable, "infoStage.btnCancel" )

end

-- PVE 입장 조건 검사
function Competition_CheckConditionPVE( count, tbLevelInfo )

	local nCount = 0
	local nMaxLevel = 0
	local nMinLevel = 99999
	
	-- 입장 조건 체크
	for i = 1, PARTY_MAX, 1 do
	
		local tbPartyMember = g_Party:GetMember( i - 1 )
		if nil == tbPartyMember[ "dwGaeaID" ] then
			break
		end
		
		nCount = nCount + 1
		
		if tbPartyMember[ "nLevel" ] < nMinLevel then
		
			nMinLevel = tbPartyMember[ "nLevel" ]
			
		end
		
		if tbPartyMember[ "nLevel" ] > nMaxLevel then
		
			nMaxLevel = tbPartyMember[ "nLevel" ]
		
		end
	
	end
	
	-- 인원수 부족
	if nCount < count then
	
		return false
	
	else
	
		-- 입장 가능 여부 체크
		for value in pairs( tbLevelInfo ) do
		
			local tbLevel = tbLevelInfo[ value ]
			if 	nMaxLevel > tbLevel[ 3 ] or
				nMinLevel < tbLevel[ 2 ] then
			
				return false
				
			end
		
		end
	
	end
	
	return true

end

------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- CTF 정보 업데이트
function Competition_UpdateCTF()

	if false == IsLoadWidget( WIDGET_ID_COMPETITION ) then
		return
	end
	
	-- 점령전 하나밖에 없다.
	g_tbCompetition_ExceptInst = g_Competition:GetExceptionPvPInstanceList()
	
	-- 트리 선택 중인 인덱스
	local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "tree" )
	
	-- 선택한게 없으면 점령전 보이고 있는 중이다. 업데이트 해준다.
	if -1 == nIndex then
	
		Competition_SetInfoExceptInst()
		return
	
	end
	
	-- 현재 트리 선택중인 카테고리
	local nCategory = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "category", "tree" )
	
	-- CTF 아니면 업데이트 할 필요 없음.
	if g_nCompetition_CATEGORY_EXCEPTION ~= nCategory then
		return
	end
	
	Competition_SetInfoExceptInst()

end

-- CDM 랭크 정보 업데이트
function Competition_CDMRankInfo( tbRankInfo )

	if false == IsLoadWidget( WIDGET_ID_COMPETITION ) then
		return
	end
	
	local strParam = ""
	
	for i = 1, g_nCompetition_CDMRANKCOUNT, 1 do
	
		local strInst = "infoCDM.item" .. tostring( i )
		
		local tbRank = tbRankInfo[ i ]
		if nil ~= tbRank then
		
			strParam = strParam .. tostring( tbRank[ 1 ] ) .. ";"
			
			SetString( WIDGET_ID_COMPETITION, "text", tbRank[ 2 ], strInst .. ".tfClub" )
			SetString( WIDGET_ID_COMPETITION, "text", tbRank[ 3 ], strInst .. ".tfName" )
			
			--!! 클럽 마크 변경 필요
			SetBool( WIDGET_ID_COMPETITION, "visible", true, strInst .. ".imgClub" )
		
		else
		
			SetString( WIDGET_ID_COMPETITION, "text", "", strInst .. ".tfClub" )
			SetString( WIDGET_ID_COMPETITION, "text", "", strInst .. ".tfName" )
			SetBool( WIDGET_ID_COMPETITION, "visible", false, strInst .. ".imgClub" )
		
		end
	
	end
	
	SetData( WIDGET_ID_COMPETITION, strParam, "infoCDM.list", false )

end

-- PVP 정보 업데이트
function Competition_UpdatePVP()

	if false == IsLoadWidget( WIDGET_ID_COMPETITION ) then
		return
	end
	Competition_SortInfoTable_MatchingPVP()
	local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" )
	if nIndex ~= -1 then
		
		local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "infoStage.listInst" )
		local nInfoIndex = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "infoIndex", "infoStage.listInst" )
		Competition_SetInfo_Matching( strInfoID, nInfoIndex )
		
	end

end

-- PVE 정보 업데이트
function Competition_UpdatePVE()

	if false == IsOpenCompetition() then
		return
	end
	
	Competition_SortInfoTablePVE()
	
	-- 단계형 인던 선택중 아니면 업데이트 할 필요 없음.
	if g_nCompetition_SelectType ~= g_nCompetition_TYPE_UNLIMIT then
		return
	end
	
	local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "tree" )
	Ev_Competition_SelectTree( nIndex )
	local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "infoStage.listInst" )
	local nInfoIndex = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "infoIndex", "infoStage.listInst" )
	Competition_SetInfoPVE( strInfoID, nInfoIndex )
	
end

-- PVE 입장 확인
function Competition_PVEAckEntrance( bShow, dwMapID )

	if true == bShow then
	
		g_bCompetition_ShowEntranceMsg = true
	
		local str = string.format( GetGameInternalText( "COMPETITION_PVE_ENREANCE_TEXT", 0 ), GetMapName( dwMapID ) )
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, 
			Ev_Competition_PVEAckEntranceOK, Ev_Competition_PVEAckEntranceCancel, nil, nil, 10, Event_MsgBox_Close )
	
	else
	
		if true == g_bCompetition_ShowEntranceMsg then
		
			Event_MsgBox_Close()
		
		end
	
	end

end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 닫기
function Ev_Competition_Close()

	CloseWidget( WIDGET_ID_COMPETITION )

end

-- 인던 트리 선택
function Ev_Competition_SelectTree( nIndex )
	
	if -1 == nIndex then
		return
	end

	-- 선택한 놈 카테고리
	local nCategory = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "category", "tree" )
	
	-- 인던 정보 설정
	-- 인던 시스템 관리 안되는 인던
	if g_nCompetition_CATEGORY_EXCEPTION == nCategory then
	
		Competition_SetInfoExceptInst()
	
	-- PVP
	elseif g_nCompetition_CATEGORY_PVP == nCategory then
	
		Competition_Select_MatchingPVPTree( nIndex )

	-- PVE
	elseif g_nCompetition_CATEGORY_PVE == nCategory then
	
		Competition_Select_MatchingPVETree( nIndex )
		Competition_SelectPVETree( nIndex )

	end
	
	SetBool( WIDGET_ID_COMPETITION, "selected", false, "infoStage.btnInfo" )
	SetBool( WIDGET_ID_COMPETITION, "visible", false, "infoCTF" )
	SetBool( WIDGET_ID_COMPETITION, "visible", false, "infoCDM" )

end

-- 인던 리스트 선택
function Ev_Competition_SelectInstList( nIndex )

	if -1 == nIndex then
		return
	end
	
	local nCategory = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "category", "infoStage.listInst" )
	local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "infoStage.listInst" )
	local nInfoIndex = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "infoIndex", "infoStage.listInst" )
	
	-- 인던 정보 설정
	-- PVP
	if g_nCompetition_CATEGORY_PVP == nCategory then
	
		Competition_SetInfo_Matching( strInfoID, nInfoIndex )

	-- PVE
	elseif g_nCompetition_CATEGORY_PVE == nCategory then
	
		Competition_SetInfo_Matching( strInfoID, nInfoIndex )
		Competition_SetInfoPVE( strInfoID, nInfoIndex )

	end

end

-- 정보 보기
function Ev_Competition_ShowInfo()

	if -1 == g_nCompetition_SelectType then
		return
	end

	-- CTF
	if g_nCompetition_TYPE_CTF == g_nCompetition_SelectType then
	
		local bShow = GetBool( WIDGET_ID_COMPETITION, "selected", "infoStage.btnInfo" )
		SetBool( WIDGET_ID_COMPETITION, "visible", bShow, "infoCTF" )
	
	-- 토너먼트
	elseif g_nCompetition_TYPE_TOURNAMENT == g_nCompetition_SelectType then
	
		--!! 토너먼트 작업 보류
	
	-- CDM
	elseif g_nCompetition_TYPE_CDM == g_nCompetition_SelectType then
	
		local bShow = GetBool( WIDGET_ID_COMPETITION, "selected", "infoStage.btnInfo" )
		SetBool( WIDGET_ID_COMPETITION, "visible", bShow, "infoCDM" )
		
		-- 랭크 정보 요청
		if true == bShow then
		
			local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" )
			local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "infoStage.listInst" )
			local nInfoIndex = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "infoIndex", "infoStage.listInst" )
			
			-- 인던 정보 리스트 없으면 종료
			local tbInfoList = g_tbCompetition_Matching_PVPInst[ strInfoID ]
			if nil == tbInfoList then
				return
			end
			
			-- 인던 정보 없으면 종료
			local tbInfo = tbInfoList[ nInfoIndex ]
			if nil == tbInfo then
				return
			end
			
			-- 랭크 정보 요청
			g_Msg:ReqCDMRankInfo( tbInfo[ 13 ] )
		
		end
	
	end

end

-- CTF 정보 닫기
function Ev_Competition_PrevInfoClose()

	SetBool( WIDGET_ID_COMPETITION, "visible", false, "infoCTF" )
	SetBool( WIDGET_ID_COMPETITION, "visible", false, "infoCDM" )
	
	-- 정보 보기 버튼 토글 끄기
	SetBool( WIDGET_ID_COMPETITION, "selected", false, "infoStage.btnInfo" )

end

-- 참여하기
function Ev_Competition_Join()

	-- CTF
	if g_nCompetition_TYPE_CTF == g_nCompetition_SelectType then
	
		-- 참여 요청
		g_Msg:ReqCTFJoin()
	
	-- CDM
	-- 깃발전
	-- 토너먼트
	-- 돌파형 인던
	elseif 	g_nCompetition_TYPE_CDM == g_nCompetition_SelectType or
			g_nCompetition_TYPE_CTFLAG == g_nCompetition_SelectType or
			g_nCompetition_TYPE_TOURNAMENT == g_nCompetition_SelectType or
			g_nCompetition_TYPE_WAIL1 == g_nCompetition_SelectType then
	
		local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" )
		local strInfoID = GetDataString( WIDGET_ID_COMPETITION, nIndex, "infoID", "infoStage.listInst" )
		local nInfoIndex = GetDataInt( WIDGET_ID_COMPETITION, nIndex, "infoIndex", "infoStage.listInst" )
		
		-- 인던 정보 리스트 없으면 종료
		local tbInfoList = g_tbCompetition_Matching_PVPInst[ strInfoID ]
		if nil == tbInfoList then
		
			tbInfoList = g_tbCompetition_Matching_PVEInst[ strInfoID ]
			if nil == tbInfoList then
				return
			end
			
		end
		
		-- 인던 정보 없으면 종료
		local tbInfo = tbInfoList[ nInfoIndex ]
		if nil == tbInfo then
			return
		end
		
		-- 참여 요청
		g_Msg:OnEnteranceTournament( tbInfo[ 13 ] )
		
	-- 단계형 인던
	elseif g_nCompetition_TYPE_UNLIMIT == g_nCompetition_SelectType then
	
		local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" )
		
		-- 참여 요청
		g_Msg:OnEnteranceCompetition( nIndex )
	
	end

end

-- 참여 취소하기
function Ev_Competition_JoinCancel()

	-- CTF
	if g_nCompetition_TYPE_CTF == g_nCompetition_SelectType then
	
		-- 참여 취소 요청
		g_Msg:ReqCancelCompetition()
	
	-- CDM
	-- 깃발전
	-- 토너먼트
	elseif 	g_nCompetition_TYPE_CDM == g_nCompetition_SelectType or
			g_nCompetition_TYPE_CTFLAG == g_nCompetition_SelectType or
			g_nCompetition_TYPE_TOURNAMENT == g_nCompetition_SelectType or
			g_nCompetition_TYPE_WAIL1 == g_nCompetition_SelectType then
	-- 단계형 인던
	elseif g_nCompetition_TYPE_UNLIMIT == g_nCompetition_SelectType then
	end

end

-- 나가기
function Ev_Competition_Exit()

	-- CTF
	if g_nCompetition_TYPE_CTF == g_nCompetition_SelectType then
	
		-- 참여 취소 요청
		g_Msg:ReqExitCompetition()
	
	-- CDM
	-- 깃발전
	-- 토너먼트
	elseif 	g_nCompetition_TYPE_CDM == g_nCompetition_SelectType or
			g_nCompetition_TYPE_CTFLAG == g_nCompetition_SelectType or
			g_nCompetition_TYPE_TOURNAMENT == g_nCompetition_SelectType or
			g_nCompetition_TYPE_WAIL1 == g_nCompetition_SelectType then
	-- 단계형 인던
	elseif g_nCompetition_TYPE_UNLIMIT == g_nCompetition_SelectType then
	end

end

-- 귀속 해제
function Ev_Competition_BindOff()

	if g_nCompetition_TYPE_UNLIMIT ~= g_nCompetition_SelectType then
		return
	end
	
	local nIndex = GetInt( WIDGET_ID_COMPETITION, "selectedIndex", "infoStage.listInst" )
	g_Msg:OnClearOfBelongingCompetition( nIndex )

end

-- 남은 시간 타이머
function Ev_Competition_Timer()

	-- 분 단위 감소
	if 0 < g_nCompetition_Min then
		
		g_nCompetition_Min = g_nCompetition_Min - 1
		
	-- 0분일 경우
	elseif 0 == g_nCompetition_Min then
	
		-- 시 단위가 0보다 크면 시간 1 깎고 59분으로
		if 0 < g_nCompetition_Hour then
		
			g_nCompetition_Min = 59
			g_nCompetition_Hour = g_nCompetition_Hour - 1
			
		-- 시간 끝남
		else
		
			g_nCompetition_Min = 0
			g_nCompetition_Hour = 0
			
		end
		
	end
	
	-- 시간 텍스트 설정
	local str = string.format( GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 8 ), g_nCompetition_Hour, g_nCompetition_Min )
	SetString( WIDGET_ID_COMPETITION, "text", str, "infoStage.labelBettleTime" )
	
	-- 시간 끝남
	if 0 == g_nCompetition_Min and 0 == g_nCompetition_Hour then
		return
	end
	
	-- 시간 타이머 설정 (1분)
	local tbQuickSet = 
	{
		time = 60000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_Competition_Timer"
	}
	SetTween( WIDGET_ID_COMPETITION, tbQuickSet, "", "", "infoStage.labelBettleTime" )

end

-- 인던 귀속 시간 타이머
function Ev_Competition_BindOffTimer()

	local bContinueTimer = false

	for value in pairs( g_tbCompetition_PVEInst ) do
	
		local tbInfoList = g_tbCompetition_PVEInst[ value ]
		for value in pairs( tbInfoList ) do
	
			local tbInfo = tbInfoList[ value ]
		
			local nGrade = 0
			local nCount = 0
			local nTime = 0
			
			for value in pairs( tbInfo[ 11 ] ) do

				local tbLevelInfo = tbInfo[ 11 ][ value ]
				
				nGrade = 21 + tbLevelInfo[ 1 ]
				nCount = nCount + tbLevelInfo[ 5 ]
				nTime = nTime + tbLevelInfo[ 6 ]
		
			end
			
			local str = GetGameWordText( "COMPETITION_PVE", nGrade ) .. " " .. 
				GetGameWordText( tbInfo[ 3 ], 0 )
				
			if 0 ~= nTime then
			
				str = str .. "(" .. GetGameWordText( "COMPETITION_PVE", 24 ) .. " " .. 
					math.floor( nTime / 60 ) .. GetGameWordText( "ITEM_BASIC_TIME", 1 ) .. " " ..
					math.floor( nTime % 60 ) .. GetGameWordText( "ITEM_BASIC_TIME", 2 ) .. ")"
					
				bContinueTimer = true
				
			else
			
				continue
			
			end
			
			str = str .. StringFormat( GetGameInternalText( "WAIL_OF_THE_BUILDING_REMTODAY_TEXT", 0 ), tbInfo[ 7 ] - nCount )
			str = HTMLFontColor( str, HTML_TEXT_COLOR_WHITE )
			-- 아래 의도가 뭐지?;
			--SetDataProp_string( WIDGET_ID_COMPETITION, tostring( value - 1 ), "label", str, "infoStage.listInst" )
			
		end
	
	end

	if true == bContinueTimer then
	
		local tbQuickSet = 
		{
			time = 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_Competition_BindOffTimer"
		}
		SetTween( WIDGET_ID_COMPETITION, tbQuickSet, "", "", "infoStage.listInst" )
	
	end
	
end

-- PVE 입장 확인 클릭
function Ev_Competition_PVEAckEntranceOK()

	g_bCompetition_ShowEntranceMsg = false
	Event_MsgBox_Close()
	
	g_Msg:OnEnteranceConfirmCompetition( true )

end

-- PVE 입장 취소 클릭
function Ev_Competition_PVEAckEntranceCancel()

	g_bCompetition_ShowEntranceMsg = false
	Event_MsgBox_Close()
	
	g_Msg:OnEnteranceConfirmCompetition( false )

end