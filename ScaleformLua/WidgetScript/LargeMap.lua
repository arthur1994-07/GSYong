
LARGEMAP_MOBLIST_COUNT = 8

-- 맵 이미지 마우스 좌표
g_nLargeMap_MapMousePosX = 0
g_nLargeMap_MapMousePosY = 0

-- 몬스터 최대 체크횟수 설정을 위한변수
g_MobList_CheckCount = 0			-- 현재 체크중인 항목수
g_MobList_MaxCheck = 3				-- 최대 체크 가능한 항목수
g_MobList_CheckList = {}

g_bLargeMap_MobNameSelectAll = false
g_tbLargeMap_MobNameList = {}

---------------------------------
g_nLargeMap_MyMapIndex = -1			-- 내 캐릭터가 있는 맵 인덱스
g_nLargeMap_MapListTotal = 0		-- 맵 리스트 개수
g_nLargeMap_SelectMapID = -1		-- 현재 선택중인 맵 ID

g_nLargeMap_Alliance_MaxNum = 20	-- 동맹 아이콘 최대 갯수

g_tbLargeMap_ZoneName = {}			-- 지역명 데이터
g_tbLargeMap_Gate = {}				-- 게이트 데이터
g_tbLargeMap_CTFAuthenticator = {}	-- CTF 인증기 데이터
g_tbLargeMap_CTFAllacne = {}		-- CTF 동맹 데이터
g_tbLargeMap_Post = {}				-- 우편 데이터 
g_tbLargeMap_Bus = {}				-- 버스 데이터
g_tbLargeMap_Locker = {}			-- 라커 데이터
g_tbLargeMap_StartPos = {}			-- 시작위치 데이터
g_tbLargeMap_ClubMan = {}			-- 클럽 매니저 데이터
g_tbLargeMap_Npc = {}				-- NPC 데이터
g_tbLargeMap_Mob = {}				-- 몹 데이터

g_bLargeMap_SearchCurMap = false	-- 현재 표시 지역에서만 검색?

g_MobListCount = 0					-- 맵의 몬스터 정보 리스트 아이템 갯수를 저장하기 위한 변수
g_tbLargeMap_SelectMob = {}			-- 몹 정보 출력 선택한 놈 ID
g_tbLargeMap_RewardItem = {}		-- 몹 보상 아이템
g_tbItemListConvert = {}			-- 봅 보상아이템 컨버터

g_tbLargeMap_SearchCharList = {}	-- 캐릭터 검색 결과 리스트
g_nLargeMap_SearchCharSelect = -1	-- 캐릭터 검색 결과 선택 인덱스

g_nLargeMap_AuthType = -1
g_tbCTIAuthInfo_CachingData = {}

-- 검색한 캐릭터 세부 정보
g_tbLargeMap_SearchChar = 
{
	bSerach = false,		-- 검색 되었나?
	nMapID	= -1,			-- 맵 ID
	nPosX = 0,				-- 위치 X
	nPosY = 0,				-- 위치 Y
	
	strSchool 	= "",		-- 학원
	strLevel	= "",		-- 레벨
	strName 	= "",		-- 이름
	strBadge 	= "",		-- 뱃지
	strCalss 	= "",		-- 클래스
	strParty 	= "",		-- 파티
	strClub 	= ""		-- 클럽
}

-- 맵 이전 마우스 좌표
g_tbLargeMap_PrevMousePos = { 0, 0 }

-- 맵 드래그 시작 위치
g_nLargeMap_DragMapX = 0
g_nLargeMap_DragMapY = 0

g_bLargeMap_InitMobList = false
g_bLargeMap_Open = false

function CloseLargeMap()

	g_bLargeMap_Open = false
	g_bLargeMap_MobNameSelectAll = false
	
end

function IsOpenLargeMap()
	return g_bLargeMap_Open
end

-- 전체맵 로드 완료
function LoadLargeMap()

	g_bLargeMap_MobNameSelectAll = false

	-- UI 닫기 이벤트
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_WIDGET_CLOSE, "CloseLargeMap", "", "" )

	-- 맵 리스트 이동 버튼
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnMyMap", "", "btnMyMap" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnPrev", "", "btnPrev" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnNext", "", "btnNext" )

	-- 설정 패널 선택
	SetBool( WIDGET_ID_LARGEMAP, "selected", true, "radioCfg" )
	LargeMap_SelectPanel( 0 )
	
	-- 패널 선택 라디오버튼
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_SIGN_SETTING", 0 ), "radioCfg_text" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_MONSTER_INFO", 0 ), "radioInfo_text" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "RN_MAP_WINDOW_SEARCH_TYPE", 0 ), "radioSearch_text" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_SelectPanelCfg", "", "radioCfg" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_SelectPanelInfo", "", "radioInfo" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_SelectPanelSearch", "", "radioSearch" )
	
	-- 맵 리스트 설정
	local tbMapListClient = g_Minimap:GetMapList()
	local tbMapList = {}
	for value in pairs( tbMapListClient ) do
	
		local tbMapClient = tbMapListClient[ value ]
		local tbMap =
		{ 
			tbMapClient[ "strName" ],
			tbMapClient[ "dwID" ]
		}
		
		table.insert( tbMapList, tbMap )
		
		g_nLargeMap_MapListTotal = g_nLargeMap_MapListTotal + 1
	
	end
	SetDataEx( WIDGET_ID_LARGEMAP, "label;dwID;", tbMapList, "comboMapList" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_LIST_INDEX_CHANGE, "Ev_LargeMap_SelectMap", "index", "comboMapList" )
	
	-- 맵 드래그 이벤트 추가
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_MOUSE_DOWN, "Ev_LargeMap_DragMapStart", "buttonIdx;stageX;stageY", "map" )
	-- 맵 좌표 숨기기
	SetBool( WIDGET_ID_LARGEMAP, "visible", false, "mcPos" )
	-- 마우스 위치에 따른 좌표 표시
	-- 맵 이미지 위에 아이콘이 올라가기 때문에 local XY를 가져올 경우 아이콘 위로 마우스가 올라가면 아이콘의 local XY를 가져온다.
	-- 따라서 stage XY를 가져와 이벤트 함수 내에서 직접 위치를 계산한다.
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_MOUSE_MOVE, "Ev_LargeMap_ShowPositionDelay", "stageX;stageY", "map" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_MOUSE_ROLLOVER, "Ev_LargeMap_MapRollOver", "stageX;stageY", "map" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_MOUSE_ROLLOUT, "Ev_LargeMap_MapRollOut", "", "map" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_MOUSE_CLICK, "Ev_LargeMap_MapClick", "buttonIdx;stageX;stageY;ctrlKey", "map" )
	
	-- 설정 패널 설정 ----------------------------------------------------------------------------------------------------	
	-- 표시 설정
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_SIGN_SETTING", 0 ), "panelCfg.checkCfgAll_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkCfgAll" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgAll", "", "panelCfg.checkCfgAll" )
	-- 게이트 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_GATE_SIGN", 0 ), "panelCfg.checkGate_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleGate, "panelCfg.checkGate" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgGate", "", "panelCfg.checkGate" )
	-- 일반 Npc 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_NORMAL_NPC_SIGN", 0 ), "panelCfg.checkNpc_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleNpc, "panelCfg.checkNpc" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgNpc", "", "panelCfg.checkNpc" )
	-- 지역명 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_PLACE_NAME_SIGN", 0 ), "panelCfg.checkZoneName_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleZoneName, "panelCfg.checkZoneName" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgZone", "", "panelCfg.checkZoneName" )
	-- 버스 정류장 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_BUS_STATION_SIGN", 0 ), "panelCfg.checkBus_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleBus, "panelCfg.checkBus" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgBus", "", "panelCfg.checkBus" )
	-- 라커 관리인 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_LOCKER_MANAGER_SIGN", 0 ), "panelCfg.checkLocker_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleLocker, "panelCfg.checkLocker" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgLocker", "", "panelCfg.checkLocker" )
	-- 시작 위치 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_START_POSITION_SIGN", 0 ), "panelCfg.checkStartPos_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleStartPos, "panelCfg.checkStartPos" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgStartPos", "", "panelCfg.checkStartPos" )
	-- 클럽 관리인 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_CLUB_MANAGER_SIGN", 0 ), "panelCfg.checkClubMan_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleClubMan, "panelCfg.checkClubMan" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgClubMan", "", "panelCfg.checkClubMan" )
	-- 우체통 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_POSTBOX_SIGN", 0 ), "panelCfg.checkPost_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisiblePost, "panelCfg.checkPost" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgPost", "", "panelCfg.checkPost" )
	-- 파티원 표시
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_PARTY_PLAYER_SIGN", 0 ), "panelCfg.checkParty_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleParty, "panelCfg.checkParty" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgParty", "", "panelCfg.checkParty" )
	
	-- 동작 설정
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_ACTION_SETTING", 0 ), "panelCfg.checkAction_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_Action, "panelCfg.checkAction" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgAction", "", "panelCfg.checkAction" )
	-- 지도 클릭시 자동 탈것 타기
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_AUTO_RIDE_VEHICLE", 0 ), "panelCfg.checkVehicle_text" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_ActionVehicle, "panelCfg.checkVehicle" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_cfgVehicle", "", "panelCfg.checkVehicle" )
	
	-- 검색한 캐릭터가 있다면 검색 결과 출력
	if true == g_tbLargeMap_SearchChar[ "bSerach" ] then
	
		SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strSchool" ], "panelSearch.labelInfo1" )
		SetString( WIDGET_ID_LARGEMAP, "text", 
			g_tbLargeMap_SearchChar[ "strLevel" ] .. " " .. g_tbLargeMap_SearchChar[ "strName" ],
			"panelSearch.labelInfo2" )
		SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strBadge" ], "panelSearch.labelInfo3" )
		SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strCalss" ], "panelSearch.labelInfo4" )
		SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strParty" ], "panelSearch.labelInfo5" )
		SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strClub" ], "panelSearch.labelInfo6" )
	
	else
	
		for i = 1, 6, 1 do
			SetString( WIDGET_ID_LARGEMAP, "text", "", "panelSearch.labelInfo" .. tostring( i ) )
		end
	
	end
	
	-- 몬스터 정보 패널 설정 -----------------------------------------------------------------------------------------------	
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_MONSTER_REWARD_ITEM", 0 ), "panelInfo.labelItem" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_MobInfoUnCheckAll", "", "panelInfo.checkMobInfo" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_LIST_ITEM_CLICK, "Ev_LargeMap_MobSelect", "index", "panelInfo.listMob" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_SCROLL, "Ev_LargeMap_MobListWheel", "", "panelInfo.listMob" )
	
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "POINT_SHOP_ALL_CATEGORY", 0 ), "panelInfo.checkMobInfo" )
	
	-- 보상 아이템 슬롯 바인딩, 이벤트 설정
	for i = 1, LARGEMAP_MOBREWARD_SLOT_TOTAL, 1 do
	
		SetString( WIDGET_ID_LARGEMAP, "binding",
			SLOT_LARGEMAP_REWARDITEM_ID .. tostring( i ), "panelInfo.slot" .. tostring( i ) )
			
		SetEvent( WIDGET_ID_LARGEMAP, EVENT_MOUSE_WHEEL, "Ev_LargeMap_RewardSlotWheel", "delta", 
			"panelInfo.slot" .. tostring( i ) )
	end
	
	-- 보상 아이템 리스트 스크롤
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_SCROLL, "Ev_LargeMap_listRewardScroll", "", "panelInfo.exList.list" )
	-- 보상 아이템 설정
	LargeMap_SetMobRewardItem()
	
	-- 검색 패널 설정 -----------------------------------------------------------------------------------------------
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_SEARCH_STUDENT", 0 ), "panelSearch.labelSearch" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_SEARCH_STUDENT_INFO", 0 ), "panelSearch.labelInfo" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_SCHOOL", 0 ) .. " : ", "panelSearch.labelSchool" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_NAME", 0 ) .. " : ", "panelSearch.labelName" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_BADGE", 0 ) .. " : ", "panelSearch.labelBadge" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_CLASS", 0 ) .. " : ", "panelSearch.labelClass" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_PARTY", 0 ) .. " : ", "panelSearch.labelParty" )
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_CLUB", 0 ) .. " : ", "panelSearch.labelClub" )
	
	if 0 ~= table.getn( g_tbLargeMap_SearchCharList ) then
	
		SetDataEx( WIDGET_ID_LARGEMAP, "label;nLevel", g_tbLargeMap_SearchCharList, "panelSearch.listChar" )		
		SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", g_nLargeMap_SearchCharSelect, "panelSearch.listChar" )
		
	end
	
	SetString( WIDGET_ID_LARGEMAP, "text", GetGameWordText( "MAP_ONLY_CURRENT_REGION", 0 ), "panelSearch.checkCurrentMap_text" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_SEARCH", 0 ), "panelSearch.btnSearch" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_SHOW_MAP_POSITION", 0 ), "panelSearch.btnDisplay" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_WHISPER", 0 ), "panelSearch.btnWhisper" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_REGISTER_FRIEND", 0 ), "panelSearch.btnFriend" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_CHARACTER_VIEW", 0 ), "panelSearch.btnInfo" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_PARTY", 0 ), "panelSearch.btnParty" )
	SetString( WIDGET_ID_LARGEMAP, "label", GetGameWordText( "MAP_INVITE_CLUB", 0 ), "panelSearch.btnClub" )
	
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bLargeMap_SearchCurMap, "panelSearch.checkCurrentMap" )
	
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_KEY_UP, "Ev_LargeMap_SearchInputKeyDown", "keyCode", "panelSearch.inputSearch" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_SearchBtnClick", "", "panelSearch.btnSearch" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_CheckCurMap", "", "panelSearch.checkCurrentMap" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnDisplay", "", "panelSearch.btnDisplay" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnWhisper", "", "panelSearch.btnWhisper" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnFriend", "", "panelSearch.btnFriend" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnInfo", "", "panelSearch.btnInfo" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnParty", "", "panelSearch.btnParty" )
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_BUTTON_CLICK, "Ev_LargeMap_btnClub", "", "panelSearch.btnClub" )
	
	SetEvent( WIDGET_ID_LARGEMAP, EVENT_LIST_INDEX_CHANGE, "Ev_LargeMap_SearchListIndex", "index", "panelSearch.listChar" )

	g_tbCTIAuthInfo_CachingData = {}
