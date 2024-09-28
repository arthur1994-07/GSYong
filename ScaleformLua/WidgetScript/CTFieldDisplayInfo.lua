
g_nCTField_STATE = 0

function LoadCTFDisplayInfo()

	-- �巡�� ���� �̺�Ʈ
	SetEvent( WIDGET_ID_CTF_DISPLAY_INFO, EVENT_MOUSE_DOWN, "Ev_CTFieldInfo_DragStart", "stageX;stageY", "" )
	
end

function OpenCTFDisplayInfo()

	CTFieldDisplayInfo_SetCTFState( g_nCTField_STATE )
	EventCall_TimeCount_CTFieldDisplayInfo()

end

function EventCall_TimeCount_CTFieldDisplayInfo()

	if IsOpenWidget( WIDGET_ID_CTF_DISPLAY_INFO ) == false then
		return
	end
	
	local CTF_STATE_STOP		= 0
	local CTF_STATE_PROGRESS	= 2
	local CTF_STATE_DONE		= 3
	local CTF_STATE_REWARD		= 4

	g_Client:GetCTFTime( g_nCTField_STATE == CTF_STATE_PROGRESS )

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "EventCall_TimeCount_CTFieldDisplayInfo"
	}
	SetTween( WIDGET_ID_CTF_DISPLAY_INFO, tbQuickSet, "", "", "" )
	
end

function CTFieldDisplayInfo_UpdateAuthenticatorStatus()

	if false == IsOpenWidget( WIDGET_ID_CTF_DISPLAY_INFO ) then
		return
	end
	
	-- ������, �̴ϸʰ� ����� ���� ���������͸� �����Ѵ�; ����ȭ�� �ʿ��ϴ�! ������ �̺�Ʈ ����̴ϱ�..
	local tbLargeMap_CTFAuthenticator = g_Minimap:GetAuthenticatorInfo( g_MyChar:GetCurrentMapID() )
	for value in pairs( tbLargeMap_CTFAuthenticator ) do
	
		local tbAuth = tbLargeMap_CTFAuthenticator[ value ]
		CallFunction( WIDGET_ID_CTF_DISPLAY_INFO, "gotoAndStop", tbAuth[ "emSchool" ] + 1, 
			"mcProgress.Controller_" .. tostring( value ) )
		
		SetString( WIDGET_ID_CTF_DISPLAY_INFO, "text", tbAuth[ "strName" ],
			"mcProgress.Controller_" .. tostring( value ) .. ".AuthControllerLabel" )
		
	end
	
end

function CTFieldDisplayInfo_SetTimeSync( strTime )

	local CTF_STATE_STOP		= 0
	local CTF_STATE_PROGRESS	= 2
	local CTF_STATE_DONE		= 3
	local CTF_STATE_REWARD		= 4

	local strInst = ""
	if g_nCTField_STATE == CTF_STATE_STOP or 
	   g_nCTField_STATE == CTF_STATE_REWARD then

	   strInst = "mcEnd"
	   
	else
	
		strInst = "mcProgress"
	
	end
	SetString( WIDGET_ID_CTF_DISPLAY_INFO, "text", strTime, strInst .. ".TIME_INFO_TEXTFIELD" )

end

-----------------------------------------------------------------------------------------------------
-- Ŭ���̾�Ʈ ��

