
-- CompetitionCDMResult.swf

-- 타이틀 개수
g_nCDMResult_TitleCount = 9
-- 리스트 개수
g_nCDMResult_ListCount = 16

-- 타이틀 정보
g_tbCDMResult_Title = {}
-- 리스트 정보
g_tbCDMResult_Info = {}
-- 나가기 시간
g_CDMReward_ClosingTime = 0

-- 로드 완료
function LoadCDMResult()

	SetBool( WIDGET_ID_CDMRESULT, "visible", false, "closeBtn" )

	-- 나가기
	local str = GetGameWordText( "CTF_RESULT_BUTTON", 4 )
	SetString( WIDGET_ID_CDMRESULT, "label", str, "btnOut" )
	SetEvent( WIDGET_ID_CDMRESULT, EVENT_BUTTON_CLICK, "Ev_CDMResult_Out", "", "btnOut" )

	-- 리스트 스크롤
	SetEvent( WIDGET_ID_CDMRESULT, EVENT_SCROLL, "Ev_CDMResult_Scroll", "", "list" )
	
	-- 보상
	HitTestEnable( WIDGET_ID_CDMRESULT, false, "mcReward" )
	for i = 1, g_nCDMResult_ListCount, 1 do
	
		SetEvent( WIDGET_ID_CDMRESULT, EVENT_MOUSE_ROLLOVER, "Ev_CDMResult_RewardRollOver", "", "btnReward" .. i, i )
		SetEvent( WIDGET_ID_CDMRESULT, EVENT_MOUSE_ROLLOUT, "Ev_CDMResult_RewardRollOut", "", "btnReward" .. i )
	
	end
	
	-- 아이템
	str = GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 0 ) .. " :"
	SetString( WIDGET_ID_CDMRESULT, "text", str, "mcReward.label" )
	
	for i = 1, CDMRESULT_REWARD_SLOT_TOTAL, 1 do
	
		SetString( WIDGET_ID_CDMRESULT, "binding", SLOT_CDMRESULT_REWARD .. i, "mcReward.slot" .. i )
	
	end
	
end

-- 오픈
function OpenCDMResult()

	-- 보상 아이템
	SetBool( WIDGET_ID_CDMRESULT, "visible", false, "mcReward" )

	-- 타이틀 설정 ----------------------------
	for i = 1, g_nCDMResult_TitleCount, 1 do
	
		local strTitle = g_tbCDMResult_Title[ i ]
		if nil ~= strTitle then
			SetString( WIDGET_ID_CDMRESULT, "text", strTitle, "tfTitle" .. i )
		end
	
	end
	
	-- 랭킹 리스트 설정 ----------------------------
	local tbListData = {}
	for nKey, tbValue in pairs( g_tbCDMResult_Info ) do
	
		local tbData = 
		{
			tbValue[ 2 ],	-- 랭킹
			tbValue[ 4 ], 	-- 클럽 이름
			tbValue[ 5 ], 	-- 클럽 마스터 이름
			tbValue[ 6 ], 	-- 포인트
			tbValue[ 7 ], 	-- 사살
			tbValue[ 8 ], 	-- 죽음
			tbValue[ 9 ], 	-- 부활
		}
		table.insert( tbListData, tbData )
	
	end
	
	table.sort( g_tbCDMResult_Info, function ( _lV, _rV ) return _lV[2] < _rV[2] end )
	table.sort( tbListData, function ( _lV, _rV ) return _lV[1] < _rV[1] end )
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4", "label5", "label6", "label7" )
	SetDataEx( WIDGET_ID_CDMRESULT, strParam, tbListData, "list", false )
	SetInt( WIDGET_ID_CDMRESULT, "scrollPosition", 0, "list" )
	
	-- 리스트 업데이트
	CDMResult_UpdateList( 0 )

	-- <보상/강제나가기 적용시간> 레이블 설정 
	CDMResult_SetTimeLabel()

end

-- 리스트 스크롤 업데이트
function CDMResult_UpdateList( nScrollPos )

	for i = 1, g_nCDMResult_ListCount, 1 do
	
		local nIndex = nScrollPos + i
		local strItemInst = "item" .. i
		
		local tbInfo = g_tbCDMResult_Info[ nIndex ]
		if nil == tbInfo then
		
			SetBool( WIDGET_ID_CDMRESULT, "visible", false, strItemInst .. ".rtClub" )
			SetBool( WIDGET_ID_CDMRESULT, "visible", false, strItemInst .. ".imgClub" )
			SetBool( WIDGET_ID_CDMRESULT, "visible", false, "btnReward" .. i )
		
		else
		
			SetBool( WIDGET_ID_CDMRESULT, "visible", true, strItemInst .. ".rtClub" )
			SetBool( WIDGET_ID_CDMRESULT, "visible", true, strItemInst .. ".imgClub" )
			SetBool( WIDGET_ID_CDMRESULT, "visible", true, "btnReward" .. i )
			
			-- 클럽 아이콘 설정
			local strParam = CreateParam( "ClubIcon_RT_MC", "ClubIcon_RT", g_MyChar.dwServerID, tbInfo[ 1 ], 16, 11 )
			UpdateClubMark( WIDGET_ID_CDMRESULT, strParam, strItemInst .. ".rtClub" )
		
		end
	
	end
	