end

function OpenLargetMap()

	local bOne, bTwo = GetDataBool( WIDGET_ID_LARGEMAP, 0, "selected", "panelInfo.listMob" )
		,GetDataBool( WIDGET_ID_LARGEMAP, 1, "selected", "panelInfo.listMob" )
	
	bOne = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelInfo.checkMobList1" )
	bTwo = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelInfo.checkMobList2" )
	
	g_bLargeMap_Open = true
	
	-- 현재 내 맵 ID
	local nMyMapID = g_MyChar:GetCurrentMapID()
	local tbMapListClient = g_Minimap:GetMapList()
	local bUIMapInfo = false
	for value in pairs( tbMapListClient ) do
	
		local tbMapClient = tbMapListClient[ value ]
	
		-- 현재 내 맵과 ID가 동일하면 인덱스 설정
		if nMyMapID == tbMapClient[ "dwID" ] then
			g_nLargeMap_MyMapIndex = value - 1
			bUIMapInfo = tbMapClient[ "bUIMapInfo" ]
		end
	
	end
	
	local nSelectMap = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	if nSelectMap ~= g_nLargeMap_MyMapIndex then

		-- 맵 선택
		SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", g_nLargeMap_MyMapIndex, "comboMapList" )
	
	end
	
	if true == g_bMap_SecretMap then
	
		SetBool( WIDGET_ID_LARGEMAP, "visible", true, "mcSecretMap" )
		
		SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", -1, "comboMapList" )
		SetString( WIDGET_ID_LARGEMAP, "defaultLabel", g_strMinimap_Name, "comboMapList" )
		
		LargeMap_SetLevelCondition( g_MyChar:GetCurrentMapID() )
	
	else
	
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "mcSecretMap" )
		SetString( WIDGET_ID_LARGEMAP, "defaultLabel", "", "comboMapList" )
		
	end
	LargeMap_SelectMap( nMyMapID )
	
	-- 미니맵으로 m2p 이동시 nSelectMap == g_nLargeMap_MyMapIndex 가 되므로, LargeMap_SelectMap 가 호출 안되는 현상이 생겨서 밖으로 뺌.
	--LargeMap_SelectMap( nMyMapID )
	
	if g_tbCTIAuthInfo_CachingData ~= nil and table.getn( g_tbCTIAuthInfo_CachingData ) ~= 0 then--bUIMapInfo == true then
	
		-- 인증기 표시할 맵 선택중 아니면 아이콘 추가할 필요 없다.
		local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
		local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelect, "dwID", "comboMapList" )
		if g_MyChar:GetCurrentMapID() == nMapID then
		
			LargeMap_SetAuth( g_tbCTIAuthInfo_CachingData, g_nLargeMap_AuthType )
			
		end

	end
	
end

-- 패널 선택
function LargeMap_SelectPanel( nIndex )

	if 0 == nIndex then
	
		SetBool( WIDGET_ID_LARGEMAP, "visible", true, "panelCfg" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "panelInfo" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "panelSearch" )
		
		SetBool( WIDGET_ID_LARGEMAP, "selected", true, "radioCfg" )
		SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioInfo" )
		SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioSearch" )
	
	elseif 1 == nIndex then
	
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "panelCfg" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", true, "panelInfo" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "panelSearch" )
		
		SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioCfg" )
		SetBool( WIDGET_ID_LARGEMAP, "selected", true, "radioInfo" )
		SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioSearch" )
	
	else
	
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "panelCfg" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "panelInfo" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", true, "panelSearch" )
		
		SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioCfg" )
		SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioInfo" )
		SetBool( WIDGET_ID_LARGEMAP, "selected", true, "radioSearch" )
	
	end

end

-- 맵 이미지 로드
function LargeMap_LoadMapImg( nMapID )

	local strMapImg = GetMinimapTexturePath() .. g_Minimap:GetMapImgFile( nMapID )
	if "" == strMapImg then
		return
	end
	
	CallFunction( WIDGET_ID_LARGEMAP, "LoadMap", strMapImg, "map" )

end

-- 아이콘 생성
function LargeMap_CreateIcon( nMapID )

	-- 몹 아이콘 설정 (로드 속도가 느려 몬스터 정보 패널 선택할 때 생성하도록 함)
	-- 맵 변경 후에 몹 아이콘이 남아있으니 레이어만 제거한다.
	-- 몹 아이콘 삭제를 위해 레이어 제거
		-- 같은 맵 재오픈시 몹정보가 남아있어야한다;
		-- 수정시 g_nLargeMap_SelectMapID 초기화 시점 고려해야한다;
		-- LargeMap_SelectMap
	if g_nLargeMap_SelectMapID ~= nMapID then
		for value in pairs( g_tbMap_MobLayer ) do
			CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_tbMap_MobLayer[ value ], "map" )
		end
	end
	--LargeMap_SetMobIcon( nMapID )	
	
	-- 게이트 아이콘 설정
	LargeMap_SetGateIcon( nMapID )
	-- 우편함 아이콘 설정
	LargeMap_SetPostIcon( nMapID )
	-- NPC 아이콘 설정
	LargeMap_SetNpcIcon( nMapID )
	-- 버스 아이콘 설정
	g_tbLargeMap_Bus = LargeMap_SetTalkNpcIcon( nMapID, g_strMap_LAYER_BUS, "bus", "_iconBus", EM_BUSSTATION )
	-- 라커 아이콘 설정
	g_tbLargeMap_Locker = LargeMap_SetTalkNpcIcon( nMapID, g_strMap_LAYER_LOCKER, "locker", "_iconLocker", EM_STORAGE )
	-- 시작위치 아이콘 설정
	g_tbLargeMap_StartPos = LargeMap_SetTalkNpcIcon( nMapID, g_strMap_LAYER_STARTPOS, "startPos", "_iconStartPos", EM_STARTPOINT )
	-- 클럽 매니저 아이콘 설정
	g_tbLargeMap_ClubMan = LargeMap_SetTalkNpcIcon( nMapID, g_strMap_LAYER_CLUBMAN, "clubMan", "_iconClub", EM_CLUB_STORAGE )
	-- 파티 아이콘 설정
	LargeMap_SetPartyIcon( nMapID )
	-- 점령전 인증기
	LargeMap_SetAuthenticatorIcon( nMapID )
	-- 점령전 동맹
	LargeMap_SetAllianceIcon( nMapID )
	-- 플레이어 아이콘 설정
	LargeMap_SetPlayerIcon( nMapID )
	-- 지역명 설정
	LargeMap_SetZoneName( nMapID )
	
	-- 표시 설정 체크 상태에 따라 레이어 visible of/off
	LargeMap_SetLayerVisible( g_strMap_LAYER_GATE, g_bMap_VisibleGate )
	LargeMap_SetLayerVisible( g_strMap_LAYER_NPC, g_bMap_VisibleNpc )
	LargeMap_SetLayerVisible( g_strMap_LAYER_TEXT, g_bMap_VisibleZoneName )
	LargeMap_SetLayerVisible( g_strMap_LAYER_BUS, g_bMap_VisibleBus )
	LargeMap_SetLayerVisible( g_strMap_LAYER_LOCKER, g_bMap_VisibleLocker )
	LargeMap_SetLayerVisible( g_strMap_LAYER_STARTPOS, g_bMap_VisibleStartPos )
	LargeMap_SetLayerVisible( g_strMap_LAYER_CLUBMAN, g_bMap_VisibleClubMan )
	LargeMap_SetLayerVisible( g_strMap_LAYER_POST, g_bMap_VisiblePost )
	LargeMap_SetLayerVisible( g_strMap_LAYER_PARTY, g_bMap_VisibleParty )

end

-- 지역명 텍스트 설정
function LargeMap_SetZoneName( nMapID )

	-- 기존 지역명 데이터 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_TEXT, "map" )
	-- 지역명 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_TEXT, "map" )
	
	-- 현재 맵에 설정된 지역명 정보를 가져와 텍스트 아이콘 추가
	g_tbLargeMap_ZoneName = g_Minimap:GetMapZoneName( nMapID )
	for value in pairs( g_tbLargeMap_ZoneName ) do
	
		local tbZoneData = g_tbLargeMap_ZoneName[ value ]
	
		local strTextID = "zone" .. tostring( value )
		local strParam = g_strMap_LAYER_TEXT .. ";" .. strTextID .. ";" .. "MapTextYellow" .. ";" .. 
			tbZoneData[ "strName" ]
		CallFunction( WIDGET_ID_LARGEMAP, "AddTextIcon", strParam, "map" )
		
		-- 텍스트 이동
		LargeMap_MoveIcon( g_strMap_LAYER_TEXT, strTextID, tbZoneData[ "x" ], tbZoneData[ "y" ], 0, 0 )
	
	end

end

