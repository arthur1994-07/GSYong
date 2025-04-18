
g_tbCdmStatus_Info = {}

g_nCdmStatus_Hour = 0
g_nCdmStatus_Min = 0
g_nCdmStatus_Sec = 0

g_strCdmStatus_TopRankClubName = ""

g_bCdmStatus_UpdateMyClubIcon = false

-- 드래그 관련 저장 값
g_nCDMStatus_DragMouseX = 0
g_nCDMStatus_DragMouseY = 0
g_nCDMStatus_DragPosX = 0
g_nCDMStatus_DragPosY = 0

function LoadCDMStatusWindow()

	local str = HTMLFontColor( GetGameWordText( "RN_CDM_PROEGSS_TEXT", 0 ), HTML_TEXT_COLOR_YELLOW )
	SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "time.labelTimeTitle" )
	
	-- 현재 1위는 클럽 아이콘 사이즈 키운다.
	SetDouble( WIDGET_ID_CDM_STATUS_WINDOW, "scaleX", 1.2, "info0.rtClub" )
	SetDouble( WIDGET_ID_CDM_STATUS_WINDOW, "scaleY", 1.2, "info0.rtClub" )
	SetDouble( WIDGET_ID_CDM_STATUS_WINDOW, "scaleX", 1.2, "info0.imgClub" )
	SetDouble( WIDGET_ID_CDM_STATUS_WINDOW, "scaleY", 1.2, "info0.imgClub" )
	
	CompetitionCDMStatusBoardListUp( g_tbCdmStatus_Info )
	Ev_CDMStatusWindowTimer()	
	
	g_strCdmStatus_TopRankClubName = ""
	g_bCdmStatus_UpdateMyClubIcon = false
	
	-- 드래그 시작 이벤트
	SetEvent( WIDGET_ID_CDM_STATUS_WINDOW, EVENT_MOUSE_DOWN, "Ev_CDMStatus_DragStart", "stageX;stageY", "" )
	
end

-- 타이머
function Ev_CDMStatusWindowTimer()

	g_nCdmStatus_Sec = g_nCdmStatus_Sec - 1
	if g_nCdmStatus_Sec < 0 then
	
		g_nCdmStatus_Sec = 59
		g_nCdmStatus_Min = g_nCdmStatus_Min - 1
		
		if g_nCdmStatus_Min < 0 then
		
			g_nCdmStatus_Min = 59
			g_nCdmStatus_Hour = g_nCdmStatus_Hour - 1
			
			if g_nCdmStatus_Hour < 0 then
			
				local str = HTMLFontColor( "00 : 00 : 00", HTML_TEXT_COLOR_YELLOW )
				SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "time.labelTime" )	
				return
				
			end
			
		end
		
	end
	
	local str = HTMLFontColor( string.format( "%02d : %02d : %02d", 
		g_nCdmStatus_Hour, g_nCdmStatus_Min, g_nCdmStatus_Sec ),
		HTML_TEXT_COLOR_YELLOW )
	SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "time.labelTime" )	
	
	local tableTimer = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_CDMStatusWindowTimer"
	}
	SetTween( WIDGET_ID_CDM_STATUS_WINDOW, tableTimer, "", "", "" )
	
end

-- 업데이트
function CompetitionCDMStatusBoardUpdate( tbInfo )

	if true == IsOpenWidget( WIDGET_ID_CDM_STATUS_WINDOW ) then
	
		CompetitionCDMStatusBoardListUp( tbInfo )
		
	else
	
		g_tbCdmStatus_Info = tbInfo
		OpenWidget( WIDGET_ID_CDM_STATUS_WINDOW )
		
	end	
	
end

function CompeititonCDMStatusBoardClose()

	CloseWidget( WIDGET_ID_CDM_STATUS_WINDOW )
	
	g_strCdmStatus_TopRankClubName = ""
	g_bCdmStatus_UpdateMyClubIcon = false
	
end

