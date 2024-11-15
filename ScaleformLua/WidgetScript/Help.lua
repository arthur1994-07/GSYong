
g_bHelp_IsOpen = false

-- 도움말 로드 완료
function LoadHelp()

	-- 드래그 시 웹뷰도 같이 이동시키기 위해 드래그 이벤트 추가
	SetEvent( WIDGET_ID_HELP, EVENT_MOUSE_DOWN, "Ev_Help_DragStart", "", "titleBtn" )
	-- 닫기 버튼 클릭
	SetEvent( WIDGET_ID_HELP, EVENT_WIDGET_CLOSE, "CloseHelp", "", "" )

	-- 도움말 웹 뷰 ID 바인딩
	SetInt( WIDGET_ID_HELP, "binding", WEB_VIEW_ID_HELP, "htmlView" )
	
end

-- 도움말 오픈
function OpenHelp()

	g_bHelp_IsOpen = true

	-- 웹 보여주기
	local strParam = CreateParam( 1, g_strHELP_ADDRESS )
	CallFunction( WIDGET_ID_HELP, "SetVisible", strParam, "htmlView" )
	
	Help_MoveWebView()

end

-- 닫기
function CloseHelp()

	g_bHelp_IsOpen = false

	-- 웹 숨기기
	local strParam = CreateParam( 0, g_strHELP_ADDRESS )
	CallFunction( WIDGET_ID_HELP, "SetVisible", strParam, "htmlView" )

	MoveHtmlWebView( WEB_VIEW_ID_HELP, false, GetScreenWidth(), GetScreenHeight(), 0, 0 )

end

-- 웹뷰 이동
function Help_MoveWebView()

	local nX = GetInt( WIDGET_ID_HELP, "x", "" ) + GetInt( WIDGET_ID_HELP, "x", "_content" ) + GetInt( WIDGET_ID_HELP, "x", "htmlView" )
	local nY = GetInt( WIDGET_ID_HELP, "y", "" ) + GetInt( WIDGET_ID_HELP, "y", "_content" ) + GetInt( WIDGET_ID_HELP, "y", "htmlView" )
	local nW = GetInt( WIDGET_ID_HELP, "width", "htmlView" )
	local nH = GetInt( WIDGET_ID_HELP, "height", "htmlView" )
	
	MoveHtmlWebView( WEB_VIEW_ID_HELP, true, nX, nY, nW, nH )

end

----------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- HTML 뷰 위치, 크기
function Help_GetHTMLViewSize()

	if false == g_bHelp_IsOpen then
		return nil
	end

	local tb = 
	{
		GetInt( WIDGET_ID_HELP, "x", "" ) + GetInt( WIDGET_ID_HELP, "x", "_content" ) + GetInt( WIDGET_ID_HELP, "x", "htmlView" ),
		GetInt( WIDGET_ID_HELP, "y", "" ) + GetInt( WIDGET_ID_HELP, "y", "_content" ) + GetInt( WIDGET_ID_HELP, "y", "htmlView" ),
		GetInt( WIDGET_ID_HELP, "width", "htmlView" ),
		GetInt( WIDGET_ID_HELP, "height", "htmlView" ),
	}

	return tb
	
end

----------------------------------------------------------------------------------------------
-- 이벤트

-- 드래그 시작
function Ev_Help_DragStart()

	InsertStageMouseMove( Ev_Help_DragMove )
	InsertStageMouseUp( Ev_Help_DragEnd )

end

-- 드래그 이동
function Ev_Help_DragMove( nStageX, nStageY )

	Help_MoveWebView()

end

-- 드래그 끝
function Ev_Help_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_Help_DragMove )
	RemoveStageMouseUp( Ev_Help_DragEnd )

end