-- 파티 아이콘 설정
function LargeMap_SetPartyIcon( nMapID )

	-- 기존 데이터 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_PARTY, "map" )
	
	-- 파티중이 아니면 생성할 필요 없다.
	if false == g_Party:IsValid() then
		return
	end	
	
	-- 파티 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_PARTY, "map" )
	
	-- 내 캐릭터 ID
	local nMyCharID = g_MyChar:GetDBNum()
	
	local nSelecteIndex = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	-- 현재 선택한 맵 ID
	local nSelectMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelecteIndex, "dwID", "comboMapList" )
	
	-- 파티원 수 만큼 아이콘 생성
	for value in pairs( g_tbParty_MemberList ) do
	
		local tbMember = g_tbParty_MemberList[ value ]
		
		-- 다른 맴버만
		if nMyCharID ~= tbMember[ "dwDbNum" ] then
			
			-- 현재 보고 있는 맵에 있는 파티원 아이콘만 생성
			if tbMember[ "dwMapID" ] == nSelectMapID then
			
				local strIconID = g_strMap_ICON_PARTY .. tostring( value )
				
				-- 아이콘 생성
				--local strParam = CreateParam( g_strMap_LAYER_PARTY, strIconID, "_iconParty", tbMember[ "strName" ] )
				--CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
				
				local strParam = g_strMap_LAYER_PARTY .. ";" .. 
					strIconID .. ";" .. 
					"_iconParty" .. ";" ..
					tbMember[ "strName" ] .. ";" ..
					tostring(EMCROW_PC) .. ";" ..
					tostring(tbMember[ "nLevel" ]) .. ";" ..
					tostring(tbMember["dwGaeaID"])
				CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
				
				-- 멤버 실제 위치
				local tbPos = tbMember[ "tbPosition" ]
				
				-- 미니맵 위치로 변환
				local tbPosUI = g_Minimap:PositionToMap( tbMember[ "dwMapID" ], tbPos[ "x" ], tbPos[ "y" ], tbPos[ "z" ] )
				
				-- [shhan][2016.10.07] nil  이 나올수 있는 구조임. 그냥 사용하면 crash 발생함.
				if tbPosUI[ "x" ] == nil then
					continue
				end
				if tbPosUI[ "y" ] == nil then
					continue
				end
				
				-- 파티원 아이콘 이동
				LargeMap_MoveIcon( g_strMap_LAYER_PARTY, strIconID, tbPosUI[ "x" ], tbPosUI[ "y" ], 0, 0 )
			
			end
		
		end
	
	end

end

-- 플레이어 아이콘 설정
function LargeMap_SetPlayerIcon( nMapID )

	-- 기존 데이터 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_PLAYER, "map" )
	-- 플레이어 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_PLAYER, "map" )
	
	-- 플레이어 이동 목표지점 아이콘 설정
	local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "_iconPlayerMove" .. ";;;;;;"
	CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
	
	-- 이동 목표 지점이 없으면 숨기기
	if nil == g_tbMap_MovePos then
		strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "0" .. ";;;;;;"
		CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
	end
	
	-- 플레이어 아이콘 추가
	strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYER .. ";" .. "_iconPlayer" .. ";;;;;;"
	CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
	
	-- 플레이어 방향 아이콘 추가
	strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERDIR .. ";" .. "_iconPlayerDir" .. ";;;;;;"
	CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
	
	strParam = CreateParam( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERDIR, "0" )
	CallFunction( WIDGET_ID_LARGEMAP, "SetHitTestEnable", strParam, "map" )
	
	-- 검색 캐릭터 아이콘 추가
	strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_SEARCHCHAR .. ";" .. "_iconSearchChar" .. ";;;;;;"
	CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
	-- 검색한 캐릭터가 없거나 선택한 맵이 다르면 visible 끄기
	if 	false == g_tbLargeMap_SearchChar[ "bSearch" ] or
		nMapID ~= g_tbLargeMap_SearchChar[ "nMapID" ] then
	
		strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_SEARCHCHAR .. ";" .. "0" .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
		
	-- 검색한 캐릭터가 있고 같은 맵이면 아이콘 이동
	else
	
		strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_SEARCHCHAR .. ";" .. 
			tostring( g_tbLargeMap_SearchChar[ "nPosX" ] ) .. ";" .. tostring( g_tbLargeMap_SearchChar[ "nPosY" ] ) .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "MoveIcon", strParam, "map" )
	
	end

end

function LargeMap_GetMobMarkName(nType)
	local strIconClass = "_iconMob"
	local nHIWORD, nLOWWORD = HIWORD(nType), LOWORD(nType)
	if nHIWORD ~= 0 then
		-- 퀘스트(1), 보스몹(2)로 체크하나 두가지 모두 해당되는 경우가 있다;
		-- 그럴 경우 퀘스트를 우선으로 처리한다;
		if (nHIWORD & 1) == 1 then
			strIconClass = strIconClass.."Quest"
		elseif nHIWORD == 2 then
			strIconClass = strIconClass.."Boss"
		end
	else
		if nLOWWORD == 0 then
			strIconClass = strIconClass.."Weak"
		elseif nLOWWORD == 1 then
			strIconClass = strIconClass.."Normal"
		elseif nLOWWORD == 2 then
			strIconClass = strIconClass.."Strong"
		elseif nLOWWORD == 3 then
			strIconClass = strIconClass.."VeryStrong"
		end
	end
	return strIconClass
end

-- 몹 아이콘 설정
function LargeMap_SetMobIcon( nMapID )

	-- 기존 아이콘 삭제를 위해 레이어 제거
	for value in pairs( g_tbMap_MobLayer ) do
		CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_tbMap_MobLayer[ value ], "map" )
	end
	
	g_tbMap_MobLayer = {}
	g_tbLargeMap_Mob = g_Minimap:GetMapMob( nMapID )
	
	for value in pairs( g_tbLargeMap_Mob ) do
	
		local tbMob = g_tbLargeMap_Mob[ value ]
		
		local strLayer = g_tbMap_MobLayer[ tbMob[ "dwID" ] ]
		if nil == strLayer then
		
			strLayer = g_strMap_LAYER_MOB .. tostring( tbMob[ "dwID" ] )
			g_tbMap_MobLayer[ tbMob[ "dwID" ] ] = strLayer
			
			-- 레이어 추가
			CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", strLayer, "map" )
		
		end
		
		local strIconClass = LargeMap_GetMobMarkName(tbMob[ "nType" ])--"_iconMob" .. tostring( tbMob[ "nType" ] )
		local strMobID = "mob" .. tostring( value )
		local strParam = strLayer .. ";" .. strMobID .. ";" .. 
			strIconClass .. ";" .. tbMob[ "strName" ] .. ";" ..
			tostring(tbMob["CrowType"]) .. ";" ..
			"-1" .. ";" ..
			tostring(tbMob["ID"])
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		-- 아이콘 이동
		LargeMap_MoveIcon( strLayer, strMobID, tbMob[ "x" ], tbMob[ "y" ], 0, 0 )
	
	end
	
	-- 몹 visible 끄기
	for value in pairs( g_tbMap_MobLayer ) do
		LargeMap_SetLayerVisible( g_tbMap_MobLayer[ value ], false )
	end

end

-- 게이트 아이콘 설정
function LargeMap_SetGateIcon( nMapID )

	-- 기존 게이트 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_GATE, "map" )
	-- 게이트 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_GATE, "map" )
	
	-- 현재 맵 게이트 정보를 가져와 아이콘 추가
	g_tbLargeMap_Gate = g_Minimap:GetMapGateName( nMapID )
	for value in pairs( g_tbLargeMap_Gate ) do
	
		local tbGateData = g_tbLargeMap_Gate[ value ]
		
		local strGateID = "gate" .. tostring( value )
		local strParam = g_strMap_LAYER_GATE .. ";" .. strGateID .. ";" 
						.. "_iconGate" .. ";" .. tbGateData[ "strName" ]..";"
						.. tostring(tbGateData[ "CrowType" ]) .. ";"
						.. "-1" .. ";"
						.. tostring(tbGateData[ "ID" ]) .. ";"
						.. tostring(nMapID) .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		strParam = CreateParam( g_strMap_LAYER_GATE, strGateID, EVENT_MOUSE_CLICK, "Ev_GateClick", "buttonIdx", tostring(tbGateData[ "ToMapID" ]) )
		--strParam = CreateParam( g_strMap_LAYER_GATE, strGateID, EVENT_MOUSE_CLICK, "Ev_GateClick", "buttonIdx", tostring(tbGateData[ "strName" ]) )
		CallFunction( WIDGET_ID_LARGEMAP, "SetEvent", strParam, "map" )
		-- 아이콘 이동
		LargeMap_MoveIcon( g_strMap_LAYER_GATE, strGateID, tbGateData[ "x" ], tbGateData[ "y" ], 0, 0 )
	end

end


-- CTF 동맹 아이콘 설정
function LargeMap_SetAllianceIcon( nMapID )

	g_tbLargeMap_CTFAllacne = g_Minimap:GetAllianceInfoList()
	
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_CTF_ALLIANCE, "map" )
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_CTF_ALLIANCE, "map" )
	
	for i = 1, g_nLargeMap_Alliance_MaxNum, 1 do
	
		local tbAlliance = g_tbLargeMap_CTFAllacne[i]
		
		local strAllanceID = "Alliance"..tostring( i )
		local strParam = CreateParam( g_strMap_LAYER_CTF_ALLIANCE, strAllanceID, "_iconCTFAlliance" )
		
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		if	tbAlliance == nil then

			strParam = g_strMap_LAYER_CTF_ALLIANCE .. ";" .. strAllanceID .. ";" .. "0" .. ";"
			CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
			
		else
		
			local tbPosUI = g_Minimap:PositionToMap( nMapID, tbAlliance[ "PosX" ], tbAlliance[ "PosY" ], tbAlliance[ "PosZ" ] )
						
			if tbPosUI ~= nil then
				LargeMap_MoveIcon( g_strMap_LAYER_CTF_ALLIANCE, strAllanceID, tbPosUI[ "x" ], tbPosUI[ "y" ], 0, 0 )
			end
			
		end
	
	end
	
end

function LargeMap_UpdateAllanceIcon()

	if g_nLargeMap_SelectMapID == -1 then
		return 
	end
	
	g_tbLargeMap_CTFAllacne = g_Minimap:GetAllianceInfoList()
		
	for i = 1, g_nLargeMap_Alliance_MaxNum, 1 do
	
		local tbAlliance = g_tbLargeMap_CTFAllacne[i]
		
		local strAllanceID = "Alliance"..tostring( i )
		local strParam = CreateParam( g_strMap_LAYER_CTF_ALLIANCE, strAllanceID, "_iconCTFAlliance" )
				
		if	tbAlliance == nil then
		
			strParam = g_strMap_LAYER_CTF_ALLIANCE .. ";" .. strAllanceID .. ";" .. "0" .. ";"
			CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
			
		else
		
		
			local tbPosUI = g_Minimap:PositionToMap( g_nLargeMap_SelectMapID, tbAlliance[ "PosX" ], tbAlliance[ "PosY" ], tbAlliance[ "PosZ" ] )
			
			if tbPosUI ~= nil then
				
						
				LargeMap_MoveIcon( g_strMap_LAYER_CTF_ALLIANCE, strAllanceID, tbPosUI[ "x" ], tbPosUI[ "y" ], 0, 0 )
			end
			
			strParam = g_strMap_LAYER_CTF_ALLIANCE .. ";" .. strAllanceID .. ";" .. "1" .. ";"
			CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
			
		end
	
	end
	
end


-- CTF 단말기 아이콘 설정
function LargeMap_SetAuthenticatorIcon( nMapID )

	g_tbLargeMap_CTFAuthenticator = g_Minimap:GetAuthenticatorInfo( nMapID )

	-- 기존 게이트 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_CTF, "map" )
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_CTF, "map" )
	
	for value in pairs( g_tbLargeMap_CTFAuthenticator ) do
	
		local tbAuth = g_tbLargeMap_CTFAuthenticator[ value ]
		local strAuthactorID = "authenticator"..tostring( value )
		
		local strParam = CreateParam( g_strMap_LAYER_CTF, strAuthactorID, "_iconCTFCaptureInfo" )
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		strParam = CreateParam( g_strMap_LAYER_CTF, strAuthactorID, tbAuth[ "emSchool" ] + 1 )
		CallFunction( WIDGET_ID_LARGEMAP, "gotoAndStopIcon", strParam, "map" )
		
		-- 아이콘 이동
		LargeMap_MoveIcon( g_strMap_LAYER_CTF, strAuthactorID, tbAuth[ "PosX" ], tbAuth[ "PosY" ], 0, 0 )
		
	end
	
