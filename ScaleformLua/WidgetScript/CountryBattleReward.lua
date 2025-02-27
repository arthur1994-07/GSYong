
-- 리스트 개수
COUNTRYBATTLEREWARD_LIST_COUNT = 10

-- 타이틀 데이터
g_tbCountryBattleReward_Title = {}
-- 리스트 데이터
g_tbCountryBattleReward_List = {}
-- 나가기 시간
g_nCountryBattleReward_OutTime = 0

-- 난사군도 전투 결과 로드
function LoadCountryBattleReward()

	SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", false, "closeBtn" )

	-- 나가기
	local str = GetGameWordText( "COMPETITION_CTF_BATTLE_TEXT", 2 )
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "label", str, "btnExit" )
	SetEvent( WIDGET_ID_COUNTRYBATTLEREWARD, EVENT_BUTTON_CLICK, "Ev_CountryBattleReward_Exit", "", "btnExit" )

end

-- 오픈
function OpenCountryBattleReward()
	
	-- 타이틀 데이터 설정
	CountryBattleReward_SetTitle()

	-- 리스트 데이터 설정
	CountryBattleReward_SetList()
	
	-- 나가기 시간 설정
	CountryBattleReward_SetExitTime()

end

-- 타이틀 데이터 설정
function CountryBattleReward_SetTitle()

	-- 순위
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", g_tbCountryBattleReward_Title[ 1 ], "labelRank" )
	-- 국기
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", g_tbCountryBattleReward_Title[ 2 ], "labelFlag" )
	-- 국가명
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", g_tbCountryBattleReward_Title[ 3 ], "labelName" )
	-- 획득 포인트
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", g_tbCountryBattleReward_Title[ 4 ], "labelPoint" )
	-- 보유 인증기
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", g_tbCountryBattleReward_Title[ 5 ], "labelAccelerator" )
	-- 인증 성공 횟수
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", g_tbCountryBattleReward_Title[ 6 ], "labelCount" )

end

-- 리스트 데이터 설정
function CountryBattleReward_SetList()

	local tbListData = {}

	for i = 1, COUNTRYBATTLEREWARD_LIST_COUNT, 1 do
	
		local strInst = "item" .. tostring( i )
	
		local tb = g_tbCountryBattleReward_List[ i ]
		if nil == tb then
		
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", false, strInst .. ".mcCountryImg" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", false, strInst .. ".tfName" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", false, strInst .. ".tfPoint" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", false, strInst .. ".tfAccelerator" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", false, strInst .. ".tfCount" )
		
		else
		
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", true, strInst .. ".mcCountryImg" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", true, strInst .. ".tfName" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", true, strInst .. ".tfPoint" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", true, strInst .. ".tfAccelerator" )
			SetBool( WIDGET_ID_COUNTRYBATTLEREWARD, "visible", true, strInst .. ".tfCount" )
			
			-- 리스트에 넣을 데이터
			local tbData = 
			{
				tostring( tb[ 1 ] ),	-- id
				tostring( tb[ 2 ] ),	-- 순위
			}
			table.insert( tbListData, tbData )
			
			-- 국기 이미지 설정 (국가 ID가 따로 안날아오는 듯? 일단 이름으로 찾자)
			CallFunction( WIDGET_ID_COUNTRYBATTLEREWARD, "gotoAndPlay",
				Display_GetCountryFrameIDByName( tb[ 4 ] ), strInst .. ".mcCountryImg" )
			
			-- 국가명
			SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 4 ], strInst .. ".tfName" )
			-- 획득 포인트
			SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 5 ], strInst .. ".tfPoint" )
			-- 보유 인증기
			SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 6 ], strInst .. ".tfAccelerator" )
			-- 인증 성공 횟수
			SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 7 ], strInst .. ".tfCount" )
			
			-- 내 국가 순위 정보 설정
			if tb[ 4 ] == g_Client:GetSelectedCountryName() then
			
				-- 국기 이미지 설정
				CallFunction( WIDGET_ID_COUNTRYBATTLEREWARD, "gotoAndPlay",
					Display_GetCountryFrameIDByName( tb[ 4 ] ), "mcMyCountryImg" )
			
				-- 순위
				SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 2 ], "labelMyRank" )
				-- 국가명
				SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 4 ], "labelMyName" )
				-- 획득 포인트
				SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 5 ], "labelMyPoint" )
				-- 보유 인증기
				SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 6 ], "labelMyAccelerator" )
				-- 인증 성공 횟수
				SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", tb[ 7 ], "labelMyCount" )
			
			end

		end
	
	end
	
	-- 리스트 설정
	local strPamram = CreateParam( "id", "label" )
	SetDataEx( WIDGET_ID_COUNTRYBATTLEREWARD, strPamram, tbListData, "list", false )

end

-- 나가기 시간 설정
function CountryBattleReward_SetExitTime()

	-- 시간 설정
	local str = GetGameWordText( "CTF_RESULT_TEXT", 0 ) .. " " .. 
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), 
		math.floor( g_nCountryBattleReward_OutTime / 60 ), g_nCountryBattleReward_OutTime % 60 ) .. ">"
		
	SetString( WIDGET_ID_COUNTRYBATTLEREWARD, "text", str, "labelTime" )
	
	-- 타이머 깍기
	g_nCountryBattleReward_OutTime = g_nCountryBattleReward_OutTime - 1
	if 0 > g_nCountryBattleReward_OutTime then
		return
	end
	
	-- 타이머 설정
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "CountryBattleReward_SetExitTime"
	}
	SetTween( WIDGET_ID_COUNTRYBATTLEREWARD, tbQuickSet, "", "", "labelTime" )

end

------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 타이틀 데이터
function CountryBattleReward_RecvTitle( tbData )

	-- 타이틀 데이터 저장
	g_tbCountryBattleReward_Title = tbData
	-- 리스트 데이터 초기화
	g_tbCountryBattleReward_List = {}
	
	if true == IsOpenWidget( WIDGET_ID_COUNTRYBATTLEREWARD ) then
		
		-- 타이틀 설정
		CountryBattleReward_SetTitle()
		
	end

end

-- 리스트 데이터
function CountryBattleReward_RecvList( tbData )

	-- 리스트 데이터 추가
	table.insert( g_tbCountryBattleReward_List, tbData )

end

-- UI 오픈
function CountryBattleReward_Open( fOutTime )

	-- 나가기 시간 저장
	g_nCountryBattleReward_OutTime = fOutTime
	
	if true == IsOpenWidget( WIDGET_ID_COUNTRYBATTLEREWARD ) then
	
		-- 리스트 설정
		CountryBattleReward_SetList()
		
		-- 나가기 시간 설정
		CountryBattleReward_SetExitTime()
		
	else
	
		OpenWidget( WIDGET_ID_COUNTRYBATTLEREWARD )
	
	end

end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 나가기
function Ev_CountryBattleReward_Exit()

	-- 나가기 요청
	g_Msg:InstanceManRequestClose()

end

--------------------
-- 테스트
function CBR_Test()

	local tbData = { "rank", "flag", "name", "point", "accelerator", "count" }
	CountryBattleReward_RecvTitle( tbData )
	
	for i = 1, 5, 1 do
	
		local tb = 
		{
			i,
			i,
			i,
			"test",
			i,
			i,
			i
		}
		table.insert( g_tbCountryBattleReward_List, tb )
	
	end
	
	CountryBattleReward_Open( 30 )

end