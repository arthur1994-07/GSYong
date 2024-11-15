
local g_tbNameDisplay = 
{
	g_dwNameDisp_CharAlly,
	g_dwNameDisp_CharEnemy,
	g_dwNameDisp_Mob,
	g_dwNameDisp_NPC,
	g_dwNameDisp_Summon,
	g_dwNameDisp_Item,
}

-- 로드 완료
function LoadNameDisplayConfig()

	local str = ""
	
	-- 타이틀
	str = GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 19 )
	SetString( WIDGET_ID_NAMEDISPLAYCONFIG, "label", str, "titleBtn" )
	
	-- 체크 박스 설정
	for i = 1, table.getn( g_tbNameDisplay ), 1 do
	
		str = tostring( i ) .. ". " ..  GetGameWordText( "NAME_DISPLAY_CONFIG", i - 1 )
		SetString( WIDGET_ID_NAMEDISPLAYCONFIG, "text", str, "check" .. i .. "_text" )
		
		str = GetGameInternalText( "UI_PERFORMANCE_WARNNING", 0 )
		SetString( WIDGET_ID_NAMEDISPLAYCONFIG, "TooltipText", str, "check" .. i .. "_text" )
	
	end

	-- 확인
	str = GetGameWordText( "MODAL_BUTTON", 0 )
	SetString( WIDGET_ID_NAMEDISPLAYCONFIG, "label", str, "btnOK" )
	SetEvent( WIDGET_ID_NAMEDISPLAYCONFIG, EVENT_BUTTON_CLICK, "Ev_NameDisplayConfig_OK", "", "btnOK" )
	
	-- 취소
	str = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_NAMEDISPLAYCONFIG, "label", str, "btnCancel" )
	SetEvent( WIDGET_ID_NAMEDISPLAYCONFIG, EVENT_BUTTON_CLICK, "Ev_NameDisplayConfig_Cancel", "", "btnCancel" )

end

-- 오픈
function OpenNameDisplayConfig()

	local tbGameOption = GetGameOption()
	
	local nNameDisplay = tbGameOption[ "dwNameDisplay" ]
	if nil == nNameDisplay then
		return
	end
	
	for i = 1, table.getn( g_tbNameDisplay ), 1 do
	
		SetBool( WIDGET_ID_NAMEDISPLAYCONFIG, "selected", nNameDisplay & g_tbNameDisplay[ i ], "check" .. i )
	
	end

end

------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인
function Ev_NameDisplayConfig_OK()

	local tbGameOption = GetGameOption()
	
	local nNameDisplay = 0
	for i = 1, table.getn( g_tbNameDisplay ), 1 do
	
		local bSelected = GetBool( WIDGET_ID_NAMEDISPLAYCONFIG, "selected", "check" .. i )
		if true == bSelected then
		
			nNameDisplay = FlagAdd( nNameDisplay, g_tbNameDisplay[ i ] )
		
		end
	
	end

	tbGameOption[ "dwNameDisplay" ] = nNameDisplay
	SetGameOption( tbGameOption )

	CloseWidget( WIDGET_ID_NAMEDISPLAYCONFIG )

end

-- 취소
function Ev_NameDisplayConfig_Cancel()

	CloseWidget( WIDGET_ID_NAMEDISPLAYCONFIG )

end