end

-- 인증기 아이콘 설정
function LargeMap_SetAuth( tbList, nType )

	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_AUTH, "map" )
	for value in pairs( tbList ) do
	
		local tbData = tbList[ value ]
		
		local strIconLinkID = ""
		if g_nAuthType_CTI == nType then
		
			if 0 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTINone"
			
			elseif 1 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTIMine"
			
			elseif 2 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTIOther"
			
			elseif 3 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTIAccelerator"
				
			end
		
		elseif g_nAuthType_Guidance == nType then
		
			if 0 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceNone"
			
			elseif 1 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceMine"
				
			elseif 2 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceNeutrality"
			
			elseif 3 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceOther"
			
			elseif 4 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceAccelerator"
				
			end
		
		end
	
		local strIconID = "iconAuth" .. tostring( value )
		local strParam = CreateParam( g_strMap_LAYER_AUTH, strIconID, strIconLinkID ) --.. ";;;;;;"
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		-- 아이콘 이동
		LargeMap_MoveIcon( g_strMap_LAYER_AUTH, strIconID, tbData[ 2 ], tbData[ 3 ], 0, 0 )
	
	end

end

-- 난사군도 인증기/보급품 아이콘 설정
function LargeMap_SetCTIAuth( tbList )

	-- 기존 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_AUTH, "map" )
	
	if false == IsOpenLargeMap() then
	
		g_nLargeMap_AuthType = g_nAuthType_CTI
		g_tbCTIAuthInfo_CachingData = tbList
		
		return
		
	end
	
	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelect, "dwID", "comboMapList" )
	if g_MyChar:GetCurrentMapID() ~= nMapID then
		--return
	end
	
	LargeMap_SetAuth( tbList, g_nAuthType_CTI )

end

-- 선도전 인증기, 가속기 아이콘 설정
function LargeMap_SetGuidanceAuth( tbList )

	-- 기존 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_AUTH, "map" )
	
	if false == IsOpenLargeMap() then
	
		g_nLargeMap_AuthType = g_nAuthType_Guidance
		g_tbCTIAuthInfo_CachingData = tbList
		
		return
		
	end
	
	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelect, "dwID", "comboMapList" )
	if g_MyChar:GetCurrentMapID() ~= nMapID then
		return
	end
	
	LargeMap_SetAuth( tbList, g_nAuthType_Guidance )

end

-- 우편함 아이콘 설정
function LargeMap_SetPostIcon( nMapID )

	-- 기존 우편함 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_POST, "map" )
	-- 우편함 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_POST, "map" )
	
	-- 현재 우편함 정보를 가져와 아이콘 추가
	g_tbLargeMap_Post = g_Minimap:GetMapPost( nMapID )
	for value in pairs( g_tbLargeMap_Post ) do
	
		local tbPostData = g_tbLargeMap_Post[ value ]
		
		local strPostID = "post" .. tostring( value )
		local strParam = g_strMap_LAYER_POST .. ";" .. strPostID .. ";" .. 
			"_iconPost" .. ";" .. tbPostData[ "strName" ].. ";" ..
			tostring(tbPostData[ "CrowType" ]) .. ";" ..
			"-1" .. ";" ..
			tostring(tbPostData[ "ID" ]) .. ";" ..
			tostring(nMapID) .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		-- 아이콘 이동
		LargeMap_MoveIcon( g_strMap_LAYER_POST, strPostID, tbPostData[ "x" ], tbPostData[ "y" ], 0, 0 )
	
	end

end

-- NPC 아이콘 설정
function LargeMap_SetNpcIcon( nMapID )

	-- 기존 NPC 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", g_strMap_LAYER_NPC, "map" )
	-- NPC 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", g_strMap_LAYER_NPC, "map" )
	
	-- 현재 NPC 정보를 가져와 아이콘 추가
	g_tbLargeMap_Npc = g_Minimap:GetMapNormalNpc( nMapID )
	for value in pairs( g_tbLargeMap_Npc ) do

		local tbNpcData = g_tbLargeMap_Npc[ value ]
		
		local strIconID = "npc" .. tostring( value )
		local strParam = g_strMap_LAYER_NPC .. ";" .. strIconID .. ";"
		
		-- 퀘스트 타입 정보가 있다면 퀘스트 NPC
		if nil ~= tbNpcData[ "nQuestType" ] then
		
			if MAP_NPC_QUEST_START == tbNpcData[ "nQuestType" ] then
			
				strParam = strParam .. "_iconQuestStart" .. ";"
			
			elseif MAP_NPC_QUEST_STEP == tbNpcData[ "nQuestType" ] then
			
				strParam = strParam .. "_iconQuestStep" .. ";"
				
			else
			
				strParam = strParam .. "_iconQuestFinish" .. ";"
			
			end
		
		-- 마켓 정보가 있고 마켓 이용 가능하다면 마켓 NPC
		elseif 	nil ~= tbNpcData[ "bMarket" ] and
				true == tbNpcData[ "bMarket" ] then
			
			strParam = strParam .. "_iconMarket" .. ";"
			
		-- 그 외 대화 가능한 NPC는 일반 NPC
		else
		
			strParam = strParam .. "_iconNormal" .. ";"
		
		end
		
		local strstrName =  HTMLFontColor( tbNpcData[ "strName" ], HTML_TEXT_COLOR_WHITE )
		--strParam = strParam .. strstrName ..";"
		strParam = strParam .. tbNpcData[ "strName" ] .. ";" .. 
					tostring(tbNpcData[ "CrowType" ]) .. ";" ..
					"-1" .. ";" ..
					tostring(tbNpcData[ "ID" ]) .. ";" ..
					tostring(nMapID) .. ";"
		-- 위에 마지막으로 들어가는 정보는 툴팁으로 출력이된다, 퀘스트 정보등이 누락되어있는데;
		-- client에서 던져줄때 이미 정보가있다; 여기서 추가면해주면될듯하다;
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		-- 아이콘 이동
		LargeMap_MoveIcon( g_strMap_LAYER_NPC, strIconID, tbNpcData[ "x" ], tbNpcData[ "y" ], 0, 0 )
--		PrintDebug( strParam )
		for k,v in pairs(tbNpcData) do
--			PrintDebug(" KEY:" .. k .. "\tVALUE:" .. tostring(v))
		end
	
	end

end

-- NPC 대화 액션 타입에 따른 아이콘 설정
function LargeMap_SetTalkNpcIcon( nMapID, strLayer, strIcon, strIconClass, nTalkType )

	-- 기존 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_LARGEMAP, "RemoveLayer", strLayer, "map" )
	-- 아이콘 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_LARGEMAP, "AddLayer", strLayer, "map" )
	
	-- 현재 정보를 가져와 아이콘 추가
	local tbNpcTalk = g_Minimap:GetMapTalkNpc( nMapID, nTalkType )
	for value in pairs( tbNpcTalk ) do
	
		local tbNpcTalkData = tbNpcTalk[ value ]
		
		local strIconID = strIcon .. tostring( value )
		local strParam = strLayer .. ";" .. strIconID .. ";" .. 
				strIconClass .. ";" .. tbNpcTalkData[ "strName" ] .. ";" ..
				tostring(tbNpcTalkData[ "CrowType" ]) .. ";" ..
				tostring(nTalkType) .. ";" ..
				tostring(tbNpcTalkData[ "ID" ]) .. ";" ..
				tostring(nMapID) .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		-- 아이콘 이동
		LargeMap_MoveIcon( strLayer, strIconID, tbNpcTalkData[ "x" ], tbNpcTalkData[ "y" ], 0, 0 )
	
	end
	
	return tbNpcTalk

end

-- 내 위치로 맵 이동
function LargeMap_MoveMapMyPos( nOffsetX, nOffsetY )

	-- 맵 이미지 이동
	local nXOffset = (nOffsetX - (GetInt( WIDGET_ID_LARGEMAP, "width", "map" ) / 2)) * -1
	local nYOffset = (nOffsetY - (GetInt( WIDGET_ID_LARGEMAP, "height", "map" ) / 2)) * -1
	
	local strParam = tostring( nXOffset ) .. ";" .. tostring( nYOffset ) .. ";"	
	CallFunction( WIDGET_ID_LARGEMAP, "MoveMap", strParam, "map" )

end

-- 아이콘 위치 이동
function LargeMap_MoveIcon( strLayer, strIcon, nPosX, nPosY, nOffsetX, nOffsetY )
    
	local strParam = strLayer .. ";" .. strIcon .. ";" .. 
		tostring( nPosX + nOffsetX ) .. ";" .. 
		tostring( nPosY + nOffsetY ) .. ";"
	CallFunction( WIDGET_ID_LARGEMAP, "MoveIcon", strParam, "map" )

end

-- 플레이어 아이콘 이동
function LargeMap_SetMyPos()

	if g_nLargeMap_SelectMapID ~= g_MyChar:GetCurrentMapID() then
		return
	end

	if nil == g_tbMap_MapMyPos then
		g_tbMap_MapMyPos = g_MyChar:GetMapPositionOffset()
	end
	
	-- 맵 이미지 이동
	local nXOffset = (g_tbMap_MapMyPos[ "x" ] - (GetInt( WIDGET_ID_LARGEMAP, "width", "map" ) / 2)) * -1
	local nYOffset = (g_tbMap_MapMyPos[ "y" ] - (GetInt( WIDGET_ID_LARGEMAP, "height", "map" ) / 2)) * -1
	
	-- 내 캐릭터 아이콘 이동
	local strParam = tostring( nXOffset ) .. ";" .. tostring( nYOffset ) .. ";"	
	LargeMap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYER,
		g_tbMap_MapMyPos[ "x" ], g_tbMap_MapMyPos[ "y" ], 0, 0 )
		
	-- 방향 아이콘도 같이 따라다녀야 하니 재설정
	LargeMap_SetCameraDir()
	
	-- 이동 목표 지점이 있으면 목표 지점과 충분히 가까워졌을 경우 숨기기
	if nil ~= g_tbMap_MovePos then
	
		-- 두 점 사이 거리 구하기
		local nLength = math.sqrt( math.pow( g_tbMap_MapMyPos[ "x" ] - g_tbMap_MovePos[ "x" ], 2 ) + 
			math.pow( g_tbMap_MapMyPos[ "y" ] - g_tbMap_MovePos[ "y" ], 2 ) )
			
		if 2 > nLength then
		
			local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "0" .. ";"
			CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
			
			g_tbMap_MovePos = nil
		
		end
		
	end

end

-- 카메라 방향 설정
function LargeMap_SetCameraDir()

	if g_nLargeMap_SelectMapID ~= g_MyChar:GetCurrentMapID() then
		return
	end

	if nil == g_tbMap_CameraDir then
		g_tbMap_CameraDir = GetCameraDir()
	end
	
	if nil == g_tbMap_MapMyPos then
		return
	end
	
	-- 캐릭터 위치로 이동
	LargeMap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERDIR, 
		g_tbMap_MapMyPos[ "x" ], g_tbMap_MapMyPos[ "y" ], 0, 0 )
		
	-- 아이콘 회전
	local strParam = CreateParam( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERDIR, g_fMap_CameraThetaY )
	CallFunction( WIDGET_ID_LARGEMAP, "RotateIcon", strParam, "map" )

end

-- 레이어 visible 설정
function LargeMap_SetLayerVisible( strLayer, bVisible )
	
	local strParam = strLayer .. ";"
	if false == bVisible then
		strParam = strParam .. "0"
	else
		strParam = strParam .. "1"
	end
	
	CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleLayer", strParam, "map" )

end

