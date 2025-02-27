
-- 진행 정보 테이블
g_tbCountryProgressInfo = {}

----------------------------------------------------
g_nCTIProgressInfo_DragMouseX = 0	-- 드래그 관련 저장 값
g_nCTIProgressInfo_DragMouseY = 0
g_nCTIProgressInfo_DragPosX = 0
g_nCTIProgressInfo_DragPosY = 0
----------------------------------------------------

-- 난사군도 진행 정보 로드
function LoadCountryProgressInfo()

	--HitTestEnable( WIDGET_ID_COUNTRYPROGRESSINFO, false, "" )
	
	SetEvent( WIDGET_ID_COUNTRYPROGRESSINFO, EVENT_MOUSE_ROLLOVER, "Ev_CountryProgressInfo_RollOver", "", "" )
	SetEvent( WIDGET_ID_COUNTRYPROGRESSINFO, EVENT_MOUSE_ROLLOUT, "Ev_CountryProgressInfo_RollOut", "", "" )

	SetDouble( WIDGET_ID_COUNTRYPROGRESSINFO, "alpha", 0.0, "bg" )
	
	-- 드래그 시작 이벤트
	SetEvent( WIDGET_ID_COUNTRYPROGRESSINFO, EVENT_MOUSE_DOWN, "Ev_CTIProgressInfo_DragStart", "stageX;stageY", "" )
end

-- 드래그 시작 이벤트
function Ev_CTIProgressInfo_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_CTIProgressInfo_Dragging )
	InsertStageMouseUp( Ev_CTIProgressInfo_DragEnd )
	
	g_nCTIProgressInfo_DragMouseX = nStageX
	g_nCTIProgressInfo_DragMouseY = nStageY
	
	g_nCTIProgressInfo_DragPosX = GetInt( WIDGET_ID_COUNTRYPROGRESSINFO, "x", "" )
	g_nCTIProgressInfo_DragPosY = GetInt( WIDGET_ID_COUNTRYPROGRESSINFO, "y", "" )
	
end

-- 드래그 이동
function Ev_CTIProgressInfo_Dragging( nStageX, nStageY )

	local nOffsetX = 0
	local nOffsetY = 0
	nOffsetX = nStageX - g_nCTIProgressInfo_DragMouseX
	nOffsetY = nStageY - g_nCTIProgressInfo_DragMouseY
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_COUNTRYPROGRESSINFO,
		g_nCTIProgressInfo_DragPosX + nOffsetX, g_nCTIProgressInfo_DragPosY + nOffsetY, 
		0, 0, 0, 0 )
		
	SetInt( WIDGET_ID_COUNTRYPROGRESSINFO, "x", nNewX, "" )
	SetInt( WIDGET_ID_COUNTRYPROGRESSINFO, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_CTIProgressInfo_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_CTIProgressInfo_Dragging )
	RemoveStageMouseUp( Ev_CTIProgressInfo_DragEnd )

end

-- 난사군도 진행 정보 오픈
function OpenCountryProgressInfo()

	CountryProgressInfo_SetInfo()

end

function Ev_CountryProgressInfo_RollOver()
	-- visible로 해주면 결과는 비슷하지만, 여백에 마우스 오버시 인식하지못한다;
	SetDouble( WIDGET_ID_COUNTRYPROGRESSINFO, "alpha", 0.75, "bg" )
end

function Ev_CountryProgressInfo_RollOut()
	SetDouble( WIDGET_ID_COUNTRYPROGRESSINFO, "alpha", 0.0, "bg" )
end

-- 진행 정보 설정
function CountryProgressInfo_SetInfo()

	-- 타이틀
	local str = ""
	
	-- 난사군도
	if 5 == g_tbCountryProgressInfo[ 5 ] then
	
		str = GetGameWordText( "CTI_INFO_NAME_WORD", g_tbCountryProgressInfo[ 1 ] ) .. " " ..
			GetGameWordText( "CTI_INFO_PROGRESS_WORD", g_tbCountryProgressInfo[ 2 ] )
	
	-- 선도전 
	elseif 3 == g_tbCountryProgressInfo[ 5 ] then
	
		str = GetGameWordText( "RENEWAL_GUIDANCE_INFO_NAME_WORD", g_tbCountryProgressInfo[ 1 ] ) .. " " ..
			GetGameWordText( "RENEWAL_GUIDANCE_INFO_PROGRESS_WORD", g_tbCountryProgressInfo[ 2 ] )
	
	end
	SetString( WIDGET_ID_COUNTRYPROGRESSINFO, "text", str, "labelTitle" )
	
	-- 목표 점수
	str = StringFormat( GetGameWordText( "CTI_INFO_WORD", 0 ), g_tbCountryProgressInfo[ 3 ] )
	SetString( WIDGET_ID_COUNTRYPROGRESSINFO, "text", str, "labelScore" )
	
	-- 남은 시간 타이머
	Ev_CountryProgressInfo_Timer()
	
end

------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 업데이트
function CountryProgressInfo_Update( nXMLIndex, nProgressIndex, nScore, fTime, nType )

	-- 진행 정보 저장
	g_tbCountryProgressInfo = 
	{
		nXMLIndex, nProgressIndex, nScore, fTime, nType
	}
	
	if false == IsOpenWidget( WIDGET_ID_COUNTRYPROGRESSINFO ) then
	
		OpenWidget( WIDGET_ID_COUNTRYPROGRESSINFO )
	
	else
	
		CountryProgressInfo_SetInfo()
	
	end

end

------------------------------------------------------------------------------------------
-- 이벤트

-- 남은 시간 타이머
function Ev_CountryProgressInfo_Timer()

	if nil == g_tbCountryProgressInfo[ 4 ] then
		return
	end

	-- 남은 시간
	local str = StringFormat( GetGameWordText( "CTI_INFO_WORD", 1 ), 
		math.floor( g_tbCountryProgressInfo[ 4 ] / 60 ), 
		g_tbCountryProgressInfo[ 4 ] % 60 )
	SetString( WIDGET_ID_COUNTRYPROGRESSINFO, "text", str, "labelTime" )
	
	-- 1초 깎기
	g_tbCountryProgressInfo[ 4 ] = g_tbCountryProgressInfo[ 4 ] - 1
	
	if 0 > g_tbCountryProgressInfo[ 4 ] then
		return
	end

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_CountryProgressInfo_Timer"
	}
	SetTween( WIDGET_ID_COUNTRYPROGRESSINFO, tbQuickSet, "", "", "labelTime" )

end