function CompetitionCDMStatusBoardListUp( tbInfo )

	if tbInfo == nil then
		return
	end
	
	local _tableInformation = tbInfo[ 1 ]
	g_nCdmStatus_Hour = _tableInformation[ 1 ]
	g_nCdmStatus_Min = _tableInformation[ 2 ]
	g_nCdmStatus_Sec = _tableInformation[ 3 ]
	
	local _sizeClub = _tableInformation[ 4 ]

	local _scoreCompare = 0
	local _indexTopClub = 0
	local _indexMyClub = 0
	
	for _index = 1, _sizeClub + 1, 1 do
	
		local _tableInformationClub = tbInfo[ _index ]
		if _tableInformationClub == nil then
			continue
		end
		
		local _versionClubMark = _tableInformationClub[ 1 ]
		local _dbNumClub = _tableInformationClub[ 2 ]
		local _scoreClub = _tableInformationClub[ 3 ]
		local _nameClub = _tableInformationClub[ 4 ]
		
		if _scoreClub > _scoreCompare then
		
			_indexTopClub = _index
			_scoreCompare = _scoreClub
			
		end
		
		if g_MyChar:GetClubID() == _dbNumClub then
		
			_indexMyClub = _index
			
		end
		
	end

	if _indexTopClub ~= 0 then
	
		local _tableInformationTopClub = tbInfo[ _indexTopClub ]
		
		-- 랭킹
		local str = GetGameWordText( "RN_CMD_RANK_RANKING_1", 0 )
		SetString( WIDGET_ID_CDM_STATUS_WINDOW, "text", str, "info0.labelRank" )
		
		-- 클럽 이름
		SetString( WIDGET_ID_CDM_STATUS_WINDOW, "text", _tableInformationTopClub[ 4 ], "info0.labelClubName" )
		
		-- 점수
		str = HTMLFontColor( string.format( "%d %s", 
			_tableInformationTopClub[ 3 ], GetGameWordText( "RN_CDM_POINT_TEXT", 0 ) ),
			HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "info0.labelScore" )
		
		local _nameClub = _tableInformationTopClub[ 4 ]
		if g_strCdmStatus_TopRankClubName ~= _nameClub then
		
			local strParam = CreateParam( "ClubIcon_RT_MC", "ClubIcon_RT", 
				g_MyChar.dwServerID, _tableInformationTopClub[ 2 ], 16, 11 )
			UpdateClubMark( WIDGET_ID_CDM_STATUS_WINDOW, strParam, "info0.rtClub" )
			
		end
		
		g_strCdmStatus_TopRankClubName = _nameClub
		
	end
	
	if _indexMyClub ~= 0 then
	
		local _tableInformationMyClub = tbInfo[ _indexMyClub ]
		local _scoreMyClub = _tableInformationMyClub[ 3 ]
		local _rankMyClub = 1
		
		for _index = 1, _sizeClub + 1, 1 do
		
			local _tableInformationClub = tbInfo[ _index ]
			local _scoreClub = _tableInformationClub[ 3 ]
			
			if _scoreClub > _scoreMyClub then
			
				_rankMyClub = _rankMyClub + 1
				
			end
			
		end
		
		-- 랭킹
		local str = HTMLFontColor( StringFormat( "%1% %2%", 
			_rankMyClub, GetGameWordText( "RN_CDM_RANKING_TEXT", 0 ) ),
			HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "info1.labelRank" )
		
		-- 클럽 이름
		str = HTMLFontColor( _tableInformationMyClub[ 4 ], HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "info1.labelClubName" )
		
		-- 점수
		str = HTMLFontColor( string.format( "%d %s", 
			_scoreMyClub, GetGameWordText( "RN_CDM_POINT_TEXT", 0 ) ),
			HTML_TEXT_COLOR_PALEGREEN )
		SetString( WIDGET_ID_CDM_STATUS_WINDOW, "htmlText", str, "info1.labelScore" )
		
		if g_bCdmStatus_UpdateMyClubIcon == false then
		
			local strParam = CreateParam( "ClubIcon_RT_MC", "ClubIcon_RT", 
				g_MyChar.dwServerID, _tableInformationMyClub[ 2 ], 16, 11 )
			UpdateClubMark( WIDGET_ID_CDM_STATUS_WINDOW, strParam, "info1.rtClub" )
			g_bCdmStatus_UpdateMyClubIcon = true
			
		end
		
	end
	
end

----------------------------------------------------------------------------------------
-- 드래그 이벤트

-- 드래그 시작 이벤트
function Ev_CDMStatus_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_CDMStatus_Dragging )
	InsertStageMouseUp( Ev_CDMStatus_DragEnd )
	
	g_nCDMStatus_DragMouseX = nStageX
	g_nCDMStatus_DragMouseY = nStageY
	
	g_nCDMStatus_DragPosX = GetInt( WIDGET_ID_CDM_STATUS_WINDOW, "x", "" )
	g_nCDMStatus_DragPosY = GetInt( WIDGET_ID_CDM_STATUS_WINDOW, "y", "" )

end

-- 드래그 이동
function Ev_CDMStatus_Dragging( nStageX, nStageY )
	
	local nOffsetX = nStageX - g_nCDMStatus_DragMouseX
	local nOffsetY = nStageY - g_nCDMStatus_DragMouseY
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_CDM_STATUS_WINDOW,
		g_nCDMStatus_DragPosX + nOffsetX, g_nCDMStatus_DragPosY + nOffsetY, 
		0, 0, 0, 0 )
		
	SetInt( WIDGET_ID_CDM_STATUS_WINDOW, "x", nNewX, "" )
	SetInt( WIDGET_ID_CDM_STATUS_WINDOW, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_CDMStatus_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_CDMStatus_Dragging )
	RemoveStageMouseUp( Ev_CDMStatus_DragEnd )

end