-- 맵 선택
function LargeMap_SelectMap( nMapID )

	-- 아이콘 생성
	LargeMap_CreateIcon( nMapID )
	
	-- 같은 맵 재오픈시 몹정보가 남아있어야한다;
	-- 수정시 g_nLargeMap_SelectMapID 초기화 시점 고려해야한다;
	if g_nLargeMap_SelectMapID ~= nMapID then
		g_bLargeMap_InitMobList = false
	end
	
	g_nLargeMap_SelectMapID = nMapID
	
	local nOffsetX = 0
	local nOffsetY = 0
	if nMapID == g_MyChar:GetCurrentMapID() then
	
		LargeMap_SetMyPos()
	
		nOffsetX = g_tbMap_MapMyPos[ "x" ]
		nOffsetY = g_tbMap_MapMyPos[ "y" ]
		
		-- 현재 위치한 맵과 같은 맵이면 플레이어 레이어 visible 켜기
		LargeMap_SetLayerVisible( g_strMap_LAYER_PLAYER, true )
		
	else
	
		nOffsetX = GetInt( WIDGET_ID_LARGEMAP, "width", "map.map" ) / 2
		nOffsetY = GetInt( WIDGET_ID_LARGEMAP, "height", "map.map" ) / 2
		
		-- 다른 맵이면 플레이어 레이어 visible 끄기
		-- 찾기 대상도 여기에 등록이된다; 
		--LargeMap_SetLayerVisible( g_strMap_LAYER_PLAYER, false )
		
	end
	
	-- 맵 이미지 로드
	LargeMap_LoadMapImg( nMapID )
	-- 맵 위치 이동
	LargeMap_MoveMapMyPos( nOffsetX, nOffsetY )
	
	-- 같은 맵 재오픈시 몹정보가 남아있어야한다;
	-- 수정시 g_nLargeMap_SelectMapID 초기화 시점 고려해야한다;
	-- 선택한 몬스터 초기화
	-- g_tbLargeMap_SelectMob = {}
	
	-- 보상 아이템 초기화
	LargeMap_ClearMobItem()
	
	-- 보상 아이템 설정
	LargeMap_SetMobRewardItem()
	
	-- 표시 설정 패널로 이동
	SetBool( WIDGET_ID_LARGEMAP, "selected", true, "radioCfg" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioInfo" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", false, "radioSearch" )
	LargeMap_SelectPanel( 0 )
	
	LargeMap_SetLevelCondition( g_nLargeMap_SelectMapID )
	
end

-- 마우스 위치에 따른 맵 이미지 위치
function LargeMap_GetMapImgPos( nStageX, nStageY )

	nStageX = math.floor( nStageX )
	nStageY = math.floor( nStageY )

	-- 윈도우 XY
	local nX = GetInt( WIDGET_ID_LARGEMAP, "x", "" )
	local nY = GetInt( WIDGET_ID_LARGEMAP, "y", "" )
	-- 실제 UI XY
	local nContentX = GetInt( WIDGET_ID_LARGEMAP, "x", "_content" )
	local nContentY = GetInt( WIDGET_ID_LARGEMAP, "y", "_content" )
	-- 맵 컴포넌트 XY
	local nMapX = GetInt( WIDGET_ID_LARGEMAP, "x", "map" )
	local nMapY = GetInt( WIDGET_ID_LARGEMAP, "y", "map" )
	-- 맵 이미지 XY
	local nMapDragX = GetInt( WIDGET_ID_LARGEMAP, "x", "map.map" ) * -1
	local nMapDragY = GetInt( WIDGET_ID_LARGEMAP, "y", "map.map" ) * -1
	
	nStageX = nStageX - nX - nContentX - nMapX + nMapDragX
	nStageY = nStageY - nY - nContentY - nMapY + nMapDragY
	
	return nStageX, nStageY

end

-- 몹 정보 리스트 설정
function LargeMap_SetMobInfoList( nMapID )
	
	SetInt(WIDGET_ID_LARGEMAP, "selectedIndex", -1, "panelInfo.listMob")
	g_tbLargeMap_MobNameList = {}
	local tbMobList = g_Minimap:GetMapMobList( nMapID )
						
	local nPlayerLevel = g_MyChar:GetLevel()

	for value in pairs( tbMobList ) do
	
		local tbMob = tbMobList[ value ]
		--[[
		MONSTER_TYPE_QUEST,
		MONSTER_TYPE_BOSS,
		MONSTER_TYPE_VERY_WEAK,
		MONSTER_TYPE_WEAK,
		MONSTER_TYPE_NORMAL,
		MONSTER_TYPE_STRONG
		]]--

		local tbMobTextInfo = LargeMap_GetMobTooltip(tbMob)
		local strTooltip = tbMobTextInfo[1]

		local tbMobName = 
		{
			tbMob[ "strName" ],
			tbMob[ "dwID" ],
			strTooltip,
			tbMobTextInfo[2],
		}
		table.insert( g_tbLargeMap_MobNameList, tbMobName )
		
	end

	if 0 == table.getn( g_tbLargeMap_MobNameList ) then
		
		RemoveData( WIDGET_ID_LARGEMAP, 0, "panelInfo.listMob", true )
		g_MobListCount = 0
		
	else
	
		g_MobListCount = table.getn( g_tbLargeMap_MobNameList )
		local strParam = CreateParam( "label1", "dwID", "TooltipHtmlText", "color1" )
		SetDataEx( WIDGET_ID_LARGEMAP, strParam, g_tbLargeMap_MobNameList, "panelInfo.listMob", false )
		
	end
	
	g_MobList_CheckList = {}
	
	g_bLargeMap_InitMobList = true

end

function LargeMap_GetMobTooltip(tbMob)
	local tbSpecificTextColor = { HTML_TEXT_COLOR_GREEN
								, HTML_TEXT_COLOR_YELLOW
								, HTML_TEXT_COLOR_PINK }
	local tbTextColor = { HTML_TEXT_COLOR_GRAY
						, HTML_TEXT_COLOR_GREEN
						, HTML_TEXT_COLOR_SKYBLUE
						, HTML_TEXT_COLOR_RED }
						
	local tbSpecificTextColor_dw = { TEXT_COLOR_GREEN
								, TEXT_COLOR_YELLOW
								, TEXT_COLOR_PINK }
	local tbTextColor_dw = { TEXT_COLOR_GRAY
						, TEXT_COLOR_GREEN
						, TEXT_COLOR_SKYBLUE
						, TEXT_COLOR_RED }
						
	local nPlayerLevel = g_MyChar:GetLevel()
	
	local nType = tbMob["nType"]
	local nHIWORD, nLOWWORD = HIWORD(nType), LOWORD(nType)
	local dwNameColor
	
	local strTooltip = ""
	-- 퀘스트(1), 보스몹(2)로 체크하나 두가지 모두 해당되는 경우가 있다;
	-- 그럴 경우 퀘스트를 우선으로 처리한다;
	if nHIWORD ~= 0 then
		if (nHIWORD & 1) == 1 then
			strTooltip = HTMLFontColor( GetGameWordText( "MAP_WINDOW_MOB_SPECIFIC_RELATIONTIP", 1 ), tbSpecificTextColor[2] )
			dwNameColor = tbSpecificTextColor_dw[ 2 ]
		else
			local strColor = HTML_TEXT_COLOR_WHITE
			dwNameColor = TEXT_COLOR_WHITE
			if nil ~= tbSpecificTextColor[nHIWORD + 1] then
				strColor = tbSpecificTextColor[nHIWORD + 1]
				dwNameColor = tbSpecificTextColor_dw[nHIWORD + 1]
			end
			
			strTooltip = HTMLFontColor( GetGameWordText( "MAP_WINDOW_MOB_SPECIFIC_RELATIONTIP", nHIWORD ), strColor )
		end
		
	else
		
		local nLvDiff = math.abs(nPlayerLevel - tbMob["nLevel"])
					
		if nLvDiff == 0 then
		
			strTooltip = HTMLFontColor( GetGameWordText( "MAP_WINDOW_MOB_SPECIFIC_RELATIONTIP", 0 ), tbSpecificTextColor[1] )
			dwNameColor = tbSpecificTextColor_dw[ 1 ]
			
		else
			
			strTooltip = HTMLFontColor( GetGameWordText( "MAP_WINDOW_MOB_RELATIONTIP", nLOWWORD ), tbTextColor[ nLOWWORD + 1] )
			if nLOWWORD == 1 or nLOWWORD == 2 then
				strTooltip = StringFormat(strTooltip, nLvDiff)
			end
			dwNameColor = tbTextColor_dw[ nLOWWORD + 1]
			
		end

	end
	
	strTooltip = HTMLFontColor( tbMob[ "strName" ], HTML_TEXT_COLOR_WHITE ) .. "\n" .. strTooltip
	local tbResult = {}
	table.insert( tbResult, strTooltip )
	table.insert( tbResult, dwNameColor )
	return tbResult
end

-- 선택한 몹 추가/제거
function LargeMap_InsertSelectMob( nIndex, bSelect )

	local nID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "dwID", "panelInfo.listMob" )
	-- 체크 했으면 보상 아이템 넣어주기
	if true == bSelect then
	
		-- 이미 ID가 들어있는지 검사
		for value in pairs( g_tbLargeMap_SelectMob ) do
			
			if nID == g_tbLargeMap_SelectMob[ value ] then
				return
			end
		
		end
	
		-- ID 저장
		table.insert( g_tbLargeMap_SelectMob, nID )
		
	-- 체크 해제면 보상 아이템 빼기
	else
	
		-- 같은 ID가 있으면 테이블에서 제거
		for value in pairs( g_tbLargeMap_SelectMob ) do
		
			if nID == g_tbLargeMap_SelectMob[ value ] then
				table.remove( g_tbLargeMap_SelectMob, value )
				break
			end
		
		end
		
	end

end

function LargeMap_ClearMobItem()
	
	g_tbLargeMap_SelectMob = {}
	
	for i = 1, LARGEMAP_MOBREWARD_SLOT_TOTAL, 1 do
	
		local strSlotID = SLOT_LARGEMAP_REWARDITEM_ID .. tostring( i )

		ResetSlot( strSlotID )
				
	end

end

-- 몹 보상 아이템 슬롯 설정
function LargeMap_SetMobRewardItem()
	
	-- 보상 아이템 가져오기
	g_tbLargeMap_RewardItem = g_Minimap:GetMobGenItemList( g_tbLargeMap_SelectMob )
	
	-- 데이터 프로바이더로 변환
	g_tbItemListConvert = {}
	for value in pairs( g_tbLargeMap_RewardItem ) do
	
		local tbItem = g_tbLargeMap_RewardItem[ value ]		
		local tbItemConvert = 
		{
			tbItem[ "MID" ],
			tbItem[ "SID" ],
		}
		table.insert( g_tbItemListConvert, tbItemConvert )
	
	end
	
	-- 리스트 설정
	if 0 == table.getn( g_tbItemListConvert ) then
		RemoveData( WIDGET_ID_LARGEMAP, 0, "panelInfo.exList.list", true )
	else
		SetDataEx( WIDGET_ID_LARGEMAP, "nMID;nSID", g_tbItemListConvert, "panelInfo.exList.list", false )
	end
	
	-- 슬롯 설정
	local nDataCount = table.getn( g_tbItemListConvert )
	
	for i = 1, LARGEMAP_MOBREWARD_SLOT_TOTAL, 1 do
	
		local strSlotID = SLOT_LARGEMAP_REWARDITEM_ID .. tostring( i )
		local nIndex = i - 1
		if nIndex < nDataCount then
			
			local nMID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "nMID", "panelInfo.exList.list" )
			local nSID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "nSID", "panelInfo.exList.list" )
			
			SetSlot_Item( strSlotID, nMID, nSID, 1 )
			
		else
			
			ResetSlot( strSlotID )
			
		end
	
	end
	
	-- 스크롤 위치 맨위로
	SetInt( WIDGET_ID_LARGEMAP, "scrollPosition", 0, "panelInfo.exList.list" )

