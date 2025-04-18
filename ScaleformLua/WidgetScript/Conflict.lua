
-- 요청에 의한 UI 출력인지, 요청을 하기위한 UI 출력인지를 구분(요청에 의한 UI일떄 true)
g_bConflict_ReqUI = false

-- 요청시에 보내진 정보를 담는 테이블
g_Tb_Conflict_sOption	= {false, 0, 1.0, 1.0}

g_tbConflict_RecvData = {}

function LoadConflict()

	SetEvent( WIDGET_ID_CONFLICT_REQUEST, EVENT_WIDGET_CLOSE, "CloseConflict" ,"", "" )

	SetEvent( WIDGET_ID_CONFLICT_REQUEST, EVENT_BUTTON_CLICK, "Conflict_Cancel", "", "Conflict.Button_Cancel" )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "label", GetGameWordText( "MODAL_BUTTON", 0 ), "Conflict.Button_OK" )		
	SetString( WIDGET_ID_CONFLICT_REQUEST, "label", GetGameWordText( "MODAL_BUTTON", 1 ), "Conflict.Button_Cancel" )					
	
	local str = GetGameWordText( "CONFT_MODAL_TITLE", 0 )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "title", str, "" )
	
	str = GetGameWordText( "CONFT_MODAL_CONDITION", 1 )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.Text_NumOfRecoverHP" )
	
	str = GetGameWordText( "CONFT_MODAL_CONDITION", 0 )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.CheckBox_AreaLimit_text" )
	
	-- 콤보박스 데이터 설정
	InitRecoverHpNum()
	InitMyHpRate()
	InitEnemyHpRate()
	
end

function OpenConflict()

	local str = StringFormat( g_MyChar:GetConftMessage(), g_strInteractMenu_Name )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.TextArea_Message" )
	
	str = g_MyChar:GetName() .. "\n" .. GetGameWordText( "CONFT_MODAL_CONDITION", 2 )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.TextArea_myHPRate" )
	
	str = g_strInteractMenu_Name .. "\n" .. GetGameWordText("CONFT_MODAL_CONDITION", 2 )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.TextArea_EnermyHPRate" )

	if g_bConflict_ReqUI == false then
	
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", false, "Conflict.Text_Receive_NumOfRecoverHP" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", false, "Conflict.Text_Receive_MyHpRate" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", false, "Conflict.Text_Receive_EnemyHpRate" )
		
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", true, "Conflict.DropDown_NumOfRecoverHP" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", true, "Conflict.DropDown_myHPRate" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", true, "Conflict.DropDown_EnermyHPRate" )
		
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "enabled", true, "Conflict.CheckBox_AreaLimit" )
		
		SetInt( WIDGET_ID_CONFLICT_REQUEST, "selectedIndex", 0, "Conflict.DropDown_NumOfRecoverHP" )
		SetInt( WIDGET_ID_CONFLICT_REQUEST, "selectedIndex", 0, "Conflict.DropDown_myHPRate" )
		SetInt( WIDGET_ID_CONFLICT_REQUEST, "selectedIndex", 0, "Conflict.DropDown_EnermyHPRate" )
	
		SetEvent( WIDGET_ID_CONFLICT_REQUEST, EVENT_BUTTON_CLICK, "ReqConflict", "", "Conflict.Button_OK" )
		
	else
	
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", true, "Conflict.Text_Receive_NumOfRecoverHP" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", true, "Conflict.Text_Receive_MyHpRate" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", true, "Conflict.Text_Receive_EnemyHpRate" )
		
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", false, "Conflict.DropDown_NumOfRecoverHP" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", false, "Conflict.DropDown_myHPRate" )
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "visible", false, "Conflict.DropDown_EnermyHPRate" )
		
		SetBool( WIDGET_ID_CONFLICT_REQUEST, "enabled", false, "Conflict.CheckBox_AreaLimit" )
	
		SetEvent( WIDGET_ID_CONFLICT_REQUEST, EVENT_BUTTON_CLICK, "AcceptConflict", "", "Conflict.Button_OK" )
		Set_ConflictReqData()
		
	end

end

function CloseConflict()

	-- 거절 메시지 요청
	if true == g_bConflict_ReqUI then
		g_Msg:AnswerConflict( false )
	end
	
end

-- 드랍 다운(물약 갯수) 
function InitRecoverHpNum()

	local RecoverHpNum = g_MyChar:GetHPRevoverNum()
	local Size = table.getn( RecoverHpNum )
	local Num = ""
	
	for  index = 1, Size, 1 do	
	
		Num = Num .. tostring( RecoverHpNum[ index ] )
		Num = Num .. ";"
		
	end
	
	SetData( WIDGET_ID_CONFLICT_REQUEST, Num, "Conflict.DropDown_NumOfRecoverHP", false )
	
