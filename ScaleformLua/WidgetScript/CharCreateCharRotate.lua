function LoadCharRotate()
	
	-- 캐릭터 회전 버튼 리피트 시간 설정(누르고 있을때 갱신시간)
	SetInt( WIDGET_ID_CHARCREATEROTATE, "repeatInterval", 10, "btnCharRotateL" )
	SetInt( WIDGET_ID_CHARCREATEROTATE, "repeatInterval", 10, "btnCharRotateR" )
	
	SetBool( WIDGET_ID_CHARCREATEROTATE, "autoRepeat", true, "btnCharRotateL" )
	SetBool( WIDGET_ID_CHARCREATEROTATE, "autoRepeat", true, "btnCharRotateR" )
	
	-- 캐릭터 좌회전 버튼 이벤트
	SetEvent( WIDGET_ID_CHARCREATEROTATE, EVENT_BUTTON_CLICK, "Event_CharCreate_btnCharRotateL", "", "btnCharRotateL" )
	-- 캐릭터 우회전 버튼 이벤트
	SetEvent( WIDGET_ID_CHARCREATEROTATE, EVENT_BUTTON_CLICK, "Event_CharCreate_btnCharRotateR", "", "btnCharRotateR" )

	CharCreateRotate_UpdatePos()
	
end

function OpenCharRotate()

	CharCreateRotate_UpdatePos()
	
end

-- 캐릭터 좌회전 버튼 이벤트
function Event_CharCreate_btnCharRotateL()

	g_CharCreate:RotateChar( false, 5.0 )
	
end

-- 캐릭터 우회전 버튼 이벤트
function Event_CharCreate_btnCharRotateR()

	g_CharCreate:RotateChar( true, 5.0 )
	
end

function CharCreateRotate_UpdatePos()

	if false == IsOpenWidget( WIDGET_ID_CHARCREATEROTATE ) then
		return
	end

	local nPosY = GetScreenHeight() - 170
	SetInt( WIDGET_ID_CHARCREATEROTATE, "x", 0, "" )
	SetInt( WIDGET_ID_CHARCREATEROTATE, "y", nPosY, "" )
	
	local nWidth = GetInt( WIDGET_ID_CHARCREATEROTATE, "width", "btnCharRotateL" )
	local tbRect = g_CharCreate:GetCharRect()
	local nLPos = tbRect[ "left" ] - nWidth
	local nRPos = tbRect[ "right" ] + nWidth
	
	SetInt( WIDGET_ID_CHARCREATEROTATE, "x", nLPos, "btnCharRotateL" )
	SetInt( WIDGET_ID_CHARCREATEROTATE, "x", nRPos, "btnCharRotateR" )
	
end