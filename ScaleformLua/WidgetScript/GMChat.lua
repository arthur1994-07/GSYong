
g_nGMChat_PrintTime = 5000
g_nGMChat_BgOffset = 8
g_nGMChat_BgMaxWidth = 300

g_tbGMChat_MsgQue = {}

-- 로드 완료
function LoadGMChat()

	HitTestEnable( WIDGET_ID_GMCHAT, false, "" )

end

-- 오픈
function OpenGMChat()

	-- 등록된 메시지 출력
	GMChat_Print()

end

-- 등록된 메시지 출력
function GMChat_Print()

	-- 출력할 메시지 없다. UI 끄자.
	if 0 == table.getn( g_tbGMChat_MsgQue ) then
	
		CloseWidget( WIDGET_ID_GMCHAT )
		return
		
	end
	
	-- 메시지 팝
	local strMsg = string.format( "[%s] : %s",  GetGameWordText( "ADMIN_NAME", 0 ), g_tbGMChat_MsgQue[ 1 ] )
	table.remove( g_tbGMChat_MsgQue, 1 )
	
	-- 텍스트 및 텍스트 컬러 설정
	SetWString( WIDGET_ID_GMCHAT, "text", strMsg, "tfMsg" )
	
	-- 배경 사이즈 설정
	local nBgWidth = GetInt( WIDGET_ID_GMCHAT, "textWidth", "tfMsg" ) + g_nGMChat_BgOffset + 14
	local nBgHeight = GetInt( WIDGET_ID_GMCHAT, "textHeight", "tfMsg" ) + g_nGMChat_BgOffset
	
	if g_nGMChat_BgMaxWidth < nBgWidth then
		nBgWidth = g_nGMChat_BgMaxWidth
	end

	SetInt( WIDGET_ID_GMCHAT, "width", nBgWidth, "bg" )	
	SetInt( WIDGET_ID_GMCHAT, "height", nBgHeight, "bg" )
	
	-- 타이머
	local tbQuickSet = 
	{
		time = g_nGMChat_PrintTime,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_GMChat_PrintComplete"
	}
	SetTween( WIDGET_ID_GMCHAT, tbQuickSet, "", "", "" )

end

-- 메시지 등록
function GMChat_Push( strMsg )

	table.insert( g_tbGMChat_MsgQue, strMsg )
	
	if false == IsOpenWidget( WIDGET_ID_GMCHAT ) then
		
		OpenWidget( WIDGET_ID_GMCHAT )
		
	end

end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 출력 시간 끝
function Ev_GMChat_PrintComplete()

	-- 다음 메시지 출력
	GMChat_Print()

end