end

-- 보상/강제나가기 적용시간
function CDMResult_SetTimeLabel()

	local strResult = GetGameWordText( "CTF_RESULT_TEXT", 0 ) .. " " .. 
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), 0, g_CDMReward_ClosingTime ) .. ">"
	SetString( WIDGET_ID_CDMRESULT, "text", strResult, "labelTime" )
	
	-- 클라이언트 종료시간 타이머 설정, Sec 초단위가 들어옴
	if 0 >= g_CDMReward_ClosingTime then
		return
	else
		g_CDMReward_ClosingTime = g_CDMReward_ClosingTime - 1
	end
	
	-- 타이머 호출
	local tbQuickSet_SetTimeLabel = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "CDMResult_SetTimeLabel"
	} 
	SetTween( WIDGET_ID_CDMRESULT, tbQuickSet_SetTimeLabel, "", "", "" )
	
end

------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 열기
function CDMResult_Open( closetime )

	g_CDMReward_ClosingTime = closetime

	OpenWidget( WIDGET_ID_CDMRESULT )
	
end

-- 닫기
function CDMResult_Close()

	CloseWidget( WIDGET_ID_CDMRESULT )
	
end

-- 타이틀 정보 설정, 기존 랭킹 정보 초기화
function CDMResult_SetTitle( _tableTitle )

	g_tbCDMResult_Title = _tableTitle
	g_tbCDMResult_Info = {}
	
end

-- 랭킹 정보 추가
function CDMResult_AddInfo( _tableInformation )

	table.insert( g_tbCDMResult_Info, _tableInformation )
	
end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 나가기
function Ev_CDMResult_Out()

	g_Msg:InstanceManRequestClose()
	
end

-- 리스트 스크롤
function Ev_CDMResult_Scroll()

	local nScrollPos = GetInt( WIDGET_ID_CDMRESULT, "scrollPosition", "list" )
	CDMResult_UpdateList( nScrollPos )
	
end

-- 보상 버튼 오버
function Ev_CDMResult_RewardRollOver( strIndex )

	local nBtnIndex = tonumber( strIndex )
	local nIndex = nBtnIndex + GetInt( WIDGET_ID_CDMRESULT, "scrollPosition", "list" )
	
	local tbInfo = g_tbCDMResult_Info[ nIndex ]
	if nil == tbInfo then
		return
	end
	
	local tbReward = tbInfo[ 10 ]
	
	-- 보상 패널 설정
	if 0 == table.getn( tbReward ) then
	
		SetBool( WIDGET_ID_CDMRESULT, "visible", false, "mcReward" )
	
	else

		SetBool( WIDGET_ID_CDMRESULT, "visible", true, "mcReward" )
		
		-- 보상 패널 위치
		local nBtnX = GetInt( WIDGET_ID_CDMRESULT, "x", "btnReward" .. nBtnIndex )
		local nBtnY = GetInt( WIDGET_ID_CDMRESULT, "y", "btnReward" .. nBtnIndex )
		SetInt( WIDGET_ID_CDMRESULT, "x", nBtnX + 16, "mcReward" )
		SetInt( WIDGET_ID_CDMRESULT, "y", nBtnY + 16, "mcReward" )
		
		-- 보상 아이템 슬롯 설정
		for i = 1, CDMRESULT_REWARD_SLOT_TOTAL, 1 do
		
			local tbItem = tbReward[ i ]
			if nil == tbItem then
			
				SetBool( WIDGET_ID_CDMRESULT, "visible", false, "mcReward.slot" .. i )
			
			else
			
				SetBool( WIDGET_ID_CDMRESULT, "visible", true, "mcReward.slot" .. i )
				SetSlot_Item( SLOT_CDMRESULT_REWARD .. i, tbItem[ 1 ], tbItem[ 2 ], tbItem[ 3 ] )
			
			end
		
		end
	
	end
	
end

-- 보상 버튼 아웃
function Ev_CDMResult_RewardRollOut()

	SetBool( WIDGET_ID_CDMRESULT, "visible", false, "mcReward" )

end