end

-- 몹 레이어 visible 설정
function LargeMap_SetVisibleMobLayer( nIndex, bVisible )

	local nID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "dwID", "panelInfo.listMob" )
	
	-- 해당 몹 ID 의 레이어가 있는지 확인
	local strLayer = g_tbMap_MobLayer[ nID ]
	if nil == strLayer then
		return
	end
	
	LargeMap_SetLayerVisible( strLayer, bVisible )
	
end

-- 몹 선택/선택 해제
function LargeMap_SelectMob( nIndex, bSelect )
	
	-- 선택한 몹 추가/제거
	LargeMap_InsertSelectMob( nIndex, bSelect )
	-- 몹 레이어 visible 활성/비활성
	LargeMap_SetVisibleMobLayer( nIndex, bSelect )
	-- 보상 아이템 재설정
	LargeMap_SetMobRewardItem()

end

-- 캐릭터 검색
function LargeMap_SearchCharacter()

	local strName = GetString( WIDGET_ID_LARGEMAP, "text", "panelSearch.inputSearch" )
	local bCurMap = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelSearch.checkCurrentMap" )
	
	local nIndex = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local nMapID = -1
	if nIndex ~= -1 then
		GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "dwID", "comboMapList" )
	end
	
	-- 서버에 캐릭터 검색 요청
	g_Msg:ReqSearchCharacter( strName, bCurMap, nMapID )

end

-- 맵 진입 조건 툴팁 생성
function LargeMap_SetLevelCondition( nMapID )

	local tbLevelCondition = g_Minimap:GetLevelCondition( nMapID )
	if 4 ~= table.getn( tbLevelCondition ) then
		return
	end
	
	local strLevel = GetGameWordText( "RN_MAP_WINDOW_CONDITON", 1 )
	
	local strTooltip = string.format( "[%s] %s", 
		GetMapName( nMapID ), 
		GetGameWordText( "RN_MAP_WINDOW_CONDITON", 0 ) )
	
	-- 동일
	if tbLevelCondition[ 1 ] == 0 then
	
		strTooltip = strTooltip .. string.format( "\n%s : %d", 
			strLevel, tbLevelCondition[ 2 ] )

	-- 초과
	elseif tbLevelCondition[ 1 ] == 1 then
	
		strTooltip = strTooltip .. string.format( "\n%s : %d %s", 
			strLevel, tbLevelCondition[ 2 ], GetGameWordText( "RN_MAP_WINDOW_HIGHER", 0 ) )
	
	-- 미만
	elseif tbLevelCondition[ 1 ] == 2 then
	
		strTooltip = strTooltip .. string.format( "\n%s : %d %s", 
			strLevel, tbLevelCondition[ 2 ], GetGameWordText( "RN_MAP_WINDOW_LOW", 0 ) )
	
	-- 이상
	elseif tbLevelCondition[ 1 ] == 3 then
	
		strTooltip = strTooltip .. string.format( "\n%s : %d %s", 
			strLevel, tbLevelCondition[ 2 ], GetGameWordText( "RN_MAP_WINDOW_SAME_HIGHER", 0 ) )
	
	-- 이하
	elseif tbLevelCondition[ 1 ] == 4 then
	
		strTooltip = strTooltip .. string.format( "\n%s : %d %s", 
			strLevel, tbLevelCondition[ 2 ], GetGameWordText( "RN_MAP_WINDOW_SAME_LOW", 0 ) )
		
	-- 항상
	elseif tbLevelCondition[ 1 ] == 5 then

		strTooltip = strTooltip .. string.format( "\n%s : %s", 
			strLevel, GetGameWordText( "RN_MAP_WINDOW_ALWAYS", 0 ) )
	
	-- min ~ max
	elseif tbLevelCondition[ 1 ] == 6 then
	
		strTooltip = strTooltip .. string.format( "\n%s : %d - %d", 
			strLevel, tbLevelCondition[ 2 ], tbLevelCondition[ 3 ] )
		
	end
	
	-- 퀘스트 조건
	if 0 ~= string.len( tbLevelCondition[ 4 ] ) then
	
		strTooltip = strTooltip .. string.format( "\n%s : %s", 
			GetGameWordText( "RN_MAP_WINDOW_CONDITON", 2 ), tbLevelCondition[ 4 ] )
	
	end
	
	SetString( WIDGET_ID_LARGEMAP, "TooltipText", strTooltip, "comboMapList" )

end

----------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 캐릭터 검색 결과
function LargeMap_SearchCharResult( tbResult )

	SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", -1, "panelSearch.listChar" )

	g_tbLargeMap_SearchCharList = {}
	for value in pairs( tbResult ) do
	
		local tbChar = tbResult[ value ]
		
		local tbCharData = 
		{
			tbChar[ "strName" ],
			tbChar[ "nLevel" ]
		}
		table.insert( g_tbLargeMap_SearchCharList, tbCharData )
	
	end
	
	SetDataEx( WIDGET_ID_LARGEMAP, "label;nLevel", g_tbLargeMap_SearchCharList, "panelSearch.listChar", false )
	
	g_tbLargeMap_SearchChar[ "bSearch" ] = false
	for i = 1, 6, 1 do
		SetString( WIDGET_ID_LARGEMAP, "text", "", "panelSearch.labelInfo" .. tostring( i ) )
	end

end

-- 검색한 캐릭터 세부 정보
function LargeMap_SearchCharDetail( nLevel, nSchool, nClass, dwGaeaID, bParty, strName, strTitle, strClubName )

	g_tbLargeMap_SearchChar[ "bSearch" ] = true

	g_tbLargeMap_SearchChar[ "strSchool" ] = GetGameWordText( "FULL_ACADEMY_NAME", nSchool )
	SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strSchool" ], "panelSearch.labelInfo1" )
	
	g_tbLargeMap_SearchChar[ "strLevel" ] = "Lv." .. tostring( nLevel )
	g_tbLargeMap_SearchChar[ "strName" ] = strName
	SetString( WIDGET_ID_LARGEMAP, "text", 
		g_tbLargeMap_SearchChar[ "strLevel" ] .. " " .. g_tbLargeMap_SearchChar[ "strName" ], "panelSearch.labelInfo2" )
	
	if 0 ~= string.len( strTitle ) then
		g_tbLargeMap_SearchChar[ "strBadge" ] = GetGameInternalText( strTitle, 0 )
	else
		g_tbLargeMap_SearchChar[ "strBadge" ] = ""
	end
	SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strBadge" ], "panelSearch.labelInfo3" )
	
	g_tbLargeMap_SearchChar[ "strCalss" ] = GetCharClassName( nClass )
	SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strCalss" ], "panelSearch.labelInfo4" )
	
	if true == bParty then
		g_tbLargeMap_SearchChar[ "strParty" ] = GetGameWordText( "MAP_HAVE_PARTY", 0 )
	else
		g_tbLargeMap_SearchChar[ "strParty" ] = GetGameWordText( "MAP_HAVE_NO_PARTY", 0 )
	end
	SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strParty" ], "panelSearch.labelInfo5" )
	
	g_tbLargeMap_SearchChar[ "strClub" ] = strClubName
	SetString( WIDGET_ID_LARGEMAP, "text", g_tbLargeMap_SearchChar[ "strClub" ], "panelSearch.labelInfo6" )

end

-- 검색한 캐릭터 지도에 표시
function LargeMap_SearchCharMapPos( tbMapPos )

	if false == g_tbLargeMap_SearchChar[ "bSearch" ] then
		return
	end
	
	g_tbLargeMap_SearchChar[ "nMapID" ] = tbMapPos[ "nMapID" ]
	g_tbLargeMap_SearchChar[ "nPosX" ] = tbMapPos[ "nPosX" ]
	g_tbLargeMap_SearchChar[ "nPosY" ] = tbMapPos[ "nPosY" ]
	
	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local nCurMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelect, "dwID", "comboMapList" )
	
	-- 검색한 캐릭터가 현재 맵과 같다면 아이콘 visible 켜고 위치 이동 시키기
	if nCurMapID ~= g_tbLargeMap_SearchChar[ "nMapID" ] then
	
		nSelect = -1
		
		-- 선택할 인덱스 찾기
		local nDataCount = GetInt( WIDGET_ID_LARGEMAP, "length", "comboMapList.dataProvider" )
		for i = 0, nDataCount - 1, 1 do
		
			local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, i, "dwID", "comboMapList" )
			if nMapID == g_tbLargeMap_SearchChar[ "nMapID" ] then
				nSelect = i
				break
			end
		
		end
		
		-- 맵 리스트 선택
		SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", nSelect, "comboMapList" )
		
		-- 맵 선택
		LargeMap_SelectMap( g_tbLargeMap_SearchChar[ "nMapID" ] )
		--LargeMap_SelectPanel( 0 )
		
		-- 맵 리스트에 없는 맵이면 맵 이름 강제 설정
		if -1 == nSelect then
		
			local strMapName = GetMapName( g_tbLargeMap_SearchChar[ "nMapID" ] )
			SetString( WIDGET_ID_LARGEMAP, "defaultLabel", strMapName, "comboMapList" )
			
		end
	
	end
	
	-- 비공개 맵으로 간주
	if nSelect == -1 then
		SetBool( WIDGET_ID_LARGEMAP, "visible", true, "mcSecretMap" )
	else
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "mcSecretMap" )
	end
	local strTooltip = tostring(g_tbLargeMap_SearchChar["strLevel"]).." "..g_tbLargeMap_SearchChar["strName"]
	local strParam = g_strMap_LAYER_PLAYER .. ";" .. 
				g_strMap_ICON_SEARCHCHAR .. ";" .. 
				strTooltip .. ";"
	CallFunction( WIDGET_ID_LARGEMAP, "SetIconTooltip", strParam, "map" )
	
	local nXOffset = (tbMapPos[ "nPosX" ] - (GetInt( WIDGET_ID_LARGEMAP, "width", "map" ) / 2)) * -1
	local nYOffset = (tbMapPos[ "nPosY" ] - (GetInt( WIDGET_ID_LARGEMAP, "height", "map" ) / 2)) * -1
	
	local strParam = tostring( nXOffset ) .. ";" .. tostring( nYOffset ) .. ";"
	CallFunction( WIDGET_ID_LARGEMAP, "MoveMap", strParam, "map" )

	local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_SEARCHCHAR .. ";" .. "1" .. ";"
	CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
		
	LargeMap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_SEARCHCHAR, 
		g_tbLargeMap_SearchChar[ "nPosX" ], g_tbLargeMap_SearchChar[ "nPosY" ], 0, 0 )

end

-- 전체맵 캐릭터 이동 아이콘 끄기
function LargeMap_HideMoveIcon()

	if false == IsOpenLargeMap() then
		return
	end

	local strParam = CreateParam( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERMOVE, 0 )
	CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )

end

-- 파티 아이콘 업데이트
function LargeMap_UpdatePartyIcon( dwIndex )

	if false == IsOpenLargeMap() then
		return
	end
	
	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	local nSelecteIndex = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	-- 현재 선택한 맵 ID
	local nSelectMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelecteIndex, "dwID", "comboMapList" )
	
	-- 현재 보고 있는 맵에 있는 파티원만 갱신
	if tbMember[ "dwMapID" ] ~= nSelectMapID then
		return
	end
	
	-- 멤버 실제 위치
	local tbPos = tbMember[ "tbPosition" ]
	
	-- 미니맵 위치로 변환
	local tbPosUI = g_Minimap:PositionToMap( tbMember[ "dwMapID" ], tbPos[ "x" ], tbPos[ "y" ], tbPos[ "z" ] )
	
	-- [shhan][2016.10.07] nil  이 나올수 있는 구조임. 그냥 사용하면 crash 발생함.
	if tbPosUI[ "x" ] == nil then
		return
	end
	if tbPosUI[ "y" ] == nil then
		return
	end
		
	-- 파티원 아이콘 이동
	local strIconID = g_strMap_ICON_PARTY .. tostring( dwIndex + 1 )
	LargeMap_MoveIcon( g_strMap_LAYER_PARTY, strIconID, tbPosUI[ "x" ], tbPosUI[ "y" ], 0, 0 )

