
-- 리스트 개수
g_nGuidanceResult_ListCount = 10

-- 선도전 타이틀 정보
g_tbGuidanceResult_Title = {}
-- 선도전 리스트 정보 
g_tbGuidanceResult_Info = {}
-- 나가기 시간
g_nGuidanceResult_OutTime = 0

-- 로드 완료
function LoadGuidanceResult()

	SetEvent( WIDGET_ID_GUIDANCERESULT, EVENT_WIDGET_CLOSE, "CloseGuidanceResult", "", "" )
	SetBool( WIDGET_ID_GUIDANCERESULT, "visible", false, "closeBtn" )

	-- 나가기
	local str = GetGameWordText( "CTF_RESULT_BUTTON", 4 )
	SetString( WIDGET_ID_GUIDANCERESULT, "label", str, "btnExit" )
	SetEvent( WIDGET_ID_GUIDANCERESULT, EVENT_BUTTON_CLICK, "Ev_GuianceResult_Exit", "", "btnExit" )
	
	-- 리스트 스크롤
	SetEvent( WIDGET_ID_GUIDANCERESULT, EVENT_SCROLL, "Ev_GuidanceResult_Scroll", "", "list" )

end

-- 오픈
function OpenGuidanceResult()

	GuidanceResult_SetInfo()
	GuidanceResult_SetTime()

end

-- 클로즈
function CloseGuidanceResult()

	g_tbGuidanceResult_Title = {}
	g_tbGuidanceResult_Info = {}

end

-- 정보 설정
function GuidanceResult_SetInfo()

	for value in pairs( g_tbGuidanceResult_Title ) do
	
		local str = g_tbGuidanceResult_Title[ value ]
		SetString( WIDGET_ID_GUIDANCERESULT, "text", str, "labelTitle" .. value )
	
	end
	
	local nMyClubID = g_MyChar:GetClubID()
	
	local tbListData = {}
	for value in pairs( g_tbGuidanceResult_Info ) do
	
		local tbInfo = g_tbGuidanceResult_Info[ value ]
		
		local tbData = 
		{
			tbInfo[ 1 ],		-- 순위
			tbInfo[ 3 ],		-- 클럽 이름
			tbInfo[ 4 ],		-- 클랍 마스터 이름
			tbInfo[ 5 ],		-- 획득 포인트
			tbInfo[ 6 ],		-- 보유 인증기 수
			tbInfo[ 7 ],		-- 인증 성공 횟수
		}
		table.insert( tbListData, tbData )
		
		-- 내 클럽이면 하단 텍스트 및 클럽 아이콘 설정
		if nMyClubID == tbInfo[ 2 ] then
		
			local str = HTMLFontColor( tostring( tbInfo[ 1 ] ), HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_GUIDANCERESULT, "htmlText", str, "labelMine1" )
			
			str = HTMLFontColor( tbInfo[ 3 ], HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_GUIDANCERESULT, "htmlText", str, "labelMine2" )
			
			str = HTMLFontColor( tbInfo[ 4 ], HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_GUIDANCERESULT, "htmlText", str, "labelMine3" )
			
			str = HTMLFontColor( tostring( tbInfo[ 5 ] ), HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_GUIDANCERESULT, "htmlText", str, "labelMine4" )
			
			str = HTMLFontColor( tostring( tbInfo[ 6 ] ), HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_GUIDANCERESULT, "htmlText", str, "labelMine5" )
			
			str = HTMLFontColor( tostring( tbInfo[ 7 ] ), HTML_TEXT_COLOR_GREEN )
			SetString( WIDGET_ID_GUIDANCERESULT, "htmlText", str, "labelMine6" )
			
			local strParam = CreateParam( "ClubIcon_RT_MC", "ClubIcon_RT", g_MyChar.dwServerID, tbInfo[ 2 ], 16, 11 )
			UpdateClubMark( WIDGET_ID_GUIDANCERESULT, strParam, "rtClubIconMine" )
		
		end
	
	end
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4", "label5", "label6" )
	SetDataEx( WIDGET_ID_GUIDANCERESULT, strParam, tbListData, "list", false )
	SetInt( WIDGET_ID_GUIDANCERESULT, "scrollPosition", 0, "list" )
	
	Ev_GuidanceResult_Scroll()

end

-- 나가기 타이머
function GuidanceResult_SetTime()

	if 0 > g_nGuidanceResult_OutTime then
		return
	end

	local nMin = math.floor( g_nGuidanceResult_OutTime / 60 )
	local nSec = math.floor( g_nGuidanceResult_OutTime % 60 )
	
	local str = string.format( "%s %s>", 
		GetGameWordText( "CTF_RESULT_TEXT", 0 ),
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), nMin, nSec ) )
	SetString( WIDGET_ID_GUIDANCERESULT, "text", str, "labelTime" )
	
	g_nGuidanceResult_OutTime = g_nGuidanceResult_OutTime - 1
	
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "GuidanceResult_SetTime"
	}
	SetTween( WIDGET_ID_GUIDANCERESULT, tbQuickSet, "", "", "labelTime" )

end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 결과 타이틀 정보 설정
function GuidanceResult_SetTitle( tbTitle )

	g_tbGuidanceResult_Title = tbTitle

end

-- 결과 리스트 정보 추가
function GuidanceResult_AddInfo( tbInfo )

	table.insert( g_tbGuidanceResult_Info, tbInfo )

end

-- 결과 오픈
function GuidanceResult_Open( nOutTime )

	g_nGuidanceResult_OutTime = nOutTime

	if false == IsOpenWidget( WIDGET_ID_GUIDANCERESULT ) then
	
		OpenWidget( WIDGET_ID_GUIDANCERESULT )
	
	else
	
		GuidanceResult_SetInfo()
		GuidanceResult_SetTime()
	
	end

end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 나가기 버튼 클릭
function Ev_GuianceResult_Exit()

	-- 나가기 요청
	g_Msg:InstanceManRequestClose()
	
	CloseWidget( WIDGET_ID_GUIDANCERESULT )

end

-- 리스트 스크롤
function Ev_GuidanceResult_Scroll()

	local nScroll = GetInt( WIDGET_ID_GUIDANCERESULT, "scrollPosition", "list" )
	
	-- 클럽 아이콘 설정
	for i = 1, g_nGuidanceResult_ListCount, 1 do
	
		local strInst = "item" .. i
		
		local tbInfo = g_tbGuidanceResult_Info[ i + nScroll ]
		if nil == tbInfo then
		
			SetBool( WIDGET_ID_GUIDANCERESULT, "visible", false, strInst .. ".rtClubIcon" )
		
		else
		
			SetBool( WIDGET_ID_GUIDANCERESULT, "visible", true, strInst .. ".rtClubIcon" )
			
			local strParam = CreateParam( "ClubIcon_RT_MC", "ClubIcon_RT", g_MyChar.dwServerID, tbInfo[ 2 ], 16, 11 )
			UpdateClubMark( WIDGET_ID_GUIDANCERESULT, strParam, strInst .. ".rtClubIcon" )
		
		end
	
	end

end