end

-- 드랍 다운(내 HP 비율) 
function InitMyHpRate()

	local MyHpRate = g_MyChar:GetHPRate_Mine()
	local Size = table.getn( MyHpRate )
	local Num = ""
	
	for index = 1, Size, 1 do
	
		Num = Num .. tostring( MyHpRate[ index ] )
		Num = Num .. ";"
		
	end
	
	SetData( WIDGET_ID_CONFLICT_REQUEST, Num, "Conflict.DropDown_myHPRate", false )
	
end

-- 드랍 다운(상대 HP 비율) 
function InitEnemyHpRate()

	local EnemyHpRate = g_MyChar:GetHPRate_Target()
	local Size = table.getn( EnemyHpRate )
	local Num = ""
	
	for index = 1, Size, 1 do
	
		Num = Num .. tostring( EnemyHpRate[ index ] )
		Num = Num .. ";"
		
	end
	
	SetData( WIDGET_ID_CONFLICT_REQUEST, Num, "Conflict.DropDown_EnermyHPRate", false )
	
end

-- 요청받은 클라이언트 데이터 셋팅용 함수
function Set_ConflictReqData()

	if nil == g_tbConflict_RecvData then
		return
	end
	
	local str = ""
	
	-- 영역 제한 여부
	SetBool( WIDGET_ID_CONFLICT_REQUEST, "selected", g_tbConflict_RecvData[ 1 ], "Conflict.CheckBox_AreaLimit" )
	
	-- 물약 사용개수 입력
	if 65535 == g_tbConflict_RecvData[ 2 ] then
		str = GetGameWordText( "UNLIMITED", 0 )
	else
		str = string.format( "%d", g_tbConflict_RecvData[ 2 ] )
	end
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.Text_Receive_NumOfRecoverHP" )
	
	-- 상대 체력 확대 비율
	str = string.format( "%.2f", g_tbConflict_RecvData[ 3 ] )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.Text_Receive_EnemyHpRate" )
	
	-- 자기 체력 확대 비율
	str = string.format( "%.2f", g_tbConflict_RecvData[ 4 ] )
	SetString( WIDGET_ID_CONFLICT_REQUEST, "text", str, "Conflict.Text_Receive_MyHpRate" )
	
end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 요청받은 클라이언트 콜 값 출력용 함수
function OPEN_ConftUI( Table_sOption )

	g_bConflict_ReqUI = true
	g_tbConflict_RecvData = Table_sOption

	if false == IsOpenWidget( WIDGET_ID_CONFLICT_REQUEST ) then
	
		OpenWidget( WIDGET_ID_CONFLICT_REQUEST )
		
	else
	
		CloseWidget( WIDGET_ID_CONFLICT_REQUEST )
		OpenWidget( WIDGET_ID_CONFLICT_REQUEST )
		
	end
	
end

--------------------------------------------------------------------------------------------------------------
-- 이벤트

--요청 할 때의 동작
function ReqConflict()

	local nSelect = -1
	sOptionIndex = { false, 0, 0, 0 }
	
	nSelect = GetBool( WIDGET_ID_CONFLICT_REQUEST, "selected", "Conflict.CheckBox_AreaLimit" )
	sOptionIndex[ 1 ] = nSelect

	nSelect = GetInt( WIDGET_ID_CONFLICT_REQUEST, "selectedIndex", "Conflict.DropDown_NumOfRecoverHP" )
	if -1 ~= nSelect then
		sOptionIndex[ 2 ] = nSelect	
	end
	
	nSelect = GetInt( WIDGET_ID_CONFLICT_REQUEST, "selectedIndex", "Conflict.DropDown_myHPRate" )
	if -1 ~= nSelect then
		sOptionIndex[ 3 ] = nSelect
	end
	
	nSelect = GetInt( WIDGET_ID_CONFLICT_REQUEST, "selectedIndex", "Conflict.DropDown_EnermyHPRate" )
	if -1 ~= nSelect then
		sOptionIndex[ 4 ] = nSelect
	end
	
	g_Msg:RequestConflict( sOptionIndex )
	
	g_bConflict_ReqUI = false
	CloseWidget( WIDGET_ID_CONFLICT_REQUEST )
	
end

function AcceptConflict()

	g_Msg:AnswerConflict( true )
	
	g_bConflict_ReqUI = false	
	CloseWidget( WIDGET_ID_CONFLICT_REQUEST )
	
end

function Conflict_Cancel()

	if g_bConflict_ReqUI == true then
	
		g_Msg:AnswerConflict( false )
		
	end

	g_bConflict_ReqUI = false
	CloseWidget( WIDGET_ID_CONFLICT_REQUEST )
	
end