end

-- 파티 아이콘 추가/삭제
function LargeMap_PartyIconAddOrRemove( dwIndex )

	if false == IsOpenLargeMap() then
		return
	end
	
	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	local nSelecteIndex = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	-- 현재 선택한 맵 ID
	local nSelectMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelecteIndex, "dwID", "comboMapList" )
	
	-- 아이콘 ID
	local strIconID = g_strMap_ICON_PARTY .. tostring( dwIndex + 1 )
	
	-- 현재 보고 있는 맵과 같으면 추가
	if tbMember[ "dwMapID" ] == nSelectMapID then
		
		-- 아이콘 생성
		local strParam = CreateParam( g_strMap_LAYER_PARTY, strIconID, "_iconParty", tbMember[ "strName" ] ) .. ";;;;;;"
		CallFunction( WIDGET_ID_LARGEMAP, "AddIcon", strParam, "map" )
		
		-- 멤버 실제 위치
		local tbPos = tbMember[ "tbPosition" ]
		
		-- 미니맵 위치로 변환
		local tbPosUI = g_Minimap:PositionToMap( tbMember[ "dwMapID" ], tbPos[ "x" ], tbPos[ "y" ], tbPos[ "z" ] )
		
		-- [shhan][2016.10.07] nil  이 나올수 있는 구조임. 그냥 사용하면 crash 발생함.
		if tbPosUI[ "x" ] == nil then
			return
		end
		if tbPosUI[ "y" ] == nil then
			return
		end
			
		-- 파티원 아이콘 이동
		LargeMap_MoveIcon( g_strMap_LAYER_PARTY, strIconID, tbPosUI[ "x" ], tbPosUI[ "y" ], 0, 0 )
		
	-- 다르면 제거
	else
	
		local strParam = CreateParam( g_strMap_LAYER_PARTY, strIconID )
		CallFunction( WIDGET_ID_LARGEMAP, "RemoveIcon", strParam, "map" )
	
	end

end

----------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 맵 리스트 플레이어가 있는 맵
function Ev_LargeMap_btnMyMap()

	if true == g_bMap_SecretMap then
	
		SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", -1, "comboMapList" )
		SetBool( WIDGET_ID_LARGEMAP, "visible", true, "mcSecretMap" )
		
		return
		
	else
	
		SetBool( WIDGET_ID_LARGEMAP, "visible", false, "mcSecretMap" )
		
	end

	local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, g_nLargeMap_MyMapIndex, "dwID", "comboMapList" )
	LargeMap_SelectMap( nMapID )
	--LargeMap_SelectPanel( 0 )
	
	SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", g_nLargeMap_MyMapIndex, "comboMapList" )

end

-- 맵 리스트 이전 맵
function Ev_LargeMap_btnPrev()

	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	if 0 ~= nSelect then
		nSelect = nSelect - 1
	else
		return
	end
	
	SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", nSelect, "comboMapList" )
	Ev_LargeMap_SelectMap( nSelect )

end

-- 맵 리스트 다음 맵
function Ev_LargeMap_btnNext()

	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	if nSelect < g_nLargeMap_MapListTotal - 1 then
		nSelect = nSelect + 1
	else
		return
	end
	
	SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", nSelect, "comboMapList" )
	Ev_LargeMap_SelectMap( nSelect )

end

-- 맵 리스트 선택
function Ev_LargeMap_SelectMap( nIndex )

	if -1 == nIndex then
		return
	end
	
	g_bLargeMap_MobNameSelectAll = false

	SetBool( WIDGET_ID_LARGEMAP, "visible", false, "mcSecretMap" )
	
	local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "dwID", "comboMapList" )
	LargeMap_SelectMap( nMapID )
	--LargeMap_SelectPanel( 0 )

end

-- 좌표 보이기 딜레이 설정
function Ev_LargeMap_ShowPositionDelay( nStageX, nStageY )

	g_nLargeMap_MapMousePosX = nStageX
	g_nLargeMap_MapMousePosY = nStageY
	
	local tbQuickSet = 
	{
		time = 100,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_LargeMap_ShowPosition"
	}
	SetTween( WIDGET_ID_LARGEMAP, tbQuickSet, "", "", "" )

end

function Ev_GateClick(clickT, toMapID)	

	if clickT == 1 then
	
		-- 맵 리스트 설정
		local tbMapListClient = g_Minimap:GetMapList()
		local tbMapList = {}
		local nCount = 0
		local nToMapID = tonumber(toMapID)
		
		for value in pairs( tbMapListClient ) do
			
			local tbMapClient = tbMapListClient[ value ]
			
			-- 현재 내 맵과 ID가 동일하면 인덱스 설정
			if nToMapID == tbMapClient[ "dwID" ] then
	
				Ev_LargeMap_SelectMap(nCount)
				SetInt( WIDGET_ID_LARGEMAP, "selectedIndex", nCount, "comboMapList" )
				
				break
				
			end	
			
			nCount = nCount + 1
			
		end
		
	end
	
end

-- 맵 마우스 올라간 곳의 좌표 보여주기
function Ev_LargeMap_ShowPosition()

	SetBool( WIDGET_ID_LARGEMAP, "visible", true, "mcPos" )

	local nX, nY = LargeMap_GetMapImgPos( g_nLargeMap_MapMousePosX, g_nLargeMap_MapMousePosY )
	
	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local tbPos = g_Minimap:ImageToMap( g_nLargeMap_SelectMapID, nX, nY )
	
	if nil == tbPos[ "x" ] or nil == tbPos[ "y" ] then
		return
	end
	
	if 	tbPos[ "x" ] ~= g_tbLargeMap_PrevMousePos[ 1 ] or
		tbPos[ "y" ] ~= g_tbLargeMap_PrevMousePos[ 2 ] then
		
		local strText = string.format( "%03d  %03d", tbPos[ "x" ], tbPos[ "y" ] )
		SetString( WIDGET_ID_LARGEMAP, "text", strText, "mcPos.labelPosition" )
		
		tbPos[ "x" ] = g_tbLargeMap_PrevMousePos[ 1 ]
		tbPos[ "y" ] = g_tbLargeMap_PrevMousePos[ 2 ]
		
	end
	
end

-- 맵 롤오버
function Ev_LargeMap_MapRollOver( nStageX, nStageY )

	Ev_LargeMap_ShowPositionDelay( nStageX, nStageY )
	
end

-- 맵 롤아웃
function Ev_LargeMap_MapRollOut()

	SetBool( WIDGET_ID_LARGEMAP, "visible", false, "mcPos" )
	
	local tbQuickSet = 
	{
		time = 0,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_LARGEMAP, tbQuickSet, "", "", "" )
	
end

-- 맵 클릭
function Ev_LargeMap_MapClick( nButtonIndex, nStageX, nStageY, bCtrl )

	if 0 ~= nButtonIndex then
		return
	end

	local nX, nY = LargeMap_GetMapImgPos( nStageX, nStageY )
	
	local nSelect = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local nMapID = GetDataInt( WIDGET_ID_LARGEMAP, nSelect, "dwID", "comboMapList" )
	local bVehicle = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkVehicle" )
	
	-- 클릭한 위치로 이동
	if true == g_Msg:MoveToMap( nMapID, nX, nY, bVehicle, bCtrl ) then
	
		g_tbMap_MovePos =
		{
			x = nX,
			y = nY
		}
		
		if g_bMap_ActionVehicle == true then
			g_Msg:ReqSetVehicle( true )
		end
		
		-- 이동 가능하다면 이동 목표지점 아이콘 이동 및 보여주기
		local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "1" .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "SetVisibleIcon", strParam, "map" )
		
		strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. 
			tostring( nX ) .. ";" .. tostring( nY ) .. ";"
		CallFunction( WIDGET_ID_LARGEMAP, "MoveIcon", strParam, "map" )
	
	end

end

-- 설정 패널 선택
function Ev_LargeMap_SelectPanelCfg()
	LargeMap_SelectPanel( 0 )
end

-- 정보 패널 선택
function Ev_LargeMap_SelectPanelInfo()

	local nIndex = GetInt( WIDGET_ID_LARGEMAP, "selectedIndex", "comboMapList" )
	local nSelectMapID = -1
	if nIndex ~= -1 then
		nSelectMapID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "dwID", "comboMapList" )
	end
	
	if false == g_bLargeMap_InitMobList then
	
		LargeMap_SetMobIcon( nSelectMapID )
		LargeMap_SetMobInfoList( nSelectMapID )
		
	end
	
	LargeMap_SelectPanel( 1 )
	
end

-- 검색 패널 선택
function Ev_LargeMap_SelectPanelSearch()
	LargeMap_SelectPanel( 2 )
end

-- 표시 설정
function Ev_LargeMap_cfgAll()

	g_bMap_VisibleAll = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkCfgAll" )	
	
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkGate" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkNpc" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkZoneName" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkBus" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkLocker" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkStartPos" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkClubMan" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkPost" )
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_VisibleAll, "panelCfg.checkParty" )
	
	Ev_LargeMap_cfgGate()
	Ev_LargeMap_cfgNpc()
	Ev_LargeMap_cfgZone()
	Ev_LargeMap_cfgBus()
	Ev_LargeMap_cfgLocker()
	Ev_LargeMap_cfgStartPos()
	Ev_LargeMap_cfgClubMan()
	Ev_LargeMap_cfgPost()
	Ev_LargeMap_cfgParty()

end

-- 게이트 표시
function Ev_LargeMap_cfgGate()

	g_bMap_VisibleGate = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkGate" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_GATE, g_bMap_VisibleGate )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_GATE, g_bMap_VisibleGate )

end

-- 일반 Npc 표시
function Ev_LargeMap_cfgNpc()

	g_bMap_VisibleNpc = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkNpc" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_NPC, g_bMap_VisibleNpc )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_NPC, g_bMap_VisibleNpc )

end

-- 지역명 표시
function Ev_LargeMap_cfgZone()

	g_bMap_VisibleZoneName = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkZoneName" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_TEXT, g_bMap_VisibleZoneName )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_TEXT, g_bMap_VisibleZoneName )

end

-- 버스 표시
function Ev_LargeMap_cfgBus()

	g_bMap_VisibleBus = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkBus" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_BUS, g_bMap_VisibleBus )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_BUS, g_bMap_VisibleBus )

end

-- 라커 표시
function Ev_LargeMap_cfgLocker()

	g_bMap_VisibleLocker = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkLocker" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_LOCKER, g_bMap_VisibleLocker )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_LOCKER, g_bMap_VisibleLocker )

end

-- 시작 위치 표시
function Ev_LargeMap_cfgStartPos()

	g_bMap_VisibleStartPos = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkStartPos" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_STARTPOS, g_bMap_VisibleStartPos )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_STARTPOS, g_bMap_VisibleStartPos )

end

-- 클럽 매니저 표시
function Ev_LargeMap_cfgClubMan()

	g_bMap_VisibleClubMan = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkClubMan" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_CLUBMAN, g_bMap_VisibleClubMan )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_CLUBMAN, g_bMap_VisibleClubMan )

end

-- 우편 표시
function Ev_LargeMap_cfgPost()

	g_bMap_VisiblePost = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkPost" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_POST, g_bMap_VisiblePost )
	
	-- 미니맵도 visible 설정
	Minimap_SetLayerVisible( g_strMap_LAYER_POST, g_bMap_VisiblePost )

end

-- 파티 표시
function Ev_LargeMap_cfgParty()

	g_bMap_VisibleParty = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkParty" )
	LargeMap_SetLayerVisible( g_strMap_LAYER_PARTY, g_bMap_VisibleParty )
	
	-- 미니맵도 visible 설정
	--Minimap_SetLayerVisible( g_strMap_LAYER_PARTY, g_bMap_VisibleParty )