-- CTF ���� ���� ����, ������¿� ���� UI�� �ٲ�;
function CTFieldDisplayInfo_SetCTFState( nState )

	g_nCTField_STATE = nState
	
	if false == IsOpenWidget(WIDGET_ID_CTF_DISPLAY_INFO) then	
		return
	end
	
	local CTF_STATE_STOP		= 0
	local CTF_STATE_PROGRESS	= 2
	local CTF_STATE_DONE		= 3
	local CTF_STATE_REWARD		= 4
	
	if	g_nCTField_STATE == CTF_STATE_STOP or 
		g_nCTField_STATE == CTF_STATE_REWARD or
		g_nCTField_STATE == CTF_STATE_DONE or
		false == g_Client:IsCTFMap_PVP()  then
		
		SetBool( WIDGET_ID_CTF_DISPLAY_INFO, "visible", false, "mcProgress" )
		SetBool( WIDGET_ID_CTF_DISPLAY_INFO, "visible", true, "mcEnd" )
	   
		local str = GetGameWordText( "INFO_CTF_DISPLAY_TEXT", 0 )
		SetString( WIDGET_ID_CTF_DISPLAY_INFO, "text", str, "mcEnd.PowerController.AuthControllerLabel" )
		
		if false == g_Client:IsCTFMap_PVP() and g_nCTField_STATE == CTF_STATE_PROGRESS then
			
			str = GetGameWordText( "INFO_CTF_DISPLAY_TEXT", 1 )
			SetString( WIDGET_ID_CTF_DISPLAY_INFO, "text", "-", "mcEnd.TIME_INFO_TEXTFIELD" )
			
		else
		
			str = GetGameWordText( "INFO_CTF_DISPLAY_TEXT", 2 )
			
		end
		SetString( WIDGET_ID_CTF_DISPLAY_INFO, "text", str, "mcEnd.PROGRESS_INFO_TEXTFIELD" )
		
		local AuthSchool = g_CTField:GetCTFCaptureSchool()
		if 	AuthSchool ~= 3 and AuthSchool ~= 4 then
				
			CallFunction( WIDGET_ID_CTF_DISPLAY_INFO, "gotoAndStop", 
				AuthSchool + 1, "mcEnd.PowerController" )
				
		else
			
			-- ���� �п� ����.
			CallFunction( WIDGET_ID_CTF_DISPLAY_INFO, "gotoAndStop", "CTF_AUTH_NONE_2", "mcEnd.PowerController" )
				
		end
		
	elseif g_nCTField_STATE == CTF_STATE_PROGRESS and true == g_Client:IsCTFMap_PVP() then
	
		SetBool( WIDGET_ID_CTF_DISPLAY_INFO, "visible", true, "mcProgress" )
		SetBool( WIDGET_ID_CTF_DISPLAY_INFO, "visible", false, "mcEnd" )
	
		local str = GetGameWordText( "INFO_CTF_DISPLAY_TEXT", 1 )
		SetString( WIDGET_ID_CTF_DISPLAY_INFO, "text", str, "mcProgress.PROGRESS_INFO_TEXTFIELD" )
		
	end
	
	CTFieldDisplayInfo_UpdateAuthenticatorStatus()
	
end

-----------------------------------------------------------------------------------------------------
-- �̺�Ʈ

-- �巡�� ���� �̺�Ʈ
function Ev_CTFieldInfo_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_CTFieldInfo_Dragging )
	InsertStageMouseUp( Ev_CTFieldInfo_DragEnd )
	
	g_nCTFInfo_DragMouseX = nStageX
	g_nCTFInfo_DragMouseY = nStageY
	
	g_nCTFInfo_DragPosX = GetInt( WIDGET_ID_CTF_DISPLAY_INFO, "x", "" )
	g_nCTFInfo_DragPosY = GetInt( WIDGET_ID_CTF_DISPLAY_INFO, "y", "" )
	
end

-- �巡�� �̵�
function Ev_CTFieldInfo_Dragging( nStageX, nStageY )

	local nOffsetX = 0
	local nOffsetY = 0
	nOffsetX = nStageX - g_nCTFInfo_DragMouseX
	nOffsetY = nStageY - g_nCTFInfo_DragMouseY
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_CTF_DISPLAY_INFO,
		g_nCTFInfo_DragPosX + nOffsetX, g_nCTFInfo_DragPosY + nOffsetY, 
		0, 0, 0, 0 )
		
	SetInt( WIDGET_ID_CTF_DISPLAY_INFO, "x", nNewX, "" )
	SetInt( WIDGET_ID_CTF_DISPLAY_INFO, "y", nNewY, "" )

end

-- �巡�� ��
function Ev_CTFieldInfo_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_CTFieldInfo_Dragging )
	RemoveStageMouseUp( Ev_CTFieldInfo_DragEnd )

end