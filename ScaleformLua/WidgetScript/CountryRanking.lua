
RANKDISPLAY_COUNT = 6	-- 랭킹 최대 개수

g_bCountryRanking_IsOpen = false		-- 열렸나?

g_nCountryRanking_Type = -1				-- 타입
g_tbCountryRanking_RankList = nil		-- 랭킹 리스트

----------------------------------------------------
g_nCTIRanking_DragMouseX = 0	-- 드래그 관련 저장 값
g_nCTIRanking_DragMouseY = 0
g_nCTIRanking_DragPosX = 0
g_nCTIRanking_DragPosY = 0
----------------------------------------------------

-- 난사군도 국가 순위
function LoadCountryRanking()

	SetEvent( WIDGET_ID_COUNTRYRANKING, EVENT_WIDGET_CLOSE, "CloseCountryRanking", "", "" )

	-- 순위
	local str = GetGameWordText( "CTI_RANK_WORD", 0 )
	SetString( WIDGET_ID_COUNTRYRANKING, "text", str, "title.labelRank" )
	
	-- 누적 포인트
	str = GetGameWordText( "CTI_RANK_WORD", 2 )
	SetString( WIDGET_ID_COUNTRYRANKING, "text", str, "title.labelScore" )
	
	SetEvent( WIDGET_ID_COUNTRYRANKING, EVENT_MOUSE_ROLLOVER, "Ev_CountryRanking_RollOver", "", "" )
	SetEvent( WIDGET_ID_COUNTRYRANKING, EVENT_MOUSE_ROLLOUT,  "Ev_CountryRanking_RollOut", "", "" )
	
	-- 배경 알파값
	CountryRanking_SetBgAlpha( 0 )

	-- 드래그 시작 이벤트
	SetEvent( WIDGET_ID_COUNTRYRANKING, EVENT_MOUSE_DOWN, "Ev_CTIRanking_DragStart", "stageX;stageY", "" )
end

-- 드래그 시작 이벤트
function Ev_CTIRanking_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_CTIRanking_Dragging )
	InsertStageMouseUp( Ev_CTIRanking_DragEnd )
	
	g_nCTIRanking_DragMouseX = nStageX
	g_nCTIRanking_DragMouseY = nStageY
	
	g_nCTIRanking_DragPosX = GetInt( WIDGET_ID_COUNTRYRANKING, "x", "" )
	g_nCTIRanking_DragPosY = GetInt( WIDGET_ID_COUNTRYRANKING, "y", "" )
	
end

-- 드래그 이동
function Ev_CTIRanking_Dragging( nStageX, nStageY )

	local nOffsetX = 0
	local nOffsetY = 0
	nOffsetX = nStageX - g_nCTIRanking_DragMouseX
	nOffsetY = nStageY - g_nCTIRanking_DragMouseY
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_COUNTRYRANKING,
		g_nCTIRanking_DragPosX + nOffsetX, g_nCTIRanking_DragPosY + nOffsetY, 
		0, 0, 0, 0 )
		
	SetInt( WIDGET_ID_COUNTRYRANKING, "x", nNewX, "" )
	SetInt( WIDGET_ID_COUNTRYRANKING, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_CTIRanking_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_CTIRanking_Dragging )
	RemoveStageMouseUp( Ev_CTIRanking_DragEnd )

end

-- 오픈
function OpenCountryRanking()

	g_bCountryRanking_IsOpen = true

	if nil == g_tbCountryRanking_RankList then
	
		-- 전체 초기화
		for i = 1, RANKDISPLAY_COUNT, 1 do
		
			local strInst = "info" .. tostring( i )
			SetBool( WIDGET_ID_COUNTRYRANKING, "visible", false, strInst )

		end
		
		-- 내꺼도 초기화
		SetString( WIDGET_ID_COUNTRYRANKING, "text", "", "infoMy.labelRank" )
		SetString( WIDGET_ID_COUNTRYRANKING, "text", "", "infoMy.labelScore" )
	
	else
	
		-- 순위 설정
		CountryRanking_UpdateList()
	
	end
	
	-- 타이틀
	CountryRanking_SetTitle()