end

-- 동작 설정
function Ev_LargeMap_cfgAction()

	g_bMap_Action = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkAction" )
	
	SetBool( WIDGET_ID_LARGEMAP, "selected", g_bMap_Action, "panelCfg.checkVehicle" )
	
	Ev_LargeMap_cfgVehicle()

end

-- 지도 클릭시 자동 탈것 타기
function Ev_LargeMap_cfgVehicle()

	g_bMap_ActionVehicle = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelCfg.checkVehicle" )
	
end

-- 몬스터 정보 체크되어 있는 항목 모두 체크해제( UnCheckAll 버튼 클릭 )(원용)
function Ev_LargeMap_MobInfoUnCheckAll()

	g_bLargeMap_MobNameSelectAll = not g_bLargeMap_MobNameSelectAll

	for value in pairs( g_tbLargeMap_MobNameList ) do
	
		local nIndex = value - 1
	
		SetIndexDataBool( WIDGET_ID_LARGEMAP, nIndex, "multiSelected", g_bLargeMap_MobNameSelectAll, "panelInfo.listMob" )
		--LargeMap_SelectMob( nIndex, g_bLargeMap_MobNameSelectAll )
		
		-- 선택한 몹 추가/제거
		LargeMap_InsertSelectMob( nIndex, g_bLargeMap_MobNameSelectAll )
		-- 몹 레이어 visible 활성/비활성
		LargeMap_SetVisibleMobLayer( nIndex, g_bLargeMap_MobNameSelectAll )
	
	end
	
	-- 보상 아이템 재설정
	LargeMap_SetMobRewardItem()

end

-- 몬스터 리스트 선택
function Ev_LargeMap_MobSelect( nIndex )

	if -1 == nIndex then
		return
	end
	local nScroll = GetInt( WIDGET_ID_LARGEMAP, "scrollPosition", "panelInfo.listMob" )	
	local nItemIndex = nIndex - nScroll + 1		
	-- EVENT_LIST_ITEM_CLICK 이벤트로 호출된다. 따라서 선택 상태가 아직 플래시에서 적용 안 되어있다. not 연산 해준다.
	local bInsert = not GetBool( WIDGET_ID_LARGEMAP, "selected", "panelInfo.checkMobList" .. tostring( nItemIndex ) )
	
	-- 몬스터 정보보기 체크박스의 활성화 혹은 비활성화 항목 결정(체크한 항목을 활성화)[최대 체크개수 결정용]
	if bInsert == true then
		g_MobList_CheckCount = g_MobList_CheckCount + 1
		g_MobList_CheckList[ nIndex ] = true
	elseif bInsert == false then
		g_MobList_CheckCount = g_MobList_CheckCount - 1
		g_MobList_CheckList[ nIndex ] = false
	end	
	
	-- 몬스터 리스트 체크박스 활성화/비활성화 처리
	Ev_LargeMap_MobList_Enable( nScroll )
	
	LargeMap_SelectMob( nIndex, bInsert )
end
-- 몬스터 리스트 체크박스 활성화/비활성화 처리
function Ev_LargeMap_MobList_Enable( nScroll )
	if true then
		return
	end
	-- 결정된 활성화 항목에 따른 활성화 비활성화 처리(개수가 Max값에 도달하면 나머지 비활성화, 이하로 내려가면 전부 활성화)
	--[[
	if g_MobList_CheckCount >= g_MobList_MaxCheck or g_MobListCount <= g_MobList_CheckCount then
		for i = 1, 8, 1 do
			SetBool( WIDGET_ID_LARGEMAP, "enabled", g_MobList_CheckList[ tostring( i + nScroll - 1 ) ], "panelInfo.checkMobList" .. tostring( i ) )
		end
	elseif g_MobList_CheckCount < g_MobList_MaxCheck then
		local nCount = lua_min(8, g_MobListCount)
		for i = 1, nCount, 1 do
			SetBool( WIDGET_ID_LARGEMAP, "enabled", true, "panelInfo.checkMobList" .. tostring( i ) )
		end
	end
	]]--

end

-- 몬스터 리스트 휠(활성화/비활성화)
function Ev_LargeMap_MobListWheel( )
	local nScroll = GetInt( WIDGET_ID_LARGEMAP, "scrollPosition", "panelInfo.listMob" )
	Ev_LargeMap_MobList_Enable( nScroll )
end

-- 슬롯 마우스 휠 (보상 아이템 리스트 스크롤 되도록 함.)
function Ev_LargeMap_RewardSlotWheel( nDelta )
	
	if 0 <= nDelta then
		nDelta = -1
	else
		nDelta = 1
	end
	
	local nScrollPos = GetInt( WIDGET_ID_LARGEMAP, "scrollPosition", "panelInfo.exList.list" )
	SetInt( WIDGET_ID_LARGEMAP, "scrollPosition", nScrollPos + nDelta, "panelInfo.exList.list" )
	
end

-- 보상 아이템 리스트 스크롤
function Ev_LargeMap_listRewardScroll()

	local nScrollPos = GetInt( WIDGET_ID_LARGEMAP, "scrollPosition", "panelInfo.exList.list" )

	-- 슬롯 설정
	local nDataCount = table.getn( g_tbItemListConvert )
	for i = 1, LARGEMAP_MOBREWARD_SLOT_TOTAL, 1 do
	
		local strSlotID = SLOT_LARGEMAP_REWARDITEM_ID .. tostring( i )
		
		local nIndex = nScrollPos * LARGEMAP_MOBREWARD_SLOT_COL + i - 1
		if nIndex < nDataCount then
			
			local nMID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "nMID", "panelInfo.exList.list" )
			local nSID = GetDataInt( WIDGET_ID_LARGEMAP, nIndex, "nSID", "panelInfo.exList.list" )
			
			SetSlot_Item( strSlotID, nMID, nSID, 1 )
			
		else
		
			ResetSlot( strSlotID )
			
		end
	
	end

end

-- 학원생 검색 입력
function Ev_LargeMap_SearchInputKeyDown( nKeyCode )

	if KEYCODE_ENTER == nKeyCode then
	
		LargeMap_SearchCharacter()
	
	end

end

-- 검색 버튼 클릭
function Ev_LargeMap_SearchBtnClick()

	LargeMap_SearchCharacter()

end

-- 현재 표시 지역에서만 검색
function Ev_LargeMap_CheckCurMap()

	g_bLargeMap_SearchCurMap = GetBool( WIDGET_ID_LARGEMAP, "selected", "panelSearch.checkCurrentMap" )

end

-- 검색한 캐릭터 지도에 표시
function Ev_LargeMap_btnDisplay()

	if  nil == g_tbLargeMap_SearchChar[ "strName" ] or 
		0 == string.len( g_tbLargeMap_SearchChar[ "strName" ] ) then
		return
	end

	if false == g_tbLargeMap_SearchChar[ "bSearch" ] then
		return
	end
	
	g_Msg:ReqSearchCharacterPosition( g_tbLargeMap_SearchChar[ "strName" ] )

end

-- 검색한 캐릭터에게 귓속말
function Ev_LargeMap_btnWhisper()

	if  nil == g_tbLargeMap_SearchChar[ "strName" ] or 
		0 == string.len( g_tbLargeMap_SearchChar[ "strName" ] ) then
		return
	end

	if 	false == g_tbLargeMap_SearchChar[ "bSearch" ] or
		false == IsOpenWidget( WIDGET_ID_BASICCHAT ) then
		return
	end

	-- 귓속말 세팅, 입력창 활성화;
	BasicChat_SetWhisper( g_tbLargeMap_SearchChar[ "strName" ] )
	BasicChat_SetFocus()

end

-- 검색한 캐릭터 친구 등록
function Ev_LargeMap_btnFriend()

	if  nil == g_tbLargeMap_SearchChar[ "strName" ] or 
		0 == string.len( g_tbLargeMap_SearchChar[ "strName" ] ) then
		return
	end

	if false == g_tbLargeMap_SearchChar[ "bSearch" ] then
		return
	end
	
	g_Msg:ReqFriendAdd( g_tbLargeMap_SearchChar["strName"] )
	
end

-- 검색한 캐릭터 정보보기
function Ev_LargeMap_btnInfo()

	if  nil == g_tbLargeMap_SearchChar[ "strName" ] or 
		0 == string.len( g_tbLargeMap_SearchChar[ "strName" ] ) then
		return
	end

	if false == g_tbLargeMap_SearchChar[ "bSearch" ] then
		return
	end
	
	g_Msg:ReqReferChar( g_tbLargeMap_SearchChar[ "strName" ] )
	
end

-- 검색한 캐릭터 파티 신청
function Ev_LargeMap_btnParty()

	if  nil == g_tbLargeMap_SearchChar[ "strName" ] or 
		0 == string.len( g_tbLargeMap_SearchChar[ "strName" ] ) then
		return
	end

	if false == g_tbLargeMap_SearchChar[ "bSearch" ] then
		return
	end

	MsgInviteParty( g_tbLargeMap_SearchChar[ "strName" ] )

end

-- 검색한 캐릭터 클럽 신청
function Ev_LargeMap_btnClub()

	if  nil == g_tbLargeMap_SearchChar[ "strName" ] or 
		0 == string.len( g_tbLargeMap_SearchChar[ "strName" ] ) then
		return
	end

	if false == g_tbLargeMap_SearchChar[ "bSearch" ] then
		return
	end
	MsgInviteClubJoin( g_tbLargeMap_SearchChar[ "strName" ] )
	--[[
	-- 검색한 캐릭터 세부 정보
	g_tbLargeMap_SearchChar = 
	{
		bSerach = false,		-- 검색 되었나?
		nMapID	= -1,			-- 맵 ID
		nPosX = 0,				-- 위치 X
		nPosY = 0,				-- 위치 Y
		
		strSchool 	= "",		-- 학원
		strLevel	= "",		-- 레벨
		strName 	= "",		-- 이름
		strBadge 	= "",		-- 뱃지
		strCalss 	= "",		-- 클래스
		strParty 	= "",		-- 파티
		strClub 	= ""		-- 클럽
	}
	]]
	--!!
end

-- 검색한 캐릭터 리스트 인덱스 변경
function Ev_LargeMap_SearchListIndex( nIndex )

	if -1 == nIndex then
		return
	end
	
	g_nLargeMap_SearchCharSelect = nIndex
	
	local strName = GetDataString( WIDGET_ID_LARGEMAP, nIndex, "label", "panelSearch.listChar" )
	g_Msg:ReqSearchCharacterDetailInfo( strName )

end

----------------------------------------------------------------------------------------
-- 맵 드래그

--  드래그 시작
function Ev_LargeMap_DragMapStart( nButtonIndex, nStageX, nStageY )

	-- 드래그는 오른쪽 마우스로 한다.
	if 1 ~= nButtonIndex and 2 ~= nButtonIndex then
		return
	end
	
	g_nLargeMap_DragMapX = nStageX
	g_nLargeMap_DragMapY = nStageY
	
	InsertStageMouseMove( Ev_LargeMap_DragMapMove )
	InsertStageMouseUp( Ev_LargeMap_DragEnd )

end

-- 드래그 무브
function Ev_LargeMap_DragMapMove( nStageX, nStageY )

	local distX = nStageX - g_nLargeMap_DragMapX
	local distY = nStageY - g_nLargeMap_DragMapY
	
	local strParam = tostring( distX ) .. ";" .. tostring( distY )
	CallFunction( WIDGET_ID_LARGEMAP, "MoveMapDist", strParam, "map" )
	
	g_nLargeMap_DragMapX = nStageX
	g_nLargeMap_DragMapY = nStageY

end

-- 드래그 끝
function Ev_LargeMap_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_LargeMap_DragMapMove )
	RemoveStageMouseUp( Ev_LargeMap_DragEnd )

end