end

-- 클로즈
function CloseCountryRanking()

	g_bCountryRanking_IsOpen = false
	g_tbCountryRanking_RankList = nil

end

-- 타이틀 이름 설정
function CountryRanking_SetTitle()

	local str = ""
	-- 난사군도
	if 5 == g_nCountryRanking_Type then
	
		-- 국가 이름
		str = GetGameWordText( "CTI_RANK_WORD", 1 )
		
		-- 내 순위 국가 이름 설정
		SetString( WIDGET_ID_COUNTRYRANKING, "text", g_Client:GetSelectedCountryName(), "infoMy.labelName" )
	
	-- 선도전 
	elseif 3 == g_nCountryRanking_Type then
	
		-- 클럽명
		str = GetGameWordText( "RENEWAL_GUIDANCE_RANK_WORD", 1 )
		
		-- 내 클럽 이름 설정
		SetString( WIDGET_ID_COUNTRYRANKING, "text", g_MyChar:GetClubName(), "infoMy.labelName" )
	
	end
	
	SetString( WIDGET_ID_COUNTRYRANKING, "text", str, "title.labelName" )

end

-- 순위 업데이트
function CountryRanking_UpdateList()

	for i = 1, RANKDISPLAY_COUNT, 1 do
	
		local strInst = "info" .. tostring( i )
	
		local tbRank = g_tbCountryRanking_RankList[ i ]
		if nil == tbRank then
		
			SetBool( WIDGET_ID_COUNTRYRANKING, "visible", false, strInst )
		
		else
		
			SetBool( WIDGET_ID_COUNTRYRANKING, "visible", true, strInst )
			
			-- 순위
			local str = tostring( tbRank[ 1 ] )
			SetString( WIDGET_ID_COUNTRYRANKING, "text", str, strInst .. ".labelRank" )
			
			-- 국가 이름
			str = tostring( tbRank[ 2 ] )
			SetString( WIDGET_ID_COUNTRYRANKING, "text", str, strInst .. ".labelName" )
			
			-- 누적 포인트
			str = StringFormat( GetGameWordText( "CTI_RANK_WORD", 4 ), tbRank[ 3 ] )
			SetString( WIDGET_ID_COUNTRYRANKING, "text", str, strInst .. ".labelScore" )
		
		end
	
	end

end

function CountryRanking_SetBgAlpha( fAlpha )

	SetDouble( WIDGET_ID_COUNTRYRANKING, "alpha", fAlpha, "top" )
	SetDouble( WIDGET_ID_COUNTRYRANKING, "alpha", fAlpha, "mid" )
	SetDouble( WIDGET_ID_COUNTRYRANKING, "alpha", fAlpha, "bot" )

end

-------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 순위 업데이트
function CountryRanking_Update( tbRankList, nType )

	g_nCountryRanking_Type = nType
	g_tbCountryRanking_RankList = tbRankList
	
	if false == g_bCountryRanking_IsOpen then
	
		OpenWidget( WIDGET_ID_COUNTRYRANKING )
	
	else
	
		CountryRanking_UpdateList()
		CountryRanking_SetTitle()
	
	end

end

-- 내 순위 업데이트
function CountryRanking_UpdateMy( nRank, nPoint )

	if false == g_bCountryRanking_IsOpen then
		return
	end
	
	-- 순위
	local str = tostring( nRank )
	SetString( WIDGET_ID_COUNTRYRANKING, "text", str, "infoMy.labelRank" )
	
	-- 누적 포인트
	str = StringFormat( GetGameWordText( "CTI_RANK_WORD", 4 ), nPoint )
	SetString( WIDGET_ID_COUNTRYRANKING, "text", str, "infoMy.labelScore" )

end

-------------------------------------------------------------------------------------------------------
-- 이벤트

function Ev_CountryRanking_RollOver()

	CountryRanking_SetBgAlpha( 0.75 )
	
end

function Ev_CountryRanking_RollOut()

	CountryRanking_SetBgAlpha( 0 )
	
end